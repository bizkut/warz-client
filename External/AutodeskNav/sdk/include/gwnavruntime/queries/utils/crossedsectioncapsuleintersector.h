/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


	


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_CrossedSectionCapsuleIntersector_H
#define Navigation_CrossedSectionCapsuleIntersector_H

#include "gwnavruntime/database/database.h"
#include "gwnavruntime/navmesh/intersections.h"
#include "gwnavruntime/navmesh/closestpoint.h"
#include "gwnavruntime/queries/utils/basediskcangoquery.h"
#include "gwnavruntime/queries/utils/basediskcastquery.h"

namespace Kaim
{

/*
class CrossedSectionCapsuleIntersector
*/
class CrossedSectionCapsuleIntersector
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Query)

public:
	CrossedSectionCapsuleIntersector(const BaseDiskCastQuery& query, KyFloat32 distOnRight, KyFloat32 distOnLeft) :
		m_integerPrecision(query.m_database->GetDatabaseGenMetrics().m_integerPrecision), m_startCenter(query.GetStartPos()), m_normalizedDir2d(query.GetNormalizedDir2d()),
		m_dist(query.GetMaxDist()), m_radius(query.GetRadius())
	{
		const Vec2f normalizedDir_ortho(m_normalizedDir2d.PerpCCW());
		const Vec3f normalizedDir3D(m_normalizedDir2d.x, m_normalizedDir2d.y, 0.f);
		const Vec3f normalizedDir3D_ortho(normalizedDir_ortho.x, normalizedDir_ortho.y, 0.f);

		m_centerOfTheEndHalfDisk = m_startCenter + (normalizedDir3D * m_dist);

		const Vec3f orientedBox2d_a(m_startCenter - (normalizedDir3D_ortho * distOnRight));
		const Vec3f m_BBoxOfTheEndHalfDisk_a(m_centerOfTheEndHalfDisk - (normalizedDir3D_ortho * distOnRight));

		m_orientedBox2d.Set(orientedBox2d_a, m_normalizedDir2d, m_dist, distOnRight + distOnLeft, 0.f);
		m_BBoxOfTheEndHalfDisk.Set(m_BBoxOfTheEndHalfDisk_a, m_normalizedDir2d, m_radius, distOnRight + distOnLeft, 0.f);
	}

	CrossedSectionCapsuleIntersector(const BaseDiskCanGoQuery& query, KyFloat32 distOnRight, KyFloat32 distOnLeft) :
		m_integerPrecision(query.m_database->GetDatabaseGenMetrics().m_integerPrecision), m_startCenter(query.GetStartPos()), m_radius(query.GetRadius())
	{
		Vec3f dir(query.GetDestPos() - query.GetStartPos());
		dir.z = 0.f;
		m_dist = dir.Normalize();
		m_normalizedDir2d.Set(dir.x, dir.y);

		const Vec2f normalizedDir_ortho(m_normalizedDir2d.PerpCCW());
		const Vec3f normalizedDir3D(m_normalizedDir2d.x, m_normalizedDir2d.y, 0.f);
		const Vec3f normalizedDir3D_ortho(normalizedDir_ortho.x, normalizedDir_ortho.y, 0.f);

		m_centerOfTheEndHalfDisk = m_startCenter + (normalizedDir3D * m_dist);

		const Vec3f orientedBox2d_a(m_startCenter - (normalizedDir3D_ortho * distOnRight));
		const Vec3f m_BBoxOfTheEndHalfDisk_a(m_centerOfTheEndHalfDisk - (normalizedDir3D_ortho * distOnRight));

		m_orientedBox2d.Set(orientedBox2d_a, m_normalizedDir2d, m_dist, distOnRight + distOnLeft, 0.f);
		m_BBoxOfTheEndHalfDisk.Set(m_BBoxOfTheEndHalfDisk_a, m_normalizedDir2d, m_radius, distOnRight + distOnLeft, 0.f);
	}

	bool DoesIntersectEdge(const CoordPos64& startEdgeCoordPos, const CoordPos64& endEdgeCoordPos)
	{
		const Vec3f startEdgePos(startEdgeCoordPos.x * m_integerPrecision, startEdgeCoordPos.y * m_integerPrecision, 0.f);
		const Vec3f endEdgePos(endEdgeCoordPos.x * m_integerPrecision, endEdgeCoordPos.y * m_integerPrecision, 0.f);

		return Intersections::SegmentVsCrossSectionCapsule2d(startEdgePos, endEdgePos, m_orientedBox2d, m_centerOfTheEndHalfDisk, m_radius, m_BBoxOfTheEndHalfDisk);
	}

	void ComputeCollisionPosOnEdge(const CoordPos64& startEdgeCoordPos, const CoordPos64& endEdgeCoordPos, Vec3f& collisionPos, KyFloat32& squareDistToCollisionPos)
	{
		const Vec3f startEdgePos(startEdgeCoordPos.x * m_integerPrecision, startEdgeCoordPos.y * m_integerPrecision, 0.f);
		const Vec3f endEdgePos(endEdgeCoordPos.x * m_integerPrecision, endEdgeCoordPos.y * m_integerPrecision, 0.f);

		ClosestPoint::OnSegmentVsDiskCast2d(startEdgePos, endEdgePos, m_startCenter, m_radius, m_normalizedDir2d, m_dist, collisionPos, squareDistToCollisionPos);
	}

	void ComputeTriangleCost(const CoordPos64& v0CoordPos, const CoordPos64& v1CoordPos, const CoordPos64& v2CoordPos, KyFloat32& cost)
	{
		const Vec3f v0(v0CoordPos.x * m_integerPrecision, v0CoordPos.y * m_integerPrecision, 0.f);
		const Vec3f v1(v1CoordPos.x * m_integerPrecision, v1CoordPos.y * m_integerPrecision, 0.f);
		const Vec3f v2(v2CoordPos.x * m_integerPrecision, v2CoordPos.y * m_integerPrecision, 0.f);

		Vec3f unused;
		ClosestPoint::OnTriangleVsDiskCast2d(v0, v1, v2, m_startCenter, m_radius, m_normalizedDir2d, m_dist, unused, cost);
	}

private:
	KyFloat32 m_integerPrecision;

	Vec3f m_startCenter;
	Vec2f m_normalizedDir2d;
	KyFloat32 m_dist;
	KyFloat32 m_radius;

	OrientedBox2d m_orientedBox2d;
	OrientedBox2d m_BBoxOfTheEndHalfDisk;
	Vec3f m_centerOfTheEndHalfDisk;
};


}


#endif //Navigation_CrossedSectionCapsuleIntersector_H

