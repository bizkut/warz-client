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
#include "morpheme/Nodes/mrNodePlaySpeedModifier.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodePlaySpeedModifierQueueUpdateTime(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NodeID thisNodeID = node->getNodeID();
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_SCALEUPDATETIME,
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
    NMP_ASSERT(currFrameNo > 0);
    NodeID activeParentNodeID = net->getActiveParentNodeID(thisNodeID);
    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeParentNodeID, thisNodeID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddInputCP(task, 1, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(0));
    net->TaskAddOutputParamZeroLifespan(task, 2, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodePlaySpeedModifierQueueEventTrack(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  NodeID thisNodeID = node->getNodeID();
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_SCALEUPDATESYNCEVENTTRACK,
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
    NMP_ASSERT(currFrameNo > 0);
    NodeID activeChildNodeID = net->getActiveChildNodeID(thisNodeID, 0);

    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, INVALID_NODE_ID, currFrameNo);
    net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddInputCP(task, 2, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(0));
  }

  return task;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
