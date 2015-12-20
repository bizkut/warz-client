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
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "morpheme/Nodes/mrNodes.h"
#include "morpheme/mrManager.h"
#include "morpheme/AnimSource/mrAnimSourceASA.h"
#include "morpheme/AnimSource/mrAnimSourceMBA.h"
#include "morpheme/AnimSource/mrAnimSourceNSA.h"
#include "morpheme/AnimSource/mrAnimSourceQSA.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/Nodes/mrNodeAnimSyncEvents.h"
// The profiling header is always included - it doesn't
//  have any overhead if profiling is disabled
#include "NMPlatform/NMProfiler.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

Manager* Manager::sm_instance = 0;

//----------------------------------------------------------------------------------------------------------------------
void Manager::initMorphemeLib()
{
#if !defined(NMP_VPU_EMULATION) && defined(NMP_PLATFORM_SIMD)
  if (!NMP::vpu::checkForHardwareSupport())
  {
    NMP_ASSERT_FAIL_MSG(
          "Failed to initialise morpheme! VPU library used on platform without support for SIMD instructions. "
          "Compile with NMP_VPU_FORCE_EMULATION to avoid this error.");
    return;
  }
#endif

  NMP_ASSERT(NMP::Memory::isInitialised() == true);

  // instantiate the Manager and call its constructor
  sm_instance = (Manager*)NMPMemoryAlloc(sizeof(Manager));
  NMP_ASSERT(sm_instance);
  new(sm_instance) Manager();

  // Register all the default node types, queuing functions, tasks, conditions, formats,
  //  prediction model types and attrib data types.
  // Note that in all cases currently this consists of filling out pre-allocated static memory rather than using
  //  allocated buffers, so there are no unRegister-type functions.
  MR::registerCoreQueuingFnsAndOutputCPTasks();
  MR::registerCoreNodeInitDatas();
  MR::registerCoreAttribDataTypes();
  MR::registerCoreTransitConditions();
  MR::registerCoreAssets();
  MR::registerCorePredictionModelTypes();

  // Count up all the default semantics so that we can allocate registry arrays of the correct sizes using allocateRegistry.
  MR::registerCoreAttribSemantics(true);
}

//----------------------------------------------------------------------------------------------------------------------
void Manager::finaliseInitMorphemeLib()
{
  // Register all the default semantics.
  // These are stored in the arrays that have been allocated by allocateRegistry().
  MR::registerCoreAttribSemantics(false);

  // Validate that every requested register entry was filled with a name
  NMP_USED_FOR_ASSERTS(Manager& instance = getInstance());
  for (uint32_t i = 0; i < ATTRIB_SEMANTIC_NM_MAX; ++i)
  {
    NMP_ASSERT(instance.getAttributeSemanticName(i));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Manager::allocateRegistry()
{
  // All semantic have been registered (registerAttributeSemanticID) and the highest semantic to be registered has been determined.
  // We can now allocate the array that holds reference creation tasks for each of them.
  NMP_ASSERT(m_taskIDsCreateReferenceToInput == NULL);
  NMP_ASSERT(m_highestRegisteredAttribSemantics > 0);
  NMP::Memory::Format memReqs(sizeof(TaskID) * getNumRegisteredAttribSemantics(), NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource desc = NMPMemoryAllocateFromFormat(memReqs);
  NMP_ASSERT(desc.ptr);
  m_taskIDsCreateReferenceToInput = (TaskID*) desc.ptr;
  memset(m_taskIDsCreateReferenceToInput, 0, memReqs.size);

  // Semantic names array.
  NMP_ASSERT(m_attribSemanticNames == NULL);
  memReqs.set(sizeof(char*) * getNumRegisteredAttribSemantics(), NMP_NATURAL_TYPE_ALIGNMENT);
  desc = NMPMemoryAllocateFromFormat(memReqs);
  NMP_ASSERT(desc.ptr);
  m_attribSemanticNames = (const char**) desc.ptr;
  memset((void*)m_attribSemanticNames, 0, memReqs.size);

  // Semantic senses array.
  NMP_ASSERT(m_attribSemanticSenses == NULL);
  memReqs.set(sizeof(AttribSemanticSense) * getNumRegisteredAttribSemantics(), NMP_NATURAL_TYPE_ALIGNMENT);
  desc = NMPMemoryAllocateFromFormat(memReqs);
  NMP_ASSERT(desc.ptr);
  m_attribSemanticSenses = (AttribSemanticSense*) desc.ptr;
  memset((void*)m_attribSemanticSenses, 0, memReqs.size);
}

//----------------------------------------------------------------------------------------------------------------------
TaskID Manager::getCreateReferenceToInputTaskID(uint32_t semantic_id) const
{
  NMP_ASSERT(m_taskIDsCreateReferenceToInput != NULL);
  NMP_ASSERT(semantic_id <= m_highestRegisteredAttribSemantics);

  return m_taskIDsCreateReferenceToInput[semantic_id];
}

//----------------------------------------------------------------------------------------------------------------------
void Manager::setCreateReferenceToInputTaskID(uint32_t semantic_id, TaskID task_id)
{
  NMP_ASSERT(m_taskIDsCreateReferenceToInput != NULL);
  NMP_ASSERT(semantic_id <= m_highestRegisteredAttribSemantics);

  m_taskIDsCreateReferenceToInput[semantic_id] = task_id;
}

//----------------------------------------------------------------------------------------------------------------------
Manager::Manager()
{
  m_initialised = false;
  m_objectPool = NULL;
  m_numRegisteredTaskQueuingFns = 0;
  m_numRegisteredOutputCPTasks = 0;
  m_numRegisteredNodeInitDatas = 0;
  m_highestRegisteredAttribDataType = 0;
  m_numRegisteredTransitConds = 0;
  m_numInitNodeInstanceFns = 0;
  m_numRegisteredDeleteNodeInstanceFns = 0;
  m_numRegisteredUpdateNodeConnectionsFns = 0;
  m_numRegisteredFindGeneratingNodeForSemanticFns = 0;
  m_numRegisteredAssets = 0;
  m_numRegisteredAnimationFormats = 0;
  m_numRegisteredMessageDatas = 0;
  m_numRegisteredMessageHandlerFns = 0;
  m_highestRegisteredAttribSemantics = 0;
  m_attribSemanticNames = NULL;
  m_attribSemanticSenses = NULL;
  m_highestRegisteredPredictionType = 0;

  // Initialise the AttribDataType registry.
  for (uint32_t i = 0; i < MAX_NUM_ATTR_DATA_TYPES; ++i)
  {
    m_attrDataRegistry[i].m_locateFn = NULL;
    m_attrDataRegistry[i].m_dislocateFn = NULL;
    m_attrDataRegistry[i].m_relocateFn = NULL;
#if defined(MR_OUTPUT_DEBUGGING)
    m_attrDataRegistry[i].m_serializeTxFn = NULL;
#endif // MR_OUTPUT_DEBUGGING
#if defined(NMP_ENABLE_ASSERTS)
    m_attrDataRegistry[i].m_attribTypeName = NULL;
#endif
  }

  // Initialise the PredictionType registry.
  for (uint32_t i = 0; i < MAX_NUM_PREDICTION_MODEL_TYPES; ++i)
  {
    m_predictionRegistry[i].m_locateFn = NULL;
    m_predictionRegistry[i].m_dislocateFn = NULL;
#if defined(NMP_ENABLE_ASSERTS)
    m_predictionRegistry[i].m_predictionModelTypeName = NULL;
#endif
  }

  m_requestAnimFn = NULL;
  m_releaseAnimFn = NULL;
  m_taskIDsCreateReferenceToInput = NULL;

  // Initialise the object/asset registry
  uint32_t numObjects = MAX_NUM_REGISTERED_OBJECTS;
  NMP::Memory::Format entryMemoryFormat;
  entryMemoryFormat.alignment = NMP_NATURAL_TYPE_ALIGNMENT;
  entryMemoryFormat.size = sizeof(ObjectRegistryEntry);

  NMP::Memory::Format format = NMP::StaticFreeList::getMemoryRequirements(entryMemoryFormat, numObjects);
  NMP::Memory::Resource desc = NMPMemoryAllocateFromFormat(format);
  NMP_ASSERT(desc.ptr);
  m_objectPool = NMP::StaticFreeList::init(desc, entryMemoryFormat, numObjects);

  // Initialise empty list head entry.
  m_objRegistry.m_ptr       = NULL;
  m_objRegistry.m_next      = NULL;
  m_objRegistry.m_objectID  = INVALID_OBJECT_ID;
  m_objRegistry.m_typeID    = kAsset_Invalid;

  // Initialise profiling (note that this is a null op if NM_PROFILING is not defined)
  NM_INIT_PROFILING();

  m_initialised = true;

  // add default core animation formats
  registerAnimationFormat(
    "asa",
    ANIM_TYPE_ASA,
    locateDefaultAnimFormatFn<MR::AnimSourceASA>,
    dislocateDefaultAnimFormatFn<MR::AnimSourceASA>,
    MR::nodeAnimSyncEventsQueueSampleTransformsFromAnimSourceASA,
    MR::nodeAnimSyncEventsQueueTrajectoryDeltaTransformASA,
    MR::nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSourceASA);

  registerAnimationFormat(
    "mba",
    ANIM_TYPE_MBA,
    locateDefaultAnimFormatFn<MR::AnimSourceMBA>,
    dislocateDefaultAnimFormatFn<MR::AnimSourceMBA>,
    MR::nodeAnimSyncEventsQueueSampleTransformsFromAnimSourceMBA,
    MR::nodeAnimSyncEventsQueueTrajectoryDeltaTransformMBA,
    MR::nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSourceMBA);

  registerAnimationFormat(
    "nsa",
    ANIM_TYPE_NSA,
    locateDefaultAnimFormatFn<MR::AnimSourceNSA>,
    dislocateDefaultAnimFormatFn<MR::AnimSourceNSA>,
    MR::nodeAnimSyncEventsQueueSampleTransformsFromAnimSourceNSA,
    MR::nodeAnimSyncEventsQueueTrajectoryDeltaTransformNSA,
    MR::nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSourceNSA);

  registerAnimationFormat(
    "qsa",
    ANIM_TYPE_QSA,
    locateDefaultAnimFormatFn<MR::AnimSourceQSA>,
    dislocateDefaultAnimFormatFn<MR::AnimSourceQSA>,
    MR::nodeAnimSyncEventsQueueSampleTransformsFromAnimSourceQSA,
    MR::nodeAnimSyncEventsQueueTrajectoryDeltaTransformQSA,
    MR::nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSourceQSA);
}

//----------------------------------------------------------------------------------------------------------------------
void Manager::setAnimFileHandlingFunctions(
  RequestAnimFn* requestAnimFn, // Tells manager the function to call when a reference to an identified
                                //  animation is required by an animation set.
                                //  External function may for example need to load the animation and
                                //  perhaps increase a ref count on it.
  ReleaseAnimFn* releaseAnimFn) // Tells manager the function to call when a reference to an animation
                                //  is no longer needed by an animation set.
                                //  External function may for example need to decrease a ref count and
                                //  possibly deallocate an animation.
{
  sm_instance->m_requestAnimFn = requestAnimFn;
  sm_instance->m_releaseAnimFn = releaseAnimFn;
}

//----------------------------------------------------------------------------------------------------------------------
Manager::RequestAnimFn* Manager::getRequestAnimFn() const
{
  return sm_instance->m_requestAnimFn;
}

//----------------------------------------------------------------------------------------------------------------------
Manager::ReleaseAnimFn* Manager::getReleaseAnimFn() const
{
  return sm_instance->m_releaseAnimFn;
}

//----------------------------------------------------------------------------------------------------------------------
void Manager::termMorphemeLib()
{
  if (!sm_instance)
    return;

  // Release profiling (note that this is a null op if NM_PROFILING is not defined)
  NM_RELEASE_PROFILING();

  sm_instance->m_initialised = false;

  if (sm_instance->m_attribSemanticNames)
  {
    NMP::Memory::memFree((void*)sm_instance->m_attribSemanticNames);
    sm_instance->m_attribSemanticNames = NULL;
  }

  if (sm_instance->m_attribSemanticSenses)
  {
    NMP::Memory::memFree((void*)sm_instance->m_attribSemanticSenses);
    sm_instance->m_attribSemanticSenses = NULL;
  }

  if (sm_instance->m_taskIDsCreateReferenceToInput)
  {
    NMP::Memory::memFree(sm_instance->m_taskIDsCreateReferenceToInput);
    sm_instance->m_taskIDsCreateReferenceToInput = NULL;
  }

  if (sm_instance->m_objectPool)
  {
    NMP::Memory::memFree(sm_instance->m_objectPool);
    sm_instance->m_objectPool = NULL;
  }

  NMP::Memory::memFree(sm_instance);


  sm_instance = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
// Object/Asset registry functions.
//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::incObjectRefCount(const void* ptr)
{
  NMP_ASSERT(ptr != 0);
  ObjectRegistryEntry* entry = sm_instance->FindRegEntryFromPtr(ptr);
  if (entry == NULL)
  {
    NMP_DEBUG_MSG("Manager: Trying to increase the ref count of an unregistered an object");
    return 0xFFFFFFFF;
  }

  ++(entry->m_refCount);
  return entry->m_refCount;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::incObjectRefCount(ObjectID objectID)
{
  NMP_ASSERT(objectID != 0);
  ObjectRegistryEntry* entry = sm_instance->FindRegEntryFromID(objectID);
  if (entry == NULL)
  {
    NMP_DEBUG_MSG("Manager: Trying to increase the ref count of an unregistered an object");
    return 0xFFFFFFFF;
  }

  ++(entry->m_refCount);
  return entry->m_refCount;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::decObjectRefCount(const void* ptr)
{
  NMP_ASSERT(ptr != 0);
  ObjectRegistryEntry* entry = sm_instance->FindRegEntryFromPtr(ptr);
  if (entry == NULL)
  {
    NMP_DEBUG_MSG("Manager: Trying to decrease the ref count of an unregistered object");
    return 0xFFFFFFFF;
  }

  NMP_ASSERT_MSG(
    entry->m_refCount > 0,
    "Manager: Trying to decrease the ref count of an object, ID: %d, whose ref count is 0 already",
    entry->m_objectID);

  --(entry->m_refCount);
  return entry->m_refCount;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::decObjectRefCount(ObjectID objectID)
{
  NMP_ASSERT(objectID != 0);
  ObjectRegistryEntry* entry = sm_instance->FindRegEntryFromID(objectID);
  if (entry == NULL)
  {
    NMP_DEBUG_MSG("Manager: Trying to decrease the ref count of an unregistered object");
    return 0xFFFFFFFF;
  }

  NMP_ASSERT_MSG(
    entry->m_refCount > 0,
    "Manager: Trying to decrease the ref count of an object, ID: %d, whose ref count is 0 already",
    entry->m_objectID);

  --(entry->m_refCount);
  return entry->m_refCount;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getObjectRefCount(ObjectID objectID)
{
  NMP_ASSERT(objectID != 0);
  ObjectRegistryEntry* entry = sm_instance->FindRegEntryFromID(objectID);
  if (entry == NULL)
  {
    return 0xFFFFFFFF;
  }
  return entry->m_refCount;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getObjectRefCount(const void* ptr)
{
  NMP_ASSERT(ptr != 0);
  ObjectRegistryEntry* entry = sm_instance->FindRegEntryFromPtr(ptr);
  if (entry == NULL)
  {
    return 0xFFFFFFFF;
  }
  return entry->m_refCount;
}

//----------------------------------------------------------------------------------------------------------------------
bool Manager::registerObject(const void* ptr, AssetType typeID, ObjectID objectID)
{
  NMP_ASSERT(objectID != 0);
  NMP_ASSERT(ptr != 0);
  // Check if an object with this unique runtime ID has already been registered.
  if (FindRegEntryFromID(objectID) != NULL)
  {
    NMP_DEBUG_MSG("An object with ID %d has already been registered with MR::Manager.\n", objectID);
    return false;
  }

  // Create a new object registry entry from the pool.
  ObjectRegistryEntry* newEntry = (ObjectRegistryEntry*) m_objectPool->allocateEntry();
  NMP_ASSERT_MSG(newEntry, "Manager Object pool full, try increasing MAX_NUM_REGISTERED_OBJECTS in mrDefines.h");
  newEntry->m_located = true;
  newEntry->m_objectID = objectID;
  newEntry->m_ptr = ptr;
  newEntry->m_refCount = 0;
  newEntry->m_typeID = typeID;
  newEntry->m_next = NULL;

  // Insert the new object into the registry (currently we just stick it at the end).
  ObjectRegistryEntry* currentEntry = &m_objRegistry;
  while (currentEntry->m_next != NULL)
  {
    currentEntry = currentEntry->m_next;
  }
  currentEntry->m_next = newEntry;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Manager::unregisterObject(const void* ptr)
{
  NMP_ASSERT(ptr != 0);
  ObjectRegistryEntry* prev = NULL;
  ObjectRegistryEntry* entry = FindRegEntryFromPtr(ptr, &prev);

  if (entry == NULL)
  {
    NMP_DEBUG_MSG("Manager: Trying to unregister an object that is not already registered");
    return false;
  }

  NMP_ASSERT_MSG(
    entry->m_refCount == 0,
    "Manager: Trying to unregister an object, ID: %d, whose ref count is not 0: %d",
    entry->m_objectID,
    entry->m_refCount);

  prev->m_next = entry->m_next;
  m_objectPool->deallocateEntry(entry);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Manager::unregisterObject(uint32_t objectID)
{
  NMP_ASSERT(objectID != 0);
  ObjectRegistryEntry* prev = NULL;
  ObjectRegistryEntry* entry = FindRegEntryFromID(objectID, &prev);

  if (entry == NULL)
  {
    NMP_DEBUG_MSG("Manager: Trying to unregister an object that is not already registered");
    return false;
  }

  NMP_ASSERT_MSG(
    entry->m_refCount == 0,
    "Manager: Trying to unregister an object, ID: %d, whose ref count is not 0: %d",
    entry->m_objectID,
    entry->m_refCount);

  prev->m_next = entry->m_next;
  m_objectPool->deallocateEntry(entry);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Manager::objectIsRegistered(uint32_t objectID)
{
  NMP_ASSERT(objectID != 0);
  return (FindRegEntryFromID(objectID) != NULL);
}

//----------------------------------------------------------------------------------------------------------------------
Manager::AssetType Manager::getRegisteredObjectType(uint32_t objectID)
{
  NMP_ASSERT(objectID != 0);
  return FindRegEntryFromID(objectID)->m_typeID;
}

//----------------------------------------------------------------------------------------------------------------------
const void* Manager::getObjectPtrFromObjectID(ObjectID objectID)
{
  NMP_ASSERT(objectID != 0);
  ObjectRegistryEntry* entry = FindRegEntryFromID(objectID);

  if (entry != NULL)
  {
    return entry->m_ptr;
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
ObjectID Manager::getObjectIDFromObjectPtr(const void* ptr)
{
  NMP_ASSERT(ptr != 0);
  ObjectRegistryEntry* entry = FindRegEntryFromPtr(ptr);

  if (entry != NULL)
  {
    return entry->m_objectID;
  }

  return INVALID_OBJECT_ID;
}

//----------------------------------------------------------------------------------------------------------------------
Manager::AssetType Manager::getAssetTypeFromObjectPtr(const void* ptr)
{
  NMP_ASSERT(ptr != 0);
  ObjectRegistryEntry* entry = FindRegEntryFromPtr(ptr);
  NMP_ASSERT(entry);
  return entry->m_typeID;
}

//----------------------------------------------------------------------------------------------------------------------
Manager::ObjectRegistryEntry* Manager::FindRegEntryFromID(ObjectID objectID, ObjectRegistryEntry** prev)
{
  NMP_ASSERT(objectID != 0);
  ObjectRegistryEntry* prevEntry = &m_objRegistry;
  ObjectRegistryEntry* currentEntry = prevEntry->m_next;

  while (currentEntry != NULL)
  {
    if (currentEntry->m_objectID == objectID)
    {
      // Found the entry.  Pass back the previous entry if it was requested.
      if (prev)
      {
        *prev = prevEntry;
      }
      return currentEntry;
    }

    prevEntry = currentEntry;
    currentEntry = currentEntry->m_next;
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
Manager::ObjectRegistryEntry* Manager::FindRegEntryFromPtr(const void* ptr, ObjectRegistryEntry** prev)
{
  NMP_ASSERT(ptr != 0);
  ObjectRegistryEntry* prevEntry = &m_objRegistry;
  ObjectRegistryEntry* currentEntry = prevEntry->m_next;

  while (currentEntry != NULL)
  {
    if (currentEntry->m_ptr == ptr)
    {
      // Found the entry.  Pass back the previous entry if it was requested.
      if (prev)
      {
        *prev = prevEntry;
      }
      return currentEntry;
    }

    prevEntry = currentEntry;
    currentEntry = currentEntry->m_next;
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
// QueueAttrTaskFn registration.
//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::registerTaskQueuingFn(QueueAttrTaskFn queuingFn, const char* fnName)
{
  NMP_ASSERT(m_numRegisteredTaskQueuingFns < MAX_NUM_QUEUING_FNS);

  // In release mode functions that compile to the same code get stripped, 
  //  this can lead to duplicate function registration here.
  NMP_VERIFY_MSG(
    getTaskQueuingFnID(queuingFn) == INVALID_FUNCTION_ID,
    "%s already registered against %s (Compiler optimisation removing duplicate functionality?)",
    fnName,
    getTaskQueuingFnName(queuingFn));

  uint32_t fnID = m_numRegisteredTaskQueuingFns + 1;
  m_taskQueuingFns[m_numRegisteredTaskQueuingFns].fnID = fnID;
  m_taskQueuingFns[m_numRegisteredTaskQueuingFns].queuingFn = queuingFn;
  m_taskQueuingFns[m_numRegisteredTaskQueuingFns].fnName = fnName;
  m_numRegisteredTaskQueuingFns++;
  return fnID;
}

//----------------------------------------------------------------------------------------------------------------------
QueueAttrTaskFn Manager::getTaskQueuingFn(uint32_t fnID) const
{
  QueueAttrTaskFn result = NULL;

  for (uint32_t i = 0; i < m_numRegisteredTaskQueuingFns; i++)
  {
    if (m_taskQueuingFns[i].fnID == fnID)
    {
      result = m_taskQueuingFns[i].queuingFn;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getTaskQueuingFnID(QueueAttrTaskFn queuingFn) const
{
  uint32_t result = INVALID_FUNCTION_ID;

  for (uint32_t i = 0; i < m_numRegisteredTaskQueuingFns; ++i)
  {
    if (m_taskQueuingFns[i].queuingFn == queuingFn)
    {
      result = m_taskQueuingFns[i].fnID;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* Manager::getTaskQueuingFnName(QueueAttrTaskFn queuingFn) const
{
  for (uint32_t i = 0; i < m_numRegisteredTaskQueuingFns; ++i)
  {
    if (m_taskQueuingFns[i].queuingFn == queuingFn)
    {
      return m_taskQueuingFns[i].fnName;
    }
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getTaskQueuingFnID(const char* fnName) const
{
  uint32_t result = INVALID_FUNCTION_ID;

  for (uint32_t i = 0; i < m_numRegisteredTaskQueuingFns; ++i)
  {
    NMP_ASSERT(m_taskQueuingFns[i].fnName);
    if (strcmp(m_taskQueuingFns[i].fnName, fnName) == 0)
    {
      result = m_taskQueuingFns[i].fnID;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// OutputCPTask registration.
//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::registerOutputCPTask(OutputCPTask outputCPTask, const char* fnName)
{
  NMP_ASSERT(m_numRegisteredOutputCPTasks < MAX_NUM_IMMEDIATE_FNS);

  // In release mode functions that compile to the same code get stripped, 
  //  this can lead to duplicate function registration here.
  NMP_ASSERT_MSG(
    getOutputCPTaskID(outputCPTask) == INVALID_FUNCTION_ID,
    "%s already registered against %s (Compiler optimisation removing duplicate functionality?)",
    fnName,
    getOutputCPTaskName(outputCPTask));
 
  uint32_t fnID = m_numRegisteredOutputCPTasks + 1;
  m_outputCPTasks[m_numRegisteredOutputCPTasks].fnID = fnID;
  m_outputCPTasks[m_numRegisteredOutputCPTasks].outputCPTask = outputCPTask;
  m_outputCPTasks[m_numRegisteredOutputCPTasks].fnName = fnName;
  NET_LOG_MESSAGE(109, "   OutputCPTask ID: %4d registered to: %s\n", m_numRegisteredOutputCPTasks, fnName);
  m_numRegisteredOutputCPTasks++;
  return fnID;
}

//----------------------------------------------------------------------------------------------------------------------
OutputCPTask Manager::getOutputCPTask(uint32_t fnID) const
{
  OutputCPTask result = NULL;

  for (uint32_t i = 0; i < m_numRegisteredOutputCPTasks; ++i)
  {
    if (m_outputCPTasks[i].fnID == fnID)
    {
      result = m_outputCPTasks[i].outputCPTask;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getOutputCPTaskID(const char* fnName) const
{
  uint32_t result = 0xFFFFFFFF;

  for (uint32_t i = 0; i < m_numRegisteredOutputCPTasks; ++i)
  {
    NMP_ASSERT(m_outputCPTasks[i].fnName);
    if (strcmp(m_outputCPTasks[i].fnName, fnName) == 0)
    {
      result = m_outputCPTasks[i].fnID;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getOutputCPTaskID(OutputCPTask outputCPTask) const
{
  uint32_t result = 0xFFFFFFFF;

  for (uint32_t i = 0; i < m_numRegisteredOutputCPTasks; ++i)
  {
    if (m_outputCPTasks[i].outputCPTask == outputCPTask)
    {
      result = m_outputCPTasks[i].fnID;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* Manager::getOutputCPTaskName(OutputCPTask outputCPTask) const
{
  for (uint32_t i = 0; i < m_numRegisteredOutputCPTasks; ++i)
  {
    if (m_outputCPTasks[i].outputCPTask == outputCPTask)
    {
      return m_outputCPTasks[i].fnName;
    }
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
// InitNodeInstance registration.
//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::registerInitNodeInstanceFn(InitNodeInstance initNodeFn, const char* fnName)
{
  NMP_ASSERT(m_numInitNodeInstanceFns < MAX_NUM_NODE_TYPES);

  // In release mode functions that compile to the same code get stripped, 
  //  this can lead to duplicate function registration here.
  NMP_VERIFY_MSG(
    getInitNodeInstanceFnID(initNodeFn) == INVALID_FUNCTION_ID,
    "%s already registered against %s (Compiler optimisation removing duplicate functionality?)",
    fnName,
    getInitNodeInstanceFnName(initNodeFn));

  uint32_t fnID = m_numInitNodeInstanceFns + 1;
  m_initNodeInstanceFns[m_numInitNodeInstanceFns].fnID = fnID;
  m_initNodeInstanceFns[m_numInitNodeInstanceFns].initFn = initNodeFn;
  m_initNodeInstanceFns[m_numInitNodeInstanceFns].fnName = fnName;
  NET_LOG_MESSAGE(109, "   InitNodeInstanceFn ID: %4d registered to: %s\n", m_numInitNodeInstanceFns, fnName);
  m_numInitNodeInstanceFns++;
  return fnID;
}

//----------------------------------------------------------------------------------------------------------------------
InitNodeInstance Manager::getInitNodeInstanceFn(uint32_t fnID) const
{
  InitNodeInstance result = NULL;

  for (uint32_t i = 0; i < m_numInitNodeInstanceFns; ++i)
  {
    if (m_initNodeInstanceFns[i].fnID == fnID)
    {
      result = m_initNodeInstanceFns[i].initFn;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getInitNodeInstanceFnID(InitNodeInstance initNodeFn) const
{
  uint32_t result = 0xFFFFFFFF;

  for (uint32_t i = 0; i < m_numInitNodeInstanceFns; ++i)
  {
    if (m_initNodeInstanceFns[i].initFn == initNodeFn)
    {
      result = m_initNodeInstanceFns[i].fnID;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* Manager::getInitNodeInstanceFnName(InitNodeInstance initNodeFn) const
{
  for (uint32_t i = 0; i < m_numInitNodeInstanceFns; ++i)
  {
    if (m_initNodeInstanceFns[i].initFn == initNodeFn)
    {
      return m_initNodeInstanceFns[i].fnName;
    }
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getInitNodeInstanceFnID(const char* fnName) const
{
  uint32_t result = 0xFFFFFFFF;

  for (uint32_t i = 0; i < m_numInitNodeInstanceFns; ++i)
  {
    NMP_ASSERT(m_initNodeInstanceFns[i].fnName);
    if (strcmp(m_initNodeInstanceFns[i].fnName, fnName) == 0)
    {
      result = m_initNodeInstanceFns[i].fnID;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// Semantic registration.
//----------------------------------------------------------------------------------------------------------------------
void Manager::registerAttributeSemanticID(
  uint32_t    id,
  const char* semanticName,
  bool        computeRegRequirements,
  AttribSemanticSense attribSemanticSense)
{
  if (computeRegRequirements)
  {
    // We are determining the registration requirements for the Manager prior to actual registration of semantics.
    NMP_ASSERT(!m_attribSemanticNames && !m_taskIDsCreateReferenceToInput);
    if ((id + 1) > m_highestRegisteredAttribSemantics)
      m_highestRegisteredAttribSemantics = id + 1;
  }
  else
  {
    // We are actually registering now so all registration arrays must have been allocated.
    NMP_ASSERT(m_attribSemanticNames && m_taskIDsCreateReferenceToInput && !m_attribSemanticNames[id]);
    NMP_ASSERT(id < m_highestRegisteredAttribSemantics);
    NMP_ASSERT(semanticName);
    m_attribSemanticNames[id] = semanticName;
    m_attribSemanticSenses[id] = attribSemanticSense;
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getHighestRegisteredAttribSemantics() const
{
  return m_highestRegisteredAttribSemantics;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getNumRegisteredAttribSemantics() const
{
  return getHighestRegisteredAttribSemantics() + 1;
}

//----------------------------------------------------------------------------------------------------------------------
const char* Manager::getAttributeSemanticName(uint32_t id) const
{
  NMP_ASSERT(m_attribSemanticNames && (id < getNumRegisteredAttribSemantics()));
  NMP_ASSERT(m_attribSemanticNames[id]);
  return m_attribSemanticNames[id];
}

//----------------------------------------------------------------------------------------------------------------------
AttribSemanticSense Manager::getAttributeSemanticSense(uint32_t id) const
{
  NMP_ASSERT(m_attribSemanticSenses && (id < getNumRegisteredAttribSemantics()));
  return m_attribSemanticSenses[id];
}

//----------------------------------------------------------------------------------------------------------------------
// DeleteNodeInstance registration.
//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::registerDeleteNodeInstanceFn(DeleteNodeInstance deleteFn, const char* fnName)
{
  NMP_ASSERT(m_numRegisteredDeleteNodeInstanceFns < MAX_NUM_NODE_TYPES);

  // In release mode functions that compile to the same code get stripped, 
  //  this can lead to duplicate function registration here.
  NMP_VERIFY_MSG(
    getDeleteNodeInstanceFnID(deleteFn) == INVALID_FUNCTION_ID,
    "%s already registered against %s (Compiler optimisation removing duplicate functionality?)",
    fnName,
    getDeleteNodeInstanceFnName(deleteFn));

  uint32_t fnID = m_numRegisteredDeleteNodeInstanceFns + 1;
  m_deleteNodeInstanceFns[m_numRegisteredDeleteNodeInstanceFns].fnID = fnID;
  m_deleteNodeInstanceFns[m_numRegisteredDeleteNodeInstanceFns].deleteFn = deleteFn;
  m_deleteNodeInstanceFns[m_numRegisteredDeleteNodeInstanceFns].fnName = fnName;
  NET_LOG_MESSAGE(109, "   DeleteNodeInstanceFn ID: %4d registered to: %s\n", m_numRegisteredDeleteNodeInstanceFns, fnName);
  m_numRegisteredDeleteNodeInstanceFns++;
  return fnID;
}

//----------------------------------------------------------------------------------------------------------------------
DeleteNodeInstance Manager::getDeleteNodeInstanceFn(uint32_t fnID) const
{
  DeleteNodeInstance result = NULL;

  for (uint32_t i = 0; i < m_numRegisteredDeleteNodeInstanceFns; ++i)
  {
    if (m_deleteNodeInstanceFns[i].fnID == fnID)
    {
      result = m_deleteNodeInstanceFns[i].deleteFn;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getDeleteNodeInstanceFnID(DeleteNodeInstance deleteFn) const
{
  uint32_t result = 0xFFFFFFFF;

  for (uint32_t i = 0; i < m_numRegisteredDeleteNodeInstanceFns; ++i)
  {
    if (m_deleteNodeInstanceFns[i].deleteFn == deleteFn)
    {
      result = m_deleteNodeInstanceFns[i].fnID;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* Manager::getDeleteNodeInstanceFnName(DeleteNodeInstance deleteFn) const
{
  for (uint32_t i = 0; i < m_numRegisteredDeleteNodeInstanceFns; ++i)
  {
    if (m_deleteNodeInstanceFns[i].deleteFn == deleteFn)
    {
      return m_deleteNodeInstanceFns[i].fnName;
    }
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getDeleteNodeInstanceFnID(const char* fnName) const
{
  uint32_t result = 0xFFFFFFFF;

  for (uint32_t i = 0; i < m_numRegisteredDeleteNodeInstanceFns; ++i)
  {
    NMP_ASSERT(m_deleteNodeInstanceFns[i].fnName);
    if (strcmp(m_deleteNodeInstanceFns[i].fnName, fnName) == 0)
    {
      result = m_deleteNodeInstanceFns[i].fnID;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// UpdateNodeConnections registration.
//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::registerUpdateNodeConnectionsFn(UpdateNodeConnections updateNodeConnectionsFn, const char* fnName)
{
  NMP_ASSERT(m_numRegisteredUpdateNodeConnectionsFns < MAX_NUM_NODE_TYPES);

  // In release mode functions that compile to the same code get stripped, 
  //  this can lead to duplicate function registration here.
  NMP_ASSERT_MSG(
    getUpdateNodeConnectionsFnID(updateNodeConnectionsFn) == INVALID_FUNCTION_ID,
    "%s already registered against %s (Compiler optimisation removing duplicate functionality?)",
    fnName,
    getUpdateNodeConnectionsFnName(updateNodeConnectionsFn));

  uint32_t fnID = m_numRegisteredUpdateNodeConnectionsFns + 1;
  m_updateNodeConnectionsFns[m_numRegisteredUpdateNodeConnectionsFns].fnID = fnID;
  m_updateNodeConnectionsFns[m_numRegisteredUpdateNodeConnectionsFns].updateConnectionsFn = updateNodeConnectionsFn;
  m_updateNodeConnectionsFns[m_numRegisteredUpdateNodeConnectionsFns].fnName = fnName;
  NET_LOG_MESSAGE(109, "   UpdateNodeConnectionsFn ID: %4d registered to: %s\n", fnID, fnName);
  m_numRegisteredUpdateNodeConnectionsFns++;

  return fnID;
}

//----------------------------------------------------------------------------------------------------------------------
UpdateNodeConnections Manager::getUpdateNodeConnectionsFn(uint32_t fnID) const
{
  UpdateNodeConnections result = NULL;

  for (uint32_t i = 0; i < m_numRegisteredUpdateNodeConnectionsFns; ++i)
  {
    if (m_updateNodeConnectionsFns[i].fnID == fnID)
    {
      result = m_updateNodeConnectionsFns[i].updateConnectionsFn;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getUpdateNodeConnectionsFnID(UpdateNodeConnections updateNodeConnectionsFn) const
{
  uint32_t result = 0xFFFFFFFF;

  for (uint32_t i = 0; i < m_numRegisteredUpdateNodeConnectionsFns; ++i)
  {
    if (m_updateNodeConnectionsFns[i].updateConnectionsFn == updateNodeConnectionsFn)
    {
      result = m_updateNodeConnectionsFns[i].fnID;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* Manager::getUpdateNodeConnectionsFnName(UpdateNodeConnections updateNodeConnectionsFn) const
{
  for (uint32_t i = 0; i < m_numRegisteredUpdateNodeConnectionsFns; ++i)
  {
    if (m_updateNodeConnectionsFns[i].updateConnectionsFn == updateNodeConnectionsFn)
    {
      return m_updateNodeConnectionsFns[i].fnName;
    }
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getUpdateNodeConnectionsFnID(const char* fnName) const
{
  uint32_t result = 0xFFFFFFFF;

  for (uint32_t i = 0; i < m_numRegisteredUpdateNodeConnectionsFns; ++i)
  {
    NMP_ASSERT(m_updateNodeConnectionsFns[i].fnName);
    if (strcmp(m_updateNodeConnectionsFns[i].fnName, fnName) == 0)
    {
      result = m_updateNodeConnectionsFns[i].fnID;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// HasQueuingFunctionForSemanticFn registartion.
//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::registerFindGeneratingNodeForSemanticFn(
  FindGeneratingNodeForSemanticFn generatesAttribSemanticTypeFn,
  const char* fnName)
{
  NMP_ASSERT(m_numRegisteredFindGeneratingNodeForSemanticFns < MAX_NUM_NODE_TYPES);

  // In release mode functions that compile to the same code get stripped, 
  //  this can lead to duplicate function registration here.
  NMP_VERIFY_MSG(
    getFindGeneratingNodeForSemanticFnID(generatesAttribSemanticTypeFn) == INVALID_FUNCTION_ID,
    "%s already registered against %s (Compiler optimisation removing duplicate functionality?)",
    fnName,
    getFindGeneratingNodeForSemanticFnName(generatesAttribSemanticTypeFn));

  uint32_t fnID = m_numRegisteredFindGeneratingNodeForSemanticFns + 1;
  m_nodeFindGeneratingNodeForSemanticFns[m_numRegisteredFindGeneratingNodeForSemanticFns].fnID = fnID;
  m_nodeFindGeneratingNodeForSemanticFns[m_numRegisteredFindGeneratingNodeForSemanticFns].findGeneratingNodeForSemanticFn = generatesAttribSemanticTypeFn;
  m_nodeFindGeneratingNodeForSemanticFns[m_numRegisteredFindGeneratingNodeForSemanticFns].fnName = fnName;
  NET_LOG_MESSAGE(109, "   FindGeneratingNodeForSemanticFn ID: %4d registered to: %s\n", m_numRegisteredFindGeneratingNodeForSemanticFns, fnName);
  m_numRegisteredFindGeneratingNodeForSemanticFns++;
  return fnID;
}

//----------------------------------------------------------------------------------------------------------------------
FindGeneratingNodeForSemanticFn Manager::getFindGeneratingNodeForSemanticFn(uint32_t fnID) const
{
  FindGeneratingNodeForSemanticFn result = NULL;

  for (uint32_t i = 0; i < m_numRegisteredFindGeneratingNodeForSemanticFns; ++i)
  {
    if (m_nodeFindGeneratingNodeForSemanticFns[i].fnID == fnID)
    {
      result = m_nodeFindGeneratingNodeForSemanticFns[i].findGeneratingNodeForSemanticFn;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getFindGeneratingNodeForSemanticFnID(FindGeneratingNodeForSemanticFn findGeneratingNodeForSemanticFn) const
{
  uint32_t result = 0xFFFFFFFF;

  for (uint32_t i = 0; i < m_numRegisteredFindGeneratingNodeForSemanticFns; ++i)
  {
    if (m_nodeFindGeneratingNodeForSemanticFns[i].findGeneratingNodeForSemanticFn == findGeneratingNodeForSemanticFn)
    {
      result = m_nodeFindGeneratingNodeForSemanticFns[i].fnID;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* Manager::getFindGeneratingNodeForSemanticFnName(FindGeneratingNodeForSemanticFn findGeneratingNodeForSemanticFn) const
{
  for (uint32_t i = 0; i < m_numRegisteredFindGeneratingNodeForSemanticFns; ++i)
  {
    if (m_nodeFindGeneratingNodeForSemanticFns[i].findGeneratingNodeForSemanticFn == findGeneratingNodeForSemanticFn)
    {
      return m_nodeFindGeneratingNodeForSemanticFns[i].fnName;
    }
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getFindGeneratingNodeForSemanticFnID(const char* fnName) const
{
  uint32_t result = 0xFFFFFFFF;

  for (uint32_t i = 0; i < m_numRegisteredFindGeneratingNodeForSemanticFns; ++i)
  {
    NMP_ASSERT(m_nodeFindGeneratingNodeForSemanticFns[i].fnName);
    if (strcmp(m_nodeFindGeneratingNodeForSemanticFns[i].fnName, fnName) == 0)
    {
      result = m_nodeFindGeneratingNodeForSemanticFns[i].fnID;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// MessageHandlerFn registration.
//----------------------------------------------------------------------------------------------------------------------
bool Manager::registerMessageDataType(
  MessageType              messageType,
  MessageDataLocateFn      locateFn,
  MessageDataDislocateFn   dislocateFn)
{
  NMP_ASSERT(m_numRegisteredMessageDatas < MAX_NUM_MESSAGE_TYPES);
  m_messageDataRegistry[m_numRegisteredMessageDatas].m_type = messageType;
  m_messageDataRegistry[m_numRegisteredMessageDatas].m_locateFn = locateFn;
  m_messageDataRegistry[m_numRegisteredMessageDatas].m_dislocateFn = dislocateFn;
  m_numRegisteredMessageDatas++;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MessageDataLocateFn Manager::getMessageDataLocateFn(MessageType messageType)
{
  MessageDataLocateFn result = NULL;

  for (uint32_t i = 0; i < m_numRegisteredMessageDatas; ++i)
  {
    if (m_messageDataRegistry[i].m_type == messageType)
    {
      result = m_messageDataRegistry[i].m_locateFn;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MessageDataDislocateFn Manager::getMessageDataDislocateFn(MessageType messageType)
{
  MessageDataDislocateFn result = NULL;

  for (uint32_t i = 0; i < m_numRegisteredMessageDatas; ++i)
  {
    if (m_messageDataRegistry[i].m_type == messageType)
    {
      result = m_messageDataRegistry[i].m_dislocateFn;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::registerMessageHandlerFn(MessageHandlerFn messageHandlerFn, const char* fnName)
{
  NMP_ASSERT(m_numRegisteredMessageHandlerFns < MAX_NUM_NODE_TYPES);

  // In release mode functions that compile to the same code get stripped, 
  //  this can lead to duplicate function registration here.
  NMP_VERIFY_MSG(
    getMessageHandlerFnID(messageHandlerFn) == INVALID_FUNCTION_ID,
    "%s already registered against %s (Compiler optimisation removing duplicate functionality?)",
    fnName,
    getMessageHandlerFnName(messageHandlerFn));

  uint32_t fnID = m_numRegisteredMessageHandlerFns + 1;
  m_messageHandlerFns[m_numRegisteredMessageHandlerFns].fnID = fnID;
  m_messageHandlerFns[m_numRegisteredMessageHandlerFns].messageHandlerFn = messageHandlerFn;
  m_messageHandlerFns[m_numRegisteredMessageHandlerFns].fnName = fnName;
  NET_LOG_MESSAGE(109, "   MessageHandlerFn ID: %4d registered to: %s\n", m_numRegisteredMessageHandlerFns, fnName);
  m_numRegisteredMessageHandlerFns++;
  return fnID;
}

//----------------------------------------------------------------------------------------------------------------------
MessageHandlerFn Manager::getMessageHandlerFn(uint32_t fnID) const
{
  MessageHandlerFn result = NULL;

  for (uint32_t i = 0; i < m_numRegisteredMessageHandlerFns; ++i)
  {
    if (m_messageHandlerFns[i].fnID == fnID)
    {
      result = m_messageHandlerFns[i].messageHandlerFn;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getMessageHandlerFnID(MessageHandlerFn messageHandlerFn) const
{
  uint32_t result = 0xFFFFFFFF;

  for (uint32_t i = 0; i < m_numRegisteredMessageHandlerFns; ++i)
  {
    if (m_messageHandlerFns[i].messageHandlerFn == messageHandlerFn)
    {
      result = m_messageHandlerFns[i].fnID;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
const char* Manager::getMessageHandlerFnName(MessageHandlerFn messageHandlerFn) const
{
  for (uint32_t i = 0; i < m_numRegisteredMessageHandlerFns; ++i)
  {
    if (m_messageHandlerFns[i].messageHandlerFn == messageHandlerFn)
    {
      return m_messageHandlerFns[i].fnName;
    }
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getMessageHandlerFnID(const char* fnName) const
{
  uint32_t result = 0xFFFFFFFF;

  for (uint32_t i = 0; i < m_numRegisteredMessageHandlerFns; ++i)
  {
    NMP_ASSERT(m_messageHandlerFns[i].fnName);
    if (strcmp(m_messageHandlerFns[i].fnName, fnName) == 0)
    {
      result = m_messageHandlerFns[i].fnID;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// NodeInitDataType registration.
//----------------------------------------------------------------------------------------------------------------------
bool Manager::registerNodeInitDataType(
  NodeInitDataType          initDataType,
  NodeInitDataLocateFn      locateFn,
  NodeInitDataDislocateFn   dislocateFn)
{
  NMP_ASSERT(m_numRegisteredNodeInitDatas < MAX_NUM_ATTR_DATA_TYPES);
  m_nodeInitDataRegistry[m_numRegisteredNodeInitDatas].m_type = initDataType;
  m_nodeInitDataRegistry[m_numRegisteredNodeInitDatas].m_locateFn = locateFn;
  m_nodeInitDataRegistry[m_numRegisteredNodeInitDatas].m_dislocateFn = dislocateFn;
  m_numRegisteredNodeInitDatas++;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NodeInitDataLocateFn Manager::getNodeInitDataLocateFn(NodeInitDataType initDataType)
{
  NodeInitDataLocateFn result = NULL;

  for (uint32_t i = 0; i < m_numRegisteredNodeInitDatas; ++i)
  {
    if (m_nodeInitDataRegistry[i].m_type == initDataType)
    {
      result = m_nodeInitDataRegistry[i].m_locateFn;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NodeInitDataDislocateFn Manager::getNodeInitDataDislocateFn(NodeInitDataType initDataType)
{
  NodeInitDataDislocateFn result = NULL;

  for (uint32_t i = 0; i < m_numRegisteredNodeInitDatas; ++i)
  {
    if (m_nodeInitDataRegistry[i].m_type == initDataType)
    {
      result = m_nodeInitDataRegistry[i].m_dislocateFn;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::registerAsset(uint32_t assetType, AssetLocateFn locateFn)
{
  // MORPH-21368: Allowing several assets of the same type (different version, different), array needs to be sized differently
  NMP_ASSERT(m_numRegisteredAssets < kAsset_NumAssetTypes);
  uint32_t entryID = m_numRegisteredAssets + 1;
  m_assetEntries[m_numRegisteredAssets].assetType = assetType;
  m_assetEntries[m_numRegisteredAssets].entryID = entryID;
  m_assetEntries[m_numRegisteredAssets].locateFn = locateFn;
  ++m_numRegisteredAssets;
  return entryID;
}

//----------------------------------------------------------------------------------------------------------------------
AssetLocateFn Manager::getAssetLocateFn(MR::Manager::AssetType assetType)
{
  AssetLocateFn result = NULL;

  for (uint32_t i = 0; i < m_numRegisteredAssets; ++i)
  {
    if (m_assetEntries[i].assetType == (uint32_t)assetType)
    {
      result = m_assetEntries[i].locateFn;
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// PredictionType registration.
//----------------------------------------------------------------------------------------------------------------------
void Manager::registerPredictionModel(
  PredictionType             type,
  const char*                NMP_USED_FOR_ASSERTS(typeName),
  PredictionModelLocateFn    locateFn,
  PredictionModelDislocateFn dislocateFn)
{
  NMP_ASSERT(type < MAX_NUM_PREDICTION_MODEL_TYPES);
  NMP_ASSERT(locateFn && dislocateFn);

  // Make sure this type ID hasn't been registered already.
  NMP_ASSERT(!m_predictionRegistry[type].m_locateFn && !m_predictionRegistry[type].m_dislocateFn);

  m_predictionRegistry[type].m_locateFn = locateFn;
  m_predictionRegistry[type].m_dislocateFn = dislocateFn;
#if defined(NMP_ENABLE_ASSERTS)
  m_predictionRegistry[type].m_predictionModelTypeName = typeName;
#endif

  if ((type + 1) > m_highestRegisteredPredictionType)
    m_highestRegisteredPredictionType = type + 1;
}

//----------------------------------------------------------------------------------------------------------------------
PredictionModelLocateFn Manager::getPredictionModelLocateFn(PredictionType type)
{
  NMP_ASSERT(type < MAX_NUM_PREDICTION_MODEL_TYPES);
  NMP_ASSERT(m_predictionRegistry[type].m_locateFn);

  return m_predictionRegistry[type].m_locateFn;
}

//----------------------------------------------------------------------------------------------------------------------
PredictionModelDislocateFn Manager::getPredictionModelDislocateFn(PredictionType type)
{
  NMP_ASSERT(type < MAX_NUM_PREDICTION_MODEL_TYPES);
  NMP_ASSERT(m_predictionRegistry[type].m_dislocateFn);
  return m_predictionRegistry[type].m_dislocateFn;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataType registration.
//----------------------------------------------------------------------------------------------------------------------
void Manager::registerAttrDataType(
  AttribDataType        attribType,
  const char*           NMP_USED_FOR_ASSERTS(attribTypeName),
  AttribLocateFn        locateFn,
  AttribDislocateFn     dislocateFn,
  AttribOutputMemReqsFn outputMemReqsFn,
  AttribRelocateFn      relocateFn,
  AttribPrepForSpuFn    NMP_UNUSED(prepForSpuFn),
  AttribSerializeTxFn   MR_OUTPUT_DEBUG_ARG(srlzTxFn))
{
  NMP_ASSERT((attribType < INVALID_ATTRIB_TYPE) && (attribType < MAX_NUM_ATTR_DATA_TYPES));
  NMP_ASSERT(!m_attrDataRegistry[attribType].m_locateFn);     // Make sure we are not already registered.
  NMP_ASSERT(!m_attrDataRegistry[attribType].m_dislocateFn);
  NMP_ASSERT(!m_attrDataRegistry[attribType].m_relocateFn);
    
  m_attrDataRegistry[attribType].m_locateFn = locateFn;
  m_attrDataRegistry[attribType].m_dislocateFn = dislocateFn;
  m_attrDataRegistry[attribType].m_outputMemReqsFn = outputMemReqsFn;
  m_attrDataRegistry[attribType].m_relocateFn = relocateFn;

#if defined(MR_OUTPUT_DEBUGGING)
  NMP_ASSERT(!m_attrDataRegistry[attribType].m_serializeTxFn);
  m_attrDataRegistry[attribType].m_serializeTxFn = srlzTxFn;
#endif // MR_OUTPUT_DEBUGGING

#if defined(NMP_ENABLE_ASSERTS)
  NMP_ASSERT(!m_attrDataRegistry[attribType].m_attribTypeName);
  m_attrDataRegistry[attribType].m_attribTypeName = attribTypeName;
#endif

  if (attribType > m_highestRegisteredAttribDataType)
    m_highestRegisteredAttribDataType = attribType;
}

//----------------------------------------------------------------------------------------------------------------------
AttribLocateFn Manager::getAttribLocateFn(AttribDataType attribType)
{
  NMP_ASSERT((attribType < INVALID_ATTRIB_TYPE) && (attribType < MAX_NUM_ATTR_DATA_TYPES));
  return m_attrDataRegistry[attribType].m_locateFn;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDislocateFn Manager::getAttribDislocateFn(AttribDataType attribType)
{
  NMP_ASSERT((attribType < INVALID_ATTRIB_TYPE) && (attribType < MAX_NUM_ATTR_DATA_TYPES));
  return m_attrDataRegistry[attribType].m_dislocateFn;
}

//----------------------------------------------------------------------------------------------------------------------
AttribOutputMemReqsFn Manager::getAttribOutputMemReqsFn(AttribDataType attribType)
{
  NMP_ASSERT((attribType < INVALID_ATTRIB_TYPE) && (attribType < MAX_NUM_ATTR_DATA_TYPES));
  return m_attrDataRegistry[attribType].m_outputMemReqsFn;
}

//----------------------------------------------------------------------------------------------------------------------
AttribRelocateFn Manager::getAttribRelocateFn(AttribDataType attribType)
{
  NMP_ASSERT((attribType < INVALID_ATTRIB_TYPE) && (attribType < MAX_NUM_ATTR_DATA_TYPES));
  return m_attrDataRegistry[attribType].m_relocateFn;
}

//----------------------------------------------------------------------------------------------------------------------
AttribSerializeTxFn Manager::getAttribSerializeTxFn(AttribDataType MR_OUTPUT_DEBUG_ARG(attribType))
{
#if defined(MR_OUTPUT_DEBUGGING)
  NMP_ASSERT((attribType < INVALID_ATTRIB_TYPE) && (attribType < MAX_NUM_ATTR_DATA_TYPES));
  return m_attrDataRegistry[attribType].m_serializeTxFn;
#else  // MR_OUTPUT_DEBUGGING
  return NULL;
#endif // MR_OUTPUT_DEBUGGING
  
}

//----------------------------------------------------------------------------------------------------------------------
const char* Manager::getAttribTypeName(AttribDataType NMP_USED_FOR_ASSERTS(attribType))
{
#if defined(NMP_ENABLE_ASSERTS)
  NMP_ASSERT((attribType < INVALID_ATTRIB_TYPE) && (attribType < MAX_NUM_ATTR_DATA_TYPES));
  return m_attrDataRegistry[attribType].m_attribTypeName;
#else  // NMP_ENABLE_ASSERTS
  return NULL;
#endif // NMP_ENABLE_ASSERTS
}

//----------------------------------------------------------------------------------------------------------------------
// TransitCondition registration.
//----------------------------------------------------------------------------------------------------------------------
bool Manager::registerTransitCondType(
  TransitConditType               conditType,
  TransitCondDefLocateFn          locateFn,
  TransitCondDefDislocateFn       dislocateFn,
  TransitCondInstanceRelocateFn   relocateFn,
  TransitCondInstanceGetMemReqsFn memReqsFn,
  TransitCondInstanceInitFn       initFn,
  TransitCondInstanceUpdateFn     updateFn,
  TransitCondInstanceQueueDepsFn  queueDepsFn,
  TransitCondInstanceResetFn      resetFn)
{
  NMP_ASSERT_MSG(getTransitCondRegistryEntry(conditType) == NULL, "Trying to register a transit condition with "
                 "ID %i, but one is already registered\n", conditType);
  NMP_ASSERT(m_numRegisteredTransitConds < MAX_NUM_TRANSIT_COND_TYPES);

  m_transitCondRegistry[m_numRegisteredTransitConds].m_type = conditType;
  m_transitCondRegistry[m_numRegisteredTransitConds].m_locateFn = locateFn;
  m_transitCondRegistry[m_numRegisteredTransitConds].m_dislocateFn = dislocateFn;
  m_transitCondRegistry[m_numRegisteredTransitConds].m_relocateFn = relocateFn;
  m_transitCondRegistry[m_numRegisteredTransitConds].m_memReqsFn = memReqsFn;
  m_transitCondRegistry[m_numRegisteredTransitConds].m_initFn = initFn;
  m_transitCondRegistry[m_numRegisteredTransitConds].m_updateFn = updateFn;
  m_transitCondRegistry[m_numRegisteredTransitConds].m_queueDepsFn = queueDepsFn;
  m_transitCondRegistry[m_numRegisteredTransitConds].m_resetFn = resetFn;
  m_numRegisteredTransitConds++;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
Manager::TransitCondRegistryEntry* Manager::getTransitCondRegistryEntry(TransitConditType conditType)
{
  TransitCondRegistryEntry* result = NULL;

  for (uint32_t i = 0; i < m_numRegisteredTransitConds; ++i)
  {
    if (m_transitCondRegistry[i].m_type == conditType)
    {
      result = &(m_transitCondRegistry[i]);
      break;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// Functions for registering animation compression formats.
//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::registerAnimationFormat(
  const char*            animationFormatString,
  AnimType               animType,
  LocateAnimFormatFn     locateAnimFormatFn,
  DislocateAnimFormatFn  dislocateAnimFormatFn,
  QueueAttrTaskFn        queueAttrTaskAnimSourceFn,
  QueueAttrTaskFn        queueAttrTaskTrajectorySourceFn,
  QueueAttrTaskFn        queueAttrTaskTrajectoryTransformSourceFn)
{
  NMP_ASSERT(animationFormatString);
  NMP_ASSERT(locateAnimFormatFn);
  NMP_ASSERT(dislocateAnimFormatFn);
  NMP_ASSERT(queueAttrTaskAnimSourceFn);
  NMP_ASSERT(queueAttrTaskTrajectorySourceFn);
  NMP_ASSERT(queueAttrTaskTrajectoryTransformSourceFn);

  // Get the animation format index
  uint32_t id = m_numRegisteredAnimationFormats;
  NMP_ASSERT(id < MAX_NUM_ANIMATION_FORMAT_TYPES);
  m_numRegisteredAnimationFormats++;

  // Set the registry entry data
  AnimationFormatRegistryEntry& entry = m_animationFormatRegistry[id];
  NMP_SPRINTF(entry.m_animationFormatString, ANIM_FORMAT_MAX_STRING_LENGTH, "%s", animationFormatString);
#ifdef NMP_ENABLE_ASSERTS
  for (uint32_t i = 0; i < (m_numRegisteredAnimationFormats - 1); ++i)
  {
    NMP_ASSERT_MSG(
      strcmp(m_animationFormatRegistry[i].m_animationFormatString, entry.m_animationFormatString) != 0,
      "Format already registered: %s", animationFormatString);
  }
#endif
  entry.m_animType = animType;
  entry.m_locateAnimFormatFn = locateAnimFormatFn;
  entry.m_dislocateAnimFormatFn = dislocateAnimFormatFn;
  entry.m_queueAttrTaskAnimSourceFn = queueAttrTaskAnimSourceFn;
  entry.m_queueAttrTaskTrajectorySourceFn = queueAttrTaskTrajectorySourceFn;
  entry.m_queueAttrTaskTrajectoryTransformSourceFn = queueAttrTaskTrajectoryTransformSourceFn;

  return id;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getNumAnimationFormatRegistryEntries()
{
  return m_numRegisteredAnimationFormats;
}

//----------------------------------------------------------------------------------------------------------------------
const Manager::AnimationFormatRegistryEntry* Manager::getAnimationFormatRegistryEntry(uint8_t index)
{
  NMP_ASSERT(index < m_numRegisteredAnimationFormats);
  return &m_animationFormatRegistry[index];
}

//----------------------------------------------------------------------------------------------------------------------
bool Manager::findAnimationFormatRegistryEntryIndex(const char* animationFormatString, uint8_t& index)
{
  NMP_ASSERT_MSG(animationFormatString, "Missing animation format string");
  NMP_ASSERT_MSG(
    m_numRegisteredAnimationFormats > 0,
    "Trying to find anim format %s in the anim registry but no formats have been registered yet",
    animationFormatString);

  for (uint8_t i = 0; i < m_numRegisteredAnimationFormats; ++i)
  {
    if (!strcmp(animationFormatString, m_animationFormatRegistry[i].m_animationFormatString))
    {
      index = i;
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool Manager::findAnimationFormatRegistryEntryIndex(AnimType animType, uint8_t& index)
{
  for (uint8_t i = 0; i < m_numRegisteredAnimationFormats; ++i)
  {
    if (animType == m_animationFormatRegistry[i].m_animType)
    {
      index = i;
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
const Manager::AnimationFormatRegistryEntry* Manager::findAnimationFormatRegistryEntry(const char* animationFormatString)
{
  NMP_ASSERT(animationFormatString);
  uint8_t formatIndex = (uint8_t) -1;
  bool success = findAnimationFormatRegistryEntryIndex(animationFormatString, formatIndex);
  if (success)
  {
    return getAnimationFormatRegistryEntry(formatIndex);
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
const Manager::AnimationFormatRegistryEntry* Manager::findAnimationFormatRegistryEntry(AnimType animType)
{
  uint8_t formatIndex = (uint8_t) -1;
  bool success = findAnimationFormatRegistryEntryIndex(animType, formatIndex);
  if (success)
  {
    return getAnimationFormatRegistryEntry(formatIndex);
  }
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getNumObjectsOfType(AssetType assetType)
{
  uint32_t numObjects = 0;
  ObjectRegistryEntry* prevEntry = &m_objRegistry;
  ObjectRegistryEntry* currentEntry = prevEntry->m_next;

  while (currentEntry != 0)
  {
    if (currentEntry->m_typeID == assetType)
    {
      ++numObjects;
    }

    prevEntry = currentEntry;
    currentEntry = currentEntry->m_next;
  }

  return numObjects;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t Manager::getObjectsOfType(AssetType assetType, ObjectID* buffer)
{
  uint32_t numObjects = 0;

  ObjectRegistryEntry* prevEntry = &m_objRegistry;
  ObjectRegistryEntry* currentEntry = prevEntry->m_next;
  while (currentEntry != 0)
  {
    if (currentEntry->m_typeID == assetType)
    {
      buffer[numObjects] = currentEntry->m_objectID;
      ++numObjects;
    }

    prevEntry = currentEntry;
    currentEntry = currentEntry->m_next;
  }

  return numObjects;
}

//----------------------------------------------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------------------------------------------
MR::AnimSourceBase* Manager::requestAnimation(RuntimeAnimAssetID animAssetID, void* userdata)
{
  NMP_ASSERT(m_requestAnimFn);
  return m_requestAnimFn(animAssetID, userdata);
}

//----------------------------------------------------------------------------------------------------------------------
void Manager::releaseAnimation(RuntimeAnimAssetID animAssetID, MR::AnimSourceBase* loadedAnimation, void* userdata)
{
  NMP_ASSERT(m_releaseAnimFn);
  m_releaseAnimFn(animAssetID, loadedAnimation, userdata);
}

//----------------------------------------------------------------------------------------------------------------------
// Embedded Morpheme Runtime Version With Forced Linkage
//----------------------------------------------------------------------------------------------------------------------
extern const uint32_t MorphemeRuntimeVersion[]
#if (defined(NM_COMPILER_GCC) || defined(NM_COMPILER_SNC)) && !defined(NM_HOST_IOS) && !defined(NM_HOST_64_BIT)
__attribute__ ((used))
#endif
= { MR_VERSION_MAJOR, MR_VERSION_MINOR, MR_VERSION_RELEASE, MR_VERSION_REVISION, MR_VERSION_RUNTIME_BINARY };
#if defined(NM_COMPILER_MSVC)
  #pragma comment(linker, "/include:?MorphemeRuntimeVersion@MR@@3QBIB")
#endif

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
