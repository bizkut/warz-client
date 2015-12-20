//=========================================================================
//	Module: ApexFileStream.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once

#if APEX_ENABLED

#include "PxFileBuf.h"
#include "r3dFileMan.h"

//////////////////////////////////////////////////////////////////////////

using physx::PxU32;
using namespace physx::general_PxIOStream2;

//////////////////////////////////////////////////////////////////////////

class ApexFileStream: public PxFileBuf
{
	r3dFile *f;
	PxFileBuf::OpenMode openMode;

public:
	ApexFileStream(const char *name, PxFileBuf::OpenMode om);
	~ApexFileStream();
	virtual OpenMode getOpenMode() const { return openMode; }
	virtual PxU32 getFileLength(void) const;
	virtual PxU32 seekRead(PxU32 loc);
	virtual PxU32 seekWrite(PxU32 loc);
	virtual PxU32 read(void *mem, PxU32 len);
	virtual PxU32 peek(void *mem, PxU32 len);
	virtual PxU32 write(const void *mem, PxU32 len);
	virtual PxU32 tellRead() const;
	virtual PxU32 tellWrite() const;
	virtual	void flush();
	virtual void close();
	virtual SeekType isSeekable() const { return PxFileBuf::SEEKABLE_READ; }

};

#endif