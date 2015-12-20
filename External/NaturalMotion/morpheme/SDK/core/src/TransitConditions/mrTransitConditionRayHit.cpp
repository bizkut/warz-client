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
#include "morpheme/TransitConditions/mrTransitConditionRayHit.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrCharacterControllerInterface.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefRayHit::defDislocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefRayHit* def = (TransitConditionDefRayHit*)tcDef;
  NMP::endianSwap(def->m_hitMode);
  NMP::endianSwap(def->m_useLocalOrientation);
  NMP::endianSwap(def->m_rayStartX);
  NMP::endianSwap(def->m_rayStartY);
  NMP::endianSwap(def->m_rayStartZ);
  NMP::endianSwap(def->m_rayDeltaX);
  NMP::endianSwap(def->m_rayDeltaY);
  NMP::endianSwap(def->m_rayDeltaZ);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefRayHit::defLocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefRayHit* def = (TransitConditionDefRayHit*)tcDef;
  NMP::endianSwap(def->m_hitMode);
  NMP::endianSwap(def->m_useLocalOrientation);
  NMP::endianSwap(def->m_rayStartX);
  NMP::endianSwap(def->m_rayStartY);
  NMP::endianSwap(def->m_rayStartZ);
  NMP::endianSwap(def->m_rayDeltaX);
  NMP::endianSwap(def->m_rayDeltaY);
  NMP::endianSwap(def->m_rayDeltaZ);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
TransitCondition* TransitConditionDefRayHit::instanceInit(
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
  TransitConditionRayHit* result = (TransitConditionRayHit*) memRes.ptr;
  memRes.increment(memReqs);

  result->m_satisfied = false;
  result->m_type = TRANSCOND_RAY_HIT_ID;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefRayHit::instanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* tcDef,
  Network*             network,
  NodeID               NMP_UNUSED(smActiveNodeID))
{
  TransitConditionRayHit*    condition;
  TransitConditionDefRayHit* conditionDef;

  NMP_ASSERT(tc && tcDef && network);
  condition = (TransitConditionRayHit*) (tc);
  conditionDef = (TransitConditionDefRayHit*) (tcDef);

  // Update the condition status.
  condition->m_satisfied = false;

  MR::CharacterControllerInterface* cc = network->getCharacterController();
  if (cc)
  {
    NMP::Vector3 rayStart;
    rayStart.set(conditionDef->m_rayStartX, conditionDef->m_rayStartY, conditionDef->m_rayStartZ);
    NMP::Vector3 rayDelta;
    rayDelta.set(conditionDef->m_rayDeltaX, conditionDef->m_rayDeltaY, conditionDef->m_rayDeltaZ);

    NMP::Matrix34 t = network->getCharacterPropertiesWorldRootTransform();
    NMP::Vector3 start;
    t.transformVector(rayStart, start);
    NMP::Vector3 delta = rayDelta;
    if (conditionDef->m_useLocalOrientation)
      t.rotateVector(rayDelta, delta);

    float hitDist;
    NMP::Vector3 hitPosition;
    NMP::Vector3 hitNormal;
    NMP::Vector3 hitVelocity;
    bool rayCastHit = network->getCharacterController()->castRayIntoCollisionWorld(
                        start,
                        delta,
                        hitDist,
                        hitPosition,
                        hitNormal,
                        hitVelocity,
                        network);

    switch (conditionDef->m_hitMode)
    {
      case 0:
        condition->m_satisfied = !rayCastHit;
        break;
      case 1:
        condition->m_satisfied = rayCastHit;
        break;
      case 2:
        condition->m_satisfied = rayCastHit && hitVelocity.magnitudeSquared() != 0.0f;
        break;
      case 3:
        condition->m_satisfied = rayCastHit && hitVelocity.magnitudeSquared() == 0.0f;
        break;
      default:
        condition->m_satisfied = false;
        NMP_ASSERT_FAIL();
        break;
    }
  }

  return condition->m_satisfied;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
