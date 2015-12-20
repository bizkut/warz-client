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

#ifndef MI_ASCII_CONVERSION_H
#define MI_ASCII_CONVERSION_H

/*!
\file
\brief MiAsciiConversion namespace contains string/value helper functions
*/


#include "MiPlatformConfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <float.h>

namespace mimp
{

	namespace MiAsc
	{

const MiU32 MiF32StrLen = 24;
const MiU32 MiF64StrLen = 32;
const MiU32 IntStrLen = 32;

MI_INLINE bool isWhiteSpace(char c);
MI_INLINE const char * skipNonWhiteSpace(const char *scan);
MI_INLINE const char * skipWhiteSpace(const char *scan);

//////////////////////////
// str to value functions
//////////////////////////
MI_INLINE bool strToBool(const char *str, const char **endptr);
MI_INLINE MiI8  strToI8(const char *str, const char **endptr);
MI_INLINE MiI16 strToI16(const char *str, const char **endptr);
MI_INLINE MiI32 strToI32(const char *str, const char **endptr);
MI_INLINE MiI64 strToI64(const char *str, const char **endptr);
MI_INLINE MiU8  strToU8(const char *str, const char **endptr);
MI_INLINE MiU16 strToU16(const char *str, const char **endptr);
MI_INLINE MiU32 strToU32(const char *str, const char **endptr);
MI_INLINE MiU64 strToU64(const char *str, const char **endptr);
MI_INLINE MiF32 strToF32(const char *str, const char **endptr);
MI_INLINE MiF64 strToF64(const char *str, const char **endptr);
MI_INLINE void strToF32s(MiF32 *v,MiU32 count,const char *str, const char**endptr);


//////////////////////////
// value to str functions
//////////////////////////
MI_INLINE const char * valueToStr( bool val, char *buf, MiU32 n );
MI_INLINE const char * valueToStr( MiI8 val, char *buf, MiU32 n );
MI_INLINE const char * valueToStr( MiI16 val, char *buf, MiU32 n );
MI_INLINE const char * valueToStr( MiI32 val, char *buf, MiU32 n );
MI_INLINE const char * valueToStr( MiI64 val, char *buf, MiU32 n );
MI_INLINE const char * valueToStr( MiU8 val, char *buf, MiU32 n );
MI_INLINE const char * valueToStr( MiU16 val, char *buf, MiU32 n );
MI_INLINE const char * valueToStr( MiU32 val, char *buf, MiU32 n );
MI_INLINE const char * valueToStr( MiU64 val, char *buf, MiU32 n );
MI_INLINE const char * valueToStr( MiF32 val, char *buf, MiU32 n );
MI_INLINE const char * valueToStr( MiF64 val, char *buf, MiU32 n );

#include "MiAsciiConversion.inl"

	}; // end of MiAsc namespace

}; // end of namespace


#endif // MI_ASCII_CONVERSION_H
