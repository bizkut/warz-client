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
#include "NodeOperatorVector3AngleBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodeOperatorVector3Angle.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeOperatorVector3AngleBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeOperatorVector3AngleBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_VECTOR3_ANGLE);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorVector3AngleBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Control parameter
  readDataPinChildNodeID(nodeDefDataBlock, "Input0", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "Input1", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorVector3AngleBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport,"Input0", 0, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
  declareDataPin(netDefCompilationInfo, nodeDefExport,"Input1", 1, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);

}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorVector3AngleBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  uint32_t numInputCPs = 2;
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    0,               // numChildren
    numInputCPs,     // numInputCPConnections
    1,               // numAnimSetEntries
    nodeDefExport);

  result += MR::AttribDataVector3::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorVector3AngleBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_VECTOR3_ANGLE, "Expecting node type CP_OP_VECTOR3_ANGLE");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Initialise the NodeDef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    0,        // numChildren
    0,        // max num active child nodes
    2,        // numInputCPConnections
    1,        // numOutputCPPins
    1,        // numAnimSetEntries                     
    nodeDefExport);

  // Flag node as an operator.
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);

  //---------------------------
  // Initialise the attrib datas.
  int axisID;
  nodeDefDataBlock->readInt(axisID, "Axis");
  NMP::Vector3 axis(0,0,0);
  if (axisID > 0)
  {
    axis[axisID-1] = 1.0f; // Set one of the primary axes
  }

  MR::AttribDataVector3* angleAttribData = MR::AttribDataVector3::init(
    memRes, 
    axis,
    MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                        // semantic
    0,                                                            // animSetIndex
    angleAttribData,                                               // attribData
    MR::AttribDataVector3::getMemoryRequirements()); // attribMemReqs

  //---------------------------
  // Initialise the task function tables.
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorVector3AngleBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::OutputCPTask* outputCPTaskFns = (MR::OutputCPTask*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  nodeDef->setOutputCPTaskFnId(
    outputCPTaskFns,
    0,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    FN_NAME(nodeOperatorVector3AngleOutputCPUpdate),
    logger);

  // Register the shared task function tables
  nodeDef->registerEmptyTaskQueuingFns(taskQueuingFnTables);
  nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);

  // Tidy up
  NMP::Memory::memFree(outputCPTaskFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceNoChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsNULL), logger);
  nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeShareInitInstanceCreateFloatOutputAttributeInitPinAttrib), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
