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
#include "morpheme/mrCommonTaskQueuingFns.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
Task* queueInitUnitLengthSyncEventTrack(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();

  // Add this task to the queue.
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_INIT_UNIT_LENGTH_SYNC_EVENT_TRACK,
                 node->getNodeID(),
                 1,
                 dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    net->TaskAddOutputParam(
      task, 0, 
      ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, 
      INVALID_NODE_ID, TPARAM_FLAG_NONE, VALID_FOREVER, LIFESPAN_FOREVER);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* queueInitEmptyEventTrackDurationSet(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();

  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_INIT_EMPTY_EVENTTRACKDURATIONSET,
                 node->getNodeID(),
                 1,
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

    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, INVALID_NODE_ID, currFrameNo);
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* queueInitSampledEventsTrack(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();

  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_INIT_SAMPLED_EVENTS_TRACK,
                 node->getNodeID(),
                 1,
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

    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, INVALID_NODE_ID, currFrameNo);
  }
  return task;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
