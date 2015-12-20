/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY

#ifndef Navigation_WorldStatistics_H
#define Navigation_WorldStatistics_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/math/fastmath.h"
#include "gwnavruntime/world/floatstat.h"

namespace Kaim
{

class World;
class VisualDebugAttributeGroup;
class VisualDebugServer;


enum WorldUpdateStep
{
	WorldUpdateStep_WorldUpdate                    = 0,
	WorldUpdateStep_WorldUpdateAllButVisualDebug      ,
	WorldUpdateStep_SendVisualDebugData               ,
	WorldUpdateStep_ProcessQueuesInWorldUpdate        ,
	WorldUpdateStep_FlushQueries                      ,
	WorldUpdateStep_UpdateDatabases                   ,
	WorldUpdateStep_UpdateDynamicNavMesh              ,
	WorldUpdateStep_UpdateSpatializations             ,
	WorldUpdateStep_UpdateBotsPathFollowing           ,
	WorldUpdateStep_QueryQueueProcess                 ,

	WorldUpdateStepIndex_Count
};

enum WorldSummaryAttributes
{
	WorldSummaryAttributes_worldUpdateStat,
	WorldSummaryAttributes_visualDebugTime,
	WorldSummaryAttributes_queriesTime,
	WorldSummaryAttributes_navdata_count,
	WorldSummaryAttributes_bot_count,
	WorldSummaryAttributes_obstacle_count,
	WorldSummaryAttributes_tagvolume_count,
	WorldSummaryAttributes_database_count,
	WorldSummaryAttributes_memory_footprint
};

enum CountersAttributes
{
	CountersAttributes_NavDataCount                ,
	CountersAttributes_NotEmptyDatabaseCount       ,
	CountersAttributes_MaxDatabasesCount           ,
	CountersAttributes_BotCount                    ,
	CountersAttributes_ObstaclesTotalCount         ,
	CountersAttributes_BoxObstaclesCount           ,
	CountersAttributes_CylinderObstaclesCount      ,
	CountersAttributes_TagVolumeTotalCount         ,
	CountersAttributes_TagVolumeToBeIntegratedCount,
	CountersAttributes_TagVolumeIntegratedCount    ,
	CountersAttributes_TagVolumeToBeRemovedCount   ,
	CountersAttributes_PointOfInterestCount        
};

enum MemoryAttributes
{
	MemoryAttributes_TotalFootprint,
	MemoryAttributes_TotalUsedSpace
};

enum SettingsAttributes
{
	SettingsAttributes_NbFramesForShortcutTrajectoryPeriodicUpdate,
};

// This class is not instantiated in the world when KY_BUILD_SHIPPING is defined
class WorldStatistics
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_WorldFwk)

public:
	WorldStatistics()
		: m_world(KY_NULL)
		, m_navdataCount(0)
		, m_botsCount(0)
		, m_boxObstaclesCount(0)
		, m_cylinderObstaclesCount(0)
		, m_pointsOfInterestCount(0)
		, m_tagVolumesToBeIntegratedCount(0)
		, m_tagVolumesIntegratedCount(0)
		, m_tagVolumesToBeRemovedCount(0)
		, m_databasesCount(0)
		, m_notEmptyDatabaseCount(0)
		, m_summaryAttributesGroup(KY_NULL)
		, m_profilingAttributesGroup(KY_NULL)
		, m_countersAttributesGroup(KY_NULL)
		, m_memoryAttributesGroup(KY_NULL)
		, m_settingsAttributesGroup(KY_NULL)
	{}

	void Initialize(World* world);

	const FloatStat& GetWorldUpdateStepStat(WorldUpdateStep worldStat) const { return m_worldUpdateStepStats[worldStat]; }

	void Update();
	void UpdateVisualDebug();
	void ResetStatValues();

	KyFloat32* GetWorldUpdateStepTimeToUpdate(WorldUpdateStep worldStat);

	void CreateSummaryAttributes();
	void UpdateSummaryAttributes();
	void CreateProfilingStatsAttributeGroup();
	void UpdateProfilingStatsAttributeGroup();
	void CreateCountersAttributeGroup();
	void UpdateCountersAttributeGroup();
	void CreateMemoryAttributeGroup();
	void UpdateMemoryAttributeGroup();
	void CreateSettingsAttributeGroup();
	void UpdateSettingsAttributeGroup();

	void Report(StringBuffer* report) const;

public:
	World* m_world;

	KyArray<FloatStat> m_worldUpdateStepStats;

	KyUInt32 m_navdataCount; // total in all databases
	KyUInt32 m_botsCount; 
	KyUInt32 m_boxObstaclesCount;
	KyUInt32 m_cylinderObstaclesCount;
	KyUInt32 m_pointsOfInterestCount;
	KyUInt32 m_tagVolumesToBeIntegratedCount;
	KyUInt32 m_tagVolumesIntegratedCount;
	KyUInt32 m_tagVolumesToBeRemovedCount;
	KyUInt32 m_databasesCount;
	KyUInt32 m_notEmptyDatabaseCount;

	VisualDebugAttributeGroup* m_summaryAttributesGroup;
	VisualDebugAttributeGroup* m_profilingAttributesGroup;
	VisualDebugAttributeGroup* m_countersAttributesGroup;
	VisualDebugAttributeGroup* m_memoryAttributesGroup;
	VisualDebugAttributeGroup* m_settingsAttributesGroup;
};

#ifndef KY_BUILD_SHIPPING
// to use only in World::Update() "sub functions"
// we may divide the number of Timer calls in WorldUpdate by 2 
#define KY_PROFILE_WORLD_UPDATE_STEP(x) \
ScopedProfilerMs scopedProfilerMs(this->m_statistics->GetWorldUpdateStepTimeToUpdate(WorldUpdateStep_##x)); \
KY_SCOPED_PERF_MARKER("World::" #x);
#else
#define KY_PROFILE_WORLD_UPDATE_STEP(x)
#endif

} // namespace Kaim

#endif

