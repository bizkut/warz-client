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

#ifndef NMRU_GEOMUTILSMP_H
#define NMRU_GEOMUTILSMP_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMathUtils.h"
#include "NMGeomUtils/NMGeomUtils.h"
#include "NMPlatform/NMvpu.h"
//----------------------------------------------------------------------------------------------------------------------

#ifndef NMRU_VPU_SHORTCUT_MACROS
  #define NMRU_VPU_SHORTCUT_MACROS
  #define vAdd NMP::vpu::add4f
  #define vSub NMP::vpu::sub4f
  #define vMul NMP::vpu::mul4f
  #define vNeg NMP::vpu::neg4f
  #define vRecip NMP::vpu::rcp4f
  #define vAbs NMP::vpu::abs4f
  #define vDiv(x, y) vMul((x), vRecip(y))
  typedef NMP::vpu::vector4_t vector4;
#endif

namespace NMRU
{

namespace GeomUtilsMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \name Global tolerance values
/// @{

// NOTE gTol and gTolSq are still essentially based on /numerical/ accuracy, not local distance, so
// as not to suffer from dependence on choice of scale.  However, sometimes manipulation of geometric
// entities results in more numerical drift than in normal algebra, and gTol allows for a greater
// margin of error in those cases.  gTol also tells you something about what kind of size of entity
// you will be able to manipulate geometrically, e.g. how much magnitude does a vector need before it
// can be validly rotated, or normalised?
/// \brief A geometrical distance tolerance, for checking stability of rotation axes and the like
const NMP::vpu::vector4_t gTol4f = NMP::vpu::set4f(GeomUtils::gTol);
/// \brief A geometrical distance tolerance, for checking stability of rotation axes and the like
const NMP::vpu::vector4_t gTolSq4f = NMP::vpu::set4f(GeomUtils::gTolSq);
/// \brief A general numerical tolerance - geometrical entities greater than this in magnitude
/// should be able to be manipulated stably
const NMP::vpu::vector4_t nTol4f = NMP::vpu::set4f(GeomUtils::nTol);
/// \brief A general numerical tolerance - geometrical entities greater than this in magnitude
/// should be able to be manipulated stably
const NMP::vpu::vector4_t nTolSq4f = NMP::vpu::set4f(GeomUtils::nTolSq);

//----------------------------------------------------------------------------------------------------------------------
/// \brief A transform made from a quaternion and a translation vector
struct PosQuatMP
{
  NMP::vpu::Vector3MP t;
  NMP::vpu::QuatMP q;
};

//----------------------------------------------------------------------------------------------------------------------
/// \name Functions for PosQuatMPs
/// @{
/// @}

//----------------------------------------------------------------------------------------------------------------------
/// \name Functions for QuatMPs
/// @{

/// Computes the rotation about a specified axis which is close as possible (in terms of remaining
/// rotation) to given quat - multiplexed for four quats and axes
NMP::vpu::QuatMP closestRotationAroundAxis(const NMP::vpu::QuatMP& q, const NMP::vpu::Vector3MP& axis);

/// Computes the rotation about a specified axis which brings fromV as close as possible to toV.
/// fromV, toV and the axis must be unit vectors - multiplexed for four sets of arguments
NMP::vpu::QuatMP forRotationAroundAxis(
  const NMP::vpu::Vector3MP& fromV,
  const NMP::vpu::Vector3MP& toV,
  const NMP::vpu::Vector3MP& axis);

/// @}

//----------------------------------------------------------------------------------------------------------------------
/// \name Functions for Joints
/// @{

/// Convert a quaternion into a twist-swing orientation vector
void quatToTwistSwing(const NMP::vpu::QuatMP& q, NMP::vpu::Vector3MP& ts);

/// Convert a twist-swing orientation vector into a quaternion
void twistSwingToQuat(const NMP::vpu::Vector3MP& ts, NMP::vpu::QuatMP& q);

/// Extract the twist (x-axis) rotation from a general rotation
void separateTwistFromTwistAndSwing(const NMP::vpu::QuatMP& q, NMP::vpu::QuatMP& qtwist);

/// Extract the swing (y-z plane rotation) from a general rotation
void separateSwingFromTwistAndSwing(const NMP::vpu::QuatMP& q, NMP::vpu::QuatMP& qswing);

/// @}

//----------------------------------------------------------------------------------------------------------------------
/// \name Useful utilities
/// @{

/// \brief Find the closest point on a 2D ellipse boundary ((x/A)^2 + (y/B)^2 = 1) to a given point.
/// This method will often return the wrong result for points inside the ellipse - in this case use
/// bisection search, which is slower but will always find the closest point.  At best it is robust
/// when the point in question drifts inside the ellipse to about 98% of the distance from the centre.
/// \param x The x component of the input point, replaced with the output x value.
/// \param y The y component of the input point, replaced with the output y value.
/// \param A The x-axis radius.  Must be greater than 0.
/// \param B The y-axis radius.  Must be greater than 0.
void closestPointOnEllipse_newton(
  NMP::vpu::vector4_t& x,
  NMP::vpu::vector4_t& y,
  const NMP::vpu::vector4_t& A,
  const NMP::vpu::vector4_t& B);

/// \brief Divide \a a by \a b except when b is zero, in which case return zero.
NMP::vpu::vector4_t safelyDivide(NMP::vpu::vector4_t a, NMP::vpu::vector4_t b);

/// @}

}  // end of namespace GeomUtilsMP

}  // end of namespace NMRU

#include "NMGeomUtils/NMGeomUtilsMP.inl"

#endif // NMRU_GEOMUTILSMP_H
