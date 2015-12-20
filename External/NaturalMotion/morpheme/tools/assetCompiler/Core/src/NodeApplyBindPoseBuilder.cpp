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
#include "NodeApplyBindPoseBuilder.h"
#include "morpheme/Nodes/mrNodeApplyBindPose.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
void NodeApplyBindPoseBuilder::getNodeDefInputConnections(
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
void NodeApplyBindPoseBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeApplyBindPoseBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{  
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    1,  // numChildren
    0,  // numInputCPConnections
    0,  // numAnimSetEntries
    nodeDefExport);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeApplyBindPoseBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_APPLY_BIND_POSE, "Expecting node type APPLY_BIND_POSE");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,
    netDefCompilationInfo,
    1,                     // numChildren
    1,                     // max num active child nodes
    0,                     // numInputCPConnections
    0,                     // numOutputCPPins
    0,                     // numAnimSetEntries
    nodeDefExport);
 
  // Flag node as a filter.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);

  // Set child node ID.
  MR::NodeID source0NodeID;
  readNodeID(nodeDefDataBlock, "NodeConnectedTo", source0NodeID); 
  nodeDef->setChildNodeID(0, source0NodeID);
  
  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeApplyBindPoseBuilder::initTaskQueuingFns(
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

  // The only attribute we mess with is transforms.
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeApplyBindPoseQueueApplyBindPoseTransforms), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeApplyBindPoseQueueApplyBindPoseTrajectoryDeltaAndTransforms), logger);

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
