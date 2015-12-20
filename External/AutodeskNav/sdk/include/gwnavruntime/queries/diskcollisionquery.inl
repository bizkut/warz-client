/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY


namespace Kaim
{

template<class TLogic>
KY_INLINE DiskCollisionQuery<TLogic>::DiskCollisionQuery() : BaseDiskCollisionQuery() {}

template<class TLogic>
KY_INLINE void DiskCollisionQuery<TLogic>::BindToDatabase(Database* database)
{
	BaseDiskCollisionQuery::BindToDatabase(database);
}

template<class TLogic>
KY_INLINE void DiskCollisionQuery<TLogic>::Initialize(const Vec3f& center, KyFloat32 radius)
{
	BaseDiskCollisionQuery::Initialize(center, radius);
}


template<class TLogic>
KY_INLINE void DiskCollisionQuery<TLogic>::SetCenterTrianglePtr(const NavTrianglePtr& centerTrianglePtr)
{
	BaseDiskCollisionQuery::SetCenterTrianglePtr(centerTrianglePtr);
}

template<class TLogic>
KY_INLINE void DiskCollisionQuery<TLogic>::SetCenterIntegerPos(const WorldIntegerPos& centerIntegerPos)
{
	BaseDiskCollisionQuery::SetCenterIntegerPos(centerIntegerPos);
}

template<class TLogic>
KY_INLINE void DiskCollisionQuery<TLogic>::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	BaseDiskCollisionQuery::SetPositionSpatializationRange(positionSpatializationRange);
}

template<class TLogic>
KY_INLINE void DiskCollisionQuery<TLogic>::SetDynamicOutputMode(DynamicOutputMode dynamicOutputMode)
{
	BaseDiskCollisionQuery::SetDynamicOutputMode(dynamicOutputMode);
}
template<class TLogic>
KY_INLINE void DiskCollisionQuery<TLogic>::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput)
{
	BaseDiskCollisionQuery::SetQueryDynamicOutput(queryDynamicOutput);
}

template<class TLogic>
KY_INLINE DiskCollisionQueryResult DiskCollisionQuery<TLogic>::GetResult()                 const { return BaseDiskCollisionQuery::GetResult();                 }
template<class TLogic>
KY_INLINE DynamicOutputMode        DiskCollisionQuery<TLogic>::GetDynamicOutputMode()      const { return BaseDiskCollisionQuery::GetDynamicOutputMode();      }
template<class TLogic>
KY_INLINE const Vec3f&             DiskCollisionQuery<TLogic>::GetCenterPos()              const { return BaseDiskCollisionQuery::GetCenterPos();              }
template<class TLogic>
KY_INLINE const NavTrianglePtr&    DiskCollisionQuery<TLogic>::GetCenterTrianglePtr()      const { return BaseDiskCollisionQuery::GetCenterTrianglePtr();      }
template<class TLogic>
KY_INLINE KyFloat32                DiskCollisionQuery<TLogic>::GetRadius()                 const { return BaseDiskCollisionQuery::GetRadius();                 }
template<class TLogic>
KY_INLINE QueryDynamicOutput*      DiskCollisionQuery<TLogic>::GetQueryDynamicOutput()     const { return BaseDiskCollisionQuery::GetQueryDynamicOutput();     }
template<class TLogic>
KY_INLINE const WorldIntegerPos&   DiskCollisionQuery<TLogic>::GetCenterIntegerPos()       const { return BaseDiskCollisionQuery::GetCenterIntegerPos();       }
template<class TLogic>
KY_INLINE const PositionSpatializationRange& DiskCollisionQuery<TLogic>::GetPositionSpatializationRange() const { return BaseDiskCollisionQuery::GetPositionSpatializationRange(); }

template<class TLogic>
KY_INLINE void DiskCollisionQuery<TLogic>::Advance(WorkingMemory* workingMemory)
{
	PerformQuery(workingMemory);
	m_processStatus = QueryDone;
}

template<class TLogic>
void DiskCollisionQuery<TLogic>::PerformQuery(WorkingMemory* workingMemory)
{
	if (GetResult() != DISKCOLLISION_NOT_PROCESSED)
		return;

	if (m_database->IsClear())
	{
		SetResult(DISKCOLLISION_DONE_CENTER_OUTSIDE);
		return;
	}

	if (workingMemory == KY_NULL)
		workingMemory = m_database->GetWorkingMemory();

	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	databaseGenMetrics.GetWorldIntegerPosFromVec3f(m_centerPos3f, m_centerIntegerPos);

	return PerformQueryWithInputCoordPos(workingMemory);
}

template<class TLogic>
void DiskCollisionQuery<TLogic>::PerformQueryWithInputCoordPos(WorkingMemory* workingMemory)
{
	ScopedPerformedQueryCounter(GetStaticType(), m_database, m_performQueryStat);
	KY_DEBUG_ASSERTN(GetResult() == DISKCOLLISION_NOT_PROCESSED, ("Query has not been correctly initialized"));

	QueryUtils queryUtils(m_database, workingMemory, GetTraverseLogicUserData());
	TraversalResult traversalRC;
	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();

	if (queryUtils.FindTriangleFromPositionIfNotValid(m_centerTrianglePtr, m_centerPos3f, m_centerIntegerPos, m_positionSpatializationRange) == false)
	{
		SetResult(DISKCOLLISION_DONE_CENTER_OUTSIDE);
		return;
	}

	const NavTriangleRawPtr centerTriangleRawPtr = m_centerTrianglePtr.GetRawPtr();

	if (centerTriangleRawPtr.CanBeTraversed<TLogic>(GetTraverseLogicUserData()) == false)
	{
		SetResult(DISKCOLLISION_DONE_CENTER_NAVTAG_FORBIDDEN);
		return;
	}

	const KyInt32 radiusCellSize = (KyInt32)(databaseGenMetrics.GetNearestInteger64FromFloatValue(m_radius) / databaseGenMetrics.m_cellSizeInCoord) + 1;
	const CellPos minCellPos(m_centerIntegerPos.m_cellPos.x - radiusCellSize, m_centerIntegerPos.m_cellPos.y - radiusCellSize);
	const CellPos maxCellPos(m_centerIntegerPos.m_cellPos.x + radiusCellSize, m_centerIntegerPos.m_cellPos.y + radiusCellSize);
	const CellBox cellBox(minCellPos, maxCellPos);

#if defined (KY_BUILD_DEBUG)
	const CoordBox64 cellsIntegerBox(databaseGenMetrics.ComputeCellOrigin(minCellPos), databaseGenMetrics.ComputeCellOrigin(maxCellPos));
	if (Max(cellsIntegerBox.Max().x - cellsIntegerBox.Min().x, cellsIntegerBox.Max().y - cellsIntegerBox.Min().y) >= (1LL << 31))
		KY_DEBUG_WARNINGN( ("startPos and destPos are too far from each other, overflow may occur during computation, result may be incoherent"));
#endif

	// Set up the visitor and the traversal
	DiskIntersector diskIntersector(*this);
	BreadthFirstSearchEdgeCollisionVisitor<TLogic, DiskIntersector> edgeIntersectionVisitor(GetTraverseLogicUserData(), diskIntersector);
	BreadthFirstSearchTraversal<BreadthFirstSearchEdgeCollisionVisitor<TLogic, DiskIntersector> > traversalDiskCollision(queryUtils, cellBox,
		edgeIntersectionVisitor);

	// set the root node : the triangle that contains m_startPos
	// Not that its navTag is not test within this function, but it has already been tested
	traversalRC = traversalDiskCollision.SetStartTriangle(centerTriangleRawPtr);
	if (traversalRC != TraversalResult_DONE)
	{
		SetResult(ConvertTraversalResultToDiskCollisionResult(traversalRC));
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
			SetResult(DISKCOLLISION_DONE_LACK_OF_WORKING_MEMORY);
			return;
		}

		scopeAutoSaveDynOutput.SetNavTriangleRawPtrs(&visitedTriangles);

		traversalDiskCollision.SetVisitedNodeContainer(&visitedTriangles);
	}

	// run the traversal algorithm
	traversalRC = traversalDiskCollision.Search();
	if (traversalRC != TraversalResult_DONE)
	{
		SetResult(ConvertTraversalResultToDiskCollisionResult(traversalRC));
		return;
	}

	// retrieve the result of the traversal
	SetResult(edgeIntersectionVisitor.m_collisionFound ? DISKCOLLISION_DONE_DISK_DOES_NOT_FIT : DISKCOLLISION_DONE_DISK_FIT);
}


}
