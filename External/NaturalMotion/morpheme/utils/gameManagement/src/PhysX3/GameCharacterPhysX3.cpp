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
#include "GameManagement/PhysX3/GameCharacterPhysX3.h"

#include "physics/mrPhysics.h"
#include "physics/mrPhysicsRig.h"
#include "NMPlatform/NMFastHeapAllocator.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
CharacterPhysX3* CharacterPhysX3::create(
  CharacterDef*               characterDef,
  MR::PhysicsScenePhysX3*     physicsScene,
  physx::PxControllerManager* controllerManager,
  physx::PxMaterial*          characterControllerMaterial,
  MR::PhysicsRigPhysX3::Type  physicsRigType,
  const NMP::Vector3&         initialPosition,
  const NMP::Quat&            initialOrientation,
  MR::AnimSetIndex            initialAnimSetIndex,
  const char*                 name,
  NMP::TempMemoryAllocator*   temporaryMemoryAllocator,
  uint32_t                    externalIdentifier)
{
  if (!name || !characterDef)
  {
    NMP_DEBUG_MSG("error: Valid name and network definition expected!");
    return NULL;
  }

  // Create and initialise an instance of PhysX3Character
  CharacterPhysX3* const instance = static_cast<CharacterPhysX3*>(NMPMemoryAlloc(sizeof(CharacterPhysX3)));
  new(instance) CharacterPhysX3();
  instance->initBaseMembers(name, characterDef, initialAnimSetIndex, temporaryMemoryAllocator, externalIdentifier);
  
  // Now create and initialise a PhysX3 specific character controller, pass it to our network and vice versa
  CharacterControllerPhysX3* const physx3CharacterController = CharacterControllerPhysX3::create(
                                                                instance->getNetwork(),
                                                                initialPosition,
                                                                initialOrientation,
                                                                physicsScene,
                                                                controllerManager,
                                                                characterControllerMaterial,
                                                                physicsRigType);
  instance->getNetwork()->setCharacterController(physx3CharacterController);

  // Run the initialising update of this character.
  //  Initialises the starting state of the Network by updating it with an absolute time of zero.
  instance->runInitialisingUpdateStep();

  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX3::destroy()
{
  CharacterControllerPhysX3* characterController = getCharacterController();
  if (characterController)
  {
    CharacterControllerPhysX3::destroy(characterController);
    setCharacterController(NULL);
  }

  termBaseMembers();
  
  // Free our memory (must be the last task performed in this function).
  NMP::Memory::memFree(this);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX3::updateToPreCharacterController(float deltaTime)
{   
  // Build queue.
  NMP_ASSERT(m_net);
  m_net->startUpdate(deltaTime);
  m_executeResult = MR::EXECUTE_RESULT_STARTED;

  // Execute the queue until complete or until we have a waiting task of MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER.
  executeNetworkWhileInProgress(MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER);  
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX3::updateCharacterControllerMove(
  float deltaTime)
{
  CharacterControllerPhysX3* characterController = getCharacterController();
  NMP_ASSERT(characterController);

  if (m_useOverrideDeltaTransform)
    characterController->updateControllerMove(deltaTime, &m_overrideDeltaTranslation, &m_overrideDeltaOrientation);
  else
    characterController->updateControllerMove(deltaTime, NULL, NULL);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX3::updateControllerMoveAdjustingStepping(float deltaTime)
{
  CharacterControllerPhysX3* characterController = getCharacterController();
  NMP_ASSERT(characterController);
  characterController->updateControllerMoveAdjustingStepping(deltaTime);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX3::updatePostCharacterController()
{
  NMP_ASSERT(m_net);

  CharacterControllerPhysX3* characterController = getCharacterController();
  NMP_ASSERT(characterController);
  
  // If the Network root position is being controlled by animation update our cached transforms 
  //  from the physics systems character controller transform.
  characterController->updateCachedTransformsFromCharacterControllerPosition();
  
  // Execute the queue until complete or until we have a waiting task of MR_TASKID_NETWORKUPDATEPHYSICS.
  executeNetworkWhileInProgress(MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEPHYSICS);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX3::updatePrePhysics()
{
  // Write our cached PhysX channel data to our rig in preparation of the physics step.
  writePhysicsRigToPhysX();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX3::updatePostPhysics()
{
  // Re-generate our physics cache data from the PhysX rig after the physics step which should have just taken place.
  readPhysicsRigFromPhysX();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX3::updateFinalise(float deltaTime)
{
  // Execute the queue until complete or until we have a waiting task of MR_TASKID_NETWORKUPDATEROOT.
  executeNetworkWhileInProgress(MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEROOT);

  // Update character controller.
  CharacterControllerPhysX3* physXCharacterController = getCharacterController(); 
  physXCharacterController->updateCachedTransformsFromPhysicsRigPosition(deltaTime);
   
  // Complete the execution of queue.
  executeNetworkWhileInProgress(MR::TASK_ID_UNSPECIFIED, MR::EXECUTE_RESULT_COMPLETE);

  // Finalize post network update
  getNetwork()->endUpdate();

  // Only reset the temporary allocator if it was created by us.
  // Note: if it was provided to us then the provider is responsible for resetting it.
  if (m_internalTemporaryMemoryAllocator)
  {
    getNetwork()->getTempMemoryAllocator()->reset();
  }

  // Generate the final world transforms now that Network execution is complete.
  updateWorldTransforms(m_net->getCharacterPropertiesWorldRootTransform());
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX3::writePhysicsRigToPhysX()
{
  MR::PhysicsRig* physicsRig = MR::getPhysicsRig(m_net);
  if (physicsRig)
  {
    physicsRig->updatePrePhysics(physicsRig->getPhysicsScene()->getNextPhysicsTimeStep());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX3::readPhysicsRigFromPhysX()
{
  MR::PhysicsRig* physicsRig = MR::getPhysicsRig(m_net);
  if (physicsRig)
  {
    physicsRig->updatePostPhysics(physicsRig->getPhysicsScene()->getNextPhysicsTimeStep());
  }
}

} // namespace game

//----------------------------------------------------------------------------------------------------------------------