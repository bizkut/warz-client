/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY


namespace Kaim
{

template<class TLogic>
KY_INLINE NearestBorderHalfEdgeFromPosQuery<TLogic>::NearestBorderHalfEdgeFromPosQuery() :
	BaseNearestBorderHalfEdgeFromPosQuery() {}

template<class TLogic>
KY_INLINE void NearestBorderHalfEdgeFromPosQuery<TLogic>::BindToDatabase(Database* database)
{
	BaseNearestBorderHalfEdgeFromPosQuery::BindToDatabase(database);
}

template<class TLogic>
KY_INLINE void NearestBorderHalfEdgeFromPosQuery<TLogic>::Initialize(const Vec3f& inputPos)
{
	BaseNearestBorderHalfEdgeFromPosQuery::Initialize(inputPos);
}

template<class TLogic>
KY_INLINE void NearestBorderHalfEdgeFromPosQuery<TLogic>::SetHorizontalTolerance(KyFloat32 horizontalTolerance)
{
	BaseNearestBorderHalfEdgeFromPosQuery::SetHorizontalTolerance(horizontalTolerance);
}

template<class TLogic>
KY_INLINE void NearestBorderHalfEdgeFromPosQuery<TLogic>::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	BaseNearestBorderHalfEdgeFromPosQuery::SetPositionSpatializationRange(positionSpatializationRange);
}

template<class TLogic>
KY_INLINE void NearestBorderHalfEdgeFromPosQuery<TLogic>::SetInputIntegerPos(const WorldIntegerPos& inputIntegerPos)
{
	BaseNearestBorderHalfEdgeFromPosQuery::SetInputIntegerPos(inputIntegerPos);
}


template<class TLogic>
KY_INLINE NearestBorderHalfEdgeFromPosQueryResult NearestBorderHalfEdgeFromPosQuery<TLogic>::GetResult() const
{
	return BaseNearestBorderHalfEdgeFromPosQuery::GetResult();
}
template<class TLogic>
KY_INLINE const Vec3f& NearestBorderHalfEdgeFromPosQuery<TLogic>::GetInputPos() const
{
	return BaseNearestBorderHalfEdgeFromPosQuery::GetInputPos(); 
}
template<class TLogic>
KY_INLINE const Vec3f& NearestBorderHalfEdgeFromPosQuery<TLogic>::GetNearestPosOnHalfEdge() const
{
	return BaseNearestBorderHalfEdgeFromPosQuery::GetNearestPosOnHalfEdge(); 
}
template<class TLogic>
KY_INLINE const NavHalfEdgePtr& NearestBorderHalfEdgeFromPosQuery<TLogic>::GetNearestHalfEdgePtrOnBorder() const
{
	return BaseNearestBorderHalfEdgeFromPosQuery::GetNearestHalfEdgePtrOnBorder();
}
template<class TLogic>
KY_INLINE const NavHalfEdgePtr& NearestBorderHalfEdgeFromPosQuery<TLogic>::GetNextHalfEdgePtrAlongBorder() const
{
	return BaseNearestBorderHalfEdgeFromPosQuery::GetNextHalfEdgePtrAlongBorder();
}
template<class TLogic>
KY_INLINE const NavHalfEdgePtr& NearestBorderHalfEdgeFromPosQuery<TLogic>::GetPrevHalfEdgePtrAlongBorder() const
{
	return BaseNearestBorderHalfEdgeFromPosQuery::GetPrevHalfEdgePtrAlongBorder();
}
template<class TLogic>
KY_INLINE KyFloat32 NearestBorderHalfEdgeFromPosQuery<TLogic>::GetHorizontalTolerance() const
{
	return BaseNearestBorderHalfEdgeFromPosQuery::GetHorizontalTolerance();
}
template<class TLogic>
KY_INLINE const PositionSpatializationRange& NearestBorderHalfEdgeFromPosQuery<TLogic>::GetPositionSpatializationRange() const
{
	return BaseNearestBorderHalfEdgeFromPosQuery::GetPositionSpatializationRange();
}
template<class TLogic>
KY_INLINE KyFloat32 NearestBorderHalfEdgeFromPosQuery<TLogic>::GetSquareDistFromNearestBorder() const
{
	return BaseNearestBorderHalfEdgeFromPosQuery::GetSquareDistFromNearestBorder();
}
template<class TLogic>
KY_INLINE const WorldIntegerPos& NearestBorderHalfEdgeFromPosQuery<TLogic>::GetInputIntegerPos() const
{
	return BaseNearestBorderHalfEdgeFromPosQuery::GetInputIntegerPos(); 
}
template<class TLogic>
KY_INLINE const WorldIntegerPos& NearestBorderHalfEdgeFromPosQuery<TLogic>::GetNearestIntegerPosOnHalfEdge() const
{
	return BaseNearestBorderHalfEdgeFromPosQuery::GetNearestIntegerPosOnHalfEdge();
}


template<class TLogic>
KY_INLINE void NearestBorderHalfEdgeFromPosQuery<TLogic>::Advance(WorkingMemory* workingMemory)
{
	PerformQuery(workingMemory);
	m_processStatus = QueryDone;
}

template<class TLogic>
inline void NearestBorderHalfEdgeFromPosQuery<TLogic>::PerformQuery(WorkingMemory* workingMemory)
{
	if (GetResult() != NEARESTHALFEDGE_NOT_PROCESSED)
		return;

	if (m_database->IsClear())
	{
		SetResult(NEARESTHALFEDGE_HALFEDGE_NOT_FOUND);
		return;
	}

	if (workingMemory == KY_NULL)
		workingMemory = m_database->GetWorkingMemory();

	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	databaseGenMetrics.GetWorldIntegerPosFromVec3f(m_inputPos3f, m_inputIntegerPos);

	return PerformQueryWithInputCoordPos(workingMemory);
}

template<class TLogic>
void NearestBorderHalfEdgeFromPosQuery<TLogic>::PerformQueryWithInputCoordPos(WorkingMemory* workingMemory)
{
	ScopedPerformedQueryCounter(GetStaticType(), m_database, m_performQueryStat);
	KY_DEBUG_ASSERTN(GetResult() == NEARESTHALFEDGE_NOT_PROCESSED, ("Query has not been correctly initialized"));

	ActiveData& activeData = *m_database->GetActiveData();
	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();

	WorkingMemArray<NavFloor*> filteredActiveFloor(workingMemory);

	if (filteredActiveFloor.IsInitialized() == false)
	{
		SetResult(NEARESTHALFEDGE_DONE_LACK_OF_WORKING_MEMORY);
		return;
	}

	const CoordPos64 inputCoordPos = databaseGenMetrics.GetCoordPos64FromWorldIntegerPos(m_inputIntegerPos);
	const KyInt64 horizontalAltitudeToleranceInt = databaseGenMetrics.GetNearestInteger64FromFloatValue(m_horizontalTolerance);

	const CoordPos64 diagonal2dOfSearchAABB(horizontalAltitudeToleranceInt, horizontalAltitudeToleranceInt);
	const CoordBox64 aABbox(inputCoordPos - diagonal2dOfSearchAABB, inputCoordPos + diagonal2dOfSearchAABB);
	CellBox cellBox;

	databaseGenMetrics.GetCellBoxOfAnAABBox(aABbox, cellBox);

	CellPos cellPos(cellBox.Min());
	const KyInt32 BoxMaxX = cellBox.Max().x;
	const KyInt32 BoxMaxY = cellBox.Max().y;
	const KyInt32 BoxMinY = cellBox.Min().y;

	NavHalfEdgeRawPtr nearestHalfEdgeRawPtr;

	for (; cellPos.x < BoxMaxX + 1; ++cellPos.x)
	{
		for (cellPos.y = BoxMinY; cellPos.y < BoxMaxY + 1; ++cellPos.y)
		{
			if(activeData.IsActiveNavFloorAtThisCellPos(cellPos) == false)
				continue;

			ActiveCell& activeCell = activeData.GetActiveCellFromCellPos_Unsafe(cellPos);
			const CoordPos64 cellOrigin(databaseGenMetrics.ComputeCellOrigin(cellPos));

			if (KY_FAILED(FindNavFloorsFromAltitudeRange(filteredActiveFloor, activeCell)))
			{
				SetResult(NEARESTHALFEDGE_DONE_LACK_OF_WORKING_MEMORY);
				return;
			}


			for (KyUInt32 indexInFilteredNavFloor = 0; indexInFilteredNavFloor < filteredActiveFloor.GetCount(); ++indexInFilteredNavFloor)
				FindNearestBorderHalfEdgeInFloor(filteredActiveFloor[indexInFilteredNavFloor], aABbox, inputCoordPos, cellOrigin, nearestHalfEdgeRawPtr);
		}
	}

	if (m_squareDistFromHalfEdge != KyFloat32MAXVAL)
	{
		const NavHalfEdgeRawPtr nextNavHalfEdgeRawPtr = nearestHalfEdgeRawPtr.GetNextNavHalfEdgeRawPtrAlongBorder<TLogic>(GetTraverseLogicUserData());
		const NavHalfEdgeRawPtr prevNavHalfEdgeRawPtr = nearestHalfEdgeRawPtr.GetPrevNavHalfEdgeRawPtrAlongBorder<TLogic>(GetTraverseLogicUserData());

		m_nearestHalfEdgeOnBorder.Set(NavFloorPtr(nearestHalfEdgeRawPtr.GetNavFloor()), nearestHalfEdgeRawPtr.GetHalfEdgeIdx());
		m_nextHalfEdgeOnBorder.Set(NavFloorPtr(nextNavHalfEdgeRawPtr.GetNavFloor()), nextNavHalfEdgeRawPtr.GetHalfEdgeIdx());
		m_prevHalfEdgeOnBorder.Set(NavFloorPtr(prevNavHalfEdgeRawPtr.GetNavFloor()), prevNavHalfEdgeRawPtr.GetHalfEdgeIdx());

		SetResult(NEARESTHALFEDGE_HALFEDGE_FOUND);
	}
	else
	{
		SetResult(NEARESTHALFEDGE_HALFEDGE_NOT_FOUND);
	}
}

template<class TLogic>
inline void NearestBorderHalfEdgeFromPosQuery<TLogic>::FindNearestBorderHalfEdgeInFloor(NavFloor* navFloor,
	const CoordBox64& aABbox, const CoordPos64& inputCoordPos, const CoordPos64& cellOrigin, NavHalfEdgeRawPtr& nearestHalfEdgeRawPtr)
{
	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	const KyFloat32 integerPrecision = databaseGenMetrics.m_integerPrecision;

	CoordPos64 closestPointOnSegment;
	KyInt64 squareDist2D;

	const NavFloorBlob* floorBlob = navFloor->GetNavFloorBlob();

	const KyUInt32 halfEdgeCount = floorBlob->GetNavHalfEdgeCount();
	const NavHalfEdge* navHalfEdges = floorBlob->m_navHalfEdges.GetValues();
	const NavVertex* navVertices = floorBlob->m_navVertices.GetValues();
	const KyFloat32* navVerticesAltitude = floorBlob->m_navVerticesAltitudes.GetValues();
	const NavFloorAABB& navFloorAABB = floorBlob->m_navFloorAABB;

	const CoordBox64 floorIntegerBox(cellOrigin + navFloorAABB.m_min.GetCoordPos64(), cellOrigin + navFloorAABB.m_max.GetCoordPos64());
	if (Intersections::AABBVsAABB2d(floorIntegerBox, aABbox) == false)
		return;

	const KyFloat32& rangeAboveNavMesh = m_positionSpatializationRange.m_rangeBelowPosition;
	const KyFloat32& rangeBelowNavMesh = m_positionSpatializationRange.m_rangeAbovePosition;

	for (NavHalfEdgeIdx halfEdgeIdx = 0; halfEdgeIdx < halfEdgeCount; ++halfEdgeIdx)
	{
		NavHalfEdgeRawPtr currentNavHalfEdgeRawPtr(navFloor, halfEdgeIdx);
		const NavHalfEdgeType edgeType = navHalfEdges[halfEdgeIdx].GetHalfEdgeType();

		if (edgeType == EDGETYPE_PAIRED)
			continue;

		const NavTriangleRawPtr triangle(navFloor, NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(halfEdgeIdx));
		if (triangle.CanBeTraversed<TLogic>(GetTraverseLogicUserData()) == false)
			continue;

		if(currentNavHalfEdgeRawPtr.IsHalfEdgeCrossable<TLogic>(GetTraverseLogicUserData()) == true)
			continue;

		const NavVertexIdx startIdx = floorBlob->NavHalfEdgeIdxToStartNavVertexIdx(halfEdgeIdx);
		const NavVertexIdx endIdx = floorBlob->NavHalfEdgeIdxToEndNavVertexIdx(halfEdgeIdx);

		const KyFloat32 startAltitude = navVerticesAltitude[startIdx];
		const KyFloat32 endAltitude = navVerticesAltitude[endIdx];

		const CoordPos64 startCoordPos(cellOrigin + navVertices[startIdx].GetCoordPos64());
		const CoordPos64 endCoordPos(cellOrigin + navVertices[endIdx].GetCoordPos64());

		KyFloat32 altitudeDiffFromStart = m_inputPos3f.z - startAltitude;
		KyFloat32 altitudeDiffFromEnd   = m_inputPos3f.z - endAltitude;

		if (altitudeDiffFromStart * altitudeDiffFromEnd > 0.f)
		{
			// the start and the end of the are on the same "side" : both are above or both are below

			// isTooFarAbove == 1.f if rangeAboveNavMesh < altitudeDiff
			// (false if below the navMesh because rangeAboveNavMesh <= 0 <= rangeAboveNavMesh);
			KyFloat32 isStartTooFarAbove = Fsel(rangeAboveNavMesh - altitudeDiffFromStart, 0.f, 1.f);
			KyFloat32 isEndTooFarAbove   = Fsel(rangeAboveNavMesh - altitudeDiffFromEnd,   0.f, 1.f);

			// isTooFarBelow == 1.f if -rangeBelowNavMesh > altitudeDiff
			// (true if above the navMesh because altitudeDiff >= 0 >= -rangeBelowNavMesh;
			KyFloat32 isStartTooFarBelow = Fsel(altitudeDiffFromStart + rangeBelowNavMesh, 0.f, 1.f);
			KyFloat32 isEndTooFarBelow   = Fsel(altitudeDiffFromEnd   + rangeBelowNavMesh, 0.f, 1.f);

			// discard edge if start and end altitude are both too high or too low
			if (isStartTooFarAbove * isEndTooFarAbove > 0.f || isStartTooFarBelow * isEndTooFarBelow > 0.f)
				continue;
		}


		if(Intersections::SegmentVsAABB2d(startCoordPos, endCoordPos, aABbox) == false)
			continue;

		ClosestPoint::OnSegmentVsPoint2d(startCoordPos, endCoordPos, inputCoordPos, closestPointOnSegment, squareDist2D);

		const Vec2f start(startCoordPos.x * integerPrecision, startCoordPos.y * integerPrecision);
		const Vec2f end(endCoordPos.x * integerPrecision, endCoordPos.y * integerPrecision);
		const Vec2f closestPoint(closestPointOnSegment.x * integerPrecision, closestPointOnSegment.y * integerPrecision);

		const KyFloat32 closestPointAltitude = startAltitude + (endAltitude - startAltitude) * (closestPoint - start).GetLength() / (end - start).GetLength();
		const Vec3f closestPointOnSegmentPos3f(closestPoint.x, closestPoint.y, closestPointAltitude);

		if (aABbox.IsInside(closestPointOnSegment) == false)
			continue;

		KyFloat32 altitudeDiff = m_inputPos3f.z - closestPointAltitude;

		// isNotTooFarAbove == 1.f if rangeAboveNavMesh >= altitudeDiff
		// (true if below the navMesh because altitudeDiff <= 0 <= rangeAboveNavMesh);
		KyFloat32 isNotTooFarAbove = Fsel(rangeAboveNavMesh - altitudeDiff, 1.f, 0.f);

		// isNotTooFarBelow == 1.f if -rangeBelowNavMesh >= altitudeDiff
		// (true if above the navMesh because altitudeDiff >= 0 >= -rangeBelowNavMesh);
		KyFloat32 isNotTooFarBelow = Fsel(rangeBelowNavMesh + altitudeDiff, 1.f, 0.f);

		if (isNotTooFarAbove * isNotTooFarBelow > 0.f)
		{
			const KyFloat32 squareDist3D = (closestPointOnSegmentPos3f - m_inputPos3f).GetSquareLength();

			if (squareDist3D < m_squareDistFromHalfEdge)
			{
				m_squareDistFromHalfEdge = squareDist3D;
				nearestHalfEdgeRawPtr = currentNavHalfEdgeRawPtr;
				m_nearestPosOnHalfEdge = closestPointOnSegmentPos3f;
				databaseGenMetrics.GetWorldIntegerPosFromCoordPos64(closestPointOnSegment, m_nearestPointOnHalfEdgeIntegerPos);
			}
		}
	}
}


}
