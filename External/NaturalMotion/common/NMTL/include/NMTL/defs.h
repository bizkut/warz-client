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
#ifndef NMTL_DEFS_H
#define NMTL_DEFS_H

//----------------------------------------------------------------------------------------------------------------------
#if defined(__APPLE__) && defined(__arm__)
#define NMTL_IOS
#endif

//----------------------------------------------------------------------------------------------------------------------
#include <cstddef>
#if !defined(__GCCXML_GNUC__) && defined(WIN32)
  #include <intrin.h>
#endif

#if defined(_MSC_VER)
  #include <crtdbg.h>

  // turn off C4985 : 'ceil': attributes not present on previous declaration warning
  // for VS2008/x64 - bug in CRT headers - ceil declared differently in math.h and intrin.h.
  #if defined (_WIN64) && _MSC_VER == 1500
    #pragma warning (disable : 4985)
  #endif

  // type_traits was added in VS2008 SP1
  // Don't include if this is a GCC-XML pass acting like MSVC
  #if (_MSC_VER >= 1500) && (_MSC_FULL_VER >= 150030729) && !defined(__GNUC__)
    #include <type_traits> // TR1
    #define NMTL_HAS_TR1
  #endif

  #if !defined(__GCCXML_GNUC__)
    #include <xmmintrin.h>
    #define NMTL_HAS_MSVC_INTRINSICS
  #endif
#endif // _MSC_VER
#ifndef NMTL_NAMESPACE_OVERRIDE
  #define NMTL_NAMESPACE nmtl
#else
  #define NMTL_NAMESPACE NMTL_NAMESPACE_OVERRIDE
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMTL_NAMESPACE
{

#ifndef NMTL_NO_DEFAULT_ALLOCATOR
  #define NMTL_NO_DEFAULT_ALLOCATOR 0
#endif
/// \brief  exports the vector to a DLL. It will export,
///           template LINK_TYPE class vector<ELEM_TYPE>
///         and related classes...
#define NMTL_VECTOR_EXPORT(ELEM_TYPE, LINK_DEFINE, NAMESPACE) \
  template LINK_DEFINE class NAMESPACE ::vector_iterator<ELEM_TYPE>; \
  template LINK_DEFINE class NAMESPACE ::vector_const_iterator<ELEM_TYPE>; \
  template LINK_DEFINE class NAMESPACE ::vector_reverse_iterator<ELEM_TYPE>; \
  template LINK_DEFINE class NAMESPACE ::vector_const_reverse_iterator<ELEM_TYPE>; \
  template LINK_DEFINE class NAMESPACE ::vector<ELEM_TYPE>;

/// \brief  exports the vector to a DLL. It will export,
///           template LINK_TYPE class vector<ELEM_TYPE>
///         and related classes...
#define NMTL_POD_VECTOR_EXPORT(ELEM_TYPE, LINK_DEFINE, NAMESPACE) \
  template LINK_DEFINE class NAMESPACE ::vector_iterator<ELEM_TYPE>; \
  template LINK_DEFINE class NAMESPACE ::vector_const_iterator<ELEM_TYPE>; \
  template LINK_DEFINE class NAMESPACE ::vector_reverse_iterator<ELEM_TYPE>; \
  template LINK_DEFINE class NAMESPACE ::vector_const_reverse_iterator<ELEM_TYPE>; \
  template LINK_DEFINE class NAMESPACE ::pod_vector<ELEM_TYPE>;

/// \brief  exports the vector to a DLL. It will export,
///         template LINK_TYPE class stack<ELEM_TYPE>
///         and related classes...
#define NMTL_STACK_EXPORT(ELEM_TYPE, LINK_DEFINE, NAMESPACE) \
  NMTL_VECTOR_EXPORT(ELEM_TYPE, LINK_DEFINE, NAMESPACE) \
  template LINK_DEFINE class NAMESPACE ::stack<ELEM_TYPE>;

/// \brief  exports the vector to a DLL. It will export,
///           template LINK_TYPE class stack<ELEM_TYPE>
///         and related classes...
#define NMTL_POD_STACK_EXPORT(ELEM_TYPE, LINK_DEFINE, NAMESPACE) \
  NMTL_POD_VECTOR_EXPORT(ELEM_TYPE, LINK_DEFINE, NAMESPACE) \
  template LINK_DEFINE class NAMESPACE ::pod_stack<ELEM_TYPE>;

/// \brief  exports the vector to a DLL. It will export,
///           template LINK_TYPE class set<ELEM_TYPE>
///         and related classes...
#define NMTL_SET_EXPORT(ELEM_TYPE, LINK_DEFINE, NAMESPACE) \
  NMTL_VECTOR_EXPORT(ELEM_TYPE, LINK_DEFINE, NAMESPACE) \
  template LINK_DEFINE class NAMESPACE ::set<ELEM_TYPE>;

/// \brief  exports the vector to a DLL. It will export,
///           template LINK_TYPE class deque<ELEM_TYPE>
///         and related classes...
#define NMTL_DEQUE_EXPORT(ELEM_TYPE, LINK_DEFIN, NAMESPACE) \
  NMTL_VECTOR_EXPORT(ELEM_TYPE, LINK_DEFINE, NAMESPACE) \
  template LINK_DEFINE class NAMESPACE ::deque<ELEM_TYPE>;

/// \brief  exports the vector to a DLL. It will export,
///           template LINK_TYPE class list<ELEM_TYPE>
///         and related classes...
#define NMTL_LIST_EXPORT(ELEM_TYPE, LINK_DEFINE, NAMESPACE) \
  NMTL_VECTOR_EXPORT(ELEM_TYPE, LINK_DEFINE, NAMESPACE) \
  template LINK_DEFINE class NAMESPACE ::list<ELEM_TYPE>;

/// \brief  exports the vector to a DLL. It will export,
///           template LINK_TYPE class vector<ELEM_TYPE>
///           template LINK_TYPE class set<ELEM_TYPE>
///           template LINK_TYPE class stack<ELEM_TYPE>
///           template LINK_TYPE class deque<ELEM_TYPE>
///           template LINK_TYPE class list<ELEM_TYPE>
///         and related classes...
#define NMTL_STL_EXPORT(ELEM_TYPE, LINK_DEFINE, NAMESPACE) \
  NMTL_VECTOR_EXPORT(ELEM_TYPE, LINK_DEFINE, NAMESPACE) \
  template LINK_DEFINE class NAMESPACE ::set<ELEM_TYPE>; \
  template LINK_DEFINE class NAMESPACE ::stack<ELEM_TYPE>; \
  template LINK_DEFINE class NAMESPACE ::deque<ELEM_TYPE>; \
  template LINK_DEFINE class NAMESPACE ::list<ELEM_TYPE>;

/// \brief  exports the hashmap_packed_chain to a DLL.
#define NMTL_HASHMAP_PACKED_CHAIN_EXPORT(TABLE_SIZE, DISCARD_LIMIT, KEY_TYPE, VALUE_TYPE, LINK_DEFINE, NAMESPACE) \
  template LINK_DEFINE class NAMESPACE ::hashmap_packed_chain<TABLE_SIZE, DISCARD_LIMIT, KEY_TYPE, VALUE_TYPE>;

/// \brief  exports the hashmap_dense_dynamic to a DLL.
#define NMTL_HASHMAP_DENSE_DYNAMIC_EXPORT(KEY_TYPE, VALUE_TYPE, LINK_DEFINE, NAMESPACE) \
  template LINK_DEFINE class NAMESPACE ::hashmap_dense_dynamic<KEY_TYPE, VALUE_TYPE>;

/// \brief  exports the unique_vector to a DLL.
#define NMTL_UNIQUE_VECTOR_EXPORT(ELEM_TYPE, LINK_DEFINE, NAMESPACE) \
  template LINK_DEFINE class NAMESPACE ::unique_vector<ELEM_TYPE>;

/// \brief  exports the fixed_free_list to a DLL.
#define NMTL_FIXED_FREE_LIST_EXPORT(_TYPE, LINK_DEFINE, NAMESPACE) \
  template LINK_DEFINE class NAMESPACE ::fixed_free_list<_TYPE>;

/// configure NMTL_32BIT and NMTL_64BIT
#if defined(_WIN64) || defined(__64BIT__) || defined(__64BIT) || defined(_LP64) || defined(__amd64__) || defined(__x86_64__)
  #ifdef NMTL_32BIT
    #error Forcing 32-bit compile on 64-bit platform
  #endif // NMTL_32BIT

  #ifndef NMTL_64BIT
    #define NMTL_64BIT
  #endif // NMTL_64BIT
#else
  #ifdef NMTL_64BIT
    #error Forcing 64-bit compile on 32-bit platform
  #endif // NMTL_64BIT

  #ifndef NMTL_32BIT
    #define NMTL_32BIT
  #endif // NMTL_32BIT
#endif

/// function inlining
#ifndef NMTL_INLINE
  #ifdef NMTL_DISABLE_INLINING
    #define NMTL_INLINE
  #else
    #define NMTL_INLINE inline
  #endif
#endif

/// DLL export macro
#ifndef NMTL_EXPORT
  #ifdef WIN32
    #if defined(NMTL_MT) || defined(NMTL_STATIC)
      #define NMTL_EXPORT
    #elif defined(NMTL_BUILDING_DLL)
      #define NMTL_EXPORT __declspec(dllexport)
    #else
      #define NMTL_EXPORT __declspec(dllimport)
    #endif
  #else
    #define NMTL_EXPORT
  #endif
#endif

#ifndef NMTL_STATIC_ASSERT
  #define NMTL_STATIC_ASSERT(exp) typedef char __NMTL_SA[(exp)?1:-1]
#endif // NMTL_STATIC_ASSERT

#ifndef NMTL_ALIGNOF
  // GCC supports the __alignof syntax as an alias for __alignof__
  #define NMTL_ALIGNOF(_x) __alignof(_x)
#endif // NMTL_ALIGNOF

#ifndef NMTL_RESTRICT
  #define NMTL_RESTRICT __restrict
#endif // NMTL_RESTRICT

#if defined(NMTL_HAS_MSVC_INTRINSICS)
  #define NMTL_PREFETCH(_x, _offset)    _mm_prefetch((const char*)(_x) + _offset, 0)
#else
  #define NMTL_PREFETCH(_x, _offset)
#endif // NMTL_HAS_MSVC_INTRINSICS

#if defined(NMTL_HAS_MSVC_INTRINSICS) && !defined(__INTEL_COMPILER)
  #pragma intrinsic(_BitScanForward)
  #pragma intrinsic(_BitScanReverse)
#endif // NMTL_HAS_MSVC_INTRINSICS

/// assert on an unimplemented function
#ifndef NMTL_UNIMPLEMENTED
  #define NMTL_UNIMPLEMENTED NMTL_ASSERT(0&&"unimplemented");
#endif

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMTL_NAMESPACE
#undef NMTL_NAMESPACE

#ifndef NMTL_ASSERT
  #ifdef _DEBUG
    #ifdef WIN32
      #define NMTL_ASSERT(X) _ASSERT(X)
    #else
      #include <assert.h>
      #define NMTL_ASSERT(X) assert(X)
    #endif
  #else
    #define NMTL_ASSERT(X)
  #endif
#endif

#endif // NMTL_DEFS_H
