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
#ifndef MR_TRANSIT_CONDITION_CROSSED_CURVE_EVENT_VALUE_DECREASING_H
#define MR_TRANSIT_CONDITION_CROSSED_CURVE_EVENT_VALUE_DECREASING_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrAttribAddress.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

static const float UNINITIALISED_LAST_SAMPLE_VALUE = -1.0f;

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionDefCrossedCurveEventValueDecreasing
/// \brief Transition condition definition for TransitConditionCurveEventCrossedValueDecreasing.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitConditionDef
///
/// Becomes true when the sampled value of the specified event track crossed a specified value.
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefCrossedCurveEventValueDecreasing : public TransitConditionDef
{
public:
  static bool defDislocate(TransitConditionDef* tcDef);
  static bool defLocate(TransitConditionDef* tcDef);
  static NMP::Memory::Format instanceGetMemoryRequirements(TransitConditionDef* tcDef);
  static TransitCondition* instanceInit(
    TransitConditionDef*   tcDef,
    NMP::Memory::Resource& memRes,
    Network*               net,
    NodeID                 sourceNodeID);
  static bool instanceUpdate(
    TransitCondition*    tc,
    TransitConditionDef* tcDef,
    Network*             net,
    NodeID               smActiveNodeID);

  static void instanceQueueDeps(
    TransitCondition*    tc,
    TransitConditionDef* tcDef,
    Network*             net,
    NodeID               smActiveNodeID);
  static void instanceReset(TransitCondition* tc, TransitConditionDef* tcDef);

  NM_INLINE const AttribAddress* getSourceNodeSampledEventsAttribAddress() const { return &m_sourceNodeSampledEventsAttribAddress; }
  NM_INLINE void setSourceNodeSampledEventsAttribAddress(AttribAddress& address) { m_sourceNodeSampledEventsAttribAddress = address; }
  NM_INLINE uint32_t getEventTrackUserData() const { return m_eventTrackUserData; }
  NM_INLINE void setEventTrackUserData(uint32_t userTypeID) { m_eventTrackUserData = userTypeID; }
  NM_INLINE uint32_t getEventUserData() const { return m_eventUserData; }
  NM_INLINE void setEventUserData(uint32_t userTypeID) { m_eventUserData = userTypeID; }

  NM_INLINE float getMonitoringValue() const { return m_value; }
  NM_INLINE void setMonitoringValue(float value) { m_value = value; }

protected:
  TransitConditionDefCrossedCurveEventValueDecreasing() { m_type = TRANSCOND_CROSSED_CURVE_EVENT_VALUE_DECREASING_ID; }

  AttribAddress m_sourceNodeSampledEventsAttribAddress; ///< Address of the sample events buffer from the source
                                                        ///< node in the transition.
  uint32_t      m_eventTrackUserData;                   ///< Monitor EventTracks that have this user data assigned to them.
  uint32_t      m_eventUserData;                        ///< Monitor Events that have this user data assigned to them.
  float         m_value;                                ///< Value change to check for.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionCrossedCurveEventValueDecreasing
/// \brief Condition satisfied when the sampled value of the specified event track crossed a specified value.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitCondition
///
/// Can be unset after this if it continues to be monitored.
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionCrossedCurveEventValueDecreasing : public TransitCondition
{
  friend class TransitConditionDefCrossedCurveEventValueDecreasing;

public:
  NM_INLINE float getLastSampledValue() const;
  NM_INLINE void setLastSampledValue(float value);

protected:
  TransitConditionCrossedCurveEventValueDecreasing();
  ~TransitConditionCrossedCurveEventValueDecreasing();

protected:
  float m_lastSampleValue; ///< Set to UNINITIALISED_LAST_SAMPLE_VALUE when a frame has no sample data
                           ///< for the track with this user ID.
};

//----------------------------------------------------------------------------------------------------------------------
// TransitConditionCurveEventCrossedValueDecreasing functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float TransitConditionCrossedCurveEventValueDecreasing::getLastSampledValue() const
{
  return m_lastSampleValue;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TransitConditionCrossedCurveEventValueDecreasing::setLastSampledValue(float value)
{
  m_lastSampleValue = value;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_CROSSED_CURVE_EVENT_VALUE_DECREASING_H
//----------------------------------------------------------------------------------------------------------------------
