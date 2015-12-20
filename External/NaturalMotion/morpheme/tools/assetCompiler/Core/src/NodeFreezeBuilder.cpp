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
#include "NodeFreezeBuilder.h"
#include "morpheme/Nodes/mrNodeFreeze.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/mrCommonTaskQueuingFns.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeFreezeBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeFreezeBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_FREEZE);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_TIME_POS);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeFreezeBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    NMP_UNUSED(childNodeIDs),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
void NodeFreezeBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeFreezeBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  const uint32_t animSetCount = animLibraryExport->getNumAnimationSets();
  // Allocate space for the NodeDef itself.
  // Note: we only allow space for AttribData in anim set 0, this is ok because we create no data for any other anim set.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    0,                       // numChildren
    0,                       // numInputCPConnections
    (uint16_t) animSetCount, // numAnimSetEntries
    nodeDefExport);

  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t i=0; i < nodeDefDep.m_animSets.size(); ++i)
  {
    // Trajectory delta transform
    result += MR::AttribDataTrajectoryDeltaTransform::getMemoryRequirements();

    // Time pos
    result += MR::AttribDataPlaybackPos::getMemoryRequirements();
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeFreezeBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_FREEZE, "Expecting node type FREEZE");
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  const uint32_t animSetCount = animLibraryExport->getNumAnimationSets();

  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    0,                       // numChildren
    0,                       // max num active child nodes
    0,                       // numInputCPConnections
    0,                       // numOutputCPPins
    (uint16_t) animSetCount, // numAnimSetEntries
    nodeDefExport);

  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t validAnimSetIdx = 0; validAnimSetIdx < nodeDefDep.m_animSets.size(); ++validAnimSetIdx)
  {
    MR::AnimSetIndex animSetIndex = nodeDefDep.m_animSets[validAnimSetIdx];

    //---------------------------
    // Initialise the attrib datas.
    // Trajectory delta transform
    MR::AttribDataTrajectoryDeltaTransform* trajectoryDeltaTransform =
      MR::AttribDataTrajectoryDeltaTransform::init(
      memRes,
      NMP::Quat(NMP::Quat::kIdentity),
      NMP::Vector3(NMP::Vector3::InitZero),
      MR::IS_DEF_ATTRIB_DATA);

    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,                   // semantic,    
      animSetIndex,                                                     // animSetIndex,
      trajectoryDeltaTransform,                                         // attribData,  
      MR::AttribDataTrajectoryDeltaTransform::getMemoryRequirements()); // attribMemReqs

    //---------------------------
    // Some nodes may request the semantic MR::ATTRIB_SEMANTIC_TIME_POS so the freeze node must be able to produce one.
    // A playback position of 0 is used because there isn't a suitable time pos for the network.
    MR::AttribDataPlaybackPos* zeroTimePos = MR::AttribDataPlaybackPos::init(
      memRes,
      0.0f,
      0.0f,
      0.0f,
      0.0f,
      false,
      false,
      MR::IS_DEF_ATTRIB_DATA);

    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_TIME_POS,
      animSetIndex,
      zeroTimePos,
      MR::AttribDataPlaybackPos::getMemoryRequirements());
  }

  bool passThroughTransformsOnce = false;
  nodeDefExport->getDataBlock()->readBool(passThroughTransformsOnce, "passThroughTransformsOnce");

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger(),
    passThroughTransformsOnce);

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeFreezeBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NMP::BasicLogger*           logger,
  bool                        passThroughTransformsOnce)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = (MR::QueueAttrTaskFn*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  if (passThroughTransformsOnce)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeFreezePassThroughLastTransformsOnce), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeFreezePassThroughLastTrajectoryDeltaAndTransformsOnce), logger);
  }
  else
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeFreezePassThroughLastTransforms), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeFreezePassThroughLastTrajectoryDeltaAndTransforms), logger);
  }
  
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(queueInitSampledEventsTrack), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, FN_NAME(queueInitUnitLengthSyncEventTrack), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, FN_NAME(queueInitEmptyEventTrackDurationSet), logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeFreezeInitInstance), logger);
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeFreezeUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeFreezeHasQueuingFunction), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
