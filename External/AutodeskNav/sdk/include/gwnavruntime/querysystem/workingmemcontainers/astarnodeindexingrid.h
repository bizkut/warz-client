/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_AstarNodeIndexInGrid_H
#define Navigation_AstarNodeIndexInGrid_H

#include "gwnavruntime/queries/utils/astartypes.h"
#include "gwnavruntime/database/activedata.h"


namespace Kaim
{

class AstarNodeIndexInGrid
{
public:

	KY_INLINE KyUInt32 GetNumberOfWordsForNodeIndices(KyUInt32 numberOfNodes) { return numberOfNodes; }

	/*
		AstarIndex on 32bits
	*/
	struct NavGraphToNodeIndices
	{
		KY_INLINE AStarNodeIndex GetAStarNodeIndex(NavGraphVertexIdx navGraphVertexIdx) const { return GetNodeIndices()[navGraphVertexIdx]; }
		KY_INLINE void SetAStarNodeIndex(NavGraphVertexIdx navGraphVertexIdx, AStarNodeIndex index) { GetNodeIndices()[navGraphVertexIdx] = index; }

		KY_INLINE AStarNodeIndex* GetNodeIndices() const { return (AStarNodeIndex*)((char*)this + m_offsetToNodeIndices); }

		KY_INLINE bool IsValid() const { return m_offsetToNodeIndices != KyUInt32MAXVAL; }
		KyUInt32 m_offsetToNodeIndices;
	};

	/*
		AstarIndex on 32bits
	*/
	struct AbstractGraphToNodeIndices
	{
		KY_INLINE AStarNodeIndex GetAStarNodeIndex(AbstractGraphNodeIdx abstractGraphNodeIdx) const { return GetNodeIndices()[abstractGraphNodeIdx]; }
		KY_INLINE void SetAStarNodeIndex(AbstractGraphNodeIdx abstractGraphNodeIdx, AStarNodeIndex index) { GetNodeIndices()[abstractGraphNodeIdx] = index; }

		KY_INLINE AStarNodeIndex* GetNodeIndices() const { return (AStarNodeIndex*)((char*)this + m_offsetToNodeIndices); }

		KY_INLINE bool IsValid() const { return m_offsetToNodeIndices != KyUInt32MAXVAL; }
		KyUInt32 m_offsetToNodeIndices;
	};

	/*
		AstarIndex on 32bits
	*/
	struct NavFloorToNodeIndices
	{
		KY_INLINE AStarNodeIndex GetAStarNodeIndex(NavHalfEdgeIdx navHalfEdgeIdx) const { return GetNodeIndices()[navHalfEdgeIdx]; }
		KY_INLINE void SetAStarNodeIndex(NavHalfEdgeIdx navHalfEdgeIdx, AStarNodeIndex index) { GetNodeIndices()[navHalfEdgeIdx] = index; }

		KY_INLINE AStarNodeIndex* GetNodeIndices() const { return (AStarNodeIndex*)((char*)this + m_offsetToNodeIndices); }
		KY_INLINE bool IsValid() const { return m_offsetToNodeIndices != KyUInt32MAXVAL; }

		KyUInt32 m_offsetToNodeIndices;
	};

	struct CellPosToNavFloors
	{
		KY_INLINE bool IsValid() const { return m_offSetToNavFloorToNodeIndices != KyUInt32MAXVAL; }
		KY_INLINE NavFloorToNodeIndices* GetNavFloorToNodeIndices() const { return (NavFloorToNodeIndices*)((char*)this + m_offSetToNavFloorToNodeIndices); }

		KyUInt32 m_offSetToNavFloorToNodeIndices;
		KyUInt32 m_navDataChangeIdx;
	};

public:
	AstarNodeIndexInGrid() : m_numberOfNavGraph(0), m_currentOffsetFromBuffer(0) {}

	AstarNodeIndexInGrid(WorkingMemory* workingMemory, ActiveData* activeData) { Init(workingMemory, activeData); }

	void Init(WorkingMemory* workingMemory, ActiveData* activeData);

	void ReleaseWorkingMemoryBuffer();

	KyUInt32 GetAvailableSizeInBytes() const;

	bool IsEnoughPlaceForAllocation(KyUInt32 sizeInBytes);

	bool TryToResize();

	void MakeEmpty();

	void* AllocateInBufferAndMemsetTo1(KyUInt32 totalSizeToNewOffSet);

	AStarNodeIndex* AllocateAstarNodeIndices(KyUInt32 numberOfEdges);

	CellPosToNavFloors* AllocateCellPosToNavFloors(KyUInt32 numberOfCell);
	NavFloorToNodeIndices* AllocateNavFloorToNodeIndex(KyUInt32 numberOfFloors);
	NavGraphToNodeIndices* AllocateNavGraphToNodeIndex(KyUInt32 numberOfGraphs);
	AbstractGraphToNodeIndices* AllocateAbstractGraphToNodeIndex(KyUInt32 numberOfAbstractGraphs);

	CellPosToNavFloors* GetCellPosToNavFloors(const CellPos& cellPos);

	KyResult GetNavFloorToNodeIndices(ActiveData* activeData, const NavFloorRawPtr& navFloorRawPtr, NavFloorToNodeIndices*& nodeIndices);
	KyResult GetNavGraphToNodeIndices(const NavGraphVertexRawPtr& navGraphVertexRawPtr, NavGraphToNodeIndices*& nodeIndices);
	KyResult GetAbstractGraphToNodeIndices(const AbstractGraphNodeRawPtr& abstractGraphNodeRawPtr, AbstractGraphToNodeIndices*& nodeIndices);

	NavGraphToNodeIndices* GetNavGraphToNodeIndices_Unsafe(const NavGraphVertexRawPtr& navGraphVertexRawPtr);
	NavFloorToNodeIndices* GetNavFloorToNodeIndices_Unsafe(const NavFloorRawPtr& navFloorRawPtr);
	AbstractGraphToNodeIndices* GetAbstractGraphToNodeIndices_Unsafe(const AbstractGraphNodeRawPtr& abstractGraphNodeRawPtr);

	bool IsInitialized() const { return m_workingMemContainerBase.IsInitialized(); }
	bool HasNavDataChanged(Database* database);

private:
	NavGraphToNodeIndices* GetNavGraphToNodeIndicesBuffer();
	AbstractGraphToNodeIndices* GetAbstractGraphToNodeIndicesBuffer();
	CellPosToNavFloors* GetCellPosToNavFloorsBuffer();

public:
	WorkingMemContainerBase m_workingMemContainerBase;

	KyUInt32 m_numberOfNavGraph;
	KyUInt32 m_navGraphChangeIdx;
	
	KyUInt32 m_numberOfAbstractGraph;
	KyUInt32 m_abstractGraphChangeIdx;

	KyUInt32 m_currentOffsetFromBuffer;
	CellBox m_activeDataCellBox;
	CellBox m_abstractGraphCellBox;
};

KY_INLINE void AstarNodeIndexInGrid::ReleaseWorkingMemoryBuffer() { m_workingMemContainerBase.ReleaseBuffer(); }

KY_INLINE KyUInt32 AstarNodeIndexInGrid::GetAvailableSizeInBytes() const { return m_workingMemContainerBase.GetBufferSize() - m_currentOffsetFromBuffer; }

KY_INLINE bool AstarNodeIndexInGrid::IsEnoughPlaceForAllocation(KyUInt32 sizeInBytes)
{
	while (GetAvailableSizeInBytes() < sizeInBytes)
	{
		if (TryToResize() == false)
			return false;
	}

	return true;
}

KY_INLINE AStarNodeIndex* AstarNodeIndexInGrid::AllocateAstarNodeIndices(KyUInt32 numberOfEdges)
{
	return (AStarNodeIndex*)AllocateInBufferAndMemsetTo1(sizeof(KyUInt32) * GetNumberOfWordsForNodeIndices(numberOfEdges));
}

KY_INLINE AstarNodeIndexInGrid::NavFloorToNodeIndices* AstarNodeIndexInGrid::AllocateNavFloorToNodeIndex(KyUInt32 numberOfFloors)
{
	return (NavFloorToNodeIndices*)AllocateInBufferAndMemsetTo1(sizeof(NavFloorToNodeIndices) * numberOfFloors);
}

KY_INLINE AstarNodeIndexInGrid::NavGraphToNodeIndices* AstarNodeIndexInGrid::AllocateNavGraphToNodeIndex(KyUInt32 numberOfGraphs)
{
	return (NavGraphToNodeIndices*)AllocateInBufferAndMemsetTo1(sizeof(NavGraphToNodeIndices) * numberOfGraphs);
}

KY_INLINE AstarNodeIndexInGrid::AbstractGraphToNodeIndices* AstarNodeIndexInGrid::AllocateAbstractGraphToNodeIndex(KyUInt32 numberOfAbstractGraphs)
{
	return (AbstractGraphToNodeIndices*)AllocateInBufferAndMemsetTo1(sizeof(AbstractGraphToNodeIndices) * numberOfAbstractGraphs);
}

KY_INLINE AstarNodeIndexInGrid::CellPosToNavFloors* AstarNodeIndexInGrid::AllocateCellPosToNavFloors(KyUInt32 numberOfCell)
{
	return (CellPosToNavFloors*)AllocateInBufferAndMemsetTo1(sizeof(CellPosToNavFloors) * numberOfCell);
}


KY_INLINE AstarNodeIndexInGrid::NavGraphToNodeIndices* AstarNodeIndexInGrid::GetNavGraphToNodeIndicesBuffer() 
{ return (NavGraphToNodeIndices*)m_workingMemContainerBase.GetBuffer(); }

KY_INLINE AstarNodeIndexInGrid::AbstractGraphToNodeIndices* AstarNodeIndexInGrid::GetAbstractGraphToNodeIndicesBuffer()
{ return (AbstractGraphToNodeIndices*) (GetNavGraphToNodeIndicesBuffer() + m_numberOfNavGraph); }

KY_INLINE AstarNodeIndexInGrid::CellPosToNavFloors* AstarNodeIndexInGrid::GetCellPosToNavFloorsBuffer()
{ return (CellPosToNavFloors*) (GetAbstractGraphToNodeIndicesBuffer() + m_numberOfAbstractGraph); }

KY_INLINE AstarNodeIndexInGrid::CellPosToNavFloors* AstarNodeIndexInGrid::GetCellPosToNavFloors(const CellPos& cellPos)
{
	CellPosToNavFloors* memoryStartForGrid = GetCellPosToNavFloorsBuffer();
	return memoryStartForGrid + m_activeDataCellBox.GetRowMajorIndex(cellPos);
}

KY_INLINE AstarNodeIndexInGrid::NavGraphToNodeIndices* AstarNodeIndexInGrid::GetNavGraphToNodeIndices_Unsafe(const NavGraphVertexRawPtr& navGraphVertexRawPtr)
{
	NavGraphToNodeIndices* memoryStartForGraphs = GetNavGraphToNodeIndicesBuffer();
	return &memoryStartForGraphs[navGraphVertexRawPtr.GetNavGraph()->m_idxInTheActiveDataBuffer];
}

KY_INLINE AstarNodeIndexInGrid::NavFloorToNodeIndices* AstarNodeIndexInGrid::GetNavFloorToNodeIndices_Unsafe(const NavFloorRawPtr& navFloorRawPtr)
{
	KY_DEBUG_ASSERTN(m_activeDataCellBox.IsInside(navFloorRawPtr.GetCellPos()), ("Invalid CellBox"));

	NavFloor* navFloor = navFloorRawPtr.GetNavFloor();
	const CellPos& cellPos = navFloor->GetCellPos();

	CellPosToNavFloors* cellPosToNavFloors = GetCellPosToNavFloors(cellPos);
	KY_DEBUG_ASSERTN(cellPosToNavFloors->IsValid(), ("Bad usage of UnSafe function"));

	NavFloorToNodeIndices& navFloorToNodeIndices = cellPosToNavFloors->GetNavFloorToNodeIndices()[navFloor->GetIndexInCollection()];
	KY_DEBUG_ASSERTN(navFloorToNodeIndices.IsValid(), ("Bad usage of UnSafe function"));

	return &navFloorToNodeIndices;
}

KY_INLINE AstarNodeIndexInGrid::AbstractGraphToNodeIndices* AstarNodeIndexInGrid::GetAbstractGraphToNodeIndices_Unsafe(const AbstractGraphNodeRawPtr& abstractGraphNodeRawPtr)
{
	AbstractGraphToNodeIndices* memoryStartForAbstractGraphs = GetAbstractGraphToNodeIndicesBuffer();
	return &memoryStartForAbstractGraphs[abstractGraphNodeRawPtr.m_abstractGraph->m_abstractGraphIdx];
}

}


#endif //Navigation_AstarNodeIndexInGrid_H

