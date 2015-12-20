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
#include "NMPlatform/NMFreelistMemoryAllocator.h"
#include "NMPlatform/NMLoggingMemoryAllocator.h"
#include "morpheme/mrBlendOps.h"

#include "GameManagement/GameAnimModule.h"
#include "GameManagement/GameCharacter.h"
#include "GameManagement/GameCharacterDef.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
void Character::setDeltaTransformOverride(
  NMP::Vector3& overrideDeltaTranslation,
  NMP::Quat&    overrideDeltaOrientation)
{
  m_overrideDeltaTranslation = overrideDeltaTranslation;
  m_overrideDeltaOrientation = overrideDeltaOrientation;
}

//----------------------------------------------------------------------------------------------------------------------
void Character::updateWorldTransforms(const NMP::Matrix34& rootTransform)
{
  NMP::DataBuffer* const transforms = m_net->getTransforms();
  MR::AnimRigDef* const animRig = m_net->getActiveRig();
  NMP_ASSERT(transforms && animRig);

  // if sourceBuffer may not be full, use applyBindPoseToUnusedChannels before calling accumulateTransforms
  // alternately, accumulateTransformsPartial can be used
  MR::BlendOpsBase::applyBindPoseToUnusedChannels(animRig->getBindPose(), transforms);
  MR::BlendOpsBase::accumulateTransforms(rootTransform.translation(), rootTransform.toQuat(), transforms, animRig, m_worldTransforms);
  m_animRigDef = animRig;
}

//----------------------------------------------------------------------------------------------------------------------
void Character::runInitialisingUpdateStep()
{
  MR::Task* task;
  MR::ExecuteResult execResult;

  m_net->startUpdate(0.0f, true);

  while ((execResult = m_net->update(task)) != MR::EXECUTE_RESULT_COMPLETE)
  {
    if (execResult != MR::EXECUTE_RESULT_IN_PROGRESS)
    {
      NMP_ASSERT(
        task->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER ||
        task->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEPHYSICS ||
        task->m_taskid == MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEROOT);

      (void)task;
    }
  }

  m_net->getTempMemoryAllocator()->reset();
}

//----------------------------------------------------------------------------------------------------------------------
bool Character::broadcastRequestMessage(MR::MessageID messageID, bool status)
{
  MR::Message requestMessage(messageID, MESSAGE_TYPE_REQUEST, status, 0, 0);
  return m_net->broadcastMessage(requestMessage) > 0;
}

//----------------------------------------------------------------------------------------------------------------------
void Character::setFloatControlParameter(MR::NodeID cpNodeID, float value)
{
  MR::AttribDataFloat floatAttribData;
  floatAttribData.m_value = value;
  m_net->setControlParameter(cpNodeID, &floatAttribData);
}

//----------------------------------------------------------------------------------------------------------------------
bool Character::initBaseMembers(
  const char*               name,
  CharacterDef*             characterDef,
  MR::AnimSetIndex          initialAnimSetIndex,
  NMP::TempMemoryAllocator* temporaryMemoryAllocator,
  uint32_t                  externalIdentifier)
{
  NMP_ASSERT(!m_name);
  m_name = new std::string;
  *m_name = name;
  m_characterDef = characterDef;
  NMP_ASSERT(m_characterDef);

  characterDef->incRefCount();

  MR::NetworkDef *netDef = m_characterDef->getNetworkDef();

  //----------------------------
  // Create a dispatcher.
#if defined(NM_HOST_CELL_PPU) && defined(NM_USE_SPU_DISPATCHER_ON_PS3)
  MR::DispatcherPS3* const dispatcher = MR::DispatcherPS3::createAndInit();
#else
  MR::DispatcherBasic* const dispatcher = MR::DispatcherBasic::createAndInit();
#endif // NM_HOST_CELL_PPU
  if (!dispatcher)
  {
    NMP_DEBUG_MSG("error: Failed to create dispatcher!");
    return false;
  }
 
  //----------------------------
  // Initialise the temporary memory allocator, if one wasn't passed in for us to use.
  initTemporaryMemoryAllocator(temporaryMemoryAllocator);
  
  //----------------------------
  // Initialise the persistent memory allocator. This is always our own.
  initPersistentMemoryAllocator();

  //----------------------------
  // Create the network instance.
  m_net = MR::Network::createAndInit(
                          netDef,
                          dispatcher,
                          m_temporaryMemoryAllocator,
                          m_persistentMemoryAllocator,
                          NULL);
  if (!m_net)
  {
    dispatcher->releaseAndDestroy();
    NMP_DEBUG_MSG("error: Failed to create network instance!");
    return false;
  }

  //----------------------
  // Set the proper initial animation set.
  m_net->setActiveAnimSetIndex(initialAnimSetIndex);

  //----------------------------
  // Setup the required network outputs.
  //
  // A morpheme update step only evaluates the semantics requested on the root node. We request a transform buffer, the
  // trajectory change and event buffers but one could choose to only evaluate the trajectory for example if a character
  // is off-screen or only the transforms if the trajectory is controller by a server.
  const MR::NodeID rootNodeId = netDef->getRootNodeID();
  const uint16_t minLifespan = 1;
  m_net->addPostUpdateAccessAttrib(rootNodeId, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, minLifespan);
  m_net->addPostUpdateAccessAttrib(rootNodeId, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, minLifespan);
  m_net->addPostUpdateAccessAttrib(rootNodeId, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, minLifespan);
  m_net->addPostUpdateAccessAttrib(rootNodeId, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, minLifespan);

  //----------------------------
  // Allocate a pose buffer.
  //
  // This buffer is used to store the final output of a network update for later use by other components of the game.
  // The buffer size is chosen to be able to hold any possible network output (the number of bones output in each frame
  // may change with the active animation set).
  const uint32_t maxBoneCount = netDef->getMaxBoneCount();
  NMP::Memory::Format bufferFormat = NMP::DataBuffer::getPosQuatMemoryRequirements(maxBoneCount);
  NMP::Memory::Resource bufferResource = NMPMemoryAllocateFromFormat(bufferFormat);
  NMP_ASSERT(bufferResource.ptr);

  m_worldTransforms = NMP::DataBuffer::initPosQuat(bufferResource, bufferFormat, maxBoneCount);

  m_externalIdentifier = externalIdentifier;

  // By default use the Networks internally calculated and stored delta transform.
  m_useOverrideDeltaTransform = false;
  m_overrideDeltaTranslation = NMP::Vector3::InitZero;
  m_overrideDeltaOrientation = NMP::Quat::kIdentity;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void Character::termBaseMembers()
{
  if (m_name)
  {
    delete m_name;
    m_name = NULL;
  }
  
  NMP::Memory::memFree(m_worldTransforms);

  if (m_net)
  {
    m_net->getDispatcher()->releaseAndDestroy();
    m_net->releaseAndDestroy();
    m_net = NULL;
  }

  termTemporaryMemoryAllocator();

  if (m_persistentMemoryAllocator)
  {
    m_persistentMemoryAllocator->term();
    NMP::Memory::memFree(m_persistentMemoryAllocator);
    m_persistentMemoryAllocator = NULL;
  }

  m_characterDef->decRefCount();
}

//----------------------------------------------------------------------------------------------------------------------
void Character::initTemporaryMemoryAllocator(
  NMP::TempMemoryAllocator* temporaryMemoryAllocator)
{
  m_temporaryMemoryAllocator = temporaryMemoryAllocator;
  m_internalTemporaryMemoryAllocator = false;
  if (!m_temporaryMemoryAllocator)
  {
    NMP::Memory::Format allocatorFormat = NMP::FastHeapAllocator::getMemoryRequirements(
                                                    DEFAULT_TEMP_ALLOCATOR_SIZE,
                                                    NMP_VECTOR_ALIGNMENT);
    NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(allocatorFormat);
    m_temporaryMemoryAllocator = NMP::FastHeapAllocator::init(
                                                    resource,
                                                    DEFAULT_TEMP_ALLOCATOR_SIZE,
                                                    NMP_VECTOR_ALIGNMENT);
    m_internalTemporaryMemoryAllocator = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Character::termTemporaryMemoryAllocator()
{
  if (m_temporaryMemoryAllocator && m_internalTemporaryMemoryAllocator)
  {
    m_temporaryMemoryAllocator->term();
    NMP::Memory::memFree(m_temporaryMemoryAllocator);
    m_temporaryMemoryAllocator = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Character::initPersistentMemoryAllocator()
{
  NMP::Memory::Format allocatorFormat;
  NMP::Memory::Resource resource;

#ifdef NMP_MEMORY_LOGGING
  allocatorFormat = NMP::LoggingMemoryAllocator::getMemoryRequirements();
  resource = NMPMemoryAllocateFromFormat(allocatorFormat);
  NMP_ASSERT(resource.ptr);

  m_persistentMemoryAllocator = NMP::LoggingMemoryAllocator::init(resource);
#else
  allocatorFormat = NMP::FreelistMemoryAllocator::getMemoryRequirements();
  resource = NMPMemoryAllocateFromFormat(allocatorFormat);
  NMP_ASSERT(resource.ptr);

  m_persistentMemoryAllocator = NMP::FreelistMemoryAllocator::init(resource);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
MR::Task* Character::executeNetworkWhileInProgress(
  MR::TaskID        NMP_USED_FOR_ASSERTS(requiredWaitingTaskID),
  MR::ExecuteResult NMP_USED_FOR_ASSERTS(requiredExecuteResult))
{
  MR::Task*         task = NULL;

  if(m_executeResult == MR::EXECUTE_RESULT_COMPLETE)
  {
    return task;
  }

  do
  {
    //----------------------------
    // Network::update() will return EXECUTE_RESULT_COMPLETE if the task is
    // flagged as external. In this case the task pointer reference will contain
    // the external task. If the task is not external when Network::update()
    // returns EXECUTE_RESULT_COMPLETE it will be NULL.
    // See the following section of the user guide for more details:
    // Understanding Morpheme > Networks > Runtime handling > Tasks > External tasks
    m_executeResult = m_net->update(task);
  } while (m_executeResult == MR::EXECUTE_RESULT_IN_PROGRESS);

  //----------------------------
  // If on completion there is a waiting task then it must be of ID requiredWaitingTaskID (unless requiredWaitingTaskID == TASK_ID_UNSPECIFIED).
  NMP_ASSERT((task == NULL) || (requiredWaitingTaskID == MR::TASK_ID_UNSPECIFIED) ? true : task->m_taskid == requiredWaitingTaskID);

  //----------------------------
  // If the requiredWaitingTaskID is TASK_ID_UNSPECIFIED then there must be no resulting external task.
  NMP_ASSERT(requiredWaitingTaskID == MR::TASK_ID_UNSPECIFIED ? task == NULL : true)

  //----------------------------
  // The execution result must be of ID requiredExecuteResult (unless requiredExecuteResult == EXECUTE_RESULT_INVALID).
  NMP_ASSERT(requiredExecuteResult == MR::EXECUTE_RESULT_INVALID ? true : m_executeResult == requiredExecuteResult);

  return task;
}

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
