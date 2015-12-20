// This code contains NVIDIA Confidential Information and is disclosed to you 
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and 
// any modifications thereto. Any use, reproduction, disclosure, or 
// distribution of this software and related documentation without an express 
// license agreement from NVIDIA Corporation is strictly prohibited.
// 
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2008-2012 NVIDIA Corporation. All rights reserved.


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

/*!
\file
\brief MiAsciiConversion namespace contains string/value helper functions
*/

MI_INLINE bool isWhiteSpace(char c)
{
	bool ret = false;
	if ( c == 32 || c == 9 || c == 13 || c == 10 || c == ',' ) ret = true;
	return ret;
}

MI_INLINE const char * skipNonWhiteSpace(const char *scan)
{
	while ( !isWhiteSpace(*scan) && *scan) scan++;
	if ( *scan == 0 ) scan = NULL;
	return scan;
}
MI_INLINE const char * skipWhiteSpace(const char *scan)
{
	while ( isWhiteSpace(*scan) && *scan ) scan++;
	if ( *scan == 0 ) scan = NULL;
	return scan;
}

//////////////////////////
// str to value functions
//////////////////////////
MI_INLINE bool strToBool(const char *str, const char **endptr)
{
	bool ret = false;
	const char *begin = skipWhiteSpace(str);
	const char *end = skipNonWhiteSpace(begin);

	if( !end )
		end = begin + strlen(str);

	MiI32 len = (MiI32)(end - begin);
	if ( MESH_IMPORT_STRING::strnicmp(begin,"true", len) == 0 || MESH_IMPORT_STRING::strnicmp(begin,"1", len) == 0 )
		ret = true;	

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);
	
	return ret;
}

MI_INLINE MiI8  strToI8(const char *str, const char **endptr)
{
	MiI8 ret;
	const char *begin = skipWhiteSpace(str);
	const char *end = skipNonWhiteSpace(begin);

	if( !end )
		end = begin + strlen(str);
	
	if( strncmp(begin, "PX_MIN_I8", end-begin) == 0)
		ret = MI_MIN_I8;
	else if( strncmp(begin, "PX_MAX_I8", end-begin) == 0)
		ret = MI_MAX_I8;
	else
	 	ret = (MiI8)strtol(begin, 0, 0); //FIXME

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

MI_INLINE MiI16 strToI16(const char *str, const char **endptr)
{
	MiI16 ret;
	const char *begin = skipWhiteSpace(str);
	const char *end = skipNonWhiteSpace(begin);

	if( !end )
		end = begin + strlen(str);
	
	if( strncmp(begin, "PX_MIN_I16", end-begin) == 0)
		ret = MI_MIN_I16;
	else if( strncmp(begin, "PX_MAX_I16", end-begin) == 0)
		ret = MI_MAX_I16;
	else
	 	ret = (MiI16)strtol(begin, 0, 0); //FIXME
	
	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

MI_INLINE MiI32 strToI32(const char *str, const char **endptr)
{
	MiI32 ret;
	const char *begin = skipWhiteSpace(str);
	const char *end = skipNonWhiteSpace(begin);

	if( !end )
		end = begin + strlen(str);
	
	if( strncmp(begin, "PX_MIN_I32", end-begin) == 0)
		ret = MI_MIN_I32;
	else if( strncmp(begin, "PX_MAX_I32", end-begin) == 0)
		ret = MI_MAX_I32;
	else
	 	ret = (MiI32)strtol(begin, 0, 0); //FIXME

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

MI_INLINE MiI64 strToI64(const char *str, const char **endptr)
{
	MiI64 ret;
	const char *begin = skipWhiteSpace(str);
    
	//FIXME
#ifdef _WIN32 //MI_WINDOWS, MI_XBOX
 	ret = (MiU64)_strtoi64(begin,0,10);
#else
	ret = (MiU64)strtoll(begin,0,10);
#endif

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

MI_INLINE MiU8  strToU8(const char *str, const char **endptr)
{
	MiU8 ret;
	const char *begin = skipWhiteSpace(str);
	
	ret = (MiU8)strtoul(begin, 0, 0);

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

MI_INLINE MiU16 strToU16(const char *str, const char **endptr)
{
	MiU16 ret;
	const char *end;
	const char *begin = skipWhiteSpace(str);

	end = skipNonWhiteSpace(begin);
	if( !end )
		end = begin + strlen(str);
	
	if( strncmp(begin, "PX_MAX_U16", end-begin) == 0)
		ret = MI_MAX_U16;
	else
	 	ret = (MiU16)strtoul(begin,0,0);

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

MI_INLINE MiU32 strToU32(const char *str, const char **endptr)
{
	MiU32 ret;
	const char *begin = skipWhiteSpace(str);
	const char *end = skipNonWhiteSpace(begin);

	if( !end )
		end = begin + strlen(str);
	
	if( strncmp(begin, "PX_MAX_U32", end-begin) == 0)
		ret = MI_MAX_U32;
	else
	 	ret = (MiU32)strtoul(begin,0,0);

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

MI_INLINE MiU64 strToU64(const char *str, const char **endptr)
{
	MiU64 ret;
	const char *begin;
	begin = skipWhiteSpace(str);

	//FIXME
#ifdef _WIN32 //MI_WINDOWS, MI_XBOX
 	ret = (MiU64)_strtoui64(begin,0,10);
#else
	ret = (MiU64)strtoull(begin,0,10);
#endif

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

MI_INLINE MiF32 strToF32(const char *str, const char **endptr)
{
	MiF32 ret;
	const char *begin = skipWhiteSpace(str);
	const char *end = skipNonWhiteSpace(begin);
	char tmpStr[MiF32StrLen];

	if( !end )
		end = begin + strlen(str);
	
	// str can be very very long, so we should copy the value to a tmp buffer
	MESH_IMPORT_STRING::strncpy_s(tmpStr, MiF32StrLen, begin, end-begin);
	tmpStr[end-begin] = 0;

	if( strncmp(tmpStr, "PX_MIN_F32", end-begin) == 0)
		ret = -MI_MAX_F32;
	else if( strncmp(tmpStr, "PX_MAX_F32", end-begin) == 0)
		ret = MI_MAX_F32;
	else
		ret = (MiF32)atof(tmpStr);
	
#if DEBUGGING_MISMATCHES
	MiF32 testRet = (MiF32)atof(begin);
	if( ret != testRet )
	{
		MI_ASSERT(0 && "Inaccurate float string");
	}
#endif

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}


MI_INLINE MiF64 strToF64(const char *str, const char **endptr)
{
	MiF64 ret;
	const char *begin = skipWhiteSpace(str);
	const char *end = skipNonWhiteSpace(begin);
	char tmpStr[MiF64StrLen];

	end = skipNonWhiteSpace(begin);

	if( !end )
		end = begin + strlen(str);
	
	// str can be very very long, so we should copy the value to a tmp buffer
	MESH_IMPORT_STRING::strncpy_s(tmpStr, MiF64StrLen, begin, end-begin);
	tmpStr[end-begin] = 0;
	
	if( strncmp(tmpStr, "PX_MIN_F64", end-begin) == 0)
		ret = -MI_MAX_F64;
	else if( strncmp(tmpStr, "PX_MAX_F64", end-begin) == 0)
		ret = MI_MAX_F64;
	else 
		ret = (MiF64)atof(tmpStr);

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

MI_INLINE void strToF32s(MiF32 *v,MiU32 count,const char *str, const char**endptr)
{
	const char *begin = skipWhiteSpace(str);

	if ( *begin == '(' ) begin++;
	for (MiU32 i=0; i<count && *begin; i++)
	{
		v[i] = (MiF32)strToF32(begin, &begin);
	}

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);
}


//////////////////////////
// value to str functions
//////////////////////////
MI_INLINE const char * valueToStr( bool val, char *buf, MiU32 n )
{
	MESH_IMPORT_STRING::sprintf_s(buf, n,"%s",val ? "true" : "false");
	return buf;
}

MI_INLINE const char * valueToStr( MiI8 val, char *buf, MiU32 n )
{
	if( val == MI_MIN_I8 )
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%s","PX_MIN_I8" );
	else if( val == MI_MAX_I8 )
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%s","PX_MAX_I8" );
	else
		MESH_IMPORT_STRING::sprintf_s(buf, n, "%d", val);
	return buf;
}

MI_INLINE const char * valueToStr( MiI16 val, char *buf, MiU32 n )
{
	if( val == MI_MIN_I16 )
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%s","PX_MIN_I16" );
	else if( val == MI_MAX_I16 )
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%s","PX_MAX_I16" );
	else
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%d",val );
	return buf;
}

MI_INLINE const char * valueToStr( MiI32 val, char *buf, MiU32 n )
{
	if( val == MI_MIN_I32 )
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%s","PX_MIN_I32" );
	else if( val == MI_MAX_I32 )
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%s","PX_MAX_I32" );
	else
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%d",val );
	return buf;
}

MI_INLINE const char * valueToStr( MiI64 val, char *buf, MiU32 n )
{
	MESH_IMPORT_STRING::sprintf_s(buf, n,"%lld",val );
	return buf;
}

MI_INLINE const char * valueToStr( MiU8 val, char *buf, MiU32 n )
{
	MESH_IMPORT_STRING::sprintf_s(buf, n, "%u", val);
	return buf;
}

MI_INLINE const char * valueToStr( MiU16 val, char *buf, MiU32 n )
{
	if( val == MI_MAX_U16 )
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%s","PX_MAX_U16" );
	else
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%u",val );
	return buf;
}

MI_INLINE const char * valueToStr( MiU32 val, char *buf, MiU32 n )
{
	if( val == MI_MAX_U32 )
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%s","PX_MAX_U32" );
	else
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%u",val );
	return buf;
}

MI_INLINE const char * valueToStr( MiU64 val, char *buf, MiU32 n )
{
	MESH_IMPORT_STRING::sprintf_s(buf, n,"%llu",val );
	return buf;
}

MI_INLINE const char * valueToStr( MiF32 val, char *buf, MiU32 n )
{
	if( !MESH_IMPORT_INTRINSICS::isFinite(val) )
	{
		MI_ASSERT( 0 && "invalid floating point" );
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%s","0" );
	}
	else if( val == -MI_MAX_F32 )
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%s","PX_MIN_F32" );
	else if( val == MI_MAX_F32 )
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%s","PX_MAX_F32" );
    else if ( val == 1 )
    	MESH_IMPORT_STRING::strcpy_s(buf, n, "1");
    else if ( val == 0 )
    	MESH_IMPORT_STRING::strcpy_s(buf, n, "0");
    else if ( val == - 1 )
    	MESH_IMPORT_STRING::strcpy_s(buf, n, "-1");
    else
    {
		MESH_IMPORT_STRING::sprintf_s(buf,n,"%.9g", (double)val ); // %g expects double
		const char *dot = strchr(buf,'.');
		const char *e = strchr(buf,'e');
		if ( dot && !e )
		{
			MiI32 len = (MiI32)strlen(buf);
			char *foo = &buf[len-1];
			while ( *foo == '0' ) foo--;
			if ( *foo == '.' )
				*foo = 0;
			else
				foo[1] = 0;
		}
    }
	return buf;
}

MI_INLINE const char * valueToStr( MiF64 val, char *buf, MiU32 n )
{
	if( !MESH_IMPORT_INTRINSICS::isFinite(val) )
	{
		MI_ASSERT( 0 && "invalid floating point" );
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%s","0" );
	}
	else if( val == -MI_MAX_F64 )
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%s","PX_MIN_F64" );
	else if( val == MI_MAX_F64 )
		MESH_IMPORT_STRING::sprintf_s(buf, n,"%s","PX_MAX_F64" );
    else if ( val == 1 )
		MESH_IMPORT_STRING::strcpy_s(buf, n, "1");
    else if ( val == 0 )
    	MESH_IMPORT_STRING::strcpy_s(buf, n, "0");
    else if ( val == - 1 )
    	MESH_IMPORT_STRING::strcpy_s(buf, n, "-1");
    else
    {
		MESH_IMPORT_STRING::sprintf_s(buf,n,"%.18g", val );
		const char *dot = strchr(buf,'.');
		const char *e = strchr(buf,'e');
		if ( dot && !e )
		{
			MiI32 len = (MiI32)strlen(buf);
			char *foo = &buf[len-1];
			while ( *foo == '0' ) foo--;
			if ( *foo == '.' )
				*foo = 0;
			else
				foo[1] = 0;
		}
    }
	return buf;
}
