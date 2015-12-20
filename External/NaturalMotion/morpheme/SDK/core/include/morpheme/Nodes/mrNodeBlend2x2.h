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
#ifndef MR_NODE_BLEND_2X2_H
#define MR_NODE_BLEND_2X2_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Blend2x2: where S0 ... Sn are the flattened input source connections
//  ---------> X
//  | S0 S1
//  | S2 S3
//  V
//  Y

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function that create and updates the active frame for the internal blend weights structure.
AttribDataBlendWeights* nodeBlend2x2CreateBlendWeights(
  NodeDef*            nodeDef,
  Network*            net);

//----------------------------------------------------------------------------------------------------------------------
void nodeBlend2x2BlendWeightsCheck(
  bool          alwaysBlendFlag,          ///< IN: Flag to enable blend optimisation
  const float*  blendWeightsAlwaysBlend,  ///< IN: The interpolation weight for non optimal blending
  const NodeID* childNodeIDsAlwaysBlend,  ///< IN: The Node IDs of the non optimal active connected children
  uint16_t&     numBlendWeights,          ///< OUT: The number of optimised weights
  float*        blendWeights,             ///< OUT: The optimised blend weights
  uint16_t&     numChildNodeIDs,          ///< OUT: The number of optimised active connected children
  NodeID*       childNodeIDs,             ///< OUT: The optimised node IDs of the active connected children
  uint16_t&     numInvalidChildNodeIDs,   ///< OUT: The number of children node ids that are optimised away
  NodeID*       invalidChildNodeIDs       ///< OUT: The optimised away node IDs
);

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2x2QueueTask(
  NodeDef*            nodeDef,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter,
  TaskID              taskID,
  AttribDataSemantic  taskDataSemantic,
  AttribDataType      attribType,
  NodeID              activeChildNode0ID,
  NodeID              activeChildNode1ID,
  NodeID              activeChildNode2ID,
  NodeID              activeChildNode3ID);

//----------------------------------------------------------------------------------------------------------------------
// Time and events
Task* nodeBlend2x2QueueSampledEventsBuffers(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

// Transforms
Task* nodeBlend2x2QueueBlendTransformBuffsInterpAttInterpPos(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

// Trajectory delta
Task* nodeBlend2x2QueueTrajectoryDeltaTransformInterpAttInterpPos(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2x2QueueTrajectoryDeltaTransformInterpAttSlerpPos(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

// Trajectory delta and transforms
Task* nodeBlend2x2QueueTrajectoryDeltaAndTransformsInterpPosInterpAttInterpTraj(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2x2QueueTrajectoryDeltaAndTransformsInterpPosInterpAttSlerpTraj(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);


//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2x2UpdateConnections(
  NodeDef*                    nodeDef,
  Network*                    net);

//----------------------------------------------------------------------------------------------------------------------
void nodeBlend2x2UpdateConnectionsSetBlendWeightsCheckForOptimisation(
  AttribDataBlendWeights*   attribBlendWeights,
  BlendOptNodeConnections*  activeNodeConnections,
  Network*                  net,
  const NodeDef*            nodeDef,
  const float*              blendWeightsAlwaysBlend );

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_BLEND_2X2_H
//----------------------------------------------------------------------------------------------------------------------
