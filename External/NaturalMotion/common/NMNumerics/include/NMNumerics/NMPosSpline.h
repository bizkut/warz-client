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
#ifndef NM_POS_SPLINE_H
#define NM_POS_SPLINE_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector3.h"
#include "NMNumerics/NMSimpleKnotVector.h"
#include <iostream>
#include <fstream>

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Cubic Bezier position spline curve.
class PosSpline : public SimpleKnotVector
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to set a position spline or preallocate memory
  /// for a position spline
  //---------------------------------------------------------------------
  //@{

  /// \brief Constructor to set the position spline from the specified
  /// control points.
  PosSpline(
    uint32_t       numKnots,
    const float*   knots,
    const Vector3* keys,
    const Vector3* tangentsA,
    const Vector3* tangentsB);

  /// \brief Constructor to set the single span position spline that represents
  /// the cubic interpolation over the knot range [ka, kb] given by the
  /// four control points [p0, p1, p2, p3]. The curve interpolates the start
  /// and end control point p0, p3 exactly.
  PosSpline(
    float          ka,
    float          kb,
    const Vector3& p0,
    const Vector3& p1,
    const Vector3& p2,
    const Vector3& p3);

  /// \brief Constructor to preallocate the memory for a position spline
  PosSpline(uint32_t maxNumKnots);

  /// \brief Constructor to set the position spline by copy
  PosSpline(const PosSpline& psp);

  // Destructor
  ~PosSpline();
  //@}

  //---------------------------------------------------------------------
  /// \name   Position spline keyframe functions
  /// \brief  Functions to get and set the position spline keyframes
  //---------------------------------------------------------------------
  //@{
  const Vector3* getKeys() const { return m_keys; }
  Vector3 getKey(uint32_t i) const;
  void setKey(uint32_t i, const Vector3& key);
  //@}

  //---------------------------------------------------------------------
  /// \name   Position spline tangent control point functions
  /// \brief  Functions to get and set the start and end tangent control points
  //---------------------------------------------------------------------
  //@{
  const Vector3* getTangentsA() const { return m_tangentsA; }
  Vector3 getTangentA(uint32_t i) const;
  void setTangentA(uint32_t i, const Vector3& tangentA);

  const Vector3* getTangentsB() const { return m_tangentsB; }
  Vector3 getTangentB(uint32_t i) const;
  void setTangentB(uint32_t i, const Vector3& tangentB);
  //@}

  //---------------------------------------------------------------------
  /// \name   Position spline curve construction
  /// \brief  Functions to build position curves
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to zero the position keys and tangent control points.
  /// The knot vector is not modified.
  void setToZero();

  /// \brief Function to set the position spline from the specified
  /// control points.
  void set(
    uint32_t       numKnots,
    const float*   knots,
    const Vector3* keys,
    const Vector3* tangentsA,
    const Vector3* tangentsB);

  /// \brief Function to set the single span position curve that represents
  /// the linear interpolation over the knot range [ka, kb] given by the
  /// two control points [p0, p1].
  void set(
    float          ka,
    float          kb,
    const Vector3& p0,
    const Vector3& p1);

  /// \brief Function to set the single span position curve that represents
  /// the quadratic interpolation over the knot range [ka, kb] given by the
  /// three control points [p0, p1, p2]. The curve interpolates the start
  /// and end control point p0, p2 exactly.
  void set(
    float          ka,
    float          kb,
    const Vector3& p0,
    const Vector3& p1,
    const Vector3& p2);

  /// \brief Function to set the single span position spline that represents
  /// the cubic interpolation over the knot range [ka, kb] given by the
  /// four control points [p0, p1, p2, p3]. The curve interpolates the start
  /// and end control point p0, p3 exactly.
  void set(
    float          ka,
    float          kb,
    const Vector3& p0,
    const Vector3& p1,
    const Vector3& p2,
    const Vector3& p3);

  /// \brief Function to initialise the position curve with a single knot
  /// and keyframe. Further curve spans are built by appending the control
  /// points of the span.
  void begin(float t, const Vector3& p0);

  /// \brief Function to append the control points of a new curve span.
  void append(float t, const Vector3& p1, const Vector3& p2, const Vector3& p3);

  /// \brief Function to insert an additional knot into the curve at t that
  /// preserves the shape and C2 continuity of the position curve. The
  /// function returns false if t lies outside the knot range or already lies
  /// at a knot site.
  bool insertKnot(float t);

  /// \brief Function to remove a knot from the curve. This function
  /// removes the knot by preserving the continuity of the new curve span with
  /// the adjoining segments. This does not necessarily mean that the new curve
  /// span interpolates the removed position key or approximate the old shape.
  /// The new span will differ most if the curve continuity at the specified
  /// knot site is not C2 continuous.
  bool removeKnot(float t);
  bool removeKnotAtIndex(uint32_t indx);

  /// \brief Function to copy the position curve. The required amount of
  /// memory must have already been preallocated to store the copy.
  PosSpline& operator=(const PosSpline& psp);

  /// \brief Function to extract the desired curve sub-spans from the input
  /// position spline between the range [ka : kb]. This is achieved by adding
  /// knots at ka and kb and extracting the required curve segments. The
  /// function returns false if the sub-span range is not completely within the
  /// knot range of the input spline curve.
  bool subSpan(const PosSpline& psp, float ka, float kb);

  /// \brief Function swaps the contents of the two position splines.
  void swap(PosSpline& psp);
  //@}

  //---------------------------------------------------------------------
  /// \name   Evaluation
  /// \brief  Functions to evaluate a position spline and its derivatives
  //---------------------------------------------------------------------
  //@{

  /// \brief Function evaluates the position spline at t
  Vector3 evaluate(float t) const;

  /// \brief Function computes the first derivative of the position
  /// spline at t
  Vector3 derivative1(float t) const;

  /// \brief Function computes the second derivative of the position
  /// spline at t
  Vector3 derivative2(float t) const;
  //@}

protected:
  Vector3* m_keys;              ///< The array of position keyframes
  Vector3* m_tangentsA;         ///< The array of curve span start angular velocities
  Vector3* m_tangentsB;         ///< The array of curve span end angular velocities
};

/// \brief Function to write the position spline data to the output stream
std::ostream& operator<<(std::ostream& os, const PosSpline& psp);

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_POS_SPLINE_H
