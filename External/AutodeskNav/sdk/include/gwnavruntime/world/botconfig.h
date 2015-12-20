/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_PathFollowerConfig_H
#define Navigation_PathFollowerConfig_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_RefCount.h"

#include "gwnavruntime/channel/channelcomputerconfig.h"
#include "gwnavruntime/pathfollower/pathprogressconfig.h"
#include "gwnavruntime/pathfollower/shortcuttrajectoryconfig.h"
#include "gwnavruntime/pathfollower/channeltrajectoryconfig.h"
#include "gwnavruntime/pathfollower/radiusprofile.h"
#include "gwnavruntime/pathfollower/avoidanceconfig.h"
#include "gwnavruntime/pathfollower/collidercollectorconfig.h"
#include "gwnavruntime/world/pathfinderconfig.h"


namespace Kaim
{

/// Enum that defines the 2 possible Trajectories.
/// TrajectoryMode_Shortcut computes a straight line to a point on the bot's path.
/// TrajectoryMode_Channel uses a Channel around the path to compute a circle arc spline trajectory.
enum TrajectoryMode
{
	TrajectoryMode_Shortcut = 0,
	TrajectoryMode_Channel  = 1
};

/// Class that aggregates all parameters that allow to configure the PathFinding and the PathFollowing of a Bot.
/// Each Bot has its own instance of BotConfig.
class BotConfig: public RefCountBase<BotConfig, MemStat_Bot>
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	BotConfig() { SetDefaults(); }

	/// Sets all members to their default value.
	void SetDefaults()
	{
		m_radius = 0.0f;
		m_height = 0.0f;
		m_pathFinderConfig.SetDefaults();
		m_pathProgressConfig.SetDefaults();
		m_trajectoryMode = TrajectoryMode_Shortcut;
		m_shortcutTrajectoryConfig.SetDefaults();
		m_enableAvoidance = false;
		m_avoidanceConfig.SetDefaults();
		m_maxDesiredLinearSpeed = 5.0f;
		m_trajectoryMinimalMoveDistance = 0.01f;
	}

	void SetTrajectoryMode(TrajectoryMode trajectoryMode) { m_trajectoryMode = trajectoryMode; }
	TrajectoryMode GetTrajectoryMode() const { return m_trajectoryMode; }

public:
	// ------------------- Bot Shape Config -------------------

	/// The Bot radius in meter. If set to 0.0f, the Bot will use m_database generation metrics radius.
	/// \defaultvalue 0.0f
	KyFloat32 m_radius;

	/// The Bot height in meter. If set to 0.0f, the Bot will use m_database generation metrics height.
	/// \defaultvalue 0.0f
	KyFloat32 m_height;

	// ------------------- PathFind Config -------------------
	PathFinderConfig m_pathFinderConfig; ///< Configure the Bot pathfinding
	ChannelComputerConfig m_channelComputerConfig; ///< Configure the Bot channel

	// ------------------- PathProgress Config -------------------
	PathProgressConfig m_pathProgressConfig; ///< Configure the Bot progress on the path

	// ------------------- Trajectory Config -------------------

	// Trajectory
	TrajectoryMode m_trajectoryMode; ///< Choose between Shortcut (Straight line) and Channel (CircleArc) trajectories
	ShortcutTrajectoryConfig m_shortcutTrajectoryConfig; ///< Configure the Bot shortcut trajectory
	ChannelTrajectoryConfig m_channelTrajectoryConfig; ///< Configure the Bot channel trajectory

	/// The maximal linear speed for the Bot. It is used to have a upper bound
	/// when estimating the Bot displacement in Trajectory computation and
	/// follow.
	/// \defaultvalue 5.0f;
	KyFloat32 m_maxDesiredLinearSpeed;

	/// The (Shortcut or Channel) Trajectory velocity computation returns zero
	/// when the distance between the bot position and the target to reach is
	/// smaller than m_trajectoryMinimalMoveDistance. This avoids to normalize
	/// epsilon-length vectors. In meters.
	/// \defaultvalue 0.01f
	KyFloat32 m_trajectoryMinimalMoveDistance;

	// Avoidance Config
	bool m_enableAvoidance; ///< Enable the Bot avoidance
	ColliderCollectorConfig m_colliderCollectorConfig; ///< Configure how the Bot collects colliders (other bots, obstacles) around
	AvoidanceConfig m_avoidanceConfig; ///< Configure the avoidance
};


} // namespace Kaim

#endif // Navigation_PathFollowerConfig_H
