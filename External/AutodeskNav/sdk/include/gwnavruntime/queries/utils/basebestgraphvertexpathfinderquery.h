/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BaseBestGraphVertexPathFinderQuery_H
#define Navigation_BaseBestGraphVertexPathFinderQuery_H

#include "gwnavruntime/queries/utils/traversalparams.h"
#include "gwnavruntime/queries/utils/pathrefinerconfig.h"
#include "gwnavruntime/queries/utils/pathfinderqueryutils.h"
#include "gwnavruntime/queries/ipathfinderquery.h"


namespace Kaim
{

/// Enumerates the possible results of an BestGraphVertexPathFinderQuery.
enum BestGraphVertexPathFinderQueryResult
{
	BESTGRAPHVERTEX_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	BESTGRAPHVERTEX_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	BESTGRAPHVERTEX_PROCESSING_TRAVERSAL,                    ///< Indicates that the astar traversal algorithm has been launched, but has not yet completed. 
	BESTGRAPHVERTEX_PROCESSING_TRAVERSAL_DONE,               ///< Indicates that the astar traversal algorithm is finished, and that the path found is ready to be refined.
	BESTGRAPHVERTEX_PROCESSING_REFINING_INIT,                ///< Indicates that the refining process is being initialized.
	BESTGRAPHVERTEX_PROCESSING_REFINING,                     ///< Indicates that the path found is being refined.
	BESTGRAPHVERTEX_PROCESSING_PATHCLAMPING_INIT,            ///< Indicates that the refiner is done, but the path has not yet been clamped to the navMesh. 
	BESTGRAPHVERTEX_PROCESSING_PATHCLAMPING,                 ///< Indicates that the refined path is being clamped to navMesh according to the altitude tolerance.
	BESTGRAPHVERTEX_PROCESSING_PATHBUILDING,                 ///< Indicates that the clamping algorithm has completed, but the path has not yet been built. 
	BESTGRAPHVERTEX_PROCESSING_CHANNEL_INIT,                 ///< Indicates that the refiner is done, the channel is going to be initialized
	BESTGRAPHVERTEX_PROCESSING_CHANNEL_COMPUTE,              ///< Indicates that the channel is initialized, the channel is being computed

	BESTGRAPHVERTEX_DONE_START_OUTSIDE,                      ///< Indicates that a NavMesh triangle could not be found for the starting point. 
	BESTGRAPHVERTEX_DONE_START_NAVTAG_FORBIDDEN,             ///< Indicates that the NavTag at the starting point is forbidden. 
	BESTGRAPHVERTEX_DONE_PATH_NOT_FOUND,                     ///< Indicates that a path could not be found between the start and destination points.
	BESTGRAPHVERTEX_DONE_NAVDATA_CHANGED,                    ///< Indicates that the query has stopped and must be relaunched because of a change in the NavData.
	BESTGRAPHVERTEX_DONE_ERROR_LACK_OF_WORKING_MEMORY,       ///< Indicates that insufficient working memory caused the query to stop. 
	BESTGRAPHVERTEX_DONE_COMPUTATION_ERROR,                  ///< Indicates that a computation error caused the query to stop.
	BESTGRAPHVERTEX_DONE_CHANNELCOMPUTATION_ERROR,           ///< Indicates that a computation error caused the query to stop.
	BESTGRAPHVERTEX_DONE_COMPUTATION_CANCELED,               ///< Indicates that the computation has been canceled.

	BESTGRAPHVERTEX_DONE_PATH_FOUND,                         ///< Indicates that a path has been found between the start and destination. 
};

/// Base class for BestGraphVertexPathFinderQuery.
class BaseBestGraphVertexPathFinderQuery : public IPathFinderQuery
{
public:
	static  QueryType        GetStaticType() { return TypeBestGraphVertexPathFinderQuery; }
	virtual QueryType        GetType() const { return TypeBestGraphVertexPathFinderQuery; }
	virtual void             BuildQueryBlob(BaseBlobHandler* blobHandler);
	virtual void             InitFromQueryBlob(World* world, void* blob);
	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return CreateStaticQueryBlobHandler(); }
	static Ptr<BaseBlobHandler>  CreateStaticQueryBlobHandler();

	// ---------------------------------- Main API functions ----------------------------------

	virtual void             GetPathFinderTextResult(String&)        const;
	virtual void             DisplayPropagationBounds(ScopedDisplayList& displayList) const;
	virtual PathFinderResult GetPathFinderResult()                   const;
	virtual const Vec3f*     GetDestination()                        const;

	BestGraphVertexPathFinderQueryResult GetResult() const;
	KyFloat32                  GetPropagationRadius()             const;
	KyFloat32                  GetFromOutsideNavMeshDistance()    const;
	KyFloat32                  GetToOutsideNavMeshDistance()      const;
	const Vec3f&               GetStartPos()                      const;
	const NavTrianglePtr&      GetStartTrianglePtr()              const;
	const NavGraphEdgePtr&     GetStartNavGraphEdgePtr()          const;
	const NavGraphVertexPtr&   GetStartNavGraphVertexPtr()        const;
	const NavTrianglePtr&      GetDestTrianglePtr()               const;
	Path*                      GetPath()                          const;
	ChannelArray*              GetChannelArray()                  const;
	KyUInt32                   GetNumberOfProcessedNodePerFrame() const;

	const PositionSpatializationRange&   GetPositionSpatializationRange() const;

	void ComputePathRefinerConfigInternals();
	const PathRefinerConfig& GetPathRefinerConfig() const;

public: // Internal
	// public because can be used by the Bot basic path finding helper functions
	void BindToDatabase(Database* database);
	void Initialize(const Vec3f& startPos);
	void SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr);
	void SetStartNavGraphEdgePtr(const NavGraphEdgePtr& startNavGraphEdgePtr, NavGraphEdgeDirection navGraphEdgePathfindMode);
	void SetStartNavGraphVertexPtr(const NavGraphVertexPtr& startNavGraphVertexPtr);

protected :
	BaseBestGraphVertexPathFinderQuery();
	virtual ~BaseBestGraphVertexPathFinderQuery() {}

	void SetComputeChannelMode(PathFinderQueryComputeChannelMode computeChannelMode);
	void SetChannelComputerConfig(const ChannelComputerConfig& channelComputerConfig);
	void SetPropagationRadius(KyFloat32 propagationRadius);
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);
	void SetFromOutsideNavMeshDistance(KyFloat32 fromOutsideNavMeshDistance);
	void SetToOutsideNavMeshDistance(KyFloat32 toOutsideNavMeshDistance);
	void SetNumberOfProcessedNodePerFrame(KyUInt32 numberOfProcessedNodePerFrame);
	void SetResult(BestGraphVertexPathFinderQueryResult result);

protected :
	/// The NavMesh triangle that corresponds to #m_startPos3f. If a valid NavTrianglePtr is specified, this value will be used.
	/// If no value is specified, it will be automatically computed during the query.
	NavTrianglePtr m_startTrianglePtr;

	/// When this NavGraph edge is valid, that means that you want the path to start with that edge.
	NavGraphEdgePtr m_startNavGraphEdgePtr;

	/// Whether we should pathfind on that edge both ways or not.
	NavGraphEdgeDirection m_navGraphEdgePathfindMode;

	/// When this NavGraph vertex is valid, that means that you want the path to start at that vertice.
	NavGraphVertexPtr m_startNavGraphVertexPtr;

	Vec3f m_startInsidePos3f; ///< if #m_startPos3f is slightly outside and we hook on the NavMesh, it is the computed inside pos, else it is equal to #m_startPos3f.

	TraversalParameters m_traversalParams;
	PathRefinerConfig m_pathRefinerConfig;

	NavGraphVertexRawPtr m_currentVertex;
	AStarNodeIndex m_currentVertexNodeIdx;

	NavGraphVertexRawPtr m_bestVertex; // the best Vertex found so far.
	AStarNodeIndex m_bestVertexNodeIdx;
	Ptr<Path> m_rememberBestPathSoFar;

	KyFloat32 m_propagationRadius;  // the propagation is limited to a 2d circle centered in startPos and with m_propagationRadius as radius

	BestGraphVertexPathFinderQueryResult m_result; ///< Updated during processing to indicate the result of the PathFinderQueryResult query.
};

KY_INLINE void BaseBestGraphVertexPathFinderQuery::SetResult(BestGraphVertexPathFinderQueryResult result)        { m_result = result;                       }
KY_INLINE void BaseBestGraphVertexPathFinderQuery::SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr)   { m_startTrianglePtr = startTrianglePtr;   }
KY_INLINE void BaseBestGraphVertexPathFinderQuery::SetPropagationRadius(KyFloat32 propagationRadius)             { m_propagationRadius = propagationRadius; }
KY_INLINE void BaseBestGraphVertexPathFinderQuery::SetComputeChannelMode(PathFinderQueryComputeChannelMode mode) { m_computeChannelMode = mode;             }
KY_INLINE void BaseBestGraphVertexPathFinderQuery::SetChannelComputerConfig(const ChannelComputerConfig& config) { m_channelComputerConfig = config;        }

KY_INLINE BestGraphVertexPathFinderQueryResult BaseBestGraphVertexPathFinderQuery::GetResult()            const { return m_result;            }
KY_INLINE const NavTrianglePtr&                BaseBestGraphVertexPathFinderQuery::GetStartTrianglePtr()  const { return m_startTrianglePtr;  }
KY_INLINE Path*                                BaseBestGraphVertexPathFinderQuery::GetPath()              const { return m_path;              }
KY_INLINE const Vec3f*                         BaseBestGraphVertexPathFinderQuery::GetDestination()       const { return KY_NULL;             }
KY_INLINE KyFloat32                            BaseBestGraphVertexPathFinderQuery::GetPropagationRadius() const { return m_propagationRadius; }

KY_INLINE KyFloat32    BaseBestGraphVertexPathFinderQuery::GetFromOutsideNavMeshDistance()    const { return m_traversalParams.GetFromOutsideNavMeshDistance();    }
KY_INLINE KyFloat32    BaseBestGraphVertexPathFinderQuery::GetToOutsideNavMeshDistance()      const { return m_traversalParams.GetToOutsideNavMeshDistance();      }
KY_INLINE KyUInt32     BaseBestGraphVertexPathFinderQuery::GetNumberOfProcessedNodePerFrame() const { return m_traversalParams.GetNumberOfProcessedNodePerFrame(); }
KY_INLINE const Vec3f& BaseBestGraphVertexPathFinderQuery::GetStartPos()                      const { return IPathFinderQuery::GetStartPos();                      }

KY_INLINE void BaseBestGraphVertexPathFinderQuery::SetFromOutsideNavMeshDistance(KyFloat32 fromOutsideNavMeshDistance)
{
	m_traversalParams.SetFromOutsideNavMeshDistance(fromOutsideNavMeshDistance);
}
KY_INLINE void BaseBestGraphVertexPathFinderQuery::SetToOutsideNavMeshDistance(KyFloat32 toOutsideNavMeshDistance)
{
	m_traversalParams.SetToOutsideNavMeshDistance(toOutsideNavMeshDistance);
}
KY_INLINE void BaseBestGraphVertexPathFinderQuery::SetNumberOfProcessedNodePerFrame(KyUInt32 numberOfProcessedNodePerFrame)
{
	m_traversalParams.SetNumberOfProcessedNodePerFrame(numberOfProcessedNodePerFrame);
}
KY_INLINE void BaseBestGraphVertexPathFinderQuery::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	m_traversalParams.SetPositionSpatializationRange(positionSpatializationRange);
}
KY_INLINE const PositionSpatializationRange& BaseBestGraphVertexPathFinderQuery::GetPositionSpatializationRange() const { return m_traversalParams.GetPositionSpatializationRange(); }

KY_INLINE void BaseBestGraphVertexPathFinderQuery::ComputePathRefinerConfigInternals()              { m_pathRefinerConfig.ComputeInternals();}
KY_INLINE const PathRefinerConfig& BaseBestGraphVertexPathFinderQuery::GetPathRefinerConfig() const { return m_pathRefinerConfig;}

KY_INLINE void BaseBestGraphVertexPathFinderQuery::SetStartNavGraphEdgePtr(const NavGraphEdgePtr& startNavGraphEdgePtr, NavGraphEdgeDirection navGraphEdgePathfindMode)
{ 
	m_startNavGraphEdgePtr     = startNavGraphEdgePtr; 
	m_navGraphEdgePathfindMode = navGraphEdgePathfindMode;
	KY_LOG_WARNING_IF(m_startTrianglePtr.IsValid(), ("You are setting a start NavGraphEdgePtr whereas a start NavTrianglePtr was set too."));
	KY_LOG_WARNING_IF(m_startNavGraphVertexPtr.IsValid(), ("You are setting a start NavGraphEdgePtr whereas a start NavGraphVertexPtr was set too."));
	m_startTrianglePtr.Invalidate();
	m_startNavGraphVertexPtr.Invalidate();
}

KY_INLINE void BaseBestGraphVertexPathFinderQuery::SetStartNavGraphVertexPtr(const NavGraphVertexPtr& startNavGraphVertexPtr)
{
	m_startNavGraphVertexPtr = startNavGraphVertexPtr; 
	KY_LOG_WARNING_IF(m_startTrianglePtr.IsValid(), ("You are setting a start NavGraphVertexPtr whereas a start NavTrianglePtr was set too."));
	KY_LOG_WARNING_IF(m_startNavGraphEdgePtr.IsValid(), ("You are setting a start NavGraphVertexPtr whereas a start NavGraphEdgePtr was set too."));
	m_startTrianglePtr.Invalidate();
	m_startNavGraphEdgePtr.Invalidate();
}
}

#endif // Navigation_BaseBestGraphVertexPathFinderQuery_H
