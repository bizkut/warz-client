/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// Primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_ConeIntersector_H
#define Navigation_ConeIntersector_H

#include "gwnavruntime/database/database.h"
#include "gwnavruntime/navmesh/intersections.h"
#include "gwnavruntime/navmesh/closestpoint.h"
#include "gwnavruntime/queries/utils/basediskcangoquery.h"
#include "gwnavruntime/queries/utils/basediskcastquery.h"


namespace Kaim
{

/*
class ConeIntersector
*/
class ConeIntersector
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Query)

public:
	ConeIntersector(const Vec3f& startCenter, const Vec3f& endCenter, KyFloat32 halfWidth, KyFloat32 integerPrecision)
		: m_integerPrecision(integerPrecision)
		, m_startCenter(startCenter)
		, m_endCenter(endCenter)
		, m_halfWidth(halfWidth)
	{
		m_normalizedDir2d = m_endCenter.Get2d() - m_startCenter.Get2d();
		m_length = m_normalizedDir2d.Normalize();

		const Vec2f leftDir = m_normalizedDir2d.PerpCCW();
		const Vec2f leftVec = m_halfWidth * leftDir;
		m_endLeft = m_endCenter + leftVec;
		m_endRight = m_endCenter - leftVec;
	}

	ConeIntersector(const Vec3f& startPos, const Vec2f& normalizedDir2d, KyFloat32 length, KyFloat32 halfWidth, KyFloat32 integerPrecision)
		: m_integerPrecision(integerPrecision)
		, m_startCenter(startPos)
		, m_normalizedDir2d(normalizedDir2d)
		, m_length(length)
		, m_halfWidth(halfWidth)
	{
		m_endCenter = m_startCenter + (m_normalizedDir2d * m_length);

		const Vec2f leftDir = m_normalizedDir2d.PerpCCW();
		const Vec2f leftVec = m_halfWidth * leftDir;
		m_endLeft = m_endCenter + leftVec;
		m_endRight = m_endCenter - leftVec;
	}

	bool DoesIntersectEdge(const CoordPos64& startEdgeCoordPos, const CoordPos64& endEdgeCoordPos)
	{
		const Vec3f startEdgePos(startEdgeCoordPos.x * m_integerPrecision, startEdgeCoordPos.y * m_integerPrecision, 0.f);
		const Vec3f endEdgePos(endEdgeCoordPos.x * m_integerPrecision, endEdgeCoordPos.y * m_integerPrecision, 0.f);

		return Intersections::SegmentVsTriangle2d(startEdgePos, endEdgePos, m_startCenter, m_endLeft, m_endRight);
	}

	void ComputeCollisionPosOnEdge(const CoordPos64& startEdgeCoordPos, const CoordPos64& endEdgeCoordPos, Vec3f& collisionPos, KyFloat32& squareDistToCollisionPos)
	{
		const Vec3f startEdgePos(startEdgeCoordPos.x * m_integerPrecision, startEdgeCoordPos.y * m_integerPrecision, 0.f);
		const Vec3f endEdgePos(endEdgeCoordPos.x * m_integerPrecision, endEdgeCoordPos.y * m_integerPrecision, 0.f);

		ClosestPoint::OnSegmentVsConeCast2d(startEdgePos, endEdgePos, m_startCenter, m_normalizedDir2d, m_length, m_halfWidth, collisionPos, squareDistToCollisionPos);
	}

	void ComputeTriangleCost(const CoordPos64& v0CoordPos, const CoordPos64& v1CoordPos, const CoordPos64& v2CoordPos, KyFloat32& cost)
	{
		const Vec3f v0(v0CoordPos.x * m_integerPrecision, v0CoordPos.y * m_integerPrecision, 0.f);
		const Vec3f v1(v1CoordPos.x * m_integerPrecision, v1CoordPos.y * m_integerPrecision, 0.f);
		const Vec3f v2(v2CoordPos.x * m_integerPrecision, v2CoordPos.y * m_integerPrecision, 0.f);

		Vec3f unused;
		ClosestPoint::OnTriangleVsConeCast2d(v0, v1, v2, m_startCenter,m_normalizedDir2d, m_length,  m_halfWidth, unused, cost);
	}

private:
	KyFloat32 m_integerPrecision;

	Vec3f m_startCenter;
	Vec3f m_endCenter;
	Vec3f m_endLeft;
	Vec3f m_endRight;

	Vec2f m_normalizedDir2d;
	KyFloat32 m_length;
	KyFloat32 m_halfWidth;

	OrientedBox2d m_orientedBox2d;
};


}


#endif //Navigation_ConeIntersector_H

