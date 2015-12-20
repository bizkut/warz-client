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
#include "NMSplineUtils/NMSplinePath.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
BaseCurvePath::BaseCurvePath()
{
}

//----------------------------------------------------------------------------------------------------------------------
BaseCurvePath::~BaseCurvePath()
{
}

//----------------------------------------------------------------------------------------------------------------------
void BaseCurvePath::clear()
{
  m_numControlPoints = 0;
  m_numSegments = -1;
  m_totalArcLength = 0.0f;
  m_numTSamples = 0;
  m_tSampleDistance = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
float BaseCurvePath::calcSectionTFromWholeArcDistance(float dist, int32_t& segmentIndex)
{
  NMP_ASSERT(dist >= 0.0f);
  NMP_ASSERT(m_numTSamples > 0);
 
  // Default to the end of the curve if we have asked for a point beyond the total arc length.
  if (dist >= m_totalArcLength)
  {
    segmentIndex = m_numSegments - 1;
    return 1.0f;
  }

  // Find the index of the section we are in.
  float arcSum = 0.0f;
  segmentIndex = 0;
  arcSum += m_segmentLengths[segmentIndex];
  int32_t samplesTotal = m_numSegmentSamples[segmentIndex];
  while (arcSum < dist)
  {
    ++segmentIndex;
    if (segmentIndex >= m_numSegments)
    {
      --segmentIndex;
      arcSum = m_totalArcLength;
      break;
    }
    NMP_ASSERT(segmentIndex < m_numSegments);
    arcSum += m_segmentLengths[segmentIndex];
    samplesTotal += m_numSegmentSamples[segmentIndex];
  }
 
  float distInToSegment = m_segmentLengths[segmentIndex] - (arcSum - dist);
  NMP_ASSERT(distInToSegment >= 0.0f && distInToSegment <= m_segmentLengths[segmentIndex]);
  samplesTotal -= m_numSegmentSamples[segmentIndex];

  //
  int32_t tTableIndex = (int32_t) (distInToSegment / m_tSampleDistance);
  float t = 1.0f;
  float overunDistance = distInToSegment - (m_tSampleDistance * ((float) tTableIndex));
  float overrunFraction;
  if (tTableIndex < ((int32_t)m_numSegmentSamples[segmentIndex] - 1))
  {
    // We are within the initial curve section covered by regular interval arc length samples.
    overrunFraction = overunDistance / m_tSampleDistance;
  }
  else
  {
    // We are within the last arc sample section.
    overrunFraction = overunDistance / m_lastSampleDistances[segmentIndex];
  }

  NMP_ASSERT(overrunFraction >= 0.0f && overrunFraction <= 1.0f);

  tTableIndex = samplesTotal + tTableIndex;
  t = m_tSamples[tTableIndex];
  float tMinus1 = 0.0f;
  if (tTableIndex > samplesTotal)
  {
    tMinus1 = m_tSamples[tTableIndex - 1];
  }
  t = tMinus1 + ((t - tMinus1) * overrunFraction);

  return t;
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// Hermite polynomial coefficient matrix for calculating curve positions.
// p(t) = (2t^3 - 3t2 + 1)p0 + (t^3 - 2t^2 + t)m0 + (-2t^3 + 3t^2)p1 + (t^3 - t^2)m1
NMP::Matrix34 HermiteCurvePath::sm_hermitePosCoeffs(
                    NMP::Vector3( 2.0f, -2.0f,  1.0f,  1.0f),
                    NMP::Vector3(-3.0f,  3.0f, -2.0f, -1.0f),
                    NMP::Vector3( 0.0f,  0.0f,  1.0f,  0.0f),
                    NMP::Vector3( 1.0f,  0.0f,  0.0f,  0.0f));

// Hermite polynomial coefficient matrix for calculating curve velocities (1st derivative of sm_hermitePosCoeffs).
// v(t) =(6t^2 - 6t)p0 + (3t^2 - 4t)m0 + (-6t^2 + 6t)p1 + (3t^2 - 2t)m1
NMP::Matrix34 HermiteCurvePath::sm_hermiteVelCoeffs(
                    NMP::Vector3( 0.0f,  0.0f,  0.0f,  0.0f),
                    NMP::Vector3( 6.0f, -6.0f,  3.0f,  3.0f),
                    NMP::Vector3(-6.0f,  6.0f, -4.0f, -2.0f),
                    NMP::Vector3( 0.0f,  0.0f,  1.0f,  0.0f));

// Hermite polynomial coefficient matrix for calculating curve accelerations (2nd derivative of sm_hermitePosCoeffs).
NMP::Matrix34 HermiteCurvePath::sm_hermiteAccelCoeffs(
                    NMP::Vector3( 0.0f,   0.0f,  0.0f,  0.0f),
                    NMP::Vector3( 0.0f,   0.0f,  0.0f,  0.0f),
                    NMP::Vector3(12.0f, -12.0f,  6.0f,  6.0f),
                    NMP::Vector3(-6.0f,   6.0f, -4.0f, -2.0f));

//----------------------------------------------------------------------------------------------------------------------
HermiteCurvePath::HermiteCurvePath()
{
  init();
}

//----------------------------------------------------------------------------------------------------------------------
HermiteCurvePath::~HermiteCurvePath()
{
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::clear()
{
  BaseCurvePath::clear();
  m_closed = false;
  m_curveType = TYPE_CATMULL_ROM;    
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::addControlPoint(const NMP::Vector3& p)
{
  NMP_ASSERT((uint32_t)m_numControlPoints < sm_maxNumControlPoints);
  m_controlPointPositions[m_numControlPoints] = p;
  m_numControlPoints++;
  m_numSegments++;
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::interpolateCurveT(float t, NMP::Vector3& pos)
{
  int32_t segIdx;
  calculateSegmentIndex(t, t, segIdx);
  return interpolateSegmentT(segIdx, t, pos);
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::interpolateCurveT(float t, NMP::Vector3& pos, NMP::Vector3& vel)
{
  int32_t segIdx;
  calculateSegmentIndex(t, t, segIdx);  
  return interpolateSegmentT(segIdx, t, pos, vel);
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::interpolateCurveT(float t, NMP::Vector3& pos, NMP::Vector3& vel, NMP::Vector3& accel)
{
  int32_t segIdx;
  calculateSegmentIndex(t, t, segIdx);  
  return interpolateSegmentT(segIdx, t, pos, vel, accel);
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::calculateInterpolationMatrix(
  int32_t        fromNodeIndex,
  NMP::Matrix34& interpMatrix)
{
  // Scale segment velocities based on curve type.
  // MORPH-21315: Remove need for conditional, potential for separating in to 2 classes.
  NMP::Vector3 tangent1(NMP::Vector3::InitZero);
  NMP::Vector3 tangent2(NMP::Vector3::InitZero);
  switch (m_curveType)
  {
    case TYPE_CATMULL_ROM:
      tangent1 = m_controlPointVelocities[fromNodeIndex];
      tangent2 = m_controlPointVelocities[fromNodeIndex + 1];
      break;
    case TYPE_ROUNDED:
      tangent1 = (m_controlPointVelocities[fromNodeIndex] * m_segmentStraightLineLengths[fromNodeIndex]);
      tangent2 = (m_controlPointVelocities[fromNodeIndex + 1] * m_segmentStraightLineLengths[fromNodeIndex]);
      break;
    default:
      NMP_ASSERT_FAIL(); // Unknown curve type.
      break;
  }
  
  // Form matrix of (point1, point2, tangent1, tangent2).
  interpMatrix.initialise(
                  m_controlPointPositions[fromNodeIndex],
                  m_controlPointPositions[fromNodeIndex + 1],
                  tangent1,
                  tangent2);
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::interpolateSegmentT(
  int32_t       segmentIndex,
  float         t,
  NMP::Vector3& pos)
{
  NMP_ASSERT(segmentIndex < m_numControlPoints);

  if ((segmentIndex + 1) == m_numControlPoints)
  {
    // We are at the end of the last section, just return the source.
    pos = m_controlPointPositions[segmentIndex];
    return;
  }

  // Do interpolation.

  // Form a vector of powers of t.
  NMP::Vector3 powers(t * t * t, t * t, t, 1.0f);

  // Form matrix of (point1, point2, tangent1, tangent2).
  NMP::Matrix34 interpMatrix;
  calculateInterpolationMatrix(segmentIndex, interpMatrix);
    
  // Calculate position on cubic.
  // pos = powers * m_hermiteCoeffsPos * pt;
  NMP::Matrix34 working;
  working.multiply4x4(sm_hermitePosCoeffs, interpMatrix);
  working.transformVectorFull(powers, pos);
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::interpolateSegmentT(
  int32_t       segmentIndex,
  float         t,
  NMP::Vector3& pos,
  NMP::Vector3& vel)
{
  NMP_ASSERT(segmentIndex < m_numControlPoints);

  if ((segmentIndex + 1) == m_numControlPoints)
  {
    // We are at the end of the last section, just return the source.
    pos = m_controlPointPositions[segmentIndex];
    vel = m_controlPointVelocities[segmentIndex];
    return;
  }
  
  // Do interpolation.

  // Form a vector of powers of t.
  NMP::Vector3 powers(t * t * t, t * t, t, 1.0f);

  // Form matrix of (point1, point2, tangent1, tangent2).
  NMP::Matrix34 interpMatrix;
  calculateInterpolationMatrix(segmentIndex, interpMatrix);

  // Calculate position on cubic.
  // pos = powers * m_hermiteCoeffsPos * pt;
  NMP::Matrix34 working;
  working.multiply4x4(sm_hermitePosCoeffs, interpMatrix);
  working.transformVectorFull(powers, pos);
  
  // Calculate velocity on cubic.
  // vel = powers * m_hermiteCoeffsVel * pt;
  working.multiply4x4(sm_hermiteVelCoeffs, interpMatrix);
  working.transformVectorFull(powers, vel);
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::interpolateSegmentT(
  int32_t       segmentIndex,
  float         t,
  NMP::Vector3& pos,
  NMP::Vector3& vel,
  NMP::Vector3& accel)
{
  NMP_ASSERT(segmentIndex < m_numControlPoints);

  if ((segmentIndex + 1) == m_numControlPoints)
  {
    // We are at the end of the last section, just return the source.
    pos = m_controlPointPositions[segmentIndex];
    vel = m_controlPointVelocities[segmentIndex];
    return;
  }
  
  // Do interpolation.

  // Form a vector of powers of t.
  NMP::Vector3 powers(t * t * t, t * t, t, 1.0f);

  // Form matrix of (point1, point2, tangent1, tangent2).
  NMP::Matrix34 interpMatrix;
  calculateInterpolationMatrix(segmentIndex, interpMatrix);

  // Calculate position on cubic.
  // pos = powers * sm_hermitePosCoeffs * pt;
  NMP::Matrix34 working;
  working.multiply4x4(sm_hermitePosCoeffs, interpMatrix);
  working.transformVectorFull(powers, pos);
  
  // Calculate velocity on cubic.
  // vel = powers * sm_hermiteVelCoeffs * pt;
  working.multiply4x4(sm_hermiteVelCoeffs, interpMatrix);
  working.transformVectorFull(powers, vel);

  // Calculate acceleration on cubic.
  // vel = powers * sm_hermiteAccelCoeffs * pt;
  working.multiply4x4(sm_hermiteAccelCoeffs, interpMatrix);
  working.transformVectorFull(powers, accel);
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::bakeAsCatmullRomCurve(
  bool  close,
  float arcSampleInterval)
{    
  calculateCatmullRomCurveVelocities(close);
  calculateSegmentStraightLineLengths();
  m_curveType = TYPE_CATMULL_ROM;

  // Do the work needed to be able to interpolate linearly along the arc length.
  calculateTTableAndArcLengths(arcSampleInterval);
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::calculateCatmullRomCurveVelocities(bool close)
{
  NMP_ASSERT(m_numControlPoints > 1);
  m_closed = close;

  if (close)
  {
    // We have been asked to make the curve a closed loop so make the last node position the same as the first.
    NMP_ASSERT(m_numControlPoints > 2); // Must be at least 3 node positions on a closed loop curve.
    m_controlPointPositions[m_numControlPoints-1] = m_controlPointPositions[0];
  }

  for(int32_t i = 1; i < m_numControlPoints - 1; ++i)
  {
    m_controlPointVelocities[i] = 0.5 * (m_controlPointPositions[i+1] - m_controlPointPositions[i-1]);
  }

  // Calculate start and end node velocities.
  if (close) // A continuous closed loop curve.
  {
    // Use numPoints-2 since numPoints-1 is the last point and == [0].
    m_controlPointVelocities[0] = 0.5 * (m_controlPointPositions[1] - m_controlPointPositions[m_numControlPoints-2]);
    // Use same tangent as already calculated for [0]
    m_controlPointVelocities[m_numControlPoints-1] = m_controlPointVelocities[0];
  }
  else          // Not a closed loop curve.
  {
    m_controlPointVelocities[0] = 0.5 * (m_controlPointPositions[1] - m_controlPointPositions[0]);
    m_controlPointVelocities[m_numControlPoints-1] = 0.5 * (m_controlPointPositions[m_numControlPoints-1] - m_controlPointPositions[m_numControlPoints-2]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::bakeAsRoundedCurve(
  bool  close,
  float arcSampleInterval)
{
  calculateRoundedCurveVelocities(close);  
  calculateSegmentStraightLineLengths();
  m_curveType = TYPE_ROUNDED;

  // Do the work needed to be able to interpolate linearly along the arc length.
  calculateTTableAndArcLengths(arcSampleInterval);
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::calculateRoundedCurveVelocities(bool close)
{
  NMP_ASSERT(m_numControlPoints > 1);
  m_closed = close;

  if (close)
  {
    // We have been asked to make the curve a closed loop so make the last node position the same as the first.
    NMP_ASSERT(m_numControlPoints > 2); // Must be at least 3 node positions on a closed loop curve.
    m_controlPointPositions[m_numControlPoints-1] = m_controlPointPositions[0];
  }

  // Calculate remaining node velocities.
  NMP::Vector3 from;
  NMP::Vector3 to;
  for (int32_t i = 1; i < m_numControlPoints - 1; i++)
  {
    // Split the angle.
    from = m_controlPointPositions[i+1] - m_controlPointPositions[i]; 
    from.normalise();
    to = m_controlPointPositions[i-1] - m_controlPointPositions[i];
    to.normalise();
    m_controlPointVelocities[i] = from - to;
    m_controlPointVelocities[i].normalise();
  }

  // Calculate start and end node velocities.
  if (close)  // A continuous closed loop curve.
  {
    // Use numPoints-2 since numPoints-1 is the last point and == [0].
    from = m_controlPointPositions[1] - m_controlPointPositions[0]; 
    from.normalise();
    to = m_controlPointPositions[m_numControlPoints-2] - m_controlPointPositions[0];
    to.normalise();
    m_controlPointVelocities[0] = from - to;
    m_controlPointVelocities[0].normalise();
    // Use same tangent as already calculated for [0]
    m_controlPointVelocities[m_numControlPoints-1] = m_controlPointVelocities[0];
  }
  else        // Not a closed loop curve.
  {
    // Calculate start node velocity.
    NMP::Vector3 temp = m_controlPointPositions[1] - m_controlPointPositions[0];
    float mag = temp.magnitude();
    temp = 3.0f * temp / mag;
    m_controlPointVelocities[0] = (temp - m_controlPointVelocities[1]) * 0.5f;

    // Calculate end node velocity.
    temp = m_controlPointPositions[m_numControlPoints - 1] - m_controlPointPositions[m_numControlPoints - 2];
    mag = temp.magnitude();
    temp = 3.0f * temp / mag;
    m_controlPointVelocities[m_numControlPoints - 1] = (temp - m_controlPointVelocities[m_numControlPoints - 2]) * 0.5f;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::calculateSegmentStraightLineLengths()
{
  // Calculate straight line distances between nodes.
  NMP::Vector3 dist;
  for (int32_t i = 0; i < m_numControlPoints - 1; i++)
  {
    dist = m_controlPointPositions[i+1] - m_controlPointPositions[i];
    m_segmentStraightLineLengths[i] = dist.magnitude();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::calculateTTableAndArcLengths(float requiredArcSampleInterval)
{
  m_totalArcLength = 0.0f;
  m_tSampleDistance = requiredArcSampleInterval;
  NMP_ASSERT(m_tSampleDistance > 0.0f);
  
  float intervalSampled = 0.0f;
  float t;
  m_numTSamples = 0;
  bool segmentEndReached;
  
  for(int32_t i = 0; i < m_numSegments; ++i)
  {
    m_segmentLengths[i] = 0.0f;
    m_numSegmentSamples[i] = 0;
  }

  // Calculate:
  //    SegmentArcLengths,
  //    TotalArcLengths,
  //    t samples at fixed arc length intervals across each section.
  for(int32_t i = 0; i < m_numSegments; ++i)
  {
    t = 0.0f;
    while (t < 1.0f)
    {
      NMP_ASSERT(m_numTSamples < sm_maxNumTSamples);
      t = estimateTForArcLengthInterval(i, t, 0.000001f, requiredArcSampleInterval, intervalSampled, segmentEndReached);
      m_segmentLengths[i] += intervalSampled;
      m_totalArcLength += intervalSampled;

      m_lastSampleDistances[i] = intervalSampled;
      m_tSamples[m_numTSamples] = t;
      ++m_numTSamples;
      ++(m_numSegmentSamples[i]);
    }
    NMP_ASSERT(m_tSamples[m_numTSamples - 1] == 1.0f);;
  }

  // Calculate segment T fractions of the whole curve.
  for(int32_t i = 0; i < m_numSegments; ++i)
  {
    m_segmentTLengths[i] = m_segmentLengths[i] / m_totalArcLength;
  }
}

//----------------------------------------------------------------------------------------------------------------------
float HermiteCurvePath::estimateTForArcLengthInterval(
  int32_t segmentIndex,
  float   startT,
  float   accuracyMultiplier,
  float   arcLengthToSample,
  float&  arcLengthSampled,
  bool&   segmentEndReached)
{
  NMP::Vector3 startPos;
  NMP::Vector3 velocity;
  NMP::Vector3 endPos;
  float tDelta = 0.0f;
  float distanceEstimate = 0.0f;
  float distanceDelta = 0.0f;
  arcLengthSampled = arcLengthToSample;

  float t = startT;
  float fractionOverStep;

  segmentEndReached = false;

  interpolateSegmentT(segmentIndex, t, startPos, velocity);
  distanceEstimate = 0.0f;
  tDelta = velocity.magnitude() * accuracyMultiplier;
  t += tDelta;

  while((distanceEstimate < arcLengthToSample) && (t <= 1.0f))
  {
    interpolateSegmentT(segmentIndex, t, endPos, velocity);
    distanceDelta = (endPos - startPos).magnitude();
    distanceEstimate += distanceDelta;
    startPos = endPos;
    tDelta = velocity.magnitude() * accuracyMultiplier;
    t += tDelta;
  }

  if (distanceEstimate > m_tSampleDistance)
  {
    fractionOverStep = (distanceEstimate - m_tSampleDistance) / distanceDelta;
    t -= (tDelta * fractionOverStep);
    NMP_ASSERT(t > 0.0f);
  }

  if (t > 1.0f)
  {
    t = 1.0f;
    arcLengthSampled = distanceEstimate;
    segmentEndReached = true;
  }

  return t;
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::interpolateCurveArcLength(
  float         dist,
  NMP::Vector3& pos)
{
  if (m_closed)
    dist = fmodf(dist, m_totalArcLength); // Allow wrapping as the path is closed.
  int32_t segmentIndex;
  float t = calcSectionTFromWholeArcDistance(dist, segmentIndex);
  return interpolateSegmentT(segmentIndex, t, pos);
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::interpolateCurveArcLength(
  float         dist,
  NMP::Vector3& pos,
  NMP::Vector3& vel)
{
  if (m_closed)
    dist = fmodf(dist, m_totalArcLength); // Allow wrapping as the path is closed.
  int32_t segmentIndex;
  float t = calcSectionTFromWholeArcDistance(dist, segmentIndex);
  return interpolateSegmentT(segmentIndex, t, pos, vel);
}

//----------------------------------------------------------------------------------------------------------------------
void HermiteCurvePath::interpolateCurveArcLength(
  float               dist,
  NMP::Vector3&       pos,
  NMP::Vector3&       vel,
  float&              rateOfTurnWithArcTravel,
  const NMP::Vector3& NMP_UNUSED(turnAboutAxis))
{
  if (m_closed)
    dist = fmodf(dist, m_totalArcLength); // Allow wrapping as the path is closed.

  int32_t segmentIndex;
  float tFrom = calcSectionTFromWholeArcDistance(dist, segmentIndex);
  interpolateSegmentT(segmentIndex, tFrom, pos, vel);

  NMP::Vector3 accel;
  NMP::Vector3 rateOfChangeOfVelocityWithDistance;
  interpolateSegmentT(segmentIndex, tFrom, pos, vel, accel);
  rateOfChangeOfVelocityWithDistance = accel / vel.magnitude();

  NMP::Vector3 velCrossRate;
  velCrossRate.cross(vel, rateOfChangeOfVelocityWithDistance);
  NMP::Vector3 rate = 1/(vel.magnitude()*vel.magnitude()) * velCrossRate;

  rateOfTurnWithArcTravel = rate.y;

#if 0
  // MORPH-21315.  At the moment this function assumes that the 2 velocity vectors are on
  // the XZ plane, also need to properly calculate the rotation amount about the turnAboutAxis.

  float tTo;
  NMP::Vector3 posTo;
  NMP::Vector3 velTo;
  NMP::Vector3 posFrom;
  NMP::Vector3 velFrom;

  float distDelta = 0.5f;
  NMP_ASSERT(m_curveTotalArcLength > distDelta);
  if(dist < m_curveTotalArcLength - distDelta)  
  {
    tTo = calculateTFromArcDistance(dist + distDelta, segmentIndex);
    interpolateSegmentT(segmentIndex, tTo, posTo, velTo);
    posFrom = pos;
    velFrom = vel;
  }
  else
  {
    tTo = tFrom;
    posTo = pos;
    velTo = vel;
    tFrom = calculateTFromArcDistance(dist - distDelta, segmentIndex);
    interpolateSegmentT(segmentIndex, tFrom, posFrom, velFrom);
  }
  velFrom.normalise();
  velTo.normalise();

  float angleDelta = NMP::clampValue(velFrom.dot(velTo), -1.0f, 1.0f);
  angleDelta = acosf(angleDelta);
  rateOfTurnWithArcTravel = angleDelta / distDelta;

  NMP::Vector3 crossVec;
  crossVec.cross(velFrom, velTo);
  if(crossVec.y < 0.0)
    rateOfTurnWithArcTravel = -rateOfTurnWithArcTravel;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
// BSplinePath functions.
//----------------------------------------------------------------------------------------------------------------------
// B-spline polynomial coefficient basis matrix for calculating curve positions.
// p(t) = 1/6 * ((-t^3 + 3t^2 - 3t +1) + (+3t^3 - 6t^2 + 4) + (-3t^3 + 3t^2 + 3t +1) + (t^3))
NMP::Matrix34 BSplinePath::sm_posCoeffs(
                    NMP::Vector3(-1.0f/6.0f,       0.5f,     -0.5f, 1.0f/6.0f),
                    NMP::Vector3(      0.5f,      -1.0f,      0.5f,      0.0f),
                    NMP::Vector3(     -0.5f,       0.0f,      0.5f,      0.0f),
                    NMP::Vector3( 1.0f/6.0f,  4.0f/6.0f, 1.0f/6.0f,      0.0f));

                   
// B-spline polynomial coefficient basis matrix for calculating curve velocities (1st derivative of sm_posCoeffs).
// v(t) = 1/6 * ((-3t^2 + 6t - 3) + (9t^2 - 12t) + (-9t^2 + 6t + 3) + (3t^2))
NMP::Matrix34 BSplinePath::sm_velCoeffs(
                    NMP::Vector3( 0.0f, -0.5f,  1.0f, -0.5f),
                    NMP::Vector3( 0.0f,  1.5f, -2.0f,  0.0f),
                    NMP::Vector3( 0.0f, -1.5f,  1.0f,  0.5f),
                    NMP::Vector3( 0.0f,  0.5f,  0.0f,  0.0f));

// B-spline polynomial coefficient basis matrix for calculating curve accelerations (2nd derivative of sm_posCoeffs).
// a(t) = 1/6 * ((-6t + 6) + (18t -12) + (-18t + 6) + (6t))
NMP::Matrix34 BSplinePath::sm_accelCoeffs(
                    NMP::Vector3( 0.0f, 0.0f,  -6.0f,  6.0f),
                    NMP::Vector3( 0.0f, 0.0f,  18.0f, 12.0f),
                    NMP::Vector3( 0.0f, 0.0f, -18.0f,  6.0f),
                    NMP::Vector3( 0.0f, 0.0f,   6.0f,  0.0f));
                    
//----------------------------------------------------------------------------------------------------------------------
BSplinePath::BSplinePath()
{
  init();
}

//----------------------------------------------------------------------------------------------------------------------
BSplinePath::~BSplinePath()
{
}

//----------------------------------------------------------------------------------------------------------------------
void BSplinePath::init()
{
  clear();
}

//----------------------------------------------------------------------------------------------------------------------
void BSplinePath::clear()
{
  BaseCurvePath::clear();
}

//----------------------------------------------------------------------------------------------------------------------
void BSplinePath::addControlPoint(const NMP::Vector3& p)
{
  m_controlPointPositions[m_numControlPoints] = p;
  ++m_numControlPoints;
  m_numSegments = m_numControlPoints - 3;
}

//----------------------------------------------------------------------------------------------------------------------
void BSplinePath::interpolateSegmentT(
    int32_t       segmentIndex,
    float         t,
    NMP::Vector3& pos)
{
  // Do interpolation.
  NMP_ASSERT(m_numSegments > 0);
  NMP_ASSERT(segmentIndex >= 0 && segmentIndex <= m_numSegments);

  // Form a vector of powers of t.
  NMP::Vector3 powers(t * t * t, t * t, t, 1.0f);

  // Form matrix of (point i-1, point i, point i+1, point i+2).
  NMP::Matrix34 interpMatrix;
  interpMatrix.initialise(
    m_controlPointPositions[segmentIndex],
    m_controlPointPositions[segmentIndex + 1],
    m_controlPointPositions[segmentIndex + 2],
    m_controlPointPositions[segmentIndex + 3]);

  // Calculate position on cubic.
  // pos = powers * sm_posCoeffs * pt;
  NMP::Matrix34 working;
  working.multiply4x4(sm_posCoeffs, interpMatrix);
  working.transformVectorFull(powers, pos);
}

//----------------------------------------------------------------------------------------------------------------------
void BSplinePath::interpolateSegmentT(
  int32_t       segmentIndex,
  float         t,
  NMP::Vector3& pos,
  NMP::Vector3& vel)
{
  // Do interpolation.
  NMP_ASSERT(m_numSegments > 0);
  NMP_ASSERT(segmentIndex >= 0 && segmentIndex <= m_numSegments);

  // Form a vector of powers of t.
  NMP::Vector3 powers(t * t * t, t * t, t, 1.0f);

  // Form matrix of (point i-1, point i, point i+1, point i+2).
  NMP::Matrix34 interpMatrix;
  interpMatrix.initialise(
    m_controlPointPositions[segmentIndex],
    m_controlPointPositions[segmentIndex + 1],
    m_controlPointPositions[segmentIndex + 2],
    m_controlPointPositions[segmentIndex + 3]);

  // Calculate position on cubic.
  // pos = powers * sm_posCoeffs * pt;
  NMP::Matrix34 working;
  working.multiply4x4(sm_posCoeffs, interpMatrix);
  working.transformVectorFull(powers, pos);

  // Calculate velocity on cubic.
  // vel = powers * sm_velCoeffs * pt;
  working.multiply4x4(sm_velCoeffs, interpMatrix);
  working.transformVectorFull(powers, vel);
}

//----------------------------------------------------------------------------------------------------------------------
void BSplinePath::interpolateCurveArcLength(
  float         dist,
  NMP::Vector3& pos)
{
  int32_t segmentIndex;
  float t = calcSectionTFromWholeArcDistance(dist, segmentIndex);
  return interpolateSegmentT(segmentIndex, t, pos);
}

//----------------------------------------------------------------------------------------------------------------------
void BSplinePath::interpolateCurveArcLength(
  float         dist,
  NMP::Vector3& pos,
  NMP::Vector3& vel)
{
  int32_t segmentIndex;
  float t = calcSectionTFromWholeArcDistance(dist, segmentIndex);
  return interpolateSegmentT(segmentIndex, t, pos, vel);
}

//----------------------------------------------------------------------------------------------------------------------
void BSplinePath::bake(float arcSampleInterval)
{
  // Do the work needed to be able to interpolate linearly along the arc length.
  calculateTTableAndArcLengths(arcSampleInterval);
}

//----------------------------------------------------------------------------------------------------------------------
void BSplinePath::calculateTTableAndArcLengths(float requiredArcSampleInterval)
{
  m_totalArcLength = 0.0f;
  m_tSampleDistance = requiredArcSampleInterval;
  NMP_ASSERT(m_tSampleDistance > 0.0f);
  
  float intervalSampled = 0.0f;
  float t;
  m_numTSamples = 0;
  bool segmentEndReached;
  
  for(int32_t i = 0; i < m_numSegments; ++i)
  {
    m_segmentLengths[i] = 0.0f;
    m_numSegmentSamples[i] = 0;
  }

  // Calculate:
  //    SegmentArcLengths,
  //    TotalArcLengths,
  //    t samples at fixed arc length intervals across each section.
  for(int32_t i = 0; i < m_numSegments; ++i)
  {
    t = 0.0f;
    while (t < 1.0f)
    {
      NMP_ASSERT(m_numTSamples < sm_maxNumTSamples);
      t = estimateTForArcLengthInterval(i, t, 0.000001f, requiredArcSampleInterval, intervalSampled, segmentEndReached);
      m_segmentLengths[i] += intervalSampled;
      m_totalArcLength += intervalSampled;

      m_lastSampleDistances[i] = intervalSampled;
      m_tSamples[m_numTSamples] = t;
      ++m_numTSamples;
      ++(m_numSegmentSamples[i]);
    }
    NMP_ASSERT(m_tSamples[m_numTSamples - 1] == 1.0f);;
  }

  // Calculate segment T fractions of the whole curve.
  for(int32_t i = 0; i < m_numSegments; ++i)
  {
    m_segmentTLengths[i] = m_segmentLengths[i] / m_totalArcLength;
  }
}

//----------------------------------------------------------------------------------------------------------------------
float BSplinePath::estimateTForArcLengthInterval(
  int32_t segmentIndex,
  float   startT,
  float   accuracyMultiplier,
  float   arcLengthToSample,
  float&  arcLengthSampled,
  bool&   segmentEndReached)
{
  NMP::Vector3 startPos;
  NMP::Vector3 velocity;
  NMP::Vector3 endPos;
  float tDelta = 0.0f;
  float distanceEstimate = 0.0f;
  float distanceDelta = 0.0f;
  arcLengthSampled = arcLengthToSample;

  float t = startT;
  float fractionOverStep;

  segmentEndReached = false;

  interpolateSegmentT(segmentIndex, t, startPos, velocity);
  distanceEstimate = 0.0f;
  float mag = velocity.magnitude();
  tDelta = mag * accuracyMultiplier;
  t += tDelta;

  while((distanceEstimate < arcLengthToSample) && (t <= 1.0f))
  {
    interpolateSegmentT(segmentIndex, t, endPos, velocity);
    distanceDelta = (endPos - startPos).magnitude();
    if (distanceDelta < 0.00000000000001f)
    {
      t = 1.0f;
      break;
    }
    distanceEstimate += distanceDelta;
    startPos = endPos;
    tDelta = velocity.magnitude() * accuracyMultiplier;
    t += tDelta;
  }

  if (distanceEstimate > m_tSampleDistance)
  {
    fractionOverStep = (distanceEstimate - m_tSampleDistance) / distanceDelta;
    t -= (tDelta * fractionOverStep);
    NMP_ASSERT(t > 0.0f);
  }

  if (t > 1.0f)
  {
    t = 1.0f;
    arcLengthSampled = distanceEstimate;
    segmentEndReached = true;
  }

  return t;
}

} // namespace NMRU

//----------------------------------------------------------------------------------------------------------------------
