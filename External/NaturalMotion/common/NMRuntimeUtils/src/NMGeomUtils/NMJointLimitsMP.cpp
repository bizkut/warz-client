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
#include "NMGeomUtils/NMJointLimitsMP.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
NMP::vpu::vector4_t JointLimitsMP::clampSimple(const Params& params, NMP::vpu::QuatMP& q)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  static const NMP::vpu::vector4_t zero4i = NMP::vpu::set4i(0);
  static const NMP::vpu::vector4_t one4i = NMP::vpu::set4i(1);
  static const NMP::vpu::vector4_t two4i = NMP::vpu::add4i(one4i, one4i);
  NMP::vpu::vector4_t result = zero4i;

  // Twist-lean for clamping
  NMP::vpu::Vector3MP tl;
  GeomUtilsMP::quatToTwistSwing(q, tl);

  vector4& t = tl.x;
  vector4& l1 = tl.y;
  vector4& l2 = tl.z;

  // Twist
  vector4 twistViolatedMask =
    NMP::vpu::or4(NMP::vpu::mask4cmpLT(t, params.lower.x), NMP::vpu::mask4cmpLT(params.upper.x, t));
  NMP::vpu::vector4_t twistViolated = NMP::vpu::sel4cmpMask(twistViolatedMask, one4i, zero4i);
  //  Clamp
  t = NMP::vpu::clamp4f(t, params.lower.x, params.upper.x);

  // Lean

  //   Test in case limits are zero - avoid divide by zero
  //   If both are zero, assume hinge, and force zero lean but ignore violation
  static const NMP::vpu::vector4_t tol4f = NMP::vpu::set4f(1e-3f);
  NMP::vpu::vector4_t oneFlatSwingMask =
    NMP::vpu::or4(NMP::vpu::mask4cmpLT(params.upper.y, tol4f), NMP::vpu::mask4cmpLT(params.upper.z, tol4f));
  //
  vector4 l1Abs = vAbs(l1);
  vector4 swing1LimNeg = vNeg(params.upper.y);
  vector4 swing1AloneViolatedMask =
    NMP::vpu::and4(NMP::vpu::mask4cmpLT(GeomUtilsMP::nTol4f, l1Abs),
    NMP::vpu::or4(NMP::vpu::mask4cmpLT(l1, swing1LimNeg),
    NMP::vpu::mask4cmpLT(params.upper.y, l1)));
  vector4 swing1IndependentlyClamped = NMP::vpu::clamp4f(l1, swing1LimNeg, params.upper.y);
  swing1IndependentlyClamped = NMP::vpu::sel4cmpLT(l1Abs, GeomUtilsMP::nTol4f, l1, swing1IndependentlyClamped);
  //
  vector4 l2Abs = vAbs(l2);
  vector4 swing2LimNeg = vNeg(params.upper.z);
  vector4 swing2AloneViolatedMask =
    NMP::vpu::and4(NMP::vpu::mask4cmpLT(GeomUtilsMP::nTol4f, l2Abs),
    NMP::vpu::or4(NMP::vpu::mask4cmpLT(l2, swing2LimNeg),
    NMP::vpu::mask4cmpLT(params.upper.z, l2)));
  vector4 swing2IndependentlyClamped = NMP::vpu::clamp4f(l2, swing2LimNeg, params.upper.z);
  swing2IndependentlyClamped = NMP::vpu::sel4cmpLT(l2Abs, GeomUtilsMP::nTol4f, l2, swing2IndependentlyClamped);
  //
  vector4 isHingeMask =
    NMP::vpu::and4(NMP::vpu::mask4cmpEQ(params.upper.y, zero4i), NMP::vpu::mask4cmpEQ(params.upper.z, zero4i));
  vector4 swingAloneViolatedMask =
    NMP::vpu::and4(NMP::vpu::or4(swing1AloneViolatedMask, swing2AloneViolatedMask), NMP::vpu::not4(isHingeMask));

  //   Normal lean clamp
  NMP::vpu::vector4_t l1unit = vMul(l1, NMP::vpu::rcp4f(params.upper.y));
  NMP::vpu::vector4_t l2unit = vMul(l2, NMP::vpu::rcp4f(params.upper.z));
  NMP::vpu::vector4_t dist4f = vSub(vAdd(vMul(l1unit, l1unit), vMul(l2unit, l2unit)), NMP::vpu::one4f());
  vector4 swingOutsideEllipseMask = NMP::vpu::mask4cmpLT(NMP::vpu::zero4f(), dist4f);
  NMP::vpu::vector4_t swingViolatedMask =
    NMP::vpu::sel4cmpMask(oneFlatSwingMask, swingAloneViolatedMask, swingOutsideEllipseMask);
  NMP::vpu::vector4_t swingViolated = NMP::vpu::sel4cmpMask(swingViolatedMask, two4i, zero4i);
  //
  vector4 l1OnSwingLimit = l1;
  vector4 l2OnSwingLimit = l2;
  GeomUtilsMP::closestPointOnEllipse_newton(l1OnSwingLimit, l2OnSwingLimit, params.upper.y, params.upper.z);
  l1OnSwingLimit = NMP::vpu::sel4cmpMask(swingOutsideEllipseMask, l1OnSwingLimit, l1);
  l2OnSwingLimit = NMP::vpu::sel4cmpMask(swingOutsideEllipseMask, l2OnSwingLimit, l2);
  //
  swingAloneViolatedMask = NMP::vpu::and4(oneFlatSwingMask, swingAloneViolatedMask);
  l1 = NMP::vpu::sel4cmpMask(swingAloneViolatedMask, swing1IndependentlyClamped, l1OnSwingLimit);
  l2 = NMP::vpu::sel4cmpMask(swingAloneViolatedMask, swing2IndependentlyClamped, l2OnSwingLimit);
  //
  l1 = NMP::vpu::sel4cmpMask(isHingeMask, NMP::vpu::zero4f(), l1);
  l2 = NMP::vpu::sel4cmpMask(isHingeMask, NMP::vpu::zero4f(), l2);

  // How were we clamped?
  result = NMP::vpu::add4i(twistViolated, swingViolated);

  // OPTIMISE  If converting back is too slow, then we can optionally convert back only if the
  // value has been clamped - this requires retrieving the contents of result so is probably more
  // complex to test than just to do anyway.
  GeomUtilsMP::twistSwingToQuat(tl, q);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimitsMP::clampHinge(const Params& params, NMP::vpu::QuatMP& qj)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  // Orientation in the limit frame
  NMP::vpu::QuatMP q;
  toInternal(params, qj, q);

  clampHingeSimple(q);

  toJoint(params, q, qj);
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimitsMP::clampHingeSimple(NMP::vpu::QuatMP& q)
{
  NMP::vpu::QuatMP qClamped;
  GeomUtilsMP::separateTwistFromTwistAndSwing(q, qClamped);

  q = qClamped;
}

//----------------------------------------------------------------------------------------------------------------------
void JointLimitsMP::getHingeAxis(
  const Params& limit,
  NMP::vpu::Vector3MP& axis,
  const NMP::vpu::Vector3MP& parentBone,
  const NMP::vpu::Vector3MP& childBone)
{
  // At the moment, assume we are only using NMSTANDARD limits - this tells us that the twist axis is
  // the x-axis
  NMP_ASSERT(limit.type == NMSTANDARD_JOINTLIMITS);

  // Initialisations
  static const vector4 one4f = NMP::vpu::one4f();
  static const vector4 zero4f = NMP::vpu::zero4f();

  // Hinge axis is the x-axis or negative x-axis in the limit frame
  axis.x = one4f;
  axis.y = zero4f;
  axis.z = zero4f;

  // Find the orientation when the two bones are parallel in the plane perpendicular
  // to the twist axis.  Do calculations in the limit frame.
  NMP::vpu::Vector3MP v1 = limit.frame.inverseRotateVector(parentBone);
  NMP::vpu::Vector3MP v2 = limit.offset.inverseRotateVector(childBone);
  v1.normalise();
  v2.normalise();
  NMP::vpu::QuatMP straightQ = GeomUtilsMP::forRotationAroundAxis(v2, v1, axis);

  // Find the orientation when the joint is on its minimum and maximum twist limit
  NMP::vpu::QuatMP lowerTwistLimitQ;
  NMP::vpu::QuatMP upperTwistLimitQ;
  GeomUtilsMP::twistSwingToQuat(NMP::vpu::Vector3MP(limit.lower.x, zero4f, zero4f), lowerTwistLimitQ);
  GeomUtilsMP::twistSwingToQuat(NMP::vpu::Vector3MP(limit.upper.x, zero4f, zero4f), upperTwistLimitQ);

  // Find whichever of the two is closest to the straight orientation.  This is then our
  // 'furthest bent backwards' limit.  If it turns out it's the lower limit, the hinge
  // axis needs to be reversed.
  vector4 dotQLower = vAbs(lowerTwistLimitQ.dot(straightQ));
  vector4 dotQUpper = vAbs(upperTwistLimitQ.dot(straightQ));
  // The cosine of an angle is larger the smaller the angle
  axis.x = NMP::vpu::sel4cmpLT(dotQUpper, dotQLower, vNeg(one4f), one4f);

  // Rotate hinge axis back into joint frame.
  axis = limit.frame.rotateVector(axis);

  axis.normalise();
}

} // end of namespace NMRU
