/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{
template<class TLogic>
KY_INLINE DiskCastQuery<TLogic>::DiskCastQuery() : BaseDiskCastQuery() {}

template<class TLogic>
KY_INLINE void DiskCastQuery<TLogic>::BindToDatabase(Database* database)
{
	BaseDiskCastQuery::BindToDatabase(database);
}

template<class TLogic>
KY_INLINE void DiskCastQuery<TLogic>::Initialize(const Vec3f& startPos, KyFloat32 radius, const Vec2f& normalizedDir2d, KyFloat32 maxDist)
{
	BaseDiskCastQuery::Initialize(startPos, radius, normalizedDir2d, maxDist);
}

template<class TLogic>
KY_INLINE void DiskCastQuery<TLogic>::SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr)   { BaseDiskCastQuery::SetStartTrianglePtr(startTrianglePtr);     }
template<class TLogic>
KY_INLINE void DiskCastQuery<TLogic>::SetStartIntegerPos(const WorldIntegerPos& startIntegerPos)    { BaseDiskCastQuery::SetStartIntegerPos(startIntegerPos);       }
template<class TLogic>
KY_INLINE void DiskCastQuery<TLogic>::SetSafetyDist(KyFloat32 safetyDist)                           { BaseDiskCastQuery::SetSafetyDist(safetyDist);                 }
template<class TLogic>
KY_INLINE void DiskCastQuery<TLogic>::SetQueryType(DiskCastQueryType queryType)                     { BaseDiskCastQuery::SetQueryType(queryType);                   }
template<class TLogic>
KY_INLINE void DiskCastQuery<TLogic>::SetDynamicOutputMode(DynamicOutputMode dynamicOutputMode )    { BaseDiskCastQuery::SetDynamicOutputMode(dynamicOutputMode);   }
template<class TLogic>
KY_INLINE void DiskCastQuery<TLogic>::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput) { BaseDiskCastQuery::SetQueryDynamicOutput(queryDynamicOutput); }
template<class TLogic>
KY_INLINE void DiskCastQuery<TLogic>::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	BaseDiskCastQuery::SetPositionSpatializationRange(positionSpatializationRange);
}
template<class TLogic>
KY_INLINE DiskCastQueryResult    DiskCastQuery<TLogic>::GetResult()                  const { return BaseDiskCastQuery::GetResult();                  }
template<class TLogic>
KY_INLINE DynamicOutputMode      DiskCastQuery<TLogic>::GetDynamicOutputMode()       const { return BaseDiskCastQuery::GetDynamicOutputMode();       }
template<class TLogic>
KY_INLINE const Vec3f&           DiskCastQuery<TLogic>::GetStartPos()                const { return BaseDiskCastQuery::GetStartPos();                }
template<class TLogic>
KY_INLINE const Vec3f&           DiskCastQuery<TLogic>::GetCollisionPos()            const { return BaseDiskCastQuery::GetCollisionPos();            }
template<class TLogic>
KY_INLINE const Vec3f&           DiskCastQuery<TLogic>::GetArrivalPos()              const { return BaseDiskCastQuery::GetArrivalPos();              }
template<class TLogic>
KY_INLINE const Vec2f&           DiskCastQuery<TLogic>::GetNormalizedDir2d()         const { return BaseDiskCastQuery::GetNormalizedDir2d();         }
template<class TLogic>
KY_INLINE const NavTrianglePtr&  DiskCastQuery<TLogic>::GetStartTrianglePtr()        const { return BaseDiskCastQuery::GetStartTrianglePtr();        }
template<class TLogic>
KY_INLINE const NavTrianglePtr&  DiskCastQuery<TLogic>::GetArrivalTrianglePtr()      const { return BaseDiskCastQuery::GetArrivalTrianglePtr();      }
template<class TLogic>
KY_INLINE const NavHalfEdgePtr&  DiskCastQuery<TLogic>::GetCollisionNavHalfEdgePtr() const { return BaseDiskCastQuery::GetCollisionNavHalfEdgePtr(); }
template<class TLogic>
KY_INLINE KyFloat32              DiskCastQuery<TLogic>::GetMaxDist()                 const { return BaseDiskCastQuery::GetMaxDist();                 }
template<class TLogic>
KY_INLINE KyFloat32              DiskCastQuery<TLogic>::GetSafetyDist()              const { return BaseDiskCastQuery::GetSafetyDist();              }
template<class TLogic>
KY_INLINE KyFloat32              DiskCastQuery<TLogic>::GetRadius()                  const { return BaseDiskCastQuery::GetRadius();                  }
template<class TLogic>
KY_INLINE DiskCastQueryType      DiskCastQuery<TLogic>::GetQueryType()               const { return BaseDiskCastQuery::GetQueryType();               }
template<class TLogic>
KY_INLINE QueryDynamicOutput*    DiskCastQuery<TLogic>::GetQueryDynamicOutput()      const { return BaseDiskCastQuery::GetQueryDynamicOutput();      }
template<class TLogic>
KY_INLINE const WorldIntegerPos& DiskCastQuery<TLogic>::GetStartIntegerPos()         const { return BaseDiskCastQuery::GetStartIntegerPos();         }
template<class TLogic>
KY_INLINE const WorldIntegerPos& DiskCastQuery<TLogic>::GetArrivalIntegerPos()       const { return BaseDiskCastQuery::GetArrivalIntegerPos();       }
template<class TLogic>
KY_INLINE const PositionSpatializationRange& DiskCastQuery<TLogic>::GetPositionSpatializationRange()  const { return BaseDiskCastQuery::GetPositionSpatializationRange();  }

template<class TLogic>
KY_INLINE void DiskCastQuery<TLogic>::Advance(WorkingMemory* workingMemory)
{
	PerformQuery(workingMemory);
	m_processStatus = QueryDone;
}


template<class TLogic>
inline void DiskCastQuery<TLogic>::PerformQuery(WorkingMemory* workingMemory)
{
	if (GetResult() != DISKCAST_NOT_PROCESSED)
		return;

	if (m_database->IsClear())
	{
		SetResult(DISKCAST_DONE_START_OUTSIDE);
		return;
	}

	if (workingMemory == KY_NULL)
		workingMemory = m_database->GetWorkingMemory();

	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	databaseGenMetrics.GetWorldIntegerPosFromVec3f(m_startPos3f, m_startIntegerPos);

	return PerformQueryWithInputCoordPos(workingMemory);
}

template<class TLogic>
inline void DiskCastQuery<TLogic>::PerformQueryWithInputCoordPos(WorkingMemory* workingMemory)
{
	ScopedPerformedQueryCounter(GetStaticType(), m_database, m_performQueryStat);
	KY_DEBUG_ASSERTN(GetResult() == DISKCAST_NOT_PROCESSED, ("Query has not been correctly initialized"));

	QueryUtils queryUtils(m_database, workingMemory,  GetTraverseLogicUserData());
	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	TraversalResult traversalRC;

	if (queryUtils.FindTriangleFromPositionIfNotValid(m_startTrianglePtr, m_startPos3f, m_startIntegerPos, m_positionSpatializationRange) == false)
	{
		SetResult(DISKCAST_DONE_START_OUTSIDE);
		return;
	}

	const NavTriangleRawPtr startTriangleRawPtr = m_startTrianglePtr.GetRawPtr();
	const Vec3f normalizedDir3D(m_normalizedDir2d.x, m_normalizedDir2d.y, 0.f);

	if(startTriangleRawPtr.CanBeTraversed<TLogic>(GetTraverseLogicUserData()) == false)
	{
		SetResult(DISKCAST_DONE_START_NAVTAG_FORBIDDEN);
		return;
	}

	const KyInt32 radiusCellSize = (KyInt32)(databaseGenMetrics .GetNearestInteger64FromFloatValue(m_radius) / databaseGenMetrics .m_cellSizeInCoord) + 1;
	const CellPos destCellPos = databaseGenMetrics .ComputeCellPos(m_startPos3f + normalizedDir3D * m_maxDist);
	const CellPos minCellPos(Min(startTriangleRawPtr.GetCellPos().x, destCellPos.x) - radiusCellSize, Min(startTriangleRawPtr.GetCellPos().y, destCellPos.y) - radiusCellSize);
	const CellPos maxCellPos(Max(startTriangleRawPtr.GetCellPos().x, destCellPos.x) + radiusCellSize, Max(startTriangleRawPtr.GetCellPos().y, destCellPos.y) + radiusCellSize);
	const CellBox cellBox(minCellPos, maxCellPos);

#if defined (KY_BUILD_DEBUG)
	const CoordBox64 cellsIntegerBox(databaseGenMetrics .ComputeCellOrigin(minCellPos), databaseGenMetrics .ComputeCellOrigin(maxCellPos));
	if (Max(cellsIntegerBox.Max().x - cellsIntegerBox.Min().x, cellsIntegerBox.Max().y - cellsIntegerBox.Min().y) >= (1LL << 31))
		KY_DEBUG_WARNINGN( ("startPos and destPos are too far from each other, overflow may occur during computation, result may be incoherent"));
#endif

	// Set up the visitor and the traversal
	CapsuleIntersector diskIntersector(m_startPos3f, m_normalizedDir2d, m_maxDist, m_radius, m_database->GetDatabaseGenMetrics().m_integerPrecision);
	BestFirstSearchEdgeCollisionVisitor<TLogic, CapsuleIntersector> edgeIntersectionVisitor(GetTraverseLogicUserData(), diskIntersector);
	BestFirstSearchTraversal<BestFirstSearchEdgeCollisionVisitor<TLogic, CapsuleIntersector> > traversalForDiskCast(queryUtils, cellBox,
		edgeIntersectionVisitor);

	// set the root node : the triangle that contains m_startPos
	// Not that its navTag is not test within this function, but it has already been tested
	traversalRC = traversalForDiskCast.SetStartTriangle(startTriangleRawPtr);
	if (traversalRC != TraversalResult_DONE)
	{
		SetResult(ConvertTraversalResultToDiskCastResult(traversalRC));
		return;
	}

	// check if we want to retrieve the visited triangles
	WorkingMemArray<NavTriangleRawPtr> visitedTriangles;
	ScopeAutoSaveDynamicOutput scopeAutoSaveDynOutput(m_queryDynamicOutput);
	if ((GetDynamicOutputMode() & QUERY_SAVE_TRIANGLES) != 0)
	{
		visitedTriangles.Init(workingMemory);
		if (visitedTriangles.IsInitialized() == false)
		{
			SetResult(DISKCAST_DONE_LACK_OF_WORKING_MEMORY);
			return;
		}

		scopeAutoSaveDynOutput.SetNavTriangleRawPtrs(&visitedTriangles);
		traversalForDiskCast.SetVisitedNodeContainer(&visitedTriangles);
	}

	// run the traversal algorithm
	traversalRC = traversalForDiskCast.Search();
	if (traversalRC != TraversalResult_DONE)
	{
		SetResult(ConvertTraversalResultToDiskCastResult(traversalRC));
		return;
	}

	KyFloat32 distReached = 0.f;
	if (edgeIntersectionVisitor.m_collisionFound)
	{
		if (GetQueryType() == DISKCAST_ADAPT_TO_STARTPOS && (edgeIntersectionVisitor.m_collisionPos - m_startPos3f).GetSquareLength2d() < m_radius * m_radius)
			return ForceDiskCast(workingMemory, cellBox, startTriangleRawPtr, visitedTriangles);

		if (KY_FAILED(FindDistReachedFromCollisionPos(edgeIntersectionVisitor.m_intersectedHalfEdgeRawPtr, edgeIntersectionVisitor.m_collisionPos, distReached)))
			return;
	}
	else
	{
		distReached = m_maxDist;
	}

	// need to perform a RayCast to compute the arrivalTrianglePtr
	RayCastQuery<TLogic> rayCastQuery;
	rayCastQuery.BindToDatabase(m_database);
	rayCastQuery.SetPositionSpatializationRange(m_positionSpatializationRange);
	rayCastQuery.SetTraverseLogicUserData(GetTraverseLogicUserData());

	rayCastQuery.Initialize(m_startPos3f, m_normalizedDir2d * distReached);
	rayCastQuery.SetStartTrianglePtr(m_startTrianglePtr);
	rayCastQuery.SetStartIntegerPos(m_startIntegerPos);
	rayCastQuery.SetPerformQueryStat(m_performQueryStat);
	rayCastQuery.PerformQueryWithInputCoordPos(workingMemory);

	if(rayCastQuery.GetResult() != RAYCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED)
	{
		// that should never happen !
		KY_LOG_ERROR( ("Unexpected error in DiskCastQuery"));
		SetResult(DISKCAST_DONE_COLLISION_FOUND_ARRIVAL_ERROR);
		return;
	}

	m_arrivalPos3f = rayCastQuery.GetArrivalPos();
	m_arrivalIntegerPos = rayCastQuery.GetArrivalIntegerPos();
	m_arrivalTrianglePtr = rayCastQuery.GetArrivalTrianglePtr();

	SetResult(distReached == m_maxDist ? DISKCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED : DISKCAST_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION);
}

template<class TLogic>
inline void DiskCastQuery<TLogic>::ForceDiskCast(WorkingMemory* workingMemory, const CellBox& cellBox,
	const NavTriangleRawPtr& startTriangleRawPtr, WorkingMemArray<NavTriangleRawPtr>& visitedTriangles)
{
	QueryUtils queryUtils(m_database, workingMemory, GetTraverseLogicUserData());
	KY_LOG_ERROR_IF(m_startTrianglePtr.IsValid() == false, ("m_startTrianglePtr must have been computed here. It must be valid !"));

	if ((GetDynamicOutputMode() & QUERY_SAVE_TRIANGLES) != 0)
	{
		visitedTriangles.MakeEmpty();
	}

	RayCastQueryResult queryOnRightResult, queryOnLeftResult;
	KyFloat32 borderDistOnRight, borderDistOnLeft;
	RayCastQuery<TLogic> rayCastQuery;
	rayCastQuery.BindToDatabase(m_database);
	rayCastQuery.SetPositionSpatializationRange(m_positionSpatializationRange);
	rayCastQuery.SetTraverseLogicUserData(GetTraverseLogicUserData());

	if (KY_FAILED(RayCastQueryFromStartOnTheRigth(workingMemory, rayCastQuery, queryOnRightResult, borderDistOnRight)))
		return;

	if (KY_FAILED(RayCastQueryFromStartOnTheLeft(workingMemory, rayCastQuery, queryOnLeftResult, borderDistOnLeft)))
		return;

	if (queryOnLeftResult == RAYCAST_DONE_CANNOT_MOVE && queryOnRightResult == RAYCAST_DONE_CANNOT_MOVE)
	{
		m_arrivalPos3f = m_startPos3f;
		m_arrivalIntegerPos = m_startIntegerPos;
		m_arrivalTrianglePtr = m_startTrianglePtr;
		SetResult(DISKCAST_DONE_CANNOT_MOVE);
		return;
	}

	// Set up the visitor and the traversal
	CrossedSectionCapsuleIntersector crossedSectionCapsuleIntersector(*this, borderDistOnRight, borderDistOnLeft);
	BestFirstSearchEdgeCollisionVisitor<TLogic, CrossedSectionCapsuleIntersector> edgeIntersectionVisitor(GetTraverseLogicUserData(), crossedSectionCapsuleIntersector);
	BestFirstSearchTraversal<BestFirstSearchEdgeCollisionVisitor<TLogic, CrossedSectionCapsuleIntersector> > traversalForDiskCast(queryUtils, cellBox,
		edgeIntersectionVisitor);

	TraversalResult traversalRC = traversalForDiskCast.SetStartTriangle(startTriangleRawPtr);
	if (traversalRC != TraversalResult_DONE)
	{
		SetResult(ConvertTraversalResultToDiskCastResult(traversalRC));
		return;
	}

	if ((GetDynamicOutputMode() & QUERY_SAVE_TRIANGLES) != 0)
		traversalForDiskCast.SetVisitedNodeContainer(&visitedTriangles);

	traversalRC = traversalForDiskCast.Search();
	if (traversalRC != TraversalResult_DONE)
	{
		SetResult(ConvertTraversalResultToDiskCastResult(traversalRC));
		return;
	}

	KyFloat32 distReached = 0.f;
	if (edgeIntersectionVisitor.m_collisionFound)
	{
		if (KY_FAILED(FindDistReachedFromCollisionPos(edgeIntersectionVisitor.m_intersectedHalfEdgeRawPtr, edgeIntersectionVisitor.m_collisionPos, distReached)))
			return;
	}
	else
		distReached = m_maxDist;

	// need to perform a RayCast to compute the arrivalTrianglePtr
	rayCastQuery.Initialize(m_startPos3f, m_normalizedDir2d * distReached);
	rayCastQuery.SetStartTrianglePtr(m_startTrianglePtr);
	rayCastQuery.SetStartIntegerPos(m_startIntegerPos);

	rayCastQuery.PerformQueryWithInputCoordPos(workingMemory);

	if(rayCastQuery.GetResult() != RAYCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED)
	{
		// that should never happen !
		KY_LOG_ERROR( ("Unexpected error in DiskCastQuery"));
		SetResult(DISKCAST_DONE_COLLISION_FOUND_ARRIVAL_ERROR);
		return;
	}

	SetResult(distReached == m_maxDist ? DISKCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED : DISKCAST_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION);
	m_arrivalPos3f = rayCastQuery.GetArrivalPos();
	m_arrivalIntegerPos = rayCastQuery.GetArrivalIntegerPos();
	m_arrivalTrianglePtr = rayCastQuery.GetArrivalTrianglePtr();
}

template<class TLogic>
inline KyResult DiskCastQuery<TLogic>::RayCastQueryFromStartOnTheRigth(WorkingMemory* workingMemory, RayCastQuery<TLogic>& rayCastQuery,
	RayCastQueryResult& queryOnRightResult, KyFloat32& borderDistOnRight)
{
	RayCastQuery<TLogic>& queryOnRight = rayCastQuery;
	queryOnRight.Initialize(m_startPos3f, m_normalizedDir2d.PerpCW() * m_radius);
	queryOnRight.SetStartTrianglePtr(m_startTrianglePtr);
	queryOnRight.SetStartIntegerPos(m_startIntegerPos);
	queryOnRight.SetPerformQueryStat(m_performQueryStat);
	queryOnRight.PerformQueryWithInputCoordPos(workingMemory);

	queryOnRightResult = queryOnRight.GetResult();

	if (queryOnRightResult == RAYCAST_DONE_COLLISION_FOUND_ARRIVAL_ERROR)
	{
		m_collisionHalfEdgePtr = queryOnRight.GetCollisionNavHalfEdgePtr();
		m_collisionPos3f = queryOnRight.GetCollisionPos();
		m_arrivalPos3f = m_startPos3f;
		m_arrivalIntegerPos = m_startIntegerPos;
		m_arrivalTrianglePtr = m_startTrianglePtr;
		SetResult(DISKCAST_DONE_CANNOT_MOVE);
		return KY_ERROR;
	}

	if (queryOnRightResult == RAYCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED)
	{
		borderDistOnRight = m_radius;
	}
	else
	{
		if (queryOnRightResult == RAYCAST_DONE_CANNOT_MOVE)
			borderDistOnRight = 0.f;
		else
			borderDistOnRight = (queryOnRight.GetArrivalPos() - queryOnRight.GetStartPos()).GetLength2d();
	}

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult DiskCastQuery<TLogic>::RayCastQueryFromStartOnTheLeft(WorkingMemory* workingMemory, RayCastQuery<TLogic>& rayCastQuery,
	RayCastQueryResult& queryOnLeftResult, KyFloat32& borderDistOnLeft)
{
	RayCastQuery<TLogic>& queryOnLeft = rayCastQuery;
	queryOnLeft.Initialize(m_startPos3f, m_normalizedDir2d.PerpCCW() * m_radius);
	queryOnLeft.SetStartTrianglePtr(m_startTrianglePtr);
	queryOnLeft.SetStartIntegerPos(m_startIntegerPos);
	queryOnLeft.SetPerformQueryStat(m_performQueryStat);
	queryOnLeft.PerformQueryWithInputCoordPos(workingMemory);

	queryOnLeftResult = queryOnLeft.GetResult();

	if (queryOnLeftResult == RAYCAST_DONE_COLLISION_FOUND_ARRIVAL_ERROR)
	{
		m_collisionHalfEdgePtr = queryOnLeft.GetCollisionNavHalfEdgePtr();
		m_collisionPos3f = queryOnLeft.GetCollisionPos();
		m_arrivalPos3f = m_startPos3f;
		m_arrivalIntegerPos = m_startIntegerPos;
		m_arrivalTrianglePtr = m_startTrianglePtr;
		SetResult(DISKCAST_DONE_CANNOT_MOVE);
		return KY_ERROR;
	}

	if (queryOnLeftResult == RAYCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED)
	{
		borderDistOnLeft = m_radius;
	}
	else
	{
		if (queryOnLeftResult == RAYCAST_DONE_CANNOT_MOVE)
			borderDistOnLeft = 0.f;
		else
			borderDistOnLeft = (queryOnLeft.GetArrivalPos() - queryOnLeft.GetStartPos()).GetLength2d();
	}

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult DiskCastQuery<TLogic>::FindDistReachedFromCollisionPos(const NavHalfEdgeRawPtr& intersectedHalfEdgeRawPtr, const Vec3f& collisionPos, KyFloat32& distReached)
{
	m_collisionHalfEdgePtr.Set(intersectedHalfEdgeRawPtr.GetNavFloor(), intersectedHalfEdgeRawPtr.GetHalfEdgeIdx());
	m_collisionPos3f = collisionPos;

	const NavTriangleRawPtr triangleRawPtr(intersectedHalfEdgeRawPtr.m_navFloorRawPtr, NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(intersectedHalfEdgeRawPtr.GetHalfEdgeIdx()));
	m_collisionPos3f.z = triangleRawPtr.GetAltitudeOfPointInTriangle(m_collisionPos3f);

	if ((m_collisionPos3f - m_startPos3f).GetSquareLength2d() < m_radius * m_radius)
	{
		m_arrivalPos3f = m_startPos3f;
		m_arrivalIntegerPos = m_startIntegerPos;
		m_arrivalTrianglePtr = m_startTrianglePtr;
		SetResult(DISKCAST_DONE_CANNOT_MOVE);
		return KY_ERROR;
	}

	Vec3f normalizedDir3D(m_normalizedDir2d.x, m_normalizedDir2d.y, 0.f);
	KyFloat32 projectionOndirDist = (m_collisionPos3f- m_startPos3f) * normalizedDir3D;
	const Vec2f normalizedDir_ortho = m_normalizedDir2d.PerpCCW();
	const Vec3f normalizedDir_ortho3D(normalizedDir_ortho.x, normalizedDir_ortho.y, 0.f);

	KyFloat32 projectionOndirDistOrtho = fabs((m_collisionPos3f - m_startPos3f) * normalizedDir_ortho3D);
	KyFloat32 moveBackDist;

	// deal with float imprecision
	if (projectionOndirDistOrtho > m_radius)
		moveBackDist = 0.f;
	else
		moveBackDist = sqrtf(m_radius * m_radius - projectionOndirDistOrtho * projectionOndirDistOrtho);

	distReached = projectionOndirDist - m_safetyDist - moveBackDist;

	if (distReached < 0.f)
	{
		m_arrivalPos3f = m_startPos3f;
		m_arrivalIntegerPos = m_startIntegerPos;
		m_arrivalTrianglePtr = m_startTrianglePtr;
		SetResult(DISKCAST_DONE_CANNOT_MOVE);
		return KY_ERROR;
	}

	return KY_SUCCESS;
}


}
