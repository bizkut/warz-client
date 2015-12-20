//----------------------------------------------------------------------------------------------------------------------
#include "NetworkDefBuilder.h"
#include "NodeScaleCharacterBuilder.h"
#include "morpheme/Nodes/mrNodeScaleCharacter.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
void NodeScaleCharacterBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Set the child node ID.
  uint32_t inputNodeID = 0xFFFFFFFF;
  nodeDefDataBlock->readUInt(inputNodeID, "SourceNodeID");
  NMP_VERIFY(inputNodeID != 0xFFFFFFFF);
  childNodeIDs.push_back((MR::NodeID)inputNodeID);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeScaleCharacterBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeScaleCharacterBuilder::getNodeDefMemoryRequirements(
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
MR::NodeDef* NodeScaleCharacterBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_SCALE_CHARACTER, "Expecting node type SCALE_CHARACTER");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Initialise the NodeDef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,
    netDefCompilationInfo,
    1,              // numChildren
    1,              // max num active child nodes
    0,              // numInputCPConnections
    0,              // numOutputCPPins
    0,              // numAnimSetEntries
    nodeDefExport);

  // Flag node as a filter.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Set the child node ID.
  uint32_t inputNodeID = 0xFFFFFFFF;
  nodeDefDataBlock->readUInt(inputNodeID, "SourceNodeID");
  NMP_VERIFY(inputNodeID != 0xFFFFFFFF);
  nodeDef->setChildNodeID(0, (MR::NodeID) inputNodeID);

  //---------------------------
  // register interest in the desired scale message
  uint32_t messageID = MR::INVALID_MESSAGE_ID;
  nodeDefDataBlock->readUInt(messageID, "ScaleMessageID");
  if (messageID != MR::INVALID_MESSAGE_ID)
  {
    netDefCompilationInfo->registerMessageInterest(messageID, nodeDef->getNodeID());
  }

  //---------------------------
  // Input control parameters.

  // ...None

  //---------------------------
  // Initialise the attrib datas.

  // ...None

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(nodeDef, netDefCompilationInfo, processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeScaleCharacterBuilder::initTaskQueuingFns(
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

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeScaleCharacterQueueUpdateTransforms), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeScaleCharacterQueueTrajectoryDeltaTransform), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeScaleCharacterQueueTrajectoryDeltaAndTransforms), logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeScaleCharacterUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(filterNodeFindGeneratingNodeForSemantic), logger);
  nodeDef->setMessageHandlerFnId(FN_NAME(nodeScaleCharacterMessageHandler), logger);
}

} // namespace AP
