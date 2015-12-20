/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_BinaryHeap_H
#define Navigation_BinaryHeap_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_Alg.h"
#include "gwnavruntime/basesystem/logger.h"
#include "gwnavruntime/math/fastmath.h"
#include "gwnavruntime/containers/kyarray.h"

namespace Kaim
{

static const KyUInt32 BinaryHeapInvalidReference = KyUInt32MAXVAL;

/* Class for BinaryHeap to ignore index tracking. */
template <typename T>
class BinaryHeapIndexTracker_None
{
public:
	static void OnAdd(T& /*value*/, KyUInt32 /*indexInBinaryHeap*/) {}
	static void OnRemove(T& /*value*/) {}
	static void OnSwap(T& /*lhs*/, T& /*rhs*/) {}
};

/* Class for BinaryHeap to update index in binaryHeap assuming index is T::m_indexInBinaryHeap. */
template <typename T>
class BinaryHeapIndexTracker_IndexInBinaryHeap
{
public:
	// Called after the element has been added.
	static KY_INLINE void OnAdd(T& value, KyUInt32 indexInBinaryHeap) { value.m_indexInBinaryHeap = indexInBinaryHeap; }
	// Called before the element has been removed.
	static KY_INLINE void OnRemove(T& value) { value.m_indexInBinaryHeap = BinaryHeapInvalidReference; }
	// Called after the elements has been swapped.
	static KY_INLINE void OnSwap(T& lhs, T& rhs) { Alg::Swap(lhs.m_indexInBinaryHeap, rhs.m_indexInBinaryHeap); }
};

/* Class for BinaryHeap to update index in binaryHeap assuming index is T::m_indexInBinaryHeap.
	Specialisation for pointer.
*/
template <typename T>
class BinaryHeapIndexTracker_IndexInBinaryHeap<T*>
{
public:
	// Called after the element has been added.
	static KY_INLINE void OnAdd(T*& value, KyUInt32 indexInBinaryHeap) { value->m_indexInBinaryHeap = indexInBinaryHeap; }
	// Called before the element has been removed.
	static KY_INLINE void OnRemove(T*& value) { value->m_indexInBinaryHeap = BinaryHeapInvalidReference; }
	// Called after the elements has been swapped.
	static KY_INLINE void OnSwap(T*& lhs, T*& rhs) { Alg::Swap(lhs->m_indexInBinaryHeap, rhs->m_indexInBinaryHeap); }
};

/*
	BinaryHeap is a class in which the first element is always the element with the "best" cost (according to Comparator)
	BinaryHeapIndexTracker is a class which is used to keep a track of index in BinaryHeap to allow removal or update or internal element.
	(@see BinaryHeapIndexTracker_IndexInBinaryHeap as example of BinaryHeapIndexTracker).
*/
template <class T, typename ArrayType = KyArray<T>, typename Comparator = DefaultLess<T>, typename BinaryHeapIndexTracker = BinaryHeapIndexTracker_None<T> >
class BinaryHeap
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:

	typedef typename ArrayType::Iterator Iterator;
	typedef typename ArrayType::ConstIterator ConstIterator;

public:
	/* Retrieves the first sorted element in the container.
		\pre IsEmpty() == false */
	KY_INLINE void GetFirst(T& item) const;

	/* Retrieves the first sorted element in the container.
		\pre IsEmpty() == false */
	KY_INLINE const T& GetFirst() const { return *Begin(); }

	/* Add a element in the container.
		\post IsSorted() == true */
	void Insert(const T& item);

	/* Removes the first sorted element in the container.
		\pre IsEmpty() == false 
		\post IsSorted() == true */
	void ExtractFirst();

	/* Removes and retrieves the first sorted element in the container.
		\pre IsEmpty() == false 
		\post IsSorted() == true */
	void ExtractFirst(T& item);

	KY_INLINE void Clear() { m_heap.Clear(); }
	KY_INLINE void Reserve(KyUInt32 size) { return m_heap.Reserve(size); }

	KY_INLINE KyUInt32 GetCount() const { return (KyUInt32)m_heap.GetSize();}
	KY_INLINE KyUInt32 GetCapacity() const { return (KyUInt32)m_heap.GetCapacity();}
	KY_INLINE bool IsEmpty() const { return GetCount() == 0; }

	bool IsSorted() const;

	/* Retrieves the referenced element in the container.
		\pre index < GetCount() */
	void At(KyUInt32 index, T& item);

	KY_INLINE const T& operator[](KyUInt32 index) const { return m_heap[index]; }
	KY_INLINE T& operator[](KyUInt32 index)             { return m_heap[index]; }

	/* Updates the referenced element and sort the container.
		\pre index < GetCount() 
		\post IsSorted() == true */
	void UpdateAt(KyUInt32 index);

	/* Removes the referenced element in the container.
		\pre index < GetCount()
		\post IsSorted() == true */
	void RemoveAt(KyUInt32 index);

	/* Removes the element pointed by the iterator in the container.
		\post IsSorted() == true
		\returns An iterator pointing to the following element in the container.*/
	Iterator Erase(Iterator it);

	/* Returns an iterator that points to the first element in the container. */
	KY_INLINE ConstIterator Begin() const { return m_heap.Begin(); }
	KY_INLINE Iterator Begin()            { return m_heap.Begin(); }

	/* Returns an iterator that points after the last element in the container. */
	KY_INLINE ConstIterator End() const { return m_heap.End(); }
	KY_INLINE Iterator End()            { return m_heap.End(); }

	Comparator& GetComparator() { return m_comparator; }
	ArrayType& GetHeap() { return m_heap; }
private:
	KY_INLINE ConstIterator GetLast() const { return ConstIterator(&m_heap, (GetCount() - 1)); }
	KY_INLINE Iterator GetLast()            { return Iterator(&m_heap, (GetCount() - 1)); }

	KY_INLINE ConstIterator GetParent(ConstIterator it) const {return Begin() + ((GetIndex(it) - 1) >> 1);}
	KY_INLINE Iterator GetParent(Iterator it)                 {return Begin() + ((GetIndex(it) - 1) >> 1);}

	/* Returns an iterator that points to the left child element in the container */
	KY_INLINE ConstIterator GetLeft(ConstIterator it) const {return Begin() + 1 + (GetIndex(it) << 1);}
	KY_INLINE Iterator GetLeft(Iterator it)                 {return Begin() + 1 + (GetIndex(it) << 1);}

	/* Returns an iterator that points to the right child element in the container */
	KY_INLINE ConstIterator GetRight(ConstIterator it) const {return Begin() + 2 + (GetIndex(it) << 1);}
	KY_INLINE Iterator GetRight(Iterator it)                 {return Begin() + 2 + (GetIndex(it) << 1);}

	KY_INLINE KyUInt32 GetIndex(ConstIterator it) const { return KyUInt32(it.GetIndex()); }
	KY_INLINE KyUInt32 GetIndex(Iterator it) const      { return KyUInt32(it.GetIndex()); }

	/* Moves an element towards the first element until it is sorted.*/
	void PercolateUp(Iterator hole);

	/* Moves an element towards the last element until it is sorted.*/
	void PercolateDown(Iterator hole);

private:
	ArrayType m_heap;
	Comparator m_comparator;
};

} // namespace Kaim

#include "gwnavruntime/containers/binaryheap.inl"

#endif // Navigation_BinaryHeap_H
