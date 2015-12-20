/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

PublicHeader:   None
Filename    :   GFxConfig.h
Content     :   GFx configuration file - contains #ifdefs for
                the optional components of the library
Created     :   June 15, 2005
Authors     :   All Scaleform Engineers

**************************************************************************/

#ifndef INC_KY_SF_Config_H
#define INC_KY_SF_Config_H

// ***** Prerequisites
//---------------------------------------------------------------------------

// KY_BUILD_DEBUG, KY_BUILD_RELEASE, KY_BUILD_SHIPPING
// have to be defined explicitly by users in their projects
// to ensure synchronization between linked libraries and header files

#include "gwnavruntime/kernel/SF_Types.h" // reqd for KY_OS_*** macros


// ***** General Scaleform options
//---------------------------------------------------------------------------

// This macro needs to be defined if it is necessary to avoid the use of Double.
// In that case Double in defined as float and thus extra #ifdef checks on
// overloads need to be done.
// NOTE: By default, PS2 and PSP define this if not manually defined here.
//#define KY_NO_DOUBLE

// Define this macro to eliminate custom wctype tables for functions such as
// G_iswspace, SFtowlower, g_towupper and so on. If this macro is defined GFx 
// will use system Unicode functions (which are incredibly slow on Microsoft 
// Windows).
//#define KY_NO_WCTYPE

// Enable thread support
#define KY_ENABLE_THREADS

#if defined(KY_OS_WINMETRO)
// Use standard C++11 thread library
#ifdef KY_ENABLE_THREADS
#define KY_USE_STD11_THREADS
#endif
#endif

// Define this to disable statistics tracking; this is useful for the final build.
#define KY_ENABLE_STATS


// Define this macro if whole JPEGLIB is compiled as C++ code (CPP-ize). 
// By default libjpeg is pure C library and public names are not mangled. 
// Though, it might be necessary to mangle jpeglib's names in order to 
// resolve names clashing issues (for example, with XBox360's xmedia.lib).
//#define KY_CPP_LIBJPEG

// Enable use of ZLIB and GZLibFile class.
// When disabled, compressed SWF files will no longer load.
// Disabling saves roughly 20k in release build.
//#define KY_ENABLE_ZLIB

// Enable the use of SIMD optimized functions, if available. If the platform does
// not support a SIMD instruction set, then this option will be disabled in KY_SIMD.h.
// SIMD instructions are disabled in debug builds, because without function inlining,
// they are considerably slower.
#ifndef KY_BUILD_DEBUG
    #define KY_ENABLE_SIMD
#endif

// Enable the use of TCP/IP sockets
// This is needed for AMP
#ifndef KY_BUILD_SHIPPING
    #define KY_ENABLE_SOCKETS
    #define KY_WIIU_SOCKET_USE_HIO
#endif

// ***** Memory/Allocation Configuration
//---------------------------------------------------------------------------

// These defines control memory tracking used for statistics and leak detection.
// By default, memory debug functionality is enabled automatically by KY_BUILD_DEBUG.
#ifdef KY_BUILD_DEBUG
    // Enable debug memory tracking. This passes __FILE__ and __LINE__ data into
    // allocations and enabled DebugStorage in the memory heap used to track
    // statistics and memory leaks.
    #define KY_MEMORY_ENABLE_DEBUG_INFO

    // Wipe memory in the allocator; this is useful for debugging.
    #define KY_MEMORY_MEMSET_ALL

	// Keeps a record of the stack of each allocation.
#if defined(_MSC_VER) && (_MSC_VER > 1400) && !defined(KY_OS_WINMETRO) && defined(KY_OS_WIN32)
	#define KY_ENABLE_STACK_REGISTRY
#endif
    // Check for corrupted memory in the free chunks. May significantly
    // slow down the allocations.
    //#define KY_MEMORY_CHECK_CORRUPTION
#else
    // Enable debug memory tracking even in the release mode.
    //#define KY_MEMORY_ENABLE_DEBUG_INFO
#endif

// Use for extreme debug only! This define enforces the use of system 
// memory allocator as much as possible. That is: no granulator; absolute 
// minimal heap and system granularities; no reserve; no segment caching.
// It increases the chances of early memory corruption detection on 
// segmentation fault. Works only with GSysAllocWinAPI and GSysAllocMMAP.
//#define KY_MEMORY_FORCE_SYSALLOC

// Use for extreme debug only! Enforces the use of _aligned_malloc/_aligned_free
// No memory heaps functionality, no statistics, no heap memory limits.
//#define KY_MEMORY_FORCE_MALLOC

// Trace all essential operations: CreateHeap, DestroyHeap, Alloc, Free, Realloc.
// See MemoryHeap::HeapTracer for details.
//#define KY_MEMORY_TRACE_ALL





// ***** Example defines for lightweight GFX on small consoles
//---------------------------------------------------------------------------

#ifdef KY_BUILD_LITE
    #undef KY_ENABLE_ZLIB
    #undef KY_ENABLE_STATS
#endif


#endif
