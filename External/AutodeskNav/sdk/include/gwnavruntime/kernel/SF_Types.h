/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

PublicHeader:   Kernel
Filename    :   KY_Types.h
Content     :   Standard library defines and simple types
Created     :   June 28, 1998
Authors     :   Michael Antonov, Brendan Iribe, Maxim Shemanarev

Notes       :   This file represents all the standard types
                to be used. These types are defined for compiler
                independence from size of types.

**************************************************************************/

#ifndef INC_KY_Kernel_Types_H
#define INC_KY_Kernel_Types_H


// ****** Operating System
//
//---------------------------------------------------------------------------
// Scaleform works on the following operating systems: (KY_OS_x)
//
//    WIN32    - Win32 (Windows 95/98/ME and Windows NT/2000/XP)
//    DARWIN   - Darwin OS (Mac OS X)
//    LINUX    - Linux
//    ANDROID  - Android
//    IPHONE   - iPhone
//    XBOX360  - Xbox 360 console
//    PS3      - Playstation 3 console
//    PSVITA   - PSVITA handheld console
//    WII      - Wii console
//    3DS      - 3DS handheld console

#if (defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))) || defined(__MACOS__)
# if (defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || defined(__IPHONE_OS_VERSION_MIN_REQUIRED))
#  define KY_OS_IPHONE
#  define KY_OS_STRING         "iPhone"
# else
#  define KY_OS_DARWIN
#  define KY_OS_MAC
#  define KY_OS_STRING         "Mac"
# endif
#elif defined(_XBOX)
# include <xtl.h>
// Xbox360 and XBox both share _XBOX definition
# if (_XBOX_VER >= 200)
#  define KY_OS_XBOX360
#  define KY_OS_STRING         "Xbox360"
# endif
#elif (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
# define KY_OS_WIN32
# define KY_OS_STRING         "Win64"
#elif (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
# define KY_OS_WIN32
# define KY_OS_STRING         "Win32"
#elif defined(__MWERKS__) && defined(__INTEL__)
# define KY_OS_WIN32
#elif defined(__linux__) || defined(__linux)
# define KY_OS_LINUX
# define KY_OS_STRING         "Linux"
#elif defined(__PPU__)
# define KY_OS_PS3
# define KY_OS_STRING         "PS3PPU"
#elif defined(__ORBIS__)
# define KY_OS_ORBIS
# define KY_OS_STRING         "PS4"
#elif defined(RVL)
# define KY_OS_WII
# define KY_OS_STRING         "Wii"
#elif defined(__psp2__)
# define KY_OS_PSVITA
# define KY_OS_STRING         "Vita"
#elif defined(NN_PLATFORM_CTR)
# define KY_OS_3DS
# define KY_OS_STRING         "3DS"
#elif defined(CAFE)
# define KY_OS_WIIU
# define KY_OS_STRING         "WiiU"
#else
# define KY_OS_OTHER
# define KY_OS_STRING         "UnknownPlatform"
#endif

// NOTE that KY_OS_WIN32 remains defined!
// KY_TODO change all this so that KY_OS_WIN32 is undefined and each type has its own define: KY_OS_DURANGO and KY_OS_WINSTORE
#if defined(KY_OS_WIN32)
# if defined(_DURANGO)
#  define KY_OS_WINMETRO
#  undef KY_OS_STRING
#  define KY_OS_STRING         "XboxOne"
#elif defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP)
#  define KY_OS_WINMETRO
#  undef KY_OS_STRING
#  define KY_OS_STRING         "WindowsPhone"
# elif defined(_M_ARM_FP)
#  define KY_OS_WINMETRO
#  undef KY_OS_STRING
#  define KY_OS_STRING         "WinStore"
# endif
#endif

// NOTE that KY_OS_LINUX remains defined!
// KY_TODO change all this so that KY_OS_LINUX is undefined and only KY_OS_ANDROID remains define
#if defined(KY_OS_LINUX)
# if defined(ANDROID)
#  define KY_OS_ANDROID
#  undef KY_OS_STRING
#  define KY_OS_STRING         "Android"
# endif
#endif

// ***** CPU Architecture
//
//---------------------------------------------------------------------------
// Scaleform supports the following CPUs: (KY_CPU_x)
//
//    X86        - x86 (IA-32)
//    X86_64     - x86_64 (amd64)
//    PPC        - PowerPC
//    PPC64      - PowerPC64
//    MIPS       - MIPS
//    ARM        - ARM based CPU
//    OTHER      - CPU for which no special support is present or needed


#if defined(__x86_64__) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#  define KY_CPU_X86_64
#  define KY_64BIT_POINTERS
#elif defined(__arm__) || defined(_M_ARM_FP)
#  define KY_CPU_ARM
#elif defined(__i386__) || defined(KY_OS_WIN32) || defined(KY_OS_XBOX)
#  define KY_CPU_X86
#elif defined(__powerpc64__) || defined(KY_OS_PS3) || defined(KY_OS_XBOX360) || defined(KY_OS_WII) || defined(KY_OS_WIIU)
#  define KY_CPU_PPC64
// Note: PS3, x360 and WII don't use 64-bit pointers.
#elif defined(__ppc__)
#  define KY_CPU_PPC
#elif defined(__mips__) || defined(__MIPSEL__)
#  define KY_CPU_MIPS
#else
#  define KY_CPU_OTHER
#endif

// ***** Coprocessor Architecture
//
//---------------------------------------------------------------------------
// Scaleform supports the following CPUs: (KY_CPU_x)
//
//    SSE        - Available on all modern x86 processors.
//    Altivec    - Available on all modern ppc processors (including PS3).
//                 X360 uses non-standard headers/defines, but does implement
//                 an extended Altivec feature set.
//    Neon       - Available on some armv7+ processors.

#if defined(__SSE__) || (defined(KY_OS_WIN32) && !defined(_M_ARM_FP))
#  define  KY_CPU_SSE
#endif // __SSE__

#if defined( __ALTIVEC__ ) && !defined(KY_OS_WII)
#  define KY_CPU_ALTIVEC
#endif // __ALTIVEC__

#if defined(__ARM_NEON__)
#   define KY_CPU_ARM_NEON
#endif // __ARM_NEON__


// ***** Include Config.h
//
//---------------------------------------------------------------------------
#ifndef KY_CONFIG_H
#define KY_CONFIG_H "SF_Config.h"
#endif

// Include conditional compilation options file
// Needs to be after OS detection code
#include KY_CONFIG_H


// ***** Compiler
//
//---------------------------------------------------------------------------
//  Scaleform is compatible with the following compilers: (KY_CC_x)
//
//     MSVC     - Microsoft Visual C/C++
//     INTEL    - Intel C++ for Linux / Windows
//     GNU      - GNU C++
//     SNC      - SN Systems ProDG C/C++ (EDG)
//     MWERKS   - Metrowerks CodeWarrior
//     BORLAND  - Borland C++ / C++ Builder
//     ARM      - ARM C/C++
//     LLVM-GCC - Low-Level Virtual Machine plugin for GCC.

#if defined(__INTEL_COMPILER)
// Intel 4.0                    = 400
// Intel 5.0                    = 500
// Intel 6.0                    = 600
// Intel 8.0                    = 800
// Intel 9.0                    = 900
# define KY_CC_INTEL       __INTEL_COMPILER

#elif defined(_MSC_VER)
// MSVC 5.0                     = 1100
// MSVC 6.0                     = 1200
// MSVC 7.0 (VC2002)            = 1300
// MSVC 7.1 (VC2003)            = 1310
// MSVC 8.0 (VC2005)            = 1400
// MSVC 9.0 (VC2008)            = 1500
// MSVC 10.0 (VC2010)           = 1600
// MSVC 11.0 (VC2012)           = 1700
# define KY_CC_MSVC        _MSC_VER

#elif defined(__MWERKS__)
// Metrowerks C/C++ 2.0         = 0x2000
// Metrowerks C/C++ 2.2         = 0x2200
# define KY_CC_MWERKS      __MWERKS__

#elif defined(__BORLANDC__) || defined(__TURBOC__)
// Borland C++ 5.0              = 0x500
// Borland C++ 5.02             = 0x520
// C++ Builder 1.0              = 0x520
// C++ Builder 3.0              = 0x530
// C++ Builder 4.0              = 0x540
// Borland C++ 5.5              = 0x550
// C++ Builder 5.0              = 0x550
// Borland C++ 5.51             = 0x551
# define KY_CC_BORLAND     __BORLANDC__

#elif defined(__ghs__)
# define KY_CC_GHS

#elif defined(__clang__)
# define KY_CC_CLANG

// SNC must come before GNUC because
// the SNC compiler defines GNUC as well
#elif defined(__SNC__)
# define KY_CC_SNC

#elif defined(__GNUC__)
# define KY_CC_GNU
# if defined(__llvm__) && __llvm__
#   define KY_CC_LLVMGCC
# endif

#elif defined(__CC_ARM)
# define KY_CC_ARM

#else
# error "SF does not support this Compiler - contact support@scaleform.com"
#endif


// Ignore compiler warnings specific to SN compiler
#if defined(KY_CC_SNC)
# pragma diag_suppress=68   // integer conversion resulted in a change of sign
# pragma diag_suppress=112  // statement is unreachable
// (possibly due to SN disliking modifying a mutable object in a const function)
# pragma diag_suppress=175  // expression has no effect
// (deliberately placed so it would NOT produce a warning...)
# pragma diag_suppress=178  // variable was declared but never referenced
# pragma diag_suppress=382  // extra ";" ignored
# pragma diag_suppress=552  // variable was set but never used
# pragma diag_suppress=613  // overloaded virtual function is only partially overridden in class
# pragma diag_suppress=999  // function funcA is hidden by funcB -- virtual function override intended?

# pragma diag_suppress=1011 // function funcA is hidden by funcB -- virtual function override intended?
# pragma diag_suppress=1421 // trigraphs (in comments)
# pragma diag_suppress=1787 // invalid value for enumeration type
# ifdef KY_OS_PSVITA
#  pragma diag_suppress=1783 // cast increases required alignment
# endif
#endif


// *** Linux Unicode - must come before Standard Includes

#ifdef KY_OS_LINUX
/* Use glibc unicode functions on linux. */
#ifndef  _GNU_SOURCE
# define _GNU_SOURCE
#endif
#endif

// *** Symbian - fix defective header files

#ifdef KY_OS_SYMBIAN
#include <_ansi.h>
#undef _STRICT_ANSI
#include <stdlib.h>
#endif

#ifdef KY_OS_WII
#define Ptr PtrXXX
#include <revolution/types.h>
#undef Ptr
#include <types.h>
#endif

#ifdef KY_OS_3DS
#include <nn/types.h>
#endif

#ifdef KY_OS_WIIU
#include <cafe/os.h>
#endif

// ***** Standard Includes
//
//---------------------------------------------------------------------------
#include    <stddef.h>
#include    <limits.h>
#include    <float.h>

// This macro needs to be defined if it is necessary to avoid the use of Double.
// In that case Double in defined as float and thus extra #ifdef checks on
// overloads need to be done. Useful for platforms with poor/unavailable
// Double support

//# define KY_NO_DOUBLE


// *****  Definitions
//
//---------------------------------------------------------------------------
// Byte order
#define KY_LITTLE_ENDIAN       1
#define KY_BIG_ENDIAN          2


namespace Kaim {

// Compiler specific integer
//typedef int                 int ;
//typedef unsigned int        unsigned;

// Pointer-size integer
typedef size_t              UPInt;
typedef ptrdiff_t           SPInt;

} // namespace Kaim {

// **** Win32 or XBox
//-----------------------
#if defined(KY_OS_WIN32) || defined(KY_OS_XBOX360)

// ** Miscellaneous compiler definitions

// DLL linking options
//-----------------------
#ifdef KY_BUILD_STATICLIB
// BUILD_STATICLIB means no DLL support, so disable symbol exports
# define    KY_IMPORT
# define    KY_EXPORT
#else
# define    KY_IMPORT   __declspec(dllimport)
# define    KY_EXPORT   __declspec(dllexport)
#endif // KY_BUILD_STATICLIB

#define KY_EXPORTC      extern "C" KY_EXPORT

// Build is used for importing (must be defined to nothing)
#define KY_BUILD

// Calling convention - goes after function return type but before function name
#ifdef __cplusplus_cli
    #define KY_FASTCALL         __stdcall
#else
    #define KY_FASTCALL         __fastcall
#endif

#define KY_STDCALL              __stdcall
#define KY_CDECL                __cdecl

// Byte order
#if defined(KY_OS_XBOX360)
#define KY_BYTE_ORDER          KY_BIG_ENDIAN
#else
#define KY_BYTE_ORDER          KY_LITTLE_ENDIAN
#endif

#if defined(KY_CC_MSVC)
// Disable "inconsistent dll linkage" warning
# pragma warning(disable : 4127)
// Disable "exception handling" warning
# pragma warning(disable : 4530)
# if (KY_CC_MSVC<1300)
// Disable "unreferenced inline function has been removed" warning
# pragma warning(disable : 4514)
// Disable "function not inlined" warning
# pragma warning(disable : 4710)
// Disable "_force_inline not inlined" warning
# pragma warning(disable : 4714)
// Disable "debug variable name longer than 255 chars" warning
# pragma warning(disable : 4786)
# endif // (KY_CC_MSVC<1300)
#endif // (KY_CC_MSVC)

// Assembly macros
#if defined(KY_CC_MSVC)
#define KY_ASM                _asm
#else
#define KY_ASM                asm
#endif // (KY_CC_MSVC)

// Inline substitute - goes before function declaration
#if defined(KY_CC_MSVC)
# define KY_INLINE          __forceinline
# define KY_NOINLINE        __declspec(noinline)
#elif defined(KY_CC_GNU)
# define KY_INLINE          __attribute__((always_inline))
# define KY_NOINLINE        __attribute__((noinline))
#else
# define KY_INLINE          inline
# define KY_NOINLINE
#endif  // KY_CC_MSVC


#ifdef UNICODE
#define KY_STR(str) L##str
#else
#define KY_STR(str) str
#endif // UNICODE

#else 

#if (defined(BYTE_ORDER) && (BYTE_ORDER == BIG_ENDIAN))|| \
    (defined(_BYTE_ORDER) && (_BYTE_ORDER == _BIG_ENDIAN))
#define KY_BYTE_ORDER          KY_BIG_ENDIAN
#elif (defined(KY_OS_PS3) || defined(__ARMEB__) || defined(KY_CPU_PPC) || defined(KY_CPU_PPC64))
#define KY_BYTE_ORDER          KY_BIG_ENDIAN
#elif defined (KY_OS_WII) || defined(KY_OS_WIIU)
#define KY_BYTE_ORDER          KY_BIG_ENDIAN

#else
#define KY_BYTE_ORDER          KY_LITTLE_ENDIAN

#endif

// **** Standard systems

// ** Miscellaneous compiler definitions

// DLL linking options
#define KY_IMPORT
#define KY_EXPORT
#define KY_EXPORTC              extern "C" KY_EXPORT

// Build is used for importing, defined to nothing
#define KY_BUILD

// Assembly macros
#define KY_ASM                  __asm__
#define KY_ASM_PROC(procname)   KY_ASM
#define KY_ASM_END              KY_ASM

// Inline substitute - goes before function declaration
#define KY_INLINE               inline
#define KY_NOINLINE

// Calling convention - goes after function return type but before function name
#define KY_FASTCALL
#define KY_STDCALL
#define KY_CDECL

#endif // defined(KY_OS_WIN32) || defined(KY_OS_XBOX) || defined(KY_OS_XBOX360) || defined(KY_OS_WINCE)

// ***** Type definitions for Win32
//---------------------------------------------------------------------------
namespace Kaim {

typedef char                Char;

// *** Type definitions for common systems

typedef char                Char;

#if defined(KY_OS_WIN32) || defined(KY_OS_XBOX360)
// 8 bit Integer (Byte)
typedef char                SInt8;
typedef unsigned char       UInt8;
typedef SInt8               SByte;
typedef UInt8               UByte;

// 16 bit Integer (Word)
typedef short               SInt16;
typedef unsigned short      UInt16;

// 32 bit Integer (DWord)
// #if defined(KY_OS_LINUX) && defined(KY_CPU_X86_64)
// typedef int                 SInt32;
// typedef unsigned int        UInt32;
// #else
// typedef long                SInt32;
// typedef unsigned long       UInt32;
// #endif

typedef long                SInt32;
typedef unsigned long       UInt32;

// 64 bit Integer (QWord)
typedef __int64             SInt64;
typedef unsigned __int64    UInt64;

// Floating point
//typedef float               float;

#ifdef KY_NO_DOUBLE
typedef float               Double;
#else
typedef double              Double;
#endif

#elif defined(KY_OS_MAC) || defined(KY_OS_IPHONE)

typedef int          SInt8  __attribute__((__mode__ (__QI__)));
typedef unsigned int UInt8  __attribute__((__mode__ (__QI__)));
typedef int          SByte  __attribute__((__mode__ (__QI__)));
typedef unsigned int UByte  __attribute__((__mode__ (__QI__)));

typedef int          SInt16 __attribute__((__mode__ (__HI__)));
typedef unsigned int UInt16 __attribute__((__mode__ (__HI__)));

typedef int          SInt32 __attribute__((__mode__ (__SI__)));
typedef unsigned int UInt32 __attribute__((__mode__ (__SI__)));

typedef int          SInt64 __attribute__((__mode__ (__DI__)));
typedef unsigned int UInt64 __attribute__((__mode__ (__DI__)));

#elif defined(KY_CC_GNU) || defined(KY_CC_SNC) || defined(KY_OS_ORBIS)

typedef int          SInt8  __attribute__((__mode__ (__QI__)));
typedef unsigned int UInt8  __attribute__((__mode__ (__QI__)));
typedef int          SByte  __attribute__((__mode__ (__QI__)));
typedef unsigned int UByte  __attribute__((__mode__ (__QI__)));

typedef int          SInt16 __attribute__((__mode__ (__HI__)));
typedef unsigned int UInt16 __attribute__((__mode__ (__HI__)));

typedef int          SInt32 __attribute__((__mode__ (__SI__)));
typedef unsigned int UInt32 __attribute__((__mode__ (__SI__)));

typedef int          SInt64 __attribute__((__mode__ (__DI__)));
typedef unsigned int UInt64 __attribute__((__mode__ (__DI__)));


#elif defined(KY_OS_WII) || defined(KY_OS_3DS) || defined(KY_OS_WIIU)

// 8 bit Integer (Byte)
typedef s8              SInt8;
typedef u8              UInt8;
typedef s8              SByte;
typedef u8              UByte;

// 16 bit Integer
typedef s16             SInt16;
typedef u16             UInt16;

// 32 bit Integer
typedef s32             SInt32;
typedef u32             UInt32;

// 64 bit Integer
typedef s64             SInt64;
typedef u64             UInt64;

#else

} // Kaim
#include <sys/types.h>
namespace Kaim {

// 8 bit Integer (Byte)
typedef int8_t              SInt8;
typedef uint8_t             UInt8;
typedef int8_t              SByte;
typedef uint8_t             UByte;

// 16 bit Integer
typedef int16_t             SInt16;
typedef uint16_t            UInt16;

// 32 bit Integer
typedef int32_t             SInt32;
typedef uint32_t            UInt32;

// 64 bit Integer
typedef int64_t             SInt64;
typedef uint64_t            UInt64;

#endif

// Floating point
//typedef float               float;

#ifdef KY_NO_DOUBLE
typedef float               Double;
#else
typedef double              Double;
#endif // KY_NO_DOUBLE

//
// ***** BaseTypes Namespace

// BaseTypes namespace is explicitly declared to allow base types to be used
// by customers directly without other contents of Scaleform namespace.
//
// Its is expected that GFx samples will declare 'using namespace Kaim::BaseTypes'
// to allow using these directly without polluting the target scope with other
// GFx/Scaleform declarations, such as Ptr<>, String or Mutex.
namespace BaseTypes
{
    //using Kaim::unsigned;
    //using Kaim::int ;
    using Kaim::UPInt;
    using Kaim::SPInt;
    using Kaim::UByte;
    using Kaim::SByte;
    using Kaim::UInt16;
    using Kaim::SInt16;
    using Kaim::UInt32;
    using Kaim::SInt32;
    using Kaim::UInt64;
    using Kaim::SInt64;
    //using Kaim::float;
    using Kaim::Double;
} // BaseTypes

} // Scaleform


// ***** Compiler Assert
//
//---------------------------------------------------------------------------
// Compile-time assert.  Thanks to Jon Jagger (http://www.jaggersoft.com) for this trick.
#define KY_COMPILER_ASSERT(x)   { int assertVar = 0; switch(assertVar){case 0: case x:;} }
// use a second parameter to build a function name in order to enable compiler assert out of functions
#define KY_COMPILER_ASSERTG(x, f)   void KaimDummyFunction_##f()KY_COMPILER_ASSERT(x)

// Compile-time message
#ifndef KY_COMPILE_TIME_MESSAGE
# define KY__QUOTE(x) #x
# define KY_QUOTE(x) KY__QUOTE(x)
# define KY___FILE_LINE__ __FILE__ "(" KY_QUOTE(__LINE__) ") : "
# if !defined(KY_CC_GNU) && !defined(KY_CC_SNC) && !defined(KY_CC_MWERKS) && !defined(KY_CC_CLANG) && !defined(KY_CC_GHS)
#  define KY_PRAGMA __pragma
# elif defined(KY_CC_SNC) || defined (KY_CC_MWERKS) || (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__ > 40400) // gcc version > 4.4.0 required to support pragma message
#  define KY_PRAGMA(x) _Pragma(#x)
# else
#  define KY_PRAGMA(x)
# endif
# define KY_COMPILE_TIME_MESSAGE( x ) KY_PRAGMA(message( KY___FILE_LINE__#x))
# define KY_TEMPORARY_CODE KY_COMPILE_TIME_MESSAGE("[KY_TEMPORARY_CODE] ########### !! TEMPORARY CODE TO BE REMOVED !! ###########")
//# define KY_TODO_ENABLED
# ifdef KY_TODO_ENABLED
#  define KY_TODO( x ) KY_COMPILE_TIME_MESSAGE("[KY_TODO]:" x)
# else
#  define KY_TODO( x ) 
# endif // KY_TODO_ENABLED
#endif //KY_COMPILE_TIME_MESSAGE

// Handy macro to quiet compiler warnings about unused parameters/variables.
#if defined(KY_CC_GNU) || defined(KY_CC_CLANG) || defined (KY_CC_GHS)
#define     KY_UNUSED(a)          do {__typeof__ (&a) __attribute__ ((unused)) __tmp = &a; } while(0)
#define     KY_UNUSED2(a,b)       KY_UNUSED(a); KY_UNUSED(b)
#define     KY_UNUSED3(a,b,c)     KY_UNUSED2(a,c); KY_UNUSED(b)
#define     KY_UNUSED4(a,b,c,d)   KY_UNUSED3(a,c,d); KY_UNUSED(b)
#else
#define     KY_UNUSED(a)          (a)
#define     KY_UNUSED2(a,b)       (a);(b)
#define     KY_UNUSED3(a,b,c)     (a);(b);(c)
#define     KY_UNUSED4(a,b,c,d)   (a);(b);(c);(d)
#endif

#define     KY_UNUSED1(a) KY_UNUSED(a)
#define     KY_UNUSED5(a1,a2,a3,a4,a5) KY_UNUSED4(a1,a2,a3,a4); KY_UNUSED(a5)
#define     KY_UNUSED6(a1,a2,a3,a4,a5,a6) KY_UNUSED4(a1,a2,a3,a4); KY_UNUSED2(a5,a6)
#define     KY_UNUSED7(a1,a2,a3,a4,a5,a6,a7) KY_UNUSED4(a1,a2,a3,a4); KY_UNUSED3(a5,a6,a7)
#define     KY_UNUSED8(a1,a2,a3,a4,a5,a6,a7,a8) KY_UNUSED4(a1,a2,a3,a4); KY_UNUSED4(a5,a6,a7,a8)
#define     KY_UNUSED9(a1,a2,a3,a4,a5,a6,a7,a8,a9) KY_UNUSED4(a1,a2,a3,a4); KY_UNUSED5(a5,a6,a7,a8,a9)


// ***** KY_DEBUG_BREAK, KY_ASSERT
//
// Change from vanilla Kernel: added KY_RELEASE_BREAK KY_RELEASE_ASSERT(p) that are defined in release too
// Microsoft Win32 specific debugging support
//#if (defined(KY_OS_WIN32) && !defined(__cplusplus_cli)) || defined(KY_OS_XBOX)
#if defined(KY_OS_WIN32)
#ifdef KY_CPU_X86
#if defined(__cplusplus_cli)
#define KY_RELEASE_BREAK do { __debugbreak(); } while(0)
#elif defined(KY_CC_GNU)
#define KY_RELEASE_BREAK do { KY_ASM("int $3\n\t"); } while(0)
#else
#define KY_RELEASE_BREAK do { KY_ASM int 3 } while (0)
#endif
#else
#define KY_RELEASE_BREAK     do { __debugbreak(); } while(0)
#endif
// Xbox360 specific debugging
#elif defined(KY_OS_XBOX360)    
#define KY_RELEASE_BREAK         DebugBreak()
// Special assert for X360 static code analysis.
#if defined(_PREFAST_)
#define SF_ASSERT(p)       do { BOOL res = (BOOL)((p) != 0); __analysis_assume(res); if (!(res))  { SF_DEBUG_BREAK; } } while(0)
#endif
#elif defined(KY_OS_ORBIS)
#include <libdbg.h>
#define KY_RELEASE_BREAK     do { SCE_BREAK(); } while(0)
#elif defined(KY_CC_CLANG)
#define KY_RELEASE_BREAK     do { __builtin_trap(); } while(0)
#elif defined(KY_CPU_X86)
#define KY_RELEASE_BREAK     do { KY_ASM("int $3\n\t"); } while(0)
#elif defined(KY_OS_3DS)
#include <nn/dbg.h>
#define KY_RELEASE_BREAK     do { nn::dbg::Break(nn::dbg::BREAK_REASON_USER); } while(0)
#elif defined(KY_OS_WIIU)
#define KY_RELEASE_BREAK     do { OSDebug(); } while(0)
#else
#define KY_RELEASE_BREAK     do { *((int *) 0) = 1; } while(0)
#endif

// This will cause compiler breakpoint
#ifndef KY_RELEASE_ASSERT
#define KY_RELEASE_ASSERT(p)                             \
	KY_TEMPORARY_CODE                                    \
	do { if (!(p))  { KY_RELEASE_BREAK; } } while(0)
#endif


//---------------------------------------------------------------------------
// If not in debug build, macros do nothing
#ifndef KY_BUILD_DEBUG

#define KY_DEBUG_BREAK  ((void)0)
#define KY_ASSERT(p)    ((void)0)

#else 

#define KY_DEBUG_BREAK  KY_RELEASE_BREAK
#define KY_ASSERT(p)    do { if (!(p))  { KY_RELEASE_BREAK; } } while(0)

#endif // KY_BUILD_DEBUG


// ******** Variable range definitions
//
//---------------------------------------------------------------------------
#if defined(KY_CC_MSVC)
#define KY_UINT64(x) x
#else
#define KY_UINT64(x) x##LL
#endif

// 8 bit Integer ranges (Byte)
#define KY_MAX_SINT8           Kaim::SInt8(0x7F)                   //  127
#define KY_MIN_SINT8           Kaim::SInt8(0x80)                   // -128
#define KY_MAX_UINT8           Kaim::UInt8(0xFF)                   //  255
#define KY_MIN_UINT8           Kaim::UInt8(0x00)                   //  0

#define KY_MAX_SBYTE           KY_MAX_SINT8                             //  127
#define KY_MIN_SBYTE           KY_MIN_SINT8                             // -128
#define KY_MAX_UBYTE           KY_MAX_UINT8                             //  255
#define KY_MIN_UBYTE           KY_MIN_UINT8                             //  0

// 16 bit Integer ranges (Word)
#define KY_MAX_SINT16          Kaim::SInt16(SHRT_MAX)              //  32767
#define KY_MIN_SINT16          Kaim::SInt16(SHRT_MIN)              // -32768
#define KY_MAX_UINT16          Kaim::UInt16(USHRT_MAX)             //  65535
#define KY_MIN_UINT16          Kaim::UInt16(0)                     //  0

// 32 bit Integer ranges (Int)
#define KY_MAX_SINT32          Kaim::SInt32(INT_MAX)               //  2147483647
#define KY_MIN_SINT32          Kaim::SInt32(INT_MIN)               // -2147483648
#define KY_MAX_UINT32          Kaim::UInt32(UINT_MAX)              //  4294967295
#define KY_MIN_UINT32          Kaim::UInt32(0)                     //  0

// 64 bit Integer ranges (Long)
#define KY_MAX_SINT64          Kaim::SInt64(0x7FFFFFFFFFFFFFFF)    // -9223372036854775808
#define KY_MIN_SINT64          Kaim::SInt64(0x8000000000000000)    //  9223372036854775807
#define KY_MAX_UINT64          Kaim::UInt64(0xFFFFFFFFFFFFFFFF)    //  18446744073709551615
#define KY_MIN_UINT64          Kaim::UInt64(0)                     //  0

// Compiler specific
#define KY_MAX_SINT            int(KY_MAX_SINT32)                       //  2147483647
#define KY_MIN_SINT            int(KY_MIN_SINT32)                       // -2147483648
#define KY_MAX_UINT            unsigned(KY_MAX_UINT32)                  //  4294967295
#define KY_MIN_UINT            unsigned(KY_MIN_UINT32)                  //  0


#if defined(KY_64BIT_POINTERS)
#define KY_MAX_SPINT           KY_MAX_SINT64
#define KY_MIN_SPINT           KY_MIN_SINT64
#define KY_MAX_UPINT           KY_MAX_UINT64
#define KY_MIN_UPINT           KY_MIN_UINT64
#else
#define KY_MAX_SPINT           KY_MAX_SINT
#define KY_MIN_SPINT           KY_MIN_SINT
#define KY_MAX_UPINT           KY_MAX_UINT
#define KY_MIN_UPINT           KY_MIN_UINT
#endif

// ***** Floating point ranges
//
//---------------------------------------------------------------------------
// Minimum and maximum (positive) float values
#define KY_MIN_FLOAT           float(FLT_MIN)               // 1.175494351e-38F
#define KY_MAX_FLOAT           float(FLT_MAX)               // 3.402823466e+38F

#ifdef KY_NO_DOUBLE
// Minimum and maximum (positive) Double values
#define KY_MIN_DOUBLE          Kaim::Double(FLT_MIN)   // 1.175494351e-38F
#define KY_MAX_DOUBLE          Kaim::Double(FLT_MAX)   // 3.402823466e+38F
#else
// Minimum and maximum (positive) Double values
#define KY_MIN_DOUBLE          Kaim::Double(DBL_MIN)   // 2.2250738585072014e-308
#define KY_MAX_DOUBLE          Kaim::Double(DBL_MAX)   // 1.7976931348623158e+308
#endif

// ***** File Path
//
//----------------------------------------------------------------------------------------
namespace Kaim
{
    enum FilePath
    {
#if defined (KY_OS_XBOX360) || defined(KY_OS_WIN32)
        FilePath_Separator = '\\',
        FilePath_OppositeSeparator = '/'
#else
        FilePath_Separator = '/',
        FilePath_OppositeSeparator = '\\'
#endif
    };

}

// ***** Flags
//
//---------------------------------------------------------------------------
#define KY_FLAG32(value)       (Kaim::UInt32(value))
#define KY_FLAG64(value)       (Kaim::UInt64(value))


// ***** Replacement new overrides
//
//---------------------------------------------------------------------------
// Calls constructor on own memory created with "new(ptr) type"
#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE

#if defined(KY_CC_MSVC)
KY_INLINE void*   operator new        (Kaim::UPInt n, void *ptr)   { return ptr; KY_UNUSED(n); }
KY_INLINE void    operator delete     (void *ptr, void *ptr2)           { return; KY_UNUSED2(ptr,ptr2); }
#else
// Needed for placement on many platforms including PSP.
#include <new>
#endif

#endif // __PLACEMENT_NEW_INLINE


// ***** Special Documentation Set
//---------------------------------------------------------------------------
// SF Build type
#if defined(KY_BUILD_DEBUG)
  #if defined(KY_BUILD_DEBUGOPT)
    #define KY_BUILD_STRING         "DebugOpt"
  #else
    #define KY_BUILD_STRING         "Debug"
  #endif
#elif defined(KY_BUILD_SHIPPING)
  #define KY_BUILD_STRING           "Shipping"
#elif defined(KY_BUILD_RELEASE)
  #define KY_BUILD_STRING           "Release"
#else
	#define KY_BUILD_STRING         "UndefinedBuildMode"
#endif


// Questinable
//#ifdef KY_DOM_INCLUDE
//
//// Enables Windows MFC Compatibility
//# define KY_BUILD_MFC
//
//// STL configuration
//// - default is STLSTD if not set
//# define KY_BUILD_STLPORT
//# define KY_BUILD_STLSGI
//# define KY_BUILD_STLSTD
//
//// Enables SF Debugging information
//# define KY_BUILD_DEBUG
//
//// Enables SF Static library build
//# define KY_BUILD_STATICLIB
//
//// Causes the SF Memory allocator
//// to be used for all new and delete's
//# define KY_BUILD_NEW_OVERRIDE
//
//// Prevents SF from defining new within
//// type macros, so developers can override
//// new using the #define new new(...) trick
//// - used with KY_DEFINE_NEW macro
//# define KY_BUILD_DEFINE_NEW
//
//// Enables SF Purify customized build
//# define KY_BUILD_PURIFY
//// Enables SF Evaluation build with time locking
//# define KY_BUILD_EVAL
//
//#endif

namespace Kaim { 

template <typename T1, typename T2>
struct Pair
{
    typedef T1 FirstType;
    typedef T2 SecondType;

    Pair(const FirstType& f, const SecondType& s) : First(f), Second(s) {}

    T1 First;
    T2 Second;
};

} //namespace Kaim { 

#endif  // INC_GTYPES_H
