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

#ifdef _WIN64
#define PLATFORMDIR "x64"
#else
#define PLATFORMDIR "win32"
#endif

#define ASSETLOCATION "./ProjectData/" PLATFORMDIR
#define NETWORK_DEF_BUNDLE_NAME ASSETLOCATION "/Network_RuntimeBasic1.nmb"

//----------------------------------------------------------------------------------------------------------------------
// RuntimeBasic1: This tutorial demonstrates how to load a morpheme network that has been exported and processed from
// morpheme:connect (See morpheme:connect documentation for details on exporting morpheme networks). The animation rig
// is extracted from the binary data and then the number of bones and their names are printed to the console. Note that
// running in release mode will not print any information to console by default.
int main(int NMP_UNUSED(argc), char** NMP_UNUSED(argv))
{
  //----------------------------
  // This program defaults to loading the "Network" from the ProjectData folder. The animation files will 
  // also be loaded from this location; see Game::AssetLoader::requestAnim() (in later tutorials) for more details.
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
  // Once we have a valid character definition we can print out information about the animation rig such as listing all
  // the bones present in the rig.
  if (gameCharacterDef->isLoaded())
  {
    NMP_STDOUT("\nList of bones in rig:");

    const MR::AnimRigDef* rig = gameCharacterDef->getAnimRigDef();
    for (uint32_t i = 0; i < rig->getNumBones(); i++)
    {
      NMP_STDOUT("%s", rig->getBoneName(i));
    }
  }

  //----------------------------
  // When we are ready to exit we can terminate CharacterManager to free any classes/memory it has stored.
  // Since gameAnimModule stores a pointer to the gameCharacterDef instance it will manage its termination as well.
  NMP_STDOUT("\nReleasing data:");
  characterManager.termMorpheme();
  NMP_STDOUT("Release complete");

  NMP_STDOUT("\nPress Enter to Exit");
  getchar();

  return 0;
}
//----------------------------------------------------------------------------------------------------------------------
