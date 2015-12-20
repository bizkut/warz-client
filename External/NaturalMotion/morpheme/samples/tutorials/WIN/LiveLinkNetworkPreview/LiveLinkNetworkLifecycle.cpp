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
#include "comms/liveLinkNetworkManager.h"

#include "GameManagement/GameCharacterDef.h"
#include "LiveLinkNetworkLifecycle.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
LiveLinkNetworkLifecycle::LiveLinkNetworkLifecycle(Game::CharacterManagerNoPhysics* characterManager):
  MCOMMS::NetworkLifecycleInterface()
{
  m_characterManager = characterManager;
}

//----------------------------------------------------------------------------------------------------------------------
LiveLinkNetworkLifecycle::~LiveLinkNetworkLifecycle()
{
}

//----------------------------------------------------------------------------------------------------------------------
MCOMMS::InstanceID LiveLinkNetworkLifecycle::createNetworkInstance(
  const MCOMMS::GUID& guid,
  const char*         instanceName,
  uint32_t            animSetIndex,
  MCOMMS::Connection* connection,
  const NMP::Vector3& characterStartPosition,
  const NMP::Quat&    characterStartRotation)
{
  if (!MCOMMS::LiveLinkNetworkManager::getInstance())
  {
    NMP_ASSERT_FAIL();
    return MCOMMS::INVALID_INSTANCE_ID;;
  }

  //----------------------------
  // Grab the network definition.
  NMP_ASSERT(m_characterManager);
  Game::CharacterDef* gameCharacterDef = m_characterManager->findCharacterDefByGUID(guid.value);
  if (!gameCharacterDef)
  {
    return MCOMMS::INVALID_INSTANCE_ID;
  }

  //----------------------------
  // Create and setup the network instance.
  //
  // The network instance was previously loaded from disk or over a COMMS connection (See LiveLinkNetworkDefLifecycle)
  // all we need to do is create an instance for that network definition on game side. We choose to create a full game
  // character which would be able to interact with the world and other characters effectively turning the network
  // preview into a full in-game test scenario.
  Game::CharacterNoPhysics* const characterData = m_characterManager->createCharacter(
                                                                    gameCharacterDef,
                                                                    characterStartPosition, 
                                                                    characterStartRotation,
                                                                    (MR::AnimSetIndex)animSetIndex);
  if(!characterData)
  {
    NMP_DEBUG_MSG("error: Failed to create Game Character, returning 0 as default instanceID");
    return 0;
  }
 
  characterData->setName(instanceName);

  //----------------------------
  // Make the network available and keep track of the characters created by this manager.
  MCOMMS::InstanceID instanceId;
  MCOMMS::LiveLinkNetworkManager::getInstance()->publishNetwork(characterData->getNetwork(), characterData->getName(), instanceId);

  CharacterEntry entry;
  entry.m_owningConnection = connection;
  entry.m_character = characterData;

  m_characters.insert(CharacterMap::value_type(instanceId, entry));

  // ...
  return instanceId;
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkNetworkLifecycle::destroyNetworkInstance(MCOMMS::InstanceID id)
{
  //----------------------------
  // Grab character (which hold the network instance).
  const CharacterMap::const_iterator it = m_characters.find(id);
  if (it == m_characters.end())
  {
    NMP_DEBUG_MSG("error: Valid instance ID expected (id=%u)!", id);
    return false;
  }

  Game::CharacterNoPhysics* characterData = it->second.m_character;
  MCOMMS::LiveLinkNetworkManager::getInstance()->revokeNetwork(characterData->getNetwork());
  MCOMMS::LiveLinkNetworkManager::getInstance()->revokeNetworkDef(characterData->getNetwork()->getNetworkDef());
  m_characters.erase(it);

  //----------------------------
  // Revoke and terminate character.
  NMP_ASSERT(m_characterManager);
  m_characterManager->destroyCharacter(characterData);

  // ...
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void LiveLinkNetworkLifecycle::onConnectionClosed(MCOMMS::Connection* connection)
{
  const CharacterMap::const_iterator end = m_characters.end();
  CharacterMap::const_iterator it = m_characters.begin();

  //----------------------------
  // Revoke and terminate characters.
  NMP_ASSERT(m_characterManager);
  for (; it != end; ++it)
  {
    if (it->second.m_owningConnection == connection)
    {
      Game::CharacterNoPhysics* characterData = it->second.m_character;

      MCOMMS::LiveLinkNetworkManager::getInstance()->revokeNetwork(characterData->getNetwork());
      MCOMMS::LiveLinkNetworkManager::getInstance()->revokeNetworkDef(characterData->getNetwork()->getNetworkDef());
      m_characterManager->destroyCharacter(characterData);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
