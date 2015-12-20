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
#include "morpheme/TransitConditions/mrTransitConditionControlParamBoolSet.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// TransitConditionDefControlParamFloatGreater functions.
//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefControlParamBoolSet::defDislocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefControlParamBoolSet* def = (TransitConditionDefControlParamBoolSet*)tcDef;
  def->m_cpConnection.endianSwap();
  NMP::endianSwap(def->m_value);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefControlParamBoolSet::defLocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefControlParamBoolSet* def = (TransitConditionDefControlParamBoolSet*)tcDef;
  def->m_cpConnection.endianSwap();  
  NMP::endianSwap(def->m_value);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
TransitCondition* TransitConditionDefControlParamBoolSet::instanceInit(
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
  TransitConditionControlParamBoolSet* result = (TransitConditionControlParamBoolSet*) memRes.ptr;
  memRes.increment(memReqs);

  result->condSetState(false);
  result->m_type = TRANSCOND_CONTROL_PARAM_BOOL_SET_ID;
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefControlParamBoolSet::instanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* tcDef,
  Network*             net,
  NodeID               smActiveNodeID)
{
  NMP_ASSERT(tc && tcDef && net);
  TransitConditionControlParamBoolSet* condition = (TransitConditionControlParamBoolSet*) (tc);
  TransitConditionDefControlParamBoolSet* conditionDef = (TransitConditionDefControlParamBoolSet*) (tcDef);
  const CPConnection* cpConnection = conditionDef->getCPConnection();
  AnimSetIndex animSet = net->getOutputAnimSetIndex(smActiveNodeID);

  // Update the only connected control parameter.
  AttribDataBool* inputCPInt = net->updateInputCPConnection<AttribDataBool>(cpConnection, animSet);

  // Update the condition status.
  condition->m_satisfied = false;
  if (inputCPInt)
  {
    condition->m_satisfied = (inputCPInt->m_value == conditionDef->m_value);
  }

  return condition->m_satisfied;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
