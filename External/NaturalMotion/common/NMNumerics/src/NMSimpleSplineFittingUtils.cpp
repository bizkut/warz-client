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
#include "NMNumerics/NMSimpleSplineFittingUtils.h"

// Disable warnings constant conditionals
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4127)
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
static const float sfutils_tol = 1e-4f;

//----------------------------------------------------------------------------------------------------------------------
// SimpleSplineFittingUtils
//----------------------------------------------------------------------------------------------------------------------
bool SimpleSplineFittingUtils::isPosChannelUnchanging(const Vector3& pa, const Vector3& pb)
{
  Vector3 p;
  p = pb - pa;
  p.x = fabs(p.x);
  p.y = fabs(p.y);
  p.z = fabs(p.z);
  if (p.x > sfutils_tol || p.y > sfutils_tol || p.z > sfutils_tol)
  {
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleSplineFittingUtils::isPosChannelUnchanging(uint32_t numSamples, const Vector3* data)
{
  Vector3 p, p0;
  if (numSamples < 2) return true;

  p0 = data[0];
  for (uint32_t i = 1; i < numSamples; ++i)
  {
    p = data[i] - p0;
    p.x = fabs(p.x);
    p.y = fabs(p.y);
    p.z = fabs(p.z);
    if (p.x > sfutils_tol || p.y > sfutils_tol || p.z > sfutils_tol)
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleSplineFittingUtils::isPosChannelUnchanging(const Vector3& pa, uint32_t numSamples, const Vector3* data)
{
  Vector3 p;
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    p = data[i] - pa;
    p.x = fabs(p.x);
    p.y = fabs(p.y);
    p.z = fabs(p.z);
    if (p.x > sfutils_tol || p.y > sfutils_tol || p.z > sfutils_tol)
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleSplineFittingUtils::isQuatChannelUnchanging(const Quat& qa, const Quat& qb)
{
  Quat q;
  q = qb - qa;
  q.x = fabs(q.x);
  q.y = fabs(q.y);
  q.z = fabs(q.z);
  q.w = fabs(q.w);
  if (q.x > sfutils_tol || q.y > sfutils_tol || q.z > sfutils_tol || q.w > sfutils_tol)
  {
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleSplineFittingUtils::isQuatChannelUnchanging(uint32_t numSamples, const Quat* quats)
{
  Quat q, q0;
  if (numSamples < 2) return true;

  q0 = quats[0];
  for (uint32_t i = 1; i < numSamples; ++i)
  {
    q = quats[i] - q0;
    q.x = fabs(q.x);
    q.y = fabs(q.y);
    q.z = fabs(q.z);
    q.w = fabs(q.w);
    if (q.x > sfutils_tol || q.y > sfutils_tol || q.z > sfutils_tol || q.w > sfutils_tol)
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleSplineFittingUtils::isQuatChannelUnchanging(const Quat& qa, uint32_t numSamples, const Quat* quats)
{
  Quat q;
  for (uint32_t i = 0; i < numSamples; ++i)
  {
    q = quats[i] - qa;
    q.x = fabs(q.x);
    q.y = fabs(q.y);
    q.z = fabs(q.z);
    q.w = fabs(q.w);
    if (q.x > sfutils_tol || q.y > sfutils_tol || q.z > sfutils_tol || q.w > sfutils_tol)
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleSplineFittingUtils::normaliseQuats(uint32_t numSamples, Quat* quats)
{
  NMP_ASSERT(quats);

  for (uint32_t i = 0; i < numSamples; ++i)
  {
    quats[i].normalise();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleSplineFittingUtils::conditionQuats(uint32_t numSamples, Quat* quats)
{
  NMP_ASSERT(quats);
  if (numSamples == 0) return;

  // Condition the quaternion data so that consecutive quaternions lie in the same hemisphere
  Quat q = quats[0];
  q.normalise();
  if (q.w < 0.0f) q = -q;
  quats[0] = q;

  for (uint32_t i = 1; i < numSamples; ++i)
  {
    q = quats[i];
    q.normalise();
    if (q.dot(quats[i-1]) < 0.0f) q = -q;
    quats[i] = q;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleSplineFittingUtils::isKnotVectorValid(uint32_t numKnots, const float* knots)
{
  if (numKnots < 2) return false;

  for (uint32_t i = 1; i < numKnots; ++i)
  {
    if (knots[i] <= knots[i-1]) return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleSplineFittingUtils::isKnotVectorValid(uint32_t numKnots, const uint32_t* knots)
{
  if (numKnots < 2) return false;

  for (uint32_t i = 1; i < numKnots; ++i)
  {
    if (knots[i] <= knots[i-1]) return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
int32_t SimpleSplineFittingUtils::interval(float t, uint32_t& intv, uint32_t numSamples, const float* ts)
{
  NMP_ASSERT(numSamples >= 2);

  int32_t rgn = 0;
  uint32_t ext = numSamples - 1;
  uint32_t low = 1;
  uint32_t high = ext - 1;

  // Check if outside the knot range
  if (t < ts[low])
  {
    if (t < ts[0]) rgn = -1;
    intv = 0;
  }
  else if (t >= ts[high])
  {
    intv = high; // Special case for the rightmost interval
    if (t > ts[ext]) rgn = 1;
  }
  else
  {
    // Determine the correct interval by binary search
    intv = (high + low) >> 1;
    while (true)
    {
      if (t < ts[intv])
      {
        high = intv;
      }
      else if (t >= ts[intv+1])
      {
        low = intv;
      }
      else
      {
        break; // Found the interval
      }
      intv = (high + low) >> 1;
    }
  }

  return rgn;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3 SimpleSplineFittingUtils::posInterpolate(float t, uint32_t numSamples, const float* ts, const Vector3* data)
{
  Vector3 p;

  // Get the interval within the sample vector that t lies in
  uint32_t intv;
  interval(t, intv, numSamples, ts);

  // Compute the interpolated quaternion
  float u = (t - ts[intv]) / (ts[intv+1] - ts[intv]);
  p.lerp(data[intv], data[intv+1], u);

  return p;
}

//----------------------------------------------------------------------------------------------------------------------
Quat SimpleSplineFittingUtils::quatInterpolate(float t, uint32_t numSamples, const float* ts, const Quat* quats)
{
  Quat q;

  // Get the interval within the sample vector that t lies in
  uint32_t intv;
  interval(t, intv, numSamples, ts);

  // Compute the interpolated quaternion
  float u = (t - ts[intv]) / (ts[intv+1] - ts[intv]);
  q.slerp(quats[intv], quats[intv+1], u);

  return q;
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleSplineFittingUtils::arcLengthVector(uint32_t numSamples, const Vector3* data, float* result)
{
  if (numSamples == 0) return;
  NMP_ASSERT(data);
  NMP_ASSERT(result);

  Vector3 dp;
  float d;

  // Initialise the arc length vector
  result[0] = 0.0f;

  for (uint32_t i = 1; i < numSamples; ++i)
  {
    // Compute the distance between the current and last positions
    dp = data[i] - data[i-1];
    d = dp.magnitude();

    // Set the arc length vector
    result[i] = result[i-1] + d;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SimpleSplineFittingUtils::angleLengthVector(uint32_t numSamples, const Quat* quats, float* result)
{
  if (numSamples == 0) return;
  NMP_ASSERT(quats);
  NMP_ASSERT(result);

  Quat qa, qb, iqa, dq;
  float w, ang;

  // Get the first quaternion keyframe
  qa = quats[0];
  qa.normalise();
  if (qa.w < 0.0f) qa = -qa;
  result[0] = 0.0f; // Initialise the angle length vector

  for (uint32_t i = 1; i < numSamples; ++i)
  {
    // Get the next quaternion keyframe in the hemisphere of the last
    qb = quats[i];
    qb.normalise();
    if (qb.dot(qa) < 0.0f) qb = -qb;

    // Compute the angle between the current and last quaternions
    iqa = qa;
    iqa.conjugate();
    dq = iqa * qb;
    w = NMP::clampValue(dq.w, -1.0f, 1.0f);
    ang = 2 * acos(w); // in [0:pi]

    // Set the angle length vector
    result[i] = result[i-1] + ang;

    // Update the old quaternion value
    qa = qb;
  }
}

//----------------------------------------------------------------------------------------------------------------------
int32_t SimpleSplineFittingUtils::findSampleSpan(
  float     t,
  uint32_t& intv,
  uint32_t  ia,
  uint32_t  ib,
  uint32_t NMP_USED_FOR_ASSERTS(numSamples),
  const float* ts)
{
  NMP_ASSERT(numSamples >= 2);

  int32_t rgn = 0;

  // Check if outside the sample range
  if (t <= ts[ia])
  {
    if (t < ts[ia]) rgn = -1;
    intv = 0;
  }
  else if (t >= ts[ib]) // Includes last sample
  {
    if (t > ts[ib]) rgn = 1;
    intv = ib;
  }
  else
  {
    // Determine the correct interval by binary search
    intv = (ib + ia) >> 1;
    while (true)
    {
      if (t < ts[intv])
      {
        ib = intv;
      }
      else if (t >= ts[intv+1])
      {
        ia = intv;
      }
      else
      {
        break; // Found the sample interval
      }
      intv = (ib + ia) >> 1;
    }
  }

  return rgn;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleSplineFittingUtils::findSamplesExclusive(float ta, float tb, uint32_t& ia, uint32_t& ib, uint32_t numSamples, const float* ts)
{
  NMP_ASSERT(ta < tb);

  uint32_t ka = 0;
  uint32_t kb = numSamples - 1;

  // Check if outside the sample range
  if (ta >= ts[kb] || tb <= ts[ka])
  {
    ia = kb;
    ib = ka;
    return false;
  }

  // Find the sample span that ta lies on
  if (findSampleSpan(ta, ia, ka, kb, numSamples, ts) == 0)
  {
    ++ia; // Exclusive
  }

  // Find the sample span that tb lies on
  ka = ia;
  if (findSampleSpan(tb, ib, ka, kb, numSamples, ts) == 0)
  {
    if (tb == ts[ib]) --ib; // Exclusive
  }

  return ia <= ib;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleSplineFittingUtils::findSamplesInclusive(float ta, float tb, uint32_t& ia, uint32_t& ib, uint32_t numSamples, const float* ts)
{
  NMP_ASSERT(ta < tb);

  uint32_t ka = 0;
  uint32_t kb = numSamples - 1;

  // Check if outside the sample range
  if (ta >= ts[kb] || tb <= ts[ka])
  {
    ia = kb;
    ib = ka;
    return false;
  }

  // Find the sample span that ta lies on
  if (findSampleSpan(ta, ia, ka, kb, numSamples, ts) == 0)
  {
    if (ta > ts[ia]) ++ia; // Inclusive
  }

  // Find the sample span that tb lies on
  ka = ia;
  findSampleSpan(tb, ib, ka, kb, numSamples, ts);

  return ia <= ib;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleSplineFittingUtils::findSamplesInclusiveTa(float ta, float tb, uint32_t& ia, uint32_t& ib, uint32_t numSamples, const float* ts)
{
  NMP_ASSERT(ta < tb);

  uint32_t ka = 0;
  uint32_t kb = numSamples - 1;

  // Check if outside the sample range
  if (ta >= ts[kb] || tb <= ts[ka])
  {
    ia = kb;
    ib = ka;
    return false;
  }

  // Find the sample span that ta lies on
  if (findSampleSpan(ta, ia, ka, kb, numSamples, ts) == 0)
  {
    if (ta > ts[ia]) ++ia; // Inclusive
  }

  // Find the sample span that tb lies on
  ka = ia;
  if (findSampleSpan(tb, ib, ka, kb, numSamples, ts) == 0)
  {
    if (tb == ts[ib]) --ib; // Exclusive
  }

  return ia <= ib;
}

//----------------------------------------------------------------------------------------------------------------------
bool SimpleSplineFittingUtils::findSamplesInclusiveTb(float ta, float tb, uint32_t& ia, uint32_t& ib, uint32_t numSamples, const float* ts)
{
  NMP_ASSERT(ta < tb);

  uint32_t ka = 0;
  uint32_t kb = numSamples - 1;

  // Check if outside the sample range
  if (ta >= ts[kb] || tb <= ts[ka])
  {
    ia = kb;
    ib = ka;
    return false;
  }

  // Find the sample span that ta lies on
  if (findSampleSpan(ta, ia, ka, kb, numSamples, ts) == 0)
  {
    ++ia; // Exclusive
  }

  // Find the sample span that tb lies on
  ka = ia;
  findSampleSpan(tb, ib, ka, kb, numSamples, ts);

  return ia <= ib;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
