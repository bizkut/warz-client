/*
* Copyright (c) 2013 NaturalMotion Ltd. All rights reserved.
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

//----------------------------------------------------------------------------------------------------------------------
#include "Helpers/ImpulseHelpers.h"

//----------------------------------------------------------------------------------------------------------------------

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
// Splits an impulse into component linear and angular parts, so it can apply enhancements as
// required e.g. torque multiplying
//----------------------------------------------------------------------------------------------------------------------
void applyEnhancedImpulseToPart(MR::PhysicsRig::Part* part, const ActorImpulse& actorImpulse)
{
  NMP_ASSERT(part);
  NMP::Vector3 impulseW = actorImpulse.hitDir * actorImpulse.impulseMagnitude;
  NMP::Vector3 posW = actorImpulse.point;

  NMP::Matrix34 actorToWorld;
  actorToWorld = part->getTransform();

  // Transform position from actor to global space if necessary
  if (!actorImpulse.positionFrameWorld)
  {
    actorToWorld.transformVector(posW);
  }

  // Transform direction from actor to global space if necessary
  if (!actorImpulse.directionFrameWorld)
  {
    actorToWorld.rotateVector(impulseW);
  }

  if (actorImpulse.useVelChange)
  {
    part->addVelocityChange(impulseW, posW);

    if (actorImpulse.torqueMultiplier != 1.0f)
    {
      // Apply a rotational boost by applying the scaled impulse either side of the CoM
      NMP::Vector3 actorCOMW = part->getCOMPosition();
      NMP::Vector3 offset = posW - actorCOMW;

      part->addVelocityChange(impulseW * (actorImpulse.torqueMultiplier - 1.0f) * 0.5f, posW + offset);
      part->addVelocityChange(impulseW * -(actorImpulse.torqueMultiplier - 1.0f) * 0.5f, posW - offset);
    }
  }
  else
  {
    // Extract torque component of localised impulse so that it can be boosted if necessary
    NMP::Vector3 actorCOMW = part->getCOMPosition();
    NMP::Vector3 torqueW = vCross(posW - actorCOMW, impulseW);
    torqueW *= actorImpulse.torqueMultiplier;

    part->addImpulse(impulseW);
    part->addTorqueImpulse(torqueW);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Apply a linear impulse to all parts of a limb.
// The magnitude of the impulse is proportional to the mass of each part.
// It will be applied in world space direction at a part's COM.
//----------------------------------------------------------------------------------------------------------------------
void applyImpulseToAllPartsOfLimb(ER::Limb* limb, const NMP::Vector3& linearImpulse)
{
  NMP_ASSERT(limb);
  const float limbMass = limb->getMass();
  NMP_ASSERT(limbMass > 0.0f);
  for (uint32_t i = 0; i < limb->getTotalNumParts(); ++i)
  {
    const float mass = limb->getPart(i)->getMass();
    NMP::Vector3 partImpulse = linearImpulse * mass / limbMass;
    // Apply impulse to COM in world space direction
    MR::PhysicsRig::Part* part = limb->getPart(i);
    part->addImpulse(partImpulse);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void applyVelocityChangeToAllPartsOfLimb(ER::Limb* limb, const NMP::Vector3& velocityChange)
{
  NMP_ASSERT(limb);
  for (uint32_t i = 0 ; i < limb->getTotalNumParts(); ++i)
  {
    MR::PhysicsRig::Part* part = limb->getPart(i);
    part->addLinearVelocityChange(velocityChange);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Apply a linear impulse to all the parts of the character.
// The magnitude of the impulse is proportional to the mass of each part.
// It will be applied in world space direction at a part's COM.
//----------------------------------------------------------------------------------------------------------------------
void applyImpulseToAllPartsOfCharacter(MR::PhysicsRig* pRig, const NMP::Vector3& linearImpulse)
{
  NMP_ASSERT(pRig);
  const float characterMass = pRig->calculateMass();
  NMP_ASSERT(characterMass> 0.0f);
  const int numParts = pRig->getNumParts();
  for (int i = 0; i < numParts; ++i)
  {
    const float partMass = pRig->getPart(i)->getMass();
    NMP::Vector3 partImpulse = linearImpulse * partMass / characterMass;
    // Apply impulse to COM in world space direction
    MR::PhysicsRig::Part* part = pRig->getPart(i);
    part->addImpulse(partImpulse);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void applyVelocityChangeToAllPartsOfCharacter(MR::PhysicsRig* pRig, const NMP::Vector3& velocityChange)
{
  NMP_ASSERT(pRig);
  const int numParts = pRig->getNumParts();
  for (int i = 0; i < numParts; ++i)
  {
    MR::PhysicsRig::Part* part = pRig->getPart(i);
    part->addLinearVelocityChange(velocityChange);
  }
}

}
