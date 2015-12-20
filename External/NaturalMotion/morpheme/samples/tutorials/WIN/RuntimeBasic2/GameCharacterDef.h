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
class CharacterDefBasic
{
public:
  
  virtual ~CharacterDefBasic() {};

  //----------------------------
  // Static method to load the network file (a SimpleBundle), relocate the objects and pointers it contains, and register
  // them with morpheme. This information is then stored in the CharacterDef instance that is passed in - A pointer to
  // the gameCharacterDef is stored in this Game::World for management.
  static CharacterDefBasic* create(const char* filename);

  //----------------------------
  // term and free a game character def instance
  static bool destroy(CharacterDefBasic* characterDef);

  //----------------------------
  // Load the animation files from the list of file names in characterDef->getAnimFileLookUp(). These are unloaded
  // when this Game::CharacterDef is terminated
  bool loadAnimations();

  //----------------------------
  // Accessors
  bool  isLoaded()  const { return m_isLoaded; }
  void* getBundle() const { return m_bundle; }

  int64_t         getBundleSize() const { return m_bundleSize; }
  MR::NetworkDef* getNetworkDef() const { return m_netDef; }
  MR::AnimRigDef* getAnimRigDef() const { return m_animRigDef; }

  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* getAnimFileLookUp() const { return m_animFileLookUp; }

protected:

  CharacterDefBasic():
    m_isLoaded(false),
    m_bundle(NULL),
    m_bundleSize(0),
    m_netDef(NULL),
    m_animRigDef(NULL),
    m_animFileLookUp(NULL)
  {
  };

  //----------------------------
  // Initialise required data
  bool init();

  //----------------------------
  // Free memory
  bool term();

protected:

  bool m_isLoaded;

  void*     m_bundle;       // We are holding onto the bundle information so that it can be easily released on term()
  int64_t   m_bundleSize;

  MR::NetworkDef* m_netDef;

  MR::AnimRigDef* m_animRigDef;
  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* m_animFileLookUp;
};

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_DEF_H
//----------------------------------------------------------------------------------------------------------------------
