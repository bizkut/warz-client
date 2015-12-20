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
#include "morpheme/TransitConditions/mrTransitConditionControlParamIntInRange.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// TransitConditionDefControlParamFloatInRange functions.
//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefControlParamIntInRange::defDislocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefControlParamIntInRange* def = (TransitConditionDefControlParamIntInRange*)tcDef;
  def->m_cpConnection.endianSwap();
  NMP::endianSwap(def->m_lowerTestValue);
  NMP::endianSwap(def->m_upperTestValue);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefControlParamIntInRange::defLocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefControlParamIntInRange* def = (TransitConditionDefControlParamIntInRange*)tcDef;
  def->m_cpConnection.endianSwap();
  NMP::endianSwap(def->m_lowerTestValue);
  NMP::endianSwap(def->m_upperTestValue);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
TransitCondition* TransitConditionDefControlParamIntInRange::instanceInit(
  TransitConditionDef*   tcDef,
  NMP::Memory::Resource& memRes,
  Network*               NMP_UNUSED(net),
  NodeID                 NMP_UNUSED(sourceNodeID))
{
  NMP_ASSERT(tcDef);

  if (!memRes.ptr)
    return NULL;

  NMP::Memory::Format memReqs = TransitConditionDef::defaultInstanceGetMemoryRequirements(tcDef);
  memRes.align(memReqs);
  TransitConditionControlParamIntInRange* result = (TransitConditionControlParamIntInRange*) memRes.ptr;
  memRes.increment(memReqs);

  result->m_satisfied = false;
  result->m_type = TRANSCOND_CONTROL_PARAM_INT_IN_RANGE_ID;
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefControlParamIntInRange::instanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* tcDef,
  Network*             net,
  NodeID               smActiveNodeID)
{
  NMP_ASSERT(tc && tcDef && net);
  TransitConditionControlParamIntInRange* condition = (TransitConditionControlParamIntInRange*) (tc);
  TransitConditionDefControlParamIntInRange* conditionDef = (TransitConditionDefControlParamIntInRange*) (tcDef);
  const CPConnection* cpConnection = conditionDef->getCPConnection();
  AnimSetIndex animSet = net->getOutputAnimSetIndex(smActiveNodeID);

  // Update the only connected control parameter.
  AttribDataInt* inputCPInt = net->updateInputCPConnection<AttribDataInt>(cpConnection, animSet);

  // Update the condition status.
  condition->m_satisfied = false;
  if (inputCPInt)
  {
    bool valueInRange = (inputCPInt->m_value >= conditionDef->m_lowerTestValue && inputCPInt->m_value <= conditionDef->m_upperTestValue);
    // same as (valueInRange && !conditionDef->m_invert) || (!valueInRange && conditionDef->m_invert)
    condition->m_satisfied = valueInRange ^ conditionDef->m_invert;
  }

  return condition->m_satisfied;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
