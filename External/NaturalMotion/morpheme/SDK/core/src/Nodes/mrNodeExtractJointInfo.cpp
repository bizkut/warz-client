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
#include "morpheme/Nodes/mrNodeExtractJointInfo.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeExtractJointInfoJointSelectUpdateConnections(
  NodeDef*           node,
  Network*           net)
{
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  NMP_ASSERT(connections);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the joint index selecting input control parameter.
  net->updateInputCPConnection<AttribDataUInt>(node->getInputCPConnection(0), animSet);

  // Recurse to children.
  net->updateNodeInstanceConnections(connections->m_activeChildNodeIDs[0], animSet);

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
namespace 
{

//----------------------------------------------------------------------------------------------------------------------
void addCommonAttributes(
  NodeDef*            node,
  Network*            net,
  Task*               task,
  AttribDataSemantic  transformSemantic,
  AttribDataType      attribType)
{
  NMP_ASSERT(task);

  FrameCount currFrameNo = net->getCurrentFrameNo();
  AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
  NMP_ASSERT(currFrameNo > 0);

  NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
  net->TaskAddParamAndDependency(task, 0, transformSemantic, attribType, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
  net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_CP_UINT, node->getNodeID(), activeAnimSetIndex); // Joint index
  net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_CP_INT, node->getNodeID(), activeAnimSetIndex); // Angle type
  net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_CP_BOOL, node->getNodeID(), activeAnimSetIndex); // Measure units
  net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
  net->TaskAddOutputParamZeroLifespan(task, 5, transformSemantic, attribType, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  net->TaskAddEmittedCPParam(task, 6, ATTRIB_SEMANTIC_CP_VECTOR3, ATTRIB_TYPE_VECTOR3, node->getNodeID(), NODE_EXTRACT_JOINT_INFO_OUT_PINID_POSITION);  // Joint position.
  net->TaskAddEmittedCPParam(task, 7, ATTRIB_SEMANTIC_CP_FLOAT, ATTRIB_TYPE_FLOAT, node->getNodeID(), NODE_EXTRACT_JOINT_INFO_OUT_PINID_ANGLE);    // Joint angle.
}
  
//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
Task* nodeExtractJointInfoObjectQueueTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_PASSTHROUGHTRANSFORMS_EXTRACTJOINTINFOOBJECTSPACE,
                 node->getNodeID(),
                 8,
                 dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    addCommonAttributes(node, net, task, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeExtractJointInfoObjectQueueTrajectoryDeltaAndTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_EXTRACTJOINTINFOOBJECTSPACE,
    node->getNodeID(),
    8,
    dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    addCommonAttributes(node, net, task, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeExtractJointInfoLocalQueueTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_PASSTHROUGHTRANSFORMS_EXTRACTJOINTINFOLOCALSPACE,
    node->getNodeID(),
    8,
    dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    addCommonAttributes(node, net, task, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeExtractJointInfoLocalQueueTrajectoryDeltaAndTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_EXTRACTJOINTINFOLOCALSPACE,
    node->getNodeID(),
    8,
    dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    addCommonAttributes(node, net, task, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeExtractJointInfoObjectJointSelectQueueTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_PASSTHROUGHTRANSFORMS_EXTRACTJOINTINFOOBJECTSPACE_JOINTSELECT,
    node->getNodeID(),
    9,
    dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    addCommonAttributes(node, net, task, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
    net->TaskAddInputCP(task, 8, ATTRIB_SEMANTIC_CP_INT, node->getInputCPConnection(0));
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeExtractJointInfoObjectJointSelectQueueTrajectoryDeltaAndTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_EXTRACTJOINTINFOOBJECTSPACE_JOINTSELECT,
    node->getNodeID(),
    9,
    dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    addCommonAttributes(node, net, task, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
    net->TaskAddInputCP(task, 8, ATTRIB_SEMANTIC_CP_INT, node->getInputCPConnection(0));
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeExtractJointInfoLocalJointSelectQueueTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_PASSTHROUGHTRANSFORMS_EXTRACTJOINTINFOLOCALSPACE_JOINTSELECT,
    node->getNodeID(),
    9,
    dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    addCommonAttributes(node, net, task, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
    net->TaskAddInputCP(task, 8, ATTRIB_SEMANTIC_CP_INT, node->getInputCPConnection(0));
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeExtractJointInfoLocalJointSelectQueueTrajectoryDeltaAndTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_EXTRACTJOINTINFOLOCALSPACE_JOINTSELECT,
    node->getNodeID(),
    9,
    dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    addCommonAttributes(node, net, task, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
    net->TaskAddInputCP(task, 8, ATTRIB_SEMANTIC_CP_INT, node->getInputCPConnection(0));
  }
  return task;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
