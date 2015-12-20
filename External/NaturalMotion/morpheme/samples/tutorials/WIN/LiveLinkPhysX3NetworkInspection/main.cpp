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
#include "morpheme/mrDebugManager.h"
#include "morpheme/mrManager.h"
#include "comms/commsServer.h"
#include "comms/physicsCommsServerModule.h"
#include "comms/physicsRuntimeTargetSimple.h"
#include "comms/runtimeTargetNull.h"
#include "comms/simpleConnectionManager.h"
#include "comms/liveLinkDataManager.h"
#include "comms/liveLinkNetworkManager.h"

#include "GameManagement/PhysX3/GameAnimSystemModulePhysX3.h"
#include "GameManagement/PhysX3/GamePhysX3Manager.h"
#include "GameManagement/PhysX3/GameCharacterPhysX3.h"
#include "GameManagement/PhysX3/GameCharacterManagerPhysX3.h"

#include "LiveLinkPhysicsDataManager.h"

#include <windows.h>    // Sleep()
#include <conio.h>      // _getch(), _kbhit()

//#include <list>
//----------------------------------------------------------------------------------------------------------------------

#define ASSETLOCATION "./ProjectData/" PLATFORMDIR
#define NETWORK_DEF_BUNDLE_NAME ASSETLOCATION "/Network_PhysicsToAnimation.nmb"

namespace 
{

//----------------------------------------------------------------------------------------------------------------------
const char FORWARDS_NAME[] = "Forwards";
const char RAGDOLL_NAME[] = "Ragdoll";
const char GETUP_NAME[] = "GetUp";
const char SPEED_NAME[] = "ControlParameters|Speed";
const char BANKING_NAME[] = "ControlParameters|Banking";

MR::MessageID forwardsMessageID = MR::INVALID_MESSAGE_ID;
MR::MessageID ragdollMessageID = MR::INVALID_MESSAGE_ID;
MR::MessageID getupMessageID = MR::INVALID_MESSAGE_ID;
MR::NodeID speedCPNodeID = MR::INVALID_NODE_ID;
MR::NodeID bankingCPNodeID = MR::INVALID_NODE_ID;

//----------------------------------------------------------------------------------------------------------------------
// Override the default playback manager to allow synchronous stepping.
class CustomSteppingPlayback : public MCOMMS::PlaybackManagementInterface
{
public:
  bool canStepInSynch() const NM_OVERRIDE
  {
    return true;
  }

  bool canPause() const
  {
    return true;
  }

  bool canChangeDeltaTime() const
  {
    return true;
  }
};

//----------------------------------------------------------------------------------------------------------------------
int32_t networkLiveLinkInspectionWithPhysics(
  Game::PhysX3Manager&          physXManager,
  Game::CharacterManagerPhysX3& characterManager)
{
  //--------------------------------------------------------------------------------------------------------------------
  // Create a connection manager, network manager and data manager.
  //
  // These are the only three components required for this tutorial while other runtime target use cases may require
  // additional components. In fact we would only really need to provide the implementation of the network and data
  // management interfaces for our purposes but also providing a connection manager allows us to monitor connections to
  // decide when to quit the application.
  const uint32_t maxNodeOutputEntries = 128;
  const uint32_t maxNumNetworkInstances = 32;
  const uint32_t maxNumNetworkDefs = 2;

  MCOMMS::SimpleConnectionManager connectionManager;
  MCOMMS::LiveLinkNetworkManager networkManager(maxNumNetworkDefs, maxNumNetworkInstances);
  MCOMMS::LiveLinkDataManager dataManager(maxNodeOutputEntries, maxNumNetworkInstances);

  //----------------------------
  // The custom playback manager is necessary in order to override the default behaviour of LiveLink. This requires
  // the main loop to pause waiting for a step command from connect.
  CustomSteppingPlayback customPlaybackManager;

  //----------------------------
  // Now create the physics data manager 
  NMP_STDOUT("\nCreating the Live Link Physics Manager:");
  LiveLinkPhysicsDataManager customPhysicsDataManager(&networkManager);

  //----------------------------
  // Create morpheme networks instances and register with LiveLink.
  //
  // See LiveLinkDataManager::enableControlParameterBuffering() for details on the control parameter buffering.
  // Publishing the network instance will make that instance available for inspection over COMMS, the respective network
  // definitions will automatically be published as well.
  Game::CharacterDef* gameCharacterDef = characterManager.getCharacterDef(0);
  networkManager.publishNetworkDef(gameCharacterDef->getNetworkDef(), gameCharacterDef->getNetworkDefGuid());

  NMP_STDOUT("\nCreating GameCharacter from GameCharacterDef:");
  Game::CharacterPhysX3* gameCharacter = characterManager.createCharacter(gameCharacterDef);
  if (!gameCharacter)
  {
    NMP_STDOUT("error: Failed to create character!");
    return false;
  }

  gameCharacter->setName("Character");

  //----------------------------
  // Publish the network instances so that they can be inspected from morpheme:connect.
  NMP_STDOUT("\nPublishing Character 1");
  MCOMMS::InstanceID gameCharId;
  if (!(networkManager.publishNetwork(gameCharacter->getNetwork(), gameCharacter->getName(), gameCharId)))
  {
    NMP_STDOUT("\nerror: Failed to get instance ID");
  }

  dataManager.enableControlParameterBuffering(gameCharId);

  //------------------------------------------------------------------------------------------------------------------
  // Set initial network states. For example the PhysicstToAnimation starts out in an idle state, we tell it to
  // transition to a (more interesting) locomotion state.
  gameCharacter->broadcastRequestMessage(forwardsMessageID, true);

  //--------------------------------------------------------------------------------------------------------------------
  // Setup the runtime target interface.
  //
  // The runtime target interface is a collection of the various runtime target components a COMMS server talks to. Only
  // those implementations required for functionality provided by a specific runtime target must be set and NULL is
  // given for all others.
  MCOMMS::RuntimeTargetInterface runtimeTarget(
    &connectionManager, //< ConnectionManagementInterface
    NULL, //< AnimationBrowserInterface 
    &networkManager, //< NetworkManagementInterface
    NULL, //< NetworkDefLifecycleInterface
    NULL, //< NetworkLifecycleInterface
    NULL, //< EnvironmentManagementInterface
    NULL, //< SceneObjectManagementInterface
    &dataManager, //< DataManagementInterface
    &customPhysicsDataManager, //< PhysicsDataManagementInterface
    NULL, //< EuphoriaDataManagementInterface
    &customPlaybackManager, //< PlaybackManagementInterface
    NULL); //< PhysicsManipulationInterface

  //----------------------------
  // Create a runtime debugging interface.
  MR::InstanceDebugInterface debugInterface;

  //----------------------------
  // Initialise our runtime debugging interface and associate it with our network instance.
  debugInterface.init(gameCharId, gameCharacter->getNetwork(), MR::DebugManager::getInstance());
  gameCharacter->getNetwork()->getDispatcher()->setDebugInterface(&debugInterface);

  //--------------------------------------------------------------------------------------------------------------------
  // Setup a COMMS server.
  //
  // COMMS_PORT is the TCP port the COMMS server will listen on, this must match the morpheme:connect runtime target
  // configuration. We are going with port 4567 which is the same configuration used by the default runtime targets
  // shipped with morpheme:connect.
  static const int COMMS_PORT = 4567;
  MCOMMS::CommsServer commsServer(&runtimeTarget, COMMS_PORT);

  MCOMMS::PhysicsCommsServerModule* const physicsCommsServerModule = new MCOMMS::PhysicsCommsServerModule();
  commsServer.registerModule(physicsCommsServerModule);
  {
    if (!commsServer.startServer(true))
    {
      NMP_MSG("\nerror: Failed to start COMMS server ...");
      return EXIT_FAILURE;
    }

    NMP_MSG("COMMS server listening on %s:%i ...\n", NMP::GetNetworkAddress(), COMMS_PORT);
  }

  //----------------------------
  // Add a COMMS debug client.
  //
  // This allows us to forward any debug information and debug draw information back to morpheme:connect across
  // COMMS.
  MR::DebugManager* debugManager = MR::DebugManager::getInstance();
  MCOMMS::CommsDebugClient debugClient(&commsServer);
  debugManager->addDebugClient(&debugClient);

  //--------------------------------------------------------------------------------------------------------------------
  // Keep updating the COMMS server.
  //
  // Updating the COMMS server will poll for new connections as well as service existing ones. We keep updating COMMS
  // until a key has been pressed or the number of connections has gone done to zero.
  static const int FPS = 30;
  static const float DELTA_TIME = 1.0f / (float)FPS;

  uint32_t frameCounter = 0;
  uint32_t state = 1;
  while (!connectionManager.connectionsReducedToZero())
  {
    //------------------------------------------------------------------------------------------------------------------
    // Abort if a key has been pressed.
    if (_kbhit())
    {
      _getch();
      break;
    }

    //------------------------------------------------------------------------------------------------------------------
    // Update the COMMS server.
    dataManager.setFrameIndex(dataManager.getFrameIndex() + 1);
    commsServer.update();

    //----------------------------
    // Clear debug lines ahead of update and after the COMMS server update.
    debugManager->clearDebugDraw();

    if (commsServer.getNumConnections() == 0)
    {
      Sleep(10);
      continue;
    }

    ++frameCounter;

    //------------------------------------------------------------------------------------------------------------------
    // Update 'AI'.
    //
    // We use simple trigonometric functions to vary the two control parameters between 0.0 and 1.0 over time.
    MR::AttribDataFloat speed;
    speed.m_value = 0.5f + (sinf((float)dataManager.getFrameIndex() / 75.0f) * 0.5f);
    gameCharacter->getNetwork()->setControlParameter(speedCPNodeID, &speed);
 
    MR::AttribDataFloat banking;
    banking.m_value = 0.5f + (sinf(0.5f + (float)dataManager.getFrameIndex() / 100.0f) * 0.5f);
    gameCharacter->getNetwork()->setControlParameter(bankingCPNodeID, &banking);

    if((frameCounter % 250) == 0)
    {
      //----------------------------
      // Simple state machine to trigger our character's state transitions.
      switch (state)
      {
        case 0:
          //----------------------------
          // Move into Forward state
          gameCharacter->broadcastRequestMessage(forwardsMessageID, true);
          state++;
          break;
        case 1:
          //----------------------------
          // Move into Ragdoll state
          gameCharacter->broadcastRequestMessage(ragdollMessageID, true);
          state++;
          break;
        case 2:
          //----------------------------
          // Move into Getup state
          gameCharacter->broadcastRequestMessage(getupMessageID, true);
          state = 0;
          break;
        default:
          break;
      }
    }

    //------------------------------------------------------------------------------------------------------------------
    // The entire morpheme update is wrapped in beginFrame()/endFrame().
    commsServer.beginFrame(DELTA_TIME);
    {
      NMP::Timer updateTimer;

      // Schedule PreCharacterControllerTasks then wait for all the update tasks to complete.
      characterManager.updatePreCharacterController(DELTA_TIME);

      // Update each characters PhysX3 character controller.
      characterManager.updateCharacterController(DELTA_TIME);

      // Schedule PostCharacterControllerTasks then wait for all the update tasks to complete.
      characterManager.updatePostCharacterController();

      // Apply cached PhysX rig updates here.
      //  Done as a separate step because setting physics engine values may not be thread safe in some situations.
      characterManager.updatePrePhysics();

      // Run the physics simulation step.
      physXManager.simulate(DELTA_TIME);

      // Update the cache off all physics rigs with the PhysX data computed during the simulation step.
      characterManager.updatePostPhysics();

      // Schedule FinaliseTasks then wait for all the update tasks to complete.
      //  Note: Each Character instance has its own temporary memory allocator that is reset after its final update.
      characterManager.updateFinalise(DELTA_TIME);

      float elapsedTime = updateTimer.stop();
#ifdef NMP_MEMORY_LOGGING
      printf("Elapsed Time: %.2f - %d\n", elapsedTime, NMP::Memory::totalAllocatedBytes());
#else
      printf("Elapsed Time: %.2f\n", elapsedTime);
#endif
    }
    commsServer.endFrame(DELTA_TIME);

    //------------------------------------------------------------------------------------------------------------------
    // Sleep for a little to keep the thread from eating up too much cycles.
    Sleep(1000 / FPS);
  }

  //----------------------------
  // Remove the comms debug client as it is only valid within the scope of this function.
  debugManager->removeDebugClient(&debugClient);

  //----------------------------
  // Shutdown our runtime debugging interface.
  debugInterface.term();

  //--------------------------------------------------------------------------------------------------------------------
  // Shutdown the COMMS server.
  commsServer.stopServer();
  NMP_MSG("COMMS server stopped\n");

  return true;
}

} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
// LiveLinkPhysXNetworkInspection: This tutorial demonstrates how to create a simple runtime target that
// has the ability to accept connections from connect via LiveLink. The characters created here can then be inspected
// from connect. The minimal support to debug physics rigs is also implemented.
int32_t main(int32_t NMP_UNUSED(argc), char** NMP_UNUSED(argv))
{
  //----------------------------
  // This program defaults to loading the "Network" from the ProjectData folder. The animation files will 
  // be loaded from the ASSETLOCATION; see Game::CharacterDef::loadAnimationsFromDisk() for more details.
  NMP_STDOUT("Defaulting <network1 filename> to : '%s'", ASSETLOCATION);

  //----------------------------
  // Initialize PhysX
  Game::PhysX3Manager physXManager;
  NMP_STDOUT("Initialising PhysX");
  physXManager.init();
  physx::PxScene* physXScene = physXManager.createScene();
  if (!physXScene)
  {
    NMP_STDOUT("error: unable to create physics");
    return false;
  }

  //---------------------------
  // Create a simple physical environment, in this case a plane the character can stand on.
  physx::PxRigidActor* physXGroundObject = physXManager.createPhysicsObjectGroundPlane();
  if (!physXGroundObject)
  {
    NMP_STDOUT("error: Failed to create ground plane!");
    return false;
  }

  //----------------------------
  // Game::CharacterManagerPhysX3 is a static class which will act as a manager for all the morpheme network definitions 
  // and network instances. In this case we are passing in a PhysX module in order to initialise the physics 
  // world.
  Game::CharacterManagerPhysX3 characterManager;
  Game::AnimSystemModulePhysX3::init();
  characterManager.init();
  characterManager.attachToPhysXScene(
                      physXManager.getPhysXScene(),
                      physXManager.getPhysXCharacterControllerManager());
    
  //----------------------------
  // We also need an instance of a GameCharacterDef that will be initialised with the binary asset data. Using the
  // GameAnimModule manager class we will initialise our GameCharacterDef. This will load the file found at 
  // NETWORKPATH and use that information to apply to the character definition instance.
  NMP_STDOUT("\nCreating GameCharacterDefs:");
  Game::CharacterDef* gameCharacterDef = characterManager.createCharacterDef(NETWORK_DEF_BUNDLE_NAME);
  if (!gameCharacterDef)
  {
    NMP_STDOUT("error: Failed to load bundle <%s>!", NETWORK_DEF_BUNDLE_NAME);
    return false;
  }
    
  //----------------------------
  // Load the animations associated with the anim set of the CharacterDef loaded from the exported network asset file
  // on disk.
  NMP_STDOUT("\nLoading Animation files for Character Definition 1:");  
  if (!gameCharacterDef->loadAnimationsFromDisk(ASSETLOCATION))
  {
    NMP_STDOUT("\nload Animations for Game Character Definition 1 returned error");
    return false;
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

  //--------------------------------------------------------------------------------------------------------------------
  // Run network inspection.
  const int32_t result = networkLiveLinkInspectionWithPhysics(physXManager, characterManager);

  //----------------------------
  // When we are ready to exit we can terminate Game::CharacterManagerPhysX3 to free any classes/memory it has stored.
  // Since Game::CharacterManagerPhysX3 stores a pointer to the CharacterDef and Character instances it will manage
  // their termination as well.
  NMP_STDOUT("\nReleasing data");
  characterManager.term();
  Game::AnimSystemModulePhysX3::term();
  NMP_STDOUT("Release complete");

  //----------------------------
  // Finally shut down PhysX.
  physXManager.destroyPhysicsObject(physXGroundObject);
  physXManager.destroyScene();
  physXManager.term();

  //--------------------------------------------------------------------------------------------------------------------
  // ...
  NMP_STDOUT("\nPress any key to exit ...");
  _getch();

  // ...
  return result;
}
//----------------------------------------------------------------------------------------------------------------------
