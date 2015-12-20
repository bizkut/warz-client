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
#include "NodeOperatorRampFloatBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodeOperatorRampFloat.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeOperatorRampFloatBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeOperatorRampFloatBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_RAMP_FLOAT);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorRampFloatBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  // Control parameter
  readDataPinChildNodeID(nodeDefDataBlock, "RateMultiplier", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorRampFloatBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  //---------------------------
  // Input control parameter.
  declareDataPin(netDefCompilationInfo, nodeDefExport, "RateMultiplier",0, true, MR::ATTRIB_SEMANTIC_CP_FLOAT); // Optional control input
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorRampFloatBuilder::getNodeDefMemoryRequirements(
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

  // Attribute data
  result += MR::AttribDataFloatArray::getMemoryRequirements(4);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorRampFloatBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_RAMP_FLOAT, "Expecting node type CP_OP_RAMP_FLOAT");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Initialise the NodeDef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    0,     // numChildren
    0,     // max num active child nodes
    1,     // numInputCPConnections
    1,     // numOutputCPPins
    1,     // numAnimSetEntries                     
    nodeDefExport);

  // Flag node as an operator.
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);

  //---------------------------
  // Initialise the attrib datas.

  // initial, rate, min and max values
  MR::AttribDataFloatArray* valuesAttribData = MR::AttribDataFloatArray::init(memRes, 4, MR::IS_DEF_ATTRIB_DATA);
  nodeDefDataBlock->readFloat(valuesAttribData->m_values[0], "InitialValue");
  nodeDefDataBlock->readFloat(valuesAttribData->m_values[1], "RateOfChange");
  nodeDefDataBlock->readFloat(valuesAttribData->m_values[2], "MinimumValue");
  nodeDefDataBlock->readFloat(valuesAttribData->m_values[3], "MaximumValue");
  
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,               // semantic
    0,                                                   // animSetIndex
    valuesAttribData,                                    // attribData
    MR::AttribDataFloatArray::getMemoryRequirements(4)); // attribMemReqs

  //---------------------------
  // Initialise the task function tables.
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorRampFloatBuilder::initTaskQueuingFns(
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

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns, 0, MR::ATTRIB_SEMANTIC_CP_FLOAT, FN_NAME(nodeOperatorRampFloatOutputCPUpdateFloat), logger);

  // Register the shared task function tables
  nodeDef->registerEmptyTaskQueuingFns(taskQueuingFnTables);
  nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);

  // Tidy up
  NMP::Memory::memFree(outputCPTaskFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceNoChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsNULL), logger);
  nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeOperatorRampFloatInitInstance), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
