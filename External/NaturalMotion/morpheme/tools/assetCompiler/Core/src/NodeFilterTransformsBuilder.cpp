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
#include "NodeFilterTransformsBuilder.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeFilterTransformsBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeFilterTransformsBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_FILTER_TRANSFORMS);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_BONE_IDS);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeFilterTransformsBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Set child node ID.
  MR::NodeID source0NodeID;
  readNodeID(nodeDefDataBlock, "NodeConnectedTo", source0NodeID);
  childNodeIDs.push_back(source0NodeID);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeFilterTransformsBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeFilterTransformsBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  int32_t numAnimSets;
  nodeDefDataBlock->readInt(numAnimSets, "NumAnimSets");
  NMP_VERIFY(numAnimSets > 0);
  
   //---------------------------
  // Allocate space for the NodeDef itself.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    1,              // numChildren
    0,              // numInputCPConnections
    numAnimSets,    // numAnimSetEntries
    nodeDefExport);

  //---------------------------
  // Filter transforms channel alphas (Anim set specific)
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < (MR::AnimSetIndex)numAnimSets; ++animSetIndex)
  {
    CHAR paramName[256];

    // Get the number of bone filter IDs for this anim set.
    int32_t numBoneFilterIDs = 0;
    sprintf_s(paramName, "FilterIdCount_%d", animSetIndex + 1); // We add one to the index as LUA arrays start at 1 and the manifest was written out using LUA array indices
    nodeDefDataBlock->readInt(numBoneFilterIDs, paramName);
    result += MR::AttribDataUIntArray::getMemoryRequirements(numBoneFilterIDs);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeFilterTransformsBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_FILTER_TRANSFORMS, "Expecting node type FILTER_TRANSFORMS");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  int16_t numAnimSets = readNumAnimSets(nodeDefDataBlock);
 
  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    1,                // numChildren
    1,                // max num active child nodes
    0,                // numInputCPConnections
    0,                // numOutputCPPins
    numAnimSets,      // numAnimSetEntries                     
    nodeDefExport);

  // Flag node as a filter.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Set child node ID.
  MR::NodeID sourceNodeID;
  readNodeID(nodeDefDataBlock, "NodeConnectedTo", sourceNodeID);
  nodeDef->setChildNodeID(0, sourceNodeID);

  //---------------------------
  // Initialise the attrib datas.
  
  //---------------------------
  // Filter transforms channel alphas (Anim set specific)
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < (MR::AnimSetIndex)numAnimSets; ++animSetIndex)
  {
    CHAR paramName[256];

    // Get the number of bone filter IDs for this anim set.
    int32_t numBoneFilterIDs = 0;
    sprintf_s(paramName, "FilterIdCount_%d", animSetIndex + 1); // We add one to the index as LUA arrays start at 1 and the manifest was written out using LUA array indices
    nodeDefDataBlock->readInt(numBoneFilterIDs, paramName);

    // Create and init the attribute structure.
    MR::AttribDataUIntArray* filteredBonesArray = MR::AttribDataUIntArray::init(memRes, numBoneFilterIDs, MR::IS_DEF_ATTRIB_DATA);
    uint32_t boneIndex;
    for (uint32_t k = 0; k < (uint32_t)numBoneFilterIDs; ++k)
    {
      sprintf_s(paramName, "Id_%d_%d", animSetIndex + 1, k + 1);
      nodeDefDataBlock->readUInt(boneIndex, paramName);
      filteredBonesArray->m_values[k] = boneIndex;
    }

    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_BONE_IDS,                                      // semantic,    
      animSetIndex,                                                      // animSetIndex,
      filteredBonesArray,                                                // attribData,  
      MR::AttribDataUIntArray::getMemoryRequirements(numBoneFilterIDs)); // attribMemReqs
  }

  //---------------------------
  // Initialise the nodedef.
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeFilterTransformsBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = (MR::QueueAttrTaskFn*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  const uint32_t numEntries = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    // Most queuing requests get passed on to our child.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0), logger);
  }

  // The only attributes we mess with are transforms and trajectory delta.
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeFilterTransformsQueueFilterTransforms), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeFilterTransformsQueueFilterTrajectoryDeltaTransform), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeFilterTransformsQueueFilterTrajectoryDeltaAndTransforms), logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsChildren), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(filterNodeFindGeneratingNodeForSemantic), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
