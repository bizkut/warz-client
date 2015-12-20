/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BaseRayCanGoOnSegmentQuery_H
#define Navigation_BaseRayCanGoOnSegmentQuery_H


#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/queries/utils/rayqueryutils.h"
#include "gwnavruntime/queries/utils/queryutils.h"
#include "gwnavruntime/navmesh/worldintegerpos.h"
#include "gwnavruntime/navmesh/identifiers/navhalfedgeptr.h"
#include "gwnavruntime/database/positionspatializationrange.h"



namespace Kaim
{

class NavHalfEdgeRawPtr;
class RawNavTagSubSegment;
template <class T> class WorkingMemArray;

/// Enumerates the possible results of a RayCanGoOnSegmentQuery.
enum RayCanGoOnSegmentQueryResult
{
	RAYCANGOONSEGMENT_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	RAYCANGOONSEGMENT_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	RAYCANGOONSEGMENT_DONE_START_OUTSIDE,                      ///< Indicates that a NavMesh triangle could not be found for the starting point. 
	RAYCANGOONSEGMENT_DONE_START_NAVTAG_FORBIDDEN,             ///< Indicates that a NavMesh triangle has been found for the starting point, but its NavTag is not considered navigable. 
	RAYCANGOONSEGMENT_DONE_COLLISION_DETECTED,                 ///< Indicates that the ray was able to travel its maximum distance without collision. 
	RAYCANGOONSEGMENT_DONE_ARRIVAL_IN_WRONG_FLOOR,             ///< Indicates that none of the above results occurred, which may indicate that the start and end points lie on different floors. 
	RAYCANGOONSEGMENT_DONE_LACK_OF_WORKING_MEMORY,             ///< Indicates that insufficient memory was available to store crossed triangles or sub-sections, and the query failed as a result. 
	RAYCANGOONSEGMENT_DONE_QUERY_TOO_LONG,                     ///< Indicates that the query was not performed, because the value of BaseRayCanGoOnSegmentQuery::m_maxDist is large enough to cause a potential integer overflow during the computation. Retry your query with a smaller maximum distance. 

	RAYCANGOONSEGMENT_DONE_SUCCESS,                            ///< Indicates that the ray was able to travel its maximum distance without collision.
};

/// Base class for RayCanGoOnSegmentQuery.
class BaseRayCanGoOnSegmentQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeRayCanGoOnSegment; }
	virtual QueryType GetType() const { return TypeRayCanGoOnSegment; }


	// ---------------------------------- Main API Functions ----------------------------------

	RayCanGoOnSegmentQueryResult       GetResult()                      const;
	const Vec3f&                       GetStartPos()                    const;
	const Vec3f&                       GetSegmentStartPos()             const;
	const Vec3f&                       GetSegmentEndPos()               const;
	const NavTrianglePtr&              GetStartTrianglePtr()            const;
	const NavTrianglePtr&              GetArrivalTrianglePtr()          const;
	DynamicOutputMode                  GetDynamicOutputMode()           const;
	QueryDynamicOutput*                GetQueryDynamicOutput()          const;
	const PositionSpatializationRange& GetPositionSpatializationRange() const;
	const WorldIntegerPos&             GetStartIntegerPos()             const;
	const WorldIntegerPos&             GetSegmentStartIntegerPos()      const;
	const WorldIntegerPos&             GetSegmentEndIntegerPos()        const;

protected:
	BaseRayCanGoOnSegmentQuery();
	~BaseRayCanGoOnSegmentQuery() {}

	void BindToDatabase(Database* database);
	void Initialize(const Vec3f& startPos, const NavTrianglePtr& startTrianglePtr, const Vec3f& segmentStartPos, const Vec3f& segmentEndPos);

	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);
	void SetDynamicOutputMode(DynamicOutputMode dynamicOutputMode);
	void SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput);
	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);
	void SetSegmentStartIntegerPos(const WorldIntegerPos& segmentStartIntegerPos);
	void SetSegmentEndIntegerPos(const WorldIntegerPos& segmentEndIntegerPos);
	void SetResult(RayCanGoOnSegmentQueryResult result);

	void RayCanGo_NoHit(const NavHalfEdgeRawPtr& lastHalfEdgeRawPtr, RawNavTagSubSegment& navTagSubSegment, WorkingMemArray<RawNavTagSubSegment>& navTagSubSegments);

protected:
	WorldIntegerPos m_startIntegerPos;
	WorldIntegerPos m_segmentStartIntegerPos;
	WorldIntegerPos m_segmentEndIntegerPos;

	Vec3f m_segmentStartPos3f; ///< The starting point of the line segment support of this CanGo.
	Vec3f m_segmentEndPos3f;   ///< The ending point of the line segment support of this CanGo.
	Vec3f m_startPos3f;        ///< The starting point of the propagation.

	NavTrianglePtr m_startTrianglePtr; ///< The NavMesh triangle that corresponds to #m_startPos3f. You can set it as an input or retrieve it as an output of the query.

	/// The altitude range around the position to use when determining #m_startTrianglePtr and #m_destTrianglePtr from the #m_startPos3f and #m_segmentEndPos3f. 
	PositionSpatializationRange m_positionSpatializationRange;

	/// Updated during processing to indicate the NavMesh triangle that corresponds to #m_segmentEndPos3f.
	NavTrianglePtr m_destTrianglePtr;

	RayCanGoOnSegmentQueryResult m_result; ///< Updated during processing to indicate the result of the query. 

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

#include "gwnavruntime/queries/utils/baseraycangoonsegmentquery.inl"



#endif //Navigation_BaseRayCanGoOnSegmentQuery_H

