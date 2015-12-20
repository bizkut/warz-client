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
#include "NodeOperatorSmoothFloatBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodeOperatorSmoothFloat.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeOperatorSmoothFloatBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeOperatorSmoothFloatBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_SMOOTH_FLOAT);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorSmoothFloatBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  readDataPinChildNodeID(nodeDefDataBlock, "Input", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorSmoothFloatBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport,  "Input",   0, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorSmoothFloatBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  uint32_t numInputCPs = 1;
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    0,               // numChildren
    numInputCPs,     // numInputCPConnections
    1,               // numAnimSetEntries
    nodeDefExport);

  // Smooth time
  result += MR::AttribDataSmoothFloatOperation::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorSmoothFloatBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_SMOOTH_FLOAT, "Expecting node type CP_OP_SMOOTH_FLOAT");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Initialise the NodeDef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    0,        // numChildren
    0,        // max num active child nodes
    1,        // numInputCPConnections
    1,        // numOutputCPPins
    1,        // numAnimSetEntries                     
    nodeDefExport);

  // Flag node as an operator.
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);

  //---------------------------
  // Initialise the attrib datas.
   
  //--------------------------
  // Type of operation: float or vector3
  bool isScalar;
  nodeDefDataBlock->readBool(isScalar, "IsScalar");

  //---------------------------
  // Smooth time
  float smoothTimeInc = 0.0f;
  nodeDefDataBlock->readFloat(smoothTimeInc, "SmoothTime_Increasing");
  float smoothTimeDec = 0.0f;
  nodeDefDataBlock->readFloat(smoothTimeDec, "SmoothTime_Decreasing");

  //---------------------------
  // Smooth velocity
  bool smoothVel = false;
  nodeDefDataBlock->readBool(smoothVel, "SmoothVelocity");

  //---------------------------
  // Initial float values
  float initValueX = 0.0f;
  nodeDefDataBlock->readFloat(initValueX, "InitValueX");
  float initValueY = 0.0f;
  nodeDefDataBlock->readFloat(initValueY, "InitValueY");
  float initValueZ = 0.0f;
  nodeDefDataBlock->readFloat(initValueZ, "InitValueZ");

  //---------------------------
  // Use initial value on init?
  bool useInitValOnInit = false;
  nodeDefDataBlock->readBool(useInitValOnInit, "UseInitValueOnInit");

  MR::AttribDataSmoothFloatOperation* smoothTimeAttribData = MR::AttribDataSmoothFloatOperation::init(
    memRes, 
    smoothTimeInc, smoothTimeDec,
    initValueX, initValueY, initValueZ,
    useInitValOnInit,
    smoothVel,
    MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                        // semantic
    0,                                                            // animSetIndex
    smoothTimeAttribData,                                         // attribData
    MR::AttribDataSmoothFloatOperation::getMemoryRequirements()); // attribMemReqs


  //---------------------------
  // Initialise the task function tables.
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    isScalar,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorSmoothFloatBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  bool                        isScalar,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::OutputCPTask* outputCPTaskFns = (MR::OutputCPTask*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  if (isScalar)
  {
    nodeDef->setOutputCPTaskFnId(outputCPTaskFns, 0, MR::ATTRIB_SEMANTIC_CP_FLOAT, FN_NAME(nodeOperatorSmoothFloatCriticallyDampFloat), logger);
  }
  else
  {
    nodeDef->setOutputCPTaskFnId(outputCPTaskFns, 0, MR::ATTRIB_SEMANTIC_CP_VECTOR3, FN_NAME(nodeOperatorSmoothFloatCriticallyDampVector), logger);
  }

  // Register the shared task function tables
  nodeDef->registerEmptyTaskQueuingFns(taskQueuingFnTables);
  nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);

  // Tidy up
  NMP::Memory::memFree(outputCPTaskFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceNoChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsNULL), logger);
  if (isScalar)
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeOperatorSmoothDampFloatInitInstance), logger);
  }
  else 
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeOperatorSmoothDampVectorInitInstance), logger);
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
