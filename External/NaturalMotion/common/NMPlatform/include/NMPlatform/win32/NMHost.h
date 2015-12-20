// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.  
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

// Included via NMPlatform.h.  Not to be included directly.

//----------------------------------------------------------------------------------------------------------------------
#if !(defined(WIN32) || defined(_WIN32))
  #error Windows NMHost.h file included in a non-Windows build!
#endif

#ifndef NM_DEFINED_FIXED_WIDTH_TYPEDEFS
  // These typedefs are required because stdint.h is not provided with Visual Studio.
  #define NM_DEFINED_FIXED_WIDTH_TYPEDEFS
  typedef signed    __int8    int8_t;
  typedef signed    __int16   int16_t;
  typedef signed    __int32   int32_t;
  typedef signed    __int64   int64_t;
  typedef unsigned  __int8    uint8_t;
  typedef unsigned  __int16   uint16_t;
  typedef unsigned  __int32   uint32_t;
  typedef unsigned  __int64   uint64_t;
#endif // NM_DEFINED_FIXED_WIDTH_TYPEDEFS

// Windows, but which type?
#if defined(_WIN64)
  #ifndef NM_HOST_WIN64
    #define NM_HOST_WIN64
  #endif
  #define NM_HOST_64_BIT
#else
  #ifndef NM_HOST_WIN32
    #define NM_HOST_WIN32
  #endif
#endif // _WIN64

#define NM_HOST_BIGENDIAN 0
#define NMP_PLATFORM_SIMD
#define NM_BREAK()   __debugbreak()
#define NM_HOST_USES_SECURE_STRING_FNS
//----------------------------------------------------------------------------------------------------------------------
