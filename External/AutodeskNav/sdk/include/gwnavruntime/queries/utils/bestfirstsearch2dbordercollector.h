/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BestFirstSearch2dBorderCollector_H
#define Navigation_BestFirstSearch2dBorderCollector_H

#include "gwnavruntime/querysystem/workingmemcontainers/trianglestatusingrid.h"

namespace Kaim
{

/*
class BestFirstSearch2dBorderCollector

EdgeIntersector must have the following functions :

	bool DoesIntersectEdge(const Vec3f& startEdgePos, const Vec3f& endEdgePos);

	void ComputeCollisionPosOnEdge(const Vec3f& startEdgePos, const Vec3f& endEdgePos, Vec3f& collisionPos, KyFloat32& squareDistToCollisionPos)

	void ComputeTriangleCost(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2, KyFloat32& cost)

*/
template<class TraverseLogic, class EdgeIntersector>
class BestFirstSearch2dBorderCollector
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Query)

public:
	BestFirstSearch2dBorderCollector(void* traverseLogicUserData, EdgeIntersector& edgeIntersector);

	bool IsSearchFinished();

	bool ShouldVisitNeighborTriangle(const NavTriangleRawPtr& triangleRawPtr, KyUInt32 indexOfNeighborTriangle);
	bool ShouldVisitTriangle(const NavTriangleRawPtr& triangleRawPtr);
	void ComputeTriangleCost(const NavTriangleRawPtr& triangleRawPtr, KyFloat32& cost);

	NavTriangleRawPtr GetNeighborTriangle(const NavTriangleRawPtr& triangleRawPtr, KyUInt32 indexOfNeighborTriangle);

	void Visit(const NavTriangleRawPtr& triangleRawPtr, KyFloat32 cost, const TriangleStatusInGrid& triangleStatus);

public:
	EdgeIntersector* m_edgeIntersector;
	void* m_traverseLogicUserData;
	bool m_collisionFound;

	KyFloat32 m_squareDistToCollisionPos;
	Vec3f m_collisionPos;
	NavHalfEdgeRawPtr m_intersectedHalfEdgeRawPtr;

private:
	bool m_shouldVisitNeighborTriangle[3];
	NavTriangleRawPtr m_neighborTriangle[3];
};

template<class TraverseLogic, class EdgeIntersector>
KY_INLINE BestFirstSearch2dBorderCollector<TraverseLogic, EdgeIntersector>::BestFirstSearch2dBorderCollector(void* traverseLogicUserData, EdgeIntersector& edgeIntersector) :
m_edgeIntersector(&edgeIntersector), m_traverseLogicUserData(traverseLogicUserData), m_collisionFound(false),
	m_squareDistToCollisionPos(KyFloat32MAXVAL)
{
	m_shouldVisitNeighborTriangle[0] = true;
	m_shouldVisitNeighborTriangle[1] = true;
	m_shouldVisitNeighborTriangle[2] = true;
}

template<class TraverseLogic, class EdgeIntersector>
KY_INLINE bool BestFirstSearch2dBorderCollector<TraverseLogic, EdgeIntersector>::IsSearchFinished() { return false; }

template<class TraverseLogic, class EdgeIntersector>
KY_INLINE bool BestFirstSearch2dBorderCollector<TraverseLogic, EdgeIntersector>::ShouldVisitNeighborTriangle(const NavTriangleRawPtr& /*triangleRawPtr*/, KyUInt32 indexOfNeighborTriangle)
{
	return m_shouldVisitNeighborTriangle[indexOfNeighborTriangle];
}

template<class TraverseLogic, class EdgeIntersector>
KY_INLINE bool BestFirstSearch2dBorderCollector<TraverseLogic, EdgeIntersector>::ShouldVisitTriangle(const NavTriangleRawPtr& /*triangleRawPtr*/) { return true; }

template<class TraverseLogic, class EdgeIntersector>
KY_INLINE NavTriangleRawPtr BestFirstSearch2dBorderCollector<TraverseLogic, EdgeIntersector>::GetNeighborTriangle(const NavTriangleRawPtr& /*triangleRawPtr*/, KyUInt32 indexOfNeighborTriangle)
{
	return m_neighborTriangle[indexOfNeighborTriangle];
}

template<class TraverseLogic, class EdgeIntersector>
void BestFirstSearch2dBorderCollector<TraverseLogic, EdgeIntersector>::ComputeTriangleCost(const NavTriangleRawPtr& triangleRawPtr, KyFloat32& cost)
{
	CoordPos64 v0, v1, v2;
	triangleRawPtr.GetVerticesCoordPos64(v0, v1, v2);
	m_edgeIntersector->ComputeTriangleCost(v0, v1, v2, cost);
}

template<class TraverseLogic, class EdgeIntersector>
void BestFirstSearch2dBorderCollector<TraverseLogic, EdgeIntersector>::Visit(const NavTriangleRawPtr& triangleRawPtr, const KyFloat32 /*triangleCost*/,
	const TriangleStatusInGrid& triangleStatus)
{
	CoordPos64 v0, v1, v2;
	CoordPos64* vertex[4] = { &v0, &v1, &v2, &v0};
	NavHalfEdgeRawPtr halfEdgeRawPtr[3];
	NavHalfEdge halfEdge[3];
	bool isACrossableEdges[3];
	bool isVisibleEdges[3];
	bool neighborTriangleAlreadyVisited[3];

	const NavHalfEdgeIdx firstNavHalfEdgeIdx = NavFloorBlob::NavTriangleIdxToFirstNavHalfEdgeIdx(triangleRawPtr.GetTriangleIdx());
	NavFloor* navFloor = triangleRawPtr.GetNavFloor();

	for(KyUInt32 i = 0; i < 3; ++i)
		halfEdgeRawPtr[i].Set(navFloor, firstNavHalfEdgeIdx + i);

	triangleRawPtr.GetVerticesCoordPos64(v0, v1, v2);

	const NavFloorBlob& navFloorBlob = *navFloor->GetNavFloorBlob();
	const NavHalfEdge* m_navHalfEdges = navFloorBlob.m_navHalfEdges.GetValues();

	for(KyUInt32 i = 0; i < 3; ++i)
		halfEdge[i] = m_navHalfEdges[firstNavHalfEdgeIdx + i];

	NavHalfEdgeRawPtr pairHalfEdgeRawPtr;
	for(KyUInt32 i = 0; i < 3; ++i)
	{
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
		isVisibleEdges[i] = neighborTriangleAlreadyVisited[i] || m_edgeIntersector->IsEdgeVisible(*vertex[i], *vertex[i+1], halfEdgeRawPtr[i], isACrossableEdges[i]);

	for(KyUInt32 i = 0; i < 3; ++i)
	{
		m_shouldVisitNeighborTriangle[i] = neighborTriangleAlreadyVisited[i]==false && isVisibleEdges[i] && isACrossableEdges[i];
	}
}

}


#endif //Navigation_BestFirstSearch2dBorderCollector_H

