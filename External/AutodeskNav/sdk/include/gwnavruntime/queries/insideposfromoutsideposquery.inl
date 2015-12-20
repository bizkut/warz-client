/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY


namespace Kaim
{

template<class TLogic>
KY_INLINE InsidePosFromOutsidePosQuery<TLogic>::InsidePosFromOutsidePosQuery() : BaseInsidePosFromOutsidePosQuery() {}

template<class TLogic>
KY_INLINE void InsidePosFromOutsidePosQuery<TLogic>::BindToDatabase(Database* database)
{
	BaseInsidePosFromOutsidePosQuery::BindToDatabase(database);
}
template<class TLogic>
KY_INLINE void InsidePosFromOutsidePosQuery<TLogic>::Initialize(const Vec3f& inputPos)
{
	BaseInsidePosFromOutsidePosQuery::Initialize(inputPos);
}

template<class TLogic>
KY_INLINE void InsidePosFromOutsidePosQuery<TLogic>::SetHorizontalTolerance(KyFloat32 horizontalTolerance)
{
	BaseInsidePosFromOutsidePosQuery::SetHorizontalTolerance(horizontalTolerance);
}
template<class TLogic>
KY_INLINE void InsidePosFromOutsidePosQuery<TLogic>::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	BaseInsidePosFromOutsidePosQuery::SetPositionSpatializationRange(positionSpatializationRange);
}
template<class TLogic>
KY_INLINE void InsidePosFromOutsidePosQuery<TLogic>::SetDistFromObstacle(KyFloat32 distFromObstacle)
{
	BaseInsidePosFromOutsidePosQuery::SetDistFromObstacle(distFromObstacle);
}
template<class TLogic>
KY_INLINE void InsidePosFromOutsidePosQuery<TLogic>::SetInputIntegerPos(const WorldIntegerPos& inputIntegerPos)
{
	BaseInsidePosFromOutsidePosQuery::SetInputIntegerPos(inputIntegerPos);
}
template<class TLogic>
KY_INLINE InsidePosFromOutsidePosQueryResult InsidePosFromOutsidePosQuery<TLogic>::GetResult() const
{
	return BaseInsidePosFromOutsidePosQuery::GetResult();
}
template<class TLogic>
KY_INLINE const Vec3f& InsidePosFromOutsidePosQuery<TLogic>::GetInputPos() const
{
	return BaseInsidePosFromOutsidePosQuery::GetInputPos();
}
template<class TLogic>
KY_INLINE const Vec3f& InsidePosFromOutsidePosQuery<TLogic>::GetInsidePos() const
{
	return BaseInsidePosFromOutsidePosQuery::GetInsidePos();
}
template<class TLogic>
KY_INLINE const NavTrianglePtr& InsidePosFromOutsidePosQuery<TLogic>::GetInsidePosTrianglePtr() const
{
	return BaseInsidePosFromOutsidePosQuery::GetInsidePosTrianglePtr();
}
template<class TLogic>
KY_INLINE KyFloat32 InsidePosFromOutsidePosQuery<TLogic>::GetHorizontalTolerance() const
{
	return BaseInsidePosFromOutsidePosQuery::GetHorizontalTolerance();
}
template<class TLogic>
KY_INLINE const PositionSpatializationRange& InsidePosFromOutsidePosQuery<TLogic>::GetPositionSpatializationRange() const
{
	return BaseInsidePosFromOutsidePosQuery::GetPositionSpatializationRange();
}
template<class TLogic>
KY_INLINE KyFloat32 InsidePosFromOutsidePosQuery<TLogic>::GetDistFromObstacle() const
{
	return BaseInsidePosFromOutsidePosQuery::GetDistFromObstacle();
}
template<class TLogic>
KY_INLINE const WorldIntegerPos& InsidePosFromOutsidePosQuery<TLogic>::GetInputIntegerPos() const
{
	return BaseInsidePosFromOutsidePosQuery::GetInputIntegerPos();
}
template<class TLogic>
KY_INLINE const WorldIntegerPos& InsidePosFromOutsidePosQuery<TLogic>::GetInsideIntegerPos() const
{
	return BaseInsidePosFromOutsidePosQuery::GetInsideIntegerPos();
}

template<class TLogic>
KY_INLINE void InsidePosFromOutsidePosQuery<TLogic>::Advance(WorkingMemory* workingMemory)
{
	PerformQuery(workingMemory);
	m_processStatus = QueryDone;
}


template<class TLogic>
inline void InsidePosFromOutsidePosQuery<TLogic>::PerformQuery(WorkingMemory* workingMemory)
{
	KY_SCOPED_PERF_MARKER("Navigation InsidePosFromOutsidePosQuery");

	if (GetResult() != INSIDEPOSFROMOUTSIDE_NOT_PROCESSED)
		return;

	if (m_database->IsClear())
	{
		SetResult(INSIDEPOSFROMOUTSIDE_DONE_POS_NOT_FOUND);
		return;
	}

	if (workingMemory == KY_NULL)
		workingMemory = m_database->GetWorkingMemory();

	const DatabaseGenMetrics& databaseGenMetrics = m_database->GetDatabaseGenMetrics();
	databaseGenMetrics.GetWorldIntegerPosFromVec3f(m_inputPos3f, m_inputIntegerPos);

	return PerformQueryWithInputCoordPos(workingMemory);
}

template<class TLogic>
inline void InsidePosFromOutsidePosQuery<TLogic>::PerformQueryWithInputCoordPos(WorkingMemory* workingMemory)
{
	ScopedPerformedQueryCounter(GetStaticType(), m_database, m_performQueryStat);
	NearestBorderHalfEdgeFromPosQuery<TLogic> nearestBorderHalfEdgeFromPosQuery;
	nearestBorderHalfEdgeFromPosQuery.BindToDatabase(m_database);
	nearestBorderHalfEdgeFromPosQuery.SetTraverseLogicUserData(GetTraverseLogicUserData());
	nearestBorderHalfEdgeFromPosQuery.SetHorizontalTolerance(m_horizontalTolerance);
	nearestBorderHalfEdgeFromPosQuery.SetPositionSpatializationRange(m_positionSpatializationRange);

	nearestBorderHalfEdgeFromPosQuery.Initialize(m_inputPos3f);
	nearestBorderHalfEdgeFromPosQuery.SetInputIntegerPos(m_inputIntegerPos);
	nearestBorderHalfEdgeFromPosQuery.PerformQueryWithInputCoordPos(workingMemory);

	Vec3f startToEndNormalInside2d;

	if (KY_FAILED(ProcessNearestBorderHalfEdgeFromPosQueryResult(workingMemory, nearestBorderHalfEdgeFromPosQuery, startToEndNormalInside2d)))
		return;

	const NavHalfEdgeRawPtr nearestBorderHalfEdgeRawPtr = nearestBorderHalfEdgeFromPosQuery.GetNearestHalfEdgePtrOnBorder().GetRawPtr();

	NavTrianglePtr startTrianglePtr(nearestBorderHalfEdgeRawPtr.GetNavFloor(), NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(nearestBorderHalfEdgeRawPtr.GetHalfEdgeIdx()));

	RayCastQuery<TLogic> rayCastQuery;
	rayCastQuery.BindToDatabase(m_database);
	rayCastQuery.SetTraverseLogicUserData(GetTraverseLogicUserData());
	rayCastQuery.Initialize(nearestBorderHalfEdgeFromPosQuery.GetNearestPosOnHalfEdge(), startToEndNormalInside2d.Get2d() * m_distFromObstacle);
	rayCastQuery.SetStartTrianglePtr(startTrianglePtr);
	rayCastQuery.SetStartIntegerPos(nearestBorderHalfEdgeFromPosQuery.GetNearestIntegerPosOnHalfEdge());
	rayCastQuery.SetPerformQueryStat(m_performQueryStat);
	rayCastQuery.PerformQueryWithInputCoordPos(workingMemory);

	const RayCastQueryResult rayCastQueryResult = rayCastQuery.GetResult();

	if (rayCastQueryResult == RAYCAST_DONE_ARRIVALPOS_FOUND_MAXDIST_REACHED || rayCastQueryResult == RAYCAST_DONE_ARRIVALPOS_FOUND_AFTER_COLLISION)
	{
		SetResult(INSIDEPOSFROMOUTSIDE_DONE_POS_FOUND);
		m_insidePos3f = rayCastQuery.GetArrivalPos();
		m_insidePosTrianglePtr = rayCastQuery.GetArrivalTrianglePtr();
	}
	else
	{
		SetResult(INSIDEPOSFROMOUTSIDE_DONE_POS_NOT_FOUND);
	}
}
}
