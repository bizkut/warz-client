/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BaseRayCastQuery_H
#define Navigation_BaseRayCastQuery_H


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

/// Enumerates the possible results of a RayCastQuery.
enum RayCastQueryResult
{
	RAYCAST_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates that the query has not yet been initialized. 
	RAYCAST_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates that the query has not yet been launched. 

	RAYCAST_DONE_START_OUTSIDE,                      ///< Indicates that a NavMesh triangle could not be found for the starting point. 
	RAYCAST_DONE_START_NAVTAG_FORBIDDEN,             ///< Indicates that a NavMesh triangle has been found for the starting point, but its NavTag is not considered navigable. 
	RAYCAST_DONE_CANNOT_MOVE,                        ///< Indicates that the starting point is very close to a NavMesh border, such that moving back the collision point resulted in an arrival position that is behind the starting point. 
	RAYCAST_DONE_COLLISION_FOUND_ARRIVAL_ERROR,      ///< Indicates that a collision point was found, but moving back from that point resulted in an arrival position that could not be snapped to the NavMesh grid. 
	RAYCAST_DONE_LACK_OF_WORKING_MEMORY,             ///< Indicates that insufficient memory was available to store crossed triangles or sub-sections, and the query failed as a result. 
	RAYCAST_DONE_QUERY_TOO_LONG,                     ///< Indicates that the query was not performed, because the value of BaseRayCastQuery::m_maxDist is large enough to cause a potential integer overflow during the computation. Retry your query with a smaller maximum distance. 
	RAYCAST_DONE_UNKNOWN_ERROR,                      ///< Indicates that an unknown error occurred during the query processing. 

	RAYCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED,   ///< Indicates that the ray was able to travel its maximum distance without collision. 
	RAYCAST_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION    ///< Indicates that a collision point was found and that a final arrival position was calculated successfully. 
};

/// Base class for RayCastQuery.
class BaseRayCastQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeRayCast; }
	virtual QueryType GetType() const { return TypeRayCast; }
	virtual void BuildQueryBlob(BaseBlobHandler* blobHandler);
	virtual void InitFromQueryBlob(World* world, void* blob);
	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return CreateStaticQueryBlobHandler(); }
	static Ptr<BaseBlobHandler> CreateStaticQueryBlobHandler();


	// ---------------------------------- Main API Functions ----------------------------------

	RayCastQueryResult     GetResult()                  const;
	const Vec3f&           GetStartPos()                const;
	const Vec3f&           GetCollisionPos()            const;
	const Vec3f&           GetArrivalPos()              const;
	const Vec2f&           GetMaxMove2D()         const;
	const NavTrianglePtr&  GetStartTrianglePtr()        const;
	const NavTrianglePtr&  GetArrivalTrianglePtr()      const;
	const NavHalfEdgePtr&  GetCollisionNavHalfEdgePtr() const;
	KyFloat32              GetMaxDist()                 const;
	DynamicOutputMode      GetDynamicOutputMode()       const;
	QueryDynamicOutput*    GetQueryDynamicOutput()      const;
	const WorldIntegerPos& GetStartIntegerPos()         const;
	const WorldIntegerPos& GetArrivalIntegerPos()       const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

	static CoordPos64 ResolveCollisionPointWhenPointsAreAligned(const CoordPos64& edgeStartPos,  const CoordPos64& edgeEndPos,  const CoordPos64& startCoordPos64,  const CoordPos64& destCoordPos64);
protected:
	BaseRayCastQuery();
	virtual ~BaseRayCastQuery() {}

	void BindToDatabase(Database* database);
	void Initialize(const Vec3f& startPos, const Vec2f& maxMove2D);

	void SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr);
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);
	void SetDynamicOutputMode(DynamicOutputMode dynamicOutputMode);
	void SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput);
	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);
	void SetResult(RayCastQueryResult result);

	void RayCast_NoHit(const NavHalfEdgeRawPtr& lastHalfEdgeRawPtr, const WorldIntegerPos& destIntegerPos, const Vec3f& destPos3f, RawNavTagSubSegment& navTagSubSegment,
		WorkingMemArray<RawNavTagSubSegment>& navTagSubSegments);
	KyResult ComputeCollisionPosition(const NavHalfEdgeRawPtr& halfEdgeIdRawPtrHit, const CoordPos64& startCoordPos64, const CoordPos64& destCoordPos64,
		CoordPos64& collisionCoordPos64, Vec3f& edgeStartPos3f, Vec3f& edgeEndPos3f, Vec3f& edgeThirdPos3f);

protected:
	WorldIntegerPos m_startIntegerPos;
	WorldIntegerPos m_arrivalIntegerPos;

	Vec3f m_startPos3f;      ///< The starting point of the line segment to be tested. 
	Vec3f m_collisionPos3f;  ///< Updated to indicate the position at which the ray collided with the NavMesh border or NavTag. 

	/// Updated during processing to indicate the final position of the ray at the end of the cast.
	/// - If no collision has been detected, this is the triangle arrived at by traversing the NavMesh in the direction of #maxMove2D.
	/// - If a collision is detected, this point is derived from #m_collisionPos3f by backtracking along the orientation
	///   of #maxMove2D. This tends to ensure that #m_arrivalPos3f will be inside the boundaries of the NavMesh.
	Vec3f m_arrivalPos3f;

	/// A vector that represents the direction and the maximum distance the ray will travel.
	/// If this distance is too high (approximately 32 times the length of a NavCell), performing
	/// the query can result in an integer overflow during calculation: in such case the query quits early
	/// and sets #m_result to #RAYCAST_DONE_QUERY_TOO_LONG. 
	Vec2f m_maxMove2D;

	/// The NavMesh triangle that corresponds to #m_startPos3f. You can set it as an input or retrieve it as an output of the query.
	NavTrianglePtr m_startTrianglePtr;

	/// The altitude range around the position to use when determining #m_startTrianglePtr from the #m_startPos3f.
	PositionSpatializationRange m_positionSpatializationRange;

	/// Updated during processing to indicate the NavMesh triangle that corresponds to #m_arrivalPos3f. Due to the imprecision
	/// of floating-point calculations, this triangle may be invalid even after moving #m_arrivalPos3f back from the point of collision. 
	NavTrianglePtr m_arrivalTrianglePtr;

	/// If a collision has been detected, this is updated to indicate the triangle edge that the ray collided with. 
	NavHalfEdgePtr m_collisionHalfEdgePtr;

	RayCastQueryResult m_result; ///< Updated during processing to indicate the result of the query. 

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

#include "gwnavruntime/queries/utils/baseraycastquery.inl"



#endif //Navigation_BaseRayCastQuery_H

