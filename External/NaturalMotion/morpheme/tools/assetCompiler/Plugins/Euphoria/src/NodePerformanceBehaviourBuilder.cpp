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
#include "NodePerformanceBehaviourBuilder.h"
#include "assetProcessor/NodeBuilderUtils.h"
#include "physics/Nodes/mrNodePhysics.h"
#include "physics/Nodes/mrNodePhysicsGrouper.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrAttribData.h"
#include "euphoria/erAttribData.h"
#include "morpheme/mrCommonTaskQueuingFns.h"
#include "euphoria/Nodes/erNodeBehaviour.h"
#include "euphoria/Nodes/erNodePerformanceBehaviour.h"
#include <tchar.h>
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
void NodePerformanceBehaviourBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    NMP_UNUSED(childNodeIDs),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
void NodePerformanceBehaviourBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodePerformanceBehaviourBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  uint32_t connectedPerformancePinCount = 0;
  nodeDefDataBlock->readUInt(connectedPerformancePinCount, "ConnectedPerformancePinCount");

  // Allocate space for the NodeDef itself.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo,
    0,                              // numChildren
    connectedPerformancePinCount,   // numInputCPConnections
    0,                              // numAnimSetEntries
    nodeDefExport);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodePerformanceBehaviourBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_PERFORMANCE_BEHAVIOUR, "Expecting Performance behaviour node type");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  uint32_t connectedPerformancePinCount = 0;
  nodeDefDataBlock->readUInt(connectedPerformancePinCount, "ConnectedPerformancePinCount");

  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,
    netDefCompilationInfo,
    0,                            // numChildren
    0,
    connectedPerformancePinCount, // numInputCPConnections
    0,                            // numOutputCPPins
    0,                            // numAnimSetEntries
    nodeDefExport);

  nodeDef->setNodeFlags(
    MR::NodeDef::NODE_FLAG_IS_PHYSICAL | 
    MR::NodeDef::NODE_FLAG_IS_BEHAVIOURAL | 
    MR::NodeDef::NODE_FLAG_REQUIRES_SEPARATE_TRAJECTORY_AND_TRANSFORMS);

  //---------------------------
  // Input control connections.
  CHAR paramName[256];
  for (MR::PinIndex i = 0; i < connectedPerformancePinCount; ++i)
  {
    sprintf_s(paramName, "ConnectedPerformanceNodeID_%d", i + 1);
    readInputPinConnectionDetails(nodeDefDataBlock,  paramName,  nodeDef, i, false);
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
void NodePerformanceBehaviourBuilder::initTaskQueuingFns(
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

  nodeDef->setTaskQueuingFnId(
    taskQueuingFns,
    MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
    ER_FN_NAME(nodeBehaviourQueueUpdateTransformsPostPhysics),
    logger);
  nodeDef->setTaskQueuingFnId(
    taskQueuingFns,
    MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
    ER_FN_NAME(nodeBehaviourQueueUpdateTrajectory),
    logger);
  nodeDef->setTaskQueuingFnId(
    taskQueuingFns,
    MR::ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS,
    ER_FN_NAME(nodePerformanceBehaviourQueueUpdateTransformsPrePhysics),
    logger);
  nodeDef->setTaskQueuingFnId(
    taskQueuingFns,
    MR::ATTRIB_SEMANTIC_OUTPUT_MASK,
    FN_NAME(grouperQueuePassThroughFromParent),
    logger);

  nodeDef->setTaskQueuingFnId(
    taskQueuingFns,
    MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER,
    FN_NAME(queueInitSampledEventsTrack),
    logger);
  nodeDef->setTaskQueuingFnId(
    taskQueuingFns,
    MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK,
    FN_NAME(queueInitUnitLengthSyncEventTrack),
    logger);
  nodeDef->setTaskQueuingFnId(
    taskQueuingFns,
    MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET,
    FN_NAME(queueInitEmptyEventTrackDurationSet),
    logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodePhysicsDeleteInstance), logger);
  nodeDef->setUpdateConnectionsFnId(ER_FN_NAME(nodePerformanceBehaviourUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(ER_FN_NAME(nodeBehaviourFindGeneratingNodeForSemantic), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
