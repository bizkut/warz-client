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
#include "euphoria/Nodes/erNodeBehaviour.h"
#include "euphoria/erAttribData.h"
#include "euphoria/erCharacter.h"
#include "euphoria/erBody.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCharacterControllerAttribData.h"
#include "physics/mrPhysics.h"
#include "physics/mrPhysicsScene.h"
#include "physics/Nodes/mrNodePhysics.h"
#include "physics/mrPhysicsTasks.h"
//----------------------------------------------------------------------------------------------------------------------

// Disable warnings constant conditionals
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4127)
#endif

using namespace MR;

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
static void updateControlParameters(NodeDef* node, Network* net)
{
  // Update the list of input control parameters specified within the AttribDataBehaviourParameters.
  for (MR::PinIndex i = 0 ; i < node->getNumInputCPConnections() ; ++i)
  {
    const CPConnection* inputCPConnection =  node->getInputCPConnection(i);
    if (inputCPConnection->m_sourceNodeID != INVALID_NODE_ID)
    {
      MR::AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

      net->updateInputCPConnection(inputCPConnection, animSet);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBehaviourUpdateConnections(
  NodeDef*           node,
  Network*           net)
{
  NET_LOG_ENTER_FUNC();

  // Note that input CPs will be updated at the end - i.e. if possible after the physics node has
  // been initialised (assuming it's been assigned to the network).
  NetworkDef* netDef = net->getNetworkDef();
  NMP_ASSERT(netDef);

  PhysicsRig* physicsRig = getPhysicsRig(net);
  bool netHasPhysicsRig = (physicsRig != 0);

  // get the physics setup attrib data
  AttribDataHandle* nodeSpecificHandle = netDef->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
  NMP_ASSERT(nodeSpecificHandle);
  AttribDataBehaviourSetup* behaviourSetup = (AttribDataBehaviourSetup*)nodeSpecificHandle->m_attribData;
  NMP_ASSERT(behaviourSetup);

  // only try to control the root if we actually have access to a physical character
  Network::RootControlMethod rootControlMethod = Network::ROOT_CONTROL_ANIMATION;
  if (netHasPhysicsRig)
  {
    rootControlMethod = MR::Network::ROOT_CONTROL_PHYSICS;
  }
  net->setRootControlMethod(rootControlMethod);

  Dispatcher* dispatcher = net->getDispatcher();
  NMP_ASSERT(dispatcher);
  NMP::MemoryAllocator* allocatorPriority = dispatcher->getMemoryAllocator(LIFESPAN_FOREVER);
  NMP_ASSERT(allocatorPriority);

  int32_t currentPriority = 0;

  const FrameCount currentFrame = net->getCurrentFrameNo();
  NMP_ASSERT(currentFrame > 0);

  // The network priority counter can't use LIFESPAN_FOREVER because it's value needs to be reset every frame
  const AttribAddress priorityCounterAttribAddress(
    ATTRIB_SEMANTIC_CP_INT,
    NETWORK_NODE_ID,
    INVALID_NODE_ID,
    currentFrame);

  // Global priority counter
  NodeBinEntry* attribIntCounter = net->getAttribDataNodeBinEntry(priorityCounterAttribAddress);

  if (!attribIntCounter)
  {
    // Create the global priority counter for this current frame
    NMP::Memory::Format priorityCounterMemReqs = AttribDataInt::getMemoryRequirements();
    NMP::Memory::Resource priorityCounterResource = NMPAllocatorAllocateFromFormat(allocatorPriority, priorityCounterMemReqs);
    NMP_ASSERT(priorityCounterResource.ptr);

    AttribDataInt* priorityCounterAttribData = AttribDataInt::init(priorityCounterResource, 1);
    priorityCounterAttribData->m_allocator = allocatorPriority;
    AttribDataHandle handle = { priorityCounterAttribData, MR::AttribDataInt::getMemoryRequirements() };
    net->addAttribData(priorityCounterAttribAddress, handle, 1);

    attribIntCounter = net->getAttribDataNodeBinEntry(priorityCounterAttribAddress);
  }
  else
  {
    // Take the global priority counter value for this current frame
    AttribDataInt* priorityCounter = (AttribDataInt*)attribIntCounter->m_attribDataHandle.m_attribData;
    currentPriority = priorityCounter->m_value;
    // Increment global priority counter
    priorityCounter->m_value += 1;
  }
  NMP_ASSERT(attribIntCounter);

  // Behaviour priority
  const AttribAddress priorityAttribAddress(ATTRIB_SEMANTIC_CP_INT, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER);
  NodeBinEntry* binEntry = net->findIgnoringValidFrame(priorityAttribAddress);
  // The bin entry won't exist on the first frame this node is active
  if (binEntry)
  {
    MR::AttribDataInt* priorityData = (MR::AttribDataInt*)binEntry->m_attribDataHandle.m_attribData;
    NMP_ASSERT(priorityData->getType() == MR::AttribDataInt::getDefaultType());
    priorityData->m_value = currentPriority;
  }
  else
  {
    AttribDataHandle handle = AttribDataInt::create(net->getPersistentMemoryAllocator(), currentPriority, 0);
    net->addAttribData(priorityAttribAddress, handle, LIFESPAN_FOREVER);
  }

  // Update the hightest_priority attribute for this specific behaviour
  Character* character = networkGetCharacter(net);
  if (character)
    character->setHighestPriority(behaviourSetup->m_behaviourID, currentPriority);

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

  NodeBinEntry* behaviourStateBinEntry = net->getAttribDataNodeBinEntry(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2,
    node->getNodeID(),
    INVALID_NODE_ID,
    VALID_FRAME_ANY_FRAME);
  AttribDataBehaviourState* behaviourState = 0;

  if (physicsStateBinEntry)
  {
    // Not the first update - just prepare the state for the next (this) frame
    physicsState = static_cast<AttribDataPhysicsState*>(physicsStateBinEntry->m_attribDataHandle.m_attribData);
    NMP_ASSERT(physicsState);
    physicsState->m_previousDeltaTime = physicsState->m_deltaTime;
    physicsState->m_deltaTime = net->getLastUpdateTimeStep();

    behaviourState = static_cast<AttribDataBehaviourState*>(behaviourStateBinEntry->m_attribDataHandle.m_attribData);
    NMP_ASSERT(behaviourState);
  }
  else
  {
    // Starting the behaviour and creating the physics/behaviour state should always occur together
    // or not at all. The function nodeBehaviourDeleteInstance() assumes that if a physics state
    // exists then a behaviour has been started (so should be stopped).

    // First update for this physics node.
    NMP::MemoryAllocator* allocator = dispatcher->getMemoryAllocator(LIFESPAN_FOREVER);
    NMP_ASSERT(allocator);

    // Get number of animation joints.
    const AnimRigDef* activeRigDef = net->getActiveRig();
    NMP_ASSERT(activeRigDef);
    uint32_t numAnimationJoints = activeRigDef->getNumBones();

    {
      // Create physics state.

      NMP::Memory::Format stateMemReqs = 
        AttribDataPhysicsState::getMemoryRequirements(numAnimationJoints);
      NMP::Memory::Resource stateResource = NMPAllocatorAllocateFromFormat(allocator, stateMemReqs);
      NMP_ASSERT(stateResource.ptr);

      physicsState = 
        AttribDataPhysicsState::init(stateResource, numAnimationJoints);
      // Store the allocator so we know where to free this attribData from when we destroy it.
      physicsState->m_allocator = allocator;

      // overwrite the attribute data for this nodes ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE
      AttribAddress physicsStateAttribAddress(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
      AttribDataHandle physicsStateAttribHandle = { physicsState, stateMemReqs };
      net->addAttribData(physicsStateAttribAddress, physicsStateAttribHandle, LIFESPAN_FOREVER);
    }

    {
      // Create behaviour state.

      // Get size of CP params arrays.
      AttribDataBehaviourParameters* behaviourParameters = node->getAttribData<AttribDataBehaviourParameters>(
        ATTRIB_SEMANTIC_BEHAVIOUR_PARAMETERS);
      NMP_ASSERT(behaviourParameters);
      uint32_t numInputCPInts = behaviourParameters->m_numInputCPInts;
      uint32_t numInputCPFloats = behaviourParameters->m_numInputCPFloats;
      uint32_t numInputCPUInt64s = behaviourParameters->m_numInputCPUInt64s;
      uint32_t numVector3s = behaviourParameters->m_numInputCPVector3s;

      NMP::Memory::Format stateMemReqs = 
        AttribDataBehaviourState::getMemoryRequirements(numInputCPInts, numInputCPFloats, numInputCPUInt64s, numVector3s);
      NMP::Memory::Resource stateResource = allocator->allocateFromFormat(stateMemReqs NMP_MEMORY_TRACKING_ARGS);
      NMP_ASSERT(stateResource.ptr);

      behaviourState = AttribDataBehaviourState::init(
        stateResource, numInputCPInts, numInputCPFloats, numInputCPUInt64s, numVector3s);
      // Store the allocator so we know where to free this attribData from when we destroy it.
      behaviourState->m_allocator = allocator;

      AttribAddress behaviourStateAttribAddress(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2, 
        node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
      AttribDataHandle behaviourStateAttribHandle = { behaviourState, stateMemReqs };
      net->addAttribData(behaviourStateAttribAddress, behaviourStateAttribHandle, LIFESPAN_FOREVER);
    }


    // create initialisation structures
    NMP::Memory::Format physicsInitMemReqs = AttribDataPhysicsInitialisation::getMemoryRequirements(numAnimationJoints);
    NMP::Memory::Resource physicsInitResource = NMPAllocatorAllocateFromFormat(allocator, physicsInitMemReqs);
    NMP_ASSERT(physicsInitResource.ptr);

    AttribDataPhysicsInitialisation* physicsInit = AttribDataPhysicsInitialisation::init(physicsInitResource, numAnimationJoints);
    // Store the allocator so we know where to free this attribData from when we destroy it.
    physicsInit->m_allocator = allocator;

    // overwrite the attribute data for this nodes ATTRIB_SEMANTIC_PHYSICS_INITIALISATION
    AttribAddress physicsInitAttribAddress(ATTRIB_SEMANTIC_PHYSICS_INITIALISATION, node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
    AttribDataHandle physicsInitHandle = { physicsInit, physicsInitMemReqs };
    net->addAttribData(physicsInitAttribAddress, physicsInitHandle, LIFESPAN_FOREVER);
  }

  // Start the behaviour if necessary - and if there's no character then make sure it starts when there is
  if (character)
  {
    if (!behaviourState->m_startedBehaviour)
    {
      character->startBehaviour(behaviourSetup->m_behaviourID);
      behaviourState->m_startedBehaviour = true;
    }
  }
  else
  {
    behaviourState->m_startedBehaviour = false;
  }

  if (physicsState && !physicsState->m_haveAddedPhysicsRigRef)
  {
    if (netHasPhysicsRig)
    {
      if (!physicsRig->isReferenced())
      {
        PhysicsInitialisation physicsInitialisation;
        physicsInitialisation.m_deltaTime = net->getLastUpdateTimeStep();

        AttribDataCharacterProperties* charControllerAttrib = net->getCharacterPropertiesAttribData();
        physicsInitialisation.m_worldRoot = charControllerAttrib->m_worldRootTransform;
        physicsInitialisation.m_prevWorldRoot = charControllerAttrib->m_prevWorldRootTransform;

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
            physicsInitialisation.m_prevWorldRoot = physicsInitialisation.m_worldRoot;
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

          physicsInitialisation.m_transforms = prevDataBuffer;
          physicsInitialisation.m_prevTransforms = prevPrevDataBuffer;
        }
        else
        {
          AnimRigDef* activeRigDef = net->getActiveRig();
          NMP_ASSERT(activeRigDef);

          const AttribDataTransformBuffer* bindPoseBuffer = activeRigDef->getBindPose();
          NMP_ASSERT(bindPoseBuffer);

          physicsInitialisation.m_transforms = bindPoseBuffer->m_transformBuffer;
          physicsInitialisation.m_prevTransforms = bindPoseBuffer->m_transformBuffer;
        }
        // This adds the reference
        initialisePhysics(physicsInitialisation, physicsRig, physicsState, net->getDispatcher());

        // Initialise the data in erBody so that operator nodes can use it
        if (character)
        {
          character->getBody().postPhysicsStep(net->getLastUpdateTimeStep());
        }
      }
      else
      {
        physicsRig->addReference();
        physicsState->m_haveAddedPhysicsRigRef = true;
      }
    }
    else
    {
      // We don't have a physics rig assigned to the network at the moment but one may be provided
      // once the task runs.
      // MORPH-21378: If the task isn't going to initialise anything it doesn't need to be passed any initialisation data.
      physicsState->m_needToInitialiseRoot = true;
      physicsState->m_haveAddedPhysicsRigRef = false;

      NodeBinEntry* physicsInitBinEntry = net->getAttribDataNodeBinEntry(
        ATTRIB_SEMANTIC_PHYSICS_INITIALISATION,
        node->getNodeID(),
        INVALID_NODE_ID,
        VALID_FRAME_ANY_FRAME);
      NMP_ASSERT(physicsInitBinEntry);
      AttribDataPhysicsInitialisation* physicsInitialisation = (AttribDataPhysicsInitialisation*)physicsInitBinEntry->m_attribDataHandle.m_attribData;
      NMP_ASSERT(physicsInitialisation);

      physicsInitialisation->m_deltaTime = net->getLastUpdateTimeStep();

      AttribDataCharacterProperties* charControllerAttrib = net->getCharacterPropertiesAttribData();
      physicsInitialisation->m_worldRoot = charControllerAttrib->m_worldRootTransform;
      physicsInitialisation->m_prevWorldRoot = charControllerAttrib->m_prevWorldRootTransform;

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
  }

  updateControlParameters(node, net);

  NET_LOG_MESSAGE(98, "nodeBehaviourUpdateConnections network rootControlMethod = %d\n", net->getRootControlMethod());

  // Recurse to children.
  int numChildren = net->getNumActiveChildren(node->getNodeID());
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());
  for (int i = 0 ; i < numChildren ; ++i)
  {
    NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), i);
    NMP_ASSERT(activeChildNodeID != INVALID_NODE_ID);
    net->updateNodeInstanceConnections(activeChildNodeID, animSet);
  }

  return node->getNodeID();
}


//----------------------------------------------------------------------------------------------------------------------
Task* nodeBehaviourQueueUpdateTransformsPrePhysics(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();

  NMP_ASSERT(node->getAttribData<AttribDataBehaviourParameters>(ATTRIB_SEMANTIC_BEHAVIOUR_PARAMETERS,
    net->getActiveAnimSetIndex())); // Ensure behaviourParams exist

  uint32_t numTaskParams = 14 + node->getNumChildNodes();

  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_BEHAVIOURUPDATETRANSFORMSPREPHYSICS,
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
    net->TaskAddNetInputParam(task, taskIndex++, ATTRIB_SEMANTIC_CHARACTER, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);
    net->TaskAddOutputParamZeroLifespan(task, taskIndex++, ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS, ATTRIB_TYPE_BOOL, INVALID_NODE_ID, currFrameNo);
    net->TaskAddDefInputParam(task, taskIndex++, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    net->TaskAddNetInputParam(task, taskIndex++, ATTRIB_SEMANTIC_PHYSICS_INITIALISATION, node->getNodeID(), MR::INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
    NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());
    net->TaskAddParamAndDependency(task, taskIndex++, ATTRIB_SEMANTIC_OUTPUT_MASK, ATTRIB_TYPE_BOOL_ARRAY, activeParentNodeID, node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);

    net->TaskAddOptionalDefInputParam(task, taskIndex++, ATTRIB_SEMANTIC_BEHAVIOUR_PARAMETERS, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddNetInputOutputParam(task, taskIndex++, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, ATTRIB_TYPE_PHYSICS_STATE, INVALID_NODE_ID, currFrameNo);
    net->TaskAddNetInputParam(task, taskIndex++, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2, node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
    net->TaskAddDefInputParam(task, taskIndex++, ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP, NETWORK_NODE_ID, activeAnimSetIndex);

    net->TaskAddDefInputParam(task,  taskIndex++, ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_DEF, NETWORK_NODE_ID);

    // m_priority
    net->TaskAddNetInputParam(task, taskIndex++, ATTRIB_SEMANTIC_CP_INT, node->getNodeID(), INVALID_NODE_ID, currFrameNo);

    // Add the children added as a result of behaviour anim inputs - note that all these children
    // will be valid - the list of all child (including invalid) node IDs is stored in the setup
    // data. However, we still might not add them if the behaviour doesn't actually want the data.
    for (uint32_t i = 0 ; i < node->getNumChildNodes(); ++i)
    {
      NodeID activeChildNodeID = node->getChildNodeID(i);

      NMP_ASSERT(activeChildNodeID != INVALID_NODE_ID);
      // get the physics setup attrib data
      AttribDataHandle* nodeSpecificHandle = net->getNetworkDef()->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
      NMP_ASSERT(nodeSpecificHandle);
      AttribDataBehaviourSetup* behaviourSetup = (AttribDataBehaviourSetup*)nodeSpecificHandle->m_attribData;
      NMP_ASSERT(behaviourSetup);

      // Test to see if the behavior wants the anim input at all.
      ER::Character* character = networkGetCharacter(net);
      bool requireTMs = false;
      if (character)
      {
        requireTMs = character->isAnimationRequiredByBehaviour(behaviourSetup->m_behaviourID, i);
      }

      // If we don't need the TMs, we set the node ID to invalid so it's transforms are not computed in the morpheme network
      if (!requireTMs)
      {
        activeChildNodeID = INVALID_NODE_ID;
      }

      if (activeChildNodeID == INVALID_NODE_ID)
      {
        net->TaskAddInputParamWithOptionallyInvalidOwningNode(task, taskIndex++, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID, currFrameNo);
      }
      else
      {
        net->TaskAddParamAndDependency(task, taskIndex++, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      }
    }

    NodeBinEntry* const behaviourStateBinEntry = net->getAttribDataNodeBinEntry(
      ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2,
      node->getNodeID(),
      INVALID_NODE_ID,
      VALID_FRAME_ANY_FRAME,
      activeAnimSetIndex);
    NMP_ASSERT(behaviourStateBinEntry);

    AttribDataBehaviourState* const behaviourState = behaviourStateBinEntry->getAttribData<AttribDataBehaviourState>();
    NMP_ASSERT(behaviourState);

    AttribDataBehaviourParameters* behaviourParameters =
      node->getAttribData<AttribDataBehaviourParameters>(
      ATTRIB_SEMANTIC_BEHAVIOUR_PARAMETERS,
      net->getActiveAnimSetIndex());
    NMP_ASSERT(behaviourParameters);
    NMP_ASSERT(behaviourParameters->m_controlParameterInputsTypes->m_numValues == node->getNumInputCPConnections());

    // note: The CP default attributes always appear at the beginning of the inputCpParameters.
    uint32_t intParamIndex = 0;
    uint32_t fltParamIndex = 0;
    uint32_t uint64ParamIndex = 0;
    uint32_t vector3ParamIndex = 0; 

    for (uint32_t i = 0; i < node->getNumInputCPConnections(); ++i)
    {
      // Pass any input control parameters into the task.
      const CPConnection* const inputCPConnection = node->getInputCPConnection(static_cast<MR::PinIndex>(i));
      const MR::AttribDataSemantic semantic = static_cast< MR::AttribDataSemantic >(behaviourParameters->m_controlParameterInputsTypes->m_values[i]);
      const bool isConnected = inputCPConnection->m_sourceNodeID != INVALID_NODE_ID;

      // cast to value type
      switch(semantic)
      {
      case ATTRIB_SEMANTIC_CP_BOOL:
        if (isConnected)
        {
          AttribDataBool* attrib = net->updateOptionalInputCPConnection<AttribDataBool>(inputCPConnection, activeAnimSetIndex);
          behaviourState->m_ints->m_values[intParamIndex] = (attrib->m_value) ? 1 : 0;
        }

        ++intParamIndex;
        break;

      case ATTRIB_SEMANTIC_CP_UINT:
        if (isConnected)
        {
          AttribDataUInt* attrib = net->updateOptionalInputCPConnection<AttribDataUInt>(inputCPConnection, activeAnimSetIndex);
          behaviourState->m_ints->m_values[intParamIndex] = attrib->m_value;
        }

        ++intParamIndex;
        break;

      case ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER:
        if (isConnected)
        {
          AttribDataPhysicsObjectPointer* attrib = net->updateOptionalInputCPConnection<AttribDataPhysicsObjectPointer>(inputCPConnection, activeAnimSetIndex);
          behaviourState->m_uint64s->m_values[uint64ParamIndex] = reinterpret_cast<size_t>(attrib->m_value);
        }

        ++uint64ParamIndex;
        break;

      case ATTRIB_SEMANTIC_CP_INT:
        if (isConnected)
        {
          AttribDataInt* attrib = net->updateOptionalInputCPConnection<AttribDataInt>(inputCPConnection, activeAnimSetIndex);
          behaviourState->m_ints->m_values[intParamIndex] = attrib->m_value;
        }

        ++intParamIndex;
        break;

      case ATTRIB_SEMANTIC_CP_FLOAT:
        if (isConnected)
        {
          AttribDataFloat* attrib = net->updateOptionalInputCPConnection<AttribDataFloat>(inputCPConnection, activeAnimSetIndex);
          behaviourState->m_floats->m_values[fltParamIndex] = attrib->m_value;
        }

        ++fltParamIndex;
        break;

      case ATTRIB_SEMANTIC_CP_VECTOR3:
        if (isConnected)
        {
          AttribDataVector3* attrib = net->updateOptionalInputCPConnection<AttribDataVector3>(inputCPConnection, activeAnimSetIndex);

          behaviourState->m_vector3Data->m_values[vector3ParamIndex]     = attrib->m_value[0];
          behaviourState->m_vector3Data->m_values[vector3ParamIndex + 1] = attrib->m_value[1];
          behaviourState->m_vector3Data->m_values[vector3ParamIndex + 2] = attrib->m_value[2];
        }

        vector3ParamIndex += 4;
        break;

      case ATTRIB_SEMANTIC_CP_VECTOR4:
        if (isConnected)
        {
          AttribDataVector4* attrib = net->updateOptionalInputCPConnection<AttribDataVector4>(inputCPConnection, activeAnimSetIndex);

          behaviourState->m_floats->m_values[fltParamIndex]     = attrib->m_value[0];
          behaviourState->m_floats->m_values[fltParamIndex + 1] = attrib->m_value[1];
          behaviourState->m_floats->m_values[fltParamIndex + 2] = attrib->m_value[2];
          behaviourState->m_floats->m_values[fltParamIndex + 2] = attrib->m_value[3];
        }

        fltParamIndex += 4;
        break;

      default:
        NMP_ASSERT_FAIL(); // unrecognized attrib type
        break;
      }
    }
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeBehaviorOutputCPUpdate(MR::NodeDef* node,
                                       MR::PinIndex outputCPPinIndex,
                                       MR::Network* net)
{
  AttribDataBehaviourSetup* behaviourSetup =
    node->getAttribData<AttribDataBehaviourSetup>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  AttribDataBehaviourParameters* behaviourParameters =
    node->getAttribData<AttribDataBehaviourParameters>(
      ATTRIB_SEMANTIC_BEHAVIOUR_PARAMETERS,
      net->getActiveAnimSetIndex());
  NMP_ASSERT(behaviourParameters);

  MR::FrameCount currentFrameNo = net->getCurrentFrameNo();

  uint32_t numOCPs = behaviourParameters->m_controlParameterOutputsTypes->m_numValues;

  AttribDataBehaviourParameters::OutputControlParam* outputControlParams =
    (AttribDataBehaviourParameters::OutputControlParam*) alloca(numOCPs * sizeof(AttribDataBehaviourParameters::OutputControlParam));

  MR::NodeBin* nodeBin = net->getNodeBin(node->getNodeID());

  OutputCPPin* outputCPPinRequested = nodeBin->getOutputCPPin(outputCPPinIndex);
  AttribData* outputCPAttribForRequestedPin = outputCPPinRequested->getAttribData();

  // Initialise the attribute data for all output CP pins on this node
  for (uint8_t iPin = 0 ; iPin < numOCPs ; ++iPin)
  {
    OutputCPPin* outputCPPin = nodeBin->getOutputCPPin(iPin);

    // Create the output attribute if it doesn't already exist, then set the pointers up to point to
    // the attrib data
    outputControlParams[iPin].type = (AttribDataSemanticEnum)
      behaviourParameters->m_controlParameterOutputsTypes->m_values[iPin];
    switch (behaviourParameters->m_controlParameterOutputsTypes->m_values[iPin])
    {    
    case ATTRIB_SEMANTIC_CP_BOOL:
      if (!outputCPPin->m_attribDataHandle.m_attribData)
        outputCPPin->m_attribDataHandle = AttribDataBool::create(net->getPersistentMemoryAllocator());
      outputControlParams[iPin].data = &outputCPPin->getAttribData<AttribDataBool>()->m_value;
      break;
    case ATTRIB_SEMANTIC_CP_UINT:
      if (!outputCPPin->m_attribDataHandle.m_attribData)
        outputCPPin->m_attribDataHandle = AttribDataUInt::create(net->getPersistentMemoryAllocator());
      outputControlParams[iPin].data = &outputCPPin->getAttribData<AttribDataUInt>()->m_value;
      break;
    case ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER:
      if (!outputCPPin->m_attribDataHandle.m_attribData)
        outputCPPin->m_attribDataHandle = AttribDataPhysicsObjectPointer::create(net->getPersistentMemoryAllocator(), 0);
      outputControlParams[iPin].data = &outputCPPin->getAttribData<AttribDataPhysicsObjectPointer>()->m_value;
      break;
    case ATTRIB_SEMANTIC_CP_INT:
      if (!outputCPPin->m_attribDataHandle.m_attribData)
        outputCPPin->m_attribDataHandle = AttribDataInt::create(net->getPersistentMemoryAllocator());
      outputControlParams[iPin].data = &outputCPPin->getAttribData<AttribDataInt>()->m_value;
      break;
    case ATTRIB_SEMANTIC_CP_FLOAT:
      if (!outputCPPin->m_attribDataHandle.m_attribData)
        outputCPPin->m_attribDataHandle = AttribDataFloat::create(net->getPersistentMemoryAllocator());
      outputControlParams[iPin].data = &outputCPPin->getAttribData<AttribDataFloat>()->m_value;
      break;
    case ATTRIB_SEMANTIC_CP_VECTOR3:
      if (!outputCPPin->m_attribDataHandle.m_attribData)
        outputCPPin->m_attribDataHandle = AttribDataVector3::create(net->getPersistentMemoryAllocator());
      outputControlParams[iPin].data = &outputCPPin->getAttribData<AttribDataVector3>()->m_value;
      break;
    case ATTRIB_SEMANTIC_CP_VECTOR4:
      if (!outputCPPin->m_attribDataHandle.m_attribData)
        outputCPPin->m_attribDataHandle = AttribDataVector4::create(net->getPersistentMemoryAllocator());
      outputControlParams[iPin].data = &outputCPPin->getAttribData<AttribDataVector4>()->m_value;
      break;
    default:
      NMP_ASSERT_FAIL();
      outputControlParams[iPin].data = 0;
      break;
    }

    outputCPPin->m_lastUpdateFrame = currentFrameNo;
  }

  // Get Euphoria to set that data. If there's no character just use the default values.
  Character* character = networkGetCharacter(net);
  if (character)
  {
    character->handleOutputControlParams(behaviourSetup->m_behaviourID, outputControlParams, numOCPs);
  }

  // We return the address of the attrib data /before/ the update, so that we return NULL if the attribute
  // was only just created.  This indicates that the attribute has not been computed yet (which it won't,
  // because for behaviour nodes this happens during the main euphoria feedback step).
  return outputCPAttribForRequestedPin;
}

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeBehaviourEmitMessageUpdate(MR::NodeDef* nodeDef, MR::PinIndex NMP_UNUSED(outputCPPinIndex), MR::Network* net)
{
  // Do nothing unless the node is active and we have access to the euphoria character
  //
  if ( !net->nodeIsActive(nodeDef->getNodeID()) )
  {
    return NULL;
  }
  Character* character = networkGetCharacter(net);
  if (!character)
  {
    return NULL;
  }

  // Get messages from the character and clear the emit request mask 
  // provided this behaviour node is the one with highest priority
  //
  uint32_t messages = 0;
  AttribAddress priorityCounterAttribAddress(
    ATTRIB_SEMANTIC_CP_INT, nodeDef->getNodeID(), INVALID_NODE_ID, net->getCurrentFrameNo());
  NodeBinEntry* attribIntCounter = net->getAttribDataNodeBinEntry(priorityCounterAttribAddress);
  if (attribIntCounter)
  {
    // Read this node's priority counter
    AttribDataInt* priorityCounter = (AttribDataInt*)attribIntCounter->m_attribDataHandle.m_attribData;
    if (priorityCounter)
    {
      // If it's the highest priority node with this behaviour id get the message(s)
       AttribDataBehaviourSetup* behaviourSetup = 
         nodeDef->getAttribData<AttribDataBehaviourSetup>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
      if ( priorityCounter->m_value == character->getHighestPriority(behaviourSetup->m_behaviourID) )
      {
        character->handleEmitMessages(behaviourSetup->m_behaviourID, messages);
      }
    }
  }

  // Send the messages if we got them
  //
  if (messages != 0)
  {
    // Get any emitted messages that may have already been created by other tasks this frame.
    AttribDataEmittedMessages* emittedMessagesInputAttrib = net->getOptionalAttribData<AttribDataEmittedMessages>(
      ATTRIB_SEMANTIC_EMITTED_MESSAGES,
      nodeDef->getNodeID(),
      nodeDef->getNodeID(),
      net->getCurrentFrameNo());
    if (!emittedMessagesInputAttrib)
    {
      // It does not exist yet so create it.
      AttribDataHandle handle = AttribDataEmittedMessages::create(net->getPersistentMemoryAllocator());
      emittedMessagesInputAttrib = (AttribDataEmittedMessages*)(handle.m_attribData);
      AttribAddress address(ATTRIB_SEMANTIC_EMITTED_MESSAGES, nodeDef->getNodeID(), nodeDef->getNodeID(), net->getCurrentFrameNo());
      net->addAttribData(address, handle, 1);
    }

    // Emit the m_messages stored in the behaviour
    emittedMessagesInputAttrib->m_value |= messages;
  }


  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
static Task* nodeBehaviourQueueUpdatePhysicalTrajectoryPostPhysics(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();

  uint32_t numParams = 5;

  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_BEHAVIOURUPDATEPHYSICALTRAJECTORYPOSTPHYSICS,
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
    NMP_ASSERT(currFrameNo > 0);

    net->TaskAddNetInputParam(task, 0, ATTRIB_SEMANTIC_PHYSICS_RIG, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);
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
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
static Task* nodeBehaviourQueueUpdateAnimatedTrajectory(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NET_LOG_ENTER_FUNC();

  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_BEHAVIOURUPDATEANIMATEDTRAJECTORY,
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

    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, INVALID_NODE_ID, currFrameNo);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBehaviourQueueUpdateTrajectory(
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
    return nodeBehaviourQueueUpdatePhysicalTrajectoryPostPhysics(
      node,
      queue,
      net,
      dependentParameter);
  }
  else
  {
    // Just copies child transforms and makes the physics body as not being controlled by physics
    return nodeBehaviourQueueUpdateAnimatedTrajectory(
      node,
      queue,
      net,
      dependentParameter);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBehaviourQueueUpdateTransformsPostPhysics(
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
    if (entry)
    {
      AttribDataPhysicsState* physicsState = entry->getAttribData<AttribDataPhysicsState>();
      NMP_ASSERT(physicsState);
      physicsState->m_haveAddedPhysicsRigRef = false;
    }

    // Just output the bind pose since no child input is really relevant. Could output the default
    // pose instead?
    Task* t = nodePhysicsQueueBindPoseTransforms(node, queue, net, dependentParameter);
    if (t)
      return t;
  }

  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_BEHAVIOURUPDATETRANSFORMSPOSTPHYSICS,
    node->getNodeID(),
    6,
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
    AnimSetIndex activeAnimSetIndex = net->getActiveAnimSetIndex();
    NMP_ASSERT(currFrameNo > 0);

    net->TaskAddDefInputParam(task, 0, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddNetInputParam(task, 1, ATTRIB_SEMANTIC_PHYSICS_RIG, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);
    net->TaskAddNetInputParam(task, 2, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);
    net->TaskAddOutputParamZeroLifespan(task, 3, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS, ATTRIB_TYPE_BOOL, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Make sure we wait until the whole network trajectory has been evaluated
    net->TaskAddParamAndDependency(task, 5, ATTRIB_SEMANTIC_ROOT_UPDATED, ATTRIB_TYPE_BOOL, NETWORK_NODE_ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeID nodeBehaviourFindGeneratingNodeForSemantic(
  MR::NodeID              NMP_UNUSED(callingNodeID),
  bool                    fromParent,   // Was this query from a parent or child node.
  MR::AttribDataSemantic  semantic,
  MR::NodeDef*            node,
  MR::Network*            net)
{
  NodeID result = INVALID_NODE_ID;

  //  ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE owned by this node but no specific queuing function for it?
  switch (semantic)
  {
  case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM:
  case ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS:
  case ATTRIB_SEMANTIC_TRANSFORM_BUFFER:
  case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER:
    {
      result = node->getNodeID();
      break;
    }
  default:
    {
      // Pass on only for requests from the child.
      if (!fromParent)
      {
        NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());
        result = net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, semantic, fromParent, node->getNodeID());
      }
      break;
    }
  }

  return result;
}


}  // namespace ER

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------

