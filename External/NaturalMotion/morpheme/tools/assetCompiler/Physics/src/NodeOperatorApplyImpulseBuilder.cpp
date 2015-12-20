//----------------------------------------------------------------------------------------------------------------------
// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
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
#include "NodeOperatorApplyImpulseBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "physics/Nodes/mrNodeOperatorApplyImpulse.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

// All possible CPInputPin names
// (This operator has rather a lot of possible (optional) connections
// by rolling a display like this we can automate hookups below)
//
static const char* s_connectionToCPInputs[] = {
  "PartIndex",
  "LocalMagnitude",
  "LocalAngularMultiplier",
  "LocalResponseRatio",
  "FullBodyMagnitude",
  "FullBodyLinearMultiplier",
  "FullBodyAngularMultiplier",
  "FullBodyResponseRatio",
  "Direction",
  "Position"
};
static const uint32_t maxInputConnections = sizeof(s_connectionToCPInputs) / sizeof(char*);

//----------------------------------------------------------------------------------------------------------------------
bool NodeOperatorApplyImpulseBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeOperatorApplyImpulseBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_APPLYIMPULSE);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorApplyImpulseBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Input Control Param Connections.
  for (uint32_t i = 0; i < maxInputConnections; ++i)
  {
    readDataPinChildNodeID(nodeDefDataBlock, s_connectionToCPInputs[i], childNodeIDs, true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorApplyImpulseBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, s_connectionToCPInputs[0], 0, true, MR::ATTRIB_SEMANTIC_CP_INT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, s_connectionToCPInputs[1], 1, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, s_connectionToCPInputs[2], 2, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, s_connectionToCPInputs[3], 3, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, s_connectionToCPInputs[4], 4, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, s_connectionToCPInputs[5], 5, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, s_connectionToCPInputs[6], 6, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, s_connectionToCPInputs[7], 7, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, s_connectionToCPInputs[8], 8, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
  declareDataPin(netDefCompilationInfo, nodeDefExport, s_connectionToCPInputs[9], 9, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorApplyImpulseBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo,
    0,                   // numChildren
    maxInputConnections, // numInputCPConnections
    1,                   // numAnimSetEntries
    nodeDefExport);

  // Attribute def data
  result += MR::AttribDataOperatorApplyImpulseDef::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorApplyImpulseBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_APPLYIMPULSE);
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Initialise the NodeDef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,
    netDefCompilationInfo,
    0,                                                  // numChildren
    0,                                                  // max num active child nodes
    maxInputConnections,                                // numInputCPConnections
    MR::NODE_OPERATOR_APPLYIMPULSE_OUT_CP_PINID_COUNT,  // numOutputCPPins
    1,                                                  // numAnimSetEntries
    nodeDefExport);

  // Flag node as an operator.
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);

  //---------------------------
  // Initialise the attrib datas.

  // Read in the node attributes
  bool PositionInWorldSpace  = false;
  bool DirectionInWorldSpace = false;
  int ImpulseType    = 0;

  nodeDefDataBlock->readBool(PositionInWorldSpace, "PositionInWorldSpace");
  nodeDefDataBlock->readBool(DirectionInWorldSpace, "DirectionInWorldSpace");
  nodeDefDataBlock->readInt(ImpulseType, "ImpulseType");

  MR::AttribDataOperatorApplyImpulseDef* definitionAttribData = MR::AttribDataOperatorApplyImpulseDef::init(
    memRes,
    ImpulseType,
    PositionInWorldSpace,
    DirectionInWorldSpace,
    MR::IS_DEF_ATTRIB_DATA);

  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                           // semantic
    0,                                                               // animSetIndex
    definitionAttribData,                                            // attribData
    MR::AttribDataOperatorApplyImpulseDef::getMemoryRequirements()); // attribMemReqs

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorApplyImpulseBuilder::initTaskQueuingFns(
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
    MR::NODE_OPERATOR_APPLYIMPULSE_OUT_CP_PINID_PERFORMANCE,
    MR::ATTRIB_SEMANTIC_CP_INT,
    FN_NAME(nodeOperatorApplyImpulseOutputCPUpdate),
    logger);

  // Register the shared task function tables
  nodeDef->registerEmptyTaskQueuingFns(taskQueuingFnTables);
  nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);

  // Tidy up
  NMP::Memory::memFree(outputCPTaskFns);

  //---------------------------
  if (nodeDef->getNumReflexiveCPPins() > 0)
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeInitPinAttribDataInstance), logger);
  }

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceNoChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsNULL), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
