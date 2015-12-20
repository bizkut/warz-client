//----------------------------------------------------------------------------------------------------------------------
#pragma once

// under VC++, we have to force the maths defines to be defined...
#ifdef _MSC_VER
# define _USE_MATH_DEFINES
#endif

//----------------------------------------------------------------------------------------------------------------------
// Configuration Defines 
//----------------------------------------------------------------------------------------------------------------------

/// \brief  If this value is 1, then XReal will be a float. If this is 0 then the lib 
///         will be compiled as doubles.
///
#ifndef XM2_USE_FLOAT
# define XM2_USE_FLOAT 1
#endif

/// \brief  set to 1 to utilize openMP, zero to disable openMP.
///         This only affects those functions with the suffix 'A', i.e. vec3AddVec3A()
///
#ifndef XM2_USE_OMP
# define XM2_USE_OMP 1
#endif

/// \brief  this is the value that approximates zero throughout the library. This is 
///         mainly used as the float epsilon when doing float comparisons in the lib.
///
#ifndef XM2_FLOAT_ZERO
# define XM2_FLOAT_ZERO 0.0001f
#endif

/// \brief  If set to 1, divisions by zero will assert (in debug mode) and no runtime checks 
///         will be performed. If set to 0, the divisions by zero will be checked at runtime 
///         and prevented from happening. 
///
#ifndef XM2_DIVIDE_BY_ZERO_MODE
# define XM2_DIVIDE_BY_ZERO_MODE 1
#endif

/// \brief  This lib uses the float versions of cosf, sinf, sqrtf etc. Some C library 
///         implementations do not have these funcs, so this can be set to 1 to define
///         those funcs as their double counterparts.
///
#ifndef CREATE_FLOAT_MATH_FUNCS
# define CREATE_FLOAT_MATH_FUNCS 0
#endif

/// \brief  true if XM classes should be initialised upon creation
///
#ifndef XM2_INIT_CLASSES
# define XM2_INIT_CLASSES 1
#endif

#ifndef XM2_USE_SSE
//# define XM2_USE_SSE
#endif

/// \brief  1 to force funcs to be inlined, 0 for normal inline keyword
///
#ifndef XM2_FORCE_INLINE
# define XM2_FORCE_INLINE 0
#endif

/// \brief  set to 1 to remove all C++ ostream/istream operators
///         from the maths classes
/// 
#ifndef XM2_NO_STREAM_IO
# define XM2_NO_STREAM_IO 0
#endif

/// \brief  set to 1 to remove all print, fprint, fscan, fread and fwrite methods
///         from the maths classes
/// 
#ifndef XM2_NO_C_IO
# define XM2_NO_C_IO 0
#endif

/// \brief  set to 1 to remove all C++ stream and C file IO from classes.
///
#ifndef XM2_NO_IO
# define XM2_NO_IO 0
#endif

/// \brief  set to 1 to remove all dependencies on the CRT from the library. This will 
///         force all math functions (sin/cos etc) to use fast approximations, will remove
///         all read/write methods from the classes, and requires that you provide an
///         allocator for the maths classes (i.e. the default malloc/free implementation
///         will not be used).
///
#ifndef XM2_NO_CRT
# define XM2_NO_CRT 0
#endif

// XM2_NO_CRT overrides the XM2_NO_IO flag
#if XM2_NO_CRT
# undef  XM2_NO_IO
# define XM2_NO_IO 1
#endif

// XM2_NO_IO overrides the XM2_NO_C_IO and XM2_NO_STREAM_IO flags
#if XM2_NO_IO
# undef  XM2_NO_C_IO
# undef  XM2_NO_STREAM_IO
# define XM2_NO_C_IO 1
# define XM2_NO_STREAM_IO 1
#endif


#ifdef XM2_USE_SSE
# if XM2_USE_FLOAT
#  define XM2_SSE 1
#  define XM2_SSE2 0
#  define XM2_FPU 0
# else
#  define XM2_SSE 0
#  define XM2_SSE2 1
#  define XM2_FPU 0
# endif 
#else
#  define XM2_SSE 0
#  define XM2_SSE2 0
#  define XM2_FPU 1
#endif

#include <xmmintrin.h>
#include <emmintrin.h>

#if !XM2_NO_CRT
# include <assert.h>
# include <math.h>
# if !XM2_NO_STREAM_IO
#  include <iostream>
# endif
#endif

#include "nmtl/allocator.h"


/// Disable visual C++ compile warning:
///  - warning C4201: nonstandard extension used : nameless struct/union
///
#ifdef _MSC_VER
# pragma warning(disable:4201)
#endif

namespace XM2
{
//----------------------------------------------------------------------------------------------------------------------
// MSVC++ secure std C library (SCL)
//----------------------------------------------------------------------------------------------------------------------

///
/// \brief  this define switches between the old std C functions (fscanf, fopen etc) and the
///         new secure C library (SCL) from Microsoft. If the compiler is visual C++, and it's 
///         version 2005 or newer, use the SCL. This is a slightly better way to deal with the 
///         'warning fopen deprecated use fopen_s instead' messages (as apposed to disabling the
///         warnings...) 
///
#if defined(_MSC_VER) && (_MSC_VER>=1400)
# define XM2_USE_SECURE_SCL 1
#else
# define XM2_USE_SECURE_SCL 0
#endif

//----------------------------------------------------------------------------------------------------------------------
// Core float data type 
//----------------------------------------------------------------------------------------------------------------------

typedef unsigned char  XU8;     ///< an unsigned 8bit type
typedef unsigned short XU16;    ///< an unsigned 16bit type
typedef unsigned int   XU32;    ///< an unsigned 32bit type
typedef char           XS8;     ///< a signed 8bit type
typedef short          XS16;    ///< a signed 16bit type
typedef int            XS32;    ///< a signed 32bit type
typedef float          XReal32; ///< 32 bit float
typedef double         XReal64; ///< 64 bit float

#if XM2_USE_FLOAT
  typedef float XReal;
#else
  typedef double XReal;
#endif

//----------------------------------------------------------------------------------------------------------------------
// XM2_INLINE 
//----------------------------------------------------------------------------------------------------------------------

#if XM2_FORCE_INLINE
# if defined(_MSC_VER)
#  define XM2_INLINE __forceinline
# elif defined(__INTEL_COMPILER)
#  define XM2_INLINE __forceinline
# elif defined(__GNUC__)
#  define XM2_INLINE __inline
# else
#  define XM2_INLINE inline
# endif
#else
# define XM2_INLINE inline
#endif

//----------------------------------------------------------------------------------------------------------------------
// Maths Consts
//----------------------------------------------------------------------------------------------------------------------

/// 2 * PI 
const XReal XM2_TWO_PI = (XReal)(2.0*M_PI); // 6.28318530717958647692      //360

/// PI
const XReal XM2_PI = ((XReal)(M_PI));       // 3.14159265358979323846      //180

/// PI/2
const XReal XM2_PI2 = ((XReal)(M_PI*0.5));  // 1.57079632679489661923      //90

/// PI/4
const XReal XM2_PI4 = ((XReal)(M_PI*0.25)); // 0.785398163397448309615     //45

//----------------------------------------------------------------------------------------------------------------------
// Platform Specific Defines 
//----------------------------------------------------------------------------------------------------------------------

#ifdef DOXYGEN
  #define XM2EXPORT
  #define XM2_ALIGN_SUFFIX(X)
  #define XM2_ALIGN_PREFIX(X)
  #define CREATE_FLOAT_MATH_FUNCS 0
  #undef XM2_USE_SSE
#else

  #ifdef WIN32

    ///  DLL linkage for windows 
    ///
    #ifndef XMD_STATIC
      #ifdef XMD_EXPORTS 
        #define XM2EXPORT __declspec(dllexport)
      #else
        #define XM2EXPORT __declspec(dllimport)
      #endif
    #else
      #define XM2EXPORT
    #endif

    /// VC++ 16 byte alignment macros
    ///
    #ifdef XM2_USE_SSE 
      #define XM2_ALIGN_PREFIX(sz) __declspec(align(sz))
      #define XM2_ALIGN_SUFFIX(sz)
    #else
      #define XM2_ALIGN_PREFIX(sz)
      #define XM2_ALIGN_SUFFIX(sz)
    #endif
    
  #else

    /// no linkage needed for linux
    ///
    #define XM2EXPORT 

    /// g++ 16 byte alignment macros
    ///
    #define XM2_ALIGN_PREFIX(sz)
    #define XM2_ALIGN_SUFFIX(sz) __attribute__ ((aligned(sz)))

  #endif

#endif

//----------------------------------------------------------------------------------------------------------------------
// math functions
//----------------------------------------------------------------------------------------------------------------------

#if CREATE_FLOAT_MATH_FUNCS
   #define sinf(X) ((XReal)sin(X))
   #define cosf(X) ((XReal)cos(X))
   #define tanf(X) ((XReal)tan(X))
   #define acosf(X) ((XReal)acos(X))
   #define asinf(X) ((XReal)asin(X))
   #define atan2f(X) ((XReal)atan2(X))
   #define sqrtf(X) ((XReal)sqrt(X))
   #define fabs(X) ((XReal)abs(X))
#endif

#if XM2_USE_FLOAT
  XM2_INLINE XReal XMsin(const XReal d) { return sinf(d); }
  XM2_INLINE XReal XMcos(const XReal d) { return cosf(d); }
  XM2_INLINE XReal XMtan(const XReal d) { return tanf(d); }
  XM2_INLINE XReal XMacos(const XReal d) { return acosf(d); }
  XM2_INLINE XReal XMasin(const XReal d) { return asinf(d); }
  XM2_INLINE XReal XMatan(const XReal d) { return atanf(d); }
  XM2_INLINE XReal XMatan2(const XReal x,const XReal y) { return atan2f(x,y); }
  XM2_INLINE XReal XMsqrt(const XReal d) { return sqrtf(d); }
  XM2_INLINE XReal XMabs(const XReal d) { return fabs(d); }
  XM2_INLINE XReal XMpow(const XReal x,const XReal y) { return powf(x,y); }
#else
  XM2_INLINE XReal XMsin(const XReal d) { return sin(d); }
  XM2_INLINE XReal XMcos(const XReal d) { return cos(d); }
  XM2_INLINE XReal XMtan(const XReal d) { return tan(d); }
  XM2_INLINE XReal XMacos(const XReal d) { return acos(d); }
  XM2_INLINE XReal XMasin(const XReal d) { return asin(d); }
  XM2_INLINE XReal XMatan(const XReal d) { return atan(d); }
  XM2_INLINE XReal XMatan2(const XReal x,const XReal y) { return atan2(x,y); }
  XM2_INLINE XReal XMsqrt(const XReal d) { return sqrt(d); }
  XM2_INLINE XReal XMabs(const XReal d) { return abs(d); }
  XM2_INLINE XReal XMpow(const XReal x,const XReal y) { return pow(x,y); }
#endif

  template<typename T>
  XM2_INLINE T XMmin(const T a,const T b) { return a<b ? a : b; }
  template<typename T>
  XM2_INLINE T XMmax(const T a,const T b) { return a>b ? a : b; }


#ifdef XM2_USE_SSE
  // SSE1 versions
  XM2_INLINE __m128 XMsin(const __m128 d);
  XM2_INLINE __m128 XMcos(const __m128 d);
  XM2_INLINE __m128 XMtan(const __m128 d);
  XM2_INLINE __m128 XMacos(const __m128 d);
  XM2_INLINE __m128 XMasin(const __m128 d);
  XM2_INLINE __m128 XMatan2(const __m128 x,const __m128 y);
  XM2_INLINE __m128 XMsqrt(const __m128 d);
  XM2_INLINE __m128 XMabs(const __m128 d);

  // SSE2 versions
  XM2_INLINE __m128d XMsin(const __m128d d);
  XM2_INLINE __m128d XMcos(const __m128d d);
  XM2_INLINE __m128d XMtan(const __m128d d);
  XM2_INLINE __m128d XMacos(const __m128d d);
  XM2_INLINE __m128d XMasin(const __m128d d);
  XM2_INLINE __m128d XMatan2(const __m128d x,const __m128d y);
  XM2_INLINE __m128d XMsqrt(const __m128d d);
  XM2_INLINE __m128d XMabs(const __m128d d);
#endif

//----------------------------------------------------------------------------------------------------------------------
// Asserts
//----------------------------------------------------------------------------------------------------------------------

/// \brief  assert macro - will be removed in release
///
#define XM2_ASSERT(X) assert(X);

/// \brief  memory alignment assertion. Used to ensure pointers provided to the maths
///         functions are all 16-byte aligned. will be removed in release
///
#if 0 //#ifdef XM2_USE_SSE
# define XM2_ALIGN_ASSERT(X) XM2_ASSERT( XM2Is16ByteAligned(X) && "[ERROR] data must be 16 byte aligned!" )
#else
# define XM2_ALIGN_ASSERT(X)
#endif

/// \defgroup CoreTypes Core XM2 math classes
/// \brief  the core math classes. 

/// \defgroup CoreArrayTypes Core XM2 math array classes
/// \brief  the core math classes. 

/// \defgroup CompoundTypes XM2 compound classes
/// \brief  maths classes built using core types

/// \defgroup STL


/// \defgroup CommonFunctions Utilities
///

//----------------------------------------------------------------------------------------------------------------------
// Utility Functions
//----------------------------------------------------------------------------------------------------------------------

/// \brief  utility function to check whether the provided pointer is actually 16 byte 
///         aligned. 
/// \param  ptr - the pointer to perform an alignment check on
/// \return 1 if aligned correctly, 0 otherwise
///
XM2EXPORT bool XM2Is16ByteAligned(const void* ptr);

/// \brief  swaps the bytes in f
/// \param  f - the float to swap
/// 
XM2_INLINE void XM2swap(float& f)
{
  unsigned char* ptr = (unsigned char*)((void*)&f);
  unsigned char temp = ptr[0];
  ptr[0]=ptr[3];
  ptr[3]=temp;
  temp = ptr[1];
  ptr[1]=ptr[2];
  ptr[2]=temp;
}

/// \brief  swaps the bytes in f
/// \param  f - the double to swap
/// 
XM2_INLINE void XM2swap(double& f)
{
  unsigned char* ptr = (unsigned char*)((void*)&f);
  unsigned char temp = ptr[0];
  ptr[0]=ptr[7];
  ptr[7]=temp;
  temp = ptr[1];
  ptr[1]=ptr[6];
  ptr[6]=temp;
  temp = ptr[2];
  ptr[2]=ptr[5];
  ptr[5]=temp;
  temp = ptr[3];
  ptr[3]=ptr[4];
  ptr[4]=temp;
}
XM2_INLINE void XM2swap(__m128& f)
{
  /// \todo replace with SSE code
  float* _f = (float*)((void*)&f);
  XM2swap(_f[0]);
  XM2swap(_f[1]);
  XM2swap(_f[2]);
  XM2swap(_f[3]);
}
XM2_INLINE void XM2swap(__m128d& f)
{
  /// \todo replace with SSE code
  double* _f = (double*)((void*)&f);
  XM2swap(_f[0]);
  XM2swap(_f[1]);
}
XM2EXPORT void XM2swap(__m128* data,unsigned num);
XM2EXPORT void XM2swap(__m128d* data,unsigned num);
XM2EXPORT void XM2swap32(void* data,unsigned num);
XM2EXPORT void XM2swap64(void* data,unsigned num);

//----------------------------------------------------------------------------------------------------------------------
// Memory Allocation Functions
//----------------------------------------------------------------------------------------------------------------------

/// \brief  generates new and delete operators for C++ classes
///
#define  XM2_MEMORY_OPERATORS \
    XM2_INLINE void* operator new(size_t s)     \
    {                                       \
      return nmtl::NMTLAlloc((size_t)s);         \
    }                                       \
    XM2_INLINE void* operator new[](size_t s)   \
    {                                       \
      return nmtl::NMTLAlloc((size_t)s);         \
    }                                       \
    XM2_INLINE void operator delete(void* ptr)  \
    {                                       \
      nmtl::NMTLFree(ptr);                         \
    }                                       \
    XM2_INLINE void operator delete[](void* ptr)\
    {                                       \
      nmtl::NMTLFree(ptr);                         \
    }

#define XM2_SSE2_INIT(X,Y) {{ X, Y }}
#define XM2_SSE2_INIT1(X) XM2_SSE2_INIT(X,X)

#define XM2_SSE_INIT(X,Y,Z,W) {{ X, Y, Z, W }}
#define XM2_SSE_INIT1(X) XM2_SSE_INIT(X,X,X,X)
//----------------------------------------------------------------------------------------------------------------------
// Degree / Radian conversion
//----------------------------------------------------------------------------------------------------------------------

/// converts from degrees to radians
///
XM2_INLINE XReal degToRad(const XReal d)
{
  return (XReal)((XM2_PI/180.0f)*d);
}

/// converts from radians to degrees
///
XM2_INLINE XReal radToDeg(const XReal r)
{
  return (XReal)((180.0f/XM2_PI)*r);
}

/// converts from degrees to radians
///
XM2_INLINE __m128 degToRad(const __m128 d)
{
  return _mm_mul_ps(_mm_set1_ps((float)(XM2_PI/180.0f)),d);
}

/// converts from radians to degrees
///
XM2_INLINE __m128 radToDeg(const __m128 r)
{
  return _mm_mul_ps(_mm_set1_ps(180.0f/(float)XM2_PI),r);
}

/// converts from degrees to radians
///
XM2_INLINE __m128d degToRad(const __m128d d)
{
  return _mm_mul_pd(_mm_set1_pd((double)XM2_PI/180.0f),d);
}

/// converts from radians to degrees
///
XM2_INLINE __m128d radToDeg(const __m128d r)
{
  return _mm_mul_pd(_mm_set1_pd(180.0f/(double)XM2_PI),r);
}


//----------------------------------------------------------------------------------------------------------------------
// float comparison
//----------------------------------------------------------------------------------------------------------------------

/// \brief  floating point comparison
///
XM2_INLINE bool float_equal(const XReal a, const XReal b, const XReal eps=XM2_FLOAT_ZERO)
{
  return (XMabs(a-b)<eps);
}

/// \brief  floating point comparison
///
XM2_INLINE bool double_equal(const double a, const double b, const double eps=XM2_FLOAT_ZERO)
{
  return (XMabs( (XReal)(a-b) )<eps);
}
}
//----------------------------------------------------------------------------------------------------------------------
// SSE sin/cos implementations
#ifdef XM2_USE_SSE
# include "XM/inl/sse/Trigonometry.inl"
# include "XM/inl/sse2/Trigonometry.inl"
#endif
//----------------------------------------------------------------------------------------------------------------------
