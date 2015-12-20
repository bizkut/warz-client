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
#ifndef GAME_THREAD_SCHEDULER_H
#define GAME_THREAD_SCHEDULER_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>

#include "GameManagement/GameCharacter.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
// 
typedef DWORD (*WorkerThreadEntryFnPtr)(void* param);

//----------------------------------------------------------------------------------------------------------------------
/// \class MorphemeBucket
/// \brief A container used to update a list of characters on a specific thread, sharing the fast heap allocator.
//----------------------------------------------------------------------------------------------------------------------
class MorphemeBucket
{
public:
  MorphemeBucket(NMP::TempMemoryAllocator* tempAllocator = 0);
  virtual ~MorphemeBucket();
    
  const CharacterList& getCharacterList() const;

  void addCharacter(Character* character);
  void removeCharacter(Character* character);

  NMP::TempMemoryAllocator* getTemporaryMemoryAllocator() const { return m_tempAllocator; }

protected:
  static const size_t DEFAULT_TEMP_ALLOCATOR_SIZE = 2 * 1024 * 1024; ///< If we need to create our own temporary memory 
                                                                     ///<  allocator this is the size we will make it.
  NMP::TempMemoryAllocator* m_tempAllocator; ///< A temporary memory allocator shared by all characters in the bucket.
  bool                      m_ownAllocator;  ///< Indicates if we are using our own temporary memory allocator, 
                                             ///<  or if it was provided to us.

  CharacterList           m_characterList; ///< All characters in this bucket.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MorphemeTask
/// \brief Base class for threaded task managed by the GameThreadScheduler.
//----------------------------------------------------------------------------------------------------------------------
class MorphemeTask
{
public:
  MorphemeTask() :
    m_deltaTime(1.0f/60.0f)
 {
 }

 virtual ~MorphemeTask() {}

 /// This class is to be inherited from with an implementation for run().
 virtual void run() = 0;

 float getDeltaTime() const { return m_deltaTime; }
 void setDeltaTime(float deltaTime) { m_deltaTime = deltaTime; }

protected:
  float           m_deltaTime;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MorphemeBucketTask
/// \brief Base class for threaded task managed by the ThreadScheduler which uses MorphemeBucket.
//----------------------------------------------------------------------------------------------------------------------
class MorphemeBucketTask : public MorphemeTask
{
public:
  MorphemeBucketTask() : m_bucket(NULL) {}

  virtual ~MorphemeBucketTask() {}

  /// This class is to be inherited from with an implementation for run().
  /// In a bucketted task, run() will generally perform an operation on all characters in the task bucket.
  virtual void run() = 0;

  void setBucket(MorphemeBucket* bucket) { m_bucket = bucket; }

protected:
  MorphemeBucket* m_bucket;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class ThreadScheduler
/// \brief Simple scheduler to load-balance the update of networks on several threads.
//----------------------------------------------------------------------------------------------------------------------
class ThreadScheduler
{
public:
  ThreadScheduler();
  ~ThreadScheduler();

  /// \brief Create a set of worker threads.
  void start(
    uint32_t               numThreads,
    WorkerThreadEntryFnPtr workerThreadEntryFnPtr = NULL); ///< Will use workerThreadEntryFunction if a function pointer is not provided.

  /// \brief Once all task execution is complete shut down all threads.
  void stop();

  /// \brief Add a task to te queue.
  void queueMorphemeTask(MorphemeTask* const task);

  /// \brief Help with any remaining tasks on the queue then when it is empty wait for all threads to finish task execution.
  void waitForAllMorphemeTasks();

  /// \brief Polls the task queue for a task to process.
  ///
  /// Exits the thread when the m_shutDownEvent has not been flagged.
  void workerThreadFunction();

  /// Is this scheduler running.
  bool isActive() { return !m_threads.empty(); }

private:

  /// \brief Passed to CreateThread as the starting address for a worker thread.
  ///
  /// This is static so that the function address is fixed.
  /// The ThreadScheduler instance is passed in the param parameter.
  /// workerThreadFunction is called on this function.
  static DWORD workerThreadEntryFunction(void* param);
      
  //----------------------
  /// \class TaskQueue
  /// \brief Holds a thread safe list of tasks to be performed. 
  ///  Tasks can pushed and popped from multiple concurrent threads.
  //----------------------
  class TaskQueue
  {
  public:
    TaskQueue();
    ~TaskQueue();

    /// \brief Push a MorphemeTask on to the back of the queue. Thread safe.
    void pushBack(MorphemeTask* const task);
    
    /// \brief Pop a MorphemeTask off of the front of the queue. Thread safe.
    ///
    ///  If a task is popped then indicate that this thread has just become active, processing a task,
    ///  by incrementing a thread safe activeWorkerCount.
    ///  This count is decremented by this thread on task completion.
    MorphemeTask* popFront(long* activeWorkerCount);

    /// \brief Is the queue empty. Thread safe.
    bool isEmpty();

  private:
    std::list<MorphemeTask*> m_queue;          ///< The task queue.    
    HANDLE                   m_queueSemaphore; ///< Allows safe multi-threaded access to the queue.
  };

  /// Type definition for a vector of thread HANDLEs.
  typedef HANDLE ThreadID;
  typedef std::vector<ThreadID> ThreadsVector;

  ThreadsVector m_threads;            ///<
  TaskQueue     m_taskQueue;          ///< 

  HANDLE        m_shutDownEvent;      ///< Global shut down event handle. When this event has been flagged all 
                                      ///<  worker threads complete their current tasks and exit.
  long          m_activeWorkerCount;  ///< Global count of all of the threads that are currently executing a task.
};

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_THREAD_SCHEDULER_H
//----------------------------------------------------------------------------------------------------------------------
