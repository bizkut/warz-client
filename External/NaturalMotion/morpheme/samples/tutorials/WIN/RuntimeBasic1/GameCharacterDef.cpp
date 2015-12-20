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
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
CharacterDefBasic* CharacterDefBasic::create(const char* filename)
{
  //----------------------------
  // Load the given bundle file into memory and load the bundle.
  NMP_STDOUT("Loading: %s", filename); 
  
  //----------------------------
  // Create and initialise an instance of our character definition
  CharacterDefBasic* const instance = static_cast<CharacterDefBasic*>(NMPMemoryAlloc(sizeof(CharacterDefBasic)));
  new(instance) CharacterDefBasic();

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
    if (!(instance->init()))
    {
      CharacterDefBasic::destroy(instance);

      NMP_ASSERT_FAIL();
      NMP_STDOUT("Failed to initialise gameCharacterDef");
    }

    NMP_STDOUT("GameCharacterDef successfully created");
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
  if (!AssetLoaderBasic::loadBundle(m_bundle, (size_t)m_bundleSize, &m_animRigDef))
  {
    NMP_STDOUT("error: Failed to unpack the bundle!");
    return false;
  }
  NMP_STDOUT("Bundle successfully loaded");

  m_isLoaded = true;

  return true;
}

// --------------------------------------------------------------------------------------------------------------------
bool CharacterDefBasic::term()
{
  if (m_isLoaded)
  {
    //----------------------------
    // unload the GameCharacterDef's bundle
    AssetLoaderBasic::unLoadBundle(m_bundle, (size_t)m_bundleSize);

    NMP::Memory::memFree(m_bundle);
  }

  //----------------------------
  // Free any other memory that may be allocated in this class here

  return true;
}

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
