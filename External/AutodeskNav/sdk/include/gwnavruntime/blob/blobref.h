/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_BlobRef_H
#define Navigation_BlobRef_H


#include "gwnavruntime/base/endianness.h"
#include "gwnavruntime/base/memory.h"


namespace Kaim
{


class BlobRefImpl
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	// Constructs a new instance of this class. 
	KY_INLINE BlobRefImpl() : m_shallowBlobSize(0), m_offset(0) {}

	KY_INLINE void* Ptr() const
	{
		if (m_shallowBlobSize == 0)
			return KY_NULL;
		else
			return (void*)((char*)&m_offset + m_offset);
	}

public:
	KyUInt32 m_shallowBlobSize;
	KyInt32 m_offset;

private:
	BlobRefImpl& operator=(const BlobRefImpl& other);
	BlobRefImpl(const BlobRefImpl& other);
};



/// A BlobRef is a type of reference that is compatible with the blob serialization framework.
/// If the blobRef does not reference anything, Ptr() returns NULL.
/// To allow the visual studio debugger to display BlobRef content, read the instructions in gwnavruntime/containers/autoexp.h
template <class T>
class BlobRef
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	KY_INLINE BlobRef() {}

	KY_INLINE T* Ptr() const { return (T*)m_impl.Ptr(); }

	KY_INLINE KyUInt32 ShallowBlobSize() const { return m_impl.m_shallowBlobSize; }
public:
	BlobRefImpl m_impl;

private:
	BlobRef& operator=(const BlobRef& blobRef);
	BlobRef(const BlobRef& blobRef);
};


// --------------------------------- inline implementation ---------------------------------

template <class T>
void SwapEndianness(Endianness::Target e, BlobRef<T>& self)
{
	if (e == Endianness::SYSTEM_ENDIANNESS)
	{
		// use count and values once they have been swapped
		SwapEndianness(e, self.m_impl.m_shallowBlobSize);
		SwapEndianness(e, self.m_impl.m_offset);

		if (self.m_impl.m_shallowBlobSize != 0)
			SwapEndianness(e, *self.Ptr());
	}
	else
	{
		// use shallowBlobSize and value before they are swapped
		KyUInt32 shallowBlobSize = self.m_impl.m_shallowBlobSize;
		T* value = self.Ptr();

		SwapEndianness(e, self.m_impl.m_shallowBlobSize);
		SwapEndianness(e, self.m_impl.m_offset);
		
		if (shallowBlobSize != 0)
			SwapEndianness(e, *value);
	}
}


}


#endif

