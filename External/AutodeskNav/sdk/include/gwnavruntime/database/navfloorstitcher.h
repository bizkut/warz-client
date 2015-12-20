/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavCellStitcher_H
#define Navigation_NavCellStitcher_H

#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/navmesh/blobs/navvertex.h"
#include "gwnavruntime/kernel/SF_RefCount.h"


namespace Kaim
{

class NavCell;
class NavCellGrid;
class NavCellPosInfo;
class NavFloor;
class Database;
class NavHalfEdgeRawPtr;
class Stitch1To1EdgeLink;
class NavFloorBlob;
class NavHalfEdge;

class EdgeVertices
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	Vec2i m_startVertex;
	Vec2i m_endVertex;
	KyFloat32 m_startAltitude;
	KyFloat32 m_endAltitude;
};

class NavFloorStitcherData
{
public:
	NavFloorStitcherData();

	void Init(NavCell& navCell, NavFloorIdx floorIdx, bool getFutureActiveFloorBlob = false);
	void Init(const Stitch1To1EdgeLink& stitch1To1EdgeLink, bool getFutureActiveFloorBlob = false);

public:
	NavFloor* m_navFloor;
	KyUInt32 m_stitch1To1EdgeCount;
	const NavFloorBlob* m_navFloorBlob;
	const NavHalfEdge* m_halfEdges;
	const NavVertex* m_navVertexBuffer;
	const KyFloat32* m_navVertexAltBuffer;
	NavHalfEdgeRawPtr* m_links;
	const KyUInt16* m_stitch1To1EdgeIdxToFirstIdxBuffer;
	const KyUInt16* m_stitch1To1EdgeIdxToCountBuffer;
	const CompactNavHalfEdgeIdx* m_navHalfEdgeIdxBuffer;
	const Stitch1To1EdgeLink* m_stitch1To1EdgeLinkBuffer;
};

class NavFloorStitcher
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	NavFloorStitcher(Database* database) : m_altitudeTolerance(0.5f), m_database(database) {}

	// --- stitch1To1Edge 
public:
	void UnStitchAll1To1EdgeInNavCell(NavCell& navCell);
	void StitchAllNew1To1EdgeAndTagPotentialRunTimeStitchNeed(const CellPos& cellPos, NavCellPosInfo* navCellPosInfo);

private :
	void RetrieveNeighborCellCandidates(const CellPos& cellPos);
	void StitchAll1To1EdgeInNavCell(NavCell& navCell);
	void TryToLinkCellBoundaryStitch1To1EdgeWithCandidates(NavCell* navCell, KyUInt32 floorIdx, KyUInt32 stitch1To1EdgeIdx,
		const EdgeVertices& edgeVertices, KyArrayPOD<NavCell*, MemStat_NavData>& neihgborCellCandidates, CardinalDir cardinalDirInNeighbor);
	void TryToLinkFloorBoundaryStitch1To1EdgeWithCandidates(NavCell* navCell, KyUInt32 floorIdx, KyUInt32 stitch1To1EdgeIdx, const EdgeVertices& edgeVertices);
	bool DoesFloorCauseRuntimeStitchInNeighbor(NavCell* navCell, KyUInt32 floorIdx);
public:
	void StitchAllNavFloorsOfNavCell(NavCell& navCell);
	void StitchAllNavFloorLink(NavCell& navCell, NavFloorIdx floorIdx);
	bool CanEdgesStitch(EdgeVertices& currentEdgeVertices, EdgeVertices& candidateEdgeVertices, NavHalfEdgeType edgeType);

	void UnStitchAllNavFloorsOfNavCell(NavCell& navCell);


	KyFloat32 m_altitudeTolerance;
	Database* m_database;

	static bool AreVerticesEqualInAltitude(KyFloat32 edge1_startAlt, KyFloat32 edge1_endAlt, KyFloat32 edge2_startAlt, KyFloat32 edge2_endAlt, KyFloat32 altitudeTolerance);

private:
	KyArrayPOD<NavCell*, MemStat_NavData> m_neihgborCellCandidates[4];
};


KY_INLINE bool NavFloorStitcher::AreVerticesEqualInAltitude(KyFloat32 edge1_startAlt, KyFloat32 edge1_endAlt, 
	KyFloat32 edge2_startAlt, KyFloat32 edge2_endAlt, KyFloat32 altitudeTolerance)
{
	const KyFloat32 altitudeDiff1 = fabsf(edge1_startAlt - edge2_endAlt);
	const KyFloat32 altitudeDiff2 = fabsf(edge1_endAlt - edge2_startAlt);
	const KyFloat32 operand1 = Fsel(altitudeDiff1 - altitudeTolerance, 0.f, 1.f);
	const KyFloat32 operand2 = Fsel(altitudeDiff2 - altitudeTolerance, 0.f, 1.f);
	return operand1 * operand2 > 0;
}

template <class VertexComparator>
class EdgeComparator
{
public:
	static bool CanEdgesStitch(const EdgeVertices& edge1, const EdgeVertices& edge2, KyFloat32 altitudeTolerance)
	{
		return
			VertexComparator::AreVerticesEqual(edge1.m_startVertex, edge2.m_endVertex) &&
			VertexComparator::AreVerticesEqual(edge1.m_endVertex, edge2.m_startVertex) &&
			NavFloorStitcher::AreVerticesEqualInAltitude(edge1.m_startAltitude, edge1.m_endAltitude, edge2.m_startAltitude, edge2.m_endAltitude, altitudeTolerance);
	}
};

class Comparator_InFloor
{
public:
	static bool AreVerticesEqual(const Vec2i& vertex1, const Vec2i& vertex2) { return vertex1 == vertex2; }
};

class Comparator_OnCellBoundEASTorWEST
{
public:
	static bool AreVerticesEqual(const Vec2i& vertex1, const Vec2i& vertex2) { return vertex1.y == vertex2.y; }
};

class Comparator_OnCellBoundNORTHorSOUTH
{
public:
	static bool AreVerticesEqual(const Vec2i& vertex1, const Vec2i& vertex2) { return vertex1.x == vertex2.x; }
};

}

#endif //Navigation_NavCellStitcher_H

