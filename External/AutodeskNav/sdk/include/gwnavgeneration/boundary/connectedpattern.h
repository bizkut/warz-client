/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: GUAL - secondary contact: LASI
#ifndef GwNavGen_ConnectedPattern_H
#define GwNavGen_ConnectedPattern_H

#include "gwnavgeneration/boundary/boundarypixel.h"
#include "gwnavgeneration/boundary/boundaryedge.h"
#include "gwnavgeneration/boundary/boundaryvertex.h"
#include "gwnavgeneration/common/boxofarrays.h"

namespace Kaim
{

static const KyUInt32 MaxVertexEdgeLinkCount = 8;


class VertexEdgeLink
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	enum EdgeInOrOut { EDGE_IN = 0, EDGE_OUT = 1 };

	void Init(EdgeInOrOut edgeInOrOut, BoundaryEdge* edge)
	{
		m_edge = edge;
		m_edgeInOrOut = edgeInOrOut;
		m_vertexIdxInEdge = (edgeInOrOut == EDGE_IN) ? 1 : 0;
	}

	void Apply(BoundaryVertex* center)
	{
		m_edge->m_vertex[m_vertexIdxInEdge] = center; // edge.vertex = center
		KY_ASSERT(m_edge->m_orientation < 4);
		if (m_edgeInOrOut == EDGE_OUT)
			center->m_outs[m_edge->m_orientation] = m_edge; // center.edges = edge
		else
			center->m_ins[m_edge->m_orientation] = m_edge; // center.edges = edge
	}

	BoundaryEdge* m_edge;
	EdgeInOrOut m_edgeInOrOut;
	KyUInt32 m_vertexIdxInEdge;
};


class SquarePixel
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)
public:
	SquarePixel() { Setup(KY_NULL, KY_NULL, false); }

	void Setup(BoundaryPixel* boundaryPixel, KyUInt32 pixelIdxInSquare, bool isOutsideCellBox)
	{
		m_boundaryPixel    = boundaryPixel;
		m_connected[0]     = KY_NULL;
		m_connected[1]     = KY_NULL;
		m_pixelIdxInSquare = pixelIdxInSquare;
		m_isInPattern      = false;
		m_isOutsideCellBox = isOutsideCellBox;
	}

	//  ORTHOGONAL EDGES
	//  +-----------+-----------+   pixel_0.edgeIn  edgeLocationInLeftPixel=0  edgeDir = North = 1
	//  |     1     |     1     |   pixel_1.edgeIn  edgeLocationInLeftPixel=1  edgeDir = West  = 2
	//  |  p3       |       p2  |   pixel_2.edgeIn  edgeLocationInLeftPixel=2  edgeDir = South = 3
	//  |2     out=0|2=in      0|   pixel_3.edgeIn  edgeLocationInLeftPixel=3  edgeDir = East  = 0
	//  |           |           |
	//  |  in=3     N   3=out   |   pixel_0.edgeOut edgeLocationInLeftPixel=1  edgeDir = West  = 2
	//  +---------W-V-E---------+   pixel_1.edgeOut edgeLocationInLeftPixel=2  edgeDir = South = 3
	//  |  out=1    S  in=1     |   pixel_2.edgeOut edgeLocationInLeftPixel=3  edgeDir = East  = 0
	//  |           |           |   pixel_3.edgeOut edgeLocationInLeftPixel=0  edgeDir = North = 1
	//  |2      in=0|2=out     0|
	//  |  p0       |       p1  |   edgeDirFromCenter:
	//  |     3     |     3     |   E=East=0  N=North=1  W=West=2  S=South=3
	//  +-----------+-----------+

	void PushVertexEdgeLinks(VertexEdgeLink* vertexEdgeLinks, KyUInt32& vertexEdgeLinksCount) const
	{
		CardinalDir edgeIn_IdxInPixel = m_pixelIdxInSquare;
		CardinalDir edgeOut_IdxInPixel = GetNextCardinalDir_CCW(edgeIn_IdxInPixel);

		BoundaryEdge* edgeIn = m_boundaryPixel->m_edges[edgeIn_IdxInPixel];
		if (edgeIn != KY_NULL)
		{
			KY_ASSERT(vertexEdgeLinksCount < MaxVertexEdgeLinkCount);
			VertexEdgeLink& link = vertexEdgeLinks[vertexEdgeLinksCount++];
			link.Init(VertexEdgeLink::EDGE_IN, edgeIn);
		}

		BoundaryEdge* edgeOut = m_boundaryPixel->m_edges[edgeOut_IdxInPixel];
		if (edgeOut != KY_NULL)
		{
			KY_ASSERT(vertexEdgeLinksCount < MaxVertexEdgeLinkCount);
			VertexEdgeLink& link = vertexEdgeLinks[vertexEdgeLinksCount++];
			link.Init(VertexEdgeLink::EDGE_OUT, edgeOut);
		}
	}

	BoundaryPixel* m_boundaryPixel;
	SquarePixel* m_connected[2];
	KyUInt32 m_pixelIdxInSquare;
	bool m_isInPattern;
	bool m_isOutsideCellBox;
};


class SquarePixelColumn
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	SquarePixelColumn(KyUInt32 squarePixelsCapacityAtInit = 50)
	{
		if (squarePixelsCapacityAtInit == 0)
			squarePixelsCapacityAtInit = 50;
		m_squarePixels.Resize(squarePixelsCapacityAtInit);
	}

	~SquarePixelColumn() {}

	void Setup(KyUInt32 idxInSquare, const NavBoundaryPos& pos, const BoxOfArrays<BoundaryPixel>::Column& boundaryPixelColumn, bool isOutsideNavBox)
	{
		m_pos = pos;
		m_squarePixels.Clear();
		m_squarePixels.Resize(boundaryPixelColumn.m_count);
		for (NavRasterFloorIdx floorIdx = 0; floorIdx < boundaryPixelColumn.m_count; ++floorIdx)
			m_squarePixels[floorIdx].Setup(&boundaryPixelColumn.m_values[floorIdx], idxInSquare, isOutsideNavBox);
	}

	PixelPos m_pos;
	KyArrayTLS_POD<SquarePixel> m_squarePixels;
};


class ConnectionSquare
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	KY_INLINE void SetupSquarePixelColumn(KyUInt32 idxInSquare, const NavBoundaryPos& pos, const BoxOfArrays<BoundaryPixel>::Column& boundaryPixelColumn, bool isOutsideNavBox)
	{
		m_columns[idxInSquare].Setup(idxInSquare, pos, boundaryPixelColumn, isOutsideNavBox);
	}
public:
	SquarePixelColumn m_columns[4];
};


class ConnectedPattern
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	enum { CCW = 0, CW = 1 };

	ConnectedPattern() : m_count(0)
	{
		for(KyUInt32 i = 0; i < 4; ++i)
			m_pixels[i] = KY_NULL;
	}

	void Init(SquarePixel* firstSquarePixel)
	{
		// push first
		firstSquarePixel->m_isInPattern = true;
		m_pixels[0] = firstSquarePixel;
		m_count = 1;

		// propagate CCW and CW
		for(KyUInt32 i = 0; i < 2; ++i)
		{
			SquarePixel* squarePixel = firstSquarePixel;
			for (;;)
			{
				squarePixel = squarePixel->m_connected[i];
				if (squarePixel == KY_NULL || squarePixel->m_isInPattern == true)
					break;
				squarePixel->m_isInPattern = true;
				if (m_count < 4)
					m_pixels[m_count] = squarePixel;
				m_count++;
			}
		}
	}

	KyFloat32 GetCenterAltitude()
	{
		KY_ASSERT(m_count != 0);
		static const KyFloat32 florCountInv[4] = { 1.f, 0.5f, 0.333333333333f, 0.25f };

		KyFloat32 altitude = 0.0f;
		for (KyUInt32 i = 0; i < m_count; ++i)
		{
			KY_ASSERT(m_pixels[i] != KY_NULL);
			altitude += m_pixels[i]->m_boundaryPixel->m_navRasterPixel->m_altitude;
		}
		altitude *= florCountInv[m_count-1];
		return altitude;
	}

	bool IsFullyOutsideCellBox()
	{
		KY_ASSERT(m_count != 0);
		for (KyUInt32 i = 0; i < m_count; ++i)
		{
			if (m_pixels[i]->m_isOutsideCellBox == false)
				return false;
		}
		return true;
	}

	bool IsNarrow()
	{
		bool isBlocked[4] = { false, false, false, false };
		bool isDeadEnd[4] = { false, false, false, false };
		for (KyUInt32 pixelIdx = 0; pixelIdx < m_count; ++pixelIdx)
		{
			SquarePixel* squarePixel = m_pixels[pixelIdx];
			KY_ASSERT(squarePixel != KY_NULL);

			BoundaryPixel* boundaryPixel = squarePixel->m_boundaryPixel;
			KY_ASSERT(boundaryPixel != KY_NULL);
			// Dead end chedc
			// Dead end  == 3 consecutive orthogonal edges
			for (CardinalDir dir = 0; dir < 4; ++dir)
			{
				// 3 consecutive orthogonal edges == dead end -> NOT necessarily narrow
				const BoundaryEdge* edges[4] = 
				{
					boundaryPixel->m_edges[  dir    ], // current dir
					boundaryPixel->m_edges[(dir+1)%4],
					boundaryPixel->m_edges[(dir+2)%4],
					boundaryPixel->m_edges[(dir+3)%4]
				};

				if (edges[0] != KY_NULL && edges[1] != KY_NULL && edges[2] != KY_NULL)
				{
					isDeadEnd[pixelIdx] = true;
					if (edges[3] != KY_NULL)
						return true; // 4 consecutive edges: isolated pixel <=> narrow otherwise simplified polyline will fail

					break;
				}
			}

			if (isDeadEnd[pixelIdx] == false)
			{
				for (CardinalDir dir = 0; dir < 4; ++dir)
				{
					// 2 opposite orthogonal edges and not deadEnd == 1 pixel corridor -> narrow
					//   =====
					//   | p |
					//   =====
					if (squarePixel->m_boundaryPixel->m_edges[            dir            ] != KY_NULL &&
						squarePixel->m_boundaryPixel->m_edges[GetOppositeCardinalDir(dir)]!= KY_NULL  )
						return true;
				}
			}
	
			//			2
			//   +--e2--+---e2-+         pixel 0: (right) e4 -> vertex 3 is blocked
			//   |	    |	   |				  (up)    e6 -> vertex 0 is blocked
			//   e4	 p3 | p2   e0		 pixel 1: (right) e6 -> vertex 0 is blocked
			// 3 +------+----- + 1				  (up)    e0 -> vertex 1 is blocked
			//   |	 p0 | p1   |		 pixel 2: (right) e0 -> vertex 1 is blocked
			//   e4	    |	   e0				  (up)    e2 -> vertex 2 is blocked
			//   +---e6-+--e6--+		 pixel 3: (right) e2 -> vertex 2 is blocked
			//	    	0						  (up)    e4 -> vertex 3 is blocked

			KyUInt32 rightEdgeIdx = (squarePixel->m_pixelIdxInSquare + 2) % 4; // ~= opposite
			KyUInt32 upEdgeIdx    = (squarePixel->m_pixelIdxInSquare + 3) % 4;
			KyUInt32 rightVtxIdx  = (squarePixel->m_pixelIdxInSquare + 3) % 4;
			KyUInt32 upVtxIdx = (squarePixel->m_pixelIdxInSquare);

			if (isBlocked[rightVtxIdx] == false && squarePixel->m_boundaryPixel->m_edges[rightEdgeIdx])
				isBlocked[rightVtxIdx] = true;

			if (isBlocked[upVtxIdx] == false  && squarePixel->m_boundaryPixel->m_edges[upEdgeIdx])
				isBlocked[upVtxIdx] = true;
		}

		//   2 U shaped pixels attached = narrow corridor !
		//   ==========
		//   | p0 - p1 |
		//   ==========
		//
		for (KyUInt32 j = 0; j < 4; ++j)
		{
			if (isDeadEnd[j] && isDeadEnd[(j+1)%4])
				return true;
		}

		//if a vertex is blocked but no edge links pattern center to it, we have a narrow pattern
		for (KyUInt32 pixelIdx = 0; pixelIdx < m_count; ++pixelIdx)
		{
			if (isDeadEnd[pixelIdx])
				continue;

			SquarePixel* squarePixel = m_pixels[pixelIdx];
			KY_ASSERT(squarePixel != KY_NULL);

			SquarePixel* neighborCCW = squarePixel->m_connected[CCW];
			if (neighborCCW != KY_NULL)
			{
				KyUInt32 ccwVtxIdx = (squarePixel->m_pixelIdxInSquare);
				if (isBlocked[ccwVtxIdx] && 
					neighborCCW->m_boundaryPixel->m_floorColor     == squarePixel->m_boundaryPixel->m_floorColor     &&
					neighborCCW->m_boundaryPixel->m_connexIdx == squarePixel->m_boundaryPixel->m_connexIdx )
					return true;
			}

			SquarePixel* neighborCW = squarePixel->m_connected[CW];
			if (neighborCW != KY_NULL)
			{
				KyUInt32 cwVtxIdx = (squarePixel->m_pixelIdxInSquare + 3) % 4;
				if (isBlocked[cwVtxIdx] && 
					neighborCW->m_boundaryPixel->m_floorColor     == squarePixel->m_boundaryPixel->m_floorColor     &&
					neighborCW->m_boundaryPixel->m_connexIdx == squarePixel->m_boundaryPixel->m_connexIdx )
				return true;
			}
		}
		return false;
	}

	KyUInt32 m_count; // in [0; 4]
	SquarePixel* m_pixels[4];
};
}


#endif
