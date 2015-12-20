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
#ifndef MR_EVENT_TRACK_SYNC_H
#define MR_EVENT_TRACK_SYNC_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "morpheme/mrDefines.h"
#include "morpheme/mrEventTrackDiscrete.h"
#include "morpheme/mrSyncEventPos.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup EventTrackSyncModule Time Synchronisation Event Tracks.
/// \ingroup EventTrackModule
///
/// Sampled from a source EventTrackDiscrete at runtime.
/// Used in Network nodes to control the sampling position of source animations.
/// For example, allowing us to match foot step events in a blend of a walk animation and a limp animation.
//----------------------------------------------------------------------------------------------------------------------

class SyncEventPos;

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::EventTrackSync
/// \brief Sub-divides a normalised unit of time and allows blending with different sub-divisions.
/// \ingroup EventTrackSyncModule
///
/// This data is sampled from a source EventTrackDiscrete at runtime.
/// Events occurring in source tracks can be blended to produce a new source track
/// with event positions interpolated between the 2 originals.
/// This data is used in Network nodes to control the sampling position of source animations.
/// For example, allowing us to match foot step events in a blend of a walk animation and a limp animation.
//----------------------------------------------------------------------------------------------------------------------
class EventTrackSync
{
public:
  EventTrackSync() { wipeDown(); }
  ~EventTrackSync() {}

  /// \brief Check that the number of events to be added from the source discrete track does not exceed MAX_NUM_SYNC_EVENTS.
  static bool validateSourceEventCount(
    TimeFraction           clipStartFraction,       ///< Where to start sampling from the source track.
    TimeFraction           clipDurationFraction,    ///< Length of the clip to sample.
    uint8_t                startDiscreteEventIndex, ///< Which source discrete event does the clipStartFraction lie within.
    EventTrackDefDiscrete* sourceTrack,             ///< Track to sample from.
    const char**           sourceTrackName);        ///< Out: name of the source discrete event track.

  /// \brief Extract an events track clip from a source events track.
  ///
  /// All values are fractional (>= 0.0 and <= 1.0).
  void init(
    TimeFraction           clipStartPosFraction,    ///< Where to start sampling from the source track.
    TimeFraction           clipDurationFraction,    ///< Length of the clip to sample.
    uint8_t                startDiscreteEventIndex, ///< Which source discrete event does the clipStartFraction lie within.
    EventTrackDefDiscrete* sourceTrack,             ///< Track to sample from.
    bool                   shouldLoop,              ///< Is this to be a looping clip (Non looping clips require an extra start event).
    int32_t                startEventIndex,         ///< Which event to use as playback start. Also marks the blend start point.
    float                  clipDurationSeconds,     ///< Duration in seconds.
    bool                   playBackwards            ///< Generate the track as if we were playing the source backwards.
  );

  /// \brief Blend 2 source sync event tracks to populate this one.
  void blendClipTracks(
    const EventTrackSync*  clip1,                ///< Source clip 1.
    const EventTrackSync*  clip2,                ///< Source clip 2.
    float                  blendWeight,          ///< Blend weighting.
    uint32_t               clip2Offset = 0       ///< Offset of clip2 events in clip1 for blending.
  );

  /// \brief Copy a source sync track in to this one.
  void copyClipTrack(
    const EventTrackSync* sourceClip,
    bool                  rebaseFromStartEventIndex); ///< If true the copied sync track will be re-based from the source tracks m_startEventIndex.

  /// \brief Adjust the supplied event position to lie within the sync track range.
  ///
  /// If beyond the range of this track loop or cap the event position appropriately.
  /// Limits event to lie within 0 to numEvents.
  /// \return if loopable & looped true.
  ///         if loopable & not looped false.
  ///         if not loopable & capped true.
  ///         if not loopable & not capped false.
  bool limitToSyncEventSpace(float& eventPos, bool loopable) const;
  bool limitToSyncEventSpace(SyncEventPos& eventPos, bool loopable) const;

  /// \brief Is this sync event position at the end of the last event of this track.
  NM_INLINE bool atEndOfLastEvent(SyncEventPos& eventPos);

  /// \brief convert from one fractional space to another.
  NM_INLINE float convRealFractionToAdjFraction(float realFraction);
  NM_INLINE float convAdjFractionToRealFraction(float adjFraction);

  /// Get the event position of this time position.
  float getAdjPosInfoFromRealFraction(TimeFraction realPlaybackPosFraction) const;
  /// Get the event index and event fraction of this time position.
  void getAdjPosInfoFromRealFraction(
    TimeFraction  realPlaybackPosFraction, ///< In real space.
    uint32_t&     adjEventIndex,           ///< In space adjusted by m_StartEventIndex.
    float&        fractionThroughEvent     ///< In space adjusted by m_StartEventIndex.
  ) const;
  void getAdjPosInfoFromRealFraction(
    TimeFraction  realPlaybackPosFraction, ///< In real space.
    SyncEventPos& adjEventPos              ///< In space adjusted by m_StartEventIndex.
  ) const;

  /// Get the event position of this time position within the space adjusted by the m_StartEventIndex.
  float getAdjPosInfoFromAdjFraction(TimeFraction adjPlaybackPosFraction) const;
  /// Reports clip segment location within the space adjusted by the m_StartEventIndex.
  void getAdjPosInfoFromAdjFraction(
    TimeFraction  adjPlaybackPosFraction,  ///< In space adjusted by m_StartEventIndex.
    uint32_t&     adjEventIndex,           ///< In space adjusted by m_StartEventIndex.
    float&        fractionThroughEvent     ///< In space adjusted by m_StartEventIndex.
  ) const;
  void getAdjPosInfoFromAdjFraction(
    TimeFraction  adjPlaybackPosFraction,  ///< In space adjusted by m_StartEventIndex.
    SyncEventPos& adjEventPos              ///< In space adjusted by m_StartEventIndex.
  ) const;

  /// Get the event position of this time position within the space that is not adjusted by the m_StartEventIndex.
  float getRealPosInfoFromRealFraction(TimeFraction adjPlaybackPosFraction) const;
  /// Reports clip segment location within the space that is not adjusted by the m_StartEventIndex.
  void getRealPosInfoFromRealFraction(
    TimeFraction  realPlaybackPosFraction,  ///< In real space.
    uint32_t&     realEventIndex,           ///< In real space.
    float&        fractionThroughEvent      ///< In real space.
  ) const;
  void getRealPosInfoFromRealFraction(
    TimeFraction  realPlaybackPosFraction,  ///< In real space.
    SyncEventPos& realEventPos              ///< In real space
  ) const;

  /// Reports a time fraction in the space adjusted by the m_StartEventIndex.
  TimeFraction getAdjPosFractionFromAdjSyncEventPos(
    uint32_t        adjEventIndex,        ///< In space adjusted by m_StartEventIndex.
    float           fractionThroughEvent  ///< In space adjusted by m_StartEventIndex.
  ) const;
  TimeFraction getAdjPosFractionFromAdjSyncEventPos(const SyncEventPos& adjEventPos) const;
  TimeFraction getAdjPosFractionFromAdjSyncEventPos(float adjEventPos) const;

  /// Reports a time fraction in the space not adjusted by the m_StartEventIndex.
  TimeFraction getRealPosFractionFromAdjSyncEventPos(
    uint32_t        adjEventIndex,        ///< In space adjusted by m_StartEventIndex.
    float           fractionThroughEvent  ///< In space adjusted by m_StartEventIndex.
  ) const;
  TimeFraction getRealPosFractionFromAdjSyncEventPos(const SyncEventPos& adjEventPos) const;
  TimeFraction getRealPosFractionFromAdjSyncEventPos(float adjEventPos) const;

  /// Reports a time fraction in the space not adjusted by the m_StartEventIndex.
  TimeFraction getRealPosFractionFromRealSyncEventPos(
    uint32_t        realEventIndex,       ///< In real space.
    float           fractionThroughEvent  ///< In real space.
  ) const;
  TimeFraction getRealPosFractionFromRealSyncEventPos(const SyncEventPos& realEventPos) const;
  TimeFraction getRealPosFractionFromRealSyncEventPos(float realEventPos) const;

  void convEventInRealSpaceToAdjustedSpace(const SyncEventPos& realEventPosIn, SyncEventPos& adjEventPosOut) const;
  void convEventInAdjustedSpaceToRealSpace(const SyncEventPos& adjEventPosIn, SyncEventPos& realEventPosOut, uint32_t startOffset = 0) const;
  float convEventInRealSpaceToAdjustedSpace(float realEventPosIn) const;
  float convEventInAdjustedSpaceToRealSpace(float adjEventPosIn, uint32_t startOffset = 0) const;

  NM_INLINE void wipeDown() { m_numEvents = 0; m_duration = 0.0f; m_durationReciprocal = 0.0f; m_startEventIndex = 0; }
  NM_INLINE uint32_t getNumEvents() const { return m_numEvents; }
  NM_INLINE uint32_t getStartEventIndex() const { return m_startEventIndex; }
  NM_INLINE void setStartEventIndex(int32_t index);
  NM_INLINE void addEvent(float startTime, float duration);

  NM_INLINE void setDuration(float duration) { m_duration = duration; }
  NM_INLINE float getDuration() const { return m_duration; }
  NM_INLINE void setDurationReciprocal(float durationReciprocal) { m_durationReciprocal = durationReciprocal; }
  NM_INLINE float getDurationReciprocal() const { return m_durationReciprocal; }

  // Reverse the content of this track. (Used when playing backwards for example).
  void reverse(bool shouldLoop); // Is this to be a looping clip.

  /// \brief Output the sync event track contents to the net log.
  void debugOutputContents(int32_t priority);

  /// \brief
  NM_INLINE EventDefDiscrete* getEvent(uint32_t index) { return m_events + index; }
  NM_INLINE EventDefDiscrete* getEvents() { return m_events; }

  bool locate();
  bool dislocate();

protected:
  uint32_t          m_startEventIndex;              ///< Which event to use as playback start.
                                                    ///< Also marks the blend start point.
  uint32_t          m_numEvents;                    ///<
  EventDefDiscrete  m_events[MAX_NUM_SYNC_EVENTS];  ///<
  float             m_duration;                     ///< Duration of this track in seconds.
  float             m_durationReciprocal;           ///< 1/m_duration. 0.0 if m_duration too small.
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void EventTrackSync::addEvent(float startTime, float duration)
{
  m_events[m_numEvents].setStartTime(startTime);
  m_events[m_numEvents].setDuration(duration);
  ++m_numEvents;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void EventTrackSync::setStartEventIndex(int32_t index)
{
  NMP_ASSERT(m_numEvents > 0);
  while (index < 0)
    index += m_numEvents;
  while ((uint32_t)index >= m_numEvents)
    index -= m_numEvents;

  m_startEventIndex = index;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float EventTrackSync::convRealFractionToAdjFraction(float realFraction)
{
  NMP_ASSERT(realFraction >= 0.0f && realFraction <= 1.0f);
  float adjFraction = realFraction - m_events[m_startEventIndex].getStartTime();
  while (adjFraction < 0.0f)
    adjFraction += 1.0f;
  return adjFraction;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float EventTrackSync::convAdjFractionToRealFraction(float adjFraction)
{
  NMP_ASSERT(adjFraction >= 0.0f && adjFraction <= 1.0f);
  float realFraction = adjFraction + m_events[m_startEventIndex].getStartTime();
  while (realFraction > 1.0f)
    realFraction -= 1.0f;
  return realFraction;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Is this sync event position at the end of the last event of this track.
NM_INLINE bool EventTrackSync::atEndOfLastEvent(SyncEventPos& eventPos)
{
  if ((eventPos.index() == (m_numEvents - 1)) && (eventPos.fraction() == 1.0f))
    return true;
  return false;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_EVENT_TRACK_SYNC_H
//----------------------------------------------------------------------------------------------------------------------
