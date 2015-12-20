/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_SegmentCastQuery_H
#define Navigation_SegmentCastQuery_H

#include "gwnavruntime/queries/utils/basesegmentcastquery.h"
#include "gwnavruntime/queries/raycastquery.h"
#include "gwnavruntime/queries/utils/rectangleintersector.h"
#include "gwnavruntime/queries/utils/bestfirstsearchedgecollisionvisitor.h"
#include "gwnavruntime/queries/utils/bestfirstsearchtraversal.h"

namespace Kaim
{

/// The SegmentCastQuery class casts a line segment in a given direction, and records any collisions it detects along the way.
/// Set up the segment by assigning a starting point (#m_startPos3f), half-width (#m_radius), and the direction
/// in which it will be cast (#m_normalizedDir2d). If the segment collides with a border of the NavMesh or crosses any forbidden
/// NavTag, the collision point is recorded (#m_collisionPos3f). The segment stops if it reaches the maximum distance without
/// a collision (#m_maxDist). The final valid position-at a collision point, or at maximum distance-is recorded in #m_arrivalPos3f.
/// The line segment is considered to be orthogonal to #m_normalizedDir2d:
/// <pre>
///               +---->-------->--------->---------->--------------+
///               |                                                 |
///               |                                                 | => m_radius
///               |                                                 |
///               |                                                 | 
/// m_startPos3f  X                                                 X  => m_arrivalPos3f
///               |                                                 |
///               |                                                 |
///               |                                                 | => m_radius
///               |                                                 |
///               +---->-------->--------->---------->--------------+  
///               <- - - - - - - - - - m_maxDist - - - - - - - - - ->
/// </pre>
template <class TraverseLogic>
class SegmentCastQuery : public BaseSegmentCastQuery
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	SegmentCastQuery();
	virtual ~SegmentCastQuery() {}

	// ---------------------------------- Functions to set up the query ----------------------------------

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);

	/// resets an initialized instance of this class with minimum input requirements.
	/// Clears all the output, and let all other parameters unchanged. They
	/// may be changed by calling the corresponding write accessors.
	/// \param startPos           Sets the value of #m_startPos3f. 
	/// \param radius             Sets the value of #m_radius.
	/// \param normalizedDir2d    Sets the value of #m_normalizedDir2d.
	/// \param maxDist            Sets the value of #m_maxDist. 
	void Initialize(const Vec3f& startPos, KyFloat32 radius, const Vec2f& normalizedDir2d, KyFloat32 maxDist);

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

	/// Write accessor for #m_safetyDist.
	/// Call this method after BindToDatabase(), during which #m_safetyDist is set to its default value. 
	void SetSafetyDist(KyFloat32 safetyDist);

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

public : // internal
	void PerformQueryWithInputCoordPos(WorkingMemory* workingMemory);
	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);
	const WorldIntegerPos& GetStartIntegerPos() const;
	const WorldIntegerPos& GetArrivalIntegerPos() const;
};

}

#include "gwnavruntime/queries/segmentcastquery.inl"

#endif //Navigation_SegmentCastQuery_H

