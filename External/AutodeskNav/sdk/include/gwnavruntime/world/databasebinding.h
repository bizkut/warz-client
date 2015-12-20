/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: JAPA

#ifndef Navigation_DatabaseBinding_H
#define Navigation_DatabaseBinding_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/kernel/SF_RefCount.h"

namespace Kaim
{

class Database;
class World;

/// DatabaseBinding is a collection of the databases to be used for world elements spatialization.
class DatabaseBinding : public RefCountBaseNTS<DatabaseBinding, MemStat_WorldFwk>
{
public:
	static const KyUInt32 InvalidBoundDatabaseIndex;


	// ---------------------------------- Public Member Functions ----------------------------------

	DatabaseBinding() : m_world(KY_NULL) {}

	/// Adds a Database to this DatabaseBinding instance. If this is the first
	/// Database, it initialize the internals to fit the databases count in the World
	/// and add the Database. If internal bound databases array is not empty,
	/// it checks the provided database is in the same World than the one(s)
	/// already there. If not, the database is simply ignored.
	/// \pre database must be a valid pointer.
	/// \retval KY_SUCCESS The provided Database has been successfully inserted in
	///         this DatabaseBinding.
	/// \retval KY_ERROR The provided Database is either associated to no World or
	///         to another World than the databases already listed in this
	///         DatabaseBinding.
	KyResult AddDataBase(Database* database);

	World* GetWorld() { return m_world; }

	KyUInt32 GetBoundDatabaseCount() const { return m_boundDatabases.GetCount(); }

	/// \param boundDatabaseIndex The index of the Database to retrieve in bound databases array.
	/// \pre boundDatabaseIndex must be valid (less than GetBoundDatabaseCount()), it is not checked there.
	Database* GetBoundDatabase(KyUInt32 boundDatabaseIndex) const { return m_boundDatabases[boundDatabaseIndex]; }

	/// \param databaseIdxInWorld The index of the Database in World databases array.
	/// \return The index of the Database in bound databases array if the database is there; KyUInt32MAXVAL otherwise.
	KyUInt32 GetBoundDatabaseIdx(KyUInt32 databaseIdxInWorld) const { return m_boundIndicesFromIdxInWorld[databaseIdxInWorld]; }

	void Clear();

private:
	void InitForWorld(World* world);

private:
	World* m_world;
	KyArray<Database*, MemStat_WorldFwk> m_boundDatabases;            // count <= m_world->GetDatabaseCount();
	KyArray<KyUInt32, MemStat_WorldFwk> m_boundIndicesFromIdxInWorld; // count == m_world->GetDatabaseCount();
};

}

#endif
