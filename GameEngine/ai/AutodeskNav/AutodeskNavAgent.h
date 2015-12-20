//=========================================================================
//	Module: AutodeskNavAgent.h
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#pragma once

namespace AutodeskNavAgentEnums
{
	enum EAvoidanceResult
	{
		NoAvoidance			= 0, //Kaim::IAvoidanceComputer::NoAvoidance,
		SlowDown			= 1, //Kaim::IAvoidanceComputer::SlowDown,
		Accelerate			= 2, //Kaim::IAvoidanceComputer::Accelerate,
		Turn				= 3, //Kaim::IAvoidanceComputer::Turn,
		Stop				= 4, //Kaim::IAvoidanceComputer::Stop,
		SolutionNotFound	= 5, //Kaim::IAvoidanceComputer::SolutionNotFound,
	};
}

#if ENABLE_AUTODESK_NAVIGATION

//////////////////////////////////////////////////////////////////////////

#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/world/bot.h"
#include "gwnavruntime/queries/astarquery.h"
#include "gwnavruntime/queries/raycastquery.h"
#include "gwnavruntime/queries/moveonnavmeshquery.h"
#include "gwnavruntime/queries/trianglefromposquery.h"
#include "gwnavruntime/queries/insideposfromoutsideposquery.h"

#include "AutodeskNavMesh.h"

class AI_Brain;

class AutodeskNavAgent
{
public:
	enum EPoiType
	{
		PoiUndefined = Kaim::PointOfInterestType_Undefined,
		PoiZombie = Kaim::PointOfInterestType_FirstClient,
		PoiSuperZombie,
	};
	enum EStatus
	{
		Idle = 0,
		ComputingPath,
		Moving,
		Arrived,
		PathNotFound,
		Failed,
	};
	EStatus		m_status;
	AutodeskNavAgentEnums::EAvoidanceResult m_prevAvoidanceResult;
	AutodeskNavAgentEnums::EAvoidanceResult m_avoidanceResult;

	Kaim::Vec3f	m_velocity;
	Kaim::Vec3f	m_position;
	Kaim::Vec3f	m_targetPos;
	float		m_arrivalPrecisionRadius;
	float		m_pathStartTime;

	Kaim::Ptr<Kaim::Bot> m_navBot;
	Kaim::Ptr<Kaim::PointOfInterest> m_navPointOfInterest;

	AI_Brain*	m_brain;
	
	AutodeskNavAgentEnums::EAvoidanceResult GetAvoidanceResult();
	bool IsAvoidanceResultChanged();

	void Move(KyFloat32 simulationTimeInSeconds);
	void  MoveOnNavGraph(KyFloat32 simulationTimeInSeconds, const Kaim::Vec3f& velocity, Kaim::BotUpdateConfig& botUpdateConfig);
	bool  MoveOnNavMesh(KyFloat32 simulationTimeInSeconds, const Kaim::Vec3f& velocity, Kaim::BotUpdateConfig& botUpdateConfig);
	
	void HandleArrivalAndUpperBound();
	void  HandleArrival();
	void  HandleUpperBound();

protected:
	// Re-writes of Kaim::Bot methods, so that we have access to the AStarQuery prior to the call to ComputeNewPathAsync(),
	// which allows us to modify the AStarQuery::m_traversalParams.
	KyResult ComputeNewPathToDestination(const Kaim::Vec3f& destPos);
	KyResult ComputeNewPathToDestination(const Kaim::Vec3f& destPos, const Kaim::NavGraphEdgePtr& startNavGraphEdgePtr, Kaim::NavGraphEdgeDirection navGraphEdgePathfindMode = Kaim::NAVGRAPHEDGE_BIDIRECTIONAL);

public:
	AutodeskNavAgent();
	virtual ~AutodeskNavAgent();

	bool Init(Kaim::World* world, const r3dVector &pos, EPoiType poiType = AutodeskNavAgent::PoiUndefined);
	void Update(float timeStep);
	bool StartMove(const r3dPoint3D &target, float maxAstarDist = 999999);
	void StopMove();
	void SetTargetSpeed(float speed);
	r3dVector GetPosition() const;
#ifndef FINAL_BUILD
	void DebugDraw();
#endif
}; 

__forceinline KyResult AutodeskNavAgent::ComputeNewPathToDestination(const Kaim::Vec3f& destPos) { return ComputeNewPathToDestination(destPos, Kaim::NavGraphEdgePtr()); }

#endif
