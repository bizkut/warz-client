/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BaseInsidePosFromOutsidePosQuery_H
#define Navigation_BaseInsidePosFromOutsidePosQuery_H

#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/navmesh/worldintegerpos.h"
#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"
#include "gwnavruntime/database/positionspatializationrange.h"



namespace Kaim
{

class BaseNearestBorderHalfEdgeFromPosQuery;


/// Enumerates the possible results of an InsidePosFromOutsidePosQuery.
enum InsidePosFromOutsidePosQueryResult
{
	INSIDEPOSFROMOUTSIDE_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	INSIDEPOSFROMOUTSIDE_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	INSIDEPOSFROMOUTSIDE_DONE_POS_NOT_FOUND,                      ///< Indicates that a valid position was not found within the query's bounding box. 
	INSIDEPOSFROMOUTSIDE_DONE_POS_FOUND                           ///< Indicates that the query was completed successfully, and a valid position on the NavMesh was found within the query's bounding box. 
};

/// Base class for InsidePosFromOutsidePosQuery.
class BaseInsidePosFromOutsidePosQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeInsidePosFromOutsidePos; }
	virtual QueryType GetType() const { return TypeInsidePosFromOutsidePos; }
	virtual void BuildQueryBlob(BaseBlobHandler* blobHandler);
	virtual void InitFromQueryBlob(World* world, void* blob);
	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return CreateStaticQueryBlobHandler(); }
	static Ptr<BaseBlobHandler> CreateStaticQueryBlobHandler();


	// ---------------------------------- Main API Functions ----------------------------------

	InsidePosFromOutsidePosQueryResult GetResult()               const;
	const Vec3f&                       GetInputPos()             const;
	const Vec3f&                       GetInsidePos()            const;
	const NavTrianglePtr&              GetInsidePosTrianglePtr() const;
	KyFloat32                          GetHorizontalTolerance()  const;
	KyFloat32                          GetDistFromObstacle()     const;
	const WorldIntegerPos&             GetInputIntegerPos()      const;
	const WorldIntegerPos&             GetInsideIntegerPos()     const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

protected:
	BaseInsidePosFromOutsidePosQuery();
	~BaseInsidePosFromOutsidePosQuery() {}

	void BindToDatabase(Database* database);
	void Initialize(const Vec3f& inputPos);

	void SetHorizontalTolerance(KyFloat32 horizontalTolerance);
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);
	void SetDistFromObstacle(KyFloat32 distFromObstacle);
	void SetInputIntegerPos(const WorldIntegerPos& inputIntegerPos);
	void SetResult(InsidePosFromOutsidePosQueryResult result);

	KyResult ProcessNearestBorderHalfEdgeFromPosQueryResult(WorkingMemory* workingMemory,
		const BaseNearestBorderHalfEdgeFromPosQuery& nearestBorderHalfEdgeFromPosQuery, Vec3f& startToEndNormalInside2d);

protected:
	WorldIntegerPos m_inputIntegerPos;
	WorldIntegerPos m_insideIntegerPos;

	Vec3f m_inputPos3f; ///< The starting position for the query. 
	
	KyFloat32 m_horizontalTolerance;    ///< The horizontal half-width (along both the X and Y axes) of the axis-aligned bounding box within which the search is performed. 
	PositionSpatializationRange m_positionSpatializationRange; ///< The Z extent from #m_inputPos3f of the axis-aligned bounding box within which the search is performed.

	/// Specifies a preferred distance between the final chosen point and the border of the NavMesh. If this value is specified, the query
	/// will attempt to find a final valid point that is this distance inside the border of the NavMesh. However, in some cases, the layout
	/// of the geometry may force the final valid point to be closer to the NavMesh border than this value. 
	KyFloat32 m_distFromObstacle;
	
	Vec3f m_insidePos3f; ///< Updated during processing to store the valid point found inside the NavMesh, if any. 

	NavTrianglePtr m_insidePosTrianglePtr; ///< Updated during processing to store the NavMesh triangle that contains #m_insidePos3f. 

	InsidePosFromOutsidePosQueryResult m_result; ///< Updated during processing to indicate the result of the query. 
};

}

#include "gwnavruntime/queries/utils/baseinsideposfromoutsideposquery.inl"



#endif //Navigation_BaseInsidePosFromOutsidePosQuery_H

