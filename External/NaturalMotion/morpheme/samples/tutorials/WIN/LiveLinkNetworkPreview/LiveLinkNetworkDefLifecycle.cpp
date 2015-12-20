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
#include "comms/connection.h"
#include "comms/LiveLinkNetworkManager.h"

//----------------------------------------------------------------------------------------------------------------------
#include "LiveLinkNetworkDefLifecycle.h"
#include "GameManagement/GameCharacterManager.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
LiveLinkNetworkDefLifecycle::LiveLinkNetworkDefLifecycle(Game::CharacterManagerNoPhysics* characterManager):
  MCOMMS::NetworkDefLifecycleInterface()
{
  m_characterManager = characterManager;
}

//----------------------------------------------------------------------------------------------------------------------
LiveLinkNetworkDefLifecycle::~LiveLinkNetworkDefLifecycle()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkNetworkDefLifecycle::loadNetworkDefinition(
  const MCOMMS::GUID& guid,
  const char*         networkDefName,
  MCOMMS::Connection* connection)
{
  if (!MCOMMS::LiveLinkNetworkManager::getInstance())
  {
    NMP_ASSERT_FAIL();
    return false;
  }

  //----------------------------
  // Network definition already loaded ?
  if (MCOMMS::LiveLinkNetworkManager::getInstance()->hasNetworkDef(guid))
  {
    return true;
  }

  // Append our string with the .nmb file extension
  static char filename[128];
  NMP_SPRINTF(filename, 128, "%s.nmb", networkDefName);

  //----------------------------
  // Retrieve bundle over the connection.
  //
  // We transfer the network definition over the COMMS connection. The bundle represents the temporary output created
  // by morpheme:connect for this preview.
  const uint32_t bundleSize = connection->getFileSize(filename);
  if (bundleSize == 0)
  {
    NMP_DEBUG_MSG("Failed to retrieve network definition size!");
    return false;
  }

  //----------------------------
  // Allocate bundle (this is a temporary buffer freed below).
  void* const bundle = NMPMemoryAllocAligned(bundleSize, NMP_VECTOR_ALIGNMENT);
  if (bundle == 0)
  {
    NMP_DEBUG_MSG("Failed to allocate memory for bundle!");
    return false;
  }

  //----------------------------
  // Retrieve network definition over connection.
  const bool getFileSucceeded = connection->getFile(bundle, bundleSize, filename);
  if (!getFileSucceeded)
  {
    NMP::Memory::memFree(bundle);
    NMP_DEBUG_MSG("Failed to transfer memory for bundle!");
    return false;
  }

  //----------------------------
  // Load network definition.
  //
  // This is similar to loading a network definition from disk only that here the bundle was retrieved over the COMMS
  // connection rather than read from disk.
  Game::CharacterDef* gameCharacterDef = Game::CharacterDef::create(bundle, bundleSize);
  NMP::Memory::memFree(bundle);

  if (!gameCharacterDef)
  {
    return false;
  }

  //----------------------------
  // Load animations over the connection.
  //
  // Finally we need to also load the animations over the COMMS connections. Again these animations are the most up to
  // date ones provided my morpheme:connect for this preview.
  if (!gameCharacterDef->loadAnimationsOverConnection(connection))
  {
    Game::CharacterDef::destroy(gameCharacterDef);

    NMP_DEBUG_MSG("error: Failed to load animations for bundle <%s>!", filename);
    return false;
  }

  //----------------------------
  // Make the network definition available and keep track of the definitions created by this manager.
  MCOMMS::LiveLinkNetworkManager::getInstance()->publishNetworkDef(gameCharacterDef->getNetworkDef(), gameCharacterDef->getNetworkDefGuid());

  m_characterManager->registerCharacterDef(gameCharacterDef);
  m_characterDefs.insert(CharacterDefMap::value_type(guid, gameCharacterDef));

  // ...
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkNetworkDefLifecycle::destroyNetworkDefinition(const MCOMMS::GUID& guid)
{
  if (!MCOMMS::LiveLinkNetworkManager::getInstance())
  {
    NMP_ASSERT_FAIL();
    return false;
  }

  //----------------------------
  // Grab network definition.
  CharacterDefMap::const_iterator it = m_characterDefs.find(guid);
  if (it == m_characterDefs.end())
  {
    //----------------------------
    // Don't destroy the network definition if it was not originally created over the connection.
    return false;
  }

  //----------------------------
  // Revoke and destroy network definition.
  Game::CharacterDef* gameCharacterDef = it->second;

  m_characterManager->destroyCharacterDef(gameCharacterDef);
  m_characterDefs.erase(guid);

  // ...
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkNetworkDefLifecycle::clearCachedData()
{
  return true;
}
