#pragma once

#ifndef FINAL_BUILD

#include "r3dProtect.h"
#include "GameCommon.h"
#include "GameCode/UserProfile.h"
#include "../ai/StateMachine.h"
#include "obj_MechAnim.h"
#include "obj_MechEvents.h"

class CMechUberEquip;

//////////////////////////////////////////////////////////////////////////

const char * const MECH_ANIMATION_LIST[] =
{
	"Walk",
	"Walk back",
	"Run",
	"Walk Wall Bump Front",
	"Walk Wall Bump Back",
	"Run Wall Bump Front",
	"Falling Short Bump",
	"Falling Loop",
	"Falling Long Bump",
	"Engine Overheat Down",
	"Engine Overheat Up",
	"Damage Front",
	"Damage Left",
	"Damage Back",
	"Damage Right",
	"Destroying and Fall",
	"Grenade Launching",
	"Machine Gun Shaking",
	"Sniper other single shooting animation",
	"Idle",
	"Loop Turn 90 Left",
	"Loop Turn 90 Right",
	"Walk Limp Right",
	"Walk Limp Left",
	"Walk Back Limp Right",
	"Walk Back Limp Left"
};

enum MechAnimationsID
{
	MECH_ANIM_WALK,
	MECH_ANIM_WALK_BACK,
	MECH_ANIM_RUN,
	MECH_ANIM_WALK_WALL_BUMP_FRONT,
	MECH_ANIM_WALK_WALL_BUMP_BACK,
	MECH_ANIM_RUN_WALL_BUMP_FRONT,
	MECH_ANIM_FALLING_SHORT_BUMP,
	MECH_ANIM_FALLING_LOOP,
	MECH_ANIM_FALLING_LONG_BUMP,
	MECH_ANIM_ENGINE_OVERHEAT_DOWN,
	MECH_ANIM_ENGINE_OVERHEAT_UP,
	MECH_ANIM_DAMAGE_FRONT,
	MECH_ANIM_DAMAGE_LEFT,
	MECH_ANIM_DAMAGE_BACK,
	MECH_ANIM_DAMAGE_RIGHT,
	MECH_ANIM_DESTROY_AND_FALL,
	MECH_ANIM_GRENADE_LAUNCHING,
	MECH_ANIM_MACHINE_GUN_SHAKING,
	MECH_ANIM_SNIPER_AND_OTHER_SINGLE_SHOOTING,
	MECH_ANIM_IDLE,
	MECH_ANIM_LOOP_TURN_90_LEFT,
	MECH_ANIM_LOOP_TURN_90_RIGHT,
	MECH_ANIM_WALK_LIMP_RIGHT,
	MECH_ANIM_WALK_LIMP_LEFT,
	MECH_ANIM_WALK_BACK_LIMP_RIGHT,
	MECH_ANIM_WALK_BACK_LIMP_LEFT,
	MECH_ANIM_COUNT
};

enum MECH_WEAPON_TYPES
{
	MECH_WEAPON_MISSILE = 0,
	MECH_WEAPON_SNIPER,
	MECH_WEAPON_GRENADE,
	MECH_WEAPON_MORTAR,
	MECH_WEAPON_MACHINEGUN
};

//////////////////////////////////////////////////////////////////////////

const int NUM_MECH_BODIES = 3;
const int NUM_MECH_ARMORS = 4;

//////////////////////////////////////////////////////////////////////////

enum r3dMechTypes
{
	r3dMechType_Light = 0,
	r3dMechType_Medium,
	r3dMechType_Heavy
};

//////////////////////////////////////////////////////////////////////////

class obj_Mech : public GameObject
{
	DECLARE_CLASS(obj_Mech, GameObject)

	r3dFiniteStateMachine<obj_Mech> animFSM;
	r3dFiniteStateMachine<obj_Mech> weaponsFireFSM;
	int drivable;

	/**	Mech particle FX events. */
	MechFXEvents::EventSystem mechEvents;

public:
	char		sPresetName[256];
	char		sMechSelected[256];
	float		fSkelListOffset;

	r3dSkeleton*	m_BindSkeleton;
	void		ReinitializeMech(const char *path);
	void		InitDefaultAnimations(const char *path);

	char		sAnimSelected[256];
	float		fAnimListOffset;
	
	r3dAnimation*	m_Animation;
	r3dAnimPool	m_AnimPool;

	struct AdditionalAnimationData
	{
		r3dString source;

		int animIndexRemap;
		float fadeStart;
		float fadeEnd;
		float fadeInTime;
		float fadeOutTime;
		int onlyUpperBody;
		int onlyLowerBody;
		int animTrackId;
		int startFrame;
		int endFrame;
		int totalFrames;

		AdditionalAnimationData()
		: animIndexRemap(-1)
		, fadeStart(0)
		, fadeEnd(1.0f)
		, fadeInTime(0.2f)
		, fadeOutTime(0.2f)
		, onlyUpperBody(0)
		, onlyLowerBody(0)
		, animTrackId(0)
		, startFrame(0)
		, endFrame(0)
		, totalFrames(0)
		{}
	};

	AdditionalAnimationData additionalAnimData[_countof(MECH_ANIMATION_LIST)];
	void		SyncAnimation(bool force = false);

	float		adjBody;
	float		adjWeap1;
	float		adjWeap2;
	float		adjWeap3;
	float		adjWeap4;
	float		adjWeap5;
	float		adjWeap6;
	
	int		boneId_MechPelvis;
	int		boneId_Weapons[MAX_NUM_MECH_WEAPONS];
	void	InitWeaponBones();

	struct WeaponsAnimData
	{
		int fireAnimID;
		r3dAnimation *anim;
		int wpnType;

		WeaponsAnimData()
		: fireAnimID(-1)
		, anim(0)
		, wpnType(0)
		{}
	};

	WeaponsAnimData wpnAnimations[MSLOT_Max - MSLOT_Weapon1];
	
	int		m_renderSkel;

	// mech equipment	
	CMechUberEquip*	uberEquip_;
	int		PlayerState;	// player state as Playerstate_e
	
	void DetectIfOnGround();
	bool bOnGround;
	float fHeightAboveGround;

	r3dSTLString bodyPaths[NUM_MECH_BODIES];
	int curBodyPart;
	r3dSTLString armorPaths[NUM_MECH_ARMORS];
	int curArmorPart;
	r3dMechTypes mechType;
	
  // movement
	r3dPoint3D	ViewAngle;
	r3dVector	cameraTargetOffset;
	r3dPoint2D	crosshairOffset;
	float		horizontalOffset;
	r3dVector	currentCameraPos;
	bool		usePivotAsCameraAnchor;
	float		cameraLockDistance;

	float		forwardSpeed;
	float fallingVelocity;

	float		m_fPlayerRotation;
	void		UpdateRotation();

	float		currentTurretAngle;
	//	In radians per sec
	float		turretRotSpeed;
	//	In sec
	float		turretRotSpeedUpTime;
	//	In sec
	float		turretRotSlowDownTime;
	float		turretStartRotTime;
	float		turretRotDelayTime;
	float		turretAllowedAngle;
	float		turretLimitAngle;
	void		UpdateTurretRotation();
	float		CalculateCurrentTurretRotSpeed(float targetAngle);
	float		LimitTargetAngle(float targetAngle, float curTurretAngle) const;
	bool		CalcTurretRotDir(float curAngle, float targetAngle) const;
	void		UnloadWeaponSlot(EMechSlot wpnSlot);
	void		LoadWeaponSlot(EMechSlot wpnSlot, const char *path);

	bool		rotationTargetReached;
	r3dVector	prevPelvisPos;
	r3dVector	GetPelvisPos();

	D3DXMATRIX	MoveMatrix;
	D3DXMATRIX	DrawMatrix;
	D3DXMATRIX	ViewMatrix;
	r3dVector	GetViewForw() const { return r3dVector(ViewMatrix._31, ViewMatrix._32, ViewMatrix._33); }
	r3dVector	GetViewRight() const { return r3dVector(ViewMatrix._11, ViewMatrix._12, ViewMatrix._13); }
	r3dVector	GetMoveForw() const { return r3dVector(MoveMatrix._31, MoveMatrix._32, MoveMatrix._33); }
	r3dVector	GetMoveRight() const { return r3dVector(MoveMatrix._11, MoveMatrix._12, MoveMatrix._13); }
	
	void		UpdateLocalMovement();

	int		AnimMode;

	// var where we will keep velocity, regardless of physx
	// this is needed, because animation and network moving player code depends on GetVelocity()
	// and velocity doesn't do a thing in ControllerPhysObj
	r3dPoint3D	plr_local_Velocity;	
	virtual	void 			SetVelocity(const r3dPoint3D& vel) { plr_local_Velocity = vel; }
	virtual	const r3dPoint3D&	GetVelocity() const { return plr_local_Velocity; }

  // controllable vars
	float		Height;
	float		MoveSpeed;
	float		MoveLimpSpeed;
	float		MoveBackLimpSpeed;
	float		MoveBackSpeed;
	float		RunSpeed;
	float		AnimWalkSpeed;
	float		rotationSpeed;

	float		moveFwdAccel;
	float		moveBackAccel;
	float		runAccel;
	float		timeSinceStartMoving;

	bool		handleAnimationTeleport;
  
	
  // TEMP CRAP
	int		bDead;
	int		TeamID;
	int		uavRequested_;
	int		uavViewActive_;
	int		laserViewActive_;
	void		GetUserName(char* out) const { *out = 0; }
	int		LoadoutSlot;
	wiCharDataFull	CurLoadout;
	void UpdateLoadoutSlot(const struct wiLoadoutSlot& slot) { r3dError("##MECH_TODO"); }
	bool		isPremiumAccount;

	virtual void		UpdateTransform() { return parent::UpdateTransform(); }
	
	// local stats for scoreboard
	struct stats_s
	{
		int		Score;
		int		GD;
		int		Kills;
		int		Deaths;

		// for kill tag (passed from server - total player stats)
		int		TotalScore;
		int		Wins;
		int		Loss;

		stats_s() 
		{
			Score = 0;
			GD = 0;
			Kills  = 0;
			Deaths = 0;
			TotalScore = 0;
			Wins = 0;
			Loss = 0;
		}
	};
	stats_s		GameStats;

 // footstep code
  private:
	void*		footStepsSnd;
	struct footStepEntry_s {
	  int		id;
	  void*		snd;
	  footStepEntry_s() : id(-1), snd(0) {}
	  footStepEntry_s(int i, void* s):id(i), snd(s) {}
	};
	r3dTL::TStaticArray<footStepEntry_s, 128> footStepsArr;
	int		currentFootStepsSoundID;
	void		UpdateFootsteps() { /*##MECH_TODO*/ }

  public:
	void		StopFootstepsSound() { /*##MECH_TODO*/ }
 
  public:
	obj_Mech();
	virtual	~obj_Mech();

	virtual	BOOL		OnCreate();
	virtual	BOOL		Load(const char* fname);
	virtual	BOOL		Update();

	virtual GameObject*	Clone() { r3dError("do not user"); return NULL; }

	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected ) OVERRIDE;
	float				DrawMeshesConfigEditor(float scrx, float scry, float scrw, float scrh);
	float				DrawAnimationsConfigEditor(float scrx, float scry, float scrw, float scrh);
	float				DrawGeneralSettingsEditor(float scrx, float scry, float scrw, float scrh);
	void		DrawDebug(const r3dCamera& Cam);

	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;
	virtual void		AppendShadowRenderables( RenderArray& rarr, int sliceIndex, const r3dCamera& Cam ) OVERRIDE;

	virtual	void		ReadSerializedData(pugi::xml_node& node);
	void				ReadOwnSerializedData(pugi::xml_node& node);
	virtual void		WriteSerializedData(pugi::xml_node& node);
	void				WriteOwnSerializedData(pugi::xml_node& node);
	void				SaveCurrentPreset();
	void				LoadPreset(const char *presetPath);

	void SwitchUpperBodyAnimation(bool onlyUpperBody, r3dAnimData *ad);
	void SwitchLowerBodyAnimation(bool onlyLowerBody, r3dAnimData *ad);
	void SetDefaultPose();

	void EDITOR_ReloadAnim( const char* fname );

	void OnControllerHit( PhysicsCallbackObject* obj ) OVERRIDE;

private:
	bool DrawPossibleElementsChoose(const char *rootDir, const char *ext, const char *forceFolder, const char *intermediateFolder, r3dSTLString &outPath);
	void SwitchUpperBodyAnimationRecursive(bool onlyUpperBody, int rootBoneID, r3dAnimData *ad);
	void ProcessMovement();
	void RemoveAnimation(int i);
	void UpdateCamera();
	void SwitchDriveModes(bool driveMech);
};


#endif