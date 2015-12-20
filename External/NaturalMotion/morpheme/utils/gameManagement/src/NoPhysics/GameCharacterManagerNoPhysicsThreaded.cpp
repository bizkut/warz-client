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
#include "gameManagement/NoPhysics/GameCharacterManagerNoPhysicsThreaded.h"
#include "GameManagement/NoPhysics/GameAnimSystemModuleNoPhysics.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
// CharacterManagerNoPhysicsThreaded functions.
//----------------------------------------------------------------------------------------------------------------------
CharacterManagerNoPhysicsThreaded::CharacterManagerNoPhysicsThreaded() :
  CharacterManagerNoPhysics()
{
  m_bucketsVector.clear();
  m_characterBucketIndexList.clear();
}

//----------------------------------------------------------------------------------------------------------------------
CharacterManagerNoPhysicsThreaded::~CharacterManagerNoPhysicsThreaded()
{
  NMP_ASSERT(m_characterBucketIndexList.empty());
  NMP_ASSERT(m_bucketsVector.empty());
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerNoPhysicsThreaded::init()
{
  NMP_ASSERT(AnimSystemModuleNoPhysics::isInitialised());

  // Base class start up of morpheme.
  CharacterManager::init();

  m_bucketsVector.clear();
  m_characterBucketIndexList.clear();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerNoPhysicsThreaded::init(uint32_t numBuckets)
{
  NMP_ASSERT(AnimSystemModuleNoPhysics::isInitialised());

  // Base class start up of morpheme.
  CharacterManager::init();

  // Initialize the buckets. Each bucket creates a local temp allocator (fast heap) that is shared between all the
  // networks assigned to the bucket.
  initBucketsVector(numBuckets);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerNoPhysicsThreaded::term()
{  
  NMP_ASSERT(AnimSystemModuleNoPhysics::isInitialised());

  // The thread scheduler should have been shut down.
  NMP_ASSERT(!m_threadScheduler.isActive());

  termBucketsVector();

  // Base class shut down of morpheme.
  CharacterManager::term();
}

//----------------------------------------------------------------------------------------------------------------------
CharacterNoPhysics* CharacterManagerNoPhysicsThreaded::createCharacterMultiThreaded(
  CharacterDef*       characterDef,
  uint32_t            bucketIndex,
  const NMP::Vector3& initialPosition,
  const NMP::Quat&    initialOrientation,
  MR::AnimSetIndex    initialAnimSetIndex,
  const char*         name)
{
  // Check that the CharacterDef has been registered with this manager.
  NMP_ASSERT(isCharacterDefRegistered(characterDef));

  // Create the CharacterNoPhysics.
  NMP_ASSERT(getBucket(bucketIndex) &&  getBucket(bucketIndex)->getTemporaryMemoryAllocator());
  CharacterNoPhysics* gameCharacter = CharacterNoPhysics::create(
                                                  characterDef,
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
void CharacterManagerNoPhysicsThreaded::initBucketsVector(uint32_t numBuckets)
{
  for (uint32_t i = 0; i < numBuckets; ++i)
  {
    m_bucketsVector.push_back(new MorphemeBucketNoPhysics());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerNoPhysicsThreaded::termBucketsVector()
{
  for (BucketsVectorNoPhysics::iterator iter = m_bucketsVector.begin(); iter != m_bucketsVector.end(); ++iter)
  {
    MorphemeBucketNoPhysics* bucket = (*iter);
    delete bucket;
    (*iter) = NULL;
  }
  m_bucketsVector.clear();

  m_characterBucketIndexList.clear();
}

//----------------------------------------------------------------------------------------------------------------------
MorphemeBucketNoPhysics* CharacterManagerNoPhysicsThreaded::getBucket(uint32_t bucketIndex)
{
  NMP_ASSERT(bucketIndex < m_bucketsVector.size());
  return m_bucketsVector[bucketIndex];
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerNoPhysicsThreaded::startThreadScheduler(uint32_t numThreads)
{
  NMP_ASSERT(numThreads > 0);
  m_threadScheduler.start(numThreads);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerNoPhysicsThreaded::stopThreadScheduler()
{
  m_threadScheduler.stop();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerNoPhysicsThreaded::waitForAllScheduledTasks()
{
  m_threadScheduler.waitForAllMorphemeTasks();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerNoPhysicsThreaded::registerCharacter(CharacterNoPhysics* character, uint32_t bucketIndex)
{
  NMP_ASSERT(bucketIndex < m_bucketsVector.size());
  CharacterManager::registerCharacter(character);
  m_bucketsVector[bucketIndex]->addCharacter(character);

  // Add to the bucket index list.
  m_characterBucketIndexList.push_back(bucketIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerNoPhysicsThreaded::unregisterCharacter(CharacterNoPhysics* character)
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
void CharacterManagerNoPhysicsThreaded::destroyCharacter(CharacterNoPhysics* character)
{
  NMP_ASSERT(character);
  // Remove it from the registered list.
  unregisterCharacter(character);

  // Destroy the Character itself.
  character->destroy();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerNoPhysicsThreaded::schedulePreCharacterControllerTasks(float deltaTime)
{
  //
  for (BucketsVectorNoPhysics::iterator iter = m_bucketsVector.begin(); iter != m_bucketsVector.end(); ++iter)
  {
    MorphemeBucketNoPhysics* bucket = (*iter);
    if (bucket && bucket->getCharacterList().size() > 0)
    {
      MorphemeTaskNoPhysicsUpdatePreCharacterController* task = bucket->getPreCharacterControllerTask();
      task->setDeltaTime(deltaTime);
      m_threadScheduler.queueMorphemeTask(task);
    }
  }
}
  
//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerNoPhysicsThreaded::scheduleFinaliseTasks(float deltaTime)
{
  for (BucketsVectorNoPhysics::iterator iter = m_bucketsVector.begin(); iter != m_bucketsVector.end(); ++iter)
  {
    MorphemeBucketNoPhysics* bucket = (*iter);
    if (bucket && bucket->getCharacterList().size() > 0)
    {
      MorphemeTaskNoPhysicsUpdateFinalise* task = bucket->getFinaliseTask();
      task->setDeltaTime(deltaTime);
      m_threadScheduler.queueMorphemeTask(task);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerNoPhysicsThreaded::resetBucketTemporaryMemoryAllocators()
{
  for (BucketsVectorNoPhysics::iterator iter = m_bucketsVector.begin(); iter != m_bucketsVector.end(); ++iter)
  {
    MorphemeBucketNoPhysics* bucket = (*iter);
    if (bucket && bucket->getCharacterList().size() > 0)
    {
      bucket->getTemporaryMemoryAllocator()->reset();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// MorphemeTaskNoPhysicsUpdatePreCharacterController functions.
//----------------------------------------------------------------------------------------------------------------------
void MorphemeTaskNoPhysicsUpdatePreCharacterController::run()
{
  NMP_ASSERT(m_bucket);
  const CharacterList& characters = m_bucket->getCharacterList();

  for (CharacterList::const_iterator iter = characters.begin(); iter != characters.end(); ++iter)
  {
    CharacterNoPhysics* character = static_cast<CharacterNoPhysics*>(*iter);
    character->updateToPreCharacterController(getDeltaTime());
  }
}

//----------------------------------------------------------------------------------------------------------------------
// MorphemeTaskNoPhysicsUpdateFinalise functions.
//----------------------------------------------------------------------------------------------------------------------
void MorphemeTaskNoPhysicsUpdateFinalise::run()
{
  NMP_ASSERT(m_bucket);
  const CharacterList& characters = m_bucket->getCharacterList();

  for (CharacterList::const_iterator iter = characters.begin(); iter != characters.end(); ++iter)
  {
    CharacterNoPhysics* character = static_cast<CharacterNoPhysics*>(*iter);
    character->updateFinalise();
  }
}

} // Game namespace

//----------------------------------------------------------------------------------------------------------------------
