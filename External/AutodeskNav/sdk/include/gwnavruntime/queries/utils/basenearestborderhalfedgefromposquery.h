/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/




// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_BaseNearestBorderHalfEdgeFromPosQuery_H
#define Navigation_BaseNearestBorderHalfEdgeFromPosQuery_H

#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/navmesh/worldintegerpos.h"
#include "gwnavruntime/navmesh/identifiers/navhalfedgeptr.h"
#include "gwnavruntime/database/positionspatializationrange.h"


namespace Kaim
{

class ActiveCell;
template <class T> class WorkingMemArray;

/// Enumerates the possible results of a NearestBorderHalfEdgeFromPosQuery.
enum NearestBorderHalfEdgeFromPosQueryResult
{
	NEARESTHALFEDGE_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	NEARESTHALFEDGE_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	NEARESTHALFEDGE_HALFEDGE_NOT_FOUND,                      ///< Indicates that a NavMesh border was not found within the query's bounding box. 
	NEARESTHALFEDGE_DONE_LACK_OF_WORKING_MEMORY,             ///< Indicates that insufficient working memory caused the query to stop. 

	NEARESTHALFEDGE_HALFEDGE_FOUND                           ///< Indicates that the query was completed successfully, and a NavMesh border was found. 
};

/// Base class for NearestBorderHalfEdgeFromPosQuery.
class BaseNearestBorderHalfEdgeFromPosQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeNearestBorderHalfEdgeFromPos; }
	virtual QueryType GetType() const { return TypeNearestBorderHalfEdgeFromPos; }
	virtual void BuildQueryBlob(BaseBlobHandler* blobHandler);
	virtual void InitFromQueryBlob(World* world, void* blob);
	virtual Ptr<BaseBlobHandler> CreateQueryBlobHandler() { return CreateStaticQueryBlobHandler(); }
	static Ptr<BaseBlobHandler> CreateStaticQueryBlobHandler();


	// ---------------------------------- Main API Functions ----------------------------------

	NearestBorderHalfEdgeFromPosQueryResult GetResult()                      const;
	const Vec3f&                            GetInputPos()                    const;
	const Vec3f&                            GetNearestPosOnHalfEdge()        const;
	const NavHalfEdgePtr&                   GetNearestHalfEdgePtrOnBorder()  const;
	const NavHalfEdgePtr&                   GetNextHalfEdgePtrAlongBorder()  const;
	const NavHalfEdgePtr&                   GetPrevHalfEdgePtrAlongBorder()  const;
	KyFloat32                               GetHorizontalTolerance()         const;
	KyFloat32                               GetSquareDistFromNearestBorder() const;
	const WorldIntegerPos&                  GetInputIntegerPos()             const;
	const WorldIntegerPos&                  GetNearestIntegerPosOnHalfEdge() const;
	const PositionSpatializationRange&      GetPositionSpatializationRange() const;


protected:
	BaseNearestBorderHalfEdgeFromPosQuery();
	virtual ~BaseNearestBorderHalfEdgeFromPosQuery() {}

	void BindToDatabase(Database* database);
	void Initialize(const Vec3f& inputPos);

	void SetHorizontalTolerance(KyFloat32 horizontalTolerance);
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);
	void SetInputIntegerPos(const WorldIntegerPos& inputIntegerPos);
	void SetResult(NearestBorderHalfEdgeFromPosQueryResult result);
	KyResult FindNavFloorsFromAltitudeRange(WorkingMemArray<NavFloor*>& filteredMetaFloorIndices, ActiveCell& activeCell);

protected:
	WorldIntegerPos m_inputIntegerPos;
	WorldIntegerPos m_nearestPointOnHalfEdgeIntegerPos;

	Vec3f m_inputPos3f; ///< The starting position for the query. 

	KyFloat32 m_horizontalTolerance; ///< The horizontal half-width (along both the X and Y axes) of the axis-aligned bounding box within which the search is performed. 
	PositionSpatializationRange m_positionSpatializationRange; ///< The Z extent from #m_inputPos3f of the axis-aligned bounding box within which the search is performed.

	/// Updated during processing to store the nearest triangle edge found in #m_inputPos3f. If no NavMesh border is found,
	/// this NavHalfEdgePtr will be invalid. 
	NavHalfEdgePtr m_nearestHalfEdgeOnBorder;

	/// Updated during processing to store the next triangle edge along the NavMesh border after #m_nearestHalfEdgeOnBorder. If no 
	/// NavMesh border is found, this NavHalfEdgePtr will be invalid. 
	NavHalfEdgePtr m_nextHalfEdgeOnBorder;

	/// Updated during processing to store the previous triangle edge along the NavMesh border before #m_nearestHalfEdgeOnBorder. If no 
	/// NavMesh border is found, this NavHalfEdgePtr will be invalid. 
	NavHalfEdgePtr m_prevHalfEdgeOnBorder;

	Vec3f m_nearestPosOnHalfEdge; ///< Updated during processing to indicate the point on #m_nearestHalfEdgeOnBorder that is the nearest to #m_inputPos3f. 

	KyFloat32 m_squareDistFromHalfEdge; ///< Updated during processing to indicate the square distance between #m_inputPos3f and #m_nearestPosOnHalfEdge. 

	NearestBorderHalfEdgeFromPosQueryResult m_result; ///< Updated during processing to indicate the result of the query. 
};


}

#include "gwnavruntime/queries/utils/basenearestborderhalfedgefromposquery.inl"

#endif //Navigation_BaseNearestBorderHalfEdgeFromPosQuery_H

