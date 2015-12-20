// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "NetworkDefBuilder.h"
#include "NodeExpandLimitsBuilder.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "physics/Nodes/mrNodeExpandLimits.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeExpandLimitsBuilder::hasSemanticLookupTable()
{
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeExpandLimitsBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Set child node ID.
  MR::NodeID sourceNodeID;
  readNodeID(nodeDefDataBlock, "SourceNodeID", sourceNodeID);
  childNodeIDs.push_back(sourceNodeID);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeExpandLimitsBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeExpandLimitsBuilder::getNodeDefMemoryRequirements( 
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  // Allocate space for the NodeDef itself
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo,
    1,              // numChildren
    0,              // numInputCPConnections
    0,              // numAnimSetEntries
    nodeDefExport);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* AP::NodeExpandLimitsBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_EXPAND_LIMITS, "Expecting node type EXPAND_LIMITS");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  
  //---------------------------
  // Initialise the nodedef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
                                  memRes,
                                  netDefCompilationInfo,
                                  1,            // numChildren
                                  1,            // max num active child nodes
                                  0,            // numInputCPConnections
                                  0,            // numOutputCPPins
                                  0,            // numAnimSetEntries             
                                  nodeDefExport);

  // Flag node as a filter.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Set child node ID.
  MR::NodeID sourceNodeID;
  readNodeID(nodeDefDataBlock, "SourceNodeID", sourceNodeID);
  nodeDef->setChildNodeID(0, sourceNodeID);
 
  //---------------------------
  // Initialise the nodedef.
  initTaskQueuingFns(nodeDef, netDefCompilationInfo, processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void AP::NodeExpandLimitsBuilder::initTaskQueuingFns(
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
    nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0));
  }

  // Task queuing function for TRANSFORM_BUFFER semantic.
  nodeDef->setTaskQueuingFnId(
                  taskQueuingFns,
                  MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
                  FN_NAME(nodeExpandLimitsQueueTransforms),
                  logger);

  // Task queuing function for TRAJECTORY_DELTA_TRANSFORM_BUFFER semantic.
  nodeDef->setTaskQueuingFnId(
                  taskQueuingFns,
                  MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
                  FN_NAME(nodeExpandLimitsQueueTrajectoryDeltaAndTransforms),
                  logger);

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
