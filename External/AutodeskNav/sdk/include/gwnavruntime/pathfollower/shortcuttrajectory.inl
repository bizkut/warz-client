/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


namespace Kaim
{


template <class TLogic>
inline KyResult ShortcutTrajectory<TLogic>::ComputeTargetOnPath(Bot* bot, KyFloat32 simulationTimeInSeconds)
{
	KY_SCOPED_PERF_MARKER("PathFollower::ComputeTargetOnPath");

	if (CanUseTargetOnPath() == false)
		return KY_ERROR;
	
	ComputeTargetOnPathSearchControl forwardSearchControl = ContinueSearch;
	bool progressBeforeTarget = bot->GetProgressOnLivePath().DoesPrecede(m_targetOnLivePath);
	if (bot->GetProgressOnLivePath().GetOnEventListStatus() == OnEventListStatus_OnEvent)
	{
		const PathEvent& pathEvent = bot->GetPathEventList().GetPathEvent(m_targetOnLivePath.GetOnEventListIndex());
		if (pathEvent.IsACheckPoint() == true)
		{
			if (progressBeforeTarget == true)
			{
				m_targetOnLivePath = bot->GetProgressOnLivePath();
				forwardSearchControl = StopSearch;
			}
		}
	}
	
	if (m_targetOnLivePath.GetOnEventListStatus() == OnEventListStatus_OnEvent && forwardSearchControl != StopSearch)
	{
		const PathEvent& pathEvent = bot->GetPathEventList().GetPathEvent(m_targetOnLivePath.GetOnEventListIndex());
		if (pathEvent.IsOnUpperBound())
			forwardSearchControl = StopSearch;
		else if (pathEvent.IsACheckPoint())
		{
			// Stop only at check point that are after the progression point
			if (progressBeforeTarget == true)
				forwardSearchControl = StopSearch;
		}
		else
		{
			const PathEdgeType currentEdgeType = m_targetOnLivePath.GetEdgeTypeToMoveOnBackward();
			if (currentEdgeType == PathEdgeType_OnNavMesh)
			{
				const PathEdgeType nextEdgeType = m_targetOnLivePath.GetEdgeTypeToMoveOnForward();
				if (nextEdgeType != PathEdgeType_OnNavMesh)
				{
					// when leaving the NavMesh, wait for the progress
					if (progressBeforeTarget == true)
						forwardSearchControl = StopSearch;
				}
			}
		}	
	}

	m_targetOnPathComputer.SetupLocalCostAwareTraverseLogic(bot, bot->GetProgressOnLivePath());
	return m_targetOnPathComputer.ComputeTargetOnPath(bot, m_targetOnLivePath, m_targetOnPathStatus, simulationTimeInSeconds, forwardSearchControl);
}


template <class TLogic>
bool ShortcutTrajectory<TLogic>::IsShortcutTrajectoryAllowingToSkipPathFollowingThisFrame(Bot* bot, KyFloat32& simulationTimeInSecondsToApplyThisFrame)
{
	// cf. World::SetNumberOfFramesForShortcutTrajectoryPeriodicUpdate()
	// In Shortcut mode, the PathFollowing Update can skip some frames, hence simulationTimeInSeconds must be adjusted accordingly
	World* world = bot->GetWorld();
	KyUInt32 updatePeriod = world->GetNumberOfFramesForShortcutTrajectoryPeriodicUpdate();
	
	const KyUInt32 worldUpdateCount = world->GetUpdateCount();
	// if updating each frame, nothing to do
	if (updatePeriod == 1)
	{
		m_lastUpdateIdx = worldUpdateCount;
		return false;
	}

	// Otherwise, check if an update must be made this frame, based on the modulo of an index computed for all bots across all databases:
	KyUInt32 cyclingIndex = bot->GetIndexInCollection(); // get index in collection
	// shift index with the number of bots in previous databases
	for (KyUInt32 i = 0; i < bot->GetDatabase()->GetDatabaseIndex(); ++i)
		cyclingIndex += world->GetDatabase(i)->GetBotsCount(); 
	cyclingIndex += worldUpdateCount; // shift index with updateCount
	KyUInt32 remainingFramesBeforeNextUpdate = cyclingIndex % updatePeriod;
	if (remainingFramesBeforeNextUpdate == 0)
	{
		m_lastUpdateIdx = worldUpdateCount;
		return false;
	}
	// adjust the simulationTimeInSeconds to apply based on next expected update
	simulationTimeInSecondsToApplyThisFrame *= remainingFramesBeforeNextUpdate;

	if (m_targetOnLivePath.GetBot() == KY_NULL) // if not initialized yet (Compute() was never called)
		m_lastUpdateIdx = worldUpdateCount; // initialized m_lastUpdateIdx

	// in case bots were removed from the collection of its own database (index in collection did change
	// or if the count of previous databases changed  (the value used to shift the index changed),
	// we ensure that we force an additional update to be sure the updatePeriod is respected,
	// otherwise components could failed, e.g. the check against wall made in the avoidance could be wrong.
	if ((worldUpdateCount - m_lastUpdateIdx) >= updatePeriod)
	{
		m_lastUpdateIdx = worldUpdateCount;
		return false;		
	}

	// if the Trajectory must not be computed, we want the rest of Bot PathFollowing to be updated though!
	if (bot->GetDoComputeTrajectory() == false)
	{
		m_lastUpdateIdx = worldUpdateCount;
		return false;
	}
		
	BotNeedFullComputeFlags& botNeedFullComputeFlags = bot->m_botNeedFullComputeFlags;
	// if full computation is needed
	if (botNeedFullComputeFlags.GetTrajectoryNeedFullCompute() == true)
	{
		m_lastUpdateIdx = worldUpdateCount;
		return false;
	}

	// if path with channel has to be recomputed better to be prudent when following it
	// since shortcut can be used as a fallback to channelTrajectory
	if (botNeedFullComputeFlags.GetChannelNeedFullCompute() == true)
	{
		m_lastUpdateIdx = worldUpdateCount;
		return false;
	}

	// if PathEventList something happened on the PathEventList we need to update
	if (botNeedFullComputeFlags.GetPathEventListNeedFullCompute() == true)
	{
		m_lastUpdateIdx = worldUpdateCount;
		return false;
	}
	else
	{
		switch (bot->m_livePath.GetPathValidityStatus())
		{
		default:
		case ValidityIntervalMustBeRecomputed:
		case ValidityIsBeingChecked:
		case Dirty_InvalidBeforeProgress_ProcessingAfter:
			// PathEventList being processed
			m_lastUpdateIdx = worldUpdateCount;
			return false;

		// PathEvent list fully processed
		case PathValidityStatus_NoPath:
		case PathIsValid:
		case Dirty_ProgressInvalid:
		case Dirty_InvalidBeforeProgress_ValidAfter:
		case Dirty_ValidBeforeProgress_InvalidAfter:
		case Dirty_InvalidBeforeAndAfterProgress:
			break;
		}
	}
	
	if (bot->GetProgressOnLivePathStatus() == ProgressOnPathPending)
	{
		m_lastUpdateIdx = worldUpdateCount;
		return false;
	}

	// if progress is at a check point, ensure that test against the check point is done with proper previous and current positions
	if (bot->GetProgressOnLivePath().IsAtACheckPoint() == true)
	{
		m_lastUpdateIdx = worldUpdateCount;
		return false;
	}

	// Never skip frames while snapping, otherwise applying the given velocity over several frames could make the game entity goes through walls
	// Note that SnapOnPathMode_UseCurrentTargetOnPath must never skip frames when PathEventList changed 
	// and this is already checked above in the test against GetPathEventListNeedFullCompute() and GetPathValidityStatus()
	ShortcutTrajectorySnapOnPathMode snapOnPathMode = GetSnapOnPathMode(bot);
	if (snapOnPathMode == SnapOnPathMode_SnapOnPath)
	{
		m_lastUpdateIdx = worldUpdateCount;
		return false;
	}

	// if no snapping, or relying on shortcut for various reasons, check TargetOnPathComputer requirements :
	if (snapOnPathMode == SnapOnPathMode_Deactivated ||
		snapOnPathMode == SnapOnPathMode_Avoiding ||
		snapOnPathMode == SnapOnPathMode_NeedClassicalShortcut)
	{
		if (m_targetOnPathComputer.IsComputationRequiredThisFrame(bot, simulationTimeInSecondsToApplyThisFrame, m_targetOnLivePath) == true)
		{
			m_lastUpdateIdx = worldUpdateCount;
			return false;
		}
	}

	// if avoidance enabled, check if it needs an update
	if (bot->GetEnableAvoidance() == true)
	{
		if ((botNeedFullComputeFlags.GetColliderCollectionNeedFullCompute() == true)  ||
			(m_avoidanceComputer->IsComputationRequiredThisFrame(bot, simulationTimeInSecondsToApplyThisFrame) == true))
		{
			m_lastUpdateIdx = worldUpdateCount;
			return false;
		}
	}

	return true;
}

template <class TLogic>
void ShortcutTrajectory<TLogic>::Clear(Bot* bot)
{
	BaseShortcutTrajectory::Clear(bot);
	m_targetOnPathComputer.Clear(bot);
}

template <class TLogic>
void ShortcutTrajectory<TLogic>::ClearTrajectory(Bot* bot)
{
	BaseShortcutTrajectory::ClearTrajectory(bot);
	m_targetOnPathComputer.Clear(bot);
}

template <class TLogic>
void ShortcutTrajectory<TLogic>::Compute(Bot* bot, KyFloat32 simulationTimeInSeconds)
{
	// return a null velocity if Bot has arrived to upperBound
	if (bot->GetPathValidityInterval().m_upperBound.GetPositionOnPathStatus() != PositionOnPathStatus_Undefined &&
		bot->HasReachedPosition(bot->GetPathValidityInterval().m_upperBound.GetPosition(), bot->GetPathProgressConfig().m_checkPointRadius))
	{
		m_trajectory->ClearOutput();
		return;
	}

	if (m_useTargetProvidedByUser == true) // use provided m_target
	{
		if (bot->GetProgressOnLivePath().GetPathEdgeType() == PathEdgeType_OnNavMesh)
			ComputeAvoidanceTrajectory(bot, simulationTimeInSeconds);
		else
			ComputeStraightTrajectory(bot, simulationTimeInSeconds);
		return;
	}

	InitTargetOnPathIfNeeded(bot);
	if (ComputeSnapOnPathTrajectory(bot, simulationTimeInSeconds) == true)
		return;

	ComputeTargetOnPath(bot, simulationTimeInSeconds);
	m_target = m_targetOnLivePath.GetPosition();
	if (m_targetOnLivePath.GetPathEdgeType() == PathEdgeType_OnNavMesh)
		ComputeAvoidanceTrajectory(bot, simulationTimeInSeconds);
	else
		ComputeStraightTrajectory(bot, simulationTimeInSeconds);
}

} // namespace Kaim
