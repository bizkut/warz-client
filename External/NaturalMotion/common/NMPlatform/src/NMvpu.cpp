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
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMvpu.h"

#ifdef NM_HOST_ANDROID
#include <cpu-features.h>
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
namespace vpu
{

//----------------------------------------------------------------------------------------------------------------------
bool checkForHardwareSupport()
{
#if defined(NMP_PLATFORM_SIMD)
  return true;
#elif defined(NM_HOST_ANDROID)
  return android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON;
#else
  return false;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
}// namespace vpu

//----------------------------------------------------------------------------------------------------------------------
}// namespace NMP
