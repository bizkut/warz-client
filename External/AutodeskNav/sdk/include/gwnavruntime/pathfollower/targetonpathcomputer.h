/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_TargetOnPathComputer_H
#define Navigation_TargetOnPathComputer_H


#include "gwnavruntime/world/world.h"
#include "gwnavruntime/path/patheventlist.h"
#include "gwnavruntime/database/database.h"
#include "gwnavruntime/queries/raycangoquery.h"
#include "gwnavruntime/queries/findfirstvisiblepositiononpathquery.h"
#include "gwnavruntime/visualsystem/displaylist.h"
#include "gwnavruntime/containers/stringstream.h"
#include "gwnavruntime/spatialization/spatializedcylinder.h"
#include "gwnavruntime/world/bot.h"

namespace Kaim
{

enum ComputeTargetOnPathResult
{
	KeepCurrentTarget = 0,
	BetterTargetFound,
	NoBetterTargetFound
};

enum ComputeTargetOnPathSearchControl
{
	ContinueSearch = 0,
	StopSearch
};


/// Enumerates the possible status values of Bot Position on path.
enum TargetOnPathStatus
{
	/// Position on path has not been yet initialized at all. Main reason is that
	/// no path has been set yet.
	TargetOnPathNotInitialized = 0,

	/// Target on path is initialized but not validated. This namely occurs when
	/// a new path has been set, the Target on path is at Bot ProgressOnPath but its
	/// validation is pending.
	TargetOnPathUnknownReachability,

	/// Target on path was on a valid NavData that has been streamed out (due to
	/// explicit streaming or DynamicNavMesh) and no valid NavData has been
	/// found there.
	TargetOnPathInInvalidNavData,

	/// Target on path is no more reachable in straight line. This is namely due
	/// to NavData changes, Position on path is on valid NavData but there is
	/// something in the way.
	TargetOnPathNotReachable,

	/// Target on path is reachable in straight line.
	TargetOnPathReachable
};

/// This class updates Bot's target on path so that:
///  - it is always directly reachable from Bot current position in straight line
///  - it is less than a maximal distance from the Bot.
/// If the current target on path is not directly reachable, it searches a new
/// target on path backwards. This case happens when the Trajectory or the game engine
/// moved the Bot in another direction than towards its target on path.
/// All the parameters to control this computer are in TargetOnPathConfig.
/// If it can't manage to find a valid target on path, the TargetOnPathStatus is
// set to TargetOnPathNotReachable. No further action is done, you have to check the
/// status and take your own decision (compute a path to the same destination, pick
/// a new destination...).
template <class TraverseLogic>
class TargetOnPathComputer
{
public:
	// ---------------------------------- Public Member Functions ----------------------------------

	TargetOnPathComputer();

	bool IsComputationRequiredThisFrame(Bot* bot, KyFloat32 simulationTimeInSeconds, PositionOnLivePath& targetOnPath);
	
	// Must be called before ComputeTargetOnPath
	void SetupLocalCostAwareTraverseLogic(const Bot* bot, const PositionOnLivePath& positionOnLivePath);
	KyResult ComputeTargetOnPath(Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& targetOnPathStatus, KyFloat32 simulationTimeInSeconds, ComputeTargetOnPathSearchControl forwardSearchControl);
	void Clear(Bot* bot);

protected:
	// This do not update Bot::m_nextShortcutSqDistTrigger
	KyResult DoComputeTargetOnPath(const Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& targetOnPathStatus, KyFloat32 simulationTimeInSeconds, ComputeTargetOnPathSearchControl forwardSearchControl);

	enum ValidationResult
	{
		CandidateIsValid = 0,
		InvalidCandidate_CanGoFailure,
		InvalidCandidate_TooFarFromBot
	};

	KyResult ComputeWhenNotOnMesh(const Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& targetOnPathStatus, KyFloat32 simulationTimeInSeconds, ComputeTargetOnPathSearchControl forwardSearchControl);
	KyResult ComputeWhenOnMesh(const Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& targetOnPathStatus, KyFloat32 simulationTimeInSeconds, KyFloat32 costFromBotToTOP, PathEdgeType currentEdgeType, ComputeTargetOnPathSearchControl forwardSearchControl);

	ComputeTargetOnPathResult Shortcut(const Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& targetOnPathStatus, KyFloat32 simulationTimeInSeconds, KyFloat32 costFromBotToTOP, PathEdgeType currentEdgeType);
	KyResult MakeSureTargetOnPathIsVisibleAndGetCostFromBotToTOP(const Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& targetOnPathStatus, KyFloat32& costFromBotToTOP);
	KyResult CheckTargetOnPathIsVisibleAndGetCostFromBotToTOP(const Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& targetOnPathStatus, KyFloat32& costFromBotToTOP);
	KyResult FindVisibleTargetOnPathBackwardWithCost(const Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& targetOnPathStatus, KyFloat32& costFromBotToTOP);
	ComputeTargetOnPathSearchControl ShortcutOnNextSample(const Bot* bot, KyFloat32 simulationTimeInSeconds, ScopedDisplayList& displayList, PositionOnLivePath& candidate, RayCanGoMarginMode rayCanGoMarginMode, PositionOnLivePath& bestCandidate, KyFloat32& costToBest);
	ValidationResult ValidateCandidateAndGetCost(const Bot* bot, ScopedDisplayList& displayList, const Vec3f& candidatePosition, RayCanGoMarginMode rayCanGoMarginMode, KyFloat32& cost);
	bool TestCanGoWithCost(RayCanGoQuery<LocalCostAwareTraverseLogic<TraverseLogic> >& canGoQuery, ScopedDisplayList& displayList,
		const Vec3f& start, const NavTrianglePtr& startTriangle, const Vec3f& dest, KyFloat32& cost);

public: //internal
	void SetupEmptyLocalCostAwareTraverseLogic(const Bot* bot);
	// This disregards backward edge type, it only check forward edge type that must be on NavMesh and bot must have a valid triangle
	KyResult DoComputeTargetOnPathOnNavMesh(const Bot* bot, PositionOnLivePath& targetOnPath, TargetOnPathStatus& targetOnPathStatus, KyFloat32 simulationTimeInSeconds, ComputeTargetOnPathSearchControl forwardSearchControl);

private:
	KyFloat32 m_nextShortcutSqDistTrigger;
	LocalCostAwareTraverseLogicData m_localCostAwareTraverseLogicData;
};


} // namespace Kaim


#include "gwnavruntime/pathfollower/targetonpathcomputer.inl"

#endif // Navigation_TargetOnPathComputer_H
