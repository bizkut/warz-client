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

#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_VPU_H
#define NM_VPU_H

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup NaturalMotionVPU Natural Motion Vector Intrinsics Optimization Layer.
/// \ingroup NaturalMotionPlatform
///
/// Contains and controls vector intrinsics optimization on all supported architectures.
//----------------------------------------------------------------------------------------------------------------------

#include "NMPlatform/NMMathPlatform.h" // contains vpu compilation options

#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable:4201) // - nonstandard extension used : nameless struct/union
#endif

//----------------------------------------------------------------------------------------------------------------------

// the base vector4_t typedef

#if defined(NMP_VPU_EMULATION)

//----------------------------------------------------------------------------------------------------------------------
/// \namespace vpu
/// \ingroup NaturalMotionVPU
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
  namespace vpu
  {
    typedef struct vector4_t
    {
      union
      {
        struct
        {
          float x;
          float y;
          float z;
          float w;
        };
        float        v[4];
        unsigned int u[4];
        int          i[4];
      };
    } vector4_t;
  }
}

#else // defined(NMP_VPU_EMULATION)

// If this include cannot be found, there may be several causes:
//  - You do not have the NMPlatform platform-specific include directory in your include path.
//  - NMvpu has not been implemented for your platform when it should have been.
//  - Your platform does not support SIMD but you have NMP_PLATFORM_SIMD defined.
#include "NMvpuPlatformIncludes.h"

#endif // defined(NMP_VPU_EMULATION)

//----------------------------------------------------------------------------------------------------------------------

// the vpu layer

namespace NMP
{
  namespace vpu
  {

// low level functionality
#include "NMPlatform/vpu/NMvpuSpec.inl"

// platform implementation
#if defined(NMP_VPU_EMULATION)
  #include "NMPlatform/vpu/NMfpu.inl"
#else // NMP_VPU_EMULATION
  // Platform-specific VPU implementation
  #include "NMvpuImplementation.inl"
#endif // NMP_VPU_EMULATION

// higher level and multiplex functionality
#include "NMPlatform/vpu/NMvpuInc.inl"

  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \namespace fpu
/// \ingroup NaturalMotionVPU
//----------------------------------------------------------------------------------------------------------------------

#if defined(NMP_VPU_MIXED) && !defined(NMP_VPU_EMULATION)

// this is for selective emulation of code, 'just' use fpu:: instead of vpu::
// (some ps3 limitations)

namespace NMP
{
  namespace fpu
  {
    typedef struct vector4_t
    {
      union
      {
        struct
        {
          float x;
          float y;
          float z;
          float w;
        };
        float        v[4];
        unsigned int u[4];
        int          i[4];
      };
    } vector4_t;

#include "NMPlatform/vpu/NMvpuSpec.inl"
#include "NMPlatform/vpu/NMfpu.inl"
#include "NMPlatform/vpu/NMvpuInc.inl"

  }
}

#else

// not mixed, so let any remaining fpu code revert to vpu compilation
// (vpu duplicated fpu code should be guarded with #ifdef NMP_VPU_MIXED)
namespace NMP
{
  namespace fpu = vpu;
}

#endif 

// some vpu shortcut macros
#define add_4f(_val1, _val2) NMP::vpu::add4f(_val1, _val2)
#define sub_4f(_val1, _val2) NMP::vpu::sub4f(_val1, _val2)
#define mul_4f(_val1, _val2) NMP::vpu::mul4f(_val1, _val2)
#define madd_4f(_val1, _val2, _vadd) NMP::vpu::madd4f(_val1, _val2, _vadd)
#define rcp_4f(_val) NMP::vpu::rcp4f(_val)

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{
  namespace vpu
  {
    /// \brief Checks for hardware support of SIMD intrinsics even if NMP_FORCE_VPU_EMULATION is defined.  If this 
    /// function fails and NMP_PLATFORM_SIMD is defined then running any NMP::vpu code may produce undefined results,
    /// compile with NMP_FORCE_VPU_EMULATION to prevent any hardware vpu code being created.
    bool checkForHardwareSupport();
  }
}

#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_VPU_H
//----------------------------------------------------------------------------------------------------------------------
