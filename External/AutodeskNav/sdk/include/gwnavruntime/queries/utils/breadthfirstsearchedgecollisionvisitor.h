/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BreadthFirstSearchEdgeCollisionVisitor_H
#define Navigation_BreadthFirstSearchEdgeCollisionVisitor_H

#include "gwnavruntime/querysystem/workingmemcontainers/trianglestatusingrid.h"

namespace Kaim
{

/*
class BreadthFirstSearchEdgeCollisionVisitor

EdgeIntersector must have the following function :

	bool DoesIntersectEdge(const Vec3f& startEdgePos, const Vec3f& endEdgePos)
*/
template<class TraverseLogic, class EdgeIntersector>
class BreadthFirstSearchEdgeCollisionVisitor
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Query)

public:
	BreadthFirstSearchEdgeCollisionVisitor(void* traverseLogicUserData, EdgeIntersector& edgeIntersector);

	bool IsSearchFinished();
	bool ShouldVisitNeighborTriangle(const NavTriangleRawPtr& triangleRawPtr, KyUInt32 indexOfNeighborTriangle);
	bool ShouldVisitTriangle(const NavTriangleRawPtr& triangleRawPtr);

	NavTriangleRawPtr GetNeighborTriangle(const NavTriangleRawPtr& triangleRawPtr, KyUInt32 indexOfNeighborTriangle);

	void Visit(const NavTriangleRawPtr& triangleRawPtr, const TriangleStatusInGrid& triangleStatus);

public:
	EdgeIntersector* m_edgeIntersector;
	void* m_traverseLogicUserData;
	bool m_collisionFound;

private:
	bool m_shouldVisitNeighborTriangle[3];
	NavTriangleRawPtr m_neighborTriangle[3];
};

template<class TraverseLogic, class EdgeIntersector>
KY_INLINE BreadthFirstSearchEdgeCollisionVisitor<TraverseLogic, EdgeIntersector>::
	BreadthFirstSearchEdgeCollisionVisitor(void* traverseLogicUserData, EdgeIntersector& edgeIntersector) :
	m_edgeIntersector(&edgeIntersector),
	m_traverseLogicUserData(traverseLogicUserData),
	m_collisionFound(false)
{
	m_shouldVisitNeighborTriangle[0] = true;
	m_shouldVisitNeighborTriangle[1] = true;
	m_shouldVisitNeighborTriangle[2] = true;
}

template<class TraverseLogic, class EdgeIntersector>
KY_INLINE bool BreadthFirstSearchEdgeCollisionVisitor<TraverseLogic, EdgeIntersector>::IsSearchFinished() { return m_collisionFound; }
template<class TraverseLogic, class EdgeIntersector>
KY_INLINE bool BreadthFirstSearchEdgeCollisionVisitor<TraverseLogic, EdgeIntersector>::ShouldVisitNeighborTriangle(const NavTriangleRawPtr& /*triangleRawPtr*/, KyUInt32 indexOfNeighborTriangle)
{
	return m_shouldVisitNeighborTriangle[indexOfNeighborTriangle];
}

template<class TraverseLogic, class EdgeIntersector>
KY_INLINE bool BreadthFirstSearchEdgeCollisionVisitor<TraverseLogic, EdgeIntersector>::ShouldVisitTriangle(const NavTriangleRawPtr& /*triangleRawPtr*/) { return true; }
template<class TraverseLogic, class EdgeIntersector>
KY_INLINE NavTriangleRawPtr BreadthFirstSearchEdgeCollisionVisitor<TraverseLogic, EdgeIntersector>::GetNeighborTriangle(const NavTriangleRawPtr& /*triangleRawPtr*/, KyUInt32 indexOfNeighborTriangle)
{
	return m_neighborTriangle[indexOfNeighborTriangle];
}

template<class TraverseLogic, class EdgeIntersector>
void BreadthFirstSearchEdgeCollisionVisitor<TraverseLogic, EdgeIntersector>::Visit(const NavTriangleRawPtr& triangleRawPtr,
	const TriangleStatusInGrid& triangleStatus)
{
	CoordPos64 v0, v1, v2;
	CoordPos64* vertex[4] = { &v0, &v1, &v2, &v0};
	NavHalfEdgeRawPtr halfEdgeRawPtr[3];
	NavHalfEdge halfEdge[3];
	bool isACrossableEdges[3];
	bool neighborTriangleAlreadyVisited[3];
	bool doesIntersectsEdges[3];

	const NavHalfEdgeIdx firstNavHalfEdgeIdx = NavFloorBlob::NavTriangleIdxToFirstNavHalfEdgeIdx(triangleRawPtr.GetTriangleIdx());
	NavFloor* navFloor = triangleRawPtr.GetNavFloor();

	for(KyUInt32 i = 0; i < 3; ++i)
		halfEdgeRawPtr[i].Set(navFloor, firstNavHalfEdgeIdx + i);

	triangleRawPtr.GetVerticesCoordPos64(v0, v1, v2);

	const NavFloorBlob& navFloorBlob = *navFloor->GetNavFloorBlob();
	const NavHalfEdge* m_navHalfEdges = navFloorBlob.m_navHalfEdges.GetValues();

	for(KyUInt32 i = 0; i < 3; ++i)
		halfEdge[i] = m_navHalfEdges[firstNavHalfEdgeIdx + i];

	for(KyUInt32 i = 0; i < 3; ++i)
	{
		NavHalfEdgeRawPtr pairHalfEdgeRawPtr;
		isACrossableEdges[i] = halfEdgeRawPtr[i].IsHalfEdgeCrossable<TraverseLogic>(m_traverseLogicUserData, halfEdge[i], &navFloorBlob, pairHalfEdgeRawPtr);

		if (isACrossableEdges[i])
		{
			bool inSameFloor = halfEdge[i].GetHalfEdgeType() == EDGETYPE_PAIRED || halfEdge[i].GetHalfEdgeType() == EDGETYPE_CONNEXBOUNDARY;
			m_neighborTriangle[i].Set(pairHalfEdgeRawPtr.GetNavFloor(), NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(pairHalfEdgeRawPtr.GetHalfEdgeIdx()));

			if (inSameFloor)
				neighborTriangleAlreadyVisited[i] = triangleStatus.IsTriangleOpen_Unsafe(m_neighborTriangle[i]);
			else
				neighborTriangleAlreadyVisited[i] = triangleStatus.IsTriangleOpen(m_neighborTriangle[i]);
		}
		else
		{
			neighborTriangleAlreadyVisited[i] = false;
		}
	}

	for(KyUInt32 i = 0; i < 3; ++i)
		doesIntersectsEdges[i] = neighborTriangleAlreadyVisited[i] || m_edgeIntersector->DoesIntersectEdge(*vertex[i], *vertex[i+1]);

	m_collisionFound =
		(isACrossableEdges[0] == false && doesIntersectsEdges[0]) ||
		(isACrossableEdges[1] == false && doesIntersectsEdges[1]) ||
		(isACrossableEdges[2] == false && doesIntersectsEdges[2]);

	for(KyUInt32 i = 0; i < 3; ++i)
		m_shouldVisitNeighborTriangle[i] = isACrossableEdges[i] && neighborTriangleAlreadyVisited[i] == false && doesIntersectsEdges[i];
}

}


#endif //Navigation_BreadthFirstSearchEdgeCollisionVisitor_H

