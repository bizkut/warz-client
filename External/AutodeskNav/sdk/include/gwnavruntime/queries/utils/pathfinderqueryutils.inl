/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: LAPA


namespace Kaim
{

//#define DBG_SendRefineOneNodeVisualDebug


template<class TLogic>
inline KyResult PathFinderQueryUtils::TryToHookOnNavMesh(const Vec3f& inputOutsidePos, const PositionSpatializationRange& positionSpatializationRange,
	KyFloat32 horizontalHookingMaxDist, KyFloat32 distFromObstacle, Vec3f& outputInsidePos, NavTrianglePtr& outputNavTrianglePtr)
{
	InsidePosFromOutsidePosQuery<TLogic> insidePosFromOutsidePosQuery;
	insidePosFromOutsidePosQuery.BindToDatabase(m_database);
	insidePosFromOutsidePosQuery.SetPositionSpatializationRange(positionSpatializationRange);
	insidePosFromOutsidePosQuery.SetTraverseLogicUserData(GetTraverseLogicUserData());
	insidePosFromOutsidePosQuery.SetHorizontalTolerance(horizontalHookingMaxDist); // look up to +/- horizontalHookingMaxDist on x,y.
	insidePosFromOutsidePosQuery.SetDistFromObstacle(distFromObstacle); // 10 cm from the border

	insidePosFromOutsidePosQuery.Initialize(inputOutsidePos);
	insidePosFromOutsidePosQuery.SetPerformQueryStat(m_performQueryStat);
	insidePosFromOutsidePosQuery.PerformQuery(m_workingMemory);

	if (insidePosFromOutsidePosQuery.GetResult() == INSIDEPOSFROMOUTSIDE_DONE_POS_FOUND)
	{
		outputInsidePos = insidePosFromOutsidePosQuery.GetInsidePos();
		outputNavTrianglePtr = insidePosFromOutsidePosQuery.GetInsidePosTrianglePtr();
		return KY_SUCCESS;
	}

	return KY_ERROR;
}

template <class TLogic>
inline KyResult PathFinderQueryUtils::RefineOneNode(const PositionSpatializationRange& positionSpatializationRange, const PathRefinerConfig& pathRefinerConfig, KyUInt32& cangoTestDone)
{
	//////////////////////////////////////////////////////////////////////////
	// Initialize some ubiquitous parameter values

	// The minimal distance the new edges will be created from the NavMesh borders.
	// We cap it to just a bit more than CanGo margin to avoid creating new edge
	// that should be then invalidated with CanGos.
	const KyFloat32 minDistToBorders = Max(1.1f * m_database->GetRayCanGoMarginFloat(PathMargin), pathRefinerConfig.m_idealDistanceToBorders);

	// The minimal new edge length.
	const KyFloat32 minNewEdgeLength = pathRefinerConfig.m_minimalNewEdgeLength;


	//////////////////////////////////////////////////////////////////////////
	// Initialize some ubiquitous queries

	MonodirectionalRayCanGoQuery<TLogic> oneWayCanGoWithCost;
	RayCastQuery<TLogic> rayCastToPointOfEdge1;
	RayCastQuery<TLogic> rayCastToPointOfEdge2;

	oneWayCanGoWithCost.BindToDatabase(m_database);
	rayCastToPointOfEdge1.BindToDatabase(m_database);
	rayCastToPointOfEdge2.BindToDatabase(m_database);

	oneWayCanGoWithCost.SetTraverseLogicUserData(GetTraverseLogicUserData());
	rayCastToPointOfEdge1.SetTraverseLogicUserData(GetTraverseLogicUserData());
	rayCastToPointOfEdge2.SetTraverseLogicUserData(GetTraverseLogicUserData());

	oneWayCanGoWithCost.SetMarginMode(PathMargin);
	oneWayCanGoWithCost.SetComputeCostMode(QUERY_COMPUTE_COST_ALONG_3DAXIS);
	oneWayCanGoWithCost.SetPositionSpatializationRange(positionSpatializationRange);
	rayCastToPointOfEdge1.SetPositionSpatializationRange(positionSpatializationRange);
	rayCastToPointOfEdge2.SetPositionSpatializationRange(positionSpatializationRange);

	oneWayCanGoWithCost.SetPerformQueryStat(m_performQueryStat);
	rayCastToPointOfEdge1.SetPerformQueryStat(m_performQueryStat);
	rayCastToPointOfEdge2.SetPerformQueryStat(m_performQueryStat);


	//////////////////////////////////////////////////////////////////////////
	// Initialize some useful aliases

	PathRefinerContext* refinerContext = m_workingMemory->m_refinerContext;
	RefinerBinHeap& refinerBinHeap = refinerContext->m_refinerBinHeap;
	WorkingMemArray<RefinerNode>& refinerNodes = refinerContext->m_refinerNodes;
	WorkingMemArray<NavGraphVertexRawPtr>& vertexRawPtrNodes = refinerContext->m_vertexRawPtrNodes;
	WorkingMemArray<NavTriangleRawPtr>& triangleRawPtrNodes = refinerContext->m_triangleRawPtrNodes;

	//////////////////////////////////////////////////////////////////////////
	// Extract next node to proceed and neighbors
	RefinerNodeIndex currentNodeIndex;
	refinerBinHeap.ExtractFirst(currentNodeIndex);

	RefinerNode* currentNode = &refinerNodes[currentNodeIndex];
	RefinerNodeIndex prevNodeIdx = currentNode->m_predecessorNodeIdx;
	RefinerNodeIndex nextNodeIdx = currentNode->m_nextNodeIdx;

	RefinerNode* prevNode = &refinerNodes[prevNodeIdx];
	RefinerNode* nextNode = &refinerNodes[nextNodeIdx];

	const Vec3f& prevNodePos = prevNode->m_nodePosition;
	const Vec3f& nextNodePos = nextNode->m_nodePosition;

	const KyFloat32 currentCostFromPrevToNext = (currentNode->m_costFromPredecessor + nextNode->m_costFromPredecessor) + 0.05f; // 5cm margin for float imprecision

	NavTrianglePtr prevTrianglePtr;

	if(prevNode->GetNodeType() != NodeType_NavGraphVertex)
	{
		prevTrianglePtr = NavTrianglePtr(triangleRawPtrNodes[prevNode->GetIdxOfRawPtrData()]);
	}
	else
	{
		const NavGraphVertexRawPtr& nodeGraphVertexRawPtr = vertexRawPtrNodes[prevNode->GetIdxOfRawPtrData()];
		NavGraphLink* navGraphLink = nodeGraphVertexRawPtr.GetGraphVertexData().m_navGraphLink;
		KY_ASSERT(navGraphLink != KY_NULL && navGraphLink->IsValid());
		prevTrianglePtr = NavTrianglePtr(navGraphLink->m_navTriangleRawPtr);
	}


	//////////////////////////////////////////////////////////////////////////
	// Try direct shortcut from prev node to next node.

	InitRayCanGoWithCost(oneWayCanGoWithCost, prevNodePos, prevTrianglePtr, nextNodePos, prevNode->m_nodeIntegerPos, nextNode->m_nodeIntegerPos);

	oneWayCanGoWithCost.PerformQueryWithInputCoordPos(m_workingMemory);
	++cangoTestDone;
	if (oneWayCanGoWithCost.GetResult() == RAYCANGO_DONE_SUCCESS)
	{
		const KyFloat32 costFromPrevToDest = oneWayCanGoWithCost.GetComputedCost();
		if (costFromPrevToDest <= currentCostFromPrevToNext)
		{
			prevNode->m_nextNodeIdx = nextNodeIdx;
			nextNode->m_predecessorNodeIdx = prevNodeIdx;
			nextNode->m_costFromPredecessor = costFromPrevToDest;
			KY_FORWARD_ERROR_NO_LOG(UpdateNodeInBinaryHeap(prevNodeIdx, prevNode));
			KY_FORWARD_ERROR_NO_LOG(UpdateNodeInBinaryHeap(nextNodeIdx, nextNode));

#ifdef DBG_SendRefineOneNodeVisualDebug
			{
				ScopedDisplayList displayList_NewEdgeCdt(m_database->GetWorld());
				displayList_NewEdgeCdt.InitSingleFrameLifespan("Result - NewEdgeCdt", "RefineOneNode");
				displayList_NewEdgeCdt.PushLine(prevNodePos, nextNodePos, VisualColor::Lime);
			}
#endif

			return KY_SUCCESS;
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// Lets now analyze navmesh borders in the path turn inner. To do so, we use
	// a trapezoide propagation to retrieve the borders in the extended triangle
	// (A', B, C'), so that to take care of PathMargin in NavMesh border collection.
	// 
	/**************************************************************************/
	/*                                                                        */
	/*                      + A'                                              */
	/*                       \                                                */
	/*                        + A (= prevNode)                                */
	/*                         \                                              */
	/*                          \                                             */
	/*                           \                                            */
	/*                            \                                           */
	/*   bisector <----------------+ B (= currentNode)                        */
	/*                            /                                           */
	/*                           /                                            */
	/*                          /                                             */
	/*                         /                                              */
	/*                        /                                               */
	/*                       /                                                */
	/*                      /                                                 */
	/*                     /                                                  */
	/*                    /                                                   */
	/*                   /                                                    */
	/*                  /                                                     */
	/*                 + C (= nextNode)                                       */
	/*                /                                                       */
	/*               + C'                                                     */
	/*                                                                        */
	/**************************************************************************/

	const Vec3f& currentNodePos = currentNode->m_nodePosition;

	Vec2f prevToNodeUnitVec(currentNodePos.x - prevNodePos.x, currentNodePos.y - prevNodePos.y);
	Vec2f nodeToNextUnitVec(nextNodePos.x - currentNodePos.x, nextNodePos.y - currentNodePos.y);
	const KyFloat32 nodeToPrevLength = prevToNodeUnitVec.Normalize(); // edge1
	const KyFloat32 nodeToNextLength = nodeToNextUnitVec.Normalize(); // edge2

	KY_ASSERT(currentNode->GetNodeType() == NodeType_NavMeshEdge);
	NavTrianglePtr nodeTrianglePtr(triangleRawPtrNodes[currentNode->GetIdxOfRawPtrData()]);


	// Filter accordingly to node angle
	const KyFloat32 dotProdOriginalEdges = prevToNodeUnitVec * nodeToNextUnitVec;
	// Take the negate here since dotProdOriginalEdges is actually the direction rotation
	// cosine whereas the pathRefinerConfig value is taken on the angle between the edges
	if (dotProdOriginalEdges > -pathRefinerConfig.m_thresholdAngleCos)
		return KY_SUCCESS; // Turn is almost flat, don't try to refine it!


	//////////////////////////////////////////////////////////////////////////
	// Compute bisector and related information

	// This ratio is the distance along original edges corresponding to 1m along the bisector. This is the
	// sines of half the angle between the original edges.
	Vec2f bissectorAxis = nodeToNextUnitVec - prevToNodeUnitVec;
	const KyFloat32 originalEdgeBisectorRatio = bissectorAxis.Normalize();
	const KyFloat32 bisectorToOriginalEdgeRatio = 2.0f / originalEdgeBisectorRatio; // 2.0 since we normalized from the unit vector sum.


	//////////////////////////////////////////////////////////////////////////
	// Compute cone parameters

	const bool nearestNodeIsPrevNode = (nodeToPrevLength <= nodeToNextLength);
	const Vec2f& unitVecToNearestNode = nearestNodeIsPrevNode ? -prevToNodeUnitVec : nodeToNextUnitVec;
	const KyFloat32& distToNearestNode = nearestNodeIsPrevNode ? nodeToPrevLength : nodeToNextLength;

	const KyFloat32 coneLengthAlongEdges = distToNearestNode + (minDistToBorders * bisectorToOriginalEdgeRatio);
	const Vec2f coneCornerRelativePos = coneLengthAlongEdges * unitVecToNearestNode;
	const Vec3f coneCornerPos = currentNodePos + coneCornerRelativePos;
	const KyFloat32 coneLength = DotProduct(coneCornerRelativePos, bissectorAxis);
	const KyFloat32 coneHalfWidth = fabsf(CrossProduct(coneCornerRelativePos, bissectorAxis));

	NavTrianglePtr currentTrianglePtr = NavTrianglePtr(triangleRawPtrNodes[currentNode->GetIdxOfRawPtrData()]);
	NavTriangleRawPtr propagationStartTriangleRawPtr = currentTrianglePtr.GetRawPtr();

#ifdef DBG_SendRefineOneNodeVisualDebug
	const VisualColor propagationColor = VisualColor::Orange;
	{
		const Vec3f secondCornerPos = currentNodePos + (2.0f * coneLength) * bissectorAxis - coneCornerRelativePos;

		ScopedDisplayList displayList_PropagationAxes(m_database->GetWorld());
		ScopedDisplayList displayList_PropagationCone(m_database->GetWorld());
		ScopedDisplayList displayList_PropagationStartTriangle(m_database->GetWorld());
		displayList_PropagationAxes.InitSingleFrameLifespan("Propagation - Axis", "RefineOneNode");
		displayList_PropagationCone.InitSingleFrameLifespan("Propagation - Cone", "RefineOneNode");
		displayList_PropagationStartTriangle.InitSingleFrameLifespan("Propagation - StartTriangle", "RefineOneNode");

		displayList_PropagationAxes.PushLine(currentNodePos, currentNodePos + bissectorAxis, propagationColor);

		VisualShapeColor coneColor;
		coneColor.m_triangleColor = propagationColor;
		coneColor.m_triangleColor.m_a = 30;
		coneColor.m_lineColor = VisualColor::Black;
		displayList_PropagationCone.PushTriangle(currentNodePos, coneCornerPos, secondCornerPos, coneColor);

		Triangle3f startTriangle;
		propagationStartTriangleRawPtr.GetVerticesPos3f(startTriangle);
		VisualShapeColor startTriangleColor;
		startTriangleColor.SetOnlyLineColor(propagationColor);
		displayList_PropagationStartTriangle.PushTriangle(startTriangle, startTriangleColor);
	}
#endif

	ConeIntersector coneIntersector(currentNodePos, bissectorAxis, coneLength, coneHalfWidth, m_database->GetDatabaseGenMetrics().m_integerPrecision);
	QueryUtils queryUtils(m_database, m_workingMemory, GetTraverseLogicUserData());
	BestFirstSearchEdgeCollisionVisitor<TLogic, ConeIntersector> edgeIntersectionVisitor(GetTraverseLogicUserData(), coneIntersector);
	BestFirstSearchTraversal<BestFirstSearchEdgeCollisionVisitor<TLogic, ConeIntersector> > traversalForConeCast(
		queryUtils, m_database->GetActiveData()->GetCellBox(), edgeIntersectionVisitor);

	cangoTestDone += 5;

	if (traversalForConeCast.SetStartTriangle(propagationStartTriangleRawPtr) != TraversalResult_DONE)
		return KY_ERROR;
	if (traversalForConeCast.Search() != TraversalResult_DONE)
		return KY_ERROR;

	// Retrieve the nearest corner
	const Vec3f nearestCorner = edgeIntersectionVisitor.m_collisionFound ? edgeIntersectionVisitor.m_collisionPos : coneCornerPos;
	const NearestCornerType nearestCornerType =
		edgeIntersectionVisitor.m_collisionFound ? NearestCornerType_InnerCorner
		: (nearestNodeIsPrevNode ? NearestCornerType_PrevNode : NearestCornerType_NextNode);
	const Vec3f nodeToNearestMeshCorner = nearestCorner - currentNodePos;
	const KyFloat32 minDist = DotProduct(nodeToNearestMeshCorner.Get2d(), bissectorAxis);

#ifdef DBG_SendRefineOneNodeVisualDebug
	{
		ScopedDisplayList displayList_TraversedTriangles(m_database->GetWorld());
		ScopedDisplayList displayList_NearestCorner(m_database->GetWorld());
		displayList_TraversedTriangles.InitSingleFrameLifespan("Result - TraversedTriangles", "RefineOneNode");
		displayList_NearestCorner.InitSingleFrameLifespan("Result - NearestCorner", "RefineOneNode");

		if (traversalForConeCast.m_visitedNodes)
		{
			VisualShapeColor traversedTriangleColor;
			traversedTriangleColor.SetOnlyTriangleColor(VisualColor::Lime);
			traversedTriangleColor.m_triangleColor.m_a = 128;
			Triangle3f traversedTriangle;
			KyUInt32 traversedTriangleCount = traversalForConeCast.m_visitedNodes->GetCount();
			NavTriangleRawPtr* traversedTriangleArray = traversalForConeCast.m_visitedNodes->GetBuffer();
			for (KyUInt32 i = 0; i < traversedTriangleCount; ++i)
			{
				traversedTriangleArray[i].GetVerticesPos3f(traversedTriangle);
				traversedTriangleColor.m_triangleColor.m_a = (KyUInt8)((KyUInt32(traversedTriangleColor.m_triangleColor.m_a) * 4) / 5);
				displayList_TraversedTriangles.PushTriangle(traversedTriangle, traversedTriangleColor);
			}
		}
		else
			displayList_TraversedTriangles.PushText(currentNodePos, VisualColor::Red, "Traversal VisitedNode array creation failed !");

		char textBuffer[32];
		switch (nearestCornerType)
		{
		case NearestCornerType_Undefined:    SFsprintf(textBuffer, 32, "Undefined   %.02f", minDist);   break;
		case NearestCornerType_PrevNode:     SFsprintf(textBuffer, 32, "PrevNode    %.02f", minDist);   break;
		case NearestCornerType_InnerCorner:  SFsprintf(textBuffer, 32, "InnerCorner %.02f", minDist);   break;
		case NearestCornerType_NextNode:     SFsprintf(textBuffer, 32, "NextNode    %.02f", minDist);   break;
		}
		displayList_NearestCorner.PushPoint(nearestCorner, 0.1f, VisualShapeColor(VisualShapeColor::Inactive(), propagationColor));
		displayList_NearestCorner.PushText(nearestCorner, VisualColor::White, textBuffer);
	}
#endif

	// Replace the current node by 2 nodes so that the new edge cuts the current node
	// turn at PathMargin of the closest corner identified just above.
	if (minDist > minDistToBorders)
	{
		const KyFloat32 distFromCurNode = minDist - minDistToBorders;
		const Vec3f newEdgeStart = currentNodePos - (bisectorToOriginalEdgeRatio * distFromCurNode) * prevToNodeUnitVec;
		const Vec3f newEdgeEnd   = currentNodePos + (bisectorToOriginalEdgeRatio * distFromCurNode) * nodeToNextUnitVec;

		if (SquareDistance2d(newEdgeStart, newEdgeEnd) < minNewEdgeLength * minNewEdgeLength)
			return KY_SUCCESS; // Avoid create tiny edges

#ifdef DBG_SendRefineOneNodeVisualDebug
		{
			ScopedDisplayList displayList_NewEdgeCdt(m_database->GetWorld());
			displayList_NewEdgeCdt.InitSingleFrameLifespan("Result - NewEdgeCdt", "RefineOneNode");

			const Vec3f bordersRenderOffset(0.0f, 0.0f, 0.02f);
			displayList_NewEdgeCdt.PushLine(newEdgeStart + bordersRenderOffset, newEdgeEnd + bordersRenderOffset, VisualColor::Cyan);
		}
#endif

		KY_ASSERT(nearestCornerType != NearestCornerType_Undefined);
		bool tryANewPointOnEdge1 = (nearestCornerType != NearestCornerType_PrevNode);
		bool tryANewPointOnEdge2 = (nearestCornerType != NearestCornerType_NextNode);


		//////////////////////////////////////////////////////////////////////////
		// Check the new edge is ok and, if so, update refine context accordingly.
		{
			KyFloat32 costFromPrevToPointOnEdge1 = 0.f;
			KyFloat32 costFromPointOnEdge2ToNextNode = 0.f;

			if (tryANewPointOnEdge1)
			{
				// retrieve triangle and IntegerPos along edge1 according to distFromNodeToPointOnEdge1
				rayCastToPointOfEdge1.Initialize(prevNodePos, newEdgeStart.Get2d() - prevNodePos.Get2d());
				rayCastToPointOfEdge1.SetStartIntegerPos(prevNode->m_nodeIntegerPos);
				rayCastToPointOfEdge1.SetStartTrianglePtr(prevTrianglePtr);
				rayCastToPointOfEdge1.PerformQueryWithInputCoordPos(m_workingMemory);
				++cangoTestDone;

				if (rayCastToPointOfEdge1.GetResult() != RAYCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED)
					return KY_SUCCESS;

				InitRayCanGoWithCost(oneWayCanGoWithCost, prevNodePos, prevTrianglePtr, rayCastToPointOfEdge1.GetArrivalPos(),
					prevNode->m_nodeIntegerPos, rayCastToPointOfEdge1.GetArrivalIntegerPos());
				oneWayCanGoWithCost.PerformQueryWithInputCoordPos(m_workingMemory);
				++cangoTestDone;

				if (oneWayCanGoWithCost.GetResult() != RAYCANGO_DONE_SUCCESS)
					// we skip this node since we are very likely to be too close from the wall.
					return KY_SUCCESS;

				costFromPrevToPointOnEdge1 = oneWayCanGoWithCost.GetComputedCost();
			}

			if (tryANewPointOnEdge2)
			{
				// retrieve triangle and IntegerPos along edge2
				rayCastToPointOfEdge2.Initialize(currentNodePos, newEdgeEnd.Get2d() - currentNodePos.Get2d());
				rayCastToPointOfEdge2.SetStartIntegerPos(currentNode->m_nodeIntegerPos);
				rayCastToPointOfEdge2.SetStartTrianglePtr(nodeTrianglePtr);
				rayCastToPointOfEdge2.PerformQueryWithInputCoordPos(m_workingMemory);
				++cangoTestDone;

				if (rayCastToPointOfEdge2.GetResult() != RAYCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED)
					return KY_SUCCESS;

				InitRayCanGoWithCost(oneWayCanGoWithCost, rayCastToPointOfEdge2.GetArrivalPos(), rayCastToPointOfEdge2.GetArrivalTrianglePtr(), nextNodePos,
					rayCastToPointOfEdge2.GetArrivalIntegerPos(), nextNode->m_nodeIntegerPos);
				oneWayCanGoWithCost.PerformQueryWithInputCoordPos(m_workingMemory);
				++cangoTestDone;

				if (oneWayCanGoWithCost.GetResult() != RAYCANGO_DONE_SUCCESS)
					// we skip this node since we are very likely to be too close from the wall.
					return KY_SUCCESS;

				costFromPointOnEdge2ToNextNode = oneWayCanGoWithCost.GetComputedCost();
			}

			if (tryANewPointOnEdge1 && tryANewPointOnEdge2)
			{
				InitRayCanGoWithCost(oneWayCanGoWithCost, rayCastToPointOfEdge1.GetArrivalPos(), rayCastToPointOfEdge1.GetArrivalTrianglePtr(),
					rayCastToPointOfEdge2.GetArrivalPos(), rayCastToPointOfEdge1.GetArrivalIntegerPos(), rayCastToPointOfEdge2.GetArrivalIntegerPos());
				oneWayCanGoWithCost.PerformQueryWithInputCoordPos(m_workingMemory);
				++cangoTestDone;

				if (oneWayCanGoWithCost.GetResult() == RAYCANGO_DONE_SUCCESS)
				{
					const KyFloat32 costFromPointOnEdge1ToPointOnEdge2 = oneWayCanGoWithCost.GetComputedCost();
					if (costFromPrevToPointOnEdge1 + costFromPointOnEdge1ToPointOnEdge2 + costFromPointOnEdge2ToNextNode <= currentCostFromPrevToNext)
					{
						// The newEdge is OK as replacement for the current node.
						// We move the current  node to the new edge start position
						// and add a new node at new edge end position.

						const RefinerNodeIndex newNodeIdx = (RefinerNodeIndex)refinerNodes.GetCount();
						if(KY_FAILED(refinerNodes.PushBack(RefinerNode())))
							return KY_ERROR;

						currentNode = &refinerNodes[currentNodeIndex];
						nextNode = &refinerNodes[nextNodeIdx];
						RefinerNode* newNode = &refinerNodes[newNodeIdx];

						triangleRawPtrNodes[currentNode->GetIdxOfRawPtrData()] = rayCastToPointOfEdge1.GetArrivalTrianglePtr().GetRawPtr();

						const KyUInt32 newNodeRawPtrDataIdx = triangleRawPtrNodes.GetCount();
						if(KY_FAILED(triangleRawPtrNodes.PushBack(rayCastToPointOfEdge2.GetArrivalTrianglePtr().GetRawPtr())))
							return KY_ERROR;

						currentNode->m_nodePosition = rayCastToPointOfEdge1.GetArrivalPos();
						currentNode->m_nodeIntegerPos = rayCastToPointOfEdge1.GetArrivalIntegerPos();

						newNode->m_nodePosition = rayCastToPointOfEdge2.GetArrivalPos();
						newNode->m_nodeIntegerPos = rayCastToPointOfEdge2.GetArrivalIntegerPos();

						newNode->SetNodeType(NodeType_NavMeshEdge);
						newNode->SetIdxOfRawPtrData(newNodeRawPtrDataIdx);

						currentNode->m_nextNodeIdx = newNodeIdx;
						newNode->m_predecessorNodeIdx = currentNodeIndex;

						newNode->m_nextNodeIdx = nextNodeIdx;
						nextNode->m_predecessorNodeIdx = newNodeIdx;

						currentNode->m_costFromPredecessor = costFromPrevToPointOnEdge1;
						newNode->m_costFromPredecessor = costFromPointOnEdge1ToPointOnEdge2;
						nextNode->m_costFromPredecessor = costFromPointOnEdge2ToNextNode;

						KY_FORWARD_ERROR_NO_LOG(UpdateNodeInBinaryHeap(currentNodeIndex, currentNode));
						KY_FORWARD_ERROR_NO_LOG(UpdateNodeInBinaryHeap(newNodeIdx, newNode));
						return KY_SUCCESS;
					}
				}
#ifdef DBG_SendRefineOneNodeVisualDebug
				else
				{
					ScopedDisplayList displayList_NewEdgeCdt(m_database->GetWorld());
					displayList_NewEdgeCdt.InitSingleFrameLifespan("Result - NewEdgeCdt", "RefineOneNode");

					const Vec3f bordersRenderOffset(0.0f, 0.0f, 0.03f);
					displayList_NewEdgeCdt.PushLine(newEdgeStart + bordersRenderOffset, newEdgeEnd + bordersRenderOffset, VisualColor::Red);
				}
#endif
			}


			if (tryANewPointOnEdge1)
			{
				InitRayCanGoWithCost(oneWayCanGoWithCost, rayCastToPointOfEdge1.GetArrivalPos(), rayCastToPointOfEdge1.GetArrivalTrianglePtr(),
					nextNodePos, rayCastToPointOfEdge1.GetArrivalIntegerPos(), nextNode->m_nodeIntegerPos);
				oneWayCanGoWithCost.PerformQueryWithInputCoordPos(m_workingMemory);
				++cangoTestDone;

				if (oneWayCanGoWithCost.GetResult() == RAYCANGO_DONE_SUCCESS)
				{
					// Moving current node to an optimized position along previous edge is OK, let's do it.

					const KyFloat32 costFromPointOnEdge1TonextNode = oneWayCanGoWithCost.GetComputedCost();
					if (costFromPrevToPointOnEdge1 + costFromPointOnEdge1TonextNode <= currentCostFromPrevToNext)
					{
						currentNode->m_nodePosition = rayCastToPointOfEdge1.GetArrivalPos();
						currentNode->m_nodeIntegerPos = rayCastToPointOfEdge1.GetArrivalIntegerPos();
						currentNode->m_costFromPredecessor = costFromPrevToPointOnEdge1;
						nextNode->m_costFromPredecessor = costFromPointOnEdge1TonextNode;

						triangleRawPtrNodes[currentNode->GetIdxOfRawPtrData()] = rayCastToPointOfEdge1.GetArrivalTrianglePtr().GetRawPtr();

						// angle did not change for prevNode
						KY_FORWARD_ERROR_NO_LOG(UpdateNodeInBinaryHeap(currentNodeIndex, currentNode));
						KY_FORWARD_ERROR_NO_LOG(UpdateNodeInBinaryHeap(nextNodeIdx, nextNode));
						return KY_SUCCESS;
					}
				}
#ifdef DBG_SendRefineOneNodeVisualDebug
				else
				{
					ScopedDisplayList displayList_NewEdgeCdt(m_database->GetWorld());
					displayList_NewEdgeCdt.InitSingleFrameLifespan("Result - NewEdgeCdt", "RefineOneNode");

					const Vec3f bordersRenderOffset(0.0f, 0.0f, 0.03f);
					displayList_NewEdgeCdt.PushLine(newEdgeStart + bordersRenderOffset, newEdgeEnd + bordersRenderOffset, VisualColor::Red);
				}
#endif
			}

			if (tryANewPointOnEdge2)
			{
				InitRayCanGoWithCost(oneWayCanGoWithCost, prevNodePos, prevTrianglePtr, rayCastToPointOfEdge2.GetArrivalPos(),
					prevNode->m_nodeIntegerPos, rayCastToPointOfEdge2.GetArrivalIntegerPos());
				oneWayCanGoWithCost.PerformQueryWithInputCoordPos(m_workingMemory);
				++cangoTestDone;

				if (oneWayCanGoWithCost.GetResult() == RAYCANGO_DONE_SUCCESS)
				{
					const KyFloat32 costFromPrevNodeToPointOnEdge2 = oneWayCanGoWithCost.GetComputedCost();
					if (costFromPrevNodeToPointOnEdge2 + costFromPointOnEdge2ToNextNode <= currentCostFromPrevToNext)
					{
						// Moving current node to an optimized position along next edge is OK, let's do it.

						currentNode->m_nodePosition = rayCastToPointOfEdge2.GetArrivalPos();
						currentNode->m_nodeIntegerPos = rayCastToPointOfEdge2.GetArrivalIntegerPos();
						currentNode->m_costFromPredecessor = costFromPrevNodeToPointOnEdge2;
						nextNode->m_costFromPredecessor = costFromPointOnEdge2ToNextNode;

						triangleRawPtrNodes[currentNode->GetIdxOfRawPtrData()] = rayCastToPointOfEdge2.GetArrivalTrianglePtr().GetRawPtr();

						// angle did not change for nextNode
						KY_FORWARD_ERROR_NO_LOG(UpdateNodeInBinaryHeap(prevNodeIdx, prevNode));
						KY_FORWARD_ERROR_NO_LOG(UpdateNodeInBinaryHeap(currentNodeIndex, currentNode));
						return KY_SUCCESS;
					}
				}
#ifdef DBG_SendRefineOneNodeVisualDebug
				else
				{
					ScopedDisplayList displayList_NewEdgeCdt(m_database->GetWorld());
					displayList_NewEdgeCdt.InitSingleFrameLifespan("Result - NewEdgeCdt", "RefineOneNode");

					const Vec3f bordersRenderOffset(0.0f, 0.0f, 0.03f);
					displayList_NewEdgeCdt.PushLine(newEdgeStart + bordersRenderOffset, newEdgeEnd + bordersRenderOffset, VisualColor::Red);
				}
#endif
			}
		}
		// End new edge validation
		//////////////////////////////////////////////////////////////////////////
	}

	return KY_SUCCESS;
}

template <class TLogic>
Ptr<Path> PathFinderQueryUtils::ComputePathFromPathClamperContext(const Vec3f& realStartPos, const Vec3f& realEndPos,
	const PositionSpatializationRange& positionSpatializationRange)
{
	KY_SCOPED_PERF_MARKER("Navigation PathFinderQuery ComputePath");
	PathClamperContext* clamperContext = m_workingMemory->m_clamperContext;
	WorkingMemArray<ClampNode>& clampNodes = clamperContext->m_clampNodes;
	WorkingMemArray<NavGraphVertexRawPtr>& vertexRawPtrNodes = clamperContext->m_vertexRawPtrNodes;
	WorkingMemArray<NavTriangleRawPtr>& triangleRawPtrNodes = clamperContext->m_triangleRawPtrNodes;
	KY_ASSERT(clampNodes.GetCount() >= 2);

	const DatabaseGenMetrics& genMetrics = m_database->GetDatabaseGenMetrics();

	bool mustAddPointAtPathEnd = false;
	bool mustAddPointAtPathBegin = false;

	// COUNT EDGES iterate on current edge [A,B]
	KyUInt32 nodesCount = 1;
	ClampNode* A = KY_NULL;
	ClampNode* B = &clampNodes[0]; // destination


	if (B->m_nodePosition != realEndPos)
	{
		mustAddPointAtPathEnd = true;
		++nodesCount;
	}

	while (B->m_predecessorNodeIdx != ClampNodeIndex_Invalid)
	{
		B = &clampNodes[B->m_predecessorNodeIdx];
		++nodesCount;
	}

	if (B->m_nodePosition != realStartPos) 
	{
		mustAddPointAtPathBegin = true;
		++nodesCount;
	}

	// ALLOC
	Path::CreateConfig config;
	config.m_nodesCount = nodesCount;
	Ptr<Path> pathPtr = Path::CreatePath(config);
	Path& path = *pathPtr;
	KyFloat32 currentDistance = 0.f;
	KyFloat32 currentCost = 0.f;
	RayCanGoQuery<TLogic> rayCanGo;
	rayCanGo.BindToDatabase(m_database);
	rayCanGo.SetPositionSpatializationRange(positionSpatializationRange);
	rayCanGo.SetTraverseLogicUserData(GetTraverseLogicUserData());
	rayCanGo.SetComputeCostMode(QUERY_COMPUTE_COST_ALONG_3DAXIS);

	// FILL iterate on current edge [A,B]
	KyUInt32 nodeIdx = nodesCount - 1;

	B = &clampNodes[0]; // destination
	PathNodeType sucessorNodeType = B->GetNodeType();

	if (mustAddPointAtPathEnd == true)
	{
		// this happen in two cases : we started from outside the navmesh or we started from a NavGraph vertex with a startPos
		// different from the NavGraphVertex Position
		const WorldIntegerPos integerPos = genMetrics.GetWorldIntegerPosFromVec3f(realEndPos);
		path.SetNodePosition3fAndInteger(nodeIdx, realEndPos, integerPos); // real outside destination
		path.GetPathCellBox().ExpandByVec2(integerPos.m_cellPos);
		const KyFloat32 edgeDistance = Distance(B->m_nodePosition, realEndPos);

		if (sucessorNodeType == NodeType_NavGraphVertex)
			path.SetPathEdgeType(nodeIdx - 1, PathEdgeType_FromNavGraphToOutside);
		else
			path.SetPathEdgeType(nodeIdx - 1, PathEdgeType_FromNavMeshToOutside);

		// for these edges, cost == distance since there is no associated navTag
		currentDistance += edgeDistance;
		currentCost += edgeDistance;

		--nodeIdx;
	}

	KyUInt32 nodeIdxBreakCondition = mustAddPointAtPathBegin ? 1 : 0;
	path.SetNodePosition3fAndInteger(nodeIdx, B->m_nodePosition, B->m_nodeIntegerPos); // destination
	path.GetPathCellBox().ExpandByVec2(B->m_nodeIntegerPos.m_cellPos);
	switch(sucessorNodeType)
	{
	case NodeType_FreePositionOnNavMesh :
		{
			path.SetNodeNavTrianglePtr(nodeIdx, NavTrianglePtr(triangleRawPtrNodes[B->GetIdxOfRawPtrData()]));
			break;
		}
	case NodeType_NavGraphVertex :
		{
			const NavGraphVertexRawPtr& nodeGraphVertexRawPtr = vertexRawPtrNodes[B->GetIdxOfRawPtrData()];
			if(nodeGraphVertexRawPtr.IsValid())
			{
				path.SetNodeNavGraphVertexPtr(nodeIdx, NavGraphVertexPtr(nodeGraphVertexRawPtr));

				NavGraphLink* navGraphLink = nodeGraphVertexRawPtr.GetGraphVertexData().m_navGraphLink;
				if (navGraphLink != KY_NULL && navGraphLink->IsValid())
					// graphVertex is connected to theNavMesh, we can retrieve the triangle in which it is linked
					path.SetNodeNavTrianglePtr(nodeIdx, NavTrianglePtr(navGraphLink->m_navTriangleRawPtr));
			}
		}
	default:
		break;
	}

	--nodeIdx;

	for(;;)
	{
		A = &clampNodes[B->m_predecessorNodeIdx];
		PathNodeType currentNodeType = A->GetNodeType();
		const Vec3f& egdeStartPos = A->m_nodePosition;
		const Vec3f& egdeEndPos = B->m_nodePosition;
		path.SetNodePosition3fAndInteger(nodeIdx, egdeStartPos, A->m_nodeIntegerPos);
		path.GetPathCellBox().ExpandByVec2(A->m_nodeIntegerPos.m_cellPos);
		const KyFloat32 edgeDistance = Distance(egdeStartPos, egdeEndPos);
		KyFloat32 edgeCost = KyFloat32MAXVAL;
		path.SetPathEdgeType(nodeIdx, PathEdgeType_OnNavMesh);// pathEdgeType is set to PathEdgeType_OnNavMesh and will be overwritten after if needed

		switch(currentNodeType)
		{
		case NodeType_FreePositionOnNavMesh :
		case NodeType_NavMeshEdge :
			{
				const NavTrianglePtr startTrianglePtr(triangleRawPtrNodes[A->GetIdxOfRawPtrData()]);

				rayCanGo.Initialize(egdeStartPos, egdeEndPos);
				rayCanGo.SetStartTrianglePtr(startTrianglePtr);
				rayCanGo.SetStartIntegerPos(A->m_nodeIntegerPos);
				rayCanGo.SetDestIntegerPos(B->m_nodeIntegerPos);
				rayCanGo.SetPerformQueryStat(m_performQueryStat);
				rayCanGo.PerformQueryWithInputCoordPos(m_workingMemory);
				if (rayCanGo.GetResult() != RAYCANGO_DONE_SUCCESS)
				{
					KY_LOG_ERROR(("Error in path cost computation, the canGo along the edge must be successful. No cost multiplier is applied to the PathEdge"));
					edgeCost = edgeDistance;
				}
				else
					edgeCost = rayCanGo.GetComputedCost();

				path.SetNodeNavTrianglePtr(nodeIdx, startTrianglePtr);
				KY_ASSERT(path.GetNodeNavTrianglePtr(nodeIdx).IsValid());
				break;
			}
		case NodeType_NavGraphVertex :
			{
				const NavGraphVertexRawPtr& nodeGraphVertexRawPtr = vertexRawPtrNodes[A->GetIdxOfRawPtrData()];
				NavGraphLink* navGraphLink = nodeGraphVertexRawPtr.GetGraphVertexData().m_navGraphLink;

				path.SetNodeNavGraphVertexPtr(nodeIdx, NavGraphVertexPtr(nodeGraphVertexRawPtr));

				if (navGraphLink != KY_NULL && navGraphLink->IsValid())
				{
					// graphVertex is connected to theNavMesh, we can retrieve the triangle in which it is linked
					path.SetNodeNavTrianglePtr(nodeIdx, NavTrianglePtr(navGraphLink->m_navTriangleRawPtr));
				}


				if (sucessorNodeType == NodeType_NavGraphVertex)
				{
					// the edge goes from a graphVertex to an other grapHVertex, check if they are on the same graph
					const NavGraphVertexRawPtr& predecessorGraphVertexRawPtr = vertexRawPtrNodes[B->GetIdxOfRawPtrData()];
					if (nodeGraphVertexRawPtr.GetNavGraph() == predecessorGraphVertexRawPtr.GetNavGraph())
					{
						// the two vertices belong to the same Point Graph, check if the edge is not an edge of the NavGraphBlob
						const NavGraphVertexIdx nextVertexIdx = predecessorGraphVertexRawPtr.GetNavGraphVertexIdx();
						const NavGraphVertex& startNavGraphVertex = nodeGraphVertexRawPtr.GetNavGraphBlob()->GetNavGraphVertex(nodeGraphVertexRawPtr.m_vertexIdx);
						const KyUInt32 neighborCount = startNavGraphVertex.GetNeighborVertexCount();

						for (KyUInt32 i = 0; i < neighborCount; ++i)
						{
							if (startNavGraphVertex.GetNeighborVertexIdx(i) == nextVertexIdx)
							{
								const NavGraphEdgePtr navGraphEdgePtr(nodeGraphVertexRawPtr.GetNavGraph(), NavGraphEdgeSmartIdx(nodeGraphVertexRawPtr.GetNavGraphVertexIdx(), i));
								path.SetEdgeNavGraphEdgePtr(nodeIdx, navGraphEdgePtr);
								path.SetPathEdgeType(nodeIdx, PathEdgeType_OnNavGraph);

								KyFloat32 costMultiplier = 1.f;
								const NavGraphEdgeRawPtr edgeRawPtr = navGraphEdgePtr.GetRawPtr();
								bool canTraverse = edgeRawPtr.CanBeTraversed<TLogic>(GetTraverseLogicUserData(), &costMultiplier);
								edgeCost = edgeDistance * costMultiplier;

								KY_UNUSED(canTraverse);
								KY_LOG_ERROR_IF(canTraverse == false || edgeCost == KyFloat32MAXVAL,
									("Error in path cost computation, the navGraphEdge must be traversable with a valid cost"));
								break;
							}
						}
					}
				}
				else if (sucessorNodeType == NodeType_FreePositionOnGraphEdge)
				{
					KY_LOG_ERROR_IF(nodeGraphVertexRawPtr.GetNavGraph() != vertexRawPtrNodes[B->GetIdxOfRawPtrData()].GetNavGraph(), ("Error, they should be in the same graph !"));
					KY_ASSERT(clamperContext->m_destNavGraphEdgePtr.IsValid());
					path.SetPathEdgeType(nodeIdx, PathEdgeType_FromNavGraphToOutside);
					path.SetEdgeNavGraphEdgePtr(nodeIdx, clamperContext->m_destNavGraphEdgePtr);
					edgeCost = edgeDistance;
				}

				// the pathEdgeType has not been changed, we are on the NavMesh. Wa can compute Cost from a CanGo
				if (path.GetPathEdgeType(nodeIdx) == PathEdgeType_OnNavMesh)
				{
					const NavTrianglePtr& startTrianglePtr = path.GetNodeNavTrianglePtr(nodeIdx);
					KY_LOG_ERROR_IF(startTrianglePtr.IsValid() == false, ("We should have associate a valid NavTrianglePtr to this pathNode since it lies on NavMesh"));
					rayCanGo.Initialize(egdeStartPos, egdeEndPos);
					rayCanGo.SetStartTrianglePtr(startTrianglePtr);
					rayCanGo.SetStartIntegerPos(A->m_nodeIntegerPos);
					rayCanGo.SetDestIntegerPos(B->m_nodeIntegerPos);
					rayCanGo.SetPerformQueryStat(m_performQueryStat);
					rayCanGo.PerformQueryWithInputCoordPos(m_workingMemory);
					if (rayCanGo.GetResult() != RAYCANGO_DONE_SUCCESS)
					{
						KY_LOG_ERROR(("Error in path cost computation, the canGo along the edge must be successful. No cost multiplier is applied to the PathEdge"));
						edgeCost = edgeDistance;
					}
					else
						edgeCost = rayCanGo.GetComputedCost();
				}
				break;
			}
		case NodeType_FreePositionOnGraphEdge :
			{
				// since the destination node has already been processed, if this happend, we are on the startNode
				KY_ASSERT(nodeIdx == 0);
				KY_ASSERT(clamperContext->m_startNavGraphEdgePtr.IsValid());
				KY_ASSERT(mustAddPointAtPathBegin == false);
				KY_ASSERT(sucessorNodeType == NodeType_NavGraphVertex || sucessorNodeType == NodeType_FreePositionOnGraphEdge); // from an edge we always cross a GraphVertex
				if (sucessorNodeType == NodeType_NavGraphVertex)
				{
					if(clamperContext->m_startNavGraphEdgePtr.GetStartNavGraphVertexPosition() == egdeEndPos)
					{
						NavGraphEdgePtr oppositeWay = clamperContext->m_startNavGraphEdgePtr.GetOppositeNavGraphEdgePtr();
						KY_ASSERT(oppositeWay.IsValid());
						path.SetEdgeNavGraphEdgePtr(nodeIdx, oppositeWay);
					}
					else 
					{
						path.SetEdgeNavGraphEdgePtr(nodeIdx, clamperContext->m_startNavGraphEdgePtr);
					}
					path.SetPathEdgeType(nodeIdx, PathEdgeType_FromOutsideToNavGraph);
					edgeCost = edgeDistance;
				}
				else
				{
					KY_ASSERT(clamperContext->m_destNavGraphEdgePtr.IsValid());
					path.SetEdgeNavGraphEdgePtr(nodeIdx, clamperContext->m_startNavGraphEdgePtr);
					path.SetPathEdgeType(nodeIdx, PathEdgeType_OutsideAlongGraphEdge);

					KyFloat32 costMultiplier = 1.f;
					const NavGraphEdgeRawPtr destEdgeRawPtr = clamperContext->m_destNavGraphEdgePtr.GetRawPtr();
					bool canTraverse = destEdgeRawPtr.CanBeTraversed<TLogic>(GetTraverseLogicUserData(), &costMultiplier);
					edgeCost = edgeDistance * costMultiplier;

					KY_UNUSED(canTraverse);
					KY_LOG_ERROR_IF(canTraverse == false || edgeCost == KyFloat32MAXVAL,
						("Error in path cost computation, the navGraphEdge must be traversable with a valid cost"));
				}

				break;
			}
		}

		B = A;
		sucessorNodeType = currentNodeType;

		currentDistance += edgeDistance;
		currentCost += edgeCost;

		if (nodeIdx == nodeIdxBreakCondition)
			break;

		--nodeIdx;
	}


	if (mustAddPointAtPathBegin == true)
	{
		--nodeIdx;
		const WorldIntegerPos integerPos = genMetrics.GetWorldIntegerPosFromVec3f(realStartPos);
		path.SetNodePosition3fAndInteger(nodeIdx, realStartPos, integerPos); // real outside start
		path.GetPathCellBox().ExpandByVec2(integerPos.m_cellPos);
		const KyFloat32 edgeDistance = Distance(realStartPos, B->m_nodePosition);

		if (sucessorNodeType == NodeType_NavGraphVertex)
			path.SetPathEdgeType(nodeIdx, PathEdgeType_FromOutsideToNavGraph);
		else
			path.SetPathEdgeType(nodeIdx, PathEdgeType_FromOutsideToNavMesh);

		currentDistance += edgeDistance;
		currentCost += edgeDistance;
	}

	KY_ASSERT(nodeIdx == 0);

	path.m_database = m_database;
	path.SetPathCostAndDistance(currentCost, currentDistance);
	return pathPtr;
}
}

