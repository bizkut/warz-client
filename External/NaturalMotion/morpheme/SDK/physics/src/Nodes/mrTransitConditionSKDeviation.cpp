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
#include "physics/Nodes/mrTransitConditionSKDeviation.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
#include "physics/mrPhysics.h"
#include "physics/mrPhysicsRig.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefSKDeviation::defDislocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefSKDeviation* def = (TransitConditionDefSKDeviation*)tcDef;
  NMP::endianSwap(def->m_deviationTime);
  NMP::endianSwap(def->m_deviationTriggerAmount);
  NMP::endianSwap(def->m_triggerWhenDeviationIsExceeded);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefSKDeviation::defLocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefSKDeviation* def = (TransitConditionDefSKDeviation*)tcDef;
  NMP::endianSwap(def->m_deviationTime);
  NMP::endianSwap(def->m_deviationTriggerAmount);
  NMP::endianSwap(def->m_triggerWhenDeviationIsExceeded);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TransitConditionDefSKDeviation::instanceGetMemoryRequirements(
  TransitConditionDef* NMP_UNUSED(tcDef))
{
  return NMP::Memory::Format(
           NMP::Memory::align(sizeof(TransitConditionDefSKDeviation), NMP_NATURAL_TYPE_ALIGNMENT),
           NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
TransitCondition* TransitConditionDefSKDeviation::instanceInit(
  TransitConditionDef*   tcDef,
  NMP::Memory::Resource& memRes,
  Network*               NMP_UNUSED(networkInstance),
  NodeID                 NMP_UNUSED(sourceNodeID))
{
  NMP_ASSERT(tcDef);

  if (!memRes.ptr)
    return NULL;

  NMP::Memory::Format memReqs = instanceGetMemoryRequirements(tcDef);
  memRes.align(memReqs);
  TransitConditionSKDeviation* result = (TransitConditionSKDeviation*) memRes.ptr;
  memRes.increment(memReqs);

  result->m_satisfied = false;
  result->m_type = TRANSCOND_SK_DEVIATION_ID;
  result->m_timeTriggerDeviationExceeded = 0.0f;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefSKDeviation::instanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* tcDef,
  Network*             network,
  NodeID               NMP_UNUSED(smActiveNodeID))
{
  TransitConditionSKDeviation*    condition;
  TransitConditionDefSKDeviation* conditionDef;

  NMP_ASSERT(tc && tcDef && network);
  condition = (TransitConditionSKDeviation*) (tc);
  conditionDef = (TransitConditionDefSKDeviation*) (tcDef);

  NMP_ASSERT(condition->m_type == TRANSCOND_SK_DEVIATION_ID);

  // Update the condition status.
  condition->m_satisfied = false;

  const MR::PhysicsRig* physicsRig = getPhysicsRig(network);
  if (physicsRig)
  {
    float deviation = physicsRig->getMaxSKDeviation();
    if ((conditionDef->m_triggerWhenDeviationIsExceeded && deviation > conditionDef->m_deviationTriggerAmount) ||
        (!conditionDef->m_triggerWhenDeviationIsExceeded && deviation < conditionDef->m_deviationTriggerAmount))
    {
      condition->m_timeTriggerDeviationExceeded += network->getLastUpdateTimeStep();
    }
    else
    {
      condition->m_timeTriggerDeviationExceeded = 0.0f;
    }
    condition->m_satisfied = condition->m_timeTriggerDeviationExceeded > conditionDef->m_deviationTime;
  }

  return condition->m_satisfied;
}

//----------------------------------------------------------------------------------------------------------------------
void TransitConditionDefSKDeviation::instanceReset(TransitCondition* tc, TransitConditionDef* tcDef)
{
  TransitConditionSKDeviation* condition = (TransitConditionSKDeviation*) (tc);
  condition->m_timeTriggerDeviationExceeded = 0.0f;
  TransitConditionDef::defaultInstanceReset(tc, tcDef);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
