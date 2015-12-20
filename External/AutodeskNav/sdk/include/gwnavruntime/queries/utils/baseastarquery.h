/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BaseAStarQuery_H
#define Navigation_BaseAStarQuery_H

#include "gwnavruntime/queries/ipathfinderquery.h"
#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"
#include "gwnavruntime/querysystem/workingmemcontainers/navdatachangeindexingrid.h"
#include "gwnavruntime/queries/utils/pathfinderqueryutils.h"
#include "gwnavruntime/queries/utils/pathrefinercontext.h"
#include "gwnavruntime/queries/utils/pathclampercontext.h"
#include "gwnavruntime/queries/utils/traversalparams.h"
#include "gwnavruntime/queries/utils/pathrefinerconfig.h"
#include "gwnavruntime/queries/utils/cellfilter.h"


namespace Kaim
{

class Path;

/// Enumerates the possible results of an AStarQuery.
enum AStarQueryResult
{
	ASTAR_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	ASTAR_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	ASTAR_PROCESSING_TRAVERSAL,                    ///< Indicates that the astar traversal algorithm has been launched, but has not yet completed. 
	ASTAR_PROCESSING_TRAVERSAL_DONE,               ///< Indicates that the astar traversal algorithm is finished, and that the path found is ready to be refined.
	ASTAR_PROCESSING_ABSTRACT_PATH,                ///< Indicates that the astar traversal algorithm traversed AbstractGraph, and the Abstract path is being built
	ASTAR_PROCESSING_REFINING_INIT,                ///< Indicates that the refining process is being initialized.
	ASTAR_PROCESSING_REFINING,                     ///< Indicates that the path found is being refined.
	ASTAR_PROCESSING_PATHCLAMPING_INIT,            ///< Indicates that the refiner is done, but the path has not yet been clamped to the navMesh. 
	ASTAR_PROCESSING_PATHCLAMPING,                 ///< Indicates that the refined path is being clamped to navMesh according to the altitude tolerance.
	ASTAR_PROCESSING_PATHBUILDING,                 ///< Indicates that the clamping algorithm has completed, but the path has not yet been built. 
	ASTAR_PROCESSING_CHANNEL_INIT,                 ///< Unused. Kept here for backward compatibility
	ASTAR_PROCESSING_CHANNEL_COMPUTE,              ///< Indicates that the channel is initialized, the channel is being computed

	ASTAR_DONE_START_OUTSIDE,                      ///< Indicates that a NavMesh triangle could not be found for the starting point. 
	ASTAR_DONE_START_NAVTAG_FORBIDDEN,             ///< Indicates that the NavTag at the starting point is forbidden. 
	ASTAR_DONE_END_OUTSIDE,                        ///< Indicates that the destination point of the query is outside the NavMesh. 
	ASTAR_DONE_END_NAVTAG_FORBIDDEN,               ///< Indicates that the NavTag at the destination point is forbidden. 
	ASTAR_DONE_PATH_NOT_FOUND,                     ///< Indicates that a path could not be found between the start and destination points.
	ASTAR_DONE_NAVDATA_CHANGED,                    ///< Indicates that the query has stopped and must be relaunched because of a change in the NavData.
	ASTAR_DONE_ERROR_LACK_OF_WORKING_MEMORY,       ///< Indicates that insufficient working memory caused the query to stop. 
	ASTAR_DONE_COMPUTATION_ERROR,                  ///< Indicates that a computation error caused the query to stop.
	ASTAR_DONE_CHANNELCOMPUTATION_ERROR,           ///< Indicates that a computation error caused the query to stop.
	ASTAR_DONE_COMPUTATION_CANCELED,               ///< Indicates that the computation has been canceled.

	ASTAR_DONE_DEST_IS_START_NO_PATH,              ///< Indicates that query starting location and ending location are equals. computing a Path would have lead to a degenerated Path with only 1 node
	ASTAR_DONE_PATH_FOUND,                         ///< Indicates that a path has been found between the start and destination. 
};

/// This enums tells if the AstarQuery should try to perform a RayCanGoQuery from start position to destination position before launching a traversal.
/// This can be used to speed up the Astar.
/// Not that if enabled and the RayCanGoQuery success, no traversal are performed at all, ignoring all the NavGraphs even if using one of them
/// would have resulted in a shorter Path.
/// The RayCanGoQuery is not tried if a NavGraphEdgePtr or NavGrapVertexRawPtr is passed as start/dest NavData location
/// By default it is disbaled (ASTAR_DO_NOT_TRY_RAYCANGO)
enum AStarQueryTryCanGoMode
{
	ASTAR_DO_NOT_TRY_RAYCANGO = 0,
	ASTAR_DO_TRY_RAYCANGO     = 1
};

/// Base class for AstarQuery.
class BaseAStarQuery : public IPathFinderQuery
{
public:
	static  QueryType            GetStaticType() { return TypeAStar; }
	virtual QueryType            GetType() const { return TypeAStar; }
	virtual void                 BuildQueryBlob(BaseBlobHandler* blobHandler);
	virtual void                 InitFromQueryBlob(World* world, void* blob);
	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return CreateStaticQueryBlobHandler(); }
	static Ptr<BaseBlobHandler>  CreateStaticQueryBlobHandler();

	// ---------------------------------- Main API functions ----------------------------------

	virtual void             GetPathFinderTextResult(String&)                         const;
	virtual void             DisplayPropagationBounds(ScopedDisplayList& displayList) const;
	virtual PathFinderResult GetPathFinderResult()                                    const;
	virtual const Vec3f*     GetDestination()                                         const;

	AStarQueryResult                     GetResult()                        const;
	KyFloat32                            GetFromOutsideNavMeshDistance()    const;
	KyFloat32                            GetToOutsideNavMeshDistance()      const;
	KyFloat32                            GetPropagationBoxExtent()          const;
	const Vec3f&                         GetStartPos()                      const;
	const Vec3f&                         GetDestPos()                       const;
	const NavTrianglePtr&                GetStartTrianglePtr()              const;
	const NavGraphEdgePtr&               GetStartNavGraphEdgePtr()          const;
	const NavGraphVertexPtr&             GetStartNavGraphVertexPtr()        const;
	const NavTrianglePtr&                GetDestTrianglePtr()               const;
	const NavGraphEdgePtr&               GetDestNavGraphEdgePtr()           const;
	const NavGraphVertexPtr&             GetDestNavGraphVertexPtr()         const;
	AStarQueryTryCanGoMode               GetTryCanGoMode()                  const;
	PathFinderQueryComputeChannelMode    GetComputeChannelMode()            const;
	const ChannelComputerConfig&         GetChannelComputerConfig()         const;
	Path*                                GetPath()                          const;
	KyUInt32                             GetNumberOfProcessedNodePerFrame() const;
	Path*                                GetAbstractPath()                  const;
	PathFinderAbstractGraphTraversalMode GetAbstractGraphTraversalMode()    const;

	const PositionSpatializationRange&   GetPositionSpatializationRange() const;

	void ComputePathRefinerConfigInternals();
	const PathRefinerConfig& GetPathRefinerConfig() const;


public: // Internal
	void BindToDatabase(Database* database);
	void Initialize(const Vec3f& startPos, const Vec3f& destPos);

	void SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr);
	void SetStartNavGraphEdgePtr(const NavGraphEdgePtr& startNavGraphEdgePtr, NavGraphEdgeDirection navGraphEdgePathfindMode);
	void SetStartNavGraphVertexPtr(const NavGraphVertexPtr& startNavGraphVertexPtr);

	void SetDestTrianglePtr(const NavTrianglePtr& destTrianglePtr);
	void SetDestNavGraphEdgePtr(const NavGraphEdgePtr& startNavGraphEdgePtr, NavGraphEdgeDirection navGraphEdgePathfindMode);
	void SetDestNavGraphVertexPtr(const NavGraphVertexPtr& startNavGraphVertexPtr);

	void SetTryCanGoMode(AStarQueryTryCanGoMode tryCanGoMode);
	void SetComputeChannelMode(PathFinderQueryComputeChannelMode computeChannelMode);
	void SetChannelComputerConfig(const ChannelComputerConfig& channelComputerConfig);
	void SetPropagationBoxExtent(KyFloat32 propagationBoxExtent);
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);
	void SetFromOutsideNavMeshDistance(KyFloat32 fromOutsideNavMeshDistance);
	void SetToOutsideNavMeshDistance(KyFloat32 toOutsideNavMeshDistance);
	void SetNumberOfProcessedNodePerFrame(KyUInt32 numberOfProcessedNodePerFrame);
	void SetAbstractGraphTraversalMode(PathFinderAbstractGraphTraversalMode abstractGraphTraversalMode);

protected :
	BaseAStarQuery();
	virtual ~BaseAStarQuery() {}
	void SetResult(AStarQueryResult result);
	
	void InitCellFilterFromAbstractGraph();

public :
	/// The NavMesh triangle that corresponds to #m_startPos3f. If a valid NavTrianglePtr is specified, this value will be used.
	/// If no value is specified, it will be automatically computed during the query.
	/// As an input AND output parameter, this member is cleared during the Setup of the Query and should therefore be specified
	// after the call to Initialize
	NavTrianglePtr m_startTrianglePtr;

	/// The NavMesh triangle that corresponds to #m_destPos3f. If a valid NavTrianglePtr is specified, this value will be used.
	/// If no value is specified, it will be automatically computed during the query.
	/// As an input AND output parameter, this member is cleared during the Setup of the Query and should therefore be specified
	// after the call to Initialize
	NavTrianglePtr m_destTrianglePtr;

	/// When this NavGraph edge is valid, that means that you want the path to start with that edge.

	NavGraphEdgePtr m_startNavGraphEdgePtr;
	NavGraphEdgePtr m_destNavGraphEdgePtr;

	/// Whether we should pathfind on that edge both ways or not.
	NavGraphEdgeDirection m_navGraphEdgePathfindMode;

	/// When this NavGraph vertex is valid, that means that you want the path to start at that vertice.
	NavGraphVertexPtr m_startNavGraphVertexPtr;
	NavGraphVertexPtr m_destNavGraphVertexPtr;

	Vec3f m_destPos3f;        ///< The destination point for the path request. 
	Vec3f m_startInsidePos3f; ///< if #m_startPos3f is slightly outside and we hook on the NavMesh, it is the computed inside pos, else it is equal to #m_startPos3f.
	Vec3f m_destInsidePos3f;  ///< if #m_destPos3f is slightly outside and we hook on the NavMesh, it is the computed inside pos, else it is equal to #m_destPos3f.

	AStarQueryTryCanGoMode m_tryCanGoMode;
	TraversalParameters m_traversalParams;
	PathRefinerConfig m_pathRefinerConfig;

	KyFloat32 m_propagationBoxExtent; ///< the propagation is limited to a 2d oriented bounding box computed by inflating the segment going from startPos to destPos by this value
	Ptr<CellFilter> m_propagationCellFilter;

	Ptr<Path> m_abstractPath;                 ///< The Abstract Path found by the query.

	AStarQueryResult m_result; ///< Updated during processing to indicate the result of the PathFinderQueryResult query.
};


KY_INLINE void BaseAStarQuery::SetResult(AStarQueryResult result)                            { m_result                = result;                }
KY_INLINE void BaseAStarQuery::SetTryCanGoMode(AStarQueryTryCanGoMode tryCanGoMode)          { m_tryCanGoMode          = tryCanGoMode;          }
KY_INLINE void BaseAStarQuery::SetComputeChannelMode(PathFinderQueryComputeChannelMode mode) { m_computeChannelMode    = mode;                  }
KY_INLINE void BaseAStarQuery::SetChannelComputerConfig(const ChannelComputerConfig& config) { m_channelComputerConfig = config;                }
KY_INLINE void BaseAStarQuery::SetPropagationBoxExtent(KyFloat32 propagationBoxExtent)       { m_propagationBoxExtent  = propagationBoxExtent;  }

KY_INLINE void BaseAStarQuery::SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr)
{
	m_startTrianglePtr = startTrianglePtr;
	KY_LOG_WARNING_IF(m_startNavGraphEdgePtr.IsValid(), ("You are setting a start NavTrianglePtr whereas a start NavGraphEdgePtr was set too."));
	KY_LOG_WARNING_IF(m_startNavGraphVertexPtr.IsValid(), ("You are setting a start NavTrianglePtr whereas a start NavGraphVertexPtr was set too."));
	m_startNavGraphEdgePtr.Invalidate();
	m_startNavGraphVertexPtr.Invalidate();
}

KY_INLINE void BaseAStarQuery::SetStartNavGraphEdgePtr(const NavGraphEdgePtr& startNavGraphEdgePtr, NavGraphEdgeDirection navGraphEdgePathfindMode)
{
	m_startNavGraphEdgePtr     = startNavGraphEdgePtr; 
	m_navGraphEdgePathfindMode = navGraphEdgePathfindMode;
	KY_LOG_WARNING_IF(m_startTrianglePtr.IsValid(), ("You are setting a start NavGraphEdgePtr whereas a start NavTrianglePtr was set too."));
	KY_LOG_WARNING_IF(m_startNavGraphVertexPtr.IsValid(), ("You are setting a start NavGraphEdgePtr whereas a start NavGraphVertexPtr was set too."));
	m_startTrianglePtr.Invalidate();
	m_startNavGraphVertexPtr.Invalidate();
}

KY_INLINE void BaseAStarQuery::SetStartNavGraphVertexPtr(const NavGraphVertexPtr& startNavGraphVertexPtr)
{
	m_startNavGraphVertexPtr = startNavGraphVertexPtr; 
	KY_LOG_WARNING_IF(m_startTrianglePtr.IsValid(), ("You are setting a start NavGraphVertexPtr whereas a start NavTrianglePtr was set too."));
	KY_LOG_WARNING_IF(m_startNavGraphEdgePtr.IsValid(), ("You are setting a start NavGraphVertexPtr whereas a start NavGraphEdgePtr was set too."));
	m_startTrianglePtr.Invalidate();
	m_startNavGraphEdgePtr.Invalidate();
}

KY_INLINE void BaseAStarQuery::SetDestTrianglePtr(const NavTrianglePtr& destTrianglePtr)
{ 
	m_destTrianglePtr  = destTrianglePtr;
	KY_LOG_WARNING_IF(m_destNavGraphEdgePtr.IsValid(), ("You are setting a dest NavTrianglePtr whereas a dest NavGraphEdgePtr was set too."));
	KY_LOG_WARNING_IF(m_destNavGraphVertexPtr.IsValid(), ("You are setting a dest NavTrianglePtr whereas a dest NavGraphVertexPtr was set too."));
	m_destNavGraphEdgePtr.Invalidate();
	m_destNavGraphVertexPtr.Invalidate();
}

KY_INLINE void BaseAStarQuery::SetDestNavGraphEdgePtr(const NavGraphEdgePtr& destNavGraphEdgePtr, NavGraphEdgeDirection navGraphEdgePathfindMode)     
{ 
	m_destNavGraphEdgePtr      = destNavGraphEdgePtr; 
	m_navGraphEdgePathfindMode = navGraphEdgePathfindMode;
	KY_LOG_WARNING_IF(m_destTrianglePtr.IsValid(), ("You are setting a dest NavGraphEdgePtr whereas a dest NavTrianglePtr was set too."));
	KY_LOG_WARNING_IF(m_destNavGraphVertexPtr.IsValid(), ("You are setting a dest NavGraphEdgePtr whereas a dest NavGraphVertexPtr was set too."));
	m_destTrianglePtr.Invalidate();
	m_destNavGraphVertexPtr.Invalidate();
}

KY_INLINE void BaseAStarQuery::SetDestNavGraphVertexPtr(const NavGraphVertexPtr& destNavGraphVertexPtr) 
{ 
	m_destNavGraphVertexPtr = destNavGraphVertexPtr; 
	KY_LOG_WARNING_IF(m_destTrianglePtr.IsValid(), ("You are setting a dest NavGraphVertexPtr whereas a dest NavTrianglePtr was set too."));
	KY_LOG_WARNING_IF(m_destNavGraphEdgePtr.IsValid(), ("You are setting a dest NavGraphVertexPtr whereas a dest NavGraphEdgePtr was set too."));
	m_destTrianglePtr.Invalidate();
	m_destNavGraphEdgePtr.Invalidate();
}

KY_INLINE AStarQueryResult                   BaseAStarQuery::GetResult()                        const { return m_result;                 }
KY_INLINE const Vec3f&                       BaseAStarQuery::GetDestPos()                       const { return m_destPos3f;              }
KY_INLINE const NavTrianglePtr&              BaseAStarQuery::GetStartTrianglePtr()              const { return m_startTrianglePtr;       }
KY_INLINE const NavGraphEdgePtr&             BaseAStarQuery::GetStartNavGraphEdgePtr()          const { return m_startNavGraphEdgePtr;   }
KY_INLINE const NavGraphVertexPtr&           BaseAStarQuery::GetStartNavGraphVertexPtr()        const { return m_startNavGraphVertexPtr; }
KY_INLINE const NavTrianglePtr&              BaseAStarQuery::GetDestTrianglePtr()               const { return m_destTrianglePtr;        }
KY_INLINE const NavGraphEdgePtr&             BaseAStarQuery::GetDestNavGraphEdgePtr()           const { return m_destNavGraphEdgePtr;    }
KY_INLINE const NavGraphVertexPtr&           BaseAStarQuery::GetDestNavGraphVertexPtr()         const { return m_destNavGraphVertexPtr;  }
KY_INLINE Path*                              BaseAStarQuery::GetPath()                          const { return m_path;                   }
KY_INLINE Path*                              BaseAStarQuery::GetAbstractPath()                  const { return m_abstractPath;           }
KY_INLINE AStarQueryTryCanGoMode             BaseAStarQuery::GetTryCanGoMode()                  const { return m_tryCanGoMode;           }
KY_INLINE PathFinderQueryComputeChannelMode  BaseAStarQuery::GetComputeChannelMode()            const { return m_computeChannelMode;     }
KY_INLINE const ChannelComputerConfig&       BaseAStarQuery::GetChannelComputerConfig()         const { return m_channelComputerConfig;  }
KY_INLINE const Vec3f*                       BaseAStarQuery::GetDestination()                   const { return &GetDestPos();            }
KY_INLINE KyFloat32                          BaseAStarQuery::GetPropagationBoxExtent()          const { return m_propagationBoxExtent;   }

KY_INLINE KyFloat32                          BaseAStarQuery::GetFromOutsideNavMeshDistance()    const { return m_traversalParams.GetFromOutsideNavMeshDistance();    }
KY_INLINE KyFloat32                          BaseAStarQuery::GetToOutsideNavMeshDistance()      const { return m_traversalParams.GetToOutsideNavMeshDistance();      }
KY_INLINE KyUInt32                           BaseAStarQuery::GetNumberOfProcessedNodePerFrame() const { return m_traversalParams.GetNumberOfProcessedNodePerFrame(); }
KY_INLINE const Vec3f&                       BaseAStarQuery::GetStartPos()                      const { return IPathFinderQuery::GetStartPos();                      }

KY_INLINE void BaseAStarQuery::SetFromOutsideNavMeshDistance(KyFloat32 fromOutsideNavMeshDistance)
{
	m_traversalParams.SetFromOutsideNavMeshDistance(fromOutsideNavMeshDistance);
}
KY_INLINE void BaseAStarQuery::SetToOutsideNavMeshDistance(KyFloat32 toOutsideNavMeshDistance)
{
	m_traversalParams.SetToOutsideNavMeshDistance(toOutsideNavMeshDistance);
}
KY_INLINE void BaseAStarQuery::SetNumberOfProcessedNodePerFrame(KyUInt32 numberOfProcessedNodePerFrame)
{
	m_traversalParams.SetNumberOfProcessedNodePerFrame(numberOfProcessedNodePerFrame);
}
KY_INLINE void BaseAStarQuery::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	m_traversalParams.SetPositionSpatializationRange(positionSpatializationRange);
}

KY_INLINE void BaseAStarQuery::SetAbstractGraphTraversalMode(PathFinderAbstractGraphTraversalMode abstractGraphTraversalMode)
{
	m_traversalParams.SetAbstractGraphTraversalMode(abstractGraphTraversalMode);
}

KY_INLINE PathFinderAbstractGraphTraversalMode BaseAStarQuery::GetAbstractGraphTraversalMode() const
{
	return m_traversalParams.GetAbstractGraphTraversalMode();
}

KY_INLINE const PositionSpatializationRange& BaseAStarQuery::GetPositionSpatializationRange() const { return m_traversalParams.GetPositionSpatializationRange(); }

KY_INLINE void BaseAStarQuery::ComputePathRefinerConfigInternals()              { m_pathRefinerConfig.ComputeInternals();}
KY_INLINE const PathRefinerConfig& BaseAStarQuery::GetPathRefinerConfig() const { return m_pathRefinerConfig;}

}

#endif //Navigation_BaseAstarQuery_H

