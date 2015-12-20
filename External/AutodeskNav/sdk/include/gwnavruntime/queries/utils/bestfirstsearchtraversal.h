/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BestFirstSearchTraversal_H
#define Navigation_BestFirstSearchTraversal_H

#include "gwnavruntime/querysystem/workingmemcontainers/workingmembinaryheap.h"
#include "gwnavruntime/querysystem/workingmemcontainers/trianglestatusingrid.h"
#include "gwnavruntime/queries/utils/navmeshtraversalcommon.h"
#include "gwnavruntime/queries/utils/propagationnode.h"
#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"
#include "gwnavruntime/queries/utils/queryutils.h"

namespace Kaim
{

class NavMeshElementBlob;


/*
TVisitor is a class that must have following methods :

	void ComputeTriangleCost(const NavTrianglePtr& trianglePtr, KyFloat32& cost);

	Visit(const NavTrianglePtr& trianglePtr, KyFloat32 cost, const TriangleStatusInGrid& triangleStatus)

	bool IsSearchFinished();

	bool ShouldVisitTriangle(const NavTrianglePtr& trianglePtr);

	bool ShouldVisitNeighborTriangle(const NavTrianglePtr& trianglePtr, KyUInt32 indexOfNeighborTriangle);

	NavTrianglePtr GetNeighborTriangle(const NavTrianglePtr& trianglePtr, KyUInt32 indexOfNeighborTriangle);

*/

template <class Visitor>
class BestFirstSearchTraversal
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Query)

public :
	BestFirstSearchTraversal(QueryUtils& queryUtils, const CellBox& cellBox, Visitor& visitor) :
		m_activeData(queryUtils.m_database->GetActiveData()),
		m_visitor(&visitor),
		m_openNodes(queryUtils.GetWorkingMemory()),
		m_triangleStatus(queryUtils.GetWorkingMemory(), cellBox),
		m_visitedNodes(KY_NULL) {}

	KY_INLINE void Clear()
	{
		m_openNodes.Clear();
		m_triangleStatus.MakeEmpty();
	}

	TraversalResult SetStartTriangle(const NavTriangleRawPtr& trianglePtr);
	TraversalResult Search();

	KY_INLINE void SetVisitedNodeContainer(WorkingMemArray<NavTriangleRawPtr>* visitedNodes) { m_visitedNodes = visitedNodes; }

private:
	TraversalResult AddTriangleIfNeverEncountered(const NavTriangleRawPtr& node);
	PropagationNode createNode(const NavTriangleRawPtr& node);

public :
	ActiveData* m_activeData;

	Visitor* m_visitor;          //< the visitor (= the "node analyser")

	WorkingMemBinaryHeap<PropagationNode> m_openNodes; //< open nodes = nodes that are about to be analysed

	TriangleStatusInGrid m_triangleStatus;      //< closed nodes = nodes that have been analysed
	WorkingMemArray<NavTriangleRawPtr>* m_visitedNodes;     //< nodes that have been visited
};


template <class Visitor>
TraversalResult BestFirstSearchTraversal<Visitor>::SetStartTriangle(const NavTriangleRawPtr& triangleRawPtr)
{
	if (m_visitor->ShouldVisitTriangle(triangleRawPtr) == false)
		return TraversalResult_DONE;

	if (m_openNodes.IsFull() == false)
	{
		m_openNodes.Insert(PropagationNode(triangleRawPtr));

		bool unused;
		if (m_triangleStatus.IsInitialized() && KY_SUCCEEDED(m_triangleStatus.OpenNodeIfNew(*m_activeData, triangleRawPtr, unused)))
				return TraversalResult_DONE;

		return TraversalResult_LACK_OF_MEMORY_FOR_CLOSED_NODES;
	}

	return TraversalResult_LACK_OF_MEMORY_FOR_OPEN_NODES;
}

template <class Visitor>
TraversalResult BestFirstSearchTraversal<Visitor>::Search()
{
	bool doesStoreVisited = m_visitedNodes != KY_NULL;

	//const KyUInt32 numberOfStartsNodes = m_openNodes.m_currentSize;
	//KyUInt32 currentNumberNode = 0;
	PropagationNode currentNode;

	while (!m_openNodes.IsEmpty())
	{
		m_openNodes.ExtractFirst(currentNode);

		m_visitor->Visit(currentNode.m_triangleRawPtr, currentNode.m_cost, m_triangleStatus);

		if (doesStoreVisited)
		{
			if (KY_FAILED(m_visitedNodes->PushBack(currentNode.m_triangleRawPtr)))
				return TraversalResult_LACK_OF_MEMORY_FOR_VISITED_NODES;
		}

		if (m_visitor->IsSearchFinished())
			return TraversalResult_DONE;

		if (m_visitor->ShouldVisitNeighborTriangle(currentNode.m_triangleRawPtr, 0))
		{
			const TraversalResult rc = AddTriangleIfNeverEncountered(m_visitor->GetNeighborTriangle(currentNode.m_triangleRawPtr, 0));
			if (rc != TraversalResult_DONE)
				return rc;
		}

		if (m_visitor->ShouldVisitNeighborTriangle(currentNode.m_triangleRawPtr, 1))
		{
			const TraversalResult rc = AddTriangleIfNeverEncountered(m_visitor->GetNeighborTriangle(currentNode.m_triangleRawPtr, 1));
			if (rc != TraversalResult_DONE)
				return rc;
		}

		if (m_visitor->ShouldVisitNeighborTriangle(currentNode.m_triangleRawPtr, 2))
		{
			const TraversalResult rc = AddTriangleIfNeverEncountered(m_visitor->GetNeighborTriangle(currentNode.m_triangleRawPtr, 2));
			if (rc != TraversalResult_DONE)
				return rc;
		}
	}

	return TraversalResult_DONE;
}

template <class Visitor>
TraversalResult BestFirstSearchTraversal<Visitor>::AddTriangleIfNeverEncountered(const NavTriangleRawPtr& triangleRawPtr)
{
	bool nodeIsNew;
	if (KY_FAILED(m_triangleStatus.OpenNodeIfNew(*m_activeData, triangleRawPtr, nodeIsNew)))
		return TraversalResult_LACK_OF_MEMORY_FOR_CLOSED_NODES;

	if (nodeIsNew)
	{
		if (m_openNodes.IsFull())
			return TraversalResult_LACK_OF_MEMORY_FOR_OPEN_NODES;

		m_openNodes.Insert(createNode(triangleRawPtr));
	}

	return TraversalResult_DONE;
}

template <class Visitor>
PropagationNode BestFirstSearchTraversal<Visitor>::createNode(const NavTriangleRawPtr& triangleRawPtr)
{
	PropagationNode node(triangleRawPtr);
	m_visitor->ComputeTriangleCost(triangleRawPtr, node.m_cost);
	return node;
}



}

#endif // Navigation_BestFirstSearchTraversal_H

