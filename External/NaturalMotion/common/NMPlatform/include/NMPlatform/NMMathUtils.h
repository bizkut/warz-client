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
#ifndef NM_MATHUTILS_H
#define NM_MATHUTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include <limits.h>
#include <math.h>
#include <float.h>

#if defined(NM_HOST_X360)
  #include <ppcintrinsics.h>
  #define NM_HAS_FSEL_INTRINSIC
#endif

#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
  #include <xmmintrin.h>
#endif

#if defined(NM_HOST_CELL_SPU)
  #include <bits/sce_math.h>
#endif // NM_HOST_CELL_SPU

#if defined(NM_HOST_CELL_PPU)
  #include <ppu_intrinsics.h>
  #define NM_HAS_FSEL_INTRINSIC
#endif // NM_HOST_CELL_PPU

//----------------------------------------------------------------------------------------------------------------------
/// \file  NMMathUtils.h
/// \ingroup NaturalMotionPlatform
/// \brief Defines a set of maths utility functions and constants.
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

/// \def USE_FAST_TRIG_FUNCTIONS
/// \brief Remove if wishing to test with standard full precision trig functions.
#define USE_FAST_TRIG_FUNCTIONS

/// \def USE_FAST_APPROX_FUNCTIONS
/// \brief Remove if wishing to test with standard full precision root and reciprocal functions.
#define USE_FAST_APPROX_FUNCTIONS

//----------------------------------------------------------------------------------------------------------------------
#define NM_PI   (3.14159265358979323846f)
// below are certain to have no operations, (NM_PI/2.0f) may possibly do the / at runtime on more basic compilers
#define NM_PI_OVER_TWO  (1.57079632679489661923f)
#define NM_PI_OVER_FOUR (0.785398163397448309616f)
#define NM_ONE_OVER_PI  (0.318309886183790671538f)
#define NM_TWO_OVER_PI  (0.636619772367581343076f)

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float degreesToRadians(float angle) { return angle * NM_PI / 180.0f; }
NM_FORCEINLINE float radiansToDegrees(float angle) { return angle * 180.0f / NM_PI; }

NM_FORCEINLINE float nmfabs(float value)
{
#if defined(NM_HOST_X360)
  return __fabs(value);
#else
  return fabs(value);
#endif // NM_HOST_X360
}

NM_FORCEINLINE float nmfloor(float value)
{
  return floorf(value);
}

NM_FORCEINLINE float nmceil(float value)
{
  return ceilf(value);
}

template <class T>
NM_FORCEINLINE void nmSwap(T& lhs, T& rhs)
{
  T temp = lhs;
  lhs = rhs, rhs = temp;
}

template <class T>
NM_FORCEINLINE bool compare(T val1, T val2, T tol)
{
  return fabs(val1 - val2) < tol;
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_FORCEINLINE T nmAlignedValue4(T val)
{
  return (val + static_cast<T>(0x03)) & static_cast<T>(~0x03);
}

//----------------------------------------------------------------------------------------------------------------------
// Floating Select
// conditional move avoiding very pricey fp-compare-and-branch on PPC
// double __fsel(double fComparand, double fValGE, double fLT);
//
// fsel  fpDest, fpRegA, fpRegB, fpRegC
// fpDest = fpRegA >= 0 ? fpRegB : fpRegC

#ifdef NM_HAS_FSEL_INTRINSIC
NM_FORCEINLINE float floatSelect(float fComparand, float fValGE, float fLT) { return (float)__fsel(fComparand, fValGE, fLT); }
#define nm_fsel_max(a, b) __fsel((a)-(b), a, b)
#define nm_fsel_min(a, b) __fsel((a)-(b), b, a)
#else
NM_FORCEINLINE float floatSelect(float fComparand, float fValGE, float fLT) { return (fComparand >= 0.0f ? fValGE : fLT); }
#define nm_fsel_max(a, b) (a > b ? a : b)
#define nm_fsel_min(a, b) (a > b ? b : a)
#endif // NM_HAS_FSEL_INTRINSIC

// Signum function avoiding branches
// signum(x) = { -1 if x < 0, 0 if x == 0, 1 if x > 0 }
NM_FORCEINLINE float signum(float x)
{  
  float val = floatSelect(x, x, -1.0f);
  val = floatSelect(-val, val, 1.0f);
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
// Integer mask avoiding branches
// result = (comparand == 0) ? 0 : 0xFFFFFFFF
NM_FORCEINLINE uint32_t uintMask(uint32_t comparand)
{
#if defined(NM_HOST_WIN32)
  return (comparand == 0) ? 0: 0xFFFFFFFF;
#else
  // A positive number or'd with its negative will have the sign bit set. The negative of zero
  // is still zero so will not have the sign bit set. The right shift operator will shift 1's
  // in if the sign bit is set and 0's otherwise.
  return (uint32_t)(((int32_t)comparand | -(int32_t)comparand) >> 31);
#endif
}

// Integer select avoiding branches
// result = (comparand == 0) ? valEqZ : valNZ
NM_FORCEINLINE uint32_t uintMask(uint32_t comparand, uint32_t valEqZ, uint32_t valNZ)
{
#if defined(NM_HOST_WIN32)
  return (comparand == 0) ? valEqZ: valNZ;
#else
  // A positive number or'd with its negative will have the sign bit set. The negative of zero
  // is still zero so will not have the sign bit set. The right shift operator will shift 1's
  // in if the sign bit is set and 0's otherwise.
  uint32_t mask = (uint32_t)(((int32_t)comparand | -(int32_t)comparand) >> 31);
  return (mask & valNZ) | (~mask & valEqZ);
#endif
}

// Integer select avoiding branches
// result = (comparand == 0) ? valEqZ : valNZ
NM_FORCEINLINE int32_t intMask(uint32_t comparand, int32_t valEqZ, int32_t valNZ)
{
#if defined(NM_HOST_WIN32)
  return (comparand == 0) ? valEqZ: valNZ;
#else
  // A positive number or'd with its negative will have the sign bit set. The negative of zero
  // is still zero so will not have the sign bit set. The right shift operator will shift 1's
  // in if the sign bit is set and 0's otherwise.
  uint32_t mask = (uint32_t)(((int32_t)comparand | -(int32_t)comparand) >> 31);
  return (int32_t)((mask & valNZ) | (~mask & valEqZ));
#endif
}

//----------------------------------------------------------------------------------------------------------------------
// Integer Select
// branchless conditional integer selection, equivalent to floatSelect
//
// return (comparand >= 0) ? valGE : valLT;
NM_FORCEINLINE int32_t intSelect(int32_t comparand, int32_t valGE, int32_t valLT)
{
#if defined(NM_HOST_WIN32)
  return (comparand >= 0) ? valGE: valLT;
#else
  int32_t conditionMask = comparand >> 31;
  return (conditionMask & valLT) | (~conditionMask & valGE);
#endif
}

NM_FORCEINLINE int64_t intSelect(int64_t comparand, int64_t valGE, int64_t valLT)
{
#if defined(NM_HOST_WIN32)
  return (comparand >= 0) ? valGE: valLT;
#else
  int64_t conditionMask = comparand >> 63;
  return (conditionMask & valLT) | (~conditionMask & valGE);
#endif
}

NM_FORCEINLINE uint32_t uintSelect(int32_t comparand, uint32_t valGE, uint32_t valLT)
{
#if defined(NM_HOST_WIN32)
  return (comparand >= 0) ? valGE: valLT;
#else
  int32_t conditionMask = comparand >> 31;
  return (conditionMask & valLT) | (~conditionMask & valGE);
#endif
}

NM_FORCEINLINE uint64_t uintSelect(int64_t comparand, uint64_t valGE, uint64_t valLT)
{
#if defined(NM_HOST_WIN32)
  return (comparand >= 0) ? valGE: valLT;
#else
  int64_t conditionMask = comparand >> 63;
  return (conditionMask & valLT) | (~conditionMask & valGE);
#endif
}

NM_FORCEINLINE intptr_t intPtrSelect(intptr_t comparand, intptr_t valGE, intptr_t valLT)
{
#if defined(NM_HOST_WIN32)
  return (comparand >= 0) ? valGE: valLT;
#else
  intptr_t conditionMask = comparand >> (8 * sizeof(intptr_t) - 1);
  return (conditionMask & valLT) | (~conditionMask & valGE);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
// Clamping
template <class T>
NM_FORCEINLINE T maximum(T t1, T t2)
{
  return (t2 > t1 ? t2 : t1);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NM_FORCEINLINE float maximum<float>(float t1, float t2)
{
  return (float)nm_fsel_max(t1, t2);
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_FORCEINLINE T minimum(T t1, T t2)
{
  return (t1 > t2 ? t2 : t1);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NM_FORCEINLINE float minimum<float>(float t1, float t2)
{
  return (float)nm_fsel_min(t1, t2);
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_FORCEINLINE T clampValue(T val, T valuemin, T valuemax)
{
  NMP_ASSERT(valuemin <= valuemax);
  return ((val < valuemin) ? valuemin : ((val > valuemax) ? valuemax : val));
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NM_FORCEINLINE float clampValue<float>(float val, float valuemin, float valuemax)
{
  NMP_ASSERT(valuemin <= valuemax);
  return (float)nm_fsel_min(nm_fsel_max(val, valuemin), (double)valuemax);
}

//----------------------------------------------------------------------------------------------------------------------
template <class T> NM_FORCEINLINE T degreesToRadians(const T& angle) { return angle * (NM_PI / 180.0f); }
template <class T> NM_FORCEINLINE T radiansToDegrees(const T& angle) { return angle * (180.0f / NM_PI); }

//----------------------------------------------------------------------------------------------------------------------
/// \brief returns true if value is in the closed interval [valuemin, valuemax]
template <class T>
NM_FORCEINLINE bool valueInRange(T val, T valuemin, T valuemax)
{
  NMP_ASSERT(valuemin <= valuemax);
  return ((valuemin <= val) && (val <= valuemax));
}

//----------------------------------------------------------------------------------------------------------------------
/// Be aware that this function can take up a lot of cycles if min and max are close together
/// compared to the distance to val use specializations above if that is the case
template <class T>
NM_FORCEINLINE void wrapValue(T& val, T valuemin, T valuemax)
{
  NMP_ASSERT(valuemin < valuemax);
  const T delta(valuemax - valuemin);
  while (val < valuemin)
    val += delta;
  while (val >= valuemax)
    val -= delta;
}

//----------------------------------------------------------------------------------------------------------------------
// Specialisation for wrapping floating point numbers to a range in a way that has bounded cost.
// This implementation avoids any conditional branches by mapping the value to the unit range.
// This approach using floor() is much faster than an implementation that uses fmod()
template <>
NM_FORCEINLINE void wrapValue<float>(float& val, float valuemin, float valuemax)
{
  NMP_ASSERT(valuemin < valuemax);
  const float delta = valuemax - valuemin;
  
  // Apply the transform that maps [valuemin, valuemax] to the unit range [0, 1].
  // Note that in this adjusted space each integral unit corresponds to a division
  // of the wrapped space.
  float u = (val - valuemin) / delta;
  
  // Floor the adjusted value to get the integral value not greater than u, then remove
  // this offset to obtain the adjusted value in the canonical unit range [0, 1]
  u -= nmfloor(u);
  
  // Apply the inverse of the original transform to map the unit range back to [valuemin, valuemax]
  val = delta * u + valuemin;
}

//----------------------------------------------------------------------------------------------------------------------
// Specialisation for wrapping integer numbers to a range in a way that has bounded cost.
template <>
NM_FORCEINLINE void wrapValue<int32_t>(int32_t& val, int32_t valuemin, int32_t valuemax)
{
  NMP_ASSERT(valuemin < valuemax);
  const int32_t delta = valuemax - valuemin;

  // Compute the remainder after division. Note that for negative
  // values division is truncated towards zero so the remainder will
  // be negative. C (ISO 1999) standard.
  int32_t u = (val - valuemin) % delta;

  // For negative remainders we need to add the valuemax, otherwise
  // for positive (and zero) numbers add the valuemin.
  val = u + intSelect(u, valuemin, valuemax);
}

//----------------------------------------------------------------------------------------------------------------------
// Math functions
template <class T>
NM_FORCEINLINE T sqr(T val)
{
  return (val * val);
}

//----------------------------------------------------------------------------------------------------------------------
template <class T>
NM_FORCEINLINE T cube(T val)
{
  return (val * val * val);
}

//----------------------------------------------------------------------------------------------------------------------
// Fast approximate (reciprocal) sqrt functions (~23 bit accuracy)
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \fn NMP::fastReciprocalSqrt
/// \brief Optimized version of 1/sqrt; uses platform intrinsics where possible.
/// (Approximate variants improved with one Newton Raphson step)
/// \ingroup NaturalMotionPlatformMath
NM_FORCEINLINE float fastReciprocalSqrt(float x)
{
  NMP_ASSERT(x >= FLT_MIN);

#if defined(USE_FAST_APPROX_FUNCTIONS)
  // NR: (3-rsa*(val*rsa))*(rsa*0.5)
  #if defined(NM_HOST_X360)
  __declspec(align(16)) float ans[4];
  __vector4 val = __vspltw(__lvlx(&x, 0), 0);
  const __vector4 mone = __vspltw(__vupkd3d(__vspltisw(0), VPACK_NORMSHORT2), 3);
  const __vector4 mhalf = __vcfsx(__vspltisw(1), 1);
  const __vector4 mthree = __vaddfp(__vaddfp(mone, mone), mone);
  __vector4 rsqrt = __vrsqrtefp(val);  
  __vector4 asqrt = __vmulfp(val, rsqrt);
  __vector4 hrsqt = __vmulfp(rsqrt, mhalf);
  __vector4 sval  = __vsubfp(mthree, __vmulfp(asqrt, rsqrt));  
  __stvx(__vmulfp(hrsqt, sval), ans, 0);
  return ans[0];  
  #elif defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
  float rse;
  const __m128 mhalf = _mm_set_ss(0.5f);
  const __m128 mthree = _mm_set_ss(3.0f);
  __m128 val   = _mm_load_ss(&x);
  __m128 rsqrt = _mm_rsqrt_ss(val); // This will return #inf on denormalised floats (< FLT_MIN) hence the assert above
  __m128 asqrt = _mm_mul_ss(val, rsqrt);
  __m128 hrsqt = _mm_mul_ss(rsqrt, mhalf);
  __m128 sval  = _mm_sub_ss(mthree, _mm_mul_ss(asqrt, rsqrt));
  _mm_store_ss(&rse, _mm_mul_ss(hrsqt, sval));
  return rse;
  #elif defined(NM_HOST_CELL_PPU)
  vec_float4 val = vec_splats(x);
  const vec_float4 mzero = vec_splats(0.0f);
  const vec_float4 mhalf = vec_splats(0.5f);
  const vec_float4 mone  = vec_add(mhalf, mhalf);
  const vec_float4 mthree = vec_add(vec_add(mone, mone), mone);
  vec_float4 rsqrt = vec_rsqrte(val);
  vec_float4 asqrt  = vec_madd(val, rsqrt, mzero);
  vec_float4 hrsqt = vec_madd(rsqrt, mhalf, mzero);
  vec_float4 sval  = vec_sub(mthree, vec_madd(asqrt, rsqrt, mzero));
  return vec_extract(vec_madd(hrsqt, sval, mzero), 0);
  #elif defined(NM_HOST_CELL_SPU)
  vec_float4 val = spu_splats(x);
  const vec_float4 mzero = spu_splats(0.0f);
  const vec_float4 mhalf = spu_splats(0.5f);
  const vec_float4 mone  = spu_add(mhalf, mhalf);
  const vec_float4 mthree = spu_add(spu_add(mone, mone), mone);
  vec_float4 rsqrt = spu_rsqrte(val);
  vec_float4 asqrt  = spu_madd(val, rsqrt, mzero);
  vec_float4 hrsqt = spu_madd(rsqrt, mhalf, mzero);
  vec_float4 sval  = spu_sub(mthree, spu_madd(asqrt, rsqrt, mzero));
  return spu_extract(spu_madd(hrsqt, sval, mzero), 0);
  #else
    return 1.0f / sqrtf(x);
  #endif
#else // !USE_FAST_APPROX_FUNCTIONS
  return 1.0f / sqrtf(x);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
/// \fn NMP::fastSqrt
/// \brief Optimized version of sqrt; uses platform intrinsics where possible.
/// (Accuracy improved with one Newton Raphson step.)
/// \ingroup NaturalMotionPlatformMath
NM_FORCEINLINE float fastSqrt(float x)
{
  NMP_ASSERT(x >= 0.0f);

#if defined(USE_FAST_APPROX_FUNCTIONS)
  #if defined(NM_HOST_X360)
  // NR: (3-(sa*rsa))*(sa*0.5)
  __declspec(align(16)) float ans[4];
  __vector4 val = __vspltw(__lvlx(&x, 0), 0);
  const __vector4 mzero = __vzero();
  const __vector4 mone = __vspltw(__vupkd3d(__vspltisw(0), VPACK_NORMSHORT2), 3);
  const __vector4 mhalf = __vcfsx(__vspltisw(1), 1);
  const __vector4 mthree = __vaddfp(__vaddfp(mone, mone), mone);
  __vector4 rsqrt = __vrsqrtefp(val);  
  __vector4 cmpeq = __vcmpeqfp(val, mzero);
  rsqrt = __vsel(rsqrt, mzero, cmpeq);
  __vector4 asqrt = __vmulfp(val, rsqrt);
  __vector4 hasqt = __vmulfp(asqrt, mhalf);
  __vector4 sval  = __vnmsubfp(asqrt, rsqrt, mthree);  
  __stvx(__vmulfp(hasqt, sval), ans, 0);
  return ans[0];  
  #elif defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
  // NR: (3-(sa*rsa))*(sa*0.5)
  float rse;
  __m128 val = _mm_load_ss(&x);
  const __m128 mhalf = _mm_set_ss(0.5f);
  const __m128 mzero = _mm_setzero_ps();
  const __m128 mone = _mm_add_ss(mhalf, mhalf);
  const __m128 mthree = _mm_add_ss(mone, _mm_add_ss(mone, mone));  
  __m128 cmpeq = _mm_cmpeq_ss(mzero, val);
  __m128 rsqrt = _mm_rsqrt_ss(_mm_add_ss(_mm_and_ps(cmpeq, mone), val));
  __m128 asqrt = _mm_mul_ss(val, rsqrt);
  __m128 hasqt = _mm_mul_ss(asqrt, mhalf);
  __m128 sval  = _mm_sub_ss(mthree, _mm_mul_ss(asqrt, rsqrt));   
  _mm_store_ss(&rse, _mm_mul_ss(hasqt, sval));
  return rse;
  #elif defined(NM_HOST_CELL_PPU)
  // NR:  sa*(1 - 0.5*((rsa*sa)-1))
  vec_float4 val = vec_splats(x);
  const vec_float4 mnhalf = vec_splats(-0.5f);
  const vec_float4 mzero = vec_splats(0.0f);
  const vec_float4 mnone = vec_add(mnhalf, mnhalf);
  vec_float4 rsa = vec_rsqrte(val);
  rsa = vec_sel(rsa, mzero, vec_cmpeq(val, mzero));
  vec_float4 sa = vec_madd(rsa, val, mzero);
  vec_float4 sanh = vec_madd(sa, mnhalf, mzero);
  return vec_extract(vec_madd(vec_madd(rsa, sa, mnone), sanh, sa), 0);
  #elif defined(NM_HOST_CELL_SPU)
  // NR:  sa*(1 - 0.5*((rsa*sa)-1))
  vec_float4 val = spu_splats(x);
  const vec_float4 mnhalf = spu_splats(-0.5f);
  const vec_float4 mzero = spu_splats(0.0f);
  const vec_float4 mnone = spu_add(mnhalf, mnhalf);
  vec_float4 rsa = spu_rsqrte(val);
  rsa = spu_sel(rsa, mzero, spu_cmpeq(val, mzero));
  vec_float4 sa = spu_madd(rsa, val, mzero);
  vec_float4 sanh = spu_madd(sa, mnhalf, mzero);
  return spu_extract(spu_madd(spu_madd(rsa, sa, mnone), sanh, sa), 0);
  #else
  return sqrtf(x);
  #endif
#else // !USE_FAST_APPROX_FUNCTIONS
  #if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
  float sq;
  _mm_store_ss(&sq, _mm_sqrt_ss(_mm_load_ss(&x)));
  return sq;
  #else
  return sqrtf(x);
  #endif
#endif
}

//----------------------------------------------------------------------------------------------------------------------
// Approximate, faster (reciprocal) sqrt functions (~4 significant figure accuracy)
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \fn NMP::approxReciprocalSqrt
/// \brief Speed optimized version of 1/sqrt; using platform intrinsics without Newton Raphson step
/// \ingroup NaturalMotionPlatformMath
NM_FORCEINLINE float approxReciprocalSqrt(float x)
{
  NMP_ASSERT(x > 0.0f);

#if defined(NM_HOST_X360)
  return (float)__frsqrte(x);  
#elif defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
  float rse;
  _mm_store_ss(&rse, _mm_rsqrt_ss(_mm_load_ss(&x)));
  return rse;
#elif defined(NM_HOST_CELL_PPU) 
  return vec_extract(vec_rsqrte(vec_splats(x)), 0);
#elif defined(NM_HOST_CELL_SPU)
  return spu_extract(spu_rsqrte(spu_splats(x)), 0);
#else
  return 1.0f / sqrtf(x);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
/// \fn NMP::approxSqrt
/// \brief Speed optimized version of sqrt; using platform intrinsics without Newton Raphson step
/// \ingroup NaturalMotionPlatformMath
NM_FORCEINLINE float approxSqrt(float x)
{
  NMP_ASSERT(x >= 0.0f);

#if defined(NM_HOST_X360)
  // This version zeros negatives:
  float rsa = (float)__frsqrte(x);
  rsa = __fself(-x, 0.0f, rsa);  // correct for zero
  return rsa * x;
#elif defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
  float rse;
  __m128 val  = _mm_load_ss(&x);
  __m128 cmpz = _mm_cmpeq_ss(_mm_setzero_ps(), val);
  const __m128 mone  = _mm_set_ss(1.0f);  
  _mm_store_ss(&rse, _mm_mul_ss(val, _mm_rsqrt_ss(_mm_add_ss(val, _mm_and_ps(cmpz, mone)))));
  return rse;
#elif defined(NM_HOST_CELL_PPU)
  vec_float4 val = vec_splats(x);
  const vec_float4 mzero = vec_splats(0.0f);
  vec_float4 rsa = vec_rsqrte(val);
  rsa = vec_sel(rsa, mzero, vec_cmpeq(val, mzero));
  return vec_extract(vec_madd(rsa, val, mzero), 0);
#elif defined(NM_HOST_CELL_SPU)
  vec_float4 val = spu_splats(x);
  const vec_float4 mzero = spu_splats(0.0f);
  vec_float4 rsa = spu_rsqrte(val);
  rsa = spu_sel(rsa, mzero, spu_cmpeq(val, mzero));
  return spu_extract(spu_madd(rsa, val, mzero), 0);
#else
  return sqrtf(x);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
// Approximate fast trig functions
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \fn NMP::fastSin
/// \brief Compute an approximation to the trigonometric sine function
/// by using a modified Taylors series representation. The adjusted coefficients
/// distribute the error over the entire angle range and has a maximum error of
/// the order 5e-6.
/// \ingroup NaturalMotionPlatformMath
/// \param x assumed to be in the range [-pi, pi]
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float fastSin(float x)
{
  NMP_ASSERT(fabs(x) <= NM_PI);

#ifdef USE_FAST_TRIG_FUNCTIONS

  static const float two_pi = 2.0f / NM_PI; // 2 / pi
  static const float map_a[5] = { -1.0f, 1.0f, 1.0f, -1.0f, -1.0f };
  static const float map_b[5] = { -NM_PI, 0.0f, 0.0f, NM_PI, NM_PI };
  static const float coefs[4] = {
    1.0f,
    -0.166658378375724f,
    0.008314539677347f,
    -0.000185599017744f
  };

  // Determine which quadrant the value is in
  uint32_t quad = (uint32_t)(two_pi * x + 2.0f);

  // Modify x depending on the quadrant
  x = map_a[quad] * x + map_b[quad];

  // Taylors series
  float x2 = x * x;
  return (coefs[0] + (coefs[1] + (coefs[2] + coefs[3] * x2) * x2) * x2) * x;

#else
  return sinf(x);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief uses the minimax polynomial approximation to arcsine.
/// \ingroup NaturalMotionPlatformMath
/// \param x assumed to be in the range [-1, 1]
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float fastArcsin(float x)
{
  NMP_ASSERT(x >= -1.0f);
  NMP_ASSERT(x <= 1.0f);

#ifdef USE_FAST_TRIG_FUNCTIONS

  const float top0 = 1.507095111f;
  const float top1 = 1.103007131f;
  const float top2 = -2.144008022f;

  const float btm0 = 1.507171600f;
  const float btm1 = -0.4089766186f;
  const float btm2 = -0.9315200116f;
  const float btm3 = 0.2836182315f;
  const float btm4 = -0.1535779990f;

  bool sign_test = x < 0;
  x = sign_test ? -x : x;

  const float top    = top0 + (top1 + top2 * x) * x;
  const float bottom = btm0 + (btm1 + (btm2 + (btm3 + btm4 * x) * x) * x) * x;

  float result = (top / bottom) - fastSqrt(1.0f - x * x);
  return sign_test ? -result : result;
#else
  return asinf(x);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief computes the arctan using the following identity
///
///         arctan(x) = arcsin(x / sqrt(1 + x^2))
///
/// \ingroup NaturalMotionPlatformMath
/// \param x assumed to be in the range [-1, 1]
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float fastArctan(float x)
{
#ifdef USE_FAST_TRIG_FUNCTIONS
  #ifdef USE_FAST_APPROX_FUNCTIONS  
  return fastArcsin(x * fastReciprocalSqrt(1.0f + x * x));
  #else
  return fastArcsin(x / sqrt(1 + x * x));
  #endif
#else
  return atanf(x);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief computes the arccos using the following identity
///         arccos(x) = PI/2 - arcsin(x)
/// \ingroup NaturalMotionPlatformMath
/// \param x assumed to be in the range [-1, 1]
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float fastArccos(float x)
{
#ifdef USE_FAST_TRIG_FUNCTIONS
  return NM_PI_OVER_TWO - fastArcsin(x);
#else
  return acosf(x);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Compute an approximation to the trigonometric sine and cosine functions by using a polynomial function representation.
/// \ingroup NaturalMotionPlatformMath
///
/// This function uses two polynomial
/// curves to represent the canonical angle range (adjusted Taylors series about 0
/// and pi). The adjusted coefficients distribute the error over the entire angle
/// range and has a maximum error of the order 2e-4. This function can represent all
/// POSITIVE finite angles.
/// \param theta must be a positive but arbitrary magnitude angle
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void fastSinCos(
  float  theta, ///< Theta must be a positive but arbitrary magnitude angle.
  float& s,     ///< Sin result.
  float& c      ///< Cos result.
)
{
  // This function can actually represent negative angles up to -pi/2.
  // Don't assert theta >= 0 since there may be small negative numerical errors
  // when calling fastSinCos(theta) with theta = fastArccos(c).
  NMP_ASSERT(theta >= -1.570796326794897f);

  // Static coefficients
  static const float two_pi = 0.636619772367581f;
  static const float pi_two = 1.570796326794897f;
  static const float ang_offset[4] = { 0.0f, -1.570796326794897f, 0.0f, -1.570796326794897f };
  static const uint32_t quad_map[4] = { 0, 1, 1, 0 };

  static const float sin_coefs[2][3] = {
    { 1.0f, -0.16613412016702f, 0.007667296842758f },
    { -1.0f, 0.16613412016702f, -0.007667296842758f }
  };

  static const float cos_coefs[2][4] = {
    { 1.0f, -0.50005809815278f, 0.041657257163128f, -0.001318224442104f },
    { -1.0f, 0.50005809815278f, -0.041657257163128f, 0.001318224442104f }
  };

  // Compute angle mapped into the [0:2pi] range
  uint32_t r = (uint32_t)(theta * two_pi); // find which pi/2 segment
  uint32_t quad = (r & 0x03); // select which pi/2 quadrant in [0:2*pi]
  float alpha = (theta - r * pi_two) + ang_offset[quad]; // Adjusted residual angle
  float u = alpha * alpha;

  // Evaluate the Taylors series of the sine function
  const float* coefs = (const float*)sin_coefs[ quad_map[quad] ];
  s = alpha * (coefs[0] + (coefs[1] + coefs[2] * u) * u);

  // Evaluate the Taylors series of the cosine function
  coefs = (const float*)cos_coefs[ quad_map[quad] ];
  c = coefs[0] + (coefs[1] + (coefs[2] + coefs[3] * u) * u) * u;
}

//----------------------------------------------------------------------------------------------------------------------
/// \fn NMP::fastAtan2
/// \brief Compute an approximation to the trigonometric atan2 function by using
/// a polynomial function representation. atan2 uses the signs of both parameters
/// to determine the quadrant of the return value.
///
/// The atan function is approximated by using two curve pieces that divide the input 
/// value range between u = [-1:1]. This is possible because of the trig identity:
///
/// atan(u) = u - u^3/3 + u^5/5 - u^7/7 ...  -1 < u < 1
///
/// atan(u) = pi/2 - atan(1/u)              u < -1, u > 1
///
/// The Taylors coefficients have been adjusted to distribute the error over the
/// entire curve and has a maximum angle error of the order 2e-4. The returned
/// angle is in the range [-pi:pi]
///
/// \param x The input x component which can be any finite number.
/// \param y The input y component which can be any finite number.
/// \return The arctangent of y / x in the range –pi to pi radians
/// \ingroup NaturalMotionPlatformMath
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float fastAtan2(float y, float x)
{
  NMP_ASSERT(!(y == 0.0f && x == 0.0f));

  // Static coefficients
  static const float atan_coefs[3][5] = {
    { 0.0f, 0.999314747965817f, -0.322266683652863f, 0.14898078328198f, -0.040826575272239f },
    { 3.141592653589793f, 0.999314747965817f, -0.322266683652863f, 0.14898078328198f, -0.040826575272239f },
    { 1.570796326794897f, -0.999314747965817f, 0.322266683652863f, -0.14898078328198f, 0.040826575272239f }
  };

  float theta, u;
  const float* coefs;

  if (nmfabs(x) > nmfabs(y))
  {
    // atan(u) = u - u^3/3 + u^5/5 - u^7/7 ...  -1 < u < 1
    coefs = (const float*)atan_coefs[x < 0];
    u = y / x;
  }
  else
  {
    // atan(u) = pi/2 - atan(1/u)       u < -1, u > 1
    u = x / y;
    coefs = (const float*)atan_coefs[2];
  }

  // Check for negative angles
  theta = floatSelect(y, coefs[0], -coefs[0]);

  // Compute the polynomial approximation for the atan(u) function
  float uu = u * u;
  theta += u * (coefs[1] + (coefs[2] + (coefs[3] + coefs[4] * uu) * uu) * uu);

  return theta;
}

//----------------------------------------------------------------------------------------------------------------------
/// \fn NMP::triangleBarycentricCoords
/// \brief Converts the input Cartesian point P into the Barycentric coordinates
/// [lambda1, lambda2, lambda3] for a triangle whose vertices are {A, B, C}.
/// P = lambda1 * A + lambda2 * B + lambda3 * C
/// \ingroup NaturalMotionPlatformMath
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void triangleBarycentricCoords(
  float Ax,
  float Ay,
  float Bx,
  float By,
  float Cx,
  float Cy,
  float Px,
  float Py,
  float& lambda1,
  float& lambda2,
  float& lambda3)
{
  // Set up the 2x2 system of linear equations
  float T[4];
  T[0] = Ax - Cx;
  T[1] = Bx - Cx;
  T[2] = Ay - Cy;
  T[3] = By - Cy;
  float b[2];
  b[0] = Px - Cx;
  b[1] = Py - Cy;

  // Solve
  float det = T[0] * T[3] - T[1] * T[2];
  NMP_ASSERT(det != 0.0f);
  float x[2];
  x[0] = (T[3] * b[0] - T[1] * b[1]) / det;
  x[1] = (T[0] * b[1] - T[2] * b[0]) / det;

  // Set the output Barycentric coordinates
  lambda1 = x[0];
  lambda2 = x[1];
  lambda3 = 1.0f - x[0] - x[1];
}

//----------------------------------------------------------------------------------------------------------------------
/// \fn NMP::triangleBarycentricToBilinear
/// \brief Converts the Barycentric coordinates [lambda1, lambda2, lambda3] for a triangle
/// whose vertices are {A, B, C} into bilinear blending coefficients [alpha, beta] such that
/// lambda1 * A + lambda2 * B + lambda3 * C = lerp( lerp(A, B, alpha), C, beta )
/// \ingroup NaturalMotionPlatformMath
//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
// Disable 'potential divide by 0.
#pragma warning(push)
#pragma warning(disable: 4723)
#endif // _MSC_VER
NM_FORCEINLINE void triangleBarycentricToBilinear(
  float NMP_UNUSED(lambda1),
  float lambda2,
  float lambda3,
  float& alpha,
  float& beta)
{
  float omu = 1.0f - lambda3;
  if (nmfabs(omu) < 1e-4f)
  {
    alpha = 0.0f;
    beta = 1.0f;
  }
  else
  {
    alpha = lambda2 / omu;
    beta = lambda3;
  }
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER

//----------------------------------------------------------------------------------------------------------------------
/// \fn NMP::triangleBarycentricFromBilinear
/// \brief Converts the bilinear blending coefficients [alpha, beta] into the Barycentric
/// coordinates [lambda1, lambda2, lambda3] for a triangle whose vertices are {A, B, C} such that
/// lambda1 * A + lambda2 * B + lambda3 * C = lerp( lerp(A, B, alpha), C, beta )
/// \ingroup NaturalMotionPlatformMath
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void triangleBarycentricFromBilinear(
  float& lambda1,
  float& lambda2,
  float& lambda3,
  float alpha,
  float beta)
{
  float omb = 1.0f - beta;
  lambda1 = (1 - alpha) * omb;
  lambda2 = alpha * omb;
  lambda3 = beta;
}

//----------------------------------------------------------------------------------------------------------------------
/// \fn NMP::jacobi
/// \brief  Compute the parameters of a 2 by 2 Jacobi transformation D = V^T * A * V
///         that diagonalizes the symmetric matrix A. The resulting columns of V
///         contain the Eigen-vectors of the system and the elements of the diagonal
///         matrix correspond to the Eigen-values. V = [cos(t) sin(t); -sin(t) cos(t)]
///
/// \param  aij - The elements A_{ i, j } of the upper triagle of matrix A
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void jacobi(float a11, float a12, float a22, float& c, float& s)
{
  if (a12 == 0.0f)
  {
    c = 1.0f;
    s = 0.0f;
  }
  else
  {
    float tau = (a22 - a11) / (2.0f * a12);
    float a = floatSelect(tau, 1.0f, -1.0f);
    float p = sqrtf(1.0f + tau * tau);
    float t = a / (a * tau + p);
    c = 1.0f / sqrtf(1.0f + t * t);
    s = t * c;
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Critically damped method of making a variable smoothly approach a value.
/// \ingroup NaturalMotionPlatformMath
///
/// When calling this you need to keep track of the rate of change of the value (valRate).
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
NM_INLINE void SmoothCD(
  T&          val,       ///< in/out: value to be smoothed.
  T&          valRate,   ///< in/out: rate of change of the value.
  const float timeDelta, ///< in: time interval.
  const T&    to,        ///< in: the target value.
  const float smoothTime ///< in: timescale for smoothing.
)
{
  if (smoothTime > 0.0f)
  {
    float omega = 2.0f / smoothTime;
    float x = omega * timeDelta;
    float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
    T change = val - to;
    T temp = (valRate + change * omega) * timeDelta;
    valRate = (valRate - temp * omega) * exp;
    val = to + (change + temp) * exp;
  }
  else if (timeDelta > 0.0f)
  {
    valRate = (to - val) / timeDelta;
    val = to;
  }
  else
  {
    val = to;
    valRate -= valRate; // zero it...
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Exponential damping of a variable towards a target
/// \ingroup NaturalMotionPlatformMath
///
/// Smooths val towards the target to with a timestep of timeDelta, smoothing over smoothTime. This
/// avoids the cost of evaluating the exponential function by a second order expansion, and is
/// stable and accurate (for all practical purposes) for all timeDelta values.
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
NM_INLINE void smoothExponential(
  T&          val,       ///< in/out: value to be smoothed.
  const float timeDelta, ///< in: time interval.
  const T&    to,        ///< in: the target value.
  const float smoothTime ///< in: timescale for smoothing.
  )
{
  float lambda = timeDelta / smoothTime;
  val = to + (val - to) / (1.0f + lambda + 0.5f * lambda * lambda);
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_MATHUTILS_H
//----------------------------------------------------------------------------------------------------------------------
