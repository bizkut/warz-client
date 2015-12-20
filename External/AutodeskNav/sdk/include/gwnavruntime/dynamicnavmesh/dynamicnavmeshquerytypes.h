/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_DynamicNavMeshQueryTypes_H
#define Navigation_DynamicNavMeshQueryTypes_H

// primary contact: LASI - secondary contact: NONE
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/math/vec2i.h"
#include "gwnavruntime/math/vec3i.h"
#include "gwnavruntime/math/geometryfunctions.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/visualsystem/visualcolor.h"
#include "gwnavruntime/math/integersegmentutils.h"
#include "gwnavruntime/basesystem/logger.h"
#include "gwnavruntime/containers/sharedpoollist.h"



namespace Kaim
{

static const KyUInt32 SWEEPLINE_CLIPPING_EXTERIOR_POLYGON_BIT = 1 << 30;
static const KyUInt32 SWEEPLINE_CLIPPING_HOLE_POLYGON_BIT     = 1 << 29;

class EdgePoints
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	EdgePoints() {}
	EdgePoints(const Vec2i& start, const Vec2i& end) : m_start(start), m_end(end) {}

	bool operator==(const EdgePoints& rhs) const { return m_start == rhs.m_start && m_end == rhs.m_end; }
	bool operator!=(const EdgePoints& rhs) const { return !operator==(rhs); }

	//return -1 below, 0 on and 1 above line
	KyInt32 ComputePointLocation(const Vec2i& pt) const { return IntegerSegment::OnAboveOrBelow<KyInt64>(pt, m_start, m_end); }

	KY_INLINE bool IsAnExtremityWithThisXValue(KyInt32 xValue, KyInt32& correpondingYValue) const
	{
		if (m_start.x == xValue) { correpondingYValue = m_start.y; return true; }
		if (m_end.x   == xValue) { correpondingYValue =   m_end.y; return true; }
		return false;
	}

	//idx = 0 or idx = 1
	const Vec2i& operator[](KyUInt32 idx) const { KY_ASSERT(idx <= 1); return (&m_start)[idx]; }
public:
	Vec2i m_start;
	Vec2i m_end;
};

class InputEdgeProperty
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public: 

	enum EdgeOrigin
	{
		EDGE_FROM_CELLBOUNDARY_EAST   = EDGETYPE_CELLBOUNDARY_EAST  /*0*/,
		EDGE_FROM_CELLBOUNDARY_NORTH  = EDGETYPE_CELLBOUNDARY_NORTH /*1*/,
		EDGE_FROM_CELLBOUNDARY_WEST   = EDGETYPE_CELLBOUNDARY_WEST  /*2*/,
		EDGE_FROM_CELLBOUNDARY_SOUTH  = EDGETYPE_CELLBOUNDARY_SOUTH /*3*/,
		EDGE_FROM_FLOORBOUNDARY       = EDGETYPE_FLOORBOUNDARY      /*4*/,
		EDGE_FROM_OBSTACLE            = EDGETYPE_OBSTACLE           /*5*/,
		EDGE_FROM_CONNEXBOUNDARY      = EDGETYPE_CONNEXBOUNDARY     /*6*/,
		EDGE_FROM_TAGVOLUME           = 7,
		EDGE_FROM_UNKNOWN_ORIGIN
	};

public:
	KY_INLINE InputEdgeProperty() { SetDefault(); }
	KY_INLINE void SetDefault()
	{
		m_edgeOrigin = EDGE_FROM_UNKNOWN_ORIGIN;
		m_ownerIndex = KyUInt32MAXVAL;
		m_stitch1To1EdgeIdx = KyUInt32MAXVAL;
		m_navTagIndex = KyUInt32MAXVAL;
		m_index = KyUInt32MAXVAL;
		m_coordinatesFlipped = false;
	}

	bool IsConnexBoundary()           const { return (m_ownerIndex & SWEEPLINE_CLIPPING_EXTERIOR_POLYGON_BIT) != 0; }
	bool IsHole()                     const { return (m_ownerIndex & SWEEPLINE_CLIPPING_HOLE_POLYGON_BIT)     != 0; }

	bool operator <(const InputEdgeProperty& rhs)  const { return m_ownerIndex <  rhs.m_ownerIndex; }
	bool operator ==(const InputEdgeProperty& rhs) const { return m_ownerIndex == rhs.m_ownerIndex; }

public:
	EdgeOrigin m_edgeOrigin; // EDGE_FROM_NAVFLOOR_XXX  or EDGE_FROM_TAGVOLUME
	KyUInt32 m_ownerIndex; // index of the contour it belong to in Navfloor or TagVolumeIndex
	KyUInt32 m_stitch1To1EdgeIdx;
	KyUInt32 m_navTagIndex; // info regarding the navtag associated to this edge : index in the array of dynamicNavTag
	// internal state
	KyUInt32 m_index; // index of the edge, unique among edges for a particular polygon
	bool m_coordinatesFlipped;
};

class InputEdge
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public: 
	InputEdge()  {}

public:
	EdgePoints m_extremities;
	InputEdgeProperty m_property;
};

// simple "std::pair"-style comparator
struct BasicLessHalfEdge
{
	KY_INLINE bool operator() (const InputEdge& edge1, const InputEdge& edge2) const
	{
		if(edge1.m_extremities.m_start != edge2.m_extremities.m_start)
			return edge1.m_extremities.m_start < edge2.m_extremities.m_start;
		else
			return edge1.m_extremities.m_end < edge2.m_extremities.m_end;
	}
};


class InputEdgePiece
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	InputEdgePiece(): m_parentEdge(KY_NULL), m_count(0), m_index(0) {}
	InputEdgePiece(const EdgePoints& subSegment, const InputEdge& parentEdge) :
	m_extremities(subSegment), m_parentEdge(&parentEdge), m_count(0), m_index(0) {}

	const InputEdgeProperty& ParentProperty() const { return m_parentEdge->m_property; }
	inline bool operator==(const InputEdgePiece& otherEdge) const
	{
		return GetEdgeStart() == otherEdge.GetEdgeStart() && GetEdgeEnd() == otherEdge.GetEdgeEnd() && GetCount() == otherEdge.GetCount();
	}
	inline bool operator<(const InputEdgePiece& otherEdge) const
	{
		if (GetEdgeStart().x != otherEdge.GetEdgeStart().x)
			return GetEdgeStart().x < otherEdge.GetEdgeStart().x;

		if (GetEdgeStart().y != otherEdge.GetEdgeStart().y)
			return GetEdgeStart().y < otherEdge.GetEdgeStart().y;

		// both edges have the same start points....
		return IntegerSegment::LessSlope<KyInt32>(GetEdgeStart(), GetEdgeEnd(), otherEdge.GetEdgeEnd());
	}
	bool operator!=(const InputEdgePiece& edge) const { return !((*this) == edge); }
	bool operator>=(const InputEdgePiece& edge) const { return !((*this) <  edge); }

	KyFloat32 EvalAtX(KyInt32 xIn) const { return IntegerSegment::EvalAtXforY_float(xIn, GetEdgeStart(), GetEdgeEnd()); }

	bool IsVertical()           const { return GetEdgeStart().x == GetEdgeEnd().x; }
	KyInt32 GetCount()          const { return m_count;                            }
	const Vec2i& GetEdgeStart() const { return m_extremities.m_start;                }
	const Vec2i& GetEdgeEnd()   const { return m_extremities.m_end;                  }

	const Vec2i& GetOriginalStart() const 
	{
		// we know, by construction, that m_edgePiece.m_start.x <= m_edgePiece.m_end.x
		// so m_edgePiece.m_start.x - m_edgePiece.m_end.x >= is the same as m_edgePiece.m_start.x == m_edgePiece.m_end.x
		const KyInt32 swappedPropertyCount = Isel(m_extremities.m_start.x - m_extremities.m_end.x, -m_count , m_count);
		const KyInt32 extremityIdx = Isel(swappedPropertyCount, 1, 0); // 0 for start, 1 for end
		return m_extremities[extremityIdx]; 
	}
	const Vec2i& GetOriginalEnd() const 
	{
		// we know, by construction, that m_edgePiece.m_start.x <= m_edgePiece.m_end.x
		// so m_edgePiece.m_start.x - m_edgePiece.m_end.x >= 0 is the same as m_edgePiece.m_start.x == m_edgePiece.m_end.x
		const KyInt32 swappedPropertyCount = Isel(m_extremities.m_start.x - m_extremities.m_end.x, -m_count , m_count);
		const KyInt32 extremityIdx = Isel(swappedPropertyCount, 0, 1); // 0 for start , 1 for end
		return m_extremities[extremityIdx];
	}
public:
	EdgePoints m_extremities;
	const InputEdge* m_parentEdge;
	// on construction in edge intersector,
	// m_count == 1 means that m_edgePiece.m_start < m_edgePiece.end || (m_edgePiece is Vertical && m_edgePiece.m_start >= m_edgePiece.end)
	// m_count == -1 means that m_edgePiece.m_start >= m_edgePiece.end || (m_edgePiece is Vertical && m_edgePiece.m_start < m_edgePiece.end)
	KyInt32 m_count;
	KyUInt32 m_index;
};

class SweepLineOutputEdgePiece;
class MergedPolygon
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	MergedPolygon() {}
public:
	KyArrayPOD<Vec2i> m_points;
	KyArrayPOD<const SweepLineOutputEdgePiece*> m_edges;
};

class MergedPolygonWithHoles
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	MergedPolygonWithHoles() {}

public:
	MergedPolygon m_exterior;
	KyArray<MergedPolygon> m_holes;
};

class TriangulatorOutput
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	KyUInt32 m_navTagIndex;
	MergedPolygon m_referencePolygon;
	KyArrayPOD<const SweepLineOutputEdgePiece*> m_inputEdgePiece; //GetCount == edgeCount
	KyArrayPOD<KyUInt32> m_triangleEdgeIndices; // GetCount == edgeCount; for triangle I, indices 3i, 3i+1, 3i+2 are the edge ()
	KyArrayPOD<KyUInt32> m_startVertexOfEdges; // GetCount == edgeCount; accesing from edgeIdx
	KyArrayPOD<KyUInt32> m_pairEdgeIndices; // GetCount == edgeCount; 
};


class IndexedPos
{
public:
	IndexedPos() : m_index(KyUInt32MAXVAL) {}
	IndexedPos(const Vec2i& pos, KyUInt32 index) : m_pos(pos), m_index(index) {}
public:
	Vec2i m_pos;
	KyUInt32 m_index;
};

class IndexedPosSorter
{
public:
	KY_INLINE bool operator() (const IndexedPos& hotPoint1, const IndexedPos& hotPoint2) const
	{
		return hotPoint1.m_pos < hotPoint2.m_pos;
	}
};

class EdgePointsComparatorAtX
{
public:
	inline EdgePointsComparatorAtX() : m_x(KY_NULL), m_justBefore(KY_NULL){}
	inline EdgePointsComparatorAtX(KyInt32 x, KyInt32 *justBefore = KY_NULL) : m_x(x), m_justBefore(justBefore){}

	inline bool operator () (const EdgePoints& edge1, const EdgePoints& edge2) const
	{
		KY_ASSERT(edge1.m_start != edge2.m_start || edge1.m_start.x == m_x);
		KY_ASSERT(edge1.m_start.x <= m_x && edge1.m_end.x >= m_x); // We are sure that edge1 intersects the vertical axis x == m_x
		KY_ASSERT(edge2.m_start.x <= m_x && edge2.m_end.x >= m_x); // We are sure that edge2 intersects the vertical axis x == m_x

		const KyInt32 edge1Miny = FastMin(edge1.m_start.y, edge1.m_end.y);
		const KyInt32 edge1Maxy = FastMax(edge1.m_start.y, edge1.m_end.y);

		const KyInt32 edge2Miny = FastMin(edge2.m_start.y, edge2.m_end.y);
		const KyInt32 edge2Maxy = FastMax(edge2.m_start.y, edge2.m_end.y);

		if (edge1Maxy < edge2Miny)
			return true; // the edge1 interval on Y-axis is strictly "below" the edge2 interval on Y-axis.

		if (edge1Miny > edge2Maxy)
			return false;  // the edge1 interval on Y-axis is strictly "above" the edge2 interval on Y-axis.

		// here we know that there is an intersection between the edge1 and edge2 interval on Y-axis
		// To know which edge is above the other on the m_x axis, we have to compute the y-value of both edges along this axis (edge1y and edge2y)

		//check if either x of elem1 is equal to x_
		KyInt32 yValueOfPointAtXOnEdge1 = 0;
		KyInt32 yValueOfPointAtXOnEdge2 = 0;

		bool edge1_at_x = edge1.IsAnExtremityWithThisXValue(m_x, yValueOfPointAtXOnEdge1);
		bool edge2_at_x = edge2.IsAnExtremityWithThisXValue(m_x, yValueOfPointAtXOnEdge2);

		if (edge1_at_x == false || edge2_at_x == false)
		{
			KY_LOG_ERROR_IF(edge2.m_start == edge1.m_start, ("We know that should not happen when this function is called, except if both of them are on the m_x axis"));

			//at least one of the segments doesn't have an end point at the current x

			//return value of ComputePointLocationFromEdge is
			// -1 below the edge ; 1 above the edge  ; 0 on the edge
			KyInt32 edge1StartPosFromEdge2 = edge2.ComputePointLocation(edge1.m_start);
			KyInt32 edge1EndPosFromEdge2   = edge2.ComputePointLocation(edge1.m_end);
			if (edge1StartPosFromEdge2 == edge1EndPosFromEdge2)
			{
				// the two extremity of edge1 are on the same side of edge2. if both of them are below, edge1<edge2 at this x
				return edge1StartPosFromEdge2 == -1;
			}

			// the two extremity of edge1 are not on the same side of edge2.
			// we know by construction that the two edges cannot intersect except at their extremity.

			KyInt32 edge2StartPosFromEdge1 = edge1.ComputePointLocation(edge2.m_start);
			// we know that edge2.m_start cannot be on the edge since if such an intersection exists, it can be only on one of the extremity
			// edge2.m_start cannot be on the end point of edge1 because if it was, both of them should be
			//     on m_x axis (we checked if (edge1_at_x == false || edge2_at_x == false) )
			// edge2.m_start cannot be on the start point of edge1 because we are sure of that when we call
			//       this function (this has to be checked). This only happen when edge1.m_start == edge2.m_start == m_x
			KY_LOG_ERROR_IF(edge2StartPosFromEdge1 == 0, ("Error in comparator object"));

			return edge2StartPosFromEdge1 == 1; //edge1's point is above edge1
		}

		// Here we know that the two edges have one of their extremity on the m_x axis

		if (yValueOfPointAtXOnEdge1 != yValueOfPointAtXOnEdge2)
			return yValueOfPointAtXOnEdge1 < yValueOfPointAtXOnEdge2;

		if (edge1 == edge2) // the two edges are equals...
			return false;

		bool edge1SlopeLessThanEdge2Slope = IntegerSegment::LessSlopeUnit<KyInt64>(
			(KyInt64)(edge1.m_end.x - edge1.m_start.x),
			(KyInt64)(edge1.m_end.y - edge1.m_start.y),
			(KyInt64)(edge2.m_end.x - edge2.m_start.x),
			(KyInt64)(edge2.m_end.y - edge2.m_start.y));

/*
		if (m_justBefore == KY_NULL || (*m_justBefore) == 0)
			return edge1SlopeLessThanEdge2Slope;
		else
			return edge1SlopeLessThanEdge2Slope == false;*/
		return edge1SlopeLessThanEdge2Slope == (m_justBefore == KY_NULL || (*m_justBefore) == 0);

	}

private:
	KyInt32 m_x; //x value at which to apply comparison
	KyInt32* m_justBefore;
};

class InputEdgePieceComp
{
public:
	inline bool operator() (const InputEdgePiece& i1, const InputEdgePiece& i2) const
	{
		const EdgePoints& edge1 = i1.m_extremities;
		const EdgePoints& edge2 = i2.m_extremities;

		if (edge1.m_start != edge2.m_start)
			return edge1.m_start < edge2.m_start;

		EdgePointsComparatorAtX lhe(edge1.m_start.x);
		return lhe(edge1, edge2);
	}
};

} // namespace Kaim



#endif // Navigation_DynamicNavMeshQueryTypes_H
