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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef GAME_CHARACTER_MANAGER_MT_ALLOC_H
#define GAME_CHARACTER_MANAGER_MT_ALLOC_H
//----------------------------------------------------------------------------------------------------------------------
#include "GameManagement/GameThreadScheduler.h"
#include "GameManagement/PhysX2/GameCharacterPhysX2.h"
#include "GameManagement/PhysX2/GameCharacterManagerPhysX2.h"

#include "MultiThreadTempAllocator.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MorphemeTaskUpdatePreCharacterController
//----------------------------------------------------------------------------------------------------------------------
class MorphemeTaskUpdatePreCharacterController : public MorphemeTask
{
public:
  void run() NM_OVERRIDE;

  void setCharacter(Character* character){m_character = character;}

protected:
  Character* m_character;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MorphemeTaskUpdatePostCharacterController
//----------------------------------------------------------------------------------------------------------------------
class MorphemeTaskUpdatePostCharacterController : public MorphemeTask
{
public:
  void run() NM_OVERRIDE;

  void setCharacter(Character* character){m_character = character;}

protected:
  Character* m_character;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MorphemeTaskUpdateFinalise
//----------------------------------------------------------------------------------------------------------------------
class MorphemeTaskUpdateFinalise : public MorphemeTask
{
public:
  void run() NM_OVERRIDE;

  void setCharacter(Character* character){m_character = character;}

protected:
  Character* m_character;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class ThreadedMorphemeCharacter
//----------------------------------------------------------------------------------------------------------------------
class ThreadedMorphemeCharacter
{
public:
  ThreadedMorphemeCharacter(Character* character, NMP::TempMemoryAllocator* tempAllocator)
  {
    m_character = character;
    m_tempAllocator = tempAllocator;
    m_preCharacterControllerTask.setCharacter(m_character);
    m_postCharacterControllerTask.setCharacter(m_character);
    m_finaliseTask.setCharacter(m_character);
  }

  MorphemeTaskUpdatePreCharacterController* getPreCharacterControllerTask()
  {
    return &m_preCharacterControllerTask;
  }

  MorphemeTaskUpdatePostCharacterController* getPostCharacterControllerTask()
  {
    return &m_postCharacterControllerTask;
  }

  MorphemeTaskUpdateFinalise* getFinaliseTask()
  {
    return &m_finaliseTask;
  }

  Character* getCharacter()
  {
    return m_character;
  }

  NMP::TempMemoryAllocator* getTemporaryMemoryAllocator()
  {
    return m_tempAllocator;
  }

protected:
  Character* m_character;
  NMP::TempMemoryAllocator* m_tempAllocator;
  MorphemeTaskUpdatePreCharacterController  m_preCharacterControllerTask;
  MorphemeTaskUpdatePostCharacterController m_postCharacterControllerTask;
  MorphemeTaskUpdateFinalise                m_finaliseTask;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class CharacterManagerMTAlloc
///  This class implements a mechanism for updating morpheme networks across multiple threads.  It uses the thread
///  scheduler to queue up tasks which update a single network at a time.  Memory management for each network instance
///  is managed by a MultiThreadTempAllocator.  The CharacterManagerMTAlloc maintains a single threadsafe pool of 
///  memory pages which are served up to each threaded job on demand.
//----------------------------------------------------------------------------------------------------------------------
class CharacterManagerMTAlloc : public CharacterManagerPhysX2
{
public:

  CharacterManagerMTAlloc();
  virtual ~CharacterManagerMTAlloc();

  /// Characters that are created via the Manager are automatically registered with the manager
  ///  and the memory management of the Character remains the responsibility of the Manager.
  /// The manager must already have been attached to a PhysX scene.
  CharacterPhysX2* createCharacterMultiThreaded(
    CharacterDef*       characterDef,       ///< Must have been registered with this manager.
    const NMP::Vector3& initialPosition = NMP::Vector3::InitZero,
    const NMP::Quat&    initialOrientation = NMP::Quat::kIdentity,
    const NMP::Vector3& graveyardPosition = NMP::Vector3(0.0f, -1000.0f, 0.0f),
    MR::AnimSetIndex    initialAnimSetIndex = 0,
    const char*         name = "");

  /// \brief Starts the specified number of threads. Each thread starts to poll the ThreadScheduler for tasks to process.
  void startThreadScheduler(uint32_t numThreads); 

  /// \brief Stops the threads that the ThreadScheduler has active.
  void stopThreadScheduler();

  /// \brief Help process any remaining tasks on the queue then when it is empty
  ///  wait for all threads to finish any remaining task execution.
  void waitForAllScheduledTasks();

  //
  void registerCharacter(CharacterPhysX2* character, NMP::TempMemoryAllocator* tempAllocator);

  // 
  void unregisterCharacter(CharacterPhysX2* character);

  // 
  void destroyCharacter(CharacterPhysX2* character);
    
  //----------------------------
  // Update functions...
  //----------------------------
  // 
  void schedulePreCharacterControllerTasks(float deltaTime);

  // 
  void schedulePostCharacterControllerTasks(float deltaTime);
    
  // 
  void scheduleFinaliseTasks(float deltaTime);

  //
  void createSharedMemoryPool(uint32_t numPages, uint32_t pageSize);

  //
  void destroySharedMemoryPool();

  //
  void resetTemporaryMemoryAllocators();
    
protected:

  /// A vector of ThreadedMorphemeCharacters. Each Character can be updated independently on a separate thread.
  typedef std::vector<ThreadedMorphemeCharacter*> CharacterVector;
  CharacterVector       m_characterVector;

  ///
  ThreadSafeMemoryPool* m_threadSafeMemoryPool;

  ///
  ThreadScheduler       m_threadScheduler;
};

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_MANAGER_MT_ALLOC_H
//----------------------------------------------------------------------------------------------------------------------

