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
#include "morpheme/mrNetwork.h"
#include "morpheme/mrNetworkLogger.h"
#include "morpheme/mrDispatcher.h"
#if defined(NM_HOST_CELL_PPU)
  #include "morpheme/mrDispatcherPS3.h"
#endif // defined(NM_HOST_CELL_PPU)
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// MR::TaskQueue
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
TaskQueue::TaskQueue(uint32_t numTasks, Task** taskList)
{
  m_numQueuedTasks = numTasks;
  m_tasks = taskList;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TaskQueue::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(TaskQueue), NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::TaskQueue* TaskQueue::init(NMP::Memory::Resource res, Network* owningNetwork)
{
  MR::TaskQueue* result = (MR::TaskQueue*)res.ptr;

  res.increment(NMP::Memory::Format(sizeof(TaskQueue), NMP_VECTOR_ALIGNMENT));

  // The task queue itself will be allocated from the temporary memory allocator of the owning network later on.
  result->m_tasks = NULL;

  result->m_owningNetwork = owningNetwork;
  result->m_numQueuedTasks = 0;
  result->m_taskQueueSize = 0;
  result->m_maxQueuedTasks = MR_TASK_QUEUE_SEED_SIZE;
  result->m_numUnprocessedTasks = 0;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
Task* TaskQueue::createNewRootTaskOnQueue(
  TaskID         taskID,
  NodeID         owningNodeID,
  uint32_t       numParameters,
  bool           externalTask,       // true if this is an external task
  bool           supportsRemoteEval, // True if the task supports remote evaluation (e.g. via the SPU).
  bool           managesOwnDMA)      // True if the task DMAs its own parameters
{
  // Check it is not already queued
  NodeBin* nodeBin = m_owningNetwork->getNodeBin(owningNodeID);
  NMP_ASSERT(nodeBin->findQueuedTaskID(taskID) == NULL);

  NMP::Memory::Format taskFmt = Task::getMemoryRequirements(numParameters);
  NMP::Memory::Resource taskResource = NMPAllocatorAllocateFromFormat(m_owningNetwork->getTempMemoryAllocator(), taskFmt);
  NMP_ASSERT(taskResource.ptr);  // Check the memory has been allocated successfully

  Task* newTask = Task::init(taskResource, numParameters, taskID, owningNodeID, externalTask, supportsRemoteEval, managesOwnDMA, m_owningNetwork->getTempMemoryAllocator(), false);

#if defined(NM_HOST_CELL_PPU)
  if (supportsRemoteEval)
  {
    SPUTaskFnMap sSPUTaskFnMap = DispatcherPS3::getSPUTaskFnMap();
    NMP_ASSERT_MSG( sSPUTaskFnMap.taskIDMap[taskID].taskFn, 
    "Task %s(%u) in node %u is marked as supportsRemoteEvaluation however no spu function was found.", m_owningNetwork->getDispatcher()->getTaskName(taskID), taskID, owningNodeID);
  }
#endif // defined(NM_HOST_CELL_PPU)

  NET_LOG_MESSAGE(99, "  Pushing Task %s (%i) onto the queue.\n", m_owningNetwork->getDispatcher()->getTaskName(taskID), taskID);

  nodeBin->addQueuedTask(newTask);

  return newTask;
}

//----------------------------------------------------------------------------------------------------------------------
Task* TaskQueue::createNewTaskOnQueue(
  TaskID         taskID,
  NodeID         owningNodeID,
  uint32_t       numParameters,
  TaskParameter* dependentParam,     // The param of the above task that depends on this task.
  bool           externalTask,       // true if this is an external task
  bool           supportsRemoteEval, // True if the task supports remote evaluation (e.g. via the SPU).
  bool           managesOwnDMA,      // True if the task DMAs its own parameters
  bool           referenceTask)
{
  Task* newTask = NULL;

  NodeBin* nodeBin = m_owningNetwork->getNodeBin(owningNodeID);
  Task* queuedTask = nodeBin->findQueuedTaskID(taskID);

  if (queuedTask)
  {
    // Task already exists on queue, don't add it again.
    NET_LOG_MESSAGE(99, "  NOT pushing Task %s (%i) onto the queue as it's already queued\n", m_owningNetwork->getDispatcher()->getTaskName(taskID), taskID);

    // Copy the new task's dependent task into the queued task.
    queuedTask->addDependentTask(dependentParam);
  }
  else
  {
    // Task does not already exist on the queue, so add it.
    NMP::Memory::Format taskFmt = Task::getMemoryRequirements(numParameters);
    NMP::Memory::Resource taskResource = NMPAllocatorAllocateFromFormat(m_owningNetwork->getTempMemoryAllocator(), taskFmt);
    NMP_ASSERT(taskResource.ptr);  // Check the memory has been allocated successfully
    newTask = Task::init(taskResource, numParameters, taskID, owningNodeID, externalTask, supportsRemoteEval, managesOwnDMA, m_owningNetwork->getTempMemoryAllocator(), referenceTask);

    NMP_ASSERT_MSG(m_owningNetwork->getDispatcher()->getTaskFunctionPointer(newTask),
                   "Attempting to queue a task that is not registered with this network's dispatcher!");

    NET_LOG_MESSAGE(99, "  Pushing Task %s (%i) onto the queue.\n", m_owningNetwork->getDispatcher()->getTaskName(taskID), taskID);
   
#if defined(NM_HOST_CELL_PPU)
    if (supportsRemoteEval)
    {
      SPUTaskFnMap sSPUTaskFnMap = DispatcherPS3::getSPUTaskFnMap();
      NMP_ASSERT_MSG( sSPUTaskFnMap.taskIDMap[taskID].taskFn, 
      "Task %s(%u) in node %u is marked as supportsRemoteEvaluation however no spu function was found.", m_owningNetwork->getDispatcher()->getTaskName(taskID), taskID, owningNodeID);
    }
#endif // defined(NM_HOST_CELL_PPU)

    // Add the dependent task.
    newTask->addDependentTask(dependentParam);

    nodeBin->addQueuedTask(newTask);
  }

  return newTask;
}

//----------------------------------------------------------------------------------------------------------------------
void TaskQueue::addToTaskList(Task* newTask)
{
  if (m_numQueuedTasks == m_taskQueueSize)
  {
    // We have filled up the task list.  It must be expanded.
    expandTaskList();
  }

  m_tasks[m_numQueuedTasks++] = newTask;
}

//----------------------------------------------------------------------------------------------------------------------
void TaskQueue::startQueuing()
{
  NMP_ASSERT(m_owningNetwork != NULL);
  NMP_ASSERT(m_tasks == NULL); // There should be no array of tasks allocated to this queue when we start.

  // We create an array of Task pointers as long as the maximum number of tasks ever queued (this is initialised with
  //  MR_TASK_QUEUE_SEED_SIZE).
  NMP::Memory::Format taskQueueFmt(m_maxQueuedTasks * sizeof(Task*), NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource taskQueueResource =
    NMPAllocatorAllocateFromFormat(m_owningNetwork->getTempMemoryAllocator(), taskQueueFmt);

  m_tasks = (Task**)taskQueueResource.ptr;
  NMP_ASSERT(m_tasks);
  m_taskQueueSize = m_maxQueuedTasks;

  return;
}

// The caching mechanism in finishQueuing requires that the
#if !defined(NM_HOST_64_BIT)
  NM_ASSERT_COMPILE_TIME(sizeof(AttribData) == 8);
#else
  NM_ASSERT_COMPILE_TIME(sizeof(AttribData) == 16);
#endif

//----------------------------------------------------------------------------------------------------------------------
void TaskQueue::finishQueuing()
{
  const uint32_t CACHE_SIZE = 256;
  AttribData* cache[CACHE_SIZE]; // This number has to be greater than the number of attrib types.
  
  for (uint32_t cacheIdx = 0; cacheIdx != CACHE_SIZE; ++cacheIdx)
  {
    cache[cacheIdx] = 0;
  }

  uint32_t targetTaskIndex = 0;

  for (uint32_t taskIdx = 0; taskIdx != m_numQueuedTasks; ++taskIdx)
  {
    Task* task = m_tasks[taskIdx];

    if (task->m_referenceTask)
    {
      NMP_ASSERT(task->m_numParams == 2);
      
      TaskParameter* param0 = task->m_params;
      TaskParameter* param1 = task->m_params + 1;

      NMP_ASSERT(param0->m_taskParamFlags.areSet(TPARAM_FLAG_INPUT));
      NMP_ASSERT(param1->m_taskParamFlags.areSet(TPARAM_FLAG_OUTPUT));
      NMP_ASSERT(param0->m_attribType == param1->m_attribType);

      if (param0->m_taskParamFlags.areSet(TPARAM_FLAG_DEPENDENT))
      {
        Task* dependentTask = (Task*)param0->m_attribDataHandle.m_attribData;

        TaskParameter* dependentParam = dependentTask->getOutputParameter(param0->m_attribAddress);

        NMP_ASSERT(dependentParam);
        NMP_ASSERT(dependentParam->m_attribDataHandle.m_attribData);
        NMP_ASSERT(param0->m_attribType == dependentParam->m_attribType);

        param0->m_attribDataHandle = dependentParam->m_attribDataHandle;
      }

      if ((param0->m_attribDataHandle.m_attribData->m_allocator == m_owningNetwork->getTempMemoryAllocator()) &&
        (param1->m_lifespan > 0))
      {
        AttribOutputMemReqsFn outputMemReqs = Manager::getInstance().getAttribOutputMemReqsFn(param1->m_attribType);
        NMP_ASSERT_MSG(outputMemReqs, "You need to register an OutputmemReqs function for type %d", param1->m_attribType);
        NMP::Memory::Format memReqs = (*outputMemReqs)(m_owningNetwork);
        NMP::MemoryAllocator* allocator = m_owningNetwork->getPersistentMemoryAllocator();
        NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);

        param1->m_attribDataHandle.m_attribData = (AttribData*)resource.ptr;
        param1->m_attribDataHandle.m_format = memReqs;

        param1->m_attribDataHandle.m_attribData->m_allocator = allocator;
        param1->m_attribDataHandle.m_attribData->setType(param1->m_attribType);
        param1->m_attribDataHandle.m_attribData->setRefCount(0);

        param1->m_taskParamFlags.set(TPARAM_FLAG_NEEDS_CREATE);

        m_tasks[targetTaskIndex] = task;
        ++targetTaskIndex;
      }
      else
      {
        param1->m_attribDataHandle = param0->m_attribDataHandle;
      }

      if (param1->m_lifespan)
      {
        m_owningNetwork->addAttribData(param1->m_attribAddress, param1->m_attribDataHandle, param1->m_lifespan);
      }
    }
    else
    {
      for (uint32_t paramIdx = 0; paramIdx < task->m_numParams; paramIdx++)
      {
        TaskParameter* param = task->m_params + paramIdx;

        if (param->m_taskParamFlags.areSet(TPARAM_FLAG_OUTPUT) && !param->m_attribDataHandle.m_attribData)
        {
          AttribOutputMemReqsFn outputMemReqs = Manager::getInstance().getAttribOutputMemReqsFn(param->m_attribType);
          NMP_ASSERT_MSG(outputMemReqs, "You need to register an OutputmemReqs function for type %d", param->m_attribType);
          NMP::Memory::Format memReqs = (*outputMemReqs)(m_owningNetwork);
          
          NMP::Memory::Resource resource;
          NMP::MemoryAllocator* allocator = 0;
          if (param->m_lifespan == 0 && cache[param->m_attribType])
          {
            allocator = m_owningNetwork->getTempMemoryAllocator();
            NMP_ASSERT(param->m_attribType < CACHE_SIZE);

            resource.ptr = cache[param->m_attribType];
            resource.format = memReqs;

            AttribData* nextCached = (AttribData*)(cache[param->m_attribType]->m_allocator);

            cache[param->m_attribType] = nextCached;
          }
          else
          {
            allocator = param->m_lifespan ? m_owningNetwork->getPersistentMemoryAllocator() : m_owningNetwork->getTempMemoryAllocator();
            resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
          }

          param->m_attribDataHandle.m_attribData = (AttribData*)resource.ptr;
          param->m_attribDataHandle.m_format = memReqs;

          param->m_attribDataHandle.m_attribData->m_allocator = allocator;
          param->m_attribDataHandle.m_attribData->setType(param->m_attribType);
          param->m_attribDataHandle.m_attribData->setRefCount(0);

          param->m_taskParamFlags.set(TPARAM_FLAG_NEEDS_CREATE);

          if (param->m_lifespan)
          {
            m_owningNetwork->addAttribData(param->m_attribAddress, param->m_attribDataHandle, param->m_lifespan);
          }
        }

        if (param->m_taskParamFlags.areSet(TPARAM_FLAG_EMITTED_CP) && !param->m_attribDataHandle.m_attribData)
        {
          AttribOutputMemReqsFn outputMemReqs = Manager::getInstance().getAttribOutputMemReqsFn(param->m_attribType);
          NMP_ASSERT_MSG(outputMemReqs, "You need to register an OutputmemReqs function for type %d", param->m_attribType);
          NMP::Memory::Format memReqs = (*outputMemReqs)(m_owningNetwork);

          NMP::Memory::Resource resource;
          NMP::MemoryAllocator* allocator = 0;
          if (param->m_lifespan == 0 && cache[param->m_attribType])
          {
            allocator = m_owningNetwork->getTempMemoryAllocator();
            NMP_ASSERT(param->m_attribType < CACHE_SIZE);

            resource.ptr = cache[param->m_attribType];
            resource.format = memReqs;

            AttribData* nextCached = (AttribData*)(cache[param->m_attribType]->m_allocator);

            cache[param->m_attribType] = nextCached;
          }
          else
          {
            allocator = param->m_lifespan ? m_owningNetwork->getPersistentMemoryAllocator() : m_owningNetwork->getTempMemoryAllocator();
            resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
          }

          param->m_attribDataHandle.m_attribData = (AttribData*)resource.ptr;
          param->m_attribDataHandle.m_format = memReqs;

          param->m_attribDataHandle.m_attribData->m_allocator = allocator;
          param->m_attribDataHandle.m_attribData->setType(param->m_attribType);
          param->m_attribDataHandle.m_attribData->setRefCount(0);

          param->m_taskParamFlags.set(TPARAM_FLAG_NEEDS_CREATE);

          NodeBin* nodeBin = m_owningNetwork->getNodeBin(param->m_attribAddress.m_owningNodeID);
          OutputCPPin* outputPin = nodeBin->getOutputCPPin(param->m_attribAddress.m_targetNodeID);

          // Make sure it doesn't already exist.
          NMP_ASSERT(!outputPin->m_attribDataHandle.m_attribData);

          outputPin->m_attribDataHandle = param->m_attribDataHandle;
          outputPin->m_lastUpdateFrame = VALID_FOREVER;
        }
      }

      for (uint32_t paramIdx = 0; paramIdx < task->m_numParams; paramIdx++)
      {
        TaskParameter* param = task->m_params + paramIdx;

        if (param->m_taskParamFlags.areSet(TPARAM_FLAG_DEPENDENT))
        {
          Task* dependentTask = (Task*)param->m_attribDataHandle.m_attribData;

          TaskParameter* dependentParam = dependentTask->getOutputParameter(param->m_attribAddress);

          NMP_ASSERT(dependentParam);
          // Possible circular dependency detected here - set MR_CIRCDEPS_DEBUGGING (mrNetwork.h) to investigate.
          NMP_ASSERT(dependentParam->m_attribDataHandle.m_attribData);
          NMP_ASSERT(param->m_attribType == dependentParam->m_attribType);

          param->m_attribDataHandle = dependentParam->m_attribDataHandle;

          NMP_ASSERT(dependentTask->m_numDependents);
          dependentTask->m_numDependents--;

          if (!dependentTask->m_numDependents)
          {
            for (uint32_t depParamIdx = 0; depParamIdx < dependentTask->m_numParams; depParamIdx++)
            {
              TaskParameter* depParam = dependentTask->m_params + depParamIdx;

              if (depParam->m_taskParamFlags.areSet(TPARAM_FLAG_OUTPUT | TPARAM_FLAG_NEEDS_CREATE) && depParam->m_lifespan == 0)
              {
                NMP_ASSERT(depParam->m_attribDataHandle.m_attribData->m_allocator == m_owningNetwork->getTempMemoryAllocator());
                NMP_ASSERT(depParam->m_attribType < CACHE_SIZE);

                AttribData* lastCached = cache[depParam->m_attribType];

                cache[depParam->m_attribType] = depParam->m_attribDataHandle.m_attribData;

                cache[depParam->m_attribType]->m_allocator = (NMP::MemoryAllocator*)lastCached;
              }
            }
          }
        }
      }

      m_tasks[targetTaskIndex] = task;
      ++targetTaskIndex;
    }
  }

  m_numQueuedTasks = targetTaskIndex;

  m_numUnprocessedTasks = m_numQueuedTasks;

  if (m_numQueuedTasks > m_maxQueuedTasks)
  {
    m_maxQueuedTasks = m_numQueuedTasks;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskQueue::clearTaskFromTasksArray(Task* task)
{
  for (uint32_t i = 0; i < m_numQueuedTasks; ++i)
  {
    if (m_tasks[i] == task)
    {
      m_tasks[i] = NULL;
      return;
    }
  }

  // Trying to clear a task from the tasks array that does not exist.
  NMP_ASSERT_FAIL();
}

//----------------------------------------------------------------------------------------------------------------------
void TaskQueue::debugOutputQueueContents(const char** taskNames)
{
  uint32_t unprocessedCount = 0;

  NET_LOG_MESSAGE(103, "Current queue contents:\n");

  for (uint32_t i = 0; i < m_numQueuedTasks; i++)
  {
    Task* queuedTask = m_tasks[i];
    if (queuedTask)
    {
      debugOutputTaskContents(queuedTask, taskNames);
      ++unprocessedCount;
    }
  }

  NMP_ASSERT(unprocessedCount == m_numUnprocessedTasks);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskQueue::debugOutputTaskContents(Task* task, const char** taskNames)
{
  (void)taskNames;

  NET_LOG_MESSAGE(103, "  Task: %28s, ID: %3d, Owning Node: %4d\n", taskNames[task->m_taskid], task->m_taskid, task->m_owningNodeID);
  NET_LOG_MESSAGE(103, "    Params:\n");

  for (uint32_t paramIdx = 0; paramIdx < task->m_numParams; paramIdx++) \
  {
    TaskParameter* param = &(task->m_params[paramIdx]);

    if (param->m_taskParamFlags.isSet(TPARAM_FLAG_INPUT))
    {
      NET_LOG_MESSAGE(101, "    Input");
      NET_LOG_OUTPUT_ATTRIB_ADDRESS(101, param->m_attribAddress, NULL);
    }
    else
    {
      NET_LOG_MESSAGE(101, "    Output                ");
      NET_LOG_OUTPUT_ATTRIB_ADDRESS(101, param->m_attribAddress, NULL);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool TaskQueue::releaseTasks()
{
  // Check that all tasks that got queued were processed
  if (m_numUnprocessedTasks == 0)
  {
    m_numQueuedTasks = 0;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool TaskQueue::freeTempData()
{
  m_tasks = NULL;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void TaskQueue::expandTaskList()
{
  // We double the size of the task queue if we run out of room here.
  NMP::Memory::Format taskQueueFmt(m_taskQueueSize * 2 * sizeof(Task*), NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource taskQueueResource =
    NMPAllocatorAllocateFromFormat(m_owningNetwork->getTempMemoryAllocator(), taskQueueFmt);

  NMP_ASSERT(taskQueueResource.ptr);  // Check we allocated correctly.

  // Copy the old task queue into the new larger one.  We simply discard the old one as it will be reclaimed when
  //  the fast heap is wiped down.
  NMP::Memory::memcpy(taskQueueResource.ptr, m_tasks, m_taskQueueSize * sizeof(Task*));
  m_tasks = (Task**)taskQueueResource.ptr;

  // Update the task queue size.
  m_taskQueueSize *= 2;

  return;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
