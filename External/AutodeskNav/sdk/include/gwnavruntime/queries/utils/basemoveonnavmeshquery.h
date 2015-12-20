/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: LAPA - secondary contact: JUBA
#ifndef Navigation_BaseMoveOnNavMeshQuery_H
#define Navigation_BaseMoveOnNavMeshQuery_H

#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"
#include "gwnavruntime/navmesh/worldintegerpos.h"

namespace Kaim
{

class Path;

/// Enumerates the possible results of a MoveOnNavMeshQuery.
enum MoveOnNavMeshQueryResult
{
	MOVEONNAVMESH_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates that the query has not yet been initialized. 
	MOVEONNAVMESH_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates that the query has not yet been launched. 

	MOVEONNAVMESH_DONE_START_OUTSIDE,                      ///< Indicates that a NavMesh triangle could not be found for the starting point. 
	MOVEONNAVMESH_DONE_START_NAVTAG_FORBIDDEN,             ///< Indicates that a NavMesh triangle has been found for the starting point, but its NavTag is not considered navigable.
	MOVEONNAVMESH_DONE_START_LINKONEDGE_ERROR,             ///< Indicates that the start position is near an edge but should not be linked to it. This is usualy due to too high values for DistanceToBoundary.
	MOVEONNAVMESH_DONE_LACK_OF_WORKING_MEMORY,             ///< Indicates that insufficient memory was available to store crossed triangles or sub-sections, and the query failed as a result. 
	MOVEONNAVMESH_DONE_UNKNOWN_ERROR,                      ///< Indicates that an unknown error occurred during the query processing. 

	MOVEONNAVMESH_DONE_ARRIVALPOS_FOUND_NO_COLLISION,      ///< Indicates an arrival point was successfully calculated without a collision with the NavMesh border.
	MOVEONNAVMESH_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION,   ///< Indicates a collision point was found, and a final arrival position was successfully calculated.
	MOVEONNAVMESH_DONE_INTERMEDIARYPOS_FOUND               ///< Indicates a collision point was found, an intermediary position has be found but progression has been blocked at this point.
};

class BaseMoveOnNavMeshQuery : public IAtomicQuery
{
public:
	static  QueryType GetStaticType() { return TypeMoveOnNavMesh; }
	virtual QueryType GetType() const { return TypeMoveOnNavMesh; }

	// ---------------------------------- Main API Functions ----------------------------------

	MoveOnNavMeshQueryResult GetResult()             const;
	const Vec3f&             GetStartPos()           const;
	const Vec3f&             GetArrivalPos()         const;
	const Vec2f&             GetNormalizedDir2d()    const;
	const NavTrianglePtr&    GetStartTrianglePtr()   const;
	KyFloat32                GetDistanceToBoundary() const;
	KyFloat32                GetDistance()           const;
	KyFloat32                GetAltitudeTolerance()  const;
	const WorldIntegerPos&   GetStartIntegerPos()    const;

public: //internal
	void BindToDatabase(Database* database);
	void Initialize(const Vec3f& startPos, const Vec2f& normalizedDir2d, KyFloat32 maxDist);
	void SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr);
	void SetDistanceToBoundary(KyFloat32 distanceToBoundary);
	void SetStartIntegerPos(const WorldIntegerPos& startIntegerPos);

protected:
	BaseMoveOnNavMeshQuery(); 
	virtual ~BaseMoveOnNavMeshQuery() {}
	void SetResult(MoveOnNavMeshQueryResult result);

protected:
	WorldIntegerPos m_startIntegerPos;

	Vec3f m_startPos3f;      ///< The starting point of the line segment being tested.
	Vec2f m_normalizedDir2d; ///< A normalized vector representing the direction the ray is cast.

	NavTrianglePtr m_startTrianglePtr; ///< The NavMesh triangle corresponding to #m_startPos3f. It can be an input, or it can be retrieved as an output of the query.

	/// The minimal distance to NavMesh boundary the resulting position should be. When the movement hits a boundary, we move back
	/// the final position so that it is at #m_distanceToBoundary meters from this boundary.
	KyFloat32 m_distanceToBoundary;

	/// Determines the maximum distance the disk will travel.
	/// If this distance is too far from the starting point (approximately 32 times the length of a NavData cell), running
	/// the query can result in an integer overflow during calculation.
	KyFloat32 m_moveDistance;

	/// Updated during processing to indicate the final position of the ray at the end of the cast.
	/// -  If no collision was detected, the result is the triangle arrived at by traversing the NavMesh in the direction of #m_normalizedDir2d.
	/// -  If a collision is detected, the result is derived from the collision point by backtracking along the orientation
	///     of #m_normalizedDir2d. This ensures #m_arrivalPos3f will be inside the boundaries of the NavMesh.
	Vec3f m_arrivalPos3f;

	MoveOnNavMeshQueryResult m_result; ///< Updated during processing to indicate the result of the query.

public:
	bool m_visualDebugActivated; ///< When toggle on, the query will send visual debug information.
};



KY_INLINE void BaseMoveOnNavMeshQuery::SetStartIntegerPos(const WorldIntegerPos& startIntegerPos)  { m_startIntegerPos    = startIntegerPos;    }
KY_INLINE void BaseMoveOnNavMeshQuery::SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr) { m_startTrianglePtr   = startTrianglePtr;   }
KY_INLINE void BaseMoveOnNavMeshQuery::SetDistanceToBoundary(KyFloat32 distanceToBoundary)         { m_distanceToBoundary = distanceToBoundary; }
KY_INLINE void BaseMoveOnNavMeshQuery::SetResult(MoveOnNavMeshQueryResult result)                  { m_result             = result;             }

KY_INLINE MoveOnNavMeshQueryResult BaseMoveOnNavMeshQuery::GetResult()             const { return m_result;             }
KY_INLINE const Vec3f&             BaseMoveOnNavMeshQuery::GetStartPos()           const { return m_startPos3f;         }
KY_INLINE const Vec3f&             BaseMoveOnNavMeshQuery::GetArrivalPos()         const { return m_arrivalPos3f;       }
KY_INLINE const Vec2f&             BaseMoveOnNavMeshQuery::GetNormalizedDir2d()    const { return m_normalizedDir2d;    }
KY_INLINE const NavTrianglePtr&    BaseMoveOnNavMeshQuery::GetStartTrianglePtr()   const { return m_startTrianglePtr;   }
KY_INLINE KyFloat32                BaseMoveOnNavMeshQuery::GetDistanceToBoundary() const { return m_distanceToBoundary; }
KY_INLINE KyFloat32                BaseMoveOnNavMeshQuery::GetDistance()           const { return m_moveDistance;       }
KY_INLINE const WorldIntegerPos&   BaseMoveOnNavMeshQuery::GetStartIntegerPos()    const { return m_startIntegerPos;    }
}

#endif //Navigation_BaseMoveOnNavMeshQuery_H

