/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_BlobFileHeader_H
#define Navigation_BlobFileHeader_H


#include "gwnavruntime/base/endianness.h"
#include "gwnavruntime/base/memory.h"


namespace Kaim
{

class BaseBlobHandler;

class BlobFileHeader
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	static const char* MagicString() { return "BlobFile"; }

	BlobFileHeader();

	/* Sets the whole structure to zeros */
	void Clear();

	/* Copy memoryBuffer to this and fix the endianness if needed.
		Typically called when loading a Blob from a buffer or a file. */
	KyResult InitFromBuffer(void* memoryBuffer, bool& isEndiannessSwap);

	/* Typically called when saving a Blob to a buffer or a file */
	void Init(KyUInt32 blobTypeId, KyUInt32 blobTypeVersion, KyUInt32 deepBlobSize, KyUInt32 shallowBlobSize);

	void Init(const BaseBlobHandler& blobHandler);

	/* GetEndianness() works even if the endianness of the structure is swapped */
	Endianness::Type GetEndianness() const;

	/* Swap endianness in place.
		After SwapEndianness() is called, all subsequent calls to BlobFileHeader are invalid !
		Swaps also m_endianness from little to big or big to little */
	void SwapEndianness();

public:
	char m_magicString[12]; // BlobFile0000
	KyUInt32 m_endianness;
	KyUInt32 m_blobTypeId;
	KyUInt32 m_blobTypeVersion;
	KyUInt32 m_deepBlobSize;
	KyUInt32 m_shallowBlobSize;
};


}


#endif
