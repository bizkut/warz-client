/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_CircularArray_INL
#define Navigation_CircularArray_INL

namespace Kaim
{

template <class T, int SID>
KY_INLINE CircularArray<T, SID>::CircularArray(MemoryHeap* heap) :
	m_buffer(KY_NULL),
	m_heap(heap),
	m_lastValidIdx(KyUInt32MAXVAL),
	m_beginIdx(0),
	m_endIdx(0),
	m_count(0)
{
	Clear();
}

template <class T, int SID>
KY_INLINE CircularArray<T, SID>::~CircularArray()
{
	Clear();
	KY_FREE(m_buffer);
}

template <class T, int SID>
void CircularArray<T, SID>::Clear()
{
	if (m_buffer != KY_NULL && m_count != 0)
	{
		if (m_beginIdx < m_endIdx)
		{
			DestroyRange(m_buffer + m_beginIdx, m_buffer + m_endIdx);
		}
		else
		{
			DestroyRange(m_buffer + m_beginIdx, m_buffer + m_lastValidIdx + 1);
			DestroyRange(m_buffer, m_buffer + m_endIdx);
		}
	}
	m_beginIdx = 0;
	m_endIdx = 0;
	m_count = 0;
}


template <class T, int SID>
void CircularArray<T, SID>::Reserve(KyUInt32 newCapacity)
{
	if (m_buffer != KY_NULL)
	{
		if(newCapacity <= GetCapacity())
			// never shrink !
			return;

		T* newBuffer;

		if (m_heap == KY_NULL)
			newBuffer = KY_MALLOC(T, newCapacity, SID);
		else
			newBuffer = KY_HEAP_MALLOC(m_heap, T, newCapacity, SID);

		if (m_count != 0)
		{
			if (m_beginIdx < m_endIdx)
			{
				ConstructRange(m_buffer + m_beginIdx, m_buffer + m_endIdx, newBuffer);
				DestroyRange(m_buffer + m_beginIdx, m_buffer + m_endIdx);
			}
			else
			{
				const KyUInt32 indexOffset = m_lastValidIdx + 1 - m_beginIdx;

				ConstructRange(m_buffer + m_beginIdx, m_buffer + m_lastValidIdx + 1, newBuffer);
				DestroyRange(m_buffer + m_beginIdx, m_buffer + m_lastValidIdx + 1);
				ConstructRange(m_buffer, m_buffer + m_endIdx, newBuffer + indexOffset);
				DestroyRange(m_buffer, m_buffer + m_endIdx);
			}
		}

		KY_FREE(m_buffer);
		m_buffer = newBuffer;
	}
	else
	{
		KY_ASSERT(newCapacity != 0);

		if (m_heap == KY_NULL)
			m_buffer = KY_MALLOC(T, newCapacity, SID);
		else
			m_buffer = KY_HEAP_MALLOC(m_heap, T, newCapacity, SID);

	}

	m_lastValidIdx = newCapacity - 1;
	m_beginIdx = 0;
	m_endIdx = m_count;
}


template <class T, int SID>
KY_INLINE KyUInt32 CircularArray<T, SID>::GetCount()    const { return m_count;                      }
template <class T, int SID>
KY_INLINE KyUInt32 CircularArray<T, SID>::GetCapacity() const { return m_lastValidIdx + 1;           }
template <class T, int SID>
KY_INLINE bool     CircularArray<T, SID>::IsEmpty()     const { return GetCount() == 0;              }
template <class T, int SID>
KY_INLINE bool     CircularArray<T, SID>::IsFull()      const { return GetCount() == GetCapacity();  }


template <class T, int SID>
KY_INLINE void CircularArray<T, SID>::IncreaseBeginIdx()
{
	m_beginIdx = m_beginIdx != m_lastValidIdx ? m_beginIdx + 1 : 0;
	--m_count;
}
template <class T, int SID>
KY_INLINE void CircularArray<T, SID>::IncreaseEndIdx()
{
	m_endIdx = m_endIdx != m_lastValidIdx ? m_endIdx + 1 : 0;
	++m_count;
}
template <class T, int SID>
KY_INLINE void CircularArray<T, SID>::DecreaseBeginIdx()
{
	m_beginIdx = m_beginIdx != 0 ? m_beginIdx - 1 : m_lastValidIdx;
	++m_count;
}
template <class T, int SID>
KY_INLINE void CircularArray<T, SID>::DecreaseEndIdx()
{
	m_endIdx = m_endIdx != 0 ? m_endIdx - 1 : m_lastValidIdx;
	--m_count;
}

template <class T, int SID>
KY_INLINE void CircularArray<T, SID>::PushBack(const T& item)
{
	AutoReserve();
	new (m_buffer + m_endIdx) T(item);
	IncreaseEndIdx();
}

template <class T, int SID>
KY_INLINE void CircularArray<T, SID>::PushFront(const T& item)
{
	AutoReserve();
	DecreaseBeginIdx();
	new (m_buffer + m_beginIdx) T(item);
}

template <class T, int SID>
KY_INLINE void CircularArray<T, SID>::PopBack()
{
	KY_ASSERT(IsEmpty() == false);
	DecreaseEndIdx();
	m_buffer[m_endIdx].~T();
}

template <class T, int SID>
KY_INLINE void CircularArray<T, SID>::PopFront()
{
	KY_ASSERT(!IsEmpty());
	m_buffer[m_beginIdx].~T();
	IncreaseBeginIdx();
}

template <class T, int SID>
inline void CircularArray<T, SID>::PopNFirstElements(KyUInt32 popCount)
{
	if (popCount == 0)
		return;

	KY_ASSERT(!IsEmpty() && popCount <= GetCount());

	if (popCount == GetCount())
	{
		Clear();
		return;
	}
	
	if (m_beginIdx >= m_endIdx && m_beginIdx + popCount > m_lastValidIdx)
	{
		const KyUInt32 elementCountToDelete = m_lastValidIdx + 1 - m_beginIdx;
		m_count -= elementCountToDelete;
		popCount -= elementCountToDelete;

		DestroyRange(m_buffer + m_beginIdx, m_buffer + m_lastValidIdx + 1);
		m_beginIdx = 0;
	}

	DestroyRange(m_buffer + m_beginIdx, m_buffer + m_beginIdx + popCount);
	m_beginIdx += popCount;
	m_count -= popCount;
}

template <class T, int SID>
KY_INLINE T& CircularArray<T, SID>::operator [] (KyUInt32 index)
{
	KY_ASSERT(index < m_count);
	const KyUInt32 realIdx = (m_beginIdx + index) % GetCapacity();
	return m_buffer[realIdx];
}

template <class T, int SID>
KY_INLINE const T& CircularArray<T, SID>::operator [] (KyUInt32 index) const
{
	KY_ASSERT(index < m_count);
	const KyUInt32 realIdx = (m_beginIdx + index) % GetCapacity();
	return m_buffer[realIdx];
}


template <class T, int SID>
KY_INLINE const T& CircularArray<T, SID>::GetFront() const
{
	KY_ASSERT(IsEmpty() == false);
	return m_buffer[m_beginIdx];
}
template <class T, int SID>
KY_INLINE T& CircularArray<T, SID>::GetFront()
{
	KY_ASSERT(IsEmpty() == false);
	return m_buffer[m_beginIdx];
}
template <class T, int SID>
KY_INLINE const T& CircularArray<T, SID>::GetBack() const
{
	KY_ASSERT(IsEmpty() == false);
	return m_buffer[m_endIdx != 0 ? m_endIdx - 1 : m_lastValidIdx];
}
template <class T, int SID>
KY_INLINE T& CircularArray<T, SID>::GetBack()
{
	KY_ASSERT(IsEmpty() == false);
	return m_buffer[m_endIdx != 0 ? m_endIdx - 1 : m_lastValidIdx];
}
} // namespace Kaim

#endif // Navigation_CircularArray_INL
