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

#ifndef NMP_RNG_H
#define NMP_RNG_H

#include "NMPlatform/NMPlatform.h"
#include <float.h>
#include <limits.h>

namespace NMP
{

// ---------------------------------------------------------------------------------------------------------------------
/// \class RNG
/// \brief Efficient pseudo-random number generator for general-purpose use
/// \ingroup NaturalMotionPlatform
// ---------------------------------------------------------------------------------------------------------------------
class RNG
{
public:
  NM_INLINE RNG();                                          // initialize with default seed
  NM_INLINE RNG(uint32_t seed);

  NM_INLINE void setSeed(uint32_t seed);
  NM_INLINE float genFloat();                               // float in range [0..1]
  NM_INLINE float genFloat(float minfv, float maxfv);       // float in range [minfv..maxfv]
  NM_INLINE float genApproxBellCurve01();                   // float in range [0..1] in approximate bell curve distribution
  NM_INLINE uint32_t genUInt32();                           // uint in range [0..UINT_MAX]
  NM_INLINE uint32_t genUInt32(uint32_t min, uint32_t max); // uint in range [min, max]
  NM_INLINE bool genBool() { return (genUInt32() & 1); }

private:
  uint32_t  m_seed,
         m_carry;
};

// ---------------------------------------------------------------------------------------------------------------------
RNG::RNG()
{
  setSeed(6291469);
}

// ---------------------------------------------------------------------------------------------------------------------
RNG::RNG(uint32_t seed)
{
  setSeed(seed);
}

// ---------------------------------------------------------------------------------------------------------------------
void RNG::setSeed(uint32_t seed)
{
  // avoid seed == 0
  m_seed = seed + !seed;

  // scatter bits into the carry
  m_carry = (m_seed ^ 0xe995) ^ (m_seed >> 16);
  m_carry += (m_carry << 3);
  m_carry ^= (m_carry >> 4);
  m_carry *= 0x27d4eb2d;
  m_carry ^= (m_carry >> 15);
}

// ---------------------------------------------------------------------------------------------------------------------
uint32_t RNG::genUInt32()
{
  // multiply-with-carry (MWCG) approach by George Marsaglia (~1994)
  // http://cliodhna.cop.uop.edu/~hetrick/na_faq.html

  // see link for possible values of a
  const uint64_t a = 2051013963;

  // a * x + c
  uint64_t temp = a * (uint64_t)m_seed + (uint64_t)m_carry;

  // ..Then the new x is the bottom 32 bits.
  //    the new carry is the top 32 bits.
  m_seed = (uint32_t)(temp & UINT_MAX);
  m_carry = (uint32_t)((temp >> 32) & UINT_MAX);

  return m_seed;
}

// ---------------------------------------------------------------------------------------------------------------------
uint32_t RNG::genUInt32(uint32_t min, uint32_t max)
{
  return (uint32_t)(genFloat() * (max - min) + min);
}

// ---------------------------------------------------------------------------------------------------------------------
float RNG::genFloat()
{
  return (genUInt32() & 0x007fffff) * FLT_EPSILON;
}

// ---------------------------------------------------------------------------------------------------------------------
float RNG::genFloat(float minfv, float maxfv)
{
  return (genFloat() * (maxfv - minfv) + minfv);
}

// ---------------------------------------------------------------------------------------------------------------------
float RNG::genApproxBellCurve01()
{
  // sum-of-uniforms / central limit theorem; a fast way to produce an approximate bell curve distribution
  // http://en.wikipedia.org/wiki/Illustration_of_the_central_limit_theorem
  float r0 = genFloat();
  float r1 = genFloat();
  float r2 = genFloat();
  float r3 = genFloat();
  return (r0 + r1 + r2 + r3) * 0.25f;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_RNG_H
//----------------------------------------------------------------------------------------------------------------------
