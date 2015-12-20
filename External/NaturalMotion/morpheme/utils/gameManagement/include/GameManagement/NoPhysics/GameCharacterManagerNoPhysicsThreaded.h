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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef GAME_CHARACTER_MANAGER_NOPHYSICS_THREADED_H
#define GAME_CHARACTER_MANAGER_NOPHYSICS_THREADED_H
//----------------------------------------------------------------------------------------------------------------------
#include "GameManagement/GameThreadScheduler.h"
#include "GameManagement/NoPhysics/GameCharacterNoPhysics.h"
#include "GameManagement/NoPhysics/GameCharacterManagerNoPhysics.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
/// \class Game::MorphemeTaskNoPhysicsUpdatePreCharacterController
//----------------------------------------------------------------------------------------------------------------------
class MorphemeTaskNoPhysicsUpdatePreCharacterController : public MorphemeBucketTask
{
public:
  void run() NM_OVERRIDE;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class Game::MorphemeTaskNoPhysicsUpdateFinalise
//----------------------------------------------------------------------------------------------------------------------
class MorphemeTaskNoPhysicsUpdateFinalise : public MorphemeBucketTask
{
public:
  void run() NM_OVERRIDE;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class Game::MorphemeBucketNoPhysics
//----------------------------------------------------------------------------------------------------------------------
class MorphemeBucketNoPhysics : public MorphemeBucket
{
public:
  MorphemeBucketNoPhysics() :
    MorphemeBucket(NULL)
  {
    m_preCharacterControllerTask.setBucket(this);
    m_finaliseTask.setBucket(this);
  }

  MorphemeTaskNoPhysicsUpdatePreCharacterController* getPreCharacterControllerTask()
  {
    return &m_preCharacterControllerTask;
  }
  MorphemeTaskNoPhysicsUpdateFinalise* getFinaliseTask()
  {
    return &m_finaliseTask;
  }

protected:
  MorphemeTaskNoPhysicsUpdatePreCharacterController m_preCharacterControllerTask;
  MorphemeTaskNoPhysicsUpdateFinalise               m_finaliseTask;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class Game::CharacterManagerNoPhysicsThreaded
/// \brief Provides a simplified interface to managing and updating a set of NoPhysics morpheme characters
///  across multiple threads.
//----------------------------------------------------------------------------------------------------------------------
class CharacterManagerNoPhysicsThreaded : public CharacterManagerNoPhysics
{
public:

  CharacterManagerNoPhysicsThreaded();
  virtual ~CharacterManagerNoPhysicsThreaded();
 
  /// \brief Allocate required memory and initialise.
  virtual void init() NM_OVERRIDE;
  void init(uint32_t numBuckets);

  /// \brief Free allocated memory and shutdown.
  virtual void term() NM_OVERRIDE;
      
  /// \brief Create a CharacterNoPhysics and register it with this manager.
  ///
  /// Characters that are created via the Manager are automatically registered with the manager
  ///  and the memory management of the Character remains the responsibility of the Manager.
  CharacterNoPhysics* createCharacterMultiThreaded(
    CharacterDef*       characterDef,       ///< Must have been registered with this manager.
    uint32_t            bucketIndex,        ///< Which bucket to put this character in to.
    const NMP::Vector3& initialPosition = NMP::Vector3::InitZero,
    const NMP::Quat&    initialOrientation = NMP::Quat::kIdentity,
    MR::AnimSetIndex    initialAnimSetIndex = 0,
    const char*         name = "");
  
  /// \brief How many buckets does this manager have.
  uint32_t getNumBuckets() { return ((uint32_t) m_bucketsVector.size()); }

  /// \brief 
  MorphemeBucketNoPhysics* getBucket(uint32_t bucketIndex);

  /// \brief Starts the specified number of threads. Each thread starts to poll the ThreadScheduler for tasks to process.
  void startThreadScheduler(uint32_t numThreads); 

  /// \brief Stops the threads that the ThreadScheduler has active.
  void stopThreadScheduler();

  /// \brief Help process any remaining tasks on the queue then when it is empty
  ///  wait for all threads to finish any remaining task execution.
  void waitForAllScheduledTasks();

  /// \brief
  void registerCharacter(CharacterNoPhysics* character, uint32_t bucketIndex);

  /// \brief
  void unregisterCharacter(CharacterNoPhysics* character);

  /// \brief
  void destroyCharacter(CharacterNoPhysics* character);
    
  //----------------------------
  // Update functions.
  //----------------------------
  /// \brief
  void schedulePreCharacterControllerTasks(float deltaTime);

  /// \brief
  void schedulePostCharacterControllerTasks(float deltaTime);
    
  /// \brief
  void scheduleFinaliseTasks(float deltaTime);

  /// \brief
  void resetBucketTemporaryMemoryAllocators();
    
protected:

  /// \brief Initialise buckets to support multi-threaded update of characters.
  void initBucketsVector(uint32_t numBuckets);

  /// \brief Destroy buckets.
  void termBucketsVector();

    
  /// A vector of buckets, each one containing a set of characters that share the same temporary memory allocator.
  /// Each bucket can be updated independently on a separate thread.
  typedef std::vector<MorphemeBucketNoPhysics*> BucketsVectorNoPhysics;
  BucketsVectorNoPhysics m_bucketsVector;

  /// Simple scheduler to load-balance the update of networks on several threads.
  ThreadScheduler        m_threadScheduler;

  /// Records which bucket each character has been put in to.
  ///  Each entry corresponds to an entry in m_characterList.
  typedef std::list<uint32_t> BucketIndexList;
  BucketIndexList        m_characterBucketIndexList;
};

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_MANAGER_NOPHYSICS_THREADED_H
//----------------------------------------------------------------------------------------------------------------------
