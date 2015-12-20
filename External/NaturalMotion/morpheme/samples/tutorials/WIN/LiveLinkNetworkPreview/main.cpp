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
#include "NMPlatform/NMPrioritiesLogger.h"
#include "NMPlatform/NMSocket.h"
#include "morpheme/mrDebugManager.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
#include "comms/commsDebugClient.h"
#include "comms/commsServer.h"
#include "comms/simpleConnectionManager.h"

#include "GameManagement/NoPhysics/GameAnimSystemModuleNoPhysics.h"
#include "GameManagement/NoPhysics/GameCharacterManagerNoPhysics.h"
#include "GameManagement/NoPhysics/GameCharacterNoPhysics.h"
#include "comms/liveLinkDataManager.h"
#include "comms/liveLinkNetworkManager.h"
#include "LiveLinkNetworkDefLifecycle.h"
#include "LiveLinkNetworkLifecycle.h"

#include <windows.h>    // Sleep()
#include <conio.h>      // _getch(), _kbhit()
#include <sstream>
//----------------------------------------------------------------------------------------------------------------------

#define ASSETLOCATION "./ProjectData/" PLATFORMDIR
#define NETWORK_DEF_BUNDLE_NAME ASSETLOCATION "/Network_BasicStateMachine.nmb"

namespace
{

//----------------------------------------------------------------------------------------------------------------------
const char FORWARDS_LEAD_LEFT_NAME[] = "ForwardsLeadLeft";
const char SPEED_NAME[] = "ControlParameters|Speed";
const char BANKING_NAME[] = "ControlParameters|Banking";

MR::MessageID forwardsLeadLeftId = MR::INVALID_MESSAGE_ID;
MR::NodeID speedCPNodeID = MR::INVALID_NODE_ID;
MR::NodeID bankingCPNodeID = MR::INVALID_NODE_ID;

const uint32_t NUM_NPCS = 2;

MCOMMS::InstanceID  instanceIDs[NUM_NPCS];

//----------------------------------------------------------------------------------------------------------------------
// Adds a n-gon (circle approximation with n sides) to the debug line buffer.
// MR::DebugManager::drawLine()
void drawNGonXZ(
  MR::DebugManager*   debugManager,
  MCOMMS::InstanceID  instanceID,
  uint32_t            frameIndex,
  uint32_t            sides,
  const NMP::Vector3& position,
  float               radius,
  const NMP::Colour&  colour)
{
  NMP::Vector3 from;
  {
    from.x = cosf(NM_PI * 2.0f * (float)(sides - 1) / (float)sides) * radius;
    from.y = 0.0f;
    from.z = sinf(NM_PI * 2.0f * (float)(sides - 1) / (float)sides) * radius;
  }

  for (uint32_t i = 0; i < sides; ++i)
  {
    NMP::Vector3 to;
    {
      to.x = cosf(NM_PI * 2.0f * (float)i / (float)sides) * radius;
      to.y = 0.0f;
      to.z = sinf(NM_PI * 2.0f * (float)i / (float)sides) * radius;
    }

    debugManager->drawLine(
      instanceID,
      MR::INVALID_NODE_ID,
      NULL,
      frameIndex,
      MR::INVALID_LIMB_INDEX,
      (from + position),
      (to + position),
      colour);
    from = to;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Starts up the game.
//
// We create a bunch of NPC characters that we update in updateAi(). The networks of these characters can be monitored
// but modified from morpheme:connect.
bool initGame(Game::CharacterManagerNoPhysics& characterManager)
{
  //----------------------------
  // Create morpheme networks instances for NPCs.
  //
  // Using the assets loaded earlier we create NPC instances of the BasicStateMachine network. Each instance gets
  // assigned a name and is added to the game manager before taking additional setup steps below.
  for (uint32_t i = 0; i < NUM_NPCS; ++i)
  {
    //----------------------------
    // Create the character.
    NMP_STDOUT("\nCreating Game Character %i:", i);
    Game::CharacterDef* gameCharacterDef = characterManager.getCharacterDef(0);
    Game::CharacterNoPhysics* gameCharacter = characterManager.createCharacter(gameCharacterDef);

    if (!gameCharacter)
    {
      NMP_STDOUT("error: Failed to create character!");
      return false;
    }

    //----------------------------
    // Generate and set a name.
    std::stringstream npcName;
    npcName << "NPC ";
    npcName << (i + 1);

    gameCharacter->setName(npcName.str().c_str());

    //----------------------------
    // Set initial network states. For example the BasicStateMachine starts out in an idle state, we tell it to
    // transition to a (more interesting) locomotion state.
    gameCharacter->broadcastRequestMessage(forwardsLeadLeftId, true);

    //----------------------------
    // Publish the network.
    //
    // This step enables LiveLink features on the network. The network instance and it's definition will be visible to
    // morpheme:connect. The Additional options set below are specific to how we setup this tutorial.
    if (MCOMMS::LiveLinkNetworkManager::getInstance())
    {
      NMP_STDOUT("Publishing Character %i", i);
      MCOMMS::LiveLinkNetworkManager::getInstance()->publishNetwork(gameCharacter->getNetwork(), gameCharacter->getName(), instanceIDs[i]);
 
      //----------------------------
      // Disable network control to prevent morpheme:connect from changing state machines or control parameters via
      // COMMS. Please see LiveLinkDataManager::enableControlParameterBuffering() for details on the control parameter
      // buffering.
      if (MCOMMS::LiveLinkDataManager::getInstance())
      {
        MCOMMS::LiveLinkDataManager::getInstance()->disableNetworkControl(instanceIDs[i]);
        MCOMMS::LiveLinkDataManager::getInstance()->enableControlParameterBuffering(instanceIDs[i]);
      }
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Simulates a AI update step.
//
// No real AI going on here, just a bunch of trigonometric functions to move the characters around.
void updateAi(const float deltaTime, Game::CharacterManagerNoPhysics& characterManager)
{
  static float time = 0.0f;
  time += deltaTime;

  for (uint32_t i = 0; i < NUM_NPCS; ++i)
  {
    Game::CharacterNoPhysics* const gameCharacter = (Game::CharacterNoPhysics*) characterManager.getCharacter(i);

    //----------------------------
    // We use simple trigonometric functions to vary the two control parameters between 0.0 and 1.0 over time.
    MR::AttribDataFloat speed;
    speed.m_value = 0.5f + (sinf((float)i + (time / 3.0f)) * 0.5f);
    gameCharacter->getNetwork()->setControlParameter(speedCPNodeID, &speed);

    MR::AttribDataFloat banking;
    banking.m_value = 0.5f + (sinf((float)i + 0.5f + (time / 4.0f)) * 0.5f);
    gameCharacter->getNetwork()->setControlParameter(bankingCPNodeID, &banking);
  }
}


//----------------------------------------------------------------------------------------------------------------------
int32_t networkPreview(Game::CharacterManagerNoPhysics& characterManager)
{
  //----------------------------
  // Create the managers required for this tutorial.
  //
  // 1) SimpleConnectionManager allows us to monitor connections to decide when to quit the application.
  //
  // 3) LiveLinkNetworkManager advertises the network definitions and instance available to COMMS. This tutorial creates
  //    a bunch of NPC characters all based on a single network definition. Both the network definition, to allow
  //    a connected user to create additional instance, and the network instances, to allow a connected use to monitor
  //    those, are published to COMMS.
  //
  // 4) LiveLinkNetworkDefLifecycle/LiveLinkNetworkLifecycle handle the creation of additional network definitions and
  //    instances as they are requested by morpheme:connect for preview purposes. This is the core part of this tutorial
  //    as it demonstrates how an actual game can be used to preview networks on the fly.
  //
  // 5) LiveLinkDataManager handles the debug output of nodes as requested by morpheme:connect. The data managed
  //    includes the final network output, node activation and control parameter activity.
  //
  // See the documentation on the individual classes for details.
  //----------------------------

  const uint32_t maxNodeOutputEntries = 128;
  const uint32_t maxNumNetworkInstances = 32;
  const uint32_t maxNumNetworkDefs = 2;

  MCOMMS::SimpleConnectionManager connectionManager;
  MCOMMS::LiveLinkNetworkManager networkManager(maxNumNetworkDefs, maxNumNetworkInstances);
  LiveLinkNetworkDefLifecycle networkDefLifecycle(&characterManager);
  LiveLinkNetworkLifecycle networkLifecycle(&characterManager);
  MCOMMS::LiveLinkDataManager dataManager(maxNodeOutputEntries, maxNumNetworkInstances);

  Game::CharacterDef* gameCharacterDef = characterManager.getCharacterDef(0);

  // Expose the network definition to livelink.
  networkManager.publishNetworkDef(gameCharacterDef->getNetworkDef(), gameCharacterDef->getNetworkDefGuid());

  //----------------------------
  // Now setup the game (create and publish network instances etc).
  if (!initGame(characterManager))
  {
    return false;
  }

  //----------------------------
  // Setup the runtime target interface.
  //
  // The runtime target interface is a collection of the various runtime target components a COMMS server talks to. Only
  // those implementations required for functionality provided by a specific runtime target must be set and NULL is
  // given for all others.
  MCOMMS::RuntimeTargetInterface runtimeTarget(
    &connectionManager, //< ConnectionManagementInterface
    NULL, //< AnimationBrowserInterface 
    &networkManager, //< NetworkManagementInterface
    &networkDefLifecycle, //< NetworkDefLifecycleInterface
    &networkLifecycle, //< NetworkLifecycleInterface
    NULL, //< EnvironmentManagementInterface
    NULL, //< SceneObjectManagementInterface
    &dataManager, //< DataManagementInterface
    NULL, //< PhysicsDataManagementInterface
    NULL, //< EuphoriaDataManagementInterface
    NULL, //< PlaybackManagementInterface
    NULL); //< PhysicsManipulationInterface

  //----------------------------
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
      NMP_STDOUT("\nerror: Failed to start COMMS server ...");
      return false;
    }

    NMP_STDOUT("\nCOMMS server listening on %s:%i ...", NMP::GetNetworkAddress(), COMMS_PORT);
  }

  //----------------------------
  // Add a COMMS debug client.
  //
  // This allows us to forward any debug information and debug draw information back to morpheme:connect across
  // COMMS.
  MR::DebugManager* debugManager = MR::DebugManager::getInstance();
  MCOMMS::CommsDebugClient debugClient(&commsServer);
  debugManager->addDebugClient(&debugClient);

  //----------------------------
  // Keep updating the COMMS server.
  //
  // Updating the COMMS server will poll for new connections as well as service existing ones. We keep updating COMMS
  // until a key has been pressed or the number of connections has gone done to zero.
  static const int FPS = 30;
  static const float DELTA_TIME = 1.0f / (float)FPS;

  while (!connectionManager.connectionsReducedToZero())
  {
    //----------------------------
    // Abort if a key has been pressed.
    if (_kbhit())
    {
      _getch();
      break;
    }

    //----------------------------
    // Update the game.
    updateAi(DELTA_TIME, characterManager);

    //----------------------------
    // Update the COMMS server.
    dataManager.setFrameIndex(dataManager.getFrameIndex() + 1);
    commsServer.update();

    //----------------------------
    // Clear debug lines ahead of update and after the COMMS server update.
    debugManager->clearDebugDraw();

    //----------------------------
    // Draw persistent debug lines.
    //
    // We draw a circle at the origin for better orientation.
    NMP::Colour colour;
    colour.setf(0.25f, 1.0f, 0.25f);
    drawNGonXZ(debugManager, MCOMMS::INVALID_INSTANCE_ID, dataManager.getFrameIndex(), 6, NMP::Vector3Zero(), 1.0f, colour);

    //----------------------------
    // The entire morpheme update is wrapped in beginFrame()/endFrame().
    commsServer.beginFrame(DELTA_TIME);
    {
      //----------------------------
      // Update all NPC characters.
      for (uint32_t i = 0; i < NUM_NPCS; ++i)
      { 
        //----------------------------
        // Publish any new Game Characters
        Game::CharacterNoPhysics* gameCharacter = (Game::CharacterNoPhysics*) characterManager.getCharacter(i);

        //----------------------------
        // Update our morpheme network
        {
          gameCharacter->updateToPreCharacterController(DELTA_TIME);
          gameCharacter->updateFinalise();
        }

        //----------------------------
        // Draw n-gon at characters position.
        //
        // This provides a visual clue as to where the NPCs are located in the scene even if the network instances are
        // not monitored directly.
        const NMP::Vector3& position = 
           gameCharacter->getNetwork()->getCharacterPropertiesWorldRootTransform().translation();
 
        colour.setf(0.1f, 0.5f, 1.0f);
        drawNGonXZ(debugManager, instanceIDs[i], dataManager.getFrameIndex(), 6, position, 1.5f, colour);

        //----------------------------
        // Draw the characters pose.
        const MR::AnimRigDef* animRig = NULL;
        const NMP::DataBuffer* const pose = gameCharacter->getWorldTransforms(animRig);

        if (pose && animRig)
        {
          colour.setf(0.5f, 0.5f, 0.5f);
          const int32_t numBones = (int32_t)animRig->getNumBones();

          for (int32_t boneInedex = 2; boneInedex < numBones; ++boneInedex)
          {
            const int32_t parentBoneIndex = animRig->getParentBoneIndex(boneInedex);

            if (parentBoneIndex > 0)
            {
              debugManager->drawLine(
                instanceIDs[i],
                MR::INVALID_NODE_ID,
                NULL,
                dataManager.getFrameIndex(),
                MR::INVALID_LIMB_INDEX,
                (*pose->getPosQuatChannelPos(parentBoneIndex)),
                (*pose->getPosQuatChannelPos(boneInedex)),
                colour);
            }
          }
        }
      }

      //----------------------------
      // Update all LiveLink characters.
      const LiveLinkNetworkLifecycle::CharacterMap characters = networkLifecycle.getCharacters();

      const LiveLinkNetworkLifecycle::CharacterMap::const_iterator end = characters.end();
      LiveLinkNetworkLifecycle::CharacterMap::const_iterator it = characters.begin();

      for (; it != end; ++it)
      {
        it->second.m_character->updateToPreCharacterController(DELTA_TIME);
        it->second.m_character->updateFinalise();
      }
    }
    commsServer.endFrame(DELTA_TIME);

    //----------------------------
    // Sleep for a little to keep the thread from eating up too much cycles.
    Sleep(1000 / FPS);
  }

  //----------------------------
  // Remove the comms debug client as it is only valid within the scope of this function.
  debugManager->removeDebugClient(&debugClient);

  //----------------------------
  // Shutdown the COMMS server.
  commsServer.stopServer();
  NMP_STDOUT("COMMS server stopped\n");

  return false;
}

} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
// LiveLinkNetworkPreview: This tutorial demonstrates how to create a simple runtime target.
int32_t main(int32_t NMP_UNUSED(argc), char** NMP_UNUSED(argv))
{
  //----------------------------
  // This program defaults to loading the "Network" from the ProjectData folder. The animation files will 
  // be loaded from the ASSETLOCATION; see Game::CharacterDef::loadAnimationsFromDisk() for more details.
  NMP_STDOUT("Defaulting <network1 filename> to : '%s'", NETWORK_DEF_BUNDLE_NAME);

  //----------------------------
  // Game::CharacterManagerNoPhysics which will act as a manager for all the morpheme network definitions 
  // and network instances. Initialise will set up the morpheme libraries and load all the default node types
  // and other default functions.
  Game::CharacterManagerNoPhysics characterManager;

  //---------------------------
  // Initialize morpheme.
  Game::AnimSystemModuleNoPhysics::init();
  characterManager.init();

  //----------------------------
  // We also need an instance of a Game::CharacterDef that will be initialised with the binary asset data. Using the
  // Game::CharacterManager manager class we will initialise our CharacterDef. This will load the file found at ASSETLOCATION
  // and use that information to apply to the character definition instance.
  NMP_STDOUT("\nCreating CharacterDef:");
  Game::CharacterDef* gameCharacterDef = characterManager.createCharacterDef(NETWORK_DEF_BUNDLE_NAME); 
  if(!gameCharacterDef)
  {
    NMP_STDOUT("\nError: Failed to create Game Character Definition 1");
    return false;
  }

  //----------------------------
  // Load the animations listed in the CharacterDef loaded from the exported network asset file.
  NMP_STDOUT("\nLoading Animation files:");
  if (!gameCharacterDef->loadAnimationsFromDisk(ASSETLOCATION))
  {
    NMP_STDOUT("\nload Animations for Game Character Definition returned error");
    return false;
  }

  //----------------------------
  // We cache the meesageID and control parameter node IDs for faster runtime access.
  forwardsLeadLeftId = gameCharacterDef->getNetworkDef()->getMessageIDFromMessageName(FORWARDS_LEAD_LEFT_NAME);
  NMP_ASSERT_MSG(forwardsLeadLeftId != MR::INVALID_MESSAGE_ID, "Message does not exist (name=%s)!", FORWARDS_LEAD_LEFT_NAME);

  speedCPNodeID = gameCharacterDef->getNetworkDef()->getNodeIDFromNodeName(SPEED_NAME);
  NMP_ASSERT_MSG(speedCPNodeID != MR::INVALID_NODE_ID, "Control parameter does not exist (name=%s)!", SPEED_NAME);

  bankingCPNodeID = gameCharacterDef->getNetworkDef()->getNodeIDFromNodeName(BANKING_NAME);
  NMP_ASSERT_MSG(bankingCPNodeID != MR::INVALID_NODE_ID, "Control parameter does not exist (name=%s)!", BANKING_NAME);

  //----------------------------
  // Run network preview.
  const int result = networkPreview(characterManager);

  //----------------------------
  // When we are ready to exit we can terminate Game::CharacterManager to free any classes/memory it has stored.
  // Since Game::CharacterManager stores a pointer to the gameCharacterDef and gameCharacter instances it will
  // manage their termination as well.
  NMP_STDOUT("\nReleasing data");
  characterManager.term();
  Game::AnimSystemModuleNoPhysics::term();
  NMP_STDOUT("Release complete");

  NMP_STDOUT("\nPress Enter to Exit");
  getchar();

  // ...
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
