/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: GUAL - secondary contact: LASI
#ifndef GwNavGen_BoundaryGraph_H
#define GwNavGen_BoundaryGraph_H

#include "gwnavruntime/navmesh/celldesc.h"
#include "gwnavgeneration/boundary/boundarypixel.h"
#include "gwnavgeneration/boundary/boundaryedge.h"
#include "gwnavgeneration/boundary/boundaryvertex.h"
#include "gwnavgeneration/boundary/boundarysimplifypolyline.h"
#include "gwnavgeneration/boundary/boundarypolygon.h"
#include "gwnavgeneration/boundary/boundarysimplifiedpolygon.h"
#include "gwnavgeneration/boundary/connectedpattern.h"
#include "gwnavgeneration/common/boxofarrays.h"
#include "gwnavgeneration/common/boxoflists.h"
#include "gwnavgeneration/common/stllist.h"
#include "gwnavgeneration/containers/tlsarray.h"
#include "gwnavruntime/base/types.h"


namespace Kaim
{

class DynamicNavRasterCell;
class GeneratorSystem;
class ConnectionSquare;

/*
          exclusive_MaxX
exclusive_MinX     |
       |           |
 +---+---+---+---+---+---+
 |   |   |   |   |   |   |
 +---#################---+
 |   #   |   |   |   #   | exclusive_Max_Y
 +---#---+---+---+---#---+
 |   #   |   |   |   #   |
 +---#---+---+---+---#---+
 |   #   |   |   |   #   |
 +---#---+---+---+---#---+
 |   #   |   |   |   #   | exclusive_Min_Y
 +---#################---+
 |   |   |   |   |   |   |
 +---+---+---+---+---+---+
*/
class BoundaryGraph
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	BoundaryGraph(const DynamicNavRasterCell* navRaster);

	~BoundaryGraph();

	KyResult Build();

	KyResult WriteIntermediateBoundaryGraphFile();

	KyUInt32 VerticesCount()           const { return m_vertices.GetElementCount();           }
	KyUInt32 EdgesCount()              const { return m_edges.GetElementCount();              }
	KyUInt32 ContoursCount()           const { return m_contours.GetElementCount();           }
	KyUInt32 PolygonsCount()           const { return m_polygons.GetCount();                  }
	KyUInt32 SimplifyPolylinesCount()  const { return m_simplifyPolylines.GetCount();         }
	KyUInt32 SimplifiedEdgesCount()    const { return m_simplifiedEdges.GetElementCount();    }
	KyUInt32 SimplifiedContoursCount() const { return m_simplifiedContours.GetElementCount(); }
	KyUInt32 SimplifiedPolygonsCount() const { return m_simplifiedPolygons.GetCount();        }

private:
	BoundaryEdge* AddEdge(CardinalDir dir, BoundaryEdgeType type, BoundaryPixel* leftPixel, KyUInt32 leftColor);
	
	// 1. Replicate navRaster structure with special treatment on corners
	void BuildBoundaryPixelColumns();

	// 2. BuildEdge along each pixel and its north neighbor
	void BuildEdgesBetweenVerticalNeighbors();

	// 3. BuildEdge along each pixel and its east neighbor
	void BuildEdgesbetweenHorizontalNeighbors();

	// 5. Based on the edges, analyse patterns and build vertices
	KyResult BuildAllVerticesInAllColumns();

	// 6. Follow edges and build contours
	void BuildAllContour();

	// 7. Group polylines in polygons
	void BuildPolygons();
	
	// 8. Simplify polylines
	void BuildSimplifyPolylinesFromContour();

	// 9. Do perform the polyline simplification
	KyResult SimplifyAllPolylines();

	// 10. Reassociate simplified polygons
	void BuildSimplifiedPolygons();

	// Misc support functions ...
	void InitBoundaryPixel(BoundaryPixel& BoundaryPixel);


	KyUInt32 ProcessEdgeColumn(const PixelPos& currentPixelPos, const PixelPos& nextPos, CardinalDir dir);
	KyUInt32 ProcessBirDirEdge(CardinalDir dir, const PixelPos& currentPos, const PixelPos& nextPos, BoundaryPixel* currentPixel, BoundaryPixel* nextPixel);
	void AddObstacleEdge(BoundaryPixel* boundaryPixel, CardinalDir dir);
	BoundaryEdge* AddEdgeInPixel(BoundaryPixel* boundaryPixel, CardinalDir dirLocationOfEdgeIdPixel, BoundaryEdgeType type);

	KyResult BuildVerticesColumn(ConnectionSquare& connectionSquare, const NavBoundaryPos& pos);
	void AddVertexColumnToBuild(const NavBoundaryPos& pos);
	void SetupSquarePixelColumn(ConnectionSquare& connectionSquare, KyUInt32 idxInSquare, const NavBoundaryPos& pos);
	void BuildContour(BoundaryEdge* beginEdge);

	void BuildSimplifyPolylinesInContour(BoundaryContour& contour);
	void BuildSimplifyPolyline_Cycle(BoundaryContour& contour);
	void BuildSimplifyPolylines_NonCycle(BoundaryContour& contour, BoundaryEdge* firstEdgeWithStaticStart);
	void BuildSimplifiedContour(BoundaryContour& contour, BoundarySimplifiedContour& simplifiedContour);
	void AddSimplifiedEdge(BoundarySimplifiedContour* simplifiedContour, BoundaryEdge* firstEdge, BoundaryEdge* lastEdge);

	bool IsOutsideWest(NavPixelCoord x)                          const;
	bool IsOutsideEast(NavPixelCoord x)                          const;
	bool IsOutsideSouth(NavPixelCoord y)                         const;
	bool IsOutsideNorth(NavPixelCoord y)                         const;
	bool IsOutsideExclusiveBox(const PixelPos& pos)              const;
	bool IsOutsideExclusiveBox(NavPixelCoord x, NavPixelCoord y) const;

	KyResult SimplifyPolylines();

	// Temporary
	bool IsInputNavRasterValid();
public:
	GeneratorSystem* m_sys;
	const DynamicNavRasterCell* m_navRaster;
	CellDesc m_cellDesc;
	PixelBox m_navPixelBox; // navRaster PixelBox
	PixelBox m_exclusivePixelBox; // exclusive PixelBox (without overlap)

	BoxOfArrays<BoundaryPixel> m_boxOfBoundaryPixelColumn; // association from NavPixels to BoundaryEdges

	GrowingPool<BoundaryEdge> m_edges;
	GrowingPool<BoundaryEdge> m_outsideEdges;
	GrowingPool<BoundaryVertex> m_vertices;
	GrowingPool<BoundaryContour> m_contours;

	KyArrayTLS<BoundaryPolygon> m_polygons;

	BitFieldTLS m_registerVertexColumnsToBuild;
	KyArrayTLS_POD<NavBoundaryPos> m_vertexColumnsToBuild;

	KyArrayTLS<BoundarySimplifyPolyline*> m_simplifyPolylines;

	GrowingPool<BoundarySimplifiedEdge> m_simplifiedEdges;
	GrowingPool<BoundarySimplifiedContour> m_simplifiedContours;
	KyArrayTLS<BoundarySimplifiedPolygon> m_simplifiedPolygons;

	KyArrayTLS<ConnectedPattern> m_connectPatterns;
};

KY_INLINE bool BoundaryGraph::IsOutsideWest(NavPixelCoord x)  const { return x == 0; }
KY_INLINE bool BoundaryGraph::IsOutsideEast(NavPixelCoord x)  const { return x == m_navPixelBox.CountX() - 1; }
KY_INLINE bool BoundaryGraph::IsOutsideSouth(NavPixelCoord y) const { return y == 0; }
KY_INLINE bool BoundaryGraph::IsOutsideNorth(NavPixelCoord y) const { return y == m_navPixelBox.CountY() - 1; }
KY_INLINE bool BoundaryGraph::IsOutsideExclusiveBox(const PixelPos& pos) const { return IsOutsideExclusiveBox(pos.x, pos.y); }
KY_INLINE bool BoundaryGraph::IsOutsideExclusiveBox(NavPixelCoord x, NavPixelCoord y) const
{
	return IsOutsideWest(x) || IsOutsideEast(x) || IsOutsideSouth(y) || IsOutsideNorth(y);
}

KY_INLINE void BoundaryGraph::SetupSquarePixelColumn(ConnectionSquare& connectionSquare, KyUInt32 idxInSquare, const NavBoundaryPos& pos)
{
	const BoxOfArrays<BoundaryPixel>::Column& boundaryPixelColumn = m_boxOfBoundaryPixelColumn.GetColumn(pos);
	connectionSquare.SetupSquarePixelColumn(idxInSquare, pos, boundaryPixelColumn, IsOutsideExclusiveBox(pos));
}

}


#endif
