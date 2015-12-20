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
#include "NodeBlendNBuilder.h"
#include "morpheme/Nodes/mrNodeBlend2.h"
#include "morpheme/Nodes/mrNodeBlendN.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrNodeBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodeBlendNSyncEvents.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeBlendNBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeBlendNBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_BLEND_N);
  NMP_VERIFY(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_LOOP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_BLEND_FLAGS);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBlendNBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount >= 2); // It is not valid to have a node with only one input (this is checked in validate).

  // Set child node IDs.
  MR::NodeID sourceNodeID;
  CHAR paramName[256];
  for (uint32_t i = 0; i < (uint32_t) sourceNodeCount; ++i)
  {
    sprintf_s(paramName, "Source%dNodeID", i);    
    readNodeID(nodeDefDataBlock, paramName, sourceNodeID);
    childNodeIDs.push_back(sourceNodeID);
  }

  // Control parameters
  readDataPinChildNodeID(nodeDefDataBlock, "Weight", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBlendNBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "Weight", 0, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeBlendNBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount >= 2); // It is not valid to have a node with only one input (this is checked in validate).

  //---------------------------
  // Allocate space for the NodeDef itself.
  // Note: we only allow space for AttribData in anim set 0, this is ok because we create no data for any other anim set.
  uint32_t controlParamCount = 1;
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    sourceNodeCount,        // numChildren
    controlParamCount,      // numInputCPConnections
    1,                      // numAnimSetEntries
    nodeDefExport);

  //---------------------------
  // Reserve space for the source weights array.
  int32_t weightCount = sourceNodeCount;
  bool wrapWeights = false;
  nodeDefDataBlock->readBool(wrapWeights, "WrapWeights");
  if (wrapWeights)
  {
    ++weightCount;
  }
  result += MR::AttribDataFloatArray::getMemoryRequirements(weightCount);

  //---------------------------
  // Time stretch attributes
  NodeTimeStretchModes timeStretchMode;
  uint32_t numAttribDatasTimeStretch;
  result += getBlendNodeTimeStretchModeMemoryRequirements(
    nodeDefExport,
    timeStretchMode,
    numAttribDatasTimeStretch);

  // Blending flags.
  result += MR::AttribDataBlendFlags::getMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeBlendNBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_BLEND_N, "Expecting node type BLEND_N");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  int32_t sourceNodeCount = 0;
  nodeDefDataBlock->readInt(sourceNodeCount, "SourceNodeCount");
  NMP_VERIFY(sourceNodeCount >= 2); // It is not valid to have a node with only one input (this is checked in validate).

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
    sourceNodeCount,        // numChildren
    2,                      // max num active child nodes
    1,                      // numInputCPConnections
    0,                      // numOutputCPPins
    1,                      // numAnimSetEntries                     
    nodeDefExport);

  // Flag as a blend node that supports blend optimisation
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_BLEND_OPT_NODE);

  //---------------------------
  // Set child node IDs.
  MR::NodeID sourceNodeID;
  CHAR paramName[256];
  for (uint32_t i = 0; i < (uint32_t) sourceNodeCount; ++i)
  {
    sprintf_s(paramName, "Source%dNodeID", i);
    readNodeID(nodeDefDataBlock, paramName, sourceNodeID);
    nodeDef->setChildNodeID(i, sourceNodeID);
  }

  // If we wrap the weights an additional wrap weight must be added.
  int32_t weightCount = sourceNodeCount;
  bool wrapWeights = false;
  nodeDefDataBlock->readBool(wrapWeights, "WrapWeights");
  if (wrapWeights)
  {
    ++weightCount;
  }

  //---------------------------
  // Initialise the attrib datas.
  
  //---------------------------
  // Add the source weights array.
  MR::AttribDataFloatArray* childNodeWeights = MR::AttribDataFloatArray::init(memRes, weightCount, MR::IS_DEF_ATTRIB_DATA);
  float sourceNodeWeight;
  for (uint32_t i = 0; i < (uint32_t) sourceNodeCount; ++i)
  {
    sprintf_s(paramName, "SourceWeight_%d", i);
    sourceNodeWeight = 0.0f;
    nodeDefDataBlock->readFloat(sourceNodeWeight, paramName);
    childNodeWeights->m_values[i] = sourceNodeWeight;
  }

  if (wrapWeights)
  {
    float wrapWeight = 0.0f;
    nodeDefDataBlock->readFloat(wrapWeight, "WrapWeight");
    childNodeWeights->m_values[sourceNodeCount] = wrapWeight;
  }
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS,                             // semantic,    
    0,                                                                  // animSetIndex,
    childNodeWeights,                                                   // attribData,  
    MR::AttribDataFloatArray::getMemoryRequirements(sourceNodeCount));  // attribMemReqs

  //---------------------------
  initBlendNodeTimeStretchModeAttribData(memRes, nodeDefExport, nodeDef);
 
  //---------------------------
  // Blending flags.
  bool slerpDeltaTrajectoryTranslation = false;
  nodeDefDataBlock->readBool(slerpDeltaTrajectoryTranslation, "SphericallyInterpolateTrajectoryPosition");

  //---------------------------
  // Blending flags
  initBlendNodeBlendFlags(
    memRes,
    nodeDefExport,
    nodeDef);

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    timeStretchMode,
    slerpDeltaTrajectoryTranslation,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBlendNBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NodeTimeStretchModes        timeStretchMode,
  bool                        slerpDeltaTrajectoryTranslation,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_VERIFY(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = (MR::QueueAttrTaskFn*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  //---------------------------
  if (nodeDef->getNumReflexiveCPPins() > 0)
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeInitPinAttribDataInstance), logger);
  }

  //------------------------------------
  // Update connections
  if (timeStretchMode == kNodeTimeStretchMatchEvents)
  {
    nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeBlendNSyncEventsFindGeneratingNodeForSemantic), logger);
  }
  else
  {
    nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeBlendNFindGeneratingNodeForSemantic), logger);
  }

  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeBlendNUpdateConnections), logger);

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_OUTPUT_MASK, FN_NAME(grouperQueuePassThroughFromParent), logger);

  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);

  //---------------------------
  // MATCH EVENTS
  if (timeStretchMode == kNodeTimeStretchMatchEvents)
  {
    //---------------------------
    // Time
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, FN_NAME(nodeBlend2SyncEventsQueueSyncEventTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TIME_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_FRACTION_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);

    //---------------------------
    // Events
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, FN_NAME(nodeBlend2SyncEventsQueueBlend2DurationEventTrackSets), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeBlend2SyncEventsQueueSampledEventsBuffers), logger);

    // We deal with sync event tracks so don't let any nodes above us pass down event start pos related data to our children.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, NULL, NULL, logger);
  }
  //---------------------------
  // NOT MATCH EVENTS
  else if (timeStretchMode == kNodeTimeStretchNone)
  {
    //---------------------------
    // Time
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS, FN_NAME(queuePassThroughChild0), logger);

    // Include handling of sync event related attributes so that if there is a transitMatchEvents somewhere in our parent
    // hierarchy this node will not crash out.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, FN_NAME(queuePassThroughChild0), logger);

    //---------------------------
    // Events
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, FN_NAME(queuePassThroughChild0), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeBlend2QueueSampledEventsBuffers), logger);

    // We don't deal with sync event tracks so let any nodes above us pass down event start pos related data to our children.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, FN_NAME(queuePassThroughChild0), logger);
  }

  //---------------------------
  // Transforms
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueBlend2TransformBuffsInterpAttInterpPos), logger);

  //---------------------------
  // Trajectory
  if (slerpDeltaTrajectoryTranslation)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeBlend2QueueTrajectoryDeltaTransformInterpAttSlerpPos), logger);
  }
  else
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeBlend2QueueTrajectoryDeltaTransformInterpAttInterpPos), logger);
  }

  //---------------------------
  // Trajectory delta and transforms
  if (slerpDeltaTrajectoryTranslation)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosSlerpTraj), logger);
  }
  else
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosInterpTraj), logger);
  }

  //---------------------------
  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
