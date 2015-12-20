/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{

KY_INLINE NavGraphEdgeRawPtr::NavGraphEdgeRawPtr() {}
KY_INLINE NavGraphEdgeRawPtr::NavGraphEdgeRawPtr(const NavGraphVertexRawPtr& navGraphVertexRawPtr, KyUInt32 neighborIndex) :
m_navGraphRawPtr(navGraphVertexRawPtr.m_navGraphRawPtr), m_edgeSmartIdx(navGraphVertexRawPtr.GetNavGraphVertexIdx(), neighborIndex) {}
KY_INLINE NavGraphEdgeRawPtr::NavGraphEdgeRawPtr(const NavGraphRawPtr& navGraphRawPtr, NavGraphEdgeSmartIdx edgeSmartIdx) :
m_navGraphRawPtr(navGraphRawPtr), m_edgeSmartIdx(edgeSmartIdx) {}

KY_INLINE bool NavGraphEdgeRawPtr::IsValid() const { return m_navGraphRawPtr.IsValid() && m_edgeSmartIdx.IsValid(); }
KY_INLINE void NavGraphEdgeRawPtr::Invalidate() { m_navGraphRawPtr.Invalidate(); m_edgeSmartIdx.Invalidate(); }

KY_INLINE bool NavGraphEdgeRawPtr::operator ==(const NavGraphEdgeRawPtr& rhs) const { return m_navGraphRawPtr == rhs.m_navGraphRawPtr && m_edgeSmartIdx == rhs.m_edgeSmartIdx; }
KY_INLINE bool NavGraphEdgeRawPtr::operator !=(const NavGraphEdgeRawPtr& rhs) const { return !(*this == rhs); }
KY_INLINE bool NavGraphEdgeRawPtr::operator < (const NavGraphEdgeRawPtr& rhs) const { return m_navGraphRawPtr < rhs.m_navGraphRawPtr || (m_navGraphRawPtr == rhs.m_navGraphRawPtr && m_edgeSmartIdx < rhs.m_edgeSmartIdx); }
KY_INLINE bool NavGraphEdgeRawPtr::operator <=(const NavGraphEdgeRawPtr& rhs) const { return !(rhs < *this); }

KY_INLINE const NavGraphBlob* NavGraphEdgeRawPtr::GetNavGraphBlob()                const { return m_navGraphRawPtr.GetNavGraphBlob();     }
KY_INLINE NavGraph*           NavGraphEdgeRawPtr::GetNavGraph()                    const { return m_navGraphRawPtr.GetNavGraph();         }
KY_INLINE NavGraphVertexIdx   NavGraphEdgeRawPtr::GetStartVertexIdx()              const { return m_edgeSmartIdx.GetStartVertexIdx();     }
KY_INLINE KyUInt32            NavGraphEdgeRawPtr::GetEdgeNumberAroundStartVertex() const { return m_edgeSmartIdx.GetNeighborVertexIdx(); }
KY_INLINE const Vec3f&        NavGraphEdgeRawPtr::GetStartNavGraphVertexPosition() const { return GetStartNavGraphVertex().GetPosition(); }
KY_INLINE const Vec3f&        NavGraphEdgeRawPtr::GetEndNavGraphVertexPosition()   const { return GetEndNavGraphVertex().GetPosition();   }

KY_INLINE NavGraphVertexRawPtr NavGraphEdgeRawPtr::GetStartNavGraphVertexRawPtr() const { return NavGraphVertexRawPtr(m_navGraphRawPtr, m_edgeSmartIdx.GetStartVertexIdx()); }
KY_INLINE const NavGraphVertex& NavGraphEdgeRawPtr::GetStartNavGraphVertex() const
{
	return GetNavGraphBlob()->GetNavGraphVertex(m_edgeSmartIdx.GetStartVertexIdx());
}

KY_INLINE NavGraphVertexRawPtr NavGraphEdgeRawPtr::GetEndNavGraphVertexRawPtr() const
{
	const NavGraphVertex& startPointGrahVertex = GetStartNavGraphVertex();
	const NavGraphVertexIdx endVertexIdx = startPointGrahVertex.GetNeighborVertexIdx(m_edgeSmartIdx.GetNeighborVertexIdx());
	return NavGraphVertexRawPtr(m_navGraphRawPtr, endVertexIdx);
}

KY_INLINE NavGraphVertexIdx NavGraphEdgeRawPtr::GetEndNavGraphVertexIdx() const
{
	const NavGraphVertex& startPointGrahVertex = GetStartNavGraphVertex();
	return startPointGrahVertex.GetNeighborVertexIdx(m_edgeSmartIdx.GetNeighborVertexIdx());
}

KY_INLINE const NavGraphVertex& NavGraphEdgeRawPtr::GetEndNavGraphVertex() const
{
	return GetNavGraphBlob()->GetNavGraphVertex(GetEndNavGraphVertexIdx());
}

KY_INLINE const NavTag& NavGraphEdgeRawPtr::GetNavTag() const
{
	const NavGraphBlob* navGraphBlob = GetNavGraphBlob();
	const NavTagIdx navTagIdx =  navGraphBlob->GetNavGraphEdgeNavTagIdx(m_edgeSmartIdx);
	return navGraphBlob->GetNavTag(navTagIdx);
}



template<class TLogic>
KY_INLINE bool NavGraphEdgeRawPtr::CanBeTraversed(void* traverseLogicUserData, KyFloat32* costMultiplier) const
{
	return CanBeTraversed<TLogic>(traverseLogicUserData, costMultiplier, typename TLogic::CostMultiplierUsage());
}

template<class TLogic>
KY_INLINE bool NavGraphEdgeRawPtr::CanBeTraversed(void* traverseLogicUserData, KyFloat32* /*costMultiplier*/, const LogicWithoutCostMultipler&) const
{
	return TLogic::CanTraverseNavTag(traverseLogicUserData, GetNavTag());
}
template<class TLogic>
KY_INLINE bool NavGraphEdgeRawPtr::CanBeTraversed(void* traverseLogicUserData, KyFloat32* costMultiplier, const LogicWithCostMultiplerPerNavTag&) const
{
	if (costMultiplier != KY_NULL) 
		*costMultiplier = 1.f;

	bool canTraverseResult =  TLogic::CanTraverseNavTag(traverseLogicUserData, GetNavTag(), costMultiplier);
	KY_LOG_ERROR_IF(costMultiplier != KY_NULL && *costMultiplier < 0.f, ("negative cost are not supported"));
	KY_LOG_ERROR_IF(costMultiplier != KY_NULL && *costMultiplier != *costMultiplier, ("costMultiplier is not a number !"));
	return canTraverseResult;
}
template<class TLogic>
KY_INLINE bool NavGraphEdgeRawPtr::CanBeTraversed(void* traverseLogicUserData, KyFloat32* costMultiplier, const LogicWithCostMultiplerPerTriangle&) const
{
	if (costMultiplier != KY_NULL) 
		*costMultiplier = 1.f;

	bool canTraverseResult =  TLogic::CanTraverseNavGraphEdge(traverseLogicUserData, *this, costMultiplier);
	KY_LOG_ERROR_IF(costMultiplier != KY_NULL && *costMultiplier < 0.f, ("negative cost are not supported"));
	KY_LOG_ERROR_IF(costMultiplier != KY_NULL && *costMultiplier != *costMultiplier, ("costMultiplier is not a number !"));
	return canTraverseResult;
}

}
