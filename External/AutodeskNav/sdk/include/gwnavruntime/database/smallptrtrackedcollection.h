/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_SmallPtrTrackedCollection_H
#define Navigation_SmallPtrTrackedCollection_H

#include "gwnavruntime/base/memory.h"


namespace Kaim
{

// small container that optimize the case of only 1 element
// designed to store T*, default constructor and destructor of template class object are not called
// no deep copy is made
template <class T>
class SmallPtrTrackedCollection
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	SmallPtrTrackedCollection() : m_count(0), m_capacity(1) { m_values.m_multipleValues = KY_NULL; }
	~SmallPtrTrackedCollection() { KY_LOG_ERROR_IF(m_count != 0, ("memory leak !")); }

	KY_INLINE bool     IsEmpty()  const { return GetCount() == 0; }
	KY_INLINE KyUInt32 GetCount() const { return m_count;         }

	KY_INLINE T** GetValues() { return m_capacity == 1 ? &m_values.m_oneValue : m_values.m_multipleValues; }

	void PushBack(MemoryHeap* heap, T* newValue)
	{
		if (m_capacity == 1)
		{
			if (IsEmpty())
			{
				m_count = 1;
				newValue->SetIndexInCollection(0);
				m_values.m_oneValue = newValue;
				return;
			}

			T* firstValue = m_values.m_oneValue;

			// no buffer allocated yet. create 1 of size 4
			m_values.m_multipleValues = (T**)KY_HEAP_ALLOC(heap, KyUInt32(4 * sizeof(T*)), MemStat_NavData);

			m_count = 2;
			m_capacity = 4;

			m_values.m_multipleValues[0] = firstValue;
			m_values.m_multipleValues[1] = newValue;

			newValue->SetIndexInCollection(1);
			return;
		}

		if (m_count == m_capacity)
			Reserve(heap, 2 * m_capacity);

		m_values.m_multipleValues[m_count] = newValue;
		newValue->SetIndexInCollection(m_count);
		++m_count;

		return;
	}

	void Reserve(MemoryHeap* heap, KyUInt32 newcapacity)
	{
		KY_DEBUG_ASSERTN(newcapacity > m_count, ("no shrinking !"));

		T** values = (T**)KY_HEAP_ALLOC(heap, newcapacity * (KyUInt32)sizeof(T*), MemStat_NavData);

		memcpy(values, m_values.m_multipleValues, m_count * sizeof(T*));

		KY_FREE(m_values.m_multipleValues);

		m_capacity = (KyUInt16)newcapacity;
		m_values.m_multipleValues = values;
	}

	void ForceClear()
	{
		if (m_capacity > 1)
		{
			for(KyUInt32 i = 0; i < m_count; ++i)
				m_values.m_multipleValues[i]->SetIndexInCollection(KyUInt32MAXVAL);

			KY_FREE(m_values.m_multipleValues);
		}
		else
		{
			if (m_count != 0)
				m_values.m_oneValue->SetIndexInCollection(KyUInt32MAXVAL);
		}

		m_count = 0;
		m_capacity = 1;
		m_values.m_oneValue = KY_NULL;
	}

	void SwapWithLastAndPopBack(T* valueToRemove)
	{
		const KyUInt32 index = valueToRemove->GetIndexInCollection();
		const KyUInt32 lastIndex = (KyUInt32)m_count - 1;

		KY_DEBUG_ASSERTN(index < m_count, ("error"));
		if (m_capacity > 1)
		{
			m_values.m_multipleValues[index]->SetIndexInCollection(KyUInt32MAXVAL);

			if (index != lastIndex)
			{
				// not the last element, swap with effective last
				m_values.m_multipleValues[lastIndex]->SetIndexInCollection(index);
				m_values.m_multipleValues[index] = m_values.m_multipleValues[lastIndex];
			}

			--m_count;
		}
		else
		{
			m_values.m_oneValue->SetIndexInCollection(KyUInt32MAXVAL);
			m_values.m_oneValue = KY_NULL;
			m_count = 0;
		}
	}

	void GetOwnershipOfData(SmallPtrTrackedCollection& other)
	{
		ForceClear();

		m_values = other.m_values;
		m_count = other.m_count;
		m_capacity = other.m_capacity;

		other.m_values.m_oneValue = KY_NULL;
		other.m_count = 0;
		other.m_capacity = 0;
	}

private:
	union Values
	{
		T*  m_oneValue;
		T** m_multipleValues;
	};

	Values m_values;
	KyUInt16 m_count;
	KyUInt16 m_capacity;
};

}

#endif //Navigation_SmallPtrTrackedCollection_H

