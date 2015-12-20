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
#ifndef MR_DISPATCHER
#define MR_DISPATCHER
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrAttribData.h"
#include "morpheme/mrTaskQueue.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
class Timer;
}

namespace MR
{

class InstanceDebugInterface;

// Forward decl
struct NodeBinEntry;

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::Dispatcher
/// \brief P.I. Dispatcher base class.  This class should be over-ridden to create a useable dispatcher.
/// \ingroup CoreModule
//----------------------------------------------------------------------------------------------------------------------
class Dispatcher
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  /// \struct MR::Dispatcher::TaskParameters
  /// \brief The TaskParameters structure holds all the data that is passed into a task function.  This structure is
  ///  constructed by the dispatcher from the Task class when it is ready to be executed.  It is then passed into the
  ///  task function.
  //--------------------------------------------------------------------------------------------------------------------
  struct TaskParameters
  {
    /// \brief Pointer to an array of task parameters.  This will be referencing the parameters from inside an MR::Task
    TaskParameter* m_parameters;
    /// \brief The number of parameters pointed to by m_parameters.
    uint32_t       m_numParameters;
    /// \brief The dispatcher which scheduled this task for evaluation.
    Dispatcher*    m_dispatcher;

    /// \brief Gets a pointer to the specific AttribData class for the input parameter at a given index.
    ///
    /// This function is designed to be called from within a task function to retrieve an input parameter for use.
    ///  This function will perform error checking that the indexed parameter is an input, and that it is of the
    ///  correct semantic and class.
    ///
    /// \return A pointer to the AttribData.
    template <typename AttribDataClass>
    NM_INLINE AttribDataClass* getInputAttrib(
      uint32_t           index,
      AttribDataSemantic semantic);

    /// \brief Gets a pointer to the specific AttribData class for the input parameter at a given index, if it exists.
    ///
    /// This function is designed to be called from within a task function to retrieve an input parameter for use.
    ///  This function will perform error checking that the indexed parameter is an input, and that it is of the
    ///  correct semantic and class.
    ///
    /// \return A pointer to the AttribData if it exists, or NULL otherwise.
    template <typename AttribDataClass>
    NM_INLINE AttribDataClass* getOptionalInputAttrib(
      uint32_t           index,
      AttribDataSemantic semantic);

    /// \brief Creates an output AttribData class, of the type specified in the template argument. Replaces any existing 
    ///  AttribData on the parameters specified output control parameter pin.
    template <typename AttribDataClass>
    NM_INLINE AttribDataClass* createOutputCPAttrib(
      uint32_t                index,
      AttribDataSemantic      semantic,
      AttribDataCreateDesc*   desc = NULL);

    /// \brief Creates an output AttribData class, of the type specified in the template argument.
    ///
    /// This function is designed to be called from within a task function to create an output parameter.  The output
    ///  parameter will be initialised using it's constructor, and the TaskParameters struct will cache a pointer to it
    ///  so the dispatcher has access to manage it after the task function returns.  It is expected that the task
    ///  function will fill in the AttribData contents.
    ///  This function will perform error checking that the indexed parameter is not an input, and that it is of the
    ///  correct semantic.
    ///
    /// \return A pointer to the created AttribData, or NULL if there is a failure.
    template <typename AttribDataClass>
    NM_INLINE AttribDataClass* createOutputAttrib(
      uint32_t                index,
      AttribDataSemantic      semantic,
      AttribDataCreateDesc*   desc = NULL);

    /// \brief Creates an output AttribData class, of the type specified in the template argument.  This version of the
    ///  function will attempt to find an AttribData of the same address as the specified indexed task parameter, and
    ///  if it is successful, it will overwrite that parameter with a newly created one.
    ///
    /// This function is designed to be called from within a task function to create an output parameter.  The output
    ///  parameter will be initialised using it's constructor, and the TaskParameters struct will cache a pointer to it
    ///  so the dispatcher has access to manage it after the task function returns.  It is expected that the task
    ///  function will fill in the AttribData contents.
    ///  This function will perform error checking that the indexed parameter is not an input, and that it is of the
    ///  correct semantic.
    ///
    /// \return A pointer to the created AttribData, or NULL if there is a failure.
    template <typename AttribDataClass>
    NM_INLINE AttribDataClass* createOutputAttribReplace(
      uint32_t                index,
      AttribDataSemantic      semantic,
      AttribDataCreateDesc*   desc = NULL);

    /// \brief Creates an output AttribDataTransformBuffer, with the length specified in the size parameter.
    ///
    /// This function is designed to be called from within a task function to create an output parameter.  The output
    ///  parameter will be initialised using it's constructor, and the TaskParameters struct will cache a pointer to it
    ///  so the dispatcher has access to manage it after the task function returns.  It is expected that the task
    ///  function will fill in the AttribData contents.
    ///  This function will perform error checking that the indexed parameter is not an input, and that it is of the
    ///  correct semantic.
    ///
    /// \return A pointer to the created AttribData, or NULL if there is a failure.
    NM_INLINE AttribDataTransformBuffer* createOutputAttribTransformBuffer(
      uint32_t           index,
      uint32_t           size,
      AttribDataSemantic semantic);

    /// \brief Creates an output AttribDataTransformBuffer. This version of the
    ///  function will attempt to find an AttribData of the same address as the specified indexed task parameter, and
    ///  if it is successful, it will overwrite that parameter with a newly created one.
    ///
    /// This function is designed to be called from within a task function to create an output parameter.  The output
    ///  parameter will be initialised using it's constructor, and the TaskParameters struct will cache a pointer to it
    ///  so the dispatcher has access to manage it after the task function returns.  It is expected that the task
    ///  function will fill in the AttribData contents.
    ///  This function will perform error checking that the indexed parameter is not an input, and that it is of the
    ///  correct semantic.
    ///
    /// \return A pointer to the created AttribData, or NULL if there is a failure.
    NM_INLINE AttribDataTransformBuffer* createOutputAttribReplaceTransformBuffer(
      uint32_t           index,
      AttribDataSemantic semantic,
      uint32_t           size);
  };

  /// \brief Typedef for a Task
  ///
  /// This function definition describes the prototype for a Task's function.  All task functions to be invoked by a
  ///  Dispatcher should conform to this prototype.
  typedef void (TaskFunction)(TaskParameters* parameters);

public:
  Dispatcher();
  virtual ~Dispatcher();

  /// \brief Release the dispatcher and any resources it may have allocated.
  ///
  /// Note: This function is provided as a pure virtual base to facilitate destruction, but
  ///  implementation is specific to each dispatcher class.
  virtual void releaseAndDestroy() = 0;

  /// \brief Process the provided TaskQueue until execution cannot continue for some reason
  /// \param queue The TaskQueue that is to be processed
  /// \param task A reference to a Task pointer.  If the execution of the queue is blocked on an external task, then
  ///  a pointer to the task should be placed in this parameter.
  /// \return This function should return a member of MR::ExecuteResult to indicate the state of the TaskQueue on
  ///  completion.
  ///
  /// This function is provided as a pure virtual base.  Execution will be specific to an individual Dispatcher
  ///  implementation.  This function will be called during Network::update to process the network's task queue.
  virtual ExecuteResult execute(TaskQueue* queue, Task *&task) = 0;

  /// \brief Search the attrib data in the associated network, returning a handle to the AttribData with the given
  ///  address if one exists.
  ///
  /// This function must be implemented by the dispatcher.
  virtual NodeBinEntry* getAttribData(const MR::AttribAddress& attributeAddress) = 0;

  /// \brief Associates a task with the supplied task ID.
  ///
  /// This function is used to register task functions with the dispatcher.  Tasks are identified by ID when queued
  ///  so a mapping of Task ID to function pointer is required for the dispatcher to operate.
  virtual bool registerTask(
    TaskFunction*   taskFnPt,
    const char*     taskName,
    const uint32_t  taskID) = 0;

  /// \brief Get a pointer to the TaskFunction which corresponds to a particular task
  ///
  /// This function must be implemented by the dispatcher.
  virtual TaskFunction* getTaskFunctionPointer(Task* task) = 0;

  /// \brief Get the text name of a task from it's task ID (if it exists)
  ///
  /// This function must be implemented by the dispatcher.
  virtual const char* getTaskName(TaskID taskID) = 0;

  /// \brief Return the task queue currently associated with this dispatcher.
  NM_INLINE TaskQueue* getTaskQueue() { return m_currentTaskQueue; }
  /// \brief Set the task queue currently associated with this dispatcher.
  NM_INLINE void setTaskQueue(TaskQueue* queue) { m_currentTaskQueue = queue; }

  /// \brief Get the temporary memory allocator associated with this dispatcher.  The temporary memory allocator is
  ///  used for data that will not persist beyond the end of a Network::update.  For example, zero-lifespan attrib data
  ///  and Tasks in the TaskQueue are allocated from the temporary memory allocator.
  NM_INLINE NMP::TempMemoryAllocator* getTempMemoryAllocator() { return m_tempMemoryAllocator; }
  /// \brief Set the temporary allocator that this dispatcher should use.
  NM_INLINE void setTempMemoryAllocator(NMP::TempMemoryAllocator* memoryAllocator) { m_tempMemoryAllocator = memoryAllocator; }

  /// \brief Get the correct memory allocator for a given lifespan.
  ///
  /// \return If the lifespan is zero, the temporary memory allocator is returned, otherwise the persistent memory
  ///  allocator is returned.
  NM_INLINE NMP::MemoryAllocator* getMemoryAllocator(uint32_t lifespan);

  /// \brief Get the persistent memory allocator associated with this dispatcher.  The persistent memory allocator is
  ///  used for data that will survive beyond the end of a Network::update.  For example, attrib data with a lifespan
  ///  greater than zero will be allocated from the persistent allocator.
  NM_INLINE NMP::MemoryAllocator* getPersistentMemoryAllocator() { return m_persistentMemoryAllocator; }
  /// \brief Set the persistent allocator that this dispatcher should use.
  NM_INLINE void setPersistentMemoryAllocator(NMP::MemoryAllocator* memoryAllocator) { m_persistentMemoryAllocator = memoryAllocator; }

  NM_INLINE void setDebugInterface(InstanceDebugInterface* debugInterface) { m_debugInterface = debugInterface; }
  NM_INLINE InstanceDebugInterface* getDebugInterface() { return m_debugInterface; }

  /// \brief Depending on which debug outputs are enabled 
  void beginTaskExecuteDebugHook(const MR::Task* task, NMP::Timer& taskTimer);
  void endTaskExecuteDebugHook(const MR::Task* task, NMP::Timer& taskTimer);

protected:
  /// The task queue currently being consumed. This is only valid during execute call.
  TaskQueue*              m_currentTaskQueue;

  /// For transmission of data to an external debugging application (probably connect).
  InstanceDebugInterface* m_debugInterface;

  /// The temporary memory allocator.
  NMP::TempMemoryAllocator*   m_tempMemoryAllocator;
  /// The persistent memory allocator.
  NMP::MemoryAllocator*   m_persistentMemoryAllocator;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::DispatcherBasic
/// \brief Simple platform independent dispatcher implementation.
///
/// The MR::DispatcherBasic is a reference implementation of a dispatcher.  It uses a single-threaded processing model
///  to simply execute ready tasks in the order they are presented to it.  It is suitable for use when only a single
///  processing element is to be used to update a network.
/// Note that MR::DispatcherBasic cannot be shared between multiple MR::Network instances.  You must create a separate 
///  DispatcherBasic class for each MR::Network.
//----------------------------------------------------------------------------------------------------------------------
class DispatcherBasic : public Dispatcher
{
public:
  /// \brief Create a new instance of MR::DispatcherBasic, using the NMP::Memory allocation routines.
  ///  Internally this function calls getMemoryRequirements() and init().
  static DispatcherBasic* createAndInit();

  /// \brief Destroy the dispatcher, freeing it's memory.
  virtual void releaseAndDestroy() NM_OVERRIDE;

  /// \brief Release any shared resources the dispatcher may have allocated.  term() should be called when the
  ///  morpheme runtime SDK is shut down to prevent any memory leaks
  static void term();

  /// \brief Returns an NMP::Memory::Format structure describing how much memory is required for an instance of
  ///  MR::DispatcherBasic.
  static NMP::Memory::Format getMemoryRequirements();

  /// \brief Initialise an instance of MR::DispatcherBasic using the supplied memory resource.
  static DispatcherBasic* init(NMP::Memory::Resource& resource);

  // \brief Process the supplied TaskQueue.
  virtual ExecuteResult execute(TaskQueue* queue, Task *&task) NM_OVERRIDE;

  /// \brief Associates a task with the supplied task ID.
  ///
  /// This function is used to register task functions with the dispatcher.  Tasks are identified by ID when queued
  ///  so a mapping of Task ID to function pointer is required for the dispatcher to operate.
  /// sourceFileID is unused in this function.
  /// Optionally, a task name can be passed in, which is used to aid in debugging.
  /// \return true if successful, false otherwise.
  virtual bool registerTask(
    TaskFunction*   taskFnPt,
    const char*     taskName,
    const uint32_t  taskID) NM_OVERRIDE;

  /// \brief Get the code item that corresponds to a particular task
  virtual TaskFunction* getTaskFunctionPointer(Task* task) NM_OVERRIDE;

  /// \brief This function is a concrete implementation of Dispatcher::getAttribData.
  virtual NodeBinEntry* getAttribData(const MR::AttribAddress& attributeAddress) NM_OVERRIDE;

  /// \brief This function is a concrete implementation of Dispatcher::getTaskName.
  virtual const char* getTaskName(TaskID taskID) NM_OVERRIDE;

  /// \brief Typedef for storage of a link in the cached list.
  typedef uintptr_t CachedPtrLink;

private:
  DispatcherBasic();

#ifdef NMP_ENABLE_ASSERTS
  /// \brief  Test that the isFull flag on any data buffers (ATTRIB_TYPE_TRANSFORM_BUFFER) output from a task are valid.
  void validateDataBuffers(Task* readyTask);
#endif // NMP_ENABLE_ASSERTS

  /// This is the PC-specific implementation of getting a task pointer from the task ID.
  static uint32_t       sm_maxNumTasks;
  static TaskFunction** sm_taskFnPtrs;
  static const char**   sm_taskNames;
};

//----------------------------------------------------------------------------------------------------------------------
// Utility Functions and Macros to assist when performing a task.
//----------------------------------------------------------------------------------------------------------------------
/// \brief Validate that a given task parameter matches it's desired usage.
///
/// Makes sure a task parameters usage flags match our expectations for its use.
/// Makes sure that the parameters semantic is of the expected type.
/// Makes sure that the attribute data does exist if it as input, or doesn't already exist if it is an output.
/// An assert thrown up from this function usually indicates a mismatch between the task queuing function and the task
///  function itself.
#ifdef NMP_ENABLE_ASSERTS
NM_INLINE void validateTaskParam(
  uint32_t            attribIndex,            // In to the params array.
  AttribDataSemantic  expectedAttribSemantic, //
  TaskParamFlags      expectedUsage,          // Input, output etc.
  TaskParameter*      parameters)             // Parameters array.
{
  NMP_ASSERT(parameters);
  TaskParameter* taskParam = &(parameters[attribIndex]);
  NMP_ASSERT(taskParam);

  AttribData* attrib = taskParam->m_attribDataHandle.m_attribData;
  TaskParamFlags taskParamFlags = taskParam->m_taskParamFlags;

  // Validate.
  NMP_ASSERT(taskParam->m_attribAddress.m_semantic == expectedAttribSemantic);
  NMP_ASSERT(taskParamFlags.areSet(expectedUsage)); // Expected usage flags must also be set in the params actual flags (note actual param flags may have other flags set).
  if (taskParamFlags.isSet(TPARAM_FLAG_INPUT))
  {
    if (!taskParamFlags.isSet(TPARAM_FLAG_OPTIONAL))
    {
      // If an input only and not optional the attribute data must exist on execution context.
      NMP_ASSERT(attrib);
    }
  }
  else
  {
    NMP_ASSERT(taskParamFlags.isSet(TPARAM_FLAG_OUTPUT) && expectedUsage.isSet(TPARAM_FLAG_OUTPUT));  // Attrib must be an output only.
  }
}
#else
  #define validateTaskParam(_attribIndex, _expectedAttribSemantic, _expectedUsage, _parameters);
#endif

//----------------------------------------------------------------------------------------------------------------------
// TaskParameters inline functions
//----------------------------------------------------------------------------------------------------------------------
template <typename AttribDataClass>
NM_INLINE AttribDataClass* Dispatcher::TaskParameters::getInputAttrib(
  uint32_t index,
  AttribDataSemantic NMP_USED_FOR_ASSERTS(semantic))
{
  NMP_ASSERT(index < m_numParameters);

  validateTaskParam(index, semantic, TPARAM_FLAG_INPUT, m_parameters);
  if (m_parameters[index].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    return NULL;
  }
  AttribDataClass* result = (AttribDataClass*)m_parameters[index].m_attribDataHandle.m_attribData;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename AttribDataClass>
NM_INLINE AttribDataClass* Dispatcher::TaskParameters::getOptionalInputAttrib(
  uint32_t index,
  AttribDataSemantic NMP_USED_FOR_ASSERTS(semantic))
{
  if (index >= m_numParameters)
    return 0;

  validateTaskParam(index, semantic, TPARAM_FLAG_INPUT | TPARAM_FLAG_OPTIONAL, m_parameters);
  if (m_parameters[index].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    return NULL;
  }
  AttribDataClass* result = (AttribDataClass*)m_parameters[index].m_attribDataHandle.m_attribData;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename AttribDataClass>
NM_INLINE AttribDataClass* Dispatcher::TaskParameters::createOutputCPAttrib(
  uint32_t           index,
  AttribDataSemantic NMP_USED_FOR_ASSERTS(semantic),
  AttribDataCreateDesc* desc)
{
  NMP_ASSERT(index < m_numParameters);
  
  // Validate parameter details.
  NMP_ASSERT(m_parameters[index].m_attribAddress.m_semantic == semantic);
  NMP_ASSERT(m_parameters[index].m_taskParamFlags.isSet(TPARAM_FLAG_EMITTED_CP));
  NMP_ASSERT(m_parameters[index].m_attribDataHandle.m_attribData)

  if (m_parameters[index].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    // The attribute doesn't already exist so create it.
    NMP::Memory::Resource resource = {m_parameters[index].m_attribDataHandle.m_attribData, m_parameters[index].m_attribDataHandle.m_format};
    m_parameters[index].m_attribDataHandle = AttribDataClass::create(resource, desc);
    m_parameters[index].m_taskParamFlags.clear(TPARAM_FLAG_NEEDS_CREATE);
  }

  return (AttribDataClass*)m_parameters[index].m_attribDataHandle.m_attribData;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename AttribDataClass>
NM_INLINE AttribDataClass* Dispatcher::TaskParameters::createOutputAttrib(
  uint32_t index,
  AttribDataSemantic NMP_USED_FOR_ASSERTS(semantic),
  AttribDataCreateDesc* desc)
{
  NMP_ASSERT(index < m_numParameters);
  validateTaskParam(index, semantic, TPARAM_FLAG_OUTPUT, m_parameters);
  NMP_ASSERT(m_parameters[index].m_attribDataHandle.m_attribData);
  NMP_ASSERT(m_parameters[index].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE));
  NMP::Memory::Resource resource = {m_parameters[index].m_attribDataHandle.m_attribData, m_parameters[index].m_attribDataHandle.m_format};
  uint16_t refCount = m_parameters[index].m_attribDataHandle.m_attribData->getRefCount();
  AttribDataHandle result = AttribDataClass::create(resource, desc);
  NMP_ASSERT(m_parameters[index].m_attribType == result.m_attribData->getType());
  m_parameters[index].m_attribDataHandle.m_attribData->setRefCount(refCount);
  m_parameters[index].m_taskParamFlags.clear(TPARAM_FLAG_NEEDS_CREATE);

  m_parameters[index].m_attribDataHandle = result;
  return (AttribDataClass*)result.m_attribData;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename AttribDataClass>
NM_INLINE AttribDataClass* Dispatcher::TaskParameters::createOutputAttribReplace(
  uint32_t index,
  AttribDataSemantic semantic,
  AttribDataCreateDesc* desc)
{
  if (m_parameters[index].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    return createOutputAttrib<AttribDataClass>(index, semantic, desc);
  }

  return (AttribDataClass*)m_parameters[index].m_attribDataHandle.m_attribData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribDataTransformBuffer* Dispatcher::TaskParameters::createOutputAttribTransformBuffer(
  uint32_t index,
  uint32_t size,
  AttribDataSemantic semantic)
{
  AttribDataTransformBufferCreateDesc desc;
  desc.m_size = size;
  return createOutputAttrib<AttribDataTransformBuffer>(index, semantic, &desc);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribDataTransformBuffer* Dispatcher::TaskParameters::createOutputAttribReplaceTransformBuffer(
  uint32_t index,
  AttribDataSemantic semantic,
  uint32_t size)
{
  if (m_parameters[index].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    return createOutputAttribTransformBuffer(index, size, semantic);
  }

  return (AttribDataTransformBuffer*)m_parameters[index].m_attribDataHandle.m_attribData;
}

//----------------------------------------------------------------------------------------------------------------------
// Dispatcher inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::MemoryAllocator* Dispatcher::getMemoryAllocator(uint32_t lifespan)
{
  NMP::MemoryAllocator* result = lifespan ? getPersistentMemoryAllocator() : getTempMemoryAllocator();

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_DISPATCHER
//----------------------------------------------------------------------------------------------------------------------
