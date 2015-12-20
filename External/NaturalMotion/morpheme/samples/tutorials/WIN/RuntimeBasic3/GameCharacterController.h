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
#ifndef GAME_CHARACTER_CONTROLLER_H
#define GAME_CHARACTER_CONTROLLER_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "morpheme/mrCharacterControllerInterfaceBase.h"
//----------------------------------------------------------------------------------------------------------------------

#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4324)
#endif

namespace Game
{


//----------------------------------------------------------------------------------------------------------------------
// CharacterControllerNoPhysics maintains the position and rotation of an instantiated character network
class CharacterControllerBasic : public MR::CharacterControllerInterfaceBase
{
public:

  ~CharacterControllerBasic() {};

  //----------------------------
  // Allocate and initialise a CharacterController instance, setting its initial position and orientation.
  // This sets the worldRootTransform to the origin, you can call setCharacterPosition() and setCharacterOrientation()
  //  after init to move the character.
  static CharacterControllerBasic* create();

  //----------------------------
  // Release a CharacterController instance.
  static void destroy(CharacterControllerBasic* characterController);

  //----------------------------
  // Set the network instance and set it's root.
  int32_t init(MR::Network* network);

  //----------------------------
  /// Update the character controller position after the network has been updated and update the networks
  ///  character controller worldRootTransform with this change.
  ///
  /// NOTE: if deltaTranslation & deltaOrientation are not provided then the delta transform
  ///  calculated and stored in the Network is used directly.
  void updateTransform(
    const NMP::Vector3* requestedDeltaWorldTranslation = NULL,
    const NMP::Quat*    requestedDeltaWorldOrientation = NULL);

  //----------------------------
  // Accessors
  NMP::Vector3* getCharacterPosition()    { return &m_characterPosition; }
  NMP::Quat*    getCharacterOrientation() { return &m_characterOrientation; } 

  //----------------------------
  // Set the character position and orientation
  void setPosition(const NMP::Vector3& position);
  void setOrientation(const NMP::Quat& orientation);

protected:

  CharacterControllerBasic();

protected: 

  NMP::Vector3 m_characterPosition;     // in World Space
  NMP::Quat    m_characterOrientation;  // in World Space

  MR::Network* m_net;               // The network associated with this character controller
};

}

#ifdef NM_COMPILER_MSVC
#pragma warning (pop)
#endif

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_CONTROLLER_H
//----------------------------------------------------------------------------------------------------------------------
