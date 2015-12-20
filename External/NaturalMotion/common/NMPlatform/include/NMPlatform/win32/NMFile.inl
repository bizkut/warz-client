// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------

#if !(defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64))
  #error This file is specific to Windows
#endif // !(defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64))

#include <sys/stat.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

// This platform does support stat.
#define NM_PLATFORM_SUPPORTS_STAT

#define NM_FSEEK(file, offset, flag) _fseeki64(file, offset, flag)
#define NM_FOPEN(file, filename, mode) fopen_s(&file, filename, mode)

//----------------------------------------------------------------------------------------------------------------------
