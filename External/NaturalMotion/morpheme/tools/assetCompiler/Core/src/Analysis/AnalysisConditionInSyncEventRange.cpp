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
#include "Analysis/AnalysisConditionInSyncEventRange.h"
#include "Analysis/AnalysisInterval.h"
#include "assetProcessor/NodeBuilderUtils.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/TransitConditions/mrTransitConditionInSyncEventRange.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisConditionInSyncEventRangeBuilder::getMemoryRequirements(
  AssetProcessor*             NMP_UNUSED(processor),
  const MR::NetworkDef*       NMP_UNUSED(networkDef),
  const ME::AnalysisNodeExport* NMP_UNUSED(analysisExport))
{
  NMP::Memory::Format result(sizeof(AnalysisConditionInSyncEventRange), NMP_VECTOR_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisCondition* AnalysisConditionInSyncEventRangeBuilder::init(
  NMP::Memory::Resource&      memRes,
  AssetProcessor*             NMP_UNUSED(processor),
  const MR::NetworkDef*       networkDef,
  const ME::AnalysisNodeExport* analysisExport)
{
  NMP_VERIFY(analysisExport);
  const ME::DataBlockExport* analysisNodeDataBlock = analysisExport->getDataBlock();

  NMP::Memory::Format memReqsHdr(sizeof(AnalysisConditionInSyncEventRange), NMP_VECTOR_ALIGNMENT);
  AnalysisConditionInSyncEventRange* result = (AnalysisConditionInSyncEventRange*)memRes.alignAndIncrement(memReqsHdr);

  // Set the network instance handlers
  result->m_instanceInitFn = instanceInit;
  result->m_instanceUpdateFn = instanceUpdate;
  result->m_instanceResetFn = instanceReset;

  // Sync event range
  analysisNodeDataBlock->readFloat(result->m_eventRangeStart, "EventRangeStart");
  analysisNodeDataBlock->readFloat(result->m_eventRangeEnd, "EventRangeEnd");

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

  result->m_sourceNodeSyncEventPlaybackPosAddress.init(
    MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS,
    sourceNodeID,
    MR::INVALID_NODE_ID,
    MR::VALID_FRAME_ANY_FRAME,
    MR::ANIMATION_SET_ANY);

  result->m_sourceNodeSyncEventTrackAddress.init(
    MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK,
    sourceNodeID,
    MR::INVALID_NODE_ID,
    MR::VALID_FRAME_ANY_FRAME,
    MR::ANIMATION_SET_ANY); 

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisConditionInSyncEventRangeBuilder::instanceInit(  
  AnalysisCondition* condition,
  MR::Network* network,
  AnalysisInterval* NMP_UNUSED(analysisInterval))
{
  NMP_VERIFY(condition);
  AnalysisConditionInSyncEventRange* target = (AnalysisConditionInSyncEventRange*)condition;

  // Tell network to keep attrib data around after network update so that we can access it.
  if (target->m_sourceNodeSyncEventPlaybackPosAddress.m_owningNodeID != MR::INVALID_NODE_ID)
  {
    network->addPostUpdateAccessAttrib(
      target->m_sourceNodeSyncEventPlaybackPosAddress.m_owningNodeID,
      target->m_sourceNodeSyncEventPlaybackPosAddress.m_semantic,
      1);
  }

  if (target->m_sourceNodeSyncEventTrackAddress.m_owningNodeID != MR::INVALID_NODE_ID)
  {
    network->addPostUpdateAccessAttrib(
      target->m_sourceNodeSyncEventTrackAddress.m_owningNodeID,
      target->m_sourceNodeSyncEventTrackAddress.m_semantic,
      1);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisConditionInSyncEventRangeBuilder::instanceUpdate(  
  AnalysisCondition* condition,
  MR::Network* network,
  AnalysisInterval* NMP_UNUSED(analysisInterval),
  uint32_t NMP_UNUSED(frameIndex))
{
  NMP_VERIFY(condition);
  AnalysisConditionInSyncEventRange* target = (AnalysisConditionInSyncEventRange*)condition;

  MR::AttribDataUpdateSyncEventPlaybackPos* syncPlaybackPos = static_cast<MR::AttribDataUpdateSyncEventPlaybackPos*>(
    network->getAttribDataRecurseFilterNodes(target->m_sourceNodeSyncEventPlaybackPosAddress, true));

  bool result = false;
  if (syncPlaybackPos)
  {
    MR::AttribDataSyncEventTrack* syncEventTrack = static_cast<MR::AttribDataSyncEventTrack*>(
      network->getAttribDataRecurseFilterNodes(target->m_sourceNodeSyncEventTrackAddress, true));

    float positionRangeEnd = syncPlaybackPos->m_absPosReal.index() + syncPlaybackPos->m_absPosReal.fraction();
    float delta = syncPlaybackPos->m_deltaPos.fraction();
    float positionRangeStart = positionRangeEnd - delta;
    
    // Evaluate the transition condition
    result = MR::TransitConditionDefInSyncEventRange::evaluateCondition(
      target->m_eventRangeStart,
      target->m_eventRangeEnd,
      false,
      positionRangeStart,
      positionRangeEnd,
      syncEventTrack);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisConditionInSyncEventRangeBuilder::instanceReset(  
  AnalysisCondition* condition)
{
  NMP_VERIFY(condition);
  AnalysisConditionInSyncEventRange* target = (AnalysisConditionInSyncEventRange*)condition;
  (void)target;
}

}
