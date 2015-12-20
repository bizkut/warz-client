/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BaseSpatializedPointCollectorInAABBQuery_H
#define Navigation_BaseSpatializedPointCollectorInAABBQuery_H

#include "gwnavruntime/queries/querydynamicoutput.h"

#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"
#include "gwnavruntime/navmesh/worldintegerpos.h"
#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/database/positionspatializationrange.h"


namespace Kaim
{
class Database;
class SpatializedPointCollectorContext;

/// Enumerates the possible results of a SpatializedPointCollectorInAABBQuery.
enum SpatializedPointCollectorInAABBQueryResult
{
	SPATIALIZEDPOINTCOLLECTOR_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates that the query has not yet been initialized. 
	SPATIALIZEDPOINTCOLLECTOR_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates that the query has not yet been launched. 

	SPATIALIZEDPOINTCOLLECTOR_DONE_START_OUTSIDE,                      ///< Indicates that a NavMesh triangle could not be found for the starting point. 
	SPATIALIZEDPOINTCOLLECTOR_DONE_LACK_OF_WORKING_MEMORY,             ///< Indicates that insufficient working memory caused the query to stop. 

	SPATIALIZEDPOINTCOLLECTOR_DONE                                     ///< Indicates that the query was completed successfully. 
};

/// Base class for SpatializedPointCollectorInAABBQuery.
class BaseSpatializedPointCollectorInAABBQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeSpatializedPointCollectorInAABB; }
	virtual QueryType GetType() const { return TypeSpatializedPointCollectorInAABB; }
	virtual void BuildQueryBlob(BaseBlobHandler* blobHandler);
	virtual void InitFromQueryBlob(World* world, void* blob);
	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return CreateStaticQueryBlobHandler(); }
	static Ptr<BaseBlobHandler> CreateStaticQueryBlobHandler();


	// ---------------------------------- Main API Functions ----------------------------------

	SpatializedPointCollectorInAABBQueryResult GetResult() const;

	const Vec3f&           GetStartPos()           const;
	const Box3f&           GetExtentBox()          const;
	const NavTrianglePtr&  GetStartTrianglePtr()   const;
	QueryDynamicOutput*    GetQueryDynamicOutput() const;
	const WorldIntegerPos& GetStartIntegerPos()    const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

	Box3f ComputeAABB() const; /// Computes the actual AABB used by this query to collect the SpatializedPoints.


protected: // internal
	BaseSpatializedPointCollectorInAABBQuery();
	virtual ~BaseSpatializedPointCollectorInAABBQuery() {}

	void BindToDatabase(Database* database);
	void Initialize(const Vec3f& startPos, const Box3f& extentBox);

	void SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr);
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);
	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);
	void SetResult(SpatializedPointCollectorInAABBQueryResult result);
	void SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput);

	KyResult PrepareWithInputCoordPos(WorkingMemory* workingMemory);

	KyResult OpenNode(SpatializedPointCollectorContext* traversalContext, const NavFloorRawPtr& navFloorRawPtr);
	KyResult OpenNode(SpatializedPointCollectorContext* traversalContext, const NavGraphEdgeRawPtr& navGraphEdgeRawPtr);

	bool ShouldOpenNavFloorNode(const NavFloorRawPtr& navFloorRawPtr);
	bool ShouldOpenNavGraphEdgeNode(const NavGraphEdgeRawPtr& navGraphEdgeRawPtr);

	// NavFloor -> Neighbor NavFloors
	KyResult TraverseNeighborNavFloors(SpatializedPointCollectorContext* traversalContext, const NavFloorRawPtr& navFloorRawPtr);

	// NavGraphEdge -> Neighbor NavFloor
	KyResult TraverseNeighborNavFloors(SpatializedPointCollectorContext* traversalContext, const NavGraphEdgeRawPtr& navGraphEdgeRawPtr);

	// NavFloor -> Neighbor navGraphEdges
	KyResult TraverseNeighborNavGraphEdges(SpatializedPointCollectorContext* traversalContext, const NavFloorRawPtr& navFloorRawPtr);

	// NavGraphEdge -> Neighbor navGraphEdges
	KyResult TraverseNeighborNavGraphEdges(SpatializedPointCollectorContext* traversalContext, const NavGraphEdgeRawPtr& navGraphEdgeRawPtr);

	KyResult ProcessNeighborNavFloor(SpatializedPointCollectorContext* traversalContext, const NavFloorRawPtr& navFloorRawPtr);
	KyResult ProcessNeighborNavGraphEdge(SpatializedPointCollectorContext* traversalContext, const NavGraphEdgeRawPtr& navGraphEdgeRawPtr);

	KyResult InitDataStructure(SpatializedPointCollectorContext* traversalContext, WorkingMemory* workingMemory);

protected:
	Box2LL m_aabb2d;

	Box3f m_extentBox; ///< The Axis Aligned Bounding Box within which to collect the spatialized elements. 

	WorldIntegerPos m_startIntegerPos; // For internal use. Do not modify. 

	Vec3f m_startPos3f; ///< The starting point of the traversal. 

	/// The NavMesh triangle that corresponds to #m_startPos3f. If you do not set this value, it will be computed automatically
	/// during the query. If you set this value to a valid NavTrianglePtr, the value you provide will be used instead. 
	NavTrianglePtr m_startTrianglePtr;

	PositionSpatializationRange m_positionSpatializationRange; ///< The altitude range around the position to use when determining #m_startTrianglePtr from the #m_startPos3f. 

	SpatializedPointCollectorInAABBQueryResult m_result; ///< Updated during processing to indicate the result of the query. 

	/// A ref-counted Ptr to a QueryDynamicOutput instance, used to store the collected SpatializedPoint.
	/// You can keep the Ptr for reuse and reassign it to another query by calling SetQueryDynamicOutput() after the BindToDatabase(), but in this case, be careful: the query
	/// may swap the underlying instance when processing if the provided buffer is to short. In this case, it is
	/// more sure to always update the Ptr when the query process is over.
	Ptr<QueryDynamicOutput> m_queryDynamicOutput;
};

}

#include "gwnavruntime/queries/utils/basespatializedpointcollectorinaabbquery.inl"

#endif // Navigation_BaseSpatializedPointCollectorInAABBQuery_H

