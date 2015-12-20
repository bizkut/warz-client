// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "euphoria/Nodes/erNodePerformanceBehaviour.h"
#include "euphoria/erAttribData.h"
#include "euphoria/erCharacter.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCharacterControllerInterface.h"
#include "physics/mrPhysics.h"
#include "physics/mrPhysicsScene.h"
#include "physics/Nodes/mrNodePhysics.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace MR;

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
NodeID nodePerformanceBehaviourUpdateConnections(
  NodeDef*           node,
  Network*           net)
{
  NET_LOG_ENTER_FUNC();

  NetworkDef* netDef = net->getNetworkDef();
  NMP_ASSERT(netDef);

  PhysicsRig* physicsRig = getPhysicsRig(net);
  bool netHasPhysicsRig = (physicsRig != 0);

  // only try to control the root if we actually have access to a physical character
  Network::RootControlMethod rootControlMethod = Network::ROOT_CONTROL_ANIMATION;
  if (netHasPhysicsRig)
  {
    rootControlMethod = MR::Network::ROOT_CONTROL_PHYSICS;
  }
  net->setRootControlMethod(rootControlMethod);

  Dispatcher* dispatcher = net->getDispatcher();
  NMP_ASSERT(dispatcher);

  const FrameCount currentFrame = net->getCurrentFrameNo();
  NMP_ASSERT(currentFrame > 0);

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
  if (physicsStateBinEntry)
  {
    // Not the first update - just prepare the state for the next (this) frame
    physicsState = (AttribDataPhysicsState*)physicsStateBinEntry->m_attribDataHandle.m_attribData;
    NMP_ASSERT(physicsState);
    physicsState->m_previousDeltaTime = physicsState->m_deltaTime;
    physicsState->m_deltaTime = net->getLastUpdateTimeStep();
  }
  else
  {
    // First update for this physics node
    NMP::MemoryAllocator* allocator = dispatcher->getMemoryAllocator(LIFESPAN_FOREVER);
    NMP_ASSERT(allocator);

    const AnimRigDef* activeRigDef = net->getActiveRig();
    NMP_ASSERT(activeRigDef);
    uint32_t numAnimationJoints = activeRigDef->getNumBones();

    // create state
    NMP::Memory::Format stateMemReqs = AttribDataPhysicsState::getMemoryRequirements(numAnimationJoints);
    NMP::Memory::Resource stateResource = allocator->allocateFromFormat(stateMemReqs NMP_MEMORY_TRACKING_ARGS);
    NMP_ASSERT(stateResource.ptr);

    physicsState = AttribDataPhysicsState::init(stateResource, numAnimationJoints);
    // Store the allocator so we know where to free this attribData from when we destroy it.
    physicsState->m_allocator = allocator;

    // overwrite the attribute data for this nodes ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE
    AttribAddress stateAttribAddress(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
    AttribDataHandle stateAttribHandle = { physicsState, stateMemReqs };
    net->addAttribData(stateAttribAddress, stateAttribHandle, LIFESPAN_FOREVER);

    // create initialisation structures
    NMP::Memory::Format physicsInitMemReqs = AttribDataPhysicsInitialisation::getMemoryRequirements(numAnimationJoints);
    NMP::Memory::Resource physicsInitResource = allocator->allocateFromFormat(physicsInitMemReqs NMP_MEMORY_TRACKING_ARGS);
    NMP_ASSERT(physicsInitResource.ptr);

    AttribDataPhysicsInitialisation* physicsInit = AttribDataPhysicsInitialisation::init(physicsInitResource, numAnimationJoints);
    // Store the allocator so we know where to free this attribData from when we destroy it.
    physicsInit->m_allocator = allocator;

    // overwrite the attribute data for this nodes ATTRIB_SEMANTIC_PHYSICS_INITIALISATION
    AttribAddress physicsInitAttribAddress(ATTRIB_SEMANTIC_PHYSICS_INITIALISATION, node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
    AttribDataHandle physicsInitHandle = { physicsInit, physicsInitMemReqs };
    net->addAttribData(physicsInitAttribAddress, physicsInitHandle, LIFESPAN_FOREVER);
  }

  // make sure that if the network has had its physics rig removed then we will properly initialise
  // it when/if it comes back.
  if (!netHasPhysicsRig)
  {
    physicsState->m_haveAddedPhysicsRigRef = false;
  }

  if (physicsState && !physicsState->m_haveAddedPhysicsRigRef && netHasPhysicsRig)
  {
    // If the body is already referenced, just take it over which means it won't need to be initialised either.
    // If it isn't already referenced then the task code will handle initialisation.
    // MORPH-21378: If the task isn't going to initialise anything it doesn't need to be passed any initialisation data.
    if (physicsRig->isReferenced())
    {
      physicsRig->addReference();
      physicsState->m_haveAddedPhysicsRigRef = true;
    }

    physicsState->m_needToInitialiseRoot = true;

    NodeBinEntry* physicsInitBinEntry = net->getAttribDataNodeBinEntry(
      ATTRIB_SEMANTIC_PHYSICS_INITIALISATION,
      node->getNodeID(),
      INVALID_NODE_ID,
      VALID_FRAME_ANY_FRAME);
    NMP_ASSERT(physicsInitBinEntry);
    AttribDataPhysicsInitialisation* physicsInitialisation = (AttribDataPhysicsInitialisation*)physicsInitBinEntry->m_attribDataHandle.m_attribData;
    NMP_ASSERT(physicsInitialisation);

    physicsInitialisation->m_deltaTime = net->getLastUpdateTimeStep();

    AttribDataCharacterProperties* ccAttribData = net->getCharacterPropertiesAttribData();

    if (currentFrame > 1)
    {
      physicsInitialisation->m_worldRoot = ccAttribData->m_worldRootTransform;
      physicsInitialisation->m_prevWorldRoot = ccAttribData->m_prevWorldRootTransform;

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

      physicsInitialisation->m_worldRoot = ccAttribData->m_worldRootTransform;
      physicsInitialisation->m_prevWorldRoot = ccAttribData->m_prevWorldRootTransform;
      bindPoseBuffer->m_transformBuffer->copyTo(physicsInitialisation->m_transforms->m_transformBuffer);
      bindPoseBuffer->m_transformBuffer->copyTo(physicsInitialisation->m_prevTransforms->m_transformBuffer);
    }
  }

  // Also make sure we have the data structures for calling our input's CP update functions
  NodeBinEntry* performanceBehaviourBinEntry = net->getAttribDataNodeBinEntry(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2,
    node->getNodeID(),
    INVALID_NODE_ID,
    VALID_FRAME_ANY_FRAME);
  AttribDataPhysicalEffectData* performanceBehaviourData = 0;
  if (!performanceBehaviourBinEntry)
  {
    // Allocate the data
    NMP::MemoryAllocator* allocator = dispatcher->getMemoryAllocator(LIFESPAN_FOREVER);
    NMP_ASSERT(allocator);

    // create state
    NMP::Memory::Format stateMemReqs = AttribDataPhysicalEffectData::getMemoryRequirements();
    NMP::Memory::Resource stateResource = allocator->allocateFromFormat(stateMemReqs NMP_MEMORY_TRACKING_ARGS);
    NMP_ASSERT(stateResource.ptr);

    performanceBehaviourData = AttribDataPhysicalEffectData::init(stateResource, net, node);
    // Store the allocator so we know where to free this attribData from when we destroy it.
    performanceBehaviourData->m_allocator = allocator;

    // overwrite the attribute data
    AttribAddress stateAttribAddress(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2, 
      node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
    AttribDataHandle stateAttribHandle = { performanceBehaviourData, stateMemReqs };
    net->addAttribData(stateAttribAddress, stateAttribHandle, LIFESPAN_FOREVER);
  }

  NET_LOG_MESSAGE(98, "nodePerformanceBehaviourUpdateConnections network rootControlMethod = %d\n", 
    net->getRootControlMethod());

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodePerformanceBehaviourQueueUpdateTransformsPrePhysics(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();

  if (!getPhysicsRig(net))
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
  }

  uint32_t numTaskParams = 11;

  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_PERFORMANCEBEHAVIOURUPDATETRANSFORMSPREPHYSICS,
                 node->getNodeID(),
                 numTaskParams,
                 dependentParameter,
                 false,
                 false);
  if (task)
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);

    uint32_t taskIndex = 0;
    net->TaskAddDefInputParam(task, taskIndex++, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddNetInputParam(task, taskIndex++, ATTRIB_SEMANTIC_PHYSICS_RIG, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);
    net->TaskAddNetInputParam(task, taskIndex++, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);
    net->TaskAddOutputParamZeroLifespan(task, taskIndex++, ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS, ATTRIB_TYPE_BOOL, INVALID_NODE_ID, currFrameNo);
    net->TaskAddNetInputOutputParam(task, taskIndex++, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, ATTRIB_TYPE_PHYSICS_STATE, INVALID_NODE_ID, currFrameNo);
    net->TaskAddNetInputParam(task, taskIndex++, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2, node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
    net->TaskAddNetInputParam(task, taskIndex++, ATTRIB_SEMANTIC_PHYSICS_INITIALISATION, node->getNodeID(), MR::INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
    NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());
    net->TaskAddParamAndDependency(task, taskIndex++, ATTRIB_SEMANTIC_OUTPUT_MASK, ATTRIB_TYPE_BOOL_ARRAY, activeParentNodeID, node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddDefInputParam(task, taskIndex++, ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddDefInputParam(task,  taskIndex++, ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_DEF, NETWORK_NODE_ID);
    net->TaskAddNetInputParam(task, taskIndex++, ATTRIB_SEMANTIC_CHARACTER, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);

    NMP_ASSERT(taskIndex == numTaskParams);
  }

  return task;
}

}  // namespace ER

//----------------------------------------------------------------------------------------------------------------------

