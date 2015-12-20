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

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup NaturalMotionPlatformMath Natural Motion platform specific core maths library.
/// \ingroup NaturalMotionPlatform
///
/// Contains common core maths functionality with specific implementations for target platforms.
//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_MATHPLATFORM_H
#define NM_MATHPLATFORM_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_VPU_MIXEDx
/// \ingroup NaturalMotionVPU
/// \brief Define to allow selective fpu emulation of vectorized code.
#define NMP_VPU_MIXEDx

//----------------------------------------------------------------------------------------------------------------------
/// \def NMP_VPU_FORCE_EMULATIONx
/// \ingroup NaturalMotionVPU
/// \brief Define to force fpu compilation of all vectorized code (non ps3).
#define NMP_VPU_FORCE_EMULATIONx

//----------------------------------------------------------------------------------------------------------------------
#if !defined(NMP_PLATFORM_SIMD) || (defined(NMP_VPU_FORCE_EMULATION) && !defined(NM_HOST_CELL_SPU) && !defined(NM_HOST_CELL_PPU))
  #define NMP_VPU_EMULATION
#endif

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_MATHPLATFORM_H
//----------------------------------------------------------------------------------------------------------------------
