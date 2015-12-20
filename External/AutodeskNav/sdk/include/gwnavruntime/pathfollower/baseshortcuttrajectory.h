/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_BaseShortcutTrajectory_H
#define Navigation_BaseShortcutTrajectory_H

#include "gwnavruntime/pathfollower/itrajectory.h"
#include "gwnavruntime/pathfollower/iavoidancecomputer.h"
#include "gwnavruntime/visualdebug/visualdebugserver.h"
#include "gwnavruntime/world/bot.h"
#include "gwnavruntime/pathfollower/trajectoryblob.h"
#include "gwnavruntime/visualdebug/messages/visualdebugidremovalblob.h"
#include "gwnavruntime/pathfollower/targetonpathcomputer.h"


namespace Kaim
{

class SpatializedCylinder;
class Database;
class VisualDebugServer;
class Bot;

enum ShortcutTrajectorySnapOnPathMode
{
	SnapOnPathMode_Deactivated,            // snapping deactivated in ShortcutTrajectoryConfig
	SnapOnPathMode_Avoiding,               // avoiding, CanGo reachability of path is unknown, so apply shortcut normally
	SnapOnPathMode_NeedClassicalShortcut,  // classical shortcut needed
	SnapOnPathMode_UseCurrentTargetOnPath, // no need for classical shortcut to update the TargetOnPath, but it must be used
	SnapOnPathMode_SnapOnPath,             // snap on path
};

/// This class computes minimal trajectory made of just one velocity. It can be configured to:
///  - either always goes straight towards target on path at maxSpeed (when Avoidance is disabled)
///  - or delegate to an IAvoidanceComputer the computation of a velocity compatible with
///    surrounding moving obstacles and bots (when Avoidance is enabled).
/// NOTE: It cannot be instantiated directly, it is only the non-templated part of the ShortcutTrajectory
class BaseShortcutTrajectory : public RefCountBase<BaseShortcutTrajectory, MemStat_PathFollowing>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_PathFollowing)
	KY_CLASS_WITHOUT_COPY(BaseShortcutTrajectory)
public:
	// ---------------------------------- Public Member Functions ----------------------------------
	BaseShortcutTrajectory(ITrajectory* trajectory)
		: m_trajectory(trajectory)
		, m_useTargetProvidedByUser(false)
		, m_target(0.0f, 0.0f, 0.0f)
		, m_lastUpdateIdx(0)
		, m_snapOnPathMode(SnapOnPathMode_Deactivated)
		, m_visualDebugBlobSent(false)
	{}
	virtual ~BaseShortcutTrajectory() = 0;
	virtual void Clear(Bot* bot); // fully clear the trajectory and default its config
	virtual void ClearTrajectory(Bot* bot); // only clear the current computed trajectory

	const Vec3f& GetTrajectoryTarget() const { return m_target; }

	void ForceTargetToTargetOnPath();
	void ForceTargetToPosition(const Vec3f& forcedTarget);

public: // internal
	void SendVisualDebugBlob(VisualDebugServer& visualDebugServer, Bot* bot, VisualDebugSendChangeEvent changeEvent);
	void RemoveAllVisualDebugBeforeTrajectoryChange(VisualDebugServer& visualDebugServer, Bot* bot);

	void InitTargetOnPathIfNeeded(Bot* bot);
	bool CanUseTargetOnPath();
	ShortcutTrajectorySnapOnPathMode GetSnapOnPathMode(Bot* bot);
	void ComputeStraightTrajectory(Bot* bot, KyFloat32 simulationTimeInSeconds);
	void ComputeAvoidanceTrajectory(Bot* bot, KyFloat32 simulationTimeInSeconds);
	bool ComputeSnapOnPathTrajectory(Bot* bot, KyFloat32 simulationTimeInSeconds);
	
public: // internal
	ITrajectory*                     m_trajectory;
	ShortcutTrajectoryConfig         m_config;
	Ptr<IAvoidanceComputer>          m_avoidanceComputer;
	PositionOnLivePath               m_targetOnLivePath;
	TargetOnPathStatus               m_targetOnPathStatus;
	bool                             m_useTargetProvidedByUser;
	Vec3f                            m_target;
	KyUInt32                         m_lastUpdateIdx;
	ShortcutTrajectorySnapOnPathMode m_snapOnPathMode;

	bool m_visualDebugBlobSent;
};


} // namespace Kaim


#endif // Navigation_Trajectory_H
