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
#include "GameManagement/Euphoria/GameAnimSystemModuleEuphoria.h"
#include "GameManagement/Euphoria/GameCharacterManagerEuphoria.h"
#include "GameManagement/Euphoria/GameCharacterEuphoria.h"
#include "GameManagement/Euphoria/GameEuphoriaManager.h"

#include "physics/Nodes/mrPhysicsNodes.h"
#include "physics/PhysX3/mrPhysicsRigPhysX3.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
CharacterManagerEuphoria::CharacterManagerEuphoria() :
  CharacterManager()
{
  m_physicsScene = NULL;
  m_physXCharacterControllerManager = NULL;
}
//----------------------------------------------------------------------------------------------------------------------
CharacterManagerEuphoria::~CharacterManagerEuphoria()
{
  NMP_ASSERT(!m_physicsScene && !m_physXCharacterControllerManager);
}

//----------------------------------------------------------------------------------------------------------------------
CharacterDefEuphoria* CharacterManagerEuphoria::createCharacterDef(
  const char* filename)
{
  Game::CharacterDefEuphoria* gameCharacterDef = Game::CharacterDefEuphoria::create(filename, Game::EuphoriaManager::validatePluginList);
  if (!gameCharacterDef || !gameCharacterDef->isLoaded())
  {
    NMP_STDOUT("gameCharacterDef %s was not loaded successfully.", filename);
    NMP_ASSERT_FAIL();
    return NULL;
  }
  registerCharacterDef(gameCharacterDef); // The management of this gameCharacterDefs memory is this CharacterManagers responsibility.
  return gameCharacterDef;
}

//----------------------------------------------------------------------------------------------------------------------
CharacterDefEuphoria* CharacterManagerEuphoria::createCharacterDef(
  void*  bundle,
  size_t bundleSize)
{
  Game::CharacterDefEuphoria* gameCharacterDef = Game::CharacterDefEuphoria::create(bundle, bundleSize, Game::EuphoriaManager::validatePluginList);
  if (!gameCharacterDef || !gameCharacterDef->isLoaded())
  {
    NMP_STDOUT("gameCharacterDef was not loaded successfully from a memory location.");
    NMP_ASSERT_FAIL();
    return NULL;
  }
  registerCharacterDef(gameCharacterDef); // The management of this gameCharacterDefs memory is this CharacterManagers responsibility.
  return gameCharacterDef;
}

//----------------------------------------------------------------------------------------------------------------------
CharacterEuphoria* CharacterManagerEuphoria::createCharacter(
  CharacterDefEuphoria*      characterDefEuphoria,
  MR::PhysicsRigPhysX3::Type physicsRigType,
  physx::PxMaterial*         characterControllerMaterial,
  const NMP::Vector3&        initialPosition,
  const NMP::Quat&           initialOrientation,
  MR::AnimSetIndex           initialAnimSetIndex,
  const char*                name,
  NMP::FastHeapAllocator*    temporaryMemoryAllocator)
{
  // Check that the CharacterDef has been registered with this manager.
  NMP_ASSERT(isCharacterDefRegistered(characterDefEuphoria));

  // Create the CharacterNoPhysics.
  NMP_ASSERT(m_physicsScene && m_physXCharacterControllerManager);
  CharacterEuphoria* gameCharacter = CharacterEuphoria::create(
                                                  characterDefEuphoria,
                                                  m_physicsScene,
                                                  m_physXCharacterControllerManager,
                                                  characterControllerMaterial,
                                                  physicsRigType,
                                                  initialPosition,
                                                  initialOrientation,
                                                  initialAnimSetIndex,
                                                  name,
                                                  temporaryMemoryAllocator);
  if (!gameCharacter)
  {
    NMP_STDOUT(
            "Failed to create game character %s instance from a loaded game character def %s.",
            name,
            characterDefEuphoria->getNetworkDef()->getName());
    NMP_ASSERT_FAIL();
    return NULL;
  }

  // The management of this CharacterNoPhysics' memory is this CharacterManagers responsibility.
  registerCharacter(gameCharacter);

  return gameCharacter;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoria::init()
{
  NMP_ASSERT(AnimSystemModuleEuphoria::isInitialised());

  // Base class start up of morpheme.
  CharacterManager::init(); 
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoria::term()
{
  NMP_ASSERT(AnimSystemModuleEuphoria::isInitialised());
  
  // If we have any initialised characters we must still be attached to the PhysX scene.
  NMP_ASSERT(getNumRegisteredCharacters() > 0 ? m_physicsScene != NULL : true);

  // Base class shut down of morpheme.
  CharacterManager::term();

  // If we are attached to a PhysX scene detach ourselves.
  detachFromPhysXScene();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoria::attachToPhysXScene(
  physx::PxScene*             physXScene, 
  physx::PxControllerManager* physXCharacterControllerManager)
{
  // Make sure we are not already attached to a scene.
  NMP_ASSERT(physXScene && physXCharacterControllerManager);
  NMP_ASSERT(!m_physicsScene && !m_physXCharacterControllerManager);
 
  // Create the morpheme wrapper to the PhysX scene.
  // Morpheme Networks use this class as a communication interface to PhysX.
  void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(MR::PhysicsScenePhysX3), NMP_VECTOR_ALIGNMENT);
  m_physicsScene = new(alignedMemory)MR::PhysicsScenePhysX3(physXScene);
  m_physicsScene->setLastPhysicsTimeStep(0.0f);
  m_physicsScene->setNextPhysicsTimeStep(0.0f);

  // Set the scene up direction.
  //  Amongst other things, the up direction is important to the character controller when
  //  it is trying to stick to the terrain.
  //  For now we just set it to be the inverse of the gravity direction set on the scene,
  //  but theoretically they could be independent values.
  physx::PxVec3 physXGravity = physXScene->getGravity();
  NMP::Vector3 worldUpVector(-physXGravity.x, -physXGravity.y, -physXGravity.z); // Invert gravity direction.
  worldUpVector.normalise(NMP::Vector3::InitTypeOneY);
  m_physicsScene->setWorldUpDirection(worldUpVector);

  // Store our reference to the PhysX character controller manager.
  m_physXCharacterControllerManager = physXCharacterControllerManager;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoria::detachFromPhysXScene()
{
  // We cannot detach ourselves from the scene if we have active characters in the scene.
  NMP_ASSERT(getNumRegisteredCharacters() == 0);
  
  if (m_physicsScene)
  {
    // In this class we have bound the existence of a character manager to the existence of a scene.
    NMP_ASSERT(m_physXCharacterControllerManager); 
    m_physXCharacterControllerManager = NULL;

    // Destroy the morpheme wrapper to the PhysX scene.
    m_physicsScene->~PhysicsScenePhysX3();
    NMP::Memory::memFree(m_physicsScene);
    m_physicsScene = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoria::updatePreCharacterController(float deltaTime)
{
  // Set the physics time step for the update
  NMP_ASSERT(m_physicsScene);
  m_physicsScene->setNextPhysicsTimeStep(deltaTime);

  // Update the network to the point where it is ready to have the actual Physics character controller updated.
  //  This includes calling the animation only implementation of updateToPreCharacterController().
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterEuphoria* euphoriaCharacter = static_cast<CharacterEuphoria*>(*iter);
    euphoriaCharacter->updateToPreCharacterController(deltaTime);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoria::updateCharacterController(float deltaTime)
{
  // Update all Networks to the point where they are ready to have the PhysX character controller updated.
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterEuphoria* euphoriaCharacter = static_cast<CharacterEuphoria*>(*iter);
    euphoriaCharacter->updateCharacterControllerMove(deltaTime);
  }
   
  // Update all Networks for dealing with staying in ground contact.
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterEuphoria* euphoriaCharacter = static_cast<CharacterEuphoria*>(*iter);
    euphoriaCharacter->updateControllerMoveAdjustingStepping(deltaTime);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoria::updatePostCharacterController(float deltaTime)
{
  // Update all Characters until we are in a position where we are required to perform the physics update step 
  //  (ie. we hit a physics task).
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterEuphoria* euphoriaCharacter = static_cast<CharacterEuphoria*>(*iter);
    euphoriaCharacter->updatePostCharacterController(deltaTime);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoria::updatePrePhysics(float deltaTime)
{
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterEuphoria* euphoriaCharacter = static_cast<CharacterEuphoria*>(*iter);
    euphoriaCharacter->updatePrePhysics(deltaTime);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoria::updatePostPhysics(float deltaTime)
{
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterEuphoria* euphoriaCharacter = static_cast<CharacterEuphoria*>(*iter);
    euphoriaCharacter->updatePostPhysics(deltaTime);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerEuphoria::updateFinalise(float deltaTime)
{
  // Tell morpheme the duration of the last physics time step immediately after the
  // physics engine is stepped.
  m_physicsScene->setLastPhysicsTimeStep(deltaTime);
  
  // Finish the remainder of the required update for our Characters.
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterEuphoria* euphoriaCharacter = static_cast<CharacterEuphoria*>(*iter);
    euphoriaCharacter->updateFinalise(deltaTime);
  }
}

} // Game namespace

//----------------------------------------------------------------------------------------------------------------------
