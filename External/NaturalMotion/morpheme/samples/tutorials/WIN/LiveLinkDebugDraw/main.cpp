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
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMPrioritiesLogger.h"
#include "NMPlatform/NMSocket.h"
#include "morpheme/mrDebugManager.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
#include "comms/commsDebugClient.h"
#include "comms/commsServer.h"
#include "comms/simpleConnectionManager.h"
#include "comms/simpleDataManager.h"
#include "comms/liveLinkNetworkManager.h"

#include "GameManagement/NoPhysics/GameAnimSystemModuleNoPhysics.h"
#include "GameManagement/NoPhysics/GameCharacterManagerNoPhysics.h"
#include "GameManagement/NoPhysics/GameCharacterNoPhysics.h"

#include <windows.h>    // Sleep()
#include <conio.h>      // _getch(), _kbhit()
//----------------------------------------------------------------------------------------------------------------------

#define ASSETLOCATION "./ProjectData/" PLATFORMDIR
#define NETWORK_DEF_BUNDLE_NAME ASSETLOCATION "/Network_BasicStateMachine.nmb"

namespace {

//----------------------------------------------------------------------------------------------------------------------
const char FORWARDS_LEAD_LEFT_NAME[] = "ForwardsLeadLeft";
const char SPEED_NAME[] = "ControlParameters|Speed";
const char BANKING_NAME[] = "ControlParameters|Banking";

MR::MessageID forwardsLeadLeftId = MR::INVALID_MESSAGE_ID;
MR::NodeID speedCPNodeID = MR::INVALID_NODE_ID;
MR::NodeID bankingCPNodeID = MR::INVALID_NODE_ID;

//----------------------------------------------------------------------------------------------------------------------
const uint32_t DELTA_TRAJECTORY_RING_BUFFER_SIZE = 30;
NMP::Quat deltaOrientationRingBuffer[DELTA_TRAJECTORY_RING_BUFFER_SIZE];
NMP::Vector3 deltaTranslationRingBuffer[DELTA_TRAJECTORY_RING_BUFFER_SIZE];
uint32_t deltaTrajectoryRingBufferIndex = 0;

// We require three lines for each arrow representing a delta trajectory and a bunch of extra lines for other purposes.
const uint32_t MAX_DEBUG_LINES = ((DELTA_TRAJECTORY_RING_BUFFER_SIZE * 3) + 50);
MCOMMS::DebugLine debugLines[MAX_DEBUG_LINES];
uint32_t numDebugLines = 0;

//----------------------------------------------------------------------------------------------------------------------
// Resets the debug line buffer index. see drawLine().
void clearDebugLines(uint32_t numPersistentLines = 0)
{
  numDebugLines = numPersistentLines;
}

//----------------------------------------------------------------------------------------------------------------------
// Adds a debug line to the buffer. see clearDebugLines(). 
// The debug draw manager must still be informed of the overall number of lines to draw each frame. This is done as
// of the update loop below (see setDataCount()).
uint32_t drawLine(const NMP::Vector3& begin, const NMP::Vector3& end, const NMP::Vector3& colour)
{
  if (numDebugLines >= MAX_DEBUG_LINES)
  {
    return 0;
  }

  debugLines[numDebugLines].m_begin.set(begin);
  debugLines[numDebugLines].m_end.set(end);
  debugLines[numDebugLines].m_colour.set(colour);
  ++numDebugLines;

  return 1;
}

//----------------------------------------------------------------------------------------------------------------------
// Adds a n-gon (circle approximation with n sides) to the debug line buffer.
uint32_t drawNGonXZ(
  MR::DebugManager*   debugManager,
  MCOMMS::InstanceID  instanceId,
  uint32_t            frameIndex,
  uint32_t            sides,
  const NMP::Vector3& position,
  float               radius,
  const NMP::Colour&  colour)
{
  uint32_t numLinesDrawn = 0;

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
      instanceId,
      MR::INVALID_NODE_ID,
      NULL,
      frameIndex,
      MR::INVALID_LIMB_INDEX,
      (from + position),
      (to + position),
      colour);
    from = to;
  }

  return numLinesDrawn;
}

//----------------------------------------------------------------------------------------------------------------------
// SimpleDataManager
// Implements MCOMMS::SimpleDataManager.
//
// This represents the absolute minimal implementation necessary.
//----------------------------------------------------------------------------------------------------------------------
class SimpleDataManager
  : public MCOMMS::SimpleDataManager
{
public:

  SimpleDataManager() : MCOMMS::SimpleDataManager() {}
  virtual ~SimpleDataManager() {}

  //----------------------------
  // [SimpleDataManager]
public:

  //----------------------------
  // Uses GameManager to find the morpheme network instance associated with the given instance ID.
  virtual MR::Network* findNetworkByInstanceID(MCOMMS::InstanceID id) const
  {
    if (MCOMMS::LiveLinkNetworkManager::getInstance())
    {
      const char* name = NULL;
      MR::Network* const network = MCOMMS::LiveLinkNetworkManager::getInstance()->findNetworkByInstanceID(id, name);
      return network;
    }

    return NULL;
  }

  //----------------------------
  // Uses GameManager to find the morpheme network definitions associated with the given GUID.
  virtual MR::NetworkDef* findNetworkDefByGuid(const MCOMMS::GUID& guid) const
  {
    if (MCOMMS::LiveLinkNetworkManager::getInstance())
    {
      MR::NetworkDef* const networkDef = MCOMMS::LiveLinkNetworkManager::getInstance()->findNetworkDefByGuid(guid);
      return networkDef;
    }

    return NULL;
  }
};

//----------------------------------------------------------------------------------------------------------------------
int32_t debugDraw(Game::CharacterNoPhysics* gameCharacter)
{
  //----------------------------
  // Create a connection manager, network manager and data manager.
  //
  // These are the only three components required for this tutorial while other runtime target use cases may require
  // additional components. In fact we would only really need to provide the implementation of the network and data
  // management interfaces for our purposes but also providing a connection manager allows us to monitor connections
  // to decide when to quit the application.

  const uint32_t maxNumNetworkInstances = 32;
  const uint32_t maxNumNetworkDefs = 2;

  MCOMMS::SimpleConnectionManager connectionManager;
  MCOMMS::LiveLinkNetworkManager networkManager(maxNumNetworkDefs, maxNumNetworkInstances);
  SimpleDataManager dataManager;

  //----------------------------
  // Expose the network definition to LiveLink.
  const Game::CharacterDef* const gameCharacterDef = gameCharacter->getCharacterDef();
  networkManager.publishNetworkDef(gameCharacterDef->getNetworkDef(), gameCharacterDef->getNetworkDefGuid());

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
    NULL, //< PhysicsDataManagementInterface
    NULL, //< EuphoriaDataManagementInterface
    NULL, //< PlaybackManagementInterface
    NULL); //< PhysicsManipulationInterface

  //----------------------------
  // Publish the network instance so it can be inspected from morpheme:connect.
  NMP_STDOUT("\nPublishing Game Character");
  MCOMMS::InstanceID instanceId;
  if (!(networkManager.publishNetwork(gameCharacter->getNetwork(), gameCharacter->getName(), instanceId)))
  {
    NMP_STDOUT("\nerror: Failed to get instance ID");
  }

  //----------------------------
  // Reset delta trajectory ring buffer.
  for (uint32_t i = 0; i < DELTA_TRAJECTORY_RING_BUFFER_SIZE; ++i)
  {
    deltaOrientationRingBuffer[i].identity();
    deltaTranslationRingBuffer[i].setToZero();
  }

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

    NMP_STDOUT("\nCOMMS server listening on %s:%i ...\n", NMP::GetNetworkAddress(), COMMS_PORT);
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
    // Update the COMMS server.
    dataManager.setFrameIndex(dataManager.getFrameIndex() + 1);
    commsServer.update();

    //----------------------------
    // Clear debug lines ahead of update and after the COMMS server update.
    debugManager->clearDebugDraw();

    //----------------------------
    // Update network input.
    //
    // We use simple trigonometric functions to vary the two control parameters between 0.0 and 1.0 over time.
    MR::AttribDataFloat speed;
    speed.m_value = 0.5f + (sinf((float)dataManager.getFrameIndex() / 75.0f) * 0.5f);
    gameCharacter->getNetwork()->setControlParameter(speedCPNodeID, &speed);
    
    MR::AttribDataFloat banking;
    banking.m_value = 0.5f + (sinf(0.5f + (float)dataManager.getFrameIndex() / 100.0f) * 0.5f);
    gameCharacter->getNetwork()->setControlParameter(bankingCPNodeID, &banking);

    //----------------------------
    // The entire morpheme update is wrapped in beginFrame()/endFrame().
    commsServer.beginFrame(DELTA_TIME);
    {
      //----------------------------
      // Update networks.
      {
        gameCharacter->updateToPreCharacterController(DELTA_TIME);
        gameCharacter->updateFinalise();
      }

      MR::Network* net = gameCharacter->getNetwork();
      
      const NMP::Vector3& gameCharPos = net->getCharacterPropertiesWorldRootTransform().translation();
      const NMP::Quat& gameCharRot = net->getCharacterPropertiesWorldRootTransform().toQuat();

      //----------------------------
      // Append deltaTranslation trajectory to ring buffer.
      deltaOrientationRingBuffer[deltaTrajectoryRingBufferIndex] = net->getOrientationChange();
      deltaTranslationRingBuffer[deltaTrajectoryRingBufferIndex] = net->getTranslationChange();
      deltaTrajectoryRingBufferIndex = (deltaTrajectoryRingBufferIndex + 1) % DELTA_TRAJECTORY_RING_BUFFER_SIZE;

      //----------------------------
      // Draw n-gon at characters position.
      NMP::Colour colour;
      colour.setf(0.1f, 0.5f, 1.0f);
      drawNGonXZ(debugManager, instanceId, dataManager.getFrameIndex(), 6, gameCharPos, 1.5f, colour);

      //----------------------------
      // Draw arrows visualizing the characters trajectory over the frames recorded in the ring buffer.
      static const float SCALE = 4.0f;
      static const float TIP_SCALE = SCALE * 0.15f;
      colour.setf(1.0f, 0.5f, 0.1f);

      NMP::Quat rotation(gameCharRot);
      NMP::Vector3 translation(gameCharPos), prevTranslation(gameCharPos);

      for (uint32_t i = 0; i < DELTA_TRAJECTORY_RING_BUFFER_SIZE; ++i)
      {
        deltaTrajectoryRingBufferIndex = (deltaTrajectoryRingBufferIndex + 1) % DELTA_TRAJECTORY_RING_BUFFER_SIZE;
      
        //----------------------------
        // Accumulate trajectory.
        const NMP::Quat deltaOrientation = deltaOrientationRingBuffer[deltaTrajectoryRingBufferIndex];
        const NMP::Vector3 deltaTranslation = deltaTranslationRingBuffer[deltaTrajectoryRingBufferIndex];
        const NMP::Vector3 deltaTranslationWorld = rotation.rotateVector(deltaTranslation);

        translation = prevTranslation + (deltaTranslationWorld * SCALE);
        rotation *= deltaOrientation;
        rotation.normalise();
      
        //----------------------------
        // Draw arrow.
        NMP::Vector3 left; left.cross(deltaTranslationWorld, NMP::Vector3YAxis());
        const NMP::Vector3 tipLeft = translation - ((deltaTranslationWorld + left) * TIP_SCALE);
        const NMP::Vector3 tipRight = translation - ((deltaTranslationWorld - left) * TIP_SCALE);

        debugManager->drawLine(instanceId, MR::INVALID_NODE_ID, NULL, dataManager.getFrameIndex(), MR::INVALID_LIMB_INDEX, prevTranslation, translation, colour);
        debugManager->drawLine(instanceId, MR::INVALID_NODE_ID, NULL, dataManager.getFrameIndex(), MR::INVALID_LIMB_INDEX, translation, tipLeft, colour);
        debugManager->drawLine(instanceId, MR::INVALID_NODE_ID, NULL, dataManager.getFrameIndex(), MR::INVALID_LIMB_INDEX, translation, tipRight, colour);

        prevTranslation = translation;
      }
    }
    commsServer.endFrame(DELTA_TIME);

    //----------------------------
    // Sleep for a little to keep the thread from eating up too much cycles.
    Sleep(1000 / FPS);
  }

  //----------------------------
  // Remove the COMMS debug client
  debugManager->removeDebugClient(&debugClient);

  //----------------------------
  // Shutdown the COMMS server.
  commsServer.stopServer();
  NMP_STDOUT("COMMS server stopped\n");

  return true;
}

} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
// LiveLinkDebugDraw: This tutorial demonstrates how to create a simple runtime target.
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
  // This also registers the definition with our manager. Management of this definitions memory is the responsibility of the characterManager.
  NMP_STDOUT("\nCreating GameCharacterDef:");
  Game::CharacterDef* gameCharacterDef = characterManager.createCharacterDef(NETWORK_DEF_BUNDLE_NAME); 
  if(!gameCharacterDef)
  {
    NMP_STDOUT("\nError: Failed to create Character Definition");
    return NULL;
  }
  
  //----------------------------
  // Load the animations listed in the GameCharacterDef loaded from the exported network asset file.
  NMP_STDOUT("\nLoading Animation files:");
  if (!gameCharacterDef->loadAnimationsFromDisk(ASSETLOCATION))
  {
    NMP_STDOUT("\nload Animations for Game Character Definition returned error");
    return NULL;
  }

  //----------------------------
  // We need an instance of a Game::Character class that represents and wraps the runtime instance of a CharacterDef
  // class. We initialise the Character instance data using the static Game::Character::create() function.
  // This class contains the runtime network instance for a character that can be updated and run. Each character in a
  // game would have a Character class instance for updating its unique instance of a character definition.
  // These characters will be identical but fully independent.
  NMP_STDOUT("\nCreating GameCharacter from GameCharacterDef");
  Game::CharacterNoPhysics* gameCharacter = characterManager.createCharacter(gameCharacterDef);
  if(!gameCharacter)
  {
    NMP_STDOUT("\nError: Failed to create Game Character");
    return NULL;
  }

  gameCharacter->setName("character1");
  
  MR::Network* network = gameCharacter->getNetwork();
  if (!network)
  {
    NMP_STDOUT("\nError: Failed to get Network from Game Character");
    return NULL;
  }

  //----------------------------
  // We cache the request and control parameter node IDs for faster runtime access.
  forwardsLeadLeftId = network->getNetworkDef()->getMessageIDFromMessageName(FORWARDS_LEAD_LEFT_NAME);
  NMP_ASSERT_MSG(forwardsLeadLeftId != MR::INVALID_MESSAGE_ID, "Request does not exist (name=%s)!", FORWARDS_LEAD_LEFT_NAME);

  speedCPNodeID = network->getNetworkDef()->getNodeIDFromNodeName(SPEED_NAME);
  NMP_ASSERT_MSG(speedCPNodeID != MR::INVALID_NODE_ID, "Control parameter does not exist (name=%s)!", SPEED_NAME);

  bankingCPNodeID = network->getNetworkDef()->getNodeIDFromNodeName(BANKING_NAME);
  NMP_ASSERT_MSG(bankingCPNodeID != MR::INVALID_NODE_ID, "Control parameter does not exist (name=%s)!", BANKING_NAME);

  //----------------------------
  // Set initial network state. For example the BasicStateMachine starts out in an idle state, we tell it to
  // transition to a (more interesting) locomotion state.
  MR::Message requestMessage(forwardsLeadLeftId, MESSAGE_TYPE_REQUEST, true, 0, 0);
  gameCharacter->getNetwork()->broadcastMessage(requestMessage);

  //----------------------------
  // Run debug draw.
  const int result = debugDraw(gameCharacter);

  //----------------------------
  // When we are ready to exit we can terminate the Game::CharacterManager to free any classes/memory it has stored.
  // Since gameAnimModule stores a pointer to the gameCharacterDef and gameCharacter instances it will
  // manage their termination as well.
  NMP_STDOUT("Releasing data:");
  characterManager.term();
  Game::AnimSystemModuleNoPhysics::term();
  NMP_STDOUT("Release complete");

  NMP_STDOUT("\nPress Enter to Exit");
  getchar();

  return result;
}
//----------------------------------------------------------------------------------------------------------------------
