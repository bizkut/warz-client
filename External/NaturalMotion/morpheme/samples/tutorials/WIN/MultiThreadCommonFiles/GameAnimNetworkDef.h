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
#ifndef GAME_ANIM_NETWORK_DEF_H
#define GAME_ANIM_NETWORK_DEF_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/connection.h"
#include "morpheme/mrNetworkDef.h"
#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookup.h"

#ifdef NM_EUPHORIA_TUTORIAL
#include "euphoria/erAttribData.h"
#include "euphoria/erCharacterDef.h"
#endif
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \class GameAnimNetworkDef
/// \brief This is a simple wrapper around the morpheme network definition and adds utilities such as animation loading.
//----------------------------------------------------------------------------------------------------------------------
class GameAnimNetworkDef
{
public:

  /// \brief Creates a network definition from the given bundle.
  static GameAnimNetworkDef* create(void* bundle, size_t bundleSize);

  /// \brief Create a network definitions from the bundle stored in the given file.
  static GameAnimNetworkDef* create(const char* filename);

  virtual ~GameAnimNetworkDef();

public:

  /// \brief Returns the maximum number of bones used by any of the animation sets in this network.
  uint32_t getMaxBoneCount() const { return m_networkDef->getMaxBoneCount(); }

  /// \brief Loads the animations of all animation sets.
  bool loadAnimations();

  /// \brief Loads the animations of all animation sets over the given connection.
  bool loadAnimationsOverConnection(MCOMMS::Connection* connection);

  /// \brief Unloads the animations of all animation sets.
  bool unloadAnimations();

public:

  MR::NetworkDef* getNetworkDef() const { return m_networkDef; }
  const uint8_t* getNetworkDefGuid() const { return m_networkDefGuid; }

#ifdef NM_EUPHORIA_TUTORIAL
  const ER::CharacterDef* getCharacterDef() const {return m_characterDef;}
  ER::CharacterDef* getCharacterDef() {return m_characterDef;}
#endif

#pragma region LiveLink
public:

  /// \brief Publishes this network definition to LiveLink.
  void publish();
#pragma endregion LiveLink

private:

  MR::NetworkDef*   m_networkDef;
  uint8_t           m_networkDefGuid[16];

  uint32_t          m_numRegisteredAssets;
  uint32_t*         m_registeredAssetIDs;
  uint32_t          m_numClientAssets;
  void**            m_clientAssets;

  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup*   m_animFileLookup;

#ifdef NM_EUPHORIA_TUTORIAL
  ER::CharacterDef*  m_characterDef;
#endif

  GameAnimNetworkDef();
};

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_ANIM_NETWORK_DEF_H
//----------------------------------------------------------------------------------------------------------------------
