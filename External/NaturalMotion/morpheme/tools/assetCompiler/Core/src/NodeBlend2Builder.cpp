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
#include "NodeBlend2Builder.h"
#include "morpheme/Nodes/mrNodeBlend2.h"
#include "morpheme/Nodes/mrNodeBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeBlend2Builder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeBlend2Builder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_BLEND_2);
  NMP_VERIFY(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_LOOP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_BLEND_FLAGS);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBlend2Builder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Source 0
  MR::NodeID source0NodeID;
  readNodeID(nodeDefDataBlock, "Source0NodeID", source0NodeID);
  childNodeIDs.push_back(source0NodeID);

  // Source 1
  MR::NodeID source1NodeID;
  readNodeID(nodeDefDataBlock, "Source1NodeID", source1NodeID);
  childNodeIDs.push_back(source1NodeID);

  // Weight
  readDataPinChildNodeID(nodeDefDataBlock, "Weight", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "EventBlendingWeight", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBlend2Builder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "Weight", 0, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "EventBlendingWeight", 1, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeBlend2Builder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  //---------------------------
  // Allocate space for the NodeDef itself.
  // Note: we only allow space for AttribData in anim set 0, this is ok because we create no data for any other anim set.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    2,                      // numChildren
    2,                      // numInputCPConnections
    1,                      // numAnimSetEntries
    nodeDefExport);

  // Reserve space for the blend weights array.
  result += MR::AttribDataFloatArray::getMemoryRequirements(2);

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
MR::NodeDef* NodeBlend2Builder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_BLEND_2, "Expecting node type BLEND_2");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  
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
    2,                      // numChildren
    2,                      // max num active child nodes
    2,                      // numInputCPConnections
    0,                      // numOutputCPPins
    1,                      // numAnimSetEntries
    nodeDefExport);

  // Flag as a blend node that supports blend optimisation
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_BLEND_OPT_NODE);

  //---------------------------
  // Set child node IDs.
  MR::NodeID source0NodeID;
  readNodeID(nodeDefDataBlock, "Source0NodeID", source0NodeID);
  nodeDef->setChildNodeID(0, source0NodeID);

  MR::NodeID source1NodeID;
  readNodeID(nodeDefDataBlock, "Source1NodeID", source1NodeID);
  nodeDef->setChildNodeID(1, source1NodeID);

  //---------------------------
  // Pass through filtering mode
  NodePassThroughModes passThroughMode = initBlendNodePassThroughMode(nodeDef, nodeDefExport);

  //---------------------------
  // Are we merging or additively blending sampled events.
  int32_t mode = kSampledEventBlendModeInvalid;
  nodeDefDataBlock->readInt(mode, "EventsBlendMode");
  NodeSampledEventBlendModes eventBlendMode = (NodeSampledEventBlendModes)mode;

  //---------------------------
  // Initialise the static attrib datas.

  //---------------------------
  // Add the blend weights array.
  MR::AttribDataFloatArray* blendWeights = MR::AttribDataFloatArray::init(memRes, 2, MR::IS_DEF_ATTRIB_DATA);
  float blendWeight = 0.0f;
  nodeDefDataBlock->readFloat(blendWeight, "BlendWeight_0");
  blendWeights->m_values[0] = blendWeight;

  blendWeight = 0.0f;
  nodeDefDataBlock->readFloat(blendWeight, "BlendWeight_1");
  blendWeights->m_values[1] = blendWeight;
  
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS,               // semantic
    0,                                                    // animSetIndex
    blendWeights,                                         // attribData
    MR::AttribDataFloatArray::getMemoryRequirements(2));  // attribMemReqs

  //---------------------------
  // Time stretch attributes
  initBlendNodeTimeStretchModeAttribData(memRes, nodeDefExport, nodeDef);

  //---------------------------
  // Blend mode attributes.
  NodeBlendModes blendMode = getBlendNodeBlendMode(nodeDefExport);

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
    passThroughMode,
    blendMode,
    eventBlendMode,
    slerpDeltaTrajectoryTranslation,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
// Update connections
//----------------------------------------------------------------------------------------------------------------------
void NodeBlend2Builder::initTaskQueuingFnsUpdateConnections(
  MR::NodeDef*                nodeDef,
  NodeTimeStretchModes        timeStretchMode,
  NodeBlendModes              blendMode,
  NMP::BasicLogger*           logger)
{
  // Determine if this is an additive blend mode
  bool isAdditive = false;
  if (blendMode == kAddQuatAddPos)
    isAdditive = true;
  else if (blendMode == kAddQuatInterpPos)       
    isAdditive = true;
  else if (blendMode == kInterpQuatAddPos)
    isAdditive = true;

  if (timeStretchMode == kNodeTimeStretchMatchEvents)
  {
    if (isAdditive)
      nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeBlend2SyncEventsAdditiveUpdateConnections), logger);
    else
      nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeBlend2SyncEventsUpdateConnections), logger);

    nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeBlend2SyncEventsFindGeneratingNodeForSemantic), logger);
  }
  else
  {
    if (isAdditive)
      nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeBlend2AdditiveUpdateConnections), logger);
    else
      nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeBlend2UpdateConnections), logger);

    nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeBlend2FindGeneratingNodeForSemantic), logger);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBlend2Builder::initTaskQueuingFnsEvents(
  MR::NodeDef*                nodeDef,
  NodeTimeStretchModes        timeStretchMode,
  NodePassThroughModes        passThroughMode,
  NodeSampledEventBlendModes  eventBlendMode,
  MR::QueueAttrTaskFn*        taskQueuingFns,
  NMP::BasicLogger*           logger)
{
  //---------------------------
  // MATCH EVENTS
  if (timeStretchMode == kNodeTimeStretchMatchEvents)
  {
    //---------------------------
    // Sync Event Track
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, FN_NAME(nodeBlend2SyncEventsQueueSyncEventTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, FN_NAME(nodeBlend2SyncEventsQueueBlend2DurationEventTrackSets), logger);

    //---------------------------
    // Time
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TIME_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_FRACTION_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);

    // We deal with sync event tracks so don't let any nodes above us pass down event start pos related data to our children.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, NULL, NULL, logger);

    //---------------------------
    // Events
    if (eventBlendMode == kMergeSampledEvents)
    {
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeBlend2SyncEventsQueueSampledEventsBuffers), logger);
    }
    else if (eventBlendMode == kAddSampledEvents)
    {
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeBlend2SyncEventsQueueAddSampledEventsBuffers), logger);
    }

  }
  //---------------------------
  // NO MATCH EVENTS
  else if (timeStretchMode == kNodeTimeStretchNone)
  {
    if (passThroughMode == kNodePassThroughNone)
    {
      //-----------
      // Include handling of sync event related attributes so that if there is a transitMatchEvents somewhere in our parent
      // hierarchy this node will not crash out.
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, FN_NAME(queuePassThroughChild0), logger);
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, FN_NAME(queuePassThroughChild0), logger);

      // Time    
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS, FN_NAME(queuePassThroughChild0), logger);
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TIME_POS, FN_NAME(queuePassThroughChild0), logger);
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_FRACTION_POS, FN_NAME(queuePassThroughChild0), logger);

      // We don't deal with sync event tracks so let any nodes above us pass down event start pos related data to our children.
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, FN_NAME(queuePassThroughChild0), logger);
     }

    //-----------
    // Events
    if (eventBlendMode == kMergeSampledEvents)
    {
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeBlend2QueueSampledEventsBuffers), logger);
    }
    else if (eventBlendMode == kAddSampledEvents)
    {
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeBlend2QueueAddSampledEventsBuffers), logger);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBlend2Builder::initTaskQueuingFnsTransforms(
  MR::NodeDef*                nodeDef,
  NodeBlendModes              blendMode,
  MR::QueueAttrTaskFn*        taskQueuingFns,
  NMP::BasicLogger*           logger)
{
  //---------------------------
  // Transforms
  if (blendMode == kAddQuatAddPos)              // Additive blend attitude and position.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueBlend2TransformBuffsAddAttAddPos), logger);
  else if (blendMode == kAddQuatInterpPos)      // Additive blend attitude and interpolate position.          
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueBlend2TransformBuffsAddAttInterpPos), logger);
  else if (blendMode == kInterpQuatAddPos)      // Interpolate attitude and additive blend position.              
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueBlend2TransformBuffsInterpAttAddPos), logger);
  else if (blendMode == kInterpQuatInterpPos)   // Interpolate attitude and position.                                 
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueBlend2TransformBuffsInterpAttInterpPos), logger);
  else
    NMP_VERIFY_FAIL("Un-expected blend mode.");
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBlend2Builder::initTaskQueuingFnsTrajectoryDelta(
  MR::NodeDef*                nodeDef,
  NodeBlendModes              blendMode,
  bool                        slerpDeltaTrajectoryTranslation,
  MR::QueueAttrTaskFn*        taskQueuingFns,
  NMP::BasicLogger*           logger)
{
  if (slerpDeltaTrajectoryTranslation)
  {
    if (blendMode == kAddQuatAddPos || blendMode == kAddQuatInterpPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeBlend2QueueTrajectoryDeltaTransformAddAttSlerpPos), logger);
    else if (blendMode == kInterpQuatAddPos || blendMode == kInterpQuatInterpPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeBlend2QueueTrajectoryDeltaTransformInterpAttSlerpPos), logger);
    else
      NMP_VERIFY_FAIL("Un-expected blend mode.");
  }
  else
  {
    if (blendMode == kAddQuatAddPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeBlend2QueueTrajectoryDeltaTransformAddAttAddPos), logger);
    else if (blendMode == kAddQuatInterpPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeBlend2QueueTrajectoryDeltaTransformAddAttInterpPos), logger);
    else if (blendMode == kInterpQuatAddPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns,MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeBlend2QueueTrajectoryDeltaTransformInterpAttAddPos), logger);
    else if (blendMode == kInterpQuatInterpPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns,MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeBlend2QueueTrajectoryDeltaTransformInterpAttInterpPos), logger);
    else
      NMP_VERIFY_FAIL("Un-expected blend mode.");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBlend2Builder::initTaskQueuingFnsTrajectoryDeltaAndTransforms(
  MR::NodeDef*                nodeDef,
  NodeBlendModes              blendMode,
  bool                        slerpDeltaTrajectoryTranslation,
  MR::QueueAttrTaskFn*        taskQueuingFns,
  NMP::BasicLogger*           logger)
{
  if (slerpDeltaTrajectoryTranslation)
  {
    if (blendMode == kAddQuatAddPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosSlerpTraj), logger);
    else if (blendMode == kAddQuatInterpPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosSlerpTraj), logger);
    else if (blendMode == kInterpQuatAddPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns,MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosSlerpTraj), logger);
    else if (blendMode == kInterpQuatInterpPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns,MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosSlerpTraj), logger);
    else
      NMP_VERIFY_FAIL("Un-expected blend mode.");
  }
  else
  {
    if (blendMode == kAddQuatAddPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosInterpTraj), logger);
    else if (blendMode == kAddQuatInterpPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosInterpTraj), logger);
    else if (blendMode == kInterpQuatAddPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns,MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosInterpTraj), logger);
    else if (blendMode == kInterpQuatInterpPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns,MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosInterpTraj), logger);
    else
      NMP_VERIFY_FAIL("Un-expected blend mode.");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBlend2Builder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NodeTimeStretchModes        timeStretchMode,
  NodePassThroughModes        passThroughMode,
  NodeBlendModes              blendMode,
  NodeSampledEventBlendModes  eventBlendMode,
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
 
  //---------------------------
  // Pass through
  const uint32_t numEntries = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  switch (passThroughMode)
  {
    case kNodePassThroughSource0:
      for (uint32_t i = 0; i < numEntries; ++i)
      {
        nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0), logger);
      }
      break;
    case kNodePassThroughSource1:
      for (uint32_t i = 0; i < numEntries; ++i)
      {
        nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild1), logger);
      }
      break;
    default:
      break;
  }

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_OUTPUT_MASK, FN_NAME(grouperQueuePassThroughFromParent), logger);

  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);

  //------------------------------------
  // Update connections
  initTaskQueuingFnsUpdateConnections(
    nodeDef,
    timeStretchMode,
    blendMode,
    logger);

  // Events
  initTaskQueuingFnsEvents(
    nodeDef,
    timeStretchMode,
    passThroughMode,
    eventBlendMode,
    taskQueuingFns,
    logger);

  // Transforms
  initTaskQueuingFnsTransforms(
    nodeDef,
    blendMode,
    taskQueuingFns,
    logger);

  // Trajectory delta
  initTaskQueuingFnsTrajectoryDelta(
    nodeDef,
    blendMode,
    slerpDeltaTrajectoryTranslation,
    taskQueuingFns,
    logger);

  // Trajectory delta & Transforms
  initTaskQueuingFnsTrajectoryDeltaAndTransforms(
    nodeDef,
    blendMode,
    slerpDeltaTrajectoryTranslation,
    taskQueuingFns,
    logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
