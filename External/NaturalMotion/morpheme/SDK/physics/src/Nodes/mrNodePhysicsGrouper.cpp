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
#include "physics/Nodes/mrNodePhysicsGrouper.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "physics/mrPhysics.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
static Network::RootControlMethod recursivelyGetRootControlMethod(
  const NodeDef* node,
  const Network* net)
{
  // Note that Behaviour nodes also set NODE_FLAG_IS_PHYSICAL so we have to catch them first, so
  // that the ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF is not misinterpretted.
  if (node->getNodeFlags() & NodeDef::NODE_FLAG_IS_BEHAVIOURAL)
  {
    return Network::ROOT_CONTROL_PHYSICS;
  }

  if (node->getNodeFlags() & NodeDef::NODE_FLAG_IS_PHYSICAL)
  {
    AttribDataHandle* handle = net->getNetworkDef()->getAttribDataHandle(
                                 ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,
                                 node->getNodeID());
    NMP_ASSERT(handle);
    AttribDataPhysicsSetup* physicsSetup = (AttribDataPhysicsSetup*) handle->m_attribData;
    NMP_ASSERT(physicsSetup);
    return (Network::RootControlMethod) physicsSetup->m_rootControlMethod;
  }

  if (node->getNodeFlags() & NodeDef::NODE_FLAG_IS_PHYSICS_GROUPER)
  {
    MR::Network::RootControlMethod baseRootControlMethod = recursivelyGetRootControlMethod(node->getChildNodeDef(0), net);
    MR::Network::RootControlMethod overrideRootControlMethod = recursivelyGetRootControlMethod(node->getChildNodeDef(1), net);

    // if an input is animation then we end up treating it like hard keyframing anyway
    if (baseRootControlMethod == Network::ROOT_CONTROL_ANIMATION)
      baseRootControlMethod = Network::ROOT_CONTROL_PHYSICAL_ANIMATION;
    if (overrideRootControlMethod == Network::ROOT_CONTROL_ANIMATION)
      overrideRootControlMethod = Network::ROOT_CONTROL_PHYSICAL_ANIMATION;

    bool useBaseTrajectory;
    if (baseRootControlMethod == overrideRootControlMethod)
    {
      AttribDataHandle* handle = net->getNetworkDef()->getAttribDataHandle(
                                   ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,
                                   node->getNodeID());
      NMP_ASSERT(handle);
      AttribDataBool* preferBaseTrajectory = (AttribDataBool*)handle->m_attribData;
      NMP_ASSERT(preferBaseTrajectory);
      useBaseTrajectory = preferBaseTrajectory->m_value;
    }
    else
    {
      useBaseTrajectory = baseRootControlMethod > overrideRootControlMethod;
    }

    if (useBaseTrajectory)
      return baseRootControlMethod;
    else
      return overrideRootControlMethod;
  }

  // some animation node - find out which child to use
  Network::RootControlMethod result = Network::ROOT_CONTROL_ANIMATION;
  const NodeConnections* nc = net->getActiveNodesConnections(node->getNodeID());
  for (uint32_t i = 0 ; i < nc->m_numActiveChildNodes ; ++i)
  {
    const NodeDef* child = net->getNetworkDef()->getNodeDef(nc->m_activeChildNodeIDs[i]);
    Network::RootControlMethod childMethod =  recursivelyGetRootControlMethod(child, net);
    if (childMethod > result)
      result = childMethod;
    if (result == Network::ROOT_CONTROL_PHYSICAL_ANIMATION)
      break;
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
static void doChildrenNeedConvertingToPhysics(
  const Network* net,
  const NodeDef* node,
  bool&          child0NeedsConverting,
  bool&          child1NeedsConverting)
{
  child0NeedsConverting = false;
  child1NeedsConverting = false;
  if (getPhysicsRig(net) && getPhysicsRig(net)->isReferenced())
  {
    NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);
    NodeID activeChildNodeID1 = net->getActiveChildNodeID(node->getNodeID(), 1);
    child0NeedsConverting = !net->isNodeOrActiveChildPhysical(net->getNetworkDef()->getNodeDef(activeChildNodeID0));
    child1NeedsConverting = !net->isNodeOrActiveChildPhysical(net->getNetworkDef()->getNodeDef(activeChildNodeID1));
  }
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodePhysicsGrouperUpdateConnections(
  NodeDef*           node,
  Network*           net)
{
  NET_LOG_ENTER_FUNC();

  // Recurse to children FIRST so that if they change to/from physics we can adapt
  const NodeConnections* con = net->getActiveNodesConnections(node->getNodeID());
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());
  for (uint32_t i = 0 ; i < con->m_numActiveChildNodes ; ++i)
  {
    NodeID activeChildNodeID = con->m_activeChildNodeIDs[i];
    net->updateNodeInstanceConnections(activeChildNodeID, animSet);
  }

  // only try to control the root if we're converting to physics and
  // actually have access to a physical character
  bool child0NeedsConverting, child1NeedsConverting;
  doChildrenNeedConvertingToPhysics(net, node, child0NeedsConverting, child1NeedsConverting);
  if (child0NeedsConverting || child1NeedsConverting)
  {
    net->setRootControlMethod(Network::ROOT_CONTROL_PHYSICAL_ANIMATION);
  }

  NET_LOG_MESSAGE(98, "nodePhysicsGrouperUpdateConnections network rootControlMethod = %d\n", net->getRootControlMethod());

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodePhysicsGrouperQueueUpdateTrajectory(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();

  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_PHYSICSGROUPERUPDATETRAJECTORY,
                 node->getNodeID(),
                 2,
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

    MR::Network::RootControlMethod baseRootControlMethod = recursivelyGetRootControlMethod(node->getChildNodeDef(0), net);
    MR::Network::RootControlMethod overrideRootControlMethod = recursivelyGetRootControlMethod(node->getChildNodeDef(1), net);

    // if an input is animation then we end up treating it like hard keyframing anyway
    if (baseRootControlMethod == Network::ROOT_CONTROL_ANIMATION)
      baseRootControlMethod = Network::ROOT_CONTROL_PHYSICAL_ANIMATION;
    if (overrideRootControlMethod == Network::ROOT_CONTROL_ANIMATION)
      overrideRootControlMethod = Network::ROOT_CONTROL_PHYSICAL_ANIMATION;

    bool useBaseTrajectory;
    if (baseRootControlMethod == overrideRootControlMethod)
    {
      AttribDataHandle* handle = net->getNetworkDef()->getAttribDataHandle(
                                   ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,
                                   node->getNodeID());
      NMP_ASSERT(handle);
      AttribDataBool* preferBaseTrajectory = (AttribDataBool*)handle->m_attribData;
      NMP_ASSERT(preferBaseTrajectory);
      useBaseTrajectory = preferBaseTrajectory->m_value;
    }
    else
    {
      useBaseTrajectory = baseRootControlMethod > overrideRootControlMethod;
    }

    NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), useBaseTrajectory ? 0 : 1);
    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, INVALID_NODE_ID, currFrameNo);
  }
  return task;
}
  
//----------------------------------------------------------------------------------------------------------------------
namespace
{
  
//----------------------------------------------------------------------------------------------------------------------
AttribDataPhysicsGrouperConfig* getPhysicsGrouperConfig(Network* net, NodeDef* node)
{
  // Add the grouper state/config if it doesn't exist
  AttribDataPhysicsGrouperConfig* physicsGrouperConfig = 0;
  AttribAddress attribAddress(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER);
  NodeBinEntry* entry = net->getAttribDataNodeBinEntry(attribAddress);
  if (entry)
  {
    physicsGrouperConfig = entry->getAttribData<AttribDataPhysicsGrouperConfig>();
    NMP_ASSERT(physicsGrouperConfig);
  }
  else
  {
    AttribDataHandle handle = AttribDataPhysicsGrouperConfig::create(net->getDispatcher()->getMemoryAllocator(LIFESPAN_FOREVER));
    net->addAttribData(attribAddress, handle, LIFESPAN_FOREVER);
    physicsGrouperConfig    = (AttribDataPhysicsGrouperConfig*) handle.m_attribData;
  }
  return physicsGrouperConfig;
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
// Note that even if we're converting one of our inputs, we don't need to do anything like extract
// the results from physics since we know the physics output will match the animation input
Task* nodePhysicsGrouperQueueUpdateTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();
  NMP_ASSERT(node->getNumChildNodes() == 2);

  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_PHYSICSGROUPERUPDATETRANSFORMS,
                 node->getNodeID(),
                 5,
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
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);
    net->TaskAddDefInputParam(task, 0, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, node->getNodeID(), activeAnimSetIndex);
    NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);
    NodeID activeChildNodeID1 = net->getActiveChildNodeID(node->getNodeID(), 1);
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID1, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodePhysicsGrouperQueueUpdateTransformsPrePhysics(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();
  NMP_ASSERT(node->getNumChildNodes() == 2);

  uint32_t paramIndex = 5;
  uint32_t numParams = paramIndex;
  AttribDataPhysicsGrouperConfig* physicsGrouperConfig = getPhysicsGrouperConfig(net, node);
  doChildrenNeedConvertingToPhysics(net, node, physicsGrouperConfig->m_convertToPhysics[0], physicsGrouperConfig->m_convertToPhysics[1]);
  if (physicsGrouperConfig->m_convertToPhysics[0])
    numParams += 1; // output mask
  if (physicsGrouperConfig->m_convertToPhysics[1])
    numParams += 1; // output mask
  if (physicsGrouperConfig->m_convertToPhysics[0] || physicsGrouperConfig->m_convertToPhysics[1])
    numParams += 2; // for merged transforms and char controller
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_PHYSICSGROUPERUPDATETRANSFORMSPREPHYSICS,
                 node->getNodeID(),
                 numParams,
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
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);

    NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);
    NodeID activeChildNodeID1 = net->getActiveChildNodeID(node->getNodeID(), 1);

    net->TaskAddNetInputParam(task, 0, ATTRIB_SEMANTIC_PHYSICS_RIG, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);
    net->TaskAddNetInputParam(task, 1, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);
    net->TaskAddOutputParamZeroLifespan(task, 2, ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS, ATTRIB_TYPE_BOOL, INVALID_NODE_ID, currFrameNo);
    net->TaskAddNetInputParam(task, 3, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER);
    net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP, NETWORK_NODE_ID, activeAnimSetIndex);

    if (physicsGrouperConfig->m_convertToPhysics[0] || physicsGrouperConfig->m_convertToPhysics[1])
    {
      net->TaskAddParamAndDependency(task, paramIndex++, ATTRIB_SEMANTIC_MERGED_PHYSICS_RIG_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, NETWORK_NODE_ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddParamAndDependency(task, paramIndex++, ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_UPDATED, ATTRIB_TYPE_BOOL, NETWORK_NODE_ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    }
    if (physicsGrouperConfig->m_convertToPhysics[0])
    {
      net->TaskAddParamAndDependency(task, paramIndex++, ATTRIB_SEMANTIC_OUTPUT_MASK, ATTRIB_TYPE_BOOL_ARRAY, node->getNodeID(), activeChildNodeID0, TPARAM_FLAG_INPUT, currFrameNo);
    }
    if (physicsGrouperConfig->m_convertToPhysics[1])
    {
      net->TaskAddParamAndDependency(task, paramIndex++, ATTRIB_SEMANTIC_OUTPUT_MASK, ATTRIB_TYPE_BOOL_ARRAY, node->getNodeID(), activeChildNodeID1, TPARAM_FLAG_INPUT, currFrameNo);
    }
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodePhysicsGrouperQueueGetOutputMask(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();

  NMP_ASSERT(node->getNumChildNodes() == 2);

  NodeID targetNodeID = dependentParameter->m_attribAddress.m_targetNodeID;
  NMP_ASSERT(targetNodeID == node->getChildNodeID(0) || targetNodeID == node->getChildNodeID(1));

  TaskID taskID = 0;
  if (targetNodeID == node->getChildNodeID(0))
    taskID = CoreTaskIDs::MR_TASKID_PHYSICSGROUPERGETOUPUTMASKBASE;
  else
    taskID = CoreTaskIDs::MR_TASKID_PHYSICSGROUPERGETOUPUTMASKOVERRIDE;

  Task* task = queue->createNewTaskOnQueue(
                 taskID,
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
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);

    net->TaskAddDefInputParam(task, 0, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, node->getNodeID(), activeAnimSetIndex);
    NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());
    net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_OUTPUT_MASK, ATTRIB_TYPE_BOOL_ARRAY, activeParentNodeID, node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(task, 3, ATTRIB_SEMANTIC_OUTPUT_MASK, ATTRIB_TYPE_BOOL_ARRAY, targetNodeID, currFrameNo);
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodePhysicsGrouperFindGeneratingNodeForSemantic(
  NodeID              NMP_UNUSED(callingNodeID),
  bool                fromParent,   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            net)
{
  NodeID result = INVALID_NODE_ID;

  switch (semantic)
  {
  case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM:
  case ATTRIB_SEMANTIC_TRANSFORM_BUFFER:
  case ATTRIB_SEMANTIC_OUTPUT_MASK:
    result = node->getNodeID();
    break;
  default:
    // Pass on to parent or child appropriately.
    if (fromParent)
    {
      if (node->getNumChildNodes() != 0)
      {
        NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), node->getPassThroughChildIndex());
        result = net->nodeFindGeneratingNodeForSemantic(activeChildNodeID0, semantic, fromParent, node->getNodeID());
      }
    }
    else
    {
      NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());
      result = net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, semantic, fromParent, node->getNodeID());
    }
    break;
  }

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
