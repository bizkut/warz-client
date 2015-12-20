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
#include "GameManagement/PhysX2/GameCharacterPhysx2.h"

#include "physics/mrPhysics.h"
#include "physics/mrPhysicsRig.h"
#include "NMPlatform/NMFastHeapAllocator.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
CharacterPhysX2* CharacterPhysX2::create(
  CharacterDef*             characterDef,
  MR::PhysicsScenePhysX2*   physicsScene,
  NxControllerManager*      controllerManager,
  const NMP::Vector3&       initialPosition,
  const NMP::Quat&          initialOrientation,
  const NMP::Vector3&       graveYardPosition,
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
  // Create and initialise an instance of PhysX2Character
  CharacterPhysX2* const instance = static_cast<CharacterPhysX2*>(NMPMemoryAlloc(sizeof(CharacterPhysX2)));
  new(instance) CharacterPhysX2();
  instance->initBaseMembers(name, characterDef, initialAnimSetIndex, temporaryMemoryAllocator, externalIdentifier);
  
  //----------------------------
  // Now create and initialise a PhysX2 specific character controller, pass it to our network and vice versa
  CharacterControllerPhysX2* const physx2CharacterController = CharacterControllerPhysX2::create(
                                                                instance->getNetwork(),
                                                                initialPosition,
                                                                initialOrientation,
                                                                graveYardPosition,
                                                                physicsScene,
                                                                controllerManager);
  instance->getNetwork()->setCharacterController(physx2CharacterController);

  // Run the initialising update of this character.
  //  Initialises the starting state of the Network by updating it with an absolute time of zero.
  instance->runInitialisingUpdateStep();

  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX2::destroy()
{
  CharacterControllerPhysX2* characterController = getCharacterController();
  if (characterController)
  {
    CharacterControllerPhysX2::destroy(characterController);
    setCharacterController(NULL);
  }

  termBaseMembers();
  
  // Free our memory (must be the last task performed in this function).
  NMP::Memory::memFree(this);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX2::updateToPreCharacterController(float deltaTime)
{   
  // Build queue.
  NMP_ASSERT(m_net);
  m_net->startUpdate(deltaTime);
  m_executeResult = MR::EXECUTE_RESULT_STARTED;

  // Execute the queue until complete or until we have a waiting task of MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER.
  executeNetworkWhileInProgress(MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER);  
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX2::updateCharacterControllerMove(float deltaTime)
{
  CharacterControllerPhysX2* characterController = getCharacterController();
  NMP_ASSERT(characterController);
  
  // If the root is under animation control update the character controller delta transform.
  if (m_useOverrideDeltaTransform)
    characterController->updateControllerMove(deltaTime, &m_overrideDeltaTranslation, &m_overrideDeltaOrientation);
  else
    characterController->updateControllerMove(deltaTime, NULL, NULL);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX2::updateControllerMoveAdjustingStepping()
{
  CharacterControllerPhysX2* characterController = getCharacterController();
  NMP_ASSERT(characterController);
  characterController->updateControllerMoveAdjustingStepping();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX2::updatePostCharacterController(float deltaTime)
{
  NMP_ASSERT(m_net);

  CharacterControllerPhysX2* characterController = getCharacterController();
  NMP_ASSERT(characterController);
  
  // If the Network root position is being controlled by animation update our cached transforms 
  //  from the physics systems character controller transform.
  characterController->updateCachedTransformsFromCharacterControllerPosition(deltaTime);
  
  // Execute the queue until complete or until we have a waiting task of MR_TASKID_NETWORKUPDATEPHYSICS.
  executeNetworkWhileInProgress(MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEPHYSICS);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX2::updatePrePhysics()
{
  // Write our cached PhysX channel data to our rig in preparation of the physics step.
  writePhysicsRigToPhysX();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX2::updatePostPhysics()
{
  // Re-generate our physics cache data from the PhysX rig after the physics step which should have just taken place.
  readPhysicsRigFromPhysX();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX2::updateFinalise(float deltaTime)
{
  // Execute the queue until complete or until we have a waiting task of MR_TASKID_NETWORKUPDATEROOT.
  executeNetworkWhileInProgress(MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATEROOT);

  // Update character controller.
  CharacterControllerPhysX2* physXCharacterController = getCharacterController(); 
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
void CharacterPhysX2::writePhysicsRigToPhysX()
{
  MR::PhysicsRig* physicsRig = MR::getPhysicsRig(m_net);
  if (physicsRig)
  {
    physicsRig->updatePrePhysics(physicsRig->getPhysicsScene()->getNextPhysicsTimeStep());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterPhysX2::readPhysicsRigFromPhysX()
{
  MR::PhysicsRig* physicsRig = MR::getPhysicsRig(m_net);
  if (physicsRig)
  {
    physicsRig->updatePostPhysics(physicsRig->getPhysicsScene()->getNextPhysicsTimeStep());
  }
}

} // namespace game

//----------------------------------------------------------------------------------------------------------------------