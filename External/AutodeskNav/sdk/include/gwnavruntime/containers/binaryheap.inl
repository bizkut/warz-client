/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_BinaryHeap_INL
#define Navigation_BinaryHeap_INL

namespace Kaim
{

template <class T, typename ArrayType, typename Comparator, typename BinaryHeapIndexTracker>
void BinaryHeap<T, ArrayType, Comparator, BinaryHeapIndexTracker>::Insert(const T& x)
{
	m_heap.PushBack(x);
	BinaryHeapIndexTracker::OnAdd(*GetLast(), KyUInt32(GetLast() - Begin()));
	// Percolate up
	PercolateUp(GetLast());
}

template <class T, typename ArrayType, typename Comparator, typename BinaryHeapIndexTracker>
inline void BinaryHeap<T, ArrayType, Comparator, BinaryHeapIndexTracker>::GetFirst(T& item) const
{
	KY_ASSERT(IsEmpty() == false);
	item = *Begin();
}

template <class T, typename ArrayType, typename Comparator, typename BinaryHeapIndexTracker>
void BinaryHeap<T, ArrayType, Comparator, BinaryHeapIndexTracker>::ExtractFirst()
{
	KY_ASSERT(IsEmpty() == false);
	Erase(Begin());
}

template <class T, typename ArrayType, typename Comparator, typename BinaryHeapIndexTracker>
void BinaryHeap<T, ArrayType, Comparator, BinaryHeapIndexTracker>::ExtractFirst(T& minItem)
{
	KY_ASSERT(IsEmpty() == false);

	GetFirst(minItem);
	ExtractFirst();
}

template <class T, typename ArrayType, typename Comparator, typename BinaryHeapIndexTracker>
void BinaryHeap<T, ArrayType, Comparator, BinaryHeapIndexTracker>::At(KyUInt32 index, T& item)
{
	KY_ASSERT(index < GetCount());
	item = m_heap[index];
}

template <class T, typename ArrayType, typename Comparator, typename BinaryHeapIndexTracker>
void BinaryHeap<T, ArrayType, Comparator, BinaryHeapIndexTracker>::UpdateAt(KyUInt32 index)
{
	KY_ASSERT(index < GetCount());
	
	if (index == 0 || m_comparator(*GetParent(ConstIterator(&m_heap, index)), m_heap[index]))
		PercolateDown(Iterator(&m_heap, index));
	else
		PercolateUp(Iterator(&m_heap, index));
}

template <class T, typename ArrayType, typename Comparator, typename BinaryHeapIndexTracker>
typename BinaryHeap<T, ArrayType, Comparator, BinaryHeapIndexTracker>::Iterator BinaryHeap<T, ArrayType, Comparator, BinaryHeapIndexTracker>::Erase(typename BinaryHeap<T, ArrayType, Comparator, BinaryHeapIndexTracker>::Iterator it)
{
	if (it != End())
	{
		const KyUInt32 count = (KyUInt32)m_heap.GetSize();

		BinaryHeapIndexTracker::OnSwap(*it, *GetLast());
		Alg::Swap(*it, *GetLast());

		BinaryHeapIndexTracker::OnRemove(*GetLast());

		m_heap.PopBack();

		if (count != 1)
		{
			// the heap os not empty
			PercolateDown(it);
			return it;
		}
	}
	return End();
}

template <class T, typename ArrayType, typename Comparator, typename BinaryHeapIndexTracker>
void BinaryHeap<T, ArrayType, Comparator, BinaryHeapIndexTracker>::RemoveAt(KyUInt32 index)
{
	KY_ASSERT(index < GetCount());
	Erase(&m_heap[index]);
}

template <class T, typename ArrayType, typename Comparator, typename BinaryHeapIndexTracker>
bool BinaryHeap<T, ArrayType, Comparator, BinaryHeapIndexTracker>::IsSorted() const
{
	if (GetCount() > 1)
	{
		for (ConstIterator it(&m_heap, 1); it != End(); ++it)
		{
			if (m_comparator(*it, *GetParent(it)) == true)
				return false;
		}
	}

	return true;
}

template <class T, typename ArrayType, typename Comparator, typename BinaryHeapIndexTracker>
void BinaryHeap<T, ArrayType, Comparator, BinaryHeapIndexTracker>::PercolateUp(Iterator hole)
{
	Iterator parent;

	for (; hole.GetIndex() > Begin().GetIndex() ; hole = parent)
	{
		parent = GetParent(hole);
		// if x higher than its parent, stop, it is right-placed
		if (m_comparator(*hole, *parent))
		{
			Alg::Swap(*hole, *parent);
			BinaryHeapIndexTracker::OnSwap(*hole, *parent);
		}
		else
			break;
	}
}

template <class T, typename ArrayType, typename Comparator, typename BinaryHeapIndexTracker>
void BinaryHeap<T, ArrayType, Comparator, BinaryHeapIndexTracker>::PercolateDown(Iterator hole)
{
	Iterator last = GetLast();
	Iterator child = GetLeft(hole);

	while ( child.GetIndex() < last.GetIndex() )
	{
		// here child has been set to the left child of hole
		// compare left child and rigth child
		if (m_comparator(*child, *(child + 1)) == false)
			++child; // the rigth child is the one we have to compare with hole

		if (m_comparator(*child, *hole) == false)
			// stop, hole is at the good place
			return; 

		// swap the hole and its child
		BinaryHeapIndexTracker::OnSwap(*child, *hole);
		Alg::Swap(*child, *hole);

		hole = child;
		// go on with the left child
		child = GetLeft(child);
	}

	// check if left == last. no rigth child
	if (child == last && m_comparator(*child, *hole))
	{
		BinaryHeapIndexTracker::OnSwap(*child, *hole);
		Alg::Swap(*child, *hole);
	}

}

} // Kaim

#endif // Navigation_BinaryHeap_INL
