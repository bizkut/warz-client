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
#include "GameManagement/Euphoria/GameCharacterEuphoria.h"

#include "physics/mrPhysics.h"
#include "physics/PhysX3/mrPhysicsRigPhysX3Articulation.h"
#include "NMPlatform/NMFastHeapAllocator.h"

#include "euphoria/erCharacter.h"
#include "euphoria/erAttribData.h"
#include "euphoria/erModule.h"
#include "euphoria/erDebugControls.h"
#include "euphoria/erNetworkInterface.h"
#include "euphoria/erCharacterDef.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
void CharacterEuphoria::initDebugInterfaceModuleNamesAndControls()
{
  // consts used here for array sizes
  const uint32_t g_maxModules = 1024;
  const uint32_t g_maxModuleNameLength = 256;
  const uint32_t kMaxControls = 1024;

  const ER::Character* character = ER::networkGetCharacter(getNetwork());
  if (!character)
  {
    NMP_DEBUG_MSG("error: character is required");
    return;
  }
  MR::Dispatcher* dispatcher = getNetwork()->getDispatcher();
  if (!dispatcher)
  {
    NMP_DEBUG_MSG("error: dispatcher is required");
    return;
  }
  MR::InstanceDebugInterface* debugInterface = dispatcher->getDebugInterface();
  if (!debugInterface)
  {
    NMP_DEBUG_MSG("error: debug interface is required");
    return;
  }

  // Initialise the module names
  char allModuleNames[g_maxModules][g_maxModuleNameLength];
  char allModuleParentNames[g_maxModules][g_maxModuleNameLength];
  char* moduleNames[g_maxModules];
  char* moduleParentNames[g_maxModules];
  for (uint32_t i = 0; i < g_maxModules; ++i)
  {
    moduleNames[i] = (char*) & (allModuleNames[i]);
    moduleParentNames[i] = (char*) & (allModuleParentNames[i]);
  }

  int32_t numModules = 0;
  if (character->m_euphoriaRootModule)
  {
    character->m_euphoriaRootModule->getModuleNames(
      &allModuleNames[0][0], &allModuleParentNames[0][0], g_maxModuleNameLength, numModules);
  }

  NMP_ASSERT((uint32_t)numModules <= g_maxModules);
  debugInterface->initModuleNames(numModules, moduleNames, moduleParentNames);

  // Initialise the controls
  uint32_t totalControls = ER::getEuphoriaDebugControlCount();
  uint32_t totalControlNames = 0;
  const char* controlNames[kMaxControls];

  for (uint32_t i = 0 ; i < totalControls ; i++)
  {
    controlNames[totalControlNames] = ER::getEuphoriaDebugControlName(i);
    if (controlNames[totalControlNames] != NULL)
      totalControlNames++;
  }

  NMP_ASSERT((uint32_t)totalControlNames <= kMaxControls);
  debugInterface->initControlNames(totalControlNames, controlNames);
}

//----------------------------------------------------------------------------------------------------------------------
CharacterEuphoria* CharacterEuphoria::create(
  CharacterDefEuphoria*       characterDefEuphoria,
  MR::PhysicsScenePhysX3*     physicsScene,
  physx::PxControllerManager* controllerManager,
  physx::PxMaterial*          characterControllerMaterial,
  MR::PhysicsRigPhysX3::Type  physicsRigType,
  const NMP::Vector3&         initialPosition,
  const NMP::Quat&            initialOrientation,
  MR::AnimSetIndex            initialAnimSetIndex,
  const char*                 name,
  NMP::TempMemoryAllocator*   temporaryMemoryAllocator)
{
  if (!name || !characterDefEuphoria)
  {
    NMP_DEBUG_MSG("error: Valid name and network definition expected!");
    return NULL;
  }

  // Create and initialise an instance of PhysX3Character
  CharacterEuphoria* const instance = static_cast<CharacterEuphoria*>(NMPMemoryAlloc(sizeof(CharacterEuphoria)));
  new(instance) CharacterEuphoria();
  instance->initBaseMembers(name, characterDefEuphoria, initialAnimSetIndex, temporaryMemoryAllocator);
  
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

  MR::PhysicsRigPhysX3Articulation* physicsRig = (MR::PhysicsRigPhysX3Articulation*) MR::getPhysicsRig(instance->getNetwork());

  instance->m_euphoriaCharacter = 0;

  // Set up ER::Character etc
  if (physicsRig)
  {
    MR::AnimRigDef* rig = instance->getNetwork()->getActiveRig();

    //physicsRig->setRigAndAnimToPhysicsMap(
    //  rig, 
    //  getAnimToPhysicsMap(getNetwork()->getNetworkDef(), 
    //  getNetwork()->getActiveAnimSetIndex()));

    MR::NetworkDef* netDef = instance->getNetwork()->getNetworkDef();

    // Check if the network contains behaviours.
    bool isBehavioural = netDef->containsNodeWithFlagsSet(MR::NodeDef::NODE_FLAG_IS_BEHAVIOURAL);

    if (isBehavioural && physicsRig->getType() == MR::PhysicsRigPhysX3::TYPE_ARTICULATED)
    {
      uint32_t ignoreGroups = (1 << MR::GROUP_CHARACTER_CONTROLLER) | (1 << MR::GROUP_NON_COLLIDABLE);

      ER::NetworkInterface* behaviourLibrary = characterDefEuphoria->getEuphoriaNetworkInterface();
      ER::CharacterDef* euphoriaCharacterDef = characterDefEuphoria->getEuphoriaCharacterDef();

      // Create the network using the network description system.
      ER::RootModule* behaviourNetwork = behaviourLibrary->createNetwork();

      MR::Dispatcher* dispatcher = instance->getNetwork()->getDispatcher();
      MR::InstanceDebugInterface* debugInterface = dispatcher->getDebugInterface();

      instance->m_euphoriaCharacter = euphoriaCharacterDef->createInstance(
                                                            rig,
                                                            (MR::PhysicsRigPhysX3Articulation*)physicsRig,
                                                            behaviourNetwork,
                                                            ignoreGroups,
                                                            true,
                                                            debugInterface);

      ER::networkSetCharacter(instance->getNetwork(), instance->m_euphoriaCharacter);
    }
  }

  // Run the initialising update of this character.
  //  Initialises the starting state of the Network by updating it with an absolute time of zero.
  instance->runInitialisingUpdateStep();

  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterEuphoria::destroy()
{
  ER::networkGetCharacter(getNetwork());
  if (m_euphoriaCharacter)
  {
    ER::RootModule* netToDelete = m_euphoriaCharacter->m_euphoriaRootModule;
    m_euphoriaCharacter->m_definition->destroyInstance(m_euphoriaCharacter);
    ER::networkSetCharacter(getNetwork(), 0);
    ((CharacterDefEuphoria*)m_characterDef)->getEuphoriaNetworkInterface()->destroyNetwork((ER::Module**)&netToDelete);
  }

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
void CharacterEuphoria::updateToPreCharacterController(float deltaTime)
{   
  // Build queue.
  NMP_ASSERT(m_net);
  m_net->startUpdate(deltaTime);
  m_executeResult = MR::EXECUTE_RESULT_STARTED;

  // Execute the queue until complete or until we have a waiting task of MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER.
  executeNetworkWhileInProgress(MR::CoreTaskIDs::MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER);  
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterEuphoria::updateCharacterControllerMove(
  float               deltaTime,
  const NMP::Vector3* requestedDeltaWorldTranslation,
  const NMP::Quat*    requestedDeltaWorldOrientation)
{
  CharacterControllerPhysX3* characterController = getCharacterController();
  NMP_ASSERT(characterController);

  characterController->updateControllerMove(deltaTime, requestedDeltaWorldTranslation, requestedDeltaWorldOrientation);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterEuphoria::updateControllerMoveAdjustingStepping(float deltaTime)
{
  CharacterControllerPhysX3* characterController = getCharacterController();
  NMP_ASSERT(characterController);
  characterController->updateControllerMoveAdjustingStepping(deltaTime);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterEuphoria::updatePostCharacterController(float NMP_UNUSED(deltaTime))
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
void CharacterEuphoria::updatePrePhysics(float deltaTime)
{
  // Only update Euphoria if the physics rig exists and is referenced - if it exists and is not
  // referenced it might only have been added to the network after update connections, in which case
  // it wasn't initialised.
  MR::PhysicsRig* physicsRig = getPhysicsRig(getNetwork());
  if (m_euphoriaCharacter)
  {
    bool usePhysicsRig = physicsRig && physicsRig->isReferenced();
    if (getNetwork()->areBehaviourNodesActive() && usePhysicsRig)
    {
      m_euphoriaCharacter->prePhysicsStep(deltaTime);
    }
    else
    {
      m_euphoriaCharacter->disableBehaviourEffects();
    }
  }

  // Write our cached PhysX channel data to our rig in preparation of the physics step.
  if (physicsRig)
  {
    physicsRig->updatePrePhysics(physicsRig->getPhysicsScene()->getNextPhysicsTimeStep());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterEuphoria::updatePostPhysics(float deltaTime)
{
  MR::PhysicsRig* physicsRig = MR::getPhysicsRig(m_net);
  bool usePhysicsRig = physicsRig && physicsRig->isReferenced();
  if (usePhysicsRig)
  {
    physicsRig->updatePostPhysics(physicsRig->getPhysicsScene()->getNextPhysicsTimeStep());
  }

  if (m_euphoriaCharacter && getNetwork()->areBehaviourNodesActive() && usePhysicsRig)
  {
    m_euphoriaCharacter->postPhysicsStep(deltaTime);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterEuphoria::updateFinalise(float deltaTime)
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
void CharacterEuphoria::writePhysicsRigToPhysX()
{
  MR::PhysicsRig* physicsRig = MR::getPhysicsRig(m_net);
  if (physicsRig)
  {
    physicsRig->updatePrePhysics(physicsRig->getPhysicsScene()->getNextPhysicsTimeStep());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterEuphoria::readPhysicsRigFromPhysX()
{
  MR::PhysicsRig* physicsRig = MR::getPhysicsRig(m_net);
  if (physicsRig)
  {
    physicsRig->updatePostPhysics(physicsRig->getPhysicsScene()->getNextPhysicsTimeStep());
  }
}

} // namespace game

//----------------------------------------------------------------------------------------------------------------------