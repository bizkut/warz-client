
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
\brief PxAsciiConversion namespace contains string/value helper functions
*/

PX_INLINE bool isWhiteSpace(char c)
{
	bool ret = false;
	if ( c == 32 || c == 9 || c == 13 || c == 10 || c == ',' ) ret = true;
	return ret;
}

PX_INLINE const char * skipNonWhiteSpace(const char *scan)
{
	while ( !isWhiteSpace(*scan) && *scan) scan++;
	if ( *scan == 0 ) scan = NULL;
	return scan;
}
PX_INLINE const char * skipWhiteSpace(const char *scan)
{
	while ( isWhiteSpace(*scan) && *scan ) scan++;
	if ( *scan == 0 ) scan = NULL;
	return scan;
}

//////////////////////////
// str to value functions
//////////////////////////
PX_INLINE bool strToBool(const char *str, const char **endptr)
{
	bool ret = false;
	const char *begin = skipWhiteSpace(str);
	const char *end = skipNonWhiteSpace(begin);

	if( !end )
		end = begin + strlen(str);

	physx::PxI32 len = (physx::PxI32)(end - begin);
	if ( physx::string::strnicmp(begin,"true", len) == 0 || physx::string::strnicmp(begin,"1", len) == 0 )
		ret = true;	

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);
	
	return ret;
}

PX_INLINE physx::PxI8  strToI8(const char *str, const char **endptr)
{
	physx::PxI8 ret;
	const char *begin = skipWhiteSpace(str);
	const char *end = skipNonWhiteSpace(begin);

	if( !end )
		end = begin + strlen(str);
	
	if( strncmp(begin, "PX_MIN_I8", end-begin) == 0)
		ret = PX_MIN_I8;
	else if( strncmp(begin, "PX_MAX_I8", end-begin) == 0)
		ret = PX_MAX_I8;
	else
	 	ret = (physx::PxI8)strtol(begin, 0, 0); //FIXME

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

PX_INLINE physx::PxI16 strToI16(const char *str, const char **endptr)
{
	physx::PxI16 ret;
	const char *begin = skipWhiteSpace(str);
	const char *end = skipNonWhiteSpace(begin);

	if( !end )
		end = begin + strlen(str);
	
	if( strncmp(begin, "PX_MIN_I16", end-begin) == 0)
		ret = PX_MIN_I16;
	else if( strncmp(begin, "PX_MAX_I16", end-begin) == 0)
		ret = PX_MAX_I16;
	else
	 	ret = (physx::PxI16)strtol(begin, 0, 0); //FIXME
	
	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

PX_INLINE physx::PxI32 strToI32(const char *str, const char **endptr)
{
	physx::PxI32 ret;
	const char *begin = skipWhiteSpace(str);
	const char *end = skipNonWhiteSpace(begin);

	if( !end )
		end = begin + strlen(str);
	
	if( strncmp(begin, "PX_MIN_I32", end-begin) == 0)
		ret = PX_MIN_I32;
	else if( strncmp(begin, "PX_MAX_I32", end-begin) == 0)
		ret = PX_MAX_I32;
	else
	 	ret = (physx::PxI32)strtol(begin, 0, 0); //FIXME

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

PX_INLINE physx::PxI64 strToI64(const char *str, const char **endptr)
{
	physx::PxI64 ret;
	const char *begin = skipWhiteSpace(str);
    
	//FIXME
#ifdef _WIN32 //PX_WINDOWS, PX_XBOX
 	ret = (physx::PxU64)_strtoi64(begin,0,10);
#else
	ret = (physx::PxU64)strtoll(begin,0,10);
#endif

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

PX_INLINE physx::PxU8  strToU8(const char *str, const char **endptr)
{
	physx::PxU8 ret;
	const char *begin = skipWhiteSpace(str);
	
	ret = (physx::PxU8)strtoul(begin, 0, 0);

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

PX_INLINE physx::PxU16 strToU16(const char *str, const char **endptr)
{
	physx::PxU16 ret;
	const char *end;
	const char *begin = skipWhiteSpace(str);

	end = skipNonWhiteSpace(begin);
	if( !end )
		end = begin + strlen(str);
	
	if( strncmp(begin, "PX_MAX_U16", end-begin) == 0)
		ret = PX_MAX_U16;
	else
	 	ret = (physx::PxU16)strtoul(begin,0,0);

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

PX_INLINE physx::PxU32 strToU32(const char *str, const char **endptr)
{
	physx::PxU32 ret;
	const char *begin = skipWhiteSpace(str);
	const char *end = skipNonWhiteSpace(begin);

	if( !end )
		end = begin + strlen(str);
	
	if( strncmp(begin, "PX_MAX_U32", end-begin) == 0)
		ret = PX_MAX_U32;
	else
	 	ret = (physx::PxU32)strtoul(begin,0,0);

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

PX_INLINE physx::PxU64 strToU64(const char *str, const char **endptr)
{
	physx::PxU64 ret;
	const char *begin;
	begin = skipWhiteSpace(str);

	//FIXME
#ifdef _WIN32 //PX_WINDOWS, PX_XBOX
 	ret = (physx::PxU64)_strtoui64(begin,0,10);
#else
	ret = (physx::PxU64)strtoull(begin,0,10);
#endif

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

PX_INLINE physx::PxF32 strToF32(const char *str, const char **endptr)
{
	physx::PxF32 ret;
	const char *begin = skipWhiteSpace(str);
	const char *end = skipNonWhiteSpace(begin);
	char tmpStr[PxF32StrLen];

	if( !end )
		end = begin + strlen(str);
	
	// str can be very very long, so we should copy the value to a tmp buffer
	physx::string::strncpy_s(tmpStr, PxF32StrLen, begin, end-begin);
	tmpStr[end-begin] = 0;

	if( strncmp(tmpStr, "PX_MIN_F32", end-begin) == 0)
		ret = -PX_MAX_F32;
	else if( strncmp(tmpStr, "PX_MAX_F32", end-begin) == 0)
		ret = PX_MAX_F32;
	else
		ret = (physx::PxF32)atof(tmpStr);
	
#if DEBUGGING_MISMATCHES
	physx::PxF32 testRet = (physx::PxF32)atof(begin);
	if( ret != testRet )
	{
		PX_ASSERT(0 && "Inaccurate float string");
	}
#endif

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}


PX_INLINE physx::PxF64 strToF64(const char *str, const char **endptr)
{
	physx::PxF64 ret;
	const char *begin = skipWhiteSpace(str);
	const char *end = skipNonWhiteSpace(begin);
	char tmpStr[PxF64StrLen];

	end = skipNonWhiteSpace(begin);

	if( !end )
		end = begin + strlen(str);
	
	// str can be very very long, so we should copy the value to a tmp buffer
	physx::string::strncpy_s(tmpStr, PxF64StrLen, begin, end-begin);
	tmpStr[end-begin] = 0;
	
	if( strncmp(tmpStr, "PX_MIN_F64", end-begin) == 0)
		ret = -PX_MAX_F64;
	else if( strncmp(tmpStr, "PX_MAX_F64", end-begin) == 0)
		ret = PX_MAX_F64;
	else 
		ret = (physx::PxF64)atof(tmpStr);

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);

	return ret;
}

PX_INLINE void strToF32s(physx::PxF32 *v,physx::PxU32 count,const char *str, const char**endptr)
{
	const char *begin = skipWhiteSpace(str);

	if ( *begin == '(' ) begin++;
	for (physx::PxU32 i=0; i<count && *begin; i++)
	{
		v[i] = (physx::PxF32)strToF32(begin, &begin);
	}

	if( endptr )
		*endptr = skipNonWhiteSpace(begin);
}


//////////////////////////
// value to str functions
//////////////////////////
PX_INLINE const char * valueToStr( bool val, char *buf, physx::PxU32 n )
{
	physx::string::sprintf_s(buf, n,"%s",val ? "true" : "false");
	return buf;
}

PX_INLINE const char * valueToStr( physx::PxI8 val, char *buf, physx::PxU32 n )
{
	if( val == PX_MIN_I8 )
		physx::string::sprintf_s(buf, n,"%s","PX_MIN_I8" );
	else if( val == PX_MAX_I8 )
		physx::string::sprintf_s(buf, n,"%s","PX_MAX_I8" );
	else
		physx::string::sprintf_s(buf, n, "%d", val);
	return buf;
}

PX_INLINE const char * valueToStr( physx::PxI16 val, char *buf, physx::PxU32 n )
{
	if( val == PX_MIN_I16 )
		physx::string::sprintf_s(buf, n,"%s","PX_MIN_I16" );
	else if( val == PX_MAX_I16 )
		physx::string::sprintf_s(buf, n,"%s","PX_MAX_I16" );
	else
		physx::string::sprintf_s(buf, n,"%d",val );
	return buf;
}

PX_INLINE const char * valueToStr( physx::PxI32 val, char *buf, physx::PxU32 n )
{
	if( val == PX_MIN_I32 )
		physx::string::sprintf_s(buf, n,"%s","PX_MIN_I32" );
	else if( val == PX_MAX_I32 )
		physx::string::sprintf_s(buf, n,"%s","PX_MAX_I32" );
	else
		physx::string::sprintf_s(buf, n,"%d",val );
	return buf;
}

PX_INLINE const char * valueToStr( physx::PxI64 val, char *buf, physx::PxU32 n )
{
	physx::string::sprintf_s(buf, n,"%lld",val );
	return buf;
}

PX_INLINE const char * valueToStr( physx::PxU8 val, char *buf, physx::PxU32 n )
{
	physx::string::sprintf_s(buf, n, "%u", val);
	return buf;
}

PX_INLINE const char * valueToStr( physx::PxU16 val, char *buf, physx::PxU32 n )
{
	if( val == PX_MAX_U16 )
		physx::string::sprintf_s(buf, n,"%s","PX_MAX_U16" );
	else
		physx::string::sprintf_s(buf, n,"%u",val );
	return buf;
}

PX_INLINE const char * valueToStr( physx::PxU32 val, char *buf, physx::PxU32 n )
{
	if( val == PX_MAX_U32 )
		physx::string::sprintf_s(buf, n,"%s","PX_MAX_U32" );
	else
		physx::string::sprintf_s(buf, n,"%u",val );
	return buf;
}

PX_INLINE const char * valueToStr( physx::PxU64 val, char *buf, physx::PxU32 n )
{
	physx::string::sprintf_s(buf, n,"%llu",val );
	return buf;
}

PX_INLINE const char * valueToStr( physx::PxF32 val, char *buf, physx::PxU32 n )
{
	if( !physx::intrinsics::isFinite(val) )
	{
		PX_ASSERT( 0 && "invalid floating point" );
		physx::string::sprintf_s(buf, n,"%s","0" );
	}
	else if( val == -PX_MAX_F32 )
		physx::string::sprintf_s(buf, n,"%s","PX_MIN_F32" );
	else if( val == PX_MAX_F32 )
		physx::string::sprintf_s(buf, n,"%s","PX_MAX_F32" );
    else if ( val == 1 )
    	physx::string::strcpy_s(buf, n, "1");
    else if ( val == 0 )
    	physx::string::strcpy_s(buf, n, "0");
    else if ( val == - 1 )
    	physx::string::strcpy_s(buf, n, "-1");
    else
    {
		physx::string::sprintf_s(buf,n,"%.9g", (double)val ); // %g expects double
		const char *dot = strchr(buf,'.');
		const char *e = strchr(buf,'e');
		if ( dot && !e )
		{
			physx::PxI32 len = (physx::PxI32)strlen(buf);
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

PX_INLINE const char * valueToStr( physx::PxF64 val, char *buf, physx::PxU32 n )
{
	if( !physx::intrinsics::isFinite(val) )
	{
		PX_ASSERT( 0 && "invalid floating point" );
		physx::string::sprintf_s(buf, n,"%s","0" );
	}
	else if( val == -PX_MAX_F64 )
		physx::string::sprintf_s(buf, n,"%s","PX_MIN_F64" );
	else if( val == PX_MAX_F64 )
		physx::string::sprintf_s(buf, n,"%s","PX_MAX_F64" );
    else if ( val == 1 )
		physx::string::strcpy_s(buf, n, "1");
    else if ( val == 0 )
    	physx::string::strcpy_s(buf, n, "0");
    else if ( val == - 1 )
    	physx::string::strcpy_s(buf, n, "-1");
    else
    {
		physx::string::sprintf_s(buf,n,"%.18g", val );
		const char *dot = strchr(buf,'.');
		const char *e = strchr(buf,'e');
		if ( dot && !e )
		{
			physx::PxI32 len = (physx::PxI32)strlen(buf);
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
