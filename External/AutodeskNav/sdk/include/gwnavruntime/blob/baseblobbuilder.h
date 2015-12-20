/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_BaseBlobBuilder_H
#define Navigation_BaseBlobBuilder_H


#include "gwnavruntime/blob/blobbuffer.h"
#include "gwnavruntime/kernel/SF_Std.h"
#include <assert.h>


namespace Kaim
{


/// BaseBlobBuilder is an abstract base class that builds a blob within a contiguous block of memory.
/// \tparam T			The name of the blob class that this object will handle.
/// This class cannot be used directly. If you are building a blob for a class that is supplied with the Gameware Navigation
/// SDK, use the derived BaseBlobBuilder class that corresponds to the blob class you want to build. If you want
/// to use the blob framework to serialize a custom class, derive your own custom class from 
/// BaseBlobBuilder<YourBlobClassName>, and implement DoBuild().
template<class T>
class BaseBlobBuilder : public NewOverrideBase<MemStat_BlobBuilder>
{
public:
	typedef T BlobType;

	BaseBlobBuilder(KyInt32 memStat = Stat_Default_Mem, MemoryHeap* heap = KY_NULL)
		: m_blobBuffer(KY_NULL)
		, m_blob(KY_NULL)
		, m_buildingPart(BUILDING_SHALLOW_PART)
		, m_shallowBlobSize(0)
		, m_memStat(memStat)
		, m_heap(heap)
	{}

	/// Indicates whether the builder is operating in COUNT mode or in WRITE mode. This can be used in 
	/// implementations of DoBuild(). 
	bool IsWriteMode() { return m_blobBuffer->IsWriteMode(); }

	/// This method:
	/// -#	Calls DoBuild() in COUNT mode to determine the amount of memory needed for the blob to be built. 
	/// -#	Allocates the amount of memory in the specified BlobHandler.
	/// -#	Calls DoBuild() again, in WRITE mode, to actually build the blob in the buffer allocated by the BlobHandler. 
	T* Build(BlobHandler<T>& blobHandler);

	/// Simple way to use DoBuild in case of flat blob (that is, a blob that does not have a BlobArray or BlobRef) 
	void BuildFlatBlob(T& blob);

	virtual ~BaseBlobBuilder() {}

private:
	/// Implement this function in any class that derives from BaseBlobBuilder. Its responsibility is to
	/// set the data for each class member maintained by #m_blob. In your implementation, use one of the 
	/// macros sets supplied for this purpose:
	/// -	#BLOB_SET, sets the value of a single class member of #m_blob.
	/// -	#BLOB_ARRAY and #BLOB_ARRAY_COPY, sets up BlobArray members of #m_blob.
	/// -	#BLOB_STRING, copies a string into a char* class member of #m_blob.
	/// -	#BLOB_BUILD, recursively builds class members of #m_blob that
	/// 	need to be built by their own BaseBlobBuilder class. 
	virtual void DoBuild() = 0;

public:
	/// For internal use. Use #BLOB_BUILD instead. 
	void DoBuildAllocatedBlob(BlobBuffer* blobBuffer, T* blob);

	/// For internal use. Use #BUILD_REFERENCED_BLOB instead. 
	void DoAllocAndBuildReferencedBlob(BlobBuffer* blobBuffer, BlobRef<T>* blobRef);

	/// For internal use. Check if m_buildingPart == BUILDING_SHALLOW_PART and return this. 
	BlobBuffer* GetBlobBufferToBuildThis()
	{
		const bool isBuildingShallowPart = (m_buildingPart == BUILDING_SHALLOW_PART);

		KY_DEBUG_ASSERTN(isBuildingShallowPart,
			("In BaseBlobBuilder::DoBuild(), BUILD_REFERENCED_BLOB must be called after all calls to BLOB_ARRAY, BLOB_ARRAY_COPY, BLOB_STRING, BLOB_BUILD"));

		return (isBuildingShallowPart) ? m_blobBuffer : KY_NULL;
	}

	BlobBuffer* GetBlobBufferToBuildReference()
	{
		EndShallowPart();
		m_buildingPart = BUILDING_DEEP_PART; 
		return m_blobBuffer;
	}

	void EndShallowPart()
	{
		if (m_buildingPart == BUILDING_SHALLOW_PART)
		{
			assert(m_shallowBlobSize == 0); // should not be called twice
			m_shallowBlobSize = m_blobBuffer->m_offset;
		}
		else // (m_buildingPart == BUILDING_DEEP_PART)
		{
			m_blobBuffer->SetBlobRefInfoFromCurrentOffset();
		}
	}

public:
	/// For internal use. Use #BLOB_SET and #BLOB_ARRAY instead. 
	BlobBuffer* m_blobBuffer;

	/// The blob maintained by this builder. Only modify using the macros listed under DoBuild(). 
	T* m_blob;

	enum BuildingPart { BUILDING_SHALLOW_PART = 0, BUILDING_DEEP_PART = 1 };
	BuildingPart m_buildingPart;

	KyUInt32 m_shallowBlobSize;

	KyInt32 m_memStat;
	MemoryHeap* m_heap;
};


/// Use this macro only in implementations of BaseBlobBuilder::DoBuild().
/// Call this macro to set a class member of the blob being built to a specified value.
/// \param blob			The class member of the blob whose value will be set.
/// \param value		The value to set.
/// In COUNT mode, this macro does nothing. In WRITE mode, it sets the value provided.
#define BLOB_SET(blob, value) \
if (this->IsWriteMode()) (blob) = (value)

/// Use this macro only in implementations of BaseBlobBuilder::DoBuild().
/// Call this macro to reserve a BlobArray member of the blob being built.
/// \param blobArray	The BlobArray class member of the blob.
/// \param count		The number of elements to reserve in blobArray.
/// In COUNT mode, only the size of the array is counted. In WRITE mode, the array
/// is built and the default constructor of each element is called.
#define BLOB_ARRAY(blobArray, count) \
this->GetBlobBufferToBuildThis()->AllocArray(this->IsWriteMode() ? &(blobArray) : KY_NULL, count)

/// Use this macro only in implementations of BaseBlobBuilder::DoBuild().
/// Call this macro to reserve a BlobArray member of the blob being built, and to copy
/// its values from another specified BlobArray.
/// \param blobArray	The BlobArray class member of the blob.
/// \param src			The source of the values to write to blobArray.
/// \param count		The number of elements to reserve in blobArray.
/// In COUNT mode, only the size of the array is counted. In WRITE mode, the array
/// is built and the memory contents of src are copied to blobArray. No constructors are
/// called for the elements in the built array.
#define BLOB_ARRAY_COPY(blobArray, src, count) \
this->GetBlobBufferToBuildThis()->AllocAndCopyArray(this->IsWriteMode() ? &(blobArray) : KY_NULL, (count) != 0 ? (src) : KY_NULL, (KyUInt32)(count))

/// same as BLOB_ARRAY_COPY but uses Kaim::Array as input
#define BLOB_ARRAY_COPY_2(blobArray, ky_array) \
BLOB_ARRAY_COPY(blobArray, ky_array.GetDataPtr(), ky_array.GetSize())

/// Use this macro only in implementations of BaseBlobBuilder::DoBuild().
/// Call this macro to copy a string value to a member of the blob being built.
/// \param str			The class member of the blob in which to copy the string.
/// \param src			The string to copy to str.
/// In COUNT mode, only the length of the string is counted. In WRITE mode, the memory contents ofsrc 
/// are copied to str.
/// This macro is effectively the same as #BLOB_ARRAY_COPY, but automatically calculates the length
/// of the array as strlen(src) + 1.
#define BLOB_STRING(str, src) \
this->GetBlobBufferToBuildThis()->AllocAndCopyArray(this->IsWriteMode() ? &(str) : KY_NULL, src, (KyUInt32)SFstrlen(src) + 1)

/// Use this macro only in implementations of BaseBlobBuilder::DoBuild().
/// Call this macro to build a class member of the blob being built if that class member is itself
/// a blob that needs to be built through its own class of BaseBlobBuilder. This automatically invokes the 
/// BaseBlobBuilder::Build() method of builder in order to set up the blob class member.
/// \param blob			The class member that needs to be built.
/// \param builder		An object that derives from BaseBlobBuilder that will be invoked to build blob.
#define BLOB_BUILD(blob, builder) \
builder.DoBuildAllocatedBlob(this->GetBlobBufferToBuildThis(), this->IsWriteMode() ? &(blob) : KY_NULL)
// alias
#define BUILD_BLOB(blob, builder) BLOB_BUILD(blob, builder)

/// Use this macro only in implementations of BaseBlobBuilder::DoBuild().
/// BUILD_REFERENCED_BLOB must be called after all calls to BLOB_ARRAY, BLOB_ARRAY_COPY, BLOB_STRING, BLOB_BUILD 
#define BUILD_REFERENCED_BLOB(blobRef, builder) \
builder.DoAllocAndBuildReferencedBlob(this->GetBlobBufferToBuildReference(), this->IsWriteMode() ? &(blobRef) : KY_NULL)

/// Use this macro only in implementations of BaseBlobBuilder::DoBuild().
/// COPY_REFERENCED_BLOB must be called after all calls to BLOB_ARRAY, BLOB_ARRAY_COPY, BLOB_STRING, BLOB_BUILD 
#define COPY_REFERENCED_BLOB(blobRef, srcBlob, srcBlobDeepSize, srcBlobShallowSize)     \
this->GetBlobBufferToBuildReference()->AllocAndCopyReferencedBlob(                      \
	this->IsWriteMode() ? &(blobRef) : KY_NULL, srcBlob, srcBlobDeepSize, srcBlobShallowSize)

#define COPY_REFERENCED_BLOB_FROM_HANDLER(blobRef, blobHandler)                          \
this->GetBlobBufferToBuildReference()->AllocAndCopyReferencedBlobFromBlobHandler(        \
	this->IsWriteMode() ? &(blobRef) : KY_NULL, blobHandler)


// --------------------------------- inline implementation ---------------------------------

template<class T>
T* BaseBlobBuilder<T>::Build(BlobHandler<T>& blobHandler)
{
	BlobBuffer blobBuffer;
	m_blobBuffer = &blobBuffer;

	// COUNT
	m_blobBuffer->Alloc<T>();
	m_buildingPart = BUILDING_SHALLOW_PART;
	m_shallowBlobSize = 0;
	DoBuild();

	// ALLOCATE
	m_blobBuffer->SwitchToWriteMode(blobHandler, m_shallowBlobSize, m_heap, m_memStat);

	// WRITE
	m_blob = m_blobBuffer->Alloc<T>();
	m_buildingPart = BUILDING_SHALLOW_PART;
	m_shallowBlobSize = 0;
	DoBuild();

	return (T*)m_blob;
}


template<class T>
void BaseBlobBuilder<T>::BuildFlatBlob(T& blob)
{
	m_blob = &blob;
	DoBuild();
}


template<class T>
void BaseBlobBuilder<T>::DoBuildAllocatedBlob(BlobBuffer* blobBuffer, T* blob)
{
	m_blobBuffer = blobBuffer;
	m_blob = blob;
	DoBuild();
}


template<class T>
void BaseBlobBuilder<T>::DoAllocAndBuildReferencedBlob(BlobBuffer* blobBuffer, BlobRef<T>* blobRef)
{
	m_blobBuffer = blobBuffer;

	m_blobBuffer->BeginBlobRefBuffer(blobRef);

	m_blob = m_blobBuffer->Alloc<T>();
	DoBuild();

	m_blobBuffer->SetBlobRefInfoFromCurrentOffset();
}


}


#endif
