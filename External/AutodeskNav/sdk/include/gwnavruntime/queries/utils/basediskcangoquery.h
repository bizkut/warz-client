/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BaseDiskCanGoQuery_H
#define Navigation_BaseDiskCanGoQuery_H

#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/queries/utils/queryutils.h"
#include "gwnavruntime/queries/utils/navmeshtraversalcommon.h"
#include "gwnavruntime/navmesh/worldintegerpos.h"
#include "gwnavruntime/queries/utils/baseraycangoquery.h"


namespace Kaim
{

class NavTriangleRawPtr;

/// Enumerates the possible results of a DiskCanGoQuery.
enum DiskCanGoQueryResult
{
	DISKCANGO_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	DISKCANGO_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	DISKCANGO_DONE_START_OUTSIDE,                      ///< Indicates that a NavMesh triangle could not be found for the starting point. 
	DISKCANGO_DONE_START_NAVTAG_FORBIDDEN,             ///< Indicates that a NavMesh triangle has been found for the starting point, but its NavTag is not considered navigable. 
	DISKCANGO_DONE_COLLISION_DETECTED,                 ///< Indicates that a collision was detected along the straight line between the starting and ending points. 
	DISKCANGO_DONE_ARRIVAL_WRONG_FLOOR,                ///< Indicates the query generated no results, which may indicate that the start and end points are on different floors. 
	DISKCANGO_DONE_LACK_OF_WORKING_MEMORY,             ///< Indicates that insufficient working memory caused the query to stop. 
	DISKCANGO_DONE_UNKNOWN_ERROR,                      ///< Indicates that an unknown error occurred during the query processing. 

	DISKCANGO_DONE_SUCCESS                             ///< Indicates that the disk can pass in a straight line-from start to end point-without a collision. 
};

/// Enumerates the possible behaviors of a BaseDiskCanGoQuery when the the starting point lies inside the NavMesh but the radius
/// collides with the border of the NavMesh.
enum DiskCanGoQueryType
{
	/// Indicates that the query will detect a collision when the the starting point lies inside the NavMesh
	/// but the radius collides with the NavMesh border.
	DISKCANGO_DONT_ADAPT_TO_STARTPOS,

	/// If the center of the disk lies near a boundary of the NavMesh, the query will try to avoid a collision
	/// by reducing the disk's radius (resulting in a non-symetric crossed-capsule intersection test instead of 
	/// a capsule intersection test).
	/// In this case, the actual extents of the disk used in the test are first computed by doing two raycast queries against the NavMesh at the
	/// starting position-in both directions orthogonally-in the direction of BaseDiskCanGoQuery::m_startPos3f to BaseDiskCanGoQuery::m_destPos3f. Each raycast has a length
	/// equal to BaseDiskCanGoQuery::m_radius. Positions of the raycast collisions are moved back very slightly from the NavMesh boundary, and the resulting positions
	/// are used as the extent of the disk. This results in a disk that is smaller than the one set in BaseDiskCanGoQuery::m_radius, but whose extents are guaranteed
	/// to be fully inside the NavMesh at its starting point.
	/// Because the actual disk used in the query may be smaller than you expect, the test may miss borders that only intrude partially
	/// into the path of the expected disk.
	DISKCANGO_ADAPT_TO_STARTPOS
};


/// Base class for DiskCanGoQuery.
class BaseDiskCanGoQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeDiskCanGo; }
	virtual QueryType GetType() const { return TypeDiskCanGo; }
	virtual void BuildQueryBlob(BaseBlobHandler* blobHandler);
	virtual void InitFromQueryBlob(World* world, void* blob);
	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return CreateStaticQueryBlobHandler(); }
	static Ptr<BaseBlobHandler> CreateStaticQueryBlobHandler();


	// ---------------------------------- Main API functions ----------------------------------

	DiskCanGoQueryResult   GetResult()             const;
	const Vec3f&           GetStartPos()           const;
	const Vec3f&           GetDestPos()            const;
	const NavTrianglePtr&  GetStartTrianglePtr()   const;
	const NavTrianglePtr&  GetDestTrianglePtr()    const;
	KyFloat32              GetRadius()             const;
	DiskCanGoQueryType     GetQueryType()          const;
	DynamicOutputMode      GetDynamicOutputMode()  const;
	QueryDynamicOutput*    GetQueryDynamicOutput() const;
	const WorldIntegerPos& GetStartIntegerPos()    const;
	const WorldIntegerPos& GetDestIntegerPos()     const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

protected:
	BaseDiskCanGoQuery();
	virtual ~BaseDiskCanGoQuery() {}

	void BindToDatabase(Database* database);
	void Initialize(const Vec3f& startPos, const Vec3f& destPos, KyFloat32 radius);

	void SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr);
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);
	void SetQueryType(DiskCanGoQueryType queryType);
	void SetDynamicOutputMode(DynamicOutputMode savingMode);
	void SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput);
	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);
	void SetDestIntegerPos(const WorldIntegerPos& destIntegerPos);
	void SetResult(DiskCanGoQueryResult result);

	DiskCanGoQueryResult ConvertRayCanGoResultToDiskCanGoResult(RayCanGoQueryResult result);
	DiskCanGoQueryResult ConvertTraversalResultToDiskCanGoResult(TraversalResult traversalRC);

protected:
	WorldIntegerPos m_startIntegerPos;
	WorldIntegerPos m_destIntegerPos;

	Vec3f m_startPos3f; ///< The start point of the line segment to be tested.
	Vec3f m_destPos3f;  ///< The end point of the line segment to be tested.

	/// The NavMesh triangle that corresponds to #m_startPos3f. Can be set as an
	/// input, or retrieved as an output of the query.
	NavTrianglePtr m_startTrianglePtr;

	/// The NavMesh triangle that corresponds to #m_destPos3f. It will be computed
	/// automatically during the query.
	NavTrianglePtr m_destTrianglePtr; 

	KyFloat32 m_radius; ///< The radius of the disk.

	/// The altitude range around the position used when determining #m_startTrianglePtr
	/// and #m_destTrianglePtr from #m_startPos3f and #m_destPos3f.
	PositionSpatializationRange m_positionSpatializationRange;

	/// An element from the #DiskCanGoQueryType enumeration that indicates if a
	/// degraded query should be run when the the starting point lies within the
	/// NavMesh but the radius collides with the border of the NavMesh.
	DiskCanGoQueryType m_queryType;

	DiskCanGoQueryResult m_result; ///< Updated during processing to indicate the result of the query.

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

#include "gwnavruntime/queries/utils/basediskcangoquery.inl"



#endif //Navigation_BaseDiskCanGoQuery_H


