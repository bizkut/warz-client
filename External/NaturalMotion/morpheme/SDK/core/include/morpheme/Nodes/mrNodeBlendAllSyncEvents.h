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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_NODE_BLEND_ALL_SYNC_EVENTS_H
#define MR_NODE_BLEND_ALL_SYNC_EVENTS_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations

// Sync Event Track
Task* nodeBlendAllSyncEventsQueueSyncEventTrack(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

// Duration event track sets.
Task* nodeBlendAllSyncEventsQueueBlend2DurationEventTrackSets(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

// Transform buffers.
Task* nodeBlendAllSyncEventsQueueSampledEventsBuffers(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_BLEND_ALL_SYNC_EVENTS_H
//----------------------------------------------------------------------------------------------------------------------
