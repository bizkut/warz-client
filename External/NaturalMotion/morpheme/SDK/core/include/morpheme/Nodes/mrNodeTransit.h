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
#ifndef MR_NODE_TRANSIT_H
#define MR_NODE_TRANSIT_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations
Task* nodeTransitQueueUpdateTimePos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeTransitBlend2QueueSampledEventsBuffers(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeTransitQueueDeadBlendCacheState(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* transitionQueueTrajectoryDeltaAddAttSlerpPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* transitionQueueTrajectoryDeltaInterpAttSlerpPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* transitionQueueTrajectoryDeltaAddAttAddPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* transitionQueueTrajectoryDeltaAddAttInterpPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* transitionQueueTrajectoryDeltaInterpAttAddPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* transitionQueueTrajectoryDeltaInterpAttInterpPos(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* transitionQueuePhysicsTrajectoryDeltaUpdate(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter, TaskID taskIDIfAnimRootBlending);
Task* nodeTransitQueueTrajectoryDeltaTransformTask(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter, TaskID taskID, AttribDataSemantic trajectorySemantic, AttribDataType attribType);

Task* queuePassThroughChild0IfNotPhysics(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* queuePassThroughChild1IfNotPhysics(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

Task* nodeTransitQueueTransformBuffs(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeTransitQueueTrajectoryDeltaAndTransformBuffs(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeTransitQueueTransformBuffsPassThroughDestTrajDelta(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeTransitQueueTransformBuffsPassThroughSourceTrajDelta(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeQueueAnimationDeadBlendUpdateTransforms(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeQueueAnimationDeadBlendUpdateTrajectoryDeltaAndTransforms(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeTransitQueueBlend2TransformBuffs(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter, uint32_t blendMode);
Task* nodeTransitQueueBlend2TrajectoryDeltaAndTransforms(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter, bool slerpTraj, uint32_t blendMode);
Task* nodeTransitQueueBlend2TransformsPassDestTrajDelta(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter, uint32_t blendMode);
Task* nodeTransitQueueBlend2TransformsPassSourceTrajDelta(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter, uint32_t blendMode);

//----------------------------------------------------------------------------------------------------------------------
void nodeTransitDeleteInstance(
  const NodeDef* node,
  Network*       net);

//----------------------------------------------------------------------------------------------------------------------
// Update connections function.
NodeID nodeTransitUpdateConnections(
  NodeDef*           node,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeTransitFindGeneratingNodeForSemantic(
  NodeID              callingNodeID,
  bool                fromParent,
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            network);

//----------------------------------------------------------------------------------------------------------------------
// helper functions for accessing source and destination states of a transit node
NodeID nodeTransitGetSourceState(const NodeDef* node);
NodeID nodeTransitGetDestinationState(const NodeDef* node);

// complete transition function also used by TransitSyncEvents
NodeID nodeTransitCompleteTransition(
  NodeDef*            node,
  Network*            net,
  NodeConnections*    nodeConnections,
  bool                reversed);

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NodeID getOwningStateMachineID(
  Network* net,
  NodeID   smChildNodeID) // Must be an immediate child of a state machine in Def data.
{
  NMP_ASSERT(net && net->getNetworkDef());
  NodeDef* smChildNodeDef = net->getNetworkDef()->getNodeDef(smChildNodeID);
  NMP_ASSERT(smChildNodeDef);
  NMP_ASSERT(net->getNetworkDef()->getNodeDef(smChildNodeDef->getParentNodeID())->getNodeTypeID() == NODE_TYPE_STATE_MACHINE);

  return smChildNodeDef->getParentNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
// Get dead blend duration from transit
NM_INLINE float getDeadBlendDuration(
  Network* NMP_UNUSED(net),
  NodeID NMP_UNUSED(node),
  AttribDataTransitDef* transitDef)
{
  return transitDef->m_duration;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_TRANSIT_H
//----------------------------------------------------------------------------------------------------------------------
