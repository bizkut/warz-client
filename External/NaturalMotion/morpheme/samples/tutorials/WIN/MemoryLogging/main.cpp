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
#define NETWORK_DEF_BUNDLE_NAME ASSETLOCATION "/Network_MemoryLogging.nmb"

//----------------------------------------------------------------------------------------------------------------------
// MemoryLogging: This example extends RuntimeBasic3 to track memory usage. Code has been added to the
// Game::Character class to output the highest temporary memory usage seen by the temporary allocator when
// NMP_MEMORY_LOGGING has been defined in NMMemory.h. This is not done by default in this tutorial. For more information,
// see the following section of the user guide: Using runtime > Analysing memory usage
int32_t main(int32_t NMP_UNUSED(argc), char** NMP_UNUSED(argv))
{
  //----------------------------
  // This program defaults to loading the "Network" from the ProjectData folder. The animation files will 
  // also be loaded from this location; see Game::AnimLoader::requestAnim() for more details.
  NMP_STDOUT("Defaulting <network1 filename> to : '%s'", NETWORK_DEF_BUNDLE_NAME);

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
  // Output memory log messages if memory logging is enabled.
#ifdef NMP_MEMORY_LOGGING
  // Requirements for the network instance
  NET_LOG_ADD_PRIORITY_RANGE(4, 4);
#endif

  //----------------------------
  // We also need an instance of a CharacterDef that will be initialised with the binary asset data. Using the
  // Game::CharacterManager manager class we will initialise our CharacterDef. This will load the file found at 
  // ASSETLOCATION and use that information to apply to the character definition instance.
  NMP_STDOUT("\nCreating GameCharacterDef:");
  Game::CharacterDef* gameCharacterDef = characterManager.createCharacterDef(NETWORK_DEF_BUNDLE_NAME, NULL);
  if(!gameCharacterDef)
  {
    NMP_STDOUT("\nError: Failed to create Game Character Definition");
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
  // We need an instance of a Game::Character class that represents and wraps the runtime instance of a Game::CharacterDef
  // class. We initialise the Game::Character instance data using the create function in MyGameCharacter.
  // This class contains the runtime network instance for a character that can be updated and run. Each character in a
  // game would have a Game::Character class instance for updating its unique instance of a character definition.
  // These characters will be identical but fully independent.
  NMP_STDOUT("\nCreating GameCharacter from GameCharacterDef:"); 
  Game::MyCharacter* gameCharacter = Game::MyCharacter::create(gameCharacterDef);
  if(!gameCharacter)
  {
    NMP_STDOUT("\nError: Failed to create Game Character");
    return NULL;
  }

  //----------------------------
  // Hand off management of the gameCharacter we just created to Game::CharacterManager.
  characterManager.registerCharacter(gameCharacter);

  NMP_STDOUT("\nPerforming Updates:");

  //----------------------------
  // Our time difference between updates
  static const float timeDelta = 1.0f / 60.0f;

  //----------------------------
  // Update i number of times to test the network update. Update would normally be called once per game frame.
  for (uint32_t i = 0; i <= 1000; ++i)
  {
    gameCharacter->updateToPreCharacterController(timeDelta);
    gameCharacter->updateFinalise();    
  }

  NMP_STDOUT("Updates Complete");

  //----------------------------
  // When we are ready to exit we can terminate the Game::CharacterManager to free any classes/memory it has stored.
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
