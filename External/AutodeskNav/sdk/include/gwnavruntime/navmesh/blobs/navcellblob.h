/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_NavCellBlob_H
#define Navigation_NavCellBlob_H

#include "gwnavruntime/navmesh/blobs/navfloorblob.h"
#include "gwnavruntime/navmesh/blobs/stitch1To1datablob.h"

namespace Kaim
{

/// The NavCellBlob contains the NavMesh static data of a NavMeshElement at a CellPos.
/// It gathers a set of NavFloorBlob and some stitching helpers (NavFloorLinkInfo).
class NavCellBlob
{
	KY_ROOT_BLOB_CLASS(NavData, NavCellBlob, 1)
	KY_CLASS_WITHOUT_COPY(NavCellBlob)
public:
	NavCellBlob() {}

	
	const CellPos&      GetCellPos()                     const; ///< Retrieves the position of this NavCellBlob within the partitioning scheme of its NavMeshElementBlob. 
	KyUInt32            GetNavFloorBlobCount()           const; ///< Retrieves the number of NavFloorBlob maintained by this NavCellBlob. 
	const NavFloorBlob& GetNavFloorBlob(NavFloorIdx idx) const; ///< Retrieves the NavFloorBlob with the specified index. 

public: // Internal
	bool IsValid() const; ///< performs some basic tests on static data. For internal debug purpose.

public:
	CellPos m_cellPos;                                       ///< The position of this NavCellBlob within the partitioning scheme of its NavMeshElementBlob. Do not modify.
	// for those 3 blobArray, count == floorCount. 
	BlobArray<NavFloor1To1StitchDataBlob> m_floor1To1StitchData; ///< The list of NavFloor1To1StitchData maintained by the NavCellBlob. Do not modify.
	BlobArray<BlobRef<NavFloorBlob> > m_floors;              ///< The list of NavFloorBlobs maintained by the NavCellBlob. Do not modify.
};

/// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
/// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.
inline void SwapEndianness(Endianness::Target e, NavCellBlob& self)
{
	SwapEndianness(e, self.m_cellPos);
	SwapEndianness(e, self.m_floor1To1StitchData);
	SwapEndianness(e, self.m_floors);
}

KY_INLINE KyUInt32            NavCellBlob::GetNavFloorBlobCount() const { return m_floors.GetCount();  }
KY_INLINE const CellPos&      NavCellBlob::GetCellPos()           const { return m_cellPos;            }
KY_INLINE const NavFloorBlob& NavCellBlob::GetNavFloorBlob(NavFloorIdx idx) const
{
	KY_DEBUG_ASSERTN(idx < GetNavFloorBlobCount(), ("Invalid NavFloorBlob index"));
	return *m_floors.GetValues()[idx].Ptr();
}

}


#endif //Navigation_NavCellBlob_H

