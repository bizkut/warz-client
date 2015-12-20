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
#ifndef GAME_CHARACTER_MANAGER_EUPHORIA_THREADED_H
#define GAME_CHARACTER_MANAGER_EUPHORIA_THREADED_H
//----------------------------------------------------------------------------------------------------------------------
#include "GameManagement/GameThreadScheduler.h"
#include "GameManagement/Euphoria/GameCharacterEuphoria.h"
#include "GameManagement/Euphoria/GameCharacterManagerEuphoria.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MorphemeTaskUpdateToPreCharacterController
//----------------------------------------------------------------------------------------------------------------------
class MorphemeTaskPhysX3UpdatePreCharacterController : public MorphemeBucketTask
{
public:
  void run() NM_OVERRIDE;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MorphemeTaskPhysX3UpdatePostCharacterController
//----------------------------------------------------------------------------------------------------------------------
class MorphemeTaskPhysX3UpdatePostCharacterController : public MorphemeBucketTask
{
public:
  void run() NM_OVERRIDE;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MorphemeTaskUpdatePostPhysics
//----------------------------------------------------------------------------------------------------------------------
class MorphemeTaskPhysX3UpdateFinalise : public MorphemeBucketTask
{
public:
  void run() NM_OVERRIDE;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MorphemeBucketPhysX3
//----------------------------------------------------------------------------------------------------------------------
class MorphemeBucketPhysX3 : public MorphemeBucket
{
public:
  MorphemeBucketPhysX3() :
    MorphemeBucket(NULL)
  {
    m_preCharacterControllerTask.setBucket(this);
    m_postCharacterControllerTask.setBucket(this);
    m_finaliseTask.setBucket(this);
  }

  MorphemeTaskPhysX3UpdatePreCharacterController* getPreCharacterControllerTask()
  {
    return &m_preCharacterControllerTask;
  }
  MorphemeTaskPhysX3UpdatePostCharacterController* getPostCharacterControllerTask()
  {
    return &m_postCharacterControllerTask;
  }
  MorphemeTaskPhysX3UpdateFinalise* getFinaliseTask()
  {
    return &m_finaliseTask;
  }

protected:
  MorphemeTaskPhysX3UpdatePreCharacterController  m_preCharacterControllerTask;
  MorphemeTaskPhysX3UpdatePostCharacterController m_postCharacterControllerTask;
  MorphemeTaskPhysX3UpdateFinalise                m_finaliseTask;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class CharacterManagerEuphoriaThreaded
/// \brief Provides a simplified interface to managing and updating a set of PhysX3 morpheme characters
///  across multiple threads.
//----------------------------------------------------------------------------------------------------------------------
class CharacterManagerEuphoriaThreaded : public CharacterManagerEuphoria
{
public:

  CharacterManagerEuphoriaThreaded();
  virtual ~CharacterManagerEuphoriaThreaded();
 
  /// \brief Allocate required memory and initialise.
  virtual void init() NM_OVERRIDE;
  void init(uint32_t numBuckets);

  /// \brief Free allocated memory and shutdown.
  virtual void term() NM_OVERRIDE;

  /// \brief Create a CharacterEuphoria and register it with this manager.
  ///
  /// Characters that are created via the Manager are automatically registered with the manager
  ///  and the memory management of the Character remains the responsibility of the Manager.
  /// The manager must already have been attached to a PhysX scene.
  CharacterEuphoria* createCharacterMultiThreaded(
    CharacterDefEuphoria*      characterDefEuphoria,  ///< Must have been registered with this manager.
    uint32_t                   bucketIndex,           ///< Which bucket to put this character in to.
    MR::PhysicsRigPhysX3::Type physicsRigType = MR::PhysicsRigPhysX3::TYPE_ARTICULATED, ///< The type of physics rig to create,
                                                                                        ///<  MR::PhysicsRigPhysX3::TYPE_ARTICULATED or 
                                                                                        ///<  MR::PhysicsRigPhysX3::TYPE_JOINTED.
    physx::PxMaterial*         characterControllerMaterial = NULL, ///< The material to use for the character controller we create
                                                                   ///<  (a default will be created if one is not provided).
    const NMP::Vector3&        initialPosition = NMP::Vector3::InitZero,
    const NMP::Quat&           initialOrientation = NMP::Quat::kIdentity,
    MR::AnimSetIndex           initialAnimSetIndex = 0,
    const char*                name = "");

  /// \brief How many buckets does this manager have.
  uint32_t getNumBuckets() { return ((uint32_t) m_bucketsVector.size()); }

  /// \brief 
  MorphemeBucketPhysX3* getBucket(uint32_t bucketIndex);

  /// \brief Starts the specified number of threads. Each thread starts to poll the ThreadScheduler for tasks to process.
  void startThreadScheduler(uint32_t numThreads); 

  /// \brief Stops the threads that the ThreadScheduler has active.
  void stopThreadScheduler();

  /// \brief Help process any remaining tasks on the queue then when it is empty
  ///  wait for all threads to finish any remaining task execution.
  void waitForAllScheduledTasks();

  /// \brief
  void registerCharacter(CharacterEuphoria* character, uint32_t bucketIndex);

  /// \brief
  void unregisterCharacter(CharacterEuphoria* character);

  /// \brief
  void destroyCharacter(CharacterEuphoria* character);
    
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
  typedef std::vector<MorphemeBucketPhysX3*> BucketsVectorPhysX3;
  BucketsVectorPhysX3   m_bucketsVector;

  /// Simple scheduler to load-balance the update of networks on several threads.
  ThreadScheduler       m_threadScheduler;

  /// Records which bucket each character has been put in to.
  ///  Each entry corresponds to an entry in m_characterList.
  typedef std::list<uint32_t> BucketIndexList;
  BucketIndexList       m_characterBucketIndexList;
};

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_MANAGER_EUPHORIA_THREADED_H
//----------------------------------------------------------------------------------------------------------------------
