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
#include "GameManagement/GameCharacterManager.h"
#include "MyGameCharacter.h"
//----------------------------------------------------------------------------------------------------------------------

#define ASSETLOCATION "./ProjectData/" PLATFORMDIR
#define NETWORK_DEF_BUNDLE_NAME ASSETLOCATION "/Network_MultipleNetworks1.nmb"

//----------------------------------------------------------------------------------------------------------------------
// MultipleNetworks1:
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
  Game::CharacterManager characterManager;

  //---------------------------
  // Initialize morpheme.
  Game::AnimSystemModuleNoPhysics::init();
  characterManager.init();

  //----------------------------
  // We also need an instance of a Game::CharacterDef that will be initialised with the binary asset data. Using the
  // Game::MyWorld manager class we will initialise our Game::CharacterDef. This will load the file found at ASSETLOCATION 
  // and use that information to apply to the character definition instance.
  NMP_STDOUT("\nCreating Character Definition:");
  Game::CharacterDef* gameCharacterDef = characterManager.createCharacterDef(NETWORK_DEF_BUNDLE_NAME, NULL);
  if (!gameCharacterDef)
  {
    NMP_STDOUT("\nError: Failed to create Game Character Definition");
    return NULL;
  }
  
  //----------------------------
  // Load the animations associated with the anim set of the GameCharacterDef loaded from the exported network asset file
  // on disk.
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
  // We need to create an instance of a MyCharacter class that represents and wraps the runtime instance of a CharacterDef
  // class. We initialise the MyCharacter instance using the static create function in MyCharacter.
  // This class contains the runtime Network instance for a character that can be updated and run. Each character in a
  // game would have a unique instance of MyCharacter.
  // Multiple MyCharacter instances can reference the same CharacterDef. These characters will be of the same type but
  // will function completely independently.
  NMP_STDOUT("\nCreating GameCharacter1 from GameCharacterDef:");
  Game::MyCharacter* gameCharacter1 = Game::MyCharacter::create(gameCharacterDef);

  NMP_STDOUT("\nCreating GameCharacter2 from GameCharacterDef:");
  Game::MyCharacter* gameCharacter2 = Game::MyCharacter::create(gameCharacterDef);

  if(!gameCharacter1)
  {
    NMP_STDOUT("\nError: Failed to create Game Character 1");
    return NULL;
  }

  if(!gameCharacter2)
  {
    NMP_STDOUT("\nError: Failed to create Game Character 2");
    return NULL;
  }

  //----------------------------
  // Hand off management of the Characters we just created to Game::MyWorld.
  characterManager.registerCharacter(gameCharacter1);
  characterManager.registerCharacter(gameCharacter2);

  NMP_STDOUT("\nPerforming Updates:");

  //----------------------------
  // Our time difference between updates
  static const float timeDelta = 1.0f / 60.0f;

  //----------------------------
  // Move character 2 slightly so that we know it is working correctly
  NMP::Vector3 setPosition = NMP::Vector3(9.0f, 0.0f, -3.0f);
  Game::CharacterControllerNoPhysics* characterController = static_cast<Game::CharacterControllerNoPhysics*>(gameCharacter2->getNetwork()->getCharacterController());

  if (!characterController)
  {
    NMP_STDOUT("\n Error: Failed to find character controller.  Has it been initialised correctly?");
    return NULL;
  }

  characterController->setPosition(setPosition);

  //----------------------------
  // Update i number of times to test the network update. Update would normally be called once per game frame.
  for (int32_t i = 0; i <= 1000; ++i)
  {
    gameCharacter1->updateToPreCharacterController(timeDelta);
    gameCharacter2->updateToPreCharacterController(timeDelta);

    gameCharacter1->updateFinalise();
    gameCharacter2->updateFinalise();
  }

  NMP_STDOUT("Updates Complete\n");

  //----------------------------
  // Output character positions - when in debug
  const NMP::Vector3 position1 = gameCharacter1->getCharacterController()->getPosition();
  const NMP::Vector3 position2 = gameCharacter2->getCharacterController()->getPosition();

  NMP_STDOUT("End Position of gameCharacter1: %.3fx, %.3fy, %.3fz", position1.x, position1.y, position1.z);
  NMP_STDOUT("End Position of gameCharacter2: %.3fx, %.3fy, %.3fz", position2.x, position2.y, position2.z);

  //----------------------------
  // When we are ready to exit we can terminate MyGameAnimModule to free any classes/memory it has stored.
  // Since MyGameAnimModule stores a pointer to the gameCharacterDef and gameCharacter instances it will
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
