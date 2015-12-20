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
#include "physics/Nodes/mrTransitConditionGroundContact.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrCharacterControllerInterface.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefGroundContact::defDislocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefGroundContact* def = (TransitConditionDefGroundContact*)tcDef;
  NMP::endianSwap(def->m_triggerTime);
  NMP::endianSwap(def->m_onGround);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefGroundContact::defLocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefGroundContact* def = (TransitConditionDefGroundContact*)tcDef;
  NMP::endianSwap(def->m_triggerTime);
  NMP::endianSwap(def->m_onGround);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
TransitCondition* TransitConditionDefGroundContact::instanceInit(
  TransitConditionDef*   tcDef,
  NMP::Memory::Resource& memRes,
  Network*               NMP_UNUSED(networkInstance),
  NodeID                 NMP_UNUSED(sourceNodeID))
{
  NMP_ASSERT(tcDef);

  if (!memRes.ptr)
    return NULL;

  NMP::Memory::Format memReqs = TransitConditionDef::defaultInstanceGetMemoryRequirements(tcDef);
  memRes.align(memReqs);
  TransitConditionGroundContact* result = (TransitConditionGroundContact*) memRes.ptr;
  memRes.increment(memReqs);

  result->m_satisfied = false;
  result->m_type = TRANSCOND_GROUND_CONTACT_ID;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefGroundContact::instanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* tcDef,
  Network*             network,
  NodeID               NMP_UNUSED(smActiveNodeID))
{
  TransitConditionGroundContact*    condition;
  TransitConditionDefGroundContact* conditionDef;

  NMP_ASSERT(tc && tcDef && network);
  condition = (TransitConditionGroundContact*) (tc);
  conditionDef = (TransitConditionDefGroundContact*) (tcDef);

  // Update the condition status.
  condition->m_satisfied = false;

  MR::CharacterControllerInterface* cc = network->getCharacterController();
  if (cc)
  {
    float time = network->getCharacterPropertiesAttribData()->m_groundContactTime;

    condition->m_satisfied =
      (conditionDef->m_onGround && time > conditionDef->m_triggerTime) ||
      (!conditionDef->m_onGround && -time > conditionDef->m_triggerTime);
  }

  return condition->m_satisfied;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
