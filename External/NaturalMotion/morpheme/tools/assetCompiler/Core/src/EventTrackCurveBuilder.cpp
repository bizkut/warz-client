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
#include "EventTrackCurveBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format EventTrackDefCurveBuilder::getMemoryRequirements(
  AP::AssetProcessor*              NMP_UNUSED(processor),
  const ME::CurveEventTrackExport* eventTrackExport)
{
  return getMemoryRequirements(eventTrackExport->getNumEvents(), eventTrackExport->getName());
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format EventTrackDefCurveBuilder::getInstanceMemoryRequirements()
{
  return getMemoryRequirements(m_numEvents, m_name);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format EventTrackDefCurveBuilder::getMemoryRequirements(
  uint32_t    numEvents,
  const char* name)
{
  NMP::Memory::Format result(sizeof(EventTrackDefCurve), MR_ATTRIB_DATA_ALIGNMENT);
  result.size += sizeof(MR::EventDefCurve) * numEvents;
  result.size += strlen(name) + 1;
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource EventTrackDefCurveBuilder::init(
  AP::AssetProcessor*              processor,
  const ME::CurveEventTrackExport* eventTrackExport)
{
  NMP::Memory::Format         memReqs;
  NMP::Memory::Resource       result;
  EventTrackDefCurveBuilder*  eventTrack;
  const ME::CurveEventExport* event;
  MR::EventDefCurve*          eventDef;
  float                       lastTime = 0.0f;

  // Initialise EventTrack.
  memReqs = EventTrackDefCurveBuilder::getMemoryRequirements(processor, eventTrackExport);
  result = NMPMemoryAllocateFromFormat(memReqs);
  eventTrack = (EventTrackDefCurveBuilder*) result.ptr;
  eventTrack->m_numEvents = eventTrackExport->getNumEvents();
  eventTrack->setUserData(eventTrackExport->getUserData());
  eventTrack->m_events = (MR::EventDefCurve*)
                         ((char*) result.ptr + NMP::Memory::align(sizeof(MR::EventTrackDefCurve), MR_ATTRIB_DATA_ALIGNMENT));

  // Copy in the event sequence name
  eventTrack->m_name = ((char*)eventTrack->m_events) + sizeof(MR::EventDefCurve) * eventTrackExport->getNumEvents();
  const char* trackName = eventTrackExport->getName();
  strcpy_s(eventTrack->m_name, strlen(trackName) + 1, trackName);

  // Initialise all Events.
  // Events in the export data must be guaranteed time ordered when accessing using an event index.
  for (uint32_t i = 0; i < eventTrack->m_numEvents; ++i)
  {
    event = eventTrackExport->getEvent(i);
    eventDef = eventTrack->getEvent(i);
    NMP_VERIFY_MSG(event->getNormalisedStartTime() >= lastTime, "Curve event start times must appear sequentially.");
    eventDef->init(event->getNormalisedStartTime(), event->getFloatValue(), event->getUserData());
    lastTime = event->getNormalisedStartTime();
  }

  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
