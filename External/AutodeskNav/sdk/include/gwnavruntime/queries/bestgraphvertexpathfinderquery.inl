/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY

// TODO: put all non template functions in a base class 
// to avoid KY_TEMPLATE_BGVPFQ everywhere


namespace Kaim
{

// ugly short names but ohterwise the inl is very hard to read
#define KY_TEMPLATE_BGVPFQ template<class VertexFinderLogic>
#define KY_FUNC_BGVPFQ BestGraphVertexPathFinderQuery<VertexFinderLogic>

KY_TEMPLATE_BGVPFQ
KY_FUNC_BGVPFQ::BestGraphVertexPathFinderQuery()
	: BaseBestGraphVertexPathFinderQuery()
	, m_traversal(TraversalCustomizer())
{
	m_traversal.m_traversalParams = &m_traversalParams;
}

KY_TEMPLATE_BGVPFQ
KY_FUNC_BGVPFQ::~BestGraphVertexPathFinderQuery() {}

KY_TEMPLATE_BGVPFQ
void KY_FUNC_BGVPFQ::BindToDatabase(Database* database)
{
	BaseBestGraphVertexPathFinderQuery::BindToDatabase(database);
}

KY_TEMPLATE_BGVPFQ
void KY_FUNC_BGVPFQ::Initialize(const Vec3f& startPos)
{
	BaseBestGraphVertexPathFinderQuery::Initialize(startPos);
}

KY_TEMPLATE_BGVPFQ
KY_INLINE void KY_FUNC_BGVPFQ::SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr)
{
	BaseBestGraphVertexPathFinderQuery::SetStartTrianglePtr(startTrianglePtr);
}
KY_TEMPLATE_BGVPFQ
KY_INLINE void KY_FUNC_BGVPFQ::SetStartNavGraphEdgePtr(const NavGraphEdgePtr& startNavGraphEdgePtr, NavGraphEdgeDirection navGraphEdgePathfindMode)
{
	BaseBestGraphVertexPathFinderQuery::SetStartNavGraphEdgePtr(startNavGraphEdgePtr, navGraphEdgePathfindMode);
}
KY_TEMPLATE_BGVPFQ
KY_INLINE void KY_FUNC_BGVPFQ::SetStartNavGraphVertexPtr(const NavGraphVertexPtr& startNavGraphVertexPtr)
{
	BaseBestGraphVertexPathFinderQuery::SetStartNavGraphVertexPtr(startNavGraphVertexPtr);
}
KY_TEMPLATE_BGVPFQ
KY_INLINE void KY_FUNC_BGVPFQ::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	BaseBestGraphVertexPathFinderQuery::SetPositionSpatializationRange(positionSpatializationRange);
}
KY_TEMPLATE_BGVPFQ
KY_INLINE void KY_FUNC_BGVPFQ::SetFromOutsideNavMeshDistance(KyFloat32 fromOutsideNavMeshDistance)
{
	BaseBestGraphVertexPathFinderQuery::SetFromOutsideNavMeshDistance(fromOutsideNavMeshDistance);
}
KY_TEMPLATE_BGVPFQ
KY_INLINE void KY_FUNC_BGVPFQ::SetPropagationRadius(KyFloat32 propagationRadius)
{
	BaseBestGraphVertexPathFinderQuery::SetPropagationRadius(propagationRadius);
}

KY_TEMPLATE_BGVPFQ
KY_INLINE void KY_FUNC_BGVPFQ::SetComputeChannelMode(PathFinderQueryComputeChannelMode computeChannelMode)
{
	BaseBestGraphVertexPathFinderQuery::SetComputeChannelMode(computeChannelMode);
}

KY_TEMPLATE_BGVPFQ
KY_INLINE void KY_FUNC_BGVPFQ::SetChannelComputerConfig(const ChannelComputerConfig& channelComputerConfig)
{
	BaseBestGraphVertexPathFinderQuery::SetChannelComputerConfig(channelComputerConfig);
}

KY_TEMPLATE_BGVPFQ
KY_INLINE void KY_FUNC_BGVPFQ::SetNumberOfProcessedNodePerFrame(KyUInt32 numberOfProcessedNodePerFrame)
{
	BaseBestGraphVertexPathFinderQuery::SetNumberOfProcessedNodePerFrame(numberOfProcessedNodePerFrame);
}

KY_TEMPLATE_BGVPFQ
KY_INLINE void KY_FUNC_BGVPFQ::SetResult(BestGraphVertexPathFinderQueryResult result) { m_result = result; }

KY_TEMPLATE_BGVPFQ
KY_INLINE const NavTrianglePtr& KY_FUNC_BGVPFQ::GetStartTrianglePtr() const
{
	return BaseBestGraphVertexPathFinderQuery::GetStartTrianglePtr();
}
KY_TEMPLATE_BGVPFQ
KY_INLINE const NavGraphEdgePtr& KY_FUNC_BGVPFQ::GetStartNavGraphEdgePtr() const
{
	return BaseBestGraphVertexPathFinderQuery::GetStartNavGraphEdgePtr();
}
KY_TEMPLATE_BGVPFQ
KY_INLINE const NavGraphVertexPtr& KY_FUNC_BGVPFQ::GetStartNavGraphVertexPtr() const
{
	return BaseBestGraphVertexPathFinderQuery::GetStartNavGraphVertexPtr();
}

KY_TEMPLATE_BGVPFQ
KY_INLINE BestGraphVertexPathFinderQueryResult KY_FUNC_BGVPFQ::GetResult() const
{
	return BaseBestGraphVertexPathFinderQuery::GetResult();
}
KY_TEMPLATE_BGVPFQ
KY_INLINE Path* KY_FUNC_BGVPFQ::GetPath() const
{
	return BaseBestGraphVertexPathFinderQuery::GetPath();
}
KY_TEMPLATE_BGVPFQ
KY_INLINE const Vec3f& KY_FUNC_BGVPFQ::GetStartPos() const
{
	return BaseBestGraphVertexPathFinderQuery::GetStartPos();
}

KY_TEMPLATE_BGVPFQ
KY_INLINE KyFloat32 KY_FUNC_BGVPFQ::GetFromOutsideNavMeshDistance() const
{
	return BaseBestGraphVertexPathFinderQuery::GetFromOutsideNavMeshDistance();
}
KY_TEMPLATE_BGVPFQ
KY_INLINE KyFloat32 KY_FUNC_BGVPFQ::GetPropagationRadius() const
{
	return BaseBestGraphVertexPathFinderQuery::GetPropagationRadius();
}

KY_TEMPLATE_BGVPFQ
KY_INLINE KyUInt32 KY_FUNC_BGVPFQ::GetNumberOfProcessedNodePerFrame() const
{
	return BaseBestGraphVertexPathFinderQuery::GetNumberOfProcessedNodePerFrame();
}
KY_TEMPLATE_BGVPFQ
KY_INLINE const PositionSpatializationRange& KY_FUNC_BGVPFQ::GetPositionSpatializationRange() const
{
	return BaseBestGraphVertexPathFinderQuery::GetPositionSpatializationRange();
}

KY_TEMPLATE_BGVPFQ
KY_INLINE void KY_FUNC_BGVPFQ::ComputePathRefinerConfigInternals()
{
	BaseBestGraphVertexPathFinderQuery::ComputePathRefinerConfigInternals();
}
KY_TEMPLATE_BGVPFQ
KY_INLINE const PathRefinerConfig& KY_FUNC_BGVPFQ::GetPathRefinerConfig() const
{
	return BaseBestGraphVertexPathFinderQuery::GetPathRefinerConfig();
}


KY_TEMPLATE_BGVPFQ
inline void KY_FUNC_BGVPFQ::ReleaseWorkingMemoryOnCancelDuringProcess(WorkingMemory* workingMemory)
{
	if (workingMemory == KY_NULL)
		workingMemory = m_database->GetWorkingMemory();

	SetResult(BESTGRAPHVERTEX_DONE_COMPUTATION_CANCELED);
	SetFinish(workingMemory);
}


KY_TEMPLATE_BGVPFQ
inline void KY_FUNC_BGVPFQ::Advance(WorkingMemory* workingMemory)
{
	ScopedSetPathNavigationProfile scopedSetPathNavigationProfile(this);

	++m_advanceCount;
	m_processStatus = QueryInProcess;

	PathFinderQueryUtils queryUtils(m_database, workingMemory, GetTraverseLogicUserData());
	queryUtils.SetPerformQueryStat(QueryStat_PathFinder);

	switch (GetResult())
	{
	case BESTGRAPHVERTEX_NOT_PROCESSED :
		{
			m_traversal.m_traversalCustomizer.m_traverseLogicUserData = GetTraverseLogicUserData();
			m_lastAdvanceFrameIdx = m_database->m_frameIdx;

			if (m_database->IsClear())
			{
				SetResult(BESTGRAPHVERTEX_DONE_START_OUTSIDE);
				SetFinish(workingMemory);
				return;
			}

			m_traversal.m_astarContext = workingMemory->GetOrCreateAStarTraversalContext();
			if (KY_FAILED(ComputeStartGraphEdgeOrGraphVertexOrTriangle(queryUtils)))
			{
				SetFinish(workingMemory);
				return;
			}

			m_traversal.m_traversalCustomizer.m_startPos = m_startPos3f;
			m_traversal.m_traversalCustomizer.m_maxRadius = m_propagationRadius;
			if (KY_FAILED(InitializeContextAndStartNode(queryUtils)))
			{
				SetResult(BESTGRAPHVERTEX_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			// init the propagation with the neighbors of the startNode
			if (KY_FAILED(InitTraversalFromStartNode(queryUtils)))
			{
				SetResult(BESTGRAPHVERTEX_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			if (m_traversal.IsThereNodeToVisit() == false)
			{
				SetResult(BESTGRAPHVERTEX_DONE_PATH_NOT_FOUND);
				SetFinish(workingMemory);
				return;
			}

			SetResult(BESTGRAPHVERTEX_PROCESSING_TRAVERSAL);

			break;
		}
	case BESTGRAPHVERTEX_PROCESSING_TRAVERSAL :
		{
			if(m_lastAdvanceFrameIdx != m_database->m_frameIdx)
			{
				if(m_traversal.m_astarContext->m_edgeIndexGrid.HasNavDataChanged(m_database))
				{
					SetResult(BESTGRAPHVERTEX_DONE_NAVDATA_CHANGED);
					SetFinish(workingMemory);
					return;
				}

				m_lastAdvanceFrameIdx = m_database->m_frameIdx;
			}

			TraversalVisitNodeContext visitNodeContext;
			for(KyUInt32 numberOfNode = 0; numberOfNode < m_traversalParams.m_numberOfVisitedNodePerFrame; ++numberOfNode)
			{
				if (KY_FAILED(m_traversal.VisitNode(queryUtils, visitNodeContext)))
				{
					SetResult(BESTGRAPHVERTEX_DONE_ERROR_LACK_OF_WORKING_MEMORY);
					SetFinish(workingMemory);
					return;
				}

				// note that we test also if (visitNodeContext.m_visitedGraphVertexRawPtr != m_startNavGraphVertexPtr)
				// because we do not want to create an empty path
				if (visitNodeContext.m_visitedGraphVertexRawPtr.IsValid() &&
					visitNodeContext.m_visitedGraphVertexRawPtr != m_startNavGraphVertexPtr.GetRawPtr())
				{
					// We visited a graphVertex, ask the customizer what to do !
					OnGraphVertexVisitedResult result = VertexFinderLogic::OnGraphVertexVisited(GetTraverseLogicUserData(),
						visitNodeContext.m_visitedGraphVertexRawPtr, m_bestVertex, m_rememberBestPathSoFar);
					switch(result)
					{
					case OnGraphVertexVisited_IgnoreVertex_GoOn :
						{
							break;
						}
					case OnGraphVertexVisited_IgnoreVertex_StopWithBestSoFar :
						{
							m_traversal.m_astarContext->m_traversalBinHeap.Clear(); // make sure the propagation is stopped
							if (m_bestVertex.IsValid() == false)
							{
								// no good enough vertex has been found so far...
								SetResult(BESTGRAPHVERTEX_DONE_PATH_NOT_FOUND);
								SetFinish(workingMemory);
								return;
							}

							if (m_rememberBestPathSoFar != KY_NULL)
							{
								// a good enough vertex has been found and the path is already computed
								// stop here roe compute channels for the final path!
								if (m_computeChannelMode == PATHFINDER_DO_NOT_COMPUTE_CHANNELS)
								{
									SetPath(m_rememberBestPathSoFar);
									SetResult(BESTGRAPHVERTEX_DONE_PATH_FOUND);
									SetFinish(workingMemory);
									return;
								}

								SetResult(BESTGRAPHVERTEX_PROCESSING_CHANNEL_INIT);
								return;
							}

							m_currentVertex.Invalidate(); // make sure we refine the path
							m_currentVertexNodeIdx = AStarNodeIndex_Invalid; 
							SetResult(BESTGRAPHVERTEX_PROCESSING_TRAVERSAL_DONE);
							return;
						}
					case OnGraphVertexVisited_VertexIsTheBestSoFar_GoOn :
						{
							m_rememberBestPathSoFar = KY_NULL;
							m_bestVertex        = visitNodeContext.m_visitedGraphVertexRawPtr;
							m_bestVertexNodeIdx = visitNodeContext.m_visitedNodeIndex;
							break;
						}
					case OnGraphVertexVisited_StopOnThisVertex :
						{
							m_traversal.m_astarContext->m_traversalBinHeap.Clear(); // make sure the propagation is stopped

							m_rememberBestPathSoFar = KY_NULL;
							m_bestVertex        = visitNodeContext.m_visitedGraphVertexRawPtr;
							m_bestVertexNodeIdx = visitNodeContext.m_visitedNodeIndex;
							m_currentVertex.Invalidate(); // make sure we refine the path
							m_currentVertexNodeIdx = AStarNodeIndex_Invalid;

							SetResult(BESTGRAPHVERTEX_PROCESSING_TRAVERSAL_DONE);
							return;
						}
					case OnGraphVertexVisited_NeedToCheckRefinePathToThisVertex :
						{
							SetResult(BESTGRAPHVERTEX_PROCESSING_TRAVERSAL_DONE);
							m_currentVertex = visitNodeContext.m_visitedGraphVertexRawPtr;
							m_currentVertexNodeIdx = visitNodeContext.m_visitedNodeIndex;
							return;
						}
					default:
						break;
					}
				}

				if (m_traversal.IsThereNodeToVisit() == false)
					break;
			}

			if (m_traversal.IsThereNodeToVisit() == false)
			{
				// PROPAGATION IS DONE !

				// Visual debugging
				// Uncomment this line to send the result of the propagation via the visual debugging
				//queryUtils.DisplayListPropagation("BestGraphVertexPathFinderQuery");

				if (m_bestVertex.IsValid() == false)
				{
					// no good enough vertex has been found so far...
					SetResult(BESTGRAPHVERTEX_DONE_PATH_NOT_FOUND);
					SetFinish(workingMemory);
					return;
				}

				if (m_rememberBestPathSoFar != KY_NULL)
				{
					// a good enough vertex has been found and the path is already computed
					// stop here or compute channels for the final path!
					if (m_computeChannelMode == PATHFINDER_DO_NOT_COMPUTE_CHANNELS)
					{
						SetPath(m_rememberBestPathSoFar);
						SetResult(BESTGRAPHVERTEX_DONE_PATH_FOUND);
						SetFinish(workingMemory);
						return;
					}

					SetResult(BESTGRAPHVERTEX_PROCESSING_CHANNEL_INIT);
					return;
				}

				// the path has not been computed, compute it !
				SetResult(BESTGRAPHVERTEX_PROCESSING_TRAVERSAL_DONE);
			}

			break;
		}

	case BESTGRAPHVERTEX_PROCESSING_TRAVERSAL_DONE :
		{
			if(m_lastAdvanceFrameIdx != m_database->m_frameIdx)
			{
				if(m_traversal.m_astarContext->m_edgeIndexGrid.HasNavDataChanged(m_database))
				{
					SetResult(BESTGRAPHVERTEX_DONE_NAVDATA_CHANGED);
					SetFinish(workingMemory);
					return;
				}

				m_lastAdvanceFrameIdx = m_database->m_frameIdx;
			}

			bool m_canClearAndReleaseTraversalMemory = true;
			AStarNodeIndex currentDestNodeIndex = m_bestVertexNodeIdx;

			if (m_currentVertex.IsValid())
			{
				//  this is an intermediate path computation to be able to choose vertices according to refined Path
				// -> The traversal is very likely to resume, therfore we must not clear the traversal context
				currentDestNodeIndex = m_currentVertexNodeIdx;
				m_canClearAndReleaseTraversalMemory = false;
			}

			KY_ASSERT(currentDestNodeIndex != AStarNodeIndex_Invalid);

			PathRefinerContext* refinerContext = workingMemory->GetOrCreatePathRefinerContext();
			workingMemory->m_navDataChangeIndexInGrid->ReleaseWorkingMemoryBuffer();
			if (KY_FAILED(refinerContext->InitFromAstarTraversalContext(workingMemory, workingMemory->m_astarContext,
				currentDestNodeIndex, m_canClearAndReleaseTraversalMemory)))
			{
				SetResult(BESTGRAPHVERTEX_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			SetResult(BESTGRAPHVERTEX_PROCESSING_REFINING_INIT);
			break;
		}

	case BESTGRAPHVERTEX_PROCESSING_REFINING_INIT :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			ComputePathRefinerConfigInternals();

			if (KY_FAILED(queryUtils.BuildRefinerBinaryHeap()))
			{
				SetResult(BESTGRAPHVERTEX_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			PathRefinerContext* refinerContext = workingMemory->GetOrCreatePathRefinerContext();
			if (refinerContext->m_currentNodeIdx == refinerContext->m_refinerNodes.GetCount())
			{
				if (refinerContext->m_refinerBinHeap.IsEmpty())
					SetResult(BESTGRAPHVERTEX_PROCESSING_PATHCLAMPING_INIT);
				else
					SetResult(BESTGRAPHVERTEX_PROCESSING_REFINING);
			}

			break;
		}
	case BESTGRAPHVERTEX_PROCESSING_REFINING :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			PathRefinerContext* refinerContext = workingMemory->GetOrCreatePathRefinerContext();
			for(KyUInt32 cangoTestCount = 0; cangoTestCount < m_traversalParams.m_numberOfCanGoTestInRefinerPerFrame;)
			{
				if (KY_FAILED(queryUtils.RefineOneNode<VertexFinderLogic>(GetPositionSpatializationRange(), GetPathRefinerConfig(), cangoTestCount)))
				{
					SetResult(BESTGRAPHVERTEX_DONE_ERROR_LACK_OF_WORKING_MEMORY);
					SetFinish(workingMemory);
					return;
				}

				if (refinerContext->m_refinerBinHeap.IsEmpty())
					break;
			}

			if (refinerContext->m_refinerBinHeap.IsEmpty())
			{
				// Visual debugging
				// Uncomment this line to send the result of the path refiner, before the path is clamped
				//queryUtils.DisplayListRefining("BestGraphVertexPathFinderQuery");

				SetResult(BESTGRAPHVERTEX_PROCESSING_PATHCLAMPING_INIT);
			}

			break;
		}
	case BESTGRAPHVERTEX_PROCESSING_PATHCLAMPING_INIT :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			if (KY_FAILED(workingMemory->m_clamperContext->InitFromRefinerContext(workingMemory, workingMemory->GetOrCreatePathRefinerContext())))
			{
				SetResult(BESTGRAPHVERTEX_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			if (m_currentVertex.IsValid() || m_computeChannelMode == PATHFINDER_DO_NOT_COMPUTE_CHANNELS)
				// Do not clamp the raw path if it is the finalt path compute and if a channel is to be computed
				SetResult(BESTGRAPHVERTEX_PROCESSING_PATHCLAMPING);
			else
				SetResult(BESTGRAPHVERTEX_PROCESSING_PATHBUILDING);

			break;
		}
	case BESTGRAPHVERTEX_PROCESSING_PATHCLAMPING :
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
						SetResult(BESTGRAPHVERTEX_DONE_COMPUTATION_ERROR);
					else
						SetResult(BESTGRAPHVERTEX_DONE_ERROR_LACK_OF_WORKING_MEMORY);

					SetFinish(workingMemory);
					return;
				}

				if (pathClamperContext->IsClampingDone())
					break;
			}

			if (pathClamperContext->IsClampingDone())
			{
				SetResult(BESTGRAPHVERTEX_PROCESSING_PATHBUILDING);
			}

			break;
		}
	case BESTGRAPHVERTEX_PROCESSING_PATHBUILDING :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			if (m_currentVertex.IsValid() == false)
			{
				// Final path computation
				KY_LOG_ERROR_IF(m_rememberBestPathSoFar != KY_NULL, ("The path for the bestVertex should be null"));
				KY_ASSERT(m_bestVertex.IsValid()); // a best vertex must have been found or we should not have a path to compute...
				PathClamperContext* pathClamperContext  = workingMemory->GetOrCreatePathClamperContext();
				pathClamperContext->m_startNavGraphEdgePtr = m_startNavGraphEdgePtr;
				m_rememberBestPathSoFar = queryUtils.ComputePathFromPathClamperContext<VertexFinderLogic>(m_startPos3f, m_bestVertex.GetGraphVertexPosition(), GetPositionSpatializationRange());

				if (m_computeChannelMode == PATHFINDER_DO_NOT_COMPUTE_CHANNELS)
				{
					SetPath(m_rememberBestPathSoFar);
					SetResult(BESTGRAPHVERTEX_DONE_PATH_FOUND);
					SetFinish(workingMemory);
					return;
				}

				SetResult(BESTGRAPHVERTEX_PROCESSING_CHANNEL_INIT);
				return;
			}
			
			// This an intermediate path computation, for now we do NOT compute channels for intermediate path
			Ptr<Path> rememberBestPath = m_rememberBestPathSoFar;
			PathClamperContext* pathClamperContext  = workingMemory->GetOrCreatePathClamperContext();
			pathClamperContext->m_startNavGraphEdgePtr = m_startNavGraphEdgePtr;

			m_rememberBestPathSoFar = queryUtils.ComputePathFromPathClamperContext<VertexFinderLogic>(
				m_startPos3f, m_currentVertex.GetGraphVertexPosition(), GetPositionSpatializationRange());
				
			OnPathToGraphVertexComputedResult result = VertexFinderLogic::OnPathToGraphVertexComputed(
				GetTraverseLogicUserData(), m_currentVertex, m_rememberBestPathSoFar, m_bestVertex, rememberBestPath);

			switch(result)
			{
			case OnPathToGraphVertexComputed_IgnoreVertex_GoOn :
				{
					m_rememberBestPathSoFar = rememberBestPath;
					m_currentVertex.Invalidate();
					m_currentVertexNodeIdx = AStarNodeIndex_Invalid;

					if (m_traversal.IsThereNodeToVisit() == false)
					{
						// Cannot resume traversal, no more nodes !

						if (m_bestVertex.IsValid() == false)
						{
							// no good enough vertex has been found so far...
							SetResult(BESTGRAPHVERTEX_DONE_PATH_NOT_FOUND);
							SetFinish(workingMemory);
							return;
						}

						if (m_rememberBestPathSoFar != KY_NULL)
						{
							// a good enough vertex has been found and the path is already computed
							// stop here or compute channels for the final path!
							if (m_computeChannelMode == PATHFINDER_DO_NOT_COMPUTE_CHANNELS)
							{
								SetPath(m_rememberBestPathSoFar);
								SetResult(BESTGRAPHVERTEX_DONE_PATH_FOUND);
								SetFinish(workingMemory);
								return;
							}

							SetResult(BESTGRAPHVERTEX_PROCESSING_CHANNEL_INIT);
							return;
						}

						SetResult(BESTGRAPHVERTEX_PROCESSING_TRAVERSAL_DONE);
						return;
					}

					// we can resume the traversal
					workingMemory->m_clamperContext->ReleaseWorkingMemory();

					// Note : we do not call workingMemory->m_navDataChangeIndexInGrid->ReleaseWorkingMemoryBuffer();
					// because we will use it if the current path will be the final Path
					SetResult(BESTGRAPHVERTEX_PROCESSING_TRAVERSAL);
					return;
				}
			case OnPathToGraphVertexComputed_IgnoreVertex_StopWithBestSoFar :
				{
					m_rememberBestPathSoFar = rememberBestPath;
					m_currentVertex.Invalidate();
					m_currentVertexNodeIdx = AStarNodeIndex_Invalid;

					if (m_bestVertex.IsValid() == false)
					{
						// no good enough vertex has been found so far...
						SetResult(BESTGRAPHVERTEX_DONE_PATH_NOT_FOUND);
						SetFinish(workingMemory);
						return;
					}

					if (m_rememberBestPathSoFar != KY_NULL)
					{
						// a good enough vertex has been found and the path is already computed
						// stop here or compute channels for the final path!
						if (m_computeChannelMode == PATHFINDER_DO_NOT_COMPUTE_CHANNELS)
						{
							SetPath(m_rememberBestPathSoFar);
							SetResult(BESTGRAPHVERTEX_DONE_PATH_FOUND);
							SetFinish(workingMemory);
							return;
						}

						SetResult(BESTGRAPHVERTEX_PROCESSING_CHANNEL_INIT);
						return;
					}

					SetResult(BESTGRAPHVERTEX_PROCESSING_TRAVERSAL_DONE);
					return;
				}
			case OnPathToGraphVertexComputed_VertexIsTheBestSoFar_GoOn :
				{
					// let m_rememberBestPathSoFar to the just computed path since it is considered to be the best !
					rememberBestPath = KY_NULL;

					m_bestVertex = m_currentVertex;
					m_bestVertexNodeIdx = m_currentVertexNodeIdx;
					m_currentVertex.Invalidate();
					m_currentVertexNodeIdx = AStarNodeIndex_Invalid;

					if (m_traversal.IsThereNodeToVisit() == false)
					{
						// Cannot resume traversal, no more nodes !

						// a good enough vertex has been found and the path is already computed
						// stop here or compute channels for the final path!
						if (m_computeChannelMode == PATHFINDER_DO_NOT_COMPUTE_CHANNELS)
						{
							SetPath(m_rememberBestPathSoFar);
							SetResult(BESTGRAPHVERTEX_DONE_PATH_FOUND);
							SetFinish(workingMemory);
							return;
						}

						SetResult(BESTGRAPHVERTEX_PROCESSING_CHANNEL_INIT);
						return;
					}

					// Resume the traversal
					workingMemory->m_clamperContext->ReleaseWorkingMemory();

					// Note : we do not call workingMemory->m_navDataChangeIndexInGrid->ReleaseWorkingMemoryBuffer();
					// because we will use it if the current path will be the final Path
					SetResult(BESTGRAPHVERTEX_PROCESSING_TRAVERSAL);
					return;
				}
			case OnPathToGraphVertexComputed_StopOnThisVertex :
				{
					// let m_path to the just computed path since it is considered to be the best !
					rememberBestPath = KY_NULL;

					m_bestVertex = m_currentVertex;
					m_bestVertexNodeIdx = m_currentVertexNodeIdx;
					m_currentVertex.Invalidate();
					m_currentVertexNodeIdx = AStarNodeIndex_Invalid;

					// a good enough vertex has been found and the path is already computed
					// stop here or compute channels for the final path!
					if (m_computeChannelMode == PATHFINDER_DO_NOT_COMPUTE_CHANNELS)
					{
						SetPath(m_rememberBestPathSoFar);
						SetResult(BESTGRAPHVERTEX_DONE_PATH_FOUND);
						SetFinish(workingMemory);
						return;
					}

					SetResult(BESTGRAPHVERTEX_PROCESSING_CHANNEL_INIT);
					return;
				}
			default:
				break;
			}
		}
	case BESTGRAPHVERTEX_PROCESSING_CHANNEL_INIT :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			ChannelArrayComputer& channelArrayComputer = *workingMemory->GetOrCreateChannelArrayComputer();
			channelArrayComputer.Init(m_rememberBestPathSoFar);
			channelArrayComputer.SetTraverseLogicUserData(GetTraverseLogicUserData());
			channelArrayComputer.SetChannelComputerConfig(m_channelComputerConfig);
			SetResult(BESTGRAPHVERTEX_PROCESSING_CHANNEL_COMPUTE);
			break;
		}
	case BESTGRAPHVERTEX_PROCESSING_CHANNEL_COMPUTE :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			ChannelArrayComputer& channelArrayComputer = *workingMemory->GetOrCreateChannelArrayComputer();
			channelArrayComputer.Advance<VertexFinderLogic>(workingMemory);

			if (channelArrayComputer.IsFinished())
			{
				if (channelArrayComputer.m_result != ChannelArrayResult_Done)
				{
					SetResult(BESTGRAPHVERTEX_DONE_CHANNELCOMPUTATION_ERROR);
					SetFinish(workingMemory);
					return;
				}

				SetPath(channelArrayComputer.m_pathWithChannels);
				channelArrayComputer.m_channelArray = KY_NULL;
				channelArrayComputer.m_pathWithChannels = KY_NULL;

				SetResult(BESTGRAPHVERTEX_DONE_PATH_FOUND);
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

KY_TEMPLATE_BGVPFQ
KY_INLINE bool KY_FUNC_BGVPFQ::HasNavDataChanged()
{
	return m_traversal.m_astarContext->m_edgeIndexGrid.HasNavDataChanged(m_database);
}

KY_TEMPLATE_BGVPFQ
inline void KY_FUNC_BGVPFQ::SetFinish(WorkingMemory* workingMemory)
{
	m_processStatus = QueryDone; // TODO - CHANGE THIS!
	m_dataBaseChangeIdx = m_database ? m_database->m_navdataChangeIdx : 0;

	m_bestVertex.Invalidate();
	m_bestVertexNodeIdx = AStarNodeIndex_Invalid;
	m_rememberBestPathSoFar = KY_NULL;

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

KY_TEMPLATE_BGVPFQ
inline KyResult KY_FUNC_BGVPFQ::CheckNavDataChange(WorkingMemory* workingMemory)
{
	if(m_lastAdvanceFrameIdx != m_database->m_frameIdx)
	{
		if(workingMemory->m_navDataChangeIndexInGrid->HasNavDataChanged(m_database))
		{
			SetResult(BESTGRAPHVERTEX_DONE_NAVDATA_CHANGED);
			SetFinish(workingMemory);
			return KY_ERROR;
		}

		m_lastAdvanceFrameIdx = m_database->m_frameIdx;
	}

	return KY_SUCCESS;
}


KY_TEMPLATE_BGVPFQ
inline KyResult KY_FUNC_BGVPFQ::ComputeStartGraphEdgeOrGraphVertexOrTriangle(PathFinderQueryUtils& queryUtils)
{
	if (m_startNavGraphEdgePtr.IsValid())
	{
		m_startInsidePos3f = m_startPos3f;
		return KY_SUCCESS;
	}

	if (m_startNavGraphVertexPtr.IsValid())
	{
		m_startInsidePos3f = m_startNavGraphVertexPtr.GetGraphVertexPosition();
		return KY_SUCCESS;
	}

	return ComputeStartTriangle(queryUtils);
}

KY_TEMPLATE_BGVPFQ
inline KyResult KY_FUNC_BGVPFQ::ComputeStartTriangle(PathFinderQueryUtils& queryUtils)
{
	KY_DEBUG_ASSERTN(queryUtils.m_database->GetActiveData() != KY_NULL, ("the PathFinderQueryUtils need a valid activeData"));

	if (queryUtils.FindTriangleFromPositionIfNotValid(m_startTrianglePtr, m_startPos3f, GetPositionSpatializationRange()) == true)
	{
		if (m_traversal.m_traversalCustomizer.CanTraverse(m_startTrianglePtr.GetRawPtr(), KY_NULL) == true)
		{
			m_startInsidePos3f = m_startPos3f;
			return KY_SUCCESS;
		}
	}

	if (GetFromOutsideNavMeshDistance() > 0.f)
	{
		// the position is outside the NavMesh or in an invalid navtag. Try to hook on the NavMesh
		if (queryUtils.TryToHookOnNavMesh<VertexFinderLogic>(m_startPos3f, GetPositionSpatializationRange(),
			GetFromOutsideNavMeshDistance(), 0.1f /*distFromObstlace*/, m_startInsidePos3f, m_startTrianglePtr) == KY_SUCCESS)
		{
			return KY_SUCCESS;
		}
	}

	if (m_startTrianglePtr.IsValid() == false)
		SetResult(BESTGRAPHVERTEX_DONE_START_OUTSIDE);
	else
		SetResult(BESTGRAPHVERTEX_DONE_START_NAVTAG_FORBIDDEN);

	return KY_ERROR;
}

KY_TEMPLATE_BGVPFQ
inline KyResult KY_FUNC_BGVPFQ::InitializeContextAndStartNode(PathFinderQueryUtils& queryUtils)
{
	if (m_startNavGraphEdgePtr.IsValid())
	{
		KY_LOG_WARNING_IF(m_startTrianglePtr.IsValid(), ("The start NavGraph Edge ptr will override the start Triangle ptr!"));
		KY_LOG_WARNING_IF(m_startNavGraphVertexPtr.IsValid(), ("The start NavGraph Edge ptr will override the start NavGraphEdge ptr!"));

		return m_traversal.InitializeContextAndStartNode(queryUtils, m_startPos3f, m_startNavGraphEdgePtr);
	}

	if (m_startNavGraphVertexPtr.IsValid())
	{
		KY_LOG_WARNING_IF(m_startTrianglePtr.IsValid(), ("The start NavGraph Edge ptr will override the start Triangle ptr!"));

		return m_traversal.InitializeContextAndStartNode(queryUtils, m_startInsidePos3f, m_startNavGraphVertexPtr);
	}

	return m_traversal.InitializeContextAndStartNode(queryUtils, m_startInsidePos3f, m_startTrianglePtr);
}


KY_TEMPLATE_BGVPFQ
inline KyResult KY_FUNC_BGVPFQ::InitTraversalFromStartNode(PathFinderQueryUtils& queryUtils)
{
	ActiveData* activeData = queryUtils.m_database->GetActiveData();
	if (m_startNavGraphEdgePtr.IsValid())
		return m_traversal.ExploreAllNodesInNavGraphEdge(activeData, m_startPos3f, m_startNavGraphEdgePtr.GetRawPtr(), AStarNodeIndex_StartNode, m_navGraphEdgePathfindMode);

	if (m_startNavGraphVertexPtr.IsValid())
	{
		// As we create directly an usual node from which we can start the traversal without manually exploring its neighbor nodes, we need to add it to the binHeap
		KY_FORWARD_ERROR_NO_LOG(m_traversal.m_astarContext->CheckTraversalBinaryHeapMemory());
		m_traversal.m_astarContext->m_traversalBinHeap.Insert(0);

		return KY_SUCCESS; // Nothing to do since the StartNode is already an usual AstarNode(NavGraphVertex)
	}

	return m_traversal.ExploreAllNodesInTriangle(activeData, m_startTrianglePtr.GetRawPtr(), AStarNodeIndex_StartNode);
}


KY_TEMPLATE_BGVPFQ
KY_INLINE bool KY_FUNC_BGVPFQ::TraversalCustomizer::CanTraverse(const NavTriangleRawPtr& triangleRawPtr, KyFloat32* costMultiplier)
{
	return triangleRawPtr.CanBeTraversed<VertexFinderLogic>(m_traverseLogicUserData, costMultiplier);
}
KY_TEMPLATE_BGVPFQ
KY_INLINE bool KY_FUNC_BGVPFQ::TraversalCustomizer::CanTraverse(const NavGraphEdgeRawPtr& edgeRawPtr, KyFloat32* costMultiplier)
{
	return edgeRawPtr.CanBeTraversed<VertexFinderLogic>(m_traverseLogicUserData, costMultiplier);
}

KY_TEMPLATE_BGVPFQ
KY_INLINE bool KY_FUNC_BGVPFQ::TraversalCustomizer::CanEnterNavTag(const NavTag& exitNavTag, const NavTag& enterNavTag, const Vec3f& pos)
{
	return VertexFinderLogic::CanEnterNavTag(m_traverseLogicUserData, exitNavTag, enterNavTag, pos);
}

KY_TEMPLATE_BGVPFQ
KY_INLINE KyFloat32 KY_FUNC_BGVPFQ::TraversalCustomizer::GetHeuristicFromNodePosition(const Vec3f& /*nodePosition*/)
{
	return 0.f;
}

KY_TEMPLATE_BGVPFQ
KY_INLINE bool KY_FUNC_BGVPFQ::TraversalCustomizer::IsNodeOutsidePropagationBounds(AStarTraversalContext* astarContext, AStarNodeIndex nodeIndex)
{
	if (m_maxRadius == 0.f)
		return false;

	const AStarNode& astarNode = astarContext->m_aStarNodes[nodeIndex];
	return SquareDistance(astarNode.m_nodePosition, m_startPos) > m_maxRadius * m_maxRadius;
}

KY_TEMPLATE_BGVPFQ
KY_INLINE bool KY_FUNC_BGVPFQ::TraversalCustomizer::ShouldVisitNode(AStarTraversalContext* /*astarContext*/, AStarNodeIndex /*nodeIndex*/) { return true; }

KY_TEMPLATE_BGVPFQ
KY_INLINE bool KY_FUNC_BGVPFQ::TraversalCustomizer::ShouldOpenHalfEdgeNode(const NavHalfEdgeRawPtr& /*halfEdge*/, NavHalfEdgeRawPtr& /*pairHalfEdgeOfNewNode*/) { return true; }

KY_TEMPLATE_BGVPFQ
KY_INLINE bool KY_FUNC_BGVPFQ::TraversalCustomizer::ShouldOpenGraphVertexNode(const NavGraphVertexRawPtr& /*navGraphVertexRawPtr*/) { return true; }

KY_TEMPLATE_BGVPFQ
KY_INLINE bool KY_FUNC_BGVPFQ::TraversalCustomizer::ShouldOpenAbstractGraphNode(const AbstractGraphNodeRawPtr& /*abstractGraphNodeRawPtr*/) { return false; }

KY_TEMPLATE_BGVPFQ
KY_INLINE bool KY_FUNC_BGVPFQ::TraversalCustomizer::ShouldOpenNavTriangleUnderAbstractGraphNode(const AbstractGraphNodeRawPtr& /*abstractGraphNodeRawPtr*/) { return false; }

KY_TEMPLATE_BGVPFQ
KY_INLINE Vec3f KY_FUNC_BGVPFQ::TraversalCustomizer::ComputeNodePositionOnHalfEdge(AStarTraversalContext* /*aStarTraversalContext*/, const Vec3f& startPosOfEdge, const Vec3f& endPosOfEdge, AStarNodeIndex /*predecessorNodeIndex*/)
{
	return (endPosOfEdge + startPosOfEdge) * 0.5f;
}

KY_TEMPLATE_BGVPFQ
KY_INLINE KyResult KY_FUNC_BGVPFQ::TraversalCustomizer::OnNavTriangleExplored(AStarTraversalContext* /*aStarTraversalContext*/, const NavTriangleRawPtr& /*navTriangleRawPtr*/, AStarNodeIndex /*currentNodeIndex*/)
{
	return KY_SUCCESS;
}

KY_TEMPLATE_BGVPFQ
KY_INLINE KyResult KY_FUNC_BGVPFQ::TraversalCustomizer::OnNavGraphEdgeExplored(AStarTraversalContext* /*aStarTraversalContext*/, const NavGraphEdgeRawPtr& /*navGraphEdgeRawPtr*/, AStarNodeIndex /*currentNodeIndex*/)
{
	return KY_SUCCESS;
}



}

