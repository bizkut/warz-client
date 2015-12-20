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
#include "EventTrackDurationBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format EventTrackDefDurationBuilder::getMemoryRequirements(
  AP::AssetProcessor*                 NMP_UNUSED(processor),
  const ME::DurationEventTrackExport* eventTrackExport)
{
  return getMemoryRequirements(eventTrackExport->getNumEvents(), eventTrackExport->getName());
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format EventTrackDefDurationBuilder::getInstanceMemoryRequirements()
{
  return getMemoryRequirements(m_numEvents, m_name);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format EventTrackDefDurationBuilder::getMemoryRequirements(
  uint32_t    numEvents,
  const char* name)
{
  NMP::Memory::Format result(sizeof(EventTrackDefDuration), MR_ATTRIB_DATA_ALIGNMENT);
  result.size += sizeof(MR::EventDefDuration) * numEvents;
  result.size += strlen(name) + 1;
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource EventTrackDefDurationBuilder::init(
  AP::AssetProcessor*                 processor,
  const ME::DurationEventTrackExport* eventTrackExport)
{
  NMP::Memory::Format           memReqs;
  NMP::Memory::Resource         result;
  const ME::DurationEventExport* event;
  MR::EventDefDuration*          eventDef;
  float                         lastTime = 0.0f;

  // Initialise EventTrack.
  memReqs = EventTrackDefDurationBuilder::getMemoryRequirements(processor, eventTrackExport);
  result = NMPMemoryAllocateFromFormat(memReqs);
  EventTrackDefDurationBuilder* eventTrack = (EventTrackDefDurationBuilder*) result.ptr;
  eventTrack->m_type = MR::kEventType_Duration;
  eventTrack->setUserData(eventTrackExport->getUserData());
  eventTrack->m_numEvents = eventTrackExport->getNumEvents();
  eventTrack->m_events = (MR::EventDefDuration*)
                         ((char*) result.ptr + NMP::Memory::align(sizeof(MR::EventTrackDefDuration), NMP_NATURAL_TYPE_ALIGNMENT));

  // Copy in the event sequence name
  eventTrack->m_name = ((char*)eventTrack->m_events) + sizeof(MR::EventDefDuration) * eventTrackExport->getNumEvents();
  const char* trackName = eventTrackExport->getName();
  strcpy_s(eventTrack->m_name, strlen(trackName) + 1, trackName);

  // Initialise all Events.
  // Events in the export data must be guaranteed time ordered.
  for (uint32_t i = 0; i < eventTrack->m_numEvents; ++i)
  {
    event = eventTrackExport->getEvent(i);
    eventDef = eventTrack->getEvent(i);

    NMP_VERIFY_MSG(event->getNormalisedStartTime() >= lastTime, "Duration event start times must appear sequentially.");
    NMP_VERIFY_MSG(event->getNormalisedDuration() >= 0.0f, "The duration of an event cannot be negative.");
    eventDef->init(event->getNormalisedStartTime(), event->getNormalisedDuration(), event->getUserData());

    lastTime = event->getNormalisedStartTime();
  }

  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
