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

#ifndef PX_FILE_BUFFER_H
#define PX_FILE_BUFFER_H

#include "Ps.h"
#include "PxFileBuf.h"
#include "PsFile.h"
#include "PsUserAllocated.h"

namespace physx
{
namespace general_PxIOStream2
{
	using namespace shdfnd;

//Use this class if you want to use your own allocator
class PxFileBufferBase : public PxFileBuf
{
public:
	PxFileBufferBase(const char *fileName,OpenMode mode)
	{
		mOpenMode = mode;
		mFph = NULL;
		mFileLength = 0;
		mSeekRead   = 0;
		mSeekWrite  = 0;
		mSeekCurrent = 0;
		switch ( mode )
		{
			case OPEN_READ_ONLY:
				physx::shdfnd::fopen_s(&mFph,fileName,"rb");
				break;
			case OPEN_WRITE_ONLY:
				physx::shdfnd::fopen_s(&mFph,fileName,"wb");
				break;
			case OPEN_READ_WRITE_NEW:
				physx::shdfnd::fopen_s(&mFph,fileName,"wb+");
				break;
			case OPEN_READ_WRITE_EXISTING:
				physx::shdfnd::fopen_s(&mFph,fileName,"rb+");
				break;
			case OPEN_FILE_NOT_FOUND:
				break;
		}
		if ( mFph )
		{
			fseek(mFph,0L,SEEK_END);
			mFileLength = ftell(mFph);
			fseek(mFph,0L,SEEK_SET);
		}
		else
		{
			mOpenMode = OPEN_FILE_NOT_FOUND;
		}
    }

	virtual						~PxFileBufferBase()
	{
		close();
	}

	virtual void close()
	{
		if( mFph )
		{
			fclose(mFph);
			mFph = 0;
		}
	}

	virtual SeekType isSeekable(void) const
	{
		return mSeekType;
	}

	virtual		PxU32			read(void* buffer, PxU32 size)	
	{
		PxU32 ret = 0;
		if ( mFph )
		{
			setSeekRead();
			ret = (PxU32)::fread(buffer,1,size,mFph);
			mSeekRead+=ret;
			mSeekCurrent+=ret;
		}
		return ret;
	}

	virtual		PxU32			peek(void* buffer, PxU32 size)
	{
		PxU32 ret = 0;
		if ( mFph )
		{
			PxU32 loc = tellRead();
			setSeekRead();
			ret = (PxU32)::fread(buffer,1,size,mFph);
			mSeekCurrent+=ret;
			seekRead(loc);
		}
		return ret;
	}

	virtual		PxU32		write(const void* buffer, PxU32 size)
	{
		PxU32 ret = 0;
		if ( mFph )
		{
			setSeekWrite();
			ret = (PxU32)::fwrite(buffer,1,size,mFph);
			mSeekWrite+=ret;
			mSeekCurrent+=ret;
			if ( mSeekWrite > mFileLength )
			{
				mFileLength = mSeekWrite;
			}
		}
		return ret;
	}

	virtual PxU32 tellRead(void) const
	{
		return mSeekRead;
	}

	virtual PxU32 tellWrite(void) const
	{
		return mSeekWrite;
	}

	virtual PxU32 seekRead(PxU32 loc) 
	{
		mSeekRead = loc;
		if ( mSeekRead > mFileLength )
		{
			mSeekRead = mFileLength;
		}
		return mSeekRead;
	}

	virtual PxU32 seekWrite(PxU32 loc)
	{
		mSeekWrite = loc;
		if ( mSeekWrite > mFileLength )
		{
			mSeekWrite = mFileLength;
		}
		return mSeekWrite;
	}

	virtual void flush(void)
	{
		if ( mFph )
		{
			::fflush(mFph);
		}
	}

	virtual OpenMode	getOpenMode(void) const
	{
		return mOpenMode;
	}

	virtual PxU32 getFileLength(void) const
	{
		return mFileLength;
	}

private:
	// Moves the actual file pointer to the current read location
	void setSeekRead(void) 
	{
		if ( mSeekRead != mSeekCurrent && mFph )
		{
			if ( mSeekRead >= mFileLength )
			{
				::fseek(mFph,0L,SEEK_END);
			}
			else
			{
				::fseek(mFph,mSeekRead,SEEK_SET);
			}
			mSeekCurrent = mSeekRead = ::ftell(mFph);
		}
	}
	// Moves the actual file pointer to the current write location
	void setSeekWrite(void)
	{
		if ( mSeekWrite != mSeekCurrent && mFph )
		{
			if ( mSeekWrite >= mFileLength )
			{
				::fseek(mFph,0L,SEEK_END);
			}
			else
			{
				::fseek(mFph,mSeekWrite,SEEK_SET);
			}
			mSeekCurrent = mSeekWrite = ::ftell(mFph);
		}
	}


	FILE		*mFph;
	PxU32		mSeekRead;
	PxU32		mSeekWrite;
	PxU32		mSeekCurrent;
	PxU32		mFileLength;
	SeekType	mSeekType;
	OpenMode	mOpenMode;
};

//Use this class if you want to use PhysX memory allocator
class PxFileBuffer: public PxFileBufferBase, public UserAllocated
{
public:
	PxFileBuffer(const char *fileName,OpenMode mode): PxFileBufferBase(fileName, mode) {}
};

}
using namespace general_PxIOStream2;
}

#endif // PX_FILE_BUFFER_H
