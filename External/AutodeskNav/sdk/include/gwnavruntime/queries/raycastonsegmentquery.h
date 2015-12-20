/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_RayCastOnSegmentQuery_H
#define Navigation_RayCastOnSegmentQuery_H


#include "gwnavruntime/queries/utils/baseraycastonsegmentquery.h"
#include "gwnavruntime/queries/trianglefromposquery.h"
#include "gwnavruntime/queries/utils/baseraycastquery.h"
#include "gwnavruntime/querysystem/workingmemcontainers/workingmemarray.h"
#include "gwnavruntime/navmesh/intersections.h"


namespace Kaim
{

/// The RayCastOnSegmentQuery class tests whether a ray (with a width of 0) can be cast from a starting point (#m_startPos3f)
/// along an edge going from #m_segmentStartPos3f to #m_segmentEndPos3f, without :
/// -	a collision with the outside border of the NavMesh.
/// -	crossing a forbidden NavTag (a triangle whose NavTag is considered as non-traversable by the traverseLogic).
/// If a collision is found, the point of collision is recorded (#m_collisionPos3f). If no collision is found, the ray stops 
/// at a maximum distance from the start point (#m_maxDist).
/// In either case, the last valid position along the ray is recorded in #m_arrivalPos3f.
template<class TraverseLogic>
class RayCastOnSegmentQuery : public BaseRayCastOnSegmentQuery
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	RayCastOnSegmentQuery();
	virtual ~RayCastOnSegmentQuery() {}

	// ---------------------------------- Functions to set up the query ----------------------------------

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);

	/// Sets an  instance of this class with minimum input requirements.
	/// Clears all the output, and let all other parameters unchanged. They
	/// may be changed by calling the corresponding write accessors.
	/// \param startPos            Sets the value of #m_startPos3f. 
	/// \param startTrianglePtr    Sets the value of #m_startTrianglePtr. 
	/// \param segmentStartPos     Sets the value of #m_segmentStartPos3f. 
	/// \param segmentEndPos       Sets the value of #m_segmentEndPos3f. 
	/// \param maxDist             Sets the value of #m_maxDist. 
	void Initialize(const Vec3f& startPos, const NavTrianglePtr& startTrianglePtr, const Vec3f& segmentStartPos, const Vec3f& segmentEndPos, KyFloat32 maxDist);

	// ---------------------------------- write accessors for query inputs ----------------------------------

	/// Write accessor for #m_queryDynamicOutput.
	/// Call this method after Initialize(), during which #m_queryDynamicOutput is set to KY_NULL.
	void SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput);

	// ---------------------------------- write accessors for query parameters ----------------------------------

	/// Write accessor for #m_positionSpatializationRange.
	/// Call this method after BindToDatabase(), during which #m_positionSpatializationRange is set to its default value. 
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);

	/// Write accessor for #m_dynamicOutputMode.
	/// Call this method after BindToDatabase during which member #m_dynamicOutputMode is set to its default value. 
	void SetDynamicOutputMode(DynamicOutputMode savingMode);

	// ---------------------------------- Query framework functions ----------------------------------

	virtual void Advance(WorkingMemory* workingMemory);

	/// Performs the query.
	/// \param workingMemory    Sandbox memory used to perform queries. If let to KY_NULL,
	///                          the #m_database internal WorkingMemory is used.
	void PerformQuery(WorkingMemory* workingMemory = KY_NULL);

	// ---------------------------------- Accessors ----------------------------------

	RayCastOnSegmentQueryResult GetResult()             const;
	const Vec3f&                GetStartPos()           const;
	const Vec3f&                GetSegmentStartPos()    const;
	const Vec3f&                GetSegmentEndPos()      const;
	const Vec3f&                GetArrivalPos()         const;
	const NavTrianglePtr&       GetStartTrianglePtr()   const;
	const NavTrianglePtr&       GetArrivalTrianglePtr() const;
	KyFloat32                   GetMaxDist()            const;
	DynamicOutputMode           GetDynamicOutputMode()  const;
	QueryDynamicOutput*         GetQueryDynamicOutput() const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

public : // internal
	void PerformQueryWithInputCoordPos(WorkingMemory* workingMemory);
	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);
	void SetSegmentStartIntegerPos(const WorldIntegerPos& segmentStartIntegerPos);
	void SetSegmentEndIntegerPos(const WorldIntegerPos& segmentEndIntegerPos);
	const WorldIntegerPos& GetStartIntegerPos() const;
	const WorldIntegerPos& GetArrivalIntegerPos() const;
	const WorldIntegerPos& GetSegmentStartIntegerPos() const;
	const WorldIntegerPos& GetSegmentEndIntegerPos() const;

private:
	void RayCast_BorderHit(QueryUtils& queryUtils, const NavHalfEdgeRawPtr& halfEdgeIdRawPtrHit, const CoordPos64& destCoordPos64,
		RawNavTagSubSegment& navTagSubSegment, WorkingMemArray<NavTriangleRawPtr>& crossedTriangles, WorkingMemArray<RawNavTagSubSegment>& navTagSubSegments);
};

}

#include "gwnavruntime/queries/raycastonsegmentquery.inl"



#endif //Navigation_RayCastOnSegmentQuery_H

