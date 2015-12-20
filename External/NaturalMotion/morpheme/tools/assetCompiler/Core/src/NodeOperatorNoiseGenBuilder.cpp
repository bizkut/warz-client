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
#include "NodeOperatorNoiseGenBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodeOperatorNoiseGen.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeOperatorNoiseGenBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeOperatorNoiseGenBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_NOISE_GEN);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorNoiseGenBuilder::getNodeDefInputConnections(
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
void NodeOperatorNoiseGenBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  //---------------------------
  // Input control parameter.
  declareDataPin(netDefCompilationInfo, nodeDefExport,  "Input", 0, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorNoiseGenBuilder::getNodeDefMemoryRequirements(
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
  result += MR::AttribDataNoiseGenDef::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorNoiseGenBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_NOISE_GEN, "Expecting node type CP_OP_NOISE_GEN");
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

  float noiseSawFrequency = 0.0f;
  nodeDefDataBlock->readFloat(noiseSawFrequency, "NoiseSawFrequency");

  int32_t flags = 0xFFFFFFFF;
  nodeDefDataBlock->readInt(flags, "NoiseFlags");

  MR::AttribDataNoiseGenDef* noiseGenAttribData = MR::AttribDataNoiseGenDef::init(memRes, flags, noiseSawFrequency, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,               // semantic
    0,                                                   // animSetIndex
    noiseGenAttribData,                                  // attribData
    MR::AttribDataNoiseGenDef::getMemoryRequirements()); // attribMemReqs

  //---------------------------
  // Initialise the task function tables.
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorNoiseGenBuilder::initTaskQueuingFns(
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

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns, 0, MR::ATTRIB_SEMANTIC_CP_FLOAT, FN_NAME(nodeOperatorNoiseGenOutputCPUpdateFloat), logger);

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
