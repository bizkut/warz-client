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
#include "NodeOperatorPhysicsInfoBuilder.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrAttribData.h"
#include "physics/Nodes/mrNodeOperatorPhysicsInfo.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeOperatorPhysicsInfoBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeOperatorPhysicsInfoBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_CP_OP_PHYSICS_INFO);
  NMP_ASSERT(semanticLookupTable);

  // Add look up index for the per-anim-set node def data
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorPhysicsInfoBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    NMP_UNUSED(childNodeIDs),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorPhysicsInfoBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeOperatorPhysicsInfoBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             NMP_UNUSED(processor))
{
  // Set the number of animation set entries.
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  uint32_t numAnimSetEntries = animLibraryExport->getNumAnimationSets();

  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    0,                  // numChildren
    0,                  // numInputCPConnections
    numAnimSetEntries,  // numAnimSetEntries
    nodeDefExport);

  // Animation set specific attrib data - the per-anim-set node def data
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSetEntries; ++animSetIndex)
  {
    result += MR::AttribDataPhysicsInfoDef::getMemoryRequirements();
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeOperatorPhysicsInfoBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_CP_OP_PHYSICS_INFO, "Expecting node type CP_OP_PHYSICS_INFO");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Set the number of animation set entries.
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  uint16_t numberOfAnimSetEntries = (uint16_t)animLibraryExport->getNumAnimationSets();

  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    0,                                                 // numChildren
    0,                                                 // max num active child nodes
    0,                                                 // numInputCPConnections
    MR::NODE_OPERATOR_PHYSICS_INFO_OUT_CP_PINID_COUNT, // numOutputCPPins
    numberOfAnimSetEntries,                            // numAnimSetEntries                     
    nodeDefExport);

  // Flag node as an operator.
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_OPERATOR_NODE);

  //---------------------------
  // Initialise the attrib datas.
  bool inWorldSpace = true;
  nodeDefDataBlock->readBool(inWorldSpace, "OutputInWorldSpace");

  // Per-anim-set attributes
  CHAR paramName[256];
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numberOfAnimSetEntries; ++animSetIndex)
  {
    // Part index
    int partIndex = 0;
    sprintf_s(paramName, "PartIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readInt(partIndex, paramName);

    MR::AttribDataPhysicsInfoDef* physicsInfoAttribData = MR::AttribDataPhysicsInfoDef::init(
      memRes,
      partIndex,
      inWorldSpace,
      MR::IS_DEF_ATTRIB_DATA);

    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET,         // semantic
      animSetIndex,                                           // animSetIndex
      physicsInfoAttribData,                                  // attribData
      MR::AttribDataPhysicsInfoDef::getMemoryRequirements()); // attribMemReqs
  }

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeOperatorPhysicsInfoBuilder::initTaskQueuingFns(
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
              MR::NODE_OPERATOR_PHYSICS_INFO_OUT_CP_PINID_POSITION,
              MR::ATTRIB_SEMANTIC_CP_VECTOR3,
              FN_NAME(nodeOperatorPhysicsInfoOutputCPUpdate),
              logger);
  nodeDef->setOutputCPTaskFnId(
              outputCPTaskFns,
              MR:: NODE_OPERATOR_PHYSICS_INFO_OUT_CP_PINID_VELOCITY,
              MR::ATTRIB_SEMANTIC_CP_VECTOR3,
              FN_NAME(nodeOperatorPhysicsInfoOutputCPUpdate),
              logger);

  // Register the shared task function tables
  nodeDef->registerEmptyTaskQueuingFns(taskQueuingFnTables);
  nodeDef->registerOutputCPTasks(outputCPTaskFnTables, outputCPTaskFns);

  // Tidy up
  NMP::Memory::memFree(outputCPTaskFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsNULL), logger);
  nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeOperatorPhysicsInfoInitInstance), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
