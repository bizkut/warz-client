/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_RayCastQuery_H
#define Navigation_RayCastQuery_H


#include "gwnavruntime/queries/utils/baseraycastquery.h"
#include "gwnavruntime/querysystem/workingmemcontainers/workingmemarray.h"
#include "gwnavruntime/navmesh/intersections.h"
#include "gwnavruntime/database/database.h"

namespace Kaim
{

class QueryUtils;
class NavHalfEdgeRawPtr;
class RawNavTagSubSegment;
template <class T> class WorkingMemArray;


/// The RayCastQuery class tests whether a ray (with a width of 0) can be cast from a starting point (#m_startPos3f)
/// in a specified direction (#maxMove2D) without:
/// -	a collision with the outside border of the NavMesh.
/// -	crossing a forbidden NavTag (a triangle whose NavTag is considered as non-traversable by the traverseLogic).
/// If a collision is found, the point of collision is recorded (#m_collisionPos3f). If no collision is found, the ray stops 
/// at the maximum distance from the start point.
/// In either case, the last valid position along the ray is recorded in #m_arrivalPos3f.
template<class TraverseLogic>
class RayCastQuery : public BaseRayCastQuery
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	RayCastQuery();
	virtual ~RayCastQuery() {}

	// ---------------------------------- Functions to set up the query ----------------------------------

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);

	/// Sets an  instance of this class with minimum input requirements.
	/// Clears all the output, and let all other parameters unchanged. They
	/// may be changed by calling the corresponding write accessors.
	/// \param startPos           Sets the value of #m_startPos3f. 
	/// \param maxMove2D          Sets the value of #m_maxMove2D.
	void Initialize(const Vec3f& startPos, const Vec2f& maxMove2D);

	// ---------------------------------- write accessors for query inputs ----------------------------------

	/// The NavMesh triangle that corresponds to #m_startPos3f. If a valid NavTrianglePtr is specified, this value will be used.
	/// If no value is specified, it will be automatically computed during the query. 
	/// Call this method after Initialize(), during which it is cleared. 
	void SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr);

	/// Write accessor for #m_queryDynamicOutput.
	/// Call this method after Initialize(), during which #m_queryDynamicOutput is set to KY_NULL.
	void SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput);

	// ---------------------------------- write accessors for query parameters ----------------------------------

	/// Write accessor for #m_positionSpatializationRange.
	/// Call this method after BindToDatabase(), during which #m_positionSpatializationRange is set to its default value. 
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);

	/// Write accessor for #m_dynamicOutputMode.
	/// Call this method after BindToDatabase(), during which #m_dynamicOutputMode is set to its default value. 
	void SetDynamicOutputMode(DynamicOutputMode savingMode);

	// ---------------------------------- Query framework functions ----------------------------------

	virtual void Advance(WorkingMemory* workingMemory);

	/// Performs the query.
	/// \param workingMemory    Sandbox memory used to perform queries. If let to KY_NULL,
	///                          the #m_database internal WorkingMemory is used.
	void PerformQuery(WorkingMemory* workingMemory = KY_NULL);

	// ---------------------------------- Accessors ----------------------------------

	RayCastQueryResult    GetResult()                  const;
	const Vec3f&          GetStartPos()                const;
	const Vec3f&          GetCollisionPos()            const;
	const Vec3f&          GetArrivalPos()              const;
	const Vec2f&          GetMaxMove2D()               const;
	const NavTrianglePtr& GetStartTrianglePtr()        const;
	const NavTrianglePtr& GetArrivalTrianglePtr()      const;
	const NavHalfEdgePtr& GetCollisionNavHalfEdgePtr() const;
	DynamicOutputMode     GetDynamicOutputMode()       const;
	QueryDynamicOutput*   GetQueryDynamicOutput()      const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

public : // internal
	void PerformQueryWithInputCoordPos(WorkingMemory* workingMemory);
	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);
	const WorldIntegerPos& GetStartIntegerPos() const;
	const WorldIntegerPos& GetArrivalIntegerPos() const;

private:
	void RayCast_BorderHit(QueryUtils& queryUtils, const NavHalfEdgeRawPtr& halfEdgeRawPtrHit, const CoordPos64& destCoordPos,
		RawNavTagSubSegment& navTagSubSegment, WorkingMemArray<NavTriangleRawPtr>& crossedTriangles, WorkingMemArray<RawNavTagSubSegment>& navTagSubSegments);
};

}

#include "gwnavruntime/queries/raycastquery.inl"

#endif //Navigation_RayCastQuery_H

