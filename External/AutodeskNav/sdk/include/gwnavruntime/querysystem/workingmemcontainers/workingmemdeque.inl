/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

namespace Kaim
{

template <class T>
KY_INLINE WorkingMemDeque<T>::WorkingMemDeque() : m_headIdx(0), m_tailIdx(0), m_lastIdx(0) {}

template <class T>
KY_INLINE WorkingMemDeque<T>::WorkingMemDeque(WorkingMemory* workingMemory)
{
	Init(workingMemory);
}

template <class T>
KY_INLINE void WorkingMemDeque<T>::Init(WorkingMemory* workingMemory)
{
	if (KY_SUCCEEDED(m_workingMemContainerBase.Init(workingMemory)))
		m_lastIdx = (m_workingMemContainerBase.GetBufferSize() / sizeof(T)) - 1;
	else
		m_lastIdx = 0;

	m_headIdx = 0;
	m_tailIdx = 0;
}

template <class T>
KY_INLINE T*   WorkingMemDeque<T>::GetBuffer()     const { return (T*)m_workingMemContainerBase.GetBuffer(); }
template <class T>
KY_INLINE bool WorkingMemDeque<T>::IsInitialized() const { return m_workingMemContainerBase.IsInitialized(); }
template <class T>
KY_INLINE bool WorkingMemDeque<T>::IsEmpty()       const { return m_tailIdx == m_headIdx;               }

template <class T>
KY_INLINE bool WorkingMemDeque<T>::IsFull()
{
	const bool isFull = m_tailIdx != m_lastIdx ? m_tailIdx + 1 == m_headIdx : m_headIdx == 0;
	return isFull && KY_FAILED(TryToResize());
}

template <class T>
KY_INLINE  void WorkingMemDeque<T>::MakeEmpty() { m_headIdx = 0; m_tailIdx = 0; }
template <class T>
KY_INLINE KyUInt32 WorkingMemDeque<T>::GetHeadIdx()    const { return m_headIdx; }
template <class T>
KY_INLINE KyUInt32 WorkingMemDeque<T>::GetTailEndIdx() const { return m_tailIdx; }

template <class T>
KY_INLINE  KyUInt32 WorkingMemDeque<T>::GetNextItemIdx(KyUInt32 idx) const
{
	KY_DEBUG_ASSERTN(idx != m_tailIdx, ("idx is the index of the last item : should no compute the next item index"));
	return idx != m_lastIdx ? idx + 1 : 0;
}

template <class T>
KY_INLINE  KyUInt32 WorkingMemDeque<T>::GetPrevItemIdx(KyUInt32 idx) const
{
	KY_DEBUG_ASSERTN(idx != m_headIdx, ("idx is the index of the firt item : should compute the previous item index"));
	return idx != 0 ? idx - 1 : m_lastIdx;
}

template <class T>
KY_INLINE KyResult WorkingMemDeque<T>::PushFront(const T& x)
{
	KY_DEBUG_ASSERTN(IsInitialized(), ("You are trying to add an item wheareas the buffer is not valid"));

	if (IsFull() == false)
	{
		m_headIdx = m_headIdx != 0 ? m_headIdx - 1 : m_lastIdx;
		GetBuffer()[m_headIdx] = x;

		return KY_SUCCESS;
	}

	return KY_ERROR;
}

template <class T>
KY_INLINE void WorkingMemDeque<T>::PopFront()
{
	KY_DEBUG_ASSERTN(IsInitialized(), ("You are trying to removing an item wheareas the buffer is not valid"));
	KY_LOG_ERROR_IF(IsEmpty() == true, ("You should not call PopFront on an empty Deque"));
	m_headIdx = m_headIdx != m_lastIdx ? m_headIdx + 1 : 0;
}

template <class T>
KY_INLINE void WorkingMemDeque<T>::Front(T& item) const
{
	KY_DEBUG_ASSERTN(IsInitialized(), ("You are trying to retrieve an item wheareas the buffer is not valid"));
	KY_LOG_ERROR_IF(IsEmpty() == true, ("You should not call Front on an empty Deque"));
	item = GetBuffer()[m_headIdx];
}

template <class T>
KY_INLINE KyResult WorkingMemDeque<T>::PushBack(const T& x)
{
	KY_DEBUG_ASSERTN(IsInitialized(), ("You are trying to add an item wheareas the buffer is not valid"));

	if (IsFull() == false)
	{
		GetBuffer()[m_tailIdx] = x;
		m_tailIdx = m_tailIdx != m_lastIdx ? m_tailIdx + 1 : 0;

		return KY_SUCCESS;
	}

	return KY_ERROR;
}

template <class T>
KY_INLINE void WorkingMemDeque<T>::PopBack()
{
	KY_DEBUG_ASSERTN(IsInitialized(), ("You are trying to remove an item wheareas the buffer is not valid"));
	KY_LOG_ERROR_IF(IsEmpty() == true, ("You should not call PopBack on an empty Deque"));
	m_tailIdx = m_tailIdx != 0 ? m_tailIdx - 1 : m_lastIdx;
}

template <class T>
KY_INLINE void WorkingMemDeque<T>::Back(T& item) const
{
	KY_DEBUG_ASSERTN(IsInitialized(), ("You are trying to retrieve an item wheareas the buffer is not valid"));
	KY_LOG_ERROR_IF(IsEmpty() == true, ("You should not call Back on an empty Deque"));
	item = GetBuffer()[m_tailIdx != 0 ? m_tailIdx - 1 : m_lastIdx];
}

template <class T>
KY_INLINE T& WorkingMemDeque<T>::GetItem(KyUInt32 idx)
{
	KY_DEBUG_ASSERTN(IsInitialized(), ("You are trying to retrieve an item wheareas the buffer is not valid"));
	KY_DEBUG_ASSERTN(idx <= m_lastIdx && idx != m_tailIdx, ("Invalid input idx. It must be lower than m_lastIdx = %d", m_lastIdx));
	return GetBuffer()[idx];
}


template <class T>
KY_INLINE KyResult WorkingMemDeque<T>::TryToResize()
{
	T* oldBuffer = GetBuffer();
	T* newBuffer = (T*)m_workingMemContainerBase.Resize();

	if (newBuffer == KY_NULL)
		return KY_ERROR;

	const KyUInt32 newLastIdx = (m_workingMemContainerBase.GetBufferSize() / sizeof(T)) - 1;

	if (oldBuffer != KY_NULL)
	{
		if (m_tailIdx > m_headIdx)
		{
			memcpy(newBuffer + m_headIdx, oldBuffer + m_headIdx, (m_tailIdx - m_headIdx) * sizeof(T));
		}
		else
		{
			KY_ASSERT(m_tailIdx < m_headIdx);
			memcpy(newBuffer, oldBuffer, m_tailIdx * sizeof(T));
			const KyUInt32 newHeadIdx = newLastIdx - (m_lastIdx - m_headIdx);
			memcpy(newBuffer + newHeadIdx, oldBuffer + m_headIdx, (m_lastIdx - m_headIdx + 1) * sizeof(T));
			m_headIdx = newHeadIdx;
		}

		m_workingMemContainerBase.ReleaseMemory(oldBuffer);
	}

	m_lastIdx = newLastIdx;
	return KY_SUCCESS;
}

}

