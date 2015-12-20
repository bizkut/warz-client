//=========================================================================
//	Module: AI_Brain.cpp
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#ifndef FINAL_BUILD

#include "AI_Brain.h"
#include "AutodeskNav/AutodeskNavAgent.h"

#if defined(WO_SERVER) && defined(ZOMBIE_PERFORMANCE_TESTING)
extern	uint32_t	g_ZombieFrameBrainsCalcCount;
extern	float		g_ZombieFrameBrainsCalculatingTime;
#endif

AI_BrainConfig::AI_BrainConfig( uint32_t id )
	: ID( id )
	, NumBrainGoals( 0 )
	, BrainGoals( NULL )
	, DefaultGoal( NULL )
{ }

AI_BrainConfig::~AI_BrainConfig()
{
	Destroy();
}

void AI_BrainConfig::Destroy()
{
	Name[0]  = _T('\0');
	DefaultGoal = NULL;
	if( BrainGoals )
	{
		for( uint32_t i = 0; i < NumBrainGoals; ++i )
		{
			if( BrainGoals[ i ].Priorities )
			{
				for( uint32_t j = 0; j < BrainGoals[ i ].NumPriorities; ++j )
				{
					if( BrainGoals[ i ].Priorities[ j ] )
					{
						delete BrainGoals[ i ].Priorities[ j ];
						BrainGoals[ i ].Priorities[ j ] = NULL;
					}
				}
				delete [] BrainGoals[ i ].Priorities;
				BrainGoals[ i ].Priorities =  NULL;
				BrainGoals[ i ].NumPriorities = 0;
			}
		}
		delete [] BrainGoals;
		BrainGoals = NULL;
		NumBrainGoals = 0;
	}
}

//---------------------------------------------------------

AI_BrainProfiles* g_pAIBrainProfiles = NULL;

AI_BrainProfiles::AI_BrainProfiles()
{
}

AI_BrainProfiles::~AI_BrainProfiles()
{
	Destroy();
}

void AI_BrainProfiles::Init()
{
	r3d_assert( m_brainConfigsHash.Size() == 0 );

	// load A.I. Brain Profiles
	{
		const char* AiDBFile = "Data/AIDB.xml";
		
		r3dFile* f = r3d_open(AiDBFile, "rb");
		if ( !f )
		{
			r3dError("Failed to open %s\n", AiDBFile);
			return;
		}

		char* fileBuffer = game_new char[f->size + 1];
		r3d_assert(fileBuffer);
		fread(fileBuffer, f->size, 1, f);
		fileBuffer[f->size] = 0;
		pugi::xml_document xmlFile;
		pugi::xml_parse_result parseResult = xmlFile.load_buffer_inplace(fileBuffer, f->size);
		fclose(f);
		if(!parseResult)
			r3dError("Failed to parse XML, error: %s", parseResult.description());
		pugi::xml_node xmlDB = xmlFile.child("DB");
		{
			pugi::xml_node xmlBrainDB = xmlDB.child("BrainDB");
			pugi::xml_node xmlBrain = xmlBrainDB.child("Brain");
			while(!xmlBrain.empty())
			{
				loadBrainConfig(xmlBrain);
				xmlBrain = xmlBrain.next_sibling();
			}
		}
		delete [] fileBuffer;
	}
}

void AI_BrainProfiles::Destroy()
{
	m_brainConfigsHash.IterateStart();
	while( m_brainConfigsHash.IterateNext() )
	{
		AI_BrainConfig* brainConfig = m_brainConfigsHash.IterateGet();
		brainConfig->Destroy();
	}
	m_brainConfigsHash.Clear();
	m_brainConfigHashKeys.clear();
}

void AI_BrainProfiles::loadBrainConfig( pugi::xml_node& xmlConfig )
{
	r3d_assert( !xmlConfig.empty() );

	uint32_t brainID = xmlConfig.attribute("brainID").as_uint();
	// check if we have that brain configuration in our database
	{
		AI_BrainConfig* temp = NULL;
		if( m_brainConfigsHash.GetObject( brainID, &temp ) )
		{
			r3dOutToLog("!!! Trying to load a brain configuration with id '%d' that is already loaded!", brainID);
			return;
		}
	}
	AI_BrainConfig* brainConfig = game_new AI_BrainConfig( brainID );
	r3d_assert(brainConfig);

	_tcsncpy_s(brainConfig->Name, AI_BrainConfig::NameLen, xmlConfig.attribute("Name").value(), AI_BrainConfig::NameLen - 1);

	// Get the Goals.
	brainConfig->NumBrainGoals = xmlConfig.attribute("NumGoals").as_uint();
	if( 0 < brainConfig->NumBrainGoals )
	{
		brainConfig->BrainGoals = game_new AI_BrainGoal[ brainConfig->NumBrainGoals ];
		r3d_assert(brainConfig->BrainGoals);

		uint32_t goalCount = 0;
		pugi::xml_node xmlGoal = xmlConfig.child("Goal");
		while( !xmlGoal.empty() )
		{
			AI_BrainGoal& brainGoal = brainConfig->BrainGoals[ goalCount ];
			brainGoal.TacticID = xmlGoal.attribute("TacticID").as_uint();
			bool isDefault = xmlGoal.attribute("Default").as_bool();
			if( isDefault )
			{
#ifndef FINAL_BUILD
				if( brainConfig->DefaultGoal )
					r3dError("Can't assign more than one default goal to a brain configuration.\n");
#endif // FINAL_BUILD
				brainConfig->DefaultGoal = &brainConfig->BrainGoals[ goalCount ];
			}

			// Get the Priorities.
			brainGoal.NumPriorities = xmlGoal.attribute("NumPriorities").as_uint();
			//r3dOutToLog("Goal - TacticID: %d  NumPriorities: %d\n", brainGoal.TacticID, brainGoal.NumPriorities );
			if( 0 < brainGoal.NumPriorities )
			{
				brainGoal.Priorities = game_new AI_BrainPriority*[ brainGoal.NumPriorities ];
				r3d_assert( brainGoal.Priorities );

				uint32_t priorityCount = 0;

				// Get the Distance Priorities.
				pugi::xml_node xmlPriority = xmlGoal.child("Distance");
				while( !xmlPriority.empty() )
				{
					AI_DistancePriority* distPriority = game_new AI_DistancePriority;
					r3d_assert( distPriority );
					brainGoal.Priorities[ priorityCount ] = (AI_BrainPriority*)distPriority;

					loadDistancePriority( xmlPriority, distPriority );
					xmlPriority = xmlPriority.next_sibling("Distance");
					++priorityCount;
				}

				// Get the Health Priorities.
				xmlPriority = xmlGoal.child("Health");
				while( !xmlPriority.empty() )
				{
					AI_HealthPriority* healthPriority = game_new AI_HealthPriority;
					r3d_assert( healthPriority );
					brainGoal.Priorities[ priorityCount ] = (AI_BrainPriority*)healthPriority;

					loadHealthPriority( xmlPriority, healthPriority );
					xmlPriority = xmlPriority.next_sibling("Health");
					++priorityCount;
				}

				// Get the Group Priorities.
				xmlPriority = xmlGoal.child("Group");
				while( !xmlPriority.empty() )
				{
					AI_GroupPriority* groupPriority = game_new AI_GroupPriority;
					r3d_assert( groupPriority );
					brainGoal.Priorities[ priorityCount ] = (AI_BrainPriority*)groupPriority;

					loadGroupPriority( xmlPriority, groupPriority );
					xmlPriority = xmlPriority.next_sibling("Group");
					++priorityCount;
				}


				//r3dOutToLog("PriorityCount: %d  NumPriorities: %d\n", priorityCount, brainGoal.NumPriorities );
				r3d_assert( priorityCount == brainGoal.NumPriorities );
			}
			xmlGoal = xmlGoal.next_sibling();
			++goalCount;
		}
		r3d_assert( goalCount == brainConfig->NumBrainGoals );
	}

	// Save the Brain Config to the hash.
	m_brainConfigsHash.Add( brainID, brainConfig );
	m_brainConfigHashKeys.push_back( brainID );
}

void AI_BrainProfiles::loadBasePriority( pugi::xml_node& xmlPriority, AI_BrainPriority* priority )
{
	r3d_assert( !xmlPriority.empty() && priority );

	priority->Priority = xmlPriority.attribute( "Priority" ).as_uint();
}

void AI_BrainProfiles::loadDistancePriority( pugi::xml_node& xmlPriority, AI_DistancePriority* priority )
{
	r3d_assert( !xmlPriority.empty() && priority );

	priority->Type = AI_EBrainPriorityTypes::Distance;
	priority->Max  = xmlPriority.attribute( "Max" ).as_float();
	priority->Min  = xmlPriority.attribute( "Min" ).as_float();

	loadBasePriority( xmlPriority, (AI_BrainPriority*)priority );

	//r3dOutToLog("Max: %0.f  Min: %0.f  Priority: %d\n", priority->Max, priority->Min, priority->Priority );
}

void AI_BrainProfiles::loadHealthPriority( pugi::xml_node& xmlPriority, AI_HealthPriority* priority )
{
	r3d_assert( !xmlPriority.empty() && priority );

	priority->Type = AI_EBrainPriorityTypes::Health;
	priority->Max  = xmlPriority.attribute( "Max" ).as_float();
	priority->Min  = xmlPriority.attribute( "Min" ).as_float();

	loadBasePriority( xmlPriority, (AI_BrainPriority*)priority );

	//r3dOutToLog("Max: %0.f  Min: %0.f  Priority: %d\n", priority->Max, priority->Min, priority->Priority );
}

void AI_BrainProfiles::loadGroupPriority( pugi::xml_node& xmlPriority, AI_GroupPriority* priority )
{
	r3d_assert( !xmlPriority.empty() && priority );

	priority->Type = AI_EBrainPriorityTypes::Group;
	priority->Max  = xmlPriority.attribute( "Max" ).as_float();
	priority->Min  = xmlPriority.attribute( "Min" ).as_float();

	loadBasePriority( xmlPriority, (AI_BrainPriority*)priority );

	//r3dOutToLog("Max: %0.f  Min: %0.f  Priority: %d\n", priority->Max, priority->Min, priority->Priority );
}


uint32_t AI_BrainProfiles::GetRandomBrainConfigID() const
{
	uint32_t configNdx = u_random( m_brainConfigHashKeys.size() );
	r3d_assert( configNdx < m_brainConfigHashKeys.size() );
	
	return m_brainConfigHashKeys[ configNdx ];
}

const AI_BrainConfig* AI_BrainProfiles::GetBrainConfig( uint32_t brainConfigID ) const
{
	AI_BrainConfig* brainConfig = NULL;
	m_brainConfigsHash.GetObject( brainConfigID, &brainConfig );

	return brainConfig;
}

const r3dgameVector(uint32_t)& AI_BrainProfiles::GetBrainConfigIDs() const
{
	return m_brainConfigHashKeys;
}

//---------------------------------------------------------

const r3dPoint3D AI_Brain::NonTarget(-9999, -9999, -9999);

AI_Brain::AI_Brain()
	: m_BrainConfigID( UINT_MAX )
	, m_Timer( -1.0f )
	, m_AccumTime( 0.0f )
#ifndef NEW_AI
	, m_MaxTime( -1.0f )
#endif // NEW_AI
	, m_State( AI_EBrainStates::Idle )
	, m_TargetPos( NonTarget )
	, m_EvalGoalNdx( UINT_MAX )
#ifndef NEW_AI
	, m_EvalTacticHashKeyNdx( UINT_MAX )
#endif // NEW_AI
{
	m_TacticParams.m_TacticConfig = NULL;
	m_TacticParams.m_Scale = 0.0f;
	m_TacticParams.m_Angle = 0.0f;
	m_TacticParams.m_Axis = NonTarget;
	m_TacticParams.m_StartPos = NonTarget;
	m_TacticParams.m_DestPos = NonTarget;
	m_TopPick.Goal = NULL;
	m_TopPick.Priority = 0;
}

AI_Brain::~AI_Brain()
{
}

void AI_Brain::Cancel()
{
	m_Timer = -1.0f;
	m_AccumTime = 0.0f;
#ifndef NEW_AI
	m_MaxTime = -1.0f;
#endif // NEW_AI
	m_TacticParams.m_TacticConfig = NULL;
	m_TacticParams.m_Scale = 0.0f;
	m_TacticParams.m_Angle = 0.0f;
	m_TacticParams.m_Axis = NonTarget;
	m_TacticParams.m_StartPos = NonTarget;
	m_TacticParams.m_DestPos = NonTarget;
	m_TacticParams.m_CurrentCheckpointNdx = UINT_MAX;
	SwitchToState( AI_EBrainStates::Idle );
	m_TargetPos = NonTarget;
	m_EvalGoalNdx = UINT_MAX;
#ifndef NEW_AI
	m_EvalTacticHashKeyNdx = UINT_MAX;
#endif // NEW_AI
	m_TopPick.Goal = NULL;
	m_TopPick.Priority = 0;
}

void AI_Brain::SwitchToState(AI_EBrainStates::AI_EBrainState in_state)
{
//	r3dOutToLog( " Switching AI_Brain from %s -> %s\n", AI_EBrainStates::aStateNames[ m_State ].c_str(), AI_EBrainStates::aStateNames[ in_state ].c_str() );
	m_State = in_state;
}

bool AI_Brain::Update()
{
	switch( m_State )
	{
	case AI_EBrainStates::Idle:
		return UpdateIdle();
	case AI_EBrainStates::Evaluation:
		return UpdateEvaluation();
	case AI_EBrainStates::Executing:
		return UpdateExecuting();
	default:
		return false;
	}
}

bool AI_Brain::UpdateIdle()
{
	if( !( m_TacticParams.m_StartPos.AlmostEqual( NonTarget ) ||
		   m_TacticParams.m_DestPos.AlmostEqual( NonTarget ) ) )
	{
		SwitchToState( AI_EBrainStates::Evaluation );
		return UpdateEvaluation();
	}
	return false;
}

bool AI_Brain::UpdateEvaluation( float approxTimeLimit /* = 0.1f */ )
{
	// What type of match should be made?
	//   First Match (first tactic found to match between Max and Min?)
	//   Best Match (Highest Priority?)
	// Currently using Best Match.

#if defined(WO_SERVER) && defined(ZOMBIE_PERFORMANCE_TESTING)
	++g_ZombieFrameBrainsCalcCount;
#endif

	float evalStartTime = r3dGetTime();
	float evalTimer = evalStartTime;
#ifdef NEW_AI
	if( UINT_MAX == m_BrainConfigID )
		m_BrainConfigID = g_pAIBrainProfiles->GetRandomBrainConfigID();
	const AI_BrainConfig* brainConfig = g_pAIBrainProfiles->GetBrainConfig( m_BrainConfigID );
	r3d_assert( brainConfig );
	while( approxTimeLimit < 0.0f || approxTimeLimit > ( r3dGetTime() - evalTimer ) )
	{
		if( UINT_MAX == m_EvalGoalNdx )
			m_EvalGoalNdx = 0;

		uint32_t distVal = 0;
		uint32_t healthVal = 0;
		uint32_t groupVal = 0;
		AI_BrainGoal& brainGoal = brainConfig->BrainGoals[ m_EvalGoalNdx ];

//		AI_TacticConfig* pTactic = NULL;
//		g_pAITactics->m_tacticsHash.GetObject( brainGoal.TacticID, &pTactic );
//		r3dOutToLog("Evaluating: %s  Index: %d\n", pTactic->m_Name, m_EvalGoalNdx);

		uint32_t numAgentsGroupPriority = UINT_MAX;
		for( uint32_t priorityNdx = 0; priorityNdx < brainGoal.NumPriorities; ++ priorityNdx )
		{
			AI_BrainPriority* priority = brainGoal.Priorities[ priorityNdx ];
			switch( priority->Type )
			{
			case AI_EBrainPriorityTypes::Distance:
				{
					// Use distance squared to avoid squareroot computations.
					float distSq = ( m_TacticParams.m_DestPos - m_TacticParams.m_StartPos ).LengthSq();
					AI_DistancePriority* distPriority = (AI_DistancePriority*)priority;
					float maxSq = (float)(distPriority->Max * distPriority->Max);
					float minSq = (float)(distPriority->Min * distPriority->Min);
					if( minSq <= distSq && maxSq >= distSq && distVal < distPriority->Priority )
						distVal = distPriority->Priority;

//					r3dOutToLog("  (%d) Min: %0.2f  Max: %0.2f  Priority: %u  Dist: %0.2f\n", distVal, distPriority->Min, distPriority->Max, distPriority->Priority, sqrt(distSq) );
				}
				break;

			case AI_EBrainPriorityTypes::Health:
				{
					AI_HealthPriority* healthPriority = (AI_HealthPriority*)priority;
					if( NULL != m_TacticParams.m_AIHealth &&
						*m_TacticParams.m_AIHealth >= healthPriority->Min && 
						*m_TacticParams.m_AIHealth <= healthPriority->Max &&
						healthVal < healthPriority->Priority )
						healthVal = healthPriority->Priority;
				}
				break;

			case AI_EBrainPriorityTypes::Group:
				{
					AI_GroupPriority* groupPriority = (AI_GroupPriority*)priority;
					// Only need to do the NavAgents search once; it's not going to change during this check.
					if( numAgentsGroupPriority == UINT_MAX )
					{
						std::tr1::unordered_set<uint32_t> PoiTypesSet;
						PoiTypesSet.insert(AutodeskNavAgent::PoiZombie);
						PoiTypesSet.insert(AutodeskNavAgent::PoiSuperZombie);
						gAutodeskNavMesh.GetNavAgentsInAABB(m_TacticParams.m_StartPos, r3dPoint3D(5.0f, 2.5f, 5.0f), PoiTypesSet, numAgentsGroupPriority, true );
					}
					if( numAgentsGroupPriority >= groupPriority->Min &&
						( numAgentsGroupPriority <= groupPriority->Max || 0.0f > groupPriority->Max ) &&
						groupVal < groupPriority->Priority )
						groupVal = groupPriority->Priority;
//					r3dOutToLog("  (%d) Min: %0.2f  Max: %0.2f  Priority: %u  Agents:%u\n", groupVal, groupPriority->Min, groupPriority->Max, groupPriority->Priority, numAgentsGroupPriority );
				}
				break;

			default:
				r3dError( "Unknown Brain Priority Type %d\n", priority->Type );
				continue;
			}
		}

		// Have we found a better goal?
		uint32_t totalVal = distVal + healthVal + groupVal;
//		r3dOutToLog("Evaluated: %s  Priority: %u = (%u + %u + %u)\n", pTactic->m_Name, totalVal, distVal, healthVal, groupVal );
		if( totalVal > m_TopPick.Priority ||
			( totalVal == m_TopPick.Priority && 0.5f < u_GetRandom(0.0f, 1.0f) )) // 50% chance if the priorities are equal.
		{
			m_TopPick.Goal = &brainConfig->BrainGoals[ m_EvalGoalNdx ];
			m_TopPick.Priority = totalVal;
		}



		// Setup for the next goal evaluation.
		if( m_EvalGoalNdx < brainConfig->NumBrainGoals - 1 )
		{		
			++m_EvalGoalNdx;
		}
		else
		{
			// If it's gotten here, then all goals have been
			// exhausted.  Accept the top pick, use the default
			// if one exists, or Cancel the evaluation and
			// return to Idle.
			if( NULL == m_TopPick.Goal && brainConfig->DefaultGoal )
			{
				m_TopPick.Goal = brainConfig->DefaultGoal;
			}
			if( NULL != m_TopPick.Goal )
			{
				uint32_t key = m_TopPick.Goal->TacticID;
				if( !g_pAITactics->GetEvalTacticParams( key, m_TacticParams ) )
				{
					r3dOutToLog( "Unable to locate tactic %d\n", key );
				}
				else
				{
					// Perform any necessary evaluation, based on
					// the Eval Tactic Params here.


					// Tactic passed initial evaluation, get the
					// rest of the TacticParams structure, checking
					// if the destination can be navigated to.
					if( g_pAITactics->GetExecTacticParams( m_TacticParams ) )
					{
//						g_pAITactics->m_tacticsHash.GetObject( key, &pTactic );
//						r3dOutToLog("Executing: %s\n", pTactic->m_Name);

#if defined(WO_SERVER) && defined(ZOMBIE_PERFORMANCE_TESTING)
						g_ZombieFrameBrainsCalculatingTime += r3dGetTime() - evalStartTime;
#endif
						SwitchToState( AI_EBrainStates::Executing );
						m_Timer = 0.0f;
						m_AccumTime = 0.0f;
						m_EvalGoalNdx = UINT_MAX;
						m_TopPick.Goal = NULL;
						m_TopPick.Priority = 0;
#ifndef NEW_AI
						m_EvalTacticHashKeyNdx = UINT_MAX;
#endif // NEW_AI
						UpdateExecuting();

						return true;
					}
				}
			}
#if defined(WO_SERVER) && defined(ZOMBIE_PERFORMANCE_TESTING)
			g_ZombieFrameBrainsCalculatingTime += r3dGetTime() - evalStartTime;
#endif

			r3dOutToLog( "Failed to find a goal that can be met.\n" );
			Cancel();
			return false;
		}

		evalTimer = r3dGetTime() - evalStartTime;
		m_Timer += evalTimer;

#if defined(WO_SERVER) && defined(ZOMBIE_PERFORMANCE_TESTING)
		g_ZombieFrameBrainsCalculatingTime += evalTimer;
#endif
	}




#else
	while( approxTimeLimit < 0.0f || approxTimeLimit > evalTimer )
	{
		// Get a Tactic to evaluate 
		// TODO: (Maybe a Mgr has assigned one
		//        already. Then what do we do?).
		if( UINT_MAX == m_EvalTacticHashKeyNdx )
			m_EvalTacticHashKeyNdx = 0;
		uint32_t key = g_pAITactics->m_tacticsHashKeys[ m_EvalTacticHashKeyNdx ];
		if( !g_pAITactics->GetEvalTacticParams( key, m_TacticParams ) )
		{
			r3dOutToLog( "Unable to locate tactic %d\n", key );
		}
		else
		{
			// Evaluate the current tactic for the chosen goals.
//			r3dOutToLog( " Evaluating Tactic(%d) %s\n", m_TacticParams.m_TacticConfig->m_TacticID, m_TacticParams.m_TacticConfig->m_Name );
//			r3dOutToLog( "	Tactic(%d) takes %0.2f of %0.2f seconds to execute.\n", m_TacticParams.m_TacticConfig->m_TacticID, m_TacticParams.m_Scale, m_MaxTime );
			if( 0.0f > m_MaxTime || m_TacticParams.m_Scale < m_MaxTime )
			{
				// Tactic passed initial evaluation, get the
				// rest of the TacticParams structure, checking
				// if the destination can be navigated to.
				if( g_pAITactics->GetExecTacticParams( m_TacticParams ) )
				{
					SwitchToState( AI_EBrainStates::Executing );
					m_Timer = 0.0f;
					m_AccumTime = 0.0f;
					m_EvalTacticHashKeyNdx = UINT_MAX;
					UpdateExecuting();

					return true;
				}
			}
		}

		// Setup for the next tactic evaluation.
		if( m_EvalTacticHashKeyNdx < ( g_pAITactics->m_tacticsHashKeys.size() - 1 ) )
		{		
			++m_EvalTacticHashKeyNdx;
		}
		else
		{
			// If it's gotten here, then all tactics have been
			// exhausted.  Cancel the evaluation and return to Idle.
			r3dOutToLog( "Failed to find a tactic that meets the goals.\n" );
			Cancel();
			return false;
		}

		evalTimer = r3dGetTime() - evalStartTime;
		m_Timer += evalTimer;
	}
#endif // NEW_AI

	return false;
}

bool AI_Brain::UpdateExecuting()
{
	float fTimeSpent = 1.1f + m_Timer - m_AccumTime;
	//m_TargetPos = m_TacticParams.m_TacticConfig->GetPosition( m_TacticParams, fTimeSpent );
	m_TargetChanged = m_TacticParams.m_TacticConfig->NextCheckpoint( m_TacticParams );
	if( m_TargetChanged )
	{
		m_TargetPos = m_TacticParams.m_TacticConfig->GetPrecomputedPosition( m_TacticParams, fTimeSpent );

		//r3dOutToLog("!!! Brain checkpointNdx %d\n", m_TacticParams.m_CurrentCheckpointNdx );
		//r3dOutToLog("!!! Brain targetpos <%0.2f, %0.2f, %0.2f>\n", m_TargetPos.x, m_TargetPos.y, m_TargetPos.z);
	}
	float fFrameTime = r3dGetFrameTime();
	m_Timer += fFrameTime;

	if( fTimeSpent > m_TacticParams.m_Scale )
		Cancel();

	return true;
}

#endif