/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: LASI - secondary contact: NONE
#ifndef GwNavGen_DynamicNavFloorHeightField_H
#define GwNavGen_DynamicNavFloorHeightField_H


#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavgeneration/common/generatormemory.h"
#include "gwnavgeneration/containers/tlsarray.h"

namespace Kaim
{

class GeneratorSystem;
class DynamicRasterCell;


enum NavFloorHeightFieldErrorUpdateStatus
{
	NavFloorHeightFieldErrorStatus_Updated,
	NavFloorHeightFieldErrorStatus_NoProjections
};

enum NavFloorHeightFieldPointStatus
{
	NavFloorHeightFieldPointStatus_Valid,
	NavFloorHeightFieldPointStatus_Disabled
};

struct NavFloorHeightFieldPoint
{
	NavFloorHeightFieldPoint()
		: m_pos(0.0f,0.0f),
		  m_minLocalAlt(KyFloat32MAXVAL),
		  m_maxLocalAlt(-KyFloat32MAXVAL),
		  m_error(0.0f),
		  m_status(NavFloorHeightFieldPointStatus_Valid) {}

	NavFloorHeightFieldPoint(const Vec2f& position2D, KyFloat32 alt)
		: m_pos(position2D),
		  m_minLocalAlt(alt),
		  m_maxLocalAlt(alt),
		  m_error(0.0f),
		  m_status(NavFloorHeightFieldPointStatus_Valid){}

	bool operator == (const NavFloorHeightFieldPoint& other)
	{
		// for pos & alts are enough
		bool same = other.m_pos == m_pos
				 && other.m_maxLocalAlt == m_maxLocalAlt
				 && other.m_minLocalAlt == m_minLocalAlt;

		return same;
	}

	Vec2f m_pos;
	KyFloat32 m_minLocalAlt;
	KyFloat32 m_maxLocalAlt;

	KyFloat32 m_error;
	NavFloorHeightFieldPointStatus m_status;
};

// --------------------------------------
// ----- DynamicNavFloorHeightField -----
// --------------------------------------
class DynamicNavFloorHeightField
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	DynamicNavFloorHeightField(GeneratorSystem* sys, KyInt32 originalPixelSize, KyInt32 stride, const PixelBox& navPixelBox);

	void AddPoint(const PixelPos& coord, KyFloat32 alt);
	NavFloorHeightFieldErrorUpdateStatus UpdateErrors(const DynamicRasterCell* rasterCell);
	NavFloorHeightFieldPoint* GetWorstErrorPoint();
	NavFloorHeightFieldPoint* GetPointAtLocalHeightFieldPos(const PixelPos& pos);
	NavFloorHeightFieldPoint* GetPointAtAbsoluteHeightFieldPos(const PixelPos& pos);

public:
	GeneratorSystem* m_sys;
	KyInt32 m_originalPixelSize; // must be >0
	KyInt32 m_stride; // must be >0
	PixelBox m_navPixelBox; // from the raster (exclusive + 1 pixel overlap)
	PixelBox m_heightFieldPixelBox; // same but with pixelSize = m_stride * m_sys->Partition().m_pixelSize
	PixelBox m_heightFieldBoundingPixelBox; // updated when adding new point
	KyArrayTLS<NavFloorHeightFieldPoint> m_points;
	KyArrayTLS_POD<KyUInt32> m_grid;
};

}


#endif //GwNavGen_DynamicNavFloor_H

