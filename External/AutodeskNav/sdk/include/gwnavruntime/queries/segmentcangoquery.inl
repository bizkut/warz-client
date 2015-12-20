/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


namespace Kaim
{

// Primary contact: JUBA - secondary contact: NOBODY

template<class TLogic>
KY_INLINE SegmentCanGoQuery<TLogic>::SegmentCanGoQuery() : BaseSegmentCanGoQuery() {}

template<class TLogic>
KY_INLINE void SegmentCanGoQuery<TLogic>::BindToDatabase(Database* database)
{
	BaseSegmentCanGoQuery::BindToDatabase(database);
}
template<class TLogic>
KY_INLINE void SegmentCanGoQuery<TLogic>::Initialize(const Vec3f& startPos, const Vec3f& destPos, KyFloat32 radius)
{
	BaseSegmentCanGoQuery::Initialize(startPos, destPos, radius);
}

template<class TLogic>
KY_INLINE void SegmentCanGoQuery<TLogic>::SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr)
{
	BaseSegmentCanGoQuery::SetStartTrianglePtr(startTrianglePtr);
}
template<class TLogic>
KY_INLINE void SegmentCanGoQuery<TLogic>::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	BaseSegmentCanGoQuery::SetPositionSpatializationRange(positionSpatializationRange);
}
template<class TLogic>
KY_INLINE void SegmentCanGoQuery<TLogic>::SetStartIntegerPos(const WorldIntegerPos& startIntegerPos)
{
	BaseSegmentCanGoQuery::SetStartIntegerPos(startIntegerPos);
}
template<class TLogic>
KY_INLINE void SegmentCanGoQuery<TLogic>::SetDestIntegerPos(const WorldIntegerPos& destIntegerPos)
{
	BaseSegmentCanGoQuery::SetDestIntegerPos(destIntegerPos);
}
template<class TLogic>
KY_INLINE void SegmentCanGoQuery<TLogic>::SetDynamicOutputMode(DynamicOutputMode savingMode)
{
	BaseSegmentCanGoQuery::SetDynamicOutputMode(savingMode);
}
template<class TLogic>
KY_INLINE void SegmentCanGoQuery<TLogic>::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput)
{
	BaseSegmentCanGoQuery::SetQueryDynamicOutput(queryDynamicOutput);
}

template<class TLogic>
KY_INLINE SegmentCanGoQueryResult SegmentCanGoQuery<TLogic>::GetResult()                  const { return BaseSegmentCanGoQuery::GetResult();                 }
template<class TLogic>
KY_INLINE DynamicOutputMode       SegmentCanGoQuery<TLogic>::GetDynamicOutputMode()       const { return BaseSegmentCanGoQuery::GetDynamicOutputMode();      }
template<class TLogic>
KY_INLINE const Vec3f&            SegmentCanGoQuery<TLogic>::GetStartPos()                const { return BaseSegmentCanGoQuery::GetStartPos();               }
template<class TLogic>
KY_INLINE const Vec3f&            SegmentCanGoQuery<TLogic>::GetDestPos()                 const { return BaseSegmentCanGoQuery::GetDestPos();                }
template<class TLogic>
KY_INLINE const NavTrianglePtr&   SegmentCanGoQuery<TLogic>::GetStartTrianglePtr()        const { return BaseSegmentCanGoQuery::GetStartTrianglePtr();       }
template<class TLogic>
KY_INLINE const NavTrianglePtr&   SegmentCanGoQuery<TLogic>::GetDestTrianglePtr()         const { return BaseSegmentCanGoQuery::GetDestTrianglePtr();        }
template<class TLogic>
KY_INLINE KyFloat32               SegmentCanGoQuery<TLogic>::GetRadius()                  const { return BaseSegmentCanGoQuery::GetRadius();                 }
template<class TLogic>
KY_INLINE const PositionSpatializationRange& SegmentCanGoQuery<TLogic>::GetPositionSpatializationRange() const
{
	return BaseSegmentCanGoQuery::GetPositionSpatializationRange();
}
template<class TLogic>
KY_INLINE QueryDynamicOutput*     SegmentCanGoQuery<TLogic>::GetQueryDynamicOutput()      const { return BaseSegmentCanGoQuery::GetQueryDynamicOutput();     }
template<class TLogic>
KY_INLINE const WorldIntegerPos&  SegmentCanGoQuery<TLogic>::GetStartIntegerPos()         const { return BaseSegmentCanGoQuery::GetStartIntegerPos();        }
template<class TLogic>
KY_INLINE const WorldIntegerPos&  SegmentCanGoQuery<TLogic>::GetDestIntegerPos()          const { return BaseSegmentCanGoQuery::GetDestIntegerPos();         }

template<class TLogic>
KY_INLINE void SegmentCanGoQuery<TLogic>::Advance(WorkingMemory* workingMemory)
{
	PerformQuery(workingMemory);
	m_processStatus = QueryDone;
}

template<class TLogic>
inline void SegmentCanGoQuery<TLogic>::PerformQuery(WorkingMemory* workingMemory)
{
	if (GetResult() != SEGMENTCANGO_NOT_PROCESSED)
		return;

	if (m_database->IsClear())
	{
		SetResult(SEGMENTCANGO_DONE_START_OUTSIDE);
		return;
	}

	if (workingMemory == KY_NULL)
		workingMemory = m_database->GetWorkingMemory();

	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	databaseGenMetrics.GetWorldIntegerPosFromVec3f(m_startPos3f, m_startIntegerPos);
	databaseGenMetrics.GetWorldIntegerPosFromVec3f(m_destPos3f, m_destIntegerPos);

	return PerformQueryWithInputCoordPos(workingMemory);
}

template<class TLogic>
inline void SegmentCanGoQuery<TLogic>::PerformQueryWithInputCoordPos(WorkingMemory* workingMemory)
{
	ScopedPerformedQueryCounter(GetStaticType(), m_database, m_performQueryStat);
	KY_DEBUG_ASSERTN(GetResult() == SEGMENTCANGO_NOT_PROCESSED, ("Query has not been correctly initialized"));

	QueryUtils queryUtils(m_database, workingMemory, GetTraverseLogicUserData());
	TraversalResult traversalRC;

	if (queryUtils.FindTriangleFromPositionIfNotValid(m_startTrianglePtr, m_startPos3f, m_startIntegerPos, m_positionSpatializationRange) == false)
	{
		SetResult(SEGMENTCANGO_DONE_START_OUTSIDE);
		return;
	}

	const NavTriangleRawPtr startTriangleRawPtr = m_startTrianglePtr.GetRawPtr();

	if(startTriangleRawPtr.CanBeTraversed<TLogic>(GetTraverseLogicUserData()) == false)
	{
		SetResult(SEGMENTCANGO_DONE_START_NAVTAG_FORBIDDEN);
		return;
	}

	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	const KyInt32 radiusCellSize = (KyInt32)(databaseGenMetrics.GetNearestInteger64FromFloatValue(m_radius) / m_database->GetDatabaseGenMetrics().m_cellSizeInCoord) + 1;
	const CellPos minCellPos(Min(m_startIntegerPos.m_cellPos.x, m_destIntegerPos.m_cellPos.x) - radiusCellSize,
	                         Min(m_startIntegerPos.m_cellPos.y, m_destIntegerPos.m_cellPos.y) - radiusCellSize);
	const CellPos maxCellPos(Max(m_startIntegerPos.m_cellPos.x, m_destIntegerPos.m_cellPos.x) + radiusCellSize,
	                         Max(m_startIntegerPos.m_cellPos.y, m_destIntegerPos.m_cellPos.y) + radiusCellSize);
	const CellBox cellBox(minCellPos, maxCellPos);


#if defined (KY_BUILD_DEBUG)
	const CoordBox64 cellsIntegerBox(m_database->GetDatabaseGenMetrics().ComputeCellOrigin(minCellPos), m_database->GetDatabaseGenMetrics().ComputeCellOrigin(maxCellPos));
	if (Max(cellsIntegerBox.Max().x - cellsIntegerBox.Min().x, cellsIntegerBox.Max().y - cellsIntegerBox.Min().y) >= (1LL << 31))
		KY_LOG_WARNING( ("startPos and destPos are too far from each other, overflow may occur during computation, result may be incoherent"));
#endif

	// Set up the visitor and the traversal
	RectangleIntersector rectangleIntersector(*this);
	BreadthFirstSearchEdgeCollisionVisitor<TLogic, RectangleIntersector> edgeIntersectionVisitor(GetTraverseLogicUserData(), rectangleIntersector);
	BreadthFirstSearchTraversal<BreadthFirstSearchEdgeCollisionVisitor<TLogic, RectangleIntersector> > traversalSegmentCanGo(queryUtils, cellBox,
		edgeIntersectionVisitor);

	// set the root node : the triangle that contains m_startPos
	// Not that its navTag is not test within this function, but it has already been tested
	traversalRC = traversalSegmentCanGo.SetStartTriangle(startTriangleRawPtr);
	if (traversalRC != TraversalResult_DONE)
	{
		SetResult(ConvertTraversalResultToSegmentCanGoResult(traversalRC));
		return;
	}

	// check if we want to retrieve the visited triangles
	WorkingMemArray<NavTriangleRawPtr> visitedTriangles;
	ScopeAutoSaveDynamicOutput scopeAutoSaveDynOutput(m_queryDynamicOutput);
	if ((GetDynamicOutputMode() & QUERY_SAVE_TRIANGLES) != 0)
	{
		visitedTriangles.Init(workingMemory);
		if (visitedTriangles.IsInitialized() == false)
		{
			SetResult(SEGMENTCANGO_DONE_LACK_OF_WORKING_MEMORY);
			return;
		}

		scopeAutoSaveDynOutput.SetNavTriangleRawPtrs(&visitedTriangles);
		traversalSegmentCanGo.SetVisitedNodeContainer(&visitedTriangles);
	}

	// run the traversal algorithm
	traversalRC = traversalSegmentCanGo.Search();
	if (traversalRC != TraversalResult_DONE)
	{
		SetResult(ConvertTraversalResultToSegmentCanGoResult(traversalRC));
		return;
	}

	// retrieve the result of the traversal
	if (edgeIntersectionVisitor.m_collisionFound)
	{
		SetResult(SEGMENTCANGO_DONE_COLLISION_DETECTED);
		return;
	}

	// check the floor with a RayCanGo
	RayCanGoQuery<TLogic> rayCanGoQuery;
	rayCanGoQuery.BindToDatabase(m_database);
	rayCanGoQuery.SetTraverseLogicUserData(GetTraverseLogicUserData());
	rayCanGoQuery.SetPositionSpatializationRange(m_positionSpatializationRange);

	rayCanGoQuery.Initialize(m_startPos3f, m_destPos3f);
	rayCanGoQuery.SetStartTrianglePtr(m_startTrianglePtr);
	rayCanGoQuery.SetStartIntegerPos(m_startIntegerPos);
	rayCanGoQuery.SetDestIntegerPos(m_destIntegerPos);
	rayCanGoQuery.SetPerformQueryStat(m_performQueryStat);
	rayCanGoQuery.PerformQueryWithInputCoordPos(workingMemory);

	m_destTrianglePtr = rayCanGoQuery.GetDestTrianglePtr();
	SetResult(ConvertRayCanGoResultToSegmentCanGoResult(rayCanGoQuery.GetResult()));
}


}

