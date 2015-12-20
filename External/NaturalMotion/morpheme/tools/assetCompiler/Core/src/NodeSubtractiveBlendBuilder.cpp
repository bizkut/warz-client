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
#include "NodeSubtractiveBlendBuilder.h"
#include "morpheme/Nodes/mrNodeBlend2.h"
#include "morpheme/Nodes/mrNodeBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodeSubtractiveBlend.h"
#include "morpheme/Nodes/mrNodeSubtractiveBlendSyncEvents.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeSubtractiveBlendBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeSubtractiveBlendBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_SUBTRACTIVE_BLEND);
  NMP_VERIFY(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_LOOP);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeSubtractiveBlendBuilder::getNodeDefInputConnections(
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
}

//----------------------------------------------------------------------------------------------------------------------
void NodeSubtractiveBlendBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeSubtractiveBlendBuilder::getNodeDefMemoryRequirements(
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
    2,               // numChildren
    0,               // numInputCPConnections
    1,               // numAnimSetEntries
    nodeDefExport);

  // Time stretch attributes
  NodeTimeStretchModes timeStretchMode;
  uint32_t numAttribDatasTimeStretch;
  result += getBlendNodeTimeStretchModeMemoryRequirements(
    nodeDefExport,
    timeStretchMode,
    numAttribDatasTimeStretch);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeSubtractiveBlendBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_SUBTRACTIVE_BLEND, "Expecting node type NODE_TYPE_SUBTRACTIVE_BLEND");
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
    0,                      // numInputCPConnections
    0,                      // numOutputCPPins
    1,                      // numAnimSetEntries
    nodeDefExport);

  // Flag as a blend node that supports blend optimisation. 
  // This node re-uses the Blend2 Sampled Events Buffers tasks.
  // Although it currently doesn't have sampled event blend optimisation 
  // implemented it would a simple task if required.
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
  // Time stretch attributes
  initBlendNodeTimeStretchModeAttribData(memRes, nodeDefExport, nodeDef);

  bool slerpDeltaTrajTrans = false;
  nodeDefDataBlock->readBool(slerpDeltaTrajTrans, "SphericallyInterpolateTrajectoryPosition");

  //---------------------------
  // Pass through filtering mode
  NodePassThroughModes passThroughMode = initBlendNodePassThroughMode(nodeDef, nodeDefExport);

  //---------------------------
  // Are we merging or additively blending sampled events.
  int32_t mode = kSampledEventBlendModeInvalid;
  nodeDefDataBlock->readInt(mode, "EventsBlendMode");
  NodeSampledEventBlendModes eventBlendMode = (NodeSampledEventBlendModes)mode;

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    timeStretchMode,
    passThroughMode,
    eventBlendMode,
    slerpDeltaTrajTrans,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeSubtractiveBlendBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NodeTimeStretchModes        timeStretchMode,
  NodePassThroughModes        passThroughMode,
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

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_OUTPUT_MASK, FN_NAME(grouperQueuePassThroughFromParent), logger);

  //-----------
  // MATCH EVENTS
  if (timeStretchMode == kNodeTimeStretchMatchEvents)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TIME_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_FRACTION_POS, FN_NAME(nodeBlend2SyncEventsQueueTimePos), logger);

    // We deal with sync event tracks so don't let any nodes above us pass down event start pos related data to our children.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, NULL, NULL, logger);

    //---------------------------
    // Sampled Events 
    if (eventBlendMode == kMergeSampledEvents)
    {
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeBlend2SyncEventsQueueSampledEventsBuffers), logger);
    }
    else if (eventBlendMode == kAddSampledEvents)
    {
      nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeBlend2SyncEventsQueueAddSampledEventsBuffers), logger);
    }

    //---------------------------
    // Sync Event Tracks
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, FN_NAME(nodeBlend2SyncEventsQueueSyncEventTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, FN_NAME(nodeBlend2SyncEventsQueueBlend2DurationEventTrackSets), logger);
  }
  //-----------
  // NONE MATCH EVENTS
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

      //-----------
      // Sampled Events
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

  //-----------
  // ATTRIB_SEMANTIC_TRANSFORMS
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeSubtractiveBlendQueueTransformBuffsSubtractAttSubtractPos), logger);
 

  //-----------
  // ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM
  if (slerpDeltaTrajectoryTranslation)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeSubtractiveBlendQueueTrajectoryDeltaTransformSubtractAttSlerpPos), logger);
  }
  else
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns,MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeSubtractiveBlendQueueTrajectoryDeltaTransformSubtractAttSubtractPos), logger);
  }

  //-----------
  // ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER
  if (slerpDeltaTrajectoryTranslation)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeSubtractiveBlendQueueTrajectoryDeltaAndTransformsSubtractAttSubtractPosSlerpTraj), logger);
  }
  else
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns,MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeSubtractiveBlendQueueTrajectoryDeltaAndTransformsSubtractAttSubtractPosInterpTraj), logger);
  }

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);

  //------------------------------------
  // Set update connections function.
  if (timeStretchMode == kNodeTimeStretchMatchEvents)
  {
    nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeSubtractiveBlendSyncEventsUpdateConnections), logger);

    nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeBlend2SyncEventsFindGeneratingNodeForSemantic), logger);
  }
  else if (timeStretchMode == kNodeTimeStretchNone)
  {
    nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeSubtractiveBlendUpdateConnections), logger);
    
    nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeBlend2FindGeneratingNodeForSemantic), logger);
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
