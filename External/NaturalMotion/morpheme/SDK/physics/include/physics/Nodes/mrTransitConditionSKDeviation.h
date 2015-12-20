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
#ifndef MR_TRANSIT_CONDITION_SK_DEVIATION_H
#define MR_TRANSIT_CONDITION_SK_DEVIATION_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionDefSKDeviation
/// \brief Transition condition definition for TransitCondititionSKDeviation.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitConditionDef
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefSKDeviation : public TransitConditionDef
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
  static void instanceReset(TransitCondition* tc, TransitConditionDef* tcDef);
  NM_INLINE float getDeviationTime() const { return m_deviationTime; }
  NM_INLINE void setDeviationTime(float deviationTime) { m_deviationTime = deviationTime; }
  NM_INLINE float getDeviationTriggerAmount() const { return m_deviationTriggerAmount; }
  NM_INLINE void setDeviationTriggerAmount(float deviationTriggerAmount)
  {
    m_deviationTriggerAmount = deviationTriggerAmount;
  }
  NM_INLINE bool getTriggerWhenDeviationIsExceeded() const { return m_triggerWhenDeviationIsExceeded; }
  NM_INLINE void setTriggerWhenDeviationIsExceeded(bool triggerWhenDeviationIsExceeded)
  {
    m_triggerWhenDeviationIsExceeded = triggerWhenDeviationIsExceeded;
  }

protected:
  TransitConditionDefSKDeviation() { m_type = TRANSCOND_SK_DEVIATION_ID; }

  float m_deviationTime;
  float m_deviationTriggerAmount;
  bool  m_triggerWhenDeviationIsExceeded;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionSKDeviation
/// \brief Condition satisfied when SK deviation from the target exceeds a trigger amount.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitCondition
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionSKDeviation : public TransitCondition
{
  friend class TransitConditionDefSKDeviation;

protected:
  TransitConditionSKDeviation();
  ~TransitConditionSKDeviation();

  float m_timeTriggerDeviationExceeded;
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_SK_DEVIATION_H
//----------------------------------------------------------------------------------------------------------------------
