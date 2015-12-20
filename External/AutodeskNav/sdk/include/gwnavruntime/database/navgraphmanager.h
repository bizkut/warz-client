/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavGraphManager_H
#define Navigation_NavGraphManager_H

#include "gwnavruntime/navgraph/navgraph.h"
#include "gwnavruntime/database/navgraphlinker.h"

namespace Kaim
{
class Database;
class NavCellGrid;
class NavGraphBlob;
class NavData;

// The NavGraphManager manages the stitching of \NavGraphs at runtime. You should not need to interact with the
// NavGraphManager directly.
class NavGraphManager
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_WorldFwk)
	KY_CLASS_WITHOUT_COPY(NavGraphManager)

public:
	KyUInt32 GetNumberOfNavGraph() const;
	NavGraph* GetNavGraph(KyUInt32 index) const;

public: // internal
	NavGraphManager(Database* database);
	~NavGraphManager();

	void Clear();

	void StartInsertion();
	NavGraph* InsertNavGraph(const NavGraphBlob* navGraphBlob, NavData* navData);
	void EndInsertion();

	void StartRemoval();
	void RemoveNavGraph(NavGraph* navGraph);
	void EndRemoval();

	void LinkWaitingNavGraphs(CellBox& cellBox);
	bool HasWaitingNavGraphs();
public: // internal
	Database* m_database;
	NavGraphLinker m_navGraphLinker;

private:
	KyUInt32 m_numberOfNavGraphToUpdate;
	CellBox m_cellBox;

	Collection<NavGraph*, MemStat_NavData> m_waitingNavGraphs;
	TrackedCollection<Ptr<NavGraph>, MemStat_NavData> m_navGraphs;
};


KY_INLINE void NavGraphManager::StartInsertion()
{
	KY_LOG_ERROR_IF(m_numberOfNavGraphToUpdate != 0, ("At this point, the number of Graph to update must be set to 0"));
	m_cellBox.Clear();
}
KY_INLINE void NavGraphManager::StartRemoval()
{
	KY_LOG_ERROR_IF(m_numberOfNavGraphToUpdate != 0, ("At this point, the number of Graph to update must be set to 0"));
	m_cellBox.Clear();
}

KY_INLINE void NavGraphManager::EndRemoval() {}

KY_INLINE KyUInt32  NavGraphManager::GetNumberOfNavGraph()       const { return m_navGraphs.GetCount(); }
KY_INLINE NavGraph* NavGraphManager::GetNavGraph(KyUInt32 index) const
{
	KY_DEBUG_ASSERTN(index < GetNumberOfNavGraph(), ("Bad index %u (max is %u)", index, GetNumberOfNavGraph()));
	return m_navGraphs[index];
}

KY_INLINE bool NavGraphManager::HasWaitingNavGraphs() { return m_waitingNavGraphs.IsEmpty() == false; }

} // namespace Kaim

#endif // Navigation_NavGraphManager_H
