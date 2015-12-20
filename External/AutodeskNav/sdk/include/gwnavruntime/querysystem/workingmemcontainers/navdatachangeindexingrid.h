/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_NavDataChangeIndexInGrid_H
#define Navigation_NavDataChangeIndexInGrid_H

#include "gwnavruntime/queries/utils/astartypes.h"
#include "gwnavruntime/database/activedata.h"

namespace Kaim
{
class AstarNodeIndexInGrid;
class Database;

class NavDataChangeIndexInGrid
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_QueryWorkingMem)
public:

	struct CellPosNavDataIdx
	{
		KY_INLINE bool IsValid() const { return m_navDataChangeIdx != 0; }
		KyUInt32 m_navDataChangeIdx;
	};

public:
	NavDataChangeIndexInGrid() : m_currentOffsetFromBuffer(0), m_navGraphChangeIdx(0) {}

	NavDataChangeIndexInGrid(WorkingMemory* workingMemory, const CellBox& cellBox) :
		m_navGraphChangeIdx(0), m_abstractGraphChangeIdx(0), m_cellBox(cellBox)
	{
		m_workingMemContainerBase.Init(workingMemory);
		m_currentOffsetFromBuffer = 0;
		MakeEmpty();
	}

	~NavDataChangeIndexInGrid() { ReleaseWorkingMemoryBuffer(); }

	void Init(WorkingMemory* workingMemory, const CellBox& cellBox)
	{
		m_workingMemContainerBase.Init(workingMemory);
		m_cellBox = cellBox;
		m_currentOffsetFromBuffer = 0;
		m_navGraphChangeIdx = 0;
		m_abstractGraphChangeIdx = 0;
		MakeEmpty();
	}

	void RetrieveAllIndices(AstarNodeIndexInGrid& astarNodeIndexInGrid);
	void RetrieveAllIndicesFromDatabase(Database* database);

	void ReleaseWorkingMemoryBuffer();

	KyUInt32 GetAvailableSizeInBytes() const;

	bool IsEnoughPlaceForAllocation(KyUInt32 sizeInBytes);

	bool TryToResize();

	void MakeEmpty();

	void* AllocateInBufferAndMemsetTo0(KyUInt32 sizeOfOneElementInBytes, KyUInt32 numberOfElements);

	CellPosNavDataIdx* AllocateCellPosNavDataIdx(KyUInt32 numberOfCell);
	CellPosNavDataIdx* GetCellPosNavDataIdx(const CellPos& cellPos);

	bool IsInitialized() const { return m_workingMemContainerBase.IsInitialized(); }
	bool HasNavDataChanged(Database* database);
public:
	WorkingMemContainerBase m_workingMemContainerBase;
	KyUInt32 m_currentOffsetFromBuffer;
	KyUInt32 m_navGraphChangeIdx;
	KyUInt32 m_abstractGraphChangeIdx;
	CellBox m_cellBox;
};

KY_INLINE void NavDataChangeIndexInGrid::ReleaseWorkingMemoryBuffer() { m_workingMemContainerBase.ReleaseBuffer(); }

KY_INLINE KyUInt32 NavDataChangeIndexInGrid::GetAvailableSizeInBytes() const { return m_workingMemContainerBase.GetBufferSize() - m_currentOffsetFromBuffer; }

KY_INLINE bool NavDataChangeIndexInGrid::IsEnoughPlaceForAllocation(KyUInt32 sizeInBytes)
{
	while (GetAvailableSizeInBytes() < sizeInBytes)
	{
		if (TryToResize() == false)
			return false;
	}

	return true;
}

KY_INLINE NavDataChangeIndexInGrid::CellPosNavDataIdx* NavDataChangeIndexInGrid::AllocateCellPosNavDataIdx(KyUInt32 numberOfCell)
{
	return (CellPosNavDataIdx*)AllocateInBufferAndMemsetTo0(sizeof(CellPosNavDataIdx), numberOfCell);
}

KY_INLINE NavDataChangeIndexInGrid::CellPosNavDataIdx* NavDataChangeIndexInGrid::GetCellPosNavDataIdx(const CellPos& cellPos)
{
	CellPosNavDataIdx* memoryStartForGrid = (CellPosNavDataIdx*)(m_workingMemContainerBase.GetBuffer());
	return memoryStartForGrid + m_cellBox.GetRowMajorIndex(cellPos);
}

}


#endif //Navigation_NavDataChangeIndexInGrid_H

