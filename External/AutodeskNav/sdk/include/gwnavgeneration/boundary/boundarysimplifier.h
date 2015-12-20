/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: LASI - secondary contact: NOBODY
#ifndef GwNavGen_BoundarySimplifier_H
#define GwNavGen_BoundarySimplifier_H

#include "gwnavruntime/base/types.h"
#include "gwnavgeneration/common/stllist.h"
#include "gwnavgeneration/containers/tlsarray.h"

#include "gwnavgeneration/boundary/boundarygraph.h"

namespace Kaim
{

class BoundaryVertexWithCost
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	BoundaryVertexWithCost() : m_cost(KyFloat32MAXVAL), m_alreadyRemoved(false) {}
	BoundaryVertexWithCost(TlsStlList<BoundarySimplifyVertex>::iterator vertex, KyFloat32 cost) : m_cost(cost), m_alreadyRemoved(false)
	{
		m_vertexIterator = vertex;
	}

	KY_INLINE bool operator<(const BoundaryVertexWithCost& other) const { return m_cost > other.m_cost; }

public:
	TlsStlList<BoundarySimplifyVertex>::iterator m_vertexIterator;
	KyFloat32 m_cost;
	bool m_alreadyRemoved;
};

class BoundarySimplifier
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	typedef TlsStlList<BoundarySimplifyVertex> VertexList;
	typedef TlsStlList<BoundarySimplifyVertex>::iterator VertexListIterator;

	BoundarySimplifier(BoundaryGraph* boundaryGraph) : m_boundaryGraph(boundaryGraph)
	{}

	//Smooth all the polylines of the input BoundaryGraph with a certain tolerance
	//Tolerance measures the maximum error allowed between a vertex of BoundaryGraph
	//and the corresponding simplified output polyline
	//Units of tolerance depends on Boundary vertex coordinates system. 
	//Tolerance can be interpreted in terms of "percentage" of pixelSize
	//for example, a tolerance of 0.5f means we accept an error of 0.5f * pixelSize
	//Vertical tolerance defines the maximum error along the up Axis. Its in meters and 
	//the value is ignored if it is negative.
	KyResult SimplifyBoundaries();

	KyResult SimplifyObstaclesAndConnexLinkBoundariesOfPolygon(KyUInt32 polygonIdx);
private:

	//based on Ramer Douglas Peucker approach
	//see: http://en.wikipedia.org/wiki/Ramer-Douglas-Peucker_algorithm
	KyResult DoSimplifyPolyline(BoundarySimplifyPolyline* polyline);

	//recursive algo. Max depth = log(polyline.size())
	void RecursiveRDP(BoundarySimplifyPolyline* polyline,
		VertexListIterator begin,
		VertexListIterator last);

	void ComputePolylineBoundingBox(KyUInt32 polylineIdx);

	KyResult SimplifyAllCellLinkAndFloorLinkBoundaries();
	void RemoveVerticesIfPossible(BoundarySimplifyPolyline* polyline, VertexListIterator begin,
		VertexListIterator last);

	bool TestSimplificationAgainstOtherPolylinesAndSections(BoundarySimplifyPolyline* polyline,
		VertexListIterator begin, VertexListIterator last,
		VertexListIterator previousVertex,VertexListIterator currentVertex, VertexListIterator nextVertex,
		const Vec2i& v0, const Vec2i& v1, const Vec2i& v2, KyFloat32 squareDist, KyUInt32 insideVertexCount, bool canCollapse);

	bool TestAgainstTheOtherSectionsOfPolyline(BoundarySimplifyPolyline* polyline, VertexListIterator begin, VertexListIterator last,
		VertexListIterator previousVertex, VertexListIterator currentVertex, VertexListIterator nextVertex,
		const Vec2i& v0, const Vec2i& v1, const Vec2i& v2, KyFloat32 squareDist);

	bool TestAgainstTheOtherPolylinesInFloor(BoundarySimplifyPolyline* polyline, VertexListIterator previousVertex,
		VertexListIterator currentVertex, VertexListIterator nextVertex, const Vec2i& v0, const Vec2i& v1, const Vec2i& v2,
		KyFloat32 squareDist, KyUInt32 insideVertexCount, bool canCollapse);

	bool TestAgainstTheOtherPointsOfPolylineSection(BoundarySimplifyPolyline* polyline,
		VertexListIterator begin, VertexListIterator last, VertexListIterator previousVertex,
		VertexListIterator currentVertex, VertexListIterator nextVertex, const Vec2i& v0, const Vec2i& v1, const Vec2i& v2, KyFloat32 squareDist);

	bool TestAgainsPoint(const Vec2i& v0, const Vec2i& v1, const Vec2i& v2, const Vec2i& otherPos,
		VertexListIterator previousVertex, VertexListIterator nextVertex, VertexListIterator otherCurrent, KyFloat32 squareDist);

	void BuildBinaryHeap(BoundarySimplifyPolyline* polyline, VertexListIterator begin,
		VertexListIterator last, KyUInt32& insideVertexCount);

	bool IsAxisAlignedElseGetCCW(const Vec2i& previousPos, const Vec2i& currentPos, const Vec2i& nextPos, Vec2i& v0, Vec2i& v1, Vec2i& v2);

	void MarkAsRemovedInTheHeap(VertexListIterator iter, VertexListIterator invalidIter);
	void GetCyclicNext(VertexList& vertices, VertexListIterator& iter);
	void GetCyclicPrev(VertexList& vertices, VertexListIterator& iter);

public:
	KyFloat32 m_verticalTolerance;
	KyFloat32 m_exteriorSimplificationTolerance;
	KyFloat32 m_interiorSimplificationTolerance;
	KyFloat32 m_toleranceForConnexLink;

private:
	BoundaryGraph* m_boundaryGraph;

	KyArrayTLS_POD<BoundarySimplifyPolyline*> m_polylinesInPolygon;
	KyArrayTLS_POD<BoundarySimplifyPolyline*> m_currentBoxIntersectingPolylines;
	KyArrayTLS_POD<Box2f> m_polylineBoudingBox;

	KyArrayTLS_CPP<VertexListIterator> m_staticVertices; // used in cycled polyline to extract p
	BinaryHeapTLS_CPP<BoundaryVertexWithCost> m_binaryHeap;
};

KY_INLINE void BoundarySimplifier::GetCyclicNext(VertexList& vertices, VertexListIterator& iter)
{
	++iter;

	if (iter == vertices.end())
		iter = vertices.begin();
}

KY_INLINE void BoundarySimplifier::GetCyclicPrev(VertexList& vertices, VertexListIterator& iter)
{
	if (iter == vertices.begin())
		iter = vertices.end();

	--iter;
}

}


#endif //GwNavGen_BoundarySimplifier_H

