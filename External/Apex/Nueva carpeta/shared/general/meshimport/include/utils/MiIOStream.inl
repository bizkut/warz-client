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
 * Copyright 2009-2011 NVIDIA Corporation.  All rights reserved.
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
\brief MiIOStream inline implementation
*/

MI_INLINE MiIOStream& MiIOStream::operator<<(bool v)
{
	if ( mBinary )
	{
		mStream.storeByte((MiI8)v);
	}
	else
	{
		char scratch[6];
		storeString( MiAsc::valueToStr(v, scratch, 6) );
	}
	return *this;
};


MI_INLINE MiIOStream& MiIOStream::operator<<(char c)
{
	mStream.storeByte((MiI8)c);
	return *this;
};

MI_INLINE MiIOStream& MiIOStream::operator<<(MiU8 c)
{
	if ( mBinary )
	{
		mStream.storeByte((MiU8)c);
	}
	else
	{
		char scratch[MiAsc::IntStrLen];
		storeString( MiAsc::valueToStr(c, scratch, MiAsc::IntStrLen) );
	}

	return *this;
};

MI_INLINE MiIOStream& MiIOStream::operator<<(MiI8 c)
{
	if ( mBinary )
	{
		mStream.storeByte((MiI8)c);
	}
	else
	{
		char scratch[MiAsc::IntStrLen];
		storeString( MiAsc::valueToStr(c, scratch, MiAsc::IntStrLen) );
	}

	return *this;
};

MI_INLINE MiIOStream& MiIOStream::operator<<(const char *c)
{
	if ( mBinary )
	{
		c = c ? c : ""; // it it is a null pointer, assign it to an empty string.
		MiU32 len = (MiU32)strlen(c);
		MI_ASSERT( len < (MAX_STREAM_STRING-1));
		if ( len > (MAX_STREAM_STRING-1) )
		{
			len = MAX_STREAM_STRING-1;
		}
		mStream.storeDword(len);
		if ( len )
			mStream.write(c,len);
	}
	else
	{
		storeString(c);
	}
	return *this;
};

MI_INLINE MiIOStream& MiIOStream::operator<<(MiU64 v)
{
	if ( mBinary )
	{
		mStream.storeDouble( (MiF64) v );
	}
	else
	{
		char scratch[MiAsc::IntStrLen];
		storeString( MiAsc::valueToStr(v, scratch, MiAsc::IntStrLen) );
	}
	return *this;
};

MI_INLINE MiIOStream& MiIOStream::operator<<(MiI64 v)
{
	if ( mBinary )
	{
		mStream.storeDouble( (MiF64) v );
	}
	else
	{
		char scratch[MiAsc::IntStrLen];
		storeString( MiAsc::valueToStr(v, scratch, MiAsc::IntStrLen) );
	}
	return *this;
};

MI_INLINE MiIOStream& MiIOStream::operator<<(MiF64 v)
{
	if ( mBinary )
	{
		mStream.storeDouble( (MiF64) v );
	}
	else
	{
		char scratch[MiAsc::MiF64StrLen];
		storeString( MiAsc::valueToStr(v, scratch, MiAsc::MiF64StrLen) );
	}
	return *this;
};

MI_INLINE MiIOStream& MiIOStream::operator<<(MiF32 v)
{
	if ( mBinary )
	{
		mStream.storeFloat(v);
	}
	else
	{
		char scratch[MiAsc::MiF32StrLen];
		storeString( MiAsc::valueToStr(v, scratch, MiAsc::MiF32StrLen) );

	}
	return *this;
};

MI_INLINE MiIOStream& MiIOStream::operator<<(MiU32 v)
{
	if ( mBinary )
	{
		mStream.storeDword(v);
	}
	else
	{
		char scratch[MiAsc::IntStrLen];
		storeString( MiAsc::valueToStr(v, scratch, MiAsc::IntStrLen) );
	}
	return *this;
};

MI_INLINE MiIOStream& MiIOStream::operator<<(MiI32 v)
{
	if ( mBinary )
	{
		mStream.storeDword( (MiU32) v );
	}
	else
	{
		char scratch[MiAsc::IntStrLen];
		storeString( MiAsc::valueToStr(v, scratch, MiAsc::IntStrLen) );
	}
	return *this;
};

MI_INLINE MiIOStream& MiIOStream::operator<<(MiU16 v)
{
	if ( mBinary )
	{
		mStream.storeWord(v);
	}
	else
	{
		char scratch[MiAsc::IntStrLen];
		storeString( MiAsc::valueToStr(v, scratch, MiAsc::IntStrLen) );
	}
	return *this;
};

MI_INLINE MiIOStream& MiIOStream::operator<<(MiI16 v)
{
	if ( mBinary )
	{
		mStream.storeWord( (MiU16) v );
	}
	else
	{
		char scratch[MiAsc::IntStrLen];
		storeString( MiAsc::valueToStr(v, scratch, MiAsc::IntStrLen) );
	}
	return *this;
};


MI_INLINE MiIOStream& MiIOStream::operator>>(MiU32 &v)
{
	if ( mBinary )
	{
		v = mStream.readDword();
	}
	return *this;
}

MI_INLINE MiIOStream& MiIOStream::operator>>(char &v)
{
	if ( mBinary )
	{
		v = mStream.readByte();
	}
	return *this;
}

MI_INLINE MiIOStream& MiIOStream::operator>>(MiU8 &v)
{
	if ( mBinary )
	{
		v = mStream.readByte();
	}
	return *this;
}

MI_INLINE MiIOStream& MiIOStream::operator>>(MiI8 &v)
{
	if ( mBinary )
	{
		v = mStream.readByte();
	}
	return *this;
}

MI_INLINE MiIOStream& MiIOStream::operator>>(MiI64 &v)
{
	if ( mBinary )
	{
		v = mStream.readDword();
	}
	return *this;
}

MI_INLINE MiIOStream& MiIOStream::operator>>(MiU64 &v)
{
	if ( mBinary )
	{
		v = (MiU64)mStream.readDouble();
	}
	return *this;
}

MI_INLINE MiIOStream& MiIOStream::operator>>(MiF64 &v)
{
	if ( mBinary )
	{
		v = mStream.readDouble();
	}
	return *this;
}

MI_INLINE MiIOStream& MiIOStream::operator>>(MiF32 &v)
{
	if ( mBinary )
	{
		v = mStream.readFloat();
	}
	return *this;
}

MI_INLINE MiIOStream& MiIOStream::operator>>(MiI32 &v)
{
	if ( mBinary )
	{
		v = mStream.readDword();
	}
	return *this;
}

MI_INLINE MiIOStream& MiIOStream::operator>>(MiU16 &v)
{
	if ( mBinary )
	{
		v = mStream.readWord();
	}
	return *this;
}

MI_INLINE MiIOStream& MiIOStream::operator>>(MiI16 &v)
{
	if ( mBinary )
	{
		v = mStream.readWord();
	}
	return *this;
}

MI_INLINE MiIOStream& MiIOStream::operator>>(bool &v)
{
	MiI8 iv;
	iv = mStream.readByte();
	v = iv ? true : false;
	return *this;
};

#define NX_IOSTREAM_COMMA_SEPARATOR if(!mBinary) *this << ' ';

MI_INLINE MiIOStream& MiIOStream::operator>>(const char *&str)
{
	str = NULL; // by default no string streamed...
	if ( mBinary )
	{
		MiU32 len=0;
		*this >> len;
		MI_ASSERT( len < (MAX_STREAM_STRING-1) );
		if ( len < (MAX_STREAM_STRING-1) )
		{
			mStream.read(mReadString,len);
			mReadString[len] = 0;
			str = mReadString;
		}
	}
	return *this;
}


MI_INLINE void  MiIOStream::storeString(const char *c,bool zeroTerminate)
{
	while ( *c )
	{
		mStream.storeByte((MiI8)*c);
		c++;
	}
	if ( zeroTerminate )
	{
		mStream.storeByte(0);
	}
}
