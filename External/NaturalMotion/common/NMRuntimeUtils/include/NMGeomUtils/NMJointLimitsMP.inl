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
// inline included by NMJointLimitsMP.h
//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE JointLimitsMP::Params::Params() :
  frame(NMP::vpu::set4f(0, 0, 0, 1.0f)),
  offset(NMP::vpu::set4f(0, 0, 0, 1.0f)),
  lower(NMP::vpu::set4f(-1.0f, 0, 0, 0)),
  upper(NMP::vpu::one4f()),
  type(NMSTANDARD_JOINTLIMITS)
{
  // ...
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE JointLimitsMP::Params::Params(
  LimitType typeIn,
  const NMP::vpu::QuatMP& frameIn,
  const NMP::vpu::Vector3MP& lowerIn,
  const NMP::vpu::Vector3MP& upperIn) :
  frame(frameIn),
  offset(NMP::vpu::set4f(0, 0, 0, 1.0f)),
  lower(lowerIn),
  upper(upperIn),
  type(typeIn)
{
  // ...
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE JointLimitsMP::Params::Params(const Params& params) :
  frame(params.frame),
  offset(params.offset),
  lower(params.lower),
  upper(params.upper),
  type(NMSTANDARD_JOINTLIMITS)
{
  // ...
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE JointLimitsMP::Params::Params(const Params& params, NMP::vpu::vector4_t factor) :
  frame(params.frame),
  offset(params.offset),
  type(params.type)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed
  NMP::vpu::vector4_t vecZero = NMP::vpu::zero4f();

  // Scaled twist limits
  NMP::vpu::vector4_t half4f = NMP::vpu::set4f(0.5f);
  NMP::vpu::vector4_t midTwist = vMul(half4f, vAdd(params.lower[0], params.upper[0]));
  NMP::vpu::vector4_t scaledHalfRangeTwist = vMul(factor, vMul(half4f, vSub(params.upper[0], params.lower[0])));
  NMP::vpu::vector4_t lower0 = vSub(midTwist, scaledHalfRangeTwist);
  NMP::vpu::vector4_t upper0 = vAdd(midTwist, scaledHalfRangeTwist);

  // Scaled swing limits
  // OPTIMISED - No need to set lower swing limits
  NMP::vpu::vector4_t upper1 = vMul(factor, params.upper[1]);
  NMP::vpu::vector4_t upper2 = vMul(factor, params.upper[2]);

  lower = NMP::vpu::Vector3MP(lower0, vecZero, vecZero);
  upper = NMP::vpu::Vector3MP(upper0, upper1, upper2);
}

NM_INLINE void JointLimitsMP::Params::pack(
  const JointLimits::Params& params0,
  const JointLimits::Params& params1,
  const JointLimits::Params& params2,
  const JointLimits::Params& params3)
{
  // At the moment, don't handle any other types
  NMP_ASSERT(
    params0.type == JointLimits::NMSTANDARD_JOINTLIMITS &&
    params1.type == JointLimits::NMSTANDARD_JOINTLIMITS &&
    params2.type == JointLimits::NMSTANDARD_JOINTLIMITS &&
    params3.type == JointLimits::NMSTANDARD_JOINTLIMITS);
  type = NMSTANDARD_JOINTLIMITS;

  frame.pack(
    *(vector4*)&params0.frame,
    *(vector4*)&params1.frame,
    *(vector4*)&params2.frame,
    *(vector4*)&params3.frame);
  offset.pack(
    *(vector4*)&params0.offset,
    *(vector4*)&params1.offset,
    *(vector4*)&params2.offset,
    *(vector4*)&params3.offset);
  lower.pack(
    *(vector4*)&params0.lower,
    *(vector4*)&params1.lower,
    *(vector4*)&params2.lower,
    *(vector4*)&params3.lower);
  upper.pack(
    *(vector4*)&params0.upper,
    *(vector4*)&params1.upper,
    *(vector4*)&params2.upper,
    *(vector4*)&params3.upper);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void JointLimitsMP::Params::setTwistLeanLimits(
  const NMP::vpu::vector4_t& twistMin,
  const NMP::vpu::vector4_t& twistMax,
  const NMP::vpu::vector4_t& lean1,
  const NMP::vpu::vector4_t& lean2)
{
  type = NMSTANDARD_JOINTLIMITS;
  NMP::vpu::vector4_t quarter4f = NMP::vpu::set4f(0.25f);
  lower = NMP::vpu::Vector3MP(
    NMP::vpu::tan4f(vMul(twistMin, quarter4f)),
    NMP::vpu::zero4f(),
    NMP::vpu::zero4f());
  upper = NMP::vpu::Vector3MP(
    NMP::vpu::tan4f(vMul(twistMax, quarter4f)),
    NMP::vpu::tan4f(vMul(lean1, quarter4f)),
    NMP::vpu::tan4f(vMul(lean2, quarter4f)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void JointLimitsMP::Params::endianSwap()
{
  NMP::endianSwap(frame);
  NMP::endianSwap(offset);
  NMP::endianSwap(lower);
  NMP::endianSwap(upper);
  NMP::endianSwap(type);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::vpu::QuatMP JointLimitsMP::clamped(const Params& params, const NMP::vpu::QuatMP& q)
{
  NMP::vpu::QuatMP qResult = q;
  clamp(params, qResult);
  return qResult;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::vpu::vector4_t JointLimitsMP::clamp(const Params& params, NMP::vpu::QuatMP& qj)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  // Orientation in the limit frame
  NMP::vpu::QuatMP q;
  toInternal(params, qj, q);
  NMP::vpu::vector4_t retval = clampSimple(params, q);
  // OPTIMISE  If converting back is too slow, then we can optionally convert back only if the
  // value has been clamped - this requires retrieving the results of retval so is probably more
  // complex to test than just to do anyway.
  toJoint(params, q, qj);
  return retval;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::vpu::vector4_t JointLimitsMP::isViolated(const Params& params, const NMP::vpu::QuatMP& qj)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  // Orientation in the limit frame
  NMP::vpu::QuatMP q;
  toInternal(params, qj, q);

  return isViolatedSimple(params, q);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::vpu::vector4_t JointLimitsMP::isViolatedSimple(const Params& params, const NMP::vpu::QuatMP& q)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  static const NMP::vpu::vector4_t zero4i = NMP::vpu::set4i(0);
  static const NMP::vpu::vector4_t one4i = NMP::vpu::set4i(1);
  static const NMP::vpu::vector4_t two4i = NMP::vpu::add4i(one4i, one4i);
  NMP::vpu::vector4_t result = zero4i;

  NMP::vpu::Vector3MP tl;
  NMP::vpu::vector4_t& t = tl.x;
  NMP::vpu::vector4_t& l1 = tl.y;
  NMP::vpu::vector4_t& l2 = tl.z;

  GeomUtilsMP::quatToTwistSwing(q, tl);

  //----------------------------------------------------------------------------------------------------------------------
  // Twist

  NMP::vpu::vector4_t twistViolatedMask =
    NMP::vpu::or4(NMP::vpu::mask4cmpLT(t, params.lower.x), NMP::vpu::mask4cmpLT(params.upper.x, t));
  NMP::vpu::vector4_t twistViolated = NMP::vpu::sel4cmpMask(twistViolatedMask, one4i, zero4i);

  //----------------------------------------------------------------------------------------------------------------------
  // Lean

  //   Test in case limits are zero - avoid divide by zero
  //   If both are zero, assume hinge, and ignore violation
  static const NMP::vpu::vector4_t tol4f = NMP::vpu::set4f(1e-3f);
  NMP::vpu::vector4_t oneFlatSwingMask =
    NMP::vpu::or4(NMP::vpu::mask4cmpLT(params.upper.y, tol4f), NMP::vpu::mask4cmpLT(params.upper.z, tol4f));
  NMP::vpu::vector4_t swing1AloneViolatedMask =
    NMP::vpu::and4(NMP::vpu::mask4cmpLT(GeomUtilsMP::nTol4f, NMP::vpu::abs4f(l1)),
    NMP::vpu::or4(NMP::vpu::mask4cmpLT(l1, NMP::vpu::neg4f(params.upper.y)),
    NMP::vpu::mask4cmpLT(params.upper.y, l1)));
  NMP::vpu::vector4_t swing2AloneViolatedMask =
    NMP::vpu::and4(NMP::vpu::mask4cmpLT(GeomUtilsMP::nTol4f, NMP::vpu::abs4f(l2)),
    NMP::vpu::or4(NMP::vpu::mask4cmpLT(l2, NMP::vpu::neg4f(params.upper.z)),
    NMP::vpu::mask4cmpLT(params.upper.z, l2)));
  NMP::vpu::vector4_t isHingeMask =
    NMP::vpu::and4(NMP::vpu::mask4cmpEQ(params.upper.y, zero4i), NMP::vpu::mask4cmpEQ(params.upper.z, zero4i));
  NMP::vpu::vector4_t swingAloneViolatedMask =
    NMP::vpu::and4(NMP::vpu::or4(swing1AloneViolatedMask, swing2AloneViolatedMask), NMP::vpu::not4(isHingeMask));

  //   Normal lean test
  NMP::vpu::vector4_t l1unit = vMul(l1, NMP::vpu::rcp4f(params.upper.y));
  NMP::vpu::vector4_t l2unit = vMul(l2, NMP::vpu::rcp4f(params.upper.z));
  NMP::vpu::vector4_t dist4f = vSub(vAdd(vMul(l1unit, l1unit), vMul(l2unit, l2unit)), NMP::vpu::one4f());
  NMP::vpu::vector4_t swingViolatedMask =
    NMP::vpu::sel4cmpMask(oneFlatSwingMask, swingAloneViolatedMask,
    NMP::vpu::mask4cmpLT(NMP::vpu::zero4f(), dist4f));
  NMP::vpu::vector4_t swingViolated = NMP::vpu::sel4cmpMask(swingViolatedMask, two4i, zero4i);

  result = NMP::vpu::add4i(twistViolated, swingViolated);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::vpu::vector4_t JointLimitsMP::degreeOfViolation(
  const Params& params,
  const NMP::vpu::QuatMP& qj,
  const NMP::vpu::vector4_t& smoothness /* = zero4f() */)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  // Orientation in the limit frame
  NMP::vpu::QuatMP q;
  toInternal(params, qj, q);

  return degreeOfViolationSimple(params, q, smoothness);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::vpu::vector4_t JointLimitsMP::degreeOfViolationSimple(
  const Params& params,
  const NMP::vpu::QuatMP& q,
  const NMP::vpu::vector4_t& smoothness /* = zero4f() */)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  NMP::vpu::vector4_t result = NMP::vpu::zero4f();

  NMP::vpu::Vector3MP tl;
  NMP::vpu::vector4_t& t = tl.x;
  NMP::vpu::vector4_t& l1 = tl.y;
  NMP::vpu::vector4_t& l2 = tl.z;

  GeomUtilsMP::quatToTwistSwing(q, tl);

  // This assumes that if the limit is zero that dof will always be zero, so doesn't
  // use it in the degree of violation value.  This prevents some sticking but can lead to limit
  // violation, depending on what the value is being used for.

  // Twist
  static const NMP::vpu::vector4_t half4f = NMP::vpu::set4f(0.5f);
  NMP::vpu::vector4_t range = vMul(half4f, vSub(params.upper.x, params.lower.x));
  NMP::vpu::vector4_t centre = vMul(half4f, vAdd(params.upper.x, params.lower.x));
  NMP::vpu::vector4_t twistUnit = vMul(vSub(t, centre), NMP::vpu::rcp4f(range));
  twistUnit = smooth(twistUnit, smoothness);
  result = NMP::vpu::sel4cmpLT(range, GeomUtilsMP::nTol4f, NMP::vpu::zero4f(), vMul(twistUnit, twistUnit));

  // Lean
  //   Test in case limits are zero - avoid divide by zero.
  NMP::vpu::vector4_t l1unit = vMul(l1, NMP::vpu::rcp4f(params.upper.y));
  result = NMP::vpu::sel4cmpLT(params.upper.y, GeomUtilsMP::nTol4f, result, vAdd(result, vMul(l1unit, l1unit)));
  NMP::vpu::vector4_t l2unit = vMul(l2, NMP::vpu::rcp4f(params.upper.z));
  result = NMP::vpu::sel4cmpLT(params.upper.z, GeomUtilsMP::nTol4f, result, vAdd(result, vMul(l2unit, l2unit)));

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::vpu::QuatMP JointLimitsMP::centre(const Params& params)
{
  NMP_ASSERT(params.type == NMSTANDARD_JOINTLIMITS); // Change to a condition only if other types needed

  static const NMP::vpu::vector4_t zero4f = NMP::vpu::zero4f();
  static const NMP::vpu::vector4_t half4f = NMP::vpu::set4f(0.5f);
  NMP::vpu::Vector3MP tl(vAdd(vMul(half4f, params.upper.x), vMul(half4f, params.lower.x)), zero4f, zero4f);
  NMP::vpu::QuatMP q;
  GeomUtilsMP::twistSwingToQuat(tl, q);
  NMP::vpu::QuatMP qj;
  toJoint(params, q, qj);
  return qj;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void JointLimitsMP::toInternal(const Params& params, const NMP::vpu::QuatMP& qj, NMP::vpu::QuatMP& q)
{
  q = params.frame.conjugate() * qj * params.offset;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void JointLimitsMP::toJoint(const Params& params, const NMP::vpu::QuatMP& q, NMP::vpu::QuatMP& qj)
{
  qj = params.frame * q * params.offset.conjugate();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::vpu::vector4_t JointLimitsMP::smooth(
  const NMP::vpu::vector4_t& valIn,
  const NMP::vpu::vector4_t& smoothnessIn)
{
  // Clamp
  static const NMP::vpu::vector4_t limitLow = NMP::vpu::set4f(1e-5f);
  static const NMP::vpu::vector4_t limitHigh = NMP::vpu::set4f(0.99f);
  NMP::vpu::vector4_t smoothness = NMP::vpu::sel4cmpLT(smoothnessIn, limitLow, limitLow, smoothnessIn);
  smoothness = NMP::vpu::sel4cmpLT(smoothness, limitHigh, smoothness, limitHigh);

  // Calculation
  static const NMP::vpu::vector4_t zero4f = NMP::vpu::zero4f();
  static const NMP::vpu::vector4_t one4f = NMP::vpu::one4f();
  static const NMP::vpu::vector4_t minusOne4f = NMP::vpu::neg4f(one4f);
  NMP::vpu::vector4_t K = vMul(vSub(one4f, smoothness), NMP::vpu::rcp4f(vMul(smoothness, smoothness)));
  NMP::vpu::vector4_t sign = NMP::vpu::sel4cmpLT(valIn, zero4f, minusOne4f, one4f);
  NMP::vpu::vector4_t val = NMP::vpu::abs4f(valIn);

  val = NMP::vpu::sel4cmpLT(val, one4f,
    vAdd(vMul((vMul(vAdd(K, one4f), NMP::vpu::rcp4f(K))),
      vSub(vMul(minusOne4f, NMP::vpu::rcp4f(vSub(vMul(K, vSub(val, one4f)), one4f))), one4f)),
      one4f),
    val);

  return vMul(val, sign);
}

} // end of namespace NMRU
