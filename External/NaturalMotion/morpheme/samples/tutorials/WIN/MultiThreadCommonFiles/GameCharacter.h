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
#ifndef GAME_CHARACTER_H
#define GAME_CHARACTER_H
//----------------------------------------------------------------------------------------------------------------------
#include "GameAnimNetwork.h"
#include "GameCharacterController.h"
//----------------------------------------------------------------------------------------------------------------------
#include <string>
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \class GameCharacterBase
//----------------------------------------------------------------------------------------------------------------------
class GameCharacterBase
{
public:

  virtual ~GameCharacterBase();

  const char* getName() const { return m_name.c_str(); }
  GameAnimNetwork* getNetwork() const { return m_network; }
  GameCharacterControllerBase* getCharacterController() const { return m_characterController; }

protected:

  GameCharacterBase(const char* name, GameAnimNetwork* network, GameCharacterControllerBase* characterController);

private:

  std::string                         m_name;
  GameAnimNetwork*                    m_network;
  GameCharacterControllerBase*        m_characterController;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class GameCharacter
/// \brief A simplistic game character.
//----------------------------------------------------------------------------------------------------------------------
class GameCharacter : public GameCharacterBase
{
public:

  static GameCharacter* create(
    const char*                  name,
    const NMP::Vector3&          initialTranslation,
    const NMP::Quat&             initialOrientation,
    GameAnimNetworkDef*          networkDef,
    MR::AnimSetIndex             initialAnimSetIndex,
    NMP::FastHeapAllocator*      tempAllocator = NULL);

public:

  void update(float deltaTime);

private:

  GameCharacter(const char* name, GameAnimNetwork* network, GameCharacterControllerBase* characterController) :
    GameCharacterBase(name, network, characterController)
  {
  }
};

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_H
//----------------------------------------------------------------------------------------------------------------------
