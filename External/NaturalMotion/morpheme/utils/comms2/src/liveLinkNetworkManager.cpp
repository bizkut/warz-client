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
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

LiveLinkNetworkManager* LiveLinkNetworkManager::sm_instance = NULL;
MCOMMS::InstanceID LiveLinkNetworkManager::sm_nextInstanceId = 1;

//----------------------------------------------------------------------------------------------------------------------
LiveLinkNetworkManager::LiveLinkNetworkManager(uint32_t maxNumCharacterDefs, uint32_t maxNumCharacters)
{
  NMP::Memory::Format characterDefsFormat = GameCharacterDefMap::getMemoryRequirements(maxNumCharacterDefs);
  NMP::Memory::Resource characterDefsRes = NMPMemoryAllocateFromFormat(characterDefsFormat);
  m_characterDefs = GameCharacterDefMap::init(characterDefsRes, maxNumCharacterDefs);
  
  NMP::Memory::Format charactersMapFormat = GameCharacterMap::getMemoryRequirements(maxNumCharacters);
  NMP::Memory::Resource charactersMapRes = NMPMemoryAllocateFromFormat(charactersMapFormat);
  m_charactersData = GameCharacterMap::init(charactersMapRes, maxNumCharacters);

  NMP_ASSERT(!sm_instance);
  sm_instance = this;
}

//----------------------------------------------------------------------------------------------------------------------
LiveLinkNetworkManager::~LiveLinkNetworkManager()
{
  NMP_ASSERT(sm_instance == this);
  sm_instance = NULL;

  NMP::Memory::memFree(m_characterDefs);
  NMP::Memory::memFree(m_charactersData);
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkNetworkManager::publishNetworkDef(MR::NetworkDef* gameCharacterDef, const uint8_t* guid)
{
  if (!gameCharacterDef)
  {
     NMP_ASSERT(gameCharacterDef);
     return false;
  }

  //----------------------------  
  // Avoid publishing the definition twice.
  const GameCharacterDefMap::iterator it = m_characterDefs->find(guid);
  if (it != m_characterDefs->end())
  {
    NMP_ASSERT_MSG(
      (*it).key == guid,
      "warning: Network definition already published under a different GUID!")
      return false;
  }

  //----------------------------
  // Add to definition list.
  m_characterDefs->insert(guid, gameCharacterDef);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkNetworkManager::revokeNetworkDef(MR::NetworkDef* gameCharacterDef)
{
  if (!gameCharacterDef)
  {
    NMP_ASSERT(gameCharacterDef);
    return false;
  }

  //----------------------------
  // Find and remove the definition.
  for (GameCharacterDefMap::iterator iter = m_characterDefs->begin(); iter != m_characterDefs->end(); ++iter)
  {
    if ((*iter).value == gameCharacterDef)
    {
      m_characterDefs->erase(iter);
      return true;
    }
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkNetworkManager::hasNetworkDef(const MCOMMS::GUID& guid) const
{
  GameCharacterDefMap::iterator it = m_characterDefs->find(guid);
  return (it != m_characterDefs->end());
}

//----------------------------------------------------------------------------------------------------------------------
MR::NetworkDef* LiveLinkNetworkManager::findNetworkDefByGuid(const MCOMMS::GUID& guid) const
{
  GameCharacterDefMap::iterator it = m_characterDefs->find(guid);
  if (it == m_characterDefs->end())
  {
    NMP_MSG("error: Valid network GUID expected!");
    return NULL;
  }

  return (*it).value;
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkNetworkManager::publishNetwork(MR::Network* gameCharacter, const char* name, MCOMMS::InstanceID& instanceId)
{
  if (!gameCharacter)
  {
    NMP_ASSERT(gameCharacter);
    return false;
  }

  NMP_ASSERT(name && (name[0] != '\0'));

  // Check that the network def is registered (this should happen externally).
  {
    bool found = false;
    for (GameCharacterDefMap::iterator iter = m_characterDefs->begin(); iter != m_characterDefs->end(); ++iter)
    {
      if ((*iter).value == gameCharacter->getNetworkDef())
      {
        found = true;
        break;
      }
    }


    NMP_ASSERT_MSG(found, "error: The network definition for this network has not been published!");
    if (!found)
    {
      instanceId = MCOMMS::INVALID_INSTANCE_ID;
      return false;
    }
  }


  //----------------------------
  // Avoid publishing the instance twice.
  const GameCharacterMap::iterator end = m_charactersData->end();
  GameCharacterMap::iterator it = m_charactersData->begin();

  // Check if the game character has already been published.  If it has then early out.
  for (; it != end; ++it)
  {
    if ((*it).value.m_gameCharacter == gameCharacter)
    {
      if (strcmp((*it).value.m_name, name) != 0)
      {
        NMP_ASSERT_FAIL_MSG("warning: Network instance already published under a different name!");
      }

      instanceId = MCOMMS::INVALID_INSTANCE_ID;
      return false;
    }
  }

  // Add to instance list.
  NetworkEntry entry(gameCharacter, name);

  instanceId = getNextInstanceId();
  m_charactersData->insert(instanceId, entry);

  // ...
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkNetworkManager::revokeNetwork(MR::Network* gameCharacter)
{
  if (!gameCharacter)
  {
    NMP_ASSERT(gameCharacter);
    return false;
  }

  //----------------------------
  // Find and remove the instance.
  const GameCharacterMap::iterator end = m_charactersData->end();
  GameCharacterMap::iterator it = m_charactersData->begin();

  for (; it != end; ++it)
  {
    if ((*it).value.m_gameCharacter == gameCharacter)
    {
      m_charactersData->erase(it);
      return true;
    }
  }

  NMP_MSG("info: The network instance was not published!");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
MR::Network* LiveLinkNetworkManager::findNetworkByInstanceID(MCOMMS::InstanceID id, const char *&name) const
{
  GameCharacterMap::iterator it = m_charactersData->find(id);
  if (it == m_charactersData->end())
  {
    NMP_MSG("error: Valid instance ID expected (id=%u)!", id);
    return NULL;
  }

  name = (*it).value.m_name;
  return (*it).value.m_gameCharacter;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t LiveLinkNetworkManager::getNetworkDefCount() const
{
  return m_characterDefs->size();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t LiveLinkNetworkManager::getNetworkDefGUIDs(MCOMMS::GUID* guids, uint32_t maxGuids) const
{
  GameCharacterDefMap::iterator end = m_characterDefs->end();
  GameCharacterDefMap::iterator it = m_characterDefs->begin();

  for (uint32_t i = 0; (i < maxGuids) && (it != end); ++i, ++it)
  {
    guids[i] = (*it).key;
  }

  return getNetworkDefCount();
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkNetworkManager::isNetworkDefinitionLoaded(const MCOMMS::GUID& guid) const
{
  return (m_characterDefs->find(guid) != m_characterDefs->end());
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t LiveLinkNetworkManager::getNetworkInstanceCount() const
{
  return m_charactersData->size();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t LiveLinkNetworkManager::getNetworkInstanceIDs(MCOMMS::InstanceID* instanceIDs, uint32_t maxIntanceIDs) const
{
  const GameCharacterMap::iterator end = m_charactersData->end();
  GameCharacterMap::iterator it = m_charactersData->begin();

  for (uint32_t i = 0; (i < maxIntanceIDs) && (it != end); ++i, ++it)
  {
    instanceIDs[i] = (*it).key;
  }

  return getNetworkInstanceCount();
}

//----------------------------------------------------------------------------------------------------------------------
const char* LiveLinkNetworkManager::getNetworkInstanceName(MCOMMS::InstanceID id) const
{
  const char* name = "?";
  findNetworkByInstanceID(id, name);
  return name;
}

//----------------------------------------------------------------------------------------------------------------------
bool LiveLinkNetworkManager::getNetworkInstanceDefinitionGUID(MCOMMS::InstanceID id, MCOMMS::GUID& guid) const
{
  const char* name = NULL;
  MR::Network* const gameCharacter = findNetworkByInstanceID(id, name);
  if (!gameCharacter)
  {
    memset(guid.value, 254, 16);    // Set to 'blank' GUID.
    return false;
  }

  memset(guid.value, 254, 16);    // Set to 'blank' GUID.
  bool found = false;
  for (GameCharacterDefMap::iterator iter = m_characterDefs->begin(); iter != m_characterDefs->end(); ++iter)
  {
    if ((*iter).value == gameCharacter->getNetworkDef())
    {
      found = true;
      guid = (*iter).key;
      break;
    }
  }

  return found;
}

} // namespace COMMS
