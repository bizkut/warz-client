//=========================================================================
//	Module: PhysXRepXHelpers.hpp
//	Copyright (C) 2011.
//=========================================================================

#pragma once
#include "r3dPCH.h"
#include "r3d.h"

//////////////////////////////////////////////////////////////////////////

#include "RepX\RepX.h"
#include "RepX\RepXUtility.h"
#include "extensions\PxStringTableExt.h"
//#include "PhysX\PxFoundation\internal\PxIOStream\public\PxFileBuf.h"

#include "PhysXRepXHelpers.h"

//////////////////////////////////////////////////////////////////////////

class MyPhysXFileBuf_ReadOnly : public PxInputData
{
private:
	r3dFile* f;
public:
	MyPhysXFileBuf_ReadOnly(const char* fname)
	{
		f = r3d_open(fname, "rb");
		r3d_assert(f);
	}

	virtual ~MyPhysXFileBuf_ReadOnly(void) {
		fclose(f);
	}

	virtual PxU32	getLength(void) const { return f->size; }
	virtual void	seek(PxU32 loc) { fseek(f, loc, SEEK_SET); }
	virtual PxU32	read(void *mem, PxU32 len) { return fread(mem, 1, len, f); }
	virtual PxU32	tell(void) const { return ftell(f); }
};

//////////////////////////////////////////////////////////////////////////

physx::repx::RepXCollection* loadCollection(const char* inPath, PxAllocatorCallback& inCallback)
{
	physx::repx::RepXExtension* theExtensions[64];
	PxU32 numExtensions = buildExtensionList( theExtensions, 64, inCallback );

	MyPhysXFileBuf_ReadOnly fileBuf(inPath);
	physx::repx::RepXCollection* retval = physx::repx::RepXCollection::create( fileBuf, theExtensions, numExtensions, inCallback );
	if ( retval )
		retval = &physx::repx::RepXUpgrader::upgradeCollection( *retval );
	return retval;
}

//////////////////////////////////////////////////////////////////////////

PhysxUserFileReadStream::PhysxUserFileReadStream(const char* filename) : 
fpr(NULL)
{
	fpr = r3d_open(filename, "rb");
}

//////////////////////////////////////////////////////////////////////////

PhysxUserFileReadStream::~PhysxUserFileReadStream()
{
	if(fpr)  fclose(fpr);
}

//////////////////////////////////////////////////////////////////////////

PxU32 PhysxUserFileReadStream::read(void* buffer, PxU32 size)
{
	size_t w = fread(buffer, size, 1, fpr);
	PX_ASSERT(w);
	return w;
}

//-------------------------------------------------------------------------
//	PhysxUserMemoryReadStream
//-------------------------------------------------------------------------

PhysxUserMemoryWriteStream::PhysxUserMemoryWriteStream()
: mData(0)
, mSize(0)
, mCapacity(0)
{

}

//////////////////////////////////////////////////////////////////////////

PhysxUserMemoryWriteStream::~PhysxUserMemoryWriteStream()
{
	delete[] mData;
}

//////////////////////////////////////////////////////////////////////////

PxU32 PhysxUserMemoryWriteStream::write(const void* src, PxU32 size)
{
	PxU32 expectedSize = mSize + size;
	if(expectedSize > mCapacity)
	{
		mCapacity = expectedSize + 4096;

		PxU8* newData = game_new PxU8[mCapacity];
		PX_ASSERT(newData!=NULL);

		if(newData)
		{
			memcpy(newData, mData, mSize);
			delete[] mData;
		}
		mData = newData;
	}
	memcpy(mData+mSize, src, size);
	mSize += size;
	return size;
}

//-------------------------------------------------------------------------
//	PhysxUserFileWriteStream
//-------------------------------------------------------------------------

PhysxUserFileWriteStream::PhysxUserFileWriteStream(const char *fileName)
: fpw(0)
{
	fpw = fopen_for_write(fileName, "wb");
}

//////////////////////////////////////////////////////////////////////////

PhysxUserFileWriteStream::~PhysxUserFileWriteStream()
{
	if (fpw)
		fclose(fpw);
}

//////////////////////////////////////////////////////////////////////////

PxU32 PhysxUserFileWriteStream::write(const void* src, PxU32 count)
{
	PxU32 cnt = fwrite(src, count, 1, fpw);
	return cnt;
}

//-------------------------------------------------------------------------
//	PhysxUserMemoryWriteStream
//-------------------------------------------------------------------------

PhysxUserMemoryReadStream::PhysxUserMemoryReadStream(PxU8* data, PxU32 length)
: mSize(length)
, mData(data)
, mPos(0)
{
}

//////////////////////////////////////////////////////////////////////////

PxU32 PhysxUserMemoryReadStream::read(void* dest, PxU32 count)
{
	PxU32 length = PxMin<PxU32>(count, mSize - mPos);
	memcpy(dest, mData + mPos, length);
	mPos += length;
	return length;
}

//////////////////////////////////////////////////////////////////////////

PxU32 PhysxUserMemoryReadStream::getLength() const
{
	return mSize;
}

//////////////////////////////////////////////////////////////////////////

void PhysxUserMemoryReadStream::seek(PxU32 offset)
{
	mPos = PxMin<PxU32>(mSize, offset);
}

//////////////////////////////////////////////////////////////////////////

PxU32 PhysxUserMemoryReadStream::tell() const
{
	return mPos;
}
