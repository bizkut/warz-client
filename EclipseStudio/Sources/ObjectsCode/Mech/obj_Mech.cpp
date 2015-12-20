#include "r3dPCH.h"
#include "r3d.h"

#ifndef FINAL_BUILD

#include "GameCommon.h"
#include "GameLevel.h"

#include "obj_Mech.h"
#include "obj_MechStates.h"
#include "../../Editors/ObjectManipulator3d.h"

#include "XMLHelpers.h"
#include "../../UI/HUD_Spectator.h"
#include <shlwapi.h>

//////////////////////////////////////////////////////////////////////////

namespace
{
	struct FileObjectsTree
	{
		stringlist_t folders;
		r3dgameVector (stringlist_t) files;
		void Clear()
		{
			folders.clear();
			files.clear();
		}
	};
	FileObjectsTree globalFileObjectsTree;

//////////////////////////////////////////////////////////////////////////

	void FillFileListFromDir(const char *path, stringlist_t &toFill, bool folders)
	{
		WIN32_FIND_DATA ffblk;
		HANDLE h = FindFirstFile(path, &ffblk);
		if(h != INVALID_HANDLE_VALUE) do 
		{
			bool isFolder = (ffblk.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
			bool doAdd = folders ? isFolder : !isFolder;

			if (doAdd)
			{
				doAdd &= strcmp(ffblk.cFileName, ".") != 0;
				doAdd &= strcmp(ffblk.cFileName, "..") != 0;
			}

			if (doAdd)
				toFill.push_back(ffblk.cFileName);

		} while(FindNextFile(h, &ffblk) != 0);
		FindClose(h);
	}

//////////////////////////////////////////////////////////////////////////

	void InitObjectsTree(const char *path, const char *ext, const char *intermediateFolder)
	{
		r3dSTLString s = r3dSTLString(path) + "*";
		FillFileListFromDir(s.c_str(), globalFileObjectsTree.folders, true);
		globalFileObjectsTree.files.resize(globalFileObjectsTree.folders.size());
		if (ext)
		{
			for (size_t i = 0; i < globalFileObjectsTree.files.size(); ++i)
			{
				s = path + globalFileObjectsTree.folders[i] + intermediateFolder + ext;
				FillFileListFromDir(s.c_str(), globalFileObjectsTree.files[i], false);
			}
		}
	}

//////////////////////////////////////////////////////////////////////////

	const float DEFAULT_CONTROLS_WIDTH = 360.0f;
	const float DEFAULT_CONTROLS_HEIGHT = 22.0f;

//////////////////////////////////////////////////////////////////////////

	/**	Weapon bones for different body types. */
	const char * const WEAPON_BONE_NAMES[][MAX_NUM_MECH_WEAPONS] =
	{
		//	Mark 1
		{ "MechWeapon1_1", "MechWeapon2_1", "", "", "", "" },
		{ "MechWeapon1_2", "MechWeapon2_2", "", "", "", "" },
		{ "MechWeapon1_3", "MechWeapon2_3", "MechWeapon3_3", "", "", "" },

		//	Mark 2
		{ "MechWeapon1_1", "MechWeapon2_1", "", "", "", "" },
		{ "MechWeapon1_2", "MechWeapon2_2", "MechWeapon3_2", "", "", "" },
		{ "MechWeapon1_3", "MechWeapon2_3", "MechWeapon3_3", "MechWeapon4_3", "", "" },

		//	Mark 3
		{ "MechWeapon1_1", "MechWeapon2_1", "MechWeapon3_1", "MechWeapon4_1", "", "" },
		{ "MechWeapon1_2", "MechWeapon2_2", "MechWeapon3_2", "MechWeapon4_2", "MechWeapon5_2", "" },
		{ "MechWeapon1_3", "MechWeapon2_3", "MechWeapon3_3", "MechWeapon4_3", "MechWeapon5_3", "MechWeapon6_3" }
	};

//////////////////////////////////////////////////////////////////////////

	float ConformAngle(float a, float pi = D3DX_PI)
	{
		if (a > pi) a = a - pi * 2;
		if (a < -pi) a = pi * 2 + a;
		return a;
	}
}

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_CLASS(obj_Mech, "obj_Mech", "Object");
AUTOREGISTER_CLASS(obj_Mech);

void _mech_AdjustBoneCallback(DWORD dwData, int boneId, D3DXMATRIX &mp, D3DXMATRIX &anim)
{
	obj_Mech* npc = (obj_Mech*)dwData;

	if(boneId == npc->boneId_MechPelvis) 
	{
		float xRot = npc->adjBody;
		float yRot = 0;

		D3DXMATRIX r1, r2;

		// rotate previos matrix, with saved position
		r3dPoint3D vv = r3dPoint3D(mp._41, mp._42, mp._43);
		D3DXMatrixRotationY(&r2, xRot);
		D3DXMatrixRotationX(&r1, -yRot);
		mp = mp * (r1 * r2);
		mp._41 = vv.x;
		mp._42 = vv.y;
		mp._43 = vv.z;
		return;
	}
	
	if
	(
		boneId == npc->boneId_Weapons[0] || boneId == npc->boneId_Weapons[1] || boneId == npc->boneId_Weapons[2] ||
		boneId == npc->boneId_Weapons[3] || boneId == npc->boneId_Weapons[4] || boneId == npc->boneId_Weapons[5]
	)
	{
		float yRot = 0;
		if(boneId == npc->boneId_Weapons[0]) yRot = npc->adjWeap1;
		if(boneId == npc->boneId_Weapons[1]) yRot = npc->adjWeap2;
		if(boneId == npc->boneId_Weapons[2]) yRot = npc->adjWeap3;
		if(boneId == npc->boneId_Weapons[3]) yRot = npc->adjWeap4;
		if(boneId == npc->boneId_Weapons[4]) yRot = npc->adjWeap5;
		if(boneId == npc->boneId_Weapons[5]) yRot = npc->adjWeap6;

		// rotate animation
		D3DXMATRIX r1;
		D3DXMatrixRotationZ(&r1, yRot);
		anim = r1 * anim;
		return;
	}
	return;    
}

obj_Mech::obj_Mech()
: animFSM(this)
, weaponsFireFSM(this)
{
	m_BindSkeleton = NULL;
	m_Animation = NULL;

	sMechSelected[0] = 0;
	sPresetName[0] = 0;
	fSkelListOffset = 0;

	sAnimSelected[0] = 0;
	fAnimListOffset = 0;

	adjBody = 0;
	adjWeap1 = 0;
	adjWeap2 = 0;
	adjWeap3 = 0;
	adjWeap4 = 0;
	adjWeap5 = 0;
	adjWeap6 = 0;

	for (int i = 0; i < _countof(boneId_Weapons); ++i)
	{
		boneId_Weapons[i] = -1;
	}
	
	m_renderSkel = 0;
	
	uberEquip_ = NULL;

	D3DXMatrixIdentity(&DrawMatrix);
	D3DXMatrixIdentity(&MoveMatrix);
	D3DXMatrixIdentity(&ViewMatrix);
	
	ViewAngle         = r3dPoint3D(0, 0, 0);
	m_fPlayerRotation = 0;
	currentTurretAngle = 0;
	forwardSpeed = 0;
	
	PlayerState       = PLAYER_IDLE;
	
	// crap-o-vars
	bDead             = 0;
	Height            = 9;
	MoveSpeed         = 10;
	MoveBackSpeed     = 8;
	MoveLimpSpeed     = 8;
	MoveBackLimpSpeed = 6;
	RunSpeed		  = 15;
	AnimWalkSpeed     = 10;
	
	AnimMode          = -1;

	drivable = 0;

	bOnGround = true;
	fHeightAboveGround = 0.0f;

	fallingVelocity = 0;

	curBodyPart = 0;
	curArmorPart = 0;
	mechType = r3dMechType_Light;

	rotationSpeed = 90.0f;

	turretRotSpeed = 90.0f;
	turretRotSpeedUpTime = 0;
	turretRotSlowDownTime = 0;
	turretStartRotTime = 0;
	turretRotDelayTime = 0;
	turretLimitAngle = 360.0f;

	cameraTargetOffset.Assign(0, 8, 0);
	crosshairOffset.Assign(0, 0);

	moveFwdAccel = 10.0f;
	moveBackAccel = 10.0f;
	runAccel = 20.0f;

	rotationTargetReached = false;
	
	horizontalOffset = 0;
	timeSinceStartMoving = 0;

	currentCameraPos.Assign(0, 0, 0);
	handleAnimationTeleport = false;
	usePivotAsCameraAnchor = true;

	cameraLockDistance = 0;

	ObjTypeFlags |= OBJTYPE_MechWarrior;

	mechEvents.Init(this);
}

obj_Mech::~obj_Mech()
{
	SAFE_DELETE(m_BindSkeleton);
	SAFE_DELETE(uberEquip_);
}

BOOL obj_Mech::Load(const char* fname)
{
	if(!parent::Load(fname))
		return FALSE;

	uberEquip_ = new CMechUberEquip();
	
	r3dBoundBox bbox;
	bbox.Size = r3dPoint3D(1, 1, 1);
	bbox.Org  = bbox.Size / -2;
	SetBBoxLocal(bbox);

	UpdateTransform();
	return TRUE;
}

BOOL obj_Mech::OnCreate()
{
	parent::OnCreate();

	NetworkLocal = 1;	//#MECH_TODO: right now all meches is local!
	if(NetworkLocal)
	{
//		ObjFlags |= OBJFLAG_SkipOcclusionCheck;	// otherwise when you are in scope mode you might not see your own player, and so scope will not render. Anyway, we should see our player always
		ObjFlags |= OBJFLAG_ForceSceneBoxBBox;

		PhysicsConfig.group = PHYSCOLL_CHARACTERCONTROLLER;
		PhysicsConfig.type  = PHYSICS_TYPE_CONTROLLER;
		PhysicsConfig.mass  = 100.0f;
		PhysicsConfig.contactOffset = 0.1f;
		PhysicsConfig.controllerRadius = 2.5f;
		PhysicsConfig.controllerHeight = 5.0f;
		PhysicsConfig.ready = true;
		PhysicsConfig.isFastMoving = true;
		PhysicsConfig.controllerHitCallback = true;
		PhysicsObject = BasePhysicsObject::CreateCharacterController(PhysicsConfig, this);

		PhysicsObject->AdjustControllerSize( PhysicsConfig.controllerRadius, PhysicsConfig.controllerHeight, PhysicsConfig.controllerHeight * 1.0f );
	}
	else
	{
		r3dError("todo");
	}
	
	return 1;
}

void obj_Mech::UpdateRotation()
{
	adjBody = R3D_DEG2RAD(-ViewAngle.x - m_fPlayerRotation);

	
	// view matrix
	D3DXMatrixRotationYawPitchRoll(&ViewMatrix, R3D_DEG2RAD(-ViewAngle.x), R3D_DEG2RAD(-ViewAngle.y), 0);

	// create move matrix (same as rotation)
	D3DXMatrixRotationYawPitchRoll(&MoveMatrix, R3D_DEG2RAD(m_fPlayerRotation), 0, 0);

	// draw matrix (model is rotated 180deg)
	const r3dPoint3D& pos = GetPosition();
	D3DXMatrixRotationYawPitchRoll(&DrawMatrix, R3D_DEG2RAD(m_fPlayerRotation + 180), 0, 0);
	DrawMatrix._41 += pos.x;
	DrawMatrix._42 += pos.y;
	DrawMatrix._43 += pos.z;

	// set physx rotation vectors
	SetRotationVector(r3dVector(m_fPlayerRotation, 0, 0));
}

void obj_Mech::SyncAnimation(bool force)
{
	R3DPROFILE_FUNCTION("obj_Mech::SyncAnimation");
	
	if(m_Animation == NULL)
		return;

	if(AnimMode == 0 && !m_Animation->AnimTracks.empty())
	{
		// adjust animation speed for walking mode based on speed
		r3dPoint3D vel = GetVelocity(); vel.y = 0;
		m_Animation->AnimTracks[0].fSpeed = vel.Length() / AnimWalkSpeed;
	}
}

BOOL obj_Mech::Update()
{
	R3DPROFILE_FUNCTION("obj_Mech::Update");
	if(!m_BindSkeleton)
		return parent::Update();

	if(!NetworkLocal) // for local it'll be done in UpdateLocalMovement
		parent::Update();

	if (drivable > 0)
	{
		ProcessMovement();
	}

	D3DXMATRIX mModelAdjust;
	D3DXMatrixRotationY(&mModelAdjust, 0); //R3D_PI);
	m_Animation->Update(r3dGetFrameTime(), r3dPoint3D(0, 0, 0), mModelAdjust);
	m_Animation->GetCurrentSkeleton();

	r3dVector diff = prevPelvisPos - GetPelvisPos();
	float l = diff.Length();
	if (handleAnimationTeleport && l > 1.0f)
	{
		SetPosition(GetPosition() + diff);
		handleAnimationTeleport = false;
		usePivotAsCameraAnchor = true;
	}

	for (int i = 0; i < _countof(wpnAnimations); ++i)
	{
		WeaponsAnimData &wad = wpnAnimations[i];
		if (wad.anim)
		{
			wad.anim->Update(r3dGetFrameTime(), r3dPoint3D(0, 0, 0), mModelAdjust);
			wad.anim->GetCurrentSkeleton();
		}
	}

	SyncAnimation();
	UpdateRotation();

	// update footsteps after moving player
	UpdateFootsteps();

	animFSM.Update();
	weaponsFireFSM.Update();

	UpdateCamera();

	if (Keyboard->IsPressed(kbsEsc) && drivable)
	{
		SwitchDriveModes(false);
		Keyboard->ClearPressed();
	}

	timeSinceStartMoving += r3dGetFrameTime();
	prevPelvisPos = GetPelvisPos();

	mechEvents.Update();

	return parent::Update();
}

//////////////////////////////////////////////////////////////////////////

// extern	PlayerStateVars_s TPSHudCameras[3][PLAYER_NUM_STATES];
// extern	Playerstate_e ActiveCameraRigID;
// extern	PlayerStateVars_s ActiveCameraRig;

extern 	Playerstate_e CurrentState;
extern 	PlayerStateVars_s CurrentRig;
extern 	PlayerStateVars_s SourceRig;
extern 	PlayerStateVars_s TargetRig;
extern 	float LerpValue;
extern	r3dPoint3D TPSHudCameraTarget;

void obj_Mech::ProcessMovement()
{
	R3DPROFILE_FUNCTION("obj_Mech::ProcessMovement");
	r3d_assert(NetworkLocal);

	// check fire weapon should be called all the time, as it will reset weapon fire in case if you are sitting on the menu, etc
	{
		R3DPROFILE_FUNCTION("update fire");
		//##MECH_TODO: CheckFireWeapon();
	}
	
	// query mouse distance, so it will not be accumulated
	int mMX=Mouse->m_MouseMoveX, mMY=Mouse->m_MouseMoveY;
	if(g_vertical_look->GetBool()) // invert mouse
		mMY = -mMY;

	int   playerState = PLAYER_IDLE;

	bool moving = false;
	float accel = moveFwdAccel * r3dGetFrameTime();;

	bool running = Keyboard->IsPressed(kbsLeftShift);
	bool altPressed = Keyboard->IsPressed(kbsLeftAlt) || Keyboard->IsPressed(kbsRightAlt);
	bool ctrlPressed = Keyboard->IsPressed(kbsLeftControl) || Keyboard->IsPressed(kbsRightControl);

	MechAnimationsFSM::WalkRun * walkAnimsArr[] =
	{
		&MechAnimationsFSM::gStateWalkForward,
		&MechAnimationsFSM::gStateWalkForwardLimpRight,
		&MechAnimationsFSM::gStateWalkForwardLimpLeft
	};

	MechAnimationsFSM::WalkBack * walkBackAnimsArr[] =
	{
		&MechAnimationsFSM::gStateWalkBackward,
		&MechAnimationsFSM::gStateWalkBackwardLimpRight,
		&MechAnimationsFSM::gStateWalkBackwardLimpLeft
	};

	int walkIdx = 0;
	if (altPressed) walkIdx = 2;
	else if (ctrlPressed) walkIdx = 1;

	float maxSpeed = walkIdx > 0 ? MoveLimpSpeed : MoveSpeed;
	MechAnimationsFSM::WalkRun *moveState = walkAnimsArr[walkIdx];

	if (running)
	{
		accel = runAccel * r3dGetFrameTime();;
		moveState = &MechAnimationsFSM::gStateRun;
		maxSpeed = RunSpeed;
	}

	bool bumping = animFSM.IsInState(MechAnimationsFSM::gStateBumpFront) || animFSM.IsInState(MechAnimationsFSM::gStateBumpBack);
	if (!bumping)
	{
		if (InputMappingMngr->isPressed(r3dInputMappingMngr::KS_MOVE_FORWARD) && forwardSpeed >= 0) 
		{
			animFSM.ChangeState(moveState);
			moving = true;
		}
		else if(InputMappingMngr->isPressed(r3dInputMappingMngr::KS_MOVE_BACKWARD) && forwardSpeed <= 0)
		{
			accel = moveBackAccel * -r3dGetFrameTime();
			animFSM.ChangeState(walkBackAnimsArr[walkIdx]);
			moving = true;
		}
		else
		{
			if (forwardSpeed > 0)
			{
				if (!running)
				{
					animFSM.ChangeState(moveState);
				}
				accel *= -1; 
			}
			else if (forwardSpeed == 0)
			{
				accel = 0;
			}
		}

		float prevFwdSpeed = forwardSpeed;
		float backSpeed = walkIdx > 0 ? MoveBackLimpSpeed : MoveBackSpeed;
		forwardSpeed = R3D_CLAMP(forwardSpeed + accel, -backSpeed, maxSpeed);
		if ((prevFwdSpeed > 0 && forwardSpeed < 0) || (prevFwdSpeed < 0 && forwardSpeed > 0))
			forwardSpeed = 0;
	}


	if (InputMappingMngr->isPressed(r3dInputMappingMngr::KS_MOVE_LEFT)) 
	{
		m_fPlayerRotation -= rotationSpeed * r3dGetFrameTime();
		m_fPlayerRotation = ConformAngle(m_fPlayerRotation, 180.0f);
		if (forwardSpeed == 0)
			animFSM.ChangeState(&MechAnimationsFSM::gStateRotate90Left);
		moving = true;
	}
	else if(InputMappingMngr->isPressed(r3dInputMappingMngr::KS_MOVE_RIGHT)) 
	{
		m_fPlayerRotation += rotationSpeed * r3dGetFrameTime();
		m_fPlayerRotation = ConformAngle(m_fPlayerRotation, 180.0f);
		if (forwardSpeed == 0)
			animFSM.ChangeState(&MechAnimationsFSM::gStateRotate90Right);
		moving = true;
	}

	//	Check for obstacles
	if (forwardSpeed != 0)
	{
		PxRaycastHit hit;
		r3dVector f = GetMoveForw() * static_cast<float>(R3D_SIGN(forwardSpeed));
		PxVec3 fwdVec(f.x, f.y, f.z);
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC));
		if(g_pPhysicsWorld->raycastSingle(PxVec3(GetPosition().x, GetPosition().y + 4.0f, GetPosition().z), fwdVec, 3.0f, PxSceneQueryFlags(PxSceneQueryFlag::eDISTANCE), hit, filter))
		{
			if (hit.distance < 1.6f)
			{
				animFSM.ChangeState(forwardSpeed > 0 ? &MechAnimationsFSM::gStateBumpFront : &MechAnimationsFSM::gStateBumpBack);
				moving = true;
				forwardSpeed = 0;
			}
		}
	}

	bool isFalling = animFSM.IsInState(MechAnimationsFSM::gStateLandingLong) || animFSM.IsInState(MechAnimationsFSM::gStateLandingShort) || !bOnGround;

	if (!moving && !isFalling && !bumping && forwardSpeed == 0)
	{
		animFSM.ChangeState(&MechAnimationsFSM::gStateMovementIdle);
		timeSinceStartMoving = 0;
	}

	//	Weapons fire handling
	if (Mouse->IsPressed(r3dMouse::mLeftButton))
	{
		weaponsFireFSM.ChangeState(&MechAnimationsFSM::gStateFireMachinegun);
	}
	else if (Mouse->IsPressed(r3dMouse::mRightButton))
	{
		weaponsFireFSM.ChangeState(&MechAnimationsFSM::gStateFireSniper);
	}
	else
	{
		weaponsFireFSM.ChangeState(&MechAnimationsFSM::gStateIdle);
	}


//  	ActiveCameraRigID = (Playerstate_e)PlayerState;
//  	ActiveCameraRig   = TPSHudCameras[0][ActiveCameraRigID];
// 
// 	//set new target
//  	SourceRig = CurrentRig;
//  	TargetRig = ActiveCameraRig;
//  	LerpValue = 0;
// 	CurrentState = (Playerstate_e)PlayerState;	

	{
		UpdateTurretRotation();

		r3dPoint2D xz(gCam.vPointTo.x, gCam.vPointTo.z);
		float l = xz.Length();

		float aY = atan2f(l, gCam.vPointTo.y);
		aY = R3D_RAD2DEG(aY) - 90.0f;

		ViewAngle.y = -aY;

		if(ViewAngle.x > 360.0f) ViewAngle.x = ViewAngle.x - 360.0f;
		if(ViewAngle.x < 0.0f)   ViewAngle.x = ViewAngle.x + 360.0f;

		// Player can't look too high!
		if(ViewAngle.y > CurrentRig.LookUpLimit )  ViewAngle.y = CurrentRig.LookUpLimit;
		if(ViewAngle.y < CurrentRig.LookDownLimit) ViewAngle.y = CurrentRig.LookDownLimit;

		// limit view angle & set body adjust
		/*
		if(ViewAngle.x >  179.0f) ViewAngle.x = 179;
		if(ViewAngle.x < -179.0f) ViewAngle.x = -179;
		*/

		adjWeap1 = R3D_DEG2RAD(+ViewAngle.y);
		adjWeap2 = R3D_DEG2RAD(-ViewAngle.y);
	}

	UpdateLocalMovement();
}

//////////////////////////////////////////////////////////////////////////

float obj_Mech::CalculateCurrentTurretRotSpeed(float targetAngle)
{
	float radSpd = R3D_DEG2RAD(turretRotSpeed);

	float curTime = r3dGetTime();
	float dt = curTime - turretStartRotTime;

	if (dt < turretRotDelayTime)
		return 0;

	dt -= turretRotDelayTime;

	float speedup = radSpd;
	if (turretRotSpeedUpTime > 0)
	{

		dt = dt / turretRotSpeedUpTime;
		dt = R3D_CLAMP(dt, 0.0f, 1.0f);

		speedup = R3D_LERP(0.0f, radSpd, dt);
	}

	float slowdown = radSpd;
	if (turretRotSlowDownTime > 0)
	{
		dt = std::abs(targetAngle - currentTurretAngle) / speedup / turretRotSlowDownTime;
		dt = R3D_CLAMP(dt, 0.0f, 1.0f);
		slowdown = R3D_LERP(speedup, 0.0f, dt);
	}

	float spd = std::min(slowdown, speedup);

	return speedup;
}

//////////////////////////////////////////////////////////////////////////

float obj_Mech::LimitTargetAngle(float targetAngle, float curTurretAngle) const
{
	float radLimitAngle = R3D_DEG2RAD(turretLimitAngle);

	//	Disable limiting if no limit angle specified
	if (radLimitAngle > D3DX_PI * 2 - 0.1f)
		return targetAngle;

	float radPl = R3D_DEG2RAD(m_fPlayerRotation);
	float minAngle = ConformAngle(-radLimitAngle / 2);
	float maxAngle = ConformAngle(radLimitAngle / 2);

	targetAngle = ConformAngle(targetAngle + radPl);
	curTurretAngle = ConformAngle(curTurretAngle + radPl);

	if (targetAngle < 0)
		targetAngle = std::max(minAngle, targetAngle);
	else
		targetAngle = std::min(maxAngle, targetAngle);

	float rv = ConformAngle(targetAngle - radPl);

	if (std::abs(curTurretAngle - targetAngle) > D3DX_PI)
	{
		if (curTurretAngle > 0 && targetAngle < 0 || curTurretAngle < 0 && targetAngle > 0)
		{
			rv = -radPl;
		}
	}
	return rv;
}

//////////////////////////////////////////////////////////////////////////

bool obj_Mech::CalcTurretRotDir(float curAngle, float targetAngle) const
{
	bool increasingAngle = false;

	if (currentTurretAngle >= 0 && targetAngle >=0 )
	{
		if (currentTurretAngle < targetAngle)
			increasingAngle = true;
	}
	else if (currentTurretAngle < 0 && targetAngle < 0)
	{
		if (currentTurretAngle < targetAngle)
			increasingAngle = true;
	}
	else
	{
		if (std::abs(currentTurretAngle - targetAngle) < D3DX_PI)
		{
			if (currentTurretAngle < 0)
				increasingAngle = true;
		}
		else
		{
			if (currentTurretAngle > 0)
				increasingAngle = true;
		}
	}
	return increasingAngle;
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::UpdateTurretRotation()
{
	float targetAngle = -atan2f(gCam.vPointTo.x, gCam.vPointTo.z);

	targetAngle = LimitTargetAngle(targetAngle, currentTurretAngle);

	currentTurretAngle = ConformAngle(currentTurretAngle);

	bool increasingAngle = CalcTurretRotDir(currentTurretAngle, targetAngle);

// 	if (!rotationTargetReached && prevRotationIncrease != increasingAngle)
// 	{
// 		increasingAngle = prevRotationIncrease;
// 	}
// 	else
// 	{
// 		prevRotationIncrease = increasingAngle;
// 	}
	
	//	Calculate angle of rotation for this frame
	float curSpeed = CalculateCurrentTurretRotSpeed(targetAngle);
	float angleInc = r3dGetFrameTime() * curSpeed;

	if (std::abs(currentTurretAngle - targetAngle) < angleInc)
	{
		currentTurretAngle = targetAngle;
		turretStartRotTime = r3dGetTime();
		rotationTargetReached = true;
	}
	else
	{ 
		rotationTargetReached = false;
		if (increasingAngle)
			currentTurretAngle += angleInc;
		else 
			currentTurretAngle -= angleInc;
	}

	currentTurretAngle = ConformAngle(currentTurretAngle);

	ViewAngle.x = R3D_RAD2DEG(currentTurretAngle);
}

//////////////////////////////////////////////////////////////////////////
BaseHUD* editor_GetHudByIndex(int index);
extern int CurHUDID;
void obj_Mech::UpdateCamera()
{
	//	Calculate camera shaking
	MechAnimationsID curMoveAnim = MECH_ANIM_COUNT;
	if (animFSM.IsInState(MechAnimationsFSM::gStateRun))
	{
		curMoveAnim = MECH_ANIM_RUN;
	}
	else if (animFSM.IsInState(MechAnimationsFSM::gStateWalkForward))
	{
		curMoveAnim = MECH_ANIM_WALK;
	}
	else if (animFSM.IsInState(MechAnimationsFSM::gStateWalkBackward))
	{
		curMoveAnim = MECH_ANIM_WALK_BACK;
	}

	float animSpeed = 0;
	if (curMoveAnim != MECH_ANIM_COUNT)
	{
		r3dAnimation::r3dAnimInfo *ai = m_Animation->GetTrack(additionalAnimData[curMoveAnim].animTrackId);
		if (ai)
		{
			animSpeed = ai->fFPS / ai->pAnim->NumFrames;
		}
	}

	r3dVector shakeVec = GetPelvisPos() - r3dVector(DrawMatrix._41, DrawMatrix._42, DrawMatrix._43);
	shakeVec.x = shakeVec.z  = 0;

	//	Calculate camera movement
/*
	if (doSmoothCameraTransition)
	{
		r3dVector v = GetPelvisPos();
		v.y = GetPosition().y;

		r3dVector camDir = v - currentCameraPos;
		currentCameraPos += camDir * r3dGetFrameTime() * 5;
		if (camDir.Length() < 0.1f)
			doSmoothCameraTransition = false;
	}
*/
	if (usePivotAsCameraAnchor)
	{
		currentCameraPos = GetPosition();
	}
	else
	{
		currentCameraPos = GetPelvisPos();
		currentCameraPos.y = GetPosition().y;
	}

	if (drivable)
	{
		CameraHUD *h = static_cast<CameraHUD*>(editor_GetHudByIndex(CurHUDID));
		h->TPS_Target = currentCameraPos + cameraTargetOffset + shakeVec;
		h->TPS_SideOffset = horizontalOffset;
		h->TPS_LockedDistance = cameraLockDistance;
	}
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::ReinitializeMech(const char *path)
{
	sPresetName[0] = 0;

	const char *mechName = strrchr(path, '\\');
	r3d_assert(mechName);
	mechName += 1;

	if (stristr(mechName, "light") != 0)
		mechType = r3dMechType_Light;
	else if (stristr(mechName, "medium") != 0)
		mechType = r3dMechType_Medium;
	else if (stristr(mechName, "heavy") != 0)
		mechType = r3dMechType_Heavy;

	uberEquip_->Reset();
	uberEquip_->SetWeaponNames(WEAPON_BONE_NAMES[curBodyPart + mechType * 3]);
	SAFE_DELETE(m_BindSkeleton);

	r3dSTLString p = r3dSTLString(path) + "\\" + mechName + ".skl";

	m_BindSkeleton = new r3dSkeleton();
	m_BindSkeleton->LoadBinary(p.c_str());
	extern void r3dDumpSkeleton(const r3dSkeleton* skel, int bone);
	r3dDumpSkeleton(m_BindSkeleton, -1);

	boneId_MechPelvis = m_BindSkeleton->GetBoneID("Mech_Head");
	if (boneId_MechPelvis < 0)
	{
		boneId_MechPelvis = m_BindSkeleton->GetBoneID("MechHead");
	}

	if (boneId_MechPelvis < 0)
	{
		r3d_assert(!"Cannot find 'Mech_Head' or 'MechHead' bone");
	}

	InitWeaponBones();

	SAFE_DELETE(m_Animation);
	m_AnimPool.Unload();

	m_Animation = game_new r3dAnimation();
	m_Animation->Init(m_BindSkeleton, &m_AnimPool, _mech_AdjustBoneCallback, (DWORD)this);

	//	Init other skeleton parts
	p = r3dSTLString(path) + "\\" + mechName + "_legs.sco";
	
	r3d_assert(r3d_access(p.c_str(), 0) == 0);
	uberEquip_->LoadSlot(MSLOT_Mech1, p.c_str());

	armorPaths[0] = "@EMPTY";
	armorPaths[1] = r3dSTLString(path) + "\\" + mechName + "_ArmorLight.sco";
	armorPaths[2] = r3dSTLString(path) + "\\" + mechName + "_ArmorMedium.sco";
	armorPaths[3] = r3dSTLString(path) + "\\" + mechName + "_ArmorHeavy.sco";

	bodyPaths[0] = r3dSTLString(path) + "\\" + mechName + "_Mark1.sco";
	bodyPaths[1] = r3dSTLString(path) + "\\" + mechName + "_Mark2.sco";
	bodyPaths[2] = r3dSTLString(path) + "\\" + mechName + "_Mark3.sco";

	//	Skip empty armor type
	for (int i = 1; i < NUM_MECH_ARMORS; ++i)
	{
		bool haveArmor = r3d_access(armorPaths[i].c_str(), 0) == 0;
		if (!haveArmor)
		{
			r3dError("Cannot find armor file: %s", armorPaths[i].c_str());
		}
	}

	for (int i = 0; i < NUM_MECH_BODIES; ++i)
	{
		bool haveBody = r3d_access(bodyPaths[i].c_str(), 0) == 0;
		if (!haveBody)
		{
			r3dError("Cannot find body file: %s", bodyPaths[i].c_str());
		}
	}

	uberEquip_->LoadSlot(MSLOT_Mech2, armorPaths[curArmorPart].c_str());
	uberEquip_->LoadSlot(MSLOT_Mech3, bodyPaths[curBodyPart].c_str());

	InitWeaponBones();

	SetBBoxLocal(uberEquip_->GetBoundingBox());
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::InitWeaponBones()
{
	//	Init weapon bones
	int numWeaponSlots = _countof(WEAPON_BONE_NAMES[curBodyPart]);
	for (int i = 0; i < _countof(boneId_Weapons); ++i)
	{
		boneId_Weapons[i] = m_BindSkeleton->GetBoneID(WEAPON_BONE_NAMES[curBodyPart + mechType * 3][i]);
	}
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::InitDefaultAnimations(const char *path)
{
	static const char * const animSuffixes[_countof(MECH_ANIMATION_LIST)] = 
	{
		"_walk.anm",
		"_Walk_Back.anm",
		"_Run.anm",
		"_Walk_Bump_Forw.anm",
		"_Walk_Bump_Back.anm",
		"_Run_Bump_Forw.anm",
		"_Falling_Short_Bump.anm",
		"_Falling_Loop.anm",
		"_Falling_Long_Bump.anm",
		"_Overheat_Down.anm",
		"_Overheat_Up.anm",
		"_Damage_Front.anm",
		"_Damage_Left.anm",
		"_Damage_Back.anm",
		"_Damage_Right.anm",
		"_Destroy.anm",
		"_Launch_Grenade_Top.anm",
		"_Launch_Machinegune_Top.anm",
		"_Launch_Sniper_Top.anm",
		"_Idle.anm",
		"_Loop_Turn_90_Left.anm",
		"_Loop_Turn_90_Right.anm",
		"_Walk_Limp_Right.anm",
		"_Walk_Limp_Left.anm",
		"_Walk_Back_Limp_Right.anm",
		"_Walk_Back_Limp_Left.anm"
	};

	const char *mechName = strrchr(path, '\\');
	r3d_assert(mechName);
	mechName += 1;

	r3dSTLString p;
	for (int i = 0; i < _countof(MECH_ANIMATION_LIST); ++i)
	{
		if (animSuffixes[i])
		{
			p = r3dSTLString(path) + "\\Animations\\" + mechName + animSuffixes[i];
			if (r3d_access(p.c_str(), 0) == 0)
			{
				AdditionalAnimationData &ad = additionalAnimData[i];
				ad.animIndexRemap = m_AnimPool.Add(MECH_ANIMATION_LIST[i], p.c_str(), 0);
				ad.onlyUpperBody = false;

				r3dAnimData *animData = m_AnimPool.Get(ad.animIndexRemap);
				ad.endFrame = animData->NumFrames;
				ad.totalFrames = animData->NumFrames;
			}
			else
			{
				additionalAnimData[i].animIndexRemap = -1;
			}
		}
	}

	SetDefaultPose();
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::SwitchUpperBodyAnimation(bool onlyUpperBody, r3dAnimData *ad)
{
 	ad->BipedSetEnabled(!onlyUpperBody);
 
 	if (!onlyUpperBody)
 		return;

	SwitchUpperBodyAnimationRecursive(onlyUpperBody, boneId_MechPelvis, ad);
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::SwitchLowerBodyAnimation(bool onlyLowerBody, r3dAnimData *ad)
{
	ad->BipedSetEnabled(!onlyLowerBody);

	if (!onlyLowerBody)
		return;

	int llegId = m_BindSkeleton->GetBoneID("Mech_LLeg1");
	int rlegId = m_BindSkeleton->GetBoneID("Mech_RLeg1");

	if (llegId < 0)
	{
		r3d_assert(!"Cannot find 'Mech_LLeg1' bone");
	}

	if (rlegId < 0)
	{
		r3d_assert(!"Cannot find 'Mech_RLeg1' bone");
	}

	SwitchUpperBodyAnimationRecursive(onlyLowerBody, llegId, ad);
	SwitchUpperBodyAnimationRecursive(onlyLowerBody, rlegId, ad);
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::SwitchUpperBodyAnimationRecursive(bool onlyUpperBody, int rootBoneID, r3dAnimData *ad)
{
	if (!m_BindSkeleton || !ad)
		return;

	const char* name = m_BindSkeleton->GetBoneName(rootBoneID);
	ad->EnableTrack(name, onlyUpperBody);

	for (int i = 0; i < m_BindSkeleton->NumBones; ++i)
	{
		int curId = m_BindSkeleton->GetParentBoneId(i);
		if (curId == rootBoneID)
		{
			SwitchUpperBodyAnimationRecursive(onlyUpperBody, i, ad);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

bool obj_Mech::DrawPossibleElementsChoose(const char *rootDir, const char *ext, const char *forceFolder, const char *intermediateFolder, r3dSTLString &outPath)
{
#ifndef FINAL_BUILD

	float SliderX = r3dRenderer->ScreenW-375-375;
	float SliderY = 100;

	//
	static char FolderName[64] = "";
	static char FileName[64] = "";
	static char Str[MAX_PATH] = "";
	static int idx = -1;
	static float CatOffset = -1;
	static int idx1 = -1;
	static float CatOffset2 = -1;

	if (globalFileObjectsTree.folders.empty())
	{
		idx = -1;
		CatOffset = -1;
		idx1 = -1;
		CatOffset2 = -1;
		InitObjectsTree(rootDir, ext, intermediateFolder);
		ZeroMemory(FolderName, _countof(FolderName));
		ZeroMemory(Str, _countof(Str));
		ZeroMemory(FileName, _countof(FileName));
	}

	if (!forceFolder)
	{
		SliderY += imgui_Static(SliderX, SliderY, FolderName, 200);

		if (imgui_DrawList(SliderX, SliderY, 360, 200, globalFileObjectsTree.folders, &CatOffset, &idx))
		{
			sprintf(FolderName, "%s", globalFileObjectsTree.folders.at(idx).c_str());
		}
		SliderY += 210;
	}
	else
	{
		for (size_t i = 0; i < globalFileObjectsTree.folders.size(); ++i)
		{
			if (0 == strcmpi(globalFileObjectsTree.folders[i].c_str(), forceFolder))
			{
				idx = i;
				break;
			}
		}
		if (idx == -1) return false;

		sprintf(FolderName, "%s", forceFolder);

	}

	bool rv = true;
	SliderY += imgui_Static(SliderX, SliderY, FileName);

	if (idx != -1)
	{
		if (ext == 0)
		{
			if(imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH / 2, DEFAULT_CONTROLS_HEIGHT, "Select"))
			{
				r3dSTLString s = r3dSTLString(rootDir) + "%s";
				sprintf (Str, s.c_str(), FolderName);
				outPath = Str;
				rv = false;
			}
		}
		else
		{
			if (imgui_DrawList(SliderX, SliderY, 360, 200, globalFileObjectsTree.files[idx] , &CatOffset2, &idx1))
			{
				sprintf (FileName,"%s", globalFileObjectsTree.files[idx][idx1].c_str());
			}
			SliderY += 205;

			r3dSTLString s = r3dSTLString(rootDir) + "%s%s%s";
			sprintf (Str, s.c_str(), FolderName, intermediateFolder, FileName);

			if (idx1 >= 0)
			{
				if(imgui_Button(SliderX, SliderY, DEFAULT_CONTROLS_WIDTH / 2, DEFAULT_CONTROLS_HEIGHT, "Select"))
				{
					outPath = Str;
					rv = false;
				}
			}
		}
	}

	if (imgui_Button(SliderX + DEFAULT_CONTROLS_WIDTH / 2, SliderY, DEFAULT_CONTROLS_WIDTH / 2, DEFAULT_CONTROLS_HEIGHT, "Cancel"))
	{
		rv = false;
		outPath.clear();
	}
#endif
	return rv;
}

//////////////////////////////////////////////////////////////////////////

float obj_Mech::DrawMeshesConfigEditor(float scrx, float scry, float scrw, float scrh)
{
	float starty = scry;

	char *str = 0;
	if (strlen(sMechSelected) > 0)
		str = strrchr(sMechSelected, '\\') + 1;

	// skeleton change
	char buf[256] = {0};
	sprintf_s(buf, _countof(buf), "Mech: %s", str);
	int off = imgui_Static(scrx, starty, buf, 300);
	static bool pickSkeleton = false;
	if (imgui_Button(scrx + 305, starty + 2, 50, 22.0f, "Pick"))
	{
		globalFileObjectsTree.Clear();
		pickSkeleton = true;
	}

	starty += off;
	starty += imgui_Checkbox(scrx, starty, "Show Skeleton", &m_renderSkel, 1);

	if (pickSkeleton)
	{
		static r3dSTLString s;
		g_pDesktopManager->End();
		pickSkeleton = DrawPossibleElementsChoose("data\\objectsdepot\\mechs\\", 0, 0, "\\", s);
		g_pDesktopManager->Begin("ed_objects");

		if (!pickSkeleton && !s.empty())
		{
			ReinitializeMech(s.c_str());
			InitDefaultAnimations(s.c_str());
			strcpy(sMechSelected, s.c_str());
		}
	}

	starty += 22.0f;

	if(m_BindSkeleton == NULL)
		return starty - scry ;

	static int pickSlot = -1;

	//	Show body parts
	static const char* list[] = { "Mark 1", "Mark 2", "Mark 3" };
	int prevVal = curBodyPart;
	starty += imgui_Static(scrx, starty, "Body type:");
	starty += imgui_Toolbar(scrx, starty, 360.0f, 30.0f, &curBodyPart, 0, list, _countof(list));
	if (prevVal != curBodyPart)
	{
		uberEquip_->LoadSlot(MSLOT_Mech3, bodyPaths[curBodyPart].c_str());
		InitWeaponBones();
		uberEquip_->SetWeaponNames(WEAPON_BONE_NAMES[curBodyPart + mechType * 3]);

		for (int i = 0; i < MAX_NUM_MECH_WEAPONS; ++i)
		{
			if (boneId_Weapons[i] == -1)
				UnloadWeaponSlot(static_cast<EMechSlot>(i + MSLOT_Weapon1));
		}
	}

	//	Show armor parts
	static const char* list2[] = { "None", "Light", "Medium", "Heavy" };
	prevVal = curArmorPart;
	starty += imgui_Static(scrx, starty, "Armor type:");
	starty += imgui_Toolbar(scrx, starty, 360.0f, 30.0f, &curArmorPart, 0, list2, _countof(list2));
	if (prevVal != curArmorPart)
	{
		uberEquip_->LoadSlot(MSLOT_Mech2, armorPaths[curArmorPart].c_str());
	}

	starty += imgui_Value_Slider(scrx, starty, "Walk speed", &MoveSpeed, 0.0f, 30.0f, "%-02.3f");
	starty += imgui_Value_Slider(scrx, starty, "Walk back speed", &MoveBackSpeed, 0.0f, 30.0f, "%-02.3f");
	starty += imgui_Value_Slider(scrx, starty, "Walk limp speed", &MoveLimpSpeed, 0.0f, 30.0f, "%-02.3f");
	starty += imgui_Value_Slider(scrx, starty, "Walk back limp speed", &MoveBackLimpSpeed, 0.0f, 30.0f, "%-02.3f");
	starty += imgui_Value_Slider(scrx, starty, "Run speed", &RunSpeed, 0.0f, 30.0f, "%-02.3f");
	starty += imgui_Value_Slider(scrx, starty, "Rotation speed", &rotationSpeed, 0.0f, 300.0f, "%-02.3f");

	starty += 5.0f;
	starty += imgui_Value_Slider(scrx, starty, "Walk acceleration", &moveFwdAccel, 1.0f, 100.0f, "%-02.3f");
	starty += imgui_Value_Slider(scrx, starty, "Walk back acceleration", &moveBackAccel, 1.0f, 100.0f, "%-02.3f");
	starty += imgui_Value_Slider(scrx, starty, "Run acceleration", &runAccel, 1.0f, 100.0f, "%-02.3f");

	starty += 22.0f;

	for (int i = MSLOT_Weapon1, k = 0; i < MSLOT_Max; i++, ++k) 
	{
		if (boneId_Weapons[k] == -1)
			break;

		bool emptyMesh = uberEquip_->slots_[i].mesh == NULL;
		char buf[265];
		sprintf
		(
			buf, "%s%d : %s", "Wpn", (i - MSLOT_Weapon1 + 1),
			emptyMesh ? "empty" : uberEquip_->slots_[i].name.c_str()
		);
		int off = imgui_Static(scrx, starty, buf, 300);

		const char * btnCaption = emptyMesh ? "Pick" : "Del";
			
		if (imgui_Button(scrx + 305, starty + 2, 50, 22.0f, btnCaption))
		{
			if (!emptyMesh)
			{
				uberEquip_->slots_[i].mesh = 0;
				uberEquip_->slots_[i].name = "";
			}
			else
			{
				globalFileObjectsTree.Clear();
				pickSlot = i;
			}
		}

		starty += off;

		if (!emptyMesh)
		{
			const static char* list[] = { "Missile", "Sniper", "Grenade", "Mortar", "M-Gun" };
			starty += imgui_Toolbar(scrx, starty, DEFAULT_CONTROLS_WIDTH, DEFAULT_CONTROLS_HEIGHT, &wpnAnimations[k].wpnType, 0, list, _countof(list)) + 3;
		}
	}

	if (pickSlot > -1)
	{
		static r3dSTLString s;
		g_pDesktopManager->End();
		bool isPicked = !DrawPossibleElementsChoose("data\\objectsdepot\\", "*.sco", "weapons", "\\", s);
		g_pDesktopManager->Begin("ed_objects");

		if (isPicked)
		{
			if (!s.empty())
			{
				LoadWeaponSlot(static_cast<EMechSlot>(pickSlot), s.c_str());
			}
			pickSlot = -1;
		}
	}
	starty += 22.0f;

	//	Show turret rotation parameters
	starty += imgui_Static(scrx, starty, "Turret rotation parameters:");
	starty += imgui_Value_Slider(scrx, starty, "Rot speed", &turretRotSpeed, 0, D3DX_PI * 180 * 4, "%-02.3f");
	starty += imgui_Value_Slider(scrx, starty, "Speedup time", &turretRotSpeedUpTime, 0, 2.0f, "%-02.3f");
//	starty += imgui_Value_Slider(scrx, starty, "Slowdown time", &turretRotSlowDownTime, 0, 2.0f, "%-02.3f");
	starty += imgui_Value_Slider(scrx, starty, "Rot delay time", &turretRotDelayTime, 0, 1.0f, "%-02.3f");
	starty += imgui_Value_Slider(scrx, starty, "Limit angle", &turretLimitAngle, 0, 360.0f, "%-02.3f");

	return starty - scry ;
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::RemoveAnimation(int i)
{
	if (m_Animation)
		m_Animation->StopAll();

	int idx = additionalAnimData[i].animIndexRemap;
	additionalAnimData[i].animIndexRemap = -1;

	for (int k = 0; k < _countof(additionalAnimData); ++k)
	{
		if (additionalAnimData[k].animIndexRemap > idx)
			--additionalAnimData[k].animIndexRemap;
	}

	m_AnimPool.Anims[idx]->pAnimPool = NULL;
	delete m_AnimPool.Anims[idx];
	m_AnimPool.Anims[idx] = 0;
	m_AnimPool.Anims.erase(m_AnimPool.Anims.begin() + idx);
	m_Animation->AnimTracks.clear();
}

//////////////////////////////////////////////////////////////////////////

float obj_Mech::DrawAnimationsConfigEditor(float scrx, float scry, float scrw, float scrh)
{
	if( !m_Animation )
		return scry;

	float starty = scry;

	static bool isExpanded[_countof(MECH_ANIMATION_LIST)] = {false};

	float scrxOff = scrx + 22.0f;

	static int pickAnim = -1;
	static char buf[MAX_PATH] = {0};

	for (int i = 0; i < _countof(MECH_ANIMATION_LIST); ++i)
	{
		int off = imgui_Static(scrxOff, starty, MECH_ANIMATION_LIST[i], 300);

		AdditionalAnimationData &aad = additionalAnimData[i];
		r3dAnimation::r3dAnimInfo *ai = m_Animation->GetTrack(aad.animTrackId);
		const char *playOrStop = "Play";
		if (ai && ai->dwStatus & ANIMSTATUS_Playing)
		{
			playOrStop = "Stop";
		}

		if (aad.animIndexRemap != -1 && m_Animation)
		{
			if (imgui_Button(scrxOff + 305, starty + 2, 30, 22.0f, playOrStop))
			{
				playOrStop[0] == 'P' ?
					MechAnimationsFSM::StartAnimationHelper(this, static_cast<MechAnimationsID>(i), ANIMFLAG_RemoveOtherNow | ANIMFLAG_Looped) :
					m_Animation->StopAll();
			}
		}

		const char *expSym = isExpanded[i] ? "V" : ">";

		if (imgui_Button(scrx, starty + 1, 22.0f, 22.0f, expSym))
		{
			isExpanded[i] = !isExpanded[i];
		}
		starty += off;

		if (isExpanded[i])
		{
			const char * path = "None";
			bool haveAnim = additionalAnimData[i].animIndexRemap != -1;
			if (haveAnim)
			{
				const char *fileName = m_AnimPool.Get(additionalAnimData[i].animIndexRemap)->pAnimFileName;
				path = strrchr(fileName, '\\') + 1;
			}
			sprintf_s(buf, _countof(buf), "Path: %s", path);

			int off = imgui_Static(scrxOff, starty, buf, 300);
			const char * btnText = "Pick";
			if (imgui_Button(scrxOff + 305, starty + 2, 30, 22.0f, btnText))
			{
				globalFileObjectsTree.Clear();
				pickAnim = i;
			}
			starty += off;
 			if (haveAnim)
 			{
 				r3dAnimData *ad = m_AnimPool.Get(additionalAnimData[i].animIndexRemap);
				AdditionalAnimationData &aad = additionalAnimData[i];
 				starty += imgui_Value_Slider(scrxOff, starty, "Speed", &ad->fFrameRate, 1.0f, 100.0f, "%-02.3f", true, true, 300);

				starty += imgui_Value_SliderI(scrxOff, starty, "Start frame", &aad.startFrame, 0, static_cast<float>(aad.totalFrames), "%d", true, true, 300);
				if (aad.startFrame > aad.endFrame)
				{
					aad.endFrame = aad.startFrame;
				}

				starty += imgui_Value_SliderI(scrxOff, starty, "End frame", &aad.endFrame, 0, static_cast<float>(aad.totalFrames), "%d", true, true, 300);
				if (aad.endFrame < aad.startFrame)
				{
					aad.startFrame = aad.endFrame;
				}
				ad->NumFrames = aad.endFrame;

				starty += imgui_Value_Slider(scrxOff, starty, "Fade in time", &aad.fadeInTime, 0.0f, 3.0f, "%-02.3f", true, true, 300);
				starty += imgui_Value_Slider(scrxOff, starty, "Fade out time", &aad.fadeOutTime, 0.0f, 3.0f, "%-02.3f", true, true, 300);

				int oldVal = aad.onlyUpperBody;
				starty += imgui_Checkbox(scrxOff, starty, 300, 22, "Only upper body", &aad.onlyUpperBody, 1);

				if (aad.onlyUpperBody != oldVal)
				{
					m_Animation->StopAll();
					r3dAnimData *ad = m_AnimPool.Get(aad.animIndexRemap);

					SwitchUpperBodyAnimation(aad.onlyUpperBody > 0, ad);
				}

				oldVal = aad.onlyUpperBody;
				starty += imgui_Checkbox(scrxOff, starty, 300, 22, "Only lower body", &aad.onlyLowerBody, 1);

				if (aad.onlyLowerBody != oldVal)
				{
					m_Animation->StopAll();
					r3dAnimData *ad = m_AnimPool.Get(aad.animIndexRemap);

					SwitchLowerBodyAnimation(aad.onlyLowerBody > 0, ad);
				}

 			}
			starty += 5;
		}
	}

	if (pickAnim > -1)
	{
		static r3dSTLString s;
		g_pDesktopManager->End();
		bool isPicked = !DrawPossibleElementsChoose("data\\objectsdepot\\mechs\\", "*.anm", 0, "\\Animations\\", s);
		g_pDesktopManager->Begin("ed_objects");

		if (isPicked)
		{
			if (!s.empty())
			{
				bool haveAnim = additionalAnimData[pickAnim].animIndexRemap != -1;

				if (haveAnim)
				{
					RemoveAnimation(pickAnim);
				}

				AdditionalAnimationData &ad = additionalAnimData[pickAnim];
				ad.animIndexRemap = m_AnimPool.Add(MECH_ANIMATION_LIST[pickAnim], s.c_str(), 0);
				ad.onlyUpperBody = false;
				r3dAnimData *animData = m_AnimPool.Get(ad.animIndexRemap);
				ad.totalFrames = animData->NumFrames;
				ad.endFrame = animData->NumFrames;
			}

			pickAnim = -1;
		}
	}

	return starty - scry;
}

//////////////////////////////////////////////////////////////////////////

float obj_Mech::DrawGeneralSettingsEditor(float scrx, float scry, float scrw, float scrh)
{
	float starty = scry;

	starty += imgui_Static(scrx, starty, "Camera position adjustments:");
	starty += imgui_Value_Slider(scrx, starty, "V offset: ", &cameraTargetOffset.y, -30.0f, 30.0f, "%-02.3f");
	starty += imgui_Value_Slider(scrx, starty, "H offset: ", &horizontalOffset, -15.0f, 15.0f, "%-02.3f");

	starty += 10.0f;

	starty += imgui_Static(scrx, starty, "Camera shaking adjustments:");

	starty += 10.0f;

	starty += imgui_Value_Slider(scrx, starty, "Lock camera distance", &cameraLockDistance, 0.0f, 50.0f, "%-02.3f");

	int forceScope = r_force_scope->GetInt() ;
	starty += imgui_Checkbox(scrx, starty, 360, 22, "Show crosshair", &forceScope, 1) ;

	extern int g_RenderScopeEffect;
	if (!r_force_scope->GetInt() && forceScope && !g_RenderScopeEffect)
	{
		extern r3dTexture* SimpleCrosshair_Reticle;
		extern r3dTexture* SimpleCrosshair_Mask ;
		extern r3dTexture* ScopeTest_BlurMask ;

		gPFX_ScopeEffect.SetScopeReticle( SimpleCrosshair_Reticle );
		gPFX_ScopeEffect.SetScopeNormal( 0 );
		gPFX_ScopeEffect.SetScopeMask( SimpleCrosshair_Mask );
		gPFX_ScopeEffect.SetScopeBlurMask( ScopeTest_BlurMask );
		gPFX_ScopeEffect.SetScopeScale(0.1f);
	}

	r_force_scope->SetInt(forceScope);

	if (forceScope)
	{
		starty += imgui_Static(scrx, starty, "Crosshair offset: ");
		starty += imgui_Value_Slider(scrx, starty, "X: ", &crosshairOffset.x, -0.5f, 0.5f, "%-02.3f");
		starty += imgui_Value_Slider(scrx, starty, "Y: ", &crosshairOffset.y, -0.5f, 0.5f, "%-02.3f");
		gPFX_ScopeEffect.SetScopeOffset(crosshairOffset);
	}

	return starty - scry;
}

//////////////////////////////////////////////////////////////////////////

float obj_Mech::DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected)
{
	float starty = scry;

	//starty += parent::DrawPropertyEditor(scrx, scry, scrw,scrh, startClass, selected );

	if( ! IsParentOrEqual( &ClassData, startClass ) )	
		return starty - scry ;

	imgui2_StringValueEx(scrx, starty, DEFAULT_CONTROLS_WIDTH, "Preset Name: ", sPresetName);
	starty += DEFAULT_CONTROLS_HEIGHT;

	if (strlen(sPresetName) == 0 && strlen(sMechSelected) != 0)
	{
		strcpy_s(sPresetName, _countof(sPresetName), strrchr(sMechSelected, '\\') + 1);
	}

	float btnW = DEFAULT_CONTROLS_WIDTH / 3;
	static bool pickPreset = 0;

	if (imgui_Button(scrx, starty, btnW, 22.0f, "Drive me"))
	{
		SwitchDriveModes(true);
	}

	if (imgui_Button(scrx + btnW, starty, btnW, 22.0f, "Save Preset"))
	{
		SaveCurrentPreset();

		extern float g_ShowSaveSign;
		g_ShowSaveSign = r3dGetTime() + 2.2f ;
	}
	if (imgui_Button(scrx + btnW * 2, starty, btnW, 22.0f, "Load Preset"))
	{
		globalFileObjectsTree.Clear();
		pickPreset = true;
	}
	starty += 32.f;

	if (!pickPreset)
	{
		static int editMode = 0;

		static const char* list[] = { "Meshes", "Animations", "Events", "General" };
		starty += imgui_Toolbar(scrx, starty, 360.0f, 28.0f, &editMode, 0, list, _countof(list));

		starty += 5.0f;

		switch (editMode)
		{
		case 0:
			starty += DrawMeshesConfigEditor(scrx, starty, scrw, scrh);
			break;
		case 1:
			starty += DrawAnimationsConfigEditor(scrx, starty, scrw, scrh);
			break;
		case 2:
			starty += mechEvents.DrawEditor(scrx, starty, scrw, scrh);
			break;
		case 3:
			starty += DrawGeneralSettingsEditor(scrx, starty, scrw, scrh);
			break;
		}
	}
	else
	{
		static r3dSTLString s;
		g_pDesktopManager->End();
		pickPreset = DrawPossibleElementsChoose("data\\objectsdepot\\", "*.xml", "mechs", "\\", s);
		g_pDesktopManager->Begin("ed_objects");

		if (!pickPreset && !s.empty())
		{
			LoadPreset(s.c_str());
		}
	}

	return starty - scry;
}

void obj_Mech::UpdateLocalMovement()
{
	r3d_assert(NetworkLocal);
	R3DPROFILE_FUNCTION("obj_Mech::UpdateLocalMovement");
	
	// important: limit movement to max for 1 sec. so server can check for teleports
	const float TimePassed = R3D_MIN(r3dGetFrameTime(), 1.0f);
	const float curTime    = r3dGetTime();

	r3dPoint3D RealAcceleration(0, 0, forwardSpeed);

	if (bOnGround)
		fallingVelocity = 20;

	// locally add gravity to acceleration
	float Gravity = 30.81f;
	fallingVelocity += Gravity * TimePassed;
	RealAcceleration.Y -= fallingVelocity;
	
	r3dPoint3D vel = r3dPoint3D(0,0,0);
	vel    += (GetMoveForw()  * RealAcceleration.Z);
	vel    += (GetMoveRight() * RealAcceleration.X);
	vel.Y  += RealAcceleration.Y;
	
	//r3dOutToLog("vel.x=%.2f, y=%.2f, z=%.2f\n", vel.x, vel.y, vel.z);

	ControllerPhysObj* controller = (ControllerPhysObj*)PhysicsObject;

	R3DPROFILE_START("PhysController::Move");
	controller->Move(vel * TimePassed, 0.5f);
	R3DPROFILE_END("PhysController::Move");
	r3dPoint3D prevPos = GetPosition();
	parent::Update(); // manually update position after updating physics. Needed for camera, otherwise it will be lagging one frame behind
	SetVelocity(vel);

	DetectIfOnGround();
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::DetectIfOnGround()
{
	PxRaycastHit hit;
	PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC));
	if(g_pPhysicsWorld->raycastSingle(PxVec3(GetPosition().x, GetPosition().y + 1.5f, GetPosition().z), PxVec3(0, -1, 0), 20.0f, PxSceneQueryFlags(PxSceneQueryFlag::eDISTANCE), hit, filter))
	{
		bOnGround = (hit.distance < 5.5f);
		fHeightAboveGround = hit.distance - 1.5f;
	}
	else
	{
		bOnGround = false;
		fHeightAboveGround = 20.0f;
	}

	if (!bOnGround && fHeightAboveGround > 1.0f)
	{
		animFSM.ChangeState(&MechAnimationsFSM::gStateFalling);
	}
}

//
// rendering
//

void obj_Mech::DrawDebug(const r3dCamera& Cam)
{
	r3dRenderer->SetTex(NULL);
	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_NZ);


	// skeleton draw
	if(m_renderSkel && m_BindSkeleton)
	{
		//r3dDrawBoundBox(GetBBoxWorld(), Cam, r3dColor(0, 0, 255), 0.2f);
		if(m_BindSkeleton)
		{
			m_Animation->GetCurrentSkeleton()->DrawSkeleton(Cam, GetPosition());
		}
	}
	
	// newly created mech
	if(!m_BindSkeleton)
	{
		r3dBoundBox bb;
		bb.Size = r3dPoint3D(2, 2, 2);
		bb.Org  = GetPosition() - bb.Size / 2;
		r3dDrawBoundBox(GetBBoxWorld(), Cam, r3dColor(0, 0, 255), 0.3f);
	}
		
	/*
	r3dPoint3D p1 = GetPosition();
	r3dDrawLine3D(p1, p1 + r3dPoint3D(0, 1, 0) * 20, Cam, 1, r3dColor(255, 0, 0));
	r3dDrawLine3D(p1, p1 + GetMoveRight() * 20, Cam, 1, r3dColor(0, 255, 0));
	r3dDrawLine3D(p1, p1 + GetMoveForw() * 20, Cam, 1, r3dColor(0, 0, 255));
	*/
}



struct obj_MechDeferredRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		obj_MechDeferredRenderable* This = static_cast<obj_MechDeferredRenderable*>( RThis );
		if(This->Parent->m_BindSkeleton == NULL)
			return;

		float vCData[ 4 ] = { 1, 1, 1, 0 } ;
		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( MC_MASKCOLOR, vCData, 1 ) );

		int oldVsId = r3dRenderer->GetCurrentVertexShaderIdx();
		This->Parent->uberEquip_->Draw(This->Parent->m_Animation->GetCurrentSkeleton(), This->Parent->DrawMatrix, true, CMechUberEquip::DT_DEFERRED);
		r3dRenderer->SetVertexShader(oldVsId);
	}

	obj_Mech* Parent;
};

struct obj_MechShadowRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		obj_MechShadowRenderable* This = static_cast<obj_MechShadowRenderable*>( RThis );
		if(This->Parent->m_BindSkeleton == NULL)
			return;
		int oldVsId = r3dRenderer->GetCurrentVertexShaderIdx();
		This->Parent->uberEquip_->DrawSM(This->Parent->m_Animation->GetCurrentSkeleton(), This->Parent->DrawMatrix, true);		
		r3dRenderer->SetVertexShader(oldVsId);
	}

	obj_Mech* Parent;
};

struct obj_MechDebugRenderable : Renderable
{
	void Init()
	{
		DrawFunc = Draw;
	}

	static void Draw( Renderable* RThis, const r3dCamera& Cam )
	{
		obj_MechDebugRenderable* This = static_cast<obj_MechDebugRenderable*>( RThis );
		This->Parent->DrawDebug(Cam);
		This->Parent->mechEvents.DrawDebug();
	}

	obj_Mech* Parent;
};

#define	RENDERABLE_BUILDING_SORT_VALUE (31*RENDERABLE_USER_SORT_VALUE)

void obj_Mech::AppendShadowRenderables( RenderArray & rarr, int sliceIndex, const r3dCamera& Cam )
{
	obj_MechShadowRenderable rend;
	rend.Init();
	rend.Parent	= this;
	rend.SortValue	= RENDERABLE_BUILDING_SORT_VALUE;

	rarr.PushBack( rend );
}

void obj_Mech::AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam )
{
	// deferred
	{
		obj_MechDeferredRenderable rend;
		rend.Init();
		rend.Parent	= this;
		rend.SortValue	= RENDERABLE_BUILDING_SORT_VALUE;

		render_arrays[ rsFillGBuffer ].PushBack( rend );
	}

	// debug
	{
		obj_MechDebugRenderable rend;
		rend.Init();
		rend.Parent	= this;
		rend.SortValue	= RENDERABLE_BUILDING_SORT_VALUE;

		render_arrays[ rsDrawComposite1 ].PushBack( rend );
	}
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::ReadSerializedData(pugi::xml_node& node)
{
	parent::ReadSerializedData(node);

	r3dString presetName;
	GetXMLVal("mech_preset", node, &presetName);

	if (presetName.Length() > 0)
		LoadPreset(presetName.c_str());
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::ReadOwnSerializedData(pugi::xml_node& node)
{
	pugi::xml_node mechNode = node.child("mech");
	
	r3dscpy(sMechSelected, mechNode.attribute("path").value());
	if (sMechSelected[0] == 0)
		return;

	curBodyPart = mechNode.attribute("body_type").as_int();
	curArmorPart = mechNode.attribute("armor_type").as_int();

	pugi::xml_attribute a;

	a = mechNode.attribute("walk_speed");
	if (!a.empty())
		MoveSpeed = a.as_float();

	a = mechNode.attribute("walk_back_speed");
	if (!a.empty())
		MoveBackSpeed = a.as_float();

	a = mechNode.attribute("walk_limp_speed");
	if (!a.empty())
		MoveLimpSpeed = a.as_float();

	a = mechNode.attribute("walk_back_limp_speed");
	if (!a.empty())
		MoveBackLimpSpeed = a.as_float();

	a = mechNode.attribute("run_speed");
	if (!a.empty())
		RunSpeed = a.as_float();

	a = mechNode.attribute("rot_speed");
	if (!a.empty())
		rotationSpeed = a.as_float();

	float val = 0;
	if (GetXMLVal("walk_fwd_accel", mechNode, &val))
	{
		moveFwdAccel = val;
	}
	if (GetXMLVal("walk_back_accel", mechNode, &val))
	{
		moveBackAccel = val;
	}
	if (GetXMLVal("run_accel", mechNode, &val))
	{
		runAccel = val;
	}

	GetXMLVal("turret_rot_speed", mechNode, &turretRotSpeed);
	GetXMLVal("turret_rot_delay", mechNode, &turretRotDelayTime);
	GetXMLVal("turret_rot_speedup", mechNode, &turretRotSpeedUpTime);
	GetXMLVal("turret_rot_limit", mechNode, &turretLimitAngle);

	GetXMLVal("camera_target", mechNode, &cameraTargetOffset);
	GetXMLVal("camera_horizontal_offset", mechNode, &horizontalOffset);
	GetXMLVal("crosshair_offset", mechNode, &crosshairOffset);
	
	GetXMLVal("camera_lock_dist", mechNode, &cameraLockDistance);

	ReinitializeMech(sMechSelected);
	
	for (int i = MSLOT_Weapon1, wpnIdx = 0; i < MSLOT_Max; ++i, ++wpnIdx) 
	{
		char name[16];
		sprintf(name, "slot%i", i);
		
		pugi::xml_node slotNode = mechNode.child(name);
		if (!slotNode)
			continue;

		const char* val = slotNode.attribute("file").value();
		if (*val == 0)
			continue;
		
		if (r3d_access(val, 0) == 0)
		{
			LoadWeaponSlot(static_cast<EMechSlot>(i), val);
			uberEquip_->LoadSlot(i, val);

			GetXMLVal("weapon_type", slotNode, &wpnAnimations[wpnIdx].wpnType);
		}
	}

	for (int i = 0; i < _countof(MECH_ANIMATION_LIST); i++) 
	{
		pugi::xml_node animNode = mechNode.child("anim");

		while (!animNode.empty() && strcmpi(animNode.attribute("name").value(), MECH_ANIMATION_LIST[i]) != 0)
		{
			animNode = animNode.next_sibling("anim");
		}

		if (animNode.empty())
			continue;

		const char* val = animNode.attribute("file").value();
		if (*val == 0)
			continue;

		if (r3d_access(val, 0) == 0)
		{
			additionalAnimData[i].animIndexRemap = m_AnimPool.Add(MECH_ANIMATION_LIST[i], val, 0);
			additionalAnimData[i].source = val;

			r3dAnimData *ad = m_AnimPool.Get(additionalAnimData[i].animIndexRemap);
			AdditionalAnimationData &aad = additionalAnimData[i];

			GetXMLVal("frame_rate", animNode, &ad->fFrameRate);
			GetXMLVal("fade_in_time", animNode, &aad.fadeInTime);
			GetXMLVal("fade_out_time", animNode, &aad.fadeOutTime);
			GetXMLVal("upper_body", animNode, &aad.onlyUpperBody);
			GetXMLVal("lower_body", animNode, &aad.onlyLowerBody);

			if (GetXMLVal("start_frame", animNode, &aad.startFrame) == 0)
				aad.startFrame = 0;
			
			if (GetXMLVal("end_frame", animNode, &aad.endFrame) == 0)
				aad.endFrame = ad->NumFrames;

			aad.totalFrames = ad->NumFrames;
			ad->NumFrames = aad.endFrame;

			if (additionalAnimData[i].onlyUpperBody && m_BindSkeleton)
			{
				r3dAnimData *ad = m_AnimPool.Get(additionalAnimData[i].animIndexRemap);
				SwitchUpperBodyAnimation(additionalAnimData[i].onlyUpperBody > 0, ad);
			}
		}
	}
	mechEvents.LoadFromXML(mechNode);
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::WriteSerializedData(pugi::xml_node& node)
{
	parent::WriteSerializedData(node);
	
	if (sPresetName && strlen(sPresetName) > 0)
	{
		char buf[MAX_PATH] = {0};
		sprintf_s(buf, MAX_PATH, "data\\objectsdepot\\Mechs\\%s.xml", sPresetName);
		SetXMLVal("mech_preset", node, buf);

		if( r3dGameLevel::IsSaving )
		{
			SaveCurrentPreset();
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::WriteOwnSerializedData(pugi::xml_node& node)
{
	parent::WriteSerializedData(node);
	pugi::xml_node mechNode = node.append_child();
	mechNode.set_name("mech");
	
	if(!sMechSelected[0])
		return;

	mechNode.append_attribute("path") = sMechSelected;
	mechNode.append_attribute("body_type") = curBodyPart;
	mechNode.append_attribute("armor_type") = curArmorPart;
	mechNode.append_attribute("walk_speed") = MoveSpeed;
	mechNode.append_attribute("walk_back_speed") = MoveBackSpeed;
	mechNode.append_attribute("walk_limp_speed") = MoveLimpSpeed;
	mechNode.append_attribute("walk_back_limp_speed") = MoveBackLimpSpeed;
	mechNode.append_attribute("run_speed") = RunSpeed;
	mechNode.append_attribute("rot_speed") = rotationSpeed;

	SetXMLVal("walk_fwd_accel", mechNode, &moveFwdAccel);
	SetXMLVal("walk_back_accel", mechNode, &moveBackAccel);
	SetXMLVal("run_accel", mechNode, &runAccel);

	SetXMLVal("turret_rot_speed", mechNode, &turretRotSpeed);
	SetXMLVal("turret_rot_delay", mechNode, &turretRotDelayTime);
	SetXMLVal("turret_rot_speedup", mechNode, &turretRotSpeedUpTime);
	SetXMLVal("turret_rot_limit", mechNode, &turretLimitAngle);

	SetXMLVal("camera_target", mechNode, &cameraTargetOffset);
	SetXMLVal("camera_horizontal_offset", mechNode, &horizontalOffset);
	SetXMLVal("crosshair_offset", mechNode, &crosshairOffset);

	SetXMLVal("camera_lock_dist", mechNode, &cameraLockDistance);

	for (int i = MSLOT_Weapon1, wpnIdx = 0; i < MSLOT_Max; ++i, ++wpnIdx) 
	{
		if (uberEquip_->slots_[i].mesh == NULL)
			continue;

		char name[16];
		sprintf(name, "slot%i", i);

		pugi::xml_node slotNode = mechNode.append_child();
		slotNode.set_name(name);

		const char *path = uberEquip_->slots_[i].mesh->FileName.c_str();
		slotNode.append_attribute("file") = path;
		SetXMLVal("weapon_type", slotNode, &wpnAnimations[wpnIdx].wpnType);
	}

	//	Animations
	for (int i = 0; i < _countof(MECH_ANIMATION_LIST); ++i)
	{
		if (additionalAnimData[i].animIndexRemap == -1)
			continue;

		r3dAnimData *ad = m_AnimPool.Get(additionalAnimData[i].animIndexRemap);
		if (!ad)
			continue;

		const char *path = ad->pAnimFileName;

		pugi::xml_node animNode = mechNode.append_child();
		animNode.set_name("anim");
		animNode.append_attribute("name") = MECH_ANIMATION_LIST[i];
		animNode.append_attribute("file") = path;

		SetXMLVal("frame_rate", animNode, &ad->fFrameRate);
		SetXMLVal("fade_in_time", animNode, &additionalAnimData[i].fadeInTime);
		SetXMLVal("fade_out_time", animNode, &additionalAnimData[i].fadeOutTime);
		SetXMLVal("upper_body", animNode, &additionalAnimData[i].onlyUpperBody);
		SetXMLVal("lower_body", animNode, &additionalAnimData[i].onlyLowerBody);
		SetXMLVal("start_frame", animNode, &additionalAnimData[i].startFrame);
		SetXMLVal("end_frame", animNode, &additionalAnimData[i].endFrame);
	}

	mechEvents.SaveToXML(mechNode);
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::SaveCurrentPreset()
{
	pugi::xml_document xmlFile;
	pugi::xml_node root = xmlFile.append_child();
	root.set_name("root");
	WriteOwnSerializedData(root);
	r3dSTLString presetPath = r3dSTLString("data\\objectsdepot\\mechs\\") + sPresetName + ".xml";
	xmlFile.save_file(presetPath.c_str());
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::LoadPreset(const char *presetPath)
{
	if (!presetPath)
		return;

	r3dFile* f = r3d_open(presetPath, "rb");
	if (!f)
	{
		r3dError("Failed to open: %s\n", presetPath);
	}
	else
	{
		for (int i = 0; i < _countof(additionalAnimData); ++i)
			additionalAnimData[i].animIndexRemap = -1;

		mechEvents.Init(this);

		char* fileBuffer = game_new char[f->size + 1];
		fread(fileBuffer, f->size, 1, f);
		fileBuffer[f->size] = 0;

		pugi::xml_document xmlFile;
		pugi::xml_parse_result parseResult = xmlFile.load_buffer_inplace(fileBuffer, f->size);
		if (!parseResult)
			r3dError("Failed to parse XML %s, error: %s", presetPath, parseResult.description());
		ReadOwnSerializedData(xmlFile.child("root"));
		strcpy_s(sPresetName, _countof(sPresetName), strrchr(presetPath, '\\') + 1);
		*(strrchr(sPresetName, '.')) = 0;
		delete fileBuffer;
		fclose(f);
	}

	SetDefaultPose();
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::UnloadWeaponSlot(EMechSlot wpnSlot)
{
	r3d_assert(wpnSlot >= MSLOT_Weapon1 && wpnSlot < MSLOT_Max);
	uberEquip_->UnloadSlot(wpnSlot);
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::LoadWeaponSlot(EMechSlot wpnSlot, const char *path)
{
	r3d_assert(wpnSlot >= MSLOT_Weapon1 && wpnSlot < MSLOT_Max);
	uberEquip_->LoadSlot(wpnSlot, path);

	//	Try to load weapon fire animation
	char buf[MAX_PATH] = {0};
	strcpy_s(buf, _countof(buf), path);
	for (int i = 0; i < _countof(buf); ++i)
	{
		if (buf[i] == '/')
			buf[i] = '\\';
	}

	char *fn = strdup(PathFindFileName(buf));
	PathRemoveExtension(fn);
	PathRemoveFileSpec(buf);

	r3dSTLString anmPath(buf);
	anmPath = anmPath + "\\Animations\\" + fn + "_Fire.anm";

	if (r3d_access(anmPath.c_str(), 0) == 0)
	{
		int wpnIdx = wpnSlot - MSLOT_Weapon1;

		CMechUberEquip::slot_s &sl = uberEquip_->slots_[wpnSlot];

		WeaponsAnimData &wad = wpnAnimations[wpnIdx];

		int fireAnimID = m_AnimPool.Add(fn, anmPath.c_str());
		delete wad.anim;
		sl.skel = 0;

		char *skelName = strdup(path);
		int len = strlen(skelName);
		if (len > 4)
		{
			skelName[len - 4] = 0;
			strncat(skelName, ".skl", 4);
		}

		if (r3d_access(skelName, 0) == 0)
		{
			r3dSkeleton *skl = new r3dSkeleton();

			skl = game_new r3dSkeleton();
			skl->LoadBinary(skelName);
			wad.anim = game_new r3dAnimation();
			wad.anim->Init(skl, &m_AnimPool, 0, reinterpret_cast<DWORD>(this));
			wad.fireAnimID = fireAnimID;

			sl.skel = wad.anim->GetCurrentSkeleton();
			delete skl;
		}
		free(skelName);
	}

	free(fn);
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::SetDefaultPose()
{
	//	Set mech to its default pose if "idle" anim is available
	AdditionalAnimationData &aad = additionalAnimData[MECH_ANIM_IDLE];
	if (aad.animIndexRemap != -1)
	{
		m_Animation->StartAnimation(aad.animIndexRemap, ANIMFLAG_Looped);
	}
}

//------------------------------------------------------------------------

void obj_Mech::EDITOR_ReloadAnim( const char* fname )
{
	SAFE_DELETE(m_Animation);
	m_AnimPool.Unload();

	m_Animation = game_new r3dAnimation();
	m_Animation->Init(m_BindSkeleton, &m_AnimPool, _mech_AdjustBoneCallback, (DWORD)this);

	for (int i = 0; i < _countof(MECH_ANIMATION_LIST); i++) 
	{
		if( r3d_access( additionalAnimData[i].source.c_str(), 0 ) == 0)
		{
			AdditionalAnimationData& aad = additionalAnimData[i];

			aad.animIndexRemap = m_AnimPool.Add(MECH_ANIMATION_LIST[i], aad.source.c_str(), 0);
			r3dAnimData *animData = m_AnimPool.Get(aad.animIndexRemap);
			aad.endFrame = animData->NumFrames;
			aad.totalFrames = animData->NumFrames;

		}
		else
		{
			additionalAnimData[i].animIndexRemap = -1;
		}
	}
}

//------------------------------------------------------------------------

void obj_Mech::OnControllerHit( PhysicsCallbackObject* obj )
{
	if( GameObject* gobj = obj->isGameObject() )
	{
		if( gobj->ObjTypeFlags & OBJTYPE_Building )
		{
			obj_Building* building = static_cast< obj_Building* >( gobj );

			if( building->CanBeDestroyed() )
			{
				building->DestroyBuilding();
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void obj_Mech::SwitchDriveModes(bool driveMech)
{
	drivable = driveMech;
	d_force_mouse_movement_editor->SetBool(driveMech);
	d_disable_editor_gui->SetBool(driveMech);

	CameraHUD *h = static_cast<CameraHUD*>(editor_GetHudByIndex(CurHUDID));
	h->CameraMode = drivable ? hud_TPSCamera : hud_FPSCamera;
	h->customTextToShow =  drivable ? "Press ESC to exit mech drive mode" : 0;

	extern ObjectManipulator3d g_Manipulator3d;
	if (driveMech)
	{
		h->TPS_Target = GetPosition();
		g_Manipulator3d.PickerRemoveFromPicked(this);
		currentCameraPos = GetPosition();
	}
	else
	{
		g_Manipulator3d.PickerAddToPicked(this);
	}
}

//////////////////////////////////////////////////////////////////////////

r3dVector obj_Mech::GetPelvisPos()
{
	D3DXMATRIX pelvisPosMat, worldPos = DrawMatrix;
	D3DXMatrixIdentity(&pelvisPosMat);
	m_Animation->GetCurrentSkeleton()->GetBoneWorldTM(boneId_MechPelvis, &pelvisPosMat, worldPos);
	r3dVector pelvisPos(pelvisPosMat._41, pelvisPosMat._42, pelvisPosMat._43);

	return pelvisPos;
}

//////////////////////////////////////////////////////////////////////////

void ReloadMechAnim( const char* fname )
{
	for( ObjectIterator iter = GameWorld().GetFirstOfAllObjects(); iter.current ; iter = GameWorld().GetNextOfAllObjects( iter ) )
	{
		GameObject* pObj = iter.current;

		if( pObj->isObjType( OBJTYPE_MechWarrior ) )
		{
			obj_Mech* mech = static_cast< obj_Mech* > ( pObj );

			mech->EDITOR_ReloadAnim( fname );
		}
	}
}

#endif