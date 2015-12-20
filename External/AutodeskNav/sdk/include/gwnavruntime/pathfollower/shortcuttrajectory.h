/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_ShortcutTrajectory_H
#define Navigation_ShortcutTrajectory_H

#include "gwnavruntime/pathfollower/baseshortcuttrajectory.h"
#include "gwnavruntime/pathfollower/iavoidancecomputer.h"
#include "gwnavruntime/visualdebug/visualdebugserver.h"
#include "gwnavruntime/world/bot.h"
#include "gwnavruntime/pathfollower/trajectoryblob.h"
#include "gwnavruntime/pathfollower/targetonpathcomputer.h"
#include "gwnavruntime/pathfollower/shortcuttrajectoryblob.h"
#include "gwnavruntime/visualdebug/messages/visualdebugidremovalblob.h"


namespace Kaim
{

class SpatializedCylinder;
class Database;
class VisualDebugServer;
class Bot;


/// This class computes minimal trajectory made of just one velocity. It can be configured to:
///  - either always goes straight towards target on path at maxSpeed (when Avoidance is disabled)
///  - or delegate to an IAvoidanceComputer the computation of a velocity compatible with
///    surrounding moving obstacles and bots (when Avoidance is enabled).
/// NOTE: It should not be instantiated directly. It is in fact a sub-component of Trajectory
template<class TraverseLogic>
class ShortcutTrajectory : public BaseShortcutTrajectory
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_PathFollowing)
public:
	// ---------------------------------- Public Member Functions ----------------------------------
	ShortcutTrajectory<TraverseLogic>(ITrajectory* trajectory) 
		: BaseShortcutTrajectory(trajectory) {}

	virtual void Compute(Bot* bot, KyFloat32 simulationTimeInSeconds);
	virtual void Clear(Bot* bot); // fully clear the trajectory and default its config
	virtual void ClearTrajectory(Bot* bot); // only clear the current computed trajectory


public: // internal
	bool IsShortcutTrajectoryAllowingToSkipPathFollowingThisFrame(Bot* bot, KyFloat32& simulationTimeInSeconds);

	KyResult ComputeTargetOnPath(Bot* bot, KyFloat32 simulationTimeInSeconds);

public: // internal
	TargetOnPathComputer<TraverseLogic> m_targetOnPathComputer;
};


} // namespace Kaim

#include "gwnavruntime/pathfollower/shortcuttrajectory.inl"

#endif // Navigation_Trajectory_H
