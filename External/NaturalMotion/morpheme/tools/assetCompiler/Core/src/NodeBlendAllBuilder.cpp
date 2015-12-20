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
#include "NetworkDefBuilder.h"
#include "NodeBlendAllBuilder.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrNodeBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodeBlendN.h"
#include "morpheme/Nodes/mrNodeBlendNSyncEvents.h"
#include "morpheme/Nodes/mrNodeBlendAll.h"
#include "morpheme/Nodes/mrNodeBlendAllSyncEvents.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeBlendAllBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeBlendAllBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_BLEND_ALL);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_LOOP);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBlendAllBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount > 0);

  // Source node IDs.
  MR::NodeID sourceNodeID;
  CHAR sourceName[256];
  for (uint32_t i = 0; i < (uint32_t) sourceNodeCount; ++i)
  {
    sprintf_s(sourceName, "Source%dNodeID", i);
    readNodeID(nodeDefDataBlock, sourceName, sourceNodeID);
    childNodeIDs.push_back(sourceNodeID);
  }

  // Weight node IDs.
  CHAR weightName[256];
  for (uint32_t i = 0; i < (uint32_t) sourceNodeCount; ++i)
  {
    sprintf_s(weightName, "Weight%d", i);
    readDataPinChildNodeID(nodeDefDataBlock, weightName, childNodeIDs, true);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBlendAllBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount > 0);

  CHAR weightName[256];
  for (uint32_t i = 0; i < (uint32_t) sourceNodeCount; ++i)
  {
    sprintf_s(weightName, "Weight%d", i);
    declareDataPin(netDefCompilationInfo, nodeDefExport, weightName, (MR::PinIndex)i, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeBlendAllBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount > 0);

  // A weight per source connection 
  uint32_t inputCPCount = sourceNodeCount;

  //---------------------------
  // Allocate space for the NodeDef itself.
  // Note: we only allow space for AttribData in anim set 0, this is ok because we create no data for any other anim set.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    sourceNodeCount, // numChildren
    inputCPCount,    // numInputCPConnections
    1,               // numAnimSetEntries
    nodeDefExport);

  // Time stretch attributes
  NodeTimeStretchModes timeStretchMode;
  uint32_t numAttribDatasTimeStretch;
  result += getBlendNodeTimeStretchModeMemoryRequirements(
    nodeDefExport,
    timeStretchMode,
    numAttribDatasTimeStretch);

  // Number of connections
  result += MR::AttribDataUInt::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeBlendAllBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_BLEND_ALL, "Expecting node type BLEND_ALL");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount > 0);

  // A weight per animation
  uint32_t numControlParams = sourceNodeCount;

  //---------------------------
  // Time stretch attributes
  NodeTimeStretchModes timeStretchMode;
  uint32_t numAttribDatasTimeStretch;
  getBlendNodeTimeStretchModeMemoryRequirements(
    nodeDefExport,
    timeStretchMode,
    numAttribDatasTimeStretch);
 
  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    sourceNodeCount,            // numChildren
    (uint16_t)sourceNodeCount,  // max num active child nodes
    numControlParams,           // numInputCPConnections
    0,                          // numOutputCPPins
    1,                          // numAnimSetEntries                     
    nodeDefExport);

  //---------------------------
  // Set source node IDs.
  MR::NodeID sourceNodeID;
  CHAR sourceName[256];
  for (uint32_t i = 0; i < (uint32_t) sourceNodeCount; ++i)
  {
    sprintf_s(sourceName, "Source%dNodeID", i);
    readNodeID(nodeDefDataBlock, sourceName, sourceNodeID);
    nodeDef->setChildNodeID(i, sourceNodeID);
  }

  //---------------------------
  // Initialise the attrib datas.

  //---------------------------
  // Time stretch attributes
  initBlendNodeTimeStretchModeAttribData(memRes, nodeDefExport, nodeDef);
 
  //---------------------------
  // SLERP flag.
  bool slerpDeltaTrajectoryTranslation = false;
  nodeDefDataBlock->readBool(slerpDeltaTrajectoryTranslation, "SphericallyInterpolateTrajectoryPosition");

  //---------------------------
  // Initialise the nodedef.
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    timeStretchMode,
    slerpDeltaTrajectoryTranslation,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBlendAllBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NodeTimeStretchModes        timeStretchMode,
  bool                        slerpDeltaTrajectoryTranslation,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = (MR::QueueAttrTaskFn*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_OUTPUT_MASK, FN_NAME(grouperQueuePassThroughFromParent), logger);

  //---------------------------
  if (nodeDef->getNumReflexiveCPPins() > 0)
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeInitPinAttribDataInstance), logger);
  }

  //-----------
  // MATCH EVENTS
  if (timeStretchMode == kNodeTimeStretchMatchEvents)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TIME_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_FRACTION_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);

    //---------------------------
    // Sampled Events Buffer
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeBlendAllSyncEventsQueueSampledEventsBuffers), logger);

    // We deal with sync event tracks so don't let any nodes above us pass down event start pos related data to our children.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, NULL, NULL, logger);

    //---------------------------
    // Sync Event Tracks
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, FN_NAME(nodeBlendAllSyncEventsQueueSyncEventTrack), logger);

    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, FN_NAME(nodeBlendAllSyncEventsQueueBlend2DurationEventTrackSets), logger);

    nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeBlendNSyncEventsFindGeneratingNodeForSemantic), logger);
  }
  //-----------
  // NONE MATCH EVENTS
  else if (timeStretchMode == kNodeTimeStretchNone)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS, FN_NAME(queuePassThroughChild0), logger);

    // We don't deal with sync event tracks so let any nodes above us pass down event start pos related data to our children.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, FN_NAME(queuePassThroughChild0), logger);

    //---------------------------
    // Include handling of sync event related attributes so that if there is a transitMatchEvents somewhere in our parent
    // hierarchy this node will not crash out.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, FN_NAME(queuePassThroughChild0), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, FN_NAME(queuePassThroughChild0), logger);

    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeBlendAllQueueSampledEventsBuffers), logger);
    nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeBlendNFindGeneratingNodeForSemantic), logger);
  }

  //---------------------------
  // ATTRIB_SEMANTIC_TRANSFORMS
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeBlendAllQueueBlendTransformBuffsInterpAttInterpPos), logger);

  //---------------------------
  // ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM
  if (slerpDeltaTrajectoryTranslation)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeBlendAllQueueTrajectoryDeltaTransformInterpAttSlerpPos), logger);
  }
  else
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeBlendAllQueueTrajectoryDeltaTransformInterpAttInterpPos), logger);
  }

  //---------------------------
  // ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER
  if (slerpDeltaTrajectoryTranslation)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeBlendAllQueueTrajectoryDeltaAndTransformsSlerpTraj), logger);
  }
  else
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeBlendAllQueueTrajectoryDeltaAndTransformsInterpTraj), logger);
  }

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //---------------------------
  // Set update connections function.
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeBlendAllUpdateConnections), logger);

  //---------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
