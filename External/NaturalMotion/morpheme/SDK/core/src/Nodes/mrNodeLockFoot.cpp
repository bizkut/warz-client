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
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/mrCharacterControllerAttribData.h"

#include "morpheme/Nodes/mrNodeLockFoot.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeLockFootUpdateConnections(
  NodeDef*           node,
  Network*           net)
{
  // Update the only connected control parameter.
  NMP_ASSERT(node->getNumInputCPConnections() == 2);
  AnimSetIndex animSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

  net->updateInputCPConnection(node->getInputCPConnection(0), animSetIndex);
  net->updateInputCPConnection(node->getInputCPConnection(1), animSetIndex);

  // Some useful info
  FrameCount currentFrameNo = net->getCurrentFrameNo();
  NMP_ASSERT(currentFrameNo > 0);
  NetworkDef* networkDef = net->getNetworkDef();

  //----------------------------------
  // Track the character's worldspace location via the character controller

  // Retrieve the lock foot state attrib data from the previous frame, if it exists.  We are
  // going to modify the trajectory transform stored in this attrib data to correct it for the
  // known motion of the character controller.  We cannot do this inside the foot lock task
  // because the character controller's trajectory transform is not available on SPU.
  // MORPH-21347: Fix up the character controller attrib data so this work-around is not necessary.
  AttribDataLockFootState* lockFootStateAttrib = net->getOptionalAttribData<AttribDataLockFootState>(
    MR::AttribAddress(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
    node->getNodeID(),
    INVALID_NODE_ID,
    currentFrameNo - 1,
    ANIMATION_SET_ANY));

  // Get the lock foot setup attrib data - this informs us whether we are using the character controller.
  const AttribDataLockFootSetup* lockFootSetupAttrib = networkDef->getAttribData<AttribDataLockFootSetup>(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,
    node->getNodeID());
  NMP_ASSERT(lockFootSetupAttrib);

  // The lock foot state attrib data is created by the transforms task, so it won't exist if the
  // node has only just become active.  In this case, it is okay to do nothing since the lock foot
  // transforms task handles this properly.
  if (lockFootStateAttrib && lockFootSetupAttrib->m_trackCharacterController)
  {
    // Get the character controller
    const AttribDataCharacterProperties* characterControllerAttrib =
      net->getAttribData<AttribDataCharacterProperties>(
        ATTRIB_SEMANTIC_CHARACTER_PROPERTIES,
        NETWORK_NODE_ID,
        ANIMATION_SET_ANY);
    NMP_ASSERT(characterControllerAttrib);

    // Copy the previous frame's trajectory transform into the lock foot state.  The transforms tasks
    // handles updating for trajectory deltas on this frame.
    AttribDataTransform& trajectoryTransform = lockFootStateAttrib->m_trajectoryTransform;
    trajectoryTransform.m_pos = characterControllerAttrib->m_prevWorldRootTransform.translation();
    trajectoryTransform.m_att = characterControllerAttrib->m_prevWorldRootTransform.toQuat();
  }

  //----------------------------------

  // Recurse to child.
  NMP_ASSERT(net->getNumActiveChildren(node->getNodeID()) == 1);
  NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
  net->updateNodeInstanceConnections(activeChildNodeID, animSetIndex);

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeLockFootQueueLockFootTransformBuffs(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NodeID nodeID = node->getNodeID();
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_LOCKFOOTTRANSFORMS,
                 nodeID,
                 11,
                 dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);

    NodeID activeChildNodeID = net->getActiveChildNodeID(nodeID, 0);
    net->TaskAddParamAndDependency(
      task, 0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID,
      TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddParamAndDependency(
      task, 1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, activeChildNodeID, INVALID_NODE_ID,
      TPARAM_FLAG_INPUT, currFrameNo);

    // IkFkBlendWeight
    net->TaskAddInputCP(task, 2, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(0));
    
    // SwivelContributionToOrientation
    net->TaskAddInputCP(task, 3, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(1));
    
    // Lock Foot const setup
    net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, nodeID);
    // Lock Foot chain setup - anim set dependent
    net->TaskAddDefInputParam(task, 5, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, nodeID, activeAnimSetIndex);

    net->TaskAddDefInputParam(task, 6, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddParamAndDependency(
      task, 7, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, activeChildNodeID, nodeID,
      TPARAM_FLAG_INPUT, currFrameNo);
    NodeID activeParentNodeID = net->getActiveParentNodeID(nodeID);
    net->TaskAddParamAndDependency(
      task, 8, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeParentNodeID, nodeID,
      TPARAM_FLAG_INPUT, currFrameNo);

    // add params for the node state
    net->TaskAddOptionalNetInputOutputParam(
      task, 9, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, ATTRIB_TYPE_LOCK_FOOT_STATE, INVALID_NODE_ID, currFrameNo);

    // The output transforms
    net->TaskAddOutputParamZeroLifespan(task, 10, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeLockFootQueueLockFootTrajectoryDeltaAndTransformsBuffs(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter)
{
  NodeID nodeID = node->getNodeID();
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_LOCKFOOTTRAJECTORY_DELTA_AND_TRANSFORMS,
    nodeID,
    10,
    dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);

    NodeID activeChildNodeID = net->getActiveChildNodeID(nodeID, 0);
    net->TaskAddParamAndDependency(
      task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID,
      TPARAM_FLAG_INPUT, currFrameNo);

    // IkFkBlendWeight
    net->TaskAddInputCP(task, 1, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(0));

    // SwivelContributionToOrientation
    net->TaskAddInputCP(task, 2, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(1));

    // Lock Foot const setup
    net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, nodeID);
    // Lock Foot chain setup - anim set dependent
    net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, nodeID, activeAnimSetIndex);

    net->TaskAddDefInputParam(task, 5, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddParamAndDependency(
      task, 6, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, activeChildNodeID, nodeID,
      TPARAM_FLAG_INPUT, currFrameNo);
    NodeID activeParentNodeID = net->getActiveParentNodeID(nodeID);
    net->TaskAddParamAndDependency(
      task, 7, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeParentNodeID, nodeID,
      TPARAM_FLAG_INPUT, currFrameNo);

    // add params for the node state
    net->TaskAddOptionalNetInputOutputParam(
      task, 8, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, ATTRIB_TYPE_LOCK_FOOT_STATE, INVALID_NODE_ID, currFrameNo);

    // The output transforms
    net->TaskAddOutputParamZeroLifespan(task, 9, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  }

  return task;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
