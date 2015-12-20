/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY


namespace Kaim
{
template<class TLogic>
KY_INLINE RayCastOnSegmentQuery<TLogic>::RayCastOnSegmentQuery() : BaseRayCastOnSegmentQuery() {}

template<class TLogic>
KY_INLINE void RayCastOnSegmentQuery<TLogic>::BindToDatabase(Database* database)
{
	BaseRayCastOnSegmentQuery::BindToDatabase(database);
}
template<class TLogic>
KY_INLINE void RayCastOnSegmentQuery<TLogic>::Initialize(const Vec3f& startPos,
	const NavTrianglePtr& startTrianglePtr, const Vec3f& segmentStartPos, const Vec3f& segmentEndPos, KyFloat32 maxDist)
{
	BaseRayCastOnSegmentQuery::Initialize(startPos, startTrianglePtr, segmentStartPos, segmentEndPos, maxDist);
}

template<class TLogic>
KY_INLINE void RayCastOnSegmentQuery<TLogic>::Advance(WorkingMemory* workingMemory)
{
	PerformQuery(workingMemory);
	m_processStatus = QueryDone;
}

template<class TLogic>
KY_INLINE void RayCastOnSegmentQuery<TLogic>::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	BaseRayCastOnSegmentQuery::SetPositionSpatializationRange(positionSpatializationRange);
}
template<class TLogic>
KY_INLINE void RayCastOnSegmentQuery<TLogic>::SetDynamicOutputMode(DynamicOutputMode savingMode)
{
	BaseRayCastOnSegmentQuery::SetDynamicOutputMode(savingMode);
}
template<class TLogic>
KY_INLINE void RayCastOnSegmentQuery<TLogic>::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput)
{
	BaseRayCastOnSegmentQuery::SetQueryDynamicOutput(queryDynamicOutput);
}

template<class TLogic>
KY_INLINE void RayCastOnSegmentQuery<TLogic>::SetStartIntegerPos(const WorldIntegerPos& startIntegerPos)
{
	BaseRayCastOnSegmentQuery::SetStartIntegerPos(startIntegerPos);
}
template<class TLogic>
KY_INLINE void RayCastOnSegmentQuery<TLogic>::SetSegmentStartIntegerPos(const WorldIntegerPos& segmentStartIntegerPos)
{
	BaseRayCastOnSegmentQuery::SetSegmentStartIntegerPos(segmentStartIntegerPos);
}
template<class TLogic>
KY_INLINE void RayCastOnSegmentQuery<TLogic>::SetSegmentEndIntegerPos(const WorldIntegerPos& segmentEndIntegerPos)     
{
	BaseRayCastOnSegmentQuery::SetSegmentEndIntegerPos(segmentEndIntegerPos);
}

template<class TLogic>
KY_INLINE RayCastOnSegmentQueryResult RayCastOnSegmentQuery<TLogic>::GetResult ()                const { return BaseRayCastOnSegmentQuery::GetResult();                 }
template<class TLogic>
KY_INLINE DynamicOutputMode           RayCastOnSegmentQuery<TLogic>::GetDynamicOutputMode()      const { return BaseRayCastOnSegmentQuery::GetDynamicOutputMode();      }
template<class TLogic>
KY_INLINE const Vec3f&                RayCastOnSegmentQuery<TLogic>::GetStartPos()               const { return BaseRayCastOnSegmentQuery::GetStartPos();               }
template<class TLogic>
KY_INLINE const Vec3f&                RayCastOnSegmentQuery<TLogic>::GetSegmentStartPos()        const { return BaseRayCastOnSegmentQuery::GetSegmentStartPos();        }
template<class TLogic>
KY_INLINE const Vec3f&                RayCastOnSegmentQuery<TLogic>::GetSegmentEndPos()          const { return BaseRayCastOnSegmentQuery::GetSegmentEndPos();          }
template<class TLogic>
KY_INLINE const Vec3f&                RayCastOnSegmentQuery<TLogic>::GetArrivalPos()             const { return BaseRayCastOnSegmentQuery::GetArrivalPos();             }
template<class TLogic>
KY_INLINE const NavTrianglePtr&       RayCastOnSegmentQuery<TLogic>::GetStartTrianglePtr()       const { return BaseRayCastOnSegmentQuery::GetStartTrianglePtr();       }
template<class TLogic>
KY_INLINE const NavTrianglePtr&       RayCastOnSegmentQuery<TLogic>::GetArrivalTrianglePtr()     const { return BaseRayCastOnSegmentQuery::GetArrivalTrianglePtr();     }
template<class TLogic>
KY_INLINE KyFloat32                   RayCastOnSegmentQuery<TLogic>::GetMaxDist()                const { return BaseRayCastOnSegmentQuery::GetMaxDist();                }
template<class TLogic>
KY_INLINE const PositionSpatializationRange& RayCastOnSegmentQuery<TLogic>::GetPositionSpatializationRange() const { return BaseRayCastOnSegmentQuery::GetPositionSpatializationRange(); }
template<class TLogic>
KY_INLINE const WorldIntegerPos&      RayCastOnSegmentQuery<TLogic>::GetStartIntegerPos()        const { return BaseRayCastOnSegmentQuery::GetStartIntegerPos();        }
template<class TLogic>
KY_INLINE const WorldIntegerPos&      RayCastOnSegmentQuery<TLogic>::GetArrivalIntegerPos()      const { return BaseRayCastOnSegmentQuery::GetArrivalIntegerPos();      }
template<class TLogic>
KY_INLINE const WorldIntegerPos&      RayCastOnSegmentQuery<TLogic>::GetSegmentStartIntegerPos() const { return BaseRayCastOnSegmentQuery::GetSegmentStartIntegerPos(); }
template<class TLogic>
KY_INLINE const WorldIntegerPos&      RayCastOnSegmentQuery<TLogic>::GetSegmentEndIntegerPos()   const { return BaseRayCastOnSegmentQuery::GetSegmentEndIntegerPos();   }
template<class TLogic>
KY_INLINE QueryDynamicOutput*         RayCastOnSegmentQuery<TLogic>::GetQueryDynamicOutput()     const { return BaseRayCastOnSegmentQuery::GetQueryDynamicOutput();     }


template<class TLogic>
void RayCastOnSegmentQuery<TLogic>::PerformQuery(WorkingMemory* workingMemory)
{
	if (GetResult() != RAYCASTONSEGMENT_NOT_PROCESSED)
		return;

	if (m_database->IsClear())
	{
		SetResult(RAYCASTONSEGMENT_DONE_START_OUTSIDE);
		return;
	}

	if (workingMemory == KY_NULL)
		workingMemory = m_database->GetWorkingMemory();

	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	databaseGenMetrics.GetWorldIntegerPosFromVec3f(m_startPos3f, m_startIntegerPos);
	databaseGenMetrics.GetWorldIntegerPosFromVec3f(m_segmentStartPos3f, m_segmentStartIntegerPos);
	databaseGenMetrics.GetWorldIntegerPosFromVec3f(m_segmentEndPos3f, m_segmentEndIntegerPos);

	PerformQueryWithInputCoordPos(workingMemory);
}

template<class TLogic>
void RayCastOnSegmentQuery<TLogic>::PerformQueryWithInputCoordPos(WorkingMemory* workingMemory)
{
	ScopedPerformedQueryCounter(GetStaticType(), m_database, m_performQueryStat);
	KY_DEBUG_ASSERTN(GetResult() == RAYCASTONSEGMENT_NOT_PROCESSED, ("Query has not been correctly initialized"));

	QueryUtils queryUtils(m_database, workingMemory, GetTraverseLogicUserData());
	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	KyFloat32 integerPrecision = databaseGenMetrics.m_integerPrecision;

	WorkingMemArray<NavTriangleRawPtr> crossedTriangles;
	WorkingMemArray<RawNavTagSubSegment> navTagSubSegments;

	Vec2f normalizedDir2d(m_segmentEndPos3f.x - m_segmentStartPos3f.x, m_segmentEndPos3f.y - m_segmentStartPos3f.y);
	normalizedDir2d.Normalize();

	if (queryUtils.FindTriangleFromPositionIfNotValid(m_startTrianglePtr, m_startPos3f, m_startIntegerPos, m_positionSpatializationRange) == false)
	{
		SetResult(RAYCASTONSEGMENT_DONE_START_OUTSIDE);
		return;
	}

	const NavTriangleRawPtr startTriangleRawPtr = m_startTrianglePtr.GetRawPtr();

	if (startTriangleRawPtr.CanBeTraversed<TLogic>(GetTraverseLogicUserData()) == false)
	{
		SetResult(RAYCASTONSEGMENT_DONE_START_NAVTAG_FORBIDDEN);
		return;
	}

	ScopeAutoSaveDynamicOutput scopeAutoSaveDynOutput(m_queryDynamicOutput);
	if ((GetDynamicOutputMode() & QUERY_SAVE_TRIANGLES) != 0)
	{
		crossedTriangles.Init(workingMemory);
		if (crossedTriangles.IsInitialized() == false)
		{
			SetResult(RAYCASTONSEGMENT_DONE_LACK_OF_WORKING_MEMORY);
			return;
		}

		scopeAutoSaveDynOutput.SetNavTriangleRawPtrs(&crossedTriangles);

		if (KY_FAILED(crossedTriangles.PushBack(startTriangleRawPtr)))
		{
			KY_LOG_WARNING( ("This query reached the maximum size of working memory"));
			SetResult(RAYCASTONSEGMENT_DONE_LACK_OF_WORKING_MEMORY);
			return;
		}
	}

	if ((GetDynamicOutputMode() & QUERY_SAVE_NAVTAGSUBSEGMENTS) != 0)
	{
		navTagSubSegments.Init(workingMemory);
		if (navTagSubSegments.IsInitialized() == false)
		{
			SetResult(RAYCASTONSEGMENT_DONE_LACK_OF_WORKING_MEMORY);
			return;
		}

		scopeAutoSaveDynOutput.SetRawNavTagSubSegments(&navTagSubSegments);
	}
	const Vec2f direction2f(normalizedDir2d * m_maxDist);
	Vec3f destPos3f(m_startPos3f.x + direction2f.x, m_startPos3f.y + direction2f.y, m_startPos3f.z);

	const WorldIntegerPos destIntegerPos(databaseGenMetrics.GetWorldIntegerPosFromVec3f(destPos3f));

	RayQueryUtils rayQueryUtils(m_startPos3f, destPos3f, GetTraverseLogicUserData(), 
		GetDynamicOutputMode(), &crossedTriangles, &navTagSubSegments);

	const bool storePropagationData =
		(GetDynamicOutputMode() & QUERY_SAVE_TRIANGLES       ) != 0 ||
		(GetDynamicOutputMode() & QUERY_SAVE_NAVTAGSUBSEGMENTS) != 0;

	if (destIntegerPos == m_startIntegerPos)
	{
		// Nothing to do
		m_arrivalPos3f = m_startPos3f;
		m_arrivalIntegerPos = m_startIntegerPos;
		m_arrivalTrianglePtr = m_startTrianglePtr;
		SetResult(RAYCASTONSEGMENT_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED);
		return;
	}

	RawNavTagSubSegment currentNavTagSubSegment;
	currentNavTagSubSegment.m_navTriangleRawPtr = startTriangleRawPtr;
	currentNavTagSubSegment.m_entrancePos3f = m_startPos3f;

	const CellPos minCellPos(Min(m_segmentStartIntegerPos.m_cellPos.x, m_segmentEndIntegerPos.m_cellPos.x), Min(m_segmentStartIntegerPos.m_cellPos.y, m_segmentEndIntegerPos.m_cellPos.y));
	const CellPos maxCellPos(Max(m_segmentStartIntegerPos.m_cellPos.x, m_segmentEndIntegerPos.m_cellPos.x) + 1, Max(m_segmentStartIntegerPos.m_cellPos.y, m_segmentEndIntegerPos.m_cellPos.y) + 1);
	const CoordBox64 cellsIntegerBox(databaseGenMetrics.ComputeCellOrigin(minCellPos), databaseGenMetrics.ComputeCellOrigin(maxCellPos));
	if (Max(cellsIntegerBox.Max().x - cellsIntegerBox.Min().x, cellsIntegerBox.Max().y - cellsIntegerBox.Min().y) >= (1LL << 20))
	{
		KY_LOG_WARNING( ("startPos and destPos are too far from each other, overflow may occur during computation, result may be incoherent"));
		SetResult(RAYCASTONSEGMENT_DONE_QUERY_TOO_LONG);
		return;
	}

	const CoordPos64 segmentStartCoordPos64 = databaseGenMetrics.GetCoordPos64FromWorldIntegerPos(m_segmentStartIntegerPos);
	const CoordPos64 segmentEndCoordPos64   = databaseGenMetrics.GetCoordPos64FromWorldIntegerPos(m_segmentEndIntegerPos);
	CoordPos64 destCoordPos64               = databaseGenMetrics.GetCoordPos64FromWorldIntegerPos(destIntegerPos);

	const CoordPos64 direction(segmentEndCoordPos64 - segmentStartCoordPos64);
	const KyInt64 startToEndquareDist = DotProduct(direction, direction);

	CoordPos64 segmentStartToDest(destCoordPos64 - segmentStartCoordPos64);
	KyInt64 startToDestSquareDist = DotProduct(segmentStartToDest, segmentStartToDest);

	if (startToEndquareDist < startToDestSquareDist)
	{
		// we do not want to go betond the end of the segment !
		destPos3f = m_segmentEndPos3f;
		destCoordPos64 = segmentEndCoordPos64;
		segmentStartToDest = direction;
		startToDestSquareDist = startToEndquareDist;
	}

		KY_DEBUG_WARNINGN_IF(startToDestSquareDist > DotProduct(direction, direction),
		("You should not use this query is you intent to go beyond the and of the segment you pass as an input, this may due to float imprecision"));

	KyInt32 edgeNumberToCrossInTriangle = rayQueryUtils.GetFirstEdgeToCrossInStartTriangle(startTriangleRawPtr, segmentStartCoordPos64, segmentEndCoordPos64);
	const NavHalfEdgeIdx firstHaldEdgeIdxToCross = NavFloorBlob::NavTriangleIdxToNavHalfEdgeIdx(startTriangleRawPtr.GetTriangleIdx(), edgeNumberToCrossInTriangle);

	NavHalfEdgeRawPtr currentHalfEdgeRawPtrToCross(startTriangleRawPtr.m_navFloorRawPtr, firstHaldEdgeIdxToCross);

	NavFloor* statingNavFloor = startTriangleRawPtr.GetNavFloor();
	CoordPos64 currentCellOrigin = databaseGenMetrics.ComputeCellOrigin(statingNavFloor->GetCellPos());
	const NavFloorBlob* currentNavFloorBlob = statingNavFloor->GetNavFloorBlob();
	NavHalfEdge currentNavHalfEdge = currentNavFloorBlob->GetNavHalfEdge(firstHaldEdgeIdxToCross);

	// Main loop
	// we check if we are not about to go beyond the point if we go farther than the current triangle
	// i.e. we check if dest is beyong the edge we are about to cross
	NavHalfEdgeRawPtr currentPairHalfEdgeRawPtr;
	while (rayQueryUtils.IsEdgeBeyondDestPos(currentCellOrigin, *currentNavFloorBlob, currentHalfEdgeRawPtrToCross.GetHalfEdgeIdx(),
		segmentStartCoordPos64, direction, destCoordPos64, startToDestSquareDist) == false)
	{
		// ok, we can try to cross the edge
		if(currentHalfEdgeRawPtrToCross.IsHalfEdgeCrossable<TLogic>(GetTraverseLogicUserData(), currentNavHalfEdge,
			currentNavFloorBlob, currentPairHalfEdgeRawPtr))
		{
			// ok we can cross the edge and pass in the other triangle
			if (IsHalfEdgeAFloorOrCellBoundary(currentNavHalfEdge.GetHalfEdgeType()))
			{
				NavFloor* currentNavFloor = currentPairHalfEdgeRawPtr.GetNavFloor();
				if (IsHalfEdgeACellBoundary(currentNavHalfEdge.GetHalfEdgeType()))
					currentCellOrigin = databaseGenMetrics.ComputeCellOrigin(currentNavFloor->GetCellPos());

				currentNavFloorBlob = currentNavFloor->GetNavFloorBlob();
			}

			if (storePropagationData)
			{
				if (KY_FAILED(rayQueryUtils.StorePropagationData(integerPrecision, currentPairHalfEdgeRawPtr, currentNavTagSubSegment, 
					currentCellOrigin, currentNavFloorBlob)))
				{
					KY_LOG_WARNING( ("This query reached the maximum size of working memory"));
					SetResult(RAYCASTONSEGMENT_DONE_LACK_OF_WORKING_MEMORY);
					return;
				}
			}
			// before looping, we compute which edge we will cross in the triangle
			// for that, we look on which side of the direction the third vertex of the triangle is.
			const NavVertex& thirdVertexPosInFloor = currentNavFloorBlob->NavHalfEdgeIdxToThirdNavVertex(currentPairHalfEdgeRawPtr.GetHalfEdgeIdx());
			const CoordPos64 thirdVertexPos = currentCellOrigin + thirdVertexPosInFloor.GetCoordPos64();
			const KyInt64 crossProductToChooseTheGoodEdge = CrossProduct(thirdVertexPos - segmentStartCoordPos64, direction);

			if (crossProductToChooseTheGoodEdge > 0)
				// go on the left of third
				// currentHalfEdgeSafeIdToCross = currentPairHalfEdgeSafeId.GetPrevHalfEdgeSafeId();
				currentPairHalfEdgeRawPtr.GetPrevHalfEdgeRawPtr(currentHalfEdgeRawPtrToCross);
			else
				// go on the right of third
				// currentHalfEdgeSafeIdToCross = currentPairHalfEdgeSafeId.GetNextHalfEdge();
				currentPairHalfEdgeRawPtr.GetNextHalfEdgeRawPtr(currentHalfEdgeRawPtrToCross);

			currentNavHalfEdge = currentNavFloorBlob->GetNavHalfEdge(currentHalfEdgeRawPtrToCross.GetHalfEdgeIdx());

			continue;
		}

		// outch ! we hit a border, we stop !
		RayCast_BorderHit(queryUtils, currentHalfEdgeRawPtrToCross, destCoordPos64, currentNavTagSubSegment, crossedTriangles, navTagSubSegments);
		return;
	}

	RayCast_NoHit(currentHalfEdgeRawPtrToCross, destIntegerPos, destPos3f, currentNavTagSubSegment, navTagSubSegments);
}

template<class TLogic>
void RayCastOnSegmentQuery<TLogic>::RayCast_BorderHit(QueryUtils& queryUtils, const NavHalfEdgeRawPtr& halfEdgeIdRawPtrHit, const CoordPos64& destCoordPos64,
	RawNavTagSubSegment& navTagSubSegment, WorkingMemArray<NavTriangleRawPtr>& crossedTriangles, WorkingMemArray<RawNavTagSubSegment>& navTagSubSegments)
{
	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	const KyFloat32 integerPrecision = databaseGenMetrics.m_integerPrecision;

	NavFloor* floor = halfEdgeIdRawPtrHit.GetNavFloor();
	const CellPos& cellPos = floor->GetCellPos();

	// before return, we retrieve the edge hit and we move back the collision point
	// the half-edge hit is defined by currentTrianglePtr and edgeToCross
	m_collisionHalfEdgePtr.Set(NavFloorPtr(floor), halfEdgeIdRawPtrHit.GetHalfEdgeIdx());

	const CoordPos64 segmentStartCoordPos64 = databaseGenMetrics.GetCoordPos64FromWorldIntegerPos(m_segmentStartIntegerPos);
	const CoordPos64 cellOrigin(databaseGenMetrics.ComputeCellOrigin(cellPos));

	const NavHalfEdgeIdx halfEdgeIdx = halfEdgeIdRawPtrHit.GetHalfEdgeIdx();

	const NavFloorBlob* navFloorBlob = floor->GetNavFloorBlob();

	const NavVertexIdx edgeStartIdx = navFloorBlob->NavHalfEdgeIdxToStartNavVertexIdx(halfEdgeIdx);
	const NavVertexIdx edgeEndIdx   = navFloorBlob->NavHalfEdgeIdxToEndNavVertexIdx(halfEdgeIdx);
	const NavVertexIdx edgeThirdIdx = navFloorBlob->NavHalfEdgeIdxToThirdNavVertexIdx(halfEdgeIdx);

	const NavVertex* const navVertices = navFloorBlob->m_navVertices.GetValues();

	const CoordPos64 edgeStartPos(cellOrigin + navVertices[edgeStartIdx].GetCoordPos64());
	const CoordPos64 edgeEndPos  (cellOrigin + navVertices[edgeEndIdx  ].GetCoordPos64());
	const CoordPos64 edgeThirdPos(cellOrigin + navVertices[edgeThirdIdx].GetCoordPos64());

	const KyFloat32* const navVerticesAltitudes = navFloorBlob->m_navVerticesAltitudes.GetValues();
	const Vec3f edgeStartPos3f(edgeStartPos.x * integerPrecision, edgeStartPos.y * integerPrecision, navVerticesAltitudes[edgeStartIdx]);
	const Vec3f edgeEndPos3f  (edgeEndPos.x   * integerPrecision, edgeEndPos.y   * integerPrecision, navVerticesAltitudes[edgeEndIdx]);
	const Vec3f edgeThirdPos3f(edgeThirdPos.x * integerPrecision, edgeThirdPos.y * integerPrecision, navVerticesAltitudes[edgeThirdIdx]);

	CoordPos64 collisionCoordPos64;
	bool parallelEdges;
	if (Intersections::LineVsLine2d(edgeStartPos, edgeEndPos, segmentStartCoordPos64, destCoordPos64, collisionCoordPos64, parallelEdges) == false)
	{
		KY_LOG_ERROR(("Unknown error in RayCast Query. Check float precision issue with the function EdgeIsBeyondDestPos"));
		SetResult(RAYCASTONSEGMENT_DONE_UNKNOWN_ERROR);
		return;
	}

	const CoordPos64 startCoordPos64 = databaseGenMetrics.GetCoordPos64FromWorldIntegerPos(m_startIntegerPos);

	if (parallelEdges)
	{
		// lines are parallel. In this case we return edgeStartPos as intersection but it is not sufficent in our case
		KY_ASSERT(collisionCoordPos64 == edgeStartPos);
		collisionCoordPos64 = BaseRayCastQuery::ResolveCollisionPointWhenPointsAreAligned(edgeStartPos, edgeEndPos, startCoordPos64, destCoordPos64);
	}


	m_collisionPos3f.x = collisionCoordPos64.x * integerPrecision;
	m_collisionPos3f.y = collisionCoordPos64.y * integerPrecision;
	m_collisionPos3f.z = Intersections::ComputeAltitudeOfPointInTriangle(m_collisionPos3f, edgeStartPos3f, edgeEndPos3f, edgeThirdPos3f);

	if (KY_FAILED(queryUtils.FindValidPositionFromIntegerIntersection<TLogic>(m_collisionPos3f, m_collisionHalfEdgePtr,
		m_arrivalIntegerPos, m_arrivalTrianglePtr)))
	{
		// if failed, m_arrivalIntegerPos did not changed
		m_arrivalPos3f = m_collisionPos3f;

		if ((GetDynamicOutputMode() & QUERY_SAVE_NAVTAGSUBSEGMENTS) != 0)
		{
			navTagSubSegment.m_exitPos3f = m_collisionPos3f;
			if(KY_FAILED(navTagSubSegments.PushBack(navTagSubSegment)))
			{
				KY_LOG_WARNING( ("This query reached the maximum size of working memory"));
				SetResult(RAYCASTONSEGMENT_DONE_LACK_OF_WORKING_MEMORY);
				return;
			}
		}

		SetResult(RAYCASTONSEGMENT_DONE_ARRIVAL_ERROR);
		return;
	}

	CoordPos64 arrivalCoordPos64 = databaseGenMetrics.GetCoordPos64FromWorldIntegerPos(m_arrivalIntegerPos);

	m_arrivalPos3f.x = arrivalCoordPos64.x * integerPrecision;
	m_arrivalPos3f.y = arrivalCoordPos64.y * integerPrecision;
	m_arrivalPos3f.z = Intersections::ComputeAltitudeOfPointInTriangle(m_collisionPos3f, edgeStartPos3f, edgeEndPos3f, edgeThirdPos3f);

	const KyInt64 dotProduct2dToNotGoBackBeforeStart = DotProduct(destCoordPos64 - startCoordPos64, arrivalCoordPos64 - startCoordPos64);

	if (dotProduct2dToNotGoBackBeforeStart < 0)
	{
		//we move back beyond m_startPos, which is bad : we return StartPos
		if ((GetDynamicOutputMode() & QUERY_SAVE_NAVTAGSUBSEGMENTS) != 0)
		{
			navTagSubSegment.m_exitPos3f = m_startPos3f;
			if(KY_FAILED(navTagSubSegments.PushBack(navTagSubSegment)))
			{
				KY_LOG_WARNING( ("This query reached the maximum size of working memory"));
				SetResult(RAYCASTONSEGMENT_DONE_LACK_OF_WORKING_MEMORY);
				return;
			}
		}

		m_arrivalPos3f = m_startPos3f;
		m_arrivalIntegerPos = m_startIntegerPos;
		m_arrivalTrianglePtr = m_startTrianglePtr;
		SetResult(RAYCASTONSEGMENT_DONE_CANNOT_MOVE);
		return;
	}

	if ((GetDynamicOutputMode() & QUERY_SAVE_TRIANGLES) != 0)
	{
		if (m_arrivalTrianglePtr.m_navFloorPtr.GetNavFloor_Unsafe() != halfEdgeIdRawPtrHit.m_navFloorRawPtr.GetNavFloor() ||
			m_arrivalTrianglePtr.GetTriangleIdx() != NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(halfEdgeIdRawPtrHit.GetHalfEdgeIdx()))
		{
			if (KY_FAILED(crossedTriangles.PushBack(m_arrivalTrianglePtr.GetRawPtr())))
			{
				KY_LOG_WARNING( ("This query reached the maximum size of working memory"));
				SetResult(RAYCASTONSEGMENT_DONE_LACK_OF_WORKING_MEMORY);
				return;
			}
		}
	}

	if ((GetDynamicOutputMode() & QUERY_SAVE_NAVTAGSUBSEGMENTS) != 0)
	{
		navTagSubSegment.m_exitPos3f = m_arrivalPos3f;
		if(KY_FAILED(navTagSubSegments.PushBack(navTagSubSegment)))
		{
			KY_LOG_WARNING( ("This query reached the maximum size of working memory"));
			SetResult(RAYCASTONSEGMENT_DONE_LACK_OF_WORKING_MEMORY);
			return;
		}
	}

	SetResult(RAYCASTONSEGMENT_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION);
}

}
