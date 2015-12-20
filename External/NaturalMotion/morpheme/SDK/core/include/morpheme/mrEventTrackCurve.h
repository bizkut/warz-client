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
#ifndef MR_EVENT_TRACK_CURVE_H
#define MR_EVENT_TRACK_CURVE_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemoryAllocator.h"
#include "NMPlatform/NMMemory.h"
#include "morpheme/mrDefines.h"
#include "morpheme/mrEventTrackBase.h"

//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

static const uint32_t USER_DATA_UNSPECIFIED = 0xFFFFFFFF;

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup EventTrackCurveModule Curve Event Tracks.
/// \ingroup EventTrackModule
///
/// Continuous value curve event tracks.
/// Events are points through which the track curve passes.
/// Curve values are linearly interpolated between event positions.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::EventDefCurve
/// \brief A curve value event; a sequence of which can describe a curve.
/// \ingroup EventTrackCurveModule
///
/// Loaded from a definition file.
//----------------------------------------------------------------------------------------------------------------------
class EventDefCurve
{
public:
  /// \brief Set all members of this Event.
  NM_INLINE void init(float time, float value, uint32_t userData);

  /// \brief Time access.
  float getTime() const { return m_time; }
  void setTime(float time) { NMP_ASSERT(time >= 0.0f && time <= 1.0f); m_time = time; }

  /// \brief Value access.
  float getValue() const { return m_value; }
  void setValue(float value) { NMP_ASSERT(value >= 0.0f); m_value = value; }

  /// \brief User Type access.
  uint32_t getUserData() const { return m_userData; }
  void setUserData(uint32_t userData) { m_userData = userData; }

  NM_INLINE void dislocate();
  NM_INLINE void locate();

protected:
  EventDefCurve() {}
  ~EventDefCurve() {}

  float     m_time;     ///< Where on the time line does this sample occur.
  float     m_value;    ///< Sample value.
  uint32_t  m_userData; ///< Allows this event to be associated with arbitrary user data.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::EventTrackDefCurve
/// \brief Sequence of curve events describing a continuous curve that can be sampled from.
/// \ingroup EventTrackCurveModule
/// \see MR::EventTrackDefBase
///
/// Used to describe an extra channel of interpolated data to be associated with animations.
/// This is source data that is initialised directly from storage.
/// Loaded from a definition file.
//----------------------------------------------------------------------------------------------------------------------
class EventTrackDefCurve : public EventTrackDefBase
{
public:
  bool locate();
  bool dislocate();
  void relocate();

  // Find the requirements of an instance of this track type.
  NMP::Memory::Format getInstanceMemoryRequirements();

  /// \brief Interpolate the 2 event values this time position lies between.
  float getValueAtTime(float time) const;

  /// \brief Returns the event which is used for interpolation in getValueAtTime.
  const EventDefCurve* getEventAtTime(float time) const;

  /// \brief Event access.
  EventDefCurve* getEvent(uint32_t index) { NMP_ASSERT(index < m_numEvents); return &m_events[index]; }

protected:
  EventTrackDefCurve() {}
  ~EventTrackDefCurve() {}

protected:
  EventDefCurve* m_events;   ///< Array of all events.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::SampledCurveEvent
/// \brief
/// \ingroup EventTrackCurveModule
//----------------------------------------------------------------------------------------------------------------------
class SampledCurveEvent
{
public:
  SampledCurveEvent() {}
  ~SampledCurveEvent() {}

  NM_INLINE void locate();
  NM_INLINE void dislocate();

  static NMP::Memory::Format getMemoryRequirements();

  /// \brief
  NM_INLINE void set(
    uint32_t  sourceTrackUserData,   ///<
    uint32_t  sourceTrackRuntimeID,  ///<
    uint32_t  sourceEventUserData,   ///<
    float     value,                 ///<
    float     blendWeight);

  void setSourceEventUserData(uint32_t userData) { m_sourceEventUserData = userData; }
  uint32_t getSourceEventUserData() const { return m_sourceEventUserData; }

  void setSourceTrackRuntimeID(uint32_t runtimeID) { m_sourceTrackRuntimeID = runtimeID; }
  uint32_t getSourceTrackRuntimeID() const { return m_sourceTrackRuntimeID; }

  void setSourceTrackUserData(uint32_t userData) { m_sourceTrackUserData = userData; }
  uint32_t getSourceTrackUserData() const { return m_sourceTrackUserData; }

  void setValue(float value) { m_value = value; }
  float getValue() const { return m_value; }

  NM_INLINE float getBlendWeight() const { return m_blendWeight; }
  NM_INLINE void scaleBlendWeight(float scale)
  {
    NMP_ASSERT(scale >= 0.0f);
    NMP_ASSERT(scale <= 1.0f);
    m_blendWeight *= scale;
  }

  SampledCurveEvent& operator = (const SampledCurveEvent& other)
  {
    this->set(
      other.m_sourceTrackUserData,
      other.m_sourceTrackRuntimeID,
      other.m_sourceEventUserData,
      other.m_value,
      other.m_blendWeight);
    return *this;
  }

protected:
  uint32_t  m_sourceTrackRuntimeID; ///< Runtime assigned event track ID.
  uint32_t  m_sourceTrackUserData;  ///< Arbitrary user data associated with the source event track.
  uint32_t  m_sourceEventUserData;  ///< Arbitrary user data associated with the event that has fired.
  float     m_value;                ///< The sample value from the curve track.
  float     m_blendWeight;          ///< The sample value from the curve track.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::SampledCurveEventsBuffer
/// \brief To hold a buffer of the values of a set of sampled curve event tracks.
/// \ingroup EventTrackCurveModule
//----------------------------------------------------------------------------------------------------------------------
class SampledCurveEventsBuffer
{
public:
  SampledCurveEventsBuffer() {}
  ~SampledCurveEventsBuffer() {}

  void locate();
  void dislocate();
  void relocate(void* location);
  void relocate() { relocate(this); }

  static SampledCurveEventsBuffer* create(NMP::MemoryAllocator* allocator, uint32_t numEvents);
  static NMP::Memory::Format getMemoryRequirements(uint32_t numEvents);
  static SampledCurveEventsBuffer* init(NMP::Memory::Resource& resource, uint32_t numEvents);

  /// \brief
  NM_INLINE void init(uint32_t eventsBufferSize, SampledCurveEvent* eventsBuffer);

  /// \brief
  NM_INLINE void clear() { m_numSamples = 0; }

  /// \brief Returns the number of sampled events in this buffer.
  NM_INLINE uint32_t getNumSampledEvents() const { return m_numSamples; }

  /// \brief Only use this if you understand how the internal management of this class works.
  NM_INLINE void setNumSampledEvents(uint32_t count) { NMP_ASSERT(count <= m_sampleBufferSize); m_numSamples = count; }

  /// \brief Returns the size of the sample buffer.
  NM_INLINE uint32_t getNumSampleBufferSize() const { return m_sampleBufferSize; }

  /// \brief Return the user event type ID of one of the triggered events in the buffer.
  NM_INLINE const SampledCurveEvent* getSampledEvent(uint32_t bufferIndex) const;

  /// \brief Return the user event type ID of one of the triggered events in the buffer.
  NM_INLINE SampledCurveEvent* getSampledEvent(uint32_t bufferIndex);

  /// \brief Sample the provided curve event track at the specified time and add the sample value to the buffer.
  bool sampleCurveTrackAndAddToBuffer(
    const EventTrackDefCurve* eventTrack, ///< Events track to sample from.
    float                     sampleTime, ///< What point to sample at.
    uint32_t                  runtimeID   ///< The runtime ID of the source event track
  );

  /// \brief Set the destination event with the provided values.
  NM_INLINE void addSampledEvent(
    uint32_t sourceTrackUserData,
    uint32_t sourceTrackRuntimeID,
    uint32_t sourceEventUserData,
    float    sourceEventValue);

  /// \brief Set the destination event with the provided values.
  NM_INLINE void addSampledEvent(
    const SampledCurveEvent* sourceEvent);

  /// \brief Combine the 2 source buffers into this one.
  ///
  /// Overwrites any existing content.
  void combine(const SampledCurveEventsBuffer* input0, const SampledCurveEventsBuffer* input1, float weight);

  /// \brief Add the source buffer into this one at indicated index, scale by a factor
  /// \sa fillAt
  ///
  /// Overwrites any existing content.
  uint32_t fillAtAndScale(uint32_t startingIndex, const SampledCurveEventsBuffer* input, const float scale);

  /// \brief Add source buffer to this one
  ///
  /// Adds to any existing content.
  void fill(const SampledCurveEventsBuffer* input);

  /// \brief Search through the m_eventSamples array for a sample with the specified source event user data.
  ///
  /// Several entries may have the same source event user data and this function can be used to find each one in turn.
  /// \return  Index of event within this buffer. INVALID_EVENT_INDEX if not found.
  uint32_t findIndexOfSampleWithSourceEventUserData(
    uint32_t            startingIndex,        ///< Event index to start search from.
    uint32_t            eventUserData         ///< Source event user data to search for.
  ) const;

  /// \brief Search through the m_eventSamples array for a sample with the specified source track user data.
  ///
  /// Several entries may have the same source track user data and this function can be used to find each one in turn.
  /// \return  Index of event within this buffer. INVALID_EVENT_INDEX if not found.
  uint32_t findIndexOfSampleWithSourceUserData(
    uint32_t            startingIndex,        ///< Event index to start search from.
    uint32_t            trackUserData,        ///< Source track user data to search for.
    uint32_t            eventUserData         ///< Source event user data to search for.
  ) const;

  /// \brief Search through the m_eventSamples array for a sample with the specified source track runtime ID.
  ///
  /// Several entries may have the same source track runtime ID and this function can be used to find each one in turn.
  /// \return  Index of event within this buffer. INVALID_EVENT_INDEX if not found.
  uint32_t findIndexOfSampleWithSourceEventTrackRuntimeID(
    uint32_t            startingIndex,        ///< Event index to start search from.
    uint32_t            trackRuntimeID        ///< Source track runtime ID to search for.
  ) const;

protected:
  uint32_t           m_sampleBufferSize;    ///< Array of triggered event types.
  SampledCurveEvent* m_eventSamples;        ///< This array is a pointer into a shared Pool.
  uint32_t           m_numSamples;          ///< The number of available samples that have actually been used.
};

//----------------------------------------------------------------------------------------------------------------------
// SampledCurveEvent functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SampledCurveEvent::locate()
{
  NMP::endianSwap(m_sourceTrackRuntimeID);
  NMP::endianSwap(m_sourceTrackUserData);
  NMP::endianSwap(m_sourceEventUserData);
  NMP::endianSwap(m_value);
  NMP::endianSwap(m_blendWeight);
}
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SampledCurveEvent::dislocate()
{
  NMP::endianSwap(m_sourceTrackRuntimeID);
  NMP::endianSwap(m_sourceTrackUserData);
  NMP::endianSwap(m_sourceEventUserData);
  NMP::endianSwap(m_value);
  NMP::endianSwap(m_blendWeight);
}

//----------------------------------------------------------------------------------------------------------------------
// SampledCurveEventsBuffer functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const SampledCurveEvent* SampledCurveEventsBuffer::getSampledEvent(uint32_t bufferIndex) const
{
  NMP_ASSERT(bufferIndex < m_sampleBufferSize && m_eventSamples);
  return &m_eventSamples[bufferIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE SampledCurveEvent* SampledCurveEventsBuffer::getSampledEvent(uint32_t bufferIndex)
{
  NMP_ASSERT(bufferIndex < m_sampleBufferSize && m_eventSamples);
  return &m_eventSamples[bufferIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SampledCurveEventsBuffer::addSampledEvent(
  uint32_t sourceTrackUserData,
  uint32_t sourceTrackRuntimeID,
  uint32_t sourceEventUserData,
  float    sourceEventValue)
{
  NMP_ASSERT(m_numSamples < m_sampleBufferSize);
  // Default to a blend weight of 1
  m_eventSamples[m_numSamples].set(
    sourceTrackUserData,
    sourceTrackRuntimeID,
    sourceEventUserData,
    sourceEventValue,
    1.0f);
  ++m_numSamples;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SampledCurveEventsBuffer::addSampledEvent(const SampledCurveEvent* sourceEvent)
{
  NMP_ASSERT(m_numSamples < m_sampleBufferSize && sourceEvent);
  m_eventSamples[m_numSamples] = *sourceEvent;
  ++m_numSamples;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SampledCurveEventsBuffer::init(uint32_t eventsBufferSize, SampledCurveEvent* eventsBuffer)
{
  m_sampleBufferSize = eventsBufferSize;
  m_eventSamples = eventsBuffer;
  m_numSamples = 0;
}

//----------------------------------------------------------------------------------------------------------------------
// EventDefCurve functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void EventDefCurve::init(float time, float value, uint32_t userData)
{
  NMP_ASSERT(time >= 0.0f && time <= 1.0f);
  NMP_ASSERT(value >= 0.0f && value <= 1.0f);

  m_time = time;
  m_value = value;
  m_userData = userData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void EventDefCurve::dislocate()
{
  NMP::endianSwap(m_time);
  NMP::endianSwap(m_value);
  NMP::endianSwap(m_userData);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void EventDefCurve::locate()
{
  NMP::endianSwap(m_time);
  NMP::endianSwap(m_value);
  NMP::endianSwap(m_userData);
}

//----------------------------------------------------------------------------------------------------------------------
// SampledCurveEvent functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void SampledCurveEvent::set(
  uint32_t sourceTrackUserData,
  uint32_t sourceTrackRuntimeID,
  uint32_t sourceEventUserData,
  float    value,
  float    blendWeight)
{
  m_sourceTrackUserData   = sourceTrackUserData;
  m_sourceTrackRuntimeID  = sourceTrackRuntimeID;
  m_sourceEventUserData   = sourceEventUserData;
  m_value = value;
  m_blendWeight = blendWeight;
}

//----------------------------------------------------------------------------------------------------------------------
bool locateEventTrackDefCurve(uint32_t assetType, void* assetMemory);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_EVENT_TRACK_CURVE_H
//----------------------------------------------------------------------------------------------------------------------
