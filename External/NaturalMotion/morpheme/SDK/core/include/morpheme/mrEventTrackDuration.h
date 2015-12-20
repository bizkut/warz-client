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
#ifndef MR_EVENT_TRACK_DURATION_H
#define MR_EVENT_TRACK_DURATION_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "morpheme/mrDefines.h"
#include "morpheme/mrEventTrackSync.h"
#include "morpheme/mrSyncEventPos.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

typedef int32_t EventIndex;
const EventIndex gUndefinedEventIndex = -1;

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup EventTrackDurationModule Duration Event Tracks.
/// \ingroup EventTrackModule
///
/// Events with a start point and a duration indicating how long it is active.
//----------------------------------------------------------------------------------------------------------------------

/// Forward declaration.
class SampledCurveEventsBuffer;
class Network;
class EventDuration;

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::EventDefDuration
/// \brief
/// \ingroup EventTrackDurationModule
//----------------------------------------------------------------------------------------------------------------------
class EventDefDuration
{
public:
  EventDefDuration() {}
  ~EventDefDuration() {}

  /// \brief Set all members of this Event.
  NM_INLINE void init(TimeFraction startTime, TimeFraction duration, uint32_t userData);

  /// \brief Start position access.
  TimeFraction getStartTime() const { return m_startTime; }
  void setStartTime(TimeFraction start) { NMP_ASSERT(start >= 0.0f); m_startTime = start; }

  /// \brief Duration of this event access.
  TimeFraction getDuration() const { return m_duration; }
  void setDuration(TimeFraction duration) { NMP_ASSERT(duration >= 0.0f); m_duration = duration; }

  /// \brief User Type access.
  uint32_t getUserData() const { return m_userData; }
  void setUserData(uint32_t userData) { m_userData = userData; }

  NM_INLINE bool dislocate();
  NM_INLINE bool locate();

protected:
  TimeFraction  m_startTime;  ///< Start point.
  TimeFraction  m_duration;   ///< Duration of this event.
  uint32_t      m_userData;   ///< Allows this event to be associated with arbitrary user data.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::EventTrackDefDuration
/// \brief
/// \ingroup EventTrackDurationModule
/// \see MR::EventTrackDefBase
//----------------------------------------------------------------------------------------------------------------------
class EventTrackDefDuration : public EventTrackDefBase
{
public:
  bool locate();
  bool dislocate();
  void relocate();

  /// \brief Event access.
  EventDefDuration* getEvent(uint32_t index) { NMP_ASSERT(index < m_numEvents); return &m_events[index]; }

protected:
  EventTrackDefDuration();
  ~EventTrackDefDuration();

protected:
  EventDefDuration* m_events;    ///< Array of all events.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class
//----------------------------------------------------------------------------------------------------------------------
class EventDurationPool
{
public:
  EventDurationPool() {}
  ~EventDurationPool() {}

  static EventDurationPool* create(NMP::MemoryAllocator* allocator, uint32_t numEvents);
  static NMP::Memory::Format getMemoryRequirements(uint32_t numEvents);
  static EventDurationPool* init(NMP::Memory::Resource& resource, uint32_t numEvents);

  void init(uint32_t size, EventDuration* eventsArray);
  void relocate(void* location);
  void relocate() { relocate(this); }

  /// \brief Allocate an EventDefDuration from the pool.
  NM_INLINE EventDuration* allocateEvent(EventIndex& eventIndex);

  /// \brief
  NM_INLINE EventDuration* getEvent(EventIndex eventIndex);
  NM_INLINE const EventDuration* getEvent(EventIndex eventIndex) const;

public:
  uint32_t       m_size;          ///< The total number of events available in the pool.
  uint32_t       m_numAllocated;  ///<
  EventDuration* m_eventsPool;    ///< The pool of events which this EventTrackDurationSet uses.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::EventDuration
/// \brief
/// \ingroup EventTrackDurationModule
//----------------------------------------------------------------------------------------------------------------------
class EventDuration
{
public:
  EventDuration() {}
  ~EventDuration() {}

  /// \brief Return the memory requirements of an instance.
  static NM_INLINE NMP::Memory::Format getMemoryRequirements();

  /// \brief Initialise an event.
  NM_INLINE void init(
    float    syncEventSpaceStartPoint, ///< Start point of this event within time synchronisation event space.
    float    syncEventSpaceMidPoint,   ///< Mid point of this event within time synchronisation event space.
    float    syncEventSpaceDuration,   ///< As a time synchronisation event fraction.
    float    weight,                   ///<
    uint32_t userData                  ///<
  );

  /// \brief Copy an events contents into this event (excluding the list connection info).
  NM_INLINE void copy(const EventDuration* sourceEvent);

  /// \brief Calculate the distance (expressed in time synchronisation event space) between the mid point of this event
  /// and the event space position specified, traveling forwards/clockwise in time synchronisation space.
  float getDistanceToEventPosForward(
    float    eventSpaceTestPosition,
    float    numEventsInSyncEventSpace, ///< Num events in the time synchronisation event space we are working in
                                        ///< (no fractional part).
    bool     loop) const;

  /// \brief Calculate the distance (expressed in event space) between the mid point of this event and the event space
  /// position specified, traveling backwards/anti-clockwise in time synchronisation space.
  float getDistanceToEventPosBackwards(
    float    toSyncEventSpaceMidPoint,
    float    numEventsInSyncEventSpace, ///< Num events in the time synchronisation event space we are working in
                                        ///< (no fractional part).
    bool     loop) const;

  /// \brief Blend the positions of 2 events together and store the result in this event.
  void blend(
    EventDuration*       event1,
    EventDuration*       event2,
    float                numEventsInSyncEventSpace, ///< Num events in the time synchronisation event space we are working in
                                                    ///< (no fractional part).
    float                weight                     ///< 0.0 = fully event1, 1.0 = fully event2.
  );

  /// \brief Determines if this event overlaps with another by any amount.
  bool overlapsWith(
    const EventDuration* testEvent,
    float                numEventsInSyncEventSpace  ///< Num events in the time synchronisation event space we are working in
                                                    ///< (no fractional part).
  ) const;

  /// \brief Determines if this event is within the specified range of the test event.
  bool withinRange(
    const EventDuration* testEvent,                 ///<
    float                testRange,                 ///<
    float                numEventsInSyncEventSpace  ///< Num events in the time synchronisation event space we are working in
                                                    ///< (no fractional part).
  ) const;

  /// \brief Determine if the event synchronisation space test position lies within the range covered by this event.
  ///
  /// If within the event the fractional position within the event is returned. 1.0 = start to 0.0 = end.
  bool liesWithin(
    float          syncEventSpaceTestPos,     ///<
    float          numEventsInSyncEventSpace, ///< Num events in the time synchronisation event space we are working in
                                              ///< (no fractional part).
    float&         positionInEvent,           ///< If within the event, returns the fractional position within the event.
                                              ///< 1.0 = start to 0.0 = end.
    bool           loop) const;

  NM_INLINE uint32_t getUserData() const { return m_userData; }
  NM_INLINE void setUserData(uint32_t userData) { m_userData = userData; }

  NM_INLINE float getSyncEventSpaceStartPoint() const { return m_syncEventSpaceStartPoint; }  ///< Expressed in time synchronisation event space.

  NM_INLINE float getSyncEventSpaceMidPoint() const { return m_syncEventSpaceMidPoint; }      ///< Expressed in time synchronisation event space.
  NM_INLINE float getSyncEventSpaceDuration() const { return m_syncEventSpaceDuration; }
  NM_INLINE float getWeight() const { return m_weight; }
  NM_INLINE void setNextIndex(EventIndex nextEventIndex) { m_next = nextEventIndex; }
  NM_INLINE EventIndex getNextIndex() const { return m_next; }
  NM_INLINE EventDuration* getNext(EventDurationPool* pool) const { NMP_ASSERT(pool); return pool->getEvent(m_next); }
  NM_INLINE const EventDuration* getNext(const EventDurationPool* pool) const { NMP_ASSERT(pool); return pool->getEvent(m_next); }
  NM_INLINE void setProcessed(bool status) { m_processed = status; }
  NM_INLINE bool isProcessed() const { return m_processed; }

protected:
  uint32_t   m_userData;                 ///< Allows this event to be associated with arbitrary user data.
  float      m_syncEventSpaceStartPoint; ///< The start point of this event, specified within time synchronisation event space.
  float      m_syncEventSpaceMidPoint;   ///< The middle point of this event, specified within time synchronisation event space.
  float      m_syncEventSpaceDuration;   ///< Time from Start to the end of the event, expressed as a time
                                         ///<  synchronisation event fraction.
  float      m_weight;                   ///< What is the maximum weight value of this event. Gives an indication of the 
                                         ///<  contribution that this event will make to the end result of what ever this
                                         ///<  event is being used for e.g. Foot locking, sound effect volume etc.
                                         ///<  Also controls how this event will be blended to some extent.
  bool       m_processed;                ///<  Working flag, used when blending;
  EventIndex m_next;                     ///< Duration events are inserted into a linked list in their parent track.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::EventTrackDuration
/// \brief
/// \ingroup EventTrackDurationModule
//----------------------------------------------------------------------------------------------------------------------
class EventTrackDuration
{
public:
  EventTrackDuration() {}
  ~EventTrackDuration() {}

  static EventTrackDuration* create(NMP::MemoryAllocator* allocator);
  static NMP::Memory::Format getMemoryRequirements();
  static EventTrackDuration* init(NMP::Memory::Resource& resource);

  /// \brief
  void init();

  /// \brief Copy this track into the destination.
  void copy(EventTrackDuration& dest);

  NM_INLINE const EventDurationPool* getPool() const { return m_pool; }
  NM_INLINE EventDurationPool* getPool() { return m_pool; }
  NM_INLINE void setPool(EventDurationPool* pool) { m_pool = pool; }

  /// \brief Access to events list.
  NM_INLINE EventDuration* getEventsListHead() { NMP_ASSERT(m_pool); return m_pool->getEvent(m_eventsListHead); }
  NM_INLINE const EventDuration* getEventsListHead() const { NMP_ASSERT(m_pool); return m_pool->getEvent(m_eventsListHead); }

  NM_INLINE EventDuration* getEventsListTail() { NMP_ASSERT(m_pool); return m_pool->getEvent(m_eventsListTail); }
  NM_INLINE const EventDuration* getEventsListTail() const { NMP_ASSERT(m_pool); return m_pool->getEvent(m_eventsListTail); }

  NM_INLINE EventIndex getEventsListHeadIndex() const { return m_eventsListHead; }
  NM_INLINE EventIndex getEventsListTailIndex() const { return m_eventsListTail; }

  /// \brief Initialise this event track from a source duration event track (with event information expressed in
  ///  normalised time) and the time synchronisation event track space that we wish the new duration event track to be
  ///  expressed within.
  void initFromSourceDef(
    float                  clipStart,           ///< Where to start sampling from the source track.
    float                  clipDuration,        ///< Length of the clip to sample.
    EventTrackDefDuration* sourceDurationTrack, ///<
    uint32_t               runtimeID,           ///<
    EventTrackSync*        sourceSyncTrack,     ///< Sync space within which these tracks are defined.
    EventDurationPool*     pool,                ///<
    bool                   loopingAllowed,      ///< Are events allowed to loop beyond the end of the sync event track.
    bool                   playBackwards        ///< Initialise this data to be played backwards.
  );

  /// \brief Copy the supplied track and if necessary repeat it so that it covers the time synchronisation period indicated.
  /// \return false if source has no events and does not do a copy.
  bool copyRepeat(
    EventTrackDuration* sourceDurationTrack,        ///<
    uint32_t            syncEventCountToRepeatUpto, ///<
    EventDurationPool*  pool,                       ///<
    bool                loopingAllowed,             ///< Are events allowed to loop beyond the end of the sync event track.
    uint32_t            offset = 0                  ///< Event Index offset.
  );

  /// \brief Steps through the list of events and returns the next event that has the specified user data
  /// \return Returns NULL if no match found.
  EventDuration* findEventWithUserData(EventDuration* eventListPos, uint32_t userdata);

  /// \brief Sample this track at the specified time.
  ///
  /// Creates SampledCurveEvent's and inserts them into the provided SampledCurveEventsBuffer.
  void sample(
    SampledCurveEventsBuffer*       floatSamplesBuffer, ///< Float buffer to add samples too.
    const SyncEventPos*             syncEventPos,       ///< Time stretch event position to sample at.
    const SyncEventPos*             prevSyncEventPos,   ///< Previous time stretch event position sampled.
    const EventTrackSync*           syncEventSeq ,      ///< Time stretch event sequence.
    bool                            loop                ///< Looping animation
  );

  /// \brief Set the status flag on all events in the supplied list.
  void setProcessedFlags(bool status, EventDuration* eventListHead);

  /// \brief Blend 2 tracks together and store the result in this track.
  ///
  /// NOTE: Will only blend event tracks that have the same runtimeID and userData.
  void blend(
    EventTrackDuration* clip1,                 ///< Source clip 1.
    EventTrackDuration* clip2,                 ///< Source clip 2.
    float               weight,                ///< Blend weighting.
    float               selectionWeightCutOff, ///< At what proportional weight do we select one events details
                                               ///< over the other.
                                               ///< Below this value select clip1, above it select clip2.
    uint32_t            matchingOperation,     ///< Whether and how to match up the events from each track.
                                               ///< See DURATION_EVENT_MATCH_...
    bool                rejectNonMatched,      ///< If an event from one of the sources has not been matched to one
                                               ///< in the other source should we just discard it?
    uint32_t            blendOperation,        ///< Operation to perform on events that have been matched up.
                                               ///< See DURATION_EVENT_BLEND_OP_TYPE_...
    EventDurationPool*  pool,
    bool                loopingAllowed,        ///< Are events allowed to loop beyond the end of the sync event track.
    float               testRange = 1.0f,      ///< Within what range do we decide to match events together.
                                               ///< When event matching WITHIN_RANGE.
                                               ///< Expressed as an event fraction.
    uint32_t            clip2Offset = 0        ///< Offset of clip2 events in clip1 for blending.
  );

  /// \brief Event count access.
  uint32_t getNumEvents() const { return m_numEvents; }
  void setNumEvents(uint32_t numEvents) { m_numEvents = numEvents; }

  /// \brief Access to runtime assigned track ID.
  uint32_t getRuntimeID() const { return m_runtimeID; }
  void setRuntimeID(uint32_t runtimeID) { m_runtimeID = runtimeID; }

  /// \brief Access to user specified data associated with this track.
  uint32_t getUserData() const { return m_userData; }
  void setUserData(uint32_t userData) { m_userData = userData; }

  /// \brief Access to count of the number of events in the time synchronisation event track associated with this track.
  uint32_t getNumSyncEvents() const { return m_numSyncEvents; }
  void setNumSyncEvents(uint32_t numSyncEvents) { m_numSyncEvents = numSyncEvents; }

protected:
  // ------------- Event find functions -------------
  /// \brief Steps through the supplied list of events and finds and returns the first event that overlaps with the test event.
  /// \return NULL if no overlap found.
  EventDuration* findOverlappingEvent(
    const EventDuration* testEvent,           ///< Event to test.
    EventDuration*       eventListHead,       ///< List of events to test against.
    float                numEventsInSyncSpace ///< Num time synchronisation events in the event space we are working in (no fractional part).
  );

  /// \brief Steps through the supplied list of events and finds and returns the first event that is within the specified
  /// range of the test event.
  /// \return NULL if no overlap found.
  EventDuration* findEventWithinRange(
    const EventDuration* testEvent,           ///< Event to test.
    EventDuration*       eventListHead,       ///< List of events to test against.
    float                range,               ///< Expressed as an event fraction.
    float                numEventsInSyncSpace ///< Num time synchronisation events in the event space we are working in (no fractional part).
  );

  /// \brief Steps through the supplied list of events and finds and returns the first event that has the same user data
  /// as the test event.
  /// \return Returns NULL if no match found.
  EventDuration* findEventWithSameUserData(
    const EventDuration* testEvent,     ///< Event to test.
    EventDuration* eventListHead        ///< List of events to test against.
  );

  /// \brief Steps through the supplied list of events and finds and returns the first event that has the same user data
  /// and overlaps with the test event.
  /// Returns NULL if no overlap found.
  EventDuration* findEventWithSameUserDataAndOverlapping(
    const EventDuration* testEvent,           ///< Event to test.
    EventDuration*       eventListHead,       ///< List of events to test against.
    float                numEventsInSyncSpace ///< Num time synchronisation events in the event space we are working in (no fractional part).
  );

  /// \brief Steps through the supplied list of events and finds and returns the first event that has the same user data
  /// and is within the specified range of the test event.
  /// \return NULL if no overlap found.
  EventDuration* findEventWithSameUserDataAndWithinRange(
    const EventDuration* testEvent,           ///< Event to test.
    EventDuration*       eventListHead,       ///< List of events to test against.
    float                range,               ///< Expressed as an event fraction.
    float                numEventsInSyncSpace ///< Num time synchronisation events in the event space we are working in (no fractional part).
  );

  // ------------- Adding event functions -------------
  /// \brief Add this event to the end of our events list.
  EventDuration* addToTail(EventDuration* eventToAdd, EventIndex eventIndex);

  /// \brief Add this event to the start of our events list.
  EventDuration* addToHead(EventDuration* eventToAdd, EventIndex eventIndex);

  /// \brief Add this event correctly sorted on start point into the events list.
  EventDuration* addSorted(EventDuration* eventToAdd, EventIndex eventIndex);

  /// \brief Initialise a new event at the end of our events list.
  /// \return Pointer to newly added event.
  NM_INLINE EventDuration* createAndAddToTail(
    float              eventSyncSpaceStartPoint,   ///< Start point of this event within time synchronisation event space.
    float              eventSyncSpaceMidPoint,     ///< Mid point of this event within time synchronisation event space.
    float              eventSyncSpaceDuration,     ///< As a time synchronisation event fraction.
    float              weight,                     ///<
    uint32_t           userData                    ///<
  );

  /// \brief Initialise a new event at the end of our events list, copying from the specified source event.
  /// \return Pointer to newly added event.
  NM_INLINE EventDuration* copyAndAddToTail(const EventDuration* sourceEvent);

  /// \brief Initialise a new event at the start of our events list.
  /// \return Pointer to newly added event.
  NM_INLINE EventDuration* createAndAddToHead(
    float              eventSyncSpaceStartPoint, ///< Start point of this event within time synchronisation event space.
    float              eventSyncSpaceMidPoint,   ///< Mid point of this event within time synchronisation event space.
    float              eventSyncSpaceDuration,   ///< As a time synchronisation event fraction.
    float              weight,                   ///<
    uint32_t           userData                  ///<
  );

  /// \brief Initialise a new event at the start of our events list, copying from the specified source event.
  /// \return Pointer to newly added event.
  NM_INLINE EventDuration* copyAndAddToHead(const EventDuration* sourceEvent);

  /// \brief Initialise a new event and insert it correctly sorted on start point into the events list.
  /// \return Pointer to newly added event.
  NM_INLINE EventDuration* createAndAddSorted(
    float              eventSyncSpaceStartPoint, ///< Start point of this event within time synchronisation event space.
    float              eventSyncSpaceMidPoint,   ///< Mid point of this event within time synchronisation event space.
    float              eventSyncSpaceDuration,   ///< As a time synchronisation event fraction.
    float              weight,                   ///<
    uint32_t           userData                  ///<
  );

  /// \brief Initialise a new event (copying from the specified source event) and insert it correctly sorted on start
  /// point into the events list.
  /// \return Pointer to newly added event.
  NM_INLINE EventDuration* copyAndAddSorted(const EventDuration* sourceEvent);

  // ------------- Blending functions -------------
  /// \brief Do not attempt to match events together for blending or rejection just copy all events straight to output.
  void blendPassThrough(
    EventTrackDuration*  clip0,
    EventTrackDuration*  clip1);

  /// \brief Blend or Reject blindly in sequence. If there are remaining events in one track then simply copy them into the result.
  void blendInSequence(
    EventTrackDuration*  clip0,                  ///<
    EventTrackDuration*  clip1,                  ///<
    float                weight,                 ///< Blend weighting.
    float                selectionWeightCutOff,  ///< At what proportional weight do we select one events details over the other.
                                                 ///< Below this value select clip1, above it select clip2.
    uint32_t             blendingOperation,      ///< Operation to perform of events that have been matched up. See DURATION_EVENT_BLEND_OP_TYPE_...
    bool                 rejectNonMatched        ///< If an event from one of the sources has not been matched to one in the other source should we just discard it?
  );

  /// \brief Only consider events with the same user data for Blending or Rejection.
  void blendSameUserData(
    EventTrackDuration*  clip0,
    EventTrackDuration*  clip1,
    float                weight,                 ///< Blend weighting.
    float                selectionWeightCutOff,  ///< At what proportional weight do we select one events details over the other.
                                                 ///< Below this value select clip1, above it select clip2.
    uint32_t             blendingOperation,      ///< Operation to perform of events that have been matched up. See DURATION_EVENT_BLEND_OP_TYPE_...
    bool                 rejectNonMatched        ///< If an event from one of the sources has not been matched to one in the other source should we just discard it?
  );

  /// \brief Blend or Reject only when events are overlapping. If not overlapping simply copy into the result.
  void blendOnOverlap(
    EventTrackDuration*  clip0,
    EventTrackDuration*  clip1,
    float                weight,                 ///< Blend weighting.
    float                selectionWeightCutOff,  ///< At what proportional weight do we select one events details over the other.
                                                 ///< Below this value select clip1, above it select clip2.
    uint32_t             blendingOperation,      ///< Operation to perform of events that have been matched up. See DURATION_EVENT_BLEND_OP_TYPE_...
    bool                 rejectNonMatched        ///< If an event from one of the sources has not been matched to one in the other source should we just discard it?
  );

  /// \brief Blend or Reject only when events are within a specified event range of each other, otherwise put straight into the result.
  void blendWithinRange(
    EventTrackDuration*  clip0,
    EventTrackDuration*  clip1,
    float                weight,                 ///< Blend weighting.
    float                selectionWeightCutOff,  ///< At what proportional weight do we select one events details over the other.
                                                 ///< Below this value select clip1, above it select clip2.
    uint32_t             blendingOperation,      ///< Operation to perform of events that have been matched up. See DURATION_EVENT_BLEND_OP_TYPE_...
    bool                 rejectNonMatched,       ///< If an event from one of the sources has not been matched to one in the other source should we just discard it?
    float                testRange               ///< Within what range do we decide to match events together. Expressed as an event fraction.
  );

  /// \brief In sequence and with the same IDs and if overlapping.
  void blendInSequenceSameUserDataOnOverlap(
    EventTrackDuration*  clip0,
    EventTrackDuration*  clip1,
    float                weight,                 ///< Blend weighting.
    float                selectionWeightCutOff,  ///< At what proportional weight do we select one events details over the other.
                                                 ///< Below this value select clip1, above it select clip2.
    uint32_t             blendingOperation,      ///< Operation to perform of events that have been matched up. See DURATION_EVENT_BLEND_OP_TYPE_...
    bool                 rejectNonMatched        ///< If an event from one of the sources has not been matched to one in the other source should we just discard it?
  );

  /// \brief In sequence and with the same IDs and within a specified range.
  void blendInSequenceSameUserDataWithinRange(
    EventTrackDuration*  clip0,
    EventTrackDuration*  clip1,
    float                weight,                 ///< Blend weighting.
    float                selectionWeightCutOff,  ///< At what proportional weight do we select one events details over the other.
                                                 ///< Below this value select clip1, above it select clip2.
    uint32_t             blendingOperation,      ///< Operation to perform of events that have been matched up. See DURATION_EVENT_BLEND_OP_TYPE_...
    bool                 rejectNonMatched,       ///< If an event from one of the sources has not been matched to one in the other source should we just discard it?
    float                testRange               ///< Within what range do we decide to match events together. Expressed as an event fraction.
  );

  /// \brief In sequence and with the same IDs.
  void blendInSequenceSameUserData(
    EventTrackDuration*  clip0,
    EventTrackDuration*  clip1,
    float                weight,                 ///< Blend weighting.
    float                selectionWeightCutOff,  ///< At what proportional weight do we select one events details over the other.
                                                 ///< Below this value select clip1, above it select clip2.
    uint32_t             blendingOperation,      ///< Operation to perform of events that have been matched up. See DURATION_EVENT_BLEND_OP_TYPE_...
    bool                 rejectNonMatched        ///< If an event from one of the sources has not been matched to one in the other source should we just discard it?
  );

  /// \brief In sequence and if overlapping.
  void blendInSequenceOnOverlap(
    EventTrackDuration*  clip0,
    EventTrackDuration*  clip1,
    float                weight,                 ///< Blend weighting.
    float                selectionWeightCutOff,  ///< At what proportional weight do we select one events details over the other.
                                                 ///< Below this value select clip1, above it select clip2.
    uint32_t             blendingOperation,      ///< Operation to perform of events that have been matched up. See DURATION_EVENT_BLEND_OP_TYPE_...
    bool                 rejectNonMatched        ///< If an event from one of the sources has not been matched to one in the other source should we just discard it?
  );

  /// \brief In sequence and within a specified range.
  void blendInSequenceWithinRange(
    EventTrackDuration*  clip0,
    EventTrackDuration*  clip1,
    float                weight,                 ///< Blend weighting.
    float                selectionWeightCutOff,  ///< At what proportional weight do we select one events details over the other.
                                                 ///< Below this value select clip1, above it select clip2.
    uint32_t             blendingOperation,      ///< Operation to perform of events that have been matched up. See DURATION_EVENT_BLEND_OP_TYPE_...
    bool                 rejectNonMatched,       ///< If an event from one of the sources has not been matched to one in the other source should we just discard it?
    float                testRange               ///< Within what range do we decide to match events together.
                                                 ///< Expressed as an event fraction.
  );

  /// \brief With the same IDs and if overlapping.
  void blendSameUserDataOnOverlap(
    EventTrackDuration*  clip0,
    EventTrackDuration*  clip1,
    float                weight,                 ///< Blend weighting.
    float                selectionWeightCutOff,  ///< At what proportional weight do we select one events details over the other.
                                                 ///< Below this value select clip1, above it select clip2.
    uint32_t             blendingOperation,      ///< Operation to perform of events that have been matched up. See DURATION_EVENT_BLEND_OP_TYPE_...
    bool                 rejectNonMatched        ///< If an event from one of the sources has not been matched to
                                                 ///< one in the other source should we just discard it?
  );

  /// \brief With the same IDs and within a specified range.
  void blendSameUserDataWithinRange(
    EventTrackDuration*  clip0,
    EventTrackDuration*  clip1,
    float                weight,                 ///< Blend weighting.
    float                selectionWeightCutOff,  ///< At what proportional weight do we select one events details
                                                 ///< over the other.
                                                 ///< Below this value select clip1, above it select clip2.
    uint32_t             blendingOperation,      ///< Operation to perform of events that have been matched up.
                                                 ///< See DURATION_EVENT_BLEND_OP_TYPE_...
    bool                 rejectNonMatched,       ///< If an event from one of the sources has not been matched to
                                                 ///< one in the other source should we just discard it?
    float                testRange               ///< Within what range do we decide to match events together.
                                                 ///< Expressed as an event fraction.
  );

protected:
  EventDurationPool* m_pool;            ///< Pool from which this tracks events are allocated.
  uint32_t           m_numEvents;       ///< How many events are recorded in this track.
  uint32_t           m_runtimeID;       ///< Runtime assigned track ID.
  uint32_t           m_userData;        ///< Allows this track to be associated with arbitrary user data.
  uint32_t           m_numSyncEvents;   ///< The number of events in the associated time synchronisation event
                                        ///< track. Duration events are expressed in this space.
  EventIndex         m_eventsListHead;  ///< Head of linked list. Smallest start point.
  EventIndex         m_eventsListTail;  ///< Tail of linked list. Largest start point.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::EventTrackDurationSet
/// \brief
/// \ingroup EventTrackDurationModule
//----------------------------------------------------------------------------------------------------------------------
class EventTrackDurationSet
{
public:
  EventTrackDurationSet() {}
  ~EventTrackDurationSet() {}

  static EventTrackDurationSet* create(
    NMP::MemoryAllocator* allocator,
    uint32_t              numEventTracks,
    uint32_t              eventsPoolSize);

  static NMP::Memory::Format getMemoryRequirements(
    uint32_t              numEventTracks,
    uint32_t              eventsPoolSize);

  static EventTrackDurationSet* init(
    NMP::Memory::Resource& resource,
    uint32_t               numEventTracks,
    uint32_t               eventsPoolSize);

  /// \brief
  void relocate(void* location);
  void relocate() { relocate(this); }

  /// \brief
  EventTrackDurationSet* copyAndPack(NMP::Memory::Resource& resource);

  /// \brief
  void init(
    uint32_t            maxNumEventTracks, ///<
    EventTrackDuration* tracks,            ///<
    EventDurationPool*  eventsPool         ///<
  );

  /// \brief Initialise this set from a set of EventTrackDefDurations'.
  void initFromSourceDef(
    float                   clipStartFraction,      ///< Where to start sampling from the source track.
    float                   clipDurationFraction,   ///< Length of the clip to sample.
    bool                    shouldLoop,             ///< Is this to be a looping clip.
                                                    ///<  Are events allowed to loop beyond the end of the sync event track.
    uint32_t                numEventTrackDefs,      ///< Defines size of durationEventTrackDefs array
    EventTrackDefDuration** durationEventTrackDefs, ///< Event track defs to sample from.
    EventTrackSync*         sourceSyncTrack,        ///< Time synchronisation event space that these duration events
                                                    ///<  are defined within.
    bool                    playBackwards           ///< Initialise this data to be played backwards.
  );

  /// \brief
  ///
  /// This must provide an estimate that will safely encompass the requirements of the blended EventTrackDurationSet.
  static void estimateBlendRequirements(
    EventTrackDurationSet* inputSet0,
    EventTrackDurationSet* inputSet1,
    uint32_t               destSyncTrackEventCount,
    uint32_t&              maxNumTracks,
    uint32_t&              maxNumEvents);

  /// \brief Blend together 2 sets of event tracks and store the result in this set instance.
  ///
  /// Will only blend event tracks that have the same runtimeID and userData.
  void blend(
    EventTrackDurationSet* inputSet0,
    EventTrackDurationSet* inputSet1,
    EventTrackSync*        syncTrack,              ///< Time synchronisation event space within which blended events must be defined.
    float                  weight,                 ///< Blend weighting.
    float                  selectionWeightCutOff,  ///< At what proportional weight do we select one events details over the other.
                                                   ///< Below this value select clip1, above it select clip2.
    uint32_t               matchingOperation,      ///< Whether and how to match up the events from each track.
                                                   ///< See DURATION_EVENT_MATCH_...
    bool                   rejectNonMatched,       ///< If an event from one of the sources has not been matched to one
                                                   ///< in the other source should we just discard it?
    uint32_t               blendOperation,         ///< Operation to perform on events that have been matched up.
                                                   ///< See DURATION_EVENT_BLEND_OP_TYPE_...
    bool                   loopingAllowed,         ///< Are events allowed to loop beyond the end of the sync event track.
    float                  testRange = 1.0f,       ///< Within what range do we decide to match events together.
                                                   ///< When event matching WITHIN_RANGE.
                                                   ///< Expressed as an event fraction.
    uint32_t               inputSet1Offset = 0     ///< Offset of inputSet1 events for blending.
  );

  /// \brief Sample all tracks in this set at the specified time.
  ///
  /// Creates SampledCurveEvent's and inserts them into the provided SampledCurveEventsBuffer.
  void sample(
    SampledCurveEventsBuffer* curveSamplesBuffer,  ///< Curve buffer to add samples too.
    TimeFraction              sampleTime,          ///< As a fraction of the playback duration.
    TimeFraction              prevSampleTime,      ///< As a fraction of the playback duration.
    EventTrackSync*           syncTrack,           ///< Time synchronisation event space within which this set is defined.
    bool                      loopingAllowed       ///< Looping animation.
  );

  /// \brief Sample all tracks in this set at the specified time.
  ///
  /// Creates SampledCurveEvent's and inserts them into the provided SampledCurveEventsBuffer.
  void sample(
    SampledCurveEventsBuffer* curveSamplesBuffer,      ///< Curve buffer to add samples too.
    SyncEventPos*             timeStretchEventPos,     ///< Time synchronisation event position to sample at.
    SyncEventPos*             prevTimeStretchEventPos, ///< Prev time synchronisation event position sampled.
    EventTrackSync*           timeStretchEventSeq,     ///< Time stretch event sequence.
    bool                      loopingAllowed           ///< Looping animation.
  );

  /// \brief Access to the count of tracks in this set.
  uint32_t getNumTracks() const { return m_numEventTracks; }
  void setNumTracks(uint32_t numTracks) { m_numEventTracks = numTracks; }

  /// \brief Access to the count of max num tracks in this set.
  uint32_t getMaxNumTracks() const { return m_maxNumEventTracks; }
  void setMaxNumTracks(uint32_t numTracks) { m_maxNumEventTracks = numTracks; }

  /// \brief Access to the events pool.
  EventDurationPool* getEventsPool() { return m_eventsPool; }

  /// \brief
  NM_INLINE EventTrackDuration* getTrack(uint32_t index) { NMP_ASSERT(index < m_numEventTracks); return &(m_tracks[index]); }
  NM_INLINE const EventTrackDuration* getTrack(uint32_t index) const { NMP_ASSERT(index < m_numEventTracks); return &(m_tracks[index]); }

protected:
  uint32_t            m_numEventTracks;    ///<
  EventTrackDuration* m_tracks;            ///<
  uint32_t            m_maxNumEventTracks; ///<
  EventDurationPool*  m_eventsPool;        ///<
};

//----------------------------------------------------------------------------------------------------------------------
// EventDefDuration functions.
//----------------------------------------------------------------------------------------------------------------------
// Set all members of this Event.
NM_INLINE void EventDefDuration::init(TimeFraction startTime, TimeFraction duration, uint32_t userData)
{
  NMP_ASSERT(startTime >= 0.0f);
  m_startTime = startTime;
  NMP_ASSERT(duration >= 0.0f);
  m_duration = duration;
  m_userData = userData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool EventDefDuration::dislocate()
{
  NMP::endianSwap(m_startTime);
  NMP::endianSwap(m_duration);
  NMP::endianSwap(m_userData);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool EventDefDuration::locate()
{
  NMP::endianSwap(m_startTime);
  NMP::endianSwap(m_duration);
  NMP::endianSwap(m_userData);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// EventTrackDuration functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE EventDuration* EventTrackDuration::createAndAddToTail(
  float    eventSyncSpaceStartPoint,   // Start point of this event within time synchronisation event space.
  float    eventSyncSpaceMidPoint,     // Mid point of this event within time synchronisation event space.
  float    eventSyncSpaceDuration,     // As a time synchronisation event fraction.
  float    weight,                     //
  uint32_t userData)                   //
{
  EventIndex eventIndex;
  EventDuration* eventToAdd = m_pool->allocateEvent(eventIndex);
  eventToAdd->init(eventSyncSpaceStartPoint, eventSyncSpaceMidPoint, eventSyncSpaceDuration, weight, userData);
  ++m_numEvents;
  return addToTail(eventToAdd, eventIndex);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE EventDuration* EventTrackDuration::copyAndAddToTail(const EventDuration* sourceEvent)
{
  NMP_ASSERT(sourceEvent);
  EventIndex eventIndex;
  EventDuration* eventToAdd = m_pool->allocateEvent(eventIndex);
  eventToAdd->copy(sourceEvent);
  ++m_numEvents;
  return addToTail(eventToAdd, eventIndex);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE EventDuration* EventTrackDuration::createAndAddToHead(
  float    eventSyncSpaceStartPoint, // Start point of this event within time synchronisation event space.
  float    eventSyncSpaceMidPoint,   // Mid point of this event within time synchronisation event space.
  float    eventSyncSpaceDuration,   // As a time synchronisation event fraction.
  float    weight,                   //
  uint32_t userData)                 //
{
  EventIndex eventIndex;
  EventDuration* eventToAdd = m_pool->allocateEvent(eventIndex);
  eventToAdd->init(eventSyncSpaceStartPoint, eventSyncSpaceMidPoint, eventSyncSpaceDuration, weight, userData);
  ++m_numEvents;
  return addToHead(eventToAdd, eventIndex);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE EventDuration* EventTrackDuration::copyAndAddToHead(const EventDuration* sourceEvent)
{
  NMP_ASSERT(sourceEvent);
  EventIndex eventIndex;
  EventDuration* eventToAdd = m_pool->allocateEvent(eventIndex);
  eventToAdd->copy(sourceEvent);
  ++m_numEvents;
  return addToHead(eventToAdd, eventIndex);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE EventDuration* EventTrackDuration::createAndAddSorted(
  float    eventSyncSpaceStartPoint, // Start point of this event within time synchronisation event space.
  float    eventSyncSpaceMidPoint,   // Mid point of this event within time synchronisation event space.
  float    eventSyncSpaceDuration,   // As a time synchronisation event fraction.
  float    weight,                   //
  uint32_t userData)                 //
{
  EventIndex eventIndex;
  EventDuration* eventToAdd = m_pool->allocateEvent(eventIndex);
  eventToAdd->init(eventSyncSpaceStartPoint, eventSyncSpaceMidPoint, eventSyncSpaceDuration, weight, userData);
  ++m_numEvents;
  return addSorted(eventToAdd, eventIndex);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE EventDuration* EventTrackDuration::copyAndAddSorted(const EventDuration* sourceEvent)
{
  NMP_ASSERT(sourceEvent);
  EventIndex eventIndex;
  EventDuration* eventToAdd = m_pool->allocateEvent(eventIndex);
  eventToAdd->copy(sourceEvent);
  ++m_numEvents;
  return addSorted(eventToAdd, eventIndex);
}

//----------------------------------------------------------------------------------------------------------------------
// EventDurationPool functions.
//----------------------------------------------------------------------------------------------------------------------
/// \brief Allocate an EventDefDuration from the pool.
NM_INLINE EventDuration* EventDurationPool::allocateEvent(EventIndex& eventIndex)
{
  NMP_ASSERT(m_numAllocated < m_size);
  eventIndex = (EventIndex) m_numAllocated;
  m_numAllocated++;
  return &(m_eventsPool[eventIndex]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE EventDuration* EventDurationPool::getEvent(EventIndex eventIndex)
{
  if (eventIndex == gUndefinedEventIndex)
    return NULL;
  NMP_ASSERT(eventIndex > gUndefinedEventIndex && eventIndex < (EventIndex)m_numAllocated);
  return &(m_eventsPool[eventIndex]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const EventDuration* EventDurationPool::getEvent(EventIndex eventIndex) const
{
  if (eventIndex == gUndefinedEventIndex)
    return NULL;
  NMP_ASSERT(eventIndex > gUndefinedEventIndex && eventIndex < (EventIndex)m_numAllocated);
  return &(m_eventsPool[eventIndex]);
}

//----------------------------------------------------------------------------------------------------------------------
// EventDefDuration functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format EventDuration::getMemoryRequirements()
{
  NMP::Memory::Format result;

  result.size = sizeof(EventDuration);
  result.alignment = NMP_NATURAL_TYPE_ALIGNMENT;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void EventDuration::init(
  float     eventSyncSpaceStartPoint, // Start point of this event within time synchronisation event space.
  float     eventSyncSpaceMidPoint,   // Mid point of this event within time synchronisation event space.
  float     eventSyncSpaceDuration,   // As a time synchronisation event fraction.
  float     weight,                   //
  uint32_t  userData)                 //
{
  NMP_ASSERT_MSG(eventSyncSpaceDuration >= 0.0f, "Duration events cannot have negative duration.");
  
  m_userData = userData;
  m_syncEventSpaceStartPoint = eventSyncSpaceStartPoint;
  m_syncEventSpaceMidPoint = eventSyncSpaceMidPoint;
  m_syncEventSpaceDuration = eventSyncSpaceDuration;
  m_weight = weight;
  m_next = gUndefinedEventIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void EventDuration::copy(const EventDuration* sourceEvent)
{
  NMP_ASSERT(sourceEvent);

  m_userData = sourceEvent->m_userData;
  m_syncEventSpaceStartPoint = sourceEvent->m_syncEventSpaceStartPoint;
  m_syncEventSpaceMidPoint = sourceEvent->m_syncEventSpaceMidPoint;
  m_syncEventSpaceDuration = sourceEvent->m_syncEventSpaceDuration;
  m_weight = sourceEvent->m_weight;
  m_next = gUndefinedEventIndex;
}

//----------------------------------------------------------------------------------------------------------------------
bool locateEventTrackDefDuration(uint32_t assetType, void* assetMemory);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_EVENT_TRACK_DURATION_H
//----------------------------------------------------------------------------------------------------------------------
