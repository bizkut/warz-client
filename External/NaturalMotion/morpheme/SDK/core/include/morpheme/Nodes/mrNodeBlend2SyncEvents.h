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
#ifndef MR_NODE_BLEND2_SYNC_EVENTS_H
#define MR_NODE_BLEND2_SYNC_EVENTS_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2SyncEventsFindGeneratingNodeForSemantic(
  NodeID              callingNodeID,
  bool                fromParent,   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            nodeDef,
  Network*            network);

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2SyncEventsQueueTimePos(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2SyncEventsQueueSyncEventTrack(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2SyncEventsQueueBlend2DurationEventTrackSets(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2SyncEventsQueueSampledEventsBuffers(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2SyncEventsQueueAddSampledEventsBuffers(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2SyncEventsQueueSampledEventsBuffersBase(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter,
  MR::TaskID      taskIDSampledEventBuffers,
  MR::TaskID      taskIDSampledEventBuffersDurationEventBuffers);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2SyncEventsUpdateConnections(
  NodeDef*           nodeDef,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2SyncEventsAdditiveUpdateConnections(
  NodeDef*           nodeDef,
  Network*           net);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_BLEND2_SYNC_EVENTS_H
//----------------------------------------------------------------------------------------------------------------------
