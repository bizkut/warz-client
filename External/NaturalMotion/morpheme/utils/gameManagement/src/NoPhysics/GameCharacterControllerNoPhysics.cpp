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
#include "gameManagement/NoPhysics/GameCharacterControllerNoPhysics.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
CharacterControllerNoPhysics* CharacterControllerNoPhysics::create(
  const NMP::Vector3& initialPosition,
  const NMP::Quat&    initialOrientation)
{
  CharacterControllerNoPhysics* const instance = static_cast<CharacterControllerNoPhysics*>(NMPMemoryAlloc(sizeof(CharacterControllerNoPhysics)));
  new(instance) CharacterControllerNoPhysics(initialPosition, initialOrientation);

  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerNoPhysics::destroy(CharacterControllerNoPhysics* characterController)
{
  NMP_ASSERT(characterController);

  NMP::Memory::memFree(characterController);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerNoPhysics::init(MR::Network* net)
{
  m_net = net;

  //----------------------------
  // Set the world root transform to the character position/orientation
  m_net->updateCharacterPropertiesWorldRootTransform(NMP::Matrix34(m_characterOrientation, m_characterPosition), false);
}

//---------------------------------------------------------------------------------------------------------------------
void CharacterControllerNoPhysics::updateControllerMove(
  const NMP::Vector3* requestedDeltaWorldTranslation,
  const NMP::Quat*    requestedDeltaWorldOrientation)
{
  if (requestedDeltaWorldTranslation && requestedDeltaWorldOrientation)
  {
    // Update the controller transform using the delta values that have been supplied.
    //  This path allows the caller to override 
    m_characterPosition += (*requestedDeltaWorldTranslation);
    m_characterOrientation *= (*requestedDeltaWorldOrientation);
  }
  else
  {
    // Update the controller transform using the delta values directly calculated by the Network.
    m_characterPosition += m_characterOrientation.rotateVector(m_net->getTranslationChange());
    m_characterOrientation *= m_net->getOrientationChange();
    m_characterOrientation.normalise();
  }

  // Update the Networks world RootTransform with the new position and orientation.
  m_net->updateCharacterPropertiesWorldRootTransform(NMP::Matrix34(m_characterOrientation, m_characterPosition), true);
  m_net->setCharacterPropertiesAchievedRequestedMovement(true);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerNoPhysics::setTransform(
  const NMP::Vector3& position,
  const NMP::Quat&    orientation)
{
  m_characterPosition = position;
  m_characterOrientation = orientation;
}

//---------------------------------------------------------------------------------------------------------------------
// Set the character's position
void CharacterControllerNoPhysics::setPosition(const NMP::Vector3& position)
{
  m_characterPosition = position;
}

//---------------------------------------------------------------------------------------------------------------------
// Set the character's orientation
void CharacterControllerNoPhysics::setOrientation(const NMP::Quat& orientation)
{
  m_characterOrientation = orientation;
}

//----------------------------------------------------------------------------------------------------------------------
CharacterControllerNoPhysics::CharacterControllerNoPhysics(
  const NMP::Vector3& initialPosition,
  const NMP::Quat&    initialOrientation)
{
  NMP_ASSERT_MSG(initialPosition == initialPosition, "Finite values expected!");
  NMP_ASSERT_MSG(initialOrientation == initialOrientation, "Finite values expected!");

  m_characterPosition = initialPosition;
  m_characterOrientation = initialOrientation;
}

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
