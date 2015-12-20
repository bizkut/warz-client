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
#include "GameAnimLoader.h"
#include "GameAnimNetworkDef.h"
#include "GameAssetLoader.h"
#include "comms/liveLinkNetworkManager.h"
//----------------------------------------------------------------------------------------------------------------------

#ifdef NM_EUPHORIA_TUTORIAL
#include "GameEuphoriaModule.h"
#include "euphoria/erNetworkInterface.h"
#endif

//----------------------------------------------------------------------------------------------------------------------
GameAnimNetworkDef* GameAnimNetworkDef::create(void* bundle, size_t bundleSize)
{
  if (!bundle || !bundleSize)
  {
    NMP_DEBUG_MSG("error: Valid bundle expected!");
    return NULL;
  }

  // Create empty instance then fill in below.
  GameAnimNetworkDef* const instance = new GameAnimNetworkDef();

  // Evaluate the number of registered (core) and client assets and allocate appropriate storage.
  GameAssetLoader::evalBundleRequirements(instance->m_numRegisteredAssets, instance->m_numClientAssets, bundle, bundleSize);
  NMP_ASSERT(instance->m_numRegisteredAssets > 0);

  instance->m_registeredAssetIDs = (uint32_t*)NMPMemoryCalloc(instance->m_numRegisteredAssets * sizeof(uint32_t));
  instance->m_clientAssets = (void**)NMPMemoryCalloc(instance->m_numClientAssets * sizeof(void*));

  // Load the assets.
  instance->m_networkDef = GameAssetLoader::loadNetworkDef(
                             instance->m_registeredAssetIDs, instance->m_clientAssets,
                             instance->m_networkDefGuid, instance->m_animFileLookup,
                             instance->m_numRegisteredAssets, instance->m_numClientAssets,
                             bundle, bundleSize);

  if (!instance->m_networkDef)
  {
    delete instance;
    NMP_DEBUG_MSG("error: Failed to unpack the bundle!");
    return NULL;
  }

#ifdef NM_EUPHORIA_TUTORIAL
  instance->m_characterDef = new ER::CharacterDef();
  instance->m_characterDef->create(instance->m_networkDef, GameEuphoriaModule::getBehaviourLibrary()->getBehaviourDefs());
#endif

  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
GameAnimNetworkDef* GameAnimNetworkDef::create(const char* filename)
{
  GameAnimNetworkDef* instance = NULL;

  // Load the given bundle file into memory and load the bundle.
  void* bundle = NULL;
  int64_t bundleSize = 0;

  const int64_t numBytesRead = NMP::NMFile::allocAndLoad(filename, &bundle, &bundleSize);
  if (numBytesRead <= 0)
  {
    NMP_DEBUG_MSG("error: Failed to load bundle file (%s)!", filename);
  }
  else
  {
    NMP_ASSERT(bundleSize >= numBytesRead);
    instance = create(bundle, (size_t)bundleSize);
  }

  // Free the memory allocated by NMP::NMFile::allocAndLoad().
  NMP::Memory::memFree(bundle);

  // ...
  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
GameAnimNetworkDef::~GameAnimNetworkDef()
{

#ifdef NM_EUPHORIA_TUTORIAL
  m_characterDef->destroy();

  delete m_characterDef;
  m_characterDef = NULL;
#endif

  if (m_networkDef)
  {
    unloadAnimations();

#pragma region LiveLink
    if (MCOMMS::LiveLinkNetworkManager::getInstance())
    {
      MCOMMS::LiveLinkNetworkManager::getInstance()->revokeNetworkDef(m_networkDef);
    }
#pragma endregion LiveLink

    GameAssetLoader::unloadAssets(m_registeredAssetIDs, m_numRegisteredAssets, m_clientAssets, m_numClientAssets);
  }

  NMP::Memory::memFree(m_clientAssets);
  NMP::Memory::memFree(m_registeredAssetIDs);
}

//----------------------------------------------------------------------------------------------------------------------
bool GameAnimNetworkDef::loadAnimations()
{
  GameAnimLoader::UserData userData;
  userData.m_animFileLookup = m_animFileLookup;

  for (uint32_t i = 0; i < m_networkDef->getNumAnimSets(); ++i)
  {
    if (!m_networkDef->loadAnimations((MR::AnimSetIndex)i, &userData))
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool GameAnimNetworkDef::loadAnimationsOverConnection(MCOMMS::Connection* connection)
{
  NMP_ASSERT_MSG(connection, "error: Valid connection expected!");

  GameAnimLoader::UserData userData;
  userData.m_animFileLookup = m_animFileLookup;
  userData.m_connection = connection;

  for (uint32_t i = 0; i < m_networkDef->getNumAnimSets(); ++i)
  {
    if (!m_networkDef->loadAnimations((MR::AnimSetIndex)i, &userData))
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool GameAnimNetworkDef::unloadAnimations()
{
  for (uint32_t i = 0; i < m_networkDef->getNumAnimSets(); ++i)
  {
    if (!m_networkDef->unloadAnimations((MR::AnimSetIndex)i, NULL))
    {
      return false;
    }
  }

  return true;
}

#pragma region LiveLink
//----------------------------------------------------------------------------------------------------------------------
void GameAnimNetworkDef::publish()
{
  if (MCOMMS::LiveLinkNetworkManager::getInstance())
  {
    MCOMMS::LiveLinkNetworkManager::getInstance()->publishNetworkDef(m_networkDef, getNetworkDefGuid());
  }
}
#pragma endregion LiveLink

//----------------------------------------------------------------------------------------------------------------------
GameAnimNetworkDef::GameAnimNetworkDef():
  m_networkDef(NULL)
  ,m_registeredAssetIDs(NULL)
  ,m_clientAssets(NULL)
#ifdef NM_EUPHORIA_TUTORIAL
  ,m_characterDef(NULL)
#endif
{
}
