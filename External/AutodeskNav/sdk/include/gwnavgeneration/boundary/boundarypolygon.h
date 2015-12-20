/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_BoundaryPolygon_H
#define GwNavGen_BoundaryPolygon_H


#include "gwnavgeneration/boundary/boundarytypes.h"
#include "gwnavgeneration/boundary/boundaryedge.h"
#include "gwnavgeneration/containers/tlsarray.h"


namespace Kaim
{


class BoundaryOrderedSimplifyPolyline
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	BoundaryOrderedSimplifyPolyline()
		: m_polyline(KY_NULL), m_order(BoundaryOrder_Unset) {}

	BoundaryOrderedSimplifyPolyline(BoundarySimplifyPolyline* polyline, BoundaryOrder order)
		: m_polyline(polyline), m_order(order) {}

public:
	BoundarySimplifyPolyline* m_polyline;
	BoundaryOrder m_order;
};


class BoundaryContour
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	BoundaryContour()
		: m_index(KyUInt32MAXVAL)
		, m_edgeCount(0)
		, m_begin(KY_NULL)
		, m_winding(ContourWinding_Unset)
		, m_leftColor(PixelColor_Unset)
		, m_leftConnexIdx(KyUInt32MAXVAL)
	{}

public:
	KyUInt32 m_index;
	KyUInt32 m_edgeCount;
	BoundaryEdge* m_begin;
	ContourWinding m_winding;
	PixelColor m_leftColor;
	KyUInt32 m_leftConnexIdx;
	KyArrayTLS<BoundaryOrderedSimplifyPolyline> m_orderedSimplifyPolylines;
};


class BoundaryPolygon
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	BoundaryPolygon() : m_leftColor(PixelColor_Unset) {}
public:
	PixelColor m_leftColor; // == idx in the array of boundaryPolygon
	KyArrayTLS_POD<BoundaryContour*> m_contours;
};


}


#endif
