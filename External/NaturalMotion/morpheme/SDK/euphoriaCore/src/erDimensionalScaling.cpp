#include "euphoria/erDimensionalScaling.h"
#include "physics/mrPhysicsRig.h"
#include "physics/mrPhysicsScene.h"

namespace ER
{

void DimensionalScaling::setFromPhysicsRig(const MR::PhysicsRig& physicsRig)
{
  float gravity = physicsRig.getPhysicsScene()->getGravity().magnitude();
  float bodyMass = physicsRig.calculateMass();
  float bodyLength = physicsRig.getBindPoseDimensions().magnitude();

  // The numbers here are chosen so that the scaling values are 1 on the character that the
  // behaviours have been developed on. So - they're "arbitrary", but appropriate for "normal"
  // characters.
  float referenceGravity = 9.8f;
  float referenceLength = 2.0137229f;
  float referenceMass = 75.924644f;

  if (gravity > 0.0f)
  {
    m_timeScale = sqrtf(bodyLength / gravity) / sqrtf(referenceLength / referenceGravity);
  }
  else
  {
    m_timeScale = 1.0f;
  }

  m_distScale = bodyLength / referenceLength;
  m_massScale = bodyMass / referenceMass;

}

}