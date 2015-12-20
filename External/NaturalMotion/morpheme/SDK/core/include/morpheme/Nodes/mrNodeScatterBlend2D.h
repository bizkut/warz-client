// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
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
#ifndef MR_NODE_SCATTER_BLEND_2D_H
#define MR_NODE_SCATTER_BLEND_2D_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
AttribDataBlendWeights* nodeScatterBlend2DCreateBlendWeights(
  const NodeDef* nodeDef,
  Network* net);

AttribDataScatterBlend2DState* nodeScatterBlend2DCreateState(
  const NodeDef* nodeDef,
  Network* net);

//----------------------------------------------------------------------------------------------------------------------
void triangleGetBlendWeights(
  const float*  childNodeWeightsAlwaysBlend,  ///< IN: The input non optimal Barycentric blend weights
  float*        blendWeightsAlwaysBlend );    ///< OUT: The bi-linear blend weights for always blending behaviour

//----------------------------------------------------------------------------------------------------------------------
void triangleBlendWeightCheck(
  bool          alwaysBlendFlag,              ///< IN: Flag to enable blend optimisation
  const float*  childNodeWeightsAlwaysBlend,  ///< IN: The input non optimal Barycentric blend weights
  const NodeID* childNodeIDsAlwaysBlend,      ///< IN: The Node IDs of the non optimal active connected children
  const float*  blendWeightsAlwaysBlend,      ///< IN: The bi-linear blend weights for always blending behaviour
  uint16_t&     numBlendWeightsOpt,           ///< OUT: The number of optimised weights
  float*        blendWeightsOpt,              ///< OUT: The optimised blend weights
  uint16_t&     numChildNodeIDsOpt,           ///< OUT: The number of optimised active connected children
  NodeID*       childNodeIDsOpt,              ///< OUT: The optimised node IDs of the active connected children
  uint16_t&     numInvalidChildNodeIDs,       ///< OUT: The number of children node ids that are optimised away
  NodeID*       invalidChildNodeIDs );        ///< OUT: The optimised away node IDs

//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations
Task* nodeScatterBlend2DQueueBlendTransformBuffsInterpAttInterpPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeScatterBlend2DQueueSampledEventsBuffers(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeScatterBlend2DQueueTrajectoryDeltaTransformInterpAttInterpPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeScatterBlend2DQueueTrajectoryDeltaTransformInterpAttSlerpPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeScatterBlend2DQueueTrajectoryDeltaAndTransformsInterpPosInterpAttInterpTraj(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeScatterBlend2DQueueTrajectoryDeltaAndTransformsInterpPosInterpAttSlerpTraj(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeScatterBlend2DOfflineUpdateConnections(
  NodeDef*           nodeDef,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeScatterBlend2DUpdateConnections(
  NodeDef*           nodeDef,
  Network*           net);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_SCATTER_BLEND_2D_H
//----------------------------------------------------------------------------------------------------------------------
