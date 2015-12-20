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
#ifndef MR_TRANSIT_CONDITION_IN_DURATION_EVENT_H
#define MR_TRANSIT_CONDITION_IN_DURATION_EVENT_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrAttribAddress.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionDefInDurationEvent
/// \brief Transition condition definition for TransitConditionDefInDurationEvent.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitConditionDef
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefInDurationEvent : public TransitConditionDef
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

protected:
  TransitConditionDefInDurationEvent() { m_type = TRANSCOND_IN_DURATION_EVENT_ID; };

  AttribAddress m_sourceNodeSampledEventsAttribAddress; ///< Address of the sample events buffer from the source
                                                        ///< node in the transition.
  uint32_t      m_eventTrackUserData;                   ///< Monitor EventTracks that have this user data assigned to them.
  uint32_t      m_eventUserData;                        ///< Monitor Events that have this user data assigned to them.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionInDurationEvent
/// \brief Condition satisfied when source Nodes duration event is (not) hit.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitCondition
///
/// Can be un-set after this if it continues to be monitored.
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionInDurationEvent: public TransitCondition
{
  friend class TransitConditionDefInDurationEvent;

protected:
  TransitConditionInDurationEvent();
  ~TransitConditionInDurationEvent();

  bool m_firstFrame;
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_IN_DURATION_EVENT_H
//----------------------------------------------------------------------------------------------------------------------
