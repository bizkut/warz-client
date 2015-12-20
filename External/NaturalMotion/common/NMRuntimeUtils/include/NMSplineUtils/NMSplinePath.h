//----------------------------------------------------------------------------------------------------------------------
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
#ifdef _MSC_VER
#pragma once
#endif
#ifndef NMRU_SPLINE_PATH_H
#define NMRU_SPLINE_PATH_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
// A piecewise curve base class.
//----------------------------------------------------------------------------------------------------------------------
class BaseCurvePath
{
public:

  /// \brief Get the number nodes (control points) on the curve.
  NM_INLINE int32_t getNumControlPoints(void) const { return m_numControlPoints; }

  /// \brief Get an exiting nodes (control point) position.
  NM_INLINE const NMP::Vector3& getControlPointPosition(int32_t index) const;

  /// \brief Set an exiting nodes (control point) position.
  /// Note baking of curve will probably be necessary.
  NM_INLINE void setControlPointPosition(int32_t index, const NMP::Vector3& value);

  /// \brief Get the number of segments that the curve is divided in to.
  NM_INLINE int32_t getNumSegments(void) const { return m_numSegments; }

  /// \brief Get the number of samples of t on an arc segment.
  NM_INLINE int32_t getSegmentNumTSamples(int32_t segmentIndex) const;

  /// \brief Get the arc length of a segment.
  NM_INLINE float getSegmentArcLength(int32_t segmentIndex) const;
  NM_INLINE float getTotalArcLength() const {return m_totalArcLength;}
  
  /// \brief Get the arc length at which we have stored samples of t.
  NM_INLINE float getTSampleDistance() const { return m_tSampleDistance; }

  /// \brief How many samples of t are there along the curve arc length.
  NM_INLINE uint32_t getNumSamplesOfT() const { return m_numTSamples; }

  /// \brief Get a specific t sample value.
  NM_INLINE float getTSample(uint32_t index) { NMP_ASSERT(index < m_numTSamples); return m_tSamples[index]; }

  /// \brief Wipe down contents ready for reuse.
  void clear();

protected:
  BaseCurvePath();
  ~BaseCurvePath();

  /// \brief Calculate the t interpolation value from a distance value along the arc length.
  float calcSectionTFromWholeArcDistance(float dist, int32_t& segmentIndex);

  static const uint32_t sm_maxNumControlPoints = 100;
  NMP::Vector3          m_controlPointPositions[sm_maxNumControlPoints];

  int32_t               m_numControlPoints;           ///< Number of control points.
  int32_t               m_numSegments;                ///< Number of curve segments = number of control points - 1.
  
  float                 m_totalArcLength;                          ///< Estimated length for whole curve.
  float                 m_segmentLengths[sm_maxNumControlPoints];  ///< Estimated length for each curve segments.
  float                 m_segmentTLengths[sm_maxNumControlPoints]; ///< Length of each segment as a fraction of the whole length.
  
  /// An array of cached values of the parametric variable t at fixed arc length sampling distances.
  /// This allows for fairly accurate linear interpolation along the curves arc length.
  static const uint32_t sm_maxNumTSamples = 1024;
  float                 m_tSamples[sm_maxNumTSamples];
  uint32_t              m_numTSamples;                ///< Total number of samples of t.
  float                 m_tSampleDistance;            ///< The arc length at which we actually have each sample of t.
                                                      ///< We calculate this so that our sample distances evenly divide the whole arc length.
  float                 m_lastSampleDistances[sm_maxNumControlPoints];///< The length of the last sample on from each segment.
  uint32_t              m_numSegmentSamples[sm_maxNumControlPoints];  ///< Num samples of t on each segment.
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NMP::Vector3& BaseCurvePath::getControlPointPosition(int32_t index) const
{
  NMP_ASSERT(index < m_numControlPoints); // "Point index is out of bounds!!"
  return m_controlPointPositions[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void BaseCurvePath::setControlPointPosition(int32_t index, const NMP::Vector3& value)
{
  NMP_ASSERT(index < m_numControlPoints); // "Point index is out of bounds!!"
  m_controlPointPositions[index] = value;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float BaseCurvePath::getSegmentArcLength(int32_t segmentIndex) const
{
  NMP_ASSERT(segmentIndex < m_numSegments);
  return m_segmentLengths[segmentIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE int32_t BaseCurvePath::getSegmentNumTSamples(int32_t segmentIndex) const
{ 
  NMP_ASSERT(segmentIndex < m_numSegments);
  return m_numSegmentSamples[segmentIndex]; 
}

//----------------------------------------------------------------------------------------------------------------------
// A piecewise curve that passes through all control points.
// C1 continuous.
// 
// Supports:
//  CatmullRomm: Allows sharp path turns at control points.
//  Rounded: Smooths path turns at control points.
//----------------------------------------------------------------------------------------------------------------------
class HermiteCurvePath : public BaseCurvePath
{
public:
  HermiteCurvePath();
  ~HermiteCurvePath();

  /// Add a node (control point) to the end of the curve.
  void addControlPoint(const NMP::Vector3& p);

  /// \brief Get an exiting nodes (control point) velocity.
  NM_INLINE const NMP::Vector3& getControlPointVelocity(int32_t index) const;

  /// \brief Set an exiting nodes (control point) velocity.
  /// Note baking of curve will probably be necessary.
  NM_INLINE void setControlPointVelocity(int32_t index, const NMP::Vector3& value);

  /// \brief Prepare the curve ready for use.
  NM_INLINE void init();

  /// \brief Wipes down the content of the curve.
  void clear(void);
  
  /// \brief Returns an interpolated point and velocity based on a parametric value over the whole series of curve segments.
  void interpolateCurveT(
    float         t,            ///< 0.0 to 1.0 representing the parametric distance along the whole curve.
    NMP::Vector3& pos);         ///< Returned position.
  void interpolateCurveT(
    float t,                    ///< 0.0 to 1.0 representing the parametric distance along the whole curve.
    NMP::Vector3& pos,          ///< Returned position.
    NMP::Vector3& vel);         ///< Returned velocity.
  void interpolateCurveT(
    float t,                    ///< 0.0 to 1.0 representing the parametric distance along the whole curve.
    NMP::Vector3& pos,          ///< Returned position.
    NMP::Vector3& vel,          ///< Returned velocity.
    NMP::Vector3& accel);       ///< Returned acceleration.

  /// \brief Returns an interpolated point based on a parametric value over a single segment.
  void interpolateSegmentT(
    int32_t       segmentIndex, ///< Control point to treat as t = 0.0. segmentIndex + 1 is treated as t = 1.0.
    float         t,            ///< 0.0 to 1.0 representing the parametric distance along the spline segment.
    NMP::Vector3& pos);         ///< Returned position.

  /// \brief Returns an interpolated point and velocity based on a parametric value over a single segment.
  void interpolateSegmentT(
    int32_t       segmentIndex, ///< Control point to treat as t = 0.0. segmentIndex + 1 is treated as t = 1.0.
    float         t,            ///< 0.0 to 1.0 representing the parametric distance along the spline segment.
    NMP::Vector3& pos,          ///< Returned position.
    NMP::Vector3& vel);         ///< Returned velocity.

  /// \brief Returns an interpolated point, velocity and acceleration based on a parametric value over a single segment.
  void interpolateSegmentT(
    int32_t       segmentIndex, ///< Control point to treat as t = 0.0. segmentIndex + 1 is treated as t = 1.0.
    float         t,            ///< 0.0 to 1.0 representing the parametric distance along the spline segment.
    NMP::Vector3& pos,          ///< Returned position.
    NMP::Vector3& vel,          ///< Returned velocity.
    NMP::Vector3& accel);       ///< Returned acceleration.

  /// \brief Recalculates the node (control point) velocities using the Catmull-Rom approach and recomputes arc lengths. 
  ///
  // tangent[i] = 0.5 * (point[i+1] - point[i-1])
  // Assumes endpoint tangents are parallel with line with neighbour.
  //
  /// Must be called after all nodes are added.
  /// If should also be called if a control point position is changed.
  /// (this is not obligatory but if not may otherwise result in an odd looking path).
  void bakeAsCatmullRomCurve(
    bool  close,              ///< Closes the spline in to a continuous loop.
                              ///< NOTE: copies the first node pos to the last node pos.
    float arcSampleInterval);

  /// \brief Recalculates the node (control point) velocities using a rounded spline approach and recomputes arc lengths. 
  ///
  /// Must be called after all nodes are added.
  /// If should also be called if a control point position is changed.
  /// (this is not obligatory but if not may otherwise result in an odd looking path).
  void bakeAsRoundedCurve(
    bool  close,              ///< Closes the spline in to a continuous loop.
                              ///< NOTE: copies the first node pos to the last node pos.
    float arcSampleInterval);
    
  /// \brief Returns an interpolated point based on a distance down the arc length of the whole spline.
  void interpolateCurveArcLength(
    float         dist,
    NMP::Vector3& pos);
  /// \brief Returns an interpolated point and velocity based on a distance down the arc length of the whole spline.
  void interpolateCurveArcLength(
    float         dist,
    NMP::Vector3& pos,
    NMP::Vector3& vel);
  void interpolateCurveArcLength(
    float               dist,
    NMP::Vector3&       pos,
    NMP::Vector3&       vel,
    float&              rateOfTurnWithArcTravel,
    const NMP::Vector3& turnAboutAxis);
    
  enum Type
  {
    TYPE_CATMULL_ROM = 0,
    TYPE_ROUNDED,
    TYPE_UNKNOWN
  };
  
protected:

  /// \brief Calculate node tangent velocities.
  void calculateCatmullRomCurveVelocities(bool close);

  /// \brief Calculate node tangent velocities.
  void calculateRoundedCurveVelocities(bool close);

  /// \brief Calculate straight line distances between nodes.
  void calculateSegmentStraightLineLengths();

  /// \brief Calculate an array of values of the parametric variable t at fixed arc length sampling distances.
  ///
  /// This allows for fairly accurate linear interpolation along the splines arc length.
  void calculateTTableAndArcLengths(float arcSampleInterval = 0.4f);
  
  /// \brief Work out which segment a specified value of t lies within.
  ///
  /// Assumes points are evenly spaced in t.
  NM_INLINE void calculateSegmentIndex(
    float    tWholeCurve,     ///< Input t value across the whole curve.
    float&   tCurveSegment,   ///< Returned calculated t value within segment.
    int32_t& segmentIndex);    ///< Returned found segment index.

  /// \brief Generate matrix necessary for easy interpolation along a curve segment in t.
  void calculateInterpolationMatrix(
    int32_t        fromNodeIndex,
    NMP::Matrix34& interpMatrix); ///< Form matrix of (point1, point2, tangent1, tangent2).
  
  /// \brief Returns an estimated value of t for an arc sample distance from a starting value of t.
  float estimateTForArcLengthInterval(
    int32_t segmentIndex,
    float   startT,             ///< The point to start the sample from.
    float   accuracyMultiplier, ///< The smaller this number is the more accurate the length estimation.
    float   arcLengthToSample,  ///< The arch length to try to sample.
    float&  arcLengthSampled,   ///< Arc length actually sampled. Not the same as arcLengthToSample 
                                ///<  only when this distance goes beyond the length of the spine.
    bool&   segmentEndReached);

    
  NMP::Vector3          m_controlPointVelocities[sm_maxNumControlPoints];
  float                 m_segmentStraightLineLengths[sm_maxNumControlPoints];///< Straight line length between each node.
  Type                  m_curveType;     ///< Controls how node velocities are generated and how interpolation is done.
  bool                  m_closed;        ///< Does this curve wrap around on itself.

  /// Hermite polynomial coefficient matrix for calculating curve positions.
  static NMP::Matrix34  sm_hermitePosCoeffs;

  /// Hermite polynomial coefficient matrix for calculating curve velocities (1st derivative of sm_hermitePosCoeffs).
  static NMP::Matrix34  sm_hermiteVelCoeffs;

  /// Hermite polynomial coefficient matrix for calculating curve accelerations (2st derivative of sm_hermitePosCoeffs).
  static NMP::Matrix34  sm_hermiteAccelCoeffs;
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void HermiteCurvePath::init()
{
  clear();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void HermiteCurvePath::calculateSegmentIndex(
  float    tWholeCurve,
  float&   tCurveSegment,
  int32_t& segmentIndex)
{
  segmentIndex = 0;
  float tSum = m_segmentTLengths[segmentIndex];

  // Find the segment index.
  while (tWholeCurve > tSum)
  {
    ++segmentIndex;
    NMP_ASSERT(segmentIndex < m_numSegments);
    if (segmentIndex < (m_numSegments - 1))
      tSum += m_segmentTLengths[segmentIndex];
    else
      tSum = 1.0f;
  }

  // Estimate the segment t value.
  tCurveSegment = m_segmentTLengths[segmentIndex] - (tSum - tWholeCurve);
  NMP_ASSERT(tCurveSegment >= 0.0f);
  tCurveSegment /= m_segmentTLengths[segmentIndex];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NMP::Vector3& HermiteCurvePath::getControlPointVelocity(int32_t index) const
{
  NMP_ASSERT(index < m_numControlPoints); // "Tangent index is out of bounds!!"
  return m_controlPointVelocities[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void HermiteCurvePath::setControlPointVelocity(int32_t index, const NMP::Vector3& value)
{
  NMP_ASSERT(index < m_numControlPoints); // "Tangent index is out of bounds!!"
  m_controlPointVelocities[index] = value;
}

//----------------------------------------------------------------------------------------------------------------------
// Curve does not pass through control points.
// C2 continuous.
//----------------------------------------------------------------------------------------------------------------------
class BSplinePath : public BaseCurvePath
{
public:
  BSplinePath();
  ~BSplinePath();

  /// \brief Prepare the curve ready for use.
  void init();

  /// \brief Wipes down the content of the curve.
  void clear(void);

  /// \brief Add a control point to the end of the curve.
  void addControlPoint(const NMP::Vector3& p);

  /// \brief Finalise the spline.
  ///
  /// Computes arc lengths, takes t samples etc.
  void bake(float arcSampleInterval);

  /// \brief Returns an interpolated point based on a parametric value over a single segment.
  void interpolateSegmentT(
    int32_t       segmentIndex, ///< Control point to treat as t = 0.0. segmentIndex + 1 is treated as t = 1.0.
    float         t,            ///< 0.0 to 1.0 representing the parametric distance along the spline segment.
    NMP::Vector3& pos);         ///< Returned position.

  /// \brief Returns an interpolated point and velocity based on a parametric value over a single segment.
  void interpolateSegmentT(
    int32_t       segmentIndex, ///< Control point to treat as t = 0.0. segmentIndex + 1 is treated as t = 1.0.
    float         t,            ///< 0.0 to 1.0 representing the parametric distance along the spline segment.
    NMP::Vector3& pos,          ///< Returned position.
    NMP::Vector3& vel);         ///< Returned velocity.

  /// \brief Returns an interpolated point based on a distance down the arc length of the whole spline.
  void interpolateCurveArcLength(
    float         dist,
    NMP::Vector3& pos);
  /// \brief Returns an interpolated point and velocity based on a distance down the arc length of the whole spline.
  void interpolateCurveArcLength(
    float         dist,
    NMP::Vector3& pos,
    NMP::Vector3& vel);

protected:

  /// \brief Calculate an array of values of the parametric variable t at fixed arc length sampling distances.
  ///
  /// This allows for fairly accurate linear interpolation along the splines arc length.
  void calculateTTableAndArcLengths(float arcSampleInterval = 0.4f);

  /// \brief Returns an estimated value of t for an arc sample distance from a starting value of t.
  float estimateTForArcLengthInterval(
    int32_t segmentIndex,
    float   startT,             ///< The point to start the sample from.
    float   accuracyMultiplier, ///< The smaller this number is the more accurate the length estimation.
    float   arcLengthToSample,  ///< The arch length to try to sample.
    float&  arcLengthSampled,   ///< Arc length actually sampled. Not the same as arcLengthToSample 
                                ///<  only when this distance goes beyond the length of the spine.
    bool&   segmentEndReached);
  
  // B-spline polynomial coefficient basis matrix for calculating curve positions.
  // p(t) = 1/6 * ((-t^3 + 3t^2 - 3t +1) + (+3t^3 - 6t^2 + 4) + (-3t^3 + 3t^2 + 3t +1) + (t^3))
  static NMP::Matrix34 sm_posCoeffs;

  // B-spline polynomial coefficient basis matrix for calculating curve velocities (1st derivative of sm_posCoeffs).
  // v(t) = 1/6 * ((-3t^2 + 6t - 3) + (9t^2 - 12t) + (-9t^2 + 6t + 3) + (3t^2))
  static NMP::Matrix34 sm_velCoeffs;
    
  // B-spline polynomial coefficient basis matrix for calculating curve accelerations (2nd derivative of sm_posCoeffs).
  // a(t) = 1/6 * ((-6t + 6) + (18t -12) + (-18t + 6) + (6t))
  static NMP::Matrix34 sm_accelCoeffs;
};

} // namespace NMRU

//----------------------------------------------------------------------------------------------------------------------
#endif // NMRU_SPLINE_PATH_H
//----------------------------------------------------------------------------------------------------------------------
