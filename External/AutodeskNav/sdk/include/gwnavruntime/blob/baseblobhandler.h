/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_IBlobHandler_H
#define Navigation_IBlobHandler_H


#include "gwnavruntime/blob/iblobtypedescriptor.h"


namespace Kaim
{


class BaseBlobHandler : public IBlobTypeDescriptor
{
	KY_CLASS_WITHOUT_COPY(BaseBlobHandler)

public:
	BaseBlobHandler();
	virtual ~BaseBlobHandler();

	void Clear();
	void ClearAndRelease();

	void InitAsWeakReference(void* blob, KyUInt32 deepBlobSize, KyUInt32 shallowBlobSize = 0);
	void* InitAsStrongReference(KyUInt32 deepBlobSize, KyUInt32 shallowBlobSize, MemoryHeap* heap, KyInt32 memStat);

	const void* VoidBlob() const       { return m_blob; }
	void*       VoidBlob()             { return m_blob; }
	void*       GetMutableBlob() const { return m_blob; }

	// Retrieves the deep size (including BlobRefs) of the blob maintained by this handler. 
	KyUInt32 BlobSize() const { return m_deepBlobSize; }

	// Retrieves the deep size (including BlobRefs) of the blob maintained by this handler. 
	KyUInt32 GetDeepBlobSize()    const { return m_deepBlobSize; }
	KyUInt32 GetShallowBlobSize() const { return m_shallowBlobSize; }

	void SwapEndianess(Endianness::Target e) { DoSwapEndianness(e, m_blob); }

public:
	void* m_blob;                //< The blob handled by this object. 
	KyUInt32 m_deepBlobSize;     //< The size of #m_blob in memory including BlobRefs. 
	KyUInt32 m_shallowBlobSize;  //< The size of #m_blob in memory excluding BlobRefs. 

	void* m_buffer;
	KyUInt32 m_bufferSize;
};


}


#endif

