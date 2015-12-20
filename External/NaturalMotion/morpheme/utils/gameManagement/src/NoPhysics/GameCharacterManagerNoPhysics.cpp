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
#include "GameManagement/NoPhysics/GameAnimSystemModuleNoPhysics.h"
#include "gameManagement/NoPhysics/GameCharacterManagerNoPhysics.h"
#include "GameManagement/NoPhysics/GameCharacterNoPhysics.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
CharacterManagerNoPhysics::CharacterManagerNoPhysics()
{
}

//----------------------------------------------------------------------------------------------------------------------
CharacterManagerNoPhysics::~CharacterManagerNoPhysics()
{
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerNoPhysics::init()
{
  NMP_ASSERT(AnimSystemModuleNoPhysics::isInitialised());

  // Base class start up of morpheme.
  CharacterManager::init();
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerNoPhysics::term()
{  
  NMP_ASSERT(AnimSystemModuleNoPhysics::isInitialised());

  // Base class shut down of morpheme.
  CharacterManager::term();
}

//----------------------------------------------------------------------------------------------------------------------
CharacterDef* CharacterManagerNoPhysics::createCharacterDef(
  const char* filename)
{
  return CharacterManager::createCharacterDef(filename, NULL);
}

//----------------------------------------------------------------------------------------------------------------------
CharacterDef* CharacterManagerNoPhysics::createCharacterDef(
  void*  bundle,
  size_t bundleSize)
{
  return CharacterManager::createCharacterDef(bundle, bundleSize, NULL);
}

//----------------------------------------------------------------------------------------------------------------------
CharacterNoPhysics* CharacterManagerNoPhysics::createCharacter(
  CharacterDef*           characterDef,
  const NMP::Vector3&     initialPosition,
  const NMP::Quat&        initialOrientation,
  MR::AnimSetIndex        initialAnimSetIndex,
  const char*             name,
  NMP::FastHeapAllocator* temporaryMemoryAllocator,
  uint32_t                externalIdentifier)
{
  // Check that the CharacterDef has been registered with this manager.
  NMP_ASSERT(isCharacterDefRegistered(characterDef));

  // Create the CharacterNoPhysics.
  CharacterNoPhysics* gameCharacter = CharacterNoPhysics::create(
                                                  characterDef,
                                                  initialPosition,
                                                  initialOrientation,
                                                  initialAnimSetIndex,
                                                  name,
                                                  temporaryMemoryAllocator,
                                                  externalIdentifier);
  if (!gameCharacter)
  {
    NMP_STDOUT(
            "Failed to create game character %s instance from a loaded game character def %s.",
            name,
            characterDef->getNetworkDef()->getName());
    NMP_ASSERT_FAIL();
    return NULL;
  }

  // The management of this CharacterNoPhysics' memory is this CharacterManagers responsibility.
  registerCharacter(gameCharacter);

  return gameCharacter;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerNoPhysics::updateToPreCharacterController(float timeDelta)
{
  //----------------------------
  // Update the network to the point where it is ready to have the actual character controller updated.
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterNoPhysics* character = static_cast<CharacterNoPhysics*>(*iter);
    character->updateToPreCharacterController(timeDelta);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterManagerNoPhysics::updateFinalise()
{
  //----------------------------
  // Finish the update of the animation only networks.
  for (CharacterList::iterator iter = m_characterList.begin(); iter != m_characterList.end(); ++iter)
  {
    CharacterNoPhysics* character = static_cast<CharacterNoPhysics*>(*iter);
    character->updateFinalise();
  }
}

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
