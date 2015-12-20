/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_Collection_H
#define Navigation_Collection_H

#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/kernel/SF_Debug.h"

namespace Kaim
{

template <typename T>
class CollectionIndexTracker_None;

template <typename T>
class CollectionIndexTrackerPtr;

template <class T, typename CollectionIndexTracker, int SID>
class CollectionImpl;

static const KyUInt32 CollectionInvalidIndex = KyUInt32MAXVAL;

/// Collection is a class which is a "Collection" of elements (not sorted, not unique)
template <class T, int SID = Stat_Default_Mem>
class Collection
{
	KY_DEFINE_NEW_DELETE_OPERATORS(SID)
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	T& PushBack(const T& value);
	T& PushBack();

	T& PushBackUnique(const T& value);

	void Clear();
	void ClearAndReleaseMemory();
	void Reserve(KyUInt32 size);

	KyUInt32 GetCount()    const;
	KyUInt32 GetCapacity() const;
	bool IsEmpty()         const;

	const T& operator[](KyUInt32 index) const;
	      T& operator[](KyUInt32 index)      ;

	const T* GetDataPtr() const;
	      T* GetDataPtr();

	/// Removes the element at specified index in the container.
	/// \return the index of the following element in the container.
	KyUInt32 RemoveAt(KyUInt32 index);

	KyUInt32 FindFirstOccurrence(const T& value, KyUInt32 startIndex = 0);
	KyUInt32 RemoveFirstOccurrence(const T& value, KyUInt32 startIndex = 0);
	bool DoesContain(const T& value) const;

public: // internal
	CollectionImpl<T, CollectionIndexTracker_None<T>, SID> m_collectionImpl;
};


/// TrackedCollection is a class which is a "Collection" of T=C* or T = Ptr< C > (not sorted, not unique).
/// We keep a track of index in Collection to allow direct removal of internal element
/// if T = C* or T = Ptr< C >, object C must have this two functions implemented :
/// <code>
///     KyUInt32 GetIndexInCollection() const;
///     void SetIndexInCollection(KyUInt32 indexInCollection) 
/// </code>
template <class T, int SID = Stat_Default_Mem>
class TrackedCollection
{
	KY_DEFINE_NEW_DELETE_OPERATORS(SID)
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	T& PushBack(const T& value);
	T& PushBack();

	T& PushBackUnique(const T& value);

	void Clear();
	void ClearAndReleaseMemory();
	void Reserve(KyUInt32 size);

	KyUInt32 GetCount()    const;
	KyUInt32 GetCapacity() const;
	bool IsEmpty()         const;

	const T& operator[](KyUInt32 index) const;
	      T& operator[](KyUInt32 index)      ;

	const T* GetDataPtr() const;
	      T* GetDataPtr();

	/// Removes the element at specified index in the container.
	/// \returns index of the following element in the container.
	KyUInt32 RemoveAt(KyUInt32 index);

	KyUInt32 FindFirstOccurrence(const T& value, KyUInt32 startIndex = 0);
	KyUInt32 RemoveFirstOccurrence(const T& value, KyUInt32 startIndex = 0);
	bool DoesContain(const T& value) const;

	KyUInt32 Remove(const T& value);

public: // internal
	CollectionImpl<T, CollectionIndexTrackerPtr<T>, SID> m_collectionImpl;
};


} // namespace Kaim

#include "gwnavruntime/containers/collection.inl"

#endif
