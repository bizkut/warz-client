// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
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
#ifndef MR_TASK_QUEUE_H
#define MR_TASK_QUEUE_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMRingBuffer.h"
#include "NMPlatform/NMVectorContainer.h"
#include "morpheme/mrTask.h"
//----------------------------------------------------------------------------------------------------------------------

#define MR_TASK_QUEUE_SEED_SIZE 8 /// < This value determines how much space to allocate initially for queued tasks 

//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

// forward decls
class Network;
struct PreAllocatedTask;

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TaskQueue
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class TaskQueue
{
private:
  TaskQueue();
public:

  /// \brief allows you to construct a new TaskQueue from an existing set of tasks
  TaskQueue(uint32_t numTasks, Task** taskList);

  static NMP::Memory::Format getMemoryRequirements();
  static MR::TaskQueue* init(NMP::Memory::Resource res, Network* owningNetwork);

  /// \brief Initialise the TaskQueue for a new frame of network tasks.
  ///
  ///  Note: This function must be called before adding any tasks to the queue.
  void startQueuing();

  void finishQueuing();

  /// \brief Do not allow more than one task of the same ID belonging to the same Node on the task list.
  Task* createNewTaskOnQueue(
    TaskID           taskID,
    NodeID           owningNodeID,
    uint32_t         numParameters,
    TaskParameter*   dependentParam,
    bool             externalTask = false,
    bool             supportsRemoteEval = true,
    bool             managesOwnDMA = false,
    bool             referenceTask = false);

  /// \brief The root seed (dummy) tasks are the only tasks that be queued without a dependent.
  Task* createNewRootTaskOnQueue(
    TaskID         taskID,
    NodeID         owningNodeID,
    uint32_t       numParameters,
    bool           externalTask = false,      // true if this is an external task
    bool           supportsRemoteEval = true, // True if the task supports remote evaluation (e.g. via the SPU).
    bool           managesOwnDMA = false      // True if the task DMAs its own parameters
  );

  void debugOutputQueueContents(const char** taskNames);
  void debugOutputTaskContents(Task* task, const char** taskNames);

  /// \brief add a task to the queue's task list
  void addToTaskList(Task* newTask);

  /// \brief Wipe the specified task from the tasks array,
  ///
  /// Note that this is generally only used when debugging queue processing problems in order to maintain an
  /// accurate list of tasks that have not yet been processed. It is too slow to use permanently.
  void clearTaskFromTasksArray(Task* task);

  /// \brief Release any tasks at the end of dispatch.
  bool releaseTasks();

  /// \brief Release any temporary memory allocated.
  bool freeTempData();

  /// \brief Expand the task list to make more room to queue tasks.  This is done with a simple realloc in the
  ///  network's fast heap.
  void expandTaskList();

  /// \brief Get the task at the indexed position in the queue.
  NM_INLINE Task* getTask(uint32_t index) {return m_tasks[index];};

  Task**                   m_tasks;               ///< Tasks to be processed are added to list array.

  uint32_t                 m_numQueuedTasks;      ///< The number of tasks that have been queued up in m_tasks.
  uint32_t                 m_taskQueueSize;       ///< The current size of the task queue pointer to by m_tasks.
  uint32_t                 m_maxQueuedTasks;      ///< This records the most tasks that were ever queued by this task queue.
  uint32_t                 m_numUnprocessedTasks; ///< The total number of tasks that were queued but have not been processed.
  Network*                 m_owningNetwork;
};

} // namespace MR
//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TASK_QUEUE_H
//----------------------------------------------------------------------------------------------------------------------
