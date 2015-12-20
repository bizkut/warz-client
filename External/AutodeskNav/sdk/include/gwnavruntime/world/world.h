/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_World_H
#define Navigation_World_H

#include "gwnavruntime/world/databasebinding.h"
#include "gwnavruntime/world/visualdebugregistry.h"
#include "gwnavruntime/world/worldstatistics.h"

#include "gwnavruntime/path/livepath.h"
#include "gwnavruntime/visualsystem/displaylist.h"
#include "gwnavruntime/visualdebug/visualdebugclientregistry.h"
#include "gwnavruntime/visualdebug/visualdebugserver.h"

#include "gwnavruntime/pathfollower/basepathprogresscomputer.h"
#include "gwnavruntime/pathfollower/ipositiononpathvalidator.h"
#include "gwnavruntime/pathfollower/avoidanceworkspace.h"
#include "gwnavruntime/querysystem/queryqueuearray.h"
#include "gwnavruntime/world/asyncquerydispatcher.h"
#include "gwnavruntime/collision/collisionworld.h"
#include "gwnavruntime/world/basenavigationprofile.h"


namespace Kaim
{

class Database;
class DatabaseBinding;
class WorldElementSpatializer;
class PointOfInterest;
class CylinderObstacle;
class BoxObstacle;
class TagVolume;
class LivePath;
class FileOpenerBase;
class BotConfig;
class ITrajectory;
class IAvoidanceComputer;
class QueryQueueArray;
class QueryQueue;
class AsyncQueryDispatcher;
class DatabaseUpdateManager;
class VisualDebugAttributesManager;

/// This class is a runtime container for Gameware Navigation WorldElements such
/// as NavData, Bots, BoxObstacles, TagVolumes...
class World : public RefCountBaseNTS<World, MemStat_WorldFwk>
{
	KY_CLASS_WITHOUT_COPY(World)

public:
	// ---------------------------------- Main API Functions ----------------------------------

	/// \param databaseCount The number of databases added to this world, must be greater than or equal to 1.
	explicit World(KyUInt32 databaseCount = 1);

	virtual ~World();

	// ---------------------------------- Update ----------------------------------

	/// Updates the World.
	/// It is possible to use Gameware Navigation as a NavMesh/NavGraph Query
	/// toolbox without calling World::Update(). But to easily support Bots,
	/// obstacles, DynamicNavMesh, etc., a World::Update() call is required
	/// every frame.
	void Update(KyFloat32 simulationTimeInSeconds = 0.016f);

	/// Retrieves the number of times Update() has been called.
	KyUInt32 GetUpdateCount() const;

	/// Returns statistics on CPU consumption during Update() and its steps.
	/// With KY_BUILD_SHIPPING, it returns KY_NULL;
	WorldStatistics* GetWorldStatistics();

	// ---------------------------------- VisualDebug ----------------------------------
	/// VisualDebugging is only enabled with KY_BUILD_DEBUG_and KY_BUILD_RELEASE
	/// With KY_BUILD_SHIPPING, functions related to VisualDebugging are replaced by empty inlined functions for convenience,
	/// However, it can still be judicious to guard your own code that uses VisualDebugging with a check against KY_BUILD_SHIPPING.

	/// Starts the visual debug server. You can start it at any time.
	KyResult StartVisualDebug(const VisualDebugServerConfig& visualDebugServerConfig);
	void StopVisualDebug();

	VisualDebugServer* GetVisualDebugServer();

	/// Returns the registry in this World of instances of WorldElement that can be
	/// visually debugged in Gameware Navigation Lab.
	VisualDebugRegistry* GetElementRegistry();
	
	/// Get the DisplayListManager that is required when creating a ScopedDisplayList
	DisplayListManager* GetDisplayListManager();

	/// Get the attributesManager that is required when creating an attribute group
	VisualDebugAttributesManager* GetAttributesManager();


	// ---------------------------------- WorldElement Getters ----------------------------------

	KyUInt32 GetDatabasesCount() const;
	Database* GetDatabase(KyUInt32 index); ///< \pre index must be valid, we don't check it there.

	KyUInt32 GetPointsOfInterestCount() const;
	PointOfInterest* GetPointOfInterest(KyUInt32 index); ///< \pre index must be valid, we don't check it there.

	KyUInt32 GetCylinderObstaclesCount() const;
	CylinderObstacle* GetCylinderObstacle(KyUInt32 index); ///< \pre index must be valid, we don't check it there.

	KyUInt32 GetBoxObstaclesCount() const;
	BoxObstacle* GetBoxObstacle(KyUInt32 index); ///< \pre index must be valid, we don't check it there.

	KyUInt32 GetTagVolumesCount() const;
	KyUInt32 GetTagVolumesToBeIntegratedCount() const;
	KyUInt32 GetTagVolumesIntegratedCount() const;
	KyUInt32 GetTagVolumesToBeRemovedCount() const;
	TagVolume* GetTagVolumesToBeIntegrated(KyUInt32 index); ///< \pre index must be valid, we don't check it there.
	TagVolume* GetTagVolumesIntegrated(KyUInt32 index); ///< \pre index must be valid, we don't check it there.
	TagVolume* GetTagVolumesToBeRemoved(KyUInt32 index); ///< \pre index must be valid, we don't check it there.

	KyUInt32 GetQueryQueueArraysCount() const;
	QueryQueueArray* GetQueryQueueArray(KyUInt32 index); ///< \pre index must be valid, we don't check it there.

	KyUInt32 GetQueryPendingOperationCount() const;

	// ---------------------------------- NavigationProfile ----------------------------------

	/// If TraverseLogic of the NavigationProfile has obsolete functions, 
	/// navigationProfile won't be added and AddNavigationProfile returns KyUInt32MAXVAL.
	KyUInt32 AddNavigationProfile(Ptr<BaseNavigationProfile> navigationProfile);
	Ptr<BaseNavigationProfile> GetNavigationProfile(KyUInt32 profileId);

	// ---------------------------------- Async Queries ----------------------------------

	/// Set a new IAsyncQueryDispatcher instance to be used by this World:
	/// - first: calls IAsyncQueryDispatcher::RemoveQueryQueuesFromWorld() on
	///   the current World's IAsyncQueryDispatcher,
	/// - then: replaces World's IAsyncQueryDispatcher,
	/// - finally: calls IAsyncQueryDispatcher::AddQueryQueuesToWorld() on the
	///   new World's IAsyncQueryDispatcher.
	/// Passing NULL does nothing.
	void SetAsyncQueryDispatcher(IAsyncQueryDispatcher* asyncQueryDispatcher);

	IAsyncQueryDispatcher* GetAsyncQueryDispatcher();

	/// Pushes an IQuery in the right QueryQueue accordingly to World's IAsyncQueryDispatcher.
	void PushAsyncQuery(IQuery* query, AsyncQueryDispatchId asyncQueryDispatchId = AsyncQueryDispatchId_Default, Bot* bot = KY_NULL);

	void CancelAsyncQuery(IQuery* query);

	void ProcessQueuesOutsideWorldUpdate();

	void SetWorldUpdateThreadId(ThreadId threadId);
	ThreadId GetWorldUpdateThreadId() const;


	// ---------------------------------- RemoveAll... ----------------------------------

	/// Remove all Bots, PointOfInterest, CylinderObstacles, BoxObstacles,
	/// TagVolumes, and NavData; and cancel all queries.
	void RemoveAndCancellAll();

	void RemoveAllBots();
	void RemoveAllPointsOfInterest();
	void RemoveAllCylinderObstacles();
	void RemoveAllBoxObstacles();
	void RemoveAllTagVolumes();
	// RemoveAllQueryQueueArrays function is not relevant.

	void ClearDatabase(KyUInt32 databaseIndex);


	// -------- Specific optimization to the ShortcutTrajectory --------
	/// Theses functions are only useful for bots with ShortcutTrajectory.
	/// It allows to skip ProgressOnPath and ShortcutTrajectory computations that are usually done each frame.
	/// Note that on some events the bot can be updated more often than the given number.
	/// For instance, if changes occur on the Path or to be sure not to miss its current target.

	/// Gives the number of frames between two updates of the PathFollowing for each bot.
	KyUInt32 GetNumberOfFramesForShortcutTrajectoryPeriodicUpdate() const;
	
	/// Set the number of frames between two updates of the PathFollowing for all bots with TrajectoryMode_Shortcut.
	/// Default value is 1 in order to get an update each frame.
	/// If 0 is passed, it is defaulted to 1.
	/// For instance, by setting a value of 5, UpdateBotPathFollowing will call the ProgressOnPathComputer and the Trajectory one frame on 5, 
	/// this will make UpdateBotPathFollowing being around 3 times faster since the number of RayCanGo used to maintained the TargetOnPathStatus is exactly divided by 5,
	/// but some other components are already skipping some frames, hence their cost is not divided by 5.
	void SetNumberOfFramesForShortcutTrajectoryPeriodicUpdate(KyUInt32 numberOfFrames);

private:
	// World::Update() steps
	void FlushCommandsInWorldUpdate();
	void ProcessQueuesInWorldUpdate();
	void FlushQueries();
	void UpdateDatabases();
	void UpdateSpatializations(KyFloat32 simulationTimeInSeconds);
	void UpdateBotsPathFollowing(KyFloat32 simulationTimeInSeconds);
	void SendVisualDebugData(KyFloat32 simulationTimeInSeconds);
	void UpdateStatistics(KyFloat32 worldUpdateMs);
	void FlushCommandsOutOfWorldUpdate();

	// World::RemoveAndCancellAll() steps
	void CancelAllQueriesAndClearCommandsInAllQueryQueueArray();
	void ClearAllDatabases();

public: // internal

	// Navigation profiles
	KyArray<Ptr<BaseNavigationProfile> > m_navigationProfiles;

	// Update counters
	KyUInt32 m_updateCount; // counter incremented on each call to Update(), used for timeslicing
	KyUInt32 m_nbFramesForShortcutTrajectoryPeriodicUpdate; // Update period used specifically with ShortcutTrajectory, cf. SetNumberOfFramesForShortcutTrajectoryPeriodicUpdate

	// databases
	KyUInt32 m_databaseCount;
	KyArray<Ptr<Database>, MemStat_WorldFwk> m_databases;
	Ptr<DatabaseBinding> m_allDatabaseBinding;

	// CollisionWorld
	Ptr<CollisionWorld> m_collisionWorld;

	// WorldElements
	TrackedCollection<Ptr<PointOfInterest >, MemStat_PointOfInterest>  m_pointsOfInterests;
	TrackedCollection<Ptr<CylinderObstacle>, MemStat_CylinderObstacle> m_cylinderObstacles;
	TrackedCollection<Ptr<BoxObstacle     >, MemStat_BoxObstacle>      m_boxObstacles;
	
	TrackedCollection<Ptr<QueryQueueArray> , MemStat_QuerySystem> m_queryQueueArrays;

	TrackedCollection<Ptr<TagVolume>, MemStat_TagVolume> m_tagVolumesToBeIntegrated;
	TrackedCollection<Ptr<TagVolume>, MemStat_TagVolume> m_tagVolumesIntegrated;
	TrackedCollection<Ptr<TagVolume>, MemStat_TagVolume> m_tagVolumesToBeRemoved;

	// Spatialization
	WorldElementSpatializer* m_worldElementSpatializer;

	// Default Profile and BotConfig
	Ptr<BaseNavigationProfile> m_defaultNavigationProfile;
	Ptr<BotConfig> m_defaultPathFollowerConfig;

	// Visual Debug
	VisualDebugServerConfig m_visualDebugServerConfig;
	VisualDebugServer* m_visualDebugServer;
	bool m_wasConnected; // to detect a connection
	DisplayListManager* m_displayListManager;
	VisualDebugRegistry* m_visualDebugRegistry;
	VisualDebugAttributesManager* m_visualDebugAttributesManager;

	// WorldStatistics
	WorldStatistics* m_statistics;

	DatabaseUpdateManager* m_databaseUpdateManager;

	// Shared Avoidances workspace
	AvoidanceWorkspace m_avoidanceWorkspace;

	// Multi Threading
	Ptr<IAsyncQueryDispatcher> m_asyncQueryDispatcher;
	bool m_isInWorldUpdate;
	ThreadId m_worldUpdateThreadId;
};


KY_INLINE KyUInt32 World::GetUpdateCount() const { return m_updateCount; }
KY_INLINE WorldStatistics* World::GetWorldStatistics() { return m_statistics; }

KY_INLINE KyUInt32 World::GetDatabasesCount() const                    { return m_databases.GetCount(); }
KY_INLINE Database* World::GetDatabase(KyUInt32 index)                 { return m_databases[index]; }

KY_INLINE KyUInt32 World::GetPointsOfInterestCount() const             { return m_pointsOfInterests.GetCount(); }
KY_INLINE PointOfInterest* World::GetPointOfInterest(KyUInt32 index)   { return m_pointsOfInterests[index]; }

KY_INLINE KyUInt32 World::GetCylinderObstaclesCount() const            { return m_cylinderObstacles.GetCount(); }
KY_INLINE CylinderObstacle* World::GetCylinderObstacle(KyUInt32 index) { return m_cylinderObstacles[index]; }

KY_INLINE KyUInt32 World::GetBoxObstaclesCount() const                 { return m_boxObstacles.GetCount(); }
KY_INLINE BoxObstacle* World::GetBoxObstacle(KyUInt32 index)           { return m_boxObstacles[index]; }

KY_INLINE KyUInt32 World::GetTagVolumesCount() const
{
	return m_tagVolumesToBeIntegrated.GetCount() + m_tagVolumesIntegrated.GetCount() + m_tagVolumesToBeRemoved.GetCount();
}
KY_INLINE KyUInt32 World::GetTagVolumesToBeIntegratedCount() const      { return m_tagVolumesToBeIntegrated.GetCount(); }
KY_INLINE KyUInt32 World::GetTagVolumesIntegratedCount() const          { return m_tagVolumesIntegrated.GetCount(); }
KY_INLINE KyUInt32 World::GetTagVolumesToBeRemovedCount() const         { return m_tagVolumesToBeRemoved.GetCount(); }
KY_INLINE TagVolume* World::GetTagVolumesToBeIntegrated(KyUInt32 index) { return m_tagVolumesToBeIntegrated[index]; }
KY_INLINE TagVolume* World::GetTagVolumesIntegrated(KyUInt32 index)     { return m_tagVolumesIntegrated[index]; }
KY_INLINE TagVolume* World::GetTagVolumesToBeRemoved(KyUInt32 index)    { return m_tagVolumesToBeRemoved[index]; }

KY_INLINE KyUInt32 World::GetQueryQueueArraysCount() const           { return m_queryQueueArrays.GetCount(); }
KY_INLINE QueryQueueArray* World::GetQueryQueueArray(KyUInt32 index) { return m_queryQueueArrays[index]; }

KY_INLINE IAsyncQueryDispatcher* World::GetAsyncQueryDispatcher() { return m_asyncQueryDispatcher; }

KY_INLINE void World::SetWorldUpdateThreadId(ThreadId threadId) { m_worldUpdateThreadId = threadId; }
KY_INLINE ThreadId World::GetWorldUpdateThreadId() const { return m_worldUpdateThreadId; }

KY_INLINE KyUInt32 World::GetNumberOfFramesForShortcutTrajectoryPeriodicUpdate() const { return m_nbFramesForShortcutTrajectoryPeriodicUpdate; }
KY_INLINE void World::SetNumberOfFramesForShortcutTrajectoryPeriodicUpdate(KyUInt32 numberOfFrames) { m_nbFramesForShortcutTrajectoryPeriodicUpdate = (numberOfFrames == 0 ? 1 : numberOfFrames); }

KY_INLINE VisualDebugServer* World::GetVisualDebugServer() { return m_visualDebugServer; }
KY_INLINE VisualDebugRegistry* World::GetElementRegistry() { return m_visualDebugRegistry; }
KY_INLINE DisplayListManager* World::GetDisplayListManager() { return m_displayListManager; }
KY_INLINE VisualDebugAttributesManager* World::GetAttributesManager() { return m_visualDebugAttributesManager; }

#ifdef KY_BUILD_SHIPPING
KY_INLINE KyResult World::StartVisualDebug(const VisualDebugServerConfig&) { return KY_ERROR; }
KY_INLINE void World::StopVisualDebug() {}
#endif

} // namespace Kaim

#endif //Navigation_World_H

