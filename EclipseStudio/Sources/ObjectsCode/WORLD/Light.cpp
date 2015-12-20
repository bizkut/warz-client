#include "r3dPCH.h"
#include "r3d.h"

#include "GameObjects\gameobj.h"
#include "GameObjects\objmanag.h"
#include "Utils/Utils.h"

#include "Lamp.h"
#include "GameStart.h"

//
//
// 	class for LampBulb Object, as you may guess..
//
//

IMPLEMENT_CLASS(obj_LampBulb, "obj_LampBulb", "Object");
AUTOREGISTER_CLASS(obj_LampBulb);


extern r3dLightSystem	WorldLightSystem;

extern int			bLightEditorActive;
extern GameObject*	LightObject;


BOOL obj_LampBulb::OnCreate()
{
 parent::OnCreate();

 DrawOrder	= OBJ_DRAWORDER_LAST;

 ObjFlags |= OBJFLAG_CollideNever | OBJFLAG_SkipCastRay;

  char FName[256];
   
 sprintf(FName,"Levels\\%s\\Scene\\CFG\\ObjectCFG.cfg", MissionInfo.MissionDir);


 Color = r3dColor(random(255), random(255), random(255));
// Color = r3dColor(255,255,255);

 r3dVector VC = r3dReadCFG_V(FName, Name.c_str(), "LightColor", r3dVector(255,240,220));

 Color = r3dColor(VC.X, VC.Y, VC.Z);
 
 EditMode = 0;

 bOn		= 1;
 bHaveLight	= 0;
 bCastShadows	= 0;

 pObject->RecalcVertexNormals();
 pObject->FillBuffers();

 bHaveLight = r3dReadCFG_I(FName, Name.c_str(), "HaveLight", 1);
 bCastShadows = r3dReadCFG_I(FName, Name.c_str(), "CastShadows", 0);
 bAffectLightmap = r3dReadCFG_I(FName, Name.c_str(), "AffectLightmap", 1);
 bStatic = r3dReadCFG_I(FName, Name.c_str(), "StaticLight", 0);


 float DX=0,DY=-1,DZ=0;

 if(strstr(Name.c_str(),"_XM")) DX = -5;
 if(strstr(Name.c_str(),"_XP")) DX = 5;
 if(strstr(Name.c_str(),"_YM")) DY = -5;
 if(strstr(Name.c_str(),"_YP")) DY = 10;
 if(strstr(Name.c_str(),"_ZM")) DZ = -5;
 if(strstr(Name.c_str(),"_ZP")) DZ = 5;

// DY -= 3;
 DY += -4;
 LT.Assign(Position.X+DX,Position.Y+DY,Position.Z+DZ);

// LT.ProjectMap = r3dRenderer->LoadTexture("Data\\Images\\Spot\\blamplight.bmp", D3DFMT_A8R8G8B8, NULL);
 LT.SpotAngle = 700.0f;

 LT.SetType(R3D_SPOT_LIGHT);
 LT.SetType(R3D_PROJECTOR_LIGHT);
 LT.SetType(R3D_OMNI_LIGHT);

// LT.SetRadius (20,400);
// LT.SetRadius (20,120);
 LT.SetRadius ((float)20,r3dReadCFG_I(FName, Name.c_str(), "LightRadius", 250));

// LT.Direction = pObject->FaceList[pObject->NumFaces/2].Normal;
 LT.Direction = r3dVector(0,-1,0.05f);
 LT.SetColor(Color.R, Color.G, Color.B);
 LT.bAffectBump = 0;
 LT.bAffectLightmap = bAffectLightmap;
 LT.bCastShadows = 0;//bCastShadows;
 LT.Assign(Position.X+DX,Position.Y+DY,Position.Z+DZ);

 bHaveRays = 1;

// if ( strstr( Name.c_str(), "_Lamp_" )) bHaveRays = 1;
// if ( strstr( Name.c_str(), "_lamp_" )) bHaveRays = 1;

 if (bHaveRays && bHaveLight) WorldLightSystem.Add(&LT); 

 float BaseSize = ((pObject->BBox.Size.X+pObject->BBox.Size.Y+pObject->BBox.Size.Z)/3.0f)*4.0f;
 
 LightBBox.Org = Position; //-r3dPoint3D(0,90.0f*0.14f,0);
 LightBBox.Org -= r3dPoint3D(BaseSize, 0.0f, BaseSize);
 LightBBox.Size = r3dPoint3D(BaseSize*2, BaseSize*2, BaseSize*2);

 Query = NULL;
 r3dRenderer->pd3ddev->CreateQuery(D3DQUERYTYPE_OCCLUSION, &Query);
 VisiblePixels = 0;

 return 1;
}


BOOL obj_LampBulb::OnDestroy()
{
 if (Query) Query->Release();
 Query = NULL;

 return parent::OnCreate();
}


BOOL obj_LampBulb::Update()
{
	if (bLightEditorActive)
		 ObjFlags = OBJFLAG_CollideNever;
	else
		 ObjFlags = OBJFLAG_CollideNever | OBJFLAG_SkipCastRay;

/*
 static int MovesDone = 0;
 static float DLP = -1;

 pObject->Move (r3dPoint3D(0,0,DLP));
 pObject->ResetXForm();
 LT.Assign(pObject->CentralPoint->X,pObject->CentralPoint->Y,pObject->CentralPoint->Z);
 MovesDone ++;

 if (MovesDone > 5*90)
 {
  DLP *= -1;
  MovesDone = 0;
 } 


 if(Keyboard->WasPressed(kbsHome))
 {
  pObject->Move (r3dPoint3D(0,0,5));
  pObject->ResetXForm();
  LT.Assign(pObject->CentralPoint->X,pObject->CentralPoint->Y,pObject->CentralPoint->Z);
 }

 if(Keyboard->WasPressed(kbsEnd))
 {
  pObject->Move (r3dPoint3D(0,0,-5));
  pObject->ResetXForm();
  LT.Assign(pObject->CentralPoint->X,pObject->CentralPoint->Y,pObject->CentralPoint->Z);
 }
*/


 BBox = pObject->BBox;

// if(Keyboard->WasPressed(kbsU)) Att0 += 0.005f;
// if(Keyboard->WasPressed(kbsI)) Att1 += 0.005f;
// if(Keyboard->WasPressed(kbsO)) Att2 += 0.005f;
// if(Keyboard->WasPressed(kbsJ)) Att0 -= 0.005f;
// if(Keyboard->WasPressed(kbsK)) Att1 -= 0.005f;
// if(Keyboard->WasPressed(kbsL)) Att2 -= 0.005f;

 VisiblePixels = 0;
 while (Query->GetData( &VisiblePixels, sizeof(DWORD), D3DGETDATA_FLUSH ) == S_FALSE);

 static float DD = -5;
 static float XP = -0.5f;

 DD += XP * r3dGetFrameTime();

 if (DD < -10) XP = 0.15f;
 if (DD > 10) XP = -0.15f;

// LT.Assign(Position.X,Position.Y+DD,Position.Z);

 static float DD1 = 0;
 static float XP1 = -0.5f;

 DD1 += XP1 * r3dGetFrameTime();

 if (DD1 < 0) XP1 = 0.5f;
 if (DD1 > 45) XP1 = -0.5f;

// LT.SetRadius (20,100+DD1);


 if (!bOn)
 {
  FadeVal -= r3dGetFrameTime()*1700.0f;

  if ( FadeVal < 0 ) { LT.bOn = 0; FadeVal = 0; }

  float VV = FadeVal / 255.0f;

  LT.SetColor(float(Color.R*VV), float(Color.G)*VV, float(Color.B)*VV);
 }  



 return TRUE;
}


BOOL obj_LampBulb::OnCollide(GameObject *tobj, CollisionInfo &trace)
{
 return TRUE;
}




BOOL obj_LampBulb::OnEvent(int event, void *data)
{
 if (event == OBJ_EVENT_GETEDITORCOMMANDS)
 {
  GOCommandArray *CA = (GOCommandArray *)data;

  switch(EditMode)
  {
       case 0:
	 CA->NumCommands = 10;
	  CA->Commands[0].CommandID = OBJ_EVENT_USER + 100;
	  strcpy(CA->Commands[0].Name,"RND COLOR");
	  CA->Commands[1].CommandID = OBJ_EVENT_USER + 101;
	  strcpy(CA->Commands[1].Name,"RESET");
	  CA->Commands[2].CommandID = OBJ_EVENT_USER + 102;
	  strcpy(CA->Commands[2].Name,"TURN ON");
	  CA->Commands[3].CommandID = OBJ_EVENT_USER + 103;
	  strcpy(CA->Commands[3].Name,"TURN OFF");
	  CA->Commands[4].CommandID = OBJ_EVENT_USER + 104;
	  sprintf(CA->Commands[4].Name,"SHADOWS: %d", bCastShadows);

	  CA->Commands[5].CommandID = OBJ_EVENT_USER + 105;
	  sprintf(CA->Commands[5].Name,"LIGHT: %d", bHaveLight);

	  CA->Commands[6].CommandID = OBJ_EVENT_USER + 106;
	  sprintf(CA->Commands[6].Name,"RADIUS+: %d", int(LT.GetOuterRadius()));

	  CA->Commands[7].CommandID = OBJ_EVENT_USER + 107;
	  sprintf(CA->Commands[7].Name,"RADIUS-: %d", int(LT.GetOuterRadius()));

	  CA->Commands[8].CommandID = OBJ_EVENT_USER + 108;
	  sprintf(CA->Commands[8].Name,"LIGHTMAP: %d", LT.bAffectLightmap);

	  CA->Commands[9].CommandID = OBJ_EVENT_USER + 109;
	  sprintf(CA->Commands[9].Name,"STATIC: %d", bStatic);

	  return TRUE;
	break;

       case 1:
	 CA->NumCommands = 7;
	  CA->Commands[0].CommandID = OBJ_EVENT_USER + 100;
	  strcpy(CA->Commands[0].Name,"RED +");
	  CA->Commands[1].CommandID = OBJ_EVENT_USER + 101;
	  strcpy(CA->Commands[1].Name,"RED -");
	  CA->Commands[2].CommandID = OBJ_EVENT_USER + 102;
	  strcpy(CA->Commands[2].Name,"GREEN +");
	  CA->Commands[3].CommandID = OBJ_EVENT_USER + 103;
	  strcpy(CA->Commands[3].Name,"GREEN -");
	  CA->Commands[4].CommandID = OBJ_EVENT_USER + 104;
	  strcpy(CA->Commands[4].Name,"BLUE +");
	  CA->Commands[5].CommandID = OBJ_EVENT_USER + 105;
	  strcpy(CA->Commands[5].Name,"BLUE -");
	  CA->Commands[6].CommandID = OBJ_EVENT_USER + 106;
	  sprintf(CA->Commands[6].Name,"BACK %d %d %d", Color.R, Color.G, Color.B );
	  return TRUE;
	break;
   }
 
 }


 if (EditMode) EditColor(event);
 else
    EditGeneral(event);


   char FName[256];
   char Str[256];
   
   sprintf(FName,"Levels\\%s\\Scene\\CFG\\ObjectCFG.cfg", MissionInfo.MissionDir);

   sprintf (Str,"%d", bCastShadows);
   r3dWriteCFG_S(FName, Name.c_str(), "CastShadows", Str);

   sprintf (Str,"%d", bStatic);
   r3dWriteCFG_S(FName, Name.c_str(), "StaticLight", Str);

   sprintf (Str,"%d", bAffectLightmap);
   r3dWriteCFG_S(FName, Name.c_str(), "AffectLightmap", Str);

   sprintf (Str,"%d", bHaveLight);
   r3dWriteCFG_S(FName, Name.c_str(), "HaveLight", Str);

   sprintf (Str,"%d", int(LT.GetOuterRadius()));
   r3dWriteCFG_S(FName, Name.c_str(), "LightRadius", Str);

   sprintf (Str,"%d %d %d", Color.R, Color.G, Color.B);
   r3dWriteCFG_S(FName, Name.c_str(), "LightColor", Str);

 return FALSE;
}


void obj_LampBulb::EditColor(int event)
{
 switch (event)
 {
  case OBJ_EVENT_USER + 100:
 	Color.R ++;
	LT.SetColor(Color.R, Color.G, Color.B);
	break;

  case OBJ_EVENT_USER + 101:
 	Color.R --;
	LT.SetColor(Color.R, Color.G, Color.B);
	break;

  case OBJ_EVENT_USER + 102:
 	Color.G ++;
	LT.SetColor(Color.R, Color.G, Color.B);
	break;

  case OBJ_EVENT_USER + 103:
 	Color.G --;
	LT.SetColor(Color.R, Color.G, Color.B);
	break;

  case OBJ_EVENT_USER + 104:
 	Color.B ++;
	LT.SetColor(Color.R, Color.G, Color.B);
	break;

  case OBJ_EVENT_USER + 105:
 	Color.B --;
	LT.SetColor(Color.R, Color.G, Color.B);
	break;

  case OBJ_EVENT_USER + 106:
        EditMode = 1 - EditMode;
	break;
 }

}



void obj_LampBulb::EditGeneral(int event)
{
 switch (event)
 {
  case OBJ_EVENT_USER + 100:
 	Color = r3dColor(random(255), random(255), random(255));
	LT.SetColor(Color.R, Color.G, Color.B);
	break;

  case OBJ_EVENT_USER + 101:
	Color = r3dColor(255,255,255);
	LT.SetColor(Color.R, Color.G, Color.B);
	break;

  case OBJ_EVENT_USER + 102:
	bOn = 1;
	LT.bOn = bOn;
	LT.SetColor(Color.R, Color.G, Color.B);
	break;

  case OBJ_EVENT_USER + 103:
	bOn = 0;
  	LT.bOn = bOn;
	break;

  case OBJ_EVENT_USER + 104:
        bCastShadows = 1 - bCastShadows;
        LT.bCastShadows = bCastShadows;
	break;

  case OBJ_EVENT_USER + 105:
	//bHaveLight = 1 - bHaveLight;
	//bOn = bHaveLight;
  	//LT.bOn = bOn;
        EditMode = 1 - EditMode;
	break;

  case OBJ_EVENT_USER + 108:
        bAffectLightmap = 1 - bAffectLightmap;
	LT.bAffectLightmap = bAffectLightmap;
	break;

  case OBJ_EVENT_USER + 109:
        bStatic = 1 - bStatic;
	break;

  case OBJ_EVENT_USER + 106:
        LT.SetRadius(20, LT.GetOuterRadius()+5);
	break;

  case OBJ_EVENT_USER + 107:
        LT.SetRadius(20, LT.GetOuterRadius()-5);
	break;
 }


}



#ifdef EH_CLIENT
void r3dDrawIcon3D(const r3dPoint3D& pos, r3dTexture *icon, const r3dColor &Col, float size)
{
	int r3dProjectToScreen(const r3dPoint3D& pos, r3dPoint3D* scrCoord);

	r3dPoint3D scr;
	if(!r3dProjectToScreen(pos, &scr))
		return;

	float TargetBoxX = scr.x;
	float TargetBoxY = scr.y;
	
	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);
	r3dDrawBox2D(TargetBoxX - size/2,TargetBoxY - size/2,size,size,Col, icon);
}
#endif


void obj_LampBulb::Draw(r3dCamera& Cam, int DrawState)
{
	
#ifdef EH_CLIENT

 if (!bLightEditorActive) return;

 r3dVector V;

 if (!GameWorld.CheckPVS(this, Cam)) return;

 switch (DrawState)
 {
  case OBJ_DRAW_USERDEFINED+300:
//	DrawGlow(Cam);
	return;


//  case OBJ_DRAW_USERDEFINED+10:  // Draw for reflection/refraction
//  case OBJ_DRAW_USERDEFINED+11:  // Draw for reflection/refraction

  case OBJ_DRAW_USERDEFINED+2:
	if (!bOn) return;
	//DrawGlow(Cam);
	return;

  case OBJ_DRAW_USERDEFINED+200:
       {
 //        if (!r3dRenderer->IsBoxInsideFrustum(LightBBox)) return;

		r3dColor Cl = r3dRenderer->AmbientColor;
        if (bOn) Cl = Color;

        r3dRenderer->SetTexture(NULL);
		r3dRenderer->SetMaterial(NULL);
		r3dRenderer->SetRenderingMode(R3D_BLEND_COPY_ZCW);

		r3dDrawIcon3D(LT, NULL, Cl, 64);

        r3dRenderer->SetTexture(NULL);
		r3dRenderer->SetMaterial(NULL);
        r3dRenderer->SetRenderingMode(R3D_BLEND_COPY_ZCW);
		if (LightObject == this )
		{
			//r3dDrawBoundBox(BBox, Cam, r3dColor(255,0,0));
			r3dDrawBoundBox(LightBBox, Cam, r3dColor(255,255,0));

			//r3dDrawLine3D(BBox.Org, BBox.Org + r3dVector(0,300,0), Cam, 3, r3dColor(0, 255, 0));
			//r3dDrawLine3D(BBox.Org, BBox.Org + r3dVector(300,0,0), Cam, 3, r3dColor(255, 255, 0));
			//r3dDrawLine3D(BBox.Org, BBox.Org + r3dVector(0,0,300), Cam, 3, r3dColor(255, 0, 0));
		}
       }
       break;

   default:
	 break;
 }

#endif 

}
