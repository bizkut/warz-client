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
#include "physics/Nodes/mrTransitConditionPhysicsInUse.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
#include "physics/mrPhysics.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefPhysicsInUse::defDislocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefPhysicsInUse* def = (TransitConditionDefPhysicsInUse*)tcDef;
  NMP::endianSwap(def->m_onInUse);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefPhysicsInUse::defLocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefPhysicsInUse* def = (TransitConditionDefPhysicsInUse*)tcDef;
  NMP::endianSwap(def->m_onInUse);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
TransitCondition* TransitConditionDefPhysicsInUse::instanceInit(
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
  TransitConditionPhysicsInUse* result = (TransitConditionPhysicsInUse*) memRes.ptr;
  memRes.increment(memReqs);

  result->m_satisfied = false;
  result->m_type = TRANSCOND_PHYSICS_IN_USE_ID;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefPhysicsInUse::instanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* tcDef,
  Network*             network,
  NodeID               NMP_UNUSED(smActiveNodeID))
{
  TransitConditionPhysicsInUse*    condition;
  TransitConditionDefPhysicsInUse* conditionDef;

  NMP_ASSERT(tc && tcDef && network);
  condition = (TransitConditionPhysicsInUse*) (tc);
  conditionDef = (TransitConditionDefPhysicsInUse*) (tcDef);

  // Update the condition status.
  bool physicsAvailable = getPhysicsRig(network) && getPhysicsRig(network)->isReferenced();
  // same as (conditionDef->getOnInUse() && physicsAvailable) || (!conditionDef->getOnInUse() && !physicsAvailable)
  condition->m_satisfied = !(conditionDef->getOnInUse() ^ physicsAvailable);

  return condition->m_satisfied;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
