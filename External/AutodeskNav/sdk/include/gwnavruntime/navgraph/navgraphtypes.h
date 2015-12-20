/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LASI - secondary contact: NOBODY
#ifndef Navigation_NavGraphTypes_H
#define Navigation_NavGraphTypes_H

#include "gwnavruntime/math/vec3f.h"

namespace Kaim
{

// This enum is used in some functions coming with a NavGraphEdgeRawPtr or a NavGraphEdgePtr to tell whether or not
// the opposite NavGraphEdge (going from endPos  to startPos in the same navGraph) should be also considered if it exists.
enum NavGraphEdgeDirection
{
	NAVGRAPHEDGE_MONODIRECTIONAL = 0,
	NAVGRAPHEDGE_BIDIRECTIONAL   = 1
};

/// Defines a type for a tag that determines whether or not a vertex in a NavGraph should be connected to the NavMesh.  
typedef KyUInt16 NavGraphVertexLinkType;
static const NavGraphVertexLinkType NavGraphVertexLinkType_NoLink        = 0; ///< Indicates that this vertex should not be connected to the NavMesh.
static const NavGraphVertexLinkType NavGraphVertexLinkType_LinkToNavMesh = 1; ///< Indicates that this vertex should be connected to the NavMesh. If the vertex is outside, we try to link it inside

typedef KyUInt16 NavGraphVertexType;
static const NavGraphVertexType NavGraphVertexType_Unset                         = 0;
static const NavGraphVertexType NavGraphVertexType_InGraphVertex                 = 1;
static const NavGraphVertexType NavGraphVertexType_AddedVertexForFromOutsideLink = 2;


// index of a StitchedGraph in the buffer of ActiveData
typedef KyInt32 NavGraphIdxInActiveData;
static const NavGraphIdxInActiveData NavGraphIdxInActiveData_Invalid = KyInt32MAXVAL;

typedef KyUInt16 CompactNavGraphVertexIdx;
static const CompactNavGraphVertexIdx CompactNavGraphVertexIdx_MAXVAL = 0xFFFF;

typedef KyUInt16 CompactNavGraphNeighborIdx;
static const CompactNavGraphNeighborIdx CompactNavGraphNeighborIdx_MAXVAL = 0xFFFF;

// vertex Index in the NavGraph
typedef KyUInt32 NavGraphVertexIdx; ///< An index that uniquely identifies a single vertex within the set of vertices owned by a NavGraph.  
static const NavGraphVertexIdx NavGraphVertexIdx_Invalid = (NavGraphVertexIdx)CompactNavGraphVertexIdx_MAXVAL; ///< Represents an invalid #NavGraphVertexIdx.  

struct NavGraphEdgeSmartIdx
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	NavGraphEdgeSmartIdx() : m_startVertexIdx(CompactNavGraphVertexIdx_MAXVAL), m_neighborIdx(CompactNavGraphNeighborIdx_MAXVAL) {}
	NavGraphEdgeSmartIdx(const NavGraphEdgeSmartIdx& smartIdx) : m_startVertexIdx(smartIdx.m_startVertexIdx), m_neighborIdx(smartIdx.m_neighborIdx) {}
	NavGraphEdgeSmartIdx(NavGraphVertexIdx startVertexIdx, KyUInt32 neighborIdx) : m_startVertexIdx((CompactNavGraphVertexIdx)startVertexIdx), m_neighborIdx((CompactNavGraphNeighborIdx)neighborIdx) {}

	bool IsValid() const;
	void Invalidate();

	bool operator < (const NavGraphEdgeSmartIdx& rhs) const;
	bool operator ==(const NavGraphEdgeSmartIdx& rhs) const;

	NavGraphVertexIdx GetStartVertexIdx() const;
	KyUInt32 GetNeighborVertexIdx()      const;
public:
	CompactNavGraphVertexIdx m_startVertexIdx;
	CompactNavGraphNeighborIdx m_neighborIdx;
};

KY_INLINE NavGraphVertexIdx NavGraphEdgeSmartIdx::GetStartVertexIdx()     const { return (NavGraphVertexIdx)m_startVertexIdx; }
KY_INLINE KyUInt32          NavGraphEdgeSmartIdx::GetNeighborVertexIdx() const { return (KyUInt32)m_neighborIdx;            }
KY_INLINE bool NavGraphEdgeSmartIdx::IsValid() const { return m_startVertexIdx != CompactNavGraphVertexIdx_MAXVAL && m_neighborIdx != CompactNavGraphNeighborIdx_MAXVAL;}
KY_INLINE void NavGraphEdgeSmartIdx::Invalidate() { m_startVertexIdx = CompactNavGraphVertexIdx_MAXVAL; m_neighborIdx = CompactNavGraphNeighborIdx_MAXVAL; }

KY_INLINE bool NavGraphEdgeSmartIdx::operator < (const NavGraphEdgeSmartIdx& rhs) const
{
	return GetStartVertexIdx() < rhs.GetStartVertexIdx() || (GetStartVertexIdx() == rhs.GetStartVertexIdx() && GetNeighborVertexIdx() < rhs.GetNeighborVertexIdx());
}
KY_INLINE bool NavGraphEdgeSmartIdx::operator ==(const NavGraphEdgeSmartIdx& rhs) const
{
	return GetStartVertexIdx() == rhs.GetStartVertexIdx() && GetNeighborVertexIdx() == rhs.GetNeighborVertexIdx();
}

} // namespace Kaim

#endif // Navigation_NavGraphTypes_H

