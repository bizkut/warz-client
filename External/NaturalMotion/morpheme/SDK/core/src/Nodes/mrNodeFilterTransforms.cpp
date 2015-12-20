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
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFilterTransformsQueueFilterTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_FILTERTRANSFORMS,
                 node->getNodeID(),
                 3,
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
    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_BONE_IDS, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(task, 2, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFilterTransformsQueueFilterTrajectoryDeltaTransform(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_FILTERTRAJECTORYDELTA,
                 node->getNodeID(),
                 4,
                 dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    FrameCount currFrameNo = net->getCurrentFrameNo();
    NMP_ASSERT(currFrameNo > 0);
    NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);

    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, INVALID_NODE_ID, currFrameNo);
    net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_BONE_IDS, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFilterTransformsQueueFilterTrajectoryDeltaAndTransforms(
  NodeDef* node,
  TaskQueue* queue,
  Network* net, 
  TaskParameter* dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_FILTERTRAJECTORYDELTAANDTRANSFORMS,
    node->getNodeID(),
    4,
    dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    FrameCount currFrameNo = net->getCurrentFrameNo();
    NMP_ASSERT(currFrameNo > 0);
    NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);

    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_BONE_IDS, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* grouperQueuePassThroughFromParent(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  // There must always be a dependent task even when this is the root node in the network.
  NMP_ASSERT(node);

  // Queue the task that will create a reference to the input parameter.
  TaskID createRefTask = CoreTaskIDs::getCreateReferenceToInputTaskID(dependentParameter->m_attribAddress.m_semantic);
  Task* task = queue->createNewTaskOnQueue(
                 createRefTask,
                 node->getNodeID(),
                 2,
                 dependentParameter,
                 false,
                 true,
                 false,
                 true);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();

    // Add dependency on our parent attrib data.
    NodeConnections* nodeConnectivity = net->getActiveNodesConnections(node->getNodeID()); // Get this nodes connectivity structure.
    NMP_ASSERT(nodeConnectivity);

    net->TaskAddParamAndDependency(
      task,
      0,
      dependentParameter->m_attribAddress.m_semantic,
      dependentParameter->m_attribType,
      nodeConnectivity->m_activeParentNodeID,
      node->getNodeID(),
      TPARAM_FLAG_INPUT,
      currFrameNo,
      dependentParameter->m_attribAddress.m_animSetIndex);

    net->TaskAddOutputParamZeroLifespan(
      task,
      1,
      dependentParameter->m_attribAddress.m_semantic,
      dependentParameter->m_attribType,
      INVALID_NODE_ID,
      currFrameNo,
      dependentParameter->m_attribAddress.m_animSetIndex);
  }

  return task;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
