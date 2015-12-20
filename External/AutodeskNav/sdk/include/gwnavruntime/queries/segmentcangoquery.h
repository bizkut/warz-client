/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_SegmentCanGoQuery_H
#define Navigation_SegmentCanGoQuery_H

#include "gwnavruntime/queries/utils/basesegmentcangoquery.h"
#include "gwnavruntime/queries/blobs/segmentcangoqueryblob.h"
#include "gwnavruntime/queries/utils/rectangleintersector.h"
#include "gwnavruntime/queries/utils/breadthfirstsearchedgecollisionvisitor.h"
#include "gwnavruntime/queries/utils/breadthfirstsearchtraversal.h"
#include "gwnavruntime/queries/raycangoquery.h"


namespace Kaim
{


/// The SegmentCanGoQuery class tests whether a line segment-with a specified half-width (#m_radius)-can move along a
/// straight line segment from a starting point to an ending point  without:
/// -	a collision with the NavMesh border.
/// -	crossing a forbidden NavTag.
/// The line segment is considered to be orthogonal to the direction between #m_startPos3f and #m_destPos3f, as follows:
/// <pre>
/// m_startA      +---->-------->--------->---------->--------------+
///               |                                                 |
///               |                                                 | => m_radius
///               |                                                 |
///               |                                                 |
/// m_startPos3f  X                                                 X  => m_destPos3f
///               |                                                 |
///               |                                                 |
///               |                                                 | => m_radius
///               |                                                 |
/// m_startB      +---->-------->--------->---------->--------------+
/// </pre>
template <class TraverseLogic>
class SegmentCanGoQuery : public BaseSegmentCanGoQuery
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	SegmentCanGoQuery();
	virtual ~SegmentCanGoQuery() {}

	// ---------------------------------- Functions to set up the query ----------------------------------

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);

	/// Sets an  instance of this class with minimum input requirements.
	/// Clears all the output, and let all other parameters unchanged. They
	/// may be changed by calling the corresponding write accessors.
	/// \param startPos    Sets the value of #m_startPos3f.
	/// \param destPos     Sets the value of #m_destPos3f.
	/// \param radius      Sets the value of #m_radius. 
	void Initialize(const Vec3f& startPos, const Vec3f& destPos, KyFloat32 radius);

	// ---------------------------------- write accessors for query inputs ----------------------------------

	/// Set The NavMesh triangle that corresponds to #m_startPos3f. If a valid NavTrianglePtr is specified, this value will be used.
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

	SegmentCanGoQueryResult GetResult()             const;
	const Vec3f&            GetStartPos()           const;
	const Vec3f&            GetDestPos()            const;
	const NavTrianglePtr&   GetStartTrianglePtr()   const;
	const NavTrianglePtr&   GetDestTrianglePtr()    const;
	KyFloat32               GetRadius()             const;
	DynamicOutputMode       GetDynamicOutputMode()  const;
	QueryDynamicOutput*     GetQueryDynamicOutput() const;

	const PositionSpatializationRange& GetPositionSpatializationRange() const;

public : // internal 
	void PerformQueryWithInputCoordPos(WorkingMemory* workingMemory);
	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);
	void SetDestIntegerPos(const WorldIntegerPos& destIntegerPos);
	const WorldIntegerPos& GetStartIntegerPos() const;
	const WorldIntegerPos& GetDestIntegerPos() const;
};

}

#include "gwnavruntime/queries/segmentcangoquery.inl"

#endif //Navigation_SegmentCanGoQuery_H

