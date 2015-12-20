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
#include "NMGeomUtils/NMJointLimits.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
char JointLimits::clampSimple(const Params& params, NMP::Quat& q)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  // Twist-lean for clamping
  NMP::Vector3 tl;
  GeomUtils::quatToTwistSwing(q, tl);

  float& t = tl.x;
  float& l1 = tl.y;
  float& l2 = tl.z;

  // Twist
  char retval = 1;
  t = t < params.lower.x ? params.lower.x : (t > params.upper.x ? params.upper.x : (retval = 0, t));

  // Lean
  //   Test in case limits are zero - avoid divide by zero
  if (params.upper.y < 1e-3f || params.upper.z < 1e-3f)
  {
    // If this is a hinge axis (all swing limits zero), don't register swing clamp
    if (params.upper.y == 0 && params.upper.z == 0)
    {
      l1 = 0;
      l2 = 0;
    }
    else
    {
      // Just clamp to independent limits
      if (NMP::nmfabs(l1) > GeomUtils::nTol)
      {
        l1 = l1 < -params.upper.y ? (retval |= 2, -params.upper.y) :
          (l1 > params.upper.y ? (retval |= 2, params.upper.y) : l1);
      }
      if (NMP::nmfabs(l2) > GeomUtils::nTol)
      {
        l2 = l2 < -params.upper.z ? (retval |= 2, -params.upper.z) :
          (l2 > params.upper.z ? (retval |= 2, params.upper.z) : l2);
      }
    }
  }
  else
  {
    float l1unit = l1 / params.upper.y;
    float l2unit = l2 / params.upper.z;
    float violationFactor = l1unit * l1unit + l2unit * l2unit - 1.0f;
    if (violationFactor > 0)
    {
      // We are clamping lean
      retval |= 2;

#define SWING_CLAMPING_ACCURATE
#ifdef SWING_CLAMPING_ACCURATE  // 'Correct' method solves quartic iteratively for closest point

      GeomUtils::closestPointOnEllipse_newton(l1, l2, params.upper.y, params.upper.z);

// Alternative clamping often pushes too close to ellipse centre and has a whole area of positions
// which will clamp to the same place - but is about twice as fast on PC.
#else

      // Clamp to independent limits first - this improves accuracy
      l1 = l1 < -params.upper.y ? -params.upper.y : (l1 > params.upper.y ? params.upper.y : l1);
      l2 = l2 < -params.upper.z ? -params.upper.z : (l2 > params.upper.z ? params.upper.z : l2);
      float normLength = sqrtf(
        (l1 * l1 / (params.upper.y * params.upper.y)) + (l2 * l2 / (params.upper.z * params.upper.z)));
      l1 /= normLength;
      l2 /= normLength;

#endif

    }
  }

  // If we haven't clamped then there's no need to convert back, nothing has changed
  // OPTIMISE  If this condition is too slow then remove it, there is no harm in doing the
  // contents (except that the contents are themselves slow)
  if (retval != 0)
  {
    GeomUtils::twistSwingToQuat(tl, q);
  }

  return retval;
}

//----------------------------------------------------------------------------------------------------------------------
char JointLimits::clampSimple(const Params& params, NMP::Quat& q, float smoothness)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  // Do a non-smoothed clamp
  char retval = clampSimple(params, q);

  NMP::Vector3 tl;
  GeomUtils::quatToTwistSwing(q, tl);

  float& t = tl.x;
  float& l1 = tl.y;
  float& l2 = tl.z;

  // We avoid divide-by-zero by clamping to a minimum value, but we then ignore zero limits.

  // Twist
  float range = 0.5f * (params.upper.x - params.lower.x);
  float rangeSafe = NMP::maximum(range, 1e-6f);
  float centre = 0.5f * params.upper.x + 0.5f * params.lower.x;
  float twistUnit = (t - centre) / rangeSafe;
  float twistUnitSmoothed = smooth(twistUnit, smoothness);

  // Lean
  float l1lim = NMP::maximum(params.upper.y, 1e-6f);
  float l2lim = NMP::maximum(params.upper.z, 1e-6f);
  float l1unit = l1 / l1lim;
  float l2unit = l2 / l2lim;
  float degreeOfViolation = 0;
  if (range > GeomUtils::nTol)
  {
    degreeOfViolation += twistUnitSmoothed * twistUnitSmoothed;
  }
  if (params.upper.y > GeomUtils::nTol)
  {
    degreeOfViolation += l1unit * l1unit;
  }
  if (params.upper.z > GeomUtils::nTol)
  {
    degreeOfViolation += l2unit * l2unit;
  }

  if (degreeOfViolation > 1.0f)
  {
    // If there is violation, everything will be clamped
    retval |= 3;

    // Bring unit twist-swing vector back onto limit boundary
    float scale = NMP::fastReciprocalSqrt(degreeOfViolation);
    twistUnit *= scale;
    l1unit *= scale;
    l2unit *= scale;

    // Calculate new non-unit twist-swing vector
    if (range > GeomUtils::nTol)
    {
      t = twistUnit * range + centre;
    }
    if (params.upper.y > GeomUtils::nTol)
    {
      l1 = l1unit * params.upper.y;
    }
    if (params.upper.z > GeomUtils::nTol)
    {
      l2 = l2unit * params.upper.z;
    }
  }

  GeomUtils::twistSwingToQuat(tl, q);

  return retval;
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimits::clampHinge(const Params& params, NMP::Quat& qj)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  // Orientation in the limit frame
  NMP::Quat q;
  toInternal(params, qj, q);

  clampHingeSimple(q);

  toJoint(params, q, qj);
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimits::clampHingeSimple(NMP::Quat& q)
{
  NMP::Quat qClamped;
  GeomUtils::separateTwistFromTwistAndSwing(q, qClamped);

  q = qClamped;
}

//----------------------------------------------------------------------------------------------------------------------
char JointLimits::clampScaled(const Params& params, NMP::Quat& qj, float factor)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  Params scaledParams(params, factor);
  return clamp(scaledParams, qj);
}

//----------------------------------------------------------------------------------------------------------------------
char JointLimits::clampScaled(const Params& params, NMP::Quat& qj, float factor, float smoothness)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  Params scaledParams(params, factor);
  return clamp(scaledParams, qj, smoothness);
}

//----------------------------------------------------------------------------------------------------------------------
char JointLimits::clampBetween(const Params& params, const NMP::Quat& qFrom, NMP::Quat& q, float* w /* = 0 */)
{
  // Convert into limit frame
  NMP::Quat qFromInternal;
  toInternal(params, qFrom, qFromInternal);
  NMP::Quat qInternal;
  toInternal(params, q, qInternal);

  // The algorithm itself is moved into this function to separate out the transform into and out
  // of the limit frame.
  char retval = clampBetweenSimple(params, qFromInternal, qInternal, w);

  // Convert back to joint space
  toJoint(params, qInternal, q);

  return retval;
}

//----------------------------------------------------------------------------------------------------------------------
char JointLimits::toBoundarySimple(
  const Params& params,
  const NMP::Quat& qFrom,
  const NMP::Vector3& axis,
  NMP::Quat& qTo,
  float* w /* = 0 */)
{
  static const float tol = 1e-6f;

  NMP::Quat qChange;
  NMP::Quat qStart = qFrom; // Need this in case qFrom and qTo reference the same data
  qChange.fromRotationVector(axis * (NM_PI - tol));
  qTo.multiply(qChange, qStart);
  return clampBetweenSimple(params, qStart, qTo, w);
}

//----------------------------------------------------------------------------------------------------------------------
// It is thought that fast slerps are probably as efficient as lerps because lerps require a
// normalisation involving a square root, which may be just as slow as doing all the
// multiplies and adds that fast slerp uses.
#define USE_SLERPS_NOT_LERPS
char JointLimits::clampBetweenSimple(
  const Params& params,
  const NMP::Quat& qFromInternal,
  NMP::Quat& qInternal,
  float* w /* = 0 */)
{
  // Do a bisection search to find limit boundary along line of rotation between the two orientations.
  // Use fast slerps, which aren't perfectly accurate, but that doesn't matter in such a search.
  // Or use nlerps, which are even less accurate, but again doesn't matter.

  // Detect hinge axes and do a normal clamp for them because clampBetween() is not meaningful
  if (params.upper.y == 0 && params.upper.z == 0)
  {
    char retval = clampSimple(params, qInternal);
    NMP_ASSERT(!w);
    return retval;
  }

  const uint32_t maxIters = 10;
  float a = 0.0f;
  float b = 1.0f;
  // Get rotations into same semi-arc for interpolation
  float fromDotTo = qFromInternal.dot(qInternal);
  if (fromDotTo < 0)
  {
    fromDotTo = -fromDotTo;
    qInternal = -qInternal;
  }

  NMP::Quat target = qInternal;
  char retval = JointLimits::isViolatedSimple(params, qInternal);
  char currentlyViolatingLimit = retval;

  if (retval != 0)
  {
    for (uint32_t iter = 0; iter < maxIters; ++iter)
    {
      float t = 0.5f * (a + b);
#ifdef USE_SLERPS_NOT_LERPS
      qInternal.fastSlerp(qFromInternal, target, t, fromDotTo);
#else
      qInternal.lerp(qFromInternal, target, t, fromDotTo);
#endif
      currentlyViolatingLimit = JointLimits::isViolatedSimple(params, qInternal);
      if (currentlyViolatingLimit)
      {
        b = t;
        retval = currentlyViolatingLimit;
      }
      else
      {
        a = t;
      }
    }

    // Make sure our output is inside the limit
#ifdef USE_SLERPS_NOT_LERPS
    qInternal.fastSlerp(qFromInternal, target, a, fromDotTo);
#else
    qInternal.lerp(qFromInternal, target, a, fromDotTo);
#endif

    // Return the actual amount of rotation
    if (w)
    {
      *w = a;
    }
  }

  // The desired orientation never was violating the limit in the first place
  else if (w)
  {
    *w = 1.0f;
  }

  return retval;
}

//----------------------------------------------------------------------------------------------------------------------
char JointLimits::clampBetween(
  const Params& params,
  const NMP::Quat& qFrom,
  NMP::Quat& q,
  float smoothness,
  float* w /* = 0 */)
{
  // Do a bisection search to find limit boundary along line of rotation between the two orientations
  // Use fast slerps, which aren't perfectly accurate, but that doesn't matter in such a search
  // Or use nlerps, which are even less accurate, but again doesn't matter

  // Convert into limit frame and keep it that way until we've finished the search
  NMP::Quat qFromInternal;
  toInternal(params, qFrom, qFromInternal);
  NMP::Quat qInternal;
  toInternal(params, q, qInternal);

  // The algorithm itself is moved into this function to separate out the transform into and out
  // of the limit frame.
  char retval = clampBetweenSimple(params, qFromInternal, qInternal, smoothness, w);

  // Convert back to joint space
  toJoint(params, qInternal, q);

  return retval;
}

//----------------------------------------------------------------------------------------------------------------------
char JointLimits::clampBetweenSimple(
  const Params& params,
  const NMP::Quat& qFromInternal,
  NMP::Quat& qInternal,
  float smoothness,
  float* w /* = 0 */)
{
  // Do a bisection search to find limit boundary along line of rotation between the two orientations
  // Use fast slerps, which aren't perfectly accurate, but that doesn't matter in such a search
  // Or use nlerps, which are even less accurate, but again doesn't matter

  // Detect hinge axes and do a normal clamp for them because clampBetween() is not meaningful
  if (params.upper.y == 0 && params.upper.z == 0)
  {
    char retval = clampSimple(params, qInternal);
    NMP_ASSERT(!w);
    return retval;
  }

  const uint32_t maxIters = 10;
  float a = 0.0f;
  float b = 1.0f;
  // Get rotations into same semi-arc for interpolation
  float fromDotTo = qFromInternal.dot(qInternal);
  if (fromDotTo < 0)
  {
    fromDotTo = -fromDotTo;
    qInternal = -qInternal;
  }

  NMP::Quat target = qInternal;
  char retval = JointLimits::isViolatedSimple(params, qInternal, smoothness);
  char currentlyViolatingLimit = retval;

  if (retval != 0)
  {
    for (uint32_t iter = 0; iter < maxIters; ++iter)
    {
      float t = 0.5f * (a + b);
#ifdef USE_SLERPS_NOT_LERPS
      qInternal.fastSlerp(qFromInternal, target, t, fromDotTo);
#else
      qInternal.lerp(qFromInternal, target, t, fromDotTo);
#endif
      currentlyViolatingLimit = JointLimits::isViolatedSimple(params, qInternal, smoothness);
      if (currentlyViolatingLimit)
      {
        b = t;
        retval = currentlyViolatingLimit;
      }
      else
      {
        a = t;
      }
    }
    // If we end over the limit, make sure our output is inside the limit
    //  if (currentlyViolatingLimit) - OPTIMISE: this condition not required
#ifdef USE_SLERPS_NOT_LERPS
    qInternal.fastSlerp(qFromInternal, target, a, fromDotTo);
#else
    qInternal.lerp(qFromInternal, target, a, fromDotTo);
#endif

    // Return the actual amount of rotation
    if (w)
    {
      *w = a;
    }
  }

  // The desired orientation never was violating the limit in the first place
  else if (w)
  {
    *w = 1.0f;
  }

  return retval;
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimits::clampBetweenWithBoundaryRepulsion(
  const Params& params,
  const NMP::Quat& qFrom,
  NMP::Quat& q,
  float repulsionFactor,
  float smoothness,
  float* w /* = 0 */)
{
  // Convert into limit frame
  NMP::Quat qFromInternal;
  toInternal(params, qFrom, qFromInternal);
  NMP::Quat qInternal;
  toInternal(params, q, qInternal);

  // The algorithm itself is moved into this function to separate out the transform into and out
  // of the limit frame
  clampBetweenWithBoundaryRepulsionSimple(params, qFromInternal, qInternal, repulsionFactor, smoothness, w);

  // Convert back to joint space
  toJoint(params, qInternal, q);
}

//----------------------------------------------------------------------------------------------------------------------
#define INTEGRATION_METHODx
void JointLimits::clampBetweenWithBoundaryRepulsionSimple(
  const Params& params,
  const NMP::Quat& qFromInternal,
  NMP::Quat& qInternal,
  float repulsionFactor,
  float smoothness,
  float* w /* = 0 */)
{
  // This algorithm finds the point along the arc of rotation where a modified form of the
  // 'degree of violation' - a value that increases as the limit boundary is approached - has
  // increased by an amount determined by the input repulsionFactor.  Obviously if the value is
  // decreasing then the motion will go all the way along.

  // If you set the repulsionFactor to 0 then you want to do a hard clamp.  In other words, the
  // boundary does not repel approaching violation until it is hit.
  // MORPH-21307:  At the moment we do a normal clamp for hinge joints because clampBetween()
  // is not meaningful.  A version is needed for hinges that incorporates boundary repulsion.
  if (repulsionFactor <= 0 || (params.upper.y == 0 && params.upper.z == 0))
  {
    clampBetweenSimple(params, qFromInternal, qInternal, smoothness, w);
    return;
  }
  // Convert repulsion factor to a softness factor
  float softnessFactor = 1.0f / repulsionFactor;

  const uint32_t maxIters = 10;
  float a = 0.0f;
  float b = 1.0f;
  // Get rotations into same semi-arc for interpolation
  float fromDotTo = qFromInternal.dot(qInternal);
  if (fromDotTo < 0)
  {
    fromDotTo = -fromDotTo;
    qInternal = -qInternal;
  }
  NMP::Quat target = qInternal;

#ifdef INTEGRATION_METHOD

  const uint32_t maxDivisions = 20;
  const float increment = 1.0f / (maxDivisions - 1.0f);
  float sum = 0;
  float t = 0;
  float startViolationFactor = 0;
  NMP::Quat prevQ = qFromInternal;
  for (uint32_t i = 0; i < maxDivisions; ++i)
  {
    float prevT = t;
    t = (float)i * increment;
  #ifdef USE_SLERPS_NOT_LERPS
    qInternal.fastSlerp(qFromInternal, target, t, fromDotTo);
  #else
    qInternal.lerp(qFromInternal, target, t, fromDotTo);
  #endif
    float degreeOfViolationFactor = degreeOfViolationSimple(params, qInternal, smoothness);
    float violationFactor = limitViolationFactor(degreeOfViolationFactor);
    float previousSum = sum;
    if (i == 0)
    {
      startViolationFactor = violationFactor;
    }
    else
    {
      sum += violationFactor;
    }
    if (degreeOfViolationFactor >= 1.0f - FLT_EPSILON)
    {
      qInternal = prevQ;
      break;
    }
    if (sum > softnessFactor)
    {
      float interpFactor = (softnessFactor - previousSum) / (sum - previousSum);
      t = prevT + interpFactor * increment;
  #ifdef USE_SLERPS_NOT_LERPS
      qInternal.fastSlerp(qFromInternal, target, t, fromDotTo);
  #else
      qInternal.lerp(qFromInternal, target, t, fromDotTo);
  #endif
      break;
    }

    prevQ = qInternal;
  }

  // Return the actual amount of rotation
  if (w)
  {
    *w = t;
  }

#else

  float startDegree = degreeOfViolationSimple(params, qFromInternal, smoothness);
  float endDegree = degreeOfViolationSimple(params, qInternal, smoothness);
  float startFactor = limitViolationFactor(startDegree);
  float endFactor = limitViolationFactor(endDegree);
  float increase = endFactor - startFactor;

  // Joint already on limit, being driven through limit
  if (startDegree >= 1.0f - FLT_EPSILON && increase >= 0)
  {
    qInternal = qFromInternal;
    if (w)
    {
      *w = 0;
    }
  }

  // Normal situation
  else if (increase > softnessFactor)
  {
    float t = a;
    for (uint32_t iter = 0; iter < maxIters; ++iter)
    {
      t = 0.5f * (a + b);
  #ifdef USE_SLERPS_NOT_LERPS
      qInternal.fastSlerp(qFromInternal, target, t, fromDotTo);
  #else
      qInternal.lerp(qFromInternal, target, t, fromDotTo);
  #endif
      float degreeOfViolationFactor = degreeOfViolationSimple(params, qInternal, smoothness);
      float violationFactor = limitViolationFactor(degreeOfViolationFactor);
      increase = violationFactor - startFactor;
      if (increase > softnessFactor || degreeOfViolationFactor >= 1.0f)
      {
        b = t;
      }
      else
      {
        a = t;
      }
    }

    // Ensure the orientation returned is the valid side of the soft limit
  #ifdef USE_SLERPS_NOT_LERPS
    qInternal.fastSlerp(qFromInternal, target, a, fromDotTo);
  #else
    qInternal.lerp(qFromInternal, target, a, fromDotTo);
  #endif

    // Return the actual amount of rotation
    if (w)
    {
      *w = a;
    }
  }

  // In practice, this means the desired orientation never was violating the limit in the first place
  else if (w)
  {
    *w = 1.0f;
  }

#endif // INTEGRATION_METHOD
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimits::weightBetween(
  const Params& params,
  const NMP::Quat& qFrom,
  NMP::Quat& q,
  float softness,
  float smoothness,
  float* w /* = 0 */)
{
  // This algorithm weights against motion near the limit boundaries.  It is a discrete
  // integration of a modified degree of violation factor along the arc of motion.  Effectively
  // motion is stopped at the point where the integral goes over the input softness value,
  // although this is only approximate.

  // Convert into limit frame and keep it that way until we've finished the search.
  NMP::Quat qFromInternal;
  toInternal(params, qFrom, qFromInternal);
  NMP::Quat qInternal;
  toInternal(params, q, qInternal);

  // The algorithm itself is moved into this function to separate out the transform into and out
  // of the limit frame.
  weightBetweenSimple(params, qFromInternal, qInternal, softness, smoothness, w);

  // Convert back to joint space.
  toJoint(params, qInternal, q);
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimits::weightBetweenSimple(
  const Params& params,
  const NMP::Quat& qFromInternal,
  NMP::Quat& qInternal,
  float softness,
  float smoothness,
  float* w /* = 0 */)
{
  // This algorithm weights against motion near the limit boundaries.  It is a discrete
  // integration of a modified degree of violation factor along the arc of motion.  Effectively
  // motion is stopped at the point where the integral goes over the input softness value,
  // although this is only approximate.

  // For hinges, just throw away illegal swing
  if (params.upper.y == 0 && params.upper.z == 0)
  {
    clampHingeSimple(qInternal);
  }

  const uint32_t maxDivisions = 20;
  const float increment = 1.0f / (maxDivisions - 1);
  // Get rotations into same semi-arc for interpolation
  float fromDotTo = qFromInternal.dot(qInternal);
  if (fromDotTo < 0)
  {
    fromDotTo = -fromDotTo;
    qInternal = -qInternal;
  }
  NMP::Quat target = qInternal;

  float sum = 0;
  float t = 0;
  for (uint32_t i = 0; i < maxDivisions; ++i)
  {
    float prevT = t;
    t = (float)i * increment;
#ifdef USE_SLERPS_NOT_LERPS
    qInternal.fastSlerp(qFromInternal, target, t, fromDotTo);
#else
    qInternal.lerp(qFromInternal, target, t, fromDotTo);
#endif
    float degreeOfViolationFactor = degreeOfViolationSimple(params, qInternal, smoothness);
    float violationFactor = softLimitViolationFactor(degreeOfViolationFactor);
    float previousSum = sum;
    sum += violationFactor;
    if (sum > softness)
    {
      float interpFactor = (softness - previousSum) / (sum - previousSum);
      t = prevT + interpFactor * increment;
#ifdef USE_SLERPS_NOT_LERPS
      qInternal.fastSlerp(qFromInternal, target, t, fromDotTo);
#else
      qInternal.lerp(qFromInternal, target, t, fromDotTo);
#endif
      break;
    }
  }

  // Return the actual amount of rotation
  if (w)
  {
    *w = t;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimits::rotateToLimit(
  const Params& params,
  NMP::Quat& q,
  const NMP::Vector3& axis,
  float smoothness /* = 0 */)
{
  // Newton iteration to find minimum of distance-from-limit function
  const uint32_t maxIters = 30;
  uint32_t iter = 0;
  float delta = 1e-4f;
  NMP::Vector3 rotvec = axis * delta;
  NMP::Quat qRot;
  qRot.fromRotationVector(rotvec, true);
  NMP::Quat qRotInv = qRot;
  qRotInv.conjugate();
  float t;
  float fdash;
  do
  {
    float f = degreeOfViolation(params, q, smoothness);
    f *= f;
    float fminus = degreeOfViolation(params, qRot * q, smoothness);
    fminus *= fminus;
    float fplus = degreeOfViolation(params, qRotInv * q, smoothness);
    fplus *= fplus;
    fdash = fplus - f;
    float fdoubledash = fplus - (2.0f * f) + fminus;
    t = -fdash * delta / fdoubledash;
    rotvec = axis * t;
    NMP::Quat qStep;
    qStep.fromRotationVector(rotvec, true);
    q = qStep * q;
    ++iter;
  }
  while (iter < maxIters && fdash > 1e-5f);
}

//----------------------------------------------------------------------------------------------------------------------
char JointLimits::clampBetweenJoint(const Params& params, const NMP::Quat& qFrom, NMP::Quat& q)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed
  NMP_ASSERT(!isViolated(Params(params, 1.01f), qFrom));
  NMP_ASSERT(isViolated(params, q));

  // Orientation in the limit frame
  NMP::Quat qFromJoint; toInternal(params, qFrom, qFromJoint);
  NMP::Quat qJoint; toInternal(params, q, qJoint);

  NMP::Vector3 tlFrom, tl;
  GeomUtils::quatToTwistSwing(qFromJoint, tlFrom);
  GeomUtils::quatToTwistSwing(qJoint, tl);

  const float& l1From = tlFrom.y;
  const float& l2From = tlFrom.z;
  float& t = tl.x;
  float& l1 = tl.y;
  float& l2 = tl.z;

  // Find the right twist, simply by clamping
  char retval = 1;
  t = t < params.lower.x ? params.lower.x : (t > params.upper.x ? params.upper.x : (retval = 0, t));

  // Find the right swing, by intersecting the line between the two positions in swing space and
  // the swing limit ellipse, and choosing the closest intersection to the target orientation.
  //   Test in case limits are zero - avoid divide by zero
  if (params.upper.y < GeomUtils::nTol || params.upper.z < GeomUtils::nTol)
  {
    // Set the return value
    retval |= l1From < -params.upper.y ? 2 : (l1From > params.upper.y ? 2 : 0);
    retval |= l2From < -params.upper.z ? 2 : (l2From > params.upper.z ? 2 : 0);
    // We needn't do any clamping in this case, just return the From swing, because it couldn't go anywhere
    l1 = l1From;
    l2 = l2From;
  }
  else
  {
    float l1unit = l1 / params.upper.y;
    float l2unit = l2 / params.upper.z;
    if (l1unit * l1unit + l2unit * l2unit > 1.0f)
    {
      // We are clamping swing
      retval |= 2;

      // This algorithm is not iterative, but it does involve solving a quadratic for the
      // intersection of a line and an ellipse
      float aa = params.upper.y * params.upper.y;
      float bb = params.upper.z * params.upper.z;

      // Get points on line (actually, just rename the ones we have)
      NMP::Vector3 m1(l1From, l2From, 1.0f);
      NMP::Vector3 m2(l1, l2, 1.0f);

      //   Coefficients
      float a = m2[0] * m2[0] / aa + m2[1] * m2[1] / bb - m2[2] * m2[2];
      float b = 2 * (m1[0] * m2[0] / aa + m1[1] * m2[1] / bb - m1[2] * m2[2]);
      float c = m1[0] * m1[0] / aa + m1[1] * m1[1] / bb - m1[2] * m1[2];
      float coeff1sqr = b * b - 4 * a * c;
      // The line could be missing the ellipse entirely if both input swings were outside the ellipse,
      // but in debug mode that should have been picked up by an assert at the top of the function
      if (coeff1sqr < 0) coeff1sqr = 0;  // In case we are numerically on the ellipse
      float coeff1 = sqrtf(coeff1sqr);
      float coeff2 = 2.0f * a;
      float lambda1 = (-b + coeff1) / coeff2;
      float lambda2 = (-b - coeff1) / coeff2;
      NMP::Vector3 x1 = m1 + m2 * lambda1;
      NMP::Vector3 x2 = m1 + m2 * lambda2;

      // Make non-homogeneous
      // This just corrects for numerical instability and ensures we get an answer
      if (NMP::nmfabs(x1[2]) < GeomUtils::nTol) x1[2] = (x1[2] >= 0) ? GeomUtils::nTol : -GeomUtils::nTol;
      if (NMP::nmfabs(x2[2]) < GeomUtils::nTol) x2[2] = (x2[2] >= 0) ? GeomUtils::nTol : -GeomUtils::nTol;
      x1 /= x1[2];
      x2 /= x2[2];

      // Choose closest of the two solutions to the desired swing
      float s1 = x1.distanceSquaredTo(m2);
      float s2 = x2.distanceSquaredTo(m2);
      if (s1 < s2)
      {
        l1 = x1[0];
        l2 = x1[1];
      }
      else
      {
        l1 = x2[0];
        l2 = x2[1];
      }
    }
  }

  GeomUtils::twistSwingToQuat(tl, qJoint);

  // Orientation back in parent frame
  toJoint(params, qJoint, q);

  NMP_ASSERT(!isViolated(Params(params, 1.01f), q));

  return retval;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat JointLimits::interpolate(const Params& params, const NMP::Quat& qFrom, const NMP::Quat& qTo, float t)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  // Orientation in the limit frame
  NMP::Quat qFromJoint; toInternal(params, qFrom, qFromJoint);
  NMP::Quat qToJoint; toInternal(params, qTo, qToJoint);

  NMP::Vector3 tlFrom, tlTo;
  GeomUtils::quatToTwistSwing(qFromJoint, tlFrom);
  GeomUtils::quatToTwistSwing(qToJoint, tlTo);

  // Swing is so close to angular space that I can safely do linear interpolation in this space
  // So the whole thing boils down to linear interpolation
  NMP::Vector3 tlOut = tlFrom * (1 - t) + tlTo * t;

  GeomUtils::twistSwingToQuat(tlOut, qToJoint);

  // Orientation back in parent frame
  NMP::Quat qReturned; toJoint(params, qToJoint, qReturned);
  return qReturned;
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimits::softenNearFlippingPoint(
  const Params& params,
  const NMP::Quat& fromQ,
  NMP::Quat& toQ,
  float flipSoftness)
{
  // Don't waste time doing stuff for zero softness (plus there's a divide-by-zero risk)
  if (flipSoftness < GeomUtils::nTol)
  {
    return;
  }

  // Get the rotations in the limit frame
  NMP::Quat qInternal;
  toInternal(params, toQ, qInternal);
  NMP::Quat qFromInternal;
  toInternal(params, fromQ, qFromInternal);

  // The algorithm itself is moved into this function to separate out the transform into and out
  // of the limit frame
  bool didSoften = softenNearFlippingPointSimple(qFromInternal, qInternal, flipSoftness);

  // Convert back to the correct frame
  if (didSoften)
  {
    toJoint(params, qInternal, toQ);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool JointLimits::softenNearFlippingPointSimple(
  const NMP::Quat& qFromInternal,
  NMP::Quat& qInternal,
  float flipSoftness)
{
  // The centre of the limit frame is the identity (w = 1, x, y, z = 0), so 180 degrees from the centre
  // of the limit frame, w is zero.  So when w is small, we're near the flipping point.
  float condition = NMP::nmfabs(qInternal.w);
  if (condition < flipSoftness)
  {
    // This code reduces the contribution of the to rotation to the output as we approach the
    // flipping point.
    condition /= flipSoftness;
    qInternal *= condition;

    // Get the rotations in the same semi-arc for interpolation
    if (qFromInternal.dot(qInternal) < 0)
    {
      qInternal *= -1.0f;
    }

    // Interpolate between the to and from rotations using simple linear interpolation
    //   If the to and from rotations are on opposite 'sides' of the flipping point, we want to
    //   interpolate around the longest arc rather than the shortest.
    if (qInternal.w * qFromInternal.w < 0)
    {
      qInternal -= qFromInternal * (1.0f - condition);
    }
    else
    {
      qInternal += qFromInternal * (1.0f - condition);
    }
    qInternal.normalise();

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimits::getHingeAxis(
  const Params& limit,
  NMP::Vector3& axis,
  const NMP::Vector3& parentBone,
  const NMP::Vector3& childBone)
{
  // At the moment, assume we are only using NMSTANDARD limits - this tells us that the twist axis is
  // the x-axis
  NMP_ASSERT(limit.type == JointLimits::NMSTANDARD_JOINTLIMITS);

  // Hinge axis is the x-axis or negative x-axis in the limit frame
  axis.set(1.0f, 0.0f, 0.0f);

  // Find the orientation when the two bones are parallel in the plane perpendicular
  // to the twist axis.  Do calculations in the limit frame.
  NMP::Vector3 v1 = limit.frame.inverseRotateVector(parentBone);
  NMP::Vector3 v2 = limit.offset.inverseRotateVector(childBone);
  v1.normalise();
  v2.normalise();
  NMP::Quat straightQ = GeomUtils::forRotationAroundAxis(v2, v1, axis);

  // Find the orientation when the joint is on its minimum and maximum twist limit
  NMP::Quat lowerTwistLimitQ;
  NMP::Quat upperTwistLimitQ;
  GeomUtils::twistSwingToQuat(NMP::Vector3(limit.lower.x, 0, 0), lowerTwistLimitQ);
  GeomUtils::twistSwingToQuat(NMP::Vector3(limit.upper.x, 0, 0), upperTwistLimitQ);

  // Find whichever of the two is closest to the straight orientation.  This is then our
  // 'furthest bent backwards' limit.  If it turns out it's the lower limit, the hinge
  // axis needs to be reversed.
  float dotQLower = NMP::nmfabs(lowerTwistLimitQ.dot(straightQ));
  float dotQUpper = NMP::nmfabs(upperTwistLimitQ.dot(straightQ));
  // The cosine of an angle is larger the smaller the angle.
  if (dotQLower > dotQUpper)
  {
    axis = -axis;
  }

  // Rotate hinge axis back into joint frame.
  axis = limit.frame.rotateVector(axis);

  axis.normalise();
}

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_UNIT_TESTING
  #include <cstring>
bool JointLimits::Params::saveCode(char prefixString[], std::ostream& stream)
{
  const int DUMPSIZE = 1024;
  int n;
  char dump[DUMPSIZE];
  n = NMP_SPRINTF(dump, DUMPSIZE, "%sframe.setXYZW(%ff, %ff, %ff, %ff);\n", prefixString, frame.x, frame.y, frame.z, frame.w);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%soffset.setXYZW(%ff, %ff, %ff, %ff);\n", prefixString, offset.x, offset.y, offset.z, offset.w);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%slower.set(%ff, %ff, %ff);\n", prefixString, lower.x, lower.y, lower.z);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%supper.set(%ff, %ff, %ff);\n", prefixString, upper.x, upper.y, upper.z);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  n = NMP_SPRINTF(dump, DUMPSIZE, "%stype = (JointLimits::LimitType)%d;\n", prefixString, (int)type);
  if (n < 0 || n >= DUMPSIZE) return false;
  stream << dump;
  return true;
}
#endif

} // end of namespace NMRU
