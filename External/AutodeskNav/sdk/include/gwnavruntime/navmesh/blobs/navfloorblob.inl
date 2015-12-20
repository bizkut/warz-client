/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{
KY_INLINE bool NavFloorAABB::DoesIntersect(const NavFloorAABB& other) const
{
	const KyInt32 operand1 = Isel((KyInt32)other.m_max.x - (KyInt32)m_min.x      , 1, 0);
	const KyInt32 operand2 = Isel((KyInt32)m_max.x       - (KyInt32)other.m_min.x, 1, 0);
	const KyInt32 operand3 = Isel((KyInt32)other.m_max.y - (KyInt32)m_min.y      , 1, 0);
	const KyInt32 operand4 = Isel((KyInt32)m_max.y       - (KyInt32)other.m_min.y, 1, 0);

	return (operand1 & operand2 & operand3 & operand4) != 0;
	/*return other.m_max.x >= m_min.x && m_max.x >= other.m_min.x && other.m_max.y >= m_min.y && m_max.y >= other.m_min.y;*/
}

KY_INLINE NavFloorBlob::NavFloorBlob() {}

KY_INLINE NavTriangleIdx NavFloorBlob::NavHalfEdgeIdxToTriangleIdx(NavHalfEdgeIdx idx)         { return FastDivisionBy3(idx);     }
KY_INLINE NavHalfEdgeIdx NavFloorBlob::NavTriangleIdxToFirstNavHalfEdgeIdx(NavTriangleIdx idx) { return idx * 3;                  }
KY_INLINE NavHalfEdgeIdx NavFloorBlob::NavHalfEdgeIdxToFirstNavHalfEdgeIdx(NavTriangleIdx idx) { return 3 * FastDivisionBy3(idx); }
KY_INLINE NavHalfEdgeIdx NavFloorBlob::NavTriangleIdxToNavHalfEdgeIdx(NavTriangleIdx idx, KyInt32 halfEdgeNumber) { return NavTriangleIdxToFirstNavHalfEdgeIdx(idx) + halfEdgeNumber; }
KY_INLINE NavHalfEdgeIdx NavFloorBlob::NavHalfEdgeIdxToNextNavHalfEdgeIdx(NavHalfEdgeIdx idx)
{
	static const NavHalfEdgeIdx s_tableForGetTheNextIdx[3] = {3, 3, 0};
	return (idx + s_tableForGetTheNextIdx[FastModuloBy3(idx)]) - 2;
}

KY_INLINE NavHalfEdgeIdx NavFloorBlob::NavHalfEdgeIdxToPrevNavHalfEdgeIdx(NavHalfEdgeIdx idx)
{
	static const NavHalfEdgeIdx s_tableForGetThePrevIdx[3] = {0, 3, 3};
	return (idx + 2) - s_tableForGetThePrevIdx[FastModuloBy3(idx)];
}

KY_INLINE KyUInt32 NavFloorBlob::NavHalfEdgeIdxToHalfEdgeNumberInTriangle(NavHalfEdgeIdx idx) { return FastModuloBy3(idx); }

KY_INLINE KyUInt32 NavFloorBlob::GetNavVertexCount()       const { return m_navVertices.GetCount();              }
KY_INLINE KyUInt32 NavFloorBlob::GetNavHalfEdgeCount()     const { return m_navHalfEdges.GetCount();             }
KY_INLINE KyUInt32 NavFloorBlob::GetNavTriangleCount()     const { return m_triangleConnexIndices.GetCount();    }
KY_INLINE KyUInt32 NavFloorBlob::GetNavConnexCount()       const { return m_connexNavTag.GetCount();             }
KY_INLINE KyUInt32 NavFloorBlob::GetNavFloorLinkCount()    const { return m_stitch1To1ToHalfEdgeInFloor.GetNavFloorLinkCount();    }
KY_INLINE KyUInt32 NavFloorBlob::GetStitch1To1EdgeCount() const { return m_stitch1To1ToHalfEdgeInFloor.GetStitch1To1EdgeCount(); }

KY_INLINE const FloorAltitudeRange& NavFloorBlob::GetFloorAltitudeRange() const { return m_altitudeRange; }

KY_INLINE const NavTag& NavFloorBlob::GetNavTag(NavTriangleIdx navTriangleIdx) const
{
	KY_DEBUG_ASSERTN(navTriangleIdx < GetNavTriangleCount(), ("Invalid vertex index"));
	return m_connexNavTag.GetValues()[NavTriangleIdxToNavConnexIdx(navTriangleIdx)];
}

KY_INLINE const NavVertex& NavFloorBlob::GetNavVertex(const NavVertexIdx idx) const
{
	KY_DEBUG_ASSERTN(idx < GetNavVertexCount(), ("Invalid vertex index"));
	return m_navVertices.GetValues()[idx];
}

KY_INLINE KyFloat32 NavFloorBlob::GetNavVertexAltitude(const NavVertexIdx idx) const
{
	KY_DEBUG_ASSERTN(idx < GetNavVertexCount(), ("Invalid vertex index"));
	return m_navVerticesAltitudes.GetValues()[idx];
}

KY_INLINE const NavHalfEdge& NavFloorBlob::GetNavHalfEdge(NavHalfEdgeIdx idx) const
{
	KY_DEBUG_ASSERTN(idx < GetNavHalfEdgeCount(), ("Invalid half-edge index"));
	return m_navHalfEdges.GetValues()[idx];
}

KY_INLINE NavHalfEdgeType         NavFloorBlob::GetHalfEdgeType(NavHalfEdgeIdx idx)                    const { return GetNavHalfEdge(idx).GetHalfEdgeType();                   }
KY_INLINE NavHalfEdgeObstacleType NavFloorBlob::GetHalfEdgeObstacleType(NavHalfEdgeIdx idx)            const { return GetNavHalfEdge(idx).GetHalfEdgeObstacleType();           }
KY_INLINE const NavVertex&        NavFloorBlob::NavHalfEdgeIdxToStartNavVertex(NavHalfEdgeIdx idx)     const { return GetNavVertex(NavHalfEdgeIdxToStartNavVertexIdx(idx));    }
KY_INLINE NavVertexIdx            NavFloorBlob::NavHalfEdgeIdxToStartNavVertexIdx(NavHalfEdgeIdx idx)  const { return GetNavHalfEdge(idx).GetStartVertexIdx();                 }
KY_INLINE const NavVertex&        NavFloorBlob::NavHalfEdgeIdxToEndNavVertex(NavHalfEdgeIdx idx)       const { return GetNavVertex(NavHalfEdgeIdxToEndNavVertexIdx(idx));      }
KY_INLINE const NavVertex&        NavFloorBlob::NavHalfEdgeIdxToThirdNavVertex(NavHalfEdgeIdx idx)     const { return GetNavVertex(NavHalfEdgeIdxToThirdNavVertexIdx(idx));    }
KY_INLINE const NavHalfEdge&      NavFloorBlob::NavHalfEdgeIdxToPairNavHalfEdge(NavHalfEdgeIdx idx)    const { return GetNavHalfEdge(NavHalfEdgeIdxToPairNavHalfEdgeIdx(idx)); }
KY_INLINE NavHalfEdgeIdx          NavFloorBlob::NavHalfEdgeIdxToPairNavHalfEdgeIdx(NavHalfEdgeIdx idx) const { return GetNavHalfEdge(idx).GetPairHalfEdgeIdx();                }

KY_INLINE NavVertexIdx       NavFloorBlob::NavHalfEdgeIdxToEndNavVertexIdx(NavHalfEdgeIdx idx)    const { return NavHalfEdgeIdxToStartNavVertexIdx(NavHalfEdgeIdxToNextNavHalfEdgeIdx(idx)); }
KY_INLINE NavVertexIdx       NavFloorBlob::NavHalfEdgeIdxToThirdNavVertexIdx(NavHalfEdgeIdx idx)  const { return NavHalfEdgeIdxToStartNavVertexIdx(NavHalfEdgeIdxToPrevNavHalfEdgeIdx(idx)); }
KY_INLINE const NavHalfEdge& NavFloorBlob::NavHalfEdgeIdxToNextNavHalfEdge(NavHalfEdgeIdx idx)    const { return GetNavHalfEdge(NavHalfEdgeIdxToNextNavHalfEdgeIdx(idx));                    }
KY_INLINE const NavHalfEdge& NavFloorBlob::NavHalfEdgeIdxToPrevNavHalfEdge(NavHalfEdgeIdx idx)    const { return GetNavHalfEdge(NavHalfEdgeIdxToPrevNavHalfEdgeIdx(idx));                    }

KY_INLINE const NavHalfEdge& NavFloorBlob::NavTriangleIdxToNavHalfEdge(NavTriangleIdx idx, const KyInt32 halfEdgeNumber) const { return GetNavHalfEdge(NavTriangleIdxToNavHalfEdgeIdx(idx, halfEdgeNumber)); }

KY_INLINE void NavFloorBlob::NavTriangleIdxToNavVertexIndices(NavTriangleIdx triangleIdx, NavVertexIdx& v0Idx, NavVertexIdx& v1Idx, NavVertexIdx& v2Idx) const
{
	KY_DEBUG_ASSERTN(triangleIdx < GetNavTriangleCount(), ("Invalid triangle index"));

	const NavHalfEdge* const navHalfEdges = m_navHalfEdges.GetValues();
	const NavHalfEdgeIdx firstNavHalfEdgeIdx = NavTriangleIdxToFirstNavHalfEdgeIdx(triangleIdx);
	v0Idx = navHalfEdges[firstNavHalfEdgeIdx + 0].GetStartVertexIdx();
	v1Idx = navHalfEdges[firstNavHalfEdgeIdx + 1].GetStartVertexIdx();
	v2Idx = navHalfEdges[firstNavHalfEdgeIdx + 2].GetStartVertexIdx();
}

KY_INLINE const NavVertex& NavFloorBlob::NavTriangleIdxToNavVertex(NavTriangleIdx idx, const KyInt32 vertexNumber) const { return GetNavVertex(NavTriangleIdxToVertexIdx(idx, vertexNumber)); }
KY_INLINE NavVertexIdx     NavFloorBlob::NavTriangleIdxToVertexIdx(NavTriangleIdx idx, const KyInt32 vertexNumber) const
{
	KY_DEBUG_ASSERTN(idx < GetNavTriangleCount(), ("Invalid triangle index"));
	KY_DEBUG_ASSERTN(vertexNumber > -1 && vertexNumber < 3, ("Invalid vertex number in triangle"));

	return NavHalfEdgeIdxToStartNavVertexIdx(idx * 3 + vertexNumber);
}

KY_INLINE NavConnexIdx NavFloorBlob::NavTriangleIdxToNavConnexIdx(NavTriangleIdx idx) const
{
	KY_DEBUG_ASSERTN(idx < GetNavTriangleCount(), ("Invalid triangle index"));
	return (NavConnexIdx)m_triangleConnexIndices.GetValues()[idx];
}

KY_INLINE bool NavFloorBlob::IsPointInsideFloor(const CoordPos64& coordPos64, const CoordPos64& cellOrigin) const
{
	const CoordBox64 floorIntegerBox(cellOrigin + m_navFloorAABB.m_min.GetCoordPos64(), cellOrigin + m_navFloorAABB.m_max.GetCoordPos64());
	const KyInt64 operand1 = Lsel(floorIntegerBox.Min().x - coordPos64.x, 0, 1); // operand1 == 1.f if pos.x > cellBox.m_min.x
	const KyInt64 operand2 = Lsel(floorIntegerBox.Max().x - coordPos64.x, 1, 0); // operand2 == 1.f if cellBox.m_max.x >= pos.x
	const KyInt64 operand3 = Lsel(coordPos64.y - floorIntegerBox.Min().y, 1, 0); // operand3 == 1.f if pos.y >= cellBox.m_min.y
	const KyInt64 operand4 = Lsel(coordPos64.y - floorIntegerBox.Max().y, 0, 1); // operand4 == 1.f if cellBox.m_max.y > pos.y

	return (operand1 & operand2 & operand3 & operand4) != 0;
}

KY_INLINE bool NavFloorBlob::IsPointInsideFloor(const CoordPos& coordPosInCell) const
{
	const CoordBox floorIntegerBox(m_navFloorAABB.m_min.GetCoordPos(), m_navFloorAABB.m_max.GetCoordPos());
	const KyInt32 operand1 = Isel(floorIntegerBox.Min().x - coordPosInCell.x, 0, 1); // operand1 == 1.f if pos.x > cellBox.m_min.x
	const KyInt32 operand2 = Isel(floorIntegerBox.Max().x - coordPosInCell.x, 1, 0); // operand2 == 1.f if cellBox.m_max.x >= pos.x
	const KyInt32 operand3 = Isel(coordPosInCell.y - floorIntegerBox.Min().y, 1, 0); // operand3 == 1.f if pos.y >= cellBox.m_min.y
	const KyInt32 operand4 = Isel(coordPosInCell.y - floorIntegerBox.Max().y, 0, 1); // operand4 == 1.f if cellBox.m_max.y > pos.y

	return (operand1 & operand2 & operand3 & operand4) != 0;
}

}