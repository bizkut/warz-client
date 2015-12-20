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
#include "morpheme/Nodes/mrNodeApplyBindPose.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeApplyBindPoseQueueApplyBindPoseTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_APPLYBINDPOSETRANSFORMS,
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
    NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);

    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(task, 2, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeApplyBindPoseQueueApplyBindPoseTrajectoryDeltaAndTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_APPLYBINDPOSETRAJECTORYDELTAANDTRANSFORMS,
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
    NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);

    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(task, 2, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  }

  return task;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
