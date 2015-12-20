/*
 * Copyright (c) 2011 NaturalMotion Ltd. All rights reserved.
 *
 * Not to be copied, adapted, modified, used, distributed, sold,
 * licensed or commercially exploited in any manner without the
 * written consent of NaturalMotion.
 *
 * All non public elements of this software are the confidential
 * information of NaturalMotion and may not be disclosed to any
 * person nor used for any purpose not expressly approved by
 * NaturalMotion in writing.
 *
 */

#include "Types/DynamicState.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
DynamicState::DynamicState(float m)
{
  NMP_ASSERT(m > 0.0f);
  mass = m;
  positionTimesMass.setToZero();
  momentum.setToZero();
  angularMomentum.setToZero();
}

//----------------------------------------------------------------------------------------------------------------------
void DynamicState::setPosition(const NMP::Vector3& position)
{
  positionTimesMass = position * mass;
}

//----------------------------------------------------------------------------------------------------------------------
void DynamicState::setVelocity(const NMP::Vector3& velocity)
{
  momentum = velocity * mass;
}

//----------------------------------------------------------------------------------------------------------------------
void DynamicState::setAngularVelocity(const NMP::Vector3& angularVelocity)
{
  angularMomentum = angularVelocity * mass;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 DynamicState::getPosition() const
{
  return mass ? positionTimesMass / mass : NMP::Vector3Zero();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 DynamicState::getVelocity() const
{
  return mass ? momentum / mass : NMP::Vector3Zero();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 DynamicState::getAngularVelocity() const
{
  return mass ? angularMomentum / mass : NMP::Vector3Zero();
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

