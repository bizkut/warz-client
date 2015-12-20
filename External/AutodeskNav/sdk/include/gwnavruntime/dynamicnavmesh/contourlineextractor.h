/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef KAIM_CONTOUR_LINE_EXTRACTOR_H
#define KAIM_CONTOUR_LINE_EXTRACTOR_H

// primary contact: LASI - secondary contact: NONE

#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/dynamicnavmesh/dynamicnavmeshquerytypes.h"

namespace Kaim
{

class WorkingMemory;
class NavFloor;
class Database;
class NavFloorBlob;
class NavHalfEdge;
template <class T>
class WorkingMemArray;
class WorkingMemBitField;


enum ContourLineEdgeType
{
	CONTOUR_LINE_TRIANGLE_DIAGONAL, 
	CONTOUR_LINE_FLOOR_OBSTACLE,
	CONTOUR_LINE_FLOOR_LINK,
	CONTOUR_LINE_TAGVOLUME,
};


class ContourLineEdge
{
public: 
	ContourLineEdge(): m_altStart(KyFloat32MAXVAL), m_altEnd(KyFloat32MAXVAL), m_navHalfEdgeIdx(NavHalfEdgeIdx_Invalid),
		m_type(CONTOUR_LINE_TRIANGLE_DIAGONAL), m_swap(false) {}

	ContourLineEdge(const Vec2i& start, const Vec2i& end, KyFloat32 altStart, KyFloat32 altEnd, NavHalfEdgeIdx idx, ContourLineEdgeType type = CONTOUR_LINE_TRIANGLE_DIAGONAL) :
	  m_edgePiece(start, end), m_altStart(altStart), m_altEnd(altEnd), m_navHalfEdgeIdx(idx),
		  m_type(type), m_swap(false) {}
public:
	EdgePoints m_edgePiece;
	KyFloat32 m_altStart;
	KyFloat32 m_altEnd;
	NavHalfEdgeIdx m_navHalfEdgeIdx;
	ContourLineEdgeType m_type;
	bool m_swap;
};


/// This class takes a NavFloor and an altitude range as input and will return an array of edges 
/// that form the contour lines of the slice between altMin and altMax. 
/// The edges thus extracted are NOT ordered and follow the cell/floor links as well as the obstacle edges
class ContourLineExtractor
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(ContourLineExtractor)
private: 
	enum AltVertexStatus
	{
		VERTEX_BELOW_RANGE, 
		VERTEX_WITHIN_RANGE, 
		VERTEX_ABOVE_RANGE
	};

public:
	ContourLineExtractor(Database* db, NavFloor* floor, const WorkingMemArray<Vec3f>& staticFloatVertices,
		const WorkingMemBitField& triangleVsTagVolumeBoxIntersection, const PixelBox& tvPixelBox, KyFloat32 altMin, KyFloat32 altMax);

	// Main function 
	KyResult ExtractContourLineInFloor(WorkingMemory* workingMemory, KyArray<ContourLineEdge>& result);
	static bool IsPointInMiddleOfEdge(const Vec2i& p, const Vec2i& a, const Vec2i& b);
	static bool IsPointInMiddleOfEdge_NotAlgined(const Vec2i& p, const Vec2i& a, const Vec2i& b);
	static bool IsPointStrictlyInMiddleOfEdge_NotAlgined(const Vec2i& p, const Vec2i& a, const Vec2i& b);

private: 
	KyResult ComputeVertexStatus(KyArrayPOD<KyUInt8>& vertexAltStatus);
	KyResult ComputeIntersectionOfEdgeWithLevelLines(const KyArrayPOD<KyUInt8>& vertexAltStatus,
		WorkingMemArray<Vec2i>& halfEdgeIntersectionWithAltMin, WorkingMemArray<Vec2i>& halfEdgeIntersectionWithAltMax, WorkingMemBitField& egdeIntersect);

	void ComputeEdgePieceForBorderEdge(const NavFloorBlob* floorBlob, NavHalfEdgeIdx halfEdgeIdx, KyFloat32 starAlt, KyFloat32 endAlt, const Vec2i& v0, const Vec2i& v1);
	void ComputeLevelLineForEdge(const NavFloorBlob* floorBlob, NavHalfEdgeIdx halfEdgeIdx, NavHalfEdgeIdx nextHalfEdgeIdx, KyFloat32 starAlt, KyFloat32 endAlt);
	void AddTriangleIsoAltitudeDiagonal(const Vec2i& start, const Vec2i& end, KyFloat32 alt, NavHalfEdgeIdx edgeIdx);
	void AddEdgePiece(const NavFloorBlob* floor, const Vec2i& start, const Vec2i& end, KyFloat32 altStart, KyFloat32 altEnd, NavHalfEdgeIdx edgeIdx);

	void GetOrComputeIntersections(NavHalfEdgeIdx halfEdgeIdx, NavHalfEdgeIdx pairHalfEdgeIdx, AltVertexStatus startVertexAltStatus,
		AltVertexStatus endVertexAltStatus, NavVertexIdx startVertexIdx, NavVertexIdx endtVertexIdx);

	void IntegerIntersectionWithZPlane(const Vec3f& a, const Vec3f& b, KyFloat32 altOfPlane, Vec2i& pos);

	void BuildContourEdgesFromIntersections(const KyArrayPOD<KyUInt8>& vertexAltStatus);
private: 
	Database* m_database;
	NavFloor* m_navFloor;
	const WorkingMemBitField& m_triangleVsTagVolumeBoxIntersection;
	const WorkingMemArray<Vec3f>& m_staticFloatVertices;
	PixelBox m_enlargeTagVolumePixelBox;
	KyFloat32 m_altMin;
	KyFloat32 m_altMax;

	KyArray<ContourLineEdge>* m_contourLineEdges;

	WorkingMemArray<Vec2i>* m_halfEdgeIntersectionWithAltMin;
	WorkingMemArray<Vec2i>* m_halfEdgeIntersectionWithAltMax;
	WorkingMemBitField* m_halfEdgeIntersect;
};

}

#endif //KAIM_CONTOUR_LINE_EXTRACTOR_H
