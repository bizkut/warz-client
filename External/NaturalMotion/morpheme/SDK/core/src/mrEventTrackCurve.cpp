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
#include "morpheme/mrEventTrackCurve.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/mrManager.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// EventTrackDefCurve functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
bool EventTrackDefCurve::locate()
{
  EventTrackDefBase::locate();

  NMP::endianSwap(m_events);
  REFIX_PTR_RELATIVE(EventDefCurve, m_events, this);

  for (uint32_t i = 0; i < m_numEvents; ++i)
    m_events[i].locate();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool EventTrackDefCurve::dislocate()
{
  for (uint32_t i = 0; i < m_numEvents; ++i)
    m_events[i].dislocate();

  UNFIX_PTR_RELATIVE(EventDefCurve, m_events, this);
  NMP::endianSwap(m_events);

  EventTrackDefBase::dislocate();

  return true;
}
#endif // NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void EventTrackDefCurve::relocate()
{
  void* ptr = this;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));
  ptr = (void*)(((size_t)ptr) + sizeof(EventTrackDefCurve));

  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  m_events = (EventDefCurve*)ptr;
  ptr = (void*)(((size_t)ptr) + (sizeof(MR::EventDefCurve) * m_numEvents));

  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  m_name = (char*)ptr;
}

//----------------------------------------------------------------------------------------------------------------------
float EventTrackDefCurve::getValueAtTime(float time) const
{
  float    prevEventValue = 0.0f;
  float    nextEventValue = 0.0f;
  float    prevEventTime = 0.0f;
  float    nextEventTime = 1.0f;
  float    result;
  float    interpolant = 1.0f;

  NMP_ASSERT(time >= 0.0f && time <= 1.0f);

  // Find the 2 values we lie between in the track.
  for (uint32_t i = 0; i < m_numEvents; ++i)
  {
    if (time <= m_events[i].getTime())
    {
      nextEventValue = m_events[i].getValue();
      nextEventTime = m_events[i].getTime();
      break;
    }
    prevEventValue = m_events[i].getValue();
    prevEventTime = m_events[i].getTime();
  }

  // Interpolate results.
  if (nextEventTime > prevEventTime)
  {
    NMP_ASSERT(nextEventTime - prevEventTime > 0.0f);
    interpolant = (time - prevEventTime) / (nextEventTime - prevEventTime);
  }
  result = prevEventValue + (interpolant * (nextEventValue - prevEventValue));

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const EventDefCurve* EventTrackDefCurve::getEventAtTime(float time) const
{
  const EventDefCurve* tmpEvent = 0;

  // Find the event which will be used for interpolation.
  for (uint32_t i = 0; i < m_numEvents; ++i)
  {
    tmpEvent = &m_events[i];
    if (time <= tmpEvent->getTime())
    {
      break;
    }
  }
  return tmpEvent;
}

//----------------------------------------------------------------------------------------------------------------------
// TriggeredDiscreteEvent functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SampledCurveEvent::getMemoryRequirements()
{
  return NMP::Memory::Format(sizeof(SampledCurveEvent), NMP_NATURAL_TYPE_ALIGNMENT);
}

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
// SampledCurveEventsBuffer functions.
//----------------------------------------------------------------------------------------------------------------------
void SampledCurveEventsBuffer::locate()
{
  NMP::endianSwap(m_sampleBufferSize);
  REFIX_SWAP_PTR(SampledCurveEvent, m_eventSamples);
  for (uint32_t i = 0; i < m_sampleBufferSize; ++i)
    m_eventSamples[i].locate();
}

//----------------------------------------------------------------------------------------------------------------------
void SampledCurveEventsBuffer::dislocate()
{
  for (uint32_t i = 0; i < m_sampleBufferSize; ++i)
    m_eventSamples[i].dislocate();

  UNFIX_SWAP_PTR(SampledCurveEvent, m_eventSamples);
  NMP::endianSwap(m_sampleBufferSize);
}
#endif // NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void SampledCurveEventsBuffer::relocate(void* location)
{
  if (m_sampleBufferSize > 0)
  {
    void* ptr = location;
    ptr = (void*)(((size_t)ptr) + NMP::Memory::align(sizeof(SampledCurveEventsBuffer), NMP_NATURAL_TYPE_ALIGNMENT));
    m_eventSamples = (SampledCurveEvent*) ptr;
  }
  else
  {
    m_eventSamples = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
SampledCurveEventsBuffer* SampledCurveEventsBuffer::create(NMP::MemoryAllocator* allocator, uint32_t numEvents)
{
  SampledCurveEventsBuffer* result;

  NMP::Memory::Format memReqs = getMemoryRequirements(numEvents);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  result = init(resource, numEvents);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SampledCurveEventsBuffer::getMemoryRequirements(uint32_t numEvents)
{
  NMP::Memory::Format result(sizeof(SampledCurveEventsBuffer), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Format eventsReqs = SampledCurveEvent::getMemoryRequirements();
  eventsReqs.size = NMP::Memory::align(eventsReqs.size * numEvents, eventsReqs.alignment);
  result += eventsReqs;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
SampledCurveEventsBuffer* SampledCurveEventsBuffer::init(NMP::Memory::Resource& resource, uint32_t numEvents)
{
  NMP::Memory::Format format(sizeof(SampledCurveEventsBuffer), NMP_NATURAL_TYPE_ALIGNMENT);

  resource.align(format);
  SampledCurveEventsBuffer* result = (SampledCurveEventsBuffer*)resource.ptr;
  resource.increment(format);

  result->m_sampleBufferSize = numEvents;
  result->m_numSamples = 0;

  if (numEvents > 0)
  {
    format = SampledCurveEvent::getMemoryRequirements();
    format.size = NMP::Memory::align(format.size * numEvents, format.alignment);

    resource.align(format);
    result->m_eventSamples = (SampledCurveEvent*) resource.ptr;
    resource.increment(format);
  }
  else
  {
    result->m_eventSamples = NULL;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// Sample the provided curve event track at the specified time and add the sample value to the buffer.
bool SampledCurveEventsBuffer::sampleCurveTrackAndAddToBuffer(
  const EventTrackDefCurve* eventTrack,  // Events track to sample from.
  float                     sampleTime,  // What point to sample at.
  uint32_t                  runtimeID)   // The runtime ID of the source event track
{
  NMP_ASSERT(eventTrack);
  NMP_ASSERT(m_numSamples < m_sampleBufferSize);

  const EventDefCurve* eventAtTime = eventTrack->getEventAtTime(sampleTime);
  NMP_ASSERT(eventAtTime);
  // Default to a blend weight of 1
  m_eventSamples[m_numSamples].set(
    eventTrack->getUserData(),
    runtimeID,
    eventAtTime->getUserData(),
    eventTrack->getValueAtTime(sampleTime),
    1.0f);
  ++m_numSamples;
  return true;  // We have added a sample
}

//----------------------------------------------------------------------------------------------------------------------
void SampledCurveEventsBuffer::combine(
  const SampledCurveEventsBuffer* input0,
  const SampledCurveEventsBuffer* input1,
  float                           weight)
{
  NMP_ASSERT(input0 && input1);
  NMP_ASSERT(input0->getNumSampledEvents() + input1->getNumSampledEvents() <= m_sampleBufferSize);
  uint32_t count = 0;
  for (uint32_t i = 0; i < input0->getNumSampledEvents(); ++i)
  {
    m_eventSamples[count] = *(input0->getSampledEvent(i));
    m_eventSamples[count].scaleBlendWeight(weight);
    ++count;
  }
  for (uint32_t i = 0; i < input1->getNumSampledEvents(); ++i)
  {
    m_eventSamples[count] = *(input1->getSampledEvent(i));
    m_eventSamples[count].scaleBlendWeight(1.0f - weight);
    ++count;
  }
  m_numSamples = input0->getNumSampledEvents() + input1->getNumSampledEvents();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SampledCurveEventsBuffer::fillAtAndScale(
  uint32_t    startingIndex,
  const SampledCurveEventsBuffer* input,
  const float scale)
{
  NMP_ASSERT(input);
  NMP_ASSERT(startingIndex + input->getNumSampledEvents() <= m_sampleBufferSize);

  uint32_t count = startingIndex;

  for (uint32_t i = 0; i < input->getNumSampledEvents(); ++i)
  {
    m_eventSamples[count] = *(input->getSampledEvent(i));
    m_eventSamples[count].scaleBlendWeight(scale);
    ++count;
  }

  return count;
}

//----------------------------------------------------------------------------------------------------------------------
void SampledCurveEventsBuffer::fill(const SampledCurveEventsBuffer* input)
{
  NMP_ASSERT(input);
  NMP_ASSERT(m_numSamples + input->getNumSampledEvents() <= m_sampleBufferSize);
  uint32_t count = m_numSamples;
  for (uint32_t i = 0; i < input->getNumSampledEvents(); ++i)
  {
    m_eventSamples[count++] = *(input->getSampledEvent(i));
  }
  m_numSamples = count;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SampledCurveEventsBuffer::findIndexOfSampleWithSourceEventUserData(
  uint32_t startingIndex,
  uint32_t eventUserData) const
{
  NMP_ASSERT(!m_eventSamples ? m_sampleBufferSize == 0 : true); // If there are no samples, the buffer size must be zero.
  uint32_t i = startingIndex;

  while (i < m_sampleBufferSize)
  {
    if (m_eventSamples[i].getSourceEventUserData() == eventUserData)
    {
      return i;
    }
    ++i;
  }

  return INVALID_EVENT_INDEX;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SampledCurveEventsBuffer::findIndexOfSampleWithSourceUserData(
  uint32_t startingIndex,
  uint32_t trackUserData,
  uint32_t eventUserData) const
{
  NMP_ASSERT(!m_eventSamples ? m_sampleBufferSize == 0 : true); // If there are no samples, the buffer size must be zero.
  uint32_t i = startingIndex;

  while (i < m_sampleBufferSize)
  {
    if (
      ((trackUserData == USER_DATA_UNSPECIFIED) || (m_eventSamples[i].getSourceTrackUserData() == trackUserData)) &&
      ((eventUserData == USER_DATA_UNSPECIFIED) || (m_eventSamples[i].getSourceEventUserData() == eventUserData)))
    {
      return i;
    }
    ++i;
  }

  return INVALID_EVENT_INDEX;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SampledCurveEventsBuffer::findIndexOfSampleWithSourceEventTrackRuntimeID(
  uint32_t startingIndex,
  uint32_t trackRuntimeID) const
{
  NMP_ASSERT(!m_eventSamples ? m_sampleBufferSize == 0 : true); // If there are no samples, the buffer size must be zero.
  uint32_t i = startingIndex;

  while (i < m_sampleBufferSize)
  {
    if (m_eventSamples[i].getSourceTrackRuntimeID() == trackRuntimeID)
    {
      return i;
    }
    ++i;
  }

  return INVALID_EVENT_INDEX;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
bool locateEventTrackDefCurve(uint32_t NMP_USED_FOR_ASSERTS(assetType), void* assetMemory)
{
  NMP_ASSERT(assetType == MR::Manager::kAsset_EventTrackCurve);
  MR::EventTrackDefCurve* eventTrack = (MR::EventTrackDefCurve*)assetMemory;
  return eventTrack->locate();
}
#endif

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
