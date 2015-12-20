/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{


KY_INLINE KyFloat32 Intersections::ComputeAltitudeOfPointInPlane(const Vec2f& p, const Vec3f& planePoint, const Vec3f& planeNormal)
{
	// we are looking for p.z
	// p is in the plane if DotProd(normal, p - v1) == 0 -> resolve and get z
	// NB : we make a division by normal.z which must not be 0 (no vertical triangle)
	return (planePoint.z + (planeNormal.x * (planePoint.x - p.x) + planeNormal.y * (planePoint.y - p.y)) / planeNormal.z);
}

KY_INLINE KyFloat32 Intersections::ComputeAltitudeOfPointInTriangle(const Vec3f& p, const Vec3f& v1, const Vec3f& v2, const Vec3f& v3)
{
	return ComputeAltitudeOfPointInTriangle(p.Get2d(), v1, v2, v3);
}


KY_INLINE KyFloat32 Intersections::ComputeAltitudeOfPointInTriangle(const Vec2f& p, const Vec3f& v1, const Vec3f& v2, const Vec3f& v3)
{
	const Vec3f normal(CrossProduct(v2- v1, v3 - v1));
	return ComputeAltitudeOfPointInPlane(p, v1, normal);
}


KY_INLINE bool Intersections::IsPointInsideCircumCircle2d(const CoordPos& v1, const CoordPos& v2, const CoordPos& v3, const CoordPos& p)
{
	return ComputeInCircumCircleDeterminant(v1, v2, v3, p) >= 0;
}

KY_INLINE bool Intersections::IsPointStrictlyInsideCircumCircle2d(const CoordPos& v1, const CoordPos& v2, const CoordPos& v3, const CoordPos& p)
{
	return ComputeInCircumCircleDeterminant(v1, v2, v3, p) > 0;
}

KY_INLINE bool Intersections::SegmentVsTriangle2d(const Vec3f& a, const Vec3f& b, const Vec3f& v1, const Vec3f& v2, const Vec3f& v3)
{
	return SegmentVsTriangle2d(a.Get2d(), b.Get2d(), v1.Get2d(), v2.Get2d(), v3.Get2d());
}

inline bool Intersections::IsOnTheLeftOfSegment2d(const Vec3f& p, const Vec3f& v1, const Vec3f& v2)
{
	return IsOnTheLeftOfSegment2d(p.Get2d(), v1.Get2d(), v2.Get2d());
}

inline bool Intersections::IsStrictlyOnTheLeftOfSegment2d(const Vec3f& p, const Vec3f& v1, const Vec3f& v2)
{
	const Vec2f v1v2(v2.x - v1.x, v2.y - v1.y);
	const Vec2f v1p(p.x - v1.x, p.y - v1.y);
	return CrossProduct(v1v2, v1p) > 0;
}

inline bool Intersections::IsOnTheLeftOfSegment2d(const Vec2f& p, const Vec2f& v1, const Vec2f& v2)
{
	const Vec2f v1v2(v2.x - v1.x, v2.y - v1.y);
	const Vec2f v1p(p.x - v1.x, p.y - v1.y);
	const Vec2f v2p(p.x - v2.x, p.y - v2.y);

	const KyFloat32 crossProductv1v2_1 =  v1v2 ^ v1p;
	const KyFloat32 crossProductv1v2_2 = -(v1v2 ^ v2p);
	const KyFloat32 resultv1v2_1 = Fsel(crossProductv1v2_1, 1.f, 0.f);
	const KyFloat32 resultv1v2_2 = Fsel(crossProductv1v2_2, 0.f, 1.f);
	const KyFloat32 isOnLeft = Fsel(-v1v2.x, Fsel(v1v2.x, Fsel(-v1v2.y, resultv1v2_2, resultv1v2_1), resultv1v2_2), resultv1v2_1);

	return isOnLeft != 0.f;
}

inline bool Intersections::IsStrictlyOnTheLeftOfSegment2d(const Vec2f& p, const Vec2f& v1, const Vec2f& v2)
{
	const Vec2f v1v2(v2.x - v1.x, v2.y - v1.y);
	const Vec2f v1p(p.x - v1.x, p.y - v1.y);
	return CrossProduct(v1v2, v1p) > 0.f;
}


inline bool Intersections::IsOnTheLeftOfTheEdge2d(const CoordPos& p, const CoordPos& v1, const CoordPos& v1v2)
{
	const CoordPos v1p(p.x - v1.x, p.y - v1.y); // 31 Bits

	KY_LOG_WARNING_IF( (v1v2.x >= 1 << 15 || -v1v2.x >= 1 << 15), ("input value require too many bits of precision. Potential computation overflow, result may be incoherent"));
	KY_LOG_WARNING_IF( (v1v2.y >= 1 << 15 || -v1v2.y >= 1 << 15), ("input value require too many bits of precision. Potential computation overflow, result may be incoherent"));
	KY_LOG_WARNING_IF( ( v1p.x >= 1 << 15 || - v1p.x >= 1 << 15), ("input value require too many bits of precision. Potential computation overflow, result may be incoherent"));
	KY_LOG_WARNING_IF( ( v1p.y >= 1 << 15 || - v1p.y >= 1 << 15), ("input value require too many bits of precision. Potential computation overflow, result may be incoherent"));

	const KyInt32 crossProductv1v2 =  CrossProduct(v1v2, v1p); // 2 * 15 + 1 = 31 bits
	const KyInt32 resultv1v2_1 = Isel(crossProductv1v2, 1, 0);
	const KyInt32 resultv1v2_2 = Isel(-crossProductv1v2, 0, 1);

	const KyInt32 isOnLeft = Isel(-v1v2.x, Isel(v1v2.x, Isel(-v1v2.y, resultv1v2_2, resultv1v2_1), resultv1v2_2), resultv1v2_1); // 0 ou 1
	return isOnLeft != 0;
}

inline bool Intersections::IsStrictlyOnTheLeftOfTheEdge2d(const CoordPos& p, const CoordPos& v1, const CoordPos& v1v2)
{
	const CoordPos v1p(p.x - v1.x, p.y - v1.y); // 31 Bits

	KY_LOG_WARNING_IF( (v1v2.x >= 1 << 15 || -v1v2.x >= 1 << 15), ("input value require too many bits of precision. Potential computation overflow, result may be incoherent"));
	KY_LOG_WARNING_IF( (v1v2.y >= 1 << 15 || -v1v2.y >= 1 << 15), ("input value require too many bits of precision. Potential computation overflow, result may be incoherent"));
	KY_LOG_WARNING_IF( ( v1p.x >= 1 << 15 || - v1p.x >= 1 << 15), ("input value require too many bits of precision. Potential computation overflow, result may be incoherent"));
	KY_LOG_WARNING_IF( ( v1p.y >= 1 << 15 || - v1p.y >= 1 << 15), ("input value require too many bits of precision. Potential computation overflow, result may be incoherent"));

	return CrossProduct(v1v2, v1p) > 0;
}

inline bool Intersections::IsOnTheLeftOfTheEdge2d(const CoordPos64& p, const CoordPos64& v1, const CoordPos64& v1v2)
{
	const CoordPos64 v1p(p.x - v1.x, p.y - v1.y); // 31 Bits

	KY_LOG_WARNING_IF( (v1v2.x >= 1LL << 31 || -v1v2.x >= 1LL << 31), ("input value require too many bits of precision. Potential computation overflow, result may be incoherent"));
	KY_LOG_WARNING_IF( (v1v2.y >= 1LL << 31 || -v1v2.y >= 1LL << 31), ("input value require too many bits of precision. Potential computation overflow, result may be incoherent"));
	KY_LOG_WARNING_IF( ( v1p.x >= 1LL << 31 || - v1p.x >= 1LL << 31), ("input value require too many bits of precision. Potential computation overflow, result may be incoherent"));
	KY_LOG_WARNING_IF( ( v1p.y >= 1LL << 31 || - v1p.y >= 1LL << 31), ("input value require too many bits of precision. Potential computation overflow, result may be incoherent"));

	const KyInt64 crossProductv1v2 =  CrossProduct(v1v2, v1p); // 2 * 15 + 1 = 31 bits
	const KyInt64 resultv1v2_1 = Lsel(crossProductv1v2, 1, 0);
	const KyInt64 resultv1v2_2 = Lsel(-crossProductv1v2, 0, 1);

	const KyInt64 isOnLeft = Lsel(-v1v2.x, Lsel(v1v2.x, Lsel(-v1v2.y, resultv1v2_2, resultv1v2_1), resultv1v2_2), resultv1v2_1); // 0 ou 1
	return isOnLeft != 0;

}

inline bool Intersections::IsStrictlyOnTheLeftOfTheEdge2d(const CoordPos64& p, const CoordPos64& v1, const CoordPos64& v1v2)
{
	const CoordPos64 v1p(p.x - v1.x, p.y - v1.y); // 31 Bits

	KY_LOG_WARNING_IF( (v1v2.x >= 1LL << 31 || -v1v2.x >= 1LL << 31), ("input value require too many bits of precision. Potential computation overflow, result may be incoherent"));
	KY_LOG_WARNING_IF( (v1v2.y >= 1LL << 31 || -v1v2.y >= 1LL << 31), ("input value require too many bits of precision. Potential computation overflow, result may be incoherent"));
	KY_LOG_WARNING_IF( ( v1p.x >= 1LL << 31 || - v1p.x >= 1LL << 31), ("input value require too many bits of precision. Potential computation overflow, result may be incoherent"));
	KY_LOG_WARNING_IF( ( v1p.y >= 1LL << 31 || - v1p.y >= 1LL << 31), ("input value require too many bits of precision. Potential computation overflow, result may be incoherent"));

	return CrossProduct(v1v2, v1p) > 0;
}

// Intersections
// we test the intersection between convex polylines by using the Separated-Axes Theorem
KY_INLINE bool Intersections::AABBVsAABB2d(const Box2LL& box1, const Box2LL& box2)
{
	const KyInt64 operand1 = Lsel(box2.m_max.x - box1.m_min.x, (KyInt64)1, (KyInt64)0); // operand1 == 1 if box2.m_max.x >= box1.m_min.x
	const KyInt64 operand2 = Lsel(box1.m_max.x - box2.m_min.x, (KyInt64)1, (KyInt64)0); // operand2 == 1 if box1.m_max.x >= box2.m_min.x
	const KyInt64 operand3 = Lsel(box2.m_max.y - box1.m_min.y, (KyInt64)1, (KyInt64)0); // operand3 == 1 if box2.m_max.y >= box1.m_min.y
	const KyInt64 operand4 = Lsel(box1.m_max.y - box2.m_min.y, (KyInt64)1, (KyInt64)0); // operand4 == 1 if box1.m_max.y >= box2.m_min.y
	return (operand1 & operand2 & operand3 & operand4) != (KyInt64)0;
}

inline bool Intersections::AABBVsAABB2d(const Box2i& box1, const Box2i& box2)
{
	const KyInt32 operand1 = Isel(box2.m_max.x - box1.m_min.x, (KyInt32)1, (KyInt32)0); // operand1 == 1 if box2.m_max.x >= box1.m_min.x
	const KyInt32 operand2 = Isel(box1.m_max.x - box2.m_min.x, (KyInt32)1, (KyInt32)0); // operand2 == 1 if box1.m_max.x >= box2.m_min.x
	const KyInt32 operand3 = Isel(box2.m_max.y - box1.m_min.y, (KyInt32)1, (KyInt32)0); // operand3 == 1 if box2.m_max.y >= box1.m_min.y
	const KyInt32 operand4 = Isel(box1.m_max.y - box2.m_min.y, (KyInt32)1, (KyInt32)0); // operand4 == 1 if box1.m_max.y >= box2.m_min.y
	return (operand1 & operand2 & operand3 & operand4) != (KyInt32)0;
}

KY_INLINE void Intersections::SegmentVsSegment2d_RoundedIntersection(const CoordPos& a32, const CoordPos& b32, const CoordPos& c32, const CoordPos& d32, CoordPos& intersection32)
{
	const Vec2LL a(a32.x, a32.y);
	const Vec2LL b(b32.x, b32.y);
	const Vec2LL c(c32.x, c32.y);
	const Vec2LL d(d32.x, d32.y);
	CoordPos64 intersection;
	SegmentVsSegment2d_RoundedIntersection(a, b, c, d, intersection);
	intersection32.Set((KyInt32)intersection.x, (KyInt32)intersection.y);
}
KY_INLINE void Intersections::SegmentVsSegment2d_Intersection(const CoordPos& a32, const CoordPos& b32, const CoordPos& c32, const CoordPos& d32, CoordPos& intersection32)
{
	const Vec2LL a(a32.x, a32.y);
	const Vec2LL b(b32.x, b32.y);
	const Vec2LL c(c32.x, c32.y);
	const Vec2LL d(d32.x, d32.y);
	CoordPos64 intersection;
	SegmentVsSegment2d_Intersection(a, b, c, d, intersection);
	intersection32.Set((KyInt32)intersection.x, (KyInt32)intersection.y);
}


KY_INLINE bool Intersections::SegmentVsSegment2d(const CoordPos& a32, const CoordPos& b32, const CoordPos& c32, const CoordPos& d32)
{
	return SegmentVsSegment2d(Vec2LL(a32.x, a32.y), Vec2LL(b32.x, b32.y), Vec2LL(c32.x, c32.y), Vec2LL(d32.x, d32.y));
}

KY_INLINE bool Intersections::SegmentVsSegment2d(const CoordPos& a32, const CoordPos& b32, const CoordPos& c32, const CoordPos& d32, CoordPos& intersection32)
{
	CoordPos64 intersection;
	bool result = SegmentVsSegment2d(Vec2LL(a32.x, a32.y), Vec2LL(b32.x, b32.y), Vec2LL(c32.x, c32.y), Vec2LL(d32.x, d32.y), intersection);
	intersection32.Set((KyInt32)intersection.x, (KyInt32)intersection.y);
	return result;
}
KY_INLINE bool Intersections::SegmentVsSegment2d_Rounded(const CoordPos& a32, const CoordPos& b32, const CoordPos& c32, const CoordPos& d32, CoordPos& intersection32)
{
	CoordPos64 intersection;
	bool result = SegmentVsSegment2d_Rounded(Vec2LL(a32.x, a32.y), Vec2LL(b32.x, b32.y), Vec2LL(c32.x, c32.y), Vec2LL(d32.x, d32.y), intersection);
	intersection32.Set((KyInt32)intersection.x, (KyInt32)intersection.y);
	return result;
}


KY_INLINE bool Intersections::SegmentVsSegment2d(const CoordPos64& aInt, const CoordPos64& bInt, const CoordPos64& cInt, const CoordPos64& dInt, CoordPos64& intersection)
{
	if (SegmentVsSegment2d(aInt, bInt, cInt, dInt) == false)
		return false;

	SegmentVsSegment2d_Intersection(aInt, bInt, cInt, dInt, intersection);
	return true;
}

KY_INLINE bool Intersections::SegmentVsSegment2d_Rounded(const CoordPos64& a, const CoordPos64& b, const CoordPos64& c, const CoordPos64& d, CoordPos64& intersection)
{
	if (SegmentVsSegment2d(a, b, c, d) == false)
		return false;

	SegmentVsSegment2d_RoundedIntersection(a, b, c, d, intersection);
	return true;
}

KY_INLINE bool Intersections::DoSegmentsHaveOneComonExtremity(const CoordPos& a, const CoordPos& b, const CoordPos& c, const CoordPos& d)
{
	const KyInt32 aEqualC = (a.x ^ c.x) | (a.y ^ c.y); // == 0 if equal
	const KyInt32 aEqualD = (a.x ^ d.x) | (a.y ^ d.y); // == 0 if equal
	const KyInt32 bEqualC = (b.x ^ c.x) | (b.y ^ c.y); // == 0 if equal
	const KyInt32 bEqualD = (b.x ^ d.x) | (b.y ^ d.y); // == 0 if equal
	const KyInt32 oneSameExtremity = aEqualC * aEqualD * bEqualC * bEqualD;
	KY_ASSERT(oneSameExtremity != 0 || (a == c || a == d || b == c || b == d));
	// if one of the 4 is 0, the result is 0
	return oneSameExtremity == 0;
}

KY_INLINE bool Intersections::IsPointEqualToOneSegmentExtremity(const CoordPos& p, const CoordPos& a, const CoordPos& b, const CoordPos& c, const CoordPos& d)
{
	const KyInt32 aEqualIntersection = (p.x ^ a.x) | (p.y ^ a.y); // == 0 if equal
	const KyInt32 bEqualIntersection = (p.x ^ b.x) | (p.y ^ b.y); // == 0 if equal
	const KyInt32 cEqualIntersection = (p.x ^ c.x) | (p.y ^ c.y); // == 0 if equal
	const KyInt32 dEqualIntersection = (p.x ^ d.x) | (p.y ^ d.y); // == 0 if equal
	const KyInt32 equalToOneExtremity = aEqualIntersection * bEqualIntersection * cEqualIntersection * dEqualIntersection;
	KY_ASSERT(equalToOneExtremity != 0 || (a == p || b == p || p == c || p == d));
	// if one of the 4 is 0, the result is 0
	return equalToOneExtremity == 0;
}


}

