/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_DynamicNavMeshContext_H
#define Navigation_DynamicNavMeshContext_H

#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/querysystem/workingmemcontainers/workingmemarray.h"
#include "gwnavruntime/querysystem/workingmemcontainers/workingmembitfield.h"
#include "gwnavruntime/database/navtag.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/dynamicnavmesh/navfloorsweeplinetypes.h"
#include "gwnavruntime/dynamicnavmesh/dynamicnavmeshquerytypes.h"
#include "gwnavruntime/navmesh/dynamicnavfloor.h"
#include "gwnavruntime/dynamicnavmesh/earclippingtriangulator.h"
#include "gwnavruntime/navmesh/dynamictriangulation_int.h"

namespace Kaim
{
class TagVolume;

class DynamicNavMeshContext
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_QueryWorkingMem)
public:

	DynamicNavMeshContext()
		: m_currentNavFloorIdxInProcess(0)
		, m_polygonVertices(Memory::GetGlobalHeap())
		, m_dynamicEarTriangulation(Memory::GetGlobalHeap())
		, m_dynaFloor(Memory::GetGlobalHeap())
	{}
	~DynamicNavMeshContext() { ReleaseWorkingMemory(); }

	bool InitForQuery(WorkingMemory* workingMemory)
	{
		m_rawInputEdges.Init(workingMemory);
		m_allTagVolumes.Init(workingMemory);
		m_tagVolumeImpactingCurrentFloor.Init(workingMemory);
		m_tagVolumeToNavTagIndex.Init(workingMemory);

		if (m_rawInputEdges.IsInitialized() == false ||
		    m_allTagVolumes.IsInitialized() == false ||
		    m_tagVolumeImpactingCurrentFloor.IsInitialized() == false ||
		    m_tagVolumeToNavTagIndex.IsInitialized() == false)
			return false;

		m_currentNavFloorIdxInProcess = 0;
		return true;
	}

	bool InitForNewFloorInQuery(WorkingMemory* /*workingMemory*/)
	{
		m_tagVolumeImpactingCurrentFloor.MakeEmpty();
		m_tagVolumeToNavTagIndex.MakeEmpty();
		m_rawInputEdges.MakeEmpty();

		m_navTags.Clear();
		m_navTagIdxOfConnex.Clear();
		m_intersectedInputEdges.Clear();
		m_extractedPolygons.Clear();
		m_mergedPolygons.Clear();
		m_sweepResult.Clear();
		m_triangulationOutputs.Clear();
		m_dynaFloor.Clear();

		return true;
	}

	void ReleaseWorkingMemory()
	{
		m_allTagVolumes.ReleaseWorkingMemoryBuffer();
		m_tagVolumeImpactingCurrentFloor.ReleaseWorkingMemoryBuffer();
		m_tagVolumeToNavTagIndex.ReleaseWorkingMemoryBuffer();
		m_rawInputEdges.ReleaseWorkingMemoryBuffer();
		m_edgesToTreat.ReleaseWorkingMemoryBuffer();
		m_rightMostPoints.ReleaseWorkingMemoryBuffer();
		m_extractedPolygonWinding.ReleaseWorkingMemoryBuffer();
		m_holeTable.ReleaseWorkingMemoryBuffer();
		m_treatedEdges.ReleaseWorkingMemoryBuffer();
		m_successorCandidates.ReleaseWorkingMemoryBuffer();
	}

public:
	NavFloorIdx m_currentNavFloorIdxInProcess;
	PixelPos m_cellOriginPixel; ///< The pixel position of the lower left corner of the navCell that contains the processed navfloor. 

	// Extract tag volume contour
	KyUInt16 m_currentTagVolumeIdx;
	KyUInt16 m_tagVolumeContourCount;

	// Polygon Building
	KyUInt16 m_currentNavTagIdx;
	KyUInt16 m_currentEdgeStart;
	const SweepLineOutputEdgePiece* m_currentPreviousEdge;
	bool m_polygonBuildAlreadyInProcess;

	// Polygon Triangulation
	KyUInt16 m_currentPolygonIdx;
	KyUInt16 m_innerPolygonIdx;
	KyUInt16 m_triangulatorOutputIdx;
	KyUInt16 m_currentVertexCountInPolygon;
	KyUInt16 m_firstValidVertexIdxInPolygon;

	WorkingMemArray<TagVolume*> m_allTagVolumes;

	WorkingMemArray<TagVolume*> m_tagVolumeImpactingCurrentFloor;
	WorkingMemArray<KyUInt32> m_tagVolumeToNavTagIndex;
	WorkingMemArray<InputEdge> m_rawInputEdges;

	WorkingMemArray<const SweepLineOutputEdgePiece*> m_edgesToTreat;

	KyArray<InputEdgePiece> m_intersectedInputEdges;

	///< Aggregates navtags found in the static floors and adds them to the TagVolumes projected in this floor.
	/// first n elements (n == navfloor.m_connexNavTag.GetCount()) are the navTag from the static navfloor. 
	/// The remainer come from the TagVolumes; their index can be found in m_tagVolumesNavTagIndexes.
	KyArray<DynamicNavTag> m_navTags;
	KyArrayPOD<KyUInt32> m_navTagIdxOfConnex;
	SweepLineOutput m_sweepResult;
	KyArray<KyArray<MergedPolygonWithHoles> > m_mergedPolygons;
	KyArray<TriangulatorOutput> m_triangulationOutputs;

	KyArrayDH<EarClippingTriangulator::PolygonVertex> m_polygonVertices; // the "linked" polygon structure
	EarClippingTriangulatorInputPolygonWithHoles m_earPolygon;
	DynamicTriangulation m_dynamicEarTriangulation;
	DynamicNavFloor m_dynaFloor;


	// Polygon builder
	KyArray<MergedPolygon> m_extractedPolygons;

	WorkingMemArray<Vec2i> m_rightMostPoints; //GetCount() == m_extractedPolygons.GetCount()
	WorkingMemArray<PolygonWinding> m_extractedPolygonWinding; //GetCount() == m_extractedPolygons.GetCount()
	WorkingMemArray<KyUInt32> m_holeTable; //GetCount() == m_extractedPolygons.GetCount()
	WorkingMemBitField m_treatedEdges;
	WorkingMemArray<KyUInt32> m_successorCandidates;
};




}


#endif //Navigation_DynamicNavMeshContext_H

