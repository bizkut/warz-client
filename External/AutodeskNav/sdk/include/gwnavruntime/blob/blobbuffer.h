/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_BlobBuffer_H
#define Navigation_BlobBuffer_H


#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/blob/blobref.h"
#include "gwnavruntime/blob/blobhandler.h"
#include "gwnavruntime/kernel/SF_Debug.h"

namespace Kaim
{


class BlobRefInfo
{
public:
	BlobRefInfo() { Clear(); }

	void Clear() { Set(KY_NULL, KY_NULL, 0); }

	void Set(KyUInt32* shallowBlobSizePtr, KyInt32* offsetPtr, KyInt32 blobGlobalOffset)
	{
		m_shallowBlobSizePtr = shallowBlobSizePtr;
		m_offsetPtr          = offsetPtr;
		m_blobGlobalOffset   = blobGlobalOffset;
	}

	bool IsValid() { return m_shallowBlobSizePtr != KY_NULL; }

public:
	KyUInt32* m_shallowBlobSizePtr;
	KyInt32* m_offsetPtr;
	KyInt32 m_blobGlobalOffset;
};


/*
BlobBuffer is used in conjunction with derivations of BaseBlobBuilder to build Blobs.
A given succession of calls to Alloc...() functions must be called twice,
once in COUNT mode (the default), once in WRITE mode (set by a call to SwitchToWriteMode()).
In COUNT mode (IsWriteMode() == false): Alloc(), AllocArray(), AllocAndCopyArray() do not write to m_buffer memory,
they just increment the size needed to fit the Blob.
In WRITE mode (IsWriteMode() == true ): Alloc(), AllocArray(), AllocAndCopyArray() do write to m_buffer memory.
You should not use BlobBuffer directly, use a derivation of BaseBlobBuilder instead
and its accompanying macros: BLOB_SET, BLOB_ARRAY, BLOB_ARRAY_COPY, BLOB_STRING, BLOB_BUILD.
*/
class BlobBuffer
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	BlobBuffer() : m_offset(0), m_buffer(KY_NULL) {}

	/* returns true when BlobBuffer is in write mode */
	bool IsWriteMode() { return m_buffer != KY_NULL; }

	/*	In COUNT mode: increment the size by sizeof(T).
		In WRITE mode: call the T default constructor on the current position in buffer.*/
	template<class T> T* Alloc();

	/*	In COUNT mode: increment the size by sizeof(T) * count.
		In WRITE mode: call count times the T default constructor on the current position in buffer. */
	template<class T> T* AllocArray(BlobArray<T>* blobArray, KyUInt32 count)
	{
		return AllocAndCopyArray<T>(blobArray, KY_NULL, count);
	}

	/*	In COUNT mode: increment the size by sizeof(T) * count.
		In WRITE mode: call count times the T default constructor on the current position in buffer. */
	template<class T> T* AllocAndCopyArray(BlobArray<T>* blobArray, const T* src, KyUInt32 count);

	template<class T> T* AllocAndCopyReferencedBlob(BlobRef<T>* blobRef, void* srcBlob, KyUInt32 srcBlobDeepSize, KyUInt32 srcBlobShallowSize);

	template<class T> T* AllocAndCopyReferencedBlobFromBlobHandler(BlobRef<T>* blobRef, const BlobHandler<T>& blobHandler);

	/* called in BaseBlobBuilder<T>::Build() */
	void SwitchToWriteMode(BaseBlobHandler& baseBlobHandler, KyUInt32 rootShallowBlobSize, MemoryHeap* heap, KyInt32 memStat);

	KyUInt32 GetAlignedSize(KyUInt32 size) { return ((size + 4 - 1) / 4) * 4; }

	template<class T> void BeginBlobRefBuffer(BlobRef<T>* blobRef);

	void SetBlobRefInfoFromCurrentOffset();

	void SetBlobRefInfoFromCopiedBlobRef(KyUInt32 shallowBlobSize);

public:
	KyUInt32 m_offset;
	char* m_buffer;
	BlobRefInfo m_blobRefInfo;
};


// --------------------------------- inline implementation ---------------------------------

template<class T>
T* BlobBuffer::Alloc()
{
	KY_ASSERT(sizeof(T) % 4 == 0);

	if (IsWriteMode() == false)
	{
		m_offset += sizeof(T);
		return KY_NULL;
	}

	T* ptr = (T*)(m_buffer + m_offset);
	::new(ptr) T(); // placement new
	m_offset += sizeof(T);
	return ptr;
}


template<class T>
T* BlobBuffer::AllocAndCopyArray(BlobArray<T>* blobArray, const T* src, KyUInt32 count)
{
	KyUInt32 size = sizeof(T) * count;
	KyUInt32 alignedSize = GetAlignedSize(size);
	KyUInt32 paddingSize = alignedSize - size;

	if (IsWriteMode() == false)
	{
		m_offset += alignedSize;
		return KY_NULL;
	}

	if (count == 0)
	{
		blobArray->m_count = count;
		blobArray->m_offset = 0; // force m_offset = 0 when count = 0
		return KY_NULL;
	}

	// get destArray
	T* dest = (T*)(m_buffer + m_offset);

	// initialize blobArray
	blobArray->m_count = count;
	blobArray->m_offset = (KyInt32)((char*)dest - (char*)&blobArray->m_offset);

	if (src != KY_NULL)
	{
		memcpy(dest, src, size);
	}
	else
	{
		memset(dest, 0, size);
		// TODO check if this is necessary
		// call default constructor on each element
		for (KyUInt32 i = 0; i < count; ++i)
			::new(dest + i) T;
	}
	for (KyUInt32 i = 0; i < paddingSize; ++i)
		((char*)dest)[size + i] = 0;

	m_offset += alignedSize;
	return dest;
}


template<class T>
void BlobBuffer::BeginBlobRefBuffer(BlobRef<T>* blobRef)
{
	if (blobRef == KY_NULL)
		return;
	SetBlobRefInfoFromCurrentOffset(); // set the previous BlobRefInfo
	m_blobRefInfo.Set(&blobRef->m_impl.m_shallowBlobSize, &blobRef->m_impl.m_offset, m_offset);
}


template<class T>
T* BlobBuffer::AllocAndCopyReferencedBlob(BlobRef<T>* blobRef, void* srcBlob, KyUInt32 srcBlobDeepSize, KyUInt32 srcBlobShallowSize)
{
	BeginBlobRefBuffer(blobRef);

	KyUInt32 alignedSize = GetAlignedSize(srcBlobDeepSize);
	KyUInt32 paddingSize = alignedSize - srcBlobDeepSize;

	if (IsWriteMode() == false || srcBlob == KY_NULL || blobRef == KY_NULL)
	{
		m_offset += alignedSize;
		SetBlobRefInfoFromCopiedBlobRef(srcBlobShallowSize);
		return KY_NULL;
	}

	char* dest = m_buffer + m_offset;
	memcpy(dest, srcBlob, srcBlobDeepSize);

	// nullify paddingSize
	for (KyUInt32 i = 0; i < paddingSize; ++i)
		((char*)dest)[srcBlobDeepSize + i] = 0;

	m_offset += alignedSize;
	SetBlobRefInfoFromCopiedBlobRef(srcBlobShallowSize);
	return (T*)dest;
}


template<class T>
T* BlobBuffer::AllocAndCopyReferencedBlobFromBlobHandler(BlobRef<T>* blobRef, const BlobHandler<T>& blobHandler)
{
	return AllocAndCopyReferencedBlob(blobRef, (void*)blobHandler.VoidBlob(), blobHandler.GetDeepBlobSize(), blobHandler.GetShallowBlobSize());
}



}


#endif
