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
#include "morpheme/TransitConditions/mrTransitConditionInDurationEvent.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefInDurationEvent::defDislocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefInDurationEvent* def = (TransitConditionDefInDurationEvent*)tcDef;
  def->m_sourceNodeSampledEventsAttribAddress.endianSwap();
  NMP::endianSwap(def->m_eventTrackUserData);
  NMP::endianSwap(def->m_eventUserData);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefInDurationEvent::defLocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefInDurationEvent* def = (TransitConditionDefInDurationEvent*)tcDef;
  def->m_sourceNodeSampledEventsAttribAddress.endianSwap();
  NMP::endianSwap(def->m_eventTrackUserData);
  NMP::endianSwap(def->m_eventUserData);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TransitConditionDefInDurationEvent::instanceGetMemoryRequirements(
  TransitConditionDef* NMP_UNUSED(tcDef))
{
  return NMP::Memory::Format(NMP::Memory::align(sizeof(TransitConditionDefInDurationEvent), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
TransitCondition* TransitConditionDefInDurationEvent::instanceInit(
  TransitConditionDef*   tcDef,
  NMP::Memory::Resource& memRes,
  Network*               net,
  NodeID                 sourceNodeID)
{
  NMP_ASSERT(tcDef);
  TransitConditionInDurationEvent* result = NULL;
  TransitConditionDefInDurationEvent* def = static_cast <TransitConditionDefInDurationEvent*> (tcDef);

  if (memRes.ptr)
  {
    NMP::Memory::Format memReqs = instanceGetMemoryRequirements(tcDef);
    memRes.align(memReqs);
    result = (TransitConditionInDurationEvent*) memRes.ptr;
    memRes.increment(memReqs);

    result->condSetState(false);
    result->m_type = TRANSCOND_IN_DURATION_EVENT_ID;
    result->m_firstFrame = true;
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
bool TransitConditionDefInDurationEvent::instanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* tcDef,
  Network*             net,
  NodeID               smActiveNodeID)
{
  TransitConditionInDurationEvent*     condition;
  TransitConditionDefInDurationEvent* conditionDef;

  NMP_ASSERT(tc && tcDef && net);
  condition = (TransitConditionInDurationEvent*) (tc);
  conditionDef = (TransitConditionDefInDurationEvent*) (tcDef);

  // Get source node sampled events buffer attrib data.
  AttribAddress address = *conditionDef->getSourceNodeSampledEventsAttribAddress();
  address.m_validFrame = net->getCurrentFrameNo() - 1;
  address.m_animSetIndex = net->getActiveAnimSetIndex();
  address.m_owningNodeID = smActiveNodeID;
  NodeBinEntry* entry = net->getAttribDataNodeBinEntry(address);

  condition->m_satisfied = false;

  // the condition should not be evaluated on its first active frame because the event track sample data will not yet be available.
  if(condition->m_firstFrame)
  {
    condition->m_firstFrame = false;
    return condition->m_satisfied;
  }

  // Search the sampled curve events array from the Node for the specified track and value change requirements.
  if (entry)
  {
    // Only deal with one sample of the specified type for now. (There could be multiple samples of the same type).
    AttribDataSampledEvents* sampledEventsAttrib = entry->getAttribData<AttribDataSampledEvents>();
    uint32_t searchOffset = 0;
    uint32_t eventIndex = sampledEventsAttrib->m_curveBuffer->findIndexOfSampleWithSourceUserData(
                            searchOffset,
                            conditionDef->getEventTrackUserData(), // Can be USER_DATA_UNSPECIFIED (meaning, any event track user data)
                            conditionDef->getEventUserData());     // Can be USER_DATA_UNSPECIFIED (meaning, any event user data).
    if (eventIndex != INVALID_EVENT_INDEX)
    {
      // An event sample with the specified type requirements has been found.
      const SampledCurveEvent* currentEventSample = sampledEventsAttrib->m_curveBuffer->getSampledEvent(eventIndex);
      NMP_ASSERT(currentEventSample);
      condition->m_satisfied = (currentEventSample->getValue() != 0.0f);
    }
  }

  condition->m_satisfied ^= conditionDef->getInvertFlag();

  return condition->m_satisfied;
}

//----------------------------------------------------------------------------------------------------------------------
void TransitConditionDefInDurationEvent::instanceQueueDeps(
  TransitCondition*    NMP_UNUSED(tc),
  TransitConditionDef* tcDef,
  Network*             net,
  NodeID               smActiveNodeID)
{
  TransitConditionDefInDurationEvent* conditionDef;
  NMP_ASSERT(tcDef);
  conditionDef = (TransitConditionDefInDurationEvent*) (tcDef);

  AttribAddress address = *conditionDef->getSourceNodeSampledEventsAttribAddress();
  address.m_owningNodeID = net->nodeFindGeneratingNodeForSemantic(smActiveNodeID, address.m_semantic, true, INVALID_NODE_ID);
  if (address.m_owningNodeID != INVALID_NODE_ID)
  {
    net->addConditionAttrib(address, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER); // Add attrib as dependent of root task to ensure it is updated next frame.
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TransitConditionDefInDurationEvent::instanceReset(
  TransitCondition*    tc,
  TransitConditionDef* NMP_UNUSED(tcDef))
{
  NMP_ASSERT(tc);
  TransitConditionInDurationEvent* condition = static_cast <TransitConditionInDurationEvent*> (tc);

  condition->m_satisfied = false;
  condition->m_firstFrame = true;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
