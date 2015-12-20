/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_RayQueryUtils_H
#define Navigation_RayQueryUtils_H

#include "gwnavruntime/queries/querydynamicoutput.h"
#include "gwnavruntime/queries/trianglefromposandtriangleseedquery.h"
#include "gwnavruntime/querysystem/workingmemcontainers/workingmemarray.h"
#include "gwnavruntime/navmesh/intersections.h"
#include "gwnavruntime/database/database.h"

namespace Kaim
{

class Box2f;
class NavTrianglePtr;
class NavTriangleRawPtr;
class NavHalfEdgePtr;
class NavHalfEdgeRawPtr;
class TriangleFromPosAndTriangleSeedQuery;
class WorldIntegerPos;
class LocalStoreActiveDataWorkspaceContext;
class RawNavTagSubSegment;
class WorkingMemory;
class Path;
template <class T> class WorkingMemArray;

/// Enumerates the possible ways a RayCanGoQuery can compute the cost of following the ray.
enum ComputeCostMode
{
	QUERY_DO_NOT_COMPUTE_COST,        ///< The cost is not computed.
	QUERY_COMPUTE_COST_ALONG_3DAXIS,  ///< The cost is computed according to the CanTraverseAndGetCostMultiplier function of the traverseLogic. These multipliers will be applied on distance between the consecutive NavTagSwitch intersections projected on the 3D axis going from startPos to DestPos.
};

/// This class is an helper used internaly to factorize code of Ray queries (RayCanGoQuery, RayCastGoQuery, ...)
class RayQueryUtils
{
	RayQueryUtils();
public:
	RayQueryUtils(const Vec3f& startPos3f, const Vec3f& destPos3f, void* traverseLogicUserData,
		DynamicOutputMode savingMode = QUERY_SAVE_NOTHING,
		WorkingMemArray<NavTriangleRawPtr>* crossedTriangles = KY_NULL,
		WorkingMemArray<RawNavTagSubSegment>* navTagSubSegments = KY_NULL,
		WorkingMemArray<RawHalfEdgeIntersection>* halfEdgeIntersections = KY_NULL,
		ComputeCostMode computeCostMode = QUERY_DO_NOT_COMPUTE_COST,
		KyFloat32 firstCostMultiplier = 1.f);

	KyResult StorePropagationData(KyFloat32 integerPrecision, const NavHalfEdgeRawPtr& currentPairHalfEdgeRawPtr, RawNavTagSubSegment& navTagSubSegment,
		const CoordPos64& cellOrigin, const NavFloorBlob* navFloorBlob);

	template<class TraverseLogic>
	KyResult StoreOrComputeCostFromPropagationData(KyFloat32 integerPrecision, const NavHalfEdgeRawPtr& currentPairHalfEdgeRawPtr,
		RawNavTagSubSegment& navTagSubSegment, const CoordPos64& cellOrigin, const NavFloorBlob* navFloorBlob, KyFloat32 nextCostMultiplier, KyFloat32& cost);

	KyInt32 GetFirstEdgeToCrossInStartTriangle(const NavTriangleRawPtr& startTriangleRawPtr, const CoordPos64& startCoordPos, const CoordPos64& destCoordPos);

	bool IsEdgeBeyondDestPos(const CoordPos64& cellOrigin, const NavFloorBlob& navFloorBlob, NavHalfEdgeIdx navHalfEdgeIdx,
	    const CoordPos64& segmentStartCoordPos, const CoordPos64& segmentDestCoordPos, const CoordPos64& destinationCoordPos, const KyInt64& startToDestSquareDist);

	void UpdateCostAlongAxis(const Vec3f& endOfCostMultiplierSegment, KyFloat32 nextCostMultiplier, KyFloat32& cost);
protected:
	KyResult StoreTriangle(const NavHalfEdgeRawPtr& currentPairHalfEdgeRawPtr);
	bool EdgeIsANavTagBoundary(const NavHalfEdgeRawPtr& currentPairHalfEdgeRawPtr, RawNavTagSubSegment& navTagSubSegment);
	KyResult ComputeAndStoreEdgeIntersection(KyFloat32 integerPrecision, const NavHalfEdgeRawPtr& currentPairHalfEdgeRawPtr, const CoordPos64& cellOrigin,
		const NavFloorBlob* navFloorBlob);
	void ComputeIntersectionWithEdge(KyFloat32 integerPrecision, const NavHalfEdgeRawPtr& currentPairHalfEdgeRawPtr, const CoordPos64& cellOrigin,
		const NavFloorBlob* navFloorBlob, Vec3f& intersection);

public:
	Vec3f m_startPos3f;
	Vec3f m_destPos3f;
	void* m_traverseLogicUserData;
	DynamicOutputMode m_savingMode;


	ComputeCostMode m_computeCostMode;
	KyFloat32 m_dist3DOverSquareDist2d;
	Vec3f m_currentCostMultiplierStartPos;
	KyFloat32 m_currentCostMultiplier;

	WorkingMemArray<NavTriangleRawPtr>* m_crossedTriangles;
	WorkingMemArray<RawNavTagSubSegment>* m_navTagSubSegments;
	WorkingMemArray<RawHalfEdgeIntersection>* m_rawHalfEdgeIntersections;
};

template<class TraverseLogic>
inline KyResult RayQueryUtils::StoreOrComputeCostFromPropagationData(KyFloat32 integerPrecision, const NavHalfEdgeRawPtr& currentPairHalfEdgeRawPtr,
	RawNavTagSubSegment& subSegment, const CoordPos64& cellOrigin, const NavFloorBlob* navFloorBlob, KyFloat32 nextCostMultiplier, KyFloat32& cost)
{
	if ((m_savingMode & QUERY_SAVE_TRIANGLES) != 0)
	{
		KY_FORWARD_ERROR_NO_LOG(StoreTriangle(currentPairHalfEdgeRawPtr));
	}

	if ((m_savingMode & QUERY_SAVE_HALFEDGEINTERSECTIONS) != 0)
	{
		KY_FORWARD_ERROR_NO_LOG(ComputeAndStoreEdgeIntersection(integerPrecision, currentPairHalfEdgeRawPtr, cellOrigin, navFloorBlob));
	}

	bool endOfSubSegment     = (m_savingMode & QUERY_SAVE_NAVTAGSUBSEGMENTS) != 0 && EdgeIsANavTagBoundary(currentPairHalfEdgeRawPtr, subSegment);
	bool costMultiplierLimit = m_computeCostMode == QUERY_COMPUTE_COST_ALONG_3DAXIS && m_currentCostMultiplier != nextCostMultiplier;
	if (endOfSubSegment || costMultiplierLimit)
	{
		Vec3f intersectionWithEdge;
		ComputeIntersectionWithEdge(integerPrecision, currentPairHalfEdgeRawPtr, cellOrigin, navFloorBlob, intersectionWithEdge);

		if (endOfSubSegment)
		{
			subSegment.m_exitPos3f = intersectionWithEdge;
			KY_FORWARD_ERROR_NO_LOG(m_navTagSubSegments->PushBack(subSegment));

			const NavTriangleIdx triangleIdx = NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(currentPairHalfEdgeRawPtr.GetHalfEdgeIdx());
			subSegment.m_navTriangleRawPtr.Set(currentPairHalfEdgeRawPtr.m_navFloorRawPtr.GetNavFloor(), triangleIdx);
			subSegment.m_entrancePos3f = subSegment.m_exitPos3f;
			subSegment.m_exitPos3f.Set(KyFloat32MAXVAL, KyFloat32MAXVAL, KyFloat32MAXVAL);
		}

		if (costMultiplierLimit)
			UpdateCostAlongAxis(intersectionWithEdge, nextCostMultiplier, cost);
	}

	return KY_SUCCESS;
}

}


#endif //Navigation_RayQueryUtils_H

