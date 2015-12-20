/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_BoundaryGraphBlobVisualGeometryBuilder_H
#define GwNavGen_BoundaryGraphBlobVisualGeometryBuilder_H

#include "gwnavruntime/visualsystem/ivisualgeometrybuilder.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavgeneration/boundary/boundarytypes.h"
#include "gwnavgeneration/navraster/navrastertypes.h"


namespace Kaim
{

class VisualGeometryBuilder;
class BoundaryGraphBlob;
class BoundaryGraphBlobVertex;
class BoundaryGraphBlobEdge;
class BoundaryGraphBlobContour;
class BoundaryGraphBlobPolygon;
class BoundaryGraphBlobSimplifyPolyline;
class BoundaryGraphBlobSimplifiedHalfEdge;
class BoundaryGraphBlobSimplifiedContour;
class BoundaryGraphBlobSimplifiedPolygon;
class VisualColor;


class BoundaryGraphBlobVisualGeometryBuilder : public IVisualGeometryBuilder
{
public:
	typedef BoundaryGraphBlobVertex             VertexBlob;
	typedef BoundaryGraphBlobEdge               EdgeBlob;
	typedef BoundaryGraphBlobContour            ContourBlob;
	typedef BoundaryGraphBlobPolygon            PolygonBlob;
	typedef BoundaryGraphBlobSimplifyPolyline   SimplifyPolylineBlob;
	typedef BoundaryGraphBlobSimplifiedHalfEdge SimplifiedEdgeBlob;
	typedef BoundaryGraphBlobSimplifiedContour  SimplifiedContourBlob;
	typedef BoundaryGraphBlobSimplifiedPolygon  SimplifiedPolygonBlob;

public:
	enum EdgeColorScheme
	{
		FromEdgeType,
		FromContourWinding,
		FromPolygonIdx,
		FromPolygonColor
	};

	class EdgeVisualInfo
	{
	public:
		EdgeVisualInfo() :
			m_polygonColor(PixelColor_Unset), m_contourWinding(ContourWinding_Unset), m_polygonIdx(0) {}

	public:
		PixelColor m_polygonColor;
		ContourWinding m_contourWinding;
		KyUInt32 m_polygonIdx;
	};

public:
	BoundaryGraphBlobVisualGeometryBuilder(BoundaryGraphBlob* boundaryGraphBlob);

	void SetEdgeColorScheme(EdgeColorScheme edgeColorScheme);

	virtual void DoBuild();

private:
	Vec3f GetVertexPos(const VertexBlob& vertex);

	void BuildStaticVerticesGeometry(VisualColor color);
	void BuildNarrowVerticesGeometry(VisualColor color);

	void BuildEdgeGeometry(const EdgeBlob& edge, EdgeVisualInfo& edgeVisualInfo);
	void BuildContourGeometry(const ContourBlob& contour, EdgeVisualInfo& edgeVisualInfo);
	void BuildPolygonsGeometry();

	void BuildSimplifiedEdgeGeometry(const SimplifiedEdgeBlob& simplifiedEdge, EdgeVisualInfo& edgeVisualInfo);
	void BuildSimplifiedContourGeometry(const SimplifiedContourBlob& contour, EdgeVisualInfo& edgeVisualInfo);
	void BuildSimplifiedPolygonsGeometry();

	void BuildSimplifyPolylinesGeometry();
	void BuildSimplifySegmentGeometry(const VertexBlob& start, const VertexBlob& end,  KyUInt32 polylineIdx);

	VisualColor GetEdgeColor(const EdgeVisualInfo& edgeVisualInfo, const BoundaryEdgeType& edgeType);

private:
	const BoundaryGraphBlob* m_graph;
	KyFloat32 m_rasterPrecision;
	EdgeColorScheme m_edgeColorScheme;
	const VertexBlob* m_vertices;
	const EdgeBlob* m_edges;
	const ContourBlob* m_contours;
	const PolygonBlob* m_polygons;
	const SimplifyPolylineBlob* m_simplifyPolylines;
	const SimplifiedEdgeBlob* m_simplifiedEdges;
	const SimplifiedContourBlob* m_simplifiedContours;
	const SimplifiedPolygonBlob* m_simplifiedPolygons;
};


}


#endif //GwNavGen_BoundaryGraphBlobVisualGeometryBuilder_H
