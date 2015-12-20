/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// Primary contact: JUBA - secondary contact: LASI
#ifndef Navigation_DynamicNavCell_H
#define Navigation_DynamicNavCell_H

#include "gwnavruntime/navmesh/celldesc.h"
#include "gwnavruntime/navmesh/dynamicnavfloor.h"

namespace Kaim
{


struct DynamicCellBoundaryFloor
{
	KyUInt32 m_floorIdx;
};

struct DynamicCellBoundaryEdge
{
	KyUInt32 m_halfEdgeIdx;
};

struct DynamicCellBoundaryEdgesInFloor
{
	DynamicCellBoundaryEdgesInFloor() : m_cellBoundaryEdges(KY_NULL) {}
	DynamicCellBoundaryEdgesInFloor(MemoryHeap* heap) : m_cellBoundaryEdges(heap)
	{
		for (KyUInt32 i = 0; i < 4; ++i)
			m_cellBoundaryEdges.PushBack(ArrayDH<DynamicCellBoundaryEdge>(heap));
	}
	ArrayDH<ArrayDH<DynamicCellBoundaryEdge> > m_cellBoundaryEdges;// one vector per Direction : CardinalDir_EST, CardinalDir_NORTH, CardinalDir_WEST, CardinalDir_SOUTH
};

// --------------------------
// ----- DynamicNavCell -----
// --------------------------
class DynamicNavCell
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	DynamicNavCell(MemoryHeap* heap, const CellDesc& cellDesc)
		: m_cellDesc(cellDesc)
		, m_floors(heap)
	{}

	void Clear()
	{
		for (KyUInt32 i = 0; i < m_floors.GetCount(); ++i)
			m_floors[i].Clear();
	}

	CardinalDir GetBoundaryDir(const PixelPos& start, const PixelPos& end) const;

	bool AreIndexesWithinBounds() const
	{
		// Check indexes inside of each floor
		for (KyUInt32 floorIdx = 0; floorIdx < m_floors.GetCount(); ++floorIdx)
		{
			if (!m_floors[floorIdx].AreIndexesWithinBounds())
			{
				return false;
			}
		}

		return true;
	}

	bool IsEmpty()
	{
		if (m_floors.IsEmpty())
			return true;

		for (KyUInt32 i = 0; i < m_floors.GetCount(); ++i)
		{
			const DynamicNavFloor& floor = m_floors[i];
			if (floor.m_edges.IsEmpty() == false ||  floor.m_triangles.IsEmpty() == false )
				return false;
		}

		return true;
	}

public:
	CellDesc m_cellDesc;
	KyArrayDH<DynamicNavFloor> m_floors;
};

}


#endif //Navigation_DynamicNavCell_H

