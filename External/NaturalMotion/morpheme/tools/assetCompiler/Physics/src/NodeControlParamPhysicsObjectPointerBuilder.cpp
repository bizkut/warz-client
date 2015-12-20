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
#include "NodeControlParamPhysicsObjectPointerBuilder.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/Nodes/mrNodeControlParamInt.h"
#include "morpheme/mrAttribData.h"
#include "NetworkDefBuilder.h"
#include "physics/Nodes/mrNodeControlParamPhysicsObjectPointer.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeControlParamPhysicsObjectPointerBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeControlParamPhysicsObjectPointerBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_PHYSICS_OBJECT_POINTER);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeControlParamPhysicsObjectPointerBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDatablock = nodeDefExport->getDataBlock();
  uint32_t inputPathsCount = 0;
  nodeDefDatablock->readUInt(inputPathsCount, "InputPathCount");
  getEmittedControlParamNodeDefInputConnections(childNodeIDs, nodeDefDatablock, inputPathsCount);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeControlParamPhysicsObjectPointerBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeControlParamPhysicsObjectPointerBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  // Create a description of the block of memory
  const ME::DataBlockExport* nodeDefDatablock = nodeDefExport->getDataBlock();
  uint32_t inputPathsCount = 0;
  nodeDefDatablock->readUInt(inputPathsCount, "InputPathCount");
  VALIDATE_OUTPUT_CONTROL_PARAMS(inputPathsCount, nodeDefDatablock);

  // Using the nodes m_inputCPConnections array to store the child paths that can potentially be active.
  // Using the nodes m_childNodeIDs array to store the ID of a node that must be active for the associated child path to be updated.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    inputPathsCount,    // numChildren
    inputPathsCount,    // numInputCPConnections
    1,                  // numAnimSetEntries
    nodeDefExport);

  // Add space for default control param attrib value.
  result += MR::AttribDataPhysicsObjectPointer::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeControlParamPhysicsObjectPointerBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_CP_PHYSICS_OBJECT_POINTER, "Expecting node type CP_PHYSICS_OBJECT_POINTER");
  const ME::DataBlockExport* nodeDefDatablock = nodeDefExport->getDataBlock();

  uint32_t inputPathsCount = 0;
  nodeDefDatablock->readUInt(inputPathsCount, "InputPathCount");
  VALIDATE_OUTPUT_CONTROL_PARAMS(inputPathsCount, nodeDefDatablock);

  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    inputPathsCount,            // numChildren
    (uint16_t)inputPathsCount,  // max num active child nodes
    inputPathsCount,            // numInputCPConnections
    1,                          // numOutputCPPins
    1,                          // numAnimSetEntries                     
    nodeDefExport);

  // Flag node as a control param.
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM);

  // If this is an Emitted control parameter init an array of connected children and set some flags.
  initEmittedControlParamData(nodeDef, nodeDefExport->getName(), nodeDefDatablock, inputPathsCount);

  //---------------------------
  // Add attributes.
  void* defaultPointer = 0;
  MR::AttribDataPhysicsObjectPointer* defaultValueAttrib = 
    MR::AttribDataPhysicsObjectPointer::init(memRes, defaultPointer, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER, // semantic,    
    0,                                             // animSetIndex,
    defaultValueAttrib,                            // attribData,  
    MR::AttribDataPhysicsObjectPointer::getMemoryRequirements());  // attribMemReqs

  //---------------------------
  // Task queuing and outputCP functions
  initTaskQueuingFns(nodeDef, netDefCompilationInfo, processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeControlParamPhysicsObjectPointerBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  
  // Never support being an emitted CP.
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Register the shared task function tables
  nodeDef->registerEmptyTaskQueuingFns(taskQueuingFnTables);
  
  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceNULL), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsNULL), logger);
  nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeControlParamPhysicsObjectPointerInitInstance), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
