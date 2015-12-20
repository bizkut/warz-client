/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavGraphBlob_H
#define Navigation_NavGraphBlob_H

#include "gwnavruntime/navgraph/blobs/navgraphvertex.h"
#include "gwnavruntime/navdata/navdatablobcategory.h"
#include "gwnavruntime/math/box3f.h"


namespace Kaim
{

/// The NavGraphBlob contains the static data of a NavGraph.
class NavGraphBlob
{
	KY_ROOT_BLOB_CLASS(NavData, NavGraphBlob, 0)
	KY_CLASS_WITHOUT_COPY(NavGraphBlob)
public:
	NavGraphBlob();

	/* Returns true if the NavGraph contains no vertices at all. */
	bool IsEmpty() const;

	KyUInt32 GetVertexCount()          const;
	KyUInt32 GetVertexToConnectCount() const;
	KyUInt32 GetEdgeCount()            const;
	KyUInt32 GetNavTagCount()          const;

	// vertex data
	const NavGraphVertex&  GetNavGraphVertex(NavGraphVertexIdx vertexIdx)               const;
	const Vec3f&           GetNavGraphVertexPosition(NavGraphVertexIdx vertexIdx)       const;
	NavTagIdx              GetNavGraphVertexNavTagIdx(NavGraphVertexIdx vertexIdx)      const;
	NavGraphVertexLinkType GetNavGraphVertexConnectionType(NavGraphVertexIdx vertexIdx) const;

	// edge data
	const NavGraphEdgeInfo& GetNavGraphEdgeInfo(const NavGraphEdgeSmartIdx& edgeIdxInGraph)      const;
	NavTagIdx               GetNavGraphEdgeNavTagIdx(const NavGraphEdgeSmartIdx& edgeIdxInGraph) const;

	// navTag
	const NavTag& GetNavTag(NavTagIdx navTagIdx) const;

	// compute graph aabb
	Box3f ComputeAABB() const;
public:
	KyUInt32 m_visualDebugId; // unused since NavGraphBlobs were sent into NavGraphArray blob from a NavData
	BlobArray<char> m_name; // optional, only for Visual Debug
	BlobArray<NavGraphVertex> m_navGraphVertices;
	BlobArray<NavTag> m_navTags;
	KyUInt32 m_edgeCount;
	KyUInt32 m_vertexToConnectCount;
};
inline void SwapEndianness(Endianness::Target e, NavGraphBlob& self)
{
	SwapEndianness(e, self.m_visualDebugId);
	SwapEndianness(e, self.m_name);
	SwapEndianness(e, self.m_navGraphVertices);
	SwapEndianness(e, self.m_navTags);
	SwapEndianness(e, self.m_edgeCount);
	SwapEndianness(e, self.m_vertexToConnectCount);
}

KY_INLINE NavGraphBlob::NavGraphBlob() {}
KY_INLINE bool NavGraphBlob::IsEmpty() const { return GetVertexCount() == 0; }

KY_INLINE KyUInt32               NavGraphBlob::GetEdgeCount()                                               const { return m_edgeCount;                                 }
KY_INLINE KyUInt32               NavGraphBlob::GetVertexCount()                                             const { return m_navGraphVertices.GetCount();               }
KY_INLINE KyUInt32               NavGraphBlob::GetVertexToConnectCount()                                    const { return m_vertexToConnectCount;                      }
KY_INLINE KyUInt32               NavGraphBlob::GetNavTagCount()                                             const { return m_navTags.GetCount();                        }
KY_INLINE const Vec3f&           NavGraphBlob::GetNavGraphVertexPosition(NavGraphVertexIdx vertexIdx)       const { return GetNavGraphVertex(vertexIdx).GetPosition();  }
KY_INLINE NavTagIdx              NavGraphBlob::GetNavGraphVertexNavTagIdx(NavGraphVertexIdx vertexIdx)      const { return GetNavGraphVertex(vertexIdx).GetNavTagIdx(); }
KY_INLINE NavGraphVertexLinkType NavGraphBlob::GetNavGraphVertexConnectionType(NavGraphVertexIdx vertexIdx) const { return GetNavGraphVertex(vertexIdx).GetLinkType();  }

KY_INLINE NavTagIdx NavGraphBlob::GetNavGraphEdgeNavTagIdx(const NavGraphEdgeSmartIdx& edgeIdxInGraph) const { return GetNavGraphEdgeInfo(edgeIdxInGraph).m_navTagIdx; }

KY_INLINE const NavGraphVertex& NavGraphBlob::GetNavGraphVertex(NavGraphVertexIdx vertexIdx) const
{
	KY_ASSERT(vertexIdx < m_navGraphVertices.GetCount());
	return m_navGraphVertices.GetValues()[vertexIdx];
}

KY_INLINE const NavGraphEdgeInfo& NavGraphBlob::GetNavGraphEdgeInfo(const NavGraphEdgeSmartIdx& edgeIdxInGraph) const
{
	return GetNavGraphVertex(edgeIdxInGraph.GetStartVertexIdx()).GetNavGraphEdgeInfo(edgeIdxInGraph.GetNeighborVertexIdx());
}

KY_INLINE const NavTag& NavGraphBlob::GetNavTag(NavTagIdx navTagIdx) const
{
	KY_ASSERT(navTagIdx < m_navTags.GetCount());
	return m_navTags.GetValues()[navTagIdx];
}

KY_INLINE Kaim::Box3f NavGraphBlob::ComputeAABB() const
{
	Box3f aabb;
	const NavGraphVertex* vertices = m_navGraphVertices.GetValues();
	for (KyUInt32 i = 0; i < m_navGraphVertices.GetCount(); ++i)
	{
		aabb.ExpandByVec3(vertices[i].m_position);
	}
	return aabb;
}

} // namespace Kaim


#endif // Navigation_NavGraphBlob_H

