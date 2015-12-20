/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_DynamicRasterColumnToAltitudes_H
#define GwNavGen_DynamicRasterColumnToAltitudes_H


#include "gwnavgeneration/raster/dynamicrastercolumn.h"
#include "gwnavgeneration/containers/tlsarray.h"


namespace Kaim
{


class DynamicRasterColumnToAltitudes
{
public:
	DynamicRasterColumnToAltitudes(KyFloat32 meterMergeDistance, KyUInt32 elementsMaxCount = 10000);

	~DynamicRasterColumnToAltitudes();

	void ColumnToAltitudes(const DynamicRasterColumn* column, KyFloat32 navTagMergeTolerance, const KyArrayTLS_POD<const NavTag*>& navTagArray);

	KyUInt32   GetAltitudesCount() { return m_altitudesCount;                                }
	KyFloat32* GetAltitudes()      { return (m_altitudesCount != 0) ? m_altitudes : KY_NULL; }

	KyUInt32  GetNavTagCount()  { return m_navTagCount;                                     }
	KyUInt32* GetNavTagIdices() { return (m_navTagCount != 0) ? m_navTagIndices : KY_NULL; }

	KyUInt32 GetMaxOwnerCountInColumn() const { return m_maxOwnerCountInMergedElements; }

private:
	void AddAltitude(KyFloat32 altitude)
	{
		KY_DEBUG_ASSERTN(m_altitudesCount < m_altitudesMaxCount, ("Max altitudeCount reached"));
		m_altitudes[m_altitudesCount] = altitude;
		++m_altitudesCount;
	}

	void AddNavTagIdx(KyUInt32 navTagIdx)
	{
		KY_DEBUG_ASSERTN(m_navTagCount < m_navTagMaxCount, ("Max NavTag count reached"));
		m_navTagIndices[m_navTagCount] = navTagIdx;
		++m_navTagCount;
	}

	void UpdateOwnerList(KyUInt32 newOwner)
	{
		KyUInt32 newOwnerIdx = 0;
		for (; newOwnerIdx < m_ownersCount; ++newOwnerIdx)
		{
			if (m_owners[newOwnerIdx] == newOwner)
				return;
		}
		KY_DEBUG_ASSERTN(newOwnerIdx < m_ownersMaxCount, ("Max owners count reached"));
		m_owners[m_ownersCount++] = newOwner;
	}

	void ResetOwnerList()
	{
		m_maxOwnerCountInMergedElements = Kaim::Max(m_maxOwnerCountInMergedElements, m_ownersCount);
		m_ownersCount = 0;
	}

private:
	KyFloat32 m_meterMergeDistance;

	KyUInt32 m_sortedElementsMaxCount;
	KyUInt32 m_sortedElementsCount;
	DynamicRasterPixel* m_sortedElements;

	KyUInt32 m_altitudesMaxCount;
	KyUInt32 m_altitudesCount;
	KyFloat32* m_altitudes;

	KyUInt32  m_navTagMaxCount;
	KyUInt32  m_navTagCount;
	KyUInt32* m_navTagIndices;

	KyUInt32  m_ownersMaxCount;
	KyUInt32  m_ownersCount;
	KyUInt32* m_owners;
	KyUInt32  m_maxOwnerCountInMergedElements;
};


}


#endif
