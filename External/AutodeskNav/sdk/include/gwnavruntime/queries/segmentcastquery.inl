/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY


namespace Kaim
{
template<class TLogic>
KY_INLINE SegmentCastQuery<TLogic>::SegmentCastQuery() : BaseSegmentCastQuery() {}

template<class TLogic>
KY_INLINE void SegmentCastQuery<TLogic>::BindToDatabase(Database* database)
{
	BaseSegmentCastQuery::BindToDatabase(database);
}

template<class TLogic>
KY_INLINE void SegmentCastQuery<TLogic>::Initialize(const Vec3f& startPos, KyFloat32 radius, const Vec2f& normalizedDir2d, KyFloat32 maxDist)
{
	BaseSegmentCastQuery::Initialize(startPos, radius, normalizedDir2d, maxDist);
}

template<class TLogic>
KY_INLINE void SegmentCastQuery<TLogic>::SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr) { BaseSegmentCastQuery::SetStartTrianglePtr(startTrianglePtr);   }
template<class TLogic>
KY_INLINE void SegmentCastQuery<TLogic>::SetStartIntegerPos(const WorldIntegerPos& startIntegerPos)  { BaseSegmentCastQuery::SetStartIntegerPos(startIntegerPos);     }
template<class TLogic>
KY_INLINE void SegmentCastQuery<TLogic>::SetSafetyDist(KyFloat32 safetyDist)                         { BaseSegmentCastQuery::SetSafetyDist(safetyDist);               }
template<class TLogic>
KY_INLINE void SegmentCastQuery<TLogic>::SetDynamicOutputMode(DynamicOutputMode dynamicOutputMode)   { BaseSegmentCastQuery::SetDynamicOutputMode(dynamicOutputMode); }
template<class TLogic>
KY_INLINE void SegmentCastQuery<TLogic>::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput)
{
	BaseSegmentCastQuery::SetQueryDynamicOutput(queryDynamicOutput);
}
template<class TLogic>
KY_INLINE void SegmentCastQuery<TLogic>::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	BaseSegmentCastQuery::SetPositionSpatializationRange(positionSpatializationRange);
}
template<class TLogic>
KY_INLINE SegmentCastQueryResult SegmentCastQuery<TLogic>::GetResult()                  const { return BaseSegmentCastQuery::GetResult();                  }
template<class TLogic>
KY_INLINE DynamicOutputMode      SegmentCastQuery<TLogic>::GetDynamicOutputMode()       const { return BaseSegmentCastQuery::GetDynamicOutputMode();       }
template<class TLogic>
KY_INLINE const Vec3f&           SegmentCastQuery<TLogic>::GetStartPos()                const { return BaseSegmentCastQuery::GetStartPos();                }
template<class TLogic>
KY_INLINE const Vec3f&           SegmentCastQuery<TLogic>::GetCollisionPos()            const { return BaseSegmentCastQuery::GetCollisionPos();            }
template<class TLogic>
KY_INLINE const Vec3f&           SegmentCastQuery<TLogic>::GetArrivalPos()              const { return BaseSegmentCastQuery::GetArrivalPos();              }
template<class TLogic>
KY_INLINE const Vec2f&           SegmentCastQuery<TLogic>::GetNormalizedDir2d()         const { return BaseSegmentCastQuery::GetNormalizedDir2d();         }
template<class TLogic>
KY_INLINE const NavTrianglePtr&  SegmentCastQuery<TLogic>::GetStartTrianglePtr()        const { return BaseSegmentCastQuery::GetStartTrianglePtr();        }
template<class TLogic>
KY_INLINE const NavTrianglePtr&  SegmentCastQuery<TLogic>::GetArrivalTrianglePtr()      const { return BaseSegmentCastQuery::GetArrivalTrianglePtr();      }
template<class TLogic>
KY_INLINE const NavHalfEdgePtr&  SegmentCastQuery<TLogic>::GetCollisionNavHalfEdgePtr() const { return BaseSegmentCastQuery::GetCollisionNavHalfEdgePtr(); }
template<class TLogic>
KY_INLINE KyFloat32              SegmentCastQuery<TLogic>::GetMaxDist()                 const { return BaseSegmentCastQuery::GetMaxDist();                 }
template<class TLogic>
KY_INLINE KyFloat32              SegmentCastQuery<TLogic>::GetSafetyDist()              const { return BaseSegmentCastQuery::GetSafetyDist();              }
template<class TLogic>
KY_INLINE const PositionSpatializationRange& SegmentCastQuery<TLogic>::GetPositionSpatializationRange() const { return BaseSegmentCastQuery::GetPositionSpatializationRange();  }
template<class TLogic>
KY_INLINE KyFloat32              SegmentCastQuery<TLogic>::GetRadius()                  const { return BaseSegmentCastQuery::GetRadius();                  }
template<class TLogic>
KY_INLINE QueryDynamicOutput*    SegmentCastQuery<TLogic>::GetQueryDynamicOutput()      const { return BaseSegmentCastQuery::GetQueryDynamicOutput();      }
template<class TLogic>
KY_INLINE const WorldIntegerPos& SegmentCastQuery<TLogic>::GetStartIntegerPos()         const { return BaseSegmentCastQuery::GetStartIntegerPos();         }
template<class TLogic>
KY_INLINE const WorldIntegerPos& SegmentCastQuery<TLogic>::GetArrivalIntegerPos()       const { return BaseSegmentCastQuery::GetArrivalIntegerPos();       }

template<class TLogic>
KY_INLINE void SegmentCastQuery<TLogic>::Advance(WorkingMemory* workingMemory)
{
	PerformQuery(workingMemory);
	m_processStatus = QueryDone;
}

template<class TLogic>
inline void SegmentCastQuery<TLogic>::PerformQuery(WorkingMemory* workingMemory)
{
	if (GetResult() != SEGMENTCAST_NOT_PROCESSED)
		return;

	if (m_database->IsClear())
	{
		SetResult(SEGMENTCAST_DONE_START_OUTSIDE);
		return;
	}

	if (workingMemory == KY_NULL)
		workingMemory = m_database->GetWorkingMemory();

	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	databaseGenMetrics.GetWorldIntegerPosFromVec3f(m_startPos3f, m_startIntegerPos);

	return PerformQueryWithInputCoordPos(workingMemory);
}


template<class TLogic>
inline void SegmentCastQuery<TLogic>::PerformQueryWithInputCoordPos(WorkingMemory* workingMemory)
{
	ScopedPerformedQueryCounter(GetStaticType(), m_database, m_performQueryStat);
	KY_DEBUG_ASSERTN(GetResult() == SEGMENTCAST_NOT_PROCESSED, ("Query has not been correctly initialized"));

	QueryUtils queryUtils(m_database, workingMemory, GetTraverseLogicUserData());
	TraversalResult TraversalRC;
	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	const KyFloat32 integerPrecision = databaseGenMetrics.m_integerPrecision;

	if (queryUtils.FindTriangleFromPositionIfNotValid(m_startTrianglePtr, m_startPos3f, m_startIntegerPos, m_positionSpatializationRange) == false)
	{
		SetResult(SEGMENTCAST_DONE_START_OUTSIDE);
		return;
	}

	const NavTriangleRawPtr startTriangleRawPtr = m_startTrianglePtr.GetRawPtr();
	const Vec3f normalizedDir3D(m_normalizedDir2d.x, m_normalizedDir2d.y, 0.f);

	if(startTriangleRawPtr.CanBeTraversed<TLogic>(GetTraverseLogicUserData()) == false)
	{
		SetResult(SEGMENTCAST_DONE_START_NAVTAG_FORBIDDEN);
		return;
	}

	const KyInt32 radiusCellSize = (KyInt32)(databaseGenMetrics.GetNearestInteger64FromFloatValue(m_radius) / databaseGenMetrics.m_cellSizeInCoord) + 1;
	const CellPos destCellPos = databaseGenMetrics.ComputeCellPos(m_startPos3f + normalizedDir3D * m_maxDist);
	const CellPos minCellPos(Min(m_startIntegerPos.m_cellPos.x, destCellPos.x) - radiusCellSize, Min(m_startIntegerPos.m_cellPos.y, destCellPos.y) - radiusCellSize);
	const CellPos maxCellPos(Max(m_startIntegerPos.m_cellPos.x, destCellPos.x) + radiusCellSize, Max(m_startIntegerPos.m_cellPos.y, destCellPos.y) + radiusCellSize);
	const CellBox cellBox(minCellPos, maxCellPos);

#if defined (KY_BUILD_DEBUG)
	const CoordBox64 cellsIntegerBox(databaseGenMetrics.ComputeCellOrigin(minCellPos), databaseGenMetrics.ComputeCellOrigin(maxCellPos));
	if (Max(cellsIntegerBox.Max().x - cellsIntegerBox.Min().x, cellsIntegerBox.Max().y - cellsIntegerBox.Min().y) >= (1LL << 31))
		KY_DEBUG_WARNINGN( ("startPos and destPos are too far from each other, overflow may occur during computation, result may be incoherent"));
#endif

	// Set up the visitor and the traversal
	RectangleIntersector rectangleIntersector(*this);
	BestFirstSearchEdgeCollisionVisitor<TLogic, RectangleIntersector> edgeIntersectionVisitor(GetTraverseLogicUserData(), rectangleIntersector);
	BestFirstSearchTraversal<BestFirstSearchEdgeCollisionVisitor<TLogic, RectangleIntersector> > traversalForSegmentCast(queryUtils, cellBox,
		edgeIntersectionVisitor);

	// set the root node : the triangle that contains m_startPos
	// Not that its navTag is not test within this function, but it has already been tested
	TraversalRC = traversalForSegmentCast.SetStartTriangle(startTriangleRawPtr);
	if (TraversalRC != TraversalResult_DONE)
	{
		SetResult(ConvertTraversalResultToSegmentCastResult(TraversalRC));
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
			SetResult(SEGMENTCAST_DONE_LACK_OF_WORKING_MEMORY);
			return;
		}

		scopeAutoSaveDynOutput.SetNavTriangleRawPtrs(&visitedTriangles);

		traversalForSegmentCast.SetVisitedNodeContainer(&visitedTriangles);
	}

	// run the traversal algorithm
	TraversalRC = traversalForSegmentCast.Search();
	if (TraversalRC != TraversalResult_DONE)
	{
		SetResult(ConvertTraversalResultToSegmentCastResult(TraversalRC));
		return;
	}

	KyFloat32 distReached = 0.f;
	if (edgeIntersectionVisitor.m_collisionFound)
	{
		const NavHalfEdgeRawPtr& intersectedHalfEdgeRawPtr = edgeIntersectionVisitor.m_intersectedHalfEdgeRawPtr;
		m_collisionHalfEdgePtr.Set(intersectedHalfEdgeRawPtr.GetNavFloor(), intersectedHalfEdgeRawPtr.GetHalfEdgeIdx());
		m_collisionPos3f.x = edgeIntersectionVisitor.m_collisionPos.x;
		m_collisionPos3f.y = edgeIntersectionVisitor.m_collisionPos.y;

		const NavTriangleRawPtr triangleRawPtr(intersectedHalfEdgeRawPtr.GetNavFloor(), NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(intersectedHalfEdgeRawPtr.GetHalfEdgeIdx()));

		m_collisionPos3f.z = triangleRawPtr.GetAltitudeOfPointInTriangle(m_collisionPos3f);

		WorldIntegerPos validCollisionIntegerPos;
		NavTrianglePtr trianglePtr;
		if (KY_FAILED(queryUtils.FindValidPositionFromIntegerIntersection<TLogic>(edgeIntersectionVisitor.m_collisionPos,
			m_collisionHalfEdgePtr, validCollisionIntegerPos, trianglePtr)))
		{
			SetResult(SEGMENTCAST_DONE_COLLISION_FOUND_ARRIVAL_ERROR);
			return;
		}

		CoordPos64 validCollisionCoordPos = databaseGenMetrics.GetCoordPos64FromWorldIntegerPos(validCollisionIntegerPos);
		const Vec3f validCollisionPos(Vec3f(validCollisionCoordPos.x * integerPrecision, validCollisionCoordPos.y * integerPrecision, m_collisionPos3f.z) - normalizedDir3D * m_safetyDist);

		distReached = (validCollisionPos - m_startPos3f) * normalizedDir3D;

		if (distReached < 0.f)
		{
			//we move back beyond startPos, which is bad : we return StartPos
			m_arrivalPos3f = m_startPos3f;
			m_arrivalIntegerPos = m_startIntegerPos;
			m_arrivalTrianglePtr = m_startTrianglePtr;
			SetResult(SEGMENTCAST_DONE_CANNOT_MOVE);
			return;
		}
	}
	else
		distReached = m_maxDist;

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

	m_arrivalPos3f = rayCastQuery.GetArrivalPos();
	m_arrivalIntegerPos = rayCastQuery.GetArrivalIntegerPos();
	m_arrivalTrianglePtr = rayCastQuery.GetArrivalTrianglePtr();

	if(rayCastQuery.GetResult() != RAYCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED && rayCastQuery.GetResult() != RAYCAST_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION)
	{
		// that should never happen !
		KY_LOG_ERROR( ("Unexpected error in RayCast"));
		SetResult(SEGMENTCAST_DONE_COLLISION_FOUND_ARRIVAL_ERROR);
		return;
	}

	SetResult(distReached == m_maxDist ? SEGMENTCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED : SEGMENTCAST_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION);
}
}
