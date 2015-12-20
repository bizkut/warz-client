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
#ifndef GAME_CHARACTER_DEF_H
#define GAME_CHARACTER_DEF_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/connection.h"
#include "morpheme/mrDispatcher.h"
#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookup.h"
#include "GameManagement/GameAssetLoader.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
/// \class Game::CharacterDef
/// \brief A wrapper class for a morpheme NetworkDef or 'Character' definition.
///
/// Each instance of CharacterDef should contain a unique set of definitions from which to create Network/Character 
/// instances from. In a game scenario GameCharacterDef could also be used for other character definition data
/// (model/texture assets for example).
//----------------------------------------------------------------------------------------------------------------------
class CharacterDef
{
public:

  virtual ~CharacterDef() {};

  //----------------------------
  // Creation and destruction.
  //----------------------------

  /// \brief Create a CharacterDef from the asset data in the file specified.
  static CharacterDef* create(
    const char*          filename,                   ///< Name of simple bundle file where we should try and load this characters assets from.
    ValidatePluginListFn validatePluginList = NULL); ///< Pointer to function that verifies that the bundle was built with the expected
                                                     ///<  asset compiler plugins and that they are specified in the correct order.

  /// \brief Create a CharacterDef from the asset data in the bundle specified.
  static CharacterDef* create(
    void*                bundle,                     ///< Block of memory that holds this characters simple bundle assets.
    size_t               bundleSize,                 ///< Memory block size.
    ValidatePluginListFn validatePluginList = NULL); ///< Pointer to function that verifies that the bundle was built with the expected
                                                     ///<  asset compiler plugins and that they are specified in the correct order.
  
  /// \brief Terminate and free a game CharacterDef instance.
  static bool destroy(CharacterDef* characterDef);

  //----------------------------
  // Animation management.
  //----------------------------

  /// \brief Loads all animations used by this Network from the file system.
  bool loadAnimationsFromDisk(const char* animLocation);
    
  /// \brief Loads all animations used by this Network over the given connection.
  bool loadAnimationsOverConnection(MCOMMS::Connection* connection);
    
  /// Unloads all animations used by this Network.
  bool unloadAnimations();

  //----------------------------
  // Accessors
  bool isLoaded() const { return m_isLoaded; }

  MR::NetworkDef* getNetworkDef() const { return m_netDef; }

  /// \return Pointer to a 16 byte value.
  const uint8_t* getNetworkDefGuid() const { return m_netDefGuid; }
  uint32_t getMaxBoneCount() const { return m_netDef->getMaxBoneCount(); }

  MR::NetworkPredictionDef* getNetworkPredictionDef() const { return m_netPredictionDef; }
  uint32_t* getRegisteredAssetIDs() const { return m_registeredAssetIDs; }
  void** getClientAssets() const { return m_clientAssets; }
  uint32_t getNumRegisteredAssets() const { return m_numRegisteredAssets; }
  uint32_t getNumClientAssets() const { return m_numClientAssets; }

  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* getAnimFileLookUp() const { return m_netDefSpecificAnimIDtoFilenameLookup; }
 
  NM_INLINE MR::NodeID getNodeID(const char* nodeName);
  NM_INLINE MR::MessageID getMessageID(const char* messageName);
  NM_INLINE MR::StateID getStateID(const char* stateName);


  uint32_t decRefCount() { NMP_ASSERT(m_refCount > 0); return --m_refCount; }
  void incRefCount() { ++m_refCount; }
  uint32_t getRefCount() { return m_refCount; }

protected:

  CharacterDef():
    m_isLoaded(false),
    m_netDef(NULL),
    m_netPredictionDef(NULL),
    m_registeredAssetIDs(NULL),
    m_clientAssets(NULL),
    m_numRegisteredAssets(0),
    m_numClientAssets(0),
    m_netDefSpecificAnimIDtoFilenameLookup(NULL)
  {
  };

  /// \brief Initialise an instance of a GameCharacterDef from a bundle loaded in to a block of memory. 
  bool init(
    void*                bundle,              ///< Block of memory that holds this characters simple bundle assets.
    size_t               bundleSize,
    ValidatePluginListFn validatePluginList); ///< Pointer to function that verifies that the bundle was built with the expected
                                              ///<  asset compiler plugins and that they are specified in the correct order.

  /// \brief Terminate an instance of a GameCharacterDef, releasing any memory allocated in this class.
  virtual void term();
  
protected:

  bool                      m_isLoaded;
  MR::NetworkDef*           m_netDef;
  uint8_t                   m_netDefGuid[16];
  
  MR::NetworkPredictionDef* m_netPredictionDef;

  uint32_t*                 m_registeredAssetIDs;
  void**                    m_clientAssets;
  uint32_t                  m_numRegisteredAssets;
  uint32_t                  m_numClientAssets;

  uint32_t                  m_refCount;
  /// Generated by the AssetCompiler and loaded from a SimpleBundle.
  /// Stores a map between animation IDs (local to this NetworkDef) and other information such as
  /// animation name, format, CRC etc. 
  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup* m_netDefSpecificAnimIDtoFilenameLookup;
};

//----------------------------------------------------------------------------------------------------------------------
// Inline functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE MR::NodeID CharacterDef::getNodeID(const char* nodeName)
{
  NMP_ASSERT(m_netDef);
  MR::NodeID nodeID = m_netDef->getNodeIDFromNodeName(nodeName);
  NMP_ASSERT_MSG(nodeID != MR::INVALID_NODE_ID, "The node of name %s does not exist.", nodeName);
  return nodeID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE MR::MessageID CharacterDef::getMessageID(const char* messageName)
{
  NMP_ASSERT(m_netDef);
  MR::MessageID messageID  = m_netDef->getMessageIDFromMessageName(messageName);
  NMP_ASSERT_MSG(messageID != MR::INVALID_MESSAGE_ID, "The message of name %s does not exist.", messageName);
  return messageID;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE MR::StateID CharacterDef::getStateID(const char* stateName)
{
  NMP_ASSERT(m_netDef);
  MR::StateID stateID = m_netDef->getStateIDFromStateName(stateName);
  NMP_ASSERT_MSG(stateID != MR::INVALID_STATE_ID, "The state of name %s does not exist.", stateName);
  return stateID;
}

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_DEF_H
//----------------------------------------------------------------------------------------------------------------------
