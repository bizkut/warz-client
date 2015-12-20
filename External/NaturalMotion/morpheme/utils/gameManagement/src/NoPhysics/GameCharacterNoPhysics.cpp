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
#include "morpheme/mrBlendOps.h"

#include "GameManagement/NoPhysics/GameCharacterNoPhysics.h"
#include "GameManagement/GameCharacterDef.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
CharacterNoPhysics* CharacterNoPhysics::create(
  CharacterDef*             characterDef,
  const NMP::Vector3&       initialPosition,
  const NMP::Quat&          initialOrientation,
  MR::AnimSetIndex          initialAnimSetIndex,
  const char*               name,
  NMP::TempMemoryAllocator* temporaryMemoryAllocator,
  uint32_t                  externalIdentifier)
{
  if (!name || !characterDef)
  {
    NMP_DEBUG_MSG("error: Valid name and network definition expected!");
    return NULL;
  }

  //----------------------------
  // Create and initialise an instance of our character
  CharacterNoPhysics* const instance = static_cast<CharacterNoPhysics*>(NMPMemoryAlloc(sizeof(CharacterNoPhysics)));
  new(instance) CharacterNoPhysics();

  instance->initBaseMembers(name, characterDef, initialAnimSetIndex, temporaryMemoryAllocator, externalIdentifier);

  //----------------------------
  // Now create a animation character controller, pass it to our network and vice versa
  CharacterControllerNoPhysics* const characterController = CharacterControllerNoPhysics::create(initialPosition, initialOrientation);
  characterController->init(instance->getNetwork());
  instance->getNetwork()->setCharacterController(characterController);

  //----------------------------
  // Run an initialising update on the network to get it into a state where it can be updated correctly.  
  instance->runInitialisingUpdateStep();

  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterNoPhysics::destroy()
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
void CharacterNoPhysics::updateToPreCharacterController(float deltaTime)
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
void CharacterNoPhysics::updateFinalise(
  const NMP::Vector3* deltaTranslation,
  const NMP::Quat*    deltaOrientation)
{
  NMP_ASSERT(m_net);

  //----------------------------
  // Update the characterController
  CharacterControllerNoPhysics* characterController = static_cast<CharacterControllerNoPhysics*>(m_net->getCharacterController());
  NMP_ASSERT(characterController);

  if (m_useOverrideDeltaTransform)
    characterController->updateControllerMove(deltaTranslation, deltaOrientation);
  else
    characterController->updateControllerMove(NULL, NULL);
  
  //----------------------------
  // Complete the execution of queue.
  executeNetworkWhileInProgress(MR::TASK_ID_UNSPECIFIED, MR::EXECUTE_RESULT_COMPLETE);

  // Finalize post network update
  getNetwork()->endUpdate();

  //----------------------------
  // Only reset the temporary allocator if it was created by us.
  // Note: if it was provided to us then the provider is responsible for resetting it.
  if (m_internalTemporaryMemoryAllocator)
  {
    m_net->getTempMemoryAllocator()->reset();
  }

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
