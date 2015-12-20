/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_AStarQueryContext_H
#define Navigation_AStarQueryContext_H

#include "gwnavruntime/querysystem/workingmemcontainers/astarnodeindexingrid.h"
#include "gwnavruntime/abstractgraph/identifiers/abstractgraphnoderawptr.h"

namespace Kaim
{
class QueryUtils;

/* Class for BinaryHeap to update index in binaryHeap assuming index is T::m_indexInBinaryHeap. */
class AstarBinHeapIndexTracker
{
public:
	AstarBinHeapIndexTracker() : m_astarNodes(KY_NULL) {}
	void SetAstarNodeArray(WorkingMemArray<AStarNode>& astarNodeArray);

	void OnAdd(AStarNodeIndex& aStarNodeIndex, KyUInt32 indexInBinaryHeap);
	void OnRemove(AStarNodeIndex& aStarNodeIndex);
	void OnSwap(AStarNodeIndex& lhs, AStarNodeIndex& rhs);

	WorkingMemArray<AStarNode>* m_astarNodes;
};

class AStarNodeComparator
{
public:
	AStarNodeComparator() : m_astarNodes(KY_NULL) {}
	void SetAstarNodeArray(WorkingMemArray<AStarNode>& astarNodeArray);

	bool operator () (const AStarNodeIndex lhsIdx, const AStarNodeIndex rhsIdx) const;

	WorkingMemArray<AStarNode>* m_astarNodes;
};


typedef WorkingMemBinaryHeap<AStarNodeIndex, AStarNodeComparator, AstarBinHeapIndexTracker> AstarTraversalBinHeap;


class AStarTraversalContext
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_QueryWorkingMem)
public:
	AStarTraversalContext() {}

	KyResult Init(QueryUtils& queryUtils);
	void ReleaseWorkingMemory();

	KyResult CheckTraversalBinaryHeapMemory();
	KyResult CheckAstarNodeArrayMemory();
	KyResult CheckNavHalfEdgeRawPtrArrayMemory();
	KyResult CheckNavGraphVertexRawPtrArrayMemory();
	KyResult CheckAbstractGraphNodeRawPtrArrayMemory();

	KyResult InsertOrUpdateInBinHeapTraversal(AStarNode& neighborNode, AStarNodeIndex neighborNodeIndex);

	AstarNodeIndexInGrid                     m_edgeIndexGrid;
	AstarTraversalBinHeap                    m_traversalBinHeap;
	WorkingMemArray<AStarNode>               m_aStarNodes;
	WorkingMemArray<NavHalfEdgeRawPtr>       m_edgeRawPtrNodes;
	WorkingMemArray<NavGraphVertexRawPtr>    m_vertexRawPtrNodes;
	WorkingMemArray<AbstractGraphNodeRawPtr> m_abstractRawPtrNodes;
};



KY_INLINE void AstarBinHeapIndexTracker::SetAstarNodeArray(WorkingMemArray<AStarNode>& astarNodeArray) { m_astarNodes = &astarNodeArray; }
KY_INLINE void AstarBinHeapIndexTracker::OnAdd(AStarNodeIndex& aStarNodeIndex, KyUInt32 indexInBinaryHeap)
{
	AStarNode& aStarNode = (*m_astarNodes)[aStarNodeIndex];
	aStarNode.m_indexInBinaryHeap = (IndexInBinHeap)indexInBinaryHeap;
}
// Called before the element has been removed.
KY_INLINE void AstarBinHeapIndexTracker::OnRemove(AStarNodeIndex& aStarNodeIndex)
{
	AStarNode& aStarNode = (*m_astarNodes)[aStarNodeIndex];
	aStarNode.m_indexInBinaryHeap = IndexInBinHeap_Removed;
}
// Called after the elements have been swapped.
KY_INLINE void AstarBinHeapIndexTracker::OnSwap(AStarNodeIndex& lhs, AStarNodeIndex& rhs)
{
	AStarNode& aStarNode1 = (*m_astarNodes)[lhs];
	AStarNode& aStarNode2 = (*m_astarNodes)[rhs];
	Alg::Swap(aStarNode1.m_indexInBinaryHeap, aStarNode2.m_indexInBinaryHeap);
}

KY_INLINE void AStarNodeComparator::SetAstarNodeArray(WorkingMemArray<AStarNode>& astarNodeArray) { m_astarNodes = &astarNodeArray; }
KY_INLINE bool AStarNodeComparator::operator () (const AStarNodeIndex lhsIdx, const AStarNodeIndex rhsIdx) const
{
	AStarNode& lhs = (*m_astarNodes)[lhsIdx];
	AStarNode& rhs = (*m_astarNodes)[rhsIdx];
	return lhs.m_costFromStart + lhs.m_estimatedCostToDest < rhs.m_costFromStart  + rhs.m_estimatedCostToDest;
}


KY_INLINE KyResult AStarTraversalContext::CheckTraversalBinaryHeapMemory()
{
	if (m_traversalBinHeap.IsFull() == false)
		return KY_SUCCESS;

	KY_LOG_WARNING( ("This traversal has reached its maximum working memory size for its BinaryHeap"));
	return KY_ERROR;
}

KY_INLINE KyResult AStarTraversalContext::CheckAstarNodeArrayMemory()
{
	if (m_aStarNodes.IsFull() == false)
		return KY_SUCCESS;

	KY_LOG_WARNING(("This traversal has reached its maximum working memory size for storing astarNodes"));
	return KY_ERROR;
}

KY_INLINE KyResult AStarTraversalContext::CheckNavHalfEdgeRawPtrArrayMemory()
{
	if (m_edgeRawPtrNodes.IsFull() == false)
		return KY_SUCCESS;

	KY_LOG_WARNING(("This traversal has reached its maximum working memory size for storing astarNodes and corresponding NavHalfEdgeRawPtr"));
	return KY_ERROR;
}

KY_INLINE KyResult AStarTraversalContext::CheckNavGraphVertexRawPtrArrayMemory()
{
	if (m_vertexRawPtrNodes.IsFull() == false)
		return KY_SUCCESS;

	KY_LOG_WARNING(("This traversal has reached its maximum working memory size for storing astarNodes and corresponding NavGraphVertexRawPtr"));
	return KY_ERROR;
}

KY_INLINE KyResult AStarTraversalContext::CheckAbstractGraphNodeRawPtrArrayMemory()
{
	if (m_abstractRawPtrNodes.IsFull() == false)
		return KY_SUCCESS;

	KY_LOG_WARNING(("This traversal has reached its maximum working memory size for storing astarNodes and corresponding AbstractGraphNodeRawPtr"));
	return KY_ERROR;
}


KY_INLINE KyResult AStarTraversalContext::InsertOrUpdateInBinHeapTraversal(AStarNode& neighborNode, AStarNodeIndex neighborNodeIndex)
{
	if (neighborNode.m_indexInBinaryHeap < IndexInBinHeap_Removed)
	{
		// (the node is open, already present in the binary heap), we compute its new position in the BinaryHeap
		m_traversalBinHeap.UpdateAt(neighborNode.m_indexInBinaryHeap);
	}
	else
	{
		// the node has been removed from the BinaryHeap, we add it again

		// check for memory to insert a newElement in the binary heap
		KY_FORWARD_ERROR_NO_LOG(CheckTraversalBinaryHeapMemory());

		m_traversalBinHeap.Insert(neighborNodeIndex);
	}

	return KY_SUCCESS;
}
}


#endif //Navigation_AStarTypes_H

