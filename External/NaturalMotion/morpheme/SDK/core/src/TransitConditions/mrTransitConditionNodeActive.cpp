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
#include "morpheme/TransitConditions/mrTransitConditionNodeActive.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// TransitConditionDefNodeActive functions.
//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefNodeActive::defDislocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefNodeActive* def = (TransitConditionDefNodeActive*) (tcDef);
  NMP::endianSwap(def->m_nodeID);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefNodeActive::defLocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefNodeActive* def = (TransitConditionDefNodeActive*) (tcDef);
  NMP::endianSwap(def->m_nodeID);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
TransitCondition* TransitConditionDefNodeActive::instanceInit(
  TransitConditionDef*   tcDef,
  NMP::Memory::Resource& memRes,
  MR::Network*           NMP_UNUSED(net),
  NodeID                 NMP_UNUSED(sourceNodeID))
{
  NMP_ASSERT(tcDef);

  if (!memRes.ptr)
    return NULL;

  NMP::Memory::Format memReqs = TransitConditionDef::defaultInstanceGetMemoryRequirements(tcDef);
  memRes.align(memReqs);
  TransitConditionNodeActive* result = (TransitConditionNodeActive*) (memRes.ptr);
  memRes.increment(memReqs);

  result->condSetState(false);
  result->m_type = TRANSCOND_NODE_ACTIVE_ID;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefNodeActive::instanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* tcDef,
  Network*             net,
  NodeID               NMP_UNUSED(smActiveNodeID))
{
  TransitConditionNodeActive*    condition;
  TransitConditionDefNodeActive* conditionDef;

  NMP_ASSERT(tc && tcDef && net);
  condition = (TransitConditionNodeActive*) (tc);
  conditionDef = (TransitConditionDefNodeActive*) (tcDef);

  // query whether the node was active in the previous frame as querying if the node
  // was active this frame will fail due to the update order of conditions.
  FrameCount lastFrameNo = net->getCurrentFrameNo() - 1;
  NodeBin* nodeBin = net->getNodeBin(conditionDef->m_nodeID);
  condition->m_satisfied = (nodeBin->getLastFrameUpdate() == lastFrameNo);
  condition->m_satisfied ^= conditionDef->getInvertFlag();

  return condition->m_satisfied;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
