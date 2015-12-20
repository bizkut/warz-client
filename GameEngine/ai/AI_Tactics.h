//=========================================================================
//	Module: AI_Tactics.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

#ifndef FINAL_BUILD

class AI_Brain;
struct AI_TacticParams;

namespace AI_TacticTypes
{
	enum AI_TacticType
	{
		StraightLine = 0,
		BezierCurve = 1
	};
};

//---------------------------------------------------------

class AI_TacticConfig
{
protected:
	AI_TacticConfig() { }

public:
	AI_TacticConfig( uint32_t id );
	virtual ~AI_TacticConfig();

	virtual void Destroy();
	virtual bool NextCheckpoint( AI_TacticParams& params ) const;

	virtual bool GetEvalTacticParams( AI_TacticParams& params ) const = 0;
	virtual bool GetExecTacticParams( AI_TacticParams& params ) const = 0;
	virtual r3dPoint3D GetPosition( const AI_TacticParams& params, const float& timeSpent ) const = 0;
	virtual r3dPoint3D GetPrecomputedPosition( AI_TacticParams& params, const float& timeSpent ) const = 0;

	static const uint32_t			NameLen = 32;

	uint32_t						m_TacticID;
	AI_TacticTypes::AI_TacticType	m_TacticType;
	TCHAR							m_Name[32];
};

//---------------------------------------------------------

class AI_StraightLineTacticConfig : public AI_TacticConfig
{
	AI_StraightLineTacticConfig() { }

public:
	AI_StraightLineTacticConfig( uint32_t id );

	void Destroy();

	bool GetEvalTacticParams( AI_TacticParams& params ) const;
	bool GetExecTacticParams( AI_TacticParams& params ) const;
	r3dPoint3D GetPosition( const AI_TacticParams& params, const float& timeSpent ) const;
	r3dPoint3D GetPrecomputedPosition( AI_TacticParams& params, const float& timeSpent ) const;
};

//---------------------------------------------------------

class AI_BezierTacticConfig : public AI_TacticConfig
{
	AI_BezierTacticConfig() { }

public:
	AI_BezierTacticConfig( uint32_t id );

	void Destroy();

	bool GetEvalTacticParams( AI_TacticParams& params ) const;
	bool GetExecTacticParams( AI_TacticParams& params ) const;
	r3dPoint3D GetPosition( const AI_TacticParams& params, const float& timeSpent ) const;
	r3dPoint3D GetPrecomputedPosition( AI_TacticParams& params, const float& timeSpent ) const;

	uint32_t	m_NumPoints;
	r3dPoint3D*	m_Points;
};

//---------------------------------------------------------

struct AI_TacticParams
{
	AI_TacticParams()
		: m_TacticConfig( NULL )
		, m_NumCheckpoints( 0 )
		, m_CurrentCheckpointNdx( UINT_MAX )
		, m_Scale( 0.0f )
		, m_Angle( 0.0f )
		, m_Axis( R3D_ZERO_VECTOR )
		, m_StartPos( R3D_ZERO_VECTOR )
		, m_DestPos( R3D_ZERO_VECTOR )
		, m_AIHealth( NULL )
	{}

	AI_TacticParams( const r3dPoint3D& startPos, const r3dPoint3D& destPos )
		: m_TacticConfig( NULL )
		, m_NumCheckpoints( 0 )
		, m_CurrentCheckpointNdx( UINT_MAX )
		, m_Scale( 0.0f )
		, m_Angle( 0.0f )
		, m_Axis( R3D_ZERO_VECTOR )
		, m_StartPos( startPos )
		, m_DestPos( destPos )
		, m_AIHealth( NULL )
	{}

	// Eval Params
	AI_TacticConfig*	m_TacticConfig;
	float				m_Scale;

	// Exec Params
	uint32_t			m_NumCheckpoints;
	uint32_t			m_CurrentCheckpointNdx;
	float				m_Angle;
	r3dVector			m_Axis;
	r3dPoint3D			m_Checkpoints[4];

	// Pre-load these Params
	r3dPoint3D			m_StartPos;
	r3dPoint3D			m_DestPos;

	// Initialization Params (Set at, or just after, Init and never change)
	const float*		m_AIHealth;	// Pointer to AI's health, so we get updated values w/o having to update a copy.
};

//---------------------------------------------------------

class AI_Tactics
{
	friend class AI_Brain;

	AI_Tactics( const AI_Tactics& tactic ) { }
	const AI_Tactics& operator =( const AI_Tactics& tactic ) { }

protected:
	void		loadTactic( pugi::xml_node& xmlTactic );
	void		loadBaseTactic( pugi::xml_node& xmlTactic, AI_TacticConfig& tactic );
	void		loadStraightLineTactic( pugi::xml_node& xmlTactic, uint32_t& tacticID );
	void		loadBezierTactic( pugi::xml_node& xmlTactic, uint32_t& tacticID );

public:
	AI_Tactics();
	~AI_Tactics();

	void		Init();
	void		Destroy();

	uint32_t	GetTacticID( const char* name ) const;
	bool		GetEvalTacticParams( const uint32_t& tacticID, AI_TacticParams& params ) const;
	bool		GetExecTacticParams( AI_TacticParams& params ) const;
	r3dPoint3D	GetPosition( const AI_TacticParams& params, const float& timeSpent ) const;

protected:
	r3dgameVector(uint32_t) m_tacticsHashKeys;
	struct TacticsIDHashFunc_T { inline int operator () (const uint32_t key) { return key; } };
	HashTableDynamic<AI_TacticConfig*, uint32_t, TacticsIDHashFunc_T, 32> m_tacticsHash;
};
extern AI_Tactics* g_pAITactics;

#endif