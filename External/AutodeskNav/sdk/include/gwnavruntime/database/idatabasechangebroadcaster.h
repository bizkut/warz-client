/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: JUBA - secondary contact: LAPA

#ifndef Navigation_INavdataChangeInDatabaseObserver_H
#define Navigation_INavdataChangeInDatabaseObserver_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/containers/collection.h"

namespace Kaim
{

class Database;
class NavData;

class ChangeInDatabaseDesc
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_WorldFwk)
public:
	enum ChangeType { NavDataAddAndRemove, DynamicNavMeshUpdate };

	ChangeType m_type;
	const TrackedCollection<Ptr<NavData>, MemStat_WorldFwk>* m_navDataBeeingAdded;
	const TrackedCollection<Ptr<NavData>, MemStat_WorldFwk>* m_navDataToBeRemoved;
	CellBox m_currentUpdateCellBox;

public: // internal
	// ctor for dynamicNavMesh update notification
	ChangeInDatabaseDesc(const CellBox& currentUpdateCellBox);
	// ctor for navData add/remove 
	ChangeInDatabaseDesc(const CellBox& currentUpdateCellBox, const TrackedCollection<Ptr<NavData>, MemStat_WorldFwk>& navDataBeeingAdded,
	const TrackedCollection<Ptr<NavData>, MemStat_WorldFwk>& navDataToBeRemoved);
};



class IDatabaseChangeBroadcaster : public RefCountBase<IDatabaseChangeBroadcaster, Stat_Default_Mem>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_WorldFwk)
	KY_CLASS_WITHOUT_COPY(IDatabaseChangeBroadcaster)
public:
	IDatabaseChangeBroadcaster(Database* database = KY_NULL) : m_database(database) {}
	void SetDatabase(Database* database) { m_database = database; }
	virtual void BroadCastChangeInDatabase(ChangeInDatabaseDesc& changeInDatabaseDesc) = 0;
public:
	Database* m_database;
};



class DefaultDatabaseChangeBroadcaster : public IDatabaseChangeBroadcaster
{
public:
	DefaultDatabaseChangeBroadcaster(Database* database) : IDatabaseChangeBroadcaster(database) {}
	virtual void BroadCastChangeInDatabase(ChangeInDatabaseDesc& changeInDatabaseDesc);
};



KY_INLINE ChangeInDatabaseDesc::ChangeInDatabaseDesc(const CellBox& currentUpdateCellBox) : 
m_type(DynamicNavMeshUpdate), m_navDataBeeingAdded(KY_NULL), m_navDataToBeRemoved(KY_NULL), m_currentUpdateCellBox(currentUpdateCellBox) {}
KY_INLINE ChangeInDatabaseDesc::ChangeInDatabaseDesc(const CellBox& currentUpdateCellBox, const TrackedCollection<Ptr<NavData>, MemStat_WorldFwk>& navDataBeeingAdded,
	const TrackedCollection<Ptr<NavData>, MemStat_WorldFwk>& navDataToBeRemoved) :
	m_type(NavDataAddAndRemove), m_navDataBeeingAdded(&navDataBeeingAdded), m_navDataToBeRemoved(&navDataToBeRemoved), m_currentUpdateCellBox(currentUpdateCellBox) {}

} // namespace Kaim

#endif // Navigation_Database_H
