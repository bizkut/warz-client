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
#include "NodeOperatorRateOfChangeBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodeOperatorRateOfChange.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeOperatorRateOfChangeBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeOperatorRateOfChangeBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_RATE_OF_CHANGE);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorRateOfChangeBuilder::getNodeDefInputConnections(
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
void NodeOperatorRateOfChangeBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "Input", 0, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorRateOfChangeBuilder::getNodeDefMemoryRequirements(
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

  result += MR::AttribDataRateOfChangeState::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorRateOfChangeBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_RATE_OF_CHANGE, "Expecting node type CP_OP_RATE_OF_CHANGE");
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

  if (isScalar)
  {
    MR::AttribDataRateOfChangeState* rateOfChangeAttribData = MR::AttribDataRateOfChangeState::init(
      memRes, 
      0.0f,
      MR::IS_DEF_ATTRIB_DATA);
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                        // semantic
      0,                                                            // animSetIndex
      rateOfChangeAttribData,                                       // attribData
      MR::AttribDataRateOfChangeState::getMemoryRequirements());    // attribMemReqs
  }
  else
  {
    MR::AttribDataRateOfChangeState* rateOfChangeAttribData = MR::AttribDataRateOfChangeState::init(
      memRes, 
      0.0f,
      0.0f,
      0.0f,
      MR::IS_DEF_ATTRIB_DATA);
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                        // semantic
      0,                                                            // animSetIndex
      rateOfChangeAttribData,                                       // attribData
      MR::AttribDataRateOfChangeState::getMemoryRequirements());    // attribMemReqs
  }

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
void NodeOperatorRateOfChangeBuilder::initTaskQueuingFns(
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
    nodeDef->setOutputCPTaskFnId(outputCPTaskFns, 0, MR::ATTRIB_SEMANTIC_CP_FLOAT, FN_NAME(nodeOperatorRateOfChangeFloat), logger);
  }
  else 
  {
    nodeDef->setOutputCPTaskFnId(outputCPTaskFns, 0, MR::ATTRIB_SEMANTIC_CP_VECTOR3, FN_NAME(nodeOperatorRateOfChangeVector), logger);
  }

  // Register the shared task function tables
  nodeDef->registerEmptyTaskQueuingFns(taskQueuingFnTables);
  nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);

  // Tidy up
  NMP::Memory::memFree(outputCPTaskFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsNULL), logger);
  if (isScalar)
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeOperatorRateOfChangeFloatInitInstance), logger);
  }
  else 
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeOperatorRateOfChangeVectorInitInstance), logger);
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
