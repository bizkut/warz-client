/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BaseRayCastOnSegmentQuery_H
#define Navigation_BaseRayCastOnSegmentQuery_H


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

/// Enumerates the possible results of a RayCastOnSegmentQuery.
enum RayCastOnSegmentQueryResult
{
	RAYCASTONSEGMENT_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates that the query has not yet been initialized. 
	RAYCASTONSEGMENT_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates that the query has not yet been launched. 

	RAYCASTONSEGMENT_DONE_START_OUTSIDE,                      ///< Indicates that a NavMesh triangle could not be found for the starting point. 
	RAYCASTONSEGMENT_DONE_START_NAVTAG_FORBIDDEN,             ///< Indicates that a NavMesh triangle has been found for the starting point, but its NavTag is not considered navigable. 
	RAYCASTONSEGMENT_DONE_CANNOT_MOVE,                        ///< Indicates that the starting point is very close to a NavMesh border, such that moving back the collision point resulted in an arrival position that is behind the starting point. 
	RAYCASTONSEGMENT_DONE_ARRIVAL_ERROR,                      ///< Indicates that a collision point was found, but moving back from that point resulted in an arrival position that could not be snapped to the NavMesh grid.
	RAYCASTONSEGMENT_DONE_LACK_OF_WORKING_MEMORY,             ///< Indicates that insufficient memory was available to store crossed triangles or sub-sections, and the query failed as a result. 
	RAYCASTONSEGMENT_DONE_QUERY_TOO_LONG,                     ///< Indicates that the query was not performed, because the value of RayCastOnSegmentQuery::m_maxDist is large enough to cause a potential integer overflow during the computation. Retry your query with a smaller maximum distance. 
	RAYCASTONSEGMENT_DONE_UNKNOWN_ERROR,                      ///< Indicates that an unknown error occurred during the query processing. 

	RAYCASTONSEGMENT_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED,   ///< Indicates that the ray was able to travel its maximum distance without collision. 
	RAYCASTONSEGMENT_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION    ///< Indicates that the ray was able to travel its maximum distance without collision. 
};

/// Base class for RayCastOnSegmentQuery.
class BaseRayCastOnSegmentQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeRayCastOnSegment; }
	virtual QueryType GetType() const { return TypeRayCastOnSegment; }


	// ---------------------------------- Main API Functions ----------------------------------

	RayCastOnSegmentQueryResult GetResult()                 const;
	const Vec3f&                GetStartPos()               const;
	const Vec3f&                GetSegmentStartPos()        const;
	const Vec3f&                GetSegmentEndPos()          const;
	const Vec3f&                GetArrivalPos()             const;
	const NavTrianglePtr&       GetStartTrianglePtr()       const;
	const NavTrianglePtr&       GetArrivalTrianglePtr()     const;
	KyFloat32                   GetMaxDist()                const;
	DynamicOutputMode           GetDynamicOutputMode()      const;
	QueryDynamicOutput*         GetQueryDynamicOutput()     const;
	const WorldIntegerPos&      GetStartIntegerPos()        const;
	const WorldIntegerPos&      GetArrivalIntegerPos()      const;
	const WorldIntegerPos&      GetSegmentStartIntegerPos() const;
	const WorldIntegerPos&      GetSegmentEndIntegerPos()   const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

protected:
	BaseRayCastOnSegmentQuery();
	virtual ~BaseRayCastOnSegmentQuery() {}

	void BindToDatabase(Database* database);
	void Initialize(const Vec3f& startPos, const NavTrianglePtr& startTrianglePtr, const Vec3f& segmentStartPos, const Vec3f& segmentEndPos, KyFloat32 maxDist);

	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);
	void SetDynamicOutputMode(DynamicOutputMode dynamicOutputMode);
	void SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput);
	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);
	void SetSegmentStartIntegerPos(const WorldIntegerPos& segmentStartIntegerPos);
	void SetSegmentEndIntegerPos(const WorldIntegerPos& segmentEndIntegerPos);
	void SetResult(RayCastOnSegmentQueryResult result);

	void RayCast_NoHit(const NavHalfEdgeRawPtr& lastHalfEdgeRawPtr, const WorldIntegerPos& destIntegerPos, const Vec3f& destPos3f,
		RawNavTagSubSegment& navTagSubSegment, WorkingMemArray<RawNavTagSubSegment>& navTagSubSegments);

protected:
	WorldIntegerPos m_startIntegerPos;
	WorldIntegerPos m_arrivalIntegerPos;
	WorldIntegerPos m_segmentStartIntegerPos;
	WorldIntegerPos m_segmentEndIntegerPos;

	Vec3f m_segmentStartPos3f; ///< The starting point of the line segment support of this Cast.
	Vec3f m_segmentEndPos3f;   ///< The ending point of the line segment support of this Cast.
	Vec3f m_startPos3f;        ///< The starting point of propagation on the NavMesh.
	Vec3f m_collisionPos3f;    ///< Updated to indicate the position at which the ray collided with the NavMesh border, NavTag or Obstacle. 
	Vec3f m_arrivalPos3f;      ///< Updated during processing to indicate the final position of the ray at the end of the cast.

	/// The NavMesh triangle that corresponds to #m_startPos3f. You can set it as an input or retrieve it as an output of the query.
	NavTrianglePtr m_startTrianglePtr;

	/// Determines the maximum distance the disk will travel.
	/// If this distance is too high (approximately 32 times the length of a NavData cell), performing
	/// the query can result in an integer overflow during calculation: in such case, the query quits early
	/// and sets #m_result to #RAYCAST_DONE_QUERY_TOO_LONG. 
	KyFloat32 m_maxDist;

	/// The altitude range around the position to use when determining #m_startTrianglePtr from the #m_startPos3f.
	PositionSpatializationRange m_positionSpatializationRange;

	/// Updated during processing to indicate the NavMesh triangle that corresponds to #m_arrivalPos3f. Due to the imprecision
	/// of floating-point calculations, this triangle may be invalid even after moving #m_arrivalPos3f back from the point of collision. 
	NavTrianglePtr m_arrivalTrianglePtr;

	/// If a collision was detected, this is updated to indicate the triangle edge that the ray collided with. 
	NavHalfEdgePtr m_collisionHalfEdgePtr;

	RayCastOnSegmentQueryResult m_result; ///< Updated during processing to indicate the result of the query. 

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

#include "gwnavruntime/queries/utils/baseraycastonsegmentquery.inl"



#endif //Navigation_BaseRayCastOnSegmentQuery_H

