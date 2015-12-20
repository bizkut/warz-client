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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef GAME_CHARACTER_MANAGER_H
#define GAME_CHARACTER_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "GameCharacterDef.h"
#include "GameCharacter.h"
#include "NMPlatform/NMPlatform.h"
#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookup.h"
//----------------------------------------------------------------------------------------------------------------------

#ifdef _WIN64
#define PLATFORMDIR "x64"
#else
#define PLATFORMDIR "win32"
#endif

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
// The Game::GameCharacterManager class is an example of how you could manage the morpheme animation system within your application.
// it would hold responsibility for managing all the functions required to run a morpheme network by wrapping many of
// morpheme api calls into conceptually simplified methods.
class CharacterManagerBasic
{
public:
  
  CharacterManagerBasic() {};
  virtual ~CharacterManagerBasic() {};

  //----------------------------
  // Initialise function for Game::CharacterManager. Any core morpheme or animation specific initialisation
  // should occur in this functions.
  void initMorpheme();

  //----------------------------
  // Free allocated memory and shutdown
  void termMorpheme();

  //----------------------------
  // Create and initialise a CharacterDef; loading its assets a specified simple bundle source file.
  //
  // CharacterDefs that are created via the Manager are automatically registered with the manager
  //  and the memory management of the Character remains the responsibility of the Manager.
  CharacterDefBasic* createCharacterDef(const char* filename );  //< Name of simple bundle file where we should try and load this characters assets from.

  //----------------------------
  // Stores a pointer to a characterDef. This allows Game::GameCharacterManager to manage this character definition.
  void registerCharacterDef(CharacterDefBasic* characterDef);

  //----------------------------
  // Pass a pointer to a Character to the CharacterList where it is stored.
  void registerCharacter(CharacterBasic* character);

  //----------------------------
  // Update any networks that are available
  void update(float timeDelta);

protected:

  CharacterDefBasic* m_characterDef;   // Store a pointer to a morpheme character definition
  CharacterBasic*    m_characterData;  // Store a pointer to a morpheme character
};


}
//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
