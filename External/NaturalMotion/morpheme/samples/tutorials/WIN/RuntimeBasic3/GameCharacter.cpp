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
#include "GameCharacter.h"

#include "GameCharacterManager.h"
#include "morpheme/mrBlendOps.h"
#include "NMPlatform/NMFreelistMemoryAllocator.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

  
//----------------------------------------------------------------------------------------------------------------------
CharacterBasic* CharacterBasic::create(CharacterDefBasic* networkDef)
{
  //----------------------------
  // Make sure the networkDef and charCtrl have been initialised
  if(!networkDef)
  {
    NMP_DEBUG_MSG("error: Invalid Character Definition!");
    return NULL;
  }

  //----------------------------
  // Create and initialise an instance of our character
  CharacterBasic* const instance = static_cast<CharacterBasic*>(NMPMemoryAlloc(sizeof(CharacterBasic)));
  new(instance) CharacterBasic(networkDef);

  //----------------------------
  // Initialise Game Character, allocate memory etc
  NMP_STDOUT("Initialise Game Character");
  if (!instance->init(networkDef))
  {
    CharacterBasic::destroy(instance);
    NMP_DEBUG_MSG("error: Failed to create Game Character!");
    return NULL;
  }

  //----------------------------
  // Create the Game Character Controller and initialise it
  NMP_STDOUT("Creating Character Controller");
  CharacterControllerBasic* const characterController = CharacterControllerBasic::create();
  if (!characterController)
  {
    CharacterControllerBasic::destroy(characterController);
    CharacterBasic::destroy(instance);
    NMP_DEBUG_MSG("error: Failed to create Character Controller!");
    return NULL;
  }

  //----------------------------
  // Set the character controller's network and set the network's character controller.
  characterController->init(instance->getNetwork());
  instance->m_net->setCharacterController(characterController);

  //----------------------------
  // Set the default active anim set - here we only have 1 to choose.
  instance->m_net->setActiveAnimSetIndex(0);

  //----------------------
  // Perform an initial update of the network with start time of zero
  NMP_STDOUT("Run initial update step");
  instance->runInitialisingUpdateStep();

  NMP_STDOUT("GameCharacter successfully created");

  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterBasic::destroy(CharacterBasic* character)
{
  //----------------------------
  // First destroy the character controller
  CharacterControllerBasic* characterController = static_cast<CharacterControllerBasic*>(character->getNetwork()->getCharacterController());
  if (characterController)
  {
    CharacterControllerBasic::destroy(characterController);
    character->getNetwork()->setCharacterController(NULL);
  }

  //----------------------------
  // Release any memory allocated in our instance
  character->term();

  //----------------------------
  // Release our instance
  NMP::Memory::memFree(character);
  character = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterBasic::updateWorldTransforms(const NMP::Matrix34& rootTransform)
{
  NMP::DataBuffer* const transforms = m_net->getTransforms();
  MR::AnimRigDef* const animRig = m_net->getActiveRig();
  NMP_ASSERT(transforms && animRig);

  // if sourceBuffer may not be full, use applyBindPoseToUnusedChannels before calling accumulateTransforms
  // alternately, accumulateTransformsPartial can be used
  MR::BlendOpsBase::applyBindPoseToUnusedChannels(animRig->getBindPose(), transforms);
  MR::BlendOpsBase::accumulateTransforms(rootTransform.translation(), rootTransform.toQuat(), transforms, animRig, m_worldTransforms);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterBasic::runInitialisingUpdateStep()
{
  MR::Task* task;
  MR::ExecuteResult execResult;

  m_net->startUpdate(0.0f, true);

  while ((execResult = m_net->update(task)) != MR::EXECUTE_RESULT_COMPLETE)
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

  m_net->getTempMemoryAllocator()->reset();
}

//----------------------------------------------------------------------------------------------------------------------
// Update the morpheme network (this GameCharacter instance)
bool CharacterBasic::update(float timeDelta)
{
  m_net->startUpdate(timeDelta);

  MR::Task* task = NULL;
  MR::ExecuteResult execResult;
  do
  {
    //----------------------------
    // Update the network
    execResult = m_net->update(task);
  } while (execResult == MR::EXECUTE_RESULT_IN_PROGRESS);

  // Finalize post network update
  m_net->endUpdate();

  //----------------------------
  // Update the characterController
  CharacterControllerBasic* characterController = static_cast<CharacterControllerBasic*>(m_net->getCharacterController());
  NMP_ASSERT(characterController);

  characterController->updateTransform();

  //----------------------------
  // Reset the temporary allocator
  m_net->getTempMemoryAllocator()->reset();

  //----------------------------
  // Accumulate the transforms after the update
  updateWorldTransforms(m_net->getCharacterPropertiesWorldRootTransform());

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterBasic::init(CharacterDefBasic* characterDef)
{
  m_characterDef = characterDef;
  MR::NetworkDef *networkDef = m_characterDef->getNetworkDef();

  //----------------------------
  // Set the temporary memory size to 512KB. This should be large enough to cope with most networks for now but to
  // accurately determine the temp memory size used, see FastHeapAllocator::PrintAllocations().
  NMP_STDOUT("Allocate required memory");
  uint32_t tempDataSize = (512 * 1024);
  NMP::Memory::Format allocatorFormat = NMP::FastHeapAllocator::getMemoryRequirements(tempDataSize, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(allocatorFormat);

  //----------------------------
  // Setting the temp memory size here and passing it to MR::Network::createAndInit will override the default 2MB
  // temporary memory per network
  m_temporaryMemoryAllocator = NMP::FastHeapAllocator::init(resource, tempDataSize, NMP_VECTOR_ALIGNMENT);

  //----------------------------
  // Initialise the persistent allocator
  allocatorFormat = NMP::FreelistMemoryAllocator::getMemoryRequirements();
  resource = NMPMemoryAllocateFromFormat(allocatorFormat);

  m_persistentMemoryAllocator = NMP::FreelistMemoryAllocator::init(resource);

  //----------------------------
  // Create a dispatcher.
  MR::DispatcherBasic* const dispatcher = MR::DispatcherBasic::createAndInit();
  if (!dispatcher)
  {
    NMP_DEBUG_MSG("error: Failed to create dispatcher!");
    return NULL;
  }

  //----------------------------
  // Create the network instance.
  //
  // Create and initialise the network instance for this morpheme character instance.
  NMP_STDOUT("Creating MR::Network");
  m_net = MR::Network::createAndInit(networkDef,
                                dispatcher,
                                m_temporaryMemoryAllocator,
                                m_persistentMemoryAllocator,
                                0);

  if (!m_net)
  {
    dispatcher->releaseAndDestroy();
    NMP_DEBUG_MSG("error: Failed to create network instance!");
    return NULL;
  }

  //----------------------------
  // Setup the required network outputs.
  //
  // A morpheme update step only evaluates the semantics requested on the root node. We request a transform buffer, the
  // trajectory change and event buffers but one could choose to only evaluate the trajectory for example if a character
  // is off-screen or only the transforms if the trajectory is controller by a server.
  const MR::NodeID rootNodeId = networkDef->getRootNodeID();
  const uint16_t minLifespan = 1;
  m_net->addPostUpdateAccessAttrib(rootNodeId, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, minLifespan);
  m_net->addPostUpdateAccessAttrib(rootNodeId, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, minLifespan);
  m_net->addPostUpdateAccessAttrib(rootNodeId, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, minLifespan);
  m_net->addPostUpdateAccessAttrib(rootNodeId, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, minLifespan);

  //----------------------------
  // Allocate a pose buffer.
  //
  // This buffer is used to store the final output of a network update for later use by other components of the game.
  // The buffer size is chosen to be able to hold any possible network output (the number of bones output in each frame
  // may change with the active animation set).
  const uint32_t maxBoneCount = networkDef->getMaxBoneCount();
  NMP::Memory::Format bufferFormat = NMP::DataBuffer::getPosQuatMemoryRequirements(maxBoneCount);
  NMP::Memory::Resource bufferResource = NMPMemoryAllocateFromFormat(bufferFormat);
  NMP_ASSERT(bufferResource.ptr);

  m_worldTransforms = NMP::DataBuffer::initPosQuat(bufferResource, bufferFormat, maxBoneCount);

  NMP_STDOUT("Game Character Initialised");

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterBasic::term()
{
  NMP::Memory::memFree(m_worldTransforms);

  if (m_net)
  {
    //----------------------------
    // Release the network before returning
    m_net->getDispatcher()->releaseAndDestroy();
    m_net->releaseAndDestroy();
    m_net = NULL;
  }

  if (m_temporaryMemoryAllocator)
  {
    m_temporaryMemoryAllocator->term();
    NMP::Memory::memFree(m_temporaryMemoryAllocator);
    m_temporaryMemoryAllocator = NULL;
  }

  if (m_persistentMemoryAllocator)
  {
    m_persistentMemoryAllocator->term();
    NMP::Memory::memFree(m_persistentMemoryAllocator);
    m_persistentMemoryAllocator = NULL;
  }

  return true;
}


} // namespace Game