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
#include "morpheme/mrNetworkControlSerialiser.h"
//----------------------------------------------------------------------------------------------------------------------

#define ASSETLOCATION "./ProjectData/" PLATFORMDIR
#define NETWORK_DEF_BUNDLE_NAME ASSETLOCATION "/Network_ControlSerialiser.nmb"

//----------------------------------------------------------------------------------------------------------------------
// NetworkControlSerialiser: This sample demonstrates usage of the NetworkControlSerialiser which can be used to keep
// networks synchronised across connections or store network input for playback later.
int32_t main(int32_t NMP_UNUSED(argc), char** NMP_UNUSED(argv))
{
  //----------------------------
  // This program defaults to loading the "Network" from the ProjectData folder. The animation files will 
  // be loaded from the ASSETLOCATION; see Game::CharacterDef::loadAnimationsFromDisk() for more details.
  NMP_STDOUT("Defaulting <network filename> to : '%s'", NETWORK_DEF_BUNDLE_NAME);

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
  // Game::CharacterManager manager class we will initialise our CharacterDef. This will load the file found at NETWORKPATH and
  // use that information to apply to the character definition instance.
  NMP_STDOUT("\nCreating CharacterDef:");
  Game::CharacterDef* gameCharacterDef = characterManager.createCharacterDef(NETWORK_DEF_BUNDLE_NAME);
  if(!gameCharacterDef)
  {
    NMP_STDOUT("\nError: Failed to create Character Definition");
    return NULL;
  }

  //----------------------------
  // Load the animations listed in the CharacterDef loaded from the exported network asset file.
  NMP_STDOUT("\nLoading Animation files:");
  if (!gameCharacterDef->loadAnimationsFromDisk(ASSETLOCATION))
  {
    NMP_STDOUT("\nload Animations for Character Definition returned error");
    return NULL;
  }

  //----------------------------
  // Once we have a valid character definition we can print out information about the animation rig such as the number
  // of bones and bone names. Now that we have loaded all the data within the asset file we can also print out the
  // number of nodes in the network and other information such as the number of state machines.
  if (gameCharacterDef->isLoaded())
  {
    NMP_STDOUT("\nPrinting data from GameCharacterDef:");
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
  // We need an instance of a Game::Character class that represents and wraps the runtime instance of a CharacterDef
  // class. We initialise the Character instance data using the static Game::Character::create() function.
  // This class contains the runtime network instance for a character that can be updated and run. Each character in a
  // game would have a Character class instance for updating its unique instance of a Character Definition.
  NMP_STDOUT("\nCreating Character from CharacterDef:");
  Game::CharacterNoPhysics* gameCharacterA = characterManager.createCharacter(gameCharacterDef);
  Game::CharacterNoPhysics* gameCharacterB = characterManager.createCharacter(gameCharacterDef);

  if(!gameCharacterA || ! gameCharacterB)
  {
    NMP_STDOUT("\nError: Failed to create Characters");
    return NULL;
  }
  
  MR::NetworkDef* characterNetworkDef = gameCharacterA->getNetwork()->getNetworkDef();

  MR::NodeID speedID = characterNetworkDef->getNodeIDFromNodeName("ControlParameters|Speed");
  MR::NodeID bankingID = characterNetworkDef->getNodeIDFromNodeName("ControlParameters|Banking");
  MR::NodeID headLookBlendWeightID = characterNetworkDef->getNodeIDFromNodeName("ControlParameters|BlendWeight");
  MR::NodeID lookAtPositionID = characterNetworkDef->getNodeIDFromNodeName("ControlParameters|LookAt");
  MR::NodeID stateMachineID = characterNetworkDef->getNodeIDFromNodeName("StateMachine1");
  MR::MessageID idleMessageID = characterNetworkDef->getMessageIDFromMessageName("Idle");
  MR::MessageID locomotionMessageID = characterNetworkDef->getMessageIDFromMessageName("Locomotion");

  if (speedID == MR::INVALID_NODE_ID ||
      bankingID == MR::INVALID_NODE_ID ||
      headLookBlendWeightID == MR::INVALID_NODE_ID ||
      lookAtPositionID == MR::INVALID_NODE_ID ||
      stateMachineID == MR::INVALID_NODE_ID ||
      idleMessageID == MR::INVALID_MESSAGE_ID ||
      locomotionMessageID == MR::INVALID_MESSAGE_ID)
  {
    NMP_STDOUT("\nError: Failed to find network inputs.");
    return NULL;
  }

  NMP_STDOUT("\nPerforming Updates:");

  //----------------------------
  // Our time difference between updates
  static const float timeDelta = 1.0f / 60.0f;

  //----------------------------
  // For the purpose of this tutorial we pretend characterNetworkA and characterNetworkB represent the same character
  // on different machines.
  MR::Network* characterNetworkA = gameCharacterA->getNetwork();
  MR::Network* characterNetworkB = gameCharacterB->getNetwork();

  //----------------------------
  // Create the packet descriptor. In an on line scenario there would be a descriptor on each machine. The descriptors
  // must be setup identically. If they're not then packets won't be interpreted correctly.
  MR::NetworkControlDescriptor* descriptor = MR::NetworkControlDescriptor::createPacketDescriptor(characterNetworkDef);

  //----------------------------
  // 8 bits of precision between 0.0f and 1.0f is ample precision for the speed control parameter
  MR::NetworkControlDescriptor::ControlParameterDescriptor& speedPd = descriptor->getControlParamDescriptor(speedID);
  speedPd.m_quantiseFactor = 256.0f;
  speedPd.m_quantiseFloat = true;
  speedPd.m_bitsOfPrecision = 8;

  //----------------------------
  // 8 bits of precision between -1.0f and 1.0f is ample precision for the banking control parameter
  MR::NetworkControlDescriptor::ControlParameterDescriptor& bankingPd = descriptor->getControlParamDescriptor(bankingID);
  bankingPd.m_quantiseFactor = 128.0f;
  bankingPd.m_quantiseOffset = -1.0f;
  bankingPd.m_quantiseFloat = true;
  bankingPd.m_bitsOfPrecision = 8;

  //----------------------------
  // Set the locomotion message to be send directly to the correct state machine. Leaving the message to broadcast would
  // still be sufficient.
  descriptor->setMessageTargetNode(locomotionMessageID, stateMachineID);

  //----------------------------
  // For this network the look at blend weight and position are not serialised to save space
  MR::NetworkControlDescriptor::ControlParameterDescriptor& blendWeightPd = descriptor->getControlParamDescriptor(headLookBlendWeightID);
  blendWeightPd.m_serialise = false;
  MR::NetworkControlDescriptor::ControlParameterDescriptor& lookAtPd = descriptor->getControlParamDescriptor(lookAtPositionID);
  lookAtPd.m_serialise = false;

  //----------------------------
  // Before we're done we need to update the descriptors packet memory requirements
  descriptor->updatePacketMemoryRequirements();

  //----------------------------
  // Now that we've initialised the descriptor we can create a serialiser for each network. In an online scenario, only
  // a single serialiser would be required because there would only be one network on each machine.
  MR::NetworkControlSerialiser* serialiserA = MR::NetworkControlSerialiser::createNetworkControlSerialiser(characterNetworkA, *descriptor);
  MR::NetworkControlSerialiser* serialiserB = MR::NetworkControlSerialiser::createNetworkControlSerialiser(characterNetworkB, *descriptor);

  NMP::Memory::Format packetMemoryRequirements = descriptor->getPacketMemoryRequirements(1);
  NMP::Memory::Resource packetMemory = NMPMemoryAllocateFromFormat(packetMemoryRequirements);

  //----------------------------
  // Update i number of times to test the network update. Update would normally be called once per game frame.
  for (int i = 0; i <= 1000; ++i)
  {
    //----------------------------
    // Set the control parameters in networkA. Note this must be set through the serialiser and can't be set directly 
    // on the network for the system to function correctly.
    MR::AttribDataFloat floatAttribData;
    floatAttribData.m_value = i / 1000.0f;
    serialiserA->setControlParam(speedID, &floatAttribData);

    floatAttribData.m_value = 1.0f - (i / 2000.0f);
    serialiserA->setControlParam(speedID, &floatAttribData);

    if (i == 300)
    {
      // Broadcast a message to set the network into the idle state.
      MR::Message msg(idleMessageID, MESSAGE_TYPE_REQUEST, true, NULL, 0);
      serialiserA->broadcastMessage(msg);
    }

    if (i == 500)
    {
      // Send a message to set the network into the locomotion state.
      MR::Message msg(locomotionMessageID, MESSAGE_TYPE_REQUEST, true, NULL, 0);
      serialiserA->sendMessage(stateMachineID, msg);
    }

    //----------------------------
    // Create the packet which has stored the control parameter change.
    uint32_t packetBitRequirement = serialiserA->createPacket(packetMemory.ptr, 0);
    (void)packetBitRequirement;
    // By reducing the precision and not serialising all attributes we've reduced the packet requirements to 20 bits
    // per network. Using delta packets would decrease this further in most cases.

    // At this point the packet could be relocated, transmitted to a different machine or saved to disc.

    // Apply the packet to networkB. networkA and networkB are now synchronised and will produce identical results when
    // updated.
    serialiserB->setFromPacket(packetMemory.ptr, 0);

    characterManager.updateToPreCharacterController(timeDelta);
    characterManager.updateFinalise();
  }

  NMP_STDOUT("Updates Complete");

  //----------------------------
  // When we are ready to exit we can terminate Game::CharacterManager to free any classes/memory it has stored.
  // Since Game::CharacterManager stores a pointer to the gameCharacterDef and gameCharacter instances it will
  // manage their termination as well.
  NMP_STDOUT("\nReleasing data:");
  NMP::Memory::memFree(packetMemory.ptr);
  NMP::Memory::memFree(serialiserA);
  NMP::Memory::memFree(serialiserB);
  NMP::Memory::memFree(descriptor);
  characterManager.term();
  Game::AnimSystemModuleNoPhysics::term();
  NMP_STDOUT("Release complete");

  NMP_STDOUT("\nPress Enter to Exit");
  getchar();

  return 0;
}
//----------------------------------------------------------------------------------------------------------------------
