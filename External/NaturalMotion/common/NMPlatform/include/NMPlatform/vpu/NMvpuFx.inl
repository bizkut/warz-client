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

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup NaturalMotionVPUFX Non-multiplexed Vector Intrinsics Function.
/// \ingroup NaturalMotionVPU
///
/// \brief Single, Non-Multiplexed Vector Functions
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUFX
///
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t sel4Min(const vector4_t vec)
{
  vector4_t rvec = rotL4(vec);
  vector4_t mask = sel4cmpLT(vec, rvec, vec, zero4f());
  rvec = rotL4(rvec); mask = sel4cmpLT(mask, rvec, mask, zero4f());
  mask = sel4cmpLT(mask, rotL4(rvec), mask, zero4f());
  return mask;
}

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUFX
///
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t sel4Max(const vector4_t vec)
{
  vector4_t rvec = rotL4(vec);
  vector4_t mask = sel4cmpLT(rvec, vec, vec, zero4f());
  rvec = rotR4(vec); mask = sel4cmpLT(rvec, mask, mask, zero4f());
  mask = sel4cmpLT(rotL4(rvec), mask, mask, zero4f());
  return mask;
}

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUFX
/// \brief Clamp four floats between four minima and four maxima
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t clamp4f(
  const vector4_t& vec,
  const vector4_t& vMin,
  const vector4_t& vMax)
{
  vector4_t result = sel4cmpLT(vec, vMin, vMin, vec);
  result = sel4cmpLT(vec, vMax, result, vMax);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUFX
///
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t uniformQuantisation16(
  const vector4_t& qMin,
  const vector4_t& qMax)
{
  vector4_t mStepSize = set4f(1.0f / 65535); // 1 / (2^16 - 1)
  return mul4f(mStepSize, sub4f(qMax, qMin));
}

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUFX
///
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t uniformQuantisation8(
  const vector4_t& qMin,
  const vector4_t& qMax)
{
  vector4_t mStepSize = set4f(1.0f / 255); // 1 / (2^8 - 1)
  return mul4f(mStepSize, sub4f(qMax, qMin));
}

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUFX
///
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t uniformQuantisationPrec(
  const vector4_t& qMin,
  const vector4_t& qMax,
  const vector4_t& prec)
{
  // (qMax - qMin) / (float)((0x80000001 << prec) - 1)
  vector4_t vs = shl4(set4i(0x80000001), prec);
  vector4_t numSteps = c2float4i(sub4i(vs, set4i(1)));
  return mul4f(rcp4f(numSteps), sub4f(qMax, qMin));
}

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUFX
///
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void normalize3(vector4_t vec)
{
  vec = mul4f(vec, rsqrt4f(dot3f(vec, vec)));
}

NM_FORCEINLINE vector4_t normalise3OrDef(vector4_t vec, vector4_t defvec)
{
  vector4_t mag2 = dot3f(vec, vec);
  return sel4cmpEQ(mag2, zero4f(), defvec, mul4f(vec, rsqrt4f(mag2)));
}

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionVPUFX
///
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t dequantise(const vector4_t& qScale, const vector4_t& qOffset, const uint16_t* inData)
{
  return madd4f(qScale, unpacku4i16f(inData), qOffset);
}

//----------------------------------------------------------------------------------------------------------------------

/// \ingroup NaturalMotionVPUFX
/// \brief Fast spherical linear interpolation between the two quaternions given.
NM_FORCEINLINE vector4_t fastSlerp2(const vector4_t& q0, const vector4_t& q1, const vector4_t& ta,  const vector4_t& fdt)
{
  vector4_t recipOnePlusFdt;
  vector4_t T, t2, T2, C;
  vector4_t startWeight, endWeight;

  const vector4_t mOne  = one4f();

  // recipOnePlusFdt = 1.0f / (1.0f + fdt), T = 1 - tas, t2 = tas * tas, T2 = T * T
  recipOnePlusFdt = rcp4f(add4f(mOne, fdt));
  T  = sub4f(mOne, ta);
  t2 = mul4f(ta, ta);
  T2 = mul4f(T, T);

  // c1 =  1.570994357000f + (0.56429298590f + (-0.17836577170f + 0.043199493520f * fdt) * fdt) * fdt;
  // c3 = -0.646139638200f + (0.59456579360f + (0.08610323953f - 0.034651229280f * fdt) * fdt) * fdt;
  // c5 =  0.079498235210f + (-0.17304369310f + (0.10792795990f - 0.014393978010f * fdt) * fdt) * fdt;
  // c7 = -0.004354102836f + (0.01418962736f + (-0.01567189691f + 0.005848706227f * fdt) * fdt) * fdt;

  static const vector4_t Ca = set4f(1.57099435700f, -0.64613963820f, 0.07949823521f, -0.004354102836f);
  static const vector4_t Cb = set4f(0.56429298590f, 0.59456579360f, -0.17304369310f, 0.014189627360f);
  static const vector4_t Cc = set4f(-0.17836577170f, 0.08610323953f, 0.10792795990f, -0.015671896910f);
  static const vector4_t Cd = set4f(0.04319949352f, -0.03465122928f, -0.01439397801f, 0.005848706227f);

  C = add4f(Ca, mul4f(fdt, add4f(Cb, mul4f(fdt, add4f(Cc, mul4f(fdt, Cd))))));

  vector4_t c1 = splatX(C);
  vector4_t c3 = splatY(C);
  vector4_t c5 = splatZ(C);
  vector4_t c7 = splatW(C);

  // startWeight = (c1 + (c3+(c5+c7*T2)*T2)*T2) * T * recipOnePlusFdt;
  // endWeight   = (c1 + (c3+(c5+c7*t2)*t2)*t2) * ta * recipOnePlusFdt;

  startWeight = mul4f(add4f(c1, mul4f(add4f(c3, mul4f(add4f(c5, mul4f(c7, T2)), T2)), T2)), mul4f(recipOnePlusFdt, T));
  endWeight   = mul4f(add4f(c1, mul4f(add4f(c3, mul4f(add4f(c5, mul4f(c7, t2)), t2)), t2)), mul4f(recipOnePlusFdt, ta));

  return add4f(mul4f(q0, startWeight), mul4f(q1, endWeight));
}

//----------------------------------------------------------------------------------------------------------------------

/// \ingroup NaturalMotionVPUFX
/// \brief Linearly interpolate between 2 sets of vectors, by distance alpha in the range [0.0f, 1.0f];
NM_FORCEINLINE vector4_t lerpMP4(const vector4_t& vecSrcA, const vector4_t& vecSrcB, const vector4_t& interpolant)
{
  return madd4f(interpolant, sub4f(vecSrcB, vecSrcA), vecSrcA);
}

/// \ingroup NaturalMotionVPUFX
/// \brief Get the Conjugate of a single quaternion
NM_FORCEINLINE vector4_t conjugate(const vector4_t& quat)
{
  return setwf(neg4f(quat), quat);
}

//----------------------------------------------------------------------------------------------------------------------

/// \ingroup NaturalMotionVPUFX
/// \brief Convert tan quarter angle rotation vector into a quaternion
NM_FORCEINLINE vector4_t convertTQARV2Quat(vector4_t quat)
{
  const vector4_t mOne  = one4f();
  const vector4_t scale = setwf(add4f(mOne, mOne), mOne);

  vector4_t mag2 = dot3f(quat, quat);
  vector4_t fact = rcp4f(add4f(mOne, mag2));

  return mul4f(setwf(quat, sub4f(mOne, mag2)), mul4f(scale, fact));
}

//----------------------------------------------------------------------------------------------------------------------

/// \ingroup NaturalMotionVPUFX
/// \brief Interpolative blending of 2 quaternion attitudes.
NM_FORCEINLINE vector4_t interpBlendQuats(vector4_t q0, vector4_t q1, vector4_t alpha)
{
  //fromDotTo = source0->dot(*source1);
  vector4_t fromDotTo = dot4f(q0, q1);
  vector4_t fromDotToAbs = abs4f(fromDotTo);

  // if (fromDotTo < 0)
  //   source1->negate();
  vector4_t q1a = neg4f(q1, fromDotTo);

  return fastSlerp2(q0, q1a, alpha, fromDotToAbs);
}

//----------------------------------------------------------------------------------------------------------------------

/// \ingroup NaturalMotionVPUFX
/// \brief Rotate a vector by a quaternion
NM_FORCEINLINE vector4_t quatRotVec(const vector4_t quat, const vector4_t vec)
{
  vector4_t mOne = one4f();
  vector4_t mW   = splatW(quat);
  vector4_t qdotv = dot3f(quat, vec);
  vector4_t result = cross3f(quat, vec);
  vector4_t mW2 = mul4f(mW, mW);

  result = mul4f(result, add4f(mW, mW));
  result = add4f(result, mul4f(vec, sub4f(add4f(mW2, mW2), mOne)));

  return add4f(result, mul4f(quat, add4f(qdotv, qdotv)));
}

/// \ingroup NaturalMotionVPUFX
/// \brief Inverse rotate a vector by a quaternion
NM_FORCEINLINE vector4_t quatInvRotVec(const vector4_t quat, const vector4_t vec)
{
  vector4_t mOne = one4f();
  vector4_t mW   = splatW(quat);
  vector4_t qdotv = dot3f(quat, vec);
  vector4_t result = cross3f(quat, vec);
  vector4_t mW2 = mul4f(mW, mW);

  mW = neg4f(mW);
  result = mul4f(result, add4f(mW, mW));
  result = add4f(result, mul4f(vec, sub4f(add4f(mW2, mW2), mOne)));

  return add4f(result, mul4f(quat, add4f(qdotv, qdotv)));
}

//----------------------------------------------------------------------------------------------------------------------

/// \ingroup NaturalMotionVPUFX
/// \brief Generate a matrix rotation basis from a quat.
NM_FORCEINLINE void rotFromQuat(vector4_t& r0, vector4_t& r1, vector4_t& r2, const vector4_t& q)
{

#if defined(NM_HOST_X360) && !defined(NMP_VPU_EMULATION)

  __vector4 Q0, Q1;
  __vector4 V0, V1, V2;
  __vector4 R1, R2;
  __vector4 ZO;
  __vector4 Constant1110;

  ZO = __vspltisw(0);

  Q0 = __vaddfp (q, q);
  Q1 = __vmulfp (q, Q0);

  ZO = __vupkd3d (ZO, VPACK_NORMSHORT2);
  Constant1110 = __vpermwi (ZO, 0xFE);

  V0 = __vpermwi (Q1, 0x40);
  V1 = __vpermwi (Q1, 0xA4);

  r0 = __vsubfp (Constant1110, V0);
  r0 = __vsubfp (r0, V1);

  V0 = __vpermwi (q, 0x7);
  V1 = __vpermwi (Q0, 0x9B);
  V0 = __vmulfp (V0, V1);

  V1 = __vspltw (q, 3);
  V2 = __vpermwi (Q0, 0x63);
  V1 = __vmulfp (V1, V2);

  R1 = __vaddfp (V0, V1);
  R2 = __vsubfp (V0, V1);

  r0 = __vrlimi (r0, ZO, 1, 3);
  r1 = __vpermwi (r0, 0x7);

  V0 = __vpermwi (R1, 0x42);
  r2 = __vsldoi (R2, R1, 2 << 2);
  V0 = __vrlimi (V0, R2, 0x6, 3);
  r2 = __vpermwi (r2, 0x88);

  r2 = __vrlimi (r2, r0, 0x3, 0);
  r1 = __vmrglw (V0, r1);
  r0 = __vrlimi (r0, V0, 0x6, 3);

#elif defined(NM_HOST_CELL_SPU) && !defined(NMP_VPU_EMULATION)

  vector4_t Q0, Q1;
  vector4_t V0, V1, V2;
  vector4_t R1, R2;
  const vector4_t ZO = { 3.0f, 3.0f, 0.0f, 1.0f };
  const vector4_t Constant1110 = { 1.0f, 1.0f, 1.0f, 0.0f };
  const vector4_t zero = (vector4_t) (0.0f);

  Q0 = add4f(q, q);
  Q1 = madd4f(q, Q0, zero);

  V0 = spu_shuffle (Q1, Q1, VPERM4(VPERM_Y, VPERM_X, VPERM_X, VPERM_X)); // V0 = Q1.yxxx
  V1 = spu_shuffle (Q1, Q1, VPERM4(VPERM_Z, VPERM_Z, VPERM_Y, VPERM_X)); // V1 = Q1.zzyx

  r0 = sub4f(Constant1110, V0);
  r0 = sub4f(r0, V1);

  V0 = spu_shuffle (q, q, VPERM4(VPERM_X, VPERM_X, VPERM_Y, VPERM_W));  // V0 = q.xxyw
  V1 = spu_shuffle (Q0, Q0, VPERM4(VPERM_Z, VPERM_Y, VPERM_Z, VPERM_W));          // V1 = Q0.zyzw
  V0 = madd4f(V0, V1, zero);

  V1 = splatW(q);      // V1 = q.wwww
  V2 = spu_shuffle (Q0, Q0, VPERM4(VPERM_Y, VPERM_Z, VPERM_X, VPERM_W)); // V2 = Q0.yzxw
  V1 = madd4f(V1, V2, zero);

  R1 = add4f(V0, V1);
  R2 = sub4f(V0, V1);

  r0 = spu_shuffle (r0, ZO, VPERM4(VPERM_X, VPERM_Y, VPERM_Z, VPERM_C)); // r0.w = 0.0f;
  r1 = spu_shuffle (r0, r0, VPERM4(VPERM_X, VPERM_X, VPERM_Y, VPERM_W)); // r1 = r0.xxyw

  V0 = spu_shuffle (R1, R1, VPERM4(VPERM_Y, VPERM_X, VPERM_X, VPERM_Z));  // V0 = R1.yxxz
  r2 = spu_shuffle (R2, R1, VPERM4(VPERM_Z, VPERM_W, VPERM_A, VPERM_B));   // r2 = R2.z | R2.w | R1.x | R1.y
  V0 = spu_shuffle (V0, R2, VPERM4(VPERM_X, VPERM_A, VPERM_B, VPERM_W));    // V0.yz = R2.xy
  r2 = spu_shuffle (r2, r2, VPERM4(VPERM_Z, VPERM_X, VPERM_Z, VPERM_X)); // r2 = r2.zxzx

  r2 = spu_shuffle (r2, r0, VPERM4(VPERM_X, VPERM_Y, VPERM_C, VPERM_D)); // r2.zw = r0.zw
  r1 = spu_shuffle (V0, r1, VPERM4(VPERM_Z, VPERM_C, VPERM_W, VPERM_D));
  r0 = spu_shuffle (r0, V0, VPERM4(VPERM_X, VPERM_A, VPERM_B, VPERM_W));   // r0.yz = V0.xy

#elif defined(NM_HOST_CELL_PPU) && !defined(NMP_VPU_EMULATION)

  vector4_t Q0, Q1;
  vector4_t V0, V1, V2;
  vector4_t R1, R2;
  const vector4_t ZO = { 3.0f, 3.0f, 0.0f, 1.0f };
  const vector4_t Constant1110 = { 1.0f, 1.0f, 1.0f, 0.0f };
  const vector4_t zero = (vector4_t) (0.0f);

  Q0 = add4f(q, q);
  Q1 = madd4f(q, Q0, zero);

  V0 = vec_perm(Q1, Q1, VPERM4(VPERM_Y, VPERM_X, VPERM_X, VPERM_X)); // V0 = Q1.yxxx
  V1 = vec_perm(Q1, Q1, VPERM4(VPERM_Z, VPERM_Z, VPERM_Y, VPERM_X)); // V1 = Q1.zzyx

  r0 = sub4f(Constant1110, V0);
  r0 = sub4f(r0, V1);

  V0 = vec_perm(q, q, VPERM4(VPERM_X, VPERM_X, VPERM_Y, VPERM_W));  // V0 = q.xxyw
  V1 = vec_perm(Q0, Q0, VPERM4(VPERM_Z, VPERM_Y, VPERM_Z, VPERM_W));          // V1 = Q0.zyzw
  V0 = madd4f(V0, V1, zero);

  V1 = splatW(q);      // V1 = q.wwww
  V2 = vec_perm(Q0, Q0, VPERM4(VPERM_Y, VPERM_Z, VPERM_X, VPERM_W)); // V2 = Q0.yzxw
  V1 = madd4f(V1, V2, zero);

  R1 = add4f(V0, V1);
  R2 = sub4f(V0, V1);

  r0 = vec_perm(r0, ZO, VPERM4(VPERM_X, VPERM_Y, VPERM_Z, VPERM_C)); // r0.w = 0.0f;
  r1 = vec_perm(r0, r0, VPERM4(VPERM_X, VPERM_X, VPERM_Y, VPERM_W)); // r1 = r0.xxyw

  V0 = vec_perm(R1, R1, VPERM4(VPERM_Y, VPERM_X, VPERM_X, VPERM_Z));  // V0 = R1.yxxz
  r2 = vec_perm(R2, R1, VPERM4(VPERM_Z, VPERM_W, VPERM_A, VPERM_B));   // r2 = R2.z | R2.w | R1.x | R1.y
  V0 = vec_perm(V0, R2, VPERM4(VPERM_X, VPERM_A, VPERM_B, VPERM_W));    // V0.yz = R2.xy
  r2 = vec_perm(r2, r2, VPERM4(VPERM_Z, VPERM_X, VPERM_Z, VPERM_X)); // r2 = r2.zxzx

  r2 = vec_perm(r2, r0, VPERM4(VPERM_X, VPERM_Y, VPERM_C, VPERM_D)); // r2.zw = r0.zw
  r1 = vec_perm(V0, r1, VPERM4(VPERM_Z, VPERM_C, VPERM_W, VPERM_D));
  r0 = vec_perm(r0, V0, VPERM4(VPERM_X, VPERM_A, VPERM_B, VPERM_W));   // r0.yz = V0.xy

#elif defined(NM_HOST_ORBIS) && !defined(NMP_VPU_EMULATION)

  vector4_t Q0, Q1;
  vector4_t V0, V1, V2;
  vector4_t R1, R2;
  const vector4_t zero = zero4f();
  vector4_t Constant1110 = one4f();
  Constant1110 = setwf(Constant1110, zero);
  
  Q0 = add4f(q, q);
  Q1 = madd4f(q, Q0, zero);

  V0 = _sce_vectormath_shuf_ps(Q1.f4, Q1.f4, 1, 0, 0, 0); // V0 = Q1.yxxx
  V1 = _sce_vectormath_shuf_ps(Q1.f4, Q1.f4, 2, 2, 1, 0); // V1 = Q1.zzyx

  r0 = sub4f(Constant1110, V0);
  r0 = sub4f(r0, V1);

  V0 = _sce_vectormath_shuf_ps(q.f4, q.f4, 0, 0, 1, 3);   // V0 = q.xxyw
  V1 = _sce_vectormath_shuf_ps(Q0.f4, Q0.f4, 2, 1, 2, 3); // V1 = Q0.zyzw
  V0 = madd4f(V0, V1, zero);

  V1 = splatW(q);      // V1 = q.wwww
  V2 = _sce_vectormath_shuf_ps(Q0.f4, Q0.f4, 1, 2, 0, 3); // V2 = Q0.yzxw
  V1 = madd4f(V1, V2, zero);

  R1 = add4f(V0, V1);
  R2 = sub4f(V0, V1);

  r0 = setwf(r0, zero); // r0.w = 0.0f;
  r1 = _sce_vectormath_shuf_ps(r0.f4, r0.f4, 0, 0, 1, 3); // r1 = r0.xxyw

  V0 = _sce_vectormath_shuf_ps(R1.f4, R1.f4, 1, 0, 0, 2); // V0 = R1.yxxz
  r2 = _sce_vectormath_shuf_ps(R2.f4, R1.f4, 2, 3, 0, 1); // r2 = R2.z | R2.w | R1.x | R1.y  
  V0 = _sce_vectormath_shuf_ps(V0.f4, R2.f4, 3, 0, 0, 1 ); // WXAB
  V0 = rotL4(V0); // XABW - V0.yz = R2.xy
  r2 = _sce_vectormath_shuf_ps(r2.f4, r2.f4, 2, 0, 2, 0); // r2 = r2.zxzx

  r2 = _sce_vectormath_shuf_ps(r2.f4, r0.f4, 0, 1, 2, 3); // r2.zw = r0.zw
  r1 = _sce_vectormath_shuf_ps(V0.f4, r1.f4, 2, 3, 2, 3); // ZWCD
  r1 = swapYZ(r1); // ZCWD
  r0 = _sce_vectormath_shuf_ps(r0.f4, V0.f4, 3, 0, 0, 1); // WXAB
  r0 = rotL4(r0); // XABW - r0.yz = V0.xy

#else

  vector4_t M1x, M1y, M1z, M1w;
  vector4_t M2x, M2y, M2z, M2w;

  buildQMP(q,
           M1x, M1y, M1z, M1w,
           M2x, M2y, M2z, M2w);

  r0 = add4f(add4f(add4f(mul4f(M2x, splatX(M1x)), mul4f(M2y, splatY(M1x))), mul4f(M2z, splatZ(M1x))), mul4f(M2w, splatW(M1x)));
  r1 = add4f(add4f(add4f(mul4f(M2x, splatX(M1y)), mul4f(M2y, splatY(M1y))), mul4f(M2z, splatZ(M1y))), mul4f(M2w, splatW(M1y)));
  r2 = add4f(add4f(add4f(mul4f(M2x, splatX(M1z)), mul4f(M2y, splatY(M1z))), mul4f(M2z, splatZ(M1z))), mul4f(M2w, splatW(M1z)));

#endif
}

//----------------------------------------------------------------------------------------------------------------------

/// \ingroup NaturalMotionVPUFX
/// \brief Generate a TM (NMP::Matrix34 format) from a NMP Quat and a Pos.
NM_FORCEINLINE void tmFromQuatPos(float* TM, const NMP::Quat& quat, const NMP::Vector3& pos)
{
  vector4_t r0, r1, r2;

  vector4_t vq = load4f((float*)&quat);
  vector4_t vp = load4f((float*)&pos);

  rotFromQuat(r0, r1, r2, vq);

  store4v(TM, r0, r1, r2, vp);
}

//----------------------------------------------------------------------------------------------------------------------
