/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_WorkingMemBinaryHeap_H
#define Navigation_WorkingMemBinaryHeap_H

#include "gwnavruntime/querysystem/workingmemcontainers/workingmemarray.h"
#include "gwnavruntime/kernel/SF_Alg.h"
#include "gwnavruntime/containers/kyarray.h"

namespace Kaim
{

typedef KyUInt16 IndexInBinHeap;
static const IndexInBinHeap IndexInBinHeap_UnSet = KyUInt16MAXVAL;
static const IndexInBinHeap IndexInBinHeap_Removed = IndexInBinHeap_UnSet - 1;

template <typename T>
class WorkingMemBinaryHeapIndexTracker_None
{
public:
	static void OnAdd(T& /*value*/, KyUInt32 /*indexInBinaryHeap*/) {}
	static void OnRemove(T& /*value*/) {}
	static void OnSwap(T& /*lhs*/, T& /*rhs*/) {}
};

template <class Item, typename Comparator = DefaultLess<Item>, typename BinaryHeapIndexTracker = WorkingMemBinaryHeapIndexTracker_None<Item> >
class WorkingMemBinaryHeap
{
public:
	WorkingMemBinaryHeap();
	WorkingMemBinaryHeap(WorkingMemory* workingMemory);

	void Init(WorkingMemory* workingMemory);

	KY_INLINE bool IsInitialized()   const;
	KY_INLINE KyUInt32 GetCount()    const;
	KY_INLINE KyUInt32 GetCapacity() const;

	KY_INLINE bool IsEmpty() const;
	KY_INLINE bool IsFull();

	KY_INLINE void Clear();

	/* Retrieves the first sorted element in the container.
		\pre IsEmpty() == false */
	KY_INLINE void GetFirst(Item& item) const;

	/* Retrieves the first sorted element in the container.
		\pre IsEmpty() == false */
	KY_INLINE const Item& GetFirst() const;

	/* Add a element in the container. */
	void Insert(const Item& item);

	/* Removes and retrieves the first sorted element in the container.
		\pre IsEmpty() == false 
		\post IsSorted() == true */
	void ExtractFirst(Item& item);

	/* Updates the referenced element and sort the container.
		\pre index < GetCount() 
		\post IsSorted() == true */
	void UpdateAt(KyUInt32 index);

	Comparator& GetComparator() { return m_comparator; }

	void ReleaseWorkingMemoryBuffer() { m_heap.ReleaseWorkingMemoryBuffer(); }

private:
	KY_INLINE KyUInt32 GetParentIndex(KyUInt32 index)     { return (index - 1) >> 1; }
	KY_INLINE KyUInt32 GetLeftChildIndex(KyUInt32 index)  { return (index << 1) + 1; }
	KY_INLINE KyUInt32 GetRigthChildIndex(KyUInt32 index) { return (index << 1) + 2; }

	/* Moves an element towards the first element until it is sorted.*/
	void PercolateUp(KyUInt32 HoleIndex);

	/* Moves an element towards the last element until it is sorted.*/
	void PercolateDown(KyUInt32 HoleIndex, KyUInt32 currentCount);

private:
	WorkingMemArray<Item> m_heap;
public:
	Comparator m_comparator;
	BinaryHeapIndexTracker m_binayHeapTracker;
};

}

#include "gwnavruntime/querysystem/workingmemcontainers/workingmembinaryheap.inl"


#endif

