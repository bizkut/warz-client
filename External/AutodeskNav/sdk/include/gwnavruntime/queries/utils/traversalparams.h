/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_TraversalParams_H
#define Navigation_TraversalParams_H

#include "gwnavruntime/database/positionspatializationrange.h"
#include "gwnavruntime/database/database.h"
#include "gwnavruntime/queries/utils/pathfinderabstractgraphtraversalmode.h"

namespace Kaim
{

/// An internal class that stores parameters used to control the propagation of traversals through the NavMesh.
/// Instances of this class are transparently set up by queries that use traversals to explore the NavMesh, such
/// as the AStarQuery.
class TraversalParameters
{
public:
	// ---------------------------------- Main API functions ----------------------------------

	KyUInt32  GetNumberOfProcessedNodePerFrame() const;
	KyFloat32 GetFromOutsideNavMeshDistance()    const;
	KyFloat32 GetToOutsideNavMeshDistance()      const;

	PathFinderAbstractGraphTraversalMode GetAbstractGraphTraversalMode() const;
	const PositionSpatializationRange&  GetPositionSpatializationRange() const;

public: // Internal
	// public because can be used by the Bot basic path finding helper functions
	void SetDefaults(Database* database)
	{
		m_numberOfVisitedNodePerFrame = 30;
		m_numberOfCanGoTestInRefinerPerFrame = 15;
		m_numberOfIntersectionTestPerFrame = 100;
		SetPositionSpatializationRange(database->GetPositionSpatializationRange());
		m_fromOutsideNavMeshDistance = 1.5f;
		m_toOutsideNavMeshDistance = 1.5f;
		m_abstractGraphTraversalMode = PATHFINDER_TRAVERSE_ABSTRACT_GRAPHS;
	}

	TraversalParameters() :
		m_fromOutsideNavMeshDistance(1.5f),
		m_toOutsideNavMeshDistance(1.5f),
		m_numberOfVisitedNodePerFrame(30),
		m_numberOfCanGoTestInRefinerPerFrame(15),
		m_numberOfIntersectionTestPerFrame(100),
		m_abstractGraphTraversalMode(PATHFINDER_TRAVERSE_ABSTRACT_GRAPHS)
	{}
	virtual ~TraversalParameters() {}

	void SetNumberOfProcessedNodePerFrame(KyUInt32 numberOfProcessedNodePerFrame);
	void SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange);
	void SetFromOutsideNavMeshDistance(KyFloat32 fromOutsideNavMeshDistance);
	void SetToOutsideNavMeshDistance(KyFloat32 toOutsideNavMeshDistance);
	void SetAbstractGraphTraversalMode(PathFinderAbstractGraphTraversalMode abstractGraphTraversalMode);

public: // Internal
	/// The altitude range used to find NavMesh triangles that correspond to the start and destination points.
	PositionSpatializationRange m_positionSpatializationRange;

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

	/// The number of nodes that will be processed each frame during the traversal part of the AStar.
	KyUInt32 m_numberOfVisitedNodePerFrame;

	/// The number of CanGo tests that will be processed each frame during the refiner part of the query.
	KyUInt32 m_numberOfCanGoTestInRefinerPerFrame;

	/// The number of intersections that will be processed each frame during the clamper part of the query.
	KyUInt32 m_numberOfIntersectionTestPerFrame;

	/// Indicates if the traversal should traverse AbstractGraphs
	PathFinderAbstractGraphTraversalMode m_abstractGraphTraversalMode;
};


KY_INLINE void TraversalParameters::SetFromOutsideNavMeshDistance(KyFloat32 fromOutsideNavMeshDistance)      { m_fromOutsideNavMeshDistance  = fromOutsideNavMeshDistance;    }
KY_INLINE void TraversalParameters::SetToOutsideNavMeshDistance(KyFloat32 toOutsideNavMeshDistance)          { m_toOutsideNavMeshDistance    = toOutsideNavMeshDistance;      }
KY_INLINE void TraversalParameters::SetNumberOfProcessedNodePerFrame(KyUInt32 numberOfProcessedNodePerFrame) { m_numberOfVisitedNodePerFrame = numberOfProcessedNodePerFrame; }

KY_INLINE KyFloat32 TraversalParameters::GetFromOutsideNavMeshDistance()    const { return m_fromOutsideNavMeshDistance;  }
KY_INLINE KyFloat32 TraversalParameters::GetToOutsideNavMeshDistance()      const { return m_toOutsideNavMeshDistance;    }
KY_INLINE KyUInt32  TraversalParameters::GetNumberOfProcessedNodePerFrame() const { return m_numberOfVisitedNodePerFrame; }

KY_INLINE const PositionSpatializationRange& TraversalParameters::GetPositionSpatializationRange() const { return m_positionSpatializationRange; }
KY_INLINE void TraversalParameters::SetPositionSpatializationRange(const PositionSpatializationRange& positionSpatializationRange)
{
	m_positionSpatializationRange = positionSpatializationRange;
}

KY_INLINE void TraversalParameters::SetAbstractGraphTraversalMode(PathFinderAbstractGraphTraversalMode abstractGraphTraversalMode)
{
	m_abstractGraphTraversalMode = abstractGraphTraversalMode;
}

KY_INLINE PathFinderAbstractGraphTraversalMode TraversalParameters::GetAbstractGraphTraversalMode() const
{
	return m_abstractGraphTraversalMode;
}

}

#endif //Navigation_TraversalParams_H

