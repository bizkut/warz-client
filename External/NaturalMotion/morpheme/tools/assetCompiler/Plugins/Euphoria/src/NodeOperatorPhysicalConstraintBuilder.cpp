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
#include "NodeOperatorPhysicalConstraintBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "euphoria/erAttribData.h"
#include "euphoria/Nodes/erNodeOperatorPhysicalConstraint.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

// All possible CPInputPin names
// (This operator has rather a lot of possible (optional) connections
// by rolling a display like this we can automate hookups below)
//
static const char* s_connectionToCPInputs[] = {
  "Activate",
  "PhysicsRigPartIndex",
  "PhysicsRigPartPosition",
  "PhysicsRigPartOrientation",
  "PhysicsObjectID",
  "PhysicsObjectPosition",
  "PhysicsObjectOrientation",
  "PhysicsObjectMassFraction",
};
static const uint32_t maxInputConnections = sizeof(s_connectionToCPInputs) / sizeof(char*);

//----------------------------------------------------------------------------------------------------------------------
bool NodeOperatorPhysicalConstraintBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeOperatorPhysicalConstraintBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_PHYSICALCONSTRAINT);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorPhysicalConstraintBuilder::getNodeDefInputConnections(
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
void NodeOperatorPhysicalConstraintBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "Activate", 0, true, MR::ATTRIB_SEMANTIC_CP_BOOL);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "PhysicsRigPartIndex", 1, true, MR::ATTRIB_SEMANTIC_CP_INT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "PhysicsRigPartPosition", 2, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "PhysicsRigPartOrientation", 3, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "PhysicsObjectID", 4, true, MR::ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "PhysicsObjectPosition", 5, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "PhysicsObjectOrientation", 6, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "PhysicsObjectMassFraction", 7, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
} 

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorPhysicalConstraintBuilder::getNodeDefMemoryRequirements(
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
  result += ER::AttribDataOperatorPhysicalConstraintDef::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorPhysicalConstraintBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_PHYSICALCONSTRAINT);
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Initialise the NodeDef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,
    netDefCompilationInfo,
    0,                                                  // numChildren
    0,                                                  // max num active child nodes
    maxInputConnections,                                // numInputCPConnections
    1,                                                  // numOutputCPPins
    1,                                                  // numAnimSetEntries
    nodeDefExport);

  // Flag node as an operator.
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);

  //---------------------------
  // Initialise the attrib datas.

  // Read in the node attributes
  bool constrainPositionX  = false;
  bool constrainPositionY  = false;
  bool constrainPositionZ  = false;
  bool constrainOrientationTwist = false;
  bool constrainOrientationSwing1 = false;
  bool constrainOrientationSwing2 = false;
  float partInertiaMultiplier = 10.0f;

  nodeDefDataBlock->readBool(constrainPositionX, "ConstrainPositionX");
  nodeDefDataBlock->readBool(constrainPositionY, "ConstrainPositionY");
  nodeDefDataBlock->readBool(constrainPositionZ, "ConstrainPositionZ");
  nodeDefDataBlock->readBool(constrainOrientationTwist, "ConstrainOrientationTwist");
  nodeDefDataBlock->readBool(constrainOrientationSwing1, "ConstrainOrientationSwing1");
  nodeDefDataBlock->readBool(constrainOrientationSwing2, "ConstrainOrientationSwing2");
  nodeDefDataBlock->readFloat(partInertiaMultiplier, "PartInertiaMultiplier");

  ER::AttribDataOperatorPhysicalConstraintDef* definitionAttribData = 
    ER::AttribDataOperatorPhysicalConstraintDef::init(
    memRes,
    constrainPositionX, constrainPositionY, constrainPositionZ, 
    constrainOrientationTwist, constrainOrientationSwing1, constrainOrientationSwing2, 
    partInertiaMultiplier,
    MR::IS_DEF_ATTRIB_DATA);

  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                                 // semantic
    0,                                                                     // animSetIndex
    definitionAttribData,                                                  // attribData
    ER::AttribDataOperatorPhysicalConstraintDef::getMemoryRequirements()); // attribMemReqs

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorPhysicalConstraintBuilder::initTaskQueuingFns(
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
    0, // Only one output pin
    MR::ATTRIB_SEMANTIC_CP_INT,
    ER_FN_NAME(nodeOperatorPhysicalConstraintOutputCPUpdate),
    logger);

  // Register the shared task function tables
  nodeDef->registerEmptyTaskQueuingFns(taskQueuingFnTables);
  nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);

  //---------------------------
  // Handle Data Pins
  if (nodeDef->getNumReflexiveCPPins() > 0)
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeInitPinAttribDataInstance), logger);
  }

  // Tidy up
  NMP::Memory::memFree(outputCPTaskFns);

  //------------------------------------
  // Other manager registered functions.
  // Note that the delete function doesn't get called - see MORPH-12428
  nodeDef->setDeleteNodeInstanceId(ER_FN_NAME(nodeOperatorPhysicalConstraintOutputDeleteInstance), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsNULL), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
