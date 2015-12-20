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
#include "morpheme/TransitConditions/mrTransitConditionControlParamFloatInRange.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// TransitConditionDefControlParamFloatInRange functions.
//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefControlParamFloatInRange::defDislocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefControlParamFloatInRange* def = (TransitConditionDefControlParamFloatInRange*)tcDef;
  def->m_cpConnection.endianSwap();
  NMP::endianSwap(def->m_lowerTestValue);
  NMP::endianSwap(def->m_upperTestValue);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefControlParamFloatInRange::defLocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefControlParamFloatInRange* def = (TransitConditionDefControlParamFloatInRange*)tcDef;
  def->m_cpConnection.endianSwap();
  NMP::endianSwap(def->m_lowerTestValue);
  NMP::endianSwap(def->m_upperTestValue);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
TransitCondition* TransitConditionDefControlParamFloatInRange::instanceInit(
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
  TransitConditionControlParamFloatInRange* result = (TransitConditionControlParamFloatInRange*) memRes.ptr;
  memRes.increment(memReqs);

  result->m_satisfied = false;
  result->m_type = TRANSCOND_CONTROL_PARAM_FLOAT_IN_RANGE_ID;
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefControlParamFloatInRange::instanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* tcDef,
  Network*             net,
  NodeID               smActiveNodeID)
{
  NMP_ASSERT(tc && tcDef && net);
  TransitConditionControlParamFloatInRange* condition = (TransitConditionControlParamFloatInRange*) (tc);
  TransitConditionDefControlParamFloatInRange* conditionDef = (TransitConditionDefControlParamFloatInRange*) (tcDef);
  const CPConnection* cpConnection = conditionDef->getCPConnection();
  AnimSetIndex animSet = net->getOutputAnimSetIndex(smActiveNodeID);

  // Update the only connected control parameter.
  AttribDataFloat* inputCPFloat = net->updateInputCPConnection<AttribDataFloat>(cpConnection, animSet);

  // Update the condition status.
  condition->m_satisfied = false;
  if (inputCPFloat)
  {
    bool valueInRange = (inputCPFloat->m_value >= conditionDef->m_lowerTestValue && inputCPFloat->m_value <= conditionDef->m_upperTestValue);
    // same as (valueInRange && !conditionDef->m_invert) || (!valueInRange && conditionDef->m_invert)
    condition->m_satisfied = valueInRange ^ conditionDef->m_invert;
  }

  return condition->m_satisfied;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
