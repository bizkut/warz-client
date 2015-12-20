//=========================================================================
//	Module: AutodeskNavAgent.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#if ENABLE_AUTODESK_NAVIGATION

//////////////////////////////////////////////////////////////////////////

#include "AutodeskNavAgent.h"
#include "../../gameobjects/PhysXWorld.h"
#include "../../gameobjects/PhysObj.h"

#include "../AI_Brain.h"

using namespace Kaim;
extern r3dCamera gCam;

#if defined(WO_SERVER) && defined(ZOMBIE_PERFORMANCE_TESTING)
extern uint32_t	g_ZombieFrameBrainsCalcCount;
extern float	g_ZombieFrameCalculatingTime;
#endif

//////////////////////////////////////////////////////////////////////////

AutodeskNavAgent::AutodeskNavAgent()
	: m_status(Idle)
	, m_prevAvoidanceResult(AutodeskNavAgentEnums::NoAvoidance)
	, m_avoidanceResult(AutodeskNavAgentEnums::NoAvoidance)
	, m_arrivalPrecisionRadius(0.2f)
	, m_navBot(KY_NULL)
	, m_navPointOfInterest(NULL)
#ifdef NEW_AI
	, m_brain(KY_NULL)
#endif // NEW_AI
{
}

//////////////////////////////////////////////////////////////////////////

AutodeskNavAgent::~AutodeskNavAgent()
{
	if (m_navPointOfInterest)
		m_navPointOfInterest->RemoveFromWorld();
	m_navPointOfInterest = KY_NULL;

	if (m_navBot)
		m_navBot->RemoveFromDatabase();
	m_navBot = KY_NULL;
#ifdef NEW_AI
	m_brain = KY_NULL;
#endif // NEW_AI
}

//////////////////////////////////////////////////////////////////////////

bool AutodeskNavAgent::Init(Kaim::World* world, const r3dVector &pos, EPoiType poiType /* = AutodeskNavAgent::PoiUndefined */)
{
	r3d_assert(world);

	Kaim::BotInitConfig botInitConfig;
	botInitConfig.m_userData = this;
	botInitConfig.m_database = world->GetDatabase(0);
	botInitConfig.m_startPosition = Vec3f(pos.x, pos.z, pos.y);
	botInitConfig.m_startNewPathNavigationProfileId = (poiType == AutodeskNavAgent::PoiSuperZombie) ? gAutodeskNavMesh.m_NavProfileIdSuperZombie : 0;

	Kaim::BotConfig botConfig;
	if( poiType == AutodeskNavAgent::PoiSuperZombie )
		botConfig.m_enableAvoidance = false;
	else
		botConfig.m_enableAvoidance = true;
		
	// suddently in 2014.5.x force passaging started to work, so zombies now collapse into each other near player.
	botConfig.m_avoidanceConfig.m_enableForcePassage = false;

	m_navBot = *KY_NEW Kaim::Bot(botInitConfig, botConfig);
	m_navBot->SetMaxDesiredLinearSpeed(5.0f);
	m_navBot->AddToDatabase();

	//m_navBot->SetCurrentVisualDebugLOD(VisualDebugLOD_Maximal);

	if( poiType != AutodeskNavAgent::PoiUndefined )
	{
		Kaim::PointOfInterestInitConfig poiInitConfig;
		poiInitConfig.m_world = world;
		poiInitConfig.m_startPosition = Kaim::Vec3f(pos.x, pos.z, pos.y);
		poiInitConfig.m_poiType = poiType;

		m_navPointOfInterest = *KY_NEW Kaim::PointOfInterest;
		m_navPointOfInterest->Init(poiInitConfig);
		m_navPointOfInterest->AddToWorld();
		m_navPointOfInterest->SetUserData(this);
	}

#ifdef NEW_AI
	m_brain    = new AI_Brain();
#endif // NEW_AI

	m_position = botInitConfig.m_startPosition;
	m_velocity = Kaim::Vec3f(0.0f, 0.0f, 0.0f);

	return true;
}

//////////////////////////////////////////////////////////////////////////

AutodeskNavAgentEnums::EAvoidanceResult AutodeskNavAgent::GetAvoidanceResult()
{
	return m_avoidanceResult;
}

//////////////////////////////////////////////////////////////////////////

bool AutodeskNavAgent::IsAvoidanceResultChanged()
{
	return m_avoidanceResult != m_prevAvoidanceResult;
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavAgent::Move(KyFloat32 simulationTimeInSeconds)
{
	Kaim::BotUpdateConfig botUpdateConfig;
	botUpdateConfig.m_position        = m_navBot->GetPosition();        // by default keep the same position
	botUpdateConfig.m_frontDirection  = m_navBot->GetFrontDirection();  // by default keep the same moving direction
	botUpdateConfig.m_velocity        = Kaim::Vec3f(0.0f, 0.0f, 0.0f);  // ... but set speed to zero

	// if trajectory is disabled either by a SmartObject or a IGameBehavior which doesn't requires a MoveOnNavmesh
	if(m_navBot->GetDoComputeTrajectory() == false)
	{
		MoveOnNavGraph(simulationTimeInSeconds, m_velocity, botUpdateConfig);
		m_navBot->SetNextUpdateConfig(botUpdateConfig);
		return;
	}

	/*
	if(Keyboard->IsPressed(kbsT))
	{
		r3dOutToLog("GetTargetOnLivePathStatus(): %d\n", m_navBot->GetTargetOnLivePathStatus());
		r3dOutToLog("GetPathValidityStatus(): %d\n", m_navBot->GetLivePath().GetPathValidityStatus());
		r3dOutToLog("GetPathFinderResult(): %d %d\n", m_pathFinderQuery->GetPathFinderResult(), m_pathFinderQuery->GetResult());
		if(m_navBot->GetPathFinderQuery())
			r3dOutToLog("m_processStatus: %d\n", m_navBot->GetPathFinderQuery()->m_processStatus);
		r3dOutToLog("\n");
	}*/
	
	// Stop if no target yet, no path yet (or must be recomputed)
	if ((m_navBot->GetLivePath().GetPathValidityStatus() == Kaim::PathValidityStatus_NoPath) || 
		(m_navBot->GetProgressOnLivePathStatus() == Kaim::ProgressOnPathPending) ||
		(m_navBot->GetProgressOnLivePathStatus() == Kaim::ProgressOnPathNotInitialized) ||
		(m_navBot->GetProgressOnLivePathStatus() == Kaim::ProgressOnPathInInvalidNavData)) // not valid any more due to streamed out NavData.
	{
		m_navBot->SetNextUpdateConfig(botUpdateConfig);
		return;
	}

	// Compute the velocity (use inertia or not)
	m_velocity = m_navBot->GetTrajectory()->GetVelocity(); 
	m_status   = Moving;
	
	if (m_velocity.GetSquareLength() > 0.0f)
	{
		switch (m_navBot->GetProgressOnLivePath().GetPathEdgeType())
		{
			case Kaim::PathEdgeType_OnNavMesh:
			{
				MoveOnNavMesh(simulationTimeInSeconds, m_velocity, botUpdateConfig);
				break;
			}

			default:
			{
				MoveOnNavGraph(simulationTimeInSeconds, m_velocity, botUpdateConfig);
				break;
			}
		}
	}

	m_navBot->SetNextUpdateConfig(botUpdateConfig);
	m_position = botUpdateConfig.m_position;

	if (m_navPointOfInterest)
	{
		Kaim::PointOfInterestUpdateConfig poiUpdateConfig;
		poiUpdateConfig.m_position = m_position;
		m_navPointOfInterest->SetNextUpdateConfig(poiUpdateConfig);
	}
}

bool AutodeskNavAgent::MoveOnNavMesh(KyFloat32 simulationTimeInSeconds, const Kaim::Vec3f& velocity, Kaim::BotUpdateConfig& botUpdateConfig)
{
	// Keep current position. But don't take reference, we will need this value after it has been changed.
	const Kaim::Vec3f previousPosition = botUpdateConfig.m_position;

	// compute new position
	botUpdateConfig.m_position = m_navBot->ComputeMoveOnNavMesh(velocity, simulationTimeInSeconds);
	botUpdateConfig.m_velocity = (botUpdateConfig.m_position - previousPosition) / simulationTimeInSeconds;
	
	return true;
}

void AutodeskNavAgent::MoveOnNavGraph(KyFloat32 simulationTimeInSeconds, const Kaim::Vec3f& velocity, Kaim::BotUpdateConfig& botUpdateConfig)
{
	botUpdateConfig.m_velocity.Set(0.0f, 0.0f, 0.0f);
	if(velocity.GetSquareLength() > 0.0f)
	{
		botUpdateConfig.m_velocity = velocity;
		botUpdateConfig.m_frontDirection = velocity;
		botUpdateConfig.m_frontDirection.Normalize();
	}
	botUpdateConfig.m_position += velocity * simulationTimeInSeconds;
}

void AutodeskNavAgent::HandleArrivalAndUpperBound()
{
	if (m_navBot->GetFollowedPath() == KY_NULL)
		return;

	// Prevent underflow in current PathEvent index, from 
	// GetUpperBoundType(), when the navBot is at the
	// beginning of the path.
	if (m_navBot->GetProgressOnLivePath().IsAtFirstNodeOfPath())
		return;

	switch(m_navBot->GetLivePath().GetUpperBoundType())
	{
		case Kaim::PathLastNode:
		{
			HandleArrival();
			break;
		}

		case Kaim::ValidityUpperBound:
		{
			HandleUpperBound();
			break;
		}

		case Kaim::ValidityTemporaryUpperBound:
			break; // Do nothing, just wait for time-sliced validity check completion
	}
}

void AutodeskNavAgent::HandleArrival()
{
	r3d_assert(m_navBot->GetFollowedPath() != KY_NULL);

	const Kaim::PositionOnLivePath& progressOnPath = m_navBot->GetProgressOnLivePath();
	if (progressOnPath.IsAtLastEdgeOfPath() == false)
		return;

	if (m_navBot->HasReachedPosition(m_targetPos, m_arrivalPrecisionRadius))
	{
#ifdef NEW_AI
		if (AI_EBrainStates::Executing == m_brain->m_State && m_brain->Update() && m_brain->m_TargetChanged)
		{
			m_targetPos = Kaim::Vec3f( m_brain->m_TargetPos.x, m_brain->m_TargetPos.z, m_brain->m_TargetPos.y );
			if(ComputeNewPathToDestination(m_targetPos) == KY_ERROR)
			{
				m_status = Failed;
			}
			m_status = ComputingPath;
			return;
		}
#endif // NEW_AI
		m_navBot->ClearFollowedPath();
		m_status = Arrived;
		m_velocity = Kaim::Vec3f(0.0f, 0.0f, 0.0f);
	}
}

void AutodeskNavAgent::HandleUpperBound()
{
	KY_TODO("rename m_validityCheckDistance to a meaningful name")
	KY_TODO("COMPUTE THE length along the path instead of the square dist")
	float m_validityCheckDistance = 1.6f;

	const Kaim::Vec3f& livePathUpperBoundPosition = m_navBot->GetPathEventList().GetLastPathEvent().m_positionOnPath.GetPosition();
	if (Kaim::SquareDistance(m_navBot->GetPosition(), livePathUpperBoundPosition) < m_validityCheckDistance * m_validityCheckDistance)
	{
		// we don't know what the that "UpperBound" yet - need to investigate!
		r3dOutToLog("!!!!! got HandleUpperBound %f,%f,%f pos: %f,%f,%f\n", 
			livePathUpperBoundPosition.x, livePathUpperBoundPosition.z, livePathUpperBoundPosition.y, 
			m_position.x, m_position.z, m_position.y);
		m_status = Failed;
	}
}

void AutodeskNavAgent::Update(float timeStep)
{
	// computing path logic
	if(m_status == ComputingPath)
	{
		float calculatingPathTime = r3dGetTime();
#ifdef NEW_AI
		if( m_brain != KY_NULL )
		{
			switch( m_brain->m_State )
			{
			case AI_EBrainStates::Idle:
			case AI_EBrainStates::Evaluation:
				if( m_brain->Update() && m_brain->m_State == AI_EBrainStates::Executing )
				{
					m_targetPos = Kaim::Vec3f( m_brain->m_TargetPos.x, m_brain->m_TargetPos.z, m_brain->m_TargetPos.y );
					if(ComputeNewPathToDestination(m_targetPos) == KY_ERROR)
					{
						m_status = Failed;
					}
				}
				break;

			default:
				break;
			}
		}
#endif // NEW_AI

		IPathFinderQuery* pathQuery = m_navBot->GetPathFinderQuery();
		if ( pathQuery 
#ifdef NEW_AI
			&& m_brain->m_State == AI_EBrainStates::Executing 
#endif // NEW_AI
			)
		{
			if( pathQuery->m_processStatus == Kaim::QueryCanceled)
			{
#ifdef NEW_AI
				if( m_brain != KY_NULL )
					m_brain->Cancel();
#endif // NEW_AI
				m_status = Idle;
			}
			else if (pathQuery->m_processStatus == Kaim::QueryDone)
			{
				m_status = (m_navBot->GetFollowedPath() != KY_NULL) ? Moving : PathNotFound;
			}
		}

#if defined(WO_SERVER) && defined(ZOMBIE_PERFORMANCE_TESTING)
		g_ZombieFrameCalculatingTime += r3dGetTime() - calculatingPathTime;
#endif
	}

	// sanity check for statuses
	switch(m_status)
	{
		default: r3dError("bad status %d", m_status);

		case Moving:
			m_prevAvoidanceResult = m_avoidanceResult;
			if( m_navBot->GetTrajectory() )
				m_avoidanceResult = (AutodeskNavAgentEnums::EAvoidanceResult)m_navBot->GetTrajectory()->GetAvoidanceResult();
			break;

		case ComputingPath:
		case Idle:
		case Arrived:
		case PathNotFound:
		case Failed:
			return;
	}

	// Check arrival & upper bound
	HandleArrivalAndUpperBound();

	Move(timeStep);

	return;
}

bool AutodeskNavAgent::StartMove(const r3dPoint3D &target, float maxAstarDist)
{
	StopMove();
	
	m_targetPos = R3D_KY(target);

	/*
	r3dOutToLog("bot pos: %f %f %f\n", m_navBot->GetPosition().x, m_navBot->GetPosition().y, m_navBot->GetPosition().z);	
	r3dOutToLog("bot trg: %f %f %f\n", m_targetPos.x, m_targetPos.y, m_targetPos.z);
	Kaim::TriangleFromPosQuery query;
	query.Initialize(m_navBot->GetDatabase(), m_targetPos);
	query.PerformQuery();
	r3dOutToLog("TriangleFromPosQuery, result:%d, triidx:%d query.GetAltitudeOfProjectionInTriangle():%f\n", query.GetResult(), query.GetResultTrianglePtr().GetTriangleIdx(), query.GetAltitudeOfProjectionInTriangle());
	*/

	// set astar distance limit
	Kaim::PathFinderConfig pfConfig = m_navBot->GetConfig().m_pathFinderConfig;
	pfConfig.m_propagationBoxExtent = maxAstarDist;
	m_navBot->SetPathFinderConfig(pfConfig);
#ifdef NEW_AI
	if( m_brain != KY_NULL )
	{
		m_brain->Cancel();
		m_brain->m_TacticParams.m_StartPos = GetPosition();
		m_brain->m_TacticParams.m_DestPos = target;
		if( m_brain->Update() && m_brain->m_State == AI_EBrainStates::Executing )
		{
			m_targetPos = Kaim::Vec3f( m_brain->m_TargetPos.x, m_brain->m_TargetPos.z, m_brain->m_TargetPos.y );
			if(ComputeNewPathToDestination(m_targetPos) == KY_ERROR)
			{
				m_status = Failed;
				return false;
			}
		}
#if defined(WO_SERVER) && defined(ZOMBIE_PERFORMANCE_TESTING)
		else
		{
			// Decrement the count, because the zombie will get a second
			// call to m_brain->Update in the normal Update loop.
			--g_ZombieFrameBrainsCalcCount;
		}
#endif
	}
	else
#endif // NEW_AI
	{
		if(ComputeNewPathToDestination(m_targetPos) == KY_ERROR)
		{
			m_status = Failed;
			return false;
		}
	}
	
	m_status = ComputingPath;
	return true;
}

void AutodeskNavAgent::StopMove()
{
	if (m_navBot->IsComputingNewPath())
		m_navBot->CancelAsyncPathComputation();

	if (m_navBot->GetFollowedPath() != KY_NULL)
		m_navBot->ClearFollowedPath();

	m_status = Idle;
}

void AutodeskNavAgent::SetTargetSpeed(float speed)
{
	m_navBot->SetMaxDesiredLinearSpeed(speed);
}


//////////////////////////////////////////////////////////////////////////

r3dVector AutodeskNavAgent::GetPosition() const
{
	return r3dVector(m_position.x, m_position.z, m_position.y);
}

//////////////////////////////////////////////////////////////////////////

#ifndef FINAL_BUILD
#ifndef WO_SERVER
void AutodeskNavAgent::DebugDraw()
{
	r3dBoundBox bb;
	bb.Org = GetPosition();
	bb.Size.Assign(m_navBot->GetRadius() * 2, m_navBot->GetHeight(), m_navBot->GetRadius() * 2);
	bb.Org.x -= bb.Size.x / 2;
	bb.Org.z -= bb.Size.z / 2;
	r3dDrawBoundBox(bb, gCam, r3dColor::white, 0.02f);

	r3dPoint3D scrCoord;
	if(r3dProjectToScreen(GetPosition() + r3dPoint3D(0, 1.8f, 0), &scrCoord))
	{
		extern CD3DFont* Font_Editor;
		r3dRenderer->SetRenderingMode(R3D_BLEND_PUSH | R3D_BLEND_NZ);
		Font_Editor->PrintF(scrCoord.x, scrCoord.y, r3dColor(255,255,255), "%d %d %d %d",
			m_status, 
			m_navBot->GetProgressOnLivePathStatus(),
			m_navBot->GetLivePath().GetPathValidityStatus(),
			m_navBot->GetTrajectory() ? m_navBot->GetTrajectory()->m_avoidanceResult : -1);
		r3dRenderer->SetRenderingMode(R3D_BLEND_POP);
	}
}
#endif
#endif

//////////////////////////////////////////////////////////////////////////

KyResult AutodeskNavAgent::ComputeNewPathToDestination(const Vec3f& destPos, const NavGraphEdgePtr& startNavGraphEdgePtr, NavGraphEdgeDirection navGraphEdgePathfindMode /* = Kaim::NAVGRAPHEDGE_BIDIRECTIONAL */)
{
	if (m_navBot->IsComputingNewPath())
		m_navBot->CancelAsyncPathComputation();

	// get an astarQuery that adheres to the NavigationProfile m_newPathToDestNavigationProfileId
	Ptr<BaseAStarQuery> astarQuery = m_navBot->m_botNavigationCollection.GetOrCreateAStarQuery(m_navBot->m_newPathToDestNavigationProfileId);
	if (astarQuery == KY_NULL)
		return KY_ERROR;

	if (m_navBot->InitAStarQueryForBot(astarQuery, destPos, startNavGraphEdgePtr, navGraphEdgePathfindMode) == KY_ERROR)
		return KY_ERROR;

	astarQuery->SetToOutsideNavMeshDistance(0.0f);
	astarQuery->SetTryCanGoMode(ASTAR_DO_TRY_RAYCANGO);

	// increase all limits for astar
	//astarQuery->SetNumberOfProcessedNodePerFrame(astarQuery->GetNumberOfProcessedNodePerFrame() * 500);
	astarQuery->m_traversalParams.m_numberOfVisitedNodePerFrame = 30 * 500;
	astarQuery->m_traversalParams.m_numberOfCanGoTestInRefinerPerFrame = 15 * 500;
	astarQuery->m_traversalParams.m_numberOfIntersectionTestPerFrame = 100 * 500;
	/*
	 m_numberOfVisitedNodePerFrame = 30;
	 m_numberOfCanGoTestInRefinerPerFrame = 15;
	 m_numberOfIntersectionTestPerFrame = 100;
	*/
	
	m_pathStartTime = r3dGetTime();
	
	m_navBot->ComputeNewPathAsync(astarQuery);
	return KY_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////

#endif
