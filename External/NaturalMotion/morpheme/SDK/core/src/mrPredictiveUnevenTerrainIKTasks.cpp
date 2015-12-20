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
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMProfiler.h"
#include "morpheme/mrCommonTasks.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrCharacterControllerAttribData.h"
#include "morpheme/mrUnevenTerrainIK.h"
#include "morpheme/mrUnevenTerrainUtilities.h"
#include "morpheme/mrTrajectoryPrediction.h"
#include "morpheme/mrDebugMacros.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::footLiftingTargetPredictor
/// \brief A class that updates the position of the foot lifting target by finding the
/// highest terrain peak from the supplied hit information, computing the lifting vector
/// between the previous footbase position and highest terrain peak, then projecting the
/// initial source animation footbase position onto this vector.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class footLiftingTargetPredictor
{
public:
  footLiftingTargetPredictor() {}
  ~footLiftingTargetPredictor() {}

  // Compute the foot lifting target by using the supplied terrain ray cast hit information
  // to determine a suitable lifting curve
  bool terrainDataSolve(
    const NMP::Vector3& prevWorldFootbaseLiftingPos,  ///< IN:  The previous frame's driven footbase lifting position (from the IK state)
    const NMP::Vector3& initWorldFootbasePos,         ///< IN:  This frames initial animation footbase position
    NMP::Vector3& targetWorldFootbaseLiftingPos,      ///< OUT: The target footbase lifting position
    NMP::Vector3& targetWorldFootbaseLiftingNormal    ///< OUT: The target footbase lifting normal
  );

  NM_INLINE void init(const NMP::Matrix34& trajectoryBasis)
  {
    m_numSamples = 0;
    m_numLiftingPeaks = 0;
    m_trajectoryBasis = trajectoryBasis;
  }

  NM_INLINE void appendTerrainHitInfo(
    const NMP::Vector3& worldFootbaseLiftingPos,
    const NMP::Vector3& worldFootbaseLiftingNormal,
    bool worldFootbaseLiftingNormalSlopeValid)
  {
    NMP_ASSERT(m_numSamples < 3);
    m_worldFootbaseLiftingPos[m_numSamples] = worldFootbaseLiftingPos;
    m_worldFootbaseLiftingNormal[m_numSamples] = worldFootbaseLiftingNormal;
    m_worldFootbaseLiftingNormalSlopeValid[m_numSamples] = worldFootbaseLiftingNormalSlopeValid;
    m_numSamples++;
  }

protected:
  // Trajectory rotation basis: X - lateral, Y - world up, Z - forward motion
  NMP::Matrix34 m_trajectoryBasis;

  // Terrain ray cast hit information
  NMP::Vector3 m_worldFootbaseLiftingPos[3];
  NMP::Vector3 m_worldFootbaseLiftingNormal[3];

  // Workspace vectors for the canonical foot lifting coordinate frame
  NMP::Vector3 m_adjPrevToLiftingPos[3];
  NMP::Vector3 m_adjLiftingNormal[3];

  // Vector of terrain hit and intersection points
  NMP::Vector3 m_adjPrevToLiftingPeak[5]; // 2*N - 1
  NMP::Vector3 m_liftingPeakNormal[5];

  // Flags indicating whether the terrain slope at the hit points is too steep
  bool m_worldFootbaseLiftingNormalSlopeValid[3];

  // Sample information
  uint32_t m_numSamples;
  uint32_t m_numLiftingPeaks;
};

//----------------------------------------------------------------------------------------------------------------------
bool footLiftingTargetPredictor::terrainDataSolve(
  const NMP::Vector3& prevWorldFootbaseLiftingPos,  ///< The previous frame's driven foot lifting position (from the IK state)
  const NMP::Vector3& initWorldFootbasePos,         ///< This frames initial animation footbase position
  NMP::Vector3& targetWorldFootbaseLiftingPos,      ///< The target foot lifting position
  NMP::Vector3& targetWorldFootbaseLiftingNormal)   ///< The target foot lifting normal
{
  // Check that we have at least the current and next terrain hit information
  if (m_numSamples < 2)
    return false;

  //---------------------------
  // Apply the translation that moves the previous foot lifting position back to the origin
  NMP::Vector3 prevFootbaseToInitPosV = initWorldFootbasePos - prevWorldFootbaseLiftingPos;
  NMP::Vector3 meanLiftingPosV(NMP::Vector3::InitZero);
  for (uint32_t i = 0; i < m_numSamples; ++i)
  {
    m_adjPrevToLiftingPos[i] = m_worldFootbaseLiftingPos[i] - prevWorldFootbaseLiftingPos;
    m_adjLiftingNormal[i] = m_worldFootbaseLiftingNormal[i];
    meanLiftingPosV += m_adjPrevToLiftingPos[i];
  }

  //---------------------------
  // Rotate the vectors back to the canonical foot lifting coordinate system.
  // X axis is the sagital plane normal
  // Y axis is the world up axis
  // Z axis is the forward motion direction
  m_trajectoryBasis.inverseRotateVector(prevFootbaseToInitPosV);
  for (uint32_t i = 0; i < m_numSamples; ++i)
  {
    m_trajectoryBasis.inverseRotateVector(m_adjPrevToLiftingPos[i]);

    // Project the terrain normals onto the sagital plane and normalise
    m_trajectoryBasis.inverseRotateVector(m_adjLiftingNormal[i]);
    m_adjLiftingNormal[i].x = 0.0f;
    float norm = m_adjLiftingNormal[i].magnitude();
    if (norm < FLT_EPSILON)
      return false;
    float recipNorm = 1.0f / norm;
    m_adjLiftingNormal[i] *= recipNorm;
  }

  // Compute the vector between the current and next hit points
  float curHitToNextHitZ = m_adjPrevToLiftingPos[m_numSamples-1].z - m_adjPrevToLiftingPos[0].z;
  if (curHitToNextHitZ < 1e-4f)
    return false;

  //---------------------------
  // Compute the current surface patch approximation
  m_adjPrevToLiftingPeak[0] = prevFootbaseToInitPosV;
  float Tay = m_adjLiftingNormal[0].z;
  float Taz = -m_adjLiftingNormal[0].y;
  NMP_ASSERT(fabs(Taz) > 1e-4f);
  float lambda = (prevFootbaseToInitPosV.z - m_adjPrevToLiftingPos[0].z) / Taz;
  m_adjPrevToLiftingPeak[0].y = m_adjPrevToLiftingPos[0].y + lambda * Tay;
  m_liftingPeakNormal[0] = m_worldFootbaseLiftingNormal[0];
  m_numLiftingPeaks = 1;

  //---------------------------
  // Compute the surface intersection hill peaks forward of the init source position
  float Nby, Nbz;
  for (uint32_t i = 1; i < m_numSamples; ++i)
  {
    // Check if the lifting position is forward of the initial animation source position
    if (m_adjPrevToLiftingPos[i].z <= prevFootbaseToInitPosV.z)
      continue;

    // Check if the two surface normals are valid
    if (m_worldFootbaseLiftingNormalSlopeValid[i] && m_worldFootbaseLiftingNormalSlopeValid[i-1])
    {
      // Find the intersection point of the two terrain surfaces to find the peak of the hill
      Tay = m_adjLiftingNormal[i-1].z;
      Taz = -m_adjLiftingNormal[i-1].y;
      Nby = m_adjLiftingNormal[i].y;
      Nbz = m_adjLiftingNormal[i].z;

      float D = Tay * Nby + Taz * Nbz;
      if (fabs(D) > 1e-4f)
      {
        // Compute the terrain peak intersection point relative to the indexed hit point
        NMP::Vector3 deltaHitV = m_adjPrevToLiftingPos[i] - m_adjPrevToLiftingPos[i-1];
        float N = deltaHitV.y * Nby + deltaHitV.z * Nbz;
        lambda = N / D;
        NMP::Vector3 deltaPeakV(0.0f, lambda * Tay, lambda * Taz);

        // Compute the terrain peak intersection point relative to the previous footbase position
        NMP::Vector3 prevFootbaseToPeakV = m_adjPrevToLiftingPos[i-1] + deltaPeakV;

        // Check if the terrain peak intersection point occurs within the lifting region
        // between the current and next terrain hit points
        float w = deltaPeakV.z / deltaHitV.z;
        if (w >= 0.0f && w <= 1.0f && prevFootbaseToPeakV.z > prevFootbaseToInitPosV.z)
        {
          // Check to see if the terrain is a hill (X component of cross product)
          float cp = deltaPeakV.y * deltaHitV.z - deltaHitV.y * deltaPeakV.z;
          if (cp > 0)
          {
            m_adjPrevToLiftingPeak[m_numLiftingPeaks] = prevFootbaseToPeakV;
            NMP::Vector3 peakNormal = m_worldFootbaseLiftingNormal[i-1] * (1.0f - w) + m_worldFootbaseLiftingNormal[i] * w;
            peakNormal.normalise();
            m_liftingPeakNormal[m_numLiftingPeaks] = peakNormal;
            m_numLiftingPeaks++;
          }
        }
      }
    }

    // Append the terrain hit position to the list
    m_adjPrevToLiftingPeak[m_numLiftingPeaks] = m_adjPrevToLiftingPos[i];
    m_liftingPeakNormal[m_numLiftingPeaks] = m_worldFootbaseLiftingNormal[i];
    m_numLiftingPeaks++;
  }

  //---------------------------
  // Find the vector from the previous foot lifting position to the highest peak
  uint32_t highestPeakIndex = 0;
  for (uint32_t i = 1; i < m_numLiftingPeaks; ++i)
  {
    // Determine if the peak vector is higher than the current selection (X component of cross product)
    float cp = m_adjPrevToLiftingPeak[i].y * m_adjPrevToLiftingPeak[highestPeakIndex].z -
               m_adjPrevToLiftingPeak[highestPeakIndex].y * m_adjPrevToLiftingPeak[i].z;
    if (cp > 0)
    {
      highestPeakIndex = i;
    }
  }

  // Project the initial animation footbase position onto the lifting vector
  targetWorldFootbaseLiftingPos = prevFootbaseToInitPosV;
  if (m_adjPrevToLiftingPeak[highestPeakIndex].z > 1e-4f)
  {
    float u = NMP::clampValue(prevFootbaseToInitPosV.z / m_adjPrevToLiftingPeak[highestPeakIndex].z, 0.0f, 1.0f);
    targetWorldFootbaseLiftingPos.y = m_adjPrevToLiftingPeak[highestPeakIndex].y * u;
  }
  else
  {
    targetWorldFootbaseLiftingPos.y = m_adjPrevToLiftingPeak[highestPeakIndex].y;
  }

  // Re-apply the transform from the canonical coordinate frame back to the
  // foot lifting coordinate system at the previous footbase lifting position
  m_trajectoryBasis.rotateVector(targetWorldFootbaseLiftingPos);
  targetWorldFootbaseLiftingPos += prevWorldFootbaseLiftingPos;

  //---------------------------
  // Compute the terrain surface normal vector by normalised linear interpolation. This is a good
  // approximation since most foot lifting will always be performed near to the current hit position.
  float curHitToPeakHitZ = m_adjPrevToLiftingPeak[highestPeakIndex].z - m_adjPrevToLiftingPos[0].z;
  if (curHitToPeakHitZ > 1e-4f)
  {
    float curHitToInitPosZ = prevFootbaseToInitPosV.z - m_adjPrevToLiftingPos[0].z;
    float u = NMP::clampValue(curHitToInitPosZ / curHitToPeakHitZ, 0.0f, 1.0f);
    targetWorldFootbaseLiftingNormal = m_liftingPeakNormal[0] * (1.0f - u) + m_liftingPeakNormal[highestPeakIndex] * u;
    targetWorldFootbaseLiftingNormal.normalise();
  }
  else
  {
    targetWorldFootbaseLiftingNormal = m_liftingPeakNormal[0];
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool projectPointOntoPlane(
  const NMP::Vector3& upAxisWS,               ///< Up axis direction in the world
  const NMP::Vector3& initWorldFootbasePos,   ///< The footbase position to vertically project onto the plane
  const NMP::Vector3& worldTerrainPos,        ///< A point on the terrain surface
  const NMP::Vector3& worldTerrainNormal,     ///< The normal of the terrain
  NMP::Vector3& targetWorldLiftingPos)        ///< The projected point on the terrain surface
{
  float D = upAxisWS.dot(worldTerrainNormal);
  if (fabs(D) < 1e-4f)
    return false;

  NMP::Vector3 initToTerrainPosV = worldTerrainPos - initWorldFootbasePos;
  float N = initToTerrainPosV.dot(worldTerrainNormal);
  float lambda = N / D;
  targetWorldLiftingPos = initWorldFootbasePos + (upAxisWS * lambda);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// GAIT CYCLE TIMINGS
//----------------------------------------------------------------------------------------------------------------------
#ifdef TEST_UT_TASK_LOGGING
void debugEventTracksUT(const EventTrackDurationSet* durationEventTrackSet)
{
  NMP_ASSERT(durationEventTrackSet);
  uint32_t numDurationTracks = durationEventTrackSet->getNumTracks();

  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\n----------------------------------------------------\n");
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "debugEventTracksUT()\n");
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "numDurationTracks = %d\n", numDurationTracks);

  // Sample the duration event tracks at the next sync event position
  for (uint32_t trackIndx = 0; trackIndx < numDurationTracks; ++trackIndx)
  {
    const EventTrackDuration* durationTrack = durationEventTrackSet->getTrack(trackIndx);
    NMP_ASSERT(durationTrack);
    uint32_t trackUserData = durationTrack->getUserData();
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "trackUserData = %d\n", trackUserData);

    uint32_t numDurationEvents = durationTrack->getNumEvents();
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "numDurationEvents = %d\n", numDurationEvents);

    const EventDuration* durationEvent = durationTrack->getEventsListHead();
    while (durationEvent)
    {
      uint32_t eventUserData = durationEvent->getUserData();
      float startEventPos = durationEvent->getSyncEventSpaceStartPoint();
      float endEventPos = startEventPos + durationEvent->getSyncEventSpaceDuration();

      NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS,
                      "  eventUserData = %d, startEventPos = %f, endEventPos = %f\n",
                      eventUserData,
                      startEventPos,
                      endEventPos);

      durationEvent = durationEvent->getNext(durationTrack->getPool());
    }
  }
}
#endif // TEST_UT_TASK_LOGGING

//----------------------------------------------------------------------------------------------------------------------  
namespace
{

//----------------------------------------------------------------------------------------------------------------------
bool findEventLiesWithinSyncEventPos(
  const EventTrackSync* syncEventTrack,
  const EventTrackDuration* eventTrack,
  const SyncEventPos* syncEventPos,
  float& syncEventPosStart,
  float& syncEventPosEnd)
{
  syncEventPosStart = FLT_MAX;
  syncEventPosEnd = -FLT_MAX;
  uint32_t numSyncEvents = syncEventTrack->getNumEvents();
  float pos = syncEventPos->get();
  float fNumSyncEvents = (float)numSyncEvents;
  bool isLoopable = true;
  bool eventFound = false;

  const EventDuration* durationEvent = eventTrack->getEventsListHead();
  while (durationEvent)
  {
    // Lies within the current sync event pos
    float fractionalPosInEvent; // 1.0 = start, 0.0 = end.
    if (durationEvent->liesWithin(
          pos,
          fNumSyncEvents,
          fractionalPosInEvent,
          isLoopable))
    {
      float startEventPos = durationEvent->getSyncEventSpaceStartPoint();
      if (pos < startEventPos)
        startEventPos -= fNumSyncEvents; // Wrapping
      float endEventPos = startEventPos + durationEvent->getSyncEventSpaceDuration();

      // Update the information
      syncEventPosStart = NMP::minimum(syncEventPosStart, startEventPos);
      syncEventPosEnd = NMP::maximum(syncEventPosEnd, endEventPos);
      eventFound = true;
    }

    // Next duration event
    durationEvent = durationEvent->getNext(eventTrack->getPool());
  }

  // Update for wrapping
  if (eventFound)
  {
    if (fNumSyncEvents - (syncEventPosEnd - syncEventPosStart) > 1e-4f)
    {
      // General case
      if (syncEventPosStart < 0.0f)
        syncEventPosStart += fNumSyncEvents;
    }
    else
    {
      // Event spanning the entire clip duration
      syncEventPosStart = 0.0f;
      syncEventPosEnd = fNumSyncEvents;
    }
  }

  return eventFound;
}
  
//----------------------------------------------------------------------------------------------------------------------
void gaitCycleUpdateTimings(
  const AttribDataPlaybackPos* curFrameTimePos,
  const AttribDataSyncEventTrack* syncEventTrackAttrib,
  const AttribDataDurationEventTrackSet* durationEventTrackSetAttrib,
  const AttribDataPredictiveUnevenTerrainPredictionDef* predictionDefAttr,
  AttribDataPredictiveUnevenTerrainPredictionState* predictionStateAttrib,
  bool predictionEnable)
{
  NM_UT_BEGIN_PROFILING("PREDICTIVE_UT_GAIT_CYCLE_TIMINGS");

  NMP_ASSERT(curFrameTimePos);
  NMP_ASSERT(syncEventTrackAttrib);
  NMP_ASSERT(durationEventTrackSetAttrib);
  NMP_ASSERT(predictionDefAttr);
  NMP_ASSERT(predictionStateAttrib);

#ifdef TEST_UT_TASK_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\n----------------------------------------------------\n");
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "gaitCycleUpdateTimings()\n");
#endif

  uint32_t numLimbs = predictionStateAttrib->m_numLimbs;
  bool isDataValidOld = predictionStateAttrib->m_isDataValid;
  float curSyncEventPosOld = predictionStateAttrib->m_curSyncEventPos;

  //---------------------------
  if (!predictionEnable)
  {
    // Invalidate the foot cycle timing information
    predictionStateAttrib->m_isDataValid = false;
    predictionStateAttrib->m_isAbs = false;
    predictionStateAttrib->m_curSyncEventPos = 0;
    for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
    {
      AttribDataPredictiveUnevenTerrainPredictionState::LimbState* predictionLimbState = predictionStateAttrib->m_limbStates[limbIndex];
      NMP_ASSERT(predictionLimbState);
      predictionLimbState->resetFootCycleTiming();
    }

    NM_UT_END_PROFILING();
    return;
  }

#ifdef TEST_UT_TASK_LOGGING
  debugEventTracksUT(durationEventTrackSetAttrib->m_durationEventTrackSet);
#endif

  //---------------------------
  // Sync event track
  NMP_ASSERT(syncEventTrackAttrib);
  const EventTrackSync* syncEventTrack = &(syncEventTrackAttrib->m_syncEventTrack);
  uint32_t numSyncEvents = syncEventTrack->getNumEvents();

  const EventTrackDurationSet* durationEventTrackSet = durationEventTrackSetAttrib->m_durationEventTrackSet;
  NMP_ASSERT(durationEventTrackSet);
  uint32_t numEventTracks = durationEventTrackSet->getNumTracks();

  // Get the current playback position as a time fraction through the sync event track
  float durationReciprocal = syncEventTrack->getDurationReciprocal();
  TimeFraction curFrameTimeFrac = curFrameTimePos->m_currentPosReal * durationReciprocal;
  // Ensure that we explicitly wrap a time at the end of the event track back to the start
  while(curFrameTimeFrac >= 1.0f)
    curFrameTimeFrac -= 1.0f;

  // Convert the current time fraction to real sync event space
  SyncEventPos curFrameEventPosReal;
  syncEventTrack->getRealPosInfoFromRealFraction(curFrameTimeFrac, curFrameEventPosReal);
  predictionStateAttrib->m_curSyncEventPos = curFrameEventPosReal.get();
  predictionStateAttrib->m_isAbs = curFrameTimePos->m_setWithAbs;

#ifdef TEST_UT_TASK_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "syncEventTrack duration = %f\n", syncEventTrack->getDuration());
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "m_isAbs = %d\n", predictionStateAttrib->m_isAbs);
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "m_curSyncEventPos = %f\n", predictionStateAttrib->m_curSyncEventPos);
#endif

  //---------------------------
  // Per limb timing information
  for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
  {
#ifdef TEST_UT_TASK_LOGGING
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\nlimbIndex = %d\n", limbIndex);
#endif

    const AttribDataPredictiveUnevenTerrainPredictionDef::LimbData* predictionDefLimbData = predictionDefAttr->m_limbInfo[limbIndex];
    AttribDataPredictiveUnevenTerrainPredictionState::LimbState* predictionLimbState = predictionStateAttrib->m_limbStates[limbIndex];
    NMP_ASSERT(predictionDefLimbData);
    NMP_ASSERT(predictionLimbState);
    uint32_t nextSyncEventPosOld = predictionLimbState->m_nextSyncEventPos;
    predictionLimbState->m_nextSyncEventPos = INVALID_EVENT_INDEX;

    //---------------------------
    // Get the event track with the required user data
    const EventTrackDuration* eventTrack = NULL;
    for (uint32_t i = 0; i < numEventTracks; ++i)
    {
      const EventTrackDuration* track = durationEventTrackSet->getTrack(i);
      NMP_ASSERT(track);
      if (track->getUserData() == predictionDefLimbData->m_eventTrackID)
      {
        eventTrack = track;
        break;
      }
    }

    // Handle case if we didn't find a matching event track
    if (!eventTrack)
    {
#ifdef TEST_UT_TASK_LOGGING
      NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "Event track with user data %d not found\n", predictionDefLimbData->m_eventTrackID);
#endif
      continue;
    }

    //---------------------------
    // Find the next marked sync event from the current event position. We need to recompute
    // the next sync event at every frame since any change in the blending weights could have
    // modified the configuration of the sync and duration events.
    // Previous and next duration event information
    float prevSyncEventPosStart = 0.0f;
    float prevSyncEventPosEnd = 0.0f;
    float nextSyncEventPosStart = 0.0f;
    float nextSyncEventPosEnd = 0.0f;
    SyncEventPos syncEventPos(0, 0.0f);

    uint32_t nextEventIndx = curFrameEventPosReal.index();
    for (uint32_t syncEventIndx = 0; syncEventIndx < numSyncEvents; ++syncEventIndx)
    {
      nextEventIndx = (nextEventIndx + 1) % numSyncEvents;
      syncEventPos.setIndex(nextEventIndx);

      if (findEventLiesWithinSyncEventPos(
            syncEventTrack,
            eventTrack,
            &syncEventPos,
            nextSyncEventPosStart,
            nextSyncEventPosEnd))
      {
        predictionLimbState->m_nextSyncEventPos = nextEventIndx;
        break;
      }
    }

    //---------------------------
    // Check for no valid markup event data
    if (predictionLimbState->m_nextSyncEventPos == INVALID_EVENT_INDEX)
    {
#ifdef TEST_UT_TASK_LOGGING
      NET_LOG_MESSAGE(
        NMP::LOG_PRIORITY_ALWAYS,
        "Event track with user data %d has no valid duration events\n",
        predictionDefLimbData->m_eventTrackID);
#endif
      continue;
    }

#ifdef TEST_UT_TASK_LOGGING
    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "nextSyncEventPosStart = %f, nextSyncEventPosEnd = %f\n",
      nextSyncEventPosStart,
      nextSyncEventPosEnd);
#endif

    //---------------------------
    // Find the previous marked sync event from the current event position. We need to recompute
    // the prev sync event at every frame since any change in the blending weights could have
    // modified the configuration of the sync and duration events.
    uint32_t prevEventIndx = predictionLimbState->m_nextSyncEventPos;
    uint32_t syncEventIndx;
    uint32_t lastSyncEventIndx = numSyncEvents - 1;
    for (syncEventIndx = 0; syncEventIndx < lastSyncEventIndx; ++syncEventIndx)
    {
      // Do modulo arithmetic correctly
      if (prevEventIndx == 0)
        prevEventIndx = lastSyncEventIndx;
      else
        prevEventIndx--;
      syncEventPos.setIndex(prevEventIndx);

      if (findEventLiesWithinSyncEventPos(
            syncEventTrack,
            eventTrack,
            &syncEventPos,
            prevSyncEventPosStart,
            prevSyncEventPosEnd))
      {
        predictionLimbState->m_prevSyncEventPos = prevEventIndx;
        break;
      }
    }

    //---------------------------
    // Wraps to the same event
    if (syncEventIndx == lastSyncEventIndx)
    {
      predictionLimbState->m_prevSyncEventPos = predictionLimbState->m_nextSyncEventPos;
      prevSyncEventPosStart = nextSyncEventPosStart;
      prevSyncEventPosEnd = nextSyncEventPosEnd;
    }
    else
    {
      // Check if prev, next event ranges are the same
      float distForward, distBackward, distStart, distEnd;
      const float errorTol = 1e-4f;

      // Start
      distForward = fabs(nextSyncEventPosStart - prevSyncEventPosStart);
      distBackward = numSyncEvents - distForward;
      distStart = NMP::minimum(distForward, distBackward);
      // End
      distForward = fabs(nextSyncEventPosEnd - prevSyncEventPosEnd);
      distBackward = numSyncEvents - distForward;
      distEnd = NMP::minimum(distForward, distBackward);

      if (distStart < errorTol && distEnd < errorTol)
      {
        // Set the sync event indices to be the same since the duration event spans more than
        // one sync event
        predictionLimbState->m_prevSyncEventPos = predictionLimbState->m_nextSyncEventPos;
        prevSyncEventPosStart = nextSyncEventPosStart;
        prevSyncEventPosEnd = nextSyncEventPosEnd;
      }
    }
    NMP_ASSERT(predictionLimbState->m_prevSyncEventPos != INVALID_EVENT_INDEX);

#ifdef TEST_UT_TASK_LOGGING
    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "prevSyncEventPosStart = %f, prevSyncEventPosEnd = %f\n",
      prevSyncEventPosStart,
      prevSyncEventPosEnd);
#endif

    //---------------------------
    // Check if the gait cycle has looped
    predictionLimbState->m_hasFootCycleLooped = false;
    if (!predictionStateAttrib->m_isAbs && isDataValidOld && nextSyncEventPosOld != INVALID_EVENT_INDEX)
    {
      // Check if the current sync event position has wrapped over the track
      if (predictionStateAttrib->m_curSyncEventPos < curSyncEventPosOld)
      {
        // Wrapping case (current pos is less than previous pos). Check if the
        // old nextSyncEventPos lies between the old and new curSyncEventPos values,
        // with consideration to wrapping.
        if (nextSyncEventPosOld > curSyncEventPosOld ||
            nextSyncEventPosOld <= predictionStateAttrib->m_curSyncEventPos)
        {
          predictionLimbState->m_hasFootCycleLooped = true;

#ifdef TEST_UT_TASK_LOGGING
          NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "hasFootCycleLooped (wrap)\n");
#endif
        }
      }
      else
      {
        // General case (current pos is greater than the old pos). Check if the
        // old nextSyncEventPos lies between the old and new curSyncEventPos values.
        if (nextSyncEventPosOld > curSyncEventPosOld &&
            nextSyncEventPosOld <= predictionStateAttrib->m_curSyncEventPos)
        {
          predictionLimbState->m_hasFootCycleLooped = true;

#ifdef TEST_UT_TASK_LOGGING
          NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "hasFootCycleLooped (general)\n");
#endif
        }
      }
    }

    //---------------------------
    // Time to gait cycle event positions relative to the current pos
    TimeFraction prevFraction = syncEventTrack->getRealPosFractionFromRealSyncEventPos(predictionLimbState->m_prevSyncEventPos, 0.0f);
    // Get sync event index and fraction for end of previous contact phase
    uint32_t syncEventIndex = (uint32_t)prevSyncEventPosEnd;
    float syncEventFrac = prevSyncEventPosEnd - ((float) syncEventIndex);
    // Wrapping if the duration event lasts for the entire clip range
    if (syncEventIndex >= numSyncEvents)
      syncEventIndex -= numSyncEvents;
    TimeFraction prevEndFraction = syncEventTrack->getRealPosFractionFromRealSyncEventPos(syncEventIndex, syncEventFrac);
    // Make prevEndFraction relative to prevFraction
    prevEndFraction -= prevFraction;
    if (prevEndFraction <= 0.0f)
      prevEndFraction += 1.0f;

    TimeFraction nextFraction = syncEventTrack->getRealPosFractionFromRealSyncEventPos(predictionLimbState->m_nextSyncEventPos, 0.0f);
    TimeFraction nextStartFraction = syncEventTrack->getRealPosFractionFromRealSyncEventPos(nextSyncEventPosStart);
    // Make nextStartFraction relative to nextFraction
    nextStartFraction -= nextFraction;
    if (nextStartFraction >= 0.0f)
      nextStartFraction -= 1.0f;

    // Make nextFraction relative to curFrameTimeFrac
    nextFraction -= curFrameTimeFrac;
    if (nextFraction <= 0.0f)
      nextFraction += 1.0f;

    // Make prevFraction relative to curFrameTimeFrac
    prevFraction -= curFrameTimeFrac;
    if (prevFraction >= nextFraction)
      prevFraction -= 1.0f;

    // Compute relative timings to event positions
    predictionLimbState->m_timeToPrevSyncEvent = prevFraction * syncEventTrack->getDuration();
    predictionLimbState->m_timeToPrevEnd = (prevFraction + prevEndFraction) * syncEventTrack->getDuration();
    predictionLimbState->m_timeToNextSyncEvent = nextFraction * syncEventTrack->getDuration();
    predictionLimbState->m_timeToNextStart = (nextFraction + nextStartFraction) * syncEventTrack->getDuration();
    // Clamp prev contact duration to next sync event
    if (predictionLimbState->m_timeToPrevEnd > predictionLimbState->m_timeToNextSyncEvent)
      predictionLimbState->m_timeToPrevEnd = predictionLimbState->m_timeToNextSyncEvent;
    // Clamp next contact duration to prev sync event
    if (predictionLimbState->m_timeToNextStart < predictionLimbState->m_timeToPrevSyncEvent)
      predictionLimbState->m_timeToNextStart = predictionLimbState->m_timeToPrevSyncEvent;
    // Find mid sync event position
    predictionLimbState->m_timeToMid = 0.5f * (predictionLimbState->m_timeToPrevEnd + predictionLimbState->m_timeToNextStart);
    // Clamp prev contact duration to mid sync event
    if (predictionLimbState->m_timeToPrevEnd > predictionLimbState->m_timeToMid)
      predictionLimbState->m_timeToPrevEnd = predictionLimbState->m_timeToMid;
    // Clamp next contact duration to mid sync event
    if (predictionLimbState->m_timeToNextStart < predictionLimbState->m_timeToMid)
      predictionLimbState->m_timeToNextStart = predictionLimbState->m_timeToMid;
    NMP_ASSERT(predictionLimbState->m_timeToPrevSyncEvent < predictionLimbState->m_timeToNextSyncEvent);
    NMP_ASSERT(predictionLimbState->m_timeToPrevSyncEvent < predictionLimbState->m_timeToPrevEnd);
    NMP_ASSERT(predictionLimbState->m_timeToPrevEnd <= predictionLimbState->m_timeToMid);
    NMP_ASSERT(predictionLimbState->m_timeToMid <= predictionLimbState->m_timeToNextStart);
    NMP_ASSERT(predictionLimbState->m_timeToNextStart <= predictionLimbState->m_timeToNextSyncEvent);

    // Compute the fractions through the foot cycle
    float recipCycleDuration = 1.0f / (predictionLimbState->m_timeToNextSyncEvent - predictionLimbState->m_timeToPrevSyncEvent);
    predictionLimbState->m_timeFractionInFootCycle = -predictionLimbState->m_timeToPrevSyncEvent * recipCycleDuration;
    NMP_ASSERT(predictionLimbState->m_timeFractionInFootCycle < 1.0f); // Should be wrapped to 0
    predictionLimbState->m_timeFractionOfMidInFootCycle = (predictionLimbState->m_timeToMid - predictionLimbState->m_timeToPrevSyncEvent) * recipCycleDuration;

    //---------------------------
    // Compute the current phase of the gait cycle
    if (predictionLimbState->m_timeToMid > 0.0f)
    {
      if (predictionLimbState->m_timeToPrevEnd > 0.0f)
        predictionLimbState->m_curLimbPhase = AttribDataPredictiveUnevenTerrainPredictionState::kPrevContact;
      else
        predictionLimbState->m_curLimbPhase = AttribDataPredictiveUnevenTerrainPredictionState::kInitialSwing;
    }
    else
    {
      if (predictionLimbState->m_timeToNextStart > 0.0f)
        predictionLimbState->m_curLimbPhase = AttribDataPredictiveUnevenTerrainPredictionState::kTerminalSwing;
      else
        predictionLimbState->m_curLimbPhase = AttribDataPredictiveUnevenTerrainPredictionState::kNextContact;
    }

    //---------------------------
    // Debug info
#ifdef TEST_UT_TASK_LOGGING
    uint32_t indx = predictionLimbState->m_curLimbPhase;
    if (indx > MR::AttribDataPredictiveUnevenTerrainPredictionState::kNumPhaseTypes)
      indx = 5;
    const char* gaitPhaseNames[6] =
    {
      "kPrevContact",
      "kInitialSwing",
      "kTerminalSwing",
      "kNextContact",
      "kNumPhaseTypes",
      "kInvalidPhase"
    };
    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "m_curLimbPhase = %d (%s)\n",
      predictionLimbState->m_curLimbPhase,
      gaitPhaseNames[indx]);

    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "m_hasFootCycleLooped = %d\n", predictionLimbState->m_hasFootCycleLooped);
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "m_timeToPrevSyncEvent = %f\n", predictionLimbState->m_timeToPrevSyncEvent);
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "m_timeToPrevEnd = %f\n", predictionLimbState->m_timeToPrevEnd);
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "m_timeToMid = %f\n", predictionLimbState->m_timeToMid);
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "m_timeToNextStart = %f\n", predictionLimbState->m_timeToNextStart);
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "m_timeToNextSyncEvent = %f\n", predictionLimbState->m_timeToNextSyncEvent);
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "m_timeFractionInFootCycle = %f\n", predictionLimbState->m_timeFractionInFootCycle);
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "m_timeFractionOfMidInFootCycle = %f\n", predictionLimbState->m_timeFractionOfMidInFootCycle);
#endif
  }

  // Set the prediction data as valid
  predictionStateAttrib->m_isDataValid = true;

  NM_UT_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void trajectoryPrediction(
  const AttribDataBasicUnevenTerrainSetup* setupAttrib,
  const AttribDataCharacterProperties* characterControllerAttrib,
  const AttribDataBasicUnevenTerrainIKSetup* inputIKSetupAttrib,
  const AttribDataUpdatePlaybackPos* networkUpdateTimeAttrib,
  AttribDataPredictiveUnevenTerrainPredictionState* predictionStateAttrib,
  bool predictionEnable)
{
  NM_UT_BEGIN_PROFILING("PREDICTIVE_UT_TRAJECTORY_PREDICTION");

  NMP_ASSERT(setupAttrib);
  NMP_ASSERT(characterControllerAttrib);
  NMP_ASSERT(inputIKSetupAttrib);
  NMP_ASSERT(networkUpdateTimeAttrib);
  NMP_ASSERT(predictionStateAttrib);

#ifdef TEST_UT_TASK_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\n----------------------------------------------------\n");
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "trajectoryPrediction()\n");
#endif

  uint32_t numLimbs = predictionStateAttrib->m_numLimbs;
  const float midForwardPredictionFraction = 0.35f;

  const NMP::Matrix34& curWorldRootTransform = characterControllerAttrib->m_worldRootTransform;

  //---------------------------
  // Compute the predicted trajectory delta (in the co-ordinate frame of the previous frame's controller root)
  NMP::Vector3 trajDeltaVelPos(NMP::Vector3::InitZero);
  NMP::Vector3 trajDeltaVelQuat(NMP::Vector3::InitZero);
  if (!networkUpdateTimeAttrib->m_isAbs && predictionEnable)
  {
    // Get the network update delta time
    NMP_ASSERT(networkUpdateTimeAttrib->m_value > 0.0f);
    float deltaTime = networkUpdateTimeAttrib->m_value;
    float recipDeltaTime = 1.0f / deltaTime;

    // Compute the up axis direction for foot lifting in the frame of the world
    NMP::Vector3 upAxisWS;
    curWorldRootTransform.rotateVector(setupAttrib->m_upAxis, upAxisWS);

    // Get the true character controller trajectory delta (This includes any corrections that
    // are required to move the controller over the terrain surface)
    const NMP::Matrix34& prevWorldRootTransform = characterControllerAttrib->m_prevWorldRootTransform;
    NMP::Matrix34 invPrevWorldRootTransform;
    invPrevWorldRootTransform.invertFast(prevWorldRootTransform);
    NMP::Matrix34 deltaWorldRootTransform;
    deltaWorldRootTransform.multiply(curWorldRootTransform, invPrevWorldRootTransform);

    // Convert the character controller delta to pos and quat components
    NMP::Quat deltaWorldRootTransformQuat = deltaWorldRootTransform.toQuat();
    NMP::Vector3 deltaWorldRootTransformPos = deltaWorldRootTransform.translation();

#ifdef TEST_UT_TASK_LOGGING
    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "deltaWorldRootTransformQuat = %f %f %f %f\n",
      deltaWorldRootTransformQuat.x,
      deltaWorldRootTransformQuat.y,
      deltaWorldRootTransformQuat.z,
      deltaWorldRootTransformQuat.w);

    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "deltaWorldRootTransformPos = %f %f %f\n",
      deltaWorldRootTransformPos.x,
      deltaWorldRootTransformPos.y,
      deltaWorldRootTransformPos.z);
#endif

    // Compute the rotational angular velocity vector
    NMP_ASSERT(deltaWorldRootTransformQuat.w >= 0.0f);
    NMP::Vector3 deltaWorldRootTransformAngVel = deltaWorldRootTransformQuat.fastLog() * (2.0f * recipDeltaTime);

    // Compute the translation velocity vector
    NMP::Vector3 deltaWorldRootTransformPosVel = deltaWorldRootTransformPos * recipDeltaTime;

#ifdef TEST_UT_TASK_LOGGING
    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "deltaWorldRootTransformAngVel = %f %f %f\n",
      deltaWorldRootTransformAngVel.x,
      deltaWorldRootTransformAngVel.y,
      deltaWorldRootTransformAngVel.z);

    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "deltaWorldRootTransformPosVel = %f %f %f\n",
      deltaWorldRootTransformPosVel.x,
      deltaWorldRootTransformPosVel.y,
      deltaWorldRootTransformPosVel.z);
#endif

    // Append the sample into the trajectory history
    predictionStateAttrib->appendTrajectorySample(
      deltaTime,
      deltaWorldRootTransformPosVel,
      deltaWorldRootTransformAngVel);

    //---------------------------
    // Apply smoothing to the samples
    float trajDeltaTime = 0.0f;
    NMP::Vector3 trajDeltaPosVel(NMP::Vector3::InitZero);
    NMP::Vector3 trajDeltaAngVel(NMP::Vector3::InitZero);
    uint32_t trajNumSamples = predictionStateAttrib->m_trajNumSamples;
    float recipTrajNumSamples = 1.0f / trajNumSamples;
    uint32_t sampleIndx = predictionStateAttrib->getTrajectorySampleBegin();
    for (uint32_t i = 0; i < trajNumSamples; ++i)
    {
      float dt;
      NMP::Vector3 deltaPosVel;
      NMP::Vector3 deltaAngVel;
      predictionStateAttrib->getTrajectorySample(
        sampleIndx,
        dt,
        deltaPosVel,
        deltaAngVel);

      trajDeltaTime += dt;
      trajDeltaPosVel += deltaPosVel;
      trajDeltaAngVel += deltaAngVel;
      sampleIndx = predictionStateAttrib->getTrajectorySampleNext(sampleIndx);
    }
    trajDeltaTime *= recipTrajNumSamples;
    trajDeltaPosVel *= recipTrajNumSamples;
    trajDeltaAngVel *= recipTrajNumSamples;

    // Compute the smoothed trajectory delta
    NMP::Vector3 trajDeltaPos = trajDeltaPosVel * trajDeltaTime;
    NMP::Quat trajDeltaQuat;
    trajDeltaQuat.fastExp(trajDeltaAngVel * (0.5f * trajDeltaTime));

    // Initialise the trajectory predictor
    TrajectoryPredictor trajectoryPredictor;
    trajectoryPredictor.init(
      upAxisWS,
      trajDeltaTime,
      trajDeltaPos,
      trajDeltaQuat);

    //---------------------------
    // Compute the predicted footplant positions
    for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
    {
      AttribDataPredictiveUnevenTerrainPredictionState::LimbState* predictionLimbState = predictionStateAttrib->m_limbStates[limbIndex];
      const AttribDataBasicUnevenTerrainIKSetup::LimbState* curIKSetupLimbState = inputIKSetupAttrib->m_limbStates[limbIndex];
      NMP_ASSERT(predictionLimbState);
      NMP_ASSERT(curIKSetupLimbState);

      predictionLimbState->m_initNextWorldTrajectoryPosValid = false;
      predictionLimbState->m_initNextWorldFootbasePosValid = false;
      if (predictionLimbState->m_curLimbPhase != AttribDataPredictiveUnevenTerrainPredictionState::kInvalidPhase)
      {
        //---------------------------
        // Update the trajectory prediction state
        NMP::Vector3 trajPredictedDeltaPos;
        NMP::Quat trajPredictedDeltaQuat;
        NMP::Matrix34 trajPredictedDeltaTM;
        NMP::Matrix34 nextWorldRootTransform;
        NMP::Matrix34 midWorldRootTransform;
        float midTimeToNextSyncEvent;

        // Trajectory prediction at the start of the next gait cycle
        trajectoryPredictor.predict(
          predictionLimbState->m_timeToNextSyncEvent,
          trajPredictedDeltaPos,
          trajPredictedDeltaQuat);
        trajPredictedDeltaTM.initialise(trajPredictedDeltaQuat, trajPredictedDeltaPos);
        nextWorldRootTransform.multiply(trajPredictedDeltaTM, curWorldRootTransform);
        predictionLimbState->m_initNextWorldTrajectoryPos = nextWorldRootTransform.translation();

        // Trajectory prediction at the mid remaining time between the current and the next gait cycle
        midTimeToNextSyncEvent = midForwardPredictionFraction * predictionLimbState->m_timeToNextSyncEvent;
        trajectoryPredictor.predict(
          midTimeToNextSyncEvent,
          trajPredictedDeltaPos,
          trajPredictedDeltaQuat);
        trajPredictedDeltaTM.initialise(trajPredictedDeltaQuat, trajPredictedDeltaPos);
        midWorldRootTransform.multiply(trajPredictedDeltaTM, curWorldRootTransform);
        predictionLimbState->m_initMidWorldTrajectoryPos = midWorldRootTransform.translation();

        // Set the initial mid and next trajectory prediction positions as valid
        predictionLimbState->m_initNextWorldTrajectoryPosValid = true;

        //---------------------------
        // Update the foot cycle prediction state

        // Compute the position of the footbase pivot point in the frame of the character root
        NMP::Vector3 initFootbasePivotPosCS;
        curWorldRootTransform.inverseTransformVector(
          curIKSetupLimbState->m_initWorldFootbasePos,
          initFootbasePivotPosCS);

        // Update the foot cycle curve fitting state and append the sample to the history
        predictionLimbState->m_footCyclePredictor.updateFootCycle(
          setupAttrib->m_upAxis,
          predictionLimbState->m_hasFootCycleLooped,
          predictionLimbState->m_timeFractionOfMidInFootCycle,
          predictionLimbState->m_timeFractionInFootCycle,
          initFootbasePivotPosCS);

        FootCyclePredictor::CurveState curveState = predictionLimbState->m_footCyclePredictor.getFootCycleCurveState();
        if (curveState != FootCyclePredictor::kInvalidCurveState)
        {
          // Predict the foot cycle position at the start of the next gait cycle
          NMP::Vector3 predictedFootbasePivotPosCS;
          predictionLimbState->m_footCyclePredictor.evaluateFootCycleCurve(
            predictionLimbState->m_timeFractionOfMidInFootCycle,
            0.0f,
            predictedFootbasePivotPosCS);

          // Compute the predicted foot cycle position at the start of the next gait cycle
          nextWorldRootTransform.transformVector(
            predictedFootbasePivotPosCS,
            predictionLimbState->m_initNextWorldFootbasePos);

          // Compute the predicted foot position at the mid remaining time between the current and the next
          // gait cycle. This mid pose is used to help with foot lifting and deciding if the predicted lifting
          // motion is valid. It need not be an accurate representation of the character's pose at the mid
          // remaining time. We use the foot cycle pose for the next predicted foot plant and the estimated
          // trajectory component to the mid remaining time. This has the following properties:
          //
          //     - When transitioning into a new foot cycle the mid pose is positioned mid way towards the
          //       next foot plant position. The separation is maximal between current, mid and next poses.
          //
          //     - Over the foot cycle the mid pose moves closer towards the next foot plant position, maintaining
          //       the fractional separation between current, mid and next poses. This allows the juxtaposition of
          //       these three poses to be useful for foot lifting and for quickly determining potential problems
          //       with lifting over the terrain to the next foot plant.
          midWorldRootTransform.transformVector(
            predictedFootbasePivotPosCS,
            predictionLimbState->m_initMidWorldFootbasePos);

          // Set the initial mid and next predicted footplant positions as valid
          predictionLimbState->m_initNextWorldFootbasePosValid = true;
        }
      }
      else
      {
        // Invalidate the foot cycle prediction state
        predictionLimbState->m_footCyclePredictor.init();
      }
    }
  }
  else
  {
    // Invalidate the foot cycle prediction
    predictionStateAttrib->resetTrajectoryPredictionHistory();
    for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
    {
      AttribDataPredictiveUnevenTerrainPredictionState::LimbState* outputPredictionLimbState = predictionStateAttrib->m_limbStates[limbIndex];
      NMP_ASSERT(outputPredictionLimbState);
      outputPredictionLimbState->resetFootCyclePrediction();
    }
  }

  NM_UT_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void footLiftingTargetInContactPhase(
  const AttribDataPredictiveUnevenTerrainPredictionState* inputPredictionStateAttrib,
  AttribDataBasicUnevenTerrainFootLiftingTarget* outputFootLiftingTargetAttrib)
{
  NMP_ASSERT(inputPredictionStateAttrib);
  NMP_ASSERT(outputFootLiftingTargetAttrib);

  uint32_t numLimbs = inputPredictionStateAttrib->m_numLimbs;
  for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
  {
    const AttribDataPredictiveUnevenTerrainPredictionState::LimbState* inputPredictionLimbState = inputPredictionStateAttrib->m_limbStates[limbIndex];
    AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData* outputFootLiftingTargetData = outputFootLiftingTargetAttrib->m_limbInfo[limbIndex];
    NMP_ASSERT(inputPredictionLimbState);
    NMP_ASSERT(outputFootLiftingTargetData);

    // Check if we are in a foot contact phase
    if (inputPredictionLimbState->m_curLimbPhase == AttribDataPredictiveUnevenTerrainPredictionState::kPrevContact ||
        inputPredictionLimbState->m_curLimbPhase == AttribDataPredictiveUnevenTerrainPredictionState::kNextContact)
    {
      outputFootLiftingTargetData->m_isFootInContactPhase = true;
    }
    else
    {
      outputFootLiftingTargetData->m_isFootInContactPhase = false;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Basis frame at the world character root with Y axis upward and Z axis in the direction of forward motion
void terrainFootLiftingBasisFrame(
  const NMP::Vector3& upAxisWS,
  const AttribDataBasicUnevenTerrainIKSetup* inputIKSetupAttrib,
  const AttribDataPredictiveUnevenTerrainPredictionState* inputPredictionStateAttrib,
  AttribDataPredictiveUnevenTerrainFootLiftingState* outputFootLiftingStateAttrib)
{
  uint32_t numLimbs = inputPredictionStateAttrib->m_numLimbs;

  // Compute the trajectory basis frame for the foot lifting coordinate system
  NMP::Vector3 meanTrajectoryV(NMP::Vector3::InitZero);
  for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
  {
    NMP::Vector3 deltaTrajectoryV;
    const AttribDataPredictiveUnevenTerrainPredictionState::LimbState* curPredictionLimbState = inputPredictionStateAttrib->m_limbStates[limbIndex];
    if (curPredictionLimbState->m_initNextWorldTrajectoryPosValid)
    {
      // Next
      deltaTrajectoryV = curPredictionLimbState->m_initNextWorldTrajectoryPos - inputIKSetupAttrib->m_characterRootWorldTM.translation();
      meanTrajectoryV += deltaTrajectoryV;
      // Mid
      deltaTrajectoryV = curPredictionLimbState->m_initMidWorldTrajectoryPos - inputIKSetupAttrib->m_characterRootWorldTM.translation();
      meanTrajectoryV += deltaTrajectoryV;
    }
  }

  outputFootLiftingStateAttrib->m_footLiftingBasisValid = false;
  NMP::Vector3& Nx = outputFootLiftingStateAttrib->m_footLiftingBasis.xAxis();
  NMP::Vector3& Ny = outputFootLiftingStateAttrib->m_footLiftingBasis.yAxis();
  NMP::Vector3& Nz = outputFootLiftingStateAttrib->m_footLiftingBasis.zAxis();
  NMP::Vector3& T = outputFootLiftingStateAttrib->m_footLiftingBasis.translation();
  T = inputIKSetupAttrib->m_characterRootWorldTM.translation();

  // X axis (perpendicular to the up axis and the mean footbase vector)
  Nx.cross(upAxisWS, meanTrajectoryV);
  float norm = Nx.magnitude();
  if (norm > 1e-4f)
  {
    float recipNorm = 1.0f / norm;
    Nx *= recipNorm;
    // Y axis (The up axis direction)
    Ny = upAxisWS;
    // Z axis encoding the forward motion direction (Remaining axis whereby
    // the YZ plane contains the mean footbase vector)
    Nz.cross(Nx, upAxisWS);

    outputFootLiftingStateAttrib->m_footLiftingBasisValid = true;
  }

#ifdef TEST_UT_TASK_LOGGING
  NET_LOG_MESSAGE(
    NMP::LOG_PRIORITY_ALWAYS,
    "Foot lifting basis: m_footLiftingBasisValid = %d\n",
    outputFootLiftingStateAttrib->m_footLiftingBasisValid);

  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "x = %f %f %f\n", Nx.x, Nx.y, Nx.z);
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "y = %f %f %f\n", Ny.x, Ny.y, Ny.z);
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "z = %f %f %f\n", Nz.x, Nz.y, Nz.z);
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "t = %f %f %f\n", T.x, T.y, T.z);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
// Invalidates terrain hit information based on the foot cycle prediction information
void terrainFootLiftingStateInitialise(
  const NMP::Vector3& upAxisWS,
  const AttribDataBasicUnevenTerrainChain* chainAttrib,
  const AttribDataPredictiveUnevenTerrainPredictionDef* predictionDefAttrib,
  const AttribDataBasicUnevenTerrainIKSetup* inputIKSetupAttrib,
  const AttribDataPredictiveUnevenTerrainPredictionState* inputPredictionStateAttrib,
  AttribDataPredictiveUnevenTerrainFootLiftingState* outputFootLiftingStateAttrib,
  bool predictionEnable)
{
  NMP_ASSERT(chainAttrib);
  NMP_ASSERT(predictionDefAttrib);
  NMP_ASSERT(inputIKSetupAttrib);
  NMP_ASSERT(inputPredictionStateAttrib);
  NMP_ASSERT(outputFootLiftingStateAttrib);

  // Distance tolerances for proximity of the current foot position to the predicted foot plant
  const float proximityTol = predictionDefAttrib->m_closeFootbaseTolFrac * inputIKSetupAttrib->m_straightestLegLength;
  const float proximityTolSquared = proximityTol * proximityTol;

  //---------------------------
  // Check an absolute time has been set
  if (inputPredictionStateAttrib->m_isAbs ||
      !inputIKSetupAttrib->m_enableIK ||
      !predictionEnable)
  {
#ifdef TEST_UT_TASK_LOGGING
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "Character is not on ground or there is no prediction for this frame: resetting the foot lifting state\n");
#endif

    outputFootLiftingStateAttrib->reset();
    return;
  }

  //---------------------------
  // Initialise the foot lifting enable flags
  uint32_t numLimbs = inputPredictionStateAttrib->m_numLimbs;
  for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
  {
    const AttribDataPredictiveUnevenTerrainPredictionState::LimbState* inputPredictionLimbState = inputPredictionStateAttrib->m_limbStates[limbIndex];
    AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData* outputFootLiftingData = outputFootLiftingStateAttrib->m_limbInfo[limbIndex];
    NMP_ASSERT(inputPredictionLimbState);
    NMP_ASSERT(outputFootLiftingData);

    // Check for an invalid prediction state
    if (inputPredictionLimbState->m_curLimbPhase != AttribDataPredictiveUnevenTerrainPredictionState::kInvalidPhase)
    {
      // When transitioning into a new foot cycle invalidate all the predicted foot lifting state information.
      // Note that the current lifting information and alternation state remains unchanged
      if (inputPredictionLimbState->m_hasFootCycleLooped)
      {
#ifdef TEST_UT_TASK_LOGGING
        NET_LOG_MESSAGE(
          NMP::LOG_PRIORITY_ALWAYS,
          "limbIndex = %d: hasFootCycleLooped = true: invalidating the raycast hit information\n",
          limbIndex);
#endif
        outputFootLiftingData->m_midWorldFootbaseHitPosValid = false;
        outputFootLiftingData->m_nextWorldFootbaseHitPosValid = false;
      }

      // Initialise the foot lifting enable flags
      outputFootLiftingData->m_midWorldHitDataEnabledForLifting = outputFootLiftingData->m_midWorldFootbaseHitPosValid;
      outputFootLiftingData->m_nextWorldHitDataEnabledForLifting = outputFootLiftingData->m_nextWorldFootbaseHitPosValid;

      // Check if we are in a foot contact phase
      if (inputPredictionLimbState->m_curLimbPhase == AttribDataPredictiveUnevenTerrainPredictionState::kPrevContact ||
          inputPredictionLimbState->m_curLimbPhase == AttribDataPredictiveUnevenTerrainPredictionState::kNextContact)
      {
#ifdef TEST_UT_TASK_LOGGING
        NET_LOG_MESSAGE(
          NMP::LOG_PRIORITY_ALWAYS,
          "limbIndex = %d: In contact phase: Forcing raycast scheduler to update the current hit point\n",
          limbIndex);
#endif
        // Force the ray-cast scheduler to update the current hit point
        outputFootLiftingData->m_midWorldHitDataEnabledForLifting = false;
        outputFootLiftingData->m_nextWorldHitDataEnabledForLifting = false;
        outputFootLiftingData->m_alternateState = AttribDataPredictiveUnevenTerrainFootLiftingState::kInvalidAlternationState;
        outputFootLiftingData->m_alternatePrediction = AttribDataPredictiveUnevenTerrainFootLiftingState::kInvalidPredictionState;
      }
    }
    else
    {
#ifdef TEST_UT_TASK_LOGGING
      NET_LOG_MESSAGE(
        NMP::LOG_PRIORITY_ALWAYS,
        "limbIndex = %d: m_curLimbPhase = kInvalidPhase: resetting the foot lifting state\n",
        limbIndex);
#endif
      // Invalid prediction state
      outputFootLiftingData->reset();
    }
  }

  //---------------------------
  // Lifting enable flags for the mid state
  for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
  {
    const AttribDataBasicUnevenTerrainIKSetup::LimbState* curIKSetupLimbState = inputIKSetupAttrib->m_limbStates[limbIndex];
    const AttribDataPredictiveUnevenTerrainPredictionState::LimbState* inputPredictionLimbState = inputPredictionStateAttrib->m_limbStates[limbIndex];
    AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData* outputFootLiftingData = outputFootLiftingStateAttrib->m_limbInfo[limbIndex];
    NMP_ASSERT(curIKSetupLimbState);
    NMP_ASSERT(inputPredictionLimbState);
    NMP_ASSERT(outputFootLiftingData);

    // Check if the mid lifting state is already disabled
    if (!outputFootLiftingData->m_midWorldHitDataEnabledForLifting)
      continue;

    //---------------------------
    // Check the proximity of the current foot position to the predicted foot plant
    NMP::Vector3 initToMidFootbasePosV = inputPredictionLimbState->m_initMidWorldFootbasePos - curIKSetupLimbState->m_initWorldFootbasePos;
    // Component in vertical direction
    float lambda = initToMidFootbasePosV.dot(upAxisWS);
    initToMidFootbasePosV -= (upAxisWS * lambda);
    float mag2 = initToMidFootbasePosV.magnitudeSquared();
    if (mag2 < proximityTolSquared)
    {
#ifdef TEST_UT_TASK_LOGGING
      NET_LOG_MESSAGE(
        NMP::LOG_PRIORITY_ALWAYS,
        "limbIndex = %d: Mid state. current foot position is in proximity to the mid prediction %f\n",
        limbIndex,
        sqrtf(mag2));
#endif
      outputFootLiftingData->m_midWorldHitDataEnabledForLifting = false;
      continue;
    }

    //---------------------------
    // Check the lateral support region (changes in trajectory)

    // Compute the root to mid footbase vector in the foot lifting basis frame
    NMP::Vector3 midFootbaseV;
    outputFootLiftingStateAttrib->m_footLiftingBasis.inverseTransformVector(
      inputPredictionLimbState->m_initNextWorldFootbasePos,
      midFootbaseV);

    // Check if the next footbase position is within the lateral support region
    float x = fabs(midFootbaseV.x);
    float z = fabs(midFootbaseV.z);
    float lateralSupportDist = inputIKSetupAttrib->m_straightestLegLength * chainAttrib->m_footLiftingHeightLimit;
    float lateralSlopeDist = z * predictionDefAttrib->m_footLiftingLateralAngleLimit;

    if (x > lateralSupportDist && x > lateralSlopeDist)
    {
#ifdef TEST_UT_TASK_LOGGING
      NET_LOG_MESSAGE(
        NMP::LOG_PRIORITY_ALWAYS,
        "limbIndex = %d: Mid state. mid footbase hit pos is outside the lateral support region\n",
        limbIndex);
#endif
      outputFootLiftingData->m_midWorldHitDataEnabledForLifting = false;
      continue;
    }
  }

  //---------------------------
  // Lifting enable flags for the next state
  for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
  {
    const AttribDataBasicUnevenTerrainIKSetup::LimbState* curIKSetupLimbState = inputIKSetupAttrib->m_limbStates[limbIndex];
    const AttribDataPredictiveUnevenTerrainPredictionState::LimbState* inputPredictionLimbState = inputPredictionStateAttrib->m_limbStates[limbIndex];
    AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData* outputFootLiftingData = outputFootLiftingStateAttrib->m_limbInfo[limbIndex];
    NMP_ASSERT(curIKSetupLimbState);
    NMP_ASSERT(inputPredictionLimbState);
    NMP_ASSERT(outputFootLiftingData);

    // Check if the next lifting state is already disabled
    if (!outputFootLiftingData->m_nextWorldHitDataEnabledForLifting)
      continue;

    //---------------------------
    // Check the proximity of the current foot position to the predicted foot plant
    NMP::Vector3 initToNextFootbasePosV = inputPredictionLimbState->m_initNextWorldFootbasePos - curIKSetupLimbState->m_initWorldFootbasePos;
    // Component in vertical direction
    float lambda = initToNextFootbasePosV.dot(upAxisWS);
    initToNextFootbasePosV -= (upAxisWS * lambda);
    float mag2 = initToNextFootbasePosV.magnitudeSquared();
    if (mag2 < proximityTolSquared)
    {
#ifdef TEST_UT_TASK_LOGGING
      NET_LOG_MESSAGE(
        NMP::LOG_PRIORITY_ALWAYS,
        "limbIndex = %d: Next state. current foot position is in proximity to the next prediction %f\n",
        limbIndex,
        sqrtf(mag2));
#endif
      outputFootLiftingData->m_nextWorldHitDataEnabledForLifting = false;
      continue;
    }

    //---------------------------
    // Check the lateral support region (changes in trajectory)

    // Compute the root to next footbase vector in the foot lifting basis frame
    NMP::Vector3 nextFootbaseV;
    outputFootLiftingStateAttrib->m_footLiftingBasis.inverseTransformVector(
      inputPredictionLimbState->m_initNextWorldFootbasePos,
      nextFootbaseV);

    // Check if the next footbase position is within the lateral support region
    float x = fabs(nextFootbaseV.x);
    float z = fabs(nextFootbaseV.z);
    float lateralSupportDist = inputIKSetupAttrib->m_straightestLegLength * chainAttrib->m_footLiftingHeightLimit;
    float lateralSlopeDist = z * predictionDefAttrib->m_footLiftingLateralAngleLimit;

    if (x > lateralSupportDist && x > lateralSlopeDist)
    {
#ifdef TEST_UT_TASK_LOGGING
      NET_LOG_MESSAGE(
        NMP::LOG_PRIORITY_ALWAYS,
        "limbIndex = %d: Next state. next footbase hit pos is outside the lateral support region\n",
        limbIndex);
#endif
      outputFootLiftingData->m_nextWorldHitDataEnabledForLifting = false;
      continue;
    }
  }
}

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void terrainRayCastAlternateForwardPrediction(
  uint32_t limbIndex,
  const NMP::Vector3& upAxisWS,
  Network* net,
  const CharacterControllerInterface* cc,
  const AttribDataBasicUnevenTerrainChain* chainAttrib,
  const AttribDataPredictiveUnevenTerrainPredictionDef* predictionDefAttrib,
  const AttribDataBasicUnevenTerrainIKSetup* inputIKSetupAttrib,
  const AttribDataPredictiveUnevenTerrainPredictionState* inputPredictionStateAttrib,
  AttribDataPredictiveUnevenTerrainFootLiftingState* outputFootLiftingStateAttrib)
{
  const AttribDataPredictiveUnevenTerrainPredictionState::LimbState* inputPredictionLimbState = inputPredictionStateAttrib->m_limbStates[limbIndex];
  AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData* outputFootLiftingData = outputFootLiftingStateAttrib->m_limbInfo[limbIndex];
  NMP_ASSERT(inputPredictionLimbState);
  NMP_ASSERT(outputFootLiftingData);
  const NMP::Matrix34& ccTM = net->getCharacterPropertiesWorldRootTransform();

  // Ensure that the predicted mid, next footbase positions are valid
  NMP_ASSERT(inputPredictionLimbState->m_initNextWorldFootbasePosValid);

  // Foot lifting limits at the current character root position
  float footLiftingUpDist = NMP::maximum(inputIKSetupAttrib->m_rootToHipsHeight * chainAttrib->m_footLiftingHeightLimit, 0.0f);
  float footLiftingDownDist = inputIKSetupAttrib->m_straightestLegLength * chainAttrib->m_footLiftingHeightLimit;

  //---------------------------
  // Check for a transition to the next prediction state
  if (outputFootLiftingData->m_alternatePrediction != AttribDataPredictiveUnevenTerrainFootLiftingState::kNextPos)
  {
    // Vector between the character root and the next predicted foot plant position
    NMP::Vector3 rootToNextFootbaseV = inputPredictionLimbState->m_initNextWorldFootbasePos - ccTM.translation();
    // Vertical component of the vector
    float lambda = rootToNextFootbaseV.dot(upAxisWS);
    // Horizontal component of the vector
    rootToNextFootbaseV -= (upAxisWS * lambda);
    // Horizontal distance
    float horizontalDist = rootToNextFootbaseV.magnitude();
    // Vertical distance corresponding to the maximum slope limit at the next foot plant position
    float verticalSlopeDist = horizontalDist * predictionDefAttrib->m_footLiftingSlopeAngleLimit;

    // Foot lifting limits at the next foot plant position. Note that we do not use the smoothed
    // vertical component of the trajectory to determine the lifting limits since movement over
    // step edges can cause inaccurate foot lifting estimates.
    float footLiftingUpDistAtNext = NMP::maximum(verticalSlopeDist, footLiftingUpDist);
    float footLiftingDownDistAtNext = NMP::maximum(verticalSlopeDist, footLiftingDownDist);

    // Compute the ray start position
    NMP::Vector3 rayStart = ccTM.translation() + rootToNextFootbaseV + (upAxisWS * footLiftingUpDistAtNext);

    // Compute the ray cast direction
    float footLiftingDeltaDistAtNext = footLiftingUpDistAtNext + footLiftingDownDistAtNext;
    NMP::Vector3 rayDelta = upAxisWS * -footLiftingDeltaDistAtNext;

    // Set the defaults
    outputFootLiftingData->m_nextWorldFootbaseHitPosValid = false;
    outputFootLiftingData->m_nextWorldFootbaseHitNormalSlopeValid = false;

    // Compute the ray-cast information
    NMP::Vector3 raycastHitPos;
    NMP::Vector3 raycastHitNormal;
    NMP::Vector3 raycastHitVelocity;
    float raycastHitDistance;

    if (cc->castRayIntoCollisionWorld(
          rayStart,
          rayDelta,
          raycastHitDistance,
          raycastHitPos,
          raycastHitNormal,
          raycastHitVelocity,
          net))
    {
      // Set the terrain hit information
      outputFootLiftingData->m_nextWorldFootbaseHitPos = raycastHitPos;
      outputFootLiftingData->m_nextWorldFootbaseHitNormal = raycastHitNormal;
      outputFootLiftingData->m_nextWorldFootbaseHitPosValid = true;

      // Check if the terrain surface slope gradient is too steep
      float cosTheta = raycastHitNormal.dot(upAxisWS);
      if (cosTheta > chainAttrib->m_footAlignToSurfaceAngleLimit)
      {
        outputFootLiftingData->m_nextWorldFootbaseHitNormalSlopeValid = true;
      }
    }

#ifdef TEST_UT_TASK_LOGGING
    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "alternateState = kPredictedPos (kNextPos), nextWorldFootbaseHitPosValid = %d\n",
      outputFootLiftingData->m_nextWorldFootbaseHitPosValid);
    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "nextWorldFootbaseHitPos = %f, %f, %f\n",
      outputFootLiftingData->m_nextWorldFootbaseHitPos.x,
      outputFootLiftingData->m_nextWorldFootbaseHitPos.y,
      outputFootLiftingData->m_nextWorldFootbaseHitPos.z);
    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "nextWorldFootbaseHitNormal = %f, %f, %f\n",
      outputFootLiftingData->m_nextWorldFootbaseHitNormal.x,
      outputFootLiftingData->m_nextWorldFootbaseHitNormal.y,
      outputFootLiftingData->m_nextWorldFootbaseHitNormal.z);
    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "nextWorldFootbaseHitNormalSlopeValid = %d\n",
      outputFootLiftingData->m_nextWorldFootbaseHitNormalSlopeValid);
#endif

#ifdef TEST_UT_TASK_DEBUG_DRAW
    MR_DEBUG_DRAW_LINE_GLOBAL(
      outputFootLiftingData->m_nextWorldFootbaseHitPos, 
      outputFootLiftingData->m_nextWorldFootbaseHitPos+outputFootLiftingData->m_nextWorldFootbaseHitNormal, 
      NMP::Colour::WHITE);
#endif

    outputFootLiftingData->m_alternatePrediction = AttribDataPredictiveUnevenTerrainFootLiftingState::kNextPos;
  }
  else
  {
    //---------------------------
    // Vector between the character root and the mid predicted foot plant position
    NMP::Vector3 rootToMidFootbaseV = inputPredictionLimbState->m_initMidWorldFootbasePos - ccTM.translation();
    // Vertical component of the vector
    float lambda = rootToMidFootbaseV.dot(upAxisWS);
    // Horizontal component of the vector
    rootToMidFootbaseV -= (upAxisWS * lambda);
    // Horizontal distance
    float horizontalDist = rootToMidFootbaseV.magnitude();
    // Vertical distance corresponding to the maximum slope limit at the mid foot plant position
    float verticalSlopeDist = horizontalDist * predictionDefAttrib->m_footLiftingSlopeAngleLimit;

    // Foot lifting limits at the mid foot plant position. Note that we do not use the smoothed
    // vertical component of the trajectory to determine the lifting limits since movement over
    // step edges can cause inaccurate foot lifting estimates.
    float footLiftingUpDistAtMid = NMP::maximum(verticalSlopeDist, footLiftingUpDist);
    float footLiftingDownDistAtMid = NMP::maximum(verticalSlopeDist, footLiftingDownDist);

    // Compute the ray start position
    NMP::Vector3 rayStart = ccTM.translation() + rootToMidFootbaseV + (upAxisWS * footLiftingUpDistAtMid);

    // Compute the ray cast direction
    float footLiftingDeltaDistAtMid = footLiftingUpDistAtMid + footLiftingDownDistAtMid;
    NMP::Vector3 rayDelta = upAxisWS * -footLiftingDeltaDistAtMid;

    // Set the defaults
    outputFootLiftingData->m_midWorldFootbaseHitPosValid = false;
    outputFootLiftingData->m_midWorldFootbaseHitNormalSlopeValid = false;

    // Compute the ray-cast information
    NMP::Vector3 raycastHitPos;
    NMP::Vector3 raycastHitNormal;
    NMP::Vector3 raycastHitVelocity;
    float raycastHitDistance;

    if (cc->castRayIntoCollisionWorld(
          rayStart,
          rayDelta,
          raycastHitDistance,
          raycastHitPos,
          raycastHitNormal,
          raycastHitVelocity,
          net))
    {
      // Set the terrain hit information
      outputFootLiftingData->m_midWorldFootbaseHitPos = raycastHitPos;
      outputFootLiftingData->m_midWorldFootbaseHitNormal = raycastHitNormal;
      outputFootLiftingData->m_midWorldFootbaseHitPosValid = true;

      // Check if the terrain surface slope gradient is too steep
      float cosTheta = raycastHitNormal.dot(upAxisWS);
      if (cosTheta > chainAttrib->m_footAlignToSurfaceAngleLimit)
      {
        outputFootLiftingData->m_midWorldFootbaseHitNormalSlopeValid = true;
      }
    }

#ifdef TEST_UT_TASK_LOGGING
    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "alternateState = kPredictedPos (kMidPos), midWorldFootbaseHitPosValid = %d\n",
      outputFootLiftingData->m_midWorldFootbaseHitPosValid);
    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "midWorldFootbaseHitPos = %f, %f, %f\n",
      outputFootLiftingData->m_midWorldFootbaseHitPos.x,
      outputFootLiftingData->m_midWorldFootbaseHitPos.y,
      outputFootLiftingData->m_midWorldFootbaseHitPos.z);
    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "midWorldFootbaseHitNormal = %f, %f, %f\n",
      outputFootLiftingData->m_midWorldFootbaseHitNormal.x,
      outputFootLiftingData->m_midWorldFootbaseHitNormal.y,
      outputFootLiftingData->m_midWorldFootbaseHitNormal.z);
    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "midWorldFootbaseHitNormalSlopeValid = %d\n",
      outputFootLiftingData->m_midWorldFootbaseHitNormalSlopeValid);
#endif

#ifdef TEST_UT_TASK_DEBUG_DRAW
    MR_DEBUG_DRAW_LINE_GLOBAL(
      outputFootLiftingData->m_nextWorldFootbaseHitPos, 
      outputFootLiftingData->m_nextWorldFootbaseHitPos+outputFootLiftingData->m_nextWorldFootbaseHitNormal, 
      NMP::Colour::BLUE);
#endif

    outputFootLiftingData->m_alternatePrediction = AttribDataPredictiveUnevenTerrainFootLiftingState::kMidPos;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Alternates ray-casting between current and predicted foot positions
void terrainFootLiftingStateUpdateRayCast(
  const NMP::Vector3& upAxisWS,
  Network* net,
  const CharacterControllerInterface* cc,
  const AttribDataBasicUnevenTerrainChain* chainAttrib,
  const AttribDataPredictiveUnevenTerrainPredictionDef* predictionDefAttrib,
  const AttribDataBasicUnevenTerrainIKSetup* inputIKSetupAttrib,
  const AttribDataPredictiveUnevenTerrainPredictionState* inputPredictionStateAttrib,
  AttribDataPredictiveUnevenTerrainFootLiftingState* outputFootLiftingStateAttrib)
{
  NM_UT_BEGIN_PROFILING("PREDICTIVE_UT_FOOT_LIFTING_STATE_RAY_CAST");

#ifdef TEST_UT_TASK_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\n----------------------------------------------------\n");
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "terrainFootLiftingStateUpdateRayCast()\n");
#endif

  uint32_t numLimbs = inputPredictionStateAttrib->m_numLimbs;
  for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
  {
    const AttribDataPredictiveUnevenTerrainPredictionState::LimbState* inputPredictionLimbState = inputPredictionStateAttrib->m_limbStates[limbIndex];
    AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData* outputFootLiftingData = outputFootLiftingStateAttrib->m_limbInfo[limbIndex];
    NMP_ASSERT(inputPredictionLimbState);
    NMP_ASSERT(outputFootLiftingData);

#ifdef TEST_UT_TASK_LOGGING
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\nlimbIndex = %d\n", limbIndex);
#endif

    //---------------------------
    // Check for a transition to the current alternation state
    if (!outputFootLiftingData->m_curWorldFootbaseHitPosValid || // Current terrain hit position is invalid
        !inputPredictionLimbState->m_initNextWorldFootbasePosValid || // Next foot plant pos prediction is invalid
        outputFootLiftingData->m_alternateState != AttribDataPredictiveUnevenTerrainFootLiftingState::kCurrentPos) // update current state every other frame
    {
      // Ray cast from the current footbase position
      const AttribDataBasicUnevenTerrainIKSetup::LimbState* curIKSetupLimbState = inputIKSetupAttrib->m_limbStates[limbIndex];
      NMP_ASSERT(curIKSetupLimbState);

      // Lifting limits
      float footLiftingUpDist = NMP::maximum(inputIKSetupAttrib->m_rootToHipsHeight * chainAttrib->m_footLiftingHeightLimit, 0.0f);
      float footLiftingDownDist = inputIKSetupAttrib->m_straightestLegLength * chainAttrib->m_footLiftingHeightLimit;

      // Compute the ray start position
      NMP::Vector3 rayStart = curIKSetupLimbState->m_initWorldFootbasePos + (upAxisWS * footLiftingUpDist);

      // Compute the ray cast direction
      float footLiftingDeltaDist = footLiftingUpDist + footLiftingDownDist;
      NMP::Vector3 rayDelta = upAxisWS * -footLiftingDeltaDist;

      // Set the defaults
      outputFootLiftingData->m_curWorldFootbaseHitPosValid = false;
      outputFootLiftingData->m_curWorldFootbaseHitNormalSlopeValid = false;

      // Compute the ray-cast information
      NMP::Vector3 raycastHitPos;
      NMP::Vector3 raycastHitNormal;
      NMP::Vector3 raycastHitVelocity;
      float raycastHitDistance;

      if (cc->castRayIntoCollisionWorld(
            rayStart,
            rayDelta,
            raycastHitDistance,
            raycastHitPos,
            raycastHitNormal,
            raycastHitVelocity,
            net))
      {
        // Set the terrain hit information
        outputFootLiftingData->m_curWorldFootbaseHitPos = raycastHitPos;
        outputFootLiftingData->m_curWorldFootbaseHitNormal = raycastHitNormal;
        outputFootLiftingData->m_curWorldFootbaseHitPosValid = true;

        // Check if the terrain surface slope gradient is too steep
        float cosTheta = raycastHitNormal.dot(upAxisWS);
        if (cosTheta > chainAttrib->m_footAlignToSurfaceAngleLimit)
        {
          outputFootLiftingData->m_curWorldFootbaseHitNormalSlopeValid = true;
        }
      }

#ifdef TEST_UT_TASK_LOGGING
      NET_LOG_MESSAGE(
        NMP::LOG_PRIORITY_ALWAYS,
        "alternateState = kCurrentPos, curWorldFootbaseHitPosValid = %d\n",
        outputFootLiftingData->m_curWorldFootbaseHitPosValid);
      NET_LOG_MESSAGE(
        NMP::LOG_PRIORITY_ALWAYS,
        "curWorldFootbaseHitPos = %f, %f, %f\n",
        outputFootLiftingData->m_curWorldFootbaseHitPos.x,
        outputFootLiftingData->m_curWorldFootbaseHitPos.y,
        outputFootLiftingData->m_curWorldFootbaseHitPos.z);
      NET_LOG_MESSAGE(
        NMP::LOG_PRIORITY_ALWAYS,
        "curWorldFootbaseHitNormal = %f, %f, %f\n",
        outputFootLiftingData->m_curWorldFootbaseHitNormal.x,
        outputFootLiftingData->m_curWorldFootbaseHitNormal.y,
        outputFootLiftingData->m_curWorldFootbaseHitNormal.z);
      NET_LOG_MESSAGE(
        NMP::LOG_PRIORITY_ALWAYS,
        "curWorldFootbaseHitNormalSlopeValid = %d\n",
        outputFootLiftingData->m_curWorldFootbaseHitNormalSlopeValid);
#endif

#ifdef TEST_UT_TASK_DEBUG_DRAW
      MR_DEBUG_DRAW_LINE_GLOBAL(
        outputFootLiftingData->m_nextWorldFootbaseHitPos, 
        outputFootLiftingData->m_nextWorldFootbaseHitPos+outputFootLiftingData->m_nextWorldFootbaseHitNormal, 
        NMP::Colour::RED);
#endif

      outputFootLiftingData->m_alternateState = AttribDataPredictiveUnevenTerrainFootLiftingState::kCurrentPos;
    }
    else
    {
      // Update the forward prediction state
      terrainRayCastAlternateForwardPrediction(
        limbIndex,
        upAxisWS,
        net,
        cc,
        chainAttrib,
        predictionDefAttrib,
        inputIKSetupAttrib,
        inputPredictionStateAttrib,
        outputFootLiftingStateAttrib);

      outputFootLiftingData->m_alternateState = AttribDataPredictiveUnevenTerrainFootLiftingState::kPredictedPos;
    }
  }

  NM_UT_END_PROFILING();
}
#endif // !NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// Validates the terrain hit information based on the lifting support region
void terrainFootLiftingStateValidate(
  const AttribDataPredictiveUnevenTerrainPredictionDef* predictionDefAttrib,
  const AttribDataPredictiveUnevenTerrainPredictionState* inputPredictionStateAttrib,
  AttribDataPredictiveUnevenTerrainFootLiftingState* outputFootLiftingStateAttrib)
{
  NMP_ASSERT(predictionDefAttrib);
  NMP_ASSERT(inputPredictionStateAttrib);
  NMP_ASSERT(outputFootLiftingStateAttrib);

  uint32_t numLimbs = inputPredictionStateAttrib->m_numLimbs;
  for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
  {
    AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData* outputFootLiftingData = outputFootLiftingStateAttrib->m_limbInfo[limbIndex];
    NMP_ASSERT(outputFootLiftingData);

    // Compute the root to mid terrain hit pos vector in the foot lifting basis frame
    NMP::Vector3 rootToMidHitPosV(NMP::Vector3::InitZero);
    if (outputFootLiftingData->m_midWorldFootbaseHitPosValid)
    {
      outputFootLiftingStateAttrib->m_footLiftingBasis.inverseTransformVector(
        outputFootLiftingData->m_midWorldFootbaseHitPos,
        rootToMidHitPosV);
    }

    //---------------------------
    // Mid
    if (outputFootLiftingData->m_midWorldHitDataEnabledForLifting)
    {
      // Check if the root to mid vector is within the lifting slope limit
      float tanTheta = fabs(rootToMidHitPosV.y) / rootToMidHitPosV.z;
      if (tanTheta > predictionDefAttrib->m_footLiftingSlopeAngleLimit)
      {
#ifdef TEST_UT_TASK_LOGGING
        NET_LOG_MESSAGE(
          NMP::LOG_PRIORITY_ALWAYS,
          "limbIndex = %d: Mid state. root to mid terrain hit pos is above lifting slope limit\n",
          limbIndex);
#endif

        // Disable both the mid and next hit positions
        outputFootLiftingData->m_midWorldHitDataEnabledForLifting = false;
        outputFootLiftingData->m_nextWorldHitDataEnabledForLifting = false;
      }
    }

    //---------------------------
    // Next
    if (outputFootLiftingData->m_nextWorldHitDataEnabledForLifting)
    {
      // Compute the root to next terrain hit pos vector in the foot lifting basis frame
      NMP::Vector3 rootToNextHitPosV;
      outputFootLiftingStateAttrib->m_footLiftingBasis.inverseTransformVector(
        outputFootLiftingData->m_nextWorldFootbaseHitPos,
        rootToNextHitPosV);

      // Check if the root to next vector is within the lifting slope limit
      float tanTheta = fabs(rootToNextHitPosV.y) / rootToNextHitPosV.z;
      if (tanTheta > predictionDefAttrib->m_footLiftingSlopeAngleLimit)
      {
#ifdef TEST_UT_TASK_LOGGING
        NET_LOG_MESSAGE(
          NMP::LOG_PRIORITY_ALWAYS,
          "limbIndex = %d: Next state. root to next terrain hit pos is above lifting slope limit\n",
          limbIndex);
#endif

        outputFootLiftingData->m_nextWorldHitDataEnabledForLifting = false;
      }
      else if (outputFootLiftingData->m_midWorldFootbaseHitPosValid)
      {
        // Compute the mid to next terrain hit pos vector in the foot lifting basis frame
        NMP::Vector3 midToNextHitPosV = rootToNextHitPosV - rootToMidHitPosV;

        // Check if the mid to next vector is within the lifting slope limit
        tanTheta = fabs(midToNextHitPosV.y) / midToNextHitPosV.z;
        if (tanTheta > predictionDefAttrib->m_footLiftingSlopeAngleLimit)
        {
#ifdef TEST_UT_TASK_LOGGING
          NET_LOG_MESSAGE(
            NMP::LOG_PRIORITY_ALWAYS,
            "limbIndex = %d: Next state. mid to next terrain hit pos is above lifting slope limit\n",
            limbIndex);
#endif

          outputFootLiftingData->m_nextWorldHitDataEnabledForLifting = false;
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void terrainFootLiftingStateComputeLiftingTarget(
  const NMP::Vector3& upAxisWS,
  const AttribDataBasicUnevenTerrainIKSetup* inputIKSetupAttrib,
  const AttribDataPredictiveUnevenTerrainFootLiftingState* outputFootLiftingStateAttrib,
  AttribDataBasicUnevenTerrainFootLiftingTarget* outputFootLiftingTargetAttrib,
  const AttribDataBasicUnevenTerrainIKState* inputIKStateAttrib, // May not exist
  const AttribDataPredictiveUnevenTerrainPredictionState* inputPredictionStateAttrib)
{
#ifdef TEST_UT_TASK_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\n----------------------------------------------------\n");
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "terrainFootLiftingStateComputeLiftingTarget()\n");
#endif

  bool isValidInputIKState = (inputIKStateAttrib && inputIKStateAttrib->m_isValidData);

  //---------------------------
  // Compute the lifting vector and project the current footbase position onto it
  uint32_t numLimbs = inputPredictionStateAttrib->m_numLimbs;
  for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
  {
    AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData* outputFootLiftingTargetData = outputFootLiftingTargetAttrib->m_limbInfo[limbIndex];
    const AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData* outputFootLiftingStateData = outputFootLiftingStateAttrib->m_limbInfo[limbIndex];    
    const AttribDataBasicUnevenTerrainIKSetup::LimbState* curIKSetupLimbState = inputIKSetupAttrib->m_limbStates[limbIndex];
    const AttribDataPredictiveUnevenTerrainPredictionState::LimbState* curPredictionLimbState = inputPredictionStateAttrib->m_limbStates[limbIndex];
    NMP_ASSERT(outputFootLiftingTargetData);
    NMP_ASSERT(outputFootLiftingStateData);
    NMP_ASSERT(curIKSetupLimbState);
    NMP_ASSERT(curPredictionLimbState);

    // Initialise the foot lifting target with the source animation pose
    outputFootLiftingTargetData->m_targetWorldFootbaseTerrainPos.setToZero();
    outputFootLiftingTargetData->m_targetWorldFootbaseLiftingPos = curIKSetupLimbState->m_initWorldFootbasePos;
    outputFootLiftingTargetData->m_targetWorldFootbaseLiftingNormal = upAxisWS;
    outputFootLiftingTargetData->m_targetWorldFootbaseLiftingValid = false;

    // Check if the current footbase terrain hit position is valid
    if (outputFootLiftingStateData->m_curWorldFootbaseHitPosValid)
    {
      NMP::Vector3 worldFootbaseHitPos;
      NMP::Vector3 worldFootbaseHitNormal;

      //---------------------------
      // Current state
      worldFootbaseHitPos = outputFootLiftingStateData->m_curWorldFootbaseHitPos;
      if (outputFootLiftingStateData->m_curWorldFootbaseHitNormalSlopeValid)
      {
        worldFootbaseHitNormal = outputFootLiftingStateData->m_curWorldFootbaseHitNormal;
      }
      else
      {
        worldFootbaseHitNormal = upAxisWS;
      }

      // Compute the approximated terrain hit point for the current footbase position based on the
      // surface patch approximation with the current terrain hit state. For steep terrain slopes
      // this approximation look quite poor if the actual surface hit normal is used. If the terrain
      // is too steep then fix the hit height and adjust the position in the ground plane. This may
      // lead to slight foot penetration with the surface, but it looks better than over approximating
      // the height and reduces jitter.
      if (!projectPointOntoPlane(
            upAxisWS,
            curIKSetupLimbState->m_initWorldFootbasePos,
            worldFootbaseHitPos,
            worldFootbaseHitNormal,
            outputFootLiftingTargetData->m_targetWorldFootbaseTerrainPos))
      {
        // Invalid current hit point approximation so use the default foot lifting target.
        continue;
      }

      // Computing a terrain data solve requires a valid previous foot lifting position
      if (outputFootLiftingStateAttrib->m_footLiftingBasisValid && isValidInputIKState)
      {
        footLiftingTargetPredictor footLiftingPredictor;
        footLiftingPredictor.init(outputFootLiftingStateAttrib->m_footLiftingBasis);

        footLiftingPredictor.appendTerrainHitInfo(
          outputFootLiftingTargetData->m_targetWorldFootbaseTerrainPos,
          outputFootLiftingStateData->m_curWorldFootbaseHitNormal, // original terrain normal
          outputFootLiftingStateData->m_curWorldFootbaseHitNormalSlopeValid);

        //---------------------------
        // Mid state
        if (outputFootLiftingStateData->m_midWorldHitDataEnabledForLifting)
        {
          worldFootbaseHitPos = outputFootLiftingStateData->m_midWorldFootbaseHitPos;
          if (outputFootLiftingStateData->m_midWorldFootbaseHitNormalSlopeValid)
          {
            worldFootbaseHitNormal = outputFootLiftingStateData->m_midWorldFootbaseHitNormal;
          }
          else
          {
            worldFootbaseHitNormal = upAxisWS;
          }

          // Compute the approximated terrain hit point for the mid footbase position based on the
          // surface patch approximation with the mid terrain hit state. Use the ground plane normal
          // of the terrain slope is too high.
          NMP::Vector3 midWorldFootbaseLiftingPos;
          if (projectPointOntoPlane(
                upAxisWS,
                curPredictionLimbState->m_initMidWorldFootbasePos,
                worldFootbaseHitPos,
                worldFootbaseHitNormal,
                midWorldFootbaseLiftingPos))
          {
            footLiftingPredictor.appendTerrainHitInfo(
              midWorldFootbaseLiftingPos,
              outputFootLiftingStateData->m_midWorldFootbaseHitNormal, // original terrain normal
              outputFootLiftingStateData->m_midWorldFootbaseHitNormalSlopeValid);
          }
        }

        //---------------------------
        // Next state
        if (outputFootLiftingStateData->m_nextWorldHitDataEnabledForLifting)
        {
          worldFootbaseHitPos = outputFootLiftingStateData->m_nextWorldFootbaseHitPos;
          if (outputFootLiftingStateData->m_nextWorldFootbaseHitNormalSlopeValid)
          {
            worldFootbaseHitNormal = outputFootLiftingStateData->m_nextWorldFootbaseHitNormal;
          }
          else
          {
            worldFootbaseHitNormal = upAxisWS;
          }

          // Compute the approximated terrain hit point for the next footbase position based on the
          // surface patch approximation with the next terrain hit state. Use the ground plane normal
          // of the terrain slope is too high.
          NMP::Vector3 nextWorldFootbaseLiftingPos;
          if (projectPointOntoPlane(
                upAxisWS,
                curPredictionLimbState->m_initNextWorldFootbasePos,
                worldFootbaseHitPos,
                worldFootbaseHitNormal,
                nextWorldFootbaseLiftingPos))
          {
            footLiftingPredictor.appendTerrainHitInfo(
              nextWorldFootbaseLiftingPos,
              outputFootLiftingStateData->m_nextWorldFootbaseHitNormal, // original terrain normal
              outputFootLiftingStateData->m_nextWorldFootbaseHitNormalSlopeValid);
          }
        }

        //---------------------------
        // Compute the foot lifting target by using the supplied terrain ray cast hit information
        // to determine a suitable lifting curve
        const AttribDataBasicUnevenTerrainIKState::LimbState* prevIKStateLimbState = inputIKStateAttrib->m_limbStates[limbIndex];
        NMP_ASSERT(prevIKStateLimbState);
        outputFootLiftingTargetData->m_targetWorldFootbaseLiftingValid = footLiftingPredictor.terrainDataSolve(
          prevIKStateLimbState->m_drivenWorldFootbaseLiftingPos,            // IN: The previous frame's driven footbase lifting position
          curIKSetupLimbState->m_initWorldFootbasePos,                      // IN: This frames initial animation footbase position
          outputFootLiftingTargetData->m_targetWorldFootbaseLiftingPos,     // OUT: The target footbase lifting position
          outputFootLiftingTargetData->m_targetWorldFootbaseLiftingNormal); // OUT: The target footbase lifting normal
      }

      //---------------------------
      // If we were unable to compute the foot lifting target based on the cur, mid and next
      // terrain hit information then set the lifting target from the surface patch approximation
      if (!outputFootLiftingTargetData->m_targetWorldFootbaseLiftingValid)
      {
        outputFootLiftingTargetData->m_targetWorldFootbaseLiftingPos = outputFootLiftingTargetData->m_targetWorldFootbaseTerrainPos;
        outputFootLiftingTargetData->m_targetWorldFootbaseLiftingNormal = outputFootLiftingStateData->m_curWorldFootbaseHitNormal; // original terrain normal
        outputFootLiftingTargetData->m_targetWorldFootbaseLiftingValid = true;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
/// \brief This task initialises the uneven terrain IK setup with the current pose of the character,
/// and updates the trajectory prediction information.
//----------------------------------------------------------------------------------------------------------------------
void TaskPredictiveUnevenTerrainIKSetup(Dispatcher::TaskParameters* parameters)
{
  NM_UT_BEGIN_PROFILING("PREDICTIVE_UT_IK_SETUP_TASK");

  AttribDataPredictiveUnevenTerrainPredictionState* predictionStateAttrib;
  AttribDataBasicUnevenTerrainIKSetup* ikSetupAttrib;
  const AttribDataBasicUnevenTerrainSetup* setupDefAttrib;
  const AttribDataCharacterProperties* characterControllerAttrib;
  const AttribDataBasicUnevenTerrainChain* chainAttrib;
  const AttribDataRig* rigAttrib;
  const AttribDataTransformBuffer* inputTransformsAttrib;
  const AttribDataPlaybackPos* curFrameTimePos;
  const AttribDataSyncEventTrack* syncEventTrackAttrib;
  const AttribDataDurationEventTrackSet* durationEventTrackSetAttrib;
  const AttribDataUpdatePlaybackPos* networkUpdateTimeAttrib;  
  const AttribDataPredictiveUnevenTerrainPredictionDef* predictionDefAttr;
  const AttribDataBool* predictionEnableAttrib;
  const AttribDataTrajectoryDeltaTransform* inputTrajectoryDeltaAttrib;

  // IK solver parameters
  UnevenTerrainHipsIK hipsIKSolver;
  UnevenTerrainLegIK legIKSolvers[2];
  UnevenTerrainFootIK footIKSolvers[2];

  // Uneven terrain const setup
  setupDefAttrib = parameters->getInputAttrib<AttribDataBasicUnevenTerrainSetup>(0, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // IK chain information
  chainAttrib = parameters->getInputAttrib<AttribDataBasicUnevenTerrainChain>(1, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  // Character controller (2 - update bool flag)
  characterControllerAttrib = parameters->getInputAttrib<AttribDataCharacterProperties>(3, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);

  // Animation rig
  rigAttrib = parameters->getInputAttrib<AttribDataRig>(4, ATTRIB_SEMANTIC_RIG);

  // Input transforms buffer
  inputTransformsAttrib = parameters->getInputAttrib<AttribDataTransformBuffer>(5, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // Uneven terrain output IK setup information
  ikSetupAttrib = parameters->createOutputAttribReplace<AttribDataBasicUnevenTerrainIKSetup>(6, ATTRIB_SEMANTIC_UNEVEN_TERRAIN_IK_SETUP);

  // The current playback position for this node
  curFrameTimePos = parameters->getInputAttrib<AttribDataPlaybackPos>(7, ATTRIB_SEMANTIC_TIME_POS);

  // Sync event track for this node
  syncEventTrackAttrib = parameters->getInputAttrib<AttribDataSyncEventTrack>(8, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);

  // Duration event track set for this node
  durationEventTrackSetAttrib = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(9, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);

  // Network update time
  networkUpdateTimeAttrib = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(10, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  // Input/Output foot plant prediction state
  predictionStateAttrib = parameters->createOutputAttribReplace<AttribDataPredictiveUnevenTerrainPredictionState>(11, ATTRIB_SEMANTIC_UNEVEN_TERRAIN_PREDICTION_STATE);

  // Prediction def data
  predictionDefAttr = parameters->getInputAttrib<AttribDataPredictiveUnevenTerrainPredictionDef>(12, ATTRIB_SEMANTIC_UNEVEN_TERRAIN_PREDICTION_DEF);

  // Prediction Enable
  predictionEnableAttrib = parameters->getInputAttrib<AttribDataBool>(13, ATTRIB_SEMANTIC_CP_BOOL);
  bool predictionEnable = predictionEnableAttrib->m_value;

  // Trajectory delta transform
  inputTrajectoryDeltaAttrib = parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(14, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);

#ifdef TEST_UT_TASK_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\n----------------------------------------------------\n");
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "TaskPredictiveUnevenTerrainIKSetup()\n");
#endif

  //---------------------------
  // IK Setup information
  unevenTerrainIKSetup(
    &hipsIKSolver,
    legIKSolvers,
    footIKSolvers,
    ikSetupAttrib,
    setupDefAttrib,
    characterControllerAttrib,
    chainAttrib,
    rigAttrib,
    inputTransformsAttrib,
    inputTrajectoryDeltaAttrib->m_deltaPos,
    inputTrajectoryDeltaAttrib->m_deltaAtt);

  //---------------------------
  // Update the gait cycle timing information
  gaitCycleUpdateTimings(
    curFrameTimePos,
    syncEventTrackAttrib,
    durationEventTrackSetAttrib,
    predictionDefAttr,
    predictionStateAttrib,
    predictionEnable);

  //---------------------------
  // Update the trajectory prediction information
  trajectoryPrediction(
    setupDefAttrib,
    characterControllerAttrib,
    ikSetupAttrib,
    networkUpdateTimeAttrib,
    predictionStateAttrib,
    predictionEnable);

  NM_UT_END_PROFILING();
}

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
// \brief This task performs a ray-cast with the physics from the character hips height in the
// downward vertical direction. This task can not be performed externally on a SPU.
//----------------------------------------------------------------------------------------------------------------------
void TaskPredictiveUnevenTerrainFootLiftingTarget(Dispatcher::TaskParameters* parameters)
{
  NM_UT_BEGIN_PROFILING("PREDICTIVE_UT_FOOT_LIFTING_TARGET_TASK");

  const AttribDataBasicUnevenTerrainSetup* setupAttrib;
  const AttribDataBasicUnevenTerrainChain* chainAttrib;
  const AttribDataPredictiveUnevenTerrainPredictionDef* predictionDefAttrib;
  const AttribDataBasicUnevenTerrainIKSetup* inputIKSetupAttrib;
  const AttribDataPredictiveUnevenTerrainPredictionState* inputPredictionStateAttrib;
  AttribDataBasicUnevenTerrainFootLiftingTarget* outputFootLiftingTargetAttrib;
  AttribDataPredictiveUnevenTerrainFootLiftingState* footLiftingStateAttrib;
  const AttribDataBasicUnevenTerrainIKState* inputIKStateAttrib;
  const AttribDataBool* predictionEnableAttrib;

#ifdef TEST_UT_TASK_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\n----------------------------------------------------\n");
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "TaskPredictiveUnevenTerrainFootLiftingTarget()\n");
#endif

  // Uneven terrain const setup
  setupAttrib = parameters->getInputAttrib<AttribDataBasicUnevenTerrainSetup>(0, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // Uneven terrain IK chain information
  chainAttrib = parameters->getInputAttrib<AttribDataBasicUnevenTerrainChain>(1, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  // Uneven terrain prediction definition information
  predictionDefAttrib = parameters->getInputAttrib<AttribDataPredictiveUnevenTerrainPredictionDef>(2, ATTRIB_SEMANTIC_UNEVEN_TERRAIN_PREDICTION_DEF);

  // Uneven terrain input IK setup information
  inputIKSetupAttrib = parameters->getInputAttrib<AttribDataBasicUnevenTerrainIKSetup>(3, ATTRIB_SEMANTIC_UNEVEN_TERRAIN_IK_SETUP);

  // Character controller (4 - update bool flag)

  // Foot plant prediction state (this frames state)
  inputPredictionStateAttrib = parameters->getInputAttrib<AttribDataPredictiveUnevenTerrainPredictionState>(5, ATTRIB_SEMANTIC_UNEVEN_TERRAIN_PREDICTION_STATE);

  // Foot lifting target information (Output)
  outputFootLiftingTargetAttrib = parameters->createOutputAttribReplace<AttribDataBasicUnevenTerrainFootLiftingTarget>(6, ATTRIB_SEMANTIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET);

  // Foot plant prediction state (Input | Output)
  footLiftingStateAttrib = parameters->createOutputAttribReplace<AttribDataPredictiveUnevenTerrainFootLiftingState>(7, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2);

  // Uneven terrain input IK state (Input - may not exist)
  inputIKStateAttrib = parameters->getOptionalInputAttrib<AttribDataBasicUnevenTerrainIKState>(8, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);

  // Prediction Enable
  predictionEnableAttrib = parameters->getInputAttrib<AttribDataBool>(9, ATTRIB_SEMANTIC_CP_BOOL);
  bool predictionEnable = predictionEnableAttrib->m_value;

  //---------------------------
  // Information
  Network* net = parameters->m_dispatcher->getTaskQueue()->m_owningNetwork;
  NMP_ASSERT(net);
  const CharacterControllerInterface* cc = net->getCharacterController();
  NMP_ASSERT(cc);

  // Compute the up axis direction for foot lifting in the frame of the world
  NMP::Vector3 upAxisWS;
  inputIKSetupAttrib->m_characterRootWorldTM.rotateVector(setupAttrib->m_upAxis, upAxisWS);

  // Determine if the feet are in a contact phase
  footLiftingTargetInContactPhase(
    inputPredictionStateAttrib,
    outputFootLiftingTargetAttrib);

  // Check if the character controller achieved its requested motion
  if (net->getCharacterPropertiesAchievedRequestedMovement())
  {
    // Compute the trajectory basis
    terrainFootLiftingBasisFrame(
      upAxisWS,
      inputIKSetupAttrib,
      inputPredictionStateAttrib,
      footLiftingStateAttrib);

    // Invalidate raycast entries if we have transitioned into a new foot cycle.
    // Enable the ray-cast information for foot lifting based on:
    //   - If in a contact phase
    //   - The proximity of the current foot position to the predicted foot plant
    //   - The lateral support region (changes in trajectory)
    terrainFootLiftingStateInitialise(
      upAxisWS,
      chainAttrib,
      predictionDefAttrib,
      inputIKSetupAttrib,
      inputPredictionStateAttrib,
      footLiftingStateAttrib,
      predictionEnable);

    // Alternate ray casting between the current and forward prediction footbase positions.
    // While the current footbase position is updated every other frame, the forward prediction
    // terrain positions are generally alternated between the mid and next foot plant positions.
    // Determine whether the gradient of the slope at the updated ray-cast position is too steep.
    terrainFootLiftingStateUpdateRayCast(
      upAxisWS,
      net,
      cc,
      chainAttrib,
      predictionDefAttrib,
      inputIKSetupAttrib,
      inputPredictionStateAttrib,
      footLiftingStateAttrib);

    // Validate the approximated ray-cast data based on:
    //   - The lifting support region (approximate shape of terrain between foot plants)
    terrainFootLiftingStateValidate(
      predictionDefAttrib,
      inputPredictionStateAttrib,
      footLiftingStateAttrib);

    // Compute the terrain hit and hill intersection points list and determine the foot lifting
    // vector to the highest peak. Compute the foot lifting target by projection the current
    // footbase position onto the lifting vector.
    terrainFootLiftingStateComputeLiftingTarget(
      upAxisWS,
      inputIKSetupAttrib,
      footLiftingStateAttrib,
      outputFootLiftingTargetAttrib,
      inputIKStateAttrib,
      inputPredictionStateAttrib);
  }
  else
  {
#ifdef TEST_UT_TASK_LOGGING
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "Character controller did not achieve its requested motion\n");
#endif

    // Invalidate the foot lifting state
    footLiftingStateAttrib->reset();

    // Set the foot lifting targets from the current source animation positions
    unevenTerrainDefaultFootLiftingTargets(
      upAxisWS,
      inputIKSetupAttrib,
      outputFootLiftingTargetAttrib);
  }

  NM_UT_END_PROFILING();
}

#endif // !NM_HOST_CELL_SPU

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
