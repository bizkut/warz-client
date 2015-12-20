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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef GAME_CHARACTER_CONTROLLER_NOPHYSICS_H
#define GAME_CHARACTER_CONTROLLER_NOPHYSICS_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "morpheme/mrCharacterControllerInterfaceBase.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
/// \class Game::CharacterControllerNoPhysics
/// \brief Maintains the position and rotation of an instantiated character.
//----------------------------------------------------------------------------------------------------------------------
class CharacterControllerNoPhysics : public MR::CharacterControllerInterfaceBase
{
public:

  virtual ~CharacterControllerNoPhysics()  {};

  /// \brief Allocate and initialise a CharacterControllerNoPhysics instance.
  static CharacterControllerNoPhysics* create(
    const NMP::Vector3& initialPosition,
    const NMP::Quat&    initialOrientation);

  /// \brief Release a CharacterControllerNoPhysics instance.
  static void destroy(CharacterControllerNoPhysics* characterController);

  /// \brief Initialise the CharacterControllerNoPhysics. 
  ///
  /// This sets the worldRootTransform to the origin, you can call setCharacterPosition and 
  /// setCharacterOrientation after init to move the character.
  void init(MR::Network* net);

  
  /// \brief Update the character controller position after the network has been updated and update the networks
  ///  character controller worldRootTransform with this change.
  ///
  /// NOTE: if deltaTranslation & deltaOrientation are not provided then the delta transform
  ///  calculated and stored in the Network is used directly.
  void updateControllerMove(
    const NMP::Vector3* requestedDeltaWorldTranslation = NULL,  
    const NMP::Quat*    requestedDeltaWorldOrientation = NULL);
      
  /// \brief Set the character world position and orientation.
  virtual void setPosition(const NMP::Vector3& position);
  virtual void setOrientation(const NMP::Quat& orientation);
  virtual void setTransform(
    const NMP::Vector3& position,
    const NMP::Quat&    orientation);

  //----------------------------
  // Accessors
  const NMP::Vector3 getPosition() const    { return m_characterPosition; }
  const NMP::Quat    getOrientation() const { return m_characterOrientation; }

protected:

  CharacterControllerNoPhysics(const NMP::Vector3& initialPosition, const NMP::Quat& initialOrientation);

protected:

  NMP::Vector3 m_characterPosition;    ///< in World Space
  NMP::Quat    m_characterOrientation; ///< in World Space

  MR::Network* m_net;                  ///< The animation Network associated with this character controller.
};

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_CONTROLLER_NOPHYSICS_H
//----------------------------------------------------------------------------------------------------------------------
