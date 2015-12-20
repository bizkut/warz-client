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
#if (!defined(NMTL_HASHFUNC_H) && !defined(NMTL_NO_SENTRY)) || defined(NMTL_NO_SENTRY)
#ifndef NMTL_NO_SENTRY
  #define NMTL_HASHFUNC_H
#endif

//----------------------------------------------------------------------------------------------------------------------
#include "nmtl/defs.h"
#include <string.h>
#include <wchar.h>
#include <assert.h>

// for uintptr_t
#if defined(_MSC_VER)
  #include <crtdefs.h>
#else
  #include <stdint.h>
#endif //_MSC_VER

#ifndef NMTL_TEMPLATE_EXPORT
  #define NMTL_TEMPLATE_EXPORT
  #define NMTL_KILL_TEMPLATE_EXPORT
#endif
#ifndef NMTL_NAMESPACE_OVERRIDE
  #define NMTL_NAMESPACE nmtl
#else
  #define NMTL_NAMESPACE NMTL_NAMESPACE_OVERRIDE
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMTL_NAMESPACE
{

#if defined(_MSC_VER)
  typedef unsigned __int64 hkey64;
#else
  typedef unsigned long long hkey64;
#endif

//----------------------------------------------------------------------------------------------------------------------
// MurmurHash2, by Austin Appleby
// Note - This code makes a few assumptions about how your machine behaves -
//
// 1. We can read a 4-byte value from any address without crashing
// 2. sizeof(int) == 4
//
// And it has a few limitations -
//
// 1. It will not work incrementally.
// 2. It will not produce the same results on little-endian and big-endian
//    machines.
//
// from http://murmurhash.googlepages.com/
// code is released to the public domain
//----------------------------------------------------------------------------------------------------------------------
NMTL_INLINE unsigned int NMTL_TEMPLATE_EXPORT MurmurHash2(const void* key, unsigned int len, unsigned int seed)
{
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.
  const unsigned int m = 0x5bd1e995;
  const int r = 24;

  // Initialize the hash to a 'random' value
  unsigned int h = seed ^ len;

  // Mix 4 bytes at a time into the hash
  const unsigned char* data = (const unsigned char*)key;

  while (len >= 4)
  {
    unsigned int k = *(const unsigned int*)data;        //lint !e826 cast to ptr of larger size

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    data += 4;
    len -= 4;
  }

  // Handle the last few bytes of the input array

  switch (len)
  {
  //lint --e{ 616, 825 } // control flows into case/default
  case 3: h ^= data[2] << 16;
  case 2: h ^= data[1] << 8;
  case 1: h ^= data[0];
    h *= m;
  }; //lint !e744 no default

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.
  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
}

//----------------------------------------------------------------------------------------------------------------------
// modification of SuperFastHash, by Paul Hsieh
// very slightly changed to operate on integer arrays instead of 8-bit streams
//
// from http://www.azillionmonkeys.com/qed/hash.html
// licence details : http://www.azillionmonkeys.com/qed/weblicense.html
//
// "source code shown on my website can be used freely under the derivative license,
//  (and may be distributed under a public domain license, whether compiled into another
//   program or not, for example)"
//
// "The content may be used for commercial purposes."
//----------------------------------------------------------------------------------------------------------------------
NMTL_INLINE unsigned int NMTL_TEMPLATE_EXPORT HsiehHash32b(const unsigned int* data, unsigned int numberOfInts, unsigned int seed)
{
  unsigned int hash = seed ^ numberOfInts, key, tmp;
  for (unsigned int i = 0; i < numberOfInts; i++)
  {
    key = (*data);
    hash += key & 0x0000FFFF;
    tmp   = ((key & 0xFFFF0000) >> 5) ^ hash;
    data ++;

    hash  = (hash << 16) ^ tmp;
    hash += hash >> 11;
  }

  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;

  return hash;
}

//----------------------------------------------------------------------------------------------------------------------
namespace hfn
{

// well-performing hash function for a single u32 value
NMTL_INLINE unsigned int NMTL_TEMPLATE_EXPORT hashFunction(unsigned int key)
{
  key = (key ^ 0xe995) ^ (key >> 16);
  key += (key << 3);
  key ^= (key >> 4);
  key *= 0x27d4eb2d;
  key ^= (key >> 15);
  return key;
}

NMTL_INLINE unsigned int NMTL_TEMPLATE_EXPORT hashFunction(int key)
{
  return hashFunction((unsigned int)key);
}

//----------------------------------------------------------------------------------------------------------------------
// well performing hash function for a single u64 value
NMTL_INLINE unsigned int NMTL_TEMPLATE_EXPORT hashFunction(hkey64 key)
{
  unsigned int hash = 0x0000000A, tmp;

  // unrolled HsiehHash for the 64-bit value, seems to perform well
  // see the comment above SuperFastHash for information about HsiehHash
  hash += (unsigned int)(key & 0xFFFF);
  tmp   = ((unsigned int)(key & 0xFFFF0000) >> 5) ^ hash;
  hash  = (hash << 16) ^ tmp;
  hash += hash >> 11;

  hash += (unsigned int)((key >> 32) & 0xFFFF);
  tmp   = (((unsigned int)(key >> 32) & 0xFFFF0000) >> 5) ^ hash;
  hash  = (hash << 16) ^ tmp;
  hash += hash >> 11;

  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;

  return hash;
}

#ifdef __INTEL_COMPILER
  #pragma warning(push)
  #pragma warning(disable : 1684) // pointer truncation from 'void *' to 'uintptr_t'
#endif // _MSC_VER

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable : 4311 4244) // pointer truncation from 'void *' to 'uintptr_t'
#endif // _MSC_VER

// should catch and redirect all pointer keys to the ptrdiff_t hash
NMTL_INLINE unsigned int NMTL_TEMPLATE_EXPORT hashFunction(void* key) { return hashFunction(reinterpret_cast<hkey64>(key)); }

NMTL_INLINE unsigned int NMTL_TEMPLATE_EXPORT hashFunction(void const* key) { return hashFunction(reinterpret_cast<hkey64>(key)); }

#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
  #pragma warning(pop)
#endif

// equality test stub
template <typename KeyType>
NMTL_INLINE bool NMTL_TEMPLATE_EXPORT hashKeysEqual(const KeyType& lhs, const KeyType& rhs) { return lhs == rhs; }

//----------------------------------------------------------------------------------------------------------------------
// support for hashing string data
// these can be used to hash strings into values suitable for keys
NMTL_INLINE unsigned int NMTL_TEMPLATE_EXPORT hashString(const char* key)
{
  return MurmurHash2(key,
                     key ? (static_cast<unsigned int>(strlen(key) * sizeof(char))) : 0U,
                     0x14B2B00B);
}

NMTL_INLINE unsigned int NMTL_TEMPLATE_EXPORT hashString(const wchar_t* key)
{
  return MurmurHash2(key,
                     key ? (static_cast<unsigned int>(wcslen(key) * sizeof(wchar_t))) : 0U,
                     0x8F5F144A);
}

NMTL_INLINE unsigned int NMTL_TEMPLATE_EXPORT hashFunction(const char* key)
{
  return hashString(key);
}

NMTL_INLINE unsigned int NMTL_TEMPLATE_EXPORT hashFunction(const wchar_t* key)
{
  return hashString(key);
}

template <typename KeyType>
NMTL_INLINE bool NMTL_TEMPLATE_EXPORT hashKeysEqual(const char* lhs, const char* rhs) { return strcmp(lhs, rhs) == 0; }

template <typename KeyType>
NMTL_INLINE bool NMTL_TEMPLATE_EXPORT hashKeysEqual(const wchar_t* lhs, const wchar_t* rhs) { return wcscmp(lhs, rhs) == 0; }

//----------------------------------------------------------------------------------------------------------------------
} // namespace hfn

//----------------------------------------------------------------------------------------------------------------------
// Given the input value, this function will return the next biggest prime number
// in the chosen sequence. Each entry is approximately half way between the two nearest
// power-of-two values.
//----------------------------------------------------------------------------------------------------------------------
NMTL_INLINE unsigned int nextPrime(unsigned int nv)
{
  if (nv < 11) return 11;
#define NMTL_HSIZE(_v) else if (nv < _v) return _v
  NMTL_HSIZE(17);
  NMTL_HSIZE(53);
  NMTL_HSIZE(97);
  NMTL_HSIZE(193);
  NMTL_HSIZE(389);
  NMTL_HSIZE(769);
  NMTL_HSIZE(1543);
  NMTL_HSIZE(3079);
  NMTL_HSIZE(6151);
  NMTL_HSIZE(12289);
  NMTL_HSIZE(24593);
  NMTL_HSIZE(49157);
  NMTL_HSIZE(98317);
  NMTL_HSIZE(196613);
  NMTL_HSIZE(393241);
  NMTL_HSIZE(786433);
  NMTL_HSIZE(1572869);
  NMTL_HSIZE(3145739);
  NMTL_HSIZE(6291469);
#undef NMTL_HSIZE
  assert(0);
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Return the next largest power of two
NMTL_INLINE unsigned int nextPow2(unsigned int nv)
{
  --nv;
  nv |= nv >> 1;
  nv |= nv >> 2;
  nv |= nv >> 4;
  nv |= nv >> 8;
  nv |= nv >> 16;
  return ++nv;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMTL_NAMESPACE

#undef NMTL_NAMESPACE
#ifdef NMTL_KILL_TEMPLATE_EXPORT
  #undef NMTL_TEMPLATE_EXPORT
#endif

#endif // NMTL_HASHTABLE_H
