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
#define NMP_VPU_CACHESIZE 32
//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void prefetchCacheLine(const void* _addr_, int _offset_)
{
  (void)_offset_;
  (void)_addr_;
  //_mm_prefetch((((char *)_addr_) + _offset_), _MM_HINT_NTA);
}

//----------------------------------------------------------------------------------------------------------------------

#define m128i(v) *(__m128i*)&(v)
#define m128(v)  *(__m128*)&(v)

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE __m128 c2int4f(const __m128 vec)
{
  __m128i ivec = _mm_cvttps_epi32(vec);
  return m128(ivec);
}

NM_FORCEINLINE __m128 c2float4i(const __m128 vec)
{
  return _mm_cvtepi32_ps(m128i(vec));
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE __m128 add4i(const __m128 val1, const __m128 val2)
{
  __m128i ivec = _mm_add_epi32(m128i(val1), m128i(val2));
  return m128(ivec);
}

NM_FORCEINLINE __m128 sub4i(const __m128 val1, const __m128 val2)
{
  __m128i ivec = _mm_sub_epi32(m128i(val1), m128i(val2));
  return m128(ivec);
}

NM_FORCEINLINE __m128 set4i(const int u)
{ 
  return _mm_set1_ps(*(float*)&u);
}

NM_FORCEINLINE __m128 set4i(const int x, const int y, const int z, const int w)
{
  __m128i ivec = _mm_set_epi32(w, z, y, x);
  return m128(ivec);
}

NM_FORCEINLINE __m128 sel4cmpiLT(const __m128& vecA, const __m128& vecB, const __m128& selTrue, const __m128& selFalse)
{
  __m128i cmplt = _mm_cmplt_epi32(m128i(vecA), m128i(vecB));
  return _mm_or_ps(_mm_and_ps(m128(cmplt), selTrue), _mm_andnot_ps(m128(cmplt), selFalse));
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE __m128 and4(const __m128 vecA, const __m128 vecB)
{
  return _mm_and_ps(vecA, vecB);
}

NM_FORCEINLINE __m128 or4(const __m128 vecA, const __m128 vecB)
{
  return _mm_or_ps(vecA, vecB);
}

NM_FORCEINLINE __m128 xor4(const __m128 vecA, const __m128 vecB)
{
  return _mm_xor_ps(vecA, vecB);
}

NM_FORCEINLINE __m128 not4(const __m128 vec) {
  return _mm_andnot_ps(vec, mask4i());
}

NM_FORCEINLINE __m128 shr4im16(const __m128 val)
{
  __m128i vec = _mm_srli_epi32(m128i(val), 16);
  return m128(vec);
}

NM_FORCEINLINE __m128 shr4im8(const __m128 val)
{
  __m128i vec = _mm_srli_epi32(m128i(val), 8);
  return m128(vec);
}

NM_FORCEINLINE __m128 shr4(const __m128 val, const __m128 vshr)
{
  __m128i vec;
  vec.m128i_u32[0] = val.m128_u32[0] >> vshr.m128_u32[0];
  vec.m128i_u32[1] = val.m128_u32[1] >> vshr.m128_u32[1];
  vec.m128i_u32[2] = val.m128_u32[2] >> vshr.m128_u32[2];
  vec.m128i_u32[3] = val.m128_u32[3] >> vshr.m128_u32[3];
  return m128(vec);
}

NM_FORCEINLINE __m128 shl4im16(const __m128 val)
{
  __m128i vec = _mm_slli_epi32(m128i(val), 16);
  return m128(vec);
}

NM_FORCEINLINE __m128 shl4im8(const __m128 val)
{
  __m128i vec = _mm_slli_epi32(m128i(val), 8);
  return m128(vec);
}

NM_FORCEINLINE __m128 shl4(const __m128 val, const __m128 vshl)
{
  __m128i vec;
  vec.m128i_u32[0] = val.m128_u32[0] << vshl.m128_u32[0];
  vec.m128i_u32[1] = val.m128_u32[1] << vshl.m128_u32[1];
  vec.m128i_u32[2] = val.m128_u32[2] << vshl.m128_u32[2];
  vec.m128i_u32[3] = val.m128_u32[3] << vshl.m128_u32[3];
  return m128(vec);
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE __m128 abs4f(const __m128 val) {
  const __m128 signMask = set4i(0x80000000);
  return _mm_andnot_ps(signMask, val);
}
NM_FORCEINLINE __m128 neg4f(const __m128 val) { return _mm_sub_ps(_mm_setzero_ps(), val); }
NM_FORCEINLINE __m128 neg4f(const __m128 val, const __m128 mask) {
  const __m128 signMask = set4i(0x80000000);
  return _mm_xor_ps(_mm_and_ps(signMask, mask), val);
}
NM_FORCEINLINE __m128 add4f(const __m128 val1, const __m128 val2) { return _mm_add_ps(val1, val2); }
NM_FORCEINLINE __m128 sub4f(const __m128 val1, const __m128 val2) { return _mm_sub_ps(val1, val2); }
NM_FORCEINLINE __m128 mul4f(const __m128 val1, const __m128 val2) { return _mm_mul_ps(val1, val2); }
NM_FORCEINLINE __m128 madd4f(const __m128 val1, const __m128 val2, const __m128 vadd) { return _mm_add_ps(_mm_mul_ps(val1, val2), vadd); }

NM_FORCEINLINE __m128 sqrt4f(const __m128 val) { return _mm_sqrt_ps(val); }

NM_FORCEINLINE __m128 rcp4f(const __m128 val)  {
  __m128 r = _mm_rcp_ps(val);
  return _mm_sub_ps(_mm_add_ps(r, r), _mm_mul_ps(_mm_mul_ps(r, val), r));
}

NM_FORCEINLINE __m128 rsqrt4f(const __m128 val)
{
  const __m128 mhalf = _mm_set_ps1(0.5f);
  const __m128 mthree = _mm_set_ps1(3.0f);

  __m128 rsqrt = _mm_rsqrt_ps(val);
  __m128 asqrt = _mm_mul_ps(val, rsqrt);
  __m128 hrsqt = _mm_mul_ps(rsqrt, mhalf);
  __m128 sval  = _mm_sub_ps(mthree, _mm_mul_ps(asqrt, rsqrt));

  return _mm_mul_ps(hrsqt, sval); // (3-rsa*(val*rsa))*(rsa*0.5)
}

// for versatility, the user is responsible for abs(vec)
NM_FORCEINLINE __m128 isBound4f(const __m128 vec, const __m128 tol)
{
  __m128 cmplt = _mm_cmplt_ps(vec, tol);
  __m128 tmp = _mm_and_ps(cmplt, _mm_shuffle_ps(cmplt, cmplt, _MM_SHUFFLE(2, 3, 0, 1)));
  return _mm_and_ps(tmp, _mm_shuffle_ps(tmp, tmp, _MM_SHUFFLE(1, 0, 3, 2)));
}

// for versatility, the user is responsible for abs(vec)
NM_FORCEINLINE __m128 isBound3f(const __m128 vec, const __m128 tol)
{
  __m128 cmplt = _mm_cmplt_ps(vec, tol);
  __m128 tmp = _mm_and_ps(cmplt, _mm_shuffle_ps(cmplt, cmplt, _MM_SHUFFLE(3, 0, 2, 1)));
  return _mm_and_ps(tmp, _mm_shuffle_ps(cmplt, cmplt, _MM_SHUFFLE(3, 1, 0, 2)));
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE __m128 mixLoHi(const __m128 vlo, const __m128 vhi) {
  return _mm_shuffle_ps(vlo, vhi, _MM_SHUFFLE(3, 2, 1, 0));
}

NM_FORCEINLINE __m128 rotL4(const __m128 v)
{
  return _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 3, 2, 1));
}
NM_FORCEINLINE __m128 rotR4(const __m128 v)
{
  return _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 1, 0, 3));
}

NM_FORCEINLINE __m128 swapLoHi(const __m128 v)
{
  return _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 0, 3, 2));
}

NM_FORCEINLINE __m128 reverse(const __m128 v)
{
  return _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 1, 2, 3));
}

NM_FORCEINLINE __m128 swapXY(const __m128 v) {
  return _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 2, 0, 1));
}

NM_FORCEINLINE __m128 swapZW(const __m128 v) {
  return _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 3, 1, 0));
}

NM_FORCEINLINE __m128 swapYZ(const __m128 v) {
  return _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 1, 2, 0));
}

NM_FORCEINLINE __m128 swapXW(const __m128 v) {
  return _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 2, 1, 3));
}

NM_FORCEINLINE __m128 swapYW(const __m128 v) {
  return _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 2, 3, 0));
}

NM_FORCEINLINE __m128 swapXZ(const __m128 v) {
  return _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 0, 1, 2));
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE __m128 splatX(const __m128 v) { return _mm_shuffle_ps(v, v, _MM_SHUFFLE(0, 0, 0, 0)); }
NM_FORCEINLINE __m128 splatY(const __m128 v) { return _mm_shuffle_ps(v, v, _MM_SHUFFLE(1, 1, 1, 1)); }
NM_FORCEINLINE __m128 splatZ(const __m128 v) { return _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 2, 2, 2)); }
NM_FORCEINLINE __m128 splatW(const __m128 v) { return _mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 3, 3, 3)); }

NM_FORCEINLINE uint32_t elemX(const __m128 v) { return v.m128_u32[0]; }
NM_FORCEINLINE uint32_t elemY(const __m128 v) { return v.m128_u32[1]; }
NM_FORCEINLINE uint32_t elemZ(const __m128 v) { return v.m128_u32[2]; }
NM_FORCEINLINE uint32_t elemW(const __m128 v) { return v.m128_u32[3]; }

NM_FORCEINLINE float floatX(const __m128 v) { return v.m128_f32[0]; }
NM_FORCEINLINE float floatY(const __m128 v) { return v.m128_f32[1]; }
NM_FORCEINLINE float floatZ(const __m128 v) { return v.m128_f32[2]; }
NM_FORCEINLINE float floatW(const __m128 v) { return v.m128_f32[3]; }

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE __m128 zero4f() { return _mm_setzero_ps(); }
NM_FORCEINLINE __m128 fltEpsilon4f() { return _mm_set_ps1(FLT_EPSILON); }
NM_FORCEINLINE __m128 one4f() { return _mm_set_ps1(1.0f); }
NM_FORCEINLINE __m128 quat4f() { return _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f); }
NM_FORCEINLINE __m128 negOne4f() { return _mm_set_ps1(-1.0f); }
NM_FORCEINLINE __m128 half4f() { return _mm_set_ps1(0.5f); }

NM_FORCEINLINE __m128 set4f(const float f) { return _mm_set_ps1(f); }
NM_FORCEINLINE __m128 set4f(const float x, const float y, const float z, const float w)
{
  return _mm_set_ps(w, z, y, x);
}

NM_FORCEINLINE __m128 setxf(const __m128 oyzw, const __m128 xooo)
{
  __m128 xooy = _mm_movelh_ps(xooo, oyzw);
  return _mm_shuffle_ps(xooy, oyzw, _MM_SHUFFLE(3, 2, 3, 0));
}

NM_FORCEINLINE __m128 setyf(const __m128 xozw, const __m128 oyoo)
{
  __m128 oyxo = _mm_movelh_ps(oyoo, xozw);
  return _mm_shuffle_ps(oyxo, xozw, _MM_SHUFFLE(3, 2, 1, 2));
}

NM_FORCEINLINE __m128 setzf(const __m128 xyow, const __m128 oozo)
{
  __m128 owzo = _mm_movehl_ps(oozo, xyow);
  return _mm_shuffle_ps(xyow, owzo, _MM_SHUFFLE(1, 2, 1, 0));
}

NM_FORCEINLINE __m128 setwf(const __m128 xyzo, const __m128 ooow)
{
  __m128 zoow = _mm_movehl_ps(ooow, xyzo);
  return _mm_shuffle_ps(xyzo, zoow, _MM_SHUFFLE(3, 0, 1, 0));
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE __m128 load1fp(float const* fp) { return _mm_load_ps1(fp); }
NM_FORCEINLINE __m128 load4f(float const* fp)  { return _mm_load_ps(fp); }
NM_FORCEINLINE __m128 loadu4f(float const* fp) { return _mm_loadu_ps(fp); }

NM_FORCEINLINE void store4f(float* fp, const __m128 v) { _mm_store_ps(fp, v); }

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE __m128 sum4f(const __m128 v)
{
  __m128 tmp = _mm_add_ps(v, _mm_shuffle_ps(v, v, _MM_SHUFFLE(2, 3, 0, 1)));
  return _mm_add_ps(_mm_shuffle_ps(tmp, tmp, _MM_SHUFFLE(1, 0, 3, 2)), tmp);
}

NM_FORCEINLINE __m128 dot4f(const __m128 a, const __m128 b)
{
  __m128 dot = _mm_mul_ps(a, b);
  __m128 tmp = _mm_add_ps(dot, _mm_shuffle_ps(dot, dot, _MM_SHUFFLE(2, 3, 0, 1)));
  return _mm_add_ps(_mm_shuffle_ps(tmp, tmp, _MM_SHUFFLE(1, 0, 3, 2)), tmp);
}

NM_FORCEINLINE __m128 dot3f(const __m128 a, const __m128 b)
{
  __m128 dot = _mm_mul_ps(a, b);
  __m128 tmp = _mm_add_ss(dot, _mm_shuffle_ps(dot, dot, 1));
  tmp = _mm_add_ss(_mm_movehl_ps(dot, dot), tmp);
  return _mm_shuffle_ps(tmp, tmp, 0);
}

NM_FORCEINLINE __m128 cross3f(const __m128 a, const __m128 b)
{
  __m128 crossV1 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1));
  __m128 crossV2 = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2));
  __m128 crossR = _mm_mul_ps(crossV1, crossV2);
  crossV1 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2));
  crossV2 = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1));
  return _mm_sub_ps(crossR, _mm_mul_ps(crossV1, crossV2));
}

NM_FORCEINLINE __m128 qqMul(const __m128 a, const __m128 b)
{
  __m128 ap1 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(0, 3, 3, 3));
  __m128 ap2 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(1, 0, 2, 1));
  __m128 ap3 = _mm_shuffle_ps(a, a, _MM_SHUFFLE(2, 1, 0, 2));

  __m128 bp0 = splatW(b);
  __m128 bp1 = setwf(b, neg4f(splatX(b)));
  __m128 bp2 = _mm_shuffle_ps(b, b, _MM_SHUFFLE(1, 1, 0, 2));
  __m128 bp3 = _mm_shuffle_ps(b, b, _MM_SHUFFLE(2, 0, 2, 1));
  bp2 = setwf(bp2, neg4f(bp2));

  return _mm_sub_ps(_mm_add_ps(_mm_mul_ps(ap2, bp2), _mm_add_ps(_mm_mul_ps(a, bp0), _mm_mul_ps(ap1, bp1))), _mm_mul_ps(ap3, bp3));
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE vector4_t sel4cmpEQ(const __m128& vecA, const __m128& vecB, const __m128& selTrue, const __m128& selFalse)
{
  __m128 cmpeq = _mm_cmpeq_ps(vecA, vecB);
  return _mm_or_ps(_mm_and_ps(cmpeq, selTrue), _mm_andnot_ps(cmpeq, selFalse));
}

NM_FORCEINLINE __m128 sel4cmpLT(const __m128& vecA, const __m128& vecB, const __m128& selTrue, const __m128& selFalse)
{
  __m128 cmplt = _mm_cmplt_ps(vecA, vecB);
  return _mm_or_ps(_mm_and_ps(cmplt, selTrue), _mm_andnot_ps(cmplt, selFalse));
}

NM_FORCEINLINE __m128 sel4cmp2LT(const __m128& vecA1, const __m128& vecA2, const __m128& vecB, const __m128& selTrue, const __m128& selFalse)
{
  __m128 cmplt = _mm_and_ps(_mm_cmplt_ps(vecA1, vecB), _mm_cmplt_ps(vecA2, vecB));
  return _mm_or_ps(_mm_and_ps(cmplt, selTrue), _mm_andnot_ps(cmplt, selFalse));
}

NM_FORCEINLINE __m128 mask4cmpLT(const __m128 vecA, const __m128 vecB)
{
  return _mm_cmplt_ps(vecA, vecB);
}

NM_FORCEINLINE __m128 mask4cmpEQ(const __m128 vecA, const __m128 vecB)
{
  return _mm_cmpeq_ps(vecA, vecB);
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE __m128 sel4cmpBits(const __m128& vecVal, const __m128& vecBits, const __m128& selTrue, const __m128& selFalse)
{
  __m128i cmpbits = _mm_cmpeq_epi32(m128i(vecBits), _mm_and_si128(m128i(vecVal), m128i(vecBits)));
  return _mm_or_ps(_mm_and_ps(m128(cmpbits), selTrue), _mm_andnot_ps(m128(cmpbits), selFalse));
}

NM_FORCEINLINE __m128 sel4cmpMask(const __m128& mask, const __m128& selTrue, const __m128& selFalse)
{
  return _mm_or_ps(_mm_and_ps(mask, selTrue), _mm_andnot_ps(mask, selFalse));
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE __m128 mask4i()
{
  return _mm_cmpeq_ps(_mm_setzero_ps(), _mm_setzero_ps());
}

NM_FORCEINLINE __m128 mask4i(const uint32_t mask)
{
  _MM_ALIGN16 static const uint32_t bmsk[4] = { 8, 4, 2, 1 };
  const __m128 selMask = _mm_load_ps((float*)&bmsk);

  __m128 bits = _mm_load_ss((float*)&mask);
  bits = _mm_shuffle_ps(bits, bits, 0);
  bits = _mm_and_ps(bits, selMask);

  // workaround for possible flushing of denormals to zero, replaces
  // return _mm_cmpneq_ps(bits, _mm_setzero_ps())
  // in this and the next two functions
  const __m128i proMask = _mm_cmpeq_epi32(m128i(bits), _mm_setzero_si128());
  return _mm_andnot_ps(m128(proMask), _mm_cmpeq_ps(_mm_setzero_ps(), _mm_setzero_ps()));
}

NM_FORCEINLINE __m128 maskr4i(const uint32_t mask)
{
  _MM_ALIGN16 static const uint32_t bmsk[4] = { 1, 2, 4, 8 };
  const __m128 selMask = _mm_load_ps((float*)&bmsk);

  __m128 bits = _mm_load_ss((float*)&mask);
  bits = _mm_shuffle_ps(bits, bits, 0);
  bits = _mm_and_ps(bits, selMask);

  const __m128i proMask = _mm_cmpeq_epi32(m128i(bits), _mm_setzero_si128());
  return _mm_andnot_ps(m128(proMask), _mm_cmpeq_ps(_mm_setzero_ps(), _mm_setzero_ps()));
}

NM_FORCEINLINE __m128 mask4i(const uint8_t mask0, const uint8_t mask1, const uint8_t mask2, const uint8_t mask3)
{
  const __m128i iZero = _mm_setzero_si128();
  const __m128i selMask = _mm_set_epi32(mask3, mask2, mask1, mask0);
  const __m128i proMask = _mm_cmpeq_epi32(selMask, iZero);
  const __m128i iMask = _mm_andnot_si128(proMask, _mm_cmpeq_epi32(iZero, iZero));
  return m128(iMask);
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void load4v(float const* data, __m128& v0, __m128& v1, __m128& v2, __m128& v3)
{
  v0 = _mm_load_ps(&data[0]);
  v1 = _mm_load_ps(&data[4]);
  v2 = _mm_load_ps(&data[8]);
  v3 = _mm_load_ps(&data[12]);
}

NM_FORCEINLINE void store4v(float* data, const __m128& v0, const __m128& v1, const __m128& v2, const __m128& v3)
{
  _mm_store_ps(&data[0],  v0);
  _mm_store_ps(&data[4],  v1);
  _mm_store_ps(&data[8],  v2);
  _mm_store_ps(&data[12], v3);
}

NM_FORCEINLINE void transpose4v(__m128& r1, __m128& r2, __m128& r3, __m128& r4)
{
  _MM_TRANSPOSE4_PS(r1, r2, r3, r4);
}

NM_FORCEINLINE void transpose4vConst(__m128& r1, __m128& r2, __m128& r3, __m128& r4,
                                     const __m128& c1, const __m128& c2, const __m128& c3, const __m128& c4)
{
  __m128 tmp3, tmp2, tmp1, tmp0;

  tmp0   = _mm_shuffle_ps(c1, c2, 0x44);
  tmp2   = _mm_shuffle_ps(c1, c2, 0xEE);
  tmp1   = _mm_shuffle_ps(c3, c4, 0x44);
  tmp3   = _mm_shuffle_ps(c3, c4, 0xEE);

  r1 = _mm_shuffle_ps(tmp0, tmp1, 0x88);
  r2 = _mm_shuffle_ps(tmp0, tmp1, 0xDD);
  r3 = _mm_shuffle_ps(tmp2, tmp3, 0x88);
  r4 = _mm_shuffle_ps(tmp2, tmp3, 0xDD);
}

NM_FORCEINLINE vector4_t horzadd4(const __m128& a, const __m128& b, const __m128& c, const __m128& d)
{
  // a0+a2 c0+c2 a1+a3 c1+c3
  __m128 s1 = _mm_add_ps(_mm_unpacklo_ps(a, c), _mm_unpackhi_ps(a, c));
  // b0+b2 d0+d2 b1+b3 d1+d3
  __m128 s2 = _mm_add_ps(_mm_unpacklo_ps(b, d), _mm_unpackhi_ps(b, d));
  // (a0+a2 b0+b2 c0+c2 d0+d2)+ (a1+a3 b1+b3 c1+c3 d1+d3)
  return _mm_add_ps(_mm_unpacklo_ps(s1, s2), _mm_unpackhi_ps(s1, s2));
}

NM_FORCEINLINE vector4_t horzadd3(const __m128& a, const __m128& b, const __m128& c)
{
  // a0+a2 c0+c2 a1+a3 c1+c3
  __m128 s1 = _mm_add_ps(_mm_unpacklo_ps(a, c), _mm_unpackhi_ps(a, c));
  // b0+b2 c0+c2 b1+b3 c1+c3
  __m128 s2 = _mm_add_ps(_mm_unpacklo_ps(b, c), _mm_unpackhi_ps(b, c));
  // (a0+a2 b0+b2 c0+c2 c0+c2)+ (a1+a3 b1+b3 c1+c3 c1+c3)
  return _mm_add_ps(_mm_unpacklo_ps(s1, s2), _mm_unpackhi_ps(s1, s2));
}

//----------------------------------------------------------------------------------------------------------------------

// note: row order
NM_FORCEINLINE void buildQMP(const __m128& quat,
                             __m128& M1x, __m128& M1y, __m128& M1z, __m128& M1w,
                             __m128& M2x, __m128& M2y, __m128& M2z, __m128& M2w)
{
  M2w = quat;
  M1w = setwf(neg4f(quat), quat);                               // (-x, -y, -z, w)

  __m128 xyv = _mm_shuffle_ps(quat, M1w, _MM_SHUFFLE(1, 0, 1, 0)); // (x, y, -x, -y)

  M2x = _mm_shuffle_ps(quat, M1w, _MM_SHUFFLE(0, 1, 2, 3)); // (w, z, -y, -x)
  M1y = _mm_shuffle_ps(M1w, quat, _MM_SHUFFLE(1, 0, 3, 2)); // (-z, w, x, y)
  M1x = _mm_shuffle_ps(quat, xyv, _MM_SHUFFLE(0, 3, 2, 3)); // (w, z, -y, x)
  M1z = _mm_shuffle_ps(xyv, quat, _MM_SHUFFLE(2, 3, 2, 1)); // (y, -x, w, z)
  M2y = _mm_shuffle_ps(M1w, xyv,  _MM_SHUFFLE(3, 0, 3, 2)); // (-z, w, x, -y)
  M2z = _mm_shuffle_ps(xyv, M1w,  _MM_SHUFFLE(2, 3, 2, 1)); // (y, -x, w, -z)
}

//----------------------------------------------------------------------------------------------------------------------
// Packed data reads:  unpack[u=unaligned][number][i16=unsigned int16][f=32bit float conversion]
//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE __m128 unpacku4i8(uint8_t const* data)
{
  const __m128i mZeroi = _mm_setzero_si128();
  __m128i pdi = _mm_loadu_si128((__m128i*)data);
  __m128i unpck = _mm_unpacklo_epi16(_mm_unpacklo_epi8(pdi, mZeroi), mZeroi);
  return m128(unpck);
}

NM_FORCEINLINE __m128 unpacku4i16(uint16_t const* data)
{
  const __m128i mZeroi = _mm_setzero_si128();
  __m128i pdi = _mm_loadu_si128((__m128i*)data);
  __m128i unpck = _mm_unpacklo_epi16(pdi, mZeroi);
  return m128(unpck);
}

NM_FORCEINLINE __m128 unpack4i16(uint16_t const* data)
{
  const __m128i mZeroi = _mm_setzero_si128();
  __m128i pdi = _mm_load_si128((__m128i*)data);
  __m128i unpck = _mm_unpacklo_epi16(pdi, mZeroi);
  return m128(unpck);
}

NM_FORCEINLINE __m128 unpacku4i16f(uint16_t const* data)
{
  __m128 unpck = unpacku4i16(data);
  return _mm_cvtepi32_ps(m128i(unpck));
}

NM_FORCEINLINE __m128 unpack4i16f(uint16_t const* data)
{
  __m128 unpck = unpack4i16(data);
  return _mm_cvtepi32_ps(m128i(unpck));
}

NM_FORCEINLINE void unpacku6i16f(__m128& r1, __m128& r2, uint16_t const* data)
{
  const __m128i mZeroi = _mm_setzero_si128();

  __m128i pdi  = _mm_loadu_si128((__m128i*)data);
  __m128i v4li = _mm_unpacklo_epi16(pdi, mZeroi);
  __m128i v4hi = _mm_unpackhi_epi16(_mm_slli_si128(pdi, 2), mZeroi);

  r1 = _mm_cvtepi32_ps(v4li);
  r2 = _mm_cvtepi32_ps(v4hi);
}

NM_FORCEINLINE void unpack8i16f(__m128& r1, __m128& r2, uint16_t const* data)
{
  const __m128i mZeroi = _mm_setzero_si128();

  __m128i pdi  = _mm_load_si128((__m128i*)data);
  __m128i v4li = _mm_unpacklo_epi16(pdi, mZeroi);
  __m128i v4hi = _mm_unpackhi_epi16(pdi, mZeroi);

  r1 = _mm_cvtepi32_ps(v4li);
  r2 = _mm_cvtepi32_ps(v4hi);
}

NM_FORCEINLINE void unpacku8i16f(__m128& r1, __m128& r2, uint16_t const* data)
{
  const __m128i mZeroi = _mm_setzero_si128();

  __m128i pdi  = _mm_loadu_si128((__m128i*)data);
  __m128i v4li = _mm_unpacklo_epi16(pdi, mZeroi);
  __m128i v4hi = _mm_unpackhi_epi16(pdi, mZeroi);

  r1 = _mm_cvtepi32_ps(v4li);
  r2 = _mm_cvtepi32_ps(v4hi);
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void unpack4QuantizedVectorsPacked(__m128& r1, __m128& r2, __m128& r3, uint32_t const* qVec)
{
  NMP_ASSERT(NMP_IS_ALIGNED(qVec, 16));
  __m128i V = _mm_load_si128((__m128i*)qVec);

  __m128 Mask = set4i(0x07FF);
  __m128i r1i = _mm_srli_epi32(V, 0x015);
  r1i = _mm_and_si128(r1i, m128i(Mask));
  r1 = _mm_cvtepi32_ps(r1i);

  __m128i r2i = _mm_srli_epi32(V, 0x0A);
  r2i = _mm_and_si128(r2i, m128i(Mask));
  r2 = _mm_cvtepi32_ps(r2i);

  Mask  = set4i(0x03FF);
  __m128i r3i = _mm_and_si128(V, m128i(Mask));
  r3 = _mm_cvtepi32_ps(r3i);
}

NM_FORCEINLINE void unpack2QuantizedVectors(__m128& r1, __m128& r2, uint32_t const* qVec)
{
  _MM_ALIGN16 uint16_t tdata[8] = { (uint16_t)((qVec[0] >> 21) & 2047),
                                    (uint16_t)((qVec[0] >> 10) & 2047),
                                    (uint16_t)((qVec[0] >> 0) & 1023), 0,
                                    (uint16_t)((qVec[1] >> 21) & 2047),
                                    (uint16_t)((qVec[1] >> 10) & 2047),
                                    (uint16_t)((qVec[1] >> 0) & 1023), 0
                                  };
  unpack8i16f(r1, r2, tdata);  
}

NM_FORCEINLINE __m128 unpackQuantizedVectorData(uint32_t const* qVec)
{
  _MM_ALIGN16 uint16_t tdata[4] = { (uint16_t)((*qVec >> 21) & 2047),
                                    (uint16_t)((*qVec >> 10) & 2047),
                                    (uint16_t)((*qVec >> 0) & 1023), 0
                                  };
  return unpack4i16(tdata);
}

NM_FORCEINLINE __m128 unpackQuantizedVector(uint32_t const* qVec)
{
  __m128 unpck = unpackQuantizedVectorData(qVec);
  return _mm_cvtepi32_ps(m128i(unpck));
}

//----------------------------------------------------------------------------------------------------------------------
