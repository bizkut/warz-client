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
#include "MyGameCharacter.h"
#include "morpheme/mrBlendOps.h"
#include "morpheme/mrNetwork.h"
#include "NMPlatform/NMFreelistMemoryAllocator.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
MyCharacter* MyCharacter::create(CharacterDef* characterDef)
{
  if (!characterDef)
  {
    NMP_DEBUG_MSG("error: Invalid Character Definition!");
    return NULL;
  }

  //----------------------------
  // Create and initialise an instance of our character
  MyCharacter* const instance = static_cast<MyCharacter*>(NMPMemoryAlloc(sizeof(MyCharacter)));
  new(instance) MyCharacter();

  //----------------------------
  // Initialise Character, set Network Def & Character Controller, allocate memory etc
  instance->initBaseMembers("", characterDef, 0, NULL);

  //----------------------------
  // Create a Character Controller
  NMP_STDOUT("Creating CharacterController");
  CharacterControllerNoPhysics* characterController = CharacterControllerNoPhysics::create(NMP::Vector3::InitZero, NMP::Quat::kIdentity);
  characterController->init(instance->getNetwork());

  //----------------------------
  // Now pass the character controller to the network.
  instance->getNetwork()->setCharacterController(characterController);

  //----------------------
  // Perform an initial update of the network with start time of zero
  instance->runInitialisingUpdateStep();

  NMP_STDOUT("Character created");

  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
void MyCharacter::destroy()
{
#ifdef NMP_MEMORY_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "Maximum temporary memory used: %d bytes\n", m_net->m_peakUsedTempData);
#endif // NMP_MEMORY_LOGGING

  CharacterControllerNoPhysics* characterController = getCharacterController();
  if (characterController)
  {
    CharacterControllerNoPhysics::destroy(characterController);
    m_net->setCharacterController(NULL);
  }

  termBaseMembers();

  // Free our memory (must be the last task performed in this function).
  NMP::Memory::memFree(this);
}

//----------------------------------------------------------------------------------------------------------------------
void MyCharacter::updateToPreCharacterController(float deltaTime)
{
  NMP_ASSERT(m_net);

  //----------------------------
  // Build queue.
  m_net->startUpdate(deltaTime);
  m_executeResult = MR::EXECUTE_RESULT_STARTED;

  //----------------------------
  // Execute the queue until complete or until we have a waiting task of MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER.
  executeNetworkWhileInProgress(MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER);
}

//----------------------------------------------------------------------------------------------------------------------
void MyCharacter::updateFinalise()
{
  NMP_ASSERT(m_net);

  //----------------------------
  // Update the characterController
  CharacterControllerNoPhysics* characterController = static_cast<CharacterControllerNoPhysics*>(m_net->getCharacterController());
  NMP_ASSERT(characterController);

  if (m_useOverrideDeltaTransform)
    characterController->updateControllerMove(&m_overrideDeltaTranslation, &m_overrideDeltaOrientation);
  else
    characterController->updateControllerMove(NULL, NULL);

  //----------------------------
  // Complete the execution of queue.
  executeNetworkWhileInProgress(MR::TASK_ID_UNSPECIFIED, MR::EXECUTE_RESULT_COMPLETE);

  // Finalize post network update
  m_net->endUpdate();

  //----------------------------
  // Reset the temporary allocator
  // In order to track peak temporary usage, check the value returned from FastHeapAllocator::reset() and update the
  // peak appropriately. Network::update() also contains code to do this if using the default 2MB temporary memory
  // allocators.
#ifdef NMP_MEMORY_LOGGING
  uint32_t lastUsedBytes = m_net->getTempMemoryAllocator()->reset();

  if (lastUsedBytes > m_net->m_peakUsedTempData)
  {
    m_net->m_peakUsedTempData = lastUsedBytes;
  }
#else  
  m_net->getTempMemoryAllocator()->reset();
#endif

  //----------------------------
  // Generate the final world transforms now that Network execution is complete.
  updateWorldTransforms(m_net->getCharacterPropertiesWorldRootTransform());

#ifdef THIS_IS_WHERE_YOU_WOULD_MAP_THE_OUTPUT_TO_YOUR_GAME_CHARACTERS_SKELETON
  const MR::AnimRigDef* animRig = NULL;
  const NMP::DataBuffer* const pose = getNetwork()->getPose(animRig);
#endif // THIS_IS_WHERE_YOU_WOULD_MAP_THE_OUTPUT_TO_YOUR_GAME_CHARACTERS_SKELETON
}

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
