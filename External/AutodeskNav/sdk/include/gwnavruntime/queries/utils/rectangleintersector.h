/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_RectangleIntersector_H
#define Navigation_RectangleIntersector_H

#include "gwnavruntime/database/database.h"
#include "gwnavruntime/navmesh/intersections.h"
#include "gwnavruntime/navmesh/closestpoint.h"
#include "gwnavruntime/queries/utils/basesegmentcangoquery.h"
#include "gwnavruntime/queries/utils/basesegmentcastquery.h"

namespace Kaim
{

/*
class RectangleIntersector
*/
class RectangleIntersector
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Query)

public:
	RectangleIntersector(BaseSegmentCastQuery& query) : m_maxDist(query.GetMaxDist())
	{
		const DatabaseGenMetrics& databaseGenMetrics = query.m_database->GetDatabaseGenMetrics();

		m_integerPrecision = databaseGenMetrics.m_integerPrecision;

		Vec2f ortho = query.GetNormalizedDir2d().PerpCCW();
		m_startA.Set(query.GetStartPos().x + (ortho.x * query.GetRadius()), query.GetStartPos().y + (ortho.y * query.GetRadius()), query.GetStartPos().z);
		m_startB.Set(query.GetStartPos().x - (ortho.x * query.GetRadius()), query.GetStartPos().y - (ortho.y * query.GetRadius()), query.GetStartPos().z);

		m_rectangle = OrientedBox2d(m_startB, query.GetNormalizedDir2d(), m_maxDist, 2.f * query.GetRadius(), 0.f);

		const Vec2f direction2f(query.GetNormalizedDir2d() * m_maxDist);
		Vec2f destPos2f(query.GetStartPos().x + direction2f.x, query.GetStartPos().y + direction2f.y);
		const CoordPos64 destCoordPos(databaseGenMetrics.GetCoordPos64FromVec2f(destPos2f));

		databaseGenMetrics.GetCoordPos64FromVec3f(m_startB, m_o1);
		const CoordPos64 startCoordPos = databaseGenMetrics.GetCoordPos64FromWorldIntegerPos(query.GetStartIntegerPos());
		m_lengthVec = destCoordPos - startCoordPos;
		m_widthVec = (startCoordPos - m_o1) * 2;
	}

	RectangleIntersector(BaseSegmentCanGoQuery& query)
	{
		const DatabaseGenMetrics& databaseGenMetrics = query.m_database->GetDatabaseGenMetrics();

		m_integerPrecision = databaseGenMetrics.m_integerPrecision;

		Vec2f dir(query.GetDestPos().x - query.GetStartPos().x, query.GetDestPos().y - query.GetStartPos().y);
		dir.Normalize();
		dir = dir.PerpCCW();
		Vec3f normalizedDir3D(dir.x, dir.y, 0.f);

		const Vec3f o1Floats(query.GetStartPos() - (normalizedDir3D * query.GetRadius()));

		databaseGenMetrics.GetCoordPos64FromVec3f(o1Floats, m_o1);
		const CoordPos64 startCoordPos = databaseGenMetrics.GetCoordPos64FromWorldIntegerPos(query.GetStartIntegerPos());
		m_lengthVec = databaseGenMetrics.GetCoordPos64FromWorldIntegerPos(query.GetDestIntegerPos()) - startCoordPos;
		m_widthVec = (startCoordPos - m_o1) * 2;
	}

	bool DoesIntersectEdge(const CoordPos64& startEdgeCoordPos, const CoordPos64& endEdgeCoordPos)
	{
		return Intersections::SegmentVsOrientedBox2d(startEdgeCoordPos, endEdgeCoordPos, m_o1, m_lengthVec, m_widthVec);
	}

	void ComputeCollisionPosOnEdge(const CoordPos64& startEdgeCoordPos, const CoordPos64& endEdgeCoordPos, Vec3f& collisionPos, KyFloat32& squareDistToCollisionPos)
	{
		const Vec3f startEdgePos(startEdgeCoordPos.x * m_integerPrecision, startEdgeCoordPos.y * m_integerPrecision, 0.f);
		const Vec3f endEdgePos(endEdgeCoordPos.x * m_integerPrecision, endEdgeCoordPos.y * m_integerPrecision, 0.f);

		ClosestPoint::OnSegmentVsSegmentCast2d(startEdgePos, endEdgePos, m_startA, m_startB, m_maxDist, collisionPos, squareDistToCollisionPos);
	}

	void ComputeTriangleCost(const CoordPos64& v0CoordPos, const CoordPos64& v1CoordPos, const CoordPos64& v2CoordPos, KyFloat32& cost)
	{
		const Vec3f v0(v0CoordPos.x * m_integerPrecision, v0CoordPos.y * m_integerPrecision, 0.f);
		const Vec3f v1(v1CoordPos.x * m_integerPrecision, v1CoordPos.y * m_integerPrecision, 0.f);
		const Vec3f v2(v2CoordPos.x * m_integerPrecision, v2CoordPos.y * m_integerPrecision, 0.f);

		Vec3f unused;
		ClosestPoint::OnTriangleVsSegmentCast2d(v0, v1, v2, m_startA, m_startB, m_maxDist, unused, cost);
	}

private:
	/*
		 o4 +-----------------------+ o3
			|                       |
			|                       |
			|                       |
		 o1 +---------------------->+
		        m_lengthVec
	*/

	CoordPos64 m_o1;
	Vec2LL m_lengthVec;
	Vec2LL m_widthVec;

	OrientedBox2d m_rectangle;
	Vec3f m_startA;
	Vec3f m_startB;
	KyFloat32 m_maxDist;


	KyFloat32 m_integerPrecision;

};


}


#endif //Navigation_RectangleIntersector_H

