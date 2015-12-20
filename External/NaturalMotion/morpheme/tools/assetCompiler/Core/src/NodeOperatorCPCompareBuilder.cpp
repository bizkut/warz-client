// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
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
#include "NodeOperatorCPCompareBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodeOperatorCPCompare.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeOperatorCPCompareBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeOperatorCPCompareBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_COND_CP_COMPARE);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorCPCompareBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Control parameters
  readVariableTypeDataPinChildNodeID(nodeDefDataBlock, "Input0", childNodeIDs, true);
  readVariableTypeDataPinChildNodeID(nodeDefDataBlock, "Input1", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorCPCompareBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  // Type of operation: float, int, uint.
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_COND_CP_COMPARE, "Expecting node type NODE_TYPE_CP_OP_COND_CP_COMPARE");

  MR::AttribDataSemantic possibleTypes[] = 
  {
    MR::ATTRIB_SEMANTIC_CP_FLOAT,
    MR::ATTRIB_SEMANTIC_CP_INT,
    MR::ATTRIB_SEMANTIC_CP_UINT
  };

  declareVariableTypeDataPin(netDefCompilationInfo, nodeDefExport, "Input0", 0, true, possibleTypes, sizeof(possibleTypes));
  declareVariableTypeDataPin(netDefCompilationInfo, nodeDefExport, "Input1", 1, true, possibleTypes, sizeof(possibleTypes));
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorCPCompareBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
                                  netDefCompilationInfo, 
                                  0,                      // numChildren
                                  2,                      // numInputCPConnections
                                  1,                      // numAnimSetEntries
                                  nodeDefExport);

  // Add the operator code
  result += MR::AttribDataValueCompareOperation::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorCPCompareBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_COND_CP_COMPARE, "Expecting node type NODE_TYPE_CP_OP_COND_CP_COMPARE_FLOAT");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Initialise the NodeDef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
                            memRes,                  
                            netDefCompilationInfo,   
                            0,                    // numChildren
                            0,                    // max num active child nodes
                            2,                    // numInputCPConnections
                            1,                    // numOutputCPPins
                            1,                    // numAnimSetEntries                     
                            nodeDefExport);
  
  // Flag node as an operator.
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);
    
  //---------------------------
  // Initialise the attrib datas.
  //--------------------------

  //---------------------------
  // Add the operator code
  int32_t operatorCode = 0xFFFFFFFF;
  nodeDefDataBlock->readInt(operatorCode, "OperationCode");
  MR::AttribDataValueCompareOperation* opCodeAttribData = MR::AttribDataValueCompareOperation::init(
              memRes,
              (MR::AttribDataValueCompareOperation::OperationType)operatorCode,
              MR::IS_DEF_ATTRIB_DATA);

  initAttribEntry(
              nodeDef,
              MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                   // semantic
              0,                                                       // animSetIndex
              opCodeAttribData,                                        // attribData
              MR::AttribDataValueCompareOperation::getMemoryRequirements()); // attribMemReqs

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
              nodeDef,
              netDefCompilationInfo,
              processor->getMessageLogger(),
              nodeDefDataBlock);

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorCPCompareBuilder::initTaskQueuingFns(
  MR::NodeDef*               nodeDef,
  NetworkDefCompilationInfo* netDefCompilationInfo,
  NMP::BasicLogger*          logger,
  const ME::DataBlockExport* nodeDefDataBlock)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::OutputCPTask* outputCPTaskFns = (MR::OutputCPTask*)MR::SharedTaskFnTables::createSharedTaskFnTable();
   
  int32_t opType; // float, int, uint.
  nodeDefDataBlock->readInt(opType, "OpType");
  switch (opType)
  {
    case 1: // float
      nodeDef->setOutputCPTaskFnId(outputCPTaskFns, 0, MR::ATTRIB_SEMANTIC_CP_BOOL, FN_NAME(nodeOperatorCPCompareFloatOutputCPUpdateBool), logger);
      break;
    case 2: // int
      nodeDef->setOutputCPTaskFnId(outputCPTaskFns, 0, MR::ATTRIB_SEMANTIC_CP_BOOL, FN_NAME(nodeOperatorCPCompareIntOutputCPUpdateBool), logger);
      break;
    case 3: // uint
      nodeDef->setOutputCPTaskFnId(outputCPTaskFns, 0, MR::ATTRIB_SEMANTIC_CP_BOOL, FN_NAME(nodeOperatorCPCompareUIntOutputCPUpdateBool), logger);
      break;
    default:
      NMP_VERIFY_FAIL("Unrecognised type on comparison operator");
      break;
  }
    
  nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeShareInitInstanceCreateBoolOutputAttributeInitPinAttrib), logger);
  
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
