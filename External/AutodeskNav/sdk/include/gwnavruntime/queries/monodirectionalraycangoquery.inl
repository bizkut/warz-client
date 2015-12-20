/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY


namespace Kaim
{
template<class TLogic>
KY_INLINE MonodirectionalRayCanGoQuery<TLogic>::MonodirectionalRayCanGoQuery() : BaseRayCanGoQuery() {}

template<class TLogic>
KY_INLINE void MonodirectionalRayCanGoQuery<TLogic>::BindToDatabase(Database* database)
{
	BaseRayCanGoQuery::BindToDatabase(database);
}

template<class TLogic>
KY_INLINE void MonodirectionalRayCanGoQuery<TLogic>::Initialize(const Vec3f& startPos, const Vec3f& destPos)
{
	BaseRayCanGoQuery::Initialize(startPos, destPos);
}

template<class TLogic>
KY_INLINE void MonodirectionalRayCanGoQuery<TLogic>::SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr)
{ BaseRayCanGoQuery::SetStartTrianglePtr(startTrianglePtr);   }
template<class TLogic>
KY_INLINE void MonodirectionalRayCanGoQuery<TLogic>::SetStartIntegerPos(const WorldIntegerPos& startIntegerPos)
{ BaseRayCanGoQuery::SetStartIntegerPos(startIntegerPos);     }
template<class TLogic>
KY_INLINE void MonodirectionalRayCanGoQuery<TLogic>::SetDestIntegerPos(const WorldIntegerPos& destIntegerPos)
{ BaseRayCanGoQuery::SetDestIntegerPos(destIntegerPos);       }

template<class TLogic>
KY_INLINE void MonodirectionalRayCanGoQuery<TLogic>::SetMarginMode(RayCanGoMarginMode marginMode)
{
	BaseRayCanGoQuery::SetMarginMode(marginMode);
}

template<class TLogic>
KY_INLINE void MonodirectionalRayCanGoQuery<TLogic>::SetDynamicOutputMode(DynamicOutputMode savingMode)
{ BaseRayCanGoQuery::SetDynamicOutputMode(savingMode);        }

template<class TLogic>
KY_INLINE void MonodirectionalRayCanGoQuery<TLogic>::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput)
{
	BaseRayCanGoQuery::SetQueryDynamicOutput(queryDynamicOutput);
}
template<class TLogic>
KY_INLINE void MonodirectionalRayCanGoQuery<TLogic>::SetComputeCostMode(ComputeCostMode computeCostMode)
{
	BaseRayCanGoQuery::SetComputeCostMode(computeCostMode);
}


template<class TLogic>
KY_INLINE void MonodirectionalRayCanGoQuery<TLogic>::SetPositionSpatializationRange(
	const PositionSpatializationRange& positionSpatializationRange)
{
	BaseRayCanGoQuery::SetPositionSpatializationRange(positionSpatializationRange);
}

template<class TLogic>
KY_INLINE RayCanGoQueryResult MonodirectionalRayCanGoQuery<TLogic>::GetResult() const
{
	return BaseRayCanGoQuery::GetResult();
}
template<class TLogic>
KY_INLINE DynamicOutputMode MonodirectionalRayCanGoQuery<TLogic>::GetDynamicOutputMode() const
{
	return BaseRayCanGoQuery::GetDynamicOutputMode();
}

template<class TLogic>
KY_INLINE const Vec3f& MonodirectionalRayCanGoQuery<TLogic>::GetStartPos() const
{
	return BaseRayCanGoQuery::GetStartPos();
}
template<class TLogic>
KY_INLINE const Vec3f& MonodirectionalRayCanGoQuery<TLogic>::GetDestPos() const
{
	return BaseRayCanGoQuery::GetDestPos();
}
template<class TLogic>
KY_INLINE const NavTrianglePtr& MonodirectionalRayCanGoQuery<TLogic>::GetStartTrianglePtr() const
{
	return BaseRayCanGoQuery::GetStartTrianglePtr();
}
template<class TLogic>
KY_INLINE const NavTrianglePtr& MonodirectionalRayCanGoQuery<TLogic>::GetDestTrianglePtr() const
{
	return BaseRayCanGoQuery::GetDestTrianglePtr();
}
template<class TLogic>
KY_INLINE QueryDynamicOutput* MonodirectionalRayCanGoQuery<TLogic>::GetQueryDynamicOutput() const
{
	return BaseRayCanGoQuery::GetQueryDynamicOutput();
}
template<class TLogic>
KY_INLINE const WorldIntegerPos& MonodirectionalRayCanGoQuery<TLogic>::GetStartIntegerPos() const
{
	return BaseRayCanGoQuery::GetStartIntegerPos();
}
template<class TLogic>
KY_INLINE const WorldIntegerPos& MonodirectionalRayCanGoQuery<TLogic>::GetDestIntegerPos() const
{
	return BaseRayCanGoQuery::GetDestIntegerPos();
}
template<class TLogic>
KY_INLINE ComputeCostMode MonodirectionalRayCanGoQuery<TLogic>::GetComputeCostMode() const
{
	return BaseRayCanGoQuery::GetComputeCostMode();
}
template<class TLogic>
KY_INLINE KyFloat32 MonodirectionalRayCanGoQuery<TLogic>::GetComputedCost() const
{
	return BaseRayCanGoQuery::GetComputedCost(); 
}

template<class TLogic>
KY_INLINE const PositionSpatializationRange& MonodirectionalRayCanGoQuery<TLogic>::GetPositionSpatializationRange() const
{
	return BaseRayCanGoQuery::GetPositionSpatializationRange();
}

template<class TLogic>
KY_INLINE void MonodirectionalRayCanGoQuery<TLogic>::Advance(WorkingMemory* workingMemory)
{
	PerformQuery(workingMemory);
	m_processStatus = QueryDone;
}

template<class TLogic>
inline void MonodirectionalRayCanGoQuery<TLogic>::PerformQuery(WorkingMemory* workingMemory)
{
	if (GetResult() != RAYCANGO_NOT_PROCESSED)
		return;

	if (m_database->IsClear())
	{
		SetResult(RAYCANGO_DONE_START_OUTSIDE);
		return;
	}

	if (workingMemory == KY_NULL)
		workingMemory = m_database->GetWorkingMemory();

	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	databaseGenMetrics.GetWorldIntegerPosFromVec3f(m_startPos3f, m_startIntegerPos);
	databaseGenMetrics.GetWorldIntegerPosFromVec3f(m_destPos3f, m_destIntegerPos);

	PerformQueryWithInputCoordPos(workingMemory);
}


template<class TLogic>
inline void MonodirectionalRayCanGoQuery<TLogic>::PerformQueryWithInputCoordPos(WorkingMemory* workingMemory)
{
	ScopedPerformedQueryCounter(GetStaticType(), m_database, m_performQueryStat);
	KY_DEBUG_ASSERTN(GetResult() == RAYCANGO_NOT_PROCESSED, ("Query has not been correctly initialized"));

	QueryUtils queryUtils(m_database, workingMemory, GetTraverseLogicUserData());
	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	KyFloat32 integerPrecision = databaseGenMetrics.m_integerPrecision;

	WorkingMemArray<NavTriangleRawPtr> crossedTriangles;
	WorkingMemArray<RawNavTagSubSegment> navTagSubSegments;
	WorkingMemArray<RawHalfEdgeIntersection> halfEdgeIntersections;

	ScopeAutoSaveDynamicOutput scopeAutoSaveDynOutput(m_queryDynamicOutput);
	if (InitDynamicOutput(workingMemory, crossedTriangles, navTagSubSegments, halfEdgeIntersections, scopeAutoSaveDynOutput) == KY_ERROR)
		return;

	KyFloat32 currentCostMultipler = 1.f;
	KyFloat32* pointerToCostMultiplier = GetComputeCostMode() == QUERY_DO_NOT_COMPUTE_COST ? KY_NULL : &currentCostMultipler;

	if (queryUtils.FindTriangleFromPositionIfNotValid(m_startTrianglePtr, m_startPos3f, m_startIntegerPos, m_positionSpatializationRange) == false)
	{
		SetResult(RAYCANGO_DONE_START_OUTSIDE);
		return;
	}

	const NavTriangleRawPtr startTriangleRawPtr = m_startTrianglePtr.GetRawPtr();
	if (startTriangleRawPtr.CanBeTraversed<TLogic>(GetTraverseLogicUserData(), pointerToCostMultiplier) == false)
	{
		SetResult(RAYCANGO_DONE_START_NAVTAG_FORBIDDEN);
		return;
	}

	if (PushTriangleInDynamicOutput(crossedTriangles, startTriangleRawPtr) == KY_ERROR)
		return;

	RayQueryUtils rayQueryUtils(m_startPos3f, m_destPos3f, GetTraverseLogicUserData(), 
		GetDynamicOutputMode(), &crossedTriangles, &navTagSubSegments, &halfEdgeIntersections, GetComputeCostMode(), currentCostMultipler);
	m_cost = 0.f;
	const bool needToCallStorePropagationDataFunction =
		(GetDynamicOutputMode() & QUERY_SAVE_TRIANGLES            ) != 0 ||
		(GetDynamicOutputMode() & QUERY_SAVE_NAVTAGSUBSEGMENTS    ) != 0 ||
		(GetDynamicOutputMode() & QUERY_SAVE_HALFEDGEINTERSECTIONS) != 0 ||
		GetComputeCostMode() != QUERY_DO_NOT_COMPUTE_COST;

	
	// not is same pos, use CoordPos64
#if defined (KY_BUILD_DEBUG)
	const CellPos minCellPos(Min(m_startIntegerPos.m_cellPos.x, m_destIntegerPos.m_cellPos.x), Min(m_startIntegerPos.m_cellPos.y, m_destIntegerPos.m_cellPos.y));
	const CellPos maxCellPos(Max(m_startIntegerPos.m_cellPos.x, m_destIntegerPos.m_cellPos.x) + 1, Max(m_startIntegerPos.m_cellPos.y, m_destIntegerPos.m_cellPos.y) + 1);
	const CoordBox64 cellsIntegerBox(databaseGenMetrics.ComputeCellOrigin(minCellPos), databaseGenMetrics.ComputeCellOrigin(maxCellPos));
	if (Max(cellsIntegerBox.Max().x - cellsIntegerBox.Min().x, cellsIntegerBox.Max().y - cellsIntegerBox.Min().y) >= (1LL << 31))
		KY_LOG_WARNING( ("startPos and destPos are too far from each other, overflow may occur during computation, result may be incoherent"));
#endif

	RawNavTagSubSegment currentNavTagSubSegment;
	currentNavTagSubSegment.m_navTriangleRawPtr = startTriangleRawPtr;
	currentNavTagSubSegment.m_entrancePos3f = m_startPos3f;

	const CoordPos64 startCoordPos = databaseGenMetrics.GetCoordPos64FromWorldIntegerPos(m_startIntegerPos);
	const CoordPos64 destCoordPos = databaseGenMetrics.GetCoordPos64FromWorldIntegerPos(m_destIntegerPos);

	const CoordPos64 direction(destCoordPos - startCoordPos); // 31 bits
	const KyInt64 startToDestSquareDist = DotProduct(direction, direction);

	KyInt32 edgeNumberToCrossInTriangle = rayQueryUtils.GetFirstEdgeToCrossInStartTriangle(startTriangleRawPtr, startCoordPos, destCoordPos);
	const NavHalfEdgeIdx firstHaldEdgeIdxToCross = NavFloorBlob::NavTriangleIdxToNavHalfEdgeIdx(startTriangleRawPtr.GetTriangleIdx(), edgeNumberToCrossInTriangle);

	NavHalfEdgeRawPtr currentHalfEdgeRawPtrToCross(startTriangleRawPtr.m_navFloorRawPtr, firstHaldEdgeIdxToCross);

	NavFloor* statingNavFloor = startTriangleRawPtr.GetNavFloor();
	CoordPos64 currentCellOrigin = databaseGenMetrics.ComputeCellOrigin(statingNavFloor->GetCellPos());
	const NavFloorBlob* currentNavFloorBlob = statingNavFloor->GetNavFloorBlob();
	NavHalfEdge currentNavHalfEdge = currentNavFloorBlob->GetNavHalfEdge(firstHaldEdgeIdxToCross);

	const KyUInt32 marginUInt = m_database->GetRayCanGoMarginInt(m_marginMode);
	const KyUInt64 sqMarginUInt = marginUInt * marginUInt;
	const KyUInt64 sqCapsuleRadiusUInt = sqMarginUInt;
	const KyUInt32 recoveryDistance = 2 * marginUInt;
	
	ThinCapsuleWithExtentOffset capsuleParams(startCoordPos, destCoordPos, sqCapsuleRadiusUInt, recoveryDistance);
	if (sqCapsuleRadiusUInt > 0)
	{
		NavHalfEdgeRawPtr otherTriangleHalfEdgeRawPtr;

		currentHalfEdgeRawPtrToCross.GetNextHalfEdgeRawPtr(otherTriangleHalfEdgeRawPtr);
		if (queryUtils.IsHalfEdgeCompatibleWithThinCapsule<TLogic>(otherTriangleHalfEdgeRawPtr, capsuleParams) == false)
		{
			SetResult(RAYCANGO_DONE_BORDER_DETECTED_WITHIN_MARGIN);
			return;
		}

		currentHalfEdgeRawPtrToCross.GetPrevHalfEdgeRawPtr(otherTriangleHalfEdgeRawPtr);
		if (queryUtils.IsHalfEdgeCompatibleWithThinCapsule<TLogic>(otherTriangleHalfEdgeRawPtr, capsuleParams) == false)
		{
			SetResult(RAYCANGO_DONE_BORDER_DETECTED_WITHIN_MARGIN);
			return;
		}
	}

	// Main loop
	// we check if we are not about to go beyond the point if we go farther than the current triangle
	// i.e. we check if dest is beyong the edge we are about to cross
	NavHalfEdgeRawPtr currentPairHalfEdgeRawPtr;
	while (rayQueryUtils.IsEdgeBeyondDestPos(currentCellOrigin, *currentNavFloorBlob, currentHalfEdgeRawPtrToCross.GetHalfEdgeIdx(),
		startCoordPos, direction, destCoordPos, startToDestSquareDist) == false)
	{
		// ok, we can try to cross the edge

		if(currentHalfEdgeRawPtrToCross.IsHalfEdgeOneWayCrossable<TLogic>(GetTraverseLogicUserData(), currentNavHalfEdge,
			currentNavFloorBlob, currentPairHalfEdgeRawPtr, pointerToCostMultiplier))
		{
			// ok we can cross the edge and pass in the other triangle

			if (IsHalfEdgeAFloorOrCellBoundary(currentNavHalfEdge.GetHalfEdgeType()))
			{
				NavFloor* currentNavFloor = currentPairHalfEdgeRawPtr.GetNavFloor();
				if (IsHalfEdgeACellBoundary(currentNavHalfEdge.GetHalfEdgeType()))
					currentCellOrigin = databaseGenMetrics.ComputeCellOrigin(currentNavFloor->GetCellPos());

				currentNavFloorBlob = currentNavFloor->GetNavFloorBlob();
			}

			if (needToCallStorePropagationDataFunction)
			{
				if (KY_FAILED(rayQueryUtils.StoreOrComputeCostFromPropagationData<TLogic>(integerPrecision,
					currentPairHalfEdgeRawPtr, currentNavTagSubSegment, currentCellOrigin, currentNavFloorBlob, currentCostMultipler, m_cost)))
				{
					KY_LOG_WARNING( ("This query reached the maximum size of working memory"));
					SetResult(RAYCANGO_DONE_LACK_OF_WORKING_MEMORY);
					return;
				}
			}

			// before looping, we compute which edge we will cross in the triangle
			// for that, we look on which side of the direction the third vertex of the triangle is.
			const NavVertex& thirdVertexPosInFloor = currentNavFloorBlob->NavHalfEdgeIdxToThirdNavVertex(currentPairHalfEdgeRawPtr.GetHalfEdgeIdx());
			const CoordPos64 thirdVertexPos = currentCellOrigin + thirdVertexPosInFloor.GetCoordPos64();
			const KyInt64 crossProductToChooseTheGoodEdge = CrossProduct(thirdVertexPos - startCoordPos, direction);

			if (sqCapsuleRadiusUInt > 0)
			{
				NavHalfEdgeRawPtr otherTriangleHalfEdgeRawPtr;
				if (crossProductToChooseTheGoodEdge > 0)
					currentPairHalfEdgeRawPtr.GetNextHalfEdgeRawPtr(otherTriangleHalfEdgeRawPtr);
				else
					currentPairHalfEdgeRawPtr.GetPrevHalfEdgeRawPtr(otherTriangleHalfEdgeRawPtr);

				if (queryUtils.IsHalfEdgeCompatibleWithThinCapsule<TLogic>(otherTriangleHalfEdgeRawPtr, capsuleParams) == false)
				{
					SetResult(RAYCANGO_DONE_BORDER_DETECTED_WITHIN_MARGIN);
					return;
				}
			}

			if (crossProductToChooseTheGoodEdge > 0)
				// go on the left of third
				currentPairHalfEdgeRawPtr.GetPrevHalfEdgeRawPtr(currentHalfEdgeRawPtrToCross);
			else
				// go on the right of third
				currentPairHalfEdgeRawPtr.GetNextHalfEdgeRawPtr(currentHalfEdgeRawPtrToCross);

			currentNavHalfEdge = currentNavFloorBlob->GetNavHalfEdge(currentHalfEdgeRawPtrToCross.GetHalfEdgeIdx());
			continue;
		}

		SetResult(RAYCANGO_DONE_COLLISION_DETECTED);
		return;
	}

	if (sqCapsuleRadiusUInt > 0)
	{
		if (currentPairHalfEdgeRawPtr.IsValid())
		{
			NavHalfEdgeRawPtr otherTriangleHalfEdgeRawPtr;

			currentPairHalfEdgeRawPtr.GetNextHalfEdgeRawPtr(otherTriangleHalfEdgeRawPtr);
			if (queryUtils.IsHalfEdgeCompatibleWithThinCapsule<TLogic>(otherTriangleHalfEdgeRawPtr, capsuleParams) == false)
			{
				SetResult(RAYCANGO_DONE_BORDER_DETECTED_WITHIN_MARGIN);
				return;
			}

			currentPairHalfEdgeRawPtr.GetPrevHalfEdgeRawPtr(otherTriangleHalfEdgeRawPtr);
			if (queryUtils.IsHalfEdgeCompatibleWithThinCapsule<TLogic>(otherTriangleHalfEdgeRawPtr, capsuleParams) == false)
			{
				SetResult(RAYCANGO_DONE_BORDER_DETECTED_WITHIN_MARGIN);
				return;
			}
		}
		else
		{
			// the first halfEdge to be crossed has not been crossed (dest is in the start triangle), check it is compatible with margin
			if (queryUtils.IsHalfEdgeCompatibleWithThinCapsule<TLogic>(currentHalfEdgeRawPtrToCross, capsuleParams) == false)
			{
				SetResult(RAYCANGO_DONE_BORDER_DETECTED_WITHIN_MARGIN);
				return;
			}
		}
	}

	RayCanGo_NoHit(currentHalfEdgeRawPtrToCross, currentNavTagSubSegment, navTagSubSegments);

	if (GetComputeCostMode() == QUERY_COMPUTE_COST_ALONG_3DAXIS)
		rayQueryUtils.UpdateCostAlongAxis(m_destPos3f, currentCostMultipler, m_cost);
}


}
