/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_NavFloorAndNavGraphEdgeStatusInGrid_H
#define Navigation_NavFloorAndNavGraphEdgeStatusInGrid_H

#include "gwnavruntime/querysystem/workingmemcontainers/statusingridbase.h"
#include "gwnavruntime/navgraph/identifiers/navgraphedgerawptr.h"

namespace Kaim
{

class NavFloorAndNavGraphEdgeStatusInGrid
{
public:
	// for navFloor, directly from cellPos to NavFloorStatus : no intermediate class
	typedef StatusInGridBase::IndexedStatus CellPosToNavFloorsStatus;

	// to access NavGraphEdge status
	// From NavGraph we allocate some memory in the buffer for NavGraphVertex (IntermediateStatusAccessor)
	// and then for each NavGraphVertex we allocate memory for outgoing NavGraphEdge status (IndexedStatus)
	typedef StatusInGridBase::IndexedStatus NavGraphVertexToOutGoingEdgeStatus;
	typedef StatusInGridBase::IntermediateStatusAccessor<NavGraphVertexToOutGoingEdgeStatus> NavGraphToNavGraphVertex;

public:
	NavFloorAndNavGraphEdgeStatusInGrid() : m_numberOfNavGraph(0) {}

	NavFloorAndNavGraphEdgeStatusInGrid(WorkingMemory* workingMemory, ActiveData* activeData) :
		m_statusInGridBase(), m_numberOfNavGraph(0)
	{
		Init(workingMemory, activeData);
	}

	void Init(WorkingMemory* workingMemory, ActiveData* activeData)
	{
		m_statusInGridBase.Init(workingMemory, activeData->GetCellBox());
		m_numberOfNavGraph = activeData->GetNavGraphCount();

		MakeEmpty();
	}

	void ReleaseWorkingMemoryBuffer() { m_statusInGridBase.ReleaseWorkingMemoryBuffer(); }
	void MakeEmpty();

	KY_INLINE CellPosToNavFloorsStatus* AllocateCellPosToNavFloorsStatus(KyUInt32 numberOfNavCell);
	KY_INLINE NavGraphToNavGraphVertex* AllocateNavGraphToNavGraphVertex(KyUInt32 numberOfGraphs);
	KY_INLINE NavGraphVertexToOutGoingEdgeStatus* AllocateNavGraphVertexToEdgeStatus(KyUInt32 numberOfOutgoingEdges);

	KY_INLINE CellPosToNavFloorsStatus* GetCellPosToNavFloorsStatus_Unsafe(const CellPos& cellPos) const;
	KY_INLINE NavGraphToNavGraphVertex* GetNavGraphToNavGraphVertex_Unsafe(NavGraphIdxInActiveData navGraphIdxInActiveData) const;

	KyResult OpenNode(ActiveData& activeData, const NavFloorRawPtr& navFloorRawPtr);
	bool IsNodeOpen(const NavFloorRawPtr& navFloorRawPtr) const;

	KyResult OpenNode(ActiveData& activeData, const NavGraphEdgeRawPtr& navGraphEdgeRawPtr);
	bool IsNodeOpen(const NavGraphEdgeRawPtr& navGraphEdgeRawPtr) const;

	KY_INLINE bool IsInitialized() const { return m_statusInGridBase.IsInitialized(); }
public:
	StatusInGridBase m_statusInGridBase;
	KyUInt32 m_numberOfNavGraph;
};

KY_INLINE NavFloorAndNavGraphEdgeStatusInGrid::CellPosToNavFloorsStatus* NavFloorAndNavGraphEdgeStatusInGrid::AllocateCellPosToNavFloorsStatus(KyUInt32 numberOfNavCell)
{
	return (CellPosToNavFloorsStatus*)m_statusInGridBase.AllocateInBufferAndMemset(sizeof(CellPosToNavFloorsStatus), numberOfNavCell, 0xFF);
}

KY_INLINE NavFloorAndNavGraphEdgeStatusInGrid::NavGraphToNavGraphVertex* NavFloorAndNavGraphEdgeStatusInGrid::AllocateNavGraphToNavGraphVertex(KyUInt32 numberOfGraphs)
{
	return (NavGraphToNavGraphVertex*)m_statusInGridBase.AllocateInBufferAndMemset(sizeof(NavGraphToNavGraphVertex), numberOfGraphs, 0xFF);
}

KY_INLINE NavFloorAndNavGraphEdgeStatusInGrid::NavGraphVertexToOutGoingEdgeStatus* NavFloorAndNavGraphEdgeStatusInGrid::AllocateNavGraphVertexToEdgeStatus(KyUInt32 numberOfOutgoingEdges)
{
	return (NavGraphVertexToOutGoingEdgeStatus*)m_statusInGridBase.AllocateInBufferAndMemset(sizeof(NavGraphVertexToOutGoingEdgeStatus), numberOfOutgoingEdges, 0xFF);
}


KY_INLINE NavFloorAndNavGraphEdgeStatusInGrid::CellPosToNavFloorsStatus*
	NavFloorAndNavGraphEdgeStatusInGrid::GetCellPosToNavFloorsStatus_Unsafe(const CellPos& cellPos) const
{
	CellPosToNavFloorsStatus* memoryStartForGrid = (CellPosToNavFloorsStatus*)((NavGraphToNavGraphVertex*)m_statusInGridBase.GetBuffer() + m_numberOfNavGraph);
	return memoryStartForGrid + m_statusInGridBase.m_cellBox.GetRowMajorIndex(cellPos);
}

KY_INLINE NavFloorAndNavGraphEdgeStatusInGrid::NavGraphToNavGraphVertex*
	NavFloorAndNavGraphEdgeStatusInGrid::GetNavGraphToNavGraphVertex_Unsafe(NavGraphIdxInActiveData navGraphIdxInActiveData) const
{
	NavGraphToNavGraphVertex* memoryStartForGraphs = (NavGraphToNavGraphVertex*)m_statusInGridBase.GetBuffer();
	return &memoryStartForGraphs[navGraphIdxInActiveData];
}

}


#endif

