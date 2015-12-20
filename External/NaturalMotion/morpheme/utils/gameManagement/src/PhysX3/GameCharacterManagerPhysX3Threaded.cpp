// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "GameManagement/PhysX3/GameAnimSystemModulePhysX3.h"
#include "gameManagement/PhysX3/GameCharacterManagerPhysX3Threaded.h"
#include "gameManagement/PhysX3/GameCharacterPhysX3.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
// CharacterManagerPhysX3Threaded functions.
//----------------------------------------------------------------------------------------------------------------------
CharacterManagerPhysX3Threaded::CharacterManagerPhysX3Threaded() :
  CharacterManagerPhysX3()
{
  m_bucketsVector.clear();
  m_characterBucketIndexList.clear();
}

//----------------------------------------------------------------------------------------------------------------------
CharacterManagerPhysX3Threaded::~CharacterManagerPhysX3Threaded()
{
  NMP_ASSERT(m_bucketsVector.empty());
  NMP_ASSERT(m_characterBucketIndexList.empty());
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3Threaded::init()
{
  CharacterManagerPhysX3::init();

  m_bucketsVector.clear();
  m_characterBucketIndexList.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3Threaded::init(uint32_t numBuckets)
{
  NMP_ASSERT(AnimSystemModulePhysX3::isInitialised());

  // Parent class start up of morpheme.
  CharacterManagerPhysX3::init();

  // Initialize the buckets. Each bucket creates a local temp allocator (fast heap) that is shared between all the
  // networks assigned to the bucket.
  initBucketsVector(numBuckets);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3Threaded::term()
{
  NMP_ASSERT(AnimSystemModulePhysX3::isInitialised());

  // The thread scheduler should have been shut down.
  NMP_ASSERT(!m_threadScheduler.isActive());

  termBucketsVector();

  CharacterManagerPhysX3::term();
}

//----------------------------------------------------------------------------------------------------------------------
CharacterPhysX3* CharacterManagerPhysX3Threaded::createCharacterMultiThreaded(
  CharacterDef*              characterDef,
  uint32_t                   bucketIndex,
  MR::PhysicsRigPhysX3::Type physicsRigType,
  physx::PxMaterial*         characterControllerMaterial,
  const NMP::Vector3&        initialPosition,
  const NMP::Quat&           initialOrientation,
  MR::AnimSetIndex           initialAnimSetIndex,
  const char*                name)
{
  // Check that the CharacterDef has been registered with this manager.
  NMP_ASSERT(isCharacterDefRegistered(characterDef));

  // Create the CharacterNoPhysics.
  NMP_ASSERT(getBucket(bucketIndex) &&  getBucket(bucketIndex)->getTemporaryMemoryAllocator());
  NMP_ASSERT(m_physicsScene && m_physXCharacterControllerManager);
  CharacterPhysX3* gameCharacter = CharacterPhysX3::create(
                                                  characterDef,
                                                  m_physicsScene,
                                                  m_physXCharacterControllerManager,
                                                  characterControllerMaterial,
                                                  physicsRigType,
                                                  initialPosition,
                                                  initialOrientation,
                                                  initialAnimSetIndex,
                                                  name,
                                                  getBucket(bucketIndex)->getTemporaryMemoryAllocator());
  if (!gameCharacter)
  {
    NMP_STDOUT(
            "Failed to create game character %s instance from a loaded game character def %s.",
            name,
            characterDef->getNetworkDef()->getName());
    NMP_ASSERT_FAIL();
    return NULL;
  }

  // The management of this CharacterNoPhysics' memory is this CharacterManagers responsibility.
  // Make sure to put it in the correct bucket.
  registerCharacter(gameCharacter, bucketIndex);

  return gameCharacter;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3Threaded::initBucketsVector(uint32_t numBuckets)
{
  for (uint32_t i = 0; i < numBuckets; ++i)
  {
    m_bucketsVector.push_back(new MorphemeBucketPhysX3());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3Threaded::termBucketsVector()
{
  for (BucketsVectorPhysX3::iterator iter = m_bucketsVector.begin(); iter != m_bucketsVector.end(); ++iter)
  {
    MorphemeBucketPhysX3* bucket = (*iter);
    delete bucket;
    (*iter) = NULL;
  }
  m_bucketsVector.clear();

  m_characterBucketIndexList.clear();
}

//----------------------------------------------------------------------------------------------------------------------
MorphemeBucketPhysX3* CharacterManagerPhysX3Threaded::getBucket(uint32_t bucketIndex)
{
  NMP_ASSERT(bucketIndex < m_bucketsVector.size());
  return m_bucketsVector[bucketIndex];
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3Threaded::startThreadScheduler(uint32_t numThreads)
{
  NMP_ASSERT(numThreads > 0);
  m_threadScheduler.start(numThreads);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3Threaded::stopThreadScheduler()
{
  m_threadScheduler.stop();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3Threaded::waitForAllScheduledTasks()
{
  m_threadScheduler.waitForAllMorphemeTasks();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3Threaded::registerCharacter(CharacterPhysX3* character, uint32_t bucketIndex)
{
  NMP_ASSERT(bucketIndex < m_bucketsVector.size());
  CharacterManager::registerCharacter(character);
  m_bucketsVector[bucketIndex]->addCharacter(character);

  // Add to the bucket index list.
  m_characterBucketIndexList.push_back(bucketIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3Threaded::unregisterCharacter(CharacterPhysX3* character)
{
  BucketIndexList::iterator indexIter = m_characterBucketIndexList.begin();
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter, ++indexIter)
  {
    if ((*iter) == character)
    {
      // Remove the entry from the list; we do not destroy the Character itself.
      m_characterList.erase(iter);

      // Remove the character entry from the correct bucket.
      m_bucketsVector[*indexIter]->removeCharacter(character);

      // Remove our entry from the bucket index list.
      m_characterBucketIndexList.erase(indexIter);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3Threaded::destroyCharacter(CharacterPhysX3* character)
{
  NMP_ASSERT(character);
  // Remove it from the registered list.
  unregisterCharacter(character);

  // Destroy the Character itself.
  character->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3Threaded::schedulePreCharacterControllerTasks(float deltaTime)
{
  // Set the physics time step for the update
  NMP_ASSERT(m_physicsScene);
  m_physicsScene->setNextPhysicsTimeStep(deltaTime);

  //
  for (BucketsVectorPhysX3::iterator iter = m_bucketsVector.begin(); iter != m_bucketsVector.end(); ++iter)
  {
    MorphemeBucketPhysX3* bucket = (*iter);
    if (bucket && bucket->getCharacterList().size() > 0)
    {
      MorphemeTaskPhysX3UpdatePreCharacterController* task = bucket->getPreCharacterControllerTask();
      task->setDeltaTime(deltaTime);
      m_threadScheduler.queueMorphemeTask(task);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3Threaded::schedulePostCharacterControllerTasks(float deltaTime)
{
  for (BucketsVectorPhysX3::iterator iter = m_bucketsVector.begin(); iter != m_bucketsVector.end(); ++iter)
  {
    MorphemeBucketPhysX3* bucket = (*iter);
    if (bucket && bucket->getCharacterList().size() > 0)
    {
      MorphemeTaskPhysX3UpdatePostCharacterController* task = bucket->getPostCharacterControllerTask();
      task->setDeltaTime(deltaTime);
      m_threadScheduler.queueMorphemeTask(task);
    }
  }
}
  
//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3Threaded::scheduleFinaliseTasks(float deltaTime)
{
  // Tell morpheme the duration of the last physics time step immediately after the
  // physics engine is stepped.
  m_physicsScene->setLastPhysicsTimeStep(deltaTime);

  for (BucketsVectorPhysX3::iterator iter = m_bucketsVector.begin(); iter != m_bucketsVector.end(); ++iter)
  {
    MorphemeBucketPhysX3* bucket = (*iter);
    if (bucket && bucket->getCharacterList().size() > 0)
    {
      MorphemeTaskPhysX3UpdateFinalise* task = bucket->getFinaliseTask();
      task->setDeltaTime(deltaTime);
      m_threadScheduler.queueMorphemeTask(task);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3Threaded::resetBucketTemporaryMemoryAllocators()
{
  for (BucketsVectorPhysX3::iterator iter = m_bucketsVector.begin(); iter != m_bucketsVector.end(); ++iter)
  {
    MorphemeBucketPhysX3* bucket = (*iter);
    if (bucket && bucket->getCharacterList().size() > 0)
    {
      bucket->getTemporaryMemoryAllocator()->reset();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// MorphemeTaskUpdateToPreCharacterController functions.
//----------------------------------------------------------------------------------------------------------------------
void MorphemeTaskPhysX3UpdatePreCharacterController::run()
{
  // Make sure to call: m_physicsScene->setNextPhysicsTimeStep(deltaTime);
  // which needs to happen once for the scene not for every bucket.

  NMP_ASSERT(m_bucket);
  const CharacterList& characters = m_bucket->getCharacterList();

  for (CharacterList::const_iterator iter = characters.begin(); iter != characters.end(); ++iter)
  {
    CharacterPhysX3* character = static_cast<CharacterPhysX3*>(*iter);
    character->updateToPreCharacterController(getDeltaTime());
  }
}

//----------------------------------------------------------------------------------------------------------------------
// MorphemeTaskUpdateToPrePhysics functions.
//----------------------------------------------------------------------------------------------------------------------
void MorphemeTaskPhysX3UpdatePostCharacterController::run()
{
  NMP_ASSERT(m_bucket);
  const CharacterList& characters = m_bucket->getCharacterList();

  for (CharacterList::const_iterator iter = characters.begin(); iter != characters.end(); ++iter)
  {
    CharacterPhysX3* character = static_cast<CharacterPhysX3*>(*iter);
    character->updatePostCharacterController();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// MorphemeTaskUpdatePostPhysics functions.
//----------------------------------------------------------------------------------------------------------------------
void MorphemeTaskPhysX3UpdateFinalise::run()
{
  NMP_ASSERT(m_bucket);
  const CharacterList& characters = m_bucket->getCharacterList();

  for (CharacterList::const_iterator iter = characters.begin(); iter != characters.end(); ++iter)
  {
    CharacterPhysX3* character = static_cast<CharacterPhysX3*>(*iter);
    character->updateFinalise(getDeltaTime());
  }
}

} // Game namespace

//----------------------------------------------------------------------------------------------------------------------
