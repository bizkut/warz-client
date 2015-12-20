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
#include "Analysis/AnalysisConditionCrossedDurationFraction.h"
#include "Analysis/AnalysisInterval.h"
#include "assetProcessor/NodeBuilderUtils.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/TransitConditions/mrTransitConditionCrossedDurationFraction.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisConditionCrossedDurationFractionBuilder::getMemoryRequirements(
  AssetProcessor*             NMP_UNUSED(processor),
  const MR::NetworkDef*       NMP_UNUSED(networkDef),
  const ME::AnalysisNodeExport* NMP_UNUSED(analysisExport))
{
  NMP::Memory::Format result(sizeof(AnalysisConditionCrossedDurationFraction), NMP_VECTOR_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisCondition* AnalysisConditionCrossedDurationFractionBuilder::init(
  NMP::Memory::Resource&      memRes,
  AssetProcessor*             NMP_UNUSED(processor),
  const MR::NetworkDef*       networkDef,
  const ME::AnalysisNodeExport* analysisExport)
{
  NMP_VERIFY(analysisExport);
  const ME::DataBlockExport* analysisNodeDataBlock = analysisExport->getDataBlock();

  NMP::Memory::Format memReqsHdr(sizeof(AnalysisConditionCrossedDurationFraction), NMP_VECTOR_ALIGNMENT);
  AnalysisConditionCrossedDurationFraction* result = (AnalysisConditionCrossedDurationFraction*)memRes.alignAndIncrement(memReqsHdr);
  
  // Set the network instance handlers
  result->m_instanceInitFn = instanceInit;
  result->m_instanceUpdateFn = instanceUpdate;
  result->m_instanceResetFn = instanceReset;

  // Trigger fraction
  analysisNodeDataBlock->readFloat(result->m_triggerFraction, "TriggerFraction");

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

  result->m_sourceNodeFractionalPosAttribAddress.init(
    MR::ATTRIB_SEMANTIC_FRACTION_POS,
    sourceNodeID,
    MR::INVALID_NODE_ID,
    MR::VALID_FRAME_ANY_FRAME,
    MR::ANIMATION_SET_ANY);

  // The source that generates the fractional position data
  result->m_sourceNodeID = sourceNodeID;

  // Internal state
  result->m_lastUpdateTimeFraction = -1.0f;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisConditionCrossedDurationFractionBuilder::instanceInit(  
  AnalysisCondition* condition,
  MR::Network* network,
  AnalysisInterval* NMP_UNUSED(analysisInterval))
{
  NMP_VERIFY(condition);
  AnalysisConditionCrossedDurationFraction* target = (AnalysisConditionCrossedDurationFraction*)condition;

  // Tell network to keep attrib data around after network update so that we can access it.
  if (target->m_sourceNodeFractionalPosAttribAddress.m_owningNodeID != MR::INVALID_NODE_ID)
  {
    // Check if this node handles event matching
    MR::NodeID sourceNode = network->nodeFindGeneratingNodeForSemantic(
      target->m_sourceNodeFractionalPosAttribAddress.m_owningNodeID,
      MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS,
      true,
      target->m_sourceNodeFractionalPosAttribAddress.m_owningNodeID);

    // The runtime will update the time independently on each child node. However, there will be
    // no sync event track and fractional position for the source node. Instead we should pass
    // through on child 0.
    if (sourceNode == MR::INVALID_NODE_ID)
    {
      sourceNode = target->m_sourceNodeFractionalPosAttribAddress.m_owningNodeID;
      while (sourceNode)
      {
        MR::NodeConnections* connections = network->getActiveNodesConnections(sourceNode);
        NMP_VERIFY(connections);
        NMP_VERIFY_MSG(connections->m_numActiveChildNodes > 0,
          "The source node %d does not match events and can't generate a fractional position",
          target->m_sourceNodeFractionalPosAttribAddress.m_owningNodeID);
        if (connections->m_numActiveChildNodes == 0)
          return;

        // Pass through on child 0
        sourceNode = connections->m_activeChildNodeIDs[0];

        if (network->nodeFindGeneratingNodeForSemantic(
          sourceNode,
          target->m_sourceNodeFractionalPosAttribAddress.m_semantic,
          true,
          target->m_sourceNodeFractionalPosAttribAddress.m_owningNodeID))
        {
          break;
        }
      }
    }
    target->m_sourceNodeID = sourceNode;

    // Ensure that the attrib data can be watched    
    network->addPostUpdateAccessAttrib(
      target->m_sourceNodeID,
      target->m_sourceNodeFractionalPosAttribAddress.m_semantic,
      1);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisConditionCrossedDurationFractionBuilder::instanceUpdate(  
  AnalysisCondition* condition,
  MR::Network* network,
  AnalysisInterval* NMP_UNUSED(analysisInterval),
  uint32_t NMP_UNUSED(frameIndex))
{
  NMP_VERIFY(condition);
  AnalysisConditionCrossedDurationFraction* target = (AnalysisConditionCrossedDurationFraction*)condition;

  MR::AttribDataPlaybackPos* timeFractionAttrib = network->getAttribData<MR::AttribDataPlaybackPos>(
    target->m_sourceNodeFractionalPosAttribAddress.m_semantic,
    target->m_sourceNodeID,
    target->m_sourceNodeFractionalPosAttribAddress.m_targetNodeID,
    target->m_sourceNodeFractionalPosAttribAddress.m_validFrame,
    target->m_sourceNodeFractionalPosAttribAddress.m_animSetIndex);

  bool result = false;
  if (timeFractionAttrib)
  {
    result = MR::TransitConditionDefCrossedDurationFraction::evaluateCondition(
      target->m_lastUpdateTimeFraction,
      target->m_triggerFraction,
      timeFractionAttrib->m_currentPosAdj,
      timeFractionAttrib);

    target->m_lastUpdateTimeFraction = timeFractionAttrib->m_currentPosAdj;
  }
  else
  {
    target->m_lastUpdateTimeFraction = -1.0f;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisConditionCrossedDurationFractionBuilder::instanceReset(  
  AnalysisCondition* condition)
{
  NMP_VERIFY(condition);
  AnalysisConditionCrossedDurationFraction* target = (AnalysisConditionCrossedDurationFraction*)condition;
  target->m_lastUpdateTimeFraction = -1.0f;
}

}
