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
// inline included by NMGeomUtils.h
//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

namespace GeomUtils
{

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE PosQuat identityPosQuat()
{
  PosQuat pq;
  pq.q.identity();
  pq.t.setToZero();
  return pq;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void identityPosQuat(PosQuat& pq)
{
  pq.q.identity();
  pq.t.setToZero();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void fromMatrix34PosQuat(PosQuat& pq, const NMP::Matrix34& m)
{
  pq.q = m.toQuat();
  pq.t = m.translation();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void fromPosQuatMatrix34(NMP::Matrix34& m, const PosQuat& pq)
{
  m.fromQuat(pq.q);
  m.translation() = pq.t;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void multiplyPosQuat(PosQuat& result, const PosQuat& pq1, const PosQuat& pq2)
{
  result.q.multiply(pq2.q, pq1.q); // Note switch because quats operate this way round
  result.t = pq2.q.rotateVector(pq1.t) + pq2.t;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void multiplyPosQuat(PosQuat& result, const PosQuat& pq)
{
  result.q = pq.q * result.q;
  result.t = pq.q.rotateVector(result.t) + pq.t;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void premultiplyPosQuat(const PosQuat& pq, PosQuat& result)
{
  result.t += result.q.rotateVector(pq.t);
  result.q.multiply(pq.q);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void premultiplyInversePosQuat(const PosQuat& pq, PosQuat& result)
{
  result.q *= ~pq.q;
  result.t -= result.q.rotateVector(pq.t);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void multiplyInversePosQuat(PosQuat& result, const PosQuat& pq1, const PosQuat& pq2)
{
  result.q = ~pq2.q * pq1.q;
  result.t = pq2.q.inverseRotateVector(pq1.t - pq2.t);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void multiplyInversePosQuat(PosQuat& result, const PosQuat& pq)
{
  result.q = ~pq.q * result.q;
  result.t = pq.q.inverseRotateVector(result.t - pq.t);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void rotatePosQuat(const NMP::Quat& q, PosQuat& result)
{
  result.q = q * result.q;
  result.t = q.rotateVector(result.t);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void inverseRotatePosQuat(const NMP::Quat& q, PosQuat& result)
{
  result.q = ~q * result.q;
  result.t = q.inverseRotateVector(result.t);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void transformVectorPosQuat(const PosQuat& pq, NMP::Vector3& result)
{
  result = pq.q.rotateVector(result) + pq.t;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void inverseTransformVectorPosQuat(const PosQuat& pq, NMP::Vector3& result)
{
  result = pq.q.inverseRotateVector(result - pq.t);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void invertPosQuat(PosQuat& result, const PosQuat& pq)
{
  result.q = ~pq.q;
  result.t = -pq.q.inverseRotateVector(pq.t);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void invertPosQuat(PosQuat& result)
{
  result.t = -result.q.inverseRotateVector(result.t);
  result.q = ~result.q;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void interpolatePosQuat(PosQuat& result, const PosQuat& pqFrom, const PosQuat& pqTo, float factor)
{
  result.q.slerp(pqFrom.q, pqTo.q, factor);
  result.t.lerp(pqFrom.t, pqTo.t, factor);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void interpolatePosQuat(PosQuat& result, const PosQuat& pqTo, float factor)
{
  result.q.slerp(pqTo.q, factor);
  result.t.lerp(pqTo.t, factor);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void interpolatePosQuat(
  PosQuat& result,
  const PosQuat& pqFrom,
  const PosQuat& pqTo,
  float orientationFactor,
  float positionFactor)
{
  result.q.slerp(pqFrom.q, pqTo.q, orientationFactor);
  result.t.lerp(pqFrom.t, pqTo.t, positionFactor);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void interpolatePosQuat(PosQuat& result, const PosQuat& pqTo, float orientationFactor, float positionFactor)
{
  result.q.slerp(pqTo.q, orientationFactor);
  result.t.lerp(pqTo.t, positionFactor);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void mirrorPosQuat(PosQuat& result, bool rx, bool ry, bool rz, bool tx, bool ty, bool tz)
{
  // OPTIMISE  These are just vector scalings so can be vectorised
  NMP::Quat mirrorQ(rx ? -1.0f : 1.0f, ry ? -1.0f : 1.0f, rz ? -1.0f : 1.0f, 1.0f);
  result.q.x *= mirrorQ.x;
  result.q.y *= mirrorQ.y;
  result.q.z *= mirrorQ.z;

  NMP::Vector3 mirrorT(tx ? -1.0f : 1.0f, ty ? -1.0f : 1.0f, tz ? -1.0f : 1.0f, 1.0f);
  result.t.x *= mirrorT.x;
  result.t.y *= mirrorT.y;
  result.t.z *= mirrorT.z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Quat closestRotationAroundAxis(const NMP::Quat& q, const NMP::Vector3& axis)
{
  float dot = q.x * axis.x + q.y * axis.y + q.z * axis.z;
  NMP::Quat result(dot * axis.x, dot * axis.y, dot * axis.z, q.w);
  result.normalise();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Quat closestRotationAroundAxis(const NMP::Quat& q, const NMP::Vector3& axis, float* condition)
{
  float dot = q.x * axis.x + q.y * axis.y + q.z * axis.z;
  NMP::Quat result(dot * axis.x, dot * axis.y, dot * axis.z, q.w);
  result.normalise();
  *condition = 1.0f;
  if ((1.0f - q.w) > nTol)
  {
    NMP::Vector3 originalAxis(q.x, q.y, q.z);
    originalAxis.normalise();
    *condition = NMP::nmfabs(originalAxis.dot(axis));
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Quat closestRotationAroundAxisSoft(const NMP::Quat& q, const NMP::Vector3& axis, float softness)
{
  float condition;
  NMP::Quat qresult = closestRotationAroundAxis(q, axis, &condition);
  if (softness > 0)
  {
    condition /= softness;
    if (condition < 1.0f)
    {
      qresult *= condition;
      if (qresult.w > 0)
      {
        qresult.w += 1.0f - condition;
      }
      else
      {
        qresult.w -= 1.0f - condition;
      }
      qresult.normalise();
    }
  }
  return qresult;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Quat forRotationSoft(const NMP::Vector3& fromV, const NMP::Vector3& toV, float softness)
{
  NMP::Quat qresult;
  qresult.forRotation(fromV, toV);
  float condition = NMP::nmfabs(1.0f + fromV.dot(toV));
  if (softness > 0)
  {
    condition /= softness;
    if (condition < 1.0f)
    {
      qresult *= condition;
      if (qresult.w > 0)
      {
        qresult.w += 1.0f - condition;
      }
      else
      {
        qresult.w -= 1.0f - condition;
      }
      qresult.normalise();
    }
  }
  return qresult;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Quat forRotationAroundAxis(const NMP::Vector3& fromV, const NMP::Vector3& toV, const NMP::Vector3& axis)
{
  // We have
  //   Ksin(RotationAngle) = -toV.dot(axis.cross(fromV)), and
  //   Kcos(RotationAngle) =  toV.dot(axis.cross(axis.cross(fromV)))
  // This could give us the minimum rather than the maximum, but it turns out that you always get the
  // minimum by negating n, which is equivalent to choosing one of two solutions to arctan - see the
  // wiki page for clarification.
  NMP::Vector3 axisCrossV1;  axisCrossV1.cross(axis, fromV);
  NMP::Vector3 axisCrossAxisCrossV1;  axisCrossAxisCrossV1.cross(axis, axisCrossV1);
  float n = -toV.dot(axisCrossV1);
  float d = toV.dot(axisCrossAxisCrossV1);

  float K = -NMP::fastSqrt(n * n + d * d);
  // qResult contains [v, w] = [2Kcos(theta/2)sin(theta/2)*axis, 2Kcos(theta/2)cos(theta/2)]
  NMP::Quat qResult(n * axis.x, n * axis.y, n * axis.z, d + K);

  // fastNormalise() handles the zero-length case for us
  qResult.fastNormalise();

  return qResult;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Quat forRotationAroundAxis(
  const NMP::Vector3& fromV,
  const NMP::Vector3& toV,
  const NMP::Vector3& axis,
  float* condition)
{
  NMP::Quat qresult = forRotationAroundAxis(fromV, toV, axis);
  *condition = (1.0f - NMP::nmfabs(fromV.dot(axis))) * (1.0f - NMP::nmfabs(toV.dot(axis)));
  return qresult;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Quat forRotationAroundAxisSoft(
  const NMP::Vector3& fromV,
  const NMP::Vector3& toV,
  const NMP::Vector3& axis,
  float axisSoftness,
  float antiparallelSoftness /* = 0 */)
{
  float axisCondition;
  NMP::Quat qresult = forRotationAroundAxis(fromV, toV, axis, &axisCondition);

  // Softening related to poor conditioning of the hinge axis
  if (axisSoftness > 0)
  {
    axisCondition /= axisSoftness;
    if (axisCondition > 1.0f)
    {
      axisCondition = 1.0f;
    }
  }
  else
  {
    axisCondition = 1.0f;
  }

  // Softening related to the proximity of the vectors to being anti-parallel
  // The formula for the dot product first must project the vectors onto the
  // plane perpendicular to the hinge axis.
  NMP::Vector3 v1 = fromV - axis * axis.dot(fromV);
  NMP::Vector3 v2 = toV - axis * axis.dot(toV);
  v1.normalise();
  v2.normalise();
  float parallelCondition = NMP::nmfabs(-1.0f - v1.dot(v2));

  if (antiparallelSoftness > 0)
  {
    parallelCondition /= antiparallelSoftness;
    if (parallelCondition > 1.0f)
    {
      parallelCondition = 1.0f;
    }
  }
  else
  {
    parallelCondition = 1.0f;
  }

  float condition = axisCondition * parallelCondition;
  qresult *= condition;
  if (qresult.w > 0)
  {
    qresult.w += 1.0f - condition;
  }
  else
  {
    qresult.w -= 1.0f - condition;
  }
  qresult.normalise();
  return qresult;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void quatSelect(NMP::Quat& q, const float& selector)
{
  q.x = NMP::floatSelect(selector, q.x, -q.x);
  q.y = NMP::floatSelect(selector, q.y, -q.y);
  q.z = NMP::floatSelect(selector, q.z, -q.z);
  q.w = NMP::floatSelect(selector, q.w, -q.w);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void shortestArcFastSlerp(NMP::Quat& qFrom, const NMP::Quat& qTo, float t)
{
  float fromDotTo = qFrom.dot(qTo);
  quatSelect(qFrom, fromDotTo); // if (fromDotTo < 0) then qFrom = -qFrom
  qFrom.fastSlerp(qTo, t, NMP::nmfabs(fromDotTo));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void interpOrExtrapSlerp(NMP::Quat& qFrom, const NMP::Quat& qTo, float t)
{
  NMP::Vector3 rFrom = qFrom.toRotationVector(true);
  NMP::Vector3 rTo = qTo.toRotationVector(true);
  float diff = (rFrom - rTo).magnitudeSquared();
  NMP::Vector3 unitVector = rFrom;
  if (unitVector.magnitudeSquared() > gTolSq)
  {
    unitVector.normalise();
    unitVector *= 2.0f;
    NMP::Vector3 rFromLow = rFrom - unitVector;
    NMP::Vector3 rFromHigh = rFrom + unitVector;
    float diffLow = (rFromLow - rTo).magnitudeSquared();
    float diffHigh = (rFromHigh - rTo).magnitudeSquared();
    if (diffLow < diff)
    {
      rFrom = rFromLow;
    }
    else if (diffHigh < diff)
    {
      rFrom = rFromHigh;
    }
  }
  rFrom.lerp(rTo, t);
  qFrom.fromRotationVector(rFrom, true);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void quatToTwistLean(const NMP::Quat& q, NMP::Vector3& tl)
{
  NMP::Vector3 ts;
  GeomUtils::quatToTwistSwing(q, ts);

  const float& t = ts.x;
  const float& s1 = ts.y;
  const float& s2 = ts.z;

  tl.x = 4.0f * atanf(t);
  float tanQuarterTheta = sqrtf(s1 * s1 + s2 * s2);
  if (tanQuarterTheta < 1e-6f)
  {
    tl.y = s1;
    tl.z = s2;
  }
  else
  {
    float scale = 4.0f * atanf(tanQuarterTheta) / tanQuarterTheta;
    tl.y = scale * s1;
    tl.z = scale * s2;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void twistleanToQuat(const NMP::Vector3& tl, NMP::Quat& q)
{
  NMP::Vector3 ts;
  float& t = ts.x;
  float& s1 = ts.y;
  float& s2 = ts.z;

  t = tanf(tl.x / 4.0f);
  float theta = sqrtf(tl.y * tl.y + tl.z * tl.z);
  if (theta < 1e-6f)
  {
    s1 = tl.y;
    s2 = tl.z;
  }
  else
  {
    float scale = tanf(theta / 4.0f) / theta;
    s1 = scale * tl.y;
    s2 = scale * tl.z;
  }

  GeomUtils::twistSwingToQuat(ts, q);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void quatToTwistSwing(const NMP::Quat& q, NMP::Vector3& ts)
{
  float chs2 = q.w * q.w + q.x * q.x;

  if (chs2 > 1.2e-7f)
  {
    float chs = NMP::fastSqrt(chs2);
    float mul = 1.0f / (chs * (chs + 1));

    // If q.w > 0, ts.x = -q.x/(q.w+chs), else ts.x = -q.x/(q.w-chs)
    float denom = q.w + NMP::floatSelect(-q.w, -chs, chs);
    ts.x = -q.x / denom;
    ts.y = mul * (q.x * q.y + q.w * q.z);
    ts.z = mul * (-q.w * q.y + q.x * q.z);
  }
  else
  {
    float rshs = NMP::fastReciprocalSqrt(1.0f - chs2);
    ts.x = 0;
    ts.y = rshs * q.z;
    ts.z = -rshs * q.y;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void twistSwingToQuat(const NMP::Vector3& ts, NMP::Quat& q)
{
  const float& t = ts.x;
  const float& s1 = ts.y;
  const float& s2 = ts.z;

  float b = 2.0f / (1.0f + s1 * s1 + s2 * s2);
  float c = 2.0f / (1.0f + t * t);
  float bm1 = b - 1.0f;
  float cm1 = c - 1.0f;
  float ct = c * t;

  q.w = bm1 * cm1;
  q.x = -t * bm1 * c;
  q.y = -b * (ct * s1 + cm1 * s2);
  q.z = -b * (ct * s2 - cm1 * s1);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void separateTwistFromTwistAndSwing(const NMP::Quat& q, NMP::Quat& qtwist)
{
  float chs2 = q.w * q.w + q.x * q.x;

  if (chs2 > 1e-6f)
  {
    float rchs = 1.0f / sqrtf(chs2);
    qtwist.setWXYZ(q.w * rchs, q.x * rchs, 0, 0);
  }
  else
  {
    qtwist.identity();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void separateSwingFromTwistAndSwing(const NMP::Quat& q, NMP::Quat& qswing)
{
  float chs2 = q.w * q.w + q.x * q.x;

  if (chs2 > 1e-6f)
  {
    float chs = sqrtf(chs2);
    qswing.setWXYZ(chs, 0, (q.w * q.y - q.x * q.z) / chs, (q.x * q.y + q.w * q.z) / chs);
  }
  else
  {
    float rshs = 1.0f / sqrtf(1.0f - chs2);
    qswing.setWXYZ(0, 0, q.y * rshs, q.z * rshs);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void quatToTwistConePlane(const NMP::Quat& q, NMP::Vector3& tcp)
{
  tcp[0] = 2.0f * atan2f(q.x, q.w);
  tcp[1] = 2.0f * atan2f(q.y, q.w);
  tcp[2] = 2.0f * atan2f(q.z, q.w);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void twistConePlaneToQuat(const NMP::Vector3& tcp, NMP::Quat& q)
{
  const float inverseTwoPi = 1.0f / (2.0f * NM_PI);
  const float tolerance = 1e-3f;

  // The tests are to avoid singularities around +-pi, +-3pi, +-5pi etc

  float twistTest = (tcp.x * inverseTwoPi) - 0.5f;
  // round up to the nearest whole number
  float twistTestRounded = (tcp.x < 0.0f ? ceilf(tcp.x - 0.5f) : NMP::nmfloor(tcp.x + 0.5f));
  if (NMP::nmfabs(twistTest - twistTestRounded) < tolerance)
  {
    q.setXYZW(1.0f, 0.0f, 0.0f, 0.0f);
    return;
  }
  else
  {
    q.x = tanf(0.5f * tcp.x);
  }

  float coneTest = (tcp.y * inverseTwoPi) - 0.5f;
  // round up to the nearest whole number
  float coneTestRounded = (tcp.y < 0.0f ? ceilf(tcp.y - 0.5f) : NMP::nmfloor(tcp.y + 0.5f));
  if (NMP::nmfabs(coneTest - coneTestRounded) < tolerance)
  {
    q.setXYZW(0.0f, 1.0f, 0.0f, 0.0f);
    return;
  }
  else
  {
    q.y = tanf(0.5f * tcp.y);
  }

  float planeTest = (tcp.z * inverseTwoPi) - 0.5f;
  // round up to the nearest whole number
  float planeTestRounded = (tcp.z < 0.0f ? ceilf(tcp.z - 0.5f) : NMP::nmfloor(tcp.z + 0.5f));
  if (NMP::nmfabs(planeTest - planeTestRounded) < tolerance)
  {
    q.setXYZW(0.0f, 0.0f, 1.0f, 0.0f);
    return;
  }
  else
  {
    q.z = tanf(0.5f * tcp.z);
  }
  q.w = 1.0f;

  q.normalise();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void solveQuadratic(float a, float b, float c, float& x1, float& x2)
{
  // Test for all zero input parameters
  NMP_ASSERT(a * a + b * b + c * c > 0);

  float coeff1sqr = (b * b - 4.0f * a * c);
  // This tolerance is set heuristically based on the various use cases of solveQuadratic in NM code.
  // The intention is to ensure that users of the solver take care to handle the situation when there
  // is no solution properly, because there is no 'obvious' default output in that case.
#ifdef NMP_ENABLE_ASSERTS
  static const float tolerance = 1e-5f;
  float testVal = NMP::minimum(coeff1sqr, safelyDivide(coeff1sqr, 4.0f * a * a, coeff1sqr));
  NMP_ASSERT(testVal > -tolerance);
#endif

  coeff1sqr = NMP::maximum(coeff1sqr, 0.0f);
  float coeff1 = sqrtf(coeff1sqr);
  float signOfB = NMP::floatSelect(b, 1.0f, -1.0f);
  float t = -0.5f * (b + signOfB * coeff1);
  float tOverA = t / a;
  float cOverT = c / t;
  x1 = NMP::floatSelect(NMP::nmfabs(a) - nTol, tOverA, cOverT);
  x2 = NMP::floatSelect(NMP::nmfabs(t) - nTol, cOverT, tOverA);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 getPointProjectedOntoLine(
  const NMP::Vector3& p0, const NMP::Vector3& p1,
  const NMP::Vector3& point, float* t)
{
  // Get line p1 - p0.
  //
  NMP::Vector3 line = p1 - p0;
  float lineMagSq = line.magnitudeSquared();

  // Provided the points have some separation return projection of point on line
  //
  if (lineMagSq > NMRU::GeomUtils::gTolSq)
  {
    NMP::Vector3 pointFromEdgeStart = point - p0;
    float s = pointFromEdgeStart.dot(line) / lineMagSq;
    if (t)
    {
      *t = s;
    }
    return p0 + s * line;
  }

  // Otherwise return p0, and s == 0
  //
  if (t)
  {
    *t = 0.0f;
  }
  return p0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool isPointOutsideEllipse(float a, float b, float x, float y)
{
  return (x * x / (a * a)) + (y * y / (b * b)) > 1.0f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float safelyDivide(float a, float b, float defaultValue /* = 0 */)
{
  return NMP::floatSelect(NMP::nmfabs(b) - GeomUtils::nTol, a / b, defaultValue);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float fastLog10(float x)
{
  static const float interpPoints[8][2] = {
    { 0.0f, -1000.0f },
    { 0.01f, -2.0f },
    { 0.04f, -1.39794f },
    { 0.1f, -1.0f },
    { 0.22f, -0.657577f },
    { 0.4f, -0.39794f },
    { 0.7f, -0.1549f },
    { 1.0f, 0 }
  };
  uint32_t i = 1;
  for (; i < 7; ++i)
  {
    if (x < interpPoints[i][0])
    {
      break;
    }
  }
  const float* low = interpPoints[i-1];
  const float* high = interpPoints[i];
  float interpFactor = (x - low[0]) / (high[0] - low[0]);
  return low[1] + interpFactor * (high[1] - low[1]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool changeValueIfDifferent(float& origValue, const float newValue, float tolerance)
{
  bool retVal = (NMP::nmfabs(origValue - newValue) > tolerance);
  origValue = newValue;
  return retVal;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T> bool changeValueIfDifferent(T& origValue, const T newValue)
{
  bool retVal = origValue != newValue;
  origValue = newValue;
  return retVal;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void preventValueDivergingFurtherFromLimits(float& value, float minValue, float maxValue, float origValue)
{
  maxValue = NMP::maximum(origValue, maxValue);
  minValue = NMP::minimum(origValue, minValue);
  value = NMP::clampValue(value, minValue, maxValue);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 calculateOrthogonalPerpendicularVector(const NMP::Vector3& fixed, const NMP::Vector3& guide)
{
  return NMP::vCross(fixed, guide);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 calculateOrthogonalPlanarVector(const NMP::Vector3& fixed, const NMP::Vector3& guide)
{
  // uses vector triple product to re-write (fixed ^ guide) ^ fixed without the crosses
  return (fixed.magnitudeSquared() * guide) - (fixed.dot(guide) * fixed);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void calculateOrthogonalVectors(
  NMP::Vector3& perpendicular,
  NMP::Vector3& planar,
  const NMP::Vector3& fixed,
  const NMP::Vector3& guide)
{
  // Undefined behavior if input and output reference the same variable.
  NMP_ASSERT(&perpendicular != &fixed);
  NMP_ASSERT(&perpendicular != &guide);
  NMP_ASSERT(&planar != &fixed);
  NMP_ASSERT(&planar != &guide);

  perpendicular = calculateOrthogonalPerpendicularVector(fixed, guide);
  planar        = calculateOrthogonalPlanarVector(fixed, guide);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void calculateOrthonormalVectors(
  NMP::Vector3& perpendicular,
  NMP::Vector3& planar,
  const NMP::Vector3& fixed,
  const NMP::Vector3& guide)
{
  calculateOrthogonalVectors(perpendicular, planar, fixed, guide);

  perpendicular.normalise();
  planar.normalise();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AABB::addPoint(const NMP::Vector3& p)
{
  m_max.x = NMP::maximum(m_max.x, p.x);
  m_min.x = NMP::minimum(m_min.x, p.x);
  m_max.y = NMP::maximum(m_max.y, p.y);
  m_min.y = NMP::minimum(m_min.y, p.y);
  m_max.z = NMP::maximum(m_max.z, p.z);
  m_min.z = NMP::minimum(m_min.z, p.z);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool AABB::isPointInside(const NMP::Vector3& p, float tolerance) const
{
  return 
    p.x > (m_min.x + tolerance) && (p.x < m_max.x - tolerance) && 
    p.y > (m_min.y + tolerance) && (p.y < m_max.y - tolerance) && 
    p.z > (m_min.z + tolerance) && (p.z < m_max.z - tolerance);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool AABB::isPointIncluded(const NMP::Vector3& p) const
{
  return 
    p.x >= m_min.x && p.x <= m_max.x && 
    p.y >= m_min.y && p.y <= m_max.y && 
    p.z >= m_min.z && p.z <= m_max.z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AABB::setEmpty()
{
  m_min.set(FLT_MAX, FLT_MAX, FLT_MAX);
  m_max = -m_min;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AABB::setInfinite()
{
  m_max.set(FLT_MAX, FLT_MAX, FLT_MAX);
  m_min = -m_min;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AABB::setCentreAndHalfExtents(const NMP::Vector3& centre, const NMP::Vector3& halfExtents)
{
  m_min = centre - halfExtents;
  m_max = centre + halfExtents;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AABB::move(const NMP::Vector3& delta)
{
  m_min += delta;
  m_max += delta;
}


//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool AABB::isEmpty() const
{
  return 
    m_max.x < m_min.x ||
    m_max.y < m_min.y ||
    m_max.z < m_min.z;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 AABB::getCentre() const 
{
  return (m_min + m_max) * 0.5f;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 AABB::getExtents () const
{
  return m_max - m_min;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float AABB::getVolume() const
{
  const NMP::Vector3 v = m_max - m_min;
  return v.x * v.y * v.z;
}


} // end of namespace GeomUtils

} // end of namespace NMRU
