// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
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
#ifndef GAME_CHARACTER_DEF_EUPHORIA_H
#define GAME_CHARACTER_DEF_EUPHORIA_H
//----------------------------------------------------------------------------------------------------------------------
#include "GameManagement/GameCharacterDef.h"
#include "euphoria/erCharacterDef.h"
#include "euphoria/erNetworkInterface.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
/// \class Game::CharacterEuphoriaDef
///
/// Extends CharacterDef to add in the Euphoria character definition and the Euphoria behaviour library.
//----------------------------------------------------------------------------------------------------------------------
class CharacterDefEuphoria : public CharacterDef
{
public:

  virtual ~CharacterDefEuphoria() {};

  //----------------------------
  // Creation and destruction.
  //----------------------------

  /// \brief Create a CharacterDefEuphoria from the asset data in the file specified.
  static CharacterDefEuphoria* create(
    const char*          filename,                   ///< Name of simple bundle file where we should try and load this characters assets from.
    ValidatePluginListFn validatePluginList = NULL); ///< Pointer to function that verifies that the bundle was built with the expected
                                                     ///<  asset compiler plugins and that they are specified in the correct order.

  /// \brief Create a CharacterDefEuphoria from the asset data in the bundle specified.
  static CharacterDefEuphoria* create(
    void*                bundle,                     ///< Block of memory that holds this characters simple bundle assets.
    size_t               bundleSize,                 ///< Memory block size.
    ValidatePluginListFn validatePluginList = NULL); ///< Pointer to function that verifies that the bundle was built with the expected
                                                     ///<  asset compiler plugins and that they are specified in the correct order.
  
  //----------------------------
  // Accessors
  ER::CharacterDef* getEuphoriaCharacterDef() {return &m_euphoriaCharacterDef;}
  ER::NetworkInterface* getEuphoriaNetworkInterface() {return m_euphoriaNetworkInterface;}

protected:

  CharacterDefEuphoria():
    CharacterDef(),
    m_euphoriaNetworkInterface(NULL)
  {
  };

   /// \brief Initialise an instance of a GameCharacterDef from a bundle loaded in to a block of memory. 
  bool init(
    void*                bundle,              ///< Block of memory that holds this characters simple bundle assets.
    size_t               bundleSize,
    ValidatePluginListFn validatePluginList); ///< Pointer to function that verifies that the bundle was built with the expected
                                              ///<  asset compiler plugins and that they are specified in the correct order.

  /// \brief Terminate an instance of a GameCharacterDef, releasing any memory allocated in this class.
  virtual void term() NM_OVERRIDE;

protected:
  ER::CharacterDef      m_euphoriaCharacterDef;
  ER::NetworkInterface* m_euphoriaNetworkInterface;
};

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_DEF_EUPHORIA_H
//----------------------------------------------------------------------------------------------------------------------
