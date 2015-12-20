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
#include "comms/liveLinkNetworkManager.h"

#include "GameManagement/GameCharacterDef.h"
#include "GameManagement/GameAnimModule.h"
#include "GameManagement/GameAnimLoader.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
CharacterDef* CharacterDef::create(const char* filename, ValidatePluginListFn validatePluginList)
{
  //----------------------------
  // Load the given bundle file into memory and load the bundle.
  NMP_DEBUG_MSG("Loading: %s", filename);

  void* bundle = NULL;
  int64_t bundleSize = 0;

  //----------------------------
  // Load binary bundle into memory
  int64_t fileSize = NMP::NMFile::allocAndLoad(filename, &bundle, &bundleSize);
  if (fileSize == -1)
  {
    NMP_ASSERT_FAIL();
    NMP_DEBUG_MSG("Failed to load file '%s'", filename);
    NMP_DEBUG_MSG("Perhaps the assets have not been processed.");
  }

  CharacterDef* instance = CharacterDef::create(bundle, (size_t)bundleSize, validatePluginList);

  //----------------------------
  // We have loaded the information in the buffer now so we can free it.
  NMP::Memory::memFree(bundle);

  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
CharacterDef* CharacterDef::create(void* bundle, size_t bundleSize, ValidatePluginListFn validatePluginList)
{
  if (!bundle || !bundleSize)
  {
    NMP_DEBUG_MSG("error: Valid bundle expected!");
    return NULL;
  }

  //----------------------------
  // Create and initialise an instance of our character definition
  CharacterDef* const instance = static_cast<CharacterDef*>(NMPMemoryAlloc(sizeof(CharacterDef)));
  new(instance) CharacterDef();

  //----------------------------
  // Allocate memory and unpack bundle
  if(!(instance->init(bundle, bundleSize, validatePluginList)))
  {
    NMP::Memory::memFree(instance);

    NMP_ASSERT_FAIL();
    NMP_DEBUG_MSG("Failed to initialise character definition");
  }

  NMP_DEBUG_MSG("Game::CharacterDef created");

  return instance;
}

//--------------------------------------------------------------------------------------------------------------------
bool CharacterDef::destroy(CharacterDef* characterDef)
{
  NMP_ASSERT(characterDef);

  characterDef->term();

  NMP::Memory::memFree(characterDef);
  characterDef = NULL;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterDef::loadAnimationsFromDisk(const char* animLocation)
{
  AnimLoader::UserData userData;
  NMP_ASSERT(m_netDefSpecificAnimIDtoFilenameLookup);
  userData.m_animFileLookup = m_netDefSpecificAnimIDtoFilenameLookup;
  userData.m_animLocation = animLocation;
  userData.m_animRegistry = &AnimLoader::sm_animRegistry;
  userData.m_connection = NULL;

  for (uint32_t i = 0; i < m_netDef->getNumAnimSets(); ++i)
  {
    if (!m_netDef->loadAnimations((MR::AnimSetIndex)i, &userData))
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterDef::loadAnimationsOverConnection(MCOMMS::Connection* connection)
{
  NMP_ASSERT_MSG(connection, "error: Valid connection expected!");

  AnimLoader::UserData userData;
  NMP_ASSERT(m_netDefSpecificAnimIDtoFilenameLookup);
  userData.m_animFileLookup = m_netDefSpecificAnimIDtoFilenameLookup;
  userData.m_connection = connection;
  userData.m_animRegistry = &AnimLoader::sm_animRegistry;
  userData.m_animLocation = NULL;

  for (uint32_t i = 0; i < m_netDef->getNumAnimSets(); ++i)
  {
    if (!m_netDef->loadAnimations((MR::AnimSetIndex)i, &userData))
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterDef::unloadAnimations()
{
  AnimLoader::UserData userData;
  userData.m_animFileLookup = m_netDefSpecificAnimIDtoFilenameLookup;
  userData.m_connection = NULL;
  userData.m_animRegistry = &AnimLoader::sm_animRegistry;

  for (uint32_t i = 0; i < m_netDef->getNumAnimSets(); ++i)
  {
    if (!m_netDef->unloadAnimations((MR::AnimSetIndex)i, &userData))
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterDef::init(
  void*                buffer,
  size_t               bufferSize,
  ValidatePluginListFn validatePluginList)
{
  NMP_ASSERT(buffer && bufferSize);

  //----------------------------
  // Evaluate the asset requirements of the network definition from the bundle
  AssetLoader::evalBundleRequirements(m_numRegisteredAssets,
    m_numClientAssets,
    buffer,
    bufferSize);

  NMP_ASSERT(m_numRegisteredAssets > 0);

  //----------------------------
  // Allocate arrays for storing asset information based on the bundle requirements
  m_registeredAssetIDs = (uint32_t*)NMPMemoryCalloc(m_numRegisteredAssets * sizeof(uint32_t));
  m_clientAssets = (void**)NMPMemoryCalloc(m_numClientAssets * sizeof(void*));

  //----------------------------
  // Process the bundle and extract the contents into memory.
  // Load the assets.
  m_netDef = AssetLoader::loadBundle(
                            buffer,
                            bufferSize,
                            m_registeredAssetIDs,
                            m_netDefGuid,
                            m_clientAssets,
                            m_numRegisteredAssets,
                            m_numClientAssets,
                            validatePluginList,
                            m_netDefSpecificAnimIDtoFilenameLookup,
                            m_netPredictionDef);
  if (!m_netDef)
  {
    NMP_ASSERT(m_netDef);
    return false;
  }

  m_isLoaded = true;
  m_refCount = 1;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterDef::term()
{
  if (m_isLoaded)
  { 
    //----------------------------
    // unload the CharacterDef's animations
    unloadAnimations();

    //----------------------------
    // unload the CharacterDef's bundle
    AssetLoader::unloadAssets(m_registeredAssetIDs, m_numRegisteredAssets, m_clientAssets, m_numClientAssets);

    m_isLoaded = false;
  }

  //----------------------------
  // Free any other memory that may be allocated in this class here
  if (m_registeredAssetIDs)
  {
    NMP::Memory::memFree(m_registeredAssetIDs);
    m_registeredAssetIDs = NULL;
  }

  if(m_clientAssets)
  {
    NMP::Memory::memFree(m_clientAssets);
    m_clientAssets = NULL;
  }
}

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
