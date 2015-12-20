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
#include "Analysis/AnalysisConditionStateMachineInState.h"
#include "Analysis/AnalysisInterval.h"
#include "assetProcessor/NodeBuilderUtils.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnalysisConditionStateMachineInStateBuilder::getMemoryRequirements(
  AssetProcessor*             NMP_UNUSED(processor),
  const MR::NetworkDef*       NMP_UNUSED(netDef),
  const ME::AnalysisNodeExport* NMP_UNUSED(analysisExport))
{
  NMP::Memory::Format result(sizeof(AnalysisConditionStateMachineInState), NMP_VECTOR_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnalysisCondition* AnalysisConditionStateMachineInStateBuilder::init(
  NMP::Memory::Resource&      memRes,
  AssetProcessor*             NMP_UNUSED(processor),
  const MR::NetworkDef*       netDef,
  const ME::AnalysisNodeExport* analysisExport)
{
  NMP_VERIFY(analysisExport);
  const ME::DataBlockExport* analysisNodeDataBlock = analysisExport->getDataBlock();

  NMP::Memory::Format memReqsHdr(sizeof(AnalysisConditionStateMachineInState), NMP_VECTOR_ALIGNMENT);
  AnalysisConditionStateMachineInState* result = (AnalysisConditionStateMachineInState*)memRes.alignAndIncrement(memReqsHdr);
  
  // Set the network instance handlers
  result->m_instanceInitFn = instanceInit;
  result->m_instanceUpdateFn = instanceUpdate;
  result->m_instanceResetFn = instanceReset;

  //------------------
  // Get the runtime node ID of the state machine to monitor.
  std::string nodeName;
  analysisNodeDataBlock->readString(nodeName, "StateMachine");
  result->m_stateMachineNodeID = netDef->getRootNodeID();
  if (!nodeName.empty())
    result->m_stateMachineNodeID = netDef->getNodeIDFromNodeName(nodeName.c_str());
  NMP_VERIFY_MSG(result->m_stateMachineNodeID != MR::INVALID_NODE_ID, "The StateMachine node for the analysis condition is invalid");
  const MR::NodeDef* nodeDef = netDef->getNodeDef(result->m_stateMachineNodeID);
  NMP_VERIFY_MSG(nodeDef->getNodeTypeID() == NODE_TYPE_STATE_MACHINE, "Node specified is not a state machine");

  //------------------
  // Get the state ID that we want to watch for.
  analysisNodeDataBlock->readString(nodeName, "State");
  MR::NodeID stateNodeID = MR::INVALID_NODE_ID;
  if (!nodeName.empty())
  {
    // This node name could be either a state or a transition.
    // Transitions are nodes and are stored in the node name <-> node id table.
    // State names are not nodes and are stored in state name <-> state id table.
    stateNodeID = netDef->getNodeIDFromNodeName(nodeName.c_str());
    if(stateNodeID == MR::INVALID_NODE_ID)
    {
      stateNodeID = netDef->getNodeIDFromStateName(nodeName.c_str());
    }
  }
  NMP_VERIFY_MSG(stateNodeID != MR::INVALID_NODE_ID, "The State node is invalid");
  const MR::AttribDataStateMachineDef* smAttribDef = netDef->getAttribData<MR::AttribDataStateMachineDef>(
                                                                  MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,
                                                                  result->m_stateMachineNodeID);
  result->m_stateID = smAttribDef->findStateIDFromNodeID(stateNodeID);
  NMP_VERIFY_MSG(result->m_stateID != MR::INVALID_STATE_ID, "Invalid state specified");

  //------------------
  // Should we become true when not in this state.
  analysisNodeDataBlock->readBool(result->m_notInState, "NotInState");

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisConditionStateMachineInStateBuilder::instanceInit(  
  AnalysisCondition* NMP_USED_FOR_VERIFY(condition),
  MR::Network*       NMP_USED_FOR_VERIFY(net),
  AnalysisInterval*  NMP_UNUSED(analysisInterval))
{
  NMP_VERIFY(condition && net);
}

//----------------------------------------------------------------------------------------------------------------------
bool AnalysisConditionStateMachineInStateBuilder::instanceUpdate(  
  AnalysisCondition* condition,
  MR::Network* net,
  AnalysisInterval* NMP_UNUSED(analysisInterval),
  uint32_t NMP_UNUSED(frameIndex))
{
  NMP_VERIFY(condition);
  AnalysisConditionStateMachineInState* target = (AnalysisConditionStateMachineInState*)condition;

  // Get the state machines current state.
  MR::AttribDataStateMachine* smAttrib = net->getAttribData<MR::AttribDataStateMachine>(
                                                        MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
                                                        target->m_stateMachineNodeID);
  NMP_VERIFY(smAttrib);

  // Compare to target state and flip when the notInState flag is set.
  bool result = target->m_notInState;
  if (smAttrib->getActiveStateID() == target->m_stateID)
    result = !result;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AnalysisConditionStateMachineInStateBuilder::instanceReset(  
  AnalysisCondition* NMP_USED_FOR_VERIFY(condition))
{
  NMP_VERIFY(condition);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------