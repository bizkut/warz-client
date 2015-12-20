// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "Morpheme/Nodes/mrNodeHipsIK.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeHipsIKQueueUpdateTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NodeID nodeID = node->getNodeID();
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // No need to queue the task if the blend weight is zero
  const AttribDataFloat* blendWeight = net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(4), animSet);
  if (blendWeight->m_value < ERROR_LIMIT)
  {
    return queuePassThroughChild0(node, queue, net, dependentParameter);
  }

  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_HIPSIKTRANSFORMS,
    nodeID,
    10,
    dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task);
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);

    // Input transforms
    net->TaskAddParamAndDependency(
      task, 0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, net->getActiveChildNodeID(nodeID, 0),
      INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    // Output transforms
    net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);

    // The rig - for the hierarchy
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);

    // General node def data
    net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, nodeID, 0);

    // Per-anim-set def data
    net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, nodeID, activeAnimSetIndex);

    // PositionDelta control parameter
    net->TaskAddInputCP(task, 5, ATTRIB_SEMANTIC_CP_VECTOR3, node->getInputCPConnection(0));

    // RotationDelta control parameter. (Is optional since the RotationDelta control parameter is
    // either connected to Vector3 or Vector4 pins as a quat or an Euler vector).
    net->TaskAddOptionalInputCP(task, 6, ATTRIB_SEMANTIC_CP_VECTOR4, node->getInputCPConnection(1));

    // RotationDeltaEuler control parameter. (Is optional since the RotationDelta control parameter is
    // either connected to Vector3 or Vector4 pins as a quat or an Euler vector).
    net->TaskAddOptionalInputCP(task, 7, ATTRIB_SEMANTIC_CP_VECTOR3, node->getInputCPConnection(2));

    // FootTurnWeight control parameter
    net->TaskAddInputCP(task, 8, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(3));

    // IkFkBlendWeight control parameter
    net->TaskAddInputCP(task, 9, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(4));
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeHipsIKQueueUpdateTrajectoryDeltaAndTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NodeID nodeID = node->getNodeID();
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // No need to queue the task if the blend weight is zero
  const AttribDataFloat* blendWeight = net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(4), animSet);
  if (blendWeight->m_value < ERROR_LIMIT)
  {
    return queuePassThroughChild0(node, queue, net, dependentParameter);
  }

  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_HIPSIKTRAJECTORY_DELTA_AND_TRANSFORMS,
    nodeID,
    10,
    dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task);
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);

    // Input trajectory and transforms
    net->TaskAddParamAndDependency(
      task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, net->getActiveChildNodeID(nodeID, 0),
      INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

    // Output transforms
    net->TaskAddOutputParamZeroLifespan(
      task, 1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);

    // The rig - for the hierarchy
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);

    // General node def data
    net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, nodeID, 0);

    // Per-anim-set def data
    net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, nodeID, activeAnimSetIndex);

    // PositionDelta control parameter
    net->TaskAddInputCP(task, 5, ATTRIB_SEMANTIC_CP_VECTOR3, node->getInputCPConnection(0));

    // RotationDelta control parameter. (Is optional since the RotationDelta control parameter is
    // either connected to Vector3 or Vector4 pins as a quat or an Euler vector).
    net->TaskAddOptionalInputCP(task, 6, ATTRIB_SEMANTIC_CP_VECTOR4, node->getInputCPConnection(1));

    // RotationDeltaEuler control parameter. (Is optional since the RotationDelta control parameter is
    // either connected to Vector3 or Vector4 pins as a quat or an Euler vector).
    net->TaskAddOptionalInputCP(task, 7, ATTRIB_SEMANTIC_CP_VECTOR3, node->getInputCPConnection(2));

    // FootTurnWeight control parameter
    net->TaskAddInputCP(task, 8, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(3));

    // IkFkBlendWeight control parameter
    net->TaskAddInputCP(task, 9, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(4));
  }

  return task;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
