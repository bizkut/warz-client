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
#include "NMPlatform/NMMemory.h"
#include "morpheme/mrDefines.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodeSmoothTransforms.h"
#include "morpheme/mrCoreTaskIDs.h"

//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Node queuing functions
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace
{
  
//----------------------------------------------------------------------------------------------------------------------
Task* subTaskSmoothTransformsQueueTransforms(
  NodeDef*            node,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter,
  TaskID              taskID,
  AttribDataSemantic  transformSemantic,
  AttribDataType      attribType)
{
  // Create a task to perform the blend from the network output transforms to the input source
  Task* task = queue->createNewTaskOnQueue(
    taskID,
    node->getNodeID(),
    10,
    dependentParameter);
  if (task)
  {
    uint32_t currFrameNo = net->getCurrentFrameNo();
    NMP_ASSERT(currFrameNo > 0);
    uint16_t activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

    // Rig def buffer for the current animation set.
    net->TaskAddDefInputParam(
      task,
      0,
      ATTRIB_SEMANTIC_RIG,
      NETWORK_NODE_ID,
      activeAnimSetIndex);

    // Input transform buffer.
    net->TaskAddParamAndDependency(
      task,
      1,
      transformSemantic,
      attribType,
      node->getChildNodeID(0),
      INVALID_NODE_ID,
      TPARAM_FLAG_INPUT,
      currFrameNo);

    // Output transform buffer (keeping hold of state at three previous steps).
    net->TaskAddOutputParam(
      task,
      2,
      transformSemantic,
      attribType,
      INVALID_NODE_ID,
      TPARAM_FLAG_OUTPUT,
      currFrameNo,
      4, // lifespan
      activeAnimSetIndex);
    net->TaskAddOptionalNetInputParam(
      task,
      3,
      transformSemantic,
      node->getNodeID(),
      INVALID_NODE_ID,
      currFrameNo - 1);
    net->TaskAddOptionalNetInputParam(
      task,
      4,
      transformSemantic,
      node->getNodeID(),
      INVALID_NODE_ID,
      currFrameNo - 2);
    net->TaskAddOptionalNetInputParam(
      task,
      5,
      transformSemantic,
      node->getNodeID(),
      INVALID_NODE_ID,
      currFrameNo - 3);

    // Parameters.
    net->TaskAddDefInputParam(
      task,
      6,
      ATTRIB_SEMANTIC_BONE_WEIGHTS,
      node->getNodeID(),
      activeAnimSetIndex);

    // Time
    NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());
    net->TaskAddParamAndDependency(
      task, 7, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeParentNodeID, node->getNodeID(),
      TPARAM_FLAG_INPUT, currFrameNo);
    // Time at previous frame (to recover delta if we are using absolute time)
    net->TaskAddOptionalNetInputParam(
      task, 8, ATTRIB_SEMANTIC_UPDATE_TIME_POS, activeParentNodeID, node->getNodeID(),
      currFrameNo - 1);

    // Smoothing strength multiplier
    net->TaskAddInputCP(task, 9, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(0));
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
}  // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
Task* nodeSmoothTransformsQueueTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return subTaskSmoothTransformsQueueTransforms(
    node,
    queue,
    net,
    dependentParameter,
    CoreTaskIDs::MR_TASKID_OUTPUT_SMOOTH_TRANSFORMS_TRANSFORMS,
    ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeSmoothTransformsQueueTrajectoryDeltaAndTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return subTaskSmoothTransformsQueueTransforms(
    node,
    queue,
    net,
    dependentParameter,
    CoreTaskIDs::MR_TASKID_OUTPUT_SMOOTH_TRANSFORMS_TRAJECTORY_DELTA_AND_TRANSFORMS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
