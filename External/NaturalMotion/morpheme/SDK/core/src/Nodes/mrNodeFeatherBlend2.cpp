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
#include "morpheme/Nodes/mrNodeBlend2.h"
#include "morpheme/Nodes/mrNodeFeatherBlend2.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFeatherBlend2QueueTask(
  NodeDef*            nodeDef,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter,
  TaskID              taskID,
  AttribDataSemantic  taskDataSemantic,
  AttribDataType      attribType,
  NodeID              activeChildNodeID0,
  NodeID              activeChildNodeID1)
{
  NMP_ASSERT(nodeDef);
  NMP_ASSERT(queue);
  NMP_ASSERT(net);

  Task* task = queue->createNewTaskOnQueue(
    taskID,
    nodeDef->getNodeID(),
    5,
    dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(nodeDef->getNodeID());
    FrameCount currFrameNo = net->getCurrentFrameNo();
    NMP_ASSERT(currFrameNo > 0);

    net->TaskAddOutputParamZeroLifespan(task, 0, taskDataSemantic, attribType, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 1, taskDataSemantic, attribType, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 2, taskDataSemantic, attribType, activeChildNodeID1, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddNetInputParam(task, 3, ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo);
    net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_BONE_WEIGHTS, nodeDef->getNodeID(), activeAnimSetIndex);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE
Task* nodeFeatherBlend2QueueTransformsTask(
  NodeDef*            nodeDef,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter,
  TaskID              taskID,
  AttribDataSemantic  taskDataSemantic,
  AttribDataType      attribType)
{
  NMP_ASSERT(nodeDef);
  NMP_ASSERT(queue);
  NMP_ASSERT(net);

  // Get the blend node optimised connections.
  const BlendOptNodeConnections* activeNodeConnections = (const BlendOptNodeConnections*)net->getActiveNodesConnections(nodeDef->getNodeID());
  NMP_ASSERT(
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs == 1 ||
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs == 2);

  Task* task;
  if (activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs == 2)
  {
    task = nodeFeatherBlend2QueueTask(
      nodeDef, queue, net, dependentParameter,
      taskID,
      taskDataSemantic,
      attribType,
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs[0],
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs[1]);
  }
  else
  {
    task = queuePassThroughOnChildNodeID(
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs[0],
      nodeDef,
      queue,
      net,
      dependentParameter);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// Transforms
Task* nodeFeatherBlend2QueueFeatherBlend2TransformBuffsAddAttAddPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRANSFORMBUFFSADDATTADDPOS,
    ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFeatherBlend2QueueFeatherBlend2TransformBuffsAddAttInterpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRANSFORMBUFFSADDATTINTERPPOS,
    ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFeatherBlend2QueueFeatherBlend2TransformBuffsInterpAttAddPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRANSFORMBUFFSINTERPATTADDPOS,
    ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFeatherBlend2QueueFeatherBlend2TransformBuffsInterpAttInterpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRANSFORMBUFFSINTERPATTINTERPPOS,
    ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
// Trajectory
Task* nodeFeatherBlend2QueueTrajectoryDeltaTransformAddAttAddPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSADDATTADDPOS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFeatherBlend2QueueTrajectoryDeltaTransformAddAttInterpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSADDATTINTERPPOS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFeatherBlend2QueueTrajectoryDeltaTransformAddAttSlerpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSADDATTSLERPPOS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFeatherBlend2QueueTrajectoryDeltaTransformInterpAttAddPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSINTERPATTADDPOS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFeatherBlend2QueueTrajectoryDeltaTransformInterpAttInterpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFeatherBlend2QueueTrajectoryDeltaTransformInterpAttSlerpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
}

//----------------------------------------------------------------------------------------------------------------------
// Trajectory delta and transforms
Task* nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosSlerpTraj(
  NodeDef* nodeDef,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSADDATTADDPOSSLERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosSlerpTraj(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSADDATTINTERPPOSSLERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosSlerpTraj(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSINTERPATTADDPOSSLERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosSlerpTraj(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSINTERPATTINTERPPOSSLERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosInterpTraj(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSADDATTADDPOSINTERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosInterpTraj(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSADDATTINTERPPOSINTERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosInterpTraj(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSINTERPATTADDPOSINTERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosInterpTraj(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFeatherBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_FEATHERBLEND2TRAJECTORYDELTAANDTRANSFORMSINTERPATTINTERPPOSINTERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
// 
NodeID nodeFeatherBlend2UpdateConnectionsFixBlendWeight(
  NodeDef*           nodeDef,
  Network*           net)
{
  //------------------------
  // Calculate the blend weights  
  AttribDataBlendWeights* attribBlendWeights = nodeBlend2CreateBlendWeights(nodeDef, net);
  BlendOptNodeConnections* activeNodeConnections = static_cast<BlendOptNodeConnections*>( net->getActiveNodesConnections(nodeDef->getNodeID()) );
  const float blendWeight = 1.0f; // Fix the blend weight
  AnimSetIndex animSet = net->getOutputAnimSetIndex(nodeDef->getNodeID());

  // Get the events blend weight
  float blendWeightEvents = blendWeight;
  if (nodeDef->getNumInputCPConnections() > 0)
  {
    // There is no connected blend weight so the events blend weight is located at index 0
    const AttribDataFloatArray* nodeChildWeights = nodeDef->getAttribData<AttribDataFloatArray>(ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS);
    AttribDataFloat* inputCPFloat0 = net->updateInputCPConnection<AttribDataFloat>(nodeDef->getInputCPConnection(0), animSet);
    blendWeightEvents = nodeBlend2CalculateBlendWeight(inputCPFloat0->m_value, nodeChildWeights->m_values);
  }

  //------------------------
  // Sets members of attribBlendWeights and activeNodeConnections. Indicating the number of and which nodes are active for 
  // the calculation of trajectory and transform and events. These are used for the optimisation of trajectory/transforms and 
  // events where only one child is active.
  nodeBlend2UpdateConnectionsSetBlendWeightsCheckForOptimisation( attribBlendWeights, 
                                                                  activeNodeConnections,
                                                                  net, 
                                                                  nodeDef,
                                                                  blendWeight, 
                                                                  blendWeightEvents,
                                                                  false );

  //------------------------
  // Initialise the playback pos
  nodeBlendPlaybackPosInit(nodeDef, net);

  //------------------------
  // Recurse to children
  NMP_ASSERT(activeNodeConnections->m_activeChildNodeIDs[0] == nodeDef->getChildNodeID(0));
  NMP_ASSERT(activeNodeConnections->m_activeChildNodeIDs[1] == nodeDef->getChildNodeID(1));
  net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[0], animSet);
  net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[1], animSet);

  return nodeDef->getNodeID();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
