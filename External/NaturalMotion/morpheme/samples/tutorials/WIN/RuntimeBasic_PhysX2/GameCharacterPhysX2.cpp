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
#include "GameCharacterPhysX2.h"

#include "GameCharacterManagerPhysX2.h"
#include "morpheme/mrCharacterControllerDef.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookup.h"
#include "morpheme/mrBlendOps.h"
#include "NMPlatform/NMFreelistMemoryAllocator.h"
#include "physics/mrPhysics.h"

namespace Game
{


//----------------------------------------------------------------------------------------------------------------------
CharacterPhysX2Basic* CharacterPhysX2Basic::create(
  CharacterDefBasic* gameCharacterDef,
  MR::PhysicsScenePhysX2* physicsScene,
  NxControllerManager* controllerManager)
{
  //----------------------------
  // Make sure the networkDef has been initialised
  if(!gameCharacterDef)
  {
    NMP_DEBUG_MSG("error: Invalid Character Definition!");
    return NULL;
  }

  //----------------------------
  // Create and initialise an instance of our character
  CharacterPhysX2Basic* const instance = static_cast<CharacterPhysX2Basic*>(NMPMemoryAlloc(sizeof(CharacterPhysX2Basic)));
  new(instance) CharacterPhysX2Basic(gameCharacterDef);

  //----------------------------
  // Initialise Game Character, allocate memory etc
  if (!instance->init(gameCharacterDef))
  {
    delete instance;
    NMP_DEBUG_MSG("error: Failed to create Game Character!");
    return NULL;
  }

  //----------------------------
  // Initialise a PhysicsCharCtrl for physics collisions. The Character class will own the character
  // controller but the does return a pointer to the PhysicsCharCtrl if required.
  NMP_STDOUT("Initialise Physics Character Controller");
  CharacterControllerPhysX2Basic* const physx2CharacterController = CharacterControllerPhysX2Basic::create(
                                                                      instance->m_net,
                                                                      physicsScene,
                                                                      controllerManager);

  if (!physx2CharacterController)
  {
    //----------------------------
    // CharacterControllerPhysX2Basic init failed
    NMP_ASSERT_FAIL();
    return NULL;
  }

  instance->getNetwork()->setCharacterController(physx2CharacterController);

  //----------------------------
  // Set the default active anim set - here we only have 1 to choose.
  instance->m_net->setActiveAnimSetIndex(0);

  //----------------------
  // Perform an initial update of the network with start time of zero
  instance->runInitialisingUpdateStep();

  NMP_STDOUT("GameCharacter created");

  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX2Basic::destroy(CharacterPhysX2Basic* character)
{
  //----------------------------
  // Release any memory allocated in our instance
  character->term();

  //----------------------------
  // Release our instance
  NMP::Memory::memFree(character);
  character = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX2Basic::updateWorldTransforms(const NMP::Matrix34& rootTransform)
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
void CharacterPhysX2Basic::runInitialisingUpdateStep()
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
// Update the network to a point where an update to the character's character controller is required.
void CharacterPhysX2Basic::updatePreCharacterController(float deltaTime)
{
  //----------------------------
  // Build queue.
  NMP_ASSERT(m_net)
    m_net->startUpdate(deltaTime);

  //----------------------------
  // Update the network until we are at the point that the character controller requires an update.
  MR::Task* task = NULL;
  MR::ExecuteResult execResult;
  do
  {
    execResult = m_net->update(task);
  } while (execResult == MR::EXECUTE_RESULT_IN_PROGRESS);

  if (task)
  {
    NMP_ASSERT(task->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Update our Character's character controller.
void CharacterPhysX2Basic::updateCharacterController(float deltaTime)
{
  CharacterControllerPhysX2Basic* physXCharacterController = getCharacterController(); 
  NMP_ASSERT(physXCharacterController)
  
  //----------------------------
  // If the root is under animation control: begin our update of the character controller by setting the requested movement.
  physXCharacterController->updatePreController();

  //----------------------------
  // If the root is under animation control: compute the required character controller transform.
  physXCharacterController->updateController(deltaTime);
}

//----------------------------------------------------------------------------------------------------------------------
// Update the network until we need to perform a physics update
void CharacterPhysX2Basic::updatePostCharacterController()
{ 
  //----------------------------
  // Update the network until we are at the point that the character controller requires an update.
  MR::Task* task = NULL;
  MR::ExecuteResult execResult;
  do
  {
    //----------------------------
    // Network::update() will return EXECUTE_RESULT_COMPLETE if the task is
    // flagged as external. In this case the task pointer reference will contain
    // the external task. If the task is not external when Network::update()
    // returns EXECUTE_RESULT_COMPLETE it will be NULL.
    // See the following section of the user guide for more details:
    // Understanding Morpheme > Networks > Runtime handling > Tasks > External tasks
    execResult = m_net->update(task);
  } while (execResult == MR::EXECUTE_RESULT_IN_PROGRESS);

  if (task)
  {
    NMP_ASSERT(task->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEPHYSICS);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Write our cached PhysX channel data to our rig in preparation of the physics step.
void CharacterPhysX2Basic::updatePrePhysics()
{
  MR::PhysicsRig* physicsRig = MR::getPhysicsRig(m_net);
  if (physicsRig)
  {
    physicsRig->updatePrePhysics(physicsRig->getPhysicsScene()->getNextPhysicsTimeStep());
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Re-generate our physics cache data from the PhysX rig after the physics step which should have just taken place.
void CharacterPhysX2Basic::updatePostPhysics()
{
  MR::PhysicsRig* physicsRig = MR::getPhysicsRig(m_net);
  if (physicsRig)
  {
    physicsRig->updatePostPhysics(physicsRig->getPhysicsScene()->getNextPhysicsTimeStep());
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Update the network after physics has been updated.
void CharacterPhysX2Basic::updateFinalise()
{
  NET_LOG_ENTER_FUNC();

  MR::Task* task = NULL;
  MR::ExecuteResult execResult;
  do
  {
    execResult = m_net->update(task);
  } while (execResult == MR::EXECUTE_RESULT_IN_PROGRESS);

  if (task)
  {
    NMP_ASSERT(task->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEROOT);
  }

  // Update character controller.
  NMP_STDOUT("updatePostPhysics: Updating trajectory");
  CharacterControllerPhysX2Basic* physXCharacterController = getCharacterController(); 
  physXCharacterController->updatePostController();

  do
  {
    execResult = m_net->update(task);
  } while (execResult == MR::EXECUTE_RESULT_IN_PROGRESS);


  // finalize post network update
  m_net->endUpdate();

  //----------------------------
  // Reset the temporary allocator
  m_net->getTempMemoryAllocator()->reset();

  //----------------------------
  // Accumulate the transforms after the updates
  updateWorldTransforms(m_net->getCharacterPropertiesWorldRootTransform());

  //----------------------------
  // we don't expect any external tasks, or any physics related tasks, to be left
  NMP_ASSERT(task == 0 && execResult == MR::EXECUTE_RESULT_COMPLETE);
}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterPhysX2Basic::init(CharacterDefBasic* characterDef)
{
  m_characterDef = characterDef;

  NMP_ASSERT(m_characterDef);

  MR::NetworkDef* networkDef = m_characterDef->getNetworkDef();

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
  m_net->addPostUpdateAccessAttrib(rootNodeId, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, minLifespan);
  m_net->addPostUpdateAccessAttrib(rootNodeId, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, minLifespan);
  m_net->addPostUpdateAccessAttrib(rootNodeId, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, minLifespan);
  m_net->addPostUpdateAccessAttrib(rootNodeId, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, minLifespan);

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
bool CharacterPhysX2Basic::term()
{
  if (m_worldTransforms)
  {
    NMP::Memory::memFree(m_worldTransforms);
  }

  CharacterControllerPhysX2Basic* characterController = getCharacterController();
  if (characterController)
  {
    CharacterControllerPhysX2Basic::destroy(characterController);
  }

  if (m_net)
  {
    //----------------------------
    // Release the network before returning
    m_net->getDispatcher()->releaseAndDestroy();
    m_net->releaseAndDestroy();
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

  return true;
}


} // namespace Game
