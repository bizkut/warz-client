/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: LASI
#ifndef Navigation_NavFloorLinkInfo_H
#define Navigation_NavFloorLinkInfo_H

#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/navmesh/blobs/navvertex.h"

namespace Kaim
{

class Stitch1To1Edge
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	Stitch1To1Edge() {}
	KY_INLINE KyUInt32        GetStartVertexIdx() const { return (KyUInt32) (m_edgeData & 0x00000FFF)       ; }
	KY_INLINE KyUInt32        GetEndVertexIdx()   const { return (KyUInt32)((m_edgeData & 0x00FFF000) >> 12); }
	KY_INLINE NavHalfEdgeType GetHalfEdgeType()   const { return (NavHalfEdgeType)(m_edgeData >> 29);         }
	/*	BitField
		bit  0 - 11 -> 12 bits : StartVertexIdx
		bit 12 - 23 -> 12 bits : EndVertexIdx
		bit 24 - 28 ->  5 bits : unused
		bit 29 - 31 ->  3 bits : NavHalfEdgeType
	*/
	KyUInt32 m_edgeData; ///< For internal use. Do not modify. 
};
KY_INLINE void SwapEndianness(Endianness::Target e, Stitch1To1Edge& self)
{
	SwapEndianness(e, self.m_edgeData);
}

// 1 per "meta" floor -> the generation tag-free navfloor, the generation tag-full navfloor
// and the runtime dynamic navFloor share the same 
class NavFloor1To1StitchDataBlob
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	NavFloor1To1StitchDataBlob()
	{
		m_firstIdxOfEdgeForType[0] = 0;
		m_firstIdxOfEdgeForType[1] = 0;
		m_firstIdxOfEdgeForType[2] = 0;
		m_firstIdxOfEdgeForType[3] = 0;
		m_firstIdxOfEdgeForType[4] = 0;
		m_edgeCountForType[0] = 0;
		m_edgeCountForType[1] = 0;
		m_edgeCountForType[2] = 0;
		m_edgeCountForType[3] = 0;
		m_edgeCountForType[4] = 0;
	}
	// store the edges coming from the tag-free navfloor version of type
	// EDGETYPE_CELLBOUNDARY_EAST, EDGETYPE_CELLBOUNDARY_NORTH, EDGETYPE_CELLBOUNDARY_WEST , EDGETYPE_CELLBOUNDARY_SOUTH, EDGETYPE_FLOORBOUNDARY
	// Stitch1To1Edge are sorted differently depending upon the edge type:
	// EDGETYPE_CELLBOUNDARY_EAST or EDGETYPE_CELLBOUNDARY_WEST   :  sorted along Y axis
	// EDGETYPE_CELLBOUNDARY_NORTH or EDGETYPE_CELLBOUNDARY_SOUTH :  sorted along X axis
	// EDGETYPE_FLOORBOUNDARY                                     :  sorted along X and Y, cf Vec2i::operator<()

	KyUInt16 m_firstIdxOfEdgeForType[5]; // see NavHalfEdgeType
	KyUInt16 m_edgeCountForType[5]; // see NavHalfEdgeType
	BlobArray<Stitch1To1Edge> m_stitch1To1Edges;
	BlobArray<NavVertex> m_navVertices;
	BlobArray<KyFloat32> m_navVertexAltitudes;
};
KY_INLINE void SwapEndianness(Endianness::Target e, NavFloor1To1StitchDataBlob& self)
{
	SwapEndianness(e, self.m_firstIdxOfEdgeForType[0]);
	SwapEndianness(e, self.m_firstIdxOfEdgeForType[1]);
	SwapEndianness(e, self.m_firstIdxOfEdgeForType[2]);
	SwapEndianness(e, self.m_firstIdxOfEdgeForType[3]);
	SwapEndianness(e, self.m_firstIdxOfEdgeForType[4]);
	SwapEndianness(e, self.m_edgeCountForType[0]);
	SwapEndianness(e, self.m_edgeCountForType[1]);
	SwapEndianness(e, self.m_edgeCountForType[2]);
	SwapEndianness(e, self.m_edgeCountForType[3]);
	SwapEndianness(e, self.m_edgeCountForType[4]);
	SwapEndianness(e, self.m_stitch1To1Edges);
	SwapEndianness(e, self.m_navVertices);
	SwapEndianness(e, self.m_navVertexAltitudes);
}


class Stitch1To1ToHalfEdgeInFloor
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	Stitch1To1ToHalfEdgeInFloor() : m_hasDifferentLinkFromStitch1To1Edge(0) {}

	KyUInt32 GetNavFloorLinkCount()    const { return m_dynamicNavFloorEdgeIdx.GetCount();      }
	KyUInt32 GetStitch1To1EdgeCount() const { return m_stitch1To1EdgeIdxToFirstIdx.GetCount(); }

	// NavHalfEdgeIdx to Stitch1To1EdgeIdx
	BlobArray<KyUInt16> m_stitch1To1EdgeIdx; // count = total of NavHalfEdge in dynamic navFloor that are on FloorLink

	// Stitch1To1EdgeIdx to NavHalfEdgeIdx 
	BlobArray<CompactNavHalfEdgeIdx> m_dynamicNavFloorEdgeIdx; // count=total of HalfEdge in dynamicFloor that are on FloorLink. Accessed by NavHalfEdge::GetBoundaryEdgeIdx()
	BlobArray<KyUInt16> m_stitch1To1EdgeIdxToFirstIdx; // count == number of Stitch1To1Edge == sizeof(m_stitch1To1Edges) in corresponding NavFloor1To1StitchData
	BlobArray<KyUInt16> m_stitch1To1EdgeIdxToCount;    // count == number of Stitch1To1Edge == sizeof(m_stitch1To1Edges) in corresponding NavFloor1To1StitchData
	KyUInt8 m_hasDifferentLinkFromStitch1To1Edge; // 0 or 1
};

KY_INLINE void SwapEndianness(Endianness::Target e, Stitch1To1ToHalfEdgeInFloor& self)
{
	SwapEndianness(e, self.m_stitch1To1EdgeIdx);
	SwapEndianness(e, self.m_dynamicNavFloorEdgeIdx);
	SwapEndianness(e, self.m_stitch1To1EdgeIdxToFirstIdx);
	SwapEndianness(e, self.m_stitch1To1EdgeIdxToCount);
	SwapEndianness(e, self.m_hasDifferentLinkFromStitch1To1Edge);
}

}

#endif //Navigation_NavFloorLinkInfo_H


