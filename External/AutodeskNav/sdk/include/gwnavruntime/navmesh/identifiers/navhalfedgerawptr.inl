/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{

KY_INLINE NavHalfEdgeRawPtr::NavHalfEdgeRawPtr() : m_navFloorRawPtr(), m_halfEdgeIdx(CompactNavHalfEdgeIdx_MAXVAL), m_boundaryEdgeIdx(KyUInt16MAXVAL) {}

KY_INLINE NavHalfEdgeRawPtr::NavHalfEdgeRawPtr(const NavFloorRawPtr& navFloorRawPtr, NavHalfEdgeIdx halfEdgeIdx) :
m_navFloorRawPtr(navFloorRawPtr), m_halfEdgeIdx((CompactNavHalfEdgeIdx)halfEdgeIdx), m_boundaryEdgeIdx(KyUInt16MAXVAL) {}

KY_INLINE void NavHalfEdgeRawPtr::Set(const NavFloorRawPtr& navFloorRawPtr, NavHalfEdgeIdx halfEdgeIdx)
{
	m_navFloorRawPtr = navFloorRawPtr;
	m_halfEdgeIdx = (CompactNavHalfEdgeIdx)halfEdgeIdx;
	m_boundaryEdgeIdx = KyUInt16MAXVAL;
}

KY_INLINE NavHalfEdgeRawPtr::NavHalfEdgeRawPtr(NavFloor* navFloor, NavHalfEdgeIdx halfEdgeIdx) : 
m_navFloorRawPtr(navFloor), m_halfEdgeIdx((CompactNavHalfEdgeIdx)halfEdgeIdx), m_boundaryEdgeIdx(KyUInt16MAXVAL) {}

KY_INLINE void NavHalfEdgeRawPtr::Set(NavFloor* navFloor, NavHalfEdgeIdx halfEdgeIdx)
{
	m_navFloorRawPtr.Set(navFloor);
	m_halfEdgeIdx = (CompactNavHalfEdgeIdx)halfEdgeIdx;
	m_boundaryEdgeIdx = KyUInt16MAXVAL;
}

KY_INLINE bool NavHalfEdgeRawPtr::IsValid() const { return m_navFloorRawPtr.IsValid() && m_halfEdgeIdx != CompactNavHalfEdgeIdx_MAXVAL; }
KY_INLINE void NavHalfEdgeRawPtr::Invalidate() { m_navFloorRawPtr.Invalidate(); m_halfEdgeIdx = CompactNavHalfEdgeIdx_MAXVAL; m_boundaryEdgeIdx = KyUInt16MAXVAL; }

KY_INLINE bool NavHalfEdgeRawPtr::operator==(const NavHalfEdgeRawPtr& navHalfEdgeRawPtr) const { return m_navFloorRawPtr == navHalfEdgeRawPtr.m_navFloorRawPtr && m_halfEdgeIdx == navHalfEdgeRawPtr.m_halfEdgeIdx;}
KY_INLINE bool NavHalfEdgeRawPtr::operator!=(const NavHalfEdgeRawPtr& rhs) const { return !(*this == rhs); }

KY_INLINE NavHalfEdgeRawPtr& NavHalfEdgeRawPtr::GetCorrespondingLink()
{
	const NavFloorBlob* navFloorBlob = GetNavFloorBlob();

	KY_DEBUG_ASSERTN(IsHalfEdgeAFloorOrCellBoundary(navFloorBlob->GetHalfEdgeType(m_halfEdgeIdx)), ("Error in halfEdge type"));
	const KyUInt32 linkIdx = navFloorBlob->GetNavHalfEdge(m_halfEdgeIdx).GetBoundaryEdgeIdx();

	return m_navFloorRawPtr.GetNavFloor()->GetLinksBuffer()[linkIdx];
}

KY_INLINE CoordPos NavHalfEdgeRawPtr::GetStartVertexCoordPosInCell() const { return GetNavFloorBlob()->NavHalfEdgeIdxToStartNavVertex(m_halfEdgeIdx).GetCoordPos(); }
KY_INLINE CoordPos NavHalfEdgeRawPtr::GetEndVertexCoordPosInCell()   const { return GetNavFloorBlob()->NavHalfEdgeIdxToEndNavVertex(m_halfEdgeIdx).GetCoordPos(); }

KY_INLINE void NavHalfEdgeRawPtr::GetNextHalfEdgeRawPtr(NavHalfEdgeRawPtr& resultRawPtr) const { resultRawPtr.Set(m_navFloorRawPtr, NavFloorBlob::NavHalfEdgeIdxToNextNavHalfEdgeIdx(m_halfEdgeIdx)); }
KY_INLINE void NavHalfEdgeRawPtr::GetPrevHalfEdgeRawPtr(NavHalfEdgeRawPtr& resultRawPtr) const { resultRawPtr.Set(m_navFloorRawPtr, NavFloorBlob::NavHalfEdgeIdxToPrevNavHalfEdgeIdx(m_halfEdgeIdx)); }

KY_INLINE NavHalfEdgeIdx      NavHalfEdgeRawPtr::GetHalfEdgeIdx()  const { return (NavHalfEdgeIdx)m_halfEdgeIdx;      }
KY_INLINE NavFloor*           NavHalfEdgeRawPtr::GetNavFloor()     const { return m_navFloorRawPtr.GetNavFloor();     }
KY_INLINE const NavFloorBlob* NavHalfEdgeRawPtr::GetNavFloorBlob() const { return m_navFloorRawPtr.GetNavFloorBlob(); }
KY_INLINE const CellPos&      NavHalfEdgeRawPtr::GetCellPos()      const { return m_navFloorRawPtr.GetCellPos();      }

KY_INLINE const NavTag& NavHalfEdgeRawPtr::GetNavTag() const { return GetNavFloorBlob()->GetNavTag(NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(m_halfEdgeIdx)); }

KY_INLINE KyFloat32 NavHalfEdgeRawPtr::GetStartVertexAltitude() const
{
	const NavFloorBlob* navFloorBlob = GetNavFloorBlob();
	return navFloorBlob->GetNavVertexAltitude(navFloorBlob->NavHalfEdgeIdxToStartNavVertexIdx(m_halfEdgeIdx));
}
KY_INLINE KyFloat32 NavHalfEdgeRawPtr::GetEndVertexAltitude() const
{
	const NavFloorBlob* navFloorBlob = GetNavFloorBlob();
	return navFloorBlob->GetNavVertexAltitude(navFloorBlob->NavHalfEdgeIdxToEndNavVertexIdx(m_halfEdgeIdx));
}

template<class TLogic>
KY_INLINE bool NavHalfEdgeRawPtr::IsHalfEdgeCrossable(void* traverseLogicUserData) const
{
	NavHalfEdgeRawPtr unused;
	return IsHalfEdgeCrossable<TLogic>(traverseLogicUserData, unused);
}
template<class TLogic>
KY_INLINE bool NavHalfEdgeRawPtr::IsHalfEdgeOneWayCrossable(void* traverseLogicUserData, const Vec3f* position) const
{
	NavHalfEdgeRawPtr unused;
	return IsHalfEdgeOneWayCrossable<TLogic>(traverseLogicUserData, unused, position);
}

template<class TLogic>
KY_INLINE bool NavHalfEdgeRawPtr::IsHalfEdgeCrossable(void* traverseLogicUserData, NavHalfEdgeRawPtr& resultRawPtr) const
{
	return IsHalfEdgeCrossable<TLogic>(traverseLogicUserData, GetNavFloorBlob(), resultRawPtr);
}
template<class TLogic>
KY_INLINE bool NavHalfEdgeRawPtr::IsHalfEdgeOneWayCrossable(void* traverseLogicUserData, NavHalfEdgeRawPtr& resultRawPtr, const Vec3f* position) const
{
	return IsHalfEdgeOneWayCrossable<TLogic>(traverseLogicUserData, GetNavFloorBlob(), resultRawPtr, position);
}

template<class TLogic>
KY_INLINE bool NavHalfEdgeRawPtr::IsHalfEdgeCrossable(void* traverseLogicUserData, const NavFloorBlob* navFloorBlob, NavHalfEdgeRawPtr& resultRawPtr) const
{
	return IsHalfEdgeCrossable<TLogic>(traverseLogicUserData, navFloorBlob->GetNavHalfEdge(m_halfEdgeIdx), navFloorBlob, resultRawPtr);
}
template<class TLogic>
KY_INLINE bool NavHalfEdgeRawPtr::IsHalfEdgeOneWayCrossable(void* traverseLogicUserData, const NavFloorBlob* navFloorBlob,
	NavHalfEdgeRawPtr& resultRawPtr, const Vec3f* position) const
{
	return IsHalfEdgeOneWayCrossable<TLogic>(traverseLogicUserData, navFloorBlob->GetNavHalfEdge(m_halfEdgeIdx), navFloorBlob, resultRawPtr, position, KY_NULL);
}
template<class TLogic>
KY_INLINE bool NavHalfEdgeRawPtr::IsHalfEdgeOneWayCrossable(void* traverseLogicUserData, NavHalfEdge navHalfEdge, const NavFloorBlob* navFloorBlob,
	NavHalfEdgeRawPtr& resultRawPtr, KyFloat32* costMultiplier) const
{
	return IsHalfEdgeOneWayCrossable<TLogic>(traverseLogicUserData, navHalfEdge, navFloorBlob, resultRawPtr, KY_NULL, costMultiplier);
}

template<class TLogic>
KY_INLINE bool NavHalfEdgeRawPtr::IsHalfEdgeOneWayCrossable(void* traverseLogicUserData, NavHalfEdge navHalfEdge, const NavFloorBlob* navFloorBlob,
	NavHalfEdgeRawPtr& resultRawPtr, const Vec3f* position, KyFloat32* costMultiplier) const
{
	return IsHalfEdgeOneWayCrossable<TLogic>(traverseLogicUserData, navHalfEdge, navFloorBlob, resultRawPtr, position, costMultiplier, typename TLogic::CanEnterNavTagMode());
}
template<class TLogic>
KY_INLINE bool NavHalfEdgeRawPtr::IsHalfEdgeOneWayCrossable(void* traverseLogicUserData, NavHalfEdge navHalfEdge, const NavFloorBlob* navFloorBlob, NavHalfEdgeRawPtr& resultRawPtr,
	const Vec3f* /*position*/, KyFloat32* costMultiplier, const LogicDoNotUseCanEnterNavTag&) const
{
	return IsHalfEdgeCrossable<TLogic>(traverseLogicUserData, navHalfEdge, navFloorBlob, resultRawPtr, costMultiplier);
}

template<class TLogic>
KY_INLINE bool NavHalfEdgeRawPtr::IsHalfEdgeCrossable(void* traverseLogicUserData, NavHalfEdge navHalfEdge, const NavFloorBlob* navFloorBlob,
	NavHalfEdgeRawPtr& resultRawPtr) const
{
	return IsHalfEdgeCrossable<TLogic>(traverseLogicUserData, navHalfEdge, navFloorBlob, resultRawPtr, KY_NULL);
}

template<class TLogic>
KY_INLINE bool NavHalfEdgeRawPtr::IsHalfEdgeCrossable(void* traverseLogicUserData, NavHalfEdge navHalfEdge, const NavFloorBlob* navFloorBlob,
	NavHalfEdgeRawPtr& resultRawPtr, KyFloat32* costMultiplier) const
{
	const NavHalfEdgeType halfEdgeType = navHalfEdge.GetHalfEdgeType();

	switch (halfEdgeType)
	{
	case EDGETYPE_OBSTACLE:
		return false;
	case EDGETYPE_PAIRED:
		{
			const NavHalfEdgeIdx pairHalfEdgeIdx = navHalfEdge.GetPairHalfEdgeIdx();
			resultRawPtr.Set(m_navFloorRawPtr, pairHalfEdgeIdx);
			const NavTriangleIdx pairTriangleIdx = NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(pairHalfEdgeIdx);
			return IsPairedEdgeCrossable<TLogic>(traverseLogicUserData, navFloorBlob, pairTriangleIdx, costMultiplier, typename TLogic::CostMultiplierUsage());
		}
	case EDGETYPE_CONNEXBOUNDARY:
		{
			const NavHalfEdgeIdx pairHalfEdgeIdx = navHalfEdge.GetPairHalfEdgeIdx();
			resultRawPtr.Set(m_navFloorRawPtr, pairHalfEdgeIdx);
			const NavTriangleIdx pairTriangleIdx = NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(pairHalfEdgeIdx);
			return IsConnexBoundaryCrossable<TLogic>(traverseLogicUserData, navFloorBlob, pairTriangleIdx, costMultiplier, typename TLogic::CostMultiplierUsage());
		}
	default :
		{
			const KyUInt32 boundaryEdgeIdx = navHalfEdge.GetBoundaryEdgeIdx();
			NavFloor* navFloor = GetNavFloor();
			NavHalfEdgeRawPtr& link = navFloor->GetLinksBuffer()[boundaryEdgeIdx];
			resultRawPtr = link;

			if (link.IsValid() == false)
				return false;

			return IsFloorOrCellLinkCrossable<TLogic>(traverseLogicUserData, link, costMultiplier, typename TLogic::CostMultiplierUsage());
		}
	}
}

template<class TLogic>
inline bool NavHalfEdgeRawPtr::IsPairedEdgeCrossable(void* /*traverseLogicUserData*/, const NavFloorBlob* /*navFloorBlob*/, NavTriangleIdx /*pairTriangleIdx*/,
	KyFloat32* /*costMultiplier*/, const LogicWithoutCostMultipler&) const
{
	// same navTag, the traversability do not change
	return true;
}
template<class TLogic>
inline bool NavHalfEdgeRawPtr::IsPairedEdgeCrossable(void* /*traverseLogicUserData*/, const NavFloorBlob* /*navFloorBlob*/, NavTriangleIdx /*pairTriangleIdx*/,
	KyFloat32* /*costMultiplier*/, const LogicWithCostMultiplerPerNavTag&) const
{
	// same navTag, the traversability and hte costMultiplier do not change
	return true;
}
template<class TLogic>
inline bool NavHalfEdgeRawPtr::IsPairedEdgeCrossable(void* traverseLogicUserData, const NavFloorBlob* /*navFloorBlob*/, NavTriangleIdx pairTriangleIdx,
	KyFloat32* costMultiplier, const LogicWithCostMultiplerPerTriangle&) const
{
	if (costMultiplier != KY_NULL) 
		*costMultiplier = 1.f;

	bool canTraverseResult = TLogic::CanTraverseNavTriangle(traverseLogicUserData, NavTriangleRawPtr(GetNavFloor(), pairTriangleIdx), costMultiplier);
	KY_LOG_ERROR_IF(costMultiplier != KY_NULL && *costMultiplier < 0.f, ("negative cost are not supported"));
	KY_LOG_ERROR_IF(costMultiplier != KY_NULL && *costMultiplier != *costMultiplier, ("costMultiplier is not a number !"));
	return canTraverseResult;
}



template<class TLogic>
inline bool NavHalfEdgeRawPtr::IsConnexBoundaryCrossable(void* traverseLogicUserData, const NavFloorBlob* navFloorBlob, NavTriangleIdx pairTriangleIdx,
	KyFloat32* /*costMultiplier*/, const LogicWithoutCostMultipler&) const
{
	return TLogic::CanTraverseNavTag(traverseLogicUserData, navFloorBlob->GetNavTag(pairTriangleIdx));
}

template<class TLogic>
inline bool NavHalfEdgeRawPtr::IsConnexBoundaryCrossable(void* traverseLogicUserData, const NavFloorBlob* navFloorBlob, NavTriangleIdx pairTriangleIdx,
	KyFloat32* costMultiplier, const LogicWithCostMultiplerPerNavTag&) const
{
	if (costMultiplier != KY_NULL) 
		*costMultiplier = 1.f;

	bool canTraverseResult = TLogic::CanTraverseNavTag(traverseLogicUserData, navFloorBlob->GetNavTag(pairTriangleIdx), costMultiplier);
	KY_LOG_ERROR_IF(costMultiplier != KY_NULL && *costMultiplier < 0.f, ("negative cost are not supported"));
	KY_LOG_ERROR_IF(costMultiplier != KY_NULL && *costMultiplier != *costMultiplier, ("costMultiplier is not a number !"));
	return canTraverseResult;
}

template<class TLogic>
inline bool NavHalfEdgeRawPtr::IsConnexBoundaryCrossable(void* traverseLogicUserData, const NavFloorBlob* /*navFloorBlob*/, NavTriangleIdx pairTriangleIdx,
	KyFloat32* costMultiplier, const LogicWithCostMultiplerPerTriangle&) const
{
	if (costMultiplier != KY_NULL) 
		*costMultiplier = 1.f;

	bool canTraverseResult = TLogic::CanTraverseNavTriangle(traverseLogicUserData, NavTriangleRawPtr(GetNavFloor(), pairTriangleIdx), costMultiplier);
	KY_LOG_ERROR_IF(costMultiplier != KY_NULL && *costMultiplier < 0.f, ("negative cost are not supported"));
	KY_LOG_ERROR_IF(costMultiplier != KY_NULL && *costMultiplier != *costMultiplier, ("costMultiplier is not a number !"));
	return canTraverseResult;
}


template<class TLogic>
inline bool NavHalfEdgeRawPtr::IsFloorOrCellLinkCrossable(void* traverseLogicUserData, const NavHalfEdgeRawPtr& link,
	KyFloat32* /*costMultiplier*/, const LogicWithoutCostMultipler&) const
{
	return TLogic::CanTraverseNavTag(traverseLogicUserData, link.GetNavTag());
}
template<class TLogic>
inline bool NavHalfEdgeRawPtr::IsFloorOrCellLinkCrossable(void* traverseLogicUserData, const NavHalfEdgeRawPtr& link,
	KyFloat32* costMultiplier, const LogicWithCostMultiplerPerNavTag&) const
{
	if (costMultiplier != KY_NULL) 
		*costMultiplier = 1.f;

	return TLogic::CanTraverseNavTag(traverseLogicUserData, link.GetNavTag(), costMultiplier);
}

template<class TLogic>
inline bool NavHalfEdgeRawPtr::IsFloorOrCellLinkCrossable(void* traverseLogicUserData, const NavHalfEdgeRawPtr& link,
	KyFloat32* costMultiplier,const LogicWithCostMultiplerPerTriangle&) const
{
	if (costMultiplier != KY_NULL) 
		*costMultiplier = 1.f;

	const NavTriangleIdx linkTriangleIdx = NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(link.GetHalfEdgeIdx());
	return TLogic::CanTraverseNavTriangle(traverseLogicUserData, NavTriangleRawPtr(link.GetNavFloor(), linkTriangleIdx), costMultiplier);
}


template<class TLogic>
inline bool NavHalfEdgeRawPtr::IsHalfEdgeOneWayCrossable(void* traverseLogicUserData, NavHalfEdge navHalfEdge, const NavFloorBlob* navFloorBlob, NavHalfEdgeRawPtr& resultRawPtr,
	const Vec3f* position, KyFloat32* costMultiplier, const LogicDoUseCanEnterNavTag&) const
{
	if (IsHalfEdgeCrossable<TLogic>(traverseLogicUserData, navHalfEdge, navFloorBlob, resultRawPtr, costMultiplier) == false)
		return false;

	const NavTag& currentNavTag = GetNavTag();
	const NavTag& comingNavTag = resultRawPtr.GetNavTag();
	if (currentNavTag == comingNavTag)
		return true;

	return TLogic::CanEnterNavTag(traverseLogicUserData, currentNavTag, comingNavTag,
		position != KY_NULL ? *position : GetMiddlePos3fOfNavHalfEdge());
}

template<class TLogic>
inline bool NavHalfEdgeRawPtr::IsStartVertexOnBorder(void* traverseLogicUserData) const
{
	if (IsHalfEdgeCrossable<TLogic>(traverseLogicUserData) == false)
		return true;

	Kaim::NavHalfEdgeRawPtr pairHalfEdgeRawPtr;
	Kaim::NavHalfEdgeRawPtr currentEdgeRawPtr;

	GetPairHalfEdgeRawPtr(pairHalfEdgeRawPtr);
	pairHalfEdgeRawPtr.GetNextHalfEdgeRawPtr(currentEdgeRawPtr);

	while (currentEdgeRawPtr.IsHalfEdgeCrossable<TLogic>(traverseLogicUserData) && (currentEdgeRawPtr != *this))
	{
		currentEdgeRawPtr.GetPairHalfEdgeRawPtr(pairHalfEdgeRawPtr);
		pairHalfEdgeRawPtr.GetNextHalfEdgeRawPtr(currentEdgeRawPtr);
	}

	return (currentEdgeRawPtr != *this);
}




KY_INLINE bool NavHalfEdgeRawPtr::IsStartVertexOnBorder() const
{
	return IsStartVertexOnBorder<DefaultTraverseLogic>(KY_NULL);
}

KY_INLINE bool NavHalfEdgeRawPtr::IsHalfEdgeCrossable(NavHalfEdgeRawPtr& resultRawPtr) const
{
	return IsHalfEdgeCrossable<DefaultTraverseLogic>(KY_NULL, GetNavFloorBlob(), resultRawPtr);
}

KY_INLINE bool NavHalfEdgeRawPtr::IsHalfEdgeCrossable() const
{
	return IsHalfEdgeCrossable<DefaultTraverseLogic>(KY_NULL);
}

template<class TLogic>
inline NavHalfEdgeRawPtr NavHalfEdgeRawPtr::GetNextNavHalfEdgeRawPtrAlongBorder(void* traverseLogicUserData) const
{
	KY_LOG_ERROR_IF(IsValid() == false, ("This function must not be called with an invalid instance of NavHalfEdgeId"));
	KY_DEBUG_ERRORN_IF(IsHalfEdgeCrossable<TLogic>(traverseLogicUserData), ("Input NavHalfEdgeId is not an Id of obstacle edge. You should not have called this function !"));

	NavHalfEdgeRawPtr resultRawPtr;
	GetNextHalfEdgeRawPtr(resultRawPtr);

	NavHalfEdgeRawPtr pairHalfEdgeRawPtr;
	while (resultRawPtr.IsHalfEdgeCrossable<TLogic>(traverseLogicUserData, pairHalfEdgeRawPtr))
	{
		pairHalfEdgeRawPtr.GetNextHalfEdgeRawPtr(resultRawPtr);
	}

	return resultRawPtr;
}

template<class TLogic>
inline NavHalfEdgeRawPtr NavHalfEdgeRawPtr::GetPrevNavHalfEdgeRawPtrAlongBorder(void* traverseLogicUserData) const
{
	KY_LOG_ERROR_IF(IsValid() == false, ("This function must not be called with an invalid instance of NavHalfEdgeId"));
	KY_DEBUG_ERRORN_IF(IsHalfEdgeCrossable<TLogic>(traverseLogicUserData), ("Input NavHalfEdgeId is not an Id of obstacle edge. You should not have called this function !"));

	NavHalfEdgeRawPtr resultRawPtr;
	GetPrevHalfEdgeRawPtr(resultRawPtr);

	NavHalfEdgeRawPtr pairHalfEdgeRawPtr;
	while (resultRawPtr.IsHalfEdgeCrossable<TLogic>(traverseLogicUserData, pairHalfEdgeRawPtr))
	{
		pairHalfEdgeRawPtr.GetPrevHalfEdgeRawPtr(resultRawPtr);
	}

	return resultRawPtr;
}


}