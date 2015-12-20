/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_ActiveNavFloorCollection_H
#define Navigation_ActiveNavFloorCollection_H

#include "gwnavruntime/navmesh/navcell.h"
#include "gwnavruntime/navmesh/navfloor.h"

namespace Kaim
{

// small container to store the activeNavFloors within a CellPos in the ActiveData
// all the NavFloor* of the same NavCell are contiguous in memory
// allocation are made in a dedicated heap
// no deep copy
class ActiveNavFloorCollection
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
	
public:
	ActiveNavFloorCollection() : m_activeNavFloors(KY_NULL), m_count(0), m_capacity(0) {}
	~ActiveNavFloorCollection() { KY_LOG_ERROR_IF(m_activeNavFloors != KY_NULL, ("memory leak !")); }

	KY_INLINE bool IsEmpty() const { return m_count == 0; }
	KY_INLINE KyUInt32 GetCount() const { return m_count; }

	void GetOwnershipOfData(ActiveNavFloorCollection& other)
	{
		ForceClear();

		m_activeNavFloors = other.m_activeNavFloors;
		m_count = other.m_count;
		m_capacity = other.m_capacity;

		other.m_activeNavFloors = KY_NULL;
		other.m_count = 0;
		other.m_capacity = 0;
	}

	// resize to m_count + numberOfNavFloorInCell, set the NavCell* for each new NavFloor*
	void InsertActiveNavFloorsOfNavCell(MemoryHeap* heap, NavCell* navCell)
	{
		const KyUInt32 numberOfNavFloorInCell = navCell->GetNavFloorCount();
		const KyUInt32 previousCount = m_count;

		KyUInt32 newCapacity = Max<KyUInt32>(4, m_capacity);
		if (m_count + numberOfNavFloorInCell > m_capacity)
		{
			while(previousCount + numberOfNavFloorInCell > newCapacity)
				newCapacity *= 2;

			Reserve(heap, newCapacity);
		}

		// += cause warning in vc8...
		m_count = m_count + (KyUInt16)numberOfNavFloorInCell;
		KY_ASSERT(m_count <= m_capacity);

		NavFloor** navFloors = GetNavFloors() + previousCount;
		for(KyUInt32 idx = 0; idx < numberOfNavFloorInCell; ++idx)
			navFloors[idx] = navCell->GetNavFloor(idx);
	}

	NavFloor** GetNavFloors() const { return m_activeNavFloors; }

	void Reserve(MemoryHeap* heap, KyUInt32 newCapacity)
	{
		KY_DEBUG_ASSERTN(newCapacity > m_capacity, ("no shrinking !"));

		char* newData = (char*)KY_HEAP_ALLOC(heap, newCapacity * (KyUInt32)sizeof(NavFloor*), MemStat_NavData);

#if defined(KY_BUILD_DEBUG)
		memset(newData, 0xFE, newCapacity * (KyUInt32)sizeof(NavFloor*));
#endif

		if (m_capacity > 0)
		{
			if (m_count >0)
				memcpy(newData, (char*)m_activeNavFloors, m_count * sizeof(NavFloor*));

			KY_FREE(m_activeNavFloors);
		}

		m_capacity = (KyUInt16)newCapacity;
		m_activeNavFloors = (NavFloor**)newData;
	}

	void Clear()
	{
#if defined(KY_BUILD_DEBUG)
		if (m_capacity > 0)
			memset((char*)m_activeNavFloors, 0, m_capacity * ((KyUInt32)sizeof(NavFloor*)));
#endif
		m_count = 0;
	}

	void ForceClear()
	{
		if (m_capacity > 0)
		{
			KY_FREE(m_activeNavFloors);

			m_count = 0;
			m_capacity = 0;
			m_activeNavFloors = KY_NULL;
		}
		else
		{
			KY_ASSERT(m_count == 0);
			KY_ASSERT(m_capacity == 0);
			KY_ASSERT(m_activeNavFloors == KY_NULL);
		}
	}

public:
	NavFloor** m_activeNavFloors;
	KyUInt16 m_count;
	KyUInt16 m_capacity;
};

}

#endif //Navigation_ActiveNavFloorCollection_H

