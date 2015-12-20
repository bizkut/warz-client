/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_BoundarySimplifyPolyline_H
#define GwNavGen_BoundarySimplifyPolyline_H


#include "gwnavgeneration/boundary/boundaryedge.h"
#include "gwnavgeneration/boundary/boundaryvertex.h"
#include "gwnavgeneration/boundary/boundarypolygon.h"
#include "gwnavgeneration/common/stllist.h"


namespace Kaim
{


class BoundarySimplifyVertex
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	BoundarySimplifyVertex() : m_vertex(KY_NULL), m_inSmallEdge(KY_NULL), m_outSmallEdge(KY_NULL) {}

	BoundarySimplifyVertex(BoundaryEdge* inSmallEdge, BoundaryVertex* vertex, BoundaryEdge* outSmallEdge)
	{
		m_pos.x = (KyFloat32)vertex->m_exclBoundaryPos.x;
		m_pos.y = (KyFloat32)vertex->m_exclBoundaryPos.y;
		m_pos.z = vertex->m_altitude;
		m_vertex = vertex;
		m_inSmallEdge = inSmallEdge;
		m_outSmallEdge = outSmallEdge;
	}

public:
	Vec3f m_pos; // x and y are the Pixel position, their are not real float positions
	BoundaryVertex* m_vertex;
	BoundaryEdge* m_inSmallEdge;
	BoundaryEdge* m_outSmallEdge;
};


// Linked list of BoundaryVertices for simplification usage
class BoundarySimplifyPolyline
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	typedef BoundarySimplifyVertex Vertex;

public:
	BoundarySimplifyPolyline()
	{
		Init(KyUInt32MAXVAL, ContourWinding_Unset, BoundaryEdgeType_Unset, PolylineCycleStatus_Unset);
	}

	void Init(KyUInt32 index, ContourWinding contourWinding, BoundaryEdgeType edgeType, BoundaryPolylineCycleStatus cycle)
	{
		m_index = index;
		m_contourWinding = contourWinding;
		m_edgeType = edgeType;
		m_vertices.clear();
		m_cycleStatus = cycle;
		m_next = KY_NULL;
	}

	void AddVertex(BoundaryEdge* inEdge, BoundaryVertex* vertex, BoundaryEdge* outEdge)
	{
		BoundarySimplifyVertex simplifyVertex(inEdge, vertex, outEdge);
		m_vertices.push_back(simplifyVertex);

		if (inEdge)
			PutEdgeInPolyline(inEdge);

		if (outEdge)
			PutEdgeInPolyline(outEdge);
	}

	void PutEdgeInPolyline(BoundaryEdge* edge)
	{
		edge->m_simplifyPolyline = this;
		edge->m_simplifyPolylineOrder = BoundaryOrder_Straight;

		if (edge->m_pair)
		{
			edge->m_pair->m_simplifyPolyline = this;
			edge->m_pair->m_simplifyPolylineOrder = BoundaryOrder_Reverse;
		}
	}

public:
	KyUInt32 m_index;
	TlsStlList<BoundarySimplifyVertex> m_vertices;
	ContourWinding m_contourWinding;
	BoundaryEdgeType m_edgeType;
	BoundaryPolylineCycleStatus m_cycleStatus; // if (m_cycle == Polyline_Cycle) then m_vertices.first != vertices.last
	BoundarySimplifyPolyline* m_next;
};


}


#endif
