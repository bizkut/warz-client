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
#include "morpheme/Nodes/mrNodeTransit.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrBlendOps.h"
#include "morpheme/Nodes/mrNodeStateMachine.h" // MORPH-21351: Remove the necessity for this include.
//----------------------------------------------------------------------------------------------------------------------

// non-public template definitions
#include "mrNodeTransit.inl"

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// helper functions for retrieving source, destination states of the transit node
NodeID nodeTransitGetSourceState(const NodeDef* node)
{
  NMP_ASSERT(node);
  return node->getChildNodeID(0);
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeTransitGetDestinationState(const NodeDef* node)
{
  NMP_ASSERT(node);
  return node->getChildNodeID(1);
}

//----------------------------------------------------------------------------------------------------------------------
void nodeTransitDeleteInstance(
  const NodeDef* node,
  Network*       net)
{
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  NMP_ASSERT(connections);

  // Recurse to our children first.
  for (uint32_t iChild = 0; iChild < connections->m_numActiveChildNodes; ++iChild)
  {
    net->deleteNodeInstance(connections->m_activeChildNodeIDs[iChild]);
  }

  // Delete all our own existing attribute data.
  net->cleanNodeData(node->getNodeID());
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeQueueAnimationDeadBlendUpdateTransforms(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  // Queue the task that computes the dead blend on the transforms.
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_ANIMDEADBLENDTRANSFORMBUFFS,
                 node->getNodeID(),
                 8,
                 dependentParameter,
                 false,
                 false);
  if (task)
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);
    NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());

    // Build list of all parameters (attributes) that are needed for the task to operate.
    // Also build a list of attribute addresses that we are dependent on having been updated before us.

    // Destination node transforms we want to blend with.
    NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);
    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    // Dead blend state data.
    net->TaskAddNetInputParam(task, 1, ATTRIB_SEMANTIC_DEAD_BLEND_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER);

    // Dead blend def data.
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_DEAD_BLEND_DEF, node->getNodeID());

    // Use the delta time to compute the dead blend extrapolation.
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeParentNodeID, node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo);

    // Use the weighting value from the transition update time.
    net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_BLEND_WEIGHTS, ATTRIB_TYPE_BLEND_WEIGHTS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    // Dead blend transform rates. Use "0" as target node to make the address unique.
    net->TaskAddNetInputOutputParam(task, 5, ATTRIB_SEMANTIC_TRANSFORM_RATES, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);

    // Dead blend data which includes the current transforms (blended during the transition).
    net->TaskAddNetInputOutputParam(task, 6, ATTRIB_SEMANTIC_DEAD_BLEND_TRANSFORMS_STATE, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);

    // The dead blend transition outputs transforms.
    net->TaskAddOutputParamZeroLifespan(task, 7, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  }

  return task;
}
//----------------------------------------------------------------------------------------------------------------------
Task* nodeQueueAnimationDeadBlendUpdateTrajectoryDeltaAndTransforms(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  // Queue the task that computes the dead blend on the transforms.
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_ANIMDEADBLENDTRAJECTORYDELTAANDTRANSFORMBUFFS,
    node->getNodeID(),
    8,
    dependentParameter,
    false,
    false);
  if (task)
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);
    NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());

    // Build list of all parameters (attributes) that are needed for the task to operate.
    // Also build a list of attribute addresses that we are dependent on having been updated before us.

    // Destination node transforms we want to blend with.
    NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);
    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    // Dead blend state data.
    net->TaskAddNetInputParam(task, 1, ATTRIB_SEMANTIC_DEAD_BLEND_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER);

    // Dead blend def data.
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_DEAD_BLEND_DEF, node->getNodeID());

    // Use the delta time to compute the dead blend extrapolation.
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeParentNodeID, node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo);

    // Use the weighting value from the transition update time.
    net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_BLEND_WEIGHTS, ATTRIB_TYPE_BLEND_WEIGHTS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    // Dead blend transform rates. Use "0" as target node to make the address unique.
    net->TaskAddNetInputParam(task, 5, ATTRIB_SEMANTIC_TRANSFORM_RATES, node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME, activeAnimSetIndex);

    // Dead blend data which includes the current transforms (blended during the transition).
    net->TaskAddNetInputOutputParam(task, 6, ATTRIB_SEMANTIC_DEAD_BLEND_TRANSFORMS_STATE, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);

    // The dead blend transition outputs transforms.
    net->TaskAddOutputParamZeroLifespan(task, 7, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitQueueBlend2TransformBuffers(
  TaskID              taskID,
  AttribDataSemantic  transformSemantic,
  AttribDataType      attribType,
  NodeDef*            node,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter)
{
  bool addDeadBlendCaching = (node->getNodeFlags() & NodeDef::NODE_FLAG_IS_TRANSITION_CAN_DEAD_BLEND) != 0;
  uint32_t numParams = 4 + (addDeadBlendCaching ? 1 : 0);
  NodeID nodeID = node->getNodeID();

  Task* task = queue->createNewTaskOnQueue(
                 taskID,
                 nodeID,
                 numParams,
                 dependentParameter,
                 false, // External task
                 true); // SPU compatible
  if (task)
  {
    uint32_t currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);
    NodeID activeChildNodeID0 = net->getActiveChildNodeID(nodeID, 0);
    NodeID activeChildNodeID1 = net->getActiveChildNodeID(nodeID, 1);

    net->TaskAddOutputParamZeroLifespan(task, 0, transformSemantic, attribType, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);

    net->TaskAddParamAndDependency(task, 1, transformSemantic, attribType, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddParamAndDependency(task, 2, transformSemantic, attribType, activeChildNodeID1, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    // This node needs to generate a weighting value.
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_BLEND_WEIGHTS, ATTRIB_TYPE_BLEND_WEIGHTS, nodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    if (addDeadBlendCaching)
    {
      net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_T_MINUS_ONE, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, nodeID, nodeID, TPARAM_FLAG_INPUT, currFrameNo + 1);
    }
  }

  return task;
}
 
//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitQueueBlend2TransformBuffs(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter,
  uint32_t       blendMode)
{
  switch (blendMode)
  {
  case BLEND_MODE_ADD_ATT_ADD_POS:
    return nodeTransitQueueBlend2TransformBuffers(
      CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMBUFFSADDATTADDPOS,
      ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      node,
      queue,
      net,
      dependentParameter);
  case BLEND_MODE_ADD_ATT_LEAVE_POS:
    return nodeTransitQueueBlend2TransformBuffers(
      CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMBUFFSADDATTINTERPPOS,
      ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      node,
      queue,
      net,
      dependentParameter);
  case BLEND_MODE_INTERP_ATT_ADD_POS:
    return nodeTransitQueueBlend2TransformBuffers(
      CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMBUFFSINTERPATTADDPOS,
      ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      node,
      queue,
      net,
      dependentParameter);
  case BLEND_MODE_INTERP_ATT_INTERP_POS:
    return nodeTransitQueueBlend2TransformBuffers(
      CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMBUFFSINTERPATTINTERPPOS,
      ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      node,
      queue,
      net,
      dependentParameter);
  default:
    // Unknown blend mode.
    NMP_ASSERT_FAIL();
    return NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitQueueBlend2TrajectoryDeltaAndTransforms(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter,
  bool           slerpTrajectory,
  uint32_t       blendMode)
{
  if (slerpTrajectory)
  {
    switch (blendMode)
    {
    case BLEND_MODE_ADD_ATT_ADD_POS:
      return nodeTransitQueueBlend2TransformBuffers(
        CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSADDATTADDPOSSLERPTRAJ,
        ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
        ATTRIB_TYPE_TRANSFORM_BUFFER,
        node,
        queue,
        net,
        dependentParameter);
    case BLEND_MODE_ADD_ATT_LEAVE_POS:
      return nodeTransitQueueBlend2TransformBuffers(
        CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSADDATTINTERPPOSSLERPTRAJ,
        ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
        ATTRIB_TYPE_TRANSFORM_BUFFER,
        node,
        queue,
        net,
        dependentParameter);
    case BLEND_MODE_INTERP_ATT_ADD_POS:
      return nodeTransitQueueBlend2TransformBuffers(
        CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTADDPOSSLERPTRAJ,
        ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
        ATTRIB_TYPE_TRANSFORM_BUFFER,
        node,
        queue,
        net,
        dependentParameter);
    case BLEND_MODE_INTERP_ATT_INTERP_POS:
      return nodeTransitQueueBlend2TransformBuffers(
        CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTINTERPPOSSLERPTRAJ,
        ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
        ATTRIB_TYPE_TRANSFORM_BUFFER,
        node,
        queue,
        net,
        dependentParameter);
    default:
      // Unknown blend mode.
      NMP_ASSERT_FAIL();
      return NULL;
    }
  }
  else
  {
    switch (blendMode)
    {
    case BLEND_MODE_ADD_ATT_ADD_POS:
      return nodeTransitQueueBlend2TransformBuffers(
        CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSADDATTADDPOSINTERPTRAJ,
        ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
        ATTRIB_TYPE_TRANSFORM_BUFFER,
        node,
        queue,
        net,
        dependentParameter);
    case BLEND_MODE_ADD_ATT_LEAVE_POS:
      return nodeTransitQueueBlend2TransformBuffers(
        CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSADDATTINTERPPOSINTERPTRAJ,
        ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
        ATTRIB_TYPE_TRANSFORM_BUFFER,
        node,
        queue,
        net,
        dependentParameter);
    case BLEND_MODE_INTERP_ATT_ADD_POS:
      return nodeTransitQueueBlend2TransformBuffers(
        CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTADDPOSINTERPTRAJ,
        ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
        ATTRIB_TYPE_TRANSFORM_BUFFER,
        node,
        queue,
        net,
        dependentParameter);
    case BLEND_MODE_INTERP_ATT_INTERP_POS:
      return nodeTransitQueueBlend2TransformBuffers(
        CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTINTERPPOSINTERPTRAJ,
        ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
        ATTRIB_TYPE_TRANSFORM_BUFFER,
        node,
        queue,
        net,
        dependentParameter);
    default:
      // Unknown blend mode.
      NMP_ASSERT_FAIL();
      return NULL;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitQueueBlend2TransformsPassDestTrajDelta(
  NodeDef* node,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter,
  uint32_t blendMode)
{
  switch (blendMode)
  {
  case BLEND_MODE_ADD_ATT_ADD_POS:
    return nodeTransitQueueBlend2TransformBuffers(
      CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMSINTERPATTINTERPPOSPASSDESTTRAJ,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      node,
      queue,
      net,
      dependentParameter);
  case BLEND_MODE_ADD_ATT_LEAVE_POS:
    return nodeTransitQueueBlend2TransformBuffers(
      CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMSADDATTINTERPPOSPASSDESTTRAJ,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      node,
      queue,
      net,
      dependentParameter);
  case BLEND_MODE_INTERP_ATT_ADD_POS:
    return nodeTransitQueueBlend2TransformBuffers(
      CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMSINTERPATTADDPOSPASSDESTTRAJ,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      node,
      queue,
      net,
      dependentParameter);
  case BLEND_MODE_INTERP_ATT_INTERP_POS:
    return nodeTransitQueueBlend2TransformBuffers(
      CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMSINTERPATTINTERPPOSPASSDESTTRAJ,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      node,
      queue,
      net,
      dependentParameter);
  default:
    // Unknown blend mode.
    NMP_ASSERT_FAIL();
    return NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitQueueBlend2TransformsPassSourceTrajDelta(
  NodeDef* node,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter,
  uint32_t blendMode)
{
  switch (blendMode)
  {
  case BLEND_MODE_ADD_ATT_ADD_POS:
    return nodeTransitQueueBlend2TransformBuffers(
      CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMSADDATTADDPOSPASSSOURCETRAJ,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      node,
      queue,
      net,
      dependentParameter);
  case BLEND_MODE_ADD_ATT_LEAVE_POS:
    return nodeTransitQueueBlend2TransformBuffers(
      CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMSADDATTINTERPPOSPASSSOURCETRAJ,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      node,
      queue,
      net,
      dependentParameter);
  case BLEND_MODE_INTERP_ATT_ADD_POS:
    return nodeTransitQueueBlend2TransformBuffers(
      CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMSINTERPATTADDPOSPASSSOURCETRAJ,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      node,
      queue,
      net,
      dependentParameter);
  case BLEND_MODE_INTERP_ATT_INTERP_POS:
    return nodeTransitQueueBlend2TransformBuffers(
      CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMSINTERPATTINTERPPOSPASSSOURCETRAJ,
      ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      node,
      queue,
      net,
      dependentParameter);
  default:
    // Unknown blend mode.
    NMP_ASSERT_FAIL();
    return NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitBlend2QueueSampledEventsBuffers(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  if (isDeadBlendingOrBlendingToPhysics(node, net))
  {
    return MR::queuePassThroughChild0(node, queue, net, dependentParameter);
  }
  else
  {
    Task* task = queue->createNewTaskOnQueue(
                   CoreTaskIDs::MR_TASKID_COMBINE2SAMPLEDEVENTSBUFFERS,
                   node->getNodeID(),
                   4,
                   dependentParameter);
    if (task)
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      NMP_ASSERT(currFrameNo > 0);
      NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);
      NodeID activeChildNodeID1 = net->getActiveChildNodeID(node->getNodeID(), 1);

      net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, INVALID_NODE_ID, currFrameNo);
      net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, activeChildNodeID1, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      // This node needs to generate a weighting value.
      net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_BLEND_WEIGHTS, ATTRIB_TYPE_BLEND_WEIGHTS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    }

    return task;
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitQueueUpdateTimePos(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  NodeConnections* nodeConnections = net->getActiveNodesConnections(node->getNodeID()); // Get this nodes connections structure.

  if (nodeConnections->m_numActiveChildNodes == 2)
  {
    NodeID activeChildNodeID0 = nodeConnections->m_activeChildNodeIDs[0];
    NodeID activeChildNodeID1 = nodeConnections->m_activeChildNodeIDs[1];

    Task* task = queue->createNewTaskOnQueue(
                   CoreTaskIDs::MR_TASKID_TRANSITUPDATETIMEPOS,
                   node->getNodeID(),
                   net->nodeFindGeneratingNodeForSemantic(activeChildNodeID1, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, true, node->getNodeID()) != INVALID_NODE_ID ? 12 : 11,
                   dependentParameter);

    if (task)
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      NMP_ASSERT(currFrameNo > 0);
      NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());

      net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeParentNodeID, node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo);

      // Assuming we will not need to access this attribute outside of this frames network update.
      net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeChildNodeID0, currFrameNo);
      net->TaskAddOutputParamZeroLifespan(task, 2, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeChildNodeID1, currFrameNo);

      // Transition duration.
      net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());

      // Blend weight generated by this task.
      net->TaskAddOutputParamZeroLifespan(task, 4, ATTRIB_SEMANTIC_BLEND_WEIGHTS, ATTRIB_TYPE_BLEND_WEIGHTS, INVALID_NODE_ID, currFrameNo);

      // Last frames time pos.
      net->TaskAddOptionalNetInputParam(task, 5, ATTRIB_SEMANTIC_TIME_POS, node->getNodeID(), INVALID_NODE_ID, currFrameNo - 1);

      // current frames time pos.
      net->TaskAddOutputParam(task, 6, ATTRIB_SEMANTIC_TIME_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, TPARAM_FLAG_NONE, currFrameNo, 2);

      // Reverse transition CP.
      AttribDataTransitDef* attrDataDef = node->getAttribData<AttribDataTransitDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
      net->TaskAddOptionalInputCP(task, 7, ATTRIB_SEMANTIC_CP_BOOL, &(attrDataDef->m_reverseInputCPConnection));

      // Sources last frame event pos, for initialisation of destination in some cases.
      net->TaskAddOptionalNetInputParamRecurseFilterNodes(task, 8, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, activeChildNodeID0, INVALID_NODE_ID, currFrameNo - 1, ANIMATION_SET_ANY, true);

      net->TaskAddOutputReplaceParam(task, 9, ATTRIB_SEMANTIC_TRANSIT_COMPLETE, ATTRIB_TYPE_BOOL, INVALID_NODE_ID, currFrameNo);

      net->TaskAddOutputReplaceParam(task, 10, ATTRIB_SEMANTIC_TRANSIT_REVERSED, ATTRIB_TYPE_BOOL, INVALID_NODE_ID, currFrameNo);

      // Destinations current synchronisation buffer, for initialisation of destination in some cases.
      if (net->nodeFindGeneratingNodeForSemantic(activeChildNodeID1, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, true, node->getNodeID()) != INVALID_NODE_ID)
      {
        net->TaskAddParamAndDependency(task, 11, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeChildNodeID1, INVALID_NODE_ID, TPARAM_FLAG_INPUT | TPARAM_FLAG_OPTIONAL, currFrameNo);
      }
    }

    return task;
  }
  else
  {
    NodeID activeChildNodeID0 = nodeConnections->m_activeChildNodeIDs[0];

    Task* task = queue->createNewTaskOnQueue(
                   CoreTaskIDs::MR_TASKID_TRANSITDEADBLENDUPDATETIMEPOS,
                   node->getNodeID(),
                   net->nodeFindGeneratingNodeForSemantic(activeChildNodeID0, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, true, node->getNodeID()) != INVALID_NODE_ID ? 9 : 8,
                   dependentParameter);

    if (task)
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      NMP_ASSERT(currFrameNo > 0);
      NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());

      net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeParentNodeID, node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo);

      // Assuming we will not need to access this attribute outside of this frames network update.
      net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeChildNodeID0, currFrameNo);

      // Transition duration.
      net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());

      // Blend weight generated by this task.
      net->TaskAddOutputParamZeroLifespan(task, 3, ATTRIB_SEMANTIC_BLEND_WEIGHTS, ATTRIB_TYPE_BLEND_WEIGHTS, INVALID_NODE_ID, currFrameNo);

      // Last frames time pos.
      net->TaskAddOptionalNetInputParam(task, 4, ATTRIB_SEMANTIC_TIME_POS, node->getNodeID(), INVALID_NODE_ID, currFrameNo - 1);

      // current frames time pos.
      net->TaskAddOutputParam(task, 5, ATTRIB_SEMANTIC_TIME_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, TPARAM_FLAG_NONE, currFrameNo, 2);

      // Sources last frame event pos, for initialisation of destination in some cases.
      net->TaskAddOptionalNetInputParamRecurseFilterNodes(task, 6, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, activeChildNodeID0, INVALID_NODE_ID, currFrameNo - 1, ANIMATION_SET_ANY, true);

      net->TaskAddOutputReplaceParam(task, 7, ATTRIB_SEMANTIC_TRANSIT_COMPLETE, ATTRIB_TYPE_BOOL, INVALID_NODE_ID, currFrameNo);

      // Destinations current synchronisation buffer, for initialisation of destination in some cases.
      if (net->nodeFindGeneratingNodeForSemantic(activeChildNodeID0, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, true, node->getNodeID()) != INVALID_NODE_ID)
      {
        net->TaskAddParamAndDependency(task, 8, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT | TPARAM_FLAG_OPTIONAL, currFrameNo);
      }
    }

    return task;
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitQueueDeadBlendCacheState(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_DEADBLENDCACHESTATE,
                 node->getNodeID(),
                 9,
                 dependentParameter,
                 false, // External task
                 false); // SPU compatible
  if (task)
  {
    NodeID nodeID = node->getNodeID();
    uint32_t currFrameNo = net->getCurrentFrameNo();
    uint16_t activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    uint32_t netOutputFrameNo = (currFrameNo > 0 ? currFrameNo - 1 : currFrameNo);
    NodeID activeChildNodeID0 = net->getActiveChildNodeID(nodeID, 0);
    NodeID activeParentNodeID = net->getActiveParentNodeID(nodeID);

    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddNetInputParam(task, 2, ATTRIB_SEMANTIC_UPDATE_TIME_POS, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);
    net->TaskAddOptionalNetInputParam(task, 3, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, activeParentNodeID, INVALID_NODE_ID, netOutputFrameNo);
    net->TaskAddOptionalNetInputParam(task, 4, ATTRIB_SEMANTIC_TRANSFORM_BUFFER_T_MINUS_ONE, nodeID, nodeID, currFrameNo);

    net->TaskAddOutputReplaceParam(task, 5, ATTRIB_SEMANTIC_TRANSFORM_BUFFER_T_MINUS_TWO, ATTRIB_TYPE_TRANSFORM_BUFFER, nodeID, currFrameNo, activeAnimSetIndex);
    net->TaskAddOutputReplaceParam(task, 6, ATTRIB_SEMANTIC_TRANSFORM_BUFFER_T_MINUS_ONE, ATTRIB_TYPE_TRANSFORM_BUFFER, nodeID, currFrameNo, activeAnimSetIndex);
    net->TaskAddOutputReplaceParam(task, 7, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_T_MINUS_ONE, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, nodeID, currFrameNo, activeAnimSetIndex);
    net->TaskAddOutputReplaceParam(task, 8, ATTRIB_SEMANTIC_UPDATE_TIME_POS_T_MINUS_ONE, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, nodeID, currFrameNo, activeAnimSetIndex);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitQueueTransformBuffs(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  // We need to decide what type of blend to perform.
  if (isDeadBlending(node, net))
  {
    // Dead Blend
    return nodeQueueAnimationDeadBlendUpdateTransforms(
             node,
             queue,
             net,
             dependentParameter);
  }
  else if (isBlendingToPhysics(node, net))
  {
    AttribDataDeadBlendState* deadBlendState = getDeadBlendState(node, net);
    // We currently support the default deadblend only.
    NMP_ASSERT(deadBlendState);

    // Standard blend 2
    return nodeTransitQueueBlend2TransformBuffs(
             node,
             queue,
             net,
             dependentParameter,
             deadBlendState->m_blendMode);
  }
  else
  {
    // Look up for blending attributes in the transition definition.
    AttribDataHandle* attrib = net->getNetworkDef()->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    NMP_ASSERT(attrib && attrib->m_attribData);
    NMP_ASSERT(attrib->m_attribData->getType() == ATTRIB_TYPE_TRANSIT_DEF);

    AttribDataTransitDef* transitDef = (AttribDataTransitDef*)attrib->m_attribData;

    return nodeTransitQueueBlend2TransformBuffs(
             node,
             queue,
             net,
             dependentParameter,
             transitDef->m_blendMode);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitQueueTrajectoryDeltaAndTransformBuffs(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  // We need to decide what type of blend to perform.
  if (isDeadBlending(node, net))
  {
    // Dead Blend
    return nodeQueueAnimationDeadBlendUpdateTrajectoryDeltaAndTransforms(
      node,
      queue,
      net,
      dependentParameter);
  }
  else if (isBlendingToPhysics(node, net))
  {
    AttribDataDeadBlendState* deadBlendState = getDeadBlendState(node, net);
    // We currently support the default deadblend only.
    NMP_ASSERT(deadBlendState);

    // Standard blend 2
    return nodeTransitQueueBlend2TrajectoryDeltaAndTransforms(
      node,
      queue,
      net,
      dependentParameter,
      deadBlendState->m_slerpTrajectoryPosition,
      deadBlendState->m_blendMode);
  }
  else
  {
    // Look up for blending attributes in the transition definition.
    AttribDataHandle* attrib = net->getNetworkDef()->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    NMP_ASSERT(attrib && attrib->m_attribData);
    NMP_ASSERT(attrib->m_attribData->getType() == ATTRIB_TYPE_TRANSIT_DEF);

    AttribDataTransitDef* transitDef = (AttribDataTransitDef*)attrib->m_attribData;

    return nodeTransitQueueBlend2TrajectoryDeltaAndTransforms(
      node,
      queue,
      net,
      dependentParameter,
      transitDef->m_slerpTrajectoryPosition,
      transitDef->m_blendMode);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitQueueTransformBuffsPassThroughDestTrajDelta(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  // Physics networks are not supported using the combined trajectory delta and transforms semantic
  NMP_ASSERT(!isBlendingToPhysics(node, net));

  // We need to decide what type of blend to perform.
  if (isDeadBlending(node, net))
  {
    // Dead Blend
    return nodeQueueAnimationDeadBlendUpdateTrajectoryDeltaAndTransforms(
      node,
      queue,
      net,
      dependentParameter);
  }
  else
  {
    // Look up for blending attributes in the transition definition.
    AttribDataHandle* attrib = net->getNetworkDef()->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    NMP_ASSERT(attrib && attrib->m_attribData);
    NMP_ASSERT(attrib->m_attribData->getType() == ATTRIB_TYPE_TRANSIT_DEF);

    AttribDataTransitDef* transitDef = (AttribDataTransitDef*)attrib->m_attribData;

    return nodeTransitQueueBlend2TransformsPassDestTrajDelta(
      node,
      queue,
      net,
      dependentParameter,
      transitDef->m_blendMode);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitQueueTransformBuffsPassThroughSourceTrajDelta(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  // Physics networks are not supported using the combined trajectory delta and transforms semantic
  NMP_ASSERT(!isBlendingToPhysics(node, net));

  // We need to decide what type of blend to perform.
  if (isDeadBlending(node, net))
  {
    // Dead Blend
    return nodeQueueAnimationDeadBlendUpdateTrajectoryDeltaAndTransforms(
      node,
      queue,
      net,
      dependentParameter);
  }
  else
  {
    // Look up for blending attributes in the transition definition.
    AttribDataHandle* attrib = net->getNetworkDef()->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    NMP_ASSERT(attrib && attrib->m_attribData);
    NMP_ASSERT(attrib->m_attribData->getType() == ATTRIB_TYPE_TRANSIT_DEF);

    AttribDataTransitDef* transitDef = (AttribDataTransitDef*)attrib->m_attribData;

    return nodeTransitQueueBlend2TransformsPassSourceTrajDelta(
      node,
      queue,
      net,
      dependentParameter,
      transitDef->m_blendMode);
  }
}

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeQueueDeadBlendUpdateDeltaTrajectoryTransform(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_DEADBLENDTRAJECTORY,
                 node->getNodeID(),
                 6,
                 dependentParameter,
                 false,
                 false);
  if (task)
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);
    NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());

    // Transit node specific data.
    net->TaskAddNetInputParam(task, 0, ATTRIB_SEMANTIC_DEAD_BLEND_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER);

    // Dead blend trajectory rate.
    net->TaskAddNetInputParam(task, 1, ATTRIB_SEMANTIC_TRANSFORM_RATES, node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME, activeAnimSetIndex);

    // Use the delta time to compute the dead blend extrapolation.
    net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeParentNodeID, node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo);

    // Destination node delta traj transform to blend with.
    NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    // This node needs to generate a weighting value.
    net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_BLEND_WEIGHTS, ATTRIB_TYPE_BLEND_WEIGHTS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    net->TaskAddOutputParamZeroLifespan(task, 5, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, INVALID_NODE_ID, currFrameNo);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitQueueTrajectoryDeltaTransformTask(
  NodeDef*            node,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter,
  TaskID              taskID,
  AttribDataSemantic  trajectorySemantic,
  AttribDataType      attribType)
{
  Task* task = queue->createNewTaskOnQueue(
                 taskID,
                 node->getNodeID(),
                 4,
                 dependentParameter);
  if (task)
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    NMP_ASSERT(currFrameNo > 0);
    NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);
    NodeID activeChildNodeID1 = net->getActiveChildNodeID(node->getNodeID(), 1);

    net->TaskAddOutputParamZeroLifespan(task, 0, trajectorySemantic, attribType, INVALID_NODE_ID, currFrameNo);
    net->TaskAddParamAndDependency(task, 1, trajectorySemantic, attribType, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddParamAndDependency(task, 2, trajectorySemantic, attribType, activeChildNodeID1, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    // This node needs to generate a weighting value.
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_BLEND_WEIGHTS, ATTRIB_TYPE_BLEND_WEIGHTS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* transitionQueueTrajectoryDeltaAddAttSlerpPos(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  if (isDeadBlendingOrBlendingToPhysics(node, net))
  {
    return transitionQueuePhysicsTrajectoryDeltaUpdate(
             node,
             queue,
             net,
             dependentParameter,
             CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSADDATTSLERPPOS);
  }
  else
  {
    return nodeTransitQueueTrajectoryDeltaTransformTask(
             node,
             queue,
             net,
             dependentParameter,
             CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSADDATTSLERPPOS,
             ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
             ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* transitionQueueTrajectoryDeltaInterpAttSlerpPos(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  if (isDeadBlendingOrBlendingToPhysics(node, net))
  {
    return transitionQueuePhysicsTrajectoryDeltaUpdate(
             node,
             queue,
             net,
             dependentParameter,
             CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS);
  }
  else
  {
    return nodeTransitQueueTrajectoryDeltaTransformTask(
             node,
             queue,
             net,
             dependentParameter,
             CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS,
             ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
             ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* transitionQueueTrajectoryDeltaAddAttAddPos(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  if (isDeadBlendingOrBlendingToPhysics(node, net))
  {
    return transitionQueuePhysicsTrajectoryDeltaUpdate(
             node,
             queue,
             net,
             dependentParameter,
             CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSADDATTADDPOS);
  }
  else
  {
    return nodeTransitQueueTrajectoryDeltaTransformTask(
             node,
             queue,
             net,
             dependentParameter,
             CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSADDATTADDPOS,
             ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
             ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* transitionQueueTrajectoryDeltaAddAttInterpPos(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  if (isDeadBlendingOrBlendingToPhysics(node, net))
  {
    return transitionQueuePhysicsTrajectoryDeltaUpdate(
             node,
             queue,
             net,
             dependentParameter,
             CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSADDATTINTERPPOS);
  }
  else
  {
    return nodeTransitQueueTrajectoryDeltaTransformTask(
             node,
             queue,
             net,
             dependentParameter,
             CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSADDATTINTERPPOS,
             ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
             ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* transitionQueueTrajectoryDeltaInterpAttAddPos(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  if (isDeadBlendingOrBlendingToPhysics(node, net))
  {
    return transitionQueuePhysicsTrajectoryDeltaUpdate(
             node,
             queue,
             net,
             dependentParameter,
             CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTADDPOS);
  }
  else
  {
    return nodeTransitQueueTrajectoryDeltaTransformTask(
             node,
             queue,
             net,
             dependentParameter,
             CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTADDPOS,
             ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
             ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* transitionQueueTrajectoryDeltaInterpAttInterpPos(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  if (isDeadBlendingOrBlendingToPhysics(node, net))
  {
    return transitionQueuePhysicsTrajectoryDeltaUpdate(
             node,
             queue,
             net,
             dependentParameter,
             CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS);
  }
  else
  {
    return nodeTransitQueueTrajectoryDeltaTransformTask(
             node,
             queue,
             net,
             dependentParameter,
             CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS,
             ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
             ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* queuePassThroughChild0IfNotPhysics(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  if (isDeadBlendingOrBlendingToPhysics(node, net))
  {
    return transitionQueuePhysicsTrajectoryDeltaUpdate(
             node,
             queue,
             net,
             dependentParameter,
             CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS);
  }
  else
  {
    return queuePassThroughChild0(
             node,
             queue,
             net,
             dependentParameter);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* queuePassThroughChild1IfNotPhysics(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  if (isDeadBlendingOrBlendingToPhysics(node, net))
  {
    return transitionQueuePhysicsTrajectoryDeltaUpdate(
             node,
             queue,
             net,
             dependentParameter,
             CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS);
  }
  else
  {
    return queuePassThroughChild1(
             node,
             queue,
             net,
             dependentParameter);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* transitionQueuePhysicsTrajectoryDeltaUpdate(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter,
  TaskID         taskIDIfAnimRootBlending)
{
  // We need to decide what type of blend to perform.
  if (isDeadBlending(node, net))
  {
    // Determine what type of physics node we are transitioning from and to.
    uint32_t destRootControlMethod = getNodeRootControlMethod(node, net);
    if (isBlendingToPhysics(node, net) && destRootControlMethod == Network::ROOT_CONTROL_PHYSICS)
      // Make the destination node in charge for the delta trajectory transform.
      return queuePassThroughChild0(
               node,
               queue,
               net,
               dependentParameter);
    else
    {
      // Perform a dead blend, because the destination is animation or physically driven animation.
      return nodeQueueDeadBlendUpdateDeltaTrajectoryTransform(
               node,
               queue,
               net,
               dependentParameter);
    }
  }
  else if (isBlendingToPhysics(node, net))
  {
    AttribDataDeadBlendDef* deadBlendDef = node->getAttribData<AttribDataDeadBlendDef>(ATTRIB_SEMANTIC_DEAD_BLEND_DEF);
    bool useBlendToPhysics = deadBlendDef->m_blendToDestinationPhysicsBones;
    uint32_t destRootControlMethod = getNodeRootControlMethod(node, net);

    // Blend to dest trajectory only if the dest is not pure physical
    // of if specifically requested.
    if (destRootControlMethod != Network::ROOT_CONTROL_PHYSICS || useBlendToPhysics)
    {
      // Standard blend 2
      return nodeTransitQueueTrajectoryDeltaTransformTask(
               node,
               queue,
               net,
               dependentParameter,
               taskIDIfAnimRootBlending,
               ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
               ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
    }
    else
    {
      // The destination is a pure physics node (ragdoll, AA, ...), just make it in charge of the trajectory
      // Make the destination node in charge for the delta trajectory transform.
      return queuePassThroughChild1(
               node,
               queue,
               net,
               dependentParameter);
    }
  }
  else
  {
    // This case should never happen
    NMP_ASSERT_FAIL();
    return NULL;
  }
}
  
//----------------------------------------------------------------------------------------------------------------------  
namespace
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief The transition is finished when it puts a TransitComplete attribute on the Network attrib list or if
/// the duration is 0 on the first frame of update.
/// \ingroup
bool transitionFinished(
  NodeDef*              node,
  Network*              net)
{
  // The transition is complete when it puts a TransitComplete attribute on the Network attrib list or if
  // the duration is 0 on the first frame of update.
  NodeBinEntry* networkEntry = net->getAttribDataNodeBinEntry(
                                 ATTRIB_SEMANTIC_TRANSIT_COMPLETE,
                                 node->getNodeID(),
                                 INVALID_NODE_ID,
                                 net->getCurrentFrameNo() - 1);
  return ((networkEntry && networkEntry->getAttribData<AttribDataBool>()->m_value == true));
}

//----------------------------------------------------------------------------------------------------------------------
void initTransition(
  Network*              net,
  AttribDataTransitDef* transitDef,
  NodeConnections*      nodeConnections)
{
  if (nodeConnections->m_justBecameActive &&
      (nodeConnections->m_numActiveChildNodes == 2))
  {
    // This transition has just become active, initialise an AttribDataPlaybackPosInit structure for the destination.
    NodeID childNodeID1 = nodeConnections->m_activeChildNodeIDs[1];
    AttribAddress address(ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, childNodeID1, INVALID_NODE_ID);
    AttribDataPlaybackPosInit* destAdjustStartSyncEventAttrib;
    FrameCount prevFrame = net->getCurrentFrameNo() - 1;
    bool initWithDurationFraction;
    float startValue;
    NodeID childNodeID0 = nodeConnections->m_activeChildNodeIDs[0];

    if (childNodeID0 == childNodeID1)
    {
      net->deleteNodeInstance(childNodeID0);
    }

    AttribDataSyncEventTrack* destSyncEventTrack =
      (AttribDataSyncEventTrack*) net->getAttribDataRecurseFilterNodes(
        ATTRIB_SEMANTIC_SYNC_EVENT_TRACK,
        childNodeID1,
        INVALID_NODE_ID,
        prevFrame);


    if (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_USE_START_DURATION_FRACTION)
    {
      initWithDurationFraction = true;
      startValue = transitDef->m_destinationStartFraction;
    }
    else
    {
      float startSyncEvent = transitDef->m_destinationStartSyncEvent;

      // Get the source nodes current event index.
      AttribDataUpdateSyncEventPlaybackPos* sourceSyncEventPlaybackPos =
        (AttribDataUpdateSyncEventPlaybackPos*) net->getAttribDataRecurseFilterNodes(
          ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS,
          childNodeID0,
          INVALID_NODE_ID,
          prevFrame);
      if (sourceSyncEventPlaybackPos)
      {
        if (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_SYNC_ON_EVENT_INDEX_AND_SYNC_ON_EVENT_FRACTION)
          startSyncEvent = sourceSyncEventPlaybackPos->m_absPosReal.get();
        else if (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_USE_EVENT_INDEX_AND_SYNC_ON_EVENT_FRACTION)
          startSyncEvent += sourceSyncEventPlaybackPos->m_absPosReal.fraction();
        else if (transitDef->m_destinationInitMethod == AttribDataTransitDef::INIT_DEST_SYNC_ON_EVENT_INDEX_AND_USE_EVENT_FRACTION)
          startSyncEvent += sourceSyncEventPlaybackPos->m_absPosReal.index();
      }

      if (destSyncEventTrack)
      {
         // MORPH-21352: Come up with a more robust way to ensure that the real value resolves to the beginning or end of the track in the task later.
        if(startSyncEvent == (float) destSyncEventTrack->m_syncEventTrack.getNumEvents())
        {
          NMP_ASSERT(destSyncEventTrack->m_syncEventTrack.getNumEvents() > 0);
          startSyncEvent -= 0.00001f;
        }
        
        // Make sure that the sync event pos lies within the sync event space of the destination.
        destSyncEventTrack->m_syncEventTrack.limitToSyncEventSpace(startSyncEvent, true);
        startSyncEvent = destSyncEventTrack->m_syncEventTrack.convEventInRealSpaceToAdjustedSpace(startSyncEvent);
      }

      initWithDurationFraction = false;
      startValue = startSyncEvent;
    }

    // Attribute already exists?
    destAdjustStartSyncEventAttrib = net->getOptionalAttribData<AttribDataPlaybackPosInit>(address);
    if (!destAdjustStartSyncEventAttrib)
    {
      // Create a new attrib for the destination.
      AttribDataHandle handle = AttribDataPlaybackPosInit::create(
                                  net->getPersistentMemoryAllocator(),
                                  initWithDurationFraction,
                                  startValue);

      // Add the new attribute to our destination child.
      net->addAttribData(address, handle, LIFESPAN_FOREVER);
    }
    else
    {
      // Overwrite existing ATTRIB_SEMANTIC_PLAYBACK_POS_INIT parameters.
      // Occurs on a second wildcard / global self-transition.
      destAdjustStartSyncEventAttrib->m_initWithDurationFraction = initWithDurationFraction;
      destAdjustStartSyncEventAttrib->m_startValue               = startValue;
      destAdjustStartSyncEventAttrib->m_adjustStartEventIndex    = 0;
    }
  }
}
 
//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
// The transition has completed for whatever reason. Destroy the transitions own node data and any source node data.
// Return the NodeID of the remaining active child node.
NodeID nodeTransitCompleteTransition(
  NodeDef*           node,
  Network*           net,
  NodeConnections*   nodeConnections,
  bool               reversed)
{
  NodeID thisNodeID = node->getNodeID();
  NodeID resultingNodeID = INVALID_NODE_ID;
  NodeID childNodeID0 = nodeConnections->m_activeChildNodeIDs[0];
  NodeID childNodeID1 = INVALID_NODE_ID;
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Transition completed.
  if ((nodeConnections->m_numActiveChildNodes == 2) &&
      (childNodeID0 != (childNodeID1 = nodeConnections->m_activeChildNodeIDs[1])))
  {
    if (reversed)
    {
      // Transition has completed in reverse.
      bool deleteChild = false;

      if (net->getActiveParentNodeID(childNodeID1) == thisNodeID)
      {
        // We are clear to delete the child, but we defer until the destination has been created.
        deleteChild = true;
      }

      // Return the source as the resulting state of the transition.
      // Only need to update the connections of the destination as the transition is now dead.
      resultingNodeID = net->updateNodeInstanceConnections(childNodeID0, animSet);

      // Recursively delete source attrib data (child we are transitioning from).
      if (deleteChild)
      {
        // This transition still owns the child tree (no active breakout), so can safely delete
        net->deleteNodeInstance(childNodeID1);
      }
    }
    else
    {
      bool deleteChild = false;

      if (net->getActiveParentNodeID(childNodeID0) == thisNodeID)
      {
        // We are clear to delete the child, but we defer until the destination has been created.
        deleteChild = true;
      }

      // Return the destination as the resulting state of the transition.
      // Only need to update the connections of the destination as the transition is now dead.
      resultingNodeID = net->updateNodeInstanceConnections(childNodeID1, animSet);

      // Recursively delete source attrib data (child we are transitioning from).
      if (deleteChild)
      {
        // This transition still owns the child tree (no active breakout), so can safely delete
        net->deleteNodeInstance(childNodeID0);
      }
    }
  }
  else
  {
    // Don't try to destroy the source node, because this is either:
    //  - A dead blended transition so that node has already been destroyed.
    //  - An immediate (zero duration) self-transition.
    resultingNodeID = net->updateNodeInstanceConnections(childNodeID0, animSet);
  }

  // Delete this transitions own attrib data.
  net->cleanNodeData(thisNodeID);

  return resultingNodeID;
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeTransitUpdateConnections(
  NodeDef*           node,
  Network*           net)
{
  NodeID thisNodeID = node->getNodeID();
  NodeID resultingNodeID = thisNodeID;
  NodeConnections* nodeConnections = net->getActiveNodesConnections(thisNodeID); // Get this node's connections structure.
  NMP_ASSERT(net->getCurrentFrameNo() > 0);

  // Get the transit def attrib data.
  AttribDataTransitDef* transitDef = node->getAttribData<AttribDataTransitDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  
  // If this is the first frame initialise the transition.
  initTransition(net, transitDef, nodeConnections);

  // If we have a transition into a sub state
  if (transitDef->m_nodeInitData && nodeConnections->m_justBecameActive)
  {
    NodeInitDataArrayDef* destInitData = transitDef->m_nodeInitData;
    for (uint32_t i = 0; i < destInitData->m_numNodeInitDatas; ++i)
    {
      // MORPH-21356: We only deal with init of state machine start state at the moment.
      NMP_ASSERT(destInitData->m_nodeInitDataArray[i]->getType() == NODE_INIT_DATA_TYPE_STATE_MACHINE);
      NodeID targetSMNodeID = destInitData->m_nodeInitDataArray[i]->getTargetNodeID();
      StateMachineInitData* smInitData = (StateMachineInitData*) destInitData->m_nodeInitDataArray[i];
      NodeID startingStateNodeID = smInitData->getInitialSubStateID();

      net->queueSetStateMachineStateByNodeID(targetSMNodeID, startingStateNodeID);
    }
  }

  //---------------
  if (transitionFinished(node, net))
  {
    NodeBinEntry* networkEntry = net->getAttribDataNodeBinEntry(
                                   ATTRIB_SEMANTIC_TRANSIT_REVERSED,
                                   thisNodeID,
                                   INVALID_NODE_ID,
                                   net->getCurrentFrameNo() - 1);
    bool reversed = networkEntry && networkEntry->getAttribData<AttribDataBool>()->m_value == true;

    // Transition has completed.
    resultingNodeID = nodeTransitCompleteTransition(node, net, nodeConnections, reversed);

    // Reset the control parameter to prevent reversing multiple times.
    if (reversed)
    {
      AttribDataBool falseAttrib;
      falseAttrib.m_value = false;
      net->setControlParameter(
                  transitDef->m_reverseInputCPConnection.m_sourceNodeID,
                  &falseAttrib,
                  transitDef->m_reverseInputCPConnection.m_sourcePinIndex);
    }
  }
  else
  {
    // Transition not complete.
    updateTransition(node, net, transitDef, nodeConnections);
  }

  NodeBinEntry* deadBlendTransformsStateEntry = net->getAttribDataNodeBinEntry(
    ATTRIB_SEMANTIC_DEAD_BLEND_TRANSFORMS_STATE,
    node->getNodeID(),
    INVALID_NODE_ID,
    VALID_FRAME_ANY_FRAME);
  if (deadBlendTransformsStateEntry)
  {
    NMP_ASSERT(deadBlendTransformsStateEntry->m_address.m_animSetIndex != ANIMATION_SET_ANY);
    AnimSetIndex animSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    if (deadBlendTransformsStateEntry->m_address.m_animSetIndex != animSetIndex)
    {
      NodeBinEntry* deadBlendTransformsRateEntry = net->getAttribDataNodeBinEntry(
        ATTRIB_SEMANTIC_TRANSFORM_RATES,
        node->getNodeID(),
        INVALID_NODE_ID,
        VALID_FRAME_ANY_FRAME);
      NMP_ASSERT(deadBlendTransformsRateEntry);
      NMP_ASSERT(deadBlendTransformsStateEntry->m_address.m_animSetIndex == deadBlendTransformsRateEntry->m_address.m_animSetIndex);

      // The animation set has changed. Create a new one and map the old one to it.
      AnimRigDef* targetRigDef = net->getRig(animSetIndex);

      {
        NMP::Memory::Format internalBuffMemReqs;
        NMP::Memory::Format buffMemReqs;
        uint32_t numRigJoints = targetRigDef->getNumBones();
        AttribDataTransformBuffer::getPosQuatMemoryRequirements(numRigJoints, buffMemReqs, internalBuffMemReqs);
        AttribDataTransformBuffer* targetTransformsAttr = AttribDataTransformBuffer::createPosQuat(deadBlendTransformsStateEntry->m_allocatorUsed, buffMemReqs, internalBuffMemReqs, numRigJoints, 1);
        AttribDataTransformBuffer* transformsAttr = (AttribDataTransformBuffer*)deadBlendTransformsStateEntry->getAttribData();
        net->getNetworkDef()->mapCopyTransformBuffers(transformsAttr->m_transformBuffer, deadBlendTransformsStateEntry->m_address.m_animSetIndex, targetTransformsAttr->m_transformBuffer, animSetIndex);

        deadBlendTransformsStateEntry->removeReference();
        deadBlendTransformsStateEntry->m_address.m_animSetIndex = animSetIndex;
        deadBlendTransformsStateEntry->m_attribDataHandle.m_attribData = targetTransformsAttr;
        deadBlendTransformsStateEntry->m_attribDataHandle.m_format = buffMemReqs;
      }

      {
        NMP::Memory::Format internalBuffMemReqs;
        NMP::Memory::Format buffMemReqs;
        uint32_t numRigJoints = targetRigDef->getNumBones();
        AttribDataTransformBuffer::getPosQuatMemoryRequirements(numRigJoints, buffMemReqs, internalBuffMemReqs);
        AttribDataTransformBuffer* targetTransformsAttr = AttribDataTransformBuffer::createPosVelAngVel(deadBlendTransformsRateEntry->m_allocatorUsed, buffMemReqs, internalBuffMemReqs, numRigJoints, 1);
        AttribDataTransformBuffer* transformsAttr = (AttribDataTransformBuffer*)deadBlendTransformsRateEntry->getAttribData();
        net->getNetworkDef()->mapCopyVelocityBuffers(transformsAttr->m_transformBuffer, deadBlendTransformsRateEntry->m_address.m_animSetIndex, targetTransformsAttr->m_transformBuffer, animSetIndex);

        deadBlendTransformsRateEntry->removeReference();
        deadBlendTransformsRateEntry->m_address.m_animSetIndex = animSetIndex;
        deadBlendTransformsRateEntry->m_attribDataHandle.m_attribData = targetTransformsAttr;
        deadBlendTransformsRateEntry->m_attribDataHandle.m_format = buffMemReqs;
      }
    }
  }

  return resultingNodeID;
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeTransitFindGeneratingNodeForSemantic(
  NodeID              NMP_UNUSED(callingNodeID),
  bool                NMP_UNUSED(fromParent),   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            NMP_UNUSED(network))
{
  NodeID result = INVALID_NODE_ID;

  switch (semantic)
  {
  case ATTRIB_SEMANTIC_UPDATE_TIME_POS:
  case ATTRIB_SEMANTIC_TRANSFORM_BUFFER:
  case ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER:
  case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM:
  case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER:
  case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_T_MINUS_ONE:
  case ATTRIB_SEMANTIC_BLEND_WEIGHTS:
    result = node->getNodeID();
    break;
  default:
    break;
  }

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
