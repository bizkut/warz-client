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
#ifndef NM_QUAT_SPLINE_H
#define NM_QUAT_SPLINE_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMVector3.h"
#include "NMNumerics/NMSimpleKnotVector.h"
#include <iostream>
#include <fstream>

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Quaternion spline curve. The form of the quaternion spline is a
/// poly-Hermite spline in the cumulative basis form as outlined in the paper:
///
/// @article{ kim95general,
///   author = "Myoung-Jun Kim and Myung-Soo Kim and Sung Yong Shin",
///   title = "A General Construction Scheme for Unit Quaternion Curves
///           with Simple High Order Derivatives",
///   journal = "Computer Graphics",
///   volume = "29",
///   number = "{Annual Conference Series}",
///   pages = "369--376",
///   year = "1995" }
///
/// QUATERNION SPLINE REPRESENTATION:
///
/// Bezier form of the cubic quaternion spline:
/// q(t) = q_0 * qexp(w_1 * C_1(t)) * qexp(w_2 * C_2(t)) * qexp(w_3 * C_3(t))
///
/// q_0 is the quaternion keyframe for the curve span
///
/// w_1, w_2, w_3 are the angular velocity 3-vectors (In frame of q_0)
///
/// C_1(t), C_2(t), C_3(t) are the cumulative basis functions
///
/// Hermite Quaternion Spline:
/// Interpolates between two quaternion keyframes q_a, q_b, with end angular
/// velocities w_a, w_b. The Bezier coefficients corresponding to the Hermite
/// form can be recovered.
/// w_1 = w_a / 3
/// w_2 = qlog( (q_a * qexp(w_a / 3))^{-1} * (q_b * qexp(w_b / 3)^{-1}) )
/// w_3 = w_b / 3
///
/// The Hermite representation only needs to store [q_a, w_a, w_b, q_b, ...]
/// However it is computationally more efficient to store [q_a, w_1, w_3, q_b, ...]
/// to avoid the unnecessary divisions.
///
/// Note that we can assign arbitrarily large angular velocities w_1, w_3 at
/// the curve end points and at the mid point w_2. The exponential mapping
/// from the log space to the quaternion space is not one to one:
/// q = qexp( w + (k*pi) * normalise(w) ), since any integer k generates the
/// same quaternion q. This gives us an extra degree of freedom when computing
/// the mid angular velocity vector w_2 to choose the number of revolutions
/// the curve turns through while maintaining the endpoint keyframe and angular
/// velocity constraints [q_a, w_1, w_3, q_b]. In practice this additional spin 
/// factor is never used, and the curve is partitioned into more spans instead.
///
///
/// QUATERNION SPLINE DATA STORAGE
///
/// A quaternion curve is represented piecewise by breaking the curve at
/// specific knot sites. The quaternion keyframes {q_a, q_b, ... q_n} occur at
/// the knot values and the shape of the curve spans between these keyframes
/// is determined by each of the [w_1, w_3] angular velocity vectors.
class QuatSpline : public SimpleKnotVector
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to set a quaternion spline or preallocate memory
  /// for a quaternion spline
  //---------------------------------------------------------------------
  //@{

  /// \brief Constructor to set the quaternion spline from the specified
  /// Hermite coefficients.
  QuatSpline(
    uint32_t       numKnots,
    const float*   knots,
    const Quat*    keys,
    const Vector3* velsA,
    const Vector3* velsB);

  /// \brief Constructor to set the single span quaternion curve that represents
  /// the spherical linear interpolation (SLERP) between quaternion keyframes
  /// [qa, qb] over the knot range [ka, kb]
  QuatSpline(
    float       ka,
    float       kb,
    const Quat& qa,
    const Quat& qb);

  /// \brief Constructor to preallocate the memory for a quaternion spline
  QuatSpline(uint32_t maxNumKnots);

  /// \brief Constructor to set the quaternion spline by copy
  QuatSpline(const QuatSpline& qsp);

  // Destructor
  ~QuatSpline();
  //@}

  //---------------------------------------------------------------------
  /// \name   Quaternion spline keyframe functions
  /// \brief  Functions to get and set the quaternion spline keyframes
  //---------------------------------------------------------------------
  //@{
  const Quat* getKeys() const { return m_keys; }
  Quat getKey(uint32_t i) const;
  void setKey(uint32_t i, const Quat& q);
  //@}

  //---------------------------------------------------------------------
  /// \name   Quaternion spline angular velocity functions
  /// \brief  Functions to get and set the start and end angular velocity vectors
  //---------------------------------------------------------------------
  //@{
  const Vector3* getVelsA() const { return m_velsA; }
  Vector3 getVelA(uint32_t i) const;
  void setVelA(uint32_t i, const Vector3& wa);

  const Vector3* getVelsB() const { return m_velsB; }
  Vector3 getVelB(uint32_t i) const;
  void setVelB(uint32_t i, const Vector3& wb);
  //@}

  //---------------------------------------------------------------------
  /// \name   Quaternion spline curve construction
  /// \brief  Functions to build quaternion curves
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to set the quaternion keys and angular velocity
  /// vectors to represent the non-moving identity frame. The knot vector
  /// is not modified.
  void identity();

  /// \brief Function to set the quaternion spline from the specified
  /// Hermite coefficients.
  void set(
    uint32_t       numKnots,
    const float*   knots,
    const Quat*    keys,
    const Vector3* velsA,
    const Vector3* velsB);

  /// \brief Function to set the single span quaternion curve that represents
  /// the spherical linear interpolation (SLERP) between quaternion keyframes
  /// [qa, qb] over the knot range [ka, kb]
  void set(
    float       ka,
    float       kb,
    const Quat& qa,
    const Quat& qb);

  /// \brief Function to set the quaternion spline that represents a
  /// single span quaternion curve in the Hermite form
  void set(
    float          ka,
    float          kb,
    const Quat&    qa,
    const Vector3& wa,
    const Vector3& wb,
    const Quat&    qb);

  /// \brief Function to initialise the quaternion curve with a single knot
  /// and keyframe. Further curve spans are built by appending the Hermite
  /// coefficients of the span.
  void begin(float t, const Quat& qa);

  /// \brief Function to append the Hermite coefficients of a new curve span.
  void append(float t, const Vector3& wa, const Vector3& wb, const Quat& qb);

  /// \brief Function to insert an additional knot into the curve at t that
  /// preserves the shape and C2 continuity of the quaternion curve. The
  /// function returns false if t lies outside the knot range or already lies
  /// at a knot site.
  bool insertKnot(float t);

  /// \brief Function to remove a knot from the curve. This function
  /// removes the knot by preserving the continuity of the new curve span with
  /// the adjoining segments. This does not necessarily mean that the new curve
  /// span interpolates the removed quaternion key or approximate the old shape.
  /// The new span will differ most if the curve continuity at the specified
  /// knot site is not C2 continuous.
  bool removeKnot(float t);
  bool removeKnotAtIndex(uint32_t indx);

  /// \brief Function to copy the quaternion curve. The required amount of
  /// memory must have already been preallocated to store the copy.
  QuatSpline& operator=(const QuatSpline& qsp);

  /// \brief Function to extract the desired curve sub-spans from the input
  /// quaternion spline between the range [ka : kb]. This is achieved by adding
  /// knots at ka and kb and extracting the required curve segments. The
  /// function returns false if the sub-span range is not completely within the
  /// knot range of the input spline curve.
  bool subSpan(const QuatSpline& qsp, float ka, float kb);

  /// \brief Function swaps the contents of the two quaternion splines.
  void swap(QuatSpline& qsp);
  //@}

  //---------------------------------------------------------------------
  /// \name   Evaluation
  /// \brief  Functions to evaluate a quaternion spline and its derivatives
  //---------------------------------------------------------------------
  //@{

  /// \brief Function evaluates the quaternion spline at t
  Quat evaluate(float t) const;

  /// \brief Function computes the first derivative of the quaternion
  /// spline at t
  Quat derivative1(float t) const;

  /// \brief Function computes the second derivative of the quaternion
  /// spline at t
  Quat derivative2(float t) const;

  /// \brief Function to compute the tangential component of angular acceleration
  /// of the quaternion spline at t
  Quat tangentialAcceleration(float t) const;
  //@}

protected:
  //---------------------------------------------------------------------
  /// \brief  Function to convert the set of Hermite coefficients corresponding
  /// to the curve span on which t lies to the Bezier form.
  int32_t getSpan(
    float    t,
    Quat&    q0,
    Vector3& w1,
    Vector3& w2,
    Vector3& w3,
    float&   ka,
    float&   kb) const;

protected:
  Quat*    m_keys;          ///< The array of quaternion keyframes
  Vector3* m_velsA;         ///< The array of curve span start angular velocities
  Vector3* m_velsB;         ///< The array of curve span end angular velocities
};

/// \brief Function to write the quaternion spline data to the output stream
std::ostream& operator<<(std::ostream& os, const QuatSpline& qsp);

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_QUAT_SPLINE_H
