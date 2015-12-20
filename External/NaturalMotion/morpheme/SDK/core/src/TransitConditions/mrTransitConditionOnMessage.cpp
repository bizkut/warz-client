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
#include "morpheme/TransitConditions/mrTransitConditionOnMessage.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefOnMessage::defDislocate(TransitConditionDef* tcDef)
{
  TransitConditionDefOnMessage* def = (TransitConditionDefOnMessage*)tcDef;
  NMP::endianSwap(def->m_messageID);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefOnMessage::defLocate(TransitConditionDef* tcDef)
{
  TransitConditionDefOnMessage* def = (TransitConditionDefOnMessage*)tcDef;
  NMP::endianSwap(def->m_messageID);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Initialise this instance from the supplied TransitConditionDef
TransitCondition* TransitConditionDefOnMessage::instanceInit(
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
  TransitConditionOnRequest* result = (TransitConditionOnRequest*) memRes.ptr;
  memRes.increment(memReqs);

  result->m_type = TRANSCOND_ON_MESSAGE_ID;
  instanceSetState(result, tcDef, false);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void TransitConditionDefOnMessage::instanceReset(TransitCondition* tc, TransitConditionDef* tcDef)
{
  instanceSetState(tc, tcDef, false);
}

//----------------------------------------------------------------------------------------------------------------------
void TransitConditionDefOnMessage::instanceSetState(TransitCondition* tc, TransitConditionDef* tcDef, bool state)
{
  NMP_ASSERT(tc && tcDef);
  TransitConditionOnRequest*    condition = (TransitConditionOnRequest*) (tc);
  TransitConditionDefOnMessage* conditionDef = (TransitConditionDefOnMessage*) (tcDef);

  // same as (conditionDef->m_onNotSet && !state) || (!conditionDef->m_onNotSet && state)
  condition->m_satisfied = (conditionDef->getInvertFlag() ^ state);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
