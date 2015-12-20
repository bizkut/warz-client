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
#include "physics/Nodes/mrNodeSetNonPhysicsTransforms.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeSetNonPhysicsTransformsQueueSetNonPhysicsTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

  // The ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP might not exist if the active animation set doesn't have a physics rig, in
  // which case the Physics source should be passed through.
  if (net->getNetworkDef()->getOptionalAttribDataHandle(
    ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP,
    NETWORK_NODE_ID,
    activeAnimSetIndex) == NULL)
  {
    return queuePassThroughChild0(node, queue, net, dependentParameter);
  }
  else
  {
    Task* task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_SETNONPHYSICSTRANSFORMS,
             node->getNodeID(),
             4,
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

      NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);
      NodeID activeChildNodeID1 = net->getActiveChildNodeID(node->getNodeID(), 1);

      net->TaskAddDefInputParam(task, 0, ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP, NETWORK_NODE_ID, activeAnimSetIndex);
      net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID1, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddOutputParamZeroLifespan(task, 3, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    }

    return task;
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
