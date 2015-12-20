/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LASI - secondary contact: GUAL
#ifndef GwNavGen_NavRasterCell_H
#define GwNavGen_NavRasterCell_H


#include "gwnavruntime/base/endianness.h"
#include "gwnavruntime/basesystem/coordsystem.h"
#include "gwnavruntime/navmesh/pixelandcellgrid.h"
#include "gwnavruntime/blob/blobarray.h"
#include "gwnavruntime/math/triangle3i.h"
#include "gwnavruntime/navmesh/celldesc.h"
#include "gwnavruntime/containers/stringstream.h"
#include "gwnavgeneration/generator/generatorblobtyperegistry.h"
#include "gwnavgeneration/navraster/navrastertypes.h"

namespace Kaim
{
class NavRasterPixel
{
public:
	NavRasterPixel();

	KyUInt32 GetFeatureMask(CardinalDir dir)        const;
	KyUInt32 GetNeighborFloorIndex(CardinalDir dir) const;

public:
	KyFloat32 m_altitude;
	PixelColor m_floorColor;
	KyUInt32 m_connexIdx;
	KyUInt32 m_connectedComponentIdx;
	KyUInt32 m_navTagIdx;
	KyUInt32 m_featureBitField; // among NavRasterFeatureMask enum values

	// connections are indexed as follow:
	//  +-----------+
	//  |     1     |
	//  |           |
	//  |2    +    0|
	//  |           |
	//  |     3     |
	//  +-----------+
	//0 corresponds to East (+1;0) and index increases CCW
	//Stores level index of neighbor position if reachable, NavRaster_NoConnection otherwise
	NavRasterFloorIdx m_neighborFloorIdx[4];

	KyUInt32 m_currentPropagationId;
};
inline void SwapEndianness(Endianness::Target e, NavRasterPixel& self)
{
	SwapEndianness(e, self.m_altitude);
	SwapEndianness(e, self.m_floorColor);
	SwapEndianness(e, self.m_connexIdx);
	SwapEndianness(e, self.m_navTagIdx);
	SwapEndianness(e, self.m_connectedComponentIdx);
	SwapEndianness(e, self.m_neighborFloorIdx[0]);
	SwapEndianness(e, self.m_neighborFloorIdx[1]);
	SwapEndianness(e, self.m_neighborFloorIdx[2]);
	SwapEndianness(e, self.m_neighborFloorIdx[3]);
	SwapEndianness(e, self.m_currentPropagationId);
}

class NavRasterColumn
{
	KY_CLASS_WITHOUT_COPY(NavRasterColumn);
public:
	NavRasterColumn() {}

	BlobArray<NavRasterPixel> m_navRasterPoints;
};
inline void SwapEndianness(Endianness::Target e, NavRasterColumn& self)
{
	SwapEndianness(e, self.m_navRasterPoints);
}


class NavRasterCell
{
	KY_ROOT_BLOB_CLASS(Generator, NavRasterCell, 1)
public:
	KyFloat32 m_rasterPrecision;
	PixelAndCellGrid m_pixelAndCellGrid;
	CellDesc m_cellDesc;
	BlobArray<NavRasterColumn> m_columns; // row major
};
inline void SwapEndianness(Endianness::Target e, NavRasterCell& self)
{
	SwapEndianness(e, self.m_rasterPrecision);
	SwapEndianness(e, self.m_pixelAndCellGrid);
	SwapEndianness(e, self.m_cellDesc);
	SwapEndianness(e, self.m_columns);
}

KY_INLINE NavRasterPixel::NavRasterPixel() :
	m_altitude(0.f),
	m_floorColor(PixelColor_Unset),
	m_connexIdx(KyUInt32MAXVAL),
	m_connectedComponentIdx(KyUInt32MAXVAL),
	m_navTagIdx(KyUInt32MAXVAL),
	m_featureBitField(0),
	m_currentPropagationId(KyUInt32MAXVAL)
{
	for(KyUInt32 i=0; i<4; ++i)
		m_neighborFloorIdx[i] = NavRasterFloorIdx_Invalid;
}

KY_INLINE KyUInt32 NavRasterPixel::GetNeighborFloorIndex(CardinalDir dir) const { return m_neighborFloorIdx[dir]; }
KY_INLINE KyUInt32 NavRasterPixel::GetFeatureMask(CardinalDir dir) const
{
/*
	const KyUInt32 bitshift = dir*8;
	return ((0xFF << bitshift) & m_featureBitField) >> bitshift;*/
	switch (dir)
	{
	case 0:
		{
			const KyUInt32 bitshift = 0;
			return ((0xFF << bitshift) & m_featureBitField) >> bitshift;
		}
	case 1:
		{
			const KyUInt32 bitshift = 8;
			return ((0xFF << bitshift) & m_featureBitField) >> bitshift;
		}
	case 2:
		{
			const KyUInt32 bitshift = 16;
			return ((0xFF << bitshift) & m_featureBitField) >> bitshift;
		}
	case 3:
		{
			const KyUInt32 bitshift = 24;
			return ((0xFF << bitshift) & m_featureBitField) >> bitshift;
		}
	default:
		return KyUInt32MAXVAL;
	}
}

}


#endif

