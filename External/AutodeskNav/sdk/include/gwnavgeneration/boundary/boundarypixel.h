/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: GUAL - secondary contact: LASI
#ifndef GwNavGen_BoundaryPixel_H
#define GwNavGen_BoundaryPixel_H


#include "gwnavgeneration/boundary/boundarytypes.h"
#include "gwnavgeneration/navraster/navrastercell.h"


namespace Kaim
{

class BoundaryEdge;

/* There is one BoundaryPixel for each NavRasterPixel. */
class BoundaryPixel
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	BoundaryPixel();

	void Init(const NavPixelPos& pos, NavRasterFloorIdx floorIdx, const NavRasterPixel* navRasterPixel, bool isOutside);
	KyUInt32 GetFeatureMask(CardinalDir dir) const;
public:
	NavPixelPos3d m_navPixelPos3d;
	const NavRasterPixel* m_navRasterPixel;
	PixelColor m_floorColor;
	KyUInt32 m_connexIdx;
	bool m_isOutside;

	// +----1=N---+ 
	// |    |     | 
	// 2=W-- ----0=E
	// |    |     |
	// +----3=S---+
	BoundaryEdge* m_edges[4];
};

KY_INLINE BoundaryPixel::BoundaryPixel() : m_navRasterPixel(KY_NULL), m_floorColor(PixelColor_Unset), m_connexIdx(KyUInt32MAXVAL), m_isOutside(false)
{
	for(CardinalDir dir = 0; dir < 4; ++dir)
		m_edges[dir] = KY_NULL;
}

KY_INLINE void BoundaryPixel::Init(const NavPixelPos& pos, NavRasterFloorIdx floorIdx, const NavRasterPixel* navRasterPixel, bool isOutside)
{
	m_navPixelPos3d.Set(pos.x, pos.y, floorIdx);
	m_navRasterPixel = navRasterPixel;
	m_floorColor = navRasterPixel->m_floorColor;
	m_connexIdx = navRasterPixel->m_connexIdx;
	m_isOutside = isOutside;
}

KY_INLINE KyUInt32 BoundaryPixel::GetFeatureMask(CardinalDir dir) const
{
	KY_ASSERT(m_navRasterPixel != KY_NULL);
	return m_navRasterPixel->GetFeatureMask(dir);
}

}


#endif
