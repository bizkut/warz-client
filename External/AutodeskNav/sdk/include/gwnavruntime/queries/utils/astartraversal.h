/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_AStarTraversal_H
#define Navigation_AStarTraversal_H

#include "gwnavruntime/queries/utils/queryutils.h"
#include "gwnavruntime/queries/utils/astartraversalcontext.h"

#include "gwnavruntime/navmesh/navfloortonavgraphlinks.h"
#include "gwnavruntime/navmesh/intersections.h"
#include "gwnavruntime/navmesh/closestpoint.h"

#include "gwnavruntime/abstractgraph/abstractgraphutils.h"

#include "gwnavruntime/database/database.h"

#include "gwnavruntime/basesystem/iperfmarkerinterface.h"

namespace Kaim
{

class TraversalVisitNodeContext
{
public:
	TraversalVisitNodeContext() {}
	void Clear();

public:
	NavGraphVertexRawPtr m_visitedGraphVertexRawPtr;
	AStarNodeIndex m_visitedNodeIndex;
};

template<class TraversalCustomzier>
class AStarTraversal
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Query)
public:
	typedef typename TraversalCustomzier::TLogic TraverseLogic;

	// Constructs a new instance of this class. 
	AStarTraversal() : m_traversalCustomizer(), m_astarContext(KY_NULL), m_traversalParams(KY_NULL) {}
	AStarTraversal(const TraversalCustomzier& customizer) : m_traversalCustomizer(customizer), m_astarContext(KY_NULL), m_traversalParams(KY_NULL) {}

	KyResult InitializeContextAndStartNode(QueryUtils& queryUtils, const Vec3f& startPos3f, const NavTrianglePtr& startTrianglePtr);
	KyResult InitializeContextAndStartNode(QueryUtils& queryUtils, const Vec3f& startPos3f, const NavGraphEdgePtr& startNavGraphEdgePtr);
	KyResult InitializeContextAndStartNode(QueryUtils& queryUtils, const Vec3f& startPos3f, const NavGraphVertexPtr& startNavGraphVertexPtr);

	// must be called after the StartNode initialisation
	KyResult InitializeDestNode(const Vec3f& destPos, const NavTrianglePtr& destTrianglePtr);
	KyResult InitializeDestNode(const Vec3f& destPos, const NavGraphEdgePtr& destNavGraphEdgePtr);
	KyResult InitializeDestNode(const NavGraphVertexPtr& destNavGraphVertexPtr);

	KyResult ExploreAllNodesInTriangle(ActiveData* activeData, const NavTriangleRawPtr& triangleRawPtr, AStarNodeIndex currentNodeIndex);
	// used only when we start the traversal from a NavGraphEdge
	KyResult ExploreAllNodesInNavGraphEdge(ActiveData* activeData, const Vec3f& posOnEdge, const NavGraphEdgeRawPtr& navGraphEdgeRawPtr, AStarNodeIndex currentNodeIndex, NavGraphEdgeDirection m_navGraphEdgePathfindMode);

	KY_INLINE bool IsThereNodeToVisit() { return m_astarContext->m_traversalBinHeap.IsEmpty() == false; }

	// Performs a single iteration of the AStar Algorithm. 
	KyResult VisitNode(QueryUtils& queryUtils, TraversalVisitNodeContext& visitNodeContext);
	KyResult UpdateOpenedOrClosedNode(AStarNodeIndex nodeIndex, AStarNodeIndex predecessorIndex, KyFloat32 costFromCurrentNode);

private:
	KyResult ExploreNeighborsOfHalfEdgeNode(ActiveData* activeData, AStarNodeIndex indexOfOpenWithLowerCost, const NavHalfEdgeRawPtr& currentEdgeRawPtr,
		const NavHalfEdgeRawPtr& pairHalfEdgeRawPtr, const Vec3f& nodePosition, const LogicDoNotUseCanEnterNavTag&);
	KyResult ExploreNeighborsOfHalfEdgeNode(ActiveData* activeData, AStarNodeIndex indexOfOpenWithLowerCost, const NavHalfEdgeRawPtr& currentEdgeRawPtr,
		const NavHalfEdgeRawPtr& pairHalfEdgeRawPtr, const Vec3f& nodePosition, const LogicDoUseCanEnterNavTag&);

	KyResult ExploreNeighborsOfHalfEdgeNode(ActiveData* activeData, AStarNodeIndex indexOfOpenWithLowerCost, const NavHalfEdgeRawPtr& halfEdgeRawPtr);
	KyResult ExploreNeihgborsOfGraphVertexNode(ActiveData* activeData, AStarNodeIndex indexOfOpenWithLowerCost, NavGraphVertexRawPtr& visitedGraphVertexRawPtr);
	KyResult ExploreNeihgborsOfAbstractGraphNode(AStarNodeIndex currentCostNodeIndex, const AbstractGraphNodeRawPtr& currentAbstractNodeRawPtr);

	KyResult ExploreHalfEdgesOfTriangle(ActiveData* activeData, const NavTriangleRawPtr& triangleRawPtr, AStarNodeIndex currentNodeIndex);
	KyResult ExploreGraphVerticesInTriangle(const NavTriangleRawPtr& triangleRawPtr, AStarNodeIndex currentNodeIndex);
	
	KyResult ExploreAbstractGraphNodesOnNavHalEdgeRawPtr(const NavHalfEdgeRawPtr& navHalfEdgeRawPtr, AStarNodeIndex currentNodeIndex);

	KyResult CreateNewHalfEdgeNode(ActiveData* activeData, const NavHalfEdgeRawPtr& HalfEdge, const NavHalfEdgeRawPtr& pairHalfEdge,
		const Vec3f& startPosOfEdge, const Vec3f& endPosOfEdge, AStarNodeIndex predecessorIndex);

	KyResult CreateNewGraphVertexNode(const NavGraphVertexRawPtr& navGraphVertexRawPtr, AStarNodeIndex predecessorIndex, KyFloat32 costFromCurrentNode);
	KyResult CreateNewAbstractGraphNode(const AbstractGraphNodeRawPtr& abstractGraphNodeRawPtr, const AbstractGraphNodeRawPtr& pairedAbstractGraphNodeRawPtr, 
		AStarNodeIndex predecessorNodeIndex, KyFloat32 costFromCurrentNode);

	KyResult OpenOrUpdateGraphVertex(const NavGraphVertexRawPtr& navGraphVertexRawPtr, AStarNodeIndex vertexNodeIndex, AStarNodeIndex currentNodeIndex, KyFloat32 costFromCurrentNode);

	KyResult OpenOrUpdateHalfEdge(ActiveData* activeData, const NavHalfEdgeRawPtr& halfEdge, AStarNodeIndex halfEdgeNodeIndex,
		const Vec3f& startPosOfEdge, const Vec3f& endPosOfEdge, AStarNodeIndex currentNodeIndex);

	KyResult OpenOrUpdateAbstractGraphNode(const AbstractGraphNodeRawPtr& neighborAbstractNodeRawPtr, 
		AStarNodeIndex neighborAstarNodeIndex, AStarNodeIndex currentNodeIndex, KyFloat32 costFromCurrentNode);

	bool ShouldOpenHalfEdgeNode(const NavHalfEdgeRawPtr& halfEdge, NavHalfEdgeRawPtr& pairHalfEdgeOfNewNode);
	bool ShouldOpenGraphVertexNode(const NavGraphVertexRawPtr& navGraphVertexRawPtr);
	bool ShouldOpenAbstractGraphNode(const AbstractGraphNodeRawPtr& abstractGraphNodeRawPtr, AbstractGraphNodeRawPtr& pairedAbstractGraphNodeRawPtr);

	bool CanTraverseNavTriangle(const NavTriangleRawPtr& triangle);
	bool CanTraverseNavTriangleAndGetCost(const NavTriangleRawPtr& triangle, const Vec3f& A, const Vec3f& B, KyFloat32& cost);
	bool CanTraverseGraphEdgeAndGetCost(const NavGraphEdgeRawPtr& edge, const Vec3f& A, const Vec3f& B, KyFloat32& cost);

	KyFloat32 EvaluateCostToDest(const Vec3f& nodePosition);

public: // internal
	KyResult InitializeInvalidDestNode();

public:
	TraversalCustomzier m_traversalCustomizer;
public:
	AStarTraversalContext* m_astarContext;
	TraversalParameters* m_traversalParams;
};

}

#include "gwnavruntime/queries/utils/astartraversal.inl"

#endif //Navigation_AStarTraversal_H

