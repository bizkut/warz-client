/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LASI - secondary contact: NOBODY

// This class is meant to speed-up the evaluation of IsInside() for each pixel in a raster
// by pre-computing some early exit conditions. The benefits of this class regarding performance are
// critical as IsPixelPos() is called once for each pixel of the raster (~10 000 times / cell) and for 
// each coloration volume
// Call to TagVolume::IsInside() can be very costly (especially when the contour has many vertices) 
// and therefore need to be avoided as much as possible, hence this class. 

#ifndef GwNavGen_RasterCellTagVolume_H
#define GwNavGen_RasterCellTagVolume_H

#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/world/tagvolumeblob.h"
#include "gwnavruntime/math/geometryfunctions.h"
#include "gwnavgeneration/input/taggedtriangle3i.h"

namespace Kaim
{

class DynamicRasterCell;
class GeneratorSystem;

class RasterCellTagVolume
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	RasterCellTagVolume();

	void Initialize(const TagVolumeBlob& volumeBlob, const DynamicRasterCell* cell);

	bool IsPixelPosInVolume(const Vec3f& pixelPos) const;

	KyUInt32 GetNavTagIdx() const { return m_tagVolume->m_navTagIdx; }

private: 
	// compute the value of the 3 bools used to speedup IsPixelPosInVolume()
	// automatically called in Ctor()
	void ComputeStatus();

	bool TestIntersectionWithTagVolume(const Vec3f* corners);
	KY_INLINE bool IsInsideTagVolume2d(const Vec2f& position) const { return GeometryFunctions::IsInside2d_Polyline(position, m_tagVolume->m_points.GetValues(), m_tagVolume->m_points.GetCount()); }
	KY_INLINE bool IsInsideTagVolume3d(const Vec3f& position) const { return (position.z <= m_tagVolume->m_aabb.m_max.z) && (position.z >= m_tagVolume->m_aabb.m_min.z) && IsInsideTagVolume2d(position.Get2d()); }

private:
	const TagVolumeBlob* m_tagVolume;
	const DynamicRasterCell* m_cell;
	KyInt32 m_pixelSize;
	KyFloat32 m_rasterPrecision;
	PixelBox m_pixelBox;

	bool m_cellFullyContained;
	bool m_cellFullyOutside;
};


} // namespace Kaim

#endif //GwNavGen_RasterCellTagVolume_H
