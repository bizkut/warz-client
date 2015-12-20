/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: LASI - secondary contact: NOBODY
#ifndef Navigation_TagVolumeQuery_H
#define Navigation_TagVolumeQuery_H

#include "gwnavruntime/querysystem/iquery.h"
#include "gwnavruntime/queries/utils/queryutils.h"
#include "gwnavruntime/world/tagvolume.h"
#include "gwnavruntime/navmesh/blobs/navhalfedge.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/navmesh/blobs/navfloorblob.h"
#include "gwnavruntime/blob/blobhandler.h"
#include "gwnavruntime/dynamicnavmesh/dynamicnavmeshquerytypes.h"
#include "gwnavruntime/dynamicnavmesh/dynamicnavmeshquerystats.h"
#include "gwnavruntime/querysystem/workingmemcontainers/workingmemarray.h"


namespace Kaim
{
class TriangulatorOutput;
class ScopedDisplayList;
class TagVolume;
class SweepLineOutput;
class TriangulatorOutput;
class DynamicNavFloor;

typedef Collection<Ptr<TagVolume>, MemStat_NavData> TagVolumeCollection;

enum MergerDebugDisplayListType
{
	MERGER_DEBUG_DISPLAYLIST_INPUT_TAGVOLUMES,
	MERGER_DEBUG_DISPLAYLIST_ROUND_SNAPPING,
	MERGER_DEBUG_DISPLAYLIST_EDGE_CULLING,
	MERGER_DEBUG_DISPLAYLIST_POLYGON_SCAN_INPUT,
	MERGER_DEBUG_DISPLAYLIST_POLYGON_SCAN_OUTPUT, 
	MERGER_DEBUG_DISPLAYLIST_TRIANGULATION,
	MERGER_DEBUG_DISPLAYLIST_COUNT
};

/// Enumerates the possible results of a DynamicNavMeshQuery.
enum DynamicNavMeshQueryResult
{
	DYNNAVMESH_NOT_INITIALIZED = QUERY_NOT_INITIALIZED, ///< Indicates the query has not yet been initialized. 
	DYNNAVMESH_NOT_PROCESSED   = QUERY_NOT_PROCESSED,   ///< Indicates the query has not yet been launched. 

	DYNNAVMESH_COLLECT_TAGVOLUME_FOR_NAVFLOOR,   // 2
	DYNNAVMESH_EXTRACTING_ALL_NAVTAGS,           // 3
	DYNNAVMESH_EXTRACTING_TAGVOLUMES_CONTOURS,   // 4
	DYNNAVMESH_EXTRACTING_NAVFLOOR_CONTOUR,      // 5
	DYNNAVMESH_ROUND_SNAPPING,                   // 6
	DYNNAVMESH_SWEEP_LINE_SORT_INPUT,            // 7
	DYNNAVMESH_SWEEP_LINE_SCAN,                  // 8
	DYNNAVMESH_SWEEP_LINE_SORT_OUTPUT,           // 9
	DYNNAVMESH_BUILD_POLYGONS_INIT,              // 10
	DYNNAVMESH_BUILD_POLYGONS_PROCESS,           // 11
	DYNNAVMESH_TRIANGULATE_POLYGONS_INIT,        // 12
	DYNNAVMESH_TRIANGULATE_POLYGONS_PROCESS,     // 13
	DYNNAVMESH_BUILD_DYNAMIC_NAVFLOOR,           // 14
	DYNNAVMESH_BUILD_FIX_VERTEX_ALTITUDES,       // 15
	DYNNAVMESH_BUILD_NAVFLOORBLOB,               // 16

	DYNNAVMESH_NAVFLOOR_INVALID,                        ///< Indicates that the navfloor currently being processed is no longer valid. 
	DYNNAVMESH_LACK_OF_WORKING_MEMORY,                  ///< Indicates that insufficient working memory caused the query to stop.
	DYNNAVMESH_INTERSECTION_COMPUTATION_ERROR,          ///< Indicates an error occurred while computing the intersections. 
	DYNNAVMESH_SWEEPLINE_COMPUTATION_ERROR,             ///< Indicates an error occurred while culling edges. 
	DYNNAVMESH_POLYGON_COMPUTATION_ERROR,               ///< Indicates an error occurred while building polygons from culled edges. 
	DYNNAVMESH_TRIANGULATION_COMPUTATION_ERROR,         ///< Indicates an error occurred while triangulating the newly obtained polygons. 
	DYNNAVMESH_ALTITUDE_COMPUTATION_ERROR,              ///< Indicates an error occurred while computing the altitude of the dynamic navmesh. 
	DYNNAVMESH_NAVFLOOR_INDICES_OUT_OF_BOUNDS,          ///< Indicates that 
	DYNNAVMESH_UNKNOWN_ERROR,                           ///< Indicates an unknown error occurred during the query processing. 

	DYNNAVMESH_DONE_SUCCESS,                            ///< Indicates the query was successfully processed.
};

/// The DynamicNavMeshQuery is used to recompute a NavCell impacted by newly projected TagVolumes or removed tagVolumes
/// This query is time-sliced
class DynamicNavMeshQuery : public ITimeSlicedQuery
{

public:
	static  QueryType GetStaticType() { return TypeDynamicNavMesh; }
	virtual QueryType GetType() const { return TypeDynamicNavMesh; }

	DynamicNavMeshQuery();
	virtual ~DynamicNavMeshQuery();

	/// Binds the query with the specified Database, clears all the inputs and outputs and sets all
	/// other parameters to their default value (they may be changed by calling the corresponding write accessors.)
	/// \param database    A pointer to the Database on which the query must be performed.
	void BindToDatabase(Database* database);
	void Initialize(NavCell* navCell, const TagVolumeCollection* integratedTagVolumesAtCellPos, const TagVolumeCollection* newTagVolumes);

	virtual void Advance(WorkingMemory* workingMemory);
	virtual void ReleaseWorkingMemoryOnCancelDuringProcess(WorkingMemory* workingMemory = KY_NULL);
	/// Read accessor for #m_result. 
	DynamicNavMeshQueryResult GetResult() const { return m_result; };

	// ---------------------------------- Accessors ----------------------------------
	const PixelPos& GetCellOriginPixel() const { return m_cellOriginPixel; }
	 
	//debug and profile
	void SetDebugDisplayLists(KyArray<ScopedDisplayList*> displayLists) { m_debugDisplayLists = displayLists; }
	const DynamicNavMeshQueryStats& GetStats() const { return m_stats; }

	void SetResult(DynamicNavMeshQueryResult result) { m_result = result; };

private:
	// Main function
	// does the following: 
	// 0. ExtractInputDataFromNavFloor
	// 1. BreakEdgeIntersections
	// 2. TagAndCullEdges
	// 3. TriangulatePolygonWithHoles
	// 4. ComputeVerticesAltitudesFromNavFloor
	// 5. BuildNavFloorFromTriangulatorOutput

	// And here are the signatures
	
	// Core functions
	KyResult TagAndCullEdges(WorkingMemory* workingMemory);
	KyResult SortSweepLineOutput(WorkingMemory* workingMemory);

	enum InitBuildPolygonResult { InitBuildPolygonResult_ReadyToBuild, InitBuildPolygonResult_NothingToDo } ;
	KyResult InitBuildPolygon(WorkingMemory* workingMemory, InitBuildPolygonResult& initBuildPolygonResult);
	enum BuildPolygonResult { BuildPolygonsResult_Finish, BuildPolygonsResult_InProcess} ;
	KyResult BuildPolygon(WorkingMemory* workingMemory, BuildPolygonResult& buildPolygonsResult);

	KyResult InitPolygonTriangulation(WorkingMemory* workingMemory);

	KyResult PreparePolygonWithHolesTriangulation(WorkingMemory* workingMemory);

	enum TriangulatePoygonResult { TriangulatePoygonResult_Finish, TriangulatePoygonResult_GoOn } ;
	KyResult TriangulatePoygon(WorkingMemory* workingMemory, TriangulatePoygonResult& triangulatePoygonResult);

	KyResult ComputeVerticesAltitudes(WorkingMemory* workingMemory);
	KyResult FillDynamicNavFloorFromTriangulatorOutputs(WorkingMemory* workingMemory, KyArrayPOD<const SweepLineOutputEdgePiece*>& edgesForEdges);
	KyResult FixHalfEdgeTypeInDynamicNavFloor(WorkingMemory* workingMemory, const KyArrayPOD<const SweepLineOutputEdgePiece*>& edgesForEdges);

	// Output build
	KyResult BuildNavFloorFromTriangulatorOutput(WorkingMemory* workingMemory);

	KyResult BuildFinalBlobFromDynamicNavFloor(WorkingMemory* workingMemory);

	// local accessors and utilities
	enum CollectTagVolumeResult { CollectTagVolume_NothingToDo, CollectTagVolume_NeedToComputeDynamicNavFloor, CollectTagVolume_NeedToRestoreStaticNavFloor };
	KyResult CollectTagVolumesImpactingNavCell(WorkingMemArray<TagVolume*>& allTagVolumes, CollectTagVolumeResult& collectTagVolumeResult);
	KyResult CollectTagVolumesToIntegrateImpactingNavCell(WorkingMemArray<TagVolume*>& allTagVolumes, const CellPos& cellPos, KyUInt32& newTagVolumeCount);
	KyResult CollectTagVolumesToRemoveImpactingNavCell(WorkingMemArray<TagVolume*>& allTagVolumes, KyUInt32& tagVolumeToRemoveCount);

	void SetResultAndFinish(DynamicNavMeshQueryResult result, WorkingMemory* workingMemory)
	{
		SetResult(result);
		SetFinish(workingMemory);
	}
	void SetFinish(WorkingMemory* workingMemory);
public: // Internal
	ScopedDisplayList* GetDisplayList(KyUInt32 index) const;
public:
	/// Input Data
	NavCell* m_navCell;
	PixelPos m_cellOriginPixel;
	const TagVolumeCollection* m_integratedTagVolumesAtCellPos;
	const TagVolumeCollection* m_newTagVolumes;

	/// Output NavFloors with TagVolumes integrated
	/// remains to size 0 if nothing changed in this 
	KyArray<Ptr<BlobHandler<NavFloorBlob> > > m_resultNavfloorHandlers;
private:
	DynamicNavMeshQueryResult m_result; ///< Updated during processing to indicate the result of the query. 

	//debug and profile
	KyArray<ScopedDisplayList*> m_debugDisplayLists;
	DynamicNavMeshQueryStats m_stats;
};

KY_INLINE ScopedDisplayList* DynamicNavMeshQuery::GetDisplayList(KyUInt32 index) const
{
#ifndef KY_BUILD_SHIPPING
	return m_debugDisplayLists.GetCount() > index  ? m_debugDisplayLists[index] : KY_NULL; 
#else
	KY_UNUSED(index);
	return KY_NULL;
#endif
}
}





#endif //Navigation_RayCanGoQuery_H

