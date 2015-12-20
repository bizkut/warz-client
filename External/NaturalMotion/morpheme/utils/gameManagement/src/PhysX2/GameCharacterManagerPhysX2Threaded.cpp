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
#include "GameManagement/PhysX2/GameAnimSystemModulePhysX2.h"
#include "gameManagement/PhysX2/GameCharacterManagerPhysX2Threaded.h"
#include "gameManagement/PhysX2/GameCharacterPhysX2.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
// CharacterManagerPhysX2Threaded functions.
//----------------------------------------------------------------------------------------------------------------------
CharacterManagerPhysX2Threaded::CharacterManagerPhysX2Threaded() :
  CharacterManagerPhysX2()
{
  m_bucketsVector.clear();
  m_characterBucketIndexList.clear();
}

//----------------------------------------------------------------------------------------------------------------------
CharacterManagerPhysX2Threaded::~CharacterManagerPhysX2Threaded()
{
  NMP_ASSERT(m_bucketsVector.empty());
  NMP_ASSERT(m_characterBucketIndexList.empty());
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Threaded::init()
{
  CharacterManagerPhysX2::init();

  m_bucketsVector.clear();
  m_characterBucketIndexList.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Threaded::init(uint32_t numBuckets)
{
  NMP_ASSERT(AnimSystemModulePhysX2::isInitialised());

  // Parent class start up of morpheme.
  CharacterManagerPhysX2::init();

  // Initialize the buckets. Each bucket creates a local temp allocator (fast heap) that is shared between all the
  // networks assigned to the bucket.
  initBucketsVector(numBuckets);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Threaded::term()
{
  NMP_ASSERT(AnimSystemModulePhysX2::isInitialised());

  // The thread scheduler should have been shut down.
  NMP_ASSERT(!m_threadScheduler.isActive());

  termBucketsVector();

  CharacterManagerPhysX2::term();
}

//----------------------------------------------------------------------------------------------------------------------
CharacterPhysX2* CharacterManagerPhysX2Threaded::createCharacterMultiThreaded(
  CharacterDef*       characterDef,
  uint32_t            bucketIndex,
  const NMP::Vector3& initialPosition,
  const NMP::Quat&    initialOrientation,
  const NMP::Vector3& graveyardPosition,
  MR::AnimSetIndex    initialAnimSetIndex,
  const char*         name)
{
  // Check that the CharacterDef has been registered with this manager.
  NMP_ASSERT(isCharacterDefRegistered(characterDef));

  // Create the CharacterNoPhysics.
  NMP_ASSERT(getBucket(bucketIndex) &&  getBucket(bucketIndex)->getTemporaryMemoryAllocator());
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
void CharacterManagerPhysX2Threaded::initBucketsVector(uint32_t numBuckets)
{
  for (uint32_t i = 0; i < numBuckets; ++i)
  {
    m_bucketsVector.push_back(new MorphemeBucketPhysX2());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Threaded::termBucketsVector()
{
  for (BucketsVectorPhysX2::iterator iter = m_bucketsVector.begin(); iter != m_bucketsVector.end(); ++iter)
  {
    MorphemeBucketPhysX2* bucket = (*iter);
    delete bucket;
    (*iter) = NULL;
  }
  m_bucketsVector.clear();

  m_characterBucketIndexList.clear();
}

//----------------------------------------------------------------------------------------------------------------------
MorphemeBucketPhysX2* CharacterManagerPhysX2Threaded::getBucket(uint32_t bucketIndex)
{
  NMP_ASSERT(bucketIndex < m_bucketsVector.size());
  return m_bucketsVector[bucketIndex];
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Threaded::startThreadScheduler(uint32_t numThreads)
{
  NMP_ASSERT(numThreads > 0);
  m_threadScheduler.start(numThreads);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Threaded::stopThreadScheduler()
{
  m_threadScheduler.stop();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Threaded::waitForAllScheduledTasks()
{
  m_threadScheduler.waitForAllMorphemeTasks();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Threaded::registerCharacter(CharacterPhysX2* character, uint32_t bucketIndex)
{
  NMP_ASSERT(bucketIndex < m_bucketsVector.size());
  CharacterManager::registerCharacter(character);
  m_bucketsVector[bucketIndex]->addCharacter(character);

  // Add to the bucket index list.
  m_characterBucketIndexList.push_back(bucketIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Threaded::unregisterCharacter(CharacterPhysX2* character)
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
void CharacterManagerPhysX2Threaded::destroyCharacter(CharacterPhysX2* character)
{
  NMP_ASSERT(character);
  // Remove it from the registered list.
  unregisterCharacter(character);

  // Destroy the Character itself.
  character->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Threaded::schedulePreCharacterControllerTasks(float deltaTime)
{
  // Set the physics time step for the update
  NMP_ASSERT(m_physicsScene);
  m_physicsScene->setNextPhysicsTimeStep(deltaTime);

  //
  for (BucketsVectorPhysX2::iterator iter = m_bucketsVector.begin(); iter != m_bucketsVector.end(); ++iter)
  {
    MorphemeBucketPhysX2* bucket = (*iter);
    if (bucket && bucket->getCharacterList().size() > 0)
    {
      MorphemeTaskPhysX2UpdatePreCharacterController* task = bucket->getPreCharacterControllerTask();
      task->setDeltaTime(deltaTime);
      m_threadScheduler.queueMorphemeTask(task);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Threaded::schedulePostCharacterControllerTasks(float deltaTime)
{
  for (BucketsVectorPhysX2::iterator iter = m_bucketsVector.begin(); iter != m_bucketsVector.end(); ++iter)
  {
    MorphemeBucketPhysX2* bucket = (*iter);
    if (bucket && bucket->getCharacterList().size() > 0)
    {
      MorphemeTaskPhysX2UpdatePostCharacterController* task = bucket->getPostCharacterControllerTask();
      task->setDeltaTime(deltaTime);
      m_threadScheduler.queueMorphemeTask(task);
    }
  }
}
  
//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Threaded::scheduleFinaliseTasks(float deltaTime)
{
  // Tell morpheme the duration of the last physics time step immediately after the
  // physics engine is stepped.
  m_physicsScene->setLastPhysicsTimeStep(deltaTime);

  for (BucketsVectorPhysX2::iterator iter = m_bucketsVector.begin(); iter != m_bucketsVector.end(); ++iter)
  {
    MorphemeBucketPhysX2* bucket = (*iter);
    if (bucket && bucket->getCharacterList().size() > 0)
    {
      MorphemeTaskPhysX2UpdateFinalise* task = bucket->getFinaliseTask();
      task->setDeltaTime(deltaTime);
      m_threadScheduler.queueMorphemeTask(task);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Threaded::resetBucketTemporaryMemoryAllocators()
{
  for (BucketsVectorPhysX2::iterator iter = m_bucketsVector.begin(); iter != m_bucketsVector.end(); ++iter)
  {
    MorphemeBucketPhysX2* bucket = (*iter);
    if (bucket && bucket->getCharacterList().size() > 0)
    {
      bucket->getTemporaryMemoryAllocator()->reset();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// MorphemeTaskUpdateToPreCharacterController functions.
//----------------------------------------------------------------------------------------------------------------------
void MorphemeTaskPhysX2UpdatePreCharacterController::run()
{
  // Make sure to call: m_physicsScene->setNextPhysicsTimeStep(deltaTime);
  // which needs to happen once for the scene not for every bucket.

  NMP_ASSERT(m_bucket);
  const CharacterList& characters = m_bucket->getCharacterList();

  for (CharacterList::const_iterator iter = characters.begin(); iter != characters.end(); ++iter)
  {
    CharacterPhysX2* character = static_cast<CharacterPhysX2*>(*iter);
    character->updateToPreCharacterController(getDeltaTime());
  }
}

//----------------------------------------------------------------------------------------------------------------------
// MorphemeTaskUpdateToPrePhysics functions.
//----------------------------------------------------------------------------------------------------------------------
void MorphemeTaskPhysX2UpdatePostCharacterController::run()
{
  NMP_ASSERT(m_bucket);
  const CharacterList& characters = m_bucket->getCharacterList();

  for (CharacterList::const_iterator iter = characters.begin(); iter != characters.end(); ++iter)
  {
    CharacterPhysX2* character = static_cast<CharacterPhysX2*>(*iter);
    character->updatePostCharacterController(getDeltaTime());
  }
}

//----------------------------------------------------------------------------------------------------------------------
// MorphemeTaskUpdatePostPhysics functions.
//----------------------------------------------------------------------------------------------------------------------
void MorphemeTaskPhysX2UpdateFinalise::run()
{
  NMP_ASSERT(m_bucket);
  const CharacterList& characters = m_bucket->getCharacterList();

  for (CharacterList::const_iterator iter = characters.begin(); iter != characters.end(); ++iter)
  {
    CharacterPhysX2* character = static_cast<CharacterPhysX2*>(*iter);
    character->updateFinalise(getDeltaTime());
  }
}

} // Game namespace

//----------------------------------------------------------------------------------------------------------------------
