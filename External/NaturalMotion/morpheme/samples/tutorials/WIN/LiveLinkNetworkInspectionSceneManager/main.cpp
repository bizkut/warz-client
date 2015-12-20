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
#include "comms/commsServer.h"
#include "comms/coreCommsServerModule.h"
#include "comms/liveLinkDataManager.h"
#include "comms/liveLinkNetworkManager.h"
#include "comms/liveLinkSceneObjectManager.h"
#include "comms/simpleConnectionManager.h"

#include "GameManagement/NoPhysics/GameAnimSystemModuleNoPhysics.h"
#include "GameManagement/NoPhysics/GameCharacterManagerNoPhysics.h"
#include "GameManagement/NoPhysics/GameCharacterNoPhysics.h"

#include <windows.h>    // Sleep()
#include <conio.h>      // _getch(), _kbhit()
//----------------------------------------------------------------------------------------------------------------------

#define ASSET_LOCATION "./ProjectData/" PLATFORMDIR
#define NETWORKPATH_BASICSTATEMACHINE  ASSET_LOCATION "/Network_BasicStateMachine.nmb"
#define NETWORKPATH_FIGHTING ASSET_LOCATION "/Network_Fighting.nmb"
#define CHARACTER_SKIN_FILE "$(RootDir)/Characters/MaleCharacter/morphemeRigs/maleCharacterAnimationRig.mcskin"

namespace 
{

//----------------------------------------------------------------------------------------------------------------------
const char FORWARDS_LEAD_LEFT_NAME[] = "ForwardsLeadLeft";
const char SPEED_NAME[] = "ControlParameters|Speed";
const char BANKING_NAME[] = "ControlParameters|Banking";

MR::MessageID forwardsLeadLeftId = MR::INVALID_MESSAGE_ID;
MR::NodeID speedCPNodeID = MR::INVALID_NODE_ID;
MR::NodeID bankingCPNodeID = MR::INVALID_NODE_ID;

//----------------------------------------------------------------------------------------------------------------------
int32_t networkInspectionSceneObjectManager(Game::CharacterManagerNoPhysics& characterManager)
{
  //----------------------------
  // Create a connection manager, network manager and data manager.
  //
  // These are the only three components required for this tutorial while other runtime target use cases may require
  // additional components. In fact we would only really need to provide the implementation of the network and data
  // management interfaces for our purposes but also providing a connection manager allows us to monitor connections to
  // decide when to quit the application.

  const uint32_t maxNodeOutputEntries = 128;
  const uint32_t maxNumNetworkInstances = 32;
  const uint32_t maxNumNetworkDefs = 2;
  const uint32_t maxNumEnvironmentObjects = 16;
  const uint32_t maxNumSceneObjects = 
    maxNumNetworkInstances * 2 + maxNumEnvironmentObjects; // One for the skin, one for the character controller,
                                                           // plus some environment objects.

  MCOMMS::SimpleConnectionManager connectionManager;
  MCOMMS::LiveLinkNetworkManager networkManager(maxNumNetworkDefs, maxNumNetworkInstances);
  MCOMMS::LiveLinkDataManager dataManager(maxNodeOutputEntries, maxNumNetworkInstances);

  // Create a scene object manager. This manager offers some useful functionality to create scene objects.
  // See the comms/liveLinkSceneObjectManager.h for some helper classes
  MCOMMS::LiveLinkSceneObjectManager sceneObjectManager(&networkManager, maxNumSceneObjects);

  //----------------------------
  // Create a runtime debugging interfaces.
  MR::InstanceDebugInterface character1DebugInterface;
  MR::InstanceDebugInterface character2DebugInterface;

  //----------------------------
  // Create morpheme networks instances and register with LiveLink.
  //
  // See LiveLinkDataManager::enableControlParameterBuffering() for details on the control parameter buffering.
  // Publishing the network instance will make that instance available for inspection over COMMS, the respective network
  // definitions will automatically be published as well.
  Game::CharacterDef* gameCharacterDef1 = characterManager.getCharacterDef(0);
  networkManager.publishNetworkDef(gameCharacterDef1->getNetworkDef(), gameCharacterDef1->getNetworkDefGuid());

  Game::CharacterDef* gameCharacterDef2 = characterManager.getCharacterDef(1);
  networkManager.publishNetworkDef(gameCharacterDef2->getNetworkDef(), gameCharacterDef2->getNetworkDefGuid());

  NMP_STDOUT("\nCreating GameCharacter1 from GameCharacterDef1:");
  Game::CharacterNoPhysics* gameCharacter1 = characterManager.createCharacter(gameCharacterDef1);
  if (!gameCharacter1)
  {
    NMP_MSG("error: Failed to create character 1!");
    return false;
  }
  gameCharacter1->setName("Character1");

  NMP_STDOUT("\nCreating GameCharacter2 from GameCharacterDef2:");
  Game::CharacterNoPhysics* gameCharacter2 = characterManager.createCharacter(gameCharacterDef2);
  if (!gameCharacter2)
  {
    NMP_MSG("error: Failed to create character 2!");
    return false;
  }
  gameCharacter2->setName("Character2");

  
  //----------------------------
  // Publish the first network instance so it can be inspected from morpheme:connect.
  NMP_STDOUT("\nPublishing Game Character 1");
  MCOMMS::InstanceID gameChar1Id;
  if (!(networkManager.publishNetwork(gameCharacter1->getNetwork(), gameCharacter1->getName(), gameChar1Id)))
  {
    NMP_STDOUT("\nerror: Failed to get instance ID");
  }

  //----------------------------
  // Disable network control to prevent morpheme:connect from changing state machines or control parameters via
  // COMMS. Please see LiveLinkDataManager::enableControlParameterBuffering() for details on the control parameter
  // buffering.
  dataManager.disableNetworkControl(gameChar1Id);
  dataManager.enableControlParameterBuffering(gameChar1Id);

  //----------------------------
  // Initialise our runtime debugging interface and associate it with our network instance.
  character1DebugInterface.init(gameChar1Id, gameCharacter1->getNetwork(), MR::DebugManager::getInstance());
  gameCharacter1->getNetwork()->getDispatcher()->setDebugInterface(&character1DebugInterface);

  //----------------------------
  // Publish the second network instance so it can be inspected from morpheme:connect.
  NMP_STDOUT("Publishing Game Character 2");
  MCOMMS::InstanceID gameChar2Id;
  if (!(networkManager.publishNetwork(gameCharacter2->getNetwork(), gameCharacter2->getName(), gameChar2Id)))
  {
    NMP_STDOUT("\nerror: Failed to get instance ID");
  }

  //----------------------------
  // Disable network control to prevent morpheme:connect from changing state machines or control parameters via
  // COMMS. Please see LiveLinkDataManager::enableControlParameterBuffering() for details on the control parameter
  // buffering.
  dataManager.disableNetworkControl(gameChar2Id);
  dataManager.enableControlParameterBuffering(gameChar2Id);

  //----------------------------
  // Initialise our runtime debugging interface and associate it with our network instance.
  character2DebugInterface.init(gameChar2Id, gameCharacter2->getNetwork(), MR::DebugManager::getInstance());
  gameCharacter2->getNetwork()->getDispatcher()->setDebugInterface(&character2DebugInterface);

  //----------------------------
  // Create the character controller scene object for the characters
  MCOMMS::GameControllerSceneObject* gameControllerObject1 = new MCOMMS::GameControllerSceneObject();
  gameControllerObject1->initSceneObject(&sceneObjectManager, gameChar1Id, NMP::Vector3(NMP::Vector3::InitTypeZero));
  sceneObjectManager.publishSceneObject(gameControllerObject1);


  MCOMMS::GameControllerSceneObject* gameControllerObject2 = new MCOMMS::GameControllerSceneObject();
  gameControllerObject2->initSceneObject(&sceneObjectManager, gameChar2Id, NMP::Vector3(NMP::Vector3::InitTypeZero));
  sceneObjectManager.publishSceneObject(gameControllerObject2);

  //----------------------------
  // Create the skin scene object for the characters
  MCOMMS::GameCharacterSceneObject* gameCharacterObject1 = new MCOMMS::GameCharacterSceneObject();
  gameCharacterObject1->initSceneObject(
    &sceneObjectManager, 
    gameChar1Id, 
    CHARACTER_SKIN_FILE);
  sceneObjectManager.publishSceneObject(gameCharacterObject1);

  MCOMMS::GameCharacterSceneObject* gameCharacterObject2 = new MCOMMS::GameCharacterSceneObject();
  gameCharacterObject2->initSceneObject(
    &sceneObjectManager, 
    gameChar2Id,
    CHARACTER_SKIN_FILE);
  sceneObjectManager.publishSceneObject(gameCharacterObject2);


  // Create a static box for the ground.
  MCOMMS::GameShapeSceneObject* gameShapeObjectGround = new MCOMMS::GameShapeSceneObject();
  {
    MCOMMS::GameShapeSceneObject::ShapeDesc shapeDesc;
    shapeDesc.shapeType = MCOMMS::GameShapeSceneObject::ShapeDesc::kShapeBox;
    shapeDesc.motionEnabled = false;
    shapeDesc.params[0] = 100;
    shapeDesc.params[1] = 100;
    shapeDesc.params[2] = 1;
    shapeDesc.initialTransform = NMP::Matrix34(NMP::Matrix34::kIdentity);
    shapeDesc.initialTransform.translation() = NMP::Vector3(0, -shapeDesc.params[2] * 0.5f, 0);
    gameShapeObjectGround->initSceneObject(&sceneObjectManager, shapeDesc);
    sceneObjectManager.publishSceneObject(gameShapeObjectGround);
  }

  // Create a dynamic shape object.
  MCOMMS::GameShapeSceneObject* gameShapeObjectDynamic = new MCOMMS::GameShapeSceneObject();
  const float sphereRadius = 2.5f;
  const float initialHeight = 5;
  NMP::Matrix34 initialObjTransform = NMP::Matrix34(NMP::Matrix34::kIdentity);
  initialObjTransform.translation() = NMP::Vector3(10, initialHeight + sphereRadius, 10);
  {
    MCOMMS::GameShapeSceneObject::ShapeDesc shapeDesc;
    shapeDesc.shapeType = MCOMMS::GameShapeSceneObject::ShapeDesc::kShapeSphere;
    shapeDesc.motionEnabled = true; // Setting this to true allows sending an updated transform for the object.
    shapeDesc.params[0] = sphereRadius;
    shapeDesc.initialTransform = initialObjTransform;
    gameShapeObjectDynamic->initSceneObject(&sceneObjectManager, shapeDesc);
    sceneObjectManager.publishSceneObject(gameShapeObjectDynamic);
  }

  // Create an environment mesh object
  MCOMMS::GameMeshSceneObject* gameMeshObject = new MCOMMS::GameMeshSceneObject();
  {
    // Describe a simple pyramid.
    const float pyramidSize = 10;
    const float cos30 = cos(NM_PI / 6.0f);
    const float sin30 = sin(NM_PI / 6.0f);
    NMP::Vector3 vertices[4];
    vertices[0] = NMP::Vector3(                   0,           0,        -pyramidSize);
    vertices[1] = NMP::Vector3(-pyramidSize * cos30,           0, pyramidSize * sin30);
    vertices[2] = NMP::Vector3( pyramidSize * cos30,           0, pyramidSize * sin30);
    vertices[3] = NMP::Vector3(                   0, pyramidSize,                   0);

    int indices[3 * 4] = 
    {
      0, 2, 1, 
      0, 1, 3, 
      1, 2, 3, 
      2, 0, 3
    };

    MCOMMS::GameMeshSceneObject::MeshDesc meshDesc;
    meshDesc.motionEnabled = false;
    meshDesc.clockWise = false;
    meshDesc.initialTransform = NMP::Matrix34(NMP::Matrix34::kIdentity);
    meshDesc.initialTransform.translation().x = -30;
    meshDesc.numPoints = 4;
    meshDesc.pointBuffer = vertices;
    meshDesc.numIndices = 4 * 3;
    meshDesc.indexBuffer = indices;
    gameMeshObject->initSceneObject(&sceneObjectManager, meshDesc);
    sceneObjectManager.publishSceneObject(gameMeshObject);
  }

  //----------------------------
  // Set initial network states. For example the BasicStateMachine starts out in an idle state, we tell it to
  // transition to a (more interesting) locomotion state.
  gameCharacter1->broadcastRequestMessage(forwardsLeadLeftId, true);

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
    NULL, //< NetworkDefLifecycleInterface
    NULL, //< NetworkLifecycleInterface
    NULL, //< EnvironmentManagementInterface
    &sceneObjectManager, //< SceneObjectManagementInterface
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

    NMP_STDOUT("\nCOMMS server listening on %s:%i ...\n", NMP::GetNetworkAddress(), COMMS_PORT);
  }
  NMP_ASSERT(commsServer.getCoreModule());
  commsServer.getCoreModule()->initializeSceneObjectsSyncCache();

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
  const int FPS = 30;
  const float DELTA_TIME = 1.0f / (float)FPS;
  float elapsedTime = 0;

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
    // Update 'AI'.
    //
    // We use simple trigonometric functions to vary the two control parameters between 0.0 and 1.0 over time.
    MR::AttribDataFloat speed;
    speed.m_value = 0.5f + (sinf((float)dataManager.getFrameIndex() / 75.0f) * 0.5f);
    gameCharacter1->getNetwork()->setControlParameter(speedCPNodeID, &speed);

    MR::AttribDataFloat banking;
    banking.m_value = 0.5f + (sinf(0.5f + (float)dataManager.getFrameIndex() / 100.0f) * 0.5f);
    gameCharacter1->getNetwork()->setControlParameter(bankingCPNodeID, &banking);

    //----------------------------
    // The entire morpheme update is wrapped in beginFrame()/endFrame().
    commsServer.beginFrame(DELTA_TIME);
    {
      //----------------------------
      // Update all networks.
      characterManager.updateToPreCharacterController(DELTA_TIME);
      characterManager.updateFinalise();  

      // Make the dynamic scene object move up and down.
      const float maxHeight = 5;
      NMP::Matrix34 tm = initialObjTransform;
      float y = initialHeight + sphereRadius - maxHeight * sin(elapsedTime);
      tm.translation().y = y;
      gameShapeObjectDynamic->setTransform(tm);
      elapsedTime += DELTA_TIME;

      // Update scene objects.
      sceneObjectManager.updateSceneObjects(DELTA_TIME);
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
  // Shutdown our runtime debugging interfaces.
  character1DebugInterface.term();
  character2DebugInterface.term();

  //----------------------------
  // Shutdown the COMMS server.
  commsServer.stopServer();
  NMP_STDOUT("COMMS server stopped");

  // Clean up scene objects.
  sceneObjectManager.revokeSceneObject(gameControllerObject1);
  sceneObjectManager.revokeSceneObject(gameControllerObject2);
  delete gameControllerObject1;
  delete gameControllerObject2;

  sceneObjectManager.revokeSceneObject(gameCharacterObject1);
  sceneObjectManager.revokeSceneObject(gameCharacterObject2);
  delete gameCharacterObject1;
  delete gameCharacterObject2;

  sceneObjectManager.revokeSceneObject(gameShapeObjectGround);
  sceneObjectManager.revokeSceneObject(gameShapeObjectDynamic);
  sceneObjectManager.revokeSceneObject(gameMeshObject);
  delete gameShapeObjectGround;
  delete gameShapeObjectDynamic;
  delete gameMeshObject;

  return true;
}

} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
// LiveLinkNetworkInspectionSceneManager
// This tutorial is an extension of the network inspection. The added features show how to create 'Scene Objects' in
// order to visualize some useful entities in morpheme:connect: the skin of the character, the character controller,
// and some environment objects.
int32_t main(int32_t NMP_UNUSED(argc), char** NMP_UNUSED(argv))
{
  //----------------------------
  // This program defaults to loading the "Network" from the ProjectData folder. The animation files will 
  // be loaded from the ASSETLOCATION; see Game::CharacterDef::loadAnimationsFromDisk() for more details.
  NMP_STDOUT("Defaulting <network1 filename> to : '%s'", NETWORKPATH_BASICSTATEMACHINE);
  NMP_STDOUT("Defaulting <network2 filename> to : '%s'", NETWORKPATH_FIGHTING);

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
  // Game::CharacterManager manager class we will initialise our CharacterDef. This will load the file found at NETWORKPATH
  // and use that information to apply to the character definition instance.
  NMP_STDOUT("\nCreating GameCharacterDefs:");
  Game::CharacterDef* gameCharacterDef1 = characterManager.createCharacterDef(NETWORKPATH_BASICSTATEMACHINE);
  if(!gameCharacterDef1)
  {
    NMP_STDOUT("\nError: Failed to create Game Character Definition 1");
    return NULL;
  }

  Game::CharacterDef* gameCharacterDef2 = characterManager.createCharacterDef(NETWORKPATH_FIGHTING); 
  if(!gameCharacterDef2)
  {
    NMP_STDOUT("\nError: Failed to create Game Character Definition 2");
    return NULL;
  }

  //----------------------------
  // Load the animations associated with the anim set of the GameCharacterDef loaded from the exported network asset file
  // on disk.
  NMP_STDOUT("\nLoading Animation files for Game Character Definition 1:");  
  if (!gameCharacterDef1->loadAnimationsFromDisk(ASSET_LOCATION))
  {
    NMP_STDOUT("\nload Animations for Game Character Definition 1 returned error");
    return NULL;
  }

  NMP_STDOUT("\nLoading Animation files for Game Character Definition 2:");
  if (!gameCharacterDef2->loadAnimationsFromDisk(ASSET_LOCATION))
  {
    NMP_STDOUT("\nload Animations for Game Character Definition 2 returned error");
    return NULL;
  }

  //----------------------------
  // We cache the request and control parameter node IDs for faster runtime access.
  forwardsLeadLeftId = gameCharacterDef1->getNetworkDef()->getMessageIDFromMessageName(FORWARDS_LEAD_LEFT_NAME);
  NMP_ASSERT_MSG(forwardsLeadLeftId != MR::INVALID_MESSAGE_ID, "Request does not exist (name=%s)!", FORWARDS_LEAD_LEFT_NAME);

  speedCPNodeID = gameCharacterDef1->getNetworkDef()->getNodeIDFromNodeName(SPEED_NAME);
  NMP_ASSERT_MSG(speedCPNodeID != MR::INVALID_NODE_ID, "Control parameter does not exist (name=%s)!", SPEED_NAME);

  bankingCPNodeID = gameCharacterDef1->getNetworkDef()->getNodeIDFromNodeName(BANKING_NAME);
  NMP_ASSERT_MSG(bankingCPNodeID != MR::INVALID_NODE_ID, "Control parameter does not exist (name=%s)!", BANKING_NAME);
  
  //----------------------------
  // Run network inspection.
  const int result = networkInspectionSceneObjectManager(characterManager);

  //----------------------------
  // When we are ready to exit we can terminate GameAnimModule to free any classes/memory it has stored.
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
