/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BestGraphVertexPathFinderQuery_H
#define Navigation_BestGraphVertexPathFinderQuery_H

#include "gwnavruntime/queries/utils/basebestgraphvertexpathfinderquery.h"
#include "gwnavruntime/queries/utils/pathclampercontext.h"
#include "gwnavruntime/queries/utils/astartraversal.h"
#include "gwnavruntime/channel/channelarraycomputer.h"


namespace Kaim
{

class PathFinderQueryUtils;

enum OnGraphVertexVisitedResult
{
	OnGraphVertexVisited_IgnoreVertex_GoOn,
	OnGraphVertexVisited_IgnoreVertex_StopWithBestSoFar,
	OnGraphVertexVisited_StopOnThisVertex,
	OnGraphVertexVisited_VertexIsTheBestSoFar_GoOn,
	OnGraphVertexVisited_NeedToCheckRefinePathToThisVertex
};

enum OnPathToGraphVertexComputedResult
{
	OnPathToGraphVertexComputed_IgnoreVertex_GoOn,
	OnPathToGraphVertexComputed_IgnoreVertex_StopWithBestSoFar,
	OnPathToGraphVertexComputed_StopOnThisVertex,
	OnPathToGraphVertexComputed_VertexIsTheBestSoFar_GoOn,
};

/// This query propagates outward through the NavMesh from the starting position. Each time the query
/// encounters a NavGraph vertex, it calls the customizer that you pass in the template argument to
/// determine whether that vertex should be chosen as the destination for the path, or whether the search
/// should continue.
/// The template argument should be a class that contains the same methods as the customizer accepted by
/// the TraverseLogic class, plus the following methods:
/// \code 
/// OnGraphVertexVisitedResult OnGraphVertexVisited(const NavGraphVertexRawPtr& currentGraphVertex, const NavGraphVertexRawPtr& bestGraphVertex, Ptr<Path> pathToBest)
/// OnPathToGraphVertexComputedResult OnPathToGraphVertexComputed(const NavGraphVertexRawPtr& currentGraphVertex, Ptr<Path> pathToCurrent, const NavGraphVertexRawPtr& bestGraphVertex, Ptr<Path> pathToBest)
/// \endcode
/// It's a common pattern to have VertexFinderLogic inherit from TraverseLogic, for ex: DefaultTraverseLogic.
/// The OnGraphVertexVisited() method is called for each vertex encountered during the propagation. It indicates by its return
/// value whether or not the vertex should be chosen as the best so far, and whether or not the query should continue.
/// It can also indicate that the path to the candidate vertex must be tested before a decision can be made; in this case
/// the query also calls the OnPathToGraphVertexComputed() method, providing the path to the candidate vertex and the path
/// to the best vertex found so far.
/// the propagation is limited in 2d to a circle centered in startPos and with m_propagationRadius as radius. (you can change it with the SetPropagationRadius method)

template<class VertexFinderLogic>
class BestGraphVertexPathFinderQuery : public BaseBestGraphVertexPathFinderQuery
{
public:

	// ---------------------------------- Public Member Functions ----------------------------------

	BestGraphVertexPathFinderQuery();
	virtual ~BestGraphVertexPathFinderQuery();

	// ---------------------------------- Functions to set up the query ----------------------------------

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);

	/// Sets an  instance of this class with minimum input requirements.
	/// Clears all the output, and let all other parameters unchanged. They
	/// may be changed by calling the corresponding write accessors.
	/// \param startPos    Sets the value of #m_startPos3f.
	void Initialize(const Vec3f& startPos);

	// ---------------------------------- write accessors for query inputs ----------------------------------

	/// The NavMesh triangle that corresponds to #m_startPos3f. If a valid NavTrianglePtr is specified, this value will be used.
	/// If no value is specified, it will be automatically computed during the query. 
	/// Call this method after Initialize(), during which it is cleared. 
	void SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr);

	/// Optional NavGraph edge you want the start position must be considered to lie on.
	/// If specified, we won't try to project the startPosition in the NavMesh and the path, if found, will start on this NavGraphEdge.
	/// Call this method after Initialize(), during which it is cleared. 
	void SetStartNavGraphEdgePtr(const NavGraphEdgePtr& startNavGraphEdgePtr, NavGraphEdgeDirection navGraphEdgePathfindMode);

	/// Optional NavGraph Vertex you want the start position must be considered to be on.
	/// If specified, we won't try to project the startPosition in the NavMesh and the path, if found, will start on this navGraphVertex.
	/// No that if you set a valid starting NavGraphVertexPtr, you will never been asked for a Path ending on this starting NavGraphVertexPtr.
	/// Call this method after Initialize(), during which it is cleared. 
	void SetStartNavGraphVertexPtr(const NavGraphVertexPtr& startNavGraphVertexPtr);

	// ---------------------------------- write accessors for query parameters ----------------------------------

	/// Write accessor for TraversalParams::m_fromOutsideNavMeshDistance.
	/// Call this method after BindToDatabase(), during which TraversalParams::m_fromOutsideNavMeshDistance is set to its default value.
	void SetFromOutsideNavMeshDistance(KyFloat32 fromOutsideNavMeshDistance);

	/// Write accessor for m_propagationRadius.
	/// Call this method after BindToDatabase(), during which m_propagationRadius is set to its default value.
	void SetPropagationRadius(KyFloat32 propagationRadius);

	/// Write accessor for TraversalParams::m_positionSpatializationRange.
	/// Call this method after BindToDatabase(), during which TraversalParams::m_positionSpatializationRange is set to its default value.
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);

	/// Write accessor for m_computeChannelMode.
	/// Call this method after BindToDatabase(), during which m_computeChannelMode is set to its default value.
	void SetComputeChannelMode(PathFinderQueryComputeChannelMode computeChannelMode);

	/// Write accessor for m_channelComputerConfig.
	/// Call this method after BindToDatabase(), during which m_channelComputerConfig is set to its default value.
	void SetChannelComputerConfig(const ChannelComputerConfig& channelComputerConfig);

	/// Write accessor for TraversalParams::m_numberOfVisitedNodePerFrame.
	/// Call this method after BindToDatabase(), during which TraversalParams::m_numberOfVisitedNodePerFrame is set to its default value.
	void SetNumberOfProcessedNodePerFrame(KyUInt32 numberOfProcessedNodePerFrame);

	// ---------------------------------- Query framework functions ----------------------------------

	/// Performs a single iteration of the PathFinder.
	/// \copydoc IQuery::Advance()
	virtual void Advance(WorkingMemory* workingMemory);

	virtual void ReleaseWorkingMemoryOnCancelDuringProcess(WorkingMemory* workingMemory = KY_NULL);

	// ---------------------------------- Accessors ----------------------------------
	BestGraphVertexPathFinderQueryResult   GetResult()          const;

	KyFloat32                GetFromOutsideNavMeshDistance()    const;
	KyFloat32                GetPropagationRadius()             const;
	const Vec3f&             GetStartPos()                      const;
	const NavTrianglePtr&    GetStartTrianglePtr()              const;
	const NavGraphEdgePtr&   GetStartNavGraphEdgePtr()          const;
	const NavGraphVertexPtr& GetStartNavGraphVertexPtr()        const;
	Path*                    GetPath()                          const;
	KyUInt32                 GetNumberOfProcessedNodePerFrame() const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

	void ComputePathRefinerConfigInternals();
	const PathRefinerConfig& GetPathRefinerConfig() const;


public: //Internal
	bool HasNavDataChanged();

protected:
	void SetFinish(WorkingMemory* workingMemory);
	KyResult ComputeStartGraphEdgeOrGraphVertexOrTriangle(PathFinderQueryUtils& queryUtils);
	KyResult ComputeStartTriangle(PathFinderQueryUtils& queryUtils);

	void SetResult(BestGraphVertexPathFinderQueryResult result);
	KyResult ComputeDestTriangle(PathFinderQueryUtils& queryUtils);
	KyResult InitDestinationNode();
	KyResult CheckNavDataChange(WorkingMemory* workingMemory);
	KyResult InitTraversalFromStartNode(PathFinderQueryUtils& queryUtils);
	KyResult InitializeContextAndStartNode(PathFinderQueryUtils& queryUtils);

protected:

	class TraversalCustomizer
	{
	public:
		typedef VertexFinderLogic TLogic;

		TraversalCustomizer() : m_traverseLogicUserData(KY_NULL) {}

		bool CanTraverse(const NavTriangleRawPtr& triangleRawPtr, KyFloat32* costMultiplier);
		bool CanTraverse(const NavGraphEdgeRawPtr& edgeRawPtr, KyFloat32* costMultiplier);

		bool CanEnterNavTag(const NavTag& exitNavTag, const NavTag& enterNavTag, const Vec3f& pos);
		KyFloat32 GetHeuristicFromNodePosition(const Vec3f& nodePosition);

//		KyFloat32 GetNavMeshEdgeCost(const Vec3f& start, const Vec3f& end, const NavTag& navTag);
		bool ShouldVisitNode(AStarTraversalContext* astarContext, AStarNodeIndex nodeIndex);
		bool IsNodeOutsidePropagationBounds(AStarTraversalContext* astarContext, AStarNodeIndex nodeIndex);
		bool ShouldOpenHalfEdgeNode(const NavHalfEdgeRawPtr& halfEdge, NavHalfEdgeRawPtr& pairHalfEdgeOfNewNode);
		bool ShouldOpenGraphVertexNode(const NavGraphVertexRawPtr& navGraphVertexRawPtr);
		bool ShouldOpenAbstractGraphNode(const AbstractGraphNodeRawPtr& abstractGraphNodeRawPtr);
		bool ShouldOpenNavTriangleUnderAbstractGraphNode(const AbstractGraphNodeRawPtr& abstractGraphNodeRawPtr);
		Vec3f ComputeNodePositionOnHalfEdge(AStarTraversalContext* aStarTraversalContext, const Vec3f& startPosOfEdge,
			const Vec3f& endPosOfEdge, AStarNodeIndex predecessorNodeIndex);
		KyResult OnNavTriangleExplored(AStarTraversalContext* aStarTraversalContext, const NavTriangleRawPtr& navTriangleRawPtr, AStarNodeIndex currentNodeIndex);
		KyResult OnNavGraphEdgeExplored(AStarTraversalContext* aStarTraversalContext, const NavGraphEdgeRawPtr& navGraphEdgeRawPtr, AStarNodeIndex currentNodeIndex);
		void* GetTraverseLogicUserData() const { return m_traverseLogicUserData; }

		// propagationBounds
		Vec3f m_startPos;
		KyFloat32 m_maxRadius;
		void* m_traverseLogicUserData;
	};

	AStarTraversal<TraversalCustomizer> m_traversal;
};

}

#include "gwnavruntime/queries/bestgraphvertexpathfinderquery.inl"

#endif //Navigation_BestGraphVertexPathFinderQuery_H

