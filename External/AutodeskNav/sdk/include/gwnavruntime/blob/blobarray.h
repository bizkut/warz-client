/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_BlobArray_H
#define Navigation_BlobArray_H


#include "gwnavruntime/base/endianness.h"
#include "gwnavruntime/base/memory.h"


namespace Kaim
{

/// A BlobArray an array that is compatible with the blob serialization framework. It is the
/// only means to store a variable array of objects in a serializable blob.
/// \tparam T    The type of object this array will hold. This type of array may not contain any pointers.
///               It must contain only base types, other blob classes, or other BlobArray objects. Each type 
///               of object stored in a BlobArray must have its own SwapEndianness() implementation.
/// \section blobarraydebug Debugging BlobArrays
/// To allow the visual studio debugger to display BlobRef content, read the instructions in gwnavruntime/containers/autoexp.h
template <class T>
class BlobArray
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	typedef T ValueType;

	// ---------------------------------- Public Member Functions ----------------------------------

	BlobArray() : m_count(0), m_offset(0) {}

	/// Retrieves a const pointer to the data stored in this array. 
	KY_INLINE const T* GetValues() const { return (const T*)((char*)&m_offset + m_offset); }

	/// Retrieves a pointer to the data stored in this array. 
	KY_INLINE T* GetValues() { return (T*)((char*)&m_offset + m_offset); }

	KY_INLINE KyUInt32 GetCount() const { return m_count; }

public: // internal
	KyUInt32 m_count; ///< The number of elements in this BlobArray. Set by #BLOB_ARRAY and #BLOB_ARRAY_COPY during BaseBlobBuilder::DoBuild(). Do not modify. 
	KyInt32 m_offset; ///< Set by #BLOB_ARRAY and #BLOB_ARRAY_COPY during BaseBlobBuilder::DoBuild(). Do not modify. 
};



// --------------------------------- inline implementation ---------------------------------

template <class T>
void SwapEndianness(Endianness::Target e, BlobArray<T>& self)
{
	if (e == Endianness::SYSTEM_ENDIANNESS)
	{
		// use count and values once they have been swapped
		SwapEndianness(e, self.m_count);
		SwapEndianness(e, self.m_offset);

		T* values = self.GetValues();
		for (KyUInt32 i = 0; i < self.m_count; ++i)
			SwapEndianness(e, values[i]);
	}
	else
	{
		// use count and values before they are swapped
		KyUInt32 count = self.m_count;
		T* values = self.GetValues();

		SwapEndianness(e, self.m_count);
		SwapEndianness(e, self.m_offset);
		
		for (KyUInt32 i = 0; i < count; ++i)
			SwapEndianness(e, values[i]);
	}
}


// Helper template function to initialiaze Array<> from BlobArray<>
// Each element of the Array<> is initialized with operator=(blobElement)
template <class ArrayT, class BlobArrayT>
void InitArrayFromBlobArray_Assign(ArrayT& kyArray, const BlobArrayT& blobArray)
{
	const typename BlobArrayT::ValueType* blobValues = blobArray.GetValues();
	KyUInt32 blobValuesCount                = blobArray.GetCount();
	kyArray.Clear();
	kyArray.Resize(blobValuesCount);
	for (KyUInt32 i = 0; i < blobValuesCount; ++i)
		kyArray[i] = blobValues[i];
}

// Helper template function to initialiaze Array<> from BlobArray<>
// Each element of the Array<> is initialized with InitFromBlob(blobElement)
template <class ArrayT, class BlobArrayT>
void InitArrayFromBlobArray_InitFromBlob(ArrayT& kyArray, const BlobArrayT& blobArray)
{
	const typename BlobArrayT::ValueType* blobValues = blobArray.GetValues();
	KyUInt32 blobValuesCount                = blobArray.GetCount();
	kyArray.Clear();
	kyArray.Resize(blobValuesCount);
	for (KyUInt32 i = 0; i < blobValuesCount; ++i)
		kyArray[i].InitFromBlob(blobValues[i]);
}

// Helper template function to initialize Array<String> from BlobArray<BlobArray<char> >
template <class ArrayT, class BlobArrayT>
void InitArrayFromBlobArray_String(ArrayT& kyArray, const BlobArrayT& blobArray)
{
	const typename BlobArrayT::ValueType* blobValues = blobArray.GetValues();
	KyUInt32 blobValuesCount                = blobArray.GetCount();
	kyArray.Clear();
	kyArray.Resize(blobValuesCount);
	for (KyUInt32 i = 0; i < blobValuesCount; ++i)
		kyArray[i] = blobValues[i].GetValues();
}


}

#endif
