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
#ifndef MR_NODE_CLOSEST_ANIM_H
#define MR_NODE_CLOSEST_ANIM_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations
Task* nodeClosestAnimQueueTransforms(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
Task* nodeClosestAnimQueueTrajectoryDeltaTransform(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
Task* nodeClosestAnimQueueTrajectoryDeltaAndTransforms(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
void nodeClosestAnimInitInstance(
  NodeDef* node,
  Network* net);

//----------------------------------------------------------------------------------------------------------------------
void nodeClosestAnimDeleteInstance(
  const NodeDef* node,
  Network*       net
);

//----------------------------------------------------------------------------------------------------------------------
// Overloaded update connections functions.
NodeID nodeClosestAnimUpdateConnections(
  NodeDef*           node,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeClosestAnimFindGeneratingNodeForSemantic(
  NodeID              callingNodeID,
  bool                fromParent,   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            network);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_CLOSEST_ANIM_H
//----------------------------------------------------------------------------------------------------------------------
