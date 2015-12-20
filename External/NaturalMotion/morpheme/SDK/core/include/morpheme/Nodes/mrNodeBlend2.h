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
#ifndef MR_NODE_BLEND2_H
#define MR_NODE_BLEND2_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2FindGeneratingNodeForSemantic(
  NodeID              callingNodeID,
  bool                fromParent,   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            nodeDef,
  Network*            network);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function that creates and updates the active frame for the internal blend weights structure.
AttribDataBlendWeights* nodeBlend2CreateBlendWeights(
  NodeDef*            nodeDef,
  Network*            net);

//----------------------------------------------------------------------------------------------------------------------  
void nodeBlendPlaybackPosInit(
  NodeDef*            nodeDef,
  Network*            net);

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlendQueueTask(
  NodeDef*            nodeDef,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter,
  TaskID              taskID,
  AttribDataSemantic  taskDataSemantic,
  AttribDataType      attribType,
  uint16_t            numChildNodes,
  const NodeID*       childNodeIDs);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function that calculates the blend weight from the input interpolant and
/// the set of source weights. This function clamps the interpolant to the source weight range
/// and returns a blend weight in the range [0:1].
float nodeBlend2CalculateBlendWeight(
  float                 interpolant,            ///< IN: Across any range.
  const float*          sourceWeights           ///< IN: The array of source weights (can be increasing or decreasing)
);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function determines whether the input blend weight lies close to either 0 or 1,
/// selecting a single connected child index, or both sources.
void nodeBlend2BlendWeightCheck(
  bool alwaysBlendFlag,                         ///< IN: Flag to enable blend optimisation
  float blendWeightAlwaysBlend,                 ///< IN: The interpolation weight for non optimal blending
  const NodeID* childNodeIDsAlwaysBlend,        ///< IN: The Node IDs of the non optimal active connected children
  uint16_t& numWeights,                         ///< OUT: The number of optimised weights
  float* blendWeights,                          ///< OUT: The optimised blend weights
  uint16_t& numChildNodeIDs,                    ///< OUT: The number of optimised active connected children
  NodeID* childNodeIDs                          ///< OUT: The optimised node IDs of the active connected children
);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function determines whether the input blend weight lies close to 0,
/// selecting a single connected child node index, or both sources.
void nodeBlend2AdditiveBlendWeightCheck(
  bool alwaysBlendFlag,                         ///< IN: Flag to enable blend optimisation
  float blendWeightAlwaysBlend,                 ///< IN: The interpolation weight for non optimal blending
  const NodeID* childNodeIDsAlwaysBlend,        ///< IN: The Node IDs of the non optimal active connected children
  uint16_t& numWeights,                         ///< OUT: The number of optimised weights
  float* blendWeights,                          ///< OUT: The optimised blend weights
  uint16_t& numChildNodeIDs,                    ///< OUT: The number of optimised active connected children
  NodeID* childNodeIDs                          ///< OUT: The optimised node IDs of the active connected children
);

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2QueueTask(
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
// Time and events
Task* nodeBlend2QueueSampledEventsBuffers(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2QueueAddSampledEventsBuffers(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2QueueSampledEventsBuffersBase(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter, MR::TaskID taskID);

//----------------------------------------------------------------------------------------------------------------------
// Transforms
Task* nodeBlend2QueueBlend2TransformBuffsAddAttAddPos(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2QueueBlend2TransformBuffsAddAttInterpPos(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2QueueBlend2TransformBuffsInterpAttAddPos(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2QueueBlend2TransformBuffsInterpAttInterpPos(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
// Trajectory delta
Task* nodeBlend2QueueTrajectoryDeltaTransformAddAttAddPos(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2QueueTrajectoryDeltaTransformAddAttInterpPos(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2QueueTrajectoryDeltaTransformAddAttSlerpPos(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2QueueTrajectoryDeltaTransformInterpAttAddPos(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2QueueTrajectoryDeltaTransformInterpAttInterpPos(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2QueueTrajectoryDeltaTransformInterpAttSlerpPos(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
// Transforms and Trajectory delta
Task* nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosSlerpTraj(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosSlerpTraj(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosSlerpTraj(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosSlerpTraj(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosInterpTraj(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosInterpTraj(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosInterpTraj(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosInterpTraj(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2UpdateConnections(
  NodeDef*           nodeDef,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2AdditiveUpdateConnections(
  NodeDef*           nodeDef,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2UpdateConnectionsBase(
  NodeDef*  nodeDef,
  Network*  net,
  bool      isSyncEvents,
  bool      isAdditive);

//----------------------------------------------------------------------------------------------------------------------
// a valid NodeID is returned if that node has been deleted.
// else INVALID_NODE_ID.
NodeID nodeBlend2UpdateConnectionsSetBlendWeightsCheckForOptimisation(
  AttribDataBlendWeights*   attribBlendWeights,
  BlendOptNodeConnections*  activeNodeConnections,
  Network*                  net,
  const NodeDef*            nodeDef,
  float                     blendWeight,
  float                     blendWeightEvents,
  bool                      isAdditive );

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_BLEND2_H
//----------------------------------------------------------------------------------------------------------------------
