//=========================================================================
//	Module: obj_Vehicle.cpp
//	Copyright (C) 2011. Online Warmongers Group Inc. All rights reserved
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "PhysXWorld.h"
#include "ObjManag.h"
#include "GameCommon.h"

#include "obj_Vehicle.h"
#include "VehicleDescriptor.h"

#include "../../EclipseStudio/Sources/Editors/ObjectManipulator3d.h"
#include "../../EclipseStudio/Sources/ObjectsCode/Gameplay/obj_VehicleSpawn.h"
#include "../../EclipseStudio/Sources/multiplayer/ClientGameLogic.h"
#include "../../EclipseStudio/Sources/ObjectsCode/AI/AI_Player.H"

#include "../../EclipseStudio/Sources/ObjectsCode/Gameplay/obj_Zombie.h"
#include "../../EclipseStudio/Sources/ObjectsCode/WEAPONS/Barricade.h"
#include "../../EclipseStudio/Sources/ObjectsCode/EFFECTS/obj_ParticleSystem.h"
#include "../../EclipseStudio/Sources/ObjectsCode/weapons/ExplosionVisualController.h"
#include "../../EclipseStudio/Sources/ObjectsCode/world/MaterialTypes.h"
#include "../../EclipseStudio/Sources/UI/HUDDisplay.h"
#include "../../EclipseStudio/Sources/UI/HUDPause.h"

#include "fmod/SoundSys.h"

#ifdef VEHICLES_ENABLED

extern bool g_bEditMode;
extern ObjectManipulator3d g_Manipulator3d;
extern int CurHUDID;
extern HUDDisplay*	hudMain;
extern HUDPause*	hudPause;
extern 	PlayerStateVars_s CurrentRig;

IMPLEMENT_CLASS(obj_Vehicle, "obj_Vehicle", "Object");
AUTOREGISTER_CLASS(obj_Vehicle);

//////////////////////////////////////////////////////////////////////////

namespace
{
	struct VehicleMeshesDeferredRenderable: public MeshDeferredRenderable
	{
		VehicleMeshesDeferredRenderable()
			: Parent(0)
		{
		}

		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw(Renderable* RThis, const r3dCamera& Cam)
		{
			R3DPROFILE_FUNCTION("VehicleMeshesDeferredRenderable");

			VehicleMeshesDeferredRenderable* This = static_cast<VehicleMeshesDeferredRenderable*>(RThis);

			r3dApplyPreparedMeshVSConsts(This->Parent->preparedVSConsts);

			This->Parent->OnPreRender();

			MeshDeferredRenderable::Draw(RThis, Cam);
		}

		obj_Vehicle *Parent;
	};

	struct VehicleMeshesShadowRenderable : public MeshShadowRenderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw(Renderable* RThis, const r3dCamera& Cam)
		{
			R3DPROFILE_FUNCTION("VehicleShadowRender");
			VehicleMeshesShadowRenderable* This = static_cast<VehicleMeshesShadowRenderable*>(RThis);

			r3dApplyPreparedMeshVSConsts(This->parent->preparedVSConsts);

			This->parent->OnPreRender();

			This->SubDrawFunc( RThis, Cam );
		}

		obj_Vehicle* parent;
	};

	struct VehicleDebugRenderable : Renderable
	{
		void Init()
		{
			DrawFunc = Draw;
		}

		static void Draw( Renderable* RThis, const r3dCamera& Cam )
		{
			VehicleDebugRenderable* This = static_cast<VehicleDebugRenderable*>( RThis );

#ifndef FINAL_BUILD
			This->Parent->DrawDebugInfo();
#endif
		}

		obj_Vehicle* Parent;
	};

	void QuaternionToEulerAngles(PxQuat &q, float &xRot, float &yRot, float &zRot)
	{
		q.normalize();

		void MatrixGetYawPitchRoll ( const D3DXMATRIX & mat, float & fYaw, float & fPitch, float & fRoll );
		PxMat33 mat(q);
		D3DXMATRIX res;
		D3DXMatrixIdentity(&res);
		res._11 = mat.column0.x;
		res._12 = mat.column0.y;
		res._13 = mat.column0.z;

		res._21 = mat.column1.x;
		res._22 = mat.column1.y;
		res._23 = mat.column1.z;

		res._31 = mat.column2.x;
		res._32 = mat.column2.y;
		res._33 = mat.column2.z;

		MatrixGetYawPitchRoll(res, xRot, yRot, zRot);
	}

//////////////////////////////////////////////////////////////////////////

	/** Helper constant transformation factors */
	struct UsefulTransforms 
	{
		PxQuat rotY_quat;
		D3DXMATRIX rotY_mat;

		UsefulTransforms()
		{
			D3DXQUATERNION rotY_D3D;
			D3DXQuaternionRotationYawPitchRoll(&rotY_D3D, D3DX_PI / 2, 0, 0);
			rotY_quat = PxQuat(rotY_D3D.x, rotY_D3D.y, rotY_D3D.z, rotY_D3D.w);
			D3DXMatrixRotationQuaternion(&rotY_mat, &rotY_D3D);
		}
	};
}

void obj_Vehicle::AppendRenderables(RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& cam)
{
	r3dMesh* mesh;

	if (hasExploded)
		mesh = damageMesh;
	else 
		mesh = MeshLOD[0];
	if (!mesh)
		return;

	uint32_t prevCount = render_arrays[ rsFillGBuffer ].Count();
	mesh->AppendRenderablesDeferred( render_arrays[ rsFillGBuffer ], r3dColor::white, GetSelfIllumMultiplier() );

	for( uint32_t i = prevCount, e = render_arrays[ rsFillGBuffer ].Count(); i < e; i ++ )
	{
		VehicleMeshesDeferredRenderable& rend = static_cast<VehicleMeshesDeferredRenderable&>( render_arrays[ rsFillGBuffer ][ i ] ) ;

		rend.Init();
		rend.Parent = this;
	}

#ifndef FINAL_BUILD
	if(r_show_vehicle_debug->GetBool())
	{
		VehicleDebugRenderable rend;
		rend.Init();
		rend.Parent		= this;
		rend.SortValue	= 2*RENDERABLE_USER_SORT_VALUE;
		render_arrays[ rsDrawFlashUI ].PushBack( rend );
	}
#endif
}

void obj_Vehicle::AppendShadowRenderables(RenderArray &rarr, int sliceIndex, const r3dCamera& cam)
{
	float distSq = (gCam - GetPosition()).LengthSq();
	float dist = sqrtf( distSq );
	UINT32 idist = (UINT32) R3D_MIN( dist * 64.f, (float)0x3fffffff );

	if( !gDisableDynamicObjectShadows && sliceIndex != r_active_shadow_slices->GetInt() - 1 )
	{
		r3dMesh* mesh;

		if (hasExploded)
			mesh = damageMesh;
		else 
			mesh = MeshLOD[0];
		if (!mesh)
			return;


		uint32_t prevCount = rarr.Count();

		mesh->AppendShadowRenderables( rarr );

		for( uint32_t i = prevCount, e = rarr.Count(); i < e; i ++ )
		{
			VehicleMeshesShadowRenderable& rend = static_cast<VehicleMeshesShadowRenderable&>( rarr[ i ] );

			rend.Init() ;
			rend.SortValue |= idist;
			rend.parent = this;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

obj_Vehicle::obj_Vehicle()
: vd(0)
, netMover(this, 0.1f, (float)PKT_C2C_MoveSetCell_s::VEHICLE_CELL_RADIUS)
, isCarDrivable(false)
, isSmoking(false)
, isOnFire(false)
, hasExploded(false)
, isDamageParticlesLoaded(false)
, isExplosionLoaded(false)
, durabilityChanged(false)
, curDurability(0)
, currentDamageLevel(0)
, crashSoundWaitTime(1.0f)
, physXObstacleIndex(-1)
, lastFuelCheckTime(0.0f)
, fuelCheckWaitTime(1.0f)
, maxFuel(0)
, curFuel(0)
, fuelChanged(false)
, sndTractionId(0)
, wasNetworkLocal(false)
, isActivePhysXVehicle(false)
, lastSpeedCheckTime(0.0f)
, speedCheckWaitTime(0.3f)
, reportedRpm(0)
, reportedSpeed(0)
, reportedSideSpeed(0.0f)
, lastMoveTime(0)
, lastEngineRpmAdjustTime(0)
, isHeadlightsEnabled(false)
, isPoliceSirenEnabled(false)
, distanceFromGround(0.0f)
, startedAccelPosition(0,0,0)
, lastDriveTime(0)
, stuckWait(2)
, isMoving(false)
, isStuckInFront(false)
, isStuckInBack(false)
, isCurrentlyStuck(false)
, lastStuckMessageTime(0)
, stuckMessageWait(30)
, sideSpeedVolume(0.0f)
, isSoundsLoaded(false)
, isTank(false)
, isReady(false)
, slowedDownTime(0)
, slowedDownWait(1)
, isSlowedDown(false)
, camangle2(0)
, camangle(0)
, camangleBackup(0)
, camangle2Backup(0)
, cameraUpDown(0)
, isHelicopter(false)
, RotorX(0)
, RotorY(0)
, HSpeed(2.0f)
, HelyUp(0.0f)
, HelyX(0.0f)
, HelyY(0.0f)
, HelyZ(0.0f)
, HelyLR(20.0f)
, HelyFB(20.0f)
, FB(0)
, LR(0)
, SirenRotation(0)
, m_LastClaxonUse(0)
{
	m_bEnablePhysics = false;

	ObjTypeFlags |= OBJTYPE_Vehicle;
	spawner = NULL;
	m_isSerializable = false;
	m_FireTank=NULL;
	FakeGlass=NULL;
	lastCrashSoundTime = r3dGetTime();
}

//////////////////////////////////////////////////////////////////////////

obj_Vehicle::~obj_Vehicle()
{
	g_pPhysicsWorld->m_VehicleManager->DeleteCar(vd);
	if( spawner )
	{
		spawner = NULL;
	}

	if (headLight1.pLightSystem)
		WorldLightSystem.Remove(&headLight1);
	
	if (headLight2.pLightSystem)
		WorldLightSystem.Remove(&headLight2);

	if (SirenLeft.pLightSystem)
		WorldLightSystem.Remove(&SirenLeft);

	if (SirenRight.pLightSystem)
		WorldLightSystem.Remove(&SirenRight);

	if (SirenLeftROT.pLightSystem)
		WorldLightSystem.Remove(&SirenLeftROT);

	if (SirenRightROT.pLightSystem)
		WorldLightSystem.Remove(&SirenRightROT);

	memset(players, 0, sizeof(obj_Player*) * MAX_SEATS);
}

//////////////////////////////////////////////////////////////////////////

BOOL obj_Vehicle::OnCreate()
{	
	if (!parent::OnCreate())
		return FALSE;
	
	TypeCamera=0;
	enableInteriorCam = false;

#ifndef FINAL_BUILD
	if (g_bEditMode)
		return CreateForEditor();
#endif

	vehicleId = CreateParams.vehicleId;
	
	particleExplosionFile = "vehicle_explosion";
	particleSmokeLevel0File = "vehicle_damage_01";
	particleSmokeLevel1File = "vehicle_damage_02";
	particleSmokeLevel2File = "vehicle_damage_03";

	r3dMesh *m = MeshLOD[0];
	if (!m)
		return FALSE;

	if (CreateParams.vehicleType == VEHICLETYPE_BUGGY)
		damageMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\Vehicles\\Drivable_Buggy_02_DMG.sco");
	else if (CreateParams.vehicleType == VEHICLETYPE_STRYKER)
		damageMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\Vehicles\\Drivable_Stryker_DMG.sco");
	else if (CreateParams.vehicleType == VEHICLETYPE_ZOMBIEKILLER)
		damageMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\Vehicles\\Zombie_killer_car_dmg.sco");
	else if (CreateParams.vehicleType == VEHICLETYPE_HUMMER)
		damageMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\Vehicles\\driveable_hummer_DMG.sco");
	else if (CreateParams.vehicleType == VEHICLETYPE_POLICE)
		damageMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\Vehicles\\driveable_Police_dmg.sco");
	else if (CreateParams.vehicleType == VEHICLETYPE_ABANDONEDSUV)
		damageMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_Vehicles\\drivable_abandoned_suv_dmg.sco");
	else if (CreateParams.vehicleType == VEHICLETYPE_BONECRUSHER)
		damageMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_Vehicles\\drivable_bonecrusher_dmg.sco");
	else if (CreateParams.vehicleType == VEHICLETYPE_COPCAR)
		damageMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_Vehicles\\drivable_cop_car_dmg.sco");
	else if (CreateParams.vehicleType == VEHICLETYPE_DUNEBUGGY)
		damageMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_Vehicles\\drivable_dune_buggy_dmg.sco");
	else if (CreateParams.vehicleType == VEHICLETYPE_ECONOLINE)
		damageMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_Vehicles\\drivable_econo_line_dmg.sco");
	else if (CreateParams.vehicleType == VEHICLETYPE_LARGETRUCK)
		damageMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_Vehicles\\drivable_large_truck_dmg.sco");
	else if (CreateParams.vehicleType == VEHICLETYPE_MILITARYAPC)
		damageMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_Vehicles\\drivable_military_apc_dmg.sco");
	else if (CreateParams.vehicleType == VEHICLETYPE_PARAMEDIC)
		damageMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_Vehicles\\drivable_paramedic_car_dmg.sco");
	else if (CreateParams.vehicleType == VEHICLETYPE_SUV)
		damageMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\G3_Vehicles\\drivable_suv_dmg.sco");
	else if (CreateParams.vehicleType == VEHICLETYPE_JEEP)
		damageMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\Vehicles\\driveable_jeep_dmg.sco");
	else if (CreateParams.vehicleType == VEHICLETYPE_HELICOPTER)
	{
		isHelicopter = true;
		damageMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\Vehicles\\Drive_Bell212.sco");
	}
	else if (CreateParams.vehicleType == VEHICLETYPE_TANK_T80)
	{
		isTank = true;
		damageMesh = r3dGOBAddMesh("Data\\ObjectsDepot\\Vehicles\\Drivable_T80_dmg.sco");
	}
	
	if (!damageMesh)
		return FALSE;
	vd = g_pPhysicsWorld->m_VehicleManager->CreateVehicle(m, this, PhysicsConfig);
	if (vd)
	{
		//	Set position and orientation for car
		SwitchToDrivable(false);
		SyncPhysicsPoseWithObjectPose();

		r3dBoundBox bb = GetBBoxLocal();
		std::swap(bb.Size.x, bb.Size.z);
		std::swap(bb.Org.x, bb.Org.z);
		SetBBoxLocal(bb);
		FireTank=r3dGetTime();
		vd->owner = this;
		RotorX=0.0f;
		RotorY=0.0f;
		HSpeed=2.0f;
		HelyUp=0.0f;
		HelyX=0.0f;
		HelyY=0.0f;
		HelyZ=0.0f;
		g_pPhysicsWorld->m_VehicleManager->UpdateFrictionValues((int)CreateParams.vehicleType, vd->tireFriction);

		if( !g_bEditMode )
		{
			char MeshFilenameGlass[512]="";
			r3dscpy(MeshFilenameGlass, FileName.c_str());
			int len = strlen(MeshFilenameGlass);
			r3dscpy(&MeshFilenameGlass[len-4], "_fakeglass.sco");
			if (r3dMesh::CanLoad(MeshFilenameGlass))
			{
				FakeGlass = (obj_FakeGlass*)srv_CreateGameObject("obj_FakeGlass", MeshFilenameGlass, GetPosition());
				FakeGlass->OnCreate();
				FakeGlass->SetFakeGlassObject(this);
			}
		}
	}

	//r3dOutToLog("!!! Vehicle Specs: MO:%.4f PT:%.4f, Mass:%.4f\n", (float)CreateParams.maxOmega, (float)CreateParams.peakTorque,(float)CreateParams.weight);
	
	vd->engineData.mMaxOmega = (float)CreateParams.maxOmega;
	vd->engineData.mPeakTorque = (float)CreateParams.peakTorque;
	vd->chassisData.mMass = (float)CreateParams.weight;

	maxFuel = (short)CreateParams.maxFuel;
	curFuel = (short)CreateParams.curFuel;

	maxDurability = (int)CreateParams.maxDurability;
	curDurability = (int)CreateParams.curDurability;
	SetDurability(curDurability, true);

	memset(players, 0, sizeof(obj_Player*) * MAX_SEATS);

	// add players to vehicle
	if (CreateParams.hasPlayers)
	{
		for (int i = 0; i < CreateParams.playerCount; ++i)
		{
			if (!CreateParams.playersInVehicle[i])
				continue;

			GameObject* gameObj = GameWorld().GetNetworkObject(CreateParams.playersInVehicle[i]);
			if (!gameObj || !gameObj->isObjType(OBJTYPE_Human))
				continue;
			
			obj_Player* player = (obj_Player*)gameObj;

			// our player might show up first, if he's not in the vehicle, put him in.
			if (!player->IsInVehicle())
				player->enterVehicle(this, i);
		}
	}

	netMover.Teleport(GetPosition());

	m_ActionUI_Title = gLangMngr.getString("Vehicle");
	m_ActionUI_Msg = gLangMngr.getString("HoldEToEnterVehicle");

	currentRpm = 0.0f;
	isCarDrivable = true;

	LoadSounds();

	SetupBoxObstacle();
	SetupPhysics();
	
	SetRotationVector(r3dVector(CreateParams.rotationX, CreateParams.rotationY, CreateParams.rotationZ));
	lastRotationVector = GetRotationVector();

	NetworkLocal = false;
	wasNetworkLocal = false;

	isReady = true;

	SetupHeadlights();
	if (CreateParams.vehicleType == VEHICLETYPE_POLICE)
		SetupPoliceSiren();

	isHeadlightsEnabled = CreateParams.isHeadlightsEnabled;
	EnableHeadlights(CreateParams.isHeadlightsEnabled);

	return vd != 0;
}

#ifndef FINAL_BUILD
BOOL obj_Vehicle::CreateForEditor()
{	
	r3dMesh *m = MeshLOD[0];
	if (!m)
		return FALSE;

	if (m->FileName == "data/objectsdepot/vehicles/drivable_buggy_02.sco")
		CreateParams.vehicleType = VEHICLETYPE_BUGGY;
	else if (m->FileName == "data/objectsdepot/vehicles/Drivable_Stryker.sco")
		CreateParams.vehicleType = VEHICLETYPE_STRYKER;
	else if (m->FileName == "data/objectsdepot/vehicles/Zombie_killer_car.sco")
		CreateParams.vehicleType = VEHICLETYPE_ZOMBIEKILLER;
	else if (m->FileName == "data/objectsdepot/vehicles/driveable_hummer.sco")
		CreateParams.vehicleType = VEHICLETYPE_HUMMER;
	else if (m->FileName == "data/objectsdepot/vehicles/driveable_Police.sco")
		CreateParams.vehicleType = VEHICLETYPE_POLICE;
	else if (m->FileName == "data/objectsdepot/g3_vehicles/drivable_abandoned_suv.sco")
		CreateParams.vehicleType = VEHICLETYPE_ABANDONEDSUV;
	else if (m->FileName == "data/objectsdepot/g3_vehicles/drivable_bonecrusher.sco")
		CreateParams.vehicleType = VEHICLETYPE_BONECRUSHER;
	else if (m->FileName == "data/objectsdepot/g3_vehicles/drivable_cop_car.sco")
		CreateParams.vehicleType = VEHICLETYPE_COPCAR;
	else if (m->FileName == "data/objectsdepot/g3_vehicles/drivable_dune_buggy.sco")
		CreateParams.vehicleType = VEHICLETYPE_DUNEBUGGY;
	else if (m->FileName == "data/objectsdepot/g3_vehicles/drivable_econo_line.sco")
		CreateParams.vehicleType = VEHICLETYPE_ECONOLINE;
	else if (m->FileName == "data/objectsdepot/g3_vehicles/drivable_large_truck.sco")
		CreateParams.vehicleType = VEHICLETYPE_LARGETRUCK;
	else if (m->FileName == "data/objectsdepot/g3_vehicles/drivable_military_apc.sco")
		CreateParams.vehicleType = VEHICLETYPE_MILITARYAPC;
	else if (m->FileName == "data/objectsdepot/g3_vehicles/drivable_paramedic_car.sco")
		CreateParams.vehicleType = VEHICLETYPE_PARAMEDIC;
	else if (m->FileName == "data/objectsdepot/g3_vehicles/drivable_suv.sco")
		CreateParams.vehicleType = VEHICLETYPE_SUV;
	else if (m->FileName == "data/objectsdepot/vehicles/driveable_jeep.sco")
		CreateParams.vehicleType = VEHICLETYPE_JEEP;
	else if (m->FileName == "data/objectsdepot/vehicles/drive_bell212.sco")
	{
		isHelicopter = true;
		CreateParams.vehicleType = VEHICLETYPE_HELICOPTER;
	}
	else if (m->FileName == "data/objectsdepot/vehicles/Drivable_T80.sco")
	{
		CreateParams.vehicleType = VEHICLETYPE_TANK_T80;
		isTank = true;
	}

	vd = g_pPhysicsWorld->m_VehicleManager->CreateVehicle(m, this, PhysicsConfig);
	if (vd)
	{
		SwitchToDrivable(false);
		SyncPhysicsPoseWithObjectPose();

		r3dBoundBox bb = GetBBoxLocal();
		std::swap(bb.Size.x, bb.Size.z);
		std::swap(bb.Org.x, bb.Org.z);
		SetBBoxLocal(bb);
		FireTank=r3dGetTime();
		vd->owner = this;
		RotorX=0.0f;
		RotorY=0.0f;
		HSpeed=2.0f;
		HelyUp=0.0f;
		HelyX=0.0f;
		HelyY=0.0f;
		HelyZ=0.0f;
		//g_pPhysicsWorld->m_VehicleManager->UpdateFrictionValues((int)CreateParams.vehicleType, vd->tireFriction);

		if( !g_bEditMode )
		{
			char MeshFilenameGlass[512]="";
			r3dscpy(MeshFilenameGlass, FileName.c_str());
			int len = strlen(MeshFilenameGlass);
			r3dscpy(&MeshFilenameGlass[len-4], "_fakeglass.sco");
			FakeGlass = (obj_FakeGlass*)srv_CreateGameObject("obj_FakeGlass", MeshFilenameGlass, GetPosition());
			FakeGlass->OnCreate();
			FakeGlass->SetFakeGlassObject(this);
		}
	}

	m_ActionUI_Title = gLangMngr.getString("Vehicle");
	m_ActionUI_Msg = gLangMngr.getString("HoldEToEnterVehicle");

	isCarDrivable = true;

	memset(players, 0, sizeof(obj_Player*) * MAX_SEATS);

	LoadSounds();
	SetupPhysics();
	if (CreateParams.vehicleType == VEHICLETYPE_POLICE)
	{
		SetupPoliceSiren();
		TogglePoliceSiren();
	}

	return vd != 0;
}
#endif

void obj_Vehicle::SetupBoxObstacle()
{
	r3d_assert(physXObstacleIndex == -1);

	D3DXQUATERNION rotd3d;
	D3DXQuaternionRotationYawPitchRoll(&rotd3d, R3D_DEG2RAD(GetRotationVector().x), R3D_DEG2RAD(GetRotationVector().y), R3D_DEG2RAD(GetRotationVector().z));

	PxBoxObstacle obstacle;
	if (CreateParams.vehicleType == VEHICLETYPE_BUGGY)
		obstacle.mHalfExtents = PxVec3(1.8f, 2.2f, 2.3f);
	else if (CreateParams.vehicleType == VEHICLETYPE_STRYKER)
		obstacle.mHalfExtents = PxVec3(2.1f, 2.2f, 3.3f);
	else if (CreateParams.vehicleType == VEHICLETYPE_ZOMBIEKILLER)
		obstacle.mHalfExtents = PxVec3(2.0f, 2.2f, 2.8f);
	else if (CreateParams.vehicleType == VEHICLETYPE_HUMMER)
		obstacle.mHalfExtents = PxVec3(2.0f, 2.2f, 3.5f);
	else if (CreateParams.vehicleType == VEHICLETYPE_POLICE)
		obstacle.mHalfExtents = PxVec3(1.5f, 2.0f, 3.3f);
	else if (CreateParams.vehicleType == VEHICLETYPE_ABANDONEDSUV)
		obstacle.mHalfExtents = PxVec3(1.5f, 2.2f, 3.0f);
	else if (CreateParams.vehicleType == VEHICLETYPE_BONECRUSHER)
		obstacle.mHalfExtents = PxVec3(1.8f, 2.2f, 3.5f);
	else if (CreateParams.vehicleType == VEHICLETYPE_COPCAR)
		obstacle.mHalfExtents = PxVec3(1.5f, 2.2f, 2.8f);
	else if (CreateParams.vehicleType == VEHICLETYPE_DUNEBUGGY)
		obstacle.mHalfExtents = PxVec3(1.2f, 2.2f, 1.8f);                             
	else if (CreateParams.vehicleType == VEHICLETYPE_ECONOLINE)
		obstacle.mHalfExtents = PxVec3(1.5f, 2.2f, 3.5f);
	else if (CreateParams.vehicleType == VEHICLETYPE_LARGETRUCK)
		obstacle.mHalfExtents = PxVec3(1.8f, 3.2f, 5.0f);
	else if (CreateParams.vehicleType == VEHICLETYPE_MILITARYAPC)
		obstacle.mHalfExtents = PxVec3(1.5f, 3.2f, 3.0f);
	else if (CreateParams.vehicleType == VEHICLETYPE_PARAMEDIC)
		obstacle.mHalfExtents = PxVec3(2.0f, 2.2f, 4.5f);
	else if (CreateParams.vehicleType == VEHICLETYPE_SUV)
		obstacle.mHalfExtents = PxVec3(1.5f, 2.2f, 3.2f);
	else if (CreateParams.vehicleType == VEHICLETYPE_JEEP)
		obstacle.mHalfExtents =  PxVec3(1.534f, 4.202f, 2.327f);
	else if (CreateParams.vehicleType == VEHICLETYPE_HELICOPTER)
		obstacle.mHalfExtents = PxVec3(2.0f, 4.738f, 4.337f);
//izkierda-derecha//arriba-abajo//delante-atras
	else if (CreateParams.vehicleType == VEHICLETYPE_TANK_T80)
		obstacle.mHalfExtents = PxVec3(3.1f, 3.2f, 5.3f);

	obstacle.mRot = PxQuat(rotd3d.x, rotd3d.y, rotd3d.z, rotd3d.w);
	obstacle.mPos = PxExtendedVec3(GetPosition().x, GetPosition().y + 1.0f, GetPosition().z);
	obstacle.mUserData = (void*)this;

	physXObstacleIndex = AcquirePlayerObstacle(GetPosition(), obstacle);
}

void obj_Vehicle::SetupHeadlights()
{
	r3dVector frontLeft = GetPosition() + GetvForw() * 2.5f + -GetvRight() * 1.2f;
	r3dVector frontRight = GetPosition() + GetvForw() * 2.5f + GetvRight() * 1.2f;

	headLight1.Assign(frontLeft+r3dPoint3D(0,2,0));
	headLight1.Direction = GetvForw();
	headLight1.SetType(R3D_SPOT_LIGHT);
	headLight1.SetRadius(20.5f, 26.0f);
	headLight1.SetColor(255, 255, 255);
	headLight1.SpotAngleInner = 30.41f;
	headLight1.SpotAngleOuter = 60.21f;
	headLight1.bCastShadows = r_lighting_quality->GetInt() == 3 && r_shadows_quality->GetInt() >= 3;
	headLight1.Intensity = 5.31f;
	headLight1.SpotAngleFalloffPow = 1.6f;
	headLight1.bSSShadowBlur = 1;
	headLight1.bUseGlobalSSSBParams = 0;
	headLight1.SSSBParams.Bias = 0.1f;
	headLight1.SSSBParams.PhysRange = 43.12f;
	headLight1.SSSBParams.Radius = 40.0f;
	headLight1.SSSBParams.Sense = 635.18f;
	headLight1.ProjectMap = r3dRenderer->LoadTexture("data\\ProjectionTextures\\flashlight_02.dds");

	headLight1.TurnOff();
	WorldLightSystem.Add(&headLight1);

	headLight2.Assign(frontRight+r3dPoint3D(0,2,0));
	headLight2.Direction = GetvForw();
	headLight2.SetType(R3D_SPOT_LIGHT);
	headLight2.SetRadius(20.5f, 26.0f);
	headLight2.SetColor(255, 255, 255);
	headLight2.SpotAngleInner = 30.41f;
	headLight2.SpotAngleOuter = 60.21f;
	headLight2.bCastShadows = r_lighting_quality->GetInt() == 3 && r_shadows_quality->GetInt() >= 3;
	headLight2.Intensity = 5.31f;
	headLight2.SpotAngleFalloffPow = 1.6f;
	headLight2.bSSShadowBlur = 1;
	headLight2.bUseGlobalSSSBParams = 0;
	headLight2.SSSBParams.Bias = 0.1f;
	headLight2.SSSBParams.PhysRange = 43.12f;
	headLight2.SSSBParams.Radius = 40.0f;
	headLight2.SSSBParams.Sense = 635.18f;
	headLight2.ProjectMap = r3dRenderer->LoadTexture("data\\ProjectionTextures\\flashlight_02.dds");

	headLight2.TurnOff();
	WorldLightSystem.Add(&headLight2);
}

void obj_Vehicle::SetupPoliceSiren()
{
	r3dPoint3D PositionLeft,PositionRight;
	bool a = getTurretPosition(PositionLeft,26);
	bool b = getTurretPosition(PositionRight,27); 

	SirenLeft.Assign(GetPosition()+PositionLeft+r3dPoint3D(0,1,0));
	SirenLeft.SetType(R3D_OMNI_LIGHT);
	SirenLeft.SetRadius(0.5f, 0.5f);
	SirenLeft.SetColor(0, 0, 255);
	SirenLeft.bCastShadows = false;
	SirenLeft.Intensity = 16.0f;
	SirenLeft.SpotAngleFalloffPow = 1.6f;
	SirenLeft.bSSShadowBlur = 0;
	SirenLeft.bUseGlobalSSSBParams = 0;
	SirenLeft.SSSBParams.Bias = 0.1f;
	SirenLeft.SSSBParams.PhysRange = 0.53f;
	SirenLeft.SSSBParams.Radius = 0.53f;
	SirenLeft.SSSBParams.Sense = 635.18f;

	SirenLeft.TurnOff();
	WorldLightSystem.Add(&SirenLeft);

	SirenRight.Assign(GetPosition()+PositionRight+r3dPoint3D(0,1,0));
	SirenRight.SetType(R3D_OMNI_LIGHT);
	SirenRight.SetRadius(0.5f, 0.5f);
	SirenRight.SetColor(255, 0, 0);
	SirenRight.bCastShadows = false;
	SirenRight.Intensity = 16.0f;
	SirenRight.SpotAngleFalloffPow = 1.6f;
	SirenRight.bSSShadowBlur = 0;
	SirenRight.bUseGlobalSSSBParams = 0;
	SirenRight.SSSBParams.Bias = 0.1f;
	SirenRight.SSSBParams.PhysRange = 0.53f;
	SirenRight.SSSBParams.Radius = 0.53f;
	SirenRight.SSSBParams.Sense = 635.18f;

	SirenRight.TurnOff();
	WorldLightSystem.Add(&SirenRight);


/////////////////////////////////////////////////////////

	SirenLeftROT.Assign(GetPosition()+PositionLeft);
	SirenLeftROT.SetType(R3D_SPOT_LIGHT);
	SirenLeftROT.SetRadius(23.16f, 23.16f);
	SirenLeftROT.SetColor(0, 0, 255);
	SirenLeftROT.SpotAngleInner = 0.0f;
	SirenLeftROT.SpotAngleOuter = 100.0f;
	SirenLeftROT.bCastShadows = 0;
	SirenLeftROT.Intensity = 2.78f;
	SirenLeftROT.SpotAngleFalloffPow = 1.6f;
	SirenLeftROT.bSSShadowBlur = 0;
	SirenLeftROT.bUseGlobalSSSBParams = 0;
	SirenLeftROT.SSSBParams.Bias = 0.1f;
	SirenLeftROT.SSSBParams.PhysRange = 0.0f;
	SirenLeftROT.SSSBParams.Radius = 0.0f;
	SirenLeftROT.SSSBParams.Sense = 635.18f;
	SirenLeft.ProjectMap = r3dRenderer->LoadTexture("data\\ProjectionTextures\\flashlight_02.dds");

	SirenLeftROT.TurnOff();
	WorldLightSystem.Add(&SirenLeftROT);

	SirenRightROT.Assign(GetPosition()+PositionRight);
	SirenRightROT.SetType(R3D_SPOT_LIGHT);
	SirenRightROT.SetRadius(23.16f, 23.16f);
	SirenRightROT.SetColor(255, 0, 0);
	SirenRightROT.SpotAngleInner = 0.0f;
	SirenRightROT.SpotAngleOuter = 100.0f;
	SirenRightROT.bCastShadows = 0;
	SirenRightROT.Intensity = 2.78f;
	SirenRightROT.SpotAngleFalloffPow = 1.6f;
	SirenRightROT.bSSShadowBlur = 0;
	SirenRightROT.bUseGlobalSSSBParams = 0;
	SirenRightROT.SSSBParams.Bias = 0.1f;
	SirenRightROT.SSSBParams.PhysRange = 0.0f;
	SirenRightROT.SSSBParams.Radius = 0.0f;
	SirenRightROT.SSSBParams.Sense = 635.18f;
	SirenLeft.ProjectMap = r3dRenderer->LoadTexture("data\\ProjectionTextures\\flashlight_02.dds");


	SirenRightROT.TurnOff();
	WorldLightSystem.Add(&SirenRightROT);
}

void obj_Vehicle::UpdatePoliceSirens()
{
	if (isPoliceSirenEnabled)
	{
		// temporary until i get light bones.
		r3dPoint3D PositionLeft,PositionRight;
		bool a = getTurretPosition(PositionLeft,26);
		bool b = getTurretPosition(PositionRight,27); 

		D3DXMATRIX toOrigin,m;
		r3dPoint3D PosLeft = GetPosition()+PositionLeft;
		D3DXMatrixTranslation(&m, PosLeft.x, PosLeft.y, PosLeft.z);
		D3DXMatrixRotationYawPitchRoll(&toOrigin, R3D_DEG2RAD(SirenRotation), 0, 0);
		D3DXMatrixMultiply(&m, &toOrigin, &m);

		SirenLeft.Assign(GetPosition()+PositionLeft+r3dPoint3D(0,0.3,0));
		SirenLeftROT.Assign(GetPosition()+PositionLeft+r3dPoint3D(0,0.3,0));
		SirenLeftROT.Direction = r3dVector(m._31, m._32, m._33);

		D3DXMATRIX toOrigin2,m2;
		PosLeft = GetPosition()+PositionRight;
		D3DXMatrixTranslation(&m2, PosLeft.x, PosLeft.y, PosLeft.z);
		D3DXMatrixRotationYawPitchRoll(&toOrigin2, R3D_DEG2RAD(-SirenRotation), 0, 0);
		D3DXMatrixMultiply(&m2, &toOrigin2, &m2);

		SirenRight.Assign(GetPosition()+PositionRight+r3dPoint3D(0,0.3,0));
		SirenRightROT.Assign(GetPosition()+PositionRight+r3dPoint3D(0,0.3,0));
		SirenRightROT.Direction = r3dVector(m2._31, m2._32, m2._33);

		SirenRotation+=20;

	}
}

void obj_Vehicle::LoadSounds()
{
		char sndstartEngine[512];
		char sndstopEngine[512];
		char sndengineLoop[512];

		sprintf(sndstartEngine,"%s",vd->startEngine);
		sprintf(sndstopEngine,"%s",vd->stopEngine);
		sprintf(sndengineLoop,"%s",vd->engineLoop);

		sndEngineStartId = SoundSys.GetEventIDByPath(sndstartEngine);	
		sndEngineStopId = SoundSys.GetEventIDByPath(sndstopEngine);
		sndEngineId = SoundSys.GetEventIDByPath(sndengineLoop);

		sndRotateTurretId = SoundSys.GetEventIDByPath("Sounds/Vehicles/TurretSound/TurretRotate");
		sndTurretStop = SoundSys.GetEventIDByPath("Sounds/Vehicles/TurretSound/TurretStop");

		sndPoliceClaxon = SoundSys.GetEventIDByPath("Sounds/Vehicles/ClaxonSnd/Police");
		sndVehicleClaxon = SoundSys.GetEventIDByPath("Sounds/Vehicles/ClaxonSnd/Claxon");

	if (sndPoliceClaxon == -1)
		r3dOutToLog("Failed to load vehicle Police claxon sound.\n");
	if (sndVehicleClaxon == -1)
		r3dOutToLog("Failed to load vehicle Claxon sound.\n");

	if (sndTurretStop == -1)
		r3dOutToLog("Failed to load vehicle engine sound.\n");

	if (sndRotateTurretId == -1)
		r3dOutToLog("Failed to load vehicle engine sound.\n");

	if (sndEngineId == -1)
		r3dOutToLog("Failed to load vehicle engine sound.\n");

	if (sndEngineStartId == -1)
		r3dOutToLog("Failed to load vehicle start up sound.\n");

	if (sndEngineStopId == -1)
		r3dOutToLog("Failed to load vehicle shut down sound.\n");

	sndExplosionId = SoundSys.GetEventIDByPath("Sounds/Effects/Explosions/RPG01");
	if (sndExplosionId == -1)
		r3dOutToLog("Failed to load vehicle explosion sound.\n");

	sndFireId = SoundSys.GetEventIDByPath("Sounds/WarZ/PlayerSounds/PLAYER_FLARELOOP");
	if (sndFireId == -1)
		r3dOutToLog("Failed to load vehicle burning sound.\n");

	sndCrashZombieId = SoundSys.GetEventIDByPath("Sounds/Vehicles/Crashes/Crash_Zombie");
	if (sndFireId == -1)
		r3dOutToLog("Failed to load vehicle crash zombie sound.\n");

	sndCrashPlayerId = SoundSys.GetEventIDByPath("Sounds/Vehicles/Crashes/Crash_Player");
	if (sndFireId == -1)
		r3dOutToLog("Failed to load vehicle crash player sound.\n");

	sndCrashMetalId = SoundSys.GetEventIDByPath("Sounds/Vehicles/Crashes/Crash_Metal");
	if (sndFireId == -1)
		r3dOutToLog("Failed to load vehicle crash metal sound.\n");

	sndCrashWoodId = SoundSys.GetEventIDByPath("Sounds/Vehicles/Crashes/Crash_Wood");
	if (sndFireId == -1)
		r3dOutToLog("Failed to load vehicle crash wood sound.\n");

	sndCrashRockId = SoundSys.GetEventIDByPath("Sounds/Vehicles/Crashes/Crash_Rock");
	if (sndFireId == -1)
		r3dOutToLog("Failed to load vehicle crash rock sound.\n");

	sndTractionLoopId = SoundSys.GetEventIDByPath("Sounds/Vehicles/Braking/SkidLoop_AllSurfaces");
	if (sndTractionLoopId == -1)
		r3dOutToLog("Failed to load vehicle braking sounds.\n");

	isSoundsLoaded = true;
}

void obj_Vehicle::SetupPhysics()
{
	PhysicsConfig.type = PHYSICS_TYPE_MESH;
	PhysicsConfig.group = PHYSCOLL_COLLISION_GEOMETRY;
	PhysicsConfig.mass = vd->chassisData.mMass;
	PhysicsConfig.isFastMoving = true;
	PhysicsConfig.isVehicle = true;
	PhysicsConfig.ready = true;

	PhysicsEnable(true);
}
//////////////////////////////////////////////////////////////////////////

BOOL obj_Vehicle::OnDestroy()
{
#ifndef FINAL_BUILD
	if (!g_bEditMode)
#endif
		ReleasePlayerObstacle(&physXObstacleIndex);

	g_pPhysicsWorld->m_VehicleManager->RemoveActiveVehicle(vd);

	if (vd)	
		vd->owner = 0;

	if (SoundSys.IsHandleValid(sndEngine))
	{
		SoundSys.Release(sndEngine);
		sndEngine = NULL;
	}
	if (SoundSys.IsHandleValid(sndTraction))
	{
		SoundSys.Release(sndTraction);
		sndTraction = NULL;
	}
	if (SoundSys.IsHandleValid(sndExplosion))
	{
		SoundSys.Release(sndExplosion);
		sndExplosion = NULL;
	}
	if (SoundSys.IsHandleValid(sndFire))
	{
		SoundSys.Release(sndFire);
		sndFire = NULL;
	}
	if (SoundSys.IsHandleValid(sndTractionLoop))
	{
		SoundSys.Stop(sndTractionLoop);
		SoundSys.Release(sndTractionLoop);
		sndTractionLoop = NULL;
	}

	if (m_FireTank)
		m_FireTank= NULL;

	if( !g_bEditMode )
	{
		if (FakeGlass!=NULL)
		{
			FakeGlass->m_FakeGlassID = 0;
			FakeGlass = NULL;
		}
	}
	
#ifndef FINAL_BUILD
	if (!g_bEditMode)
	{
#endif
		if(particleExplosion && isExplosionLoaded)
			particleExplosion->bKillDelayed = 1;
		if(particleDamage && isDamageParticlesLoaded)
			particleDamage->bKillDelayed = 1;
#ifndef FINAL_BUILD	
	}
#endif
	return parent::OnDestroy();
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

BOOL obj_Vehicle::Update()
{
#ifndef FINAL_BUILD
	if ( g_bEditMode )
	{
		if( CurHUDID == 0 && d_drive_vehicles->GetBool() == false ) {
			if ( spawner != NULL && g_Manipulator3d.IsSelected( this ) )
			{
				spawner->SetPosition( GetPosition() );
				spawner->SetRotationVector( GetRotationVector() );
			}
		}

		if (PhysicsObject)
		{
			r3dVector angles = GetRotationVector();
			angles.x += 90.0f;
			PhysicsObject->SetRotation(angles);
		}

		if (wasNetworkLocal)
		{
			if (g_pPhysicsWorld->m_VehicleManager->RemoveActiveVehicle(vd))
				isActivePhysXVehicle = false;

			SwitchToDrivable(false);
			PhysicsEnable(true);

			wasNetworkLocal = false;
			NetworkLocal = false;
		}
		CheckEditorInput();
		UpdateSounds();
		UpdateRotor();
		HelicopterPhysx();
		if (CreateParams.vehicleType == VEHICLETYPE_POLICE)
		UpdatePoliceSirens();

		return TRUE;
	}
#endif 

	if (HasDriver())
		CheckFuel();

	// keep physics on this vehicle enabled while flying through the air.
	if (isActivePhysXVehicle)
	{
		CheckDistanceFromGround();
	}

	// disable active physics on this vehicle if possible
	if (isActivePhysXVehicle && wasNetworkLocal && vd->GetSpeed() < 0.1f && distanceFromGround < 0.35f)
		ReleaseDrivable();

	if (isActivePhysXVehicle)
		SendUpdate();
	else if (!NetworkLocal) // if ((HasPlayers())
		UpdatePositionFromRemote();

	if (isDamageParticlesLoaded && particleDamage && isActive())
	{
		r3dVector pos = GetPosition();
		pos.y += 0.65f;

		particleDamage->SetPosition(pos + (-GetvForw() * 1.25f));

		//if (isOnFire)
		//	SoundSys.SetSoundPos(sndFire, GetPosition());
	}

	// this is temporary code until the dynamic physics object is possible.
	// this code keeps the box obstacle rotated
	D3DXQUATERNION rotd3d;
	D3DXQuaternionRotationYawPitchRoll(&rotd3d, R3D_DEG2RAD(GetRotationVector().x), R3D_DEG2RAD(GetRotationVector().y), R3D_DEG2RAD(GetRotationVector().z));
	UpdatePlayerObstacle(physXObstacleIndex, GetPosition(), PxQuat(rotd3d.x, rotd3d.y, rotd3d.z, rotd3d.w));

	// keep the physics object in the same rotation orientation as the vehicle actor.
	if (PhysicsObject)
	{
		r3dVector angles = GetRotationVector();
		angles.x += 90.0f;
		PhysicsObject->SetRotation(angles);
	}

	UpdateSounds();
	UpdateUI();
	UpdateHeadlights();
	UpdateRotor();
	CheckInput();
	HelicopterPhysx();

	if (CreateParams.vehicleType == VEHICLETYPE_POLICE)
		UpdatePoliceSirens();

	if (FakeGlass)
		FakeGlass->SetFakeGlassObject(this);

	// when hitting a barricade, the vehicle is forced to apply the brakes to simulate a slow down
	if (isSlowedDown && r3dGetTime() > slowedDownTime + slowedDownWait)
		isSlowedDown = false;

	if (NetworkLocal)
		CheckIsStuck();
	
	return TRUE;
}

void obj_Vehicle::ReleaseDrivable()
{
	if (g_pPhysicsWorld->m_VehicleManager->RemoveActiveVehicle(vd))
		isActivePhysXVehicle = false;

	SwitchToDrivable(false);
	PhysicsEnable(true);

	wasNetworkLocal = false;
	NetworkLocal = false;
}

void obj_Vehicle::CheckIsStuck()
{
	if (isMoving && !g_pPhysicsWorld->m_VehicleManager->IsInReverse())
	{
		if (r3dGetTime() > lastDriveTime + stuckWait && GetMovedDistance() < MIN_MOVE_FOR_STUCK_MSG)
			isStuckInFront = true;
	}
	else if (isMoving && g_pPhysicsWorld->m_VehicleManager->IsInReverse())
	{
		if (r3dGetTime() > lastDriveTime + stuckWait && GetMovedDistance() < MIN_MOVE_FOR_STUCK_MSG)
			isStuckInBack = true;
	}
	
	if (!isStuckInFront && !isStuckInBack)
		isCurrentlyStuck = false;

	if (!isCurrentlyStuck && isStuckInFront && isStuckInBack)
	{
		isCurrentlyStuck = true;

		if (r3dGetTime() > lastStuckMessageTime + stuckMessageWait)
		{
			lastStuckMessageTime = r3dGetTime();
			hudMain->addChatMessage(0, "<SYSTEM>", "If you are stuck, exit your vehicle, look at it and press the UNSTUCK key.", 2);

			isStuckInFront = false;
			isStuckInBack = false;
		}
	}
}

void obj_Vehicle::UpdateHeadlights()
{
	if (isHeadlightsEnabled)
	{
		// temporary until i get light bones.
		r3dVector frontLeft = GetPosition() + GetvForw() * 2.5f + -GetvRight() * 1.2f;
		r3dVector frontRight = GetPosition() + GetvForw() * 2.5f + GetvRight() * 1.2f;

		headLight1.Assign(frontLeft+r3dPoint3D(0,2,0));
		headLight1.Direction = GetvForw();

		headLight2.Assign(frontRight+r3dPoint3D(0,2,0));
		headLight2.Direction = GetvForw();
	}
}

void obj_Vehicle::SendTurretData()
{
	PKT_C2S_TurrerAngles_s n;
	n.camangle		= camangleBackup;
	n.cameraUpDown  = camangle2Backup;
	p2pSendToHost(this, &n, sizeof(n));
}

void obj_Vehicle::CheckInput()
{
	if(NetworkLocal)
	{
		if( hudPause && hudPause->isActive() ) return;
		if( hudMain && ( hudMain->isChatInputActive() || hudMain->isPlayersListVisible() ) ) return;

		if (InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_AIM))
		{
			g_pPhysicsWorld->m_VehicleManager->cameraContoller.SetRearView(true);		
		}
		else if (InputMappingMngr->wasReleased(r3dInputMappingMngr::KS_AIM))
		{
			g_pPhysicsWorld->m_VehicleManager->cameraContoller.SetRearView(false);
		}

		if (gClientLogic().localPlayer_->seatPosition == 0 && InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_TOGGLE_FLASHLIGHT))
		{
			ToggleHeadlights();
		}

		/*if (gClientLogic().localPlayer_->seatPosition == 0 && InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_TOGGLE_NIGHTVISION))
		{
			TogglePoliceSiren();
		}*/
		if (isTank == true)
		{
			if (gClientLogic().localPlayer_ && gClientLogic().localPlayer_->seatPosition == 0)
			{
				if (Mouse->WasPressed(r3dMouse::mRightButton))
				{
					RightMouseClick=!RightMouseClick;
				}
				if (Mouse->IsPressed(r3dMouse::mLeftButton))
				{
					bool HaveTankShells = false;
					int  Slot=0;
					for(int i=0; i<gClientLogic().localPlayer_->CurLoadout.BackpackSize; ++i)
					{
						if(gClientLogic().localPlayer_->CurLoadout.Items[i].itemID == WeaponConfig::ITEMID_TankShell)
						{
							HaveTankShells = true;
							Slot=i;
							break;
						}
					}
					if ((r3dGetTime() - FireTank) > 2)
					{
						FireTank=r3dGetTime();
						if (HaveTankShells == false)
						{
							SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Misc/emptyclip"),GetPosition());
							return;
						}
						else {
							SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Vehicles/TurretSound/Gunfiretank"),GetPosition());
							//SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/NewWeapons/Special/Explosion_antipersonnel_mine_single"),GetPosition());
							r3dVector FirePosition;

							bool returnValue = getFireSpace( FirePosition, 0 );
							r3dPoint3D PositionFire=GetPosition()+FirePosition;
							PositionFire+=r3dPoint3D(0,-camangle2Backup/6.5f,0);

							if (m_FireTank != NULL)
							{
								m_FireTank=NULL;
							}
							m_FireTank = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "tanksmokefire", PositionFire );
							m_FireTank->RenderScale=1.5f;

							wiInventoryItem& wi = gClientLogic().localPlayer_->CurLoadout.Items[Slot];
							gClientLogic().localPlayer_->localPlayer_UseItem(Slot, wi.itemID, GetPosition(), 0.0);
							if(--wi.quantity <= 0) {
								wi.Reset();
								gClientLogic().localPlayer_->OnBackpackChanged(Slot);
							}
						}
	
					}
				}
				if (Mouse->IsPressed(r3dMouse::mRightButton))
				{
					if (camangle2Backup<5.0f && RightMouseClick)
					{
						camangle2Backup+=0.1f;
						cameraUpDown=camangle2Backup + GetRotationVector().y;
						if (!SoundSys.isPlaying(GunSnd))
						{
							GunSnd = SoundSys.Play(sndRotateTurretId,GetPosition());
							SoundSys.Start(GunSnd);
						}

						SendTurretData();
					}
					else if (camangle2Backup>-6.0f && !RightMouseClick)
					{
						camangle2Backup-=0.1f;
						cameraUpDown=camangle2Backup + GetRotationVector().y;
						if (!SoundSys.isPlaying(GunSnd))
						{
							GunSnd = SoundSys.Play(sndRotateTurretId,GetPosition());
							SoundSys.Start(GunSnd);
						}

						SendTurretData();
					}
					else
					{
						if (SoundSys.isPlaying(GunSnd))
						{
							SoundSys.Stop(GunSnd);
							SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Vehicles/TurretSound/TurretStop"),GetPosition());
						}
					}
				}
				else {
					if (SoundSys.isPlaying(GunSnd))
					{
						SoundSys.Stop(GunSnd);
						SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Vehicles/TurretSound/TurretStop"),GetPosition());
					}
				}

				if (Keyboard->IsPressed(kbsLeftShift))
				{
					if ((int)camangle>(int)camangle2)// Move Left
					{
						camangleBackup+=1.0f;
						if (!SoundSys.isPlaying(TurretSnd))
						{
							TurretSnd = SoundSys.Play(sndRotateTurretId,GetPosition());
							SoundSys.Start(TurretSnd);
						}
					}
					else if ((int)camangle<(int)camangle2)// MoveRight
					{
						camangleBackup-=1.0f;
						if (!SoundSys.isPlaying(TurretSnd))
						{
							TurretSnd = SoundSys.Play(sndRotateTurretId,GetPosition());
							SoundSys.Start(TurretSnd);
						}
					}
					else {
						if (SoundSys.isPlaying(TurretSnd))
						{
							SoundSys.Stop(TurretSnd);
							SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Vehicles/TurretSound/TurretStop"),GetPosition());
						}
					}

					SendTurretData();
				}
				else {
					if (SoundSys.isPlaying(TurretSnd))
					{
						SoundSys.Stop(TurretSnd);
						SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Vehicles/TurretSound/TurretStop"),GetPosition());
					}
				}
				camangle2=camangleBackup + GetRotationVector().x;
				cameraUpDown=camangle2Backup + GetRotationVector().y;
			}
		}

	}
}

#ifndef FINAL_BUILD
void obj_Vehicle::CheckEditorInput()
{
		if (isTank == true)
		{
			if (gClientLogic().localPlayer_ && HasDriver())
			{
				if (Mouse->WasPressed(r3dMouse::mRightButton))
				{
					RightMouseClick=!RightMouseClick;
				}
				if (Mouse->IsPressed(r3dMouse::mLeftButton))
				{
					if ((r3dGetTime() - FireTank) > 2)
					{
						FireTank=r3dGetTime();
						SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Vehicles/TurretSound/Gunfiretank"),GetPosition());
						//SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Vehicles/TurretSound/Treadtank"),GetPosition());
						r3dVector FirePosition;

						bool returnValue = getFireSpace( FirePosition, 0 );
						r3dPoint3D PositionFire=GetPosition()+FirePosition;
						PositionFire+=r3dPoint3D(0,-camangle2Backup/6.5f,0);

						if (m_FireTank != NULL)
						{
							m_FireTank=NULL;
						}
						m_FireTank = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", "tanksmokefire", PositionFire );
						m_FireTank->RenderScale=1.5f;
					}
				}
				if (Mouse->IsPressed(r3dMouse::mRightButton))
				{
					if (camangle2Backup<5.0f && RightMouseClick)
					{
						camangle2Backup+=0.1f;
						cameraUpDown=camangle2Backup + GetRotationVector().y;
						if (!SoundSys.isPlaying(GunSnd))
						{
							GunSnd = SoundSys.Play(sndRotateTurretId,GetPosition());
							SoundSys.Start(GunSnd);
						}
					}
					else if (camangle2Backup>-6.0f && !RightMouseClick)
					{
						camangle2Backup-=0.1f;
						cameraUpDown=camangle2Backup + GetRotationVector().y;
						if (!SoundSys.isPlaying(GunSnd))
						{
							GunSnd = SoundSys.Play(sndRotateTurretId,GetPosition());
							SoundSys.Start(GunSnd);
						}
					}
					else
					{
						if (SoundSys.isPlaying(GunSnd))
						{
							SoundSys.Stop(GunSnd);
							SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Vehicles/TurretSound/TurretStop"),GetPosition());
						}
					}
				}
				else {
					if (SoundSys.isPlaying(GunSnd))
					{
						SoundSys.Stop(GunSnd);
						SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Vehicles/TurretSound/TurretStop"),GetPosition());
					}
				}

				if (Keyboard->IsPressed(kbsLeftShift))
				{
					if ((int)camangle>(int)camangle2)// Move Left
					{
						camangleBackup+=1.0f;
						if (!SoundSys.isPlaying(TurretSnd))
						{
							TurretSnd = SoundSys.Play(sndRotateTurretId,GetPosition());
							SoundSys.Start(TurretSnd);
						}
					}
					else if ((int)camangle<(int)camangle2)// MoveRight
					{
						camangleBackup-=1.0f;
						if (!SoundSys.isPlaying(TurretSnd))
						{
							TurretSnd = SoundSys.Play(sndRotateTurretId,GetPosition());
							SoundSys.Start(TurretSnd);
						}
					}
					else {
						if (SoundSys.isPlaying(TurretSnd))
						{
							SoundSys.Stop(TurretSnd);
							SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Vehicles/TurretSound/TurretStop"),GetPosition());
						}
					}
				}
				else {
					if (SoundSys.isPlaying(TurretSnd))
					{
						SoundSys.Stop(TurretSnd);
						SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Vehicles/TurretSound/TurretStop"),GetPosition());
					}
				}
				camangle2=camangleBackup + GetRotationVector().x;
				cameraUpDown=camangle2Backup + GetRotationVector().y;
			}
		}
}
#endif

void obj_Vehicle::SendUpdate()
{
	PKT_C2C_VehicleMoveSetCell_s setCell;
	PKT_C2C_VehicleMoveRel_s moveRel;

	// setup move data
	CVehicleNetCellMover::moveData_s md;
	md.pos       = GetPosition();
	md.rot		 = GetRotationVector();
	md.state     = 0;

	PxVehicleWheels* wheels;
	
	if (isTank == false)
		wheels = vd->vehicle;
	else
		wheels = vd->tank;

	// get wheel turn angle and speed
	for (uint32_t i = 0; i < vd->numWheels; ++i)
	{
		// get suspension travel
		moveRel.wheelSuspTravel[ i ] = (short)floor(wheels->mWheelsDynData.getSuspJounce(i) * 65535 / 1000);

		// get rotations
		moveRel.wheelRotation[i] = (short)floor(wheels->mWheelsDynData.getWheelRotationAngle(i) * 65535 / 1000);
		moveRel.wheelTurnAngle[i] = (short)floor(wheels->mWheelsDynData.getSteer(i) * 65535 / 1000);
	}

	moveRel.engineRpm = (int)GetEngineRpm();
	moveRel.sideSpeed = sideSpeedVolume > 0 ? (BYTE)sideSpeedVolume * 10 : (BYTE)0;

	DWORD pktFlags = netMover.SendPosUpdate(md, &setCell, &moveRel);

	if(pktFlags & 0x1)
		p2pSendToHost(this, &setCell, sizeof(setCell), true);
	if(pktFlags & 0x2)
		p2pSendToHost(this, &moveRel, sizeof(moveRel), false);
}

void obj_Vehicle::CheckDistanceFromGround()
{
	PxTransform pose;
	if (isTank == false)
		pose = vd->vehicle->getRigidDynamicActor()->getGlobalPose();
	else
		pose = vd->tank->getRigidDynamicActor()->getGlobalPose();
	PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC);

	float MAX_CASTING_DISTANCE = 2000.0f;

	PxRaycastHit hit;
	if (g_pPhysicsWorld->PhysXScene->raycastSingle(pose.p, PxVec3(0, -1, 0), MAX_CASTING_DISTANCE, PxSceneQueryFlag::eIMPACT|PxSceneQueryFlag::eNORMAL, hit, filter))
	{
		distanceFromGround = hit.distance;
	}
}

bool obj_Vehicle::CheckFuel()
{
	if (r3dGetTime() > lastFuelCheckTime + fuelCheckWaitTime)
	{
		lastFuelCheckTime = r3dGetTime();

		if (vd->GetSpeed() > 0.2f)
			curFuel -= 5;
		else
			curFuel -= 1;

		if (curFuel <= 0)
		{
			curFuel = 0;
			OnRunOutOfGas();

			fuelChanged = true;
		}
	}

	return curFuel > 0;
}

void obj_Vehicle::UpdatePositionFromPhysx()
{
	if (!wasNetworkLocal)
	{
		if (!vd || !NetworkLocal)
			return;
	}

	PxRigidDynamicFlags f;
	
	if (isTank == false)
		vd->vehicle->getRigidDynamicActor()->getRigidDynamicFlags();
	else
		vd->tank->getRigidDynamicActor()->getRigidDynamicFlags();

	if ( !(f & PxRigidDynamicFlag::eKINEMATIC) )
	{
		PxTransform t;
		
		if (isTank == false)
			t = vd->vehicle->getRigidDynamicActor()->getGlobalPose();
		else
			t = vd->tank->getRigidDynamicActor()->getGlobalPose();

		SetPosition(r3dVector(t.p.x, t.p.y, t.p.z));

		r3dVector angles;
		QuaternionToEulerAngles(t.q, angles.x, angles.y, angles.z);
		angles = D3DXToDegree(angles);

		SetRotationVector(angles);

		if (!g_bEditMode)
			UpdatePlayers();
	}

#ifndef FINAL_BUILD
	if (!g_bEditMode)
#endif
		if (GetSpeed() > 7.0f)
			IsGoingToCollide(GetvForw());
}

void obj_Vehicle::UpdatePositionFromRemote()
{
	r3d_assert(!NetworkLocal && !wasNetworkLocal);

	const float fTimePassed = r3dGetFrameTime();
	
	r3dVector r;
	extern float getMinimumAngleDistance(float, float);

	float xDistance = getMinimumAngleDistance(GetRotationVector().x, lastRotationVector.x);
	if (xDistance < 0.0f) 
		xDistance = -xDistance;

	if (xDistance > 25)
		r.x = lastRotationVector.x;
	else
		r.x = R3D_LERP(GetRotationVector().x, lastRotationVector.x, 8.0f * fTimePassed);		

	float yDistance = getMinimumAngleDistance(GetRotationVector().y, lastRotationVector.y);
	if (yDistance < 0.0f) 
		yDistance = -yDistance;

	if (yDistance > 25)
		r.y = lastRotationVector.y;
	else
		r.y = R3D_LERP(GetRotationVector().y, lastRotationVector.y, 8.0f * fTimePassed);		

	float zDistance = getMinimumAngleDistance(GetRotationVector().y, lastRotationVector.y);
	if (zDistance < 0.0f) 
		zDistance = -zDistance;

	if (zDistance > 25)
		r.z = lastRotationVector.z;
	else
		r.z = R3D_LERP(GetRotationVector().z, lastRotationVector.z, 8.0f * fTimePassed);

	if(netVelocity.LengthSq() > 0.0001f)
	{	
		SetRotationVector(r);

		r3dPoint3D p;
		p.x = R3D_LERP(GetPosition().x, netMover.GetVehicleNetPos().x, 3.0f * fTimePassed);
		p.y = R3D_LERP(GetPosition().y, netMover.GetVehicleNetPos().y, 3.0f * fTimePassed);
		p.z = R3D_LERP(GetPosition().z, netMover.GetVehicleNetPos().z, 3.0f * fTimePassed);

		SetPosition(p);
	
		r3dPoint3D v = netMover.GetVehicleNetPos() - GetPosition();
		float d = netVelocity.Dot(v);
		if(d < 0) 
		{
			SetPosition(netMover.GetVehicleNetPos());
			netVelocity = r3dPoint3D(0, 0, 0);
		}
	}
	
	UpdatePlayers();

#ifndef FINAL_BUILD
	if (!g_bEditMode)
#endif
		if (GetSpeed() > 7.0f)
			IsGoingToCollide(GetvForw());
}

void obj_Vehicle::UpdatePlayers()
{
	for (uint32_t i = 0; i < vd->numSeats; ++i)
	{
		if (!players[i])
			continue;

		r3dVector SeatPosNewVehicles = r3dVector(0,0,0);
		if (CreateParams.vehicleType == VEHICLETYPE_ABANDONEDSUV)
			SeatPosNewVehicles = r3dVector(0,0.2,0);
		else if (CreateParams.vehicleType == VEHICLETYPE_DUNEBUGGY)
			SeatPosNewVehicles = r3dVector(0,0.1,0);

		r3dVector pos;
		if (vd->GetSeatIndex(pos, players[i]->seatPosition))
		{
			players[i]->UpdatePosition(GetPosition() + pos - SeatPosNewVehicles);
			players[i]->m_fPlayerRotationTarget = GetRotationVector().x;
		}
	}
}

void obj_Vehicle::HelicopterPhysx() // controls of helicopter
{
	if (isHelicopter == true)
	{
		const float fTimePassed = r3dGetFrameTime();
		PxTransform pose = vd->vehicle->getRigidDynamicActor()->getGlobalPose();
		r3dVector angles;
		r3dPoint3D position(0,0,0);
		//PxVec3 Gravity(0,0,0);

		if (HasDriver() && HSpeed > 70)
		{
			if (Keyboard->IsPressed(kbsD)) // Rotate Left
			{
				if (!(Keyboard->IsPressed(kbsUp) || Keyboard->IsPressed(kbsDown)))
					HelyX+=0.5f;
			}
			else if (Keyboard->IsPressed(kbsS)) // rotate Right
			{
				if (!(Keyboard->IsPressed(kbsUp) || Keyboard->IsPressed(kbsDown)))
					HelyX-=0.5f;
			}
			if (Keyboard->IsPressed(kbsUp)) // move front
			{
				if (HelyY<15.0f)
					HelyY+=0.5f;

				if (HelyFB>1 && FB>=0)
					FB=HelyFB-=0.25f;
			}
			else {
				if (FB >0 && HelyFB<20)
					FB=HelyFB+=0.05f;

				if (HelyY>0)
				{
					HelyY-=0.5f;
					if (HelyY<0) HelyY = 0;
				}
			}
			if (Keyboard->IsPressed(kbsDown)) // Move back
			{	
				if (HelyY>-15.0f)
					HelyY-=0.5f;
				
				if (HelyFB>5 && FB<=0)
					FB=abs(HelyFB-=0.5f)*-1;
			}
			else {
				if (FB <0 && HelyFB<20)
					FB=abs(HelyFB+=0.05f)*-1;
				if (HelyY<0)
				{
					HelyY+=0.5f;
				}
			}
			if (Keyboard->IsPressed(kbsRight)) // move Right
			{
				if (HelyZ>-30.0)
					HelyZ-=0.5f;

				if ((Keyboard->IsPressed(kbsUp) || Keyboard->IsPressed(kbsDown)))
				{
					HelyX+=0.5f;
				}
				else {
					if (HelyLR>5 && LR>=0)
						LR=HelyLR-=0.5f;
				}
			}
			else {
				if (LR >0 && HelyLR<20)
					LR=HelyLR+=0.1f;
				if (HelyZ<0)
				{
					HelyZ+=0.5f;
				}
			}

			if (Keyboard->IsPressed(kbsLeft)) // move Left
			{
				
				if (HelyZ<30.0)
					HelyZ+=0.5f;

				if ((Keyboard->IsPressed(kbsUp) || Keyboard->IsPressed(kbsDown)))
				{
					HelyX-=0.5f;
				}
				else {
					if (HelyLR>5 && LR<=0)
						LR=abs(HelyLR-=0.5f)*-1;
				}

			}
			else {
				if (LR <0 && HelyLR<20)
					LR=abs(HelyLR+=0.1f)*-1;

				if (HelyZ>0)
				{
					HelyZ-=0.5f;
					if (HelyZ<0) HelyZ = 0;
				}
			}
			if (Keyboard->IsPressed(kbsA)) // move up
			{
				//r3dOutToLog("###### distanceFromGround %f\n",distanceFromGround);
				if (distanceFromGround<100)
					HelyUp=1.0f;
				//Gravity = PxVec3(0,1,0);
			}
			else if (Keyboard->IsPressed(kbsZ)) // Move down
			{
				HelyUp=-1.0f;
				//Gravity = PxVec3(0,-2,0);
			}
		}
		else {
			
			if (distanceFromGround>0)
			{
				if (HSpeed < 50 || !CheckFuel())
				{
					//r3dOutToLog("######## HSpeed: %f\n",HSpeed);
					//Gravity = PxVec3(0,-9.81f,0);
					if (HelyUp>0)
						HelyUp=-0.1f;
				}
			}
		}
		pose.p.y+=HelyUp;
		if (HelyFB>=20)
			FB=0;
		if (HelyLR>=20)
			LR=0;
		//r3dOutToLog("######## HelyFB: %f FB: %f\n",HelyFB,FB);
		position = r3dPoint3D(pose.p.x,pose.p.y,pose.p.z) + (GetvForw() / FB) + (GetvRight() / LR);
				if (HelyUp<0)
				{
					HelyUp+=0.001f;
				}
				else if (HelyUp>0)
				{
					HelyUp-=0.001f;
					if (HelyUp<0) HelyUp = 0;
				}

		//g_pPhysicsWorld->PhysXScene->setGravity(Gravity);
		
		D3DXMATRIX RotateMatrix,RotateMatrix2,toOrigin,fromOrigin,boneTransform;
		D3DXMatrixTranslation(&toOrigin, -GetPosition().x, -GetPosition().y, -GetPosition().z);
		D3DXMatrixTranslation(&fromOrigin, pose.p.x, pose.p.y, pose.p.z);
		
		D3DXMatrixRotationYawPitchRoll(&RotateMatrix,R3D_DEG2RAD(HelyX),R3D_DEG2RAD(HelyY),R3D_DEG2RAD(HelyZ));
		D3DXMatrixMultiply(&boneTransform, &fromOrigin, &RotateMatrix);

		D3DXMatrixMultiply(&boneTransform, &boneTransform, &toOrigin);

		PxMat33 orientation(PxVec3(boneTransform._11, boneTransform._12, boneTransform._13),
						PxVec3(boneTransform._21, boneTransform._22, boneTransform._23),
						PxVec3(boneTransform._31, boneTransform._32, boneTransform._33));
		pose.q = PxQuat(orientation);

		/*D3DMATRIX m = GetRotationMatrix();*/
		pose.p.x=position.x;//HelyFB;
		pose.p.z=position.z;//HelyLR;
		
		/*HelyLR=position.z;
		HelyFB=position.x;*/
		vd->vehicle->getRigidDynamicActor()->setGlobalPose(pose);
		//SetPosition(r3dPoint3D(position.x,pose.p.y,position.z));
		r3dPoint3D vel = GetVelocity();
		vel.x *= pose.p.x;
		vel.y *= pose.p.y;
		vel.z *= pose.p.y;
		if(vel.Length() < 0.001f) vel = r3dPoint3D(0, 0, 0);
		SetVelocity(vel);
	}
}

int obj_Vehicle::GetDisplayDurability()
{
	float percent = ((float)curDurability / maxDurability) * 100;				
	int displayDura = 0;

	if (percent > 0 && percent <= 20)
		displayDura = 1;
	else if (percent > 20 && percent <= 40)
		displayDura = 2;
	else if (percent > 40 && percent <= 60)
		displayDura = 3;
	else if (percent > 60 && percent <= 80)
		displayDura = 4;
	else if (percent > 80 && percent <= 100)
		displayDura = 5;

	return displayDura;
}

//////////////////////////////////////////////////////////////////////////

bool obj_Vehicle::HasDriver()
{
	return players[0] != NULL;
}


bool obj_Vehicle::HasPlayers() const
{
	uint32_t playerCount = 0;
	for(uint32_t i = 0; i < vd->numSeats; ++i)
	{
		if (players[i])
			playerCount++;
	}
	return playerCount > 0;
}

//////////////////////////////////////////////////////////////////////////

void obj_Vehicle::SetPosition(const r3dPoint3D& pos)
{
	parent::SetPosition(pos);
	SyncPhysicsPoseWithObjectPose();
}

void obj_Vehicle::SetPositionNoPose(const r3dPoint3D& pos)
{
	parent::SetPosition(pos);
}
//////////////////////////////////////////////////////////////////////////

void obj_Vehicle::SetRotationVector(const r3dVector& Angles)
{
	parent::SetRotationVector(Angles);
	SyncPhysicsPoseWithObjectPose();
}

//////////////////////////////////////////////////////////////////////////

void obj_Vehicle::SwitchToDrivable(bool doDrive)
{
	if (vd)
	{
		if (vd->hasTracks == 0)
			vd->vehicle->getRigidDynamicActor()->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, !doDrive);
		else
			vd->tank->getRigidDynamicActor()->setRigidDynamicFlag(PxRigidDynamicFlag::eKINEMATIC, !doDrive);
	}

	if (doDrive)
		g_pPhysicsWorld->m_VehicleManager->DriveCar(vd);
}

//////////////////////////////////////////////////////////////////////////

void obj_Vehicle::SyncPhysicsPoseWithObjectPose()
{
	if (!vd)
		return;


	PxRigidDynamicFlags f;
	
	if (isTank == false)
		f = vd->vehicle->getRigidDynamicActor()->getRigidDynamicFlags();
	else
		f = vd->tank->getRigidDynamicActor()->getRigidDynamicFlags();

	if (!(f & PxRigidDynamicFlag::eKINEMATIC))
		return;

	r3dPoint3D pos(GetPosition());
	D3DXMATRIX rotM(GetRotationMatrix());
	D3DXQUATERNION q;
	D3DXQuaternionRotationMatrix(&q, &rotM);
	PxQuat quat(q.x, q.y, q.z, q.w);

	PxTransform carPose(PxVec3(pos.x, pos.y, pos.z), quat);

	if (isTank == false)
		vd->vehicle->getRigidDynamicActor()->setGlobalPose(carPose);
	else
		vd->tank->getRigidDynamicActor()->setGlobalPose(carPose);
}

void obj_Vehicle::SyncPhysicsObjectPose()
{
	if (!m_bEnablePhysics)
		return;

	PxActor* actor = PhysicsObject->getPhysicsActor();

	if(actor->isRigidActor())
	{
		PxRigidActor* dyn = actor->isRigidActor();

		r3dPoint3D pos(GetPosition());
		D3DXMATRIX rotM(GetRotationMatrix());
		D3DXQUATERNION q;
		D3DXQuaternionRotationMatrix(&q, &rotM);
		PxQuat quat(q.x, q.y, q.z, q.w);

		PxTransform carPose(PxVec3(pos.x, pos.y, pos.z), quat);

		dyn->setGlobalPose(carPose);

		UpdateTransform();
	}
}

//////////////////////////////////////////////////////////////////////////

void obj_Vehicle::SetBoneMatrices()
{
	if (!vd)
		return;

	PxRigidDynamic *a = NULL;
	if (isTank == false)
		a = vd->vehicle->getRigidDynamicActor();
	else
		a = vd->tank->getRigidDynamicActor();

	static const UsefulTransforms T;

	//	Init with identities
	for (int i = 0; i < vd->skl->NumBones; ++i)
	{
		r3dBone &b = vd->skl->Bones[i];
		b.CurrentTM = T.rotY_mat;
	}

	//	Retrieve vehicle wheels positions
	PxShape *shapes[VEHICLE_PARTS_COUNT+VEHICLE_DAMPER_COUNT] = {0};
	PxU32 sn = a->getShapes(shapes, VEHICLE_PARTS_COUNT+VEHICLE_DAMPER_COUNT);
	int CountDamper = 1;
	for (PxU32 i = 0; i < sn; ++i)
	{
		PxShape *s = shapes[i];
		PxU32 boneIndex = reinterpret_cast<PxU32>(s->userData);
		r3dBone &b = vd->skl->Bones[boneIndex];

		bool bIsWheel = false;
		bool bIsTurret = false;
		bool bIsCannon = false;
		bool bIsDamper = false;

		int FoundWheel = 1;

		for( uint32_t j = 0; j < vd->numWheels; ++j )
		{
			if( &b == vd->wheelsData[ j ].wheelBone )
			{
				bIsWheel = true;
					FoundWheel+= j;
				break;
			}
		}

		if (strcmp(b.Name,"Bone_Turrent_Base") == 0 && isTank == true)
			bIsTurret = true;
		if (strcmp(b.Name,"Bone_Turrent_Gun") == 0 && isTank == true)
			bIsCannon = true;

		if (strncmp(b.Name,"Bone_Damper_",12) == 0 && isTank == true)
			bIsDamper = true;

		D3DXMATRIX boneTransform,boneTransform_Damper;
		PxTransform pose = s->getLocalPose();
		if( bIsWheel )
		{
			PxVec3 bonePos = PxVec3(b.vRelPlacement.x, b.vRelPlacement.y, b.vRelPlacement.z);
			D3DXMATRIX toOrigin, fromOrigin;
			D3DXMatrixTranslation(&toOrigin, -bonePos.x, -bonePos.y, -bonePos.z);
			D3DXMatrixTranslation(&fromOrigin, pose.p.x, pose.p.y, pose.p.z);

			pose.q = pose.q * T.rotY_quat;

			wheelBones[FoundWheel] = pose.p.y;

			D3DXQUATERNION q(pose.q.x, pose.q.y, pose.q.z, pose.q.w);
			D3DXMatrixRotationQuaternion(&boneTransform, &q);

			D3DXMatrixMultiply(&boneTransform, &toOrigin, &boneTransform);
			D3DXMatrixMultiply(&boneTransform, &boneTransform, &fromOrigin);

		}
		else if (bIsDamper)
		{
			if (isTank == true)
			{
				float positionwheelD = 0.0f;

				if (CountDamper>0 && CountDamper<5)
				{
					if (!vd->tank->isInAir())
						positionwheelD = wheelBones[CountDamper]+1.05f;
					else
						positionwheelD = -wheelBones[CountDamper]+2.25f;
				}
				else if(CountDamper>4 && CountDamper<13)
				{
					positionwheelD = -wheelBones[CountDamper]+2.2f;
				}
				else if (CountDamper>12 && CountDamper<15)
				{
					positionwheelD = -wheelBones[CountDamper]+2.9f;
				}
				else 
				{
					positionwheelD = -wheelBones[CountDamper]+2.6f;
				}
				b.vRelPlacement.y = positionwheelD;

				r3dMesh* meshTread = MeshLOD[0];
				r3dMaterial* mat;
				for (int a = 0;a<meshTread->GetNumMaterials();a++)
				{
					if (meshTread->MatChunks[a].MatName == "t80_tread_left")
					{
						mat = meshTread->MatChunks[ a ].Mat;

						//r3dOutToLog("######### MATERIAL NAME %s\n",mat->Name);
						//break;
					}
					else if (meshTread->MatChunks[a].MatName == "t80_tread_right")
					{
						mat = meshTread->MatChunks[ a ].Mat;
						//r3dOutToLog("######### MATERIAL NAME %s\n",mat->Name);
						//break;
					}
				}
				CountDamper++;
			}

			PxVec3 bonePos = PxVec3(b.vRelPlacement.x, b.vRelPlacement.y, b.vRelPlacement.z);
			D3DXMATRIX toOrigin, fromOrigin;
			D3DXMatrixTranslation(&toOrigin, -bonePos.x, -bonePos.y, -bonePos.z);
			D3DXMatrixTranslation(&fromOrigin, pose.p.x, pose.p.y, pose.p.z);

			pose.q = pose.q * T.rotY_quat;

			D3DXQUATERNION q(pose.q.x, pose.q.y, pose.q.z, pose.q.w);
			D3DXMatrixRotationQuaternion(&boneTransform, &q);

			D3DXMatrixMultiply(&boneTransform, &toOrigin, &boneTransform);
			D3DXMatrixMultiply(&boneTransform, &boneTransform, &fromOrigin);

		}
		else if (isHelicopter)
		{
			PxVec3 bonePos = PxVec3(b.vRelPlacement.x, b.vRelPlacement.y, b.vRelPlacement.z);
			D3DXMATRIX toOrigin, fromOrigin;
			D3DXMatrixTranslation(&toOrigin, -bonePos.x, -bonePos.y, -bonePos.z);
			D3DXMatrixTranslation(&fromOrigin, pose.p.x, pose.p.y, pose.p.z);

			pose.q = pose.q * T.rotY_quat;

			D3DXQUATERNION q(pose.q.x, pose.q.y, pose.q.z, pose.q.w);
			D3DXMatrixRotationQuaternion(&boneTransform, &q);

			if (strcmp(b.Name,"Rotor_001") == 0)
			{
				D3DXMATRIX RotateMatrix,RotateMatrix2;
				D3DXMatrixRotationYawPitchRoll(&RotateMatrix,R3D_DEG2RAD(RotorX),0,0);
				D3DXMatrixMultiply(&toOrigin, &toOrigin, &RotateMatrix);
			}
			else if (strcmp(b.Name,"Rotor_002") == 0)
			{
				D3DXMATRIX RotateMatrix,RotateMatrix2;
				D3DXMatrixRotationYawPitchRoll(&RotateMatrix,0,0,R3D_DEG2RAD(RotorY));
				D3DXMatrixMultiply(&toOrigin, &toOrigin, &RotateMatrix);
			}

			D3DXMatrixMultiply(&boneTransform, &toOrigin, &boneTransform);
			D3DXMatrixMultiply(&boneTransform, &boneTransform, &fromOrigin);

		}
		else if( bIsTurret && camangle2 !=0 || bIsCannon && cameraUpDown!=0)
		{
			PxVec3 bonePos = PxVec3(b.vRelPlacement.x, b.vRelPlacement.y, b.vRelPlacement.z);
			D3DXMATRIX toOrigin, fromOrigin, test;
			D3DXMatrixTranslation(&toOrigin, -bonePos.x, -bonePos.y, -bonePos.z);
			D3DXMatrixTranslation(&fromOrigin, pose.p.x, pose.p.y, pose.p.z);

			pose.q = pose.q * T.rotY_quat;

			D3DXQUATERNION q(pose.q.x, pose.q.y, pose.q.z, pose.q.w);
			D3DXMatrixRotationQuaternion(&boneTransform, &q);

			if( bIsTurret && camangle2 !=0)
			{
				D3DXMATRIX RotateMatrix;
				D3DXMatrixRotationYawPitchRoll(&RotateMatrix,R3D_DEG2RAD(camangle2-GetRotationVector().x),0,0);
				D3DXMatrixMultiply(&fromOrigin, &fromOrigin, &RotateMatrix);
				vd->PositionFire=camangle2-GetRotationVector().x;
			}
			if ( bIsCannon && cameraUpDown!=0)
			{
				D3DXMATRIX RotateMatrix;
				D3DXMatrixRotationYawPitchRoll(&RotateMatrix,R3D_DEG2RAD(camangle2-GetRotationVector().x),R3D_DEG2RAD(cameraUpDown-GetRotationVector().y),0);
				D3DXMatrixMultiply(&fromOrigin, &fromOrigin, &RotateMatrix);
			}

			D3DXMatrixMultiply(&boneTransform, &toOrigin, &boneTransform);
			D3DXMatrixMultiply(&boneTransform, &boneTransform, &fromOrigin);
		}
		else
		{
			pose.q = pose.q * T.rotY_quat;

			D3DXQUATERNION q(pose.q.x, pose.q.y, pose.q.z, pose.q.w);
			D3DXMatrixRotationQuaternion(&boneTransform, &q);
		}

		b.CurrentTM = boneTransform;
	}

	vd->skl->SetShaderConstants();
}
//////////////////////////////////////////////////////////////////////////

extern PxF32 &gVehicleTireGroundFriction;

#ifndef FINAL_BUILD
//#define EXTENDED_VEHICLE_CONFIG
float obj_Vehicle::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float y = scry + parent::DrawPropertyEditor(scrx, scry, scrw, scrh, startClass, selected );

	if( !IsParentOrEqual( &ClassData, startClass ) || !vd)
		return y;

	y += 10.0f;
	y += imgui_Static(scrx, y, "Vehicle base configuration");
	y += imgui_Value_SliderI(scrx, y, "Has Tracks", &vd->hasTracks, 0, 1, "%d");
	y += imgui_Value_SliderI(scrx, y, "Max Fuel", &vd->maxFuel, 2000, 10000, "%d");
	y += imgui_Value_SliderI(scrx, y, "Max Health", &vd->durability, 1500, 20000, "%d");
	y += imgui_Value_SliderI(scrx, y, "Armor Ext", &vd->armorExterior, 1, 100, "%d");
	y += imgui_Value_SliderI(scrx, y, "Armor Int", &vd->armorInterior, 1, 100, "%d");
	y += imgui_Value_SliderI(scrx, y, "Plyr Melee Dmg", &vd->thresholdIgnoreMelee, 1, 100, "%d");
	y += imgui_Value_SliderI(scrx, y, "Plyr Range Dmg", &vd->thresholdIgnoreBullets, 1, 100, "%d");

	y += imgui_Static(scrx, y, "Vehicle physics configuration");

	float lastTireFriction = vd->tireFriction;
	y += imgui_Value_Slider(scrx, y, "Friction", &vd->tireFriction, 0.5f, 8.0f, "%-02.2f");

	// update vehicle manager
	if (vd->tireFriction != lastTireFriction)
	{
		g_pPhysicsWorld->m_VehicleManager->UpdateFrictionValues((int)CreateParams.vehicleType, vd->tireFriction);
	}

#ifdef EXTENDED_VEHICLE_CONFIG
	PxVehicleAckermannGeometryData &ad = vd->ackermannData;
	y += 5.0f;
	y += imgui_Static(scrx, y, "Chassis:");
	y += imgui_Value_Slider(scrx, y, "Mass", &vd->chassisData.mMass, 100.0f, 5000.0f, "%-02.2f");
	y += imgui_Value_Slider(scrx, y, "Ackerman accuracy", &ad.mAccuracy, 0.0f, 1.0f, "%-02.2f");
#endif

	y += 5.0f;
	y += imgui_Static(scrx, y, "Chassis:");
	y += imgui_Value_Slider(scrx, y, "Mass", &vd->chassisData.mMass, 100.0f, 5000.0f, "%-02.2f");

	PxVehicleEngineData &ed = vd->engineData;
	y += 5.0f;
	y += imgui_Static(scrx, y, "Engine:");
	y += imgui_Value_Slider(scrx, y, "Peak torque", &ed.mPeakTorque, 100.0f, 5000.0f, "%-02.f");
	float x = ed.mMaxOmega / 0.104719755f;
	y += imgui_Value_Slider(scrx, y, "Max RPM", &x, 0.0f, 15000.0f, "%-02.0f");
	ed.mMaxOmega = x * 0.104719755f;

//#ifdef EXTENDED_VEHICLE_CONFIG
	y += 5.0f;
	y += imgui_Static(scrx, y, "Gears:");
	PxVehicleGearsData &gd = vd->gearsData;
	y += imgui_Value_Slider(scrx, y, "Switch time", &gd.mSwitchTime, 0.0f, 3.0f, "%-02.2f");
	for(uint32_t i = 0; i < gd.mNumRatios; ++i)
	{
		char gearName[10];
		if (i == 0)
			snprintf(gearName, sizeof(gearName), "Reverse");
		else if (i == 1)
			snprintf(gearName, sizeof(gearName), "Neutral");
		else 
			snprintf(gearName, sizeof(gearName), "Gear %d", i-1);

		y += imgui_Value_Slider(scrx, y, gearName, &gd.mRatios[i], -10.0f, 10.0f, "%-02.2f");
	}
	
	y += 10.0f;
	if (imgui_Button(scrx, y, 80.0f, 30.0f, "Add Gear"))
		gd.mNumRatios++;
	if (imgui_Button(scrx + 80, y, 80.0f, 30.0f, "Remove Gear"))
		gd.mNumRatios--;

	PxVehicleDifferential4WData &dd = vd->diffData;
	y += 30.0f;
	y += imgui_Static(scrx, y, "Differential:");
	y += imgui_Value_Slider(scrx, y, "Front-rear split", &dd.mFrontRearSplit, 0.0f, 1.0f, "%-02.3f");
//#endif
	y += 10.0f;
	y += imgui_Static(scrx, y, "Select wheel:");

	static int currentWheel = 0;
	imgui_Value_SliderI(scrx, y, "Selected Wheel:", &currentWheel, 0.0f, (float)(vd->numWheels - 1), "%d");

	y += 30.0f;

	VehicleDescriptor::WheelData &wd = vd->wheelsData[currentWheel];

	y += 5.0f;
//#ifdef EXTENDED_VEHICLE_CONFIG
	y += imgui_Value_Slider(scrx, y, "Mass", &wd.wheelData.mMass, 1.0f, 100.0f, "%-02.3f");
	y += imgui_Value_Slider(scrx, y, "Spring max compression", &wd.suspensionData.mMaxCompression, 0.0f, 2.0f, "%-02.3f");
	y += imgui_Value_Slider(scrx, y, "Spring max droop", &wd.suspensionData.mMaxDroop, 0.0f, 2.0f, "%-02.3f");
//#endif
	y += imgui_Value_Slider(scrx, y, "Break torque", &wd.wheelData.mMaxBrakeTorque, 0.0f, 25000.0f, "%-02.0f");
	float f = R3D_RAD2DEG(wd.wheelData.mMaxSteer);
	y += imgui_Value_Slider(scrx, y, "Steer angle", &f, 0.0f, 90.0f, "%-02.2f");
	wd.wheelData.mMaxSteer = R3D_DEG2RAD(f);
	if (currentWheel >= 2)
	{
		vd->wheelsData[(currentWheel + 1) % 2 + 2].wheelData.mMaxSteer = wd.wheelData.mMaxSteer;
	}
	float z = R3D_RAD2DEG(wd.wheelData.mToeAngle);
	wd.wheelData.mToeAngle = R3D_DEG2RAD(z);
//#ifdef EXTENDED_VEHICLE_CONFIG
	y += imgui_Value_Slider(scrx, y, "Spring strength", &wd.suspensionData.mSpringStrength, 10000.0f, 50000.0f, "%-05.0f");
//#endif
	y += imgui_Value_Slider(scrx, y, "Spring damper", &wd.suspensionData.mSpringDamperRate, 500.0f, 9000.0f, "%-02.0f");

	y += 10.0f;
	if (imgui_Button(scrx, y, 360.0f, 22.0f, "Save Vehicle Data"))
	{
		vd->Save(0);
	}
	y += 22.0f;

	vd->ConfigureVehicleSimulationData();
	return y - scry;
}

void obj_Vehicle::DrawDebugInfo() const
{
	if (g_bEditMode)
		return;

	r3dRenderer->SetMaterial(NULL);

	struct PushRestoreStates
	{
		PushRestoreStates()
		{
			r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );
		}

		~PushRestoreStates()
		{
			r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
		}
	} pushRestoreStates; (void)pushRestoreStates;

	r3dPoint3D position;

	switch( r_show_vehicle_debug->GetInt() )
	{
	case 4:	// Draw Bone Labels
	case 3: // Draw Bones
		{
			int alpha = 255;
			int CCol  = 0;
			r3dColor Col[] = { r3dColor(255, 155, 0, alpha), r3dColor(200, 0, 255, alpha), r3dColor(255, 0, 200, alpha) };
			float w1 = 0.01f;
			float w2 = 0.025f;

			// drawing actual bone position/locations 
			for(int i=0;i<vd->skl->NumBones;i++) 
			{
				r3dBone &b = vd->skl->Bones[i];

				//r3dOutToLog("!!! Bone: %d (%d) '%s'\n", i, b.iBoneId, b.Name);

				bool bIsWheel = false;
				for( uint32_t j = 0; j < vd->numWheels; ++j )
				{
					if( &b == vd->wheelsData[ j ].wheelBone )
					{
						bIsWheel = true;
						break;
					}
				}
				
				// Wheels don't show properly atm.
				if( bIsWheel )
					continue;

				// Rotate the bone to it's Global pose.
				r3dPoint3D v1;
				PxTransform t;

				if (isTank == false)	
					t = vd->vehicle->getRigidDynamicActor()->getGlobalPose();
				else
					t = vd->tank->getRigidDynamicActor()->getGlobalPose();

				D3DXQUATERNION q, q1, q2 = D3DXQUATERNION( t.q.x, t.q.y, t.q.z, t.q.w );
				D3DXQuaternionRotationMatrix( &q1, &b.CurrentTM );
				q = q1 * q2;
				D3DXMATRIX m1;
				D3DXMatrixRotationQuaternion( &m1, & q );
				r3dMatrix m2;
				m2.Assign( m1._11, m1._12, m1._13,
						   m1._21, m1._22, m1._23,
						   m1._31, m1._32, m1._33 );
				b.vRelPlacement.Rotate( m2, v1 );
				v1 += GetPosition();

				r3dColor cc = Col[CCol];
				float sz = 0.2f;
				r3dDrawLine3D(v1 - r3dVector(0,sz,0), v1 + r3dVector(0,sz,0), gCam, w2, r3dColor(255,0,0,alpha));
				r3dDrawLine3D(v1 - r3dVector(sz,0,0), v1 + r3dVector(sz,0,0), gCam, w2, r3dColor(0,0,255,alpha));
				r3dDrawLine3D(v1 - r3dVector(0,0,sz), v1 + r3dVector(0,0,sz), gCam, w2, r3dColor(0,255,0,alpha));
				r3dPoint3D labelPos;
				if (r_show_vehicle_debug->GetInt() >= 4 && r3dProjectToScreen(v1 + r3dPoint3D(0, 0.2f, 0), &labelPos))
				{
					Font_Editor->PrintF(labelPos.x, labelPos.y, r3dColor(255, 255, 255), "%s", b.Name);
				}
				++CCol;
				CCol = CCol % 2;
			}
		}
		// Intentional fall-through
	case 2: // Mark the Seat positions
		{
			r3dVector seat;
			for( uint32_t i = 0; i < vd->numSeats; ++i )
			{
				if( vd->GetSeatIndex( seat, i ) )
					r3dDrawSphereSolid( GetPosition() + seat, 0.1f, gCam, r3dColor(255, 255, 255));
			}
		}
		// Intentional fall-through
	case 1:
		if( NetworkLocal || GetPlayerById(gClientLogic().localPlayer_->GetNetworkID()) )
		{
			position = r3dPoint3D(900, 10, 0);
			Font_Editor->PrintF(position.x, position.y, r3dColor(255,255,255), "Reverse: %s", g_pPhysicsWorld->m_VehicleManager->IsInReverse() ? "true" : "false");

			if (isTank == false)
			{
				Font_Editor->PrintF(position.x, position.y + 12, r3dColor(255,255,255), "Accel: %s:%.3f", g_pPhysicsWorld->m_VehicleManager->carControlData.getDigitalAccel() ? "true" : "false", g_pPhysicsWorld->m_VehicleManager->carControlData.getAnalogAccel());
				Font_Editor->PrintF(position.x, position.y + 24, r3dColor(255,255,255), "Brake: %s:%.3f", g_pPhysicsWorld->m_VehicleManager->carControlData.getDigitalBrake() ? "true" : "false", g_pPhysicsWorld->m_VehicleManager->carControlData.getAnalogBrake());
			}

			Font_Editor->PrintF(position.x, position.y + 36, r3dColor(255,255,255), "Gear: %d", vd->vehicle->mDriveDynData.getCurrentGear());
			Font_Editor->PrintF(position.x, position.y + 48, r3dColor(255,255,255), "Gear Num Ratios: %d", vd->vehicle->mDriveSimData.getGearsData().mNumRatios);
			Font_Editor->PrintF(position.x, position.y + 60, r3dColor(255,255,255), "Gear Final Ratio: %.3f", vd->vehicle->mDriveSimData.getGearsData().mFinalRatio);
			Font_Editor->PrintF(position.x, position.y + 72, r3dColor(255,255,255), "Gear Ratio: %.3f", vd->vehicle->mDriveSimData.getGearsData().mRatios[vd->vehicle->mDriveDynData.getCurrentGear()]);
			Font_Editor->PrintF(position.x, position.y + 84, r3dColor(255,255,255), "Gear Switch Time: %.3f", vd->vehicle->mDriveSimData.getGearsData().mSwitchTime);
			Font_Editor->PrintF(position.x, position.y + 96, r3dColor(255,255,255), "Engine RPM: %.3f", vd->GetEngineRpm());
			Font_Editor->PrintF(position.x, position.y + 108, r3dColor(255,255,255), "Engine Rad/Sec: %.3f", vd->vehicle->mDriveDynData.getEngineRotationSpeed());
			Font_Editor->PrintF(position.x, position.y + 120, r3dColor(255,255,255), "Max Speed: %.3f", vd->GetMaxSpeed());
			position = r3dPoint3D(1100, 10, 0);
			Font_Editor->PrintF(position.x, position.y, r3dColor(255,255,255), "Peak Torque: %.3f", vd->engineData.mPeakTorque);
			char diffType[64];
			switch( vd->diffData.mType )
			{
			case PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD:		sprintf(diffType, "4-wheel drive with limited slip"); break;
			case PxVehicleDifferential4WData::eDIFF_TYPE_LS_FRONTWD:	sprintf(diffType, "front-wheel drive with limited slip"); break;
			case PxVehicleDifferential4WData::eDIFF_TYPE_LS_REARWD:		sprintf(diffType, "rear-wheel drive with limited slip"); break;
			case PxVehicleDifferential4WData::eDIFF_TYPE_OPEN_4WD:		sprintf(diffType, "4-wheel drive with open differential"); break;
			case PxVehicleDifferential4WData::eDIFF_TYPE_OPEN_FRONTWD:	sprintf(diffType, "front-wheel drive with open differential"); break;
			case PxVehicleDifferential4WData::eDIFF_TYPE_OPEN_REARWD:	sprintf(diffType, "rear-wheel drive with open differential"); break;
			default: sprintf(diffType, "Unknown"); break;
			}
			Font_Editor->PrintF(position.x, position.y + 12, r3dColor(255,255,255), "Differential Type: %s", diffType);
			Font_Editor->PrintF(position.x, position.y + 24, r3dColor(255,255,255), "Front/Rear Split: %.3f", vd->diffData.mFrontRearSplit);
			Font_Editor->PrintF(position.x, position.y + 36, r3dColor(255,255,255), "Front Left/Right Split: %.3f", vd->diffData.mFrontLeftRightSplit);
			Font_Editor->PrintF(position.x, position.y + 48, r3dColor(255,255,255), "Rear Left/Right Split: %.3f", vd->diffData.mRearLeftRightSplit);
			Font_Editor->PrintF(position.x, position.y + 60, r3dColor(255,255,255), "Side speed: %.3f", vd->GetSideSpeed());
			Font_Editor->PrintF(position.x, position.y + 72, r3dColor(255,255,255), "Long Tire slip: %.3f %.3f %.3f %.3f", vd->GetLongTireSlip(0), vd->GetLongTireSlip(1), vd->GetLongTireSlip(2), vd->GetLongTireSlip(3));
			Font_Editor->PrintF(position.x, position.y + 84, r3dColor(255,255,255), "Lat Tire slip: %.3f %.3f %.3f %.3f", vd->GetLatTireSlip(0), vd->GetLatTireSlip(1), vd->GetLatTireSlip(2), vd->GetLatTireSlip(3));
			Font_Editor->PrintF(position.x, position.y + 96, r3dColor(255,255,255), "Distance From Ground %.3f", distanceFromGround);
			char seats[256] = { 0 };
			
			/*
			for (uint32_t i = 0; i < vd->numSeats; ++i)
			{
				if (!players[i])
					continue;

				char seat[16] = { 0 };
				sprintf(seat, "Seat(%d):taken ", players[i]->seatPosition);
				strcat(seats, seat);			
			}
			*/
			Font_Editor->PrintF(position.x, position.y + 108, r3dColor(255,255,255), "%s", seats);
		}
		if (r3dProjectToScreen(GetPosition() + r3dPoint3D(0, 1, 0), &position))
		{
			Font_Label->PrintF(position.x, position.y - 30, r3dColor(255, 255, 255), "Id: %d", vehicleId);
			Font_Label->PrintF(position.x, position.y - 15, r3dColor(255, 255, 255), "Speed: %.1f", vd->GetSpeed());
			Font_Label->PrintF(position.x, position.y, r3dColor(255, 255, 255), "RPM: %f", vd->GetEngineRpm());
			Font_Label->PrintF(position.x, position.y + 15, r3dColor(255, 255, 255), "HP: %d", curDurability);
			Font_Label->PrintF(position.x, position.y + 30, r3dColor(255, 255, 255), "Fuel: %d/%d", curFuel, maxFuel);
			Font_Label->PrintF(position.x, position.y + 45, r3dColor(255, 255, 255), "Rotation: %.3f,%.3f,%.3f", GetRotationVector().x,GetRotationVector().y,GetRotationVector().z);
		}
		break;
	default:
		break;
	}
}
#endif

//////////////////////////////////////////////////////////////////////////

const bool obj_Vehicle::getExitSpace( r3dVector& outVector, int exitIndex)
{
	return vd->GetExitIndex(outVector, exitIndex);
}

const bool obj_Vehicle::getFireSpace( r3dVector& outVector, int exitIndex )
{
	return vd->GetFireIndex( outVector,exitIndex);
}

const bool obj_Vehicle::getTurretPosition( r3dVector& outVector, int exitIndex )
{
	return vd->GetTurretPositionIndex( outVector,exitIndex);
}

BOOL obj_Vehicle::OnNetReceive(DWORD EventID, const void* packetData, int packetSize)
{
	r3d_assert(!(ObjFlags & OBJFLAG_JustCreated)); // make sure that object was actually created before processing net commands

	if (!isReady)
		return TRUE;

	switch(EventID)
	{
	default: return FALSE;

	case PKT_C2C_VehicleMoveSetCell:
		{
			const PKT_C2C_VehicleMoveSetCell_s& n = *(PKT_C2C_VehicleMoveSetCell_s*)packetData;
			r3d_assert(packetSize == sizeof(n));

			OnNetPacket(n);
			break;
		}	
	case PKT_C2C_VehicleMoveRel:
		{
			const PKT_C2C_VehicleMoveRel_s& n = *(PKT_C2C_VehicleMoveRel_s*)packetData;
			r3d_assert(packetSize == sizeof(n));

			OnNetPacket(n);
			break;
		}		
	case PKT_S2C_VehicleUnstuck:
		{
			const PKT_S2C_VehicleUnstuck_s& n = *(PKT_S2C_VehicleUnstuck_s*)packetData;
			r3d_assert(packetSize == sizeof(n));

			OnNetPacket(n);
			break;
		}
	case PKT_C2C_VehicleAction:
		{
			const PKT_C2C_VehicleAction_s& n = *(PKT_C2C_VehicleAction_s*)packetData;
			r3d_assert(packetSize == sizeof(n));

			OnNetPacket(n);
			break;
		}
	case PKT_S2C_VehicleDurability:
		{
			const PKT_S2C_VehicleDurability_s& n = *(PKT_S2C_VehicleDurability_s*)packetData;
			r3d_assert(packetSize == sizeof(n));

			OnNetPacket(n);
			break;
		}
	case PKT_S2C_VehicleFuel:
		{
			const PKT_S2C_VehicleFuel_s& n = *(PKT_S2C_VehicleFuel_s*)packetData;
			r3d_assert(packetSize == sizeof(n));

			OnNetPacket(n);
			break;
		}
	case PKT_C2C_VehicleHeadlights:
		{
			const PKT_C2C_VehicleHeadlights_s& n = *(PKT_C2C_VehicleHeadlights_s*)packetData;
			r3d_assert(packetSize == sizeof(n));

			OnNetPacket(n);
			break;
		}
	case PKT_C2S_TurrerAngles:
		{
			const PKT_C2S_TurrerAngles_s& n = *(PKT_C2S_TurrerAngles_s*)packetData;
			r3d_assert(packetSize == sizeof(n));

			OnNetPacket(n);
			break;
		}
	}

	return TRUE;
}

void obj_Vehicle::OnNetPacket(const PKT_C2C_VehicleMoveSetCell_s& n)
{
	if (!NetworkLocal && !wasNetworkLocal) 
	{
		netMover.SetCell(n);
	}
}

void obj_Vehicle::OnNetPacket(const PKT_C2C_VehicleMoveRel_s& n)
{
	if (!NetworkLocal && !wasNetworkLocal) 
	{
		lastMoveTime = r3dGetTime();

		const CVehicleNetCellMover::moveData_s& md = netMover.DecodeMove(n);

		r3dPoint3D vel = netMover.GetVelocityToNetTarget(
			GetPosition(),
			vd->GetMaxSpeed() * 3.5f,
			1.0f);

		netVelocity = vel;

		reportedSpeed = (float)n.speed;
		reportedRpm	 = (float)n.engineRpm;
		reportedSideSpeed = (float)n.sideSpeed / 10.0f;
		
		lastRotationVector = r3dVector(md.rot.x, md.rot.y, md.rot.z);

		{
			for( uint32_t i = 0; i < vd->numWheels; ++i )
			{
				float turn = (float)n.wheelTurnAngle[i] * 1000 / 65535;
				float wheelRot = (float)n.wheelRotation[i] * 1000 / 65535;
				float wheelSuspTravel = (float)n.wheelSuspTravel[i] * 1000 / 65535;

				D3DXQUATERNION rotation, turnAngle;
				D3DXQuaternionRotationYawPitchRoll(&turnAngle, turn, 0, 0);
				D3DXQuaternionRotationYawPitchRoll(&rotation, 0, wheelRot, 0);
				PxQuat qTurn(turnAngle.x, turnAngle.y, turnAngle.z, turnAngle.w);
				PxQuat qRot(rotation.x, rotation.y, rotation.z, rotation.w);
				if(!qTurn.isSane())
					qTurn = PxQuat(0,0,0,1);
				if(!qRot.isSane())
					qRot = PxQuat(0,0,0,1);
				PxQuat q(qTurn * qRot);
				q.normalize();

				PxVec3& vWheel = vd->wheelCenterOffsets[ i ];
				PxTransform t(PxVec3(vWheel.x, vWheel.y + wheelSuspTravel, vWheel.z), q);
				
				PxVehicleWheels* wheels;
				if (isTank == false)
					wheels = vd->vehicle;
				else
					wheels = vd->tank;
				PxShape *wheel = NULL;
				wheels->getRigidDynamicActor()->getShapes(&wheel, 1, i);
				if (wheel)
					wheel->setLocalPose(t);
			}
		}
	}
}

void obj_Vehicle::OnNetPacket(const PKT_S2C_VehicleUnstuck_s& n)
{
	if (n.isSuccess)
	{
		SetPosition(n.position);
		SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Vehicles/Unstuck/Successful"), GetPosition());
	}
	else
	{
		SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/Vehicles/Unstuck/Failure"), GetPosition());
	}
}

void obj_Vehicle::OnNetPacket(const PKT_C2C_VehicleAction_s& n)
{
	// todo - 
	// car audio 
	// car particles
	if ((int)n.action == 2)
	{
		if (CreateParams.vehicleType == VEHICLETYPE_POLICE ||CreateParams.vehicleType == VEHICLETYPE_COPCAR)
		{
			ClaxonSnd = SoundSys.Play(sndPoliceClaxon,GetPosition());
			SoundSys.Start(ClaxonSnd);
		}
		else {
			ClaxonSnd = SoundSys.Play(sndVehicleClaxon,GetPosition());
			SoundSys.Start(ClaxonSnd);
		}
	}
}

void obj_Vehicle::OnNetPacket(const PKT_S2C_VehicleFuel_s& n)
{
	curFuel = n.fuel;

	if (curFuel == 0)
		OnRunOutOfGas();
}

void obj_Vehicle::OnNetPacket(const PKT_S2C_VehicleDurability_s& n)
{
	SetDurability(n.durability, false);
}

void obj_Vehicle::OnNetPacket(const PKT_C2C_VehicleHeadlights_s& n)
{
	EnableHeadlights(n.isHeadlightsEnabled);
}

void obj_Vehicle::OnNetPacket(const PKT_C2S_TurrerAngles_s& n)
{
		camangle2       = n.camangle;
		cameraUpDown    = n.cameraUpDown;
}

void obj_Vehicle::Repair()
{
}

int obj_Vehicle::GetArmor() const
{
	return CreateParams.armor;
}

int obj_Vehicle::GetWeight() const
{
	return CreateParams.weight;
}

int obj_Vehicle::GetDurability()
{
	return curDurability;
}

void obj_Vehicle::SetDurability(short durability, bool isOnLoad)
{
	if(durability != curDurability)
		durabilityChanged = true;

	curDurability = durability;
	
	int dura = GetDurabilityAsPercent();

	if (curDurability > 0)
	{
		if (dura <= 10)
		{
			StartDamageParticles(true, 2);
		}
		else if (dura <= 20)
		{
			StartDamageParticles(true, 1);
		}
		else if (dura <= 30)
		{
			StartDamageParticles(true, 0);
		}
		else if (dura >= 31)
		{
			StartDamageParticles(false, 0);
		}
	}
	else
	{
		// we only want the vehicle to smoke after the explosion until it despawns
		if (isOnLoad)
		{
			hasExploded = true;
			StartDamageParticles(true, 0);
		}
		else if (!hasExploded && durability <= 0)
		{
			DoExplosion();
		}
	}
}

void obj_Vehicle::UpdateRotor()
{
	if (isHelicopter)
	{
		if (HasDriver() && GetFuelAsPercent() > 0)
		{
			if (RotorX<=360)
			{
				RotorX+=1.0f+HSpeed;
				if (RotorX>=360)
				{
					if (HSpeed<100.0f)
						HSpeed++;
						RotorX=0.0f;
				}
			}

			if (RotorY<=360)
			{
				RotorY+=1.0f+HSpeed;
				if (RotorY>=360)
				{
					RotorY=0.0f;
				}
			}
		}
		else {
			if (HSpeed>2.0f)
			{
				RotorX+=1.0f+HSpeed;
				if (RotorX>=360)
				{
					HSpeed--;
					RotorX=0.0f;
				}
			}

			if (HSpeed>2.0f)
			{
				RotorY+=1.0f+HSpeed;
				if (RotorY>=360)
				{
					RotorY=0.0f;
				}
			}
			else {
				HSpeed=2.0f;
				if (SoundSys.IsHandleValid(sndEngine))
					PlayEngineSound(false);
			}
		}
	}
}

void obj_Vehicle::UpdateSounds()
{
	// ensure sounds are playing for this vehicle
	if (HasPlayers() && GetFuelAsPercent() > 0)
	{
		if (!SoundSys.IsHandleValid(sndEngine))
			PlayEngineSound(true);

		if (!SoundSys.IsHandleValid(sndTractionLoop))
			PlayTractionLoopSound(true);
	}
	else
	{
		if (CreateParams.vehicleType != VEHICLETYPE_HELICOPTER)
		{
			if (SoundSys.IsHandleValid(sndEngine))
				PlayEngineSound(false);

			if (SoundSys.IsHandleValid(sndTraction))
				PlayTractionSound(false, -1);

			if (SoundSys.IsHandleValid(sndTractionLoop))
				PlayTractionLoopSound(false);

			return;
		}
	}

	if (SoundSys.IsHandleValid(ClaxonSnd))
	{
		if (SoundSys.isPlaying(ClaxonSnd))
			SoundSys.Set3DAttributes(ClaxonSnd, &GetPosition(), 0, 0);
	}

	if (GetSpeed() > 0.0f)
	{
		R3DPROFILE_START("RayCast");
		PhysicsCallbackObject* target = NULL;
		const MaterialType *mt = 0;

		PxRaycastHit hit;
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC));

		bool hitResult = g_pPhysicsWorld->raycastSingle(PxVec3(GetPosition().x, GetPosition().y + 0.5f, GetPosition().z), PxVec3(0, -1, 0), 1.0f, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter);
		if( hitResult )
		{
			if( hit.shape && (target = static_cast<PhysicsCallbackObject*>(hit.shape->getActor().userData)))
			{
				PxU32 faceIndex = hit.faceIndex;
				{
					PxTriangleMeshGeometry pxGeometry;
					if(hit.shape->getTriangleMeshGeometry(pxGeometry))
					{
						r3d_assert(pxGeometry.triangleMesh);
						const PxU32* remap = pxGeometry.triangleMesh->getTrianglesRemap();
						r3d_assert(remap);
						PxU32 numTriangles = pxGeometry.triangleMesh->getNbTriangles();
						r3d_assert(faceIndex < numTriangles);
						faceIndex = remap[faceIndex];
					}
				}
				r3dMaterial* material = 0;
				GameObject *gameObj = target->isGameObject();
				if(gameObj)
				{
					if( gameObj->isObjType( OBJTYPE_Mesh ) )
						material = static_cast< MeshGameObject* > ( target )->MeshLOD[ 0 ]->GetFaceMaterial( faceIndex );
					if(gameObj->isObjType(OBJTYPE_Terrain))
					{
						mt = Terrain->GetMaterialType(GetPosition());
					}
				}
				else if(target->hasMesh())
				{
					material = target->hasMesh()->GetFaceMaterial( faceIndex );
				}
				if (!material)
					material = target->GetMaterial(faceIndex);

				if(material && !mt)
				{
					mt = g_pMaterialTypes->Get(r3dHash::MakeHash(material->TypeName));
				}
			}
		}
		R3DPROFILE_END("RayCast");

		if (sndTractionId > 0)
		{
			int tractionSoundId = sndTractionId;

			if (mt)
				tractionSoundId = mt->vehicleSound;

			if (tractionSoundId != sndTractionId)
				PlayTractionSound(false, -1);

			if (sndTraction == NULL)
			{
				sndTractionId = tractionSoundId;

				if (tractionSoundId > 0)
				{
					PlayTractionSound(true, tractionSoundId);					
				}
			}
		}

		if (sndTraction)
		{
			float speed = GetSpeed();

			SoundSys.SetParamValue(sndTraction, "speed", speed);
			SoundSys.Set3DAttributes(sndTraction, &GetPosition(), 0, 0);
		}
	}

	if (sndTractionLoop && isTank == false)
	{
		sideSpeedVolume = 0.0f;

		if (NetworkLocal)
		{
			if (vd->GetSideSpeed() > 15.f)
				sideSpeedVolume = 1.0f;
			else 
				sideSpeedVolume = vd->GetSideSpeed() / 15.0f;

			if (vd->GetSpeed() > 0.25f)
			{
				if (vd->GetLongTireSlip(0) > 0.9f)
					sideSpeedVolume += vd->GetLongTireSlip(0);

				if (vd->GetLatTireSlip(0) > 0.3f)
					sideSpeedVolume += vd->GetLatTireSlip(0);
			}

			if (sideSpeedVolume > 1.0f)
				sideSpeedVolume = 1.0f;
		}
		else
		{
			sideSpeedVolume = reportedSideSpeed;
		}

		if (SoundSys.IsHandleValid(sndTractionLoop))
		{
			SoundSys.SetParamValue(sndTractionLoop, "Skid_Amount", sideSpeedVolume);
			SoundSys.SetParamValue(sndTractionLoop, "Concrete_Surface_Amount", sideSpeedVolume);
			SoundSys.Set3DAttributes(sndTractionLoop, &GetPosition(), 0, 0);
		}
	}

	if (SoundSys.IsHandleValid(sndEngine))
	{	
		SoundSys.SetParamValue(sndEngine, "rpm", GetEngineRpm());
		SoundSys.Set3DAttributes(sndEngine, &GetPosition(), 0, 0);

		if (isTank == true)
		{
			SoundSys.SetParamValue(TreadSnd, "speed", GetSpeed()*5);
			SoundSys.Set3DAttributes(TreadSnd, &GetPosition(), 0, 0);
		}
		if (isHelicopter == true)
		{
			float HeliSpeed = HSpeed<64?HSpeed:64;
			SoundSys.SetParamValue(sndEngine, "speed", HeliSpeed);
			SoundSys.Set3DAttributes(sndEngine, &GetPosition(), 0, 0);
		}
			
	}
}

void obj_Vehicle::SendActions(BYTE actions)
{
	PKT_C2C_VehicleAction_s n;
	n.action = actions;

	p2pSendToHost(this, &n, sizeof(n));
}

bool obj_Vehicle::IsOnGround()
{
	// todo
	// check for cheaters who can fly.

	// query PhysX vehicle to detect if wheels are on ground.

	return true;
}

PxReal obj_Vehicle::GetEngineRpm()
{
	float adjustedRpm = 0.0f;

	if (NetworkLocal)
		adjustedRpm = vd->GetEngineRpm();
	else
		adjustedRpm = reportedRpm;

	// just in case some butthole decides to modify the data and send more than 8000 rpms.
	// rpms is sent from local client to remote, because server has no idea about vehicle physics really.
	if (adjustedRpm > 8000)
		adjustedRpm = 8000;

	return adjustedRpm;
}

void obj_Vehicle::OnContactModify(PhysicsCallbackObject *obj, PxContactSet& contacts)
{
	if (!obj)
		return;

	GameObject* gameObj = obj->isGameObject();
	if (!gameObj)
		return;

	if (gameObj->isObjType(OBJTYPE_Barricade) && CanSmashBarricade())
	{
		for(uint32_t i = 0; i < contacts.size(); ++i)
		{
			contacts.ignore(i);

			obj_Barricade* barricade = (obj_Barricade*)gameObj;
			barricade->Destroy(false);
		}
	}
}

void obj_Vehicle::OnCollide(PhysicsCallbackObject *obj, CollisionInfo &trace)
{
	lastCollisionNormal = trace.Normal;
	
	if (!obj)
		return;

	GameObject* gameObj = obj->isGameObject();
	if (!gameObj)
		return;

	if (gameObj->isObjType(OBJTYPE_Zombie))
	{
		obj_Zombie* zombie = (obj_Zombie*)gameObj;
		if (!zombie)
			return;

		PlayCrashSound(OBJTYPE_Zombie);
		
	}
	else if (gameObj->isObjType(OBJTYPE_Human))
	{
		obj_Player* player = (obj_Player*)gameObj;
		if (!player)
			return;
		
		player->StartPushFromVehicle(GetSpeed(), vd->GetMaxSpeed());
		PlayCrashSound(OBJTYPE_Human);
	}
	else if (gameObj->isObjType(OBJTYPE_Mesh))
	{	
		PlayCrashSound(OBJTYPE_Mesh, NULL);
	}
	else if (gameObj->isObjType(OBJTYPE_Terrain))
	{
		PlayCrashSound(OBJTYPE_Terrain);
	}
	else if (gameObj->isObjType(OBJTYPE_Vehicle))
	{
		PlayCrashSound(OBJTYPE_Vehicle);
	}
}

void obj_Vehicle::PlayCrashSound(int objectType, const MaterialType* mt)
{
	if (!isSoundsLoaded)
		return;

	if (r3dGetTime() > lastCrashSoundTime + crashSoundWaitTime)
	{
		lastCrashSoundTime = r3dGetTime();

		float volume = 0.0f;

		if (GetSpeed() > 0.0f)
			volume = GetSpeed() / 45.0f;

		switch (objectType)
		{
		case OBJTYPE_Human:
			volume = 1.0f;
			SoundSys.PlayAndForgetWithVolume(sndCrashPlayerId, GetPosition(), volume);
			break;
		case OBJTYPE_Zombie:
			volume = 1.0f;
			SoundSys.PlayAndForgetWithVolume(sndCrashZombieId, GetPosition(), volume);
			break;
		case OBJTYPE_Terrain:
			SoundSys.PlayAndForgetWithVolume(sndCrashRockId, GetPosition(), volume);
			break;
		case OBJTYPE_Mesh:
		case OBJTYPE_Vehicle:
			SoundSys.PlayAndForgetWithVolume(sndCrashMetalId, GetPosition(), volume);
			break;
		/*case OBJTYPE_Mesh:
			if (!mt)
			{
				SoundSys.PlayAndForget(sndCrashMetalId, GetPosition());
				return;
			}

			SoundSys.PlayAndForget(mt->vehicleCrashSound, GetPosition());
			break;*/
		}
	}
}

void obj_Vehicle::IsGoingToCollide(r3dPoint3D dir)
{
	if (gClientLogic().localPlayer_->CurLoadout.GameFlags & wiCharDataFull::GAMEFLAG_NearPostBox)
		return;

	PxBoxGeometry boxTest(1.1f, 0.3f, 1.0f);
	PxTransform boxPose; 
	PxTransform vehiclePose;

	if (isTank == false)
	{
		boxPose = vd->vehicle->getRigidDynamicActor()->getGlobalPose();
		vehiclePose = vd->vehicle->getRigidDynamicActor()->getGlobalPose();
	}
	else
	{
		boxPose = vd->tank->getRigidDynamicActor()->getGlobalPose();
		vehiclePose = vd->tank->getRigidDynamicActor()->getGlobalPose();
	}

	r3dPoint3D vehicle(vehiclePose.p.x, vehiclePose.p.y, vehiclePose.p.z);

	PxSceneQueryFilterData filter(PxFilterData((1<<PHYSCOLL_NETWORKPLAYER)|(1<<PHYSCOLL_CHARACTERCONTROLLER), 0, 0, 0), PxSceneQueryFilterFlag::eDYNAMIC);
	PxSceneQueryFilterData filterTest(PxFilterData((1<<PHYSCOLL_NETWORKPLAYER)|(1<<PHYSCOLL_CHARACTERCONTROLLER)|COLLIDABLE_PLAYER_COLLIDABLE_MASK_NOVEH, 0, 0, 0), PxSceneQueryFilterFlag::eDYNAMIC|PxSceneQueryFilterFlag::eSTATIC);

	PxVec3 castDirection(dir.x, dir.y, dir.z);
	if (g_pPhysicsWorld->m_VehicleManager->IsInReverse())
		castDirection = PxVec3(-dir.x, -dir.y, -dir.z);

	float MAX_CASTING_DISTANCE = 5.3f * GetSpeed() / vd->GetMaxSpeed(); //

	bool isBlockHit;
	PxSweepHit sweepHits[128];
	int results = g_pPhysicsWorld->PhysXScene->sweepMultiple(boxTest, vehiclePose, castDirection, MAX_CASTING_DISTANCE, PxSceneQueryFlag::eNORMAL, sweepHits, 128, isBlockHit, filter);

	for (int i = 0; i < results; ++i)
	{
		PhysicsCallbackObject* target = NULL;

		if (sweepHits[i].shape && (target = static_cast<PhysicsCallbackObject*>(sweepHits[i].shape->getActor().userData)))
		{
			GameObject* gameObj = target->isGameObject();
			if (!gameObj)
				continue;

			PxRaycastHit hit;
			r3dPoint3D impact(sweepHits[i].impact.x, sweepHits[i].impact.y, sweepHits[i].impact.z);
			r3dVector testDir = (impact - vehicle).Normalize();
			float distance = (impact - vehicle).Length();

			if (gameObj->isObjType(OBJTYPE_Human))
			{
				obj_Player* player = (obj_Player*)gameObj;
				if (player)
				{
					if (g_pPhysicsWorld->raycastSingle(PxVec3(vehicle.x, vehicle.y, vehicle.z), PxVec3(testDir.x, testDir.y, testDir.z), distance, PxSceneQueryFlag::eIMPACT|PxSceneQueryFlag::eNORMAL|PxSceneQueryFlag::eINITIAL_OVERLAP, hit, filterTest))
					{
						PhysicsCallbackObject* targetTest = NULL;

						if (hit.shape && (targetTest = static_cast<PhysicsCallbackObject*>(hit.shape->getActor().userData)))
						{
							GameObject* gameObjTest = targetTest->isGameObject();
							if (gameObjTest && gameObjTest->isObjType(OBJTYPE_Human))
								player->AttemptVehicleHit(vd->GetSpeed(), this);
						}
					}					
					else // it's not a game object, just hit the player
						player->AttemptVehicleHit(vd->GetSpeed(), this);
				}
			}
			else if (gameObj->isObjType(OBJTYPE_Zombie))
			{

				obj_Zombie* zombie = (obj_Zombie*)gameObj;
				if (zombie)
				{

					if (g_pPhysicsWorld->raycastSingle(PxVec3(vehicle.x, vehicle.y, vehicle.z), PxVec3(testDir.x, testDir.y, testDir.z), distance, PxSceneQueryFlag::eIMPACT|PxSceneQueryFlag::eNORMAL|PxSceneQueryFlag::eINITIAL_OVERLAP, hit, filterTest) && !zombie->IsDogZombie())
					{
						PhysicsCallbackObject* targetTest = NULL;
	
						if (hit.shape && (targetTest = static_cast<PhysicsCallbackObject*>(hit.shape->getActor().userData)))
						{

							GameObject* gameObjTest = targetTest->isGameObject();
							if (gameObjTest && gameObjTest->isObjType(OBJTYPE_Zombie))
								zombie->AttemptVehicleHit(vd->GetSpeed(), this);
						}
					}
					else
						zombie->AttemptVehicleHit(vd->GetSpeed(), this);
				}
			}
			else if (gameObj->isObjType(OBJTYPE_Barricade))
			{
				if( GetSpeed() > 10.0f )
				{
					obj_Barricade* barricade = (obj_Barricade*)gameObj;
					if (barricade->CanStopVehicle())
						g_pPhysicsWorld->m_VehicleManager->carControlData.setDigitalAccel(false);

					barricade->Destroy(false);
				}
			}
		}
	}
}

void obj_Vehicle::DoSlowDown()
{
	isSlowedDown = true;
	slowedDownTime = r3dGetTime();

	if (isTank == false)
		g_pPhysicsWorld->m_VehicleManager->carControlData.setDigitalBrake(true);
}

void obj_Vehicle::StartDamageParticles(bool enable, int damageLevel)
{
	if (!enable && isDamageParticlesLoaded)
	{	
		isDamageParticlesLoaded = false;

		particleDamage->bKillDelayed = 1;
		if (SoundSys.IsHandleValid(sndFire))
			SoundSys.Stop(sndFire);
		isOnFire = false;
		return;
	}

	isSmoking = enable;

	if (isDamageParticlesLoaded && currentDamageLevel == damageLevel)
		return;

	currentDamageLevel = damageLevel;

	if (enable && GetDamageParticles(damageLevel))
	{
		// disable previous smoke if loaded
		if (isDamageParticlesLoaded)
		{
			isDamageParticlesLoaded = false;
			//SoundSys.Stop(sndFire);
			particleDamage->bKill = 1;
		}

		particleDamage = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", GetDamageParticles(damageLevel), GetPosition());
		isDamageParticlesLoaded = true;

		if (damageLevel > 1)
		{
			//SoundSys.Play(sndFireId, GetPosition());
			isOnFire = true;
		}
		else
		{
			isOnFire = false;
			//SoundSys.Stop(sndFire);
		}

		return;
	}

	// disable
	if (isDamageParticlesLoaded)
	{
		particleDamage->bKillDelayed = 1;
		particleDamage = NULL;
		isDamageParticlesLoaded = false;
		if (isOnFire)
		{
			SoundSys.Stop(sndFire);
			isOnFire = false;
		}
	}
}

void obj_Vehicle::DoExplosion()
{
	if (hasExploded)
		return;

	hasExploded = true;

	PlayExplosionSound();

	isCarDrivable = false;

	if (GetExplosion())
	{
		particleExplosion = (obj_ParticleSystem*)srv_CreateGameObject("obj_ParticleSystem", GetExplosion(), GetPosition());
		isExplosionLoaded = true;
		isPoliceSirenEnabled = false;
	}

	gExplosionVisualController.AddExplosion(GetPosition(), 15.0f);

	if (sndEngine)
	{
		SoundSys.Stop(sndEngine);
		sndEngine = NULL;
	}

	StartDamageParticles(true, 0);
}

void obj_Vehicle::PhysicsEnable( const int& physicsEnabled )
{
	UpdateTransform();

	if(physicsEnabled != (int)m_bEnablePhysics)
	{
		m_bEnablePhysics = physicsEnabled?true:false;
		if (!m_bEnablePhysics)
		{
			SAFE_DELETE(PhysicsObject);
		}
		else
		{
			CreatePhysicsData();
		}
	}
}

void obj_Vehicle::AddOrUpdatePlayer(obj_Player* player)
{
	r3d_assert(!players[player->seatPosition]);

	players[player->seatPosition] = player;
}

obj_Player* obj_Vehicle::GetPlayerById(DWORD playerId) const
{
	for (uint32_t i = 0; i < vd->numSeats; ++i)
	{
		if (!players[i])
			continue;

		if (players[i]->GetNetworkID() == playerId)
			return players[i];
	}

	return 0;
}

bool obj_Vehicle::RemovePlayerById(DWORD playerId)
{
	for (uint32_t i = 0; i < vd->numSeats; ++i)
	{
		if (!players[i])
			continue;

		if (players[i]->GetNetworkID() == playerId)
		{
			players[i] = 0;
			return true;
		}
 	}

	return false;
}

bool obj_Vehicle::RemovePlayerBySeat(int seat)
{
	// this method is only ran for people disconnecting, so clean up.
	r3d_assert(players[seat]);

	players[seat]->ClearVehicle();
	players[seat]->TogglePhysicsSimulation(true);
	players[seat] = 0;

	if (hudMain)
		hudMain->setCarSeatInfo(seat, "empty");

	return true;
}

void obj_Vehicle::EnterVehicle(obj_Player* player)
{
	r3d_assert(player);

	if (!HasPlayers())
		PlayEngineStartSound();

	AddOrUpdatePlayer(player);

	if (player->NetworkLocal)
	{
		wasNetworkLocal = false;
		if (player->seatPosition == 0)
		{
			NetworkLocal = true;

			if (!isActivePhysXVehicle)
			{
				isActivePhysXVehicle = g_pPhysicsWorld->m_VehicleManager->SetActiveVehicle(vd);

				PhysicsEnable(false);
				SwitchToDrivable(true);
			}

			if (isTank == false)
				vd->vehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
			else
				vd->tank->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
		}

#ifndef FINAL_BUILD
		if (!g_bEditMode)
		{
#endif
			if (CreateParams.vehicleType == VEHICLETYPE_BUGGY)
				hudMain->setCarTypeInfo("buggy");
			else if (CreateParams.vehicleType == VEHICLETYPE_STRYKER)
				hudMain->setCarTypeInfo("stryker");
			else if (CreateParams.vehicleType == VEHICLETYPE_ZOMBIEKILLER)
				hudMain->setCarTypeInfo("truck");
			else if (CreateParams.vehicleType == VEHICLETYPE_HUMMER)
				hudMain->setCarTypeInfo("car");
			else if (CreateParams.vehicleType == VEHICLETYPE_POLICE)
				hudMain->setCarTypeInfo("car");
			else if (CreateParams.vehicleType == VEHICLETYPE_COPCAR)
				hudMain->setCarTypeInfo("car");
			else if (CreateParams.vehicleType == VEHICLETYPE_ABANDONEDSUV)
				hudMain->setCarTypeInfo("car");
			else if (CreateParams.vehicleType == VEHICLETYPE_BONECRUSHER)
				hudMain->setCarTypeInfo("car");
			else if (CreateParams.vehicleType == VEHICLETYPE_DUNEBUGGY)
				hudMain->setCarTypeInfo("buggy");
			else if (CreateParams.vehicleType == VEHICLETYPE_ECONOLINE)
				hudMain->setCarTypeInfo("truck");
			else if (CreateParams.vehicleType == VEHICLETYPE_LARGETRUCK)
				hudMain->setCarTypeInfo("truck");
			else if (CreateParams.vehicleType == VEHICLETYPE_MILITARYAPC)
				hudMain->setCarTypeInfo("car");
			else if (CreateParams.vehicleType == VEHICLETYPE_PARAMEDIC)
				hudMain->setCarTypeInfo("truck");
			else if (CreateParams.vehicleType == VEHICLETYPE_SUV)
				hudMain->setCarTypeInfo("car");
			else if (CreateParams.vehicleType == VEHICLETYPE_HELICOPTER)
				hudMain->setCarTypeInfo("huey");
			if (CreateParams.vehicleType == VEHICLETYPE_TANK_T80)
				hudMain->setCarTypeInfo("tank");
			if (CreateParams.vehicleType == VEHICLETYPE_JEEP)
				hudMain->setCarTypeInfo("car");

			for (uint32_t i = 0; i < vd->numSeats; ++i)
			{
				if (!players[i])
				{
					hudMain->setCarSeatInfo(i, "empty");
					continue;
				}
				
				if (players[i]->GetNetworkID() == player->GetNetworkID())
					hudMain->setCarSeatInfo(i, "player");
				else if (players[i]->GroupID != 0 && players[i]->GroupID == player->GroupID)
					hudMain->setCarSeatInfo(i, "team");
				else if (players[i]->ClanID != 0 && players[i]->ClanID == player->ClanID)
					hudMain->setCarSeatInfo(i, "clan");
				else
					hudMain->setCarSeatInfo(i, "filled");
			}

			hudMain->showCarInfo(true);
#ifndef FINAL_BUILD
		}
#endif
	}
	else
	{
		if (player->seatPosition == 0)
		{
			if (isActivePhysXVehicle)
			{
				g_pPhysicsWorld->m_VehicleManager->RemoveActiveVehicle(vd);
				isActivePhysXVehicle = false;

				SwitchToDrivable(false);
				PhysicsEnable(true);

				NetworkLocal = false;
				wasNetworkLocal = false;			
			}
		}

#ifndef FINAL_BUILD
		if (!g_bEditMode)
		{
#endif
			if (player->ClanID != 0 && player->ClanID == gClientLogic().localPlayer_->ClanID)
				hudMain->setCarSeatInfo(player->seatPosition, "clan");
			else if (player->GroupID != 0 && player->GroupID == gClientLogic().localPlayer_->GroupID)
				hudMain->setCarSeatInfo(player->seatPosition, "team");
			else 
				hudMain->setCarSeatInfo(player->seatPosition, "filled");
#ifndef FINAL_BUILD
		}
#endif
	}
}

void obj_Vehicle::ExitVehicle(obj_Player* player)
{
	if (!player || !GetPlayerById(player->GetNetworkID()))
		return;

	RemovePlayerById(player->GetNetworkID());
	
	if (player->NetworkLocal)
	{
		if (player->seatPosition == 0)
		{
			wasNetworkLocal = true;
			NetworkLocal = false;

			netVelocity = r3dPoint3D(0, 0, 0);
		}

		if (curFuel > 0 && !HasPlayers())
			PlayEngineStopSound();

#ifndef FINAL_BUILD
		if (!g_bEditMode)
#endif
			hudMain->showCarInfo(false);
	}
	else
	{
#ifndef FINAL_BUILD
		if (!g_bEditMode)
#endif
			hudMain->setCarSeatInfo(player->seatPosition, "empty");
	}
}

int obj_Vehicle::GetFuelAsPercent()
{
#ifndef FINAL_BUILD
	if (g_bEditMode) return 100;
#endif

	if (curFuel == 0 || maxFuel == 0)
		return 0;

	float curValue = (float)curFuel / (float)maxFuel * 100;
	return (int)curValue;
}

void obj_Vehicle::SetFuel(int amount)
{
	curFuel = amount;

	if (curFuel > 0)
		isCarDrivable = true;
	else
		OnRunOutOfGas();
}

bool obj_Vehicle::HasFuel()
{
#ifndef FINAL_BUILD
	if (g_bEditMode) return true;
#endif

	return curFuel > 0;
}

void obj_Vehicle::OnRunOutOfGas()
{
	if (!isCarDrivable)
		return;

	isCarDrivable = false; 

	PlayEngineSound(false);
	PlayEngineStopSound();	
}

bool obj_Vehicle::CanSmashBarricade()
{
	return GetSpeed() > 10.0f;
}

float obj_Vehicle::GetSpeed()
{
	if (NetworkLocal)
		return vd->GetSpeed();
	
	return reportedSpeed;
}

void obj_Vehicle::UpdateUI()
{
	obj_Player* plr = gClientLogic().localPlayer_;
	if(plr && plr->currentVehicleId == vehicleId)
	{
		if (hasExploded || plr->bDead)
		{
			hudMain->showCarInfo(false);
			return;
		}

		if(plr->IsInVehicle())
		{
			bool reportSpeed = false;
			if (r3dGetTime() > lastSpeedCheckTime + speedCheckWaitTime)
			{
				lastSpeedCheckTime = r3dGetTime();
				reportSpeed = true;
			}

			if( reportSpeed || durabilityChanged || fuelChanged )
			{
				if (CreateParams.vehicleType == VEHICLETYPE_HELICOPTER)
					hudMain->setCarInfo(GetDurabilityAsPercent(), 100 - (int)(HSpeed==2.0?0.0f:HSpeed), (int)(HSpeed==2.0?0.0f:HSpeed), GetFuelAsPercent(), 100 - (int)(HSpeed==2.0?0.0f:HSpeed));
				else
					hudMain->setCarInfo(GetDurabilityAsPercent(), 100 - GetSpeedAsPercent(), (int)GetSpeed(), GetFuelAsPercent(), 100 - GetRpmAsPercent());
			}
		}
	}
}

int obj_Vehicle::GetSpeedAsPercent()
{
	return (int)(GetSpeed() / vd->GetMaxSpeed() * 100);
}

int obj_Vehicle::GetRpmAsPercent()
{
	return (int)(GetEngineRpm() / 8000 * 100);
}

int obj_Vehicle::GetDurabilityAsPercent()
{
	return (int)((float)curDurability / (float)maxDurability * 100.0f);
}

void obj_Vehicle::EnableHeadlights(bool enabled)
{
	if (enabled)
	{
		headLight1.TurnOn();
		headLight2.TurnOn();
	}
	else 
	{
		headLight1.TurnOff();
		headLight2.TurnOff();
	}

	if (NetworkLocal)
		SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/NewWeapons/Melee/flashlight"), GetPosition());
}

void obj_Vehicle::EnablePoliceSiren(bool enabled)
{
	if (enabled)
	{
		SirenLeft.TurnOn();
		SirenRight.TurnOn();
		SirenLeftROT.TurnOn();
		SirenRightROT.TurnOn();
	}
	else 
	{
		SirenLeft.TurnOff();
		SirenRight.TurnOff();
		SirenLeftROT.TurnOff();
		SirenRightROT.TurnOff();
	}

	if (NetworkLocal)
		SoundSys.PlayAndForget(SoundSys.GetEventIDByPath("Sounds/NewWeapons/Melee/flashlight"), GetPosition());
}

void obj_Vehicle::TogglePoliceSiren()
{
	isPoliceSirenEnabled = !isPoliceSirenEnabled;

	EnablePoliceSiren(isPoliceSirenEnabled);
}

void obj_Vehicle::ToggleHeadlights()
{
	isHeadlightsEnabled = !isHeadlightsEnabled;

	if (NetworkLocal)
	{
		PKT_C2C_VehicleHeadlights_s n;
		n.isHeadlightsEnabled = isHeadlightsEnabled ;
		p2pSendToHost(this, &n, sizeof(n));
	}
}

void obj_Vehicle::StartTrackingPosition()
{
	startedAccelPosition = GetPosition();
}

bool obj_Vehicle::IsStuck()
{
	return isCurrentlyStuck;
}

void obj_Vehicle::SetIsMoving()
{
	isMoving = true;
	lastDriveTime = r3dGetTime();
}

void obj_Vehicle::ClearIsMoving()
{
	isMoving = false;
	lastDriveTime = 0;
}

float obj_Vehicle::GetMovedDistance()
{
	return (startedAccelPosition - GetPosition()).Length();
}

void obj_Vehicle::PlayEngineSound(bool isPlaying)
{
	if (!isSoundsLoaded)
		return;

	if (isPlaying)
	{
		if (!SoundSys.IsHandleValid(sndEngine))
		{
			sndEngine = SoundSys.Play(sndEngineId, GetPosition());

			float volume = 0.2f;
			SoundSys.SetProperty(sndEngine, FMOD_EVENTPROPERTY_VOLUME, &volume);

			if (isTank == true)
			{
				float volume2 = 1.0f;
				TreadSnd = SoundSys.Play(SoundSys.GetEventIDByPath("Sounds/Vehicles/TurretSound/Treadtank"),GetPosition());
				SoundSys.SetProperty(TreadSnd, FMOD_EVENTPROPERTY_VOLUME, &volume2);
			}
		}
	}
	else
	{
		if (SoundSys.IsHandleValid(sndEngine))
		{
			SoundSys.Stop(sndEngine);
			sndEngine = NULL;

			if (isTank == true)
			{
				SoundSys.Stop(TreadSnd);
				TreadSnd = NULL;
			}
		}
	}
}

void obj_Vehicle::PlayEngineStartSound()
{
	if (!isSoundsLoaded)
		return;

	if (sndEngineStartId != -1)
		SoundSys.PlayAndForget(sndEngineStartId, GetPosition());
}

void obj_Vehicle::PlayEngineStopSound()
{
	if (!isSoundsLoaded)
		return;

	if (sndEngineStartId != -1)
		SoundSys.PlayAndForget(sndEngineStopId, GetPosition());
}

void obj_Vehicle::PlayTractionSound(bool isPlaying, int soundId)
{
	if (!isSoundsLoaded)
		return;

	if (isPlaying)
	{
		if (!SoundSys.IsHandleValid(sndTraction))
			sndTraction = SoundSys.Play(sndTractionId, GetPosition());	
	}
	else
	{
		if (SoundSys.IsHandleValid(sndTraction))
		{
			SoundSys.Stop(sndTraction);
			sndTraction = NULL;
		}
	}
}

void obj_Vehicle::PlayTractionLoopSound(bool isPlaying)
{
	if (!isSoundsLoaded)
		return;

	if (isPlaying)
	{
		if (!SoundSys.IsHandleValid(sndTractionLoop))
			sndTractionLoop = SoundSys.Play(sndTractionLoopId, GetPosition());
	}
	else
	{
		if (SoundSys.IsHandleValid(sndTractionLoop))
		{
			SoundSys.Stop(sndTractionLoop);
			sndTractionLoop = NULL;
		}
	}
}

void obj_Vehicle::PlayExplosionSound()
{
	if (!isSoundsLoaded)
		return;

	if (sndExplosionId != -1)
		SoundSys.PlayAndForget(sndExplosionId, GetPosition());
}
#endif
