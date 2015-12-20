/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#include "gwnavruntime/navmesh/closestpoint.h"


namespace Kaim
{


KY_INLINE CoordPos64 ThinCapsuleWithExtentOffset::GetOffset() // get the offset by which the capsule must be reduced, cache the result
{
	if (m_offset.x == KyInt64MAXVAL)
	{
		CoordPos64 capsule = m_capsuleEndCoordPos - m_capsuleStartCoordPos;
		KyUInt32 capsuleLength = (KyUInt32)(floorf(sqrtf((KyFloat32)m_capsuleSquareLength)));
		m_offset = (capsule * m_recoveryDistance) / capsuleLength;
	}
	return m_offset;		
}

template<class TLogic>
inline bool QueryUtils::IsHalfEdgeCompatibleWithThinCapsule(const NavHalfEdgeRawPtr& halfEdge, ThinCapsuleWithExtentOffset& capsuleParams)
{
	const CoordPos64 halfEdgeStartCoordPos = halfEdge.GetStartVertexCoordPos64();
	const CoordPos64 halfEdgeEndCoordPos   = halfEdge.GetEndVertexCoordPos64();
	
	// sqDistHalfEdge != 0, no degenerated NavHalfEdge of length 0 expected!
	const KyUInt64 sqDistHalfEdge = (halfEdgeEndCoordPos - halfEdgeStartCoordPos).GetSquareLength();

	// Adjust test capsule to start/end conditions: if start/end are close to the
	// halEdge, we move them forward/backward accordingly to recoveryDistance.
	const KyUInt64 capsuleSquareRadiusBySqDistHalfEdge = capsuleParams.m_capsuleSquareRadius * sqDistHalfEdge;
	CoordPos64 testCapsuleStartCoordPos = capsuleParams.m_capsuleStartCoordPos;
	CoordPos64 testCapsuleEndCoordPos = capsuleParams.m_capsuleEndCoordPos;

	// factorDependentOfChangeInCapsuleStart values:
	// 1 if capsule's start is not changed, so if capsule's end is changed, it checks that the capsule is greater than recovery distance (done using square distances)
	// 4 otherwise, so if capsule's end is changed as well as start, it checks that the capsule is greater than twice the recovery distance (done using square distances)
	KyUInt32 factorDependentOfChangeInCapsuleStart = 1;
 	if (ClosestPoint::GetSquareDistancePointToSegmentFactorSegmentSquareLength(testCapsuleStartCoordPos, halfEdgeStartCoordPos, halfEdgeEndCoordPos, sqDistHalfEdge) < capsuleSquareRadiusBySqDistHalfEdge)
	{
		KyUInt32 sqRecoveryDistance = capsuleParams.m_recoveryDistance * capsuleParams.m_recoveryDistance;
		if (capsuleParams.m_capsuleSquareLength <= sqRecoveryDistance) // check that the caspule length is greater than the recoveryDistance
			return true;  // The segment is too small to actually take margin into account => ignore margin in that case

		testCapsuleStartCoordPos += capsuleParams.GetOffset(); // move start inside the capsule by recoveryDistance
		factorDependentOfChangeInCapsuleStart = 4; // start was changed, if end has to be changed as well, capsule length must be twice recoveryDistance
	}
 	if (ClosestPoint::GetSquareDistancePointToSegmentFactorSegmentSquareLength(testCapsuleEndCoordPos, halfEdgeStartCoordPos, halfEdgeEndCoordPos, sqDistHalfEdge) < capsuleSquareRadiusBySqDistHalfEdge)
	{
		KyUInt32 sqRecoveryDistance = capsuleParams.m_recoveryDistance * capsuleParams.m_recoveryDistance;
		if (capsuleParams.m_capsuleSquareLength <= sqRecoveryDistance*factorDependentOfChangeInCapsuleStart)
			return true;  // The segment is too small to actually take margin into account => ignore margin in that case

		testCapsuleEndCoordPos -= capsuleParams.GetOffset(); // move end inside the capsule by recoveryDistance
	}

	// Check the provided HalfEdge
	KyUInt64 sqDistToSegment = ClosestPoint::GetSquareDistanceSegmentToSegment(
		halfEdgeStartCoordPos, halfEdgeEndCoordPos, testCapsuleStartCoordPos, testCapsuleEndCoordPos);

	if (sqDistToSegment > capsuleParams.m_capsuleSquareRadius)
		return true;

	if (halfEdge.IsHalfEdgeCrossable<TLogic>(GetTraverseLogicUserData()) == false)
		return false;

	// Check neighbor triangles around start vertex
	NavHalfEdgeRawPtr currentHalfEdge;
	GetNextHalfEdgeAroundStartVertex(halfEdge, currentHalfEdge);
	while (currentHalfEdge != halfEdge)
	{
		sqDistToSegment = ClosestPoint::GetSquareDistanceSegmentToSegment(
			currentHalfEdge.GetStartVertexCoordPos64(),
			currentHalfEdge.GetEndVertexCoordPos64(),
			testCapsuleStartCoordPos, testCapsuleEndCoordPos);

		if (sqDistToSegment > capsuleParams.m_capsuleSquareRadius)
			break;

		if (currentHalfEdge.IsHalfEdgeCrossable<TLogic>(GetTraverseLogicUserData()) == false)
			return false;

		GetNextHalfEdgeAroundStartVertex(currentHalfEdge, currentHalfEdge);
	}

	// Check neighbor triangles around end vertex
	GetPrevHalfEdgeAroundDestVertex(halfEdge, currentHalfEdge);
	while (currentHalfEdge != halfEdge)
	{
		sqDistToSegment = ClosestPoint::GetSquareDistanceSegmentToSegment(
			currentHalfEdge.GetStartVertexCoordPos64(),
			currentHalfEdge.GetEndVertexCoordPos64(),
			testCapsuleStartCoordPos, testCapsuleEndCoordPos);

		if (sqDistToSegment > capsuleParams.m_capsuleSquareRadius)
			break;

		if (currentHalfEdge.IsHalfEdgeCrossable<TLogic>(GetTraverseLogicUserData()) == false)
			return false;

		GetPrevHalfEdgeAroundDestVertex(currentHalfEdge, currentHalfEdge);
	}

	return true;
}

KY_INLINE void QueryUtils::GetNextHalfEdgeAroundStartVertex(const NavHalfEdgeRawPtr& currentHalfEdge, NavHalfEdgeRawPtr& nextHalfEdge)
{
	NavHalfEdgeRawPtr currentPair;
	currentHalfEdge.GetPairHalfEdgeRawPtr(currentPair);
	currentPair.GetNextHalfEdgeRawPtr(nextHalfEdge);
}

KY_INLINE void QueryUtils::GetPrevHalfEdgeAroundDestVertex(const NavHalfEdgeRawPtr& currentHalfEdge, NavHalfEdgeRawPtr& prevHalfEdge)
{
	NavHalfEdgeRawPtr currentPair;
	currentHalfEdge.GetPairHalfEdgeRawPtr(currentPair);
	currentPair.GetPrevHalfEdgeRawPtr(prevHalfEdge);
}


template<class TLogic>
inline KyResult QueryUtils::FindValidPositionFromIntegerIntersection(const Vec3f& collisionPos3f,
	const NavHalfEdgePtr& halfEdgePtrHit, WorldIntegerPos& arrivalIntegerPos, NavTrianglePtr& arrivalTrianglePtr)
{
	const DatabaseGenMetrics& genMetrics = m_database->GetDatabaseGenMetrics();

	const NavTrianglePtr lastTrianglePtr(halfEdgePtrHit.m_navFloorPtr, NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(halfEdgePtrHit.GetHalfEdgeIdx()));
	const KyInt32 stepMax = 2;
	KyInt32 currentSign = 1;

	TriangleFromPosAndTriangleSeedQuery triangleFromPosAndTriangleSeedQuery;
	triangleFromPosAndTriangleSeedQuery.BindToDatabase(m_database);
	triangleFromPosAndTriangleSeedQuery.Initialize(lastTrianglePtr, collisionPos3f);


	const NavHalfEdgeIdx currentEdgeIdx = halfEdgePtrHit.GetHalfEdgeIdx();
	const NavHalfEdgeIdx nextEdgeIdx = NavFloorBlob::NavHalfEdgeIdxToNextNavHalfEdgeIdx(halfEdgePtrHit.GetHalfEdgeIdx());
	const NavHalfEdgeIdx prevEdgeIdx = NavFloorBlob::NavHalfEdgeIdxToPrevNavHalfEdgeIdx(halfEdgePtrHit.GetHalfEdgeIdx());

	const NavFloorBlob& lastFloorBlob = *halfEdgePtrHit.GetNavFloorBlob_Unsafe();
	const NavHalfEdge* const navHalfEdges = lastFloorBlob.m_navHalfEdges.GetValues();

	const NavVertexIdx startIdx = navHalfEdges[currentEdgeIdx].GetStartVertexIdx();
	const NavVertexIdx endIdx   = navHalfEdges[nextEdgeIdx].GetStartVertexIdx();
	const NavVertexIdx thirdIdx = navHalfEdges[prevEdgeIdx].GetStartVertexIdx();

	const NavVertex* const navVertices = lastFloorBlob.m_navVertices.GetValues();

	const CoordPos64 lastCellOrigin = genMetrics.ComputeCellOrigin(halfEdgePtrHit.GetNavFloor_Unsafe()->GetCellPos());

	CheckIfCurrentIntegerPositionIsValidParam params;
	params.m_currentArrivalIntegerPos = genMetrics.GetWorldIntegerPosFromVec3f(collisionPos3f);

	params.m_start = lastCellOrigin + navVertices[startIdx].GetCoordPos64(); // 31 bits
	params.m_end   = lastCellOrigin + navVertices[endIdx].GetCoordPos64();   // 31 bits
	params.m_third = lastCellOrigin + navVertices[thirdIdx].GetCoordPos64(); // 31 bits

	params.m_startToEnd   = params.m_end   - params.m_start; // 31 bits
	params.m_endToThird   = params.m_third - params.m_end;   // 31 bits
	params.m_thirdTostart = params.m_start - params.m_third; // 31 bits

	KY_LOG_WARNING_IF((  params.m_startToEnd.x >= 1LL << 31 || -  params.m_startToEnd.x >= 1LL << 31), ("Input value requires too many bits of precision. Potential computation overflow, result may be incoherent"));
	KY_LOG_WARNING_IF((  params.m_endToThird.x >= 1LL << 31 || -  params.m_endToThird.x >= 1LL << 31), ("Input value requires too many bits of precision. Potential computation overflow, result may be incoherent"));
	KY_LOG_WARNING_IF((params.m_thirdTostart.x >= 1LL << 31 || -params.m_thirdTostart.x >= 1LL << 31), ("Input value requires too many bits of precision. Potential computation overflow, result may be incoherent"));
	KY_LOG_WARNING_IF((  params.m_startToEnd.y >= 1LL << 31 || -  params.m_startToEnd.y >= 1LL << 31), ("Input value requires too many bits of precision. Potential computation overflow, result may be incoherent"));
	KY_LOG_WARNING_IF((  params.m_endToThird.y >= 1LL << 31 || -  params.m_endToThird.y >= 1LL << 31), ("Input value requires too many bits of precision. Potential computation overflow, result may be incoherent"));
	KY_LOG_WARNING_IF((params.m_thirdTostart.y >= 1LL << 31 || -params.m_thirdTostart.y >= 1LL << 31), ("Input value requires too many bits of precision. Potential computation overflow, result may be incoherent"));

	if (KY_SUCCEEDED(CheckIfCurrentIntegerPositionIsValid<TLogic>(params, triangleFromPosAndTriangleSeedQuery,
		arrivalIntegerPos, arrivalTrianglePtr)))
		return KY_SUCCESS;

	const KyInt32 cellSizeInCoord = m_database->GetDatabaseGenMetrics().m_cellSizeInCoord;
	// spiral loop
	for (KyInt32 i = 1; i < stepMax + 1; ++i)
	{
		for (KyInt32 j = 1; j <= i; ++j)
		{
			if (currentSign > 0)
				params.m_currentArrivalIntegerPos.Add1OnCoordPosX(cellSizeInCoord);
			else
				params.m_currentArrivalIntegerPos.Remove1OnCoordPosX(cellSizeInCoord);

			if (KY_SUCCEEDED(CheckIfCurrentIntegerPositionIsValid<TLogic>(params, triangleFromPosAndTriangleSeedQuery, arrivalIntegerPos, arrivalTrianglePtr)))
				return KY_SUCCESS;
		}

		for (KyInt32 j = 1; j <= i; ++j)
		{
			if (currentSign > 0)
				params.m_currentArrivalIntegerPos.Add1OnCoordPosY(cellSizeInCoord);
			else
				params.m_currentArrivalIntegerPos.Remove1OnCoordPosY(cellSizeInCoord);

			if (KY_SUCCEEDED(CheckIfCurrentIntegerPositionIsValid<TLogic>(params, triangleFromPosAndTriangleSeedQuery, arrivalIntegerPos, arrivalTrianglePtr)))
				return KY_SUCCESS;
		}

		currentSign = -currentSign;
	}

	for (KyInt32 j = 1; j <= stepMax; ++j)
	{
		params.m_currentArrivalIntegerPos.Add1OnCoordPosX(cellSizeInCoord);

		if (KY_SUCCEEDED(CheckIfCurrentIntegerPositionIsValid<TLogic>(params, triangleFromPosAndTriangleSeedQuery, arrivalIntegerPos, arrivalTrianglePtr)))
			return KY_SUCCESS;
	}

	return KY_ERROR;
}


template<class TLogic>
inline KyResult  QueryUtils::CheckIfCurrentIntegerPositionIsValid(const QueryUtils::CheckIfCurrentIntegerPositionIsValidParam& params,
	TriangleFromPosAndTriangleSeedQuery& triangleFromPosAndTriangleSeedQuery, WorldIntegerPos& arrivalIntegerPos, NavTrianglePtr& arrivalTrianglePtr)
{
	const CoordPos64 collisionPos64 = m_database->GetDatabaseGenMetrics().GetCoordPos64FromWorldIntegerPos(params.m_currentArrivalIntegerPos);

	if (Intersections::IsStrictlyOnTheLeftOfTheEdge2d(collisionPos64, params.m_start, params.m_startToEnd) == false)
		return KY_ERROR;

	triangleFromPosAndTriangleSeedQuery.Initialize(triangleFromPosAndTriangleSeedQuery.GetSeedTrianglePtr(), triangleFromPosAndTriangleSeedQuery.GetInputPos());
	triangleFromPosAndTriangleSeedQuery.SetInputIntegerPos(params.m_currentArrivalIntegerPos);

	if (Intersections::IsOnTheLeftOfTheEdge2d(collisionPos64, params.m_end, params.m_endToThird) && 
		Intersections::IsOnTheLeftOfTheEdge2d(collisionPos64, params.m_third, params.m_thirdTostart))
	{
		arrivalTrianglePtr = triangleFromPosAndTriangleSeedQuery.GetSeedTrianglePtr();
		arrivalIntegerPos = triangleFromPosAndTriangleSeedQuery.GetInputIntegerPos();
		return KY_SUCCESS;
	}

	triangleFromPosAndTriangleSeedQuery.PerformQueryWithInputCoordPos(m_workingMemory);

	if (triangleFromPosAndTriangleSeedQuery.GetResult() == NEARESTTRIANGLEFROMSEED_DONE_TRIANGLE_FOUND)
	{
		const NavTriangleRawPtr resultTriangle = triangleFromPosAndTriangleSeedQuery.GetResultTrianglePtr().GetRawPtr();
		if(resultTriangle.CanBeTraversed<TLogic>(GetTraverseLogicUserData()))
		{
			arrivalTrianglePtr = triangleFromPosAndTriangleSeedQuery.GetResultTrianglePtr();
			arrivalIntegerPos = triangleFromPosAndTriangleSeedQuery.GetInputIntegerPos();
			return KY_SUCCESS;
		}
	}

	return KY_ERROR;
}

}

