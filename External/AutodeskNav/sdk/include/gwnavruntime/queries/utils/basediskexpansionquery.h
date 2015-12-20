/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BaseDiskExpansionQuery_H
#define Navigation_BaseDiskExpansionQuery_H

#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/queries/utils/queryutils.h"
#include "gwnavruntime/navmesh/worldintegerpos.h"
#include "gwnavruntime/navmesh/identifiers/navhalfedgeptr.h"
#include "gwnavruntime/queries/utils/navmeshtraversalcommon.h"


namespace Kaim
{

/// Enumerates the possible results of a DiskExpansionQuery.
enum DiskExpansionQueryResult
{
	DISKEXPANSION_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	DISKEXPANSION_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	DISKEXPANSION_DONE_CENTER_OUTSIDE,                     ///< Indicates that the center point lies outside the navigable area of the NavMesh. 
	DISKEXPANSION_DONE_CENTER_NAVTAG_FORBIDDEN,            ///< Indicates that the NavTag at the center point is forbidden. 
	DISKEXPANSION_DONE_CANNOT_EXPAND,                      ///< Indicates that the center point lies within the NavMesh, but the disk cannot be expanded. 
	DISKEXPANSION_DONE_LACK_OF_WORKING_MEMORY,             ///< Indicates that insufficient working memory caused the query to stop. 
	DISKEXPANSION_DONE_UNKNOWN_ERROR,                      ///< Indicates that an unknown error occurred during the query processing. 

	DISKEXPANSION_DONE_DISK_FIT,                           ///< Indicates that the disk fits inside the navigable areas of the NavMesh even at maximum size. 
	DISKEXPANSION_DONE_COLLISION_FOUND                     ///< Indicates a collision was detected when expanding the disk before the maximum size of the disk was reached. 
};

/// Base class for DiskExpansionQuery.
class BaseDiskExpansionQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeDiskExpansion; }
	virtual QueryType GetType() const { return TypeDiskExpansion; }
	virtual void BuildQueryBlob(BaseBlobHandler* blobHandler);
	virtual void InitFromQueryBlob(World* world, void* blob);
	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return CreateStaticQueryBlobHandler(); }
	static Ptr<BaseBlobHandler> CreateStaticQueryBlobHandler();


	// ---------------------------------- Main API Functions ----------------------------------

	DiskExpansionQueryResult GetResult()                  const;
	const Vec3f&             GetCenterPos()               const;
	const NavTrianglePtr&    GetCenterTrianglePtr()       const;
	KyFloat32                GetRadiusMax()               const;
	KyFloat32                GetResultRadius()            const;
	const Vec3f&             GetCollisionPos()            const;
	const NavHalfEdgePtr&    GetCollisionNavHalfEdgePtr() const;
	KyFloat32                GetSafetyDist()              const;
	DynamicOutputMode        GetDynamicOutputMode()       const;
	QueryDynamicOutput*      GetQueryDynamicOutput()      const;
	const WorldIntegerPos&   GetCenterIntegerPos()        const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

protected:
	BaseDiskExpansionQuery();
	virtual ~BaseDiskExpansionQuery() {}

	void BindToDatabase(Database* database);
	void Initialize(const Vec3f& center, KyFloat32 radiusMax);

	void SetCenterTrianglePtr(const NavTrianglePtr& centerTrianglePtr);
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);
	void SetSafetyDist(KyFloat32 safetyDist);
	void SetDynamicOutputMode(DynamicOutputMode savingMode);
	void SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput);
	void SetCenterIntegerPos(const WorldIntegerPos& centerIntegerPos);
	void SetResult(DiskExpansionQueryResult result);

	DiskExpansionQueryResult ConvertTraversalResultToDiskExpansionResult(TraversalResult traversalRC);
protected:
	WorldIntegerPos m_centerIntegerPos;

	Vec3f m_centerPos3f; ///< The position of the center of the disk.

	/// The NavMesh triangle that corresponds to #m_centerPos3f. If a valid NavTrianglePtr is specified, this value will be used.
	/// If no value is specified, it will be automatically computed during the query. 
	NavTrianglePtr m_centerTrianglePtr;

	/// The maximum radius for the disk. If the disk expands to this radius without a collision, the query stops. 
	KyFloat32 m_radiusMax;

	/// When a collision is detected, the radius is decreased by this distance, 
	/// to ensure the final disk will lie completely within the valid area of the NavMesh.
	/// \units    meters
	/// \defaultvalue   0.01
	KyFloat32 m_safetyDist;

	/// The altitude range around the position used to determine #m_centerTrianglePtr from the #m_centerPos3f.
	PositionSpatializationRange m_positionSpatializationRange;

	/// Updated during processing to indicate the position of the collision on the NavMesh border 
	/// (if a collision is detected). 
	Vec3f m_collisionPos3f;

	/// Updated during processing to indicate the triangle edge that the disk collided with 
	/// (if a collision is detected). 
	NavHalfEdgePtr m_collisionHalfEdgePtr;

	/// Updated during processing to indicate the final result of the disk at the end of the query:
	/// -    If no collision is detected, this value will match #m_radiusMax.
	/// -    If a collision is detected, this value will be the largest valid radius that fits in the available space. 
	KyFloat32 m_resultRadius;

	DiskExpansionQueryResult m_result; ///< Updated during processing to indicate the result of the query. 

	DynamicOutputMode m_dynamicOutputMode; ///< Specifies the type of data stored in #m_queryDynamicOutput.

	/// A ref-counted Ptr to a QueryDynamicOutput instance, used to store the data encountered during the query process.
	/// You have to first call SetDynamicOutputMode(), after BindToDatabase() that set #m_dynamicOutputMode to #QUERY_SAVE_NOTHING by default
	/// and before processing the query, to define the data type you want to gather.
	/// You can keep the Ptr for reuse and reassign it to another query by calling SetQueryDynamicOutput() after the BindToDatabase(), but in this case, be careful: the query
	/// may swap the underlying instance when processing if the provided buffer is to short. In this case, it is
	/// more sure to always update the Ptr when the query process is over.
	Ptr<QueryDynamicOutput> m_queryDynamicOutput;
};

}

#include "gwnavruntime/queries/utils/basediskexpansionquery.inl"

#endif //Navigation_DiskCollisionQuery_H

