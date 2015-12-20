/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_MultiDestinationPathFinderQuery_H
#define Navigation_MultiDestinationPathFinderQuery_H

#include "gwnavruntime/queries/utils/basemultidestinationpathfinderquery.h"
#include "gwnavruntime/queries/utils/astartraversal.h"
#include "gwnavruntime/queries/utils/astartraversalcontext.h"
#include "gwnavruntime/queries/raycangoquery.h"
#include "gwnavruntime/database/database.h"
#include "gwnavruntime/path/path.h"
#include "gwnavruntime/channel/channelarraycomputer.h"
#include "gwnavruntime/math/box2iiterator.h"


namespace Kaim
{

template<class TraverseLogic>
class MultiDestinationPathFinderQuery : public BaseMultiDestinationPathFinderQuery
{
public:

	// ---------------------------------- Public Member Functions ----------------------------------

	MultiDestinationPathFinderQuery();
	virtual ~MultiDestinationPathFinderQuery();

	// ---------------------------------- Query framework functions ----------------------------------

	/// Performs a single iteration of the PathFinder.
	/// \copydoc IQuery::Advance()
	virtual void Advance(WorkingMemory* workingMemory);

	virtual void ReleaseWorkingMemoryOnCancelDuringProcess(WorkingMemory* workingMemory = KY_NULL);

public: //Internal
	bool HasNavDataChanged();

protected:
	void SetFinish(WorkingMemory* workingMemory);
	KyResult ComputeStartGraphEdgeOrGraphVertexOrTriangle(PathFinderQueryUtils& queryUtils);
	KyResult ComputeStartTriangle(PathFinderQueryUtils& queryUtils);
	KyResult ComputeDestTriangleIfNeeded(PathFinderQueryUtils& queryUtils);
	KyResult InitializeContextAndStartNode(PathFinderQueryUtils& queryUtils);
	KyResult InitAllDestinationNodes();
	KyResult InitDestinationNodeFromDestNavGraphEdge(PathFinderPoint& dest);
	KyResult InitDestinationNodeFromDestNavGraphVertex(PathFinderPoint& dest);
	KyResult InitDestinationNodeFromDestNavTriangle(PathFinderPoint& dest);
	KyResult InitTraversalFromStartNode(PathFinderQueryUtils& queryUtils);
	void InitPropagationBounds();

	KyResult CheckNavDataChangeFromAstarContext(WorkingMemory* workingMemory);
	KyResult CheckNavDataChange(WorkingMemory* workingMemory);
	
	bool ShouldReturnAfterTryingCanGo(WorkingMemory* workingMemory);

	bool HasExplorationReachedAllDestNodes();
	bool HasExplorationReachedAtLeastADestNode();
	
	void ComputeRawCosts();
	void BuildRawPaths();
	PathEdgeType GetPathEdgeType(const AStarNode& predecessorNode, const AStarNode& node);

	bool ProcessFirstDestination();
	bool UpdateResultAndSelectFirstDestination();
	bool ProcessNextDestination(WorkingMemory* workingMemory);

	KyUInt32 CountAbstractGraphNodes(WorkingMemory* workingMemory, AStarNodeIndex destNodeIdx, AStarNodeIndex& concreteDestinationNodeIdx);
	void BuildAbstractPath(WorkingMemory* workingMemory, AStarNodeIndex& concreteDestAStarNodeIndex);

protected:
	class TraversalCustomizer
	{
	public:
		typedef TraverseLogic TLogic;

		TraversalCustomizer();

		bool CanTraverse(const NavTriangleRawPtr& triangleRawPtr, KyFloat32* costMultiplier);
		bool CanTraverse(const NavGraphEdgeRawPtr& edgeRawPtr, KyFloat32* costMultiplier);
		bool CanEnterNavTag(const NavTag& exitNavTag, const NavTag& enterNavTag, const Vec3f& pos);
		KyFloat32 GetHeuristicFromNodePosition(const Vec3f& nodePosition);

		void* GetTraverseLogicUserData() const { return m_traverseLogicUserData; }
		bool ShouldVisitNode(AStarTraversalContext* aStarTraversalContext, AStarNodeIndex nodeIndex);
		bool IsNodeOutsidePropagationBounds(AStarTraversalContext* aStarTraversalContext, AStarNodeIndex nodeIndex);
		bool ShouldOpenHalfEdgeNode(const NavHalfEdgeRawPtr& halfEdge, NavHalfEdgeRawPtr& pairHalfEdgeOfNewNode);
		bool ShouldOpenGraphVertexNode(const NavGraphVertexRawPtr& navGraphVertexRawPtr);
		bool ShouldOpenAbstractGraphNode(const AbstractGraphNodeRawPtr& abstractGraphNodeRawPtr);
		bool ShouldOpenNavTriangleUnderAbstractGraphNode(const AbstractGraphNodeRawPtr& abstractGraphNodeRawPtr);
		Vec3f ComputeNodePositionOnHalfEdge(AStarTraversalContext* aStarTraversalContext, const Vec3f& startPosOfEdge, const Vec3f& endPosOfEdge, AStarNodeIndex predecessorNodeIndex);
		KyResult OnNavTriangleExplored(AStarTraversalContext* aStarTraversalContext, const NavTriangleRawPtr& navTriangleRawPtr, AStarNodeIndex currentNodeIndex);
		KyResult OnNavGraphEdgeExplored(AStarTraversalContext* aStarTraversalContext, const NavGraphEdgeRawPtr& navGraphEdgeRawPtr, AStarNodeIndex currentNodeIndex);

		MultiDestinationPathFinderQuery<TraverseLogic>* m_query;
		void* m_traverseLogicUserData;
	};

	AStarTraversal<TraversalCustomizer> m_traversal;
	KyArray<MonodirectionalRayCanGoQuery<TraverseLogic> > m_rayCanGoQueries;
	KyUInt32 m_currentProcessedDestIdx;
};

}

#include "gwnavruntime/queries/multidestinationpathfinderquery.inl"

#endif //Navigation_MultiDestinationPathFinderQuery_H

