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
#ifndef MR_TRANSIT_CONDITION_DISCRETE_EVENT_TRIGGERED_H
#define MR_TRANSIT_CONDITION_DISCRETE_EVENT_TRIGGERED_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrAttribAddress.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionDefDiscreteEventTriggered
/// \brief Transition condition definition for TransitConditionDiscreteEventTriggered.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitConditionDef
///
/// Becomes true when the specified event is triggered.
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefDiscreteEventTriggered : public TransitConditionDef
{
public:
  static bool defDislocate(TransitConditionDef* tcDef);
  static bool defLocate(TransitConditionDef* tcDef);
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

  NM_INLINE const AttribAddress* getSourceNodeSampledEventsAttribAddress() const { return &m_sourceNodeSampledEventsAttribAddress; }
  NM_INLINE void setSourceNodeSampledEventsAttribAddress(AttribAddress& address) { m_sourceNodeSampledEventsAttribAddress = address; }
  NM_INLINE uint32_t getEventUserDataTrigger() const { return m_eventUserData; }
  NM_INLINE void setEventUserDataTrigger(uint32_t eventUserDat) { m_eventUserData = eventUserDat; }

protected:
  TransitConditionDefDiscreteEventTriggered() { m_type = TRANSCOND_DISCRETE_EVENT_TRIGGERED_ID; }

  AttribAddress m_sourceNodeSampledEventsAttribAddress; ///< Address of the sample events buffer from the source
                                                        ///< node in the transition.
  uint32_t      m_eventUserData;                        ///< ID to monitor for.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionDiscreteEventTriggered
/// \brief Condition satisfied when source Node playback triggers a discrete event.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitCondition
///
/// Becomes true when the specified event is triggered in the Node that is being watched.
/// Can be unset after this if it continues to be monitored.
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDiscreteEventTriggered : public TransitCondition
{
  friend class TransitConditionDefDiscreteEventTriggered;

protected:
  TransitConditionDiscreteEventTriggered();
  ~TransitConditionDiscreteEventTriggered();

protected:

};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_DISCRETE_EVENT_TRIGGERED_H
//----------------------------------------------------------------------------------------------------------------------
