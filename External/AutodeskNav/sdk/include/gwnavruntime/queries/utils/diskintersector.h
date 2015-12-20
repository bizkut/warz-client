/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_DiskIntersector_H
#define Navigation_DiskIntersector_H

#include "gwnavruntime/database/database.h"
#include "gwnavruntime/navmesh/intersections.h"
#include "gwnavruntime/navmesh/closestpoint.h"
#include "gwnavruntime/queries/utils/basediskcollisionquery.h"
#include "gwnavruntime/queries/utils/basediskexpansionquery.h"

namespace Kaim
{

/*
class DiskIntersector
*/
class DiskIntersector
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Query)

public:
	DiskIntersector(const BaseDiskExpansionQuery& query) :
		m_integerPrecision(query.m_database->GetDatabaseGenMetrics().m_integerPrecision),
		m_center3f(query.GetCenterPos()), m_radius(query.GetRadiusMax()) {}

	DiskIntersector(const BaseDiskCollisionQuery& query) :
		m_integerPrecision(query.m_database->GetDatabaseGenMetrics().m_integerPrecision),
		m_center3f(query.GetCenterPos()), m_radius(query.GetRadius()) {}

	bool DoesIntersectEdge(const CoordPos64& startEdgeCoordPos, const CoordPos64& endEdgeCoordPos)
	{
		const Vec3f startEdgePos(startEdgeCoordPos.x * m_integerPrecision, startEdgeCoordPos.y * m_integerPrecision, 0.f);
		const Vec3f endEdgePos(endEdgeCoordPos.x * m_integerPrecision, endEdgeCoordPos.y * m_integerPrecision, 0.f);

		return Intersections::SegmentVsDisk2d(startEdgePos, endEdgePos, m_center3f, m_radius);
	}

	void ComputeCollisionPosOnEdge(const CoordPos64& startEdgeCoordPos, const CoordPos64& endEdgeCoordPos, Vec3f& collisionPos, KyFloat32& squareDistToCollisionPos)
	{
		const Vec3f startEdgePos(startEdgeCoordPos.x * m_integerPrecision, startEdgeCoordPos.y * m_integerPrecision, 0.f);
		const Vec3f endEdgePos(endEdgeCoordPos.x * m_integerPrecision, endEdgeCoordPos.y * m_integerPrecision, 0.f);

		ClosestPoint::OnSegmentVsPoint2d(startEdgePos, endEdgePos, m_center3f, collisionPos, squareDistToCollisionPos);
	}

	void ComputeTriangleCost(const CoordPos64& v0CoordPos, const CoordPos64& v1CoordPos, const CoordPos64& v2CoordPos, KyFloat32& cost)
	{
		const Vec3f v0(v0CoordPos.x * m_integerPrecision, v0CoordPos.y * m_integerPrecision, 0.f);
		const Vec3f v1(v1CoordPos.x * m_integerPrecision, v1CoordPos.y * m_integerPrecision, 0.f);
		const Vec3f v2(v2CoordPos.x * m_integerPrecision, v2CoordPos.y * m_integerPrecision, 0.f);

		Vec3f unused;
		ClosestPoint::OnTriangleVsPoint2d(v0, v1, v2, m_center3f, unused, cost);
	}

private:
	KyFloat32 m_integerPrecision;

	Vec3f m_center3f;
	KyFloat32 m_radius;
};


}


#endif //Navigation_DiskIntersector_H

