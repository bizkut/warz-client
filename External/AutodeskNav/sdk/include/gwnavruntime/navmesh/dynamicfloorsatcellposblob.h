/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_DynamicFloorsAtCellPosBlob_H
#define Navigation_DynamicFloorsAtCellPosBlob_H

#include "gwnavruntime/navmesh/blobs/navfloorblob.h"
#include "gwnavruntime/blob/blobref.h"
#include "gwnavruntime/base/guidcompound.h"
#include "gwnavruntime/world/runtimeblobcategory.h"

namespace Kaim
{
class NavCell;

class DynamicNavFloorsInCellBlob
{
	KY_CLASS_WITHOUT_COPY(DynamicNavFloorsInCellBlob)
public:
	DynamicNavFloorsInCellBlob() {}

public:
	GuidCompound m_navMeshGuidCompound;
	NavCellIdx m_cellIdxInNavMesh;
	BlobArray<BlobRef<NavFloorBlob> > m_floors;
};

// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.
inline void SwapEndianness(Endianness::Target e, DynamicNavFloorsInCellBlob& self)
{
	SwapEndianness(e, self.m_navMeshGuidCompound);
	SwapEndianness(e, self.m_cellIdxInNavMesh);
	SwapEndianness(e, self.m_floors);
}


class DynamicNavFloorsBlobBuilder : public BaseBlobBuilder<DynamicNavFloorsInCellBlob>
{
public:
	DynamicNavFloorsBlobBuilder(NavCell* navCell) : m_navCell(navCell) {}

private:
	virtual void DoBuild();

private:
	NavCell* m_navCell;
};


// Creates a DynamicNavFloorsInCellBlob to remove in a database all dynamic navcells registered in another DynamicNavFloorsInCellBlob.
// INTERNAL: Used for the visualDebugging of dynamic NavMesh in Lab.
// Implementation: Creates a DynamicNavFloorsInCellBlob from another one, and forces all navcells to be empty.
class DynamicNavFloorsRemoverBlobBuilder : public BaseBlobBuilder<DynamicNavFloorsInCellBlob>
{
public:
	DynamicNavFloorsRemoverBlobBuilder(const DynamicNavFloorsInCellBlob* dynamicNavFloorsToRemove) : m_dynamicNavFloorsToRemove(dynamicNavFloorsToRemove) {}

private:
	virtual void DoBuild();

private:
	const DynamicNavFloorsInCellBlob* m_dynamicNavFloorsToRemove;
};


// The NavCellBlob contains the static data for a NavCell: a single cell within the grid of cells maintained by a NavMesh.  
class DynamicFloorsAtCellPosBlob
{
	KY_ROOT_BLOB_CLASS(Runtime, DynamicFloorsAtCellPosBlob, 0)
	KY_CLASS_WITHOUT_COPY(DynamicFloorsAtCellPosBlob)

public:
	
	DynamicFloorsAtCellPosBlob() {}

public:
	KyUInt32 m_databaseIndex;
	CellPos m_cellPos;
	BlobArray<DynamicNavFloorsInCellBlob > m_dynamicNavFloorsInCellBlobs; 
};

// Swaps the endianness of the data in the specified object. This function allows this type of object to be serialized through the blob framework.
// The first parameter specifies the target endianness, and the second parameter provides the object whose data is to be swapped.
inline void SwapEndianness(Endianness::Target e, DynamicFloorsAtCellPosBlob& self)
{
	SwapEndianness(e, self.m_databaseIndex);
	SwapEndianness(e, self.m_cellPos);
	SwapEndianness(e, self.m_dynamicNavFloorsInCellBlobs);
}


class DynamicFloorsAtCellPosBuilder : public BaseBlobBuilder<DynamicFloorsAtCellPosBlob>
{
public:
	DynamicFloorsAtCellPosBuilder(NavCell** navCells, KyUInt32 navCellCount, KyUInt32 databaseIndex) :
		m_navCells(navCells), m_navCellCount(navCellCount), m_databaseIndex(databaseIndex) {}

private:
	virtual void DoBuild();

private:
	NavCell** m_navCells;
	KyUInt32 m_navCellCount;
	KyUInt32 m_databaseIndex;
};



class DynamicFloorsAtCellPosRemoverBlobBuilder : public BaseBlobBuilder<DynamicFloorsAtCellPosBlob>
{
public:
	DynamicFloorsAtCellPosRemoverBlobBuilder(const DynamicFloorsAtCellPosBlob* src) : m_src(src) {}

private:
	virtual void DoBuild();

private:
	const DynamicFloorsAtCellPosBlob* m_src;
};


}

#endif //Navigation_DynamicFloorsAtCellPosBlob_H

