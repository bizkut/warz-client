/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavMeshElement_H
#define Navigation_NavMeshElement_H

#include "gwnavruntime/navmesh/blobs/navmeshelementblob.h"

namespace Kaim
{

class NavCell;
class Database;
/// This class is a runtime wrapper of a NavMeshElementBlob.
/// It is used to store runtime data associated to this NavMeshElementBlob and to gathers all the corresponding NavCells.
/// It is also used as a safe guard against the removal of the underlying NavMeshElementBlob to prevent us from accessing
/// free memory and crashing.
class NavMeshElement
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
	KY_CLASS_WITHOUT_COPY(NavMeshElement)
	KY_REFCOUNT_MALLOC_FREE(NavMeshElement)

public:
	bool IsStillLoaded() const; ///< This function returns false if the NavData containing corresponding NavMeshElementBlob has been removed from the Database.
	const NavMeshElementBlob* GetNavMeshElementBlob() const;
	const GuidCompound&       GetGuidCompound()       const; ///< Retrieves the GuidCompound that uniquely identifies this NavMeshElement.

public: // internal
	void OnRemove(); // Call OnRemove on each NavCell

	KyUInt32 GetIndexInCollection() const;                 // contract to be in a TrackedCollection
	void SetIndexInCollection(KyUInt32 indexInCollection); // contract to be in a TrackedCollection

	static Ptr<NavMeshElement> Create(const NavMeshElementBlob& navMeshElementBlob, Database* database, MemoryHeap* pheap = KY_NULL);
private:
	static KyUInt32 ComputeSizeForNavMeshElement(const NavMeshElementBlob& navMeshElementBlob);
	static KyUInt32 ComputeTotalSizeForNavMeshElement(const NavMeshElementBlob& navMeshElementBlob);

	~NavMeshElement(); // This object is create by a placement new within a big buffer. Its destructor is never called.

public: // internal
	Database* m_database;     ///< The Database in which the parentNavData has been added to. Do not modify. 
	NavCell** m_navCells;     ///< The array of NavCells that make up this NavMeshElement. Do not modify. 
	KyUInt32 m_numberOfCells; ///< The number of NavCells in #m_navCells. Do not modify.

	KyUInt32 m_indexInCollection;      ///< For TrackedCollection
	KyUInt32 m_guidCompoundSortedIdx;
	bool m_guidCompoundIsCompatible;
private:
	const NavMeshElementBlob* m_navMeshElementBlob; ///< The NavMeshElementBlob that contains the static data for this NavMesh. Do not modify. 
};

KY_INLINE bool NavMeshElement::IsStillLoaded() const { return m_navMeshElementBlob != KY_NULL; }

KY_INLINE const NavMeshElementBlob* NavMeshElement::GetNavMeshElementBlob() const
{
	KY_DEBUG_ASSERTN(IsStillLoaded(),("You Cannot call this function if NavMeshElement has been unloaded or not loaded yet"));
	return m_navMeshElementBlob;
}

KY_INLINE const GuidCompound& NavMeshElement::GetGuidCompound()      const
{
	KY_DEBUG_ASSERTN(IsStillLoaded(),("You Cannot call this function if NavMeshElement has been unloaded or not loaded yet"));
	return m_navMeshElementBlob->m_guidCompound;
}
KY_INLINE KyUInt32            NavMeshElement::GetIndexInCollection() const { return m_indexInCollection; }
KY_INLINE void                NavMeshElement::SetIndexInCollection(KyUInt32 indexInCollection) { m_indexInCollection = indexInCollection; }

KY_INLINE KyUInt32 NavMeshElement::ComputeSizeForNavMeshElement(const NavMeshElementBlob& navMeshElementBlob)
{
	return sizeof(NavMeshElement) + navMeshElementBlob.GetNavCellBlobCount() * (KyUInt32)sizeof(void*);
}
}

#endif //Navigation_NavMeshElement_H

