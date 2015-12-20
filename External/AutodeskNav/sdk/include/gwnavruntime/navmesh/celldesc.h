/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_CellDesc_H
#define Navigation_CellDesc_H


#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/math/box2f.h"


namespace Kaim
{

class PixelAndCellGrid;


class CellDesc
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	void Init(const PixelAndCellGrid& pixelAndCellGrid, const CellPos& cellPos, KyFloat32 rasterPrecision);
	bool operator==(const CellDesc& other) const;

public:
	CellPos m_cellPos;
	PixelBox m_exclusivePixelBox;
	PixelBox m_enlargedPixelBox;
	CoordBox m_exclusiveCoordBox;
	CoordBox m_enlargedCoordBox;
	Box2f m_box2f;
};
inline void SwapEndianness(Endianness::Target e, CellDesc& self)
{
	SwapEndianness(e, self.m_cellPos);
	SwapEndianness(e, self.m_exclusivePixelBox);
	SwapEndianness(e, self.m_enlargedPixelBox);
	SwapEndianness(e, self.m_exclusiveCoordBox);
	SwapEndianness(e, self.m_enlargedCoordBox);
	SwapEndianness(e, self.m_box2f);
}


}


#endif
