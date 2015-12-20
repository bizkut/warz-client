/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_DatabaseUpdateManager_H
#define Navigation_DatabaseUpdateManager_H

#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/blob/blobhandler.h"
#include "gwnavruntime/visualsystem/displaylist.h"
#include "gwnavruntime/math/box2f.h"
#include "gwnavruntime/containers/collection.h"
#include "gwnavruntime/querysystem/iquery.h"

namespace Kaim
{

class World;
class Database;
class TagVolume;

class DynamicNavMeshQuery;
class MakeNavFloorStitchQuery;
class QueryCounterOnDone;
typedef Collection<Ptr<TagVolume>, MemStat_NavData> TagVolumeCollection;
class NavCell;

enum DatabaseUpdateStatus
{
	DatabaseUpdateManagerIdle                                 = 0,

	UpdatingNavDataInDatabases_ProcessingNavData              = 1,
	UpdatingNavDataInDatabases_WaitingDynamicNavMeshQueries   = 2,
	UpdatingNavDataInDatabases_PerformingStitchOneToOne       = 3,
	UpdatingNavDataInDatabases_WaitingRunTimeStitchQueries    = 4,
	UpdatingNavDataInDatabases_FinalizeNavDataUpdate          = 5,

	UpdatingTagVolumeIntegration_ProcessingTagVolumes         = 6,
	UpdatingTagVolumeIntegration_WaitingDynamicNavMeshQueries = 7,
	UpdatingTagVolumeIntegration_ProcessingDynNavMeshResults  = 8,
	UpdatingTagVolumeIntegration_WaitingRunTimeStitchQueries  = 9,
	UpdatingTagVolumeIntegration_FinalizeTagVolumeUpdate      = 10,

	CancellingTagVolumeIntegration                            = 11
};

class DatabaseUpdateManager
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_DynamicNavMesh)

public:
	explicit DatabaseUpdateManager(World* world);
	~DatabaseUpdateManager();

	void Update();

	// if you specify a databaseIdx, only this database will be updated
	void FullNavDataAddRemoveUpdate(KyUInt32 databaseIdx = KyUInt32MAXVAL);
	void FullTagVolumeIntegrationUpdate();

	// Warning ! cancels all the asynchronous query and launch them synchronously
	void ForceUpdateToCompleteSynchronously();

	// call on World::RemoveAllTagvolumes
	void CancelTagVolumeIntegrationUpdate();

	void ToggleDebugRender(bool toggle);

	bool IsIdle() const;
	bool IsUpdatingNavData() const;
	bool IsUpdatingTagVolumeIntegration() const;
public: 
	void UpdateFrameIdxInAllDatabases();
	void PerformNextUpdateStepAccordingToStatus(QueryProcessMode processMode);

	bool IsThereNavDataToAddedOrRemovedInOneDatabase();
	void StartNewNavDataUpdateInDatabase(QueryProcessMode processMode);
	void PerformStitchOneToOne(QueryProcessMode processMode);
	void FinalizeNavDataAddRemoveUpdate();

	bool IsThereNewTagVolumesToIntegrateOrDeIntegrate();
	void StartNewTagVolumeIntegrationUpdate(QueryProcessMode processMode);
	void CreateDynamicNavMeshQueriesForDatabase(KyUInt32 databaseIdx, QueryProcessMode queryProcessMode);
	void ProcessDynamicNavMeshQueryResults(QueryProcessMode queryProcessMode);
	void FinalizeTagVolumeIntegrationUpdate();

	void Clear();

	void CreateMakeNavFloorStitchQuery(Database* database, NavCell* navCell,  QueryProcessMode queryProcessMode);
	void CreateDynamicNavMeshQuery(Database* database, NavCell* navCell,
		const TagVolumeCollection* integratedTagVolumesAtCellPos, const TagVolumeCollection* newTagVolumes, QueryProcessMode queryProcessMode);

	KyUInt32 GetNumberOfWaitingDynamicNavMeshQueries() const { return m_dynamicNavmeshQueriesInProcess; }
	KyUInt32 GetNumberOfWaitingRunTimeStitchQueries()  const { return m_stitchQueriesInProcess;         }

	void RunSynchronouslyAllNonFinishedQueries();

	// Call by the database when NavLMesh are added in case of the Database was Empty.
	bool IsThereWaitingTagVolumesInDatabase(KyUInt32 databaseIdx);
	void EnlargeCellBoxAccordingToWaitingTagVolumes(KyUInt32 databaseIdx, CellBox& currentNavMeshUpdateCellBox);
	void SpatializeWaitingTagVolumesInNavCellGrid(KyUInt32 databaseIdx);
private:
	void RenderDisplayLists();
	void ClearDebugRender();

	void DebugTagVolumeInUpdateDisplayList();

	void SnapshotOfTagVolumesToIntegrateAndToRemove();
	void ComputeCellBoxOfTagvolumeToIntegratePerDatabase();
	void MarkAllCellsConcernedByTagVolumeIntegrationUpdate(TagVolume* tagVolume);
	void MarkAllCellsConcernedByUpdateInDatabase(Database* database, const CellBox& tagVolumeCellBox);

	void PushNewIntegratedTagVolumeInCellBox(TagVolume* tagVolume, Database* database, const CellBox& cellBoxInDatabase);
	void RemoveDeIntegratedTagVolumeFromCellBox(TagVolume* tagVolume, Database* database, const CellBox& cellBoxInDatabase);

	void PostProcessIntegratedTagVolumes();
	void PostProcessDeIntegratedTagVolumes();

	void CancelAllQueries();

	void PrintErrorMessageForFaillingDynamicNavMeshQueries();
public:
	World* m_world;

	DatabaseUpdateStatus m_status;

	KyArray<CellBox, MemStat_DynamicNavMesh> m_cellBoxOfUpdateInDatabase;                                               // indexed on databaseIdx
	KyArray<CellBox, MemStat_DynamicNavMesh> m_cellBoxOfTagvolumeToIntegrate;                                           // indexed on databaseIdx

	KyArray<Collection<Ptr<TagVolume>, MemStat_DynamicNavMesh>, MemStat_DynamicNavMesh> m_waitingTagVolumesPerDatabase; // indexed on databaseIdx

	KyArray<KyArray<Ptr<DynamicNavMeshQuery>, MemStat_DynamicNavMesh>, MemStat_DynamicNavMesh> m_dynNavMeshQueriesPerDatabase; // indexed on databaseIdx
	KyArray<KyArray<Ptr<MakeNavFloorStitchQuery>, MemStat_DynamicNavMesh>, MemStat_DynamicNavMesh> m_stitchQueriesPerDatabase; // indexed on databaseIdx

	AtomicInt<int> m_dynamicNavmeshQueriesInProcess;
	AtomicInt<int> m_stitchQueriesInProcess;

	Ptr<QueryCounterOnDone> m_OnDoneForDynamicNavMeshQueries;
	Ptr<QueryCounterOnDone> m_OnDoneForRunTimeStitchQueries;

	TagVolumeCollection m_tagVolumesInDeIntegration;
	TagVolumeCollection m_tagVolumesInIntegration;

	// for visualDebugManager (debug)
	bool m_debugDisplayListsEnabled;
	KyArray<ScopedDisplayList*> m_debugDisplayLists;
};

KY_INLINE bool DatabaseUpdateManager::IsIdle() const { return m_status == DatabaseUpdateManagerIdle; }
KY_INLINE bool DatabaseUpdateManager::IsUpdatingNavData() const
{
	return m_status >= UpdatingNavDataInDatabases_ProcessingNavData && m_status <= UpdatingNavDataInDatabases_FinalizeNavDataUpdate;
}

KY_INLINE bool DatabaseUpdateManager::IsUpdatingTagVolumeIntegration() const
{
	return m_status >= UpdatingTagVolumeIntegration_ProcessingTagVolumes && m_status <= UpdatingTagVolumeIntegration_FinalizeTagVolumeUpdate;
}

KY_INLINE bool DatabaseUpdateManager::IsThereWaitingTagVolumesInDatabase(KyUInt32 databaseIdx)
{
	return m_waitingTagVolumesPerDatabase[databaseIdx].GetCount() != 0;
}

} // namespace Kaim

#endif //Navigation_DatabaseUpdateManager_H
