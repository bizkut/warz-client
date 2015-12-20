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
#include "morpheme/Nodes/mrNodeApplyGlobalTime.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeApplyGlobalTimeQueueUpdateTimePos(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_APPLYGLOBALTIME,
                 node->getNodeID(),
                 2,
                 dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    NMP_ASSERT(currFrameNo > 0);

    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, NETWORK_NODE_ID, INVALID_NODE_ID , TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
  }

  return task;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
