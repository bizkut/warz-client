/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavHalfEdge_H
#define Navigation_NavHalfEdge_H

#include "gwnavruntime/navmesh/navmeshtypes.h"

namespace Kaim
{

/// Each instance of NavHalfEdge represents a single edge of a single triangle within the NavMesh.
/// It is called a "half-edge" because adjacent triangles do not share the edge that forms their border;
/// instead, each triangle maintains its own half-edge to represent its own boundary.
class NavHalfEdge
{
public:
	NavHalfEdge() {}

	
	NavHalfEdgeType GetHalfEdgeType()   const; ///< Returns an element from the #NavHalfEdgeType enumeration that indicates the type of border this edge represents.
	NavVertexIdx    GetStartVertexIdx() const; ///< Returns the index of the starting vertex of the edge. 

	/// If this edge borders an edge in an adjoining triangle, this method returns the index of that adjacent
	/// edge. Only call this method if GetHalfEdgeType() returns Kaim::EDGETYPE_CONNEXBOUNDARY or Kaim::EDGETYPE_PAIRED.
	NavHalfEdgeIdx GetPairHalfEdgeIdx() const;

	/// If this edge lies on the border of the NavFloor, this method returns the index of this edge among the boundary edges
	/// maintained by its NavFloor. Only call this method if IsHalfEdgeABoundary(GetHalfEdgeType()) returns true. 
	KyUInt32 GetBoundaryEdgeIdx() const;

	/// If this edge lies on the border of the NavCell, this method returns the cardinal direction of the border where it lies in
	/// its NavCell. Only call this method if IsHalfEdgeACellBoundary(GetHalfEdgeType()) returns true. 
	KyUInt32 GetCellBoundaryDir() const;

	/// If this edge borders an obstacle, this method returns the index of the next edge that borders that obstacle.
	/// Only call this method if GetHalfEdgeType() returns Kaim::EDGETYPE_OBSTACLE. 
	NavHalfEdgeIdx GetNextObstacleHalfEdgeIdx() const;

	/// Returns an element from the #NavHalfEdgeObstacleType enumeration that indicates the type of obstacle this edge represents.
	/// Only call this method if GetHalfEdgeType() returns Kaim::EDGETYPE_OBSTACLE.
	NavHalfEdgeObstacleType GetHalfEdgeObstacleType() const;

public:
	/*	BitField
		bit  0 - 13 -> 14 bits : The index of the adjoining half-edge. Access through GetPairHalfEdgeIdx() or GetBoundaryEdgeIdx()
		bit 14 - 25 -> 12 bits : StartVertexIdx
		bit 26 - 27 ->  2 bits : unused
		bit      28 ->  1 bit  : NavHalfEdgeObstacleType
		bit 29 - 31 ->  3 bits : NavHalfEdgeType
	*/
	KyUInt32 m_edgeData; ///< For internal use. Do not modify. 
};

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.
KY_INLINE void SwapEndianness(Endianness::Target e, NavHalfEdge& self)
{
	SwapEndianness(e, self.m_edgeData);
}


KY_INLINE NavVertexIdx            NavHalfEdge::GetStartVertexIdx()       const { return (NavVertexIdx)((m_edgeData & 0x03FFC000) >> 14);            }
KY_INLINE NavHalfEdgeType         NavHalfEdge::GetHalfEdgeType()         const { return (NavHalfEdgeType)(m_edgeData >> 29);                        }
KY_INLINE NavHalfEdgeObstacleType NavHalfEdge::GetHalfEdgeObstacleType() const { return (NavHalfEdgeObstacleType)((m_edgeData & 0x10000000) >> 28); }

KY_INLINE CardinalDir NavHalfEdge::GetCellBoundaryDir() const
{
	KY_DEBUG_ASSERTN(IsHalfEdgeACellBoundary(GetHalfEdgeType()) == true,
		("You must not call this function if the halfEdge is not of type NavHalfEdgeType::CELL_BOUNDARY"));

	return (CardinalDir)(GetHalfEdgeType());
}


KY_INLINE NavHalfEdgeIdx NavHalfEdge::GetPairHalfEdgeIdx() const
{
	KY_DEBUG_ASSERTN(GetHalfEdgeType() == EDGETYPE_PAIRED || GetHalfEdgeType() == EDGETYPE_CONNEXBOUNDARY,
		("You must not call this function if the halfEdge is not of type NavHalfEdgeType::PAIRED."));

	return (NavHalfEdgeIdx)(m_edgeData & 0x00003FFF);
}

KY_INLINE KyUInt32 NavHalfEdge::GetBoundaryEdgeIdx() const
{
	KY_DEBUG_ASSERTN(IsHalfEdgeAFloorOrCellBoundary(GetHalfEdgeType()) == true,
		("You must not call this function if the halfEdge is not a boundary edge."));

	return (KyUInt32)(m_edgeData & 0x00003FFF);
}

KY_INLINE NavHalfEdgeIdx NavHalfEdge::GetNextObstacleHalfEdgeIdx() const
{
	KY_DEBUG_ASSERTN(GetHalfEdgeType() == EDGETYPE_OBSTACLE,
		("You must not call this function if the halfEdge is not of type NavHalfEdgeType::OBSTACLE."));

	return (NavHalfEdgeIdx)(m_edgeData & 0x00003FFF);
}

}

#endif //Navigation_NavHalfEdge_H

