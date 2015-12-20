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
#ifndef MR_NODE_SUBTRACTIVE_BLEND_H
#define MR_NODE_SUBTRACTIVE_BLEND_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function that creates and updates the active frame for the internal blend weights structure.
AttribDataBlendWeights* nodeSubtractiveBlendCreateBlendWeights(
  NodeDef* nodeDef,
  Network* net);

//----------------------------------------------------------------------------------------------------------------------
// Transforms
Task* nodeSubtractiveBlendQueueTransformBuffsSubtractAttSubtractPos(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

// Trajectory delta
Task* nodeSubtractiveBlendQueueTrajectoryDeltaTransformSubtractAttSubtractPos(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeSubtractiveBlendQueueTrajectoryDeltaTransformSubtractAttSlerpPos(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

// Transforms and Trajectory delta
Task* nodeSubtractiveBlendQueueTrajectoryDeltaAndTransformsSubtractAttSubtractPosSlerpTraj(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeSubtractiveBlendQueueTrajectoryDeltaAndTransformsSubtractAttSubtractPosInterpTraj(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeSubtractiveBlendUpdateConnections(
  NodeDef*           nodeDef,
  Network*           net);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_SUBTRACTIVE_BLEND_H
//----------------------------------------------------------------------------------------------------------------------
