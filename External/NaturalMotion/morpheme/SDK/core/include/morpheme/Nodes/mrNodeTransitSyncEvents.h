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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_NODE_TRANSIT_SYNC_EVENTS_H
#define MR_NODE_TRANSIT_SYNC_EVENTS_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations
Task* nodeTransitSyncEventsQueueUpdateTimePos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeTransitSyncEventsQueueSyncEventTrack(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeTransitSyncEventsQueueTransformBuffs(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeTransitSyncEventsQueueTrajectoryDeltaAndTransformBuffs(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeTransitSyncEventsQueueTransformBuffsPassThroughDestTrajDelta(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeTransitSyncEventsQueueTransformBuffsPassThroughSourceTrajDelta(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeTransitSyncEventsQueueSampledEventsBuffers(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeTransitSyncEventsQueueBlend2DurationEventTrackSets(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
// Update connections function.
NodeID nodeTransitSyncEventsUpdateConnections(
  NodeDef*           node,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeTransitSyncEventsFindGeneratingNodeForSemantic(
  NodeID              callingNodeID,
  bool                fromParent,   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            network);

//----------------------------------------------------------------------------------------------------------------------
float getDeadBlendDuration(
  Network* net,
  NodeID   nodeID,
  AttribDataTransitSyncEventsDef* transitDef);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_TRANSIT_SYNC_EVENTS_H
//----------------------------------------------------------------------------------------------------------------------
