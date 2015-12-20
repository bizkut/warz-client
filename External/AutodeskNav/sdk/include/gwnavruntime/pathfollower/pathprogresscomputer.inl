/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY

namespace Kaim
{

template <class TLogic>
inline void PathProgressComputer<TLogic>::Update(Bot* bot, KyFloat32 simulationTimeInSeconds)
{
	KY_UNUSED(simulationTimeInSeconds);

	if (bot->m_botNeedFullComputeFlags.GetPathEventListNeedFullCompute())
		bot->m_livePath.ForceValidityIntervalRecompute();

	// Let's make sure that we have a path we can follow.
	// When a new path is available, replace the current path with the new one
	// and set the target on path to new path first node.
	UpdatePathEventList(bot);

	if (bot->GetFollowedPath() != KY_NULL)
	{
		ComputeProgressOnPath(bot);
	}
}

template <class TLogic>
inline void PathProgressComputer<TLogic>::UpdatePathEventList(Bot* bot)
{
	// Perform an interval validity step if needed
	PathValidityStatus pathValidityStatus = bot->m_livePath.GetPathValidityStatus();
	switch(pathValidityStatus)
	{
	case PathValidityStatus_NoPath :
		{
			bot->m_progressOnPathStatus = ProgressOnPathNotInitialized;
			bot->m_positionInChannel.Invalidate();
			bot->m_progressOnLivePath.Clear();
			bot->m_manualControlInterval.Clear();
			return;
		}

	case Dirty_ChannelInvalid: // KY_DEPRECATED, remove this case when removing Dirty_ChannelInvalid from PathValidityStatus enum.
			bot->m_botNeedFullComputeFlags.ChannelHasBeenInvalidated();
			bot->m_livePath.m_validityInterval.SetPathValidityStatus(ValidityIntervalMustBeRecomputed);
			// no return, no break, 
			// but goes on into case ValidityIntervalMustBeRecomputed
	case ValidityIntervalMustBeRecomputed:
		{
			// What to do when the path may be dirty (e.g. after database change broadcast)
			// restart scouting from botPositionOnPath. 1 step backward and 1 step forward

			bot->m_botNeedFullComputeFlags.PathEventListStartedFullCompute();

			if (bot->GetPathEventList().GetPathEventCount() != 0)
				m_pathEventListObserver->OnPathEventListDestroy(bot, bot->GetPathEventList(), IPathEventListObserver::RecomputePathEventListForSamePath);

			// What to do when the path may be dirty (after event broadCast?)
			// restart scouting from botPositionOnPath. 1 step backward and 1 step forward
			
			KyFloat32 validationDistanceBackward = bot->GetPathProgressConfig().m_pathValidationDistanceBackward;
			KyFloat32 validationDistanceForward = bot->GetPathProgressConfig().m_pathValidationDistanceForward;
			const PositionOnPath& progressOnPath = bot->m_progressOnLivePath.GetPositionOnPath();			
			ManualControlStatus enterManualControlStatus = bot->m_manualControlInterval.GetEnterStatus();
			ManualControlStatus exitManualControlStatus = bot->m_manualControlInterval.GetExitStatus();
			//KY_TODO("ReStartIntervalComputation should have an option to validate forward until leaving the NavTag of the ManualControlInterval 
			//		"instead of tweaking validationDistance even though it removes possibility that m_enterPos becomes the lowerBound"
			//		"but it only decreases chances that m_exitPos is really founs")
			//KY_TODO("ReStartIntervalComputation should instead have an option to validate forward until leaving the NavTag of the ManualControlInterval ")
			if (enterManualControlStatus > ManualControlStatus_PositionInvalid)
			{
				validationDistanceBackward += progressOnPath.ComputeDistance(bot->m_manualControlInterval.GetEnterPositionOnPath());
				if (exitManualControlStatus > ManualControlStatus_PositionInvalid)
					validationDistanceForward += progressOnPath.ComputeDistance(bot->m_manualControlInterval.GetExitPositionOnPath());
			}
			
			bot->m_livePath.ReStartIntervalComputation<TLogic>(bot->GetBotTraverseLogicUserData(), progressOnPath,
				validationDistanceBackward, validationDistanceForward);

			if (bot->GetPathEventList().GetPathEventCount() != 0)
			{
				bot->m_progressOnLivePath.FindLocationOnEventListFromScratch();
				bot->m_manualControlInterval.FindLocationOnEventListFromScratch();
				if (exitManualControlStatus == ManualControlStatus_Pending)
					bot->m_manualControlInterval.UpdatePendingExitPosition();

				bot->m_progressOnPathStatus = ProgressOnPathPending;
				m_pathEventListObserver->OnPathEventListBuildingStageDone(bot, bot->GetPathEventList(), 0, IPathEventListObserver::FirstIntervalIsNew);
			}

			// make the Bot aware that the PathEventList has been recomputed, this will recompute the Trajectory
			bot->m_botNeedFullComputeFlags.PathEventListHasChanged();
			break;
		}
	case ValidityIsBeingChecked:
	case Dirty_InvalidBeforeProgress_ProcessingAfter :
		{
			const KyUInt32 previousCount = bot->GetPathEventList().GetPathEventCount();
			PathValidityInterval::FirstNewIntervalExtendedStatus firstIntervalStatus = PathValidityInterval::FirstNewIntervalIsNotAnExtension;

			//What to do when the path were tagged as "may be dirty" but its full validity check is not finished
			// go on scouting : 1 step forward
			bot->m_livePath.ValidateForward<TLogic>(bot->GetBotTraverseLogicUserData(),
				bot->GetPathProgressConfig().m_pathValidationDistanceForward, &firstIntervalStatus);

			KY_LOG_ERROR_IF(previousCount > bot->GetPathEventList().GetPathEventCount() ,("We should have created node, and not delete them !"));

			if (firstIntervalStatus == PathValidityInterval::FirstNewIntervalIsNotAnExtension)
			{
				m_pathEventListObserver->OnPathEventListBuildingStageDone(bot, bot->GetPathEventList(),
					previousCount-1, IPathEventListObserver::FirstIntervalIsNew);
			}
			else
			{
				m_pathEventListObserver->OnPathEventListBuildingStageDone(bot, bot->GetPathEventList(), previousCount-2,
					IPathEventListObserver::FirstIntervalIsExtensionOfPreviousLast);
			}

			// make the Bot aware that the PathEventList has been recomputed, this will recompute the Trajectory
			bot->m_botNeedFullComputeFlags.PathEventListHasChanged();

			break;
		}

	default:
		break;
	}


	if (bot->m_livePath.GetPathValidityStatus() == Dirty_ProgressInvalid ||
		bot->m_progressOnLivePath.GetOnEventListStatus() == OnEventListStatus_Undefined ||
		bot->m_progressOnLivePath.GetOnEventListIndex() == KyUInt32MAXVAL)
	{
		bot->m_progressOnPathStatus = ProgressOnPathInInvalidNavData;
		bot->m_progressOnLivePath.ClearEventListLocation();
		bot->m_manualControlInterval.ClearEventListLocation();
		bot->m_positionInChannel.Invalidate();
		return;
	}

	KY_LOG_ERROR_IF(bot->GetPathEventList().GetPathEventCount() == 0, ("Error, if the pathEventlist is empty, no following can be process"));
	KY_ASSERT(bot->m_progressOnLivePath.GetPath() == bot->GetFollowedPath());
}



}
