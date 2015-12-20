//----------------------------------------------------------------------------------------------------------------------
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
#include "NodeOperatorVector3ToFloatsBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodeOperatorVector3ToFloats.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorVector3ToFloatsBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Control parameter
  readDataPinChildNodeID(nodeDefDataBlock, "Input", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorVector3ToFloatsBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  //---------------------------
  // Input Control Param Connections.
  declareDataPin(netDefCompilationInfo, nodeDefExport,"Input", 0, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorVector3ToFloatsBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  uint32_t inputCPCount = 1;
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    0,            // numChildren
    inputCPCount, // numInputCPConnections
    0,            // numAnimSetEntries
    nodeDefExport);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorVector3ToFloatsBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_VECTOR3_TO_FLOATS);

  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    0,                                                      // numChildren
    0,                                                      // max num active child nodes
    1,                                                      // numInputCPConnections
    MR::NODE_OPERATOR_VECTOR3_TO_FLOATS_OUT_CP_PINID_COUNT, // numOutputCPPins
    0,                                                      // numAnimSetEntries                     
    nodeDefExport);

  // Flag node as an operator.
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorVector3ToFloatsBuilder::initTaskQueuingFns(
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
                  MR:: NODE_OPERATOR_VECTOR3_TO_FLOATS_OUT_CP_PINID_X,
                  MR::ATTRIB_SEMANTIC_CP_FLOAT,
                  FN_NAME(nodeOperatorVector3ToFloatsOutputCPUpdate),
                  logger);
  nodeDef->setOutputCPTaskFnId(
                  outputCPTaskFns,
                  MR::NODE_OPERATOR_VECTOR3_TO_FLOATS_OUT_CP_PINID_Y,
                  MR::ATTRIB_SEMANTIC_CP_FLOAT,
                  FN_NAME(nodeOperatorVector3ToFloatsOutputCPUpdate),
                  logger);
  nodeDef->setOutputCPTaskFnId(
                  outputCPTaskFns,
                  MR::NODE_OPERATOR_VECTOR3_TO_FLOATS_OUT_CP_PINID_Z,
                  MR::ATTRIB_SEMANTIC_CP_FLOAT,
                  FN_NAME(nodeOperatorVector3ToFloatsOutputCPUpdate),
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
  nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeOperatorVector3ToFloatsInitInstance), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
