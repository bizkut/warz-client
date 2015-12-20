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
#ifndef GAME_CHARACTER_MANAGER_H
#define GAME_CHARACTER_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "GameAnimModule.h"
#include "GameCharacter.h"
#include "GameCharacterDef.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
/// \class Game::CharacterManager
///
/// Provides a simplified interface to managing and updating a set of morpheme characters.
/// It is an example of how you could manage the morpheme animation system within your application.
/// It manages lists of characters and character definitions wrapping many morpheme API calls into conceptually simplified methods.
/// It includes the registration of character definitions and characters as well as management of their subsequent use.
///
/// This is a base class that can be used as is, but it is also inherited from to support specific physics skus.
//----------------------------------------------------------------------------------------------------------------------
class CharacterManager
{
public:
    
  CharacterManager();
  virtual ~CharacterManager();

  /// \brief Allocate required memory and initialise
  virtual void init();

  /// \brief Free allocated memory and shutdown
  virtual void term();
  
  //----------------------------
  // CharacterDef management functions.
  //----------------------------

  /// \brief Create and initialise a CharacterDef; loading its assets a specified simple bundle source file.
  ///
  /// CharacterDefs that are created via the Manager are automatically registered with the manager
  ///  and the memory management of the Character remains the responsibility of the Manager.
  CharacterDef* createCharacterDef(
    const char*          filename,            ///< Name of simple bundle file where we should try and load this characters assets from.
    ValidatePluginListFn validatePluginList); ///< Pointer to function that verifies that the bundle was built with the expected
                                              ///<  asset compiler plugins and that they are specified in the correct order.

  /// \brief Create CharacterDef and initialise it from the simple bundle assets that have been loaded in to the specified memory block.
  ///
  /// CharacterDefs that are created via the Manager are automatically registered with the manager
  ///  and the memory management of the Character remains the responsibility of the Manager.
  CharacterDef* createCharacterDef(
    void*                bundle,              ///< Block of memory that holds this characters simple bundle assets.
    size_t               bundleSize,          ///< Memory block size.
    ValidatePluginListFn validatePluginList); ///< Pointer to function that verifies that the bundle was built with the expected
                                              ///<  asset compiler plugins and that they are specified in the correct order.

  /// \brief Removes the pointer to a characterDef stored in Game::CharacterManager and destroys the CharacterDef.
  void destroyCharacterDef(CharacterDef* characterDef);

  /// \brief Stores a pointer to a characterDef. CharacterManager will now be responsible for management of this CharacterDefs memory.
  ///
  /// Do not delete this CharacterDef independent of the CharacterManager.
  void registerCharacterDef(CharacterDef* characterDef);

  /// \brief Removes the pointer to a characterDef stored in Game::CharacterManager.
  void unregisterCharacterDef(CharacterDef* characterDef);

  /// \brief Try and find a registered ChracterDef with this GUID.
  CharacterDef* findCharacterDefByGUID(const uint8_t guid[16]);

  /// \brief Registered CharacterDef count.
  uint32_t getNumRegisteredCharacterDefs()  { return (uint32_t) m_characterDefList.size(); }

  /// \brief Get the registered CharacterDefList.
  const CharacterDefList& getCharacterDefList() const { return m_characterDefList; }

  /// \brief Accessing by index in not a reliable method of addressing.
  ///  Entries are stored in a list so indexing can change.
  CharacterDef* getCharacterDef(uint32_t index);

  //----------------------------
  // Character management functions.
  //----------------------------

  /// \brief Unregisters the character instance and destroys it.
  ///   
  /// There is no corresponding create function as this is specific to particular skus 
  /// i.e. no physics and specfic physics engines.
  /// These specialised create functions are implemented in inherited managers.
  void destroyCharacter(Character* character);

  /// \brief Pass a pointer to a Character to the CharacterList where it is stored.
  void registerCharacter(Character* character);

  /// \brief Removes a pointer to a Character from the CharacterList.
  void unregisterCharacter(Character* character);
   
  /// \brief Get the registered CharacterList.
  const CharacterList& getCharacterList() const { return m_characterList; }
    
  /// \brief Registered Character count.
  uint32_t getNumRegisteredCharacters() { return (uint32_t) m_characterList.size(); }

  /// \brief Accessing by index in not a reliable method of addressing.
  ///
  ///  Entries are stored in a list so indexing can change.
  Character* getCharacter(uint32_t index);
  
  //----------------------------
  // Profiling.
  //----------------------------

  /// \brief Start a PIX trace.  Does nothing unless the relevant macros are enabled.
  void startProfiling();

  /// \brief End the PIX trace and output profiling log.  Does nothing unless the relevant macros are enabled.
  void endProfiling();

protected:

  /// \brief Is the Character already registered.
  bool isCharacterRegistered(Character* character) const;

  /// \brief Is the CharacterDef already registered.
  bool isCharacterDefRegistered(CharacterDef* characterDef) const;
  
  /// A list of all the initialised CharacterDefs.
  CharacterDefList    m_characterDefList;

  /// A list of all the initialised Character instances.
  CharacterList       m_characterList;
};

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_MANAGER_H
//----------------------------------------------------------------------------------------------------------------------
