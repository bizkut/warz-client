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
#include "GameManagement/PhysX3/GameAnimSystemModulePhysX3.h"
#include "gameManagement/PhysX3/GameCharacterManagerPhysX3.h"
#include "GameManagement/PhysX3/GameCharacterPhysX3.h"
#include "GameManagement/PhysX3/GamePhysX3Manager.h"

#include "physics/Nodes/mrPhysicsNodes.h"
#include "physics/PhysX3/mrPhysicsRigPhysX3.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
CharacterManagerPhysX3::CharacterManagerPhysX3() :
  CharacterManager()
{
  m_physicsScene = NULL;
  m_physXCharacterControllerManager = NULL;
}
//----------------------------------------------------------------------------------------------------------------------
CharacterManagerPhysX3::~CharacterManagerPhysX3()
{
  NMP_ASSERT(!m_physicsScene && !m_physXCharacterControllerManager);
}

//----------------------------------------------------------------------------------------------------------------------
CharacterDef* CharacterManagerPhysX3::createCharacterDef(
  const char* filename)
{
  return CharacterManager::createCharacterDef(filename, Game::PhysX3Manager::validatePluginList);
}

//----------------------------------------------------------------------------------------------------------------------
CharacterDef* CharacterManagerPhysX3::createCharacterDef(
  void*  bundle,
  size_t bundleSize)
{
  return CharacterManager::createCharacterDef(bundle, bundleSize, Game::PhysX3Manager::validatePluginList);
}

//----------------------------------------------------------------------------------------------------------------------
CharacterPhysX3* CharacterManagerPhysX3::createCharacter(
  CharacterDef*              characterDef,       // Must have been registered with this manager.
  MR::PhysicsRigPhysX3::Type physicsRigType,
  physx::PxMaterial*         characterControllerMaterial,
  const NMP::Vector3&        initialPosition,
  const NMP::Quat&           initialOrientation,
  MR::AnimSetIndex           initialAnimSetIndex,
  const char*                name,
  NMP::FastHeapAllocator*    temporaryMemoryAllocator,
  uint32_t                   externalIdentifier)
{
  // Check that the CharacterDef has been registered with this manager.
  NMP_ASSERT(isCharacterDefRegistered(characterDef));

  // Create the CharacterNoPhysics.
  NMP_ASSERT(m_physicsScene && m_physXCharacterControllerManager);
  CharacterPhysX3* gameCharacter = CharacterPhysX3::create(
                                                  characterDef,
                                                  m_physicsScene,
                                                  m_physXCharacterControllerManager,
                                                  characterControllerMaterial,
                                                  physicsRigType,
                                                  initialPosition,
                                                  initialOrientation,
                                                  initialAnimSetIndex,
                                                  name,
                                                  temporaryMemoryAllocator,
                                                  externalIdentifier);
  if (!gameCharacter)
  {
    NMP_STDOUT(
            "Failed to create game character %s instance from a loaded game character def %s.",
            name,
            characterDef->getNetworkDef()->getName());
    NMP_ASSERT_FAIL();
    return NULL;
  }

  // The management of this CharacterNoPhysics' memory is this CharacterManagers responsibility.
  registerCharacter(gameCharacter);

  return gameCharacter;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3::init()
{
  NMP_ASSERT(AnimSystemModulePhysX3::isInitialised());

  // Base class start up of morpheme.
  CharacterManager::init(); 
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3::term()
{
  NMP_ASSERT(AnimSystemModulePhysX3::isInitialised());
  
  // If we have any initialised characters we must still be attached to the PhysX scene.
  NMP_ASSERT(getNumRegisteredCharacters() > 0 ? m_physicsScene != NULL : true);

  // Base class shut down of morpheme.
  CharacterManager::term();

  // If we are attached to a PhysX scene detach ourselves.
  detachFromPhysXScene();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3::attachToPhysXScene(
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
void CharacterManagerPhysX3::detachFromPhysXScene()
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
void CharacterManagerPhysX3::updatePreCharacterController(float deltaTime)
{
  // Set the physics time step for the update
  NMP_ASSERT(m_physicsScene);
  m_physicsScene->setNextPhysicsTimeStep(deltaTime);

  // Update the network to the point where it is ready to have the actual Physics character controller updated.
  //  This includes calling the animation only implementation of updateToPreCharacterController().
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterPhysX3* physX3Character = static_cast<CharacterPhysX3*>(*iter);
    physX3Character->updateToPreCharacterController(deltaTime);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3::updateCharacterController(float deltaTime)
{
  // Update all Networks to the point where they are ready to have the PhysX character controller updated.
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterPhysX3* physX3Character = static_cast<CharacterPhysX3*>(*iter);
    physX3Character->updateCharacterControllerMove(deltaTime);
  }
  
  // We need to perform two updates on the PhysX3 character controllers in our scene in order to correctly
  //  handle sticking to the ground (when walking down slopes etc).
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterPhysX3* physX3Character = static_cast<CharacterPhysX3*>(*iter);
    physX3Character->updateControllerMoveAdjustingStepping(deltaTime);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3::updatePostCharacterController()
{
  // Update all Characters until we are in a position where we are required to perform the physics update step 
  //  (ie. we hit a physics task).
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterPhysX3* physX3Character = static_cast<CharacterPhysX3*>(*iter);
    physX3Character->updatePostCharacterController();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3::updatePrePhysics()
{
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterPhysX3* physX3Character = static_cast<CharacterPhysX3*>(*iter);
    physX3Character->updatePrePhysics();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3::updatePostPhysics()
{
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterPhysX3* physX3Character = static_cast<CharacterPhysX3*>(*iter);
    physX3Character->updatePostPhysics();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX3::updateFinalise(float deltaTime)
{
  // Tell morpheme the duration of the last physics time step immediately after the
  // physics engine is stepped.
  m_physicsScene->setLastPhysicsTimeStep(deltaTime);
  
  // Finish the remainder of the required update for our Characters.
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterPhysX3* physX3Character = static_cast<CharacterPhysX3*>(*iter);
    physX3Character->updateFinalise(deltaTime);
  }
}

} // Game namespace

//----------------------------------------------------------------------------------------------------------------------
