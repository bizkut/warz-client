/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

#include "gwnavruntime/pathfollower/trajectory.h"
#include "gwnavruntime/world/bot.h"
#include "gwnavruntime/pathfollower/shortcuttrajectory.h"
#include "gwnavruntime/pathfollower/channeltrajectory.h"
#include "gwnavruntime/pathfollower/circlearcspline.h"
#include "gwnavruntime/pathfollower/positiononcirclearcspline.h"
#include "gwnavruntime/pathfollower/targetonpathcomputer.h"

namespace Kaim
{

template<class TLogic>
Trajectory<TLogic>::Trajectory() 
	: m_trajectoryMode(TrajectoryMode_Shortcut)
	, m_previousTrajectoryModeForVisualDebug(TrajectoryMode_Shortcut)
{
	m_shortcutTrajectory = *KY_NEW ShortcutTrajectory<TLogic>(this);
}

template<class TLogic>
Trajectory<TLogic>::~Trajectory() {}

template<class TLogic>
void Trajectory<TLogic>::SetTrajectoryMode(TrajectoryMode trajectoryMode, Bot* bot)
{
	if (trajectoryMode == m_trajectoryMode)
	{
		if (bot->m_botNeedFullComputeFlags.GetTrajectoryNeedFullCompute() == false)
			return; // do not return if trajectory must be fully recomputed
	}
	else
	{
		m_previousTrajectoryModeForVisualDebug = m_trajectoryMode;
		m_trajectoryMode = trajectoryMode;
	}

	// Update ShortcutTrajecoryConfig in all cases, since ChannelTrajectory relies on Shortcut on some occasions.
	m_shortcutTrajectory->m_config = bot->GetShortcutTrajectoryConfig();
	if (m_trajectoryMode == TrajectoryMode_Channel)
	{
		if (m_channelTrajectory == KY_NULL)
		{
			m_channelTrajectory = *KY_NEW ChannelTrajectory(this);
			m_channelTrajectory->m_avoidanceComputer = m_avoidanceComputer;
		}

		m_channelTrajectory->m_config = bot->GetChannelTrajectoryConfig();
	}
}


template<class TLogic>
bool Trajectory<TLogic>::IsShortcutTrajectoryAllowingToSkipPathFollowingThisFrame(Bot* bot, KyFloat32& simulationTimeInSeconds)
{
	SetTrajectoryMode(bot->GetConfig().m_trajectoryMode, bot); // update trajectory mode
	if (m_trajectoryMode == TrajectoryMode_Channel)
		return false;

	//else if (m_trajectoryMode == TrajectoryMode_Shortcut)
	return m_shortcutTrajectory->IsShortcutTrajectoryAllowingToSkipPathFollowingThisFrame(bot, simulationTimeInSeconds);
}


template<class TLogic>
void Trajectory<TLogic>::Clear(Bot* bot)
{
	if (m_shortcutTrajectory)
		m_shortcutTrajectory->Clear(bot);
	if (m_channelTrajectory)
		m_channelTrajectory->Clear(bot);
	ClearOutput();
}

template<class TLogic>
void Trajectory<TLogic>::ClearTrajectory(Bot* bot)
{
	if (m_shortcutTrajectory)
		m_shortcutTrajectory->ClearTrajectory(bot);
	if (m_channelTrajectory)
		m_channelTrajectory->ClearTrajectory(bot);
	ClearOutput();
}

template<class TLogic>
void Trajectory<TLogic>::Compute(Bot* bot, KyFloat32 simulationTimeInSeconds)
{
	SetTrajectoryMode(bot->GetConfig().m_trajectoryMode, bot); // update trajectory mode

	if (m_trajectoryMode == TrajectoryMode_Channel)
	{
		m_channelTrajectory->Compute(bot, simulationTimeInSeconds);
		if (m_channelTrajectory->IsFallbackToShortcutRequired() == false)
			return;

		// Fallback to Shortcut trajectory:	
		// force use to SnapOnPath mode in order to update the TargetOnPath only if required
		// Note that when switching to shortcut m_shortcutTrajectory->m_config is set back to the bot Config.
		m_shortcutTrajectory->m_config.m_snapOnPathDistance = 0.05f;
	}

	// if (trajectoryMode == TrajectoryMode_Shortcut) or fallback from channelComputer
	m_shortcutTrajectory->Compute(bot, simulationTimeInSeconds);
}

template<class TLogic>
const CircleArcSpline* Trajectory<TLogic>::GetSpline() const
{
	if (m_trajectoryMode == TrajectoryMode_Shortcut || m_channelTrajectory == KY_NULL)
		return KY_NULL;

	return &m_channelTrajectory->m_trajectorySpline;
}

template<class TLogic>
const PositionOnCircleArcSpline* Trajectory<TLogic>::GetPosOnSpline() const
{
	if (m_trajectoryMode == TrajectoryMode_Shortcut || m_channelTrajectory == KY_NULL)
		return KY_NULL;

	return &m_channelTrajectory->m_positionOnSpline;
}

template<class TLogic>
void Trajectory<TLogic>::SendVisualDebugBlob(VisualDebugServer& visualDebugServer, Bot* bot, VisualDebugSendChangeEvent changeEvent)
{
	if (bot->m_trajectory != this) // This trajectory is not the one from the given bot
	{		
		VisualDebugDataRemovalBlobBuilder removalBlobBuilder = VisualDebugDataRemovalBlobBuilder(bot->GetVisualDebugId(), TrajectoryBlob::GetBlobTypeId());
		visualDebugServer.Send(removalBlobBuilder);

		m_shortcutTrajectory->RemoveAllVisualDebugBeforeTrajectoryChange(visualDebugServer, bot);
		if (m_channelTrajectory)
			m_channelTrajectory->RemoveAllVisualDebugBeforeTrajectoryChange(visualDebugServer, bot);
		return;
	}

	TrajectoryBlobBuilder builder(this, bot);
	visualDebugServer.Send(builder);

	if (m_trajectoryMode == TrajectoryMode_Channel && m_channelTrajectory != KY_NULL)
	{
		m_channelTrajectory->SendVisualDebugBlob(visualDebugServer, bot, changeEvent);
		if (m_channelTrajectory->m_resetTrajectoryStatus == ResetTrajectoryStatus_Success)
		{
			if (m_previousTrajectoryModeForVisualDebug == TrajectoryMode_Shortcut || m_shortcutTrajectory->m_visualDebugBlobSent)
				m_shortcutTrajectory->RemoveAllVisualDebugBeforeTrajectoryChange(visualDebugServer, bot);
			return;
		}
	}

	if (m_trajectoryMode == TrajectoryMode_Shortcut && m_previousTrajectoryModeForVisualDebug == TrajectoryMode_Channel && m_channelTrajectory != KY_NULL)
		m_channelTrajectory->RemoveAllVisualDebugBeforeTrajectoryChange(visualDebugServer, bot);
	m_shortcutTrajectory->SendVisualDebugBlob(visualDebugServer, bot, changeEvent);
}

template<class TLogic>
void Trajectory<TLogic>::SetAvoidanceComputer(Ptr<IAvoidanceComputer> avoidanceComputer)
{
	m_avoidanceComputer = avoidanceComputer;

	if (m_shortcutTrajectory != KY_NULL)
		m_shortcutTrajectory->m_avoidanceComputer = avoidanceComputer;
	
	if (m_channelTrajectory != KY_NULL)
		m_channelTrajectory->m_avoidanceComputer = avoidanceComputer;
}

template<class TLogic>
ResetTrajectoryStatus Trajectory<TLogic>::ResetTrajectory(Bot* bot, PositionOnLivePath& progressOnLivePath, PositionOnLivePath& directTargetOnLivePath)
{
	SetTrajectoryMode(bot->GetConfig().m_trajectoryMode, bot); // update trajectory mode

	ResetTrajectoryStatus resetTrajectoryStatus = CanResetTrajectory(bot, progressOnLivePath, directTargetOnLivePath);
	if (resetTrajectoryStatus < ResetTrajectoryStatus_Success)
		return resetTrajectoryStatus;
	
	m_shortcutTrajectory->ClearTrajectory(bot);
	if (m_trajectoryMode == TrajectoryMode_Channel)
	{
		m_channelTrajectory->ClearTrajectory(bot);
		m_channelTrajectory->m_resetTrajectoryStatus = resetTrajectoryStatus;
		if (resetTrajectoryStatus == ResetTrajectoryStatus_Success)
			return resetTrajectoryStatus;
		// otherwise, update ShortcutTrajectory since it will be used as fallback trajectory
	}

	// in all cases, update ShortcutTrajectory
	m_shortcutTrajectory->m_targetOnPathStatus = TargetOnPathReachable;
	m_shortcutTrajectory->m_targetOnLivePath = directTargetOnLivePath;
	m_shortcutTrajectory->m_target = directTargetOnLivePath.GetPosition(); // don't forget m_target for SnapOnPath	
	return resetTrajectoryStatus;
}

template<class TLogic>
ResetTrajectoryStatus Trajectory<TLogic>::CanResetTrajectory(Bot* bot, PositionOnLivePath& progressOnLivePath, PositionOnLivePath& directTargetOnLivePath)
{
	directTargetOnLivePath.Clear();
	Path* path = bot->GetFollowedPath();
	if (path == KY_NULL)
		return ResetTrajectoryStatus_InInvalidNavData;

	const PositionOnPath& positionOnPath = progressOnLivePath.GetPositionOnPath();
	if (positionOnPath.GetPositionOnPathStatus() == PositionOnPathStatus_Undefined)
		return ResetTrajectoryStatus_InInvalidNavData;

	const PathValidityInterval& pathValidityInterval = bot->GetPathValidityInterval();
	if (positionOnPath.IsStrictlyBefore(pathValidityInterval.m_lowerBound)) // befor lowerBound
		return ResetTrajectoryStatus_InInvalidNavData;

	if (pathValidityInterval.m_upperBound.IsStrictlyBefore(positionOnPath)) // after upperBound
		return ResetTrajectoryStatus_InInvalidNavData;

	const bool isMovingForwardOutsideNavMesh = (positionOnPath.GetEdgeTypeToMoveOnForward() != PathEdgeType_OnNavMesh);
	if (isMovingForwardOutsideNavMesh == true) // it is expected to be out of NavMesh
	{
		directTargetOnLivePath = progressOnLivePath;
		return ResetTrajectoryStatus_Success_OutsideNavMesh;
	}

	NavTrianglePtr navTrianglePtr = bot->GetNavTrianglePtr();
	const bool isOnNavTriangle = navTrianglePtr.IsValid();
	if (isOnNavTriangle == false) // supposed to be on NavMesh but not in a triangle
		return ResetTrajectoryStatus_OutsideNavMesh;

	ResetTrajectoryStatus successStatusWithTargetOnPath = ResetTrajectoryStatus_Success;
	ResetTrajectoryStatus failureStatusWithTargetOnPath = ResetTrajectoryStatus_CannotShortcutToPath;
	if (path->m_channelArray != KY_NULL)
	{
		// find position in channel
		ChannelSectionPtr positionInChannel;
		positionInChannel.SetFromPositionOnPath(positionOnPath);
		if (positionInChannel.IsValid() == false) // positionOnPath not in a channel section
			return ResetTrajectoryStatus_OutsideChannel;

		KyUInt32 positionFlags = 0;
		if (positionInChannel.IsPositionInSection(bot->GetPosition().Get2d(), positionFlags) == true) // bot is in channel section of positionOnPath
		{
			directTargetOnLivePath = progressOnLivePath;
			return ResetTrajectoryStatus_Success;
		}

		// bot out of position in channel, test if shortcut handle the situation
		successStatusWithTargetOnPath = ResetTrajectoryStatus_Success_OutsideChannel;
		failureStatusWithTargetOnPath = ResetTrajectoryStatus_OutsideChannel;
	}

	// check if shortcut can successfully bring bot to path from the given progressOnLivePath
	TargetOnPathComputer<TLogic> targetOnPathComputer;
	TargetOnPathStatus targetOnPathStatus = TargetOnPathUnknownReachability;
	KyFloat32 simulationTimeUsedToFindOutMinRecomputation = 0.f;
	directTargetOnLivePath = progressOnLivePath;

	// it would be preferable to use an empty LocalCostAwareTraverseLogic
	//       targetOnPathComputer.SetupEmptyLocalCostAwareTraverseLogic(bot);
	// because the Bot could stand on another NavTag than the one encoded in the path
	// however in case of a SimpleLogic the ShortcutTrajectory's TargetOnPathComputer will fail contrarily to what stated this function.
//	KY_TODO("Fix TargetOnPathComputer to handle Bot placed on Not-On-Path NavTags with a SimpleTraverseLogic, "
//		"or fix it by handling these TraverseLogic's typetraits differently here")
	targetOnPathComputer.SetupLocalCostAwareTraverseLogic(bot, directTargetOnLivePath);	
	if (targetOnPathComputer.DoComputeTargetOnPathOnNavMesh(
			bot, directTargetOnLivePath, targetOnPathStatus, 
			simulationTimeUsedToFindOutMinRecomputation, ContinueSearch) == KY_ERROR)
		return failureStatusWithTargetOnPath;
	if (targetOnPathStatus != TargetOnPathReachable)
		return failureStatusWithTargetOnPath;

	return successStatusWithTargetOnPath;
}

} // namespace Kaim
