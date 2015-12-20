/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_ITrajectory_H
#define Navigation_ITrajectory_H

#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/pathfollower/iavoidancecomputer.h"
#include "gwnavruntime/world/worldelement.h"
#include "gwnavruntime/pathfollower/resettrajectorystatus.h"

namespace Kaim
{

class Bot;
class VisualDebugServer;
class CircleArcSpline;
class PositionOnCircleArcSpline;
class PositionOnLivePath;

/// This interface class defines at the same time the trajectory data structure
/// and its computation method.
/// We provide a default simple trajectory (see Trajectory class).
/// If you need more complex trajectories (e.g. based on splines), you have to
/// implement your own ITrajectory. In that case, don't forget to compute
/// ITrajectory::m_velocity that is used for Bot position prediction.
class ITrajectory : public RefCountBase<ITrajectory, MemStat_PathFollowing>
{
	KY_CLASS_WITHOUT_COPY(ITrajectory)

public:
	// ---------------------------------- Public Member Functions ----------------------------------

	ITrajectory() : m_avoidanceResult(IAvoidanceComputer::NoAvoidance) {}
	virtual ~ITrajectory() {}

	virtual void Compute(Bot* bot, KyFloat32 simulationTimeInSeconds) = 0;

	/// send visual debug of your trajectory
	virtual void SendVisualDebugBlob(VisualDebugServer&, Bot*, VisualDebugSendChangeEvent) {}

	KY_INLINE const Vec3f& GetVelocity() const { return m_velocity; }
	KY_INLINE const IAvoidanceComputer::AvoidanceResult& GetAvoidanceResult() const { return m_avoidanceResult; }

	virtual const CircleArcSpline* GetSpline() const { return KY_NULL; }
	virtual const PositionOnCircleArcSpline* GetPosOnSpline() const { return KY_NULL; }

	// The ideal planar velocity is the one the entity wanted to follow if there was no avoidance
	virtual const Vec2f& GetIdealVelocity() const { return m_idealVelocity; }

public: // internal

	virtual void Clear(Bot* bot) = 0; // fully clear the trajectory and default its config
	virtual void ClearTrajectory(Bot* bot) = 0; // only clear the current computed trajectory

	virtual ResetTrajectoryStatus ResetTrajectory(Bot* /*bot*/, PositionOnLivePath& /*progressOnLivePath*/, PositionOnLivePath& /*directTargetOnLivePath*/) { return ResetTrajectoryStatus_Success; }
	virtual ResetTrajectoryStatus CanResetTrajectory(Bot* /*bot*/, PositionOnLivePath& /*progressOnLivePath*/, PositionOnLivePath& /*directTargetOnLivePath*/) { return ResetTrajectoryStatus_Success; }

	/// Used specifically for the ShortcutTrajectroy to determine when the Bot PathFollowing can be skipped or not
	/// cf. World::SetPathFollowingUpdatePeriodForShortcutTrajectory()
	virtual bool IsShortcutTrajectoryAllowingToSkipPathFollowingThisFrame(Bot* /*bot*/, KyFloat32& /*simulationTimeInSecondsToApplyThisFrame*/) { return false; }

	void ClearOutput()
	{
		m_velocity = Vec3f::Zero();
		m_idealVelocity = Vec2f::Zero();
		m_avoidanceResult = IAvoidanceComputer::NoAvoidance;
	}

public: // internal
	Vec3f m_velocity;
	Vec2f m_idealVelocity;
	IAvoidanceComputer::AvoidanceResult m_avoidanceResult;
};

} // namespace Kaim

#endif // Navigation_ITrajectory_H
