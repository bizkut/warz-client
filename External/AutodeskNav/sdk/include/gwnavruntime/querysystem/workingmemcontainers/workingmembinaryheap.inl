/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


namespace Kaim
{

template <class T, typename Comparator, typename BinaryHeapIndexTracker>
KY_INLINE WorkingMemBinaryHeap<T, Comparator, BinaryHeapIndexTracker>::WorkingMemBinaryHeap() : m_heap() {}

template <class T, typename Comparator, typename BinaryHeapIndexTracker>
KY_INLINE WorkingMemBinaryHeap<T, Comparator, BinaryHeapIndexTracker>::WorkingMemBinaryHeap(WorkingMemory* workingMemory) :
	m_heap(workingMemory) {}

template <class T, typename Comparator, typename BinaryHeapIndexTracker>
KY_INLINE void WorkingMemBinaryHeap<T, Comparator, BinaryHeapIndexTracker>::Init(WorkingMemory* workingMemory)
{
	m_heap.Init(workingMemory);
}

template <class T, typename Comparator, typename BinaryHeapIndexTracker>
KY_INLINE bool WorkingMemBinaryHeap<T, Comparator, BinaryHeapIndexTracker>::IsInitialized()   const { return m_heap.IsInitialized(); }
template <class T, typename Comparator, typename BinaryHeapIndexTracker>
KY_INLINE KyUInt32 WorkingMemBinaryHeap<T, Comparator, BinaryHeapIndexTracker>::GetCount()    const { return m_heap.GetCount();      }
template <class T, typename Comparator, typename BinaryHeapIndexTracker>
KY_INLINE KyUInt32 WorkingMemBinaryHeap<T, Comparator, BinaryHeapIndexTracker>::GetCapacity() const { return m_heap.GetCapacity();   }
template <class T, typename Comparator, typename BinaryHeapIndexTracker>
KY_INLINE bool WorkingMemBinaryHeap<T, Comparator, BinaryHeapIndexTracker>::IsEmpty()         const { return m_heap.IsEmpty();       }
template <class T, typename Comparator, typename BinaryHeapIndexTracker>
KY_INLINE bool WorkingMemBinaryHeap<T, Comparator, BinaryHeapIndexTracker>::IsFull() { return m_heap.IsFull(); }
template <class T, typename Comparator, typename BinaryHeapIndexTracker>
KY_INLINE void WorkingMemBinaryHeap<T, Comparator, BinaryHeapIndexTracker>::Clear() { m_heap.MakeEmpty(); }

template <class T, typename Comparator, typename BinaryHeapIndexTracker>
KY_INLINE void WorkingMemBinaryHeap<T, Comparator, BinaryHeapIndexTracker>::GetFirst(T& item) const
{
	KY_ASSERT(IsEmpty() == false);
	item = m_heap[0];
}

template <class T, typename Comparator, typename BinaryHeapIndexTracker>
KY_INLINE const T&  WorkingMemBinaryHeap<T, Comparator, BinaryHeapIndexTracker>::GetFirst() const
{
	KY_ASSERT(IsEmpty() == false);
	return m_heap[0];
}

template <class T, typename Comparator, typename BinaryHeapIndexTracker>
void WorkingMemBinaryHeap<T, Comparator, BinaryHeapIndexTracker>::Insert(const T& x)
{
	KY_ASSERT(m_heap.GetCount() != m_heap.GetCapacity());

	const KyUInt32 newIndex = m_heap.GetCount();
	m_heap.PushBack_UnSafe(x);
	m_binayHeapTracker.OnAdd(m_heap[newIndex], newIndex);
	// Percolate up
	PercolateUp(newIndex);
}

template <class T, typename Comparator, typename BinaryHeapIndexTracker>
void WorkingMemBinaryHeap<T, Comparator, BinaryHeapIndexTracker>::ExtractFirst(T& minItem)
{
	KY_ASSERT(IsEmpty() == false);

	const KyUInt32 countMinus1 = m_heap.GetCount() - 1;

	minItem = m_heap[0];

	m_binayHeapTracker.OnSwap(m_heap[0], m_heap[countMinus1]);
	Alg::Swap(m_heap[0], m_heap[countMinus1]);

	m_binayHeapTracker.OnRemove(m_heap[countMinus1]);

	m_heap.PopBack();

	if (countMinus1 != 0)
	{
		// the heap is not empty
		PercolateDown(0, countMinus1);
	}
}

template <class T, typename Comparator, typename BinaryHeapIndexTracker>
void WorkingMemBinaryHeap<T, Comparator, BinaryHeapIndexTracker>::UpdateAt(KyUInt32 index)
{
	KY_ASSERT(index < GetCount());

	if (index == 0 || m_comparator(m_heap[GetParentIndex(index)], m_heap[index]))
		PercolateDown(index, GetCount());
	else
		PercolateUp(index);
}

template <class T, typename Comparator, typename BinaryHeapIndexTracker>
void WorkingMemBinaryHeap<T, Comparator, BinaryHeapIndexTracker>::PercolateUp(KyUInt32 holeIndex)
{
	KyUInt32 parentIndex;

	while (holeIndex > 0)
	{
		parentIndex = GetParentIndex(holeIndex);
		if (m_comparator(m_heap[holeIndex], m_heap[parentIndex]))
		{
			m_binayHeapTracker.OnSwap(m_heap[holeIndex], m_heap[parentIndex]);
			Alg::Swap(m_heap[holeIndex], m_heap[parentIndex]);
			holeIndex = parentIndex;
		}
		else
			break;
	}
}

template <class T, typename Comparator, typename BinaryHeapIndexTracker>
void WorkingMemBinaryHeap<T, Comparator, BinaryHeapIndexTracker>::PercolateDown(KyUInt32 holeIndex, KyUInt32 currentCount)
{
	KyUInt32 childIndex = GetLeftChildIndex(holeIndex);
	KyUInt32 lastIndex = currentCount - 1;

	while (childIndex < lastIndex)
	{
		
		// here child has been set to the left child of hole
		// compare left child and rigth child
		if (m_comparator(m_heap[childIndex], m_heap[childIndex+1]) == false)
			++childIndex; // the rigth child is the one we have to compare with hole

		if (m_comparator(m_heap[childIndex], m_heap[holeIndex]) == false)
			// stop, hole is at the good place
			return; 

		// swap the hole and its child
		m_binayHeapTracker.OnSwap(m_heap[childIndex], m_heap[holeIndex]);
		Alg::Swap(m_heap[childIndex], m_heap[holeIndex]);

		holeIndex = childIndex;
		// go on with the left child
		childIndex = GetLeftChildIndex(holeIndex);
	}

	// check if left == last. no rigth child
	if (childIndex == lastIndex && m_comparator(m_heap[childIndex], m_heap[holeIndex]))
	{
		m_binayHeapTracker.OnSwap(m_heap[childIndex], m_heap[holeIndex]);
		Alg::Swap(m_heap[childIndex], m_heap[holeIndex]);
	}
}

}
