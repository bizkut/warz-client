/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{

template<class TLogic>
PathFromPolylineQuery<TLogic>::PathFromPolylineQuery() {}

template<class TLogic>
PathFromPolylineQuery<TLogic>::~PathFromPolylineQuery() {}

template<class TLogic>
void PathFromPolylineQuery<TLogic>::BindToDatabase(Database* database)
{
	BasePathFromPolylineQuery::BindToDatabase(database);
}

template<class TLogic>
void PathFromPolylineQuery<TLogic>::Initialize(const Vec3f* vertexBuffer, KyUInt32 vertexCount)
{
	BasePathFromPolylineQuery::Initialize(vertexBuffer, vertexCount);
}

template<class TLogic>
KY_INLINE void PathFromPolylineQuery<TLogic>::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	BasePathFromPolylineQuery::SetPositionSpatializationRange(positionSpatializationRange);
}

template<class TLogic>
KY_INLINE PathFromPolylineQueryResult PathFromPolylineQuery<TLogic>::GetResult()   const { return BasePathFromPolylineQuery::GetResult();   }
template<class TLogic>
KY_INLINE const Vec3f&                PathFromPolylineQuery<TLogic>::GetStartPos() const { return BasePathFromPolylineQuery::GetStartPos(); }
template<class TLogic>
KY_INLINE Path*                       PathFromPolylineQuery<TLogic>::GetPath()     const { return BasePathFromPolylineQuery::GetPath();     }
template<class TLogic>
KY_INLINE const PositionSpatializationRange& PathFromPolylineQuery<TLogic>::GetPositionSpatializationRange() const
{
	return BasePathFromPolylineQuery::GetPositionSpatializationRange();
}

template<class TLogic>
inline void PathFromPolylineQuery<TLogic>::ReleaseWorkingMemoryOnCancelDuringProcess(WorkingMemory* workingMemory)
{
	if (workingMemory == KY_NULL)
		workingMemory = m_database->GetWorkingMemory();

	SetResult(PATHFROMPOLYLINE_DONE_COMPUTATION_CANCELED);
	SetFinish(workingMemory);
}

template<class TLogic>
inline void PathFromPolylineQuery<TLogic>::Advance(WorkingMemory* workingMemory)
{
	ScopedSetPathNavigationProfile scopedSetPathNavigationProfile(this);

	++m_advanceCount;
	m_processStatus = QueryInProcess;

	PathFinderQueryUtils queryUtils(m_database, workingMemory, GetTraverseLogicUserData());
	switch (GetResult())
	{
	case PATHFROMPOLYLINE_NOT_PROCESSED :
		{
			if (m_polylinePositions.GetCount() == 0)
			{
				SetResult(PATHFROMPOLYLINE_DONE_ERROR_POLYLINE_EMPTY);
				SetFinish(workingMemory);
				return;
			}

			if (m_database->IsClear())
			{
				SetResult(PATHFROMPOLYLINE_DONE_DATABASE_EMPTY);
				SetFinish(workingMemory);
				return;
			}

			if (KY_FAILED(FirstStepOfClamperContextInit(workingMemory)))
			{
				if( GetResult() == PATHFROMPOLYLINE_NOT_PROCESSED)
					SetResult(PATHFROMPOLYLINE_DONE_ERROR_LACK_OF_WORKING_MEMORY);

				SetFinish(workingMemory);
				return;
			}

			SetResult(PATHFROMPOLYLINE_PROCESSING_PATHCLAMPING_INIT);
			break;
		}
	case PATHFROMPOLYLINE_PROCESSING_PATHCLAMPING_INIT :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			if (KY_FAILED(PathClamperContextInitSetp(workingMemory)))
			{
				SetResult(PATHFROMPOLYLINE_DONE_POLYLINE_INVALID);
				SetFinish(workingMemory);
				return;
			}

			PathClamperContext* clamperContext = workingMemory->GetOrCreatePathClamperContext();
			if (clamperContext->m_clamperStatus == PathClamperContext::Initialisation)
				// Init is not finished
				return;

			if (m_computeChannelMode == PATHFINDER_DO_NOT_COMPUTE_CHANNELS)
				// Do not clamp the raw path if a channel is to be computed
				SetResult(PATHFROMPOLYLINE_PROCESSING_PATHCLAMPING);
			else
				SetResult(PATHFROMPOLYLINE_PROCESSING_PATHBUILDING);

			break;
		}
	case PATHFROMPOLYLINE_PROCESSING_PATHCLAMPING :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			PathClamperContext* pathClamperContext = workingMemory->GetOrCreatePathClamperContext();
			for(KyUInt32 intersectionTestCount = 0; intersectionTestCount < m_numberOfIntersectionTestPerAdvance;)
			{
				ClampResult clampResult = (ClampResult)queryUtils.ClampOneEdge(intersectionTestCount);
				if (clampResult != ClampResult_SUCCESS)
				{
					if (clampResult == ClampResult_FAIL_CANGOHIT)
						SetResult(PATHFROMPOLYLINE_DONE_COMPUTATION_ERROR);
					else
						SetResult(PATHFROMPOLYLINE_DONE_ERROR_LACK_OF_WORKING_MEMORY);

					SetFinish(workingMemory);
					return;
				}

				if (pathClamperContext->IsClampingDone())
					break;
			}

			if (pathClamperContext->IsClampingDone())
			{
				SetResult(PATHFROMPOLYLINE_PROCESSING_PATHBUILDING);
			}

			break;
		}
	case PATHFROMPOLYLINE_PROCESSING_PATHBUILDING :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			PathClamperContext* pathClamperContext  = workingMemory->GetOrCreatePathClamperContext();
			pathClamperContext->m_startNavGraphEdgePtr.Invalidate();
			pathClamperContext->m_destNavGraphEdgePtr.Invalidate();

			Ptr<Path> rawPath = queryUtils.ComputePathFromPathClamperContext<TLogic>(m_startPos3f, *m_polylinePositions.Last(), GetPositionSpatializationRange());

			if (m_computeChannelMode == PATHFINDER_DO_NOT_COMPUTE_CHANNELS)
			{
				SetPath(rawPath);
				SetResult(PATHFROMPOLYLINE_DONE_PATH_FOUND);
				SetFinish(workingMemory);
				return;
			}

			ChannelArrayComputer& channelArrayComputer = *workingMemory->GetOrCreateChannelArrayComputer();
			channelArrayComputer.Init(rawPath);
			channelArrayComputer.SetTraverseLogicUserData(GetTraverseLogicUserData());
			channelArrayComputer.SetChannelComputerConfig(m_channelComputerConfig);

			SetResult(PATHFROMPOLYLINE_PROCESSING_CHANNEL_COMPUTE);
			break;
		}
	case PATHFROMPOLYLINE_PROCESSING_CHANNEL_COMPUTE :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			ChannelArrayComputer& channelArrayComputer = *workingMemory->GetOrCreateChannelArrayComputer();
			channelArrayComputer.Advance<TLogic>(workingMemory);

			if (channelArrayComputer.IsFinished())
			{
				if (channelArrayComputer.m_result != ChannelArrayResult_Done)
				{
					SetResult(PATHFROMPOLYLINE_DONE_CHANNELCOMPUTATION_ERROR);
					SetFinish(workingMemory);
					return;
				}

				SetPath(channelArrayComputer.m_pathWithChannels);
				channelArrayComputer.m_channelArray = KY_NULL;
				channelArrayComputer.m_pathWithChannels = KY_NULL;

				SetResult(PATHFROMPOLYLINE_DONE_PATH_FOUND);
				SetFinish(workingMemory);
				return;
			}

			break;
		}
	default:
		{
			m_lastAdvanceFrameIdx = m_database->m_frameIdx;
			SetFinish(workingMemory);
			break;
		}
	}
}

template<class TLogic>
inline KyResult PathFromPolylineQuery<TLogic>::FirstStepOfClamperContextInit(WorkingMemory* workingMemory)
{
	Box3f aabb3f;
	for(KyUInt32 i = 0; i < m_polylinePositions.GetCount(); ++i)
		aabb3f.ExpandByVec3(m_polylinePositions[i]);

	CellBox cellBox;
	m_database->GetDatabaseGenMetrics().GetCellBoxOfAnAABBox(aabb3f, cellBox);

	workingMemory->GetOrCreateNavDataChangeIndexInGrid()->Init(workingMemory, cellBox);
	if (workingMemory->m_navDataChangeIndexInGrid->IsInitialized() == false)
		return KY_ERROR;

	workingMemory->m_navDataChangeIndexInGrid->RetrieveAllIndicesFromDatabase(m_database);

	PathClamperContext* clamperContext = workingMemory->GetOrCreatePathClamperContext();

	clamperContext->m_triangleRawPtrNodes.Init(workingMemory);
	clamperContext->m_vertexRawPtrNodes.Init(workingMemory);
	clamperContext->m_clampNodes.Init(workingMemory);

	if (clamperContext->m_triangleRawPtrNodes.IsInitialized() == false)
		return KY_ERROR;
	if (clamperContext->m_vertexRawPtrNodes.IsInitialized() == false)
		return KY_ERROR;
	if (clamperContext->m_clampNodes.IsInitialized() == false)
	{
		KY_LOG_WARNING(("Not enough working memory for storing ClampNodes"));
		return KY_ERROR;
	}

	if (KY_FAILED(clamperContext->m_triangleRawPtrNodes.SetMinimumCapacity(m_polylinePositions.GetCount())))
		return KY_ERROR;

	if (KY_FAILED(clamperContext->m_clampNodes.SetMinimumCapacity(m_polylinePositions.GetCount())))
	{
		KY_LOG_WARNING(("Not enough working memory for storing ClampNodes"));
		return KY_ERROR;
	}

	// fill the clamp node following the channel from end to start with raycasts
	clamperContext->m_currentVertexIdx = (KyUInt16)m_polylinePositions.GetCount() - 1;
	clamperContext->m_currentIdxInTriangleBuffer = 0;
	clamperContext->m_currentStartTriangleRawPtr.Invalidate();
	clamperContext->m_currentStartPos = m_polylinePositions[clamperContext->m_currentVertexIdx];

	TriangleFromPosQuery query;
	query.BindToDatabase(m_database);
	query.Initialize(clamperContext->m_currentStartPos);
	query.PerformQuery();
	if (query.GetResult() != TRIANGLEFROMPOS_DONE_TRIANGLE_FOUND)
	{
		SetResult(PATHFROMPOLYLINE_DONE_END_OUTSIDE);
		return KY_ERROR;
	}

	clamperContext->m_currentStartIntegerPos     = query.GetInputIntegerPos();
	clamperContext->m_currentStartTriangleRawPtr = query.GetResultTrianglePtr().GetRawPtr();

	clamperContext->m_clamperStatus = PathClamperContext::Initialisation;

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult PathFromPolylineQuery<TLogic>::PathClamperContextInitSetp(WorkingMemory* workingMemory)
{
	PathClamperContext* clamperContext = workingMemory->GetOrCreatePathClamperContext();

	RayCastQuery<DefaultTraverseLogic> rayCastQuery;
	rayCastQuery.BindToDatabase(m_database);

	KyUInt32 intersectionTestCount = 0;
	while (clamperContext->m_currentVertexIdx > 0)
	{
		if (intersectionTestCount == m_numberOfRayCastPerAdavance)
			return KY_SUCCESS;

		ClampNodeIndex prevNodeIdx = clamperContext->m_currentIdxInTriangleBuffer + 1;
		ClampNodeIndex nextNodeIdx = clamperContext->m_currentIdxInTriangleBuffer - 1; //== ClampNodeIndex_Invalid si currentIdxInTriangleBuffer == 0

		clamperContext->m_triangleRawPtrNodes.PushBack_UnSafe(clamperContext->m_currentStartTriangleRawPtr);
		clamperContext->m_clampNodes.PushBack_UnSafe(ClampNode(clamperContext->m_currentStartPos,
			clamperContext-> m_currentStartIntegerPos,
			NodeTypeAndRawPtrDataIdx(NodeType_FreePositionOnNavMesh, clamperContext->m_currentIdxInTriangleBuffer),
			prevNodeIdx, nextNodeIdx));

		const Vec3f& nextPos = m_polylinePositions[clamperContext->m_currentVertexIdx - 1];
		rayCastQuery.Initialize(clamperContext->m_currentStartPos, nextPos.Get2d() - clamperContext->m_currentStartPos.Get2d());
		rayCastQuery.SetStartIntegerPos(clamperContext->m_currentStartIntegerPos);
		rayCastQuery.SetStartTrianglePtr(NavTrianglePtr(clamperContext->m_currentStartTriangleRawPtr));
		rayCastQuery.PerformQueryWithInputCoordPos(workingMemory);

		if (rayCastQuery.GetResult() != RAYCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED)
			return KY_ERROR;

		clamperContext->m_currentStartPos = nextPos;
		clamperContext->m_currentStartIntegerPos = rayCastQuery.GetArrivalIntegerPos();
		clamperContext->m_currentStartTriangleRawPtr = rayCastQuery.GetArrivalTrianglePtr().GetRawPtr();

		--clamperContext->m_currentVertexIdx;
		++clamperContext->m_currentIdxInTriangleBuffer;
	}

	clamperContext->m_triangleRawPtrNodes.PushBack_UnSafe(clamperContext->m_currentStartTriangleRawPtr);
	clamperContext->m_clampNodes.PushBack_UnSafe(ClampNode(clamperContext->m_currentStartPos, clamperContext->m_currentStartIntegerPos,
		NodeTypeAndRawPtrDataIdx(NodeType_FreePositionOnNavMesh, clamperContext->m_currentIdxInTriangleBuffer),
		ClampNodeIndex_Invalid, clamperContext->m_currentIdxInTriangleBuffer - 1));

	clamperContext->m_currentClampNodeIdx = (ClampNodeIndex)(m_polylinePositions.GetCount() - 1); // init with the start Node
	clamperContext->m_clamperStatus = PathClamperContext::NeedToComputeIntersections;

	return KY_SUCCESS;
}

template<class TLogic>
inline void PathFromPolylineQuery<TLogic>::SetFinish(WorkingMemory* workingMemory)
{
	m_processStatus = QueryDone; // TODO - CHANGE THIS!
	m_dataBaseChangeIdx = m_database ? m_database->m_navdataChangeIdx : 0;

	if (workingMemory->m_astarContext != KY_NULL)
	{
		workingMemory->m_clamperContext->ReleaseWorkingMemory();
		workingMemory->m_navDataChangeIndexInGrid->ReleaseWorkingMemoryBuffer();
	}

	if (workingMemory->m_channelArrayComputer != KY_NULL)
		workingMemory->m_channelArrayComputer->Clear();
}

template<class TLogic>
inline KyResult PathFromPolylineQuery<TLogic>::CheckNavDataChange(WorkingMemory* workingMemory)
{
	if(m_lastAdvanceFrameIdx != m_database->m_frameIdx)
	{
		if(workingMemory->m_navDataChangeIndexInGrid->HasNavDataChanged(m_database))
		{
			SetResult(PATHFROMPOLYLINE_DONE_NAVDATA_CHANGED);
			SetFinish(workingMemory);
			return KY_ERROR;
		}

		m_lastAdvanceFrameIdx = m_database->m_frameIdx;
	}

	return KY_SUCCESS;
}

}

