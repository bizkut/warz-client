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
#include "GameCharacterManager.h"
#include "GameCharacterDef.h"

#define ASSETLOCATION "./ProjectData/" PLATFORMDIR 
#define NETWORK_DEF_BUNDLE_NAME ASSETLOCATION "/Network_RuntimeBasic2.nmb"

//----------------------------------------------------------------------------------------------------------------------
// RuntimeBasic2: This tutorial continues from RuntimeBasic1 by loading all the data contained within
// binary asset file (exported from morpheme; connect) and not just the animation rig. This means we can start reading
// information about the network such as the number of nodes and state machines. This tutorial also loads the animation
// files listed within the asset file/character definition but does not run them.
int main(int NMP_UNUSED(argc), char** NMP_UNUSED(argv))
{
  //----------------------------
  // This program defaults to loading the "Network" from the ProjectData folder. The animation files will 
  // also be loaded from this location; see Game::AnimLoader::requestAnim() for more details.
  NMP_STDOUT("Defaulting <network filename> to : '%s'", NETWORK_DEF_BUNDLE_NAME);

  //----------------------------
  // Game::CharacterManagerBasic which will act as a manager for all the morpheme network definitions and
  // network instances. Initialise will set up the morpheme libraries and load all the default node types
  // and other default functions.
  Game::CharacterManagerBasic characterManager;

  //----------------------------
  // Initialise will set up the morpheme libraries and load all the default node types
  // and other default functions.
  characterManager.initMorpheme();

  //----------------------------
  // We also need an instance of a Game::CharacterDef that will be initialised with the binary asset data. Using the
  // CharacterManager manager class we will initialise our CharacterDef. This will load the file found at  
  // NETWORK_DEF_BUNDLE_NAME and use that information to apply to the character definition instance.
  NMP_STDOUT("\nCreating GameCharacterDef:");
  Game::CharacterDefBasic* gameCharacterDef = characterManager.createCharacterDef(NETWORK_DEF_BUNDLE_NAME);
  if(!gameCharacterDef)
  {
    NMP_STDOUT("\nError: Failed to create Game Character Definition");
    return NULL;
  }

  //----------------------------
  // Load the animations listed in the GameCharacterDef loaded from the exported network asset file. We will just
  // load the tutorials into memory in this tutorial.
  NMP_STDOUT("\nLoading Animation files:");
  if (!gameCharacterDef->loadAnimations())
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

    const MR::AnimRigDef* rig = gameCharacterDef->getAnimRigDef();
    for (uint32_t i = 0; i < rig->getNumBones(); i++)
    {
      NMP_STDOUT("%s", rig->getBoneName(i));
    }

    NMP_STDOUT("");
  }

  //----------------------------
  // When we are ready to exit we can terminate the CharacterManager to free any classes/memory it has stored.
  // Since CharacterManager stores a pointer to the gameCharacterDef instance it will manage its termination as well.
  NMP_STDOUT("\nReleasing data:");
  characterManager.termMorpheme();
  NMP_STDOUT("Release complete");

  NMP_STDOUT("\nPress Enter to Exit");
  getchar();

  return 0;
}
//----------------------------------------------------------------------------------------------------------------------
