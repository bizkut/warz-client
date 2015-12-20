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
#ifndef MR_NODE_FEATHER_BLEND2_H
#define MR_NODE_FEATHER_BLEND2_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFeatherBlend2QueueTask(
  NodeDef*            nodeDef,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter,
  TaskID              taskID,
  AttribDataSemantic  taskDataSemantic,
  AttribDataType      attribType,
  NodeID              activeChildNodeID0,
  NodeID              activeChildNodeID1);

//----------------------------------------------------------------------------------------------------------------------
// Transforms
Task* nodeFeatherBlend2QueueFeatherBlend2TransformBuffsAddAttAddPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeFeatherBlend2QueueFeatherBlend2TransformBuffsAddAttInterpPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeFeatherBlend2QueueFeatherBlend2TransformBuffsInterpAttAddPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeFeatherBlend2QueueFeatherBlend2TransformBuffsInterpAttInterpPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
// Delta trajectory
Task* nodeFeatherBlend2QueueTrajectoryDeltaTransformAddAttAddPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeFeatherBlend2QueueTrajectoryDeltaTransformAddAttInterpPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeFeatherBlend2QueueTrajectoryDeltaTransformAddAttSlerpPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeFeatherBlend2QueueTrajectoryDeltaTransformInterpAttAddPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeFeatherBlend2QueueTrajectoryDeltaTransformInterpAttInterpPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeFeatherBlend2QueueTrajectoryDeltaTransformInterpAttSlerpPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
// Delta trajectory and transforms
Task* nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosSlerpTraj(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosSlerpTraj(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosSlerpTraj(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosSlerpTraj(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosInterpTraj(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosInterpTraj(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosInterpTraj(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosInterpTraj(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeFeatherBlend2UpdateConnectionsFixBlendWeight(
  NodeDef*           nodeDef,
  Network*           net);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_FEATHER_BLEND2_H
//----------------------------------------------------------------------------------------------------------------------
