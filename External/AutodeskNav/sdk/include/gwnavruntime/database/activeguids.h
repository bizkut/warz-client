/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_ActiveGuids_H
#define Navigation_ActiveGuids_H

#include "gwnavruntime/base/guidcompound.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"

namespace Kaim
{

class NavMeshElement;

class ActiveGuids
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	void AddActiveKyGuid(const KyGuid& kyGuid);
	void RemoveActiveKyGuid(const KyGuid& kyGuid);

	//activatedGuid must be sorted
	KY_INLINE bool IsGuidCompoundCompatible(const GuidCompound& guidCompound) const
	{
		if (guidCompound.GetKyGuidCount() == 1)
			// we assume that all the GuidCompound with only one KyGuid are compatible since they are from a "main" sector
			return true;

		return IsNotMainGuidCompoundCompatible(guidCompound);
	}

	void Clear() { m_activeGuids.ClearAndRelease(); }

private :
	bool IsNotMainGuidCompoundCompatible(const GuidCompound& guidCompound) const;

public:
	KyArray<KyGuid, MemStat_NavData> m_activeGuids;
};

class GuidCompoundSorter
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_WorldFwk)
	KY_CLASS_WITHOUT_COPY(GuidCompoundSorter)

public:
	GuidCompoundSorter() {}
	class GuidCompoundSorterElement
	{
		KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_WorldFwk)
	public:
		GuidCompoundSorterElement() : m_guidCompound(KY_NULL), m_navMeshElement(KY_NULL) {}
		GuidCompoundSorterElement(NavMeshElement* navMeshElement, const GuidCompound* guidCompound) : m_guidCompound(guidCompound), m_navMeshElement(navMeshElement) {}

		const GuidCompound* m_guidCompound;
		NavMeshElement* m_navMeshElement;
	};

	void AddNavMeshElement(NavMeshElement* navMeshElement);
	void RemoveNavMeshElement(NavMeshElement* navMnavMeshElementesh);
	void Sort(ActiveGuids& activeGuids, CellBox& currentUpdateCellBox, bool forceActiveIfAlone = false);
public:

	KyArray<GuidCompoundSorterElement, MemStat_NavData> m_guidCompoundArray;
};

}


#endif //Navigation_ActiveGuids_H

