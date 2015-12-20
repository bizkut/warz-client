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
#include "morpheme/Nodes/mrNodeMirrorTransforms.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeMirrorQueueSampledEventsBuffer(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = 0;
  NodeID thisNodeID = node->getNodeID();
  NodeID activeParentNodeID = net->getActiveParentNodeID(thisNodeID);

  // if parent has a sync track
  if (net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, false, thisNodeID) != INVALID_NODE_ID)
  {
    task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_MIRROR_SAMPLED_EVENTS,
             thisNodeID,
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
      NodeID activeChildNodeID0 = net->getActiveChildNodeID(thisNodeID, 0);

      net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, INVALID_NODE_ID, currFrameNo);
      net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING, NETWORK_NODE_ID, activeAnimSetIndex);
    }
  }
  else
  {
    task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_MIRROR_SAMPLED_AND_DURATION_EVENTS,
             thisNodeID,
             7,
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
      NodeID activeChildNodeID0 = net->getActiveChildNodeID(thisNodeID, 0);

      net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, INVALID_NODE_ID, currFrameNo);
      net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, thisNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, thisNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddOptionalDefInputParam(task, 5, ATTRIB_SEMANTIC_LOOP, activeChildNodeID0);
      net->TaskAddDefInputParam(task, 6, ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING, NETWORK_NODE_ID, activeAnimSetIndex);
    }
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeMirrorQueueTimePos(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = 0;

  NodeID thisNodeID = node->getNodeID();
  NodeID activeParentNodeID = net->getActiveParentNodeID(thisNodeID);

  // if parent has a sync track
  if (net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, false, thisNodeID) != INVALID_NODE_ID)
  {
    task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_MIRROR_UPDATE_TIME_VIA_SYNC_EVENT_POS,
             node->getNodeID(),
             6,
             dependentParameter);

#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
    NMP_ASSERT(task)
#else
    if (task)
#endif
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      NMP_ASSERT(currFrameNo > 0);

      net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS, activeParentNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_TIME_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputParamZeroLifespan(task, 2, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, thisNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddOptionalNetInputOutputParam(task, 4, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 5, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
    }
  }
  else
  {
    task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_MIRROR_UPDATE_TIME_VIA_TIME_POS,
             thisNodeID,
             7,
             dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
    NMP_ASSERT(task)
#else
    if (task)
#endif
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      NMP_ASSERT(currFrameNo > 0);

      net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeParentNodeID, thisNodeID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_TIME_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputParamZeroLifespan(task, 2, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, thisNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddOptionalNetInputOutputParam(task, 4, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 5, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);

      // Find if someone (usually a transition) has stored a start playback pos adjustment on us or on a filter node parent.
      net->TaskAddOptionalNetInputParamRecurseToParent(task, 6, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER, ANIMATION_SET_ANY);
    }
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeMirrorQueueSyncEventsBuffer(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NodeID thisNodeID = node->getNodeID();

  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_MIRROR_SYNC_EVENTS_AND_OFFSET,
                 thisNodeID,
                 5,
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
    NodeID activeChildNodeID0 = net->getActiveChildNodeID(thisNodeID, 0);

    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, INVALID_NODE_ID, currFrameNo);
    net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddInputParam(task, 2, ATTRIB_SEMANTIC_SYNC_EVENT_OFFSET, thisNodeID, INVALID_NODE_ID, TPARAM_FLAG_NONE, VALID_FOREVER);
    net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING, NETWORK_NODE_ID, activeAnimSetIndex);

    // Find if someone (usually a transition) has stored a start sync event adjustment on us or on a filter node parent.
    net->TaskAddOptionalNetInputParamRecurseToParent(task, 4, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, thisNodeID, INVALID_NODE_ID, VALID_FOREVER, ANIMATION_SET_ANY);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeMirrorQueueDurationEventsBuffer(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  NodeID thisNodeID = node->getNodeID();

  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_MIRROR_DURATION_EVENTS,
                 thisNodeID,
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
    NodeID activeChildNodeID0 = net->getActiveChildNodeID(thisNodeID, 0);

    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, INVALID_NODE_ID, currFrameNo);
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING, NETWORK_NODE_ID, activeAnimSetIndex);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeMirrorQueueTransformBuff(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_MIRROR_TRANSFORMS,
                 node->getNodeID(),
                 5,
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

    net->TaskAddDefInputParam(task, 0, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_BONE_IDS, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddOutputParamZeroLifespan(task, 4, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeMirrorQueueTrajectoryDeltaTransform(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_MIRROR_TRAJECTORY_DELTA,
                 node->getNodeID(),
                 5,
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

    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, INVALID_NODE_ID, currFrameNo);
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_BONE_IDS, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
  }

  return task;
}
//----------------------------------------------------------------------------------------------------------------------
Task* nodeMirrorQueueTrajectoryDeltaAndTransforms(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_MIRROR_TRAJECTORY_DELTA_TRANSFORMS,
    node->getNodeID(),
    5,
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

    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_BONE_IDS, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
  }

  return task;
}


} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
