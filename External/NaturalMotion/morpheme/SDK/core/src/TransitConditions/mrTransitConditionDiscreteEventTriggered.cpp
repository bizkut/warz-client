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
#include "morpheme/TransitConditions/mrTransitConditionDiscreteEventTriggered.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefDiscreteEventTriggered::defDislocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefDiscreteEventTriggered* def = (TransitConditionDefDiscreteEventTriggered*)tcDef;
  NMP::endianSwap(def->m_eventUserData);
  def->m_sourceNodeSampledEventsAttribAddress.endianSwap();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefDiscreteEventTriggered::defLocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefDiscreteEventTriggered* def = (TransitConditionDefDiscreteEventTriggered*)tcDef;
  NMP::endianSwap(def->m_eventUserData);
  def->m_sourceNodeSampledEventsAttribAddress.endianSwap();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
TransitCondition* TransitConditionDefDiscreteEventTriggered::instanceInit(
  TransitConditionDef*   tcDef,
  NMP::Memory::Resource& memRes,
  Network*               net,
  NodeID                 sourceNodeID)
{
  NMP_ASSERT(tcDef);
  TransitConditionDiscreteEventTriggered* result = NULL;
  TransitConditionDefDiscreteEventTriggered* def = static_cast <TransitConditionDefDiscreteEventTriggered*> (tcDef);

  if (memRes.ptr)
  {
    NMP::Memory::Format memReqs = TransitConditionDef::defaultInstanceGetMemoryRequirements(tcDef);
    memRes.align(memReqs);
    result = (TransitConditionDiscreteEventTriggered*) memRes.ptr;
    memRes.increment(memReqs);

    result->condSetState(false);
    result->m_type = TRANSCOND_DISCRETE_EVENT_TRIGGERED_ID;
  }
  else
  {
    // Tell network to keep attrib data around after network update so that we can access it.
    NMP_ASSERT(sourceNodeID != INVALID_NODE_ID);
    AttribAddress address = *def->getSourceNodeSampledEventsAttribAddress();
    net->addPostUpdateAccessAttrib(sourceNodeID, address.m_semantic, 2);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefDiscreteEventTriggered::instanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* tcDef,
  Network*             net,
  NodeID               smActiveNodeID)
{
  NMP_ASSERT(tc && tcDef && net);
  TransitConditionDiscreteEventTriggered* condition = (TransitConditionDiscreteEventTriggered*) (tc);
  TransitConditionDefDiscreteEventTriggered* conditionDef = (TransitConditionDefDiscreteEventTriggered*) (tcDef);

  // Get source node sampled events buffer attrib data.
  AttribAddress address = *conditionDef->getSourceNodeSampledEventsAttribAddress();
  address.m_validFrame = net->getCurrentFrameNo() - 1;
  address.m_animSetIndex = net->getActiveAnimSetIndex();
  address.m_owningNodeID = smActiveNodeID;
  AttribDataSampledEvents* sampledEventsAttrib = (AttribDataSampledEvents*) net->getAttribDataRecurseFilterNodes(address, true);

  // Search the sampled curve events array from the Node for the specified track and value change requirements.
  condition->m_satisfied = false;
  if (sampledEventsAttrib)
  {
    // Search the triggered discrete events array from the Node for the for the specified userTypeID.
    condition->m_satisfied = sampledEventsAttrib->m_discreteBuffer->findIndexOfEventWithSourceEventUserData(0, conditionDef->m_eventUserData) != INVALID_EVENT_INDEX;
  }

  return condition->m_satisfied;
}

//----------------------------------------------------------------------------------------------------------------------
void TransitConditionDefDiscreteEventTriggered::instanceQueueDeps(
  TransitCondition*    NMP_UNUSED(tc),
  TransitConditionDef* tcDef,
  Network*             net,
  NodeID               smActiveNodeID)
{
  NMP_ASSERT(tcDef && net);
  TransitConditionDefDiscreteEventTriggered* conditionDef = (TransitConditionDefDiscreteEventTriggered*) (tcDef);

  // Get source node sampled events buffer attrib data.
  AttribAddress address = *conditionDef->getSourceNodeSampledEventsAttribAddress();
  address.m_owningNodeID = net->nodeFindGeneratingNodeForSemantic(smActiveNodeID, address.m_semantic, true, INVALID_NODE_ID);
  if (address.m_owningNodeID != INVALID_NODE_ID)
  {
    net->addConditionAttrib(address, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER); // Add attrib as dependent of root task to ensure it is updated next frame.
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
