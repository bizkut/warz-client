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
#include "morpheme/TransitConditions/mrTransitConditionControlParamUIntGreater.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// TransitConditionDefControlParamUIntGreater functions.
//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefControlParamUIntGreater::defDislocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefControlParamUIntGreater* def = (TransitConditionDefControlParamUIntGreater*)tcDef;
  def->m_cpConnection.endianSwap();
  NMP::endianSwap(def->m_testValue);
  NMP::endianSwap(def->m_orEqual);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefControlParamUIntGreater::defLocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefControlParamUIntGreater* def = (TransitConditionDefControlParamUIntGreater*)tcDef;
  def->m_cpConnection.endianSwap();
  NMP::endianSwap(def->m_testValue);
  NMP::endianSwap(def->m_orEqual);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
TransitCondition* TransitConditionDefControlParamUIntGreater::instanceInit(
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
  TransitConditionControlParamUIntGreater* result = (TransitConditionControlParamUIntGreater*) memRes.ptr;
  memRes.increment(memReqs);

  result->condSetState(false);
  result->m_type = TRANSCOND_CONTROL_PARAM_UINT_GREATER_ID;
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefControlParamUIntGreater::instanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* tcDef,
  Network*             net,
  NodeID               smActiveNodeID)
{
  NMP_ASSERT(tc && tcDef && net);
  TransitConditionControlParamUIntGreater* condition = (TransitConditionControlParamUIntGreater*) (tc);
  TransitConditionDefControlParamUIntGreater* conditionDef = (TransitConditionDefControlParamUIntGreater*) (tcDef);
  const CPConnection* cpConnection = conditionDef->getCPConnection();
  AnimSetIndex animSet = net->getOutputAnimSetIndex(smActiveNodeID);

  // Update the only connected control parameter.
  AttribDataUInt* inputCPUInt = net->updateInputCPConnection<AttribDataUInt>(cpConnection, animSet);

  // Update the condition status.
  condition->m_satisfied = false;
  if (inputCPUInt)
  {
    condition->m_satisfied =
      (inputCPUInt->m_value > conditionDef->m_testValue) ||
      (conditionDef->m_orEqual && inputCPUInt->m_value == conditionDef->m_testValue);
  }

  return condition->m_satisfied;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
