//=========================================================================
//	Module: obj_Zombie.h
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#pragma once

#include "multiplayer/P2PMessages.h"
#include "multiplayer/NetCellMover.h"
#include "ZombieStates.h"
#include "r3dInterpolator.h"

class r3dPhysSkeleton;
class r3dPhysDogSkeleton;
#ifdef VEHICLES_ENABLED
class obj_Vehicle;
#endif
//////////////////////////////////////////////////////////////////////////

#ifndef FINAL_BUILD
struct DebugData
{
	float							m_time;
	EZombieStates::EGlobalStates	m_zombieState;
	EZombieStates::EGlobalStates	m_zombieAnimState;
	r3dPoint3D						m_point;
	r3dVector						m_velocity;

	DebugData()
		: m_time( r3dGetTime() )
		, m_zombieState( EZombieStates::ZState_Idle )
		, m_zombieAnimState( EZombieStates::ZState_Idle )
		, m_point( 0, 0, 0 )
		, m_velocity( 0, 0, 0 )
	{
	}

	DebugData( EZombieStates::EGlobalStates zombieState, EZombieStates::EGlobalStates zombieAnimState, const r3dPoint3D& point, const r3dVector& velocity )
		: m_time( r3dGetTime() )
		, m_zombieState( zombieState )
		, m_zombieAnimState( zombieAnimState )
		, m_point( point )
		, m_velocity( velocity )
	{
	}

	DebugData( const DebugData& data )
		: m_time( r3dGetTime() )
		, m_zombieState( data.m_zombieState )
		, m_zombieAnimState( data.m_zombieAnimState )
		, m_point( data.m_point )
		, m_velocity( data.m_velocity )
	{
	}

	const DebugData& operator=( const DebugData& data )
	{
		m_time				= data.m_time;
		m_zombieState		= data.m_zombieState;
		m_zombieAnimState	= data.m_zombieAnimState;
		m_point				= data.m_point;
		m_velocity			= data.m_velocity;

		return *this;
	}
};
struct DebugHistory
{
	static const uint32_t size = 10;
	uint32_t	m_index;
	float		m_minDistDelta;
	float		m_minTimeDelta;
	DebugData	m_data[ size ];
	bool		m_first;

	DebugHistory()
		: m_index( 0 )
		, m_minDistDelta( 1.0f )
		, m_minTimeDelta( 1.0f )
		, m_first( true )
	{
		memset( m_data, 0, sizeof( DebugData ) * size );
	}

	DebugHistory( const DebugHistory& history )
		: m_index( history.m_index )
		, m_minDistDelta( history.m_minDistDelta )
		, m_minTimeDelta( history.m_minTimeDelta )
		, m_first( history.m_first )
	{
		memcpy( m_data, history.m_data, sizeof( DebugData ) * size );
	}

	void Add( const DebugData& data )
	{
		DebugData& prevData = m_data[ m_index ];
		if( (data.m_time - prevData.m_time) > m_minTimeDelta )
		{
			r3dPoint3D distDelta = data.m_point - prevData.m_point;
			if( distDelta.Length() > m_minDistDelta )
			{
				if( !m_first )
					m_index = ( m_index + 1 ) % size;
				else
					m_first = false;
				m_data[ m_index ] = data;
			}
		}
	}
};
#endif

//////////////////////////////////////////////////////////////////////////

class obj_Zombie: public GameObject
{
	DECLARE_CLASS(obj_Zombie, GameObject)
	
private:
	int		m_typeIndex;		// 0 - normal zombie, 1 - super zombie, 2 - zombie dog

	bool	m_isFemale;

	bool	m_isIdleCrouched;
	bool	m_isIdleEating;

	int		attackCounter;
	int		superAttackDir;

	// sounds
	float m_sndMaxDistScream;
	float m_sndMaxDistIdle;
	float m_sndMaxDistChase;
	float m_sndMaxDistAttack;
	float m_sndMaxDistAll;

	void* m_sndScreamHandle;
	void* m_sndIdleHandle;
	void* m_sndChaseHandle;
	void* m_sndAttackHandle;
	float m_sndAttackLength;
	float m_sndAttackNextPlayTime;
	void* m_sndDeathHandle;
	void* m_sndHurtHandle;

	// Animation frames used to sync sounds.
	int prevAttackFrame;
	int prevWalkFrame;
	int prevRunFrame;
	int prevSprintFrame;

	void UpdateSounds();

	void DestroySounds();
	void PlayAttackSound();
	void PlayScreamSound();
	void PlaySuperZombieAlertSound(int alert);
	void PlaySuperZombieChestBeatSound();
	void PlaySuperZombieDestroyBarricadeSound();
	void PlaySuperZombieFootstepSound();
	void PlaySuperZombieRunAttackSound();
	bool isSoundAudible();

	void UpdateZombieAnimForVelocity();
	void UpdateDogAnimForVelocity();
	void UpdateSuperZombieAnimForVelocity();
	
	bool		gotDebugMove;
	PKT_S2C_Zombie_DBG_AIInfo_s dbgAiInfo;

#ifndef FINAL_BUILD
public:
	float	m_minSpeed;
	float	m_maxSpeed;

	DebugHistory	m_debugHistory;
#endif

public:
	void PlayDeathSound();
	void PlayHurtSound();

	PKT_S2C_CreateZombie_s CreateParams;

	/**	Zombie animation system. */
	r3dAnimation	anim_;
	int				ZombieAnimState;
	int				ZombieAnimStateLevel;
	int				ZombieAnimGroup; // Prevent Thriller looking Zombies, by keeping them in the same grouping of anims.
	struct RequestAnimTimers
	{
		RequestAnimTimers()
			: IdleReqTime( -1.0f )
			, WalkReqTime( -1.0f )
			, RunReqTime( -1.0f )
			, SprintReqTime( -1.0f )
			, CurAnimReqTime( -1.0f )
		{
		}
		float GetIdleTime() const		{ if( 0.0f < IdleReqTime ) return r3dGetTime() - IdleReqTime; return 0.0f; }
		float GetWalkTime() const		{ if( 0.0f < WalkReqTime ) return r3dGetTime() - WalkReqTime; return 0.0f; }
		float GetRunTime() const		{ if( 0.0f < RunReqTime ) return r3dGetTime() - RunReqTime; return 0.0f; }
		float GetSprintTime() const		{ if( 0.0f < SprintReqTime ) return r3dGetTime() - SprintReqTime; return 0.0f; }
		float GetCurAnimTime() const	{ if( 0.0f < CurAnimReqTime ) return r3dGetTime() - CurAnimReqTime; return 0.0f; }
		void ResetTimers()	{ IdleReqTime = -1.0f; WalkReqTime = -1.0f; RunReqTime = -1.0f; SprintReqTime = -1.0f; CurAnimReqTime = -1.0f; }
		void ReqIdle()		{ if( 0.0f < IdleReqTime) return; ResetTimers(); CurAnimReqTime = IdleReqTime = r3dGetTime(); }
		void ReqWalk()		{ if( 0.0f < WalkReqTime) return; ResetTimers(); CurAnimReqTime = WalkReqTime = r3dGetTime(); }
		void ReqRun()		{ if( 0.0f < RunReqTime) return; ResetTimers(); CurAnimReqTime = RunReqTime = r3dGetTime(); }
		void ReqSprint()	{ if( 0.0f < SprintReqTime) return; ResetTimers(); CurAnimReqTime = SprintReqTime = r3dGetTime(); }
		float		IdleReqTime;
		float		WalkReqTime;
		float		RunReqTime;
		float		SprintReqTime;
		float		CurAnimReqTime;

		// Minimum times required for switching, for an anim change based on velocity.
		static const float MinReqTime;
		static const float MinCurAnimTime;
	};
	RequestAnimTimers		ZombieReqAnimTimers;
	struct r3dVectorAverage
	{
		static const int	SampleSize = 3;
		int					CurrentSample;
		float				SetTimer;
		const float			MinTime;
		r3dVector			Samples[ SampleSize ];

		r3dVectorAverage();

		void		Reset ();
		void		SetCurSample( const r3dVector& sample );
		r3dVector	GetAverage() const;
	};
	r3dVectorAverage AvgVelocity;
	r3dSlerpInterpolator turnInterp;

	int physSkeletonIndex;
	int isPhysInRagdoll;

	int wasVisible;
	gobjid_t	targetId;

	r3dMesh*	zombieParts[4]; //head/body/legs/special
	bool		gotLocalBbox;

	bool		IsSuperZombie() { return m_typeIndex == 1; }

	bool		IsDogZombie() { return m_typeIndex == 2; }

	float		VelocityDog;

	void		UpdateAnimations( int fullAnimUpdate );
	void		RecalcAnimations( int advanceOnly );
	int		AddAnimation(const char* anim);

	void		AddSprintAnim(int& transAnimIdOut, int& sprintAnimIdOut);
	void		ForceIdleAnimCheck();
	int			StartIdleAnim();
	int			StartIdleCrouchedAnim();
	int			StartIdleEatCrouchedAnim();
	void		StartWalkAnim();
	void		StartWalk2Anim();
	void		StartRunAnim();
	void		StartRun2Anim();
	void		StartSprintAnim();
	void		StartSprint2Anim();
	void		StartAttackAnim();
	void		StartTurnAnim(float finishAngle);
	int			PlayCallForHelpAnim();
	int			PlayDeadStandAnim();
	int			PlayStaggeredAnim();
	int			PlaySuperZombieAlertAnim(int alert);
	int			PlaySuperZombieChestBeatAnim();
	int			PlaySuperZombieRunAttackAnim();
	int			PlaySuperZombieSuperAttackAnim();

	CNetCellMover	netMover;
	void		ProcessMovement();
	void		CancelSprint();

	void		SetFacingVector( const r3dVector& v );
	r3dVector	GetFacingVector() const;
	r3dVector	GetRightVector() const;

	int		PhysXObstacleIndex;

	r3dPoint3D	lastTimeHitPos;
	int		lastTimeDmgType;
	int		lastTimeHitBoneID;
	int		staggeredTrack;
	int		attackTrack;
	int		holdAttackTrack;
	int		WalkTrack;
	int		RunTrack;
	int		SprintTrack;
	float		walkSpeed;
	float		SprintTimer;
	float		SprintFalloffTimer;

	int		ZombieState;

	bool	bDead;

	int UpdateWarmUp;
	int PhysicsOn;

	struct ZombieSortEntry
	{
		obj_Zombie*	zombie;
		float		distance;
	};

	static r3dTL::TArray< ZombieSortEntry > ZombieList;

	struct CacheEntry
	{
		r3dPhysSkeleton*	skeleton;
		r3dPhysDogSkeleton*	skeletonDog;
		bool				allocated;
	};

	static r3dTL::TArray< CacheEntry > PhysSkeletonCache;

	static void InitPhysSkeletonCache( float progressStart, float progressEnd );

	static void FreePhysSkeletonCache();

	static void ReassignSkeletons();

	void		LinkSkeleton( int cacheIndex );
	void		UnlinkSkeleton();

	void		SwitchPhysToRagdoll( bool ragdollOn, r3dPoint3D* hitForce, int boneID, bool isVehicleHit = false );

	void		SetZombieState(int state, bool atStart);
	
	void		DoDeath();

	r3dPhysSkeleton* GetPhysSkeleton() const;
	r3dPhysDogSkeleton* GetPhysSkeletonDog() const;

	virtual	BOOL	OnNetReceive(DWORD EventID, const void* packetData, int packetSize);
	void			OnNetPacket(const PKT_S2C_MoveTeleport_s& n);
	void			OnNetPacket(const PKT_C2C_MoveSetCell_s& n);
	void			OnNetPacket(const PKT_C2C_MoveRel_s& n);
	void			OnNetPacket(const PKT_S2C_ZombieSetState_s& n);
	void			OnNetPacket(const PKT_S2C_ZombieSetTurnState_s& n);
	void			OnNetPacket(const PKT_S2C_ZombieSprint_s& n);
	void			OnNetPacket(const PKT_S2C_ZombieTarget_s& n);
	void			OnNetPacket(const PKT_S2C_ZombieCycleFidget_s& n);

public:
	obj_Zombie();
	~obj_Zombie();

	virtual	BOOL OnCreate();

	void UpdateStart();
	void UpdateConcurrent( int updateAnimation );
	void UpdateStop();

	virtual BOOL Update();
	virtual BOOL OnDestroy();
	virtual void OnPreRender();
	
	virtual void OnContactModify(PhysicsCallbackObject *obj, PxContactSet& contacts);

	virtual void AppendShadowRenderables(RenderArray &rarr, int sliceIndex, const r3dCamera& cam);
	virtual void AppendRenderables(RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& cam);
	
#ifdef VEHICLES_ENABLED
	void AttemptVehicleHit(PxF32 vehicleSpeed, obj_Vehicle* vehicle);
	void PhysicsEnable( const int& physicsEnabled );
#endif

#ifndef FINAL_BUILD
	void DrawDebugInfo() const;
#endif
};

