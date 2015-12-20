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

#define ASSETLOCATION "./ProjectData/" PLATFORMDIR
#define NETWORK_DEF_BUNDLE_NAME1 ASSETLOCATION "/Network_MultipleNetworks2.nmb"
#define NETWORK_DEF_BUNDLE_NAME2 ASSETLOCATION "/Network2_MultipleNetworks2.nmb"

//----------------------------------------------------------------------------------------------------------------------
// MultipleNetworks2:
int32_t main(int32_t NMP_UNUSED(argc), char** NMP_UNUSED(argv))
{
  //----------------------------
  // This program defaults to loading the "Network" from the ProjectData folder. The animation files will 
  // be loaded from the ASSETLOCATION; see Game::CharacterDef::loadAnimationsFromDisk() for more details.
  NMP_STDOUT("Defaulting <network1 filename> to : '%s'", NETWORK_DEF_BUNDLE_NAME1);
  NMP_STDOUT("Defaulting <network2 filename> to : '%s'", NETWORK_DEF_BUNDLE_NAME2);

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
  // Game::AnimModule manager class we will initialise our Game::CharacterDef. This will load the file found at 
  // ASSETLOCATION and use that information to apply to the character definition instance.
  NMP_STDOUT("\nCreating Game Character Definition 1:"); 
  Game::CharacterDef* gameCharacterDef1 = characterManager.createCharacterDef(NETWORK_DEF_BUNDLE_NAME1);
  if(!gameCharacterDef1)
  {
    NMP_STDOUT("\nError: Failed to create Game Character Definition 1");
    return NULL;
  }

  NMP_STDOUT("\nCreating Game Character Definition 2:");
  Game::CharacterDef* gameCharacterDef2 = characterManager.createCharacterDef(NETWORK_DEF_BUNDLE_NAME2);
  if(!gameCharacterDef2)
  {
    NMP_STDOUT("\nError: Failed to create Game Character Definition 2");
    return NULL;
  }

  //----------------------------
  // Load the animations associated with the anim set of the GameCharacterDef loaded from the exported network asset file
  // on disk.
  NMP_STDOUT("\nLoading Animation files for Character Definition 1:");
  if (!gameCharacterDef1->loadAnimationsFromDisk(ASSETLOCATION))
  {
    NMP_STDOUT("\nload Animations for Character Definition 1 returned error");
    return NULL;
  }

  NMP_STDOUT("\nLoading Animation files for Character Definition 2:");
  if (!gameCharacterDef2->loadAnimationsFromDisk(ASSETLOCATION))
  {
    NMP_STDOUT("\nload Animations for Character Definition 2 returned error");
    return NULL;
  }

  //----------------------------
  // Once we have a valid character definition we can print out information about the animation rig such as the number
  // of bones and bone names. Now that we have loaded all the data within the asset file we can also print out the
  // number of nodes in the network and other information such as the number of state machines.
  if (gameCharacterDef1->isLoaded())
  {
    NMP_STDOUT("\nPrinting data from GameCharacterDef1:");
    NMP_STDOUT("Number of Node Definitions: %d", gameCharacterDef1->getNetworkDef()->getNumNodeDefs());
    NMP_STDOUT("Number of State Machines: %d", gameCharacterDef1->getNetworkDef()->getNumStateMachines());
    NMP_STDOUT("Number of Control Parameters: %d", gameCharacterDef1->getNetworkDef()->getNumControlParameterNodes());
    NMP_STDOUT("\nList of bones in rig:");

    const MR::AnimRigDef* rig = gameCharacterDef1->getNetworkDef()->getRig(0);
    for (uint32_t i = 0; i < rig->getNumBones(); i++)
    {
      NMP_STDOUT("%s", rig->getBoneName(i));
    }
  }

  //----------------------------
  // Repeat the process for our second character definition.
  if (gameCharacterDef2->isLoaded())
  {
    NMP_STDOUT("\nPrinting data from GameCharacterDef2:");
    NMP_STDOUT("Number of Node Definitions: %d", gameCharacterDef2->getNetworkDef()->getNumNodeDefs());
    NMP_STDOUT("Number of State Machines: %d", gameCharacterDef2->getNetworkDef()->getNumStateMachines());
    NMP_STDOUT("Number of Control Parameters: %d", gameCharacterDef2->getNetworkDef()->getNumControlParameterNodes());
    NMP_STDOUT("\nList of bones in rig:");

    const MR::AnimRigDef* rig = gameCharacterDef2->getNetworkDef()->getRig(0);
    for (uint32_t i = 0; i < rig->getNumBones(); i++)
    {
      NMP_STDOUT("%s", rig->getBoneName(i));
    }
  }

  //----------------------------
  // We need an instance of a CharacterNoPhysics class that represents and wraps the runtime instance of a CharacterDef
  // class. We initialise the CharacterNoPhysics instance using the createCharacter() function in Game::CharacterManagerNoPhysics.
  // This class contains the runtime Network instance for a character that can be updated and run. Each character in a
  // game would have a unique instance of CharacterNoPhysics.
  // Multiple CharacterNoPhysics instances can reference the same CharacterDef. These characters will be of the same type but
  // will function completely independently.
  NMP_STDOUT("\nCreating GameCharacter1 from GameCharacterDef");
  Game::CharacterNoPhysics* gameCharacter1 = characterManager.createCharacter(gameCharacterDef1);
  if (!gameCharacter1)
  {
    NMP_STDOUT("\nError: Failed to create Character 1");
    return NULL;
  }

  NMP_STDOUT("\nCreating GameCharacter2 from GameCharacterDef");
  Game::CharacterNoPhysics* gameCharacter2 = characterManager.createCharacter(gameCharacterDef2);
  if (!gameCharacter2)
  {
    NMP_STDOUT("\nError: Failed to create Character 2");
    return NULL;
  }

  NMP_STDOUT("\nPerforming Updates:");

  //----------------------------
  // Our time difference between updates
  static const float timeDelta = 1.0f / 60.0f;

  //----------------------------
  // Move character 2 slightly so that we know it is working correctly
  NMP::Vector3 setPosition = NMP::Vector3(9.0f, 0.0f, -3.0f);
  Game::CharacterControllerNoPhysics* characterController1 = static_cast<Game::CharacterControllerNoPhysics*>(gameCharacter1->getNetwork()->getCharacterController());
  Game::CharacterControllerNoPhysics* characterController2 = static_cast<Game::CharacterControllerNoPhysics*>(gameCharacter2->getNetwork()->getCharacterController());

  if (!characterController1 || !characterController2)
  {
    NMP_STDOUT("\n Error: Failed to find character controllers.  Have they been initialised correctly?");
    return NULL;
  }

  characterController1->setPosition(setPosition);

  //----------------------------
  // Update i number of times to test the network update. Update would normally be called once per game frame.
  for (int32_t i = 0; i <= 1000; ++i)
  {
    characterManager.updateToPreCharacterController(timeDelta);
    characterManager.updateFinalise();
  }

  NMP_STDOUT("Updates Complete\n");

  //----------------------------
  // Output character positions - when in debug
  const NMP::Vector3 position1 = characterController1->getPosition();
  const NMP::Vector3 position2 = characterController2->getPosition();

  NMP_STDOUT("End Position of gameCharacter1: %.3fx, %.3fy, %.3fz", position1.x, position1.y, position1.z);
  NMP_STDOUT("End Position of gameCharacter2: %.3fx, %.3fy, %.3fz", position2.x, position2.y, position2.z);

  //----------------------------
  // When we are ready to exit we can terminate Game::CharacterManager to free any classes/memory it has stored.
  // Since Game::CharacterManager stores a pointer to the gameCharacterDef and gameCharacter instances it will
  // manage their termination as well.
  NMP_STDOUT("\nReleasing data:");
  characterManager.term();
  Game::AnimSystemModuleNoPhysics::term();
  NMP_STDOUT("Release complete");

  NMP_STDOUT("\nPress Enter to Exit");
  getchar();

  return 0;
}
//----------------------------------------------------------------------------------------------------------------------
