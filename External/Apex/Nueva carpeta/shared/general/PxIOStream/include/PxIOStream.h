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

#ifndef PX_IOSTREAM_H
#define PX_IOSTREAM_H

/*!
\file
\brief PxIOStream class
*/
#include "Ps.h"
#include "PxFileBuf.h"
#include "foundation/PxVec3.h"
#include "foundation/PxQuat.h"
#include <PxMat33Legacy.h>
#include <PxMat34Legacy.h>
#include "foundation/PxBounds3.h"
#include <string.h>
#include <stdlib.h>
#include "PsString.h"
#include "PxAsciiConversion.h"

#define safePrintf physx::string::sprintf_s

PX_PUSH_PACK_DEFAULT

namespace physx
{
	namespace general_PxIOStream2
	{

/**
\brief A wrapper class for physx::PxFileBuf that provides both binary and ASCII streaming capabilities
*/
class PxIOStream
{
	static const physx::PxU32 MAX_STREAM_STRING = 1024;
public:
	/**
	\param [in] stream the physx::PxFileBuf through which all reads and writes will be performed
	\param [in] streamLen the length of the input data stream when de-serializing
	*/
	PxIOStream(physx::PxFileBuf &stream,physx::PxU32 streamLen) : mBinary(true), mStreamLen(streamLen), mStream(stream) { };
	~PxIOStream(void) { };

	/**
	\brief Set the stream to binary or ASCII

	\param [in] state if true, stream is binary, if false, stream is ASCII

	If the stream is binary, stream access is passed straight through to the respecitve 
	physx::PxFileBuf methods.  If the stream is ASCII, all stream reads and writes are converted to
	human readable ASCII.
	*/
	PX_INLINE void setBinary(bool state) { mBinary = state; }
	PX_INLINE bool getBinary() { return mBinary; }

	PX_INLINE PxIOStream& operator<<(bool v);
	PX_INLINE PxIOStream& operator<<(char c);
	PX_INLINE PxIOStream& operator<<(physx::PxU8 v);
	PX_INLINE PxIOStream& operator<<(physx::PxI8 v);

	PX_INLINE PxIOStream& operator<<(const char *c);
	PX_INLINE PxIOStream& operator<<(physx::PxI64 v);
	PX_INLINE PxIOStream& operator<<(physx::PxU64 v);
	PX_INLINE PxIOStream& operator<<(physx::PxF64 v);
	PX_INLINE PxIOStream& operator<<(physx::PxF32 v);
	PX_INLINE PxIOStream& operator<<(physx::PxU32 v);
	PX_INLINE PxIOStream& operator<<(physx::PxI32 v);
	PX_INLINE PxIOStream& operator<<(physx::PxU16 v);
	PX_INLINE PxIOStream& operator<<(physx::PxI16 v);
	PX_INLINE PxIOStream& operator<<(const physx::PxVec3 &v);
	PX_INLINE PxIOStream& operator<<(const physx::PxQuat &v);
	PX_INLINE PxIOStream& operator<<(const physx::PxMat33Legacy &v);
	PX_INLINE PxIOStream& operator<<(const physx::PxMat34Legacy &v);
	PX_INLINE PxIOStream& operator<<(const physx::PxBounds3 &v);

	PX_INLINE PxIOStream& operator>>(const char *&c);
	PX_INLINE PxIOStream& operator>>(bool &v);
	PX_INLINE PxIOStream& operator>>(char &c);
	PX_INLINE PxIOStream& operator>>(physx::PxU8 &v);
	PX_INLINE PxIOStream& operator>>(physx::PxI8 &v);
	PX_INLINE PxIOStream& operator>>(physx::PxI64 &v);
	PX_INLINE PxIOStream& operator>>(physx::PxU64 &v);
	PX_INLINE PxIOStream& operator>>(physx::PxF64 &v);
	PX_INLINE PxIOStream& operator>>(physx::PxF32 &v);
	PX_INLINE PxIOStream& operator>>(physx::PxU32 &v);
	PX_INLINE PxIOStream& operator>>(physx::PxI32 &v);
	PX_INLINE PxIOStream& operator>>(physx::PxU16 &v);
	PX_INLINE PxIOStream& operator>>(physx::PxI16 &v);
	PX_INLINE PxIOStream& operator>>(physx::PxVec3 &v);
	PX_INLINE PxIOStream& operator>>(physx::PxQuat &v);
	PX_INLINE PxIOStream& operator>>(physx::PxMat33Legacy &v);
	PX_INLINE PxIOStream& operator>>(physx::PxMat34Legacy &v);
	PX_INLINE PxIOStream& operator>>(physx::PxBounds3 &v);

	physx::PxU32 getStreamLen(void) const { return mStreamLen; }

	physx::PxFileBuf& getStream(void) { return mStream; }

	PX_INLINE void storeString(const char *c,bool zeroTerminate=false);

private:
	PxIOStream& operator=( const PxIOStream& );


	bool      mBinary; // true if we are serializing binary data.  Otherwise, everything is assumed converted to ASCII
	physx::PxU32     mStreamLen; // the length of the input data stream when de-serializing.
	physx::PxFileBuf &mStream;
	char			mReadString[MAX_STREAM_STRING]; // a temp buffer for streaming strings on input.
};

#include "PxIOStream.inl" // inline methods...

	}; // end of namespace
	using namespace general_PxIOStream2;
}; // end of physx namespace

PX_POP_PACK

#endif // PX_IOSTREAM_H
