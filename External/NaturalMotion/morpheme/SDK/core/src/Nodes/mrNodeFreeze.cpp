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
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/Nodes/mrNodeFreeze.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------  
namespace
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFreezePassThrough(
  NodeDef*            node,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter,
  TaskID              taskID,
  AttribDataSemantic  transformSemantic,
  AttribDataType      attribType,
  FrameCount          outputFrameCount,
  uint16_t            outputLifeSpan)
{
  NET_LOG_ENTER_FUNC();
  const FrameCount currFrameNo = net->getCurrentFrameNo();
  NMP_ASSERT(currFrameNo > 0);

  Task* task = queue->createNewTaskOnQueue(
    taskID,
    node->getNodeID(),
    3,
    dependentParameter);

#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

    NodeID rootID = net->getActiveChildNodeID(NETWORK_NODE_ID, 0);
    net->TaskAddOptionalNetInputParam(task, 0, transformSemantic, rootID , INVALID_NODE_ID, currFrameNo - 1);
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    if( outputLifeSpan == 0 )
    {
      net->TaskAddOutputParamZeroLifespan(task, 2, transformSemantic, attribType, INVALID_NODE_ID, outputFrameCount, activeAnimSetIndex);
    }
    else
    {
      net->TaskAddOutputParam(task, 2, transformSemantic, attribType, INVALID_NODE_ID, TPARAM_FLAG_NONE, outputFrameCount, outputLifeSpan, activeAnimSetIndex);
    }
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFreezePassThroughLastTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFreezePassThrough(
    node,
    queue,
    net,
    dependentParameter,
    CoreTaskIDs::MR_TASKID_BUFFER_LAST_FRAME_TRANSFORM_BUFFER,
    ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER,
    net->getCurrentFrameNo(),
    0);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFreezePassThroughLastTrajectoryDeltaAndTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFreezePassThrough(
    node,
    queue,
    net,
    dependentParameter,
    CoreTaskIDs::MR_TASKID_BUFFER_LAST_FRAME_TRAJECTORY_AND_TRANSFORMS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER,
    net->getCurrentFrameNo(),
    0);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFreezePassThroughLastTransformsOnce(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFreezePassThrough(
    node,
    queue,
    net,
    dependentParameter,
    CoreTaskIDs::MR_TASKID_BUFFER_LAST_FRAME_TRANSFORM_BUFFER,
    ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER,
    VALID_FOREVER,
    LIFESPAN_FOREVER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeFreezePassThroughLastTrajectoryDeltaAndTransformsOnce(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeFreezePassThrough(
    node,
    queue,
    net,
    dependentParameter,
    CoreTaskIDs::MR_TASKID_BUFFER_LAST_FRAME_TRANSFORMS_ZERO_TRAJECTORY,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER,
    VALID_FOREVER,
    LIFESPAN_FOREVER);
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeFreezeHasQueuingFunction(
  NodeID              NMP_UNUSED(callingNodeID),
  bool                NMP_UNUSED(fromParent),   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            NMP_UNUSED(network))
{
  NodeID result = INVALID_NODE_ID;

  switch (semantic)
  {
    case ATTRIB_SEMANTIC_TRANSFORM_BUFFER:
    case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER:
    case ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER:
    case ATTRIB_SEMANTIC_SYNC_EVENT_TRACK:
    case ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET:
      result = node->getNodeID();
      break;
    default:
      break;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeFreezeInitInstance(NodeDef* node, Network* net)
{
  NodeID rootNodeID = net->getNetworkDef()->getRootNodeID();

  // Add post update access attrib for transform semantics to maintain previous frame data.
  net->addPostUpdateAccessAttrib(rootNodeID, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, 2);
  net->addPostUpdateAccessAttrib(rootNodeID, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, 2);

  nodeShareInitInstanceInvalidateAllChildren(node, net);
}

NodeID nodeFreezeUpdateConnections(
  NodeDef*           node,
  Network*           net)
{
  getNodeTransformsBuffer(node->getNodeID(), net, net->getCurrentFrameNo(), net->getOutputAnimSetIndex(node->getNodeID()));

  return node->getNodeID();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
