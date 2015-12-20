/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{

KY_INLINE PathValidityInterval::PathValidityInterval() : m_pathValidityStatus(PathValidityStatus_NoPath)
{
	m_queryDynamicOutput = QueryDynamicOutput::Create(QueryDynamicOutput::Config());
}
KY_INLINE PathValidityInterval::~PathValidityInterval() 
{
	Clear(); 
}

KY_INLINE PathValidityStatus PathValidityInterval::GetPathValidityStatus() const { return m_pathValidityStatus; }

KY_INLINE void PathValidityInterval::SetPathValidityStatus(PathValidityStatus status) { m_pathValidityStatus = status; }

template<class TLogic>
inline KyResult PathValidityInterval::ReStartIntervalComputation(Path* path, void* traverseLogicUserData, const PositionOnPath& positionOnPath,
	KyFloat32 minDistanceFromTargetOnPathBackward, KyFloat32 minDistanceFromTargetOnPathForward)
{
	KY_ASSERT(path != KY_NULL && path->GetNodeCount() > 1);
	KY_ASSERT(positionOnPath.GetPath() == path);

	ClearBoundsAndEventList();

	// first we check if the positionOnPath is still valid
	m_upperBound = positionOnPath;
	m_lowerBound = positionOnPath;
	m_needToCheckLastEventForRemoval = false;

	const KyUInt32 pathEdgeIdx = positionOnPath.GetPathEdgeIndex();
	PathEdgeType edgeType = path->GetPathEdgeType(pathEdgeIdx);
	switch (edgeType)
	{
	case PathEdgeType_OnNavMesh :
		{
			// retrieve the triangle of currentTargetOnPath
			// NB up to now, we cannot retrieve a graph edge from this query
			TriangleFromPosQuery triangleFromPosQuery;
			triangleFromPosQuery.BindToDatabase(positionOnPath.GetPath()->m_database);
			triangleFromPosQuery.Initialize(positionOnPath.GetPosition());
			triangleFromPosQuery.SetPerformQueryStat(QueryStat_PathValidityInterval);
			triangleFromPosQuery.PerformQuery();
			if (triangleFromPosQuery.GetResult() != TRIANGLEFROMPOS_DONE_TRIANGLE_FOUND)
			{
				SetPathValidityStatus(Dirty_ProgressInvalid);
				ClearBoundsAndEventList();
				return KY_ERROR;
			}

			const NavTriangleRawPtr triangleRawPtr = triangleFromPosQuery.GetResultTrianglePtr().GetRawPtr();
			if (triangleRawPtr.CanBeTraversed<TLogic>(traverseLogicUserData) == false)
			{
				SetPathValidityStatus(Dirty_ProgressInvalid);
				ClearBoundsAndEventList();
				return KY_ERROR;
			}

			m_lowerTrianglePtr = triangleFromPosQuery.GetResultTrianglePtr();
			m_upperTrianglePtr = triangleFromPosQuery.GetResultTrianglePtr();
			break;
		}

	case PathEdgeType_OutsideAlongGraphEdge :
	case PathEdgeType_OnNavGraph :
		{
			NavGraphEdgePtr& navGraphEdgePtr = path->GetEdgeNavGraphEdgePtr(pathEdgeIdx);
			if (navGraphEdgePtr.IsValid() == false)
			{
				SetPathValidityStatus(Dirty_ProgressInvalid);
				ClearBoundsAndEventList();
				return KY_ERROR;
			}

			NavGraphEdgeRawPtr edgeRawPtr = navGraphEdgePtr.GetRawPtr();
			if (edgeRawPtr.CanBeTraversed<TLogic>(traverseLogicUserData) == false)
			{
				SetPathValidityStatus(Dirty_ProgressInvalid);
				ClearBoundsAndEventList();
				return KY_ERROR;
			}

			m_lowerTrianglePtr.Invalidate();
			m_upperTrianglePtr.Invalidate();

			break;
		}
	case PathEdgeType_FromOutsideToNavMesh:
	case PathEdgeType_FromNavMeshToOutside:
	case PathEdgeType_FromOutsideToNavGraph:
	case PathEdgeType_FromNavGraphToOutside:
		{
			// Nothing to do when we are on such an edge since the edge is not supported by any structure of the NavData
			// arrival or destination triangle/NavGraphVertex validity will be tested during the a validation process
			break;
		}
	default :
		{
			KY_LOG_ERROR(("Position on path need to be somewhere on the path"));
			break;
		}
	}

	SetPathValidityStatus(ValidityIsBeingChecked);

	// ok, we perform the first step of path validation step
	// note that the scouting backWard is perform here once and for all as we do not need to go back to the start.

	KyResult validityBackwardResult = KY_SUCCESS;
	KyResult validityForwardResult = KY_SUCCESS;

	// just check if currentTarget is not the GetPath() Start node
	if (m_lowerBound.IsAtFirstNodeOfPath() == false)
	{
		validityBackwardResult = ValidateBackWard<TLogic>(path, traverseLogicUserData, minDistanceFromTargetOnPathBackward);

		if (m_upperBound != m_lowerBound)
		{
			if (KY_FAILED(BuildEventListFromLowerBoundToUpperBound(path)))
			{
				// The build of the event list failed
				// we consider the validity interval to be invalid before target because we cannot build the event List
				SetPathValidityStatus(Dirty_InvalidBeforeProgress_ProcessingAfter);
				m_pathEventList.Clear();
				m_lowerBound = m_upperBound;
				m_lowerTrianglePtr = m_upperTrianglePtr;
				m_needToCheckLastEventForRemoval = false;
			}
			else
			{
				ValidateNavTagTransitionsBackward<TLogic>(traverseLogicUserData, typename TLogic::CanEnterNavTagMode());

				if (m_pathEventList.GetPathEventCount() > 1)
				{
					// make the pathEventList in wanted configuration for the call to ValidateForward
					SetPathEventStatusForLastEvent(PathEventStatus_TemporaryUpperBound);
				}
			}
		}
	}

	// just check if currentTarget is not the GetPath() End node
	if (m_upperBound.IsAtLastNodeOfPath() == false)
	{
		if (m_pathEventList.m_pathEventIntervalArray.GetCount() == 0)
		{
			// No pathEvent has been found, create 1 at positionOnPath
			PathEventInterval& firstEventOnPath = m_pathEventList.m_pathEventIntervalArray.PushDefault();
			firstEventOnPath.m_endingEventOnPath.m_positionOnPath = m_upperBound;
			firstEventOnPath.m_endingEventOnPath.m_eventStatusInList = PathEventStatus_LowerBound;
			PathEventType eventType = GetEventTypeFromLowerBound(path, m_upperBound);
			firstEventOnPath.m_endingEventOnPath.m_eventType = eventType;

			if (GetPathValidityStatus() != Dirty_InvalidBeforeProgress_ProcessingAfter && eventType == PathEventType_OnNavMeshAfterCollision)
				firstEventOnPath.m_endingEventOnPath.m_eventType = PathEventType_OnNavMeshPathNode;
		}

		FirstNewIntervalExtendedStatus unused = FirstNewIntervalIsNotAnExtension;
		validityForwardResult = ValidateForward<TLogic>(path, traverseLogicUserData, minDistanceFromTargetOnPathForward, &unused);
	}
	else
	{
		if (GetPathValidityStatus() == Dirty_InvalidBeforeProgress_ProcessingAfter)
			SetPathValidityStatus(Dirty_InvalidBeforeProgress_ValidAfter);
		else
			SetPathValidityStatus(PathIsValid);

		if (m_pathEventList.GetPathEventCount() > 1)
			SetPathEventStatusForLastEvent(PathEventStatus_UpperBound);
	}

	// Deal with failure cases. It should not happen if the building stage run correctly but errors may occurs if the path is too close from wall for instance
	if (m_pathEventList.GetPathEventCount() <= 1)
	{
		SetPathValidityStatus(Dirty_ProgressInvalid);
		ClearBoundsAndEventList();
		return KY_ERROR;
	}

	return validityBackwardResult == KY_SUCCESS && validityForwardResult == KY_SUCCESS ? KY_SUCCESS : KY_ERROR;
}

template<class TLogic>
inline KyResult PathValidityInterval::ValidateBackWard(Path* path, void* traverseLogicUserData, KyFloat32 minimumdrawBackDistance)
{
	KY_ASSERT(m_lowerBound.GetPath() != KY_NULL);
	KY_ASSERT(m_lowerBound.GetPath() == path);
	KY_ASSERT(m_lowerBound.GetPath()->GetNodeCount() > 1);
	KY_ASSERT(m_lowerBound.IsAtFirstNodeOfPath() == false);

	KyFloat32 remainingDistance = minimumdrawBackDistance;

	for(;;)
	{
		const Vec3f previousPositon = m_lowerBound.GetPosition(); // Don't take reference here, we want to get its displacement distance

		if(KY_FAILED(TestCanGoToPrevPathNode<TLogic>(path, traverseLogicUserData)))
		{
			FindLastValidPositionBackward<TLogic>(path, traverseLogicUserData);
			return KY_ERROR;
		}

		remainingDistance -= Distance(previousPositon, m_lowerBound.GetPosition());

		if (m_lowerBound.IsAtFirstNodeOfPath())
			// we reach the start of the Path, we stop
			return KY_SUCCESS;

		if (remainingDistance < 0.f)
			// we move enough
			return KY_SUCCESS;
	}
}

template<class TLogic>
inline KyResult PathValidityInterval::ValidateForward(Path* path, void* traverseLogicUserData, KyFloat32 minimumDistanceAdvancement, FirstNewIntervalExtendedStatus* firstNewInterval)
{
	KY_ASSERT(m_upperBound.GetPath() != KY_NULL);
	KY_ASSERT(m_upperBound.GetPath() == path);
	KY_ASSERT(m_upperBound.IsAtLastNodeOfPath() == false);

	// Fix the status of the last event
	{
		PathEventInterval& lastInterval = m_pathEventList.m_pathEventIntervalArray.Back();
		PathEvent& lastEvent = lastInterval.m_endingEventOnPath;
		KY_LOG_ERROR_IF(lastEvent.m_eventStatusInList != PathEventStatus_TemporaryUpperBound && lastEvent.m_eventStatusInList != PathEventStatus_LowerBound, ("error of event status"));
		if (lastEvent.m_eventStatusInList == PathEventStatus_TemporaryUpperBound)
			lastEvent.m_eventStatusInList = PathEventStatus_WithinBounds;
	}

	KyFloat32 remainingDistance = minimumDistanceAdvancement;

	for(;;)
	{
		const Vec3f previousPosition = m_upperBound.GetPosition(); // Don't take reference here, we want to get its displacement distance
		const KyUInt32 lastNodeIdx = m_pathEventList.GetPathEventCount() - 1;
		KyResult CanGoResult = TestCanGoToNextPathNode<TLogic>(path, traverseLogicUserData, firstNewInterval);

		if (CanGoResult != KY_SUCCESS)
			FindLastValidPositionForward<TLogic>(path, traverseLogicUserData, firstNewInterval);

		if(KY_FAILED(ValidateNavTagTransitionForward<TLogic>(traverseLogicUserData, lastNodeIdx, typename TLogic::CanEnterNavTagMode())) || KY_FAILED(CanGoResult))
		{
			// We are done with the validation
			if (m_pathEventList.GetPathEventCount() != 1)
				SetPathEventStatusForLastEvent(PathEventStatus_UpperBound);

			return KY_ERROR;
		}

		remainingDistance -= Distance(previousPosition, m_upperBound.GetPosition());

		if (m_upperBound.IsAtLastNodeOfPath())
		{
			// we reach the end of the Path, we stop

			if (GetPathValidityStatus() == Dirty_InvalidBeforeProgress_ProcessingAfter)
				SetPathValidityStatus(Dirty_InvalidBeforeProgress_ValidAfter);
			else
				SetPathValidityStatus(PathIsValid);

			// We are done with the validation
			SetPathEventStatusForLastEvent(PathEventStatus_UpperBound);
			return KY_SUCCESS;
		}

		if (remainingDistance <= 0.0f) // we move enough
		{
			// We are done for this validity stage, we did not reached then end of path yet, we tag the upperBound as temporary
			SetPathEventStatusForLastEvent(PathEventStatus_TemporaryUpperBound);
			return KY_SUCCESS;
		}
	}
}

template<class TLogic>
inline KyResult PathValidityInterval::TestCanGoToPrevPathNode(Path* path, void* traverseLogicUserData)
{
	KY_ASSERT(m_lowerBound.GetPath() != KY_NULL);
	KY_ASSERT(m_lowerBound.GetPath() == path);
	KY_ASSERT(m_lowerBound.GetPath()->GetNodeCount() > 1);
	KY_ASSERT(m_lowerBound.IsAtFirstNodeOfPath() == false);

	const KyUInt32 pathEdgeIdxToMoveOn = m_lowerBound.GetEdgeIdxToMoveOnBackward();

	const Vec3f& edgeStart = path->GetPathEdgeStartPosition(pathEdgeIdxToMoveOn);
	const Vec3f& edgeEnd = path->GetPathEdgeEndPosition(pathEdgeIdxToMoveOn);

	PathEdgeType edgeType = path->GetPathEdgeType(pathEdgeIdxToMoveOn);

	switch (edgeType)
	{
	case PathEdgeType_OnNavMesh:
		{
			RayCanGoOnSegmentQuery<TLogic> rayCanGoOnSegmentQuery;
			rayCanGoOnSegmentQuery.BindToDatabase(path->m_database);
			rayCanGoOnSegmentQuery.SetTraverseLogicUserData(traverseLogicUserData);
			rayCanGoOnSegmentQuery.Initialize(m_lowerBound.GetPosition(), m_lowerTrianglePtr, edgeEnd, edgeStart);
			rayCanGoOnSegmentQuery.SetDynamicOutputMode(QUERY_SAVE_NAVTAGSUBSEGMENTS);
			rayCanGoOnSegmentQuery.SetPerformQueryStat(QueryStat_PathValidityInterval);
			rayCanGoOnSegmentQuery.PerformQuery();

			if (rayCanGoOnSegmentQuery.GetResult() != RAYCANGOONSEGMENT_DONE_SUCCESS)
				return KY_ERROR;

			m_lowerTrianglePtr = rayCanGoOnSegmentQuery.GetArrivalTrianglePtr();
			break;
		}
	case PathEdgeType_OnNavGraph:
		{
			const NavGraphEdgePtr& graphEdgePtr = path->GetEdgeNavGraphEdgePtr(pathEdgeIdxToMoveOn);
			if (graphEdgePtr.IsValid() == false)
				return KY_ERROR;

			NavGraphEdgeRawPtr edgeRawPtr = graphEdgePtr.GetRawPtr();
			if (edgeRawPtr.CanBeTraversed<TLogic>(traverseLogicUserData) == false)
				return KY_ERROR;

			const NavGraphVertexRawPtr startGraphVertex = graphEdgePtr.GetRawPtr().GetStartNavGraphVertexRawPtr();
			const GraphVertexData& vertexData = startGraphVertex.GetGraphVertexData();

			if (vertexData.m_navGraphLink != KY_NULL && vertexData.m_navGraphLink->IsValid())
				m_lowerTrianglePtr = NavTrianglePtr(vertexData.m_navGraphLink->m_navTriangleRawPtr);
			else
				m_lowerTrianglePtr.Invalidate();

			break;
		}
	case PathEdgeType_FromNavGraphToOutside:
		{
			const NavGraphVertexPtr& graphVertexPtr = path->GetNodeNavGraphVertexPtr(pathEdgeIdxToMoveOn);
			if (graphVertexPtr.IsValid() == false)
				return KY_ERROR;

			const NavGraphVertexRawPtr vertexRawPtr = graphVertexPtr.GetRawPtr();
			const GraphVertexData& vertexData = vertexRawPtr.GetGraphVertexData();

			if (vertexData.m_navGraphLink != KY_NULL && vertexData.m_navGraphLink->IsValid())
				m_lowerTrianglePtr = NavTrianglePtr(vertexData.m_navGraphLink->m_navTriangleRawPtr);
			else
				m_lowerTrianglePtr.Invalidate();

			break;
		}
	case PathEdgeType_OutsideAlongGraphEdge:
		{
			const NavGraphEdgePtr& graphEdgePtr = path->GetEdgeNavGraphEdgePtr(pathEdgeIdxToMoveOn);
			if (graphEdgePtr.IsValid() == false)
				return KY_ERROR;

			NavGraphEdgeRawPtr edgeRawPtr = graphEdgePtr.GetRawPtr();
			if (edgeRawPtr.CanBeTraversed<TLogic>(traverseLogicUserData) == false)
				return KY_ERROR;

			m_lowerTrianglePtr.Invalidate();
			break;
		}
	case PathEdgeType_FromOutsideToNavGraph:
	case PathEdgeType_FromOutsideToNavMesh:
	case PathEdgeType_FromNavMeshToOutside:
		{
			m_lowerTrianglePtr.Invalidate();
			break;
		}
	default :
		{
			KY_LOG_ERROR(("Position on path need to be somewhere on the path"));
			break;
		}
	}

	m_lowerBound.MoveBackwardToPrevPathNode_Unsafe();
	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult PathValidityInterval::TestCanGoToNextPathNode(Path* path, void* traverseLogicUserData, FirstNewIntervalExtendedStatus* firstNewInterval)
{
	KY_ASSERT(m_upperBound.GetPath() != KY_NULL);
	KY_ASSERT(m_upperBound.GetPath() == path);
	KY_ASSERT(m_upperBound.GetPath()->GetNodeCount() > 1);
	KY_ASSERT(m_upperBound.IsAtLastNodeOfPath() == false);

	const KyUInt32 pathEdgeIdxToMoveOn = m_upperBound.GetEdgeIdxToMoveOnForward();

	const Vec3f& edgeStart = path->GetPathEdgeStartPosition(pathEdgeIdxToMoveOn);
	const Vec3f& edgeEnd = path->GetPathEdgeEndPosition(pathEdgeIdxToMoveOn);

	PathEdgeType edgeType = path->GetPathEdgeType(pathEdgeIdxToMoveOn);

	switch (edgeType)
	{
	case PathEdgeType_OnNavMesh:
		{
			RayCanGoOnSegmentQuery<TLogic> rayCanGoOnSegmentQuery;
			rayCanGoOnSegmentQuery.BindToDatabase(path->m_database);
			rayCanGoOnSegmentQuery.SetTraverseLogicUserData(traverseLogicUserData);
			rayCanGoOnSegmentQuery.SetDynamicOutputMode(QUERY_SAVE_NAVTAGSUBSEGMENTS);

			rayCanGoOnSegmentQuery.Initialize(m_upperBound.GetPosition(), m_upperTrianglePtr, edgeStart, edgeEnd);
			rayCanGoOnSegmentQuery.SetQueryDynamicOutput(m_queryDynamicOutput);
			rayCanGoOnSegmentQuery.SetPerformQueryStat(QueryStat_PathValidityInterval);
			rayCanGoOnSegmentQuery.PerformQuery();

			if (rayCanGoOnSegmentQuery.GetResult() != RAYCANGOONSEGMENT_DONE_SUCCESS)
				return KY_ERROR;

			// there is at least one stag subSegment
			m_queryDynamicOutput = rayCanGoOnSegmentQuery.GetQueryDynamicOutput();
			KY_LOG_ERROR_IF(m_queryDynamicOutput->GetNavTagSubSegment(m_queryDynamicOutput->GetNavTagSubSegmentCount() - 1).m_exitPos3f.Get2d() != edgeEnd.Get2d(), ("This should not happen"));

			CreateIntervalForNavTagSubSegment(path, pathEdgeIdxToMoveOn, firstNewInterval);
			m_upperTrianglePtr = rayCanGoOnSegmentQuery.GetArrivalTrianglePtr();
			break;
		}
	case PathEdgeType_OutsideAlongGraphEdge:
	case PathEdgeType_OnNavGraph:
		{
			m_needToCheckLastEventForRemoval = false;
			const NavGraphEdgePtr& graphEdgePtr = path->GetEdgeNavGraphEdgePtr(pathEdgeIdxToMoveOn);

			if (graphEdgePtr.IsValid() == false)
				return KY_ERROR;

			NavGraphEdgeRawPtr edgeRawPtr = graphEdgePtr.GetRawPtr();
			if (edgeRawPtr.CanBeTraversed<TLogic>(traverseLogicUserData) == false)
				return KY_ERROR;

			if (edgeType == PathEdgeType_OnNavGraph)
				CreateIntervalForGraphEdge(path, pathEdgeIdxToMoveOn);
			else
				CreateIntervalForOutsideAlongGraphEdge(path, pathEdgeIdxToMoveOn);

			const NavGraphVertexRawPtr endGraphVertex = graphEdgePtr.GetRawPtr().GetEndNavGraphVertexRawPtr();
			const GraphVertexData& vertexData = endGraphVertex.GetGraphVertexData();

			if (vertexData.m_navGraphLink != KY_NULL && vertexData.m_navGraphLink->IsValid())
				m_upperTrianglePtr = NavTrianglePtr(vertexData.m_navGraphLink->m_navTriangleRawPtr);
			else
				m_upperTrianglePtr.Invalidate();
			break;
		}
	case PathEdgeType_FromOutsideToNavGraph:
		{
			m_needToCheckLastEventForRemoval = false;
			const NavGraphVertexPtr& graphVertexPtr = path->GetNodeNavGraphVertexPtr(pathEdgeIdxToMoveOn+1);
			if (graphVertexPtr.IsValid() == false)
				return KY_ERROR;

			CreateIntervalForGraphEdge(path, pathEdgeIdxToMoveOn);
			m_upperTrianglePtr = path->GetNodeNavTrianglePtr(pathEdgeIdxToMoveOn + 1);
			break;
		}
	case PathEdgeType_FromOutsideToNavMesh:
		{
			m_needToCheckLastEventForRemoval = false;
			CreateIntervalForFromOutsidePathEdge(path, pathEdgeIdxToMoveOn);
			m_upperTrianglePtr = path->GetNodeNavTrianglePtr(pathEdgeIdxToMoveOn + 1);
			break;
		}
	case PathEdgeType_FromNavGraphToOutside:
	case PathEdgeType_FromNavMeshToOutside:
		{
			m_needToCheckLastEventForRemoval = false;
			CreateIntervalForToOutsidePathEdge(path, pathEdgeIdxToMoveOn);
			m_upperTrianglePtr.Invalidate();
			break;
		}

	default:
		KY_LOG_ERROR(("all edges on the path must have a defined type"));
		break;
	}

	m_upperBound.MoveForwardToNextPathNode_Unsafe();
	return KY_SUCCESS;
}


template<class TLogic>
inline void PathValidityInterval::FindLastValidPositionForward(Path* path, void* traverseLogicUserData, FirstNewIntervalExtendedStatus* firstNewInterval)
{
	KY_ASSERT(m_upperBound.GetPath() != KY_NULL);
	KY_ASSERT(m_upperBound.GetPath() == path);
	KY_ASSERT(m_upperBound.IsAtLastNodeOfPath() == false);

	if (GetPathValidityStatus() == Dirty_InvalidBeforeProgress_ProcessingAfter)
		SetPathValidityStatus(Dirty_InvalidBeforeAndAfterProgress);
	else
		SetPathValidityStatus(Dirty_ValidBeforeProgress_InvalidAfter);

	const KyUInt32 pathEdgeIdxToMoveOn = m_upperBound.GetEdgeIdxToMoveOnForward();

	if (path->GetPathEdgeType(pathEdgeIdxToMoveOn) != PathEdgeType_OnNavMesh)
		return;

	if (m_upperTrianglePtr.IsValid() == false)
		return;

	const Vec3f& edgeStart = path->GetPathEdgeStartPosition(pathEdgeIdxToMoveOn);
	const Vec3f& edgeEnd = path->GetPathEdgeEndPosition(pathEdgeIdxToMoveOn);
	const KyFloat32 castDistance = Distance2d(m_upperBound.GetPosition(), edgeEnd);

	RayCastOnSegmentQuery<TLogic> rayCastOnSegmentQuery;
	rayCastOnSegmentQuery.BindToDatabase(path->m_database);
	rayCastOnSegmentQuery.SetTraverseLogicUserData(traverseLogicUserData);
	rayCastOnSegmentQuery.SetDynamicOutputMode(QUERY_SAVE_NAVTAGSUBSEGMENTS);

	rayCastOnSegmentQuery.Initialize(m_upperBound.GetPosition(), m_upperTrianglePtr, edgeStart, edgeEnd, castDistance);
	rayCastOnSegmentQuery.SetQueryDynamicOutput(m_queryDynamicOutput);
	rayCastOnSegmentQuery.SetPerformQueryStat(QueryStat_PathValidityInterval);
	rayCastOnSegmentQuery.PerformQuery();

	if ((rayCastOnSegmentQuery.GetResult() == RAYCASTONSEGMENT_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED) || 
		(rayCastOnSegmentQuery.GetResult() == RAYCASTONSEGMENT_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION))
	{
		m_queryDynamicOutput = rayCastOnSegmentQuery.GetQueryDynamicOutput();
		CreateIntervalsForNavTagSubsegmentButLast(path, pathEdgeIdxToMoveOn, firstNewInterval);
		CreateIntervalForLastNavTagSubSegment(path, pathEdgeIdxToMoveOn, PathEventType_OnNavMeshAfterCollision, firstNewInterval);

		m_upperBound.InitOnPathEdge(path, rayCastOnSegmentQuery.GetArrivalPos(), pathEdgeIdxToMoveOn);
		m_upperTrianglePtr = rayCastOnSegmentQuery.GetArrivalTrianglePtr();

		PathEventInterval& lastInterval = m_pathEventList.m_pathEventIntervalArray.Back();
		PathEvent& lastEvent = lastInterval.m_endingEventOnPath;
		lastEvent.m_positionOnPath = m_upperBound;
		m_needToCheckLastEventForRemoval = false;
	}
}

template<class TLogic>
inline void PathValidityInterval::FindLastValidPositionBackward(Path* path, void* traverseLogicUserData)
{
	KY_ASSERT(m_lowerBound.GetPath() != KY_NULL);
	KY_ASSERT(m_lowerBound.GetPath() == path);
	KY_ASSERT(m_lowerBound.IsAtFirstNodeOfPath() == false);

	SetPathValidityStatus(Dirty_InvalidBeforeProgress_ProcessingAfter);

	const KyUInt32 pathEdgeIdxToMoveOn = m_lowerBound.GetEdgeIdxToMoveOnBackward();

	if (path->GetPathEdgeType(pathEdgeIdxToMoveOn) != PathEdgeType_OnNavMesh)
		return;

	if (m_lowerTrianglePtr.IsValid() == false)
		return;

	const Vec3f& edgeStart = path->GetPathEdgeStartPosition(pathEdgeIdxToMoveOn);
	const Vec3f& edgeEnd = path->GetPathEdgeEndPosition(pathEdgeIdxToMoveOn);
	const KyFloat32 castDistance = Distance2d(m_lowerBound.GetPosition(), edgeStart);

	RayCastOnSegmentQuery<TLogic> rayCastOnSegmentQuery;
	rayCastOnSegmentQuery.BindToDatabase(path->m_database);
	rayCastOnSegmentQuery.SetTraverseLogicUserData(traverseLogicUserData);
	rayCastOnSegmentQuery.Initialize(m_lowerBound.GetPosition(), m_lowerTrianglePtr, edgeEnd, edgeStart, castDistance);
	rayCastOnSegmentQuery.SetPerformQueryStat(QueryStat_PathValidityInterval);
	rayCastOnSegmentQuery.PerformQuery();

	if ((rayCastOnSegmentQuery.GetResult() == RAYCASTONSEGMENT_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED) || 
		(rayCastOnSegmentQuery.GetResult() == RAYCASTONSEGMENT_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION))
	{
		m_lowerBound.InitOnPathEdge(path, rayCastOnSegmentQuery.GetArrivalPos(), pathEdgeIdxToMoveOn);
		m_lowerTrianglePtr = rayCastOnSegmentQuery.GetArrivalTrianglePtr();
	}
}


template<class TLogic>
KY_INLINE void PathValidityInterval::ValidateNavTagTransitionsBackward(void* /*traverseLogicUserData*/, const LogicDoNotUseCanEnterNavTag& /*navTagType*/)
{
	return;
}
template<class TLogic>
KY_INLINE KyResult PathValidityInterval::ValidateNavTagTransitionForward(void* /*traverseLogicUserData*/,
	KyUInt32 /*firstNewEvent*/, const LogicDoNotUseCanEnterNavTag& /*navTagType*/)
{
	return KY_SUCCESS;
}

template<class TLogic>
inline void PathValidityInterval::ValidateNavTagTransitionsBackward(void* traverseLogicUserData, const LogicDoUseCanEnterNavTag& /*navTagType*/)
{
	// We do not look for the first and the last PathEvent since we are sure they are not of type NavTagSwitch
	for(KyUInt32 i = m_pathEventList.GetPathEventCount() - 1; i > 1; --i)
	{
		const KyUInt32 pathEventIdx = i-1;
		const PathEvent& pathEvent = m_pathEventList.GetPathEvent(pathEventIdx);
		if (pathEvent.m_eventType != PathEventType_NavTagSwitch)
			continue;

		const NavTag* exitNavTag = m_pathEventList.GetNavTagOnIntervalBeforeEvent(pathEventIdx);
		const NavTag* enterNavTag = m_pathEventList.GetNavTagOnIntervalAfterEvent(pathEventIdx);
		KY_ASSERT(exitNavTag != KY_NULL);
		KY_ASSERT(enterNavTag != KY_NULL);

		const Vec3f& positionOnPath = pathEvent.m_positionOnPath.GetPosition();
		if (TLogic::CanEnterNavTag(traverseLogicUserData, *exitNavTag, *enterNavTag, positionOnPath) == false)
		{
			m_lowerBound = pathEvent.m_positionOnPath;

			// We remove all the PathEvent of the list that are before the PathEvent of Idx pathEventIdx;
			KyArray<PathEventInterval, MemStat_Bot>& eventArray = m_pathEventList.m_pathEventIntervalArray;
			const KyUInt32 eventCount = eventArray.GetCount();
			KyUInt32 newIdx = 0;
			for(KyUInt32 browingIdx = pathEventIdx; browingIdx < eventCount; ++browingIdx)
			{
				eventArray[newIdx] = eventArray[browingIdx];
				++newIdx;
			}
			KY_ASSERT(newIdx > 0 && newIdx < eventArray.GetCount());

			eventArray.Resize(newIdx);
			eventArray[0].m_navTagOnInterval.Invalidate(); // the first pathEvent does not store any navTag
			eventArray[0].m_endingEventOnPath.m_eventType = PathEventType_OnNavMeshAfterCollision;
			eventArray[0].m_endingEventOnPath.m_eventStatusInList = PathEventStatus_LowerBound;
			SetPathValidityStatus(Dirty_InvalidBeforeProgress_ProcessingAfter);
			return;
		}
	}
}

template<class TLogic>
inline KyResult PathValidityInterval::ValidateNavTagTransitionForward(void* traverseLogicUserData,
	KyUInt32 firstNewEvent, const LogicDoUseCanEnterNavTag& /*navTagType*/)
{
	// We do not look for the last PathEvent since we are sure they are not of type NavTagSwitch
	for(KyUInt32 i = firstNewEvent; i < m_pathEventList.GetPathEventCount(); ++i)
	{
		const KyUInt32 pathEventIdx = i;
		PathEvent& pathEvent = m_pathEventList.GetPathEvent(pathEventIdx);
		if (pathEvent.m_eventType != PathEventType_NavTagSwitch)
			continue;

		const NavTag* exitNavTag = m_pathEventList.GetNavTagOnIntervalBeforeEvent(pathEventIdx);
		const NavTag* enterNavTag = m_pathEventList.GetNavTagOnIntervalAfterEvent(pathEventIdx);
		KY_ASSERT(exitNavTag != KY_NULL);
		KY_ASSERT(enterNavTag != KY_NULL);

		const Vec3f& positionOnPath = pathEvent.m_positionOnPath.GetPosition();
		if (TLogic::CanEnterNavTag(traverseLogicUserData, *exitNavTag, *enterNavTag, positionOnPath) == false)
		{
			KyArray<PathEventInterval, MemStat_Bot>& eventArray = m_pathEventList.m_pathEventIntervalArray;
			eventArray.Resize(i + 1); // pop back the next path events.
			pathEvent.m_eventType = PathEventType_OnNavMeshAfterCollision;
			m_upperBound = pathEvent.m_positionOnPath;

			if (GetPathValidityStatus() == Dirty_InvalidBeforeProgress_ProcessingAfter ||
				GetPathValidityStatus() == Dirty_InvalidBeforeAndAfterProgress)
				SetPathValidityStatus(Dirty_InvalidBeforeAndAfterProgress);
			else
				SetPathValidityStatus(Dirty_ValidBeforeProgress_InvalidAfter);

			return KY_ERROR;
		}
	}

	return KY_SUCCESS;
}


}

