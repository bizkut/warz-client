/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_AbstractGraphNodeRawPtr_H
#define Navigation_AbstractGraphNodeRawPtr_H


#include "gwnavruntime/abstractgraph/abstractgraphcellgrid.h"
#include "gwnavruntime/abstractgraph/abstractgraph.h"
#include "gwnavruntime/abstractgraph/blobs/abstractgraphcellblob.h"
#include "gwnavruntime/abstractgraph/blobs/abstractgraphfloorblob.h"
#include "gwnavruntime/abstractgraph/abstractgraphtypes.h"



namespace Kaim
{

class NavHalfEdgeRawPtr;

class AbstractGraphNodeRawPtr
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	AbstractGraphNodeRawPtr() { Invalidate(); }
	AbstractGraphNodeRawPtr(AbstractGraph* abstractGraph, AbstractGraphNodeIdx nodeIdx) { Set(abstractGraph, nodeIdx); }

	KyResult InitFromNavHalfEdgeRawPtr(const NavHalfEdgeRawPtr& navHalfEdgeRawPtr);

	bool IsValid() const { return m_abstractGraph != KY_NULL && m_nodeIdx < AbstractGraphNodeIdx_Invalid; }
	void Invalidate();

	void Set(AbstractGraph* abstractGraph, AbstractGraphNodeIdx nodeIdx);

	KyFloat32 GetNeighborCost(AbstractGraphNodeIdx abstractGraphNodeIdx) const;

	AbstractGraphNodeRawPtr GetPairedAbstractGraphNodeRawPtr() const;
	const AbstractGraphNodeLink& GetAbstractGraphNodeLink() const;
	NavFloorIdx GetOriginalNavFloorIdx() const;
	CellPos GetCellPos() const;
	CardinalDir GetCellBoundary() const;

	AbstractGraphCellGrid* GetAbstractGraphCellGrid() const;
public:
	AbstractGraph* m_abstractGraph;
	AbstractGraphNodeIdx m_nodeIdx;
};



KY_INLINE void AbstractGraphNodeRawPtr::Invalidate()
{
	m_abstractGraph = KY_NULL;
	m_nodeIdx = AbstractGraphNodeIdx_Invalid;
}

KY_INLINE void AbstractGraphNodeRawPtr::Set(AbstractGraph* abstractGraph, AbstractGraphNodeIdx nodeIdx)
{
	m_abstractGraph = abstractGraph;
	m_nodeIdx = nodeIdx;
}

KY_INLINE KyFloat32 AbstractGraphNodeRawPtr::GetNeighborCost(AbstractGraphNodeIdx abstractGraphNodeIdx) const  { return m_abstractGraph->GetNeighborCost(m_nodeIdx, abstractGraphNodeIdx); }

KY_INLINE const AbstractGraphNodeLink& AbstractGraphNodeRawPtr::GetAbstractGraphNodeLink() const { return m_abstractGraph->m_links[m_nodeIdx]; }
KY_INLINE AbstractGraphNodeRawPtr AbstractGraphNodeRawPtr::GetPairedAbstractGraphNodeRawPtr() const 
{
	const LoadedAbstractGraphNodeIdx& pairedNodeIdx = GetAbstractGraphNodeLink().m_pairedNodeIdx;
	if (pairedNodeIdx.IsValid() == false)
		return AbstractGraphNodeRawPtr();

	AbstractGraphNodeRawPtr pairedNode;
	pairedNode.m_abstractGraph = GetAbstractGraphCellGrid()->GetAbstractGraph(pairedNodeIdx);
	pairedNode.m_nodeIdx = pairedNodeIdx.m_nodeIdx;
	return pairedNode;
}

KY_INLINE AbstractGraphCellGrid* AbstractGraphNodeRawPtr::GetAbstractGraphCellGrid() const { return m_abstractGraph->GetAbstractGraphCellGrid(); }

KY_INLINE NavFloorIdx AbstractGraphNodeRawPtr::GetOriginalNavFloorIdx() const
{
	const AbstractGraphCellFloorIndices& indices = m_abstractGraph->m_blob->m_graphNodeIdxToGraphCellFloorIndices.GetValues()[m_nodeIdx];
	const AbstractGraphCellBlob* abstractCellBlob = m_abstractGraph->m_blob->m_abstractCells.GetValues()[indices.m_abstractCellIdx].Ptr();
	return abstractCellBlob->m_abstractFloors.GetValues()[indices.m_abstractFloorIdx].Ptr()->m_originalNavFloorIdx;
}

KY_INLINE CellPos AbstractGraphNodeRawPtr::GetCellPos() const
{
	const AbstractGraphCellFloorIndices& indices = m_abstractGraph->m_blob->m_graphNodeIdxToGraphCellFloorIndices.GetValues()[m_nodeIdx];
	const AbstractGraphCellBlob* abstractCellBlob = m_abstractGraph->m_blob->m_abstractCells.GetValues()[indices.m_abstractCellIdx].Ptr();
	return abstractCellBlob->m_cellPos;
}

KY_INLINE CardinalDir AbstractGraphNodeRawPtr::GetCellBoundary() const
{
	const AbstractGraphCellFloorIndices& indices = m_abstractGraph->m_blob->m_graphNodeIdxToGraphCellFloorIndices.GetValues()[m_nodeIdx];
	const AbstractGraphCellBlob* abstractCellBlob = m_abstractGraph->m_blob->m_abstractCells.GetValues()[indices.m_abstractCellIdx].Ptr();
	const AbstractGraphFloorBlob* abstractFloorBlob = abstractCellBlob->m_abstractFloors.GetValues()[indices.m_abstractFloorIdx].Ptr();
	return abstractFloorBlob->GetCellBoundaryDir(m_nodeIdx);
}


}

#endif
