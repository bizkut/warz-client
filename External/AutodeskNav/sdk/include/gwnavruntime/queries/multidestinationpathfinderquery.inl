/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



namespace Kaim
{

template<class TLogic>
MultiDestinationPathFinderQuery<TLogic>::MultiDestinationPathFinderQuery()
	: m_traversal(TraversalCustomizer())
{
	m_traversal.m_traversalParams = &m_traversalParams;
	m_traversal.m_traversalCustomizer.m_query = this;
}

template<class TLogic>
MultiDestinationPathFinderQuery<TLogic>::~MultiDestinationPathFinderQuery() {}

template<class TLogic>
inline void MultiDestinationPathFinderQuery<TLogic>::ReleaseWorkingMemoryOnCancelDuringProcess(WorkingMemory* workingMemory)
{
	if (workingMemory == KY_NULL)
		workingMemory = m_database->GetWorkingMemory();

	SetResult(MULTIDESTINATIONPATHFINDER_DONE_COMPUTATION_CANCELED);
	SetFinish(workingMemory);
}


template<class TLogic>
inline bool MultiDestinationPathFinderQuery<TLogic>::ShouldReturnAfterTryingCanGo(WorkingMemory* workingMemory)
{
	if (m_start.m_navGraphEdgePtr[0].IsValid() || m_start.m_navGraphVertexPtr.IsValid())
		return false;

	KY_LOG_ERROR_IF(m_start.m_trianglePtr.IsValid() == false, ("Error, a valid NavTrianglePtr should have been found for start"));

	bool allDestReachableViaCanGo = true;
	for (KyUInt32 destIdx = 0; destIdx < m_destinations.GetCount(); ++destIdx)
	{
		PathFinderPoint& dest = m_destinations[destIdx];
		MultiDestinationPathFinderQueryResult& result = m_resultPerDestinations[destIdx];
		if (result != MULTIDESTINATIONPATHFINDER_NOT_PROCESSED) // dest is known as unreachable or dest is start
		{
			allDestReachableViaCanGo = false;
			continue;
		}

		if (dest.m_navGraphEdgePtr[0].IsValid() || dest.m_navGraphVertexPtr.IsValid())
		{
			allDestReachableViaCanGo = false;
			continue;
		}

		KY_LOG_ERROR_IF(dest.m_trianglePtr.IsValid() == false, ("Error, a valid NavTrianglePtr should have been found for dest %u", destIdx));

		MonodirectionalRayCanGoQuery<TLogic>& canGoQuery = m_rayCanGoQueries[destIdx];
		canGoQuery.BindToDatabase(m_database);
		canGoQuery.SetTraverseLogicUserData(GetTraverseLogicUserData());
		canGoQuery.SetMarginMode(PathMargin);
		canGoQuery.SetPositionSpatializationRange(GetPositionSpatializationRange());
		if((GetFinalOutputMode() == MULTIDESTINATIONPATHFINDER_BUILD_LOWEST_COST_PATH_ONLY)
			|| (GetFinalOutputMode() & MULTIDESTINATIONPATHFINDER_COMPUTE_RAW_COSTS))
			canGoQuery.SetComputeCostMode(QUERY_COMPUTE_COST_ALONG_3DAXIS);

		canGoQuery.Initialize(m_start.m_pos3f, dest.m_pos3f);
		canGoQuery.SetStartTrianglePtr(m_start.m_trianglePtr);
		canGoQuery.SetPerformQueryStat(QueryStat_PathFinder);
		canGoQuery.PerformQuery(workingMemory);
		if (canGoQuery.GetResult() != RAYCANGO_DONE_SUCCESS || canGoQuery.GetDestTrianglePtr() != dest.m_trianglePtr)
		{
			allDestReachableViaCanGo = false;
			continue;
		}

		m_resultPerDestinations[destIdx] = MULTIDESTINATIONPATHFINDER_TOPROCESS_FROM_CANGOQUERY;
	}

	return allDestReachableViaCanGo;
}


template<class TLogic>
inline void MultiDestinationPathFinderQuery<TLogic>::InitPropagationBounds()
{
	InitCellFilterFromAbstractGraph();
}


KY_EXIT_CLASS_TEMPLATE_SCOPE_BEGIN(MultiDestinationPathFinderQuery, Advance, TLogic)
for (KyUInt32 destIdx = 0; destIdx < self->GetDestCount(); ++destIdx)
{
	if (self->GetPath(destIdx))
		self->GetPath(destIdx)->SetNavigationProfileId(self->GetNavigationProfileId());
	if (self->GetAbstractPath(destIdx))
		self->GetAbstractPath(destIdx)->SetNavigationProfileId(self->GetNavigationProfileId());
	if (self->GetRawPath(destIdx))
		self->GetRawPath(destIdx)->SetNavigationProfileId(self->GetNavigationProfileId());
}
KY_EXIT_CLASS_SCOPE_END

template<class TLogic>
inline void MultiDestinationPathFinderQuery<TLogic>::Advance(WorkingMemory* workingMemory)
{
	KY_EXIT_SCOPE_TEMPLATE_INSTANCE(MultiDestinationPathFinderQuery, Advance, TLogic);

	++m_advanceCount;
	m_processStatus = QueryInProcess;

	PathFinderQueryUtils queryUtils(m_database, workingMemory, GetTraverseLogicUserData());
	queryUtils.SetPerformQueryStat(QueryStat_PathFinder);
	switch (GetResult())
	{
	case MULTIDESTINATIONPATHFINDER_NOT_PROCESSED :
		{
			m_traversal.m_traversalCustomizer.m_traverseLogicUserData = GetTraverseLogicUserData();

			m_lastAdvanceFrameIdx = m_database->m_frameIdx;

			m_paths.Resize(m_destinations.GetCount());
			m_abstractPaths.Resize(m_destinations.GetCount());
			m_rawPaths.Resize(m_destinations.GetCount());
			m_rawCosts.Resize(m_destinations.GetCount());
			m_rayCanGoQueries.Resize(m_destinations.GetCount());
			

			if (m_database->IsClear())
			{
				SetResult(MULTIDESTINATIONPATHFINDER_DONE_START_OUTSIDE);
				SetFinish(workingMemory);
				return;
			}

			m_traversal.m_astarContext = workingMemory->GetOrCreateAStarTraversalContext();

			if (KY_FAILED(ComputeStartGraphEdgeOrGraphVertexOrTriangle(queryUtils)))
			{
				SetFinish(workingMemory);
				return;
			}

			if (KY_FAILED(ComputeDestTriangleIfNeeded(queryUtils)))
			{
				SetResult(MULTIDESTINATIONPATHFINDER_DONE_ENDS_ARE_UNREACHABLE);
				SetFinish(workingMemory);
				return;
			}


			bool allDestIsStart = true;
			for (KyUInt32 destIdx = 0; destIdx < m_destinations.GetCount(); ++destIdx)
			{
				PathFinderPoint& dest = m_destinations[destIdx];
				MultiDestinationPathFinderQueryResult& destResult = m_resultPerDestinations[destIdx];
				if ((dest.m_pos3f == m_start.m_pos3f) ||
					(m_start.m_navGraphVertexPtr.IsValid() && m_start.m_navGraphVertexPtr == dest.m_navGraphVertexPtr))
				{
					destResult = MULTIDESTINATIONPATHFINDER_DONE_DEST_IS_START_NO_PATH;
				}
				else
				{
					allDestIsStart = false;
				}
			}
			if (allDestIsStart == true)
			{
				SetResult(MULTIDESTINATIONPATHFINDER_DONE_DESTS_ARE_START_NO_PATH);
				SetFinish(workingMemory);
				return;
			}
			
			if (GetTryCanGoMode() == MULTIDESTINATIONPATHFINDER_DO_TRY_RAYCANGO)
			{
				if (ShouldReturnAfterTryingCanGo(workingMemory) == true) // All dest nodes reachable by can go
				{
					if (ProcessFirstDestination() == false)
					{
						SetFinish(workingMemory);
						return;
					}

					SetResult(MULTIDESTINATIONPATHFINDER_PROCESSING_CANGOQUERY);
					m_resultPerDestinations[m_currentProcessedDestIdx] = MULTIDESTINATIONPATHFINDER_PROCESSING;
					return;
				}
			}

			if (KY_FAILED(InitializeContextAndStartNode(queryUtils)))
			{
				SetResult(MULTIDESTINATIONPATHFINDER_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			// Init all dest nodes,
			// including the dest nodes that are unreachable or which are at start pos,
			// so that indices match.
			if (KY_FAILED(InitAllDestinationNodes()))
			{
				SetResult(MULTIDESTINATIONPATHFINDER_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			InitPropagationBounds();

			// init the propagation with the neighbors of the startNode
			if (KY_FAILED(InitTraversalFromStartNode(queryUtils)))
			{
				SetResult(MULTIDESTINATIONPATHFINDER_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			if (m_traversal.IsThereNodeToVisit() == false)
			{
				if (HasExplorationReachedAtLeastADestNode() == false)
				{
					// no neighbor for the start nodes
					SetResult(MULTIDESTINATIONPATHFINDER_DONE_NO_PATH_FOUND);
					SetFinish(workingMemory);
					return;
				}

				SetResult(MULTIDESTINATIONPATHFINDER_PROCESSING_TRAVERSAL_DONE);
			}
			else
				SetResult(MULTIDESTINATIONPATHFINDER_PROCESSING_TRAVERSAL);

			break;
		}
	case MULTIDESTINATIONPATHFINDER_PROCESSING_TRAVERSAL :
		{
			if (KY_FAILED(CheckNavDataChangeFromAstarContext(workingMemory)))
				return;

			TraversalVisitNodeContext visitNodeContext;
			for(KyUInt32 numberOfNode = 0; numberOfNode < m_traversalParams.m_numberOfVisitedNodePerFrame; ++numberOfNode)
			{
				if (KY_FAILED(m_traversal.VisitNode(queryUtils, visitNodeContext)))
				{
					SetResult(MULTIDESTINATIONPATHFINDER_DONE_ERROR_LACK_OF_WORKING_MEMORY);
					SetFinish(workingMemory);
					return;
				}

				if (m_traversal.IsThereNodeToVisit() == false)
					break;
			}

			if (m_traversal.IsThereNodeToVisit() == false)
			{
				// Visual debugging
				// Uncomment this line to send the result of the propagation via the visual debugging
				// queryUtils.DisplayListPropagation("MultiDestinationPathFinderQuery");
				
				if (ProcessFirstDestination() == false)
				{
					SetFinish(workingMemory);
					return;
				}

				SetResult(MULTIDESTINATIONPATHFINDER_PROCESSING_TRAVERSAL_DONE);
			}

			break;
		}

	case MULTIDESTINATIONPATHFINDER_PROCESSING_TRAVERSAL_DONE :
		{
			if (KY_FAILED(CheckNavDataChangeFromAstarContext(workingMemory)))
				return;

			if (m_resultPerDestinations[m_currentProcessedDestIdx] == MULTIDESTINATIONPATHFINDER_TOPROCESS_FROM_CANGOQUERY)
				SetResult(MULTIDESTINATIONPATHFINDER_PROCESSING_CANGOQUERY);
			else
				SetResult(MULTIDESTINATIONPATHFINDER_PROCESSING_ABSTRACT_PATH);
			
			m_resultPerDestinations[m_currentProcessedDestIdx] = MULTIDESTINATIONPATHFINDER_PROCESSING;
			break;
		}

	case MULTIDESTINATIONPATHFINDER_PROCESSING_CANGOQUERY :
		{
			MonodirectionalRayCanGoQuery<TLogic>& canGoQuery = m_rayCanGoQueries[m_currentProcessedDestIdx];
			if (KY_FAILED(workingMemory->m_clamperContext->InitFromCanGo(workingMemory, canGoQuery)))
			{
				SetResult(MULTIDESTINATIONPATHFINDER_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			SetResult(MULTIDESTINATIONPATHFINDER_PROCESSING_PATHCLAMPING);
			break;
		}

	case MULTIDESTINATIONPATHFINDER_PROCESSING_ABSTRACT_PATH :
		{
			if (KY_FAILED(CheckNavDataChangeFromAstarContext(workingMemory)))
				return;
			
			AStarNodeIndex concreteDestAStarNodeIndex = AStarNodeIndex_FirstDestNode + m_currentProcessedDestIdx;
			BuildAbstractPath(workingMemory, concreteDestAStarNodeIndex);

			PathRefinerContext* refinerContext = workingMemory->GetOrCreatePathRefinerContext();
			if (KY_FAILED(refinerContext->InitFromAstarTraversalContext(workingMemory, workingMemory->m_astarContext, concreteDestAStarNodeIndex, false)))
			{
				SetResult(MULTIDESTINATIONPATHFINDER_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			SetResult(MULTIDESTINATIONPATHFINDER_PROCESSING_REFINING_INIT);
			break;
		}



	case MULTIDESTINATIONPATHFINDER_PROCESSING_REFINING_INIT :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			ComputePathRefinerConfigInternals();

			if (KY_FAILED(queryUtils.BuildRefinerBinaryHeap()))
			{
				SetResult(MULTIDESTINATIONPATHFINDER_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			PathRefinerContext* refinerContext = workingMemory->GetOrCreatePathRefinerContext();
			if (refinerContext->m_currentNodeIdx == refinerContext->m_refinerNodes.GetCount())
			{
				if (refinerContext->m_refinerBinHeap.IsEmpty())
					SetResult(MULTIDESTINATIONPATHFINDER_PROCESSING_PATHCLAMPING_INIT);
				else
					SetResult(MULTIDESTINATIONPATHFINDER_PROCESSING_REFINING);
			}

			break;
		}
	case MULTIDESTINATIONPATHFINDER_PROCESSING_REFINING :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			PathRefinerContext* refinerContext = workingMemory->GetOrCreatePathRefinerContext();
			for(KyUInt32 cangoTestCount = 0; cangoTestCount < m_traversalParams.m_numberOfCanGoTestInRefinerPerFrame;)
			{
				if (KY_FAILED(queryUtils.RefineOneNode<TLogic>(GetPositionSpatializationRange(), GetPathRefinerConfig(), cangoTestCount)))
				{
					SetResult(MULTIDESTINATIONPATHFINDER_DONE_ERROR_LACK_OF_WORKING_MEMORY);
					SetFinish(workingMemory);
					return;
				}

				// Visual debugging
				// Uncomment this line to send the result of the path refiner step by step
				// queryUtils.DisplayListRefining("MultiDestinationPathFinderQuery"); break;

				if (refinerContext->m_refinerBinHeap.IsEmpty())
					break;
			}

			if (refinerContext->m_refinerBinHeap.IsEmpty())
			{
				// Visual debugging
				// Uncomment this line to send the result of the path refiner, before the path is clamped
				// queryUtils.DisplayListRefining("MultiDestinationPathFinderQuery");

				SetResult(MULTIDESTINATIONPATHFINDER_PROCESSING_PATHCLAMPING_INIT);
			}

			break;
		}
	case MULTIDESTINATIONPATHFINDER_PROCESSING_PATHCLAMPING_INIT :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			if (KY_FAILED(workingMemory->m_clamperContext->InitFromRefinerContext(workingMemory, workingMemory->GetOrCreatePathRefinerContext())))
			{
				SetResult(MULTIDESTINATIONPATHFINDER_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			if (m_computeChannelMode == PATHFINDER_DO_NOT_COMPUTE_CHANNELS)
				// Do not clamp the raw path if a channel is to be computed
				SetResult(MULTIDESTINATIONPATHFINDER_PROCESSING_PATHCLAMPING);
			else
				SetResult(MULTIDESTINATIONPATHFINDER_PROCESSING_PATHBUILDING);

			break;
		}
	case MULTIDESTINATIONPATHFINDER_PROCESSING_PATHCLAMPING :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			PathClamperContext* pathClamperContext = workingMemory->GetOrCreatePathClamperContext();
			for(KyUInt32 intersectionTestCount = 0; intersectionTestCount < m_traversalParams.m_numberOfIntersectionTestPerFrame;)
			{
				ClampResult clampResult = (ClampResult)queryUtils.ClampOneEdge(intersectionTestCount);
				if (clampResult != ClampResult_SUCCESS)
				{
					if (clampResult == ClampResult_FAIL_CANGOHIT)
						SetResult(MULTIDESTINATIONPATHFINDER_DONE_COMPUTATION_ERROR);
					else
						SetResult(MULTIDESTINATIONPATHFINDER_DONE_ERROR_LACK_OF_WORKING_MEMORY);

					SetFinish(workingMemory);
					return;
				}

				if (pathClamperContext->IsClampingDone())
					break;
			}

			if (pathClamperContext->IsClampingDone())
			{
				SetResult(MULTIDESTINATIONPATHFINDER_PROCESSING_PATHBUILDING);
			}

			break;
		}
	case MULTIDESTINATIONPATHFINDER_PROCESSING_PATHBUILDING :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			PathClamperContext* pathClamperContext  = workingMemory->GetOrCreatePathClamperContext();
			if (m_abstractPaths[m_currentProcessedDestIdx] != KY_NULL)
			{
				Vec3f concreteDestPos = pathClamperContext->m_clampNodes[0].m_nodePosition;
				pathClamperContext->m_startNavGraphEdgePtr = m_start.m_navGraphEdgePtr[0];
				m_currentRawPath = queryUtils.ComputePathFromPathClamperContext<TLogic>(m_start.m_pos3f, concreteDestPos, GetPositionSpatializationRange());
			}
			else
			{
				PathFinderPoint& dest = m_destinations[m_currentProcessedDestIdx];
				pathClamperContext->m_startNavGraphEdgePtr = m_start.m_navGraphEdgePtr[0];
				pathClamperContext->m_destNavGraphEdgePtr  = dest.m_navGraphEdgePtr[0];				
				m_currentRawPath = queryUtils.ComputePathFromPathClamperContext<TLogic>(m_start.m_pos3f, dest.m_pos3f, GetPositionSpatializationRange());
			}

			if (m_computeChannelMode == PATHFINDER_DO_NOT_COMPUTE_CHANNELS)
			{
				m_paths[m_currentProcessedDestIdx] = m_currentRawPath;

				if (ProcessNextDestination(workingMemory) == true)
					break;

				SetResult(MULTIDESTINATIONPATHFINDER_DONE_SOME_PATHS_FOUND);
				SetFinish(workingMemory);
				return;
			}

			SetResult(MULTIDESTINATIONPATHFINDER_PROCESSING_CHANNEL_INIT);
			break;
		}
	case MULTIDESTINATIONPATHFINDER_PROCESSING_CHANNEL_INIT :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			ChannelArrayComputer& channelArrayComputer = *workingMemory->GetOrCreateChannelArrayComputer();
			channelArrayComputer.Init(m_currentRawPath);
			channelArrayComputer.SetTraverseLogicUserData(GetTraverseLogicUserData());
			channelArrayComputer.SetChannelComputerConfig(m_channelComputerConfig);
			
			SetResult(MULTIDESTINATIONPATHFINDER_PROCESSING_CHANNEL_COMPUTE);
			break;
		}
	case MULTIDESTINATIONPATHFINDER_PROCESSING_CHANNEL_COMPUTE :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			ChannelArrayComputer& channelArrayComputer = *workingMemory->GetOrCreateChannelArrayComputer();
			channelArrayComputer.Advance<TLogic>(workingMemory);

			if (channelArrayComputer.IsFinished())
			{
				if (channelArrayComputer.m_result != ChannelArrayResult_Done)
				{
					SetResult(MULTIDESTINATIONPATHFINDER_DONE_CHANNELCOMPUTATION_ERROR);
					SetFinish(workingMemory);
					return;
				}

				m_paths[m_currentProcessedDestIdx] = channelArrayComputer.m_pathWithChannels;
				channelArrayComputer.m_channelArray = KY_NULL;
				channelArrayComputer.m_pathWithChannels = KY_NULL;
				if (ProcessNextDestination(workingMemory) == true)
					break;

				SetResult(MULTIDESTINATIONPATHFINDER_DONE_SOME_PATHS_FOUND);
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
KY_INLINE KyUInt32 MultiDestinationPathFinderQuery<TLogic>::CountAbstractGraphNodes(WorkingMemory* workingMemory, AStarNodeIndex destNodeIdx, AStarNodeIndex& concreteDestinationNodeIdx)
{
	const AStarTraversalContext* astarContext = workingMemory->m_astarContext;
	const WorkingMemArray<AStarNode>& aStarNodes = astarContext->m_aStarNodes;

	const AStarNode* currentAstarNode = &aStarNodes[destNodeIdx];
	AStarNodeIndex currentNodeIdx = destNodeIdx;
	KyUInt32 abstractGraphNodeCount = 0;
	bool onAbstractPath = false;
	for(;;)
	{
		PathNodeType currentNodeType = currentAstarNode->GetNodeType();
		switch(currentNodeType)
		{
		case NodeType_AbstractGraphNode:
			++abstractGraphNodeCount;
			onAbstractPath = true;
			break;

		case NodeType_NavMeshEdge:
		case NodeType_NavGraphVertex:
		case NodeType_FreePositionOnNavMesh:
		case NodeType_FreePositionOnGraphEdge:
			if (onAbstractPath == true)
			{
				concreteDestinationNodeIdx = currentNodeIdx;
				onAbstractPath = false;
			}
			break;
		}

		if (currentAstarNode->GetIndexOfPredecessor() == AStarNodeIndex_Invalid)
		{
			// start node found
			if (onAbstractPath == true)
				concreteDestinationNodeIdx = AStarNodeIndex_Invalid;
			return abstractGraphNodeCount;
		}

		currentNodeIdx = currentAstarNode->GetIndexOfPredecessor();
		currentAstarNode = &aStarNodes[currentNodeIdx];
	}
}

template<class TLogic>
KY_INLINE void MultiDestinationPathFinderQuery<TLogic>::BuildAbstractPath(WorkingMemory* workingMemory, AStarNodeIndex& concreteDestAStarNodeIndex)
{
	AStarNodeIndex currentDestAStarNodeIndex = AStarNodeIndex_FirstDestNode + m_currentProcessedDestIdx;
	concreteDestAStarNodeIndex = currentDestAStarNodeIndex;
	KyUInt32 abstractNodeCount = CountAbstractGraphNodes(workingMemory, currentDestAStarNodeIndex, concreteDestAStarNodeIndex);
	KY_ASSERT(concreteDestAStarNodeIndex != AStarNodeIndex_Invalid);
	if (abstractNodeCount == 0)
	{
		m_abstractPaths[m_currentProcessedDestIdx] = KY_NULL;
		return;
	}

	PathFinderPoint& dest = m_destinations[m_currentProcessedDestIdx];
	KyUInt32 nodesCount = abstractNodeCount +2; // add start and end nodes;
	Path::CreateConfig config;
	config.m_nodesCount = nodesCount;

	Ptr<Path> pathPtr = Path::CreatePath(config);
	Path& path = *pathPtr;

	KyUInt32 nodeIdx = nodesCount - 1;
	const DatabaseGenMetrics& genMetrics = m_database->GetDatabaseGenMetrics();
	WorldIntegerPos integerPos = genMetrics.GetWorldIntegerPosFromVec3f(dest.m_pos3f);
	path.SetNodePosition3fAndInteger(nodeIdx, dest.m_pos3f, integerPos);
	path.GetPathCellBox().ExpandByVec2(integerPos.m_cellPos);
	path.SetPathEdgeType(nodeIdx-1, PathEdgeType_AbstractEdge);
	--nodeIdx;

	AStarTraversalContext* astarContext = workingMemory->m_astarContext;
	WorkingMemArray<AStarNode>& aStarNodes = astarContext->m_aStarNodes;
	AStarNode* currentAstarNode = &aStarNodes[currentDestAStarNodeIndex];
	KyFloat32 currentDistance = currentAstarNode->m_costFromStart;
	path.SetPathCostAndDistance(currentDistance, currentDistance);
	path.m_database = m_database;

	AStarNodeIndex currentNodeIdx = currentDestAStarNodeIndex;
	for(;;)
	{
		PathNodeType currentNodeType = currentAstarNode->GetNodeType();
		if (currentNodeType == NodeType_AbstractGraphNode)
		{
			integerPos = genMetrics.GetWorldIntegerPosFromVec3f(currentAstarNode->m_nodePosition);
			path.SetNodePosition3fAndInteger(nodeIdx, currentAstarNode->m_nodePosition, integerPos);
			path.GetPathCellBox().ExpandByVec2(integerPos.m_cellPos);
			path.SetPathEdgeType(nodeIdx-1, PathEdgeType_AbstractEdge);
			--nodeIdx;
		}


		if (currentAstarNode->GetIndexOfPredecessor() == AStarNodeIndex_Invalid)
		{
			integerPos = genMetrics.GetWorldIntegerPosFromVec3f(m_start.m_pos3f);
			path.SetNodePosition3fAndInteger(nodeIdx, m_start.m_pos3f, integerPos);
			path.GetPathCellBox().ExpandByVec2(integerPos.m_cellPos);
			break;
		}

		currentNodeIdx = currentAstarNode->GetIndexOfPredecessor();
		currentAstarNode = &aStarNodes[currentNodeIdx];
	}

	m_abstractPaths[m_currentProcessedDestIdx] = pathPtr;
}


template<class TLogic>
KY_INLINE bool MultiDestinationPathFinderQuery<TLogic>::HasNavDataChanged()
{
	return m_traversal.m_astarContext->m_edgeIndexGrid.HasNavDataChanged(m_database);
}

template<class TLogic>
inline void MultiDestinationPathFinderQuery<TLogic>::SetFinish(WorkingMemory* workingMemory)
{
	m_processStatus = QueryDone; // TODO - CHANGE THIS!
	m_dataBaseChangeIdx = m_database ? m_database->m_navdataChangeIdx : 0;
	
	m_currentRawPath = KY_NULL;

	if (workingMemory->m_astarContext != KY_NULL)
	{
		workingMemory->m_astarContext->ReleaseWorkingMemory();
		workingMemory->m_refinerContext->ReleaseWorkingMemory();
		workingMemory->m_clamperContext->ReleaseWorkingMemory();
		workingMemory->m_navDataChangeIndexInGrid->ReleaseWorkingMemoryBuffer();
	}
	if (workingMemory->m_channelArrayComputer != KY_NULL)
		workingMemory->m_channelArrayComputer->Clear();
}

template<class TLogic>
inline KyResult MultiDestinationPathFinderQuery<TLogic>::CheckNavDataChange(WorkingMemory* workingMemory)
{
	if(m_lastAdvanceFrameIdx != m_database->m_frameIdx)
	{
		if(workingMemory->m_navDataChangeIndexInGrid->HasNavDataChanged(m_database))
		{
			SetResult(MULTIDESTINATIONPATHFINDER_DONE_NAVDATA_CHANGED);
			SetFinish(workingMemory);
			return KY_ERROR;
		}

		m_lastAdvanceFrameIdx = m_database->m_frameIdx;
	}

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult MultiDestinationPathFinderQuery<TLogic>::CheckNavDataChangeFromAstarContext(WorkingMemory* workingMemory)
{
	if(m_lastAdvanceFrameIdx != m_database->m_frameIdx)
	{
		if(m_traversal.m_astarContext->m_edgeIndexGrid.HasNavDataChanged(m_database))
		{
			SetResult(MULTIDESTINATIONPATHFINDER_DONE_NAVDATA_CHANGED);
			SetFinish(workingMemory);
			return KY_ERROR;
		}

		m_lastAdvanceFrameIdx = m_database->m_frameIdx;
	}

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult MultiDestinationPathFinderQuery<TLogic>::ComputeStartGraphEdgeOrGraphVertexOrTriangle(PathFinderQueryUtils& queryUtils)
{
	if (m_start.m_navGraphEdgePtr[0].IsValid())
	{
		m_start.m_insidePos3f = m_start.m_pos3f;
		return KY_SUCCESS;
	}

	if (m_start.m_navGraphVertexPtr.IsValid())
	{
		m_start.m_insidePos3f = m_start.m_navGraphVertexPtr.GetGraphVertexPosition();
		return KY_SUCCESS;
	}

	return ComputeStartTriangle(queryUtils);
}

template<class TLogic>
inline KyResult MultiDestinationPathFinderQuery<TLogic>::ComputeStartTriangle(PathFinderQueryUtils& queryUtils)
{
	KY_DEBUG_ASSERTN(queryUtils.m_database->GetActiveData() != KY_NULL, ("the PathFinderQueryUtils need a valid activeData"));

	if (queryUtils.FindTriangleFromPositionIfNotValid(m_start.m_trianglePtr, m_start.m_pos3f, GetPositionSpatializationRange()) == true)
	{
		if(m_traversal.m_traversalCustomizer.CanTraverse(m_start.m_trianglePtr.GetRawPtr(), KY_NULL) == true)
		{
			m_start.m_insidePos3f = m_start.m_pos3f;
			return KY_SUCCESS;
		}
	}

	if (GetFromOutsideNavMeshDistance() > 0.f)
	{
		// the position is outside the NavMesh or in an invalid navtag. Try to hook on the NavMesh
		if (queryUtils.TryToHookOnNavMesh<TLogic>(m_start.m_pos3f, GetPositionSpatializationRange(), GetFromOutsideNavMeshDistance(),
			0.1f /*distFromObstacle*/, m_start.m_insidePos3f, m_start.m_trianglePtr) == KY_SUCCESS)
		{
			return KY_SUCCESS;
		}
	}

	if (m_start.m_trianglePtr.IsValid() == false)
		SetResult(MULTIDESTINATIONPATHFINDER_DONE_START_OUTSIDE);
	else
		SetResult(MULTIDESTINATIONPATHFINDER_DONE_START_NAVTAG_FORBIDDEN);

	return KY_ERROR;
}


template<class TLogic>
inline KyResult MultiDestinationPathFinderQuery<TLogic>::ComputeDestTriangleIfNeeded(PathFinderQueryUtils& queryUtils)
{
	KyResult result = KY_ERROR;
	for (KyUInt32 destIdx = 0; destIdx < m_destinations.GetCount(); ++destIdx)
	{
		PathFinderPoint& dest = m_destinations[destIdx];
		MultiDestinationPathFinderQueryResult& destResult = m_resultPerDestinations[destIdx];
		if(dest.m_navGraphEdgePtr[0].IsValid())
		{
			dest.m_insidePos3f = dest.m_pos3f;
			result = KY_SUCCESS;
			continue;
		}

		if (dest.m_navGraphVertexPtr.IsValid())
		{
			dest.m_insidePos3f = dest.m_navGraphVertexPtr.GetGraphVertexPosition();
			result = KY_SUCCESS;
			continue;
		}

		KY_DEBUG_ASSERTN(queryUtils.m_database->GetActiveData() != KY_NULL, ("the PathFinderQueryUtils need a valid activeData"));

		if (queryUtils.FindTriangleFromPositionIfNotValid(dest.m_trianglePtr, dest.m_pos3f, GetPositionSpatializationRange()) == true)
		{
			if(m_traversal.m_traversalCustomizer.CanTraverse(dest.m_trianglePtr.GetRawPtr(), KY_NULL) == true)
			{
				dest.m_insidePos3f = dest.m_pos3f;
				result = KY_SUCCESS;
				continue;
			}
		}

		if (GetToOutsideNavMeshDistance() > 0.f)
		{
			// the position is outside the NavMesh or in an invalid navtag. Try to hook on the NavMesh
			if (queryUtils.TryToHookOnNavMesh<TLogic>(dest.m_pos3f, GetPositionSpatializationRange(),
				GetToOutsideNavMeshDistance(), 0.1f /*distFromObstacle*/, dest.m_insidePos3f, dest.m_trianglePtr) == KY_SUCCESS)
			{
				result = KY_SUCCESS;
				continue;
			}
		}

		if (dest.m_trianglePtr.IsValid() == false)
			destResult = MULTIDESTINATIONPATHFINDER_DONE_END_OUTSIDE;
		else
			destResult = MULTIDESTINATIONPATHFINDER_DONE_END_NAVTAG_FORBIDDEN;
	}

	return result;
}

template<class TLogic>
inline KyResult MultiDestinationPathFinderQuery<TLogic>::InitializeContextAndStartNode(PathFinderQueryUtils& queryUtils)
{
	if (m_start.m_navGraphEdgePtr[0].IsValid())
	{
		KY_LOG_WARNING_IF(m_start.m_trianglePtr.IsValid(), ("The start NavGraph Edge ptr will override the start Triangle ptr!"));
		KY_LOG_WARNING_IF(m_start.m_navGraphVertexPtr.IsValid(), ("The start NavGraph Edge ptr will override the start NavGraphEdge ptr!"));

		return m_traversal.InitializeContextAndStartNode(queryUtils, m_start.m_pos3f, m_start.m_navGraphEdgePtr[0]);
	}

	if (m_start.m_navGraphVertexPtr.IsValid())
	{
		KY_LOG_WARNING_IF(m_start.m_trianglePtr.IsValid(), ("The start NavGraph Edge ptr will override the start Triangle ptr!"));

		return m_traversal.InitializeContextAndStartNode(queryUtils, m_start.m_insidePos3f, m_start.m_navGraphVertexPtr);
	}

	return m_traversal.InitializeContextAndStartNode(queryUtils, m_start.m_insidePos3f, m_start.m_trianglePtr);
}

template<class TLogic>
inline KyResult MultiDestinationPathFinderQuery<TLogic>::InitAllDestinationNodes()
{
	for (KyUInt32 destIdx = 0; destIdx < m_destinations.GetCount(); ++destIdx)
	{
		PathFinderPoint& dest = m_destinations[destIdx];

		if (m_resultPerDestinations[destIdx] == MULTIDESTINATIONPATHFINDER_TOPROCESS_FROM_CANGOQUERY)
		{
			m_traversal.InitializeInvalidDestNode(); // no need to look for this destination in the traversal
		}
		else if(dest.m_navGraphEdgePtr[0].IsValid())
		{
			if (InitDestinationNodeFromDestNavGraphEdge(dest) == KY_ERROR)
				return KY_ERROR;
		}
		else if(dest.m_navGraphVertexPtr.IsValid())
		{
			if (InitDestinationNodeFromDestNavGraphVertex(dest) == KY_ERROR)
				return KY_ERROR;
		}
		else if(dest.m_trianglePtr.IsValid())
		{
			if (InitDestinationNodeFromDestNavTriangle(dest) == KY_ERROR)
				return KY_ERROR;
		}
		else
		{
			m_traversal.InitializeInvalidDestNode();
		}

		KY_ASSERT(AStarNodeIndex_FirstDestNode + destIdx == m_traversal.m_astarContext->m_aStarNodes.GetCount() - 1);
	}

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult MultiDestinationPathFinderQuery<TLogic>::InitDestinationNodeFromDestNavGraphEdge(PathFinderPoint& dest)
{
	KY_FORWARD_ERROR_NO_LOG(m_traversal.InitializeDestNode(dest.m_pos3f, dest.m_navGraphEdgePtr[0]));

	return KY_SUCCESS;
}
template<class TLogic>
inline KyResult MultiDestinationPathFinderQuery<TLogic>::InitDestinationNodeFromDestNavGraphVertex(PathFinderPoint& dest)
{
	KY_FORWARD_ERROR_NO_LOG(m_traversal.InitializeDestNode(dest.m_navGraphVertexPtr));

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult MultiDestinationPathFinderQuery<TLogic>::InitDestinationNodeFromDestNavTriangle(PathFinderPoint& dest)
{
	KY_FORWARD_ERROR_NO_LOG(m_traversal.InitializeDestNode(dest.m_insidePos3f, dest.m_trianglePtr));

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult MultiDestinationPathFinderQuery<TLogic>::InitTraversalFromStartNode(PathFinderQueryUtils& queryUtils)
{
	ActiveData* activeData = queryUtils.m_database->GetActiveData();
	if (m_start.m_navGraphEdgePtr[0].IsValid())
	{
		NavGraphEdgeDirection navGraphEdgePathfindMode = NAVGRAPHEDGE_MONODIRECTIONAL;
		if (m_start.m_navGraphEdgePtr[1].IsValid() == true)
			navGraphEdgePathfindMode = NAVGRAPHEDGE_BIDIRECTIONAL;
		
		return m_traversal.ExploreAllNodesInNavGraphEdge(activeData, m_start.m_pos3f, m_start.m_navGraphEdgePtr[0].GetRawPtr(), AStarNodeIndex_StartNode, navGraphEdgePathfindMode);
	}

	if (m_start.m_navGraphVertexPtr.IsValid())
	{
		bool allDestPrecededByStart = true;
		for (KyUInt32 destIdx = 0; destIdx < m_destinations.GetCount(); ++destIdx)
		{
			PathFinderPoint& dest = m_destinations[destIdx];
			AStarNodeIndex currentDestAStarNodeIndex = AStarNodeIndex_FirstDestNode + destIdx;
			if (m_start.m_navGraphVertexPtr == dest.m_navGraphVertexPtr)
				m_traversal.m_astarContext->m_aStarNodes[currentDestAStarNodeIndex].m_indexOfPredecessor = AStarNodeIndex_StartNode;
			else
				allDestPrecededByStart = false;
		}

		if (allDestPrecededByStart == false)
		{
			// As we create directly an usual node from which we can start the traversal without manually exploring its neighbor nodes, we need to add it to the binHeap
			KY_FORWARD_ERROR_NO_LOG(m_traversal.m_astarContext->CheckTraversalBinaryHeapMemory());
			m_traversal.m_astarContext->m_traversalBinHeap.Insert(0);
		}

		return KY_SUCCESS; // Nothing to do since the StartNode is already an usual AstarNode(NavGraphVertex)
	}

	return m_traversal.ExploreAllNodesInTriangle(activeData, m_start.m_trianglePtr.GetRawPtr(), AStarNodeIndex_StartNode);
}


template<class TLogic>
bool MultiDestinationPathFinderQuery<TLogic>::HasExplorationReachedAllDestNodes() 
{
	for (KyUInt32 destIdx = 0; destIdx < m_destinations.GetCount(); ++destIdx)
	{
		AStarNodeIndex currentDestAStarNodeIndex = AStarNodeIndex_FirstDestNode + destIdx;
		if (m_traversal.m_astarContext->m_aStarNodes[currentDestAStarNodeIndex].m_indexOfPredecessor == AStarNodeIndex_Invalid)
			return false;
	}
	return true;
}

template<class TLogic>
bool MultiDestinationPathFinderQuery<TLogic>::HasExplorationReachedAtLeastADestNode() 
{
	for (KyUInt32 destIdx = 0; destIdx < m_destinations.GetCount(); ++destIdx)
	{
		AStarNodeIndex currentDestAStarNodeIndex = AStarNodeIndex_FirstDestNode + destIdx;
		if (m_traversal.m_astarContext->m_aStarNodes[currentDestAStarNodeIndex].m_indexOfPredecessor != AStarNodeIndex_Invalid)
			return true;
	}
	return false;
}

template<class TLogic>
void MultiDestinationPathFinderQuery<TLogic>::ComputeRawCosts()
{
	// if computing only raw costs or building only the cheapest path, result for the destination must be set
	bool mustSetDestResults = (
		(GetFinalOutputMode() == MULTIDESTINATIONPATHFINDER_BUILD_LOWEST_COST_PATH_ONLY) ||
		(GetFinalOutputMode() == MULTIDESTINATIONPATHFINDER_COMPUTE_RAW_COSTS));
	
	for (KyUInt32 destIdx = 0; destIdx < m_destinations.GetCount(); ++destIdx)
	{
		switch (m_resultPerDestinations[destIdx])
		{
		case MULTIDESTINATIONPATHFINDER_TOPROCESS_FROM_CANGOQUERY:
			{
				MonodirectionalRayCanGoQuery<TLogic>& rayCanGo = m_rayCanGoQueries[destIdx];
				m_rawCosts[destIdx] = rayCanGo.GetComputedCost();
				if (mustSetDestResults)
					m_resultPerDestinations[destIdx] = MULTIDESTINATIONPATHFINDER_DONE_PATH_FOUND;
				break;
			}
		case MULTIDESTINATIONPATHFINDER_NOT_PROCESSED:
			{
				AStarNodeIndex currentDestAStarNodeIndex = AStarNodeIndex_FirstDestNode + destIdx;
				AStarNode& node = m_traversal.m_astarContext->m_aStarNodes[currentDestAStarNodeIndex];
				if (node.m_indexOfPredecessor != AStarNodeIndex_Invalid)
				{
					m_rawCosts[destIdx] = node.m_costFromStart;
					if (mustSetDestResults)
						m_resultPerDestinations[destIdx] = MULTIDESTINATIONPATHFINDER_DONE_PATH_FOUND;
				}
				else
				{
					m_rawCosts[destIdx] = KyFloat32MAXVAL;
					if (mustSetDestResults)
						m_resultPerDestinations[destIdx] = MULTIDESTINATIONPATHFINDER_DONE_PATH_NOT_FOUND;
				}
				break;
			}
		default:
			{
				m_rawCosts[destIdx] = KyFloat32MAXVAL;
				break;
			}
		}
	}
}


template<class TLogic>
void MultiDestinationPathFinderQuery<TLogic>::BuildRawPaths()
{
	const DatabaseGenMetrics& genMetrics = m_database->GetDatabaseGenMetrics();
	bool mustSetDestResults = ((GetFinalOutputMode() & MULTIDESTINATIONPATHFINDER_BUILD_PATHS) == 0); // if not building refined paths, result for the destination must be set
	for (KyUInt32 destIdx = 0; destIdx < m_destinations.GetCount(); ++destIdx)
	{
		switch (m_resultPerDestinations[destIdx])
		{
		case MULTIDESTINATIONPATHFINDER_TOPROCESS_FROM_CANGOQUERY:
			{
				KyUInt32 nodesCount = 2;
				Path::CreateConfig config;
				config.m_nodesCount = nodesCount;

				Ptr<Path> pathPtr = Path::CreatePath(config);
				Path& path = *pathPtr;

				const PathFinderPoint& dest = m_destinations[destIdx];
				WorldIntegerPos integerPos = genMetrics.GetWorldIntegerPosFromVec3f(dest.m_pos3f);
				path.SetNodePosition3fAndInteger(1, dest.m_pos3f, integerPos);
				path.GetPathCellBox().ExpandByVec2(integerPos.m_cellPos);
				path.SetPathEdgeType(0, PathEdgeType_OnNavMesh);

				integerPos = genMetrics.GetWorldIntegerPosFromVec3f(m_start.m_pos3f);
				path.SetNodePosition3fAndInteger(0, m_start.m_pos3f, integerPos);
				path.GetPathCellBox().ExpandByVec2(integerPos.m_cellPos);

				MonodirectionalRayCanGoQuery<TLogic>& rayCanGo = m_rayCanGoQueries[destIdx];
				path.SetPathCostAndDistance(rayCanGo.GetComputedCost(), Distance(dest.m_pos3f, m_start.m_pos3f));
				m_rawPaths[destIdx] = pathPtr;
				if (mustSetDestResults)
					m_resultPerDestinations[destIdx] = MULTIDESTINATIONPATHFINDER_DONE_PATH_FOUND;
				break;
			}

		case MULTIDESTINATIONPATHFINDER_NOT_PROCESSED:
			{
				AStarNodeIndex currentDestAStarNodeIndex = AStarNodeIndex_FirstDestNode + destIdx;
				const AStarNode& destNode = m_traversal.m_astarContext->m_aStarNodes[currentDestAStarNodeIndex];
				if (destNode.m_indexOfPredecessor == AStarNodeIndex_Invalid)
				{
					if (mustSetDestResults)
						m_resultPerDestinations[destIdx] = MULTIDESTINATIONPATHFINDER_DONE_PATH_NOT_FOUND;
					break; // no Path
				}

				// count Nodes in Path
				KyUInt32 nodesCount = 0;
				bool startIsOutside = (m_start.m_pos3f != m_start.m_insidePos3f);
				PathFinderPoint& dest = m_destinations[destIdx];
				bool destIsOutside = (dest.m_pos3f != dest.m_insidePos3f);
				if (startIsOutside)
					++nodesCount;
				if (destIsOutside)
					++nodesCount;
				const AStarNode* node = &destNode;
				while (node)
				{
					++nodesCount;
					if (node->GetIndexOfPredecessor() == AStarNodeIndex_Invalid)
						break;
					node = &m_traversal.m_astarContext->m_aStarNodes[node->GetIndexOfPredecessor()];
				}
				
				// instantiate path
				Path::CreateConfig config;
				config.m_nodesCount = nodesCount;
				Ptr<Path> pathPtr = Path::CreatePath(config);
				Path& path = *pathPtr;

				// set outside dest node if any
				node = &destNode;
				KyUInt32 nodeIdx = nodesCount - 1;
				if (destIsOutside)
				{
					const WorldIntegerPos integerPos = genMetrics.GetWorldIntegerPosFromVec3f(dest.m_pos3f);
					path.SetNodePosition3fAndInteger(nodeIdx, dest.m_pos3f, integerPos);
					path.GetPathCellBox().ExpandByVec2(integerPos.m_cellPos);
					PathEdgeType pathEdgeType = PathEdgeType_Undefined;
					switch (node->GetNodeType())
					{
					case NodeType_NavMeshEdge:
					case NodeType_FreePositionOnNavMesh:
						pathEdgeType = PathEdgeType_FromNavMeshToOutside;
						break;
					case NodeType_NavGraphVertex:
					case NodeType_FreePositionOnGraphEdge:
						pathEdgeType = PathEdgeType_FromNavGraphToOutside;
						break;
					default:
						break;			
					}
					path.SetPathEdgeType(nodeIdx-1, pathEdgeType);
					--nodeIdx;
				}
				
				// add nodes
				for (;;)
				{
					const WorldIntegerPos integerPos = genMetrics.GetWorldIntegerPosFromVec3f(node->m_nodePosition);
					path.SetNodePosition3fAndInteger(nodeIdx, node->m_nodePosition, integerPos);
					path.GetPathCellBox().ExpandByVec2(integerPos.m_cellPos);
					if (node->GetIndexOfPredecessor() == AStarNodeIndex_Invalid)
						break; // start node has no predecessor

					const AStarNode& predecessorNode = m_traversal.m_astarContext->m_aStarNodes[node->GetIndexOfPredecessor()];
					PathEdgeType pathEdgeType = GetPathEdgeType(predecessorNode, *node);
					KY_ASSERT(nodeIdx >= 1);
					path.SetPathEdgeType(nodeIdx-1, pathEdgeType);
					node = &predecessorNode;
					--nodeIdx;
				}

				// set outside start node if any
				if (startIsOutside)
				{
					KY_ASSERT(nodeIdx == 0);
					const WorldIntegerPos integerPos = genMetrics.GetWorldIntegerPosFromVec3f(m_start.m_pos3f);
					path.SetNodePosition3fAndInteger(0, m_start.m_pos3f, integerPos);
					path.GetPathCellBox().ExpandByVec2(integerPos.m_cellPos);
					PathEdgeType pathEdgeType = PathEdgeType_Undefined;
					switch (node->GetNodeType())
					{
					case NodeType_NavMeshEdge:
					case NodeType_FreePositionOnNavMesh:
						pathEdgeType = PathEdgeType_FromOutsideToNavMesh;
						break;
					case NodeType_NavGraphVertex:
					case NodeType_FreePositionOnGraphEdge:
						pathEdgeType = PathEdgeType_FromOutsideToNavGraph;
						break;
					default:
						break;			
					}
					path.SetPathEdgeType(nodeIdx, pathEdgeType);
				}

				// compute distance and set it with cost to the path
				KyFloat32 distance = 0.f;
				for (KyUInt32 i = 0; i < path.GetEdgeCount(); ++i)
					distance += Distance(path.GetPathEdgeStartPosition(i), path.GetPathEdgeEndPosition(i));
				path.SetPathCostAndDistance(destNode.m_costFromStart, distance);
				m_rawPaths[destIdx] = pathPtr;
				if (mustSetDestResults)
					m_resultPerDestinations[destIdx] = MULTIDESTINATIONPATHFINDER_DONE_PATH_FOUND;
				break;
			}
		
		default:
			break;
		}
	}
}

template<class TLogic>
PathEdgeType MultiDestinationPathFinderQuery<TLogic>::GetPathEdgeType(const AStarNode& predecessorNode, const AStarNode& node)
{
	switch (node.GetNodeType())
	{
	case NodeType_NavMeshEdge:
	case NodeType_FreePositionOnNavMesh:
		switch (predecessorNode.GetNodeType())
		{
		case NodeType_NavMeshEdge:
		case NodeType_FreePositionOnNavMesh:
			return PathEdgeType_OnNavMesh;
		case NodeType_NavGraphVertex:
		case NodeType_FreePositionOnGraphEdge:
			return PathEdgeType_OnNavGraph;
		case NodeType_AbstractGraphNode:
			return PathEdgeType_AbstractEdge;
		default:
		case NodeType_Invalid:
			return PathEdgeType_Undefined;
		}

	case NodeType_NavGraphVertex:
	case NodeType_FreePositionOnGraphEdge:
		switch (predecessorNode.GetNodeType())
		{
		case NodeType_NavMeshEdge:
		case NodeType_FreePositionOnNavMesh:
			return PathEdgeType_OnNavGraph;
		case NodeType_NavGraphVertex:
		case NodeType_FreePositionOnGraphEdge:
			return PathEdgeType_OnNavGraph;
		case NodeType_AbstractGraphNode:
			return PathEdgeType_AbstractEdge;
		default:
		case NodeType_Invalid:
			return PathEdgeType_Undefined;
		}

	case NodeType_AbstractGraphNode:
		return PathEdgeType_AbstractEdge;

	default:
	case NodeType_Invalid:
		return PathEdgeType_Undefined;
	}
}

template<class TLogic>
bool MultiDestinationPathFinderQuery<TLogic>::ProcessFirstDestination()
{
	if (GetFinalOutputMode() == MULTIDESTINATIONPATHFINDER_BUILD_LOWEST_COST_PATH_ONLY)
	{
		ComputeRawCosts();
		KyFloat32 lowestCost = KyFloat32MAXVAL;
		KyUInt32 cheapestDestIdx = KyUInt32MAXVAL;
		for (KyUInt32 destIdx = 0; destIdx < m_destinations.GetCount(); ++destIdx)
		{
			if (m_rawCosts[destIdx] < lowestCost)
			{
				lowestCost = m_rawCosts[destIdx];
				cheapestDestIdx = destIdx;
			}
		}

		if (cheapestDestIdx == KyUInt32MAXVAL)
		{
			SetResult(MULTIDESTINATIONPATHFINDER_DONE_NO_PATH_FOUND);
			return false;
		}

		m_currentProcessedDestIdx = cheapestDestIdx;
	}
	else
	{
		// update result per destination
		if (UpdateResultAndSelectFirstDestination() == false)
		{
			SetResult(MULTIDESTINATIONPATHFINDER_DONE_NO_PATH_FOUND);
			return false;
		}

		if ((GetFinalOutputMode() & MULTIDESTINATIONPATHFINDER_COMPUTE_RAW_COSTS) != 0)
			ComputeRawCosts();
		if ((GetFinalOutputMode() & MULTIDESTINATIONPATHFINDER_BUILD_RAW_PATHS) != 0)
			BuildRawPaths();

		if ((GetFinalOutputMode() & MULTIDESTINATIONPATHFINDER_BUILD_PATHS) == 0)
		{
			SetResult(MULTIDESTINATIONPATHFINDER_DONE_SOME_PATHS_FOUND);
			return false;
		}
	}

	return true;
}

template<class TLogic>
bool MultiDestinationPathFinderQuery<TLogic>::UpdateResultAndSelectFirstDestination()
{
	m_currentProcessedDestIdx = KyUInt32MAXVAL;
	KyUInt32 destIdx = m_destinations.GetCount();
	for (--destIdx; destIdx != KyUInt32MAXVAL; --destIdx)
	{
		if (m_resultPerDestinations[destIdx] == MULTIDESTINATIONPATHFINDER_TOPROCESS_FROM_CANGOQUERY)
		{
			m_currentProcessedDestIdx = destIdx;
			continue;
		}
		else if (m_resultPerDestinations[destIdx] != MULTIDESTINATIONPATHFINDER_NOT_PROCESSED)
			continue; // node are either known as unreachable, or are at start pos.

		AStarNodeIndex currentDestAStarNodeIndex = AStarNodeIndex_FirstDestNode + destIdx;
		if (m_traversal.m_astarContext->m_aStarNodes[currentDestAStarNodeIndex].m_indexOfPredecessor == AStarNodeIndex_Invalid)
			m_resultPerDestinations[destIdx] = MULTIDESTINATIONPATHFINDER_DONE_PATH_NOT_FOUND;
		else
			m_currentProcessedDestIdx = destIdx;
	}

	if (m_currentProcessedDestIdx == KyUInt32MAXVAL) // no path found at all
		return false;
	return true;
}

template<class TLogic>
bool MultiDestinationPathFinderQuery<TLogic>::ProcessNextDestination(WorkingMemory* workingMemory)
{
	m_currentRawPath = KY_NULL;

	m_resultPerDestinations[m_currentProcessedDestIdx] = MULTIDESTINATIONPATHFINDER_DONE_PATH_FOUND;
	if (GetFinalOutputMode() == MULTIDESTINATIONPATHFINDER_BUILD_LOWEST_COST_PATH_ONLY)
		return false; // no other destination to process

	MultiDestinationPathFinderQueryResult result = MULTIDESTINATIONPATHFINDER_DONE_SOME_PATHS_FOUND;
	while ((m_currentProcessedDestIdx < m_destinations.GetCount() -1) &&
		(result == MULTIDESTINATIONPATHFINDER_DONE_SOME_PATHS_FOUND))
	{
		++m_currentProcessedDestIdx;
		switch (m_resultPerDestinations[m_currentProcessedDestIdx])
		{
			case MULTIDESTINATIONPATHFINDER_TOPROCESS_FROM_CANGOQUERY:
				{
					result = MULTIDESTINATIONPATHFINDER_PROCESSING_CANGOQUERY; // stop the loop
					break; // break the switch
				}
			case MULTIDESTINATIONPATHFINDER_NOT_PROCESSED:
				{
					result = MULTIDESTINATIONPATHFINDER_PROCESSING_ABSTRACT_PATH; // stop the loop
					break; // break the switch
				}
			default:
				break; // break the switch
		}
	}

	if (result == MULTIDESTINATIONPATHFINDER_DONE_SOME_PATHS_FOUND)
		return false; // no other destination to process


	m_resultPerDestinations[m_currentProcessedDestIdx] = MULTIDESTINATIONPATHFINDER_PROCESSING;
	workingMemory->m_refinerContext->ReleaseWorkingMemory();
	workingMemory->m_clamperContext->ReleaseWorkingMemory();
	workingMemory->m_navDataChangeIndexInGrid->ReleaseWorkingMemoryBuffer();
	SetResult(result);
	return true;
}


////////////////////////////////////////////////////////////////////
/// AStarTraversalCustomizer
///////////////////////////////////////////////////////////////////

template<class TLogic>
KY_INLINE MultiDestinationPathFinderQuery<TLogic>::TraversalCustomizer::TraversalCustomizer() : m_query(KY_NULL) {}
template<class TLogic>
KY_INLINE bool MultiDestinationPathFinderQuery<TLogic>::TraversalCustomizer::CanTraverse(const NavTriangleRawPtr& triangleRawPtr, KyFloat32* costMultiplier)
{
	return triangleRawPtr.CanBeTraversed<TLogic>(m_traverseLogicUserData, costMultiplier);
}
template<class TLogic>
KY_INLINE bool MultiDestinationPathFinderQuery<TLogic>::TraversalCustomizer::CanTraverse(const NavGraphEdgeRawPtr& edgeRawPtr, KyFloat32* costMultiplier)
{
	return edgeRawPtr.CanBeTraversed<TLogic>(m_traverseLogicUserData, costMultiplier);
}

template<class TLogic>
KY_INLINE bool MultiDestinationPathFinderQuery<TLogic>::TraversalCustomizer::CanEnterNavTag(const NavTag& exitNavTag, const NavTag& enterNavTag, const Vec3f& pos)
{
	return TLogic::CanEnterNavTag(m_traverseLogicUserData, exitNavTag, enterNavTag, pos);
}

template<class TLogic>
KY_INLINE KyFloat32 MultiDestinationPathFinderQuery<TLogic>::TraversalCustomizer::GetHeuristicFromNodePosition(const Vec3f& /*nodePosition*/)
{
	//return TLogic::GetHeuristicFromDistanceToDest(m_traverseLogicUserData, Distance(nodePosition, m_query->m_destInsidePos3f));
	return 0.f;
}

template<class TLogic>
KY_INLINE bool MultiDestinationPathFinderQuery<TLogic>::TraversalCustomizer::ShouldVisitNode(AStarTraversalContext* astarContext, AStarNodeIndex nodeIndex)
{
	if (AStarNodeIndex_FirstDestNode <= nodeIndex && nodeIndex < (AStarNodeIndex_FirstDestNode + m_query->GetDestCount()))
	{
		bool allDestNodesFound = true;
		for (KyUInt32 destIdx = 0; destIdx < m_query->GetDestCount(); ++destIdx)
		{
			// if node status already known, continue
			if (m_query->m_resultPerDestinations[destIdx] != MULTIDESTINATIONPATHFINDER_NOT_PROCESSED)
				continue;
			
			// otherwise, check if dest node has a predecessor
			AStarNodeIndex currentDestAStarNodeIndex = AStarNodeIndex_FirstDestNode + destIdx;
			AStarNode& astarNode = astarContext->m_aStarNodes[currentDestAStarNodeIndex];
			if (astarNode.GetIndexOfPredecessor() == AStarNodeIndex_Invalid)
			{
				allDestNodesFound = false;
				break;
			}
		}
		
		if (allDestNodesFound == true)
		{
			// empty the binary heap
			astarContext->m_traversalBinHeap.Clear();
			return false;
		}
	}
	
	return true;
}

template<class TLogic>
KY_INLINE bool MultiDestinationPathFinderQuery<TLogic>::TraversalCustomizer::IsNodeOutsidePropagationBounds(AStarTraversalContext* astarContext, AStarNodeIndex nodeIndex)
{
	if (m_query->m_propagationCellFilter == KY_NULL) // no filter
		return false;

	AStarNode& astarNode = astarContext->m_aStarNodes[nodeIndex];
	PathNodeType pathNodeType = astarNode.GetNodeType();
	if (pathNodeType == NodeType_NavMeshEdge || pathNodeType == NodeType_FreePositionOnNavMesh)
	{
		KyUInt32 rawPtrIdx = astarNode.GetIdxOfRawPtrData();
		NavHalfEdgeRawPtr& halfEdge = astarContext->m_edgeRawPtrNodes[rawPtrIdx];
		const CellPos& cellPos = halfEdge.GetCellPos();
		if (m_query->m_propagationCellFilter->IsInFilter(cellPos) == false)
			return true;
	}
// 	else if (pathNodeType == NodeType_NavGraphVertex || pathNodeType == NodeType_FreePositionOnGraphEdge)
// 	{
// 		const DatabaseGenMetrics& genMetrics = m_astarQuery->m_database->GetDatabaseGenMetrics();
// 		CellPos cellPos = genMetrics.ComputeCellPos(astarNode.m_nodePosition);
// 		return (m_astarQuery->m_propagationCellFilter->IsInFilter(cellPos) == false);
// 	}
	return false;
}

template<class TLogic>
KY_INLINE bool MultiDestinationPathFinderQuery<TLogic>::TraversalCustomizer::ShouldOpenGraphVertexNode(const NavGraphVertexRawPtr& /*navGraphVertexRawPtr*/) { return true; }
template<class TLogic>
KY_INLINE bool MultiDestinationPathFinderQuery<TLogic>::TraversalCustomizer::ShouldOpenAbstractGraphNode(const AbstractGraphNodeRawPtr& /*abstractGraphNodeRawPtr*/) { return true; }
template<class TLogic>
KY_INLINE bool MultiDestinationPathFinderQuery<TLogic>::TraversalCustomizer::ShouldOpenNavTriangleUnderAbstractGraphNode(const AbstractGraphNodeRawPtr& abstractGraphNodeRawPtr)
{
	// Check if connected to destination NavMesh
	for (KyUInt32 destIdx = 0; destIdx < m_query->m_destinations.GetCount(); ++destIdx)
	{
		PathFinderPoint& dest = m_query->m_destinations[destIdx];
		CellPos cellPos = dest.m_trianglePtr.GetCellPos();
		AbstractGraph* destAbstractGraph = m_query->m_database->m_abstractGraphCellGrid->GetAbstractGraph(cellPos);
		if (abstractGraphNodeRawPtr.m_abstractGraph == destAbstractGraph)
			return true;
	}

	return false;
}

template<class TLogic>
inline Vec3f MultiDestinationPathFinderQuery<TLogic>::TraversalCustomizer::ComputeNodePositionOnHalfEdge(AStarTraversalContext* aStarTraversalContext,
	const Vec3f& startPosOfEdge, const Vec3f& endPosOfEdge, AStarNodeIndex predecessorNodeIndex)
{
	WorkingMemArray<AStarNode>& aStarNodes = aStarTraversalContext->m_aStarNodes;
	AStarNode& predecessorNode = aStarNodes[predecessorNodeIndex];
	Vec3f nodePos, grandPredecessorNodePos;
	const Vec3f edge(endPosOfEdge - startPosOfEdge);
	Vec3f reducedStartPosOfEdge;
	Vec3f reducedEndPosOfEdge;
	const KyFloat32 edgeSquareLength = edge.GetSquareLength();
	// Do not move more than 30cm from the wall, so the limit if 10% = 30cm -> 3m
	const KyFloat32 maxDistFromWall = 0.3f;
	const KyFloat32 maxSquareDistEdge = maxDistFromWall * maxDistFromWall * 10.f * 10.f;
	if (edgeSquareLength > maxSquareDistEdge)
	{
		const KyFloat32 length = sqrtf(edgeSquareLength);
		const Vec3f dir = edge / length;
		reducedStartPosOfEdge = startPosOfEdge + dir * maxDistFromWall;
		reducedEndPosOfEdge   =   endPosOfEdge - dir * maxDistFromWall;
	}
	else
	{
		reducedStartPosOfEdge = startPosOfEdge + edge * 0.1f; // 10% of margin to not be as far enough from walls
		reducedEndPosOfEdge   =   endPosOfEdge - edge * 0.1f;
	}

	bool canTryToGoStraigthForward = predecessorNodeIndex != AStarNodeIndex_StartNode && predecessorNode.GetNodeType() != NodeType_NavGraphVertex;

	if (canTryToGoStraigthForward)
		grandPredecessorNodePos = aStarNodes[predecessorNode.GetIndexOfPredecessor()].m_nodePosition;

	if (canTryToGoStraigthForward &&
		Intersections::SegmentVsSegment2d(reducedStartPosOfEdge, reducedEndPosOfEdge, grandPredecessorNodePos,
		grandPredecessorNodePos + (predecessorNode.m_nodePosition - grandPredecessorNodePos) * 1000.f, nodePos))
	{
		// previous node is not a GraphVertexNode and we can keep on going straight forward
	}
	else
	{
		nodePos = (endPosOfEdge + startPosOfEdge) * 0.5f;
	}

	return nodePos;
}

template<class TLogic>
inline KyResult MultiDestinationPathFinderQuery<TLogic>::TraversalCustomizer::OnNavTriangleExplored(AStarTraversalContext* aStarTraversalContext, const NavTriangleRawPtr& navTriangleRawPtr, AStarNodeIndex currentNodeIndex)
{
	for (KyUInt32 destIdx = 0; destIdx < m_query->m_destinations.GetCount(); ++destIdx)
	{
		PathFinderPoint& dest = m_query->m_destinations[destIdx];
		AStarNodeIndex currentDestAStarNodeIndex = AStarNodeIndex_FirstDestNode + destIdx;
		NavTriangleRawPtr destTriangleRawPtr = dest.m_trianglePtr.GetRawPtr();
		if (navTriangleRawPtr != destTriangleRawPtr)
			continue;

		WorkingMemArray<AStarNode>& aStarNodes = aStarTraversalContext->m_aStarNodes;
		AstarTraversalBinHeap& binaryHeap = aStarTraversalContext->m_traversalBinHeap;

		// check if we do not have to update the DestNode
		AStarNode& destNode = aStarNodes[currentDestAStarNodeIndex];
		KyFloat32 costMultiplier = 1.f;
		if(CanTraverse(destTriangleRawPtr, &costMultiplier) == false)
			return KY_SUCCESS;

		KY_LOG_ERROR_IF(costMultiplier < 0.f, ("negative cost are not supported"));

		const KyFloat32 cost = Distance(aStarNodes[currentNodeIndex].m_nodePosition, destNode.m_nodePosition) * costMultiplier;
		if (destNode.m_indexInBinaryHeap != IndexInBinHeap_UnSet)
		{
			// destNode is already opened ->  we update it
			KY_FORWARD_ERROR_NO_LOG(m_query->m_traversal.UpdateOpenedOrClosedNode(currentDestAStarNodeIndex, currentNodeIndex, cost));
		}
		else
		{
			// check for memory to insert a newElement in the binary heap
			KY_FORWARD_ERROR_NO_LOG(aStarTraversalContext->CheckTraversalBinaryHeapMemory());

			// dest node has not been open yet. We add it in the Binary Heap
			destNode.m_costFromStart = aStarNodes[currentNodeIndex].m_costFromStart + cost; // so far the dest is bound to be in the navMesh
			destNode.SetIndexOfPredecessor(currentNodeIndex);

			binaryHeap.Insert(currentDestAStarNodeIndex);
		}
	}

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult MultiDestinationPathFinderQuery<TLogic>::TraversalCustomizer::OnNavGraphEdgeExplored(AStarTraversalContext* aStarTraversalContext, const NavGraphEdgeRawPtr& navGraphEdgeRawPtr, AStarNodeIndex currentNodeIndex)
{
	KY_ASSERT(navGraphEdgeRawPtr.IsValid());

	for (KyUInt32 destIdx = 0; destIdx < m_query->m_destinations.GetCount(); ++destIdx)
	{
		PathFinderPoint& dest = m_query->m_destinations[destIdx];
		AStarNodeIndex currentDestAStarNodeIndex = AStarNodeIndex_FirstDestNode + destIdx;
		for(KyUInt32 i=0; i<2; ++i)
		{
			if (navGraphEdgeRawPtr != dest.m_navGraphEdgePtr[i].GetRawPtr())
				continue;
			
			WorkingMemArray<AStarNode>& aStarNodes = aStarTraversalContext->m_aStarNodes;
			AstarTraversalBinHeap& binaryHeap = aStarTraversalContext->m_traversalBinHeap;

			// check if we do not have to update the DestNode
			AStarNode& destNode = aStarNodes[currentDestAStarNodeIndex];
			KY_ASSERT(navGraphEdgeRawPtr.GetStartNavGraphVertexPosition() == aStarNodes[currentNodeIndex].m_nodePosition ||
				currentNodeIndex == AStarNodeIndex_StartNode || currentNodeIndex == currentDestAStarNodeIndex);

			KyFloat32 costMultiplier = 1.f;
			if(CanTraverse(navGraphEdgeRawPtr, &costMultiplier) == false)
				continue;

			const KyFloat32 cost = Distance(aStarNodes[currentNodeIndex].m_nodePosition, destNode.m_nodePosition) * costMultiplier;

			if (destNode.m_indexInBinaryHeap != IndexInBinHeap_UnSet)
			{
				// destNode is already opened ->  we update it
				KY_FORWARD_ERROR_NO_LOG(m_query->m_traversal.UpdateOpenedOrClosedNode(currentDestAStarNodeIndex, currentNodeIndex, cost));
			}
			else
			{
				// check for memory to insert a newElement in the binary heap
				KY_FORWARD_ERROR_NO_LOG(aStarTraversalContext->CheckTraversalBinaryHeapMemory());

				// dest node has not been open yet. We add it in the Byndary Heap
				destNode.m_costFromStart = aStarNodes[currentNodeIndex].m_costFromStart + cost;
				destNode.SetIndexOfPredecessor(currentNodeIndex);
				destNode.SetIdxOfRawPtrData(aStarTraversalContext->m_vertexRawPtrNodes.GetCount());
				aStarTraversalContext->m_vertexRawPtrNodes.PushBack(navGraphEdgeRawPtr.GetEndNavGraphVertexRawPtr());

				binaryHeap.Insert(currentDestAStarNodeIndex);
			}
		}
	}


	return KY_SUCCESS;
}

template<class TLogic>
inline bool MultiDestinationPathFinderQuery<TLogic>::TraversalCustomizer::ShouldOpenHalfEdgeNode(const NavHalfEdgeRawPtr& /*halfEdge*/, NavHalfEdgeRawPtr& pairHalfEdgeOfNewNode)
{
	const NavFloor* navFloor = pairHalfEdgeOfNewNode.GetNavFloor();
	const NavFloorBlob* navFloorBlob = navFloor->GetNavFloorBlob();
	const NavTriangleIdx triangleIdx = NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(pairHalfEdgeOfNewNode.GetHalfEdgeIdx());

	// Check whether crossing the edge leads to a dead end
	if (navFloor->GetNavFloorToNavGraphLinks()->IsTriangleLinkedToGraph(triangleIdx) == false)
	{
		const NavHalfEdgeIdx nextIdx =  NavFloorBlob::NavHalfEdgeIdxToNextNavHalfEdgeIdx(pairHalfEdgeOfNewNode.GetHalfEdgeIdx());
		const NavHalfEdgeIdx prevtIdx = NavFloorBlob::NavHalfEdgeIdxToPrevNavHalfEdgeIdx(pairHalfEdgeOfNewNode.GetHalfEdgeIdx());

		const NavHalfEdge* const halfEdges = navFloorBlob->m_navHalfEdges.GetValues();

		const NavTriangleRawPtr triangleRawPtr(pairHalfEdgeOfNewNode.m_navFloorRawPtr, NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(pairHalfEdgeOfNewNode.GetHalfEdgeIdx()));

		if (halfEdges[nextIdx].GetHalfEdgeType() == EDGETYPE_OBSTACLE && halfEdges[prevtIdx].GetHalfEdgeType() == EDGETYPE_OBSTACLE)
		{
			for (KyUInt32 destIdx = 0; destIdx < m_query->m_destinations.GetCount(); ++destIdx)
			{
				PathFinderPoint& dest = m_query->m_destinations[destIdx];
				if (triangleRawPtr == dest.m_trianglePtr.GetRawPtr())
					return true;
			}
			return false; // edge crossable but leading to a dead end, do not create a node
		}
	}

	return true;
}

}

