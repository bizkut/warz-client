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

#include "simpleBundle/simpleBundle.h"
#include "NMPlatform/NMFile.h"
#include "morpheme/Nodes/mrNodes.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
// Initialise CharacterManager.
void CharacterManagerBasic::initMorpheme()
{
  NMP_STDOUT("\nInitialising Game::CharacterManagerBasic:");

  NMP::Memory::init();

  //----------------------------
  // Initialise morpheme library 
  NMP_STDOUT("Initialising Morpheme Library");
  MR::Manager::initMorphemeLib();

  NMP_STDOUT("Initialisation of Game::CharacterManagerBasic complete");
}

//----------------------------------------------------------------------------------------------------------------------
// Unload any data stored in this Game::CharacterManagerBasic, including Game::CharacterDefs that have been loaded and 
// unload the memory they store.
//
// Note that this isn't exactly robust - if we load bundle A and bundle B, and both contain the same rig, the rig will
// be registered only once (by A), but will be unregistered when B is torn down. If A is not immediately torn down it
// will reference the rig that has just been unregistered. Since the pointer is still valid this will not immediately
// fail, but may introduce subtle bugs. To get around this, either load and unload all your network assets at the same
// time, or use a ref counting scheme to register and unregister assets. See the Multiple Network tutorials for more
// details.
void CharacterManagerBasic::termMorpheme()
{
  if (m_characterDef->isLoaded())
  {
    NMP_STDOUT("Releasing gameCharacterDef");

    //----------------------------
    // Once we've finished with the binary file release it.
    CharacterDefBasic::destroy(m_characterDef);
  }

  //----------------------------
  // Terminate morpheme library
  MR::Manager::termMorphemeLib();
  NMP::Memory::shutdown();
}

//----------------------------------------------------------------------------------------------------------------------
// Creates a CharacterDef and registers it with the manager.
CharacterDefBasic* CharacterManagerBasic::createCharacterDef(const char* filename)
{
  Game::CharacterDefBasic* gameCharacterDef = Game::CharacterDefBasic::create(filename);
  if (!gameCharacterDef || !gameCharacterDef->isLoaded())
  {
    NMP_STDOUT("gameCharacterDef %s was not loaded successfully.", filename);
    NMP_ASSERT_FAIL();
    return NULL;
  }
  registerCharacterDef(gameCharacterDef); // The management of this gameCharacterDefs memory is is this CharacterManagers responsibility.
  return gameCharacterDef;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerBasic::registerCharacterDef(CharacterDefBasic* characterDef)
{
  NMP_STDOUT("\nRegistering GameCharacterDef with CharacterManager");

  if (!characterDef)
  {
    NMP_DEBUG_MSG("error: Valid character definition expected!");
    return;
  }

  //----------------------------
  // store a pointer to the character for use later
  m_characterDef = characterDef;
}

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
