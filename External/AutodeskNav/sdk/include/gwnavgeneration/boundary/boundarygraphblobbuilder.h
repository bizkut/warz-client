/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_BoundaryGraphBlobBuilder_H
#define GwNavGen_BoundaryGraphBlobBuilder_H


#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavgeneration/boundary/boundarygraphblob.h"


namespace Kaim
{

class BoundaryGraph;
class BoundaryVertex;
class BoundaryEdge;
class BoundaryContour;
class BoundaryPolygon;
class BoundarySimplifyPolyline;
class BoundarySimplifiedEdge;
class BoundarySimplifiedContour;
class BoundarySimplifiedPolygon;


class BoundaryGraphBlobBuilder : public BaseBlobBuilder<BoundaryGraphBlob>
{
public:
	BoundaryGraphBlobBuilder(BoundaryGraph* boundaryGraph) :
		m_boundaryGraph(boundaryGraph)
	{}

private:
	virtual void DoBuild();

	void FillVertices(BoundaryGraphBlob::Vertex* blobVertices);
	void FillVertex(const BoundaryVertex& dynVertex, const PixelPos& offsetPos, BoundaryGraphBlob::Vertex& blobVertex);

	void FillEdges(BoundaryGraphBlob::Edge* blobEdges);
	void FillEdge(const BoundaryEdge& dynEdge, BoundaryGraphBlob::Edge& blobEdge);

	void FillContours(BoundaryGraphBlob::Contour* blobContours);
	void FillContour(const BoundaryContour& dynContour, BoundaryGraphBlob::Contour& blobContour);

	void BuildPolygons();
	void BuildPolygon(const BoundaryPolygon* dynPolygon, BoundaryGraphBlob::Polygon* blobPolygon);

	void BuildSimplifyPolylines();
	void BuildSimplifyPolyline(const BoundarySimplifyPolyline& dynPolyline, BoundaryGraphBlob::SimplifyPolyline& blobPolyline);

	void FillSimplifiedEdges(BoundaryGraphBlob::SimplifiedHalfEdge* blobEdges);
	void FillSimplifiedEdge(const BoundarySimplifiedEdge& dynSimplifiedEdge, BoundaryGraphBlob::SimplifiedHalfEdge& blobSimplifiedEdge);

	void FillSimplifiedContours(BoundaryGraphBlob::SimplifiedContour* blobSimplifiedContours);
	void FillSimplifiedContour(const BoundarySimplifiedContour& dynSimplifiedContour, BoundaryGraphBlob::SimplifiedContour& blobSimplifiedContour);

	void BuildSimplifiedPolygons();
	void BuildSimplifiedPolygon(const BoundarySimplifiedPolygon* dynSimplifiedPolygon, BoundaryGraphBlob::SimplifiedPolygon* blobSimplifiedPolygon);

private:
	BoundaryGraph* m_boundaryGraph;
};


}


#endif

