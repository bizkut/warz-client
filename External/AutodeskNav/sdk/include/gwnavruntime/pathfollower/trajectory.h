/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_Trajectory_H
#define Navigation_Trajectory_H

#include "gwnavruntime/pathfollower/itrajectory.h"
#include "gwnavruntime/pathfollower/shortcuttrajectory.h"
#include "gwnavruntime/path/patheventlist.h"
#include "gwnavruntime/world/botconfig.h"


namespace Kaim
{

class SpatializedCylinder;
class Database;
class VisualDebugServer;
class ChannelTrajectory;

/// This class computes the trajectory either with ShortcutTrajectory or with ChannelTrajectory
template<class TraverseLogic>
class Trajectory : public ITrajectory
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_PathFollowing)
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	Trajectory();

	virtual ~Trajectory();

	TrajectoryMode GetTrajectoryMode() const { return m_trajectoryMode; }

	virtual void Compute(Bot* bot, KyFloat32 simulationTimeInSeconds);

	void SetAvoidanceComputer(Ptr<IAvoidanceComputer> avoidanceComputer);
	Ptr<IAvoidanceComputer> GetAvoidanceComputer() { return m_avoidanceComputer; }

	virtual const CircleArcSpline* GetSpline() const;
	virtual const PositionOnCircleArcSpline* GetPosOnSpline() const;

public: // internal

	virtual void Clear(Bot* bot); // fully clear the trajectory and default its config
	virtual void ClearTrajectory(Bot* bot); // only clear the current computed trajectory

	virtual bool IsShortcutTrajectoryAllowingToSkipPathFollowingThisFrame(Bot* bot, KyFloat32& simulationTimeInSeconds);

	virtual ResetTrajectoryStatus ResetTrajectory(Bot* bot, PositionOnLivePath& progressOnLivePath, PositionOnLivePath& directTargetOnLivePath);
	virtual ResetTrajectoryStatus CanResetTrajectory(Bot* bot, PositionOnLivePath& progressOnLivePath, PositionOnLivePath& directTargetOnLivePath);

	virtual void SendVisualDebugBlob(VisualDebugServer& visualDebugServer, Bot* bot, VisualDebugSendChangeEvent changeEvent);

protected:
	void SetTrajectoryMode(TrajectoryMode trajectoryMode, Bot* bot); // called in compute if m_trajectoryMode differs from the one set in the bot config

public: // internal
	TrajectoryMode m_trajectoryMode;
	Ptr<IAvoidanceComputer> m_avoidanceComputer;
	Ptr<ShortcutTrajectory<TraverseLogic> > m_shortcutTrajectory;
	Ptr<ChannelTrajectory>  m_channelTrajectory;

	TrajectoryMode m_previousTrajectoryModeForVisualDebug; //< used for visualDebugPurpose;
};

} // namespace Kaim

#include "gwnavruntime/pathfollower/trajectory.inl"

#endif // Navigation_Trajectory_H
