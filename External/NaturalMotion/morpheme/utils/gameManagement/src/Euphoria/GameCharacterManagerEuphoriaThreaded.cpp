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
#include "GameManagement/Euphoria/GameAnimSystemModuleEuphoria.h"
#include "GameManagement/Euphoria/GameCharacterManagerEuphoriaThreaded.h"
#include "GameManagement/Euphoria/GameCharacterEuphoria.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
// CharacterManagerEuphoriaThreaded functions.
//----------------------------------------------------------------------------------------------------------------------
CharacterManagerEuphoriaThreaded::CharacterManagerEuphoriaThreaded() :
  CharacterManagerEuphoria()
{
  m_bucketsVector.clear();
  m_characterBucketIndexList.clear();
}

//----------------------------------------------------------------------------------------------------------------------
CharacterManagerEuphoriaThreaded::~CharacterManagerEuphoriaThreaded()
{
  NMP_ASSERT(m_bucketsVector.empty());
  NMP_ASSERT(m_characterBucketIndexList.empty());
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoriaThreaded::init()
{
  CharacterManagerEuphoria::init();

  m_bucketsVector.clear();
  m_characterBucketIndexList.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoriaThreaded::init(uint32_t numBuckets)
{
  NMP_ASSERT(AnimSystemModuleEuphoria::isInitialised());

  // Parent class start up of morpheme.
  CharacterManagerEuphoria::init();

  // Initialize the buckets. Each bucket creates a local temp allocator (fast heap) that is shared between all the
  // networks assigned to the bucket.
  initBucketsVector(numBuckets);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoriaThreaded::term()
{
  NMP_ASSERT(AnimSystemModuleEuphoria::isInitialised());

  // The thread scheduler should have been shut down.
  NMP_ASSERT(!m_threadScheduler.isActive());

  termBucketsVector();

  CharacterManagerEuphoria::term();
}

//----------------------------------------------------------------------------------------------------------------------
CharacterEuphoria* CharacterManagerEuphoriaThreaded::createCharacterMultiThreaded(
  CharacterDefEuphoria*      characterDefEuphoria,
  uint32_t                   bucketIndex,
  MR::PhysicsRigPhysX3::Type physicsRigType,
  physx::PxMaterial*         characterControllerMaterial,
  const NMP::Vector3&        initialPosition,
  const NMP::Quat&           initialOrientation,
  MR::AnimSetIndex           initialAnimSetIndex,
  const char*                name)
{
  // Check that the CharacterDef has been registered with this manager.
  NMP_ASSERT(isCharacterDefRegistered(characterDefEuphoria));

  // Create the CharacterNoPhysics.
  NMP_ASSERT(getBucket(bucketIndex) &&  getBucket(bucketIndex)->getTemporaryMemoryAllocator());
  NMP_ASSERT(m_physicsScene && m_physXCharacterControllerManager);
  CharacterEuphoria* gameCharacter = CharacterEuphoria::create(
                                                  characterDefEuphoria,
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
            characterDefEuphoria->getNetworkDef()->getName());
    NMP_ASSERT_FAIL();
    return NULL;
  }

  // The management of this CharacterNoPhysics' memory is this CharacterManagers responsibility.
  // Make sure to put it in the correct bucket.
  registerCharacter(gameCharacter, bucketIndex);

  return gameCharacter;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoriaThreaded::initBucketsVector(uint32_t numBuckets)
{
  for (uint32_t i = 0; i < numBuckets; ++i)
  {
    m_bucketsVector.push_back(new MorphemeBucketPhysX3());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoriaThreaded::termBucketsVector()
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
MorphemeBucketPhysX3* CharacterManagerEuphoriaThreaded::getBucket(uint32_t bucketIndex)
{
  NMP_ASSERT(bucketIndex < m_bucketsVector.size());
  return m_bucketsVector[bucketIndex];
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoriaThreaded::startThreadScheduler(uint32_t numThreads)
{
  NMP_ASSERT(numThreads > 0);
  m_threadScheduler.start(numThreads);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoriaThreaded::stopThreadScheduler()
{
  m_threadScheduler.stop();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoriaThreaded::waitForAllScheduledTasks()
{
  m_threadScheduler.waitForAllMorphemeTasks();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoriaThreaded::registerCharacter(CharacterEuphoria* character, uint32_t bucketIndex)
{
  NMP_ASSERT(bucketIndex < m_bucketsVector.size());
  CharacterManager::registerCharacter(character);
  m_bucketsVector[bucketIndex]->addCharacter(character);

  // Add to the bucket index list.
  m_characterBucketIndexList.push_back(bucketIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoriaThreaded::unregisterCharacter(CharacterEuphoria* character)
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
void CharacterManagerEuphoriaThreaded::destroyCharacter(CharacterEuphoria* character)
{
  NMP_ASSERT(character);
  // Remove it from the registered list.
  unregisterCharacter(character);

  // Destroy the Character itself.
  character->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoriaThreaded::schedulePreCharacterControllerTasks(float deltaTime)
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
void CharacterManagerEuphoriaThreaded::schedulePostCharacterControllerTasks(float deltaTime)
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
void CharacterManagerEuphoriaThreaded::scheduleFinaliseTasks(float deltaTime)
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
void CharacterManagerEuphoriaThreaded::resetBucketTemporaryMemoryAllocators()
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
    CharacterEuphoria* character = static_cast<CharacterEuphoria*>(*iter);
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
    CharacterEuphoria* character = static_cast<CharacterEuphoria*>(*iter);
    character->updatePostCharacterController(getDeltaTime());
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
    CharacterEuphoria* character = static_cast<CharacterEuphoria*>(*iter);
    character->updateFinalise(getDeltaTime());
  }
}

} // Game namespace

//----------------------------------------------------------------------------------------------------------------------
