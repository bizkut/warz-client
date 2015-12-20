/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

/**************************************************************************

Filename    :   KY_SIMD.h
Content     :   Defines and includes for SIMD functionality.
Created     :   Dec 2010
Authors     :   Bart Muzzin

**************************************************************************/

#ifndef INC_KY_SF_SIMD_H
#define INC_KY_SF_SIMD_H
#pragma once

#include "gwnavruntime/kernel/SF_Config.h"
#include "gwnavruntime/kernel/SF_Types.h"

// Forward defines.
// Vector4f         typedef for 128 bit register for floating point data.
// Vector4i         typedef for 128 bit register for integer data.
// KY_SIMD_ALIGN    required alignment attribute for any data used by SIMD operations.
// IS               typedef for the default SIMD instruction set for a platform. This should
//                  be something that is minimally supported on the platform.

#if defined(KY_ENABLE_SIMD)
    #if defined (KY_CPU_SSE) 
        #include "gwnavruntime/kernel/SF_SIMD_SSE.h"
    #elif defined(KY_OS_XBOX360)
        #include "gwnavruntime/kernel/SF_SIMD_X360.h"
    #elif defined(KY_CPU_ALTIVEC)
        #include "gwnavruntime/kernel/SF_SIMD_Altivec.h"
    #elif defined(KY_CPU_ARM_NEON) && !defined(KY_OS_PSVITA)
        #include "gwnavruntime/kernel/SF_SIMD_ARMNeon.h"
    #else
        // Your platform does not support any SIMD processor - disabling.
        #undef KY_ENABLE_SIMD
    #endif
#endif // KY_ENABLE_SIMD


#if !defined(KY_ENABLE_SIMD)

    #define KY_SIMD_ALIGN(x) x   // No alignment for non-SIMD platforms.

    // Prefetch/Obj are special cases, are used mainly within functions, so make them available
    // always, even if they don't do anything.
namespace Kaim { namespace SIMD 
    { 
        class InstructionSet
        {
        public:
            static void Prefetch( const void * /*p*/, int /*offset*/ ) { }
            template<class T>
            static void PrefetchObj( const T * /*obj*/ ) { }
        };
        typedef InstructionSet IS;
    }}
#endif


#endif // INC_KY_SF_SIMD_H
