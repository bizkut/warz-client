/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: BEHE - secondary contact: GUAL
#ifndef GwNavGen_DynamicRasterCellDilatator_H
#define GwNavGen_DynamicRasterCellDilatator_H


#include "gwnavruntime/navmesh/navmeshtypes.h"


namespace Kaim
{

class DynamicRasterCell;

/*
All the interval in the final raster will be dilated vertically by halfHeight up and down,
and laterally by radius pixels.
radius*m_sys->Partition().m_pixelSize
NB1: Must be called BEFORE ReleaseDynamicColumns !
NB2: ReleaseDynamicColumns is called before function exit
TODO: Realloc dynamic collumn if ReleaseDynamicColumns was called
typical usage:
	DynamicRasterCell copy = original;
	copy.Dilate(x);
*/
class DynamicRasterCellDilatator
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	DynamicRasterCellDilatator(const DynamicRasterCell* input, PixelCoord radius, KyFloat32 halfHeight) : m_input(input), m_radius(radius), m_halfHeight(halfHeight) {}

	void Dilate(DynamicRasterCell& dilated) const;

protected:
	const DynamicRasterCell* m_input;
	PixelCoord m_radius;
	KyFloat32 m_halfHeight;
};


} // namespace Kaim


#endif
