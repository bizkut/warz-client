/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavMeshElementManager_H
#define Navigation_NavMeshElementManager_H

#include "gwnavruntime/database/navcellgrid.h"
#include "gwnavruntime/database/activeguids.h"

#include "gwnavruntime/math/box2f.h"


namespace Kaim
{

class Database;
class NavGraphManager;

/// This class is a runtime container for the all NavMeshElement that are created from the NavMeshElementBlob added in this Database.
/// It is responsible for creating these NavMeshElement, for sorting the NavMeshElement according to their GuidCompound to speed up
/// the NavFloor stitching performed by the NavCellGrid.
/// You should not need to access this class except if you need to browse all theNavMeshElement within a Database.
class NavMeshElementManager
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_WorldFwk)
	KY_CLASS_WITHOUT_COPY(NavMeshElementManager)
public:

	/// Indicates the number of NavMeshes that have been added to the NavMeshElementManager. 
	KyUInt32 GetNavMeshElementCount() const;

	/// Retrieves the NavMesh with the specified index from within the list of NavMeshes in the NavMeshElementManager. 
	NavMeshElement* GetNavMeshElement(KyUInt32 idx) const;

public: // internal
	NavMeshElementManager(Database* database);
	~NavMeshElementManager();
	void SetNavGraphManager(NavGraphManager* navGraphManager);
	void Clear();

	NavMeshElement* InsertNavMeshElement(const NavMeshElementBlob* navMeshElementBlob); // Call by the Database
	void PrepareNavMeshRemoval(NavMeshElement* navMeshElement);  // Call by the Database
	void RemoveNavMeshElement(NavMeshElement* navMeshElement);  // Call by the Database
	void SortNavMeshAccordingToGuidCompound(CellBox& currentUpdateCellBox, bool forceActiveIfAlone = false);

public:

	/// Called internally before adding a NavMeshElement to the NavMeshElementManager. Calls IsCompatibleWith() to validate
	/// that the generation parameters in the NavMeshElement match any existing NavMeshes already added to the
	/// NavMeshElementManager. If no NavMeshes have been added to the NavMeshElementManager, it validates that the generation
	/// parameters are not set to invalid values. 
	KyResult CheckGenerationParameters(const NavMeshGenParameters& navMeshGenParameters);

	/// Indicates whether the specified NavMeshElement was created with the same generation parameters
	/// as the NavMeshes that have already been loaded into this NavMeshElementManager. 
	bool IsCompatibleWith(const NavMeshGenParameters& navMeshGenParameters) const;

public: // internal
	// Used by the generationd
	NavMeshElement* SingleInsertion(const NavMeshElementBlob* navMeshElementBlob, bool forceActiveIfAlone = false);
	void SingleRemoval(NavMeshElement* navMeshElement);
public: // internal
	Database* m_database;

	ActiveGuids m_activatedGuids;
	GuidCompoundSorter m_guidCompoundSorter;

private:
	TrackedCollection<Ptr<NavMeshElement>, MemStat_NavData> m_navMeshElements;
};

KY_INLINE NavMeshElementManager::~NavMeshElementManager() { Clear(); }

KY_INLINE KyUInt32 NavMeshElementManager::GetNavMeshElementCount() const { return m_navMeshElements.GetCount(); }

KY_INLINE NavMeshElement* NavMeshElementManager::GetNavMeshElement(KyUInt32 idx) const
{
	KY_DEBUG_ASSERTN(idx < GetNavMeshElementCount(), ("Input idx must be lower than GetNumberOfInsertedNavMesh() == %d", GetNavMeshElementCount()));
	return m_navMeshElements[idx];
}

KY_INLINE void NavMeshElementManager::SortNavMeshAccordingToGuidCompound(CellBox& currentUpdateCellBox, bool forceActiveIfAlone)
{
	m_guidCompoundSorter.Sort(m_activatedGuids, currentUpdateCellBox, forceActiveIfAlone);
}

}


#endif //Navigation_NavMeshElementManager_H

