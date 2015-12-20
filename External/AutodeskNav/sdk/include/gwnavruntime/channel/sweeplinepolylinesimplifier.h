/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_SweepLinePolylineSimplifier_H
#define Navigation_SweepLinePolylineSimplifier_H


#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/containers/sharedpoollist.h"


namespace Kaim
{

class SweepLinePoint
{
public:
	enum Type
	{
		Undefined,         // The sweep line point is not defined
		OnPolylinePoint,   // The sweep line point is exactly at the input polyline point m_index
		OnPolylineEdge,    // The sweep line point lies exactly on the input polyline edge from point m_index to point (m_index + 1)
		BelowEdge          // The sweep line point lies below the input polyline edge from point m_index to point (m_index + 1) (its Y value is smaller than the one of the point on input polyline with same X)
	};

	SweepLinePoint()
		: m_type(Undefined)
		, m_index(KyUInt32MAXVAL)
		, m_isTangentCapped(false)
	{}

	void SetAndCapTangent(Vec2f tangent, Vec2f maxTangent);

	Vec2f m_localPos; // the position in the local coordinate system
	Vec2f m_tangent; // (dx, dy) at this point
	Type m_type;
	KyUInt32 m_index; // The polyline point index, or the index of the start point of the edge it lies on or before.
	bool m_isTangentCapped;
};

class SweepLineSegment
{
public:
	SweepLineSegment() : m_addStartPoint(true) {}

	SweepLinePoint m_start;
	SweepLinePoint m_end;
	bool m_addStartPoint;
};

class SweepLinePointLessYSorter
{
public:
	SweepLinePointLessYSorter(const KyArray<SweepLinePoint>* sweepLinePointArray)
		: m_sweepLinePointArray(sweepLinePointArray)
	{}

	KY_INLINE bool operator() (KyUInt32 firstIdx, KyUInt32 secondIdx) const
	{
		const SweepLinePoint& e1 = m_sweepLinePointArray->At(firstIdx);
		const SweepLinePoint& e2 = m_sweepLinePointArray->At(secondIdx);
		return (
			(e1.m_localPos.y < e2.m_localPos.y) ||                                    // Sort in increasing Y order...
			((e1.m_localPos.y == e2.m_localPos.y) && (e1.m_localPos.x < e2.m_localPos.x)) // ... and for same Y, in increasing X order.
			);
	}

private:
	const KyArray<SweepLinePoint>* m_sweepLinePointArray;
};

class SweepLinePolylineSimplifier
{
public:
	SweepLinePolylineSimplifier();
	~SweepLinePolylineSimplifier() { Clear(); }

	void Clear();

	KyResult Compute(const SharedPoolList<Vec3f>& inPolyline,
		const Vec3f& origin, const Vec2f& xAxis, const Vec2f& yAxis,
		KyFloat32 maxX, KyFloat32 maxY, KyFloat32 maxTangent,
		SharedPoolList<Vec3f>& outPolyline);

private:
	Vec2f CapTangent(const Vec2f& tangent, const Vec2f& maxTangent);
	Vec2f Intersect(const Vec2f& p0, const Vec2f& t0, const Vec2f& p1, const Vec2f& t1);
	void AddToClosedPoints(const Vec2f& pos);
	KyResult TranslateInputPolyline(const SharedPoolList<Vec3f>& inPolyline);
	KyResult TranslateOutputPolyline(SharedPoolList<Vec3f>& outPolyline);
	void SweepLine(KyUInt32 inPolylinePointIdx);
	void AddPolylinePointToActiveSegment(SharedPoolList<SweepLineSegment>::Iterator activeSegmentIt, KyUInt32 inPolylinePointIdx);
	KyResult ComputeSimplifiedPolyline();

private:
	KyArray<SweepLinePoint> m_inPolyline;
	KyArray<Vec2f> m_closedPoints;
	SharedPoolList<SweepLineSegment> m_activeSegments;
	SharedPoolList<SweepLineSegment>::NodePool m_segmentPool;
	Vec3f m_origin;
	Vec2f m_xAxis;
	Vec2f m_yAxis;
	KyFloat32 m_maxX;
	KyFloat32 m_maxY;
	Vec2f m_maxTangent;
};

} // namespace Kaim

#endif // Navigation_SweepLinePolylineSimplifier_H