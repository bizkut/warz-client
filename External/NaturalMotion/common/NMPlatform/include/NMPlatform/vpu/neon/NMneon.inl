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
#define NMP_VPU_CACHESIZE 128

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float32x4_t ExtractFloat_2(float32x4_t v, uint32_t slot)
{
  switch(slot)
  {
  case 0:
    return vdupq_lane_f32(vget_low_f32(v), 0);
  case 1:
    return vdupq_lane_f32(vget_low_f32(v), 1);
  case 2:
    return vdupq_lane_f32(vget_high_f32(v), 0);
  default:
    return vdupq_lane_f32(vget_high_f32(v), 1);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE uint32x4_t ExtractUInt_2(uint32x4_t v, uint32_t slot)
{
  switch(slot)
  {
  case 0:
    return vdupq_lane_u32(vget_low_u32(v), 0);
  case 1:
    return vdupq_lane_u32(vget_low_u32(v), 1);
  case 2:
    return vdupq_lane_u32(vget_high_u32(v), 0);
  default:
    return vdupq_lane_u32(vget_high_u32(v), 1);
  }
}

#define SPLAT_FLOAT(xxxx, yyyy, zzzz, wwww) \
  vcombine_f32(vget_low_f32(vextq_f32(xxxx, yyyy, 3)), vget_low_f32(vextq_f32(zzzz, wwww, 3)))

#define SPLAT_UINT(xxxx, yyyy, zzzz, wwww) \
  vcombine_u32(vget_low_u32(vextq_u32(xxxx, yyyy, 3)), vget_low_u32(vextq_u32(zzzz, wwww, 3)))

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float32x4_t SwizzleF4(float32x4_t vec, uint32_t x, uint32_t y, uint32_t z, uint32_t w)
{
  float32x4_t _x = ExtractFloat_2(vec, x);
  float32x4_t _y = ExtractFloat_2(vec, y);
  float32x4_t _z = ExtractFloat_2(vec, z);
  float32x4_t _w = ExtractFloat_2(vec, w);
  return SPLAT_FLOAT(_x, _y, _z, _w);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float32x4_t SwizzleSPLITF4(float32x4_t vec, float32x4_t vec2, uint32_t x, uint32_t y, uint32_t z, uint32_t w)
{
  float32x4_t _x = ExtractFloat_2(vec, x);
  float32x4_t _y = ExtractFloat_2(vec, y);
  float32x4_t _z = ExtractFloat_2(vec2, z);
  float32x4_t _w = ExtractFloat_2(vec2, w);
  return SPLAT_FLOAT(_x, _y, _z, _w);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float32x4_t Swizzle4(float32x4_t vec1, float32x4_t vec2, float32x4_t vec3, float32x4_t vec4, uint32_t x, uint32_t y, uint32_t z, uint32_t w)
{
  float32x4_t _x = ExtractFloat_2(vec1, x);
  float32x4_t _y = ExtractFloat_2(vec2, y);
  float32x4_t _z = ExtractFloat_2(vec3, z);
  float32x4_t _w = ExtractFloat_2(vec4, w);
  return SPLAT_FLOAT(_x, _y, _z, _w);
}


//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE uint32x4_t SwizzleU4(uint32x4_t vec, uint32_t x, uint32_t y, uint32_t z, uint32_t w)
{
  uint32x4_t _x = ExtractUInt_2(vec, x);
  uint32x4_t _y = ExtractUInt_2(vec, y);
  uint32x4_t _z = ExtractUInt_2(vec, z);
  uint32x4_t _w = ExtractUInt_2(vec, w);
  return SPLAT_UINT(_x, _y, _z, _w);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void prefetchCacheLine(const void* addr, int offset)
{
  (void)addr;
  (void)offset;
} 

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t c2int4f(const vector4_t vec)
{
  vector4_t res;
  res.i = vcvtq_s32_f32(vec.f);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t c2float4i(const vector4_t vec)
{

  vector4_t res;
  res.f = vcvtq_f32_s32(vec.i);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t add4i(const vector4_t val1, const vector4_t val2)
{
  vector4_t res;
  res.i = vaddq_s32(val1.i, val2.i);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t sub4i(const vector4_t val1, const vector4_t val2)
{ 
  vector4_t res;
  res.i = vsubq_s32(val1.i, val2.i);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t set4i(const int u)
{
  vector4_t res;
  res.i = vdupq_n_s32(u);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t set4i(const int x, const int y, const int z, const int w)
{
  vector4_t res;
  res.i = (int32x4_t){x, y, z, w};
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t sel4cmpiLT(const vector4_t& vecA, const vector4_t& vecB, const vector4_t& selTrue, const vector4_t& selFalse)
{
  uint32x4_t mod = vcltq_s32(vecA.i, vecB.i);
  vector4_t res;
  uint32x4_t trueComponent = vandq_u32(selTrue.u, mod);
  uint32x4_t falseComponent = vbicq_u32(selFalse.u, mod);
  res.u = vorrq_u32(trueComponent, falseComponent);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t and4(const vector4_t vecA, const vector4_t vecB)
{
  vector4_t res;
  res.u = vandq_u32(vecA.u, vecB.u);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t or4(const vector4_t vecA, const vector4_t vecB)
{
  vector4_t res;
  res.u = vorrq_u32(vecA.u, vecB.u);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t xor4(const vector4_t vecA, const vector4_t vecB)
{
  vector4_t res;
  res.u = veorq_u32(vecA.u, vecB.u);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t not4(const vector4_t vec)
{
  vector4_t res;
  res.u = vmvnq_u32(vec.u);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t shr4im16(const vector4_t val)
{
  vector4_t res;
  res.u = vshrq_n_u32(val.u, 16);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t shr4im8(const vector4_t val)
{
  vector4_t res;
  res.u = vshrq_n_u32(val.u, 8);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t shr4(const vector4_t val, const vector4_t vshr)
{
  vector4_t res;
  res.u = vshlq_u32(val.u, vnegq_s32(vshr.i));
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t shl4im16(const vector4_t val)
{
  vector4_t res;
  res.u = vshlq_n_u32(val.u, 16);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t shl4im8(const vector4_t val)
{
  vector4_t res;
  res.u = vshlq_n_u32(val.u, 8);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t shl4(const vector4_t val, const vector4_t vshl)
{
  vector4_t res;
  res.u = vshlq_u32(val.u, vshl.i);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t abs4f(const vector4_t val)
{
  vector4_t res;
  res.f = vabsq_f32(val.f);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t neg4f(const vector4_t val)
{
  vector4_t res;
  res.f = vnegq_f32(val.f);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t neg4f(const vector4_t val, const vector4_t mask)
{
  uint32x4_t signMask = vdupq_n_u32(0x80000000);
 
  vector4_t res;
  res.u = veorq_u32(vandq_u32(signMask, mask.u), val.u);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t add4f(const vector4_t val1, const vector4_t val2)
{
  vector4_t res;
  res.f = vaddq_f32(val1.f, val2.f);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t sub4f(const vector4_t val1, const vector4_t val2)
{ 
  vector4_t res;
  res.f = vsubq_f32(val1.f, val2.f);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t mul4f(const vector4_t val1, const vector4_t val2)
{
  vector4_t res;
  res.f = vmulq_f32(val1.f, val2.f);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t madd4f(const vector4_t val1, const vector4_t val2, const vector4_t vadd)
{ 
  vector4_t res;
  res.f = vmlaq_f32(vadd.f, val1.f, val2.f);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t sqrt4f(const vector4_t val)
{
  uint32x4_t zero = set4i(0).u;
  float32x4_t estimate = vrsqrteq_f32(val.f);
  estimate = vmulq_f32(estimate, vrsqrtsq_f32(vmulq_f32(estimate, estimate), val.f));
  estimate = vmulq_f32(estimate, vrsqrtsq_f32(vmulq_f32(estimate, estimate), val.f));
  estimate = vmulq_f32(val.f, estimate);
  estimate = vreinterpretq_f32_u32(vbicq_u32(vreinterpretq_u32_f32(estimate), zero));

  vector4_t res;
  res.f = estimate;
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t rcp4f(const vector4_t val)
{
  vector4_t res;

  res.f = vrecpeq_f32(val.f);
  res.f = vmulq_f32(res.f, vrecpsq_f32(res.f, val.f));
  res.f = vmulq_f32(res.f, vrecpsq_f32(res.f, val.f));
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t rsqrt4f(const vector4_t val)
{
  vector4_t res;
  res.f = vrsqrteq_f32(val.f);
  res.f = vmulq_f32(res.f, vrsqrtsq_f32(vmulq_f32(res.f, res.f), val.f));
  res.f = vmulq_f32(res.f, vrsqrtsq_f32(vmulq_f32(res.f, res.f), val.f));
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t isBound4f(const vector4_t vec, const vector4_t tol)
{
  // Do the comparison on all elements
  uint32x4_t cmplt = vcltq_f32(vec.f, tol.f);

  // Check each element against each other and return the result
  uint32x4_t tmpA = vrev64q_u32(cmplt);
  uint32x4_t andResA = vandq_u32(cmplt, tmpA);
  uint32x4_t tmpB = vcombine_u32(vget_high_u32(andResA), vget_low_u32(andResA));

  vector4_t res;
  res.u = vandq_u32(andResA, tmpB);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t isBound3f(const vector4_t vec, const vector4_t tol)
{
  float32x4_t vecMod = vsetq_lane_f32(-FLT_MAX, vec.f, 3);

  // Do the comparison on all elements
  uint32x4_t cmplt = vcltq_f32(vecMod, tol.f);

  // Check each element against each other and return the result
  uint32x4_t tmpA = vrev64q_u32(cmplt);
  uint32x4_t andResA = vandq_u32(cmplt, tmpA);
  uint32x4_t tmpB = vcombine_u32(vget_high_u32(andResA), vget_low_u32(andResA));

  vector4_t res;
  res.u = vandq_u32(andResA, tmpB);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t mixLoHi(const vector4_t vlo, const vector4_t vhi)
{
  vector4_t res;
  res.u = vcombine_u32(vget_low_u32(vlo.u), vget_high_u32(vhi.u));
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t rotL4(const vector4_t v)
{
  vector4_t res;
  res.u = SwizzleU4(v.u, 1, 2, 3, 0);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t rotR4(const vector4_t v)
{
  vector4_t res;
  res.u = SwizzleU4(v.u, 3, 0, 1, 2);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t swapLoHi(const vector4_t v)
{
  vector4_t res;
  res.u = SwizzleU4(v.u, 2, 3, 0, 1);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t reverse(const vector4_t v)
{
  vector4_t res;
  res.u = SwizzleU4(v.u, 3, 2, 1, 0);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t swapXY(const vector4_t v)
{
  vector4_t res;
  res.u = SwizzleU4(v.u, 1, 0, 2, 3);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t swapZW(const vector4_t v)
{
  vector4_t res;
  res.u = SwizzleU4(v.u, 0, 1, 3, 2);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t swapYZ(const vector4_t v)
{
  vector4_t res;
  res.u = SwizzleU4(v.u, 0, 2, 1, 3);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t swapXW(const vector4_t v)
{
  vector4_t res;
  res.u = SwizzleU4(v.u, 3, 1, 2, 0);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t swapYW(const vector4_t v)
{
  vector4_t res;
  res.u = SwizzleU4(v.u, 0, 3, 2, 1);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t swapXZ(const vector4_t v)
{
  vector4_t res;
  res.u = SwizzleU4(v.u, 2, 1, 0, 3);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t splatX(const vector4_t v)
{ 
  vector4_t res;
  res.u = vdupq_lane_u32(vget_low_u32(v.u), 0);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t splatY(const vector4_t v)
{
  vector4_t res;
  res.u = vdupq_lane_u32(vget_low_u32(v.u), 1);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t splatZ(const vector4_t v)
{ 
  vector4_t res;
  res.u = vdupq_lane_u32(vget_high_u32(v.u), 0);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t splatW(const vector4_t v)
{
  vector4_t res;
  res.u = vdupq_lane_u32(vget_high_u32(v.u), 1);
  return res;
}

NM_FORCEINLINE uint32_t elemX(const vector4_t v) { return vgetq_lane_u32(v.u, 0); }
NM_FORCEINLINE uint32_t elemY(const vector4_t v) { return vgetq_lane_u32(v.u, 1); }
NM_FORCEINLINE uint32_t elemZ(const vector4_t v) { return vgetq_lane_u32(v.u, 2); }
NM_FORCEINLINE uint32_t elemW(const vector4_t v) { return vgetq_lane_u32(v.u, 3); }

NM_FORCEINLINE float floatX(const vector4_t v) { return vgetq_lane_f32(v.f, 0); }
NM_FORCEINLINE float floatY(const vector4_t v) { return vgetq_lane_f32(v.f, 1); }
NM_FORCEINLINE float floatZ(const vector4_t v) { return vgetq_lane_f32(v.f, 2); }
NM_FORCEINLINE float floatW(const vector4_t v) { return vgetq_lane_f32(v.f, 3); }

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t zero4f()
{
  vector4_t res;
  res.f = vmovq_n_f32(0.0f);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t one4f()
{
  vector4_t res;
  res.f = vmovq_n_f32(1.0f);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t quat4f()
{
  vector4_t res;
  res.f = (float32x4_t){0.0f, 0.0f, 0.0f, 1.0f};
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t negOne4f()
{
  vector4_t res;
  res.f = vmovq_n_f32(-1.0f);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t half4f()
{
  vector4_t res;
  res.f = vmovq_n_f32(0.5f);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t fltEpsilon4f()
{
  return set4f(FLT_EPSILON);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t set4f(const float f)
{
  vector4_t res;
  res.f = vdupq_n_f32(f);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t set4f(const float x, const float y, const float z, const float w)
{
  vector4_t res;
  res.f = (float32x4_t){x, y, z, w};
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t setxf(const vector4_t d, const vector4_t s)
{
  vector4_t res;
  res.f = vsetq_lane_f32(floatX(s), d.f, 0);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t setyf(const vector4_t d, const vector4_t s)
{
  vector4_t res;
  res.f = vsetq_lane_f32(floatY(s), d.f, 1);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t setzf(const vector4_t d, const vector4_t s)
{
  vector4_t res;
  res.f = vsetq_lane_f32(floatZ(s), d.f, 2);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t setwf(const vector4_t d, const vector4_t s)
{
  vector4_t res;
  res.f = vsetq_lane_f32(floatW(s), d.f, 3);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t load1fp(float const* fp)
{
  vector4_t res;
  res.f = vld1q_dup_f32((float32_t*)fp);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t load4f(float const* data)
{
  vector4_t res;
  res.f = vld1q_f32((float32_t*)data);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t loadu4f(float const* data)
{
  return load4f(data);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void store4f(float* data, const vector4_t val)
{
  vst1q_f32((float32_t*)data, val.f);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t sum4f(const vector4_t v)
{
  float32x2_t xy = vget_low_f32(v.f);
  float32x2_t zw = vget_high_f32(v.f);
  float32x2_t xPlusYZPlusW = vpadd_f32(xy, zw);
  float32x2_t xPlusYPlusZPlusW = vpadd_f32(xPlusYZPlusW, xPlusYZPlusW);

  vector4_t dot4;
  dot4.f = vdupq_lane_f32(xPlusYPlusZPlusW, 0);
  return dot4;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t dot4f(const vector4_t a, const vector4_t b)
{
  float32x4_t c = vmulq_f32(a.f, b.f);
  float32x2_t xy = vget_low_f32(c);
  float32x2_t zw = vget_high_f32(c);
  float32x2_t xPlusYZPlusW = vpadd_f32(xy, zw);
  float32x2_t xPlusYPlusZPlusW = vpadd_f32(xPlusYZPlusW, xPlusYZPlusW);

  vector4_t dot4;
  dot4.f = vdupq_lane_f32(xPlusYPlusZPlusW, 0);
  return dot4;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t dot3f(const vector4_t a, const vector4_t b)
{ 
  float32x4_t c = vmulq_f32(a.f, b.f);
  float32x2_t xy = vget_low_f32(c);
  float32x2_t zz = vdup_lane_f32(vget_high_f32(c), 0);
  float32x2_t xPlusY = vpadd_f32(xy, xy);
  float32x2_t xPlusYPlusZ = vadd_f32(xPlusY, zz);

  vector4_t dot3;
  dot3.f = vdupq_lane_f32(xPlusYPlusZ, 0);
  return dot3;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t cross3f(const vector4_t a, const vector4_t b)
{
  float32x4_t crossV1 = SwizzleF4(a.f, 1, 2, 0, 3);
  float32x4_t crossV2 = SwizzleF4(b.f, 2, 0, 1, 3);
  float32x4_t crossR = vmulq_f32(crossV1, crossV2);
  crossV1 = SwizzleF4(a.f, 2, 0, 1, 3);
  crossV2 = SwizzleF4(b.f, 1, 2, 0, 3);

  vector4_t res;
  res.f = vsubq_f32(crossR, vmulq_f32(crossV1, crossV2));
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t qqMul(const vector4_t a, const vector4_t b)
{
  float32x4_t ap1 = SwizzleF4(a.f, 3, 3, 3, 0);
  float32x4_t ap2 = SwizzleF4(a.f, 1, 2, 0, 1);
  float32x4_t ap3 = SwizzleF4(a.f, 2, 0, 1, 2);

  float32x4_t bp0 = splatW(b).f;
  float32x4_t bp1 = setwf(b, neg4f(splatX(b))).f;
  float32x4_t bp2 = SwizzleF4(b.f, 2, 0, 1, 1);
  float32x4_t bp3 = SwizzleF4(b.f, 1, 2, 0, 2);
  float temp = -vgetq_lane_f32(bp2, 3);
  bp2 = vsetq_lane_f32(temp, bp2, 3);

  vector4_t res;
  res.f = vsubq_f32(vaddq_f32(vmulq_f32(ap2, bp2), vaddq_f32(vmulq_f32(a.f, bp0), vmulq_f32(ap1, bp1))), vmulq_f32(ap3, bp3));
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t sel4cmpEQ(const vector4_t& vecA, const vector4_t& vecB, const vector4_t& selTrue, const vector4_t& selFalse)
{
  uint32x4_t mod = vceqq_f32(vecA.f, vecB.f);
  vector4_t res;
  uint32x4_t trueComponent = vandq_u32(selTrue.u, mod);
  uint32x4_t falseComponent = vbicq_u32(selFalse.u, mod);
  res.u = vorrq_u32(trueComponent, falseComponent);
  return res;  
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t sel4cmpLT(const vector4_t& vecA, const vector4_t& vecB, const vector4_t& selTrue, const vector4_t& selFalse)
{
  uint32x4_t mod = vcltq_f32(vecA.f, vecB.f);
  vector4_t res;
  uint32x4_t trueComponent = vandq_u32(selTrue.u, mod);
  uint32x4_t falseComponent = vbicq_u32(selFalse.u, mod);
  res.u = vorrq_u32(trueComponent, falseComponent);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t sel4cmp2LT(const vector4_t& vecA1, const vector4_t& vecA2, const vector4_t& vecB, const vector4_t& selTrue, const vector4_t& selFalse)
{
  uint32x4_t mod = vandq_u32(vcltq_f32(vecA1.f, vecB.f), vcltq_f32(vecA2.f, vecB.f));
  vector4_t res;
  uint32x4_t trueComponent = vandq_u32(selTrue.u, mod);
  uint32x4_t falseComponent = vbicq_u32(selFalse.u, mod);
  res.u = vorrq_u32(trueComponent, falseComponent);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t mask4cmpLT(const vector4_t vecA, const vector4_t vecB)
{
  vector4_t res;
  res.u = vcltq_f32(vecA.f, vecB.f);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t mask4cmpEQ(const vector4_t vecA, const vector4_t vecB)
{
  vector4_t res;
  res.u = vceqq_f32(vecA.f, vecB.f);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t sel4cmpBits(const vector4_t& vecVal, const vector4_t& vecBits, const vector4_t& selTrue, const vector4_t& selFalse)
{
  uint32x4_t mod = vceqq_u32(vandq_u32(vecVal.u, vecBits.u), vecBits.u);
  uint32x4_t trueComponent = vandq_u32(selTrue.u, mod);
  uint32x4_t falseComponent = vbicq_u32(selFalse.u, mod);
  
  vector4_t res;
  res.u = vorrq_u32(trueComponent, falseComponent);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t sel4cmpMask(const vector4_t& mask, const vector4_t& selTrue, const vector4_t& selFalse)
{
  uint32x4_t trueComponent = vandq_u32(selTrue.u, mask.u);
  uint32x4_t falseComponent = vbicq_u32(selFalse.u, mask.u);

  vector4_t res;
  res.u = vorrq_u32(trueComponent, falseComponent);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t mask4i()
{
  return set4i(-1);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t mask4i(const uint32_t mask)
{
  uint32x4_t maskTest = set4i(0x8, 0x4, 0x2, 0x1).u;
  uint32x4_t maskExtended = set4i(mask, mask, mask, mask).u;
  uint32x4_t maskedElems = vandq_u32(maskTest, maskExtended);

  vector4_t res;
  res.u = vceqq_u32(maskedElems, maskTest);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t maskr4i(const uint32_t mask)
{
  uint32x4_t maskTest = set4i(0x1, 0x2, 0x4, 0x8).u;
  uint32x4_t maskExtended = set4i(mask, mask, mask, mask).u;
  uint32x4_t maskedElems = vandq_u32(maskTest, maskExtended);

  vector4_t res;
  res.u = vceqq_u32(maskedElems, maskTest);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t mask4i(const uint8_t mask0, const uint8_t mask1, const uint8_t mask2, const uint8_t mask3)
{
  uint32x4_t maskExtended = set4i(mask0, mask1, mask2, mask3).u;
  uint32x4_t maskedElems = vceqq_u32(zero4f().u, maskExtended);

  vector4_t res;
  res.u = vmvnq_u32(maskedElems);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void load4v(float const* data, vector4_t& v0, vector4_t& v1, vector4_t& v2, vector4_t& v3)
{
  v0.f = vld1q_f32((float32_t*)data);
  v1.f = vld1q_f32((float32_t*)&data[4]);
  v2.f = vld1q_f32((float32_t*)&data[8]);
  v3.f = vld1q_f32((float32_t*)&data[12]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void store4v(float* data, const vector4_t& v0, const vector4_t& v1, const vector4_t& v2, const vector4_t& v3)
{
  vst1q_f32((float32_t*)data, v0.f);
  vst1q_f32((float32_t*)&data[4], v1.f);
  vst1q_f32((float32_t*)&data[8], v2.f);
  vst1q_f32((float32_t*)&data[12], v3.f);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void transpose4v(vector4_t& r1, vector4_t& r2, vector4_t& r3, vector4_t& r4)
{
  float32x4x2_t tmpA = vtrnq_f32(r1.f, r2.f);
  float32x4x2_t tmpB = vtrnq_f32(r3.f, r4.f);
  r4.f = vcombine_f32(vget_high_f32(tmpA.val[1]), vget_high_f32(tmpB.val[1]));
  r3.f = vcombine_f32(vget_high_f32(tmpA.val[0]), vget_high_f32(tmpB.val[0]));
  r2.f = vcombine_f32(vget_low_f32(tmpA.val[1]), vget_low_f32(tmpB.val[1]));
  r1.f = vcombine_f32(vget_low_f32(tmpA.val[0]), vget_low_f32(tmpB.val[0]));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void transpose4vConst(vector4_t& r1, vector4_t& r2, vector4_t& r3, vector4_t& r4,
                                     const vector4_t& c1, const vector4_t& c2, const vector4_t& c3, const vector4_t& c4)
{
  float32x4x2_t tmpA = vtrnq_f32(c1.f, c2.f);
  float32x4x2_t tmpB = vtrnq_f32(c3.f, c4.f);
  r1.f = vcombine_f32(vget_low_f32(tmpA.val[0]), vget_low_f32(tmpB.val[0]));
  r2.f = vcombine_f32(vget_low_f32(tmpA.val[1]), vget_low_f32(tmpB.val[1]));
  r3.f = vcombine_f32(vget_high_f32(tmpA.val[0]), vget_high_f32(tmpB.val[0]));
  r4.f = vcombine_f32(vget_high_f32(tmpA.val[1]), vget_high_f32(tmpB.val[1]));
}

#ifdef NM_COMPILER_SNC
  #pragma diag_push
  #pragma diag_suppress = 551 // variable is used before is value is set
#endif
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t horzadd4(const vector4_t& a, const vector4_t& b, const vector4_t& c, const vector4_t& d)
{
  return set4f(
    floatX(sum4f(a)),
    floatY(sum4f(b)),
    floatZ(sum4f(c)),
    floatW(sum4f(d)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t horzadd3(const vector4_t& a, const vector4_t& b, const vector4_t& c)
{
  vector4_t res;
  res = setxf(res, sum4f(a));
  res = setyf(res, sum4f(b));
  res = setzf(res, sum4f(c));
  return res;
}

#ifdef NM_COMPILER_SNC
  #pragma diag_pop
#endif

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void buildQMP(const vector4_t& quat,
                             vector4_t& M1x, vector4_t& M1y, vector4_t& M1z, vector4_t& M1w,
                             vector4_t& M2x, vector4_t& M2y, vector4_t& M2z, vector4_t& M2w)
{
  M1w = setwf(neg4f(quat), quat);                              // (-x, -y, -z, w)
  M1x.f = Swizzle4(quat.f, quat.f, M1w.f, quat.f, 3, 2, 1, 0); // (w, z, -y, x)
  M1y.f = Swizzle4(M1w.f, quat.f, quat.f, quat.f, 2, 3, 0, 1); // (-z, w, x, y)
  M1z.f = Swizzle4(quat.f, M1w.f, quat.f, quat.f, 1, 0, 3, 2); // (y, -x, w, z)

  M2x.f = Swizzle4(quat.f, quat.f, M1w.f, M1w.f, 3, 2, 1, 0);  // (w, z, -y, -x)
  M2y.f = Swizzle4(M1w.f, quat.f, quat.f, M1w.f, 2, 3, 0, 1);  // (-z, w, x, -y)
  M2z.f = Swizzle4(quat.f, M1w.f, quat.f, M1w.f, 1, 0, 3, 2);  // (y, -x, w, -z)
  M2w = quat;                                                  // (x, y, z, w)
}

//----------------------------------------------------------------------------------------------------------------------
// Packed data reads:  unpack[u=unaligned][number][i16=unsigned int16][f=32bit float conversion]
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t unpacku4i8(uint8_t const* data)
{
  uint8x8_t chars = vld1_u8(data);
  uint16x8_t shorts = vmovl_u8(chars);

  vector4_t res;
  res.u = vmovl_u16(vget_low_u16(shorts));
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t unpacku4i16(uint16_t const* data)
{
  uint16x4_t shorts = vld1_u16(data);

  vector4_t res;
  res.u = vmovl_u16(shorts);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t unpack4i16(uint16_t const* data)
{
  uint16x4_t shorts = vld1_u16(data);

  vector4_t res;
  res.u = vmovl_u16(shorts);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void unpacku6i16f(vector4_t& r1, vector4_t& r2, uint16_t const* data)
{
  r1.f = vcvtq_f32_u32(vmovl_u16(vld1_u16(data)));
  r2.f = vcvtq_f32_u32(vmovl_u16(vld1_u16(data + 3)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t unpacku4i16f(uint16_t const* data)
{
  vector4_t res;
  res.f = vcvtq_f32_u32(vmovl_u16(vld1_u16(data)));
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t unpack4i16f(uint16_t const* data)
{
  vector4_t res;
  res.f = vcvtq_f32_u32(vmovl_u16(vld1_u16(data)));
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void unpack8i16f(vector4_t& r1, vector4_t& r2, uint16_t const* data)
{
  uint16x8_t shorts = vld1q_u16(data);

  r1.f = vcvtq_f32_u32(vmovl_u16(vget_low_u16(shorts)));
  r2.f = vcvtq_f32_u32(vmovl_u16(vget_high_u16(shorts)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void unpacku8i16f(vector4_t& r1, vector4_t& r2, uint16_t const* data)
{
  uint16x8_t shorts = vld1q_u16(data);

  r1.f = vcvtq_f32_u32(vmovl_u16(vget_low_u16(shorts)));
  r2.f = vcvtq_f32_u32(vmovl_u16(vget_high_u16(shorts)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void unpack4QuantizedVectorsPacked(vector4_t& r1, vector4_t& r2, vector4_t& r3, uint32_t const* qVec)
{
  NMP_ASSERT(NMP_IS_ALIGNED(qVec, 16));

  uint32x4_t V = vld1q_u32(qVec);

  uint32x4_t mask = set4i(0x07FF).u;
  uint32x4_t rl1 = vshrq_n_u32(V, 0x015);
  rl1 = vandq_u32(rl1, mask);
  r1.f = vcvtq_f32_u32(rl1);

  uint32x4_t r2i = vshrq_n_u32(V, 0x0A);
  r2i = vandq_u32(r2i, mask);
  r2.f = vcvtq_f32_u32(r2i);

  mask = set4i(0x03FF).u;
  uint32x4_t r3i = vandq_u32(V, mask);
  r3.f = vcvtq_f32_u32(r3i);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void unpack2QuantizedVectors(vector4_t& r1, vector4_t& r2, uint32_t const* qVec)
{
  NMP_ALIGN_PREFIX(16) uint16_t tdata[8] =
  { (uint16_t)((qVec[0] >> 21) & 2047),
    (uint16_t)((qVec[0] >> 10) & 2047),
    (uint16_t)((qVec[0] >> 0) & 1023), 0,
    (uint16_t)((qVec[1] >> 21) & 2047),
    (uint16_t)((qVec[1] >> 10) & 2047),
    (uint16_t)((qVec[1] >> 0) & 1023), 0
  };
  unpack8i16f(r1, r2, tdata);  
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t unpackQuantizedVectorData(uint32_t const* qVec)
{
  NMP_ALIGN_PREFIX(16) uint16_t tdata[4] = {
    (uint16_t)((*qVec >> 21) & 2047),
    (uint16_t)((*qVec >> 10) & 2047),
    (uint16_t)((*qVec >> 0) & 1023),
    0
  };

  return unpacku4i16(tdata);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t unpackQuantizedVector(uint32_t const* qVec)
{
  vector4_t res = unpackQuantizedVectorData(qVec);
  res.f = vcvtq_f32_u32(res.u);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
