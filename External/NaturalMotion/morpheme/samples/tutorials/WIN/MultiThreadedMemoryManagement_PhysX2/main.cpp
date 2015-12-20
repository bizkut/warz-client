// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPrioritiesLogger.h"
#include "NMPlatform/NMSocket.h"

#include "GameManagement/PhysX2/GameAnimSystemModulePhysX2.h"
#include "GameManagement/PhysX2/GamePhysX2Manager.h"
#include "GameCharacterManagerMTAlloc.h"

// Our sample multi-thread friendly allocator
#include "MultiThreadTempAllocator.h"

#include <windows.h>    // Sleep()
#include <conio.h>      // _getch(), _kbhit()
//----------------------------------------------------------------------------------------------------------------------

// The path for all assets used by this tutorial.
#define ASSETS_LOCATION "./ProjectData/" PLATFORMDIR

// The path and name of the NMB that contains the NetworkDef we want to use.
#define NETWORK_DEF_BUNDLE_NAME ASSETS_LOCATION "/Network_PhysicsToAnimation.nmb"

namespace 
{

//----------------------------------------------------------------------------------------------------------------------
// Messages we want to send to our Network instances (Names and IDs).
const char FORWARDS_NAME[] = "Forwards";
const char RAGDOLL_NAME[] = "Ragdoll";
const char GETUP_NAME[] = "GetUp";
MR::MessageID forwardsMessageID = MR::INVALID_MESSAGE_ID;
MR::MessageID ragdollMessageID = MR::INVALID_MESSAGE_ID;
MR::MessageID getupMessageID = MR::INVALID_MESSAGE_ID;

// Control parameter nodes that we want to send data to (Full path names and NodeIDs).
const char SPEED_NAME[] = "ControlParameters|Speed";
const char BANKING_NAME[] = "ControlParameters|Banking";
MR::NodeID speedCPNodeID = MR::INVALID_NODE_ID;
MR::NodeID bankingCPNodeID = MR::INVALID_NODE_ID;


//----------------------------------------------------------------------------------------------------------------------
int32_t networkMultiThreadedUpdatePhysics(
  Game::PhysX2Manager*                  physXManager,
  Game::CharacterManagerMTAlloc*        characterManager,
  Game::CharacterDef*                   gameCharacterDef)
{
  const uint32_t NUM_CHARACTERS = 32;
  const uint32_t NUM_UPDATE_THREADS = 8;

  // Note: This controls the size of the pages of memory in the thread-safe memory pool.  If you make the pages
  //  larger, it may improve performance by reducing the number of mutexed accesses to the memory pool that need to be
  //  made during update.  If you make the pages smaller, it will increase the efficiency of memory usage as there will
  //  be less wasted memory in partially-filled pages.  Some experimentation will reveal the sweet-spot in your
  //  application for memory usage vs. performance.
  const uint32_t MEMORY_PAGE_SIZE = 16 * 1024;  // 16K pages
  // This is a very large estimate for the total amount of pages needed in this tutorial.  You can tune this value
  //  downwards until you are running out of pages in your worst-case memory usage frames.
  const uint32_t NUM_MEMORY_PAGES = 8 * NUM_CHARACTERS;  // 128K per character in the shared pool.

  //---------------------------
  // Initialize the thread scheduler. The scheduler creates one N workers threads, one per worker object that is
  // added to the following list.
  characterManager->startThreadScheduler(NUM_UPDATE_THREADS);
  characterManager->createSharedMemoryPool(NUM_MEMORY_PAGES, MEMORY_PAGE_SIZE);

  Game::CharacterPhysX2* characters[NUM_CHARACTERS];

  //---------------------------
  // Create all the Character instances and add them the CharacterManager.
  for (uint32_t i = 0; i < NUM_CHARACTERS; ++i)
  {
    //---------------------------
    // Create morpheme networks instances and register with LiveLink.
    //
    // See LiveLinkDataManager::enableControlParameterBuffering() for details on the control parameter buffering.
    // Publishing the network instance will make that instance available for inspection over COMMS, the respective network
    // definitions will automatically be published as well.
    char characterName[32];
    NMP_SPRINTF(characterName, 32, "character%d", i + 1);

    characters[i] = characterManager->createCharacterMultiThreaded(
                                            gameCharacterDef,    
                                            NMP::Vector3(-1.0f * i, 0.0f, 0.0f),  // Starting position.
                                            NMP::Quat::kIdentity,                 // Starting orientation.
                                            characterManager->calculateNextCharacterGraveyardPosition(2.0f),
                                            0,                                    // Starting animation set index.
                                            characterName);

    if (!characters[i])
    {
      NMP_STDOUT("error: Failed to create character!");
      return EXIT_FAILURE;
    }
    else
    {
      //---------------------------
      // Set initial network states. For example the PhysicstToAnimation starts out in an idle state, we tell it to
      // transition to a (more interesting) locomotion state.
      characters[i]->broadcastRequestMessage(forwardsMessageID, true);
    }
  }

  static const int FPS = 30;
  static const float DELTA_TIME = 1.0f / (float)FPS;

  uint32_t frameCounter = 0;
  bool infinity = true;
  while (infinity)
  {
    //---------------------------
    // Abort if a key has been pressed.
    if (_kbhit())
    {
      _getch();
      break;
    }

    ++frameCounter;

    //---------------------------
    // Update our simple 'AI'.
    uint32_t iCharacter = 0;
    const Game::CharacterList& charcterList = characterManager->getCharacterList();
    for (Game::CharacterList::const_iterator  iter = charcterList.begin(); iter != charcterList.end(); ++iter)
    {
      Game::CharacterPhysX2* gameChar = static_cast<Game::CharacterPhysX2*>(*iter);
      
      // We use simple trigonometric functions to vary the two control parameters between 0.0 and 1.0 over time.
      const float speed = 0.5f + (sinf((float)frameCounter / 75.0f) * 0.5f);
      gameChar->setFloatControlParameter(speedCPNodeID, speed);

      const float banking = 0.5f + (sinf(0.5f + (float)frameCounter / 100.0f + iCharacter) * 0.5f);
      gameChar->setFloatControlParameter(bankingCPNodeID, banking);

      if ((frameCounter % (100 + iCharacter * 5)) == 0)
      {
        gameChar->broadcastRequestMessage(ragdollMessageID, true);
      }
      else if ((frameCounter % (300 + iCharacter * 5)) == 0)
      {
        gameChar->broadcastRequestMessage(getupMessageID, true);
      }

      ++iCharacter;
    }


    {
      NMP::Timer updateTimer;

      // Schedule PreCharacterControllerTasks then wait for all the update tasks to complete.
      // Thread safe.
      characterManager->schedulePreCharacterControllerTasks(DELTA_TIME);
      characterManager->waitForAllScheduledTasks();
            
      // Not thread safe as physics system properties are directly set here.
      characterManager->updateCharacterController(DELTA_TIME);
            
      // Schedule PostCharacterControllerTasks then wait for all the update tasks to complete.
      // Thread safe.
      characterManager->schedulePostCharacterControllerTasks(DELTA_TIME);
      characterManager->waitForAllScheduledTasks();

      // Apply cached PhysX rig updates here.
      //  Not thread safe as physics system properties are directly set here.
      characterManager->updatePrePhysics();

      // Run the global physics simulation step.
      physXManager->simulate(DELTA_TIME);

      // Update the cache for all physics rigs with the PhysX data computed during the simulation step.
      //  Not thread safe as physics system properties are directly set here.
      characterManager->updatePostPhysics();

      // Schedule FinaliseTasks then wait for all the update tasks to complete.
      // Thread safe.
      characterManager->scheduleFinaliseTasks(DELTA_TIME);
      characterManager->waitForAllScheduledTasks();

      // Each character has a client allocator and there is a shared threadsafe pool.
      //  These allocators must be reset after all characters have finished their frame update.
      characterManager->resetTemporaryMemoryAllocators();

#ifdef THIS_IS_WHERE_YOU_WOULD_MAP_THE_OUTPUT_TO_YOUR_GAME_CHARACTERS_SKELETON
        const MR::AnimRigDef* animRig = NULL;
        const NMP::DataBuffer* pose = characterManager->getCharacter(0)->getWorldTransforms(animRig);
#endif // THIS_IS_WHERE_YOU_WOULD_MAP_THE_OUTPUT_TO_YOUR_GAME_CHARACTERS_SKELETON

      float elapsedTime = updateTimer.stop();
#ifdef NMP_MEMORY_LOGGING
      printf("Elapsed Time: %.2f - %d\n", elapsedTime, NMP::Memory::totalAllocatedBytes());
#else
      printf("Elapsed Time: %.2f\n", elapsedTime);
#endif
    }

    //---------------------------
    // Sleep for a little to keep the thread from eating up too much cycles.
    Sleep(1000 / FPS);
  }

  for (uint32_t i = 0; i < NUM_CHARACTERS; ++i)
  {
    characterManager->destroyCharacter(characters[i]);
  }

  //---------------------------
  // Tidy up.
  characterManager->stopThreadScheduler();
  characterManager->destroySharedMemoryPool();
  
  //---------------------------
  // ...
  NMP_STDOUT("\nPress any key to exit ...");
  _getch();

  return EXIT_SUCCESS;
}

} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
// MultiThreadedMemoryManagement: This tutorial demonstrates how to create a simple runtime target.
int32_t main(int32_t NMP_UNUSED(argc), char** NMP_UNUSED(argv))
{
  Game::PhysX2Manager physXManager;
  Game::CharacterManagerMTAlloc characterManager;

  //---------------------------
  // Initialize PhysX
  physXManager.init();
  physXManager.createScene();

  //---------------------------
  // Create a simple physical environment, in this case a plane the character can stand on.
  NxActor* physXGroundObject = physXManager.createPhysicsObjectGroundPlane();
  
  //---------------------------
  // Initialize morpheme and attach it to PhysX.
  Game::AnimSystemModulePhysX2::init();
  characterManager.init();
  characterManager.attachToPhysXScene(
                      physXManager.getPhysXScene(),
                      physXManager.getPhysXCharacterControllerManager());
  
  
  //---------------------------
  // Load morpheme assets.
  //
  // We load the network definition for the PhysicsToAnimation network. You will notice that only instances
  // created from network definitions compatible (same GUID) with the one currently loaded in morpheme:connect will be
  // listed in the runtime instance selection dialog.
  Game::CharacterDef* gameCharacterDef = characterManager.createCharacterDef(NETWORK_DEF_BUNDLE_NAME);
  if (!gameCharacterDef)
  {
    NMP_STDOUT("error: Failed to load bundle <%s>!", NETWORK_DEF_BUNDLE_NAME);
    return EXIT_FAILURE;
  }
  else
  {
    if (!gameCharacterDef->loadAnimationsFromDisk(ASSETS_LOCATION))
    {
      NMP_STDOUT("error: Failed to load animations for bundle <%s>!", NETWORK_DEF_BUNDLE_NAME);
      return EXIT_FAILURE;
    }

    // We cache the request and control parameter node IDs for faster runtime access.
    forwardsMessageID = gameCharacterDef->getNetworkDef()->getMessageIDFromMessageName(FORWARDS_NAME);
    NMP_ASSERT_MSG(forwardsMessageID != MR::INVALID_MESSAGE_ID, "Request does not exist (name=%s)!", FORWARDS_NAME);

    ragdollMessageID = gameCharacterDef->getNetworkDef()->getMessageIDFromMessageName(RAGDOLL_NAME);
    NMP_ASSERT_MSG(forwardsMessageID != MR::INVALID_MESSAGE_ID, "Request does not exist (name=%s)!", RAGDOLL_NAME);

    speedCPNodeID = gameCharacterDef->getNetworkDef()->getNodeIDFromNodeName(SPEED_NAME);
    NMP_ASSERT_MSG(speedCPNodeID != MR::INVALID_NODE_ID, "Control parameter does not exist (name=%s)!", SPEED_NAME);

    bankingCPNodeID = gameCharacterDef->getNetworkDef()->getNodeIDFromNodeName(BANKING_NAME);
    NMP_ASSERT_MSG(bankingCPNodeID != MR::INVALID_NODE_ID, "Control parameter does not exist (name=%s)!", BANKING_NAME);

    getupMessageID = gameCharacterDef->getNetworkDef()->getMessageIDFromMessageName(GETUP_NAME);
    NMP_ASSERT_MSG(getupMessageID != MR::INVALID_MESSAGE_ID, "Request does not exist (name=%s)!", GETUP_NAME);
  }

  //---------------------------
  // Run network inspection.
  const int32_t result = networkMultiThreadedUpdatePhysics(&physXManager, &characterManager, gameCharacterDef);

  characterManager.destroyCharacterDef(gameCharacterDef);

  //---------------------------
  // Detach morpheme from PhysX and shut it down.
  characterManager.detachFromPhysXScene();
  characterManager.term();
  Game::AnimSystemModulePhysX2::term();
  
  //---------------------------
  // Shut down PhysX.
  physXManager.destroyPhysicsObject(physXGroundObject);
  physXManager.destroyScene();
  physXManager.term();
  
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
