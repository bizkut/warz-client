/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: LAPA - secondary contact: NOBODY


namespace Kaim
{
template <class SpatializedPointCollectorFilter>
SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::SpatializedPointCollectorInAABBQuery() : 
BaseSpatializedPointCollectorInAABBQuery(), m_spatializedPointCollectorFilter() {}

template <class SpatializedPointCollectorFilter>
SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::SpatializedPointCollectorInAABBQuery(const SpatializedPointCollectorFilter& collectorFilter) :
BaseSpatializedPointCollectorInAABBQuery(), m_spatializedPointCollectorFilter(collectorFilter) {}

template <class SpatializedPointCollectorFilter>
KY_INLINE void SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::BindToDatabase(Database* database)
{
	BaseSpatializedPointCollectorInAABBQuery::BindToDatabase(database);
}

template <class SpatializedPointCollectorFilter>
KY_INLINE void SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::Initialize(const Vec3f& startPos, const Box3f& aABB)
{
	BaseSpatializedPointCollectorInAABBQuery::Initialize(startPos, aABB);
}

template <class SpatializedPointCollectorFilter>
KY_INLINE void SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::Advance(WorkingMemory* workingMemory)
{
	PerformQuery(workingMemory);
	m_processStatus = QueryDone;
}

template <class SpatializedPointCollectorFilter>
KY_INLINE void SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::SetStartTrianglePtr(const NavTrianglePtr& startTrianglePtr) 
{
	BaseSpatializedPointCollectorInAABBQuery::SetStartTrianglePtr(startTrianglePtr);
}
template <class SpatializedPointCollectorFilter>
KY_INLINE void SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	BaseSpatializedPointCollectorInAABBQuery::SetPositionSpatializationRange(positionSpatializationRange);
}
template <class SpatializedPointCollectorFilter>
KY_INLINE void SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::SetStartIntegerPos(const WorldIntegerPos& startIntegerPos)
{
	BaseSpatializedPointCollectorInAABBQuery::SetStartIntegerPos(startIntegerPos);
}
template <class SpatializedPointCollectorFilter>
KY_INLINE void SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::SetQueryDynamicOutput(QueryDynamicOutput* queryDynamicOutput)
{
	return BaseSpatializedPointCollectorInAABBQuery::SetQueryDynamicOutput(queryDynamicOutput);
}

template <class SpatializedPointCollectorFilter>
KY_INLINE void SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::SetResult(SpatializedPointCollectorInAABBQueryResult result)
{
	BaseSpatializedPointCollectorInAABBQuery::SetResult(result);
}

template <class SpatializedPointCollectorFilter>
KY_INLINE SpatializedPointCollectorInAABBQueryResult SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::GetResult() const
{
	return BaseSpatializedPointCollectorInAABBQuery::GetResult();
}

template <class SpatializedPointCollectorFilter>
KY_INLINE const Vec3f& SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::GetStartPos() const
{
	return BaseSpatializedPointCollectorInAABBQuery::GetStartPos();
}
template <class SpatializedPointCollectorFilter>
KY_INLINE const Box3f& SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::GetExtentBox() const
{
	return BaseSpatializedPointCollectorInAABBQuery::GetExtentBox();
}
template <class SpatializedPointCollectorFilter>
KY_INLINE const NavTrianglePtr& SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::GetStartTrianglePtr() const
{
	return BaseSpatializedPointCollectorInAABBQuery::GetStartTrianglePtr();
}
template <class SpatializedPointCollectorFilter>
KY_INLINE const PositionSpatializationRange& SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::GetPositionSpatializationRange() const
{
	return BaseSpatializedPointCollectorInAABBQuery::GetPositionSpatializationRange();
}
template <class SpatializedPointCollectorFilter>
KY_INLINE const WorldIntegerPos& SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::GetStartIntegerPos()  const
{
	return BaseSpatializedPointCollectorInAABBQuery::GetStartIntegerPos();
}

template <class SpatializedPointCollectorFilter>
KY_INLINE QueryDynamicOutput* SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::GetQueryDynamicOutput() const
{
	return BaseSpatializedPointCollectorInAABBQuery::GetQueryDynamicOutput();
}

template <class SpatializedPointCollectorFilter>
inline void SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::PerformQuery(WorkingMemory* workingMemory)
{
	if (GetResult() != SPATIALIZEDPOINTCOLLECTOR_NOT_PROCESSED)
		return;

	if (m_database->IsClear())
	{
		SetResult(SPATIALIZEDPOINTCOLLECTOR_DONE_START_OUTSIDE);
		return;
	}

	if (workingMemory == KY_NULL)
		workingMemory = m_database->GetWorkingMemory();

	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	databaseGenMetrics.GetWorldIntegerPosFromVec3f(m_startPos3f, m_startIntegerPos);

	PerformQueryWithInputCoordPos(workingMemory);
}

template <class SpatializedPointCollectorFilter>
void SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::PerformQueryWithInputCoordPos(WorkingMemory* workingMemory)
{
	ScopedPerformedQueryCounter(GetStaticType(), m_database, m_performQueryStat);

	SpatializedPointCollectorContext* collectorContext = workingMemory->GetOrCreateSpatializedPointCollectorContext();
	if (KY_SUCCEEDED(PrepareWithInputCoordPos(workingMemory)))
	{ // Scope to be sure we release the working memory after the result has been copied into m_queryDynamicOutput 

		ScopeAutoSaveDynamicOutput scopeAutoSaveDynamicOutput(m_queryDynamicOutput);
		scopeAutoSaveDynamicOutput.SetSpatializedPoints(&collectorContext->m_resultCollection);

		RunCollectorTraversal(collectorContext);
	}

	collectorContext->ReleaseWorkingMemory();
}

template <class SpatializedPointCollectorFilter>
inline void SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::RunCollectorTraversal(SpatializedPointCollectorContext* traversalContext)
{
	CollectorTraversalNodeIdx traversalNodeIdx;

	while (traversalContext->m_openNodes.IsEmpty() == false)
	{
		traversalContext->m_openNodes.Front(traversalNodeIdx);
		traversalContext->m_openNodes.PopFront();

		if (traversalNodeIdx.IsNavFloorNode())
		{
			// copy NavFloorRawPtr on the stack to be sure that we can use it in ProcessNavFloorNode even if the array is resized
			NavFloorRawPtr navFloorNode = traversalContext->m_navFloorRawPtrNodes[traversalNodeIdx.GetNodeIndex()];
			if (KY_FAILED((ProcessNavFloorNode(traversalContext, navFloorNode))))
			{
				if (GetResult() == SPATIALIZEDPOINTCOLLECTOR_NOT_PROCESSED)
					SetResult(SPATIALIZEDPOINTCOLLECTOR_DONE_LACK_OF_WORKING_MEMORY);

				return;
			}
		}
		else
		{
			// copy navGraphEdgeNode on the stack to be sure that we can use it in ProcessNavGraphEdgeNode even if the array is resized
			NavGraphEdgeRawPtr navGraphEdgeNode = traversalContext->m_edgeRawPtrNodes[traversalNodeIdx.GetNodeIndex()];
			if (KY_FAILED((ProcessNavGraphEdgeNode(traversalContext, navGraphEdgeNode))))
			{
				if (GetResult() == SPATIALIZEDPOINTCOLLECTOR_NOT_PROCESSED)
					SetResult(SPATIALIZEDPOINTCOLLECTOR_DONE_LACK_OF_WORKING_MEMORY);

				return;
			}
		}
	}

	SetResult(SPATIALIZEDPOINTCOLLECTOR_DONE);
	return;
}

template <class SpatializedPointCollectorFilter>
inline KyResult SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::ProcessNavFloorNode(SpatializedPointCollectorContext* traversalContext, const NavFloorRawPtr& navFloorRawPtr)
{
	KY_FORWARD_ERROR_NO_LOG(CollectInNavFloor(traversalContext, navFloorRawPtr));
	KY_FORWARD_ERROR_NO_LOG(TraverseNeighborNavFloors(traversalContext, navFloorRawPtr));
	KY_FORWARD_ERROR_NO_LOG(TraverseNeighborNavGraphEdges(traversalContext, navFloorRawPtr));
	return KY_SUCCESS;
}

template <class SpatializedPointCollectorFilter>
inline KyResult SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::ProcessNavGraphEdgeNode(SpatializedPointCollectorContext* traversalContext, const NavGraphEdgeRawPtr& navGraphEdgeRawPtr)
{
	KY_FORWARD_ERROR_NO_LOG(CollectInNavGraphEdge(traversalContext, navGraphEdgeRawPtr));
	KY_FORWARD_ERROR_NO_LOG(TraverseNeighborNavFloors(traversalContext, navGraphEdgeRawPtr));
	KY_FORWARD_ERROR_NO_LOG(TraverseNeighborNavGraphEdges(traversalContext, navGraphEdgeRawPtr));
	return KY_SUCCESS;
}

template <class SpatializedPointCollectorFilter>
inline KyResult SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::CollectInNavGraphEdge(SpatializedPointCollectorContext* /*traversalContext*/, const NavGraphEdgeRawPtr& /*navGraphEdgeRawPtr*/)
{
	return KY_SUCCESS;
}

template <class SpatializedPointCollectorFilter>
inline KyResult SpatializedPointCollectorInAABBQuery<SpatializedPointCollectorFilter>::CollectInNavFloor(SpatializedPointCollectorContext* traversalContext, const NavFloorRawPtr& navFloorRawPtr)
{
	const Box3f aabb = ComputeAABB();
	const Collection<SpatializedPoint*>& navFloorSpatializedPoints = navFloorRawPtr.GetNavFloor()->GetSpatializedPoints();
	const KyUInt32 spatializedPointCount = navFloorSpatializedPoints.GetCount();
	for (KyUInt32 i = 0; i < spatializedPointCount; ++i)
	{
		SpatializedPoint* spatializedPoint = navFloorSpatializedPoints[i];
		if (aabb.IsPoint3DInside(spatializedPoint->GetPosition()) == false)
			continue;

		if (m_spatializedPointCollectorFilter.ShouldCollectSpatializedPoint(spatializedPoint) == false)
			continue;

		if (KY_FAILED(traversalContext->m_resultCollection.PushBack(navFloorSpatializedPoints[i])))
		{
			KY_LOG_WARNING( ("This query reached the maximum size of working memory"));
			return KY_ERROR;
		}
	}

	return KY_SUCCESS;
}

}

