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
#include "morpheme/mrNetwork.h"
#include "morpheme/mrTaskUtilities.h"
#include "morpheme/mrMirroredAnimMapping.h"
#include "morpheme/mrBlendOps.h"
#include "NMGeomUtils/NMGeomUtils.h"
#include "NMPlatform/NMQuat.h"
#ifdef NM_HOST_CELL_SPU
  #include "morphemeSPU/mrDispatcherSPU.h"
  #include "NMPlatform/ps3/NMSPUDMAController.h"
#endif // NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
namespace 
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Given a time position work out the actual position to sample from the animation buffer.
float calcAnimSamplePos(float timePos, AttribDataSourceAnim* sourceAnim)
{
  float animSamplePos = (sourceAnim->m_sourceAnimDuration * sourceAnim->m_clipStartFraction);
  if (sourceAnim->m_playBackwards)
    animSamplePos += ((sourceAnim->m_sourceAnimDuration * (sourceAnim->m_clipEndFraction - sourceAnim->m_clipStartFraction)) - timePos);
  else
    animSamplePos += timePos;
  NMP_ASSERT(animSamplePos >= 0.0f && animSamplePos <= sourceAnim->m_sourceAnimDuration);

  return animSamplePos;
}

//----------------------------------------------------------------------------------------------------------------------
void calcCurrentAnimSamplePos(
  AttribDataPlaybackPos*  attrAnimSamplePosOut,
  AttribDataPlaybackPos*  attrTimeIn,
  AttribDataSourceAnim*   sourceAnim)
{
  // Store in real and adjusted values as anim sample pos is not expressed in either space.
  attrAnimSamplePosOut->m_currentPosAdj = calcAnimSamplePos(
      attrTimeIn->m_currentPosReal,
      sourceAnim);
  attrAnimSamplePosOut->m_currentPosReal = attrAnimSamplePosOut->m_currentPosAdj;
}

//----------------------------------------------------------------------------------------------------------------------
void calcPreviousAnimSamplePos(
  AttribDataPlaybackPos*  attrAnimSamplePosOut,
  AttribDataPlaybackPos*  attrTimeIn,
  AttribDataSourceAnim*   sourceAnim)
{
  // Store in real and adjusted values as anim sample pos is not expressed in either space.
  attrAnimSamplePosOut->m_previousPosAdj = calcAnimSamplePos(
        attrTimeIn->m_previousPosReal,
        sourceAnim);
  attrAnimSamplePosOut->m_previousPosReal = attrAnimSamplePosOut->m_previousPosAdj;
}

//----------------------------------------------------------------------------------------------------------------------
void subTaskUpdateDeltaTimeOnSyncEventsNode(
  AttribDataUpdateSyncEventPlaybackPos* attrMyPreviousSyncEventPosIn,
  AttribDataSyncEventTrack*             attrMySyncEventTrackIn,
  bool                                  loopableIn,
  AttribDataPlaybackPos*                attrMyTimePosOut,
  AttribDataUpdateSyncEventPlaybackPos* attrMyCurrentSyncEventPosOut,
  AttribDataPlaybackPos*                attrMyFractionPosOut,
  AttribDataUpdatePlaybackPos*          attrParentUpdateTimePosIn,
  AttribDataPlaybackPosInit*            playbackPosInit,
  AttribDataUpdatePlaybackPos*          attrMyUpdateTimePosOut)
{
  AttribDataUpdateSyncEventPlaybackPos prevSyncEventPos;
  if (!attrMyPreviousSyncEventPosIn)
  {
    // First frame initialisation with a delta from parent - Calculate previous frame sync event pos.
    prevSyncEventPos.m_isAbs = true;

    if (!playbackPosInit)                                 // No playback initialisation structure just use parents value.
    {
      prevSyncEventPos.m_absPosAdj.set(0, 0.0f);
    }
    else if (playbackPosInit->m_initWithDurationFraction) // Start at a fraction of the duration of the destination.
    {
      uint32_t eventIndex;
      float    eventFraction;

      attrMySyncEventTrackIn->m_syncEventTrack.getAdjPosInfoFromAdjFraction(playbackPosInit->m_startValue, eventIndex, eventFraction);
      prevSyncEventPos.m_absPosAdj.set(eventIndex, eventFraction);
    }
    else                                                 // Start at an event position.
    {
      prevSyncEventPos.m_absPosAdj.set(playbackPosInit->m_startValue);
    }
    attrMySyncEventTrackIn->m_syncEventTrack.limitToSyncEventSpace(prevSyncEventPos.m_absPosAdj, loopableIn);
    attrMySyncEventTrackIn->m_syncEventTrack.convEventInAdjustedSpaceToRealSpace(prevSyncEventPos.m_absPosAdj, prevSyncEventPos.m_absPosReal);

    attrMyTimePosOut->setInclusiveStart(true);
    attrMyFractionPosOut->setInclusiveStart(true);
  }
  else
  {
    prevSyncEventPos = (*attrMyPreviousSyncEventPosIn);

    attrMyTimePosOut->setInclusiveStart(false);
    attrMyFractionPosOut->setInclusiveStart(false);
  }
  
  // Calculate last frames values. Necessary as sync event track config is likely to have changed.
  calcPreviousPlaybackValuesFromMyAbsSyncEventPos(
    attrMySyncEventTrackIn->m_syncEventTrack,
    loopableIn,
    &prevSyncEventPos,
    &prevSyncEventPos,
    attrMyFractionPosOut,
    attrMyTimePosOut);

  // Calculate this frames values.
  calcCurrentPlaybackValuesFromParentDeltaTimeAdjSpace(
    attrMySyncEventTrackIn->m_syncEventTrack,
    attrParentUpdateTimePosIn,
    &prevSyncEventPos,
    loopableIn,
    attrMyFractionPosOut,
    attrMyTimePosOut,
    attrMyCurrentSyncEventPosOut,
    attrMyUpdateTimePosOut);
}

//----------------------------------------------------------------------------------------------------------------------
// Either copy an existing previous frame value or generate a new one from our parents update value.
void getPreviousFrameSyncEventPos(
  AttribDataUpdateSyncEventPlaybackPos& outputPrevSyncEventPos,   // To be filled in.
  AttribDataUpdateSyncEventPlaybackPos* existingPrevSyncEventPos, // There may already be an existing previous frame value.
  AttribDataUpdateSyncEventPlaybackPos* parentUpdateSyncEventPos,
  AttribDataSyncEventTrack*             syncEventTrack,
  bool                                  loopable)
{
  if (!existingPrevSyncEventPos)
  {
    // First frame initialisation with a delta from parent - Calculate previous frame sync event pos.

    // Index part.
    int32_t index = ((int32_t)parentUpdateSyncEventPos->m_absPosReal.index()) - ((int32_t)parentUpdateSyncEventPos->m_deltaPos.index());
    if (index < 0)
    {
      if (loopable)
        index += ((int32_t)syncEventTrack->m_syncEventTrack.getNumEvents());
      else
        index = 0;
    }

    // Fraction part.
    float fraction = parentUpdateSyncEventPos->m_absPosReal.fraction() - parentUpdateSyncEventPos->m_deltaPos.fraction();
    fraction = NMP::floatSelect(fraction, fraction, fmodf(fraction, 1.0f) + 1.0f); // Wrap around if (fraction < 0.0f)
    
    outputPrevSyncEventPos.m_absPosAdj.set(index, fraction);
    syncEventTrack->m_syncEventTrack.limitToSyncEventSpace(outputPrevSyncEventPos.m_absPosAdj, loopable);
    syncEventTrack->m_syncEventTrack.convEventInAdjustedSpaceToRealSpace(outputPrevSyncEventPos.m_absPosAdj, outputPrevSyncEventPos.m_absPosReal);
  }
  else
  {
    outputPrevSyncEventPos = (*existingPrevSyncEventPos);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void subTaskUpdateDeltaSyncEventOnSyncEventsNode(
  AttribDataUpdateSyncEventPlaybackPos* attrMyPreviousSyncEventPosIn,
  AttribDataSyncEventTrack*             attrMySyncEventTrackIn,
  AttribDataBool*                       attrMyLoopableIn,
  AttribDataPlaybackPos*                attrMyTimePosOut,
  AttribDataUpdateSyncEventPlaybackPos* attrMyCurrentSyncEventPosOut,
  AttribDataPlaybackPos*                attrMyFractionPosOut,
  AttribDataUpdateSyncEventPlaybackPos* attrParentAdjUpdateSyncEventPosIn,
  AttribDataUpdatePlaybackPos*          attrMyUpdateTimePosOut)
{
  AttribDataUpdateSyncEventPlaybackPos prevSyncEventPos;
  getPreviousFrameSyncEventPos(
    prevSyncEventPos,
    attrMyPreviousSyncEventPosIn,
    attrParentAdjUpdateSyncEventPosIn,
    attrMySyncEventTrackIn,
    attrMyLoopableIn->m_value);

  // Calculate last frames values. Necessary as sync event track config is likely to have changed.
  calcPreviousPlaybackValuesFromMyAbsSyncEventPos(
    attrMySyncEventTrackIn->m_syncEventTrack,
    attrMyLoopableIn->m_value,
    &prevSyncEventPos,
    &prevSyncEventPos,
    attrMyFractionPosOut,
    attrMyTimePosOut);

  // Calculate this frames values.
  calcCurrentPlaybackValuesFromParentDeltaSyncEventPosAdjSpace(
    attrMySyncEventTrackIn->m_syncEventTrack,
    attrParentAdjUpdateSyncEventPosIn,
    &prevSyncEventPos,
    attrMyLoopableIn->m_value,
    attrMyFractionPosOut,
    attrMyTimePosOut,
    attrMyCurrentSyncEventPosOut,
    attrMyUpdateTimePosOut);
}
    
//---------------------------------------------------------------------------------------------------------------------- 
}

//----------------------------------------------------------------------------------------------------------------------
// Updates are provided to us by our parent in our Adjusted space (parents Real space).
// All working is done internally in our Adjusted space.
// Provision to parent, conditions etc is in our Adjusted space.
// Real space is only used internally to calculate animation sampling position.
void TaskAnimSyncEventsUpdateTimePos(Dispatcher::TaskParameters* parameters)
{
  AttribDataUpdatePlaybackPos* attrParentUpdateTimePosIn = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(0, ATTRIB_SEMANTIC_UPDATE_TIME_POS);
  AttribDataUpdateSyncEventPlaybackPos* attrMyPreviousSyncEventPosIn = parameters->getInputAttrib<AttribDataUpdateSyncEventPlaybackPos>(1, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataUpdateSyncEventPlaybackPos* attrMyCurrentSyncEventPosOut = parameters->createOutputAttribReplace<AttribDataUpdateSyncEventPlaybackPos>(1, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataPlaybackPos* attrMyTimePosOut = parameters->createOutputAttribReplace<AttribDataPlaybackPos>(2, ATTRIB_SEMANTIC_TIME_POS);
  AttribDataPlaybackPos* attrMyAnimSamplePosOut = parameters->createOutputAttribReplace<AttribDataPlaybackPos>(3, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS);
  AttribDataPlaybackPos* attrMyFractionPosOut = parameters->createOutputAttribReplace<AttribDataPlaybackPos>(4, ATTRIB_SEMANTIC_FRACTION_POS);
  AttribDataBool* attrMyRealLoopedOnUpdateOut = parameters->createOutputAttrib<AttribDataBool>(5, ATTRIB_SEMANTIC_LOOPED_ON_UPDATE);
  AttribDataBool* attrMyLoopableIn = parameters->getInputAttrib<AttribDataBool>(6, ATTRIB_SEMANTIC_LOOP);
  AttribDataSourceAnim* sourceAnim = parameters->getInputAttrib<AttribDataSourceAnim>(7, ATTRIB_SEMANTIC_SOURCE_ANIM);
  AttribDataSyncEventTrack* attrMySyncEventTrackIn = parameters->getInputAttrib<AttribDataSyncEventTrack>(8, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);

  if (attrParentUpdateTimePosIn->m_isAbs)
  {
    // Absolute from parent - Calculate this frames values.
    calcCurrentPlaybackValuesFromParentAbsTimeAdjSpace(
      attrMySyncEventTrackIn->m_syncEventTrack,
      attrParentUpdateTimePosIn,
      attrMyLoopableIn->m_value,
      attrMyFractionPosOut,
      attrMyTimePosOut,
      attrMyCurrentSyncEventPosOut);

    // Derive anim sample pos for this frame.
    calcCurrentAnimSamplePos(attrMyAnimSamplePosOut, attrMyTimePosOut, sourceAnim);
    attrMyAnimSamplePosOut->setWithAbs();
    attrMyAnimSamplePosOut->setInclusiveStart(true);

    // Cant loop when we are doing a straight set of our playback position.
    attrMyRealLoopedOnUpdateOut->m_value = false;
  }
  else
  {
    // Delta from parent - Calculate last and this frames values.
    AttribDataPlaybackPosInit* playbackPosInit = parameters->getOptionalInputAttrib<AttribDataPlaybackPosInit>(9, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT);
    subTaskUpdateDeltaTimeOnSyncEventsNode(
      attrMyPreviousSyncEventPosIn,
      attrMySyncEventTrackIn,
      attrMyLoopableIn->m_value,
      attrMyTimePosOut,
      attrMyCurrentSyncEventPosOut,
      attrMyFractionPosOut,
      attrParentUpdateTimePosIn,
      playbackPosInit,
      NULL);

    // Derive anim sample position this and the last frame.
    calcPreviousAnimSamplePos(attrMyAnimSamplePosOut, attrMyTimePosOut, sourceAnim);
    calcCurrentAnimSamplePos(attrMyAnimSamplePosOut, attrMyTimePosOut, sourceAnim);
    attrMyAnimSamplePosOut->setWithDelta();
    attrMyAnimSamplePosOut->setInclusiveStart(attrMyTimePosOut->m_inclusiveStart);

    // Determine if the real space pos has looped around.
    // Note that looping/non looping is resolved about the start event index i.e. 0 in adjusted space.
    attrMyRealLoopedOnUpdateOut->m_value = false;
    if (attrMyFractionPosOut->m_currentPosReal < attrMyFractionPosOut->m_previousPosReal)
      attrMyRealLoopedOnUpdateOut->m_value = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Updates are provided to us by our parent in our Adjusted space (parents Real space).
// All working is done internally in our Adjusted space.
// Provision to parent, conditions etc is in our Adjusted space.
// Real space is only used internally to calculate animation sampling position.
void TaskAnimSyncEventsUpdateSyncEventPos(Dispatcher::TaskParameters* parameters)
{
  AttribDataUpdateSyncEventPlaybackPos* attrParentUpdateSyncEventPosIn = parameters->getInputAttrib<AttribDataUpdateSyncEventPlaybackPos>(0, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataPlaybackPos* attrMyTimePosOut = parameters->createOutputAttribReplace<AttribDataPlaybackPos>(1, ATTRIB_SEMANTIC_TIME_POS);
  AttribDataUpdateSyncEventPlaybackPos* attrMyPreviousSyncEventPosIn = parameters->getOptionalInputAttrib<AttribDataUpdateSyncEventPlaybackPos>(2, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataUpdateSyncEventPlaybackPos* attrMyCurrentSyncEventPosOut = parameters->createOutputAttribReplace<AttribDataUpdateSyncEventPlaybackPos>(2, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataPlaybackPos* attrMyAnimSamplePosOut = parameters->createOutputAttribReplace<AttribDataPlaybackPos>(3, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS);
  AttribDataPlaybackPos* attrMyFractionPosOut = parameters->createOutputAttribReplace<AttribDataPlaybackPos>(4, ATTRIB_SEMANTIC_FRACTION_POS);
  AttribDataBool* attrMyRealLoopedOnUpdateOut = parameters->createOutputAttrib<AttribDataBool>(5, ATTRIB_SEMANTIC_LOOPED_ON_UPDATE);
  AttribDataBool* attrMyLoopableIn = parameters->getInputAttrib<AttribDataBool>(6, ATTRIB_SEMANTIC_LOOP);
  AttribDataSourceAnim* sourceAnim = parameters->getInputAttrib<AttribDataSourceAnim>(7, ATTRIB_SEMANTIC_SOURCE_ANIM);
  AttribDataSyncEventTrack* attrMySyncEventTrackIn = parameters->getInputAttrib<AttribDataSyncEventTrack>(8, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);

  if (attrParentUpdateSyncEventPosIn->m_isAbs)
  {
    // Absolute from parent - Calculate this frames values.
    calcCurrentPlaybackValuesFromParentAbsSyncEventPos(
      attrMySyncEventTrackIn->m_syncEventTrack,
      attrMyLoopableIn->m_value,
      attrParentUpdateSyncEventPosIn,
      attrMyCurrentSyncEventPosOut,
      attrMyFractionPosOut,
      attrMyTimePosOut);

    // Derive anim sample pos for this frame.
    calcCurrentAnimSamplePos(attrMyAnimSamplePosOut, attrMyTimePosOut, sourceAnim);
    attrMyAnimSamplePosOut->setWithAbs();
    attrMyAnimSamplePosOut->setInclusiveStart(true);

    // Cant loop when we are doing a straight set of our playback position.
    attrMyRealLoopedOnUpdateOut->m_value = false;
  }
  else
  {
    // Delta from parent - Calculate last and this frames values.
    AttribDataUpdateSyncEventPlaybackPos prevSyncEventPos;
    getPreviousFrameSyncEventPos(
      prevSyncEventPos,
      attrMyPreviousSyncEventPosIn,
      attrParentUpdateSyncEventPosIn,
      attrMySyncEventTrackIn,
      attrMyLoopableIn->m_value);

    // Calculate last frames values. Necessary as sync event track config is likely to have changed.
    calcPreviousPlaybackValuesFromMyAbsSyncEventPos(
      attrMySyncEventTrackIn->m_syncEventTrack,
      attrMyLoopableIn->m_value,
      &prevSyncEventPos,
      &prevSyncEventPos,
      attrMyFractionPosOut,
      attrMyTimePosOut);

    // Calculate this frames values.
    calcCurrentPlaybackValuesFromParentDeltaSyncEventPosAdjSpace(
      attrMySyncEventTrackIn->m_syncEventTrack,
      attrParentUpdateSyncEventPosIn,
      &prevSyncEventPos,
      attrMyLoopableIn->m_value,
      attrMyFractionPosOut,
      attrMyTimePosOut,
      attrMyCurrentSyncEventPosOut);

    // Derive anim sample position this and the last frame.
    calcPreviousAnimSamplePos(attrMyAnimSamplePosOut, attrMyTimePosOut, sourceAnim);
    calcCurrentAnimSamplePos(attrMyAnimSamplePosOut, attrMyTimePosOut, sourceAnim);
    attrMyAnimSamplePosOut->setWithDelta();
    attrMyTimePosOut->setInclusiveStart(!attrMyPreviousSyncEventPosIn);
    attrMyFractionPosOut->setInclusiveStart(!attrMyPreviousSyncEventPosIn);
    attrMyAnimSamplePosOut->setInclusiveStart(!attrMyPreviousSyncEventPosIn);

    // Determine if the real space pos has looped around.
    // Note that looping/non looping is resolved about the start event index i.e. 0 in adjusted space.
    attrMyRealLoopedOnUpdateOut->m_value = false;
    if (attrMyFractionPosOut->m_currentPosReal < attrMyFractionPosOut->m_previousPosReal)
      attrMyRealLoopedOnUpdateOut->m_value = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskInitSyncEventTrackFromDiscreteEventTrack(Dispatcher::TaskParameters* parameters)
{
  AttribDataSyncEventTrack* syncEventTrack = parameters->createOutputAttrib<AttribDataSyncEventTrack>(0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataSourceEventTrackSet* animSourceEventTracks = parameters->getInputAttrib<AttribDataSourceEventTrackSet>(1, ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS);
  AttribDataSourceAnim* sourceAnim = parameters->getInputAttrib<AttribDataSourceAnim>(2, ATTRIB_SEMANTIC_SOURCE_ANIM);
  AttribDataBool* loopable = parameters->getInputAttrib<AttribDataBool>(3, ATTRIB_SEMANTIC_LOOP);
  AttribDataPlaybackPosInit* playbackPosInit = parameters->getOptionalInputAttrib<AttribDataPlaybackPosInit>(4, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT);

#ifdef NM_HOST_CELL_SPU
  NMP_ASSERT(parameters->m_dispatcher);
  AttribDataSourceEventTrackSet::prepDiscreteEventTrackSetForSPU(animSourceEventTracks, parameters->m_dispatcher->getTempMemoryAllocator());
#endif // NM_HOST_CELL_SPU

  // Initialise the synchronisation event track from the appropriate discrete event track.
  NMP_ASSERT(sourceAnim->m_syncEventTrackIndex < animSourceEventTracks->m_numDiscreteEventTracks);

  // Add any adjust start sync event to the start sync event specified in the def.
  int32_t startSyncEvent = sourceAnim->m_startSyncEventIndex;
  if (playbackPosInit)
    startSyncEvent += playbackPosInit->m_adjustStartEventIndex;

  syncEventTrack->m_syncEventTrack.init(
    sourceAnim->m_clipStartFraction,
    (sourceAnim->m_clipEndFraction - sourceAnim->m_clipStartFraction),
    sourceAnim->m_clipStartSyncEventIndex,
    animSourceEventTracks->m_sourceDiscreteEventTracks[sourceAnim->m_syncEventTrackIndex],
    loopable->m_value,
    startSyncEvent,
    (sourceAnim->m_sourceAnimDuration * (sourceAnim->m_clipEndFraction - sourceAnim->m_clipStartFraction)),
    sourceAnim->m_playBackwards);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskInitUnitLengthSyncEventTrack(Dispatcher::TaskParameters* parameters)
{
  AttribDataSyncEventTrack* attribSyncEventTrack = parameters->createOutputAttrib<AttribDataSyncEventTrack>(0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);

  // Initialise a unit length synchronisation track with one event at the start running the length of the track.
  attribSyncEventTrack->m_syncEventTrack.wipeDown();
  attribSyncEventTrack->m_syncEventTrack.addEvent(0.0f, 1.0f);
  attribSyncEventTrack->m_syncEventTrack.setStartEventIndex(0);
  attribSyncEventTrack->m_syncEventTrack.setDuration(1.0f);
  attribSyncEventTrack->m_syncEventTrack.setDurationReciprocal(1.0f);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskInitSampledEventsBuffer(Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();
  AttribDataSampledEventsCreateDesc desc(0, 0);
  AttribDataSampledEvents* outputEventsBuffer = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);
  (void)outputEventsBuffer;
}

//----------------------------------------------------------------------------------------------------------------------
void calculateTrueEventSamplingTimeFractions(
  AttribDataPlaybackPos* fractionTimePos,
  AttribDataSourceAnim*  sourceAnim,
  float&                 trueSamplePrevFraction,
  float&                 trueSampleCurrentFraction,
  float&                 trueSampleDeltaFraction)
{
  NMP_ASSERT(fractionTimePos && sourceAnim);
  trueSamplePrevFraction = fractionTimePos->m_previousPosReal;
  trueSampleCurrentFraction = fractionTimePos->m_currentPosReal;
  float clipDurationFraction = (sourceAnim->m_clipEndFraction - sourceAnim->m_clipStartFraction);
  trueSampleDeltaFraction = NMP::minimum(clipDurationFraction * fractionTimePos->m_delta, 1.0f);
  if (sourceAnim->m_playBackwards)
  {
    trueSampleCurrentFraction = 1.0f - trueSampleCurrentFraction;
    trueSamplePrevFraction = 1.0f - trueSamplePrevFraction;
  }
  trueSamplePrevFraction = sourceAnim->m_clipStartFraction + (trueSamplePrevFraction * clipDurationFraction);
  trueSampleCurrentFraction = sourceAnim->m_clipStartFraction + (trueSampleCurrentFraction * clipDurationFraction);

  NMP_ASSERT((trueSampleDeltaFraction >= 0.0f) && (trueSampleDeltaFraction <= 1.0f));
}

//----------------------------------------------------------------------------------------------------------------------
void findTriggeredDiscreteEvents(
  uint32_t&                             numSampledDiscreteTracks,         // OUT
  SampledDiscreteTrack*                 sampledDiscreteTracks,            // OUT
  uint32_t&                             totalNumTriggeredDiscreteEvents,  // OUT
  EventDefDiscrete**                    triggeredDiscreteEvents,          // OUT
  const AttribDataSourceEventTrackSet*  sourceDiscreteEventTracks,        // IN
  const AttribDataSourceAnim*           sourceAnim,                       // IN
  bool                                  allowLooping,                     // IN
  float                                 sampleStartFraction,              // IN
  float                                 sampleDeltaFraction,              // IN
  bool                                  inclusiveStart)                   // IN
{
  NMP_ASSERT(sampledDiscreteTracks && triggeredDiscreteEvents && sourceDiscreteEventTracks && sourceAnim);

  totalNumTriggeredDiscreteEvents = 0;
  numSampledDiscreteTracks = 0;

  for (uint32_t i = 0; i < sourceDiscreteEventTracks->m_numDiscreteEventTracks; ++i)
  {
    const EventTrackDefDiscrete* currentSourceTrack = sourceDiscreteEventTracks->m_sourceDiscreteEventTracks[i];
    NMP_ASSERT(currentSourceTrack);
    NMP_ASSERT(MAX_NUM_DISCRETE_EVENTS > totalNumTriggeredDiscreteEvents);
    uint32_t numTriggeredDiscreteEvents = currentSourceTrack->findEventsBetweenTimes(
                                            sampleStartFraction,                          // timeFrom.
                                            sourceAnim->m_playBackwards,                  // sampleForwards.
                                            sampleDeltaFraction,                          // sampleDuration.
                                            allowLooping,                                 // allowWrapping.
                                            &(triggeredDiscreteEvents[totalNumTriggeredDiscreteEvents]), // Passed in to be filled.
                                            MAX_NUM_DISCRETE_EVENTS - totalNumTriggeredDiscreteEvents,        // Num slots remaining for found events.
                                            sourceAnim->m_clipStartFraction,
                                            sourceAnim->m_clipEndFraction,
                                            inclusiveStart);

    if (numTriggeredDiscreteEvents > 0)
    {
      sampledDiscreteTracks[numSampledDiscreteTracks].m_eventsArrayStartIndex = totalNumTriggeredDiscreteEvents;
      sampledDiscreteTracks[numSampledDiscreteTracks].m_runtimeID = currentSourceTrack->getTrackID();
      sampledDiscreteTracks[numSampledDiscreteTracks].m_numTriggeredEvents = numTriggeredDiscreteEvents;
      sampledDiscreteTracks[numSampledDiscreteTracks].m_userData = currentSourceTrack->getUserData();
      totalNumTriggeredDiscreteEvents += numTriggeredDiscreteEvents;
      ++numSampledDiscreteTracks;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskSampleEventsFromSourceTracks(Dispatcher::TaskParameters* parameters)
{
  AttribDataSourceEventTrackSet* sourceDiscreteEventTracks = parameters->getInputAttrib<AttribDataSourceEventTrackSet>(1, ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS);
  AttribDataPlaybackPos* fractionTimePos = parameters->getInputAttrib<AttribDataPlaybackPos>(2, ATTRIB_SEMANTIC_FRACTION_POS);
  AttribDataSourceAnim* sourceAnim = parameters->getInputAttrib<AttribDataSourceAnim>(3, ATTRIB_SEMANTIC_SOURCE_ANIM);
  AttribDataBool* loopable = parameters->getInputAttrib<AttribDataBool>(4, ATTRIB_SEMANTIC_LOOP);

#ifdef NM_HOST_CELL_SPU
  NMP_ASSERT(parameters->m_dispatcher);
  AttribDataSourceEventTrackSet::prepDiscreteEventTrackSetForSPU(sourceDiscreteEventTracks, parameters->m_dispatcher->getTempMemoryAllocator());
  AttribDataSourceEventTrackSet::prepCurveEventTrackSetForSPU(sourceDiscreteEventTracks, parameters->m_dispatcher->getTempMemoryAllocator());
#endif // NM_HOST_CELL_SPU

  //-----------------------------
  // Work out what the true fractions are.
  float trueSampleStartFraction;
  float trueSampleEndFraction;
  float trueSampleDeltaFraction;
  calculateTrueEventSamplingTimeFractions(
    fractionTimePos,
    sourceAnim,
    trueSampleStartFraction,
    trueSampleEndFraction,
    trueSampleDeltaFraction);

  //-----------------------------
  // Find triggered discrete events.
  uint32_t totalNumTriggeredDiscreteEvents = 0;
  EventDefDiscrete* triggeredDiscreteEvents[MAX_NUM_DISCRETE_EVENTS];
  uint32_t numSampledDiscreteTracks = 0;
  SampledDiscreteTrack sampledDiscreteTracks[MAX_NUM_DISCRETE_EVENTS];
  findTriggeredDiscreteEvents(
    numSampledDiscreteTracks,
    sampledDiscreteTracks,
    totalNumTriggeredDiscreteEvents,
    triggeredDiscreteEvents,
    sourceDiscreteEventTracks,
    sourceAnim,
    loopable->m_value,
    trueSampleStartFraction,
    trueSampleDeltaFraction,
    fractionTimePos->m_inclusiveStart);

  //-----------------------------
  // Now we know how many events we will need to output we can create our output events buffer.
  AttribDataSampledEventsCreateDesc desc(totalNumTriggeredDiscreteEvents, sourceDiscreteEventTracks->m_numCurveEventTracks);
  AttribDataSampledEvents* sampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);

  //-----------------------------
  // Fill in triggered discrete events buffer.
  uint32_t j = 0;
  for (uint32_t i = 0; i < numSampledDiscreteTracks; ++i)
  {
    for (uint32_t k = 0; k < sampledDiscreteTracks[i].m_numTriggeredEvents; ++k, ++j)
    {
      sampledEvents->m_discreteBuffer->setEvent(
        j,
        sampledDiscreteTracks[i].m_userData,
        sampledDiscreteTracks[i].m_runtimeID,
        triggeredDiscreteEvents[j]->getUserData());
    }
  }

  //-----------------------------
  // Sample curve events.
  for (uint32_t i = 0; i < sourceDiscreteEventTracks->m_numCurveEventTracks; ++i)
  {
    EventTrackDefCurve* currentSourceTrack = sourceDiscreteEventTracks->m_sourceCurveEventTracks[i];
    uint32_t runtimeTrackID = currentSourceTrack->getTrackID();
    NMP_ASSERT(sampledEvents->m_curveBuffer);
    sampledEvents->m_curveBuffer->sampleCurveTrackAndAddToBuffer(
      currentSourceTrack,     // Events track to sample from.
      trueSampleEndFraction,  // What point to sample at.
      runtimeTrackID);        // The runtime ID of the source event track.
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCombine2SampledEventsBuffers(Dispatcher::TaskParameters* parameters)
{
  AttribDataSampledEvents* source0SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents* source1SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(2, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(3, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  
  NMP_ASSERT(blendWeights->m_sampledEventsNumWeights == 1);
  NMP_ASSERT(blendWeights->m_sampledEventsWeights[0] >= 0.0f && blendWeights->m_sampledEventsWeights[0] <= 1.0f);

  NMP_ASSERT(
    source0SampledEventsBuffer->m_curveBuffer &&
    source1SampledEventsBuffer->m_curveBuffer &&
    source0SampledEventsBuffer->m_discreteBuffer &&
    source1SampledEventsBuffer->m_discreteBuffer);

  // Combine both input buffers into one.
  uint32_t totalNumTriggeredDiscreteEvents =
    source0SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents() +
    source1SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents();
  uint32_t totalNumCurveEventSamples =
    source0SampledEventsBuffer->m_curveBuffer->getNumSampledEvents() +
    source1SampledEventsBuffer->m_curveBuffer->getNumSampledEvents();
  AttribDataSampledEventsCreateDesc desc(totalNumTriggeredDiscreteEvents, totalNumCurveEventSamples);
  AttribDataSampledEvents* sampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);

  float weight = 1.0f - blendWeights->m_sampledEventsWeights[0];
  sampledEvents->m_discreteBuffer->combine(source0SampledEventsBuffer->m_discreteBuffer, source1SampledEventsBuffer->m_discreteBuffer, weight);
  sampledEvents->m_curveBuffer->combine(source0SampledEventsBuffer->m_curveBuffer, source1SampledEventsBuffer->m_curveBuffer, weight);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskAdd2SampledEventsBuffers(Dispatcher::TaskParameters* parameters)
{
  AttribDataSampledEvents* source0SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents* source1SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(2, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(3, ATTRIB_SEMANTIC_BLEND_WEIGHTS);

  NMP_ASSERT(blendWeights->m_sampledEventsNumWeights == 1);
  NMP_ASSERT(blendWeights->m_sampledEventsWeights[0] >= 0.0f && blendWeights->m_sampledEventsWeights[0] <= 1.0f);

  NMP_ASSERT(
    source0SampledEventsBuffer->m_curveBuffer &&
    source1SampledEventsBuffer->m_curveBuffer &&
    source0SampledEventsBuffer->m_discreteBuffer &&
    source1SampledEventsBuffer->m_discreteBuffer);

  // Grab our events for this frame
  uint32_t totalNumTriggeredDiscreteEvents =
    source0SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents() +
    source1SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents();
  uint32_t totalNumCurveEventSamples =
    source0SampledEventsBuffer->m_curveBuffer->getNumSampledEvents() +
    source1SampledEventsBuffer->m_curveBuffer->getNumSampledEvents();
  AttribDataSampledEventsCreateDesc desc(totalNumTriggeredDiscreteEvents, totalNumCurveEventSamples);
  AttribDataSampledEvents* sampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);

  // Add both sources discrete event buffers together, preserve blend weight for source 0 and attenuate for source 1.
  sampledEvents->m_discreteBuffer->additiveCombine(
    source0SampledEventsBuffer->m_discreteBuffer, 
    source1SampledEventsBuffer->m_discreteBuffer, 
    blendWeights->m_sampledEventsWeights[0]);
  
  // And add each of our curve event buffers into our output buffer, preserve blend weight for source 0 and attenuate for source 1.
  sampledEvents->m_curveBuffer->fill(source0SampledEventsBuffer->m_curveBuffer);
  uint32_t startingIndex = sampledEvents->m_curveBuffer->getNumSampledEvents();
  sampledEvents->m_curveBuffer->fillAtAndScale(startingIndex, source1SampledEventsBuffer->m_curveBuffer, blendWeights->m_sampledEventsWeights[0]);
}

//----------------------------------------------------------------------------------------------------------------------

void TaskCombine2x2SampledEventsBuffers(Dispatcher::TaskParameters* parameters)
{
  AttribDataSampledEvents* source0SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents* source1SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(2, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents* source2SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(3, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents* source3SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(4, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(5, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  
  NMP_ASSERT(blendWeights->m_sampledEventsNumWeights == 2);
  NMP_ASSERT(blendWeights->m_sampledEventsWeights[0] >= 0.0f && blendWeights->m_sampledEventsWeights[0] <= 1.0f);
  NMP_ASSERT(blendWeights->m_sampledEventsWeights[1] >= 0.0f && blendWeights->m_sampledEventsWeights[1] <= 1.0f);

  NMP_ASSERT(
    source0SampledEventsBuffer->m_curveBuffer &&
    source1SampledEventsBuffer->m_curveBuffer &&
    source2SampledEventsBuffer->m_curveBuffer &&
    source3SampledEventsBuffer->m_curveBuffer &&
    source0SampledEventsBuffer->m_discreteBuffer &&
    source1SampledEventsBuffer->m_discreteBuffer &&
    source2SampledEventsBuffer->m_discreteBuffer &&
    source3SampledEventsBuffer->m_discreteBuffer);

  // Combine both input buffers into one.
  uint32_t totalNumTriggeredDiscreteEvents =
    source0SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents() +
    source1SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents() +
    source2SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents() +
    source3SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents();
  uint32_t totalNumCurveEventSamples =
    source0SampledEventsBuffer->m_curveBuffer->getNumSampledEvents() +
    source1SampledEventsBuffer->m_curveBuffer->getNumSampledEvents() +
    source2SampledEventsBuffer->m_curveBuffer->getNumSampledEvents() +
    source3SampledEventsBuffer->m_curveBuffer->getNumSampledEvents();
  AttribDataSampledEventsCreateDesc desc(totalNumTriggeredDiscreteEvents, totalNumCurveEventSamples);
  AttribDataSampledEvents* sampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);

  float omx = 1.0f - blendWeights->m_sampledEventsWeights[0];
  float omy = 1.0f - blendWeights->m_sampledEventsWeights[1];
  const float weights[4] =
  {
    omy * omx,
    omy * blendWeights->m_sampledEventsWeights[0],
    blendWeights->m_sampledEventsWeights[1]  * omx,
    blendWeights->m_sampledEventsWeights[1] * blendWeights->m_sampledEventsWeights[0]
  };

  {
    uint32_t curIdx = 0;
    sampledEvents->m_discreteBuffer->fillAtAndScale(curIdx, source0SampledEventsBuffer->m_discreteBuffer, weights[0]);
    curIdx += source0SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents();
    sampledEvents->m_discreteBuffer->fillAtAndScale(curIdx, source1SampledEventsBuffer->m_discreteBuffer, weights[1]);
    curIdx += source1SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents();
    sampledEvents->m_discreteBuffer->fillAtAndScale(curIdx, source2SampledEventsBuffer->m_discreteBuffer, weights[2]);
    curIdx += source2SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents();
    sampledEvents->m_discreteBuffer->fillAtAndScale(curIdx, source3SampledEventsBuffer->m_discreteBuffer, weights[3]);
  }

  {
    uint32_t curIdx = 0;
    sampledEvents->m_curveBuffer->fillAtAndScale(curIdx, source0SampledEventsBuffer->m_curveBuffer, weights[0]);
    curIdx += source0SampledEventsBuffer->m_curveBuffer->getNumSampledEvents();
    sampledEvents->m_curveBuffer->fillAtAndScale(curIdx, source1SampledEventsBuffer->m_curveBuffer, weights[1]);
    curIdx += source1SampledEventsBuffer->m_curveBuffer->getNumSampledEvents();
    sampledEvents->m_curveBuffer->fillAtAndScale(curIdx, source2SampledEventsBuffer->m_curveBuffer, weights[2]);
    curIdx += source2SampledEventsBuffer->m_curveBuffer->getNumSampledEvents();
    sampledEvents->m_curveBuffer->fillAtAndScale(curIdx, source3SampledEventsBuffer->m_curveBuffer, weights[3]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskTriangleCombineSampledEventsBuffers(Dispatcher::TaskParameters* parameters)
{
  AttribDataSampledEvents* source0SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents* source1SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(2, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents* source2SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(3, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(4, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  
  NMP_ASSERT(blendWeights->m_eventsNumWeights == 2);
  NMP_ASSERT(blendWeights->m_eventsWeights[0] >= 0.0f && blendWeights->m_eventsWeights[0] <= 1.0f);
  NMP_ASSERT(blendWeights->m_eventsWeights[1] >= 0.0f && blendWeights->m_eventsWeights[1] <= 1.0f);

  NMP_ASSERT(
    source0SampledEventsBuffer->m_curveBuffer &&
    source1SampledEventsBuffer->m_curveBuffer &&
    source2SampledEventsBuffer->m_curveBuffer &&
    source0SampledEventsBuffer->m_discreteBuffer &&
    source1SampledEventsBuffer->m_discreteBuffer &&
    source2SampledEventsBuffer->m_discreteBuffer);

  // Combine both input buffers into one.
  uint32_t totalNumTriggeredDiscreteEvents =
    source0SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents() +
    source1SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents() +
    source2SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents();
  uint32_t totalNumCurveEventSamples =
    source0SampledEventsBuffer->m_curveBuffer->getNumSampledEvents() +
    source1SampledEventsBuffer->m_curveBuffer->getNumSampledEvents() +
    source2SampledEventsBuffer->m_curveBuffer->getNumSampledEvents();
  AttribDataSampledEventsCreateDesc desc(totalNumTriggeredDiscreteEvents, totalNumCurveEventSamples);
  AttribDataSampledEvents* sampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);

  float omx = 1.0f - blendWeights->m_eventsWeights[0];
  float omy = 1.0f - blendWeights->m_eventsWeights[1];
  const float weights[3] =
  {
    omy * omx,
    omy * blendWeights->m_eventsWeights[0],
    blendWeights->m_eventsWeights[1]
  };

  {
    uint32_t curIdx = 0;
    sampledEvents->m_discreteBuffer->fillAtAndScale(curIdx, source0SampledEventsBuffer->m_discreteBuffer, weights[0]);
    curIdx += source0SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents();
    sampledEvents->m_discreteBuffer->fillAtAndScale(curIdx, source1SampledEventsBuffer->m_discreteBuffer, weights[1]);
    curIdx += source1SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents();
    sampledEvents->m_discreteBuffer->fillAtAndScale(curIdx, source2SampledEventsBuffer->m_discreteBuffer, weights[2]);
  }

  {
    uint32_t curIdx = 0;
    sampledEvents->m_curveBuffer->fillAtAndScale(curIdx, source0SampledEventsBuffer->m_curveBuffer, weights[0]);
    curIdx += source0SampledEventsBuffer->m_curveBuffer->getNumSampledEvents();
    sampledEvents->m_curveBuffer->fillAtAndScale(curIdx, source1SampledEventsBuffer->m_curveBuffer, weights[1]);
    curIdx += source1SampledEventsBuffer->m_curveBuffer->getNumSampledEvents();
    sampledEvents->m_curveBuffer->fillAtAndScale(curIdx, source2SampledEventsBuffer->m_curveBuffer, weights[2]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCombineAllSampledEventsBuffers(Dispatcher::TaskParameters* parameters)
{
  // Get temp allocator.
  NMP::TempMemoryAllocator* tempAllocator = parameters->m_dispatcher->getTempMemoryAllocator();
  NMP_ASSERT(tempAllocator);

  uint32_t parameterIndex = 0;
  // Get the number of connected inputs
  uint32_t numConnections = parameters->m_numParameters - 2;

  // Source sampled events buffer
  NMP::Memory::Format memReq(sizeof(AttribDataSampledEvents*) * numConnections, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memRes = NMPAllocatorAllocateFromFormat(tempAllocator, memReq);
  AttribDataSampledEvents** sourceSampledEvents = (AttribDataSampledEvents**)memRes.ptr;
  for (uint32_t i = 0; i < numConnections; ++i)
  {
    sourceSampledEvents[i] = parameters->getInputAttrib<AttribDataSampledEvents>(++parameterIndex, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
    NMP_ASSERT( sourceSampledEvents[i]->m_curveBuffer );
  };

  // Blend weights
  AttribDataFloatArray* weights = parameters->getInputAttrib<AttribDataFloatArray>(++parameterIndex, ATTRIB_SEMANTIC_CP_FLOAT);

  // Combine input buffers into one.
  uint32_t totalNumTriggeredDiscreteEvents = 0;
  uint32_t totalNumCurveEventSamples = 0;
  for (uint32_t i = 0; i < numConnections; ++i)
  {
    totalNumTriggeredDiscreteEvents += sourceSampledEvents[i]->m_discreteBuffer->getNumTriggeredEvents();
    totalNumCurveEventSamples += sourceSampledEvents[i]->m_curveBuffer->getNumSampledEvents();
  }

  // Output sampled events buffer
  AttribDataSampledEventsCreateDesc desc(totalNumTriggeredDiscreteEvents, totalNumCurveEventSamples);
  AttribDataSampledEvents* sampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);

  uint32_t curDiscreteIdx = 0;
  uint32_t curCurveIdx = 0;
  for (uint32_t i = 0; i<numConnections; ++i)
  {
    sampledEvents->m_discreteBuffer->fillAtAndScale(curDiscreteIdx, sourceSampledEvents[i]->m_discreteBuffer, 1.0f - weights->m_values[i]);
    curDiscreteIdx += sourceSampledEvents[i]->m_discreteBuffer->getNumTriggeredEvents();

    sampledEvents->m_curveBuffer->fillAtAndScale(curCurveIdx, sourceSampledEvents[i]->m_curveBuffer, 1.0f - weights->m_values[i]);
    curCurveIdx += sourceSampledEvents[i]->m_curveBuffer->getNumSampledEvents();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2SyncEventTracks(Dispatcher::TaskParameters* parameters)
{
  AttribDataSyncEventTrack* mySyncEventTrack = parameters->createOutputAttribReplace<AttribDataSyncEventTrack>(0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataSyncEventTrack* source0SyncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataSyncEventTrack* source1SyncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(2, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataUInt* startSyncEventIndex = parameters->getInputAttrib<AttribDataUInt>(3, ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(4, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  AttribDataPlaybackPosInit* playbackPosInit = parameters->getOptionalInputAttrib<AttribDataPlaybackPosInit>(5, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT);

  // Interpolate blend event clips.
  NMP_ASSERT(blendWeights->m_eventsNumWeights == 1);
  NMP_ASSERT(blendWeights->m_eventsWeights[0] >= 0.0f && blendWeights->m_eventsWeights[0] <= 1.0f);
  mySyncEventTrack->m_syncEventTrack.blendClipTracks(
    &source0SyncEventTrack->m_syncEventTrack,
    &source1SyncEventTrack->m_syncEventTrack,
    blendWeights->m_eventsWeights[0]);

  // Add any adjust start sync event to the start sync event specified in the def.
  int32_t startSyncEvent = startSyncEventIndex->m_value;
  if (playbackPosInit)
    startSyncEvent += playbackPosInit->m_adjustStartEventIndex;

  mySyncEventTrack->m_syncEventTrack.setStartEventIndex(startSyncEvent);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2x2SyncEventTracks(Dispatcher::TaskParameters* parameters)
{
  AttribDataSyncEventTrack* mySyncEventTrack = parameters->createOutputAttribReplace<AttribDataSyncEventTrack>(0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataSyncEventTrack* source0SyncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataSyncEventTrack* source1SyncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(2, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataSyncEventTrack* source2SyncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataSyncEventTrack* source3SyncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(4, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataUInt* startSyncEventIndex = parameters->getInputAttrib<AttribDataUInt>(5, ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(6, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  AttribDataPlaybackPosInit* playbackPosInit = parameters->getOptionalInputAttrib<AttribDataPlaybackPosInit>(7, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT);

  NMP_ASSERT(blendWeights->m_eventsNumWeights == 2);
  NMP_ASSERT(blendWeights->m_eventsWeights[0] >= 0.0f && blendWeights->m_eventsWeights[0] <= 1.0f);
  NMP_ASSERT(blendWeights->m_eventsWeights[1] >= 0.0f && blendWeights->m_eventsWeights[1] <= 1.0f);

  // Interpolate 4 blend event clips.
  EventTrackSync temp0SyncEventTrack, temp1SyncEventTrack;
  temp0SyncEventTrack.blendClipTracks(
    &source0SyncEventTrack->m_syncEventTrack,
    &source1SyncEventTrack->m_syncEventTrack,
    blendWeights->m_eventsWeights[0]);
  temp1SyncEventTrack.blendClipTracks(
    &source2SyncEventTrack->m_syncEventTrack,
    &source3SyncEventTrack->m_syncEventTrack,
    blendWeights->m_eventsWeights[0]);
  mySyncEventTrack->m_syncEventTrack.blendClipTracks(
    &temp0SyncEventTrack,
    &temp1SyncEventTrack,
    blendWeights->m_eventsWeights[1]);

  // Add any adjust start sync event to the start sync event specified in the def.
  int32_t startSyncEvent = startSyncEventIndex->m_value;
  if (playbackPosInit)
    startSyncEvent += playbackPosInit->m_adjustStartEventIndex;

  mySyncEventTrack->m_syncEventTrack.setStartEventIndex(startSyncEvent);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskTriangleBlendSyncEventTracks(Dispatcher::TaskParameters* parameters)
{
  AttribDataSyncEventTrack* outputSyncEventTrack = parameters->createOutputAttribReplace<AttribDataSyncEventTrack>(0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataSyncEventTrack* source0SyncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataSyncEventTrack* source1SyncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(2, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataSyncEventTrack* source2SyncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataUInt* startSyncEventIndex = parameters->getInputAttrib<AttribDataUInt>(4, ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(5, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  AttribDataPlaybackPosInit* playbackPosInit = parameters->getOptionalInputAttrib<AttribDataPlaybackPosInit>(6, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT);

  NMP_ASSERT(blendWeights->m_eventsNumWeights == 2);
  NMP_ASSERT(blendWeights->m_eventsWeights[0] >= 0.0f && blendWeights->m_eventsWeights[0] <= 1.0f);
  NMP_ASSERT(blendWeights->m_eventsWeights[1] >= 0.0f && blendWeights->m_eventsWeights[1] <= 1.0f);

  // Interpolate 3 blend event clips.
  EventTrackSync tempSyncEventTrack;

  tempSyncEventTrack.blendClipTracks(
    &source0SyncEventTrack->m_syncEventTrack,
    &source1SyncEventTrack->m_syncEventTrack,
    blendWeights->m_eventsWeights[0]);

  outputSyncEventTrack->m_syncEventTrack.blendClipTracks(
    &tempSyncEventTrack,
    &source2SyncEventTrack->m_syncEventTrack,
    blendWeights->m_eventsWeights[1]);

  // Add any adjust start sync event to the start sync event specified in the def.
  int32_t startSyncEvent = startSyncEventIndex->m_value;
  if (playbackPosInit)
    startSyncEvent += playbackPosInit->m_adjustStartEventIndex;

  outputSyncEventTrack->m_syncEventTrack.setStartEventIndex(startSyncEvent);
}

#ifdef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void TaskBlendAllSyncEventTracks(Dispatcher::TaskParameters* parameters)
{
  uint32_t dmaTags = 0;
  // Number of connections
  uint32_t numConnections = parameters->m_numParameters - 4;

  MR::DispatcherSPU* dispatcher = (MR::DispatcherSPU*)parameters->m_dispatcher;
  NMP::MemoryAllocator* tempAllocator = dispatcher->getTempMemoryAllocator();

  //-----------------------------
  // Output sync event track
  AttribDataSyncEventTrack* mySyncEventTrack = parameters->createOutputAttribReplace<AttribDataSyncEventTrack>(0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  
  //-----------------------------
  // Start sync event index
  // Load the first transform buffer from main memory
  dmaTags = FetchParameterHelper(&parameters->m_parameters[1], tempAllocator);
  NMP::SPUDMAController::dmaWait(dmaTags);
  AttribDataUInt* startSyncEventIndex = parameters->getInputAttrib<AttribDataUInt>(1, ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX);

  //-----------------------------
  // Load the first Sync Event Track from main memory
  dmaTags = FetchParameterHelper(&parameters->m_parameters[2], tempAllocator);
  NMP::SPUDMAController::dmaWait(dmaTags);
  AttribDataSyncEventTrack* accumulatedSyncTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(2, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);

  //-----------------------------
  // Get the normalised blend weights
  dmaTags = FetchParameterHelper(&parameters->m_parameters[numConnections + 2], tempAllocator);
  NMP::SPUDMAController::dmaWait(dmaTags);
  AttribDataFloatArray* weights = parameters->getInputAttrib<AttribDataFloatArray>(numConnections + 2, ATTRIB_SEMANTIC_CP_FLOAT);
  AttribDataFloatArray::relocate(weights);

  
  //-----------------------------
  // Playback pos init
  dmaTags = FetchParameterHelper(&parameters->m_parameters[numConnections + 3], tempAllocator);
  NMP::SPUDMAController::dmaWait(dmaTags);
  AttribDataPlaybackPosInit* playbackPosInit = parameters->getInputAttrib<AttribDataPlaybackPosInit>(numConnections+3, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT);

  float weight = 0.0f;
  float accumulatedWeight = weights->m_values[0];
  // Pass thorough the sync event track if there is just one active input
  if (accumulatedWeight == 1.0f)
  {
    mySyncEventTrack->m_syncEventTrack = accumulatedSyncTrack->m_syncEventTrack;
  }
  else
  {
    uint32_t paramIndex = 3;
    AttribDataSyncEventTrack* nextSyncTrack;
    NMP::Memory::Resource attrResource = NMPAllocatorAllocateFromFormat(tempAllocator, parameters->m_parameters[paramIndex].m_attribDataHandle.m_format);
    for (uint32_t i=0; i < numConnections - 1; ++i)
    {
      //-----------------------------
      // Load next transform buffer, but not allocate new memory for it
      dmaTags = NMP::SPUDMAController::dmaGet(
        attrResource.ptr, 
        (uint32_t)(parameters->m_parameters[paramIndex].m_attribDataHandle.m_attribData), 
        parameters->m_parameters[paramIndex].m_attribDataHandle.m_format.size);
      NMP::SPUDMAController::dmaWait(dmaTags);
      parameters->m_parameters[paramIndex].m_attribDataHandle.m_attribData = (MR::AttribData*)(attrResource.ptr);
      nextSyncTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(paramIndex, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);  
      //-----------------------------
      // normalised blend weight
      weight = accumulatedWeight / (accumulatedWeight + weights->m_values[i+1]);
      //-----------------------------
      // perform the actual blend
      mySyncEventTrack->m_syncEventTrack.blendClipTracks(
        &accumulatedSyncTrack->m_syncEventTrack,
        &nextSyncTrack->m_syncEventTrack,
        1.0f - weight);

      accumulatedSyncTrack->m_syncEventTrack = mySyncEventTrack->m_syncEventTrack;
      accumulatedWeight += weights->m_values[i+1];
      ++paramIndex;
    }
  }

  //-----------------------------
  // Add any adjust start sync event to the start sync event specified in the def.
  int32_t startSyncEvent = startSyncEventIndex->m_value;
  if (playbackPosInit)
    startSyncEvent += playbackPosInit->m_adjustStartEventIndex;

  mySyncEventTrack->m_syncEventTrack.setStartEventIndex(startSyncEvent);
}
#else
//----------------------------------------------------------------------------------------------------------------------
void TaskBlendAllSyncEventTracks(Dispatcher::TaskParameters* parameters)
{
  // Output sync event track
  AttribDataSyncEventTrack* mySyncEventTrack = parameters->createOutputAttribReplace<AttribDataSyncEventTrack>(0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);

  // Start sync event index
  AttribDataUInt* startSyncEventIndex = parameters->getInputAttrib<AttribDataUInt>(1, ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX);

  // Store a roll back point on the temp allocator to restore back to after we have done our local working.
  NMP::TempMemoryAllocator* tempAllocator = parameters->m_dispatcher->getTempMemoryAllocator();
  NMP_ASSERT(tempAllocator);
  NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

  // Number child input connections.
  uint32_t numConnections = parameters->m_numParameters - 4;
  
  // Get the input sync tracks and store in an array.
  NMP::Memory::Format memReq(sizeof(AttribDataSyncEventTrack*) * numConnections, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memRes = NMPAllocatorAllocateFromFormat(childAllocator, memReq);
  AttribDataSyncEventTrack** sourceSyncEventTracks = (AttribDataSyncEventTrack**)memRes.ptr;
  uint32_t param = 2;
  for (uint32_t i = 0; i < numConnections; ++i)
  {
    sourceSyncEventTracks[i] = parameters->getInputAttrib<AttribDataSyncEventTrack>(param, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
    ++param;
  }
  // Get the normalised blend weights
  AttribDataFloatArray* weights = parameters->getInputAttrib<AttribDataFloatArray>(param, ATTRIB_SEMANTIC_CP_FLOAT);
  ++param;

  // Playback pos init
  AttribDataPlaybackPosInit* playbackPosInit = parameters->getOptionalInputAttrib<AttribDataPlaybackPosInit>(param, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT);

  float weight = 0.0f;
  float accumulatedWeight = weights->m_values[0];
  if (accumulatedWeight == 1.0f)
  {
    mySyncEventTrack->m_syncEventTrack = sourceSyncEventTracks[0]->m_syncEventTrack;
  }
  else
  {
    // Temp sync event track.
    NMP::Memory::Format memReqTempTrack(sizeof(AttribDataSyncEventTrack), NMP_VECTOR_ALIGNMENT);
    NMP::Memory::Resource memResTempTrack = NMPAllocatorAllocateFromFormat(childAllocator, memReqTempTrack);
    AttribDataSyncEventTrack* tempTrack = (AttribDataSyncEventTrack*)memResTempTrack.ptr;
    tempTrack->m_syncEventTrack = sourceSyncEventTracks[0]->m_syncEventTrack;
    for (uint32_t i=0; i < numConnections - 1; ++i)
    {
      // Normalised blend weight.
      weight = accumulatedWeight / (accumulatedWeight + weights->m_values[i+1]);

      // Perform the actual blend.
      mySyncEventTrack->m_syncEventTrack.blendClipTracks(
        &tempTrack->m_syncEventTrack,
        &sourceSyncEventTracks[i+1]->m_syncEventTrack,
        1.0f - weight);

      tempTrack->m_syncEventTrack = mySyncEventTrack->m_syncEventTrack;
      accumulatedWeight += weights->m_values[i+1];
    }
    childAllocator->memFree(tempTrack);
  }

  // Roll back temp working memory.
  childAllocator->memFree(sourceSyncEventTracks);
  tempAllocator->destroyChildAllocator(childAllocator);

  // Add any adjust start sync event to the start sync event specified in the def.
  int32_t startSyncEvent = startSyncEventIndex->m_value;
  if (playbackPosInit)
    startSyncEvent += playbackPosInit->m_adjustStartEventIndex;

  mySyncEventTrack->m_syncEventTrack.setStartEventIndex(startSyncEvent);
}
#endif

//----------------------------------------------------------------------------------------------------------------------
// Updates are provided to us by our parent in our Adjusted space (parents Real space).
// All working is done internally in our Adjusted space.
// Provision to parent, conditions etc is in our Adjusted space.
// Provision to children is in our Real space.
void TaskBlend2SyncEventsUpdateTimeViaTimePos(Dispatcher::TaskParameters* parameters)
{
  AttribDataUpdatePlaybackPos* attrParentUpdateTimePosIn = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(0, ATTRIB_SEMANTIC_UPDATE_TIME_POS);
  AttribDataPlaybackPos* attrMyTimePosOut = parameters->createOutputAttribReplace<AttribDataPlaybackPos>(1, ATTRIB_SEMANTIC_TIME_POS);
  AttribDataPlaybackPos* attrMyFractionPosOut = parameters->createOutputAttribReplace<AttribDataPlaybackPos>(2, ATTRIB_SEMANTIC_FRACTION_POS);
  AttribDataBool* attrMyLoopableIn = parameters->getInputAttrib<AttribDataBool>(3, ATTRIB_SEMANTIC_LOOP);
  AttribDataSyncEventTrack* attrMySyncEventTrackIn = parameters->getInputAttrib<AttribDataSyncEventTrack>(4, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataUpdateSyncEventPlaybackPos* attrMyPreviousSyncEventPosIn = parameters->getInputAttrib<AttribDataUpdateSyncEventPlaybackPos>(5, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataUpdateSyncEventPlaybackPos* attrMyCurrentSyncEventPosOut = parameters->createOutputAttribReplace<AttribDataUpdateSyncEventPlaybackPos>(5, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataUpdatePlaybackPos* attrMyUpdateTimePosOut = parameters->createOutputAttribReplace<AttribDataUpdatePlaybackPos>(6, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  if (attrParentUpdateTimePosIn->m_isAbs)
  {
    // Absolute from parent - Calculate this frames values.
    calcCurrentPlaybackValuesFromParentAbsTimeAdjSpace(
      attrMySyncEventTrackIn->m_syncEventTrack,
      attrParentUpdateTimePosIn,
      attrMyLoopableIn->m_value,
      attrMyFractionPosOut,
      attrMyTimePosOut,
      attrMyCurrentSyncEventPosOut,
      attrMyUpdateTimePosOut);
  }
  else
  {
    // Delta from parent - Calculate last and this frames values.
    AttribDataPlaybackPosInit* playbackPosInit = parameters->getOptionalInputAttrib<AttribDataPlaybackPosInit>(7, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT);
    subTaskUpdateDeltaTimeOnSyncEventsNode(
      attrMyPreviousSyncEventPosIn,
      attrMySyncEventTrackIn,
      attrMyLoopableIn->m_value,
      attrMyTimePosOut,
      attrMyCurrentSyncEventPosOut,
      attrMyFractionPosOut,
      attrParentUpdateTimePosIn,
      playbackPosInit,
      attrMyUpdateTimePosOut);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2SyncEventsUpdateTimeViaSyncEventPos(Dispatcher::TaskParameters* parameters)
{
  AttribDataUpdateSyncEventPlaybackPos* attrParentAdjUpdateSyncEventPosIn = parameters->getInputAttrib<AttribDataUpdateSyncEventPlaybackPos>(0, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataPlaybackPos* attrMyTimePosOut = parameters->createOutputAttribReplace<AttribDataPlaybackPos>(1, ATTRIB_SEMANTIC_TIME_POS);
  AttribDataPlaybackPos* attrMyFractionPosOut = parameters->createOutputAttribReplace<AttribDataPlaybackPos>(2, ATTRIB_SEMANTIC_FRACTION_POS);
  AttribDataSyncEventTrack* attrMySyncEventTrackIn = parameters->getInputAttrib<AttribDataSyncEventTrack>(3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataBool* attrMyLoopableIn = parameters->getInputAttrib<AttribDataBool>(4, ATTRIB_SEMANTIC_LOOP);
  AttribDataUpdateSyncEventPlaybackPos* attrMyPreviousSyncEventPosIn = parameters->getInputAttrib<AttribDataUpdateSyncEventPlaybackPos>(5, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataUpdateSyncEventPlaybackPos* attrMyCurrentSyncEventPosOut = parameters->createOutputAttribReplace<AttribDataUpdateSyncEventPlaybackPos>(5, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataUpdatePlaybackPos* attrMyUpdateTimePosOut = parameters->createOutputAttribReplace<AttribDataUpdatePlaybackPos>(6, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  if (attrParentAdjUpdateSyncEventPosIn->m_isAbs)
  {
    // Calculate this frames values.
    calcCurrentPlaybackValuesFromParentAbsSyncEventPos(
      attrMySyncEventTrackIn->m_syncEventTrack,
      attrMyLoopableIn->m_value,
      attrParentAdjUpdateSyncEventPosIn,
      attrMyCurrentSyncEventPosOut,
      attrMyFractionPosOut,
      attrMyTimePosOut,
      attrMyUpdateTimePosOut);
  }
  else
  {
    // Delta from parent - Calculate last and this frames values.
    subTaskUpdateDeltaSyncEventOnSyncEventsNode(
      attrMyPreviousSyncEventPosIn,
      attrMySyncEventTrackIn,
      attrMyLoopableIn,
      attrMyTimePosOut,
      attrMyCurrentSyncEventPosOut,
      attrMyFractionPosOut,
      attrParentAdjUpdateSyncEventPosIn,
      attrMyUpdateTimePosOut);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskEmptyTask(Dispatcher::TaskParameters* NMP_UNUSED(parameters))
{
}

//----------------------------------------------------------------------------------------------------------------------
void TaskNetworkDummyTask(Dispatcher::TaskParameters* NMP_UNUSED(parameters))
{
  // should never be called
  NMP_ASSERT_FAIL();
}

//----------------------------------------------------------------------------------------------------------------------
static void filterCopy(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* buffer,
  uint32_t               numFilterChannels,
  const uint32_t*        filterChannels)    // Channels to filter out.
{
  uint32_t i;

  buffer->copyTo(destBuffer);

  for (i = 0; i < numFilterChannels; ++i)
  {
    NMP_ASSERT(filterChannels[i] <= destBuffer->getLength());
    destBuffer->getUsedFlags()->clearBit(filterChannels[i]);
  }

  // Clear Full status, if any filtering, including channel zero.
  if (numFilterChannels)
  {
    destBuffer->setFullFlag(false);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskFilterTransforms(Dispatcher::TaskParameters* parameters)
{
  AttribDataTransformBuffer* inputTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataUIntArray* filteredBoneIds = parameters->getInputAttrib<AttribDataUIntArray>(1, ATTRIB_SEMANTIC_BONE_IDS);

  const uint32_t numRigJoints = inputTransforms->m_transformBuffer->getLength();
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(2, numRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // Perform the actual filter.
  filterCopy(
    outputTransforms->m_transformBuffer,
    inputTransforms->m_transformBuffer,
    filteredBoneIds->m_numValues,
    filteredBoneIds->m_values);
}
    

//----------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------- 
namespace
{
// Get some joint info from a transform buffer.
void extractJointInfo(
  int32_t          jointIndex,
  NMP::Vector3&    jointPos,
  float&           jointAngle,
  AnimRigDef*      rig,
  NMP::DataBuffer* transformBuffer,
  float&           eulerXAngle,
  float&           eulerYAngle,
  float&           eulerZAngle)
{
  jointPos.setToZero();

  if ((jointIndex > -1) && (jointIndex < (int32_t)transformBuffer->getLength()))
  {
    int32_t parentJointIndex = rig->getParentBoneIndex(jointIndex);
    if (parentJointIndex != AnimRigDef::NO_PARENT_BONE)
    {
      // Calculate the angle between this bone and its parent.
      NMP::Quat jointQuat = *(transformBuffer->getChannelQuatFast(jointIndex));

      NMP::Quat parentJointQuat = *(transformBuffer->getChannelQuatFast(parentJointIndex));
      jointAngle = parentJointQuat.angleTo(jointQuat);

      jointAngle = jointQuat.angle();

      NMP::Vector3 jointEulerAngles = jointQuat.toEulerXYZ();
      eulerXAngle = jointEulerAngles.x;
      eulerYAngle = jointEulerAngles.y;
      eulerZAngle = jointEulerAngles.z;
    }
    jointPos = *(transformBuffer->getChannelPosFast(jointIndex));
  }
}

//---------------------------------------------------------------------------------------------------------------------- 
}

//----------------------------------------------------------------------------------------------------------------------
// Outputs component values of a joint from the input transforms in object space.
static void subTaskPassThroughTransformsExtractJointInfoObjectSpace(
  Dispatcher::TaskParameters* parameters,
  AttribDataSemantic transformBuffer)
{
  AttribDataTransformBuffer* inputTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(0, transformBuffer);
  AttribDataUInt* jointIndex = parameters->getInputAttrib<AttribDataUInt>(1, ATTRIB_SEMANTIC_CP_UINT);
  const uint32_t numRigJoints = inputTransforms->m_transformBuffer->getLength();
  AttribDataInt* angleType = parameters->getInputAttrib<AttribDataInt>(2, ATTRIB_SEMANTIC_CP_INT);
  AttribDataBool* measureUnits = parameters->getInputAttrib<AttribDataBool>(3, ATTRIB_SEMANTIC_CP_BOOL);
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(4, ATTRIB_SEMANTIC_RIG);
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(5, numRigJoints, transformBuffer);

  AttribDataVector3* outputJointPosCP = parameters->createOutputCPAttrib<AttribDataVector3>(6, ATTRIB_SEMANTIC_CP_VECTOR3);
  AttribDataFloat* outputJointAngleCP = parameters->createOutputCPAttrib<AttribDataFloat>(7, ATTRIB_SEMANTIC_CP_FLOAT);

  const AnimRigDef*      animRig(rigAttribData->m_rig);
  const NMP::DataBuffer* inputBuffer(inputTransforms->m_transformBuffer);

  NMP::Matrix34 characterSpaceTM(*(inputBuffer->getChannelQuatFast(jointIndex->m_value)), *(inputBuffer->getChannelPosFast(jointIndex->m_value)));

  for (int32_t index(jointIndex->m_value); index != AnimRigDef::NO_PARENT_BONE; index = animRig->getParentBoneIndex(index))
  {
    NMP::Matrix34 partTM(*(animRig->getBindPoseBoneQuat(index)), *(animRig->getBindPoseBonePos(index)));
    characterSpaceTM = partTM * characterSpaceTM;
  }

  const uint32_t trajectoryIndex(animRig->getTrajectoryBoneIndex());
  const NMP::Matrix34 trajectoryTM(*(inputBuffer->getChannelQuatFast(trajectoryIndex)), *(inputBuffer->getChannelPosFast(trajectoryIndex)));
  characterSpaceTM = trajectoryTM * characterSpaceTM;

  // Write to output.

  // Construct character space transforms.
  // OPTIMISE - Use MR::BlendOpsBase::accumulateTransforms, except a version that fills in the bind pose for missing channels.
  calculateCharSpaceTransforms(
    rigAttribData->m_rig,
    inputTransforms->m_transformBuffer,
    outputTransforms->m_transformBuffer);

  float angle  = 0.0f;
  float eulerX = 0.0f;
  float eulerY = 0.0f;
  float eulerZ = 0.0f;

  // Extract the info from the joint.
  extractJointInfo(
    jointIndex->m_value,
    outputJointPosCP->m_value,
    angle,
    rigAttribData->m_rig,
    outputTransforms->m_transformBuffer,
    eulerX,
    eulerY,
    eulerZ);

  // Select the correct angle to output
  switch(angleType->m_value)
  {
  case 0: // Total
    outputJointAngleCP->m_value = angle;
    break;
  case 1: // EulerX
    outputJointAngleCP->m_value = eulerX;
    break;
  case 2: // EulerY
    outputJointAngleCP->m_value = eulerY;
    break;
  case 3: // EulerZ
    outputJointAngleCP->m_value = eulerZ;
    break;
  default:
    NMP_ASSERT_FAIL();
  }
  // Convert to degrees if requested. [MeasureUnit=true] radians, [MeasureUnit=false] degrees
  if (measureUnits->m_value == false)
  {
    outputJointAngleCP->m_value = NMP::radiansToDegrees(outputJointAngleCP->m_value);
  }

  // Overwrite the output transforms buffer (where we have been working temporarily) with the input buffer.
  inputTransforms->m_transformBuffer->copyTo(outputTransforms->m_transformBuffer);
}


//----------------------------------------------------------------------------------------------------------------------
void TaskPassThroughTransformsExtractJointInfoObjectSpace(Dispatcher::TaskParameters* parameters)
{
  subTaskPassThroughTransformsExtractJointInfoObjectSpace(parameters, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskPassThroughTrajectoryDeltaAndTransformsExtractJointInfoObjectSpace(Dispatcher::TaskParameters* parameters)
{
  subTaskPassThroughTransformsExtractJointInfoObjectSpace(parameters, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
// Outputs component values of a joint from the input transforms in local space.
void subTaskPassThroughTransformsExtractJointInfoLocalSpace(
  Dispatcher::TaskParameters* parameters,
  AttribDataSemantic transformSemantic)
{
  AttribDataTransformBuffer* inputTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(0, transformSemantic);
  AttribDataUInt* jointIndex = parameters->getInputAttrib<AttribDataUInt>(1, ATTRIB_SEMANTIC_CP_UINT);
  const uint32_t numRigJoints = inputTransforms->m_transformBuffer->getLength();
  AttribDataInt* angleType = parameters->getInputAttrib<AttribDataInt>(2, ATTRIB_SEMANTIC_CP_INT);
  AttribDataBool* measureUnits = parameters->getInputAttrib<AttribDataBool>(3, ATTRIB_SEMANTIC_CP_BOOL);
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(4, ATTRIB_SEMANTIC_RIG);
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(5, numRigJoints, transformSemantic);
  AttribDataVector3* outputJointPosCP = parameters->createOutputCPAttrib<AttribDataVector3>(6, ATTRIB_SEMANTIC_CP_VECTOR3);
  AttribDataFloat* outputJointAngleCP = parameters->createOutputCPAttrib<AttribDataFloat>(7, ATTRIB_SEMANTIC_CP_FLOAT);

  // Overwrite the output transforms buffer with the input buffer.
  inputTransforms->m_transformBuffer->copyTo(outputTransforms->m_transformBuffer);

  float angle  = 0.0f;
  float eulerX = 0.0f;
  float eulerY = 0.0f;
  float eulerZ = 0.0f;

  // Extract the info from the joint.
  extractJointInfo(
    jointIndex->m_value,
    outputJointPosCP->m_value,
    angle,
    rigAttribData->m_rig,
    outputTransforms->m_transformBuffer,
    eulerX,
    eulerY,
    eulerZ);

  // Select the correct angle to output
  switch(angleType->m_value)
  {
  case 0: // Total
    outputJointAngleCP->m_value = angle;
    break;
  case 1: // EulerX
    outputJointAngleCP->m_value = eulerX;
    break;
  case 2: // EulerY
    outputJointAngleCP->m_value = eulerY;
    break;
  case 3: // EulerZ
    outputJointAngleCP->m_value = eulerZ;
    break;
  default:
    NMP_ASSERT_FAIL();
  }
  // Convert to degrees if requested. [MeasureUnit=true] radians, [MeasureUnit=false] degrees
  if (measureUnits->m_value == false)
  {
    outputJointAngleCP->m_value = NMP::radiansToDegrees(outputJointAngleCP->m_value);
  }
}
    
//----------------------------------------------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------------------------------------------
void TaskPassThroughTransformsExtractJointInfoLocalSpace(Dispatcher::TaskParameters* parameters)
{
  subTaskPassThroughTransformsExtractJointInfoLocalSpace(parameters, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskPassThroughTrajectoryDeltaAndTransformsExtractJointInfoLocalSpace(Dispatcher::TaskParameters* parameters)
{
  subTaskPassThroughTransformsExtractJointInfoLocalSpace(parameters, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
namespace 
{

//----------------------------------------------------------------------------------------------------------------------
void subTaskPassThroughTransformsExtractJointInfoObjectSpaceJointSelect(
  Dispatcher::TaskParameters* parameters,
  AttribDataSemantic transformSemantic)
{
  AttribDataTransformBuffer* inputTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(0, transformSemantic);
  AttribDataUInt* defaultJointIndex = parameters->getInputAttrib<AttribDataUInt>(1, ATTRIB_SEMANTIC_CP_UINT);
  const uint32_t numRigJoints = inputTransforms->m_transformBuffer->getLength();
  AttribDataInt* angleType = parameters->getInputAttrib<AttribDataInt>(2, ATTRIB_SEMANTIC_CP_INT);
  AttribDataBool* measureUnits = parameters->getInputAttrib<AttribDataBool>(3, ATTRIB_SEMANTIC_CP_BOOL);
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(4, ATTRIB_SEMANTIC_RIG);
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(5, numRigJoints, transformSemantic);
  AttribDataVector3* outputJointPosCP = parameters->createOutputCPAttrib<AttribDataVector3>(6, ATTRIB_SEMANTIC_CP_VECTOR3);
  AttribDataFloat* outputJointAngleCP = parameters->createOutputCPAttrib<AttribDataFloat>(7, ATTRIB_SEMANTIC_CP_FLOAT);
  AttribDataInt* jointIndex = parameters->getInputAttrib<AttribDataInt>(8, ATTRIB_SEMANTIC_CP_INT);

  // Use the dynamically chosen bone index if we have a sensible one, otherwise use the def default.
  int32_t sampleJointIndex = defaultJointIndex->m_value;
  if ((jointIndex->m_value > -1) && (jointIndex->m_value < (int32_t)rigAttribData->m_rig->getNumBones()))
    sampleJointIndex = jointIndex->m_value;

  // Construct character space transforms.
  // OPTIMISE - Use MR::BlendOpsBase::accumulateTransforms, except a version that fills in the bind pose for missing channels.
  calculateCharSpaceTransforms(
    rigAttribData->m_rig,
    inputTransforms->m_transformBuffer,
    outputTransforms->m_transformBuffer);

  float angle  = 0.0f;
  float eulerX = 0.0f;
  float eulerY = 0.0f;
  float eulerZ = 0.0f;

  // Extract the info from the joint.
  extractJointInfo(
    sampleJointIndex,
    outputJointPosCP->m_value,
    angle,
    rigAttribData->m_rig,
    outputTransforms->m_transformBuffer,
    eulerX,
    eulerY,
    eulerZ);

  // Convert to degrees if requested. [MeasureUnit=true] radians, [MeasureUnit=false] degrees
  if (measureUnits->m_value == false)
  {
    eulerX = NMP::radiansToDegrees(eulerX);
    eulerY = NMP::radiansToDegrees(eulerY);
    eulerZ = NMP::radiansToDegrees(eulerZ);
  }

  // Select the correct angle to output
  switch(angleType->m_value)
  {
  case 0: // Total
    outputJointAngleCP->m_value = angle;
    break;
  case 1: // EulerX
    outputJointAngleCP->m_value = eulerX;
    break;
  case 2: // EulerY
    outputJointAngleCP->m_value = eulerY;
    break;
  case 3: // EulerZ
    outputJointAngleCP->m_value = eulerZ;
    break;
  default:
    NMP_ASSERT_FAIL();
  }

  // Overwrite the output transforms buffer (where we have been working temporarily) with the input buffer.
  inputTransforms->m_transformBuffer->copyTo(outputTransforms->m_transformBuffer);
}
 
//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void TaskPassThroughTransformsExtractJointInfoObjectSpaceJointSelect(Dispatcher::TaskParameters* parameters)
{
  subTaskPassThroughTransformsExtractJointInfoObjectSpaceJointSelect(parameters, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskPassThroughTrajectoryDeltaAndTransformsExtractJointInfoObjectSpaceJointSelect(Dispatcher::TaskParameters* parameters)
{
  subTaskPassThroughTransformsExtractJointInfoObjectSpaceJointSelect(parameters, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
}

    
//----------------------------------------------------------------------------------------------------------------------   
namespace
{

//----------------------------------------------------------------------------------------------------------------------
void subTaskPassThroughTransformsExtractJointInfoLocalSpaceJointSelect(
  Dispatcher::TaskParameters* parameters,
  AttribDataSemantic transformSemantic)
{
  AttribDataTransformBuffer* inputTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(0, transformSemantic);
  AttribDataUInt* defaultJointIndex = parameters->getInputAttrib<AttribDataUInt>(1, ATTRIB_SEMANTIC_CP_UINT);
  const uint32_t numRigJoints = inputTransforms->m_transformBuffer->getLength();
  AttribDataInt* angleType = parameters->getInputAttrib<AttribDataInt>(2, ATTRIB_SEMANTIC_CP_INT);
  AttribDataBool* measureUnits = parameters->getInputAttrib<AttribDataBool>(3, ATTRIB_SEMANTIC_CP_BOOL);
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(4, ATTRIB_SEMANTIC_RIG);
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(5, numRigJoints, transformSemantic);
  AttribDataVector3* outputJointPosCP = parameters->createOutputCPAttrib<AttribDataVector3>(6, ATTRIB_SEMANTIC_CP_VECTOR3);
  AttribDataFloat* outputJointAngleCP = parameters->createOutputCPAttrib<AttribDataFloat>(7, ATTRIB_SEMANTIC_CP_FLOAT);
  AttribDataInt* jointIndex = parameters->getInputAttrib<AttribDataInt>(8, ATTRIB_SEMANTIC_CP_INT);

  // Overwrite the output transforms buffer with the input buffer.
  inputTransforms->m_transformBuffer->copyTo(outputTransforms->m_transformBuffer);

  // Use the dynamically chosen bone index if we have a sensible one, otherwise use the def default.
  int32_t sampleJointIndex = defaultJointIndex->m_value;
  if ((jointIndex->m_value > -1) && (jointIndex->m_value < (int32_t)rigAttribData->m_rig->getNumBones()))
    sampleJointIndex = jointIndex->m_value;

  float angle  = 0.0f;
  float eulerX = 0.0f;
  float eulerY = 0.0f;
  float eulerZ = 0.0f;

  // Extract the info from the joint.
  extractJointInfo(
    sampleJointIndex,
    outputJointPosCP->m_value,
    angle,
    rigAttribData->m_rig,
    outputTransforms->m_transformBuffer,
    eulerX,
    eulerY,
    eulerZ);

  // Select the correct angle to output
  switch(angleType->m_value)
  {
  case 0: // Total
    outputJointAngleCP->m_value = angle;
    break;
  case 1: // EulerX
    outputJointAngleCP->m_value = eulerX;
    break;
  case 2: // EulerY
    outputJointAngleCP->m_value = eulerY;
    break;
  case 3: // EulerZ
    outputJointAngleCP->m_value = eulerZ;
    break;
  default:
    NMP_ASSERT_FAIL();
  }
  // Convert to degrees if requested. [MeasureUnit=true] radians, [MeasureUnit=false] degrees
  if (measureUnits->m_value == false)
  {
    outputJointAngleCP->m_value = NMP::radiansToDegrees(outputJointAngleCP->m_value);
  }
}

//----------------------------------------------------------------------------------------------------------------------   
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void TaskPassThroughTransformsExtractJointInfoLocalSpaceJointSelect(Dispatcher::TaskParameters* parameters)
{
  subTaskPassThroughTransformsExtractJointInfoLocalSpaceJointSelect(parameters, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskPassThroughTrajectoryDeltaAndTransformsExtractJointInfoLocalSpaceJointSelect(Dispatcher::TaskParameters* parameters)
{
  subTaskPassThroughTransformsExtractJointInfoLocalSpaceJointSelect(
    parameters, 
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskFilterTrajectoryDelta(Dispatcher::TaskParameters* parameters)
{
  AttribDataTrajectoryDeltaTransform* outputTrajDeltaTransform = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  AttribDataTrajectoryDeltaTransform* inputTrajDeltaTransform = parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  AttribDataUIntArray* filteredBoneIds = parameters->getInputAttrib<AttribDataUIntArray>(2, ATTRIB_SEMANTIC_BONE_IDS);
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(3, ATTRIB_SEMANTIC_RIG);

  NMP_ASSERT(rigAttribData->m_rig);
  uint32_t trajBoneIndex = rigAttribData->m_rig->getTrajectoryBoneIndex();

  bool nodeFiltersTrajectory = false;
  for (uint32_t i = 0; i < filteredBoneIds->m_numValues; i++)
  {
    NMP_ASSERT(filteredBoneIds->m_values);
    if (filteredBoneIds->m_values[i] == trajBoneIndex)
    {
      nodeFiltersTrajectory = true;
      break;
    }
  }

  if (nodeFiltersTrajectory || inputTrajDeltaTransform->m_filteredOut)
  {
    // Either we have filtered the trajectory channel or our child has.
    outputTrajDeltaTransform->m_deltaAtt.identity();
    outputTrajDeltaTransform->m_deltaPos.setToZero();
    outputTrajDeltaTransform->m_filteredOut = true;
  }
  else
  {
    // Output valid trajectory channel data.
    outputTrajDeltaTransform->m_deltaAtt = inputTrajDeltaTransform->m_deltaAtt;
    outputTrajDeltaTransform->m_deltaPos = inputTrajDeltaTransform->m_deltaPos;
    outputTrajDeltaTransform->m_filteredOut = false;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void TaskFilterTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters)
{
  AttribDataTransformBuffer* inputTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  AttribDataUIntArray* filteredBoneIds = parameters->getInputAttrib<AttribDataUIntArray>(2, ATTRIB_SEMANTIC_BONE_IDS);
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(3, ATTRIB_SEMANTIC_RIG);

  const uint32_t numRigJoints = inputTransforms->m_transformBuffer->getLength();
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(0, numRigJoints, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

  // Perform the actual filter.
  filterCopy(
    outputTransforms->m_transformBuffer,
    inputTransforms->m_transformBuffer,
    filteredBoneIds->m_numValues,
    filteredBoneIds->m_values);

  NMP_ASSERT(rigAttribData->m_rig);
  uint32_t trajBoneIndex = rigAttribData->m_rig->getTrajectoryBoneIndex();

  bool nodeFiltersTrajectory = false;
  for (uint32_t i = 0; i < filteredBoneIds->m_numValues; i++)
  {
    NMP_ASSERT(filteredBoneIds->m_values);
    if (filteredBoneIds->m_values[i] == trajBoneIndex)
    {
      nodeFiltersTrajectory = true;
      break;
    }
  }

  if (nodeFiltersTrajectory || !inputTransforms->m_transformBuffer->hasChannel(0))
  {
    // Either we have filtered the trajectory channel or our child has.
    outputTransforms->m_transformBuffer->getChannelQuat(0)->identity();
    outputTransforms->m_transformBuffer->getChannelPos(0)->setToZero();
    outputTransforms->m_transformBuffer->setChannelUnused(0);
  }
  else
  {
    // Output valid trajectory channel data.
    outputTransforms->m_transformBuffer->setChannelUsed(0);
  }
}


//----------------------------------------------------------------------------------------------------------------------
void TaskApplyGlobalTimeUpdateTimePos(Dispatcher::TaskParameters* parameters)
{
  AttribDataUpdatePlaybackPos* networkUpdateTimePosIn = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(0, ATTRIB_SEMANTIC_UPDATE_TIME_POS);
  AttribDataUpdatePlaybackPos* networkUpdateTimePosOut = parameters->createOutputAttrib<AttribDataUpdatePlaybackPos>(1, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  networkUpdateTimePosOut->copy(networkUpdateTimePosIn, networkUpdateTimePosOut);
}

//----------------------------------------------------------------------------------------------------------------------
// Set bind pose transforms on any channels that are not currently set with valid transform data.
NM_INLINE void subTaskApplyBindPoseTransforms(Dispatcher::TaskParameters* parameters, AttribDataSemantic transformSemantic)
{
  AttribDataTransformBuffer* inputTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(0, transformSemantic);
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(1, ATTRIB_SEMANTIC_RIG);

  const uint32_t numRigJoints = inputTransforms->m_transformBuffer->getLength();
  NMP_ASSERT(numRigJoints == rigAttribData->m_rig->getNumBones());
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(2, numRigJoints, transformSemantic);

  inputTransforms->m_transformBuffer->copyTo(outputTransforms->m_transformBuffer);

  BlendOpsBase::applyBindPoseToUnusedChannels(rigAttribData->m_rig->getBindPose(), outputTransforms->m_transformBuffer);
}
//----------------------------------------------------------------------------------------------------------------------
// Set bind pose transforms on any channels that are not currently set with valid transform data.
void TaskApplyBindPoseTransforms(Dispatcher::TaskParameters* parameters)
{
  subTaskApplyBindPoseTransforms(parameters, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskApplyBindPoseTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters)
{
  subTaskApplyBindPoseTransforms(parameters, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskGetBindPoseTransforms(Dispatcher::TaskParameters* parameters)
{
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(0, ATTRIB_SEMANTIC_RIG);
  const uint32_t numRigJoints = rigAttribData->m_rig->getNumBones();
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(1, numRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  NMP::Quat* qOut = outputTransforms->m_transformBuffer->getPosQuatChannelQuat(0);
  NMP::Vector3* pOut = outputTransforms->m_transformBuffer->getPosQuatChannelPos(0);

  MR::AnimRigDef* rig = rigAttribData->m_rig;
  for (uint32_t i = 0; i < numRigJoints; ++i)
  {
    qOut[i] = *rig->getBindPoseBoneQuat(i);
    pOut[i] = *rig->getBindPoseBonePos(i);
    outputTransforms->m_transformBuffer->getUsedFlags()->setBit(i);
  }

  outputTransforms->m_transformBuffer->calculateFullFlag();
}
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void subTaskBufferLastFramesTransformBuffer(Dispatcher::TaskParameters* parameters, AttribDataSemantic transformSemantic)
{
  AttribDataTransformBuffer* inputTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(0, transformSemantic);

  if (inputTransforms)
  {
    const uint32_t numOutputTransforms = inputTransforms->m_transformBuffer->getLength();
    AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(2, numOutputTransforms, transformSemantic);
    inputTransforms->m_transformBuffer->copyTo(outputTransforms->m_transformBuffer);
  }
  else
  {
    //second case - there is no data available. We just fall back to bind pose
    AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(1, ATTRIB_SEMANTIC_RIG);
    const uint32_t numRigJoints = rigAttribData->m_rig->getNumBones();
    AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(2, numRigJoints, transformSemantic);

    NMP::Quat* qOut = outputTransforms->m_transformBuffer->getChannelQuat(0);
    NMP::Vector3* pOut = outputTransforms->m_transformBuffer->getChannelPos(0);

    MR::AnimRigDef* rig = rigAttribData->m_rig;
    for (uint32_t i = 0; i < numRigJoints; ++i)
    {
      qOut[i] = *rig->getBindPoseBoneQuat(i);
      pOut[i] = *rig->getBindPoseBonePos(i);
      outputTransforms->m_transformBuffer->getUsedFlags()->setBit(i);
    }

    outputTransforms->m_transformBuffer->setFullFlag(true);
  }
}


//----------------------------------------------------------------------------------------------------------------------
void TaskBufferLastFramesTransformBuffer(Dispatcher::TaskParameters* parameters)
{
  subTaskBufferLastFramesTransformBuffer(parameters, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBufferLastFramesTrajectoryDeltaAndTransformBuffer(Dispatcher::TaskParameters* parameters)
{
  subTaskBufferLastFramesTransformBuffer(parameters, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBufferLastFramesTransformsZeroTrajectory(Dispatcher::TaskParameters* parameters)
{
  subTaskBufferLastFramesTransformBuffer(parameters, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

  // Zero the trajectory delta
  AttribDataTransformBuffer* outputTransformBuffer = (AttribDataTransformBuffer*)parameters->m_parameters[2].m_attribDataHandle.m_attribData;
  NMP_ASSERT(outputTransformBuffer);
  NMP_ASSERT(outputTransformBuffer->getType() == ATTRIB_TYPE_TRANSFORM_BUFFER);
  outputTransformBuffer->m_transformBuffer->setChannelPos(0, NMP::Vector3(NMP::Vector3::InitZero));
  outputTransformBuffer->m_transformBuffer->setChannelQuat(0, NMP::Quat(NMP::Quat::kIdentity));
}

//----------------------------------------------------------------------------------------------------------------------
namespace 
{

//----------------------------------------------------------------------------------------------------------------------
void calculateLocalSpaceTransforms(MR::AnimRigDef* rig, NMP::DataBuffer* charSpaceTransf, NMP::DataBuffer* localTransf)
{
  // By looping in reverse we start with the extremities and progress towards the root joint. This means
  // we can operate on a single transform buffer (i.e. this function will work correctly if charSpaceTransf and
  // localTransf refer to the same buffer).
  for (uint32_t i = rig->getNumBones(); i > 0; )
  {
    --i;

    // Check for a valid channel
    if (!charSpaceTransf->hasChannel(i))
    {
      localTransf->setPosQuatChannelPos(i, NMP::Vector3(NMP::Vector3::InitZero));
      localTransf->setPosQuatChannelQuat(i, NMP::Quat(NMP::Quat::kIdentity));
      localTransf->setChannelUnused(i);
      continue;
    }

    // get the world space position / quaternion
    NMP::Vector3 worldPos = *charSpaceTransf->getPosQuatChannelPos(i);
    NMP::Quat worldQ = *charSpaceTransf->getPosQuatChannelQuat(i);

    // If child then LocalTransform = WorldTransform * ParentInverseWorldTranform
    int32_t parentIdx = rig->getHierarchy()->getParentIndex(i);
    if (parentIdx > 0)
    {
      NMP::Vector3 parentPos = *charSpaceTransf->getPosQuatChannelPos(parentIdx);
      NMP::Quat parentQ = *charSpaceTransf->getPosQuatChannelQuat(parentIdx);
      parentQ.conjugate();
      worldPos -= parentPos;
      worldPos = parentQ.rotateVector(worldPos);
      worldQ = parentQ * worldQ;
    }

    localTransf->setPosQuatChannelQuat(i, worldQ);
    localTransf->setPosQuatChannelPos(i, worldPos);
    localTransf->setChannelUsed(i);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void subTaskMirrorTransformsWithMapping(
  const uint32_t                 numRigJoints,
  uint32_t                       mirrorAxis,
  AttribDataMirroredAnimMapping* mirroredAnimMap,
  NMP::DataBuffer*               characterSpaceTransforms)
{
  // First, mirror all transforms in the rig
  // OPTIMISE  Missing channels (unused channels) must still be mirrored to account for offsets, but they have
  // been given the identity as their transforms.  You can retrieve their mirrored TM directly from their parent,
  // or whatever joint in their ancestry is the first used channel.  However, doing this may be slower than
  // just mirroring what you have.
  switch (mirrorAxis)
  {
  case 0: // reflect in YZ plane
  {
    for (uint32_t boneIndex = 0; boneIndex != numRigJoints; ++boneIndex)
    {
      NMP::Quat rot = *characterSpaceTransforms->getPosQuatChannelQuat(boneIndex);
      rot.y = -rot.y;
      rot.z = -rot.z;
      characterSpaceTransforms->setPosQuatChannelQuat(boneIndex, rot);

      NMP::Vector3 pos = *characterSpaceTransforms->getPosQuatChannelPos(boneIndex);
      pos.x = -pos.x;
      characterSpaceTransforms->setPosQuatChannelPos(boneIndex, pos);
    }
  }
  break;

  case 1: // reflect in ZX plane
  {
    for (uint32_t boneIndex = 0; boneIndex != numRigJoints; ++boneIndex)
    {
      NMP::Quat rot = *characterSpaceTransforms->getPosQuatChannelQuat(boneIndex);
      rot.x = -rot.x;
      rot.z = -rot.z;
      characterSpaceTransforms->setPosQuatChannelQuat(boneIndex, rot);

      NMP::Vector3 pos = *characterSpaceTransforms->getPosQuatChannelPos(boneIndex);
      pos.y = -pos.y;
      characterSpaceTransforms->setPosQuatChannelPos(boneIndex, pos);
    }
  }
  break;

  case 2: // reflect in XY plane
  {
    for (uint32_t boneIndex = 0; boneIndex != numRigJoints; ++boneIndex)
    {
      NMP::Quat rot = *characterSpaceTransforms->getPosQuatChannelQuat(boneIndex);
      rot.x = -rot.x;
      rot.y = -rot.y;
      characterSpaceTransforms->setPosQuatChannelQuat(boneIndex, rot);

      NMP::Vector3 pos = *characterSpaceTransforms->getPosQuatChannelPos(boneIndex);
      pos.z = -pos.z;
      characterSpaceTransforms->setPosQuatChannelPos(boneIndex, pos);
    }
  }
  break;
  }

  uint32_t numBonesInMap = mirroredAnimMap->getNumMappings();

  // The TM's are now mirrored, but some TM's need to be swapped over
  // Even missing channels must be swapped over, to account for offset rotations.
  for (uint32_t mappingIndex = 0; mappingIndex != numBonesInMap; ++mappingIndex)
  {
    // get indices of transforms to swap
    uint32_t left = mirroredAnimMap->getLeftBone(mappingIndex);
    uint32_t right = mirroredAnimMap->getRightBone(mappingIndex);

    // check to see if channels are used
    bool leftUsed  = characterSpaceTransforms->hasChannel(left);
    bool rightUsed = characterSpaceTransforms->hasChannel(right);

    // temp vars to hold swapped values
    NMP::Quat rotR, rotL;
    NMP::Vector3 translateL, translateR;

    // grab channel data for the transform on the left side
    rotL = *characterSpaceTransforms->getPosQuatChannelQuat(left);
    translateL = *characterSpaceTransforms->getPosQuatChannelPos(left);

    // grab channel data for the transform on the right side, and copy into the left hand side
    rotR = *characterSpaceTransforms->getPosQuatChannelQuat(right);
    translateR = *characterSpaceTransforms->getPosQuatChannelPos(right);

    characterSpaceTransforms->setPosQuatChannelQuat(left, rotR);
    characterSpaceTransforms->setPosQuatChannelPos(left, translateR);

    // finally copy the data for the right side into the left
    characterSpaceTransforms->setPosQuatChannelQuat(right, rotL);
    characterSpaceTransforms->setPosQuatChannelPos(right, translateL);

    // Swap the channel used flags
    characterSpaceTransforms->setChannelUsedFlag(left, rightUsed);
    characterSpaceTransforms->setChannelUsedFlag(right, leftUsed);
  }

  // Now apply offsets to joints that are not swapped.
  // Again, we need to do this for all joints even if they are unused channels, in case they
  // have children which are used.
  NMP_ASSERT(mirroredAnimMap->getNumBones() == numRigJoints);
  for (uint32_t boneIndex = 0; boneIndex != numRigJoints; ++boneIndex)
  {
   NMP::Quat rotate = *characterSpaceTransforms->getPosQuatChannelQuat(boneIndex);
   rotate = rotate * *mirroredAnimMap->getOffset(boneIndex);
   characterSpaceTransforms->setPosQuatChannelQuat(boneIndex, rotate);
  }
}

//----------------------------------------------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------------------------------------------
void TaskMirrorUpdateTimeViaSyncEventPos(Dispatcher::TaskParameters* parameters)
{
  AttribDataUpdateSyncEventPlaybackPos* attrParentUpdateSyncEventPosIn = parameters->getInputAttrib<AttribDataUpdateSyncEventPlaybackPos>(0, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataPlaybackPos* attrMyTimePosOut = parameters->createOutputAttrib<AttribDataPlaybackPos>(1, ATTRIB_SEMANTIC_TIME_POS);
  AttribDataPlaybackPos* attrMyFractionPosOut = parameters->createOutputAttrib<AttribDataPlaybackPos>(2, ATTRIB_SEMANTIC_FRACTION_POS);
  AttribDataSyncEventTrack* attrMySyncEventTrackIn = parameters->getInputAttrib<AttribDataSyncEventTrack>(3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataUpdateSyncEventPlaybackPos* attrMyPreviousSyncEventPosIn = parameters->getInputAttrib<AttribDataUpdateSyncEventPlaybackPos>(4, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataUpdateSyncEventPlaybackPos* attrMyCurrentSyncEventPosOut = parameters->createOutputAttribReplace<AttribDataUpdateSyncEventPlaybackPos>(4, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataUpdatePlaybackPos* attrMyUpdateTimePosOut = parameters->createOutputAttribReplace<AttribDataUpdatePlaybackPos>(5, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  if (attrParentUpdateSyncEventPosIn->m_isAbs)
  {
    // Calculate this frames values.
    calcCurrentPlaybackValuesFromParentAbsSyncEventPos(
      attrMySyncEventTrackIn->m_syncEventTrack,
      true,
      attrParentUpdateSyncEventPosIn,
      attrMyCurrentSyncEventPosOut,
      attrMyFractionPosOut,
      attrMyTimePosOut,
      attrMyUpdateTimePosOut);
  }
  else
  {
    // Delta from parent - Calculate last and this frames values.
    AttribDataUpdateSyncEventPlaybackPos prevSyncEventPos;
    if (!attrMyPreviousSyncEventPosIn)
    {
      // First frame initialisation with a delta from parent - Calculate previous frame sync event pos.
      prevSyncEventPos.m_isAbs = true;
      float previousValue = attrParentUpdateSyncEventPosIn->m_absPosReal.get() - attrParentUpdateSyncEventPosIn->m_deltaPos.fraction();
      if (previousValue < 0.0f)
      {
        // wrap-around, ensure previous value is modulo the number of events in the sync event track
        float numEvents = (float) attrMySyncEventTrackIn->m_syncEventTrack.getNumEvents();
        previousValue = numEvents + fmodf(previousValue, numEvents);
      }
      prevSyncEventPos.m_absPosAdj.set(previousValue);
      attrMySyncEventTrackIn->m_syncEventTrack.limitToSyncEventSpace(prevSyncEventPos.m_absPosAdj, true);
      attrMySyncEventTrackIn->m_syncEventTrack.convEventInAdjustedSpaceToRealSpace(prevSyncEventPos.m_absPosAdj, prevSyncEventPos.m_absPosReal);
    }
    else
    {
      prevSyncEventPos = (*attrMyPreviousSyncEventPosIn);
    }
    // Calculate last frames values. Necessary as sync event track config is likely to have changed.
    calcPreviousPlaybackValuesFromMyAbsSyncEventPos(
      attrMySyncEventTrackIn->m_syncEventTrack,
      true,
      &prevSyncEventPos,
      &prevSyncEventPos,
      attrMyFractionPosOut,
      attrMyTimePosOut);

    // Calculate this frames values.
    calcCurrentPlaybackValuesFromParentDeltaSyncEventPosAdjSpace(
      attrMySyncEventTrackIn->m_syncEventTrack,
      attrParentUpdateSyncEventPosIn,
      &prevSyncEventPos,
      true,
      attrMyFractionPosOut,
      attrMyTimePosOut,
      attrMyCurrentSyncEventPosOut,
      attrMyUpdateTimePosOut);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Updates are provided to us by our parent in our Adjusted space (parents Real space).
// All working is done internally in our Adjusted space.
// Provision to parent, conditions etc is in our Adjusted space.
// Provision to children is in our Real space.
void TaskMirrorUpdateTimeViaTimePos(Dispatcher::TaskParameters* parameters)
{
  AttribDataUpdatePlaybackPos* attrParentUpdateTimePosIn = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(0, ATTRIB_SEMANTIC_UPDATE_TIME_POS);
  AttribDataPlaybackPos* attrMyTimePosOut = parameters->createOutputAttrib<AttribDataPlaybackPos>(1, ATTRIB_SEMANTIC_TIME_POS);
  AttribDataPlaybackPos* attrMyFractionPosOut = parameters->createOutputAttrib<AttribDataPlaybackPos>(2, ATTRIB_SEMANTIC_FRACTION_POS);
  AttribDataSyncEventTrack* attrMySyncEventTrackIn = parameters->getInputAttrib<AttribDataSyncEventTrack>(3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataUpdateSyncEventPlaybackPos* attrMyPreviousSyncEventPosIn = parameters->getInputAttrib<AttribDataUpdateSyncEventPlaybackPos>(4, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataUpdateSyncEventPlaybackPos* attrMyCurrentSyncEventPosOut = parameters->createOutputAttribReplace<AttribDataUpdateSyncEventPlaybackPos>(4, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS);
  AttribDataUpdatePlaybackPos* attrMyUpdateTimePosOut = parameters->createOutputAttribReplace<AttribDataUpdatePlaybackPos>(5, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  if (attrParentUpdateTimePosIn->m_isAbs)
  {
    // Calculate this frames values.
    calcCurrentPlaybackValuesFromParentAbsTimeAdjSpace(
      attrMySyncEventTrackIn->m_syncEventTrack,
      attrParentUpdateTimePosIn,
      true,
      attrMyFractionPosOut,
      attrMyTimePosOut,
      attrMyCurrentSyncEventPosOut,
      attrMyUpdateTimePosOut);
  }
  else
  {
    // Delta from parent - Calculate last and this frames values.
    AttribDataPlaybackPosInit* playbackPosInit = parameters->getOptionalInputAttrib<AttribDataPlaybackPosInit>(6, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT);
    subTaskUpdateDeltaTimeOnSyncEventsNode(
      attrMyPreviousSyncEventPosIn,
      attrMySyncEventTrackIn,
      true,
      attrMyTimePosOut,
      attrMyCurrentSyncEventPosOut,
      attrMyFractionPosOut,
      attrParentUpdateTimePosIn,
      playbackPosInit,
      attrMyUpdateTimePosOut);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskMirrorSyncEventsAndOffset(Dispatcher::TaskParameters* parameters)
{
  AttribDataSyncEventTrack* mySyncEventTrack = parameters->createOutputAttrib<AttribDataSyncEventTrack>(0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataSyncEventTrack* sourceSyncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataUInt* eventOffset = parameters->getInputAttrib<AttribDataUInt>(2, ATTRIB_SEMANTIC_SYNC_EVENT_OFFSET);
  AttribDataMirroredAnimMapping* animMirrorMappingAttrib = parameters->getInputAttrib<AttribDataMirroredAnimMapping>(3, ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING);
  AttribDataPlaybackPosInit* playbackPosInit = parameters->getOptionalInputAttrib<AttribDataPlaybackPosInit>(4, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT);

  // Copy input sync track; rebasing the copy.
  mySyncEventTrack->m_syncEventTrack.copyClipTrack(&sourceSyncEventTrack->m_syncEventTrack, true);

  // offset the start index.
  int32_t startSyncEvent = eventOffset->m_value;
  if (playbackPosInit)
    startSyncEvent += playbackPosInit->m_adjustStartEventIndex;
  mySyncEventTrack->m_syncEventTrack.setStartEventIndex(startSyncEvent);

  // Mirror event userdata.
  uint32_t numSyncEvents = mySyncEventTrack->m_syncEventTrack.getNumEvents();
  for (uint32_t i = 0; i != numSyncEvents; ++i)
  {
    EventDefDiscrete* thisEvent = mySyncEventTrack->m_syncEventTrack.getEvent(i);

    uint32_t userDataId = thisEvent->getUserData();
    for (uint32_t j = 0; j != animMirrorMappingAttrib->m_numEventIds; ++j)
    {
      uint32_t leftUserId = animMirrorMappingAttrib->getLeftEventIdMapping(j);
      uint32_t rightUserId = animMirrorMappingAttrib->getRightEventIdMapping(j);

      if (leftUserId == userDataId)
      {
        userDataId = rightUserId;
        break;
      }

      if (rightUserId == userDataId)
      {
        userDataId = leftUserId;
        break;
      }
    }
    thisEvent->setUserData(userDataId);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskMirrorSampledEvents(Dispatcher::TaskParameters* parameters)
{
  AttribDataSampledEvents* sourceSampledEvents = parameters->getInputAttrib<AttribDataSampledEvents>(1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataMirroredAnimMapping* mirroredAnimMapping = parameters->getInputAttrib<AttribDataMirroredAnimMapping>(2, ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING);

  // create output events buffer.
  NMP_ASSERT(sourceSampledEvents->m_discreteBuffer);
  NMP_ASSERT(sourceSampledEvents->m_curveBuffer);
  AttribDataSampledEventsCreateDesc desc(sourceSampledEvents->m_discreteBuffer->getNumTriggeredEvents(), sourceSampledEvents->m_curveBuffer->getNumSampledEvents());
  AttribDataSampledEvents* outputSampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);

  // mirror the events
  subTaskMirrorSampledEvents(sourceSampledEvents, outputSampledEvents, mirroredAnimMapping);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void subTaskMirrorTransforms(
  AttribDataRig* rigAttribData,
  AttribDataMirroredAnimMapping* rigAttribMirroredAnimMapping,
  AttribDataUIntArray* unfilteredBoneIds,
  const NMP::DataBuffer* inputTransforms,
  NMP::DataBuffer* outputTransforms)
{
  const uint32_t numRigJoints = rigAttribData->m_rig->getNumBones();

  // Construct character space transforms.
  // OPTIMISE - Use MR::BlendOpsBase::accumulateTransforms, except a version that fills in the
  // bind pose for missing channels.  If you do this you need to normalise the quat at every joint.
  calculateCharSpaceTransforms(
    rigAttribData->m_rig,
    inputTransforms,
    outputTransforms);

  // Mirror the joints
  subTaskMirrorTransformsWithMapping(
    numRigJoints,
    rigAttribMirroredAnimMapping->m_axis,
    rigAttribMirroredAnimMapping,
    outputTransforms);

  // Reconstruct and output the local space transforms
  calculateLocalSpaceTransforms(
    rigAttribData->m_rig,
    outputTransforms,
    outputTransforms);

  // Replace all the unfiltered joints with the input buffer joint data
  for (uint32_t i = 0; i < unfilteredBoneIds->m_numValues; ++i)
  {
    uint32_t boneIndex = unfilteredBoneIds->m_values[i];
    NMP_ASSERT(boneIndex < numRigJoints);

    if (outputTransforms->hasChannel(boneIndex))
    {
      outputTransforms->setPosQuatChannelPos(boneIndex, *inputTransforms->getPosQuatChannelPos(boneIndex));
      outputTransforms->setPosQuatChannelQuat(boneIndex, *inputTransforms->getPosQuatChannelQuat(boneIndex));
    }
  }

  outputTransforms->calculateFullFlag();
}

//----------------------------------------------------------------------------------------------------------------------
void TaskMirrorTransforms(Dispatcher::TaskParameters* parameters)
{
  // Retrieve the rig attributes.
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(0, ATTRIB_SEMANTIC_RIG);

  // Retrieve the rig mapping attributes.
  AttribDataMirroredAnimMapping* rigAttribMirroredAnimMapping =
    parameters->getInputAttrib<AttribDataMirroredAnimMapping>(1, ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING);

  // Retrieve the unfiltered bone ids.
  AttribDataUIntArray* unfilteredBoneIds = parameters->getInputAttrib<AttribDataUIntArray>(2, ATTRIB_SEMANTIC_BONE_IDS);

  // Retrieve current input transform buffer.
  AttribDataTransformBuffer* inputTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(3, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // Create an output transform buffer.
  const uint32_t numOutputTransforms = inputTransforms->m_transformBuffer->getLength();
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(4, numOutputTransforms, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  subTaskMirrorTransforms(
    rigAttribData,
    rigAttribMirroredAnimMapping,
    unfilteredBoneIds,
    inputTransforms->m_transformBuffer,
    outputTransforms->m_transformBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
void subTaskMirrorTrajectoryDelta(
  NMP::Vector3* outputTrajDeltaPos,
  NMP::Quat* outputTrajDeltaAtt,
  bool* outputFilteredOut,
  AttribDataMirroredAnimMapping* rigAttribMirroredAnimMapping,
  AttribDataUIntArray* unfilteredBoneIds,
  AttribDataRig* rigAttribData,
  const NMP::Vector3& inputTrajDeltaPos,
  const NMP::Quat& inputTrajDeltaAtt,
  bool inputFilteredOut)
{
  // early out if the trajectory is filtered out already
  if (inputFilteredOut)
  {
    // Either we have filtered the trajectory channel or our child has.
    outputTrajDeltaAtt->identity();
    outputTrajDeltaPos->setToZero();
    *outputFilteredOut = true;

    return;
  }

  *outputTrajDeltaPos = inputTrajDeltaPos;
  *outputTrajDeltaAtt = inputTrajDeltaAtt;
  *outputFilteredOut = false;

  NMP_ASSERT(rigAttribData->m_rig);
  uint32_t trajBoneIndex = rigAttribData->m_rig->getTrajectoryBoneIndex();

  bool nodeFiltersTrajectory = true;
  for (uint32_t i = 0; i < unfilteredBoneIds->m_numValues; ++i)
  {
    uint32_t boneIndex = unfilteredBoneIds->m_values[i];
    NMP_ASSERT(boneIndex < rigAttribData->m_rig->getNumBones());

    if (boneIndex == trajBoneIndex)
    {
      nodeFiltersTrajectory = false;
      break;
    }
  }

  if (nodeFiltersTrajectory)
  {
    // first, mirror all transforms in the rig
    switch (rigAttribMirroredAnimMapping->m_axis)
    {
    case 0: // reflect in YZ plane
      {
        outputTrajDeltaAtt->y = -outputTrajDeltaAtt->y;
        outputTrajDeltaAtt->z = -outputTrajDeltaAtt->z;
        outputTrajDeltaPos->x = -outputTrajDeltaPos->x;
      }
      break;

    case 1: // reflect in ZX plane
      {
        outputTrajDeltaAtt->z = -outputTrajDeltaAtt->z;
        outputTrajDeltaAtt->x = -outputTrajDeltaAtt->x;
        outputTrajDeltaPos->y = -outputTrajDeltaPos->y;
      }
      break;

    case 2: // reflect in XY plane
      {
        outputTrajDeltaAtt->x = -outputTrajDeltaAtt->x;
        outputTrajDeltaAtt->y = -outputTrajDeltaAtt->y;
        outputTrajDeltaPos->z = -outputTrajDeltaPos->z;
      }
      break;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void TaskMirrorTrajectoryDelta(Dispatcher::TaskParameters* parameters)
{
  // Output trajectory transforms
  AttribDataTrajectoryDeltaTransform* outputTrajDeltaTransform = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  // Retrieve the rig attributes
  AttribDataMirroredAnimMapping* rigAttribMirroredAnimMapping =  parameters->getInputAttrib<AttribDataMirroredAnimMapping>(1, ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING);
  // Retrieve the unfiltered bone ids
  AttribDataUIntArray* unfilteredBoneIds = parameters->getInputAttrib<AttribDataUIntArray>(2, ATTRIB_SEMANTIC_BONE_IDS);
  // Retrieve the rig attributes
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(3, ATTRIB_SEMANTIC_RIG);
  // Input trajectory transforms
  AttribDataTrajectoryDeltaTransform* inputTrajDeltaTransform = parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(4, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);

  subTaskMirrorTrajectoryDelta(
    &outputTrajDeltaTransform->m_deltaPos,
    &outputTrajDeltaTransform->m_deltaAtt,
    &outputTrajDeltaTransform->m_filteredOut,
    rigAttribMirroredAnimMapping,
    unfilteredBoneIds,
    rigAttribData,
    inputTrajDeltaTransform->m_deltaPos,
    inputTrajDeltaTransform->m_deltaAtt,
    inputTrajDeltaTransform->m_filteredOut);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskMirrorTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters)
{
  // Retrieve the rig attributes
  AttribDataMirroredAnimMapping* rigAttribMirroredAnimMapping =  parameters->getInputAttrib<AttribDataMirroredAnimMapping>(1, ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING);
  // Retrieve the unfiltered bone ids
  AttribDataUIntArray* unfilteredBoneIds = parameters->getInputAttrib<AttribDataUIntArray>(2, ATTRIB_SEMANTIC_BONE_IDS);
  // Retrieve the rig attributes
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(3, ATTRIB_SEMANTIC_RIG);
  // Input trajectory delta and transforms
  AttribDataTransformBuffer* inputTrajDeltaTransform = parameters->getInputAttrib<AttribDataTransformBuffer>(4, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  const NMP::DataBuffer* inputTransforms = inputTrajDeltaTransform->m_transformBuffer;

  // Create an output transform buffer.
  const uint32_t numOutputTransforms = inputTransforms->getLength();
  AttribDataTransformBuffer* outputTransformsAttrib = parameters->createOutputAttribTransformBuffer(0, numOutputTransforms, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
  NMP::DataBuffer* outputTransforms = outputTransformsAttrib->m_transformBuffer;

  bool outputFilteredOut = !outputTransforms->hasChannel(0);

  subTaskMirrorTrajectoryDelta(
    outputTransforms->getPosQuatChannelPos(0),
    outputTransforms->getPosQuatChannelQuat(0),
    &outputFilteredOut,
    rigAttribMirroredAnimMapping,
    unfilteredBoneIds,
    rigAttribData,
    *inputTransforms->getPosQuatChannelPos(0),
    *inputTransforms->getPosQuatChannelQuat(0),
    inputTransforms->hasChannel(0));

  outputTransforms->setChannelUsedFlag(0, !outputFilteredOut);

  subTaskMirrorTransforms(
    rigAttribData,
    rigAttribMirroredAnimMapping,
    unfilteredBoneIds,
    inputTransforms,
    outputTransforms);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskScaleUpdateTime(Dispatcher::TaskParameters* parameters)
{
  AttribDataUpdatePlaybackPos* parentsUpdateTimePos = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(0, ATTRIB_SEMANTIC_UPDATE_TIME_POS);
  AttribDataFloat* timeScaleFactor = parameters->getInputAttrib<AttribDataFloat>(1, ATTRIB_SEMANTIC_CP_FLOAT);
  AttribDataUpdatePlaybackPos* timePos = parameters->createOutputAttrib<AttribDataUpdatePlaybackPos>(2, ATTRIB_SEMANTIC_UPDATE_TIME_POS);
  timePos->m_isFraction = parentsUpdateTimePos->m_isFraction;
  timePos->m_isAbs = parentsUpdateTimePos->m_isAbs;

  if (parentsUpdateTimePos->m_isFraction && parentsUpdateTimePos->m_isAbs)
  {
    // Do not scale a fractional absolute time from our parent.
    timePos->m_value = parentsUpdateTimePos->m_value;
  }
  else
  {
    // Scale update time.
    float timeScale = NMP::maximum(0.0f,  timeScaleFactor->m_value);
    timePos->m_value = parentsUpdateTimePos->m_value * timeScale;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskScaleUpdateSyncEventTrack(Dispatcher::TaskParameters* parameters)
{
  AttribDataSyncEventTrack* mySyncEventTrack = parameters->createOutputAttrib<AttribDataSyncEventTrack>(0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataSyncEventTrack* source0SyncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataFloat* timeScaleFactor = parameters->getInputAttrib<AttribDataFloat>(2, ATTRIB_SEMANTIC_CP_FLOAT);

  float timeScale = NMP::maximum(0.0f,  timeScaleFactor->m_value);

  // Scale sync event track duration
  float sourceDuration = source0SyncEventTrack->m_syncEventTrack.getDuration();
  float scaledDuration;
  if (timeScale > ERROR_LIMIT)
    scaledDuration = sourceDuration / timeScale;
  else
    scaledDuration = sourceDuration / ERROR_LIMIT;
  float scaledDurationReciprocal = 1.0f / scaledDuration;

  mySyncEventTrack->m_syncEventTrack.copyClipTrack(&(source0SyncEventTrack->m_syncEventTrack), false);
  mySyncEventTrack->m_syncEventTrack.setDuration(scaledDuration);
  mySyncEventTrack->m_syncEventTrack.setDurationReciprocal(scaledDurationReciprocal);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskSetUpdateTimeViaControlParam(Dispatcher::TaskParameters* parameters)
{
  AttribDataFloat* timeScaleFactor = parameters->getInputAttrib<AttribDataFloat>(0, ATTRIB_SEMANTIC_CP_FLOAT);
  AttribDataUpdatePlaybackPos* timePos = parameters->createOutputAttrib<AttribDataUpdatePlaybackPos>(1, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  timePos->m_value = NMP::clampValue(timeScaleFactor->m_value, 0.0f, 1.0f);
  timePos->m_isFraction = true;
  timePos->m_isAbs = true;
}

//----------------------------------------------------------------------------------------------------------------------
void TaskNetworkUpdateCharacterController(Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();
  NET_LOG_MESSAGE(98, "**** Executing TaskNetworkUpdateCharacterController ****\n");
  parameters->createOutputAttrib<AttribDataBool>(0, ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_UPDATED);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskNetworkUpdatePhysics(Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();
  NET_LOG_MESSAGE(98, "**** Executing TaskNetworkUpdatePhysics *****\n");
  parameters->createOutputAttrib<AttribDataBool>(0, ATTRIB_SEMANTIC_PHYSICS_UPDATED);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskNetworkUpdateRoot(Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();
  NET_LOG_MESSAGE(98, "**** Executing TaskNetworkUpdateRoot ****\n");
  parameters->createOutputAttrib<AttribDataBool>(0, ATTRIB_SEMANTIC_ROOT_UPDATED);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskNetworkMergePhysicsRigTransformBuffers(Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();
  NET_LOG_MESSAGE(98, "**** Executing TaskNetworkMergePhysicsRigTransformBuffers ****\n");

  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(0, ATTRIB_SEMANTIC_RIG);
  const uint32_t numRigJoints = rigAttribData->m_rig->getNumBones();
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(1, numRigJoints, ATTRIB_SEMANTIC_MERGED_PHYSICS_RIG_TRANSFORM_BUFFER);
  outputTransforms->m_transformBuffer->getUsedFlags()->clearAll();

  // Initialise output to the bind pose (in case of ragdoll which has no inputs)
  MR::AnimRigDef* rig = rigAttribData->m_rig;
  NMP::Quat* qOut = outputTransforms->m_transformBuffer->getPosQuatChannelQuat(0);
  NMP::Vector3* pOut = outputTransforms->m_transformBuffer->getPosQuatChannelPos(0);
  for (uint32_t i = 0; i < numRigJoints; ++i)
  {
    qOut[i] = *rig->getBindPoseBoneQuat(i);
    pOut[i] = *rig->getBindPoseBonePos(i);
  }

  uint32_t numInputTransforms = (parameters->m_numParameters - 2) / 2;
  for (uint32_t j = 0 ; j < numInputTransforms ; ++j)
  {
    AttribDataTransformBuffer* inputTransformBuffer = parameters->getInputAttrib<AttribDataTransformBuffer>(j * 2 + 2, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
    AttribDataBoolArray* inputMask = parameters->getInputAttrib<AttribDataBoolArray>(j * 2 + 3, ATTRIB_SEMANTIC_OUTPUT_MASK);

    NMP_ASSERT(inputTransformBuffer->m_transformBuffer);

    for (uint32_t i = 0 ; i < numRigJoints ; ++i)
    {
      if (inputTransformBuffer->m_transformBuffer->hasChannel(i) && inputMask->m_values[i] == true)
      {
        outputTransforms->m_transformBuffer->setPosQuatChannelPos(i, *inputTransformBuffer->m_transformBuffer->getPosQuatChannelPos(i));
        outputTransforms->m_transformBuffer->setPosQuatChannelQuat(i, *inputTransformBuffer->m_transformBuffer->getPosQuatChannelQuat(i));
        outputTransforms->m_transformBuffer->setChannelUsed(i);
      }
    }
  }

  outputTransforms->m_transformBuffer->calculateFullFlag();
}

//----------------------------------------------------------------------------------------------------------------------
void TaskScaleToDuration(Dispatcher::TaskParameters* parameters)
{
  AttribDataUpdatePlaybackPos* parentsUpdateTimePos = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(0, ATTRIB_SEMANTIC_UPDATE_TIME_POS);
  AttribDataSyncEventTrack* source0SyncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataFloat* requestedDuration = parameters->getInputAttrib<AttribDataFloat>(2, ATTRIB_SEMANTIC_CP_FLOAT);
  AttribDataUpdatePlaybackPos* timePos = parameters->createOutputAttrib<AttribDataUpdatePlaybackPos>(3, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  float newDuration = NMP::maximum(0.0001f, requestedDuration->m_value); // avoid div-by-zero
  float timeScale = (1.0f / newDuration) * source0SyncEventTrack->m_syncEventTrack.getDuration();

  timePos->m_isFraction = parentsUpdateTimePos->m_isFraction;
  timePos->m_isAbs = parentsUpdateTimePos->m_isAbs;
  timePos->m_value = (parentsUpdateTimePos->m_value * timeScale);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskScaleToDurationSyncEventTrack(Dispatcher::TaskParameters* parameters)
{
  AttribDataSyncEventTrack* source0SyncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataFloat* requestedDuration = parameters->getInputAttrib<AttribDataFloat>(1, ATTRIB_SEMANTIC_CP_FLOAT);
  AttribDataSyncEventTrack* mySyncEventTrack = parameters->createOutputAttrib<AttribDataSyncEventTrack>(2, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);

  mySyncEventTrack->m_syncEventTrack.copyClipTrack(&(source0SyncEventTrack->m_syncEventTrack), false);

  float newDuration = NMP::maximum(0.0001f, requestedDuration->m_value); // avoid div-by-zero
  mySyncEventTrack->m_syncEventTrack.setDuration(newDuration);
  mySyncEventTrack->m_syncEventTrack.setDurationReciprocal(1.0f / newDuration);
}

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
void subTaskOutputSmoothTransformsTransforms(Dispatcher::TaskParameters* parameters, AttribDataSemantic transformSemantic)
{
  //////////////////////////////////////////////////////////////////////////
  // RETRIEVE INPUT PARAMS
  //////////////////////////////////////////////////////////////////////////

  // Rig def buffer for the current animation set.
  const AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(
    0,
    ATTRIB_SEMANTIC_RIG);
  NMP_ASSERT(rigAttribData);
  const AnimRigDef* rigDef = rigAttribData->m_rig;
  NMP_ASSERT(rigDef);
  uint32_t jointsNum = rigDef->getNumBones();
  NMP_ASSERT(jointsNum > 0);

  // The input transforms buffer.
  AttribDataTransformBuffer* inputTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(
    1, transformSemantic);
  NMP_ASSERT(inputTransforms);
  NMP_ASSERT(inputTransforms->m_transformBuffer->getLength() == jointsNum);

  // Output transform buffer (keeping hold of state at three previous steps).
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(
    2,
    jointsNum,
    transformSemantic);
  NMP_ASSERT(outputTransforms);
  NMP_ASSERT(outputTransforms->m_transformBuffer->getLength() == jointsNum);
  AttribDataTransformBuffer* outputTransforms_km1 = parameters->getOptionalInputAttrib<AttribDataTransformBuffer>(
    3,
    transformSemantic);
  AttribDataTransformBuffer* outputTransforms_km2 = parameters->getOptionalInputAttrib<AttribDataTransformBuffer>(
    4,
    transformSemantic);
  AttribDataTransformBuffer* outputTransforms_km3 = parameters->getOptionalInputAttrib<AttribDataTransformBuffer>(
    5,
    transformSemantic);

  // Parameters.
  AttribDataFloatArray* smoothingStrengthsAttrib = parameters->getInputAttrib<AttribDataFloatArray>(
    6,
    ATTRIB_SEMANTIC_BONE_WEIGHTS);
  NMP_ASSERT(smoothingStrengthsAttrib);

  // Time
  AttribDataUpdatePlaybackPos* timePosAttrib =
    parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(7, ATTRIB_SEMANTIC_UPDATE_TIME_POS);
  NMP_ASSERT(timePosAttrib);
  AttribDataUpdatePlaybackPos* timePosAttribPrevious =
    parameters->getOptionalInputAttrib<AttribDataUpdatePlaybackPos>(8, ATTRIB_SEMANTIC_UPDATE_TIME_POS);
  float deltaTime = 0;
  if (timePosAttrib->m_isAbs)
  {
    if (timePosAttribPrevious)
    {
      deltaTime = timePosAttrib->m_value - timePosAttribPrevious->m_value;
    }
  }
  else
  {
    deltaTime = timePosAttrib->m_value;
  }

  // Smoothing strength multiplier control parameter
  AttribDataFloat* smoothnessMultiplierAttr =
    parameters->getInputAttrib<AttribDataFloat>(9, ATTRIB_SEMANTIC_CP_FLOAT);
  float smoothnessMultiplier = smoothnessMultiplierAttr->m_value;

  // Make sure that at the very least we pass through transforms
  inputTransforms->m_transformBuffer->copyTo(outputTransforms->m_transformBuffer);

  //////////////////////////////////////////////////////////////////////////
  // PERFORM ACTION
  //////////////////////////////////////////////////////////////////////////

  // Reset filter if we just started the runtime target.
  if (outputTransforms_km1 == 0)
  {
    outputTransforms_km1 = outputTransforms;
  }
  if (outputTransforms_km2 == 0)
  {
    outputTransforms_km2 = outputTransforms_km1;
  }
  if (outputTransforms_km3 == 0)
  {
    outputTransforms_km3 = outputTransforms_km2;
  }

  // When we switch animation sets our transform history will be wrong and we need to wait until
  // all the joint data has been cleared out before filtering
  // MORPH-21359: even when the rigs are the same size we should still stop filtering when switching sets
  if (outputTransforms_km1->m_transformBuffer->getLength() != jointsNum ||
    outputTransforms_km2->m_transformBuffer->getLength() != jointsNum ||
    outputTransforms_km3->m_transformBuffer->getLength() != jointsNum)
  {
    return;
  }

  // Filter each joint.
  // NOTE: all elements before the root (including the character transform) are smoothed both translationally
  //       and rotationally (because we need to smooth the translations and because the joints in the character
  //       are all rotational joints, only the joints before and including the root influence the translations).
  //       All elements after the root are only smoothed rotationally.
  float& dt = deltaTime;
  for (uint32_t jointIndex = 0; jointIndex < jointsNum; ++jointIndex)
  {
    // Ignore unused channels and joints with their smoothing factor set to zero
    float smoothingStrength = smoothingStrengthsAttrib->m_values[jointIndex] * smoothnessMultiplier;
    if (!inputTransforms->m_transformBuffer->hasChannel(jointIndex) ||
      !outputTransforms_km1->m_transformBuffer->hasChannel(jointIndex) ||
      !outputTransforms_km2->m_transformBuffer->hasChannel(jointIndex) ||
      !outputTransforms_km3->m_transformBuffer->hasChannel(jointIndex) ||
      smoothingStrength == 0)
    {
      continue;
    }

    // Cut-off frequency for this joint - convert into Hz from 0-1 user weight value
    // NOTE: when observing the data before filtering we notice that the hips are quite noisy
    //       while the other joints look better. Because of that, the user should smooth the
    //       joints below (and including) the root more than the ones after.
    float wn =
      (MR_SMOOTH_TRANSFORMS_NODE_FILTERS_MIN_CUT_OFF_FREQUENCY -
      (MR_SMOOTH_TRANSFORMS_NODE_FILTERS_MAX_CUT_OFF_FREQUENCY -
      MR_SMOOTH_TRANSFORMS_NODE_FILTERS_MIN_CUT_OFF_FREQUENCY) / MR_SMOOTH_TRANSFORMS_NODE_RESCALE_CONTROLS_COEFF) *
      NMRU::GeomUtils::fastLog10(smoothingStrength);

    // Clamp value, avoiding singular cut-off frequency of 0 and meaningless very high cut-off frequency which does
    // nothing at all.
    wn = NMP::clampValue(wn,
      MR_SMOOTH_TRANSFORMS_NODE_FILTERS_MIN_CUT_OFF_FREQUENCY,
      MR_SMOOTH_TRANSFORMS_NODE_FILTERS_MAX_CUT_OFF_FREQUENCY);

    // Get sample quats and clean them (find representation of quats which are in the same hemisphere of S3).
    NMP::Vector3* posU_k = inputTransforms->m_transformBuffer->getChannelPos(jointIndex);
    NMP::Quat* quatU_k = inputTransforms->m_transformBuffer->getChannelQuat(jointIndex);

    NMP::Vector3* posY_k = outputTransforms->m_transformBuffer->getChannelPos(jointIndex);
    NMP::Quat* quatY_k = outputTransforms->m_transformBuffer->getChannelQuat(jointIndex);

    NMP::Vector3* posY_km1 = outputTransforms_km1->m_transformBuffer->getChannelPos(jointIndex);
    NMP::Quat* quatY_km1 = outputTransforms_km1->m_transformBuffer->getChannelQuat(jointIndex);

    NMP::Vector3* posY_km2 = outputTransforms_km2->m_transformBuffer->getChannelPos(jointIndex);
    NMP::Quat* quatY_km2 = outputTransforms_km2->m_transformBuffer->getChannelQuat(jointIndex);

    NMP::Vector3* posY_km3 = outputTransforms_km3->m_transformBuffer->getChannelPos(jointIndex);
    NMP::Quat* quatY_km3 = outputTransforms_km3->m_transformBuffer->getChannelQuat(jointIndex);

    // The coefficients below correspond to the ones of a Butterworth filter of the 3rd order (so it is fair to
    // leave the hard coded values 'as are').
    float wn2 = wn * wn;
    float wn3 = wn2 * wn;
    float dt2 = dt * dt;
    float dt3 = dt2 * dt;
    float wndt = wn * dt;
    float wndtx2 = 2.0f * wndt;
    float wn2dt2 = wn2 * dt2;
    float wn2dt2x2 = 2.0f * wn2dt2;
    float wn3dt3 = wn3 * dt3;
    float b0 = 1.0f + wndtx2 + wn2dt2x2 + wn3dt3;
    float b1 = -3.0f - 4.0f * wndt - wn2dt2x2;
    float b2 = 3.0f + wndtx2;
    float b3 = -1.0f;
    float a = wn3dt3;

    float invB0 = 1.0f / b0;

    // Filtering.
    // NOTE: all joints are filtered on their rotational component, the joints before (and including) the
    //       root are filtered on their translational component additionally.
    if (jointIndex <= rigDef->getCharacterRootBoneIndex())
    {
      *posY_k = (*posU_k * a - *posY_km1 * b1 - *posY_km2 * b2 - *posY_km3 * b3) * invB0;
    }

    // Get quats into the same semi-arc by flipping the signs of their coefficients if necessary
    // OPTIMISE  Instead we might negate the quats for this frame if appropriate.  We would then know
    // that all the previous frames' quats were in the same semi-arc, so we would only ever need to test
    // the current against the previous frame, reducing it to one dot-product and one Quat::negate().
    b1 *= NMP::floatSelect(quatU_k->dot(*quatY_km1), 1.0f, -1.0f);
    b2 *= NMP::floatSelect(quatU_k->dot(*quatY_km2), 1.0f, -1.0f);
    b3 *= NMP::floatSelect(quatU_k->dot(*quatY_km3), 1.0f, -1.0f);
    *quatY_k = (*quatU_k * a - *quatY_km1 * b1 - *quatY_km2 * b2 - *quatY_km3 * b3) * invB0;
    quatY_k->normalise();
  }

  // Pass through the channel full info
  outputTransforms->m_transformBuffer->setFullFlag(inputTransforms->m_transformBuffer->isFull());
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void TaskOutputSmoothTransformsTransforms(Dispatcher::TaskParameters* parameters)
{
  subTaskOutputSmoothTransformsTransforms(parameters, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskOutputSmoothTransformsTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters)
{
  subTaskOutputSmoothTransformsTransforms(parameters, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
}

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void TaskDeadBlendCacheState(Dispatcher::TaskParameters* parameters)
{
  AttribDataTransformBuffer* source0Transforms               = parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataTrajectoryDeltaTransform* source0TrajDelta       = parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  AttribDataUpdatePlaybackPos* networkPlayPosAttrib          = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(2, ATTRIB_SEMANTIC_UPDATE_TIME_POS);
  AttribDataTransformBuffer* networkOutputTMinusOneAttrib    = parameters->getOptionalInputAttrib<AttribDataTransformBuffer>(3, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* transformsInputTMinusOneAttrib  = parameters->getOptionalInputAttrib<AttribDataTransformBuffer>(4, ATTRIB_SEMANTIC_TRANSFORM_BUFFER_T_MINUS_ONE);

  const uint32_t numRigJoints = source0Transforms->m_transformBuffer->getLength();

#ifdef NMP_ENABLE_ASSERTS
  if (transformsInputTMinusOneAttrib)
  {
    const uint32_t numRigJoints1 = transformsInputTMinusOneAttrib->m_transformBuffer->getLength();
    NMP_ASSERT(numRigJoints == numRigJoints1);
  }
#endif

  // fail safe, use current transforms for previous frame
  NMP::DataBuffer* transformsInputTMinusOne = source0Transforms->m_transformBuffer;

  if (transformsInputTMinusOneAttrib)
  {
    // use data cached by this task from previous frame
    transformsInputTMinusOne = transformsInputTMinusOneAttrib->m_transformBuffer;
  }
  else if (networkOutputTMinusOneAttrib)
  {
    // use network output from previous frame if passed
    transformsInputTMinusOne = networkOutputTMinusOneAttrib->m_transformBuffer;
  }

  AttribDataTransformBuffer* transformsOutputTMinusTwo = parameters->createOutputAttribReplaceTransformBuffer(5, ATTRIB_SEMANTIC_TRANSFORM_BUFFER_T_MINUS_TWO, numRigJoints);
  transformsInputTMinusOne->copyTo(transformsOutputTMinusTwo->m_transformBuffer);

  AttribDataTransformBuffer* transformsOutputTMinusOne = parameters->createOutputAttribReplaceTransformBuffer(6, ATTRIB_SEMANTIC_TRANSFORM_BUFFER_T_MINUS_ONE, numRigJoints);
  source0Transforms->m_transformBuffer->copyTo(transformsOutputTMinusOne->m_transformBuffer);

  AttribDataTrajectoryDeltaTransform* trajDeltaOutputTMinusOne = parameters->createOutputAttribReplace<AttribDataTrajectoryDeltaTransform>(7, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_T_MINUS_ONE);
  trajDeltaOutputTMinusOne->m_deltaAtt = source0TrajDelta->m_deltaAtt;
  trajDeltaOutputTMinusOne->m_deltaPos = source0TrajDelta->m_deltaPos;
  trajDeltaOutputTMinusOne->m_filteredOut = source0TrajDelta->m_filteredOut;

  AttribDataUpdatePlaybackPos* playbackPosOutputTMinusOne = parameters->createOutputAttribReplace<AttribDataUpdatePlaybackPos>(8, ATTRIB_SEMANTIC_UPDATE_TIME_POS_T_MINUS_ONE);
  playbackPosOutputTMinusOne->m_isAbs      = networkPlayPosAttrib->m_isAbs;
  playbackPosOutputTMinusOne->m_isFraction = networkPlayPosAttrib->m_isFraction;
  playbackPosOutputTMinusOne->m_value      = networkPlayPosAttrib->m_value;
}
#endif

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
