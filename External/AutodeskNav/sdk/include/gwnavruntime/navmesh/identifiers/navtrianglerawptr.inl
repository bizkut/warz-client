/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{


KY_INLINE NavTriangleRawPtr::NavTriangleRawPtr() : m_navFloorRawPtr(), m_triangleIdx(CompactNavTriangleIdx_MAXVAL) {}

KY_INLINE NavTriangleRawPtr::NavTriangleRawPtr(const NavFloorRawPtr& navFloorRawPtr, NavTriangleIdx triangleIdx) :
m_navFloorRawPtr(navFloorRawPtr), m_triangleIdx((CompactNavTriangleIdx)triangleIdx) {}

KY_INLINE void NavTriangleRawPtr::Set(const NavFloorRawPtr& navFloorRawPtr, NavTriangleIdx triangleIdx)
{
	m_navFloorRawPtr = navFloorRawPtr;
	m_triangleIdx = (CompactNavTriangleIdx)triangleIdx;
}

KY_INLINE NavTriangleRawPtr::NavTriangleRawPtr(NavFloor* navFloor, NavTriangleIdx triangleIdx) :
m_navFloorRawPtr(navFloor), m_triangleIdx((CompactNavTriangleIdx)triangleIdx) {}

KY_INLINE void NavTriangleRawPtr::Set(NavFloor* navFloor, NavTriangleIdx triangleIdx)
{
	m_navFloorRawPtr.Set(navFloor);
	m_triangleIdx = (CompactNavTriangleIdx)triangleIdx;
}

KY_INLINE bool NavTriangleRawPtr::IsValid() const { return m_navFloorRawPtr.IsValid() && m_triangleIdx != CompactNavTriangleIdx_MAXVAL; }
KY_INLINE void NavTriangleRawPtr::Invalidate() { m_navFloorRawPtr.Invalidate(); m_triangleIdx = CompactNavTriangleIdx_MAXVAL; }

KY_INLINE bool NavTriangleRawPtr::operator==(const NavTriangleRawPtr& rhs) const { return m_navFloorRawPtr == rhs.m_navFloorRawPtr && m_triangleIdx == rhs.m_triangleIdx;}
KY_INLINE bool NavTriangleRawPtr::operator!=(const NavTriangleRawPtr& rhs) const { return !(*this == rhs); }

KY_INLINE NavTriangleIdx NavTriangleRawPtr::GetTriangleIdx() const { return (NavTriangleIdx)m_triangleIdx; }

KY_INLINE NavFloor*           NavTriangleRawPtr::GetNavFloor()       const { return m_navFloorRawPtr.GetNavFloor();              }
KY_INLINE const NavFloorBlob* NavTriangleRawPtr::GetNavFloorBlob()   const { return m_navFloorRawPtr.GetNavFloorBlob();          }
KY_INLINE const CellPos&      NavTriangleRawPtr::GetCellPos()        const { return m_navFloorRawPtr.GetCellPos();               }
KY_INLINE const NavTag&       NavTriangleRawPtr::GetNavTag()         const { return GetNavFloorBlob()->GetNavTag(m_triangleIdx); }

KY_INLINE void NavTriangleRawPtr::GetVerticesPos3f(Triangle3f& triangle3f) const
{
	GetVerticesPos3f(triangle3f.A, triangle3f.B, triangle3f.C);
}


template<class TLogic>
KY_INLINE bool NavTriangleRawPtr::CanBeTraversed(void* traverseLogicUserData, KyFloat32* costMultiplier) const
{
	return CanBeTraversed<TLogic>(traverseLogicUserData, costMultiplier, typename TLogic::CostMultiplierUsage());
}

template<class TLogic>
KY_INLINE bool NavTriangleRawPtr::CanBeTraversed(void* traverseLogicUserData, KyFloat32* /*costMultiplier*/, const LogicWithoutCostMultipler&) const
{
	return TLogic::CanTraverseNavTag(traverseLogicUserData, GetNavTag());
}
template<class TLogic>
KY_INLINE bool NavTriangleRawPtr::CanBeTraversed(void* traverseLogicUserData, KyFloat32* costMultiplier, const LogicWithCostMultiplerPerNavTag&) const
{
	if (costMultiplier != KY_NULL) 
		*costMultiplier = 1.f;

	bool canTraverseResult = TLogic::CanTraverseNavTag(traverseLogicUserData, GetNavTag(), costMultiplier);
	KY_LOG_ERROR_IF(costMultiplier != KY_NULL && *costMultiplier < 0.f, ("negative cost are not supported"));
	KY_LOG_ERROR_IF(costMultiplier != KY_NULL && *costMultiplier != *costMultiplier, ("costMultiplier is not a number !"));
	return canTraverseResult;
}

template<class TLogic>
KY_INLINE bool NavTriangleRawPtr::CanBeTraversed(void* traverseLogicUserData, KyFloat32* costMultiplier, const LogicWithCostMultiplerPerTriangle&) const
{
	if (costMultiplier != KY_NULL) 
		*costMultiplier = 1.f;

	bool canTraverseResult = TLogic::CanTraverseNavTriangle(traverseLogicUserData, *this, costMultiplier);
	KY_LOG_ERROR_IF(costMultiplier != KY_NULL && *costMultiplier < 0.f, ("negative cost are not supported"));
	KY_LOG_ERROR_IF(costMultiplier != KY_NULL && *costMultiplier != *costMultiplier, ("costMultiplier is not a number !"));
	return canTraverseResult;
}

}

