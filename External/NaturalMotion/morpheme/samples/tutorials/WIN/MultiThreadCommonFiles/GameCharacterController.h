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

//----------------------------------------------------------------------------------------------------------------------
class GameCharacterControllerBase
{
public:

  virtual ~GameCharacterControllerBase() {}
  virtual MR::CharacterControllerInterface* getCharacterController() const { return NULL; }

protected:

  GameCharacterControllerBase() {}
};

//----------------------------------------------------------------------------------------------------------------------
/// \class GameCharacterController
/// A wrapper around a physics or non-physics character controller
//----------------------------------------------------------------------------------------------------------------------
class GameCharacterController : public GameCharacterControllerBase
{
public:

  static GameCharacterController* create(const NMP::Vector3& initialTranslation, const NMP::Quat& initialOrientation);
  ~GameCharacterController();

  MR::CharacterControllerInterface* getCharacterController() const NM_OVERRIDE { return m_characterController; }

  void init(MR::Network*  network);

  /// \brief Accumulates the trajectory and updates the underlying morpheme character controller.
  /// \see MR::Network::getTranslationChange()
  /// \see MR::Network::getOrientationChange()
  void updateController(const NMP::Vector3& deltaTranslation, const NMP::Quat& deltaOrientation);

  // Accessors
  NMP::Vector3* getCharacterPosition()    { return &m_characterPosition; }
  NMP::Quat*    getCharacterOrientation() { return &m_characterOrientation; }  

  // A simple helper class to workaround MR::CharacterController having protected constructor / destructor
  class NoPhysicsCharacterController : public MR::CharacterControllerInterfaceBase
  {
  public:
    NoPhysicsCharacterController() {}
    virtual ~NoPhysicsCharacterController() {};
  };

private:
  
  NMP::Vector3 m_characterPosition;
  NMP::Quat    m_characterOrientation;
  
  NoPhysicsCharacterController* m_characterController;
  MR::Network* m_network;


  GameCharacterController(const NMP::Vector3& initialTranslation, const NMP::Quat& initialOrientation);
};

#ifdef NM_COMPILER_MSVC
#pragma warning (pop)
#endif

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_CONTROLLER_H
//----------------------------------------------------------------------------------------------------------------------
