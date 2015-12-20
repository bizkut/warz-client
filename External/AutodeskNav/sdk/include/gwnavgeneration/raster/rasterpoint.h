/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_RasterPoint_H
#define GwNavGen_RasterPoint_H



#include "gwnavruntime/math/vec2i.h"
#include "gwnavruntime/math/vec3i.h"

namespace Kaim
{


class RasterPoint : public Vec2i
{
public:
	RasterPoint() : meter_z(0.0f) {}

	void Set(const Vec2i& v, KyFloat32 _meter_z)
	{
		x = v.x;
		y = v.y;
		meter_z = _meter_z;
	}

public:
	KyFloat32 meter_z;
};


}


#endif
