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
#include "morpheme/mrEventTrackDiscrete.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/mrManager.h"
//----------------------------s------------------------------------------------------------------------------------------

namespace MR
{

/// Durations of the events in a discrete track must sum closely to 1.0f.
const float gMaxDurationSumErrorLimit = 0.00000001f;

//----------------------------------------------------------------------------------------------------------------------
// EventTrackDefDiscrete functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
// This function is only ever called via EventTrackDefBase::locate, so we do not need to deal with endian swapping base
// class members.
bool EventTrackDefDiscrete::locate()
{
  EventTrackDefBase::locate();

  NMP::endianSwap(m_events);
  REFIX_PTR_RELATIVE(EventDefDiscrete, m_events, this);

  for (uint32_t i = 0; i < m_numEvents; ++i)
    m_events[i].locate();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// This function is only ever called via EventTrackDefBase::dislocate, so we do not need to deal with endian swapping
// base class members.
bool EventTrackDefDiscrete::dislocate()
{
  for (uint32_t i = 0; i < m_numEvents; ++i)
    m_events[i].dislocate();

  UNFIX_PTR_RELATIVE(EventDefDiscrete, m_events, this);
  NMP::endianSwap(m_events);

  EventTrackDefBase::dislocate();

  return true;
}
#endif // NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void EventTrackDefDiscrete::relocate()
{
  void* ptr = this;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));
  ptr = (void*)(((size_t)ptr) + sizeof(EventTrackDefDiscrete));

  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  m_events = (EventDefDiscrete*)ptr;
  ptr = (void*)(((size_t)ptr) + (sizeof(MR::EventDefDiscrete) * m_numEvents));

  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  m_name = (char*)ptr;
}

//----------------------------------------------------------------------------------------------------------------------
uint8_t EventTrackDefDiscrete::findEventIndexForTimeFraction(TimeFraction timeFraction) const
{
  NMP_ASSERT(m_events && timeFraction >= 0.0f && timeFraction <= 1.0f);
  NMP_ASSERT(m_numEvents < 0xFF);
  for (uint8_t index = 0; index < m_numEvents; index++)
  {
    if (timeFraction <= (m_events[index].getStartTime() + m_events[index].getDuration()))
      return index;
  }

  NMP_ASSERT_FAIL();
  return 0xFF;
}

//----------------------------------------------------------------------------------------------------------------------
bool EventTrackDefDiscrete::validate() const
{
  TimeFraction  sum = 0.0f;

  NMP_ASSERT(m_events && m_numEvents);
  for (uint32_t index = 0; index < m_numEvents; index++)
  {
    NMP_ASSERT(m_events[index].getDuration() > 0.0f);
    sum += m_events[index].getDuration();
  }

  if (NMP::nmfabs(sum - 1.0f) < gMaxDurationSumErrorLimit)
    return true;

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void EventTrackDefDiscrete::addToFoundEventsArray(
  EventDefDiscrete*  eventToAdd, 
  EventDefDiscrete** foundEvents,            // Passed in to be filled.
  uint32_t           foundEventsArraySize,   // Indicates foundEvents array size.
  uint32_t&          addedEventsCount) const // Incremented if add is successful.
{
  // Add to array of found events.
  if (addedEventsCount < foundEventsArraySize)
  {
    foundEvents[addedEventsCount] = eventToAdd;
    addedEventsCount++;
  }
  else
  {
    // Cannot continue adding events as we have exceeding the available buffer array space. Try incrementing max buffer size: gMaxNumFoundEvents.
    // MORPH-21364: Display this message in connect.
    NET_LOG_ERROR_MESSAGE("ERROR: Buffer overrun when sampling from Discrete Event Track: %s\n", m_name);
    NET_LOG_ERROR_MESSAGE("ERROR: This may happen with looping very small anim clip lengths. Or with many events located close together.\n");
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t EventTrackDefDiscrete::sampleEventsBackwards(
  float              timeFrom,             // What point to start sampling from.
  float              sampleDuration,       // How long to sample for.
  bool               allowWrapping,        // If true the track will wrap around
                                           // over the start or end dependent on whether
                                           // it is playing backwards or forwards.
  EventDefDiscrete** foundEvents,          // Passed in to be filled.
  uint32_t           foundEventsArraySize, // Indicates foundEvents array size.
  float              rangeStart,           // The animation clip range start.
  float              rangeEnd,             // The animation clip range end.
  bool               inclusiveStart) const // Whether the start of the range should be included or not.
{
  float eventTime;
  uint32_t addedEventsCount = 0; // Number of events found.
  float timeTo = timeFrom - sampleDuration;
  
  if (timeTo <= rangeStart)
  {
    // Handle sample periods that wrap off the start of the allowed range.
    // Do this by testing both the section at the start of the range and the wrapped section at the end of the range.
    if (allowWrapping)
    {
      timeTo = rangeEnd - (rangeStart - timeTo);
      NMP_ASSERT(timeTo >= rangeStart && timeTo <= rangeEnd);
            
      for (int32_t i = 0; i < ((int32_t)m_numEvents); ++i)
      {
        eventTime = m_events[i].getStartTime();
        if ((eventTime < timeFrom && eventTime >= rangeStart) ||
            (eventTime <= rangeEnd && eventTime >= timeTo) ||
            (inclusiveStart && eventTime == timeFrom))
        {
          addToFoundEventsArray(&(m_events[i]), foundEvents, foundEventsArraySize, addedEventsCount);
        }
      }
      return addedEventsCount;
    }

    // Not a looping anim so cap the sample period to the range start.
    timeTo = rangeStart;
  }
  
  // Handle sample periods that lie withing the allowed range.
  for (int32_t i = 0; i < ((int32_t)m_numEvents); ++i)
  {
    eventTime = m_events[i].getStartTime();
    if ((eventTime < timeFrom && eventTime >= timeTo) || (inclusiveStart && eventTime == timeFrom))
    {
      addToFoundEventsArray(&(m_events[i]), foundEvents, foundEventsArraySize, addedEventsCount);
    }
  }

  return addedEventsCount;
}
                      
//----------------------------------------------------------------------------------------------------------------------
uint32_t EventTrackDefDiscrete::sampleEventsForwards(
  float              timeFrom,             // What point to start sampling from.
  float              sampleDuration,       // How long to sample for.
  bool               allowWrapping,        // If true the track will wrap around
                                           // over the start or end dependent on whether
                                           // it is playing backwards or forwards.
  EventDefDiscrete** foundEvents,          // Passed in to be filled.
  uint32_t           foundEventsArraySize, // Indicates foundEvents array size.
  float              rangeStart,           // The animation clip range start.
  float              rangeEnd,             // The animation clip range end.
  bool               inclusiveStart) const // Whether the start of the range should be included or not.
{
  float eventTime;
  uint32_t addedEventsCount = 0; // Number of events found.
  float timeTo = timeFrom + sampleDuration;
  
  if (timeTo >= rangeEnd)
  {
    // Handle sample periods that wrap off the end of the allowed range.
    // Do this by testing both the section at the end of the range and the wrapped section at the start of the range.
    if (allowWrapping)
    {
      timeTo = rangeStart + (timeTo - rangeEnd);
      timeTo = NMP::minimum(timeTo, timeFrom); // Do not allow the sample period to wrap over more than 1 complete range interval.
      NMP_ASSERT((timeTo >= rangeStart) && (timeTo <= rangeEnd));
            
      for (int32_t i = 0; i < ((int32_t)m_numEvents); ++i)
      {
        eventTime = m_events[i].getStartTime();
        if ((eventTime > timeFrom && eventTime <= rangeEnd) ||
            (eventTime >= rangeStart && eventTime <= timeTo)||
            (inclusiveStart && eventTime == timeFrom))
        {
          addToFoundEventsArray(&(m_events[i]), foundEvents, foundEventsArraySize, addedEventsCount);
        }
      }
      return addedEventsCount;
    }

    // Not a looping anim so cap the sample period to the range end.
    timeTo = rangeEnd;
  }
  
  // Handle sample periods that lie withing the allowed range.
  for (int32_t i = 0; i < ((int32_t)m_numEvents); ++i)
  {
    eventTime = m_events[i].getStartTime();
    if ((eventTime > timeFrom && eventTime <= timeTo) || (inclusiveStart && eventTime == timeFrom))
    {
      addToFoundEventsArray(&(m_events[i]), foundEvents, foundEventsArraySize, addedEventsCount);
    }
  }

  return addedEventsCount;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t EventTrackDefDiscrete::findEventsBetweenTimes(
  float              timeFrom,             // What point to start sampling from.
  bool               sampleBackwards,      // Track forwards or backwards in the EventTrack.
  float              sampleDuration,       // How long to sample for.
  bool               allowWrapping,        // If true the track will wrap around
                                           // over the start or end dependent on whether
                                           // it is playing backwards or forwards.
  EventDefDiscrete** foundEvents,          // Passed in to be filled.
  uint32_t           foundEventsArraySize, // Indicates foundEvents array size.
  float              rangeStart,           // The animation clip range start.
  float              rangeEnd,             // The animation clip range end.
  bool               inclusiveStart) const // Whether the start of the range should be included or not.
{
  NMP_ASSERT(rangeEnd >= rangeStart);
  NMP_ASSERT(timeFrom >= rangeStart && timeFrom <= rangeEnd);
  NMP_ASSERT(rangeStart >= 0.0f && rangeEnd <= 1.0f);
 
  // Handle zero duration sample periods.
  // MORPH-21365: Stop repeat sampling of the same location, for example when a non looping anim has reached its end.
  if (sampleDuration == 0.0f)
  {
    float eventTime;
    uint32_t addedEventsCount = 0;
    for (int32_t i = 0; i < ((int32_t)m_numEvents); ++i)
    {
      eventTime = m_events[i].getStartTime();
      if (eventTime == timeFrom)
      {
        addToFoundEventsArray(&(m_events[i]), foundEvents, foundEventsArraySize, addedEventsCount);
      }
    }
    return addedEventsCount;
  }

  // Sampling backwards.
  if (sampleBackwards)
    return sampleEventsBackwards(timeFrom, sampleDuration, allowWrapping, foundEvents, foundEventsArraySize, rangeStart, rangeEnd, inclusiveStart);
  
  // Sampling forwards.
  return sampleEventsForwards(timeFrom, sampleDuration, allowWrapping, foundEvents, foundEventsArraySize, rangeStart, rangeEnd, inclusiveStart);
}

//----------------------------------------------------------------------------------------------------------------------
// TriggeredDiscreteEvent functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TriggeredDiscreteEvent::getMemoryRequirements()
{
  return NMP::Memory::Format(sizeof(TriggeredDiscreteEvent), NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
// TriggeredDiscreteEventsBuffer functions.
//----------------------------------------------------------------------------------------------------------------------
void TriggeredDiscreteEventsBuffer::locate()
{
  NMP::endianSwap(m_numEvents);
  REFIX_SWAP_PTR(TriggeredDiscreteEvent, m_triggeredEvents);
  for (uint32_t i = 0; i < m_numEvents; ++i)
    m_triggeredEvents->locate();
}

//----------------------------------------------------------------------------------------------------------------------
void TriggeredDiscreteEventsBuffer::dislocate()
{
  for (uint32_t i = 0; i < m_numEvents; ++i)
    m_triggeredEvents->dislocate();

  UNFIX_SWAP_PTR(TriggeredDiscreteEvent, m_triggeredEvents);
  NMP::endianSwap(m_numEvents);
}

//----------------------------------------------------------------------------------------------------------------------
void TriggeredDiscreteEventsBuffer::relocate(void* location)
{
  if (m_numEvents > 0)
  {
    void* ptr = location;
    ptr = (void*)(((size_t)ptr) + NMP::Memory::align(sizeof(TriggeredDiscreteEventsBuffer), NMP_NATURAL_TYPE_ALIGNMENT));
    m_triggeredEvents = (TriggeredDiscreteEvent*) ptr;
  }
  else
  {
    m_triggeredEvents = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
TriggeredDiscreteEventsBuffer* TriggeredDiscreteEventsBuffer::create(NMP::MemoryAllocator* allocator, uint32_t numEvents)
{
  TriggeredDiscreteEventsBuffer* result;

  NMP::Memory::Format memReqs = getMemoryRequirements(numEvents);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  result = init(resource, numEvents);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TriggeredDiscreteEventsBuffer::getMemoryRequirements(uint32_t numEvents)
{
  NMP::Memory::Format result(sizeof(TriggeredDiscreteEventsBuffer), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Format eventsReqs = TriggeredDiscreteEvent::getMemoryRequirements();
  eventsReqs.size = NMP::Memory::align(eventsReqs.size * numEvents, eventsReqs.alignment);
  result += eventsReqs;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
TriggeredDiscreteEventsBuffer* TriggeredDiscreteEventsBuffer::init(NMP::Memory::Resource& resource, uint32_t numEvents)
{
  NMP::Memory::Format format(sizeof(TriggeredDiscreteEventsBuffer), NMP_NATURAL_TYPE_ALIGNMENT);

  resource.align(format);
  TriggeredDiscreteEventsBuffer* result = (TriggeredDiscreteEventsBuffer*)resource.ptr;
  resource.increment(format);

  result->m_numEvents = numEvents;

  if (numEvents > 0)
  {
    format = TriggeredDiscreteEvent::getMemoryRequirements();
    format.size = NMP::Memory::align(format.size * numEvents, format.alignment);

    resource.align(format);
    result->m_triggeredEvents = (TriggeredDiscreteEvent*) resource.ptr;
    resource.increment(format);
  }
  else
  {
    result->m_triggeredEvents = NULL;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t TriggeredDiscreteEventsBuffer::findIndexOfEventWithSourceEventUserData(
  uint32_t startingIndex,
  uint32_t eventUserData) const
{
  uint32_t i;

  i = startingIndex;
  while (i < m_numEvents)
  {
    NMP_ASSERT(m_triggeredEvents);
    if (m_triggeredEvents[i].getSourceEventUserData() == eventUserData)
      return i;
    ++i;
  }

  return INVALID_EVENT_INDEX;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t TriggeredDiscreteEventsBuffer::findIndexOfEventWithSourceEventTrackUserData(
  uint32_t startingIndex,
  uint32_t trackUserData) const
{
  uint32_t i;

  i = startingIndex;
  while (i < m_numEvents)
  {
    NMP_ASSERT(m_triggeredEvents);
    if (m_triggeredEvents[i].getSourceTrackUserData() == trackUserData)
      return i;
    ++i;
  }

  return INVALID_EVENT_INDEX;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t TriggeredDiscreteEventsBuffer::findIndexOfEventWithSourceEventTrackRuntimeID(
  uint32_t startingIndex,
  uint32_t trackRuntimeID) const
{
  uint32_t i;

  i = startingIndex;
  while (i < m_numEvents)
  {
    NMP_ASSERT(m_triggeredEvents);
    if (m_triggeredEvents[i].getSourceTrackRuntimeID() == trackRuntimeID)
      return i;
    ++i;
  }

  return INVALID_EVENT_INDEX;
}

//----------------------------------------------------------------------------------------------------------------------
void TriggeredDiscreteEventsBuffer::combine(
  const TriggeredDiscreteEventsBuffer* input0,
  const TriggeredDiscreteEventsBuffer* input1,
  float                                weight)
{
  NMP_ASSERT(input0 && input1);
  NMP_ASSERT(input0->getNumTriggeredEvents() + input1->getNumTriggeredEvents() <= m_numEvents);
  uint32_t count = 0;
  for (uint32_t i = 0; i < input0->getNumTriggeredEvents(); ++i)
  {
    m_triggeredEvents[count] = *(input0->getTriggeredEvent(i));
    m_triggeredEvents[count].scaleBlendWeight(weight);
    ++count;
  }
  for (uint32_t i = 0; i < input1->getNumTriggeredEvents(); ++i)
  {
    m_triggeredEvents[count] = *(input1->getTriggeredEvent(i));
    m_triggeredEvents[count].scaleBlendWeight(1.0f - weight);
    ++count;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TriggeredDiscreteEventsBuffer::additiveCombine(
  const TriggeredDiscreteEventsBuffer* input0,
  const TriggeredDiscreteEventsBuffer* input1,
  float                                weight)
{
  NMP_ASSERT(input0 && input1);
  NMP_ASSERT(input0->getNumTriggeredEvents() + input1->getNumTriggeredEvents() <= m_numEvents);
  uint32_t count = 0;
  for (uint32_t i = 0; i < input0->getNumTriggeredEvents(); ++i)
  {
    m_triggeredEvents[count] = *(input0->getTriggeredEvent(i));
    ++count;
  }
  for (uint32_t i = 0; i < input1->getNumTriggeredEvents(); ++i)
  {
    m_triggeredEvents[count] = *(input1->getTriggeredEvent(i));
    m_triggeredEvents[count].scaleBlendWeight(weight);
    ++count;
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t TriggeredDiscreteEventsBuffer::fillAt(
  uint32_t startingIndex,
  const TriggeredDiscreteEventsBuffer* input)
{
  NMP_ASSERT(input);
  NMP_ASSERT(startingIndex + input->getNumTriggeredEvents() <= m_numEvents);

  uint32_t count = startingIndex;

  for (uint32_t i = 0; i < input->getNumTriggeredEvents(); ++i)
  {
    m_triggeredEvents[count] = *(input->getTriggeredEvent(i));
    ++count;
  }

  return count;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t TriggeredDiscreteEventsBuffer::fillAtAndScale(
  uint32_t    startingIndex,
  const TriggeredDiscreteEventsBuffer* input,
  const float scale)
{
  NMP_ASSERT(input);
  NMP_ASSERT(startingIndex + input->getNumTriggeredEvents() <= m_numEvents);

  uint32_t count = startingIndex;

  for (uint32_t i = 0; i < input->getNumTriggeredEvents(); ++i)
  {
    m_triggeredEvents[count] = *(input->getTriggeredEvent(i));
    m_triggeredEvents[count].scaleBlendWeight(scale);
    ++count;
  }

  return count;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
bool locateEventTrackDefDiscrete(uint32_t NMP_USED_FOR_ASSERTS(assetType), void* assetMemory)
{
  NMP_ASSERT(assetType == MR::Manager::kAsset_EventTrackDiscrete);
  MR::EventTrackDefDiscrete* eventTrack = (MR::EventTrackDefDiscrete*)assetMemory;
  return eventTrack->locate();
}
#endif

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
