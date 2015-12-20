/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_ChannelSectionWidener_H
#define Navigation_ChannelSectionWidener_H

#include "gwnavruntime/navmesh/identifiers/navtriangleptr.h"
#include "gwnavruntime/queries/utils/clearancepolygonintersector.h"
#include "gwnavruntime/containers/sharedpoollist.h"
#include "gwnavruntime/math/vec3f.h"


namespace Kaim
{

class Database;
class Channel;
class Gate;


class ChannelSectionWidener
{
public:
	ChannelSectionWidener();

	void Clear();

	void Initialize(Database* database, Channel* channel, Gate* sectionStartGate, Gate* sectionEndGate, const NavTrianglePtr& sectionStartNavTrianglePtr);

	KyResult Compute(
		const SharedPoolList<ClearancePolygonIntersector::BorderEdge>& leftBorders,
		const SharedPoolList<ClearancePolygonIntersector::BorderEdge>& rightBorders);

private:
	void TranslateBorderEdgeListToPolyline(
		const SharedPoolList<ClearancePolygonIntersector::BorderEdge>& borderEdgeList,
		const Vec2f& toOutsideDir2d, KyFloat32 margin,
		SharedPoolList<Vec3f>& borderPolyline);

	KyResult AddIntermediaryGatesFromIntermediaryPolylines();

	Database* m_database;
	Channel* m_channel;
	Gate* m_sectionStartGate;
	Gate* m_sectionEndGate;
	NavTrianglePtr m_sectionStartNavTriangle;

	SharedPoolList<Vec3f> m_intermediaryBordersOnLeft;
	SharedPoolList<Vec3f> m_intermediaryBordersOnRight;
	SharedPoolList<Vec3f> m_intermediaryPolylineOnLeft;
	SharedPoolList<Vec3f> m_intermediaryPolylineOnRight;
	SharedPoolList<Vec3f>::NodePool m_positionPool;

	Vec2f m_pathEdgeDir2d;
	KyFloat32 m_pathEdgeLength2d;

public:
	KyFloat32 m_maxTangent;
	KyFloat32 m_idealClearanceRadius;
	KyFloat32 m_collapseDistance; // The distance beneath which close border points are collapsed into one point
	bool m_advancedVisualDebuggingEnabled;
};

} // namespace Kaim

#endif // Navigation_ChannelSectionWidener_H
