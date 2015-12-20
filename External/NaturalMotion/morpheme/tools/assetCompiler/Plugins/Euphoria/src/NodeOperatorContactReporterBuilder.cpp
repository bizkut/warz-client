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
#include "NodeOperatorContactReporterBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "euphoria/erAttribData.h"
#include "euphoria/Nodes/erNodeOperatorContactReporter.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

// We have no inputs
static const uint32_t maxInputConnections = 0;

//----------------------------------------------------------------------------------------------------------------------
bool NodeOperatorContactReporterBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeOperatorContactReporterBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_CONTACTREPORTER);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorContactReporterBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    NMP_UNUSED(childNodeIDs),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorContactReporterBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorContactReporterBuilder::getNodeDefMemoryRequirements(
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

  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  int32_t numCollisionGroups = 0;
  nodeDefDataBlock->readInt(numCollisionGroups, "NumCollisionGroupsAttrib");
  
  result += ER::AttribDataOperatorContactReporterDef::getMemoryRequirements(numCollisionGroups);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorContactReporterBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_CONTACTREPORTER);
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Initialise the NodeDef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,
    netDefCompilationInfo,
    0,                                                  // numChildren
    0,                                                  // max num active child nodes
    maxInputConnections,                                // numInputCPConnections
    ER::NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_COUNT,  // numOutputCPPins
    1,                                                  // numAnimSetEntries
    nodeDefExport);

  // Flag node as an operator.
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);

  //---------------------------
  // Initialise the attrib data.

  // Read in the node attributes
  float MinImpulseMagnitude = 0.0;
  int32_t NumCollisionGroups = -1;
  
  nodeDefDataBlock->readFloat(MinImpulseMagnitude, "MinImpulseMagnitudeAttrib");
  nodeDefDataBlock->readInt(NumCollisionGroups, "NumCollisionGroupsAttrib");
  
  ER::AttribDataOperatorContactReporterDef* hitDetectorAttribs = 
    ER::AttribDataOperatorContactReporterDef::init(
      memRes,
      MinImpulseMagnitude,
      NumCollisionGroups,
      MR::IS_DEF_ATTRIB_DATA);

  CHAR paramName[256];
  int32_t collisionGroup;
  for (int32_t i = 0; i < NumCollisionGroups; ++i)
  {
    sprintf_s(paramName, "CollisionGroups_%d", i);
    collisionGroup = -1;
    nodeDefDataBlock->readInt(collisionGroup, paramName);
    hitDetectorAttribs->m_collisionGroups[i] = collisionGroup;
  }

  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                           // semantic
    0,                                                               // animSetIndex
    hitDetectorAttribs,                                              // attribData
    ER::AttribDataOperatorContactReporterDef::getMemoryRequirements(NumCollisionGroups)); // attribMemReqs

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorContactReporterBuilder::initTaskQueuingFns(
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

  // We don't have and may not need one of these as other outputs being hooked will
  // give the same effect of inducing an update
  //nodeDef->setOutputCPTaskFnId(
  //  outputCPTaskFns,
  //  ER::NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_PERFORMANCE,
  //  MR::ATTRIB_SEMANTIC_CP_INT,
  //  FN_NAME(nodeOperatorContactReporterOutputCPUpdate),
  //  logger);

  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_LIMBINDEX,
    MR::ATTRIB_SEMANTIC_CP_INT,
    ER_FN_NAME(nodeOperatorContactReporterOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_PARTINDEX,
    MR::ATTRIB_SEMANTIC_CP_INT,
    ER_FN_NAME(nodeOperatorContactReporterOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_RIGPARTINDEX,
    MR::ATTRIB_SEMANTIC_CP_INT,
    ER_FN_NAME(nodeOperatorContactReporterOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_POINTLOCAL,
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    ER_FN_NAME(nodeOperatorContactReporterOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_NORMALLOCAL,
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    ER_FN_NAME(nodeOperatorContactReporterOutputCPUpdate),
    logger);
  nodeDef->setOutputCPTaskFnId(outputCPTaskFns,
    ER::NODE_OPERATOR_CONTACTREPORTER_OUT_CP_PINID_IMPULSEWORLD,
    MR::ATTRIB_SEMANTIC_CP_VECTOR3,
    ER_FN_NAME(nodeOperatorContactReporterOutputCPUpdate),
    logger);

  // Register the shared task function tables
  nodeDef->registerEmptyTaskQueuingFns(taskQueuingFnTables);
  nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);


  // Tidy up
  NMP::Memory::memFree(outputCPTaskFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setInitNodeInstanceFnId(ER_FN_NAME(nodeOperatorContactReporterInitInstance), logger);
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceNoChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsNULL), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
