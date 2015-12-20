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
#include "physics/Nodes/mrNodePhysics.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "physics/mrPhysics.h"
#include "morpheme/mrCharacterControllerInterface.h"
#include "physics/mrPhysicsScene.h"
//----------------------------------------------------------------------------------------------------------------------

// Disable warnings constant conditionals
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4127)
#endif

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
void updateControlParameters(NodeDef* node, Network* net)
{
  NMP_ASSERT(net->getCurrentFrameNo() > 0);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());
  net->updateOptionalInputCPConnection<AttribDataFloat>(node->getInputCPConnection(0), animSet); // inputCPFloatKWeight
  net->updateOptionalInputCPConnection<AttribDataFloat>(node->getInputCPConnection(1), animSet); // inputCPFloatAAWeight
  // Note - don't update the PhysicalEffect CP as that's done in the task
}

//----------------------------------------------------------------------------------------------------------------------  
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
NodeID nodePhysicsUpdateConnections(
  NodeDef*           node,
  Network*           net)
{
  NET_LOG_ENTER_FUNC();

  updateControlParameters(node, net);

  NetworkDef* netDef = net->getNetworkDef();
  NMP_ASSERT(netDef);

  PhysicsRig* physicsRig = getPhysicsRig(net);
  bool netHasPhysicsRig = (physicsRig != 0 && physicsRig->getAnimToPhysicsMap());

  // Get the physics setup attrib data.
  AttribDataPhysicsSetup* physicsSetup = node->getAttribData<AttribDataPhysicsSetup>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  
  // only try to control the root if we actually have access to a physical character
  Network::RootControlMethod rootControlMethod = Network::ROOT_CONTROL_ANIMATION;
  if (netHasPhysicsRig)
  {
    rootControlMethod = (Network::RootControlMethod)physicsSetup->m_rootControlMethod;
  }
  net->setRootControlMethod(rootControlMethod);

  Dispatcher* dispatcher = net->getDispatcher();
  NMP_ASSERT(dispatcher);
  NMP::MemoryAllocator* allocator = dispatcher->getMemoryAllocator(LIFESPAN_FOREVER);
  NMP_ASSERT(allocator);     

  // Prepare the physics state/initialisation structures.
  // Do this - in particular the adding reference to physics - here so that it is done before any existing physics
  // node gets deleted - so we get a smooth handover from the old to new physics node without need for
  // re-initialisation.
  NodeBinEntry* physicsStateBinEntry = net->getAttribDataNodeBinEntry(
                                         ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
                                         node->getNodeID(),
                                         INVALID_NODE_ID,
                                         VALID_FRAME_ANY_FRAME);
  AttribDataPhysicsState* physicsState = 0;

  AnimSetIndex animSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

  if (physicsStateBinEntry)
  {
    if (physicsStateBinEntry->m_address.m_animSetIndex != animSetIndex)
    {
      NodeBinEntry* physicsInitBinEntry = net->getAttribDataNodeBinEntry(
                                          ATTRIB_SEMANTIC_PHYSICS_INITIALISATION,
                                          node->getNodeID(),
                                          INVALID_NODE_ID,
                                          VALID_FRAME_ANY_FRAME);

      NMP_ASSERT(physicsInitBinEntry->m_address.m_animSetIndex == physicsStateBinEntry->m_address.m_animSetIndex);
      // First update for this physics node
      if (netHasPhysicsRig)
      {
        const AnimRigDef* activeRigDef = net->getActiveRig();
        NMP_ASSERT(activeRigDef);
        uint32_t numAnimationJoints = activeRigDef->getNumBones();

        // create state
        NMP::Memory::Format stateMemReqs = AttribDataPhysicsState::getMemoryRequirements(numAnimationJoints);
        NMP::Memory::Resource stateResource = NMPAllocatorAllocateFromFormat(allocator, stateMemReqs);
        NMP_ASSERT(stateResource.ptr);

        physicsState = AttribDataPhysicsState::init(stateResource, numAnimationJoints, 1);
        // Store the allocator so we know where to free this attribData from when we destroy it.
        physicsState->m_allocator = allocator;
        physicsState->m_deltaTime = physicsStateBinEntry->getAttribData<AttribDataPhysicsState>()->m_deltaTime;
        physicsState->m_previousDeltaTime = physicsStateBinEntry->getAttribData<AttribDataPhysicsState>()->m_previousDeltaTime;
        physicsState->m_updateCounter = physicsStateBinEntry->getAttribData<AttribDataPhysicsState>()->m_updateCounter;
        physicsState->m_haveAddedPhysicsRigRef = physicsStateBinEntry->getAttribData<AttribDataPhysicsState>()->m_haveAddedPhysicsRigRef;
        physicsState->m_needToInitialiseRoot = physicsStateBinEntry->getAttribData<AttribDataPhysicsState>()->m_needToInitialiseRoot;

        net->getNetworkDef()->mapCopyTransformBuffers(physicsStateBinEntry->getAttribData<AttribDataPhysicsState>()->m_previousChildTransforms->m_transformBuffer, physicsStateBinEntry->m_address.m_animSetIndex, physicsState->m_previousChildTransforms->m_transformBuffer, animSetIndex);

        physicsStateBinEntry->removeReference();
        physicsStateBinEntry->m_address.m_animSetIndex = animSetIndex;
        physicsStateBinEntry->m_attribDataHandle.m_attribData = physicsState;
        physicsStateBinEntry->m_attribDataHandle.m_format = stateMemReqs;

        // create initialisation structures
        NMP::Memory::Format physicsInitMemReqs = AttribDataPhysicsInitialisation::getMemoryRequirements(numAnimationJoints);
        NMP::Memory::Resource physicsInitResource = NMPAllocatorAllocateFromFormat(allocator, physicsInitMemReqs);
        NMP_ASSERT(physicsInitResource.ptr);

        AttribDataPhysicsInitialisation* physicsInit = AttribDataPhysicsInitialisation::init(physicsInitResource, numAnimationJoints, 1);
        // Store the allocator so we know where to free this attribData from when we destroy it.
        physicsInit->m_allocator = allocator;
        physicsInit->m_worldRoot = physicsInitBinEntry->getAttribData<AttribDataPhysicsInitialisation>()->m_worldRoot;
        physicsInit->m_prevWorldRoot = physicsInitBinEntry->getAttribData<AttribDataPhysicsInitialisation>()->m_prevWorldRoot;
        physicsInit->m_deltaTime = physicsInitBinEntry->getAttribData<AttribDataPhysicsInitialisation>()->m_deltaTime;

        net->getNetworkDef()->mapCopyTransformBuffers(physicsInitBinEntry->getAttribData<AttribDataPhysicsInitialisation>()->m_transforms->m_transformBuffer, physicsInitBinEntry->m_address.m_animSetIndex, physicsInit->m_transforms->m_transformBuffer, animSetIndex);
        net->getNetworkDef()->mapCopyTransformBuffers(physicsInitBinEntry->getAttribData<AttribDataPhysicsInitialisation>()->m_prevTransforms->m_transformBuffer, physicsInitBinEntry->m_address.m_animSetIndex, physicsInit->m_prevTransforms->m_transformBuffer, animSetIndex);

        physicsInitBinEntry->removeReference();
        physicsInitBinEntry->m_address.m_animSetIndex = animSetIndex;
        physicsInitBinEntry->m_attribDataHandle.m_attribData = physicsInit;
        physicsInitBinEntry->m_attribDataHandle.m_format = physicsInitMemReqs;
      }
    }
    
    // Not the first update - just prepare the state for the next (this) frame
    physicsState = physicsStateBinEntry->getAttribData<AttribDataPhysicsState>();
    NMP_ASSERT(physicsState);
    physicsState->m_previousDeltaTime = physicsState->m_deltaTime;
    physicsState->m_deltaTime = net->getLastUpdateTimeStep();

  }
  else
  {
    // First update for this physics node
    if (netHasPhysicsRig)
    {
      const AnimRigDef* activeRigDef = net->getActiveRig();
      NMP_ASSERT(activeRigDef);
      uint32_t numAnimationJoints = activeRigDef->getNumBones();

      // create state
      NMP::Memory::Format stateMemReqs = AttribDataPhysicsState::getMemoryRequirements(numAnimationJoints);
      NMP::Memory::Resource stateResource = NMPAllocatorAllocateFromFormat(allocator, stateMemReqs);
      NMP_ASSERT(stateResource.ptr);

      physicsState = AttribDataPhysicsState::init(stateResource, numAnimationJoints);
      // Store the allocator so we know where to free this attribData from when we destroy it.
      physicsState->m_allocator = allocator;

      // overwrite the attribute data for this nodes ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE
      AttribAddress stateAttribAddress(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME, animSetIndex);
      AttribDataHandle stateAttribHandle = { physicsState, stateMemReqs };
      net->addAttribData(stateAttribAddress, stateAttribHandle, LIFESPAN_FOREVER);

      // create initialisation structures
      NMP::Memory::Format physicsInitMemReqs = AttribDataPhysicsInitialisation::getMemoryRequirements(numAnimationJoints);
      NMP::Memory::Resource physicsInitResource = NMPAllocatorAllocateFromFormat(allocator, physicsInitMemReqs);
      NMP_ASSERT(physicsInitResource.ptr);

      AttribDataPhysicsInitialisation* physicsInit = AttribDataPhysicsInitialisation::init(physicsInitResource, numAnimationJoints);
      // Store the allocator so we know where to free this attribData from when we destroy it.
      physicsInit->m_allocator = allocator;

      // overwrite the attribute data for this nodes ATTRIB_SEMANTIC_PHYSICS_INITIALISATION
      AttribAddress physicsInitAttribAddress(ATTRIB_SEMANTIC_PHYSICS_INITIALISATION, node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME, animSetIndex);
      AttribDataHandle physicsInitHandle = { physicsInit, physicsInitMemReqs };
      net->addAttribData(physicsInitAttribAddress, physicsInitHandle, LIFESPAN_FOREVER);
    }
  }

  if (physicsState && !physicsState->m_haveAddedPhysicsRigRef && netHasPhysicsRig)
  {
    // If the body is already referenced, just take it over which means it won't need to be initialised either.
    // If it isn't already referenced then the task code will handle initialisation.
    if (physicsRig->isReferenced())
    {
      physicsRig->addReference();
      physicsState->m_haveAddedPhysicsRigRef = true;
    }

    physicsState->m_needToInitialiseRoot = (physicsSetup->m_rootControlMethod == Network::ROOT_CONTROL_PHYSICS);

    NodeBinEntry* physicsInitBinEntry = net->getAttribDataNodeBinEntry(
                                          ATTRIB_SEMANTIC_PHYSICS_INITIALISATION,
                                          node->getNodeID(),
                                          INVALID_NODE_ID,
                                          VALID_FRAME_ANY_FRAME);
    NMP_ASSERT(physicsInitBinEntry);
    AttribDataPhysicsInitialisation* physicsInitialisation = physicsInitBinEntry->getAttribData<AttribDataPhysicsInitialisation>();
    NMP_ASSERT(physicsInitialisation);

    physicsInitialisation->m_deltaTime = net->getLastUpdateTimeStep();

    AttribDataCharacterProperties* charControllerAttrib = net->getCharacterPropertiesAttribData();
    physicsInitialisation->m_worldRoot = charControllerAttrib->m_worldRootTransform;
    physicsInitialisation->m_prevWorldRoot = charControllerAttrib->m_prevWorldRootTransform;

    FrameCount currentFrame = net->getCurrentFrameNo();
    if (currentFrame > 1)
    {
      FrameCount prevFrame = currentFrame - 1;
      FrameCount prevPrevFrame = prevFrame;
      if (currentFrame > 2)
      {
        prevPrevFrame = currentFrame - 2;
      }
      else
      {
        physicsInitialisation->m_prevWorldRoot = physicsInitialisation->m_worldRoot;
      }

      // Find a parent state machine or the network that can provide us with two transform buffers with which to
      // initialise physics.
      NodeID parentSubNetworkID = node->getNodeID();
      const NMP::DataBuffer* prevDataBuffer = 0;
      const NMP::DataBuffer* prevPrevDataBuffer = 0;
      do
      {
        parentSubNetworkID = getParentStateMachineOrNetworkRootNodeID(netDef->getNodeDef(parentSubNetworkID), net);
        prevDataBuffer = getNodeTransformsBuffer(parentSubNetworkID, net, prevFrame, net->getOutputAnimSetIndex(parentSubNetworkID));
        prevPrevDataBuffer = getNodeTransformsBuffer(parentSubNetworkID, net, prevPrevFrame, net->getOutputAnimSetIndex(parentSubNetworkID));
      } while (parentSubNetworkID != netDef->getRootNodeID() && (prevDataBuffer == 0 || prevPrevDataBuffer == 0));

      // if there was no previous frame then use the bind pose
      if (prevDataBuffer == 0)
      {
        const AnimRigDef* activeRigDef = net->getActiveRig();
        const AttribDataTransformBuffer* bindPoseBuffer = activeRigDef->getBindPose();
        prevDataBuffer = bindPoseBuffer->m_transformBuffer;
      }
      // if there was no frame before the previous frame then use the previous frame
      if (prevPrevDataBuffer == 0)
      {
        prevPrevDataBuffer = prevDataBuffer;
      }

      prevDataBuffer->copyTo(physicsInitialisation->m_transforms->m_transformBuffer);
      prevPrevDataBuffer->copyTo(physicsInitialisation->m_prevTransforms->m_transformBuffer);            
    }
    else
    {
      AnimRigDef* activeRigDef = net->getActiveRig();
      NMP_ASSERT(activeRigDef);

      const AttribDataTransformBuffer* bindPoseBuffer = activeRigDef->getBindPose();
      NMP_ASSERT(bindPoseBuffer);

      bindPoseBuffer->m_transformBuffer->copyTo(physicsInitialisation->m_transforms->m_transformBuffer);
      bindPoseBuffer->m_transformBuffer->copyTo(physicsInitialisation->m_prevTransforms->m_transformBuffer);
    }
  }

  // Also make sure we have the data structures for calling our input's CP update functions, if we
  // have a connection.
  if (node->getInputCPConnectionSourceNodeID(2) != INVALID_NODE_ID)
  {
    NodeBinEntry* physicalEffectDataBinEntry = net->getAttribDataNodeBinEntry(
      ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2,
      node->getNodeID(),
      INVALID_NODE_ID,
      VALID_FRAME_ANY_FRAME);
    AttribDataPhysicalEffectData* physicalEffectData = 0;
    if (!physicalEffectDataBinEntry)
    {
      // Create state
      NMP::Memory::Format stateMemReqs = AttribDataPhysicalEffectData::getMemoryRequirements();
      NMP::Memory::Resource stateResource = allocator->allocateFromFormat(stateMemReqs NMP_MEMORY_TRACKING_ARGS);
      NMP_ASSERT(stateResource.ptr);

      physicalEffectData = AttribDataPhysicalEffectData::init(stateResource, net, node);
      // Store the allocator so we know where to free this attribData from when we destroy it.
      physicalEffectData->m_allocator = allocator;

      // overwrite the attribute data
      AttribAddress stateAttribAddress(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2, 
        node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
      AttribDataHandle stateAttribHandle = { physicalEffectData, stateMemReqs };
      net->addAttribData(stateAttribAddress, stateAttribHandle, LIFESPAN_FOREVER);
    }
  }

  NET_LOG_MESSAGE(98, "nodePhysicsUpdateConnections network rootControlMethod = %d\n", net->getRootControlMethod());

  // Check we are above any retargeting
  const AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());
  if (animSet != net->getActiveAnimSetIndex())
  {
    NET_LOG_ERROR_MESSAGE("Closest anim nodes must be above any retarget nodes in the network.\n");
  }

  // Recurse to children.
  int numChildren = net->getNumActiveChildren(node->getNodeID());
  for (int i = 0 ; i < numChildren ; ++i)
  {
    NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), i);
    NMP_ASSERT(activeChildNodeID != INVALID_NODE_ID);
    net->updateNodeInstanceConnections(activeChildNodeID, animSet);
  }

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodePhysicsQueueUpdateTrajectory(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();

  NMP_ASSERT(node->getAttribData(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

  // depending on the network control method call the appropriate queuing function
  if (net->getRootControlMethod() == Network::ROOT_CONTROL_PHYSICS)
  {
    // physics controlled root - post physics trajectory task depends on physics having been stepped.
    return nodePhysicsQueueUpdatePhysicalTrajectoryPostPhysics(
             node,
             queue,
             net,
             dependentParameter);
  }
  else
  {
    // Just copies child transforms and makes the physics body as not being controlled by physics
    return nodePhysicsQueueUpdateAnimatedTrajectory(
             node,
             queue,
             net,
             dependentParameter);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodePhysicsQueueUpdateAnimatedTrajectory(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();

  const uint32_t numTaskParams( (net->getNumActiveChildren(node->getNodeID()) != 0) ? 4 : 3 );

  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_PHYSICSUPDATEANIMATEDTRAJECTORY,
                 node->getNodeID(),
                 numTaskParams,
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

    const AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

    net->TaskAddDefInputParam(task, 0, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(task, 2, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, INVALID_NODE_ID, currFrameNo);
    // This can get called even if we're ragdoll if there's no physical body, or it is partial-body ragdoll
    if (net->getNumActiveChildren(node->getNodeID()) != 0)
    {
      NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
      net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    }
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodePhysicsQueueUpdatePhysicalTrajectoryPostPhysics(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();

  uint32_t numParams = 7;

  AttribDataPhysicsSetup* physicsSetup = node->getAttribData<AttribDataPhysicsSetup>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  bool queueChildTrajectory = 
    physicsSetup->m_method == AttribDataPhysicsSetup::METHOD_LOCAL_SK && 
    net->getNumActiveChildren(node->getNodeID()) != 0;
  if (queueChildTrajectory)
    numParams += 1;

  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_PHYSICSUPDATEPHYSICALTRAJECTORYPOSTPHYSICS,
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

    net->TaskAddNetInputParam(task, 0, ATTRIB_SEMANTIC_PHYSICS_RIG, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    net->TaskAddNetInputParam(task, 1, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_DEF, NETWORK_NODE_ID);
    net->TaskAddOutputParamZeroLifespan(task, 3, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, INVALID_NODE_ID, currFrameNo);
    // during transition to this node then we want to blend the root according to the transition weighting,
    // so find the appropriate transition node, if it exists. If not, then  get a default value from ourselves
    bool foundStateMachine;
    NodeID parentSubNetworkID = getParentStateMachineOrNetworkRootNodeID(node, net, &foundStateMachine);
    if (!foundStateMachine)
    {
      net->TaskAddInputParam(task, 4, ATTRIB_SEMANTIC_BLEND_WEIGHTS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_OPTIONAL, currFrameNo);
    }
    else
    {
      const NodeConnections* nc = net->getActiveNodesConnections(parentSubNetworkID);
      NMP_ASSERT(nc->m_numActiveChildNodes == 1);
      NodeID transitNodeID = nc->m_activeChildNodeIDs[0];
      NodeDef* transitNodeDef = net->getNetworkDef()->getNodeDef(transitNodeID);
      if (transitNodeDef->getNodeFlags() & NodeDef::NODE_FLAG_IS_TRANSITION)
        net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_BLEND_WEIGHTS, ATTRIB_TYPE_BLEND_WEIGHTS, transitNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT | TPARAM_FLAG_OPTIONAL, currFrameNo);
      else
        net->TaskAddInputParam(task, 4, ATTRIB_SEMANTIC_BLEND_WEIGHTS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_OPTIONAL, currFrameNo);
    }

    // Child animation trajectory is needed for LocalSK
    net->TaskAddDefInputParam(task, 5, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    net->TaskAddDefInputParam(task, 6, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, node->getNodeID(), activeAnimSetIndex);
    // This can get called even if we're ragdoll if there's no physical body, or it is partial-body ragdoll
    if (queueChildTrajectory)
    {
      NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
      net->TaskAddParamAndDependency(task, 7, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    }
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodePhysicsQueueUpdateTransformsPrePhysics(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();

  AttribDataPhysicsSetup* physicsSetup = node->getAttribData<AttribDataPhysicsSetup>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  int32_t extraInputs = 0;
  if (physicsSetup->m_hasAnimationChild)
  {
    extraInputs = 2; // direct input and the merged input
  }

  if (net->getRootControlMethod() != Network::ROOT_CONTROL_PHYSICS)
  {
    // Depend on the character controller update OR use the network trajectory output
    ++extraInputs;
  }

  uint32_t numTaskParams = 14 + extraInputs;

  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_PHYSICSUPDATETRANSFORMSPREPHYSICS,
                 node->getNodeID(),
                 numTaskParams,
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

    NMP_ASSERT_MSG(activeAnimSetIndex == net->getActiveAnimSetIndex(),
      "Physics Nodes must operate in the same anim set as the root of the network!");

    net->TaskAddDefInputParam(task, 0, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddNetInputParam(task, 1, ATTRIB_SEMANTIC_PHYSICS_RIG, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    net->TaskAddNetInputParam(task, 2, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(task, 3, ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS, ATTRIB_TYPE_BOOL, INVALID_NODE_ID, currFrameNo);
    net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    net->TaskAddDefInputParam(task, 5, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddOptionalNetInputOutputParam(task, 6, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, ATTRIB_TYPE_PHYSICS_STATE, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    net->TaskAddOptionalNetInputParam(task, 7, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2, node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
    if (node->getInputCPConnectionSourceNodeID(0) != INVALID_NODE_ID)
    {
      net->TaskAddInputCP(task, 8, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(0));
    }
    else
    {
      net->TaskAddDefInputParam(task, 8, ATTRIB_SEMANTIC_CP_FLOAT, node->getNodeID());
    }
    if (node->getInputCPConnectionSourceNodeID(1) != INVALID_NODE_ID)
    {
      net->TaskAddInputCP(task, 9, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(1));
    }
    else
    {
      net->TaskAddDefInputParam(task, 9, ATTRIB_SEMANTIC_CP_FLOAT, node->getNodeID());
    }
    net->TaskAddNetInputParam(task, 10, ATTRIB_SEMANTIC_PHYSICS_INITIALISATION, node->getNodeID(), MR::INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
    NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());
    net->TaskAddParamAndDependency(task, 11, ATTRIB_SEMANTIC_OUTPUT_MASK, ATTRIB_TYPE_BOOL_ARRAY, activeParentNodeID, node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);

    net->TaskAddDefInputParam(task, 12, ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP, NETWORK_NODE_ID, activeAnimSetIndex);

    net->TaskAddDefInputParam(task, 13, ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_DEF, NETWORK_NODE_ID);

    // Rather than depending on our child directly, go through the network-owned task that merges all our child inputs.
    // Also supply the raw child input in case the node wants to "use root as anchor"
    uint32_t taskIndex(14);
    if (physicsSetup->m_hasAnimationChild)
    {
      NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
      net->TaskAddParamAndDependency(task, taskIndex++, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddParamAndDependency(task, taskIndex++, ATTRIB_SEMANTIC_MERGED_PHYSICS_RIG_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, NETWORK_NODE_ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    }

    if (net->getRootControlMethod() != Network::ROOT_CONTROL_PHYSICS)
    {
      if (net->getCharacterPropertiesPhysicsAndCharacterControllerUpdate() != PHYSICS_AND_CC_UPDATE_SEPARATE)
      {
        // If using keyframing and a single physics/character controller update, then the transform
        // update will/may want the network's trajectory delta to help predict the root position
        // when using a single physics/character controller update
        net->TaskAddParamAndDependency(task, taskIndex++, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, net->getNetworkDef()->getRootNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      }
      else
      {
        // Separate physics/character controller updates, so depend on the controller update
        net->TaskAddParamAndDependency(task, taskIndex++, ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_UPDATED, ATTRIB_TYPE_BOOL, NETWORK_NODE_ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      }
    }

    NMP_ASSERT(taskIndex == numTaskParams);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodePhysicsQueueBindPoseTransforms(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_GETBINDPOSETRANSFORMS,
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
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);
    net->TaskAddDefInputParam(task, 0, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodePhysicsQueueUpdateTransformsNoPhysics(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();
  AttribDataPhysicsSetup* physicsSetup;
  {
    AttribDataHandle* handle = net->getNetworkDef()->getAttribDataHandle(
                                 ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,
                                 node->getNodeID());
    NMP_ASSERT(handle);
    physicsSetup = (AttribDataPhysicsSetup*) handle->m_attribData;
    NMP_ASSERT(physicsSetup);
  }

  // If there's a child animation, just pass it through.
  if (physicsSetup->m_hasAnimationChild)
  {
    return queuePassThroughChild0(node, queue, net, dependentParameter);
  }
  else
  {
    return nodePhysicsQueueBindPoseTransforms(node, queue, net, dependentParameter);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodePhysicsQueueUpdateTransformsPostPhysics(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();

  PhysicsRig* physicsRig = getPhysicsRig(net);
  bool netHasPhysicsRig = (physicsRig != 0 && physicsRig->getAnimToPhysicsMap());

  if (!netHasPhysicsRig)
  {
    // if the rig got removed, make sure that when it comes back we will initialise it
    NodeBinEntry* entry = net->getAttribDataNodeBinEntry(
                            ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
                            node->getNodeID(),
                            INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
    AttribDataPhysicsState* physicsState = 0;
    if (entry)
    {
      physicsState = entry->getAttribData<AttribDataPhysicsState>();
      NMP_ASSERT(physicsState);
      physicsState->m_haveAddedPhysicsRigRef = false;
    }

    // Try to queue pass-through
    Task* t = nodePhysicsQueueUpdateTransformsNoPhysics(node, queue, net, dependentParameter);
    // if we can just pass through then do so. Otherwise carry on a queue tasks that will make some sensible default output
    if (t)
      return t;
  }

  const uint32_t numTaskParams( (net->getNumActiveChildren(node->getNodeID()) != 0) ? 9 : 8 );

  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_PHYSICSUPDATETRANSFORMSPOSTPHYSICS,
                 node->getNodeID(),
                 numTaskParams,
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
    net->TaskAddNetInputParam(task, 1, ATTRIB_SEMANTIC_PHYSICS_RIG, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    net->TaskAddNetInputParam(task, 2, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);
    net->TaskAddOutputParamZeroLifespan(task, 3, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS, ATTRIB_TYPE_BOOL, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Make sure we wait until the whole network trajectory has been evaluated
    net->TaskAddParamAndDependency(task, 5, ATTRIB_SEMANTIC_ROOT_UPDATED, ATTRIB_TYPE_BOOL, NETWORK_NODE_ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddDefInputParam(task, 6, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    net->TaskAddDefInputParam(task, 7, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET, node->getNodeID(), activeAnimSetIndex);

    // this must be the last param since it's optional
    if (net->getNumActiveChildren(node->getNodeID()) != 0)
    {
      NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
      net->TaskAddParamAndDependency(task, 8, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    }
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodePhysicsFindGeneratingNodeForSemantic(
  NodeID              NMP_UNUSED(callingNodeID),
  bool                fromParent,   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            net)
{
  NodeID result = INVALID_NODE_ID;
  NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());

  //  ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE owned by this node but no specific queuing function for it?
  switch (semantic)
  {
  case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM:
  {
    if (net->getRootControlMethod() != Network::ROOT_CONTROL_PHYSICS)
    {
      if (fromParent)
      {
        if (net->getNumActiveChildren(node->getNodeID()) != 0)
        {
          NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
          result = net->nodeFindGeneratingNodeForSemantic(activeChildNodeID, semantic, fromParent, node->getNodeID());
        }
      }
      else
      {
        result = net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, semantic, fromParent, node->getNodeID());
      }
    }
    break;
  }
  case ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS:
  case ATTRIB_SEMANTIC_TRANSFORM_BUFFER:
  case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER:
  {
    result = node->getNodeID();
    break;
  }
  default:
  {
    // Pass on to parent or child appropriately.
    if (fromParent)
    {
      if (net->getNumActiveChildren(node->getNodeID()) != 0)
      {
        NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
        result = net->nodeFindGeneratingNodeForSemantic(activeChildNodeID, semantic, fromParent, node->getNodeID());
      }
    }
    else
    {
      result = net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, semantic, fromParent, node->getNodeID());
    }
    break;
  }
  }

  return result;
}

}  // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------

