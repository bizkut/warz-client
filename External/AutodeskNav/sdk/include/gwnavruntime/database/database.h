/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: JUBA - secondary contact: LAPA

#ifndef Navigation_Database_H
#define Navigation_Database_H

#include "gwnavruntime/world/worldelement.h"
#include "gwnavruntime/database/databasegenmetrics.h"
#include "gwnavruntime/database/databasegeometrybuildingmanager.h"
#include "gwnavruntime/database/idatabasechangebroadcaster.h"
#include "gwnavruntime/database/querystatistics.h"
#include "gwnavruntime/containers/collection.h"
#include "gwnavruntime/blob/baseblobhandler.h"
#include "gwnavruntime/database/positionspatializationrange.h"
#include "gwnavruntime/kernel/SF_Std.h"
#include "gwnavruntime/querysystem/iquery.h"


namespace Kaim
{

class AbstractGraphCellGrid;
class NavMeshGenParameters;
class NavMeshElementManager;
class NavGraphManager;
class ITriangleCostMap;
class WorkingMemory;
class ActiveData;
class NavCellGrid;
class NavData;
class Bot;


/// Defines the different mode for the NavMesh border margin to be used in RayCanGo queries.
enum RayCanGoMarginMode
{
	NoMargin = 0,            ///< Margin to be used to get ray can go exactly on the NavMesh borders.
	DiagonalStripMargin,     ///< Margin to be used when computing the DiagonalStrip used for Channel computation.
	ChannelMargin,           ///< Margin to be used for Channel computation.
	PathMargin,              ///< Margin to be used in any path computation ray can go queries to ensure the computed Path is not too close from NavMesh borders.
	IdealTrajectoryMargin,   ///< Margin to be used in any path following ray can go queries to ensure the computed trajectory is not too close from NavMesh borders.
	MinimalTrajectoryMargin, ///< Margin to be used only to reevaluate a trajectory previously validated with can go using IdealTrajectoryMargin, to avoid hysteresis on it.
	MoveOnNavMeshMargin,     ///< Margin to be used in MoveOnNavMesh or any other actual movement computation.

	RayCanGoMarginModeCount // internal
};


/// This class is a runtime container for all NavData that represents the world from the point of view of a specific type of Bots.
///  Databases are created once and for all during the World construction.
/// Through some sub-classes (NavMeshElementManager, NavGraphManager, NavCellGrid, ...), the Database performs the additions and removals
///  of NavData, maintaining a coherent state of "active" navigable data on which Bots are moving, Queries are performed, in term
///  of overlaps, dynamic version of NavFloors, stitching of the NavCells, etc...
/// The Database also maintains an instance of WorkingMemory, which is used by default by the Queries that are not processed within
///  a QueryQueue. 
class Database : public WorldElement
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_WorldFwk)
	KY_CLASS_WITHOUT_COPY(Database)

public:
	static  WorldElementType GetStaticType()       { return TypeDatabase; }
	virtual WorldElementType GetType()       const { return TypeDatabase; }
	virtual const char*      GetTypeName()   const { return "Database";   }
	virtual void DoSendVisualDebug(VisualDebugServer& visualDebugServer, VisualDebugSendChangeEvent changeEvent); 

	Database(World* world, KyUInt32 databaseIndex);
	virtual ~Database();

	void Clear(); // internal. RemoveAll navData, bots, clear memory of cell grid and activeData

	// ---------------------------------- Public Member Functions ----------------------------------

	/// Retrieves the index of the Database in the array of Databases maintained by the World.
	KyUInt32 GetDatabaseIndex() const;

	/// Retrieves the DatabaseGenMetrics, which offers services for converting between floating-point coordinates
	///  and the integer coordinate system used internally by the NavData.
	const DatabaseGenMetrics& GetDatabaseGenMetrics() const;

	DatabasePositionSpatializationRange&       GetDatabasePositionSpatializationRange();
	const DatabasePositionSpatializationRange& GetDatabasePositionSpatializationRange() const;
	const PositionSpatializationRange&         GetPositionSpatializationRange()         const;

	/// Returns true if the Database is not associated to any set of generation parameters. It false is returned, it means that
	///  only NavData compatible with current generation parameters can be added. In this case, you can use the function
	///  IsCompatibleWith to check that.
	/// Note that Databases are cleared only during a call to World::RemoveAndCancellAll()
	bool IsClear() const;

	/// Indicates whether or not the specified NavData object was created with the same generation parameters
	/// as the NavData objects that have already been loaded into this Database so that it can be added too.
	bool IsCompatibleWith(const NavData& navData) const;

	/// Remove all the NavData from this Database.
	/// \pre This function cannot be called if the Database in updating
	void RemoveAllNavData();
	KyUInt32 GetAliveNavDataCount() const;

	void RemoveAllBots();
	KyUInt32 GetBotsCount() const;
	Bot* GetBot(KyUInt32 index) const;

	/// The WorkingMemory used by default by Queries when processed outside a QueryQueue. You should not need
	//  to access this explicitly except for modifying the memory size limit of this WorkingMemory.
	WorkingMemory* GetWorkingMemory() const;

	/// Returns a CellBox covering all NavData that has been changed on last World::Update().
	const CellBox& GetLastUpdateCellBox() const;

	/// Returns true when something has changed on last World::Update(). This is useful when
	/// you want to maintain some information on your own that is based on some
	/// NavData element. You can then call GetLastUpdateCellBox() to retrieve the
	/// overall CellBox concerned by last changes.
	bool HasChangedLastFrame() const;

public: // internal
	/// Return statistics on queries called on this Database
	PerformQueryStatistics* GetPerformQueryStatistics();

	void SetEventBroadcaster(IDatabaseChangeBroadcaster* eventBroadCaster);
	void UpdateImmediate();

	// ---------------------------------- Internally used by the NavigationLab for Database Visual Representation ----------------------------------
	// If a IVisualGeometryFactory is provided with SetVisualGeometryFactory(), the database will use a
	// grid of IVisualGeometry that can be independently rebuild when only part of the database change.
	// If no IVisualGeometryFactory has been set, it is still possible to display the all database in
	// only one IVisualGeometry by providing a IVisualGeometry with SetVisualGeometry(). Note that building
	// the IVisualGeometry for all the database can be time consuming in big worlds. 

	/// Sets the instance of IVisualGeometryFactory that will be used by the DatabaseVisRepGrid to
	/// create IVisualGeometry to which this object will send triangles for rendering a tile of the Database
	void SetVisualGeometryFactory(Ptr<IVisualGeometryFactory> factory);

	/// Sets the instance of IVisualGeometry to which this object will send its triangles for rendering when
	/// ComputeVisualGeometry() is called. 
	void SetVisualGeometry(Ptr<IVisualGeometry> geometry);


	/// Sends to the IVisualGeometry or to the IVisualGeometry of the DatabaseVisRepGrid (depending on what you provided)
	/// a set of triangles that express the data maintained by this object for rendering.
	void BuildVisualGeometry(bool forceRebuildAll = false);


	/// Retrieves the instance of the DatabaseGeometryBuildingManager maintained by this object, which manage
	/// the tiling of DatabaseVisualGeometryBuilder.
	DatabaseGeometryBuildingManager& GetDatabaseGeometryBuildingManager();

	/// Retrieves the instance of IVisualGeometry to which this object sends its triangles for rendering set
	/// Using SetVisualGeometry() or KY_NULL if none has been set. 
	IVisualGeometry* GetVisualGeometry() const;

	// ---------------------------------- Called by various components, World, Queries ----------------------------------

	ActiveData* GetActiveData() const;

	bool IsThereNavDataToBeAddedOrRemoved() const; // call by World::Update
	bool IsDatabaseUpdatingNavData() const; // call by World::Update

	void UpdateWithDynamicNavFloors(const CellBox& enlargeUpdateCellBox);

	void TakeSnapshotAndPrepareActiveDataUpdate(QueryProcessMode processMode);
	void UpdateActiveDataAndClearSnapshot(); // call by World::Update
	void UpdateBotsPathFollowing(KyFloat32 simulationTimeInSeconds); // call by World::Update
	void UpdateBotsGenMetrics();

	KyUInt32 GetRayCanGoMarginInt(RayCanGoMarginMode marginMode);
	KyFloat32 GetRayCanGoMarginFloat(RayCanGoMarginMode marginMode);

	void SetupGenerationMetrics(const NavMeshGenParameters& navMeshGenParameters); // called by internal members
	
public: // DEPRECATED functions

	// This function was called each frame internally, it should not be called anymore.
	KY_DEPRECATED(void ResetChannelValidityStatusOfAllBots());

private:
	friend class DatabaseUpdateManager;
	friend class NavMeshElementManager;
	void IncrementNavDataChangeIndex();

	void RemoveNavGraphsOfNavDataBeeingRemoved();
	void AddNavGraphsOfNavDataBeeingAdded();

	void SnapShotOfNavDataToUpdate();
	void ClearSnapShotOfNavDataToUpdate();

	void PrepareNavMeshInsertionOfNavDataBeeingAdded();
	void PrepareNavMeshRemovalOfNavDataBeeingRemoved();
	void RemoveNavMeshOfNavDataBeeingRemoved();

	void ComputeCellNextActiveNessStatus(QueryProcessMode queryProcessMode);
	void UpdateStitchOneToOneAndLaunchRuntimeStitchQueries(QueryProcessMode queryProcessMode);
	void UpdateActiveData();

	void PrepareAbstractGraphInsertionOfNavDataBeeingAdded();
	void InsertAbstractGraphOfNavDataBeeingAdded();
	void RemoveAbstractGraphOfNavDataBeeingRemoved();

	void ForceSendAllDynamicFloorsAtCellPos(VisualDebugServer &visualDebugServer);

	enum CompatibilityStatus
	{
		Compatibility_Untested,
		Compatibility_Error,
		Compatibility_Success,
	};
	CompatibilityStatus IsCompatibleWith(const NavData& navData, const TrackedCollection<Ptr<NavData>, MemStat_WorldFwk>& navDataCollection) const;

public: // internal
	KyUInt32 m_navdataChangeIdx; // 0 in an invalid Value
	KyUInt32 m_frameIdx; // mainly to be accessed by queries to check if consecutive calls to Advance occur during the same frame
	CellBox m_lastUpdateBox;

	DatabasePositionSpatializationRange m_positionSpatializationRange;
	DatabaseGenMetrics m_generationMetrics;          ///< generation parameters. Set by the NavMeshElementManager. Do Not Modify

	ActiveData*             m_activeData;            ///< all the data that are active
	NavCellGrid*            m_navCellGrid;           ///< bookeeping, stitching, spatialization, NavCells version management, ...
	NavMeshElementManager*  m_navMeshElementManager; ///< NavMesh container + GUID manager/sorter
	NavGraphManager*        m_navGraphManager;       ///< NavGraph container
	AbstractGraphCellGrid*  m_abstractGraphCellGrid;
	WorkingMemory*          m_workingMemory;

	TrackedCollection<Ptr<Bot>, MemStat_WorldFwk> m_bots;

	TrackedCollection<Ptr<ITriangleCostMap>, MemStat_WorldFwk> m_triangleCostMaps;

	TrackedCollection<Ptr<NavData>, MemStat_WorldFwk> m_navDataToBeAdded;
	TrackedCollection<Ptr<NavData>, MemStat_WorldFwk> m_navDataBeeingAdded;
	TrackedCollection<Ptr<NavData>, MemStat_WorldFwk> m_navDatas;
	TrackedCollection<Ptr<NavData>, MemStat_WorldFwk> m_navDataToBeRemoved;
	TrackedCollection<Ptr<NavData>, MemStat_WorldFwk> m_navDataBeeingRemoved;

	Ptr<IDatabaseChangeBroadcaster> m_eventBroadCaster;

	DatabaseGeometryBuildingManager m_geometryBuildingManager;

	bool m_needToSendActiveNavData;        ///< (VisualDebug) Avoids sending all the active NavData each frame. Accessed by the NavCellGrid.
	Collection<Ptr<BaseBlobHandler> > m_visualDebugBlobCollection; ///< (VisualDebug) store the blob ot be send during the DoSendVisualDebug call.

private:
	PerformQueryStatistics* m_queryStatistics;

	KyUInt32 m_databaseIndex; // Index of the Database in the world, set once and for all in the constructor

	// internal for update
	CellBox m_currentNavMeshUpdateCellBox;
	CellBox m_currentNavGraphUpdateCellBox;
	CellBox m_currentAbstractGraphUpdateCellBox;
	KyUInt32 m_navDataWithNavMeshElementCount;
	KyUInt32 m_navDataWithAbstractGraphCount;
};

KY_INLINE bool           Database::IsClear()                  const { return m_generationMetrics.IsClear(); }
KY_INLINE KyUInt32       Database::GetDatabaseIndex()         const { return m_databaseIndex;               }
KY_INLINE KyUInt32       Database::GetBotsCount()             const { return m_bots.GetCount();             }
KY_INLINE Bot*           Database::GetBot(KyUInt32 index)     const { return m_bots[index];                 }
KY_INLINE WorkingMemory* Database::GetWorkingMemory()         const { return m_workingMemory;               }
KY_INLINE ActiveData*    Database::GetActiveData()            const { return m_activeData;                  }
KY_INLINE const CellBox& Database::GetLastUpdateCellBox()     const { return m_lastUpdateBox;               }
KY_INLINE bool           Database::HasChangedLastFrame()      const { return m_lastUpdateBox.IsValid();     }

KY_INLINE bool     Database::IsThereNavDataToBeAddedOrRemoved() const { return m_navDataToBeAdded.GetCount()   + m_navDataToBeRemoved.GetCount() != 0; }
KY_INLINE bool     Database::IsDatabaseUpdatingNavData()   const { return m_navDataBeeingAdded.GetCount() + m_navDataBeeingRemoved.GetCount() != 0; }
KY_INLINE KyUInt32 Database::GetAliveNavDataCount()        const { return m_navDatas.GetCount() + m_navDataToBeRemoved.GetCount() + m_navDataBeeingRemoved.GetCount(); }

KY_INLINE DatabaseGeometryBuildingManager&           Database::GetDatabaseGeometryBuildingManager()           { return m_geometryBuildingManager;                                      }
KY_INLINE const DatabaseGenMetrics&                  Database::GetDatabaseGenMetrics()                  const { return m_generationMetrics;                                            }
KY_INLINE const DatabasePositionSpatializationRange& Database::GetDatabasePositionSpatializationRange() const { return m_positionSpatializationRange;                                  }
KY_INLINE DatabasePositionSpatializationRange&       Database::GetDatabasePositionSpatializationRange()       { return m_positionSpatializationRange;                                  }
KY_INLINE const PositionSpatializationRange&         Database::GetPositionSpatializationRange()         const { return m_positionSpatializationRange.GetPositionSpatializationRange(); }

KY_INLINE IVisualGeometry* Database::GetVisualGeometry() const  { return m_geometryBuildingManager.GetVisualGeometry(); }

KY_INLINE void Database::SetVisualGeometryFactory(Ptr<IVisualGeometryFactory> factory) { GetDatabaseGeometryBuildingManager().SetVisualGeometryFactory(factory); }
KY_INLINE void Database::SetVisualGeometry(Ptr<IVisualGeometry> geometry)              { GetDatabaseGeometryBuildingManager().SetVisualGeometry(geometry); }
KY_INLINE void Database::IncrementNavDataChangeIndex() { m_navdataChangeIdx = m_navdataChangeIdx != KyUInt32MAXVAL ? m_navdataChangeIdx + 1 : 1; }

KY_INLINE void Database::BuildVisualGeometry(bool forceRebuildAll) { m_geometryBuildingManager.BuildGeometry(forceRebuildAll); }

KY_INLINE KyUInt32 Database::GetRayCanGoMarginInt(RayCanGoMarginMode rayCanGoMarginMode)
{
	static const KyUInt32 marginValues[RayCanGoMarginModeCount] = {  //   PixelSize | 5 cm | 10 cm | 20 cm | 30 cm 
		0,  // UInt32 value for NoMargin.                            // ------------+------+-------+-------+-------
		10, // UInt32 value for DiagonalStripMargin                  // value in cm |      |       |       |       // Just a bit less than ChannelMargin
		15, // UInt32 value for ChannelMargin                        // value in cm |      |       |       |       // Very small to prevent polygon shrinking errors
		45, // UInt32 value for PathMargin.                          // value in cm | 1.76 |  3.52 |  7.03 | 10.55 
		35, // UInt32 value for IdealTrajectoryMargin.               // value in cm | 1.37 |  2.73 |  5.47 |  8.20 
		30, // UInt32 value for MinimalTrajectoryMargin.             // value in cm | 1.17 |  2.34 |  4.69 |  7.03 
		10  // UInt32 value for MoveOnNavMeshMargin.                 // value in cm | 0.39 |  0.78 |  1.56 |  2.34 
	};
	return marginValues[rayCanGoMarginMode];
}

KY_INLINE KyFloat32 Database::GetRayCanGoMarginFloat(RayCanGoMarginMode rayCanGoMarginMode)
{
	return GetRayCanGoMarginInt(rayCanGoMarginMode) * GetDatabaseGenMetrics().m_integerPrecision;
}

KY_INLINE PerformQueryStatistics* Database::GetPerformQueryStatistics() { return m_queryStatistics; }

} // namespace Kaim

#endif // Navigation_Database_H
