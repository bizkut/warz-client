/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

namespace Kaim
{

//////////////////////////////////////////////////////////////////////////
//
// MoveOnNavMeshQuery
//
//////////////////////////////////////////////////////////////////////////
template <class TLogic>
KY_INLINE MoveOnNavMeshQuery<TLogic>::MoveOnNavMeshQuery() : BaseMoveOnNavMeshQuery() {}

template <class TLogic>
KY_INLINE void MoveOnNavMeshQuery<TLogic>::BindToDatabase(Database* database)
{
	BaseMoveOnNavMeshQuery::BindToDatabase(database);
}

template <class TLogic>
KY_INLINE void MoveOnNavMeshQuery<TLogic>::Initialize(const Vec3f& startPos, const Vec2f& normalizedDir2d, KyFloat32 maxDist)
{
	BaseMoveOnNavMeshQuery::Initialize(startPos, normalizedDir2d, maxDist);
}

template <class TLogic>
KY_INLINE void MoveOnNavMeshQuery<TLogic>::SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr) { BaseMoveOnNavMeshQuery::SetStartTrianglePtr(startTrianglePtr); }
template<class TLogic>
KY_INLINE void MoveOnNavMeshQuery<TLogic>::SetStartIntegerPos(const WorldIntegerPos& startIntegerPos)  { BaseMoveOnNavMeshQuery::SetStartIntegerPos(startIntegerPos);   }
template <class TLogic>
KY_INLINE void MoveOnNavMeshQuery<TLogic>::SetDistanceToBoundary(KyFloat32 distanceToBoundary) { BaseMoveOnNavMeshQuery::SetDistanceToBoundary(distanceToBoundary); }
template <class TLogic>
KY_INLINE void MoveOnNavMeshQuery<TLogic>::SetResult(MoveOnNavMeshQueryResult result)        { BaseMoveOnNavMeshQuery::SetResult(result); }
template <class TLogic>
KY_INLINE MoveOnNavMeshQueryResult MoveOnNavMeshQuery<TLogic>::GetResult()             const { return BaseMoveOnNavMeshQuery::GetResult();             }
template <class TLogic>
KY_INLINE const Vec3f&             MoveOnNavMeshQuery<TLogic>::GetStartPos()           const { return BaseMoveOnNavMeshQuery::GetStartPos();           }
template <class TLogic>
KY_INLINE const Vec3f&             MoveOnNavMeshQuery<TLogic>::GetArrivalPos()         const { return BaseMoveOnNavMeshQuery::GetArrivalPos();         }
template <class TLogic>
KY_INLINE const Vec2f&             MoveOnNavMeshQuery<TLogic>::GetNormalizedDir2d()    const { return BaseMoveOnNavMeshQuery::GetNormalizedDir2d();    }
template <class TLogic>
KY_INLINE const NavTrianglePtr&    MoveOnNavMeshQuery<TLogic>::GetStartTrianglePtr()   const { return BaseMoveOnNavMeshQuery::GetStartTrianglePtr();   }
template <class TLogic>
KY_INLINE KyFloat32                MoveOnNavMeshQuery<TLogic>::GetDistanceToBoundary() const { return BaseMoveOnNavMeshQuery::GetDistanceToBoundary(); }
template <class TLogic>
KY_INLINE KyFloat32                MoveOnNavMeshQuery<TLogic>::GetDistance()           const { return BaseMoveOnNavMeshQuery::GetDistance();           }
template<class TLogic>
KY_INLINE const WorldIntegerPos&   MoveOnNavMeshQuery<TLogic>::GetStartIntegerPos()    const { return BaseMoveOnNavMeshQuery::GetStartIntegerPos();    }

template <class TLogic>
KY_INLINE void MoveOnNavMeshQuery<TLogic>::Advance(WorkingMemory* workingMemory)
{
	PerformQuery(workingMemory);
	m_processStatus = QueryDone;
}


//////////////////////////////////////////////////////////////////////////
//
// Utility classes
//
//////////////////////////////////////////////////////////////////////////
enum RailDirection
{
	RailDirection_Undefined, // No direction should be defined.
	RailDirection_Away,      // the destination is away from the edge. This is used when the start is inside an edge vicinity and need to be moved up to the rail, but the move direction is going away from the rail.
	RailDirection_Direct,    // the rail is followed in HalfEdge direction
	RailDirection_Reverse    // the rail is followed in the direction opposite to HalfEdges
};

enum LinkToEdgeRailResult
{
	NoInteractingEdgeFound,
	LinkedToEdgeRail,
	ShouldNotLinkToNearestEdge
};

enum MoveOnRailResult
{
	QuitRail,
	SwitchRail,
	MovedOnRail,
	NotMoved
};

class EdgeGeometry
{
public:
	EdgeGeometry() {}
	EdgeGeometry(const NavHalfEdgeRawPtr& halfEdge) { ComputeFrom(halfEdge); }

	const Vec3f& GetStartPos() const { return m_edgeStart;   }
	const Vec3f& GetEndPos()   const { return m_edgeEnd;     }
	const Vec3f& GetEdgeVec()  const { return m_edge;        }
	const Vec2f& GetLeftDir()  const { return m_edgeLeftDir; }

	void ComputeFrom(const NavHalfEdgeRawPtr& halfEdge);
private:
	Vec3f m_edgeStart;
	Vec3f m_edgeEnd;
	Vec3f m_edge;
	Vec2f m_edgeLeftDir;
};

class RailExtremity
{
public:
	enum Type
	{
		SinglePoint,
		DoublePoint,
		Undefined
	};

	RailExtremity() : m_type(Undefined) {}

	const Vec3f& GetPosition()       const { return m_railExtremityPosition;  }
	const Vec3f& GetTransitionPos()  const { return m_railTransitionPosition; }
	KyFloat32    GetAbscissa()       const { return m_abscissa;               }
	bool         IsDoubleExtremity() const { return m_type == DoublePoint;    }

	void Compute(const EdgeGeometry& edgeGeom, const EdgeGeometry& prevEdgeGeom, const EdgeGeometry& nextEdgeGeom,
		KyFloat32 distanceToBoundary, const Vec3f& edgeExtremity);

private:
	void Set(const EdgeGeometry& edgeGeometry, const Vec3f& position);
	void Set(const EdgeGeometry& edgeGeometry, const Vec3f& position, const Vec3f& end);

	Vec3f m_railExtremityPosition;
	Vec3f m_railTransitionPosition;
	KyFloat32 m_abscissa;
	Type m_type;
};

class ScopedDisplaySuccessiveMoves
{
public:
	ScopedDisplaySuccessiveMoves(BaseMoveOnNavMeshQuery* query) : m_query(query) {}
	~ScopedDisplaySuccessiveMoves();
	void IncrementColor(VisualColor& color, KyUInt8 colorIncrement);

	KyArrayPOD<Vec3f> m_successivePositions;
	BaseMoveOnNavMeshQuery* m_query;
};


class PositionOnRail
{
public:
	PositionOnRail(void* traverseLogicUserData, World* world, bool visualDebugActivated);

	const EdgeGeometry& GetEdgeGeometry()   const { return m_edgeGeometry;   }
	const Vec3f&        GetPositionOnRail() const { return m_positionOnRail; }
	RailDirection       GetDirection()      const { return m_direction;      }

	template <class TLogic>
	bool Setup(const NavHalfEdgeRawPtr& navHalfEdge, KyFloat32 distanceToBoundary, const Vec3f& entry, const Vec2f& moveDir2d);

	template <class TLogic>
	bool GetNextRailHalfEdgeRawPtr(NavHalfEdgeRawPtr& nextRailHalfEdgeRawPtr, void* traverseLogicUserData);

	template <class TLogic>
	LinkToEdgeRailResult LinkToEdgeRail(const Vec3f& start, const Vec3f& dest, const Vec2f& moveDir2d,
		const WorkingMemArray<NavHalfEdgeRawPtr>& nearbyEdges, KyFloat32 distanceToBorder, KyFloat32& hangingDistance);

	bool SetupNoTemplate(const NavHalfEdgeRawPtr& navHalfEdge, const NavHalfEdgeRawPtr& prevHalfEdge, const NavHalfEdgeRawPtr& nextHalfEdge,
		KyFloat32 distanceToBoundary, const Vec3f& entry, const Vec2f& moveDir2d);

	MoveOnRailResult MoveOnRail(KyFloat32 distanceToMove, KyFloat32& distanceActuallyMoved);
private:
	template <class TLogic>
	LinkToEdgeRailResult LinkToEdgeRail_StartNearBorder(const Vec3f& start, const Vec3f& dest, const Vec2f& moveDir2d,
		const WorkingMemArray<NavHalfEdgeRawPtr>& nearbyEdges, KyFloat32 distanceToBorder, KyFloat32& hangingDistance);

	template <class TLogic>
	LinkToEdgeRailResult LinkToEdgeRail_StartFarFromAnyBorder(const Vec3f& start, const Vec3f& dest, const Vec2f& moveDir2d,
		const WorkingMemArray<NavHalfEdgeRawPtr>& nearbyEdges, KyFloat32 distanceToBorder, KyFloat32& hangingDistance);

	MoveOnRailResult MoveToExtremity(const RailExtremity& extremity, KyFloat32& distanceActuallyMoved);

	void ComputeNearestHalfEdge_StartNearBorder(const WorkingMemArray<NavHalfEdgeRawPtr>& nearbyEdges, const Vec3f& start, const Vec3f& dest,
		NavHalfEdgeRawPtr& nearestHalfEdgeFallback, NavHalfEdgeRawPtr& nearestHalfEdge, KyFloat32& minSqDistFallback,
		KyFloat32& minSqDist, Vec3f& nearestPosOnNearestEdgeFallback, Vec3f& nearestPosOnNearestEdge);

	void ComputeNearestHalfEdge_StartFarFromAnyBorder(const WorkingMemArray<NavHalfEdgeRawPtr> &nearbyEdges, const Vec3f& start,
		const Vec3f& dest, KyFloat32 distanceToBorder, NavHalfEdgeRawPtr& nearestHalfEdge, Vec3f& nearestPosOnNearestRail);

	// Generic parameter
	void* m_traverseLogicUserData;
	KyFloat32 m_distanceToBoundary;

	// Current rail
	NavHalfEdgeRawPtr m_navHalfEdge;
	EdgeGeometry m_edgeGeometry;
	RailExtremity m_railStart;
	RailExtremity m_railEnd;

	// Current position
	Vec3f m_positionOnRail;
	KyFloat32 m_abscissaOnEdge;

	// Current movement
	RailDirection m_direction;


	KyUInt32 m_setupCount;
	World* m_world;
	bool m_visualDebugActivated;

};

template <class TLogic>
inline bool PositionOnRail::Setup(const NavHalfEdgeRawPtr& navHalfEdge, KyFloat32 distanceToBoundary, const Vec3f& entry, const Vec2f& moveDir2d)
{
	const NavHalfEdgeRawPtr prevHalfEdge = navHalfEdge.GetPrevNavHalfEdgeRawPtrAlongBorder<TLogic>(m_traverseLogicUserData);
	const NavHalfEdgeRawPtr nextHalfEdge = navHalfEdge.GetNextNavHalfEdgeRawPtrAlongBorder<TLogic>(m_traverseLogicUserData);
	return SetupNoTemplate(navHalfEdge, prevHalfEdge, nextHalfEdge, distanceToBoundary, entry, moveDir2d);
}

template <class TLogic>
inline bool PositionOnRail::GetNextRailHalfEdgeRawPtr(NavHalfEdgeRawPtr& nextRailHalfEdgeRawPtr, void* traverseLogicUserData)
{
	if (m_navHalfEdge.IsValid())
	{
		switch (m_direction)
		{
		case RailDirection_Direct:
			{
				nextRailHalfEdgeRawPtr = m_navHalfEdge.GetNextNavHalfEdgeRawPtrAlongBorder<TLogic>(traverseLogicUserData);
				return nextRailHalfEdgeRawPtr.IsValid();
			}

		case RailDirection_Reverse:
			{
				nextRailHalfEdgeRawPtr = m_navHalfEdge.GetPrevNavHalfEdgeRawPtrAlongBorder<TLogic>(traverseLogicUserData);
				return nextRailHalfEdgeRawPtr.IsValid();
			}

		default:
			break;
		}
	}

	return false;
}


template <class TLogic>
inline LinkToEdgeRailResult PositionOnRail::LinkToEdgeRail(const Vec3f& start, const Vec3f& dest, const Vec2f& moveDir2d, const WorkingMemArray<NavHalfEdgeRawPtr>& nearbyEdges, KyFloat32 distanceToBorder, KyFloat32& hangingDistance)
{
	if (LinkToEdgeRail_StartNearBorder<TLogic>(start, dest, moveDir2d, nearbyEdges, distanceToBorder, hangingDistance) == LinkedToEdgeRail)
		return LinkedToEdgeRail;

	return LinkToEdgeRail_StartFarFromAnyBorder<TLogic>(start, dest, moveDir2d, nearbyEdges, distanceToBorder, hangingDistance);
}

template <class TLogic>
inline LinkToEdgeRailResult PositionOnRail::LinkToEdgeRail_StartNearBorder(const Vec3f& start, const Vec3f& dest, const Vec2f& moveDir2d, const WorkingMemArray<NavHalfEdgeRawPtr>& nearbyEdges, KyFloat32 distanceToBorder, KyFloat32& hangingDistance)
{
	Vec3f nearestPosOnNearestEdge;
	NavHalfEdgeRawPtr nearestHalfEdge;
	KyFloat32 minSqDist = KyFloat32MAXVAL;

	// We ignore edges that are entirely behind the move.
	// We consider as behind when the projection on edge back dir is more than ignoreDist (slight negative value).
	Vec3f nearestPosOnNearestEdgeFallback;
	NavHalfEdgeRawPtr nearestHalfEdgeFallback;
	KyFloat32 minSqDistFallback = KyFloat32MAXVAL;

	ComputeNearestHalfEdge_StartNearBorder(nearbyEdges, start, dest, nearestHalfEdgeFallback, nearestHalfEdge,
		minSqDistFallback, minSqDist, nearestPosOnNearestEdgeFallback, nearestPosOnNearestEdge);

	if (minSqDist <= distanceToBorder * distanceToBorder)
	{
		// Setup hanging information
		if (Setup<TLogic>(nearestHalfEdge, distanceToBorder, nearestPosOnNearestEdge, moveDir2d))
		{
			hangingDistance = Distance2d(m_positionOnRail, start);
			return LinkedToEdgeRail;
		}

		return ShouldNotLinkToNearestEdge;
	}

	if (minSqDistFallback <= distanceToBorder * distanceToBorder)
	{
		// Setup hanging information
		if (Setup<TLogic>(nearestHalfEdgeFallback, distanceToBorder, nearestPosOnNearestEdgeFallback, moveDir2d))
		{
			hangingDistance = Distance2d(m_positionOnRail, start);
			return LinkedToEdgeRail;
		}

		return ShouldNotLinkToNearestEdge;
	}

	return NoInteractingEdgeFound;
}

template <class TLogic>
inline LinkToEdgeRailResult PositionOnRail::LinkToEdgeRail_StartFarFromAnyBorder(const Vec3f& start, const Vec3f& dest, const Vec2f& moveDir2d, const WorkingMemArray<NavHalfEdgeRawPtr>& nearbyEdges, KyFloat32 distanceToBorder, KyFloat32& hangingDistance)
{
	// \pre: start is outside all nearby edge capsule.
	Vec3f nearestPosOnNearestRail;
	NavHalfEdgeRawPtr nearestHalfEdge;

	ComputeNearestHalfEdge_StartFarFromAnyBorder(nearbyEdges, start, dest, distanceToBorder, nearestHalfEdge, nearestPosOnNearestRail);

	if (nearestHalfEdge.IsValid() == false)
		return NoInteractingEdgeFound;

	// Setup hanging information
	if (Setup<TLogic>(nearestHalfEdge, distanceToBorder, nearestPosOnNearestRail, moveDir2d))
	{
		hangingDistance = Distance2d(m_positionOnRail, start);
		return LinkedToEdgeRail;
	}

	return ShouldNotLinkToNearestEdge;
}

template <class TLogic>
void MoveOnNavMeshQuery<TLogic>::PerformQuery(WorkingMemory* workingMemory)
{
	ScopedPerformedQueryCounter(GetStaticType(), m_database, m_performQueryStat);
	if (GetResult() != MOVEONNAVMESH_NOT_PROCESSED)
		return;

	if (workingMemory == KY_NULL)
		workingMemory = m_database->GetWorkingMemory();

	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	databaseGenMetrics.GetWorldIntegerPosFromVec3f(m_startPos3f, m_startIntegerPos);

	PerformQueryWithInputCoordPos(workingMemory);
}

template <class TLogic>
inline void MoveOnNavMeshQuery<TLogic>::PerformQueryWithInputCoordPos(WorkingMemory* workingMemory)
{
	WorkingMemArray<NavHalfEdgeRawPtr> nearbyEdges(workingMemory);
	if (nearbyEdges.IsInitialized() == false)
	{
		SetResult(MOVEONNAVMESH_DONE_LACK_OF_WORKING_MEMORY);
		return;
	}

	if (CollectNearbyNavMeshBorders(workingMemory, nearbyEdges) == false)
		return;

	SolveCase(workingMemory, nearbyEdges);
}

template <class TLogic>
bool MoveOnNavMeshQuery<TLogic>::CollectNearbyNavMeshBorders(WorkingMemory* workingMemory, WorkingMemArray<NavHalfEdgeRawPtr>& nearbyEdges)
{
	QueryUtils queryUtils(m_database, workingMemory, GetTraverseLogicUserData());
	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	TraversalResult traversalRC;

	if (queryUtils.FindTriangleFromPositionIfNotValid(m_startTrianglePtr, m_startPos3f, m_startIntegerPos, m_database->GetPositionSpatializationRange()) == false)
	{
		SetResult(MOVEONNAVMESH_DONE_START_OUTSIDE);
		return false;
	}

	const NavTriangleRawPtr startTriangleRawPtr = m_startTrianglePtr.GetRawPtr();
	if(startTriangleRawPtr.CanBeTraversed<TLogic>(GetTraverseLogicUserData()) == false)
	{
		SetResult(MOVEONNAVMESH_DONE_START_NAVTAG_FORBIDDEN);
		return false;
	}

	const KyInt32 radiusCellSize = (KyInt32)(databaseGenMetrics .GetNearestInteger64FromFloatValue(m_distanceToBoundary) / databaseGenMetrics.m_cellSizeInCoord) + 1;
	const CellPos destCellPos = databaseGenMetrics.ComputeCellPos(m_startPos3f + Vec3f(m_normalizedDir2d) * m_moveDistance);
	const CellPos minCellPos(Min(startTriangleRawPtr.GetCellPos().x, destCellPos.x) - radiusCellSize, Min(startTriangleRawPtr.GetCellPos().y, destCellPos.y) - radiusCellSize);
	const CellPos maxCellPos(Max(startTriangleRawPtr.GetCellPos().x, destCellPos.x) + radiusCellSize, Max(startTriangleRawPtr.GetCellPos().y, destCellPos.y) + radiusCellSize);
	const CellBox cellBox(minCellPos, maxCellPos);

	const KyFloat32 integerPrecision = m_database->GetDatabaseGenMetrics().m_integerPrecision;
	CapsuleIntersector capsuleIntersector(m_startPos3f, m_normalizedDir2d, m_moveDistance, m_distanceToBoundary, integerPrecision);
	BreadthFirstSearchEdgeCollisionCollector<TLogic, CapsuleIntersector> edgeIntersectionCollector(GetTraverseLogicUserData(), capsuleIntersector, &nearbyEdges);
	BreadthFirstSearchTraversal<BreadthFirstSearchEdgeCollisionCollector<TLogic, CapsuleIntersector> > traversalForDiskCast(queryUtils, cellBox, edgeIntersectionCollector);

	// set the root node : the triangle that contains m_startPos
	// Not that its navTag is not test within this function, but it has already been tested
	traversalRC = traversalForDiskCast.SetStartTriangle(startTriangleRawPtr);
	if (traversalRC != TraversalResult_DONE)
	{
		SetResult(traversalRC != TraversalResult_ERROR ? MOVEONNAVMESH_DONE_LACK_OF_WORKING_MEMORY : MOVEONNAVMESH_DONE_UNKNOWN_ERROR);
		return false;
	}

	// run the traversal algorithm
	traversalRC = traversalForDiskCast.Search();

	if (traversalRC != TraversalResult_DONE)
	{
		SetResult(traversalRC != TraversalResult_ERROR ? MOVEONNAVMESH_DONE_LACK_OF_WORKING_MEMORY : MOVEONNAVMESH_DONE_UNKNOWN_ERROR);
		return false;
	}

#ifndef KY_BUILD_SHIPPING
	if (m_visualDebugActivated)
	{
		const VisualShapeColor capsuleColor(VisualColor::Gray, VisualShapePrimitive_Line);
		ScopedDisplayList displayList_Capsule(m_database->GetWorld(), DisplayList_Enable);
		displayList_Capsule.InitSingleFrameLifespan("EdgeCollection: Capsule", "MoveOnNavMeshQuery");
		displayList_Capsule.PushStadium(m_startPos3f, m_startPos3f + m_moveDistance * m_normalizedDir2d, m_distanceToBoundary, 24, capsuleColor);

		const VisualColor nearbyEdgesColor = VisualColor::Orange;
		ScopedDisplayList displayList_NearbyEdges(m_database->GetWorld(), DisplayList_Enable);
		displayList_NearbyEdges.InitSingleFrameLifespan("EdgeCollection: NearbyEdges", "MoveOnNavMeshQuery");
		for(KyUInt32 i = 0; i < nearbyEdges.GetCount(); ++i)
			displayList_NearbyEdges.PushLine(nearbyEdges[i].GetStartVertexPos3f(), nearbyEdges[i].GetEndVertexPos3f(), nearbyEdgesColor);
	}
#endif

	return true;
}

template <class TLogic>
bool MoveOnNavMeshQuery<TLogic>::SolveCase(WorkingMemory* workingMemory, const WorkingMemArray<NavHalfEdgeRawPtr>& nearbyEdges)
{
	// We first make a rayCast to be able to change the input distance into a distance 2D
	// because all the computations thereafter are in 2D
	// As we may have to perform the Raycast with the distance after, we check if the distance2d is
	// indeed different

	Vec2f maxMove2D = m_normalizedDir2d * m_moveDistance;
	KyFloat32 moveDistance2d = m_moveDistance;

	RayCastQuery<TLogic> rayCastQuery;
	rayCastQuery.BindToDatabase(m_database);
	rayCastQuery.SetTraverseLogicUserData(GetTraverseLogicUserData());
	rayCastQuery.Initialize(m_startPos3f, maxMove2D);
	rayCastQuery.SetStartTrianglePtr(m_startTrianglePtr);
	rayCastQuery.SetStartIntegerPos(m_startIntegerPos);
	rayCastQuery.SetPerformQueryStat(m_performQueryStat);
	rayCastQuery.PerformQueryWithInputCoordPos(workingMemory);

	bool needToRecomputeRaycast = false;
	switch (rayCastQuery.GetResult())
	{
	case RAYCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED:
	case RAYCAST_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION:
		{
			Vec3f dir3d = rayCastQuery.GetArrivalPos() - rayCastQuery.GetStartPos();
			// if less than 1 cm altitude difference, Keep m_moveDistance
			if (fabs(dir3d.z) > 0.01f)
			{
				KyFloat32 length = dir3d.GetLength();
				moveDistance2d = (m_moveDistance*m_moveDistance) / length;
				maxMove2D = m_normalizedDir2d * moveDistance2d;
				needToRecomputeRaycast = true;
			}
		}
	default :
		break;
	}

	PositionOnRail positionOnRail(GetTraverseLogicUserData(), m_database->GetWorld(), m_visualDebugActivated);

	KyFloat32 atomicMoveDistance;
	const Vec3f dest = m_startPos3f + Vec3f(maxMove2D);
	LinkToEdgeRailResult linkToEdgeRailResult = positionOnRail.LinkToEdgeRail<TLogic>(m_startPos3f, dest, m_normalizedDir2d,
		nearbyEdges, m_distanceToBoundary, atomicMoveDistance);

	switch (linkToEdgeRailResult)
	{
	case LinkedToEdgeRail:
		break;

	case NoInteractingEdgeFound:
		{
			if (needToRecomputeRaycast)
			{
				rayCastQuery.Initialize(m_startPos3f, maxMove2D);
				rayCastQuery.SetStartTrianglePtr(m_startTrianglePtr);
				rayCastQuery.SetStartIntegerPos(m_startIntegerPos);
				rayCastQuery.SetPerformQueryStat(m_performQueryStat);
				rayCastQuery.PerformQueryWithInputCoordPos(workingMemory);
			}
			switch (rayCastQuery.GetResult())
			{
			case RAYCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED:
			case RAYCAST_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION:
				// Should always be RAYCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED...
				// ... but due to float precision, RAYCAST_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION is possible.
				m_arrivalPos3f = rayCastQuery.GetArrivalPos();
				SetResult(MOVEONNAVMESH_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION);
				return true;

			default :
					// should never occur.
					m_arrivalPos3f = m_startPos3f;
					SetResult(MOVEONNAVMESH_DONE_UNKNOWN_ERROR);
					return false;
			}
		}

	case ShouldNotLinkToNearestEdge:
		{
			m_arrivalPos3f = m_startPos3f;
			SetResult(MOVEONNAVMESH_DONE_START_LINKONEDGE_ERROR);
			return false;
		}

	default:
		break;
	}

	ScopedDisplaySuccessiveMoves scopedDisplaySuccessiveMoves(this);
	if (m_visualDebugActivated)
		scopedDisplaySuccessiveMoves.m_successivePositions.PushBack(m_startPos3f);

	// Reduce the distance to move accordingly to first edge, start position and move direction
	KyFloat32 remainingDistance;
	if (positionOnRail.GetDirection() != RailDirection_Away)
	{
		// When going towards the edge, we reduce the distance proportionally to the projection on the edge.
		Vec3f edgeDir = positionOnRail.GetEdgeGeometry().GetEdgeVec();
		edgeDir.Normalize();
		const KyFloat32 moveProjOnEdge = DotProduct2d(edgeDir, dest - positionOnRail.GetPositionOnRail());
		remainingDistance = moveProjOnEdge >= 0.0f ? moveProjOnEdge : -moveProjOnEdge;
	}
	else
	{
		// When going away, we don't reduce the distance (just integrate the hanging distance)
		remainingDistance  = moveDistance2d /*- atomicMoveDistance*/;
	}

	bool prevAtomicMoveWasNull = false;
	const KyFloat32 exitMoveDistance = (0.2f * m_distanceToBoundary);
	while (remainingDistance > 0.0f && positionOnRail.GetDirection() != RailDirection_Undefined)
	{
		if (m_visualDebugActivated)
			scopedDisplaySuccessiveMoves.m_successivePositions.PushBack(positionOnRail.GetPositionOnRail());

		const MoveOnRailResult moveOnRailResult = positionOnRail.MoveOnRail(remainingDistance, atomicMoveDistance);
		if (moveOnRailResult == SwitchRail)
		{
			if ((prevAtomicMoveWasNull == false) || (atomicMoveDistance != 0.0f))
			{
				remainingDistance -= atomicMoveDistance;
				prevAtomicMoveWasNull = (atomicMoveDistance == 0.0f);
				NavHalfEdgeRawPtr nextRailHalfEdgeRawPtr;
				if (positionOnRail.GetNextRailHalfEdgeRawPtr<TLogic>(nextRailHalfEdgeRawPtr, GetTraverseLogicUserData()))
				{
					const RailDirection prevDirection = positionOnRail.GetDirection();
					if (positionOnRail.Setup<TLogic>(nextRailHalfEdgeRawPtr, m_distanceToBoundary, positionOnRail.GetPositionOnRail(), m_normalizedDir2d) == false)
					{
						m_arrivalPos3f = positionOnRail.GetPositionOnRail();
						SetResult(MOVEONNAVMESH_DONE_INTERMEDIARYPOS_FOUND);
						return true;
					}

					const RailDirection nextDirection = positionOnRail.GetDirection();
					if ((nextDirection == RailDirection_Away) || (nextDirection == prevDirection))
						continue;
				}
			}

			m_arrivalPos3f = positionOnRail.GetPositionOnRail();
			SetResult(MOVEONNAVMESH_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION);
			return true;
		}

		if (moveOnRailResult == QuitRail)
		{
			remainingDistance = Max(remainingDistance - atomicMoveDistance, exitMoveDistance);

			// Move the position
			const Vec2f& railExitDirection = m_normalizedDir2d;
			Vec3f railExitPosition = positionOnRail.GetPositionOnRail() + exitMoveDistance * railExitDirection;
			remainingDistance -= exitMoveDistance;

			LinkToEdgeRailResult linkResult = positionOnRail.LinkToEdgeRail<TLogic>(railExitPosition, dest, m_normalizedDir2d, nearbyEdges, m_distanceToBoundary, atomicMoveDistance);
			if (linkResult == NoInteractingEdgeFound)
			{
				m_arrivalPos3f = railExitPosition + remainingDistance * railExitDirection;
				SetResult(MOVEONNAVMESH_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION);
				return true;
			}
		}

		remainingDistance -= atomicMoveDistance;
	}

	m_arrivalPos3f = positionOnRail.GetPositionOnRail();
	SetResult(MOVEONNAVMESH_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION);
	return true;
}

}

