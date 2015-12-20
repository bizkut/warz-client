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
#include "GameAnimNetwork.h"
#include "GameCharacterController.h"
#include "comms/liveLinkNetworkManager.h"
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMFreelistMemoryAllocator.h"
#include "morpheme/mrBlendOps.h"
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
GameAnimNetwork* GameAnimNetwork::create(
  GameAnimNetworkDef*           networkDef,
  MR::AnimSetIndex              initialAnimSetIndex,
  GameCharacterControllerBase*  characterController,
  NMP::FastHeapAllocator*       tempAllocator)
{
  if (!networkDef || !characterController)
  {
    NMP_DEBUG_MSG("error: Valid network definition and controller expected!");
    return NULL;
  }

  // Create empty instance then fill in below.
  GameAnimNetwork* const instance = new GameAnimNetwork(networkDef);

  //--------------------------------------------------------------------------------------------------------------------
  // Create a dispatcher.
  MR::DispatcherBasic* const dispatcher = MR::DispatcherBasic::createAndInit();
  if (!dispatcher)
  {
    NMP_DEBUG_MSG("error: Failed to create dispatcher!");
    return NULL;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Initialize the temporary allocator, if one wasn't passed in already.
  instance->m_temporaryMemoryAllocator = NULL;
  NMP::FastHeapAllocator* initialTempAllocator = tempAllocator;
  if (!initialTempAllocator)
  {
    const uint32_t sizeInBytes = 512 * 1024;
    NMP::Memory::Format allocatorFormat = NMP::FastHeapAllocator::getMemoryRequirements(sizeInBytes, NMP_VECTOR_ALIGNMENT);
    NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(allocatorFormat);

    instance->m_temporaryMemoryAllocator = NMP::FastHeapAllocator::init(resource, sizeInBytes, NMP_VECTOR_ALIGNMENT);
    initialTempAllocator = instance->m_temporaryMemoryAllocator;
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Initialize the persistent allocator
  NMP::Memory::Format allocatorFormat = NMP::FreelistMemoryAllocator::getMemoryRequirements();
  NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(allocatorFormat);

  instance->m_persistentMemoryAllocator = NMP::FreelistMemoryAllocator::init(resource);

  //--------------------------------------------------------------------------------------------------------------------
  // Create the network instance.
  instance->m_network = MR::Network::createAndInit(networkDef->getNetworkDef(), dispatcher, initialTempAllocator, instance->m_persistentMemoryAllocator, 0);
  if (!instance->m_network)
  {
    dispatcher->releaseAndDestroy();
    NMP_DEBUG_MSG("error: Failed to create network instance!");
    return NULL;
  }

  instance->m_network->setCharacterController(characterController->getCharacterController());
  instance->m_network->setActiveAnimSetIndex(initialAnimSetIndex);

  //--------------------------------------------------------------------------------------------------------------------
  // Setup the required network outputs.
  //
  // A morpheme update step only evaluates the semantics requested on the root node. We request a transform buffer, the
  // trajectory change and event buffers but one could choose to only evaluate the trajectory for example if a character
  // is off-screen or only the transforms if the trajectory is controller by a server.
  const MR::NodeID rootNodeId = networkDef->getNetworkDef()->getRootNodeID();
  const uint16_t minLifespan = 1;
  instance->m_network->addPostUpdateAccessAttrib(rootNodeId, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, minLifespan);
  instance->m_network->addPostUpdateAccessAttrib(rootNodeId, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, minLifespan);
  instance->m_network->addPostUpdateAccessAttrib(rootNodeId, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, minLifespan);
  instance->m_network->addPostUpdateAccessAttrib(rootNodeId, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, minLifespan);

  //--------------------------------------------------------------------------------------------------------------------
  // Allocate a pose buffer.
  //
  // This buffer is used to store the final output of a network update for later use by other components of the game.
  // The buffer size is chosen to be able to hold any possible network output (the number of bones output in each frame
  // may change with the active animation set).
  const uint32_t maxBoneCount = networkDef->getMaxBoneCount();
  NMP::Memory::Format bufferFormat = NMP::DataBuffer::getPosQuatMemoryRequirements(maxBoneCount);
  NMP::Memory::Resource bufferResource = NMPMemoryAllocateFromFormat(bufferFormat);
  if (!bufferResource.ptr)
  {
    delete instance;
    NMP_DEBUG_MSG("error: Failed to allocate pose buffer!");
    return NULL;
  }

  instance->m_pose = NMP::DataBuffer::initPosQuat(bufferResource, bufferFormat, maxBoneCount);

  // ...
  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
GameAnimNetwork::~GameAnimNetwork()
{
  NMP::Memory::memFree(m_pose);

  if (m_network)
  {
#pragma region LiveLink
    if (MCOMMS::LiveLinkNetworkManager::getInstance())
    {
      MCOMMS::LiveLinkNetworkManager::getInstance()->revokeNetwork(m_network);
    }
#pragma endregion LiveLink

    m_network->getDispatcher()->releaseAndDestroy();
    m_network->releaseAndDestroy();
  }

  if (m_temporaryMemoryAllocator)
  {
    m_temporaryMemoryAllocator->term();
    NMP::Memory::memFree(m_temporaryMemoryAllocator);
  }

  if (m_persistentMemoryAllocator)
  {
    m_persistentMemoryAllocator->term();
    NMP::Memory::memFree(m_persistentMemoryAllocator);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool GameAnimNetwork::setActiveAnimSetIndex(MR::AnimSetIndex animationSetIndex)
{
  return m_network->setActiveAnimSetIndex(animationSetIndex);
}

//----------------------------------------------------------------------------------------------------------------------
bool GameAnimNetwork::broadcastRequestMessage(MR::MessageID messageID, bool status)
{
  MR::Message requestMessage(messageID, MESSAGE_TYPE_REQUEST, status, 0, 0);
  return m_network->broadcastMessage(requestMessage) > 0;
}

//----------------------------------------------------------------------------------------------------------------------
void GameAnimNetwork::setControlParameter(MR::NodeID owningNodeID, float value)
{
  MR::AttribDataFloat floatAttribData;
  floatAttribData.m_value = value;
  m_network->setControlParameter(owningNodeID, &floatAttribData);
}

//----------------------------------------------------------------------------------------------------------------------
void GameAnimNetwork::updatePose(const NMP::Matrix34& rootTransform)
{
  NMP::DataBuffer* const transforms = m_network->getTransforms();
  MR::AnimRigDef* const animRig = m_network->getActiveRig();
  NMP_ASSERT(transforms && animRig);

  MR::BlendOpsBase::accumulateTransforms(rootTransform.translation(), rootTransform.toQuat(), transforms, animRig, m_pose);
  m_animRig = animRig;
}

//----------------------------------------------------------------------------------------------------------------------
GameAnimNetwork::GameAnimNetwork(GameAnimNetworkDef* networkDef) :
  m_networkDef(networkDef),
  m_network(NULL),
  m_pose(NULL),
  m_animRig(NULL),
  m_temporaryMemoryAllocator(NULL),
  m_persistentMemoryAllocator(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
void GameAnimNetwork::runInitializingUpdateStep()
{
  MR::Task* task;
  MR::ExecuteResult execResult;

  m_network->startUpdate(0.0f, true);

  while ((execResult = m_network->update(task)) != MR::EXECUTE_RESULT_COMPLETE)
  {
    if (execResult != MR::EXECUTE_RESULT_IN_PROGRESS)
    {
      NMP_ASSERT(
        task->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER ||
        task->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEPHYSICS ||
        task->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEROOT);

      (void)task;
    }
  }

  m_network->getTempMemoryAllocator()->reset();
}
