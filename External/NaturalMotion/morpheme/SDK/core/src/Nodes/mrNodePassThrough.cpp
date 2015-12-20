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
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief This function passes through the dependency of a piece of attribute data.
///
/// Either:
///  + from a child to the parent or
///  + from the parent to a child.
/// Which direction to pass this dependency is automatically determined based on whether the incoming dependency is
/// from a child (or grandchild) or from a parent (or grandparent).
/// If the dependency is from the parent direction, which child to pass the dependency on to is determined by the
/// provided active child index.
/// This function is used by the optimised blend functions
Task* queuePassThroughOnChildNodeID(
  NodeID          activeChildNodeID,
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  // There must always be a dependent task even when this is the root node in the network.
  NMP_ASSERT(node);

  // Queue the task that will create a reference to the input parameter.
  TaskID createRefTask = CoreTaskIDs::getCreateReferenceToInputTaskID(dependentParameter->m_attribAddress.m_semantic);
  Task* task = queue->createNewTaskOnQueue(
    createRefTask,
    node->getNodeID(),
    2,
    dependentParameter,
    false,
    true,
    false,
    true);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();

    // Dependency must come directly from a parent or child. NOTE: if necessary dev a system to hold
    // dependency direction, up or down the tree.
    NodeConnections* nodeConnections = net->getActiveNodesConnections(node->getNodeID());
    NMP_ASSERT(nodeConnections);
    NodeID activeParentNodeID = nodeConnections->m_activeParentNodeID;
    NMP_ASSERT(activeChildNodeID != INVALID_NODE_ID && activeParentNodeID != INVALID_NODE_ID);

    AttribSemanticSense sense = Manager::getInstance().getAttributeSemanticSense(dependentParameter->m_attribAddress.m_semantic);
    NMP_ASSERT(sense != ATTRIB_SENSE_NONE);
    // If the attrib flows from parent to child, the request must come from the parent.
    bool fromParent = (sense == ATTRIB_SENSE_FROM_CHILD_TO_PARENT);

    if (fromParent)
    {
      // Add dependency on our child attrib data.
      net->TaskAddParamAndDependency(
        task,
        0,
        dependentParameter->m_attribAddress.m_semantic,
        dependentParameter->m_attribType,
        activeChildNodeID,
        node->getNodeID(),
        TPARAM_FLAG_INPUT,
        currFrameNo,
        dependentParameter->m_attribAddress.m_animSetIndex);
    }
    else
    {
      // Add dependency on our parent attrib data.
      net->TaskAddParamAndDependency(
        task,
        0,
        dependentParameter->m_attribAddress.m_semantic,
        dependentParameter->m_attribType,
        activeParentNodeID,
        node->getNodeID(),
        TPARAM_FLAG_INPUT,
        currFrameNo,
        dependentParameter->m_attribAddress.m_animSetIndex);
    }

    // Add our output attrib.
    net->TaskAddOutputParamZeroLifespan(
      task,
      1,
      dependentParameter->m_attribAddress.m_semantic,
      dependentParameter->m_attribType,
      INVALID_NODE_ID,
      currFrameNo,
      net->getOutputAnimSetIndex(node->getNodeID()));
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief This function passes through the dependency of a piece of attribute data.
///
/// Either:
///  + from a child to the parent or
///  + from the parent to a child.
/// Which direction to pass this dependency is automatically determined based on whether the incoming dependency is
/// from a child (or grandchild) or from a parent (or grandparent).
/// If the dependency is from the parent direction, which child to pass the dependency on to is determined by the
/// provided active child index.
Task* queuePassThroughOnChildIndex(
  uint32_t        activeChildIndex,
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  // There must always be a dependent task even when this is the root node in the network.
  NMP_ASSERT(node);

  // Queue the task that will create a reference to the input parameter.
  TaskID createRefTask = CoreTaskIDs::getCreateReferenceToInputTaskID(dependentParameter->m_attribAddress.m_semantic);
  Task* task = queue->createNewTaskOnQueue(
                 createRefTask,
                 node->getNodeID(),
                 2,
                 dependentParameter,
                 false,
                 true,
                 false,
                 true);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();

    // Dependency must come directly from a parent or child. NOTE: if necessary dev a system to hold
    // dependency direction, up or down the tree.
    NodeConnections* nodeConnections = net->getActiveNodesConnections(node->getNodeID());
    NMP_ASSERT(nodeConnections);
    NMP_ASSERT(nodeConnections->m_numActiveChildNodes > activeChildIndex);
    NodeID activeChildNodeID = nodeConnections->m_activeChildNodeIDs[activeChildIndex];
    NodeID activeParentNodeID = nodeConnections->m_activeParentNodeID;
    NMP_ASSERT(activeChildNodeID != INVALID_NODE_ID && activeParentNodeID != INVALID_NODE_ID);

    AttribSemanticSense sense = Manager::getInstance().getAttributeSemanticSense(dependentParameter->m_attribAddress.m_semantic);
    NMP_ASSERT(sense != ATTRIB_SENSE_NONE);
    // If the attrib flows from parent to child, the request must come from the parent.
    bool fromParent = (sense == ATTRIB_SENSE_FROM_CHILD_TO_PARENT);
    
    AnimSetIndex activeChildAnimSetIndex = net->getOutputAnimSetIndex(activeChildNodeID);
    if (dependentParameter->m_attribAddress.m_animSetIndex == ANIMATION_SET_ANY)
    {
      activeChildAnimSetIndex = ANIMATION_SET_ANY;
    }

    AnimSetIndex activeParentAnimSetIndex = net->getOutputAnimSetIndex(activeParentNodeID);
    if (dependentParameter->m_attribAddress.m_animSetIndex == ANIMATION_SET_ANY)
    {
      activeParentAnimSetIndex = ANIMATION_SET_ANY;
    }

    if (fromParent)
    {
      // Add dependency on our child attrib data.
      net->TaskAddParamAndDependency(
        task,
        0,
        dependentParameter->m_attribAddress.m_semantic,
        dependentParameter->m_attribType,
        activeChildNodeID,
        node->getNodeID(),
        TPARAM_FLAG_INPUT,
        currFrameNo,
        activeChildAnimSetIndex);
    }
    else
    {
      // Add dependency on our parent attrib data.
      net->TaskAddParamAndDependency(
        task,
        0,
        dependentParameter->m_attribAddress.m_semantic,
        dependentParameter->m_attribType,
        activeParentNodeID,
        node->getNodeID(),
        TPARAM_FLAG_INPUT,
        currFrameNo,
        activeParentAnimSetIndex);
    }

    // Add our output attrib.
    net->TaskAddOutputParamZeroLifespan(
      task,
      1,
      dependentParameter->m_attribAddress.m_semantic,
      dependentParameter->m_attribType,
      INVALID_NODE_ID,
      currFrameNo,
      net->getOutputAnimSetIndex(node->getNodeID()));
  }

  return task;
}
  
//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
Task* queuePassThroughChild0(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return queuePassThroughOnChildIndex(
           0,    // Active child index.
           node,
           queue,
           net,
           dependentParameter);
}

//----------------------------------------------------------------------------------------------------------------------
Task* queuePassThroughChild1(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return queuePassThroughOnChildIndex(
           1,    // Active child index.
           node,
           queue,
           net,
           dependentParameter);
}

//----------------------------------------------------------------------------------------------------------------------
NodeID filterNodeFindGeneratingNodeForSemantic(
  NodeID              NMP_UNUSED(callingNodeID),
  bool                fromParent,   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            net)
{
  NodeID result = INVALID_NODE_ID;

  NMP_ASSERT(node);
  NMP_ASSERT(node->getNodeFlags().isSet(NodeDef::NODE_FLAG_IS_FILTER));
  NMP_ASSERT(net->getNumActiveChildren(node->getNodeID()) >= 1);

  QueueAttrTaskFn queuingFn = node->getTaskQueueingFn(semantic);
  if (queuingFn == queuePassThroughChild0)
  {
    if (fromParent)
    {
      NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
      result = net->nodeFindGeneratingNodeForSemantic(activeChildNodeID, semantic, fromParent, node->getNodeID());
    }
    else
    {
      NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());
      result = net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, semantic, fromParent, node->getNodeID());
    }
  }
  else if (queuingFn)
  {
    NMP_ASSERT(queuingFn != queuePassThroughChild0);
    result = node->getNodeID();
  }

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
