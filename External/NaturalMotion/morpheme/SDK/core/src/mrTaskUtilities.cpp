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
#include "morpheme/mrTaskUtilities.h"
#include "morpheme/mrNetwork.h"

#ifdef NM_HOST_CELL_SPU
  #include "morpheme/mrSPUDefines.h"
  #include "NMPlatform/ps3/NMSPUDMAController.h"
#endif
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
// Calculate the delta number of events given this and last frames real event positions.
// Only calculates differences in event index.
void calculateSyncEventIndexDelta(
  AttribDataUpdateSyncEventPlaybackPos* attrPreviousSyncEventPos, // Adjusted real values must already be calculated.
  AttribDataUpdateSyncEventPlaybackPos* attrCurrentSyncEventPos,  // Adjusted real values must already be calculated.
                                                                  //   Delta num events stored here for output.
  EventTrackSync&                       syncEventTrackIn)
{
  // deltaPosIndex should strictly be an int64_t, but MAX_NUM_SYNC_EVENTS < INT_MAX
  NM_ASSERT_COMPILE_TIME(MAX_NUM_SYNC_EVENTS < INT_MAX);

  // calculate index delta
  uint32_t currentPosIndex  = attrCurrentSyncEventPos->m_absPosReal.index();
  uint32_t previousPosIndex = attrPreviousSyncEventPos->m_absPosReal.index();
  int32_t  deltaPosIndex    = currentPosIndex - previousPosIndex;

  // calculate fraction delta
  float currentPosFrac  = attrCurrentSyncEventPos->m_absPosReal.get();
  float previousPosFrac = attrPreviousSyncEventPos->m_absPosReal.get();
  float deltaPosFrac    = currentPosFrac - previousPosFrac;

  // handle wrap-around, for multiple or single event in track
  if (deltaPosFrac < -ERROR_LIMIT)
  {
    uint32_t numEvents = syncEventTrackIn.getNumEvents();
    deltaPosFrac  = currentPosFrac + ((float)numEvents - previousPosFrac);
    deltaPosIndex += numEvents;
  }
  else if (deltaPosFrac < 0)
  {
    deltaPosFrac = 0;
  }

  // set sync event position delta
  attrCurrentSyncEventPos->m_deltaPos.setIndex(deltaPosIndex);
  attrCurrentSyncEventPos->m_deltaPos.setFraction(deltaPosFrac);
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void calcCurrentPlaybackValuesFromParentDeltaTimeAdjSpace(
  EventTrackSync&                       syncEventTrackIn,
  AttribDataUpdatePlaybackPos*          attrDeltaTimePosIn,         // Delta time in my adjusted space.
  AttribDataUpdateSyncEventPlaybackPos* attrPreviousSyncEventPosIn, // Adjusted and real values.
                                                                    //   Must already have been scaled to fit
                                                                    //   current sync event track.
  bool                                  loopableIn,
  AttribDataPlaybackPos*                attrFractionOut,            // Adjusted and real values.
                                                                    // Previous values must already be set.
  AttribDataPlaybackPos*                attrTimeOut,                // Adjusted and real values.
  AttribDataUpdateSyncEventPlaybackPos* attrCurrentSyncEventPosOut, // Adjusted and real values.
  AttribDataUpdatePlaybackPos*          attrUpdateTimePosOut)       // Real space.
{
  //---------------------------------
  // Generate real space values.

  // Fraction position.
  attrFractionOut->m_delta = attrDeltaTimePosIn->getFraction(syncEventTrackIn.getDurationReciprocal());
  attrFractionOut->m_currentPosReal = attrFractionOut->m_previousPosReal + attrFractionOut->m_delta;

  if (attrFractionOut->m_currentPosReal + ERROR_LIMIT >= 1.0f)
  {
    if (loopableIn)
    {
      // Loop anim.
      if (attrFractionOut->m_currentPosReal >= 1.0f)
        attrFractionOut->m_currentPosReal = fmodf(attrFractionOut->m_currentPosReal, 1.0f);
      else
        attrFractionOut->m_currentPosReal = 0.0f;
    }
    else
    {
      // Set to end of anim.
      attrFractionOut->m_currentPosReal = 1.0f;
      attrFractionOut->m_delta = 1.0f - attrFractionOut->m_previousPosReal;
      NMP_ASSERT(attrFractionOut->m_delta >= 0.0f);
    }
  }
  NMP_ASSERT(attrFractionOut->m_currentPosReal >= 0.0f && attrFractionOut->m_currentPosReal <= 1.0f);

  // Time position.
  attrTimeOut->m_currentPosReal = syncEventTrackIn.getDuration() * attrFractionOut->m_currentPosReal;
  NMP_ASSERT(attrTimeOut->m_currentPosReal >= 0.0f && attrTimeOut->m_currentPosReal <= syncEventTrackIn.getDuration());
  attrTimeOut->m_delta = syncEventTrackIn.getDuration() * attrFractionOut->m_delta;

  // Calculate real space time update value that can be used by children.
  if (attrUpdateTimePosOut)
  {
    attrUpdateTimePosOut->m_isFraction = false;
    attrUpdateTimePosOut->m_isAbs = false;
    attrUpdateTimePosOut->m_value = attrTimeOut->m_delta;
  }

  // Event position.
  syncEventTrackIn.getRealPosInfoFromRealFraction(attrFractionOut->m_currentPosReal, attrCurrentSyncEventPosOut->m_absPosReal);
  calculateSyncEventIndexDelta(attrPreviousSyncEventPosIn, attrCurrentSyncEventPosOut, syncEventTrackIn);

  //---------------------------------
  // Generate adjusted space values.
  attrFractionOut->m_currentPosAdj = syncEventTrackIn.convRealFractionToAdjFraction(attrFractionOut->m_currentPosReal);
  attrTimeOut->m_currentPosAdj = syncEventTrackIn.getDuration() * attrFractionOut->m_currentPosAdj;
  NMP_ASSERT(attrTimeOut->m_currentPosAdj >= 0.0f && attrTimeOut->m_currentPosAdj <= syncEventTrackIn.getDuration());
  syncEventTrackIn.convEventInRealSpaceToAdjustedSpace(
    attrCurrentSyncEventPosOut->m_absPosReal,
    attrCurrentSyncEventPosOut->m_absPosAdj);

  //---------------------------------
  // Store in the output attrib data that the last valid time update was a delta.
  attrTimeOut->setWithDelta();
  attrCurrentSyncEventPosOut->setWithDelta();
  attrFractionOut->setWithDelta();
}

//----------------------------------------------------------------------------------------------------------------------  
namespace
{

//----------------------------------------------------------------------------------------------------------------------
// Take parents abs time value and update our current values.
void calcPlaybackValuesFromParentAbsTimeAdjSpace(
  EventTrackSync&                       syncEventTrackIn,
  AttribDataUpdatePlaybackPos*          attrAbsTimePosIn,     // Abs time in our adjusted space.
  bool                                  loopableIn,
  float&                                adjFractionOut,
  float&                                realFractionOut,
  float&                                adjTimeOut,
  float&                                realTimeOut,
  AttribDataUpdateSyncEventPlaybackPos* attrSyncEventPosOut,  // Adjusted and real values.
  AttribDataUpdatePlaybackPos*          attrUpdateTimePosOut) // Real space.
{
  //---------------------------------
  // Generate real space values.

  // Get the time as a fraction of the duration in real space.
  float adjAbsFractionIn = attrAbsTimePosIn->getFraction(syncEventTrackIn.getDurationReciprocal());
  if (adjAbsFractionIn > 1.0f)
  {
    if (loopableIn)
    {
      // Loop anim.
      adjAbsFractionIn = fmodf(adjAbsFractionIn, 1.0f);
    }
    else
    {
      // Set to end of anim.
      adjAbsFractionIn = 1.0f;
    }
  }
  realFractionOut = syncEventTrackIn.convAdjFractionToRealFraction(adjAbsFractionIn);
  NMP_ASSERT(realFractionOut >= 0.0f && realFractionOut <= 1.0f);

  // Calculate time in real space.
  realTimeOut = syncEventTrackIn.getDuration() * realFractionOut;
  NMP_ASSERT(realTimeOut >= 0.0f && realTimeOut <= syncEventTrackIn.getDuration());

  // Calculate real space time update value that can be used by children.
  if (attrUpdateTimePosOut)
  {
    attrUpdateTimePosOut->m_isFraction = true;
    attrUpdateTimePosOut->m_isAbs = true;
    attrUpdateTimePosOut->m_value = realFractionOut;
  }

  // Calculate new event position in real space.
  syncEventTrackIn.getRealPosInfoFromRealFraction(
    realFractionOut,
    attrSyncEventPosOut->m_absPosReal);

  //---------------------------------
  // Generate adjusted space values.
  adjFractionOut = syncEventTrackIn.convRealFractionToAdjFraction(realFractionOut);
  adjTimeOut = syncEventTrackIn.getDuration() * adjFractionOut;
  NMP_ASSERT(adjTimeOut >= 0.0f && adjTimeOut <= syncEventTrackIn.getDuration());
  syncEventTrackIn.convEventInRealSpaceToAdjustedSpace(
    attrSyncEventPosOut->m_absPosReal,
    attrSyncEventPosOut->m_absPosAdj);
}
  
//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
// Take parents abs time value and update our previous values.
void calcPreviousPlaybackValuesFromParentAbsTimeAdjSpace(
  EventTrackSync&                       syncEventTrackIn,
  AttribDataUpdatePlaybackPos*          attrAbsTimePosIn,     // Abs time in our adjusted space.
  bool                                  loopableIn,
  AttribDataPlaybackPos*                attrFractionOut,      // Adjusted and real values.
  AttribDataPlaybackPos*                attrTimeOut,          // Adjusted and real values.
  AttribDataUpdateSyncEventPlaybackPos* attrSyncEventPosOut,  // Adjusted and real values.
  AttribDataUpdatePlaybackPos*          attrUpdateTimePosOut) // Real space.
{
  calcPlaybackValuesFromParentAbsTimeAdjSpace(
    syncEventTrackIn,
    attrAbsTimePosIn,
    loopableIn,
    attrFractionOut->m_previousPosAdj,
    attrFractionOut->m_previousPosReal,
    attrTimeOut->m_previousPosAdj,
    attrTimeOut->m_previousPosReal,
    attrSyncEventPosOut,
    attrUpdateTimePosOut);

  //---------------------------------
  // Store in the output attrib data that the last valid time update was an absolute.
  attrTimeOut->setWithAbs();
  attrSyncEventPosOut->setWithAbs();
  attrFractionOut->setWithAbs();
}

//----------------------------------------------------------------------------------------------------------------------
// Take parents abs time value and update our current values.
void calcCurrentPlaybackValuesFromParentAbsTimeAdjSpace(
  EventTrackSync&                       syncEventTrackIn,
  AttribDataUpdatePlaybackPos*          attrAbsTimePosIn,     // Abs time in our adjusted space.
  bool                                  loopableIn,
  AttribDataPlaybackPos*                attrFractionOut,      // Adjusted and real values.
  AttribDataPlaybackPos*                attrTimeOut,          // Adjusted and real values.
  AttribDataUpdateSyncEventPlaybackPos* attrSyncEventPosOut,  // Adjusted and real values.
  AttribDataUpdatePlaybackPos*          attrUpdateTimePosOut) // Real space.
{
  calcPlaybackValuesFromParentAbsTimeAdjSpace(
    syncEventTrackIn,
    attrAbsTimePosIn,
    loopableIn,
    attrFractionOut->m_currentPosAdj,
    attrFractionOut->m_currentPosReal,
    attrTimeOut->m_currentPosAdj,
    attrTimeOut->m_currentPosReal,
    attrSyncEventPosOut,
    attrUpdateTimePosOut);

  //---------------------------------
  // Store in the output attrib data that the last valid time update was an absolute.
  attrTimeOut->setWithAbs();
  attrTimeOut->setInclusiveStart(true);
  attrSyncEventPosOut->setWithAbs();
  attrFractionOut->setWithAbs();
  attrFractionOut->setInclusiveStart(true);
}

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
void calcPlaybackValuesFromAbsEventPosAdjSpace(
  EventTrackSync&                       syncEventTrackIn,
  bool                                  loopableIn,
  SyncEventPos&                         ajdSyncEventPosIn,   // In our adjusted space.
  AttribDataUpdateSyncEventPlaybackPos* attrSyncEventPosOut, // Adjusted and real values, capped or looped appropriately.
  float&                                adjFractionOut,
  float&                                adjTimeOut,
  float&                                realFractionOut,
  float&                                realTimeOut)
{
  // Convert adjusted space sync event pos to real space.
  attrSyncEventPosOut->m_absPosReal.setFraction(ajdSyncEventPosIn.fraction());
  attrSyncEventPosOut->m_absPosReal.setIndex(ajdSyncEventPosIn.index() + syncEventTrackIn.getStartEventIndex());

  // Limit the real event space pos to the current sync event space.
  //  Allow wrap around of the event pos when not marked as looping if the input update event index is less than
  //  the sync track event count.
  if (loopableIn || ajdSyncEventPosIn.index() < syncEventTrackIn.getNumEvents())
    syncEventTrackIn.limitToSyncEventSpace(attrSyncEventPosOut->m_absPosReal, true);
  else
    syncEventTrackIn.limitToSyncEventSpace(attrSyncEventPosOut->m_absPosReal, false);

  // Derive real space time values.
  realFractionOut = syncEventTrackIn.getRealPosFractionFromRealSyncEventPos(attrSyncEventPosOut->m_absPosReal);
  realTimeOut = syncEventTrackIn.getDuration() * realFractionOut;
  NMP_ASSERT(realTimeOut >= 0.0f && realTimeOut <= syncEventTrackIn.getDuration());

  // Derive adjusted space time values.
  adjFractionOut = syncEventTrackIn.convRealFractionToAdjFraction(realFractionOut);
  adjTimeOut = syncEventTrackIn.getDuration() * adjFractionOut;
  NMP_ASSERT(adjTimeOut >= 0.0f && adjTimeOut <= syncEventTrackIn.getDuration());
  syncEventTrackIn.convEventInRealSpaceToAdjustedSpace(
    attrSyncEventPosOut->m_absPosReal,
    attrSyncEventPosOut->m_absPosAdj);
}

//---------------------------------------------------------------------------------------------------------------------- 
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
// Take parents abs sync event value and update our current values.
void calcCurrentPlaybackValuesFromParentAbsSyncEventPos(
  EventTrackSync&                       mySyncEventTrackIn,
  bool                                  loopableIn,
  AttribDataUpdateSyncEventPlaybackPos* attrParentSyncEventPosIn, // Adj value in parent adjusted space.
                                                                  // Real value in parent real space and our adjusted space.
  AttribDataUpdateSyncEventPlaybackPos* attrMySyncEventPosOut,    // Adjusted and real values.
                                                                  //   Capped or looped appropriately.
  AttribDataPlaybackPos*                attrMyFractionOut,        // Adjusted and real values.
  AttribDataPlaybackPos*                attrMyTimeOut,            // Adjusted and real values.
  AttribDataUpdatePlaybackPos*          attrUpdateTimePosOut)     // Real space.
{
  calcPlaybackValuesFromAbsEventPosAdjSpace(
    mySyncEventTrackIn,
    loopableIn,
    attrParentSyncEventPosIn->m_absPosReal,
    attrMySyncEventPosOut,
    attrMyFractionOut->m_currentPosAdj,
    attrMyTimeOut->m_currentPosAdj,
    attrMyFractionOut->m_currentPosReal,
    attrMyTimeOut->m_currentPosReal);

  // Calculate real space time update value that can be used by children.
  if (attrUpdateTimePosOut)
  {
    attrUpdateTimePosOut->m_isFraction = true;
    attrUpdateTimePosOut->m_isAbs = true;
    attrUpdateTimePosOut->m_value = attrMyFractionOut->m_currentPosReal;
  }

  // Store in the output attrib data that the last valid time update was an absolute.
  attrMyTimeOut->setWithAbs();
  attrMyTimeOut->setInclusiveStart(true);
  attrMySyncEventPosOut->setWithAbs();
  attrMyFractionOut->setWithAbs();
  attrMyFractionOut->setInclusiveStart(true);
}

//----------------------------------------------------------------------------------------------------------------------
// Take our previous abs sync event value and update our previous values.
void calcPreviousPlaybackValuesFromMyAbsSyncEventPos(
  EventTrackSync&                       mySyncEventTrackIn,
  bool                                  loopableIn,
  AttribDataUpdateSyncEventPlaybackPos* attrMySyncEventPosIn,  // Adjusted value required.
  AttribDataUpdateSyncEventPlaybackPos* attrMySyncEventPosOut, // Adjusted and real values, Capped or looped appropriately.
                                                               // Can be the same as attrSyncEventPosIn.
  AttribDataPlaybackPos*                attrMyFractionOut,     // Adjusted and real values.
  AttribDataPlaybackPos*                attrMyTimeOut)         // Adjusted and real values.
{
  calcPlaybackValuesFromAbsEventPosAdjSpace(
    mySyncEventTrackIn,
    loopableIn,
    attrMySyncEventPosIn->m_absPosAdj,
    attrMySyncEventPosOut,
    attrMyFractionOut->m_previousPosAdj,
    attrMyTimeOut->m_previousPosAdj,
    attrMyFractionOut->m_previousPosReal,
    attrMyTimeOut->m_previousPosReal);
}

//----------------------------------------------------------------------------------------------------------------------
void calcCurrentPlaybackValuesFromParentDeltaSyncEventPosAdjSpace(
  EventTrackSync&                       mySyncEventTrackIn,
  AttribDataUpdateSyncEventPlaybackPos* attrParentDeltaSyncEventPosIn, // Delta sync event position.
  AttribDataUpdateSyncEventPlaybackPos* attrMyPreviousSyncEventPosIn,  // Adjusted and real values.
                                                                       //   Must already have been scaled to fit
                                                                       //   current sync event track.
  bool                                  loopableIn,
  AttribDataPlaybackPos*                attrMyFractionOut,             // Adjusted and real values.
                                                                       // Previous values must already be set.
  AttribDataPlaybackPos*                attrMyTimeOut,                 // Adjusted and real values.
  AttribDataUpdateSyncEventPlaybackPos* attrMyCurrentSyncEventPosOut,  // Adjusted and real values.
  AttribDataUpdatePlaybackPos*          attrUpdateTimePosOut)          // Real space.
{
  //---------------------------------
  // Generate real space values.

  // Event position.
  if (!loopableIn && mySyncEventTrackIn.atEndOfLastEvent(attrMyPreviousSyncEventPosIn->m_absPosReal))
  {
    // At the end of a non looping clip.
    attrMyCurrentSyncEventPosOut->m_absPosReal = attrMyPreviousSyncEventPosIn->m_absPosReal;
    attrMyCurrentSyncEventPosOut->m_deltaPos.set(0, 0.0f);
  }
  else
  {
    attrMyCurrentSyncEventPosOut->m_absPosReal.setFraction(attrParentDeltaSyncEventPosIn->m_absPosReal.fraction());
    attrMyCurrentSyncEventPosOut->m_absPosReal.setIndex(
      attrMyPreviousSyncEventPosIn->m_absPosReal.index() + attrParentDeltaSyncEventPosIn->m_deltaPos.index());
    mySyncEventTrackIn.limitToSyncEventSpace(attrMyCurrentSyncEventPosOut->m_absPosReal, loopableIn);
    attrMyCurrentSyncEventPosOut->m_deltaPos = attrParentDeltaSyncEventPosIn->m_deltaPos;
  }

  // Fraction position.
  attrMyFractionOut->m_currentPosReal = mySyncEventTrackIn.getRealPosFractionFromRealSyncEventPos(attrMyCurrentSyncEventPosOut->m_absPosReal);
  NMP_ASSERT(attrMyFractionOut->m_currentPosReal >= 0.0f && attrMyFractionOut->m_currentPosReal <= 1.0f);
  if (attrMyFractionOut->m_currentPosReal < attrMyFractionOut->m_previousPosReal)
    attrMyFractionOut->m_delta = 1.0f - attrMyFractionOut->m_previousPosReal + attrMyFractionOut->m_currentPosReal;
  else
    attrMyFractionOut->m_delta = attrMyFractionOut->m_currentPosReal - attrMyFractionOut->m_previousPosReal;
  NMP_ASSERT(attrMyFractionOut->m_delta >= 0.0f);

  // Time position.
  attrMyTimeOut->m_currentPosReal = mySyncEventTrackIn.getDuration() * attrMyFractionOut->m_currentPosReal;
  NMP_ASSERT(attrMyTimeOut->m_currentPosReal >= 0.0f && attrMyTimeOut->m_currentPosReal <= mySyncEventTrackIn.getDuration());
  attrMyTimeOut->m_delta = mySyncEventTrackIn.getDuration() * attrMyFractionOut->m_delta;

  // Calculate real space time update value that can be used by children.
  if (attrUpdateTimePosOut)
  {
    attrUpdateTimePosOut->m_isFraction = false;
    attrUpdateTimePosOut->m_isAbs = false;
    attrUpdateTimePosOut->m_value = attrMyTimeOut->m_delta;
  }

  //---------------------------------
  // Generate adjusted space values.
  attrMyFractionOut->m_currentPosAdj = mySyncEventTrackIn.convRealFractionToAdjFraction(attrMyFractionOut->m_currentPosReal);
  attrMyTimeOut->m_currentPosAdj = mySyncEventTrackIn.getDuration() * attrMyFractionOut->m_currentPosAdj;
  NMP_ASSERT(attrMyTimeOut->m_currentPosAdj >= 0.0f && attrMyTimeOut->m_currentPosAdj <= mySyncEventTrackIn.getDuration());
  mySyncEventTrackIn.convEventInRealSpaceToAdjustedSpace(
    attrMyCurrentSyncEventPosOut->m_absPosReal,
    attrMyCurrentSyncEventPosOut->m_absPosAdj);

  //---------------------------------
  // Store in the output attrib data that the last valid time update was a delta.
  attrMyTimeOut->setWithDelta();
  attrMyCurrentSyncEventPosOut->setWithDelta();
  attrMyFractionOut->setWithDelta();
}

//----------------------------------------------------------------------------------------------------------------------
void subTaskMirrorSampledEvents(
  AttribDataSampledEvents*       sourceSampledEvents,
  AttribDataSampledEvents*       outputSampledEvents,
  AttribDataMirroredAnimMapping* mirroredAnimMapping)
{
  //------------------
  // Triggered events.
  TriggeredDiscreteEventsBuffer* sourceEvents = sourceSampledEvents->m_discreteBuffer;
  TriggeredDiscreteEventsBuffer* outputEvents = outputSampledEvents->m_discreteBuffer;
  if (sourceEvents)
  {
    uint32_t numEvents = sourceEvents->getNumTriggeredEvents();
    for (uint32_t i = 0; i != numEvents; ++i)
    {
      const TriggeredDiscreteEvent* sourceEvent = sourceEvents->getTriggeredEvent(i);
      TriggeredDiscreteEvent* outputEvent = outputEvents->getTriggeredEvent(i);

      // Swap the track userdata.
      uint32_t trackUserData = mirroredAnimMapping->findTrackIDMapping(sourceEvent->getSourceTrackUserData());

      // Swap the event userdata.
      uint32_t eventUserData = mirroredAnimMapping->findEventMappingID(sourceEvent->getSourceEventUserData());

      /// Note: We do not swap the runtime ID of the source track.
      uint32_t trackRuntimeID = sourceEvent->getSourceTrackRuntimeID();

      float blendWeight = sourceEvent->getBlendWeight();

      outputEvent->set(trackUserData, trackRuntimeID, eventUserData, blendWeight);
    }
  }

  //------------------
  // Sampled events.
  SampledCurveEventsBuffer* sourceCurveEvents = sourceSampledEvents->m_curveBuffer;
  SampledCurveEventsBuffer* outputCurveEvents = outputSampledEvents->m_curveBuffer;
  if (sourceCurveEvents)
  {
    uint32_t numEvents = sourceCurveEvents->getNumSampledEvents();
    for (uint32_t i = 0; i != numEvents; ++i)
    {
      const SampledCurveEvent* sourceEvent = sourceCurveEvents->getSampledEvent(i);
      SampledCurveEvent* outputEvent = outputCurveEvents->getSampledEvent(i);

      // Swap the track userdata.
      uint32_t trackUserData = mirroredAnimMapping->findTrackIDMapping(sourceEvent->getSourceTrackUserData());
      outputEvent->setSourceTrackUserData(trackUserData);

      // Swap the event userdata.
      uint32_t eventUserData = mirroredAnimMapping->findEventMappingID(sourceEvent->getSourceEventUserData());
      outputEvent->setSourceEventUserData(eventUserData);

      /// Note: We do not swap the runtime ID of the source track.
      outputEvent->setSourceTrackRuntimeID(sourceEvent->getSourceTrackRuntimeID());

      outputEvent->setValue(sourceEvent->getValue());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief For debugging of tasks primarily on SPU.
///
/// Displays for each task parameter:
///  + Attrib pointer location.
///  + Size.
///  + Alignment.
void displayTaskParameterAttribInfo(
  Dispatcher::TaskParameters* parameters,
  uint32_t                    logPriority)
{
  (void)logPriority;
#ifdef NM_HOST_CELL_SPU
  spu_printf("  Task parameter list info:\n");
  for (uint32_t i = 0; i < parameters->m_numParameters; ++i)
  {
    spu_printf(
      "    Param %d. Ptr = %p, Size = %zu, Align = %zu\n",
      i,
      (void*)(parameters->m_parameters[i].m_attribDataHandle.m_attribData),
      parameters->m_parameters[i].m_attribDataHandle.m_format.size,
      parameters->m_parameters[i].m_attribDataHandle.m_format.alignment);
  }
  NMP::SPUDMAController::spuMemoryInfo(cellSpursGetCurrentSpuId());
#else
  NET_LOG_MESSAGE(logPriority, "  Task parameter list info:\n");
  for (uint32_t i = 0; i < parameters->m_numParameters; ++i)
  {
    NET_LOG_MESSAGE(
      logPriority,
      "    Param %d. Ptr = %p, Size = %d, Align = %d\n",
      i,
      (void*)(parameters->m_parameters[i].m_attribDataHandle.m_attribData),
      parameters->m_parameters[i].m_attribDataHandle.m_format.size,
      parameters->m_parameters[i].m_attribDataHandle.m_format.alignment);
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void calculateCharSpaceTransforms(MR::AnimRigDef* rig, const NMP::DataBuffer* localTransf, NMP::DataBuffer* charSpaceTransf)
{
  NMP_ASSERT(rig != 0);
  NMP_ASSERT(rig->getHierarchy()->getParentIndex(0) == -1);

  NMP::Vector3 workingPos(NMP::Vector3::InitZero);
  NMP::Quat workingAng(NMP::Quat::kIdentity);

  // Compute the character space transforms
  for (uint32_t i = 0; i < rig->getNumBones(); ++i)
  {
    // Check for a valid channel, otherwise invalidate.
    if (localTransf->hasChannel(i))
    {
      charSpaceTransf->setChannelUsed(i);

      workingAng = *(localTransf->getPosQuatChannelQuat(i));
      workingPos = *(localTransf->getPosQuatChannelPos(i));
    }
    else
    {
      // We need to do this in case this channel has no parent to get a transform from later.
      workingAng.identity();
      workingPos.setToZero();

      charSpaceTransf->setChannelUnused(i);
    }

    // Compute the accumulated transform.  We have to do this regardless of whether or not the channel
    // is used because it may have children that depend on it that are used.  However, unused
    // channels can legally be assumed to have an identity transform - that means we can just copy
    // their accumulated transform from their parent.
    int32_t parentIndex = rig->getParentBoneIndex(i);
    if (parentIndex > 0)
    {
      // OPTIMISE  Usually quat operations are very slow, therefore it's better to avoid them if you
      // can.  However, there is no harm in always doing this first block and removing the conditional,
      // if it is even slower.
      if (charSpaceTransf->hasChannel(i))
      {
        workingAng = *charSpaceTransf->getPosQuatChannelQuat(parentIndex) * workingAng;
        workingPos = *charSpaceTransf->getPosQuatChannelPos(parentIndex) +
          (*charSpaceTransf->getPosQuatChannelQuat(parentIndex)).rotateVector(workingPos);
      }
      else
      {
        workingAng = *charSpaceTransf->getPosQuatChannelQuat(parentIndex);
        workingPos = *charSpaceTransf->getPosQuatChannelPos(parentIndex);
      }

      // Accumulation involves a lot of multiplies, so you have to normalise quats at every stage.
      // OPTIMISE - You can experiment with using fastNormalise() to see if it is sufficiently accurate.
      workingAng.normalise();
    }

    charSpaceTransf->setPosQuatChannelPos(i, workingPos);
    charSpaceTransf->setPosQuatChannelQuat(i, workingAng);
  }
}

#ifdef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
/// \brief For debugging of tasks on SPU.
///
/// Displays for each task parameter:
///  + Attrib pointer location.
///  + Size.
///  + Alignment.
bool displayTaskParameterAttribInfoSPU(MR::Task* task)
{
  spu_printf("  Task parameter list info:\n");
  if (task->m_numParams < 0 || task->m_numParams > 200)
  {
    spu_printf("Task params: %i\n", task->m_numParams);
    return false;
  }

  for (uint32_t i = 0; i < task->m_numParams; ++i)
  {
    spu_printf(
      "    Param %d. Ptr = %p, Size = %zu, Align = %zu\n",
      i,
      (void*)(task->m_params[i].m_attribDataHandle.m_attribData),
      task->m_params[i].m_attribDataHandle.m_format.size,
      task->m_params[i].m_attribDataHandle.m_format.alignment);
  }

  NMP::SPUDMAController::spuMemoryInfo(cellSpursGetCurrentSpuId());
  return true;
}
#endif // NM_HOST_CELL_SPU

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
