/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{

KY_INLINE void TraversalVisitNodeContext::Clear()
{
	m_visitedGraphVertexRawPtr.Invalidate();
	m_visitedNodeIndex = AStarNodeIndex_Invalid;
}


template<class TraversalCustomzier>
KY_INLINE bool AStarTraversal<TraversalCustomzier>::CanTraverseNavTriangle(const NavTriangleRawPtr& triangle)
{
	return m_traversalCustomizer.CanTraverse(triangle, KY_NULL);
}
template<class TraversalCustomzier>
KY_INLINE bool AStarTraversal<TraversalCustomzier>::CanTraverseNavTriangleAndGetCost(const NavTriangleRawPtr& triangle, const Vec3f& A, const Vec3f& B, KyFloat32& edgeCost)
{
	KyFloat32 costMultiplier = 1.f;
	if (m_traversalCustomizer.CanTraverse(triangle, &costMultiplier))
	{
		edgeCost = Distance(A, B) * costMultiplier;
		return true;
	}

	return false;
}
template<class TraversalCustomzier>
KY_INLINE bool AStarTraversal<TraversalCustomzier>::CanTraverseGraphEdgeAndGetCost(const NavGraphEdgeRawPtr& edge, const Vec3f& A, const Vec3f& B, KyFloat32& edgeCost)
{
	KyFloat32 costMultiplier = 1.f;
	if (m_traversalCustomizer.CanTraverse(edge, &costMultiplier))
	{
		edgeCost = Distance(A, B) * costMultiplier;
		return true;
	}

	return false;
}

template<class TraversalCustomzier>
KY_INLINE KyFloat32 AStarTraversal<TraversalCustomzier>::EvaluateCostToDest(const Vec3f& nodePosition) { return m_traversalCustomizer.GetHeuristicFromNodePosition(nodePosition); }

template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::InitializeContextAndStartNode(QueryUtils& queryUtils, const Vec3f& startPos3f, const NavGraphEdgePtr& startNavGraphEdgePtr)
{
	KY_ASSERT(startNavGraphEdgePtr.IsValid());
	KY_FORWARD_ERROR_NO_LOG(m_astarContext->Init(queryUtils));

	WorkingMemArray<AStarNode>& aStarNodes = m_astarContext->m_aStarNodes;

	//create a node for the start point
	aStarNodes.PushBack(AStarNode(AStarNodeIndex_Invalid, startPos3f, NodeType_FreePositionOnGraphEdge, m_astarContext->m_vertexRawPtrNodes.GetCount()));
	KY_ASSERT(AStarNodeIndex_StartNode == aStarNodes.GetCount() - 1);

	//Init the startNode values
	aStarNodes[AStarNodeIndex_StartNode].m_costFromStart = 0.f;
	aStarNodes[AStarNodeIndex_StartNode].m_estimatedCostToDest = EvaluateCostToDest(startPos3f);

	// Initialise indice memory
	const NavGraphVertexRawPtr endNavGraphVertexRawPtr = startNavGraphEdgePtr.GetRawPtr().GetEndNavGraphVertexRawPtr();
	KY_FORWARD_ERROR_NO_LOG(m_astarContext->m_vertexRawPtrNodes.PushBack(endNavGraphVertexRawPtr));

	AstarNodeIndexInGrid::NavGraphToNodeIndices* navGraphToNodeIndices = KY_NULL;
	if (KY_FAILED(m_astarContext->m_edgeIndexGrid.GetNavGraphToNodeIndices(endNavGraphVertexRawPtr, navGraphToNodeIndices)))
	{
		KY_LOG_WARNING( ("No more memory to store the AstarNodeIndex of a new encountered NavGraph"));
		return KY_ERROR;
	}

	return KY_SUCCESS;
}

template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::InitializeContextAndStartNode(QueryUtils& queryUtils, const Vec3f& startPos3f, const NavGraphVertexPtr& startNavGraphVertexPtr)
{
	KY_ASSERT(startNavGraphVertexPtr.IsValid());
	KY_LOG_ERROR_IF(startPos3f != startNavGraphVertexPtr.GetGraphVertexPosition(), ("Error, the input position is not the NavGraphVertex Position"));
	
	KY_FORWARD_ERROR_NO_LOG(m_astarContext->Init(queryUtils));

	WorkingMemArray<AStarNode>& aStarNodes = m_astarContext->m_aStarNodes;

	//create a node for the start point
	aStarNodes.PushBack(AStarNode(AStarNodeIndex_Invalid, startPos3f, NodeType_NavGraphVertex,  m_astarContext->m_vertexRawPtrNodes.GetCount()));
	KY_ASSERT(AStarNodeIndex_StartNode == aStarNodes.GetCount() - 1);

	//Init the startNode values
	aStarNodes[AStarNodeIndex_StartNode].m_costFromStart = 0.f;
	aStarNodes[AStarNodeIndex_StartNode].m_estimatedCostToDest = EvaluateCostToDest(startPos3f);

	// Initialise indice memory
	const NavGraphVertexRawPtr navGraphVertexRawPtr = startNavGraphVertexPtr.GetRawPtr();
	KY_FORWARD_ERROR_NO_LOG(m_astarContext->m_vertexRawPtrNodes.PushBack(navGraphVertexRawPtr));

	AstarNodeIndexInGrid::NavGraphToNodeIndices* navGraphToNodeIndices = KY_NULL;
	if (KY_FAILED(m_astarContext->m_edgeIndexGrid.GetNavGraphToNodeIndices(navGraphVertexRawPtr, navGraphToNodeIndices)))
	{
		KY_LOG_WARNING( ("No more memory to store the AstarNodeIndex of a new encountered NavGraph"));
		return KY_ERROR;
	}

	navGraphToNodeIndices->SetAStarNodeIndex(navGraphVertexRawPtr.GetNavGraphVertexIdx(), 0);
	return KY_SUCCESS;
}

template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::InitializeContextAndStartNode(QueryUtils& queryUtils, const Vec3f& startPos3f, const NavTrianglePtr& startTrianglePtr)
{
	KY_ASSERT(startTrianglePtr.IsValid());

	KY_FORWARD_ERROR_NO_LOG(m_astarContext->Init(queryUtils));

	WorkingMemArray<AStarNode>& aStarNodes = m_astarContext->m_aStarNodes;
	WorkingMemArray<NavHalfEdgeRawPtr>& aStarNodesEdgeRawPtrs = m_astarContext->m_edgeRawPtrNodes;

	const NavTriangleRawPtr startTriangleRawPtr = startTrianglePtr.GetRawPtr();
	// set the first edge of start triangle for startPoint
	const NavHalfEdgeRawPtr startEdgeRawPtr(startTriangleRawPtr.m_navFloorRawPtr, NavFloorBlob::NavTriangleIdxToFirstNavHalfEdgeIdx(startTriangleRawPtr.GetTriangleIdx()));
	//create a node for the start point
	aStarNodes.PushBack(AStarNode(AStarNodeIndex_Invalid, startPos3f, NodeType_FreePositionOnNavMesh, 0));
	KY_ASSERT(AStarNodeIndex_StartNode == aStarNodes.GetCount() - 1);
	KY_FORWARD_ERROR_NO_LOG(aStarNodesEdgeRawPtrs.PushBack(startEdgeRawPtr));

	//Init the startNode values
	aStarNodes[AStarNodeIndex_StartNode].m_costFromStart = 0.f;
	aStarNodes[AStarNodeIndex_StartNode].m_estimatedCostToDest = EvaluateCostToDest(startPos3f);

	return KY_SUCCESS;
}

template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::InitializeDestNode(const Vec3f& destPos, const NavTrianglePtr& destTrianglePtr)
{
	WorkingMemArray<AStarNode>& aStarNodes   = m_astarContext->m_aStarNodes;
	WorkingMemArray<NavHalfEdgeRawPtr>& aStarNodesEdgeRawPtrs = m_astarContext->m_edgeRawPtrNodes;

	// set the first edge of start triangle for startPoint
	const NavHalfEdgeRawPtr destEdgeRawPtr(destTrianglePtr.GetNavFloor(), NavFloorBlob::NavTriangleIdxToFirstNavHalfEdgeIdx(destTrianglePtr.GetTriangleIdx()));

	//create a node for the destination point
	KyUInt32 idxInRawPtr = aStarNodesEdgeRawPtrs.GetCount();
	aStarNodes.PushBack(AStarNode(AStarNodeIndex_Invalid, destPos, NodeType_FreePositionOnNavMesh, idxInRawPtr));
	KY_FORWARD_ERROR_NO_LOG(aStarNodesEdgeRawPtrs.PushBack(destEdgeRawPtr));

	return KY_SUCCESS;
}

template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::InitializeDestNode(const Vec3f& destPos, const NavGraphEdgePtr& destNavGraphEdgePtr)
{
	WorkingMemArray<AStarNode>&            aStarNodes              = m_astarContext->m_aStarNodes;
	WorkingMemArray<NavGraphVertexRawPtr>& aStarNodesVertexRawPtrs = m_astarContext->m_vertexRawPtrNodes;

	KyUInt32 idxInRawPtr = aStarNodesVertexRawPtrs.GetCount();
	aStarNodes.PushBack(AStarNode(AStarNodeIndex_Invalid, destPos, NodeType_FreePositionOnGraphEdge, idxInRawPtr));
	const NavGraphVertexRawPtr navGraphVertexRawPtr = destNavGraphEdgePtr.GetRawPtr().GetStartNavGraphVertexRawPtr();
	KY_FORWARD_ERROR_NO_LOG(aStarNodesVertexRawPtrs.PushBack(navGraphVertexRawPtr));

	// Initialise indice memory
	KY_FORWARD_ERROR_NO_LOG(m_astarContext->m_vertexRawPtrNodes.PushBack(navGraphVertexRawPtr));

	// make sure memory is allocated for this NavGraph but do not associated any AstarNodeIndex to any GraphVertex
	AstarNodeIndexInGrid::NavGraphToNodeIndices* navGraphToNodeIndices = KY_NULL;
	if (KY_FAILED(m_astarContext->m_edgeIndexGrid.GetNavGraphToNodeIndices(navGraphVertexRawPtr, navGraphToNodeIndices)))
	{
		KY_LOG_WARNING( ("No more memory to store the AstarNodeIndex of a new encountered NavGraph"));
		return KY_ERROR;
	}

	return KY_SUCCESS;
}

template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::InitializeDestNode(const NavGraphVertexPtr& destNavGraphVertexPtr)
{
	WorkingMemArray<AStarNode>&            aStarNodes              = m_astarContext->m_aStarNodes;
	WorkingMemArray<NavGraphVertexRawPtr>& aStarNodesVertexRawPtrs = m_astarContext->m_vertexRawPtrNodes;

	KyUInt32 idxInAStarNodes = aStarNodes.GetCount();
	KyUInt32 idxInRawPtr = aStarNodesVertexRawPtrs.GetCount();
	aStarNodes.PushBack(AStarNode(AStarNodeIndex_Invalid, destNavGraphVertexPtr.GetGraphVertexPosition(), NodeType_NavGraphVertex, idxInRawPtr));
	KY_FORWARD_ERROR_NO_LOG(aStarNodesVertexRawPtrs.PushBack(destNavGraphVertexPtr.GetRawPtr()));
	// Initialise indice memory
	const NavGraphVertexRawPtr navGraphVertexRawPtr = destNavGraphVertexPtr.GetRawPtr();

	AstarNodeIndexInGrid::NavGraphToNodeIndices* navGraphToNodeIndices = KY_NULL;
	if (KY_FAILED(m_astarContext->m_edgeIndexGrid.GetNavGraphToNodeIndices(navGraphVertexRawPtr, navGraphToNodeIndices)))
	{
		KY_LOG_WARNING( ("No more memory to store the AstarNodeIndex of a new encountered NavGraph"));
		return KY_ERROR;
	}

	// if the start Pos was associated the same navGraphVertexRawPtr, the corresponding astarNodeIndex has been set to 0
	// In this case we set it to the dest node index on purpose
	navGraphToNodeIndices->SetAStarNodeIndex(navGraphVertexRawPtr.GetNavGraphVertexIdx(), idxInAStarNodes);

	return KY_SUCCESS;
}

template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::InitializeInvalidDestNode()
{
	WorkingMemArray<AStarNode>&            aStarNodes              = m_astarContext->m_aStarNodes;
	aStarNodes.PushBack(AStarNode());
	return KY_SUCCESS;
}


template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::ExploreAllNodesInNavGraphEdge(ActiveData* /*activeData*/, const Vec3f& posOnEdge, const NavGraphEdgeRawPtr& navGraphEdgeRawPtr,
	AStarNodeIndex currentNodeIndex, NavGraphEdgeDirection m_navGraphEdgePathfindMode)
{
	const NavGraphVertexRawPtr endNavGraphVertexRawPtr = navGraphEdgeRawPtr.GetEndNavGraphVertexRawPtr();

	AstarNodeIndexInGrid::NavGraphToNodeIndices* navGraphToNodeIndices = KY_NULL;
	m_astarContext->m_edgeIndexGrid.GetNavGraphToNodeIndices(endNavGraphVertexRawPtr, navGraphToNodeIndices);
	KY_ASSERT(navGraphToNodeIndices);
	
	{
		KyFloat32 cost = 0.f;
		if (CanTraverseGraphEdgeAndGetCost(navGraphEdgeRawPtr, posOnEdge, endNavGraphVertexRawPtr.GetGraphVertexPosition(), cost))
		{
			const AStarNodeIndex neighborNodeIndex = navGraphToNodeIndices->GetAStarNodeIndex(endNavGraphVertexRawPtr.GetNavGraphVertexIdx());
			KY_FORWARD_ERROR_NO_LOG(OpenOrUpdateGraphVertex(endNavGraphVertexRawPtr, neighborNodeIndex, currentNodeIndex, cost));
			KY_FORWARD_ERROR_NO_LOG(m_traversalCustomizer.OnNavGraphEdgeExplored(m_astarContext, navGraphEdgeRawPtr, currentNodeIndex));
		}
	}

	if (m_navGraphEdgePathfindMode != NAVGRAPHEDGE_BIDIRECTIONAL)
		return KY_SUCCESS;

	NavGraphEdgeRawPtr oppositeNavGraphEdgeRawPtr =  navGraphEdgeRawPtr.GetOppositeNavGraphEdgeRawPtr();
	if(oppositeNavGraphEdgeRawPtr.IsValid() == false)
		return KY_SUCCESS;

	KY_ASSERT(oppositeNavGraphEdgeRawPtr.GetEndNavGraphVertexRawPtr() == navGraphEdgeRawPtr.GetStartNavGraphVertexRawPtr());
	const NavGraphVertexRawPtr oppositeEndNavGraphVertexRawPtr = navGraphEdgeRawPtr.GetStartNavGraphVertexRawPtr();

	{
		KyFloat32 cost = 0.f;
		if (CanTraverseGraphEdgeAndGetCost(oppositeNavGraphEdgeRawPtr, posOnEdge, oppositeEndNavGraphVertexRawPtr.GetGraphVertexPosition(), cost))
		{
			const AStarNodeIndex neighborNodeIndex = navGraphToNodeIndices->GetAStarNodeIndex(oppositeEndNavGraphVertexRawPtr.GetNavGraphVertexIdx());
			KY_FORWARD_ERROR_NO_LOG(OpenOrUpdateGraphVertex(oppositeEndNavGraphVertexRawPtr, neighborNodeIndex, currentNodeIndex, cost));
			KY_FORWARD_ERROR_NO_LOG(m_traversalCustomizer.OnNavGraphEdgeExplored(m_astarContext, oppositeNavGraphEdgeRawPtr, currentNodeIndex));
		}
	}


	return KY_SUCCESS;
}

template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::VisitNode(QueryUtils& queryUtils, TraversalVisitNodeContext& visitNodeContext)
{
	ActiveData* activeData = queryUtils.m_database->GetActiveData();
	visitNodeContext.Clear();
	KY_UNUSED(activeData);

	// retrieve the node with the lower cost from the binary heap
	AStarNodeIndex lowerCostNodeIndex = AStarNodeIndex_Invalid;
	m_astarContext->m_traversalBinHeap.ExtractFirst(lowerCostNodeIndex);

	// check the behavior of the binaryHeap
	KY_ASSERT(m_astarContext->m_aStarNodes[lowerCostNodeIndex].m_indexInBinaryHeap == IndexInBinHeap_Removed);
	// we never put the startNode in the BinHeap
	KY_ASSERT(lowerCostNodeIndex != AStarNodeIndex_StartNode || m_astarContext->m_aStarNodes[AStarNodeIndex_StartNode].GetNodeType() == NodeType_NavGraphVertex);

	visitNodeContext.m_visitedNodeIndex = lowerCostNodeIndex;

	// check if we have to stop the traversal on this node
	if (m_traversalCustomizer.ShouldVisitNode(m_astarContext, lowerCostNodeIndex) == false)
		return KY_SUCCESS;

	// We do not reach the dest node, go through the neighbors of the node
	AStarNode& lowerCostNode = m_astarContext->m_aStarNodes[lowerCostNodeIndex];
	if ((lowerCostNode.GetNodeType() != NodeType_AbstractGraphNode) &&
		(m_traversalCustomizer.IsNodeOutsidePropagationBounds(m_astarContext, lowerCostNodeIndex)))
		return KY_SUCCESS;

	Kaim::PathNodeType pathNodeType = lowerCostNode.GetNodeType();
	switch (pathNodeType)
	{
	case NodeType_NavMeshEdge :
		{
			// retrieve the corresponding NavHalfEdgeRawPtr and its pair NavHalfEdgeRawPtr
			NavHalfEdgeRawPtr currentEdgeRawPtr = m_astarContext->m_edgeRawPtrNodes[lowerCostNode.GetIdxOfRawPtrData()];
			NavHalfEdgeRawPtr pairHalfEdgeRawPtr;

			const bool isHalfEdgeCrossable = currentEdgeRawPtr.IsHalfEdgeCrossable<TraverseLogic>(
				m_traversalCustomizer.GetTraverseLogicUserData(), pairHalfEdgeRawPtr);
			KY_UNUSED(isHalfEdgeCrossable);

#if defined(KY_BUILD_DEBUG)
			if (m_traversalParams->m_abstractGraphTraversalMode == PATHFINDER_DO_NOT_TRAVERSE_ABSTRACT_GRAPHS)
			{
				KY_ASSERT(isHalfEdgeCrossable); // no node should have been created for a non crossable edge
				// the current and its paired edge must have the same AStarNodeIndex
				AstarNodeIndexInGrid::NavFloorToNodeIndices* navFloorToNodeIndices = m_astarContext->m_edgeIndexGrid.GetNavFloorToNodeIndices_Unsafe(pairHalfEdgeRawPtr.m_navFloorRawPtr);
				KY_ASSERT(navFloorToNodeIndices->GetAStarNodeIndex(pairHalfEdgeRawPtr.GetHalfEdgeIdx()) == lowerCostNodeIndex);
			}
#endif

			KY_FORWARD_ERROR_NO_LOG(ExploreNeighborsOfHalfEdgeNode(activeData, lowerCostNodeIndex, currentEdgeRawPtr,
				pairHalfEdgeRawPtr, lowerCostNode.m_nodePosition, typename TraverseLogic::CanEnterNavTagMode()));

			break;
		}
	case NodeType_NavGraphVertex :
		{
			KY_FORWARD_ERROR_NO_LOG(ExploreNeihgborsOfGraphVertexNode(activeData, lowerCostNodeIndex, visitNodeContext.m_visitedGraphVertexRawPtr));
			break;
		}
	case NodeType_AbstractGraphNode :
		{
			AbstractGraphNodeRawPtr currentAbstractNodeRawPtr = m_astarContext->m_abstractRawPtrNodes[lowerCostNode.GetIdxOfRawPtrData()];
			AbstractGraphNodeRawPtr pairedAbstractNodeRawPtr = currentAbstractNodeRawPtr.GetPairedAbstractGraphNodeRawPtr();

#if defined(KY_BUILD_DEBUG)
			KY_ASSERT(pairedAbstractNodeRawPtr.IsValid() == true); // no node should have been created for an unpaired node
			// the current and its paired node must have the same AStarNodeIndex
			AstarNodeIndexInGrid::AbstractGraphToNodeIndices* abstractGraphToNodeIndices = m_astarContext->m_edgeIndexGrid.GetAbstractGraphToNodeIndices_Unsafe(pairedAbstractNodeRawPtr);
			KY_ASSERT(abstractGraphToNodeIndices->GetAStarNodeIndex(pairedAbstractNodeRawPtr.m_nodeIdx) == lowerCostNodeIndex);
#endif

			KY_FORWARD_ERROR_NO_LOG(ExploreNeihgborsOfAbstractGraphNode(lowerCostNodeIndex, currentAbstractNodeRawPtr));
			KY_FORWARD_ERROR_NO_LOG(ExploreNeihgborsOfAbstractGraphNode(lowerCostNodeIndex, pairedAbstractNodeRawPtr));
			break;
		}
	case NodeType_FreePositionOnNavMesh   :
	case NodeType_FreePositionOnGraphEdge :
		{
			break;
		}
	default:
		{
			KY_ASSERT(false);
			break;
		}
	}

	// node visited !
	return KY_SUCCESS;
}

template<class TraversalCustomzier>
KY_INLINE KyResult AStarTraversal<TraversalCustomzier>::ExploreNeighborsOfHalfEdgeNode(ActiveData* activeData, AStarNodeIndex lowerCostNodeIndex, const NavHalfEdgeRawPtr& currentEdgeRawPtr, const NavHalfEdgeRawPtr& pairHalfEdgeRawPtr, const Vec3f& /*nodePosition*/, const LogicDoNotUseCanEnterNavTag&)
{
	if (m_traversalParams->m_abstractGraphTraversalMode == PATHFINDER_TRAVERSE_ABSTRACT_GRAPHS)
	{
		if (pairHalfEdgeRawPtr.IsValid() == true)
			KY_FORWARD_ERROR_NO_LOG(ExploreNeighborsOfHalfEdgeNode(activeData, lowerCostNodeIndex, pairHalfEdgeRawPtr))
		
		KY_FORWARD_ERROR_NO_LOG(ExploreAbstractGraphNodesOnNavHalEdgeRawPtr(currentEdgeRawPtr, lowerCostNodeIndex))
	}
	else
	{
		if (pairHalfEdgeRawPtr.IsValid() == true)
			KY_FORWARD_ERROR_NO_LOG(ExploreNeighborsOfHalfEdgeNode(activeData, lowerCostNodeIndex, pairHalfEdgeRawPtr))
	}	

	KY_FORWARD_ERROR_NO_LOG(ExploreNeighborsOfHalfEdgeNode(activeData, lowerCostNodeIndex, currentEdgeRawPtr));
	
	return KY_SUCCESS;
}

template<class TraversalCustomzier>
KY_INLINE KyResult AStarTraversal<TraversalCustomzier>::ExploreNeighborsOfHalfEdgeNode(ActiveData* activeData, AStarNodeIndex lowerCostNodeIndex, const NavHalfEdgeRawPtr& currentEdgeRawPtr, const NavHalfEdgeRawPtr& pairHalfEdgeRawPtr, const Vec3f& nodePosition, const LogicDoUseCanEnterNavTag&)
{
	bool  pairEdgeIsValid = pairHalfEdgeRawPtr.IsValid();
	if (currentEdgeRawPtr.IsHalfEdgeOneWayCrossable<TraverseLogic>(m_traversalCustomizer.m_traverseLogicUserData, &nodePosition))
	{
		KY_FORWARD_ERROR_NO_LOG(ExploreNeighborsOfHalfEdgeNode(activeData, lowerCostNodeIndex, pairHalfEdgeRawPtr));
		
		if (m_traversalParams->m_abstractGraphTraversalMode == PATHFINDER_TRAVERSE_ABSTRACT_GRAPHS)
			KY_FORWARD_ERROR_NO_LOG(ExploreAbstractGraphNodesOnNavHalEdgeRawPtr(currentEdgeRawPtr, lowerCostNodeIndex));
	}

	if (pairEdgeIsValid == true)
	{
		if (pairHalfEdgeRawPtr.IsHalfEdgeOneWayCrossable<TraverseLogic>(m_traversalCustomizer.m_traverseLogicUserData, &nodePosition))
			KY_FORWARD_ERROR_NO_LOG(ExploreNeighborsOfHalfEdgeNode(activeData, lowerCostNodeIndex, currentEdgeRawPtr));
	}

	return KY_SUCCESS;
}

template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::ExploreNeighborsOfHalfEdgeNode(ActiveData* activeData, AStarNodeIndex currentNodeIndex,
	const NavHalfEdgeRawPtr& currentEdgeRawPtr)
{
	AstarNodeIndexInGrid& edgeIndexGrid = m_astarContext->m_edgeIndexGrid;
	Vec3f vertices[3];

	AstarNodeIndexInGrid::NavFloorToNodeIndices* const navFloorToNodeIndices = edgeIndexGrid.GetNavFloorToNodeIndices_Unsafe(currentEdgeRawPtr.m_navFloorRawPtr);

	const NavTriangleRawPtr triangleRawPtr    (currentEdgeRawPtr.m_navFloorRawPtr, NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(currentEdgeRawPtr.GetHalfEdgeIdx()));
	const NavHalfEdgeRawPtr nextHalfEdgeRawPtr(currentEdgeRawPtr.m_navFloorRawPtr, NavFloorBlob::NavHalfEdgeIdxToNextNavHalfEdgeIdx(currentEdgeRawPtr.GetHalfEdgeIdx()));
	const NavHalfEdgeRawPtr prevHalfEdgeRawPtr(currentEdgeRawPtr.m_navFloorRawPtr, NavFloorBlob::NavHalfEdgeIdxToPrevNavHalfEdgeIdx(currentEdgeRawPtr.GetHalfEdgeIdx()));

	AStarNodeIndex nextNodeIndex = navFloorToNodeIndices->GetAStarNodeIndex(nextHalfEdgeRawPtr.GetHalfEdgeIdx());
	AStarNodeIndex prevNodeIndex = navFloorToNodeIndices->GetAStarNodeIndex(prevHalfEdgeRawPtr.GetHalfEdgeIdx());

	// compute the indices of the edges in the triangle (0, 1 or 2) to identify the start and end vertex of each edge
	const KyUInt32 currentIdxInTriangle = NavFloorBlob::NavHalfEdgeIdxToHalfEdgeNumberInTriangle( currentEdgeRawPtr.GetHalfEdgeIdx());
	const KyUInt32 nextIdxInTriangle    = NavFloorBlob::NavHalfEdgeIdxToHalfEdgeNumberInTriangle(nextHalfEdgeRawPtr.GetHalfEdgeIdx());
	const KyUInt32 prevIdxInTriangle    = NavFloorBlob::NavHalfEdgeIdxToHalfEdgeNumberInTriangle(prevHalfEdgeRawPtr.GetHalfEdgeIdx());

	if (nextNodeIndex == AStarNodeIndex_Invalid || prevNodeIndex == AStarNodeIndex_Invalid)
		triangleRawPtr.GetVerticesPos3f(vertices[0], vertices[1], vertices[2]);

	KY_FORWARD_ERROR_NO_LOG(OpenOrUpdateHalfEdge(activeData, nextHalfEdgeRawPtr, nextNodeIndex, vertices[nextIdxInTriangle], vertices[prevIdxInTriangle], currentNodeIndex));
	KY_FORWARD_ERROR_NO_LOG(OpenOrUpdateHalfEdge(activeData, prevHalfEdgeRawPtr, prevNodeIndex, vertices[prevIdxInTriangle], vertices[currentIdxInTriangle], currentNodeIndex));
	KY_FORWARD_ERROR_NO_LOG(ExploreGraphVerticesInTriangle(triangleRawPtr, currentNodeIndex));

	KY_FORWARD_ERROR_NO_LOG(m_traversalCustomizer.OnNavTriangleExplored(m_astarContext, triangleRawPtr, currentNodeIndex));

	return KY_SUCCESS;
}


template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::OpenOrUpdateHalfEdge(ActiveData* activeData, const NavHalfEdgeRawPtr& halfEdgeRawPtr,
	AStarNodeIndex halfEdgeNodeIndex, const Vec3f& startPosOfEdge, const Vec3f& endPosOfEdge, AStarNodeIndex currentNodeIndex)
{
	WorkingMemArray<AStarNode>& aStarNodes = m_astarContext->m_aStarNodes;

	if (halfEdgeNodeIndex == AStarNodeIndex_Invalid)
	{
		NavHalfEdgeRawPtr pairHalfEdgeOfNewNode;
		bool shouldOpenHalfEdgeNodeOnNavMesh = ShouldOpenHalfEdgeNode(halfEdgeRawPtr, pairHalfEdgeOfNewNode);
		if (shouldOpenHalfEdgeNodeOnNavMesh == true)
		{
			// no index for this Edge -> it has never been encoutered, it is a new node
			KY_FORWARD_ERROR_NO_LOG(CreateNewHalfEdgeNode(activeData, halfEdgeRawPtr, pairHalfEdgeOfNewNode, startPosOfEdge, endPosOfEdge, currentNodeIndex));
		}
		else if (m_traversalParams->m_abstractGraphTraversalMode == PATHFINDER_TRAVERSE_ABSTRACT_GRAPHS)
		{
			// if has a paired edge but it is not crossable (i.e. pairHalfEdgeOfNewNode is valid but ShouldOpenHalfEdgeNode returned false), do not use AbstractGraphNodes
			// Otherwise, if no pair edge, create an AStar node for this NavHalfEdge anyway in order to explore AbstractGraphs
			// because it is a situation, where pairedHalfEdge would come from a neighbor NavData which is not loaded, or a dynamic change in the neighbor NavData
			if (pairHalfEdgeOfNewNode.IsValid() == false)
			{
				// no need to look for the abstractGraphNode, it is useless, simply open the halfEdgeNode and AbstractGraphs would be explored if there are any
				// so yes, we could open halfEdge on the border for nothing, so do it only if there are AbstractGraph loaded
				if (activeData->m_database->m_abstractGraphCellGrid->m_abstractGraphs.IsEmpty() == false)
					KY_FORWARD_ERROR_NO_LOG(CreateNewHalfEdgeNode(activeData, halfEdgeRawPtr, pairHalfEdgeOfNewNode, startPosOfEdge, endPosOfEdge, currentNodeIndex));
			}
		}
	}
	else
	{
		if (halfEdgeNodeIndex != aStarNodes[currentNodeIndex].GetIndexOfPredecessor())
		{
			// A node has already be created for this halfEdge -> we update it if needed !
			const NavTriangleRawPtr triangle(halfEdgeRawPtr.GetNavFloor(), NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(halfEdgeRawPtr.GetHalfEdgeIdx()));
			KyFloat32 edgeCost = 0.f;
			if (CanTraverseNavTriangleAndGetCost(triangle, aStarNodes[currentNodeIndex].m_nodePosition, aStarNodes[halfEdgeNodeIndex].m_nodePosition, edgeCost))
			{
				KY_FORWARD_ERROR_NO_LOG(UpdateOpenedOrClosedNode(halfEdgeNodeIndex, currentNodeIndex, edgeCost));
			}
		}
	}

	return KY_SUCCESS;
}

template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::CreateNewHalfEdgeNode(ActiveData* activeData,
	const NavHalfEdgeRawPtr& halfEdge, const NavHalfEdgeRawPtr& pairHalfEdge,
	const Vec3f& startPosOfEdge, const Vec3f& endPosOfEdge, AStarNodeIndex predecessorNodeIndex)
{
	WorkingMemArray<AStarNode>& aStarNodes                    = m_astarContext->m_aStarNodes;
	WorkingMemArray<NavHalfEdgeRawPtr>& aStarNodesEdgeRawPtrs = m_astarContext->m_edgeRawPtrNodes;
	AstarNodeIndexInGrid& edgeIndexGrid                       = m_astarContext->m_edgeIndexGrid;

	PathNodeType pathNodeType = NodeType_NavMeshEdge;
	Vec3f nodePos = m_traversalCustomizer.ComputeNodePositionOnHalfEdge(m_astarContext, startPosOfEdge, endPosOfEdge, predecessorNodeIndex);
	if (pairHalfEdge.IsValid() == false)
	{
		// if no pairHalfEdge, this halfEdge was opened due to abstractGraphNode lying on it
		// move slightly the nodePos away from the edge and preferably still in the triangle
		NavHalfEdgeRawPtr prevHalfEdge;
		halfEdge.GetPrevHalfEdgeRawPtr(prevHalfEdge);
		const Vec3f thirdVertexOfTriangle = prevHalfEdge.GetStartVertexPos3f();
		KyFloat32 margin = activeData->m_database->GetRayCanGoMarginFloat(PathMargin);

		Vec3f shiftVector = (thirdVertexOfTriangle - nodePos);
		// ensure margin doesn't move the pos out of the triangle, otherwise it could be out of NavMesh !
		KyFloat32 maxSquareMargin = shiftVector.GetSquareLength();
		if (maxSquareMargin < margin*margin)
			margin = sqrtf(maxSquareMargin) * 0.5f; // use half of the possible length
		shiftVector.Normalize();
		shiftVector *= margin;
		nodePos += shiftVector;
	}

	KyFloat32 edgeCost = 0.f;
	const NavTriangleRawPtr triangle(halfEdge.GetNavFloor(), NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(halfEdge.GetHalfEdgeIdx()));
	if (CanTraverseNavTriangleAndGetCost(triangle, aStarNodes[predecessorNodeIndex].m_nodePosition, nodePos, edgeCost) == false)
		return KY_SUCCESS;

	KY_FORWARD_ERROR_NO_LOG(m_astarContext->CheckAstarNodeArrayMemory()); // check for memory to create a new node
	KY_FORWARD_ERROR_NO_LOG(m_astarContext->CheckNavHalfEdgeRawPtrArrayMemory()); // check for memory to create a new node
	KY_FORWARD_ERROR_NO_LOG(m_astarContext->CheckTraversalBinaryHeapMemory()); // check for memory to insert a newElement in the binary heap

	// the new node Index
	AStarNodeIndex newNodeIndex = (AStarNodeIndex)aStarNodes.GetCount();

	aStarNodes.PushBack_UnSafe(AStarNode(predecessorNodeIndex, nodePos, pathNodeType, aStarNodesEdgeRawPtrs.GetCount()));
	aStarNodesEdgeRawPtrs.PushBack(halfEdge);

	AstarNodeIndexInGrid::NavFloorToNodeIndices* navFloorToNodeIndices = edgeIndexGrid.GetNavFloorToNodeIndices_Unsafe(halfEdge.m_navFloorRawPtr);
	// set the index of the current edge
	navFloorToNodeIndices->SetAStarNodeIndex(halfEdge.GetHalfEdgeIdx(), newNodeIndex);

	if (pairHalfEdge.IsValid())
	{
		if (halfEdge.m_navFloorRawPtr != pairHalfEdge.m_navFloorRawPtr)
		{
			// retrieve node indices of the navfloor that contains the pair halfEdge
			if (KY_FAILED(edgeIndexGrid.GetNavFloorToNodeIndices(activeData, pairHalfEdge.m_navFloorRawPtr, navFloorToNodeIndices)))
			{
				KY_LOG_WARNING( ("This traversal reached the maximum size of propagation memory in the activeData"));
				return KY_ERROR;
			}
		}
		// set the index to the pair edge also
		navFloorToNodeIndices->SetAStarNodeIndex(pairHalfEdge.GetHalfEdgeIdx(), newNodeIndex);
	}

	AStarNode& newNode = aStarNodes[newNodeIndex];
	// compute first cost of the node
	AStarNode& predecessorNodeReload = aStarNodes[predecessorNodeIndex]; // reload because of a possible resize of the astarNodeArray

	// compute the estimated cost to dest once and for all
	newNode.m_estimatedCostToDest = EvaluateCostToDest(nodePos);
	newNode.m_costFromStart = predecessorNodeReload.m_costFromStart + edgeCost;
	m_astarContext->m_traversalBinHeap.Insert(newNodeIndex);

	return KY_SUCCESS;
}

template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::ExploreGraphVerticesInTriangle(const NavTriangleRawPtr& triangleRawPtr,
	AStarNodeIndex currentNodeIndex)
{
	AstarNodeIndexInGrid& edgeIndexGrid = m_astarContext->m_edgeIndexGrid;
	Vec3f nodePosition = m_astarContext->m_aStarNodes[currentNodeIndex].m_nodePosition;

	NavFloor* navFloor = triangleRawPtr.GetNavFloor();
	NavGraphLinkCollection& navGraphLinks = navFloor->GetNavFloorToNavGraphLinks()->m_navGraphLinks;

	for (KyUInt32 linkIdx = 0 ; linkIdx < navGraphLinks.GetCount(); ++linkIdx)
	{
		NavGraphLink* navGraphLink = navGraphLinks[linkIdx];
		if (triangleRawPtr.GetTriangleIdx() != navGraphLink->m_navTriangleRawPtr.GetTriangleIdx())
			continue;

		const NavGraphVertexRawPtr& navGraphVertexRawPtr = navGraphLink->m_navGraphVertexRawPtr;

		// declare navGraphToNodeIndices in a local scope to be sure it won't be used after a memory resize
		AstarNodeIndexInGrid::NavGraphToNodeIndices* navGraphToNodeIndices = KY_NULL;
		if (KY_FAILED(edgeIndexGrid.GetNavGraphToNodeIndices(navGraphVertexRawPtr, navGraphToNodeIndices)))
		{
			KY_LOG_WARNING( ("No more memory to store the AstarNodeIndex of a new encountered NavGraph"));
			return KY_ERROR;
		}

		const AStarNodeIndex vertexNodeIndex = navGraphToNodeIndices->GetAStarNodeIndex(navGraphVertexRawPtr.GetNavGraphVertexIdx());
		if (vertexNodeIndex != currentNodeIndex)
		{
			KyFloat32 cost = 0.f;
			if (CanTraverseNavTriangleAndGetCost(triangleRawPtr, nodePosition, navGraphVertexRawPtr.GetGraphVertexPosition(), cost))
			{
				KY_FORWARD_ERROR_NO_LOG(OpenOrUpdateGraphVertex(navGraphVertexRawPtr, vertexNodeIndex, currentNodeIndex, cost));
			}
		}
	}

	return KY_SUCCESS;
}

template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::ExploreNeihgborsOfGraphVertexNode(ActiveData* activeData, AStarNodeIndex currentNodeIndex, NavGraphVertexRawPtr& visitedNavGraphVertexRawPtr)
{
	WorkingMemArray<AStarNode>& aStarNodes   = m_astarContext->m_aStarNodes;
	AstarNodeIndexInGrid& edgeIndexGrid = m_astarContext->m_edgeIndexGrid;

	const KyUInt32 indexOfvertexRawPtr = aStarNodes[currentNodeIndex].GetIdxOfRawPtrData();

	// this variable can be use till a create a new node because it may cause a memory reallocation which will cause the reference to be invalid
	// that is why we store a copy of the NavGraphRawPtr and the VertexIdx
	NavGraphVertexRawPtr& navGraphVertexRawPtr = m_astarContext->m_vertexRawPtrNodes[indexOfvertexRawPtr];
	visitedNavGraphVertexRawPtr = navGraphVertexRawPtr;
	const NavGraphRawPtr navGraphRawPtr        = navGraphVertexRawPtr.m_navGraphRawPtr;
	const NavGraphVertexIdx navGraphVertexIdx  = navGraphVertexRawPtr.GetNavGraphVertexIdx();

	// we are processing neighbors of a node that exists, so memory for indices of its navGraph should have been allocated yet -> unsafe function
	// WE SHOULD NOT RESIZE THE GRID WHEN BROWSING THE VERTICES OF THE SAME GRAPH
	AstarNodeIndexInGrid::NavGraphToNodeIndices* const navGraphToNodeIndices = edgeIndexGrid.GetNavGraphToNodeIndices_Unsafe(navGraphVertexRawPtr);

	const NavGraphBlob& navGraphBlob = *navGraphRawPtr.GetNavGraphBlob();

	const NavGraphVertex& navGraphVertex = navGraphBlob.GetNavGraphVertex(navGraphVertexIdx);
	const KyUInt32 neighborCount        = navGraphVertex.GetNeighborVertexCount();

	// browse the graphVertex neighbor nodes
	for(KyUInt32 i = 0; i < neighborCount; ++i)
	{
		const NavGraphVertexIdx neighborVertexIdx = navGraphVertex.GetNeighborVertexIdx(i);
		NavGraphEdgeRawPtr edgeRawPtr(visitedNavGraphVertexRawPtr, i);

		KyFloat32 cost = 0.f;
		if (CanTraverseGraphEdgeAndGetCost(edgeRawPtr, navGraphVertex.m_position, navGraphBlob.GetNavGraphVertexPosition(neighborVertexIdx), cost) == false)
			continue;

		const AStarNodeIndex neighborNodeIndex = navGraphToNodeIndices->GetAStarNodeIndex(neighborVertexIdx);

		KY_FORWARD_ERROR_NO_LOG(OpenOrUpdateGraphVertex(NavGraphVertexRawPtr(navGraphRawPtr, neighborVertexIdx), neighborNodeIndex, currentNodeIndex, cost));

		NavGraphEdgeRawPtr navGraphEdgeRawPtr(navGraphVertexRawPtr, i);
		KY_FORWARD_ERROR_NO_LOG(m_traversalCustomizer.OnNavGraphEdgeExplored(m_astarContext, navGraphEdgeRawPtr, currentNodeIndex));
	}


	// Check if this GraphVertex is connected to a triangle node
	const GraphVertexData& vertexData = navGraphRawPtr.GetNavGraph()->GetGraphVertexData(navGraphVertexIdx);
	NavGraphLink* navGraphLink = vertexData.m_navGraphLink;

	if (navGraphLink == KY_NULL || navGraphLink->IsValid() == false)
		// vertex is not linked to the navMesh
		return KY_SUCCESS;

	const NavTriangleRawPtr& triangleRawPtr = navGraphLink->m_navTriangleRawPtr;

	if (CanTraverseNavTriangle(triangleRawPtr) == false)
		return KY_SUCCESS;

	ExploreAllNodesInTriangle(activeData, triangleRawPtr, currentNodeIndex);

	return KY_SUCCESS;
}

template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::ExploreAbstractGraphNodesOnNavHalEdgeRawPtr( 
	const NavHalfEdgeRawPtr& navHalfEdgeRawPtr, AStarNodeIndex currentNodeIndex)
{
	AbstractGraphNodeRawPtr abstractGraphNodeRawPtr;
	if (abstractGraphNodeRawPtr.InitFromNavHalfEdgeRawPtr(navHalfEdgeRawPtr) == KY_ERROR)
		return KY_SUCCESS;

	if (abstractGraphNodeRawPtr.GetAbstractGraphNodeLink().CanTraverse() == false)
		return KY_SUCCESS;

	// declare abstractToNodeIndices in a local scope to be sure it won't be used after a memory resize
	AstarNodeIndexInGrid& edgeIndexGrid = m_astarContext->m_edgeIndexGrid;
	AstarNodeIndexInGrid::AbstractGraphToNodeIndices* abstractGraphToNodeIndices = KY_NULL;
	if (KY_FAILED(edgeIndexGrid.GetAbstractGraphToNodeIndices(abstractGraphNodeRawPtr, abstractGraphToNodeIndices)))
	{
		KY_LOG_WARNING( ("No more memory to store the AstarNodeIndex of a new encountered NavGraph"));
		return KY_ERROR;
	}

	const AStarNodeIndex astarNodeIndex = abstractGraphToNodeIndices->GetAStarNodeIndex(abstractGraphNodeRawPtr.m_nodeIdx);
	if (astarNodeIndex != currentNodeIndex)
	{
		const KyFloat32 cost = 0.f;
		KY_FORWARD_ERROR_NO_LOG(OpenOrUpdateAbstractGraphNode(abstractGraphNodeRawPtr, astarNodeIndex, currentNodeIndex, cost));
	}
	return KY_SUCCESS; // node found
}


template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::ExploreNeihgborsOfAbstractGraphNode(AStarNodeIndex currentNodeIndex, const AbstractGraphNodeRawPtr& currentAbstractNodeRawPtr)
{
	if (currentAbstractNodeRawPtr.IsValid() == false)
		return KY_SUCCESS;

	if (currentAbstractNodeRawPtr.GetAbstractGraphNodeLink().CanTraverse() == false)
		return KY_SUCCESS;

	AstarNodeIndexInGrid& edgeIndexGrid = m_astarContext->m_edgeIndexGrid;
	
	// browse the neighbor nodes
// 	KyUInt32 neighborCount = currentAbstractNodeRawPtr.GetNeighborNodeCount();
// 	const CompactAbstractGraphNodeIdx* neighborNodes = currentAbstractNodeRawPtr.GetNeighborNodes();
// 	const KyFloat32* neighborCosts = currentAbstractNodeRawPtr.GetNeighborCosts();
// 	for(KyUInt32 i = 0; i < neighborCount; ++i)
// 	{
// 		AbstractGraphNodeIdx neighborAbstractNodeIdx = neighborNodes[i];
// 		KyFloat32 neighborCost = neighborCosts[i];
// 	
// 		// must be retrieve since paired node could are in other graphs and could reallocate!
// 		AstarNodeIndexInGrid::AbstractGraphToNodeIndices* const abstractGraphToNodeIndices = 
// 			edgeIndexGrid.GetAbstractGraphToNodeIndices_Unsafe(currentAbstractNodeRawPtr);
// 
// 		AStarNodeIndex neighborAstarNodeIndex = abstractGraphToNodeIndices->GetAStarNodeIndex(neighborAbstractNodeIdx);
// 
// 		AbstractGraphNodeRawPtr neighborAbstractNodeRawPtr(currentAbstractNodeRawPtr.m_abstractGraph, neighborAbstractNodeIdx);
// 		KY_FORWARD_ERROR_NO_LOG(OpenOrUpdateAbstractGraphNode(neighborAbstractNodeRawPtr, neighborAstarNodeIndex, currentNodeIndex, neighborCost));
// 	}

// 	KyUInt32 neighborCount = currentAbstractNodeRawPtr.m_abstractGraph->GetNodeCount();
// 	const KyFloat32* neighborCosts = currentAbstractNodeRawPtr.GetNeighborCosts();
// 	for(KyUInt32 neighborAbstractNodeIdx = 0; neighborAbstractNodeIdx < neighborCount; ++neighborAbstractNodeIdx)
// 	{
// 		KyFloat32 neighborCost = neighborCosts[neighborAbstractNodeIdx];
// 		if (neighborCost < 0.f)
// 			continue;
// 
// 		// must be retrieve since paired node could are in other graphs and could reallocate!
// 		AstarNodeIndexInGrid::AbstractGraphToNodeIndices* const abstractGraphToNodeIndices = 
// 			edgeIndexGrid.GetAbstractGraphToNodeIndices_Unsafe(currentAbstractNodeRawPtr);
// 
// 		AStarNodeIndex neighborAstarNodeIndex = abstractGraphToNodeIndices->GetAStarNodeIndex(neighborAbstractNodeIdx);
// 
// 		AbstractGraphNodeRawPtr neighborAbstractNodeRawPtr(currentAbstractNodeRawPtr.m_abstractGraph, neighborAbstractNodeIdx);
// 		KY_FORWARD_ERROR_NO_LOG(OpenOrUpdateAbstractGraphNode(neighborAbstractNodeRawPtr, neighborAstarNodeIndex, currentNodeIndex, neighborCost));
// 	}

	KyUInt32 neighborCount = currentAbstractNodeRawPtr.m_abstractGraph->GetNodeCount();
	for(KyUInt32 neighborAbstractNodeIdx = 0; neighborAbstractNodeIdx < neighborCount; ++neighborAbstractNodeIdx)
	{
		KyFloat32 neighborCost = currentAbstractNodeRawPtr.GetNeighborCost(neighborAbstractNodeIdx);
		if (neighborCost < 0.f)
			continue;

		// must be retrieve since paired node could are in other graphs and could reallocate!
		AstarNodeIndexInGrid::AbstractGraphToNodeIndices* const abstractGraphToNodeIndices = 
			edgeIndexGrid.GetAbstractGraphToNodeIndices_Unsafe(currentAbstractNodeRawPtr);

		AStarNodeIndex neighborAstarNodeIndex = abstractGraphToNodeIndices->GetAStarNodeIndex(neighborAbstractNodeIdx);

		AbstractGraphNodeRawPtr neighborAbstractNodeRawPtr(currentAbstractNodeRawPtr.m_abstractGraph, neighborAbstractNodeIdx);
		KY_FORWARD_ERROR_NO_LOG(OpenOrUpdateAbstractGraphNode(neighborAbstractNodeRawPtr, neighborAstarNodeIndex, currentNodeIndex, neighborCost));
	}

	// Check if connected to destination NavMesh
	if (m_traversalCustomizer.ShouldOpenNavTriangleUnderAbstractGraphNode(currentAbstractNodeRawPtr) == false)
		return KY_SUCCESS;

	if (currentAbstractNodeRawPtr.GetAbstractGraphNodeLink().m_navMeshLinkStatus == AbstractGraphNodeLink::NavMeshLink_NoNavMesh)
		return KY_SUCCESS;

	AbstractGraphVertex abstractNodeVertex;
	KyFloat32 abstractNodeAltitude;
	currentAbstractNodeRawPtr.m_abstractGraph->m_blob->GetVertexAndAltitude(currentAbstractNodeRawPtr.m_nodeIdx, abstractNodeVertex, abstractNodeAltitude);
	CardinalDir cellBoundaryDir = currentAbstractNodeRawPtr.GetCellBoundary();
	KyUInt32 axisIndex = AbstractGraphUtils::GetNavVertexAxisIndexFromCellBoundary(cellBoundaryDir);
	
	ActiveData* activeData = currentAbstractNodeRawPtr.m_abstractGraph->GetDatabase()->GetActiveData();
	CellPos cellPos = currentAbstractNodeRawPtr.GetCellPos();
	ActiveCell* activeCellRawPtr = activeData->GetActiveCellFromCellPos(cellPos);
	KY_ASSERT(activeCellRawPtr != KY_NULL);
	if (activeCellRawPtr == KY_NULL)
		return KY_SUCCESS;

	ActiveCell& activeCell = *activeCellRawPtr;
	if (activeCell.GetActiveNavFloorsCount() == 0)
		return KY_SUCCESS;

	NavFloorIdx originalNavFloorIdx = currentAbstractNodeRawPtr.GetOriginalNavFloorIdx();
	KY_ASSERT(originalNavFloorIdx < activeCell.GetActiveNavFloorsCount());
	NavFloor* navFloor = activeCell.GetActiveNavFloorBuffer()[originalNavFloorIdx];
	KY_ASSERT(navFloor != KY_NULL);
	if (navFloor == KY_NULL)
		return KY_SUCCESS;

	const NavFloorBlob* navFloorBlob = navFloor->GetNavFloorBlob();
	const NavCellBlob* navCellBlob = navFloor->m_navCell->m_navCellBlob;

	const NavFloor1To1StitchDataBlob& stitchDataBlob = navCellBlob->m_floor1To1StitchData.GetValues()[originalNavFloorIdx];
	KyUInt16 stitch1To1EdgeCount = stitchDataBlob.m_edgeCountForType[cellBoundaryDir];
	KY_ASSERT(stitch1To1EdgeCount != 0);
	if (stitch1To1EdgeCount == 0)
		return KY_SUCCESS;
	
	KyUInt16 stitch1To1EdgeFirstIdx = stitchDataBlob.m_firstIdxOfEdgeForType[cellBoundaryDir];			
	// Note that stitch1To1Edges are sorted along the x or y axis depending on the cellBoundaryDir (cf. CellBoundaryEdgeSorter)
	const Stitch1To1Edge* localStitchEdges = &(stitchDataBlob.m_stitch1To1Edges.GetValues()[stitch1To1EdgeFirstIdx]);
	const NavVertex* stitchVertices = stitchDataBlob.m_navVertices.GetValues();
	const KyFloat32* stitchAltitudes = stitchDataBlob.m_navVertexAltitudes.GetValues();
	const Stitch1To1ToHalfEdgeInFloor& stitch1To1ToHalfEdgeInFloor = navFloorBlob->m_stitch1To1ToHalfEdgeInFloor;
	for (KyUInt16 localStitchIdx = 0; localStitchIdx < stitch1To1EdgeCount; ++localStitchIdx)
	{
		KyUInt32 stitch1To1EdgeIdx = stitch1To1EdgeFirstIdx + localStitchIdx;

		KyUInt32 edgeInFloorCount = stitch1To1ToHalfEdgeInFloor.m_stitch1To1EdgeIdxToCount.GetValues()[stitch1To1EdgeIdx];
		if (edgeInFloorCount == 0) // no halfEdge in Floor are tied to stitch1To1Edge
			continue;

		const Stitch1To1Edge& stitchEdge = localStitchEdges[localStitchIdx];
		NavVertexIdx startVertexIdx = stitchEdge.GetStartVertexIdx();
		NavVertexIdx endVertexIdx = stitchEdge.GetEndVertexIdx();
		KyUInt8 start = stitchVertices[startVertexIdx][axisIndex];
		KyUInt8 end = stitchVertices[endVertexIdx][axisIndex];
		KyFloat32 startAltitude = stitchAltitudes[startVertexIdx];
		KyFloat32 endAltitude = stitchAltitudes[endVertexIdx];

		bool isNodeInRange = Min(start, end) <= abstractNodeVertex[axisIndex] && abstractNodeVertex[axisIndex] <= Max(start, end);
		if (isNodeInRange == false)
			continue;

		isNodeInRange = Min(startAltitude, endAltitude) <= abstractNodeAltitude && abstractNodeAltitude <= Max(startAltitude, endAltitude);
		if (isNodeInRange == false)
			continue;

		// ok found the stitch1To1Edge, so we can propagate on NavMesh from this AbstractGraphNode
		KyUInt16 edgeInFloorFirstIdx = stitch1To1ToHalfEdgeInFloor.m_stitch1To1EdgeIdxToFirstIdx.GetValues()[stitch1To1EdgeIdx];
		for (KyUInt32 i = 0; i < edgeInFloorCount; ++i)
		{
			CompactNavHalfEdgeIdx halfEdgeIdx = stitch1To1ToHalfEdgeInFloor.m_dynamicNavFloorEdgeIdx.GetValues()[edgeInFloorFirstIdx + i];
			NavTriangleIdx triangleIdx = NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(halfEdgeIdx);
			NavTriangleRawPtr triangleRawPtr(navFloor, triangleIdx);

			if (CanTraverseNavTriangle(triangleRawPtr) == false)
				continue;

			ExploreAllNodesInTriangle(activeData, triangleRawPtr, currentNodeIndex);
		}
		return KY_SUCCESS;
	}

	return KY_SUCCESS;
}

template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::ExploreAllNodesInTriangle(ActiveData* activeData, const NavTriangleRawPtr& triangleRawPtr, AStarNodeIndex currentNodeIndex)
{
	KY_FORWARD_ERROR_NO_LOG(ExploreHalfEdgesOfTriangle(activeData, triangleRawPtr, currentNodeIndex));
	KY_FORWARD_ERROR_NO_LOG(ExploreGraphVerticesInTriangle(triangleRawPtr, currentNodeIndex));
	KY_FORWARD_ERROR_NO_LOG(m_traversalCustomizer.OnNavTriangleExplored(m_astarContext, triangleRawPtr, currentNodeIndex));

	return KY_SUCCESS;
}



template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::CreateNewAbstractGraphNode(const AbstractGraphNodeRawPtr& abstractGraphNodeRawPtr, const AbstractGraphNodeRawPtr& pairedAbstractGraphNodeRawPtr, AStarNodeIndex predecessorNodeIndex, KyFloat32 costFromCurrentNode)
{
	KY_FORWARD_ERROR_NO_LOG(m_astarContext->CheckAstarNodeArrayMemory()); // check for memory to create a new node
	KY_FORWARD_ERROR_NO_LOG(m_astarContext->CheckAbstractGraphNodeRawPtrArrayMemory()); // check for memory to create a new node
	KY_FORWARD_ERROR_NO_LOG(m_astarContext->CheckTraversalBinaryHeapMemory()); // check for memory to insert a newElement in the binary heap

	WorkingMemArray<AStarNode>& aStarNodes                    = m_astarContext->m_aStarNodes;
	WorkingMemArray<AbstractGraphNodeRawPtr>& aStarNodesAbstractNodeRawPtrs = m_astarContext->m_abstractRawPtrNodes;
	AstarNodeIndexInGrid& edgeIndexGrid                       = m_astarContext->m_edgeIndexGrid;

	// the new node Index
	AStarNodeIndex newNodeIndex = (AStarNodeIndex)aStarNodes.GetCount();

	const Vec3f nodePos = abstractGraphNodeRawPtr.m_abstractGraph->GetNodePosition(abstractGraphNodeRawPtr.m_nodeIdx);
	
	aStarNodes.PushBack_UnSafe(AStarNode(predecessorNodeIndex, nodePos, NodeType_AbstractGraphNode, aStarNodesAbstractNodeRawPtrs.GetCount()));
	aStarNodesAbstractNodeRawPtrs.PushBack(abstractGraphNodeRawPtr);

	AstarNodeIndexInGrid::AbstractGraphToNodeIndices* abstractGraphToNodeIndices = edgeIndexGrid.GetAbstractGraphToNodeIndices_Unsafe(abstractGraphNodeRawPtr);

	// set the index of the current edge
	abstractGraphToNodeIndices->SetAStarNodeIndex(abstractGraphNodeRawPtr.m_nodeIdx, newNodeIndex);

	// retrieve node indices of the AbstractGraph that contains the pair AbstractGraphNode
	abstractGraphToNodeIndices = KY_NULL;
	if (KY_FAILED(edgeIndexGrid.GetAbstractGraphToNodeIndices(pairedAbstractGraphNodeRawPtr, abstractGraphToNodeIndices)))
	{
		KY_LOG_WARNING( ("This traversal reached the maximum size of propagation memory in the activeData"));
		return KY_ERROR;
	}

	// set the index to the pair edge also
	abstractGraphToNodeIndices->SetAStarNodeIndex(pairedAbstractGraphNodeRawPtr.m_nodeIdx, newNodeIndex);

	AStarNode& newNode = aStarNodes[newNodeIndex];
	// compute first cost of the node
	AStarNode& predecessorNode = aStarNodes[predecessorNodeIndex];

	// compute the estimated cost to dest once and for all
	newNode.m_estimatedCostToDest = EvaluateCostToDest(nodePos);
	newNode.m_costFromStart = predecessorNode.m_costFromStart + costFromCurrentNode;
	m_astarContext->m_traversalBinHeap.Insert(newNodeIndex);

	return KY_SUCCESS;
}

template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::CreateNewGraphVertexNode(const NavGraphVertexRawPtr& navGraphVertexRawPtr, AStarNodeIndex predecessorNodeIndex, KyFloat32 costFromCurrentNode)
{
	WorkingMemArray<AStarNode>& aStarNodes = m_astarContext->m_aStarNodes;
	WorkingMemArray<NavGraphVertexRawPtr>& aStarNodesVertexRawPtrs = m_astarContext->m_vertexRawPtrNodes;

	// check for memory to create a new node
	KY_FORWARD_ERROR_NO_LOG(m_astarContext->CheckAstarNodeArrayMemory());
	KY_FORWARD_ERROR_NO_LOG(m_astarContext->CheckNavGraphVertexRawPtrArrayMemory());
	// check for memory to insert a newElement in the binary heap
	KY_FORWARD_ERROR_NO_LOG(m_astarContext->CheckTraversalBinaryHeapMemory());

	NavGraph* navGraph = navGraphVertexRawPtr.GetNavGraph();
	const NavGraphBlob& navGraphBlob = *navGraph->GetNavGraphBlob();

	const Vec3f& vertexPosition = navGraphBlob.GetNavGraphVertexPosition(navGraphVertexRawPtr.GetNavGraphVertexIdx());

	// set the new node Index
	AStarNodeIndex newNodeIndex = (AStarNodeIndex)aStarNodes.GetCount();

	// insert node in the binary Heap
	aStarNodes.PushBack(AStarNode(predecessorNodeIndex, vertexPosition, NodeType_NavGraphVertex, aStarNodesVertexRawPtrs.GetCount()));
	aStarNodesVertexRawPtrs.PushBack(navGraphVertexRawPtr);

	AstarNodeIndexInGrid::NavGraphToNodeIndices* navGraphToNodeIndices = m_astarContext->m_edgeIndexGrid.GetNavGraphToNodeIndices_Unsafe(navGraphVertexRawPtr);
	navGraphToNodeIndices->SetAStarNodeIndex(navGraphVertexRawPtr.GetNavGraphVertexIdx(), newNodeIndex);

	AStarNode& newNode = aStarNodes[newNodeIndex];
	AStarNode& predecessorNode = aStarNodes[predecessorNodeIndex];
	// compute the estimated cost to dest once and for all
	newNode.m_estimatedCostToDest = EvaluateCostToDest(vertexPosition);

	// compute first cost of the node
	newNode.m_costFromStart = predecessorNode.m_costFromStart + costFromCurrentNode;

	m_astarContext->m_traversalBinHeap.Insert(newNodeIndex);

	return KY_SUCCESS;
}

template<class TraversalCustomzier>
KY_INLINE bool AStarTraversal<TraversalCustomzier>::ShouldOpenHalfEdgeNode(const NavHalfEdgeRawPtr& halfEdge, NavHalfEdgeRawPtr& pairHalfEdgeOfNewNode)
{
	// check if the edge can be crossed
	if (halfEdge.IsHalfEdgeOneWayCrossable<TraverseLogic>(m_traversalCustomizer.GetTraverseLogicUserData(), pairHalfEdgeOfNewNode, KY_NULL) == false)
		return false; // edge no crossable, do not create a node

	return m_traversalCustomizer.ShouldOpenHalfEdgeNode(halfEdge, pairHalfEdgeOfNewNode);
}


template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::OpenOrUpdateGraphVertex(const NavGraphVertexRawPtr& navGraphVertexRawPtr, 
	AStarNodeIndex vertexNodeIndex, AStarNodeIndex currentNodeIndex, KyFloat32 costFromCurrentNode)
{
	WorkingMemArray<AStarNode>& aStarNodes = m_astarContext->m_aStarNodes;

	if (vertexNodeIndex == AStarNodeIndex_Invalid)
	{
		if (ShouldOpenGraphVertexNode(navGraphVertexRawPtr))
		{
			// no index for this vertex -> it has never been encoutered, it is a new node
			KY_FORWARD_ERROR_NO_LOG(CreateNewGraphVertexNode(navGraphVertexRawPtr, currentNodeIndex, costFromCurrentNode));
		}
	}
	else
	{
		if (vertexNodeIndex != aStarNodes[currentNodeIndex].GetIndexOfPredecessor())
		{
			// A node has already be created for this vertex -> we update it
			KY_FORWARD_ERROR_NO_LOG(UpdateOpenedOrClosedNode(vertexNodeIndex, currentNodeIndex, costFromCurrentNode));
		}
	}

	return KY_SUCCESS;
}

template<class TraversalCustomzier>
inline bool AStarTraversal<TraversalCustomzier>::ShouldOpenGraphVertexNode(const NavGraphVertexRawPtr& navGraphVertexRawPtr)
{
	return m_traversalCustomizer.ShouldOpenGraphVertexNode(navGraphVertexRawPtr);
}

template<class TraversalCustomzier>
KY_INLINE KyResult AStarTraversal<TraversalCustomzier>::OpenOrUpdateAbstractGraphNode(const AbstractGraphNodeRawPtr& abstractNodeRawPtr, AStarNodeIndex astarNodeIndex, AStarNodeIndex currentNodeIndex, KyFloat32 costFromCurrentNode)
{
	WorkingMemArray<AStarNode>& aStarNodes = m_astarContext->m_aStarNodes;

	if (astarNodeIndex == AStarNodeIndex_Invalid)
	{
		AbstractGraphNodeRawPtr pairedAbstractNodeRawPtr;
		if (ShouldOpenAbstractGraphNode(abstractNodeRawPtr, pairedAbstractNodeRawPtr))
		{
			// no index for this vertex -> it has never been encoutered, it is a new node
			KY_FORWARD_ERROR_NO_LOG(CreateNewAbstractGraphNode(abstractNodeRawPtr, pairedAbstractNodeRawPtr, currentNodeIndex, costFromCurrentNode));
		}
	}
	else
	{
		if (astarNodeIndex != aStarNodes[currentNodeIndex].GetIndexOfPredecessor())
		{
			// A node has already be created for this vertex -> we update it
			KY_FORWARD_ERROR_NO_LOG(UpdateOpenedOrClosedNode(astarNodeIndex, currentNodeIndex, costFromCurrentNode));
		}
	}

	return KY_SUCCESS;
}

template<class TraversalCustomzier>
inline bool AStarTraversal<TraversalCustomzier>::ShouldOpenAbstractGraphNode(const AbstractGraphNodeRawPtr& abstractGraphNodeRawPtr, AbstractGraphNodeRawPtr& pairedAbstractGraphNodeRawPtr)
{
	if (abstractGraphNodeRawPtr.GetAbstractGraphNodeLink().CanTraverse() == false)
		return false;

	pairedAbstractGraphNodeRawPtr = abstractGraphNodeRawPtr.GetPairedAbstractGraphNodeRawPtr();
	return m_traversalCustomizer.ShouldOpenAbstractGraphNode(pairedAbstractGraphNodeRawPtr);
}



template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::ExploreHalfEdgesOfTriangle(ActiveData* activeData, const NavTriangleRawPtr& triangleRawPtr,
	AStarNodeIndex currentNodeIndex)
{
	AstarNodeIndexInGrid& edgeIndexGrid = m_astarContext->m_edgeIndexGrid;

	AstarNodeIndexInGrid::NavFloorToNodeIndices* navFloorToNodeIndicesPtr = KY_NULL;

	// retrieve a pointer the NodeIndices of this navFloorBlob HalfEdges
	if (KY_FAILED(edgeIndexGrid.GetNavFloorToNodeIndices(activeData, triangleRawPtr.m_navFloorRawPtr, navFloorToNodeIndicesPtr)))
	{
		KY_LOG_WARNING( ("This traversal reached the maximum size of propagation memory in the activeData"));
		return KY_ERROR;
	}

	KY_ASSERT(navFloorToNodeIndicesPtr != KY_NULL);

	NavHalfEdgeRawPtr edgeRawPtr[3];
	AStarNodeIndex edgeIndex[3];
	Vec3f v0f, v1f, v2f;
	Vec3f* vertices[4]; // the vertices of the Triangle
	vertices[0] = &v0f;
	vertices[1] = &v1f;
	vertices[2] = &v2f;
	vertices[3] = &v0f;

	const NavHalfEdgeIdx firstHalfEdgeIdx = NavFloorBlob::NavTriangleIdxToFirstNavHalfEdgeIdx(triangleRawPtr.GetTriangleIdx());

	// compute NavHalfEdgeRawPtr and retrieve pointer to Nodeindex
	for (KyUInt32 i = 0; i < 3; ++i)
	{
		edgeRawPtr[i].Set(triangleRawPtr.m_navFloorRawPtr, firstHalfEdgeIdx + i);
		edgeIndex[i] = navFloorToNodeIndicesPtr->GetAStarNodeIndex(firstHalfEdgeIdx + i);
	}

	navFloorToNodeIndicesPtr = KY_NULL; // be sure it won't be used anymore !

	if (edgeIndex[0] == AStarNodeIndex_Invalid || edgeIndex[1] == AStarNodeIndex_Invalid || edgeIndex[2] == AStarNodeIndex_Invalid)
	{
		// retrieve the three vertices of the triangles
		triangleRawPtr.GetVerticesPos3f(v0f, v1f, v2f);
	}

	for (KyUInt32 i = 0; i < 3; ++i)
	{
		KY_FORWARD_ERROR_NO_LOG(OpenOrUpdateHalfEdge(activeData, edgeRawPtr[i], edgeIndex[i], *vertices[i], *vertices[i + 1], currentNodeIndex));
	}

	return KY_SUCCESS;
}

template<class TraversalCustomzier>
inline KyResult AStarTraversal<TraversalCustomzier>::UpdateOpenedOrClosedNode(AStarNodeIndex neighborNodeIndex, AStarNodeIndex currentNodeIndex, KyFloat32 costFromCurrentNode)
{
	WorkingMemArray<AStarNode>& aStarNodes = m_astarContext->m_aStarNodes;

	AStarNode& newPredecessor = aStarNodes[currentNodeIndex];
	AStarNode& neighborNode = aStarNodes[neighborNodeIndex];

	const KyFloat32 newCost = newPredecessor.m_costFromStart + costFromCurrentNode;

	if (newCost >= neighborNode.m_costFromStart)
		return KY_SUCCESS;

	neighborNode.SetIndexOfPredecessor(currentNodeIndex);
	neighborNode.m_costFromStart = newCost;

	return m_astarContext->InsertOrUpdateInBinHeapTraversal(neighborNode, neighborNodeIndex);
}

}
