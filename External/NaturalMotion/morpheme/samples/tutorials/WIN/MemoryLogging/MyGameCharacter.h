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
#ifdef _MSC_VER
  #pragma once
#endif
  #ifndef MY_GAME_CHARACTER_H
#define MY_GAME_CHARACTER_H
//----------------------------------------------------------------------------------------------------------------------
#include "GameManagement/NoPhysics/GameCharacterNoPhysics.h"
#include "NMPlatform/NMLoggingMemoryAllocator.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

class MyCharacter : public Character
{
public:

  virtual ~MyCharacter() {};

  //----------------------------
  // Create an instance of a morpheme network (GameCharacter)
  static MyCharacter* create(CharacterDef* characterDef);

  /// \brief Release an instance of a character.
  virtual void destroy() NM_OVERRIDE;

  //----------------------------
  // Character update stages...
  //----------------------------

  // Update step 1: 
  //  Build the queue and process it up to the point where the global character controller update is required.
  //  After this step the game or AI can amend the position of the character if so desired.
  void updateToPreCharacterController(float deltaTime);

  // Update step 2:
  //   Move the character controller by the suggested delta transform, finish any remaining network execution and compute world transforms.
  void updateFinalise();

  // Returns the character controller held in the network.
  CharacterControllerNoPhysics*  getCharacterController() { return static_cast<CharacterControllerNoPhysics*>(m_net->getCharacterController()); }
};

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // MY_GAME_CHARACTER_H
//----------------------------------------------------------------------------------------------------------------------
