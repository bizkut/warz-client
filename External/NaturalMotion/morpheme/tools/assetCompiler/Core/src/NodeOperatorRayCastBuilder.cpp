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
#include "NodeOperatorRayCastBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodeOperatorRayCast.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeOperatorRayCastBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeOperatorRayCastBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_RAY_CAST);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorRayCastBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    NMP_UNUSED(childNodeIDs),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorRayCastBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorRayCastBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  uint32_t inputCPCount = 0;
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    0,               // numChildren
    inputCPCount,    // numInputCPConnections
    1,               // numAnimSetEntries
    nodeDefExport);
  
  // Attribute data
  result += MR::AttribDataRayCastDef::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorRayCastBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_RAY_CAST, "Expecting node type CP_OP_RAY_CAST");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    0,                                             // numChildren
    0,                                             // max num active child nodes
    0,                                             // numInputCPConnections
    MR::NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_COUNT, // numOutputCPPins
    1,                                             // numAnimSetEntries                     
    nodeDefExport);

  // Flag node as an operator.
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);

  //---------------------------
  // Read in the ray cast data and hook up the attribDataTies linked list
  float rayStartX;
  nodeDefDataBlock->readFloat(rayStartX, "RayStartX");
  float rayStartY;
  nodeDefDataBlock->readFloat(rayStartY, "RayStartY");
  float rayStartZ;
  nodeDefDataBlock->readFloat(rayStartZ, "RayStartZ");
  float rayDeltaX;
  nodeDefDataBlock->readFloat(rayDeltaX, "RayDeltaX");
  float rayDeltaY;
  nodeDefDataBlock->readFloat(rayDeltaY, "RayDeltaY");
  float rayDeltaZ;
  nodeDefDataBlock->readFloat(rayDeltaZ, "RayDeltaZ");
  
  bool useLocalOrient;
  nodeDefDataBlock->readBool(useLocalOrient, "UseLocalOrientation");

  NMP::Vector3 rayStart(rayStartX, rayStartY, rayStartZ);
  NMP::Vector3 rayDelta(rayDeltaX, rayDeltaY, rayDeltaZ);
  // Up Axis
  uint32_t upAxisIndex = 1;
  nodeDefDataBlock->readUInt(upAxisIndex, "UpAxisIndex");

  MR::AttribDataRayCastDef* rayCastAttribData = MR::AttribDataRayCastDef::init(
    memRes,
    useLocalOrient,
    rayStart,
    rayDelta,
    upAxisIndex,
    MR::IS_DEF_ATTRIB_DATA);

  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,              // semantic
    0,                                                  // animSetIndex
    rayCastAttribData,                                  // attribData
    MR::AttribDataRayCastDef::getMemoryRequirements()); // attribMemReqs

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorRayCastBuilder::initTaskQueuingFns(
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
              MR::NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_DISTANCE,
              MR::ATTRIB_SEMANTIC_CP_FLOAT,
              FN_NAME(nodeOperatorRayCastOutputCPUpdate),
              logger);
  nodeDef->setOutputCPTaskFnId(
              outputCPTaskFns,
              MR:: NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_POSITION,
              MR::ATTRIB_SEMANTIC_CP_VECTOR3,
              FN_NAME(nodeOperatorRayCastOutputCPUpdate),
              logger);
  nodeDef->setOutputCPTaskFnId(
              outputCPTaskFns,
              MR:: NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_HIT_NORMAL,
              MR::ATTRIB_SEMANTIC_CP_VECTOR3,
              FN_NAME(nodeOperatorRayCastOutputCPUpdate),
              logger);
  nodeDef->setOutputCPTaskFnId(
              outputCPTaskFns,
              MR:: NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_HIT_PITCH_DOWN_ANGLE,
              MR::ATTRIB_SEMANTIC_CP_FLOAT,
              FN_NAME(nodeOperatorRayCastOutputCPUpdate),
              logger);
  nodeDef->setOutputCPTaskFnId(
              outputCPTaskFns,
              MR:: NODE_OPERATOR_RAY_CAST_OUT_CP_PINID_HIT_ROLL_RIGHT_ANGLE,
              MR::ATTRIB_SEMANTIC_CP_FLOAT,
              FN_NAME(nodeOperatorRayCastOutputCPUpdate),
              logger);

  // Register the shared task function tables
  nodeDef->registerEmptyTaskQueuingFns(taskQueuingFnTables);
  nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);

  // Tidy up
  NMP::Memory::memFree(outputCPTaskFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceNoChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsNULL), logger);
  nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeOperatorRayCastInitInstance), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
