// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrCommonTasks.h"
#include "morpheme/mrTaskUtilities.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrBlendOps.h"
#include "morpheme/mrNetworkLogger.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
#define OUTPUT_DEAD_TRANSFORMS (0)

//----------------------------------------------------------------------------------------------------------------------
void blend2TrajectoryDeltaTransform(
  NMP::Vector3*       trajectoryDeltaPos,
  NMP::Quat*          trajectoryDeltaAtt,
  bool*               trajectoryFilteredOut,
  const NMP::Vector3& source0TrajectoryDeltaPos,
  const NMP::Quat&    source0TrajectoryDeltaAtt,
  bool                source0TrajectoryFilteredOut,
  const NMP::Vector3& source1TrajectoryDeltaPos,
  const NMP::Quat&    source1TrajectoryDeltaAtt,
  bool                source1TrajectoryFilteredOut,
  uint32_t            blendMode,
  bool                slerpTrajectoryPosition,
  float               blendWeight)
{
  *trajectoryFilteredOut = false;

  if (source0TrajectoryFilteredOut && source1TrajectoryFilteredOut)
  {
    // Both sources have been filtered out.
    trajectoryDeltaAtt->identity();
    trajectoryDeltaPos->setToZero();
    *trajectoryFilteredOut = true;
  }
  else if (source0TrajectoryFilteredOut)
  {
    // Only source 1 exists.
    *trajectoryDeltaAtt = source1TrajectoryDeltaAtt;
    *trajectoryDeltaPos = source1TrajectoryDeltaPos;
  }
  else if (source1TrajectoryFilteredOut)
  {
    // Only source 0 exists.
    *trajectoryDeltaAtt = source0TrajectoryDeltaAtt;
    *trajectoryDeltaPos = source0TrajectoryDeltaPos;
  }
  else
  {
    // Both sources exist, do the blend.
    float clampedWeighting = NMP::clampValue(blendWeight, 0.0f, 1.0f);

    if (blendMode == BLEND_MODE_ADD_ATT_ADD_POS || blendMode == BLEND_MODE_ADD_ATT_LEAVE_POS)
    {
      BlendOps::additiveBlendQuats(
        trajectoryDeltaAtt,
        &source0TrajectoryDeltaAtt,
        &source1TrajectoryDeltaAtt,
        clampedWeighting);
    }
    else
    {
      BlendOps::interpBlendQuats(
        trajectoryDeltaAtt,
        &source0TrajectoryDeltaAtt,
        &source1TrajectoryDeltaAtt,
        clampedWeighting);
    }

    if (blendMode == BLEND_MODE_ADD_ATT_ADD_POS || blendMode == BLEND_MODE_INTERP_ATT_ADD_POS)
    {
      // Additive blend pos.
      *trajectoryDeltaPos = source0TrajectoryDeltaPos + (source1TrajectoryDeltaPos * clampedWeighting);
    }
    else
    {
      if (slerpTrajectoryPosition)
      {
        trajectoryDeltaPos->slerp(source0TrajectoryDeltaPos, source1TrajectoryDeltaPos, clampedWeighting);
      }
      else
      {
        trajectoryDeltaPos->lerp(source0TrajectoryDeltaPos, source1TrajectoryDeltaPos, clampedWeighting);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void IntegrateTransforms(
  NMP::DataBuffer& transforms,
  NMP::DataBuffer& transformRates,
  float            dt,
  float            rateDamping)
{
  uint32_t buffLength = transforms.getLength();
  NMP_ASSERT(buffLength == transformRates.getLength());
  for (uint32_t j = 0; j < buffLength; ++j)
  {
    if (transforms.hasChannel(j) && transformRates.hasChannel(j))
    {
      NMP::Quat q = *transforms.getChannelQuat(j);
      NMP::Vector3 p = *transforms.getChannelPos(j);

      const NMP::Vector3& vel = *transformRates.getChannelVel(j);
      NMP::Vector3& angVel = *transformRates.getChannelAngVel(j);

      angVel *= expf(-dt * rateDamping);

      p += vel * dt;
      q += (NMP::Quat(angVel.x, angVel.y, angVel.z, 0.0f) * q) * (0.5f * dt);
      q.normalise();

      transforms.setChannelPos(j, p);
      transforms.setChannelQuat(j, q);
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------- 
namespace
{

//----------------------------------------------------------------------------------------------------------------------
// Calculate the delta number of events after a time update and add this to the transition event count.
// Includes fractional differences.
void updateTransitSyncEventsCount(
  AttribDataTransitSyncEvents*          attrPreviousTransitStateIn,
  AttribDataTransitSyncEvents*          attrCurrentTransitStateOut,
  AttribDataUpdateSyncEventPlaybackPos* attrPreviousSyncEventPosIn,
  AttribDataUpdateSyncEventPlaybackPos* attrCurrentSyncEventPosIn,
  EventTrackSync&                       syncEventTrack,
  bool                                  reverseTransition)
{
  float from = attrPreviousSyncEventPosIn->m_absPosAdj.get();
  float to = attrCurrentSyncEventPosIn->m_absPosAdj.get();
  float transitEventsDelta;

  // Calculate the delta number of events.
  if (from <= to)
  {
    transitEventsDelta = to - from;
  }
  else
  {
    float eventCount = (float) syncEventTrack.getNumEvents();
    if (from < eventCount)
      transitEventsDelta = to + (syncEventTrack.getNumEvents() - from);
    else // Cope with our overall synchronisation event count being less than the last frames count.
      transitEventsDelta = 0.0f;
  }

  // Accumulate the transition event count, taking into account transition direction.
  attrCurrentTransitStateOut->m_transitionEventCount = reverseTransition ?
      attrPreviousTransitStateIn->m_transitionEventCount - transitEventsDelta :
      attrPreviousTransitStateIn->m_transitionEventCount + transitEventsDelta;
}

//----------------------------------------------------------------------------------------------------------------------
float updateWeightingFromTransitionEventCount(
  float durationInEvents,
  AttribDataTransitSyncEvents* transitState)
{
  NMP_ASSERT(transitState);
  float weighting;

  //-----------------------
  // Transit starts neither complete or reversed.
  transitState->m_completed = false;
  transitState->m_reversed = false;

  // Update the weighting value given how many events we have crossed since the
  // start of the transition and the required duration of the transition in events.
  if (durationInEvents == 0.0f)
  {
    //-----------------------
    // Duration is Zero length so indicate transition is complete and set blendWeight to 1.0f
    transitState->m_completed = true;
    weighting = 1.0f;
  }
  else
  {
    weighting = transitState->m_transitionEventCount / durationInEvents;
    if (weighting > 1.0f)
    {
      //-----------------------
      // blendWeight value indicates transition is complete set to complete and set blendWeight to 1.0f
      transitState->m_completed = true;
      weighting = 1.0f;
    }
    else
    {
      //if the blend weight has reversed below 0 set the blend weight to -1 to signal a reversed transit
      if (weighting < 0.0f)
      {
        //-----------------------
        //if the blend weight has reversed below 0 signal a reversed transit, transit complete and set blendWeight to 0.0f
        transitState->m_completed = true;
        transitState->m_reversed = true;
        weighting = -1.0f;
      }
      else
      {
        weighting -= 0.5f;
        weighting *= NM_PI;
        NMP_ASSERT(weighting <= NM_PI_OVER_TWO && weighting >= -NM_PI_OVER_TWO);
        weighting = NMP::fastSin(weighting);
        weighting += 1.0f;
        weighting *= 0.5f;
        NMP_ASSERT(weighting <= 1.0f && weighting >= 0.0f);
      }
    }
  }

  return weighting;
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void TaskTransitSyncEventsUpdateTimeViaSyncEventPos(Dispatcher::TaskParameters* parameters)
{
  AttribDataUpdateSyncEventPlaybackPos* attrParentAdjUpdateSyncEventPosIn = parameters->getInputAttrib<AttribDataUpdateSyncEventPlaybackPos>(0, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataPlaybackPos* attrMyTimePosOut = parameters->createOutputAttrib<AttribDataPlaybackPos>(1, ATTRIB_SEMANTIC_TIME_POS);
  AttribDataPlaybackPos* attrMyFractionPosOut = parameters->createOutputAttrib<AttribDataPlaybackPos>(2, ATTRIB_SEMANTIC_FRACTION_POS);
  AttribDataSyncEventTrack* attrMySyncEventTrackIn = parameters->getInputAttrib<AttribDataSyncEventTrack>(3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataTransitSyncEventsDef* attrMyTransitDefIn = parameters->getInputAttrib<AttribDataTransitSyncEventsDef>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  AttribDataUpdateSyncEventPlaybackPos* attrMyPreviousSyncEventPosIn = parameters->getInputAttrib<AttribDataUpdateSyncEventPlaybackPos>(5, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataUpdateSyncEventPlaybackPos* attrMyCurrentSyncEventPosOut = parameters->createOutputAttribReplace<AttribDataUpdateSyncEventPlaybackPos>(5, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataTransitSyncEvents* attrMyCurrentTransitStateOut = parameters->createOutputAttribReplace<AttribDataTransitSyncEvents>(6, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);
  AttribDataBlendWeightsCreateDesc desc(1);
  AttribDataBlendWeights* attrMyBlendWeightsOut = parameters->createOutputAttribReplace<AttribDataBlendWeights>(7, ATTRIB_SEMANTIC_BLEND_WEIGHTS, &desc);

  // If reversible get control param attrib data.
  bool reverseTransition = false;
  AttribDataBool* attrMyReverseTransitionControlParamIn = parameters->getOptionalInputAttrib<AttribDataBool>(8, ATTRIB_SEMANTIC_CP_BOOL);
  if (attrMyTransitDefIn->m_reversible && attrMyReverseTransitionControlParamIn)
    reverseTransition = attrMyReverseTransitionControlParamIn->m_value;

  AttribDataUpdatePlaybackPos* attrMyUpdateTimePosOut = parameters->createOutputAttribReplace<AttribDataUpdatePlaybackPos>(9, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  if (attrParentAdjUpdateSyncEventPosIn->m_isAbs || !attrMyPreviousSyncEventPosIn)
  {
    // Calculate this frames values.
    calcCurrentPlaybackValuesFromParentAbsSyncEventPos(
      attrMySyncEventTrackIn->m_syncEventTrack,
      true,   // Loopable.
      attrParentAdjUpdateSyncEventPosIn,
      attrMyCurrentSyncEventPosOut,
      attrMyFractionPosOut,
      attrMyTimePosOut,
      attrMyUpdateTimePosOut);

    // Set the transition event count to zero.
    attrMyCurrentTransitStateOut->m_transitionEventCount = 0.0f;
  }
  else
  {
    AttribDataUpdateSyncEventPlaybackPos prevSyncEventPos;
    prevSyncEventPos = (*attrMyCurrentSyncEventPosOut);

    AttribDataTransitSyncEvents prevTransitState;
    prevTransitState = (*attrMyCurrentTransitStateOut);

    attrMyPreviousSyncEventPosIn = &prevSyncEventPos;
    AttribDataTransitSyncEvents* const attrMyPreviousTransitStateIn = &prevTransitState;

    // Calculate last frames values. Necessary as sync event track config is likely to have changed.
    calcPreviousPlaybackValuesFromMyAbsSyncEventPos(
      attrMySyncEventTrackIn->m_syncEventTrack,
      true,   // Loopable.
      attrMyPreviousSyncEventPosIn,
      attrMyPreviousSyncEventPosIn,
      attrMyFractionPosOut,
      attrMyTimePosOut);

    // Calculate this frames values.
    calcCurrentPlaybackValuesFromParentDeltaSyncEventPosAdjSpace(
      attrMySyncEventTrackIn->m_syncEventTrack,
      attrParentAdjUpdateSyncEventPosIn,
      attrMyPreviousSyncEventPosIn,
      true,   // Loopable.
      attrMyFractionPosOut,
      attrMyTimePosOut,
      attrMyCurrentSyncEventPosOut,
      attrMyUpdateTimePosOut);

    // Work out how much further through the transition we are.
    updateTransitSyncEventsCount(
      attrMyPreviousTransitStateIn,
      attrMyCurrentTransitStateOut,
      attrMyPreviousSyncEventPosIn,
      attrMyCurrentSyncEventPosOut,
      attrMySyncEventTrackIn->m_syncEventTrack,
      reverseTransition);
  }

  // Update the weighting based on how far through the transition we are.
  float blendWeight = updateWeightingFromTransitionEventCount(
    attrMyTransitDefIn->m_durationInEvents,
    attrMyCurrentTransitStateOut);

  // Set the blend weights
  attrMyBlendWeightsOut->setAllWeights(blendWeight);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskTransitSyncEventsUpdateTimeViaTimePos(Dispatcher::TaskParameters* parameters)
{
  AttribDataUpdatePlaybackPos* attrParentAdjUpdateTimePosIn = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(0, ATTRIB_SEMANTIC_UPDATE_TIME_POS);
  AttribDataPlaybackPos* attrMyTimePosOut = parameters->createOutputAttrib<AttribDataPlaybackPos>(1, ATTRIB_SEMANTIC_TIME_POS);
  AttribDataPlaybackPos* attrMyFractionPosOut = parameters->createOutputAttrib<AttribDataPlaybackPos>(2, ATTRIB_SEMANTIC_FRACTION_POS);
  AttribDataSyncEventTrack* attrMySyncEventTrackIn = parameters->getInputAttrib<AttribDataSyncEventTrack>(3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataTransitSyncEventsDef* attrMyTransitDefIn = parameters->getInputAttrib<AttribDataTransitSyncEventsDef>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  AttribDataUpdateSyncEventPlaybackPos* attrMyPreviousSyncEventPosIn = parameters->getInputAttrib<AttribDataUpdateSyncEventPlaybackPos>(5, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataUpdateSyncEventPlaybackPos* attrMyCurrentSyncEventPosOut = parameters->createOutputAttribReplace<AttribDataUpdateSyncEventPlaybackPos>(5, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataTransitSyncEvents* attrMyCurrentTransitStateOut = parameters->createOutputAttribReplace<AttribDataTransitSyncEvents>(6, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);
  AttribDataBlendWeightsCreateDesc desc(1);
  AttribDataBlendWeights* attrMyBlendWeightsOut = parameters->createOutputAttribReplace<AttribDataBlendWeights>(7, ATTRIB_SEMANTIC_BLEND_WEIGHTS, &desc);

  // If reversible get control param attrib data.
  bool reverseTransition = false;
  AttribDataBool* attrReverseTransitionControlParamIn = parameters->getOptionalInputAttrib<AttribDataBool>(8, ATTRIB_SEMANTIC_CP_BOOL);
  if (attrMyTransitDefIn->m_reversible && attrReverseTransitionControlParamIn)
    reverseTransition = attrReverseTransitionControlParamIn->m_value;

  AttribDataUpdatePlaybackPos* attrMyUpdateTimePosOut = parameters->createOutputAttribReplace<AttribDataUpdatePlaybackPos>(9, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  if (attrParentAdjUpdateTimePosIn->m_isAbs || !attrMyPreviousSyncEventPosIn)
  {
    // Calculate this frames values.
    calcCurrentPlaybackValuesFromParentAbsTimeAdjSpace(
      attrMySyncEventTrackIn->m_syncEventTrack,
      attrParentAdjUpdateTimePosIn,
      true,   // Loopable.
      attrMyFractionPosOut,
      attrMyTimePosOut,
      attrMyCurrentSyncEventPosOut,
      attrMyUpdateTimePosOut);

    // Set the transition event count to zero.
    attrMyCurrentTransitStateOut->m_transitionEventCount = 0.0f;
  }
  else
  {
    AttribDataUpdateSyncEventPlaybackPos prevSyncEventPos;
    prevSyncEventPos = (*attrMyCurrentSyncEventPosOut);

    AttribDataTransitSyncEvents prevTransitState;
    prevTransitState = (*attrMyCurrentTransitStateOut);

    attrMyPreviousSyncEventPosIn = &prevSyncEventPos;
    AttribDataTransitSyncEvents* const attrMyPreviousTransitStateIn = &prevTransitState;

    // Calculate last frames values. Necessary as sync event track config is likely to have changed.
    calcPreviousPlaybackValuesFromMyAbsSyncEventPos(
      attrMySyncEventTrackIn->m_syncEventTrack,
      true,   // Loopable.
      attrMyPreviousSyncEventPosIn,
      attrMyPreviousSyncEventPosIn,
      attrMyFractionPosOut,
      attrMyTimePosOut);

    // Calculate this frames values.
    calcCurrentPlaybackValuesFromParentDeltaTimeAdjSpace(
      attrMySyncEventTrackIn->m_syncEventTrack,
      attrParentAdjUpdateTimePosIn,
      attrMyPreviousSyncEventPosIn,
      true,   // Loopable.
      attrMyFractionPosOut,
      attrMyTimePosOut,
      attrMyCurrentSyncEventPosOut,
      attrMyUpdateTimePosOut);

    // Work out how much further through the transition we are.
    updateTransitSyncEventsCount(
      attrMyPreviousTransitStateIn,
      attrMyCurrentTransitStateOut,
      attrMyPreviousSyncEventPosIn,
      attrMyCurrentSyncEventPosOut,
      attrMySyncEventTrackIn->m_syncEventTrack,
      reverseTransition);
  }

  // Update the weighting based on how far through the transition we are.
  float blendWeight = updateWeightingFromTransitionEventCount(
    attrMyTransitDefIn->m_durationInEvents,
    attrMyCurrentTransitStateOut);

  // Set the blend weights
  attrMyBlendWeightsOut->setAllWeights(blendWeight);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskTransitSyncEventsBlendSyncEventTracks(Dispatcher::TaskParameters* parameters)
{
  AttribDataSyncEventTrack* myOutputSyncEventTrack = parameters->createOutputAttribReplace<AttribDataSyncEventTrack>(0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataSyncEventTrack* sourceSyncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataSyncEventTrack* destSyncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(2, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataBlendWeights* blendWeights = parameters->getOptionalInputAttrib<AttribDataBlendWeights>(3, ATTRIB_SEMANTIC_BLEND_WEIGHTS);

  // Interpolate blend event clips.
  float clampedWeighting = 0.0f;
  if (blendWeights)
  {
    NMP_ASSERT(blendWeights->m_eventsNumWeights == 1);
    clampedWeighting = NMP::clampValue(blendWeights->m_eventsWeights[0], 0.0f, 1.0f);
  }
  myOutputSyncEventTrack->m_syncEventTrack.blendClipTracks(
    &sourceSyncEventTrack->m_syncEventTrack,
    &destSyncEventTrack->m_syncEventTrack,
    clampedWeighting,
    0);
  myOutputSyncEventTrack->m_syncEventTrack.setStartEventIndex(0);

  myOutputSyncEventTrack->m_transitionOffset = (int32_t) sourceSyncEventTrack->m_syncEventTrack.getStartEventIndex();
}

//----------------------------------------------------------------------------------------------------------------------
void TaskTransitSyncEventsPassThroughSyncEventTrack(Dispatcher::TaskParameters* parameters)
{
  AttribDataSyncEventTrack* myOutputSyncEventTrack = parameters->createOutputAttribReplace<AttribDataSyncEventTrack>(0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataSyncEventTrack* destSyncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataTransitSyncEventsDef* myTransitDef = parameters->getInputAttrib<AttribDataTransitSyncEventsDef>(2, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  AttribDataUInt* myDestEventOffset = parameters->getOptionalInputAttrib<AttribDataUInt>(3, ATTRIB_SEMANTIC_CP_UINT);

  if (!myDestEventOffset)
  {
    // This is the first frame so we need to initialise the destination event offset.
    AttribDataUInt* myOutputDestEventOffset = parameters->createOutputAttribReplace<AttribDataUInt>(3, ATTRIB_SEMANTIC_CP_UINT);

    myDestEventOffset = myOutputDestEventOffset;

    if (!myTransitDef->m_usingDestStartEventIndex)
    {
      // Use an event count to offset the blending of the Source with the Destination.
      myDestEventOffset->m_value = myTransitDef->m_destEventTrackOffset;
    }
    else
    {
      myDestEventOffset->m_value = 0;
    }
  }

  myOutputSyncEventTrack->m_syncEventTrack.copyClipTrack(&destSyncEventTrack->m_syncEventTrack, false);
  if (myDestEventOffset)
    myOutputSyncEventTrack->m_syncEventTrack.setStartEventIndex(myDestEventOffset->m_value);
  else
    myOutputSyncEventTrack->m_syncEventTrack.setStartEventIndex(0);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskDeadBlendTrajectory(Dispatcher::TaskParameters* parameters)
{
  AttribDataDeadBlendState* deadBlendState = parameters->getInputAttrib<AttribDataDeadBlendState>(0, ATTRIB_SEMANTIC_DEAD_BLEND_STATE);
  // Dead blend rate.
  AttribDataTransformBuffer* deadBlendRatesAttrib = parameters->getInputAttrib<AttribDataTransformBuffer>(1, ATTRIB_SEMANTIC_TRANSFORM_RATES);
  AttribDataUpdatePlaybackPos* deltaTime = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(2, ATTRIB_SEMANTIC_UPDATE_TIME_POS);
  AttribDataTrajectoryDeltaTransform* destTrajectoryTransform = parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(3, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(4, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 1);
  AttribDataTrajectoryDeltaTransform* outputTransform = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(5, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);

  float dt = deltaTime->m_value;

  NMP::DataBuffer* deadBlendRates = deadBlendRatesAttrib->m_transformBuffer;
  if (deadBlendRates->hasChannel(0))
  {
    outputTransform->m_deltaAtt.fromRotationVector(*deadBlendRates->getPosVelAngVelChannelAngVel(0) * dt);
    outputTransform->m_deltaPos = *deadBlendRates->getPosVelAngVelChannelPosVel(0) * dt;
    outputTransform->m_filteredOut = false;
  }
  else
  {
    outputTransform->m_deltaAtt.identity();
    outputTransform->m_deltaPos.setToZero();
    outputTransform->m_filteredOut = true;
  }

  NMP::Vector3 source1DeltaPos = outputTransform->m_deltaPos;
  NMP::Quat source1DeltaAtt = outputTransform->m_deltaAtt;
  bool source1FilteredOut = outputTransform->m_filteredOut;
  AttribDataTrajectoryDeltaTransform* source2 = destTrajectoryTransform;

  blend2TrajectoryDeltaTransform(
    &outputTransform->m_deltaPos,
    &outputTransform->m_deltaAtt,
    &outputTransform->m_filteredOut,
    source1DeltaPos,
    source1DeltaAtt,
    source1FilteredOut,
    source2->m_deltaPos,
    source2->m_deltaAtt,
    source2->m_filteredOut,
    deadBlendState->m_blendMode,
    deadBlendState->m_slerpTrajectoryPosition,
    blendWeights->m_trajectoryAndTransformsWeights[0]);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskAnimDeadBlendTransformBuffs(Dispatcher::TaskParameters* parameters)
{
  AttribDataTransformBuffer* destTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataDeadBlendState* deadBlendState = parameters->getInputAttrib<AttribDataDeadBlendState>(1, ATTRIB_SEMANTIC_DEAD_BLEND_STATE);
  AttribDataDeadBlendDef* deadBlendDef = parameters->getInputAttrib<AttribDataDeadBlendDef>(2, ATTRIB_SEMANTIC_DEAD_BLEND_DEF);
  AttribDataUpdatePlaybackPos* deltaTime = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(3, ATTRIB_SEMANTIC_UPDATE_TIME_POS);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(4, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 1);
  const uint32_t numAnimationJoints = destTransforms->m_transformBuffer->getLength();
  // Dead blend rates - can get modified.
  AttribDataTransformBuffer* deadBlendRates = parameters->createOutputAttribReplaceTransformBuffer(5, ATTRIB_SEMANTIC_TRANSFORM_RATES, numAnimationJoints);
  // Create this frame's output attribute that will replace the last frames attribute when we copy back attrib.
  AttribDataTransformBuffer* deadBlendTransformsState = parameters->createOutputAttribReplaceTransformBuffer(6, ATTRIB_SEMANTIC_DEAD_BLEND_TRANSFORMS_STATE, numAnimationJoints);
  AttribDataTransformBuffer* transforms = parameters->createOutputAttribTransformBuffer(7, numAnimationJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  // Optional anim rig and channel map

  NMP::Quat origHipsOrientation(NMP::Quat::kIdentity);
  const float dt = deltaTime->m_value;

  // integration (animation dead blend)
  IntegrateTransforms(
    *(deadBlendTransformsState->m_transformBuffer),
    *(deadBlendRates->m_transformBuffer),
    dt, deadBlendDef->m_deadReckoningAngularVelocityDamping);

#if OUTPUT_DEAD_TRANSFORMS
  // nothing to do
  (void) blendWeights;
  (void) transforms;
  (void) deadBlendTransformsState;
  deadBlendTransformsState->m_transformBuffer->copyTo(transforms->m_transformBuffer);
#else

  NMP::DataBuffer* sourceBuffer0 = deadBlendTransformsState->m_transformBuffer;
  NMP::DataBuffer* sourceBuffer1 = destTransforms->m_transformBuffer;

  // Real blend 2
  switch (deadBlendState->m_blendMode)
  {
  case BLEND_MODE_ADD_ATT_ADD_POS:
    MR::BlendOps::addQuatAddPosPartial(
      transforms->m_transformBuffer,
      sourceBuffer0,
      sourceBuffer1,
      blendWeights->m_trajectoryAndTransformsWeights[0]);
    break;
  case BLEND_MODE_ADD_ATT_LEAVE_POS:
    MR::BlendOps::addQuatLeavePosPartial(
      transforms->m_transformBuffer,
      sourceBuffer0,
      sourceBuffer1,
      blendWeights->m_trajectoryAndTransformsWeights[0]);
    break;
  case BLEND_MODE_INTERP_ATT_ADD_POS:
    MR::BlendOps::interpQuatAddPosPartial(
      transforms->m_transformBuffer,
      sourceBuffer0,
      sourceBuffer1,
      blendWeights->m_trajectoryAndTransformsWeights[0]);
    break;
  default:
    MR::BlendOps::interpQuatInterpPosPartial(
      transforms->m_transformBuffer,
      sourceBuffer0,
      sourceBuffer1,
      blendWeights->m_trajectoryAndTransformsWeights[0]);
    break;
  }

#endif
}

//----------------------------------------------------------------------------------------------------------------------
void TaskAnimDeadBlendTrajectoryDeltaAndTransformBuffs(Dispatcher::TaskParameters* parameters)
{
  AttribDataTransformBuffer* destTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  AttribDataDeadBlendState* deadBlendState = parameters->getInputAttrib<AttribDataDeadBlendState>(1, ATTRIB_SEMANTIC_DEAD_BLEND_STATE);
  AttribDataDeadBlendDef* deadBlendDef = parameters->getInputAttrib<AttribDataDeadBlendDef>(2, ATTRIB_SEMANTIC_DEAD_BLEND_DEF);
  AttribDataUpdatePlaybackPos* deltaTime = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(3, ATTRIB_SEMANTIC_UPDATE_TIME_POS);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(4, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_trajectoryAndTransformsNumWeights == 1);
  const uint32_t numAnimationJoints = destTransforms->m_transformBuffer->getLength();
  // Dead blend rates - can get modified.
  AttribDataTransformBuffer* deadBlendRates = parameters->createOutputAttribReplaceTransformBuffer(5, ATTRIB_SEMANTIC_TRANSFORM_RATES, numAnimationJoints);
  // Create this frame's output attribute that will replace the last frames attribute when we copy back attrib.
  AttribDataTransformBuffer* deadBlendTransformsState = parameters->createOutputAttribReplaceTransformBuffer(6, ATTRIB_SEMANTIC_DEAD_BLEND_TRANSFORMS_STATE, numAnimationJoints);
  AttribDataTransformBuffer* transformsAttr = parameters->createOutputAttribTransformBuffer(7, numAnimationJoints, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  // Optional anim rig and channel map

  NMP::DataBuffer* transforms = transformsAttr->m_transformBuffer;

  NMP::Quat origHipsOrientation(NMP::Quat::kIdentity);
  const float dt = deltaTime->m_value;

  // integration (animation dead blend)
  IntegrateTransforms(
    *(deadBlendTransformsState->m_transformBuffer),
    *(deadBlendRates->m_transformBuffer),
    dt, deadBlendDef->m_deadReckoningAngularVelocityDamping);

#if OUTPUT_DEAD_TRANSFORMS
  // nothing to do
  (void) blendWeights;
  (void) transforms;
  (void) deadBlendTransformsState;
  deadBlendTransformsState->m_transformBuffer->copyTo(transforms);
#else

  NMP::DataBuffer* sourceBuffer0 = deadBlendTransformsState->m_transformBuffer;
  NMP::DataBuffer* sourceBuffer1 = destTransforms->m_transformBuffer;

  // Real blend 2
  switch (deadBlendState->m_blendMode)
  {
  case BLEND_MODE_ADD_ATT_ADD_POS:
    MR::BlendOps::addQuatAddPosPartial(
      transforms,
      sourceBuffer0,
      sourceBuffer1,
      blendWeights->m_trajectoryAndTransformsWeights[0]);
    break;
  case BLEND_MODE_ADD_ATT_LEAVE_POS:
    MR::BlendOps::addQuatLeavePosPartial(
      transforms,
      sourceBuffer0,
      sourceBuffer1,
      blendWeights->m_trajectoryAndTransformsWeights[0]);
    break;
  case BLEND_MODE_INTERP_ATT_ADD_POS:
    MR::BlendOps::interpQuatAddPosPartial(
      transforms,
      sourceBuffer0,
      sourceBuffer1,
      blendWeights->m_trajectoryAndTransformsWeights[0]);
    break;
  default:
    MR::BlendOps::interpQuatInterpPosPartial(
      transforms,
      sourceBuffer0,
      sourceBuffer1,
      blendWeights->m_trajectoryAndTransformsWeights[0]);
    break;
  }

#endif

  {
    // Blend the trajectory delta
    transforms->getPosQuatChannelQuat(0)->fromRotationVector(*deadBlendRates->m_transformBuffer->getPosVelAngVelChannelAngVel(0) * dt);
    *transforms->getPosQuatChannelPos(0) = *deadBlendRates->m_transformBuffer->getPosVelAngVelChannelPosVel(0) * dt;

    NMP::Vector3 source1DeltaPos = *transforms->getPosQuatChannelPos(0);
    NMP::Quat source1DeltaAtt = *transforms->getPosQuatChannelQuat(0);
    bool source1FilteredOut = !transforms->hasChannel(0);

    bool trajectoryFilfteredOut = false;
    blend2TrajectoryDeltaTransform(
      transforms->getPosQuatChannelPos(0),
      transforms->getPosQuatChannelQuat(0),
      &trajectoryFilfteredOut,
      source1DeltaPos,
      source1DeltaAtt,
      source1FilteredOut,
      *destTransforms->m_transformBuffer->getPosQuatChannelPos(0),
      *destTransforms->m_transformBuffer->getPosQuatChannelQuat(0),
      !destTransforms->m_transformBuffer->hasChannel(0),
      deadBlendState->m_blendMode,
      deadBlendState->m_slerpTrajectoryPosition,
      blendWeights->m_trajectoryAndTransformsWeights[0]);

    transforms->setChannelUsedFlag(0, !trajectoryFilfteredOut);
  }

  transforms->calculateFullFlag();
}

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
// Work out the update time values for the first frame of a simple transition.
void transitGenerateFirstFrameTimeValues(
  AttribDataPlaybackPos*                currentFrametimePos,
  AttribDataUpdatePlaybackPos*          parentsUpdateTimePos,
  AttribDataUpdatePlaybackPos*          destUpdateTimePos,
  AttribDataUpdateSyncEventPlaybackPos* sourceSyncEventPos,
  AttribDataTransitDef*                 transitDef,
  AttribDataSyncEventTrack*             destSyncEventTrack,
  NodeID                                transitNodeID)
{
  currentFrametimePos->m_previousPosAdj = parentsUpdateTimePos->m_value;
  currentFrametimePos->m_currentPosAdj = parentsUpdateTimePos->m_value;
  currentFrametimePos->m_setWithAbs = true;
  currentFrametimePos->m_delta = 0.0f;

  // First frame of transition so set the destination time.
  destUpdateTimePos->m_isAbs = true;
  if (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_USE_START_DURATION_FRACTION)
  {
    // Start at a fraction of the duration of the destination.
    destUpdateTimePos->m_isFraction = true;
    destUpdateTimePos->m_value = transitDef->m_destinationStartFraction;
  }
  else if (!destSyncEventTrack)
  {
    // Start at a fraction of the duration of the destination.
    destUpdateTimePos->m_isFraction = true;
    destUpdateTimePos->m_value = transitDef->m_destinationStartFraction;

    // MORPH-21376: Output this in connect.
    NMP_MSG("WARNING: Unable to accurately determine the start position of the destination of the transition NodeID %d.", transitNodeID);
    NMP_MSG("This is because the destination has not generated a sync event track this frame.");
    NMP_MSG("A sync event track is the only way to get a duration value from the destination.");
  }
  else if (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_USE_EVENT_INDEX_AND_USE_EVENT_FRACTION)
  {
    // Start at a specified event position.
    float destinationStartSyncEvent = transitDef->m_destinationStartSyncEvent;
    float fractionStartPos = destSyncEventTrack->m_syncEventTrack.getAdjPosFractionFromAdjSyncEventPos(destinationStartSyncEvent);
    destUpdateTimePos->m_value = fractionStartPos * destSyncEventTrack->m_syncEventTrack.getDuration();
    destUpdateTimePos->m_isFraction = false;
  }
  else if (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_USE_EVENT_INDEX_AND_SYNC_ON_EVENT_FRACTION && sourceSyncEventPos)
  {
    // Start at a specified event index and synchronise the start event fraction of the destination with the current fraction of the source.
    float destinationStartSyncEvent = NMP::nmfloor(transitDef->m_destinationStartSyncEvent); // Only interested in the integer part.
    if (sourceSyncEventPos->m_absPosAdj.fraction() < 1.0f)
      destinationStartSyncEvent += sourceSyncEventPos->m_absPosAdj.fraction();
    float fractionStartPos = destSyncEventTrack->m_syncEventTrack.getAdjPosFractionFromAdjSyncEventPos(destinationStartSyncEvent);
    destUpdateTimePos->m_value = fractionStartPos * destSyncEventTrack->m_syncEventTrack.getDuration();
    destUpdateTimePos->m_isFraction = false;
  }
  else if (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_SYNC_ON_EVENT_INDEX_AND_USE_EVENT_FRACTION && sourceSyncEventPos)
  {
    // Start at the same event pos as the source but use the specified event fraction.
    float destinationStartSyncEvent = (float) sourceSyncEventPos->m_absPosAdj.index();
    destinationStartSyncEvent += transitDef->m_destinationStartSyncEvent; // Assume only contains the fractional part.
    float fractionStartPos = destSyncEventTrack->m_syncEventTrack.getAdjPosFractionFromAdjSyncEventPos(destinationStartSyncEvent);
    destUpdateTimePos->m_value = fractionStartPos * destSyncEventTrack->m_syncEventTrack.getDuration();
    destUpdateTimePos->m_isFraction = false;
  }
  else if (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_SYNC_ON_EVENT_INDEX_AND_SYNC_ON_EVENT_FRACTION && sourceSyncEventPos)
  {
    // Start at the same event pos as the source.
    float fractionStartPos = destSyncEventTrack->m_syncEventTrack.getAdjPosFractionFromAdjSyncEventPos(sourceSyncEventPos->m_absPosAdj);
    destUpdateTimePos->m_value = fractionStartPos * destSyncEventTrack->m_syncEventTrack.getDuration();
    destUpdateTimePos->m_isFraction = false;
  }
  else
  {
    destUpdateTimePos->m_isFraction = true;
    destUpdateTimePos->m_value = transitDef->m_destinationStartFraction;
    if ((transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_USE_EVENT_INDEX_AND_SYNC_ON_EVENT_FRACTION) ||
        (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_SYNC_ON_EVENT_INDEX_AND_USE_EVENT_FRACTION) ||
        (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_SYNC_ON_EVENT_INDEX_AND_SYNC_ON_EVENT_FRACTION))
    {
      // MORPH-21376: Output this in connect.
      NMP_MSG("WARNING: Unable to accurately determine the start position of the destination of the transition NodeID %d.", transitNodeID);
      NMP_MSG("No source sync event position was generated this frame, this may result in unexpected behaviour.");
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Work out the update time values for the first frame of a simple transition.
void transitDeadBlendGenerateFirstFrameTimeValues(
  AttribDataPlaybackPos*                currentFrametimePos,
  AttribDataUpdatePlaybackPos*          parentsUpdateTimePos,
  AttribDataUpdatePlaybackPos*          destUpdateTimePos,
  AttribDataUpdateSyncEventPlaybackPos* sourceSyncEventPos,
  AttribDataTransitDef*                 transitDef,
  AttribDataSyncEventTrack*             destSyncEventTrack,
  NodeID                                transitNodeID)
{
  currentFrametimePos->m_previousPosAdj = parentsUpdateTimePos->m_value;
  currentFrametimePos->m_currentPosAdj = parentsUpdateTimePos->m_value;
  currentFrametimePos->m_setWithAbs = true;
  currentFrametimePos->m_delta = 0.0f;

  // First frame of transition so set the destination time.
  destUpdateTimePos->m_isAbs = true;
  if (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_USE_START_DURATION_FRACTION)
  {
    // Start at a fraction of the duration of the destination.
    destUpdateTimePos->m_isFraction = true;
    destUpdateTimePos->m_value = transitDef->m_destinationStartFraction;
  }
  else if (!destSyncEventTrack)
  {
    // Start at a fraction of the duration of the destination.
    destUpdateTimePos->m_isFraction = true;
    destUpdateTimePos->m_value = transitDef->m_destinationStartFraction;

    // MORPH-21376: Output this in connect.
    NMP_MSG("WARNING: Unable to accurately determine the start position of the destination of the transition NodeID %d.", transitNodeID);
    NMP_MSG("This is because the destination has not generated a sync event track this frame.");
    NMP_MSG("A sync event track is the only way to get a duration value from the destination.");     
  }
  else if (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_USE_EVENT_INDEX_AND_USE_EVENT_FRACTION)
  {
    // Start at a specified event position.
    float destinationStartSyncEvent = transitDef->m_destinationStartSyncEvent;
    if(destinationStartSyncEvent - destSyncEventTrack->m_syncEventTrack.getNumEvents() >= 0.0f)
    {
      NMP_ASSERT(destSyncEventTrack->m_syncEventTrack.getNumEvents() > 0);
      destinationStartSyncEvent = -0.00001f;
    }
    destinationStartSyncEvent = destSyncEventTrack->m_syncEventTrack.convEventInRealSpaceToAdjustedSpace(destinationStartSyncEvent);
    float fractionStartPos = destSyncEventTrack->m_syncEventTrack.getAdjPosFractionFromAdjSyncEventPos(destinationStartSyncEvent);
    destUpdateTimePos->m_value = fractionStartPos * destSyncEventTrack->m_syncEventTrack.getDuration();
    destUpdateTimePos->m_isFraction = false;
  }
  else if (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_USE_EVENT_INDEX_AND_SYNC_ON_EVENT_FRACTION && sourceSyncEventPos)
  {
    // Start at a specified event index and synchronise the start event fraction of the destination with the current fraction of the source.
    float destinationStartSyncEvent = NMP::nmfloor(transitDef->m_destinationStartSyncEvent); // Only interested in the integer part.
    if (sourceSyncEventPos->m_absPosAdj.fraction() < 1.0f)
      destinationStartSyncEvent += sourceSyncEventPos->m_absPosAdj.fraction();
    if(destinationStartSyncEvent - destSyncEventTrack->m_syncEventTrack.getNumEvents() >= 0.0f)
    {
      NMP_ASSERT(destSyncEventTrack->m_syncEventTrack.getNumEvents() > 0);
      destinationStartSyncEvent = -0.00001f;
    }
    float fractionStartPos = destSyncEventTrack->m_syncEventTrack.getAdjPosFractionFromAdjSyncEventPos(destinationStartSyncEvent);
    destUpdateTimePos->m_value = fractionStartPos * destSyncEventTrack->m_syncEventTrack.getDuration();
    destUpdateTimePos->m_isFraction = false;
  }
  else if (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_SYNC_ON_EVENT_INDEX_AND_USE_EVENT_FRACTION && sourceSyncEventPos)
  {
    // Start at the same event pos as the source but use the specified event fraction.
    float destinationStartSyncEvent = (float) sourceSyncEventPos->m_absPosAdj.index();
    destinationStartSyncEvent += transitDef->m_destinationStartSyncEvent; // Assume only contains the fractional part.
    if(destinationStartSyncEvent - destSyncEventTrack->m_syncEventTrack.getNumEvents() >= 0.0f)
    {
      NMP_ASSERT(destSyncEventTrack->m_syncEventTrack.getNumEvents() > 0);
      destinationStartSyncEvent = -0.00001f;
    }
    float fractionStartPos = destSyncEventTrack->m_syncEventTrack.getAdjPosFractionFromAdjSyncEventPos(destinationStartSyncEvent);
    destUpdateTimePos->m_value = fractionStartPos * destSyncEventTrack->m_syncEventTrack.getDuration();
    destUpdateTimePos->m_isFraction = false;
  }
  else if (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_SYNC_ON_EVENT_INDEX_AND_SYNC_ON_EVENT_FRACTION && sourceSyncEventPos)
  {
    // Start at the same event pos as the source.
    float fractionStartPos = destSyncEventTrack->m_syncEventTrack.getAdjPosFractionFromAdjSyncEventPos(sourceSyncEventPos->m_absPosAdj);
    destUpdateTimePos->m_value = fractionStartPos * destSyncEventTrack->m_syncEventTrack.getDuration();
    destUpdateTimePos->m_isFraction = false;
  }
  else
  {
    destUpdateTimePos->m_isFraction = true;
    destUpdateTimePos->m_value = transitDef->m_destinationStartFraction;
    if ((transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_USE_EVENT_INDEX_AND_SYNC_ON_EVENT_FRACTION) ||
      (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_SYNC_ON_EVENT_INDEX_AND_USE_EVENT_FRACTION) ||
      (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_SYNC_ON_EVENT_INDEX_AND_SYNC_ON_EVENT_FRACTION))
    {
      // MORPH-21376: Output this in connect.
      NMP_MSG("WARNING: Unable to accurately determine the start position of the destination of the transition NodeID %d.", transitNodeID);
      NMP_MSG("No source sync event position was generated this frame, this may result in unexpected behaviour.");
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void TaskTransitUpdateTimePos(Dispatcher::TaskParameters* parameters)
{
  AttribDataUpdatePlaybackPos* parentsUpdateTimePos = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(0, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  AttribDataUpdatePlaybackPos* sourceUpdateTimePos = parameters->createOutputAttrib<AttribDataUpdatePlaybackPos>(1, ATTRIB_SEMANTIC_UPDATE_TIME_POS);
  AttribDataUpdatePlaybackPos* destUpdateTimePos = parameters->createOutputAttrib<AttribDataUpdatePlaybackPos>(2, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  AttribDataTransitDef* transitDef = parameters->getInputAttrib<AttribDataTransitDef>(3, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  AttribDataBlendWeightsCreateDesc desc(1);
  AttribDataBlendWeights* blendWeights = parameters->createOutputAttrib<AttribDataBlendWeights>(4, ATTRIB_SEMANTIC_BLEND_WEIGHTS, &desc);

  AttribDataPlaybackPos* lastFrameTimePos = parameters->getOptionalInputAttrib<AttribDataPlaybackPos>(5, ATTRIB_SEMANTIC_TIME_POS);
  AttribDataPlaybackPos* currentFrametimePos = parameters->createOutputAttrib<AttribDataPlaybackPos>(6, ATTRIB_SEMANTIC_TIME_POS);

  // If reversible get control param attrib data.
  bool reverseTransition = false;
  AttribDataBool* cpAttribBool = parameters->getOptionalInputAttrib<AttribDataBool>(7, ATTRIB_SEMANTIC_CP_BOOL);
  if (transitDef->m_reversible && cpAttribBool)
    reverseTransition = cpAttribBool->m_value;

  AttribDataUpdateSyncEventPlaybackPos* sourceSyncEventPos = parameters->getOptionalInputAttrib<AttribDataUpdateSyncEventPlaybackPos>(8, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);

  AttribDataBool* transitComplete = parameters->createOutputAttribReplace<AttribDataBool>(9, ATTRIB_SEMANTIC_TRANSIT_COMPLETE);
  AttribDataBool* transitReversed = parameters->createOutputAttribReplace<AttribDataBool>(10, ATTRIB_SEMANTIC_TRANSIT_REVERSED);

  AttribDataSyncEventTrack* destSyncEventTrack = parameters->getOptionalInputAttrib<AttribDataSyncEventTrack>(11, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);

  // This section of code will freeze the time of the source and/or destination if the transitions has been flagged to do so
  sourceUpdateTimePos->m_isFraction = parentsUpdateTimePos->m_isFraction;
  sourceUpdateTimePos->m_isAbs = parentsUpdateTimePos->m_isAbs;
  if (parentsUpdateTimePos->m_isAbs || !transitDef->m_freezeSource)
  {
    sourceUpdateTimePos->m_value = parentsUpdateTimePos->m_value;
  }
  else
  {
    sourceUpdateTimePos->m_value = 0;
  }
  destUpdateTimePos->m_isFraction = parentsUpdateTimePos->m_isFraction;
  destUpdateTimePos->m_isAbs = parentsUpdateTimePos->m_isAbs;
  if (parentsUpdateTimePos->m_isAbs || !transitDef->m_freezeDest)
  {
    destUpdateTimePos->m_value = parentsUpdateTimePos->m_value;
  }
  else
  {
    destUpdateTimePos->m_value = 0;
  }
  //-----------------------
  // Update transition time.
  if (parentsUpdateTimePos->m_isAbs || !lastFrameTimePos)
  {
    if (!lastFrameTimePos)
    {
      currentFrametimePos->m_previousPosAdj = parentsUpdateTimePos->m_value;
      currentFrametimePos->m_currentPosAdj = parentsUpdateTimePos->m_value;
      currentFrametimePos->m_setWithAbs = true;
      currentFrametimePos->m_delta = 0.0f;
    }
    else
    {
      // This is the first frame of update for the transit so calculate the starting time values.
      transitGenerateFirstFrameTimeValues(
        currentFrametimePos,
        parentsUpdateTimePos,
        destUpdateTimePos,
        sourceSyncEventPos,
        transitDef,
        destSyncEventTrack,
        parameters->m_parameters[3].m_attribAddress.m_owningNodeID);
    }
  }
  else
  {
    currentFrametimePos->m_previousPosAdj = lastFrameTimePos->m_currentPosAdj;
    if (reverseTransition)
      currentFrametimePos->m_currentPosAdj = lastFrameTimePos->m_currentPosAdj - parentsUpdateTimePos->m_value;
    else
      currentFrametimePos->m_currentPosAdj = lastFrameTimePos->m_currentPosAdj + parentsUpdateTimePos->m_value;
    currentFrametimePos->m_setWithAbs = false;
    currentFrametimePos->m_delta = parentsUpdateTimePos->m_value;
  }

  //-----------------------
  // Transit starts neither complete or reversed.
  transitComplete->m_value = false;
  transitReversed->m_value = false;

  //-----------------------
  // Calculate blend weight.
  float blendWeight;
  if (transitDef->m_duration == 0.0f)
  {
    //-----------------------
    // Duration is Zero length so indicate transition is complete and set blendWeight to 1.0f
    transitComplete->m_value = true;
    blendWeight = 1.0f;
  }
  else
  {
    blendWeight = currentFrametimePos->m_currentPosAdj / transitDef->m_duration;
    if (blendWeight >= 1.0f)
    {
      //-----------------------
      // blendWeight value indicates transition is complete set to complete and set blendWeight to 1.0f
      transitComplete->m_value = true;
      blendWeight = 1.0f;
    }
    else
    {
      //-----------------------
      //if the blend weight has reversed below 0 signal a reversed transit, transit complete and set blendWeight to 0.0f
      if (blendWeight < 0.0f)
      {
        transitComplete->m_value = true;
        transitReversed->m_value = true;
        blendWeight = 0.0f;
      }
      else
      {
        // remap the blend weight from a linear ramp to a sine segment such that:
        // blendWeight = 0.0 -> sin(-pi/2) + 1
        // blendWeight = 1.0 -> sin(pi/2)
        // Note that the resultant range of blendWeight is still 0 -> 1

        // Initially, 0 < blendWeight < 1
        blendWeight -= 0.5f;
        blendWeight *= NM_PI;
        NMP_ASSERT(blendWeight <= NM_PI_OVER_TWO && blendWeight >= -NM_PI_OVER_TWO);
        // Now, -pi/2 < blendWeight < pi/2
        blendWeight = NMP::fastSin(blendWeight);
        // Now, sin(-pi/2) < blendWeight < sin(pi/2)
        blendWeight += 1.0f;
        blendWeight *= 0.5f;
        // Now, sin(-pi/2)+1 < blendWeight < sin(pi/2)
        NMP_ASSERT(blendWeight <= 1.0f && blendWeight >= 0.0f);
      }
    }
  }

  // Set the blend weights
  blendWeights->setAllWeights(blendWeight);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskTransitDeadBlendUpdateTimePos(Dispatcher::TaskParameters* parameters)
{
  AttribDataUpdatePlaybackPos* parentsUpdateTimePos = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(0, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  AttribDataUpdatePlaybackPos* destUpdateTimePos = parameters->createOutputAttrib<AttribDataUpdatePlaybackPos>(1, ATTRIB_SEMANTIC_UPDATE_TIME_POS);
  destUpdateTimePos->m_isFraction = parentsUpdateTimePos->m_isFraction;
  destUpdateTimePos->m_isAbs = parentsUpdateTimePos->m_isAbs;
  destUpdateTimePos->m_value = parentsUpdateTimePos->m_value;

  AttribDataTransitDef* transitDef = parameters->getInputAttrib<AttribDataTransitDef>(2, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  AttribDataBlendWeightsCreateDesc desc(1);
  AttribDataBlendWeights* blendWeights = parameters->createOutputAttrib<AttribDataBlendWeights>(3, ATTRIB_SEMANTIC_BLEND_WEIGHTS, &desc);

  AttribDataPlaybackPos* lastFrameTimePos = parameters->getOptionalInputAttrib<AttribDataPlaybackPos>(4, ATTRIB_SEMANTIC_TIME_POS);
  AttribDataPlaybackPos* currentFrametimePos = parameters->createOutputAttrib<AttribDataPlaybackPos>(5, ATTRIB_SEMANTIC_TIME_POS);

  AttribDataUpdateSyncEventPlaybackPos* sourceSyncEventPos = parameters->getOptionalInputAttrib<AttribDataUpdateSyncEventPlaybackPos>(6, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);

  AttribDataBool* transitComplete = parameters->createOutputAttribReplace<AttribDataBool>(7, ATTRIB_SEMANTIC_TRANSIT_COMPLETE);

  AttribDataSyncEventTrack* destSyncEventTrack = parameters->getOptionalInputAttrib<AttribDataSyncEventTrack>(8, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);

  // This section of code will freeze the time of the source and/or destination if the transitions has been flagged to do so
  destUpdateTimePos->m_isFraction = parentsUpdateTimePos->m_isFraction;
  destUpdateTimePos->m_isAbs = parentsUpdateTimePos->m_isAbs;
  if (parentsUpdateTimePos->m_isAbs || !transitDef->m_freezeDest)
  {
    destUpdateTimePos->m_value = parentsUpdateTimePos->m_value;
  }
  else
  {
    destUpdateTimePos->m_value = 0;
  }
  //-----------------------
  // Update transition time.
  if (parentsUpdateTimePos->m_isAbs || !lastFrameTimePos)
  {
    if (!lastFrameTimePos)
    {
      currentFrametimePos->m_previousPosAdj = parentsUpdateTimePos->m_value;
      currentFrametimePos->m_currentPosAdj = parentsUpdateTimePos->m_value;
      currentFrametimePos->m_setWithAbs = true;
      currentFrametimePos->m_delta = 0.0f;
    }
    else
    {
      // This is the first frame of update for the transit so calculate the starting time values.
      transitDeadBlendGenerateFirstFrameTimeValues(
        currentFrametimePos,
        parentsUpdateTimePos,
        destUpdateTimePos,
        sourceSyncEventPos,
        transitDef,
        destSyncEventTrack,
        parameters->m_parameters[2].m_attribAddress.m_owningNodeID);
    }
  }
  else
  {
    currentFrametimePos->m_previousPosAdj = lastFrameTimePos->m_currentPosAdj;
    currentFrametimePos->m_currentPosAdj = lastFrameTimePos->m_currentPosAdj + parentsUpdateTimePos->m_value;
    currentFrametimePos->m_setWithAbs = false;
    currentFrametimePos->m_delta = parentsUpdateTimePos->m_value;
  }

  //-----------------------
  // Calculate blend weight.
  float blendWeight;
  if (transitDef->m_duration == 0.0f)
    blendWeight = 1.0f;
  else
  {
    blendWeight = currentFrametimePos->m_currentPosAdj / transitDef->m_duration;
    if (blendWeight >= 1.0f)
      blendWeight = 1.0f;
    else
    {
      blendWeight -= 0.5f;
      blendWeight *= NM_PI;
      NMP_ASSERT(blendWeight <= NM_PI_OVER_TWO && blendWeight >= -NM_PI_OVER_TWO);
      blendWeight = NMP::fastSin(blendWeight);
      blendWeight += 1.0f;
      blendWeight *= 0.5f;
      NMP_ASSERT(blendWeight <= 1.0f && blendWeight >= 0.0f);
    }
  }

  //-----------------------
  // Add an attribute to the network indicating if the transition has completed.
  if (blendWeight == 1.0f)
    transitComplete->m_value = true;
  else
    transitComplete->m_value = false;

  // Set the blend weights
  blendWeights->setAllWeights(blendWeight);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
