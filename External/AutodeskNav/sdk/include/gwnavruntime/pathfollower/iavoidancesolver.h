/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: MUBI - secondary contact: LAPA
#ifndef Navigation_IAvoidanceSolver_H
#define Navigation_IAvoidanceSolver_H

#include "gwnavruntime/pathfollower/iavoidancecomputer.h"
#include "gwnavruntime/channel/channelsectionptr.h"


namespace Kaim
{
class AvoidanceConfig;
class SpatializedPoint;
class AvoidanceWorkspace;
class Database;
class Gate;

class AvoidanceSolverConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_PathFollowing)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	AvoidanceSolverConfig()
		: m_avoidanceComputerConfig(KY_NULL)
		, m_colliderSpatializedPoint(KY_NULL)
		, m_workspace(KY_NULL)
		, m_database(KY_NULL)
		, m_deltaTime(0.016f)
		, m_bot(KY_NULL)
	{}

	AvoidanceSolverConfig(const AvoidanceConfig* avoidanceComputerConfig,
	                             const SpatializedPoint* colliderSpatializedPoint,
	                             AvoidanceWorkspace* workspace,
	                             Database* database,
	                             KyFloat32 simulationTimeInSeconds, Bot* bot)
	{
		Init(avoidanceComputerConfig,
			colliderSpatializedPoint,
			workspace,
			database,
			simulationTimeInSeconds, bot);
	}
	
	void Init(const AvoidanceConfig* avoidanceComputerConfig,
		const SpatializedPoint* colliderSpatializedPoint,
		AvoidanceWorkspace* workspace,
		Database* database,
		KyFloat32 simulationTimeInSeconds,
		Bot* bot)
	{
		m_avoidanceComputerConfig = avoidanceComputerConfig;
		m_colliderSpatializedPoint = colliderSpatializedPoint;
		m_workspace = workspace;
		m_database = database;
		m_deltaTime = simulationTimeInSeconds;
		m_channelSectionPtr.Invalidate();
		m_bot = bot;
	}

public:
	// ---------------------------------- Usual Common Members ----------------------------------

	const AvoidanceConfig*   m_avoidanceComputerConfig;
	const SpatializedPoint*  m_colliderSpatializedPoint;
	AvoidanceWorkspace*      m_workspace;
	Database*                m_database;
	KyFloat32                m_deltaTime;
	Bot*                     m_bot;
	
	// ---------------------------------- Channel Specific Members ----------------------------------
	// Define the local limit of the Channel (if any) in which the avoidance should try to find a solution

	ChannelSectionPtr        m_channelSectionPtr;
};


/// Interface class for Avoidance solvers.
class IAvoidanceSolver: public RefCountBase<IAvoidanceSolver, MemStat_PathFollowing>
{
	KY_CLASS_WITHOUT_COPY(IAvoidanceSolver)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	IAvoidanceSolver() {}

	virtual ~IAvoidanceSolver() {}

	virtual IAvoidanceComputer::AvoidanceResult Solve( AvoidanceSolverConfig& config, const Vec3f& previousVelocity, const Vec3f& desiredVelocity, Vec3f& outputVelocity) = 0;
};

} // namespace Kaim

#endif // Navigation_IAvoidanceSolver_H
