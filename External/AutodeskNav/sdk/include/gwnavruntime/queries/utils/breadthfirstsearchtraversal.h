/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BreadthFirstSearchTraversal_H
#define Navigation_BreadthFirstSearchTraversal_H

#include "gwnavruntime/querysystem/workingmemcontainers/workingmemdeque.h"
#include "gwnavruntime/querysystem/workingmemcontainers/workingmemarray.h"
#include "gwnavruntime/querysystem/workingmemcontainers/trianglestatusingrid.h"
#include "gwnavruntime/queries/utils/navmeshtraversalcommon.h"
#include "gwnavruntime/queries/utils/queryutils.h"
#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"


namespace Kaim
{

class NavMeshElementBlob;

/*
Visitor is a class that must have following methods :

	Visit(const NavTrianglePtr& trianglePtr, const TriangleStatusInGrid& triangleStatus)

	bool IsSearchFinished();
	bool ShouldVisitTriangle(const NavTrianglePtr& trianglePtr);
	bool ShouldVisitNeighborTriangle(const NavTrianglePtr& trianglePtr, KyUInt32 indexOfNeighborTriangle);
	NavTrianglePtr GetNeighborTriangle(const NavTrianglePtr& trianglePtr, KyUInt32 indexOfNeighborTriangle);

*/
template <class Visitor>
class BreadthFirstSearchTraversal
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Query)

public :
	BreadthFirstSearchTraversal(
		QueryUtils& queryUtils, const CellBox& cellBox, Visitor& visitor) :
			m_activeData(queryUtils.m_database->GetActiveData()),
			m_visitor(&visitor),
			m_openNodes(queryUtils.GetWorkingMemory()),
			m_triangleStatus(queryUtils.GetWorkingMemory(), cellBox),
			m_visitedNodes(KY_NULL) {}

	void Clear()
	{
		m_openNodes.MakeEmpty();
		m_triangleStatus.MakeEmpty();
	}

	// the navTag of the startTriangle is not tested
	inline TraversalResult SetStartTriangle(const NavTriangleRawPtr& triangleRawPtr);

	TraversalResult AddTriangleIfNeverEncountered(const NavTriangleRawPtr& triangleRawPtr);

	TraversalResult Search();

	KY_INLINE void SetVisitedNodeContainer(WorkingMemArray<NavTriangleRawPtr>* visitedNodes) { m_visitedNodes = visitedNodes; }

public :
	ActiveData* m_activeData;

	Visitor* m_visitor;                     //< the visitor (= the "node analyser")

	WorkingMemDeque<NavTriangleRawPtr> m_openNodes;    //< open nodes = nodes that are about to be analysed

	TriangleStatusInGrid m_triangleStatus;  //< closed nodes = nodes that have been analysed
	WorkingMemArray<NavTriangleRawPtr>* m_visitedNodes;     //< nodes that have been visited
};

template <class Visitor>
TraversalResult BreadthFirstSearchTraversal<Visitor>:: SetStartTriangle(const NavTriangleRawPtr& triangleRawPtr)
{
	if (m_visitor->ShouldVisitTriangle(triangleRawPtr) == false)
		return TraversalResult_DONE;

	if (KY_FAILED(m_openNodes.PushBack(triangleRawPtr)))
		return TraversalResult_LACK_OF_MEMORY_FOR_OPEN_NODES;

	bool unused;
	if (m_triangleStatus.IsInitialized() && KY_SUCCEEDED(m_triangleStatus.OpenNodeIfNew(*m_activeData, triangleRawPtr, unused)))
		return TraversalResult_DONE;

	return TraversalResult_LACK_OF_MEMORY_FOR_CLOSED_NODES;
}

template <class Visitor>
TraversalResult BreadthFirstSearchTraversal<Visitor>::Search()
{
	bool doesStoreVisited = m_visitedNodes != KY_NULL;

	NavTriangleRawPtr currentTrianglerRawPtr;

	while (!m_openNodes.IsEmpty())
	{
		m_openNodes.Front(currentTrianglerRawPtr);
		m_openNodes.PopFront();

		m_visitor->Visit(currentTrianglerRawPtr, m_triangleStatus);

		if (doesStoreVisited)
		{
			if (KY_FAILED(m_visitedNodes->PushBack(currentTrianglerRawPtr)))
				return TraversalResult_LACK_OF_MEMORY_FOR_VISITED_NODES;
		}

		if (m_visitor->IsSearchFinished())
			return TraversalResult_DONE;

		if (m_visitor->ShouldVisitNeighborTriangle(currentTrianglerRawPtr, 0))
		{
			const TraversalResult rc = AddTriangleIfNeverEncountered(m_visitor->GetNeighborTriangle(currentTrianglerRawPtr, 0));
			if (rc != TraversalResult_DONE)
				return rc;
		}

		if (m_visitor->ShouldVisitNeighborTriangle(currentTrianglerRawPtr, 1))
		{
			const TraversalResult rc = AddTriangleIfNeverEncountered(m_visitor->GetNeighborTriangle(currentTrianglerRawPtr, 1));
			if (rc != TraversalResult_DONE)
				return rc;
		}

		if (m_visitor->ShouldVisitNeighborTriangle(currentTrianglerRawPtr, 2))
		{
			const TraversalResult rc = AddTriangleIfNeverEncountered(m_visitor->GetNeighborTriangle(currentTrianglerRawPtr, 2));
			if (rc != TraversalResult_DONE)
				return rc;
		}
	}

	return TraversalResult_DONE;
}

template <class Visitor>
TraversalResult BreadthFirstSearchTraversal<Visitor>::AddTriangleIfNeverEncountered(const NavTriangleRawPtr& triangleRawPtr)
{
	bool nodeIsNew;
	if (KY_FAILED(m_triangleStatus.OpenNodeIfNew(*m_activeData, triangleRawPtr, nodeIsNew)))
		return TraversalResult_LACK_OF_MEMORY_FOR_CLOSED_NODES;

	if (nodeIsNew && KY_FAILED(m_openNodes.PushBack(triangleRawPtr)))
		return TraversalResult_LACK_OF_MEMORY_FOR_OPEN_NODES;

	return TraversalResult_DONE;
}

}

#endif // Navigation_BreadthFirstSearchTraversal_H

