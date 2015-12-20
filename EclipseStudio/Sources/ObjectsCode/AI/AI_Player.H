#pragma once

#include "r3dProtect.h"
#include "GameCommon.h"
#include "GameCode/UserProfile.h"
#include "multiplayer/P2PMessages.h"
#include "multiplayer/NetCellMover.h"
#include "ObjectsCode/AI/AI_PlayerAnim.H"
#include "../../../GameEngine/CamouflageDataManager.h"
#include "APIScaleformGfx.h"
#include "AI_PlayerSenses.h"

class Weapon;
class Gear;
class CUberEquip;
class CUberAnim;

#ifdef VEHICLES_ENABLED
class obj_Vehicle;
#endif

struct ThreatStats
{
	float		visualSum;
	float		audibleSum;
	float		interceptSum;
	float		nearbySum;
	float		closestZombie;
	uint32_t	zombieNearbyCount;
	uint32_t	zombieInterceptCount;
	uint32_t	inSightRangeCount;
	uint32_t	inAudioRangeCount;

	float		factorClosest;
	float		factorNearby;
	float		factorIntercept;
	float		factorVisible;
	float		factorAudible;

	float		weightedClosest;
	float		weightedNearby;
	float		weightedIntercept;
	float		weightedVisible;
	float		weightedAudible;

	float		threat;

	float		timer;

#ifndef FINAL_BUILD
	float		plrVisibilitySq;
	float		plrAudibilitySq;
	float		plrMaxPerceptionSq;
	float		plrMaxPerception;
	r3dVector	plrPosition;
	r3dVector	plrVelocity;
	r3dVector	plrFuturePos;
	r3dVector	zmbPosition;
	r3dVector	zmbVector;
#endif

	ThreatStats();

	void Clear();
	void Reset();
};

///////////////////////////////////////////////////////////////////////////////

class obj_Player : public GameObject
{
public:
	DECLARE_CLASS(obj_Player, GameObject)

	obj_Player();
	virtual ~obj_Player();

	r3dPoint3D UpDownEditor;
	float LeftRight;
	float LeftRight2;
	float UpDown;
	bool m_ModeEditor;
	bool m_ModeCreator;
	uint32_t m_NumbeAirDrop;

	bool m_DevPlayerHide;

	enum AuraType
	{
		AT_NONE,
		AT_BERSERK,
		AT_HEALING,
		AT_MORHINE,
		AT_SPAWNPROTECTION,
		AT_COUNT
	} m_AuraType ;

	float m_AuraTransarency;

	float		m_SpawnProtectedUntil;

	char				m_MinimapTagIconName[64]; // temp, to be optimized soon

	float   CurrentSpread;

	#define MAX_UAV_TARGETS 4
	gobjid_t m_localPlayer_lastUAVTarget[MAX_UAV_TARGETS];

	PxCapsuleObstacle physXObstacle;
	int	PhysXObstacleIndex;
	int	PhysXTerra3WarmUp;	// frame count after object creation, used to disable movement while Terrain3 is initialized

	float	nextSendCamPosTime;

	float	lastTimeUsedConsumable;
	float	currentConsumableCooldownTime;

	gp2pnetid_t lastTradeRequestedFrom;

	DWORD	CustomerID;
	DWORD	GroupID;
	r3dSec_string<64, 0xA9>	m_EncryptedUserName; // utf8 string so use it carefully. Encrypted to make life more difficult for haxorz :)
	// encrypted user name
	void		GetUserName(char* out) const {
		m_EncryptedUserName.get(out);
	}
	void  GetUserNameAndClanTag(char* out) const {
		if(ClanID != 0)
		{
			char tmpUsername[64];
			m_EncryptedUserName.get(tmpUsername);
			const char* tagColor = "#000000";
			switch(ClanTagColor)
			{
			case 1: tagColor = "#aa0000"; break;
			case 2: tagColor = "#a6780c"; break;
			case 3: tagColor = "#10a49e"; break;
			case 4: tagColor = "#20a414"; break;
			case 5: tagColor = "#104ea4"; break;
			case 6: tagColor = "#9610a4"; break;
			case 7: tagColor = "#444444"; break;
			case 8: tagColor = "#a4a4a4"; break;
			default:
				break;
			}
			sprintf(out, "<font color='%s'>[%s]</font> %s", tagColor, ClanTag, tmpUsername);
			return;
		}
		else
			return GetUserName(out);
	}
	int		ClanID;
	int		ClanTagColor;
	char		ClanTag[5*2];

	Scaleform::GFx::Value m_CharIcon;

	wiCharDataFull	CurLoadout;
	float		m_GearWeight;
	void		BackpackGridSwap(int gridFrom, int gridTo);
	void		OnBackpackChanged(int idx);
	void		OnRemoveAttachments(int idx);

	bool	m_enableRendering;

	PlayerLifeProps lifeProperties;
	float			showSlotsInfoTimer;

private:
	bool	m_ItemSimulateLeftClick;
	/**	Memorize bounding box, when switch to ragdoll. We will restore it after switching back. */
	r3dBoundBox m_OrigBBox;
	bool preparedToFakeDie;

public:
	static r3dTexture* s_laserPointerTex;
	static r3dTexture* s_flashlightFlashTex;
	static r3dTexture* s_laserFlashTex;
	static r3dTexture* s_lightHaloTex;

	// for UAV view
	static r3dTexture* s_UAVFriendlyBlinkTex;

	uint32_t		m_needToDrawBarricadePlacement; // itemID of what shield to draw
	r3dMesh*		m_BarricadeMeshPlacement; // mesh to render when you are placing shield, not sure how to implement that better rather than having pointer to mesh directly
	r3dPoint3D		m_BarricadeMeshPlacementPosition;
	bool			m_BarricadeMeshAllowToPlaceOnTop;

	// shared for all placeable items
	PrecalculatedMeshShaderConsts	m_ConsumableItemPlacementMeshVSConsts;
	bool			m_canPlaceConsumableItem;

	class r3dPhysSkeleton* m_PhysSkeleton;
	bool m_disablePhysSkeleton; // for character in frontend and respawn

	// anim
	CUberEquip*	uberEquip_;
	CUberAnim*	uberAnim_;

	void CreateUberAnim();

	void EDITOR_RecreateUberAnim();

	r3dSkeleton*	GetSkeleton();	// access to r3dSkeleton inside uberAnim
	r3dAnimation&	GetAnimation();	// access to r3dAnimation inside uberAnim
	int		PlayerState;	// player state as Playerstate_e
	int		PlayerMoveDir;	// movement direction as CUberData::EAnimDir

	int		CreatedFromEditor;

	void UpdateLoadoutSlot(const struct wiCharDataFull& slot, int slotIdxToUpdate); // slotIdxToUpdate=-1 if whole loadout needs to be updated
	void RemoveWpnAttm(int wid, int attmType, bool isPacketRequired = true); // also used to equip default attm
	void EquipWpnAttm(int wid, int backpackSlot);
	bool ChangeBackpack(int slotfrom);
	void DropItem(int slotFrom);
	void loadHeroMeshes(const class HeroConfig* heroConfig);

	void updateWeaponUI();

	float	grenadeReadyTime;

	void switchFPS_TPS();
	bool isFlashlightOn;

	void		SyncAnimation(bool force = false);
	void		UpdateAnimations(const D3DXMATRIX& CharDrawMatrix);

	r3dPoint3D	GetHeadPosition();

	Weapon*		m_Weapons[NUM_WEAPONS_ON_PLAYER]; // 0,1 - rifles. 2-item, 3-item, 4-item, 5-item
	int		m_SelectedWeapon;
	int		m_PrevSelectedWeapon;
	bool		hasScopeMode() const;
	bool		hasScopeMode_hidePlayerModel() const;
	bool		m_isPressedFireTrigger;
	bool		m_isAiming;
	bool		m_isChangedAiming1;  // not resetting event about aiming mode change. used in Weapon::Update
	bool		m_isInScope;
	bool		m_isFinishedAiming; // this one will make sure that player is aiming and that he finished playing aim animation, so no insta aim by clicking fire and aim at the same time

	bool		ChangeWeaponByIndex(int index);
	void		UpdateCharWeaponMeshes();
	void		ReloadWeaponFromSlot(int wpnSlot, int ammoSlot, int amount);
	void		UnloadWeaponClip(int wid);

	static void AddBloodOnGround(const r3dPoint3D& posOfShot); // will cast a ray down from posOfShot
	float		m_BloodTimer;

	// hit marker
	float		m_HitMarkerFadeout;
	r3dTexture*	m_HitMarkerTex; // only for local player

	float ActionUI_pressedKeyTimer;

	// local stats for scoreboard
	struct stats_s
	{
		int		XP;
		int		GD;

		stats_s() 
		{
			XP = 0;
			GD = 0;
		}
	};
	stats_s		GameStats;

	// queue of local player added rewards
	r3dgameVector(PKT_S2C_AddScore_s) rewards_;
	float		lastRewardShowTime_;
	void		ProcessRewardQueue();

	float		bodyAdjust_x;		// rotation
	float		bodyAdjust_y[2];	// bend [cur, target]

	int		boneId_Bip01_Spine1;
	int		boneId_Bip01_Spine2;
	int		boneId_Bip01_Neck;
	int		boneId_Bip01_LUpperArm;
	int		boneId_Bip01_RUpperArm;
	int		boneId_Bip01_Head;

	int		boneId_Bip01_R_Hand;
	int		boneId_Bip01_L_Hand;

	// for adrenaline and epinephrine shots
	float		m_SpeedBoost;
	float		m_SpeedBoostTime;

	float		m_MorhineShotTime;

	float		m_BandagesEffectTime;

	// var where we will keep velocity, regardless of physx
	// this is needed, because animation and network moving player code
	// depends on GetVelocity()
	// and velocity doesn't do a thing in ControllerPhysObj
	r3dPoint3D	plr_local_Velocity;	
	virtual	void 			SetVelocity(const r3dPoint3D& vel) { plr_local_Velocity = vel; }
	virtual	const r3dPoint3D&	GetVelocity() const { return plr_local_Velocity; }
	float	plr_local_moving_speed;

	// info about player
	float		Height;
	float		getPlayerHeightForCamera() const; 
	float		getPlayerHeightForCameraWithPlayerState() const;

	// sprint energy vars
	float m_Stamina;
	float m_StaminaPenaltyTime;
	float		getMaxStamina();

	// sniper & breath vars
	r3dVector SniperViewMod; // to simulate rifle movement when trying to snipe
	float		getMaxBreath();
	bool		m_isHoldingBreath;
	r3dSec_type<float, 0x5A1C2DB8> m_Breath;

	float		lastDamageRcvd;
	float		lastTimeHit;
	int		lastTimeHitBone;
	r3dPoint3D	lastTimeHitForce;
	void		ApplyDamage(const r3dPoint3D& dmgPos, float damage, GameObject* fromObj, int bodyBone, int dmgType);
	float		BloodEffect; // timer
	int			bDead;
	float		OnlyFPSCameraTime;
	float		TimeOfDeath; // for animations after dying
	STORE_CATEGORIES DeathDamageSource;
	r3dPoint3D	PosOfDeath;
	float		TimeOfLastRespawn;
	int			Dead_KillerID; // killer ID of a person who killed you
	bool		DisableKillerView; // if user clicks on a mouse, show respawn right away
	bool		bCrouch;
	bool		bProne;
	bool		isGameHardcore; //gamehardcore
	int			HardMaxPlayers; //gamehardcore

	// jump stuff
	void		DetectIfOnGround();
	bool		bOnGround; // true if player is touching any geometry
	float		fHeightAboveGround;
	void		StartJump();
	bool		IsJumpActive();
	float		JumpVelocity;
	float		JumpStartTime;	// time when we actually need to apply JumpVelocity
	float		StartFallingHeight;
	float		StartFallingTime;

#ifdef VEHICLES_ENABLED
	void StartPushFromVehicle(float vehicleSpeed, float maxSpeed);
	bool IsPushActive();
	void ClearVehicle();
	float pushVelocity;
	float pushStartTime;
	float pushedDistance;
	float pushedTime;
#endif

	// swim stuff
	bool		bIsSwimming;
	bool		IsSwimming();
	bool		IsInWater( float& waterDepth, float& waterLevel );

	// Missions stuff
#ifdef MISSIONS
	struct MissionActionData
	{
		uint32_t	missionID;
		uint32_t	actionID;
		uint32_t	actionType;
		uint32_t	subActionType;
		uint32_t	amount;	 // How many to Kill/Use/Drop/Collect
		uint32_t	count;	 // How many have been Killed/Used/Dropped/Collected, OR, what is the current state
		uint32_t	itemID;	 // Item ID for ItemActions, Weapon Restriction [0:None, weaponID:One, UINT_MAX:Multiple] for KillActions, or Object Hash for StateActions
		uint32_t	objType; // What to kill for KillActions
		r3dPoint3D	position;
		r3dVector	extents;
		bool		inArea;
		bool		completed;
		char		missionName[64];
	};
	std::vector<uint32_t> acceptedMissions;
	std::map< uint32_t, MissionActionData >	missionsActionData; // Key = actionID
	void		UpdateMissionActionData( const PKT_S2C_MissionActionUpdate_s& actionData );
	void		UpdateMissionAreaRestrictions();
	void		RemoveMissionActionData( uint32_t missionID, uint32_t reason );
#endif

	r3dSec_type<float,		0x5Ac3D3Aa>	RecoilCooldown;

	r3dVector RecoilViewMod; // adds recoil to ViewAngle
	r3dVector PermRecoilViewMod; // this recoil doesn't snap back to original position
	float		m_LastKillTime;
	r3dVector RecoilViewModTarget;
	r3dVector RecoilViewModTarget2; // used for when you are being shot

	r3dVector ViewAngle;		// mouse controlled view angle, in degrees. 
	r3dPoint3D RealAcceleration; // actual player speed, after weight mod, jump, gravity, etc
	r3dPoint3D InputAcceleration; // that is the desired accel based on what buttons player pressed
	float m_RemainingSecondWindTime;  // for the second wind Ability

	r3dVector m_vVision;		// current vector of vision view

	D3DXMATRIX 	DrawRotMatrix;	// character rotation matrix
	D3DXMATRIX 	DrawFullMatrix;
	D3DXMATRIX 	DrawFullMatrix_Localized;

	D3DXMATRIX	MoveMatrix;
	r3dVector	GetvForw() const { return r3dVector(MoveMatrix._31, MoveMatrix._32, MoveMatrix._33); }
	r3dVector	GetvRight() const { return r3dVector(MoveMatrix._11, MoveMatrix._12, MoveMatrix._13); }
	r3dVector	GetvRightDraw() const { return r3dVector(DrawRotMatrix._11, DrawRotMatrix._12, DrawRotMatrix._13); }

	float		m_fPlayerRotation;
	float		m_fPlayerRotationTarget;
	void		UpdateRotation();
	void		 UpdateUpperBodyAngLegs();
	void		 UpdateRotationToTarget(float fY, float fTurnSpeed);

	CNetCellMover	netMover;

	// set player position and reset cell base, so it'll be updated on next movement
	void TeleportPlayer(const r3dPoint3D& pos, const char* reason)
	{
		//r3dOutToLog("... %p: TeleportPlayer %s\n", this, reason);
		SetPosition(pos);
		netMover.Teleport(pos);
	}

	/**	Turn on or off physics simulation for character physics (basically remove/add actors from scene). */
	void TogglePhysicsSimulation(bool on);

#ifdef VEHICLES_ENABLED
	obj_Vehicle* currentVehicle;
	void RequestVehicleEnter(DWORD vehicleId);
	void RequestVehicleExit(DWORD vehicleId);

	obj_Vehicle* IsNearVehicle();

	void OnCollide(PhysicsCallbackObject *obj, CollisionInfo &trace);
#endif

	float		afterRespawnTimer;
	float		lastTimeWeaponSwitch;	// to prevent switching weapon and keep firing
	float		lastTimeAirHornUse;	// to prevent use many times at same time the AirHorn
	float		m_ZombieRepelentTime;

	r3dPoint3D		viewTargetPos;
	r3dPoint3D		viewTargetNorm;


	void		UpdateTargetting();
	void		PlayerFired(int hitId, const r3dPoint3D& pos);
	r3dPoint3D	GetMuzzlerPosition();

	int			CalcZombieThreatToPlayer( ThreatStats& threatStat, const r3dPoint3D& atPosition );
	int			GetGraduatedZombieThreat();

	void		DoDeath(int networdIDKiller, bool forced_by_server, STORE_CATEGORIES damageSource); // not passing the actual pointer, to make sure that we will not later access dead pointer, in case if killer left the game
	void		ResPawnFast();
	void		Undie();
	void		DoRagdoll(bool toRagdoll, int boneId = 0, const r3dPoint3D* impulse = NULL, bool isVehicleHit = false);

	void		DrawLabel();
	void		DrawReticle();
	float		m_ReticleTargetScale;
	float		m_ReticleCurrentScale;

	bool		m_isFemaleHero;

#ifdef VEHICLES_ENABLED
	int			seatPosition;
#endif

public:
	int			Darkness;

	// UAV
	bool		uavViewActive_;
	int		uavRequested_;
	DWORD		UavNid_;
	gobjid_t	uavId_;
	void		ProcessSpawnUAV();
	bool		 GetUAVSpawnPos(r3dPoint3D* pos);
	void		ToggleUAVView(bool force_disable=false);
	bool		HideChat;

	virtual	BOOL		Load(const char *name);
	virtual	BOOL		OnCreate();			
	virtual	BOOL		OnDestroy();		

	virtual	BOOL		Update();			

	void				UpdateLocalPlayerMovement();

	virtual void		AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& Cam ) OVERRIDE;
	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )  OVERRIDE;

	virtual	BOOL		OnNetReceive(DWORD EventID, const void* packetData, int packetSize);
	void			 OnNetPacket(const PKT_S2C_MoveTeleport_s& n);
	void			 OnNetPacket(const PKT_C2C_MoveSetCell_s& n);
	void			 OnNetPacket(const PKT_C2C_MoveRel_s& n);
	void			 OnNetPacket(const PKT_C2C_PlayerJump_s& n);
	void			 OnNetPacket(const PKT_S2C_AddScore_s& n);
	void			 OnNetPacket(const PKT_S2C_AddResource_s& n);
	void			 OnNetPacket(const PKT_S2C_SetPlayerVitals_s& n);
	void			 OnNetPacket(const PKT_S2C_SetPlayerLoadout_s& n);
	void			 OnNetPacket(const PKT_S2C_SetPlayerAttachments_s& n);
	void			 OnNetPacket(const PKT_S2C_SetPlayerWorldFlags_s& n);
	void			 OnNetPacket(const PKT_S2C_PlayerRawDamage_s& n);
	void			 OnNetPacket(const PKT_C2C_PlayerSwitchWeapon_s& n);
	void			 OnNetPacket(const PKT_C2C_PlayerSwitchFlashlight_s& n);
	void			 OnNetPacket(const PKT_C2C_PlayerReadyGrenade_s& n);
	void			 OnNetPacket(const PKT_C2C_PlayerThrewGrenade_s& n);
	void			 OnNetPacket(const PKT_C2C_PlayerReload_s& n);
	void			 OnNetPacket(const PKT_C2C_PlayerFired_s& n);
	void			 OnNetPacket(const PKT_C2C_PlayerHitNothing_s& n);
	void			 OnNetPacket(const PKT_C2C_PlayerHitStatic_s& n);
	void			 OnNetPacket(const PKT_C2C_PlayerHitStaticPierced_s& n);
	void			 OnNetPacket(const PKT_C2C_PlayerHitDynamic_s& n);
	void			 OnNetPacket(const PKT_S2C_PlayerRemoveAttachments_s& n);
	void			 OnNetPacket(const PKT_S2C_SpawnExplosion_s& n);
	void			 OnNetPacket(const PKT_C2C_PlayerUseItem_s& n);
	void			 OnNetPacket(const PKT_S2C_PlayerUsedItemAns_s& n);
	void			 OnNetPacket(const PKT_S2C_BackpackAddNew_s& n);
	void			 OnNetPacket(const PKT_S2C_BackpackModify_s& n);
	void			 OnNetPacket(const PKT_S2C_BackpackReplace_s& n);
	void			 OnNetPacket(const PKT_S2C_BackpackUnlock_s& n);
	void			 OnNetPacket(const PKT_S2C_RepairItemAns_s& n);
	void			 OnNetPacket(const PKT_S2C_InventoryAddNew_s& n);
	void			 OnNetPacket(const PKT_S2C_InventoryModify_s& n);
	void			 OnNetPacket(const PKT_S2C_InventoryOpAns_s& n);
	void			 OnNetPacket(const PKT_S2C_PlayerWeapDataRepReq_s& n);
	void			 OnNetPacket(const PKT_C2C_TradeRequest_s& n);
	void			 OnNetPacket(const PKT_C2C_TradeItem_s& n);
	void			 OnNetPacket(const PKT_S2C_CraftAns_s& n);

	virtual void		UpdateTransform();

	void		stopFootstepsSound();

	bool		bAllowToUseWeapons;
	void		CheckFireWeapon();
	float		GetGrenadeLaunchDelay();
	void		GrenadeCallbackFromWeapon(const wiInventoryItem& wi);

	void		OnGameEnded();
	BYTE		getBodyNameFromBoneName(const char* boneName);

	void		localPlayer_UseItem(int slotID, uint32_t itemID, const r3dPoint3D& pos, float var1=0.0f, float var2=0.0f, float var3=0.0f, DWORD var4=0);

#ifdef VEHICLES_ENABLED
	bool isHitByVehicle;

	enum VehicleViews
	{
		VehicleView_None,
		VehicleView_Driver,
		VehicleView_Passenger,
	};

	VehicleViews vehicleViewActive_;

	void enterVehicle( obj_Vehicle* target_Vehicle, int seat );
	void exitVehicle(obj_Vehicle* targetVehicle, r3dPoint3D exitPosition);
	bool IsInVehicle();
	bool TestExitPosition( r3dVector &exitPosition, bool ignoreGround = false );
	void AttemptVehicleHit(PxF32 vehicleSpeed, obj_Vehicle* vehicle);
	void SetCurrentVehicleId(DWORD vehicleId, int seatPosition);

	void PhysicsEnable( const int& physicsEnabled );
#endif 

	bool isVoiceActive() const { return m_isVoiceActive; }
	void setVoiceActive(bool set);

private:
	bool		m_isVoiceActive;

	void		executeWeaponFireLocalPlayer(const r3dPoint3D& from, float recoil, float spread, float recoilCoolDownMod, float holdingDelay, int fireSeqNo, bool checkAgainsMuzzler, int damagePercFromPiercing);

	/**	Footsteps sound. */
	float		footsteps_timeSinceLastMissingHitResult;
	void		*footStepsSnd;
	float		footstepsSndBaseVolume;
	struct footStepEntry_s {
		int		id;
		void*		snd;
		float		base_volume;
		footStepEntry_s() : id(-1), snd(0), base_volume(1.0f) {}
		footStepEntry_s(int i, void* s, float v):id(i), snd(s), base_volume(v) {}
	};
	r3dTL::TStaticArray<footStepEntry_s, 128> footStepsArr;
	int		currentFootStepsSoundID;
	void		UpdateFootsteps();
	void		UpdateSwimSounds();

	void		UpdateSounds();

	// sprinting sounds
	void*	m_sndBreathSprint;
	float	m_sndBreathBaseVolume;

	void*	m_sndInPain;

	void*	m_sndClothMovementSprint;

	void*	m_sndSniperBreath;
	void*	m_sndSniperHeartSlow;
	void*	m_sndSniperExhale;

	// swimming sounds
	void*	m_sndSwimIdle;
	void*	m_sndSwimSlow;
	void*	m_sndSwim;
	void*	m_sndSwimFast;
	int		m_sndSwimIdleId;
	int		m_sndSwimSlowId;
	int		m_sndSwimId;
	int		m_sndSwimFastId;

	void		UpdateActionUI();
public:

#ifdef VEHICLES_ENABLED
	bool isPlayerInVehicle;
	bool hasOpenVehicleRequest;
	DWORD currentVehicleId;
#endif

	CamoTimeLine camoTimeLine;

#ifndef FINAL_BUILD
	int		Permanufactura;
	float	Permanufactura_Counter;
#endif

};

extern void InitializePhysSkeletonCache( int count, float progressStart, float progressEnd ) ;
extern void DestroyPhysSkeletonCache() ;
extern r3dPhysSkeleton* AquireCacheSkeleton();
extern void ReleaseCachedPhysSkeleton(r3dPhysSkeleton* skel);

extern void InitializePhysObstacleCache(int count, int capsuleCount);
extern void DestroyPhysObstacleCache();
extern int AcquirePlayerObstacle(const r3dPoint3D& pos, const PxBoxObstacle& obstacle);
extern void ReleasePlayerObstacle(int* out_idx);
extern void UpdatePlayerObstacle(int idx, const r3dPoint3D& pos);
extern void UpdatePlayerObstacle(int idx, const r3dPoint3D& pos, const PxQuat& rot);
extern int AcquirePlayerObstacleCapsule(const r3dPoint3D& pos, const PxCapsuleObstacle& obstacle);
extern void ReleasePlayerObstacleCapsule(int* out_idx);
extern void UpdatePlayerObstacleCapsule(int idx, const r3dPoint3D& pos);


// for use when processing a bullet hit by obj_bullet or obj_Player::executeWeaponFireLocalPlayer
bool ProcessBulletHit( int &damageFromPiercable, GameObject* owner, const r3dPoint3D &hitPoint, const r3dPoint3D &hitNormal, GameObject* shootTarget, const r3dMaterial* shootMaterial, const char* hitActorName, const WeaponConfig *weaponInfo, const r3dPoint3D& muzzlerPosAtFireStart, gobjid_t bulletObjID);
