/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



#ifndef Navigation_BaseMultiDestinationPathFinderQueryResult_H
#define Navigation_BaseMultiDestinationPathFinderQueryResult_H

#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"
#include "gwnavruntime/querysystem/workingmemcontainers/navdatachangeindexingrid.h"
#include "gwnavruntime/queries/ipathfinderquery.h"
#include "gwnavruntime/queries/utils/pathfinderqueryutils.h"
#include "gwnavruntime/queries/utils/pathrefinercontext.h"
#include "gwnavruntime/queries/utils/pathclampercontext.h"
#include "gwnavruntime/queries/utils/traversalparams.h"
#include "gwnavruntime/queries/utils/pathrefinerconfig.h"
#include "gwnavruntime/queries/utils/cellfilter.h"
#include "gwnavruntime/base/types.h"


namespace Kaim
{

class Path;

/// Enumerates the possible results of an AStarQuery.
enum MultiDestinationPathFinderQueryResult
{
	MULTIDESTINATIONPATHFINDER_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	MULTIDESTINATIONPATHFINDER_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 
	
	// ---------------------  Done once for all destinations ----------------------------
	MULTIDESTINATIONPATHFINDER_PROCESSING_TRAVERSAL,                    ///< Indicates that the astar traversal algorithm has been launched, but has not yet completed. 
	MULTIDESTINATIONPATHFINDER_PROCESSING_TRAVERSAL_DONE,               ///< Indicates that the astar traversal algorithm is finished, and that the path found is ready to be refined.
	
	// ---------------------  Done for each destination ----------------------------
	MULTIDESTINATIONPATHFINDER_PROCESSING_CANGOQUERY,                   ///< Indicates that initial CanGoQuery reached a dest from start, and the path from this dest is being computed from the CanGoQuery
	MULTIDESTINATIONPATHFINDER_PROCESSING_ABSTRACT_PATH,                ///< Indicates that the astar traversal algorithm traversed AbstractGraph, and an Abstract path is being built for one destination
	MULTIDESTINATIONPATHFINDER_PROCESSING_REFINING_INIT,                ///< Indicates that the refining process is being initialized for one destination
	MULTIDESTINATIONPATHFINDER_PROCESSING_REFINING,                     ///< Indicates that the path found is being refined for one destination
	MULTIDESTINATIONPATHFINDER_PROCESSING_PATHCLAMPING_INIT,            ///< Indicates that the refiner is done, but the path has not yet been clamped to the navMesh
	MULTIDESTINATIONPATHFINDER_PROCESSING_PATHCLAMPING,                 ///< Indicates that the refined path of one destination is being clamped to navMesh according to the altitude tolerance.
	MULTIDESTINATIONPATHFINDER_PROCESSING_PATHBUILDING,                 ///< Indicates that the clamping algorithm has completed, but the path has not yet been built. 
	MULTIDESTINATIONPATHFINDER_PROCESSING_CHANNEL_INIT,                 ///< Indicates that the refiner is done, the channel is going to be initialized for one destination
	MULTIDESTINATIONPATHFINDER_PROCESSING_CHANNEL_COMPUTE,              ///< Indicates that the channel is initialized, the channel is being computed for one destination

	// ---------------------  Possible results at the end of query  ----------------------------
	MULTIDESTINATIONPATHFINDER_DONE_START_OUTSIDE,                      ///< Indicates that a NavMesh triangle could not be found for the starting point. 
	MULTIDESTINATIONPATHFINDER_DONE_START_NAVTAG_FORBIDDEN,             ///< Indicates that the NavTag at the starting point is forbidden. 
	MULTIDESTINATIONPATHFINDER_DONE_ENDS_ARE_UNREACHABLE,               ///< Indicates that all destination points are either outside or in forbidden navtag. 
	MULTIDESTINATIONPATHFINDER_DONE_DESTS_ARE_START_NO_PATH,            ///< Indicates that all destinations are equals to starting location, computing a Path would have lead to a degenerated Path with only 1 node
	MULTIDESTINATIONPATHFINDER_DONE_NO_PATH_FOUND,                      ///< Indicates that no path was found for any destinations.

	MULTIDESTINATIONPATHFINDER_DONE_NAVDATA_CHANGED,                    ///< Indicates that the query has stopped and must be relaunched because of a change in the NavData.
	MULTIDESTINATIONPATHFINDER_DONE_ERROR_LACK_OF_WORKING_MEMORY,       ///< Indicates that insufficient working memory caused the query to stop. 
	MULTIDESTINATIONPATHFINDER_DONE_COMPUTATION_ERROR,                  ///< Indicates that a computation error caused the query to stop.
	MULTIDESTINATIONPATHFINDER_DONE_CHANNELCOMPUTATION_ERROR,           ///< Indicates that a computation error caused the query to stop.
	MULTIDESTINATIONPATHFINDER_DONE_COMPUTATION_CANCELED,               ///< Indicates that the computation has been canceled.

	MULTIDESTINATIONPATHFINDER_DONE_SOME_PATHS_FOUND,                     ///< Indicates some or all destinations have a path

	// ---------------------  Used individually for m_resultPerDestination ----------------------------
	MULTIDESTINATIONPATHFINDER_TOPROCESS_FROM_CANGOQUERY,                ///< CAnGoQuery validated the reachability, Path will be built from the query
	MULTIDESTINATIONPATHFINDER_PROCESSING,                              ///< Indicates that the destination point is currently processed (from MULTIDESTINATIONPATHFINDER_PROCESSING_ABSTRACT_PATH to MULTIDESTINATIONPATHFINDER_PROCESSING_CHANNEL_COMPUTE).
	MULTIDESTINATIONPATHFINDER_DONE_END_OUTSIDE,                        ///< Indicates that the destination point is outside the NavMesh.
	MULTIDESTINATIONPATHFINDER_DONE_END_NAVTAG_FORBIDDEN,               ///< Indicates that the NavTag at the destination point is forbidden.
	MULTIDESTINATIONPATHFINDER_DONE_PATH_NOT_FOUND,                     ///< Indicates that a path could not be found between the start and destination point.	
	MULTIDESTINATIONPATHFINDER_DONE_DEST_IS_START_NO_PATH,              ///< Indicates that query starting location and ending location are equals, computing a Path would have lead to a degenerated Path with only 1 node
	MULTIDESTINATIONPATHFINDER_DONE_PATH_FOUND,                         ///< Indicates that a path has been found between the start and destination. 
};

enum MultiDestinationPathFinderTryCanGoMode
{
	MULTIDESTINATIONPATHFINDER_DO_NOT_TRY_RAYCANGO = 0,
	MULTIDESTINATIONPATHFINDER_DO_TRY_RAYCANGO     = 1
};

/// Usually only one of MultiDestinationPathFinderFinalOutput values is useful,
/// but for debug purpose, it is possible to pass several of them
enum MultiDestinationPathFinderFinalOutput
{
	MULTIDESTINATIONPATHFINDER_BUILD_LOWEST_COST_PATH_ONLY = 0, ///< Build the path to the destination with the lowest cost.
	MULTIDESTINATIONPATHFINDER_COMPUTE_RAW_COSTS = 1 << 0, ///< Fulfill m_rawCosts.
	MULTIDESTINATIONPATHFINDER_BUILD_RAW_PATHS   = 1 << 1, ///< For Debug Only, fulfill m_rawPaths, no refining, no clamping, no channel making paths unsuitable for PathFollowing, but it gives an idea of what m_rawCosts correspond to.
	MULTIDESTINATIONPATHFINDER_BUILD_PATHS       = 1 << 2, ///< Fulfill m_paths and/or m_abstractPaths. Paths are refined, clamped and channels are potentially computed, making the query slower.	
};


static const AStarNodeIndex AStarNodeIndex_FirstDestNode = 1;

class PathFinderPoint
{
public:
	PathFinderPoint() { Clear(); }

	void Clear()
	{
		m_pos3f.Set(KyFloat32MAXVAL, KyFloat32MAXVAL, KyFloat32MAXVAL);
		m_trianglePtr.Invalidate();
		m_navGraphEdgePtr[0].Invalidate();
		m_navGraphEdgePtr[1].Invalidate();
		m_navGraphVertexPtr.Invalidate();
		m_navGraphVertexPtr.Invalidate();
	}	

	void SetPosition(const Vec3f& pos3f);
	void SetTrianglePtr(const NavTrianglePtr& destTrianglePtr);
	void SetNavGraphEdgePtr(const NavGraphEdgePtr& startNavGraphEdgePtr, NavGraphEdgeDirection navGraphEdgePathfindMode);
	void SetNavGraphVertexPtr(const NavGraphVertexPtr& startNavGraphVertexPtr);

public:
	NavTrianglePtr m_trianglePtr;
	NavGraphEdgePtr m_navGraphEdgePtr[2];
	NavGraphVertexPtr m_navGraphVertexPtr;
	Vec3f m_pos3f;        ///< The destination point for the path request.
	Vec3f m_insidePos3f;  ///< if #m_pos3f is slightly outside and we hook on the NavMesh, it is the computed inside pos, else it is equal to #m_pos3f.
};

/// Base class for MultiDestinationPathFinderQuery.
class BaseMultiDestinationPathFinderQuery : public ITimeSlicedQuery
{
public:
	static  QueryType            GetStaticType() { return TypeMultiDestinationPathFinderQuery; }
	virtual QueryType            GetType() const { return TypeMultiDestinationPathFinderQuery; }
	virtual void                 BuildQueryBlob(BaseBlobHandler* blobHandler);
	virtual void                 InitFromQueryBlob(World* world, void* blob);
	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return CreateStaticQueryBlobHandler(); }
	static Ptr<BaseBlobHandler>  CreateStaticQueryBlobHandler();

	// ---------------------------------- Main API functions ----------------------------------

	MultiDestinationPathFinderQueryResult  GetResult()                                const;
	KyFloat32                              GetFromOutsideNavMeshDistance()            const;
	KyFloat32                              GetToOutsideNavMeshDistance()              const;

	Ptr<CellFilter>                        GetPropagationCellFilter()                 const;

	const Vec3f&                           GetStartPos()                              const;
	const NavTrianglePtr&                  GetStartTrianglePtr()                      const;
	const NavGraphEdgePtr&                 GetStartNavGraphEdgePtr()                  const;
	const NavGraphVertexPtr&               GetStartNavGraphVertexPtr()                const;

	KyUInt32                               GetDestCount()                             const;
	const Vec3f&                           GetDestPos(KyUInt32 destIdx)               const;
	const NavTrianglePtr&                  GetDestTrianglePtr(KyUInt32 destIdx)       const;
	const NavGraphEdgePtr&                 GetDestNavGraphEdgePtr(KyUInt32 destIdx)   const;
	const NavGraphVertexPtr&               GetDestNavGraphVertexPtr(KyUInt32 destIdx) const;
	MultiDestinationPathFinderQueryResult  GetDestResult(KyUInt32 destIdx)            const;

	MultiDestinationPathFinderTryCanGoMode GetTryCanGoMode()                          const;
	PathFinderQueryComputeChannelMode      GetComputeChannelMode()                    const;
	const ChannelComputerConfig&           GetChannelComputerConfig()                 const;
	KyUInt32                               GetNumberOfProcessedNodePerFrame()         const;
	Path*                                  GetPath(KyUInt32 destIdx)                  const;
	Path*                                  GetAbstractPath(KyUInt32 destIdx)          const;
	KyFloat32                              GetRawCost(KyUInt32 destIdx)               const;

	const PositionSpatializationRange&     GetPositionSpatializationRange() const;

	void ComputePathRefinerConfigInternals();
	const PathRefinerConfig& GetPathRefinerConfig() const;


	KyUInt32 GetNavigationProfileId() const;

	KyUInt32 GetFinalOutputMode() const; ///< ///< possible returned values are defined by MultiDestinationPathFinderFinalOutput enum
	
public:
	void BindToDatabase(Database* database);
	void Initialize(const PathFinderPoint& start);
	void AddDestination(const PathFinderPoint& additionalDest);

	void SetFinalOutputMode(KyUInt32 finalOutputMode); ///< cf MultiDestinationPathFinderFinalOutput enum
	void SetTryCanGoMode(MultiDestinationPathFinderTryCanGoMode tryCanGoMode);
	void SetComputeChannelMode(PathFinderQueryComputeChannelMode computeChannelMode);
	void SetChannelComputerConfig(const ChannelComputerConfig& channelComputerConfig);
	void SetPropagationFilter(Ptr<CellFilter> propagationFilter);
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);
	void SetFromOutsideNavMeshDistance(KyFloat32 fromOutsideNavMeshDistance);
	void SetToOutsideNavMeshDistance(KyFloat32 toOutsideNavMeshDistance);
	void SetNumberOfProcessedNodePerFrame(KyUInt32 numberOfProcessedNodePerFrame);
	void SetAbstractGraphTraversalMode(PathFinderAbstractGraphTraversalMode abstractGraphTraversalMode);
	void SetNavigationProfileId(KyUInt32 navigationProfileId);

protected :
	BaseMultiDestinationPathFinderQuery();
	virtual ~BaseMultiDestinationPathFinderQuery() {}
	void SetResult(MultiDestinationPathFinderQueryResult result);

	void InitCellFilterFromAbstractGraph();

public: // internal
	Path* GetRawPath(KyUInt32 destIdx) const; // For Debug only.

protected :
	KyUInt32 m_navigationProfileId;

	MultiDestinationPathFinderTryCanGoMode m_tryCanGoMode;
	PathFinderQueryComputeChannelMode m_computeChannelMode; ///< The mode toggling Channels computation.
	ChannelComputerConfig m_channelComputerConfig; ///< The parameter set specific to Channels computation.
	KyUInt32 m_finalOutputMode; ///< cf enum MultiDestinationPathFinderFinalOutput
	KyUInt32 m_dataBaseChangeIdx;     ///< Stores the revision of the Database when the Path has been computed.

	PathFinderPoint m_start;
	KyArray<PathFinderPoint> m_destinations;

	TraversalParameters m_traversalParams;
	PathRefinerConfig m_pathRefinerConfig;

	Ptr<CellFilter> m_propagationCellFilter;

	KyArray<Ptr<Path> > m_paths;
	KyArray<Ptr<Path> > m_abstractPaths;
	KyArray<Ptr<Path> > m_rawPaths; // For Debug only.
	KyArrayPOD<KyFloat32>  m_rawCosts;

	KyArray<MultiDestinationPathFinderQueryResult> m_resultPerDestinations;

	MultiDestinationPathFinderQueryResult m_result; ///< Updated during processing to indicate the result of the PathFinderQueryResult query.

protected:
	Ptr<Path> m_currentRawPath;
};


KY_INLINE void PathFinderPoint::SetPosition(const Vec3f& pos3f) { m_pos3f = pos3f; }

KY_INLINE void PathFinderPoint::SetTrianglePtr(const NavTrianglePtr& trianglePtr)
{
	m_trianglePtr = trianglePtr;
	KY_LOG_WARNING_IF(m_navGraphEdgePtr[0].IsValid(), ("You are setting a NavTrianglePtr whereas a NavGraphEdgePtr was set too."));
	KY_LOG_WARNING_IF(m_navGraphVertexPtr.IsValid(), ("You are setting a NavTrianglePtr whereas a NavGraphVertexPtr was set too."));
	m_navGraphEdgePtr[0].Invalidate();
	m_navGraphEdgePtr[1].Invalidate();
	m_navGraphVertexPtr.Invalidate();
}

KY_INLINE void PathFinderPoint::SetNavGraphEdgePtr(const NavGraphEdgePtr& navGraphEdgePtr, NavGraphEdgeDirection navGraphEdgePathfindMode)
{
	m_navGraphEdgePtr[0] = navGraphEdgePtr; 
	if(navGraphEdgePathfindMode == NAVGRAPHEDGE_BIDIRECTIONAL)
		m_navGraphEdgePtr[1] = navGraphEdgePtr.GetOppositeNavGraphEdgePtr();
	else
		m_navGraphEdgePtr[1].Invalidate();

	KY_LOG_WARNING_IF(m_trianglePtr.IsValid(), ("You are setting a NavGraphEdgePtr whereas a NavTrianglePtr was set too."));
	KY_LOG_WARNING_IF(m_navGraphVertexPtr.IsValid(), ("You are setting a NavGraphEdgePtr whereas a NavGraphVertexPtr was set too."));
	m_trianglePtr.Invalidate();
	m_navGraphVertexPtr.Invalidate();
}

KY_INLINE void PathFinderPoint::SetNavGraphVertexPtr(const NavGraphVertexPtr& navGraphVertexPtr)
{
	m_navGraphVertexPtr = navGraphVertexPtr; 
	KY_LOG_WARNING_IF(m_trianglePtr.IsValid(), ("You are setting a NavGraphVertexPtr whereas a NavTrianglePtr was set too."));
	KY_LOG_WARNING_IF(m_navGraphEdgePtr[0].IsValid(), ("You are setting a NavGraphVertexPtr whereas a NavGraphEdgePtr was set too."));
	m_trianglePtr.Invalidate();
	m_navGraphEdgePtr[0].Invalidate();
	m_navGraphEdgePtr[1].Invalidate();
}

KY_INLINE void BaseMultiDestinationPathFinderQuery::SetFinalOutputMode(KyUInt32 finalOutputMode)       { m_finalOutputMode = finalOutputMode; }
KY_INLINE void BaseMultiDestinationPathFinderQuery::SetTryCanGoMode(MultiDestinationPathFinderTryCanGoMode tryCanGoMode)           { m_tryCanGoMode          = tryCanGoMode;          }
KY_INLINE void BaseMultiDestinationPathFinderQuery::SetResult(MultiDestinationPathFinderQueryResult result)                        { m_result                = result;                }
KY_INLINE void BaseMultiDestinationPathFinderQuery::SetComputeChannelMode(PathFinderQueryComputeChannelMode computeChannelMode)    { m_computeChannelMode    = computeChannelMode;    }
KY_INLINE void BaseMultiDestinationPathFinderQuery::SetChannelComputerConfig(const ChannelComputerConfig& channelComputerConfig)   { m_channelComputerConfig = channelComputerConfig; }
KY_INLINE void BaseMultiDestinationPathFinderQuery::SetPropagationFilter(Ptr<CellFilter> propagationFilter)                     { m_propagationCellFilter  = propagationFilter;    }

KY_INLINE MultiDestinationPathFinderQueryResult BaseMultiDestinationPathFinderQuery::GetResult() const { return m_result; }

KY_INLINE KyUInt32 BaseMultiDestinationPathFinderQuery::GetFinalOutputMode() const  { return m_finalOutputMode; }

KY_INLINE const Vec3f&              BaseMultiDestinationPathFinderQuery::GetStartPos()                const { return m_start.m_pos3f;              }
KY_INLINE const NavTrianglePtr&     BaseMultiDestinationPathFinderQuery::GetStartTrianglePtr()        const { return m_start.m_trianglePtr;        }
KY_INLINE const NavGraphEdgePtr&    BaseMultiDestinationPathFinderQuery::GetStartNavGraphEdgePtr()    const { return m_start.m_navGraphEdgePtr[0]; }
KY_INLINE const NavGraphVertexPtr&  BaseMultiDestinationPathFinderQuery::GetStartNavGraphVertexPtr()  const { return m_start.m_navGraphVertexPtr;  }

KY_INLINE KyUInt32                  BaseMultiDestinationPathFinderQuery::GetDestCount()                               const { return m_destinations.GetCount();                    }
KY_INLINE const Vec3f&              BaseMultiDestinationPathFinderQuery::GetDestPos(KyUInt32 destIdx)                 const { return m_destinations[destIdx].m_pos3f;              }
KY_INLINE const NavTrianglePtr&     BaseMultiDestinationPathFinderQuery::GetDestTrianglePtr(KyUInt32 destIdx)         const { return m_destinations[destIdx].m_trianglePtr;        }
KY_INLINE const NavGraphEdgePtr&    BaseMultiDestinationPathFinderQuery::GetDestNavGraphEdgePtr(KyUInt32 destIdx)     const { return m_destinations[destIdx].m_navGraphEdgePtr[0]; }
KY_INLINE const NavGraphVertexPtr&  BaseMultiDestinationPathFinderQuery::GetDestNavGraphVertexPtr(KyUInt32 destIdx)   const { return m_destinations[destIdx].m_navGraphVertexPtr;  }
KY_INLINE MultiDestinationPathFinderQueryResult  BaseMultiDestinationPathFinderQuery::GetDestResult(KyUInt32 destIdx) const { return m_resultPerDestinations[destIdx];             }

KY_INLINE MultiDestinationPathFinderTryCanGoMode BaseMultiDestinationPathFinderQuery::GetTryCanGoMode()                  const { return m_tryCanGoMode;           }
KY_INLINE Path*                                  BaseMultiDestinationPathFinderQuery::GetPath(KyUInt32 destIdx)          const { return m_paths[destIdx];         }
KY_INLINE Path*                                  BaseMultiDestinationPathFinderQuery::GetAbstractPath(KyUInt32 destIdx)  const { return m_abstractPaths[destIdx]; }
KY_INLINE Path*                                  BaseMultiDestinationPathFinderQuery::GetRawPath(KyUInt32 destIdx)       const { return m_rawPaths[destIdx];      }
KY_INLINE KyFloat32                              BaseMultiDestinationPathFinderQuery::GetRawCost(KyUInt32 destIdx)       const { return m_rawCosts[destIdx];      }
KY_INLINE PathFinderQueryComputeChannelMode      BaseMultiDestinationPathFinderQuery::GetComputeChannelMode()            const { return m_computeChannelMode;     }
KY_INLINE const ChannelComputerConfig&           BaseMultiDestinationPathFinderQuery::GetChannelComputerConfig()         const { return m_channelComputerConfig;  }
KY_INLINE Ptr<CellFilter>                        BaseMultiDestinationPathFinderQuery::GetPropagationCellFilter()         const { return m_propagationCellFilter;  }


KY_INLINE KyFloat32 BaseMultiDestinationPathFinderQuery::GetFromOutsideNavMeshDistance()    const { return m_traversalParams.GetFromOutsideNavMeshDistance();    }
KY_INLINE KyFloat32 BaseMultiDestinationPathFinderQuery::GetToOutsideNavMeshDistance()      const { return m_traversalParams.GetToOutsideNavMeshDistance();      }
KY_INLINE KyUInt32  BaseMultiDestinationPathFinderQuery::GetNumberOfProcessedNodePerFrame() const { return m_traversalParams.GetNumberOfProcessedNodePerFrame(); }


KY_INLINE void BaseMultiDestinationPathFinderQuery::SetFromOutsideNavMeshDistance(KyFloat32 fromOutsideNavMeshDistance)
{
	m_traversalParams.SetFromOutsideNavMeshDistance(fromOutsideNavMeshDistance);
}
KY_INLINE void BaseMultiDestinationPathFinderQuery::SetToOutsideNavMeshDistance(KyFloat32 toOutsideNavMeshDistance)
{
	m_traversalParams.SetToOutsideNavMeshDistance(toOutsideNavMeshDistance);
}
KY_INLINE void BaseMultiDestinationPathFinderQuery::SetNumberOfProcessedNodePerFrame(KyUInt32 numberOfProcessedNodePerFrame)
{
	m_traversalParams.SetNumberOfProcessedNodePerFrame(numberOfProcessedNodePerFrame);
}
KY_INLINE void BaseMultiDestinationPathFinderQuery::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	m_traversalParams.SetPositionSpatializationRange(positionSpatializationRange);
}

KY_INLINE void BaseMultiDestinationPathFinderQuery::SetAbstractGraphTraversalMode(PathFinderAbstractGraphTraversalMode abstractGraphTraversalMode)
{
	m_traversalParams.SetAbstractGraphTraversalMode(abstractGraphTraversalMode);
}


KY_INLINE const PositionSpatializationRange& BaseMultiDestinationPathFinderQuery::GetPositionSpatializationRange() const { return m_traversalParams.GetPositionSpatializationRange(); }

KY_INLINE void BaseMultiDestinationPathFinderQuery::ComputePathRefinerConfigInternals()              { m_pathRefinerConfig.ComputeInternals();}
KY_INLINE const PathRefinerConfig& BaseMultiDestinationPathFinderQuery::GetPathRefinerConfig() const { return m_pathRefinerConfig;}


KY_INLINE void     BaseMultiDestinationPathFinderQuery::SetNavigationProfileId(KyUInt32 navigationProfileId) { m_navigationProfileId = navigationProfileId; }
KY_INLINE KyUInt32 BaseMultiDestinationPathFinderQuery::GetNavigationProfileId() const { return m_navigationProfileId; }

}

#endif //Navigation_BaseMultiDestinationPathFinderQueryResult_H

