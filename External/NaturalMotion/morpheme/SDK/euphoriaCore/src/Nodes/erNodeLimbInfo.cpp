//----------------------------------------------------------------------------------------------------------------------
#include "Euphoria/Nodes/erNodeLimbInfo.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{
  using namespace MR;

//----------------------------------------------------------------------------------------------------------------------
static void nodeLimbInfoQueue(
  NodeDef*                node,
  Network*                net,
  MR::AttribDataSemantic  transformSemantic,
  MR::AttribDataType      attribType,
  Task*                   task)
{
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);

    NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);

    // Parameters required by extract joint info sub-task 
    net->TaskAddParamAndDependency(
      task, 0, transformSemantic, attribType, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddDefInputParam(
      task, 1, ATTRIB_SEMANTIC_CP_UINT, node->getNodeID(), activeAnimSetIndex); // Joint index
    net->TaskAddDefInputParam(
      task, 2, ATTRIB_SEMANTIC_CP_INT, node->getNodeID(), 0); // Output orientation type
    net->TaskAddDefInputParam(
      task, 3, ATTRIB_SEMANTIC_CP_BOOL, node->getNodeID(), 0); // Output angle in radians
    net->TaskAddDefInputParam(
      task, 4, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(
      task, 5, transformSemantic, attribType, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    net->TaskAddEmittedCPParam(
      task, 6, ATTRIB_SEMANTIC_CP_VECTOR3, ATTRIB_TYPE_VECTOR3, node->getNodeID(), NODE_LIMB_INFO_OUT_PINID_POSITION); // Joint position.
    net->TaskAddEmittedCPParam(
      task, 7, ATTRIB_SEMANTIC_CP_VECTOR3, ATTRIB_TYPE_VECTOR3, node->getNodeID(), NODE_LIMB_INFO_OUT_PINID_AXIS); // Joint orientation axis.

    // Additional parameters required by limb info task
    net->TaskAddDefInputParam(
      task, 8, ATTRIB_SEMANTIC_CP_VECTOR4, node->getNodeID(), activeAnimSetIndex); // Rotation quat from rig to limb.
  }
}


//----------------------------------------------------------------------------------------------------------------------
Task* nodeLimbInfoQueueTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_PASSTHROUGHTRANSFORMS_LIMBINFO,
    node->getNodeID(),
    9,
    dependentParameter);

  nodeLimbInfoQueue(
  node,
  net,
  ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
  ATTRIB_TYPE_TRANSFORM_BUFFER,
  task);

  return task;
}


//----------------------------------------------------------------------------------------------------------------------
Task* nodeLimbInfoQueueTrajectoryDeltaAndTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_LIMBINFO,
    node->getNodeID(),
    9,
    dependentParameter);

  nodeLimbInfoQueue(
    node,
    net,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER,
    task);

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
