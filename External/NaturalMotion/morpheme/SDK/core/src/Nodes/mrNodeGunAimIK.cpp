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
#include "morpheme/Nodes/mrNodeGunAimIK.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeGunAimIKQueueGunAimIKSetup(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  // PPU-only on PS3 (because of the character controller being used) so does not support remote
  // evaluation (last argument).
  Task* task = queue->createNewTaskOnQueue(
                 MR::CoreTaskIDs::MR_TASKID_GUNAIMSETUP,
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
    net->TaskAddNetInputParam(task, 0, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);

    // The previous frame's world space trajectory transform, extracted from the character controller
    net->TaskAddOutputParamZeroLifespan(
      task, 1, ATTRIB_SEMANTIC_TRAJECTORY_TRANSFORM,
      ATTRIB_TYPE_TRANSFORM, INVALID_NODE_ID, currFrameNo);
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
bool nodeGunAimIKShouldQueuePassThrough(NodeDef* node, Network* net)
{
  const CPConnection* cpConnection = node->getInputCPConnection(1);
  NMP_ASSERT(cpConnection->m_sourceNodeID != INVALID_NODE_ID);

  // Get the AttribData from connected nodes output control param pin. It must already exist.
  NodeBin* nodeBin = net->getNodeBin(cpConnection->m_sourceNodeID);
  OutputCPPin* outputCPPin = nodeBin->getOutputCPPin(cpConnection->m_sourcePinIndex);
  AttribDataFloat* blendWeight = outputCPPin->getAttribData<AttribDataFloat>();

  // node should pass through if control parameter is connected and the weight is close to 0
  return blendWeight->m_value <= IK_NODE_PASS_THROUGH_WEIGHT;
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
Task* nodeGunAimIKQueueGunAimIKTransformBuffs(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  // Pass through for low blend values.
  if (nodeGunAimIKShouldQueuePassThrough(node, net))
  {
    return queuePassThroughChild0(node, queue, net, dependentParameter);
  }

  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_GUNAIMTRANSFORMS,
                 node->getNodeID(),
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

    NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
    net->TaskAddParamAndDependency(
      task, 0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID,
      TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddParamAndDependency(
      task, 1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, activeChildNodeID, INVALID_NODE_ID,
      TPARAM_FLAG_INPUT, currFrameNo);

    // Two connected control parameters providing the TargetPos and the BlendWeight.
    net->TaskAddInputCP(task, 2, ATTRIB_SEMANTIC_CP_VECTOR3, node->getInputCPConnection(0));
    net->TaskAddInputCP(task, 3, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(1));

    net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    net->TaskAddDefInputParam(
      task, 5, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 6, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(
      task, 7, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);

    // The previous frame's trajectory transform, computed by the setup task
    net->TaskAddParamAndDependency(
      task, 8, ATTRIB_SEMANTIC_TRAJECTORY_TRANSFORM, ATTRIB_TYPE_TRANSFORM, node->getNodeID(),
      INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    // The kinematic joint limits, if they exist
    net->TaskAddOptionalDefInputParam(
      task, 9, ATTRIB_SEMANTIC_JOINT_LIMITS, NETWORK_NODE_ID, activeAnimSetIndex);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeGunAimIKQueueGunAimIKTrajectoryDeltaAndTransformBuffs(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  // Pass through for low blend values.
  if (nodeGunAimIKShouldQueuePassThrough(node, net))
  {
    return queuePassThroughChild0(node, queue, net, dependentParameter);
  }

  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_GUNAIMTRAJECTORY_DELTA_AND_TRANSFORMS,
    node->getNodeID(),
    9,
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

    NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
    net->TaskAddParamAndDependency(
      task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID,
      TPARAM_FLAG_INPUT, currFrameNo);

    // Two connected control parameters providing the TargetPos and the BlendWeight.
    net->TaskAddInputCP(task, 1, ATTRIB_SEMANTIC_CP_VECTOR3, node->getInputCPConnection(0));
    net->TaskAddInputCP(task, 2, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(1));

    net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    net->TaskAddDefInputParam(
      task, 4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 5, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(
      task, 6, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);

    // The character controller, for determining the world-space location of the character
    net->TaskAddNetInputParam(
      task, 7, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);

    // The kinematic joint limits, if they exist
    net->TaskAddOptionalDefInputParam(
      task, 8, ATTRIB_SEMANTIC_JOINT_LIMITS, NETWORK_NODE_ID, activeAnimSetIndex);
  }

  return task;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
