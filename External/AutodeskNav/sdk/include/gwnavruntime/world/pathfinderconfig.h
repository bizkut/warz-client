/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#ifndef Navigation_PathFinderConfig_H
#define Navigation_PathFinderConfig_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/queries/utils/pathfinderabstractgraphtraversalmode.h"

namespace Kaim
{


/// Class that aggregates most important parameters that allow to configure the PathFinding of a Bot.
class PathFinderConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Bot)

public:
	PathFinderConfig() { SetDefaults(); }

	void SetDefaults()
	{
		m_fromOutsideNavMeshDistance = 1.5f;
		m_toOutsideNavMeshDistance   = 1.5f;
		m_propagationBoxExtent       = 200.0f;
		m_abstractGraphTraversalMode = PATHFINDER_TRAVERSE_ABSTRACT_GRAPHS;
	}

	bool operator==(const PathFinderConfig& other) const
	{
		return m_fromOutsideNavMeshDistance   == other.m_fromOutsideNavMeshDistance
			&& m_toOutsideNavMeshDistance   == other.m_toOutsideNavMeshDistance
			&& m_propagationBoxExtent       == other.m_propagationBoxExtent
			&& m_abstractGraphTraversalMode == other.m_abstractGraphTraversalMode;
	}

	bool operator!=(const PathFinderConfig& other) const
	{
		return !operator==(other);
	}

public:
	/// The maximum distance from the start along the X axis and along the Y axis that will be searched for NavMesh,
	/// if the start point is outside the NavMesh.
	/// The search looks for triangles in an axis-aligned bounding box around the start point, with a half extent on
	/// the x and y axes equal to #m_fromOutsideNavMeshDistance.
	/// To disable this feature, you can set this value to 0.f
	/// Default value is 1.5f meters.
	KyFloat32 m_fromOutsideNavMeshDistance;

	/// The maximum distance from the destination along the X axis and along the Y axis that will be searched for NavMesh,
	/// if the destination point is outside the NavMesh.
	/// The search looks for triangles in an axis-aligned bounding box around the destination point, with a half extent on
	/// the x and y axes equal to #m_fromOutsideNavMeshDistance.
	/// To disable this feature, you can set this value to 0.f
	/// Default value is 1.5f meters.
	KyFloat32 m_toOutsideNavMeshDistance;

	/// The PathFinder propagation is limited to a 2d oriented bounding box computed by inflating the segment going
	/// from startPos to destPos by m_propagationBoxExtent.
	/// \defaultvalue 200.0f
	KyFloat32 m_propagationBoxExtent;

	/// Indicates if the PathFinder is allowed to traverse AbstractGraphs or not
	/// \defaultvalue PATHFINDER_TRAVERSE_ABSTRACT_GRAPHS
	PathFinderAbstractGraphTraversalMode m_abstractGraphTraversalMode;
};

}

#endif
