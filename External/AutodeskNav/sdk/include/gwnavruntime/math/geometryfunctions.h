/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_GeometryFunctions_H
#define Navigation_GeometryFunctions_H

#include "gwnavruntime/navmesh/navmeshtypes.h"
#include "gwnavruntime/containers/kyarray.h"

namespace Kaim
{
class Vec2f;
class Vec2i;
namespace GeometryFunctions
{
// Tests whether or not a specified point is inside the area defined by a polyline in two 
// dimensions, ignoring their altitude.
// \param p					The point to test.
// \param points			The list of points in the polyline. The list must also be closed: its last point must be the
// 							same as its first point.
// \param count				The number of points in the points array.
// \return true if the point is inside the polyline (including borders by default, depending on insideIfOnBorder parameter) or false otherwise.
bool IsInside2d_Polyline(const Vec2f& p, const Vec2f* points, KyUInt32 count, bool* OnBorder = KY_NULL);
bool IsInside2d_Polyline(const Vec2i& p, const Vec2i* points, KyUInt32 count, bool* OnBorder = KY_NULL);

PolygonWinding ComputePolygonWinding(const Vec2i* polygon, KyUInt32 count);
PolygonWinding ComputePolygonWinding(const Vec2f* polygon, KyUInt32 count);
inline PolygonWinding ComputePolygonWinding(const KyArrayPOD<Vec2i>& polygon)
{
	return ComputePolygonWinding(polygon.GetDataPtr(), polygon.GetCount());
}


bool IsTwisted(const Vec2f* polygon, KyUInt32 count);

} //namespace GeometryFunctions


} //namespace Kaim

#endif // Navigation_GeometryFunctions_H
