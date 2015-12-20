/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_BoundaryEdge_H
#define GwNavGen_BoundaryEdge_H


#include "gwnavgeneration/boundary/boundarytypes.h"
#include "gwnavgeneration/boundary/boundarypixel.h"
#include "gwnavgeneration/boundary/boundaryvertex.h"


namespace Kaim
{

class BoundaryVertex;
class BoundaryPixel;
class BoundaryContour;
class BoundarySimplifyPolyline;
class BoundarySimplifiedEdge;


class BoundaryEdge
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	void Init(CardinalDir dir, BoundaryEdgeType type, bool isInside, KyUInt32 leftColor, KyUInt32 leftNavTagIdx, KyUInt32 index);

	void ComputeNextEdge();

	bool IsInContour()            const;
	bool IsOutside()              const;
	bool HasDiscontinuityVertex() const;

public:
	KyUInt32 m_index;

	CardinalDir m_orientation;
	BoundaryEdgeType m_type;
	BoundaryVertex* m_vertex[2];
	KyUInt32 m_leftColor;
	KyUInt32 m_leftConnexIdx;
	bool m_isOutside;
	KyUInt32 m_leftPixelFeatureTag;
	BoundaryEdge* m_pair;

	// contour related
	BoundaryContour* m_contour;
	BoundaryEdge* m_next;

	// simplifyPolyline related
	BoundarySimplifyPolyline* m_simplifyPolyline;
	BoundaryOrder m_simplifyPolylineOrder;

	// simplifiedEdge related
	BoundarySimplifiedEdge* m_simplifiedEdge;
};


KY_INLINE void BoundaryEdge::Init(CardinalDir dir, BoundaryEdgeType type, bool isOutside, KyUInt32 leftColor, KyUInt32 leftNavTagIdx, KyUInt32 index)
{
	m_index                 = index;
	m_orientation           = dir;
	m_type                  = type;
	m_vertex[0]             = KY_NULL;
	m_vertex[1]             = KY_NULL;
	m_leftColor             = leftColor;
	m_leftConnexIdx         = leftNavTagIdx;
	m_isOutside             = isOutside;
	m_leftPixelFeatureTag   = UNDEFINED_NAVRASTER_FEATURE;
	m_pair                  = KY_NULL;
	m_contour               = KY_NULL;
	m_next                  = KY_NULL;
	m_simplifyPolyline      = KY_NULL;
	m_simplifyPolylineOrder = BoundaryOrder_Unset;
	m_simplifiedEdge        = KY_NULL;
}

KY_INLINE bool BoundaryEdge::IsInContour()            const { return m_contour          != KY_NULL; }
KY_INLINE bool BoundaryEdge::IsOutside()              const { return m_isOutside;                   }
KY_INLINE bool BoundaryEdge::HasDiscontinuityVertex() const
{
	return (m_vertex[0] != KY_NULL && m_vertex[0]->IsDiscontinuity()) ||
		   (m_vertex[1] != KY_NULL && m_vertex[1]->IsDiscontinuity());
}

}


#endif
