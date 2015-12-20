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
#include "MyGameCharacter.h"

#include "NMPlatform/NMFreelistMemoryAllocator.h"
#include "GameManagement/GameCharacterManager.h"
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
void MyCharacter::updateFinalise(
  const NMP::Vector3* deltaTranslation,
  const NMP::Quat*    deltaOrientation)
{
  NMP_ASSERT(m_net);

  //----------------------------
  // Update the characterController
  CharacterControllerNoPhysics* characterController = static_cast<CharacterControllerNoPhysics*>(m_net->getCharacterController());
  NMP_ASSERT(characterController);

  characterController->updateControllerMove(deltaTranslation, deltaOrientation);

  //----------------------------
  // Complete the execution of queue.
  executeNetworkWhileInProgress(MR::TASK_ID_UNSPECIFIED, MR::EXECUTE_RESULT_COMPLETE);

  // Finalize post network update
  m_net->endUpdate();

  //----------------------------
  // Only reset the temporary allocator if it was created by us.
  // Note: if it was provided to us then the provider is responsible for resetting it.
  m_net->getTempMemoryAllocator()->reset();

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
