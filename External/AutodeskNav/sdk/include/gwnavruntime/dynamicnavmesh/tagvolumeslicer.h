/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef KAIM_TAG_VOLUME_SLICER_H
#define KAIM_TAG_VOLUME_SLICER_H

// primary contact: LASI - secondary contact: NONE

#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/math/vec2i.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/dynamicnavmesh/contourlineextractor.h"
#include "gwnavruntime/querysystem/workingmemcontainers/workingmemarray.h"
#include "../containers/bitfield.h"

namespace Kaim
{

class WorkingMemory;
class NavFloor;
class Database;
class NavFloorBlob;
class TagVolume;
class ScopedDisplayList;

enum TagVolumeSlicerResult
{
	TAG_VOLUME_SLICE_RESULT_SKIPPED,
	TAG_VOLUME_SLICE_RESULT_SUCCESS,
	TAG_VOLUME_SLICE_RESULT_FAILURE
};


class ContourLineEdgePiece
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public: 
	ContourLineEdgePiece() : m_parentEdge(KY_NULL), m_swap(false), m_canceled(false), m_inPolygon(false), m_currentPolygonIdx(0) {}
public:
	EdgePoints m_edgePiece;
	const ContourLineEdge* m_parentEdge;
	bool m_swap;
	bool m_canceled;
	bool m_inPolygon;
	KyUInt16 m_currentPolygonIdx;
};

class TypedIndexedPos
{
public:
	enum PointType
	{
		TagVolumeEdgeExtremity            = /*1 <<*/ 0,
		LevelLineEdgeExtremity            = /*1 <<*/ 1,
		DiagonalEdgeExtremity             = /*1 <<*/ 2,
		IntersectionWithLevelLineDiagonal = /*1 <<*/ 3,
		IntersectionWithFloorLink         = /*1 <<*/ 4,
		IntersectionWithBorder            = /*1 <<*/ 5,

		InvalidPointType
	};

	TypedIndexedPos() : m_index(KyUInt32MAXVAL), m_pointType(InvalidPointType) {}
	TypedIndexedPos(const Vec2i& pos, KyUInt32 index, PointType pointType) :
		m_pos(pos), m_index(index), m_pointType(pointType) {}

public:
	Vec2i m_pos;
	KyUInt32 m_index;
	PointType m_pointType;
};

class TypedIndexedPosSorter
{
public:
	bool operator() (const TypedIndexedPos& hotPoint1, const TypedIndexedPos& hotPoint2) const
	{
		return hotPoint1.m_pos < hotPoint2.m_pos;
	}
};

class UniqueHotPoint
{
public:
	UniqueHotPoint() {}
	UniqueHotPoint(const Vec2i& pos) : m_pos(pos) {}
public:
	Vec2i m_pos;
};

class TagVolumeSlicer
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public: 
	TagVolumeSlicer(Database* db, NavFloor* navFloor, WorkingMemory* workingMemory) : 
	m_database(db), m_navFloor(navFloor), m_workingMemory(workingMemory), m_slicedTagVolumes(KY_NULL) {}

public:
	TagVolumeSlicerResult SliceTagVolumeAccordingToAltitudes(const TagVolume* tagVolume, KyArray<KyArrayPOD<Vec2i> >& slicedTagVolumes, ScopedDisplayList* displayList);

private:
	enum InternalResult
	{
		Stop_Failure,
		Stop_SkipTagVolume,
		Stop_KeepTagVolumeContour,
		KeppGoingOn
	};
	void Clear();

	KyResult ComputeTagVolumeContour(const TagVolume& tagVolume);

	KyResult ComputeNavFloorData(WorkingMemArray<PixelBox>& trianglePixelBoxArray, WorkingMemArray<Vec3f>& staticFloatVertices,
		WorkingMemBitField& triangleVsTagVolumeBoxIntersection);

	KyResult ExtractLevelLines(const WorkingMemBitField& triangleVsTagVolumeBoxIntersection, const WorkingMemArray<Vec3f>& staticFloatVertices);

	void ComputeTriangleIndexAndVertexStatusOfTagVolumeVertices(const WorkingMemArray<PixelBox>& trianglePixelBoxArray, const WorkingMemArray<Vec3f>& staticFloatVertices);

	InternalResult IntersectContourLineWithTagVolume();

	KyResult BuildPolygons(KyArray<KyArrayPOD<Vec2i> >& result);
	KyResult BuildPolygonFromTagVolume(KyArray<KyArray<Vec2i> >& result);

	KyResult MarkAsTreatedPairedLevelLineEdges(BitField& levelLinesIsPaired, KyUInt32& firstNonTVEdge);
	KyFloat32 GetAltitudeOfPointInTriangle(const Vec2i& tVpixelPos, const WorkingMemArray<Vec3f>& staticFloatVertices, NavTriangleIdx triangleIdx);
	bool ProjectTagVolumePixelPosInStaticFloor(const WorkingMemArray<PixelBox>& trianglePixelBoxArray, const Vec2i& tVpixelPos,
		NavTriangleIdx& triangleIdx);

	void AddTagVolumeEdgesToContourLines();
	KyResult ComputeAllContourEdgeBBox(WorkingMemArray<Box2i>& edgeBoundingBoxes);
	void AddAllEdgeExtremitiesToHotPoints(WorkingMemArray<TypedIndexedPos>& hotPoints);
	KyResult ComputeIntersections(const WorkingMemArray<Box2i>& edgeBoundingBoxes, KyArray<KyArrayPOD<KyUInt32> >& cuts,
		WorkingMemArray<TypedIndexedPos>& hotPoints);

	TypedIndexedPos::PointType GetIntersectionPointType(ContourLineEdgeType nonTvEdgeType);

	KyResult RemoveDuplicatedHotPoints(WorkingMemArray<TypedIndexedPos>& hotPoints, WorkingMemArray<UniqueHotPoint>& uniqueHotPoints, KyArray<KyArrayPOD<KyUInt32> >& cuts);
	void BreakEdgesOnHotPoints(const WorkingMemArray<Box2i>& edgeBoundingBoxes, WorkingMemArray<UniqueHotPoint>& uniqueHotPoints, KyArray<KyArrayPOD<KyUInt32> >& cuts);
	void SortCuts(const WorkingMemArray<UniqueHotPoint>& uniqueHotPoints, KyArray<KyArrayPOD<KyUInt32> >& cuts);
	void BuildFinalTagVolumeBrokenContour(const WorkingMemArray<UniqueHotPoint>& uniqueHotPoints, KyArray<KyArrayPOD<KyUInt32> >& cuts);
	void BuildContourEdgePiece(const WorkingMemArray<UniqueHotPoint>& uniqueHotPoints, KyArray<KyArrayPOD<KyUInt32> >& cuts);

	KyResult BuildPolygonsStartingFromTagVolumeEdges(KyUInt32 firstNonTVEdge, KyArray<KyArrayPOD<Vec2i> >& result, BitField& treatedEdges);
	KyResult BuildOtherPolygons(KyUInt32 firstNonTVEdge, KyArray<KyArrayPOD<Vec2i> >& result, BitField& treatedEdges);

	void RenderLevelLines(ScopedDisplayList* displayList);
	void RenderBrokenEdges(ScopedDisplayList* displayList);
	void RenderSlicedTagVolumes(const KyArray<KyArrayPOD<Vec2i> >& result, ScopedDisplayList* displayList);

	void PrintTagVolumeForDebug(const TagVolume* tagVolume);
private: 
	Database* m_database;
	NavFloor* m_navFloor;
	WorkingMemory* m_workingMemory;

	KyFloat32 m_altMin;
	KyFloat32 m_altMax;
	PixelBox m_tagVolumePixelBox;
	PixelBox m_tagVolumePixelBoxEnlargeOf1;
	// internal
	KyArrayPOD<Vec2i> m_tagVolumePixelPos;
	enum TvAltStatus
	{
		TVALTSTATUS_INSIDE,
		TVALTSTATUS_OUTSIDE,
		TVALTSTATUS_UNSET
	};

	KyArrayPOD<TvAltStatus> m_verticesAltStatus;

	KyArrayPOD<Vec2i> m_brokenTagVolumePixelPos;
	KyArray<ContourLineEdge> m_contourLineEdges;
	KyArray<ContourLineEdgePiece> m_contourLineEdgePiece;
	KyArray<KyArrayPOD<Vec2i> >* m_slicedTagVolumes;
	KyUInt16 m_polygonCount;
};

}

#endif //KAIM_CONTOUR_LINE_EXTRACTOR_H
