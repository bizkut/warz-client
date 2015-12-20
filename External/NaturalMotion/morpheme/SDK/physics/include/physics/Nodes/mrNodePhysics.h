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
#ifndef MR_NODE_PHYSICS_H
#define MR_NODE_PHYSICS_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

NodeID nodePhysicsUpdateConnections(
  NodeDef* node,
  Network* net);

void nodePhysicsDeleteInstance(
  const NodeDef* node,
  Network*       net
);

//----------------------------------------------------------------------------------------------------------------------
Task* nodePhysicsQueueUpdateTrajectory(
  NodeDef* node, 
  TaskQueue* queue, 
  Network* net, 
  TaskParameter* dependentParameter);

Task* nodePhysicsQueueUpdateAnimatedTrajectory(
  NodeDef* node, 
  TaskQueue* queue, 
  Network* net, 
  TaskParameter* dependentParameter);

Task* nodePhysicsQueueUpdatePhysicalTrajectoryPostPhysics(
  NodeDef* node, 
  TaskQueue* queue, 
  Network* net, 
  TaskParameter* dependentParameter);

Task* nodePhysicsQueueUpdateTransformsPrePhysics(
  NodeDef* node, 
  TaskQueue* queue, 
  Network* net, 
  TaskParameter* dependentParameter);

Task* nodePhysicsQueueUpdateTransformsPostPhysics(
  NodeDef* node, 
  TaskQueue* queue, 
  Network* net, 
  TaskParameter* dependentParameter);

Task* nodePhysicsQueueUpdateTransformsNoPhysics(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter);

Task* nodePhysicsQueueBindPoseTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodePhysicsFindGeneratingNodeForSemantic(
  NodeID              callingNodeID,
  bool                fromParent,   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            network);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_PHYSICS_H
//----------------------------------------------------------------------------------------------------------------------
