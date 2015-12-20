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
#include "morpheme/TransitConditions/mrTransitConditionControlParamIntGreater.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// TransitConditionDefControlParamFloatGreater functions.
//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefControlParamIntGreater::defDislocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefControlParamIntGreater* def = (TransitConditionDefControlParamIntGreater*)tcDef;
  def->m_cpConnection.endianSwap();
  NMP::endianSwap(def->m_testValue);
  NMP::endianSwap(def->m_orEqual);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefControlParamIntGreater::defLocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefControlParamIntGreater* def = (TransitConditionDefControlParamIntGreater*)tcDef;
  def->m_cpConnection.endianSwap();
  NMP::endianSwap(def->m_testValue);
  NMP::endianSwap(def->m_orEqual);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
TransitCondition* TransitConditionDefControlParamIntGreater::instanceInit(
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
  TransitConditionControlParamIntGreater* result = (TransitConditionControlParamIntGreater*) memRes.ptr;
  memRes.increment(memReqs);

  result->condSetState(false);
  result->m_type = TRANSCOND_CONTROL_PARAM_INT_GREATER_ID;
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefControlParamIntGreater::instanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* tcDef,
  Network*             net,
  NodeID               smActiveNodeID)
{
  NMP_ASSERT(tc && tcDef && net);
  TransitConditionControlParamIntGreater* condition = (TransitConditionControlParamIntGreater*) (tc);
  TransitConditionDefControlParamIntGreater* conditionDef = (TransitConditionDefControlParamIntGreater*) (tcDef);
  const CPConnection* cpConnection = conditionDef->getCPConnection();
  AnimSetIndex animSet = net->getOutputAnimSetIndex(smActiveNodeID);

  // Update the only connected control parameter.
  AttribDataInt* inputCPInt = net->updateInputCPConnection<AttribDataInt>(cpConnection, animSet);

  // Update the condition status.
  condition->m_satisfied = false;
  if (inputCPInt)
  {
    condition->m_satisfied =
      (inputCPInt->m_value > conditionDef->m_testValue) ||
      (conditionDef->m_orEqual && inputCPInt->m_value == conditionDef->m_testValue);
  }

  return condition->m_satisfied;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
