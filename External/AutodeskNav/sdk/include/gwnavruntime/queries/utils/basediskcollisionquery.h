/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BaseDiskCollisionQuery_H
#define Navigation_BaseDiskCollisionQuery_H


#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/queries/utils/queryutils.h"
#include "gwnavruntime/queries/utils/navmeshtraversalcommon.h"
#include "gwnavruntime/navmesh/worldintegerpos.h"

namespace Kaim
{

/// Enumerates the possible results of a DiskCollisionQuery.
enum DiskCollisionQueryResult
{
	DISKCOLLISION_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates that the query has not yet been initialized. 
	DISKCOLLISION_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates that the query has not yet been launched. 

	DISKCOLLISION_DONE_CENTER_OUTSIDE,                ///< Indicates that the center point lies outside the navigable area of the NavMesh. 
	DISKCOLLISION_DONE_CENTER_NAVTAG_FORBIDDEN,       ///< Indicates that the NavTag at the center point was forbidden. 
	DISKCOLLISION_DONE_DISK_DOES_NOT_FIT,             ///< Indicates that the center point lies inside the NavMesh, but that the radius collides with a border or crosses a forbidden terrain. 
	DISKCOLLISION_DONE_LACK_OF_WORKING_MEMORY,        ///< Indicates that insufficient working memory caused the query to stop. 
	DISKCOLLISION_DONE_UNKNOWN_ERROR,                 ///< Indicates that an unknown error occurred during the query processing. 

	DISKCOLLISION_DONE_DISK_FIT                       ///< Indicates that the disk fits inside the navigable area of the NavMesh. 
};

/// Base class for DiskCollisionQuery.
class BaseDiskCollisionQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeDiskCollision; }
	virtual QueryType GetType() const { return TypeDiskCollision; }
	virtual void BuildQueryBlob(BaseBlobHandler* blobHandler);
	virtual void InitFromQueryBlob(World* world, void* blob);
	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return CreateStaticQueryBlobHandler(); }
	static Ptr<BaseBlobHandler> CreateStaticQueryBlobHandler();


	// ---------------------------------- Main API Functions ----------------------------------

	DiskCollisionQueryResult GetResult()             const;
	const Vec3f&             GetCenterPos()          const;
	const NavTrianglePtr&    GetCenterTrianglePtr()  const;
	KyFloat32                GetRadius()             const;
	DynamicOutputMode        GetDynamicOutputMode()  const;
	QueryDynamicOutput*      GetQueryDynamicOutput() const;
	const WorldIntegerPos&   GetCenterIntegerPos()   const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

protected:
	BaseDiskCollisionQuery();
	~BaseDiskCollisionQuery() {}

	void BindToDatabase(Database* database);
	void Initialize(const Vec3f& center, KyFloat32 radius);

	void SetCenterTrianglePtr(const NavTrianglePtr& centerTrianglePtr);
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);
	void SetDynamicOutputMode(DynamicOutputMode savingMode);
	void SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput);
	void SetCenterIntegerPos(const WorldIntegerPos& centerIntegerPos);
	void SetResult(DiskCollisionQueryResult result);

	DiskCollisionQueryResult ConvertTraversalResultToDiskCollisionResult(TraversalResult traversalRC);

protected:
	WorldIntegerPos m_centerIntegerPos;

	Vec3f m_centerPos3f; ///< The position of the center of the disk. 

	/// The NavMesh triangle that corresponds to #m_centerPos3f. Can be set as an input, or retrieved as an output of the query. 
	NavTrianglePtr m_centerTrianglePtr;

	KyFloat32 m_radius; ///< The radius of the disk. 

	/// The altitude range around the position used when determining #m_centerTrianglePtr from the #m_centerPos3f.
	PositionSpatializationRange m_positionSpatializationRange;

	DiskCollisionQueryResult m_result; ///< Updated during processing to indicate the result of the query. 

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

#include "gwnavruntime/queries/utils/basediskcollisionquery.inl"

#endif //Navigation_BaseDiskCollisionQuery_H

