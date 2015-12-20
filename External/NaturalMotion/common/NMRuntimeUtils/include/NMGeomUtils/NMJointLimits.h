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

#ifndef NMRU_NMJOINTLIMITS_H
#define NMRU_NMJOINTLIMITS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMemory.h"

#include "NMGeomUtils/NMGeomUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Provides functions for operating with joint limits
namespace JointLimits
{

/// \brief If needed this will identify different limit types
enum LimitType
{
  NMSTANDARD_JOINTLIMITS = 0,
  NUM_JOINTLIMITS_TYPES
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Joint limits and joint limit data manipulation.
struct Params
{

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Construct
  /// @{
  Params();
  Params(LimitType typeIn, const NMP::Quat& frameIn, const NMP::Vector3& lowerIn, const NMP::Vector3& upperIn);
  /// \brief Copy.
  Params(const Params& params);
  /// \brief Create a scaled limit.
  Params(const Params& params, float factor);
  /// @}

  /// \brief Set from twist and lean angles in radians (as opposed to twist/swing angles in tan-quarter-radians)
  void setTwistLeanLimits(float twistMin, float twistMax, float lean1, float lean2);

  /// \brief Set as simple twist-only limit that allows only clockwise rotation around a hinge axis
  /// from a zero position related to when the parent and child bone are parallel.
  /// \param parentBone and \a childBone are the local bone translations for this joint and its child.
  /// \param hingeAxis The rotation axis in the frame of the joint's parent.
  void setAsDirectedHinge(const NMP::Vector3& parentBone, const NMP::Vector3& childBone, const NMP::Vector3& hingeAxis, float zeroHingeAngle);

  /// \brief Get twist lower limit in radians.
  float getTwistMin() const;
  /// \brief Get twist upper limit in radians.
  float getTwistMax() const;
  /// \brief Get swing 1 limit in radians.
  float getSwing1Limit() const;
  /// \brief Get swing 2 limit in radians.
  float getSwing2Limit() const;
  /// \brief Get lean 1 limit in radians.
  float getLean1Limit() const;
  /// \brief Get lean 1 limit in radians.
  float getLean2Limit() const;

  //----------------------------------------------------------------------------------------------------------------------
  // Serialisation

  /// \brief Prepare this object for another platform.
  void endianSwap();

#ifdef NM_UNIT_TESTING
  /// \brief Test-only code for saving out params in a form that can be used to populate
  /// parameters objects.  Do not ever use in production.
  bool saveCode(char prefixString[], std::ostream& stream);
#endif

  //----------------------------------------------------------------------------------------------------------------------
  // Parameters

  /// \brief The limit frame in the same space as the joint frame is parented.
  NMP::Quat frame;

  /// \brief A (possibly unused) offset to the moving frame, i.e. what frame is limited - expressed
  /// in the joint frame, i.e. parented to the joint.
  NMP::Quat offset;

  /// \brief Three numbers specifying (in some type-dependent way) the minimum rotations.
  NMP::Vector3 lower;

  /// \brief Three numbers specifying (in some type-dependent way) the maximum rotations.
  NMP::Vector3 upper;

  /// \brief At the moment, there is only one type of joint limit representation, but this will
  /// extend.
  LimitType type;

}; // end of struct JointLimits::Params

//----------------------------------------------------------------------------------------------------------------------
/// \name Functions related to clamping joints
/// Testing and clamping functions often return a byte, the bits of which indicate how the limit
/// was violated in a type-dependent way.  For standard limits bit 0 indicates twist violation,
/// and bit 1 indicates swing violation.
/// @{

/// \brief Test violation.
char isViolated(const Params& params, const NMP::Quat& qj);

/// \brief The content of isViolated(), without any rotation into the limit frame, which
/// is assumed already done.
char isViolatedSimple(const Params& params, const NMP::Quat& q);

/// \brief Test violation on smoothed limit boundary.
/// \param smoothness 0 to 1, zero meaning no smoothing.
/// For NMSTANDARD_JOINTLIMITS, \a smoothness affects curvature of the twist limit
/// boundary w.r.t lean.  At 0 there is no smoothing and the limits are at their
/// maxima, as for \a isViolated().  At 1 the smoothing gives a limit shape that is
/// ellipsoidal in twist-swing-swing space, hence strong coupling between twist and swing.
char isViolated(const Params& params, const NMP::Quat& qj, float smoothness);

/// \brief The content of isViolated() with smoothing, without any rotation into the limit
/// frame, which is assumed already done.
char isViolatedSimple(const Params& params, const NMP::Quat& q, float smoothness);

/// \brief Put a number on 'degree' of violation, a distance from the limit in some space.
/// \param smoothness 0 to 1, zero meaning no smoothing.
/// For NMSTANDARD_JOINTLIMITS, \a smoothness affects curvature of the twist limit
/// boundary w.r.t lean.  At 0 there is no smoothing and the limits are at their
/// maxima.  At 1 the smoothing gives a limit shape that is ellipsoidal in
/// twist-swing-swing space, hence strong coupling between twist and swing.
float degreeOfViolation(const Params& params, const NMP::Quat& qj, float smoothness = 0);

/// \brief The content is degreeOfViolation(), without any rotation into the limit frame,
/// which is assumed already done.
float degreeOfViolationSimple(const Params& params, const NMP::Quat& q, float smoothness = 0);

/// \brief As for degreeOfViolationSimple() but without smoothing and returning separately the
/// \a twist and \a swingSq values representing the twist and swing squared violations.
float degreeOfViolationTwistSwingSqSimple(const Params& params, const NMP::Quat& q, float& twist, float& swingSq);

/// \brief Generic clamp.
char clamp(const Params& params, NMP::Quat& qj);

/// \brief As for clamp() except without any rotation to and from the limit frame, which is
/// assumed done.
char clampSimple(const Params& params, NMP::Quat& q);

/// \brief Generic clamp onto smoothed limit boundary.
/// \param smoothness 0 to 1, zero meaning no smoothing.
/// For NMSTANDARD_JOINTLIMITS, \a smoothness affects curvature of the twist limit
/// boundary w.r.t lean.  At 0 there is no smoothing and the limits are at their
/// maxima.  At 1 the smoothing gives a limit shape that is ellipsoidal in
/// twist-swing-swing space, hence strong coupling between twist and swing.
char clamp(const Params& params, NMP::Quat& qj, float smoothness);

/// \brief As for clamp() except without any rotation to and from the limit frame, which is
/// assumed done.
char clampSimple(const Params& params, NMP::Quat& q, float smoothness);

/// \brief Remove all swing, or rotation around anything other than the x-axis in
/// the limit frame.
void clampHinge(const Params& params, NMP::Quat& qj);

/// \brief The content of clampHinge(), without any rotation into the limit frame,
/// which is assumed already done.
void clampHingeSimple(NMP::Quat& q);

/// \brief Version which returns the clamped quaternion.
NMP::Quat clamped(const Params& params, const NMP::Quat& qj);

/// \brief Version of \a clamped() for smoothed limits.
NMP::Quat clamped(const Params& params, const NMP::Quat& qj, float smoothness);

/// \brief Clamp to scaled limits - usually used to ensure a clamped result is
/// guaranteed to be inside the limits even after numerical manipulation.
char clampScaled(const Params& params, NMP::Quat& qj, float factor);

/// \brief As for \a clampScaled(), for smoothed limits.
char clampScaled(const Params& params, NMP::Quat& qj, float factor, float smoothness);

/// \brief Clamp along an arc of rotation.
/// \param w Optional parameter, returns the fraction of the way from qFrom to q actually rotated.
char clampBetween(const Params& params, const NMP::Quat& qFrom, NMP::Quat& q, float* w = 0);

/// \brief Move to the limit boundary, about the specified \a axis which should be a unit vector.
/// \param qTo the result of rotating \a qFrom to the limit boundary or the result of rotating \a qFrom by pi
/// \param w Optional parameter, returns the fraction of half a circle achieved before the limit is found.
char toBoundarySimple(
  const Params& params, const NMP::Quat& qFrom, const NMP::Vector3& axis, NMP::Quat& qTo, float* w = 0);

/// \brief As for clampBetween(), but assumes joints are already moved into the limit frame.
char clampBetweenSimple(const Params& params, const NMP::Quat& qFromInternal, NMP::Quat& qInternal, float* w = 0);

/// \brief Clamp along an arc of rotation, for smoothed limit boundary.
/// \param w Optional parameter, returns the fraction of the way from qFrom to q actually rotated.
char clampBetween(const Params& params, const NMP::Quat& qFrom, NMP::Quat& q, float smoothness, float* w = 0);

/// \brief As for clampBetween(), but assumes joints are already moved into the limit frame.
char clampBetweenSimple(
  const Params& params, const NMP::Quat& qFromInternal, NMP::Quat& qInternal, float smoothness, float* w = 0);

/// \brief Move along an arc of rotation, for smoothed limit boundary, by an amount determined by
/// a soft weighting calculation and the \a limitRepulsionFactor.  This algorithm will prevent
/// limit violation.
/// \param w Optional parameter, returns the fraction of the way from qFrom to q actually rotated.
void clampBetweenWithBoundaryRepulsion(
  const Params& params,
  const NMP::Quat& qFrom,
  NMP::Quat& q,
  float limitRepulsionFactor,
  float smoothness,
  float* w = 0);

/// \brief As for clampBetweenWithBoundaryRepulsion(), but assuming joints are already moved into the
/// limit frame.
void clampBetweenWithBoundaryRepulsionSimple(
  const Params& params,
  const NMP::Quat& qFrom,
  NMP::Quat& q,
  float limitRepulsionFactor,
  float smoothness,
  float* w = 0);

/// \brief Restrict motion along the arc of rotation by an amount related to the proximity to
/// the limit boundary.
/// \param softness A low softness results in more restricted motion as the limit is approached.
/// A high softness means the limits are more easily penetrated.  Good values are 1 to 10.
/// \param w Optional parameter, returns the fraction of the way from qFrom to q actually rotated.
void weightBetween(
  const Params& params,
  const NMP::Quat& qFrom,
  NMP::Quat& q,
  float softness,
  float smoothness,
  float* w = 0);

/// \brief As for weightBetween(), but assumes joints are already moved into the limit frame.
void weightBetweenSimple(
  const Params& params,
  const NMP::Quat& qFromInternal,
  NMP::Quat& qInternal,
  float softness,
  float smoothness,
  float* w = 0);

/// \brief Rotate around an axis towards the nearest limit boundary until it is reached.
void rotateToLimit(const Params& params, NMP::Quat& q, const NMP::Vector3& axis, float smoothness = 0);

/// \brief Clamp along an arc of rotation specialised for this joint type.
/// If the joint type is NMSTANDARD (x-twist/yz-swing) then the swing and twist are
/// dealt with separately, so the arc the result will lie on is an arc of swing.
char clampBetweenJoint(const Params& params, const NMP::Quat& qFrom, NMP::Quat& q);

/// \brief Interpolate in a way specific to this joint type, not around a great circle like slerp.
NMP::Quat interpolate(const Params& params, const NMP::Quat& qFrom, const NMP::Quat& qTo, float t);

/// \brief The orientation which is precisely at zero swing, and the centre of the twist limits
/// (for NMSTANDARD limits).
NMP::Quat centre(const Params& params);

/// \brief Removes sudden flip from one limit boundary to another as desired orientation traverse 180 degree
/// rotation, by ramping down the amount of rotation to zero and then back up in the vicinity
/// of the discontinuity.
void softenNearFlippingPoint(
  const Params& params,
  const NMP::Quat& fromQ,
  NMP::Quat& toQ,
  float flipSoftness);

/// \brief As for softenNearFlippingPoint(), but assumes joints are already moved into the limit frame.
/// \return true if rotation was softened.
bool softenNearFlippingPointSimple(
  const NMP::Quat& qFromInternal,
  NMP::Quat& qInternal,
  float flipSoftness);

/// @} End of testing & clamping doxygen group

//----------------------------------------------------------------------------------------------------------------------
/// \name Conversions
/// @{

/// \brief Convert a joint orientation into the rotation between the limited frame and limit frame.
void toInternal(const Params& params, const NMP::Quat& qj, NMP::Quat& q);

/// \brief Convert an internal joint orientation (rotation between limit frame and limited frame) into
/// a joint orientation.
void toJoint(const Params& params, const NMP::Quat& q, NMP::Quat& qj);

/// @}

//----------------------------------------------------------------------------------------------------------------------
/// \name Utility functions
/// @{

/// \brief The function used to adjust twist limit violation so that it starts to increase as the
/// boundary approaches, rather than being zero and then suddenly 1.
float smooth(float val, float smoothness);

/// \brief Converts a 'degree of violation' value to a form which asymptotes at the
/// limit boundary.
float limitViolationFactor(float errIn);

/// \brief Converts a 'degree of violation' value to a form which increases steeply around the
/// limit boundary.
float softLimitViolationFactor(float errIn);

/// \brief Get the x axis of the limit frame, directed to ensure that the range of motion is a clockwise rotation
/// from maximum extension.
/// \param parentBone Specified in parent frame, same frame as the limit frame is specified.
/// \param childBone Specified in child frame, same frame as the limit offset is specified.
void getHingeAxis(
  const Params& limit,
  NMP::Vector3& axis,
  const NMP::Vector3& parentBone,
  const NMP::Vector3& childBone);

/// @}

} // end of namespace JointLimits

} // end of namespace NMRU

#include "NMGeomUtils/NMJointLimits.inl"

#endif // NMRU_NMJOINTLIMITS_H
