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
#ifndef NMP_VPU_CACHESIZE
  #define NMP_VPU_CACHESIZE 32
#endif//NMP_VPU_CACHESIZE
//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void prefetchCacheLine(const void* _addr_, int _offset_)
{
  (void)_offset_;
  (void)_addr_;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE vector4_t c2int4f(const vector4_t vec)
{
  vector4_t ivec;
  ivec.u[0] = (uint32_t)vec.x;
  ivec.u[1] = (uint32_t)vec.y;
  ivec.u[2] = (uint32_t)vec.z;
  ivec.u[3] = (uint32_t)vec.w;
  return ivec;
}

NM_FORCEINLINE vector4_t c2float4i(const vector4_t ivec)
{
  vector4_t vec;
  vec.x = (float)ivec.u[0];
  vec.y = (float)ivec.u[1];
  vec.z = (float)ivec.u[2];
  vec.w = (float)ivec.u[3];
  return vec;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE vector4_t add4i(const vector4_t val1, const vector4_t val2)
{
  vector4_t vec;
  vec.u[0] = val1.u[0] + val2.u[0];
  vec.u[1] = val1.u[1] + val2.u[1];
  vec.u[2] = val1.u[2] + val2.u[2];
  vec.u[3] = val1.u[3] + val2.u[3];
  return vec;
}

NM_FORCEINLINE vector4_t sub4i(const vector4_t val1, const vector4_t val2)
{
  vector4_t vec;
  vec.u[0] = val1.u[0] - val2.u[0];
  vec.u[1] = val1.u[1] - val2.u[1];
  vec.u[2] = val1.u[2] - val2.u[2];
  vec.u[3] = val1.u[3] - val2.u[3];
  return vec;
}

NM_FORCEINLINE vector4_t set4i(const int u)
{
  vector4_t vec;
  vec.u[0] = u;
  vec.u[1] = u;
  vec.u[2] = u;
  vec.u[3] = u;
  return vec;
}

NM_FORCEINLINE vector4_t set4i(const int x, const int y, const int z, const int w)
{
  vector4_t vec;
  vec.u[0] = x;
  vec.u[1] = y;
  vec.u[2] = z;
  vec.u[3] = w;
  return vec;
}

NM_FORCEINLINE vector4_t sel4cmpiLT(const vector4_t& vecA, const vector4_t& vecB, const vector4_t& selTrue, const vector4_t& selFalse)
{
  vector4_t vec;
  vec.x = vecA.i[0] < vecB.i[0] ? selTrue.x : selFalse.x;
  vec.y = vecA.i[1] < vecB.i[1] ? selTrue.y : selFalse.y;
  vec.z = vecA.i[2] < vecB.i[2] ? selTrue.z : selFalse.z;
  vec.w = vecA.i[3] < vecB.i[3] ? selTrue.w : selFalse.w;
  return vec;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE vector4_t and4(const vector4_t vecA, const vector4_t vecB)
{
  vector4_t vec;
  vec.u[0] = vecA.u[0] & vecB.u[0];
  vec.u[1] = vecA.u[1] & vecB.u[1];
  vec.u[2] = vecA.u[2] & vecB.u[2];
  vec.u[3] = vecA.u[3] & vecB.u[3];
  return vec;
}

NM_FORCEINLINE vector4_t or4(const vector4_t vecA, const vector4_t vecB)
{
  vector4_t vec;
  vec.u[0] = vecA.u[0] | vecB.u[0];
  vec.u[1] = vecA.u[1] | vecB.u[1];
  vec.u[2] = vecA.u[2] | vecB.u[2];
  vec.u[3] = vecA.u[3] | vecB.u[3];
  return vec;
}

NM_FORCEINLINE vector4_t xor4(const vector4_t vecA, const vector4_t vecB)
{
  vector4_t vec;
  vec.u[0] = vecA.u[0] ^ vecB.u[0];
  vec.u[1] = vecA.u[1] ^ vecB.u[1];
  vec.u[2] = vecA.u[2] ^ vecB.u[2];
  vec.u[3] = vecA.u[3] ^ vecB.u[3];
  return vec;
}

NM_FORCEINLINE vector4_t not4(const vector4_t val)
{
  vector4_t vec;
  vec.u[0] = ~val.u[0];
  vec.u[1] = ~val.u[1];
  vec.u[2] = ~val.u[2];
  vec.u[3] = ~val.u[3];
  return vec;
}

NM_FORCEINLINE vector4_t shr4im16(const vector4_t val)
{
  vector4_t vec;
  vec.u[0] = val.u[0] >> 16;
  vec.u[1] = val.u[1] >> 16;
  vec.u[2] = val.u[2] >> 16;
  vec.u[3] = val.u[3] >> 16;
  return vec;
}

NM_FORCEINLINE vector4_t shr4im8(const vector4_t val)
{
  vector4_t vec;
  vec.u[0] = val.u[0] >> 8;
  vec.u[1] = val.u[1] >> 8;
  vec.u[2] = val.u[2] >> 8;
  vec.u[3] = val.u[3] >> 8;
  return vec;
}

NM_FORCEINLINE vector4_t shr4(const vector4_t val, const vector4_t vshr)
{
  vector4_t vec;
  vec.u[0] = val.u[0] >> vshr.u[0];
  vec.u[1] = val.u[1] >> vshr.u[1];
  vec.u[2] = val.u[2] >> vshr.u[2];
  vec.u[3] = val.u[3] >> vshr.u[3];
  return vec;
}

NM_FORCEINLINE vector4_t shl4im16(const vector4_t val)
{
  vector4_t vec;
  vec.u[0] = val.u[0] << 16;
  vec.u[1] = val.u[1] << 16;
  vec.u[2] = val.u[2] << 16;
  vec.u[3] = val.u[3] << 16;
  return vec;
}

NM_FORCEINLINE vector4_t shl4im8(const vector4_t val)
{
  vector4_t vec;
  vec.u[0] = val.u[0] << 8;
  vec.u[1] = val.u[1] << 8;
  vec.u[2] = val.u[2] << 8;
  vec.u[3] = val.u[3] << 8;
  return vec;
}

NM_FORCEINLINE vector4_t shl4(const vector4_t val, const vector4_t vshl)
{
  vector4_t vec;
  vec.u[0] = val.u[0] << vshl.u[0];
  vec.u[1] = val.u[1] << vshl.u[1];
  vec.u[2] = val.u[2] << vshl.u[2];
  vec.u[3] = val.u[3] << vshl.u[3];
  return vec;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE vector4_t abs4f(const vector4_t val)
{
  vector4_t vec;
  vec.v[0] = fabs(val.v[0]);
  vec.v[1] = fabs(val.v[1]);
  vec.v[2] = fabs(val.v[2]);
  vec.v[3] = fabs(val.v[3]);
  return vec;
}

NM_FORCEINLINE vector4_t neg4f(const vector4_t val)
{
  vector4_t vec;
  vec.v[0] = -val.v[0];
  vec.v[1] = -val.v[1];
  vec.v[2] = -val.v[2];
  vec.v[3] = -val.v[3];
  return vec;
}

NM_FORCEINLINE vector4_t neg4f(const vector4_t val, const vector4_t mask)
{
  vector4_t vec;
  vec.u[0] = val.u[0] ^ (0x80000000 & mask.u[0]);
  vec.u[1] = val.u[1] ^ (0x80000000 & mask.u[1]);
  vec.u[2] = val.u[2] ^ (0x80000000 & mask.u[2]);
  vec.u[3] = val.u[3] ^ (0x80000000 & mask.u[3]);
  return vec;
}

NM_FORCEINLINE vector4_t add4f(const vector4_t val1, const vector4_t val2)
{
  vector4_t vec;
  vec.v[0] = val1.v[0] + val2.v[0];
  vec.v[1] = val1.v[1] + val2.v[1];
  vec.v[2] = val1.v[2] + val2.v[2];
  vec.v[3] = val1.v[3] + val2.v[3];
  return vec;
}

NM_FORCEINLINE vector4_t sub4f(const vector4_t val1, const vector4_t val2)
{
  vector4_t vec;
  vec.v[0] = val1.v[0] - val2.v[0];
  vec.v[1] = val1.v[1] - val2.v[1];
  vec.v[2] = val1.v[2] - val2.v[2];
  vec.v[3] = val1.v[3] - val2.v[3];
  return vec;
}

NM_FORCEINLINE vector4_t mul4f(const vector4_t val1, const vector4_t val2)
{
  vector4_t vec;
  vec.v[0] = val1.v[0] * val2.v[0];
  vec.v[1] = val1.v[1] * val2.v[1];
  vec.v[2] = val1.v[2] * val2.v[2];
  vec.v[3] = val1.v[3] * val2.v[3];
  return vec;
}

NM_FORCEINLINE vector4_t madd4f(const vector4_t val1, const vector4_t val2, const vector4_t vadd)
{
  vector4_t vec;
  vec.v[0] = val1.v[0] * val2.v[0] + vadd.v[0];
  vec.v[1] = val1.v[1] * val2.v[1] + vadd.v[1];
  vec.v[2] = val1.v[2] * val2.v[2] + vadd.v[2];
  vec.v[3] = val1.v[3] * val2.v[3] + vadd.v[3];
  return vec;
}

NM_FORCEINLINE vector4_t sqrt4f(const vector4_t val)
{
  vector4_t vec;
  vec.v[0] = sqrtf(val.v[0]);
  vec.v[1] = sqrtf(val.v[1]);
  vec.v[2] = sqrtf(val.v[2]);
  vec.v[3] = sqrtf(val.v[3]);
  return vec;
}

NM_FORCEINLINE vector4_t rcp4f(const vector4_t val)
{
  vector4_t vec;
  vec.v[0] = 1 / val.v[0];
  vec.v[1] = 1 / val.v[1];
  vec.v[2] = 1 / val.v[2];
  vec.v[3] = 1 / val.v[3];
  return vec;
}

NM_FORCEINLINE vector4_t rsqrt4f(const vector4_t val)
{
  vector4_t vec;
  vec.v[0] = 1 / sqrtf(val.v[0]);
  vec.v[1] = 1 / sqrtf(val.v[1]);
  vec.v[2] = 1 / sqrtf(val.v[2]);
  vec.v[3] = 1 / sqrtf(val.v[3]);
  return vec;
}

// for versatility, the user is responsible for abs(vec)
NM_FORCEINLINE vector4_t isBound4f(const vector4_t v, const vector4_t tol)
{
  vector4_t vec;
  uint32_t ret = ((v.v[0] < tol.v[0]) & (v.v[1] < tol.v[1]) &
                  (v.v[2] < tol.v[2]) & (v.v[3] < tol.v[3])) ? 0xFFFFFFFF : 0;
  vec.u[0] = ret;
  vec.u[1] = ret;
  vec.u[2] = ret;
  vec.u[3] = ret;
  return vec;
}

// for versatility, the user is responsible for abs(vec)
NM_FORCEINLINE vector4_t isBound3f(const vector4_t v, const vector4_t tol)
{
  vector4_t vec;
  uint32_t ret = ((v.v[0] < tol.v[0]) & (v.v[1] < tol.v[1]) &
                  (v.v[2] < tol.v[2])) ? 0xFFFFFFFF : 0;
  vec.u[0] = ret;
  vec.u[1] = ret;
  vec.u[2] = ret;
  return vec;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE vector4_t mixLoHi(const vector4_t vlo, const vector4_t vhi)
{
  vector4_t vec;
  vec.v[0] = vlo.v[0];
  vec.v[1] = vlo.v[1];
  vec.v[2] = vhi.v[2];
  vec.v[3] = vhi.v[3];
  return vec;
}

NM_FORCEINLINE vector4_t rotL4(const vector4_t v)
{
  vector4_t vec;
  vec.v[0] = v.v[1];
  vec.v[1] = v.v[2];
  vec.v[2] = v.v[3];
  vec.v[3] = v.v[0];
  return vec;
}
NM_FORCEINLINE vector4_t rotR4(const vector4_t v)
{
  vector4_t vec;
  vec.v[0] = v.v[3];
  vec.v[1] = v.v[0];
  vec.v[2] = v.v[1];
  vec.v[3] = v.v[2];
  return vec;
}

NM_FORCEINLINE vector4_t swapLoHi(const vector4_t v)
{
  vector4_t vec;
  vec.v[0] = v.v[2];
  vec.v[1] = v.v[3];
  vec.v[2] = v.v[0];
  vec.v[3] = v.v[1];
  return vec;
}

NM_FORCEINLINE vector4_t reverse(const vector4_t v)
{
  vector4_t vec;
  vec.v[0] = v.v[3];
  vec.v[1] = v.v[2];
  vec.v[2] = v.v[1];
  vec.v[3] = v.v[0];
  return vec;
}

NM_FORCEINLINE vector4_t swapXY(const vector4_t v)
{
  vector4_t vec;
  vec.v[0] = v.v[1];
  vec.v[1] = v.v[0];
  vec.v[2] = v.v[2];
  vec.v[3] = v.v[3];
  return vec;
}

NM_FORCEINLINE vector4_t swapZW(const vector4_t v)
{
  vector4_t vec;
  vec.v[0] = v.v[0];
  vec.v[1] = v.v[1];
  vec.v[2] = v.v[3];
  vec.v[3] = v.v[2];
  return vec;
}

NM_FORCEINLINE vector4_t swapYZ(const vector4_t v)
{
  vector4_t vec;
  vec.v[0] = v.v[0];
  vec.v[1] = v.v[2];
  vec.v[2] = v.v[1];
  vec.v[3] = v.v[3];
  return vec;
}

NM_FORCEINLINE vector4_t swapXW(const vector4_t v)
{
  vector4_t vec;
  vec.v[0] = v.v[3];
  vec.v[1] = v.v[1];
  vec.v[2] = v.v[2];
  vec.v[3] = v.v[0];
  return vec;
}

NM_FORCEINLINE vector4_t swapYW(const vector4_t v)
{
  vector4_t vec;
  vec.v[0] = v.v[0];
  vec.v[1] = v.v[3];
  vec.v[2] = v.v[2];
  vec.v[3] = v.v[1];
  return vec;
}

NM_FORCEINLINE vector4_t swapXZ(const vector4_t v)
{
  vector4_t vec;
  vec.v[0] = v.v[2];
  vec.v[1] = v.v[1];
  vec.v[2] = v.v[0];
  vec.v[3] = v.v[3];
  return vec;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE vector4_t splatX(const vector4_t v)
{
  vector4_t vec;
  vec.v[0] = v.v[0];
  vec.v[1] = v.v[0];
  vec.v[2] = v.v[0];
  vec.v[3] = v.v[0];
  return vec;
}

NM_FORCEINLINE vector4_t splatY(const vector4_t v)
{
  vector4_t vec;
  vec.v[0] = v.v[1];
  vec.v[1] = v.v[1];
  vec.v[2] = v.v[1];
  vec.v[3] = v.v[1];
  return vec;
}

NM_FORCEINLINE vector4_t splatZ(const vector4_t v)
{
  vector4_t vec;
  vec.v[0] = v.v[2];
  vec.v[1] = v.v[2];
  vec.v[2] = v.v[2];
  vec.v[3] = v.v[2];
  return vec;
}

NM_FORCEINLINE vector4_t splatW(const vector4_t v)
{
  vector4_t vec;
  vec.v[0] = v.v[3];
  vec.v[1] = v.v[3];
  vec.v[2] = v.v[3];
  vec.v[3] = v.v[3];
  return vec;
}

NM_FORCEINLINE uint32_t elemX(const vector4_t v) { return v.u[0]; }
NM_FORCEINLINE uint32_t elemY(const vector4_t v) { return v.u[1]; }
NM_FORCEINLINE uint32_t elemZ(const vector4_t v) { return v.u[2]; }
NM_FORCEINLINE uint32_t elemW(const vector4_t v) { return v.u[3]; }

NM_FORCEINLINE float floatX(const vector4_t v) { return v.v[0]; }
NM_FORCEINLINE float floatY(const vector4_t v) { return v.v[1]; }
NM_FORCEINLINE float floatZ(const vector4_t v) { return v.v[2]; }
NM_FORCEINLINE float floatW(const vector4_t v) { return v.v[3]; }

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE vector4_t zero4f()
{
  vector4_t vec;
  vec.v[0] = 0.0f;
  vec.v[1] = 0.0f;
  vec.v[2] = 0.0f;
  vec.v[3] = 0.0f;
  return vec;
}

NM_FORCEINLINE vector4_t one4f()
{
  vector4_t vec;
  vec.v[0] = 1.0f;
  vec.v[1] = 1.0f;
  vec.v[2] = 1.0f;
  vec.v[3] = 1.0f;
  return vec;
}

NM_FORCEINLINE vector4_t quat4f()
{
  vector4_t vec;
  vec.v[0] = 0.0f;
  vec.v[1] = 0.0f;
  vec.v[2] = 0.0f;
  vec.v[3] = 1.0f;
  return vec;
}

NM_FORCEINLINE vector4_t negOne4f()
{
  vector4_t vec;
  vec.v[0] = -1.0f;
  vec.v[1] = -1.0f;
  vec.v[2] = -1.0f;
  vec.v[3] = -1.0f;
  return vec;
}

NM_FORCEINLINE vector4_t half4f()
{
  vector4_t vec;
  vec.v[0] = 0.5f;
  vec.v[1] = 0.5f;
  vec.v[2] = 0.5f;
  vec.v[3] = 0.5f;
  return vec;
}

NM_FORCEINLINE vector4_t fltEpsilon4f()
{
  vector4_t vec;
  vec.v[0] = FLT_EPSILON;
  vec.v[1] = FLT_EPSILON;
  vec.v[2] = FLT_EPSILON;
  vec.v[3] = FLT_EPSILON;
  return vec;
}

NM_FORCEINLINE vector4_t set4f(const float f)
{
  vector4_t vec;
  vec.v[0] = f;
  vec.v[1] = f;
  vec.v[2] = f;
  vec.v[3] = f;
  return vec;
}

NM_FORCEINLINE vector4_t set4f(const float x, const float y, const float z, const float w)
{
  vector4_t vec;
  vec.v[0] = x;
  vec.v[1] = y;
  vec.v[2] = z;
  vec.v[3] = w;
  return vec;
}

NM_FORCEINLINE vector4_t setxf(const vector4_t oyzw, const vector4_t xooo)
{
  vector4_t vec = oyzw;
  vec.v[0] = xooo.v[0];
  return vec;
}

NM_FORCEINLINE vector4_t setyf(const vector4_t xozw, const vector4_t oyoo)
{
  vector4_t vec = xozw;
  vec.v[1] = oyoo.v[1];
  return vec;
}

NM_FORCEINLINE vector4_t setzf(const vector4_t xyow, const vector4_t oozo)
{
  vector4_t vec = xyow;
  vec.v[2] = oozo.v[2];
  return vec;
}

NM_FORCEINLINE vector4_t setwf(const vector4_t xyzo, const vector4_t ooow)
{
  vector4_t vec = xyzo;
  vec.v[3] = ooow.v[3];
  return vec;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE vector4_t load1fp(float const* fp)
{
  vector4_t vec;
  vec.v[0] = *fp;
  vec.v[1] = *fp;
  vec.v[2] = *fp;
  vec.v[3] = *fp;
  return vec;
}

NM_FORCEINLINE vector4_t load4f(float const* fp)
{
  vector4_t vec;
  vec.v[0] = fp[0];
  vec.v[1] = fp[1];
  vec.v[2] = fp[2];
  vec.v[3] = fp[3];
  return vec;
}

NM_FORCEINLINE vector4_t loadu4f(float const* fp)
{
  return load4f(fp);
}

NM_FORCEINLINE void store4f(float* fp, const vector4_t v)
{
  fp[0] = v.v[0];
  fp[1] = v.v[1];
  fp[2] = v.v[2];
  fp[3] = v.v[3];
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE vector4_t sum4f(const vector4_t v)
{
  float sum = v.v[0] + v.v[1] + v.v[2] + v.v[3];
  vector4_t vec;
  vec.v[0] = sum;
  vec.v[1] = sum;
  vec.v[2] = sum;
  vec.v[3] = sum;
  return vec;
}

NM_FORCEINLINE vector4_t dot4f(const vector4_t a, const vector4_t b)
{
  float dot = a.v[0] * b.v[0] + a.v[1] * b.v[1] + a.v[2] * b.v[2] + a.v[3] * b.v[3];
  vector4_t vec;
  vec.v[0] = dot;
  vec.v[1] = dot;
  vec.v[2] = dot;
  vec.v[3] = dot;
  return vec;
}

NM_FORCEINLINE vector4_t dot3f(const vector4_t a, const vector4_t b)
{
  float dot = a.v[0] * b.v[0] + a.v[1] * b.v[1] + a.v[2] * b.v[2];
  vector4_t vec;
  vec.v[0] = dot;
  vec.v[1] = dot;
  vec.v[2] = dot;
  vec.v[3] = dot;
  return vec;
}

NM_FORCEINLINE vector4_t cross3f(const vector4_t a, const vector4_t b)
{
  vector4_t vec;
  vec.v[0] = (a.y * b.z) - (a.z * b.y);
  vec.v[1] = (a.z * b.x) - (a.x * b.z);
  vec.v[2] = (a.x * b.y) - (a.y * b.x);
  vec.v[3] = 0.0f;
  return vec;
}

NM_FORCEINLINE vector4_t qqMul(const vector4_t a, const vector4_t b)
{
  vector4_t vec;
  vec.v[0] = b.w * a.x + b.x * a.w + a.y * b.z - a.z * b.y;
  vec.v[1] = b.w * a.y + b.y * a.w + a.z * b.x - a.x * b.z;
  vec.v[2] = b.w * a.z + b.z * a.w + a.x * b.y - a.y * b.x;
  vec.v[3] = b.w * a.w - b.x * a.x - b.y * a.y - b.z * a.z;
  return vec;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE vector4_t sel4cmpEQ(const vector4_t& vecA, const vector4_t& vecB, const vector4_t& selTrue, const vector4_t& selFalse)
{
  vector4_t vec;
  vec.v[0] = vecA.v[0] == vecB.v[0] ? selTrue.v[0] : selFalse.v[0];
  vec.v[1] = vecA.v[1] == vecB.v[1] ? selTrue.v[1] : selFalse.v[1];
  vec.v[2] = vecA.v[2] == vecB.v[2] ? selTrue.v[2] : selFalse.v[2];
  vec.v[3] = vecA.v[3] == vecB.v[3] ? selTrue.v[3] : selFalse.v[3];
  return vec;
}

NM_FORCEINLINE vector4_t sel4cmpLT(const vector4_t& vecA, const vector4_t& vecB, const vector4_t& selTrue, const vector4_t& selFalse)
{
  vector4_t vec;
  vec.v[0] = vecA.v[0] < vecB.v[0] ? selTrue.v[0] : selFalse.v[0];
  vec.v[1] = vecA.v[1] < vecB.v[1] ? selTrue.v[1] : selFalse.v[1];
  vec.v[2] = vecA.v[2] < vecB.v[2] ? selTrue.v[2] : selFalse.v[2];
  vec.v[3] = vecA.v[3] < vecB.v[3] ? selTrue.v[3] : selFalse.v[3];
  return vec;
}

NM_FORCEINLINE vector4_t sel4cmp2LT(const vector4_t& vecA1, const vector4_t& vecA2, const vector4_t& vecB, const vector4_t& selTrue, const vector4_t& selFalse)
{
  vector4_t vec;
  vec.v[0] = (vecA1.v[0] < vecB.v[0]) && (vecA2.v[0] < vecB.v[0]) ? selTrue.v[0] : selFalse.v[0];
  vec.v[1] = (vecA1.v[1] < vecB.v[1]) && (vecA2.v[1] < vecB.v[1]) ? selTrue.v[1] : selFalse.v[1];
  vec.v[2] = (vecA1.v[2] < vecB.v[2]) && (vecA2.v[2] < vecB.v[2]) ? selTrue.v[2] : selFalse.v[2];
  vec.v[3] = (vecA1.v[3] < vecB.v[3]) && (vecA2.v[3] < vecB.v[3]) ? selTrue.v[3] : selFalse.v[3];
  return vec;
}

NM_FORCEINLINE vector4_t mask4cmpLT(const vector4_t vecA, const vector4_t vecB)
{
  vector4_t vec;
  vec.u[0] = vecA.v[0] < vecB.v[0] ? 0xFFFFFFFF : 0;
  vec.u[1] = vecA.v[1] < vecB.v[1] ? 0xFFFFFFFF : 0;
  vec.u[2] = vecA.v[2] < vecB.v[2] ? 0xFFFFFFFF : 0;
  vec.u[3] = vecA.v[3] < vecB.v[3] ? 0xFFFFFFFF : 0;
  return vec;
}

NM_FORCEINLINE vector4_t mask4cmpEQ(const vector4_t vecA, const vector4_t vecB)
{
  vector4_t vec;
  vec.u[0] = vecA.v[0] == vecB.v[0] ? 0xFFFFFFFF : 0;
  vec.u[1] = vecA.v[1] == vecB.v[1] ? 0xFFFFFFFF : 0;
  vec.u[2] = vecA.v[2] == vecB.v[2] ? 0xFFFFFFFF : 0;
  vec.u[3] = vecA.v[3] == vecB.v[3] ? 0xFFFFFFFF : 0;
  return vec;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE vector4_t sel4cmpBits(const vector4_t& vecVal, const vector4_t& vecBits, const vector4_t& selTrue, const vector4_t& selFalse)
{
  vector4_t vec;
  vec.v[0] = (vecVal.u[0] & vecBits.u[0]) == vecBits.u[0] ? selTrue.v[0] : selFalse.v[0];
  vec.v[1] = (vecVal.u[1] & vecBits.u[1]) == vecBits.u[1] ? selTrue.v[1] : selFalse.v[1];
  vec.v[2] = (vecVal.u[2] & vecBits.u[2]) == vecBits.u[2] ? selTrue.v[2] : selFalse.v[2];
  vec.v[3] = (vecVal.u[3] & vecBits.u[3]) == vecBits.u[3] ? selTrue.v[3] : selFalse.v[3];
  return vec;
}

NM_FORCEINLINE vector4_t sel4cmpMask(const vector4_t& mask, const vector4_t& selTrue, const vector4_t& selFalse)
{
  vector4_t vec;
  vec.u[0] = (mask.u[0] & selTrue.u[0]) | ((~mask.u[0]) & selFalse.u[0]);
  vec.u[1] = (mask.u[1] & selTrue.u[1]) | ((~mask.u[1]) & selFalse.u[1]);
  vec.u[2] = (mask.u[2] & selTrue.u[2]) | ((~mask.u[2]) & selFalse.u[2]);
  vec.u[3] = (mask.u[3] & selTrue.u[3]) | ((~mask.u[3]) & selFalse.u[3]);
  return vec;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE vector4_t mask4i()
{
  vector4_t vec;
  vec.u[0] = 0xFFFFFFFF;
  vec.u[1] = 0xFFFFFFFF;
  vec.u[2] = 0xFFFFFFFF;
  vec.u[3] = 0xFFFFFFFF;
  return vec;
}

NM_FORCEINLINE vector4_t mask4i(const uint32_t mask)
{
  vector4_t vec;
  vec.u[0] = (mask & 0x8) == 0 ? 0 : 0xFFFFFFFF;
  vec.u[1] = (mask & 0x4) == 0 ? 0 : 0xFFFFFFFF;
  vec.u[2] = (mask & 0x2) == 0 ? 0 : 0xFFFFFFFF;
  vec.u[3] = (mask & 0x1) == 0 ? 0 : 0xFFFFFFFF;
  return vec;
}

NM_FORCEINLINE vector4_t maskr4i(const uint32_t mask)
{
  vector4_t vec;
  vec.u[0] = (mask & 0x1) == 0 ? 0 : 0xFFFFFFFF;
  vec.u[1] = (mask & 0x2) == 0 ? 0 : 0xFFFFFFFF;
  vec.u[2] = (mask & 0x4) == 0 ? 0 : 0xFFFFFFFF;
  vec.u[3] = (mask & 0x8) == 0 ? 0 : 0xFFFFFFFF;
  return vec;
}

NM_FORCEINLINE vector4_t mask4i(const uint8_t mask0, const uint8_t mask1, const uint8_t mask2, const uint8_t mask3)
{
  vector4_t vec;
  vec.u[0] = mask0 == 0 ? 0 : 0xFFFFFFFF;
  vec.u[1] = mask1 == 0 ? 0 : 0xFFFFFFFF;
  vec.u[2] = mask2 == 0 ? 0 : 0xFFFFFFFF;
  vec.u[3] = mask3 == 0 ? 0 : 0xFFFFFFFF;
  return vec;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void load4v(float const* data, vector4_t& v0, vector4_t& v1, vector4_t& v2, vector4_t& v3)
{
  v0 = load4f(&data[0]);
  v1 = load4f(&data[4]);
  v2 = load4f(&data[8]);
  v3 = load4f(&data[12]);
}

NM_FORCEINLINE void store4v(float* data, const vector4_t& v0, const vector4_t& v1, const vector4_t& v2, const vector4_t& v3)
{
  store4f(&data[0],  v0);
  store4f(&data[4],  v1);
  store4f(&data[8],  v2);
  store4f(&data[12], v3);
}

NM_FORCEINLINE void transpose4v(vector4_t& r1, vector4_t& r2, vector4_t& r3, vector4_t& r4)
{
  vector4_t vx; vx.v[0] = r1.v[0]; vx.v[1] = r2.v[0]; vx.v[2] = r3.v[0]; vx.v[3] = r4.v[0];
  vector4_t vy; vy.v[0] = r1.v[1]; vy.v[1] = r2.v[1]; vy.v[2] = r3.v[1]; vy.v[3] = r4.v[1];
  vector4_t vz; vz.v[0] = r1.v[2]; vz.v[1] = r2.v[2]; vz.v[2] = r3.v[2]; vz.v[3] = r4.v[2];
  vector4_t vw; vw.v[0] = r1.v[3]; vw.v[1] = r2.v[3]; vw.v[2] = r3.v[3]; vw.v[3] = r4.v[3];
  r1 = vx; r2 = vy; r3 = vz; r4 = vw;
}

NM_FORCEINLINE void transpose4vConst(vector4_t& r1, vector4_t& r2, vector4_t& r3, vector4_t& r4,
                                     const vector4_t& c1, const vector4_t& c2, const vector4_t& c3, const vector4_t& c4)
{
  r1.v[0] = c1.v[0]; r1.v[1] = c2.v[0]; r1.v[2] = c3.v[0]; r1.v[3] = c4.v[0];
  r2.v[0] = c1.v[1]; r2.v[1] = c2.v[1]; r2.v[2] = c3.v[1]; r2.v[3] = c4.v[1];
  r3.v[0] = c1.v[2]; r3.v[1] = c2.v[2]; r3.v[2] = c3.v[2]; r3.v[3] = c4.v[2];
  r4.v[0] = c1.v[3]; r4.v[1] = c2.v[3]; r4.v[2] = c3.v[3]; r4.v[3] = c4.v[3];
}

NM_FORCEINLINE vector4_t horzadd4(const vector4_t& a, const vector4_t& b, const vector4_t& c, const vector4_t& d)
{
  vector4_t vec;
  vec.v[0] = a.v[0] + a.v[1] + a.v[2] + a.v[3];
  vec.v[1] = b.v[0] + b.v[1] + b.v[2] + b.v[3];
  vec.v[2] = c.v[0] + c.v[1] + c.v[2] + c.v[3];
  vec.v[3] = d.v[0] + d.v[1] + d.v[2] + d.v[3];
  return vec;
}

NM_FORCEINLINE vector4_t horzadd3(const vector4_t& a, const vector4_t& b, const vector4_t& c)
{
  vector4_t vec;
  vec.v[0] = a.v[0] + a.v[1] + a.v[2] + a.v[3];
  vec.v[1] = b.v[0] + b.v[1] + b.v[2] + b.v[3];
  vec.v[2] = c.v[0] + c.v[1] + c.v[2] + c.v[3];
  vec.v[3] = 0.0f;
  return vec;
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void buildQMP(const vector4_t& quat,
                             vector4_t& M1x, vector4_t& M1y, vector4_t& M1z, vector4_t& M1w,
                             vector4_t& M2x, vector4_t& M2y, vector4_t& M2z, vector4_t& M2w)
{
  M1x = set4f(quat.w, quat.z, -quat.y, quat.x);
  M1y = set4f(-quat.z, quat.w, quat.x, quat.y);
  M1z = set4f(quat.y, -quat.x, quat.w, quat.z);
  M1w = set4f(-quat.x, -quat.y, -quat.z, quat.w);

  M2x = set4f(quat.w, quat.z, -quat.y, -quat.x);
  M2y = set4f(-quat.z, quat.w, quat.x, -quat.y);
  M2z = set4f(quat.y, -quat.x, quat.w, -quat.z);
  M2w = set4f(quat.x, quat.y, quat.z, quat.w);
}

//----------------------------------------------------------------------------------------------------------------------
// Packed data reads:  unpack[u=unaligned][number][i16=unsigned int16][f=32bit float conversion]
//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE vector4_t unpacku4i8(uint8_t const* data)
{
  vector4_t vec;
  vec.u[0] = (uint32_t)data[0];
  vec.u[1] = (uint32_t)data[1];
  vec.u[2] = (uint32_t)data[2];
  vec.u[3] = (uint32_t)data[3];
  return vec;
}

NM_FORCEINLINE vector4_t unpack4i16(uint16_t const* data)
{
  vector4_t vec;
  vec.u[0] = (uint32_t)data[0];
  vec.u[1] = (uint32_t)data[1];
  vec.u[2] = (uint32_t)data[2];
  vec.u[3] = (uint32_t)data[3];
  return vec;
}

NM_FORCEINLINE vector4_t unpacku4i16(uint16_t const* data)
{
  return unpack4i16(data);
}

NM_FORCEINLINE vector4_t unpack4i16f(uint16_t const* data)
{
  vector4_t vec;
  vec.v[0] = (float)data[0];
  vec.v[1] = (float)data[1];
  vec.v[2] = (float)data[2];
  vec.v[3] = (float)data[3];
  return vec;
}

NM_FORCEINLINE vector4_t unpacku4i16f(uint16_t const* data)
{
  return unpack4i16f(data);
}

NM_FORCEINLINE void unpacku6i16f(vector4_t& r1, vector4_t& r2, uint16_t const* data)
{
  r1.v[0] = (float)data[0];
  r1.v[1] = (float)data[1];
  r1.v[2] = (float)data[2];
  r1.v[3] = 0.0f;
  r2.v[0] = (float)data[3];
  r2.v[1] = (float)data[4];
  r2.v[2] = (float)data[5];
  r2.v[3] = 0.0f;
}

NM_FORCEINLINE void unpack8i16f(vector4_t& r1, vector4_t& r2, uint16_t const* data)
{
  r1.v[0] = (float)data[0];
  r1.v[1] = (float)data[1];
  r1.v[2] = (float)data[2];
  r1.v[3] = (float)data[3];
  r2.v[0] = (float)data[4];
  r2.v[1] = (float)data[5];
  r2.v[2] = (float)data[6];
  r2.v[3] = (float)data[7];
}

NM_FORCEINLINE void unpacku8i16f(vector4_t& r1, vector4_t& r2, uint16_t const* data)
{
  unpack8i16f(r1, r2, data);
}

//----------------------------------------------------------------------------------------------------------------------

NM_FORCEINLINE void unpack4QuantizedVectorsPacked(vector4_t& r1, vector4_t& r2, vector4_t& r3, uint32_t const* qVec)
{
  r1.v[0] = (float)((qVec[0] >> 21) & 2047);
  r1.v[1] = (float)((qVec[1] >> 21) & 2047);
  r1.v[2] = (float)((qVec[2] >> 21) & 2047);
  r1.v[3] = (float)((qVec[3] >> 21) & 2047);

  r2.v[0] = (float)((qVec[0] >> 10) & 2047);
  r2.v[1] = (float)((qVec[1] >> 10) & 2047);
  r2.v[2] = (float)((qVec[2] >> 10) & 2047);
  r2.v[3] = (float)((qVec[3] >> 10) & 2047);

  r3.v[0] = (float)(qVec[0] & 1023);
  r3.v[1] = (float)(qVec[1] & 1023);
  r3.v[2] = (float)(qVec[2] & 1023);
  r3.v[3] = (float)(qVec[3] & 1023);
}

NM_FORCEINLINE void unpack2QuantizedVectors(vector4_t& r1, vector4_t& r2, uint32_t const* qVec)
{
  r1.v[0] = (float)((qVec[0] >> 21) & 2047);
  r1.v[1] = (float)((qVec[0] >> 10) & 2047);
  r1.v[2] = (float)((qVec[0] >> 0)  & 1023);
  r1.v[3] = 0.0f;
  r2.v[0] = (float)((qVec[1] >> 21) & 2047);
  r2.v[1] = (float)((qVec[1] >> 10) & 2047);
  r2.v[2] = (float)((qVec[1] >> 0)  & 1023);
  r2.v[3] = 0.0f;
}

NM_FORCEINLINE vector4_t unpackQuantizedVectorData(uint32_t const* qVec)
{
  vector4_t vec;
  vec.u[0] = (*qVec >> 21) & 2047;
  vec.u[1] = (*qVec >> 10) & 2047;
  vec.u[2] = (*qVec >> 0)  & 1023;
  vec.u[3] = 0;
  return vec;
}

NM_FORCEINLINE vector4_t unpackQuantizedVector(uint32_t const* qVec)
{
  vector4_t vec;
  vec.v[0] = (float)((*qVec >> 21) & 2047);
  vec.v[1] = (float)((*qVec >> 10) & 2047);
  vec.v[2] = (float)((*qVec >> 0)  & 1023);
  vec.v[3] = 0.0f;
  return vec;
}

//----------------------------------------------------------------------------------------------------------------------
