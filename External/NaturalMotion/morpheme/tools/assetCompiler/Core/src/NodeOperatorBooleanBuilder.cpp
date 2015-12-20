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
#include "NodeOperatorBooleanBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodeOperatorBoolean.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

const MR::PinIndex g_maxNumInputCPPins = 6;

//----------------------------------------------------------------------------------------------------------------------
bool NodeOperatorBooleanBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeOperatorBooleanBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_BOOLEAN);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
MR::PinIndex NodeOperatorBooleanBuilder::findConnectedInputCPPins(
  const ME::NodeExport*    nodeDefExport,
  std::vector<MR::NodeID>* childNodeIDs)
{ 
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  std::vector<MR::NodeID> workingChildNodeIDs;
  if (!childNodeIDs)
    childNodeIDs = &workingChildNodeIDs;

  // Read the first pin that must be there.
  readDataPinChildNodeID(nodeDefDataBlock, "Input1", *childNodeIDs, false);
  
  // Read the subsequent optionally connected pins. This cannot be sparse.
  char nodeIDLabel[16];
  for (MR::PinIndex i = 2; i <= g_maxNumInputCPPins; ++i)
  {
    NMP_SPRINTF(nodeIDLabel, 16, "Input%d", i);
    if (!readDataPinChildNodeID(nodeDefDataBlock, nodeIDLabel, *childNodeIDs, true))
      break;
  }
  
  return (MR::PinIndex) childNodeIDs->size();
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorBooleanBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  findConnectedInputCPPins(nodeDefExport, &childNodeIDs);
  
  NMP_VERIFY(childNodeIDs.size() > 0);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorBooleanBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  // Input control parameters.
  MR::PinIndex inputCPCount = findConnectedInputCPPins(nodeDefExport);
  char nodeIDLabel[16];
  for (MR::PinIndex i = 1; i <= inputCPCount; ++i)
  {
    NMP_SPRINTF(nodeIDLabel, 16, "Input%d", i);
    declareDataPin(netDefCompilationInfo, nodeDefExport, nodeIDLabel, i-1, false, MR::ATTRIB_SEMANTIC_CP_BOOL);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorBooleanBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  MR::PinIndex inputCPCount = findConnectedInputCPPins(nodeDefExport);
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
                                  netDefCompilationInfo, 
                                  0,               // numChildren
                                  inputCPCount,    // numInputCPConnections
                                  1,               // numAnimSetEntries
                                  nodeDefExport);

  // Add the operator code
  result += MR::AttribDataBooleanOperation::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorBooleanBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_BOOLEAN, "Expecting node type CP_OP_BOOLEAN");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Initialise the NodeDef itself.
  MR::PinIndex inputCPCount = findConnectedInputCPPins(nodeDefExport);
  MR::NodeDef* nodeDef = initCoreNodeDef(
                              memRes,                  
                              netDefCompilationInfo,   
                              0,            // numChildren
                              0,            // max num active child nodes
                              inputCPCount, // numInputCPConnections
                              1,            // numOutputCPPins
                              1,            // numAnimSetEntries                     
                              nodeDefExport);
  
  // Flag node as an operator.
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);
    

  //---------------------------
  // Initialise the attrib datas.
  //--------------------------
    
  // Add the operator code
  int32_t operatorCode = 0xFFFFFFFF;
  nodeDefDataBlock->readInt(operatorCode, "OperationCode");
  bool invertResult = false;
  nodeDefDataBlock->readBool(invertResult, "InvertResult");
  MR::AttribDataBooleanOperation* opCodeAttribData = MR::AttribDataBooleanOperation::init(
                                                          memRes,
                                                          (MR::AttribDataBooleanOperation::OperationType)operatorCode,
                                                          invertResult,
                                                          MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
        nodeDef,
        MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                    // semantic
        0,                                                        // animSetIndex
        opCodeAttribData,                                         // attribData
        MR::AttribDataBooleanOperation::getMemoryRequirements()); // attribMemReqs

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
        nodeDef,
        netDefCompilationInfo,
        processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorBooleanBuilder::initTaskQueuingFns(
  MR::NodeDef*               nodeDef,
  NetworkDefCompilationInfo* netDefCompilationInfo,
  NMP::BasicLogger*          logger)
{
  //------------------------------------
  // Shared task function tables.
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::OutputCPTask* outputCPTaskFns = (MR::OutputCPTask*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns, 0, MR::ATTRIB_SEMANTIC_CP_BOOL, FN_NAME(nodeOperatorBooleanOutputCPUpdateBool), logger);
  nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeShareInitInstanceCreateBoolOutputAttribute), logger);
      
  // Register the shared task function tables.
  nodeDef->registerEmptyTaskQueuingFns(taskQueuingFnTables);
  nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);

  // Tidy up.
  NMP::Memory::memFree(outputCPTaskFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceNoChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsNULL), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
