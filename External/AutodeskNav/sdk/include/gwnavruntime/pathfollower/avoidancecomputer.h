/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_AvoidanceComputer_H
#define Navigation_AvoidanceComputer_H

#include "gwnavruntime/pathfollower/iavoidancecomputer.h"
#include "gwnavruntime/pathfollower/iavoidancefilter.h"
#include "gwnavruntime/pathfollower/avoidanceworkspace.h"
#include "gwnavruntime/queries/spatializedpointcollectorinaabbquery.h"
#include "gwnavruntime/pathfollower/avoidancesolver.h"
#include "gwnavruntime/pathfollower/avoidanceworkspacedisplay.h"
#include "gwnavruntime/world/botconfig.h"


namespace Kaim
{

class SpatializedCylinder;
class SpatializedPoint;
class AvoidanceComputer;
class Bot;


/// Class used to filter potential colliders in AvoidanceComputer at SpatializedPoint
/// collection step.
class AvoidanceComputerCollectorFilter
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	AvoidanceComputerCollectorFilter()
		: m_bot(KY_NULL)
		, m_avoidanceComputer(KY_NULL)
	{}

	bool ShouldCollectSpatializedPoint(SpatializedPoint* spatializedPoint);

	// ---------------------------------- Public Data Members ----------------------------------

	Bot* m_bot;
	AvoidanceComputer* m_avoidanceComputer;
};


/// This class compute collision-free trajectories.
/// It is used by the Trajectory class to delegate avoidance trajectory
/// computation. You can reuse it in your own ITrajectory implementation class.
class AvoidanceComputer : public IAvoidanceComputer
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_PathFollowing)
	KY_CLASS_WITHOUT_COPY(AvoidanceComputer)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	AvoidanceComputer();

	void SetAvoidanceSolver(Ptr<IAvoidanceSolver> avoidanceSolver) { m_avoidanceSolver = avoidanceSolver; }
	void SetAvoidanceFilter(Ptr<IAvoidanceFilter> avoidanceFilter) { m_avoidanceFilter = avoidanceFilter; }
	
	virtual AvoidanceResult Compute(Bot& bot, KyFloat32 simulationTimeInSeconds, KyFloat32 desiredSpeed, const Vec2f& movingDirection, Vec3f& avoidanceVelocity);

protected:
	friend class AvoidanceComputerCollectorFilter;

	void CollectSpatializedPoints(Bot& bot);
	void SetupAvoidanceWorkspace(const Bot& bot, KyFloat32 desiredSpeed, const Vec2f& desiredMovingDirection);
	void FillAvoidanceWorkspace(const Bot& bot);

	IAvoidanceComputer::AvoidanceResult SolveAvoidanceWorkspace(Bot& bot, KyFloat32 simulationTimeInSeconds, KyFloat32 desiredSpeed, const Vec2f& movingDirection, Vec3f& outputAvoidanceVelocity);
	
	void UpdateForcePassage(Bot& bot, KyFloat32 simulationTimeInSeconds, const Vec2f& desiredVelocity, 
		AvoidanceSolverConfig& solverConfig, IAvoidanceComputer::AvoidanceResult& solverResult, Vec2f& avoidanceVelocity);

	bool DoesCollectionQueryNeedToBeUpdated(const Bot& bot);
	const SpatializedCylinder* GetSpatializedCylinderFromSpatializedPoint(SpatializedPoint* spatializedPoint);
	AvoidanceWorkspaceAddColliderResult AddColliderToVOWorkspace(const AvoidanceConfig& avoidanceConfig, AvoidanceWorkspace& avoidanceWorkspace, const SpatializedCylinder& colliderCylinder, const KyFloat32 databaseRadiusPrecision);

	void SetupSolverConfig(AvoidanceSolverConfig&, const Bot&);

	IAvoidanceComputer::AvoidanceResult ComputeAvoidanceVelocity( Bot& bot, KyFloat32 simulationTimeInSeconds, KyFloat32 desiredSpeed, const Vec2f& desiredMovingDirection, Vec3f& avoidanceVelocity);

public: // internal
	// Used specifically by the ShortcutTrajectory in order to skip, cf. World::SetNumberOfFramesForShortcutTrajectoryPeriodicUpdate()
	virtual bool IsComputationRequiredThisFrame(Bot* bot, KyFloat32 simulationTimeInSeconds);

protected:
	Ptr<IAvoidanceSolver> m_avoidanceSolver; // If let to KY_NULL, AvoidanceComputer will instantiate a AvoidanceSolver with the world default TraverseLogic 
	SpatializedPointCollectorInAABBQuery<AvoidanceComputerCollectorFilter> m_collectQuery; // Put this in a Ptr<> if you want to push it in a queryQueue
	Ptr<IAvoidanceFilter> m_avoidanceFilter;
};


} // namespace Kaim

#endif // Navigation_AvoidanceComputer_H
