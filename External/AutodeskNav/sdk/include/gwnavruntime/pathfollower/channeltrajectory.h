/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: MUBI
#ifndef Navigation_ChannelTrajectory_H
#define Navigation_ChannelTrajectory_H

#include "gwnavruntime/pathfollower/itrajectory.h"

#include "gwnavruntime/pathfollower/circlearcspline.h"
#include "gwnavruntime/pathfollower/circlearcsplinecomputer.h"
#include "gwnavruntime/pathfollower/positiononcirclearcspline.h"
#include "gwnavruntime/pathfollower/radiusprofile.h"
#include "gwnavruntime/pathfollower/channeltrajectoryconfig.h"
#include "gwnavruntime/channel/channelsectionptr.h"


namespace Kaim
{

class VisualDebugServer;
class ChannelTrajectoryConfig;
class PositionOnLivePath;
class CircleArcSplineComputer;
class ITrajectory;


/// ChannelTrajectory computes a CircleArcSpline that allows to anticipate and
/// adapt the velocity in the turns. To use this trajectory class, you have to
/// compute Channels along your bot paths.
/// NOTE: It should not be instantiated directly. It is in fact a sub-component of Trajectory
class ChannelTrajectory: public RefCountBase<ChannelTrajectory, MemStat_PathFollowing>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_PathFollowing)
	KY_CLASS_WITHOUT_COPY(ChannelTrajectory)

	enum EDirectionComputationMode
	{
		Follow,
		BeginningAvoidance,
		Avoiding,
		RecoverFromAvoidance
	};

public:
	ChannelTrajectory(ITrajectory* trajectory);
	~ChannelTrajectory() {}

	void Compute(Bot* bot, KyFloat32 simulationTimeInSeconds);

	const CircleArcSpline* GetSpline() const;
	const PositionOnCircleArcSpline* GetPosOnSpline() const;

public: // internal

	//////////////////////////////////////////////////////////////////////////
	// High-level internal methods

	void Clear(Bot* bot); // fully clear the trajectory and default its config
	void ClearTrajectory(Bot* bot); // only clear the current computed trajectory

	bool IsFallbackToShortcutRequired();

	//////////////////////////////////////////////////////////////////////////
	// VisualDebug

	virtual void SendVisualDebugBlob(VisualDebugServer& visualDebugServer, Bot* bot, VisualDebugSendChangeEvent changeEvent);
	void RemoveAllVisualDebugBeforeTrajectoryChange(VisualDebugServer& visualDebugServer, Bot* bot);

protected:
	bool CanUseChannel(Bot* bot);
	bool IsBotBackInChannel(Bot* bot);
	bool IsBotOutOfChannel(Bot* bot);
	bool IsEndOfChannelReached(Bot* bot);

	//////////////////////////////////////////////////////////////////////////
	// Spline computation internal methods

	void UpdateSpline(Bot* bot);
	bool ShouldRecomputeSpline(Bot* bot);
	void ComputeSpline(Bot* bot);

	void ComputeSplineStartPosition(Bot* bot);
	void ComputeSplineEndPosition(Bot* bot);
	void AddStartBubblesToCircleArcSplineComputer(Bot* bot, CircleArcSplineComputer& circleArcSplineComputer);
	KyUInt32 SelectSpline(const CircleArcSplineComputer& circleArcSplineComputer);


	//////////////////////////////////////////////////////////////////////////
	// Spline follow internal methods

	void UpdateSplinePosition(Bot* bot, KyFloat32 simulationTimeInSeconds);
	void ComputeMovingDirection(Bot* bot, KyFloat32 simulationTimeInSeconds, Vec2f& outMovingDirection);
	void ComputeVelocity(Bot* bot, KyFloat32 simulationTimeInSeconds);

	void ComputeAvoidanceTrajectory(Bot* bot, KyFloat32 simulationTimeInSeconds, KyFloat32 desiredSpeed, const Kaim::Vec2f& movingDirection);
	void StopBot();
	

public: // internal
	ITrajectory* m_trajectory;
	ResetTrajectoryStatus m_resetTrajectoryStatus;
	Ptr<IAvoidanceComputer> m_avoidanceComputer;

	CircleArcSpline m_trajectorySpline;
	PositionOnCircleArcSpline m_positionOnSpline;
	PositionOnCircleArcSpline m_targetOnSpline;

	ChannelSectionPtr m_splineStartSection;
	ChannelSectionPtr m_splineEndSection;
	Vec3f m_splineStartPosition;
	Vec3f m_splineEndPosition;

	// Some spline tail end information computed when identifying spline cut position
	// and used once new head is computed to append it to the remaining tail.
	CircleArc m_splineTailLastTurn;
	PositionOnCircleArcSpline m_splineTailEndPosition;
	KyUInt32 m_splineTailEndChannelSectionIdx;
	KyUInt32 m_positionOnSplineChannelSectionIdx;


	ChannelTrajectoryConfig m_config;
	RadiusProfile m_followRadiusProfile;

	bool m_advancedVisualDebuggingEnabled;
	SplineInputBlobDumpConfig m_advancedDebug_SplineInputBlobDumpConfig;

	EDirectionComputationMode m_directionComputationMode;
	Vec2f                     m_previousMovingDirection;
	float                     m_recoveryTime;
	float                     m_stopWaitTime;
};


KY_INLINE void ChannelTrajectory::StopBot()
{
	m_targetOnSpline = m_positionOnSpline;
	m_trajectory->ClearOutput();
}

} // namespace Kaim

#endif // Navigation_ChannelTrajectory_H
