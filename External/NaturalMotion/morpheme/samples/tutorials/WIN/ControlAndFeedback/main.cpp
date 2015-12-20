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
#include "GameManagement/NoPhysics/GameAnimSystemModuleNoPhysics.h"
#include "GameManagement/NoPhysics/GameCharacterManagerNoPhysics.h"
#include "GameManagement/NoPhysics/GameCharacterNoPhysics.h"
//----------------------------------------------------------------------------------------------------------------------

#define MAX_NODE_ARRAY_COUNT 30
#define ASSETLOCATION "./ProjectData/" PLATFORMDIR
#define NETWORK_DEF_BUNDLE_NAME ASSETLOCATION "/Network_ControlAndFeedback.nmb"

//----------------------------------------------------------------------------------------------------------------------
//Reset all currently active messageIDs on all state machines. Do NOT call this half way though a morpheme update.
void resetAllNetworkMessageIDs(MR::Network* net)
{
#if defined(MR_RESET_SM_MESSAGE_IDS_POST_UPDATE)
  // This is already done internally after every update so don't bother.
  // To turn on this behaviour look for #define MR_RESET_SM_MESSAGE_IDS_POST_UPDATE
  // and remove the "x" off the end.
  NMP_ASSERT_FAIL();
#endif
  //reset the message states in messageID conditions to false
  net->clearMessageStatusesOnAllStateMachines();
};

//----------------------------------------------------------------------------------------------------------------------
//Reset all currently active messageIDs on all state machines. Do NOT call this half way though a morpheme update.
void resetAllStateMachineMessageIDs(MR::Network* net, MR::NodeID smNodeID)
{
#if defined(MR_RESET_SM_MESSAGE_IDS_POST_UPDATE)
  // This is already done internally after every update so don't bother.
  // To turn on this behaviour look for #define MR_RESET_SM_MESSAGE_IDS_POST_UPDATE
  // and remove the "x" off the end.
  NMP_ASSERT_FAIL();
#endif
  //reset the message states in messageID conditions to false
  net->clearMessageStatusesOnStateMachine(smNodeID);
};

//----------------------------------------------------------------------------------------------------------------------
/// Check for any events that were triggered during the last update, and print their user data out.
/// returns true if any events were triggered, and false otherwise.
bool printSampledEvents(MR::Network* network)
{
  MR::TriggeredDiscreteEventsBuffer* discreteEvents = NULL;
  MR::SampledCurveEventsBuffer* durationEvents = NULL;

  MR::AttribAddress eventAttribAddress(
    MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER,
    network->getNetworkDef()->getRootNodeID(),
    MR::INVALID_NODE_ID,
    network->getCurrentFrameNo(),
    network->getActiveAnimSetIndex());
  MR::NodeBinEntry* eventBufferEntry = network->getAttribDataNodeBinEntry(eventAttribAddress);

  if (eventBufferEntry)
  {
    NMP_ASSERT(
      eventBufferEntry->m_attribDataHandle.m_attribData && 
      (eventBufferEntry->m_attribDataHandle.m_attribData->getType() == MR::ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER));
    discreteEvents = ((MR::AttribDataSampledEvents*) eventBufferEntry->m_attribDataHandle.m_attribData)->m_discreteBuffer;
    durationEvents = ((MR::AttribDataSampledEvents*) eventBufferEntry->m_attribDataHandle.m_attribData)->m_curveBuffer;
    NMP_ASSERT(discreteEvents && durationEvents);

    uint32_t numDiscreteEvents = discreteEvents->getNumTriggeredEvents();
    uint32_t numDurationEvents = durationEvents->getNumSampledEvents();

    // Early out if no events have fired
    if (numDiscreteEvents == 0 && numDurationEvents == 0)
    {
      printf("*** No events at frame %d ***\n\n", network->getCurrentFrameNo());
      return false;
    }
    printf("*** Events at frame %d ***\n\n", network->getCurrentFrameNo());

    for (uint32_t eventIdx = 0 ; eventIdx < numDiscreteEvents ; ++eventIdx)
    {
      const MR::TriggeredDiscreteEvent* discreteEvent = discreteEvents->getTriggeredEvent(eventIdx);
      NMP_ASSERT(discreteEvent);
      printf("**************************");
      printf(
        " DiscreteEvent\nTrack ID: %d\nTrack name: %s\nTrack user data: %d\nEvent user data: %d\nEvent contribution to output: %f\n",
        discreteEvent->getSourceTrackRuntimeID(),
        network->getNetworkDef()->getEventTrackNameFromRuntimeID(discreteEvent->getSourceTrackRuntimeID()),
        discreteEvent->getSourceTrackUserData(),
        discreteEvent->getSourceEventUserData(),
        discreteEvent->getBlendWeight());
    }

    // Check for active duration events
    {
      for (uint32_t i = 0 ; i < numDurationEvents ; ++i)
      {
        const MR::SampledCurveEvent* durationEvent = durationEvents->getSampledEvent(i);
        printf("**************************");
        printf(" FloatEvent\nTrack ID: %d\nTrack name: %s\nTrack user data: %d\nEvent user data: %d\nValue: %f\nEvent contribution: %f\n",
               durationEvent->getSourceTrackRuntimeID(),
               network->getNetworkDef()->getEventTrackNameFromRuntimeID(durationEvent->getSourceTrackRuntimeID()),
               durationEvent->getSourceTrackUserData(),
               durationEvent->getSourceEventUserData(),
               durationEvent->getValue(),
               durationEvent->getBlendWeight());
      }
    }
    printf("\n");

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void printActiveNodes(MR::Network* network)
{
  MR::NodeID nodeIds[MAX_NODE_ARRAY_COUNT];
  uint32_t activeNodeCount = network->getActiveNodes(nodeIds, MAX_NODE_ARRAY_COUNT);
  NMP_ASSERT(activeNodeCount <= MAX_NODE_ARRAY_COUNT);

  printf("*** Active Nodes at frame %d ***\n\n", network->getCurrentFrameNo());
  for (uint32_t i = 0; i != activeNodeCount; ++i)
  {
    const char* nodeName = network->getNetworkDef()->getNodeNameFromNodeID((MR::NodeID)nodeIds[i]);
    printf("Current Active Node runtime ID: %d. Name: %s \n", (MR::NodeID)nodeIds[i], nodeName);
  }
  printf("\n\n");
}

//----------------------------------------------------------------------------------------------------------------------
bool isNodeActive(MR::Network* network, const MR::NodeID nodeId)
{
  MR::NodeID nodeIds[MAX_NODE_ARRAY_COUNT];
  uint32_t activeNodeCount = network->getActiveNodes(nodeIds, MAX_NODE_ARRAY_COUNT);
  NMP_ASSERT(activeNodeCount <= MAX_NODE_ARRAY_COUNT);

  for (uint32_t i = 0; i != activeNodeCount; ++i)
  {
    if (nodeId == (MR::NodeID)nodeIds[i])
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void setFloatControlParameter(MR::Network* network, MR::NodeID cpId, float floatValue)
{
  // The CP attribute data already exists. We just need to set the new value.
  MR::AttribDataFloat* floatCP_Data = (MR::AttribDataFloat*)network->getControlParameter(cpId);
  NMP_ASSERT(floatCP_Data);
  floatCP_Data->m_value = floatValue;
  network->setControlParameter(cpId, floatCP_Data);
}

//----------------------------------------------------------------------------------------------------------------------
float getFloatControlParameter(MR::Network* network, MR::NodeID cpId)
{
  MR::AttribDataFloat* flatCP_Data = (MR::AttribDataFloat*)network->getControlParameter(cpId);
  NMP_ASSERT(flatCP_Data);
  return flatCP_Data->m_value;
}

//----------------------------------------------------------------------------------------------------------------------
// morphemeRuntimeeventsRequestsAndControlParams: This example shows how to access events at runtime as well as showing 
// how to send requests to a network and accessing control parameters.
int32_t main(int32_t NMP_UNUSED(argc), char** NMP_UNUSED(argv))
{
  //----------------------------
  // This program defaults to loading the "Network" from the ProjectData folder. The animation files will 
  // also be loaded from this location; see Game::CharacterManager::requestAnim() for more details.
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
  // Game::CharacterManager manager class we will initialise our Game::CharacterDef. This will load the file found at 
  // ASSETLOCATION and use that information to apply to the character definition instance.
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
  // Once we have a valid character definition we can print out information about the animation rig such as the number
  // of bones and bone names. Now that we have loaded all the data within the asset file we can also print out the
  // number of nodes in the network and other information such as the number of state machines.
  if (gameCharacterDef->isLoaded())
  {
    NMP_STDOUT("\nPrinting data from CharacterDef:");
    NMP_STDOUT("Number of Node Definitions: %d", gameCharacterDef->getNetworkDef()->getNumNodeDefs());
    NMP_STDOUT("Number of State Machines: %d", gameCharacterDef->getNetworkDef()->getNumStateMachines());
    NMP_STDOUT("Number of Control Parameters: %d", gameCharacterDef->getNetworkDef()->getNumControlParameterNodes());
    NMP_STDOUT("\nList of bones in rig:");

    const MR::AnimRigDef* rig = gameCharacterDef->getNetworkDef()->getRig(0);
    for (uint32_t i = 0; i < rig->getNumBones(); i++)
    {
      NMP_STDOUT("%s", rig->getBoneName(i));
    }
  }

  //----------------------------
  // We need an instance of a Game::Character class that represents and wraps the runtime instance of a Game::CharacterDef
  // class. We initialise the Game::Character instance data using the create function in Game::Character.
  // This class contains the runtime network instance for a character that can be updated and run. Each character in a
  // game would have a Game::Character class instance for updating its unique instance of a character definition.
  // These characters will be identical but fully independent.
  NMP_STDOUT("\nCreating Character from CharacterDef");
  Game::CharacterNoPhysics* gameCharacter = characterManager.createCharacter(gameCharacterDef);
  if(!gameCharacter)
  {
    NMP_STDOUT("\nError: Failed to create Game Character");
    return NULL;
  }
  MR::Network* network = gameCharacter->getNetwork();

  //----------------------------
  // Our time difference between updates
  static const float timeDelta = 1.0f / 60.0f;
 
  //----------------------------
  // Store the runtime id's we are going to need at runtime

  //----------------------------
  //[NODES]
  MR::NodeID forwardsStateOutputNode = network->getNetworkDef()->getNodeIDFromNodeName("Forwards|BlendNMatchEvents1");
  NMP_ASSERT(forwardsStateOutputNode != MR::INVALID_NODE_ID);
  MR::NodeID idleStateOutputNode = network->getNetworkDef()->getNodeIDFromNodeName("Idle|_01001_StandingIdle");
  NMP_ASSERT(idleStateOutputNode != MR::INVALID_NODE_ID);
  MR::NodeID transitToTurnNode = network->getNetworkDef()->getNodeIDFromNodeName("TransitMatchEvents3");
  NMP_ASSERT(transitToTurnNode != MR::INVALID_NODE_ID);

  //----------------------------
  //[CONTROL PARAMETERS]
  MR::NodeID CP_SPEED = network->getNetworkDef()->getNodeIDFromNodeName("ControlParameters|Speed");
  NMP_ASSERT(CP_SPEED != MR::INVALID_NODE_ID);
 
  //----------------------------
  //[REQUESTS]
  MR::MessageID START_LEFT  = network->getNetworkDef()->getMessageIDFromMessageName("ForwardsLeadLeft");
  NMP_ASSERT(START_LEFT != MR::INVALID_NODE_ID);
  MR::MessageID TURN_LEFT  = network->getNetworkDef()->getMessageIDFromMessageName("TurnLeft");
  NMP_ASSERT(TURN_LEFT != MR::INVALID_NODE_ID);
 
  //----------------------------
  // Make sure we start at slow pace.
  setFloatControlParameter(network, CP_SPEED, 0.0f);

  NMP_STDOUT("\nPerforming Updates:");

  //----------------------------
  // Update i number of times to test the network update. Update would normally be called once per game frame.
  for (int32_t i = 0; i <= 4000; ++i)
  {
    //----------------------------
    //update the network
    characterManager.updateToPreCharacterController(timeDelta);
    characterManager.updateFinalise();
    
    //----------------------------
    // Make sure we are in the idle state before sending a request message to move forwards at frame 10
    if (i == 10 && isNodeActive(network, idleStateOutputNode))
    {
      //----------------------------
      // Send forward request message to network
      MR::Message forwardRequestMessage(START_LEFT, MESSAGE_TYPE_REQUEST, true, 0, 0);
      network->broadcastMessage(forwardRequestMessage);
    }

    //----------------------------
    // Make sure we are going forwards and not transitioning to a turn blend tree state.
    if (isNodeActive(network, forwardsStateOutputNode) && !isNodeActive(network, transitToTurnNode))
    {
      //----------------------------
      // Slowly increase the "Speed" control parameter
      float cpFloat = getFloatControlParameter(network, CP_SPEED);
      cpFloat += 0.005f;
      cpFloat = NMP::clampValue(cpFloat, 0.0f, 1.0f);
      setFloatControlParameter(network, CP_SPEED, cpFloat);

      //----------------------------
      // Send a turn request message when the "Speed" control parameter reaches 1.0.
      if (getFloatControlParameter(network, CP_SPEED) >= 1.0f)
      {
        MR::Message turnLeftMessage(TURN_LEFT, MESSAGE_TYPE_REQUEST, true, 0, 0);
        network->broadcastMessage(turnLeftMessage);

        //----------------------------
        // Reset the "Speed" control parameter to 0.0.
        setFloatControlParameter(network, CP_SPEED, 0.0f);
      }
    }

    //----------------------------
    // print active nodes
    printActiveNodes(network);

    //----------------------------
    // print sampled events
    printSampledEvents(network);
  }

  NMP_STDOUT("Updates Complete\n");

  //----------------------------
  // When we are ready to exit we can terminate Game::CharacterManager to free any classes/memory it has stored.
  // Since Game::CharacterManager stores a pointer to the CharacterDef and Character instances it will manage
  // their termination as well.
  NMP_STDOUT("Releasing data:");
  characterManager.term();
  Game::AnimSystemModuleNoPhysics::term();
  NMP_STDOUT("Release complete");

  NMP_STDOUT("\nPress Enter to Exit");
  getchar();

  return 0;
}
//----------------------------------------------------------------------------------------------------------------------
