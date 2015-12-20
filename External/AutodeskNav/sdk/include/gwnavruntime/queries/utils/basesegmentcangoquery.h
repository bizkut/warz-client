/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BaseSegmentCanGoQuery_H
#define Navigation_BaseSegmentCanGoQuery_H

#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/queries/utils/queryutils.h"
#include "gwnavruntime/navmesh/worldintegerpos.h"
#include "gwnavruntime/queries/utils/baseraycangoquery.h"
#include "gwnavruntime/queries/utils/navmeshtraversalcommon.h"

namespace Kaim
{

/// Enumerates the possible results of a SegmentCanGoQuery.
enum SegmentCanGoQueryResult
{
	SEGMENTCANGO_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates that the query has not yet been initialized. 
	SEGMENTCANGO_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates that the query has not yet been launched. 

	SEGMENTCANGO_DONE_START_OUTSIDE,                      ///< Indicates that a NavMesh triangle could not be found for the starting point. 
	SEGMENTCANGO_DONE_START_NAVTAG_FORBIDDEN,             ///< Indicates that the NavTag at the starting point was forbidden. 
	SEGMENTCANGO_DONE_COLLISION_DETECTED,                 ///< Indicates that a collision was detected between the starting and ending points. 
	SEGMENTCANGO_DONE_ARRIVAL_WRONG_FLOOR,                ///< Indicates that none of the above results occurred, which may indicate that the start and end points lie on different floors. 
	SEGMENTCANGO_DONE_LACK_OF_WORKING_MEMORY,             ///< Indicates that insufficient working memory caused the query to stop. 
	SEGMENTCANGO_DONE_UNKNOWN_ERROR,                      ///< Indicates that an unknown error occurred during the query processing. 

	SEGMENTCANGO_DONE_SUCCESS                             ///< Indicates that the segment can pass in a straight line from its starting point to the ending point without a collision. 
};

/// Base class for SegmentCanGoQuery.
class BaseSegmentCanGoQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeSegmentCanGo; }
	virtual QueryType GetType() const { return TypeSegmentCanGo; }
	virtual void BuildQueryBlob(BaseBlobHandler* blobHandler);
	virtual void InitFromQueryBlob(World* world, void* blob);
	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return CreateStaticQueryBlobHandler(); }
	static Ptr<BaseBlobHandler> CreateStaticQueryBlobHandler();


	// ---------------------------------- Main API Functions ----------------------------------

	SegmentCanGoQueryResult GetResult()             const;
	const Vec3f&            GetStartPos()           const;
	const Vec3f&            GetDestPos()            const;
	const NavTrianglePtr&   GetStartTrianglePtr()   const;
	const NavTrianglePtr&   GetDestTrianglePtr()    const;
	KyFloat32               GetRadius()             const;
	DynamicOutputMode       GetDynamicOutputMode()  const;
	QueryDynamicOutput*     GetQueryDynamicOutput() const;
	const WorldIntegerPos&  GetStartIntegerPos()    const;
	const WorldIntegerPos&  GetDestIntegerPos()     const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

protected:
	BaseSegmentCanGoQuery();
	~BaseSegmentCanGoQuery() {}

	void BindToDatabase(Database* database);
	void Initialize(const Vec3f& startPos, const Vec3f& destPos, KyFloat32 radius);

	void SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr);
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);
	void SetDynamicOutputMode(DynamicOutputMode savingMode);
	void SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput);
	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);
	void SetDestIntegerPos(const WorldIntegerPos& destIntegerPos);
	void SetResult(SegmentCanGoQueryResult result);

	SegmentCanGoQueryResult ConvertRayCanGoResultToSegmentCanGoResult(RayCanGoQueryResult result);
	SegmentCanGoQueryResult ConvertTraversalResultToSegmentCanGoResult(TraversalResult traversalRC);

protected:
	WorldIntegerPos m_startIntegerPos;
	WorldIntegerPos m_destIntegerPos;

	Vec3f m_startPos3f;                ///< The starting point of the line segment to be tested. 
	Vec3f m_destPos3f;                 ///< The ending point of the line segment to be tested. 
	NavTrianglePtr m_startTrianglePtr; ///< The NavMesh triangle that corresponds to #m_startPos3f. You can set it as an input or retrieve it as an output of the query 
	NavTrianglePtr m_destTrianglePtr;  ///< The NavMesh triangle that corresponds to #m_destPos3f. It will be computed automatically during the query. 
	KyFloat32 m_radius;                ///< The radius of the disk. 

	/// The altitude range around the position to use when determining #m_startTrianglePtr and #m_destTrianglePtr from the #m_startPos3f and #m_destPos3f. 
	PositionSpatializationRange m_positionSpatializationRange;

	SegmentCanGoQueryResult m_result; ///< Updated during processing to indicate the results of the query. 

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

#include "gwnavruntime/queries/utils/basesegmentcangoquery.inl"

#endif //Navigation_BaseSegmentCanGoQuery_H

