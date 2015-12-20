/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_NavFloorBoxBlob_H
#define Navigation_NavFloorBoxBlob_H

#include "gwnavruntime/navmesh/navfloor.h"
#include "gwnavruntime/navmesh/blobs/navfloorblob.h"
#include "gwnavruntime/database/database.h"
#include "gwnavruntime/navmesh/identifiers/navfloorptr.h"

namespace Kaim
{

class NavFloorPtr;

/// Blob to visual debug NavFloor information.
class NavFloorBoxBlob
{
public:
	enum ValidityStatus
	{
		VALID,
		INVALID
	};

	NavFloorBoxBlob() : m_status((KyUInt16)INVALID)
	{}

	ValidityStatus GetValidityStatus() const { return (ValidityStatus)m_status; }

	Box3f m_floorAABB;
	CellPos m_cellPos; ///< The position of the cell that contains this floor within the grid of PathData cells.
	KyUInt16 m_status;
};

inline void SwapEndianness(Kaim::Endianness::Target e, NavFloorBoxBlob& self)
{
	SwapEndianness(e, self.m_floorAABB);
	SwapEndianness(e, self.m_cellPos);
	SwapEndianness(e, self.m_status);
}


class NavFloorBoxBlobBuilder : public BaseBlobBuilder<NavFloorBoxBlob>
{
public:
	NavFloorBoxBlobBuilder(const NavFloorPtr& floorPtr) : m_floorPtr(floorPtr) {}
private:
	void DoBuild()
	{
		if (IsWriteMode())
		{
			if (m_floorPtr.IsValid() == false)
			{
				m_blob->m_status = (KyUInt16)NavFloorBoxBlob::INVALID;
				return;
			}

			NavFloor* navFloor = m_floorPtr.GetNavFloor_Unsafe();
			const NavFloorBlob* navFloorBlob = navFloor->GetNavFloorBlob();

			const DatabaseGenMetrics& databaseGenMetrics = navFloor->m_database->GetDatabaseGenMetrics();


			const CoordPos64 cellOrigin64(databaseGenMetrics.ComputeCellOrigin(navFloor->GetCellPos()));
			const Vec2f cellOrigin(databaseGenMetrics.GetFloatValueFromInteger64(cellOrigin64.x), databaseGenMetrics.GetFloatValueFromInteger64(cellOrigin64.y));

			m_blob->m_floorAABB.m_min.x = cellOrigin.x + databaseGenMetrics.GetFloatValueFromInteger32(navFloorBlob->m_navFloorAABB.m_min.GetX32());
			m_blob->m_floorAABB.m_max.x = cellOrigin.x + databaseGenMetrics.GetFloatValueFromInteger32(navFloorBlob->m_navFloorAABB.m_max.GetX32());

			m_blob->m_floorAABB.m_min.y = cellOrigin.y + databaseGenMetrics.GetFloatValueFromInteger32(navFloorBlob->m_navFloorAABB.m_min.GetY32());
			m_blob->m_floorAABB.m_max.y = cellOrigin.y + databaseGenMetrics.GetFloatValueFromInteger32(navFloorBlob->m_navFloorAABB.m_max.GetY32());

			m_blob->m_floorAABB.m_min.z = navFloor->m_floorAltitudeRange.m_minZ;
			m_blob->m_floorAABB.m_max.z = navFloor->m_floorAltitudeRange.m_maxZ;


			m_blob->m_cellPos = navFloor->GetCellPos();
			m_blob->m_status = (KyUInt16)NavFloorBoxBlob::VALID;

		}
	}

	NavFloorPtr m_floorPtr;
};

}

#endif // Navigation_NavFloorBoxBlob_H
