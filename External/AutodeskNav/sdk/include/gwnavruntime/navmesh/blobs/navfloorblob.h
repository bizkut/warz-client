/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: LASI
#ifndef Navigation_NavFloorBlob_H
#define Navigation_NavFloorBlob_H

#include "gwnavruntime/database/navtag.h"
#include "gwnavruntime/navmesh/blobs/navhalfedge.h"
#include "gwnavruntime/navmesh/blobs/navvertex.h"
#include "gwnavruntime/navdata/navdatablobcategory.h"
#include "gwnavruntime/navmesh/blobs/flooraltituderange.h"
#include "gwnavruntime/navmesh/blobs/stitch1To1datablob.h"

namespace Kaim
{

/// Represents the integer axis-aligned bounding box of a NavFloor.
class NavFloorAABB
{
public:
	NavFloorAABB() {}

	/// Indicates whether or not the bounding box represented by this object intersects the specified NavFloorAABB. 
	bool DoesIntersect(const NavFloorAABB& other) const;

	NavVertex m_min; ///< The minima of the axis-aligned bounding box computed during NavData generation. Do not modify.
	NavVertex m_max; ///< The maxima of the axis-aligned bounding box computed during NavData generation. Do not modify.
};


/// The NavFloorBlob contains a connected and not overlapping part of triangle mesh static data within a NavCellBlob.
/// 
/// NavTriangles are implicitly defined by three consecutive NavHalfEdge in #m_navHalfEdges, there is no explicit NavTriangle class.
/// A NavTag is associated to each NavTriangle
class NavFloorBlob
{
	KY_ROOT_BLOB_CLASS(NavData, NavFloorBlob, 0)
	KY_CLASS_WITHOUT_COPY(NavFloorBlob)
public:
	NavFloorBlob();

	// ---------------------------------- static member functions ----------------------------------

	static NavTriangleIdx NavHalfEdgeIdxToTriangleIdx(NavHalfEdgeIdx idx);         ///< Retrieves the index of the triangle that contains the edge specified the input NavHalfEdgeIdx.

	static NavHalfEdgeIdx NavTriangleIdxToFirstNavHalfEdgeIdx(NavTriangleIdx idx); ///< Retrieves the first NavHalfEdgeIdx of NavTriangle specified by the input NavTriangleIdx.
	static NavHalfEdgeIdx NavHalfEdgeIdxToFirstNavHalfEdgeIdx(NavHalfEdgeIdx idx); ///< Retrieves the first NavHalfEdgeIdx of NavTriangle thet contains the edge specified by the input NavHalfEdgeIdx.
	static NavHalfEdgeIdx NavHalfEdgeIdxToNextNavHalfEdgeIdx(NavHalfEdgeIdx idx);  ///< Retrieves the triangle NavHalfEdgeIdx that follows the edge specified the input NavHalfEdgeIdx within its triangle.
	static NavHalfEdgeIdx NavHalfEdgeIdxToPrevNavHalfEdgeIdx(NavHalfEdgeIdx idx);  ///< Retrieves the triangle NavHalfEdgeIdx that precedes the edge specified the input NavHalfEdgeIdx within its triangle.

	/// Retrieves one edge from the specified triangle.
	/// \param idx				The index of the triangle whose edge is to be retrieved.
	/// \param halfEdgeNumber	The index of the edge to retrieve within the triangle. May be 0, 1 or 2.
	static NavHalfEdgeIdx NavTriangleIdxToNavHalfEdgeIdx(NavTriangleIdx idx, KyInt32 halfEdgeNumber);

	/// Retrieves the index of the specified edge within its triangle.
	/// \return 0, 1 or 2.
	static KyUInt32 NavHalfEdgeIdxToHalfEdgeNumberInTriangle(NavHalfEdgeIdx idx);

	// ---------- accessors ----------

	KyUInt32 GetNavVertexCount()   const;
	KyUInt32 GetNavHalfEdgeCount() const;
	KyUInt32 GetNavTriangleCount() const;

	const FloorAltitudeRange& GetFloorAltitudeRange()                      const;
	const NavTag&             GetNavTag(NavTriangleIdx navTriangleIdx)     const; ///< Retrieves the NavTag associated to this triangle.
	const NavVertex&          GetNavVertex(const NavVertexIdx idx)         const; ///< Retrieves the vertex with the specified index within this NavFloorBlob.
	const NavHalfEdge&        GetNavHalfEdge(NavHalfEdgeIdx idx)           const; ///< Retrieves the triangle edge with the specified index within this NavFloorBlob.
	KyFloat32                 GetNavVertexAltitude(const NavVertexIdx idx) const; ///< Retrieves the altitude of the vertex with the specified index within this NavFloorBlob.
	NavHalfEdgeType           GetHalfEdgeType(NavHalfEdgeIdx idx)          const; ///< Retrieves the type of the specified triangle edge.
	NavHalfEdgeObstacleType   GetHalfEdgeObstacleType(NavHalfEdgeIdx idx)  const; ///< Retrieves the obstacle type of the specified triangle edge.

	// ---------- function to browse the NavFloorBlob data structure ----------

	const NavVertex& NavHalfEdgeIdxToStartNavVertex(NavHalfEdgeIdx idx)    const; ///< Retrieves the vertex at the start of the specified triangle edge.
	NavVertexIdx     NavHalfEdgeIdxToStartNavVertexIdx(NavHalfEdgeIdx idx) const; ///< Retrieves the index of the vertex at the start of the specified triangle edge.

	const NavVertex& NavHalfEdgeIdxToEndNavVertex(NavHalfEdgeIdx idx)    const; ///< Retrieves the vertex at the end of the specified triangle edge.
	NavVertexIdx     NavHalfEdgeIdxToEndNavVertexIdx(NavHalfEdgeIdx idx) const; ///< Retrieves the index of the vertex at the end of the specified triangle edge.

	const NavVertex& NavHalfEdgeIdxToThirdNavVertex(NavHalfEdgeIdx idx)    const; ///< Retrieves the vertex in the triangle that is not connected to the specified triangle edge.
	NavVertexIdx     NavHalfEdgeIdxToThirdNavVertexIdx(NavHalfEdgeIdx idx) const; ///< Retrieves the index of the vertex in the triangle that is not connected to the specified triangle edge.

	const NavHalfEdge& NavHalfEdgeIdxToPairNavHalfEdge(NavHalfEdgeIdx idx)    const; ///< Retrieves the triangle edge that is adjacent to the specified triangle edge, if any.
	NavHalfEdgeIdx     NavHalfEdgeIdxToPairNavHalfEdgeIdx(NavHalfEdgeIdx idx) const; ///< Retrieves the index of the triangle edge that is adjacent to the specified triangle edge, if any.

	const NavHalfEdge& NavHalfEdgeIdxToNextNavHalfEdge(NavHalfEdgeIdx idx) const; ///< Retrieves the edge that follows the specified triangle edge within its triangle.
	const NavHalfEdge& NavHalfEdgeIdxToPrevNavHalfEdge(NavHalfEdgeIdx idx) const; ///< Retrieves the edge that precedes the specified triangle edge within its triangle.

	/// Retrieves one edge from the specified triangle.
	/// \param idx               The index of the triangle whose edge is to be retrieved.
	/// \param halfEdgeNumber    The index of the edge to retrieve within the triangle. May be 0, 1 or 2.
	const NavHalfEdge& NavTriangleIdxToNavHalfEdge(NavTriangleIdx idx, const KyInt32 halfEdgeNumber) const;

	/// Retrieves the indices of the vertices that make up the specified triangle.
	/// \param triangleIdx    The index of the triangle whose vertices are to be retrieved.
	/// \param[out] v0Idx     The index of the first vertex in the triangle.
	/// \param[out] v1Idx     The index of the second vertex in the triangle.
	/// \param[out] v2Idx     The index of the third vertex in the triangle.
	void NavTriangleIdxToNavVertexIndices(NavTriangleIdx triangleIdx, NavVertexIdx& v0Idx, NavVertexIdx& v1Idx, NavVertexIdx& v2Idx) const;

	/// Retrieves one vertex from the specified triangle.
	/// \param idx             The index of the triangle whose edge is to be retrieved.
	/// \param vertexNumber    The index of the vertex to retrieve within the triangle. May be 0, 1 or 2.
	const NavVertex& NavTriangleIdxToNavVertex(NavTriangleIdx idx, const KyInt32 vertexNumber) const;

	/// Retrieves the index of one vertex from the specified triangle.
	/// \param idx             The index of the triangle whose edge is to be retrieved.
	/// \param vertexNumber    The index of the vertex to retrieve within the triangle. May be 0, 1 or 2.
	NavVertexIdx NavTriangleIdxToVertexIdx(NavTriangleIdx idx, const KyInt32 vertexNumber) const;

	// ---------- geometrical test ----------

	/// Indicates whether or not the specified position lies inside the bounding box of the NavFloorBlob on the (X,Y) plane. For internal use.
	bool IsPointInsideFloor(const CoordPos64& coordPos64, const CoordPos64& cellOrigin) const;

	/// Indicates whether or not the specified position lies inside the bounding box of the NavFloorBlob on the (X,Y) plane. For internal use.
	bool IsPointInsideFloor(const CoordPos& coordPosInCell) const;

public: // Internal
	KyUInt32 GetNavConnexCount()   const;
	NavConnexIdx NavTriangleIdxToNavConnexIdx(NavTriangleIdx idx) const; ///< Retrieves the index of the NavConnex associated to the triangle specified by the input NavTriangleIdx.

	bool IsValid() const;                           ///< performs some basic tests on static data. For internal debug purpose.
	bool IsTriangleValid(NavTriangleIdx idx) const; ///< performs some basic tests on static data. For internal debug purpose.

	bool IsHalfEdgeValid(NavHalfEdgeIdx currentEdgeIdx, const Vec2i &v1, const Vec2i &v2) const;

	KyUInt32 GetNavFloorLinkCount()    const;
	KyUInt32 GetStitch1To1EdgeCount() const;

public:
	BlobArray<CompactNavConnexIdx> m_triangleConnexIndices; ///< The array of CompactNavConnexIdx associated to each triangle. GetCount() == GetTriangleCount(). Do not modify.
	BlobArray<NavTag> m_connexNavTag;                       ///< The array of NavTag associated to each NavConnex. GetCount() == GetConnexCount(). Do not modify.
	BlobArray<NavHalfEdge> m_navHalfEdges;                  ///< The array of triangle edges maintained by the NavFloorBlob. GetCount() == GetHalfEdgeCount(). Do not modify.
	BlobArray<NavVertex> m_navVertices;                     ///< The array of triangle vertices maintained by the NavFloorBlob. GetCount() == GetVertexCount(). Do not modify.
	BlobArray<KyFloat32> m_navVerticesAltitudes;            ///< The array of triangle vertices altitude maintained by the NavFloorBlob. GetCount() == GetVertexCount(). Do not modify.
	FloorAltitudeRange m_altitudeRange;                     ///< The range of altitude the vertices of this NavFloor are within
	NavFloorAABB m_navFloorAABB;                            ///< An axis-oriented bounding box that stores the extents of this NavFloorBlob.

	Stitch1To1ToHalfEdgeInFloor m_stitch1To1ToHalfEdgeInFloor;
};


/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.
KY_INLINE void SwapEndianness(Endianness::Target e, NavFloorAABB& self)
{
	SwapEndianness(e, self.m_min);
	SwapEndianness(e, self.m_max);
}
KY_INLINE void SwapEndianness(Endianness::Target e, NavFloorBlob& self)
{
	SwapEndianness(e, self.m_triangleConnexIndices);
	SwapEndianness(e, self.m_connexNavTag);
	SwapEndianness(e, self.m_navHalfEdges);
	SwapEndianness(e, self.m_navVertices);
	SwapEndianness(e, self.m_navVerticesAltitudes);

	SwapEndianness(e, self.m_altitudeRange);
	SwapEndianness(e, self.m_navFloorAABB);
	SwapEndianness(e, self.m_stitch1To1ToHalfEdgeInFloor);
}

}

#include "gwnavruntime/navmesh/blobs/navfloorblob.inl"

#endif //Navigation_NavFloorBlob_H


