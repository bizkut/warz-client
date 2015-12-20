/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NONE
#include "gwnavruntime/navmesh/traverselogicobsoletechecker.h"

namespace Kaim
{

template<class TLogic>
inline bool NavigationProfile<TLogic>::HasTraverseLogicNoObsoleteFunctions() const
{
	return TraverseLogicObsoleteChecker::HasNoObsoleteFunctions<TraverseLogic>();
}

template<class TLogic>
inline Ptr<BaseAStarQuery> NavigationProfile<TLogic>::CreateAStarQuery()
{
	Ptr<BaseAStarQuery> aStarQuery = *KY_NEW AStarQuery<TraverseLogic>();
	aStarQuery->SetNavigationProfileId(m_profileId);
	return aStarQuery;
}

template<class TLogic>
inline Ptr<BasePathProgressComputer> NavigationProfile<TLogic>::GetSharedPathProgressComputer()
{
	if (m_pathProgressComputer == KY_NULL)
	{
		m_pathProgressComputer = *KY_NEW PathProgressComputer<TraverseLogic>();
		m_pathProgressComputer->SetPathEventListObserver(GetSharedPathEventListObserver());
		m_pathProgressComputer->SetPositionOnPathCheckPointValidator(GetSharedPositionOnPathValidator());
	}
	return m_pathProgressComputer;
}

template<class TLogic>
inline Ptr<IPathEventListObserver> NavigationProfile<TLogic>::GetSharedPathEventListObserver()
{
	if (m_defaultPathEventListObserver == KY_NULL)
		m_defaultPathEventListObserver = *KY_NEW DefaultPathEventListObserver();
	return m_defaultPathEventListObserver;
}

template<class TLogic>
inline Ptr<IPositionOnPathValidator> NavigationProfile<TLogic>::GetSharedPositionOnPathValidator()
{
	if (m_positionOnPathCheckPointValidator == KY_NULL)
		m_positionOnPathCheckPointValidator = *KY_NEW PositionOnPathCheckPointValidator();
	return m_positionOnPathCheckPointValidator;
}

template<class TLogic>
inline Ptr<ITrajectory> NavigationProfile<TLogic>::CreateTrajectory()
{
	Ptr<Trajectory<TraverseLogic> > trajectory = *KY_NEW Trajectory<TraverseLogic>;
	trajectory->SetAvoidanceComputer(GetSharedAvoidanceComputer().GetPtr());
	return trajectory;
}

template<class TLogic>
inline Ptr<IAvoidanceComputer> NavigationProfile<TLogic>::GetSharedAvoidanceComputer()
{
	if (m_avoidanceComputer == KY_NULL)
	{
		m_avoidanceComputer = *KY_NEW AvoidanceComputer();
		m_avoidanceComputer->SetAvoidanceSolver(GetSharedAvoidanceSolver());
		m_avoidanceComputer->SetAvoidanceFilter(GetSharedAvoidanceFilter());
	}
	return m_avoidanceComputer;
}

template<class TLogic>
inline Ptr<IAvoidanceSolver> NavigationProfile<TLogic>::GetSharedAvoidanceSolver()
{
	if (m_avoidanceSolver == KY_NULL)
		m_avoidanceSolver = *KY_NEW AvoidanceSolver<TraverseLogic>();
	return m_avoidanceSolver;
}

template<class TLogic>
inline Ptr<IAvoidanceFilter> NavigationProfile<TLogic>::GetSharedAvoidanceFilter()
{
	return KY_NULL; // No Avoidance filter by default
}

template<class TLogic>
inline Ptr<BaseRayCanGoQuery> NavigationProfile<TLogic>::CreateRayCanGoQuery()
{
	return *KY_NEW RayCanGoQuery<TraverseLogic>;
}

template<class TLogic>
inline Vec3f NavigationProfile<TLogic>::ComputeMoveOnNavMesh(Bot* bot, const Vec3f& velocity, KyFloat32 simulationTimeInSeconds)
{
	return bot->ComputeMoveOnNavMeshWithTraverseLogic<TraverseLogic>(velocity, simulationTimeInSeconds);
}

}
