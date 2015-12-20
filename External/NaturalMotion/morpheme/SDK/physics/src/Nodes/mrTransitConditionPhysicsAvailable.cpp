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
#include "physics/Nodes/mrTransitConditionPhysicsAvailable.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
#include "physics/mrPhysics.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefPhysicsAvailable::defDislocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefPhysicsAvailable* def = (TransitConditionDefPhysicsAvailable*)tcDef;
  NMP::endianSwap(def->m_onAvailable);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefPhysicsAvailable::defLocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefPhysicsAvailable* def = (TransitConditionDefPhysicsAvailable*)tcDef;
  NMP::endianSwap(def->m_onAvailable);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
TransitCondition* TransitConditionDefPhysicsAvailable::instanceInit(
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
  TransitConditionPhysicsAvailable* result = (TransitConditionPhysicsAvailable*) memRes.ptr;
  memRes.increment(memReqs);

  result->m_satisfied = false;
  result->m_type = TRANSCOND_PHYSICS_AVAILABLE_ID;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefPhysicsAvailable::instanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* tcDef,
  Network*             network,
  NodeID               NMP_UNUSED(smActiveNodeID))
{
  TransitConditionPhysicsAvailable*    condition;
  TransitConditionDefPhysicsAvailable* conditionDef;

  NMP_ASSERT(tc && tcDef && network);
  condition = (TransitConditionPhysicsAvailable*) (tc);
  conditionDef = (TransitConditionDefPhysicsAvailable*) (tcDef);

  // Update the condition status.
  bool physicsAvailable = getPhysicsRig(network) != 0;
  // same as (conditionDef->getOnAvailable() && physicsAvailable) || (!conditionDef->getOnAvailable() && !physicsAvailable)
  condition->m_satisfied = !(conditionDef->getOnAvailable() ^ physicsAvailable);

  return condition->m_satisfied;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
