/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BaseSegmentCastQuery_H
#define Navigation_BaseSegmentCastQuery_H

#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/queries/utils/queryutils.h"
#include "gwnavruntime/navmesh/worldintegerpos.h"
#include "gwnavruntime/navmesh/identifiers/navhalfedgeptr.h"
#include "gwnavruntime/queries/utils/navmeshtraversalcommon.h"


namespace Kaim
{

/// Enumerates the possible results of a SegmentCastQuery.
enum SegmentCastQueryResult
{
	SEGMENTCAST_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates that the query has not yet been initialized. 
	SEGMENTCAST_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates that the query has not yet been launched. 

	SEGMENTCAST_DONE_START_OUTSIDE,                      ///< Indicates that a NavMesh triangle could not be found for the starting point. 
	SEGMENTCAST_DONE_START_NAVTAG_FORBIDDEN,             ///< Indicates that the NavTag at the starting point was forbidden. 
	SEGMENTCAST_DONE_CANNOT_MOVE,                        ///< Indicates that the starting point is very close to a NavMesh border. Either the segment does not fit in the NavMesh at its start position, or moving back the collision point resulted in an arrival position that is behind the starting point. 
	SEGMENTCAST_DONE_COLLISION_FOUND_ARRIVAL_ERROR,      ///< Indicates that moving back the collision point resulted in an arrival position that is outside of the NavMesh. 
	SEGMENTCAST_DONE_LACK_OF_WORKING_MEMORY,             ///< Indicates that insufficient working memory caused the query to stop. 
	SEGMENTCAST_DONE_UNKNOWN_ERROR,                      ///< Indicates that an error occured during the query computation

	SEGMENTCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED,   ///< Indicates that the segment was able to travel its maximum distance without collision. 
	SEGMENTCAST_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION    ///< Indicates that a final arrival position was calculated successfully. 
};

/// Base class for SegmentCastQuery.
class BaseSegmentCastQuery : public IAtomicQuery
{
public :
	static  QueryType GetStaticType() { return TypeSegmentCast; }
	virtual QueryType GetType() const { return TypeSegmentCast; }
	virtual void BuildQueryBlob(BaseBlobHandler* blobHandler);
	virtual void InitFromQueryBlob(World* world, void* blob);
	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return CreateStaticQueryBlobHandler(); }
	static Ptr<BaseBlobHandler> CreateStaticQueryBlobHandler();


	// ---------------------------------- Main API Functions ----------------------------------

	SegmentCastQueryResult GetResult()                  const;
	const Vec3f&           GetStartPos()                const;
	const Vec3f&           GetCollisionPos()            const;
	const Vec3f&           GetArrivalPos()              const;
	const Vec2f&           GetNormalizedDir2d()         const;
	const NavTrianglePtr&  GetStartTrianglePtr()        const;
	const NavTrianglePtr&  GetArrivalTrianglePtr()      const;
	const NavHalfEdgePtr&  GetCollisionNavHalfEdgePtr() const;
	KyFloat32              GetRadius()                  const;
	KyFloat32              GetMaxDist()                 const;
	KyFloat32              GetSafetyDist()              const;
	DynamicOutputMode      GetDynamicOutputMode()       const;
	QueryDynamicOutput*    GetQueryDynamicOutput()      const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

	const WorldIntegerPos& GetStartIntegerPos() const;
	const WorldIntegerPos& GetArrivalIntegerPos() const;

protected:
	BaseSegmentCastQuery();
	virtual ~BaseSegmentCastQuery(){}

	void BindToDatabase(Database* database);
	void Initialize(const Vec3f& startPos, KyFloat32 radius, const Vec2f& normalizedDir2d, KyFloat32 maxDist);

	void SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr);
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);
	void SetSafetyDist(KyFloat32 safetyDist);
	void SetDynamicOutputMode(DynamicOutputMode savingMode);
	void SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput);

	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);
	void SetResult(SegmentCastQueryResult result);
	SegmentCastQueryResult ConvertTraversalResultToSegmentCastResult(TraversalResult traversalRC);

protected:
	WorldIntegerPos m_startIntegerPos;
	WorldIntegerPos m_arrivalIntegerPos;

	Vec3f m_startPos3f;                ///< The starting position of the segment. 
	Vec2f m_normalizedDir2d;           ///< A normalized vector that represents the direction in which the segment is cast. 
	KyFloat32 m_radius;                ///< The half-width of the segment .
	KyFloat32 m_maxDist;               ///< Determines the maximum distance the segment will travel. 
	NavTrianglePtr m_startTrianglePtr; ///< The NavMesh triangle that corresponds to #m_startPos3f. You can set it as an input or retrieve it as an output of the query 

	/// When a collision is detected, the collision point is moved back along #m_normalizedDir2d by this distance, in order
	/// to ensure that the final arrival position will lie within the valid area of the NavMesh. 
	KyFloat32 m_safetyDist;

	/// The altitude range around the position to use when determining #m_startTrianglePtr from the #m_startPos3f.
	PositionSpatializationRange m_positionSpatializationRange;

	/// Updated during processing to indicate the position of the collision point between the segment and the NavMesh border,
	/// if a collision has been detected. 
	Vec3f m_collisionPos3f;

	/// Updated during processing to indicate the final position of the segment at the end of the cast.
	/// - If no collision is detected, this is the point that lies at a distance of #m_maxDist from the starting point
	///   in the direction of #m_normalizedDir2d.
	/// - If a collision is detected, this point is derived from #m_collisionPos3f by backtracking along the orientation
	///   of #m_normalizedDir2d a distance equal to #m_safetyDist. This tends to ensure that #m_arrivalPos3f will be inside the boundaries
	///   of the NavMesh. 
	Vec3f m_arrivalPos3f;

	/// Updated during processing to indicate the NavMesh triangle that corresponds to #m_arrivalPos3f. Due to the imprecision
	/// of floating-point calculations, this triangle may be invalid even after moving #m_arrivalPos3f back from the point of collision. 
	NavTrianglePtr m_arrivalTrianglePtr;

	/// Updated during processing to indicate the triangle edge that the segment collided with, if a collision is detected.
	NavHalfEdgePtr m_collisionHalfEdgePtr;

	SegmentCastQueryResult m_result; ///< Updated during processing to indicate the result of the query. 

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

#include "gwnavruntime/queries/utils/basesegmentcastquery.inl"

#endif //Navigation_BaseSegmentCastQuery_H

