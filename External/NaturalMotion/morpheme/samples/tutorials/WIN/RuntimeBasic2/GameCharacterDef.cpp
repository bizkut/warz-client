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
#include "GameCharacterDef.h"

#include "GameCharacterManager.h"
#include "GameAssetLoader.h"
#include "simpleBundle/simpleBundle.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
CharacterDefBasic* CharacterDefBasic::create(const char* filename)
{
  //----------------------------
  // Create and initialise an instance of our character definition
  CharacterDefBasic* const instance = static_cast<CharacterDefBasic*>(NMPMemoryAlloc(sizeof(CharacterDefBasic)));
  new(instance) CharacterDefBasic();

  //----------------------------
  // Load the given bundle file into memory and load the bundle.
  NMP_STDOUT("Loading: %s", filename);

  //----------------------------
  // Load binary bundle into memory
  int64_t fileSize = NMP::NMFile::allocAndLoad(filename, &instance->m_bundle, &instance->m_bundleSize);
  if (fileSize <= -1)
  {
    NMP_ASSERT_FAIL();
    NMP_STDOUT("Failed to load file: '%s'", filename);
    NMP_STDOUT("Perhaps the assets have not been processed.");

    return NULL;
  }
  else
  {
    NMP_ASSERT(instance->m_bundleSize >= fileSize);
    NMP_STDOUT("Loaded file: '%s", filename);

    //----------------------------
    // Allocate memory and unpack bundle
    if(!(instance->init()))
    {
      NMP::Memory::memFree(instance);

      NMP_ASSERT_FAIL();
      NMP_STDOUT("Failed to initialise gameCharacterDef");
    }

    NMP_STDOUT("CharacterDef successfully created");
  }

  return instance;
}

//--------------------------------------------------------------------------------------------------------------------
bool CharacterDefBasic::destroy(CharacterDefBasic* characterDef)
{
  NMP_ASSERT(characterDef);

  characterDef->term();

  NMP::Memory::memFree(characterDef);
  characterDef = NULL;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterDefBasic::loadAnimations()
{
  //----------------------------
  // Load animations listed in this network definition
  for (uint32_t i = 0; i < m_netDef->getNumAnimSets(); ++i)
  {
    if (!m_netDef->loadAnimations((MR::AnimSetIndex)i, m_animFileLookUp))
    {
      return false;
    }
  }

  NMP_STDOUT("Animations successfully loaded");
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterDefBasic::init()
{
  if (!m_bundle || !m_bundleSize)
  {
    NMP_STDOUT("error: Valid bundle expected!");
    return NULL;
  }

  //----------------------------
  // Process the bundle and extract the contents into memory
  NMP_STDOUT("Loading bundle:");
  m_netDef = AssetLoaderBasic::loadBundle(m_bundle,
    (size_t)m_bundleSize,
    &m_animRigDef,
    &m_animFileLookUp);

  if (!m_netDef)
  {
    NMP_STDOUT("error: Failed to unpack the bundle!");
    return false;
  }
  NMP_STDOUT("Bundle successfully loaded");

  m_isLoaded = true;

  return true;
}

//--------------------------------------------------------------------------------------------------------------------
bool CharacterDefBasic::term()
{
  if (m_isLoaded)
  {
    for (uint32_t i = 0; i < m_netDef->getNumAnimSets(); ++i)
    {
      if (!m_netDef->unloadAnimations((MR::AnimSetIndex)i, NULL))
      {
        return false;
      }
    }

    //----------------------------
    // In the same theme as loadBundle above we call a function that can be cut-and-paste into any program.
    AssetLoaderBasic::unLoadBundle(m_bundle, (size_t)m_bundleSize);
    
    //----------------------------
    // We didn't register the animFileLookUp during loadBundle so we need to manually free it here.
    NMP::Memory::memFree(m_animFileLookUp);

    //----------------------------
    // We're finished with it so free the bundle.
    NMP::Memory::memFree(m_bundle);
  }

  //----------------------------
  // Free any memory that may be allocated in this class here
  return true;
}

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
