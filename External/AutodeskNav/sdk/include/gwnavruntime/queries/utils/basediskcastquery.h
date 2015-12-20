/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BaseDiskCastQuery_H
#define Navigation_BaseDiskCastQuery_H

#include "gwnavruntime/querysystem/iquery.h"

#include "gwnavruntime/queries/utils/queryutils.h"
#include "gwnavruntime/queries/utils/navmeshtraversalcommon.h"

#include "gwnavruntime/navmesh/worldintegerpos.h"
#include "gwnavruntime/navmesh/identifiers/navhalfedgeptr.h"

namespace Kaim
{

class NavTriangleRawPtr;

/// Enumerates the possible results of a DiskCastQuery.
enum DiskCastQueryResult
{
	DISKCAST_NOT_INITIALIZED = QUERY_NOT_INITIALIZED,  ///< Indicates that the query has not yet been initialized. 
	DISKCAST_NOT_PROCESSED   = QUERY_NOT_PROCESSED,    ///< Indicates that the query has not yet been launched. 

	DISKCAST_DONE_START_OUTSIDE,                       ///< Indicates that a NavMesh triangle could not be found for the starting point. 
	DISKCAST_DONE_START_NAVTAG_FORBIDDEN,              ///< Indicates that the NavTag at the starting point was forbidden. 
	DISKCAST_DONE_CANNOT_MOVE,                         ///< Indicates that the starting point is too close to a NavMesh border. Either the disk does not fit in the NavMesh at its start position, or moving back the collision point resulted in an arrival position that is behind the starting point. 
	DISKCAST_DONE_COLLISION_FOUND_ARRIVAL_ERROR,       ///< Indicates that moving back the collision point resulted in an arrival position that is outside of the NavMesh. 
	DISKCAST_DONE_LACK_OF_WORKING_MEMORY,              ///< Indicates that insufficient working memory caused the query to stop. 
	DISKCAST_DONE_UNKNOWN_ERROR,                       ///< Indicates that an unknown error occurred during the query processing. 

	DISKCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED,    ///< Indicates that the disk was able to travel its maximum distance without collision. 
	DISKCAST_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION,    ///< Indicates that a final arrival position was calculated successfully. 
};


/// Enumerates the possible behavior of a BaseDiskCastQuery when the the starting point lies inside the NavMesh but the radius
/// collides with the border of the NavMesh.
enum DiskCastQueryType
{
	/// Indicates that the query will detect a collision when the the starting point lies inside the NavMesh
	/// but the radius collides with the NavMesh border.
	DISKCAST_DONT_ADAPT_TO_STARTPOS,

	/// If the center of the disk lies near a boundary of the NavMesh, the query will try to avoid a collision
	/// by reducing the disk's radius (resulting in a non-symmetric crossed-capsule intersection test instead of 
	/// a capsule intersection test).
	/// In this case, the actual extents of the disk used in the test are first computed by doing two ray cast queries against the NavMesh at the
	/// starting position-in both directions orthogonally-to BaseDiskCastQuery::m_normalizedDir2d. Each raycast has a length
	/// equal to BaseDiskCastQuery::m_radius. Positions of the raycast collisions are moved back very slightly from the NavMesh boundary, and the resulting positions
	/// are used as the extent of the disk. This results in a disk that is smaller than the one set in BaseDiskCastQuery::m_radius, but whose extents are guaranteed
	/// to be fully inside the NavMesh at its starting point.
	/// Because the actual disk used in the query may be smaller than you expect, the test may miss borders that only intrude partially
	/// into the path of the expected disk.
	DISKCAST_ADAPT_TO_STARTPOS
};

/// Base class for DiskCastQuery.
class BaseDiskCastQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeDiskCast; }
	virtual QueryType GetType() const { return TypeDiskCast; }
	virtual void BuildQueryBlob(BaseBlobHandler* blobHandler);
	virtual void InitFromQueryBlob(World* world, void* blob);
	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return CreateStaticQueryBlobHandler(); }
	static Ptr<BaseBlobHandler> CreateStaticQueryBlobHandler();


	// ---------------------------------- Main API Functions ----------------------------------

	DiskCastQueryResult    GetResult()                  const;
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
	DiskCastQueryType      GetQueryType()               const;
	DynamicOutputMode      GetDynamicOutputMode()       const;
	QueryDynamicOutput*    GetQueryDynamicOutput()      const;
	const WorldIntegerPos& GetStartIntegerPos()         const;
	const WorldIntegerPos& GetArrivalIntegerPos()       const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

protected:
	BaseDiskCastQuery();
	virtual ~BaseDiskCastQuery() {}

	void BindToDatabase(Database* database);
	void Initialize(const Vec3f& startPos, KyFloat32 radius, const Vec2f& normalizedDir2d, KyFloat32 maxDist);

	void SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr);
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);
	void SetSafetyDist(KyFloat32 safetyDist);
	void SetQueryType(DiskCastQueryType queryType);
	void SetDynamicOutputMode(DynamicOutputMode savingMode);
	void SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput);
	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);
	void SetResult(DiskCastQueryResult result);

	DiskCastQueryResult ConvertTraversalResultToDiskCastResult(TraversalResult traversalRC);


protected:
	WorldIntegerPos m_startIntegerPos;
	WorldIntegerPos m_arrivalIntegerPos;

	Vec3f m_startPos3f;                ///< The start position of the disk. 
	Vec2f m_normalizedDir2d;           ///< A normalized vector representing the direction in which the disk is cast. 
	KyFloat32 m_radius;                ///< The radius of the disk. 
	KyFloat32 m_maxDist;               ///< The maximum distance the disk will travel. 
	NavTrianglePtr m_startTrianglePtr; ///< The NavMesh triangle that corresponds to #m_startPos3f. Can be set as an input, or retrieved as an output of the query. 

	/// When a collision is detected, the collision point is moved back along #m_normalizedDir2d by this distance, 
	/// to ensure that the final arrival position will lie within the valid area of the NavMesh. 
	KyFloat32 m_safetyDist;

	/// The altitude range around the position uses to determine #m_startTrianglePtr from #m_startPos3f.
	PositionSpatializationRange m_positionSpatializationRange;

	/// An element from the DiskCanGoQueryType enumeration that indicates if a degraded query should be run when the the starting point
	/// lies within the NavMesh but the radius collides with the border of the NavMesh.
	DiskCastQueryType m_queryType;

	/// Updated during processing to indicate the position of the collision point between the disk and the NavMesh border 
	/// (if a collision has been detected). 
	Vec3f m_collisionPos3f;

	/// Updated during processing to indicate the final position of the disk at the end of the cast:
	/// - If no collision is detected, this is the point that lies at a distance of #m_maxDist from the starting point
	///   in the direction of #m_normalizedDir2d.
	/// - If a collision is detected, this point is derived from #m_collisionPos3f by backtracking along the orientation
	///   of #m_normalizedDir2d a distance equal to #m_safetyDist. This ensures that #m_arrivalPos3f will be inside the boundaries
	///   of the NavMesh. 
	Vec3f m_arrivalPos3f;

	/// Updated during processing to indicate the NavMesh triangle that corresponds to #m_arrivalPos3f. The imprecision
	/// of floating-point calculations may render this triangle invalid after moving #m_arrivalPos3f back from the point of collision. 
	NavTrianglePtr m_arrivalTrianglePtr;

	NavHalfEdgePtr m_collisionHalfEdgePtr; ///< Updated during processing to indicate the triangle edge the disk collided with (if a collision is detected). 

	DiskCastQueryResult m_result; ///< Updated during processing to indicate the result of the query. 

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

#include "gwnavruntime/queries/utils/basediskcastquery.inl"



#endif //Navigation_BaseDiskCastQuery_H

