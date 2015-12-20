/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_RasterCell_H
#define GwNavGen_RasterCell_H


#include "gwnavruntime/base/endianness.h"
#include "gwnavruntime/basesystem/coordsystem.h"
#include "gwnavruntime/navmesh/pixelandcellgrid.h"
#include "gwnavruntime/navmesh/celldesc.h"
#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/containers/stringstream.h"
#include "gwnavgeneration/generator/generatorblobtyperegistry.h"
#include "gwnavruntime/database/navtag.h"


namespace Kaim
{


class RasterColumn
{
	KY_CLASS_WITHOUT_COPY(RasterColumn)
public:
	RasterColumn() {}
public:
	BlobArray<KyFloat32> m_altitudes;
	BlobArray<KyUInt32> m_navTagIndices;
};
inline void SwapEndianness(Endianness::Target e, RasterColumn& self)
{
	SwapEndianness(e, self.m_altitudes);
	SwapEndianness(e, self.m_navTagIndices);
}


class RasterCell
{
	KY_CLASS_WITHOUT_COPY(RasterCell)
	KY_ROOT_BLOB_CLASS(Generator, RasterCell, 0)
public:
	RasterCell() {}

public:
	KyFloat32 m_rasterPrecisionInMeters;
	PixelAndCellGrid m_pixelAndCellGrid;
	CellDesc m_cellDesc;
	BlobArray<RasterColumn> m_columns; // row major
	BlobArray<NavTag> m_navTags;
};
inline void SwapEndianness(Endianness::Target e, RasterCell& self)
{
	SwapEndianness(e, self.m_rasterPrecisionInMeters);
	SwapEndianness(e, self.m_pixelAndCellGrid);
	SwapEndianness(e, self.m_cellDesc);
	SwapEndianness(e, self.m_columns);
	SwapEndianness(e, self.m_navTags);
}


}


#endif

