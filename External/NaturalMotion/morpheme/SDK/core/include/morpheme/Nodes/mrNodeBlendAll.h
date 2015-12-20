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
#ifndef MR_NODE_BLEND_ALL_H
#define MR_NODE_BLEND_ALL_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations
Task* nodeBlendAllQueueBlendTransformBuffsInterpAttInterpPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeBlendAllQueueSampledEventsBuffers(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeBlendAllQueueTrajectoryDeltaTransformInterpAttInterpPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeBlendAllQueueTrajectoryDeltaTransformInterpAttSlerpPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeBlendAllQueueTrajectoryDeltaAndTransformsSlerpTraj(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlendAllQueueTrajectoryDeltaAndTransformsInterpTraj(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
// Overloaded update connections functions.
NodeID nodeBlendAllUpdateConnections(
  NodeDef*           node,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
// Helper functions
//----------------------------------------------------------------------------------------------------------------------
void normaliseWeights(AttribDataFloat** blendCP, const uint16_t numConnections, AttribDataFloatArray* attribBlendWeights, float totalWeight);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_BLEND_ALL_H
//----------------------------------------------------------------------------------------------------------------------
