/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: JUBA - secondary contact: GUAL
#ifndef Navigation_NavData_H
#define Navigation_NavData_H

#include "gwnavruntime/blob/blobaggregate.h"
#include "gwnavruntime/world/worldelement.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/navdata/sectordescriptor.h"

namespace Kaim
{

class NavMeshElement;
class NavMeshElementBlob;
class NavGraph;
class NavGraphBlob;
class NavGraphBlobBuilder;
class AbstractGraph;
class AbstractGraphBlob;
class Database;
class VisualDebugServer;
class KyGuid;

/// The NavData class is the object containing navigation data that will be added to one Database. Navigation data is
///  the information Bots and other AI components use to understand the terrain and their movement opportunities. This is comprised
///  ofNavMesh (abstract representations of the areas considered navigable) and NavGraph (abstract representations of the
///  topology of the terrain).
/// To be usable by bots, for query processing or other AI components, a NavData must be added to a Database and be "alive" in this
/// Database, which means the addition must be effectively done (that may take several frames in async mode).
/// Read-Only navigation data (NavMeshElementBlob, NavGraphBlob, ...) are store within a BlobAggregate that can be shared through
///  several instances of NavData in a multi World scenario with memory sharing. In this case, create one BlobAggregate for use
///  in all worlds, then create a NavData object for each world where the NavData will be used. In a singleWorld scenario or multi
///  World without memory sharing, use this class directly, do not use BlobAggregate.
/// The NavData class offers a mechanism for loading data from a .NavData files created by the Gameware Navigation Generation framework.
class NavData : public WorldElement
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
	KY_CLASS_WITHOUT_COPY(NavData)

public:
	static  WorldElementType GetStaticType()       { return TypeNavData; }
	virtual WorldElementType GetType()       const { return TypeNavData; }
	virtual const char*      GetTypeName()   const { return "NavData";   }
	virtual void DoSendVisualDebug(VisualDebugServer& server, VisualDebugSendChangeEvent changeEvent);

	// ---------------------------------- Public Member Functions ----------------------------------


	/// Enumerates the different status a NavData can have in one Database.
	/// Note that when a NavData is effectively removed from a Database, its status is set to NotReferencedInAnyDatabase
	enum DatabaseStatus
	{
		NotReferencedInAnyDatabase = 0, ///< 
		ToBeAdded_NotAlive         = 1, ///< 
		BeingAdded_NotAlive        = 2, ///< 
		AddedToDabase_Alive        = 3, ///< 
		ToBeRemoved_Alive          = 4, ///< 
		BeingRemoved_Alive         = 5  ///< 
	};


	NavData(Database* database = KY_NULL); // call Init
	virtual ~NavData();

	Database*      GetDatabase()       const;
	DatabaseStatus GetDatabaseStatus() const;
	
	/// Gives the Guid used to identify the NavMesh contained in this NavData.
	/// return KY_NULL if no NavMesh is contained, SectorName will be used instead for identifying the NavData.
	const KyGuid*  GetMainGuid()       const;
	
	/// Returns the Name passed to Kaim::GeneratorSector. Used for VisualDebug to access the NavData and ClientInput files.
	/// Used for VisualDebug to access the NavData and ClientInput files.
	/// It is used to identify .NavData files containing only NavGraphs
	const String& GetSectorName() const;
	/// Returns the relative output directory used to load the file when visual debugging.
	/// Typically, it is provided to the generator when this NavData was built
	const String& GetGeneratorRelativeOutputDirectory() const;

	// ---------------------------------- Memory loading or sharing Functions ----------------------------------

	/// To use in single World scenario, or multi world scenario without NavData memory sharing.
	/// Must be called before adding the NavData to its associated Database.
	KyResult Load(const char* fileName, FileOpenerBase* fileOpener = KY_NULL);
	KyResult Load(File* file);
	KyResult LoadFromMemory(void* memory);

	/// This function is here to save a graph-only NavData, at runtime or in Generation Post Process. (This function should probably be called SaveGraphOnlyNavData.)
	/// Note that it forces m_sectorName to be the simple filename without extension.
	KyResult Save(const char* fileName, FileOpenerBase* fileOpener = KY_NULL, Endianness::Type endianness = Endianness::BigEndian);

	/// To use in multi World scenario with NavData memory sharing.
	/// Must be called before adding the NavData to its associated Database.
	void SetBlobAggregate(BlobAggregate* BlobAggregate);


	// ---------------------------------- Main API Functions ----------------------------------

	/// Associates current instance to a Database.
	/// This function may return KY_ERROR if the current instance of NavData has already been added to a Database.
	/// If a valid Database is passed to the constructor, you do not need to call this function.
	KyResult Init(Database* database);

	/// This function creates and add a NavGraphBlob to the BlobAggregate of this NavData.
	/// If no BlobAggregate is present, one will be created (which means you can add a NavGraph to an empty NavData).
	/// \pre ThisNavData must not have been added to a Database, or completely removed from the Database.
	KyResult AddNavGraph(NavGraphBlobBuilder* navGraphBlobBuilder);

	/// This function does not modify the alive NavData in the Database immediately, the NavData won't be immediately
	///  alive in the Database and associated NavMesh/NavGraph won't be immediately "activated". This will be done
	///  asynchronously during the the next calls to World::Update().
	/// The addition of a NavaData is effectively done when GetDatabaseStatus()==AddedToDabase.
	/// If you want the NavData to be immediately and effectively added, use AddToDatabaseImmediate() instead.
	/// It increments this->RefCount
	/// It returns KY_ERROR if the NavData has no chance to be successfully added to the Database.
	/// KY_SUCCESS otherwise, check m_databaseStatus to know when it is effectively alive in the database.
	KyResult AddToDatabaseAsync(); 

	/// This function does not modify the alive NavData in the Database immediately. The NavData won't be immediately
	///  removed from the database and associated NavMesh/NavGraph won't be immediately "de-activated". This will done
	///  asynchronously during the the next calls to World::Update().
	/// The removal of a NavaData is effectively done when GetDatabaseStatus()==NotReferencedInAnyDatabase.
	/// If you want the NavData to be immediately and effectively removed, use RemoveFromDatabaseImmediate() instead.
	/// It may decrements this->RefCount
	KyResult RemoveFromDatabaseAsync();

	/// Add the NavData to the Database outside the World::Update(). The NavData addition is fully performed
	///  and the NavData becomes alive in the Database just after this call.
	/// It must not be used when you have some asynchronousQuery running outside the World::Update().
	/// It must not be used if you have called AddToDatabaseAsync() or RemoveFromDatabaseAsync() with other NavData.
	/// As this function calls for an immediate update of the database, it can cause CPU peaks.
	/// It increments this->RefCount
	/// It returns KY_SUCCESS if the NavData is successfully added to the database. KY_ERROR otherwise.
	KyResult AddToDatabaseImmediate();

	/// Remove the NavData from the Database outside the World::Update(). The NavData removal is fully performed
	///  and the NavData is nor more alive in the Database after this call.
	/// It must not be used when you have some asynchronousQuery running outside the World::Update().
	/// It must not be used if you have called AddToDatabaseAsync() or RemoveFromDatabaseAsync() with other NavData.
	/// As this function calls for an immediate update of the database, it can cause CPU peaks.
	/// It decrements this->RefCount
	KyResult RemoveFromDatabaseImmediate();

	/// Indicates whether the specified NavData has been effectively added to the Database and not 
	///  effectively removed yet from the Database. That means that, through the Database, it will be used for
	/// Query processing and Bot PathFollowing, etc...
	/// It corresponds to these 3 DatabaseStatus : AddedToDabase_Alive, ToBeRemoved_Alive, BeingRemoved_Alive.
	bool IsAliveInDatabase() const;

	/// Indicates whether the specified NavData was created with the same generation parameters
	/// as this object. If this method returns true, the two NavData objects can be loaded into the
	/// same Database at the same time. 
	bool IsCompatibleWith(const NavData& navData) const;

public: //internal 
	void ComputeCellBoxOfNavMeshes();
	void ComputeCellBoxOfNavGraphs();
	void ComputeCellBoxOfAbstractGraphs();

	void OnNavDataBeeingAdded();
	void OnNavDataAdditionFinished();
	void OnNavDataBeeingRemoved();
	void OnNavDataRemoveFinished();

	BlobAggregate::Collection<NavMeshElementBlob>  GetNavMeshElementBlobCollection()  const;
	BlobAggregate::Collection<NavGraphBlob> GetNavGraphBlobCollection() const;
	BlobAggregate::Collection<AbstractGraphBlob> GetAbstractGraphBlobCollection() const;

	void OnLinkInfoChange();

	void AddSectorDescriptorToBlobAggregate();
	KyUInt32 GetMainNavMeshElementIndexInCollection() const;

public: //internal 
	Database* m_database;
	DatabaseStatus m_databaseStatus;
	Ptr<BlobAggregate> m_blobAggregate; // do not set m_blobAggregate directly, use SetBlobAggregate()
	KyArrayPOD<NavMeshElement* , MemStat_NavData> m_navMeshElements;
	KyArrayPOD<NavGraph*, MemStat_NavData> m_navGraphs;
	KyArrayPOD<AbstractGraph*, MemStat_NavData> m_abstractGraphs;
	CellBox m_navDataCellBox;
	SectorDescriptor m_sectorDescriptor;

	enum VisualDebugStatus { VisualDebugStatus_NothingToSend, VisualDebugStatus_SendALL, VisualDebugStatus_SendGraphLinkInfo };
	VisualDebugStatus m_visualDebugStatus; // visual debug purpose
};

KY_INLINE NavData::NavData(Database* database) 
	: m_database(KY_NULL)
	, m_databaseStatus(NotReferencedInAnyDatabase)
	, m_visualDebugStatus(VisualDebugStatus_NothingToSend)
{
	Init(database);
}

KY_INLINE NavData::~NavData() {}

KY_INLINE bool NavData::IsAliveInDatabase() const 
{ return m_databaseStatus == AddedToDabase_Alive || m_databaseStatus == ToBeRemoved_Alive || m_databaseStatus == BeingRemoved_Alive; }

KY_INLINE NavData::DatabaseStatus NavData::GetDatabaseStatus() const { return m_databaseStatus; }
KY_INLINE Database*               NavData::GetDatabase()       const { return m_database;       }

KY_INLINE void NavData::OnNavDataBeeingAdded()                         { m_databaseStatus = NavData::BeingAdded_NotAlive;   }
KY_INLINE void NavData::OnNavDataBeeingRemoved()                       { m_databaseStatus = NavData::BeingRemoved_Alive; }

KY_INLINE void NavData::OnNavDataAdditionFinished()
{
	m_databaseStatus = NavData::AddedToDabase_Alive;
	m_visualDebugStatus = VisualDebugStatus_SendALL;
	RegisterToVisualDebug();
}

KY_INLINE void NavData::OnNavDataRemoveFinished()
{
	m_databaseStatus = NavData::NotReferencedInAnyDatabase; 
	UnRegisterFromVisualDebug();
}

KY_INLINE void NavData::OnLinkInfoChange()
{
	if (m_visualDebugStatus == VisualDebugStatus_NothingToSend)
		m_visualDebugStatus = VisualDebugStatus_SendGraphLinkInfo;
}


}


#endif

