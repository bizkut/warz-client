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
#include "GameCharacterController.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
GameCharacterController* GameCharacterController::create(
  const NMP::Vector3& initialTranslation,
  const NMP::Quat&    initialOrientation)
{
  return new GameCharacterController(initialTranslation, initialOrientation);
}

//----------------------------------------------------------------------------------------------------------------------
GameCharacterController::GameCharacterController(
  const NMP::Vector3& initialTranslation,
  const NMP::Quat&    initialOrientation):
m_characterController(NULL)
{
  NMP_ASSERT_MSG(initialTranslation == initialTranslation, "Finite values expected!");
  NMP_ASSERT_MSG(initialOrientation == initialOrientation, "Finite values expected!");

  m_characterPosition = initialTranslation;
  m_characterOrientation = initialOrientation;

  // Create a character controller.
  m_characterController = new NoPhysicsCharacterController();
}

//----------------------------------------------------------------------------------------------------------------------
void GameCharacterController::init(MR::Network*  network)
{
  //----------------------------
  m_network = network;
  // Set the world root transform to the character position/orientation
  m_network->updateCharacterPropertiesWorldRootTransform(NMP::Matrix34(m_characterOrientation, m_characterPosition), false);
}

//----------------------------------------------------------------------------------------------------------------------
void GameCharacterController::updateController(const NMP::Vector3& deltaTranslation, const NMP::Quat& deltaOrientation)
{
  // Accumulate trajectory.
  m_characterPosition += m_characterOrientation.rotateVector(deltaTranslation);
  m_characterOrientation *= deltaOrientation;
  m_characterOrientation.normalise();

  // update the networks world RootTransform to the adjusted position and orientation
  m_network->updateCharacterPropertiesWorldRootTransform(NMP::Matrix34(m_characterOrientation, m_characterPosition), true);
  m_network->setCharacterPropertiesAchievedRequestedMovement(true);
}

//----------------------------------------------------------------------------------------------------------------------
GameCharacterController::~GameCharacterController()
{
  delete m_characterController;
  m_characterController = NULL;
}
