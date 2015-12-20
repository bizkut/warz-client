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
#include "morpheme/mrNetworkRestorePoint.h"
//----------------------------------------------------------------------------------------------------------------------

#define ASSETLOCATION "./ProjectData/" PLATFORMDIR
#define NETWORK_DEF_BUNDLE_NAME ASSETLOCATION "/NetworkRestorePoint.nmb"

//----------------------------------------------------------------------------------------------------------------------
// NetworkRestorePoint: This example demonstrates how to use a network restore point.
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
  // This also registers the definition with our manager. Management of this definitions memory is the responsibility of the characterManager.
  NMP_STDOUT("\nCreating CharacterDef:");
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
    NMP_STDOUT("\nload Animations for Character Definition returned error");
    return NULL;
  }

  //----------------------------
  // We need an instance of a Game::Character class that represents and wraps the runtime instance of a CharacterDef
  // class. We initialise the Character instance data using the static Game::Character::create() function.
  // This class contains the runtime network instance for a character that can be updated and run. Each character in a
  // game would have a Character class instance for updating its unique instance of a character definition.
  NMP_STDOUT("\nCreating Character from CharacterDef:");
  Game::CharacterNoPhysics* gameCharacter = characterManager.createCharacter(gameCharacterDef);
  if(!gameCharacter)
  {
    NMP_STDOUT("\nError: Failed to create Character 1");
    return NULL;
  }
  MR::Network* gameCharacterNetwork = gameCharacter->getNetwork();

  NMP_STDOUT("\nPerforming Updates:");

  //----------------------------
  // Our time difference between updates
  static const float timeDelta = 1.0f / 60.0f;

  //----------------------------
  // Update i number of times to test the network update. Update would normally be called once per game frame.
  MR::NetworkRestorePoint* restorePoint = NULL;
  for (int i = 0; i <= 200; ++i)
  {
    characterManager.updateToPreCharacterController(timeDelta);
    characterManager.updateFinalise();

    if (i == 50)
    {
      NMP_STDOUT("\nCreating restore point: ");

      //----------------------------
      // Create a network restore point. The restore point allows us to return any network that uses the same network
      // definition to the current state of the network.
      restorePoint = MR::NetworkRestorePoint::createAndInit(gameCharacterNetwork);

      NMP_STDOUT("Restore point created using %i bytes", restorePoint->getMemoryUsage().size);

      //----------------------------
      // If we wanted to save the restore point to disk or send it across a connection it would need be dislocated first.
      // restorePoint->dislocate(true);

      // Once the network is loaded or received it must be located before it can be used.
      // restorePoint->locate(gameCharacterNetwork->getNetworkDef(), true);
    }

    if (i == 150)
    {
      NMP_STDOUT("\nRestoring network:");

      //----------------------------
      // Restore the network to the state it was in when the restore point was created. Note that the restore point could
      // be used again to restore this or another network.
      restorePoint->applyToNetwork(gameCharacterNetwork);

      NMP_STDOUT("Network restored");
    }
  }

  NMP_STDOUT("\nUpdates Complete");

  // Release the restore point we created
  NMP::Memory::memFree(restorePoint);

  //----------------------------
  // When we are ready to exit we can terminate the GameAnimModule to free any classes/memory it has stored.
  // Since gameAnimModule stores a pointer to the gameCharacterDef and gameCharacter instances it will
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
