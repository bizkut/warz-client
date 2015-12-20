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
#include "NodeMirrorTransformsBuilder.h"
#include "NetworkDefBuilder.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/Nodes/mrNodeMirrorTransforms.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrAttribData.h"
#include <tchar.h>
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeMirrorBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeMirrorBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_MIRROR_TRANSFORMS_ID);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_BONE_IDS);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_SYNC_EVENT_OFFSET);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeMirrorBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Set child node ID.
  MR::NodeID inputNodeID;
  readNodeID(nodeDefDataBlock, "InputNodeID", inputNodeID);
  childNodeIDs.push_back(inputNodeID);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeMirrorBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeMirrorBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Work out how many attrib datas we actually have.
  int32_t numAnimSets;
  nodeDefDataBlock->readInt(numAnimSets, "NumAnimSets");
  NMP_VERIFY(numAnimSets > 0);
  
  //---------------------------
  // Allocate space for the NodeDef itself
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    1,               // numChildren
    0,               // numInputCPConnections
    numAnimSets,     // numAnimSetEntries
    nodeDefExport);

  //---------------------------
  // Per anim set attribute data
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < (MR::AnimSetIndex)numAnimSets; ++animSetIndex)
  {
    CHAR paramName[256];

    // Get the number of bone filter IDs for this anim set.
    int32_t numUnfilteredBoneIDs = 0;
    sprintf_s(paramName, "NonMirroredIdCount_%d", animSetIndex + 1); // We add one to the index as LUA arrays start at 1 and the manifest was written out using LUA array indices
    nodeDefDataBlock->readInt(numUnfilteredBoneIDs, paramName);
    result += MR::AttribDataUIntArray::getMemoryRequirements(numUnfilteredBoneIDs);
  }

  //---------------------------
  // Sync event offset
  result += MR::AttribDataInt::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeMirrorBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY(nodeDefExport->getTypeID() == NODE_MIRROR_TRANSFORMS_ID);
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  int16_t numAnimSets = readNumAnimSets(nodeDefDataBlock);
  
  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    1,              // numChildren
    1,              // max num active child nodes
    0,              // numInputCPConnections
    0,              // numOutputCPPins
    numAnimSets,    // numAnimSetEntries                     
    nodeDefExport);

  // Flag node as a filter only if we have been asked to pass through sync event processing.
  // This is not strictly accurate but flagging it as a filter when it actually handles sync events creates
  // problems with the function TaskAddOptionalNetInputParamRecurseFilterNodes.
  // Once this area of code has been re-architected we can revisit here.
  bool eventPassThrough = false;
  nodeDefDataBlock->readBool(eventPassThrough, "EventPassThrough");

  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Set child node ID.
  MR::NodeID source0NodeID;
  readNodeID(nodeDefDataBlock, "InputNodeID", source0NodeID);
  nodeDef->setChildNodeID(0, source0NodeID);

  //---------------------------
  // Initialise the attrib datas.
  
  //---------------------------
  // Per anim set attribute data
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < (MR::AnimSetIndex)numAnimSets; ++animSetIndex)
  {
    CHAR paramName[256];

    // Get the number of unfiltered bone IDs for this anim set.
    int32_t numUnfilteredBoneIDs = 0;
    sprintf_s(paramName, "NonMirroredIdCount_%d", animSetIndex + 1); // We add one to the index as LUA arrays start at 1 and the manifest was written out using LUA array indices
    nodeDefDataBlock->readInt(numUnfilteredBoneIDs, paramName);

    // Create and init the attribute structure.
    MR::AttribDataUIntArray* unfilteredBonesArray = MR::AttribDataUIntArray::init(memRes, numUnfilteredBoneIDs, MR::IS_DEF_ATTRIB_DATA);
    uint32_t boneIndex;
    for (uint32_t k = 0; k < (uint32_t)numUnfilteredBoneIDs; ++k)
    {
      sprintf_s(paramName, "Id_%d_%d", animSetIndex + 1, k + 1);
      nodeDefDataBlock->readUInt(boneIndex, paramName);
      unfilteredBonesArray->m_values[k] = boneIndex;
    }
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_BONE_IDS,                                          // semantic,    
      animSetIndex,                                                          // animSetIndex,
      unfilteredBonesArray,                                                  // attribData,  
      MR::AttribDataUIntArray::getMemoryRequirements(numUnfilteredBoneIDs)); // attribMemReqs
  }

  //---------------------------
  // Sync event offset
  int32_t eventOffset = 0;
  nodeDefDataBlock->readInt(eventOffset, "EventOffset");
  MR::AttribDataInt* eventOffsetAttrib = MR::AttribDataInt::init(memRes, eventOffset, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_SYNC_EVENT_OFFSET,       // semantic,    
    0,                                           // animSetIndex,
    eventOffsetAttrib,                           // attribData,  
    MR::AttribDataInt::getMemoryRequirements()); // attribMemReqs

  // Initialise the node def.
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    eventPassThrough,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeMirrorBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  bool                        eventPassThrough,
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

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeMirrorQueueTransformBuff), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeMirrorQueueTrajectoryDeltaTransform), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeMirrorQueueTrajectoryDeltaAndTransforms), logger);

  // if we are mirroring events
  if (!eventPassThrough)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TIME_POS, FN_NAME(nodeMirrorQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS, FN_NAME(nodeMirrorQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, FN_NAME(nodeMirrorQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_FRACTION_POS, FN_NAME(nodeMirrorQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, FN_NAME(nodeMirrorQueueSyncEventsBuffer), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeMirrorQueueSampledEventsBuffer), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, FN_NAME(nodeMirrorQueueDurationEventsBuffer), logger);

    // We deal with sync event tracks so don't let any nodes above us pass down event start pos related data to our children.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, NULL, NULL, logger);
  }

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
