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
#include "NodeBlendNxMBuilder.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrNodeBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodeBlendN.h"
#include "morpheme/Nodes/mrNodeBlendNSyncEvents.h"
#include "morpheme/Nodes/mrNodeBlend2x2.h"
#include "morpheme/Nodes/mrNodeBlend2x2SyncEvents.h"
#include "morpheme/Nodes/mrNodeBlendNxM.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeBlendNxMBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeBlendNxMBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_BLEND_NXM);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_LOOP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_BLEND_FLAGS);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBlendNxMBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // X value count.
  int32_t nodeCountX = 0;
  nodeDefDataBlock->readInt(nodeCountX, "NodeCountX");
  //---------------------------
  // Y value count.
  int32_t nodeCountY = 0;
  nodeDefDataBlock->readInt(nodeCountY, "NodeCountY");

  int32_t sourceNodeCount = nodeCountX * nodeCountY;
  NMP_VERIFY(sourceNodeCount > 0);

  //---------------------------
  // Child node IDs.
  MR::NodeID sourceNodeID;
  CHAR paramName[256];
  for (uint32_t i = 0; i < (uint32_t) sourceNodeCount; ++i)
  {
    sprintf_s(paramName, "Source%dNodeID", i);
    readNodeID(nodeDefDataBlock, paramName, sourceNodeID);
    childNodeIDs.push_back(sourceNodeID);
  }

  // Set up connected weight X control param nodes data.
  readDataPinChildNodeID(nodeDefDataBlock, "WeightX", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "WeightY", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBlendNxMBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "WeightX", 0, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "WeightY", 1, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeBlendNxMBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // X value count.
  int32_t nodeCountX = 0;
  nodeDefDataBlock->readInt(nodeCountX, "NodeCountX");
  //---------------------------
  // Y value count.
  int32_t nodeCountY = 0;
  nodeDefDataBlock->readInt(nodeCountY, "NodeCountY");

  int32_t sourceNodeCount = nodeCountX * nodeCountY;

  //---------------------------
  // Allocate space for the NodeDef itself.
  // Note: we only allow space for AttribData in anim set 0, this is ok because we create no data for any other anim set.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    sourceNodeCount, // numChildren
    2,               // numInputCPConnections
    1,               // numAnimSetEntries
    nodeDefExport);

  //---------------------------
  bool wrapWeightsX = false;
  nodeDefDataBlock->readBool(wrapWeightsX, "WrapWeightsX");
  bool wrapWeightsY = false;
  nodeDefDataBlock->readBool(wrapWeightsY, "WrapWeightsY");

  // Reserve space for the BlendNxM attrib data
  result += MR::AttribDataBlendNxMDef::getMemoryRequirements(
    (uint16_t)nodeCountX,
    (uint16_t)nodeCountY,
    wrapWeightsX,
    wrapWeightsY);

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
MR::NodeDef* NodeBlendNxMBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_BLEND_NXM, "Expecting node type BLEND_NxM");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // X value count.
  int32_t nodeCountX = 0;
  nodeDefDataBlock->readInt(nodeCountX, "NodeCountX");
  NMP_VERIFY(nodeCountX > 0);

  //---------------------------
  // Y value count.
  int32_t nodeCountY = 0;
  nodeDefDataBlock->readInt(nodeCountY, "NodeCountY");
  NMP_VERIFY(nodeCountY > 0);

  uint32_t sourceNodeCount = (uint32_t)(nodeCountX * nodeCountY);

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
    4,                      // max num active child nodes
    2,                      // numInputCPConnections
    0,                      // numOutputCPPins
    1,                      // numAnimSetEntries                     
    nodeDefExport);

  // Flag as a blend node that supports blend optimisation
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_BLEND_OPT_NODE);

  //---------------------------
  // Set child node IDs.
  MR::NodeID sourceNodeID;
  CHAR paramName[256];
  for (uint32_t i = 0; i < sourceNodeCount; ++i)
  {
    sprintf_s(paramName, "Source%dNodeID", i);
    readNodeID(nodeDefDataBlock, paramName, sourceNodeID);
    nodeDef->setChildNodeID(i, sourceNodeID);
  }

  //---------------------------
  // Initialise the attrib datas.
  bool wrapWeightsX = false;
  nodeDefDataBlock->readBool(wrapWeightsX, "WrapWeightsX");
  bool wrapWeightsY = false;
  nodeDefDataBlock->readBool(wrapWeightsY, "WrapWeightsY");

  NMP::Memory::Format memReqsWeights = MR::AttribDataBlendNxMDef::getMemoryRequirements(
    (uint16_t)nodeCountX,
    (uint16_t)nodeCountY,
    wrapWeightsX,
    wrapWeightsY);

  MR::AttribDataBlendNxMDef* weightsDefData = MR::AttribDataBlendNxMDef::init(
    memRes,
    (uint16_t)nodeCountX,
    (uint16_t)nodeCountY,
    wrapWeightsX,
    wrapWeightsY,
    MR::IS_DEF_ATTRIB_DATA);

  // Weights X
  for (uint32_t i = 0; i < (uint32_t)nodeCountX; ++i)
  {
    float weight = 0.0f;
    sprintf_s(paramName, "BlendWeightX_%d", i);
    nodeDefDataBlock->readFloat(weight, paramName);
    weightsDefData->m_weightsX[i] = weight;
  }

  if (wrapWeightsX)
  {
    float weight = 0.0f;
    nodeDefDataBlock->readFloat(weight, "WrapWeightX");
    weightsDefData->m_weightsX[nodeCountX] = weight;
  }

  // Weights Y
  for (uint32_t i = 0; i < (uint32_t)nodeCountY; ++i)
  {
    float weight = 0.0f;
    sprintf_s(paramName, "BlendWeightY_%d", i);
    nodeDefDataBlock->readFloat(weight, paramName);
    weightsDefData->m_weightsY[i] = weight;
  }

  if (wrapWeightsY)
  {
    float weight = 0.0f;
    nodeDefDataBlock->readFloat(weight, "WrapWeightY");
    weightsDefData->m_weightsY[nodeCountY] = weight;
  }

  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS,   // semantic,    
    0,                                        // animSetIndex,
    weightsDefData,                           // attribData,  
    memReqsWeights);                          // attribMemReqs

  //---------------------------
  // Time stretch attributes
  initBlendNodeTimeStretchModeAttribData(memRes, nodeDefExport, nodeDef);

  //---------------------------
  // Blend mode attributes.
  bool slerpDeltaTrajectoryTranslation = false;
  nodeDefDataBlock->readBool(slerpDeltaTrajectoryTranslation, "SphericallyInterpolateTrajectoryPosition");

  //---------------------------
  // Blending flags.
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
void NodeBlendNxMBuilder::initTaskQueuingFns(
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

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_OUTPUT_MASK, FN_NAME(grouperQueuePassThroughFromParent), logger);

  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeBlendNxMUpdateConnections), logger);
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);

  //---------------------------
  // MATCH EVENTS
  if (timeStretchMode == kNodeTimeStretchMatchEvents)
  {
    //---------------------------
    // Time
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, FN_NAME(nodeBlend2x2SyncEventsQueueSyncEventTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TIME_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_FRACTION_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);

    //---------------------------
    // Events
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, FN_NAME(nodeBlend2x2SyncEventsQueueBlend2x2DurationEventTrackSets), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeBlend2x2SyncEventsQueueSampledEventsBuffers), logger);

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
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeBlend2x2QueueSampledEventsBuffers), logger);

    // We don't deal with sync event tracks so let any nodes above us pass down event start pos related data to our children.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, FN_NAME(queuePassThroughChild0), logger);
  }

  //---------------------------
  // Transforms
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeBlend2x2QueueBlendTransformBuffsInterpAttInterpPos), logger);

  //---------------------------
  // Trajectory
  if (slerpDeltaTrajectoryTranslation)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeBlend2x2QueueTrajectoryDeltaTransformInterpAttSlerpPos), logger);
  }
  else
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeBlend2x2QueueTrajectoryDeltaTransformInterpAttInterpPos), logger);
  }

  //---------------------------
  // Trajectory delta and transforms
  if (slerpDeltaTrajectoryTranslation)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeBlend2x2QueueTrajectoryDeltaAndTransformsInterpPosInterpAttSlerpTraj), logger);
  }
  else
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeBlend2x2QueueTrajectoryDeltaAndTransformsInterpPosInterpAttInterpTraj), logger);
  }

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
