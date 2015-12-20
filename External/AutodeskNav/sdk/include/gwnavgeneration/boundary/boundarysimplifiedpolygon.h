/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_BoundarySimplifiedPolygon_H
#define GwNavGen_BoundarySimplifiedPolygon_H


#include "gwnavgeneration/boundary/boundarytypes.h"
#include "gwnavgeneration/navraster/navrastertypes.h"
#include "gwnavgeneration/containers/tlsarray.h"


namespace Kaim
{

class BoundaryEdge;
class BoundaryVertex;
class BoundarySimplifiedContour;
class BoundarySimplifiedEdge;
class DynamicNavHalfEdge;

class BoundarySimplifiedPolygon
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	BoundarySimplifiedPolygon() : m_leftColor(PixelColor_Unset) {}

public:
	PixelColor m_leftColor;
	KyArrayTLS_POD<BoundarySimplifiedContour*> m_simplifiedContours;
};

class BoundarySimplifiedContour
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	BoundarySimplifiedContour();

	void Init(KyUInt32 index, PixelColor leftColor, KyUInt32 leftNavTagIdx, BoundarySimplifiedPolygon* polygon);
	void PushEdge(BoundarySimplifiedEdge* edge);

public:
	KyUInt32 m_index;
	KyUInt32 m_edgeCount;
	BoundarySimplifiedEdge* m_begin;
	BoundarySimplifiedEdge* m_last;
	ContourWinding m_winding;
	PixelColor m_leftColor;
	KyUInt32 m_leftConnexIdx;
	BoundarySimplifiedPolygon* m_polygon;
};


class BoundarySimplifiedEdge
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	enum BoundarySimplifiedEdge_Status
	{
		BoundarySimplifiedEdge_ACTIVE, 
		BoundarySimplifiedEdge_DISCARDED 
	};

	BoundarySimplifiedEdge();

	void Init(KyUInt32 index, BoundaryEdge* firstEdge, BoundaryEdge* lastEdge, BoundarySimplifiedContour* contour);

public:
	KyUInt32 m_index;
	BoundaryEdgeType m_edgeType;
	BoundaryEdge* m_firstEdge;
	BoundaryEdge* m_lastEdge;
	BoundaryVertex* m_vertex[2];
	PixelColor m_leftColor;
	KyUInt32 m_leftNavTagIdx;
	BoundarySimplifiedContour* m_contour;
	BoundarySimplifiedEdge* m_pair;
	BoundarySimplifiedEdge* m_next;
	KyFloat32 m_accumulatedInteriorPixelError;
	KyFloat32 m_accumulatedExteriorPixelError;
	KyFloat32 m_accumulatedSurfaceError;
	BoundarySimplifiedEdge_Status m_status;
	KyUInt32 m_edgeIdxInDynNavFloor;
};


}


#endif
