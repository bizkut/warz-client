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
#include "NodeOperatorOneInputArithmeticVector3Builder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodeOperatorOneInputArithmetic.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeOperatorOneInputArithmeticVector3Builder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeOperatorOneInputArithmeticVector3Builder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC_VECTOR3);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorOneInputArithmeticVector3Builder::getNodeDefInputConnections(
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
void NodeOperatorOneInputArithmeticVector3Builder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "Input", 0, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorOneInputArithmeticVector3Builder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  uint32_t inputCPCount = 1;
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    0,               // numChildren
    inputCPCount,    // numInputCPConnections
    1,               // numAnimSetEntries
    nodeDefExport);

  // Add the operator code
  result += MR::AttribDataArithmeticOperation::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorOneInputArithmeticVector3Builder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC_VECTOR3, "Expecting node type NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC_VECTOR3");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Initialise the NodeDef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    0,      // numChildren
    0,      // max num active child nodes
    1,      // numInputCPConnections
    1,      // numOutputCPPins
    1,      // numAnimSetEntries                     
    nodeDefExport);

  // Flag node as an operator.
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);


  //---------------------------
  // Initialise the attrib datas.

  //---------------------------
  // Add the operator code.
  int32_t operatorCode = 0xFFFFFFFF;
  nodeDefDataBlock->readInt(operatorCode, "OperationCode");
  float constant = 0.0f;
  nodeDefDataBlock->readFloat(constant, "ConstantValue");

  NMP::Vector3 constantVec(0,0,0);
  nodeDefDataBlock->readFloat(constantVec.x, "ConstantValueX");
  nodeDefDataBlock->readFloat(constantVec.y, "ConstantValueY");
  nodeDefDataBlock->readFloat(constantVec.z, "ConstantValueZ");
  MR::AttribDataArithmeticOperation* opCodeAttribData = MR::AttribDataArithmeticOperation::init(
    memRes,
    (MR::AttribDataArithmeticOperation::OperationType)operatorCode,
    constant,
    constantVec.x,
    constantVec.y,
    constantVec.z,
    MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                   // semantic
    0,                                                       // animSetIndex
    opCodeAttribData,                                        // attribData 
    MR::AttribDataArithmeticOperation::getMemoryRequirements()); // attribMemReqs

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorOneInputArithmeticVector3Builder::initTaskQueuingFns(
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

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns, 0, MR::ATTRIB_SEMANTIC_CP_VECTOR3, FN_NAME(nodeOperatorOneInputArithmeticOutputCPUpdateVector), logger);
  nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeShareInitInstanceCreateVector3OutputAttributeInitPinAttrib), logger);

  // Register the shared task function tables
  nodeDef->registerEmptyTaskQueuingFns(taskQueuingFnTables);
  nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);

  // Tidy up
  NMP::Memory::memFree(outputCPTaskFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceNoChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsNULL), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
