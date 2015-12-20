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
#include "physics/Nodes/mrTransitConditionPhysicsMoving.h"
#include "physics/mrPhysicsRig.h"
#include "physics/mrPhysicsAttribData.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------
namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefPhysicsMoving::defDislocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefPhysicsMoving* def = (TransitConditionDefPhysicsMoving*)tcDef;
  NMP::endianSwap(def->m_VelocityThreshold);
  NMP::endianSwap(def->m_AngularVelocityThreshold);
  NMP::endianSwap(def->m_minFrameCountBelowThresholds);
  NMP::endianSwap(def->m_onNotSet);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefPhysicsMoving::defLocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefPhysicsMoving* def = (TransitConditionDefPhysicsMoving*)tcDef;
  NMP::endianSwap(def->m_VelocityThreshold);
  NMP::endianSwap(def->m_AngularVelocityThreshold);
  NMP::endianSwap(def->m_minFrameCountBelowThresholds);
  NMP::endianSwap(def->m_onNotSet);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TransitConditionDefPhysicsMoving::instanceGetMemoryRequirements(
  TransitConditionDef* NMP_UNUSED(tcDef))
{
  return NMP::Memory::Format(
           NMP::Memory::align(
             sizeof(TransitConditionDefPhysicsMoving),
             NMP_NATURAL_TYPE_ALIGNMENT),
           NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
TransitCondition* TransitConditionDefPhysicsMoving::instanceInit(
  TransitConditionDef*   tcDef,
  NMP::Memory::Resource& memRes,
  Network*               NMP_UNUSED(net),
  NodeID                 NMP_UNUSED(sourceNodeID))
{
  NMP_ASSERT(tcDef);

  if (!memRes.ptr)
    return NULL;

  NMP::Memory::Format memReqs = instanceGetMemoryRequirements(tcDef);
  memRes.align(memReqs);
  TransitConditionPhysicsMoving* result = (TransitConditionPhysicsMoving*) memRes.ptr;
  memRes.increment(memReqs);

  result->condSetState(false);
  result->m_type = TRANSCOND_PHYSICS_MOVING_ID;
  result->m_numFramesVelocityConditionSatisfied = 0;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefPhysicsMoving::instanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* tcDef,
  Network*             net,
  NodeID               NMP_UNUSED(smActiveNodeID))
{
  TransitConditionPhysicsMoving*     condition;
  TransitConditionDefPhysicsMoving* conditionDef;

  NMP_ASSERT(tc && tcDef && net);
  condition = (TransitConditionPhysicsMoving*) (tc);
  conditionDef = (TransitConditionDefPhysicsMoving*) (tcDef);
  condition->m_satisfied = false;

  NodeBinEntry* nodeBinEntry = net->getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_PHYSICS_RIG, NETWORK_NODE_ID, INVALID_NODE_ID);
  if (nodeBinEntry)
  {
    AttribDataPhysicsRig* physicsRigAttrib = nodeBinEntry->getAttribData<AttribDataPhysicsRig>();
    if (physicsRigAttrib->m_physicsRig)
    {
      uint32_t numParts = physicsRigAttrib->m_physicsRig->getPhysicsRigDef()->getNumParts();
      float fAverageVelocity = 0.0f;
      float fAverageAngularVelocity = 0.0f;
      for (uint32_t partCount = 0; partCount < numParts; ++partCount)
      {
        PhysicsRig::Part* part = physicsRigAttrib->m_physicsRig->getPart(partCount);
        fAverageVelocity += part->getVel().magnitudeSquared();
        fAverageAngularVelocity += part->getAngVel().magnitudeSquared();
      }

      // this might look a bit awkward but it's obviously more effective than doing this:
      // sqrtf(fAverageVelocity/(float)numParts)< (conditionDef->getVelocityThreshold())
      bool belowThresholds =
        (fAverageVelocity <= (conditionDef->getVelocityThreshold() * conditionDef->getVelocityThreshold() * (float)numParts)) &&
        (fAverageAngularVelocity <= (conditionDef->getAngularVelocityThreshold() * conditionDef->getAngularVelocityThreshold() * (float)numParts));

      // If the below thresholds state matches the onNotSet state then the velocity condition has been satisfied this frame
      if (belowThresholds == conditionDef->getOnNotSet())
      {
        ++condition->m_numFramesVelocityConditionSatisfied;
        // The condition is satisfied when sufficient frames that satisfy the velocity condition have passed.
        condition->m_satisfied = (condition->m_numFramesVelocityConditionSatisfied >= conditionDef->getMinFrameCount());
      }
      else
      {
        condition->m_numFramesVelocityConditionSatisfied = 0;
      }
    }
    else
    {
      condition->m_numFramesVelocityConditionSatisfied = 0;
    }
  }

  return condition->m_satisfied;
}

//----------------------------------------------------------------------------------------------------------------------
void TransitConditionDefPhysicsMoving::instanceReset(
  TransitCondition*    tc,
  TransitConditionDef* NMP_UNUSED(tcDef))
{
  NMP_ASSERT(tc);
  TransitConditionPhysicsMoving* condition = static_cast <TransitConditionPhysicsMoving*> (tc);
  condition->m_numFramesVelocityConditionSatisfied = 0;
  condition->m_satisfied = false;
}

} // namespace MR
//----------------------------------------------------------------------------------------------------------------------
