/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// ---------- Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_ClosestPoint_H
#define Navigation_ClosestPoint_H

#include "gwnavruntime/navmesh/navmeshtypes.h"

namespace Kaim
{

class ClosestPoint // class with static functions only
{
public :
// oneShape1VsShape2 = compute the closest point of Shape1 from Shape2

// Compute the point of the Segment [ab] the is closest from the Point p
static void OnSegmentVsPoint2d(const Vec3f& a, const Vec3f& b, const Vec3f& p, Vec3f& closestPointOnSegment, KyFloat32& squareDist2d);
static void OnSegmentVsPoint2d(const CoordPos64& a, const CoordPos64& b, const CoordPos64& p, CoordPos64& closestPointOnSegment, KyInt64& squareDist2d);

// Compute the point the Triangle [v1, v2, v3] the is closest from the Point p
static void OnTriangleVsPoint2d(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3, const Vec3f& p, Vec3f& closestPointOnTriangle, KyFloat32& squareDist2d);

// Compute the point of the Segment [v1v2] that is the closest from the Segment[ab]
static void OnSegmentVsSegmentCast2d(const Vec3f& v1, const Vec3f& v2, const Vec3f& a, const Vec3f& b,
	const KyFloat32 distCast, Vec3f& closestPointonTriangle, KyFloat32& squareDist2d);
// Compute the point the Triangle [v1, v2, v3] the is closest from the Segment [ab]
static void OnTriangleVsSegmentCast2d(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3, const Vec3f& a,
	const Vec3f& b, const KyFloat32 distCast, Vec3f& closestPointonTriangle, KyFloat32& squareDist2d);

// Compute the point the Segment [v1v2] the is closest from the Disk (center, radius)
static void OnSegmentVsDiskCast2d(const Vec3f& v1, const Vec3f& v2, const Vec3f& center, const KyFloat32 radius,
	const Vec2f& normalizedDir2D, const KyFloat32 distCast, Vec3f& closestPointonTriangle, KyFloat32& squareDist2d);
// Compute the point the Triangle [v1, v2, v3] the is closest from the Disk (center, radius)
static void OnTriangleVsDiskCast2d(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3, const Vec3f& center, const KyFloat32 radius,
	const Vec2f& normalizedDir2D, const KyFloat32 distCast, Vec3f& closestPointonTriangle, KyFloat32& squareDist2d);

// Compute the point the Segment [v1v2] the is closest from the Cone (apex, normalizedDir2D, length, halfAngleSine)
static void OnSegmentVsConeCast2d(const Vec3f& v1, const Vec3f& v2, const Vec3f& apex, const Vec2f& normalizedDir2D, KyFloat32 length,
	const KyFloat32 halfAngleSine, Vec3f& closestPointonTriangle, KyFloat32& squareDist2d);
// Compute the point the Triangle [v1, v2, v3] the is closest from the Cone (apex, normalizedDir2D, length, halfAngleSine)
static void OnTriangleVsConeCast2d(const Vec3f& v1, const Vec3f& v2, const Vec3f& v3, const Vec3f& apex, const Vec2f& normalizedDir2D,
	KyFloat32 length, const KyFloat32 halfAngleSine, Vec3f& closestPointonTriangle, KyFloat32& squareDist2d);

// Compute only the square distance between a segment and a point
static KyUInt64 GetSquareDistancePointToSegment(const CoordPos64& P, const CoordPos64& A, const CoordPos64& B, const KyInt64& sqDistAB);

// Compute only the square distance between a segment and a point multiplied by the segment square length
// NOTE: Be cautious to cases where sqDistAB == 0 !
static KyUInt64 GetSquareDistancePointToSegmentFactorSegmentSquareLength(const CoordPos64& P, const CoordPos64& A, const CoordPos64& B, const KyInt64& sqDistAB);

// Compute only the square distance between two segments
static KyUInt64 GetSquareDistanceSegmentToSegment(const CoordPos64& P, const CoordPos64& Q, const CoordPos64& A, const CoordPos64& B);

// The same as the previous ones, but in float metrics.
static KyFloat32 GetSquareDistancePointToSegment(const Vec2f& P, const Vec2f& A, const Vec2f& B, const KyFloat32& sqDistAB);
static KyFloat32 GetSquareDistancePointToSegmentFactorSegmentSquareLength(const Vec2f& P, const Vec2f& A, const Vec2f& B, KyFloat32 sqDistAB);
static KyFloat32 GetSquareDistanceSegmentToSegment(const Vec2f& P, const Vec2f& Q, const Vec2f& A, const Vec2f& B);

static bool OnSegmentVsDisk2dIfIntersection(const Vec3f& A, const Vec3f& B, const Vec3f& C, KyFloat32 radius, Vec3f& intersection);
static bool OnSegmentCapsule2dIfIntersection(const Vec3f& P, const Vec3f& Q, const Vec3f& A, const Vec3f& B, KyFloat32 radius, Vec3f& intersection);
};

}

#endif // Navigation_ClosestPoint_H

