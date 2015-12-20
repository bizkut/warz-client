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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_EVENT_TRACK_DISCRETE_H
#define MR_EVENT_TRACK_DISCRETE_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemoryAllocator.h"
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMMathUtils.h"
#include "morpheme/mrDefines.h"
#include "morpheme/mrEventTrackBase.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup EventTrackDiscreteModule Discrete Event Tracks.
/// \ingroup EventTrackModule
///
/// Fixed point event types. Each event also stores the duration until the following event.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::EventDefDiscrete
/// \brief A trigger type event with duration and type.
/// \ingroup EventTrackDiscreteModule
///
/// Duration is the time until the start of the following event in the track.
/// This type of event can be used for marking foot falls, triggering sound playback etc.
/// Loaded from a definition file, but also dynamically created.
//----------------------------------------------------------------------------------------------------------------------
class EventDefDiscrete
{
public:
  EventDefDiscrete() {}
  ~EventDefDiscrete() {}

  /// \brief Set all members of this Event.
  NM_INLINE void init(TimeFraction startTime, TimeFraction duration, uint32_t userData);
  NM_INLINE void copy(const EventDefDiscrete& source);

  /// \brief Start position access.
  TimeFraction getStartTime() const { return m_startTime; }
  void setStartTime(TimeFraction start) { NMP_ASSERT(start >= 0.0f); m_startTime = start; }

  /// \brief Duration to the next Event access.
  TimeFraction getDuration() const { return m_duration; }
  void setDuration(TimeFraction duration) { NMP_ASSERT(duration >= 0.0f); m_duration = duration; }

  /// \brief User Type access.
  uint32_t getUserData() const { return m_userData; }
  void setUserData(uint32_t userData) { m_userData = userData; }

  NM_INLINE void dislocate();
  NM_INLINE void locate();

protected:
  TimeFraction  m_startTime;  ///< Start point.
  TimeFraction  m_duration;   ///< To the next event.
  uint32_t      m_userData;   ///< Allows this event to be associated with arbitrary user data.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::EventTrackDefDiscrete
/// \brief Sequence of events dividing up a unit interval.
/// \ingroup EventTrackDiscreteModule
/// \see MR::EventTrackDefBase
///
/// Used to mark significant events in an animation, such as foot impacts.
/// All values are fractional (>= 0.0 and <= 1.0).
/// This is source data that is initialised directly from storage.
/// Loaded from a definition file.
//----------------------------------------------------------------------------------------------------------------------
class TriggeredDiscreteEvent; //Forward declaration
class EventTrackDefDiscrete : public EventTrackDefBase
{
public:
  bool locate();
  bool dislocate();
  void relocate();

  // Find the requirements of an instance of this track type.
  NMP::Memory::Format getInstanceMemoryRequirements();

  /// \brief Find which Event this time fraction lies within.
  ///
  /// Note that if the timeFraction is before the start of event zero then this function will still
  /// return event index zero as the event that this timeFraction lies within. So not strictly accurate,
  /// but it is the current desired behavior.
  uint8_t findEventIndexForTimeFraction(TimeFraction timeFraction) const;

  /// \brief Debug only function, makes sure the sum of durations comes to 1.0.
  bool validate() const;

  /// \brief Event access.
  EventDefDiscrete* getEvent(uint32_t index) { NMP_ASSERT(index < m_numEvents); return &m_events[index]; }
  const EventDefDiscrete* getEvent(uint32_t index) const { NMP_ASSERT(index < m_numEvents); return &m_events[index]; }

  /// \brief Find the events that occur between the specified times.
  /// \return Number of discrete events that occur between the 2 times.
  uint32_t findEventsBetweenTimes(
    float              timeFrom,             ///< What point to start sampling from
    bool               sampleBackwards,      ///< Track forwards or backwards in the EventTrack.
    float              sampleDuration,       ///< How long to sample for.
    bool               allowWrapping,        ///< If true the track will wrap around
                                             ///<  over the start or end dependent on weather
                                             ///<  it is playing backwards or forwards.
    EventDefDiscrete** foundEvents,          ///<  Passed in to be filled.
    uint32_t           foundEventsArraySize, ///< Indicates foundEvents array size.
    float              rangeStart,           ///< The animation clip range start.
    float              rangeEnd,             ///< The animation clip range end.
    bool               inclusiveStart) const;///< Whether the start of the range should be included or not.

protected:

  /// \brief Add to array of found events.
  void addToFoundEventsArray(
    EventDefDiscrete*  eventToAdd, 
    EventDefDiscrete** foundEvents,             ///< Passed in to be filled.
    uint32_t           foundEventsArraySize,    ///< Indicates size triggeredEventTypes array size.
    uint32_t&          addedEventsCount) const; ///< Incremented if add is successful.

  /// \brief Find triggered discrete events over the sampleDuration backwards from timeFrom.
  /// \return The number of events found and added to foundEvents.
  uint32_t sampleEventsBackwards(
    float              timeFrom,             ///< What point to start sampling from.
    float              sampleDuration,       ///< How long to sample for.
    bool               allowWrapping,        ///< If true the track will wrap around
                                             ///< over the start or end dependent on whether
                                             ///< it is playing backwards or forwards.
    EventDefDiscrete** foundEvents,          ///< Passed in to be filled.
    uint32_t           foundEventsArraySize, ///< Indicates foundEvents array size.
    float              rangeStart,           ///< The animation clip range start.
    float              rangeEnd,             ///< The animation clip range end.
    bool               inclusiveStart) const;///< Whether the start of the range should be included or not.

  /// \brief Find triggered discrete events over the sampleDuration forwards from timeFrom.
  /// \return The number of events found and added to foundEvents.
  uint32_t sampleEventsForwards(
    float              timeFrom,             ///< What point to start sampling from.
    float              sampleDuration,       ///< How long to sample for.
    bool               allowWrapping,        ///< If true the track will wrap around
                                             ///< over the start or end dependent on whether
                                             ///< it is playing backwards or forwards.
    EventDefDiscrete** foundEvents,          ///< Passed in to be filled.
    uint32_t           foundEventsArraySize, ///< Indicates foundEvents array size.
    float              rangeStart,           ///< The animation clip range start.
    float              rangeEnd,             ///< The animation clip range end.
    bool               inclusiveStart) const;///< Whether the start of the range should be included or not.

protected:
  EventDefDiscrete* m_events;    ///< Array of all events.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TriggeredDiscreteEvent
/// \brief
/// \ingroup EventTrackDiscreteModule
//----------------------------------------------------------------------------------------------------------------------
class TriggeredDiscreteEvent
{
public:
  TriggeredDiscreteEvent() {}
  ~TriggeredDiscreteEvent() {}

  static NMP::Memory::Format getMemoryRequirements();

  NM_INLINE void locate();
  NM_INLINE void dislocate();

  NM_INLINE void set(uint32_t sourceTrackUserData, uint32_t sourceTrackRuntimeID, uint32_t sourceEventUserData, float weight);

  NM_INLINE void setSourceEventUserData(uint32_t userData) { m_sourceEventUserData = userData; }
  NM_INLINE uint32_t getSourceEventUserData() const { return m_sourceEventUserData; }

  NM_INLINE void setSourceTrackRuntimeID(uint32_t runtimeID) { m_sourceTrackRuntimeID = runtimeID; }
  NM_INLINE uint32_t getSourceTrackRuntimeID() const { return m_sourceTrackRuntimeID; }

  NM_INLINE void setSourceTrackUserData(uint32_t userData) { m_sourceTrackUserData = userData; }
  NM_INLINE uint32_t getSourceTrackUserData() const { return m_sourceTrackUserData; }

  NM_INLINE float getBlendWeight() const { return m_blendWeight; }
  NM_INLINE void scaleBlendWeight(float scale) { m_blendWeight *= NMP::clampValue(scale, 0.0f, 1.0f); }

  TriggeredDiscreteEvent& operator = (const TriggeredDiscreteEvent& other)
  {
    this->set(other.m_sourceTrackUserData, other.m_sourceTrackRuntimeID, other.m_sourceEventUserData, other.m_blendWeight);
    return *this;
  }

protected:
  uint32_t m_sourceTrackRuntimeID; ///< Runtime assigned event track ID.
  uint32_t m_sourceTrackUserData;  ///< Arbitrary user data associated with the source event track.
  uint32_t m_sourceEventUserData;  ///< Arbitrary user data associated with the event that has fired.
  float    m_blendWeight;          ///< Cumulative contribution of this event to the end result.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TriggeredDiscreteEventsBuffer
/// \brief To hold a buffer of the Discrete Events have triggered during a Network update.
/// \ingroup EventTrackDiscreteModule
///
/// By trigger we mean that the start time of an event has occurred in this update period.
//----------------------------------------------------------------------------------------------------------------------
class TriggeredDiscreteEventsBuffer
{
public:
  TriggeredDiscreteEventsBuffer() {}
  ~TriggeredDiscreteEventsBuffer() {}

  void locate();
  void dislocate();
  void relocate(void* location);
  void relocate() { relocate(this); }

  static TriggeredDiscreteEventsBuffer* create(NMP::MemoryAllocator* allocator, uint32_t numEvents);
  static NMP::Memory::Format getMemoryRequirements(uint32_t numEvents);
  static TriggeredDiscreteEventsBuffer* init(NMP::Memory::Resource& resource, uint32_t numEvents);

  /// \brief Returns the number of triggered events in this buffer.
  NM_INLINE uint32_t getNumTriggeredEvents() const { return m_numEvents; }

  /// \brief Return the user event type ID of one of the triggered events in the buffer.
  NM_INLINE const TriggeredDiscreteEvent* getTriggeredEvent(uint32_t bufferIndex) const;

  /// \brief Return the user event type ID of one of the triggered events in the buffer.
  NM_INLINE TriggeredDiscreteEvent* getTriggeredEvent(uint32_t bufferIndex);

  /// \brief Search through the m_eventTriggered array for a triggered event with the specified source event user data.
  ///
  /// Several entries may have the same source event user data and this function can be used to find each one in turn.
  /// \return  Index of event within this buffer. INVALID_EVENT_INDEX if not found.
  uint32_t findIndexOfEventWithSourceEventUserData(
    uint32_t startingIndex, ///< Event index to start search from.
    uint32_t eventUserData  ///< Source event user data to search for.
  ) const;

  /// \brief Search through the m_eventTriggered array for a triggered event with the specified source track user data.
  ///
  /// Several entries may have the same source track user data and this function can be used to find each one in turn.
  /// \return  Index of event within this buffer. INVALID_EVENT_INDEX if not found.
  uint32_t findIndexOfEventWithSourceEventTrackUserData(
    uint32_t startingIndex, ///< Event index to start search from.
    uint32_t trackUserData  ///< Source track user data to search for.
  ) const;

  /// \brief Search through the m_eventTriggered array for a triggered event with the specified source track runtime ID.
  ///
  /// Several entries may have the same source track runtime ID and this function can be used to find each one in turn.
  /// \return  Index of event within this buffer. INVALID_EVENT_INDEX if not found.
  uint32_t findIndexOfEventWithSourceEventTrackRuntimeID(
    uint32_t startingIndex, ///< Event index to start search from.
    uint32_t trackRuntimeID ///< Source track runtime ID to search for.
  ) const;

  /// \brief Set the destination event with the provided values.
  NM_INLINE void setEvent(uint32_t eventIndex, uint32_t sourceTrackUserData, uint32_t sourceTrackRuntimeID, uint32_t sourceEventUserData);

  /// \brief Set the destination event with the source.
  NM_INLINE void setEvent(uint32_t eventIndex, const TriggeredDiscreteEvent* sourceEvent);

  /// \brief Combine the 2 source buffers into this one.
  ///
  /// Overwrites any existing content.
  void combine(const TriggeredDiscreteEventsBuffer* input0, const TriggeredDiscreteEventsBuffer* input1, float weight);

  /// \brief Combine the 2 source buffers into this one.
  ///
  /// Preserves blend weight of source 0 and attenuates source 1.
  void additiveCombine(const TriggeredDiscreteEventsBuffer* input0, const TriggeredDiscreteEventsBuffer* input1, float weight);

  /// \brief Add the source buffer into this one at indicated index.
  ///
  /// Overwrites any existing content.
  uint32_t fillAt(uint32_t startingIndex, const TriggeredDiscreteEventsBuffer* input);

  /// \brief Add the source buffer into this one at indicated index, scale by a factor
  /// \sa fillAt
  ///
  /// Overwrites any existing content.
  uint32_t fillAtAndScale(uint32_t startingIndex, const TriggeredDiscreteEventsBuffer* input, const float scale);

protected:
  uint32_t                m_numEvents;       ///<
  TriggeredDiscreteEvent* m_triggeredEvents; ///< Array of triggered events.
};

//----------------------------------------------------------------------------------------------------------------------
// TriggeredDiscreteEvent functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TriggeredDiscreteEvent::locate()
{
  NMP::endianSwap(m_sourceTrackRuntimeID);
  NMP::endianSwap(m_sourceTrackUserData);
  NMP::endianSwap(m_sourceEventUserData);
  NMP::endianSwap(m_blendWeight);
}
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TriggeredDiscreteEvent::dislocate()
{
  NMP::endianSwap(m_sourceTrackRuntimeID);
  NMP::endianSwap(m_sourceTrackUserData);
  NMP::endianSwap(m_sourceEventUserData);
  NMP::endianSwap(m_blendWeight);
}

//----------------------------------------------------------------------------------------------------------------------
// TriggeredDiscreteEventsBuffer functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const TriggeredDiscreteEvent* TriggeredDiscreteEventsBuffer::getTriggeredEvent(uint32_t bufferIndex) const
{
  NMP_ASSERT(bufferIndex < m_numEvents && m_triggeredEvents);
  return &(m_triggeredEvents[bufferIndex]);
}

//----------------------------------------------------------------------------------------------------------------------
// TriggeredDiscreteEventsBuffer functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE TriggeredDiscreteEvent* TriggeredDiscreteEventsBuffer::getTriggeredEvent(uint32_t bufferIndex)
{
  NMP_ASSERT(bufferIndex < m_numEvents && m_triggeredEvents);
  return &(m_triggeredEvents[bufferIndex]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TriggeredDiscreteEventsBuffer::setEvent(
  uint32_t eventIndex,
  uint32_t sourceTrackUserData,
  uint32_t sourceTrackRuntimeID,
  uint32_t sourceEventUserData)
{
  NMP_ASSERT(eventIndex < m_numEvents);
  // Default to a blend weight of 1
  m_triggeredEvents[eventIndex].set(sourceTrackUserData, sourceTrackRuntimeID, sourceEventUserData, 1.0f);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TriggeredDiscreteEventsBuffer::setEvent(uint32_t eventIndex, const TriggeredDiscreteEvent* sourceEvent)
{
  NMP_ASSERT(eventIndex < m_numEvents && sourceEvent);
  m_triggeredEvents[eventIndex] = *sourceEvent;
}

//----------------------------------------------------------------------------------------------------------------------
// EventDefDiscrete functions.
//----------------------------------------------------------------------------------------------------------------------
// Set all members of this Event.
NM_INLINE void EventDefDiscrete::init(TimeFraction startTime, TimeFraction duration, uint32_t userType)
{
  NMP_ASSERT(startTime >= 0.0f);
  m_startTime = startTime;
  NMP_ASSERT(duration >= 0.0f);
  m_duration = duration;
  m_userData = userType;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void EventDefDiscrete::copy(const EventDefDiscrete& source)
{
  m_startTime = source.m_startTime;
  m_duration = source.m_duration;
  m_userData = source.m_userData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void EventDefDiscrete::dislocate()
{
  NMP::endianSwap(m_startTime);
  NMP::endianSwap(m_duration);
  NMP::endianSwap(m_userData);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void EventDefDiscrete::locate()
{
  NMP::endianSwap(m_startTime);
  NMP::endianSwap(m_duration);
  NMP::endianSwap(m_userData);
}

//----------------------------------------------------------------------------------------------------------------------
// TriggeredDiscreteEvent functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TriggeredDiscreteEvent::set(
  uint32_t sourceTrackUserData,
  uint32_t sourceTrackRuntimeID,
  uint32_t sourceEventUserData,
  float    blendWeight)
{
  m_sourceTrackUserData   = sourceTrackUserData;
  m_sourceTrackRuntimeID  = sourceTrackRuntimeID;
  m_sourceEventUserData   = sourceEventUserData;
  m_blendWeight           = blendWeight;
}

//----------------------------------------------------------------------------------------------------------------------
bool locateEventTrackDefDiscrete(uint32_t assetType, void* assetMemory);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_EVENT_TRACK_DISCRETE_H
//----------------------------------------------------------------------------------------------------------------------
