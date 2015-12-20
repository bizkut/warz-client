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
#include "NMPlatform/NMMemory.h"
#include "physics/mrPhysicsRigDef.h"
#include "sharedDefines/mPhysicsDebugInterface.h"
#include "NMGeomUtils/NMGeomUtils.h"

//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
// PhysicsSixDOFJointModifiableLimits
//----------------------------------------------------------------------------------------------------------------------
PhysicsSixDOFJointModifiableLimits::PhysicsSixDOFJointModifiableLimits()
  : m_swing1Limit(0.0f)
  , m_swing2Limit(0.0f)
  , m_twistLimitLow(0.0f)
  , m_twistLimitHigh(0.0f)
{
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsSixDOFJointModifiableLimits::clampToLimits(
  NMP::Quat&             q,
  float                  limitFrac,
  const PhysicsJointDef& jointDef,
  const NMP::Quat&       origQ) const
{
  // Get the local joint axes in each frame as l0, l1
  // axis = x, normal = y, perp = z
  NMP::Quat l0 = jointDef.m_parentPartFrame.toQuat();
  NMP::Quat l1 = jointDef.m_childPartFrame.toQuat();

  // now "assuming" the parent is at the origin (since we already have the relative rotation q)
  // we want to calculate rot, the relative rotation of the child local frame from the parent local frame
  NMP::Quat l0Inv = ~l0;
  NMP::Quat l1Inv = ~l1;

  NMP::Quat rot = l0Inv * q * l1;
  NMP::Quat origRot = l0Inv * origQ * l1;

  // convert to twist, swing1/2 and limit
  NMP::Vector3 tss, origTss;
  NMRU::GeomUtils::quatToTwistSwing(rot, tss);
  NMRU::GeomUtils::quatToTwistSwing(origRot, origTss);
  origTss *= limitFrac;

  // swing
  float s1Max = tanf(limitFrac * getSwing1Limit() / 4.0f);
  float s2Max = tanf(limitFrac * getSwing2Limit() / 4.0f);

  if (s1Max > 0.0f && s2Max > 0.0f)
  {
    if (NMRU::GeomUtils::isPointOutsideEllipse(s2Max, s1Max, tss.y, tss.z))
    {
      if (NMRU::GeomUtils::isPointOutsideEllipse(s2Max, s1Max, origTss.y, origTss.z))
      {
        // if the destination is outside the ellipse then clamp it - to either the ellipse, or
        // if the original values are also outside the ellipse then clamp to the ellipse that they lie on
        float aSquared = (origTss.y / s2Max) * (origTss.y / s2Max) + (origTss.z / s1Max) * (origTss.z / s1Max);
        float a = sqrtf(aSquared);
        s2Max = a * s2Max;
        s1Max = a * s1Max;

        // clamp if the new value is further out than the old
        if (NMRU::GeomUtils::isPointOutsideEllipse(s2Max, s1Max, tss.y, tss.z))
        {
          NMRU::GeomUtils::closestPointOnEllipse_newton(tss.y, tss.z, s2Max, s1Max);
        }
      }
      else
      {
        // destination is outside the ellipse, but original is inside - so clamp to the original
        NMRU::GeomUtils::closestPointOnEllipse_newton(tss.y, tss.z, s2Max, s1Max);
      }
    }
  }
  else if (s1Max > 0.0f)
  {
    NMRU::GeomUtils::preventValueDivergingFurtherFromLimits(
      tss.y, -s2Max, s2Max, origTss.y);
    NMRU::GeomUtils::preventValueDivergingFurtherFromLimits(
      tss.z, -s1Max, s1Max, origTss.z);
  }
  // twist
  // note that the twist limit direction is reversed compared to our calculations
  NMRU::GeomUtils::preventValueDivergingFurtherFromLimits(
    tss.x,
    -tanf(limitFrac * getTwistLimitHigh() / 4.0f),
    -tanf(limitFrac * getTwistLimitLow() / 4.0f),
    origTss.x);

  // limited - now convert back
  NMRU::GeomUtils::twistSwingToQuat(tss, rot);
  q = l0 * rot * l1Inv;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsSixDOFJointModifiableLimits::clampToLimits(
  NMP::Quat&             q,
  float                  limitFrac,
  const PhysicsJointDef& jointDef) const
{
  // Get the local joint axes in each frame as l0, l1
  // axis = x, normal = y, perp = z
  NMP::Quat l0 = jointDef.m_parentPartFrame.toQuat();
  NMP::Quat l1 = jointDef.m_childPartFrame.toQuat();

  // now "assuming" the parent is at the origin (since we already have the relative rotation q)
  // we want to calculate rot, the relative rotation of the child local frame from the parent local frame
  NMP::Quat l0Inv = ~l0;
  NMP::Quat l1Inv = ~l1;

  NMP::Quat rot = l0Inv * q * l1;

  // convert to twist, swing1/2 and limit
  NMP::Vector3 tss;
  NMRU::GeomUtils::quatToTwistSwing(rot, tss);

  // swing
  float s1Max = tanf(limitFrac * getSwing1Limit() / 4.0f);
  float s2Max = tanf(limitFrac * getSwing2Limit() / 4.0f);

  if (s1Max > 0.0f && s2Max > 0.0f)
  {
    if (NMRU::GeomUtils::isPointOutsideEllipse(s2Max, s1Max, tss.y, tss.z))
    {
      // destination is outside the ellipse, but original is inside - so clamp to the original
      NMRU::GeomUtils::closestPointOnEllipse_newton(tss.y, tss.z, s2Max, s1Max);
    }
  }
  else
  {
    // Simple clamp if one or both of the ranges is zero
    tss.y = NMP::clampValue(tss.y, -s2Max, s2Max);
    tss.z = NMP::clampValue(tss.z, -s1Max, s1Max);
  }
  // twist
  // note that the twist limit direction is reversed compared to our calculations
  tss.x = NMP::clampValue(
    tss.x, 
    -tanf(limitFrac * getTwistLimitHigh() / 4.0f), 
    -tanf(limitFrac * getTwistLimitLow() / 4.0f));

  // limited - now convert back
  NMRU::GeomUtils::twistSwingToQuat(tss, rot);
  q = l0 * rot * l1Inv;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsSixDOFJointModifiableLimits::expand(const NMP::Quat& orientation, const PhysicsJointDef& jointDef)
{
  // Get the local joint axes in each frame as l0, l1
  // axis = x, normal = y, perp = z
  NMP::Quat l0 = jointDef.m_parentPartFrame.toQuat();
  NMP::Quat l1 = jointDef.m_childPartFrame.toQuat();

  // now "assuming" the parent is at the origin (since we already have the relative rotation q)
  // we want to calculate rot, the relative rotation of the child local frame from the parent local frame
  NMP::Quat l0Inv = ~l0;
  NMP::Quat rot = l0Inv * orientation * l1;

  // convert to twist, swing1/2 and limit
  NMP::Vector3 twistAndSwing;
  NMRU::GeomUtils::quatToTwistSwing(rot, twistAndSwing);

  // Expand twist limit.
  const float twist(4.0f * atanf(-twistAndSwing.x)); // Map twist out of tan quarter space.

  m_twistLimitLow = (twist < m_twistLimitLow) ? twist : m_twistLimitLow;
  m_twistLimitHigh = (twist > m_twistLimitHigh) ? twist : m_twistLimitHigh;

  // Expand swing
  const float swing1(twistAndSwing.z);
  const float swing2(twistAndSwing.y);

  // Map swing limits into tan quarter space.
  // Zero swing limits are allowed - they're only clamped when written to the physics engine if
  // necessary. We still want to be able to expand hinge axes if necessary, so add just enough to
  // avoid division by zero.
  const float swing1Limit = NMP::maximum(tanf(m_swing1Limit / 4.0f), 0.001f);
  const float swing2Limit = NMP::maximum(tanf(m_swing2Limit / 4.0f), 0.001f);


  if (NMRU::GeomUtils::isPointOutsideEllipse(swing1Limit, swing2Limit, swing1, swing2))
  {
    const float swing1Term = (swing1 / swing1Limit);
    const float swing2Term = (swing2 / swing2Limit);

    // Add epsilon to ensure that errors from fastSqrt don't lead to slightly too small joint limit.
    const float scale = NMP::fastSqrt((swing1Term * swing1Term) + (swing2Term * swing2Term)) + 0.01f;

    const float swing1LimitExpanded = scale * swing1Limit;
    const float swing2LimitExpanded = scale * swing2Limit;

    NMP_ASSERT(!NMRU::GeomUtils::isPointOutsideEllipse(swing1LimitExpanded, swing2LimitExpanded, swing1, swing2));

    // Map swing limits out of tan quarter space.
    m_swing1Limit = NMP::minimum(4.0f * atanf(swing1LimitExpanded), NM_PI);
    m_swing2Limit = NMP::minimum(4.0f * atanf(swing2LimitExpanded), NM_PI);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsSixDOFJointModifiableLimits::scale(float scaleFactor)
{
  NMP_ASSERT(scaleFactor > 0.0f);

  // Scale twist limits (about the centre of range).
  float twistMid = (m_twistLimitHigh + m_twistLimitLow) * 0.5f;
  float scaledTwistHalfRange = (m_twistLimitHigh - m_twistLimitLow) * 0.5f * scaleFactor;
  m_twistLimitLow  = NMP::maximum(twistMid - scaledTwistHalfRange, -NM_PI);
  m_twistLimitHigh = NMP::minimum(twistMid + scaledTwistHalfRange * scaleFactor, NM_PI);

  // Scale swing limits.
  m_swing1Limit = NMP::clampValue(m_swing1Limit * scaleFactor, 0.0f, NM_PI);
  m_swing2Limit = NMP::clampValue(m_swing2Limit * scaleFactor, 0.0f, NM_PI);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsSixDOFJointModifiableLimits::endianSwap()
{
  NMP::endianSwap(m_swing1Limit);
  NMP::endianSwap(m_swing2Limit);
  NMP::endianSwap(m_twistLimitLow);
  NMP::endianSwap(m_twistLimitHigh);
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsSixDOFJointLimits
//----------------------------------------------------------------------------------------------------------------------
PhysicsSixDOFJointLimits::PhysicsSixDOFJointLimits()
  : m_swingLimitContactDistance(0.0f)
  , m_twistLimitContactDistance(0.0f)
{}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsSixDOFJointLimits::endianSwap()
{
  m_modifiableLimits.endianSwap();
  NMP::endianSwap(m_swingLimitContactDistance);
  NMP::endianSwap(m_twistLimitContactDistance);
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsRagdollJointLimits
//----------------------------------------------------------------------------------------------------------------------
void PhysicsRagdollJointLimits::clampToLimits(
  NMP::Quat&              orientation,
  float                   limitFraction,
  const PhysicsJointDef&  jointDef) const
{
  const NMP::Vector3 xAxis = NMP::Vector3XAxis();

  //-------------------------
  // compute the fractional limits of this joint
  const float twistLimitMinFraction = limitFraction * m_twistMinAngle;
  const float twistLimitMaxFraction = limitFraction * m_twistMaxAngle;

  const float coneLimitFraction = limitFraction * m_coneAngle;

  const float planeLimitMinFraction = limitFraction * m_planeMinAngle;
  const float planeLimitMaxFraction = limitFraction * m_planeMaxAngle;

  bool clamped = false;

  //-------------------------
  // convert the orientation in body space in to the orientation in joint space.
  NMP::Quat jointSpaceOrientation;

  NMP::Quat parentPartFrameOrientation = jointDef.m_parentPartFrame.toQuat();
  NMP::Quat inverseParentOrientation = parentPartFrameOrientation;
  inverseParentOrientation.conjugate();

  NMP::Quat childPartFrameOrientation = jointDef.m_childPartFrame.toQuat();
  jointSpaceOrientation = inverseParentOrientation * orientation * childPartFrameOrientation;

  //-------------------------
  // convert the orientation to be clamped to a direction vector
  NMP::Vector3 direction;
  jointSpaceOrientation.rotateVector(xAxis, direction);

  //-------------------------
  // clamp twist independently
  float twistAngle = jointSpaceOrientation.toRotationVector().x;
  if (!NMP::valueInRange(twistAngle, twistLimitMinFraction, twistLimitMaxFraction))
  {
    clamped = true;

    twistAngle = NMP::clampValue(twistAngle, twistLimitMinFraction, twistLimitMaxFraction);
  }

  //-------------------------
  // clamp cone angle
  // project the cone limit on to the x-axis and compare with direction.x
  // if direction.x is less than the projected cone length then the limit was violated.
  float cosConeLimit = cos(coneLimitFraction);
  if (direction.x < cosConeLimit)
  {
    clamped = true;

    direction.x = cosConeLimit;

    // reset the other direction components
    float scaleFactor = NMP::fastSqrt((1.0f - NMP::sqr(direction.x)) / (NMP::sqr(direction.y) + NMP::sqr(direction.z)));
    direction.y = scaleFactor * direction.y;
    direction.z = scaleFactor * direction.z;
  }

  //-------------------------
  // clamp plane angle
  // project the plane limits on to the y-axis and compare with direction.x
  // angle between the y-axis and the cone side is ((pi/2) - angle) so use sin not cos
  // if direction.y is less than the projected cone length then the limit was violated.
  float sinPlaneMinLimit = sin(planeLimitMinFraction);
  float sinPlaneMaxLimit = sin(planeLimitMaxFraction);
  if (!NMP::valueInRange(direction.z, sinPlaneMinLimit, sinPlaneMaxLimit))
  {
    clamped = true;

    direction.z = NMP::clampValue(direction.z, sinPlaneMinLimit, sinPlaneMaxLimit);

    // reset the other direction components
    float scaleFactor = NMP::fastSqrt((1.0f - NMP::sqr(direction.z)) / (NMP::sqr(direction.x) + NMP::sqr(direction.y)));
    direction.x = scaleFactor * direction.x;
    direction.y = scaleFactor * direction.y;
  }

  //-------------------------
  // now all degrees have been clamped generate the new orientation
  if (clamped)
  {
    NMP::Quat conePlaneOrientation;
    conePlaneOrientation.forRotation(xAxis, direction);

    NMP::Quat twistOrientation;
    twistOrientation.fromAxisAngle(xAxis, twistAngle);

    jointSpaceOrientation.multiply(conePlaneOrientation, twistOrientation);

    // convert back from the orientation in joint space to the orientation in to body space.
    NMP::Quat inverseChildOrientation = childPartFrameOrientation;
    inverseChildOrientation.conjugate();
    orientation = parentPartFrameOrientation * jointSpaceOrientation * inverseChildOrientation;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRagdollJointLimits::expand(const NMP::Quat& orientation, const PhysicsJointDef& jointDef)
{
  //-------------------------
  // convert the orientation in body space in to the orientation in joint space.
  NMP::Quat jointSpaceOrientation;

  NMP::Quat parentPartFrameOrientation = jointDef.m_parentPartFrame.toQuat();
  NMP::Quat inverseParentOrientation = parentPartFrameOrientation;
  inverseParentOrientation.conjugate();

  NMP::Quat childPartFrameOrientation = jointDef.m_childPartFrame.toQuat();
  jointSpaceOrientation = inverseParentOrientation * orientation * childPartFrameOrientation;

  const NMP::Vector3 xAxis = NMP::Vector3XAxis();

  //-------------------------
  // convert the orientation to be clamped to a direction vector
  NMP::Vector3 direction;
  jointSpaceOrientation.rotateVector(xAxis, direction);

  const float twistAngle = jointSpaceOrientation.toRotationVector().x;
  const float coneAngle  = acosf(direction.x);
  const float planeAngle = asinf(direction.z);

  //-------------------------
  // expand twist independently
  m_twistMinAngle = NMP::minimum(m_twistMinAngle, twistAngle);
  m_twistMaxAngle = NMP::maximum(m_twistMaxAngle, twistAngle);

  //-------------------------
  // expand cone angle
  m_coneAngle = NMP::maximum(m_coneAngle, coneAngle);

  //-------------------------
  // expand plane angle

  const float planeMin = NMP::minimum(planeAngle, m_planeMinAngle);
  const float planeMax = NMP::maximum(planeAngle, m_planeMaxAngle);

  m_planeMinAngle = (direction.z < 0.0f) ? planeMin : m_planeMinAngle;
  m_planeMaxAngle = (direction.z > 0.0f) ? planeMax : m_planeMaxAngle;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRagdollJointLimits::scale(float scaleFactor)
{
  NMP_ASSERT_FAIL_MSG("As yet untested implemetation.");

  // Scale twist limits (about the centre of range).
  float twistMid = (m_twistMinAngle + m_twistMaxAngle) * 0.5f;
  float scaledTwistHalfRange = (m_twistMaxAngle - m_twistMinAngle) * 0.5f * scaleFactor;
  m_twistMinAngle = NMP::clampValue(twistMid - scaledTwistHalfRange, -NM_PI_OVER_TWO, 0.0f);
  m_twistMaxAngle = NMP::clampValue(twistMid + scaledTwistHalfRange, 0.0f, NM_PI_OVER_TWO);

  // Scale cone limit.
  m_coneAngle = NMP::clampValue(m_coneAngle * scaleFactor, 0.0f, NM_PI);

  // Scale plane limits.
  m_planeMinAngle = NMP::clampValue(m_planeMinAngle * scaleFactor, -NM_PI_OVER_TWO, 0.0f);
  m_planeMaxAngle = NMP::clampValue(m_planeMaxAngle * scaleFactor, 0.0f, NM_PI_OVER_TWO);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRagdollJointLimits::endianSwap()
{
  NMP::endianSwap(m_coneAngle);
  NMP::endianSwap(m_planeMinAngle);
  NMP::endianSwap(m_planeMaxAngle);
  NMP::endianSwap(m_twistMinAngle);
  NMP::endianSwap(m_twistMaxAngle);
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsJointDef
//----------------------------------------------------------------------------------------------------------------------
void PhysicsJointDef::locate()
{
  NMP::endianSwap(m_jointType);

  NMP::endianSwap(m_parentPartFrame);
  NMP::endianSwap(m_childPartFrame);

  NMP::endianSwap(m_parentPartIndex);
  NMP::endianSwap(m_childPartIndex);

  NMP::endianSwap(m_parentFrameQuat);
  NMP::endianSwap(m_childFrameQuat);

  REFIX_SWAP_PTR(char, m_name);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsJointDef::dislocate()
{
  NMP::endianSwap(m_jointType);

  NMP::endianSwap(m_parentPartFrame);
  NMP::endianSwap(m_childPartFrame);

  NMP::endianSwap(m_parentPartIndex);
  NMP::endianSwap(m_childPartIndex);

  NMP::endianSwap(m_parentFrameQuat);
  NMP::endianSwap(m_childFrameQuat);

  UNFIX_SWAP_PTR(char, m_name);
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsSixDOFJointDef::SoftLimit
//----------------------------------------------------------------------------------------------------------------------
void PhysicsSixDOFJointDef::SoftLimit::locate()
{
  endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsSixDOFJointDef::SoftLimit::dislocate()
{
  endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsSixDOFJointDef::SoftLimit::endianSwap()
{
  NMP::endianSwap(m_parentPartFrame);
  NMP::endianSwap(m_childPartFrame);

  NMP::endianSwap(m_swing1Motion);
  NMP::endianSwap(m_swing2Motion);
  NMP::endianSwap(m_twistMotion);

  NMP::endianSwap(m_swing1Limit);
  NMP::endianSwap(m_swing2Limit);
  NMP::endianSwap(m_twistLimitLow);
  NMP::endianSwap(m_twistLimitHigh);

  NMP::endianSwap(m_strengthScale);
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsSixDOFJointDef
//----------------------------------------------------------------------------------------------------------------------
void PhysicsSixDOFJointDef::locate()
{
  PhysicsJointDef::locate();

  endianSwap();

  REFIX_SWAP_PTR(PhysicsDriverData, m_driverData);

  if (m_softLimit)
  {
    REFIX_SWAP_PTR(SoftLimit, m_softLimit);
    m_softLimit->locate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsSixDOFJointDef::dislocate()
{
  PhysicsJointDef::dislocate();

  endianSwap();

  UNFIX_SWAP_PTR(PhysicsDriverData, m_driverData);

  if (m_softLimit)
  {
    m_softLimit->dislocate();
    UNFIX_SWAP_PTR(SoftLimit, m_softLimit);
  }
}

#if defined(MR_OUTPUT_DEBUGGING)
//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsSixDOFJointDef::serializeTx(
  uint16_t nameToken,
  void* outputBuffer,
  uint32_t NMP_USED_FOR_ASSERTS(outputBufferSize)) const
{
  uint32_t dataSize = sizeof(PhysicsSixDOFJointPersistentData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    PhysicsSixDOFJointPersistentData* persistentData = (PhysicsSixDOFJointPersistentData*)outputBuffer;

    persistentData->m_parentLocalFrame = m_parentPartFrame;
    persistentData->m_childLocalFrame = m_childPartFrame;

    persistentData->m_parentPartIndex = m_parentPartIndex;
    persistentData->m_childPartIndex = m_childPartIndex;

    persistentData->m_jointType = PhysicsJointPersistentData::JOINT_TYPE_SIX_DOF;

    persistentData->m_nameToken = nameToken;

    persistentData->m_swing1Limit = m_hardLimits.getSwing1Limit();
    persistentData->m_swing2Limit = m_hardLimits.getSwing2Limit();
    persistentData-> m_twistLimitLow = m_hardLimits.getTwistLimitLow();
    persistentData->m_twistLimitHigh = m_hardLimits.getTwistLimitHigh();

    PhysicsSixDOFJointPersistentData::endianSwap(persistentData);
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsSixDOFJointDef::serializeSoftLimitTx(
  uint16_t,
  void* outputBuffer,
  uint32_t NMP_USED_FOR_ASSERTS(outputBufferSize)) const
{
  if (m_softLimit)
  {
    uint32_t dataSize = sizeof(PhysicsSoftLimitPersistentData);
    if (outputBuffer)
    {
      NMP_ASSERT(outputBufferSize >= dataSize);
      PhysicsSoftLimitPersistentData* persistentData = (PhysicsSoftLimitPersistentData*)outputBuffer;

      persistentData->m_softLimitFrame1Local = m_softLimit->m_parentPartFrame;
      persistentData->m_softLimitFrame2Local = m_softLimit->m_childPartFrame;
      persistentData->m_parentPartIndex = m_parentPartIndex;
      persistentData->m_childPartIndex = m_childPartIndex;
      persistentData->m_swing1SoftLimit = m_softLimit->m_swing1Limit;
      persistentData->m_swing2SoftLimit = m_softLimit->m_swing2Limit;
      persistentData->m_twistSoftLimitLow = m_softLimit->m_twistLimitLow;
      persistentData->m_twistSoftLimitHigh = m_softLimit->m_twistLimitHigh;

      PhysicsSoftLimitPersistentData::endianSwap(persistentData);
    }
    return dataSize;
  }
  return 0;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsSixDOFJointDef::hasSoftLimit() const
{
  return (m_softLimit != NULL);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat PhysicsSixDOFJointDef::getSoftLimitParentLocalQuat() const
{
  if (m_softLimit)
  {
    return m_softLimit->m_parentPartFrame.toQuat();
  }
  else
  {
    return NMP::Quat::kIdentity;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsSixDOFJointDef::endianSwap()
{
  NMP::endianSwap(m_swing1Motion);
  NMP::endianSwap(m_swing2Motion);
  NMP::endianSwap(m_twistMotion);

  m_hardLimits.endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
// PhysicsRagdollJointDef
//----------------------------------------------------------------------------------------------------------------------
void PhysicsRagdollJointDef::locate()
{
  PhysicsJointDef::locate();
  endianSwap();
  REFIX_SWAP_PTR(PhysicsDriverData, m_driverData);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRagdollJointDef::dislocate()
{
  PhysicsJointDef::dislocate();
  endianSwap();
  UNFIX_SWAP_PTR(PhysicsDriverData, m_driverData);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRagdollJointDef::endianSwap()
{
  m_limits.endianSwap();
}

#if defined(MR_OUTPUT_DEBUGGING)
//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRagdollJointDef::serializeTx(
  uint16_t  nameToken,
  void*     outputBuffer,
  uint32_t  NMP_USED_FOR_ASSERTS(outputBufferSize)) const
{
  uint32_t dataSize = sizeof(PhysicsRagdollJointPersistentData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    PhysicsRagdollJointPersistentData* persistentData = (PhysicsRagdollJointPersistentData*)outputBuffer;

    persistentData->m_parentLocalFrame = m_parentPartFrame;
    persistentData->m_childLocalFrame = m_childPartFrame;

    persistentData->m_parentPartIndex = m_parentPartIndex;
    persistentData->m_childPartIndex = m_childPartIndex;

    persistentData->m_jointType = PhysicsJointPersistentData::JOINT_TYPE_RAGDOLL;

    persistentData->m_nameToken = nameToken;

    persistentData->m_coneAngle     = m_limits.getConeAngle();
    persistentData->m_planeMinAngle = m_limits.getPlaneMinAngle();
    persistentData->m_planeMaxAngle = m_limits.getPlaneMaxAngle();
    persistentData->m_twistMinAngle = m_limits.getTwistMinAngle();
    persistentData->m_twistMaxAngle = m_limits.getTwistMaxAngle();

    PhysicsRagdollJointPersistentData::endianSwap(persistentData);
  }
  return dataSize;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
// PhysicsRigDef
//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigDef::locate()
{
  NMP::endianSwap(m_numParts);
  NMP::endianSwap(m_numJoints);
  NMP::endianSwap(m_numCollisionGroups);
  NMP::endianSwap(m_numTrajectoryParts);
  NMP::endianSwap(m_numMaterials);

  NMP::endianSwap(m_rootPart.index);
  NMP::endianSwap(m_rootPart.transform);

  REFIX_SWAP_PTR(Part, m_parts);
  REFIX_SWAP_PTR(PhysicsJointDef*, m_joints);
  REFIX_SWAP_PTR(CollisionGroup, m_collisionGroups);
  REFIX_SWAP_PTR(uint32_t, m_trajectoryParts);
  REFIX_SWAP_PTR(Part::Material, m_materials);

  // parts
  for (int32_t i = 0; i < m_numParts; ++i)
  {
    Part& part = m_parts[i];
    REFIX_SWAP_PTR(char, part.name);
    NMP::endianSwap(part.actor);
    if (part.actor.driverData)
      REFIX_PTR(PhysicsDriverData, part.actor.driverData);
    NMP::endianSwap(part.body);
    if (part.body.driverData)
      REFIX_PTR(PhysicsDriverData, part.body.driverData);

    // volume
    REFIX_SWAP_PTR(char, part.volume.name);
    NMP::endianSwap(part.volume.numSpheres);
    NMP::endianSwap(part.volume.numBoxes);
    NMP::endianSwap(part.volume.numCapsules);

    REFIX_SWAP_PTR(Part::Volume::Sphere, part.volume.spheres);
    REFIX_SWAP_PTR(Part::Volume::Box, part.volume.boxes);
    REFIX_SWAP_PTR(Part::Volume::Capsule, part.volume.capsules);

    for (int32_t j = 0; j != part.volume.numSpheres; ++j)
    {
      NMP::endianSwap(part.volume.spheres[j].localPose);
      NMP::endianSwap(part.volume.spheres[j].parentIndex);
      NMP::endianSwap(part.volume.spheres[j].density);
      NMP::endianSwap(part.volume.spheres[j].materialID);
      if (part.volume.spheres[j].driverData)
        REFIX_SWAP_PTR(PhysicsDriverData, part.volume.spheres[j].driverData);
      NMP::endianSwap(part.volume.spheres[j].radius);
    }

    for (int32_t j = 0; j != part.volume.numBoxes; ++j)
    {
      NMP::endianSwap(part.volume.boxes[j].localPose);
      NMP::endianSwap(part.volume.boxes[j].parentIndex);
      NMP::endianSwap(part.volume.boxes[j].density);
      NMP::endianSwap(part.volume.boxes[j].materialID);
      if (part.volume.boxes[j].driverData)
        REFIX_SWAP_PTR(PhysicsDriverData, part.volume.boxes[j].driverData);
      NMP::endianSwap(part.volume.boxes[j].dimensions);
    }

    for (int32_t j = 0; j != part.volume.numCapsules; ++j)
    {
      NMP::endianSwap(part.volume.capsules[j].localPose);
      NMP::endianSwap(part.volume.capsules[j].parentIndex);
      NMP::endianSwap(part.volume.capsules[j].density);
      NMP::endianSwap(part.volume.capsules[j].materialID);
      if (part.volume.capsules[j].driverData)
        REFIX_SWAP_PTR(PhysicsDriverData, part.volume.capsules[j].driverData);
      NMP::endianSwap(part.volume.capsules[j].radius);
      NMP::endianSwap(part.volume.capsules[j].height);
    }
  }

  // joints
  for (int32_t i = 0; i < m_numJoints; ++i)
  {
    REFIX_SWAP_PTR(PhysicsJointDef, m_joints[i]);

    switch (m_joints[i]->m_jointType)
    {
      case PhysicsJointDef::JOINT_TYPE_SIX_DOF:
      {
        PhysicsSixDOFJointDef* joint = static_cast<PhysicsSixDOFJointDef*>(m_joints[i]);
        joint->locate();
        break;
      }
      case PhysicsJointDef::JOINT_TYPE_RAGDOLL:
      {
        PhysicsRagdollJointDef* joint = static_cast<PhysicsRagdollJointDef*>(m_joints[i]);
        joint->locate();
        break;
      }
      default:
        break;
    }
  }

  for (int32_t i = 0; i < m_numCollisionGroups; ++i)
  {
    CollisionGroup& cs = m_collisionGroups[i];
    REFIX_SWAP_PTR(int32_t, cs.indices);
    NMP::endianSwap(cs.numIndices);
    NMP::endianSwap(cs.enabled);
    for (int32_t j = 0; j < cs.numIndices; ++j)
    {
      NMP::endianSwap(cs.indices[j]);
    }
  }

  // trajectory parts
  for (int32_t i = 0; i < m_numTrajectoryParts; ++i)
  {
    NMP::endianSwap(m_trajectoryParts[i]);
  }

  // materials
  for (uint32_t i = 0; i < m_numMaterials; ++i)
  {
    NMP::endianSwap(m_materials[i].friction);
    NMP::endianSwap(m_materials[i].restitution);
    if (m_materials[i].driverData)
      REFIX_SWAP_PTR(PhysicsDriverData, m_materials[i].driverData);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigDef::dislocate()
{
  for (int32_t i = 0; i < m_numParts; ++i)
  {
    Part& part = m_parts[i];
    UNFIX_SWAP_PTR(char, part.name);

    if (part.actor.driverData)
      UNFIX_PTR(PhysicsDriverData, part.actor.driverData);
    NMP::endianSwap(part.actor);
    if (part.body.driverData)
      UNFIX_PTR(PhysicsDriverData, part.body.driverData);
    NMP::endianSwap(part.body);

    // volume
    UNFIX_SWAP_PTR(char, part.volume.name);

    for (int32_t j = 0; j != part.volume.numSpheres; ++j)
    {
      NMP::endianSwap(part.volume.spheres[j].localPose);
      NMP::endianSwap(part.volume.spheres[j].parentIndex);
      NMP::endianSwap(part.volume.spheres[j].density);
      NMP::endianSwap(part.volume.spheres[j].materialID);
      if (part.volume.spheres[j].driverData)
        UNFIX_SWAP_PTR(PhysicsDriverData, part.volume.spheres[j].driverData);
      NMP::endianSwap(part.volume.spheres[j].radius);
    }

    for (int32_t j = 0; j != part.volume.numBoxes; ++j)
    {
      NMP::endianSwap(part.volume.boxes[j].localPose);
      NMP::endianSwap(part.volume.boxes[j].parentIndex);
      NMP::endianSwap(part.volume.boxes[j].density);
      NMP::endianSwap(part.volume.boxes[j].materialID);
      if (part.volume.boxes[j].driverData)
        UNFIX_SWAP_PTR(PhysicsDriverData, part.volume.boxes[j].driverData);
      NMP::endianSwap(part.volume.boxes[j].dimensions);
    }

    for (int32_t j = 0; j != part.volume.numCapsules; ++j)
    {
      NMP::endianSwap(part.volume.capsules[j].localPose);
      NMP::endianSwap(part.volume.capsules[j].parentIndex);
      NMP::endianSwap(part.volume.capsules[j].density);
      NMP::endianSwap(part.volume.capsules[j].materialID);
      if (part.volume.capsules[j].driverData)
        UNFIX_SWAP_PTR(PhysicsDriverData, part.volume.capsules[j].driverData);
      NMP::endianSwap(part.volume.capsules[j].radius);
      NMP::endianSwap(part.volume.capsules[j].height);
    }

    NMP::endianSwap(part.volume.numSpheres);
    NMP::endianSwap(part.volume.numBoxes);
    NMP::endianSwap(part.volume.numCapsules);

    UNFIX_SWAP_PTR(Part::Volume::Sphere, part.volume.spheres);
    UNFIX_SWAP_PTR(Part::Volume::Box, part.volume.boxes);
    UNFIX_SWAP_PTR(Part::Volume::Capsule, part.volume.capsules);
  }

  for (int32_t i = 0; i < m_numJoints; ++i)
  {
    switch (m_joints[i]->m_jointType)
    {
      case PhysicsJointDef::JOINT_TYPE_SIX_DOF:
      {
        PhysicsSixDOFJointDef* joint = static_cast<PhysicsSixDOFJointDef*>(m_joints[i]);
        joint->dislocate();
        break;
      }
      case PhysicsJointDef::JOINT_TYPE_RAGDOLL:
      {
        PhysicsRagdollJointDef* joint = static_cast<PhysicsRagdollJointDef*>(m_joints[i]);
        joint->dislocate();
        break;
      }
      default:
        break;
    }

    UNFIX_SWAP_PTR(PhysicsJointDef, m_joints[i]);
  }

  for (int32_t i = 0; i < m_numCollisionGroups; ++i)
  {
    CollisionGroup& cs = m_collisionGroups[i];
    for (int32_t j = 0; j < cs.numIndices; ++j)
    {
      NMP::endianSwap(cs.indices[j]);
    }
    NMP::endianSwap(cs.numIndices);
    NMP::endianSwap(cs.enabled);
    UNFIX_SWAP_PTR(int32_t, cs.indices);
  }

  for (int32_t i = 0; i < m_numTrajectoryParts; ++i)
  {
    NMP::endianSwap(m_trajectoryParts[i]);
  }

  for (uint32_t i = 0; i < m_numMaterials; ++i)
  {
    NMP::endianSwap(m_materials[i].friction);
    NMP::endianSwap(m_materials[i].restitution);
    if (m_materials[i].driverData)
      UNFIX_SWAP_PTR(PhysicsDriverData, m_materials[i].driverData);
  }

  NMP::endianSwap(m_rootPart.index);
  NMP::endianSwap(m_rootPart.transform);

  NMP::endianSwap(m_numParts);
  NMP::endianSwap(m_numJoints);
  NMP::endianSwap(m_numCollisionGroups);
  NMP::endianSwap(m_numTrajectoryParts);
  NMP::endianSwap(m_numMaterials);

  UNFIX_SWAP_PTR(Part, m_parts);
  UNFIX_SWAP_PTR(PhysicsJointDef*, m_joints);
  UNFIX_SWAP_PTR(CollisionGroup, m_collisionGroups);
  UNFIX_SWAP_PTR(uint32_t, m_trajectoryParts);
  UNFIX_SWAP_PTR(Part::Material, m_materials);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
int32_t PhysicsRigDef::getPartIndexFromName(const char* name, bool matchPartial) const
{
  if (!name)
  {
    return -1;
  }
  for (int32_t i = 0 ; i < m_numParts ; ++i)
  {
    const Part& part = m_parts[i];
    if (matchPartial)
    {
      if (strstr(name, part.name) != NULL)
        return i;
    }
    else
    {
      if (strcmp(name, part.name) == 0)
        return i;
    }
  }
  return -1;
}

//----------------------------------------------------------------------------------------------------------------------
int32_t PhysicsRigDef::getParentPart(int32_t childIndex) const
{
  for (int32_t i = 0; i < m_numJoints; ++i)
  {
    const PhysicsJointDef& jointDef = *m_joints[i];

    if (jointDef.m_childPartIndex == childIndex)
    {
      return jointDef.m_parentPartIndex;
    }
  }

  return -1;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 PhysicsRigDef::getZeroPoseTMRelRoot(uint32_t iPart) const
{
  NMP_ASSERT(iPart < getNumParts());
  NMP::Matrix34 targetTM(NMP::Matrix34::kIdentity);

  if (iPart > 0)
  {
    // walk the hierarchy and accumulate the transforms
    do
    {
      // Note that the parent joint index of a bone is always the bone index - 1, since its a tree.
      PhysicsJointDef* parentJointDef = m_joints[iPart-1];
      const NMP::Matrix34& frame1 = parentJointDef->m_parentPartFrame;
      NMP::Matrix34 frame2 = parentJointDef->m_childPartFrame;
      frame2.invert();
      targetTM = targetTM * frame2 * frame1;
    } while ((iPart = getParentPart(iPart)) > 0);
    targetTM.orthonormalise();
  }
  return targetTM;
}

//----------------------------------------------------------------------------------------------------------------------
int32_t PhysicsRigDef::calculateNumJointsInChain(int32_t endPartId, int32_t rootPartId) const
{
  int32_t numJoints = 0, partIndex = endPartId;

  while (partIndex != rootPartId)
  {
    partIndex = getParentPart(partIndex);
    if (partIndex == -1)
    {
      NMP_DEBUG_MSG("Invalid joints chain");
      return 0;
    }
    ++numJoints;
  }
  return numJoints;
}

#if defined(MR_OUTPUT_DEBUGGING)
//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRigDef::Part::serializeTx(
  int32_t parentIndex,
  uint16_t nameToken,
  void* outputBuffer,
  uint32_t NMP_USED_FOR_ASSERTS(outputBufferSize)) const
{
  uint32_t dataSize = sizeof(PhysicsPartPersistentData);
  dataSize += volume.numBoxes * sizeof(PhysicsBoxPersistentData);
  dataSize += volume.numCapsules * sizeof(PhysicsCapsulePersistentData);
  dataSize += volume.numSpheres * sizeof(PhysicsSpherePersistentData);

  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    PhysicsPartPersistentData *partPersistentData = (PhysicsPartPersistentData *)outputBuffer;

    partPersistentData->m_parentIndex = parentIndex;
    partPersistentData->m_physicsObjectID = MR_INVALID_PHYSICS_OBJECT_ID;
    partPersistentData->m_nameToken = nameToken;
    partPersistentData->m_numBoxes = static_cast<uint32_t>(volume.numBoxes);
    partPersistentData->m_numCapsules = static_cast<uint32_t>(volume.numCapsules);
    partPersistentData->m_numSpheres = static_cast<uint32_t>(volume.numSpheres);

    for (uint32_t i = 0; i != partPersistentData->m_numBoxes; ++i)
    {
      PhysicsBoxPersistentData* shapeData = partPersistentData->getBox(i);

      NMP_IS_ALIGNED(shapeData, NMP_VECTOR_ALIGNMENT);

      shapeData->m_parentIndex = volume.boxes[i].parentIndex;
      shapeData->m_localPose = volume.boxes[i].localPose;

      // mult by 2 to counter the halving in PhysicsRigDefBuilder.cpp
      // physicsRigDef->m_parts[i].volume.boxes[j].dimensions = physicsBoxExport->getDimensions() * 0.5f;
      shapeData->m_width = 2.0f * volume.boxes[i].dimensions.x;
      shapeData->m_height = 2.0f * volume.boxes[i].dimensions.y;
      shapeData->m_depth = 2.0f * volume.boxes[i].dimensions.z;

      PhysicsBoxPersistentData::endianSwap(shapeData);
    }

    for (uint32_t i = 0; i != partPersistentData->m_numCapsules; ++i)
    {
      PhysicsCapsulePersistentData* shapeData = partPersistentData->getCapsule(i);

      NMP_IS_ALIGNED(shapeData, NMP_VECTOR_ALIGNMENT);

      shapeData->m_parentIndex = volume.capsules[i].parentIndex;
      shapeData->m_localPose = volume.capsules[i].localPose;

      shapeData->m_radius = volume.capsules[i].radius;
      shapeData->m_height = volume.capsules[i].height;

      PhysicsCapsulePersistentData::endianSwap(shapeData);
    }

    for (uint32_t i = 0; i != partPersistentData->m_numSpheres; ++i)
    {
      PhysicsSpherePersistentData* shapeData = partPersistentData->getSphere(i);

      NMP_IS_ALIGNED(shapeData, NMP_VECTOR_ALIGNMENT);

      shapeData->m_parentIndex = volume.spheres[i].parentIndex;
      shapeData->m_localPose = volume.spheres[i].localPose;

      shapeData->m_radius = volume.spheres[i].radius;

      PhysicsSpherePersistentData::endianSwap(shapeData);
    }

    // can't call PhysicsPartPersistentData::endianSwap as that swaps the shapes as well.
    // the shapes must be swapped separately as the part endian swap swaps the shapes after the
    // shape type counts which would break everything.
    NMP::netEndianSwap(partPersistentData->m_numSpheres);
    NMP::netEndianSwap(partPersistentData->m_numCapsules);
    NMP::netEndianSwap(partPersistentData->m_numBoxes);
    NMP::netEndianSwap(partPersistentData->m_nameToken);
    NMP::netEndianSwap(partPersistentData->m_parentIndex);
  }

  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRigDef::serializeTx(
  AnimSetIndex animSetIndex,
  void* outputBuffer,
  uint32_t NMP_USED_FOR_ASSERTS(outputBufferSize)) const
{
  uint32_t dataSize = sizeof(PhysicsRigDefPersistentData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    PhysicsRigDefPersistentData* outputData = (PhysicsRigDefPersistentData*)outputBuffer;
    outputData->m_animSetIndex = animSetIndex;
    outputData->m_numParts = m_numParts;
    outputData->m_numJoints = m_numJoints;

    PhysicsRigDefPersistentData::endianSwap(outputData);
  }
  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
