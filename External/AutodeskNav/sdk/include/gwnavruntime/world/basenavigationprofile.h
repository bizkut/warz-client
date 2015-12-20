/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NONE
#ifndef Navigation_BaseNavigationProfile_H
#define Navigation_BaseNavigationProfile_H


#include "gwnavruntime/queries/utils/baseastarquery.h"
#include "gwnavruntime/queries/utils/basebestgraphvertexpathfinderquery.h"
#include "gwnavruntime/path/ipatheventlistobserver.h"
#include "gwnavruntime/pathfollower/basepathprogresscomputer.h"
#include "gwnavruntime/pathfollower/ipositiononpathvalidator.h"
#include "gwnavruntime/pathfollower/itrajectory.h"
#include "gwnavruntime/pathfollower/iavoidancecomputer.h"
#include "gwnavruntime/pathfollower/iavoidancesolver.h"
#include "gwnavruntime/pathfollower/iavoidancefilter.h"


namespace Kaim
{


/// BaseNavigationProfile and its derivation NavigationProfile<TraverseLogic> is a helper class that simplifies the customization of the navigation.
/// Enforces all PathFinding and PathFollowing customizations to use the same TraverseLogic.
/// Gathers explicitly the different customization interfaces that can be implemented in the PathFollower.
/// Makes a clear distinction between [classes that are sharable between bots] and [classes that are per Bot] transparent.
class BaseNavigationProfile : public RefCountBase<BaseNavigationProfile, MemStat_Bot>
{
public:
	BaseNavigationProfile() : m_profileId(KyUInt32MAXVAL) {}

	void SetProfileId(KyUInt32 profileId) { m_profileId = profileId; }
	KyUInt32 GetProfileId() const { return m_profileId; }

	// ------------------------ PathFinding ------------------------
	virtual Ptr<BaseAStarQuery> CreateAStarQuery() = 0;

	// ------------------------ PathProgress ------------------------
	// BasePathProgressComputer::Update() is called in Bot::UpdatePathFollowing()
	virtual Ptr<BasePathProgressComputer> GetSharedPathProgressComputer() = 0;

	// IPathEventListObserver is called in BasePathProgressComputer::Update()
	virtual Ptr<IPathEventListObserver> GetSharedPathEventListObserver() = 0;

	// IPositionOnPathValidator is called in BasePathProgressComputer::Update()
	virtual Ptr<IPositionOnPathValidator> GetSharedPositionOnPathValidator() = 0;

	// ------------------------ Trajectory ------------------------
	// ITrajectory::Compute() is called in Bot::UpdatePathFollowing() after BasePathProgressComputer::Update()
	virtual Ptr<ITrajectory> CreateTrajectory() = 0;

	// AvoidanceComputer::Compute() is called in ITrajectory::Compute()
	virtual Ptr<IAvoidanceComputer> GetSharedAvoidanceComputer() = 0;

	// AvoidanceSolver::Solve() is called in the AvoidanceComputer
	virtual Ptr<IAvoidanceSolver> GetSharedAvoidanceSolver() = 0;

	// IAvoidanceFilter::Filter() is called in the AvoidanceComputer
	virtual Ptr<IAvoidanceFilter> GetSharedAvoidanceFilter() = 0;

	// ------------------------ Queries ------------------------
	virtual Ptr<BaseRayCanGoQuery> CreateRayCanGoQuery() = 0;

	// ------------------------ Bot Queries ------------------------
	virtual Vec3f ComputeMoveOnNavMesh(Bot* bot, const Vec3f& velocity, KyFloat32 simulationTimeInSeconds) = 0;

public: //internal
	virtual bool HasTraverseLogicNoObsoleteFunctions() const = 0;

public:
	KyUInt32 m_profileId;
};


/// Per Bot data that correspond to the usage of a NavigationProfile in a Bot
class BotNavigation : public RefCountBase<BotNavigation, MemStat_Bot>
{
public:
	BotNavigation() : m_navigationProfileId(KyUInt32MAXVAL), m_navigationProfile(KY_NULL) {}

	void Init(BaseNavigationProfile* navigationProfile);

	KyUInt32               GetProfileId() { return m_navigationProfile->GetProfileId(); }
	BaseNavigationProfile* GetProfile()   { return m_navigationProfile; }

	Ptr<BaseAStarQuery>           GetOrCreateAStarQuery();
	Ptr<BasePathProgressComputer> GetOrCreatePathProgessComputer();
	Ptr<ITrajectory>              GetOrCreateTrajectory();

	Vec3f ComputeMoveOnNavMesh(Bot* bot, const Kaim::Vec3f& velocity, KyFloat32 simulationTimeInSeconds);

private:
	KyUInt32 m_navigationProfileId;
	BaseNavigationProfile* m_navigationProfile;

	Ptr<BaseAStarQuery>           m_aStarQuery;
	Ptr<BasePathProgressComputer> m_pathProgressComputer;
	Ptr<ITrajectory>              m_trajectory;
};


class BotNavigationCollection
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Bot)

public:
	BotNavigationCollection() : m_navigationProfiles(KY_NULL) {}

	// initialized with World::m_navigationProfiles
	void Init(KyArray<Ptr<BaseNavigationProfile> >* navigationProfiles) { m_navigationProfiles = navigationProfiles; }

	Ptr<BaseAStarQuery>           GetOrCreateAStarQuery(KyUInt32 navigationProfileId);
	Ptr<BasePathProgressComputer> GetOrCreatePathProgessComputer(KyUInt32 navigationProfileId);
	Ptr<ITrajectory>              GetOrCreateTrajectory(KyUInt32 navigationProfileId);

	Vec3f ComputeMoveOnNavMesh(KyUInt32 navigationProfileId, Bot* bot, const Kaim::Vec3f& velocity, KyFloat32 simulationTimeInSeconds);

private:
	bool CanUseNavigationProfileId(KyUInt32 navigationProfileId)
	{
		return m_navigationProfiles != KY_NULL && navigationProfileId < m_navigationProfiles->GetCount();
	}

	Ptr<BotNavigation> GetOrCreateBotNavigation(KyUInt32 navigationProfileId);

private:
	KyArray<Ptr<BaseNavigationProfile> >* m_navigationProfiles;
	KyArray<Ptr<BotNavigation> > m_botNavigations;
};


}

#endif
