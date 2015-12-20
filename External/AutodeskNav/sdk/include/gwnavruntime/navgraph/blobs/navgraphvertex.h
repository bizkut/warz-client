/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavGraphVertex_H
#define Navigation_NavGraphVertex_H

#include "gwnavruntime/basesystem/logger.h"
#include "gwnavruntime/navgraph/navgraphtypes.h"
#include "gwnavruntime/database/navtag.h"


namespace Kaim
{

class NavGraphEdgeInfo
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	NavGraphEdgeInfo() : m_navTagIdx(NavTagIdx_Invalid) {}
	NavTagIdx m_navTagIdx;
};
inline void SwapEndianness(Endianness::Target e, NavGraphEdgeInfo& self)
{
	SwapEndianness(e, self.m_navTagIdx);
}

///////////////////////////////////////////////////////////////////////////////////////////
// NavGraphVertex
///////////////////////////////////////////////////////////////////////////////////////////

/*	This class represents a single vertex within a NavGraph */
class NavGraphVertex
{
	KY_CLASS_WITHOUT_COPY(NavGraphVertex)
public:

	NavGraphVertex();

	// vertex data
	KyUInt32               GetNeighborVertexCount() const;
	const Vec3f&           GetPosition()            const;
	NavTagIdx              GetNavTagIdx()           const;
	NavGraphVertexLinkType GetLinkType()            const;

	// edge data
	NavGraphVertexIdx       GetNeighborVertexIdx(KyUInt32 neighborIdx)     const;
	const NavGraphEdgeInfo& GetNavGraphEdgeInfo(KyUInt32 neighborIdx)      const;
	NavTagIdx               GetNavGraphEdgeNavTagIdx(KyUInt32 neighborIdx) const;

public:
	Vec3f m_position;
	NavTagIdx m_navTagIdx;
	NavGraphVertexLinkType m_linkType;

	// Edges around this vertex
	BlobArray<CompactNavGraphVertexIdx> m_neigbhourVertices;	/*< Used internally to iterate through outgoing \NavGraphEdges. Do not modify. */
	BlobArray<NavGraphEdgeInfo> m_edgesInfo;
};
inline void SwapEndianness(Endianness::Target e, NavGraphVertex& self)
{
	SwapEndianness(e, self.m_position);
	SwapEndianness(e, self.m_navTagIdx);
	SwapEndianness(e, self.m_linkType);
	SwapEndianness(e, self.m_neigbhourVertices);
	SwapEndianness(e, self.m_edgesInfo);
}

KY_INLINE NavGraphVertex::NavGraphVertex() : m_navTagIdx(NavTagIdx_Invalid), m_linkType(NavGraphVertexLinkType_LinkToNavMesh) {};

KY_INLINE KyUInt32               NavGraphVertex::GetNeighborVertexCount() const { return m_neigbhourVertices.GetCount(); }
KY_INLINE const Vec3f&           NavGraphVertex::GetPosition()            const { return m_position;                     }
KY_INLINE NavTagIdx              NavGraphVertex::GetNavTagIdx()           const { return m_navTagIdx;                    }
KY_INLINE NavGraphVertexLinkType NavGraphVertex::GetLinkType()            const { return m_linkType;                     }

KY_INLINE NavGraphVertexIdx NavGraphVertex::GetNeighborVertexIdx(KyUInt32 neighborIdx) const
{
	KY_ASSERT(neighborIdx < m_neigbhourVertices.GetCount());
	return (NavGraphVertexIdx)m_neigbhourVertices.GetValues()[neighborIdx];
}

KY_INLINE const NavGraphEdgeInfo& NavGraphVertex::GetNavGraphEdgeInfo(KyUInt32 neighborIdx) const
{
	KY_ASSERT(neighborIdx < m_edgesInfo.GetCount());
	return m_edgesInfo.GetValues()[neighborIdx];
}

KY_INLINE NavTagIdx NavGraphVertex::GetNavGraphEdgeNavTagIdx(KyUInt32 neighborIdx) const { return GetNavGraphEdgeInfo(neighborIdx).m_navTagIdx; }

} // namespace Kaim


#endif // Navigation_NavGraphVertex_H

