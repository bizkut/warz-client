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
#ifndef MR_NODE_PHYSICS_GROUPER_H
#define MR_NODE_PHYSICS_GROUPER_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

Task* nodePhysicsGrouperQueueUpdateTrajectory(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodePhysicsGrouperQueueUpdateTransforms(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodePhysicsGrouperQueueGetOutputMask(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodePhysicsGrouperQueueUpdateTransformsPrePhysics(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

NodeID nodePhysicsGrouperUpdateConnections(
  NodeDef*           node,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodePhysicsGrouperFindGeneratingNodeForSemantic(
  NodeID              callingNodeID,
  bool                fromParent,   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            network);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_PHYSICS_GROUPER_H
//----------------------------------------------------------------------------------------------------------------------
