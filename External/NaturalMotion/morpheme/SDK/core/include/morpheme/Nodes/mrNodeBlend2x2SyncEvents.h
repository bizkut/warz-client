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
#ifndef MR_NODE_BLEND_2X2_SYNC_EVENTS_H
#define MR_NODE_BLEND_2X2_SYNC_EVENTS_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2x2SyncEventsQueueSyncEventTrack(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2x2SyncEventsQueueSampledEventsBuffers(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2x2SyncEventsQueueBlend2x2DurationEventTrackSets(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2x2SyncEventsUpdateConnections(
  NodeDef*           nodeDef,
  Network*           net);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_BLEND_2X2_SYNC_EVENTS_H
//----------------------------------------------------------------------------------------------------------------------
