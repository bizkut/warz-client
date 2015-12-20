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
#include "physics/mrPhysicsRigDef.h"
#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookup.h"

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
  // the gameCharacterDef is stored in this Game::CharacterManager for management.
  static CharacterDefBasic* create(const char* filename);

  //----------------------------
  // term and free a game character def instance
  static bool destroy(CharacterDefBasic* characterDef);

  //----------------------------
  // Load the animation files from the list of file names in gameCharacterDef->getAnimFileLookUp(). These are unloaded
  // when this CharacterDef is terminated.
  bool loadAnimations();

  //----------------------------
  // Accessors
  bool  isLoaded()  const { return m_isLoaded; }

  MR::NetworkDef* getNetworkDef() const { return m_netDef; }

  MR::CharacterControllerDef* getSampleCharCtrl() const { return m_SampleCharCtrl; }

  uint32_t* getRegisteredAssetIDs()  const { return m_registeredAssetIDs; }
  void**    getClientAssets()        const { return m_clientAssets; }
  uint32_t  getNumRegisteredAssets() const { return m_numRegisteredAssets; }
  uint32_t  getNumClientAssets()     const { return m_numClientAssets; }

  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* getAnimFileLookUp() const { return m_animFileLookUp; }

protected:

  CharacterDefBasic():
    m_isLoaded(false),
    m_netDef(NULL),
    m_SampleCharCtrl(NULL),
    m_animFileLookUp(NULL),
    m_registeredAssetIDs(NULL),
    m_clientAssets(NULL),
    m_numRegisteredAssets(0),
    m_numClientAssets(0)
  {
  };

  //----------------------------
  // Initialise and terminate an instance of CharacterDef. The initialisation function takes in the information
  // required to create a network/character instance
  bool init(void* bundle, size_t bundleSize);

  //----------------------------
  // Releases any memory allocated in this class.
  bool term();

protected:

  bool  m_isLoaded;

  MR::NetworkDef* m_netDef;

  MR::CharacterControllerDef*                     m_SampleCharCtrl;

  uint32_t* m_registeredAssetIDs;
  void**    m_clientAssets;
  uint32_t  m_numRegisteredAssets;
  uint32_t  m_numClientAssets;

  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* m_animFileLookUp;
};


} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_DEF_H
//----------------------------------------------------------------------------------------------------------------------
