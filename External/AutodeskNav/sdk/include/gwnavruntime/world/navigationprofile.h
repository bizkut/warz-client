/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NONE
#ifndef Navigation_NavigationProfile_H
#define Navigation_NavigationProfile_H

#include "gwnavruntime/world/basenavigationprofile.h"
#include "gwnavruntime/pathfollower/pathprogresscomputer.h"
#include "gwnavruntime/queries/astarquery.h"
#include "gwnavruntime/queries/bestgraphvertexpathfinderquery.h"
#include "gwnavruntime/pathfollower/trajectory.h"
#include "gwnavruntime/pathfollower/positiononpathcheckpointvalidator.h"
#include "gwnavruntime/pathfollower/avoidancecomputer.h"
#include "gwnavruntime/world/bot.h"

namespace Kaim
{

/// BaseNavigationProfile and its derivation NavigationProfile<TraverseLogic> is a helper class that simplifies the customization of the navigation.
/// Enforces all PathFinding and PathFollowing customizations to use the same TraverseLogic.
/// Gathers explicitly the different customization interfaces that can be implemented in the PathFollower.
/// Makes a clear distinction between [classes that are sharable between bots] and [classes that are per Bot] transparent.
/// NavigationProfile is a very commonly used derivation of BaseNavigationProfile.
/// Most user customizations of BaseNavigationProfile are derivations of NavigationProfile<TraverseLogicT>.
template<class TLogic>
class NavigationProfile : public BaseNavigationProfile
{
public:
	typedef TLogic TraverseLogic;

	virtual Ptr<BaseAStarQuery>           CreateAStarQuery();
	virtual Ptr<BasePathProgressComputer> GetSharedPathProgressComputer();
	virtual Ptr<IPathEventListObserver>   GetSharedPathEventListObserver();
	virtual Ptr<IPositionOnPathValidator> GetSharedPositionOnPathValidator();
	virtual Ptr<ITrajectory>              CreateTrajectory();
	virtual Ptr<IAvoidanceComputer>       GetSharedAvoidanceComputer();
	virtual Ptr<IAvoidanceSolver>         GetSharedAvoidanceSolver();
	virtual Ptr<IAvoidanceFilter>         GetSharedAvoidanceFilter();
	virtual Ptr<BaseRayCanGoQuery>        CreateRayCanGoQuery();

	virtual Vec3f ComputeMoveOnNavMesh(Bot* bot, const Vec3f& velocity, KyFloat32 simulationTimeInSeconds);

public: // internal
	virtual bool HasTraverseLogicNoObsoleteFunctions() const;
	
public:
	Ptr<PathProgressComputer<TraverseLogic> >   m_pathProgressComputer;
	Ptr<DefaultPathEventListObserver>           m_defaultPathEventListObserver;
	Ptr<PositionOnPathCheckPointValidator>      m_positionOnPathCheckPointValidator;
	Ptr<AvoidanceComputer>                      m_avoidanceComputer;
	Ptr<AvoidanceSolver<TraverseLogic> > m_avoidanceSolver;
	// No IAvoidanceFilter by default
};


/// DefaultNavigationProfile
class DefaultNavigationProfile : public NavigationProfile<DefaultTraverseLogic>
{
};

}

#include "gwnavruntime/world/navigationprofile.inl"

#endif
