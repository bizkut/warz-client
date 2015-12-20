/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// primary contact: LASI - secondary contact: NOBODY
#ifndef Navigation_BoxContour_H
#define Navigation_BoxContour_H

#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/math/vec2f.h"


namespace Kaim
{

class Transform;
class Box3f;

namespace BoxContour
{
	KyResult ComputeHexagonalContour_2d(const Transform& transform, const Box3f& localBox, KyArray<Vec2f>& contour, KyFloat32 mergePointDistance);
	KyResult ComputeHexagonalContour(const Transform& transform, const Box3f& localBox, KyArray<Vec2f>& contour, KyFloat32& altMin, KyFloat32& altMax, KyFloat32 mergePointDistance);

	void Init4PointsContour(const Vec2f& center, const Vec2f& v1, const Vec2f& v2, KyArray<Vec2f>& contour);
	void Init6PointsContour(const Vec2f& center, const Vec2f& v1, const Vec2f& v2, const Vec2f& v3, KyArray<Vec2f>& contour);

} //namespace BoxContour

} //namespace Kaim

#endif // Navigation_BoxContour_H
