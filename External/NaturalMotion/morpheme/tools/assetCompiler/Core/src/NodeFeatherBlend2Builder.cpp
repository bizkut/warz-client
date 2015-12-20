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
#include "NodeFeatherBlend2Builder.h"
#include "morpheme/Nodes/mrNodeBlend2.h"
#include "morpheme/Nodes/mrNodeBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrNodeFeatherBlend2.h"
#include "morpheme/Nodes/mrNodeFeatherBlend2SyncEvents.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeFeatherBlend2Builder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeFeatherBlend2Builder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_FEATHER_BLEND_2);
  NMP_VERIFY(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_BONE_WEIGHTS);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_LOOP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_BLEND_FLAGS);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeFeatherBlend2Builder::getNodeDefInputConnections(
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

  // The blend weight is optional so check for a valid node id.
  readDataPinChildNodeID(nodeDefDataBlock, "Weight", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "EventBlendingWeight", childNodeIDs, true);

}

//----------------------------------------------------------------------------------------------------------------------
void NodeFeatherBlend2Builder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "Weight", 0, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "EventBlendingWeight", 1, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeFeatherBlend2Builder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Feather blend channel alphas (Anim set specific)
  uint32_t numAnimSets = 0;
  nodeDefDataBlock->readUInt(numAnimSets, "NumAnimSets");
  NMP_VERIFY(numAnimSets > 0);
 
  //---------------------------
  // Allocate space for the NodeDef itself.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    2,                      // numChildren
    2,                      // numInputCPConnections
    numAnimSets,            // numAnimSetEntries
    nodeDefExport);

  //---------------------------
  // Reserve space for the blend weights array.
  result += MR::AttribDataFloatArray::getMemoryRequirements(2);

  // Feather blend channel alphas (Anim set specific)
  CHAR paramNumAlphas[256];
  for (uint32_t i = 0; i < numAnimSets; ++i)
  {

    sprintf_s(paramNumAlphas, "ChannelAlphasSet%dCount", i);
    uint32_t numAlphaValues = 0;
    nodeDefDataBlock->readUInt(numAlphaValues, paramNumAlphas);
    result += MR::AttribDataFeatherBlend2ChannelAlphas::getMemoryRequirements((uint16_t)numAlphaValues);
  }
  
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
MR::NodeDef* NodeFeatherBlend2Builder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_FEATHER_BLEND_2, "Expecting node type FEATHER_BLEND_2");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  NMP_VERIFY(animLibraryExport);

  //---------------------------
  // Feather blend channel alphas (Anim set specific)
  int16_t numAnimSets = readNumAnimSets(nodeDefDataBlock);
    
  // Time stretch attributes
  NodeTimeStretchModes timeStretchMode;
  uint32_t numAttribDatasTimeStretch;
  NMP::Memory::Format memReqsTimeStretch = getBlendNodeTimeStretchModeMemoryRequirements(
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
    numAnimSets,            // numAnimSetEntries                     
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
  // Initialise the attrib datas.
  
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
    MR::ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS,              // semantic,    
    0,                                                   // animSetIndex,
    blendWeights,                                        // attribData,  
    MR::AttribDataFloatArray::getMemoryRequirements(2)); // attribMemReqs

  //---------------------------
  // Feather blend channel alphas (Anim set specific)
  CHAR paramNumAlphas[256];
  CHAR paramAlpha[256];
  for (uint16_t setIndex = 0; setIndex < numAnimSets; ++setIndex)
  {
    // Get the animation rig
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(setIndex);
    const ME::RigExport* animRigExport = animSetExport->getRig();
    NMP_VERIFY(animRigExport);
    const ProcessedAsset* animRigAsset = processor->findProcessedAsset(animRigExport->getGUID());
    NMP_VERIFY(animRigAsset);
    MR::AnimRigDef* animRigDef = (MR::AnimRigDef*) animRigAsset->assetMemory.ptr;
    NMP_VERIFY(animRigDef);

    // Get the trajectory bone index
    uint32_t trajBoneIndex = animRigDef->getTrajectoryBoneIndex();

    // Reserve space for remaining attrib datas.
    sprintf_s(paramNumAlphas, "ChannelAlphasSet%dCount", setIndex);
    uint32_t numAlphaValues = 0;
    nodeDefDataBlock->readUInt(numAlphaValues, paramNumAlphas);    

    MR::AttribDataFeatherBlend2ChannelAlphas* alphaValuesAttribData =
      MR::AttribDataFeatherBlend2ChannelAlphas::init(memRes, (uint16_t)numAlphaValues, MR::IS_DEF_ATTRIB_DATA);
    float alphaValue;
    for (uint32_t j = 0; j < numAlphaValues; ++j)
    {
      sprintf_s(paramAlpha, "ChannelAlphasSet%d_Value%d", setIndex, j);
      alphaValue = 0.0f;
      nodeDefDataBlock->readFloat(alphaValue, paramAlpha);
      alphaValuesAttribData->m_channelAlphas[j] = alphaValue;
    }
    // tidy AlphaValues
    uint32_t avTidy = numAlphaValues;
    while (avTidy & 3)
    {
      (alphaValuesAttribData->m_channelAlphas[avTidy++]) = 1.0f;
    }

    // Determine the filtered out flags
    NMP_VERIFY(trajBoneIndex < numAlphaValues);
    alphaValuesAttribData->m_trajectoryBoneIndex = (uint16_t)trajBoneIndex;

    // Add the attrib data entry
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_BONE_WEIGHTS,                                 // semantic,    
      setIndex,                                                         // animSetIndex,
      alphaValuesAttribData,                                            // attribData,  
      MR::AttribDataFeatherBlend2ChannelAlphas::getMemoryRequirements((uint16_t)numAlphaValues)); // attribMemReqs
  }

  //---------------------------
  // Time stretch attributes
  initBlendNodeTimeStretchModeAttribData(memRes, nodeDefExport, nodeDef);
 
  //---------------------------
  // Blend mode attributes.
  bool additiveBlendAtt = false;
  nodeDefDataBlock->readBool(additiveBlendAtt, "AdditiveBlendAttitude");
  bool additiveBlendPos = false;
  nodeDefDataBlock->readBool(additiveBlendPos, "AdditiveBlendPosition");

  //---------------------------
  // Are we merging or additively blending sampled events.
  int32_t mode = kSampledEventBlendModeInvalid;
  nodeDefDataBlock->readInt(mode, "EventsBlendMode");
  NodeSampledEventBlendModes eventBlendMode = (NodeSampledEventBlendModes)mode;

  NodeBlendModes blendMode;
  if (additiveBlendAtt && additiveBlendPos)
    blendMode = kAddQuatAddPos;
  else if (additiveBlendAtt && !additiveBlendPos)
    blendMode = kAddQuatInterpPos;
  else if (!additiveBlendAtt && additiveBlendPos)
    blendMode = kInterpQuatAddPos;
  else
    blendMode = kInterpQuatInterpPos;

  bool slerpDeltaTrajTrans = false;
  nodeDefDataBlock->readBool(slerpDeltaTrajTrans, "SphericallyInterpolateTrajectoryPosition");

  //---------------------------
  // Blending flags.
  initBlendNodeBlendFlags(
    memRes,
    nodeDefExport,
    nodeDef);

  //---------------------------
  // Initialise the task function tables
  MR::NodeID weightNodeID;
  readNodeID(nodeDefDataBlock, "Weight", weightNodeID, true);

  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    timeStretchMode,
    passThroughMode,
    blendMode,
    eventBlendMode,
    slerpDeltaTrajTrans,
    weightNodeID != MR::INVALID_NODE_ID, // was there a connection to the blend weight pin
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeFeatherBlend2Builder::initTaskQueuingFnsUpdateConnections(
  MR::NodeDef*                nodeDef,
  NodeTimeStretchModes        timeStretchMode,
  bool                        isBlendWeightConnected,
  NMP::BasicLogger*           logger)
{
  if (timeStretchMode == kNodeTimeStretchMatchEvents)
  {
    if (isBlendWeightConnected)
    {
      // Use additive blend weight check for the transforms
      nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeBlend2SyncEventsAdditiveUpdateConnections), logger);
    }
    else
    {
      // We need to fix the blend weight to 1
      nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeFeatherBlend2SyncEventsUpdateConnectionsFixBlendWeight), logger);
    }

    nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeBlend2SyncEventsFindGeneratingNodeForSemantic), logger);
  }
  //---------------------------
  // NO MATCH EVENTS
  else if (timeStretchMode == kNodeTimeStretchNone)
  {
    if (isBlendWeightConnected)
    {
      // Use additive blend weight check for the transforms
      nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeBlend2AdditiveUpdateConnections), logger);
    }
    else
    {
      // We need to fix the blend weight to 1
      nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeFeatherBlend2UpdateConnectionsFixBlendWeight), logger);
    }

    nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeBlend2FindGeneratingNodeForSemantic), logger);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeFeatherBlend2Builder::initTaskQueuingFnsEvents(
  MR::NodeDef*                nodeDef,
  NodeTimeStretchModes        timeStretchMode,
  NodePassThroughModes        passThroughMode,
  NodeSampledEventBlendModes  eventBlendMode,
  MR::QueueAttrTaskFn*        taskQueuingFns,
  NMP::BasicLogger*           logger)
{
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

    //---------------------------
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
void NodeFeatherBlend2Builder::initTaskQueuingFnsTransforms(
  MR::NodeDef*                nodeDef,
  NodeBlendModes              blendMode,
  MR::QueueAttrTaskFn*        taskQueuingFns,
  NMP::BasicLogger*           logger)
{
  if (blendMode == kAddQuatAddPos)         // Additive blend attitude and position.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeFeatherBlend2QueueFeatherBlend2TransformBuffsAddAttAddPos), logger);
  else if (blendMode == kAddQuatInterpPos)      // Additive blend attitude and interpolate position.          
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeFeatherBlend2QueueFeatherBlend2TransformBuffsAddAttInterpPos), logger);
  else if (blendMode == kInterpQuatAddPos)      // Interpolate attitude and additive blend position.              
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeFeatherBlend2QueueFeatherBlend2TransformBuffsInterpAttAddPos), logger);
  else if (blendMode == kInterpQuatInterpPos)   // Interpolate attitude and position.                                 
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeFeatherBlend2QueueFeatherBlend2TransformBuffsInterpAttInterpPos), logger);
  else
    NMP_VERIFY_FAIL("Un-expected blend mode.");
}

//----------------------------------------------------------------------------------------------------------------------
void NodeFeatherBlend2Builder::initTaskQueuingFnsTrajectoryDelta(
  MR::NodeDef*                nodeDef,
  NodeBlendModes              blendMode,
  bool                        slerpDeltaTrajectoryTranslation,
  MR::QueueAttrTaskFn*        taskQueuingFns,
  NMP::BasicLogger*           logger)
{
  if (slerpDeltaTrajectoryTranslation)
  {
    if (blendMode == kAddQuatAddPos || blendMode == kAddQuatInterpPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeFeatherBlend2QueueTrajectoryDeltaTransformAddAttSlerpPos), logger);
    else if (blendMode == kInterpQuatAddPos || blendMode == kInterpQuatInterpPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeFeatherBlend2QueueTrajectoryDeltaTransformInterpAttSlerpPos), logger);
    else
      NMP_VERIFY_FAIL("Un-expected blend mode.");
  }
  else
  {
    if (blendMode == kAddQuatAddPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeFeatherBlend2QueueTrajectoryDeltaTransformAddAttAddPos), logger);
    else if (blendMode == kAddQuatInterpPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeFeatherBlend2QueueTrajectoryDeltaTransformAddAttInterpPos), logger);
    else if (blendMode == kInterpQuatAddPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns,MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeFeatherBlend2QueueTrajectoryDeltaTransformInterpAttAddPos), logger);
    else if (blendMode == kInterpQuatInterpPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns,MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeFeatherBlend2QueueTrajectoryDeltaTransformInterpAttInterpPos), logger);
    else
      NMP_VERIFY_FAIL("Un-expected blend mode.");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeFeatherBlend2Builder::initTaskQueuingFnsTrajectoryDeltaAndTransforms(
  MR::NodeDef*                nodeDef,
  NodeBlendModes              blendMode,
  bool                        slerpDeltaTrajectoryTranslation,
  MR::QueueAttrTaskFn*        taskQueuingFns,
  NMP::BasicLogger*           logger)
{
  if (slerpDeltaTrajectoryTranslation)
  {
    if (blendMode == kAddQuatAddPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosSlerpTraj), logger);
    else if (blendMode == kAddQuatInterpPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosSlerpTraj), logger);
    else if (blendMode == kInterpQuatAddPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosSlerpTraj), logger);
    else if (blendMode == kInterpQuatInterpPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosSlerpTraj), logger);
    else
      NMP_VERIFY_FAIL("Un-expected blend mode.");
  }
  else
  {
    if (blendMode == kAddQuatAddPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosInterpTraj), logger);
    else if (blendMode == kAddQuatInterpPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosInterpTraj), logger);
    else if (blendMode == kInterpQuatAddPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosInterpTraj), logger);
    else if (blendMode == kInterpQuatInterpPos)
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosInterpTraj), logger);
    else
      NMP_VERIFY_FAIL("Un-expected blend mode.");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeFeatherBlend2Builder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NodeTimeStretchModes        timeStretchMode,
  NodePassThroughModes        passThroughMode,
  NodeBlendModes              blendMode,
  NodeSampledEventBlendModes  eventBlendMode,
  bool                        slerpDeltaTrajectoryTranslation,
  bool                        isBlendWeightConnected,
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
    isBlendWeightConnected,
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
