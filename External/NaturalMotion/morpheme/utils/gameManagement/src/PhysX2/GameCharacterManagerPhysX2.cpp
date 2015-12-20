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
#include "GameManagement/PhysX2/GameAnimSystemModulePhysX2.h"
#include "gameManagement/PhysX2/GameCharacterManagerPhysX2.h"
#include "GameManagement/PhysX2/GameCharacterPhysX2.h"
#include "GameManagement/PhysX2/GamePhysX2Manager.h"

#include "physics/Nodes/mrPhysicsNodes.h"
#include "physics/PhysX2/mrPhysicsRigPhysX2.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

// Default altitude of the graveyard.
const float PHYSICS_GRAVEYARD_ALTITUDE = -1000.0f;

//----------------------------------------------------------------------------------------------------------------------
CharacterManagerPhysX2::CharacterManagerPhysX2() :
  CharacterManager()
{
  m_physicsScene = NULL;
  m_physXCharacterControllerManager = NULL;
  m_graveYardSeed = 0;
}
//----------------------------------------------------------------------------------------------------------------------
CharacterManagerPhysX2::~CharacterManagerPhysX2()
{
  NMP_ASSERT(!m_physicsScene && !m_physXCharacterControllerManager);
}

//----------------------------------------------------------------------------------------------------------------------
CharacterDef* CharacterManagerPhysX2::createCharacterDef(
  const char* filename)
{
  return CharacterManager::createCharacterDef(filename, Game::PhysX2Manager::validatePluginList);
}

//----------------------------------------------------------------------------------------------------------------------
CharacterDef* CharacterManagerPhysX2::createCharacterDef(
  void*  bundle,
  size_t bundleSize)
{
  return CharacterManager::createCharacterDef(bundle, bundleSize, Game::PhysX2Manager::validatePluginList);
}

//----------------------------------------------------------------------------------------------------------------------
CharacterPhysX2* CharacterManagerPhysX2::createCharacter(
  CharacterDef*           characterDef,       // Must have been registered with this manager.
  const NMP::Vector3&     initialPosition,
  const NMP::Quat&        initialOrientation,
  const NMP::Vector3&     graveyardPosition,
  MR::AnimSetIndex        initialAnimSetIndex ,
  const char*             name,
  NMP::FastHeapAllocator* temporaryMemoryAllocator,
  uint32_t                externalIdentifier)
{
  // Check that the CharacterDef has been registered with this manager.
  NMP_ASSERT(isCharacterDefRegistered(characterDef));

  // Create the CharacterNoPhysics.
  NMP_ASSERT(m_physicsScene && m_physXCharacterControllerManager);
  CharacterPhysX2* gameCharacter = CharacterPhysX2::create(
                                                  characterDef,
                                                  m_physicsScene,
                                                  m_physXCharacterControllerManager,
                                                  initialPosition,
                                                  initialOrientation,
                                                  graveyardPosition,
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
void CharacterManagerPhysX2::init()
{
  NMP_ASSERT(AnimSystemModulePhysX2::isInitialised());

  // Base class start up of morpheme.
  CharacterManager::init(); 

  // The graveyard positioning system needs a starting seed value.
  m_graveYardSeed = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2::term()
{
  NMP_ASSERT(AnimSystemModulePhysX2::isInitialised());
  
  // If we have any initialised characters we must still be attached to the PhysX scene.
  NMP_ASSERT(getNumRegisteredCharacters() > 0 ? m_physicsScene != NULL : true);

  // Base class shut down of morpheme.
  CharacterManager::term();

  // If we are attached to a PhysX scene detach ourselves.
  detachFromPhysXScene();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2::attachToPhysXScene(
  NxScene*             physXScene,
  NxControllerManager* physXCharacterControllerManager)
{
  // Make sure we are not already attached to a scene.
  NMP_ASSERT(physXScene && physXCharacterControllerManager);
  NMP_ASSERT(!m_physicsScene && !m_physXCharacterControllerManager);
 
  // Create the morpheme wrapper to the PhysX scene.
  // Morpheme Networks use this class as a communication interface to PhysX.
  void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(MR::PhysicsScenePhysX2), NMP_VECTOR_ALIGNMENT);
  m_physicsScene = new(alignedMemory) MR::PhysicsScenePhysX2(physXScene);
  m_physicsScene->setLastPhysicsTimeStep(0.0f);
  m_physicsScene->setNextPhysicsTimeStep(0.0f);

  // Set the scene up direction.
  //  Amongst other things, the up direction is important to the character controller when
  //  it is trying to stick to the terrain.
  //  For now we just set it to be the inverse of the gravity direction set on the scene,
  //  but theoretically they could be independent values.
  NxVec3 physXGravity;
  physXScene->getGravity(physXGravity);
  NMP::Vector3 worldUpVector(-physXGravity.x, -physXGravity.y, -physXGravity.z); // Invert gravity direction.
  worldUpVector.normalise(NMP::Vector3::InitTypeOneY);
  m_physicsScene->setWorldUpDirection(worldUpVector);

  // Store our reference to the PhysX character controller manager.
  m_physXCharacterControllerManager = physXCharacterControllerManager;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2::detachFromPhysXScene()
{
  // We cannot detach ourselves from the scene if we have active characters in the scene.
  NMP_ASSERT(getNumRegisteredCharacters() == 0);
  
  if (m_physicsScene)
  {
    // In this class we have bound the existence of a character manager to the existence of a scene.
    NMP_ASSERT(m_physXCharacterControllerManager); 
    m_physXCharacterControllerManager = NULL;

    // Destroy the morpheme wrapper to the PhysX scene.
    m_physicsScene->~PhysicsScenePhysX2();
    NMP::Memory::memFree(m_physicsScene);

    m_physicsScene = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2::updatePreCharacterController(float deltaTime)
{
  // Set the physics time step for the update
  NMP_ASSERT(m_physicsScene);
  m_physicsScene->setNextPhysicsTimeStep(deltaTime);

  // Update the network to the point where it is ready to have the actual Physics character controller updated.
  //  This includes calling the animation only implementation of updateToPreCharacterController().
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterPhysX2* physX2Character = static_cast<CharacterPhysX2*>(*iter);
    physX2Character->updateToPreCharacterController(deltaTime);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2::updateCharacterController(float deltaTime)
{
  // Update all Networks to the point where they are ready to have the PhysX character controller updated.
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterPhysX2* physX2Character = static_cast<CharacterPhysX2*>(*iter);
    physX2Character->updateCharacterControllerMove(deltaTime);
  }
  
  // We need to perform two updates on the PhysX2 character controllers in our scene in order to correctly
  //  handle sticking to the ground (when walking down slopes etc).
  m_physXCharacterControllerManager->updateControllers();  // Global PhysX character manager update 1.
   
  // Update all Networks for dealing with staying in ground contact.
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterPhysX2* physX2Character = static_cast<CharacterPhysX2*>(*iter);
    physX2Character->updateControllerMoveAdjustingStepping();
  }

  m_physXCharacterControllerManager->updateControllers(); // Global PhysX character manager update 2.
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2::updatePostCharacterController(float deltaTime)
{
  // Update all Characters until we are in a position where we are required to perform the physics update step 
  //  (ie. we hit a physics task).
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterPhysX2* physX2Character = static_cast<CharacterPhysX2*>(*iter);
    physX2Character->updatePostCharacterController(deltaTime);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2::updatePrePhysics()
{
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterPhysX2* physX2Character = static_cast<CharacterPhysX2*>(*iter);
    physX2Character->updatePrePhysics();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2::updatePostPhysics()
{
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterPhysX2* physX2Character = static_cast<CharacterPhysX2*>(*iter);
    physX2Character->updatePostPhysics();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2::updateFinalise(float deltaTime)
{
  // Tell morpheme the duration of the last physics time step immediately after the
  // physics engine is stepped.
  m_physicsScene->setLastPhysicsTimeStep(deltaTime);
  
  // Finish the remainder of the required update for our Characters.
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterPhysX2* physX2Character = static_cast<CharacterPhysX2*>(*iter);
    physX2Character->updateFinalise(deltaTime);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 CharacterManagerPhysX2::calculateNextCharacterGraveyardPosition(float horizontalSpacingFactor)
{
  NMP_ASSERT(m_physicsScene);
  NMP::Vector3 worldDownVector = -m_physicsScene->getWorldUpDirection();
  NMP::Vector3 graveyardPos = m_physicsScene->getWorldUpDirection() * PHYSICS_GRAVEYARD_ALTITUDE;

  float u = 0;
  float v = 0;

  uint32_t graveSeed = m_graveYardSeed;

  // Generate horizontal offset values u & v.
  while(graveSeed)
  {
    u += horizontalSpacingFactor * (graveSeed&1);
    graveSeed >>= 1;
    v += horizontalSpacingFactor * (graveSeed&1);
    graveSeed >>= 1;
    horizontalSpacingFactor *= 2;
  }

  // A fairly un-robust method of determining which axes to offset in.
  if (worldDownVector.x < -0.5)
  {
    // X up.
    graveyardPos.y += u;
    graveyardPos.z += v;
  }
  else if (worldDownVector.y < -0.5)
  {
    // Y up.
    graveyardPos.x += u;
    graveyardPos.z += v;
  }
  else
  {
    // Z up.
    graveyardPos.x += u;
    graveyardPos.y += v;
  }

  // Increment the seed ready for the next update.
  m_graveYardSeed++;

  return graveyardPos;
}

} // Game namespace

//----------------------------------------------------------------------------------------------------------------------
