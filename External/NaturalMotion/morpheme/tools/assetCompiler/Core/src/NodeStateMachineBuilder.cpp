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
#include <tchar.h>
#include "NetworkDefBuilder.h"
#include "NodeStateMachineBuilder.h"
#include "assetProcessor/TransitConditionDefBuilder.h"
#include "assetProcessor/TransitDeadBlendDefBuilder.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/TransitConditions/mrTransitConditionOnMessage.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace
{

//----------------------------------------------------------------------------------------------------------------------
int32_t sortTransitionsBasedOnTransitPriority(
  MR::StateDef*                  stateDef,
  MR::AttribDataStateMachineDef* stateMachineDef,
  const ME::NetworkDefExport*    netDefExport)
{
  // bubble sort
  int32_t highestPriority = 0;
  int32_t flag = 1; // set flag to 1 to start first pass
  MR::StateID tempStateID;
  int32_t numTransits = stateDef->m_numExitTransitionStates; 
  for(int32_t i = 1; (i <= numTransits) && flag; i++)
  {
    flag = 0;
    for (int32_t j = 0; j < (numTransits - 1); j++)
    {
      MR::StateID thisStateID = stateDef->m_exitTransitionStateIDs[j];
      MR::StateDef* thisStateDef = stateMachineDef->getStateDef(thisStateID);
      int32_t thisItemPriority = 0;
      netDefExport->getNode(thisStateDef->m_nodeID)->getDataBlock()->readInt(thisItemPriority,"TransitionPriority");
      MR::StateID nextStateID = stateDef->m_exitTransitionStateIDs[j+1];
      MR::StateDef* nextStateDef = stateMachineDef->getStateDef(nextStateID);
      int32_t nextItemPriority = 0;
      netDefExport->getNode(nextStateDef->m_nodeID)->getDataBlock()->readInt(nextItemPriority,"TransitionPriority");
      if (nextItemPriority > thisItemPriority) // descending order
      { 
        tempStateID = stateDef->m_exitTransitionStateIDs[j];             
        stateDef->m_exitTransitionStateIDs[j] = (stateDef->m_exitTransitionStateIDs[j+1]);
        stateDef->m_exitTransitionStateIDs[j+1] = tempStateID;
        flag = 1;
      }
    }
  }

  // It is now sorted so return the priority of the first item.
  if (stateDef->m_numExitTransitionStates > 0)
  {
    MR::StateDef* transitStateDef = stateMachineDef->getStateDef(stateDef->m_exitTransitionStateIDs[0]);
    netDefExport->getNode(transitStateDef->m_nodeID)->getDataBlock()->readInt(highestPriority,"TransitionPriority");
  }
  return highestPriority;
}

//----------------------------------------------------------------------------------------------------------------------
bool isNodeOrDescendantPhysical(
  const AP::NodeDefDependency& nodeDefDependancies,
  const AP::NetworkDefCompilationInfo& netDefCompInfo,
  bool* visitedFlags)
{ 
  const MR::NodeDef* nodeDef = netDefCompInfo.getNodeDef(nodeDefDependancies.m_nodeID);
  const uint16_t nodeDefFlags = nodeDef->getNodeFlags();
  // Node defs stored in the net def compilation info have been dislocated so we need to endian swap the flags before 
  // reading them
  NMP::endianSwap(nodeDefFlags);

  if (nodeDefFlags & MR::NodeDef::NODE_FLAG_IS_PHYSICAL)
  {
    return true;
  }

  if (nodeDefFlags & MR::NodeDef::NODE_FLAG_IS_PHYSICS_GROUPER)
  {
    return true;
  }

  for (uint32_t i = 0; i < nodeDefDependancies.m_childNodeIDs.size(); ++i)
  {
    MR::NodeID childNodeID = nodeDefDependancies.m_childNodeIDs[i]->m_nodeID;
    // Make sure we don't visit nodes twice. If this check isn't done we can end up visiting nodes  multiple times if many
    // multiply connected pass down pins are used.
    const MR::NodeDef* childDef = netDefCompInfo.getNodeDef(childNodeID);
    if (childDef && !visitedFlags[childNodeID])
    {
      visitedFlags[childNodeID] = true;
      if (isNodeOrDescendantPhysical(*nodeDefDependancies.m_childNodeIDs[i], netDefCompInfo, visitedFlags))
      {
        return true;
      }
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool isDeadBlendOrPhysicsTransition(
  const ME::NodeExport& nodeDefExport,
  const AP::NetworkDefCompilationInfo& netDefCompInfo)
{
  const ME::DataBlockExport* dataBlock = nodeDefExport.getDataBlock();
  NMP_VERIFY(dataBlock);

  uint32_t sourceNodeID = MR::INVALID_NODE_ID;
  if (!dataBlock->readUInt(sourceNodeID, "SourceNodeID"))
  {
    // If there is no source node ID then this is an active state transition which may produce a deadblend
    return true;
  }

  uint32_t destNodeID = MR::INVALID_NODE_ID;
  bool readResult = dataBlock->readUInt(destNodeID, "DestNodeID");
  NMP_VERIFY(readResult);

  // Check for self transitions
  if (destNodeID == sourceNodeID)
  {
    return true;
  }

  // Dead blending occurs if the source node is physical
  uint32_t flagArraySize = sizeof(bool) * netDefCompInfo.getNumNodes();
  bool* visitedFlags = (bool*)NMP::Memory::memAlloc(flagArraySize NMP_MEMORY_TRACKING_ARGS);
  memset(visitedFlags, 0, flagArraySize);

  const AP::NodeDefDependency& sourceNodeDependency = netDefCompInfo.getNodeDefDependency(sourceNodeID);
  if (isNodeOrDescendantPhysical(sourceNodeDependency, netDefCompInfo, visitedFlags))
  {
    NMP::Memory::memFree(visitedFlags);
    return true;
  }

  // We also need to capture transitions where the destination node is physical
  memset(visitedFlags, 0, flagArraySize);
  const AP::NodeDefDependency& destNodeDependency = netDefCompInfo.getNodeDefDependency(destNodeID);
  if (isNodeOrDescendantPhysical(destNodeDependency, netDefCompInfo, visitedFlags))
  {
    NMP::Memory::memFree(visitedFlags);
    return true;
  }

  NMP::Memory::memFree(visitedFlags);

  bool deadBlendSourceOnBreakout = false;
  if (dataBlock->readBool(deadBlendSourceOnBreakout, "DeadblendBreakoutToSource"))
  {
    if (deadBlendSourceOnBreakout)
    {
      return true;
    }
  }

  return false;
}

} // namespace

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeStateMachineBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeStateMachineBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_STATE_MACHINE);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeStateMachineBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  uint32_t numSteadyStateNodes;
  nodeDefDataBlock->readUInt(numSteadyStateNodes, "ChildNodeCount");
  uint32_t numTransitionStateNodes;
  nodeDefDataBlock->readUInt(numTransitionStateNodes, "ChildTransitionCount");

  //---------------------------
  // Steady State Node IDs.
  NMP_VERIFY(numSteadyStateNodes > 0);  
  char paramName[256];
  for (uint32_t i = 0; i < numSteadyStateNodes; ++i)
  {
    MR::NodeID nodeID;
    sprintf_s(paramName, "RuntimeChildNodeID_%d", i);
    readNodeID(nodeDefDataBlock, paramName, nodeID);
    childNodeIDs.push_back(nodeID);
  }

  //---------------------------
  // Transitional State Node IDs.
  if (numTransitionStateNodes)
  {
    for (uint32_t i = 0; i < numTransitionStateNodes; ++i)
    {
      MR::NodeID nodeID;
      sprintf_s(paramName, "RuntimeChildTransitID_%d", i);
      readNodeID(nodeDefDataBlock, paramName, nodeID);
      childNodeIDs.push_back(nodeID);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeStateMachineBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
// Returns 0xFFFFFFFF if not found.
uint32_t findNodeIDInArray(
  MR::NodeID        nodeIDToFind, 
  const MR::NodeID* arrayToSearch,
  uint32_t          arraySize)
{
  for (uint32_t i = 0; i < arraySize; ++i)
  {
    if (arrayToSearch[i] == nodeIDToFind)
      return i;
  }

  return 0xFFFFFFFF;
}

//----------------------------------------------------------------------------------------------------------------------
// Find transitional State Node IDs and steady State Node IDs.
// Find out and store how many conditions there are on every transition.
// Allocates arrays internally.
void findChildStatesInfo(
  const ME::NetworkDefExport* netDefExport,
  const ME::NodeExport*       nodeDefExport,
  uint32_t&                   numSteadyStateNodes,
  MR::NodeID*&                steadyStateNodeIDs,
  uint32_t&                   numTransitionStateNodes,
  MR::NodeID*&                transitionNodeIDs,
  MR::ConditionIndex&         totalNumTransitConditions,
  MR::ConditionIndex*&        transitionEntryConditionCounts,
  MR::ConditionIndex*&        numExitConditionsPerState,
  MR::ConditionIndex&         numExitConditionsGlobalState)
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  
  //---------------------------
  // Transitional State Node IDs.
  nodeDefDataBlock->readUInt(numTransitionStateNodes, "ChildTransitionCount");
  transitionNodeIDs = NULL;
  
  char paramName[256];
  if (numTransitionStateNodes)
  {
    transitionNodeIDs = new MR::NodeID[numTransitionStateNodes];
    for (uint32_t i = 0; i < numTransitionStateNodes; ++i)
    {
      MR::NodeID nodeID;
      sprintf_s(paramName, "RuntimeChildTransitID_%d", i);
      readNodeID(nodeDefDataBlock, paramName, nodeID);
      transitionNodeIDs[i] = nodeID;
    }
  }

  //---------------------------
  // Steady State Node IDs.
  nodeDefDataBlock->readUInt(numSteadyStateNodes, "ChildNodeCount");
  NMP_VERIFY(numSteadyStateNodes > 0);
  steadyStateNodeIDs = new MR::NodeID[numSteadyStateNodes];
  for (uint32_t i = 0; i < numSteadyStateNodes; ++i)
  {
    MR::NodeID nodeID;
    sprintf_s(paramName, "RuntimeChildNodeID_%d", i);
    readNodeID(nodeDefDataBlock, paramName, nodeID);
    steadyStateNodeIDs[i] = nodeID;
  }
   
  
  //---------------------------
  // Initialise storage for counts of exit conditions from each state.
  numExitConditionsPerState = new MR::ConditionIndex[numSteadyStateNodes + numTransitionStateNodes];
  memset(numExitConditionsPerState, 0, sizeof(MR::ConditionIndex) * (numSteadyStateNodes + numTransitionStateNodes));
  numExitConditionsGlobalState = 0;

  //---------------------------
  // Find out and store how many conditions there are on every transition.
  transitionEntryConditionCounts = NULL;
  if (numTransitionStateNodes)
  {
    transitionEntryConditionCounts = new MR::ConditionIndex[numTransitionStateNodes];
    memset(transitionEntryConditionCounts, 0, sizeof(MR::ConditionIndex) * numTransitionStateNodes);
        
    int32_t  stateDefIndex = 0;
    
    // Transitions from steady states.
    for (uint32_t i = 0; i < numSteadyStateNodes; ++i)
    {
      const ME::NodeExport* nodeExport = netDefExport->getNode(steadyStateNodeIDs[i]);
      uint32_t numExitTransitions = nodeExport->getNumConditionSets();
      for (uint32_t j = 0; j < numExitTransitions; ++j)
      {
        const ME::ConditionSetExport* condSet = nodeExport->getConditionSet(j);
        MR::NodeID transitNodeID = (MR::NodeID) condSet->getTargetNodeID();
        uint32_t transitIndex = findNodeIDInArray(transitNodeID, transitionNodeIDs, numTransitionStateNodes);
        NMP_ASSERT(transitIndex != 0xFFFFFFFF);
        MR::ConditionIndex numConditionIndices = (MR::ConditionIndex) condSet->getConditionIndices().size();
        transitionEntryConditionCounts[transitIndex] = numConditionIndices;
        numExitConditionsPerState[stateDefIndex] += numConditionIndices;
      }
      ++stateDefIndex;
    }

    // Transitions from transitions.
    for (uint32_t i = 0; i < numTransitionStateNodes; ++i)
    {
      const ME::NodeExport* nodeExport = netDefExport->getNode(transitionNodeIDs[i]);
      uint32_t numExitTransitions = nodeExport->getNumConditionSets();
      for (uint32_t j = 0; j < numExitTransitions; ++j)
      {
        const ME::ConditionSetExport* condSet = nodeExport->getConditionSet(j);
        MR::NodeID transitNodeID = (MR::NodeID) condSet->getTargetNodeID();
        uint32_t transitIndex = findNodeIDInArray(transitNodeID, transitionNodeIDs, numTransitionStateNodes);
        NMP_ASSERT(transitIndex != 0xFFFFFFFF);
        MR::ConditionIndex numConditionIndices = (MR::ConditionIndex) condSet->getConditionIndices().size();
        transitionEntryConditionCounts[transitIndex] = numConditionIndices;
        numExitConditionsPerState[stateDefIndex] += numConditionIndices;
      }
      ++stateDefIndex;
    }

    // Transitions from global state.
    {
      const ME::NodeExport* nodeExport = nodeDefExport;
      uint32_t numExitTransitions = nodeExport->getNumCommonConditionSets();
      for (uint32_t j = 0; j < numExitTransitions; ++j)
      {
        const ME::ConditionSetExport* condSet = nodeExport->getCommonConditionSet(j);
        MR::NodeID transitNodeID = (MR::NodeID) condSet->getTargetNodeID();
        uint32_t transitIndex = findNodeIDInArray(transitNodeID, transitionNodeIDs, numTransitionStateNodes);
        NMP_ASSERT(transitIndex != 0xFFFFFFFF);
        MR::ConditionIndex numConditionIndices = (MR::ConditionIndex) condSet->getConditionIndices().size();
        transitionEntryConditionCounts[transitIndex] = numConditionIndices;
        numExitConditionsGlobalState += numConditionIndices;
      }
    }
  }

  //---------------------------
  // Sum the number of transition conditions.
  uint32_t workingNumTransitConditions = 0;
  for(uint32_t i = 0; i < numTransitionStateNodes; ++i)
    workingNumTransitConditions += transitionEntryConditionCounts[i];
  NMP_VERIFY(workingNumTransitConditions < MR::INVALID_CONDITION_INDEX);
  totalNumTransitConditions = (MR::ConditionIndex) workingNumTransitConditions;
}

//----------------------------------------------------------------------------------------------------------------------
// Allocate a bool array and init all entries to false.
bool* allocateBoolArray(uint32_t numEntries)
{
  bool* result = NULL;
  if (numEntries)
  {
    result = new bool[numEntries];
    for (uint32_t i = 0; i < numEntries; ++i)
    {
      result[i] = false;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeStateMachineBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  //---------------------------
  // Find transitional State Node IDs and steady State Node IDs.
  // Find out and store how many conditions there are on every transition.
  uint32_t            numSteadyStateNodes;
  MR::NodeID*         steadyStateNodeIDs;
  uint32_t            numTransitionStateNodes;
  MR::NodeID*         transitionNodeIDs;
  MR::ConditionIndex  totalNumTransitConditions;
  MR::ConditionIndex* transitionEntryConditionCounts;
  MR::ConditionIndex* numExitConditionsPerState;
  MR::ConditionIndex  numExitConditionsGlobalState;

  findChildStatesInfo(
                  netDefExport,
                  nodeDefExport,
                  numSteadyStateNodes,
                  steadyStateNodeIDs,
                  numTransitionStateNodes,
                  transitionNodeIDs,
                  totalNumTransitConditions,
                  transitionEntryConditionCounts,
                  numExitConditionsPerState,
                  numExitConditionsGlobalState);

  // Allocate a working array to indicate if each transition has already been processed.
  bool* transitionsProcessed = allocateBoolArray(numTransitionStateNodes);  // Flags when each transition has been processed. Needed when we have shared active state transitions.
   
  //---------------------------
  // Allocate space for the NodeDef itself.
  // Note: we only allow space for AttribData in anim set 0, this is ok because we create no data for any other anim set.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
                                  netDefCompilationInfo, 
                                  numSteadyStateNodes + numTransitionStateNodes,  // numChildren
                                  0,                                              // numInputCPConnections
                                  1,                                              // numAnimSetEntries
                                  nodeDefExport);

  // Add size of AttribData structure itself.
  result += MR::AttribDataStateMachineDef::getMemoryRequirements();


  //---------------------------
  // Add size of StateDef array.
  uint32_t totalNumStates = numSteadyStateNodes + numTransitionStateNodes;
  NMP::Memory::Format memReqs;
  memReqs.size = NMP::Memory::align(totalNumStates * sizeof(MR::StateDef), NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
  result += memReqs;

  //---------------------------
  // Add size of single active state StateDef.
  memReqs.size = NMP::Memory::align(sizeof(MR::StateDef), NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
  result += memReqs;

  //---------------------------
  // Steady state Nodes.
  int32_t stateDefIndex = 0;
  for (uint32_t i = 0; i < numSteadyStateNodes; ++i)
  {
    const ME::NodeExport* nodeExport = netDefExport->getNode(steadyStateNodeIDs[i]);

    // Exit condition index array for StateDef.
    uint32_t numExitConditions = numExitConditionsPerState[stateDefIndex];
    memReqs.size = NMP::Memory::align(numExitConditions * sizeof(MR::ConditionIndex), NMP_NATURAL_TYPE_ALIGNMENT);
    memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
    result += memReqs;

    // exitTransitionStateIDs.
    uint32_t numExitTransitions = nodeExport->getNumConditionSets();
    memReqs.size = NMP::Memory::align(numExitTransitions * sizeof(MR::StateID), NMP_NATURAL_TYPE_ALIGNMENT);
    memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
    result += memReqs;   
    ++stateDefIndex;
  }

  //---------------------------
  // Transition state Nodes.
  for (uint32_t i = 0; i < numTransitionStateNodes; ++i)
  {
    const ME::NodeExport* nodeExport = netDefExport->getNode(transitionNodeIDs[i]);

    // Exit condition index array for StateDef.
    uint32_t numExitConditions = numExitConditionsPerState[stateDefIndex];
    memReqs.size = NMP::Memory::align(numExitConditions * sizeof(MR::ConditionIndex), NMP_NATURAL_TYPE_ALIGNMENT);
    memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
    result += memReqs;

    // exitTransitionStateIDs.
    uint32_t numExitTransitions = nodeExport->getNumConditionSets();
    memReqs.size = NMP::Memory::align(numExitTransitions * sizeof(MR::StateID), NMP_NATURAL_TYPE_ALIGNMENT);
    memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
    result += memReqs;

    // Entry condition index array for transition StateDef.
    memReqs.size = NMP::Memory::align(transitionEntryConditionCounts[i] * sizeof(MR::ConditionIndex), NMP_NATURAL_TYPE_ALIGNMENT);
    memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
    result += memReqs;
    ++stateDefIndex;
  }

  //---------------------------
  // Global state.
  {
    const ME::NodeExport* nodeExport = nodeDefExport;

    // Global Condition index array for StateDef.
    uint32_t numExitConditions = numExitConditionsGlobalState;
    memReqs.size = NMP::Memory::align(numExitConditions * sizeof(MR::ConditionIndex), NMP_NATURAL_TYPE_ALIGNMENT);
    memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
    result += memReqs;

    // StateChangeDefs.
    uint32_t numExitTransitions = nodeExport->getNumCommonConditionSets();
    memReqs.size = NMP::Memory::align(numExitTransitions * sizeof(MR::StateID), NMP_NATURAL_TYPE_ALIGNMENT);
    memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
    result += memReqs;
  }

  //---------------------------
  // Condition pointers array.
  memReqs.size = NMP::Memory::align(totalNumTransitConditions * sizeof(MR::TransitConditionDef*), NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
  result += memReqs;

  //---------------------------
  // Space for conditions themselves.
  // Note: This code assumes that there is no reuse of the same condition.

  // From steady state nodes.
  uint32_t conditionCountValidation = 0;
  for (uint32_t i = 0; i < numSteadyStateNodes; ++i)
  {
    const ME::NodeExport* nodeExport = netDefExport->getNode(steadyStateNodeIDs[i]);
    uint32_t numExitTransitions = nodeExport->getNumConditionSets();
    for (uint32_t j = 0; j < numExitTransitions; ++j)
    {
      const ME::ConditionSetExport* condSet = nodeExport->getConditionSet(j);
      MR::NodeID transitNodeID = (MR::NodeID) condSet->getTargetNodeID();
      uint32_t transitIndex = findNodeIDInArray(transitNodeID, transitionNodeIDs, numTransitionStateNodes);
      NMP_ASSERT((transitIndex != 0xFFFFFFFF) && (transitIndex < numTransitionStateNodes));
      if (!transitionsProcessed[transitIndex])
      {
        // Add the conditions of this transition because we have not already processed them.
        std::vector<uint32_t> conditionIndicesVec = condSet->getConditionIndices();
        conditionCountValidation += (uint32_t) conditionIndicesVec.size();
        for (uint32_t k = 0; k < conditionIndicesVec.size(); ++k)
        {
          const ME::ConditionExport* conditionExport = nodeExport->getCondition(conditionIndicesVec[k]);
          MR::TransitConditType conditionType = (MR::TransitConditType) conditionExport->getTypeID();
          TransitConditionDefBuilder* conditionBuilder = processor->getTransitConditionDefBuilder(conditionType);
          if (!conditionBuilder)
          {
            processor->outputConditionError(conditionType, "Cannot find transit condition builder.");
          }
          NMP_VERIFY(conditionBuilder);

          // Space for the transit condition def itself.
          result += conditionBuilder->getMemoryRequirements(conditionExport, netDefExport, processor);
        }
      }
      transitionsProcessed[transitIndex] = true;
    }
  }

  // From transition state nodes.
  for (uint32_t i = 0; i < numTransitionStateNodes; ++i)
  {
    const ME::NodeExport* nodeExport = netDefExport->getNode(transitionNodeIDs[i]);
    uint32_t numExitTransitions = nodeExport->getNumConditionSets();
    for (uint32_t j = 0; j < numExitTransitions; ++j)
    {
      const ME::ConditionSetExport* condSet = nodeExport->getConditionSet(j);
      MR::NodeID transitNodeID = (MR::NodeID) condSet->getTargetNodeID();
      uint32_t transitIndex = findNodeIDInArray(transitNodeID, transitionNodeIDs, numTransitionStateNodes);
      NMP_ASSERT((transitIndex != 0xFFFFFFFF) && (transitIndex < numTransitionStateNodes));
      if (!transitionsProcessed[transitIndex])
      {
        // Add the conditions of this transition because we have not already processed them.
        std::vector<uint32_t> conditionIndicesVec = condSet->getConditionIndices();
        conditionCountValidation += (uint32_t) conditionIndicesVec.size();
        for (uint32_t k = 0; k < conditionIndicesVec.size(); ++k)
        {
          const ME::ConditionExport* conditionExport = nodeExport->getCondition(conditionIndicesVec[k]);
          MR::TransitConditType conditionType = (MR::TransitConditType) conditionExport->getTypeID();
          TransitConditionDefBuilder* conditionBuilder = processor->getTransitConditionDefBuilder(conditionType);
          if (!conditionBuilder)
          {
            processor->outputConditionError(conditionType, "Cannot find transit condition builder.");
          }
          NMP_VERIFY(conditionBuilder);

          // Space for the transit condition def itself.
          result += conditionBuilder->getMemoryRequirements(conditionExport, netDefExport, processor);
        }
      }
      transitionsProcessed[transitIndex] = true;
    }
  }

  // Global state conditions.
  {
    const ME::NodeExport* nodeExport = nodeDefExport;
    uint32_t numExitTransitions = nodeExport->getNumCommonConditionSets();
    for (uint32_t j = 0; j < numExitTransitions; ++j)
    {
      const ME::ConditionSetExport* condSet = nodeExport->getCommonConditionSet(j);
      MR::NodeID transitNodeID = (MR::NodeID) condSet->getTargetNodeID();
      uint32_t transitIndex = findNodeIDInArray(transitNodeID, transitionNodeIDs, numTransitionStateNodes);
      NMP_ASSERT((transitIndex != 0xFFFFFFFF) && (transitIndex < numTransitionStateNodes));
      if (!transitionsProcessed[transitIndex])
      {
        // Add the conditions of this transition because we have not already processed them.
        std::vector<uint32_t> conditionIndicesVec = condSet->getConditionIndices();
        conditionCountValidation += (uint32_t) conditionIndicesVec.size();
        for (uint32_t k = 0; k < conditionIndicesVec.size(); ++k)
        {
          const ME::ConditionExport* conditionExport = nodeExport->getCommonCondition(conditionIndicesVec[k]);
          MR::TransitConditType conditionType = (MR::TransitConditType) conditionExport->getTypeID();
          TransitConditionDefBuilder* conditionBuilder = processor->getTransitConditionDefBuilder(conditionType);
          if (!conditionBuilder)
          {
            processor->outputConditionError(conditionType, "Cannot find transit condition builder.");
          }
          NMP_VERIFY(conditionBuilder);

          // Space for the transit condition def itself.
          result += conditionBuilder->getMemoryRequirements(conditionExport, netDefExport, processor);
        }
      }
      transitionsProcessed[transitIndex] = true;
    }
  }

  NMP_VERIFY(conditionCountValidation == totalNumTransitConditions);


  // Clean up working memory.
  if (numSteadyStateNodes)
    delete [] steadyStateNodeIDs;
  if (numTransitionStateNodes)
  {
    delete [] transitionNodeIDs;
    delete [] transitionEntryConditionCounts;
    delete [] transitionsProcessed;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// For each state allocate the internal index arrays for transition node ids and conditions.
void allocateIndexingArraysForStateDefs(
  uint32_t                       numSteadyStateNodes,
  const MR::NodeID*              steadyStateNodeIDs,
  uint32_t                       numTransitionStateNodes,
  const MR::NodeID*              transitionNodeIDs,
  NetworkDefCompilationInfo*     netDefCompilationInfo,
  const ME::NetworkDefExport*    netDefExport,
  const ME::NodeExport*          nodeDefExport,
  MR::NodeDef*                   nodeDef,
  MR::AttribDataStateMachineDef* stateMachineDef,
  const MR::ConditionIndex*      transitionEntryConditionCounts,
  bool&                          shouldExtendOutputLifespans,
  NMP::Memory::Resource&         memRes,
  MR::ConditionIndex*            numExitConditionsPerState,
  MR::ConditionIndex             numExitConditionsGlobalState)
{
  NMP::Memory::Format memReqs;

  //---------------------------
  // Steady state Nodes.
  int32_t       stateDefIndex = 0;
  MR::StateDef* currentStateDef;
  for (uint32_t i = 0; i < numSteadyStateNodes; ++i)
  {
    const ME::NodeExport* nodeExport = netDefExport->getNode(steadyStateNodeIDs[i]);
    currentStateDef = &stateMachineDef->m_stateDefs[stateDefIndex];
    currentStateDef->m_nodeID = steadyStateNodeIDs[i];
    nodeDef->setChildNodeID(stateDefIndex, steadyStateNodeIDs[i]);

    // Exit condition index array for StateDef.
    NMP_VERIFY(numExitConditionsPerState[stateDefIndex] < MR::INVALID_CONDITION_INDEX);
    currentStateDef->m_numExitConditions = numExitConditionsPerState[stateDefIndex];
    memReqs.size = NMP::Memory::align(currentStateDef->m_numExitConditions * sizeof(MR::ConditionIndex), NMP_NATURAL_TYPE_ALIGNMENT);
    memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
    memRes.align(memReqs);
    if (currentStateDef->m_numExitConditions > 0)
      currentStateDef->m_exitConditionIndexes = (MR::ConditionIndex*) memRes.ptr;
    else
      currentStateDef->m_exitConditionIndexes = NULL;
    memRes.increment(memReqs);

    // exitTransitionStateIDs.
    NMP_VERIFY(nodeExport->getNumConditionSets() < MR::INVALID_STATE_ID);
    currentStateDef->m_numExitTransitionStates = (MR::StateID) nodeExport->getNumConditionSets();
    memReqs.size = NMP::Memory::align(currentStateDef->m_numExitTransitionStates * sizeof(MR::StateID), NMP_NATURAL_TYPE_ALIGNMENT);
    memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
    memRes.align(memReqs);
    if (currentStateDef->m_numExitTransitionStates > 0)
      currentStateDef->m_exitTransitionStateIDs = (MR::StateID*) memRes.ptr;
    else
      currentStateDef->m_exitTransitionStateIDs = NULL;
    memRes.increment(memReqs);
    
    // Only transitions have entry conditions.
    currentStateDef->m_numEntryConditions = 0;
    currentStateDef->m_entryConditionIndexes = NULL;

    ++stateDefIndex;
  }

  //---------------------------
  // Transition Nodes.
  shouldExtendOutputLifespans = false;

  for (uint32_t i = 0; i < numTransitionStateNodes; ++i)
  {
    const ME::NodeExport* nodeExport = netDefExport->getNode(transitionNodeIDs[i]);
    currentStateDef = &stateMachineDef->m_stateDefs[stateDefIndex];
    currentStateDef->m_nodeID = transitionNodeIDs[i];
    nodeDef->setChildNodeID(stateDefIndex, transitionNodeIDs[i]);

    // Check to see if this transition might dead blend or transition to physics, in which case this state machine
    //  needs to extend the lifespan of it's outputs to allow the runtime to provide seamless transitions.
    if (!shouldExtendOutputLifespans && isDeadBlendOrPhysicsTransition(*nodeExport, *netDefCompilationInfo))
    {
      shouldExtendOutputLifespans = true;
    }

    // Exit condition index array for StateDef.
    NMP_VERIFY(numExitConditionsPerState[stateDefIndex] < MR::INVALID_CONDITION_INDEX);
    currentStateDef->m_numExitConditions = numExitConditionsPerState[stateDefIndex];
    memReqs.size = NMP::Memory::align(currentStateDef->m_numExitConditions * sizeof(MR::ConditionIndex), NMP_NATURAL_TYPE_ALIGNMENT);
    memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
    memRes.align(memReqs);
    if (currentStateDef->m_numExitConditions > 0)
      currentStateDef->m_exitConditionIndexes = (MR::ConditionIndex*) memRes.ptr;
    else
      currentStateDef->m_exitConditionIndexes = NULL;
    memRes.increment(memReqs);

    // exitTransitionStateIDs.
    NMP_VERIFY(nodeExport->getNumConditionSets() < MR::INVALID_STATE_ID);
    currentStateDef->m_numExitTransitionStates = (MR::StateID) nodeExport->getNumConditionSets();
    memReqs.size = NMP::Memory::align(currentStateDef->m_numExitTransitionStates * sizeof(MR::StateID), NMP_NATURAL_TYPE_ALIGNMENT);
    memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
    memRes.align(memReqs);
    if (currentStateDef->m_numExitTransitionStates > 0)
      currentStateDef->m_exitTransitionStateIDs = (MR::StateID*) memRes.ptr;
    else
      currentStateDef->m_exitTransitionStateIDs = NULL;
    memRes.increment(memReqs);

    // Entry condition index array for transition StateDef.
    currentStateDef->m_numEntryConditions = transitionEntryConditionCounts[i];
    memReqs.size = NMP::Memory::align(currentStateDef->m_numEntryConditions * sizeof(MR::ConditionIndex), NMP_NATURAL_TYPE_ALIGNMENT);
    memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
    memRes.align(memReqs);
    if (currentStateDef->m_numEntryConditions > 0)
      currentStateDef->m_entryConditionIndexes = (MR::ConditionIndex*) memRes.ptr;
    else
      currentStateDef->m_entryConditionIndexes = NULL;
    memRes.increment(memReqs);

    ++stateDefIndex;
  }

  //---------------------------
  // Global state.
  {
    currentStateDef = stateMachineDef->m_globalStateDef;
    currentStateDef->m_nodeID = MR::INVALID_NODE_ID;

    // Exit condition index array for StateDef.
    NMP_VERIFY(numExitConditionsGlobalState < MR::INVALID_CONDITION_INDEX);
    currentStateDef->m_numExitConditions = numExitConditionsGlobalState;
    memReqs.size = NMP::Memory::align(currentStateDef->m_numExitConditions * sizeof(MR::ConditionIndex), NMP_NATURAL_TYPE_ALIGNMENT);
    memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
    memRes.align(memReqs);
    if (currentStateDef->m_numExitConditions > 0)
      currentStateDef->m_exitConditionIndexes = (MR::ConditionIndex*) memRes.ptr;
    else
      currentStateDef->m_exitConditionIndexes = NULL;
    memRes.increment(memReqs);

    // exitTransitionStateIDs.
    NMP_VERIFY(nodeDefExport->getNumCommonConditionSets() < MR::INVALID_STATE_ID);
    currentStateDef->m_numExitTransitionStates = (MR::StateID) nodeDefExport->getNumCommonConditionSets();
    memReqs.size = NMP::Memory::align(currentStateDef->m_numExitTransitionStates * sizeof(MR::StateID), NMP_NATURAL_TYPE_ALIGNMENT);
    memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
    memRes.align(memReqs);
    if (currentStateDef->m_numExitTransitionStates > 0)
      currentStateDef->m_exitTransitionStateIDs = (MR::StateID*) memRes.ptr;
    else
      currentStateDef->m_exitTransitionStateIDs = NULL;
    memRes.increment(memReqs);

    // Only transitions have entry conditions.
    currentStateDef->m_numEntryConditions = 0;
    currentStateDef->m_entryConditionIndexes = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void wipeDownStateDef(MR::StateDef* stateDef)
{
  stateDef->m_nodeID = MR::INVALID_NODE_ID;
  stateDef->m_numEntryConditions = 0;
  stateDef->m_entryConditionIndexes = NULL;
  stateDef->m_numExitConditions = 0;
  stateDef->m_exitConditionIndexes = NULL;
  stateDef->m_numExitBreakoutConditions = 0;
  stateDef->m_numExitTransitionStates = 0;
  stateDef->m_exitTransitionStateIDs = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
// Allocate the state machines StateDef arrays.
void allocateStateDefs(
  MR::AttribDataStateMachineDef* stateMachineDef,
  uint32_t                       numSteadyStateNodes,
  uint32_t                       numTransitionStateNodes,
  NMP::Memory::Resource&         memRes)
{
  // Set StateDefs array.
  NMP_VERIFY((numSteadyStateNodes + numTransitionStateNodes) < MR::INVALID_STATE_ID)
  stateMachineDef->m_numStates = (MR::StateID) (numSteadyStateNodes + numTransitionStateNodes);
  NMP::Memory::Format memReqs;
  memReqs.size = NMP::Memory::align(stateMachineDef->m_numStates * sizeof(MR::StateDef), NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
  memRes.align(memReqs);
  if (stateMachineDef->m_numStates > 0)
    stateMachineDef->m_stateDefs = (MR::StateDef*) memRes.ptr;
  else
    stateMachineDef->m_stateDefs = NULL;
  memRes.increment(memReqs);
  for (uint32_t i = 0; i < stateMachineDef->m_numStates; ++i)
    wipeDownStateDef(&(stateMachineDef->m_stateDefs[i]));

  //---------------------------
  // Set single active state StateDef.
  memReqs.size = NMP::Memory::align(sizeof(MR::StateDef), NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
  memRes.align(memReqs);
  stateMachineDef->m_globalStateDef = (MR::StateDef*) memRes.ptr;
  memRes.increment(memReqs);
  wipeDownStateDef(stateMachineDef->m_globalStateDef);
}

//----------------------------------------------------------------------------------------------------------------------
// Fill in m_exitTransitionStateIDs for all states.
// Fill in local m_entryConditionIndexes of transitions.
void setExitTransitStateIDsAndEntryConditionIndexes(
  MR::AttribDataStateMachineDef* stateMachineDef,
  uint32_t                       numSteadyStateNodes,
  const MR::NodeID*              steadyStateNodeIDs,
  uint32_t                       numTransitionStateNodes,
  const MR::NodeID*              transitionNodeIDs,
  const ME::NetworkDefExport*    netDefExport,
  const ME::NodeExport*          nodeDefExport)
{
  // Allocate a working array to indicate if each transition has already been processed.
  bool* transitionsProcessed = allocateBoolArray(numTransitionStateNodes);  // Flags when each transition has been processed. Needed when we have shared active state transitions.

  
  // Steady state Nodes.
  int32_t stateDefIndex = 0;
  MR::StateDef* currentStateDef;
  MR::StateDef* transitionStateDef;
  MR::ConditionIndex conditionIndex = 0;
  for (uint32_t i = 0; i < numSteadyStateNodes; ++i)
  {
    currentStateDef = &stateMachineDef->m_stateDefs[stateDefIndex];
    const ME::NodeExport* nodeExport = netDefExport->getNode(steadyStateNodeIDs[i]);
    NMP_VERIFY(currentStateDef->m_numExitTransitionStates == nodeExport->getNumConditionSets());
    for (uint32_t j = 0; j < nodeExport->getNumConditionSets(); ++j)
    {
      const ME::ConditionSetExport* condSet = nodeExport->getConditionSet(j);
      MR::NodeID transitNodeID = (MR::NodeID) condSet->getTargetNodeID();

      // Set destination transition state index.
      currentStateDef->m_exitTransitionStateIDs[j] = stateMachineDef->findStateIDFromNodeID(transitNodeID);
      NMP_VERIFY(currentStateDef->m_exitTransitionStateIDs[j] < stateMachineDef->getNumStates());

      // Set the entry conditions indexes of the destination transition.
      uint32_t transitIndex = findNodeIDInArray(transitNodeID, transitionNodeIDs, numTransitionStateNodes);
      NMP_VERIFY((transitIndex != 0xFFFFFFFF) && (transitIndex < numTransitionStateNodes));
      transitionStateDef = &stateMachineDef->m_stateDefs[currentStateDef->m_exitTransitionStateIDs[j]];
      std::vector<uint32_t> conditionIndicesVec = condSet->getConditionIndices();
      NMP_VERIFY(transitionStateDef->getNumEntryConditions() == conditionIndicesVec.size());
      if (!transitionsProcessed[transitIndex])
      {
        // Add the conditions of this transition because we have not already processed them.
        for (uint32_t k = 0; k < conditionIndicesVec.size(); ++k)
        {
          transitionStateDef->m_entryConditionIndexes[k] = conditionIndex;
          ++conditionIndex;   
          NMP_VERIFY(conditionIndex < MR::INVALID_CONDITION_INDEX);
        }
      }
      transitionsProcessed[transitIndex] = true;
    }

    ++stateDefIndex;
  }

  // Transition Nodes.
  for (uint32_t i = 0; i < numTransitionStateNodes; ++i)
  {
    currentStateDef = &stateMachineDef->m_stateDefs[stateDefIndex];
    const ME::NodeExport* nodeExport = netDefExport->getNode(transitionNodeIDs[i]);
    NMP_VERIFY(currentStateDef->m_numExitTransitionStates == nodeExport->getNumConditionSets());
    for (uint32_t j = 0; j < nodeExport->getNumConditionSets(); ++j)
    {
      const ME::ConditionSetExport* condSet = nodeExport->getConditionSet(j);
      MR::NodeID transitNodeID = (MR::NodeID) condSet->getTargetNodeID();

      // Set destination transition state index.
      currentStateDef->m_exitTransitionStateIDs[j] = stateMachineDef->findStateIDFromNodeID(transitNodeID);
      NMP_VERIFY(currentStateDef->m_exitTransitionStateIDs[j] < stateMachineDef->getNumStates());

      // Set the entry conditions indexes of the destination transition.
      uint32_t transitIndex = findNodeIDInArray(transitNodeID, transitionNodeIDs, numTransitionStateNodes);
      NMP_VERIFY((transitIndex != 0xFFFFFFFF) && (transitIndex < numTransitionStateNodes));
      transitionStateDef = &stateMachineDef->m_stateDefs[currentStateDef->m_exitTransitionStateIDs[j]];
      std::vector<uint32_t> conditionIndicesVec = condSet->getConditionIndices();
      NMP_VERIFY(transitionStateDef->getNumEntryConditions() == conditionIndicesVec.size());
      if (!transitionsProcessed[transitIndex])
      {
        // Add the conditions of this transition because we have not already processed them.
        for (uint32_t k = 0; k < conditionIndicesVec.size(); ++k)
        {
          transitionStateDef->m_entryConditionIndexes[k] = conditionIndex;
          ++conditionIndex;
          NMP_VERIFY(conditionIndex < MR::INVALID_CONDITION_INDEX);
        }
      }
      transitionsProcessed[transitIndex] = true;
    }

    ++stateDefIndex;
  }
  
  // Global state.
  {
    currentStateDef = stateMachineDef->m_globalStateDef;
    NMP_VERIFY(currentStateDef->m_numExitTransitionStates == nodeDefExport->getNumCommonConditionSets());
    for (uint32_t j = 0; j <  nodeDefExport->getNumCommonConditionSets(); ++j)
    {
      const ME::ConditionSetExport* condSet = nodeDefExport->getCommonConditionSet(j);
      MR::NodeID transitNodeID = (MR::NodeID) condSet->getTargetNodeID();

      // Set destination transition state index.
      currentStateDef->m_exitTransitionStateIDs[j] = stateMachineDef->findStateIDFromNodeID(transitNodeID);
      NMP_VERIFY(currentStateDef->m_exitTransitionStateIDs[j] < stateMachineDef->getNumStates());

      // Set the entry conditions indexes of the destination transition.
      uint32_t transitIndex = findNodeIDInArray(transitNodeID, transitionNodeIDs, numTransitionStateNodes);
      NMP_VERIFY((transitIndex != 0xFFFFFFFF) && (transitIndex < numTransitionStateNodes));
      transitionStateDef = &stateMachineDef->m_stateDefs[currentStateDef->m_exitTransitionStateIDs[j]];
      std::vector<uint32_t> conditionIndicesVec = condSet->getConditionIndices();
      NMP_VERIFY(transitionStateDef->getNumEntryConditions() == conditionIndicesVec.size());
      if (!transitionsProcessed[transitIndex])
      {
        // Add the conditions of this transition because we have not already processed them.
        for (uint32_t k = 0; k < conditionIndicesVec.size(); ++k)
        {
          transitionStateDef->m_entryConditionIndexes[k] = conditionIndex;
          ++conditionIndex;
          NMP_VERIFY(conditionIndex < MR::INVALID_CONDITION_INDEX);
        }
      }
      transitionsProcessed[transitIndex] = true;
    }
  }

  // Tidy up working data.
  if (transitionsProcessed)
    delete [] transitionsProcessed;
}

//----------------------------------------------------------------------------------------------------------------------
// Build the state machines global array of transition conditions.
// Also orders transitions from each state based on their authored priorities.
void buildConditions(
  MR::AttribDataStateMachineDef* stateMachineDef,
  uint32_t                       numSteadyStateNodes,
  const MR::NodeID*              steadyStateNodeIDs,
  uint32_t                       numTransitionStateNodes,
  const MR::NodeID*              transitionNodeIDs,
  const ME::NetworkDefExport*    netDefExport,
  const ME::NodeExport*          nodeDefExport,
  AssetProcessor*                processor,
  NetworkDefCompilationInfo*     netDefCompilationInfo,
  NMP::Memory::Resource&         memRes,
  MR::NodeDef*                   nodeDef)
{
  // Allocate a working array to indicate if each transition has already been processed.
  bool* transitionsProcessed = allocateBoolArray(numTransitionStateNodes);  // Flags when each transition has been processed. Needed when we have shared active state transitions.

  // Store the highest priority found to decide what to prioritise first at runtime.
  stateMachineDef->m_prioritiseGlobalTransitions = false;
  int32_t maxPriority = 0;
  int32_t tempPriority = 0;
  
  //---------------------------
  // From steady state nodes.
  int32_t stateDefIndex = 0;
  MR::StateDef* currentStateDef;
  MR::StateDef* transitionStateDef;
  uint32_t conditionIndex = 0;
  for (uint32_t i = 0; i < numSteadyStateNodes; ++i)
  {
    currentStateDef = &stateMachineDef->m_stateDefs[stateDefIndex];
    const ME::NodeExport* nodeExport = netDefExport->getNode(steadyStateNodeIDs[i]);
    NMP_VERIFY(currentStateDef->m_numExitTransitionStates == nodeExport->getNumConditionSets());
    for (uint32_t j = 0; j < nodeExport->getNumConditionSets(); ++j)
    {
      const ME::ConditionSetExport* condSet = nodeExport->getConditionSet(j);
      MR::NodeID transitNodeID = (MR::NodeID) condSet->getTargetNodeID();

      uint32_t transitIndex = findNodeIDInArray(transitNodeID, transitionNodeIDs, numTransitionStateNodes);
      NMP_VERIFY((transitIndex != 0xFFFFFFFF) && (transitIndex < numTransitionStateNodes));
      transitionStateDef = &stateMachineDef->m_stateDefs[currentStateDef->m_exitTransitionStateIDs[j]];

      std::vector<uint32_t> conditionIndicesVec = condSet->getConditionIndices();
      NMP_VERIFY(transitionStateDef->getNumEntryConditions() == conditionIndicesVec.size());
      if (!transitionsProcessed[transitIndex])
      {
        // Add the conditions of this transition because we have not already processed them.
        for (uint32_t k = 0; k < conditionIndicesVec.size(); ++k)
        {
          NMP_VERIFY(conditionIndicesVec[k] < nodeExport->getNumConditions());
          const ME::ConditionExport* conditionExport = nodeExport->getCondition(conditionIndicesVec[k]);
          MR::TransitConditType conditionType = (MR::TransitConditType) conditionExport->getTypeID();
          TransitConditionDefBuilder* conditionBuilder = processor->getTransitConditionDefBuilder(conditionType);
          if (!conditionBuilder)
            processor->outputConditionError(conditionType, "Cannot find transit condition builder.");
          NMP_VERIFY(conditionBuilder);

          // Create the transit condition def itself
          stateMachineDef->m_conditions[conditionIndex] = conditionBuilder->init(memRes, conditionExport, netDefExport, processor, steadyStateNodeIDs[i]);
          // Make sure the condition builder succeeded.
          NMP_VERIFY_MSG(
                  stateMachineDef->m_conditions[conditionIndex],
                  "Condition %i of type %i in state machine ID %i did not compile successfully!\n",
                  j,
                  conditionType,
                  nodeDefExport->getNodeID());
          // Ensure that the condition function table is initialised
          MR::TransitConditionDef::relocate(stateMachineDef->m_conditions[conditionIndex]);

          // Register message interest
          if (conditionType == TRANSCOND_ON_MESSAGE_ID)
          {
            MR::TransitConditionDefOnMessage* messageConditionDef = 
              static_cast<MR::TransitConditionDefOnMessage*>(stateMachineDef->m_conditions[conditionIndex]);

            netDefCompilationInfo->registerMessageInterest(messageConditionDef->getMessageID(), nodeDef->getNodeID());
          }

          ++conditionIndex;
        }
      }
      transitionsProcessed[transitIndex] = true;
    }

    // Used to prioritize transitions.
    tempPriority = sortTransitionsBasedOnTransitPriority(currentStateDef, stateMachineDef, netDefExport);
    maxPriority = (tempPriority > maxPriority) ? tempPriority : maxPriority;
   
    ++stateDefIndex;
  }


  //---------------------------
  // From transition state nodes.
  for (uint32_t i = 0; i < numTransitionStateNodes; ++i)
  {
    currentStateDef = &stateMachineDef->m_stateDefs[stateDefIndex];
    const ME::NodeExport* nodeExport = netDefExport->getNode(transitionNodeIDs[i]);
    NMP_VERIFY(currentStateDef->m_numExitTransitionStates == nodeExport->getNumConditionSets());
    for (uint32_t j = 0; j < nodeExport->getNumConditionSets(); ++j)
    {
      const ME::ConditionSetExport* condSet = nodeExport->getConditionSet(j);
      MR::NodeID transitNodeID = (MR::NodeID) condSet->getTargetNodeID();
      uint32_t transitIndex = findNodeIDInArray(transitNodeID, transitionNodeIDs, numTransitionStateNodes);
      NMP_VERIFY((transitIndex != 0xFFFFFFFF) && (transitIndex < numTransitionStateNodes));
      transitionStateDef = &stateMachineDef->m_stateDefs[currentStateDef->m_exitTransitionStateIDs[j]];
      std::vector<uint32_t> conditionIndicesVec = condSet->getConditionIndices();
      NMP_VERIFY(transitionStateDef->getNumEntryConditions() == conditionIndicesVec.size());
      if (!transitionsProcessed[transitIndex])
      {
        // Add the conditions of this transition because we have not already processed them.
        for (uint32_t k = 0; k < conditionIndicesVec.size(); ++k)
        {
          NMP_VERIFY(conditionIndicesVec[k] < nodeExport->getNumConditions());
          const ME::ConditionExport* conditionExport = nodeExport->getCondition(conditionIndicesVec[k]);
          MR::TransitConditType conditionType = (MR::TransitConditType) conditionExport->getTypeID();
          TransitConditionDefBuilder* conditionBuilder = processor->getTransitConditionDefBuilder(conditionType);
          if (!conditionBuilder)
            processor->outputConditionError(conditionType, "Cannot find transit condition builder.");
          NMP_VERIFY(conditionBuilder);

          // Create the transit condition def itself
          stateMachineDef->m_conditions[conditionIndex] = conditionBuilder->init(memRes, conditionExport, netDefExport, processor, transitionNodeIDs[i]);
          // Make sure the condition builder succeeded.
          NMP_VERIFY_MSG(
                  stateMachineDef->m_conditions[conditionIndex],
                  "Condition %i of type %i in state machine ID %i did not compile successfully!\n",
                  j,
                  conditionType,
                  nodeDefExport->getNodeID());
          // Ensure that the condition function table is initialised
          MR::TransitConditionDef::relocate(stateMachineDef->m_conditions[conditionIndex]);

          // Register message interest
          if (conditionType == TRANSCOND_ON_MESSAGE_ID)
          {
            MR::TransitConditionDefOnMessage* messageConditionDef = 
              static_cast<MR::TransitConditionDefOnMessage*>(stateMachineDef->m_conditions[conditionIndex]);

            netDefCompilationInfo->registerMessageInterest(messageConditionDef->getMessageID(), nodeDef->getNodeID());
          }

          ++conditionIndex;
        }
      }
      transitionsProcessed[transitIndex] = true;
    }

    // Used to prioritize transitions.
    tempPriority = sortTransitionsBasedOnTransitPriority(currentStateDef, stateMachineDef, netDefExport);
    maxPriority = (tempPriority > maxPriority) ? tempPriority : maxPriority;
   
    ++stateDefIndex;
  }


  //---------------------------
  // Global/Active state conditions.
  {
    currentStateDef = stateMachineDef->m_globalStateDef;
    const ME::NodeExport* nodeExport = nodeDefExport;
    NMP_VERIFY(currentStateDef->m_numExitTransitionStates == nodeExport->getNumCommonConditionSets());
    for (uint32_t j = 0; j < nodeExport->getNumCommonConditionSets(); ++j)
    {
      const ME::ConditionSetExport* condSet = nodeExport->getCommonConditionSet(j);
      MR::NodeID transitNodeID = (MR::NodeID) condSet->getTargetNodeID();
      uint32_t transitIndex = findNodeIDInArray(transitNodeID, transitionNodeIDs, numTransitionStateNodes);
      NMP_VERIFY((transitIndex != 0xFFFFFFFF) && (transitIndex < numTransitionStateNodes));
      transitionStateDef = &stateMachineDef->m_stateDefs[currentStateDef->m_exitTransitionStateIDs[j]];
      std::vector<uint32_t> conditionIndicesVec = condSet->getConditionIndices();
      NMP_VERIFY(transitionStateDef->getNumEntryConditions() == conditionIndicesVec.size());
      if (!transitionsProcessed[transitIndex])
      {
        // Add the conditions of this transition because we have not already processed them.
        for (uint32_t k = 0; k < conditionIndicesVec.size(); ++k)
        {
          NMP_VERIFY(conditionIndicesVec[k] < nodeExport->getNumCommonConditions());
          const ME::ConditionExport* conditionExport = nodeExport->getCommonCondition(conditionIndicesVec[k]);
          MR::TransitConditType conditionType = (MR::TransitConditType) conditionExport->getTypeID();
          TransitConditionDefBuilder* conditionBuilder = processor->getTransitConditionDefBuilder(conditionType);
          if (!conditionBuilder)
            processor->outputConditionError(conditionType, "Cannot find transit condition builder.");
          NMP_VERIFY(conditionBuilder);

          // Create the transit condition def itself
          stateMachineDef->m_conditions[conditionIndex] = conditionBuilder->init(memRes, conditionExport, netDefExport, processor, MR::INVALID_NODE_ID);
          // Make sure the condition builder succeeded.
          NMP_VERIFY_MSG(
                  stateMachineDef->m_conditions[conditionIndex],
                  "Condition %i of type %i in state machine ID %i did not compile successfully!\n",
                  j,
                  conditionType,
                  nodeDefExport->getNodeID());
          // Ensure that the condition function table is initialised
          MR::TransitConditionDef::relocate(stateMachineDef->m_conditions[conditionIndex]);

          // Register message interest
          if (conditionType == TRANSCOND_ON_MESSAGE_ID)
          {
            MR::TransitConditionDefOnMessage* messageConditionDef = 
              static_cast<MR::TransitConditionDefOnMessage*>(stateMachineDef->m_conditions[conditionIndex]);

            netDefCompilationInfo->registerMessageInterest(messageConditionDef->getMessageID(), nodeDef->getNodeID());
          }

          ++conditionIndex;
        }
      }
      transitionsProcessed[transitIndex] = true;
    }
   
    // used to prioritize transitions
    tempPriority = sortTransitionsBasedOnTransitPriority(currentStateDef, stateMachineDef, netDefExport);
    if (tempPriority > maxPriority)
      stateMachineDef->m_prioritiseGlobalTransitions = true;
  }

  // Tidy up working data.
  if (transitionsProcessed)
    delete [] transitionsProcessed;
}

//----------------------------------------------------------------------------------------------------------------------
void allocateConditionPointersArray(
  MR::AttribDataStateMachineDef* stateMachineDef,
  MR::ConditionIndex             totalNumTransitConditions,
  NMP::Memory::Resource&         memRes)
{
  stateMachineDef->m_numConditions = totalNumTransitConditions;
  NMP::Memory::Format memReqs;
  memReqs.size = NMP::Memory::align(stateMachineDef->m_numConditions * sizeof(MR::TransitConditionDef*), NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
  memRes.align(memReqs);
  if (stateMachineDef->m_numConditions > 0)
    stateMachineDef->m_conditions = (MR::TransitConditionDef**) memRes.ptr;
  else
    stateMachineDef->m_conditions = NULL;
  memRes.increment(memReqs);
}

//----------------------------------------------------------------------------------------------------------------------
// Copy condition indexes from transitions to their source states.
void copyStateDefExitConditionIndexes(MR::AttribDataStateMachineDef* stateMachineDef)
{
  //---------------------------
  // Static and Transition states.
  MR::StateDef* currentStateDef;
  MR::StateDef* transitionStateDef;
  for (uint32_t i = 0; i < stateMachineDef->m_numStates; ++i)
  {
    currentStateDef = &stateMachineDef->m_stateDefs[i];
    MR::ConditionIndex currentStateConditionIndex = 0;
    for (uint32_t j = 0; j < currentStateDef->m_numExitTransitionStates; ++j)
    {
      transitionStateDef = &stateMachineDef->m_stateDefs[currentStateDef->m_exitTransitionStateIDs[j]];
      for (MR::ConditionIndex k = 0; k < transitionStateDef->getNumEntryConditions(); ++k)
      {
        NMP_VERIFY(currentStateConditionIndex < currentStateDef->getNumExitConditions());
        currentStateDef->m_exitConditionIndexes[currentStateConditionIndex] = transitionStateDef->getEntryConditionStateMachineIndex(k);
        ++currentStateConditionIndex;
      }
    }
  }

  //---------------------------
  // Global state.
  {
    currentStateDef = stateMachineDef->m_globalStateDef;
    MR::ConditionIndex currentStateConditionIndex = 0;
    for (uint32_t j = 0; j < currentStateDef->m_numExitTransitionStates; ++j)
    {
      transitionStateDef = &stateMachineDef->m_stateDefs[currentStateDef->m_exitTransitionStateIDs[j]];
      for (MR::ConditionIndex k = 0; k < transitionStateDef->getNumEntryConditions(); ++k)
      {
        NMP_VERIFY(currentStateConditionIndex < currentStateDef->getNumExitConditions());
        currentStateDef->m_exitConditionIndexes[currentStateConditionIndex] = transitionStateDef->getEntryConditionStateMachineIndex(k);
        ++currentStateConditionIndex;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Set the default starting state index.
void setDefaultStartingStateIndex(
  MR::AttribDataStateMachineDef* stateMachineDef,
  const ME::DataBlockExport*     nodeDefDataBlock)
{
  // The default starting state index is a global NodeID in the XML we need to convert it to a local state index.
  uint32_t defaultStartingNodeID;
  nodeDefDataBlock->readUInt(defaultStartingNodeID, "DefaultNodeID");
  stateMachineDef->m_defaultStartingStateID = stateMachineDef->findStateIDFromNodeID((MR::NodeID)defaultStartingNodeID);
}

//----------------------------------------------------------------------------------------------------------------------
// Count the number of breakout transitions per StateDef and flag breakout conditions in a working parallel breakoutCondition array.
// Also in-place reorder conditions in each state definition so that breakouts appear first in the array.
void processBreakoutTransitions(
  MR::AttribDataStateMachineDef* stateMachineDef,
  const ME::NetworkDefExport*    netDefExport)
{
  // Temporary workspace for identifying conditions from breakout transits
  bool* breakoutConditFlags = NULL;             // Flags against all conditions
  bool* workingBreakoutConditFlags = NULL;      // Flags only used on the current states conditions.
  MR::ConditionIndex totalNumTransitConditions = stateMachineDef->getNumConditions();
  if (totalNumTransitConditions)
  {
    breakoutConditFlags = new bool[totalNumTransitConditions];
    memset(breakoutConditFlags, 0, sizeof(bool) * totalNumTransitConditions);
    workingBreakoutConditFlags = new bool[totalNumTransitConditions];
  }

  // Calculate the total number of breakout transition conditions from every state
  // and flag every condition as breakout condition or not.
  MR::StateDef* currentStateDef;
  MR::StateDef* transitionStateDef;
  // Loop through all states.
  for (uint32_t i = 0; i < stateMachineDef->m_numStates; ++i)
  {
    currentStateDef = &stateMachineDef->m_stateDefs[i];

    // Clear the working breakout condition flags array.
    memset(workingBreakoutConditFlags, 0, sizeof(bool) * totalNumTransitConditions);

    // Reset out counter.
    MR::ConditionIndex numBreakoutConditions = 0;
    
    // Loop through all transitions from this state.
    for (uint32_t j = 0; j < currentStateDef->m_numExitTransitionStates; ++j)
    {
      transitionStateDef = &stateMachineDef->m_stateDefs[currentStateDef->m_exitTransitionStateIDs[j]];

      // Get breakout transition flag.
      bool breakoutTransit = false;
      const ME::DataBlockExport* nodeDefDataBlock = netDefExport->getNode(transitionStateDef->getNodeID())->getDataBlock();
      nodeDefDataBlock->readBool(breakoutTransit, "BreakoutTransit");

      if (breakoutTransit)
      {
        // This is a breakout transition, so all of its conditions are break out conditions.
        for (uint32_t k = 0; k < transitionStateDef->getNumEntryConditions(); ++k)
        {
          MR::ConditionIndex conditionIndex = transitionStateDef->m_entryConditionIndexes[k];
          numBreakoutConditions += (breakoutTransit && !workingBreakoutConditFlags[conditionIndex]) ? 1 : 0;
          NMP_VERIFY(numBreakoutConditions < MR::INVALID_CONDITION_INDEX);
          breakoutConditFlags[conditionIndex] |= breakoutTransit;
          workingBreakoutConditFlags[conditionIndex] |= breakoutTransit;
        }
      }
    }

    // Store total breakout transits per StateDef and move to next block
    currentStateDef->m_numExitBreakoutConditions = numBreakoutConditions;
  }

  //---------------------------
  // In-place reorder of conditions in each state definition so that breakouts appear first in the array.
  for (uint32_t i = 0; i < stateMachineDef->m_numStates; ++i)
  {
    currentStateDef = &stateMachineDef->m_stateDefs[i];

    MR::ConditionIndex numBreakoutConditions = 0;
    for (uint32_t j = 0; j < currentStateDef->m_numExitConditions; ++j)
    {
      MR::ConditionIndex conditionIndex = currentStateDef->m_exitConditionIndexes[j];
      if (breakoutConditFlags[conditionIndex])
      {
        // Swap current breakout condition index with first non-breakout condition index
        MR::ConditionIndex temp = currentStateDef->m_exitConditionIndexes[numBreakoutConditions];
        currentStateDef->m_exitConditionIndexes[numBreakoutConditions] = conditionIndex;
        currentStateDef->m_exitConditionIndexes[j] = temp;
        numBreakoutConditions++;
        NMP_VERIFY(numBreakoutConditions < MR::INVALID_CONDITION_INDEX);
      }
    }
    NMP_VERIFY(currentStateDef->m_numExitBreakoutConditions == numBreakoutConditions);
  }

  // Clean up working memory.
  if (totalNumTransitConditions)
  {
    delete [] breakoutConditFlags;
    delete [] workingBreakoutConditFlags;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void findSourceAndDestStateIDsForTransitions(
  MR::AttribDataStateMachineDef* stateMachineDef,
  const ME::NetworkDefExport*    netDefExport,
  uint32_t                       numSteadyStateNodes,
  uint32_t                       numTransitionStateNodes)
{
#ifdef MR_INCLUDE_CONNECTIVITY
  MR::StateDef* transitionStateDef;
  uint32_t startTransitStateID = numSteadyStateNodes;
  uint32_t entTransitStateID = numSteadyStateNodes + numTransitionStateNodes;
  for (uint32_t i = startTransitStateID; i < entTransitStateID; ++i)
  {
    transitionStateDef = &stateMachineDef->m_stateDefs[i];
    MR::NodeID transitNodeID = transitionStateDef->getNodeID();

    // Read and set the source StateID.
    const ME::DataBlockExport* nodeDefDataBlock = netDefExport->getNode(transitNodeID)->getDataBlock();
    MR::NodeID sourceNodeID;
    readNodeID(nodeDefDataBlock, "SourceNodeID", sourceNodeID, true);
    transitionStateDef->m_sourceStateID = stateMachineDef->findStateIDFromNodeID(sourceNodeID);

    // Read and set the destination StateID.
    MR::NodeID destNodeID;
    readNodeID(nodeDefDataBlock, "DestNodeID", destNodeID, true);
    transitionStateDef->m_destinationStateID = stateMachineDef->findStateIDFromNodeID(destNodeID);
  }
#else // MR_INCLUDE_CONNECTIVITY
  (void) stateMachineDef;
  (void) netDefExport;
  (void) numSteadyStateNodes;
  (void) numTransitionStateNodes;
#endif // MR_INCLUDE_CONNECTIVITY
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeStateMachineBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_STATE_MACHINE, "Expecting node type STATE_MACHINE");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Find transitional State Node IDs and steady State Node IDs.
  // Find out and store how many conditions there are on every transition.
  uint32_t            numSteadyStateNodes = 0;
  MR::NodeID*         steadyStateNodeIDs = NULL;
  uint32_t            numTransitionStateNodes = 0;
  MR::NodeID*         transitionNodeIDs = NULL;
  MR::ConditionIndex  totalNumTransitConditions = 0;
  MR::ConditionIndex* transitionEntryConditionCounts = NULL;
  MR::ConditionIndex* numExitConditionsPerState = NULL;
  MR::ConditionIndex  numExitConditionsGlobalState = 0;
  findChildStatesInfo(
                netDefExport,
                nodeDefExport,
                numSteadyStateNodes,
                steadyStateNodeIDs,
                numTransitionStateNodes,
                transitionNodeIDs,
                totalNumTransitConditions,
                transitionEntryConditionCounts,
                numExitConditionsPerState,
                numExitConditionsGlobalState);

  //---------------------------
  // Initialise the NodeDef itself
  uint32_t numChildren = numSteadyStateNodes + numTransitionStateNodes;
  NMP_VERIFY(numChildren < MR::INVALID_STATE_ID)
  MR::NodeDef* nodeDef = initCoreNodeDef(
                              memRes,                  
                              netDefCompilationInfo,   
                              numChildren,            // numChildren
                              1,                      // max num active child nodes
                              0,                      // numInputCPConnections
                              0,                      // numOutputCPPins
                              1,                      // numAnimSetEntries
                              nodeDefExport);

  // Flag node as a state machine and a filter node.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_STATE_MACHINE | MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Initialise the StateMachineDef attribute data.
  MR::AttribDataStateMachineDef* stateMachineDef = MR::AttribDataStateMachineDef::init(memRes, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
                nodeDef,
                MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                    // semantic,    
                0,                                                        // animSetIndex,
                stateMachineDef,                                          // attribData,  
                MR::AttribDataStateMachineDef::getMemoryRequirements());  // attribMemReqs
  
  //---------------------------
  // Allocate the state machines StateDef arrays.
  allocateStateDefs(
                stateMachineDef,
                numSteadyStateNodes,
                numTransitionStateNodes,
                memRes);

  //---------------------------
  // For each state allocate the internal index arrays for transition node ids and conditions.
  bool shouldExtendOutputLifespans;
  allocateIndexingArraysForStateDefs(
                numSteadyStateNodes,
                steadyStateNodeIDs,
                numTransitionStateNodes,
                transitionNodeIDs,
                netDefCompilationInfo,
                netDefExport,
                nodeDefExport,
                nodeDef,
                stateMachineDef,
                transitionEntryConditionCounts,
                shouldExtendOutputLifespans,
                memRes,
                numExitConditionsPerState,
                numExitConditionsGlobalState);

  //---------------------------
  // Fill in m_exitTransitionStateIDs for all states and fill in local m_entryConditionIndexes of transitions.
  setExitTransitStateIDsAndEntryConditionIndexes(
                stateMachineDef,
                numSteadyStateNodes,
                steadyStateNodeIDs,
                numTransitionStateNodes,
                transitionNodeIDs,
                netDefExport,
                nodeDefExport);

  //---------------------------
  // Copy condition indexes from transitions to their source states.
  copyStateDefExitConditionIndexes(stateMachineDef);
  
  //---------------------------
  // Allocate the condition pointers array.
  allocateConditionPointersArray(stateMachineDef, totalNumTransitConditions, memRes);
 
  //---------------------------
  // Build the conditions.
  buildConditions(
                stateMachineDef,
                numSteadyStateNodes,
                steadyStateNodeIDs,
                numTransitionStateNodes,
                transitionNodeIDs,
                netDefExport,
                nodeDefExport,
                processor,
                netDefCompilationInfo,
                memRes,
                nodeDef);

  //---------------------------
  // Set the default starting state index.
  setDefaultStartingStateIndex(stateMachineDef, nodeDefDataBlock);

  //---------------------------
  // Count the number of breakout transitions per StateDef and flag breakout conditions in a working parallel breakoutCondition array.
  // Also in-place reorder conditions in each state definition so that breakouts appear first in the array.
  processBreakoutTransitions(stateMachineDef, netDefExport);

  //---------------------------
  // For all transition states find their source and destination states. This allows for fast query of connectivity at runtime.
  findSourceAndDestStateIDsForTransitions(stateMachineDef, netDefExport, numSteadyStateNodes, numTransitionStateNodes);


  //---------------------------
  // Clean up working memory.
  if (numSteadyStateNodes)
  {
    delete [] steadyStateNodeIDs;
  }
  if (numTransitionStateNodes)
  {
    delete [] transitionNodeIDs;
    delete [] transitionEntryConditionCounts;
  }
  if (numExitConditionsPerState)
  {
    delete [] numExitConditionsPerState;
  }

  //---------------------------
  // Initialise the nodedef.
  initTaskQueuingFns(nodeDef, netDefCompilationInfo, processor->getMessageLogger(), shouldExtendOutputLifespans);

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeStateMachineBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NMP::BasicLogger*           logger,
  bool                        shouldExtendOutputLifespans)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = (MR::QueueAttrTaskFn*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  const uint32_t numEntries = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    // Most queuing requests get passed on to our child.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0), logger);
  }

  // If the state machine might dead blend or transition to physics then use the state machine specific pass through
  // task which extends the lifespan of the attr data so that its available for dead blending
  if (shouldExtendOutputLifespans)
  {
    nodeDef->setTaskQueuingFnId(
      taskQueuingFns,
      MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
      FN_NAME(stateMachineNodeQueuePassThrough),
      logger);

    nodeDef->setTaskQueuingFnId(
      taskQueuingFns,
      MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
      FN_NAME(stateMachineNodeQueuePassThrough),
      logger);

    nodeDef->setTaskQueuingFnId(
      taskQueuingFns,
      MR::ATTRIB_SEMANTIC_TRAJECTORY_TRANSFORM,
      FN_NAME(stateMachineNodeQueuePassThrough),
      logger);

    nodeDef->setTaskQueuingFnId(
      taskQueuingFns,
      MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
      FN_NAME(stateMachineNodeQueuePassThrough),
      logger);
  }

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeStateMachineDeleteInstance), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeStateMachineUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeStateMachineFindGeneratingNodeForSemantic), logger);
  nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeStateMachineInitInstance), logger);
  nodeDef->setMessageHandlerFnId(FN_NAME(nodeStateMachineHandleMessages), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
