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
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMathUtils.h"
#include "morpheme/mrEventTrackSync.h"
#include "morpheme/mrSyncEventPos.h"
#include "morpheme/mrNetworkLogger.h"
#include "morpheme/mrManager.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Extract an events track clip from a source events track.
// All values are fractional (>= 0.0 and <= 1.0).
void EventTrackSync::init(
  TimeFraction           clipStartFraction,       // Where to start sampling from the source track.
  TimeFraction           clipDurationFraction,    // Length of the clip to sample.
  uint8_t                startDiscreteEventIndex, // Which source discrete event does the clipStartFraction lie within.
  EventTrackDefDiscrete* sourceTrack,             // Track to sample from.
  bool                   shouldLoop,              // Is this to be a looping clip.
  int32_t                startEventIndex,         // Which event to use as playback start. Also marks the blend start point.
  float                  clipDurationSeconds,     // Duration in seconds.
  bool                   playBackwards)           // Generate the track as if we were playing the source backwards.
{
  uint32_t  clipEventIndex = 0;

  // Where will our clip end.
  float clipEndFraction = clipStartFraction + clipDurationFraction;

  // What source event do we start in.
  NMP_ASSERT(sourceTrack);
  uint32_t srcEventIndex = startDiscreteEventIndex;
 
  // Find remaining events within the clip.
  if (clipDurationFraction > ERROR_LIMIT)
  {
    float srcEventStart;
    while (srcEventIndex < sourceTrack->getNumEvents())
    {
      srcEventStart = sourceTrack->getEvent(srcEventIndex)->getStartTime();
      if (srcEventStart >= clipStartFraction && srcEventStart <= clipEndFraction)  // Only add the event if we encompass it
      {
        if (clipEventIndex < MAX_NUM_SYNC_EVENTS)
        {
          m_events[clipEventIndex].setStartTime((srcEventStart - clipStartFraction) / clipDurationFraction);
          clipEventIndex++;
        }
        else
        {
          // Cannot continue adding events as we have exceeded the available buffer array space. Try incrementing max buffer size: MAX_NUM_SYNC_EVENTS.
          NET_LOG_ERROR_MESSAGE("ERROR: Max num sync events exceeded in source discrete track: %s\n", sourceTrack->getTrackName());
        }
      }
      srcEventIndex++;
    }
  }


  if (clipEventIndex == 0)
  {
    // Clips with no events in are currently not valid (May wish to cope with no events in an event sequence?).
    // For now add a default one at the start.
    m_events[clipEventIndex].setStartTime(0.0f);
    clipEventIndex++;
  }

  // Calculate the durations.
  uint32_t i;
  for (i = 0; i < clipEventIndex - 1; ++i)
  {
    m_events[i].setDuration(m_events[i + 1].getStartTime() - m_events[i].getStartTime());
    m_events[i].setUserData(0);
  }

  // Special case for the last event
  if (shouldLoop)
    m_events[i].setDuration(1.0f - (m_events[i].getStartTime() - m_events[0].getStartTime()));
  else
    m_events[i].setDuration(1.0f - m_events[i].getStartTime());
  m_events[i].setUserData(0);
  NMP_ASSERT(((float) m_events[i].getDuration()) >= 0.0f);
  NMP_ASSERT(clipEventIndex > 0);
  m_numEvents = clipEventIndex;

  // Make sure start event is within our range of events.
  setStartEventIndex(startEventIndex);

  // Store the clip duration (seconds).
  NMP_ASSERT(clipDurationSeconds >= 0.0f);
  m_duration = clipDurationSeconds;

  // Update the duration reciprocal.
  if (m_duration > ERROR_LIMIT)
    m_durationReciprocal = 1.0f / m_duration;
  else
    m_durationReciprocal = 0.0f;

  // Reverse if playing backwards
  if (playBackwards)
    reverse(shouldLoop);
}

//----------------------------------------------------------------------------------------------------------------------
bool EventTrackSync::validateSourceEventCount(
  TimeFraction           clipStartFraction,       // Where to start sampling from the source track.
  TimeFraction           clipDurationFraction,    // Length of the clip to sample.
  uint8_t                startDiscreteEventIndex, // Which source discrete event does the clipStartFraction lie within.
  EventTrackDefDiscrete* sourceTrack,             // Track to sample from.
  const char**           sourceTrackName)         // Out: name of the source discrete event track.  // char* const* 
 
{
  uint32_t  clipEventCount = 0;
  
  // Where will our clip end.
  float clipEndFraction = clipStartFraction + clipDurationFraction;

  // What source event do we start in.
  NMP_ASSERT(sourceTrack);
  uint32_t srcEventIndex = startDiscreteEventIndex;

  *sourceTrackName = sourceTrack->getTrackName();

  // Find remaining events within the clip.
  if (clipDurationFraction > ERROR_LIMIT)
  {
    float srcEventStart;
    while (srcEventIndex < sourceTrack->getNumEvents())
    {
      srcEventStart = sourceTrack->getEvent(srcEventIndex)->getStartTime();
      if (srcEventStart >= clipStartFraction && srcEventStart <= clipEndFraction)  // Only add the event if we encompass it
      {
        if (clipEventCount < MAX_NUM_SYNC_EVENTS)
        {
          clipEventCount++;
        }
        else
        {
          // Cannot continue adding events as we have exceeded the available buffer array space. Try incrementing max buffer size: MAX_NUM_SYNC_EVENTS.
          NET_LOG_ERROR_MESSAGE("ERROR: Max num sync events exceeded in source discrete track: %s\n", sourceTrackName);
          return false;
        }
      }
      srcEventIndex++;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackSync::reverse(bool shouldLoop)
{
  EventTrackSync    working;
  EventDefDiscrete* workingEvents = working.getEvents();

  NMP_ASSERT(m_numEvents > 0);

  // Reverse in to temp working track.
  if (shouldLoop && (m_events[0].getStartTime() > 0.0f))
  {
    int32_t inI = 0;
    int32_t outI = (int32_t)m_numEvents - 1;
    float startTime = 1.0f - m_events[outI].getStartTime();
    float duration;
    for (; inI < (int32_t)m_numEvents - 1; ++inI, --outI)
    {
      workingEvents[inI].setUserData(m_events[outI].getUserData());
      workingEvents[inI].setStartTime(startTime);

      duration = m_events[outI - 1].getDuration();
      workingEvents[inI].setDuration(duration);
      startTime += duration;
    }

    // Deal with the last event.
    workingEvents[inI].setUserData(m_events[outI].getUserData());
    workingEvents[inI].setStartTime(startTime);
    duration = m_events[m_numEvents - 1].getDuration();
    workingEvents[inI].setDuration(duration);
  }
  else
  {
    int32_t outI = (int32_t)m_numEvents - 1;
    float startTime = 0.0f;
    for (int32_t inI = 0; inI < (int32_t)m_numEvents; ++inI, --outI)
    {
      workingEvents[inI].setUserData(m_events[outI].getUserData());
      workingEvents[inI].setStartTime(startTime);
      workingEvents[inI].setDuration(m_events[outI].getDuration());
      startTime += m_events[outI].getDuration();
    }
  }

  // Copy from temp working track.
  for (uint32_t i = 0; i < m_numEvents; ++i)
  {
    m_events[i].copy(workingEvents[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Only blends durations. Start times match up with blended durations.
// It is meaningless to try and blend start times.
void EventTrackSync::blendClipTracks(
  const EventTrackSync* clip1,       // Source clip 1.
  const EventTrackSync* clip2,       // Source clip 2.
  float                 blendWeight, // Blend weighting.
  uint32_t              clip2Offset) // Offset of clip2 events in clip1 for blending.
{
  int32_t clip1Index = 0;
  int32_t clip2Index = 0;
  float   durationScale;
  float   workingDuration;
  float   event1Duration;
  float   event2Duration;

  NMP_ASSERT(clip1 && clip2);
  NMP_ASSERT(blendWeight >= 0.0f && blendWeight <= 1.0f);
  NMP_ASSERT(clip1->m_numEvents > 0 && clip2->m_numEvents > 0);

  // Store values for calculating duration in seconds.
  float source1Duration = clip1->getDuration();
  float source2Duration = clip2->getDuration();

  // clip1/clip2 are const so we can cache these to avoid LHS penalties further down
  const int32_t clip1StartEventIndex = (int32_t)clip1->getStartEventIndex();
  const int32_t clip2StartEventIndex = (int32_t)clip2->getStartEventIndex();
  const float clip1NumEvents = (float)clip1->m_numEvents;
  const float clip2NumEvents = (float)clip2->m_numEvents;

  TimeFraction start = 0.0f;
  if (clip1->m_numEvents >= clip2->m_numEvents)
  {
    // Loop through events using clip1 as our basis.
    m_numEvents = clip1->m_numEvents;
    durationScale = clip2NumEvents / clip1NumEvents;
    for (uint32_t i = 0; i < m_numEvents; ++i)
    {
      clip1Index = (i + clip1StartEventIndex) % clip1->m_numEvents;
      clip2Index = (i + clip2StartEventIndex + clip2Offset) % clip2->m_numEvents;
      event1Duration = clip1->m_events[clip1Index].getDuration();
      event2Duration = clip2->m_events[clip2Index].getDuration() * durationScale;

      m_events[i].setStartTime(start);

      const float duration = event1Duration + ((event2Duration - event1Duration) * blendWeight);
      m_events[i].setDuration(duration);
      start += duration;
    }

    // For calculating duration in seconds.
    source2Duration *= (clip1NumEvents / clip2NumEvents);
  }
  else
  {
    // Loop through events using clip2 as our basis.
    m_numEvents = clip2->m_numEvents;
    durationScale = clip1NumEvents / clip2NumEvents;
    for (uint32_t i = 0; i < m_numEvents; ++i)
    {
      clip1Index = (i + clip1StartEventIndex) % clip1->m_numEvents;
      clip2Index = (i + clip2StartEventIndex + clip2Offset) % clip2->m_numEvents;
      event1Duration = clip1->m_events[clip1Index].getDuration() * durationScale;
      event2Duration = clip2->m_events[clip2Index].getDuration();

      m_events[i].setStartTime(start);

      const float duration = event1Duration + ((event2Duration - event1Duration) * blendWeight);
      m_events[i].setDuration(duration);
      start += duration;
    }

    // For calculating duration in seconds.
    source1Duration *= (clip2NumEvents / clip1NumEvents);
  }

  // Make sure that the resulting event sequence is normalised.
  durationScale = 1.0f / start;
  start = 0.0f;
  for (uint32_t i = 0; i < m_numEvents; i++)
  {
    m_events[i].setStartTime(m_events[i].getStartTime() * durationScale);
    m_events[i].setDuration(m_events[i].getDuration() * durationScale);
  }

  // Make sure our last interval takes us back to the first event,
  // checking for floating point inaccuracies as we do so.
  if (m_events[m_numEvents - 1].getStartTime() > 1.0f)
    m_events[m_numEvents - 1].setStartTime(1.0f);
  if (((float) m_events[m_numEvents - 1].getDuration()) < 0.0f)
    m_events[m_numEvents - 1].setDuration(0.0f);
  workingDuration = 1.0f - m_events[m_numEvents - 1].getStartTime();
  if (workingDuration < 0.0f)
    workingDuration = 0.0f;
  m_events[m_numEvents - 1].setDuration(workingDuration);

  // Blend the duration.
  m_duration = source1Duration + (blendWeight * (source2Duration - source1Duration));
  if (m_duration > ERROR_LIMIT)
    m_durationReciprocal = 1.0f / m_duration;
  else
    m_durationReciprocal = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackSync::copyClipTrack(const EventTrackSync* sourceClip, bool rebaseFromStartEventIndex)
{
  NMP_ASSERT(sourceClip);

  uint32_t copyIndex = 0;
  m_startEventIndex = sourceClip->m_startEventIndex;
  if (rebaseFromStartEventIndex)
  {
    copyIndex = sourceClip->m_startEventIndex;
    m_startEventIndex = 0;
  }

  // Copy the events from the source. 
  // Note that m_startEventIndex in the source becomes event index 0 in the copy if we have been asked to re-base the copy.
  for (uint32_t i = 0; i < sourceClip->m_numEvents; i++)
  {
    uint32_t moduloCopyIndex = copyIndex % sourceClip->m_numEvents;
    m_events[i].setDuration(sourceClip->m_events[moduloCopyIndex].getDuration());
    m_events[i].setStartTime(sourceClip->m_events[moduloCopyIndex].getStartTime());
    ++copyIndex;
  }

  m_numEvents = sourceClip->m_numEvents;
  m_duration = sourceClip->m_duration;
  m_durationReciprocal = sourceClip->m_durationReciprocal;
  NMP_ASSERT(m_startEventIndex < m_numEvents);
}

//----------------------------------------------------------------------------------------------------------------------
bool EventTrackSync::limitToSyncEventSpace(float& eventPos, bool loopable) const
{
  if (m_duration < ERROR_LIMIT)
  {
    eventPos = 0.0f;
    return false;
  }

  bool result = false;
  float numEvents = (float) m_numEvents;
  if (loopable)
  {
    while (eventPos >= numEvents)
    {
      eventPos -= numEvents;
      result = true;
    }
  }
  else
  {
    if (eventPos >= numEvents)
    {
      eventPos = numEvents;
      result = true;
    }
  }
  NMP_ASSERT(eventPos >= 0.0f);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool EventTrackSync::limitToSyncEventSpace(SyncEventPos& eventPos, bool loopable) const
{
  if (m_duration < ERROR_LIMIT)
  {
    eventPos.set(0, 0.0f);
    return false;
  }

  NMP_ASSERT(m_numEvents > 0);
  bool result = false;
  if (eventPos.index() >= m_numEvents)
  {
    if (loopable)
    {
      // Limit event index to lie within sync event space.
      eventPos.setIndex(eventPos.index() % m_numEvents);
    }
    else
    {
      // Cap event position to end of last event.
      eventPos.setIndex(m_numEvents - 1); ;
      eventPos.setFraction(1.0f);
    }
    result = true;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackSync::getAdjPosInfoFromRealFraction(
  TimeFraction  realPlaybackPosFraction, // IN: In real space.
  SyncEventPos& adjEventPos) const       // OUT: In space adjusted by m_StartEventIndex.
{
  uint32_t  clipSegmentIndex;
  float     fractionThroughClipSegment;

  getAdjPosInfoFromRealFraction(realPlaybackPosFraction, clipSegmentIndex, fractionThroughClipSegment);
  adjEventPos.set(clipSegmentIndex, fractionThroughClipSegment);
}

//----------------------------------------------------------------------------------------------------------------------
float EventTrackSync::getAdjPosInfoFromRealFraction(TimeFraction realPlaybackPosFraction) const
{
  uint32_t  clipSegmentIndex;
  float     fractionThroughClipSegment;

  getAdjPosInfoFromRealFraction(realPlaybackPosFraction, clipSegmentIndex, fractionThroughClipSegment);

  return (((float) clipSegmentIndex) + fractionThroughClipSegment);
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackSync::getAdjPosInfoFromRealFraction(
  TimeFraction  realPlaybackPosFraction,    // IN: In real space.
  uint32_t&     adjEventIndex,              // OUT: In space adjusted by m_StartEventIndex.
  float&        fractionThroughEvent) const // OUT: In space adjusted by m_StartEventIndex.
{
  uint32_t index;

  NMP_ASSERT(realPlaybackPosFraction >= 0.0f && realPlaybackPosFraction <= 1.0f);
  NMP_ASSERT(m_numEvents > 0);

  fractionThroughEvent = -1.0f;

  if (realPlaybackPosFraction < m_events[0].getStartTime())
  {
    // We're dealing with a looping clip and we're before the first event
    // so the event is the last one.
    adjEventIndex = m_numEvents - 1;
    // Now we can get the correct fraction through segment.
    if (m_events[adjEventIndex].getDuration() < ERROR_LIMIT)
    {
      fractionThroughEvent = 1.0f;
    }
    else
    {
      fractionThroughEvent =
        (m_events[adjEventIndex].getDuration() - (m_events[0].getStartTime() - realPlaybackPosFraction)) /
        m_events[adjEventIndex].getDuration();
    }
  }
  else
  {
    bool haveFoundSegment = false;
    // Search the event track for the event and fraction.
    for (index = 0; index < m_numEvents; index++)
    {
      if ((m_events[index].getStartTime() + m_events[index].getDuration()) >= realPlaybackPosFraction)
      {
        haveFoundSegment = true;
        adjEventIndex = index;
        if (m_events[index].getDuration() < ERROR_LIMIT)
        {
          fractionThroughEvent = 1.0f;
        }
        else
        {
          fractionThroughEvent = (realPlaybackPosFraction - m_events[index].getStartTime()) / m_events[index].getDuration();
          while (fractionThroughEvent > 1.0f)
            fractionThroughEvent -= 1.0f;
        }
        break;
      }
    }
    // Make sure we have found the event we are in.
    if (!haveFoundSegment)
    {
      // didn't find the event, probably overran the end of the clip due to floating point errors
      adjEventIndex = m_numEvents - 1;
      fractionThroughEvent = 1.0f;
    }
  }

  NMP_ASSERT(fractionThroughEvent >= 0.0f);

  // Adjust event index into unadjusted space for return.
  if (m_startEventIndex <= adjEventIndex)
    adjEventIndex = adjEventIndex - m_startEventIndex;
  else
  {
    NMP_ASSERT(m_numEvents >= (m_startEventIndex - adjEventIndex));
    adjEventIndex = m_numEvents - (m_startEventIndex - adjEventIndex);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackSync::getRealPosInfoFromRealFraction(
  TimeFraction  realPlaybackPosFraction, // IN: In real space.
  SyncEventPos& realEventPos) const      // OUT: In real space.
{
  uint32_t  clipSegmentIndex;
  float     fractionThroughClipSegment;

  getRealPosInfoFromRealFraction(realPlaybackPosFraction, clipSegmentIndex, fractionThroughClipSegment);
  realEventPos.set(clipSegmentIndex, fractionThroughClipSegment);
}

//----------------------------------------------------------------------------------------------------------------------
float EventTrackSync::getRealPosInfoFromRealFraction(TimeFraction realPlaybackPosFraction) const
{
  uint32_t  clipSegmentIndex;
  float     fractionThroughClipSegment;

  getRealPosInfoFromRealFraction(realPlaybackPosFraction, clipSegmentIndex, fractionThroughClipSegment);

  return (((float) clipSegmentIndex) + fractionThroughClipSegment);
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackSync::getRealPosInfoFromRealFraction(
  TimeFraction realPlaybackPosFraction,    // IN: In real space.
  uint32_t&    realEventIndex,             // OUT: In real space.
  float&       fractionThroughEvent) const // OUT: In real space.
{
  NMP_ASSERT(realPlaybackPosFraction >= 0.0f && realPlaybackPosFraction <= 1.0f);
  NMP_ASSERT(m_numEvents > 0);

  fractionThroughEvent = -1.0f;

  if (realPlaybackPosFraction < m_events[0].getStartTime())
  {
    // We're dealing with a looping clip and we're before the first event so the event is the last one.
    realEventIndex = m_numEvents - 1;
    // Now we can get the correct fraction through segment.
    if (m_events[realEventIndex].getDuration() < ERROR_LIMIT)
    {
      fractionThroughEvent = 1.0f;
    }
    else
    {
      fractionThroughEvent =
        (m_events[realEventIndex].getDuration() - (m_events[0].getStartTime() - realPlaybackPosFraction)) /
        m_events[realEventIndex].getDuration();
    }
  }
  else
  {
    // Initialise at the end of the last event case we don't find an event.
    // (probably because we overran the end of the clip due to floating point error).
    realEventIndex = m_numEvents - 1;
    fractionThroughEvent = 1.0f;
   
    // Search the event track for the event and fraction.
    for (uint32_t i = 0; i < m_numEvents; i++)
    {
      if (realPlaybackPosFraction <= (m_events[i].getStartTime() + m_events[i].getDuration()))
      {
        realEventIndex = i;
        if (m_events[i].getDuration() <= ERROR_LIMIT)
        {
          fractionThroughEvent = 1.0f;
        }
        else
        {
          fractionThroughEvent = (realPlaybackPosFraction - m_events[i].getStartTime()) / m_events[i].getDuration();
          fractionThroughEvent = NMP::minimum(fractionThroughEvent, 1.0f);
        }
        break;
      }
    }
  }

  NMP_ASSERT(fractionThroughEvent >= 0.0f);
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackSync::getAdjPosInfoFromAdjFraction(
  TimeFraction  adjPlaybackPosFraction, // IN: In space adjusted by m_StartEventIndex.
  SyncEventPos& adjEventPos) const      // OUT: In space adjusted by m_StartEventIndex.
{
  uint32_t  clipSegmentIndex;
  float     fractionThroughClipSegment;

  getAdjPosInfoFromAdjFraction(adjPlaybackPosFraction, clipSegmentIndex, fractionThroughClipSegment);
  adjEventPos.set(clipSegmentIndex, fractionThroughClipSegment);
}

//----------------------------------------------------------------------------------------------------------------------
float EventTrackSync::getAdjPosInfoFromAdjFraction(TimeFraction adjPlaybackPosFraction) const
{
  uint32_t clipSegmentIndex;
  float    fractionThroughClipSegment;

  getAdjPosInfoFromAdjFraction(adjPlaybackPosFraction, clipSegmentIndex, fractionThroughClipSegment);

  return (((float) clipSegmentIndex) + fractionThroughClipSegment);
}

//----------------------------------------------------------------------------------------------------------------------
// Reports clip segment location within the space adjusted by the m_StartEventIndex.
void EventTrackSync::getAdjPosInfoFromAdjFraction(
  TimeFraction adjPlaybackPosFraction,      // In space adjusted by m_StartEventIndex.
  uint32_t&    adjEventIndex,               // In space adjusted by m_StartEventIndex.
  float&       fractionThroughEvent) const  // In space adjusted by m_StartEventIndex.
{
  uint32_t index;

  NMP_ASSERT(adjPlaybackPosFraction >= 0.0f && adjPlaybackPosFraction <= 1.0f);
  NMP_ASSERT(m_startEventIndex < m_numEvents);
  NMP_ASSERT(m_numEvents > 0);

  if (adjPlaybackPosFraction == 1.0f)
  {
    // We are right at the end of the clip.
    adjEventIndex = m_numEvents - 1;
    fractionThroughEvent = 1.0f;
    return;
  }
  else if (adjPlaybackPosFraction == 0.0f)
  {
    adjEventIndex = 0;
    fractionThroughEvent = 0.0f;
    return;
  }

  fractionThroughEvent = -1.0f;

  // Move playbackPos into unadjusted space in order to do comparisons.
  adjPlaybackPosFraction += m_events[m_startEventIndex].getStartTime();
  if (adjPlaybackPosFraction > 1.0f)
    adjPlaybackPosFraction -= 1.0f;
  NMP_ASSERT(adjPlaybackPosFraction >= 0.0f && adjPlaybackPosFraction <= 1.0f);

  if (adjPlaybackPosFraction < m_events[0].getStartTime())
  {
    // We're dealing with a looping clip and we're before the first event so the event we're in is the last one.
    adjEventIndex = m_numEvents - 1;
    // Now we can get the correct fraction through segment.
    if (NMP::nmfabs(m_events[adjEventIndex].getDuration() - m_events[0].getStartTime() - (1.0f - m_events[adjEventIndex].getStartTime())) < ERROR_LIMIT) // Validate that this is a looping anim.
    {
      fractionThroughEvent = (m_events[adjEventIndex].getDuration() - (m_events[0].getStartTime() - adjPlaybackPosFraction)) /
                             m_events[adjEventIndex].getDuration();
    }
    else
    {
      fractionThroughEvent = 1.0f;
    }
  }
  else
  {
    // Search the event track for the event and fraction
    bool haveFoundSegment = false;
    for (index = 0; index < m_numEvents; index++)
    {
      if ((m_events[index].getStartTime() + m_events[index].getDuration()) >= adjPlaybackPosFraction)
      {
        haveFoundSegment = true;
        adjEventIndex = index;
        if (m_events[index].getDuration() < ERROR_LIMIT)
        {
          fractionThroughEvent = 1.0f;
        }
        else
        {
          fractionThroughEvent = (adjPlaybackPosFraction - m_events[index].getStartTime()) / m_events[index].getDuration();
          while (fractionThroughEvent > 1.0f)
            fractionThroughEvent -= 1.0f;
        }
        break;
      }
    }
    // Make sure we have found the event we are in.
    if (!haveFoundSegment)
    {
      // didn't find the event, probably overran the end of the clip due to floating point errors
      adjEventIndex = m_numEvents - 1;
      fractionThroughEvent = 1.0f;
    }
  }

  // Make sure we have found the event we are lying in.
  NMP_ASSERT(fractionThroughEvent >= 0.0f);

  // Adjust event index into unadjusted space for return.
  if (m_startEventIndex <= adjEventIndex)
    adjEventIndex = adjEventIndex - m_startEventIndex;
  else
  {
    NMP_ASSERT(m_numEvents >= (m_startEventIndex - adjEventIndex));
    adjEventIndex = m_numEvents - (m_startEventIndex - adjEventIndex);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Reports a time fraction in the space adjusted by the m_StartEventIndex.
TimeFraction EventTrackSync::getAdjPosFractionFromAdjSyncEventPos(
  uint32_t adjEventIndex,              // In space adjusted by m_StartEventIndex.
  float    fractionThroughEvent) const // In space adjusted by m_StartEventIndex.
{
  NMP_ASSERT(m_numEvents > 0);
  NMP_ASSERT(m_startEventIndex < m_numEvents);
  NMP_ASSERT(fractionThroughEvent >= 0.0f && fractionThroughEvent <= 1.0f);
  adjEventIndex = (adjEventIndex + m_startEventIndex) % m_numEvents; // Wrap the clip segment index if it is greater than the number of events in this track.
  TimeFraction realPlaybackPos = m_events[adjEventIndex].getStartTime() + (m_events[adjEventIndex].getDuration() * fractionThroughEvent);
  // handle looping clips
  while (realPlaybackPos > 1.0f)
    realPlaybackPos -= 1.0f;
  NMP_ASSERT(realPlaybackPos >= 0.0f && realPlaybackPos <= 1.0f);

  // Put the play back pos in to the space adjusted by the m_StartEventIndexs start time.
  TimeFraction adjustedPlayBackPos = realPlaybackPos - m_events[m_startEventIndex].getStartTime();
  if (adjustedPlayBackPos < 0.0f)
    adjustedPlayBackPos = 1.0f + adjustedPlayBackPos;
  NMP_ASSERT(adjustedPlayBackPos >= 0.0f && adjustedPlayBackPos <= 1.0f);

  return adjustedPlayBackPos;
}

//----------------------------------------------------------------------------------------------------------------------
TimeFraction EventTrackSync::getAdjPosFractionFromAdjSyncEventPos(const SyncEventPos& adjEventPos) const
{
  return getAdjPosFractionFromAdjSyncEventPos(adjEventPos.index(), adjEventPos.fraction());
}

//----------------------------------------------------------------------------------------------------------------------
TimeFraction EventTrackSync::getAdjPosFractionFromAdjSyncEventPos(float adjEventPos) const
{
  NMP_ASSERT(adjEventPos >= 0.0f);
  uint32_t index = ((uint32_t) adjEventPos);
  float    fraction = adjEventPos - ((float) index);
  return getAdjPosFractionFromAdjSyncEventPos(index, fraction);
}

//----------------------------------------------------------------------------------------------------------------------
// Reports a time fraction in the space not adjusted by the m_StartEventIndex.
TimeFraction EventTrackSync::getRealPosFractionFromAdjSyncEventPos(
  uint32_t adjEventIndex,              // In space adjusted by m_StartEventIndex.
  float    fractionThroughEvent) const // In space adjusted by m_StartEventIndex.
{
  NMP_ASSERT(fractionThroughEvent >= 0.0f && fractionThroughEvent <= 1.0f);
  NMP_ASSERT(m_numEvents > 0);
  adjEventIndex = (adjEventIndex + m_startEventIndex) % m_numEvents; // Wrap the clip segment index if it is greater than the number of events in this track.
  TimeFraction realPlaybackPos = m_events[adjEventIndex].getStartTime() + (m_events[adjEventIndex].getDuration() * fractionThroughEvent);
  // handle looping clips
  while (realPlaybackPos > 1.0f)
    realPlaybackPos -= 1.0f;
  NMP_ASSERT(realPlaybackPos >= 0.0f && realPlaybackPos <= 1.0f);

  return realPlaybackPos;
}

//----------------------------------------------------------------------------------------------------------------------
TimeFraction EventTrackSync::getRealPosFractionFromAdjSyncEventPos(const SyncEventPos& adjEventPos) const
{
  return getRealPosFractionFromAdjSyncEventPos(adjEventPos.index(), adjEventPos.fraction());
}

//----------------------------------------------------------------------------------------------------------------------
TimeFraction EventTrackSync::getRealPosFractionFromAdjSyncEventPos(float adjEventPos) const
{
  NMP_ASSERT(adjEventPos >= 0.0f);
  uint32_t index = ((uint32_t) adjEventPos);
  float fraction = adjEventPos - ((float) index);
  return getRealPosFractionFromAdjSyncEventPos(index, fraction);
}

//----------------------------------------------------------------------------------------------------------------------
TimeFraction EventTrackSync::getRealPosFractionFromRealSyncEventPos(
  uint32_t realEventIndex,       // In real space.
  float    fractionThroughEvent  // In real space.
) const
{
  NMP_ASSERT(fractionThroughEvent >= 0.0f && fractionThroughEvent <= 1.0f);
  NMP_ASSERT(m_numEvents > 0);
  NMP_ASSERT(realEventIndex < m_numEvents);

  TimeFraction realPlaybackPos = m_events[realEventIndex].getStartTime() + (m_events[realEventIndex].getDuration() * fractionThroughEvent);
  // handle looping clips
  while (realPlaybackPos > 1.0f)
    realPlaybackPos -= 1.0f;
  NMP_ASSERT(realPlaybackPos >= 0.0f && realPlaybackPos <= 1.0f);

  return realPlaybackPos;
}

//----------------------------------------------------------------------------------------------------------------------
TimeFraction EventTrackSync::getRealPosFractionFromRealSyncEventPos(const SyncEventPos& realEventPos) const
{
  return getRealPosFractionFromRealSyncEventPos(realEventPos.index(), realEventPos.fraction());
}

//----------------------------------------------------------------------------------------------------------------------
TimeFraction EventTrackSync::getRealPosFractionFromRealSyncEventPos(float realEventPos) const
{
  NMP_ASSERT(realEventPos >= 0.0f);
  uint32_t index = ((uint32_t) realEventPos);
  float fraction = realEventPos - ((float) index);
  return getRealPosFractionFromRealSyncEventPos(index, fraction);
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackSync::convEventInRealSpaceToAdjustedSpace(
  const SyncEventPos& realEventPosIn,
  SyncEventPos&       ajdEventPosOut) const
{
  NMP_ASSERT(realEventPosIn.index() < m_numEvents); // Input event must lie within limits.

  uint32_t clipSegmentIndex;
  if (m_startEventIndex <= realEventPosIn.index())
    clipSegmentIndex = realEventPosIn.index() - m_startEventIndex;
  else
  {
    NMP_ASSERT(m_numEvents >= (m_startEventIndex - realEventPosIn.index()));
    clipSegmentIndex = m_numEvents - (m_startEventIndex - realEventPosIn.index());
  }
  ajdEventPosOut.set(clipSegmentIndex, realEventPosIn.fraction());
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackSync::convEventInAdjustedSpaceToRealSpace(
  const SyncEventPos& adjEventPosIn,
  SyncEventPos&       realEventPosOut,
  uint32_t            startOffset) const
{
  NMP_ASSERT(adjEventPosIn.index() <= m_numEvents); // Input event must lie within limits.

  uint32_t clipSegmentIndex = (adjEventPosIn.index() + m_startEventIndex + startOffset) % m_numEvents;
  realEventPosOut.set(clipSegmentIndex, adjEventPosIn.fraction());
}

//----------------------------------------------------------------------------------------------------------------------
float EventTrackSync::convEventInRealSpaceToAdjustedSpace(float realEventPosIn) const
{
  float numEvents = (float) m_numEvents;
  NMP_ASSERT(realEventPosIn <= numEvents); // Input event must lie within limits.

  float adjEventPosOut = realEventPosIn - m_startEventIndex;
  while (adjEventPosOut < 0.0f)
    adjEventPosOut += numEvents;
  return adjEventPosOut;
}

//----------------------------------------------------------------------------------------------------------------------
float EventTrackSync::convEventInAdjustedSpaceToRealSpace(float adjEventPosIn, uint32_t startOffset) const
{
  float numEvents = (float) m_numEvents;
  NMP_ASSERT(adjEventPosIn < numEvents); // Input event must lie within limits.
  float realEventPosOut = adjEventPosIn + m_startEventIndex + startOffset;
  while (realEventPosOut >= numEvents)
    realEventPosOut -= numEvents;
  return realEventPosOut;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
bool EventTrackSync::locate()
{
  NMP::endianSwap(m_startEventIndex);
  NMP::endianSwap(m_numEvents);
  NMP::endianSwap(m_duration);
  NMP::endianSwap(m_durationReciprocal);

  for (uint32_t i = 0; i < m_numEvents; ++i)
    m_events[i].locate();

  return true;
};

//----------------------------------------------------------------------------------------------------------------------
bool EventTrackSync::dislocate()
{
  for (uint32_t i = 0; i < m_numEvents; ++i)
    m_events[i].dislocate();

  NMP::endianSwap(m_durationReciprocal);
  NMP::endianSwap(m_duration);
  NMP::endianSwap(m_numEvents);
  NMP::endianSwap(m_startEventIndex);

  return true;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_NETWORK_LOGGING)
void EventTrackSync::debugOutputContents(int32_t priority)
{
  NET_LOG_MESSAGE(priority, "Sync Event Track, StartEventIndex = %d, NumEvents = %d, Duration = %f\n", m_startEventIndex, m_numEvents, m_duration);
  NET_LOG_MESSAGE(priority, "StartTime, Duration\n");
  for (uint32_t i = 0; i < m_numEvents; ++i)
  {
    float startTime = m_events[i].getStartTime();
    float duration = m_events[i].getDuration();
    NET_LOG_MESSAGE(priority, " %1.6f, %1.6f\n", startTime, duration);
  }
}
#else // #if defined(MR_NETWORK_LOGGING)
void EventTrackSync::debugOutputContents(int32_t priority)
{
  (void) priority;
}
#endif // #if defined(MR_NETWORK_LOGGING)

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
