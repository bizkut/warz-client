/*
 * Copyright 2010 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */

#ifndef PX_ASCII_CONVERSION_H
#define PX_ASCII_CONVERSION_H

/*!
\file
\brief PxAsciiConversion namespace contains string/value helper functions
*/


#include "PsShare.h"
#include "foundation/PxAssert.h"
#include "PsString.h"
#include "foundation/PxIntrinsics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>

namespace physx
{
namespace general_string_parsing2
{
namespace PxAsc
{

const physx::PxU32 PxF32StrLen = 24;
const physx::PxU32 PxF64StrLen = 32;
const physx::PxU32 IntStrLen = 32;

PX_INLINE bool isWhiteSpace(char c);
PX_INLINE const char * skipNonWhiteSpace(const char *scan);
PX_INLINE const char * skipWhiteSpace(const char *scan);

//////////////////////////
// str to value functions
//////////////////////////
PX_INLINE bool strToBool(const char *str, const char **endptr);
PX_INLINE physx::PxI8  strToI8(const char *str, const char **endptr);
PX_INLINE physx::PxI16 strToI16(const char *str, const char **endptr);
PX_INLINE physx::PxI32 strToI32(const char *str, const char **endptr);
PX_INLINE physx::PxI64 strToI64(const char *str, const char **endptr);
PX_INLINE physx::PxU8  strToU8(const char *str, const char **endptr);
PX_INLINE physx::PxU16 strToU16(const char *str, const char **endptr);
PX_INLINE physx::PxU32 strToU32(const char *str, const char **endptr);
PX_INLINE physx::PxU64 strToU64(const char *str, const char **endptr);
PX_INLINE physx::PxF32 strToF32(const char *str, const char **endptr);
PX_INLINE physx::PxF64 strToF64(const char *str, const char **endptr);
PX_INLINE void strToF32s(physx::PxF32 *v,physx::PxU32 count,const char *str, const char**endptr);


//////////////////////////
// value to str functions
//////////////////////////
PX_INLINE const char * valueToStr( bool val, char *buf, physx::PxU32 n );
PX_INLINE const char * valueToStr( physx::PxI8 val, char *buf, physx::PxU32 n );
PX_INLINE const char * valueToStr( physx::PxI16 val, char *buf, physx::PxU32 n );
PX_INLINE const char * valueToStr( physx::PxI32 val, char *buf, physx::PxU32 n );
PX_INLINE const char * valueToStr( physx::PxI64 val, char *buf, physx::PxU32 n );
PX_INLINE const char * valueToStr( physx::PxU8 val, char *buf, physx::PxU32 n );
PX_INLINE const char * valueToStr( physx::PxU16 val, char *buf, physx::PxU32 n );
PX_INLINE const char * valueToStr( physx::PxU32 val, char *buf, physx::PxU32 n );
PX_INLINE const char * valueToStr( physx::PxU64 val, char *buf, physx::PxU32 n );
PX_INLINE const char * valueToStr( physx::PxF32 val, char *buf, physx::PxU32 n );
PX_INLINE const char * valueToStr( physx::PxF64 val, char *buf, physx::PxU32 n );

#include "PxAsciiConversion.inl"

}; // end of namespace
}; // end of namespace
using namespace general_string_parsing2;
}; // end of namespace


#endif // PX_ASCII_CONVERSION_H
