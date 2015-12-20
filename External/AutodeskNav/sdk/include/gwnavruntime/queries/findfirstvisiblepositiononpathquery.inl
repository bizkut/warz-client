/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: LAPA - secondary contact: NOBODY


namespace Kaim
{

template<class TLogic>
KY_INLINE FindFirstVisiblePositionOnPathQuery<TLogic>::FindFirstVisiblePositionOnPathQuery()
	: BaseFindFirstVisiblePositionOnPathQuery()
	, m_displayList(KY_NULL)
{}

template<class TLogic>
KY_INLINE void FindFirstVisiblePositionOnPathQuery<TLogic>::BindToDatabase(Database* database)
{
	BaseFindFirstVisiblePositionOnPathQuery::BindToDatabase(database);
}

template<class TLogic>
KY_INLINE void FindFirstVisiblePositionOnPathQuery<TLogic>::Initialize(
	const PositionOnPath& startPositionOnPath, const PositionOnPath& endPositionOnPath, const Vec3f& visibilityStartPoint)
{
	BaseFindFirstVisiblePositionOnPathQuery::Initialize(startPositionOnPath, endPositionOnPath, visibilityStartPoint);
}

template<class TLogic>
KY_INLINE void FindFirstVisiblePositionOnPathQuery<TLogic>::SetVisibilityStartTrianglePtr(const NavTrianglePtr& startTrianglePtr) { BaseFindFirstVisiblePositionOnPathQuery::SetVisibilityStartTrianglePtr(startTrianglePtr); }

template<class TLogic>
KY_INLINE void FindFirstVisiblePositionOnPathQuery<TLogic>::SetSamplingDistance(KyFloat32 samplingDistance)
{
	BaseFindFirstVisiblePositionOnPathQuery::SetSamplingDistance(samplingDistance);
}

template<class TLogic>
KY_INLINE void FindFirstVisiblePositionOnPathQuery<TLogic>::SetRayCanGoMarginMode(RayCanGoMarginMode rayCanGoMarginMode)
{
	BaseFindFirstVisiblePositionOnPathQuery::SetRayCanGoMarginMode(rayCanGoMarginMode);
}

template<class TLogic>
KY_INLINE FindFirstVisiblePositionOnPathQueryResult FindFirstVisiblePositionOnPathQuery<TLogic>::GetResult() const
{
	return BaseFindFirstVisiblePositionOnPathQuery::GetResult();
}

template<class TLogic> KY_INLINE const PositionOnPath& FindFirstVisiblePositionOnPathQuery<TLogic>::GetStartPositionOnPath()        const
{
	return BaseFindFirstVisiblePositionOnPathQuery::GetStartPositionOnPath();
}
template<class TLogic> KY_INLINE const PositionOnPath& FindFirstVisiblePositionOnPathQuery<TLogic>::GetEndPositionOnPath()          const
{
	return BaseFindFirstVisiblePositionOnPathQuery::GetEndPositionOnPath();
}
template<class TLogic> KY_INLINE const Vec3f&          FindFirstVisiblePositionOnPathQuery<TLogic>::GetVisibilityStartPoint()       const
{
	return BaseFindFirstVisiblePositionOnPathQuery::GetVisibilityStartPoint();
}
template<class TLogic> KY_INLINE const NavTrianglePtr& FindFirstVisiblePositionOnPathQuery<TLogic>::GetVisibilityStartTrianglePtr() const
{
	return BaseFindFirstVisiblePositionOnPathQuery::GetVisibilityStartTrianglePtr();
}
template<class TLogic> KY_INLINE KyFloat32             FindFirstVisiblePositionOnPathQuery<TLogic>::GetSamplingDistance()           const
{
	return BaseFindFirstVisiblePositionOnPathQuery::GetSamplingDistance();
}
template<class TLogic> KY_INLINE const PositionOnPath& FindFirstVisiblePositionOnPathQuery<TLogic>::GetFirstVisiblePositionOnPath() const
{
	return BaseFindFirstVisiblePositionOnPathQuery::GetFirstVisiblePositionOnPath();
}

template<class TLogic>
KY_INLINE void FindFirstVisiblePositionOnPathQuery<TLogic>::Advance(WorkingMemory* workingMemory)
{
	PerformQuery(workingMemory);
	m_processStatus = QueryDone;
}

template<class TLogic>
inline void FindFirstVisiblePositionOnPathQuery<TLogic>::PerformQuery(WorkingMemory* workingMemory)
{
	ScopedPerformedQueryCounter(GetStaticType(), m_database, m_performQueryStat);
	if (GetResult() != FINDFIRSTVISIBLEPOSITIONONPATH_NOT_PROCESSED)
		return;

	KY_ASSERT(m_database != KY_NULL);

	if (workingMemory == KY_NULL)
		workingMemory = m_database->GetWorkingMemory();

	// Set default result value
	m_result = FINDFIRSTVISIBLEPOSITIONONPATH_DONE_NO_VISIBLE_POSITION_FOUND;

#ifndef KY_BUILD_SHIPPING
	if (m_displayList)
	{
		const KyFloat32 pointDisplaySize = Min(0.1f, 0.25f * m_samplingDistance);
		Kaim::VisualShapeColor shapeColor;
		shapeColor.m_lineColor = VisualColor::Cyan;
		m_displayList->PushPoint(m_startPositionOnPath.GetPosition(), pointDisplaySize, shapeColor);
		shapeColor.m_lineColor = VisualColor::Magenta;
		m_displayList->PushPoint(m_endPositionOnPath.GetPosition(), pointDisplaySize, shapeColor);
	}
#endif

	// Compute the nearest projection on each path edge to be considered
	const KyUInt32 maxNearestPositionOnPathCount = 1 + m_startPositionOnPath.GetPathEdgeIndex() - m_endPositionOnPath.GetPathEdgeIndex();
	PositionOnPathWorkingMemArray nearestPositionOnPathArray;
	if (KY_FAILED(nearestPositionOnPathArray.BindToDatabase(workingMemory, maxNearestPositionOnPathCount)))
	{
		SetResult(FINDFIRSTVISIBLEPOSITIONONPATH_DONE_LACK_OF_WORKING_MEMORY);
		return;
	}
	PositionOnPath currentPositionOnPath = m_startPositionOnPath;
	while (m_endPositionOnPath.IsStrictlyBefore(currentPositionOnPath))
	{
		PositionOnPath previousPositionOnPath = currentPositionOnPath;
		previousPositionOnPath.MoveBackwardToPrevPathNode_Unsafe();
		const Vec3f& prevPathNodePos = previousPositionOnPath.GetPosition();
		const Vec3f edgeVector = currentPositionOnPath.GetPosition() - prevPathNodePos;
		const KyFloat32 dotProd3d = DotProduct(edgeVector, m_visibilityStartPos3f - prevPathNodePos);
		if ((dotProd3d > 0.0f) && (dotProd3d < edgeVector.GetSquareLength()))
		{
			PositionOnPath nearestPositionOnPath = previousPositionOnPath;
			nearestPositionOnPath.MoveForward(dotProd3d / edgeVector.GetLength());
			if (KY_FAILED(nearestPositionOnPathArray.PushBack(nearestPositionOnPath)))
			{
				SetResult(FINDFIRSTVISIBLEPOSITIONONPATH_DONE_LACK_OF_WORKING_MEMORY);
				return;
			}
		}
		currentPositionOnPath = previousPositionOnPath;
	}
	const KyUInt32 nearestPositionOnPathCount = nearestPositionOnPathArray.GetCount();

	// Instantiate a rayCanGoQuery to be used in all subsequent ValidateCandidate calls
	RayCanGoQuery<TLogic> rayCanGoQuery;
	rayCanGoQuery.BindToDatabase(m_database);
	rayCanGoQuery.SetTraverseLogicUserData(GetTraverseLogicUserData());
	rayCanGoQuery.SetMarginMode(m_rayCanGoMarginMode);
	rayCanGoQuery.SetComputeCostMode(QUERY_COMPUTE_COST_ALONG_3DAXIS);

	KyUInt32 nextNearestToCheck = 0;
	m_firstVisiblePositionOnPath = m_startPositionOnPath;
	PositionOnPath nextNearestPOP;
	while (m_endPositionOnPath.IsStrictlyBefore(m_firstVisiblePositionOnPath))
	{
		m_firstVisiblePositionOnPath.MoveBackward_StopAtPathNode_Unsafe(m_samplingDistance);

		for ( ; nextNearestToCheck < nearestPositionOnPathCount; ++nextNearestToCheck)
		{
			nearestPositionOnPathArray.GetPositionOnPath(nextNearestToCheck, nextNearestPOP);
			if (m_firstVisiblePositionOnPath.IsStrictlyBefore(nextNearestPOP))
			{
				if (ValidateCandidate(rayCanGoQuery, nextNearestPOP, workingMemory) == StopSampling)
				{
					m_firstVisiblePositionOnPath = nextNearestPOP;
					return;
				}
			}
			else
				break;
		}

		if (ValidateCandidate(rayCanGoQuery, m_firstVisiblePositionOnPath, workingMemory) == StopSampling)
			return;
	}
}

template<class TLogic>
KY_INLINE BaseFindFirstVisiblePositionOnPathQuery::CandidateValidationResult FindFirstVisiblePositionOnPathQuery<TLogic>::ValidateCandidate(
	RayCanGoQuery<TLogic>& rayCanGoQuery, const PositionOnPath& candidate, WorkingMemory* workingMemory)
{
	rayCanGoQuery.Initialize(m_visibilityStartPos3f, candidate.GetPosition());
	rayCanGoQuery.SetStartTrianglePtr(m_visibilityStartTrianglePtr);
	rayCanGoQuery.SetPerformQueryStat(m_performQueryStat);
	rayCanGoQuery.PerformQuery(workingMemory);

	CandidateValidationResult result = StopSampling;
	Kaim::VisualShapeColor shapeColor;
	shapeColor.m_lineColor = VisualColor::White;

	switch (rayCanGoQuery.GetResult())
	{
	case RAYCANGO_DONE_START_OUTSIDE:
		{
			m_result = FINDFIRSTVISIBLEPOSITIONONPATH_DONE_VISIBILITYSTARTPOINT_OUTSIDE;
			shapeColor.m_lineColor = VisualColor::Magenta;
			result = StopSampling;
			break;
		}

	case RAYCANGO_DONE_START_NAVTAG_FORBIDDEN:
		{
			m_result = FINDFIRSTVISIBLEPOSITIONONPATH_DONE_VISIBILITYSTARTPOINT_NAVTAG_FORBIDDEN;
			shapeColor.m_lineColor = VisualColor::Magenta;
			result = StopSampling;
			break;
		}

	case RAYCANGO_DONE_LACK_OF_WORKING_MEMORY:
		{
			m_result = FINDFIRSTVISIBLEPOSITIONONPATH_DONE_LACK_OF_WORKING_MEMORY;
			shapeColor.m_lineColor = VisualColor::DarkRed;
			result = StopSampling;
			break;
		}

	case RAYCANGO_DONE_UNKNOWN_ERROR:
		{
			m_result = FINDFIRSTVISIBLEPOSITIONONPATH_DONE_UNKNOWN_ERROR;
			shapeColor.m_lineColor = VisualColor::DarkRed;
			result = StopSampling;
			break;
		}

	case RAYCANGO_DONE_COLLISION_DETECTED:
	case RAYCANGO_DONE_ARRIVAL_IN_WRONG_FLOOR:
	case RAYCANGO_DONE_BORDER_DETECTED_WITHIN_MARGIN:
		{
			shapeColor.m_lineColor = VisualColor::Red;
			result = ContinueSampling;
			break;
		}

	case RAYCANGO_DONE_SUCCESS:
		{
			m_result = FINDFIRSTVISIBLEPOSITIONONPATH_DONE_SUCCESS;
			m_costToVisibleTargetOnPath = rayCanGoQuery.GetComputedCost();
			shapeColor.m_lineColor = VisualColor::Lime;
			result = StopSampling;
			break;
		}
	default:
		{
			KY_LOG_ERROR(("This must not happen !"));
			m_result = FINDFIRSTVISIBLEPOSITIONONPATH_DONE_UNKNOWN_ERROR;
			shapeColor.m_lineColor = VisualColor::DarkRed;
			result = StopSampling;
			break;
		}
	}

#ifndef KY_BUILD_SHIPPING
	if (m_displayList)
	{
		const KyFloat32 pointDisplaySize = Min(0.1f, 0.25f * m_samplingDistance);
		m_displayList->PushPoint(candidate.GetPosition(), pointDisplaySize, shapeColor);
		m_displayList->PushLine(rayCanGoQuery.GetStartPos(), rayCanGoQuery.GetDestPos(), shapeColor.m_lineColor);
	}
#endif

	return result;
}

} // namespace Kaim
