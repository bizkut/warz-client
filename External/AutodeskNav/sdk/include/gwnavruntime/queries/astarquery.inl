/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{

//////////////////////////////////////////////////////////////////////////
// Visual debugging
// Uncomment this line to send the result of the path refiner, before the path is clamped.
//#define DBG_AStarQueryVisualDebug_RefiningOneEdgePerFrame


template<class TLogic>
AStarQuery<TLogic>::AStarQuery() : m_traversal(TraversalCustomizer())
{
	m_traversal.m_traversalParams = &m_traversalParams;
	m_traversal.m_traversalCustomizer.m_astarQuery = this;
}

template<class TLogic>
AStarQuery<TLogic>::~AStarQuery() {}

template<class TLogic>
void AStarQuery<TLogic>::BindToDatabase(Database* database)
{
	BaseAStarQuery::BindToDatabase(database);
}

template<class TLogic>
void AStarQuery<TLogic>::Initialize(const Vec3f& startPos, const Vec3f& destPos)
{
	BaseAStarQuery::Initialize(startPos, destPos);
}

template<class TLogic>
KY_INLINE void AStarQuery<TLogic>::SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr)
{
	BaseAStarQuery::SetStartTrianglePtr(startTrianglePtr);
}
template<class TLogic>
KY_INLINE void AStarQuery<TLogic>::SetDestTrianglePtr(const NavTrianglePtr& destTrianglePtr)
{
	BaseAStarQuery::SetDestTrianglePtr(destTrianglePtr);
}
template<class TLogic>
KY_INLINE void AStarQuery<TLogic>::SetStartNavGraphEdgePtr(const NavGraphEdgePtr& startNavGraphEdgePtr, NavGraphEdgeDirection navGraphEdgePathfindMode)
{
	BaseAStarQuery::SetStartNavGraphEdgePtr(startNavGraphEdgePtr, navGraphEdgePathfindMode);
}
template<class TLogic>
KY_INLINE void AStarQuery<TLogic>::SetStartNavGraphVertexPtr(const NavGraphVertexPtr& startNavGraphVertexPtr)
{
	BaseAStarQuery::SetStartNavGraphVertexPtr(startNavGraphVertexPtr);
}

template<class TLogic>
KY_INLINE void AStarQuery<TLogic>::SetDestNavGraphEdgePtr(const NavGraphEdgePtr& destNavGraphEdgePtr, NavGraphEdgeDirection navGraphEdgePathfindMode)
{
	BaseAStarQuery::SetDestNavGraphEdgePtr(destNavGraphEdgePtr, navGraphEdgePathfindMode);
}
template<class TLogic>
KY_INLINE void AStarQuery<TLogic>::SetDestNavGraphVertexPtr(const NavGraphVertexPtr& destNavGraphVertexPtr)
{
	BaseAStarQuery::SetDestNavGraphVertexPtr(destNavGraphVertexPtr);
}

template<class TLogic>
KY_INLINE void AStarQuery<TLogic>::SetTryCanGoMode(AStarQueryTryCanGoMode tryCanGoMode)
{
	BaseAStarQuery::SetTryCanGoMode(tryCanGoMode);
}
template<class TLogic>
KY_INLINE void AStarQuery<TLogic>::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	BaseAStarQuery::SetPositionSpatializationRange(positionSpatializationRange);
}
template<class TLogic>
KY_INLINE void AStarQuery<TLogic>::SetFromOutsideNavMeshDistance(KyFloat32 fromOutsideNavMeshDistance)
{
	BaseAStarQuery::SetFromOutsideNavMeshDistance(fromOutsideNavMeshDistance);
}
template<class TLogic>
KY_INLINE void AStarQuery<TLogic>::SetToOutsideNavMeshDistance(KyFloat32 toOutsideNavMeshDistance)
{
	BaseAStarQuery::SetToOutsideNavMeshDistance(toOutsideNavMeshDistance);
}
template<class TLogic>
KY_INLINE void AStarQuery<TLogic>::SetPropagationBoxExtent(KyFloat32 propagationBoxExtent)
{
	BaseAStarQuery::SetPropagationBoxExtent(propagationBoxExtent);
}
template<class TLogic>
KY_INLINE void AStarQuery<TLogic>::SetNumberOfProcessedNodePerFrame(KyUInt32 numberOfProcessedNodePerFrame)
{
	BaseAStarQuery::SetNumberOfProcessedNodePerFrame(numberOfProcessedNodePerFrame);
}

template<class TLogic>
KY_INLINE void AStarQuery<TLogic>::SetAbstractGraphTraversalMode(PathFinderAbstractGraphTraversalMode abstractGraphTraversalMode)
{
	BaseAStarQuery::SetAbstractGraphTraversalMode(abstractGraphTraversalMode);
}

template<class TLogic>
KY_INLINE void AStarQuery<TLogic>::SetComputeChannelMode(PathFinderQueryComputeChannelMode computeChannelMode)
{
	BaseAStarQuery::SetComputeChannelMode(computeChannelMode);
}

template<class TLogic>
KY_INLINE void AStarQuery<TLogic>::SetChannelComputerConfig(const ChannelComputerConfig& channelComputerConfig)
{
	BaseAStarQuery::SetChannelComputerConfig(channelComputerConfig);
}

template<class TLogic>
KY_INLINE AStarQueryResult           AStarQuery<TLogic>::GetResult()                     const { return BaseAStarQuery::GetResult();                     }
template<class TLogic>
KY_INLINE const Vec3f&               AStarQuery<TLogic>::GetStartPos()                   const { return BaseAStarQuery::GetStartPos();                   }
template<class TLogic>
KY_INLINE const Vec3f&               AStarQuery<TLogic>::GetDestPos()                    const { return BaseAStarQuery::GetDestPos();                    }
template<class TLogic>
KY_INLINE const NavTrianglePtr&      AStarQuery<TLogic>::GetStartTrianglePtr()           const { return BaseAStarQuery::GetStartTrianglePtr();           }
template<class TLogic>
KY_INLINE const NavGraphEdgePtr&     AStarQuery<TLogic>::GetStartNavGraphEdgePtr()       const { return BaseAStarQuery::GetStartNavGraphEdgePtr();       }
template<class TLogic>
KY_INLINE const NavGraphVertexPtr&   AStarQuery<TLogic>::GetStartNavGraphVertexPtr()     const { return BaseAStarQuery::GetStartNavGraphVertexPtr();     }
template<class TLogic>
KY_INLINE const NavTrianglePtr&      AStarQuery<TLogic>::GetDestTrianglePtr()            const { return BaseAStarQuery::GetDestTrianglePtr();            }
template<class TLogic>
KY_INLINE const NavGraphEdgePtr&     AStarQuery<TLogic>::GetDestNavGraphEdgePtr()        const { return BaseAStarQuery::GetDestNavGraphEdgePtr();        }
template<class TLogic>
KY_INLINE const NavGraphVertexPtr&   AStarQuery<TLogic>::GetDestNavGraphVertexPtr()      const { return BaseAStarQuery::GetDestNavGraphVertexPtr();      }
template<class TLogic>
KY_INLINE Path*                      AStarQuery<TLogic>::GetPath()                       const { return BaseAStarQuery::GetPath();                       }
template<class TLogic>
KY_INLINE KyFloat32                  AStarQuery<TLogic>::GetFromOutsideNavMeshDistance() const { return BaseAStarQuery::GetFromOutsideNavMeshDistance(); }
template<class TLogic>
KY_INLINE KyFloat32                  AStarQuery<TLogic>::GetToOutsideNavMeshDistance()   const { return BaseAStarQuery::GetToOutsideNavMeshDistance();   }
template<class TLogic>
KY_INLINE KyFloat32                  AStarQuery<TLogic>::GetPropagationBoxExtent()       const { return BaseAStarQuery::GetPropagationBoxExtent();       }
template<class TLogic>
KY_INLINE AStarQueryTryCanGoMode     AStarQuery<TLogic>::GetTryCanGoMode()               const { return BaseAStarQuery::GetTryCanGoMode();               }

template<class TLogic>
KY_INLINE KyUInt32  AStarQuery<TLogic>::GetNumberOfProcessedNodePerFrame() const { return BaseAStarQuery::GetNumberOfProcessedNodePerFrame(); }
template<class TLogic>
KY_INLINE const PositionSpatializationRange& AStarQuery<TLogic>::GetPositionSpatializationRange() const { return BaseAStarQuery::GetPositionSpatializationRange(); }

template<class TLogic>
KY_INLINE void AStarQuery<TLogic>::ComputePathRefinerConfigInternals() { BaseAStarQuery::ComputePathRefinerConfigInternals(); }
template<class TLogic>
KY_INLINE const PathRefinerConfig& AStarQuery<TLogic>::GetPathRefinerConfig() const { return BaseAStarQuery::GetPathRefinerConfig(); }


template<class TLogic>
inline void AStarQuery<TLogic>::ReleaseWorkingMemoryOnCancelDuringProcess(WorkingMemory* workingMemory)
{
	if (workingMemory == KY_NULL)
		workingMemory = m_database->GetWorkingMemory();

	SetResult(ASTAR_DONE_COMPUTATION_CANCELED);
	SetFinish(workingMemory);
}

template<class TLogic>
inline bool AStarQuery<TLogic>::ShouldReturnAfterTryingCanGo(WorkingMemory* workingMemory)
{
	if (m_startNavGraphEdgePtr.IsValid() || m_startNavGraphVertexPtr.IsValid() || m_destNavGraphEdgePtr.IsValid() || m_destNavGraphVertexPtr.IsValid())
		return false;

	KY_LOG_ERROR_IF(m_startTrianglePtr.IsValid() == false, ("Error, a valid NavTrianglePtr should have been found for startPos"));
	KY_LOG_ERROR_IF(m_destTrianglePtr.IsValid() == false, ("Error, a valid NavTrianglePtr should have been found for destPos"));
	MonodirectionalRayCanGoQuery<TLogic> canGoQuery;
	canGoQuery.BindToDatabase(m_database);
	canGoQuery.SetTraverseLogicUserData(GetTraverseLogicUserData());
	canGoQuery.SetMarginMode(PathMargin);
	canGoQuery.SetPositionSpatializationRange(GetPositionSpatializationRange());

	canGoQuery.Initialize(m_startPos3f, m_destPos3f);
	canGoQuery.SetStartTrianglePtr(m_startTrianglePtr);
	canGoQuery.SetPerformQueryStat(QueryStat_PathFinder);
	canGoQuery.PerformQuery(workingMemory);
	if (canGoQuery.GetResult() != RAYCANGO_DONE_SUCCESS || canGoQuery.GetDestTrianglePtr() != m_destTrianglePtr)
		return false;

	if (KY_FAILED(workingMemory->m_clamperContext->InitFromCanGo(workingMemory, canGoQuery)))
	{
		SetResult(ASTAR_DONE_ERROR_LACK_OF_WORKING_MEMORY);
		SetFinish(workingMemory);
	}
	else
	{
		SetResult(ASTAR_PROCESSING_PATHCLAMPING);
	}

	return true;
}


template<class TLogic>
inline void AStarQuery<TLogic>::InitPropagationBounds()
{
	InitCellFilterFromAbstractGraph();
	if (m_propagationBoxExtent <= 0.f)
		m_traversal.m_traversalCustomizer.m_propagationBox.Clear();
	else
		m_traversal.m_traversalCustomizer.m_propagationBox.InitAs2dInflatedSegment(m_startPos3f, m_destPos3f, m_propagationBoxExtent);
}


KY_EXIT_CLASS_TEMPLATE_SCOPE_BEGIN(AStarQuery, Advance, TLogic)
if (self->GetPath())
	self->GetPath()->SetNavigationProfileId(self->GetNavigationProfileId());
if (self->GetAbstractPath())
	self->GetAbstractPath()->SetNavigationProfileId(self->GetNavigationProfileId());
KY_EXIT_CLASS_SCOPE_END

template<class TLogic>
inline void AStarQuery<TLogic>::Advance(WorkingMemory* workingMemory)
{
	KY_EXIT_SCOPE_TEMPLATE_INSTANCE(AStarQuery, Advance, TLogic);

	++m_advanceCount;
	m_processStatus = QueryInProcess;

	PathFinderQueryUtils queryUtils(m_database, workingMemory, GetTraverseLogicUserData());
	queryUtils.SetPerformQueryStat(QueryStat_PathFinder);
	switch (GetResult())
	{
	case ASTAR_NOT_PROCESSED :
		{
			m_traversal.m_traversalCustomizer.m_traverseLogicUserData = GetTraverseLogicUserData();

			m_lastAdvanceFrameIdx = m_database->m_frameIdx;

			if (m_database->IsClear())
			{
				SetResult(ASTAR_DONE_START_OUTSIDE);
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
				SetFinish(workingMemory);
				return;
			}

			if ((m_destPos3f == m_startPos3f) ||
				(m_startNavGraphVertexPtr.IsValid() && m_startNavGraphVertexPtr == m_destNavGraphVertexPtr))
			{
				SetResult(ASTAR_DONE_DEST_IS_START_NO_PATH);
				SetFinish(workingMemory);
				return;
			}

			if (GetTryCanGoMode() == ASTAR_DO_TRY_RAYCANGO)
			{
				if (ShouldReturnAfterTryingCanGo(workingMemory))
					return;
			}

			if (KY_FAILED(InitializeContextAndStartNode(queryUtils)))
			{
				SetResult(ASTAR_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			if (KY_FAILED(InitDestinationNode()))
			{
				SetResult(ASTAR_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			InitPropagationBounds();

			// init the propagation with the neighbors of the startNode
			if (KY_FAILED(InitTraversalFromStartNode(queryUtils)))
			{
				SetResult(ASTAR_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			if (m_traversal.IsThereNodeToVisit() == false)
			{
				if (HasExplorationReachedDestNode() == false)
				{
					// no neighbor for the start nodes
					SetResult(ASTAR_DONE_PATH_NOT_FOUND);
					SetFinish(workingMemory);
					return;
				}

				SetResult(ASTAR_PROCESSING_TRAVERSAL_DONE);
			}
			else
				SetResult(ASTAR_PROCESSING_TRAVERSAL);

			break;
		}
	case ASTAR_PROCESSING_TRAVERSAL :
		{
			if (KY_FAILED(CheckNavDataChangeFromAstarContext(workingMemory)))
				return;

			TraversalVisitNodeContext visitNodeContext;
			for(KyUInt32 numberOfNode = 0; numberOfNode < m_traversalParams.m_numberOfVisitedNodePerFrame; ++numberOfNode)
			{
				if (KY_FAILED(m_traversal.VisitNode(queryUtils, visitNodeContext)))
				{
					SetResult(ASTAR_DONE_ERROR_LACK_OF_WORKING_MEMORY);
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
				// queryUtils.DisplayListPropagation("AstarQuery");

				if (HasExplorationReachedDestNode() == false)
				{
					SetResult(ASTAR_DONE_PATH_NOT_FOUND);
					SetFinish(workingMemory);
					return;
				}

				SetResult(ASTAR_PROCESSING_TRAVERSAL_DONE);
			}

			break;
		}

	case ASTAR_PROCESSING_TRAVERSAL_DONE :
		{
			if (KY_FAILED(CheckNavDataChangeFromAstarContext(workingMemory)))
				return;


			SetResult(ASTAR_PROCESSING_ABSTRACT_PATH);
			break;
		}

	case ASTAR_PROCESSING_ABSTRACT_PATH :
		{
			if (KY_FAILED(CheckNavDataChangeFromAstarContext(workingMemory)))
				return;
			
			AStarNodeIndex concreteDestAStarNodeIndex;
			BuildAbstractPath(workingMemory, concreteDestAStarNodeIndex);

			PathRefinerContext* refinerContext = workingMemory->GetOrCreatePathRefinerContext();
			if (KY_FAILED(refinerContext->InitFromAstarTraversalContext(workingMemory, workingMemory->m_astarContext, concreteDestAStarNodeIndex)))
			{
				SetResult(ASTAR_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			SetResult(ASTAR_PROCESSING_REFINING_INIT);
			break;
		}

	case ASTAR_PROCESSING_REFINING_INIT :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			ComputePathRefinerConfigInternals();

			if (KY_FAILED(queryUtils.BuildRefinerBinaryHeap()))
			{
				SetResult(ASTAR_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			PathRefinerContext* refinerContext = workingMemory->GetOrCreatePathRefinerContext();
			if (refinerContext->m_currentNodeIdx == refinerContext->m_refinerNodes.GetCount())
			{
				if (refinerContext->m_refinerBinHeap.IsEmpty())
					SetResult(ASTAR_PROCESSING_PATHCLAMPING_INIT);
				else
					SetResult(ASTAR_PROCESSING_REFINING);
			}

			break;
		}
	case ASTAR_PROCESSING_REFINING :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			PathRefinerContext* refinerContext = workingMemory->GetOrCreatePathRefinerContext();
			for(KyUInt32 cangoTestCount = 0; cangoTestCount < m_traversalParams.m_numberOfCanGoTestInRefinerPerFrame;)
			{
				if (KY_FAILED(queryUtils.RefineOneNode<TLogic>(GetPositionSpatializationRange(), GetPathRefinerConfig(), cangoTestCount)))
				{
					SetResult(ASTAR_DONE_ERROR_LACK_OF_WORKING_MEMORY);
					SetFinish(workingMemory);
					return;
				}

#ifdef DBG_AStarQueryVisualDebug_RefiningOneEdgePerFrame
				queryUtils.DisplayListRefining("AstarQuery");
				break;
#else
				if (refinerContext->m_refinerBinHeap.IsEmpty())
					break;
#endif
			}

			if (refinerContext->m_refinerBinHeap.IsEmpty())
			{
#ifdef DBG_AStarQueryVisualDebug_RefiningOneEdgePerFrame
				queryUtils.DisplayListRefining("AstarQuery");
#endif
				SetResult(ASTAR_PROCESSING_PATHCLAMPING_INIT);
			}

			break;
		}
	case ASTAR_PROCESSING_PATHCLAMPING_INIT :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			if (KY_FAILED(workingMemory->m_clamperContext->InitFromRefinerContext(workingMemory, workingMemory->GetOrCreatePathRefinerContext())))
			{
				SetResult(ASTAR_DONE_ERROR_LACK_OF_WORKING_MEMORY);
				SetFinish(workingMemory);
				return;
			}

			if (m_computeChannelMode == PATHFINDER_DO_NOT_COMPUTE_CHANNELS)
				// Do not clamp the raw path if a channel is to be computed
				SetResult(ASTAR_PROCESSING_PATHCLAMPING);
			else
				SetResult(ASTAR_PROCESSING_PATHBUILDING);

			break;
		}
	case ASTAR_PROCESSING_PATHCLAMPING :
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
						SetResult(ASTAR_DONE_COMPUTATION_ERROR);
					else
						SetResult(ASTAR_DONE_ERROR_LACK_OF_WORKING_MEMORY);

					SetFinish(workingMemory);
					return;
				}

				if (pathClamperContext->IsClampingDone())
					break;
			}

			if (pathClamperContext->IsClampingDone())
			{
				SetResult(ASTAR_PROCESSING_PATHBUILDING);
			}

			break;
		}
	case ASTAR_PROCESSING_PATHBUILDING :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			Ptr<Path> rawPath;
			PathClamperContext* pathClamperContext  = workingMemory->GetOrCreatePathClamperContext();
			if (m_abstractPath != KY_NULL)
			{
				Vec3f concreteDestPos = pathClamperContext->m_clampNodes[0].m_nodePosition; // destination is stored as first element
				pathClamperContext->m_startNavGraphEdgePtr = m_startNavGraphEdgePtr;
				rawPath = queryUtils.ComputePathFromPathClamperContext<TLogic>(m_startPos3f, concreteDestPos, GetPositionSpatializationRange());
			}
			else
			{
				pathClamperContext->m_startNavGraphEdgePtr = m_startNavGraphEdgePtr;
				pathClamperContext->m_destNavGraphEdgePtr  = m_destNavGraphEdgePtr;
				rawPath = queryUtils.ComputePathFromPathClamperContext<TLogic>(m_startPos3f, m_destPos3f, GetPositionSpatializationRange());
			}

			if (m_computeChannelMode == PATHFINDER_DO_NOT_COMPUTE_CHANNELS)
			{
				SetPath(rawPath);
				SetResult(ASTAR_DONE_PATH_FOUND);
				SetFinish(workingMemory);
				return;
			}

			ChannelArrayComputer& channelArrayComputer = *workingMemory->GetOrCreateChannelArrayComputer();
			channelArrayComputer.Init(rawPath);
			channelArrayComputer.SetTraverseLogicUserData(GetTraverseLogicUserData());
			channelArrayComputer.SetChannelComputerConfig(m_channelComputerConfig);

			SetResult(ASTAR_PROCESSING_CHANNEL_COMPUTE);
			break;
		}
	case ASTAR_PROCESSING_CHANNEL_COMPUTE :
		{
			if (KY_FAILED(CheckNavDataChange(workingMemory)))
				return;

			ChannelArrayComputer& channelArrayComputer = *workingMemory->GetOrCreateChannelArrayComputer();
			channelArrayComputer.Advance<TLogic>(workingMemory);

			if (channelArrayComputer.IsFinished())
			{
				if (channelArrayComputer.m_result != ChannelArrayResult_Done)
				{
					SetResult(ASTAR_DONE_CHANNELCOMPUTATION_ERROR);
					SetFinish(workingMemory);
					return;
				}

				SetPath(channelArrayComputer.m_pathWithChannels);
				channelArrayComputer.m_channelArray = KY_NULL;
				channelArrayComputer.m_pathWithChannels = KY_NULL;

				SetResult(ASTAR_DONE_PATH_FOUND);
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
KY_INLINE KyUInt32 AStarQuery<TLogic>::CountAbstractGraphNodes(WorkingMemory* workingMemory, AStarNodeIndex& concreteDestinationNodeIdx)
{
	const AStarTraversalContext* astarContext = workingMemory->m_astarContext;
	const WorkingMemArray<AStarNode>& aStarNodes = astarContext->m_aStarNodes;

	const AStarNode* currentAstarNode = &aStarNodes[AStarNodeIndex_DestNode];
	AStarNodeIndex currentNodeIdx = AStarNodeIndex_DestNode;
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
KY_INLINE void AStarQuery<TLogic>::BuildAbstractPath(WorkingMemory* workingMemory, AStarNodeIndex& concreteDestAStarNodeIndex)
{

	concreteDestAStarNodeIndex = AStarNodeIndex_DestNode;
	KyUInt32 abstractNodeCount = CountAbstractGraphNodes(workingMemory, concreteDestAStarNodeIndex);
	KY_ASSERT(concreteDestAStarNodeIndex != AStarNodeIndex_Invalid);
	if (abstractNodeCount == 0)
	{
		m_abstractPath = KY_NULL;
		return;
	}

	KyUInt32 nodesCount = abstractNodeCount +2; // add start and end nodes;
	Path::CreateConfig config;
	config.m_nodesCount = nodesCount;

	Ptr<Path> pathPtr = Path::CreatePath(config);
	Path& path = *pathPtr;

	KyUInt32 nodeIdx = nodesCount - 1;
	const DatabaseGenMetrics& genMetrics = m_database->GetDatabaseGenMetrics();
	WorldIntegerPos integerPos = genMetrics.GetWorldIntegerPosFromVec3f(m_destPos3f);
	path.SetNodePosition3fAndInteger(nodeIdx, m_destPos3f, integerPos);
	path.GetPathCellBox().ExpandByVec2(integerPos.m_cellPos);
	path.SetPathEdgeType(nodeIdx-1, PathEdgeType_AbstractEdge);
	--nodeIdx;

	AStarTraversalContext* astarContext = workingMemory->m_astarContext;
	WorkingMemArray<AStarNode>& aStarNodes = astarContext->m_aStarNodes;
	AStarNode* currentAstarNode = &aStarNodes[AStarNodeIndex_DestNode];
	KyFloat32 currentDistance = currentAstarNode->m_costFromStart;
	path.SetPathCostAndDistance(currentDistance, currentDistance);
	path.m_database = m_database;

	AStarNodeIndex currentNodeIdx = AStarNodeIndex_DestNode;
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
			integerPos = genMetrics.GetWorldIntegerPosFromVec3f(m_startPos3f);
			path.SetNodePosition3fAndInteger(nodeIdx, m_startPos3f, integerPos);
			path.GetPathCellBox().ExpandByVec2(integerPos.m_cellPos);
			break;
		}

		currentNodeIdx = currentAstarNode->GetIndexOfPredecessor();
		currentAstarNode = &aStarNodes[currentNodeIdx];
	}

	m_abstractPath = pathPtr;
}


template<class TLogic>
KY_INLINE bool AStarQuery<TLogic>::HasNavDataChanged()
{
	return m_traversal.m_astarContext->m_edgeIndexGrid.HasNavDataChanged(m_database);
}

template<class TLogic>
inline void AStarQuery<TLogic>::SetFinish(WorkingMemory* workingMemory)
{
	m_processStatus = QueryDone; // TODO - CHANGE THIS!
	m_dataBaseChangeIdx = m_database ? m_database->m_navdataChangeIdx : 0;

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
inline KyResult AStarQuery<TLogic>::CheckNavDataChange(WorkingMemory* workingMemory)
{
	if(m_lastAdvanceFrameIdx != m_database->m_frameIdx)
	{
		if(workingMemory->m_navDataChangeIndexInGrid->HasNavDataChanged(m_database))
		{
			SetResult(ASTAR_DONE_NAVDATA_CHANGED);
			SetFinish(workingMemory);
			return KY_ERROR;
		}

		m_lastAdvanceFrameIdx = m_database->m_frameIdx;
	}

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult AStarQuery<TLogic>::CheckNavDataChangeFromAstarContext(WorkingMemory* workingMemory)
{
	if(m_lastAdvanceFrameIdx != m_database->m_frameIdx)
	{
		if(m_traversal.m_astarContext->m_edgeIndexGrid.HasNavDataChanged(m_database))
		{
			SetResult(ASTAR_DONE_NAVDATA_CHANGED);
			SetFinish(workingMemory);
			return KY_ERROR;
		}

		m_lastAdvanceFrameIdx = m_database->m_frameIdx;
	}

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult AStarQuery<TLogic>::ComputeStartGraphEdgeOrGraphVertexOrTriangle(PathFinderQueryUtils& queryUtils)
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

template<class TLogic>
inline KyResult AStarQuery<TLogic>::ComputeStartTriangle(PathFinderQueryUtils& queryUtils)
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
		if (queryUtils.TryToHookOnNavMesh<TLogic>(m_startPos3f, GetPositionSpatializationRange(), GetFromOutsideNavMeshDistance(),
			0.1f /*distFromObstlace*/, m_startInsidePos3f, m_startTrianglePtr) == KY_SUCCESS)
		{
			return KY_SUCCESS;
		}
	}

	if (m_startTrianglePtr.IsValid() == false)
		SetResult(ASTAR_DONE_START_OUTSIDE);
	else
		SetResult(ASTAR_DONE_START_NAVTAG_FORBIDDEN);

	return KY_ERROR;
}


template<class TLogic>
inline KyResult AStarQuery<TLogic>::ComputeDestTriangleIfNeeded(PathFinderQueryUtils& queryUtils)
{
	if(m_destNavGraphEdgePtr.IsValid())
	{
		m_destInsidePos3f = m_destPos3f;
		return KY_SUCCESS;
	}

	if (m_destNavGraphVertexPtr.IsValid())
	{
		m_destInsidePos3f = m_destNavGraphVertexPtr.GetGraphVertexPosition();
		return KY_SUCCESS;
	}

	KY_DEBUG_ASSERTN(queryUtils.m_database->GetActiveData() != KY_NULL, ("the PathFinderQueryUtils need a valid activeData"));

	if (queryUtils.FindTriangleFromPositionIfNotValid(m_destTrianglePtr, m_destPos3f, GetPositionSpatializationRange()) == true)
	{
		if(m_traversal.m_traversalCustomizer.CanTraverse(m_destTrianglePtr.GetRawPtr(), KY_NULL) == true)
		{
			m_destInsidePos3f = m_destPos3f;
			return KY_SUCCESS;
		}
	}

	if (GetToOutsideNavMeshDistance() > 0.f)
	{
		// the position is outside the NavMesh or in an invalid navtag. Try to hook on the NavMesh
		if (queryUtils.TryToHookOnNavMesh<TLogic>(m_destPos3f, GetPositionSpatializationRange(),
			GetToOutsideNavMeshDistance(), 0.1f /*distFromObstlace*/, m_destInsidePos3f, m_destTrianglePtr) == KY_SUCCESS)
		{
			return KY_SUCCESS;
		}
	}

	if (m_destTrianglePtr.IsValid() == false)
		SetResult(ASTAR_DONE_END_OUTSIDE);
	else
		SetResult(ASTAR_DONE_END_NAVTAG_FORBIDDEN);

	return KY_ERROR;
}

template<class TLogic>
inline KyResult AStarQuery<TLogic>::InitializeContextAndStartNode(PathFinderQueryUtils& queryUtils)
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

template<class TLogic>
inline KyResult AStarQuery<TLogic>::InitDestinationNode()
{
	if(m_destNavGraphEdgePtr.IsValid())
		return InitDestinationNodeFromDestNavGraphEdge();

	if(m_destNavGraphVertexPtr.IsValid())
		return InitDestinationNodeFromDestNavGraphVertex();

	if(m_destTrianglePtr.IsValid())
		return InitDestinationNodeFromDestNavTriangle();

	return KY_ERROR;
}

template<class TLogic>
inline KyResult AStarQuery<TLogic>::InitDestinationNodeFromDestNavGraphEdge()
{
	KY_FORWARD_ERROR_NO_LOG(m_traversal.InitializeDestNode(m_destPos3f, m_destNavGraphEdgePtr));
	KY_ASSERT(AStarNodeIndex_DestNode == m_traversal.m_astarContext->m_aStarNodes.GetCount() - 1);

	m_traversal.m_traversalCustomizer.m_destTriangleRawPtr.Invalidate();
	m_traversal.m_traversalCustomizer.m_destNavGraphEdgeRawPtr[0] = m_destNavGraphEdgePtr.GetRawPtr();
	
	if(m_navGraphEdgePathfindMode == NAVGRAPHEDGE_BIDIRECTIONAL)
		m_traversal.m_traversalCustomizer.m_destNavGraphEdgeRawPtr[1] = m_destNavGraphEdgePtr.GetRawPtr().GetOppositeNavGraphEdgeRawPtr();
	else
		m_traversal.m_traversalCustomizer.m_destNavGraphEdgeRawPtr[1].Invalidate();

	return KY_SUCCESS;
}
template<class TLogic>
inline KyResult AStarQuery<TLogic>::InitDestinationNodeFromDestNavGraphVertex()
{
	KY_FORWARD_ERROR_NO_LOG(m_traversal.InitializeDestNode(m_destNavGraphVertexPtr));
	KY_ASSERT(AStarNodeIndex_DestNode == m_traversal.m_astarContext->m_aStarNodes.GetCount() - 1);
	m_traversal.m_traversalCustomizer.m_destTriangleRawPtr.Invalidate();
	m_traversal.m_traversalCustomizer.m_destNavGraphEdgeRawPtr[0].Invalidate();
	m_traversal.m_traversalCustomizer.m_destNavGraphEdgeRawPtr[1].Invalidate();

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult AStarQuery<TLogic>::InitDestinationNodeFromDestNavTriangle()
{
	KY_FORWARD_ERROR_NO_LOG(m_traversal.InitializeDestNode(m_destInsidePos3f, m_destTrianglePtr));
	KY_ASSERT(AStarNodeIndex_DestNode == m_traversal.m_astarContext->m_aStarNodes.GetCount() - 1);

	m_traversal.m_traversalCustomizer.m_destTriangleRawPtr = m_destTrianglePtr.GetRawPtr();
	m_traversal.m_traversalCustomizer.m_destNavGraphEdgeRawPtr[0].Invalidate();
	m_traversal.m_traversalCustomizer.m_destNavGraphEdgeRawPtr[1].Invalidate();

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult AStarQuery<TLogic>::InitTraversalFromStartNode(PathFinderQueryUtils& queryUtils)
{
	ActiveData* activeData = queryUtils.m_database->GetActiveData();
	if (m_startNavGraphEdgePtr.IsValid())
		return m_traversal.ExploreAllNodesInNavGraphEdge(activeData, m_startPos3f, m_startNavGraphEdgePtr.GetRawPtr(), AStarNodeIndex_StartNode, m_navGraphEdgePathfindMode);

	if (m_startNavGraphVertexPtr.IsValid())
	{
		if (m_startNavGraphVertexPtr != m_destNavGraphVertexPtr)
		{
			// As we create directly an usual node from which we can start the traversal without manually exploring its neighbor nodes, we need to add it to the binHeap
			KY_FORWARD_ERROR_NO_LOG(m_traversal.m_astarContext->CheckTraversalBinaryHeapMemory());
			m_traversal.m_astarContext->m_traversalBinHeap.Insert(0);
		}
		else
		{
			m_traversal.m_astarContext->m_aStarNodes[AStarNodeIndex_DestNode].m_indexOfPredecessor = AStarNodeIndex_StartNode;
		}

		return KY_SUCCESS; // Nothing to do since the StartNode is already an usual AstarNode(NavGraphVertex)
	}

	return m_traversal.ExploreAllNodesInTriangle(activeData, m_startTrianglePtr.GetRawPtr(), AStarNodeIndex_StartNode);
}


template<class TLogic>
bool AStarQuery<TLogic>::HasExplorationReachedDestNode() { return m_traversal.m_astarContext->m_aStarNodes[AStarNodeIndex_DestNode].m_indexOfPredecessor != AStarNodeIndex_Invalid; }

////////////////////////////////////////////////////////////////////
/// AStarTraversalCustomizer
///////////////////////////////////////////////////////////////////

template<class TLogic>
KY_INLINE AStarQuery<TLogic>::TraversalCustomizer::TraversalCustomizer() : m_astarQuery(KY_NULL) {}
template<class TLogic>
KY_INLINE bool AStarQuery<TLogic>::TraversalCustomizer::CanTraverse(const NavTriangleRawPtr& triangleRawPtr, KyFloat32* costMultiplier)
{
	return triangleRawPtr.CanBeTraversed<TLogic>(m_traverseLogicUserData, costMultiplier);
}
template<class TLogic>
KY_INLINE bool AStarQuery<TLogic>::TraversalCustomizer::CanTraverse(const NavGraphEdgeRawPtr& edgeRawPtr, KyFloat32* costMultiplier)
{
	return edgeRawPtr.CanBeTraversed<TLogic>(m_traverseLogicUserData, costMultiplier);
}

template<class TLogic>
KY_INLINE bool AStarQuery<TLogic>::TraversalCustomizer::CanEnterNavTag(const NavTag& exitNavTag, const NavTag& enterNavTag, const Vec3f& pos)
{
	return TLogic::CanEnterNavTag(m_traverseLogicUserData, exitNavTag, enterNavTag, pos);
}

template<class TLogic>
KY_INLINE KyFloat32 AStarQuery<TLogic>::TraversalCustomizer::GetHeuristicFromNodePosition(const Vec3f& nodePosition)
{
	return TLogic::GetHeuristicFromDistanceToDest(m_traverseLogicUserData, Distance(nodePosition, m_astarQuery->m_destInsidePos3f));
}

template<class TLogic>
KY_INLINE bool AStarQuery<TLogic>::TraversalCustomizer::ShouldVisitNode(AStarTraversalContext* astarContext, AStarNodeIndex nodeIndex)
{
	if (nodeIndex == AStarNodeIndex_DestNode)
	{
		// empty the binary
		astarContext->m_traversalBinHeap.Clear();
		return false;
	}

	return true;
}
template<class TLogic>
KY_INLINE bool AStarQuery<TLogic>::TraversalCustomizer::IsNodeOutsidePropagationBounds(AStarTraversalContext* astarContext, AStarNodeIndex nodeIndex) 
{
	const AStarNode& astarNode = astarContext->m_aStarNodes[nodeIndex];
	if (m_propagationBox.IsValid() == true)
		return m_propagationBox.IsInside2d(astarNode.m_nodePosition) == false;
		
	if (m_astarQuery->m_propagationCellFilter == KY_NULL)
		return false;
	
	PathNodeType pathNodeType = astarNode.GetNodeType();
	if (pathNodeType == NodeType_NavMeshEdge || pathNodeType == NodeType_FreePositionOnNavMesh)
	{
		KyUInt32 rawPtrIdx = astarNode.GetIdxOfRawPtrData();
		NavHalfEdgeRawPtr& halfEdge = astarContext->m_edgeRawPtrNodes[rawPtrIdx];
		const CellPos& cellPos = halfEdge.GetCellPos();
		if (m_astarQuery->m_propagationCellFilter->IsInFilter(cellPos) == false)
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
KY_INLINE bool AStarQuery<TLogic>::TraversalCustomizer::ShouldOpenGraphVertexNode(const NavGraphVertexRawPtr& /*navGraphVertexRawPtr*/) { return true; }
template<class TLogic>
KY_INLINE bool AStarQuery<TLogic>::TraversalCustomizer::ShouldOpenAbstractGraphNode(const AbstractGraphNodeRawPtr& /*abstractGraphNodeRawPtr*/) { return true; }
template<class TLogic>
KY_INLINE bool AStarQuery<TLogic>::TraversalCustomizer::ShouldOpenNavTriangleUnderAbstractGraphNode(const AbstractGraphNodeRawPtr& abstractGraphNodeRawPtr)
{
	// Check if connected to destination NavMesh
	CellPos cellPos = m_destTriangleRawPtr.GetCellPos();
	AbstractGraph* destAbstractGraph = m_astarQuery->m_database->m_abstractGraphCellGrid->GetAbstractGraph(cellPos);
	if (abstractGraphNodeRawPtr.m_abstractGraph == destAbstractGraph)
		return true;

	return false;
}

template<class TLogic>
inline Vec3f AStarQuery<TLogic>::TraversalCustomizer::ComputeNodePositionOnHalfEdge(AStarTraversalContext* aStarTraversalContext,
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
		const Vec3f& destPos = m_astarQuery->m_destInsidePos3f;

		// we cannot keep on going straight forward
		// try to find a position on the go straight towards the dest pos
		if (Intersections::SegmentVsSegment2d(reducedStartPosOfEdge, reducedEndPosOfEdge, predecessorNode.m_nodePosition, destPos, nodePos))
		{
			// Ok find a new position
			// insert node in the binary Heap
		}
		else
		{
			// no "good" position on the edge, try to find a position towards destPos
			KyFloat32 squareDist;
			ClosestPoint::OnSegmentVsPoint2d(reducedStartPosOfEdge, reducedEndPosOfEdge, destPos, nodePos, squareDist);
			// insert node in the binary Heap
		}
	}

	return nodePos;
}

template<class TLogic>
inline KyResult AStarQuery<TLogic>::TraversalCustomizer::OnNavTriangleExplored(AStarTraversalContext* aStarTraversalContext, const NavTriangleRawPtr& navTriangleRawPtr, AStarNodeIndex currentNodeIndex)
{
	if (navTriangleRawPtr == m_destTriangleRawPtr)
	{
		WorkingMemArray<AStarNode>& aStarNodes = aStarTraversalContext->m_aStarNodes;
		AstarTraversalBinHeap& binaryHeap = aStarTraversalContext->m_traversalBinHeap;

		// check if we do not have to update the DestNode
		AStarNode& destNode = aStarNodes[AStarNodeIndex_DestNode];
		KyFloat32 costMultiplier = 1.f;
		if(CanTraverse(m_destTriangleRawPtr, &costMultiplier) == false)
			return KY_SUCCESS;

		KY_LOG_ERROR_IF(costMultiplier <= 0.f, ("negative cost are not supported"));

		const KyFloat32 cost = Distance(aStarNodes[currentNodeIndex].m_nodePosition, destNode.m_nodePosition) * costMultiplier;
		if (destNode.m_indexInBinaryHeap != IndexInBinHeap_UnSet)
		{
			// destNode is already opened ->  we update it
			KY_FORWARD_ERROR_NO_LOG(m_astarQuery->m_traversal.UpdateOpenedOrClosedNode(AStarNodeIndex_DestNode, currentNodeIndex, cost));
		}
		else
		{
			// check for memory to insert a newElement in the binary heap
			KY_FORWARD_ERROR_NO_LOG(aStarTraversalContext->CheckTraversalBinaryHeapMemory());

			// dest node has not been open yet. We add it in the Binary Heap
			destNode.m_costFromStart = aStarNodes[currentNodeIndex].m_costFromStart + cost; // so far the dest is bound to be in the navMesh
			destNode.SetIndexOfPredecessor(currentNodeIndex);

			binaryHeap.Insert(AStarNodeIndex_DestNode);
		}
	}

	return KY_SUCCESS;
}

template<class TLogic>
inline KyResult AStarQuery<TLogic>::TraversalCustomizer::OnNavGraphEdgeExplored(AStarTraversalContext* aStarTraversalContext, const NavGraphEdgeRawPtr& navGraphEdgeRawPtr, AStarNodeIndex currentNodeIndex)
{
	KY_ASSERT(navGraphEdgeRawPtr.IsValid());

	for(KyUInt32 i=0; i<2; ++i)
	{
		if (navGraphEdgeRawPtr == m_destNavGraphEdgeRawPtr[i])
		{
			WorkingMemArray<AStarNode>& aStarNodes = aStarTraversalContext->m_aStarNodes;
			AstarTraversalBinHeap& binaryHeap = aStarTraversalContext->m_traversalBinHeap;

			// check if we do not have to update the DestNode
			AStarNode& destNode = aStarNodes[AStarNodeIndex_DestNode];
			KY_ASSERT(navGraphEdgeRawPtr.GetStartNavGraphVertexPosition() == aStarNodes[currentNodeIndex].m_nodePosition ||
				currentNodeIndex == AStarNodeIndex_StartNode || currentNodeIndex == AStarNodeIndex_DestNode);

			KyFloat32 costMultiplier = 1.f;
			if(CanTraverse(navGraphEdgeRawPtr, &costMultiplier) == false)
				continue;

			const KyFloat32 cost = Distance(aStarNodes[currentNodeIndex].m_nodePosition, destNode.m_nodePosition) * costMultiplier;

			if (destNode.m_indexInBinaryHeap != IndexInBinHeap_UnSet)
			{
				// destNode is already opened ->  we update it
				KY_FORWARD_ERROR_NO_LOG(m_astarQuery->m_traversal.UpdateOpenedOrClosedNode(AStarNodeIndex_DestNode, currentNodeIndex, cost));
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

				binaryHeap.Insert(AStarNodeIndex_DestNode);
			}
		}
	}
	

	return KY_SUCCESS;
}

template<class TLogic>
inline bool AStarQuery<TLogic>::TraversalCustomizer::ShouldOpenHalfEdgeNode(const NavHalfEdgeRawPtr& /*halfEdge*/, NavHalfEdgeRawPtr& pairHalfEdgeOfNewNode)
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

		if (halfEdges[nextIdx].GetHalfEdgeType() == EDGETYPE_OBSTACLE && halfEdges[prevtIdx].GetHalfEdgeType() == EDGETYPE_OBSTACLE &&
			triangleRawPtr != m_destTriangleRawPtr)
			return false; // edge crossable but leading to a dead end, do not create a node
	}

	return true;
}

}

