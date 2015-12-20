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
#include "morpheme/TransitConditions/mrTransitConditionCrossedCurveEventValueDecreasing.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrEventTrackCurve.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefCrossedCurveEventValueDecreasing::defDislocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefCrossedCurveEventValueDecreasing* def = (TransitConditionDefCrossedCurveEventValueDecreasing*)tcDef;
  def->m_sourceNodeSampledEventsAttribAddress.endianSwap();
  NMP::endianSwap(def->m_eventTrackUserData);
  NMP::endianSwap(def->m_eventUserData);
  NMP::endianSwap(def->m_value);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefCrossedCurveEventValueDecreasing::defLocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefCrossedCurveEventValueDecreasing* def = (TransitConditionDefCrossedCurveEventValueDecreasing*) tcDef;
  def->m_sourceNodeSampledEventsAttribAddress.endianSwap();
  NMP::endianSwap(def->m_eventTrackUserData);
  NMP::endianSwap(def->m_eventUserData);
  NMP::endianSwap(def->m_value);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TransitConditionDefCrossedCurveEventValueDecreasing::instanceGetMemoryRequirements(
  TransitConditionDef* NMP_UNUSED(tcDef))
{
  return NMP::Memory::Format(NMP::Memory::align(sizeof(TransitConditionCrossedCurveEventValueDecreasing), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
TransitCondition* TransitConditionDefCrossedCurveEventValueDecreasing::instanceInit(
  TransitConditionDef*   tcDef,
  NMP::Memory::Resource& memRes,
  Network*               net,
  NodeID                 sourceNodeID)
{
  NMP_ASSERT(tcDef);
  TransitConditionCrossedCurveEventValueDecreasing* result = NULL;
  TransitConditionDefCrossedCurveEventValueDecreasing* def = static_cast <TransitConditionDefCrossedCurveEventValueDecreasing*> (tcDef);

  if (memRes.ptr)
  {
    NMP::Memory::Format memReqs = instanceGetMemoryRequirements(tcDef);
    memRes.align(memReqs);
    result = (TransitConditionCrossedCurveEventValueDecreasing*) memRes.ptr;
    memRes.increment(memReqs);

    result->condSetState(false);
    result->m_type = TRANSCOND_CROSSED_CURVE_EVENT_VALUE_DECREASING_ID;
    result->m_lastSampleValue = UNINITIALISED_LAST_SAMPLE_VALUE;
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
bool TransitConditionDefCrossedCurveEventValueDecreasing::instanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* tcDef,
  Network*             net,
  NodeID               smActiveNodeID)
{
  TransitConditionCrossedCurveEventValueDecreasing*    condition;
  TransitConditionDefCrossedCurveEventValueDecreasing* conditionDef;

  NMP_ASSERT(tc && tcDef && net);
  condition = (TransitConditionCrossedCurveEventValueDecreasing*) (tc);
  conditionDef = (TransitConditionDefCrossedCurveEventValueDecreasing*) (tcDef);

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
    // Only deal with one sample of the specified type for now. (There could be multiple samples of the same type).
    uint32_t searchOffset = 0;
    uint32_t eventIndex = sampledEventsAttrib->m_curveBuffer->findIndexOfSampleWithSourceUserData(
                            searchOffset,
                            conditionDef->getEventTrackUserData(),
                            conditionDef->getEventUserData());
    if (eventIndex != INVALID_EVENT_INDEX)
    {
      const SampledCurveEvent* currentEventSample = sampledEventsAttrib->m_curveBuffer->getSampledEvent(eventIndex);
      NMP_ASSERT(currentEventSample);

      // Crossed monitoring value.
      condition->m_satisfied =
        (condition->getLastSampledValue() != UNINITIALISED_LAST_SAMPLE_VALUE) &&
        (condition->getLastSampledValue() > conditionDef->getMonitoringValue()) &&
        (currentEventSample->getValue() <= conditionDef->getMonitoringValue());

      condition->setLastSampledValue(currentEventSample->getValue());
    }
    else
    {
      // No sample this frame.
      condition->setLastSampledValue(UNINITIALISED_LAST_SAMPLE_VALUE);
    }
  }
  else
  {
    condition->setLastSampledValue(UNINITIALISED_LAST_SAMPLE_VALUE);
  }

  return condition->m_satisfied;
}

//----------------------------------------------------------------------------------------------------------------------
void TransitConditionDefCrossedCurveEventValueDecreasing::instanceQueueDeps(
  TransitCondition*    NMP_UNUSED(tc),
  TransitConditionDef* tcDef,
  Network*             net,
  NodeID               smActiveNodeID)
{
  TransitConditionDefCrossedCurveEventValueDecreasing* conditionDef;
  NMP_ASSERT(tcDef);
  conditionDef = (TransitConditionDefCrossedCurveEventValueDecreasing*) (tcDef);

  AttribAddress address = *conditionDef->getSourceNodeSampledEventsAttribAddress();
  address.m_owningNodeID = net->nodeFindGeneratingNodeForSemantic(smActiveNodeID, address.m_semantic, true, INVALID_NODE_ID);
  if (address.m_owningNodeID != INVALID_NODE_ID)
  {
    // Add attrib as dependent of root task to ensure it is updated next frame.
    net->addConditionAttrib(address, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TransitConditionDefCrossedCurveEventValueDecreasing::instanceReset(
  TransitCondition*    tc,
  TransitConditionDef* NMP_UNUSED(tcDef))
{
  NMP_ASSERT(tc);
  TransitConditionCrossedCurveEventValueDecreasing* condition = static_cast <TransitConditionCrossedCurveEventValueDecreasing*> (tc);

  condition->m_satisfied = false;
  condition->m_lastSampleValue = UNINITIALISED_LAST_SAMPLE_VALUE;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
