/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_HalfEdgeDataPtrInGrid_H
#define Navigation_HalfEdgeDataPtrInGrid_H

#include "gwnavruntime/database/activedata.h"
#include "gwnavruntime/querysystem/workingmemcontainers/workingmemcontainerbase.h"

namespace Kaim
{

class HalfEdgeDataPtrInGrid
{
public:

	struct NavFloorToEdgeDataPtrBuffer
	{
		KY_INLINE void* GetEdgeDataPtr(NavHalfEdgeIdx navHalfEdgeIdx) const { return GetEdgeDataPtrBuffer()[navHalfEdgeIdx]; }
		KY_INLINE void SetEdgeDataPtr(NavHalfEdgeIdx navHalfEdgeIdx, void* tdata) { GetEdgeDataPtrBuffer()[navHalfEdgeIdx] = tdata; }

		KY_INLINE void** GetEdgeDataPtrBuffer() const { return (void**)((char*)this + m_offsetToEdgeDataPtrBuffer); }

		KY_INLINE bool IsValid() const { return m_offsetToEdgeDataPtrBuffer != (UPInt)-1; }
		UPInt m_offsetToEdgeDataPtrBuffer;
	};

	struct CellPosToNavFloors
	{
		KY_INLINE bool IsValid() const { return m_offSetToNavFloorToEdgeDataPtrBuffer != KyUInt32MAXVAL; }
		KY_INLINE NavFloorToEdgeDataPtrBuffer* GetNavFloorToEdgeDataPtrBuffer() const { return (NavFloorToEdgeDataPtrBuffer*)((char*)this + m_offSetToNavFloorToEdgeDataPtrBuffer); }

		KyUInt32 m_offSetToNavFloorToEdgeDataPtrBuffer;
		KyUInt32 m_navDataChangeIdx;
	};

public:
	HalfEdgeDataPtrInGrid() : m_currentOffsetFromBuffer(0) {}
	HalfEdgeDataPtrInGrid(WorkingMemory* workingMemory, ActiveData* activeData) : m_cellBox(activeData->GetCellBox())
	{
		m_workingMemContainerBase.Init(workingMemory);
		m_currentOffsetFromBuffer = 0;
		MakeEmpty();
	}

	void Init(WorkingMemory* workingMemory, ActiveData* activeData)
	{
		m_workingMemContainerBase.Init(workingMemory);
		m_cellBox = activeData->GetCellBox();
		m_currentOffsetFromBuffer = 0;
		MakeEmpty();
	}

	void ReleaseWorkingMemoryBuffer();

	KyUInt32 GetAvailableSizeInBytes() const;

	bool IsEnoughPlaceForAllocation(KyUInt32 sizeInBytes);

	bool TryToResize();

	void MakeEmpty();

	void* AllocateInBufferAndMemsetTo1(KyUInt32 totalSizeToNewOffSet);
	void* AllocateInBufferAndMemsetTo0(KyUInt32 totalSizeToNewOffSet);

	void** AllocateEdgeDataPtrBuffer(KyUInt32 numberOfEdges);

	CellPosToNavFloors* AllocateCellPosToNavFloors(KyUInt32 numberOfCell);
	NavFloorToEdgeDataPtrBuffer* AllocateNavFloorToEdgeData(KyUInt32 numberofFloors);

	CellPosToNavFloors* GetCellPosToNavFloors(const CellPos& cellPos);

	KyResult GetNavFloorToEdgeDataPtrBuffer(ActiveData* activeData, const NavFloorRawPtr& navFloorRawPtr, NavFloorToEdgeDataPtrBuffer*& nodeIndices);
	NavFloorToEdgeDataPtrBuffer* GetNavFloorToEdgeDataPtrBuffer_Unsafe(const NavFloorRawPtr& navFloorRawPtr);

	bool IsInitialized() const { return m_workingMemContainerBase.IsInitialized(); }
	bool HasNavDataChanged(Database* database);
public:
	WorkingMemContainerBase m_workingMemContainerBase;
	KyUInt32 m_currentOffsetFromBuffer;
	CellBox m_cellBox;
};


KY_INLINE void HalfEdgeDataPtrInGrid::ReleaseWorkingMemoryBuffer() { m_workingMemContainerBase.ReleaseBuffer(); }
KY_INLINE KyUInt32 HalfEdgeDataPtrInGrid::GetAvailableSizeInBytes() const { return m_workingMemContainerBase.GetBufferSize() - m_currentOffsetFromBuffer; }
KY_INLINE bool HalfEdgeDataPtrInGrid::IsEnoughPlaceForAllocation(KyUInt32 sizeInBytes)
{
	while (GetAvailableSizeInBytes() < sizeInBytes)
	{
		if (TryToResize() == false)
			return false;
	}

	return true;
}

KY_INLINE void** HalfEdgeDataPtrInGrid::AllocateEdgeDataPtrBuffer(KyUInt32 numberOfEdges)
{
	return (void**)AllocateInBufferAndMemsetTo0(numberOfEdges * sizeof(void*));
}

KY_INLINE HalfEdgeDataPtrInGrid::NavFloorToEdgeDataPtrBuffer* HalfEdgeDataPtrInGrid::AllocateNavFloorToEdgeData(KyUInt32 numberofFloors)
{
	return (NavFloorToEdgeDataPtrBuffer*)AllocateInBufferAndMemsetTo1(sizeof(NavFloorToEdgeDataPtrBuffer) * numberofFloors);
}

KY_INLINE HalfEdgeDataPtrInGrid::CellPosToNavFloors* HalfEdgeDataPtrInGrid::AllocateCellPosToNavFloors(KyUInt32 numberOfCell)
{
	return (CellPosToNavFloors*)AllocateInBufferAndMemsetTo1(sizeof(CellPosToNavFloors) * numberOfCell);
}


KY_INLINE HalfEdgeDataPtrInGrid::CellPosToNavFloors* HalfEdgeDataPtrInGrid::GetCellPosToNavFloors(const CellPos& cellPos)
{
	CellPosToNavFloors* memoryStartForGrid = (CellPosToNavFloors*)m_workingMemContainerBase.GetBuffer();
	return memoryStartForGrid + m_cellBox.GetRowMajorIndex(cellPos);
}


KY_INLINE HalfEdgeDataPtrInGrid::NavFloorToEdgeDataPtrBuffer* HalfEdgeDataPtrInGrid::GetNavFloorToEdgeDataPtrBuffer_Unsafe(const NavFloorRawPtr& navFloorRawPtr)
{
	KY_DEBUG_ASSERTN(m_cellBox.IsInside(navFloorRawPtr.GetCellPos()), ("Invalid CellBox"));

	NavFloor* navFloor = navFloorRawPtr.GetNavFloor();
	const CellPos& cellPos = navFloor->GetCellPos();

	CellPosToNavFloors* cellPosToNavFloors = GetCellPosToNavFloors(cellPos);
	KY_DEBUG_ASSERTN(cellPosToNavFloors->IsValid(), ("Bad usage of UnSafe function"));

	NavFloorToEdgeDataPtrBuffer& navFloorToNodeIndices = cellPosToNavFloors->GetNavFloorToEdgeDataPtrBuffer()[navFloor->GetIndexInCollection()];
	KY_DEBUG_ASSERTN(navFloorToNodeIndices.IsValid(), ("Bad usage of UnSafe function"));

	return &navFloorToNodeIndices;
}


}



#endif //Navigation_HalfEdgeDataPtrInGrid_H

