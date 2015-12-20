// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

// #include <set>
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMPrioritiesLogger.h"
#include "NMPlatform/NMSocket.h"
#include "comms/commsServer.h"
#include "comms/runtimeTargetInterface.h"
#include "comms/simpleConnectionManager.h"
#include "comms/liveLinkDataManager.h"
#include "comms/liveLinkNetworkManager.h"

#include "GameManagement/NoPhysics/GameAnimSystemModuleNoPhysics.h"
#include "GameManagement/NoPhysics/GameCharacterManagerNoPhysicsThreaded.h"

#include <windows.h>    // Sleep()
#include <conio.h>      // _getch(), _kbhit()
//----------------------------------------------------------------------------------------------------------------------

// The path for all assets used by this tutorial.
#define ASSETS_LOCATION "./ProjectData/" PLATFORMDIR

// The path and name of the NMB that contains the NetworkDef we want to use.
#define NETWORK_DEF_BUNDLE_NAME ASSETS_LOCATION "/Network_BasicStateMachine.nmb"

// Allows compilation of this demo for single or multi-threaded update.
#define MORPHEME_MULTI_THREADED_UPDATE

namespace 
{

//----------------------------------------------------------------------------------------------------------------------
// Messages we want to send to our Network instances (Names and IDs).
const char FORWARDS_LEAD_LEFT_NAME[] = "ForwardsLeadLeft";
MR::MessageID forwardsLeadLeftId = MR::INVALID_MESSAGE_ID;

// Control parameter nodes that we want to send data to (Full path names and NodeIDs).
const char SPEED_NAME[] = "ControlParameters|Speed";
const char BANKING_NAME[] = "ControlParameters|Banking";
MR::NodeID speedCPNodeID = MR::INVALID_NODE_ID;
MR::NodeID bankingCPNodeID = MR::INVALID_NODE_ID;

//----------------------------------------------------------------------------------------------------------------------
int32_t networkMultiThreadedUpdate(
  Game::CharacterManagerNoPhysicsThreaded* characterManager,
  Game::CharacterDef*                      gameCharacterDef)
{
  const uint32_t NUM_CHARACTERS = 32;

#ifdef MORPHEME_MULTI_THREADED_UPDATE
  //---------------------------
  const uint32_t numBuckets = characterManager->getNumBuckets();

  // if the NUM_CHARACTERS is not able to be evenly distributed between the NUM_BUCKETS allow an extra character per bucket.
  uint32_t extraCharacterPerBucket = (NUM_CHARACTERS % numBuckets == 0) ? 0 : 1;
  const uint32_t NUM_CHARACTERS_PER_BUCKET = (NUM_CHARACTERS / numBuckets) + extraCharacterPerBucket;
  const uint32_t NUM_UPDATE_THREADS = 4;

  // Initialize the thread scheduler. The scheduler creates one N-1 worker threads as it's going to schedule some work
  // on the main thread, while waiting for workers to complete.
  // Please note that this is implementation of the scheduler is fairly simple and far from optimal. It's only purpose
  // is to show how morpheme networks can be updated from multiple threads. This scheduler is based on the task-queue.
  // Each worker thread queries the queue for work (tasks) and the main thread executes some tasks (if available) while
  // waiting for the workers to complete. This should grant a fair level of load-balancing.
  characterManager->startThreadScheduler(NUM_UPDATE_THREADS);
#endif // MORPHEME_MULTI_THREADED_UPDATE

  //---------------------------
  // Create a connection manager, network manager and data manager.
  //
  // These are the only three components required for this tutorial while other runtime target use cases may require
  // additional components. In fact we would only really need to provide the implementation of the network and data
  // management interfaces for our purposes but also providing a connection manager allows us to monitor connections to
  // decide when to quit the application.
  const uint32_t maxNodeOutputEntries = 128;
  const uint32_t maxNumNetworkInstances = NUM_CHARACTERS;
  const uint32_t maxNumNetworkDefs = 2;

  MCOMMS::SimpleConnectionManager commsConnectionManager;
  MCOMMS::LiveLinkNetworkManager commsNetworkManager(maxNumNetworkDefs, maxNumNetworkInstances);
  MCOMMS::LiveLinkDataManager commsDataManager(maxNodeOutputEntries, maxNumNetworkInstances);
  
  // Expose the network definition to livelink.
  commsNetworkManager.publishNetworkDef(gameCharacterDef->getNetworkDef(), gameCharacterDef->getNetworkDefGuid());

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

    Game::CharacterNoPhysics* gameChar = NULL;
#ifdef MORPHEME_MULTI_THREADED_UPDATE
    // Update which bucket to add the next character to.
    uint32_t bucketIndex = i / NUM_CHARACTERS_PER_BUCKET;
    gameChar = characterManager->createCharacterMultiThreaded(
                                            gameCharacterDef,    
                                            bucketIndex,
                                            NMP::Vector3(-1.0f * i, 0.0f, 0.0f),  // Starting position.
                                            NMP::Quat::kIdentity,                 // Starting orientation.
                                            0,                                    // Starting animation set index.
                                            characterName);
#else // MORPHEME_MULTI_THREADED_UPDATE
    gameChar = characterManager->createCharacter(
                                            gameCharacterDef,
                                            NMP::Vector3(-1.0f * i, 0.0f, 0.0f),  // Starting position.
                                            NMP::Quat::kIdentity,                 // Starting orientation.
                                            0,                                    // Starting animation set index.
                                            characterName,
                                            NULL);                                // Forces the character to create its own temporary memory allocator.
#endif // MORPHEME_MULTI_THREADED_UPDATE
   
    if (!gameChar)
    {
      NMP_MSG("error: Failed to create character!");
      return EXIT_FAILURE;
    }
    else
    {
      MCOMMS::InstanceID gameCharId;
      commsNetworkManager.publishNetwork(gameChar->getNetwork(), gameChar->getName(), gameCharId);
      commsDataManager.enableControlParameterBuffering(gameCharId);

      //---------------------------
      // Set initial network states. For example the PhysicstToAnimation starts out in an idle state, we tell it to
      // transition to a (more interesting) locomotion state.
      gameChar->broadcastRequestMessage(forwardsLeadLeftId, true);
    }
  }

  //---------------------------
  // Setup the runtime target interface.
  //
  // The runtime target interface is a collection of the various runtime target components a COMMS server talks to. Only
  // those implementations required for functionality provided by a specific runtime target must be set and NULL is
  // given for all others.
  MCOMMS::RuntimeTargetInterface runtimeTarget(
                                    &commsConnectionManager,   // ConnectionManagementInterface
                                    NULL,                      // AnimationBrowserInterface 
                                    &commsNetworkManager,      // NetworkManagementInterface
                                    NULL,                      // NetworkDefLifecycleInterface
                                    NULL,                      // NetworkLifecycleInterface
                                    NULL,                      // EnvironmentManagementInterface
                                    NULL,                      // SceneObjectManagementInterface
                                    &commsDataManager,         // DataManagementInterface
                                    NULL,                      // PhysicsDataManagementInterface
                                    NULL,                      // EuphoriaDataManagementInterface
                                    NULL,                      // PlaybackManagementInterface
                                    NULL);                     // PhysicsManipulationInterface
 
  //---------------------------
  // Setup a COMMS server.
  //
  // COMMS_PORT is the TCP port the COMMS server will listen on, this must match the morpheme:connect runtime target
  // configuration. We are going with port 4567 which is the same configuration used by the default runtime targets
  // shipped with morpheme:connect.
  static const int COMMS_PORT = 4567;
  MCOMMS::CommsServer commsServer(&runtimeTarget, COMMS_PORT);
  {
    if (!commsServer.startServer(true))
    {
      NMP_MSG("\nerror: Failed to start COMMS server ...");
      return EXIT_FAILURE;
    }

    NMP_MSG("COMMS server listening on %s:%i ...\n", NMP::GetNetworkAddress(), COMMS_PORT);
  }

  //---------------------------
  // Keep updating the COMMS server.
  //
  // Updating the COMMS server will poll for new connections as well as service existing ones. We keep updating COMMS
  // until a key has been pressed or the number of connections has gone done to zero.
  static const int FPS = 30;
  static const float DELTA_TIME = 1.0f / (float)FPS;

  while (!commsConnectionManager.connectionsReducedToZero())
  {
    //---------------------------
    // Abort if a key has been pressed.
    if (_kbhit())
    {
      _getch();
      break;
    }

    //---------------------------
    // Update the COMMS server.
    commsDataManager.setFrameIndex(commsDataManager.getFrameIndex() + 1);
    commsServer.update();


    //---------------------------
    // Update our simple 'AI'.
    const Game::CharacterList& charcterList = characterManager->getCharacterList();
    for (Game::CharacterList::const_iterator  iter = charcterList.begin(); iter != charcterList.end(); ++iter)
    {
      Game::CharacterNoPhysics* gameChar = static_cast<Game::CharacterNoPhysics*>(*iter);
      
      // We use simple trigonometric functions to vary the two control parameters between 0.0 and 1.0 over time.
      const float speed = 0.5f + (sinf((float)commsDataManager.getFrameIndex() / 75.0f) * 0.5f);
      gameChar->setFloatControlParameter(speedCPNodeID, speed);

      const float banking = 0.5f + (sinf(0.5f + (float)commsDataManager.getFrameIndex() / 100.0f) * 0.5f);
      gameChar->setFloatControlParameter(bankingCPNodeID, banking);
    }

    //---------------------------
    // The entire morpheme update is wrapped in beginFrame()/endFrame().
    commsServer.beginFrame(DELTA_TIME);
    {
      NMP::Timer updateTimer;

#ifdef MORPHEME_MULTI_THREADED_UPDATE

      // Note: We have a 2 step update for our animation only characters; 
      //  this could easily be merged in to a 1 step update for faster processing,
      //  but we have introduced the second phase to allow easier AI amendment of transform deltas.

      // Schedule PreCharacterControllerTasks then wait for all the update tasks to complete.
      // Thread safe.
      characterManager->schedulePreCharacterControllerTasks(DELTA_TIME);
      characterManager->waitForAllScheduledTasks();

      // Schedule FinaliseTasks then wait for all the update tasks to complete.
      // Thread safe.
      characterManager->scheduleFinaliseTasks(DELTA_TIME);
      characterManager->waitForAllScheduledTasks();

      // Each bucket has a temporary memory allocator that is shared by all characters in the bucket.
      //  These allocators must be reset after all characters have finished their frame update.
      characterManager->resetBucketTemporaryMemoryAllocators();

#else // MORPHEME_MULTI_THREADED_UPDATE

      //
      characterManager->updateToPreCharacterController(DELTA_TIME);

      //
      characterManager->updateFinalise();

#endif // MORPHEME_MULTI_THREADED_UPDATE


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
    commsServer.endFrame(DELTA_TIME);

    //---------------------------
    // Sleep for a little to keep the thread from eating up too much cycles.
    Sleep(1000 / FPS);
  }

  //---------------------------
  // Shutdown the COMMS server.
  commsServer.stopServer();
  NMP_MSG("COMMS server stopped\n");

  //---------------------------
  // Tidy up.
#ifdef MORPHEME_MULTI_THREADED_UPDATE
  characterManager->stopThreadScheduler();
#endif
 
  //---------------------------
  // ...
  NMP_MSG("\nPress any key to exit ...");
  _getch();

  return EXIT_SUCCESS;
}

} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
// LiveLinkNetworkInspection: This tutorial demonstrates how to create a simple runtime target.
int32_t main(int32_t NMP_UNUSED(argc), char** NMP_UNUSED(argv))
{
  Game::CharacterManagerNoPhysicsThreaded characterManager;

  //---------------------------
  // Initialize morpheme.
  Game::AnimSystemModuleNoPhysics::init();

#ifdef MORPHEME_MULTI_THREADED_UPDATE
  const uint32_t NUM_BUCKETS = 8;
  characterManager.init(NUM_BUCKETS);
#else
  characterManager.init();
#endif // MORPHEME_MULTI_THREADED_UPDATE

  //---------------------------
  // Load morpheme assets.
  //
  // We load the network definition for the BasicStateMachine network. You will notice that only instances
  // created from network definitions compatible (same GUID) with the one currently loaded in morpheme:connect will be
  // listed in the runtime instance selection dialog.
  Game::CharacterDef* gameCharacterDef = characterManager.createCharacterDef(NETWORK_DEF_BUNDLE_NAME); 
  if (!gameCharacterDef)
  {
    NMP_MSG("error: Failed to load bundle <%s>!", NETWORK_DEF_BUNDLE_NAME);
    return EXIT_FAILURE;
  }
  else
  {
    if (!gameCharacterDef->loadAnimationsFromDisk(ASSETS_LOCATION))
    {
      NMP_MSG("error: Failed to load animations for bundle <%s>!", NETWORK_DEF_BUNDLE_NAME);
      return EXIT_FAILURE;
    }

    // We cache the request and control parameter node IDs for faster runtime access.
    forwardsLeadLeftId = gameCharacterDef->getNetworkDef()->getMessageIDFromMessageName(FORWARDS_LEAD_LEFT_NAME);
    NMP_ASSERT_MSG(forwardsLeadLeftId != MR::INVALID_MESSAGE_ID, "Request does not exist (name=%s)!", FORWARDS_LEAD_LEFT_NAME);

    speedCPNodeID = gameCharacterDef->getNetworkDef()->getNodeIDFromNodeName(SPEED_NAME);
    NMP_ASSERT_MSG(speedCPNodeID != MR::INVALID_NODE_ID, "Control parameter does not exist (name=%s)!", SPEED_NAME);

    bankingCPNodeID = gameCharacterDef->getNetworkDef()->getNodeIDFromNodeName(BANKING_NAME);
    NMP_ASSERT_MSG(bankingCPNodeID != MR::INVALID_NODE_ID, "Control parameter does not exist (name=%s)!", BANKING_NAME);
  }

  //---------------------------
  // Run network inspection.
  const int32_t result = networkMultiThreadedUpdate(&characterManager, gameCharacterDef);
  
  //---------------------------
  // Shutdown morpheme.
  characterManager.term();
  Game::AnimSystemModuleNoPhysics::term();

  return result;
}
//----------------------------------------------------------------------------------------------------------------------
