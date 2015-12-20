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
#include "morpheme/Nodes/mrNodeBasicUnevenTerrain.h"
#include "morpheme/mrUnevenTerrainUtilities.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Queues for calculating the uneven terrain IK setup
Task* nodeBasicUnevenTerrainQueueIKSetupTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  //---------------------------
  // Information
  uint16_t activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
  NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
  uint32_t currFrameNo = net->getCurrentFrameNo();
  NMP_ASSERT(currFrameNo > 0);

  // Compute the uneven terrain setup information.
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_BASIC_UNEVEN_TERRAIN_IK_SETUP,
    node->getNodeID(),
    8,
    dependentParameter);

  if (task)
  {
    // Uneven terrain node definition setup data
    net->TaskAddDefInputParam(task, 0, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    // IK chain information
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, node->getNodeID(), activeAnimSetIndex);

    // Character controller
    net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_UPDATED, ATTRIB_TYPE_BOOL, NETWORK_NODE_ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo); 
    net->TaskAddNetInputParam(task, 3, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);

    // Animation rig
    net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    // Input transforms buffer
    net->TaskAddParamAndDependency(task, 5, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Uneven terrain output IK setup information
    net->TaskAddOutputReplaceParam(task, 6, ATTRIB_SEMANTIC_UNEVEN_TERRAIN_IK_SETUP, ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_IK_SETUP, INVALID_NODE_ID, currFrameNo);

    // Trajectory delta transform
    net->TaskAddParamAndDependency(task, 7, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// Queues for computing the foot lifting target transforms. This task is executed post
// character controller update
Task* nodeBasicUnevenTerrainQueueFootLiftingTargets(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  //---------------------------
  // Information
  uint16_t activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
  uint32_t currFrameNo = net->getCurrentFrameNo();
  NMP_ASSERT(currFrameNo > 0);

  //---------------------------
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_BASIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET,
    node->getNodeID(),
    5,
    dependentParameter,
    false,  // externalTask
    false); // supportsRemoteEval (Must perform physics ray cast on main processor)

  if (task)
  {
    // Uneven terrain node definition setup data
    net->TaskAddDefInputParam(task, 0, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    // IK chain information
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, node->getNodeID(), activeAnimSetIndex);
    // Uneven terrain input IK setup information
    net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_UNEVEN_TERRAIN_IK_SETUP, ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_IK_SETUP, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    // Character controller updated flag
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_UPDATED, ATTRIB_TYPE_BOOL, NETWORK_NODE_ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Output foot lifting target information
    net->TaskAddOutputReplaceParam(task, 4, ATTRIB_SEMANTIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET, ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET, INVALID_NODE_ID, currFrameNo);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// Queues for computing the transform buffer. This applies leg IK to adapt to the uneven terrain.
// This task is executed post character controller update.
NM_INLINE Task* nodeBasicUnevenTerrainQueue(
  NodeDef*            node,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter,
  TaskID              taskID,
  AttribDataSemantic  transformSemantic,
  AttribDataType      attribType)
{
  // Information
  uint16_t activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
  uint32_t currFrameNo = net->getCurrentFrameNo();
  NMP_ASSERT(currFrameNo > 0);
  NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);

#ifdef TEST_UT_NODE_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\n---------------------------\n");
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "currFrameNo = %d\n", currFrameNo);
#endif

  // Queue the transforms task
  Task* task = queue->createNewTaskOnQueue(
    taskID,
    node->getNodeID(),
    12,
    dependentParameter);

  if (task)
  {
    // Network update time
    net->TaskAddNetInputParam(task, 0, ATTRIB_SEMANTIC_UPDATE_TIME_POS, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);
    // Input transforms buffer
    net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    // Output transforms buffer
    net->TaskAddOutputParamZeroLifespan(task, 2, transformSemantic, attribType, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    // Animation rig
    net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    // Character controller updated
    net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_UPDATED, ATTRIB_TYPE_BOOL, NETWORK_NODE_ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Uneven terrain node definition setup data
    net->TaskAddDefInputParam(task, 5, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    // IK chain information
    net->TaskAddDefInputParam(task, 6, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, node->getNodeID(), activeAnimSetIndex);
    // Uneven terrain input IK setup information
    net->TaskAddParamAndDependency(task, 7, ATTRIB_SEMANTIC_UNEVEN_TERRAIN_IK_SETUP, ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_IK_SETUP, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    // Foot lifting target information
    net->TaskAddParamAndDependency(task, 8, ATTRIB_SEMANTIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET, ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Uneven terrain input IK state (may not exist)
    net->TaskAddOptionalNetInputOutputParam(task, 9, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_IK_STATE, INVALID_NODE_ID, currFrameNo);
    // IkHipsWeight
    net->TaskAddInputCP(task, 10, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(0));
    // IkFkBlendWeight
    net->TaskAddInputCP(task, 11, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(1));
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBasicUnevenTerrainQueueTransformBuffs(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeBasicUnevenTerrainQueue(
    node,
    queue,
    net,
    dependentParameter,
    CoreTaskIDs::MR_TASKID_BASIC_UNEVEN_TERRAIN_TRANSFORMS,
    ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
