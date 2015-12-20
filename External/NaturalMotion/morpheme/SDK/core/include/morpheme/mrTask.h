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
#ifndef MR_TASK
#define MR_TASK
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemoryAllocator.h"
#include "NMPlatform/NMFlags.h"

#include "morpheme/mrAttribAddress.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup Tasks.
/// \ingroup NetworkModule
///
/// A Task instance is a self contained Network Attribute Data processing unit.
/// It forms the basic component of data processing within a Network.
/// During network processing a task dependency list is created. This is followed by execution of the tasks in
/// dependency order.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \enum  MR::TaskParamFlag
/// \brief Task processing units have a set of associated parameters.
/// \ingroup Tasks
///
/// Note that only certain flag combinations are valid:
/// TPARAM_FLAG_OUTPUT
/// TPARAM_FLAG_INPUT
/// TPARAM_FLAG_INPUT | TPARAM_FLAG_OPTIONAL
enum TaskParamFlag
{
  TPARAM_FLAG_NONE              = 0,
  TPARAM_FLAG_OUTPUT            = 1 << 0, ///< Task parameter is an output.
                                          ///<  The task is contracted to create this attribute during its execution.
  TPARAM_FLAG_INPUT             = 1 << 1, ///< Task parameter is an input.
                                          ///<  This input must exist before the task can execute.
  TPARAM_FLAG_OPTIONAL          = 1 << 2, ///< On task execution the existence of this attribute is optional.
  TPARAM_FLAG_EMITTED_CP        = 1 << 3, ///< This is an emitted control parameter. CPs coming from tasks must be emitted
                                          ///<  because their result is frame delayed (unlike output CPs)
  TPARAM_FLAG_UNINITIALISED     = 1 << 4, ///< This task parameter has not been initialised yet.
                                          ///<  Primarily for use when debugging.
  TPARAM_FLAG_DEPENDENT         = 1 << 5, ///< This parameter depends on other.
  TPARAM_FLAG_NEEDS_CREATE      = 1 << 6  ///< The parameter's memory has been allocated, but not initialized.
};

//----------------------------------------------------------------------------------------------------------------------
/// \enum  MR::ExecuteResult
/// \brief
/// \ingroup CoreModule
typedef enum
{
  EXECUTE_RESULT_COMPLETE = 0,
  EXECUTE_RESULT_IN_PROGRESS,
  EXECUTE_RESULT_STARTED,
  EXECUTE_RESULT_BLOCKED,
  EXECUTE_RESULT_INVALID
} ExecuteResult;

//----------------------------------------------------------------------------------------------------------------------
/// \typedef TaskParamFlags
/// \brief Task parameter flags.
/// \ingroup Tasks
/// \see MR::TaskParamFlag.
typedef NMP::Flags TaskParamFlags;

//----------------------------------------------------------------------------------------------------------------------
/// \struct MR::TaskParameter
/// \brief A Task instance has 1 or more parameters
/// \ingroup Tasks
//----------------------------------------------------------------------------------------------------------------------
struct TaskParameter
{
  void init(
    const TaskParamFlags  attribUsage,
    const AttribAddress&  attribAddress,
    AttribDataType        attribType,
    uint16_t              lifespan = 0)
  {
#ifdef NM_DEBUG
    checkUninitialised();
#endif

    m_attribAddress = attribAddress;
    m_taskParamFlags.clearThenSet(attribUsage);
    m_lifespan = lifespan;
    m_attribDataHandle.m_attribData = NULL;
    m_attribDataHandle.m_format.set((size_t)0, 0u);
    m_attribType = attribType;
  }

  void init(
    const TaskParamFlags attribUsage,
    AttribDataSemantic   semantic,
    AttribDataType       attribType,
    NodeID               owningNodeID,
    NodeID               targetNodeID = INVALID_NODE_ID,
    FrameCount           validFrame = VALID_FOREVER,
    AnimSetIndex         animSetIndex = ANIMATION_SET_ANY,
    uint16_t             lifespan = 0)
  {
#ifdef NM_DEBUG
    checkUninitialised();
#endif

    m_attribAddress.init(semantic, owningNodeID, targetNodeID, validFrame, animSetIndex);
    m_taskParamFlags.clearThenSet(attribUsage);
    m_lifespan = lifespan;
    m_attribDataHandle.m_attribData = NULL;
    m_attribDataHandle.m_format.set((size_t)0, 0u);
    m_attribType = attribType;
  }

#ifdef NM_DEBUG
  /// \brief Check the parameter is not yet initialised.
  void checkUninitialised() const;
#endif

  AttribAddress    m_attribAddress;    ///< Address of this attrib data.
  TaskParamFlags   m_taskParamFlags;   ///< Indicating input, output, etc.
  uint16_t         m_lifespan;         ///< If this is an output parameter what lifespan should it be created with.
  AttribDataType   m_attribType;       ///< If this is an output parameter what type it is.
  AttribDataHandle m_attribDataHandle; ///< A handle to the actual AttribData.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::Task
/// \brief A queued task structure contains all information required to prepare for and to execute a registered
///  Task function.
/// \ingroup Tasks
///
/// Note that Tasks need to be name spaced sensibly and need to be registered with the dispatcher.
/// Tasks must not be node-specific in any way
//----------------------------------------------------------------------------------------------------------------------
class Task
{
public:
  /// \return The memory requirements of a Task with the specified numParamaters.
  static NMP::Memory::Format getMemoryRequirements(uint32_t numParameters);

  /// \brief Initialise a Task within the provided memory resource.
  NM_INLINE static Task* init(
    NMP::Memory::Resource resource,
    uint32_t              numParameters,
    TaskID                taskID,
    NodeID                owningNodeID,
    bool                  externalTask,
    bool                  supportsRemoteEval,
    bool                  managesOwnDMA,
    NMP::MemoryAllocator* allocator,
    bool                  referenceTask);

  /// \brief
  NM_INLINE void addDependentTask(TaskParameter* param);

  /// \brief
  NM_INLINE TaskParameter* getParameter(const AttribAddress& attribAddress);

  /// \brief
  NM_INLINE TaskParameter* getOutputParameter(const AttribAddress& attribAddress);

  /// \brief Set all params on this task as not having been initialised.
  NM_INLINE void initialiseTaskParams();

  /// \brief Validate that all parameters for this task have been initialised.
  NM_INLINE void validateTaskParams(const char* taskName, const char* owningNodeName);

#ifdef NMP_ENABLE_ASSERTS
  /// \brief Checks to see if an input/output parameter of the same semantic be combined into a single inputOutput parameter.
  NM_INLINE void checkForTaskParamMerges(const char* taskName);
#endif//NMP_ENABLE_ASSERTS

  NMP::MemoryAllocator* m_allocator;                    ///< Which Memory allocator was this created by. When it comes
                                                        ///<  to deleting the task we know which allocator to use.
  Task*                 m_next;                         ///< Next task in the queue for a node (look at NodeBin::addQueuedTask/findQueuedTaskID)
  uint32_t              m_numParams;                    ///< The number of parameters on this task, including: inputs,
                                                        ///<  outputs and dependents.
  uint32_t              m_numDependents;                ///< The number of dependents that have currently been added to
                                                        ///<  this Task.
  TaskID                m_taskid;                       ///< The unique id of this task
  bool                  m_external;                     ///< A task that must be performed external to network update.
                                                        ///<  For example:
                                                        ///<   1. Global physics simulation.
                                                        ///<   2. Character controller update.
                                                        ///<   3. A.I. pre-step 
                                                        ///<  etc.
  bool                  m_taskSupportsRemoteEvaluation; ///< True if the task can be transferred to another computation 
                                                        ///< unit for execution (e.g. SPU)
  bool                  m_taskManagesOwnDMA;            ///< True if the task DMAs its own parameters
  bool                  m_referenceTask;
  NodeID                m_owningNodeID;                 ///< Which node has queued the execution of this task.
  TaskParameter             m_params[1];                ///< Array of parameters for this task - note that there may be more than 1.
                                                        ///<  Do not add anything after this member as it will get trampled.
};

//----------------------------------------------------------------------------------------------------------------------
// Task functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Task* Task::init(
  NMP::Memory::Resource resource,
  uint32_t              numParameters,
  TaskID                taskID,
  NodeID                owningNodeID,
  bool                  externalTask,
  bool                  supportsRemoteEval,
  bool                  managesOwnDMA,
  NMP::MemoryAllocator* allocator,
  bool                  referenceTask)
{
  // We must have an allocator supplied.
  NMP_ASSERT(allocator);

  // Make sure the memory is there
  NMP_ASSERT(resource.ptr);

  Task* result = (Task*)resource.ptr;

  result->m_numParams = numParameters;
  result->m_numDependents = 0;

  result->m_taskid = taskID;
  result->m_owningNodeID = owningNodeID;
  result->m_external = externalTask;
  result->m_taskSupportsRemoteEvaluation = supportsRemoteEval;
  result->m_taskManagesOwnDMA = managesOwnDMA;
  result->m_allocator = allocator;
  result->m_next = NULL;
  result->m_referenceTask = referenceTask;

  result->initialiseTaskParams();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_DEBUG

NM_INLINE void Task::initialiseTaskParams()
{
  // Mark all the params as uninitialised so that we can validate that they have all been initialised before
  //  processing later.
  for (uint32_t i = 0; i < m_numParams; ++i)
    m_params[i].m_taskParamFlags.clearThenSet(TPARAM_FLAG_UNINITIALISED);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TaskParameter::checkUninitialised() const
{
  // If this flag is not set, we have already initialised this parameter, this will overwrite the existing parameter.
  //  Note the check is only valid if initialiseTaskParams correctly initialises the task flags.
  if (!m_taskParamFlags.isSet(TPARAM_FLAG_UNINITIALISED))
  {
    NET_LOG_ERROR_MESSAGE(
      "TaskParameter initialised twice for node %d, semantic %d. Previous data was overwritten.\n",
      m_attribAddress.m_owningNodeID,
      m_attribAddress.m_semantic);
  }
}

#else // NM_DEBUG
NM_INLINE void Task::initialiseTaskParams() {}
#endif // NM_DEBUG

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Task::validateTaskParams(
  const char* NMP_USED_FOR_ASSERTS(taskName),
  const char* NMP_USED_FOR_ASSERTS(owningNodeName))
{
  NMP_ASSERT(taskName && owningNodeName); // If this assert fails it may be that the TaskID simply doesn't exist.
  for (uint32_t i = 0; i < m_numParams; ++i)
  {
    NMP_ASSERT_MSG(
      !m_params[i].m_taskParamFlags.isSet(TPARAM_FLAG_UNINITIALISED),
      "Task param %d, for task ID: %i, Name: %s, owner %i %s is uninitialised",
      i,
      m_taskid,
      taskName,
      m_owningNodeID,
      owningNodeName);
  }
}

#ifdef NMP_ENABLE_ASSERTS
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void canBeCombinedIntoInOut(
  const TaskParameter& inputParam, 
  const TaskParameter& outputParam,
  const char* taskName,
  const uint32_t inputParamIndex,
  const uint32_t outputParamIndex)
{
  // Only params that are not already input/output params can be considered for combination
  if (inputParam.m_taskParamFlags.isSet(TPARAM_FLAG_INPUT) && outputParam.m_taskParamFlags.isSet(TPARAM_FLAG_OUTPUT) &&
    !inputParam.m_taskParamFlags.areSet(TPARAM_FLAG_INPUT | TPARAM_FLAG_OUTPUT) &&
    !outputParam.m_taskParamFlags.areSet(TPARAM_FLAG_INPUT | TPARAM_FLAG_OUTPUT) &&
    !inputParam.m_taskParamFlags.isSet(TPARAM_FLAG_UNINITIALISED) && 
    !outputParam.m_taskParamFlags.isSet(TPARAM_FLAG_UNINITIALISED))
  {
    // If this assert has fired two task params have been created which could be combined into a single inOut param.
    //
    // Use MR::Network::TaskAddNetInputAndOutputParam or MR::Network::TaskAddOptionalNetInputAndOutputParam
    NMP_ASSERT_MSG(!(inputParam.m_attribAddress.m_semantic == outputParam.m_attribAddress.m_semantic &&
      inputParam.m_taskParamFlags.isSet(TPARAM_FLAG_INPUT) && 
      outputParam.m_taskParamFlags.isSet(TPARAM_FLAG_OUTPUT) &&
      inputParam.m_lifespan == outputParam.m_lifespan &&
      inputParam.m_attribAddress.m_owningNodeID == outputParam.m_attribAddress.m_owningNodeID &&
      inputParam.m_attribAddress.m_targetNodeID == outputParam.m_attribAddress.m_targetNodeID &&
      inputParam.m_attribAddress.m_animSetIndex == outputParam.m_attribAddress.m_animSetIndex),
      "Task params %d and %d for semantic %s in task %s could be merged into a single input/output param.",
      inputParamIndex,
      outputParamIndex,
      AttribAddress::getAttribSemanticName(inputParam.m_attribAddress.m_semantic),
      taskName);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Task::checkForTaskParamMerges(const char* taskName)
{
  NMP_ASSERT(taskName); // If this assert fails it may be that the TaskID simply doesn't exist.
  for (uint32_t i = 0; i < m_numParams; ++i)
  {
    for (uint32_t j = i + 1; j < m_numParams; ++j)
    {
      canBeCombinedIntoInOut(m_params[i], m_params[j], taskName, i, j);
      canBeCombinedIntoInOut(m_params[j], m_params[i], taskName, j, i);
    }
  }
}
#endif //NMP_ENABLE_ASSERTS

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Task::addDependentTask(TaskParameter* param)
{
  param->m_attribDataHandle.m_attribData = (AttribData*)this;
  param->m_taskParamFlags.set(TPARAM_FLAG_DEPENDENT);
  m_numDependents++;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE TaskParameter* Task::getParameter(const AttribAddress& attribAddress)
{
  for (uint32_t i = 0; i < m_numParams; i++)
  {
    TaskParameter* param = &m_params[i];

    if ((param->m_attribAddress.m_owningNodeID == attribAddress.m_owningNodeID) &&
        (param->m_attribAddress.m_semantic == attribAddress.m_semantic) &&
        ((param->m_attribAddress.m_targetNodeID == attribAddress.m_targetNodeID) ||
         (attribAddress.m_targetNodeID == INVALID_NODE_ID) ||
         (param->m_attribAddress.m_targetNodeID == INVALID_NODE_ID)) &&  // If the task is depending on a certain target but the supplied attrib address is target agnostic that's fine.
        ((param->m_attribAddress.m_validFrame == attribAddress.m_validFrame) ||
         (param->m_attribAddress.m_validFrame == VALID_FOREVER) ||
         (attribAddress.m_validFrame == VALID_FOREVER) ||
         (attribAddress.m_validFrame == VALID_FRAME_ANY_FRAME)) && // If the task is depending on a certain validity frame but the supplied attribdata is valid forever that's cool.
        ((param->m_attribAddress.m_animSetIndex == attribAddress.m_animSetIndex) ||
         (attribAddress.m_animSetIndex == ANIMATION_SET_ANY) ||
         (param->m_attribAddress.m_animSetIndex == ANIMATION_SET_ANY)))
    {
      return param;
    }
  }

  return NULL;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE TaskParameter* Task::getOutputParameter(const AttribAddress& attribAddress)
{
  for (uint32_t i = 0; i < m_numParams; i++)
  {
    TaskParameter* param = &m_params[i];

    // We are only interested in output parameters.
    if (!param->m_taskParamFlags.isSet(TPARAM_FLAG_OUTPUT))
    {
      continue;
    }

    // If the task is depending on a certain target but the supplied attrib address is target agnostic that's fine.
    if ((param->m_attribAddress.m_semantic == attribAddress.m_semantic) &&
        ((param->m_attribAddress.m_targetNodeID == attribAddress.m_targetNodeID) ||
         (attribAddress.m_targetNodeID == INVALID_NODE_ID) || param->m_attribAddress.m_targetNodeID == INVALID_NODE_ID))
    {
      return param;
    }
  }

  return NULL;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format Task::getMemoryRequirements(uint32_t numParameters)
{
  NMP::Memory::Format result(sizeof(Task), NMP_VECTOR_ALIGNMENT);

  NMP_ASSERT(numParameters); // Tasks need at least one input or output
  result.size += sizeof(TaskParameter) * (numParameters - 1);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
/// Outputting an attribute address to the network log.
#if defined(MR_NETWORK_LOGGING)

extern void netLogOutputTaskParams(uint32_t priority, Task* task);
extern void netLogOutputTaskParam(uint32_t priority, Task* task, uint32_t index);
  #define NET_LOG_OUTPUT_TASK_PARAM(_priority, _task, _index) netLogOutputTaskParam(_priority, _task, _index);
  #define NET_LOG_OUTPUT_TASK_PARAMS(_priority, _task) netLogOutputTaskParams(_priority, _task);

#else // defined(MR_NETWORK_LOGGING)

  #define NET_LOG_OUTPUT_TASK_PARAM(priority, task, index) {}
  #define NET_LOG_OUTPUT_TASK_PARAMS(_priority, _task) {}

#endif // defined(MR_NETWORK_LOGGING)

//----------------------------------------------------------------------------------------------------------------------
/// \struct MR::DataRef
/// \brief Data references are used by tasks to be able to access 1 or more pieces of data that may not
/// currently be in the same memory space as the task.
/// \ingroup AttribData
///
/// e.g. 1. Sampling an animation on SPU. In most cases we cannot send the entire anim to the SPU for sampling so instead
/// we send an array of anim section references. The task on SPU then calculates which section it needs to sample and
/// queues the correct AttribRef section for DMA.
//----------------------------------------------------------------------------------------------------------------------
struct DataRef
{
  void* getData(NMP::Memory::Format sectionMemFormat, NMP::MemoryAllocator* allocator);

  void* m_data; ///< Direct pointer to attrib data on PC, Xbox etc.
                ///<  On SPU: a PPU pointer - reference to memory to DMA on to the SPU.
};

}
//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TASK
//----------------------------------------------------------------------------------------------------------------------
