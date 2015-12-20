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
#include "NodeSwitchBuilder.h"
#include "morpheme/Nodes/mrNodeSwitch.h"
#include "morpheme/Nodes/mrNodeSequence.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{
  
//----------------------------------------------------------------------------------------------------------------------
bool NodeSwitchBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeSwitchBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_SWITCH);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeSwitchBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount >= 2); // It is not valid to have a node with only one input (this is checked in validate).

  // Set child node IDs.
  CHAR paramName[256];
  for (uint32_t i = 0; i < (uint32_t) sourceNodeCount; ++i)
  {
    MR::NodeID sourceNodeID;
    sprintf_s(paramName, "Source%dNodeID", i);
    readNodeID(nodeDefDataBlock, paramName, sourceNodeID);
    childNodeIDs.push_back(sourceNodeID);
  }

  // Control parameters
  readDataPinChildNodeID(nodeDefDataBlock, "Weight", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeSwitchBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
    declareDataPin(netDefCompilationInfo, nodeDefExport, "Weight", 0, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeSwitchBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount >= 2); // It is not valid to have a node with only one input (this is checked in validate).

  //---------------------------
  // Allocate space for the NodeDef itself.
  // Note: we only allow space for AttribData in anim set 0, this is ok because we create no data for any other anim set.
  uint32_t controlParamCount = 1;
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    sourceNodeCount,    // numChildren
    controlParamCount,  // numInputCPConnections
    1,                  // numAnimSetEntries
    nodeDefExport);

  //---------------------------
  // Reserve space for the source weights array.
  int32_t weightCount = sourceNodeCount;
  bool wrapWeights = false;
  nodeDefDataBlock->readBool(wrapWeights, "WrapWeights");
  if (wrapWeights)
  {
    ++weightCount;
  }
  result += MR::AttribDataFloatArray::getMemoryRequirements(weightCount);

  //---------------------------
  // Attribute datas.
  result += MR::AttribDataSwitchDef::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeSwitchBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_SWITCH, "Expecting node type SWITCH");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount >= 2); // It is not valid to have a node with only one input (this is checked in validate).

  //---------------------------
  // Initialise the NodeDef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    sourceNodeCount,   // numChildren
    1,                 // max num active child nodes
    1,                 // numInputCPConnections
    0,                 // numOutputCPPins
    1,                 // numAnimSetEntries                     
    nodeDefExport);

  // Flag node as a filter.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Set child node IDs.
  MR::NodeID sourceNodeID;
  CHAR paramName[256];
  for (uint32_t i = 0; i < (uint32_t) sourceNodeCount; ++i)
  {
    sprintf_s(paramName, "Source%dNodeID", i);
    readNodeID(nodeDefDataBlock, paramName, sourceNodeID);
    nodeDef->setChildNodeID(i, sourceNodeID);
  }

  // If we wrap the weights an additional wrap weight must be added.
  int32_t weightCount = sourceNodeCount;
  bool wrapWeights = false;
  nodeDefDataBlock->readBool(wrapWeights, "WrapWeights");
  if (wrapWeights)
  {
    ++weightCount;
  }

  //---------------------------
  // Initialise the attrib datas.
  
  //---------------------------
  // Add the source weights array.
  MR::AttribDataFloatArray* childNodeWeights = MR::AttribDataFloatArray::init(memRes, weightCount, MR::IS_DEF_ATTRIB_DATA);
  float sourceNodeWeight;
  for (uint32_t i = 0; i < (uint32_t) sourceNodeCount; ++i)
  {
    sprintf_s(paramName, "SourceWeight_%d", i);
    sourceNodeWeight = 0.0f;
    nodeDefDataBlock->readFloat(sourceNodeWeight, paramName);
    childNodeWeights->m_values[i] = sourceNodeWeight;
  }

  if (wrapWeights)
  {
    float wrapWeight = 0.0f;
    nodeDefDataBlock->readFloat(wrapWeight, "WrapWeight");
    childNodeWeights->m_values[sourceNodeCount] = wrapWeight;
  }
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS,                             // semantic
    0,                                                                  // animSetIndex
    childNodeWeights,                                                   // attribData
    MR::AttribDataFloatArray::getMemoryRequirements(sourceNodeCount));  // attribMemReqs

  MR::AttribDataSwitchDef::EvaluateMode evalMode;
  nodeDefDataBlock->readUInt((uint32_t&)evalMode, "EvaluationMethod");

  MR::AttribDataSwitchDef::InputSelectionMethod inputSelectionMethod;
  nodeDefDataBlock->readUInt((uint32_t&)inputSelectionMethod, "InputSelectionMethod");
  MR::AttribDataSwitchDef* switchAttribData = MR::AttribDataSwitchDef::init(memRes, evalMode, inputSelectionMethod, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                              // semantic
    0,                                                                  // animSetIndex
    switchAttribData,                                                   // attribData
    MR::AttribDataSwitchDef::getMemoryRequirements());                  // attribMemReqs

  //---------------------------
  // Initialise the task function tables.
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeSwitchBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_VERIFY(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = (MR::QueueAttrTaskFn*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  const uint32_t numEntries = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    // Most queuing requests get passed on to our active child.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0), logger);
  }

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeSequenceAndSwitchDeleteInstance), logger);
  nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeShareInitInstanceInvalidateAllChildrenInitPinAttrib), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeSwitchUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(filterNodeFindGeneratingNodeForSemantic), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
