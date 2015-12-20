//=========================================================================
//	Module: obj_Vehicle.hpp
//	Copyright (C) 2011. Online Warmongers Group Inc. All rights reserved
//=========================================================================
#ifdef VEHICLES_ENABLED

#pragma once

#include "GameObj.h"
#include "vehicle/PxVehicleDrive.h"
#include "../../EclipseStudio/Sources/multiplayer/NetCellMover.h"
#include "../../EclipseStudio/Sources/ObjectsCode/Gameplay/SharedUsableItem.h"
#include "../../EclipseStudio/Sources/ObjectsCode/world/Lamp.h"
#include "../../EclipseStudio/Sources/ObjectsCode/Gameplay/obj_fakeglass.h"
//////////////////////////////////////////////////////////////////////////

class obj_VehicleSpawn;
struct VehicleDescriptor;

const uint32_t MAX_SEATS = 9;
const float MIN_MOVE_FOR_STUCK_MSG = 1.0f;

class VehicleSeat
{
public:
	VehicleSeat();
	~VehicleSeat();

	DWORD playerId;
	int seat;
	bool isWeaponAttached;
	bool isAllowedToShoot;
};

class obj_Vehicle: public SharedUsableItem
{
	DECLARE_CLASS(obj_Vehicle, GameObject);
	
	PKT_S2C_VehicleSpawn_s CreateParams;

	obj_VehicleSpawn* spawner;

	VehicleDescriptor *vd;

	obj_Building* Tread;

	bool isReady;

	CVehicleNetCellMover	netMover;
	r3dPoint3D		netVelocity;
	r3dVector		nextRotation;

	int TypeCamera;
	bool enableInteriorCam;

	obj_Player* players[MAX_SEATS];

	float wheelBones[17];
	float RotorX;
	float RotorY;
	float HSpeed;
	float HelyUp;
	float HelyX;
	float HelyY;
	float HelyZ;
	float HelyLR;
	float HelyFB;
	float FB;
	float LR;

	int physXObstacleIndex;
	
	PxReal lastWheel0Jounce;
	PxReal lastWheel1Jounce;
	PxReal lastWheel2Jounce;
	PxReal lastWheel3Jounce;

	PxReal lastWheelRotSpeeds[4];
	PxReal lastWheelTurnAngles[4];

	PxReal wheelRotSpeeds[4];
	PxReal wheelTurnAngles[4];

	float wheelSuspensionTravel[4];
	float lastWheelSuspensionTravel[4];

	float lastCrashSoundTime;;
	float crashSoundWaitTime;

	float m_LastClaxonUse;

	int sndCrashPlayerId;
	int sndCrashZombieId;
	int sndCrashMetalId;
	int sndCrashWoodId;
	int sndCrashRockId;

	int sndTractionLoopId;
	int sndEngineStartId;
	int sndEngineStopId;
	int sndEngineId;
	int sndTractionId;

	int sndRotateTurretId;
	int sndTurretStop;

	int sndPoliceClaxon;
	int sndVehicleClaxon;

	int sndExplosionId;
	int sndFireId;

	float camangle2;
	float camangle;
	float camangleBackup;
	float camangle2Backup;
	float cameraUpDown;
	bool RightMouseClick;
	float FireTank;

	void* sndTractionLoop;
	void* sndEngineStart;
	void* sndEngineStop;
	void* sndEngine;
	void* sndTraction;
	void* sndExplosion;
	void* sndFire;

	bool isSoundsLoaded;

	float lastRotationX;
	float lastRotationY;
	float lastRotationZ;

	void	*TurretSnd;
	void	*GunSnd;
	void	*TreadSnd;

	void	*ClaxonSnd;

	// temporary
	float currentRpm;
	bool isCarDrivable;

	r3dVector lastCollisionNormal;
	r3dVector lastRotationVector;

	char* particleExplosionFile;
	char* particleSmokeLevel0File;
	char* particleSmokeLevel1File;
	char* particleSmokeLevel2File;

	class obj_ParticleSystem* particleExplosion;
	class obj_ParticleSystem* particleDamage;

	int currentDamageLevel;
	bool isDamageParticlesLoaded;
	bool isSmoking;
	bool isOnFire;
	bool hasExploded;
	bool isExplosionLoaded;

	r3dMesh* damageMesh;
	r3dMesh* WeaponMesh;

	bool durabilityChanged;
	int curDurability;
	int maxDurability;

	float lastFuelCheckTime;
	float fuelCheckWaitTime;
	int maxFuel;
	int curFuel;
	bool fuelChanged;

	float lastSpeedCheckTime;
	float speedCheckWaitTime;
	float reportedSpeed;
	float reportedRpm;
	float reportedSideSpeed;

	bool isActivePhysXVehicle;

	float lastMoveTime;
	float lastEngineRpmAdjustTime;

	r3dLight headLight1;
	r3dLight headLight2;

	r3dLight SirenLeft;
	r3dLight SirenRight;

	r3dLight SirenLeftROT;
	r3dLight SirenRightROT;

	int SirenRotation;

	void SetupBoxObstacle();
	void SetupHeadlights();
	void SetupPoliceSiren();
	void SetupPhysics();
	bool isHeadlightsEnabled;

	bool isPoliceSirenEnabled;

	float distanceFromGround;
	bool isSlowedDown;
	float slowedDownTime;
	float slowedDownWait;

#ifndef FINAL_BUILD
	BOOL CreateForEditor();
	void CheckEditorInput();
#endif

	float lastDriveTime;
	float stuckWait;
	r3dPoint3D startedAccelPosition;
	bool isCurrentlyStuck;
	bool isMoving;
	bool isStuckInFront;
	bool isStuckInBack;
	float lastStuckMessageTime;
	float stuckMessageWait;

	void CheckIsStuck();
	float GetMovedDistance();

	float sideSpeedVolume;

	bool isTank;
	
public:	
	bool isHelicopter;
	void EnableHeadlights(bool enabled);
	void ToggleHeadlights();

	void EnablePoliceSiren(bool enabled);
	void TogglePoliceSiren();

	void AddOrUpdatePlayer(obj_Player* player);
	obj_Player* GetPlayerById(DWORD playerId) const;
	obj_Player* GetPlayerBySeat(int seat);
	obj_FakeGlass* FakeGlass;
	bool RemovePlayerById(DWORD playerId);
	bool RemovePlayerBySeat(int seat);

	void UpdatePlayers();

	void HelicopterPhysx();

	void LoadSounds();
	void PlayEngineSound(bool isPlaying);
	void PlayEngineStartSound();
	void PlayEngineStopSound();
	void PlayTractionSound(bool isPlaying, int soundId);
	void PlayTractionLoopSound(bool isPlaying);
	void PlayExplosionSound();
	void PlayCrashSound(int objectType, const MaterialType* mt = NULL);

	void CheckDistanceFromGround();

	void EnterVehicle(obj_Player* player);
	void ExitVehicle(obj_Player* player);

	void SyncPhysicsPoseWithObjectPose();
	void SyncPhysicsObjectPose();
	void SetBoneMatrices();

	void SendActions(BYTE b);

	const char* GetExplosion() const { return particleExplosionFile; }
	const char* GetDamageParticles(int damageLevel) const 
	{ 
		if (damageLevel == 0)
			return particleSmokeLevel0File; 
		else if (damageLevel == 1)
			return particleSmokeLevel1File; 
		else if (damageLevel == 2)
			return particleSmokeLevel2File;

		return NULL;
	}

	// This must match server for loading of vehicles
	enum VehicleTypes
	{
		VEHICLETYPE_INVALID = -1,
		VEHICLETYPE_BUGGY = 0,
		VEHICLETYPE_STRYKER,
		VEHICLETYPE_ZOMBIEKILLER,
		VEHICLETYPE_HUMMER,
		VEHICLETYPE_POLICE,
		VEHICLETYPE_ABANDONEDSUV,
		VEHICLETYPE_BONECRUSHER,
		VEHICLETYPE_COPCAR,
		VEHICLETYPE_DUNEBUGGY,
		VEHICLETYPE_ECONOLINE,
		VEHICLETYPE_LARGETRUCK,
		VEHICLETYPE_MILITARYAPC,
		VEHICLETYPE_PARAMEDIC,
		VEHICLETYPE_SUV,
		VEHICLETYPE_JEEP,
		VEHICLETYPE_TANK_T80,
		VEHICLETYPE_HELICOPTER
	};
	
public:
	obj_Vehicle();
	~obj_Vehicle();
	virtual BOOL Update();
	virtual BOOL OnCreate();
	virtual BOOL OnDestroy();

	virtual void SetPosition(const r3dPoint3D& pos);
	void SetPositionNoPose(const r3dPoint3D& pos);
	virtual	void SetRotationVector(const r3dVector& Angles);
	virtual void OnPreRender() { SetBoneMatrices(); }

	void SwitchToDrivable(bool doDrive);
	const VehicleDescriptor* getVehicleDescriptor() { return vd; }

	bool HasPlayers() const;
	bool HasDriver();
	bool HasPassenger();

	bool IsOnGround();

	void Repair();

	void DoSlowDown();

	int GetArmor() const;
	int GetWeight() const;
	int GetDurability();
	void SetDurability(short durability, bool isOnLoad = false);

	void DoExplosion();
	void StartDamageParticles(bool enable, int damageLevel);

	r3dVector GetvForw() const 
	{ 
		D3DXMATRIX m = GetRotationMatrix();
		return r3dVector(m._31, m._32, m._33); 
	}
	r3dVector GetvRight() const 
	{ 
		D3DXMATRIX m = GetRotationMatrix();
		return r3dVector(m._11, m._12, m._13); 
	}
	
	void IsGoingToCollide(r3dPoint3D dir);

#ifndef FINAL_BUILD
	float DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected);

	void DrawDebugInfo() const;
#endif

	const bool getExitSpace( r3dVector& outVector, int exitIndex );
	const bool getFireSpace( r3dVector& outVector, int exitIndex );
	const bool getTurretPosition( r3dVector& outVector, int exitIndex );
	void setVehicleSpawner( obj_VehicleSpawn* targetSpawner) { spawner = targetSpawner;}
	void SendUpdate();
	void UpdatePositionFromRemote();
	void UpdatePositionFromPhysx();

	void ResetVehicle();
	void ReleaseDrivable();

	bool IsCarDrivable() { return isCarDrivable; }

	void UpdatePhysicsEnabled( const int& physicsEnabled )
	{
		if(physicsEnabled != (int)m_bEnablePhysics)
		{
			m_bEnablePhysics = physicsEnabled?true:false;
			if(!m_bEnablePhysics)
			{
				SAFE_DELETE(PhysicsObject);
			}
			else
				CreatePhysicsData();
		}
	}

public:
	void StartTrackingPosition();
	bool IsStuck();
	void SetIsMoving();
	void ClearIsMoving();

	bool wasNetworkLocal;
	DWORD vehicleId;

	PxReal GetEngineRpm();
	void UpdateRotor();
	void UpdateSounds();

	void PhysicsEnable( const int& physicsEnabled );
	int GetFuelAsPercent();
	void SetFuel(int amount);
	void OnRunOutOfGas();
	bool CheckFuel();
	bool HasFuel();

	bool CanSmashBarricade();
	float GetSpeed();
	int GetDisplayDurability();
	int GetDurabilityAsPercent();
	int GetSpeedAsPercent();
	int GetRpmAsPercent();

	void UpdateUI();

	void PoliceSirensUpdate();
	void UpdatePoliceSirens();
	void UpdateHeadlights();
	void SendTurretData();
	void CheckInput();

	class obj_ParticleSystem* m_FireTank;
public:
	virtual void AppendRenderables( RenderArray ( & render_arrays )[ rsCount ], const r3dCamera& Cam );
	virtual void AppendShadowRenderables(RenderArray &rarr, int sliceIndex, const r3dCamera& cam);

	BOOL OnNetReceive(DWORD EventID, const void* packetData, int packetSize);
	void OnNetPacket(const PKT_C2C_VehicleMoveSetCell_s& n);
	void OnNetPacket(const PKT_C2C_VehicleMoveRel_s& n);
	void OnNetPacket(const PKT_S2C_VehicleUnstuck_s& n);
	void OnNetPacket(const PKT_C2C_VehicleAction_s& n);
	void OnNetPacket(const PKT_S2C_VehicleDurability_s &n);
	void OnNetPacket(const PKT_S2C_VehicleFuel_s& n);
	void OnNetPacket(const PKT_C2C_VehicleHeadlights_s& n);
	void OnNetPacket(const PKT_C2S_TurrerAngles_s& n);

	virtual void OnContactModify(PhysicsCallbackObject* obj, PxContactSet& contacts);
	virtual	void OnCollide(PhysicsCallbackObject *obj, CollisionInfo &trace);
};

#endif
