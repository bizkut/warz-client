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

#include "euphoria/erDebugDraw.h"

#include "Types/LimbControl.h"
#include "Types/ContactInfo.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
static const float minWeighting = 1e-8f;

//----------------------------------------------------------------------------------------------------------------------
LimbControl::LimbControl()
{
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::reinit(float stiffness, float dampingRatio, float driveCompensation)
{
  NMP_ASSERT(stiffness >= 0.0f);
  NMP_ASSERT(dampingRatio >= 0.0f);
  NMP_ASSERT(driveCompensation >= 0.0f);
  // Relationship between strength and stiffness, and between damping and damping ratio, come from
  // standard results on damped harmonic oscillators (e.g. look up critically damped spring). Note
  // that stiffness is the (undamped) natural frequency of the spring (often called w0).
  m_strength = NMP::sqr(stiffness);
  m_damping = 2.0f * dampingRatio * stiffness;

  m_driveCompensation = driveCompensation;
  m_weightedPositionWeight = 0.0f;
  m_weightedOrientationWeight = 0.0f;
  m_weightedTargetPosition.setToZero();
  m_weightedTargetOrientation.identity();
  m_weightedTargetNormal.setToZero();
  m_weightedNormalWeight = 0.0f;
  m_endSupportAmount = 0.0f;
  m_rootSupportedAmount = 0.0f;
  m_endFrictionScale = 1.0f;
  m_implicitStiffness = 0.0f;
  m_skinWidthIncrease = 0.0f;
  m_weightedGravityCompensation = 0.0f;
  m_weightedSupportForceMultipler = m_strength;
  m_softLimitStiffnessScale = 1.0f;
  m_weightedRootPositionWeight = 0.0f;
  m_weightedRootOrientationWeight = 0.0f;
  m_weightedRootTargetPosition.setToZero();
  m_weightedRootTargetOrientation.identity();
  m_weightedLocalNormal.set(m_strength, 0, 0);

  m_weightedRootTargetVelocity.setToZero();
  m_weightedRootTargetAngularVelocity.setToZero();
  m_weightedTargetVelocity.setToZero();
  m_weightedTargetAngularVelocity.setToZero();

  m_collisionGroupIndex = -1.0f;
#if defined(MR_OUTPUT_DEBUGGING)
  m_debugControlAmounts.setToZero();
#endif
  m_targetDeltaStep = 1.0f;
  m_useIncrementalIK = 1.0f;
  m_swivelAmount = 0.0f;
  m_strengthReductionTowardsEnd = 0.0f;

  ER::assertIsInitialised(this);
}

//----------------------------------------------------------------------------------------------------------------------
LimbControl::LimbControl(float stiffness, float dampingRatio, float driveCompensation)
{
  reinit(stiffness, dampingRatio, driveCompensation);
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setDamping(float damping)
{
  m_damping = damping;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setControlAmount(uint32_t MR_OUTPUT_DEBUG_ARG(controlType), float MR_OUTPUT_DEBUG_ARG(amount))
{
#if defined(MR_OUTPUT_DEBUGGING)
  m_debugControlAmounts.setControlAmount((ER::DebugControl) controlType, amount);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
const ER::LimbControlAmounts* LimbControl::getControlAmounts() const
{
#if defined(MR_OUTPUT_DEBUGGING)
  return &m_debugControlAmounts;
#else
  return 0;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setTargetPos(
  const NMP::Vector3& targetPos,
  float positionWeight,
  const NMP::Vector3& velocityOfTargetPos)
{
  NMP_ASSERT(targetPos.isValid());
  NMP_ASSERT(velocityOfTargetPos.isValid());
  NMP_ASSERT(m_strength >= 0.0f);
  m_weightedPositionWeight = positionWeight * m_strength;
  m_weightedTargetPosition = targetPos * m_weightedPositionWeight;
  m_weightedTargetVelocity = velocityOfTargetPos * positionWeight * m_damping;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setTargetNormal(
  const NMP::Vector3& targetNormal,
  float normalWeight,
  const NMP::Vector3& angularVelocityOfTargetNormal,
  const NMP::Vector3& localNormalVector)
{
  NMP_ASSERT(normalWeight == 0.0f || targetNormal.isValid());
  NMP_ASSERT(normalWeight == 0.0f || angularVelocityOfTargetNormal.isValid());
  NMP_ASSERT(m_strength >= 0.0f);
  m_weightedNormalWeight = normalWeight * m_strength;
  m_weightedTargetNormal = targetNormal * m_weightedNormalWeight;
  m_weightedTargetAngularVelocity = angularVelocityOfTargetNormal * normalWeight * m_damping;
  m_weightedLocalNormal = localNormalVector * m_weightedNormalWeight;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setTargetOrientation(
  const NMP::Matrix34& targetOrientation,
  float orientWeight,
  const NMP::Vector3& angularVelocityOfTargetOrientation)
{
  NMP_ASSERT(orientWeight == 0.0f || targetOrientation.isValidTM(0.05f));
  NMP_ASSERT(orientWeight == 0.0f || angularVelocityOfTargetOrientation.isValid());
  NMP_ASSERT(m_strength >= 0.0f);
  m_weightedOrientationWeight = orientWeight * m_strength;
  m_weightedTargetOrientation = targetOrientation.toQuat() * m_weightedOrientationWeight;
  m_weightedTargetAngularVelocity = angularVelocityOfTargetOrientation * orientWeight * m_damping;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setTargetOrientation(
  const NMP::Quat& targetOrientation, float orientWeight,
  const NMP::Vector3& angularVelocityOfTargetOrientation)
{
  NMP_ASSERT(orientWeight == 0.0f || targetOrientation.isNormal());
  NMP_ASSERT(orientWeight == 0.0f || angularVelocityOfTargetOrientation.isValid());
  NMP_ASSERT(m_strength >= 0.0f);
  m_weightedOrientationWeight = orientWeight * m_strength;
  m_weightedTargetOrientation = targetOrientation * m_weightedOrientationWeight;
  m_weightedTargetAngularVelocity = angularVelocityOfTargetOrientation * orientWeight * m_damping;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setExpectedRootPosForTarget(
  const NMP::Vector3& rootTargetPos,
  float rootPositionWeight,
  const NMP::Vector3& velocityOfRootTarget)
{
  NMP_ASSERT(rootPositionWeight == 0.0f || velocityOfRootTarget.isValid());
  NMP_ASSERT(m_strength >= 0.0f);
  m_weightedRootPositionWeight = rootPositionWeight * m_strength;
  m_weightedRootTargetPosition = rootTargetPos * m_weightedRootPositionWeight;
  float rootTargetVelWeight = rootPositionWeight * m_damping;
  m_weightedRootTargetVelocity = velocityOfRootTarget * rootTargetVelWeight;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setExpectedRootOrientationForTarget(
  const NMP::Quat& rootTargetOrientation,
  float rootOrientationWeight,
  const NMP::Vector3& angularVelocityOfRootTarget)
{
  NMP_ASSERT(rootOrientationWeight == 0.0f || angularVelocityOfRootTarget.isValid());
  NMP_ASSERT(m_strength >= 0.0f);
  m_weightedRootOrientationWeight = rootOrientationWeight * m_strength;
  m_weightedRootTargetOrientation = rootTargetOrientation * m_weightedRootOrientationWeight;
  float rootTargetVelWeight = rootOrientationWeight * m_damping;
  m_weightedRootTargetAngularVelocity = angularVelocityOfRootTarget * rootTargetVelWeight;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setExpectedRootForTarget(
  const NMP::Matrix34& rootTarget,
  float rootTargetWeight,
  const NMP::Vector3& velocityOfRootTarget,
  const NMP::Vector3& angularVelocityOfRootTarget)
{
  setExpectedRootPosForTarget(rootTarget.translation(), rootTargetWeight, velocityOfRootTarget);
  setExpectedRootOrientationForTarget(rootTarget.toQuat(), rootTargetWeight, angularVelocityOfRootTarget);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 LimbControl::getExpectedRootPosForTarget() const
{
  return m_weightedRootTargetPosition / (m_weightedRootPositionWeight + 1e-10f);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat LimbControl::getExpectedRootOrientationForTarget() const
{
  NMP::Quat rootTargetOrient;
  if (m_weightedRootOrientationWeight)
    rootTargetOrient.normalise(m_weightedRootTargetOrientation);
  else
    rootTargetOrient.setXYZW(0, 0, 0, 1);
  return rootTargetOrient;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 LimbControl::getExpectedRootForTarget() const
{
  return NMP::Matrix34(getExpectedRootOrientationForTarget(), getExpectedRootPosForTarget());
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setTargetDeltaStep(float w)
{
  m_targetDeltaStep = w;
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getTargetDeltaStep() const
{
  return m_targetDeltaStep;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setStrengthReductionTowardsEnd(float reduction)
{
  m_strengthReductionTowardsEnd = reduction;
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getStrengthReductionTowardsEnd() const
{
  return m_strengthReductionTowardsEnd;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setCollisionGroupIndex(float index)
{
  m_collisionGroupIndex = index;
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getCollisionGroupIndex() const
{
  return m_collisionGroupIndex;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setEndSupportAmount(float endSupportAmount)
{
  m_endSupportAmount = endSupportAmount;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setRootSupportedAmount(float rootSupportedAmount)
{
  m_rootSupportedAmount = rootSupportedAmount;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setEndFrictionScale(float endFrictionScale)
{
  m_endFrictionScale = endFrictionScale;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setImplicitStiffness(float implicitStiffness)
{
  m_implicitStiffness = implicitStiffness;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setSkinWidthIncrease(float skinWidthIncrease)
{
  m_skinWidthIncrease = skinWidthIncrease;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setGravityCompensation(float gravityCompensation)
{
  // allow GC > 1 as it's just a clamp
  NMP_ASSERT(gravityCompensation >= 0.0f);
  NMP_ASSERT(m_strength >= 0.0f);
  m_weightedGravityCompensation = gravityCompensation * m_strength;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setSupportForceMultiplier(float supportForceMultipler)
{
  NMP_ASSERT(m_strength >= 0.0f);
  m_weightedSupportForceMultipler = supportForceMultipler * m_strength;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setSoftLimitStiffnessScale(float softLimitStiffnessScale)
{
  m_softLimitStiffnessScale = softLimitStiffnessScale;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setSwivelAmount(float swivelAmount)
{
  NMP_ASSERT(-1.0f <= swivelAmount && swivelAmount <= 1.0f);
  m_swivelAmount = swivelAmount;
}

//----------------------------------------------------------------------------------------------------------------------
void LimbControl::setUseIncrementalIK(bool useIncrementalIK)
{
  m_useIncrementalIK = (useIncrementalIK)? 1.0f : 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 LimbControl::getTargetPos() const
{
  return m_weightedTargetPosition / (m_weightedPositionWeight + minWeighting);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 LimbControl::getTargetNormal() const
{
  return m_weightedNormalWeight ? NMP::vNormalise(m_weightedTargetNormal) : NMP::Vector3(1, 0, 0);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 LimbControl::getLocalNormalVector() const
{
  return m_weightedNormalWeight ? NMP::vNormalise(m_weightedLocalNormal) : NMP::Vector3(1, 0, 0);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat LimbControl::getTargetOrientation() const
{
  NMP::Quat quat;
  if (m_weightedOrientationWeight)
    quat.normalise(m_weightedTargetOrientation);
  else
    quat.setXYZW(0, 0, 0, 1);
  return quat;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 LimbControl::getTargetsVelocity() const
{
  NMP_ASSERT(m_strength >= 0.0f);
  return m_weightedTargetVelocity * m_strength / (m_weightedPositionWeight * m_damping + minWeighting);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 LimbControl::getTargetsAngularVelocity() const
{
  NMP_ASSERT(m_strength >= 0.0f);
  return m_weightedTargetAngularVelocity * m_strength / (m_weightedOrientationWeight * m_damping + minWeighting);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 LimbControl::getRootsVelocity() const
{
  NMP_ASSERT(m_strength >= 0.0f);
  return m_weightedRootTargetVelocity * m_strength / (m_weightedRootPositionWeight * m_damping + minWeighting);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 LimbControl::getRootsAngularVelocity() const
{
  NMP_ASSERT(m_strength >= 0.0f);
  return m_weightedRootTargetAngularVelocity * m_strength / 
    (m_weightedRootOrientationWeight * m_damping + minWeighting); 
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getPositionWeight() const
{
  NMP_ASSERT(m_strength >= 0.0f);
  return m_weightedPositionWeight / (m_strength + minWeighting);
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getRootPositionWeight() const
{
  NMP_ASSERT(m_strength >= 0.0f);
  return m_weightedRootPositionWeight / (m_strength + minWeighting);
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getRootOrientationWeight() const
{
  NMP_ASSERT(m_strength >= 0.0f);
  return m_weightedRootOrientationWeight / (m_strength + minWeighting);
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getNormalWeight() const
{
  NMP_ASSERT(m_strength >= 0.0f);
  return m_weightedNormalWeight / (m_strength + minWeighting);
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getOrientationWeight() const
{
  NMP_ASSERT(m_strength >= 0.0f);
  return m_weightedOrientationWeight / (m_strength + minWeighting);
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getStrength() const
{
  NMP_ASSERT(m_strength >= 0.0f);
  return m_strength;
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getDamping() const
{
  return m_damping;
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getStiffness() const
{
  NMP_ASSERT(m_strength >= 0.0f);
  return sqrtf(m_strength);
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getDampingRatio() const
{
  NMP_ASSERT(m_strength >= 0.0f);
  return m_damping / (2.0f * sqrtf(m_strength));
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getDriveCompensation() const
{
  NMP_ASSERT(m_strength >= 0.0f);
  return m_driveCompensation; // this is so the compensation adds/blends just like the stiffness does
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getEndSupportAmount() const
{
  return m_endSupportAmount;
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getRootSupportedAmount() const
{
  return m_rootSupportedAmount;
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getEndFrictionScale() const
{
  return m_endFrictionScale;
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getImplicitStiffness() const
{
  return m_implicitStiffness;
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getSkinWidthIncrease() const
{
  return m_skinWidthIncrease;
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getGravityCompensation() const
{
  NMP_ASSERT(m_strength >= 0.0f);
  return m_weightedGravityCompensation / (m_strength + minWeighting);
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getSupportForceMultiplier() const
{
  NMP_ASSERT(m_strength >= 0.0f);
  return m_weightedSupportForceMultipler / (m_strength + minWeighting);
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getSoftLimitStiffnessScale() const
{
  NMP_ASSERT(m_strength >= 0.0f);
  return m_softLimitStiffnessScale;
}

//----------------------------------------------------------------------------------------------------------------------
float LimbControl::getSwivelAmount() const
{
  return m_swivelAmount;
}

//----------------------------------------------------------------------------------------------------------------------
bool LimbControl::getUseIncrementalIK() const
{
  return m_useIncrementalIK > 0.0f;
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

