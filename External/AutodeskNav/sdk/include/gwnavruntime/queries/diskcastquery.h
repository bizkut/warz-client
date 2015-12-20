/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_DiskCastQuery_H
#define Navigation_DiskCastQuery_H

#include "gwnavruntime/queries/utils/basediskcastquery.h"
#include "gwnavruntime/queries/raycastquery.h"
#include "gwnavruntime/queries/utils/capsuleintersector.h"
#include "gwnavruntime/queries/utils/crossedsectioncapsuleintersector.h"
#include "gwnavruntime/queries/utils/bestfirstsearchedgecollisionvisitor.h"
#include "gwnavruntime/queries/utils/bestfirstsearchtraversal.h"

namespace Kaim
{

class NavTriangleRawPtr;

/// The DiskCastQuery class tests whether a disk with a specified radius (#m_radius) can be cast in a specified
/// orientation (#m_normalizedDir2d) from a starting point without:
/// - a collision with the outside border of the NavMesh,
/// - crossing a forbidden NavTag (a triangle whose NavTag is considered as non-traversable by the traverseLogic).
/// If a collision is found, the point of collision is recorded (#m_collisionPos3f). If no collision is found, the disk stops 
/// at the maximum distance from the start point (#m_maxDist).
/// In either case, the last valid position of the disk is recorded in #m_arrivalPos3f.
template<class TraverseLogic>
class DiskCastQuery : public BaseDiskCastQuery
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	DiskCastQuery();
	virtual ~DiskCastQuery() {}


	// ---------------------------------- Functions to set up the query ----------------------------------

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);

	/// Sets an  instance of this class with minimum input requirements.
	/// Clears all the output, and let all other parameters unchanged. They
	/// may be changed by calling the corresponding write accessors.
	/// \param startPos           Sets the value of #m_startPos3f. 
	/// \param radius             Sets the value of #m_radius.
	/// \param normalizedDir2d    Sets the value of #m_normalizedDir2d.
	/// \param maxDist            Sets the value of #m_maxDist. 
	void Initialize(const Vec3f& startPos, KyFloat32 radius, const Vec2f& normalizedDir2d, KyFloat32 maxDist);

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
	/// Call this method after BindToDatabase(), during which #m_positionSpatializationRange set is to its default value.
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);

	/// Write accessor for #m_safetyDist.
	/// Call this method after BindToDatabase(), during which #m_safetyDist is set to its default value. 
	void SetSafetyDist(KyFloat32 safetyDist);

	/// Write accessor for #m_queryType.
	/// Call this method after BindToDatabase(), during which #m_queryType is set to its default value. 
	void SetQueryType(DiskCastQueryType queryType);

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

	DiskCastQueryResult   GetResult()                  const;
	const Vec3f&          GetStartPos()                const;
	const Vec3f&          GetCollisionPos()            const;
	const Vec3f&          GetArrivalPos()              const;
	const Vec2f&          GetNormalizedDir2d()         const;
	const NavTrianglePtr& GetStartTrianglePtr()        const;
	const NavTrianglePtr& GetArrivalTrianglePtr()      const;
	const NavHalfEdgePtr& GetCollisionNavHalfEdgePtr() const;
	KyFloat32             GetRadius()                  const;
	KyFloat32             GetMaxDist()                 const;
	KyFloat32             GetSafetyDist()              const;
	DiskCastQueryType     GetQueryType()               const;
	DynamicOutputMode     GetDynamicOutputMode()       const;
	QueryDynamicOutput*   GetQueryDynamicOutput()      const;
	const PositionSpatializationRange& GetPositionSpatializationRange() const;

public : // internal
	void PerformQueryWithInputCoordPos(WorkingMemory* workingMemory);
	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);
	const WorldIntegerPos& GetStartIntegerPos() const;
	const WorldIntegerPos& GetArrivalIntegerPos() const;

private:
	void ForceDiskCast(WorkingMemory* workingMemory, const CellBox& cellBox, const NavTriangleRawPtr& startTriangleRawPtr, WorkingMemArray<NavTriangleRawPtr>& visitedTriangles);
	KyResult RayCastQueryFromStartOnTheRigth(WorkingMemory* workingMemory, RayCastQuery<TraverseLogic>& rayCastQuery,
		RayCastQueryResult& queryOnRightResult, KyFloat32& borderDistOnRight);
	KyResult RayCastQueryFromStartOnTheLeft(WorkingMemory* workingMemory, RayCastQuery<TraverseLogic>& rayCastQuery,
		RayCastQueryResult& queryOnLeftResult, KyFloat32& borderDistOnLeft);
	KyResult FindDistReachedFromCollisionPos(const NavHalfEdgeRawPtr& intersectedHalfEdgeRaw, const Vec3f& collisionPos, KyFloat32& distReached);
};

}

#include "gwnavruntime/queries/diskcastquery.inl"



#endif //Navigation_DiskCastQuery_H

