#include "r3dPCH.h"
#ifndef FINAL_BUILD

#include "r3d.h"
#include "d3dfont.h"
#include "d3dfont.h"

#include "UI\Hud_ParticleEditor.h"

#include "GameCommon.h"
#include "..\..\..\GameEngine\gameobjects\obj_Dummy.h"
#include "..\Editors\EditedValueTracker.h"

#include "Editors\ObjectManipulator3d.h"

#include "ObjectsCode\Effects\obj_ParticleSystem.h"
#include "Particle.h"
#include "Particle_Int.h"

obj_ParticleSystem*	ParticleObj = NULL;
DummyObject*	DummyTargetObj = NULL;
bool			DummyVisible = true;
r3dVector ParticleSystemOldPos = r3dVector(0,0,0);
r3dVector DummyTargetOldPos = r3dVector(0,0,0);
r3dVector ParticleSystemOldRot = r3dVector(0,0,1);

	// selected objectID
//	gobjid_t	UI_SelectedObjID	= invalidGameObjectID;

// on-cursor selection
	r3dPoint3D	UI_ParticlePos;		// snapped to object position (object center)

static int enablemouselook = 1;
static int firstrun = 1;

char g_szStartParticleEdit[MAX_PATH] = {"default"};
bool g_bNewParticle = false;

void ParticleEditorSetDefaultParticle ( const char * sPrt )
{
	g_bNewParticle = false;
	if ( sPrt && stricmp ( sPrt, "empty" ) )
		r3dscpy ( g_szStartParticleEdit, sPrt );
	else
	{
		g_bNewParticle = true;
		r3dscpy ( g_szStartParticleEdit, "empty" );
	}
}

r3dVector DirectionToYawPitchRoll (r3dVector vDir)
{
	void MatrixGetYawPitchRoll ( const D3DXMATRIX & , float &, float &, float & );


	vDir.Normalize();
	r3dVector vUp = ( fabsf ( vDir.Dot(r3dVector(0,1,0)) ) > 0.9f ) ? r3dVector(1,0,0) : r3dVector(0,1,0);
	r3dVector vR = vUp.Cross ( vDir );
	vR.Normalize();
	vUp = vDir.Cross ( vR );

	D3DXMATRIX mMat;
	D3DXMatrixIdentity(&mMat);
	memcpy ( mMat.m[0], vR.d3dx(), sizeof ( r3dVector ) );
	memcpy ( mMat.m[1], vUp.d3dx(), sizeof ( r3dVector ) );
	memcpy ( mMat.m[2], vDir.d3dx(), sizeof ( r3dVector ) );

	r3dVector vDirRet;
	MatrixGetYawPitchRoll ( mMat, vDirRet.x, vDirRet.y, vDirRet.z );
	vDirRet.x = R3D_RAD2DEG(vDirRet.x);
	vDirRet.y = R3D_RAD2DEG(vDirRet.y);
	vDirRet.z = R3D_RAD2DEG(vDirRet.z);
	return vDirRet;
}

void ParticleHUD :: InitPure()
{
	UI_ParticlePos     = r3dPoint3D(0, 0, 0);
	FPS_Position.Assign(0,20,-20);

	FPS_Acceleration.Assign(0,0,0);
	FPS_vViewOrig.Assign(0,-25,0);
	FPS_ViewAngle.Assign(0,0,0);
	FPS_vVision.Assign(0, 0, 1);


}



void ParticleHUD :: DestroyPure ()
{
}


void ParticleHUD :: SetCameraDir (r3dPoint3D vPos )
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

r3dPoint3D ParticleHUD :: GetCameraDir () const
{
	return FPS_vVision;
}

void ParticleHUD :: SetCameraPure ( r3dCamera &Cam)
{
 r3dPoint3D CamPos = FPS_Position;
 r3dPoint3D ViewPos = CamPos + FPS_vVision*10.0f;

 extern float GameFOV;
 Cam.FOV = GameFOV;
 Cam.SetPosition( CamPos );
 Cam.PointTo(ViewPos);
}  



void ParticleHUD :: Draw()
{
	assert(bInited);
	
	if ( !bInited ) return;

	r3dSetFiltering( R3D_POINT );
	
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, 	FALSE );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHAREF,        	1 );

	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

	if(!enablemouselook)
	{
		  ProcessParticleEditor();
	}

	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, 	FALSE );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHAREF,        	1 );
}





//----------------------------------------------------------------
void ParticleHUD :: Process()
//----------------------------------------------------------------
{
 FPS_Acceleration.Assign(0, 0, 0);

 float	glb_MouseSens    = 0.5f;	// in range (0.1 - 1.0)
 float  glb_MouseSensAdj = 1.0f;	// in range (0.1 - 1.0)

 extern bool g_bStartedAsParticleEditor;

 if (!ParticleObj)
 {		
	 ParticleObj = (obj_ParticleSystem *)srv_CreateGameObject("obj_ParticleSystem", g_szStartParticleEdit, g_bStartedAsParticleEditor ? r3dVector ( 0,0,0 ) : UI_ParticlePos);
	 ParticleObj->ObjFlags |= OBJFLAG_SkipCastRay; 
	
	 if ( !DummyTargetObj )
	{
		DummyTargetObj = (DummyObject*)srv_CreateGameObject ( "DummyObject", "dummy", g_bStartedAsParticleEditor ? r3dVector ( 0,0,0 ) : UI_ParticlePos );
		DummyTargetObj->SetScale ( 2.0f * r3dVector ( 1.0f, 1.0f, 1.0f ) );
		GameWorld().UnlinkObject(DummyTargetObj);
	}

	ParticleEditorStart();
 }

 OnProcess ();

  enablemouselook = Mouse->IsPressed(r3dMouse::mRightButton) && !g_imgui_LockRbr;

  ProcessPick( true );
  //if (!enablemouselook)
  //  if(Keyboard->IsPressed(kbsLeftControl)) ProcessPick();

//
//  Mouse controls are here
//
	int mMX=Mouse->m_MouseMoveX, mMY=Mouse->m_MouseMoveY;

  if(!enablemouselook && !firstrun) return;
  firstrun = 0;

	FPS_vViewOrig.x += float(-mMX) * glb_MouseSensAdj;
	FPS_vViewOrig.y += float(-mMY) * glb_MouseSensAdj;

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

  // walk
  float fSpeed = 5.0f; //_ai_fWalkSpeed;

  if(Keyboard->IsPressed(kbsW)) FPS_Acceleration.Z = fSpeed;
  if(Keyboard->IsPressed(kbsS)) FPS_Acceleration.Z = -fSpeed * 0.7f;
  if(Keyboard->IsPressed(kbsA)) FPS_Acceleration.X = -fSpeed * 0.7f;
  if(Keyboard->IsPressed(kbsD)) FPS_Acceleration.X = fSpeed * 0.7f;
  if(Keyboard->IsPressed(kbsQ)) FPS_Position.Y    += SRV_WORLD_SCALE(1.0f)* r3dGetFrameTime();
  if(Keyboard->IsPressed(kbsE)) FPS_Position.Y    -= SRV_WORLD_SCALE(1.0f)* r3dGetFrameTime();

  float mult = 1;
  if(Keyboard->IsPressed(kbsLeftShift)) mult = 64.0f;


  FPS_Position += FPS_vVision * FPS_Acceleration.Z * r3dGetFrameTime() * mult;
  FPS_Position += FPS_vRight * FPS_Acceleration.X * r3dGetFrameTime() *mult;

//  if (FPS_Position.Y < terra_GetH(FPS_Position)) FPS_Position.Y = terra_GetH(FPS_Position);



	
  return;
}



void ParticleHUD :: ProcessPick( bool bSimple/* = false*/ )
{
  int mx, my;
  Mouse->GetXY(mx,my);

  if ( bSimple )
  {
	  bool hud_ProcessCameraPick(float, float);
	  if(!hud_ProcessCameraPick((float)mx, (float)my)) {
		  extern r3dPoint3D	UI_TargetPos;
		  UI_TargetPos = FPS_Position;
	  }
  }
  else
  {
	  r3dPoint3D dir;
	  r3dScreenTo3D((float)mx, (float)my, &dir);

	  CollisionInfo CL;
	  const GameObject* target = GameWorld().CastRay(gCam, dir, 2000.0f, &CL); //bp, do bbox check so that clicks are better

	  if(target) 
	  {
		UI_ParticlePos   = CL.NewPosition; //target->Position;
	  } 
	  else 
	  {
		extern BOOL terra_FindIntersection(r3dPoint3D &vFrom, r3dPoint3D &vTo, r3dPoint3D &colpos, int iterations);
		r3dPoint3D v3;
		if(terra_FindIntersection(gCam, gCam + dir*2000, v3, 2000)) 
		{
		  UI_ParticlePos   = v3;
		}
		else
		{
			float fH = terra_GetH ( gCam );
			UI_ParticlePos = gCam;
			UI_ParticlePos.y = fH;
		}
	  }

	  extern bool g_bStartedAsParticleEditor;

	  extern r3dPoint3D editTorchPosition ;
	  editTorchPosition = g_bStartedAsParticleEditor ? r3dVector ( 0,0,0 ) : UI_ParticlePos;

	  if ( ParticleObj )
	  {
		
		ParticleSystemOldPos = ParticleObj->GetPosition();

		if ( DummyTargetObj )
		{
		  DummyTargetObj->SetPosition(ParticleObj->GetPosition() + 10.0f * ParticleObj->Torch->Direction );
		  DummyTargetOldPos = DummyTargetObj->GetPosition ();
		}

	  }
  }

  return;
}




static void r3dDrawBox2DTC(int X, int Y, int W, int H, r3dColor &C, float CellX, float CellY, float SX, float SY, float SSX, float SSY, r3dTexture *Tex)
{
	float TC[16];

	float StepX = 1.0f / CellX;
	float StepY = 1.0f / CellY;

	TC[0] = SX*StepX; 		
	TC[1] = SY*StepY;
	TC[2] = SX*StepX+SSX*StepX; 	
	TC[3] = SY*StepY;
	TC[4] = SX*StepX+SSX*StepX;
	TC[5] = SY*StepY+SSY*StepY;
	TC[6] = SX*StepX; 		
	TC[7] = SY*StepY+SSY*StepY; 

	r3dDrawBox2D((float)X, (float)Y, (float)W, (float)H, C, Tex, TC);
}



static void Edit_Value_Slider_OLD(float x, float y, float minval, float maxval, const char* fmt, const char* name, float* edit_val, int doslider = true)
{
  imgui_Value_Slider(x, y, name, edit_val, minval, maxval, fmt, doslider);
}

static void Edit_Value_Slider_OLD(float x, float y, float minval, float maxval, const char* fmt, const char* name, int* edit_val, int doslider = true)
{
  imgui_Value_SliderI(x, y, name, edit_val, minval, maxval, fmt, doslider);
}



void ParticleHUD :: ProcessParticleEditor()
{
	void ParticleEditor_Process_NEW();
	ParticleEditor_Process_NEW();
}

void ParticleHUD::ParticleEditorStart()
{
	static bool g_bDesktopInited = false;

	if ( !g_bDesktopInited )
	{
		Desktop().SetViewSize( r3dRenderer->ScreenW, r3dRenderer->ScreenH );

		Desktop_c * pDesc;

		pDesc = g_pDesktopManager->AddDesktop( "particle_editor_first" );
		float fSize1 = 35 + 34 * 2;
		pDesc->SetViewPosition( r3dRenderer->ScreenW - 375, fSize1 );
		pDesc->SetViewSize( 362, r3dRenderer->ScreenH - fSize1 - 10 );

		float fSize2 = 35 + 34 * 2 + 24 * 3;
		pDesc = g_pDesktopManager->AddDesktop( "particle_editor_second" );
		pDesc->SetViewPosition( r3dRenderer->ScreenW - 375, fSize2 + 7 );
		pDesc->SetViewSize( 362, r3dRenderer->ScreenH - fSize2 - 10 - 7 );

		g_bDesktopInited = true;
	}

	ProcessPick ();
/*
	FPS_vVision = r3dVector ( 1, -1, 1 );
	FPS_vVision.Normalize();
	FPS_Position = UI_ParticlePos - 50.0f * FPS_vVision;
	FPS_vRight = FPS_vVision.Cross(r3dVector ( 0,1,0 ));
	FPS_vRight.Normalize();
	FPS_vUp = FPS_vVision.Cross(FPS_vRight);
*/
	ParticleSystemOldRot = ParticleObj->Torch->PD->OrgDirection; 
	ParticleSystemOldRot.Normalize();

	if(ParticleObj)
		ParticleObj->SetRotationVector ( DirectionToYawPitchRoll ( ParticleSystemOldRot ) );

	extern r3dPoint3D	BeamTargetOffset;

	//BeamTargetOffset = ParticleObj->Torch->BeamTargetPosition - ParticleObj->Torch->Position;
	if ( DummyTargetObj && ParticleObj )
		DummyTargetObj->SetPosition(ParticleObj->GetPosition () + BeamTargetOffset);

	void ParticleEditor_Start_NEW();
	ParticleEditor_Start_NEW();
}

void ParticleHUD::OnProcess ()
{
	//if ( g_Manipulator3d.PickedObject() != ParticleObj && g_Manipulator3d.PickedObject() != DummyTargetObj )
	//{
	//	g_Manipulator3d.PickedObjectSet ( ParticleObj );
	//}

	g_Manipulator3d.TypeFilterSet("");
	g_Manipulator3d.Enable();
	g_Manipulator3d.Lock();
	g_Manipulator3d.ScaleDisable ();

	/*
	if ( Keyboard->IsPressed(kbsLeftControl) )
	{
		if ( g_Manipulator3d.PickedObject() != ParticleObj )
			g_Manipulator3d.PickedObjectSet ( ParticleObj );
	}
	else
	{
		if ( g_Manipulator3d.PickedObject()  )
			g_Manipulator3d.PickedObjectSet ( NULL );
	}*/

	bool imgui2_IsCursorOver2d();
	bool imgui_IsCursorOver2d();

	if ( Keyboard->IsPressed(kbsLeftControl) && (Mouse->IsPressed(r3dMouse::mLeftButton)) && !imgui2_IsCursorOver2d () && !imgui_IsCursorOver2d() )
	{
		if ( ParticleObj )
		{
			extern r3dPoint3D UI_TargetPos;
			extern r3dPoint3D editTorchPosition;

			editTorchPosition = UI_TargetPos;
		}
	}
	if ( Keyboard->IsPressed(kbsLeftAlt) && (Mouse->IsPressed(r3dMouse::mLeftButton)) )
	{
		if ( DummyTargetObj )
		{
			extern r3dPoint3D	UI_TargetPos;
			DummyTargetObj->SetPosition ( UI_TargetPos );
		}
	}

	extern r3dParticleData*   EditTorch;
	extern int		curEmitterSlot;
	extern r3dPoint3D	BeamTargetOffset;

	if ( EditTorch && EditTorch->PType[curEmitterSlot] )
	{
		if ( EditTorch->PType[curEmitterSlot]->ParticleType != R3D_PARTICLE_BEAM )
		{
			if ( g_Manipulator3d.PickedObject() == DummyTargetObj )
				g_Manipulator3d.PickedObjectSet ( ParticleObj );

			if ( DummyVisible )
			{
				//GameWorld().UnlinkObject(DummyTargetObj);
				DummyVisible = false;
			}
		}
		else
		{
			if ( !DummyVisible )
			{
				//GameWorld().LinkObject(DummyTargetObj);
				DummyVisible = true;
			}
		}
	}
	
	if ( ParticleObj && DummyTargetObj )
	{
		bool bMoved = false;

		{
			bool bTargetMoved = false;
			if ( fabsf ( ( ParticleSystemOldPos - ParticleObj->GetPosition() ).LengthSq() ) > 0.0001f )
			{
				bTargetMoved = true;
				ParticleSystemOldPos = ParticleObj->GetPosition();
			}

			if ( fabsf ( ( DummyTargetOldPos - DummyTargetObj->GetPosition() ).LengthSq() ) > 0.0001f )
			{
				bTargetMoved = true;
				DummyTargetOldPos = DummyTargetObj->GetPosition();
			}

			if ( bTargetMoved )
			{
				BeamTargetOffset = DummyTargetOldPos - ParticleSystemOldPos;
			}
			//else if ( !DummyVisible )
			//{
			//	DummyTargetOldPos = ParticleSystemOldPos + BeamTargetOffset;
			//}
		}

		r3dVector vCurDir;
		memcpy ( vCurDir.d3dx (), ParticleObj->GetRotationMatrix().m[2], sizeof ( r3dVector ) );//DummyTargetOldPos - ParticleSystemOldPos;
		vCurDir.Normalize();

		r3dVector vRotDiff = ParticleSystemOldRot - vCurDir;

		if ( ( fabsf( vRotDiff.x ) > 0.0001f ) || ( fabsf( vRotDiff.y ) > 0.0001f ) || ( fabsf( vRotDiff.z ) > 0.0001f ) )
		{
			ParticleSystemOldRot = vCurDir;
			bMoved = true;
		}

		extern r3dParticleData*   EditTorch;

		r3d_assert(EditTorch == ParticleObj->Torch->PD);

		r3dVector vOldDir = EditTorch->OrgDirection;
		vOldDir.Normalize();

		if ( bMoved )
			EditTorch->OrgDirection = vCurDir;
		else if ( vCurDir.Dot ( vOldDir ) < 0.99f )
		{
			//float fDist = (DummyTargetOldPos - ParticleSystemOldPos).Length ();
			//r3dVector vNewDummyPos = ParticleSystemOldPos + fDist * vOldDir;

			//DummyTargetOldPos = vNewDummyPos;
			//DummyTargetObj->SetPosition(vNewDummyPos);
			ParticleSystemOldRot = vOldDir;
			ParticleObj->SetRotationVector ( DirectionToYawPitchRoll ( vOldDir ) );
		}		
	}
}

void ParticleHUD::OnHudUnselected ()
{
	g_Manipulator3d.Unlock();
	g_Manipulator3d.Disable();
	
	if ( ParticleObj )
	{
		char sName[MAX_PATH];
		r3dscpy ( sName, ParticleObj->Name.c_str() );
		
		GameWorld().DeleteObject( ParticleObj );
		ParticleObj = NULL;

		void ParticleEditorReloadParticle ( const char* );
		ParticleEditorReloadParticle ( sName );
	}

	if ( g_bNewParticle )
		ParticleEditorSetDefaultParticle ( "default" );

	ValueTrackersManager::Instance().SetUpdateMask(~TT_PARTICLE_EMITTER_VARS & ValueTrackersManager::Instance().GetUpdateMask());
}


#endif // FINAL_BUILD