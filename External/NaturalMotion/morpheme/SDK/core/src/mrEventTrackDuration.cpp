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
#include "morpheme/mrEventTrackDuration.h"
#include "morpheme/mrEventTrackCurve.h"
#include "morpheme/mrManager.h"
#include "NMPlatform/NMMathUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

const float blendingErrorTolerance = 0.0001f;

//----------------------------------------------------------------------------------------------------------------------
// EventTrackDefDuration functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
bool EventTrackDefDuration::locate()
{
  EventTrackDefBase::locate();

  NMP::endianSwap(m_events);
  REFIX_PTR_RELATIVE(EventDefDuration, m_events, this);

  for (uint32_t i = 0; i < m_numEvents; ++i)
    m_events[i].locate();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool EventTrackDefDuration::dislocate()
{
  for (uint32_t i = 0; i < m_numEvents; ++i)
    m_events[i].dislocate();

  UNFIX_PTR_RELATIVE(EventDefDuration, m_events, this);
  NMP::endianSwap(m_events);

  EventTrackDefBase::dislocate();

  return true;
}
#endif // NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void EventTrackDefDuration::relocate()
{
  void* ptr = this;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  ptr = (void*)(((size_t)ptr) + sizeof(EventTrackDefDuration));

  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  m_events = (EventDefDuration*)ptr;
  ptr = (void*)(((size_t)ptr) + (sizeof(MR::EventDefDuration) * m_numEvents));

  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  m_name = (char*)ptr;
}

//----------------------------------------------------------------------------------------------------------------------
// EventDefDuration functions.
//----------------------------------------------------------------------------------------------------------------------
void EventDuration::blend(
  EventDuration* event1,
  EventDuration* event2,
  float          numEventsInSyncEventSpace,  // Num events in the time synchronisation event space we are working in (no fractional part).
  float          weight)                     // 0.0 = fully event1, 1.0 = fully event2.
{
  float eventSpaceDistanceForwards;
  float eventSpaceDistanceBackwards;
  float interpolatedDistance;

  NMP_ASSERT(event1 && event2);
  NMP_ASSERT(weight >= 0.0 && weight <= 1.0f)

  if (weight < (0.0f + blendingErrorTolerance))
  {
    // If close enough to zero just copy from event1.
    m_syncEventSpaceStartPoint = event1->getSyncEventSpaceStartPoint();
    m_syncEventSpaceMidPoint = event1->getSyncEventSpaceMidPoint();
    m_syncEventSpaceDuration = event1->getSyncEventSpaceDuration();
    m_weight = event1->getWeight();
  }
  else if (weight > (1.0f - blendingErrorTolerance))
  {
    // If close enough to 1.0 just copy from event2.
    m_syncEventSpaceStartPoint = event2->getSyncEventSpaceStartPoint();
    m_syncEventSpaceMidPoint = event2->getSyncEventSpaceMidPoint();
    m_syncEventSpaceDuration = event2->getSyncEventSpaceDuration();
    m_weight = event2->getWeight();
  }
  else
  {
    // -- Find the blended mid point --.
    // Determine which direction we should be blending in forwards/clockwise or backwards/anti-clockwise.
    eventSpaceDistanceForwards = event1->getDistanceToEventPosForward(
                                   event2->getSyncEventSpaceMidPoint(),
                                   numEventsInSyncEventSpace,
                                   true);
    eventSpaceDistanceBackwards = event1->getDistanceToEventPosBackwards(
                                    event2->getSyncEventSpaceMidPoint(),
                                    numEventsInSyncEventSpace,
                                    true);
    if (eventSpaceDistanceBackwards < eventSpaceDistanceForwards)
    {
      // Blend backwards.
      interpolatedDistance = eventSpaceDistanceBackwards * weight;
      m_syncEventSpaceMidPoint = event1->getSyncEventSpaceMidPoint() - interpolatedDistance;
      if (m_syncEventSpaceMidPoint < 0.0f)
      {
        // Cope with wrapping around.
        m_syncEventSpaceMidPoint = numEventsInSyncEventSpace + m_syncEventSpaceMidPoint;
        NMP_ASSERT(m_syncEventSpaceMidPoint >= 0.0f && m_syncEventSpaceMidPoint <= numEventsInSyncEventSpace);
      }
    }
    else
    {
      // Blend forwards.
      interpolatedDistance = eventSpaceDistanceForwards * weight;
      m_syncEventSpaceMidPoint = event1->getSyncEventSpaceMidPoint() + interpolatedDistance;
      if (m_syncEventSpaceMidPoint >= numEventsInSyncEventSpace)
      {
        // Cope with wrapping around.
        m_syncEventSpaceMidPoint -= numEventsInSyncEventSpace;
        NMP_ASSERT(m_syncEventSpaceMidPoint >= 0.0f && m_syncEventSpaceMidPoint < numEventsInSyncEventSpace);
      }
    }

    // -- Find the blended duration and weight --.
    m_syncEventSpaceDuration = event1->getSyncEventSpaceDuration() + ((event2->getSyncEventSpaceDuration() - event1->getSyncEventSpaceDuration()) * weight);
    m_weight = event1->getWeight() + ((event2->getWeight() - event1->getWeight()) * weight);

    // -- Find the blended start point --.
    m_syncEventSpaceStartPoint = m_syncEventSpaceMidPoint - (m_syncEventSpaceDuration * 0.5f);
    if (m_syncEventSpaceStartPoint < 0.0f) // deal with wrapping around.
    {
      m_syncEventSpaceStartPoint = numEventsInSyncEventSpace + m_syncEventSpaceStartPoint;
      while (m_syncEventSpaceStartPoint >= numEventsInSyncEventSpace)
        m_syncEventSpaceStartPoint -= numEventsInSyncEventSpace;
      NMP_ASSERT(m_syncEventSpaceStartPoint >= 0.0f && m_syncEventSpaceStartPoint < numEventsInSyncEventSpace);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Determines if this event overlaps with another by any amount.
bool EventDuration::overlapsWith(
  const EventDuration* testEvent,
  float                numEventsInSyncEventSpace) const// Num events in the time synchronisation event space we are working in (no fractional part).
{
  NMP_ASSERT(testEvent);

  // Determine the distance between the mid point of this event and the test event forwards/clockwise or backwards/anti-clockwise.
  float eventSpaceDistanceForwards = getDistanceToEventPosForward(
                                       testEvent->getSyncEventSpaceMidPoint(),
                                       numEventsInSyncEventSpace,
                                       true);
  float eventSpaceDistanceBackwards = getDistanceToEventPosBackwards(
                                        testEvent->getSyncEventSpaceMidPoint(),
                                        numEventsInSyncEventSpace,
                                        true);

  // Work out under what range the events will overlap.
  float overlapRange = (m_syncEventSpaceDuration * 0.5f) + (testEvent->getSyncEventSpaceDuration() * 0.5f);

  if ((eventSpaceDistanceForwards < overlapRange) || (eventSpaceDistanceBackwards < overlapRange))
    return true; // Overlap.

  return false; // Don't overlap.
}

//----------------------------------------------------------------------------------------------------------------------
// Determines if this event is within the specified range of the test event.
bool EventDuration::withinRange(
  const EventDuration* testEvent,
  float                testRange,
  float                numEventsInSyncEventSpace) const // Num events in the time synchronisation event space we are working in (no fractional part).
{
  NMP_ASSERT(testEvent);

  // Determine the distance between the mid point of this event and the test event forwards/clockwise or backwards/anti-clockwise.
  float eventSpaceDistanceForwards = getDistanceToEventPosForward(
                                       testEvent->getSyncEventSpaceMidPoint(),
                                       numEventsInSyncEventSpace,
                                       true);
  float eventSpaceDistanceBackwards = getDistanceToEventPosBackwards(
                                        testEvent->getSyncEventSpaceMidPoint(),
                                        numEventsInSyncEventSpace,
                                        true);

  // Work out under what range the events will overlap.
  float overlapRange = (m_syncEventSpaceDuration * 0.5f) + (testEvent->getSyncEventSpaceDuration() * 0.5f);

  if ((eventSpaceDistanceForwards < overlapRange) || (eventSpaceDistanceBackwards < overlapRange))
    return true; // Overlap.

  if ((eventSpaceDistanceForwards - overlapRange) < testRange)
    return true; // Within range.

  if ((eventSpaceDistanceBackwards - overlapRange) < testRange)
    return true; // Within range.

  return false; // Not within range.
}

//----------------------------------------------------------------------------------------------------------------------
float EventDuration::getDistanceToEventPosForward(
  float toSyncEventSpaceMidPoint,
  float numEventsInSyncEventSpace,
  bool  loop) const
{
  float eventSpaceDistance = numEventsInSyncEventSpace;

  NMP_ASSERT(m_syncEventSpaceMidPoint < numEventsInSyncEventSpace);
  NMP_ASSERT(toSyncEventSpaceMidPoint <= numEventsInSyncEventSpace);

  if (toSyncEventSpaceMidPoint >= m_syncEventSpaceMidPoint)
  {
    // No wrapping.
    eventSpaceDistance = toSyncEventSpaceMidPoint - m_syncEventSpaceMidPoint;
  }
  else if (loop)
  {
    // Wraps around.
    eventSpaceDistance = numEventsInSyncEventSpace - m_syncEventSpaceMidPoint + toSyncEventSpaceMidPoint;
  }

  return eventSpaceDistance;
}

//----------------------------------------------------------------------------------------------------------------------
float EventDuration::getDistanceToEventPosBackwards(
  float toSyncEventSpaceMidPoint,
  float numEventsInSyncEventSpace,
  bool  loop) const
{
  float eventSpaceDistance = numEventsInSyncEventSpace;

  NMP_ASSERT(m_syncEventSpaceMidPoint < numEventsInSyncEventSpace);
  NMP_ASSERT(toSyncEventSpaceMidPoint <= numEventsInSyncEventSpace);

  if (toSyncEventSpaceMidPoint <= m_syncEventSpaceMidPoint)
  {
    // No wrapping.
    eventSpaceDistance = m_syncEventSpaceMidPoint - toSyncEventSpaceMidPoint;
  }
  else if (loop)
  {
    // Wraps around.
    eventSpaceDistance = m_syncEventSpaceMidPoint + (numEventsInSyncEventSpace - toSyncEventSpaceMidPoint);
  }

  return eventSpaceDistance;
}

//----------------------------------------------------------------------------------------------------------------------
// If within the event, returns the fractional position within the event. 1.0 = start to 0.0 = end.
// Note that the fraction position is still in event space
bool EventDuration::liesWithin(
  float  syncEventSpaceTestPos,
  float  numEventsInSyncEventSpace,
  float& positionInEvent,
  bool   loop) const
{
  // get the (event space) distance to the midpoint of the duration event
  float eventSpaceDistanceForwards = getDistanceToEventPosForward(syncEventSpaceTestPos, numEventsInSyncEventSpace, loop);
  if (eventSpaceDistanceForwards <= (m_syncEventSpaceDuration * 0.5f))
  {
    // Read line below as: positionInEvent = if (m_syncEventSpaceDuration > 0.0f) ...
    positionInEvent = NMP::floatSelect(m_syncEventSpaceDuration, ((m_syncEventSpaceDuration * 0.5f) - eventSpaceDistanceForwards) / m_syncEventSpaceDuration, 0.0f);
    NMP_ASSERT(positionInEvent >= 0.0f && positionInEvent <= 1.0f);
    return true;
  }
  
  float eventSpaceDistanceBackwards = getDistanceToEventPosBackwards(syncEventSpaceTestPos, numEventsInSyncEventSpace, loop);
  if (eventSpaceDistanceBackwards <= (m_syncEventSpaceDuration * 0.5f))
  {
    // Read line below as: positionInEvent = if (m_syncEventSpaceDuration > 0.0f) ...
    positionInEvent = NMP::floatSelect(m_syncEventSpaceDuration, (eventSpaceDistanceBackwards + (m_syncEventSpaceDuration * 0.5f)) / m_syncEventSpaceDuration, 0.0f);
    NMP_ASSERT(positionInEvent >= 0.0f && positionInEvent <= 1.0f);
    return true;
  }

  positionInEvent = 0.0f;
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// EventTrackDuration functions.
//----------------------------------------------------------------------------------------------------------------------
EventTrackDuration* EventTrackDuration::create(NMP::MemoryAllocator* allocator)
{
  EventTrackDuration* result;

  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  result = init(resource);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format EventTrackDuration::getMemoryRequirements()
{
  return NMP::Memory::Format(sizeof(EventTrackDuration), NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
EventTrackDuration* EventTrackDuration::init(NMP::Memory::Resource& resource)
{
  NMP::Memory::Format format(sizeof(EventTrackDuration), NMP_NATURAL_TYPE_ALIGNMENT);

  resource.align(format);
  EventTrackDuration* result = (EventTrackDuration*)resource.ptr;
  resource.increment(format);

  result->m_runtimeID = 0xFFFFFFFF;
  result->m_userData = 0xFFFFFFFF;
  result->m_numSyncEvents = 0xFFFFFFFF;
  result->m_eventsListHead = gUndefinedEventIndex;
  result->m_eventsListTail = gUndefinedEventIndex;
  result->m_numEvents = 0;
  result->m_pool = NULL;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackDuration::init()
{
  m_pool = NULL;
  m_numEvents = 0;
  m_runtimeID = 0xFFFFFFFF;
  m_userData = 0xFFFFFFFF;
  m_numSyncEvents = 0;
  m_eventsListHead = gUndefinedEventIndex;
  m_eventsListTail = gUndefinedEventIndex;
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackDuration::initFromSourceDef(
  float                  clipStartPosTrackFraction, // Clip start time as a fraction of the whole source track.
                                                    //  Defines where to start sampling from the duration event track.
  float                  clipDurationTrackFraction, // Length of the clip to sample as a fraction of the whole source track.
  EventTrackDefDuration* durationEventTrackDef,     //
  uint32_t               runtimeID,
  EventTrackSync*        syncEventTrack,            // Sync space within which these tracks are defined.
  EventDurationPool*     pool,
  bool                   loopingAllowed,            // Are events allowed to loop beyond the end of the sync event track.
  bool                   playBackwards)             // Initialise this data to be played backwards.
{
  EventDefDuration* sourceEventDef;
  float             clipEndPosTrackFraction;    // Clip end time as a fraction of the whole source track.
  
  float             eventStartPosTrackFraction; // Duration event start time as a fraction of the whole source track.
  float             eventEndPosTrackFraction;   // Duration event end time as a fraction of the whole source track.
  float             eventDurationTrackFraction; //
  float             eventStartPosClipFraction;  // Duration event start time as a fraction of the clip section.
  float             eventEndPosClipFraction;    // Duration event end time as a fraction of the clip section.
  float             eventStartPosSyncSpace;
  float             eventMidPosSyncSpace;
  float             eventEndPosSyncSpace;
  float             eventDurationSyncSpace;
  float             modEventEndPosTrackFraction;

  NMP_ASSERT(durationEventTrackDef && syncEventTrack && pool);
  NMP_ASSERT(m_numEvents == 0);

  m_pool = pool;

  //-----------------------
  // Store some useful info about the clip section.
  float numSyncEventsInClip = (float) syncEventTrack->getNumEvents(); // Encompassed in the clip section.
  clipEndPosTrackFraction = clipStartPosTrackFraction + clipDurationTrackFraction;
  NMP_ASSERT(clipEndPosTrackFraction <= 1.0f);
  float recipClipDurationTrackFraction = 0.0f;
  if (clipDurationTrackFraction > ERROR_LIMIT)
    recipClipDurationTrackFraction = 1.0f / clipDurationTrackFraction;
 
  //-----------------------
  // Find events that occur within the clip section and add them to our result duration event track.
  for (uint32_t i = 0; i < durationEventTrackDef->getNumEvents(); i++)
  {
    sourceEventDef = durationEventTrackDef->getEvent(i);

    //-----------------------
    // Store some useful info about this duration event.
    eventStartPosTrackFraction = sourceEventDef->getStartTime();
    eventDurationTrackFraction = sourceEventDef->getDuration();
    NMP_ASSERT(sourceEventDef->getDuration() >= 0.0f && sourceEventDef->getDuration() <= 1.0f); // Events can only last a maximum of 1 loop of the source. Values greater than this are meaningless in our time/event space systems.
    eventEndPosTrackFraction = eventStartPosTrackFraction + eventDurationTrackFraction;
    modEventEndPosTrackFraction = fmodf(eventEndPosTrackFraction, 1.0f);
   
    //-----------------------
    // Work out if this duration event is coincident with the clip section at all.

    if ((eventStartPosTrackFraction >= clipStartPosTrackFraction) && (eventStartPosTrackFraction <= clipEndPosTrackFraction))
    {
      //-----------------------
      // Start of event lies within clip section.
      eventStartPosClipFraction = (eventStartPosTrackFraction - clipStartPosTrackFraction) * recipClipDurationTrackFraction;

      // Calculate the end position in clip fraction space.
      if (eventEndPosTrackFraction <= clipEndPosTrackFraction)
      {
        eventEndPosClipFraction = (eventEndPosTrackFraction - clipStartPosTrackFraction) * recipClipDurationTrackFraction;  // Event end comes before the clip section end.
      }
      else
      {
        if ((modEventEndPosTrackFraction >= clipStartPosTrackFraction) && (modEventEndPosTrackFraction <= clipEndPosTrackFraction) && loopingAllowed)
          eventEndPosClipFraction = (modEventEndPosTrackFraction - clipStartPosTrackFraction) * recipClipDurationTrackFraction; // Event loops around past the start of the clip section.
        else
          eventEndPosClipFraction = 1.0f;  // Event is clipped off by the end of the clip section.
      }

      // Flip start and end positions if playing backwards.
      if (playBackwards)
      {
        float temp = eventStartPosClipFraction;
        eventStartPosClipFraction = 1.0f - eventEndPosClipFraction;
        eventEndPosClipFraction = 1.0f - temp;
      }

      // Calculate sync event space start, end, duration and mid point.
      eventStartPosSyncSpace = syncEventTrack->getAdjPosInfoFromRealFraction(eventStartPosClipFraction);
      eventEndPosSyncSpace = syncEventTrack->getAdjPosInfoFromRealFraction(eventEndPosClipFraction);
      if (eventDurationTrackFraction >= 0.9999)
        eventDurationSyncSpace = numSyncEventsInClip;
      else if (eventEndPosSyncSpace < eventStartPosSyncSpace)
        eventDurationSyncSpace = (numSyncEventsInClip - eventStartPosSyncSpace) + eventEndPosSyncSpace;
      else
        eventDurationSyncSpace = eventEndPosSyncSpace - eventStartPosSyncSpace;
      eventMidPosSyncSpace = fmodf(eventStartPosSyncSpace + (eventDurationSyncSpace * 0.5f), numSyncEventsInClip);

      // Add a new event to the resulting track.
      createAndAddSorted(
        eventStartPosSyncSpace,         // Start point of this event within time stretch event space.
        eventMidPosSyncSpace,           // Mid point of this event within time stretch event space.
        eventDurationSyncSpace,         // Time to end as a time stretch event fraction.
        1.0f,                           // Weight.
        sourceEventDef->getUserData()); // Event User data.
    }
    else if ((modEventEndPosTrackFraction >= clipStartPosTrackFraction) && (modEventEndPosTrackFraction <= clipEndPosTrackFraction))
    {
      //-----------------------
      // End of event lies within clip section and start of event is at the beginning of the clip section.
      
      // Calculate the end position in clip fraction space.
      eventStartPosClipFraction = 0.0f;
      eventEndPosClipFraction = (modEventEndPosTrackFraction - clipStartPosTrackFraction) * recipClipDurationTrackFraction;

      // Flip start and end positions if playing backwards.
      if (playBackwards)
      {
        float temp = eventStartPosClipFraction;
        eventStartPosClipFraction = 1.0f - eventEndPosClipFraction;
        eventEndPosClipFraction = 1.0f - temp;
      }

      // Calculate sync event space start, end, duration and mid point.
      eventStartPosSyncSpace = syncEventTrack->getAdjPosInfoFromRealFraction(eventStartPosClipFraction);
      eventEndPosSyncSpace = syncEventTrack->getAdjPosInfoFromRealFraction(eventEndPosClipFraction);
      if (eventEndPosSyncSpace < eventStartPosSyncSpace)
        eventDurationSyncSpace = (numSyncEventsInClip - eventStartPosSyncSpace) + eventEndPosSyncSpace;
      else
        eventDurationSyncSpace = eventEndPosSyncSpace - eventStartPosSyncSpace;
      eventMidPosSyncSpace = fmodf(eventStartPosSyncSpace + (eventDurationSyncSpace * 0.5f), numSyncEventsInClip);

      // Add a new event to the resulting track.
      createAndAddSorted(
        eventStartPosSyncSpace,         // Start point of this event within time stretch event space.
        eventMidPosSyncSpace,           // Mid point of this event within time stretch event space.
        eventDurationSyncSpace,         // Time to end as a time stretch event fraction.
        1.0f,                           // Weight.
        sourceEventDef->getUserData()); // Event User data.
    }
    else if (((eventStartPosTrackFraction < clipStartPosTrackFraction) && (modEventEndPosTrackFraction > clipEndPosTrackFraction)) ||
             ((eventStartPosTrackFraction >= modEventEndPosTrackFraction) &&
              (((eventStartPosTrackFraction > clipEndPosTrackFraction) && (modEventEndPosTrackFraction > clipEndPosTrackFraction)) ||
               ((eventStartPosTrackFraction < clipStartPosTrackFraction) && (modEventEndPosTrackFraction < clipStartPosTrackFraction)))))
    {
      //-----------------------
      // Event encompasses clip section.     
      eventStartPosSyncSpace = syncEventTrack->getAdjPosInfoFromRealFraction(0.0f);
      eventEndPosSyncSpace = syncEventTrack->getAdjPosInfoFromRealFraction(1.0f);
      eventDurationSyncSpace = numSyncEventsInClip;
      eventMidPosSyncSpace = fmodf(eventStartPosSyncSpace + (eventDurationSyncSpace * 0.5f), numSyncEventsInClip);

      // Add a new event to the resulting track.
      createAndAddSorted(
        eventStartPosSyncSpace,         // Start point of this event within time stretch event space.
        eventMidPosSyncSpace,           // Mid point of this event within time stretch event space.
        eventDurationSyncSpace,         // Time to end as a time stretch event fraction.
        1.0f,                           // Weight.
        sourceEventDef->getUserData()); // Event User data.
    }
  }

  // Fill in remaining track data.
  m_runtimeID = runtimeID;
  m_userData = durationEventTrackDef->getUserData();
  m_numSyncEvents = syncEventTrack->getNumEvents();
}

//----------------------------------------------------------------------------------------------------------------------
EventDuration* EventTrackDuration::addToTail(EventDuration* eventToAdd, EventIndex eventIndex)
{
  NMP_ASSERT(eventToAdd && m_pool);

  if (m_eventsListTail != gUndefinedEventIndex)
  {
    // Events must be added in increasing start point sequence.
    EventDuration* tailEvent = m_pool->getEvent(m_eventsListTail);
    NMP_ASSERT(eventToAdd->getSyncEventSpaceStartPoint() >= tailEvent->getSyncEventSpaceStartPoint());

    // Not an empty list only need to affect the tail.
    tailEvent->setNextIndex(eventIndex);
    m_eventsListTail = eventIndex;
  }
  else
  {
    // Empty list.
    m_eventsListHead = eventIndex;
    m_eventsListTail = eventIndex;
  }
  eventToAdd->setNextIndex(gUndefinedEventIndex);

  return eventToAdd;
}

//----------------------------------------------------------------------------------------------------------------------
EventDuration* EventTrackDuration::addToHead(EventDuration* eventToAdd, EventIndex eventIndex)
{
  NMP_ASSERT(eventToAdd);

  if (m_eventsListHead != gUndefinedEventIndex)
  {
    // Events must be added in increasing start point sequence.
    NMP_ASSERT(eventToAdd->getSyncEventSpaceStartPoint() <= m_pool->getEvent(m_eventsListHead)->getSyncEventSpaceStartPoint());

    // Not an empty list only need to affect the head.
    eventToAdd->setNextIndex(m_eventsListHead);
    m_eventsListHead = eventIndex;
  }
  else
  {
    // Empty list.
    m_eventsListHead = eventIndex;
    m_eventsListTail = eventIndex;
    eventToAdd->setNextIndex(gUndefinedEventIndex);
  }

  return eventToAdd;
}

//----------------------------------------------------------------------------------------------------------------------
EventDuration* EventTrackDuration::addSorted(EventDuration* eventToAdd, EventIndex eventIndex)
{
  NMP_ASSERT(eventToAdd && m_pool);

  EventIndex currentEventIndex = m_eventsListHead;
  EventDuration* lastEvent = NULL;
  while (currentEventIndex != gUndefinedEventIndex)
  {
    EventDuration* currentEvent = m_pool->getEvent(currentEventIndex);
    if (currentEvent->getSyncEventSpaceStartPoint() > eventToAdd->getSyncEventSpaceStartPoint())
    {
      if (!lastEvent)
        return addToHead(eventToAdd, eventIndex); // Insert at head.

      // Not at head or tail insert between 2 events.
      lastEvent->setNextIndex(eventIndex);
      eventToAdd->setNextIndex(currentEventIndex);
      return eventToAdd;
    }

    lastEvent = currentEvent;
    currentEventIndex = currentEvent->getNextIndex();
  }

  return addToTail(eventToAdd, eventIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackDuration::copy(EventTrackDuration& dest)
{
  dest.setNumEvents(0);
  dest.setRuntimeID(m_runtimeID);
  dest.setUserData(m_userData);
  dest.setNumSyncEvents(m_numSyncEvents);
  dest.m_eventsListHead = gUndefinedEventIndex;
  dest.m_eventsListTail = gUndefinedEventIndex;

  EventIndex currentEventIndex = m_eventsListHead;
  while (currentEventIndex != gUndefinedEventIndex)
  {
    EventDuration* currentEvent = m_pool->getEvent(currentEventIndex);
    dest.copyAndAddToTail(currentEvent);
    currentEventIndex = currentEvent->getNextIndex();
  }

  NMP_ASSERT(dest.getNumEvents() == m_numEvents);
}

//----------------------------------------------------------------------------------------------------------------------
// Copy the supplied track and if necessary repeat it so that it covers the time synchronisation period indicated.
bool EventTrackDuration::copyRepeat(
  EventTrackDuration* sourceDurationTrack,
  uint32_t            syncEventCountToRepeatUpto,
  EventDurationPool*  pool,
  bool                loopingAllowed,     // Are events allowed to loop beyond the end of the sync event track.
  uint32_t            offset)             // Event Index offset.
{
  float  eventIndexLoopAdjust = -((float)offset);

  NMP_ASSERT(sourceDurationTrack && pool);
  NMP_ASSERT(m_numEvents == 0 && m_eventsListHead == gUndefinedEventIndex);

  m_pool = pool;

  if (!sourceDurationTrack->m_numEvents)
    return false;

  uint32_t sourceSyncEventCount = sourceDurationTrack->getNumSyncEvents();
  NMP_ASSERT(syncEventCountToRepeatUpto >= sourceSyncEventCount);

  EventIndex durationEventIndex = sourceDurationTrack->getEventsListHeadIndex();
  EventDuration* durationEvent = sourceDurationTrack->getEventsListHead();
  NMP_ASSERT(durationEvent);
  float eventSpaceStartPoint = durationEvent->getSyncEventSpaceStartPoint() + eventIndexLoopAdjust;
  while (eventSpaceStartPoint > 0.f)
  {
    eventIndexLoopAdjust -= (float)sourceSyncEventCount;
    eventSpaceStartPoint -= (float)sourceSyncEventCount;
  }

  const float overlappingEventOffset = (float)(((syncEventCountToRepeatUpto + (sourceSyncEventCount >> 1)) / sourceSyncEventCount) * sourceSyncEventCount);
  float eventSpaceStartPointLimit = ((float)syncEventCountToRepeatUpto);

  while (eventSpaceStartPoint < eventSpaceStartPointLimit)
  {
    float eventSpaceDuration = durationEvent->getSyncEventSpaceDuration();
    float eventSpaceEndPoint = eventSpaceStartPoint + eventSpaceDuration;

    // only consider events that are at least overlapping 0.0
    if (eventSpaceEndPoint >= 0.0f)
    {
      // overlapping 0.0 ?
      if (eventSpaceStartPoint < 0.0f)
      {
        NMP_ASSERT(eventSpaceEndPoint <= ((float) syncEventCountToRepeatUpto));

        // move event into last/next segment
        eventSpaceStartPoint += overlappingEventOffset;

        if (loopingAllowed)
        {
          if (eventSpaceStartPoint < eventSpaceStartPointLimit)
          {
            // event wraps over the end of the sequence so to make sure it's not added again, adjust the limit
            eventSpaceStartPointLimit = eventSpaceStartPoint;
          }
          else
          {
            // clip event at 0.0
            eventSpaceStartPoint = 0.0f;
            eventSpaceDuration = eventSpaceEndPoint;
          }
        }
        else
        {
          // clip event at 0.0
          eventSpaceDuration = eventSpaceEndPoint;
        }
      }
      // start point >= 0.0
      else
      {
        if ((eventSpaceEndPoint >= eventSpaceStartPointLimit))
        {
          eventSpaceDuration = eventSpaceStartPointLimit - eventSpaceStartPoint;
        }
      }

      float eventSpaceMidPoint = eventSpaceStartPoint + (eventSpaceDuration * 0.5f);
      if (eventSpaceMidPoint >= ((float)syncEventCountToRepeatUpto)) // Cope with wrapping mid points.
        eventSpaceMidPoint -= ((float)syncEventCountToRepeatUpto);
      NMP_ASSERT(eventSpaceMidPoint >= 0.0f && eventSpaceMidPoint < ((float)syncEventCountToRepeatUpto));

      createAndAddSorted(
        eventSpaceStartPoint,
        eventSpaceMidPoint,
        eventSpaceDuration,
        durationEvent->getWeight(),
        durationEvent->getUserData());
    }

    durationEventIndex = durationEvent->getNextIndex();
    durationEvent = sourceDurationTrack->getPool()->getEvent(durationEventIndex);
    if (durationEventIndex == gUndefinedEventIndex)
    {
      eventIndexLoopAdjust += ((float) sourceSyncEventCount);
      durationEventIndex = sourceDurationTrack->getEventsListHeadIndex();
      durationEvent = sourceDurationTrack->getPool()->getEvent(durationEventIndex);
    }
    eventSpaceStartPoint = durationEvent->getSyncEventSpaceStartPoint() + eventIndexLoopAdjust;
  }

  m_numSyncEvents = syncEventCountToRepeatUpto;
  m_userData = sourceDurationTrack->getUserData();
  m_runtimeID = sourceDurationTrack->getRuntimeID();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
EventDuration* EventTrackDuration::findEventWithUserData(EventDuration* eventListPos, uint32_t userdata)
{
  while (eventListPos)
  {
    if (eventListPos->getUserData() == userdata)
      return eventListPos;
    eventListPos = eventListPos->getNext(m_pool);
  }
  return eventListPos;
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackDuration::sample(
  SampledCurveEventsBuffer* curveSamplesBuffer,  // Curve buffer to add samples too.
  const SyncEventPos*       syncEventPos,        // Time stretch event position to sample at.
  const SyncEventPos*       prevSyncEventPos,    // Previous time stretch event position sampled.
  const EventTrackSync*     syncEventTrack,      // Time stretch event sequence.
  bool                      loop)                // Looping animation
{
  EventDuration* currentEvent;
  float          fractionalPosInEvent; // 1.0 = start, 0.0 = end.
  float          numSyncEvents = (float) m_numSyncEvents; // do conversion once.

  NMP_ASSERT(curveSamplesBuffer);
  NMP_ASSERT(numSyncEvents > 0);

  currentEvent = m_pool->getEvent(m_eventsListHead);
  while (currentEvent)
  {
    bool prevSampleLiesWithinEvent = currentEvent->liesWithin(prevSyncEventPos->get(), numSyncEvents, fractionalPosInEvent, loop);
    bool sampleLiesWithinEvent = currentEvent->liesWithin(syncEventPos->get(), numSyncEvents, fractionalPosInEvent, loop);

    if (sampleLiesWithinEvent || prevSampleLiesWithinEvent)
    {
      // fractionalPosInEvent is specified in event space - i.e. non linear
      // Need to move into time space
      const float syncEventLength = (float)m_numSyncEvents;
      float endEventPosUnwrapped = currentEvent->getSyncEventSpaceStartPoint() + currentEvent->getSyncEventSpaceDuration();
      // Wrap the end point if the event duration goes beyond the end of the animation
      if (endEventPosUnwrapped >= syncEventLength)
      {
        float endEventPosWrapped = fmodf(endEventPosUnwrapped, syncEventLength);

        // When wrapping the end event position clamp it to the start to deal with floating point errors.
        endEventPosUnwrapped = NMP::minimum(endEventPosWrapped, currentEvent->getSyncEventSpaceStartPoint());
      }

      float sampleEventPosUnwrapped = currentEvent->getSyncEventSpaceStartPoint() +
        (1.0f - fractionalPosInEvent) * currentEvent->getSyncEventSpaceDuration();

      SyncEventPos startEventPos(currentEvent->getSyncEventSpaceStartPoint());
      SyncEventPos endEventPos(endEventPosUnwrapped);
      SyncEventPos sampleEventPos(fmodf(sampleEventPosUnwrapped, syncEventLength));

      TimeFraction realStartPos = syncEventTrack->getRealPosFractionFromAdjSyncEventPos(startEventPos);
      TimeFraction realEndPos = syncEventTrack->getRealPosFractionFromAdjSyncEventPos(endEventPos);
      TimeFraction realSamplePos = syncEventTrack->getRealPosFractionFromAdjSyncEventPos(sampleEventPos);
      
      NMP_ASSERT((realStartPos >= 0.0f) && (realStartPos <= 1.0f));
      NMP_ASSERT((realEndPos >= 0.0f) && (realEndPos <= 1.0f));
      NMP_ASSERT((realSamplePos >= 0.0f) && (realSamplePos <= 1.0f));

      float realDuration;
      float realEventFraction;
      if (realEndPos > realStartPos)
      {
        NMP_ASSERT((realSamplePos >= realStartPos) && (realSamplePos <= realEndPos));
        realDuration = realEndPos - realStartPos;
        realEventFraction = (realDuration!=0) ? (realEndPos - realSamplePos) / realDuration : 0.0f;
        NMP_ASSERT((realEventFraction >= 0.0f) && (realEventFraction <= 1.0f));
      }
      else
      {
        realDuration = realEndPos + (1.0f - realStartPos);
        if (realSamplePos <= realEndPos)
        {
          realEventFraction = (realDuration!=0) ? (realEndPos - realSamplePos) / realDuration : 0.0f;
          NMP_ASSERT((realEventFraction >= 0.0f) && (realEventFraction <= 1.0f));
        }
        else
        {
          realEventFraction = (realDuration!=0) ? 1.0f - ((realSamplePos - realStartPos) / realDuration) : 1.0f;
          NMP_ASSERT((realEventFraction >= 0.0f) && (realEventFraction <= 1.0f));
        }
      }

      NMP_ASSERT((realEventFraction >= 0.0f) && (realEventFraction <= 1.0f));

      // Add a sample to the output buffer.
      realEventFraction = NMP::clampValue(realEventFraction, 0.0f, 1.0f);
      curveSamplesBuffer->addSampledEvent(
        m_userData,
        m_runtimeID,
        currentEvent->getUserData(),
        realEventFraction);
    }
    currentEvent = currentEvent->getNext(m_pool);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// NOTE: Will only blend event tracks that have the same runtimeID and userData.
void EventTrackDuration::blend(
  EventTrackDuration* clip0,                 // Source clip 0.
  EventTrackDuration* clip1,                 // Source clip 1.
  float               weight,                // Blend weighting.
  float               selectionWeightCutOff, // At what proportional weight do we select one events details over the other.
                                             // Below this value select clip1, above it select clip1.
  uint32_t            matchingOperation,     // Whether and how to match up the events from each track.
                                             // See DURATION_EVENT_MATCH_...
  bool                rejectNonMatched,      // If an event from one of the sources has not been matched to one
                                             // in the other source should we just discard it?
  uint32_t            blendingOperation,     // Operation to perform of events that have been matched up.
                                             // See DURATION_EVENT_BLEND_OP_TYPE_...
  EventDurationPool*  pool,
  bool                loopingAllowed,        // Are events allowed to loop beyond the end of the sync event track.
  float               testRange,             // Within what range do we decide to match events together.
                                             // When event matching WITHIN_RANGE.
                                             // Expressed as an event fraction.
  uint32_t            clip1Offset)           // Offset of clip1 events in clip1 for blending.
{
  NMP_ASSERT(pool && clip0 && clip1);
  NMP_ASSERT(weight >= 0.0f && weight <= 1.0f);
  NMP_ASSERT(clip0->getUserData() == clip1->getUserData());
  NMP_ASSERT(clip0->getRuntimeID() == clip1->getRuntimeID());

  m_pool = pool;

  uint32_t numSyncEvents0 = clip0->getNumSyncEvents();
  uint32_t numSyncEvents1 = clip1->getNumSyncEvents();

  //-------------------------------------------------------------------------
  // Scale clips to match each others time synchronisation event counts if necessary.

  // Make working clips ready for use.
  EventTrackDuration workingClip0;
  workingClip0.init();
  EventTrackDuration workingClip1;
  workingClip1.init();

  uint32_t numSyncEvents = NMP::maximum(numSyncEvents0, numSyncEvents1);

  // Generate a new event track for clip0 as it needs to loop to encompass the number of synchronisation events in clip1.
  EventTrackDuration* inputTrack0 = &workingClip0;
  inputTrack0->copyRepeat(clip0, numSyncEvents, pool, loopingAllowed, clip1Offset);

  // Generate a new event track for clip1 as it needs to loop to encompass the number of synchronisation events in clip0.
  EventTrackDuration* inputTrack1 = &workingClip1;
  inputTrack1->copyRepeat(clip1, numSyncEvents, pool, loopingAllowed, clip1Offset);

  //-------------------------------------------------------------------------
  // Blend clips.

  m_userData = inputTrack0->getUserData();
  m_runtimeID = inputTrack0->getRuntimeID();
  m_numSyncEvents = inputTrack0->getNumSyncEvents();

  switch (matchingOperation)
  {
  case DURATION_EVENT_MATCH_PASS_THROUGH:
    blendPassThrough(inputTrack0, inputTrack1);
    break;
  case DURATION_EVENT_MATCH_IN_SEQUENCE:
    blendInSequence(inputTrack0, inputTrack1, weight, selectionWeightCutOff, blendingOperation, rejectNonMatched);
    break;
  case DURATION_EVENT_MATCH_SAME_IDS:
    blendSameUserData(inputTrack0, inputTrack1, weight, selectionWeightCutOff, blendingOperation, rejectNonMatched);
    break;
  case DURATION_EVENT_MATCH_ON_OVERLAP:
    blendOnOverlap(inputTrack0, inputTrack1, weight, selectionWeightCutOff, blendingOperation, rejectNonMatched);
    break;
  case DURATION_EVENT_MATCH_WITHIN_RANGE:
    blendWithinRange(inputTrack0, inputTrack1, weight, selectionWeightCutOff, blendingOperation, rejectNonMatched, testRange);
    break;
  case DURATION_EVENT_MATCH_IN_SEQUENCE_SAME_IDS_ON_OVERLAP:
    // In sequence and with the same IDs and if overlapping.
    blendInSequenceSameUserDataOnOverlap(inputTrack0, inputTrack1, weight, selectionWeightCutOff, blendingOperation, rejectNonMatched);
    break;
  case DURATION_EVENT_MATCH_IN_SEQUENCE_SAME_IDS_WITHIN_RANGE:
    // In sequence and with the same IDs and within a specified range.
    blendInSequenceSameUserDataWithinRange(inputTrack0, inputTrack1, weight, selectionWeightCutOff, blendingOperation, rejectNonMatched, testRange);
    break;
  case DURATION_EVENT_MATCH_IN_SEQUENCE_SAME_IDS:
    // In sequence and with the same IDs.
    blendInSequenceSameUserData(inputTrack0, inputTrack1, weight, selectionWeightCutOff, blendingOperation, rejectNonMatched);
    break;
  case DURATION_EVENT_MATCH_IN_SEQUENCE_ON_OVERLAP:
    // In sequence and if overlapping.
    blendInSequenceOnOverlap(inputTrack0, inputTrack1, weight, selectionWeightCutOff, blendingOperation, rejectNonMatched);
    break;
  case DURATION_EVENT_MATCH_IN_SEQUENCE_WITHIN_RANGE:
    // In sequence and within a specified range.
    blendInSequenceWithinRange(inputTrack0, inputTrack1, weight, selectionWeightCutOff, blendingOperation, rejectNonMatched, testRange);
    break;
  case DURATION_EVENT_MATCH_SAME_IDS_ON_OVERLAP:
    // With the same IDs and if overlapping.
    blendSameUserDataOnOverlap(inputTrack0, inputTrack1, weight, selectionWeightCutOff, blendingOperation, rejectNonMatched);
    break;
  case DURATION_EVENT_MATCH_SAME_IDS_WITHIN_RANGE:
    // With the same IDs and within a specified range.
    blendSameUserDataWithinRange(inputTrack0, inputTrack1, weight, selectionWeightCutOff, blendingOperation, rejectNonMatched, testRange);
    break;
  default:
    NMP_DEBUG_MSG("Unknown event matching operation");
    NMP_ASSERT_FAIL();
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Set the status flag on all events in the supplied list.
void EventTrackDuration::setProcessedFlags(bool status, EventDuration* eventListHead)
{
  EventDuration* currentEvent = eventListHead;
  while (currentEvent)
  {
    currentEvent->setProcessed(status);
    currentEvent = currentEvent->getNext(m_pool);
  }
}

//----------------------------------------------------------------------------------------------------------------------
EventDuration* EventTrackDuration::findOverlappingEvent(
  const EventDuration* testEvent,           // Event to test.
  EventDuration*       eventListHead,       // List of events to test against.
  float                numEventsInSyncSpace) // Num time synchronisation events in the event space we are working in (no fractional part).
{
  NMP_ASSERT(testEvent);

  EventDuration* currentEvent = eventListHead;
  while (currentEvent)
  {
    if (testEvent->overlapsWith(currentEvent, numEventsInSyncSpace))
      return currentEvent;  // Overlap found.
    currentEvent = currentEvent->getNext(m_pool);
  }

  return NULL; // No overlaps found.
}

//----------------------------------------------------------------------------------------------------------------------
EventDuration* EventTrackDuration::findEventWithinRange(
  const EventDuration* testEvent,     // Event to test.
  EventDuration*       eventListHead,       // List of events to test against.
  float                range,                // Expressed as an event fraction.
  float                numEventsInSyncSpace) // Num time synchronisation events in the event space we are working in (no fractional part).
{
  NMP_ASSERT(testEvent);

  EventDuration* currentEvent = eventListHead;
  while (currentEvent)
  {
    if (testEvent->withinRange(currentEvent, range, numEventsInSyncSpace))
      return currentEvent;  // Overlap found.
    currentEvent = currentEvent->getNext(m_pool);
  }

  return NULL; // No overlaps found.
}

//----------------------------------------------------------------------------------------------------------------------
EventDuration* EventTrackDuration::findEventWithSameUserData(
  const EventDuration* testEvent,
  EventDuration*       eventListHead)
{
  NMP_ASSERT(testEvent);

  EventDuration* currentEvent = eventListHead;
  while (currentEvent)
  {
    if (testEvent->getUserData() == currentEvent->getUserData())
      return currentEvent;  // Has same user data.
    currentEvent = currentEvent->getNext(m_pool);
  }

  return NULL; // No events within range found.
}

//----------------------------------------------------------------------------------------------------------------------
EventDuration* EventTrackDuration::findEventWithSameUserDataAndOverlapping(
  const EventDuration*  testEvent,
  EventDuration*        eventListHead,
  float                 numEventsInSyncSpace)  // Num time synchronisation events in the event space we are working in (no fractional part).
{
  NMP_ASSERT(testEvent);

  EventDuration* currentEvent = eventListHead;
  while (currentEvent)
  {
    if ((testEvent->getUserData() == currentEvent->getUserData()) && (testEvent->overlapsWith(currentEvent, numEventsInSyncSpace)))
      return currentEvent;  // Has same user data and overlaps.
    currentEvent = currentEvent->getNext(m_pool);
  }

  return NULL; // No events with same user data and overlapping found.
}

//----------------------------------------------------------------------------------------------------------------------
EventDuration* EventTrackDuration::findEventWithSameUserDataAndWithinRange(
  const EventDuration* testEvent,       // Event to test.
  EventDuration*       eventListHead,         // List of events to test against.
  float                range,                 // Expressed as an event fraction.
  float                numEventsInSyncSpace) // Num time synchronisation events in the event space we are working in (no fractional part).
{
  NMP_ASSERT(testEvent);

  EventDuration* currentEvent = eventListHead;
  while (currentEvent)
  {
    if ((testEvent->getUserData() == currentEvent->getUserData()) && (testEvent->withinRange(currentEvent, range, numEventsInSyncSpace)))
      return currentEvent;  // Has same user data and in range.
    currentEvent = currentEvent->getNext(m_pool);
  }

  return NULL; // No events with same user data and within range found.
}

//----------------------------------------------------------------------------------------------------------------------
// Do not attempt to match events together for blending or rejection just copy all events straight to output.
void EventTrackDuration::blendPassThrough(
  EventTrackDuration* clip0,
  EventTrackDuration* clip1)
{
  NMP_ASSERT(clip0 && clip1);
  NMP_ASSERT(clip0->getNumSyncEvents() == clip1->getNumSyncEvents());
  NMP_ASSERT(m_numEvents == 0);

  // Copy event track 1 first.
  EventDuration* event0 = clip0->getEventsListHead();
  while (event0)
  {
    copyAndAddToTail(event0);
    event0 = event0->getNext(clip0->getPool());
  }

  // Copy event track 2.
  EventDuration* event1 = clip1->getEventsListHead();
  while (event1)
  {
    copyAndAddSorted(event1);
    event1 = event1->getNext(clip1->getPool());
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Blend or Reject blindly in sequence. If there are remaining events in one track then simply copy them into the result.
void EventTrackDuration::blendInSequence(
  EventTrackDuration* clip0,
  EventTrackDuration* clip1,
  float               weight,                // Blend weighting.
  float               selectionWeightCutOff, // At what proportional weight do we select one events details over the other.
  uint32_t            blendingOperation,     // Operation to perform on events that have been matched up. See EVENT_BLEND_OP_TYPE_...
  bool                rejectNonMatched)      // Discard un-matched events.
{
  EventDuration blendedEvent;     // Working event which events will be blended into before copying into final track.

  NMP_ASSERT(clip0 && clip1);
  NMP_ASSERT(clip0->getNumSyncEvents() == clip1->getNumSyncEvents());
  NMP_ASSERT(m_numEvents == 0);

  float numEventsInEventSpace = (float) clip0->getNumSyncEvents();

  EventDuration* event0 = clip0->getEventsListHead();
  EventDuration* event1 = clip1->getEventsListHead();

  while (event0 && event1)
  {
    switch (blendingOperation)
    {
    case DURATION_EVENT_BLEND_OP_TYPE_BLEND:     // Blend events and store in result.
    {
      blendedEvent.blend(event0, event1, numEventsInEventSpace, weight);
      if (weight <= selectionWeightCutOff)
        blendedEvent.setUserData(event0->getUserData());
      else
        blendedEvent.setUserData(event1->getUserData());
      copyAndAddSorted(&blendedEvent);
      break;
    }
    case DURATION_EVENT_BLEND_OP_TYPE_SELECT:  // Select event to store in result.
    {
      if (weight <= selectionWeightCutOff)
        copyAndAddSorted(event0);
      else
        copyAndAddSorted(event1);
      break;
    }
    default:
    {
      NMP_DEBUG_MSG("Unknown event blending type requested");
      NMP_ASSERT_FAIL();
      break;
    }
    }

    event0 = event0->getNext(clip0->getPool());
    event1 = event1->getNext(clip1->getPool());
  }

  if (!rejectNonMatched)
  {
    while (event0)
    {
      // Only track 1 still has events to copy.
      copyAndAddSorted(event0);
      event0 = event0->getNext(clip0->getPool());
    }
    while (event1)
    {
      // Only track 2 still has events to copy.
      copyAndAddSorted(event1);
      event1 = event1->getNext(clip1->getPool());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Only consider events with the same ID for Blending or Rejection.
void EventTrackDuration::blendSameUserData(
  EventTrackDuration* clip0,
  EventTrackDuration* clip1,
  float               weight,                // Blend weighting.
  float               selectionWeightCutOff, // At what proportional weight do we select one events details over the other.
  uint32_t            blendingOperation,     // Operation to perform on events that have been matched up. See EVENT_BLEND_OP_TYPE_...
  bool                rejectNonMatched)      // Discard un-matched events.
{
  EventDuration* event0;
  EventDuration* event1;
  EventDuration  blendedEvent;     // Working event which events will be blended into before copying into final track.
  float          numEventsInEventSpace;

  NMP_ASSERT(clip0 && clip1);
  NMP_ASSERT(clip0->getNumSyncEvents() == clip1->getNumSyncEvents());
  NMP_ASSERT(m_numEvents == 0);

  numEventsInEventSpace = (float) clip0->getNumSyncEvents();

  // Clear all processed flags on source arrays.
  event0 = clip0->getEventsListHead();
  clip0->setProcessedFlags(false, event0);
  event1 = clip1->getEventsListHead();
  clip1->setProcessedFlags(false, event1);

  // Search for all overlapping events, blend them together and store in the resulting event track.
  while (event0)
  {
    event1 = clip1->getEventsListHead();
    event1 = clip1->findEventWithSameUserData(event0, event1);
    while (event1)
    {
      // Set source events as having been processed.
      event0->setProcessed(true);
      event1->setProcessed(true);

      switch (blendingOperation)
      {
      case DURATION_EVENT_BLEND_OP_TYPE_BLEND: // Blend events and store in result.
        blendedEvent.blend(event0, event1, numEventsInEventSpace, weight);
        if (weight <= selectionWeightCutOff)
          blendedEvent.setUserData(event0->getUserData());
        else
          blendedEvent.setUserData(event1->getUserData());
        copyAndAddSorted(&blendedEvent);
        break;
      case DURATION_EVENT_BLEND_OP_TYPE_SELECT:  // Select event to store in result.
        if (weight <= selectionWeightCutOff)
          copyAndAddSorted(event0);
        else
          copyAndAddSorted(event1);
        break;
      default:
        NMP_DEBUG_MSG("Unknown event blending type requested");
        NMP_ASSERT_FAIL();
        break;
      }

      // Continue trying to find events in clip1 that have the same user data as the clip0 event.
      event1 = event1->getNext(clip1->getPool());
      event1 =  clip1->findEventWithSameUserData(event0, event1);
    }
    event0 = event0->getNext(clip0->getPool());
  }

  // Deal with remaining events from both source tracks.
  if (!rejectNonMatched)
  {
    event0 = clip0->getEventsListHead();
    while (event0)
    {
      if (!event0->isProcessed())
        copyAndAddSorted(event0);
      event0 = event0->getNext(clip0->getPool());
    }

    event1 = clip1->getEventsListHead();
    while (event1)
    {
      if (!event1->isProcessed())
        copyAndAddSorted(event1);
      event1 = event1->getNext(clip1->getPool());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Blend or Reject only when events are overlapping. If not overlapping simply copy into the result.
void EventTrackDuration::blendOnOverlap(
  EventTrackDuration*  clip0,
  EventTrackDuration*  clip1,
  float                weight,                // Blend weighting.
  float                selectionWeightCutOff, // At what proportional weight do we select one events details over the other.
  uint32_t             blendingOperation,     // Operation to perform on events that have been matched up. See EVENT_BLEND_OP_TYPE_...
  bool                 rejectNonMatched)      // Discard un-matched events.
{
  EventDuration* event0;
  EventDuration* event1;
  EventDuration  blendedEvent;     // Working event which events will be blended into before copying into final track.
  float          numEventsInEventSpace;

  NMP_ASSERT(clip0 && clip1);
  NMP_ASSERT(clip0->getNumSyncEvents() == clip1->getNumSyncEvents());
  NMP_ASSERT(m_numEvents == 0);

  numEventsInEventSpace = (float) clip0->getNumSyncEvents();

  // Clear all processed flags on source arrays.
  event0 = clip0->getEventsListHead();
  clip0->setProcessedFlags(false, event0);
  event1 = clip1->getEventsListHead();
  clip1->setProcessedFlags(false, event1);

  // Search for all overlapping events, blend them together and store in the resulting event track.
  while (event0)
  {
    event1 = clip1->getEventsListHead();
    event1 = clip1->findOverlappingEvent(event0, event1, numEventsInEventSpace);
    while (event1)
    {
      // Set source events as having been processed.
      event0->setProcessed(true);
      event1->setProcessed(true);

      switch (blendingOperation)
      {
      case DURATION_EVENT_BLEND_OP_TYPE_BLEND: // Blend events and store in result.
        blendedEvent.blend(event0, event1, numEventsInEventSpace, weight);
        if (weight <= selectionWeightCutOff)
          blendedEvent.setUserData(event0->getUserData());
        else
          blendedEvent.setUserData(event1->getUserData());
        copyAndAddSorted(&blendedEvent);
        break;
      case DURATION_EVENT_BLEND_OP_TYPE_SELECT:  // Select event to store in result.
        if (weight <= selectionWeightCutOff)
          copyAndAddSorted(event0);
        else
          copyAndAddSorted(event1);
        break;
      default:
        NMP_DEBUG_MSG("Unknown event blending type requested");
        NMP_ASSERT_FAIL();
        break;
      }

      // Continue trying to find events in clip1 that overlap with this event from clip0.
      event1 = event1->getNext(clip1->getPool());
      event1 = clip1->findOverlappingEvent(event0, event1, numEventsInEventSpace);
    }
    event0 = event0->getNext(clip0->getPool());
  }

  // Deal with remaining events from both source tracks.
  if (!rejectNonMatched)
  {
    event0 = clip0->getEventsListHead();
    while (event0)
    {
      if (!event0->isProcessed())
        copyAndAddSorted(event0);
      event0 = event0->getNext(clip0->getPool());
    }

    event1 = clip1->getEventsListHead();
    while (event1)
    {
      if (!event1->isProcessed())
        copyAndAddSorted(event1);
      event1 = event1->getNext(clip1->getPool());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Blend or Reject only when events are within a specified event range of each other, otherwise put straight into the result.
void EventTrackDuration::blendWithinRange(
  EventTrackDuration* clip0,
  EventTrackDuration* clip1,
  float               weight,                // Blend weighting.
  float               selectionWeightCutOff, // At what proportional weight do we select one events details over the other.
  uint32_t            blendingOperation,     // Operation to perform on events that have been matched up. See EVENT_BLEND_OP_TYPE_...
  bool                rejectNonMatched,      // Discard un-matched events.
  float               testRange)             // Within what range do we decide to match events together. Expressed as an event fraction.
{
  EventDuration* event0;
  EventDuration* event1;
  EventDuration  blendedEvent;     // Working event which events will be blended into before copying into final track.
  float          numEventsInEventSpace;

  NMP_ASSERT(clip0 && clip1);
  NMP_ASSERT(clip0->getNumSyncEvents() == clip1->getNumSyncEvents());
  NMP_ASSERT(m_numEvents == 0);

  numEventsInEventSpace = (float) clip0->getNumSyncEvents();

  // Clear all processed flags on source arrays.
  event0 = clip0->getEventsListHead();
  clip0->setProcessedFlags(false, event0);
  event1 = clip1->getEventsListHead();
  clip1->setProcessedFlags(false, event1);

  // Search for all overlapping events, blend them together and store in the resulting event track.
  while (event0)
  {
    event1 = clip1->getEventsListHead();
    event1 = clip1->findEventWithinRange(event0, event1, testRange, numEventsInEventSpace);
    while (event1)
    {
      // Set source events as having been processed.
      event0->setProcessed(true);
      event1->setProcessed(true);

      switch (blendingOperation)
      {
      case DURATION_EVENT_BLEND_OP_TYPE_BLEND: // Blend events and store in result.
        blendedEvent.blend(event0, event1, numEventsInEventSpace, weight);
        if (weight <= selectionWeightCutOff)
          blendedEvent.setUserData(event0->getUserData());
        else
          blendedEvent.setUserData(event1->getUserData());
        copyAndAddSorted(&blendedEvent);
        break;
      case DURATION_EVENT_BLEND_OP_TYPE_SELECT:  // Select event to store in result.
        if (weight <= selectionWeightCutOff)
          copyAndAddSorted(event0);
        else
          copyAndAddSorted(event1);
        break;
      default:
        NMP_DEBUG_MSG("Unknown event blending type requested");
        NMP_ASSERT_FAIL();
        break;
      }

      // Continue trying to find events in clip1 that overlap with this event from clip0.
      event1 = event1->getNext(clip1->getPool());
      event1 = clip1->findEventWithinRange(event0, event1, testRange, numEventsInEventSpace);
    }
    event0 = event0->getNext(clip0->getPool());
  }

  // Deal with remaining events from both source tracks.
  if (!rejectNonMatched)
  {
    event0 = clip0->getEventsListHead();
    while (event0)
    {
      if (!event0->isProcessed())
        copyAndAddSorted(event0);
      event0 = event0->getNext(clip0->getPool());
    }

    event1 = clip1->getEventsListHead();
    while (event1)
    {
      if (!event1->isProcessed())
        copyAndAddSorted(event1);
      event1 = event1->getNext(clip1->getPool());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// In sequence and with the same IDs and if overlapping.
void EventTrackDuration::blendInSequenceSameUserDataOnOverlap(
  EventTrackDuration* clip0,
  EventTrackDuration* clip1,
  float               weight,                // Blend weighting.
  float               selectionWeightCutOff, // At what proportional weight do we select one events details over the other.
  uint32_t            blendingOperation,     // Operation to perform on events that have been matched up. See EVENT_BLEND_OP_TYPE_...
  bool                rejectNonMatched)      // Discard un-matched events.
{
  EventDuration* event0;
  EventDuration* event1;
  EventDuration  blendedEvent;     // Working event which events will be blended into before copying into final track.
  float          numEventsInEventSpace;

  NMP_ASSERT(clip0 && clip1);
  NMP_ASSERT(clip0->getNumSyncEvents() == clip1->getNumSyncEvents());
  NMP_ASSERT(m_numEvents == 0);

  numEventsInEventSpace = (float) clip0->getNumSyncEvents();

  event0 = clip0->getEventsListHead();
  event1 = clip1->getEventsListHead();
  while (event0 && event1)
  {
    if ((event0->getUserData() == event1->getUserData()) && (event0->overlapsWith(event1, numEventsInEventSpace)))
    {
      // Set source events as having been processed.
      event0->setProcessed(true);
      event1->setProcessed(true);

      switch (blendingOperation)
      {
      case DURATION_EVENT_BLEND_OP_TYPE_BLEND:     // Blend events and store in result.
        blendedEvent.blend(event0, event1, numEventsInEventSpace, weight);
        if (weight <= selectionWeightCutOff)
          blendedEvent.setUserData(event0->getUserData());
        else
          blendedEvent.setUserData(event1->getUserData());
        copyAndAddSorted(&blendedEvent);
        break;
      case DURATION_EVENT_BLEND_OP_TYPE_SELECT:  // Select event to store in result.
        if (weight <= selectionWeightCutOff)
          copyAndAddSorted(event0);
        else
          copyAndAddSorted(event1);
        break;
      default:
        NMP_DEBUG_MSG("Unknown event blending type requested");
        NMP_ASSERT_FAIL();
        break;
      }
    }
    event0 = event0->getNext(clip0->getPool());
    event1 = event1->getNext(clip1->getPool());
  }

  // Deal with remaining events from both source tracks.
  if (!rejectNonMatched)
  {
    event0 = clip0->getEventsListHead();
    while (event0)
    {
      if (!event0->isProcessed())
        copyAndAddSorted(event0);
      event0 = event0->getNext(clip0->getPool());
    }

    event1 = clip1->getEventsListHead();
    while (event1)
    {
      if (!event1->isProcessed())
        copyAndAddSorted(event1);
      event1 = event1->getNext(clip1->getPool());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// In sequence and with the same IDs and within a specified range.
void EventTrackDuration::blendInSequenceSameUserDataWithinRange(
  EventTrackDuration*  clip0,
  EventTrackDuration*  clip1,
  float                weight,                // Blend weighting.
  float                selectionWeightCutOff, // At what proportional weight do we select one events details over the other.
  uint32_t             blendingOperation,     // Operation to perform on events that have been matched up. See EVENT_BLEND_OP_TYPE_...
  bool                 rejectNonMatched,      // Discard un-matched events.
  float                testRange)             // Within what range do we decide to match events together. Expressed as an event fraction.
{
  EventDuration* event0;
  EventDuration* event1;
  EventDuration  blendedEvent;     // Working event which events will be blended into before copying into final track.
  float          numEventsInEventSpace;

  NMP_ASSERT(clip0 && clip1);
  NMP_ASSERT(clip0->getNumSyncEvents() == clip1->getNumSyncEvents());
  NMP_ASSERT(m_numEvents == 0);

  numEventsInEventSpace = (float) clip0->getNumSyncEvents();

  event0 = clip0->getEventsListHead();
  event1 = clip1->getEventsListHead();
  while (event0 && event1)
  {
    if ((event0->getUserData() == event1->getUserData()) && (event0->withinRange(event1, testRange, numEventsInEventSpace)))
    {
      // Set source events as having been processed.
      event0->setProcessed(true);
      event1->setProcessed(true);

      switch (blendingOperation)
      {
      case DURATION_EVENT_BLEND_OP_TYPE_BLEND:     // Blend events and store in result.
        blendedEvent.blend(event0, event1, numEventsInEventSpace, weight);
        if (weight <= selectionWeightCutOff)
          blendedEvent.setUserData(event0->getUserData());
        else
          blendedEvent.setUserData(event1->getUserData());
        copyAndAddSorted(&blendedEvent);
        break;
      case DURATION_EVENT_BLEND_OP_TYPE_SELECT:  // Select event to store in result.
        if (weight <= selectionWeightCutOff)
          copyAndAddSorted(event0);
        else
          copyAndAddSorted(event1);
        break;
      default:
        NMP_DEBUG_MSG("Unknown event blending type requested");
        NMP_ASSERT_FAIL();
        break;
      }
    }
    event0 = event0->getNext(clip0->getPool());
    event1 = event1->getNext(clip1->getPool());
  }

  // Deal with remaining events from both source tracks.
  if (!rejectNonMatched)
  {
    event0 = clip0->getEventsListHead();
    while (event0)
    {
      if (!event0->isProcessed())
        copyAndAddSorted(event0);
      event0 = event0->getNext(clip0->getPool());
    }

    event1 = clip1->getEventsListHead();
    while (event1)
    {
      if (!event1->isProcessed())
        copyAndAddSorted(event1);
      event1 = event1->getNext(clip1->getPool());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// In sequence and with the same IDs.
void EventTrackDuration::blendInSequenceSameUserData(
  EventTrackDuration* clip0,
  EventTrackDuration* clip1,
  float               weight,                // Blend weighting.
  float               selectionWeightCutOff, // At what proportional weight do we select one events details over the other.
  uint32_t            blendingOperation,     // Operation to perform on events that have been matched up. See EVENT_BLEND_OP_TYPE_...
  bool                rejectNonMatched)      // Discard un-matched events.
{
  EventDuration* event0;
  EventDuration* event1;
  EventDuration  blendedEvent;     // Working event which events will be blended into before copying into final track.
  float          numEventsInEventSpace;

  NMP_ASSERT(clip0 && clip1);
  NMP_ASSERT(clip0->getNumSyncEvents() == clip1->getNumSyncEvents());
  NMP_ASSERT(m_numEvents == 0);

  numEventsInEventSpace = (float) clip0->getNumSyncEvents();

  event0 = clip0->getEventsListHead();
  event1 = clip1->getEventsListHead();
  while (event0 && event1)
  {
    if (event0->getUserData() == event1->getUserData())
    {
      // Set source events as having been processed.
      event0->setProcessed(true);
      event1->setProcessed(true);

      switch (blendingOperation)
      {
      case DURATION_EVENT_BLEND_OP_TYPE_BLEND:     // Blend events and store in result.
        blendedEvent.blend(event0, event1, numEventsInEventSpace, weight);
        if (weight <= selectionWeightCutOff)
          blendedEvent.setUserData(event0->getUserData());
        else
          blendedEvent.setUserData(event1->getUserData());
        copyAndAddSorted(&blendedEvent);
        break;
      case DURATION_EVENT_BLEND_OP_TYPE_SELECT:  // Select event to store in result.
        if (weight <= selectionWeightCutOff)
          copyAndAddSorted(event0);
        else
          copyAndAddSorted(event1);
        break;
      default:
        NMP_DEBUG_MSG("Unknown event blending type requested");
        NMP_ASSERT_FAIL();
        break;
      }
    }
    event0 = event0->getNext(clip0->getPool());
    event1 = event1->getNext(clip1->getPool());
  }

  // Deal with remaining events from both source tracks.
  if (!rejectNonMatched)
  {
    event0 = clip0->getEventsListHead();
    while (event0)
    {
      if (!event0->isProcessed())
        copyAndAddSorted(event0);
      event0 = event0->getNext(clip0->getPool());
    }

    event1 = clip1->getEventsListHead();
    while (event1)
    {
      if (!event1->isProcessed())
        copyAndAddSorted(event1);
      event1 = event1->getNext(clip1->getPool());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// In sequence and if overlapping.
void EventTrackDuration::blendInSequenceOnOverlap(
  EventTrackDuration* clip0,
  EventTrackDuration* clip1,
  float               weight,                // Blend weighting.
  float               selectionWeightCutOff, // At what proportional weight do we select one events details over the other.
  uint32_t            blendingOperation,     // Operation to perform on events that have been matched up. See EVENT_BLEND_OP_TYPE_...
  bool                rejectNonMatched)      // Discard un-matched events.
{
  EventDuration* event0;
  EventDuration* event1;
  EventDuration  blendedEvent;     // Working event which events will be blended into before copying into final track.
  float          numEventsInEventSpace;

  NMP_ASSERT(clip0 && clip1);
  NMP_ASSERT(clip0->getNumSyncEvents() == clip1->getNumSyncEvents());
  NMP_ASSERT(m_numEvents == 0);

  numEventsInEventSpace = (float) clip0->getNumSyncEvents();

  event0 = clip0->getEventsListHead();
  event1 = clip1->getEventsListHead();
  while (event0 && event1)
  {
    if (event0->overlapsWith(event1, numEventsInEventSpace))
    {
      // Set source events as having been processed.
      event0->setProcessed(true);
      event1->setProcessed(true);

      switch (blendingOperation)
      {
      case DURATION_EVENT_BLEND_OP_TYPE_BLEND:     // Blend events and store in result.
        blendedEvent.blend(event0, event1, numEventsInEventSpace, weight);
        if (weight <= selectionWeightCutOff)
          blendedEvent.setUserData(event0->getUserData());
        else
          blendedEvent.setUserData(event1->getUserData());
        copyAndAddSorted(&blendedEvent);
        break;
      case DURATION_EVENT_BLEND_OP_TYPE_SELECT:  // Select event to store in result.
        if (weight <= selectionWeightCutOff)
          copyAndAddSorted(event0);
        else
          copyAndAddSorted(event1);
        break;
      default:
        NMP_DEBUG_MSG("Unknown event blending type requested");
        NMP_ASSERT_FAIL();
        break;
      }
    }
    event0 = event0->getNext(clip0->getPool());
    event1 = event1->getNext(clip1->getPool());
  }

  // Deal with remaining events from both source tracks.
  if (!rejectNonMatched)
  {
    event0 = clip0->getEventsListHead();
    while (event0)
    {
      if (!event0->isProcessed())
        copyAndAddSorted(event0);
      event0 = event0->getNext(clip0->getPool());
    }

    event1 = clip1->getEventsListHead();
    while (event1)
    {
      if (!event1->isProcessed())
        copyAndAddSorted(event1);
      event1 = event1->getNext(clip1->getPool());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// In sequence and within a specified range.
void EventTrackDuration::blendInSequenceWithinRange(
  EventTrackDuration* clip0,
  EventTrackDuration* clip1,
  float               weight,                // Blend weighting.
  float               selectionWeightCutOff, // At what proportional weight do we select one events details over the other.
  uint32_t            blendingOperation,     // Operation to perform on events that have been matched up. See EVENT_BLEND_OP_TYPE_...
  bool                rejectNonMatched,      // Discard un-matched events.
  float               testRange)             // Within what range do we decide to match events together. Expressed as an event fraction.
{
  EventDuration* event0;
  EventDuration* event1;
  EventDuration  blendedEvent;     // Working event which events will be blended into before copying into final track.
  float          numEventsInEventSpace;

  NMP_ASSERT(clip0 && clip1);
  NMP_ASSERT(clip0->getNumSyncEvents() == clip1->getNumSyncEvents());
  NMP_ASSERT(m_numEvents == 0);

  numEventsInEventSpace = (float) clip0->getNumSyncEvents();

  event0 = clip0->getEventsListHead();
  event1 = clip1->getEventsListHead();
  while (event0 && event1)
  {
    if (event0->withinRange(event1, testRange, numEventsInEventSpace))
    {
      // Set source events as having been processed.
      event0->setProcessed(true);
      event1->setProcessed(true);

      switch (blendingOperation)
      {
      case DURATION_EVENT_BLEND_OP_TYPE_BLEND:     // Blend events and store in result.
        blendedEvent.blend(event0, event1, numEventsInEventSpace, weight);
        if (weight <= selectionWeightCutOff)
          blendedEvent.setUserData(event0->getUserData());
        else
          blendedEvent.setUserData(event1->getUserData());
        copyAndAddSorted(&blendedEvent);
        break;
      case DURATION_EVENT_BLEND_OP_TYPE_SELECT:  // Select event to store in result.
        if (weight <= selectionWeightCutOff)
          copyAndAddSorted(event0);
        else
          copyAndAddSorted(event1);
        break;
      default:
        NMP_DEBUG_MSG("Unknown event blending type requested");
        NMP_ASSERT_FAIL();
        break;
      }
    }
    event0 = event0->getNext(clip0->getPool());
    event1 = event1->getNext(clip1->getPool());
  }

  // Deal with remaining events from both source tracks.
  if (!rejectNonMatched)
  {
    event0 = clip0->getEventsListHead();
    while (event0)
    {
      if (!event0->isProcessed())
        copyAndAddSorted(event0);
      event0 = event0->getNext(clip0->getPool());
    }

    event1 = clip1->getEventsListHead();
    while (event1)
    {
      if (!event1->isProcessed())
        copyAndAddSorted(event1);
      event1 = event1->getNext(clip1->getPool());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// With the same IDs and if overlapping.
void EventTrackDuration::blendSameUserDataOnOverlap(
  EventTrackDuration* clip0,
  EventTrackDuration* clip1,
  float               weight,                // Blend weighting.
  float               selectionWeightCutOff, // At what proportional weight do we select one events details over the other.
  uint32_t            blendingOperation,     // Operation to perform on events that have been matched up. See EVENT_BLEND_OP_TYPE_...
  bool                rejectNonMatched)      // Discard un-matched events.
{
  EventDuration* event0;
  EventDuration* event1;
  EventDuration  blendedEvent;     // Working event which events will be blended into before copying into final track.
  float          numEventsInEventSpace;

  NMP_ASSERT(clip0 && clip1);
  NMP_ASSERT(clip0->getNumSyncEvents() == clip1->getNumSyncEvents());
  NMP_ASSERT(m_numEvents == 0);

  numEventsInEventSpace = (float) clip0->getNumSyncEvents();

  // Clear all processed flags on source arrays.
  event0 = clip0->getEventsListHead();
  clip0->setProcessedFlags(false, event0);
  event1 = clip1->getEventsListHead();
  clip1->setProcessedFlags(false, event1);

  // Search for all overlapping events, blend them together and store in the resulting event track.
  while (event0)
  {
    event1 = clip1->getEventsListHead();
    event1 = clip1->findEventWithSameUserDataAndOverlapping(event0, event1, numEventsInEventSpace);
    while (event1)
    {
      // Set source events as having been processed.
      event0->setProcessed(true);
      event1->setProcessed(true);

      switch (blendingOperation)
      {
      case DURATION_EVENT_BLEND_OP_TYPE_BLEND: // Blend events and store in result.
        blendedEvent.blend(event0, event1, numEventsInEventSpace, weight);
        if (weight <= selectionWeightCutOff)
          blendedEvent.setUserData(event0->getUserData());
        else
          blendedEvent.setUserData(event1->getUserData());
        copyAndAddSorted(&blendedEvent);
        break;
      case DURATION_EVENT_BLEND_OP_TYPE_SELECT:  // Select event to store in result.
        if (weight <= selectionWeightCutOff)
          copyAndAddSorted(event0);
        else
          copyAndAddSorted(event1);
        break;
      default:
        NMP_DEBUG_MSG("Unknown event blending type requested");
        NMP_ASSERT_FAIL();
        break;
      }

      // Continue trying to find events in clip1 that have the same user data as the clip0 event.
      event1 = event1->getNext(clip1->getPool());
      event1 = clip1->findEventWithSameUserDataAndOverlapping(event0, event1, numEventsInEventSpace);
    }
    event0 = event0->getNext(clip0->getPool());
  }

  // Deal with remaining events from both source tracks.
  if (!rejectNonMatched)
  {
    event0 = clip0->getEventsListHead();
    while (event0)
    {
      if (!event0->isProcessed())
        copyAndAddSorted(event0);
      event0 = event0->getNext(clip0->getPool());
    }

    event1 = clip1->getEventsListHead();
    while (event1)
    {
      if (!event1->isProcessed())
        copyAndAddSorted(event1);
      event1 = event1->getNext(clip1->getPool());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// With the same IDs and within a specified range.
void EventTrackDuration::blendSameUserDataWithinRange(
  EventTrackDuration* clip0,
  EventTrackDuration* clip1,
  float               weight,                // Blend weighting.
  float               selectionWeightCutOff, // At what proportional weight do we select one events details over the other.
  uint32_t            blendingOperation,     // Operation to perform on events that have been matched up. See EVENT_BLEND_OP_TYPE_...
  bool                rejectNonMatched,      // Discard un-matched events.
  float               testRange)             // Within what range do we decide to match events together. Expressed as an event fraction.
{
  EventDuration* event0;
  EventDuration* event1;
  EventDuration  blendedEvent;     // Working event which events will be blended into before copying into final track.
  float          numEventsInEventSpace;

  NMP_ASSERT(clip0 && clip1);
  NMP_ASSERT(clip0->getNumSyncEvents() == clip1->getNumSyncEvents());
  NMP_ASSERT(m_numEvents == 0);

  numEventsInEventSpace = (float) clip0->getNumSyncEvents();

  // Clear all processed flags on source arrays.
  event0 = clip0->getEventsListHead();
  clip0->setProcessedFlags(false, event0);
  event1 = clip1->getEventsListHead();
  clip1->setProcessedFlags(false, event1);

  // Search for all overlapping events, blend them together and store in the resulting event track.
  while (event0)
  {
    event1 = clip1->getEventsListHead();
    event1 = clip1->findEventWithSameUserDataAndWithinRange(event0, event1, testRange, numEventsInEventSpace);
    while (event1)
    {
      // Set source events as having been processed.
      event0->setProcessed(true);
      event1->setProcessed(true);

      switch (blendingOperation)
      {
      case DURATION_EVENT_BLEND_OP_TYPE_BLEND: // Blend events and store in result.
        blendedEvent.blend(event0, event1, numEventsInEventSpace, weight);
        if (weight <= selectionWeightCutOff)
          blendedEvent.setUserData(event0->getUserData());
        else
          blendedEvent.setUserData(event1->getUserData());
        copyAndAddSorted(&blendedEvent);
        break;
      case DURATION_EVENT_BLEND_OP_TYPE_SELECT:  // Select event to store in result.
        if (weight <= selectionWeightCutOff)
          copyAndAddSorted(event0);
        else
          copyAndAddSorted(event1);
        break;
      default:
        NMP_DEBUG_MSG("Unknown event blending type requested");
        NMP_ASSERT_FAIL();
        break;
      }

      // Continue trying to find events in clip1 that have the same user data as the clip0 event.
      event1 = event1->getNext(clip1->getPool());
      event1 = clip1->findEventWithSameUserDataAndWithinRange(event0, event1, testRange, numEventsInEventSpace);
    }
    event0 = event0->getNext(clip0->getPool());
  }

  // Deal with remaining events from both source tracks.
  if (!rejectNonMatched)
  {
    event0 = clip0->getEventsListHead();
    while (event0)
    {
      if (!event0->isProcessed())
        copyAndAddSorted(event0);
      event0 = event0->getNext(clip0->getPool());
    }

    event1 = clip1->getEventsListHead();
    while (event1)
    {
      if (!event1->isProcessed())
        copyAndAddSorted(event1);
      event1 = event1->getNext(clip1->getPool());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// EventTrackDurationSet functions.
//----------------------------------------------------------------------------------------------------------------------
EventTrackDurationSet* EventTrackDurationSet::create(
  NMP::MemoryAllocator* allocator,
  uint32_t              numEventTracks,
  uint32_t              eventsPoolSize)
{
  EventTrackDurationSet* result;

  NMP::Memory::Format memReqs = getMemoryRequirements(numEventTracks, eventsPoolSize);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  result = init(resource, numEventTracks, eventsPoolSize);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format EventTrackDurationSet::getMemoryRequirements(
  uint32_t numEventTracks,
  uint32_t eventsPoolSize)
{
  NMP::Memory::Format result(sizeof(EventTrackDurationSet), NMP_NATURAL_TYPE_ALIGNMENT);
  for (uint32_t i = 0; i < numEventTracks; ++i)
    result += EventTrackDuration::getMemoryRequirements();

  result += EventDurationPool::getMemoryRequirements(eventsPoolSize);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
EventTrackDurationSet* EventTrackDurationSet::init(
  NMP::Memory::Resource& resource,
  uint32_t               numEventTracks,
  uint32_t               eventsPoolSize)
{
  NMP::Memory::Format format(sizeof(EventTrackDurationSet), NMP_NATURAL_TYPE_ALIGNMENT);

  resource.align(format);
  EventTrackDurationSet* result = (EventTrackDurationSet*)resource.ptr;
  resource.increment(format);

  result->m_tracks = (EventTrackDuration*) resource.ptr;
  for (uint32_t i = 0; i < numEventTracks; ++i)
    EventTrackDuration::init(resource);
  result->m_maxNumEventTracks = numEventTracks;
  result->m_numEventTracks = 0;

  result->m_eventsPool = EventDurationPool::init(resource, eventsPoolSize);

  for (uint32_t i = 0; i < result->m_maxNumEventTracks; ++i)
  {
    result->m_tracks[i].setPool(result->m_eventsPool);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackDurationSet::init(
  uint32_t            maxNumEventTracks, //
  EventTrackDuration* tracks,            //
  EventDurationPool*  eventsPool)        //
{
  m_numEventTracks = 0;
  m_tracks = tracks;
  m_maxNumEventTracks = maxNumEventTracks;
  m_eventsPool = eventsPool;

  for (uint32_t i = 0; i < m_numEventTracks; ++i)
  {
    m_tracks[i].setPool(m_eventsPool);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackDurationSet::relocate(void* location)
{
  NMP_ASSERT(NMP_IS_ALIGNED(this, NMP_NATURAL_TYPE_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(location, NMP_NATURAL_TYPE_ALIGNMENT));
  size_t offset = NMP::Memory::align(sizeof(EventTrackDurationSet), NMP_NATURAL_TYPE_ALIGNMENT);

  EventTrackDuration* tracks = (EventTrackDuration*)(((size_t)this) + offset);
  m_tracks = (EventTrackDuration*)(((size_t)location) + offset);
  offset += NMP::Memory::align(sizeof(EventTrackDuration) * m_maxNumEventTracks, NMP_NATURAL_TYPE_ALIGNMENT);

  EventDurationPool* eventsPool = (EventDurationPool*)(((size_t)this) + offset);
  m_eventsPool = (EventDurationPool*)(((size_t)location) + offset);
  eventsPool->relocate(m_eventsPool);

  for (uint32_t i = 0; i < m_numEventTracks; ++i)
  {
    tracks[i].setPool(m_eventsPool);
  }
}

//----------------------------------------------------------------------------------------------------------------------
EventTrackDurationSet* EventTrackDurationSet::copyAndPack(NMP::Memory::Resource& resource)
{
  NMP_ASSERT(NMP_IS_ALIGNED(resource.ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  EventTrackDurationSet* result = (EventTrackDurationSet*) resource.ptr;
  resource.increment(NMP::Memory::align(sizeof(EventTrackDurationSet), NMP_NATURAL_TYPE_ALIGNMENT));

  result->m_numEventTracks = m_numEventTracks;
  result->m_maxNumEventTracks = m_numEventTracks;
  result->m_tracks = (EventTrackDuration*) resource.ptr;
  resource.increment(NMP::Memory::align(sizeof(EventTrackDuration) * m_numEventTracks, NMP_NATURAL_TYPE_ALIGNMENT));

  result->m_eventsPool = (EventDurationPool*) resource.ptr;
  result->m_eventsPool->relocate();
  result->m_eventsPool->m_numAllocated = 0;
  result->m_eventsPool->m_size = m_eventsPool->m_numAllocated;

  for (uint32_t i = 0; i < m_numEventTracks; ++i)
  {
    result->m_tracks[i].setPool(result->m_eventsPool);
    m_tracks[i].copy(result->m_tracks[i]);
  }

  result->m_eventsPool->m_size = result->m_eventsPool->m_numAllocated;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackDurationSet::initFromSourceDef(
  float                   clipStartFraction,            // Where to start sampling from the source track.
  float                   clipDurationFraction,         // Length of the clip to sample.
  bool                    shouldLoop,                   // Is this to be a looping clip.
                                                        // Are events allowed to loop beyond the end of the sync event track.
  uint32_t                numSourceEventTrackDefs,      //
  EventTrackDefDuration** sourceDurationEventTrackDefs, // Event track defs to sample from.
  EventTrackSync*         sourceSyncTrack,              // Time stretch event space that these duration events are defined within.
  bool                    playBackwards)
{
  NMP_ASSERT(sourceSyncTrack);
  NMP_ASSERT(numSourceEventTrackDefs <= MAX_NUM_DURATION_EVENT_TRACKS_PER_SET);

  NMP_ASSERT(m_maxNumEventTracks == numSourceEventTrackDefs);
  m_numEventTracks = numSourceEventTrackDefs;
  for (uint32_t i = 0; i < numSourceEventTrackDefs; ++i)
  {
    uint32_t runtimeID = sourceDurationEventTrackDefs[i]->getTrackID();
    m_tracks[i].initFromSourceDef(
      clipStartFraction,
      clipDurationFraction,
      sourceDurationEventTrackDefs[i],
      runtimeID,
      sourceSyncTrack,
      m_eventsPool,
      shouldLoop,
      playBackwards);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// This must provide an estimate that will safely encompass the requirements of the blended EventTrackDurationSet.
void EventTrackDurationSet::estimateBlendRequirements(
  EventTrackDurationSet* inputSet0,
  EventTrackDurationSet* inputSet1,
  uint32_t               destSyncTrackEventCount,
  uint32_t&              maxNumTracks,
  uint32_t&              maxNumEvents)
{
  maxNumTracks = inputSet0->m_numEventTracks +  inputSet1->m_numEventTracks;
  maxNumEvents = 0;

  // /MORPH-21366: remove the * 2 multiply which only exists to allow space for a working track in the blend.
  for (uint32_t i = 0; i < inputSet0->m_numEventTracks; ++i)
  {
    uint32_t multiplier = NMP::maximum((destSyncTrackEventCount / inputSet0->getTrack(i)->getNumSyncEvents()) + 1, uint32_t(1)) * 2;
    maxNumEvents += (inputSet0->getTrack(i)->getNumEvents() * multiplier);
  }

  for (uint32_t i = 0; i < inputSet1->m_numEventTracks; ++i)
  {
    uint32_t multiplier = NMP::maximum((destSyncTrackEventCount / inputSet1->getTrack(i)->getNumSyncEvents()) + 1, uint32_t(1)) * 2;
    maxNumEvents += (inputSet1->getTrack(i)->getNumEvents() * multiplier);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackDurationSet::blend(
  EventTrackDurationSet* inputSet0,
  EventTrackDurationSet* inputSet1,
  EventTrackSync*        syncTrack,             // Time synchronisation event space within which blended events must be defined.
  float                  weight,                // Blend weighting.
  float                  selectionWeightCutOff, // At what proportional weight do we select one events details over the other.
                                                // Below this value select clip0, above it select clip1.
  uint32_t               matchingOperation,     // Whether and how to match up the events from each track.
                                                // See DURATION_EVENT_MATCH_...
  bool                   rejectNonMatched,      // If an event from one of the sources has not been matched to one
                                                // in the other source should we just discard it?
  uint32_t               blendOperation,        // Operation to perform on events that have been matched up.
                                                // See DURATION_EVENT_BLEND_OP_TYPE_...
  bool                   loopingAllowed,        // Are events allowed to loop beyond the end of the sync event track.
  float                  testRange,             // Within what range do we decide to match events together.
                                                // When event matching WITHIN_RANGE.
                                                // Expressed as an event fraction.
  uint32_t               inputSet1Offset)       // Offset of inputSet1 events for blending.
{
  uint32_t            i;
  uint32_t            k;
  EventTrackDuration* inputTrack0;
  EventTrackDuration* inputTrack1;
  bool                processed[MAX_NUM_DURATION_EVENT_TRACKS_PER_SET];

  NMP_ASSERT(m_numEventTracks == 0);
  NMP_ASSERT(inputSet0 && inputSet1 && syncTrack);
  NMP_ASSERT((inputSet0->m_numEventTracks < MAX_NUM_DURATION_EVENT_TRACKS_PER_SET) &&
             (inputSet1->m_numEventTracks < MAX_NUM_DURATION_EVENT_TRACKS_PER_SET) &&
             (m_maxNumEventTracks < MAX_NUM_DURATION_EVENT_TRACKS_PER_SET));

  //------------------------------------
  // Loop through all source0 tracks trying to find tracks to blend with from source1.
  for (i = 0; i < inputSet1->m_numEventTracks; ++i)
    processed[i] = false;

  for (i = 0; i < inputSet0->m_numEventTracks; ++i)
  {
    inputTrack0 = &(inputSet0->m_tracks[i]);

    // Find a track with matching runtimeID and userData from source1.
    inputTrack1 = NULL;
    for (k = 0; k < inputSet1->m_numEventTracks; ++k)
    {
      if ((inputSet0->m_tracks[i].getUserData() == inputSet1->m_tracks[k].getUserData()) &&
          (inputSet0->m_tracks[i].getRuntimeID() == inputSet1->m_tracks[k].getRuntimeID()))
      {
        inputTrack1 = &(inputSet1->m_tracks[k]);
        processed[k] = true;
        break;
      }
    }

    if (inputTrack1)
    {
      // 2 tracks to blend together.
      NMP_ASSERT(m_numEventTracks < MAX_NUM_DURATION_EVENT_TRACKS_PER_SET);
      m_tracks[m_numEventTracks].blend(
        inputTrack0,
        inputTrack1,
        weight,
        selectionWeightCutOff,
        matchingOperation,
        rejectNonMatched,
        blendOperation,
        m_eventsPool,
        loopingAllowed,
        testRange,
        inputSet1Offset);
      NMP_ASSERT(m_tracks[m_numEventTracks].getNumSyncEvents() == 
                 NMP::maximum(inputTrack0->getNumSyncEvents(), inputTrack1->getNumSyncEvents()));
      m_numEventTracks++;
    }
    else
    {
      // Track0 with no matching Track1.
      NMP_ASSERT(m_numEventTracks < MAX_NUM_DURATION_EVENT_TRACKS_PER_SET);
      if (m_tracks[m_numEventTracks].copyRepeat(
            inputTrack0,
            syncTrack->getNumEvents(),
            m_eventsPool,
            loopingAllowed,
            inputSet1Offset))
        m_numEventTracks++;
    }
  }

  //------------------------------------
  // Deal with un-matched tracks from Set1
  for (i = 0; i < inputSet1->m_numEventTracks; ++i)
  {
    if (!processed[i])
    {
      // Track1 with no matching Track0.
      NMP_ASSERT(m_numEventTracks < MAX_NUM_DURATION_EVENT_TRACKS_PER_SET);
      if (m_tracks[m_numEventTracks].copyRepeat(
            &(inputSet1->m_tracks[i]),
            syncTrack->getNumEvents(),
            m_eventsPool,
            loopingAllowed,
            inputSet1Offset))
        m_numEventTracks++;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackDurationSet::sample(
  SampledCurveEventsBuffer* curveSamplesBuffer,  // Curve buffer to add samples too.
  TimeFraction              sampleTime,          // As a fraction of the playback duration.
  TimeFraction              prevSampleTime,      // As a fraction of the playback duration.
  EventTrackSync*           syncTrack,           // Time synchronisation event space within which this set is defined.
  bool                      loopingAllowed)      // Looping animation
{
  NMP_ASSERT(curveSamplesBuffer && syncTrack);
  NMP_ASSERT(sampleTime >= 0.0f && sampleTime <= 1.0f);

  // Work out where in event space this sample time is.
  SyncEventPos sampleEvent(syncTrack->getAdjPosInfoFromAdjFraction(sampleTime));
  SyncEventPos prevSampleEvent(syncTrack->getAdjPosInfoFromAdjFraction(prevSampleTime));

  sample(curveSamplesBuffer, &sampleEvent, &prevSampleEvent, syncTrack, loopingAllowed);
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackDurationSet::sample(
  SampledCurveEventsBuffer* curveSamplesBuffer,  // Curve buffer to add samples too.
  SyncEventPos*             syncEventPos,        // Time synchronisation event position to sample at.
  SyncEventPos*             prevSyncEventPos,    // Prev time synchronisation event position sampled.
  EventTrackSync*           syncEventSeq,        // Time stretch event sequence.
  bool                      loopingAllowed)      // Looping animation
{
  uint32_t  i;

  NMP_ASSERT(curveSamplesBuffer && syncEventPos);

  // Sample each track in turn.
  for (i = 0; i < m_numEventTracks; ++i)
  {
    m_tracks[i].sample(curveSamplesBuffer, syncEventPos, prevSyncEventPos, syncEventSeq, loopingAllowed);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// EventDurationPool functions.
//----------------------------------------------------------------------------------------------------------------------
EventDurationPool* EventDurationPool::create(NMP::MemoryAllocator* allocator, uint32_t numEvents)
{
  EventDurationPool* result;

  NMP::Memory::Format memReqs = getMemoryRequirements(numEvents);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  result = init(resource, numEvents);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format EventDurationPool::getMemoryRequirements(uint32_t numEvents)
{
  NMP::Memory::Format result(sizeof(EventDurationPool), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Format eventsReqs = EventDuration::getMemoryRequirements();
  eventsReqs.size = NMP::Memory::align(eventsReqs.size * numEvents, eventsReqs.alignment);
  result += eventsReqs;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
EventDurationPool* EventDurationPool::init(NMP::Memory::Resource& resource, uint32_t numEvents)
{
  NMP::Memory::Format format(sizeof(EventDurationPool), NMP_NATURAL_TYPE_ALIGNMENT);

  resource.align(format);
  EventDurationPool* result = (EventDurationPool*)resource.ptr;
  resource.increment(format);

  format = EventDuration::getMemoryRequirements();
  format.size = NMP::Memory::align(format.size * numEvents, format.alignment);

  resource.align(format);
  result->m_eventsPool = (EventDuration*) resource.ptr; ;
  resource.increment(format);

  result->m_size = numEvents;
  result->m_numAllocated = 0;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void EventDurationPool::init(uint32_t size, EventDuration* eventsArray)
{
  m_numAllocated = 0;
  m_size = size;
  m_eventsPool = eventsArray;
}

//----------------------------------------------------------------------------------------------------------------------
void EventDurationPool::relocate(void* location)
{
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));

  ptr = (void*)(((size_t)ptr) + NMP::Memory::align(sizeof(EventDurationPool), NMP_NATURAL_TYPE_ALIGNMENT));
  m_eventsPool = (EventDuration*)ptr;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
bool locateEventTrackDefDuration(uint32_t NMP_USED_FOR_ASSERTS(assetType), void* assetMemory)
{
  NMP_ASSERT(assetType == MR::Manager::kAsset_EventTrackDuration);
  MR::EventTrackDefDuration* eventTrack = (MR::EventTrackDefDuration*)assetMemory;
  return eventTrack->locate();
}
#endif

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
