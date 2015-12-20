/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_AstarQuery_H
#define Navigation_AstarQuery_H

#include "gwnavruntime/queries/utils/baseastarquery.h"
#include "gwnavruntime/queries/utils/astartraversal.h"
#include "gwnavruntime/queries/utils/astartraversalcontext.h"
#include "gwnavruntime/queries/raycangoquery.h"
#include "gwnavruntime/database/database.h"
#include "gwnavruntime/path/path.h"
#include "gwnavruntime/channel/channelarraycomputer.h"


namespace Kaim
{

/// Runs an A-star algorithm to find a Path through the navigation data (NavMesh &
/// NavGraph) from a starting point to a destination, refines the path found, and
/// fills an object of type Path.
///
/// The template argument must be a class that have at least these three static functions : \code
///     bool CanTraverseNavTriangle(void* traverseLogicUserData, const NavTag& navTag)
///     bool CanTraverseAndGetCostMultiplier(void* traverseLogicUserData, const Kaim::NavTag& navTag, const Kaim::Vec3f& start,
///         const Kaim::Vec3f& end, CostMultiplierCallContext callContext, KyFloat32& costMultiplier);
///     KyFloat32 GetHeuristicFromDistanceToDest(KyFloat32 distance3DToDestination) \endcode
/// The template argument must also have to define a type CanEnterNavTagMode with typedef as follow, depending whether or not you have one-way navTag transition :
///        \code typedef DoNotUseCanEnterNavTag CanEnterNavTagMode;  // this to indicate there is no One-Way navTag switch at all \endcode
///    or
///        \code typedef UseCanEnterNavTag CanEnterNavTagMode; // this to indicate there can be some One-Way navTag switched that need to be considered during pathFinding \endcode
/// If you are using UseCanEnterNavTag, you also have to implement this function :
///    \code  bool CanEnterNavTag(const NavTag& exitNavTag, const NavTag& enterNavTag, const Vec3f& pos) \endcode
/// This function must tell if, at position pos, we can go from exitNavTag to enterNavTag.
	/// Please refer to the SimpleTraverseLogic class documentation for details on these functions
///
/// Note that the template argument is stored in value within this class.
///
/// The propagation of the AStarQuery is bound to 2d oriented bounding box computed by inflating the segment going from startPos to destPos by a boxExtent
/// that can be modified by calling the SetPropagationBoxExtent method. Note that bounds are only in 2D.
///
/// \verbatim
///                      Propagation bounding box
///  ____________________________________________________________________________
/// |                                                                           ^
/// |                                                                           |
/// |<----------->                                                 <----------->| boxExtent
/// |  boxExtent                                                      boxExtent |
/// |             x===============================================x             x
/// |        startPos                                         destPos           |
/// |                                                                           | boxExtent
/// |                                                                           |
/// |___________________________________________________________________________v
/// \endverbatim


template<class TraverseLogic>
class AStarQuery : public BaseAStarQuery
{
public:

	// ---------------------------------- Public Member Functions ----------------------------------

	AStarQuery();
	virtual ~AStarQuery();

	// ---------------------------------- Functions to set up the query ----------------------------------

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);

	/// Sets an  instance of this class with minimum input requirements.
	/// Clears all the input and output, lets all configuration parameters unchanged. They
	/// may be changed by calling the corresponding write accessors.
	/// \param startPos    Sets the value of #m_startPos3f.
	/// \param destPos     Sets the value of #m_destPos3f. 
	void Initialize(const Vec3f& startPos, const Vec3f& destPos);

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
	/// Call this method after Initialize(), during which it is cleared. 
	void SetStartNavGraphVertexPtr(const NavGraphVertexPtr& startNavGraphVertexPtr);

	/// The NavMesh triangle that corresponds to #m_destPos3f. If a valid NavTrianglePtr is specified, this value will be used.
	/// If no value is specified, it will be automatically computed during the query.
	/// Call this method after Initialize(), during which it is cleared. 
	void SetDestTrianglePtr(const NavTrianglePtr& destTrianglePtr);

	/// Optional NavGraph edge you want the dest position must be considered to lie on.
	/// If specified, we won't try to project the dest position in the NavMesh and the path, if found, will end on this NavGraphEdge.
	/// Call this method after Initialize(), during which it is cleared. 
	void SetDestNavGraphEdgePtr(const NavGraphEdgePtr& startNavGraphEdgePtr, NavGraphEdgeDirection navGraphEdgePathfindMode);

	/// Optional NavGraph Vertex you want the dest position must be considered to be on.
	/// If specified, we won't try to project the  dest position in the NavMesh and the path, if found, will end on this navGraphVertex.
	/// Note that you pass the same NavGraphVertexPtr to SetStartNavGraphVertexPtr and SetDestNavGraphVertexPtr, no path will be calculated.
	/// Call this method after Initialize(), during which it is cleared. 
	void SetDestNavGraphVertexPtr(const NavGraphVertexPtr& startNavGraphVertexPtr);

	// ---------------------------------- write accessors for query parameters ----------------------------------

	/// Write accessor for m_propagationBoxExtent.
	/// Call this method after BindToDatabase(), during which m_propagationBoxExtent is set to its default value.
	void SetPropagationBoxExtent(KyFloat32 propagationBoxExtent);

	/// Write accessor for m_tryCanGoMode.
	/// Call this method after BindToDatabase(), during which m_tryCanGoMode is set to its default value.
	void SetTryCanGoMode(AStarQueryTryCanGoMode tryCanGoMode);

	/// Write accessor for TraversalParams::m_fromOutsideNavMeshDistance.
	/// Call this method after BindToDatabase(), during which TraversalParams::m_fromOutsideNavMeshDistance is set to its default value.
	void SetFromOutsideNavMeshDistance(KyFloat32 fromOutsideNavMeshDistance);
	
	/// Write accessor for TraversalParams::m_toOutsideNavMeshDistance.
	/// Call this method after BindToDatabase(), during which TraversalParams::m_toOutsideNavMeshDistance is set to its default value.
	void SetToOutsideNavMeshDistance(KyFloat32 toOutsideNavMeshDistance);

	/// Write accessor for TraversalParams::m_positionSpatializationRange.
	/// Call this method after BindToDatabase(), during which TraversalParams::m_positionSpatializationRange is set to its default value.
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);

	/// Write accessor for m_computeChannelMode.
	/// Call this method after BindToDatabase(), during which m_computeChannelMode is set to its default value.
	void SetComputeChannelMode(PathFinderQueryComputeChannelMode computeChannelMode);

	/// Write accessor for m_channelComputerConfig.
	/// Call this method after BindToDatabase(), during which m_channelComputerConfig is set to its default value.
	void SetChannelComputerConfig(const ChannelComputerConfig& channelComputerConfig);

	/// Write accessor for TraversalParams::m_abstractGraphTraversalMode.
	/// Call this method after BindToDatabase(), during which TraversalParams::m_abstractGraphTraversalMode is set to its default value.
	void SetAbstractGraphTraversalMode(PathFinderAbstractGraphTraversalMode abstractGraphTraversalMode);

	/// Write accessor for TraversalParams::m_numberOfVisitedNodePerFrame.
	/// Call this method after BindToDatabase(), during which TraversalParams::m_numberOfVisitedNodePerFrame is set to its default value.
	void SetNumberOfProcessedNodePerFrame(KyUInt32 numberOfProcessedNodePerFrame);

	// ---------------------------------- Query framework functions ----------------------------------

	/// Performs a single iteration of the PathFinder.
	/// \copydoc IQuery::Advance()
	virtual void Advance(WorkingMemory* workingMemory);

	virtual void ReleaseWorkingMemoryOnCancelDuringProcess(WorkingMemory* workingMemory = KY_NULL);

	// ---------------------------------- Accessors ----------------------------------
	AStarQueryResult           GetResult()                        const;
	AStarQueryTryCanGoMode     GetTryCanGoMode()                  const;
	KyFloat32                  GetFromOutsideNavMeshDistance()    const;
	KyFloat32                  GetToOutsideNavMeshDistance()      const;
	KyFloat32                  GetPropagationBoxExtent()          const;
	const Vec3f&               GetStartPos()                      const;
	const Vec3f&               GetDestPos()                       const;
	const NavTrianglePtr&      GetStartTrianglePtr()              const;
	const NavGraphEdgePtr&     GetStartNavGraphEdgePtr()          const;
	const NavGraphVertexPtr&   GetStartNavGraphVertexPtr()        const;
	const NavTrianglePtr&      GetDestTrianglePtr()               const;
	const NavGraphEdgePtr&     GetDestNavGraphEdgePtr()           const;
	const NavGraphVertexPtr&   GetDestNavGraphVertexPtr()         const;
	Path*                      GetPath()                          const;
	KyUInt32                   GetNumberOfProcessedNodePerFrame() const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

	void ComputePathRefinerConfigInternals();
	const PathRefinerConfig& GetPathRefinerConfig() const;

public: //Internal
	bool HasNavDataChanged();

protected:
	void SetFinish(WorkingMemory* workingMemory);
	KyResult ComputeStartGraphEdgeOrGraphVertexOrTriangle(PathFinderQueryUtils& queryUtils);
	KyResult ComputeStartTriangle(PathFinderQueryUtils& queryUtils);
	KyResult ComputeDestTriangleIfNeeded(PathFinderQueryUtils& queryUtils);
	KyResult InitializeContextAndStartNode(PathFinderQueryUtils& queryUtils);
	KyResult InitDestinationNode();
	KyResult InitDestinationNodeFromDestNavGraphEdge();
	KyResult InitDestinationNodeFromDestNavGraphVertex();
	KyResult InitDestinationNodeFromDestNavTriangle();
	KyResult InitTraversalFromStartNode(PathFinderQueryUtils& queryUtils);
	void InitPropagationBounds();

	KyResult CheckNavDataChangeFromAstarContext(WorkingMemory* workingMemory);
	KyResult CheckNavDataChange(WorkingMemory* workingMemory);
	bool HasExplorationReachedDestNode();

	bool ShouldReturnAfterTryingCanGo(WorkingMemory* workingMemory);

	KyUInt32 CountAbstractGraphNodes(WorkingMemory* workingMemory, AStarNodeIndex& concreteDestinationNodeIdx);
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

		OrientedBox2d m_propagationBox;
		AStarQuery<TraverseLogic>* m_astarQuery;
		NavTriangleRawPtr m_destTriangleRawPtr;
		NavGraphEdgeRawPtr m_destNavGraphEdgeRawPtr[2];
		void* m_traverseLogicUserData;
	};

	AStarTraversal<TraversalCustomizer> m_traversal;
};

}

#include "gwnavruntime/queries/astarquery.inl"

#endif //Navigation_AstarQuery_H

