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
#include "assetProcessor/NodeBuilder.h"
#include "assetProcessor/NodeBuilderUtils.h"
#include "NodeBehaviourGrouperBuilder.h"
#include "euphoria/Nodes/erNodeBehaviourGrouper.h"
#include "physics/Nodes/mrNodePhysicsGrouper.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrAttribData.h"
#include "euphoria/erAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
void NodeBehaviourGrouperBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  uint32_t connectedBehaviourPinCount = 0;
  nodeDefDataBlock->readUInt(connectedBehaviourPinCount, "ConnectedBehaviourPinCount");

  // Set child node IDs.
  CHAR paramName[256];
  MR::NodeID sourceNodeID;
  for (uint32_t i = 0; i < connectedBehaviourPinCount; ++i)
  {
    sprintf_s(paramName, "ConnectedBehaviourNodeID_%d", i + 1);
    readNodeID(nodeDefDataBlock, paramName, sourceNodeID);
    childNodeIDs.push_back(sourceNodeID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBehaviourGrouperBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeBehaviourGrouperBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  uint32_t connectedBehaviourPinCount = 0;
  nodeDefDataBlock->readUInt(connectedBehaviourPinCount, "ConnectedBehaviourPinCount");

  // Allocate space for the NodeDef itself.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo,
    connectedBehaviourPinCount,     // numChildren
    0,                              // numInputCPConnections
    0,                              // numAnimSetEntries
    nodeDefExport);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeBehaviourGrouperBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY(nodeDefExport->getTypeID() == NODE_TYPE_BEHAVIOUR_GROUPER);
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  uint32_t connectedBehaviourPinCount = 0;
  nodeDefDataBlock->readUInt(connectedBehaviourPinCount, "ConnectedBehaviourPinCount");

  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,
    netDefCompilationInfo,
    connectedBehaviourPinCount,   // numChildren
    (uint16_t) connectedBehaviourPinCount,
    0,                            // numInputCPConnections
    0,                            // numOutputCPPins
    0,                            // numAnimSetEntries
    nodeDefExport);

  // Flag node as a filter node.
  // NOTE: The grouper is not a true filter node as it has more than 1 child node.
  // At the moment the grouper passes through messages from the parent to the first child, this will create problems
  // in some situations, so it may be that at a later date we should re-architect this node to operate more like a
  // blend2 node that only processes certain attribute types and does not pass through. This has the disadvantage
  // that we may need to have more than 1 grouper node type but it would produce a more robust system.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Set child node IDs.
  CHAR paramName[256];
  MR::NodeID sourceNodeID;
  for (uint32_t i = 0; i < connectedBehaviourPinCount; ++i)
  {
    sprintf_s(paramName, "ConnectedBehaviourNodeID_%d", i + 1);
    readNodeID(nodeDefDataBlock, paramName, sourceNodeID);
    nodeDef->setChildNodeID(i, sourceNodeID);
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
void NodeBehaviourGrouperBuilder::initTaskQueuingFns(
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
    // All queuing requests get passed on to our first child.
    // The mask should pass straight through too.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0), logger);
  }

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsChildren), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(ER_FN_NAME(nodeBehaviourGrouperFindGeneratingNodeForSemantic), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
