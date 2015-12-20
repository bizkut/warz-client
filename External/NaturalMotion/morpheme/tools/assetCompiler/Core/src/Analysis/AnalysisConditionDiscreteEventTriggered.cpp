// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "Analysis/AnalysisConditionDiscreteEventTriggered.h"
#include "Analysis/AnalysisInterval.h"
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisConditionDiscreteEventTriggeredBuilder::getMemoryRequirements(
  AssetProcessor*             NMP_UNUSED(processor),
  const MR::NetworkDef*       NMP_UNUSED(networkDef),
  const ME::AnalysisNodeExport* NMP_UNUSED(analysisExport))
{
  NMP::Memory::Format result(sizeof(AnalysisConditionDiscreteEventTriggered), NMP_VECTOR_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisCondition* AnalysisConditionDiscreteEventTriggeredBuilder::init(
  NMP::Memory::Resource&      memRes,
  AssetProcessor*             NMP_UNUSED(processor),
  const MR::NetworkDef*       networkDef,
  const ME::AnalysisNodeExport* analysisExport)
{
  NMP_VERIFY(analysisExport);
  const ME::DataBlockExport* analysisNodeDataBlock = analysisExport->getDataBlock();

  NMP::Memory::Format memReqsHdr(sizeof(AnalysisConditionDiscreteEventTriggered), NMP_VECTOR_ALIGNMENT);
  AnalysisConditionDiscreteEventTriggered* result = (AnalysisConditionDiscreteEventTriggered*)memRes.alignAndIncrement(memReqsHdr);

  // Set the network instance handlers
  result->m_instanceInitFn = instanceInit;
  result->m_instanceUpdateFn = instanceUpdate;
  result->m_instanceResetFn = NULL;

  // Trigger event
  analysisNodeDataBlock->readUInt(result->m_eventUserData, "EventUserData");

  // Get the runtime node ID from the source export path name
  std::string sourceNodeName;
  analysisNodeDataBlock->readString(sourceNodeName, "SourceNode");
  MR::NodeID sourceNodeID = networkDef->getRootNodeID();
  if (!sourceNodeName.empty())
  {
    sourceNodeID = networkDef->getNodeIDFromNodeName(sourceNodeName.c_str());
  }
  NMP_VERIFY_MSG(
    sourceNodeID != MR::INVALID_NODE_ID,
    "The Source node for the analysis condition is invalid");

  result->m_sourceNodeSampledEventsAttribAddress.init(
    MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER,
    sourceNodeID,
    MR::INVALID_NODE_ID,
    MR::VALID_FRAME_ANY_FRAME,
    MR::ANIMATION_SET_ANY);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisConditionDiscreteEventTriggeredBuilder::instanceInit(  
  AnalysisCondition* condition,
  MR::Network* network,
  AnalysisInterval* NMP_UNUSED(analysisInterval))
{
  NMP_VERIFY(condition);
  AnalysisConditionDiscreteEventTriggered* target = (AnalysisConditionDiscreteEventTriggered*)condition;

  // Tell network to keep attrib data around after network update so that we can access it.
  if (target->m_sourceNodeSampledEventsAttribAddress.m_owningNodeID != MR::INVALID_NODE_ID)
  {
    network->addPostUpdateAccessAttrib(
      target->m_sourceNodeSampledEventsAttribAddress.m_owningNodeID,
      target->m_sourceNodeSampledEventsAttribAddress.m_semantic,
      1);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisConditionDiscreteEventTriggeredBuilder::instanceUpdate(  
  AnalysisCondition* condition,
  MR::Network* network,
  AnalysisInterval* NMP_UNUSED(analysisInterval),
  uint32_t NMP_UNUSED(frameIndex))
{
  NMP_VERIFY(condition);
  AnalysisConditionDiscreteEventTriggered* target = (AnalysisConditionDiscreteEventTriggered*)condition;

  // Get source node sampled events buffer attrib data.
  MR::AttribDataSampledEvents* sampledEventsAttrib = static_cast<MR::AttribDataSampledEvents*>(
    network->getAttribDataRecurseFilterNodes(target->m_sourceNodeSampledEventsAttribAddress, true));

  bool result = false;
  if (sampledEventsAttrib)
  {
    // Search the triggered discrete events array from the Node for the for the specified userTypeID.
    result = sampledEventsAttrib->m_discreteBuffer->findIndexOfEventWithSourceEventUserData(0, target->m_eventUserData) != INVALID_EVENT_INDEX;
  }

  return result;
}

}
