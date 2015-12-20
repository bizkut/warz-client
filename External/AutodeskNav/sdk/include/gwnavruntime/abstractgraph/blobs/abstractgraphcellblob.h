/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_AbstractGraphCellBlob_H
#define Navigation_AbstractGraphCellBlob_H

#include "gwnavruntime/abstractgraph/blobs/abstractgraphfloorblob.h"
#include "gwnavruntime/abstractgraph/abstractgraphtypes.h"
#include "gwnavruntime/navdata/navdatablobcategory.h"
#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/blob/blobref.h"

namespace Kaim
{

class DatabaseGenMetrics;
class NavMeshGenParameters;


class AbstractGraphCellBlob
{
	KY_ROOT_BLOB_CLASS(NavData, AbstractGraphCellBlob, 0)
public:
	Vec3f GetPosition(AbstractGraphNodeIdx graphNodeIdx, AbstractGraphFloorIdx graphFloorIdx, const DatabaseGenMetrics& genMetrics) const;

public:
	CellPos m_cellPos;

	BlobArray< BlobRef<AbstractGraphFloorBlob> > m_abstractFloors;
};

inline void SwapEndianness(Endianness::Target e, AbstractGraphCellBlob& self)
{
	SwapEndianness(e, self.m_cellPos);
	SwapEndianness(e, self.m_abstractFloors);
}


}


#endif
