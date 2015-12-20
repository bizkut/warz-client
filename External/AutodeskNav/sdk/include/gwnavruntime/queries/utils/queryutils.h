/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_QueryUtils_H
#define Navigation_QueryUtils_H

#include "gwnavruntime/queries/querydynamicoutput.h"
#include "gwnavruntime/queries/trianglefromposandtriangleseedquery.h"
#include "gwnavruntime/navmesh/intersections.h"
#include "gwnavruntime/database/database.h"

namespace Kaim
{

class Box2f;
class NavTrianglePtr;
class NavTriangleRawPtr;
class NavHalfEdgePtr;
class NavHalfEdgeRawPtr;
class TriangleFromPosAndTriangleSeedQuery;
class ThinCapsuleWithExtentOffset;

/// This class is an helper used internally by the Queries to factorize Code that is used in many Queries.
class QueryUtils
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	QueryUtils() : m_database(KY_NULL), m_workingMemory(KY_NULL), m_traverseLogicUserData(KY_NULL), m_performQueryStat(PerformQueryStat_Unspecified) {}
	QueryUtils(Database* database, WorkingMemory* workingMemory, void* traverseLogicUserData);
	void Init(Database* database, WorkingMemory* workingMemory, void* traverseLogicUserData);
	void SetPerformQueryStat(PerformQueryStat performQueryStat);

	// ------------------------------ Accessors -----------------------------------
	Database*      GetDatabase()              { return m_database;              }
	WorkingMemory* GetWorkingMemory()         { return m_workingMemory;         }
	void*          GetTraverseLogicUserData() { return m_traverseLogicUserData; }

	// ------------------------------ Location functions -----------------------------------

	/// Checks the validity of the specified triangle. If the triangle is not valid, this method tries to retrieve a 
	/// valid triangle ID from the specified position. If a valid triangle is found, it updatesnavTrianglePtr.
	/// \param [in,out] navTrianglePtr		The triangle to test. If the triangle is invalid and a valid triangle is
	/// 									found, this parameter will be updated with the ID of the new triangle.
	/// \param pos							The position used to retrieve a new triangle, if needed.
	/// \param positionSpatializationRange	The maximum difference in altitude between pos and the returned triangle.
	/// \return true if the input triangle is valid or if navTrianglePtr was updated, otherwise false.
	bool FindTriangleFromPositionIfNotValid(NavTrianglePtr& navTrianglePtr, const Vec3f& pos, const PositionSpatializationRange& positionSpatializationRange);

	/// Checks the validity of the specified triangle. If the triangle is not valid, this method tries to retrieve a 
	/// valid triangle ID from the specified position. If a valid triangle is found, it updatesnavTrianglePtr.
	/// \param [in,out] navTrianglePtr		The triangle to test. If the triangle is invalid and a valid triangle is
	/// 									found, this parameter will be updated with the ID of the new triangle.
	/// \param pos							The position used to retrieve a new triangle, if needed.
	/// \param positionSpatializationRange	The maximum difference in altitude between pos and the returned triangle.
	/// \param [out] outputIntegerPos		Stores the integer position of the returned triangle, or of pos if the
	/// 									original triangle was valid.
	/// \return true if the input triangle is valid or if navTrianglePtr was updated, otherwise false.
	bool FindTriangleFromPositionIfNotValid(NavTrianglePtr& navTrianglePtr, const Vec3f& pos, const PositionSpatializationRange& positionSpatializationRange, WorldIntegerPos& outputIntegerPos);

	/// Checks the validity of the specified triangle. If the triangle is not valid, this method tries to retrieve a 
	/// valid triangle ID from the specified position. If a valid triangle is found, it updatesnavTrianglePtr.
	/// \param [in,out] navTrianglePtr		The triangle to test. If the triangle is invalid and a valid triangle is
	/// 									found, this parameter will be updated with the ID of the new triangle.
	/// \param pos							The position used to retrieve a new triangle, if needed. Only the altitude
	/// 									of this position is used.
	/// \param [in] integerPos				Provides the (x,y) coordinates of the desired position.
	/// \param positionSpatializationRange	The maximum difference in altitude between pos and the returned triangle.
	/// \return true if the input triangle is valid or if navTrianglePtr was updated, otherwise false.
	bool FindTriangleFromPositionIfNotValid(NavTrianglePtr& navTrianglePtr, const Vec3f& pos, const WorldIntegerPos& integerPos, const PositionSpatializationRange& positionSpatializationRange);


	/// Check if the specified half edge intersects the specified thin capsule.
	/// \param halfEdge					The half edge to check.
	/// \param capsuleStartCoordPos		The capsule start position expressed in CoordPos64.
	/// \param capsuleEndCoordPos		The capsule end position expressed in CoordPos64.
	/// \param capsuleSquareRadius		The capsule square radius, expressed in integer.
	/// \param recoveryDistance			The distance the start position is moved forward or the end position
	///									is moved backward when they are too close from the halfEdge.
	/// \pre The radius must be less than PixelSize / 3.
	template<class TraverseLogic>
	bool IsHalfEdgeCompatibleWithThinCapsule(const NavHalfEdgeRawPtr& halfEdge,	ThinCapsuleWithExtentOffset& capsuleParams);


public : // internal
	void GetNextHalfEdgeAroundStartVertex(const NavHalfEdgeRawPtr& currentHalfEdge, NavHalfEdgeRawPtr& nextHalfEdge); // Provides the next HalEdge (CW) starting from the provided HalEdge start vertex.
	void GetPrevHalfEdgeAroundDestVertex(const NavHalfEdgeRawPtr& currentHalfEdge, NavHalfEdgeRawPtr& prevHalfEdge); // Provides the previous HalEdge (CCW) ending at the provided HalEdge end vertex.

	template<class TraverseLogic>
	KyResult FindValidPositionFromIntegerIntersection(const Vec3f& collisionPos3f, const NavHalfEdgePtr& halfEdgePtrHit,
		WorldIntegerPos& arrivalIntegerPos, NavTrianglePtr& arrivalTrianglePtr);

private :
	class CheckIfCurrentIntegerPositionIsValidParam
	{
	public:
		CoordPos64 m_start;
		CoordPos64 m_end;
		CoordPos64 m_third;
		CoordPos64 m_startToEnd;
		CoordPos64 m_endToThird;
		CoordPos64 m_thirdTostart;
		WorldIntegerPos m_currentArrivalIntegerPos;
	};

	template<class TraverseLogic>
	KyResult  CheckIfCurrentIntegerPositionIsValid(const CheckIfCurrentIntegerPositionIsValidParam& params,
		TriangleFromPosAndTriangleSeedQuery& triangleFromPosAndTriangleSeedQuery, WorldIntegerPos& arrivalIntegerPos, NavTrianglePtr& arrivalTrianglePtr);

public:
	Database* m_database; ///< The Database taken into account by queries made through this object. Do not modify.
	WorkingMemory* m_workingMemory; ///< Contains memory buffers used for performing queries and storing results. For internal use. Do not modify.
	void* m_traverseLogicUserData;
	PerformQueryStat m_performQueryStat;
};

/// Particuliar class used to represent the ThinCapsule for QueryUtils::IsHalfEdgeCompatibleWithThinCapsule().
/// GetOffset() lazily computes the offset by which the extents of the capsule can be reduced.
class ThinCapsuleWithExtentOffset
{
public:
	ThinCapsuleWithExtentOffset(const CoordPos64& capsuleStartCoordPos, const CoordPos64& capsuleEndCoordPos, 
		KyUInt64 capsuleSquareRadius, KyUInt32 recoveryDistance)
	{
		m_capsuleStartCoordPos = capsuleStartCoordPos;
		m_capsuleEndCoordPos = capsuleEndCoordPos;
		m_capsuleSquareRadius = capsuleSquareRadius;
		m_recoveryDistance = recoveryDistance;

		CoordPos64 capsule = m_capsuleEndCoordPos - m_capsuleStartCoordPos;
		m_capsuleSquareLength = capsule.GetSquareLength();
		// m_offset will be computed lazily (only if needed)
		m_offset.x = KyInt64MAXVAL;
	}

	CoordPos64 GetOffset(); /// lazily compute the offset and returns it

public:
	CoordPos64 m_capsuleStartCoordPos;
	CoordPos64 m_capsuleEndCoordPos;
	KyUInt64 m_capsuleSquareRadius;
	KyUInt32 m_recoveryDistance;

	KyUInt64 m_capsuleSquareLength;

private:
	CoordPos64 m_offset;
};




}

#include "gwnavruntime/queries/utils/queryutils.inl"

#endif //Navigation_QueryUtils_H

