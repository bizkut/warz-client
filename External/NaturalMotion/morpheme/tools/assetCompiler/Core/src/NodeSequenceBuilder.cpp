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
#include "NodeSequenceBuilder.h"
#include "morpheme/Nodes/mrNodeSequence.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{
  
//----------------------------------------------------------------------------------------------------------------------
bool NodeSequenceBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeSequenceBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_SEQUENCE);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CP_BOOL);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeSequenceBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  int32_t connectedPinCount = 0;
  nodeDefDataBlock->readInt(connectedPinCount, "ConnectedPinCount");
  NMP_VERIFY(connectedPinCount > 0);

  // Set child node IDs.
  MR::NodeID sourceNodeID;
  CHAR paramName[256];
  for (uint32_t i = 0; i < (uint32_t) connectedPinCount; ++i)
  {
    sprintf_s(paramName, "ConnectedNodeID_%d", i);
    readNodeID(nodeDefDataBlock, paramName, sourceNodeID);
    childNodeIDs.push_back(sourceNodeID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeSequenceBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeSequenceBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  int32_t connectedPinCount = 0;
  nodeDefDataBlock->readInt(connectedPinCount, "ConnectedPinCount");
  NMP_VERIFY(connectedPinCount > 0);

  //---------------------------
  // Allocate space for the NodeDef itself
  // Note: we only allow space for AttribData in anim set 0, this is ok because we create no data for any other anim set.
  uint32_t inputCPCount = 0;
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    connectedPinCount,  // numChildren
    inputCPCount,       // numInputCPConnections
    1,                  // numAnimSetEntries
    nodeDefExport);

  // Looping
  result += MR::AttribDataBool::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeSequenceBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_SEQUENCE, "Expecting node type SEQUENCE");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  int32_t connectedPinCount = 0;
  nodeDefDataBlock->readInt(connectedPinCount, "ConnectedPinCount");
  NMP_VERIFY(connectedPinCount > 0);

  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    connectedPinCount, // numChildren
    1,                 // max num active child nodes
    0,                 // numInputCPConnections
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
  for (uint32_t i = 0; i < (uint32_t) connectedPinCount; ++i)
  {
    sprintf_s(paramName, "ConnectedNodeID_%d", i);
    readNodeID(nodeDefDataBlock, paramName, sourceNodeID);
    nodeDef->setChildNodeID(i, sourceNodeID);
  }

  //---------------------------
  // Initialise the attrib datas.
  
  // Looping
  bool loopVal = false;
  nodeDefDataBlock->readBool(loopVal, "Loop");
  MR::AttribDataBool* loopable = MR::AttribDataBool::init(memRes, loopVal, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_CP_BOOL,                   // semantic,    
    0,                                             // animSetIndex,
    loopable,                                      // attribData,  
    MR::AttribDataBool::getMemoryRequirements());  // attribMemReqs

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeSequenceBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NMP::BasicLogger*           logger)
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

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeSequenceAndSwitchDeleteInstance), logger);
  nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeShareInitInstanceInvalidateAllChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeSequenceUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(filterNodeFindGeneratingNodeForSemantic), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
