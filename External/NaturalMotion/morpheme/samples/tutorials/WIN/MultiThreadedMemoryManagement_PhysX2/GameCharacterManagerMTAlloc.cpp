// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "GameCharacterManagerMTAlloc.h"
// Our sample multi-thread friendly allocator
#include "MultiThreadTempAllocator.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
// CharacterManagerPhysX2 functions.
//----------------------------------------------------------------------------------------------------------------------
CharacterManagerMTAlloc::CharacterManagerMTAlloc() :
  CharacterManagerPhysX2()
{

}

//----------------------------------------------------------------------------------------------------------------------
CharacterManagerMTAlloc::~CharacterManagerMTAlloc()
{

}

//----------------------------------------------------------------------------------------------------------------------
CharacterPhysX2* CharacterManagerMTAlloc::createCharacterMultiThreaded(
  CharacterDef*       characterDef,
  const NMP::Vector3& initialPosition,
  const NMP::Quat&    initialOrientation,
  const NMP::Vector3& graveyardPosition,
  MR::AnimSetIndex    initialAnimSetIndex,
  const char*         name)
{
  // Check that the CharacterDef has been registered with this manager.
  NMP_ASSERT(isCharacterDefRegistered(characterDef));

  // Create the client allocator
  NMP::Memory::Format allocatorFormat = MultiThreadTempAllocator::getMemoryRequirements();
  NMP::Memory::Resource tempAllocRes = NMPMemoryAllocateFromFormat(allocatorFormat);
  MultiThreadTempAllocator* tempAllocator = MultiThreadTempAllocator::init(tempAllocRes, m_threadSafeMemoryPool);

  // Create the CharacterPhysX2.
  NMP_ASSERT(m_physicsScene && m_physXCharacterControllerManager);
  CharacterPhysX2* gameCharacter = CharacterPhysX2::create(
                                                  characterDef,
                                                  m_physicsScene,
                                                  m_physXCharacterControllerManager,
                                                  initialPosition,
                                                  initialOrientation,
                                                  graveyardPosition,
                                                  initialAnimSetIndex,
                                                  name,
                                                  tempAllocator);
  if (!gameCharacter)
  {
    NMP_STDOUT(
            "Failed to create game character %s instance from a loaded game character def %s.",
            name,
            characterDef->getNetworkDef()->getName());
    NMP_ASSERT_FAIL();
    return NULL;
  }

  // The management of this CharacterPhysX2's memory is this CharacterManagers responsibility.
  registerCharacter(gameCharacter, tempAllocator);

  return gameCharacter;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerMTAlloc::startThreadScheduler(uint32_t numThreads)
{
  NMP_ASSERT(numThreads > 0);
  m_threadScheduler.start(numThreads);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerMTAlloc::stopThreadScheduler()
{
  m_threadScheduler.stop();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerMTAlloc::waitForAllScheduledTasks()
{
  m_threadScheduler.waitForAllMorphemeTasks();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerMTAlloc::registerCharacter(CharacterPhysX2* character, NMP::TempMemoryAllocator* tempAllocator)
{
  CharacterManager::registerCharacter(character);
  // Add the character to the multi-threaded character list.  This will cause it to get queued for execution
  ThreadedMorphemeCharacter* threadedCharacter = new ThreadedMorphemeCharacter(character, tempAllocator);

  m_characterVector.push_back(threadedCharacter);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerMTAlloc::unregisterCharacter(CharacterPhysX2* character)
{
  CharacterManager::unregisterCharacter(character);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerMTAlloc::destroyCharacter(CharacterPhysX2* character)
{
  NMP_ASSERT(character);
  // Remove it from the registered list.
  unregisterCharacter(character);

  NMP::TempMemoryAllocator *tempAllocator = character->getNetwork()->getTempMemoryAllocator();

  // Destroy the Character itself.
  character->destroy();

  // Destroy the MultiThreadTempAllocator
  tempAllocator->term();
  NMP::Memory::memFree(tempAllocator);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerMTAlloc::schedulePreCharacterControllerTasks(float deltaTime)
{
  // Set the physics time step for the update
  NMP_ASSERT(m_physicsScene);
  m_physicsScene->setNextPhysicsTimeStep(deltaTime);

  for (CharacterVector::iterator iter = m_characterVector.begin(); iter != m_characterVector.end(); ++iter)
  {
    ThreadedMorphemeCharacter* threadedCharacter = (*iter);

    MorphemeTaskUpdatePreCharacterController* task = threadedCharacter->getPreCharacterControllerTask();
    task->setDeltaTime(deltaTime);
    m_threadScheduler.queueMorphemeTask(task);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerMTAlloc::schedulePostCharacterControllerTasks(float deltaTime)
{
  for (CharacterVector::iterator iter = m_characterVector.begin(); iter != m_characterVector.end(); ++iter)
  {
    ThreadedMorphemeCharacter* threadedCharacter = (*iter);

    MorphemeTaskUpdatePostCharacterController* task = threadedCharacter->getPostCharacterControllerTask();
    task->setDeltaTime(deltaTime);
    m_threadScheduler.queueMorphemeTask(task);

  }
}
  
//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerMTAlloc::scheduleFinaliseTasks(float deltaTime)
{
  // Tell morpheme the duration of the last physics time step immediately after the
  // physics engine is stepped.
  m_physicsScene->setLastPhysicsTimeStep(deltaTime);

  for (CharacterVector::iterator iter = m_characterVector.begin(); iter != m_characterVector.end(); ++iter)
  {
    ThreadedMorphemeCharacter* threadedCharacter = (*iter);

    MorphemeTaskUpdateFinalise* task = threadedCharacter->getFinaliseTask();
    task->setDeltaTime(deltaTime);
    m_threadScheduler.queueMorphemeTask(task);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerMTAlloc::createSharedMemoryPool(uint32_t numPages, uint32_t pageSize)
{
  NMP::Memory::Format allocatorFormat = ThreadSafeMemoryPool::getMemoryRequirements(
    numPages, pageSize, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource sharedPoolResource = NMPMemoryAllocateFromFormat(allocatorFormat);
  m_threadSafeMemoryPool = ThreadSafeMemoryPool::init(
    sharedPoolResource, numPages, pageSize, NMP_VECTOR_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerMTAlloc::destroySharedMemoryPool()
{
  m_threadSafeMemoryPool->term();
  NMP::Memory::memFree(m_threadSafeMemoryPool);
  m_threadSafeMemoryPool = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerMTAlloc::resetTemporaryMemoryAllocators()
{
  // We need to reset all the character's allocators, which will free any unused pages back to the pool
  for (CharacterVector::iterator iter = m_characterVector.begin(); iter != m_characterVector.end(); ++iter)
  {
    ThreadedMorphemeCharacter* threadedCharacter = (*iter);

    threadedCharacter->getTemporaryMemoryAllocator()->reset();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// MorphemeTaskUpdatePreCharacterController functions.
//----------------------------------------------------------------------------------------------------------------------
void MorphemeTaskUpdatePreCharacterController::run()
{
  // Make sure to call: m_physicsScene->setNextPhysicsTimeStep(deltaTime);
  // which needs to happen once for the scene not for every bucket.

  NMP_ASSERT(m_character);
  CharacterPhysX2* character = static_cast<CharacterPhysX2*>(m_character);
  character->updateToPreCharacterController(getDeltaTime());
}

//----------------------------------------------------------------------------------------------------------------------
// MorphemeTaskUpdatePostCharacterController functions.
//----------------------------------------------------------------------------------------------------------------------
void MorphemeTaskUpdatePostCharacterController::run()
{
  NMP_ASSERT(m_character);
  CharacterPhysX2* character = static_cast<CharacterPhysX2*>(m_character);
  character->updatePostCharacterController(getDeltaTime());
}

//----------------------------------------------------------------------------------------------------------------------
// MorphemeTaskUpdateFinalise functions.
//----------------------------------------------------------------------------------------------------------------------
void MorphemeTaskUpdateFinalise::run()
{
  NMP_ASSERT(m_character);
  CharacterPhysX2* character = static_cast<CharacterPhysX2*>(m_character);
  character->updateFinalise(getDeltaTime());
}

} // Game namespace

//----------------------------------------------------------------------------------------------------------------------
