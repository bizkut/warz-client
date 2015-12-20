//----------------------------------------------------------------------------------------------------------------------
#include "euphoria/Nodes/erNodeTrajectoryOverride.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
static MR::Task* nodeTrajectoryOverrideCreateTask(
  MR::NodeDef*        node,
  MR::TaskQueue*      queue,
  MR::Network*        net,
  MR::TaskParameter*  dependentParameter)
{
  MR::Task* task = queue->createNewTaskOnQueue(
    MR::CoreTaskIDs::MR_TASKID_TRAJECTORYOVERRIDE,
    node->getNodeID(),
    7,
    dependentParameter);

  if (task)
  {
    MR::FrameCount currFrameNo = net->getCurrentFrameNo();

    NMP_ASSERT(currFrameNo > 0);

    const MR::AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    const MR::NodeID poseChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 1);

    net->TaskAddParamAndDependency(task, 0,  MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, MR::ATTRIB_TYPE_TRANSFORM_BUFFER, poseChildNodeID, MR::INVALID_NODE_ID, MR::TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddDefInputParam(task, 1,  MR::ATTRIB_SEMANTIC_RIG, MR::NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddNetInputParam(task, 2,  MR::ATTRIB_SEMANTIC_PHYSICS_RIG, MR::NETWORK_NODE_ID, MR::INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    net->TaskAddNetInputParam(task, 3,  MR::ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, MR::NETWORK_NODE_ID, MR::INVALID_NODE_ID, currFrameNo);
    net->TaskAddDefInputParam(task, 4,  MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, node->getNodeID());
    net->TaskAddInputCP(task, 5,  MR::ATTRIB_SEMANTIC_CP_VECTOR3, node->getInputCPConnection(0));
    net->TaskAddOutputParamZeroLifespan(task, 6,  MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, MR::ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, MR::INVALID_NODE_ID, currFrameNo);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
MR::Task* nodeTrajectoryOverrideQueueUpdate(
  MR::NodeDef*        node,
  MR::TaskQueue*      queue,
  MR::Network*        net,
  MR::TaskParameter*  dependentParameter)
{
  // Determine whether node is in the active or pass through state - default to true if pin is not connected.
  bool isActive(true);
  MR::AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  MR::AttribDataBool* const activateValue(
    net->updateOptionalInputCPConnection<MR::AttribDataBool>(node->getInputCPConnection(1), animSet));
  isActive = activateValue->m_value; 

  MR::Task* task(NULL);

  // Create a trajectory override task if "activate" input CP is true, otherwise create a pass through task.
  if (isActive)
  {
    task = nodeTrajectoryOverrideCreateTask(node, queue, net, dependentParameter);
  }
  else
  {
    task = queuePassThroughChild0(node, queue, net, dependentParameter);
  }
  
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// Update of any node data that must happen before queue building or execution.
MR::NodeID nodeTrajectoryOverrideUpdateConnections(
  MR::NodeDef* node,
  MR::Network* net)
{
  NMP_ASSERT(node);
  NMP_ASSERT(net);

  const MR::AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  net->updateInputCPConnection<MR::AttribDataVector3>(node->getInputCPConnection(0), animSet);
  net->updateOptionalInputCPConnection<MR::AttribDataBool>(node->getInputCPConnection(1), animSet);

  // Get this nodes connections structure to fill in.
  MR::NodeConnections* nodeConnections = net->getActiveNodesConnections(node->getNodeID());

  NMP_ASSERT(nodeConnections);
  nodeConnections->m_numActiveChildNodes = 2;

  // Recurse to children.

  // Update Source child node.
  MR::NodeID activeChildNodeID0 = node->getChildNodeID(0);
  nodeConnections->m_activeChildNodeIDs[0] = activeChildNodeID0;
  net->updateNodeInstanceConnections(activeChildNodeID0, animSet);

  // Update Pose child node.
  MR::NodeID activeChildNodeID1 = node->getChildNodeID(1);
  nodeConnections->m_activeChildNodeIDs[1] = activeChildNodeID1;
  net->updateNodeInstanceConnections(activeChildNodeID1, animSet);

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeID nodeTrajectoryOverrideFindGeneratingNodeForSemantic(
  MR::NodeID              NMP_UNUSED(callingNodeID),
  bool                    fromParent,   // Was this query from a parent or child node.
  MR::AttribDataSemantic  semantic,
  MR::NodeDef*            node,
  MR::Network*            network)
{
  MR::NodeID result = MR::INVALID_NODE_ID;

  switch (semantic)
  {
  case  MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM:
    result = node->getNodeID();
    break;
  default:
    // Pass on to parent or child appropriately.
    if (fromParent)
    {
      NMP_ASSERT(node->getNumChildNodes() != 0);

      MR::NodeID activeChildNodeID0 = network->getActiveChildNodeID(node->getNodeID(), node->getPassThroughChildIndex());
      result = network->nodeFindGeneratingNodeForSemantic(activeChildNodeID0, semantic, fromParent, node->getNodeID());
    }
    else
    {
      MR::NodeID activeParentNodeID = network->getActiveParentNodeID(node->getNodeID());
      result = network->nodeFindGeneratingNodeForSemantic(activeParentNodeID, semantic, fromParent, node->getNodeID());
    }
    break;
  }

  return result; 
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
