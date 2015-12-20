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
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMFastHeapAllocator.h"

#include "GameManagement/GameThreadScheduler.h"

#include <algorithm>
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
// GameThreadScheduler functions
//----------------------------------------------------------------------------------------------------------------------
ThreadScheduler::ThreadScheduler()
{
  m_activeWorkerCount = 0;
  m_threads.clear();
}

//----------------------------------------------------------------------------------------------------------------------
ThreadScheduler::~ThreadScheduler()
{
  // Check that the scheduler has been shut down already.
  NMP_ASSERT(m_threads.empty());
}

//----------------------------------------------------------------------------------------------------------------------
void ThreadScheduler::start(
  uint32_t               numThreads,
  WorkerThreadEntryFnPtr inputWorkerThreadEntryFnPtr)
{
  // Make sure we haven't already been started.
  NMP_ASSERT(m_threads.empty());

  m_activeWorkerCount = 0;

  // Create the shut down event handle. 
  m_shutDownEvent = CreateEvent(
                        0,     // Default security attributes.
                        TRUE,  // Manual-reset event
                        FALSE, // Initial state is non signaled.
                        0);    // Object name.

  // if we haven't been provided with a worker thread entry function use our own.
  WorkerThreadEntryFnPtr workerThreadEntryFnPtr = workerThreadEntryFunction;
  if (inputWorkerThreadEntryFnPtr)
    workerThreadEntryFnPtr = inputWorkerThreadEntryFnPtr;

  // Create the threads.
  //  Note: we create numThreads-1 worker threads. This is because 
  for (uint32_t i = 1; i < numThreads; ++i)
  {
    ThreadID threadID = CreateThread(
                          0,                                  // Default security.
                          0,                                  // Default stack size.
                          (LPTHREAD_START_ROUTINE)workerThreadEntryFnPtr, // Thread function pointer. 
                          this,                               // Thread function parameter.
                                                              //  We pass the ThreadScheduler as a parameter to the thread.
                          0,                                  // Default startup flags.
                          0);
    m_threads.push_back(threadID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ThreadScheduler::stop()
{
  // Make sure we have been started.
  NMP_ASSERT(!m_threads.empty());

  // Flag to all the threads that they should exit once there are no more tasks to execute on the queue.
  SetEvent(m_shutDownEvent);

  // Wait for all worker threads to exit.
  for (ThreadsVector::iterator iter = m_threads.begin(); iter != m_threads.end(); ++iter)
  {
    WaitForSingleObject((*iter), INFINITE); // A thread signals when it finishes execution.
  }

  // Close all worker thread handles.
  for (ThreadsVector::iterator iter = m_threads.begin(); iter != m_threads.end(); ++iter)
  {
    CloseHandle((*iter));
  }

  // Clear the vector of thread handles.
  m_threads.clear();

  // Close the shut down event handle.
  CloseHandle(m_shutDownEvent);
}

//----------------------------------------------------------------------------------------------------------------------
void ThreadScheduler::queueMorphemeTask(MorphemeTask* const task)
{
  m_taskQueue.pushBack(task);
}

//----------------------------------------------------------------------------------------------------------------------
void ThreadScheduler::waitForAllMorphemeTasks()
{
  // If there are any remaining tasks on the queue lets help by processing them ourselves.
  MorphemeTask* task = NULL;
  while ((task = m_taskQueue.popFront(&m_activeWorkerCount)) != NULL)
  {
    task->run();

    // Flag that we are no longer an active worker thread.
    // (The pop had previously incremented this count).
    InterlockedDecrement(&m_activeWorkerCount);
  }

  // There are no tasks on the queue, but we must still wait until the m_activeWorkerCount has gone to zero
  //  i.e. all tasks have finished execution.
  while (InterlockedCompareExchange(&m_activeWorkerCount, 0, 0) > 0);
}

//----------------------------------------------------------------------------------------------------------------------
DWORD ThreadScheduler::workerThreadEntryFunction(void* param)
{
  ThreadScheduler* const worker = static_cast<ThreadScheduler* const>(param);
  worker->workerThreadFunction();

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ThreadScheduler::workerThreadFunction()
{
  // While the m_shutDownEvent has not been flagged
  //  pole the task queue every millisecond for a task to process.
  const uint32_t MAX_WAIT_FOR_EXIT_MILLI_SECONDS = 1;
  while (WaitForSingleObject(m_shutDownEvent, MAX_WAIT_FOR_EXIT_MILLI_SECONDS) != WAIT_OBJECT_0)
  {
    // Loop while there is work to do in the queue.
    MorphemeTask* task = NULL;
    while ((task = m_taskQueue.popFront(&m_activeWorkerCount)) != NULL)
    {
      // Execute the task.
      task->run();

      // Flag that we are no longer an active worker thread.
      // (The pop had previously incremented this count).
      InterlockedDecrement(&m_activeWorkerCount);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
ThreadScheduler::TaskQueue::TaskQueue()
{
  // Create a semaphore to allow multi-threaded access to the queue.
  m_queueSemaphore = CreateSemaphore(0, 1, 1, 0);
}

//----------------------------------------------------------------------------------------------------------------------
ThreadScheduler::TaskQueue::~TaskQueue()
{
  CloseHandle(m_queueSemaphore);
}

//----------------------------------------------------------------------------------------------------------------------
void ThreadScheduler::TaskQueue::pushBack(MorphemeTask* const task)
{
  // Lock access to the queue with the semaphore.
#ifdef NM_DEBUG
  BOOL result = 
#endif // NM_DEBUG
    WaitForSingleObject(m_queueSemaphore, INFINITE);
  NMP_ASSERT(result == WAIT_OBJECT_0);

  m_queue.push_back(task);

  // Allow access to the queue by releasing the semaphore.
#ifdef NM_DEBUG
  BOOL releaseResult = 
#endif // NM_DEBUG
    ReleaseSemaphore(m_queueSemaphore, 1, 0);
  NMP_ASSERT(releaseResult);
}

//----------------------------------------------------------------------------------------------------------------------
MorphemeTask* ThreadScheduler::TaskQueue::popFront(long* activeWorkerCount)
{
  MorphemeTask* task = NULL;

  // Lock access to the queue with the semaphore.
#ifdef NM_DEBUG
  BOOL result = 
#endif // NM_DEBUG
    WaitForSingleObject(m_queueSemaphore, INFINITE);
  NMP_ASSERT(result == WAIT_OBJECT_0);

  if (!m_queue.empty())
  {
    // Flag that this thread has just become active, processing a task.
    //  This count is decremented by this thread on task completion.
    InterlockedIncrement(activeWorkerCount);

    task = m_queue.front();
    m_queue.pop_front();
  }

  // Allow access to the queue by releasing the semaphore.
#ifdef NM_DEBUG
  BOOL releaseResult = 
#endif // NM_DEBUG
    ReleaseSemaphore(m_queueSemaphore, 1, 0);
  NMP_ASSERT(releaseResult);

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
bool ThreadScheduler::TaskQueue::isEmpty()
{
  // Lock access to the queue with the semaphore.
#ifdef NM_DEBUG
  BOOL result = 
#endif // NM_DEBUG
    WaitForSingleObject(m_queueSemaphore, INFINITE);
  NMP_ASSERT(result == WAIT_OBJECT_0);

  bool isEmpty = m_queue.empty();
  
  // Allow access to the queue by releasing the semaphore.
#ifdef NM_DEBUG
  BOOL releaseResult = 
#endif // NM_DEBUG
    ReleaseSemaphore(m_queueSemaphore, 1, 0);
  NMP_ASSERT(releaseResult);

  return isEmpty;
}

//----------------------------------------------------------------------------------------------------------------------
// GameMorphemeBucket functions
//----------------------------------------------------------------------------------------------------------------------
MorphemeBucket::MorphemeBucket(NMP::TempMemoryAllocator* tempAllocator)
{
  m_characterList.clear();
  m_tempAllocator = tempAllocator;
  m_ownAllocator = false;

  if (m_tempAllocator == NULL)
  {
    // We have not been provided with a temporary memory allocator so create our own.
    m_ownAllocator = true;
    NMP::Memory::Format allocatorFormat = NMP::FastHeapAllocator::getMemoryRequirements(
                                                DEFAULT_TEMP_ALLOCATOR_SIZE,
                                                NMP_VECTOR_ALIGNMENT);
    NMP::Memory::Resource tempAllocRes = NMPMemoryAllocateFromFormat(allocatorFormat);
    m_tempAllocator = NMP::FastHeapAllocator::init(
                                                tempAllocRes,
                                                DEFAULT_TEMP_ALLOCATOR_SIZE,
                                                NMP_VECTOR_ALIGNMENT);
  }
}

//----------------------------------------------------------------------------------------------------------------------
MorphemeBucket::~MorphemeBucket()
{
  if (m_ownAllocator)
  {
    // The temporary memory allocator is our own so we need to destroy it.
    NMP_ASSERT(m_tempAllocator);
    NMP::Memory::memFree(m_tempAllocator);
    m_tempAllocator = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
const CharacterList& MorphemeBucket::getCharacterList() const
{
  return m_characterList;
}

//----------------------------------------------------------------------------------------------------------------------
void MorphemeBucket::addCharacter(Character* character)
{
  NMP_ASSERT(character);

  // Make sure the Character is not already registered.
  CharacterList::iterator iter = std::find(m_characterList.begin(), m_characterList.end(), character);
  NMP_ASSERT(iter == m_characterList.end()); 

  // Add to the list.
  m_characterList.push_back(character);
  NMP_ASSERT(character->getNetwork());
  character->getNetwork()->setTempMemoryAllocator(m_tempAllocator);
}

//----------------------------------------------------------------------------------------------------------------------
void MorphemeBucket::removeCharacter(Character* character)
{
  NMP_ASSERT(character);
  // Find the Character in the list.
  CharacterList::iterator iter = std::find(m_characterList.begin(), m_characterList.end(), character);
  NMP_ASSERT((iter != m_characterList.end()) && ((*iter) == character)); // Make sure the Character is in the registered list.

  // Remove the entry from the list; we do not destroy the Character itself.
  m_characterList.erase(iter);
}

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
