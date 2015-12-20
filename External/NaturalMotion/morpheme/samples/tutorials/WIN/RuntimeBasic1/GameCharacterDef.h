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
#ifndef GAME_CHARACTER_DEF_H
#define GAME_CHARACTER_DEF_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrDispatcher.h"
#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookup.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
// Game::CharacterDef is a wrapper class for a morpheme 'network' or 'character' definition. Each instance of
// CharacterDef should contain a unique set of definitions of which to create network/character instances from. In a
// game scenario CharacterDef could also be used for other character definition data (model/texture assets for
// example).
//----------------------------------------------------------------------------------------------------------------------
class CharacterDefBasic
{
public:

  virtual ~CharacterDefBasic() {};

  //----------------------------
  // Static method to load the network file (a SimpleBundle), relocate the objects and pointers it contains, and register
  // them with morpheme.
  static CharacterDefBasic* create(const char* filename);///< Name of simple bundle file where we should try and load this characters assets from.

  //----------------------------
  // term and free a game character def instance
  static bool destroy(CharacterDefBasic* characterDef);

  //----------------------------
  // Accessors
  //----------------------------

  bool  isLoaded()  const { return m_isLoaded; }

  int64_t         getBundleSize() const { return m_bundleSize; }
  MR::AnimRigDef* getAnimRigDef() const { return m_animRigDef; }

protected:

  CharacterDefBasic():
    m_isLoaded(false),
    m_bundle(NULL),
    m_bundleSize(0),
    m_animRigDef(NULL)
  {
  };

  //----------------------------
  // Initialise and terminate an instance of CharacterDef.
  bool init();

  //----------------------------
  // Free memory
  bool term();

protected:

  bool m_isLoaded;

  void*    m_bundle;      // We are holding onto the bundle information so that it can be easily released on term()
  int64_t  m_bundleSize;

  MR::AnimRigDef* m_animRigDef;
};

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_DEF_H
//----------------------------------------------------------------------------------------------------------------------
