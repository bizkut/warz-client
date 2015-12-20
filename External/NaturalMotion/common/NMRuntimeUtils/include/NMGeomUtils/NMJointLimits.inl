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
// inline included by NMJointLimits.h
//----------------------------------------------------------------------------------------------------------------------

#include <float.h>

#define JOINTLIMIT_OPTIMISATIONSx

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE JointLimits::Params::Params() :
  frame(NMP::Quat::kIdentity),
  offset(NMP::Quat::kIdentity),
  lower(-1.0f, 0.0f, 0.0f),
  upper(1.0f, 1.0f, 1.0f),
  type(NMSTANDARD_JOINTLIMITS)
{
  // ...
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE JointLimits::Params::Params(
  LimitType typeIn,
  const NMP::Quat& frameIn,
  const NMP::Vector3& lowerIn,
  const NMP::Vector3& upperIn) :
  frame(frameIn),
  offset(NMP::Quat::kIdentity),
  lower(lowerIn),
  upper(upperIn),
  type(typeIn)
{
  // ...
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE JointLimits::Params::Params(const Params& params) :
  frame(params.frame),
  offset(params.offset),
  lower(params.lower),
  upper(params.upper),
  type(NMSTANDARD_JOINTLIMITS)
{
  // ...
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE JointLimits::Params::Params(const Params& params, float factor) :
  frame(params.frame),
  offset(params.offset),
  type(params.type)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  // Force zero limit ranges up to at least this value, and/or prevent negative swing limits and twist
  // limits for which the lower limit is higher than the upper limit.
  const float xZeroMinimum = 0.0f;

  // Scaled twist limits
  float midTwist = 0.5f * (params.lower[0] + params.upper[0]);
  float scaledHalfRangeTwist = factor * 0.5f * (params.upper[0] - params.lower[0]);
  scaledHalfRangeTwist = NMP::maximum(scaledHalfRangeTwist, xZeroMinimum);
  lower[0] = midTwist - scaledHalfRangeTwist;
  upper[0] = midTwist + scaledHalfRangeTwist;

  // Scaled swing limits
  // OPTIMISE No need to set lower swing limits
  upper[1] = factor * params.upper[1];
  upper[2] = factor * params.upper[2];
  upper[1] = NMP::maximum(upper[1], xZeroMinimum);
  upper[2] = NMP::maximum(upper[2], xZeroMinimum);
  lower[1] = -upper[1];
  lower[2] = -upper[2];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void JointLimits::Params::setTwistLeanLimits(float twistMin, float twistMax, float lean1, float lean2)
{
  type = NMSTANDARD_JOINTLIMITS;
  lower.set(tanf(twistMin / 4.0f), 0, 0);
  upper.set(tanf(twistMax / 4.0f), tanf(lean1 / 4.0f), tanf(lean2 / 4.0f));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float JointLimits::Params::getTwistMin() const
{
  return 4.0f * atanf(lower.x);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float JointLimits::Params::getTwistMax() const
{
  return 4.0f * atanf(upper.x);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float JointLimits::Params::getSwing1Limit() const
{
  return 4.0f * atanf(upper.y);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float JointLimits::Params::getSwing2Limit() const
{
  return 4.0f * atanf(upper.z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float JointLimits::Params::getLean1Limit() const
{
  return getSwing1Limit();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float JointLimits::Params::getLean2Limit() const
{
  return getSwing2Limit();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void JointLimits::Params::setAsDirectedHinge(
  const NMP::Vector3& parentBone,
  const NMP::Vector3& childBone,
  const NMP::Vector3& hingeAxis,
  float zeroHingeAngle)
{
  type = NMSTANDARD_JOINTLIMITS;

  // Adjust the limit frame so that the hinge axis is always x-axis rotation
  NMP::Vector3 axis(1.0f, 0.0f, 0.0f);
  NMP::Vector3 hingeAxisInFrame = frame.inverseRotateVector(hingeAxis);
  float dot = axis.dot(hingeAxisInFrame);
  float sign = NMP::floatSelect(dot, 1.0f, -1.0f);
  if (NMP::nmfabs(NMP::nmfabs(hingeAxisInFrame.x) - 1.0f) > GeomUtils::gTol)
  {
    NMP::Quat offsetQ;
    offsetQ.forRotation(axis, hingeAxisInFrame);
    frame *= offsetQ;
    frame.normalise();
  }

  // Get the bone vectors as projected into the plane perpendicular to the hinge, in
  // the frame of the joint's parent when the joint is at its origin.
  NMP::Vector3 v1 = parentBone;
  NMP::Quat q = frame * ~offset;
  NMP::Vector3 v2 = q.rotateVector(childBone);
  v1 = v1 - hingeAxis * hingeAxis.dot(v1);
  v2 = v2 - hingeAxis * hingeAxis.dot(v2);

  // Check that the bones, as projected onto the hinge axis, have significant length,
  // or else this kind of limit is not meaningful.
  if (v1.magnitudeSquared() > GeomUtils::gTolSq &&
      v2.magnitudeSquared() > GeomUtils::gTolSq)
  {

    // Normalise bone vectors to use for measuring angles
    v1.normalise();
    v2.normalise();

    // Get the hinge angle when the two bones point in the same direction
    NMP::Vector3 dirnV;
    dirnV.cross(v2, v1);
    // sign converts to positive x-axis rotation as limits require
    float dirn = sign * dirnV.dot(hingeAxis);
    float cosAngle = v1.dot(v2);
    float straightAngle = NMRU_ACOSF_SAFE(cosAngle);
    straightAngle = NMP::floatSelect(dirn, straightAngle, -straightAngle);

    // Set the twist limits to go between the given zero angle (relative to straight
    // bones) and the bones being folded back on each other.  Correct for wrap-around.
    float limit1 = straightAngle + zeroHingeAngle;
    float limit2 = straightAngle + NM_PI;
    if (limit1 < -NM_PI)
    {
      limit1 = limit1 + 2.0f * NM_PI;
    }
    if (limit1 > NM_PI)
    {
      limit1 = limit1 - 2.0f * NM_PI;
    }
    if (limit2 < -NM_PI)
    {
      limit2 = limit2 + 2.0f * NM_PI;
    }
    if (limit2 > NM_PI)
    {
      limit2 = limit2 - 2.0f * NM_PI;
    }
    // Remembering that in twist-swing space, twist is reversed rotation (i.e. clockwise),
    // so the limits are opposite.
    if (limit1 < limit2)
    {
      lower.x = tanf(-limit2 / 4.0f);
      upper.x = tanf(-limit1 / 4.0f);
    }
    else
    {
      lower.x = tanf(-limit1 / 4.0f);
      upper.x = tanf(-limit2 / 4.0f);
    }

    // Set the swing limits to a small value, since zero limits can cause issues and we know this
    // joint is a hinge
    upper.y = 1e-2f;
    upper.z = 1e-2f;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void JointLimits::Params::endianSwap()
{
  NMP::endianSwap(frame);
  NMP::endianSwap(offset);
  NMP::endianSwap(lower);
  NMP::endianSwap(upper);
  NMP::endianSwap(type);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Quat JointLimits::clamped(const Params& params, const NMP::Quat& q)
{
  NMP::Quat qResult = q;
  clamp(params, qResult);
  return qResult;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Quat JointLimits::clamped(const Params& params, const NMP::Quat& q, float smoothness)
{
  NMP::Quat qResult = q;
  clamp(params, qResult, smoothness);
  return qResult;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE char JointLimits::clamp(const Params& params, NMP::Quat& qj)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  // Orientation in the limit frame
  NMP::Quat q;
  toInternal(params, qj, q);
  char retval = clampSimple(params, q);
  // If we haven't clamped then there's no need to convert back, nothing has changed.
  // OPTIMISE  If this condition is too slow then remove it, there is no harm in doing the
  // contents (except that the contents are themselves slow).
  if (retval != 0)
  {
    toJoint(params, q, qj);
  }
  return retval;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE char JointLimits::clamp(const Params& params, NMP::Quat& qj, float smoothness)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  // Orientation in the limit frame
  NMP::Quat q;
  toInternal(params, qj, q);
  char retval = clampSimple(params, q, smoothness);
  // If we haven't clamped then there's no need to convert back, nothing has changed.
  // OPTIMISE  If this condition is too slow then remove it, there is no harm in doing the
  // contents (except that the contents are themselves slow).
  if (retval != 0)
  {
    toJoint(params, q, qj);
  }
  return retval;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE char JointLimits::isViolated(const Params& params, const NMP::Quat& qj)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  // Orientation in the limit frame
  NMP::Quat q;
  toInternal(params, qj, q);

  return isViolatedSimple(params, q);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE char JointLimits::isViolatedSimple(const Params& params, const NMP::Quat& q)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  NMP::Vector3 tl;
  float& t = tl.x;
  float& l1 = tl.y;
  float& l2 = tl.z;

  //----------------------------------------------------------------------------------------------------------------------
  // Twist

#ifdef JOINTLIMIT_OPTIMISATIONS
  // Instead of converting directly to twist swing, we do it in stages so we don't have to
  // convert any more than we have to.
  float chs2 = q.w * q.w + q.x * q.x;
  t = 0;
  if (chs2 > 1.2e-7f)
  {
    float tanHalfMinTwistLimit = 2.0f * params.lower.x / (1.0f - params.lower.x * params.lower.x);
    float tanHalfMaxTwistLimit = 2.0f * params.upper.x / (1.0f - params.upper.x * params.upper.x);
    t = -q.x / q.w;
    if (t < tanHalfMinTwistLimit || t > tanHalfMaxTwistLimit)
    {
      return 1;
    }
  }
#else
  GeomUtils::quatToTwistSwing(q, tl);

  if (t < params.lower.x || t > params.upper.x)
  {
    return 1;
  }
#endif

  //----------------------------------------------------------------------------------------------------------------------
  // Lean

#ifdef JOINTLIMIT_OPTIMISATIONS
  // Quick check for minimal lean cone
  float tanQuarterMinLeanLimit = NMP::minimum(params.upper.y, params.upper.z);
  float tanHalfMinLeanLimit = 2.0f * tanQuarterMinLeanLimit / (1.0f - tanQuarterMinLeanLimit * tanQuarterMinLeanLimit);
  float t2 = tanHalfMinLeanLimit * tanHalfMinLeanLimit;
  float cosMinLeanLimit = (1 - t2) / (1 + t2);
  float rotatedXAxisX = 2.0f * chs2 - 1.0f;
  if (rotatedXAxisX >= cosMinLeanLimit)
  {
    return 0;
  }

  // Calculate lean
  if (chs2 > 1.2e-7f)
  {
    float chs = NMP::fastSqrt(chs2);
    float mul = 1.0f / (chs * (chs + 1.0f));
    l1 = mul * (q.x * q.y + q.w * q.z);
    l2 = mul * (-q.w * q.y + q.x * q.z);
  }
  else
  {
    float rshs = NMP::fastReciprocalSqrt(1.0f - chs2);
    l1 = rshs * q.z;
    l2 = -rshs * q.y;
  }
#endif

  //   Test in case limits are zero - avoid divide by zero
  if (params.upper.y < 1e-3f || params.upper.z < 1e-3f)
  {
    // If both are zero, assume hinge, and ignore violation
    if (!(params.upper.y == 0 && params.upper.z == 0))
    {
      // Just test independent limits
      if (NMP::nmfabs(l1) > GeomUtils::nTol && (l1 < -params.upper.y || l1 > params.upper.y))
      {
        return 2;
      }
      if (NMP::nmfabs(l2) > GeomUtils::nTol && (l2 < -params.upper.z || l2 > params.upper.z))
      {
        return 2;
      }
    }
  }
  else
  {
    float l1unit = l1 / params.upper.y;
    float l2unit = l2 / params.upper.z;
    if (l1unit * l1unit + l2unit * l2unit - 1.0f > 0)
    {
      return 2;
    }
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE char JointLimits::isViolated(const Params& params, const NMP::Quat& qj, float smoothness)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  // Orientation in the limit frame
  NMP::Quat q;
  toInternal(params, qj, q);

  return isViolatedSimple(params, q, smoothness);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE char JointLimits::isViolatedSimple(const Params& params, const NMP::Quat& q, float smoothness)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  if (smoothness == 0)
  {
    return isViolatedSimple(params, q);
  }
  else if (degreeOfViolationSimple(params, q, smoothness) > 1.0f)
  {
    // Using degreeOfViolation it is not possible to distinguish between twist violation and
    // swing violation, so return that they were both violated.
    return 3;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float JointLimits::degreeOfViolation(const Params& params, const NMP::Quat& qj, float smoothness /* = 0 */)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  // Orientation in the limit frame
  NMP::Quat q;
  toInternal(params, qj, q);

  return degreeOfViolationSimple(params, q, smoothness);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float JointLimits::degreeOfViolationSimple(const Params& params, const NMP::Quat& q, float smoothness /* = 0 */)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  NMP::Vector3 tl;
  GeomUtils::quatToTwistSwing(q, tl);

  const float& t = tl.x;
  const float& l1 = tl.y;
  const float& l2 = tl.z;

  // This assumes that if the limit is zero that dof will always be zero, so doesn't
  // use it in the degree of violation value.  This prevents some sticking but can lead to limit
  // violation, depending on what the value is being used for.

  float retval = 0;

  // Twist
  float range = 0.5f * (params.upper.x - params.lower.x);
  if (range > GeomUtils::nTol)
  {
    float centre = 0.5f * params.upper.x + 0.5f * params.lower.x;
    float twistUnit = (t - centre) / range;
    twistUnit = smooth(twistUnit, smoothness);
    retval += twistUnit * twistUnit;
  }

  // Lean
  //   Test in case limits are zero - avoid divide by zero.
  if (params.upper.y > GeomUtils::nTol)
  {
    float l1unit = l1 / params.upper.y;
    retval += l1unit * l1unit;
  }
  if (params.upper.z > GeomUtils::nTol)
  {
    float l2unit = l2 / params.upper.z;
    retval += l2unit * l2unit;
  }

  return retval;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float JointLimits::degreeOfViolationTwistSwingSqSimple(
  const Params& params,
  const NMP::Quat& q,
  float& twist,
  float& swingSq)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  NMP::Vector3 tl;
  GeomUtils::quatToTwistSwing(q, tl);

  const float& t = tl.x;
  const float& l1 = tl.y;
  const float& l2 = tl.z;

  // Twist
  float range = 0.5f * (params.upper.x - params.lower.x);
  if (range > GeomUtils::nTol)
  {
    float centre = 0.5f * params.upper.x + 0.5f * params.lower.x;
    twist = (t - centre) / range;
  }
  else
  {
    twist = 0;
  }

  swingSq = 0;

  // Lean
  //   Test in case limits are zero - avoid divide by zero.
  if (params.upper.y > GeomUtils::nTol)
  {
    float l1unit = l1 / params.upper.y;
    swingSq = l1unit * l1unit;
  }
  if (params.upper.z > GeomUtils::nTol)
  {
    float l2unit = l2 / params.upper.z;
    swingSq += l2unit * l2unit;
  }
  return twist * twist + swingSq;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Quat JointLimits::centre(const Params& params)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  // OPTIMISE Could break down the call to twistSwingToQuat() to eliminate the calculations involving
  // the zero swing values.
  NMP::Vector3 tl(0.5f * params.upper.x + 0.5f * params.lower.x, 0, 0);
  NMP::Quat q;
  GeomUtils::twistSwingToQuat(tl, q);
  NMP::Quat qj;
  toJoint(params, q, qj);
  return qj;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void JointLimits::toInternal(const Params& params, const NMP::Quat& qj, NMP::Quat& q)
{
#ifdef JOINTLIMIT_OPTIMISATIONS
  // Avoid multiply if identity
  if (params.frame.w == 1.0f && params.offset.w == 1.0f)
  {
    q = qj;
  }
  else
#endif
  {
    q = ~params.frame * qj * params.offset;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void JointLimits::toJoint(const Params& params, const NMP::Quat& q, NMP::Quat& qj)
{
#ifdef JOINTLIMIT_OPTIMISATIONS
  // Avoid multiply if identity
  if (params.frame.w == 1.0f && params.offset.w == 1.0f)
  {
    qj = q;
  }
  else
#endif
  {
    qj = params.frame * q * ~params.offset;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float JointLimits::smooth(float val, float smoothness)
{
  smoothness = NMP::clampValue(smoothness, 1e-5f, 0.99f);
  float K = (1.0f - smoothness) / (smoothness * smoothness);
  float sign = 1.0f;
  if (val < 0)
  {
    sign = -1.0f;
    val = -val;
  }
  // This function is a modified 1/x curve designed to rise rapidly from 0 to 1 around val = 1.
  // This allows us to mix twist and swing together a bit by adding twist error only near the
  // the twist limit boundary.
  // If val > 1 we leave it unchanged.
  if (val < 1.0f)
  {
    val = ((K + 1.0f) / K) * ((-1.0f / (K * (val  - 1.0f) - 1.0f)) - 1.0f) + 1.0f;
  }

  return val * sign;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float JointLimits::limitViolationFactor(float errIn)
{
  if (errIn >= 1.0f - FLT_EPSILON)
  {
    return FLT_MAX;
  }
  else
  {
    return -1.0f / (errIn - 1.0f);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float JointLimits::softLimitViolationFactor(float errIn)
{
  return NMP::nmfabs(errIn);
}

} // end of namespace NMRU
