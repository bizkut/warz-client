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
#include "EventTrackDiscreteBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format EventTrackDefDiscreteBuilder::getMemoryRequirements(
  AP::AssetProcessor*                 NMP_UNUSED(processor),
  const ME::DiscreteEventTrackExport* eventTrackExport)
{
  return getMemoryRequirements(eventTrackExport->getNumEvents(), eventTrackExport->getName());
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format EventTrackDefDiscreteBuilder::getInstanceMemoryRequirements()
{
  return getMemoryRequirements(m_numEvents, m_name);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format EventTrackDefDiscreteBuilder::getMemoryRequirements(
  uint32_t    numEvents,
  const char* name)
{
  NMP::Memory::Format result(sizeof(EventTrackDefDiscrete), MR_ATTRIB_DATA_ALIGNMENT);
  result.size += sizeof(MR::EventDefDiscrete) * numEvents;
  result.size += strlen(name) + 1;
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource EventTrackDefDiscreteBuilder::init(
  AP::AssetProcessor*                 processor,
  const ME::DiscreteEventTrackExport* eventTrackExport)
{
  // Initialise EventTrack.
  NMP::Memory::Format memReqs = EventTrackDefDiscreteBuilder::getMemoryRequirements(processor, eventTrackExport);
  NMP::Memory::Resource result = NMPMemoryAllocateFromFormat(memReqs);
  EventTrackDefDiscreteBuilder* eventTrack = (EventTrackDefDiscreteBuilder*) result.ptr;
  eventTrack->m_numEvents = eventTrackExport->getNumEvents();
  eventTrack->setUserData(eventTrackExport->getUserData());
  eventTrack->m_events = (MR::EventDefDiscrete*)
                         ((char*) result.ptr + NMP::Memory::align(sizeof(MR::EventTrackDefDiscrete), NMP_NATURAL_TYPE_ALIGNMENT));

  // Copy in the event sequence name
  eventTrack->m_name = ((char*)eventTrack->m_events) + sizeof(MR::EventDefDiscrete) * eventTrackExport->getNumEvents();
  const char* trackName = eventTrackExport->getName();
  strcpy_s(eventTrack->m_name, strlen(trackName) + 1, trackName);

  eventTrack->m_type = MR::kEventType_Discrete;

  // Initialise all Events.
  // Events in the export data must be guaranteed increasing time ordered by index.
  for (uint32_t i = 0; i < eventTrack->m_numEvents; ++i)
  {
    const ME::DiscreteEventExport* thisEvent = eventTrackExport->getEvent(i);
    float thisEventPos = thisEvent->getNormalisedTime();
    if (thisEventPos > 1.0f)
      thisEventPos = 1.0f;  // Event times must be normalised (0.0 - 1.0)
    NMP_VERIFY_MSG(thisEventPos >= 0.0f && thisEventPos <= 1.0f, "Event Position lies outside normalised range (0.0 - 1.0).");

    uint32_t thisEventUserType = thisEvent->getUserData();
    float nextEventPos;
    if (i == (eventTrack->m_numEvents - 1))
    {
      // Deal with last Event.
      nextEventPos = 1.0f;
    }
    else
    {
      // This Event has a following Event.
      const ME::DiscreteEventExport* nextEvent = eventTrackExport->getEvent(i + 1);
      nextEventPos = nextEvent->getNormalisedTime();
      if (nextEventPos > 1.0f)
        nextEventPos = 1.0f;  // Event times must be normalised (0.0 - 1.0)
      NMP_VERIFY_MSG(nextEventPos >= 0.0f && nextEventPos <= 1.0f, "Event Position lies outside normalised range (0.0 - 1.0).");
    }

    // Initialise the EventDef.
    float eventDuration = nextEventPos - thisEventPos;
    NMP_VERIFY_MSG(eventDuration >= 0.0f, "Event duration must be greater or equal to 0.0");
    new(&eventTrack->m_events[i]) MR::EventDefDiscrete;
    MR::EventDefDiscrete* eventDef = eventTrack->getEvent(i);
    eventDef->init(thisEventPos, eventDuration, thisEventUserType);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::EventTrackDefDiscrete* EventTrackDefDiscreteBuilder::initEmpty(
  const NMP::Memory::Resource* desc,
  uint32_t                     numEvents,
  const char*                  name)
{
  // Initialise EventTrack.
  EventTrackDefDiscreteBuilder* result = (EventTrackDefDiscreteBuilder*) desc->ptr;
  result->m_type = MR::kEventType_Discrete;

  NMP_VERIFY_MSG(numEvents > 0, "Must initialise a discrete event track builder with more than zero events");
  result->m_numEvents = numEvents;
  result->m_events = (MR::EventDefDiscrete*)
                     ((char*) desc->ptr + NMP::Memory::align(sizeof(MR::EventTrackDefDiscrete), NMP_NATURAL_TYPE_ALIGNMENT));

  // Copy in the event track name.
  result->m_name = ((char*)result->m_events) + sizeof(MR::EventDefDiscrete) * numEvents;
  strcpy_s(result->m_name, strlen(name) + 1, name);

  result->setUserData(0);

  for (uint32_t i = 0; i < result->m_numEvents; ++i)
  {
    new(&result->m_events[i]) MR::EventDefDiscrete;
  }

  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
