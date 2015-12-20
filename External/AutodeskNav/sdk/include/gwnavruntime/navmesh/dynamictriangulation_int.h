/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: JUBA - secondary contact: LASI
#ifndef Navigation_DynamicTriangulation_Integer_H
#define Navigation_DynamicTriangulation_Integer_H

#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/containers/collection.h"
#include "gwnavruntime/basesystem/logger.h"

namespace Kaim
{

class DynTriHalfEdge;
class IndexedTriangleSoup2i;
class Triangle3i;

typedef CoordPos AlignedCoordPos;

enum DynamicVertexIntUserDataContent
{
	DTI_UNKNOWN_VERTEX_USERDATA,
	DTI_TRIANGLE_SOUP_VERTEX_TAG_DATA
};
// ----------------------------
// ----- DynamicVertex_Int --------
// ----------------------------
class DynTriVertex
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	DynTriVertex() : m_idx(KyUInt32MAXVAL), m_coordPos(KyInt32MAXVAL, KyInt32MAXVAL), m_altitude(KyFloat32MAXVAL), m_userData(KY_NULL)
	{
		m_outGoingEdgeIdx.Reserve(12);
	}


	KY_INLINE void InsertInOutGoingEdges(KyUInt32 dynamicHalfEdgeIdx)
	{
		KY_ASSERT(m_outGoingEdgeIdx.DoesContain(dynamicHalfEdgeIdx) == false);
		m_outGoingEdgeIdx.PushBack(dynamicHalfEdgeIdx);
		KY_ASSERT(m_outGoingEdgeIdx.DoesContain(dynamicHalfEdgeIdx));
	}

	KY_INLINE void RemoveFromOutGoingEdges(KyUInt32 dynamicHalfEdgeIdx)
	{
		KY_ASSERT(m_outGoingEdgeIdx.DoesContain(dynamicHalfEdgeIdx));
		m_outGoingEdgeIdx.RemoveFirstOccurrence(dynamicHalfEdgeIdx);
		KY_ASSERT(m_outGoingEdgeIdx.DoesContain(dynamicHalfEdgeIdx) == false);
	}

public:
	KyUInt32 m_idx;
	AlignedCoordPos m_coordPos;
	KyFloat32 m_altitude;
	Collection<KyUInt32> m_outGoingEdgeIdx;

	void* m_userData;
};

// ------------------------------
// ----- DynamicTriangle_Int --------
// ------------------------------
class DynTriTriangle
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	DynTriTriangle() : m_idx(KyUInt32MAXVAL), m_edgeIdx(KyUInt32MAXVAL), m_userData(KY_NULL) {}

	void GetLocalTriangle3i(KyUInt32 pixelSize, KyFloat32 rasterPrecision, Triangle3i& triangle3i) const;
public:
	KyUInt32 m_idx;
	KyUInt32 m_edgeIdx; // one edge around the face

	void* m_userData;
};

// ---------------------------
// ----- DynamicHalfEdge_Int -----
// ---------------------------
class DynTriHalfEdge
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	DynTriHalfEdge() : m_idx(KyUInt32MAXVAL), m_startVertexIdx(KyUInt32MAXVAL), m_triangleIdx(KyUInt32MAXVAL),
		m_nextEdgeIdx(KyUInt32MAXVAL), m_pairEdgeIdx(KyUInt32MAXVAL), m_status(HalfEdgeStatus_NotOpen), m_userData(KY_NULL) {}

	enum HalfEdgeStatus
	{
		HalfEdgeStatus_Open = 0,
		HalfEdgeStatus_NotOpen = 1,
		HalfEdgeStatus_Locked = 2,

		HalfEdgeStatus_FORCE32 = 0xFFFFFFFF
	};

	KY_INLINE bool IsABorder() const { return m_pairEdgeIdx == KyUInt32MAXVAL; }
	KY_INLINE bool IsLocked() const { return m_status == HalfEdgeStatus_Locked; }

public:
	KyUInt32 m_idx;
	KyUInt32 m_startVertexIdx;
	KyUInt32 m_triangleIdx;
	KyUInt32 m_nextEdgeIdx;
	KyUInt32 m_pairEdgeIdx;
	HalfEdgeStatus m_status;
	void* m_userData;
};

/*	Instance of this class are returned by DynamicTriangulation_Int::InsertANewVertexInMeshAndUpdateTriangulation
	if m_result == RESULT_NEW_VERTEX_CREATED => m_vertex refers to the new created vertex
	if m_result == RESULT_CLOSE_TO_EXISTING_VERTEX => m_vertex refers to the vertex we are close to
	if m_result == RESULT_CLOSE_TO_BORDER_EDGE => m_ halfEdge refers to the border HalfEdge we are close to
	if m_result == RESULT_NOT_IN_TRIANGLE => no triangle contains input position. No usefull datan, m_vertex and m_halfEdge remains to KY_NULL
	if m_result == RESULT_REFUSED => position refused becaus of floating precision issue (we don't want to create reverse face). No usefull data,  m_vertex and m_halfEdge remains to KY_NULL
*/

class DynTriangulationInsertionResult
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:

	DynTriangulationInsertionResult() : m_result(RESULT_FORCE32), m_vertexIdx(KyUInt32MAXVAL), m_halfEdgeIdx(KyUInt32MAXVAL) {}

	enum Result
	{
		RESULT_NEW_VERTEX_CREATED = 0,
		RESULT_EXISTING_VERTEX = 1,
		RESULT_ON_A_BORDER_EDGE = 2,
		RESULT_NOT_IN_TRIANGLE = 3,
		RESULT_REFUSED = 4,
		RESULT_FORCE32 = 0xFFFFFFFF
	};

	Result m_result;
	KyUInt32 m_vertexIdx;
	KyUInt32 m_halfEdgeIdx;
};


// --------------------------------
// ----- DynamicTriangulation_Int -----
// --------------------------------
class DynamicTriangulation
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(DynamicTriangulation)
public:
	KY_INLINE DynamicTriangulation(MemoryHeap* heap) :
		m_triangles(heap),
		m_vertices(heap),
		m_edges(heap), 
		m_edgeIdxToProcess(heap), 
		m_userDataContent(DTI_UNKNOWN_VERTEX_USERDATA)
	{}

	~DynamicTriangulation() { Clear(); }

	void Clear();


	// mesh Creation
	KyUInt32 AddVertex(const AlignedCoordPos& coordPos, KyFloat32 altitude = KyFloat32MAXVAL);
	KyUInt32 AddTriangle(KyUInt32 v0Idx, KyUInt32 v1Idx, KyUInt32 v2Idx);

	// Use this after calling Resize with good value on m_vertices and m_edges
	void AddVertex(KyUInt32 vertexIdx, const AlignedCoordPos& coordPos, KyFloat32 altitude = KyFloat32MAXVAL);
	//KyUInt32 AddTriangle(KyUInt32 triangleIdx, KyUInt32 v0Idx, KyUInt32 v1Idx, KyUInt32 v2Idx);


	KyResult BuildFromIndexedTriangleSoup(const IndexedTriangleSoup2i* triangulation);
	KyResult BuildIndexedTriangleSoup(IndexedTriangleSoup2i* triangulation);

	// Process the triangulation to get a "Delaunay Like" triangulation
	// This function should be called before inserting new vertices and Updating Triangulation
	void GetABetterTriangulation();

	/*-------------- Functions used for better altitude approximation -------------- */

	// Add a new vertex in the Mesh and triangulate around him
	// return KyUInt32MAXVAL if
	//     - the position isn't in any triangle
	//     - the position is too close to one of the three vertices of the triangle (according to m_squareMinDistFromExistingPoint, distances are in 2D)
	//     - the position is too close to a border edge (according to m_squareMinDistFromBorder, distances are in 2D)
	// If the position is close to an edge (according to m_maxSquareDistToConsiderAPointOnAnEdge, distances are in 2D), the vertex is snapped on the edge
	KyUInt32 InsertANewVertexInMesh(const CoordPos& position, DynTriangulationInsertionResult& result);

	// perform an InsertANewVertexInMesh and update the triangulation to get a "Delauney Like" triangulation
	// if impactedTrianglesIndices is not null, fill it with the indices of modified triangles
	DynTriangulationInsertionResult InsertANewVertexInMeshAndUpdateTriangulation(
		const CoordPos& position, KyArrayDH_POD<KyUInt32>* impactedTrianglesIndices = KY_NULL);

	KY_INLINE void LockEdge(KyUInt32 halfEdgeIdx)
	{
		DynTriHalfEdge& edge = m_edges[halfEdgeIdx];

		if (edge.m_status != DynTriHalfEdge::HalfEdgeStatus_Locked)
		{
			KY_DEBUG_ASSERTN(edge.m_status == DynTriHalfEdge::HalfEdgeStatus_NotOpen ,("Don't change edge status during process"));
			edge.m_status = DynTriHalfEdge::HalfEdgeStatus_Locked;
		}

		if (edge.IsABorder() == false && m_edges[edge.m_pairEdgeIdx].m_status != DynTriHalfEdge::HalfEdgeStatus_Locked)
		{
			KY_DEBUG_ASSERTN(m_edges[edge.m_pairEdgeIdx].m_status == DynTriHalfEdge::HalfEdgeStatus_NotOpen ,("Don't change edge status during process"));
			m_edges[edge.m_pairEdgeIdx].m_status = DynTriHalfEdge::HalfEdgeStatus_Locked;
		}
	}


	void GetLocalTriangle3i(KyUInt32 triangleIdx, KyUInt32 pixelSize, KyFloat32 rasterPrecision, Triangle3i& triangle3i) const;

public: // internal
	KyResult FlipEdge(KyUInt32 halfEdgeIdx);

	void UpdateTriangulationAfterVertexInsertion(KyUInt32 newVertexIdx);

	bool IsEdgeLegal(KyUInt32 halfEdgeIdx);
	bool IsEdgeLegalAfterPointInsertion(KyUInt32 halfEdgeIdx, KyUInt32 newVertexIdx);

	// dynamic mesh modification
	KyUInt32 FindTriangleThatContainsAPosition(const CoordPos& position, DynTriangulationInsertionResult& result);

	void InsertANewVertexInsideATriangle(KyUInt32 newVertexIdx, KyUInt32 triangleIdx);
	void InsertANewVertexInTheMiddleOfAnEdge(KyUInt32 newVertexIdx, KyUInt32 halfEdgeIdx);
	void InsertANewVertexInTheMiddleOfABorderEdge(KyUInt32 newVertexIdx, KyUInt32 halfEdgeIdx);

	void AddToEdgeToProcessIfNotOpen(KyUInt32 halfEdgeIdx);
	void AddToEdgeToProcessIfIllegalAndNotOpen(KyUInt32 halfEdgeIdx);

	static bool IsQuadrilateralConvex(const AlignedCoordPos& a, const AlignedCoordPos& b, const AlignedCoordPos& c, const AlignedCoordPos& d);
private :
	KyUInt32 GetNewDynamicVertex();
	KyUInt32 GetNewDynamicHalfEdge();
	KyUInt32 GetNewDynamicTriangle();

public:
	KyArrayDH<DynTriTriangle> m_triangles;
	KyArrayDH<DynTriVertex> m_vertices;
	KyArrayDH<DynTriHalfEdge> m_edges;

	KyArrayDH_POD<KyUInt32> m_edgeIdxToProcess;

	DynamicVertexIntUserDataContent m_userDataContent;
};


KY_INLINE void DynamicTriangulation::AddVertex(KyUInt32 vertexIdx, const AlignedCoordPos& coordPos, KyFloat32 altitude)
{
	KY_ASSERT(vertexIdx < m_vertices.GetCount());
	KY_LOG_ERROR_IF(((coordPos.x & 0xFFFFFF00) != 0 || (coordPos.y & 0xFFFFFF00) != 0), ("Invalid coordinates : %d - %d", coordPos.x, coordPos.y));
	DynTriVertex& vertex = m_vertices[vertexIdx];

	vertex.m_idx = vertexIdx;
	vertex.m_coordPos = coordPos;
	vertex.m_altitude = altitude;
}
}


#endif //Navigation_DynamicTriangulation_Integer_H

