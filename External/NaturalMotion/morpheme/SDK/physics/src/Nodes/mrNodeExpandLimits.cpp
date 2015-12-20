// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "physics/nodes/mrNodeExpandLimits.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
static void nodeExpandLimitsCreateTask(
  NodeDef*           node,
  Network*           net,
  AttribDataSemantic transformSemantic,
  AttribDataType     attribType,
  Task*              task)
{
  if (task)
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    NMP_ASSERT(currFrameNo > 0);

    const AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    const NodeID sourceChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
    const NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());

    net->TaskAddParamAndDependency(task, 0, transformSemantic, attribType, sourceChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_OUTPUT_MASK, ATTRIB_TYPE_BOOL_ARRAY, activeParentNodeID, node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddNetInputParam(task, 2, ATTRIB_SEMANTIC_PHYSICS_RIG, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    net->TaskAddOptionalDefInputParam(task, 3, ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(task, 4, transformSemantic, attribType, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeExpandLimitsQueueTransforms(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  Task* const task = queue->createNewTaskOnQueue(
                                CoreTaskIDs::MR_TASKID_PASSTHROUGHTRANSFORMS_EXPANDLIMITS,
                                node->getNodeID(),
                                5,
                                dependentParameter,
                                false, // External task
                                false); // supports remote eval

  nodeExpandLimitsCreateTask(node, net, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, task);

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeExpandLimitsQueueTrajectoryDeltaAndTransforms(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  Task* const task = queue->createNewTaskOnQueue(
                                CoreTaskIDs::MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_EXPANDLIMITS,
                                node->getNodeID(),
                                5,
                                dependentParameter,
                                false, // External task
                                false); // supports remote eval

  nodeExpandLimitsCreateTask(node, net, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, task);

  return task;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
