/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_RasterTriangle_H
#define GwNavGen_RasterTriangle_H

#include "gwnavruntime/base/endianness.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavgeneration/raster/rasterpoint.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/basesystem/intcoordsystem.h"


namespace Kaim
{

class Triangle3i;


class RasterTriangle
{
public:
	RasterTriangle();

	bool Setup(const Triangle3i& triangle, KyFloat32 rasterPrecisionInMeter);

	bool Setup(const RasterPoint& rpA, const RasterPoint& rpB, const RasterPoint& rpC,  KyFloat32 rasterPrecisionInMeter);
	
	KY_INLINE KyFloat32 meterZ(Coord x, Coord y)
	{
		KyFloat32 meter_AMx = IntCoordSystem::IntToNavigation_Dist(x - iA.x, m_rasterPrecisionInMeter);
		KyFloat32 meter_AMy = IntCoordSystem::IntToNavigation_Dist(y - iA.y, m_rasterPrecisionInMeter);
		return A.z - (meter_AMx * N.x + meter_AMy * N.y) * inv_Nz;
	}

	KyFloat32 meterStepZ_AlongX(Coord dx) { return dz_on_dx * IntCoordSystem::IntToNavigation_Dist(dx, m_rasterPrecisionInMeter); }

	/* Returns true if M is inside triangle or exactly on one edge. */
	bool IsInside2d(const Vec2i& M) const { return M.IsInsideTriangle(iA, iB, iC); }

	/* Returns true if M is inside triangle or exactly on one edge. We assume the triangle is not colinear in this test. */
	bool IsInside2d_NoColinear(const Vec2i& M) const { return M.IsInsideNotColinearTriangle(iA, iB, iC); }

	/* Returns true if M is strictly inside triangle. */
	bool IsStrictlyInside2d(const Vec2i& M) const { return M.IsStrictlyInsideTriangle(iA, iB, iC); }

public:
	Vec2i iA, iB, iC;

	Vec3f A;
	Vec3f AB;
	Vec3f AC;
	Vec3f N;
	KyFloat32 inv_Nz;
	KyFloat32 dz_on_dx;
	KyFloat32 m_rasterPrecisionInMeter;
};


}


#endif
