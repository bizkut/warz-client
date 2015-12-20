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
#include "NodeControlParamUIntBuilder.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/Nodes/mrNodeControlParamUInt.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrAttribData.h"
#include "NetworkDefBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeControlParamUIntBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeControlParamUIntBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_UINT);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CP_UINT);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeControlParamUIntBuilder::getNodeDefInputConnections(
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
void NodeControlParamUIntBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeControlParamUIntBuilder::getNodeDefMemoryRequirements(
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
  result += MR::AttribDataUInt::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeControlParamUIntBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_CP_UINT, "Expecting node type CP_UINT");
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
  uint32_t defaultInt = 0;
  nodeDefDatablock->readUInt(defaultInt, "DefaultInt");
  MR::AttribDataUInt* defaultValueAttrib = MR::AttribDataUInt::init(memRes, defaultInt, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_CP_UINT,                   // semantic,    
    0,                                             // animSetIndex,
    defaultValueAttrib,                            // attribData,  
    MR::AttribDataUInt::getMemoryRequirements());  // attribMemReqs

  //---------------------------
  // Task queuing and outputCP functions
  initTaskQueuingFns(nodeDef, netDefCompilationInfo, processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeControlParamUIntBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  
  // Set the outputCP update task pointer.
  if (nodeDef->getNodeFlags().areSet(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE)) // Are we an Emitted control param.
  {
    MR::OutputCPTask* outputCPTaskFns = (MR::OutputCPTask*)MR::SharedTaskFnTables::createSharedTaskFnTable();
    nodeDef->setOutputCPTaskFnId(outputCPTaskFns, 0, MR::ATTRIB_SEMANTIC_CP_UINT, FN_NAME(nodeControlParamUIntEmittedCPUpdateUInt), logger);
    nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);
    NMP::Memory::memFree(outputCPTaskFns);
  }
  else
  {
    nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);
  }

  // Register the shared task function tables
  nodeDef->registerEmptyTaskQueuingFns(taskQueuingFnTables);
  
  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceNULL), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsNULL), logger);
  nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeControlParamUIntInitInstance), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
