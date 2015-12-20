/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_TriangleStatusInGrid_H
#define Navigation_TriangleStatusInGrid_H

#include "gwnavruntime/querysystem/workingmemcontainers/statusingridbase.h"
#include "gwnavruntime/database/activedata.h"

namespace Kaim
{

class TriangleStatusInGrid
{
	// to access Triangle Status
	// From CellPos we allocate some memory in the buffer for NavFloors (IntermediateStatusAccessor)
	// and then for each navFloor we allocate memory for triangles status (IndexedStatus)
	typedef StatusInGridBase::IndexedStatus NavFloorToTriangles;
	typedef StatusInGridBase::IntermediateStatusAccessor<NavFloorToTriangles> CellPosToNavFloors;
	
public:
	TriangleStatusInGrid(WorkingMemory* workingMemory, const CellBox& cellBox) :
	 m_statusInGridBase(workingMemory, cellBox)
	{
		MakeEmpty();
	}

	void MakeEmpty();

	KY_INLINE NavFloorToTriangles* AllocateNavFloorToTriangles(KyUInt32 numberofFloors);
	KY_INLINE CellPosToNavFloors* AllocateCellPosToNavCells(KyUInt32 numberOfNavCell);
	KY_INLINE CellPosToNavFloors* GetCellPosToNavFloors(const CellPos& cellPos);

	KyResult OpenNodeIfNew(ActiveData& activeData, const NavTriangleRawPtr& triangleRawPtr, bool& nodeWasNew);
	bool IsTriangleOpen(const NavTriangleRawPtr& triangleRawPtr) const;

	KY_INLINE bool IsTriangleOpen_Unsafe(const NavTriangleRawPtr& triangleRawPtr) const;

	KY_INLINE bool IsInitialized() const { return m_statusInGridBase.IsInitialized(); }

	void ReleaseWorkingMemoryBuffer() { m_statusInGridBase.ReleaseWorkingMemoryBuffer(); }
public:
	StatusInGridBase m_statusInGridBase;
};

KY_INLINE TriangleStatusInGrid::NavFloorToTriangles* TriangleStatusInGrid::AllocateNavFloorToTriangles(KyUInt32 numberofFloors)
{
	return (NavFloorToTriangles*)m_statusInGridBase.AllocateInBufferAndMemset(sizeof(NavFloorToTriangles), numberofFloors, 0xFF);
}

KY_INLINE TriangleStatusInGrid::CellPosToNavFloors* TriangleStatusInGrid::AllocateCellPosToNavCells(KyUInt32 numberOfNavCell)
{
	return (CellPosToNavFloors*)m_statusInGridBase.AllocateInBufferAndMemset(sizeof(CellPosToNavFloors), numberOfNavCell, 0xFF);
}

KY_INLINE bool TriangleStatusInGrid::IsTriangleOpen_Unsafe(const NavTriangleRawPtr& triangleRawPtr) const
{
	KY_DEBUG_ASSERTN(m_statusInGridBase.m_cellBox.IsInside(triangleRawPtr.GetCellPos()), ("Invalid CellBox"));

	NavFloor* navFloor = triangleRawPtr.GetNavFloor();
	const CellPos& cellPos = navFloor->GetCellPos();

	CellPosToNavFloors& cellPosToNavFloors = ((CellPosToNavFloors*)m_statusInGridBase.GetBuffer())[m_statusInGridBase.m_cellBox.GetRowMajorIndex(cellPos)];
	KY_DEBUG_ASSERTN(cellPosToNavFloors.IsValid(), ("Unsafe mode, it should never happen"));

	NavFloorToTriangles& navFloorToTriangles = *cellPosToNavFloors.GetUnderlyingObject(navFloor->GetIndexInCollection());
	KY_DEBUG_ASSERTN(navFloorToTriangles.IsValid(), ("Unsafe mode, it should never happen"));

	return navFloorToTriangles.IsNodeOpen(triangleRawPtr.GetTriangleIdx());
}

KY_INLINE TriangleStatusInGrid::CellPosToNavFloors* TriangleStatusInGrid::GetCellPosToNavFloors(const CellPos& cellPos)
{
	return (CellPosToNavFloors*)m_statusInGridBase.GetBuffer() + m_statusInGridBase.m_cellBox.GetRowMajorIndex(cellPos);
}

}


#endif

