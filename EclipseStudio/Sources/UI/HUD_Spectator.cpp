#include "r3dPCH.h"
#include "r3d.h"
#include "d3dfont.h"
#include "d3dfont.h"

#include "../SF/Console/Config.h"
#include "UI\Hud_spectator.h"
#include "Editors\LevelEditor.h"

#include "ObjectsCode\Gameplay\obj_ZombieDummy.h"
#include "ObjectsCode\Gameplay\obj_DeerDummy.h"
#include "..\Editors\LevelEditor_Collections.h"

#include "GameCommon.h"
#include "GameLevel.h"

// selected objectID
gobjid_t	UI_SelectedObjID	= invalidGameObjectID;
// on-cursor selection
r3dPoint3D	UI_TargetPos;		// snapped to object position (object center)
r3dPoint3D	UI_TargetNorm;
r3dPoint3D	UI_TargetPos2;		// unsnapped position
r3dPoint3D	UI_TerraTargetPos;
gobjid_t	UI_TargetObjID		= invalidGameObjectID;
r3dMaterial *UI_TargetMaterial = NULL;
int			UI_TargetObjectAdditionalInfo = 0;

float GameFOV = VIEW_FOV;

static int enablemouselook = 1;

#include "Editors\LevelEditor.h"


BaseHUD  		*Hud = NULL;


CameraHUD::CameraHUD()
: customTextToShow ( NULL )
{

}

CameraHUD::~CameraHUD()
{ 
}

void CameraHUD :: InitPure()
{
	CameraMode = hud_TopdownCamera;

	UI_SelectedObjID = invalidGameObjectID;
	UI_TargetObjID   = invalidGameObjectID;
	UI_TargetPos     = r3dPoint3D(0, 0, 0);
	FPS_Position.Assign(0,0,0);

#ifndef FINAL_BUILD
	ReadXMLEditorSettingsStartPosition( &FPS_Position );
#endif

	if ( Terrain && Terrain->IsLoaded() )
	{
		float h = terra_GetH(FPS_Position);
		if(FPS_Position.Y < h) FPS_Position.Y = h;

		// otherwise terrain update fails
		if( FPS_Position.X < 0 )
			FPS_Position.X = 0 ;

		if( FPS_Position.Z < 0 )
			FPS_Position.Z = 0 ;

		const r3dTerrainDesc& desc = Terrain->GetDesc() ;

		float totalW = desc.XSize ;
		float totalH = desc.ZSize ;

		if( FPS_Position.X > totalW )
			FPS_Position.X = totalW ;

		if( FPS_Position.Z > totalH )
			FPS_Position.Z = totalH ;
	}

	FPS_Acceleration.Assign(0,0,0);
	FPS_vViewOrig.Assign(0,0,0);
	FPS_ViewAngle.Assign(0,0,0);
	FPS_vVision.Assign(0, 0, 1);

	TPS_DistanceToTarget = 15.0f;
	TPS_Target.Assign(0, 0, 0);
	TPS_SideOffset = 0.0f;
	TPS_LockedDistance = 0.0f;
}



void CameraHUD :: DestroyPure()
{
	//	floatTextMgrDestroy();
}

void CameraHUD :: SetCameraDir (r3dPoint3D vPos )
{
	FPS_vVision = vPos;
	FPS_vVision.Normalize();

	r3dVector v1 = FPS_vVision;
	v1.y = 0;
	v1.Normalize();

	FPS_vViewOrig.x = - R3D_RAD2DEG(atan2 ( v1.x, v1.z ));
	FPS_vViewOrig.y = R3D_RAD2DEG(asin ( FPS_vVision.y ));
	FPS_vViewOrig.z = 0;
	FPS_ViewAngle = FPS_vViewOrig;
}

r3dPoint3D CameraHUD :: GetCameraDir () const
{
	return FPS_vVision;
}


void CameraHUD :: SetCameraPure ( r3dCamera &Cam)
{
	switch (CameraMode)
	{
	case hud_FPSCamera:
	case hud_TPSCamera:
	case hud_TopdownCamera:
	default:
		SetFPSCamera(Cam);
		break;

	}

}  

extern int  g_usePlayerEditorCamera;
BaseHUD* editor_GetHudByIndex(int index);

void CameraHUD :: SetFPSCamera(r3dCamera &Cam )
{
	if(g_usePlayerEditorCamera)
		return editor_GetHudByIndex(1)->SetCamera(Cam);

	r3dPoint3D CamPos = FPS_Position;
	CamPos += GetCamOffset();

	r3dPoint3D ViewPos = CamPos + FPS_vVision*10.0f;

#ifndef FINAL_BUILD
	if(d_spectator_fov->GetFloat() > 0.0f)
		GameFOV = d_spectator_fov->GetFloat();
	else
#endif
		GameFOV = VIEW_FOV;

	Cam.FOV = GameFOV;
	Cam.SetPosition( CamPos );
	Cam.PointTo(ViewPos);
}


void CameraHUD :: Draw()
{
	assert(bInited);

	if ( !bInited ) return;

	r3dSetFiltering( R3D_POINT );

	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, 	FALSE );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHAREF,        	1 );

	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

	//	floatTextMgrDraw();

	//Font_Label->PrintF(20,20,r3dColor(255,255,255), "fps %f\n%fms\n%d", 1.0f/r3dGetFrameTime(), r3dGetFrameTime()*1000.0f,Mod);

#ifndef FINAL_BUILD
	GameObject* TargetObj = GameWorld().GetObject(UI_TargetObjID);
	if (r_hide_icons->GetInt() == 0)
	{
		if(TargetObj)
		{
			Font_Label->PrintF(20,88,r3dColor(255,255,255), "OBJ: %s", TargetObj->Name.c_str());
		}

		if (customTextToShow)
		{
			Font_Label->PrintF(420,20,r3dColor(255,255,255), customTextToShow);
		}
	}
#endif


#ifndef FINAL_BUILD
	LevelEditor.Process(!enablemouselook && !d_disable_editor_gui->GetBool());
#endif

	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, 	FALSE );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHAREF,        	1 );
}

/*virtual*/
r3dPoint3D
CameraHUD :: GetCamOffset() const /*OVERRIDE*/
{
	return r3dPoint3D( 0.f, 1.8f, 0.f );
}


void CameraHUD :: ProcessCheats()
{
}



//----------------------------------------------------------------
void CameraHUD :: Process()
//----------------------------------------------------------------
{
	ProcessCheats();

	// alt-8 spawn test players for sergey
	if(Keyboard->WasPressed(kbs8) && (Keyboard->IsPressed(kbsLeftAlt) || Keyboard->IsPressed(kbsRightAlt)))
	{
		extern void CreateDummyEditorPlayer();
		CreateDummyEditorPlayer();
	}

	// alt-9 spawn test zombie
	if(Keyboard->WasPressed(kbs9) && (Keyboard->IsPressed(kbsLeftAlt) || Keyboard->IsPressed(kbsRightAlt)))
	{
		obj_ZombieDummy* z = (obj_ZombieDummy *)srv_CreateGameObject("obj_ZombieDummy", "Zombie", UI_TargetPos + r3dPoint3D(0, 0.01f, 0));
	}

	// alt-0 spawn test deer
	if(Keyboard->WasPressed(kbs0) && (Keyboard->IsPressed(kbsLeftAlt) || Keyboard->IsPressed(kbsRightAlt)))
	{
		obj_DeerDummy* d = (obj_DeerDummy *)srv_CreateGameObject("obj_DeerDummy", "Deer", UI_TargetPos + r3dPoint3D(0, 0.01f, 0));
	}

	switch (CameraMode)
	{
	case hud_FPSCamera:
	case hud_TPSCamera:
	case hud_TopdownCamera:
	default:
		ProcessFPSCamera();
		break;
	}

	if (!enablemouselook)
		ProcessPick();

	return;
}



void CameraHUD :: ProcessFPSCamera()
{
#ifndef FINAL_BUILD
	FPS_Acceleration.Assign(0, 0, 0);

	float	glb_MouseSens    = 0.5f;	// in range (0.1 - 1.0)
	float  glb_MouseSensAdj = 1.0f;	// in range (0.1 - 1.0)


	enablemouselook = (Mouse->IsPressed(r3dMouse::mRightButton)) && !g_imgui_LockRbr;


	if (!enablemouselook && !d_force_mouse_movement_editor->GetBool()) return;


	//
	//  Mouse controls are here
	//
	int mMX=Mouse->m_MouseMoveX, mMY=Mouse->m_MouseMoveY;

	FPS_vViewOrig.x += float(-mMX) * glb_MouseSensAdj;
	FPS_vViewOrig.y += float(-mMY) * glb_MouseSensAdj;

	if(Gamepad->IsConnected()) // overwrite mouse
	{
		float X, Y;
		Gamepad->GetRightThumb(X, Y);
		FPS_vViewOrig.x += float(-X) * glb_MouseSensAdj * r_gamepad_view_sens->GetFloat();
		FPS_vViewOrig.y += float(Y) * glb_MouseSensAdj * r_gamepad_view_sens->GetFloat() * (g_vertical_look->GetBool()?-1.0f:1.0f);
	}

	// Player can't look too high!
	if(FPS_vViewOrig.y > 85)  FPS_vViewOrig.y = 85;
	if(FPS_vViewOrig.y < -85) FPS_vViewOrig.y = -85;

	FPS_ViewAngle = FPS_vViewOrig; // + FPS_vViewDistortion;

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

	if (CameraMode == hud_TPSCamera)
	{
		ProcessTPSCamera();
		return;
	}

	// walk
	float fSpeed = r_editor_move_speed->GetFloat();

	float mult = 1;
	if(Keyboard->IsPressed(kbsLeftShift)) mult = 30.0f; //64.0f;

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

	//if (FPS_Position.Y < terra_GetH(FPS_Position)) FPS_Position.Y = terra_GetH(FPS_Position);

	if(Keyboard->WasPressed(kbsF6))
	{
		r3dPoint3D PP = FPS_Position + FPS_vVision*60.0f;

		r3dOutToLog("POINT at %f %f %f\n", PP.X, PP.Y, PP.Z);

		GameObject *Car = srv_CreateGameObject("obj_Phys", "Data\\Models\\Objects\\Box01.sco", PP);
	}

#endif
}

void CameraHUD::ProcessTPSCamera()
{
#ifndef FINAL_BUILD
	FPS_Position = TPS_Target - FPS_vVision * TPS_DistanceToTarget + FPS_vRight * TPS_SideOffset;

	if (TPS_LockedDistance > 0)
	{
		TPS_DistanceToTarget = TPS_LockedDistance;
	}
	else
	{
		int mouseScroll = Mouse->m_WheelScroll;
		TPS_DistanceToTarget -= mouseScroll * 0.01f;
	}
#endif
}

bool hud_ProcessCameraPick(float mx, float my)
{
	R3DPROFILE_FUNCTION( "hud_ProcessCameraPick" ) ;

	float vx, vy, vw, vh;
	r3dRenderer->GetBackBufferViewport( &vx, &vy, &vw, &vh );

	mx = mx - vx;
	my = my - vy;

	r3dPoint3D dir;
	r3dScreenTo3D(mx, my, &dir);

	UI_TargetPos      = r3dPoint3D(0, 99999, 0); 
	UI_TargetObjID    = invalidGameObjectID;
	UI_TargetMaterial = NULL;

	CollisionInfo CL;
	const GameObject* target = GameWorld().CastRay(gCam, dir, 25000.0f, &CL); //bp, do bbox check so that clicks are better

	bool found = false;

	if (target)
	{
		UI_TargetObjID = target->ID;
		UI_TargetPos   = CL.NewPosition; //target->Position;
		UI_TargetPos2  = CL.NewPosition;
		UI_TargetNorm  = CL.Normal;
		UI_TargetObjectAdditionalInfo = CL.objectInfo;

		if(CL.Material)
			UI_TargetMaterial  = CL.Material;

		found = true;
	}

	float dist = 9999999;
	r3dMaterial* collectionsMaterial = Get_Material_By_Ray(gCam, dir, dist);

	if (collectionsMaterial)
	{
		if (!CL.Material || CL.Distance > dist)
		{
			UI_TargetMaterial  = collectionsMaterial;
			UI_TargetPos = gCam + dir * dist;
			found = true;
		}
	}

	if ( Terrain && Terrain->IsLoaded() ) {
		extern BOOL terra_FindIntersection(r3dPoint3D &vFrom, r3dPoint3D &vTo, r3dPoint3D &colpos, int iterations);
		r3dPoint3D v3;

		if(terra_FindIntersection(gCam, gCam + dir*20000, v3, 20000)) 
		{
			UI_TerraTargetPos = v3;

			float offset = 0;
			//	Add small offset for roads, because usually roads and terrain coincide, and false roads non-selection events can happen
			if (target && target->isObjType(OBJTYPE_Road))
			{
				offset = -0.2f;
			}

			r3dVector oldDistance = UI_TargetPos - gCam;
			r3dVector newDistance = v3 - gCam;
			// if the new distance is closer than the hit, and the new distance is in the same direction as the hit. 
			if( !found || ( oldDistance.LengthSq() + offset > newDistance.LengthSq() && newDistance.Normalize().Dot(dir) > 0 ) )
			{
				UI_TargetObjID = invalidGameObjectID;
				UI_TargetPos   = v3;
				UI_TargetNorm  = Terrain->GetNormal( v3 ) ;
				UI_TargetPos2  = v3;
				UI_TargetMaterial = NULL;
				found = true;
			}
		}
	}

	return found;
}

void CameraHUD :: ProcessPick()
{
	int mx, my;
	Mouse->GetXY(mx,my);
	if(!hud_ProcessCameraPick((float)mx, (float)my)) {
		UI_TargetPos = FPS_Position;
	}

	// cheat: teleport player
	if((Keyboard->IsPressed(kbsLeftAlt) && Keyboard->WasPressed(kbsT)))
	{
		//r->AI_actor.SetPosition(UI_TargetPos);
		FPS_Position = UI_TargetPos;
	}
}

