//=========================================================================
//	Module: ApexFileStream.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"
#if APEX_ENABLED

#include "ApexFileStream.h"

//////////////////////////////////////////////////////////////////////////

namespace
{
	const char * OpenModeConversionTable[] =
	{
		"",
		"r",
		"w",
		"w+",
		"a+"
	};
} // unnamed namespace

//////////////////////////////////////////////////////////////////////////


ApexFileStream::ApexFileStream(const char *name, physx::general_PxIOStream2::PxFileBuf::OpenMode om)
: f(r3d_open(name, OpenModeConversionTable[om]))
, openMode(om)
{

}

//////////////////////////////////////////////////////////////////////////

ApexFileStream::~ApexFileStream()
{

}

//////////////////////////////////////////////////////////////////////////

PxU32 ApexFileStream::seekRead(PxU32 loc)
{
	if (!f) return 0;
	fseek(f, static_cast<long>(loc), SEEK_SET);
	return static_cast<PxU32>(ftell(f));
}

//////////////////////////////////////////////////////////////////////////

PxU32 ApexFileStream::seekWrite(PxU32 loc)
{
	return seekRead(loc);
}

//////////////////////////////////////////////////////////////////////////

PxU32 ApexFileStream::read(void *mem, PxU32 len)
{
	if (!f) return 0;
	PxU32 rv = static_cast<PxU32>(fread(mem, len, 1, f));
	return rv * len;
}

//////////////////////////////////////////////////////////////////////////

PxU32 ApexFileStream::peek(void *mem, PxU32 len)
{
	if (!f) return 0;
	long loc = ftell(f);
	PxU32 rv = static_cast<PxU32>(fread(mem, len, 1, f));
	fseek(f, loc, SEEK_SET);
	return rv * len;
}

//////////////////////////////////////////////////////////////////////////

PxU32 ApexFileStream::write(const void *mem, PxU32 len)
{
	r3d_assert(!"Writing is not supported");
	return 0;
}

//////////////////////////////////////////////////////////////////////////

PxU32 ApexFileStream::tellRead() const
{
	return tellWrite();
}

//////////////////////////////////////////////////////////////////////////

PxU32 ApexFileStream::tellWrite() const
{
	if (!f) return 0;
	PxU32 rv = static_cast<PxU32>(ftell(f));
	return rv;
}

//////////////////////////////////////////////////////////////////////////

void ApexFileStream::flush()
{
	// Do nothing
}

//////////////////////////////////////////////////////////////////////////

void ApexFileStream::close()
{
	if (!f) return;
	fclose(f);
}

//////////////////////////////////////////////////////////////////////////

PxU32 ApexFileStream::getFileLength() const
{
	if (!f) return 0;
	return static_cast<PxU32>(f->size);
}

#endif