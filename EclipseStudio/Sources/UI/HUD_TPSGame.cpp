#include "r3dPCH.h"
#include "r3d.h"

#include "r3dBackgroundTaskDispatcher.h"

#include "d3dfont.h"

#include "GameCommon.h"
#include "Gameplay_Params.h"

#include "UI\HUD_TPSGame.h"
#include "ObjectsCode/AI/AI_Player.h"
#include "ObjectsCode/AI/AI_PlayerAnim.h"
#include "ObjectsCode/Gameplay/BasePlayerSpawnPoint.h"
#include "ObjectsCode/weapons/WeaponArmory.h"

#include "ObjectsCode/Gameplay/obj_UAV.h"

#include "APIScaleformGfx.h"

#include "multiplayer/ClientGameLogic.h"

#include "HUDCameraEffects.h"

#include "UI\HUDDisplay.h"
#include "UI\HUDPause.h"
#include "UI\HUDAttachments.h"
#include "UI\HUDVault.h"
#include "UI\HUDStore.h"
#include "UI\HUDActionUI.h"
#include "UI\HUDSafelock.h"
#include "UI\HUDTrade.h"
#include "UI\HUDRepair.h"
#include "UI\HUDCrafting.h"

#include "..\GameEngine\gameobjects\obj_Vehicle.h"

#include "rendering/Deffered/D3DMiscFunctions.h"

extern bool g_bEditMode;
extern float GameFOV;

extern float getWaterDepthAtPos(const r3dPoint3D& pos);

HUDDisplay*	hudMain = NULL;
HUDPause*	hudPause = NULL;
HUDVault*	hudVault = NULL;
HUDSafelock* hudSafelock = NULL;
HUDTrade* hudTrade = NULL;
HUDStore*	hudStore = NULL;
HUDAttachments*	hudAttm = NULL;
HUDActionUI*	hudActionUI = NULL;
HUDRepair*	hudRepair = NULL;
HUDCrafting* hudCraft = NULL;


#define VEHICLE_CINEMATIC_MODE 0

void TPSGameHUD_AddHitEffect(GameObject* from)
{
	obj_Player* pl = gClientLogic().localPlayer_;
	if(!pl) return;
	if(pl->bDead) return;

	pl->BloodEffect = 3.0f;
}



TPSGameHUD::TPSGameHUD()
{
	FPS_Acceleration.Assign(0,0,0);
	FPS_vViewOrig.Assign(0,0,0);
	FPS_ViewAngle.Assign(0,0,0);
	FPS_vVision.Assign(0,0,0);
	FPS_vRight.Assign(0,0,0);
	FPS_vUp.Assign(0,0,0);
	FPS_vForw.Assign(0,0,0);
	cameraRayLen = 20000.0f;
}

TPSGameHUD::~TPSGameHUD()
{
}

static bool TPSGameHud_Inited;
void TPSGameHUD_OnStartGame()
{
	const GBGameInfo& ginfo = gClientLogic().m_gameInfo;

	hudMain = game_new HUDDisplay();
	hudPause = game_new HUDPause();
	hudVault = game_new HUDVault();
	hudSafelock = game_new HUDSafelock();
	hudTrade = game_new HUDTrade();
	hudStore = game_new HUDStore();
	hudActionUI = game_new HUDActionUI();
	hudAttm = game_new HUDAttachments();
	hudRepair = game_new HUDRepair();
	hudCraft = game_new HUDCrafting();

	hudMain->Init();
	hudPause->Init();
	hudVault->Init();
	hudSafelock->Init();
	hudTrade->Init();
	hudStore->Init();
	hudActionUI->Init();
	hudAttm->Init();
	hudRepair->Init();
	hudCraft->Init();

	Mouse->Hide(true);
	// lock mouse to a window when playing a game
	d_mouse_window_lock->SetBool(true);
	Mouse->SetRange(r3dRenderer->HLibWin);


	extern int g_CCBlackWhite;
	extern float g_fCCBlackWhitePwr;
	g_CCBlackWhite = false;
	g_fCCBlackWhitePwr = 0.0f;

	TPSGameHud_Inited = true;
}

void TPSGameHUD :: DestroyPure()
{
	if(TPSGameHud_Inited)
	{
		TPSGameHud_Inited = false;

		hudPause->Unload();
		hudVault->Unload();
		hudSafelock->Unload();
		hudTrade->Unload();
		hudStore->Unload();
		hudMain->Unload();
		hudActionUI->Unload();
		hudAttm->Unload();
		hudRepair->Unload();
		hudCraft->Unload();

		SAFE_DELETE(hudMain);
		SAFE_DELETE(hudPause);
		SAFE_DELETE(hudVault);
		SAFE_DELETE(hudSafelock);
		SAFE_DELETE(hudTrade);
		SAFE_DELETE(hudStore);
		SAFE_DELETE(hudActionUI);
		SAFE_DELETE(hudAttm);
		SAFE_DELETE(hudRepair);
		SAFE_DELETE(hudCraft);
	}
}

void TPSGameHUD :: SetCameraDir (r3dPoint3D vPos )
{

}

r3dPoint3D TPSGameHUD :: GetCameraDir () const
{
	return r3dVector(1,0,0);
}


extern	PlayerStateVars_s TPSHudCameras[3][PLAYER_NUM_STATES];
extern	Playerstate_e ActiveCameraRigID;
extern	PlayerStateVars_s ActiveCameraRig;

extern 	Playerstate_e CurrentState;
extern 	PlayerStateVars_s CurrentRig;
extern 	PlayerStateVars_s SourceRig;
extern 	PlayerStateVars_s TargetRig;
extern 	float LerpValue;
extern	r3dPoint3D TPSHudCameraTarget;

extern  float	TPSCameraPointToAdj[3];
extern  float   TPSCameraPointToAdjCrouch[3];

void TPSGameHUD :: InitPure()
{
	// reinit hud rigs based on camera mode
	CurrentRig = TPSHudCameras[g_camera_mode->GetInt()][PLAYER_IDLE];
	SourceRig  = CurrentRig;
	TargetRig  = CurrentRig;
}


// camPos = current camera pos. target = player's head pos
bool CheckCameraCollision(r3dPoint3D& camPos, const r3dPoint3D& target, bool checkCamera)
{
	R3DPROFILE_FUNCTION("CheckCameraCollision");

	r3dPoint3D origCamPos = camPos;
    int LoopBreaker = 0;

	r3dPoint3D motion = (camPos - target);
	float motionLen = motion.Length();
	int MaxLoopBreaker = 10;
	if(motionLen > 0.1f)
	{
		motion.Normalize();
		MaxLoopBreaker = int(ceilf(motionLen/0.05f));

		PxSphereGeometry camSphere(0.3f);
		PxSphereGeometry camSphere2(0.3f);
		PxTransform camPose(PxVec3(target.x, target.y, target.z), PxQuat(0,0,0,1));
		PxTransform camPose2(PxVec3(camPos.x, camPos.y, camPos.z), PxQuat(0,0,0,1));

		PxSweepHit sweepResults[32];
		bool blockingHit;
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_PLAYER_COLLIDABLE_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC);

		// first pass, check if there are any collisions in the direction from head to camera
		bool firstPass = false;
		int numRes=g_pPhysicsWorld->PhysXScene->sweepMultiple(camSphere, camPose, PxVec3(motion.x, motion.y, motion.z), motionLen, PxSceneQueryFlag::eINITIAL_OVERLAP|PxSceneQueryFlag::eINITIAL_OVERLAP_KEEP|PxSceneQueryFlag::eNORMAL, sweepResults, 32, blockingHit, filter);
		while(( 0 != numRes || 0 < getWaterDepthAtPos(camPos + r3dPoint3D(0, -camSphere.radius * 0.5f, 0)) ) && LoopBreaker<MaxLoopBreaker)
		{
			firstPass = true;
			if(numRes == -1)
			{
				r3d_assert(false);
				break;
			}

			r3dPoint3D tmp = -motion;
			tmp.Normalize();
			camPos += tmp * 0.05f;

			LoopBreaker++;

			motion = (camPos - target);
			motionLen = motion.Length();
			if(motionLen < 0.005f)
			{
				LoopBreaker = MaxLoopBreaker;
				break;
			}
			motion.Normalize();

			numRes=g_pPhysicsWorld->PhysXScene->sweepMultiple(camSphere, camPose, PxVec3(motion.x, motion.y, motion.z), motionLen, PxSceneQueryFlag::eINITIAL_OVERLAP|PxSceneQueryFlag::eINITIAL_OVERLAP_KEEP|PxSceneQueryFlag::eNORMAL, sweepResults, 32, blockingHit, filter);
		}

		// second pass, check if there are any collisions in the direction from camera to head, as PhysX doesn't report collisions sometimes for other direction
		if(!firstPass)
		{
			motion = -motion;
			numRes=g_pPhysicsWorld->PhysXScene->sweepMultiple(camSphere2, camPose2, PxVec3(motion.x, motion.y, motion.z), motionLen, PxSceneQueryFlag::eINITIAL_OVERLAP|PxSceneQueryFlag::eINITIAL_OVERLAP_KEEP|PxSceneQueryFlag::eNORMAL, sweepResults, 32, blockingHit, filter);
			while(( 0 != numRes || 0 < getWaterDepthAtPos(camPos + r3dPoint3D(0, -camSphere.radius * 0.5f, 0)) ) && LoopBreaker<MaxLoopBreaker)
			{
				if(numRes == -1)
				{
					r3d_assert(false);
					break;
				}

				r3dPoint3D tmp = motion;
				tmp.Normalize();
				camPos += tmp * 0.05f;

				LoopBreaker++;

				motion = (target-camPos);
				motionLen = motion.Length();
				if(motionLen < 0.005f)
				{
					LoopBreaker = MaxLoopBreaker;
					break;
				}
				motion.Normalize();

				numRes=g_pPhysicsWorld->PhysXScene->sweepMultiple(camSphere2, camPose2, PxVec3(motion.x, motion.y, motion.z), motionLen, PxSceneQueryFlag::eINITIAL_OVERLAP|PxSceneQueryFlag::eINITIAL_OVERLAP_KEEP|PxSceneQueryFlag::eNORMAL, sweepResults, 32, blockingHit, filter);
			}
		}
	}

	if(checkCamera)
	{
		extern bool g_CameraInsidePlayer;
		if((camPos - target).Length() < 0.6f)
			g_CameraInsidePlayer = true;
		else
			g_CameraInsidePlayer = false;

		if(g_camera_mode->GetInt()==2) // in FPS mode this check not needed
			g_CameraInsidePlayer = false;

		if (gClientLogic().localPlayer_->m_ModeCreator == true)
			g_CameraInsidePlayer = false;
	}

	return (LoopBreaker == MaxLoopBreaker);
}

float g_shootCameraShakeTimer = 0.0f;
void Get_Camera_Bob(r3dPoint3D& camBob, r3dPoint3D& camUp, const obj_Player* player)
{
	r3d_assert(player);
	camBob.Assign(0,0,0);
	camUp.Assign(0,1,0);

	static float accumul = 0.0f;
	accumul += r3dGetFrameTime()*1.0f*u_GetRandom(0.75f, 1.25f);

	
	// only use this in FPS, but calculate out here.

	float wave = r3dSin(accumul) * g_shootCameraShakeTimer;
	if(g_shootCameraShakeTimer>0)
	{
		g_shootCameraShakeTimer = R3D_MAX(g_shootCameraShakeTimer-r3dGetFrameTime()*3.f, 0.0f);
	}

	if(g_camera_mode->GetInt()==2)
	{
		r3dPoint3D up(0,1,0);
		r3dPoint3D rightVector = player->m_vVision.Cross( up );
		up.RotateAroundVector(rightVector, wave*20.0f );

		camUp = up;
		
		float mStepDist = 0.1f;
		r3dPoint3D mCurPos = player->GetPosition();
		mCurPos -= player->oldstate.Position;
		float len = mCurPos.Length();

		//BP ok, make step distance the lenth of the anim
		// then len, is the current frame converted to percent :)
		r3dgameVector(r3dAnimation::r3dAnimInfo)::iterator it;
		float curframe = 0;
		float numframe = 0;
		for(it = player->uberAnim_->anim.AnimTracks.begin(); it != player->uberAnim_->anim.AnimTracks.end(); ++it) 
		{
			r3dAnimation::r3dAnimInfo &ai = *it;
			if(!(ai.dwStatus & ANIMSTATUS_Playing)) 
				continue;
			if(!(ai.dwStatus & ANIMSTATUS_Paused)) 
			{
				if(ai.pAnim && ai.pAnim->NumFrames < 60)
				{
					curframe = ai.fCurFrame;
					numframe = (float)ai.pAnim->NumFrames;
					break;
				}
			}
		}
		if(player->PlayerState == PLAYER_MOVE_SPRINT
			|| player->PlayerState == PLAYER_SWIM_FAST
			)
			mStepDist *=2;

		float mWave = 0;
		bool rightlean = true;
		if(numframe > 0)
		{
			mWave = curframe / numframe; 
			
			// want to go 0-1-0 at 0,50,100
			// want to go 0-1-0-(-1)-0 at 0,25,50,75,100
 			if(mWave >= 0.5f)
			{	
				mWave -= 0.5f;
				rightlean = false; 
			} else 
			{
				rightlean = true; 

			}

 			mWave *=2;
			// if greater than .5, subtract .5 to make 0.5
			// now go back down to zero if > .5
			if(mWave >= 0.5f)
				mWave = 1.0f - mWave;
			mWave *=2;
		}

		float boba = 0.1f;
		float rolla = 0.2f;

		// in aim mode no bob
		switch ( player->PlayerState)
		{
		case PLAYER_MOVE_WALK_AIM:
			// crouch mode no bob
		case PLAYER_MOVE_CROUCH: // intentional fallthrough
		case PLAYER_MOVE_CROUCH_AIM:// intentional fallthrough
		case PLAYER_MOVE_PRONE: // intentional fallthrough
		case PLAYER_PRONE_AIM:// intentional fallthrough
		case PLAYER_PRONE_IDLE:// intentional fallthrough
		case PLAYER_IDLE:// intentional fallthrough
		case PLAYER_IDLEAIM:// intentional fallthrough
			// no bob if turn in place or idle
			{
				mWave = 0; 
				boba = 0.0f;
			}
			break;
		// lower bob on low speed
		case PLAYER_MOVE_RUN:
			{
				boba *= .6f; // yeah this doesn't do anything, it's here in case we want to tweak it. 
				rolla = 1;
			}
			break;
		case PLAYER_MOVE_SPRINT:
			{
				boba *=1.0f; 
				rolla = 2;
			}
			break;
		// Add some motion from the ocean
		case PLAYER_SWIM_IDLE:
			{
				boba *= .3f;
				rolla = 1;
			}
			break;
		case PLAYER_SWIM_SLOW:
			{
				boba *= .4f;
				rolla = 1;
			}
			break;
		case PLAYER_SWIM:
			{
				boba *= .6f;
				rolla = 1.4f;
			}
			break;
		case PLAYER_SWIM_FAST:
			{
				boba *= 1.0f;
				rolla = 2;
			}
			break;
		}

		//boba = 0; // disable bob
		camBob.y = boba * sin(mWave * R3D_PI_2);

		r3dPoint3D p(0,1,0);

		float _angle = sin(mWave * R3D_PI_2 ) * rolla;
		if(_angle < 0)
			_angle += 360.0f;
		else if(_angle > 360.0f)
			_angle -=360.0f;
	
		if ( rightlean == false ) 
		{
			_angle = -(_angle);
		}

		p.RotateAroundZ(_angle);
		p.Normalize();
		
		// this currently will half the camUp's lean.  But the system works with this. 
		camUp = camUp + p;
		camUp.Normalize();
	}

	return;

}

static bool g_CameraPointToAdj_HasAdjustedVec = false;
static r3dPoint3D g_CameraPointToAdj_adjVec(0,0,0);
static r3dPoint3D g_CameraPointToAdj_nextAdjVec(0,0,0);

float		g_CameraLeftSideSource = -0.7f;
float		g_CameraLeftSideTarget = 1.0f;
float		g_CameraLeftSideLerp = 1.0f;

float		getCameraLeftSide()
{
	return R3D_LERP(g_CameraLeftSideSource, g_CameraLeftSideTarget, g_CameraLeftSideLerp);
}

void		updateCameraLeftSide()
{
	if(g_CameraLeftSideLerp < 1.0f)
		g_CameraLeftSideLerp = R3D_CLAMP(g_CameraLeftSideLerp+r3dGetFrameTime()*5.0f, 0.0f, 1.0f);
}

r3dPoint3D getAdjustedPointTo(obj_Player* pl, const r3dPoint3D& PointTo, const r3dPoint3D& CamPos)
{
	if(g_camera_mode->GetInt()==2)
		return R3D_ZERO_VECTOR;

	static r3dPoint3D currentLookAt(0,0,0);
	static float	  currentLookAtDist = 0.0f;
	if(LerpValue == 1.0f)
	{
		//r3dOutToLog("Lerp finished\n");
		g_CameraPointToAdj_adjVec = g_CameraPointToAdj_nextAdjVec;
	}
	else if(!g_CameraPointToAdj_HasAdjustedVec)
	{
        {
            r3dPoint3D dir;
            if(pl->m_isInScope || g_camera_mode->GetInt() != 1 )
                r3dScreenTo3D(r3dRenderer->ScreenW2, r3dRenderer->ScreenH2, &dir);
            else
                r3dScreenTo3D(r3dRenderer->ScreenW2, r3dRenderer->ScreenH*0.32f, &dir);

            PxRaycastHit hit;
            PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK|(1<<PHYSCOLL_NETWORKPLAYER), 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC);
            if(g_pPhysicsWorld->raycastSingle(PxVec3(gCam.x, gCam.y, gCam.z), PxVec3(dir.x, dir.y, dir.z), 2000.0f, PxSceneQueryFlag::eIMPACT|PxSceneQueryFlag::eDISTANCE, hit, filter))
            {
                currentLookAt.Assign(hit.impact.x, hit.impact.y, hit.impact.z);
                currentLookAtDist = hit.distance;
            }
            else
            {
                currentLookAt = CamPos + dir * 1000.0f;
                currentLookAtDist = 1000.0f;
            }
        }


		g_CameraPointToAdj_HasAdjustedVec = true;
		r3dPoint3D DestCamPos = pl->GetPosition();
		r3dPoint3D offset;
		if(pl->hasScopeMode())
		{
			offset =  r3dPoint3D( 0, (pl->getPlayerHeightForCamera() +  TargetRig.ScopePosition.Y), 0 );
			offset += pl->GetvRight() * TargetRig.ScopePosition.X;
			offset += pl->m_vVision * (TargetRig.ScopePosition.Z);
		}
		else
		{
			offset =  r3dPoint3D( 0, (pl->getPlayerHeightForCamera() +  TargetRig.Position.Y), 0 );
			offset += pl->GetvRight() * TargetRig.Position.X * getCameraLeftSide();
			offset += pl->m_vVision * (TargetRig.Position.Z);
		}

		DestCamPos += offset;

        r3dPoint3D playerPosHead = pl->GetPosition(); playerPosHead.y += pl->getPlayerHeightForCameraWithPlayerState();
        CheckCameraCollision(DestCamPos, playerPosHead, false);

		r3dPoint3D curViewVec = PointTo - CamPos;
		curViewVec.Normalize();

		r3dPoint3D destViewVec = PointTo - DestCamPos;
		destViewVec.Normalize();

		r3dPoint3D curLookAt = currentLookAt;
		if(!pl->hasScopeMode() &&  g_camera_mode->GetInt() == 1 ) // we only need this offset in offcenter mode.
		{
			float fHeight = currentLookAtDist * tan(R3D_DEG2RAD(TargetRig.FOV) * 0.5f);
			curLookAt.y -= fHeight * 0.35f;
		}
		r3dPoint3D destViewVec2 = curLookAt - DestCamPos;
		destViewVec2.Normalize();

        float d = pl->m_vVision.Dot(destViewVec2);
		if(d < 0.99f)
			destViewVec2 = destViewVec;
		//r3dOutToLog("Lerp=%.2f, pl_state=%d, aiming=%d\n", LerpValue, pl->PlayerState, pl->m_isAiming);

		static bool wasAiming = false;
		if(pl->m_isAiming)
		{
			if(!wasAiming)
			{
				wasAiming = true;
				if(currentLookAtDist < 5.0f && pl->hasScopeMode())
					g_CameraPointToAdj_nextAdjVec = r3dPoint3D(0,0.25f,0);
				else
					g_CameraPointToAdj_nextAdjVec = destViewVec2 - destViewVec;
			}
		}
		else
		{
			wasAiming = false;
			g_CameraPointToAdj_nextAdjVec = r3dPoint3D(0,0,0);
		}

        //r3dOutToLog("(%d): %.2f, %.2f\n", pl->m_isAiming, d1, d2);
		//r3dOutToLog("switching (%d): %.2f, %.2f, %.2f; %.2f\n", pl->m_isAiming, g_CameraPointToAdj_nextAdjVec.x, g_CameraPointToAdj_nextAdjVec.y, g_CameraPointToAdj_nextAdjVec.z, currentLookAtDist);
        //r3dOutToLog("vec: %.2f, %.2f, %.2f; %.2f, %.2f, %.2f\n", destViewVec.x, destViewVec.y, destViewVec.z, destViewVec2.x, destViewVec2.y, destViewVec2.z);
	}

	return R3D_LERP(g_CameraPointToAdj_adjVec, g_CameraPointToAdj_nextAdjVec, LerpValue);
}

extern float DepthOfField_NearStart;
extern float DepthOfField_NearEnd;
extern float DepthOfField_FarStart;
extern float DepthOfField_FarEnd;
extern int _FAR_DOF;
extern int _NEAR_DOF;
extern int LevelDOF;

// runs in actual game
int spectator_observingPlrIdx = 0;
r3dPoint3D spectator_cameraPos(0,0,0);
void TPSGameHUD :: SetCameraPure ( r3dCamera &Cam)
{
	if(d_video_spectator_mode->GetBool() || d_observer_mode->GetBool())
	{
		r3dPoint3D CamPos = FPS_Position;
		CamPos.Y += 1.8f;
		r3dPoint3D ViewPos = CamPos + FPS_vVision*10.0f;

		Cam.FOV = r_video_fov->GetFloat();
		Cam.SetPosition( CamPos );
		Cam.PointTo(ViewPos);

		LevelDOF = r_video_DOF_enable->GetBool();
		_NEAR_DOF = 1;
		_FAR_DOF = 1;
		DepthOfField_NearStart = r_video_nearDOF_start->GetFloat();
		DepthOfField_NearEnd = r_video_nearDOF_end->GetFloat();
		DepthOfField_FarStart = r_video_farDOF_start->GetFloat();
		DepthOfField_FarEnd = r_video_farDOF_end->GetFloat();
		
		return;
	}
	const ClientGameLogic& CGL = gClientLogic();
	obj_Player* pl = CGL.localPlayer_;
	if(pl == 0)
	{
		return;
	}

	// uav camera
	extern bool SetCameraPlayerUAV(const obj_Player* pl, r3dCamera &Cam);
	if(SetCameraPlayerUAV(pl, Cam))
	{
		FPS_Position = Cam;
		GameFOV = Cam.FOV;
		return;
	}

#ifdef VEHICLES_ENABLED
	extern bool SetCameraPlayerVehicle(const obj_Player* pl, r3dCamera &Cam);
	if(SetCameraPlayerVehicle(pl, Cam))
	{
		//r3dOutToLog("!!! Cam: Position:<%.2f,%.2f,%.2f> PointTo:<%.2f,%.2f,%.2f> Aspect:%.2f FOW:%.2f\n", Cam.x, Cam.y, Cam.z, Cam.vPointTo.x, Cam.vPointTo.y, Cam.vPointTo.z, Cam.Aspect, Cam.FOV);
		FPS_Position = Cam;
		GameFOV = Cam.FOV;
		return;
	}
#endif

	// dead camera
	if(pl->bDead)
	{
		r3dPoint3D camPos, camPointTo, camDir;
		bool do_camera = false;
		bool check_cam_collision = true;
		{
			static r3dPoint3D oldPlayerPos(0,0,0);
			static r3dPoint3D camPosOffset(0,0,0);
			camPointTo = pl->GetPosition() + r3dPoint3D(0, 0.3f, 0); // add a bit of offset, as sometimes player's pos is just a bit under ground and because of that physics will show collision and our camera will be moved under ground

			// find a cam position
			if(!oldPlayerPos.AlmostEqual(pl->GetPosition())) // make sure to do that check only once
			{
				oldPlayerPos = pl->GetPosition();
				camPosOffset = r3dPoint3D(0.0f, 0, 0.0f);
			}

			int mMX=Mouse->m_MouseMoveX, mMY=Mouse->m_MouseMoveY;
			if(g_vertical_look->GetBool()) // invert mouse
				mMY = -mMY;
			float  glb_MouseSensAdj = CurrentRig.MouseSensetivity * g_mouse_sensitivity->GetFloat();	
			//  Mouse controls are here

			float mmoveX = float(-mMX) * glb_MouseSensAdj;
			float mmoveY = float(-mMY) * glb_MouseSensAdj;

			if(hudPause && !hudPause->isActive() && Keyboard->IsPressed(kbsLeftShift) && pl->CurLoadout.Hardcore == 0)
			{
				camPosOffset.x += mmoveX;
				camPosOffset.y += mmoveY;
			}
			else if(hudPause && !hudPause->isActive() && pl->CurLoadout.Hardcore == 1)
			{
				camPosOffset.x += mmoveX;
				camPosOffset.y += mmoveY;
			}

			if(camPosOffset.x > 360.0f ) camPosOffset.x = camPosOffset.x - 360.0f;
			if(camPosOffset.x < 0.0f )   camPosOffset.x = camPosOffset.x + 360.0f;
			if(camPosOffset.y > 85 )  camPosOffset.y = 85;
			if(camPosOffset.y < -85) camPosOffset.y = -85;


			D3DXMATRIX mr;
			D3DXMatrixRotationYawPitchRoll(&mr, R3D_DEG2RAD(-camPosOffset.x), R3D_DEG2RAD(-camPosOffset.y), 0);
			camDir = r3dVector(mr._31, mr._32, mr._33);

			camPos = pl->GetPosition() - camDir*10.0f; 
			do_camera = true;
			check_cam_collision = true;
		}

		if(do_camera)
		{
			extern int g_CCBlackWhite;
			extern float g_fCCBlackWhitePwr;
			g_CCBlackWhite = 1;
			g_fCCBlackWhitePwr = R3D_CLAMP((r3dGetTime() - pl->TimeOfDeath)/2.0f, 0.0f, 1.0f); // go to black and white while look at our dead body
			
			// check for collision
			if(check_cam_collision)
			{
				CheckCameraCollision(camPos, camPointTo, false);
				if(Terrain) // weird bug with terrain. for some reason physx isn't colliding with it 
				{
					if(camPos.y < (Terrain->GetHeight(camPos)+0.5f))
						camPos.y = Terrain->GetHeight(camPos) + 0.5f;
				}
				// to prevent camera being stuck inside of player under some angles
				if((camPos-camPointTo).Length() < 1.0f)
					camPos = pl->GetPosition() - r3dPoint3D(camDir.x, -1.0f, camDir.z)*1.0f; 
			}

			Cam.FOV = 60;
			Cam.SetPosition( camPos );
			Cam.PointTo( camPointTo );
			FPS_Position = Cam;
			return;
		}
	}

	if(pl->bDead && hudAttm->isActive())
		hudAttm->Deactivate();

	if(hudPause->isActive()) return;
//	if(hudAttm->isActive()) return;//fixed bug 1267, (camera doesn't lerp hit "show attachment key" for the first time when attachments are active) 
	if(hudVault->isActive()) return;
	if(hudRepair->isActive()) return;
	if(hudCraft->isActive()) return;
	if(hudSafelock->isActive()) return;
	if(hudTrade->isActive()) return;
	if(hudStore->isActive()) return;

	int mMX=Mouse->m_MouseMoveX, mMY=Mouse->m_MouseMoveY;
	if(g_vertical_look->GetBool()) // invert mouse
		mMY = -mMY;

	GameFOV = CurrentRig.FOV;

	r3dPoint3D CamPos = pl->GetPosition();
	CamPos.Y +=  (pl->getPlayerHeightForCamera()+  CurrentRig.Position.Y);
	updateCameraLeftSide();
	CamPos += pl->GetvRight() * CurrentRig.Position.X * getCameraLeftSide();
	CamPos += pl->m_vVision * CurrentRig.Position.Z;

	r3dPoint3D playerPos = pl->GetPosition();
	r3dPoint3D PointTo = playerPos;
	PointTo.Y += (pl->getPlayerHeightForCamera() + TPSHudCameraTarget.Y);
	PointTo += pl->GetvRight() * TPSHudCameraTarget.X;

	// check for collision
	{
		r3dPoint3D playerPosHead = playerPos; playerPosHead.y += pl->getPlayerHeightForCameraWithPlayerState();
		r3dPoint3D savedCamPos = CamPos;
		if(CheckCameraCollision(CamPos, playerPosHead, true) && (pl->PlayerState == PLAYER_MOVE_CROUCH || pl->PlayerState == PLAYER_MOVE_CROUCH_AIM || pl->PlayerState == PLAYER_MOVE_PRONE || pl->PlayerState == PLAYER_PRONE_AIM || pl->PlayerState == PLAYER_PRONE_IDLE)) 
		{
			CamPos = savedCamPos;
			playerPosHead = playerPos;
			playerPosHead.y += pl->getPlayerHeightForCameraWithPlayerState()-0.8f;
			CheckCameraCollision(CamPos, playerPosHead, true);
		}
	}

	PointTo += (pl->m_vVision) * 50;//cameraRayLen;//CurrentRig.Target.Z;

	r3dPoint3D adjPointTo(0,0,0);
	adjPointTo = getAdjustedPointTo(pl, PointTo, CamPos);

	r3dPoint3D camBob, camUp;
	Get_Camera_Bob(camBob, camUp, pl);

	

	//set camera editor mode
	if (pl->m_ModeCreator == true) // mode creator
	{
		r3dVector CamPos2 = pl->GetPosition();
		CamPos2 += r3dPoint3D( 0, ( 5 ), 0 );

		D3DXMATRIX mr;
		D3DXMatrixRotationYawPitchRoll(&mr, R3D_DEG2RAD(pl->GetRotationVector().x), R3D_DEG2RAD(15.0f), 0);

		r3dVector ForwardVector = r3dVector(mr ._31, mr ._32, mr ._33);

		CamPos2 += -ForwardVector * 4 ;
		Cam.SetPosition(CamPos2);
		Cam.PointTo( CamPos2 + ForwardVector * 3 + r3dVector ( 0, -1, 0) );
		Cam.vUP = r3dPoint3D(0, 1, 0);
		Cam.FOV = CurrentRig.FOV;
	}
	else {
		Cam.FOV = GameFOV;
		Cam.SetPosition( CamPos + camBob);
		Cam.PointTo(PointTo + camBob);
		Cam.vUP = camUp;
	}

	Cam.vPointTo += adjPointTo;

#ifndef FINAL_BUILD
	if( g_pHUDCameraEffects )
	{
		g_pHUDCameraEffects->Update( &Cam, CamPos ) ;
	}
#endif

	FPS_Position = Cam;
}  

static void DrawMenus()
{
	if(d_video_spectator_mode->GetBool() && !d_observer_mode->GetBool()) // no UI in spectator mode
		return;
#ifndef FINAL_BUILD
	if(d_disable_game_hud->GetBool())
		return;
#endif

#if 0
	typedef r3dgameVector(r3dSTLString) stringlist_t;
	extern stringlist_t currentMovies ;

	typedef r3dgameVector( float ) floats ;
	extern floats movieDurations ;

	char buff[ 512 ] ;
	sprintf( buff, "%d - Num Drawcalls", r3dRenderer->Stats.NumDraws );

	currentMovies.push_back( buff );
	movieDurations.push_back( 0.1f );

	typedef r3dgameVector( int ) sorties ;
	static sorties ss ;

	ss.resize( movieDurations.size() );

	for( int i = 0, e = movieDurations.size(); i < e; i ++ )
	{
		ss[ i ] = i ;
	}

	for( int i = 0, e = movieDurations.size(); i < e; i ++ )
	{
		for( int j = 0, e = movieDurations.size() - 1 ; j < e; j ++ )
		{
			if( movieDurations[ ss[ j ] ] > movieDurations[ ss[ j + 1 ] ] )
			{
				std::swap( ss[ j ], ss[ j + 1 ] );
			}
		}
	}

	r3dSetFiltering( R3D_POINT );

	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, 	FALSE );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHAREF,        	1 );

	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

	for( int i = 0, e = (int)currentMovies.size(); i < e; i ++ )
	{
		Font_Label->PrintF(r3dRenderer->ScreenW - 330, r3dRenderer->ScreenH-e*22 - 220 + i*22,r3dColor(255,255,255), "%.1f - %s", movieDurations[ ss[ i ] ] * 1000.f, currentMovies[ ss[ i ] ].c_str() );
	}

	currentMovies.clear();
	movieDurations.clear();

	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, 	FALSE );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHAREF,        	1 );

	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);
#endif
	/*
	if(win::bSuspended && hudMain->isChatInputActive())
	{
		// if suspended, remove chat input focus to prevent HUD lock up
		hudMain->forceChatInputDisabled();
		hudMain->showChat(hudMain->isChatVisible(), true);
	}
	*/

	if(!win::bSuspended && hudMain->isChatInputActive())
	{
		bool escPressed = Keyboard->WasPressed(kbsEsc);
		if(escPressed)
		{
			hudMain->hideGravestone();
			hudMain->hideReadNote();
			hudMain->hideWriteNote();
			hudMain->hideSafelockPin();
		}
	}

	if(!win::bSuspended && !hudMain->isChatInputActive() && !hudMain->isPlayersListVisible()) 
	{
		bool showHudPause = Keyboard->WasPressed(kbsEsc);
		if(!hudPause->isActive() || (hudPause->isActive() && !hudPause->isDisabledHotKeys()))
			showHudPause = showHudPause || InputMappingMngr->wasReleased(r3dInputMappingMngr::KS_SWITCH_MINIMAP) || InputMappingMngr->wasReleased(r3dInputMappingMngr::KS_INVENTORY);

		if(showHudPause && !hudAttm->isActive() && !hudVault->isActive() && !hudStore->isActive() && !hudSafelock->isActive() && !hudTrade->isActive() && !hudRepair->isActive() && !hudCraft->isActive())
		{
			if(!hudPause->isActive())
			{
				hudPause->Activate();
				if(InputMappingMngr->wasReleased(r3dInputMappingMngr::KS_SWITCH_MINIMAP))
					hudPause->showMap();
				else if(InputMappingMngr->wasReleased(r3dInputMappingMngr::KS_INVENTORY))
					hudPause->showInventory();
			}
			else
				hudPause->Deactivate();
		}

		if((Keyboard->WasPressed(kbsEsc)) && hudVault->isActive())
			hudVault->Deactivate();

		if((Keyboard->WasPressed(kbsEsc)) && hudRepair->isActive())
			hudRepair->Deactivate();

		bool showCraft = InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_SHOW_CRAFT_MENU);
		if(showCraft && !hudAttm->isActive() && !hudVault->isActive() && !hudStore->isActive() && !hudSafelock->isActive() && !hudTrade->isActive() && !hudRepair->isActive() && !hudPause->isActive())
		{
			if(!hudCraft->isActive())
				hudCraft->Activate();
			else
				hudCraft->Deactivate();
		}

#ifdef ENABLE_INGAME_STORE
		bool showStore = InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_SHOW_STORE);
		if(showStore && !hudAttm->isActive() && !hudVault->isActive() && !hudSafelock->isActive() && !hudTrade->isActive() && !hudRepair->isActive())
		{
			if(!hudStore->isActive())
			{
				if(hudPause->isActive())
					hudPause->Deactivate();

				hudStore->Activate();
			}
			else
				hudStore->Deactivate();
		}
#endif

		if((Keyboard->WasPressed(kbsEsc)) && hudCraft->isActive())
			hudCraft->Deactivate();

		if((Keyboard->WasPressed(kbsEsc)) && hudSafelock->isActive())
			hudSafelock->Deactivate();
		
		//DO NOT Deactivate() trade by escape
		if((Keyboard->WasPressed(kbsEsc)) && hudTrade->isActive())
			hudTrade->requestClose();

		if((Keyboard->WasPressed(kbsEsc)) && hudStore->isActive())
			hudStore->Deactivate();

		bool showAttachment = InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_SHOW_ATTACHMENTS);

#ifdef VEHICLES_ENABLED
		if (gClientLogic().localPlayer_ && gClientLogic().localPlayer_->IsInVehicle() && showAttachment)
			showAttachment = false;
#endif

		if(showAttachment)
		{
			if(!hudAttm->isActive())
			{
				hudAttm->Activate();
			}
			else
				hudAttm->Deactivate();
		}
		if(hudAttm->isActive() && Keyboard->WasPressed(kbsEsc))
			hudAttm->Deactivate();

#ifdef MISSIONS
		bool showMissionHUD = false;
		obj_Player* pl = gClientLogic().localPlayer_; // can be null
		if( pl && !hudCraft->isActive() && !hudAttm->isActive() && !hudVault->isActive() && !hudStore->isActive() && !hudSafelock->isActive() && !hudTrade->isActive() && !hudRepair->isActive() && !hudPause->isActive() )
		{
			if( InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_TOGGLE_MISSION_HUD) )
			{
				if( pl->missionsActionData.size() > 0 )
				{
					if( !hudMain->isMissionHUDVisible() )
						hudMain->showMissionHUD();
					else
						hudMain->hideMissionHUD();
				}
				else
				{
					hudMain->showMessage(gLangMngr.getString("HUD_Msg_NoActiveMissions"));
				}
			}
			pl->UpdateMissionAreaRestrictions();
			showMissionHUD = hudMain->isMissionHUDVisible();
		}
#endif
	}

	if(hudPause->isActive())
	{
		r3dMouse::Show(); // make sure that mouse is visible

		R3DPROFILE_START( "hudPause->" );

		hudPause->Update();
		hudPause->Draw();

		R3DPROFILE_END( "hudPause->" );

		return;
	}
	
	if(hudVault->isActive())
	{
		r3dMouse::Show(); // make sure that mouse is visible

		R3DPROFILE_START( "hudVault->" );

		hudVault->Update();
		hudVault->Draw();

		R3DPROFILE_END( "hudVault->" );

		return;
	}

	if(hudRepair->isActive())
	{
		r3dMouse::Show(); // make sure that mouse is visible

		R3DPROFILE_START( "hudRepair->" );

		hudRepair->Update();
		hudRepair->Draw();

		R3DPROFILE_END( "hudRepair->" );

		return;
	}

	if(hudCraft->isActive())
	{
		r3dMouse::Show(); // make sure that mouse is visible

		R3DPROFILE_START( "hudCraft->" );

		hudCraft->Update();
		hudCraft->Draw();

		R3DPROFILE_END( "hudCraft->" );

		return;
	}

	if(hudSafelock->isActive())
	{
		r3dMouse::Show(); // make sure that mouse is visible

		R3DPROFILE_START( "hudSafelock->" );

		hudSafelock->Update();
		hudSafelock->Draw();

		R3DPROFILE_END( "hudSafelock->" );

		return;
	}
	
	if(hudTrade->isActive())
	{
		r3dMouse::Show(); // make sure that mouse is visible

		R3DPROFILE_START( "hudTrade->" );

		hudTrade->Update();
		hudTrade->Draw();

		R3DPROFILE_END( "hudTrade->" );

		return;
	}
	
	if(hudStore->isActive())
	{
		r3dMouse::Show(); // make sure that mouse is visible

		R3DPROFILE_START( "hudStore->" );

		hudStore->Update();
		hudStore->Draw();

		R3DPROFILE_END( "hudStore->" );

		return;
	}

	if(hudAttm->isActive())
	{
		r3dMouse::Show(); // make sure that mouse is visible

		R3DPROFILE_START( "hudAttm->" );

		hudAttm->Update();
		hudAttm->Draw();

		R3DPROFILE_END( "hudAttm->" );

		return;
	}

	if(hudActionUI->isActive())
	{
		R3DPROFILE_START( "hudActionUI->" );
		hudActionUI->Update();
		hudActionUI->Draw();
		R3DPROFILE_END( "hudActionUI->" );
	}

	bool ChatWindowSwitch = InputMappingMngr->wasReleased(r3dInputMappingMngr::KS_CHAT);

	const ClientGameLogic& CGL = gClientLogic();
	const obj_Player* pl = CGL.localPlayer_; // can be null
	if(pl == NULL) // no player, we need to show respawn menu and let player enter game
	{
	}
	else
	{
		if(ChatWindowSwitch && hudMain && !hudMain->isChatInputActive() && !hudMain->isPlayersListVisible() && r_render_in_game_HUD->GetBool() && pl->uavViewActive_==false)
		{
			hudMain->showChatInput("");
		}

		if(hudMain)
		{
			if(InputMappingMngr->wasReleased(r3dInputMappingMngr::KS_CHAT_CHANNEL1) && r_render_in_game_HUD->GetBool() && !(hudMain && hudMain->isSafelockPinActive()))
				hudMain->setChatChannel(0);
			if(InputMappingMngr->wasReleased(r3dInputMappingMngr::KS_CHAT_CHANNEL2) && r_render_in_game_HUD->GetBool() && !(hudMain && hudMain->isSafelockPinActive()))
				hudMain->setChatChannel(1);
			if(InputMappingMngr->wasReleased(r3dInputMappingMngr::KS_CHAT_CHANNEL3) && r_render_in_game_HUD->GetBool() && !(hudMain && hudMain->isSafelockPinActive()))
				hudMain->setChatChannel(2);
			if(InputMappingMngr->wasReleased(r3dInputMappingMngr::KS_CHAT_CHANNEL4) && r_render_in_game_HUD->GetBool() && !(hudMain && hudMain->isSafelockPinActive()))
				hudMain->setChatChannel(3);
		}

		if(hudMain && !hudMain->isChatInputActive() && !hudMain->isPlayersListVisible())
		{
			if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_TURN_OFF_HUD))
				r_render_in_game_HUD->SetBool(!r_render_in_game_HUD->GetBool());
		}
		if(hudMain && !hudMain->isChatInputActive())
		{
			bool tabPressed = InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_SHOW_PLAYERS);
			bool escPressed = Keyboard->WasPressed(kbsEsc);
			if(tabPressed && !hudMain->isPlayersListVisible() && pl->isGameHardcore == false) //gamehardcore
			{
				hudMain->clearPlayersList();
				int index = 0;
				for(int i=0; i<R3D_ARRAYSIZE(CGL.playerNames); i++)
				{
					if(CGL.playerNames[i].Gamertag[0] && CGL.playerNames[i].DevIsHide == false)
					{
						hudMain->addPlayerToList(i, index++, (void*)&CGL.playerNames[i]);
					}
				}
				hudMain->showPlayersList(1);
			}
			else if((tabPressed||escPressed) && hudMain->isPlayersListVisible())
				hudMain->showPlayersList(0);
		}

		// render flash UI for objects

		R3DPROFILE_START( "GameWorld().Draw(rsDrawFlashUI)" );
		GameWorld().Draw(rsDrawFlashUI);
		R3DPROFILE_END( "GameWorld().Draw(rsDrawFlashUI)" );

		{
			{
				R3DPROFILE_START( "hudMain->" );

				hudMain->Update();
				hudMain->Draw();

				R3DPROFILE_END( "hudMain->" );
			}

			// issue d3d cheat check on some frames (will stop issuing anti cheat if caught cheat)
			// wait 5 minute before doing check. after that, do check every other 2-5 minutes
			VMPROTECT_BeginMutation("screenshot check");
			if(!pl->bDead && (r3dGetTime() - pl->TimeOfLastRespawn)>300.0f && !hudAttm->isActive() && !hudPause->isActive() && !hudActionUI->isActive() && 
				!hudVault->isActive() && !hudStore->isActive() && !hudSafelock->isActive() && !hudTrade->isActive() && !hudRepair->isActive() && !hudCraft->isActive())
			{
				static float nextCheck = r3dGetTime() + u_GetRandom(120.0f, 300.0f);
				if(r3dGetTime() > nextCheck)
				{
					issueD3DAntiCheatCodepath( ANTICHEAT_WALLHACK );
					nextCheck = r3dGetTime() + u_GetRandom(120.0f, 300.0f);
				}
// 				static float nextCheck2 = r3dGetTime() + u_GetRandom(300.0f, 600.0f);
// 				if(r3dGetTime() > nextCheck2)
// 				{
// 					issueD3DAntiCheatCodepath( ANTICHEAT_SCREEN_HELPERS2 );
// 					nextCheck2 = r3dGetTime() + u_GetRandom(300.0f, 600.0f);
// 				}
			}
			VMPROTECT_End();
		}

		if(hudMain && (hudMain->isChatInputActive() || hudMain->isPlayersListVisible())) // also checks for write note, so do not hide mouse
			return;

		// draw main hud with hidden mouse
		// this call is FREE if mouse was hidden already
		// [denis]: do not remove for now, this is minor hack for situation when app was started inactive. 
		// [pavel]: that fucks up controls, when big map is on screen, or scoreboard is, you shouldn't be able to move character, as in that mode you are actually using mouse
		//			if app was started inactive, just press M twice and that's it. 
		// [pavel]: ok, that should fix a problem. If non of modal windows are active, then hide mouse.
		if(!win::bSuspended && !g_cursor_mode->GetInt())
		{
			if (gClientLogic().localPlayer_)// respawn fast
			{
				if (gClientLogic().localPlayer_->bDead==0)
					r3dMouse::Hide();
			}
			else
				r3dMouse::Hide();
		}
	}
}

void TPSGameHUD :: Draw()
{
	if(!TPSGameHud_Inited) r3dError("!TPSGameHud_Inited");

	assert(bInited);
	if ( !bInited ) return;

	R3DPROFILE_D3DSTART( D3DPROFILE_SCALEFORM ) ;

	r3dSetFiltering( R3D_POINT );

	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, 	FALSE );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHAREF,        	1 );

	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);

#ifndef FINAL_BUILD
	if( d_print_postfx->GetInt() )
	{
		g_pPostFXChief->DEBUG_PrintPostFXes();
	}
#endif

	DrawMenus();

	R3DPROFILE_D3DEND( D3DPROFILE_SCALEFORM ) ;

	return;  
}

static void ProcessUAVMovement(obj_UAV* uav, int mMX, int mMY)
{
	float sens = 0.3f * g_mouse_sensitivity->GetFloat();

	uav->ViewAngle.x += float(-mMX) * sens;
	uav->ViewAngle.y += float(-mMY) * sens;

	if(Gamepad->IsConnected()) // overwrite mouse
	{
		float X, Y;
		Gamepad->GetRightThumb(X, Y);
		uav->ViewAngle.x += float(-X) * sens * r_gamepad_view_sens->GetFloat();
		uav->ViewAngle.y += float(Y) * sens * r_gamepad_view_sens->GetFloat() * (g_vertical_look->GetBool()?-1.0f:1.0f);
	}


	if(uav->ViewAngle.x > 360.0f ) uav->ViewAngle.x = uav->ViewAngle.x - 360.0f;
	if(uav->ViewAngle.x < 0.0f )   uav->ViewAngle.x = uav->ViewAngle.x + 360.0f;
	if(uav->ViewAngle.y > 0 ) uav->ViewAngle.y = 0;
	if(uav->ViewAngle.y < -80) uav->ViewAngle.y = -80;

	D3DXMATRIX mr;
	D3DXMatrixRotationYawPitchRoll(&mr, R3D_DEG2RAD(-uav->ViewAngle.x), R3D_DEG2RAD(-uav->ViewAngle.y), 0);
	const r3dPoint3D vVision  = r3dVector(mr._31, mr._32, mr._33);
	const r3dPoint3D vForw    = r3dVector(mr._31, 0, mr._33).NormalizeTo(); // need to be normalized, otherwise UAV speed depends on your camera view angle
	const r3dPoint3D vRight   = r3dVector(mr._11, 0, mr._13);
	const r3dPoint3D vUp      = r3dVector(0, 1, 0);

	uav->vVision = vVision;
	
	r3dPoint3D acc = r3dPoint3D(0, 0, 0);
	if(InputMappingMngr->isPressed(r3dInputMappingMngr::KS_MOVE_FORWARD)) acc.z = GPP->UAV_FLY_SPEED_V;
	if(InputMappingMngr->isPressed(r3dInputMappingMngr::KS_MOVE_BACKWARD)) acc.z = -GPP->UAV_FLY_SPEED_V;
	if(InputMappingMngr->isPressed(r3dInputMappingMngr::KS_MOVE_LEFT)) acc.x = -GPP->UAV_FLY_SPEED_V;
	if(InputMappingMngr->isPressed(r3dInputMappingMngr::KS_MOVE_RIGHT)) acc.x = GPP->UAV_FLY_SPEED_V;
	if(InputMappingMngr->isPressed(r3dInputMappingMngr::KS_UAV_UP)) acc.y = GPP->UAV_FLY_SPEED_H;
	if(InputMappingMngr->isPressed(r3dInputMappingMngr::KS_UAV_DOWN)) acc.y = -GPP->UAV_FLY_SPEED_H;

	if(Gamepad->IsConnected())
	{
		float RX, RY, TL, TR;
		Gamepad->GetLeftThumb(RX, RY);
		Gamepad->GetTrigger(TL, TR);

		acc.Z = -RY*r_gamepad_move_speed->GetFloat();
		acc.Z = RY*r_gamepad_move_speed->GetFloat();
		acc.X = -RX*r_gamepad_move_speed->GetFloat();
		acc.X = RX*r_gamepad_move_speed->GetFloat();
		acc.Y = -TR * r_gamepad_move_speed->GetFloat();
		acc.Y = TL * r_gamepad_move_speed->GetFloat();
	}

	
	r3dPoint3D vel = r3dPoint3D(0, 0, 0);
	vel += vForw  * acc.z;
	vel += vRight * acc.x;
	vel += vUp    * acc.y;
	if(vel.Length() > 0.001f)
		uav->SetVelocity(vel);
	
	return;
}

bool SetCameraPlayerUAV(const obj_Player* pl, r3dCamera &Cam)
{
	if(!pl->uavViewActive_)
		return false;

	obj_UAV* uav = (obj_UAV*)GameWorld().GetObject(pl->uavId_);
	if(!uav)
		return false;

	r3dPoint3D Pos = uav->GetPosition()+r3dPoint3D(0,0.2,0); /*+r3dPoint3D(0,-1,0)*/;
	Cam.FOV = 35;
	Cam.SetPosition(Pos);
	if (!(hudPause && hudPause->isActive()))
		Cam.PointTo(Pos + uav->vVision);
	Cam.vUP = r3dPoint3D(0, 1, 0);
	return true;
}


bool SetCameraPlayerVehicle(const obj_Player* pl, r3dCamera &Cam)
{
	static bool wasDrivenByPlayer = false;

#ifdef VEHICLES_ENABLED
	if (gClientLogic().localPlayer_ && gClientLogic().localPlayer_->IsInVehicle())
	{
		if (!g_pPhysicsWorld->m_VehicleManager->hasDrivableCar)
			return false;

		obj_Vehicle* vehicle = g_pPhysicsWorld->m_VehicleManager->GetDrivenCar()->owner;
		if (!vehicle)
		{
			wasDrivenByPlayer = false;
		}
		else
		{
			bool enableMoveCam = false;

			if (InputMappingMngr->isPressed(r3dInputMappingMngr::KS_PRIMARY_FIRE) && !(hudPause && hudPause->isActive()) && !(hudMain && hudMain->isPlayersListVisible()) && !(hudMain && hudMain->isChatInputActive()))
			{
				enableMoveCam = true;		
			}

			if (vehicle->isTank || vehicle->isHelicopter)
				enableMoveCam=true;

			if (InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_CONSTRUCTOR) && gClientLogic().localPlayer_->seatPosition == 0 && !(hudPause && hudPause->isActive()) && !(hudMain && hudMain->isPlayersListVisible()) && !(hudMain && hudMain->isChatInputActive()))
			{
				if (vehicle->CreateParams.vehicleType != obj_Vehicle::VEHICLETYPE_HELICOPTER && vehicle->CreateParams.vehicleType != obj_Vehicle::VEHICLETYPE_TANK_T80)
				{
					if (r3dGetTime() > vehicle->m_LastClaxonUse)
					{
						vehicle->SendActions(2);
						vehicle->m_LastClaxonUse = r3dGetTime() + 3.0f;
					}
				}
			}

			if (InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_SWITCH_FPS_TPS) && !(hudPause && hudPause->isActive()) && !(hudMain && hudMain->isPlayersListVisible()) && !(hudMain && hudMain->isChatInputActive()))
			{
				switch(vehicle->TypeCamera)
				{
					case 0: // distant camera
							vehicle->TypeCamera = 1;
							break;
					case 1: // nearest camera
							vehicle->TypeCamera = 2;
							break;
					case 2: // interior camera
							vehicle->TypeCamera = 0;
							break;

				}
				if ((vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_HELICOPTER ||
					vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_HUMMER) && vehicle->TypeCamera == 1)
					vehicle->TypeCamera = 2;

				if ((vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_ABANDONEDSUV ||
					vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_DUNEBUGGY ||
					vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_HELICOPTER ||
					vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_HUMMER ||
					vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_JEEP ||
					vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_POLICE) && vehicle->TypeCamera == 2)
				{
					vehicle->enableInteriorCam = !vehicle->enableInteriorCam;
					gClientLogic().localPlayer_->m_enableRendering = !vehicle->enableInteriorCam;
				}
				else {
					if (vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_HUMMER || 
						vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_ABANDONEDSUV ||
						vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_DUNEBUGGY ||
						vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_JEEP ||
						vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_HELICOPTER ||
						vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_POLICE)
							gClientLogic().localPlayer_->m_enableRendering = true;
						else 
							gClientLogic().localPlayer_->m_enableRendering = false;
					vehicle->enableInteriorCam = false;
				}
			}

			if (enableMoveCam && !vehicle->enableInteriorCam)
			{
					r3dVector CamPos = vehicle->GetPosition();
					CamPos += r3dPoint3D( 0, ( 5 ), 0 );

					int mMX=Mouse->m_MouseMoveX, mMY=Mouse->m_MouseMoveY;
					float  glb_MouseSensAdj = CurrentRig.MouseSensetivity * g_mouse_sensitivity->GetFloat();

					static float camangle = 0;
					static float camangle2 = 0;
					
					camangle += float(-mMX) * glb_MouseSensAdj;
					camangle2 += float(-mMY) * glb_MouseSensAdj;

					if(camangle2 > 5.0f ) camangle2 = 5.0f;
					if(camangle2 < -30.0f )   camangle2 = -30.0f;

					vehicle->camangle = -camangle;

					D3DXMATRIX mr;
					D3DXMatrixRotationYawPitchRoll(&mr, R3D_DEG2RAD(-camangle), R3D_DEG2RAD(-camangle2), 0);

					r3dVector vehicleForwardVector = r3dVector(mr ._31, mr ._32, mr ._33);

					CamPos += -vehicleForwardVector * 10 ;
					Cam.SetPosition(CamPos);
					Cam.PointTo( CamPos + vehicleForwardVector * 3 + r3dVector ( 0, -1, 0) );
					Cam.vUP = r3dPoint3D(0, 1, 0);
					Cam.FOV = CurrentRig.FOV;
			}
			else if (vehicle->enableInteriorCam)
			{
					obj_Player* pl = gClientLogic().localPlayer_;
					r3dVector CamPos = pl->GetPosition();
					if (vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_POLICE)
						CamPos += r3dPoint3D( 0, ( 0.8 ), 0 );
					else
						CamPos += r3dPoint3D( 0, ( 0.7 ), 0 );

					D3DXMATRIX mr;
					D3DXMatrixRotationYawPitchRoll(&mr, R3D_DEG2RAD(vehicle->GetRotationVector().x), R3D_DEG2RAD(vehicle->GetRotationVector().y), R3D_DEG2RAD(vehicle->GetRotationVector().z));

					r3dVector Rotation = vehicle->GetvForw(); //r3dVector(mr ._31, mr ._32, mr ._33);

					Cam.SetPosition(CamPos);
					Cam.PointTo( CamPos + Rotation);
					Cam.vUP = r3dPoint3D(0, 1, 0);
					Cam.FOV = CurrentRig.FOV;
			}
			else {

				if ((vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_ABANDONEDSUV ||
					vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_DUNEBUGGY ||
					vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_ZOMBIEKILLER ||
					vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_COPCAR ||
					vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_JEEP ||
					vehicle->CreateParams.vehicleType == obj_Vehicle::VEHICLETYPE_POLICE) && vehicle->TypeCamera != 2)
				{
					g_pPhysicsWorld->m_VehicleManager->ConfigureCamera(Cam,vehicle->TypeCamera);
				}
				else {
					g_pPhysicsWorld->m_VehicleManager->ConfigureCamera(Cam,0);
				}
			}
			wasDrivenByPlayer = true;
		}
	}
	else
	{
		wasDrivenByPlayer = false;
	}
#endif

	return wasDrivenByPlayer;
}

static float g_lastAimAnimTime = -1.f;

bool g_AutoRunToggled = false;

void ProcessCamera(obj_Player* pl)
{
	ActiveCameraRigID = (Playerstate_e)pl->PlayerState;
	ActiveCameraRig   = TPSHudCameras[g_camera_mode->GetInt()][ActiveCameraRigID];
	
	// use this to update the camera from the options. 
	static int currentCameraMode = g_camera_mode->GetInt();

	// if we arn't in the correct view mode currently.   And we are not doing a aim zoom, or the previous lerp is done. 
	if ( ( CurrentState != pl->PlayerState || currentCameraMode != g_camera_mode->GetInt()) && (LerpValue >= 1.0f || ( !pl->m_isAiming) ) )
	{
		currentCameraMode = g_camera_mode->GetInt();
		//set new target
		SourceRig = CurrentRig;
		TargetRig = ActiveCameraRig;

		if(SourceRig.Position.AlmostEqual(TargetRig.Position) && SourceRig.ScopePosition.AlmostEqual(TargetRig.ScopePosition))
		{
			// workaround for a quickscoping and firing at the same time and causing a camera to tilt up.
			if((TPSHudCameras[g_camera_mode->GetInt()][CurrentState].allowScope && !TargetRig.allowScope) || (!TPSHudCameras[g_camera_mode->GetInt()][CurrentState].allowScope && TargetRig.allowScope))
			{
				g_CameraPointToAdj_HasAdjustedVec = false;
			}
		}
		else
		{
			g_CameraPointToAdj_adjVec = R3D_LERP(g_CameraPointToAdj_adjVec, g_CameraPointToAdj_nextAdjVec, LerpValue);
			g_CameraPointToAdj_HasAdjustedVec = false;
		}

		LerpValue = 0;

		CurrentState = (Playerstate_e)pl->PlayerState;	
	}
	else
	{
		// just lerp
		if (LerpValue < 1.0f)
		{
			const Weapon* wpn = pl->m_Weapons[pl->m_SelectedWeapon];

			float lerpMOD = 1.0f;
			STORE_CATEGORIES equippedItemCat = wpn ? wpn->getCategory() : storecat_INVALID;;
			if(TargetRig.allowScope) // slow down aiming for those categories
				if(equippedItemCat == storecat_MG)
					lerpMOD = 0.5f;

			LerpValue += r3dGetFrameTime()*6.5f*lerpMOD;
			if (LerpValue >1.0f) LerpValue = 1.0f;

			CurrentRig.Lerp(pl, SourceRig, TargetRig, LerpValue);
		}
		else
			CurrentRig.Lerp(pl, SourceRig, TargetRig, 1.0f);
	}
}

void ProcessPlayerMovement(obj_Player* pl, bool editor_debug )
{
	r3d_assert(pl->NetworkLocal);

#ifdef VEHICLES_ENABLED
	if( !g_bEditMode )
	{
		// chris temp comment:
		// temporarily left in comment so that I don't forget to actually apply the WANTED code
		// the idea behind this is to stop the player from being able to do certain things
		// like pick up items, or control the vehicle while typing, etc

		if (pl->IsInVehicle())
		{
			if(hudMain->isChatInputActive()) return;
			if(hudMain->isPlayersListVisible()) return;
			if(hudPause->isActive()) return;
			if(hudVault->isActive()) return;
			if(hudRepair->isActive()) return;
			if(hudCraft->isActive()) return;
			if(hudSafelock->isActive()) return;
			if(hudTrade->isActive()) return;
			if(hudStore->isActive()) return;
			if(hudAttm->isActive()) return;
			if(pl->uavViewActive_) return; // nuevo

			if (InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_INTERACT))
			{
				obj_Vehicle* vehicle = g_pPhysicsWorld->m_VehicleManager->GetDrivenCar()->owner;
				if (!vehicle)
					return;

				gClientLogic().localPlayer_->RequestVehicleExit(vehicle->vehicleId);
			}
			ProcessCamera(pl);
		}

		if (pl->IsInVehicle())
			return;
	}
#endif

	// check fire weapon should be called all the time, as it will reset weapon fire in case if you are sitting on the menu, etc      
	{
		R3DPROFILE_FUNCTION("update fire");
		pl->CheckFireWeapon();
	}

	if (pl->m_ModeCreator == true)
		return;

	static float proningUpDownLastAction = 0.0f;
	bool stillPlayingProningAnim = false;
	if((r3dGetTime()-proningUpDownLastAction)<0.9f)
		stillPlayingProningAnim = true;

	if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_SWITCH_FPS_TPS) && !(hudAttm && hudAttm->isActive()) && !(hudMain && (hudMain->isChatInputActive() || hudMain->isPlayersListVisible())) && !Mouse->GetMouseVisibility() && !stillPlayingProningAnim)
	{
		pl->switchFPS_TPS();
	}

	r3dPoint3D prevAccel = pl->InputAcceleration;
	pl->InputAcceleration.Assign(0, 0, 0);
	
	static int shiftWasPressed = 0;
	float movingSpeed = pl->plr_local_moving_speed * (1.0f/r3dGetFrameTime());

	// query mouse distance, so it will not be accumulated
	int mMX=Mouse->m_MouseMoveX, mMY=Mouse->m_MouseMoveY;
	if(g_vertical_look->GetBool()) // invert mouse
		mMY = -mMY;

	bool disablePlayerRotation = false;
	bool disablePlayerMovement = false;

	// exit for uav view
	if(pl->uavViewActive_)
	{
		if(Keyboard->WasPressed(kbsEsc) || InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_INTERACT))
		{
			Keyboard->ClearPressed(); // eat up ESC key so exit game screen will not be activated
			pl->ToggleUAVView();
		}
	}

	// uav first time spawn check, automatically switch
	if(pl->uavRequested_ == 1 && pl->uavId_ != invalidGameObjectID && GameWorld().GetObject(pl->uavId_))
	{
		pl->ToggleUAVView();
		pl->uavRequested_ = 2;
	}
	// uav control
	if(pl->uavViewActive_)
	{
		// UAV might be destroyed at the moment
		obj_UAV* uav = (obj_UAV*)GameWorld().GetObject(pl->uavId_);
		if(uav)
		{
			ProcessUAVMovement(uav, mMX, mMY);
		}

		disablePlayerMovement = true; // do not exit from function, let it go, as otherwise character will be stuck in prev.state
		return;
	}

	if(
#ifdef VEHICLES_ENABLED
		pl->IsInVehicle() ||
#endif
		Mouse->GetMouseVisibility() || (hudMain && (hudMain->isChatInputActive() || hudMain->isPlayersListVisible()))) // do not update player if we are in menu control mode!
	{	
		disablePlayerMovement = true;
		disablePlayerRotation = true;
	}
	if(pl->bDead)
		return;

	const Weapon* wpn = pl->m_Weapons[pl->m_SelectedWeapon];

	if(!pl->m_isInScope)
	{
		if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_SWITCH_TPS_CAMERA_SIDE) && !(hudVault && hudVault->isActive()) && 
			!(hudCraft && hudCraft->isActive())&& !(hudRepair && hudRepair->isActive()) && !(hudStore && hudStore->isActive()) && 
			!(hudSafelock && hudSafelock->isActive()))//cynthia: 1785
		{
			R3D_SWAP(g_CameraLeftSideSource, g_CameraLeftSideTarget);
			g_CameraLeftSideLerp = 0.0f;
		}
	}

		if(wpn->getItemID() == WeaponConfig::ITEMID_Cypher2 && !pl->uavViewActive_) // UAV
		{
			if(InputMappingMngr->wasReleased(r3dInputMappingMngr::KS_PRIMARY_FIRE))
			{
				if(pl->uavRequested_ == 0) // spawn UAV firstly
				{
					pl->ProcessSpawnUAV();
				}
				else if(pl->uavRequested_ == 3)
				{
					if(hudMain) hudMain->showMessage(gLangMngr.getString("$HUD_Msg_UAVOffline"));
				}
				else {
						pl->ToggleUAVView();
					}

				
			}
		}

	bool  aiming      = pl->m_isAiming;
	bool  swimming    = pl->IsSwimming();
	int   playerState = swimming ? PLAYER_SWIM_IDLE : aiming ? PLAYER_IDLEAIM : PLAYER_IDLE;

	if( g_lastAimAnimTime < 0 )
		g_lastAimAnimTime = r3dGetTime();

	float newAimTime = r3dGetTime();
	float deltaAimTime = newAimTime - g_lastAimAnimTime;
	g_lastAimAnimTime = newAimTime;

	const float AIM_LERP_SPEED = 8.0f;

	float lerpT = R3D_MIN( AIM_LERP_SPEED * deltaAimTime, 1.0f );

	if( aiming )
	{
		r_grass_zoom_coef->SetFloat( R3D_LERP( r_grass_zoom_coef->GetFloat(), 2.0f, lerpT ) );
	}
	else
	{
		r_grass_zoom_coef->SetFloat( R3D_LERP( r_grass_zoom_coef->GetFloat(), 1.0f, lerpT ) );
	}

	if(!(g_camera_mode->GetInt()==2 && pl->NetworkLocal))
	{
		if(pl->IsJumpActive()) 
		{
			// in jump, keep current state  (so strafe will stay, for example) and disable movement
			playerState = pl->PlayerState;
		}
	}
	
	// not able to sprint with equipped RPG
	bool disableSprint = false;
	r3dAnimation::r3dAnimInfo* animInfo = pl->uberAnim_->anim.GetTrack(pl->uberAnim_->grenadeThrowTrackID);
	if(!(pl->uberAnim_->grenadePinPullTrackID==CUberAnim::INVALID_TRACK_ID && !(animInfo && (animInfo->GetStatus()&ANIMSTATUS_Playing))))
		disableSprint = true;

	if(pl->CurLoadout.Health < 10.0f)
		disableSprint = true;

	// check if player can straighten up, in case if there is something above his head he will not be able to stop crouching
	bool force_crouch = false;
	if(pl->bCrouch)
	{
		PxBoxGeometry bbox(0.2f, 0.5f, 0.2f);
		PxTransform pose(PxVec3(pl->GetPosition().x, pl->GetPosition().y+1.6f, pl->GetPosition().z), PxQuat(0,0,0,1));
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC);
		PxShape* shape;
		force_crouch = g_pPhysicsWorld->PhysXScene->overlapAny(bbox, pose, shape, filter);
	}
	bool crouching = pl->bCrouch;

	const float crouchStopDepth = 1.0f;
	float waterDepth;
	float waterLevel;
	if(pl->fHeightAboveGround < 0.5f && !(!swimming && pl->IsInWater(waterDepth, waterLevel) && waterDepth >= crouchStopDepth))
	{
		if(g_toggle_crouch->GetBool())
		{
			if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_CROUCH) || Gamepad->WasReleased(gpB))
				crouching = !crouching;
		}
		else
			crouching = InputMappingMngr->isPressed(r3dInputMappingMngr::KS_CROUCH) || Gamepad->IsPressed(gpB);
	}
	else
		crouching = false;

	if(disablePlayerMovement)
		crouching = false;

	if(force_crouch)
		crouching = true;

	if(crouching) 
		playerState = swimming ? PLAYER_SWIM_SLOW : aiming ? PLAYER_MOVE_CROUCH_AIM : PLAYER_MOVE_CROUCH;
	// check if player can straighten up, in case if there is something above his head he will not be able to stop proning
	bool force_prone = false;
	bool wasProning = pl->bProne;
	bool proning = pl->bProne;
	if(!swimming)
	{
		if(proning)
		{
			PxBoxGeometry bbox(0.2f, 0.5f, 0.2f);
			PxTransform pose(PxVec3(pl->GetPosition().x, pl->GetPosition().y+1.6f, pl->GetPosition().z), PxQuat(0,0,0,1));
			PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK, 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC);
			PxShape* shape;
			force_prone = g_pPhysicsWorld->PhysXScene->overlapAny(bbox, pose, shape, filter);
		}
		if(pl->fHeightAboveGround < 0.5f)
		{
			if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_PRONE) && !disablePlayerMovement)
				proning = !proning;
		}
		else
			proning = false;

		if(force_prone)
			proning = true;

		{
			extern float getWaterDepthAtPos(const r3dPoint3D& pos);
			float waterDepth = getWaterDepthAtPos(pl->GetPosition());
			if(waterDepth > 0.5f)
				proning = false;
		}

		if(proning)
			crouching = false;

		if(proning) 
		{
			if(!wasProning)
			{
				playerState = PLAYER_PRONE_DOWN;
				proningUpDownLastAction = r3dGetTime();
			}
			else if(wasProning && pl->PlayerState == PLAYER_PRONE_DOWN) // check if we are still playing anim
			{
				bool stillPlayingAnim = false;
				r3dgameVector(r3dAnimation::r3dAnimInfo)::iterator it;
				for(it=pl->uberAnim_->anim.AnimTracks.begin(); it!=pl->uberAnim_->anim.AnimTracks.end(); ++it) 
				{
					const r3dAnimation::r3dAnimInfo& ai = *it;
					if(ai.pAnim->iAnimId == pl->uberAnim_->data_->aid_.prone_down_weapon || ai.pAnim->iAnimId == pl->uberAnim_->data_->aid_.prone_down_noweapon)
					{
						if(!(ai.dwStatus & ANIMSTATUS_Finished))
							stillPlayingAnim = true;
						break;
					}
				}
				// in FPS mode we don't play lower animations, so in FPS mode proning up and down is immediate. Instead, added this timer. Probably can remove animation check above as well
				if(stillPlayingProningAnim)
					stillPlayingAnim = true;
			
				if(stillPlayingAnim)
					playerState = PLAYER_PRONE_DOWN;
				else
					playerState = aiming ? PLAYER_PRONE_AIM : PLAYER_PRONE_IDLE;

			}
			else
				playerState = aiming ? PLAYER_PRONE_AIM : PLAYER_PRONE_IDLE;
		}
		else
		{
			if(wasProning)
			{
				playerState = PLAYER_PRONE_UP;
				proningUpDownLastAction = r3dGetTime();
			}
			else if(pl->PlayerState == PLAYER_PRONE_UP) // check if we are still playing anim
			{
				bool stillPlayingAnim = false;
				r3dgameVector(r3dAnimation::r3dAnimInfo)::iterator it;
				for(it=pl->uberAnim_->anim.AnimTracks.begin(); it!=pl->uberAnim_->anim.AnimTracks.end(); ++it) 
				{
					const r3dAnimation::r3dAnimInfo& ai = *it;
					if(ai.pAnim->iAnimId == pl->uberAnim_->data_->aid_.prone_up_weapon || ai.pAnim->iAnimId == pl->uberAnim_->data_->aid_.prone_up_noweapon)
					{
						if(!(ai.dwStatus & ANIMSTATUS_Finished))
							stillPlayingAnim = true;
						break;
					}
				}
				// in FPS mode we don't play lower animations, so in FPS mode proning up and down is immediate. Instead, added this timer. Probably can remove animation check above as well
				if(stillPlayingProningAnim)
					stillPlayingAnim = true;

				if(stillPlayingAnim)
					playerState = PLAYER_PRONE_UP;
			}
		}

		if(playerState == PLAYER_PRONE_UP || playerState == PLAYER_PRONE_DOWN)
		{
			disablePlayerMovement = true;
			disablePlayerRotation = true;
		}

		if(proning && aiming)
			disablePlayerMovement = true;
	}

	VMPROTECT_BeginMutation("ProcessPlayerMovement_Accel");	
	{

		r3dPoint3D accelaration(0,0,0);
		if(!disablePlayerMovement)
		{
			// if facing a wall and cannot sprint - stop sprint
			bool canSprint = (shiftWasPressed<3) || (shiftWasPressed>=3 && movingSpeed > 1.0f);
			if(InputMappingMngr->isPressed(r3dInputMappingMngr::KS_SPRINT) || Gamepad->IsPressed(gpLeftShoulder))
				shiftWasPressed++;
			else
				shiftWasPressed = 0;

			if(aiming || pl->m_isHoldingBreath) // cannot spring and aim. also, in default key binding spring and hold breath are on the same key
				shiftWasPressed = 0;

			// due to animation, firstly check left and right movement, so that if you move diagonally we will play moving forward animation
			float thumbX, thumbY;
			Gamepad->GetLeftThumb(thumbX, thumbY);
			if(InputMappingMngr->isPressed(r3dInputMappingMngr::KS_MOVE_LEFT)) 
			{
				accelaration += (aiming)?r3dPoint3D(-GPP->AI_WALK_SPEED*GPP->AI_BASE_MOD_SIDE,0,0):r3dPoint3D(-GPP->AI_RUN_SPEED*GPP->AI_BASE_MOD_SIDE,0,0);
			}
			else if(InputMappingMngr->isPressed(r3dInputMappingMngr::KS_MOVE_RIGHT)) 
			{
				accelaration += (aiming)?r3dPoint3D(GPP->AI_WALK_SPEED*GPP->AI_BASE_MOD_SIDE,0,0):r3dPoint3D(GPP->AI_RUN_SPEED*GPP->AI_BASE_MOD_SIDE,0,0);
			}
			else if(thumbX!=0.0f)
			{
				accelaration += (aiming)?r3dPoint3D(GPP->AI_WALK_SPEED*GPP->AI_BASE_MOD_SIDE*thumbX,0,0):r3dPoint3D(GPP->AI_RUN_SPEED*GPP->AI_BASE_MOD_SIDE*thumbX,0,0);
			}

			//r3dOutToLog("sprint: %d, canSprint: %d, disable: %d, speed: %.3f, swimming: %d\n", (int)shiftWasPressed, (int)canSprint, (int)disableSprint, movingSpeed, (int)swimming);
			if(shiftWasPressed && canSprint /*&& pl->bOnGround*/ && !crouching && !proning && !disableSprint && (pl->m_Stamina>0.0f) && pl->m_StaminaPenaltyTime<=0 ) 
			{
				playerState = swimming ? PLAYER_SWIM_FAST : PLAYER_MOVE_SPRINT;
				accelaration *= 0.5f; // half side movement when sprinting
				accelaration += r3dPoint3D(0,0,GPP->AI_SPRINT_SPEED);
				accelaration  = accelaration.NormalizeTo() * GPP->AI_SPRINT_SPEED;
			}
			else
			{
				if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_MOVE_FORWARD_TOGGLE))
					g_AutoRunToggled = !g_AutoRunToggled;
				if(g_AutoRunToggled)
				{
					if(InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_MOVE_FORWARD) || 
					   InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_MOVE_BACKWARD) )
					   g_AutoRunToggled = false;
				}

				if((g_AutoRunToggled || InputMappingMngr->isPressed(r3dInputMappingMngr::KS_MOVE_FORWARD)) || shiftWasPressed) 
				{
					float spd = GPP->AI_BASE_MOD_FORWARD * (aiming ? GPP->AI_WALK_SPEED : GPP->AI_RUN_SPEED);
					accelaration += r3dPoint3D(0,0,spd);
					accelaration  = accelaration.NormalizeTo() * spd;
				}
				else if(InputMappingMngr->isPressed(r3dInputMappingMngr::KS_MOVE_BACKWARD))
				{
					float spd = GPP->AI_BASE_MOD_BACKWARD * (aiming ? GPP->AI_WALK_SPEED : GPP->AI_RUN_SPEED);
					accelaration += r3dPoint3D(0,0,-spd);
					accelaration  = accelaration.NormalizeTo() * spd;
				}
				else if(thumbY!=0.0f)
				{
					if(thumbY>0)
						accelaration += (aiming)?r3dPoint3D(0,0,GPP->AI_WALK_SPEED*GPP->AI_BASE_MOD_FORWARD*thumbY):r3dPoint3D(0,0,GPP->AI_RUN_SPEED*GPP->AI_BASE_MOD_FORWARD*thumbY);
					else
						accelaration += (aiming)?r3dPoint3D(0,0,GPP->AI_WALK_SPEED*GPP->AI_BASE_MOD_BACKWARD*thumbY):r3dPoint3D(0,0,GPP->AI_RUN_SPEED*GPP->AI_BASE_MOD_BACKWARD*thumbY);
				}
			}

			// set walk/run state
			if(playerState != PLAYER_MOVE_SPRINT && !crouching && !proning && (accelaration.x || accelaration.z)
				&& playerState != PLAYER_SWIM_FAST)
				playerState = swimming ? PLAYER_SWIM : aiming ? PLAYER_MOVE_WALK_AIM : PLAYER_MOVE_RUN;
			// set prone walk state
			if(playerState != PLAYER_MOVE_SPRINT && !crouching && proning && (accelaration.x || accelaration.z)
				&& playerState != PLAYER_SWIM_FAST
				)
				playerState = aiming ? PLAYER_PRONE_AIM : PLAYER_MOVE_PRONE;

			if((playerState == PLAYER_MOVE_RUN || playerState == PLAYER_MOVE_SPRINT
				|| playerState == PLAYER_SWIM || playerState == PLAYER_SWIM_FAST
				) && (r3dGetTime() < pl->m_SpeedBoostTime))
			{
				accelaration *= pl->m_SpeedBoost;
			}
			
#ifndef FINAL_BUILD
			if(gUserProfile.ProfileData.isDevAccount && Keyboard->IsPressed(kbsLeftAlt))
				accelaration *= 5.0f;
#endif

			// 		STORE_CATEGORIES equippedItemCat = wpn ? wpn->getCategory() : storecat_INVALID;;
			// 		if(equippedItemCat == storecat_SUPPORT || equippedItemCat == storecat_MG)
			// 			accelaration *= 0.8f; // 20% slow down
		}


		if(pl->CurLoadout.Health < GPP->c_fSpeedMultiplier_LowHealthLevel)
			accelaration *= GPP->c_fSpeedMultiplier_LowHealthValue;
		if(pl->CurLoadout.Thirst > GPP->c_fSpeedMultiplier_HighThirstLevel)
			accelaration *= GPP->c_fSpeedMultiplier_HighThirstValue;
		if(pl->CurLoadout.Hunger > GPP->c_fSpeedMultiplier_HighHungerLevel)
			accelaration *= GPP->c_fSpeedMultiplier_HighHungerValue;

		// IMPORTANT: 0.4f and 0.2f should be synced on server in bool obj_ServerPlayer::CheckForFastMove() 
		if(crouching)
			accelaration *= 0.4f;
		if(proning)
			accelaration *= 0.2f;

		/*if(pl->IsJumpActive()) // don't allow to change direction when jumping
		pl->InputAcceleration = prevAccel;
		else*/
		pl->InputAcceleration = accelaration;

		// process jump after assigning InputAcceleration, so that we can predict where player will jump
		if(!disablePlayerMovement)
		{
			if(pl->bOnGround && (InputMappingMngr->wasPressed(r3dInputMappingMngr::KS_JUMP)||Gamepad->WasPressed(gpA)) 
				&& !crouching 
				&& !proning
				&& !swimming
				&& !pl->IsJumpActive()
				&& prevAccel.z >= 0 /* prevent jump backward*/
				)
			{
				pl->StartJump();
			}

		}

		if(!editor_debug)
			pl->PlayerState   = playerState;

		pl->PlayerMoveDir = CUberData::GetMoveDirFromAcceleration(pl->InputAcceleration);

	} VMPROTECT_End();	

	// adjust player physx controller size
	// TODO: we need to adjust size only when animation blending was finished! ask Denis how.
	if(crouching != pl->bCrouch || proning!=pl->bProne)
	{
		if(crouching || proning)
			pl->PhysicsObject->AdjustControllerSize(0.3f, 0.8f, 0.8f);
		else
			pl->PhysicsObject->AdjustControllerSize(0.3f, 1.4f, 1.1f);
	}
	
	pl->bCrouch = crouching;
	pl->bProne = proning;

	ProcessCamera(pl); // camara player

	if(!disablePlayerRotation)
	{
		float  glb_MouseSensAdj = CurrentRig.MouseSensetivity * g_mouse_sensitivity->GetFloat();	

#if !defined(FINAL_BUILD)
		if( r_do_hackerzoom->GetInt() )
		{
			glb_MouseSensAdj /= sqrtf( r_hackerzoom->GetFloat() );
		}
#endif
		//  Mouse controls are here

		float mmoveX = float(-mMX) * glb_MouseSensAdj;
		float mmoveY = float(-mMY) * glb_MouseSensAdj;

		// fight only vertical recoil, apply adjustment leftover to viewvector
		if(pl->RecoilViewModTarget.y > 0.01f && mmoveY < 0) {
			pl->RecoilViewModTarget.y += mmoveY;
			if(pl->RecoilViewModTarget.y < 0) {
				mmoveY = pl->RecoilViewModTarget.y;
				pl->RecoilViewModTarget.y = 0;
			} else {
				mmoveY = 0;
			}
		}

		pl->ViewAngle.x += mmoveX;
		pl->ViewAngle.y += mmoveY;

		if(Gamepad->IsConnected()) // overwrite mouse
		{
			float X, Y;
			Gamepad->GetRightThumb(X, Y);
			pl->ViewAngle.x += float(-X) * glb_MouseSensAdj * r_gamepad_view_sens->GetFloat();
			pl->ViewAngle.y += float(Y) * glb_MouseSensAdj * r_gamepad_view_sens->GetFloat() * (g_vertical_look->GetBool()?-1.0f:1.0f);
		}

		if(pl->ViewAngle.x > 360.0f ) pl->ViewAngle.x = pl->ViewAngle.x - 360.0f;
		if(pl->ViewAngle.x < 0.0f )   pl->ViewAngle.x = pl->ViewAngle.x + 360.0f;

		// Player can't look too high!
		if(pl->ViewAngle.y > CurrentRig.LookUpLimit )  pl->ViewAngle.y = CurrentRig.LookUpLimit;
		if(pl->ViewAngle.y < CurrentRig.LookDownLimit) pl->ViewAngle.y = CurrentRig.LookDownLimit;

		// set player rotation (except when planting mines)
		pl->m_fPlayerRotationTarget = -pl->ViewAngle.x;

		// calculate player vision
		r3dVector FinalViewAngle = pl->ViewAngle + pl->RecoilViewMod + pl->SniperViewMod;
		if(FinalViewAngle.x > 360.0f ) FinalViewAngle.x = FinalViewAngle.x - 360.0f;
		if(FinalViewAngle.x < 0.0f )   FinalViewAngle.x = FinalViewAngle.x + 360.0f;
		// Player can't look too high!
		if(FinalViewAngle.y > CurrentRig.LookUpLimit )  FinalViewAngle.y = CurrentRig.LookUpLimit;
		if(FinalViewAngle.y < CurrentRig.LookDownLimit) FinalViewAngle.y = CurrentRig.LookDownLimit;

		D3DXMATRIX mr;
		D3DXMatrixRotationYawPitchRoll(&mr, R3D_DEG2RAD(-FinalViewAngle.x), R3D_DEG2RAD(-FinalViewAngle.y), 0);
		pl->m_vVision  = r3dVector(mr._31, mr._32, mr._33);
	}

	if (TargetRig.FXFunc) TargetRig.FXFunc(LerpValue);

	pl->UpdateLocalPlayerMovement();
}

//----------------------------------------------------------------
void TPSGameHUD :: Process()
//----------------------------------------------------------------
{
	if( g_cursor_mode->GetInt() )
	{
		imgui_Update();
		imgui2_Update();
	}

	{
		r3dSetAsyncLoading( 1 ) ;
	}

	obj_Player* pl = gClientLogic().localPlayer_;
	if(!pl) return;

#ifndef FINAL_BUILD
	bool allow_specator_mode = true;
#else
	bool allow_specator_mode = gUserProfile.CustomerID==1415858 && (gClientLogic().m_gameInfo.gameServerId==134216 || gClientLogic().m_gameInfo.gameServerId==148646); //MMZ
#endif

	if(Keyboard->WasPressed(kbsF8) && allow_specator_mode)
	{
		d_video_spectator_mode->SetBool(!d_video_spectator_mode->GetBool());
		static float DOF_NS=0, DOF_NE=0, DOF_FS=0, DOF_FE=0;
		static int DOF_N=0, DOF_F=0, DOF_ENABLE=0;
		if(d_video_spectator_mode->GetBool())
		{
			FPS_vViewOrig.Assign(pl->ViewAngle);
			// save
			DOF_NS=DepthOfField_NearStart;
			DOF_NE=DepthOfField_NearEnd;
			DOF_FS=DepthOfField_FarStart;
			DOF_FE=DepthOfField_FarEnd;
			DOF_N=_NEAR_DOF;
			DOF_F=_FAR_DOF;
			DOF_ENABLE=LevelDOF;
		}
		else
		{
			// restore
			DepthOfField_NearStart=DOF_NS;
			DepthOfField_NearEnd=DOF_NE;
			DepthOfField_FarStart=DOF_FS;
			DepthOfField_FarEnd=DOF_FE;
			_NEAR_DOF=DOF_N;
			_FAR_DOF=DOF_F;
			LevelDOF=DOF_ENABLE;
		}
	}

	bool allow_observer_mode = false;
	if(Keyboard->WasPressed(kbsF9) && allow_observer_mode)
	{
		d_observer_mode->SetBool(!d_observer_mode->GetBool());
		if(d_observer_mode->GetBool())
		{
			FPS_vViewOrig.Assign(pl->ViewAngle);
		}
	}

	if(d_video_spectator_mode->GetBool() || d_observer_mode->GetBool())
	{
		FPS_Acceleration.Assign(0, 0, 0);

		float  glb_MouseSensAdj = g_mouse_sensitivity->GetFloat();

#if !defined(FINAL_BUILD)
		if( r_do_hackerzoom->GetInt() )
		{
			glb_MouseSensAdj /= sqrtf( r_hackerzoom->GetFloat() );
		}
#endif

		// camera view
		if(Gamepad->IsConnected())
		{
			float X, Y;
			Gamepad->GetRightThumb(X, Y);
			FPS_vViewOrig.x += float(-X) * r_gamepad_view_sens->GetFloat();
			FPS_vViewOrig.y += float(Y) * r_gamepad_view_sens->GetFloat() * (g_vertical_look->GetBool()?-1.0f:1.0f);
		}
		else // mouse fallback
		{
			int mMX=Mouse->m_MouseMoveX, mMY=Mouse->m_MouseMoveY;

			FPS_vViewOrig.x += float(-mMX) * glb_MouseSensAdj;
			FPS_vViewOrig.y += float(-mMY) * glb_MouseSensAdj * (g_vertical_look->GetBool()?-1.0f:1.0f);
		}

		if(FPS_vViewOrig.y > 85)  FPS_vViewOrig.y = 85;
		if(FPS_vViewOrig.y < -85) FPS_vViewOrig.y = -85;

		FPS_ViewAngle = FPS_vViewOrig;

		if(FPS_ViewAngle.y > 360 ) FPS_ViewAngle.y = FPS_ViewAngle.y - 360;
		if(FPS_ViewAngle.y < 0 )   FPS_ViewAngle.y = FPS_ViewAngle.y + 360;


		D3DXMATRIX mr;

		D3DXMatrixIdentity(&mr);
		D3DXMatrixRotationYawPitchRoll(&mr, R3D_DEG2RAD(-FPS_ViewAngle.x), R3D_DEG2RAD(-FPS_ViewAngle.y), 0);

		FPS_vVision  = r3dVector(mr._31, mr._32, mr._33);

		D3DXMatrixIdentity(&mr);
		D3DXMatrixRotationYawPitchRoll(&mr, R3D_DEG2RAD(-FPS_ViewAngle.x), 0, 0);
		FPS_vRight = r3dVector(mr._11, mr._12, mr._13);
		FPS_vUp    = r3dVector(0, 1, 0);
		FPS_vForw  = r3dVector(mr._31, mr._32, mr._33);

		FPS_vForw.Normalize();
		FPS_vRight.Normalize();
		FPS_vVision.Normalize();

		// walk
		float fSpeed = r_editor_move_speed->GetFloat();

		float mult = 1;
		if(Keyboard->IsPressed(kbsLeftShift)) mult = d_spectator_fast_move_mul->GetFloat();
		if(Keyboard->IsPressed(kbsLeftControl)) mult = d_spectator_slow_move_mul->GetFloat();

		if(Keyboard->IsPressed(kbsW)) FPS_Acceleration.Z = fSpeed;
		if(Keyboard->IsPressed(kbsS)) FPS_Acceleration.Z = -fSpeed * 0.7f;
		if(Keyboard->IsPressed(kbsA)) FPS_Acceleration.X = -fSpeed * 0.7f;
		if(Keyboard->IsPressed(kbsD)) FPS_Acceleration.X = fSpeed * 0.7f;
		if(Keyboard->IsPressed(kbsQ)) FPS_Position.Y    += SRV_WORLD_SCALE(1.0f)* r3dGetFrameTime() * mult;
		if(Keyboard->IsPressed(kbsE)) FPS_Position.Y    -= SRV_WORLD_SCALE(1.0f)* r3dGetFrameTime() * mult;

		if(Gamepad->IsConnected())
		{
			float RX, RY, TL, TR;
			Gamepad->GetLeftThumb(RX, RY);
			Gamepad->GetTrigger(TL, TR);

			if(RY>0.01f)
				FPS_Acceleration.Z += RY*r_gamepad_move_speed->GetFloat();
			else if(RY<-0.01f)
				FPS_Acceleration.Z += RY*r_gamepad_move_speed->GetFloat();
			if(RX>0.01f)
				FPS_Acceleration.X += RX*r_gamepad_move_speed->GetFloat();
			else if(RX<-0.01f)
				FPS_Acceleration.X += RX*r_gamepad_move_speed->GetFloat();
			FPS_Position.Y    += r3dGetFrameTime() * TR * r_gamepad_move_speed->GetFloat();
			FPS_Position.Y    -= r3dGetFrameTime() * TL * r_gamepad_move_speed->GetFloat();
		}

		FPS_Position += FPS_vVision * FPS_Acceleration.Z * r3dGetFrameTime() * mult;
		FPS_Position += FPS_vRight * FPS_Acceleration.X * r3dGetFrameTime() *mult;

		return;
	}
	
	ProcessPlayerMovement(pl, false);
}