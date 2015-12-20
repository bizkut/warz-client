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
#include "morpheme/mrDispatcher.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrCommonTasks.h"
#include "morpheme/mrNetworkLogger.h"
#include "morpheme/mrInstanceDebugInterface.h"
#include "NMPlatform/NMProfiler.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

/// Enables the maintenance of an accurate list of tasks that have not yet been processed on the tasks list.
#define TASK_QUEUE_DEBUGGINGx

//----------------------------------------------------------------------------------------------------------------------
// P.I. Dispatcher implementation
//----------------------------------------------------------------------------------------------------------------------
Dispatcher::Dispatcher() : m_currentTaskQueue(0), m_debugInterface(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
Dispatcher::~Dispatcher()
{
}

//----------------------------------------------------------------------------------------------------------------------
ExecuteResult Dispatcher::execute(TaskQueue* NMP_UNUSED(queue), Task*& NMP_UNUSED(task))
{
  // This function should only exist in the platform specific versions of the dispatcher.
  NMP_ASSERT_FAIL();
  return EXECUTE_RESULT_COMPLETE;
}

//----------------------------------------------------------------------------------------------------------------------
void Dispatcher::beginTaskExecuteDebugHook(const MR::Task* task, NMP::Timer& taskTimer)
{
#if defined(MR_OUTPUT_DEBUGGING)
  if (m_debugInterface)
  {
    // Start task execution timing.
    if (m_debugInterface->debugOutputsAreOn(DEBUG_OUTPUT_PROFILING))
    {
      taskTimer.start();
    }
    if (m_debugInterface->debugOutputsAreOn(DEBUG_OUTPUT_DEBUG_DRAW))
    {
      m_debugInterface->setCurrentNodeID(task->m_owningNodeID);
    }
  }
#else
  (void)task;
  (void)taskTimer;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void Dispatcher::endTaskExecuteDebugHook(const MR::Task* task, NMP::Timer& taskTimer)
{
#if defined(MR_OUTPUT_DEBUGGING)
  if (m_debugInterface)
  {
    // Finish and record task execution timing.
    if (m_debugInterface && m_debugInterface->debugOutputsAreOn(DEBUG_OUTPUT_PROFILING))
    {
      float taskTime = taskTimer.stop();
      m_debugInterface->addNodeTiming(task->m_owningNodeID, MR::NTTaskExecution, taskTime);
    }
    if (m_debugInterface->debugOutputsAreOn(DEBUG_OUTPUT_DEBUG_DRAW))
    {
      m_debugInterface->setCurrentNodeID(MR::INVALID_NODE_ID);
    }
  }
#else
  (void)task;
  (void)taskTimer;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
// PC Dispatcher implementation
//----------------------------------------------------------------------------------------------------------------------
uint32_t                   DispatcherBasic::sm_maxNumTasks = 0;
Dispatcher::TaskFunction** DispatcherBasic::sm_taskFnPtrs = NULL;
const char**               DispatcherBasic::sm_taskNames = NULL;

//----------------------------------------------------------------------------------------------------------------------
DispatcherBasic::DispatcherBasic()
{
}

//----------------------------------------------------------------------------------------------------------------------
DispatcherBasic* DispatcherBasic::createAndInit()
{
  NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(getMemoryRequirements());
  NMP_ASSERT(resource.ptr);
  return init(resource);
}

//----------------------------------------------------------------------------------------------------------------------
void DispatcherBasic::releaseAndDestroy()
{
  NMP::Memory::memFree(this);
}

//----------------------------------------------------------------------------------------------------------------------
void DispatcherBasic::term()
{
  if(sm_taskFnPtrs)
  {
    NMP::Memory::memFree(sm_taskFnPtrs);
    sm_taskFnPtrs = NULL;
  }

  if(sm_taskNames)
  {
    NMP::Memory::memFree(sm_taskNames);
    sm_taskNames = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format DispatcherBasic::getMemoryRequirements()
{
  return NMP::Memory::Format(sizeof(DispatcherBasic), NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
DispatcherBasic* DispatcherBasic::init(NMP::Memory::Resource& resource)
{
  DispatcherBasic* result = (DispatcherBasic*)resource.ptr;
  resource.increment(getMemoryRequirements());

  // Call the constructor in-place to setup the vtable.
  new(result) DispatcherBasic();

  if(!sm_taskFnPtrs)
  {
    // We seed the task function pointer tables with a block that should be big enough to cover all the 
    //  morpheme core tasks
    sm_maxNumTasks = 256;
    sm_taskFnPtrs = (TaskFunction**)NMPMemoryAlloc(sm_maxNumTasks * sizeof(TaskFunction*));
    memset(sm_taskFnPtrs, 0, sm_maxNumTasks * sizeof(TaskFunction*));
    sm_taskNames = (const char**)NMPMemoryAlloc(sm_maxNumTasks * sizeof(const char*));
    memset(sm_taskNames, 0, sm_maxNumTasks * sizeof(const char*));
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool DispatcherBasic::registerTask(
  TaskFunction*   taskFnPtr,
  const char*     taskName,
  const uint32_t  taskID)
{
  if(taskID >= sm_maxNumTasks)
  {
    // We need to expand the task fn arrays to deal with this new, larger task ID.

    TaskFunction** tempTaskFnPtrs = (TaskFunction**)NMPMemoryAlloc((taskID + 1) * sizeof(TaskFunction*)); 
    memset(tempTaskFnPtrs, 0, (taskID + 1) * sizeof(TaskFunction*));
    const char** tempTaskNames = (const char**)NMPMemoryAlloc((taskID + 1) * sizeof(const char*));
    memset(tempTaskNames, 0, (taskID + 1) * sizeof(const char*));

    NMP::Memory::memcpy(tempTaskFnPtrs, sm_taskFnPtrs, sm_maxNumTasks * sizeof(TaskFunction*));
    NMP::Memory::memcpy(tempTaskNames, sm_taskNames, sm_maxNumTasks * sizeof(const char*));

    NMP::Memory::memFree(sm_taskFnPtrs);
    NMP::Memory::memFree(sm_taskNames);

    sm_taskFnPtrs = tempTaskFnPtrs;
    sm_taskNames = tempTaskNames;

    sm_maxNumTasks = taskID + 1;
  }

  // If a task is already registered against this ID make sure its the same task.
  NMP_ASSERT_MSG(sm_taskFnPtrs[taskID] == NULL ? true : taskFnPtr == sm_taskFnPtrs[taskID], "Trying to register a new task function in a slot that is already used.")

  sm_taskFnPtrs[taskID] = taskFnPtr;
  sm_taskNames[taskID] = taskName;

  NET_LOG_MESSAGE(109, "   Task ID: %4d registered to: %s\n", taskID, taskName);

#if defined(NM_USE_SNTUNER)
  // Output list of taskID to name mappings for use with SNTuner Mappings.ini
  printf("0x%x=%s\n", taskID + 1, taskName);
#endif

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const char* DispatcherBasic::getTaskName(TaskID taskID)
{
  NMP_ASSERT(taskID < sm_maxNumTasks);

  return sm_taskNames[taskID];
}

//----------------------------------------------------------------------------------------------------------------------
NodeBinEntry* DispatcherBasic::getAttribData(const AttribAddress& attributeAddress)
{
  // See if it's in the network.
  NMP_ASSERT(m_currentTaskQueue && m_currentTaskQueue->m_owningNetwork);
  return m_currentTaskQueue->m_owningNetwork->getAttribDataNodeBinEntry(attributeAddress);
}

//----------------------------------------------------------------------------------------------------------------------
#ifdef NMP_ENABLE_ASSERTS
void DispatcherBasic::validateDataBuffers(Task* readyTask)
{
  for (uint32_t i = 0; i < readyTask->m_numParams; i++)
  {
    TaskParameter* param = &readyTask->m_params[i];

    // We are only interested in output parameters.
    if (param->m_taskParamFlags.isSet(TPARAM_FLAG_OUTPUT))
    {
      // We have an output buffer, see if it's transform buffer
      if (param->m_attribDataHandle.m_attribData &&
          param->m_attribDataHandle.m_attribData->getType() == ATTRIB_TYPE_TRANSFORM_BUFFER)
      {
        // We have a transform buffer that was created by the previous task, make sure it's valid
        NMP::DataBuffer* buffer = ((AttribDataTransformBuffer*)param->m_attribDataHandle.m_attribData)->m_transformBuffer;
        NMP_ASSERT_MSG(
          buffer->isFull() == buffer->getUsedFlags()->calculateAreAllSet(),
          "Invalid output Databuffer created in task: %28s, ID: %3d, Owning Node: %4d\n",
          sm_taskNames[readyTask->m_taskid],
          readyTask->m_taskid,
          readyTask->m_owningNodeID);
      }
    }
  }
}
#endif // NMP_ENABLE_ASSERTS

//----------------------------------------------------------------------------------------------------------------------
ExecuteResult DispatcherBasic::execute(TaskQueue* queue, Task*& task)
{
  m_currentTaskQueue = queue;

#ifdef MR_OUTPUT_DEBUGGING
  NMP::Timer executeQueueTimer(true);
#endif // COMMS_DEBUGGING_TEST

  while (queue->m_numUnprocessedTasks)
  {
    // look in the task list in order and find something to execute
    Task* readyTask = queue->getTask(queue->m_numQueuedTasks - queue->m_numUnprocessedTasks);

    if (readyTask)
    {
      // Task is ready to execute so let's find the corresponding function to call.

      NET_LOG_MESSAGE(107, "------------\n%3i. DispatcherPC::execute has a ready task ID: %i, Name: %s, owner %i %s\n",
                      queue->m_numQueuedTasks - queue->m_numUnprocessedTasks,
                      readyTask->m_taskid,
                      sm_taskNames[readyTask->m_taskid],
                      readyTask->m_owningNodeID,
                      m_currentTaskQueue->m_owningNetwork->getNetworkDef()->getNodeNameFromNodeID(readyTask->m_owningNodeID));
      NET_LOG_OUTPUT_TASK_PARAMS(108, readyTask);

#if defined(TASK_QUEUE_DEBUGGING)
      queue->clearTaskFromTasksArray(readyTask);
#endif

      // If it's an external task, we must break execution
      if (readyTask->m_external)
      {
        task = readyTask;
        return EXECUTE_RESULT_BLOCKED;
      }
      else
      {
#ifdef MR_OUTPUT_DEBUGGING
        NMP::Timer taskTimer(true);
        beginTaskExecuteDebugHook(readyTask, taskTimer);
#endif // MR_OUTPUT_DEBUGGING

        // Get a pointer to the function we're going to use
        TaskFunction* taskFn = getTaskFunctionPointer(readyTask);
        NMP_ASSERT(taskFn);

        // Go ahead and call it with the task's parameters
        Dispatcher::TaskParameters taskParameters = { &(readyTask->m_params[0]), readyTask->m_numParams, this };
        taskFn(&taskParameters);
        m_currentTaskQueue->m_numUnprocessedTasks--;

#ifdef NMP_ENABLE_ASSERTS
        validateDataBuffers(readyTask);
#endif // NMP_ENABLE_ASSERTS

#ifdef MR_OUTPUT_DEBUGGING
        endTaskExecuteDebugHook(readyTask, taskTimer);
#endif // MR_OUTPUT_DEBUGGING
      }
    }
    else
    {
      NET_LOG_ERROR_MESSAGE("------------\nDispatcher was unable to find a task to process whose dependents have all been evaluated\n");
      queue->debugOutputQueueContents(sm_taskNames);
      NMP_ASSERT_FAIL();  // We are in an invalid situation, there's something blocking that's not external.
    }
  }

  // We're done with the task queue now so we can destroy any tasks that were queued in it.
#if defined(TASK_QUEUE_DEBUGGING)
  if (queue->m_numUnprocessedTasks > 0)
  {
    NET_LOG_ERROR_MESSAGE("Queue not empty after dispatcher execute:\n");
    queue->debugOutputQueueContents(sm_taskNames);
  }
#endif // TASK_QUEUE_DEBUGGING

  if (queue->releaseTasks())
  {
    return EXECUTE_RESULT_COMPLETE;
  }
  else
  {
    return EXECUTE_RESULT_IN_PROGRESS;
  }
}

//----------------------------------------------------------------------------------------------------------------------
Dispatcher::TaskFunction* DispatcherBasic::getTaskFunctionPointer(Task* task)
{
  return sm_taskFnPtrs[task->m_taskid];
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
