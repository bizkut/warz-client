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
// inline included by NMGeomUtilsMP.h
//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

namespace GeomUtilsMP
{

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::vpu::QuatMP closestRotationAroundAxis(const NMP::vpu::QuatMP& q, const NMP::vpu::Vector3MP& axis)
{
  // dot = q.x * axis.x + q.y * axis.y + q.z * axis.z
  NMP::vpu::vector4_t dot = NMP::vpu::add4f(
    NMP::vpu::add4f(NMP::vpu::mul4f(q.x, axis.x), NMP::vpu::mul4f(q.y, axis.y)),
    NMP::vpu::mul4f(q.z, axis.z));
  NMP::vpu::QuatMP result(
    NMP::vpu::mul4f(dot, axis.x), NMP::vpu::mul4f(dot, axis.y), NMP::vpu::mul4f(dot, axis.z), q.w);
  result.normalise();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE NMP::vpu::QuatMP forRotationAroundAxis(
  const NMP::vpu::Vector3MP& fromV,
  const NMP::vpu::Vector3MP& toV,
  const NMP::vpu::Vector3MP& axis)
{
  // See FPU version for explanation
  NMP::vpu::Vector3MP axisCrossV1 = axis.cross(fromV);
  NMP::vpu::Vector3MP axisCrossAxisCrossV1 = axis.cross(axisCrossV1);
  vector4 n = vNeg(toV.dot(axisCrossV1));
  vector4 d = toV.dot(axisCrossAxisCrossV1);

  // In the original version we needed to check for sign safety, but now we can do away with the
  // conditions and one square root
  vector4 K = vNeg(NMP::vpu::sqrt4f(vAdd(vMul(n, n), vMul(d, d))));
  // qResult contains [v, w] = [2Kcos(theta/2)sin(theta/2)*axis, 2Kcos(theta/2)cos(theta/2)]
  NMP::vpu::QuatMP qResult(vMul(n, axis.x), vMul(n, axis.y), vMul(n, axis.z), vAdd(d, K));
  // Normalising removes the term 2Kcos(theta/2) leaving us with a valid quat which is correct.
  // normalise() handles the zero-length case for us.
  qResult.normalise();

  return qResult;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void quatToTwistSwing(const NMP::vpu::QuatMP& q, NMP::vpu::Vector3MP& ts)
{
  NMP::vpu::vector4_t chs2 = vAdd(vMul(q.w, q.w), vMul(q.x, q.x));

  {
    NMP::vpu::vector4_t chs = NMP::vpu::sqrt4f(chs2);
    NMP::vpu::vector4_t mul = NMP::vpu::rcp4f(vMul(chs, vAdd(chs, NMP::vpu::one4f())));

    // If q.w > 0, ts.x = -q.x/(q.w+chs), else ts.x = -q.x/(q.w-chs)
    NMP::vpu::vector4_t denom = vAdd(q.w, NMP::vpu::sel4cmpLT(NMP::vpu::zero4f(), q.w, chs, NMP::vpu::neg4f(chs)));
    denom = NMP::vpu::rcp4f(denom);
    ts.x = vMul(NMP::vpu::neg4f(q.x), denom);
    ts.y = vMul(mul, vAdd(vMul(q.x, q.y), vMul(q.w, q.z)));
    ts.z = vMul(mul, vAdd(vMul(NMP::vpu::neg4f(q.w), q.y), vMul(q.x, q.z)));
  }

  NMP::vpu::Vector3MP tsTiny;
  {
    NMP::vpu::vector4_t rshs = NMP::vpu::rsqrt4f(vSub(NMP::vpu::one4f(), chs2));
    tsTiny.x = NMP::vpu::zero4f();
    tsTiny.y = vMul(rshs, q.z);
    tsTiny.z = vMul(NMP::vpu::neg4f(rshs), q.y);
  }

  // Choose tsTiny instead if the amount of twist is zero
  NMP::vpu::vector4_t mask = NMP::vpu::mask4cmpLT(chs2, NMP::vpu::set4f(1.2e-7f));
  ts = tsTiny.merge(ts, mask);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void twistSwingToQuat(const NMP::vpu::Vector3MP& ts, NMP::vpu::QuatMP& q)
{
  const NMP::vpu::vector4_t& t = ts.x;
  const NMP::vpu::vector4_t& s1 = ts.y;
  const NMP::vpu::vector4_t& s2 = ts.z;

  NMP::vpu::vector4_t two4f = NMP::vpu::set4f(2.0f);
  NMP::vpu::vector4_t b = vMul(two4f, NMP::vpu::rcp4f(vAdd(vAdd(NMP::vpu::one4f(), vMul(s1, s1)), vMul(s2, s2))));
  NMP::vpu::vector4_t c = vMul(two4f, NMP::vpu::rcp4f(vAdd(NMP::vpu::one4f(), vMul(t, t))));
  NMP::vpu::vector4_t bm1 = vSub(b, NMP::vpu::one4f());
  NMP::vpu::vector4_t cm1 = vSub(c, NMP::vpu::one4f());
  NMP::vpu::vector4_t ct = vMul(c, t);
  NMP::vpu::vector4_t negb = NMP::vpu::neg4f(b);

  q.w = vMul(bm1, cm1);
  q.x = vMul(vMul(NMP::vpu::neg4f(t), bm1), c);
  q.y = vMul(negb, vAdd(vMul(ct, s1), vMul(cm1, s2)));
  q.z = vMul(negb, vSub(vMul(ct, s2), vMul(cm1, s1)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void separateTwistFromTwistAndSwing(const NMP::vpu::QuatMP& q, NMP::vpu::QuatMP& qtwist)
{
  vector4 chs2 = vAdd(vMul(q.w, q.w), vMul(q.x, q.x));
  vector4 zero4f = NMP::vpu::zero4f();

  vector4 rchs = NMP::vpu::rsqrt4f(chs2);
  qtwist.w = vMul(q.w, rchs);
  qtwist.x = vMul(q.x, rchs);
  qtwist.y = zero4f;
  qtwist.z = zero4f;

  // Return identity if poorly conditioned
  vector4 conditionMask = NMP::vpu::mask4cmpLT(chs2, NMP::vpu::set4f(1e-6f));
  NMP::vpu::QuatMP identityQ;
  identityQ.identity();
  qtwist = identityQ.merge(qtwist, conditionMask);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void separateSwingFromTwistAndSwing(const NMP::vpu::QuatMP& q, NMP::vpu::QuatMP& qswing)
{
  vector4 chs2 = vAdd(vMul(q.w, q.w), vMul(q.x, q.x));
  vector4 zero4f = NMP::vpu::zero4f();

  // Well-conditioned
  {
    vector4 chs = NMP::vpu::sqrt4f(chs2);
    vector4 rchs = vRecip(chs);
    qswing.w = chs;
    qswing.x = zero4f;
    qswing.y = vMul(vSub(vMul(q.w, q.y), vMul(q.x, q.z)), rchs);
    qswing.z = vMul(vAdd(vMul(q.x, q.y), vMul(q.w, q.z)), rchs);
  }
  // Poorly conditioned
  {
    vector4 rshs = NMP::vpu::rsqrt4f(vSub(NMP::vpu::one4f(), chs2));
    NMP::vpu::QuatMP qswingPoorlyConditioned(
      zero4f,
      vMul(q.y, rshs),
      vMul(q.z, rshs),
      zero4f);

    vector4 conditionMask = NMP::vpu::mask4cmpLT(chs2, NMP::vpu::set4f(1e-6f));
    qswing = qswingPoorlyConditioned.merge(qswing, conditionMask);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::vpu::vector4_t safelyDivide(NMP::vpu::vector4_t a, NMP::vpu::vector4_t b)
{
  NMP::vpu::vector4_t zeroMask = NMP::vpu::mask4cmpLT(NMP::vpu::abs4f(b), nTol4f);
  return NMP::vpu::sel4cmpMask(zeroMask, NMP::vpu::zero4f(), NMP::vpu::mul4f(a, NMP::vpu::rcp4f(b)));
}

} // end of namespace GeomUtilsMP

} // end of namespace NMRU
