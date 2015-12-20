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
#include "GameCharacterManagerPhysX2.h"

#include "GameAnimLoader.h"
#include "simpleBundle/simpleBundle.h"
#include "NMPlatform/NMFile.h"
#include "morpheme/mrCharacterControllerDef.h"
#include "morpheme/Nodes/mrNodes.h"
#include "physics/Nodes/mrPhysicsNodes.h"
#include "physics/mrPhysics.h"
#include "morpheme/Nodes/mrNodeAnimSyncEvents.h"

#include <vector>

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Basic::initMorpheme()
{
  NMP_STDOUT("\nInitialising CharacterManager");

  NMP::Memory::init();

  //----------------------------
  // Initialise morpheme library
  MR::Manager::initMorphemeLib();
  
  // Dispatcher initialisation
  MR::Dispatcher* dispatcher = MR::DispatcherBasic::createAndInit();

  // Engine specific assets
  MR::Manager::getInstance().registerAsset(MR::Manager::kAsset_PhysicsRigDef, MR::locatePhysicsRigDefPhysX2);

  // Initialise the physics core by registering the task queuing functions,
  // attribute data and the task functions with the dispatchers.
  MR::initMorphemePhysics(1, &dispatcher);

  // Finalize semantic and core task registration
  MR::Manager::getInstance().allocateRegistry();
  MR::CoreTaskIDs::registerNMCoreTasks(dispatcher);

  MR::Manager::getInstance().finaliseInitMorphemeLib();
  MR::finaliseInitMorphemePhysics();

  // Tidy up the dispatcher instance since we have finished initialising the static data
  dispatcher->releaseAndDestroy();

  //----------------------------
  // Initialise animation file handling functions
  MR::Manager::getInstance().setAnimFileHandlingFunctions(
    AnimLoaderBasic::requestAnim, 
    AnimLoaderBasic::releaseAnim);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Basic::termMorpheme()
{
  if (m_physicsScene)
  {
    NMP::Memory::memFree(m_physicsScene);
    m_physicsScene = NULL;
  }

  if (m_characterData)
  {
    NMP_STDOUT("Releasing Game Character Data");
    CharacterPhysX2Basic::destroy(m_characterData);
  }

  if (m_characterDef->isLoaded())
  {
    //----------------------------
    // Once we've finished with the binary file release it.
    NMP_STDOUT("Releasing Game Character Definition");

    CharacterDefBasic::destroy(m_characterDef);
  }

  //----------------------------
  // Terminate morpheme library
  MR::DispatcherBasic::term();
#ifdef NM_HOST_CELL_PPU
  MR::DispatcherPS3::term();
#endif // NM_HOST_CELL_PPU
  MR::Manager::termMorphemeLib();
  NMP::Memory::shutdown();
}

//----------------------------------------------------------------------------------------------------------------------
// Creates a CharacterDef and registers it with the manager.
CharacterDefBasic* CharacterManagerPhysX2Basic::createCharacterDef(const char* filename)
{
  Game::CharacterDefBasic* gameCharacterDef = Game::CharacterDefBasic::create(filename);
  if (!gameCharacterDef || !gameCharacterDef->isLoaded())
  {
    NMP_STDOUT("gameCharacterDef %s was not loaded successfully.", filename);
    NMP_ASSERT_FAIL();
    return NULL;
  }
  registerCharacterDef(gameCharacterDef); // The management of this gameCharacterDefs memory is is this CharacterManagers responsibility.
  return gameCharacterDef;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Basic::attachToPhysXScene(NxScene* nxScene)
{
  //----------------------------
  // Make sure we are not already attached to a scene.
  NMP_ASSERT(nxScene);

  //----------------------------
  // Create the morpheme wrapper to the PhysX scene.
  // Morpheme Networks use this class as a communication interface to PhysX.
  void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(MR::PhysicsScenePhysX2), NMP_VECTOR_ALIGNMENT);
  m_physicsScene = new(alignedMemory) MR::PhysicsScenePhysX2(nxScene);
  m_physicsScene->setLastPhysicsTimeStep(0.0f);
  m_physicsScene->setNextPhysicsTimeStep(0.0f);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Basic::updateCharacterController(float deltaTime)
{
  NMP_ASSERT(m_characterData);
  NMP_ASSERT(m_physicsScene);

  //----------------------------
  // Set the physics time step for the update
  m_physicsScene->setNextPhysicsTimeStep(deltaTime);

  if (m_characterData)
  {
    //----------------------------
    // Update the network to the point where it is ready to have the actual Physics character controller updated.
    //  This includes calling the animation only implementation of updateToPreCharacterController().
    m_characterData->updatePreCharacterController(deltaTime);

    //----------------------------
    // Now update the actual character controller.
    m_characterData->updateCharacterController(deltaTime);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Basic::updatePostCharacterController()
{
  NMP_ASSERT(m_characterData);

  if (m_characterData)
  {
    //----------------------------
    // Update the Character until we are in a position where we are required to perform the physics update step 
    //  (ie. we hit a physics task).
    m_characterData->updatePostCharacterController();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Basic::updatePrePhysics()
{
  NMP_ASSERT(m_characterData);

  if (m_characterData)
  {
    m_characterData->updatePrePhysics();

  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Basic::updatePostPhysics()
{
  NMP_ASSERT(m_characterData);

  if (m_characterData)
  {
    m_characterData->updatePostPhysics();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Basic::updateFinalise(float deltaTime)
{
  NMP_ASSERT(m_characterData);
  NMP_ASSERT(m_physicsScene);

  //----------------------------
  // Tell morpheme the duration of the last physics time step immediately after the
  // physics engine is stepped.
  m_physicsScene->setLastPhysicsTimeStep(deltaTime);

  // Finish the remainder of the required update for our Characters.
  if (m_characterData)
  {
    m_characterData->updateFinalise();
  }
}



//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Basic::registerCharacterDef(CharacterDefBasic* characterDef)
{
  if (!characterDef)
  {
    NMP_DEBUG_MSG("error: Valid character definition expected!");
    return;
  }

  NMP_STDOUT("\nRegistering GameCharacterDef with AnimModule");

  //----------------------------
  // store a pointer to the character for use later
  m_characterDef = characterDef;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerPhysX2Basic::registerCharacter(CharacterPhysX2Basic* characterData)
{
  if (!characterData)
  {
    NMP_DEBUG_MSG("error: Valid character data expected!");
    return;
  }

  NMP_STDOUT("\nRegistering GameCharacterData with AnimModule");

  //----------------------------
  // store a pointer to the character for use later
  m_characterData = characterData;
}


} // namespace Game
