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
#ifndef NMTL_TYPE_UTILS_H
#define NMTL_TYPE_UTILS_H

//----------------------------------------------------------------------------------------------------------------------
#include <functional>
#include <algorithm>
#include <stddef.h>
#ifdef WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif
#ifndef NMTL_IOS
  #include <xmmintrin.h>
  #include <emmintrin.h>
#endif
#include "nmtl/allocator.h"

//----------------------------------------------------------------------------------------------------------------------
namespace nmtl
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  determines if S is the same as T
template<typename S, typename T>
struct are_the_same
{
private:
  typedef char A;
  struct B { char dummy[2]; };
  template<typename U>
  static A are_same(const U&);
  static B are_same(const S&);
  static T MakeT();
public:
  enum { result = (sizeof(B) == sizeof(are_same(MakeT()))) };
};

#pragma warning(push)
#pragma warning(disable:4800)
#pragma warning(disable:4244)

//----------------------------------------------------------------------------------------------------------------------
/// \brief  determines if S can be converted to T
template<typename T, typename S>
struct can_convert_to
{
private:
  typedef char A;
  struct B { char dummy[2]; };
  static A can_convert(const S&);
  static B can_convert(...);
  static T& MakeT();
public:
  enum { result = (sizeof(A) == sizeof(can_convert(MakeT()))) };
};
#pragma warning(pop)

//----------------------------------------------------------------------------------------------------------------------
/// \brief  determines if T is a signed integer
template<typename T>
struct is_signed_integer
{
  enum
  {
    result = are_the_same<T, bool>::result ||
             are_the_same<T, char>::result ||
             are_the_same<T, wchar_t>::result ||
             are_the_same<T, short>::result ||
             are_the_same<T, int>::result ||
             are_the_same<T, long>::result ||
             are_the_same<T, long long>::result
#ifdef WIN32
             || are_the_same<T, LARGE_INTEGER>::result
#endif
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  determines if T is an unsigned integer
template<typename T>
struct is_unsigned_integer
{
  enum
  {
    result = are_the_same<T, unsigned char>::result ||
             are_the_same<T, unsigned short>::result ||
             are_the_same<T, unsigned int>::result ||
             are_the_same<T, unsigned long>::result ||
             are_the_same<T, unsigned long long>::result
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  determines if T is a signed or unsigned integer
template<typename T>
struct is_integer
{
  enum
  {
    result = is_signed_integer<T>::result ||
             is_unsigned_integer<T>::result
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  determines if T is a pointer
template<typename T>
struct is_pointer
{
  enum
  {
    result = can_convert_to<T, void*>::result ||
             can_convert_to<T, void const*>::result ||
             can_convert_to<T, void volatile*>::result ||
             can_convert_to<T, void const volatile*>::result
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  determines if T is a float or double
template<typename T>
struct is_float
{
  enum
  {
    result = are_the_same<T, float>::result ||
             are_the_same<T, double>::result
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  determines if T is an SSE intrinsic type
#ifdef NMTL_IOS
template<typename T>
struct is_sse
{
  enum
  {
    result = false
  };
};
#else
template<typename T>
struct is_sse
{
  enum
  {
    result = are_the_same<T, __m128>::result ||
             are_the_same<T, __m128d>::result ||
             are_the_same<T, __m128i>::result
  };
};
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \brief  determines if T is an enum.
/// \note   this may fail if T is a class that overloads the cast to int operator,
///         or if a global assignment operator has been defined to convert the enum
///         to an integer
template<typename T>
struct is_enum
{
public:
  enum
  {
    result = can_convert_to<T, int>::result &&
             !can_convert_to<int, T>::result
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  determines if T is an enum.
/// \note   this may fail if T is a class that overloads the cast to int operator,
///         or if a global assignment operator has been defined to convert the enum
///         to an integer
template<typename T>
struct is_function
{
public:
  enum
  {
    result = 0
  };
};
template<bool, typename T>
struct _is_class
{
  enum { result = false };
};
template<typename T>
struct _is_class<false, T>
{
  enum
  {
    result = !(is_integer<T>::result ||
               is_float<T>::result ||
               is_pointer<T>::result ||
               is_enum<T>::result ||
               is_function<T>::result)
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  determines if T is an enum.
/// \note   this may fail if T is a class that overloads the cast to int operator,
///         or if a global assignment operator has been defined to convert the enum
///         to an integer
template<typename T>
struct is_class
{
public:
  enum
  {
    result = _is_class< is_sse<T>::result, T >::result
  };
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace nmtl

#endif // NMTL_TYPE_UTILS_H
