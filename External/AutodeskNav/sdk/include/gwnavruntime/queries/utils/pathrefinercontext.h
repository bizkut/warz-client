/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_PathRefinerContext_H
#define Navigation_PathRefinerContext_H

#include "gwnavruntime/querysystem/workingmemcontainers/navdatachangeindexingrid.h"

namespace Kaim
{

typedef KyUInt16 RefinerNodeIndex;
static const RefinerNodeIndex RefinerNodeIndex_Invalid = KyUInt16MAXVAL;

class RefinerNode
{
public:
	RefinerNode();
	RefinerNode(const Vec3f& pos, NodeTypeAndRawPtrDataIdx nodeTypeAndRawPtrDataIdx, RefinerNodeIndex predecessorIdx, RefinerNodeIndex nextNodeIdx);


	PathNodeType GetNodeType()    const;
	KyUInt32 GetIdxOfRawPtrData() const;

	void SetNodeType(PathNodeType nodeType);
	void SetIdxOfRawPtrData(KyUInt32 indexOfRawPtrData);

	Vec3f m_nodePosition;
	WorldIntegerPos m_nodeIntegerPos;
	KyFloat32 m_refinerCost;
	KyFloat32 m_costFromPredecessor;
	NodeTypeAndRawPtrDataIdx m_nodeTypeAndRawPtrDataIdx; // 32 bits
	RefinerNodeIndex m_predecessorNodeIdx; // 16 bits
	RefinerNodeIndex m_nextNodeIdx; // 16bits
	IndexInBinHeap m_indexInBinaryHeap; // 16bits
};

class RefinerBinHeapIndexTracker
{
public:
	RefinerBinHeapIndexTracker() : m_refinerNodes(KY_NULL) {}

	void SetRefinerNodeArray(WorkingMemArray<RefinerNode>& refinerNodes);
	// Called after the element has been added.
	void OnAdd(RefinerNodeIndex refinerNodeIndex, KyUInt32 indexInBinaryHeap);
	// Called before the element has been removed.
	void OnRemove(RefinerNodeIndex refinerNodeIndex);
	// Called after the elements has been swapped.
	void OnSwap(RefinerNodeIndex lhs, RefinerNodeIndex rhs);

	WorkingMemArray<RefinerNode>* m_refinerNodes;
};

class RefinerNodeComparator
{
public:
	RefinerNodeComparator() : m_refinerNodes(KY_NULL) {}
	void SetRefinerNodeArray(WorkingMemArray<RefinerNode>& refinerNodes);
	bool operator () (const RefinerNodeIndex lhsIdx, const RefinerNodeIndex rhsIdx) const;

	WorkingMemArray<RefinerNode>* m_refinerNodes;
};

typedef WorkingMemBinaryHeap<RefinerNodeIndex, RefinerNodeComparator, RefinerBinHeapIndexTracker> RefinerBinHeap;

class AStarTraversalContext;
class PathRefinerContext
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_QueryWorkingMem)
public:
	PathRefinerContext() : m_currentNodeIdx(RefinerNodeIndex_Invalid) {}
	~PathRefinerContext() { ReleaseWorkingMemory(); }

	KyResult InitFromAstarTraversalContext(WorkingMemory* workingMemory, AStarTraversalContext* astarContext,
		AStarNodeIndex destinationNodeIdx, bool doClearAStarTraversalContext = true);

	void ReleaseWorkingMemory();


	RefinerBinHeap                        m_refinerBinHeap;
	WorkingMemArray<RefinerNode>          m_refinerNodes;
	WorkingMemArray<NavTriangleRawPtr>    m_triangleRawPtrNodes;
	WorkingMemArray<NavGraphVertexRawPtr> m_vertexRawPtrNodes;

	RefinerNodeIndex m_currentNodeIdx;
};


KY_INLINE RefinerNode::RefinerNode() :
	m_refinerCost(KyFloat32MAXVAL),
	m_costFromPredecessor(KyFloat32MAXVAL),
	m_predecessorNodeIdx(RefinerNodeIndex_Invalid),
	m_nextNodeIdx(RefinerNodeIndex_Invalid),
	m_indexInBinaryHeap(IndexInBinHeap_UnSet)
{}

KY_INLINE RefinerNode::RefinerNode(const Vec3f& pos, NodeTypeAndRawPtrDataIdx nodeTypeAndRawPtrDataIdx, RefinerNodeIndex predecessorIdx, RefinerNodeIndex nextNodeIdx) :
	m_nodePosition(pos),
	m_refinerCost(KyFloat32MAXVAL),
	m_costFromPredecessor(KyFloat32MAXVAL),
	m_nodeTypeAndRawPtrDataIdx(nodeTypeAndRawPtrDataIdx),
	m_predecessorNodeIdx(predecessorIdx),
	m_nextNodeIdx(nextNodeIdx),
	m_indexInBinaryHeap(IndexInBinHeap_UnSet)
{}

KY_INLINE PathNodeType RefinerNode:: GetNodeType()    const { return m_nodeTypeAndRawPtrDataIdx.GetNodeType();        }
KY_INLINE KyUInt32 RefinerNode::GetIdxOfRawPtrData()  const { return m_nodeTypeAndRawPtrDataIdx.GetIdxOfRawPtrData(); }
KY_INLINE void RefinerNode::SetNodeType(PathNodeType nodeType)             { m_nodeTypeAndRawPtrDataIdx.SetNodeType(nodeType);                 }
KY_INLINE void RefinerNode::SetIdxOfRawPtrData(KyUInt32 indexOfRawPtrData) { m_nodeTypeAndRawPtrDataIdx.SetIdxOfRawPtrData(indexOfRawPtrData); }


KY_INLINE void RefinerBinHeapIndexTracker::SetRefinerNodeArray(WorkingMemArray<RefinerNode>& refinerNodes) { m_refinerNodes = &refinerNodes; }
KY_INLINE void RefinerBinHeapIndexTracker::OnAdd(RefinerNodeIndex refinerNodeIndex, KyUInt32 indexInBinaryHeap)
{
	RefinerNode& node = (*m_refinerNodes)[refinerNodeIndex];
	node.m_indexInBinaryHeap = (IndexInBinHeap)indexInBinaryHeap;
}
KY_INLINE void RefinerBinHeapIndexTracker::OnRemove(RefinerNodeIndex refinerNodeIndex)
{
	RefinerNode& node = (*m_refinerNodes)[refinerNodeIndex];
	node.m_indexInBinaryHeap = IndexInBinHeap_UnSet;
}
KY_INLINE void RefinerBinHeapIndexTracker::OnSwap(RefinerNodeIndex lhs, RefinerNodeIndex rhs)
{
	RefinerNode& node1 = (*m_refinerNodes)[lhs];
	RefinerNode& node2 = (*m_refinerNodes)[rhs];
	Alg::Swap(node1.m_indexInBinaryHeap, node2.m_indexInBinaryHeap);
}

KY_INLINE void RefinerNodeComparator::SetRefinerNodeArray(WorkingMemArray<RefinerNode>& refinerNodes) { m_refinerNodes = &refinerNodes; }
KY_INLINE bool RefinerNodeComparator::operator() (const RefinerNodeIndex lhsIdx, const RefinerNodeIndex rhsIdx) const
{
	RefinerNode& lhs = (*m_refinerNodes)[lhsIdx];
	RefinerNode& rhs = (*m_refinerNodes)[rhsIdx];
	return lhs.m_refinerCost < rhs.m_refinerCost;
}

KY_INLINE void PathRefinerContext::ReleaseWorkingMemory()
{
	m_triangleRawPtrNodes.ReleaseWorkingMemoryBuffer();
	m_vertexRawPtrNodes.ReleaseWorkingMemoryBuffer();
	m_refinerBinHeap.ReleaseWorkingMemoryBuffer();
	m_refinerNodes.ReleaseWorkingMemoryBuffer();
}

}


#endif //Navigation_PathRefinerContext_H

