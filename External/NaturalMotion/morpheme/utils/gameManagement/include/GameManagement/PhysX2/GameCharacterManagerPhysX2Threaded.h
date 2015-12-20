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
#ifndef GAME_CHARACTER_MANAGER_PHYSX2_THREADED_H
#define GAME_CHARACTER_MANAGER_PHYSX2_THREADED_H
//----------------------------------------------------------------------------------------------------------------------
#include "GameManagement/GameThreadScheduler.h"
#include "GameManagement/PhysX2/GameCharacterPhysX2.h"
#include "GameManagement/PhysX2/GameCharacterManagerPhysX2.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MorphemeTaskUpdateToPreCharacterController
//----------------------------------------------------------------------------------------------------------------------
class MorphemeTaskPhysX2UpdatePreCharacterController : public MorphemeBucketTask
{
public:
  void run() NM_OVERRIDE;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MorphemeTaskPhysX2UpdatePostCharacterController
//----------------------------------------------------------------------------------------------------------------------
class MorphemeTaskPhysX2UpdatePostCharacterController : public MorphemeBucketTask
{
public:
  void run() NM_OVERRIDE;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MorphemeTaskUpdatePostPhysics
//----------------------------------------------------------------------------------------------------------------------
class MorphemeTaskPhysX2UpdateFinalise : public MorphemeBucketTask
{
public:
  void run() NM_OVERRIDE;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MorphemeBucketPhysX2
//----------------------------------------------------------------------------------------------------------------------
class MorphemeBucketPhysX2 : public MorphemeBucket
{
public:
  MorphemeBucketPhysX2() :
    MorphemeBucket(NULL)
  {
    m_preCharacterControllerTask.setBucket(this);
    m_postCharacterControllerTask.setBucket(this);
    m_finaliseTask.setBucket(this);
  }

  MorphemeTaskPhysX2UpdatePreCharacterController* getPreCharacterControllerTask()
  {
    return &m_preCharacterControllerTask;
  }
  MorphemeTaskPhysX2UpdatePostCharacterController* getPostCharacterControllerTask()
  {
    return &m_postCharacterControllerTask;
  }
  MorphemeTaskPhysX2UpdateFinalise* getFinaliseTask()
  {
    return &m_finaliseTask;
  }

protected:
  MorphemeTaskPhysX2UpdatePreCharacterController  m_preCharacterControllerTask;
  MorphemeTaskPhysX2UpdatePostCharacterController m_postCharacterControllerTask;
  MorphemeTaskPhysX2UpdateFinalise                m_finaliseTask;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class CharacterManagerPhysX2Threaded
/// \brief Provides a simplified interface to managing and updating a set of PhysX2 morpheme characters
///  across multiple threads.
//----------------------------------------------------------------------------------------------------------------------
class CharacterManagerPhysX2Threaded : public CharacterManagerPhysX2
{
public:

  CharacterManagerPhysX2Threaded();
  virtual ~CharacterManagerPhysX2Threaded();
 
  /// \brief Allocate required memory and initialise.
  virtual void init() NM_OVERRIDE;
  void init(uint32_t numBuckets);

  /// \brief Free allocated memory and shutdown.
  virtual void term() NM_OVERRIDE;

  /// \brief Create a CharacterPhysX2 and register it with this manager.
  ///
  /// Characters that are created via the Manager are automatically registered with the manager
  ///  and the memory management of the Character remains the responsibility of the Manager.
  /// The manager must already have been attached to a PhysX scene.
  CharacterPhysX2* createCharacterMultiThreaded(
    CharacterDef*       characterDef,       ///< Must have been registered with this manager.
    uint32_t            bucketIndex,        ///< Which bucket to put this character in to.
    const NMP::Vector3& initialPosition = NMP::Vector3::InitZero,
    const NMP::Quat&    initialOrientation = NMP::Quat::kIdentity,
    const NMP::Vector3& graveyardPosition = NMP::Vector3(0.0f, -1000.0f, 0.0f),
    MR::AnimSetIndex    initialAnimSetIndex = 0,
    const char*         name = "");

  /// \brief How many buckets does this manager have.
  uint32_t getNumBuckets() { return ((uint32_t) m_bucketsVector.size()); }

  /// \brief 
  MorphemeBucketPhysX2* getBucket(uint32_t bucketIndex);

  /// \brief Starts the specified number of threads. Each thread starts to poll the ThreadScheduler for tasks to process.
  void startThreadScheduler(uint32_t numThreads); 

  /// \brief Stops the threads that the ThreadScheduler has active.
  void stopThreadScheduler();

  /// \brief Help process any remaining tasks on the queue then when it is empty
  ///  wait for all threads to finish any remaining task execution.
  void waitForAllScheduledTasks();

  /// \brief
  void registerCharacter(CharacterPhysX2* character, uint32_t bucketIndex);

  /// \brief
  void unregisterCharacter(CharacterPhysX2* character);

  /// \brief
  void destroyCharacter(CharacterPhysX2* character);
    
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
  typedef std::vector<MorphemeBucketPhysX2*> BucketsVectorPhysX2;
  BucketsVectorPhysX2   m_bucketsVector;

  /// Simple scheduler to load-balance the update of networks on several threads.
  ThreadScheduler       m_threadScheduler;

  /// Records which bucket each character has been put in to.
  ///  Each entry corresponds to an entry in m_characterList.
  typedef std::list<uint32_t> BucketIndexList;
  BucketIndexList       m_characterBucketIndexList;
};

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_MANAGER_PHYSX2_THREADED_H
//----------------------------------------------------------------------------------------------------------------------
