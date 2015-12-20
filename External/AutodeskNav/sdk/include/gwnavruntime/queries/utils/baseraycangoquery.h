/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BaseRayCanGoQuery_H
#define Navigation_BaseRayCanGoQuery_H

#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/queries/utils/rayqueryutils.h"
#include "gwnavruntime/queries/utils/queryutils.h"
#include "gwnavruntime/navmesh/worldintegerpos.h"
#include "gwnavruntime/database/positionspatializationrange.h"


namespace Kaim
{

class NavHalfEdgeRawPtr;
class RawNavTagSubSegment;
template <class T> class WorkingMemArray;

/// Enumerates the possible results of a RayCanGoQuery.
enum RayCanGoQueryResult
{
	RAYCANGO_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates that the query has not yet been initialized. 
	RAYCANGO_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates that the query has not yet been launched. 

	RAYCANGO_DONE_START_OUTSIDE,                      ///< Indicates that a NavMesh triangle could not be found for the starting point. 
	RAYCANGO_DONE_START_NAVTAG_FORBIDDEN,             ///< Indicates that a NavMesh triangle has been found for the starting point, but its NavTag is not considered navigable. 
	RAYCANGO_DONE_COLLISION_DETECTED,                 ///< Indicates that a collision was detected along the straight line between the starting and ending points (either a border of the navmesh has been hit, or a call to CanTraverseNavTriangle returned false.
	RAYCANGO_DONE_BORDER_DETECTED_WITHIN_MARGIN,      ///< Indicates that a border was detected close (less than margin) to the straight line between the starting and ending points. 
	RAYCANGO_DONE_ARRIVAL_IN_WRONG_FLOOR,             ///< Indicates that none of the above results occurred, which may indicate that the start and end points lie on different floors. 
	RAYCANGO_DONE_LACK_OF_WORKING_MEMORY,             ///< Indicates that insufficient memory was available to store crossed triangles or sub-sections, and the query failed as a result. 
	RAYCANGO_DONE_UNKNOWN_ERROR,                      ///< Indicates that an unknown error occurred during the query processing. 

	RAYCANGO_DONE_SUCCESS                             ///< Indicates that the ray can pass in a straight line from the starting point to the ending point without a collision. 
};

/// Base class for RayCanGoQuery.
class BaseRayCanGoQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeRayCanGo; }
	virtual QueryType GetType() const { return TypeRayCanGo; }
	virtual void BuildQueryBlob(BaseBlobHandler* blobHandler);
	virtual void InitFromQueryBlob(World* world, void* blob);
	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return CreateStaticQueryBlobHandler(); }
	static Ptr<BaseBlobHandler> CreateStaticQueryBlobHandler();


	// ---------------------------------- Main API Functions ----------------------------------

	RayCanGoQueryResult    GetResult()                 const;
	const Vec3f&           GetStartPos()               const;
	const Vec3f&           GetDestPos()                const;
	const NavTrianglePtr&  GetStartTrianglePtr()       const;
	const NavTrianglePtr&  GetDestTrianglePtr()        const;
	ComputeCostMode        GetComputeCostMode()        const;
	KyFloat32              GetComputedCost()           const;
	DynamicOutputMode      GetDynamicOutputMode()      const;
	QueryDynamicOutput*    GetQueryDynamicOutput()     const;
	const WorldIntegerPos& GetStartIntegerPos()        const;
	const WorldIntegerPos& GetDestIntegerPos()         const;
	RayCanGoMarginMode     GetMarginMode()             const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;


public : // internal
	void BindToDatabase(Database* database);
	void Initialize(const Vec3f& startPos, const Vec3f& destPos);

	void SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr);
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);
	void SetMarginMode(RayCanGoMarginMode marginMode);
	void SetDynamicOutputMode(DynamicOutputMode dynamicOutputMode);
	void SetComputeCostMode(ComputeCostMode computeCostMode);
	void SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput);
	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);
	void SetDestIntegerPos(const WorldIntegerPos& destIntegerPos);

protected:
	BaseRayCanGoQuery();
	virtual ~BaseRayCanGoQuery() {}

	void SetResult(RayCanGoQueryResult result);
	void RayCanGo_NoHit(const NavHalfEdgeRawPtr& lastHalfEdgeRawPtr, RawNavTagSubSegment& navTagSubSegment, WorkingMemArray<RawNavTagSubSegment>& navTagSubSegments);

	KyResult InitDynamicOutput(WorkingMemory* workingMemory, WorkingMemArray<NavTriangleRawPtr>& crossedTriangles,
		WorkingMemArray<RawNavTagSubSegment>& navTagSubSegments, WorkingMemArray<RawHalfEdgeIntersection>& halfEdgeIntersections,
		ScopeAutoSaveDynamicOutput &scopeAutoSaveDynOutput);
	KyResult PushTriangleInDynamicOutput(WorkingMemArray<NavTriangleRawPtr> &crossedTriangles, const NavTriangleRawPtr& startTriangleRawPtr);
protected:
	WorldIntegerPos m_startIntegerPos;
	WorldIntegerPos m_destIntegerPos;

	Vec3f m_startPos3f; ///< The starting point of the line segment to be tested. 
	Vec3f m_destPos3f;  ///< The ending point of the line segment to be tested. 

	NavTrianglePtr m_startTrianglePtr; ///< The NavMesh triangle that corresponds to #m_startPos3f. You can set it as an input or retrieve it as an output of the query.
	NavTrianglePtr m_destTrianglePtr; ///< The NavMesh triangle that corresponds to #m_destPos3f. It will be computed automatically during the query. 

	/// The altitude range around the position to use when determining #m_startTrianglePtr and #m_destTrianglePtr from the #m_startPos3f and #m_destPos3f. 
	PositionSpatializationRange m_positionSpatializationRange;

	RayCanGoQueryResult m_result; ///< Updated during processing to indicate the result of the query. 

	ComputeCostMode m_computeCostMode; ///< Determines whether or not the query computes the approximate cost of following the line segment. If set to #QUERY_COMPUTE_COST_ALONG_3DAXIS, the cost is calculated and stored in #m_cost.
	KyFloat32 m_cost; ///< Updated during processing to store the cost of the line segment, if enabled by #m_computeCostMode.

	RayCanGoMarginMode m_marginMode; ///< The margin mode to be used in this query to eventually ensure the ray is not too close to any NavMesh border.

	DynamicOutputMode m_dynamicOutputMode; ///< Specifies the type of data stored in #m_queryDynamicOutput.

	/// A ref-counted Ptr to a QueryDynamicOutput instance, used to store the data encountered during the query process.
	/// You have to first call SetDynamicOutputMode(), after BindToDatabase() that set it to #QUERY_SAVE_NOTHING by default
	/// and before processing the query, to define the data type you want to gather.
	/// You can keep the Ptr for reuse and reassign it to another query by calling SetQueryDynamicOutput() after the BindToDatabase(), but in this case, be careful: the query
	/// may swap the underlying instance when processing if the provided buffer is to short. In this case, it is
	/// more sure to always update the Ptr when the query process is over.
	Ptr<QueryDynamicOutput> m_queryDynamicOutput;
};

}

#include "gwnavruntime/queries/utils/baseraycangoquery.inl"



#endif //Navigation_BaseRayCanGoQuery_H

