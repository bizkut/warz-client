/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: LAPA


namespace Kaim
{

template <class TLogic>
TargetOnPathComputer<TLogic>::TargetOnPathComputer() : m_nextShortcutSqDistTrigger(KyFloat32MAXVAL) {}

template <class TLogic>
void TargetOnPathComputer<TLogic>::Clear(Bot* bot)
{
	m_localCostAwareTraverseLogicData.m_allowedNavTagForTraversal.Clear();
	m_localCostAwareTraverseLogicData.m_userTraverseLogicData = KY_NULL;

	m_nextShortcutSqDistTrigger = KyFloat32MAXVAL;
	if (bot != KY_NULL)
		bot->m_nextShortcutSqDistTrigger = KyFloat32MAXVAL;
}

template <class TLogic>
inline bool TargetOnPathComputer<TLogic>::IsComputationRequiredThisFrame(Bot* bot, KyFloat32 simulationTimeInSeconds, PositionOnLivePath& targetOnPath)
{
	const ShortcutTrajectoryConfig& shortcutTrajectoryConfig = bot->GetShortcutTrajectoryConfig();
	KyFloat32 sqDistanceForceComputation = shortcutTrajectoryConfig.m_shortcutForceAttemptDistance * shortcutTrajectoryConfig.m_shortcutForceAttemptDistance;
	KyFloat32 sqDistanceExpectedMove = bot->GetMaxDesiredLinearSpeed() * simulationTimeInSeconds;
	sqDistanceExpectedMove *= sqDistanceExpectedMove;
	if (SquareDistance(bot->GetPosition(), targetOnPath.GetPosition()) < Max(sqDistanceForceComputation, sqDistanceExpectedMove))
		return true;

	return false;
}


template <class TLogic>
inline void TargetOnPathComputer<TLogic>::SetupLocalCostAwareTraverseLogic(const Bot* bot, const PositionOnLivePath& positionOnLivePath)
{
	m_localCostAwareTraverseLogicData.m_allowedNavTagForTraversal.Clear();
	bot->ComputeNavTagArrayForward(positionOnLivePath, bot->GetPathProgressConfig().m_navTagDiscoveryDistance, 
		m_localCostAwareTraverseLogicData.m_allowedNavTagForTraversal);
	bot->ComputeNavTagArrayBackward(positionOnLivePath, bot->GetPathProgressConfig().m_navTagDiscoveryDistance, 
		m_localCostAwareTraverseLogicData.m_allowedNavTagForTraversal);
	m_localCostAwareTraverseLogicData.m_userTraverseLogicData = bot->GetBotTraverseLogicUserData();
}

template <class TLogic>
inline void TargetOnPathComputer<TLogic>::SetupEmptyLocalCostAwareTraverseLogic(const Bot* bot)
{
	m_localCostAwareTraverseLogicData.m_allowedNavTagForTraversal.Clear();
	m_localCostAwareTraverseLogicData.m_userTraverseLogicData = bot->GetBotTraverseLogicUserData();
}


template <class TLogic>
inline KyResult TargetOnPathComputer<TLogic>::ComputeTargetOnPath(Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& targetOnPathStatus, KyFloat32 simulationTimeInSeconds, ComputeTargetOnPathSearchControl forwardSearchControl)
{
	m_nextShortcutSqDistTrigger = bot->m_nextShortcutSqDistTrigger;
	KyResult result = DoComputeTargetOnPath(bot, targetOnPath, targetOnPathStatus, simulationTimeInSeconds, forwardSearchControl);
	if (result == KY_SUCCESS)
		bot->m_nextShortcutSqDistTrigger = m_nextShortcutSqDistTrigger;
	return result;
}

template <class TLogic>
inline KyResult TargetOnPathComputer<TLogic>::DoComputeTargetOnPath(const Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& targetOnPathStatus, KyFloat32 simulationTimeInSeconds, ComputeTargetOnPathSearchControl forwardSearchControl)
{
	m_localCostAwareTraverseLogicData.m_userTraverseLogicData = bot->GetBotTraverseLogicUserData();

	const PathEdgeType currentEdgeType = targetOnPath.GetPathEdgeType();
	KyFloat32 costFromBotToTOP = KyFloat32MAXVAL;

	if (currentEdgeType == PathEdgeType_OnNavMesh)
	{
		// Ensure current target on path is still visible
		KY_FORWARD_ERROR_NO_LOG(MakeSureTargetOnPathIsVisibleAndGetCostFromBotToTOP(bot, targetOnPath, targetOnPathStatus, costFromBotToTOP));
	}

	if (targetOnPath.IsAtUpperBoundOfLivePath())
		return KY_SUCCESS;

	const PathEdgeType nextEdgeType = targetOnPath.GetPositionOnPath().GetEdgeTypeToMoveOnForward();
	if (currentEdgeType == PathEdgeType_OnNavMesh && nextEdgeType == PathEdgeType_OnNavMesh)
	{
		return ComputeWhenOnMesh(bot, targetOnPath, targetOnPathStatus, simulationTimeInSeconds, costFromBotToTOP, currentEdgeType, forwardSearchControl);
	}
	
	return ComputeWhenNotOnMesh(bot, targetOnPath, targetOnPathStatus, simulationTimeInSeconds, forwardSearchControl);
}

template <class TLogic>
inline KyResult TargetOnPathComputer<TLogic>::DoComputeTargetOnPathOnNavMesh(const Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& targetOnPathStatus, KyFloat32 simulationTimeInSeconds, ComputeTargetOnPathSearchControl forwardSearchControl)
{
	m_localCostAwareTraverseLogicData.m_userTraverseLogicData = bot->GetBotTraverseLogicUserData();

	PathEdgeType currentEdgeType = targetOnPath.GetPositionOnPath().GetEdgeTypeToMoveOnForward();
	if (currentEdgeType != PathEdgeType_OnNavMesh)
		return KY_ERROR;

	if (bot->GetNavTrianglePtr().IsValid() == false)
		return KY_ERROR;

	KyFloat32 costFromBotToTOP = KyFloat32MAXVAL;
	// Ensure current target on path is still visible on NavMesh
	if (CheckTargetOnPathIsVisibleAndGetCostFromBotToTOP(bot, targetOnPath, targetOnPathStatus, costFromBotToTOP) == KY_ERROR)
	{
		// Target on path is really not reachable ! => if possible, try to
		// backtrack it along the Path.
		targetOnPathStatus = TargetOnPathNotReachable;

		if (FindVisibleTargetOnPathBackwardWithCost(bot, targetOnPath, targetOnPathStatus, costFromBotToTOP) == KY_ERROR)
			return KY_ERROR;

		if (targetOnPathStatus == TargetOnPathNotReachable)
			return KY_SUCCESS;

		currentEdgeType = targetOnPath.GetPathEdgeType();
		if (currentEdgeType != PathEdgeType_OnNavMesh) // targeting a not-OnNavmesh edge, whereas we have a triangle
		{
			targetOnPathStatus = TargetOnPathNotReachable;
			return KY_SUCCESS;
		}
	}

	if (targetOnPath.IsAtUpperBoundOfLivePath())
		return KY_SUCCESS;

	return ComputeWhenOnMesh(bot, targetOnPath, targetOnPathStatus, simulationTimeInSeconds, costFromBotToTOP, currentEdgeType, forwardSearchControl);
}

template <class TLogic>
inline KyResult TargetOnPathComputer<TLogic>::ComputeWhenNotOnMesh(const Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& /*targetOnPathStatus*/, KyFloat32 simulationTimeInSeconds, ComputeTargetOnPathSearchControl forwardSearchControl)
{
	// In not in EdgeOnMesh case, we simply move the target on path to next PathEvent
	if (forwardSearchControl == ContinueSearch)
	{
		const Vec3f& botPosition = bot->GetPosition();
		const Vec3f& targetOnPathPosition = targetOnPath.GetPosition();
		const KyFloat32 sqDistToCurrentTargetOnPath = SquareDistance(botPosition, targetOnPathPosition);
		const ShortcutTrajectoryConfig& shortcutTrajectoryConfig = bot->GetShortcutTrajectoryConfig();
		const KyFloat32 sqDistanceForceComputation = shortcutTrajectoryConfig.m_shortcutForceAttemptDistance * shortcutTrajectoryConfig.m_shortcutForceAttemptDistance;
		KyFloat32 sqDistanceExpectedMove = bot->GetMaxDesiredLinearSpeed() * simulationTimeInSeconds;
		sqDistanceExpectedMove *= sqDistanceExpectedMove;
		if (sqDistToCurrentTargetOnPath < Max(sqDistanceForceComputation, sqDistanceExpectedMove))
		{
			const PathEdgeType nextEdgeType = targetOnPath.GetPositionOnPath().GetEdgeTypeToMoveOnForward();
			if (nextEdgeType != PathEdgeType_OnNavMesh)
				targetOnPath.MoveForward_StopAtEventOrAtPathNode(shortcutTrajectoryConfig.m_maxDistanceFromBot);
			else // if (targetOnPath.IsAtUpperBoundOfLivePath() == false) is checked before calling this function
			{
				if (targetOnPath.GetPositionOnPathStatus() == PositionOnPathStatus_OnPathNode)
					targetOnPath.MoveForwardToNextPathEdge();
				else if (targetOnPath.GetOnEventListStatus() == OnEventListStatus_OnEvent)
					targetOnPath.MoveForwardToNextPathEventInterval();
			}
		}
	}

	return KY_SUCCESS;
}

template <class TLogic>
inline KyResult TargetOnPathComputer<TLogic>::ComputeWhenOnMesh(const Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& targetOnPathStatus, KyFloat32 simulationTimeInSeconds, KyFloat32 costFromBotToTOP, PathEdgeType currentEdgeType, ComputeTargetOnPathSearchControl forwardSearchControl)
{
	if (forwardSearchControl == ContinueSearch)
		Shortcut(bot, targetOnPath, targetOnPathStatus, simulationTimeInSeconds, costFromBotToTOP, currentEdgeType);

	return KY_SUCCESS;
}

template <class TLogic>
inline KyResult TargetOnPathComputer<TLogic>::MakeSureTargetOnPathIsVisibleAndGetCostFromBotToTOP(const Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& targetOnPathStatus, KyFloat32& costFromBotToTOP)
{
	if (CheckTargetOnPathIsVisibleAndGetCostFromBotToTOP(bot, targetOnPath, targetOnPathStatus, costFromBotToTOP) == KY_SUCCESS)
		return KY_SUCCESS;

	// Target on path is really not reachable ! => if possible, try to
	// backtrack it along the Path.
	targetOnPathStatus = TargetOnPathNotReachable;

	return FindVisibleTargetOnPathBackwardWithCost(bot, targetOnPath, targetOnPathStatus, costFromBotToTOP);
}

template <class TLogic>
inline KyResult TargetOnPathComputer<TLogic>::CheckTargetOnPathIsVisibleAndGetCostFromBotToTOP(const Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& targetOnPathStatus, KyFloat32& costFromBotToTOP)
{
	const NavTrianglePtr& botNavTrianglePtr = bot->GetNavTrianglePtr();
	Database* database = bot->GetDatabase();
	const Vec3f& botPos = bot->GetPosition();
	const Vec3f& targetPos = targetOnPath.GetPosition();

	// We are reevaluating the current TargetOnPath, thus lets use minimal trajectory margin in the can go query.
	RayCanGoMarginMode rayCanGoMarginMode = (targetOnPath.GetMarginModeUsedForValidation() != NoMargin) ? MinimalTrajectoryMargin : NoMargin;

	RayCanGoQuery<LocalCostAwareTraverseLogic<TLogic> > canGoQuery;
	canGoQuery.BindToDatabase(database);
	canGoQuery.SetTraverseLogicUserData(&m_localCostAwareTraverseLogicData);
	canGoQuery.SetMarginMode(rayCanGoMarginMode);
	canGoQuery.SetComputeCostMode(QUERY_COMPUTE_COST_ALONG_3DAXIS);

	const DisplayListActivation displayListActivation = ((bot->GetCurrentVisualDebugLOD() == VisualDebugLOD_Maximal) ? DisplayList_Enable : DisplayList_Disable);
	ScopedDisplayList displayList(database->GetWorld(), displayListActivation);
#ifndef KY_BUILD_SHIPPING
	displayList.InitUserControlledLifespan_AssociateWithWorldElement("VisibilityCheck", "TargetOnPath", bot->GetVisualDebugId());
	displayList.HideByDefaultInNavigationLab();
#endif

	if (TestCanGoWithCost(canGoQuery, displayList, botPos, botNavTrianglePtr, targetPos, costFromBotToTOP))
	{
		targetOnPathStatus = TargetOnPathReachable;
		return KY_SUCCESS;
	}

	return KY_ERROR;
}

template <class TLogic>
inline bool TargetOnPathComputer<TLogic>::TestCanGoWithCost(RayCanGoQuery<LocalCostAwareTraverseLogic<TLogic> >& canGoQuery, ScopedDisplayList& displayList,
	const Vec3f& start, const NavTrianglePtr& startTriangle, const Vec3f& dest, KyFloat32& cost)
{
	canGoQuery.Initialize(start, dest);
	canGoQuery.SetStartTrianglePtr(startTriangle);
	canGoQuery.SetPerformQueryStat(QueryStat_TargetOnPathIsStillVisible);
	canGoQuery.PerformQuery();
	cost = canGoQuery.GetComputedCost();

	// Visual debug
#ifdef KY_BUILD_SHIPPING
	KY_UNUSED(displayList);
#else
	VisualColor color = canGoQuery.GetResult() == RAYCANGO_DONE_SUCCESS ? VisualColor::Lime : VisualColor::Red;
	displayList.PushLine(canGoQuery.GetStartPos(), canGoQuery.GetDestPos(), color);
	displayList.PushLine(canGoQuery.GetDestPos(), canGoQuery.GetDestPos() + Vec3f::UnitZ(), color);
	if (canGoQuery.GetMarginMode() != NoMargin)
	{
		const KyFloat32 margin = canGoQuery.m_database->GetRayCanGoMarginFloat(canGoQuery.GetMarginMode());
		displayList.PushStadium(canGoQuery.GetStartPos(), canGoQuery.GetDestPos(), margin, 24, VisualShapeColor(color, VisualShapePrimitive_Line));
	}
#endif

	return canGoQuery.GetResult() == Kaim::RAYCANGO_DONE_SUCCESS;
}

template <class TLogic>
inline KyResult TargetOnPathComputer<TLogic>::FindVisibleTargetOnPathBackwardWithCost(const Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& targetOnPathStatus, KyFloat32& costFromBotToTOP)
{
	if (targetOnPath.IsAtLowerBoundOfLivePath())
		return KY_ERROR;

	// Cannot can go to target on path => Find backward first visible target on path
	const ShortcutTrajectoryConfig& targetOnPathConfig = bot->GetShortcutTrajectoryConfig();
	const KyFloat32 backwardMaxDistance = targetOnPathConfig.m_backwardMaxDistance;
	const KyFloat32 backwardSamplingDistance = targetOnPathConfig.m_backwardSamplingDistance;

	PositionOnLivePath backwardTOPLimit = targetOnPath;
	backwardTOPLimit.MoveBackward_StopAtEvent(backwardMaxDistance);

	FindFirstVisiblePositionOnPathQuery<LocalCostAwareTraverseLogic<TLogic> > findFirstVisiblePositionOnPathQuery;
	findFirstVisiblePositionOnPathQuery.BindToDatabase(bot->GetDatabase());
	findFirstVisiblePositionOnPathQuery.SetTraverseLogicUserData(&m_localCostAwareTraverseLogicData);
	findFirstVisiblePositionOnPathQuery.SetRayCanGoMarginMode(IdealTrajectoryMargin);
	findFirstVisiblePositionOnPathQuery.SetSamplingDistance(backwardSamplingDistance);

	findFirstVisiblePositionOnPathQuery.Initialize(targetOnPath.GetPositionOnPath(), backwardTOPLimit.GetPositionOnPath(), bot->GetPosition());
	findFirstVisiblePositionOnPathQuery.SetVisibilityStartTrianglePtr(bot->GetNavTrianglePtr());
#ifndef KY_BUILD_SHIPPING
	const DisplayListActivation displayListActivation = ((bot->GetCurrentVisualDebugLOD() == VisualDebugLOD_Maximal) ? DisplayList_Enable : DisplayList_Disable);
	ScopedDisplayList displayList(bot->GetWorld(), displayListActivation);
	displayList.InitUserControlledLifespan_AssociateWithWorldElement("BackwardSearch", "TargetOnPath", bot->GetVisualDebugId());
	displayList.HideByDefaultInNavigationLab();
	findFirstVisiblePositionOnPathQuery.m_displayList = &displayList;
#else
	findFirstVisiblePositionOnPathQuery.m_displayList = KY_NULL;
#endif

	findFirstVisiblePositionOnPathQuery.SetPerformQueryStat(QueryStat_TargetOnPathSearchBackward);
	findFirstVisiblePositionOnPathQuery.PerformQuery();

	if (findFirstVisiblePositionOnPathQuery.GetResult() == FINDFIRSTVISIBLEPOSITIONONPATH_DONE_SUCCESS)
	{
		targetOnPath.SetNewPositionOnPathBackward(findFirstVisiblePositionOnPathQuery.GetFirstVisiblePositionOnPath());
		targetOnPath.SetMarginModeUsedForValidation(findFirstVisiblePositionOnPathQuery.GetRayCanGoMarginMode());
		costFromBotToTOP = findFirstVisiblePositionOnPathQuery.GetCostToVisiblePositionOnPath();
		targetOnPathStatus = TargetOnPathReachable;
		return KY_SUCCESS;
	}

	return KY_ERROR;
}

template <class TLogic>
inline ComputeTargetOnPathResult TargetOnPathComputer<TLogic>::Shortcut(
	const Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& /*targetOnPathStatus*/, KyFloat32 simulationTimeInSeconds,
	KyFloat32 costFromBotToTOP, PathEdgeType currentEdgeType)
{
	const ShortcutTrajectoryConfig& targetOnPathConfig = bot->GetShortcutTrajectoryConfig();

	const Vec3f& botPosition = bot->GetPosition();
	const Vec3f& targetOnPathPosition = targetOnPath.GetPosition();
	const KyFloat32 sqDist2dToCurrentTargetOnPath = SquareDistance2d(botPosition, targetOnPathPosition);

	if (bot->m_botNeedFullComputeFlags.GetTrajectoryNeedFullCompute() == false)
	{
		// When computation is not forced, we do it only when the TOP is
		// - either close to Bot (less than m_shortcutForceAttemptDistance)
		// - or the Bot sufficiently moved toward the TOP
		const KyFloat32 shortcutForceAttemptSquareDistance = targetOnPathConfig.m_shortcutForceAttemptDistance * targetOnPathConfig.m_shortcutForceAttemptDistance;
		if (sqDist2dToCurrentTargetOnPath > Max(shortcutForceAttemptSquareDistance, m_nextShortcutSqDistTrigger))
			return KeepCurrentTarget;
	}

	// Visual debug the candidate validation
	const DisplayListActivation displayListActivation = ((bot->GetCurrentVisualDebugLOD() == VisualDebugLOD_Maximal) ? DisplayList_Enable : DisplayList_Disable);
	ScopedDisplayList displayList(bot->GetWorld(), displayListActivation);
#ifndef KY_BUILD_SHIPPING
	displayList.InitUserControlledLifespan_AssociateWithWorldElement("ValidCandidate", "TargetOnPath", bot->GetVisualDebugId());
	displayList.HideByDefaultInNavigationLab();
#endif

	bool canMoveTargetOnPath = true;
	if (currentEdgeType != PathEdgeType_OnNavMesh)
	{
		KY_LOG_ERROR_IF(costFromBotToTOP != KyFloat32MAXVAL, ("In this case no cost should have computed for current TOP"));
		// We are reevaluating the current TargetOnPath. Lets use the minimal trajectory margin in can go query.
		const ValidationResult validationResult = ValidateCandidateAndGetCost(bot, displayList, targetOnPathPosition, MinimalTrajectoryMargin, costFromBotToTOP);
		if (validationResult != CandidateIsValid)
			canMoveTargetOnPath = false;
	}

	// When the bot is close to its current target on path, we relax margin constraint on the first candidate
	KyFloat32 desiredSpeed = bot->GetMaxDesiredLinearSpeed();
	const KyFloat32 minDistToTOP = desiredSpeed * simulationTimeInSeconds;
	RayCanGoMarginMode rayCanGoMarginMode = (sqDist2dToCurrentTargetOnPath > (minDistToTOP * minDistToTOP)) ? IdealTrajectoryMargin : NoMargin;

	PositionOnLivePath candidate = targetOnPath;
	KyFloat32 currentCostToCandidate = costFromBotToTOP;
	PositionOnLivePath bestCandidate; // Don't initialize it!
	if (canMoveTargetOnPath)
	{
		KY_LOG_ERROR_IF(costFromBotToTOP == KyFloat32MAXVAL, ("We should have computed a valid cost to current TargetOnPath to decide whether we can shortcut."));

		while (candidate.IsAtUpperBoundOfLivePath() == false) 
		{
			PositionOnLivePath previousCandidate     = candidate;
			PositionOnLivePath previousBestCandidate = bestCandidate;
			if (ShortcutOnNextSample(bot, simulationTimeInSeconds, displayList, candidate, rayCanGoMarginMode, bestCandidate, currentCostToCandidate) != ContinueSearch)
				break;

			// The first candidate has been validated (with or without margin constraint)
			// => reactivate margin constraint for next candidates validation.
			rayCanGoMarginMode = IdealTrajectoryMargin;
		}
	}

	if (bestCandidate.GetPositionOnPathStatus() != PositionOnPathStatus_Undefined)
	{
		targetOnPath = bestCandidate;
		targetOnPath.SetMarginModeUsedForValidation(rayCanGoMarginMode);
		m_nextShortcutSqDistTrigger = targetOnPathConfig.m_shortcutAttemptDistanceRatio * SquareDistance(botPosition, targetOnPath.GetPosition());
		return BetterTargetFound;
	}

	m_nextShortcutSqDistTrigger = targetOnPathConfig.m_shortcutAttemptDistanceRatio * sqDist2dToCurrentTargetOnPath;
	return NoBetterTargetFound;
}

template <class TLogic>
inline ComputeTargetOnPathSearchControl
	TargetOnPathComputer<TLogic>::ShortcutOnNextSample(
	const Bot* bot, KyFloat32 /*simulationTimeInSeconds*/, ScopedDisplayList& displayList, PositionOnLivePath& candidate, RayCanGoMarginMode rayCanGoMarginMode, PositionOnLivePath& bestCandidate, KyFloat32& costToCurrentCandidate)
{
	const ShortcutTrajectoryConfig& shortcutConfig = bot->GetShortcutTrajectoryConfig();
	KY_LOG_ERROR_IF(bot != candidate.GetBot(), ("Error, the PositionOnLivePath should be associated to the current Bot"));
	const KyFloat32 samplingDistance = shortcutConfig.m_samplingDistance;
	
	// check if cost must be disregarded based on bot position and current candidate
	bool previousCandidateReachedDisregardCostIfNextIsValid = 
		bot->HasReachedPosition(candidate.GetPosition(), shortcutConfig.m_shortcutForceAttemptDistance);
	
	// Move current candidate to next and get its cost
	KyFloat32 costFromCurrentToNewCandidate = 0.f;
	candidate.MoveForwardAndGetCost_StopAtEventOrAtPathNode<TLogic>(bot->GetBotTraverseLogicUserData(), samplingDistance, rayCanGoMarginMode, costFromCurrentToNewCandidate);

	// Check if next candidate is valid and get new cost
	KyFloat32 costFromBotToNewCandidate = KyFloat32MAXVAL;
	const ValidationResult validationResult = ValidateCandidateAndGetCost(bot, displayList, candidate.GetPosition(), rayCanGoMarginMode, costFromBotToNewCandidate);
	if (validationResult != CandidateIsValid)
		return StopSearch;

	// if cost must be taken into account check it
	if (previousCandidateReachedDisregardCostIfNextIsValid == false)
		if (costFromBotToNewCandidate > costToCurrentCandidate + costFromCurrentToNewCandidate)
			return StopSearch;

	// set new bestCandiate
	bestCandidate = candidate;
	costToCurrentCandidate = costFromBotToNewCandidate;

	if (candidate.IsAtUpperBoundOfLivePath())
		return StopSearch;

	if (candidate.IsAtACheckPoint())
		return StopSearch;

	PathEdgeType pathEdgeType = candidate.GetEdgeTypeToMoveOnForward();
	if (pathEdgeType != PathEdgeType_OnNavMesh)
		return StopSearch;
	
	return ContinueSearch;
}

template<class TLogic>
inline typename TargetOnPathComputer<TLogic>::ValidationResult
	TargetOnPathComputer<TLogic>::ValidateCandidateAndGetCost(
	const Bot* bot, ScopedDisplayList& displayList, const Vec3f& candidatePosition, RayCanGoMarginMode rayCanGoMarginMode, KyFloat32& cost)
{
	Database* database = bot->GetDatabase();
	const Vec3f& botPosition = bot->GetPosition();
	const NavTrianglePtr& botNavTrianglePtr = bot->GetNavTrianglePtr();
	const ShortcutTrajectoryConfig& targetOnPathConfig = bot->GetShortcutTrajectoryConfig();

	const KyFloat32 maxDistanceFromBot = targetOnPathConfig.m_maxDistanceFromBot;
	if (SquareDistance(botPosition, candidatePosition) > maxDistanceFromBot * maxDistanceFromBot)
		return InvalidCandidate_TooFarFromBot;

	RayCanGoQuery<LocalCostAwareTraverseLogic<TLogic> > canGoQuery;
	canGoQuery.BindToDatabase(database);
	canGoQuery.SetTraverseLogicUserData(&m_localCostAwareTraverseLogicData);
	canGoQuery.SetMarginMode(rayCanGoMarginMode);
	canGoQuery.SetComputeCostMode(QUERY_COMPUTE_COST_ALONG_3DAXIS);

	canGoQuery.Initialize(botPosition, candidatePosition);
	canGoQuery.SetStartTrianglePtr(botNavTrianglePtr);
	canGoQuery.SetPerformQueryStat(QueryStat_TargetOnPathShortcutForward);
	canGoQuery.PerformQuery();

#ifndef KY_BUILD_SHIPPING
	const VisualColor& color = canGoQuery.GetResult() == RAYCANGO_DONE_SUCCESS ? VisualColor::Lime : VisualColor::Red;
	displayList.PushLine(canGoQuery.GetStartPos(), canGoQuery.GetDestPos(), color);
	displayList.PushLine(canGoQuery.GetDestPos(), canGoQuery.GetDestPos() + Vec3f::UnitZ(), color);
	if (rayCanGoMarginMode != NoMargin)
	{
		const KyFloat32 margin = database->GetRayCanGoMarginFloat(rayCanGoMarginMode);
		displayList.PushStadium(canGoQuery.GetStartPos(), canGoQuery.GetDestPos(), margin, 24, VisualShapeColor(color, VisualShapePrimitive_Line));
	}
#else
	KY_UNUSED(displayList);
#endif

	if (canGoQuery.GetResult() == Kaim::RAYCANGO_DONE_SUCCESS)
	{
		cost = canGoQuery.GetComputedCost();
		return CandidateIsValid;
	}

	return InvalidCandidate_CanGoFailure;
}

} // namespace Kaim
