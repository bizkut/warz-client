//=========================================================================
//	Module: AI_Brain.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

#ifndef FINAL_BUILD

#define NEW_AI

#include "AI_Tactics.h"

namespace AI_EBrainStates
{
	enum AI_EBrainState
	{
		 Idle
		,Evaluation
		,Executing
	};
	static r3dString const aStateNames[3] = {
		 "Idle"
		,"Evaluation"
		,"Executing"
	};
};

//---------------------------------------------------------

namespace AI_EBrainPriorityTypes
{
	enum AI_EBrainPriorityType
	{
		 Distance
		,Health
		,Group
	};
};

//---------------------------------------------------------

struct AI_BrainPriority
{
	uint32_t	Type;
	uint32_t	Priority;
};

//---------------------------------------------------------

struct AI_DistancePriority : public AI_BrainPriority
{
	float	Max;
	float	Min;
};

//---------------------------------------------------------

struct AI_HealthPriority : public AI_BrainPriority
{
	float	Max;
	float	Min;
};

//---------------------------------------------------------

struct AI_GroupPriority : public AI_BrainPriority
{
	float	Max;
	float	Min;
};

//---------------------------------------------------------

struct AI_BrainGoal
{
	uint32_t			TacticID;
	uint32_t			NumPriorities;
	AI_BrainPriority**	Priorities;
};

//---------------------------------------------------------

class AI_BrainConfig
{
	AI_BrainConfig() { }
	const AI_BrainConfig& operator=( const AI_BrainConfig& config ) { }

public:
	AI_BrainConfig( uint32_t id );
	~AI_BrainConfig();

	void		Destroy();

	static const uint32_t NameLen = 32;

	uint32_t			ID;
	uint32_t			NumBrainGoals;
	AI_BrainGoal*		BrainGoals;
	AI_BrainGoal*		DefaultGoal;
	TCHAR				Name[NameLen];
};

//---------------------------------------------------------

class AI_BrainProfiles
{
	AI_BrainProfiles( const AI_BrainProfiles& profiles ) { }
	const AI_BrainProfiles& operator=( const AI_BrainProfiles& profiles ) { }

protected:
	void		loadBrainConfig( pugi::xml_node& xmlConfig );
	void		loadBasePriority( pugi::xml_node& xmlPriority, AI_BrainPriority* priority );
	void		loadDistancePriority( pugi::xml_node& xmlPriority, AI_DistancePriority* priority );
	void		loadHealthPriority( pugi::xml_node& xmlPriority, AI_HealthPriority* priority );
	void		loadGroupPriority( pugi::xml_node& xmlPriority, AI_GroupPriority* priority );

public:
	AI_BrainProfiles();
	~AI_BrainProfiles();

	void		Init();
	void		Destroy();

	uint32_t	GetRandomBrainConfigID() const;
	const AI_BrainConfig*	GetBrainConfig( uint32_t brainConfigID ) const;

	const r3dgameVector(uint32_t)& GetBrainConfigIDs() const;

protected:
	r3dgameVector(uint32_t) m_brainConfigHashKeys;
	struct BrainConfigIDHashFunc_T { inline int operator () (const uint32_t key) { return key; } };
	HashTableDynamic<AI_BrainConfig*, uint32_t, BrainConfigIDHashFunc_T, 32> m_brainConfigsHash;
};
extern AI_BrainProfiles* g_pAIBrainProfiles;

//---------------------------------------------------------

struct AI_TopPick
{
	AI_BrainGoal*		Goal;
	uint32_t			Priority;
};

//---------------------------------------------------------

class AI_Brain
{
	AI_Brain( const AI_Brain& brain ) { }
	const AI_Brain& operator =( const AI_Brain& brain ) { }

protected:
	void		SwitchToState(AI_EBrainStates::AI_EBrainState in_state);
	bool		UpdateIdle();
	bool		UpdateEvaluation( float approxTimeLimit = 0.1f ); // Use a negative value for no Time Limit.
	bool		UpdateExecuting();

public:
	AI_Brain();
	~AI_Brain();

	void		Cancel();
	bool		Update();

	uint32_t						m_BrainConfigID;
	float							m_Timer;
	float							m_AccumTime;	// Time to subract from the Timer, for things like staggering.
#ifndef NEW_AI
	float							m_MaxTime;		// Maximum time wanted to spend executing tactics.
#endif // NEW_AI
	AI_TacticParams					m_TacticParams;	// Tactic currently being used.
	AI_EBrainStates::AI_EBrainState	m_State;		// What is the AI doing?
	r3dPoint3D						m_TargetPos;	// Where is the AI headed?
	bool							m_TargetChanged;// Has the target changed?

	AI_TopPick						m_TopPick;		// What is the choice so far?

protected:
	uint32_t		m_EvalGoalNdx;
#ifndef NEW_AI
	uint32_t		m_EvalTacticHashKeyNdx;
#endif // NEW_AI
	static const r3dPoint3D	NonTarget;
};

#endif