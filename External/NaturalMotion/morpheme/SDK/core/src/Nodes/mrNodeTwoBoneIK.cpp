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
#include "morpheme/Nodes/mrNodeTwoBoneIK.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Update of any node data that must happen before queue building or execution.
NodeID nodeTwoBoneIKUpdateConnections(
  NodeDef*           node,
  Network*           net)
{
  NMP_ASSERT(net->getCurrentFrameNo() > 0);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // EffectorTarget
  net->updateInputCPConnection<AttribDataVector3>(node->getInputCPConnection(0), animSet);
  
  // TargetOrientation (Optional - runtime defined)
  net->updateOptionalInputCPConnection<AttribDataVector4>(node->getInputCPConnection(1), animSet);

  // SwivelAngle
  net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(2), animSet);

  // IkFkBlendWeight
  net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(3), animSet);

  // SwivelContributionToOrientation
  net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(4), animSet);

  // Recurse to children.
  NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
  net->updateNodeInstanceConnections(activeChildNodeID, animSet);

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTwoBoneIKQueueTwoBoneIKSetup(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  // PPU-only on PS3 (because of the character controller being used) so does not support remote
  // evaluation (last argument).
  Task* task = queue->createNewTaskOnQueue(
                 MR::CoreTaskIDs::MR_TASKID_TWOBONEIKSETUP,
                 node->getNodeID(),
                 2,
                 dependentParameter,
                 false,
                 false);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    NMP_ASSERT(currFrameNo > 0);

    // The character controller, for determining the world-space location of the character
    net->TaskAddNetInputParam(
      task, 0, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, NETWORK_NODE_ID,
      INVALID_NODE_ID, currFrameNo);

    // The previous frame's world space trajectory transform, extracted from the character controller
    net->TaskAddOutputParamZeroLifespan(
      task, 1, ATTRIB_SEMANTIC_TRAJECTORY_TRANSFORM,
      ATTRIB_TYPE_TRANSFORM, INVALID_NODE_ID, currFrameNo);
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Task* nodeTwoBoneIKQueueTwoBoneIKTransforms(
  NodeDef*            node,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter,
  TaskID              taskID,
  AttribDataSemantic  transformSemantic,
  AttribDataType      attribType,
  bool                queueTrajectoryDelta)
{
  uint32_t taskParamCount;
  if (queueTrajectoryDelta)
  {
    taskParamCount = 12;
  }
  else
  {
    taskParamCount = 11;
  }

  Task* task = queue->createNewTaskOnQueue(
    taskID,
    node->getNodeID(),
    taskParamCount,
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

    uint32_t taskIndex = 0;

    NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
    net->TaskAddParamAndDependency(
      task, taskIndex++, transformSemantic, attribType, activeChildNodeID,
      INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    if (queueTrajectoryDelta)
    {
      // Trajectory delta computed so far for this frame
      net->TaskAddParamAndDependency(
        task, taskIndex++, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, activeChildNodeID,
        INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    }

    // control parameters

    // EffectorTarget
    net->TaskAddInputCP(task, taskIndex++, ATTRIB_SEMANTIC_CP_VECTOR3, node->getInputCPConnection(0));

    // TargetOrientation (Optional - runtime defined)
    net->TaskAddOptionalInputCP(task, taskIndex++, ATTRIB_SEMANTIC_CP_VECTOR4, node->getInputCPConnection(1));

    // SwivelAngle
    net->TaskAddInputCP(task, taskIndex++, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(2));

    // IkFkBlendWeight
    net->TaskAddInputCP(task, taskIndex++, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(3));
    
    // SwivelContributionToOrientation
    net->TaskAddInputCP(task, taskIndex++, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(4));
    
    // IK const setup
    net->TaskAddDefInputParam(task, taskIndex++, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    // IK chain setup - anim set dependent
    net->TaskAddDefInputParam(
      task, taskIndex++, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddDefInputParam(
      task, taskIndex++, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(
      task, taskIndex++, transformSemantic, attribType, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);

    // The previous frame's trajectory transform, computed by the setup task
    net->TaskAddParamAndDependency(
      task, taskIndex++, ATTRIB_SEMANTIC_TRAJECTORY_TRANSFORM, ATTRIB_TYPE_TRANSFORM, node->getNodeID(),
       INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTwoBoneIKQueueTwoBoneIKTransformBuffs(
  NodeDef*            node,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter)
{
  return nodeTwoBoneIKQueueTwoBoneIKTransforms(
    node,
    queue,
    net,
    dependentParameter,
    MR::CoreTaskIDs::MR_TASKID_TWOBONEIKTRANSFORMS,
    ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER,
    true);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTwoBoneIKQueueTwoBoneIKTrajectoryDeltaAndTransformBuffs(
  NodeDef*            node,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter)
{
  return nodeTwoBoneIKQueueTwoBoneIKTransforms(
    node,
    queue,
    net,
    dependentParameter,
    MR::CoreTaskIDs::MR_TASKID_TWOBONEIKTRAJECTORY_DELTA_AND_TRANSFORMS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER,
    false);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
