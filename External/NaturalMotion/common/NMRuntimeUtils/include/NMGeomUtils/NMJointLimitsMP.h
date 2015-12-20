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

#ifndef NMRU_NMJOINTLIMITSMP_H
#define NMRU_NMJOINTLIMITSMP_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "NMGeomUtils/NMGeomUtilsMP.h"
#include "NMGeomUtils/NMJointLimits.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Provides functions for operating with four multiplexed joint limits
namespace JointLimitsMP
{

/// \brief If needed this will identify different limit types
enum LimitType
{
  NMSTANDARD_JOINTLIMITS = 0,
  NUM_JOINTLIMITS_TYPES
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Joint limits and joint limit data manipulation
struct Params
{

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief Construct
  Params();
  Params(
    LimitType typeIn,
    const NMP::vpu::QuatMP& frameIn,
    const NMP::vpu::Vector3MP& lowerIn,
    const NMP::vpu::Vector3MP& upperIn);
  /// \brief Copy
  Params(const Params& params);
  /// \brief Create a scaled limit
  Params(const Params& params, NMP::vpu::vector4_t factor);

  /// \brief Set from four scalar JointLimits::Params
  void pack(
    const JointLimits::Params& params0,
    const JointLimits::Params& params1,
    const JointLimits::Params& params2,
    const JointLimits::Params& params3);

  /// \brief Set from twist and lean angles in radians (as opposed to twist/swing angles in tan-quarter-radians)
  void setTwistLeanLimits(
    const NMP::vpu::vector4_t& twistMin,
    const NMP::vpu::vector4_t& twistMax,
    const NMP::vpu::vector4_t& lean1,
    const NMP::vpu::vector4_t& lean2);

  //----------------------------------------------------------------------------------------------------------------------
  // Serialisation

  /// \brief Prepare this object for another platform.
  void endianSwap();

  //----------------------------------------------------------------------------------------------------------------------
  // Parameters

  /// \brief The limit frame in the same space as the joint frame is parented.
  NMP::vpu::QuatMP frame;

  /// \brief A (possibly unused) offset to the moving frame, i.e. what frame is limited - expressed
  /// in the joint frame, i.e. parented to the joint.
  NMP::vpu::QuatMP offset;

  /// \brief Three numbers specifying (in some type-dependent way) the minimum rotations.
  NMP::vpu::Vector3MP lower;

  /// \brief Three numbers specifying (in some type-dependent way) the maximum rotations.
  NMP::vpu::Vector3MP upper;

  /// \brief At the moment, there is only one type of joint limit representation, but this will
  /// extend.
  LimitType type;

}; // end of struct JointLimits::Params

//----------------------------------------------------------------------------------------------------------------------
/// \brief Test violation
NMP::vpu::vector4_t isViolated(const Params& params, const NMP::vpu::QuatMP& qj);

/// \brief The content of isViolated(), without any rotation into the limit frame, which
/// is assumed already done
NMP::vpu::vector4_t isViolatedSimple(const Params& params, const NMP::vpu::QuatMP& q);

/// \brief Put a number on 'degree' of violation, a distance from the limit in some space.
/// \return A positive number.  1 is on the limit surface, >1 is violation, <1 is
/// inside the limits.
/// \param smoothness 0 to 1, zero meaning no smoothing.
/// For NMSTANDARD_JOINTLIMITS, \a smoothness affects curvature of the twist limit
/// boundary w.r.t lean.  At 0 there is no smoothing and the limits are at their
/// maxima.  At 1 the smoothing gives a limit shape that is ellipsoidal in
/// twist-swing-swing space, hence strong coupling between twist and swing.
NMP::vpu::vector4_t degreeOfViolation(
  const Params& params,
  const NMP::vpu::QuatMP& qj,
  const NMP::vpu::vector4_t& smoothness = NMP::vpu::zero4f());

/// \brief The content is degreeOfViolation(), without any rotation into the limit frame,
/// which is assumed already done.
NMP::vpu::vector4_t degreeOfViolationSimple(
  const Params& params,
  const NMP::vpu::QuatMP& qj,
  const NMP::vpu::vector4_t& smoothness = NMP::vpu::zero4f());

/// \brief Generic clamp.
NMP::vpu::vector4_t clamp(const Params& params, NMP::vpu::QuatMP& qj);

/// \brief As for clamp() except without any rotation to and from the limit frame, which is
/// assumed done.
NMP::vpu::vector4_t clampSimple(const Params& params, NMP::vpu::QuatMP& q);

/// \brief Remove all swing, or rotation around anything other than the x-axis in
/// the limit frame.
void clampHinge(const Params& params, NMP::vpu::QuatMP& qj);

/// \brief The content of clampHinge(), without any rotation into the limit frame,
/// which is assumed already done.
void clampHingeSimple(NMP::vpu::QuatMP& q);

/// \brief Version which returns the clamped quaternion.
NMP::vpu::QuatMP clamped(const Params& params, const NMP::vpu::QuatMP& qj);

/// \brief The orientation which is precisely at zero swing, and the centre of the twist limits
/// (for NMSTANDARD limits).
NMP::vpu::QuatMP centre(const Params& params);

//----------------------------------------------------------------------------------------------------------------------
// Conversions

/// \brief Convert a joint orientation into the rotation between the limited frame and limit frame
void toInternal(const Params& params, const NMP::vpu::QuatMP& qj, NMP::vpu::QuatMP& q);

/// \brief Convert an internal joint orientation (rotation between limit frame and limited frame) into
/// a joint orientation
void toJoint(const Params& params, const NMP::vpu::QuatMP& q, NMP::vpu::QuatMP& qj);

//----------------------------------------------------------------------------------------------------------------------
// Utility functions

/// \brief The function used to adjust twist limit violation so that it starts to increase as the
/// boundary approaches, rather than being zero and then suddenly 1.
NMP::vpu::vector4_t smooth(const NMP::vpu::vector4_t& val, const NMP::vpu::vector4_t& smoothness);

/// \brief Get the x axis of the limit frame, directed to ensure that the range of motion is a clockwise rotation
/// from maximum extension.
/// \param parentBone Specified in parent frame, same frame as the limit frame is specified.
/// \param childBone Specified in child frame, same frame as the limit offset is specified.
void getHingeAxis(
  const Params& limit,
  NMP::vpu::Vector3MP& axis,
  const NMP::vpu::Vector3MP& parentBone,
  const NMP::vpu::Vector3MP& childBone);

} // end of namespace JointLimitsMP

} // end of namespace NMRU

#include "NMGeomUtils/NMJointLimitsMP.inl"

#endif // NMRU_NMJOINTLIMITSMP_H
