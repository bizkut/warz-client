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
#ifndef MR_NODE_ANIM_SYNC_EVENTS
#define MR_NODE_ANIM_SYNC_EVENTS
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Update connections function declaration.
NodeID nodeAnimSyncEventsUpdateConnections(NodeDef* node, Network* net);

//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations.
Task* nodeAnimSyncEventsQueueTimePosPreCompSyncTrack(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeAnimSyncEventsQueueTimePosDynamicSyncTrack(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeAnimSyncEventsQueueSampleTransformsFromAnimSource(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeAnimSyncEventsQueueSampleTransformsFromAnimSourceASA(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeAnimSyncEventsQueueSampleTransformsFromAnimSourceMBA(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeAnimSyncEventsQueueSampleTransformsFromAnimSourceNSA(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeAnimSyncEventsQueueSampleTransformsFromAnimSourceQSA(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeAnimSyncEventsQueueTrajectoryDeltaTransform(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeAnimSyncEventsQueueTrajectoryDeltaTransformASA(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeAnimSyncEventsQueueTrajectoryDeltaTransformMBA(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeAnimSyncEventsQueueTrajectoryDeltaTransformNSA(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeAnimSyncEventsQueueTrajectoryDeltaTransformQSA(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSource(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSourceASA(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSourceMBA(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSourceNSA(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSourceQSA(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeAnimSyncEventsQueueSampledEventsBufferPreCompSyncTrack(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeAnimSyncEventsQueueSampledEventsBufferDynamicSyncTrack(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeAnimSyncEventsFindGeneratingNodeForSemantic(
  NodeID              callingNodeID,
  bool                fromParent,   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            network);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_ANIM_SYNC_EVENTS
//----------------------------------------------------------------------------------------------------------------------
