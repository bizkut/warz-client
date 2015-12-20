#include "r3dPCH.h"

#ifndef FINAL_BUILD

#include "r3d.h"

#include "UI/r3dMenu.h"
typedef r3dgameVector(r3dSTLString) stringlist_t;

#include "Particle.h"
#include "Particle_Int.h"
#include "UI/UIimEdit.h"
#include "UI/UIimEdit2.h"

#include "GameCommon.h"
#include "..\..\..\GameEngine\gameobjects\obj_Dummy.h"

#include "Editors\ObjectManipulator3d.h"

#include "ObjectsCode\Effects\obj_ParticleSystem.h"
#include "EditedValueTracker.h"
#include "ParticleEditorUndoSubsystem.h"

extern obj_ParticleSystem*	ParticleObj;
extern DummyObject*	DummyTargetObj;

void ParticleEditorSetDefaultParticle ( const char * );
void UpdateParticleAtlas( const char* basePath, const char* atlasName );

extern float gFillBufferTime;
extern float gFillUpdateTime;
extern int gNumTrails;

namespace
{
	int		CurMode;
	
	stringlist_t	vParticleNames;
	float		particleNamesOffset;
	stringlist_t	vTextureNames;
	float		textureNamesOffset;
	stringlist_t	vMeshNames;
	float		meshNamesOffset;

	enum pickid_e {
		PICK_None,
		PICK_Particle,
		PICK_GlobalMeshName,
		PICK_MeshName,
		PICK_MeshTexture,
		PICK_MeshDistortTexture,
		PICK_NormalTex,
		PICK_DistortTex,
		PICK_PartTexture,
		PICK_BackTexture,
	};

	enum ParticleActions_e {
		eNone = 0,
		eCreate = 1,
		eLoad,
		eSave,
		eSaveAs,
		eRestart,
		eReloadTex,
		eReset,
		eChangeEmitterName,
	};

	pickid_e	activeListID_ = PICK_None;

}

const char * PARTICLE_DATA_PATH_PATTERN = "Data\\Particles\\%s";

int		curEmitterSlot;
r3dParticleData*   EditTorch    = NULL;

bool		DrawPickMenu();

#pragma warning(push)
#pragma warning(disable:4244)

static const int	pe_slots_x     = 5;		// emitters slots x/y
static const int	pe_slots_y     = 35;
static const int	pe_emitter_w   = 365;		// emitter info x/y
static const int	pe_emitter_y   = 35;
static const int	pe_fileinfo[4] = {2, 35, 200, 60};

extern	r3dCamera	gCam;

static	int		torchMoving       = 0;
static	int		torchMoveUp       = 0;
static	int		torchUseMoveDir   = 0;
r3dPoint3D	editTorchPosition     = r3dPoint3D(0, 0, 0);
static	float		torchAnimTime     = 0;
static	float		torchMoveRadius   = 30;
r3dPoint3D	BeamTargetOffset  = r3dPoint3D(100, 20, 100);

extern	const void*	imgui2_val;
extern	bool		imgui2_disabled;

static	r3dTexture*	BackTexture = NULL;
static	char		InputLabel[256] = "";
static	char		InputLine[500] = "";
static	int		ActiveTextEditId = 0;

static	float		CameraZoom = 100.0f;
static	float		AngleY = -45.0f;
static	float		AngleZ = 45.0f;
static	float		_fileindex = 0;
static	int			bGlow = 1;
static	int			bTex = 1;
static	int			bBackTex  = 0;
static	int			bDrawGrid = 0;
static	char		BackFileName[100] = "None";

static	r3dParticleEmitter	pastedFx;
static	int			havePastedFx = false;

static bool imgui2_Value_Old(float minval, float maxval, const char* fmt, const char* name, float* edit_val, int doslider = true) {
	return imgui2_Value(name, edit_val, minval, maxval, fmt, doslider);
}

static bool imgui2_Value_Old(float minval, float maxval, const char* fmt, const char* name, int* edit_val, int doslider = true) {
	return imgui2_Value(name, edit_val, minval, maxval, fmt, doslider);
}

static void menuDrawGrid()
{
	r3dRenderer->SetTex(NULL);
	r3dRenderer->Flush();
	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_ZC);

	extern r3dCamera gCam;

	for(int i=0;i<50;i++) {
		r3dDrawLine3D(r3dPoint3D(-250+i*10,0.5,-250), r3dPoint3D(-250+i*10,0.5,250), gCam, 0.1f, r3dColor(80, 80, 80));
		r3dDrawLine3D(r3dPoint3D(-250,0.5,-250+i*10), r3dPoint3D(250,0.5,-250+i*10), gCam, 0.1f, r3dColor(80, 80, 80));
	}

	r3dDrawLine3D(r3dPoint3D(0,0.5,250), r3dPoint3D(0,0.5,-250), gCam, 0.2f, r3dColor(50,50,50));
	r3dDrawLine3D(r3dPoint3D(250,0.5,0), r3dPoint3D(-250,0.5,0), gCam, 0.2f, r3dColor(50,50,50));

	r3dDrawLine3D(r3dPoint3D(0,0,0), r3dPoint3D(110,0,0), gCam, 0.2f, r3dColor(0, 0, 255));	//X
	r3dDrawLine3D(r3dPoint3D(0,0,0), r3dPoint3D(0,0,110), gCam, 0.2f, r3dColor(255, 0, 0));	//Z
	r3dDrawLine3D(r3dPoint3D(0,0,0), r3dPoint3D(0,110,0), gCam, 0.2f, r3dColor(0, 255, 0)); //Y

	r3dRenderer->Flush();

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

	r3dDrawBox2D(X, Y, W, H, C, Tex, TC);
}

static bool saveParticle(r3dParticleData* torch, const char* fname, bool bShowSavedMenu = true )
{
	unlink(torch->FileName);

	::SetFileAttributes(fname, FILE_ATTRIBUTE_NORMAL);

	// either file can not exist or it must have write permission
	if(_access(fname, 0) != 0 || _access(fname, 2) == 0) {
		torch->Save(fname);
		if ( bShowSavedMenu )
			CurMode = 5000;
		return true;
	}

	CurMode = 5001;
	return false;
}

//--------------------------------------------------------------------------------------------------------
static bool IsImageSupport( const char * szFileName )
{
	FixedString s( szFileName );

	FixedString sExt = s.GetExt();
	if( sExt == ".tga" || sExt == ".dds" )
		return true;

	return false;
}

//
// get a list for particles and textures
//

static void ScanForFiles()
{
	WIN32_FIND_DATA ffblk;
	HANDLE h;
	char buf[MAX_PATH];

	// scan for particles
	vParticleNames.clear();
	particleNamesOffset = 0;
	sprintf(buf, "%s\\*.prt", "data\\Particles");

	h = FindFirstFile(buf, &ffblk);
	if(h != INVALID_HANDLE_VALUE) {
		do {
			vParticleNames.push_back(ffblk.cFileName);
		} while(FindNextFile(h, &ffblk) != 0);
		FindClose(h);
	}

	// scan for textures
	vTextureNames.clear();
	textureNamesOffset = 0;

	sprintf(buf, "%s\\*.*", "data\\Particles");

	h = FindFirstFile(buf, &ffblk);
	if(h != INVALID_HANDLE_VALUE) 
	{
		do 
		{
			if(ffblk.cFileName[0] == '.' || (ffblk.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				continue;

			FixFileName( ffblk.cFileName, buf );

			if ( IsImageSupport( buf ) )
				vTextureNames.push_back( buf );

		} 
		while(FindNextFile(h, &ffblk) != 0);
		FindClose(h);
	}

	// scan for meshes
	vMeshNames.clear();
	meshNamesOffset = 0;
	sprintf(buf, "%s\\*.sco", "data\\Particles");

	h = FindFirstFile(buf, &ffblk);
	if(h != INVALID_HANDLE_VALUE) {
		do {
			vMeshNames.push_back(ffblk.cFileName);
		} while(FindNextFile(h, &ffblk) != 0);
		FindClose(h);
	}

	return;
}

static void StartTextEdit(int id, const char* label, const char* value)
{
	r3dscpy(InputLabel, label);
	r3dscpy(InputLine, value);
	ActiveTextEditId = id;

	extern void imgui2_StartTextEdit(const char* valtext, void* edit_val);
	imgui2_StartTextEdit(InputLine, InputLine);

	return;
}




static void EnableListPick(pickid_e id)
{
	activeListID_ = id;
	win::input_Flush();

	return;
}

void ParticleReloadTex(r3dTexture** tex, const char* name)
{
	if(*tex) {
		r3dRenderer->DeleteTexture(*tex);
	}

	*tex = r3dRenderer->LoadTexture(name);

	return;
}

void NotifyReloadPartTexture(const char *Str2)
{
	ParticleReloadTex(&EditTorch->Texture, Str2);
	char Str3[512];
	sprintf(Str3, "%s%s", Str2, ".Atlas");
	EditTorch->Atlas.load(Str3);
}

void ParticleReloadMesh(r3dParticleEmitter &PE, const char *Str2, bool defaultMaterial )
{
	SAFE_DELETE(PE.Mesh);
	PE.Mesh = gfx_new r3dMesh( 0 );
	PE.Mesh->Load( Str2, defaultMaterial );
	PE.Mesh->FillBuffers();
}

//-------------------------------------------------------------------------
//	Global params value trackers
//-------------------------------------------------------------------------

static void DrawGlobalParams()
{
	imgui2_Static("Global parameters for particle system");

	imgui2_Value_Old(32.0f, 8192.0, "%.4f", "Cull Distance", &EditTorch->CullDistance);

	imgui2_Value_Old(0.0f, 10.0, "%.4f", "Global Scale", &EditTorch->OrgGlobalScale);
	imgui2_Value_Old(-1, 30, "%.4f", "WarmUpTime", &EditTorch->WarmUpTime);
	imgui2_Value_Old(0.05f, 10.0f, "%.4f", "Depth Cutoff", &EditTorch->DepthBlendValue);
	imgui2_Checkbox("ZSort", &EditTorch->ZSortEnabled);
	imgui2_Checkbox("Meshes Use Deferred Lighting", &EditTorch->bDeferredMeshParticles);

	if ( EditTorch->bDeferredMeshParticles )
		imgui2_Checkbox("Meshes Cast Shadows", &EditTorch->bDeferredMeshParticlesShadows);
	else
	{
		imgui2_Checkbox( "Particles Receive Shadows", &EditTorch->bReceivesShadows );
		imgui2_Checkbox( "Accept dynamic lights", &EditTorch->bAcceptsDynamicLights ) ;
	}

	static const char* EmitTypeNames[]={
		"Point",
		"Line",
		"Mesh",
		"Ring",
		"Circle",
	};
	imgui2_ListValue("Emitter Type", &EditTorch->EmitterType, EmitTypeNames, R3D_ARRAYSIZE(EmitTypeNames));
	switch(EditTorch->EmitterType) {
		case 0:
			imgui2_Value_Old(0.0,100.0,"%.4f","Emit Radius", &EditTorch->EmitRadius);
			break;
		case 1:
			imgui2_Static("Emit Vector");
			imgui2_Value_Old(-50.0,50.0,"%.4f","  X", &EditTorch->EmitVector.x);
			imgui2_Value_Old(-50.0,50.0,"%.4f","  Y", &EditTorch->EmitVector.y);
			imgui2_Value_Old(-50.0,50.0,"%.4f","  Z", &EditTorch->EmitVector.z);
			break;
		case 2:
			break;
		case 3:
			imgui2_Value_Old(0.0,100.0,"%.4f","Emit Radius", &EditTorch->EmitRadius);
			break;
		case 4:
			imgui2_Value_Old(0.0,100.0,"%.4f","Emit Radius", &EditTorch->EmitRadius);
			break;
	}
	imgui2_Value_Old(-1, 5000.0,"%.4f","Emit Time", &EditTorch->EmitTime);

	imgui2_Static("Direction Vector");
	imgui2_Value_Old(-1.0,1.0, "%.4f", "  X", &EditTorch->OrgDirection.x);
	imgui2_Value_Old(-1.0,1.0, "%.4f", "  Y", &EditTorch->OrgDirection.y);
	imgui2_Value_Old(-1.0,1.0, "%.4f", "  Z", &EditTorch->OrgDirection.z);

	/*
	r3dVector v1 = (ParticleObj->Torch->Position - gCam).NormalizeTo();
	r3dVector v2 = (Sun->SunLight.Direction).NormalizeTo();
	float k = v1.Dot(v2);
	k = (k + 1)/2;
	imgui2_Static("dot1: %f", k);

	//r3dGameLevel::Environment->m_turbitity.Reset(20.0f);
	//r3dDrawBox2D(200, 200, 200, 200, gPartShadeColor);
	*/
}

static void DrawTextureParams ()
{
	imgui2_Static("Textures for particle system");

	char EditTexName[MAX_PATH];
	sprintf ( EditTexName, "Texture             %s", EditTorch->TextureFileName.c_str() );
	if ( imgui2_Static_Checked ( EditTexName ) )
		EnableListPick(PICK_PartTexture);

	if( imgui2_Button( "Update Atlas" ) )
	{
		char drive[ 16 ], path[ 256 ], name[ 256 ], ext[ 256 ];

		_splitpath( EditTorch->TextureFileName.c_str(), drive, path, name, ext );

		UpdateParticleAtlas( "Data/Particles/", name );
	}

	imgui2_Checkbox("UseDistort", &EditTorch->bDistort);
	if(EditTorch->bDistort) {
		char EditDistTexName[MAX_PATH];
		sprintf ( EditDistTexName, "  DistortTex       %s", EditTorch->DistortTextureFileName.c_str());
		if ( imgui2_Static_Checked ( EditDistTexName ) )
			EnableListPick(PICK_DistortTex);
	}

	imgui2_Checkbox("UseNormal", &EditTorch->bNormalMap);

	if(EditTorch->bNormalMap) 
	{
		imgui2_Value_Old(0.0f, 1.0f, "%.4f", "Bump Power", &EditTorch->fBumpPower);

		char EditNormTexName[MAX_PATH];
		sprintf ( EditNormTexName, "  NormalTex       %s", EditTorch->NormalTextureFileName.c_str() );
		if ( imgui2_Static_Checked ( EditNormTexName ) )
			EnableListPick(PICK_NormalTex);
	}

	imgui2_Static("Glow");

	imgui2_Value_Old(0.0f, 4.0f, "%.4f", "Intensity", &EditTorch->GlowIntensity);

	imgui2_Static("Distortion Blue (DistB) or Diffuse Alpha(DiA) used for glow");

	imgui2_Value_Old(0.0f, 1.0f, "%.4f", "DistB <-> DiA", &EditTorch->GlowFromDistortB_or_FromDiffuseA);


	imgui2_Static( "Atlas Creation" );

	static char selectedAtlas[ 512 ] = { 0 };

#define BASE_PARTICLE_PATH "Data/Particles/"

	imgui2_DirList( 360.f, 233.f, BASE_PARTICLE_PATH"*.*", selectedAtlas );

	if( selectedAtlas[ 0 ] && imgui2_Button( "Create Atlas" ) )
	{
		UpdateParticleAtlas( BASE_PARTICLE_PATH, selectedAtlas );		
	}
}

static void DrawBirthOverLifeParams(int SlotID)
{
	r3d_assert(EditTorch->PType[SlotID] != NULL);
	r3dParticleEmitter& PE = *EditTorch->PType[SlotID];

	imgui2_Static("Over life time parameters at BIRTH");

	imgui2_Value( "Time Lapse", &PE.BirthChartsTimeLapse, 0.01f, 30.f, "%.2f" );

	static int CurOverTimeIdx = 0;
	const static char* list[] = { "SIZE" };
	imgui2_Toolbar(&CurOverTimeIdx, 0, list, R3D_ARRAYSIZE(list) );

	switch( CurOverTimeIdx )
	{
	case 0:
		{
			if( imgui2_Button( "Reset Size" ) )
				PE.BirthSizeOverLife.Reset(1);

			static float chartScale = 1.f ;

			imgui2_DrawFloatGradient("Size",  &PE.BirthSizeOverLife, &chartScale, 0, 10.f );
		}
		break;
	}

}

static void DrawOverLifeParams(int SlotID)
{
	r3d_assert(EditTorch->PType[SlotID] != NULL);
	r3dParticleEmitter& PE = *EditTorch->PType[SlotID];

	imgui2_Static("Over life time parameters");

	static int CurOverTimeIdx = 0;
	const static char* list[] = { "COLOR", "SIZE", "SPIN", "GRAVITY", "DRAG", "VELOC", "FRAMES"  };
	imgui2_Toolbar(&CurOverTimeIdx, 0, list, R3D_ARRAYSIZE(list));

	switch (CurOverTimeIdx)
	{
	case	0:
		if(imgui2_Button("Reset Color"))
			PE.ColorOverLife.Reset(255.f);
		imgui2_DrawColorGradient("Color", &PE.ColorOverLife);

		if(imgui2_Button("Reset Opacity"))
			PE.OpacityOverLife.Reset(255.0f);

		imgui2_DrawFloatGradient("Opacity",  &PE.OpacityOverLife, NULL,  0, 255);
		imgui2_Static("Blend Mode");
		imgui2_Indent(1.0f);
		if(imgui2_Button("Reset To Alpha")) {
			PE.BlendModeOverLife.Reset(1);
		}
		if(imgui2_Button("Reset To Addictive")) {
			PE.BlendModeOverLife.Reset(0);
		}
		imgui2_Indent(0.0f);

		imgui2_DrawFloatGradient("BlendMode", &PE.BlendModeOverLife, NULL, 0, 1);
		break;

	case	1:
		{
			if(imgui2_Button("Reset Size"))
				PE.SizeOverLife.Reset(1);

			static float chartScale = 1.f ;

			imgui2_DrawFloatGradient("Size",  &PE.SizeOverLife, &chartScale, 0, 10.f );
		}
		break;

	case	2:
		{
			if(imgui2_Button("Reset Spin"))
				PE.SpinOverLife.Reset(0);

			static float chartScale = 1.f ;

			imgui2_DrawFloatGradient("Spin",  &PE.SpinOverLife, &chartScale, 0, 1000);
		}
		break;

	case	3:
		{
			if(imgui2_Button("Reset Gravity"))
				PE.GravityOverLife.Reset(0);

			static float chartScale = 1.f ;

			imgui2_DrawFloatGradient("Gravity",  &PE.GravityOverLife, &chartScale, -20, 20);
		}
		break;

	case	4:
		{
			if(imgui2_Button("Reset Drag"))
				PE.BindGravityOverLife.Reset(0.0f);

			static float chartScale = 1.f ;

			imgui2_DrawFloatGradient("DRAG", &PE.BindGravityOverLife, &chartScale, 0.f,  1);
		}
		break;

	case	5:
		{
			if(imgui2_Button("Reset Velocity"))
				PE.VelocityOverLife.Reset(1);

			static float chartScale = 1.f ;

			imgui2_DrawFloatGradient("Velocity",  &PE.VelocityOverLife, &chartScale, 0, 20);
		}
		break;

	case	6:
		{
			if(imgui2_Button("Reset Frames"))
				PE.FramesOverLife.Reset(0,1);

			static float chartScale = 1.f ;
			imgui2_DrawFloatGradient("Frames",  &PE.FramesOverLife, &chartScale, 0, 1);
		}
	}

	return;
}

static void DrawOverLifeParamsForLight()
{
	imgui2_Static("Parameters for Attached LIGHT");

	imgui2_Checkbox("Has Light", &EditTorch->HasLight);

	if ( EditTorch->HasLight )
	{
		imgui2_Value_Old(0.01f, 30*60, "%.4f", "Light Lifetime", &EditTorch->LightLifetime);

		imgui2_Value_Old(0.5f, 20, "%.2f", "Light Intensity", &EditTorch->LightIntensity);

		imgui2_Value_Old(0.0f, 20, "%.2f", "Radius 1", &EditTorch->LightRadius1Base);
		imgui2_Value_Old(EditTorch->LightRadius1Base+0.5f, EditTorch->LightRadius1Base+20.0f, "%.2f", "Radius 2", &EditTorch->LightRadius2Base);

		imgui2_Checkbox( "Casts Shadow", &EditTorch->LightCastsShadows );

		if( EditTorch->LightCastsShadows )
		{
			imgui2_Checkbox( "Blur Shadows", &EditTorch->LightSSShadowBlur );

			if( EditTorch->LightSSShadowBlur )
			{
				imgui2_Value_Old( 0.0f, 0.5f, "%-02.2f", "Blur Bias", &EditTorch->SSSBParams.Bias );
				imgui2_Value_Old( 0.0f, 1024.0f, "%-02.2f", "Physicality", &EditTorch->SSSBParams.PhysRange );
				imgui2_Value_Old( 0.0f, 1024.0f, "%-02.2f", "Depth Sens.", &EditTorch->SSSBParams.Sense );
				imgui2_Value_Old( 0.0f, 12.0f, "%-02.2f", "Blur Radius", &EditTorch->SSSBParams.Radius );
			}
		}

		imgui2_Static("Light Position Offset");
			imgui2_Value_Old(-10.0f, 10, "%.2f", "   X", &EditTorch->ParticleLightOffset.X);
			imgui2_Value_Old(-10.0f, 10, "%.2f", "   Y", &EditTorch->ParticleLightOffset.Y);
			imgui2_Value_Old(-10.0f, 10, "%.2f", "   Z", &EditTorch->ParticleLightOffset.Z);
		//if(imgui2_Button("Reset"))
		//	EditTorch->LightRadius1.Reset(1.0f);

		imgui2_Static("Color");
		imgui2_DrawColorGradient("Color", &EditTorch->LightColor);

		imgui2_Static("Light Function");

#if 0
		const static char* lfunction[] = { "CUSTOM", "GLOW", "FIRE" };

		imgui2_Toolbar(&EditTorch->LightFunctionID, 0, lfunction, R3D_ARRAYSIZE(lfunction));

		if (EditTorch->LightFunctionID == 0)
#else
		// force to 0 cause other values are unused anyway
		EditTorch->LightFunctionID = 0 ;
#endif
		{

		if(imgui2_Button("Reset"))
			EditTorch->LightRadius1.Reset(1.0f);
		imgui2_DrawFloatGradient("Radius1 Variation",  &EditTorch->LightRadius1, NULL, 0.0f, 1.0f);

		if(imgui2_Button("Reset"))
			EditTorch->LightRadius2.Reset(1.0f);
		imgui2_DrawFloatGradient("Radius2 Variation",  &EditTorch->LightRadius1, NULL, 0.0f, 1.0f);
		}


	}
}



static void DrawGeneralParams(int SlotID)
{
	r3d_assert(EditTorch->PType[SlotID] != NULL);
	r3dParticleEmitter& PE = *EditTorch->PType[SlotID];

	int prevCastsShadows = PE.bCastsShadows ;
	imgui2_Checkbox( "Casts shadows", &PE.bCastsShadows ) ;

	if( PE.bCastsShadows != prevCastsShadows )
	{
		EditTorch->UpdateCastsShadows() ;
	}

	imgui2_Checkbox("Emitter", &PE.bEmmiter);

	if( PE.bEmmiter )
	{
		imgui2_Checkbox( "Emitter Distance Spawn", &PE.bEmitterDistanceSpawn ) ;
	}

	if(EditTorch->PType[PE.bEmmiterTypeID])
		EditTorch->PType[PE.bEmmiterTypeID]->bUsedAsEmitter = false; // in case if we change our ID while emitter is set to true
	imgui2_Value_Old(0.0f, r3dParticleData::MAX_EMITTER_SLOTS - 1, "%2.0f", "EmitterIndex", &PE.bEmmiterTypeID);
	if(EditTorch->PType[PE.bEmmiterTypeID] && PE.bEmmiterTypeID!=SlotID)
		EditTorch->PType[PE.bEmmiterTypeID]->bUsedAsEmitter = PE.bEmmiter;
	else // not valid slot
		PE.bEmmiter = false;

	imgui2_Checkbox("Single Particle", &PE.bSingleParticle);
	imgui2_Value_Old(0,R3D_MAX( EditTorch->Atlas.count - 1, 0 ), "%3.0f", "Start Frame", &PE.StartFrame);

	static int NumFrames = 0 ;
	NumFrames = PE.GetNumFramesI() ;
	imgui2_Value_Old(1.0f,R3D_MAX( EditTorch->Atlas.count, 1 ), "%3.0f", "Num Frames", &NumFrames );
	PE.SetNumFrames( NumFrames ) ;

	imgui2_Checkbox( "Play Once", &PE.PlayOnce );

	imgui2_Value_Old(0.1f,60.0f, "%.4f", "Frame Rate", &PE.FrameRate,false);
	imgui2_Value_Old(-1.0f,1000.0f, "%.4f", "Start Time", &PE.StartTime,false);
	imgui2_Value_Old(-1.0f,1000.0f, "%.4f", "EndTime", &PE.EndTime,false);

	imgui2_Value_Old(0.01f,1500.0f, "%.2f", "Num", &PE.ParticleBirthRate);

	imgui2_Static("Emitter Local Offset");
	imgui2_Value_Old(-50.0,50.0, "%.4f", "  X", &PE.EmmiterOffset.x);
	imgui2_Value_Old(-50.0,50.0, "%.4f", "  Y", &PE.EmmiterOffset.y);
	imgui2_Value_Old(-50.0,50.0, "%.4f", "  Z", &PE.EmmiterOffset.z);
	imgui2_Static("");
	imgui2_Value_Old(0.01f,50.0f, "%.4f", "Size", &PE.ParticleSize);
	imgui2_Value_Old(0.05f,3600.0, "%.4f", "Life Time", &PE.ParticleLifeTime);
	imgui2_Value_Old(0.0,100.0, "%.4f", "Velocity", &PE.ParticleVelocity);
	imgui2_Value_Old(-200.0,200.0, "%.4f", "Gravity", &PE.ParticleGravity);
	imgui2_Value_Old(-360.0,360.0, "%.4f", "Spin", &PE.ParticleSpin);
	imgui2_Value_Old(0.0,10.0, "%.4f", "Emit Dist Passed", &PE.EmitDistance);
	imgui2_Value_Old(-10.0,10.0, "%.4f", "UV Speed", &PE.UVSpeed);

	imgui2_Static("Born Position Variance");
	imgui2_Value_Old(0.0,100.0, "%.4f", "  X", &PE.BornPosRand.x);
	imgui2_Value_Old(0.0,100.0, "%.4f", "  Y", &PE.BornPosRand.y);
	imgui2_Value_Old(0.0,100.0, "%.4f", "  Z", &PE.BornPosRand.z);
	imgui2_Static("");
	imgui2_Value_Old(0.0,1.0, "%.4f", "Size Variance", &PE.ParticleSizeVar);
	imgui2_Value_Old(0.0,1.0, "%.4f", "Life Variance", &PE.ParticleLifeTimeVar);
	imgui2_Value_Old(0.0,1.0, "%.4f", "Velocity Variance", &PE.ParticleVelocityVar);
	imgui2_Value_Old(0.0,1.0, "%.4f", "Gravity Variance", &PE.ParticleGravityVar);
	imgui2_Value_Old(-360.0,360.0, "%.4f", "Spin Variance", &PE.ParticleSpinVar);
}

static void DrawParams2(int SlotID)
{
	r3d_assert(EditTorch->PType[SlotID] != NULL);
	r3dParticleEmitter& PE = *EditTorch->PType[SlotID];

	imgui2_Static("Random Motion");
	imgui2_Value_Old(0.0,20.0, "%.4f", "  X ", &PE.MotionRand.x);
	imgui2_Value_Old(0.0,20.0, "%.4f", "  Y ", &PE.MotionRand.y);
	imgui2_Value_Old(0.0,20.0, "%.4f", "  Z ", &PE.MotionRand.z);

	imgui2_Static("Random Motion Change Delta");
	imgui2_Value_Old(0.0f,10.0f, "%.3f", "Value", &PE.MotionRandDelta );

	imgui2_Static("Random Motion Change Smoothing");
	imgui2_Value_Old(0.0,1.0f, "%.3f", "Value", &PE.MotionRandSmooth );

	imgui2_Checkbox("Random Emit Direction", &PE.bRandomDirection);
	if ( PE.bRandomDirection )
	{
		imgui2_Static("Random Direction");
		imgui2_Value_Old(0.0,1.0, "%.4f", "  X", &PE.RandomVector.x);
		imgui2_Value_Old(0.0,1.0, "%.4f", "  Y", &PE.RandomVector.y);
		imgui2_Value_Old(0.0,1.0, "%.4f", "  Z", &PE.RandomVector.z);
	}
	else
	{	
		imgui2_Static("Direction Variance");
		imgui2_Value_Old(-1.0,1.0, "%.4f", "  X", &PE.DirectionRand.x);
		imgui2_Value_Old(-1.0,1.0, "%.4f", "  Y", &PE.DirectionRand.y);
		imgui2_Value_Old(-1.0,1.0, "%.4f", "  Z", &PE.DirectionRand.z);
	}
}


static void DrawTypeParams(int SlotID)
{
	r3d_assert(EditTorch->PType[SlotID] != NULL);
	r3dParticleEmitter& PE = *EditTorch->PType[SlotID];
	char buf[256] = {0};

	switch (PE.ParticleType)
	{
	case R3D_PARTICLE_TRAIL:
		imgui2_Value_Old(0.0,50.0, "%.4f", "WIDTH", &PE.RayWidth);
		imgui2_Value_Old(0.0,100.0, "%.4f", "StepDist", &PE.TrailStepDist);
		imgui2_Value_Old(0.0f,8.0f, "%.2f", "Size Coef Min", &PE.TrailSizeCoefMin );
		imgui2_Value_Old(0.0f,8.0f, "%.2f", "Size Coef Max", &PE.TrailSizeCoefMax );
		imgui2_Value_Old(0.0f,8.0f, "%.2f", "Opacity Coef Min", &PE.TrailOpacityCoefMin );
		imgui2_Value_Old(0.0f,8.0f, "%.2f", "Opacity Coef Max", &PE.TrailOpacityCoefMax );
		imgui2_Value_Old(0.0f,1.0f, "%.2f", "Trail Tale Fade", &PE.TrailTaleFade );
		imgui2_Value_Old(0.05f,8.0f, "%.2f", "Trail Tale Fade Pow", &PE.TrailTaleFadePow );
		imgui2_Value_Old(0.0f,0.25f, "%.2f", "Drift", &PE.TrailDrift );
		break;

	case R3D_PARTICLE_CAMERAQUAD:
		break;

	case R3D_PARTICLE_ARBITARYQUAD:
		// DENIS - controls for angles
		// Be sure you linked proper particle system !!!
		imgui2_Checkbox("DirectionOriented", &PE.bDirectionOriented);

		if(imgui2_Button("Reset"))
			PE.AngleXOverLife.Reset(0.0f);
		{
			static float chartScale = 1.f ;
			imgui2_DrawFloatGradient("#AngleX",  &PE.AngleXOverLife, &chartScale, -360, 360);
		}

		if(imgui2_Button("Reset"))
			PE.AngleYOverLife.Reset(0.0f);
		{
			static float chartScale = 1.f ;
			imgui2_DrawFloatGradient("#AngleY",  &PE.AngleYOverLife, &chartScale, -360, 360);
		}
		if(imgui2_Button("Reset"))
			PE.AngleZOverLife.Reset(0.0f);
		{
			static float chartScale = 1.f ;
			imgui2_DrawFloatGradient("#AngleZ",  &PE.AngleZOverLife, &chartScale, -360, 360);
		}
		break;

	case R3D_PARTICLE_RAY:
		imgui2_Value_Old(0.0,25.0, "%.4f", "WIDTH", &PE.RayWidth);
		break;

	case R3D_PARTICLE_BEAM:
		imgui2_Value_Old(0.0,25.0, "%.4f", "WIDTH", &PE.RayWidth);
		break;

	case R3D_PARTICLE_MESH:
		strcpy_s(buf, _countof(buf), PE.MeshFName.c_str());

		if(imgui2_StringValue("Mesh", buf, false))
		{
			PE.MeshFName = buf;
			EnableListPick( PICK_GlobalMeshName );
		}

		imgui2_Checkbox("disable culling", &PE.bMeshDisableCulling);
		imgui2_Checkbox("deferred render", &EditTorch->bDeferredMeshParticles, 1 );

		if( EditTorch->bDeferredMeshParticles )
		{
			imgui2_Checkbox("cast shadows", &EditTorch->bDeferredMeshParticlesShadows, 0 );
		}
		else
		{
			imgui2_Checkbox("Use Distort", &PE.bMeshUseDistortTexture);
		}

		imgui2_Checkbox("ALIGN TO VEC", &PE.bDirectionOriented);

		if(!PE.bDirectionOriented)
		{
			if(imgui2_Button("Reset"))
				PE.AngleXOverLife.Reset(0.0f);
			{
				static float chartScale = 1.f ;
				imgui2_DrawFloatGradient("#AngleX",  &PE.AngleXOverLife, &chartScale, -360, 360);
			}

			if(imgui2_Button("Reset"))
				PE.AngleYOverLife.Reset(0.0f);
			{
				static float chartScale = 1.f ;
				imgui2_DrawFloatGradient("#AngleY",  &PE.AngleYOverLife, &chartScale, -360, 360);
			}

			if(imgui2_Button("Reset"))
				PE.AngleZOverLife.Reset(0.0f);
			{
				static float chartScale = 1.f ;
				imgui2_DrawFloatGradient("#AngleZ",  &PE.AngleZOverLife, &chartScale, -360, 360);
			}
		}
		else
		{
			imgui2_Static ( "Additional Rotation" );
			imgui2_Value_Old(0.0f, 360.0, "%.4f", "  X", &PE.vDirOrientedAdditRotation.x);
			imgui2_Value_Old(0.0f, 360.0, "%.4f", "  Y", &PE.vDirOrientedAdditRotation.y);
			imgui2_Value_Old(0.0f, 360.0, "%.4f", "  Z", &PE.vDirOrientedAdditRotation.z);
		}
		break;

	default:
		break;
	}
}

static const char* ParticleEmitterTypes[] =	{ "CAMERA",	"ARBITARY",	"RAY",	"MESH",	"TRAIL",	"BEAM"	};
enum 										{ CAMERA, 	ARBITRARY,	RAY,	MESH,	TRAIL,		BEAM	};

int ParticleTypeToLocalEnum( int type )
{
	switch( type )
	{
	case R3D_PARTICLE_CAMERAQUAD:
		return CAMERA ;
	case R3D_PARTICLE_ARBITARYQUAD:
		return ARBITRARY ;
	case R3D_PARTICLE_RAY:
		return RAY ;
	case R3D_PARTICLE_MESH:
		return MESH ;
	case R3D_PARTICLE_TRAIL:
		return TRAIL ;
	case R3D_PARTICLE_BEAM:
		return BEAM ;
	};

	r3d_assert( false );
	return CAMERA ;
}

int LocalEnumToParticleType( int type )
{
	switch( type )
	{
	case CAMERA:
		return R3D_PARTICLE_CAMERAQUAD ;
	case ARBITRARY:
		return R3D_PARTICLE_ARBITARYQUAD ;
	case RAY:
		return R3D_PARTICLE_RAY ;
	case MESH:
		return R3D_PARTICLE_MESH ;
	case TRAIL:
		return R3D_PARTICLE_TRAIL ;
	case BEAM:
		return R3D_PARTICLE_BEAM ;
	}

	r3d_assert( false );
	return R3D_PARTICLE_CAMERAQUAD ;
}



static void DrawParticleSlot(int SlotID)
{
	struct ZoneInfo
	{
		float       X, Y;
		float	w;
		float       h;
	};
	ZoneInfo z;
	z.h = 80;
	z.w = 140;
	z.X = pe_slots_x + (SlotID>7?150:0);
	z.Y = pe_slots_y + (SlotID%8) * z.h + (SlotID%8) * 1;

	if(!imgui2_disabled && Mouse->IsPressed(r3dMouse::mLeftButton) && imgui_mx >= z.X && imgui_mx < z.X+z.w && imgui_my > z.Y && imgui_my < z.Y+z.h)
	{
		if (EditTorch)
		{
			r3dParticleEmitter *pe = EditTorch->PType[SlotID];
			if (pe)
			{
				ValueTrackersManager::Instance().SetUpdateMask(ValueTrackersManager::Instance().GetUpdateMask() | TT_PARTICLE_EMITTER_VARS);
				InitParticleEmitterValueTrackers(*pe);
			}
			else
			{
				ValueTrackersManager::Instance().SetUpdateMask(~TT_PARTICLE_EMITTER_VARS & ValueTrackersManager::Instance().GetUpdateMask());
			}
		}
		curEmitterSlot = SlotID;
	}

	r3dColor24 TCol = r3dColor24(255,255,255);
	r3dColor24 BCol = r3dColor24(0,0,0);
	int issel = curEmitterSlot == SlotID;
	if(issel) {
		TCol = r3dColor24(170,170,255);
		BCol = r3dColor24(255,168,0);
		
		static int typeListIdx = 0;
		const static char* typeListNames[] = { "Particle System Properties", "Selected Emitter Properties" };
		const static char* typeListNamesSingle[] = { "Particle System Properties" };
		bool bCurEmitterExists = EditTorch->PType[SlotID] != NULL;
		if ( !bCurEmitterExists ) typeListIdx = 0;

		//g_pDesktopManager->Begin("particle_editor_first");

		imgui2_StartArea("", r3dRenderer->ScreenW - pe_emitter_w, pe_emitter_y, pe_emitter_w - 15, r3dRenderer->ScreenH - 100, false);

		if ( bCurEmitterExists )
			imgui2_Toolbar(&typeListIdx, 0, typeListNames, R3D_ARRAYSIZE(typeListNames));
		else
			imgui2_Toolbar(&typeListIdx, 0, typeListNamesSingle, R3D_ARRAYSIZE(typeListNamesSingle));

		// params for particle system
		if ( typeListIdx == 0 )
		{
			static int globalListIdx = 0;
			const static char* globalListNames[] = { "General", "Textures", "Light" };
			imgui2_Toolbar(&globalListIdx, 0, globalListNames, R3D_ARRAYSIZE(globalListNames));

			g_pDesktopManager->Begin("particle_editor_first");

			switch(globalListIdx)
			{
			case 0:
				DrawGlobalParams();
				break;
			case 1:
				DrawTextureParams();
				break;
			case 2:
				DrawOverLifeParamsForLight ();
				break;
			};

			g_pDesktopManager->End ();
		}
		else // params for selected particle emitter
		{
			r3d_assert ( bCurEmitterExists );
			r3d_assert(EditTorch->PType[SlotID] != NULL);

			r3dParticleEmitter& pe = *EditTorch->PType[SlotID];

			imgui2_Static("");
			static int particleType;
			particleType = ParticleTypeToLocalEnum( pe.ParticleType ) ;

			int prevType = particleType;
			imgui2_ListValue("PARTICLE TYPE", &particleType, ParticleEmitterTypes, R3D_ARRAYSIZE(ParticleEmitterTypes)) ;

			if( prevType != particleType )
			{
				for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
				{
					if( obj->ObjTypeFlags & OBJTYPE_Particle )
					{
						obj_ParticleSystem* ops = static_cast< obj_ParticleSystem* >( obj );

						if( ops->Torch->PD == ParticleObj->Torch->PD )
							ops->Restart();
					}
				}
			}

			pe.ParticleType = LocalEnumToParticleType( particleType );
			if( pe.ParticleType == R3D_PARTICLE_TRAIL )
			{
				pe.bSingleParticle = true ;
				
				ParticleObj->Torch->EmitersTypes |= ( 1 << R3D_PARTICLE_TRAIL ) ;
			}

			imgui2_Static("");

			static int groupListIdx = 0;
			const static char* groupListNames[] = { "General", "Dir&Rnd", "Over Life", "At Birth", "Type Uniq" };
			
			imgui2_Toolbar(&groupListIdx, 0, groupListNames, R3D_ARRAYSIZE(groupListNames));

			g_pDesktopManager->Begin("particle_editor_second");

			switch(groupListIdx)
			{
			case 0:
				DrawGeneralParams(SlotID);
				break;

			case 1:
				DrawParams2(SlotID);
				break;

			case 2:
				DrawOverLifeParams(SlotID);
				break;

			case 3:
				DrawBirthOverLifeParams(SlotID);
				break;
			
			case 4:
				DrawTypeParams(SlotID);
				break;
			}

			g_pDesktopManager->End();
		}

		imgui2_EndArea();

		//g_pDesktopManager->End();
	}
	r3dDrawBox2D(z.X,   z.Y,   z.w,   z.h, BCol);
	r3dDrawBox2D(z.X+2, z.Y+2, z.w-4, z.h-4, r3dColor24(70,70,70));

	if(EditTorch->PType[SlotID] == NULL) 
	{
		if(imgui2_ButtonEx(z.X+20, z.Y+25, 60, 20, "NEW")) {
			SAFE_DELETE(EditTorch->PType[SlotID]);
			EditTorch->PType[SlotID] = gfx_new r3dParticleEmitter;
			r3dParticleEmitter* pe = EditTorch->PType[SlotID];

			char name[128] = "slot0";
			if(SlotID != 0 && EditTorch->PType[0]) r3dscpy(name, EditTorch->PType[0]->Name);

			pe->Load(EditTorch->FileName, name);
			name[strlen(name)-1] = 0;
			sprintf(pe->Name, "%s%d", name, SlotID);
			ValueTrackersManager::Instance().SetUpdateMask(TT_PARTICLE_EMITTER_VARS | ValueTrackersManager::Instance().GetUpdateMask());
			InitParticleEmitterValueTrackers(*pe);

			ParticleEmitterAddDelUndo *undoItem = static_cast<ParticleEmitterAddDelUndo*>(g_pUndoHistory->CreateUndoItem(UA_PARTICLE_EMITTER_ADDDEL, 0));
			undoItem->SetEmitter(&EditTorch->PType[SlotID], SlotID);
			undoItem->SetAsCreateUndo();
		}

		if(havePastedFx) {
			if(imgui2_ButtonEx(z.X+20, z.Y+50, 60, 20, "PASTE")) {
				SAFE_DELETE(EditTorch->PType[SlotID]);
				EditTorch->PType[SlotID] = gfx_new r3dParticleEmitter;
				r3dParticleEmitter* pe = EditTorch->PType[SlotID];

				pe->Copy(&pastedFx, pastedFx.Name);
				if(isdigit(pe->Name[0])) {
					pe->Name[0] = SlotID + '0';
				} else {
					char name[100];
					sprintf(name, "%d_%s", SlotID, pe->Name);
					r3dscpy(pe->Name, name);
				}
				havePastedFx = false;
				ParticleEmitterAddDelUndo *undoItem = static_cast<ParticleEmitterAddDelUndo*>(g_pUndoHistory->CreateUndoItem(UA_PARTICLE_EMITTER_ADDDEL, 0));
				undoItem->SetEmitter(&EditTorch->PType[SlotID], SlotID);
				undoItem->SetAsCreateUndo();
			}
		}

	} 
	else 
	{
		r3dParticleEmitter& PE = *EditTorch->PType[SlotID];

// 		int YFrame = int(PE.StartFrame / EditTorch->XDiv);
// 
// 		float YPos, XPos;
// 		YPos = float(YFrame);
// 		XPos = int(PE.StartFrame - (YPos*EditTorch->XDiv)) % EditTorch->XDiv;

		r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);

		r3dDrawBox2D(z.X+10, 
			z.Y+10, 
			z.h-20,
			z.h-20, 
			r3dColor(0,0,0));

		AtlasDesc::Rect rect = EditTorch->Atlas.rect(PE.StartFrame);
		float tc[8] = {rect.minX, rect.minY, rect.maxX, rect.minY, rect.maxX, rect.maxY, rect.minX, rect.maxY};
		r3dDrawBox2D(z.X+10, 
			z.Y+10, 
			z.h-20,
			z.h-20, 
			r3dColor(255,255,255), EditTorch->Texture, tc);


// 		r3dDrawBox2DTC(z.X+10, 
// 			z.Y+10, 
// 			z.h-20,
// 			z.h-20, 
// 			r3dColor(255,255,255), 
// 			EditTorch->XDiv, EditTorch->YDiv, XPos, YPos, 1, 1, EditTorch->Texture);

		MenuFont_Editor->PrintF(z.X, z.Y, r3dColor(255,255,255), "%s", PE.Name);
		if(!imgui2_disabled && issel && Mouse->IsPressed(r3dMouse::mLeftButton) && imgui_mx > z.X && imgui_mx < z.X+100 && imgui_my > z.Y && imgui_my < z.Y+15) {
			StartTextEdit(eChangeEmitterName, "EmitterName", PE.Name);
		}

		if(imgui2_ButtonEx(z.X+3, z.Y+60, 14, 18, PE.bActive ? "X" : ".")) {
			imgui2_val = EditTorch;
			PE.bActive^=1;
		}

		//if(imgui2_ButtonEx(z.X+80, z.Y+10, 50, 18, "up")) {
		//}

		if ( EditTorch->PType[SlotID]->bEmmiter )
		{
			if ( EditTorch->PType[EditTorch->PType[SlotID]->bEmmiterTypeID] )
				imgui2_StaticEx(z.X+80, z.Y+5, 50, false, false, "E:%s", EditTorch->PType[EditTorch->PType[SlotID]->bEmmiterTypeID]->Name);
		}
		else 
		{
			char sEmit[256] = "S:";
			char sEmitOne[256] = "S:";
			int iExists = 0;
			for ( int i = 0; i < r3dParticleData::MAX_EMITTER_SLOTS; i++ )
			{
				if ( EditTorch->PType[i] && EditTorch->PType[i]->bEmmiter && EditTorch->PType[i]->bEmmiterTypeID == SlotID )
				{
					char sCopy[256];
					r3dscpy ( sCopy, sEmit );
					if ( iExists > 0 )
						sprintf(sEmit,"%s,%i", sCopy, i );
					else
					{
						sprintf(sEmit,"%s%i", sCopy, i );
						sprintf(sEmitOne,"S:%s", EditTorch->PType[i]->Name );
					}
					
					iExists++;
				}
			}

			if ( iExists )
				imgui2_StaticEx(z.X+80, z.Y+5, 50, false, false, ( iExists == 1 ) ? sEmitOne : sEmit );
		}
		if(imgui2_ButtonEx(z.X+80, z.Y+30, 50, 18, "del"))
		{
			g_pUndoHistory->RemoveUndoActionsByTag(TT_PARTICLE_EMITTER_VARS);
			ParticleEmitterAddDelUndo *undoItem = static_cast<ParticleEmitterAddDelUndo*>(g_pUndoHistory->CreateUndoItem(UA_PARTICLE_EMITTER_ADDDEL, 0));
			undoItem->SetEmitter(&EditTorch->PType[SlotID], SlotID);
			undoItem->SetAsDeleteUndo();

			SAFE_DELETE(EditTorch->PType[SlotID]);
			EditTorch->FixSubEmitters();
			if(ParticleObj->Torch)
				ParticleObj->Torch->ClearParticlesOfType( SlotID );
			ValueTrackersManager::Instance().SetUpdateMask(~TT_PARTICLE_EMITTER_VARS & ValueTrackersManager::Instance().GetUpdateMask());
		}

		if(imgui2_ButtonEx(z.X+80, z.Y+50, 50, 18, "copy")) {
			imgui2_val = EditTorch;

			havePastedFx = true;
			pastedFx.Copy(EditTorch->PType[curEmitterSlot], EditTorch->PType[curEmitterSlot]->Name);
		}
		r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);
	}

	// refresh dir
	if ( ParticleObj && ParticleObj->Torch )
		ParticleObj->Torch->Direction = EditTorch->OrgDirection;

	//win::input_Flush();
	g_pDesktopManager->Draw();
}

static void Draw_3D_Window()
{
	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);

	if(bDrawGrid) {
		menuDrawGrid();
	}
}

static void DrawFileInfo()
{
	/*
	imgui2_StartArea("", pe_fileinfo[0], pe_fileinfo[1], pe_fileinfo[2], pe_fileinfo[3], true);
	imgui2_Checkbox("Glow",     &bGlow);
	imgui2_Checkbox("Textured", &bTex);
	imgui2_Checkbox("Back",     &bBackTex);

	EditTorch->bRenderUntextured = !bTex;

	if(bBackTex && imgui2_StringValue("BackTex", BackFileName, false)) {
	EnableListPick(PICK_BackTexture);
	}

	imgui2_Checkbox("MoveSystem", &torchMoving);
	imgui2_EndArea();
	*/	

	return;
}

static int menuDrawButtons(float &x, float y, const char** list, int count)
{
	int ret = -1;
	for(int i=0; i<count; i++) {
		SIZE sz;
		MenuFont_Editor->GetTextExtent(list[i], &sz);

		float w = sz.cx + 20;
		if(imgui2_ButtonEx(x, y, w, 25, list[i], false)) {
			ret = i;
		}

		x += w + 5;
	}

	return ret;
}

void ParticleEditorReloadParticle (const char * szParticle )
{
	char sNameFull[256];
	sprintf(sNameFull,"Data\\Particles\\%s.prt", szParticle);

	void r3dParticleSystemReloadCacheFor ( const char * );
	void ReloadParticles ( const char * );
	r3dParticleSystemReloadCacheFor ( sNameFull );
	ReloadParticles ( szParticle );
}

#define RELOAD_SHADER_BUTTON 0

static void ProcessMenu()
{
	static const char* menuButtons[] = { 
		"Create",
		"Load", 
		"Save", 
		"Save As", 
		"Restart",
		"Reload Textures", 
		"Reset",
	};

	// for bkg draw
	imgui2_StartArea("", 2, 2, r3dRenderer->ScreenW-4, 25+4, true);

	float x = 4;
	float y = 4;

	// View mode
	switch(1 + menuDrawButtons(x, y, menuButtons, R3D_ARRAYSIZE(menuButtons)))
	{
	case eCreate: // create
		void LoadParticle ( const char * );
		LoadParticle ( "empty" );
		//StartTextEdit(eCreate, "Create File ", "no_name");
		break;
	case eLoad: // load
		ScanForFiles();
		EnableListPick(PICK_Particle);
		break;

	case eSave: // save
		extern bool g_bNewParticle;
		if ( !g_bNewParticle )
			saveParticle(EditTorch, EditTorch->FileName);
		else
		{
			char sPartName[MAX_PATH];
			_splitpath(EditTorch->FileName, NULL, NULL, sPartName, NULL );
			StartTextEdit(eSaveAs, "New File Name", g_bNewParticle ? "no_name" : sPartName );
		}
		break;

	case eSaveAs: // save as
		extern bool g_bNewParticle;
		char sPartName[MAX_PATH];
		_splitpath(EditTorch->FileName, NULL, NULL, sPartName, NULL );
		StartTextEdit(eSaveAs, "New File Name", g_bNewParticle ? "no_name" : sPartName );
		break;

	case eRestart: // restart
		ParticleObj->Torch->Restart(r3dGetTime());
		break;

	case eReloadTex: // reload tex
		{
			r3dOutToLog("RELOADING TEXTURE !!!\n");
			if(EditTorch)
			{
				if( EditTorch->Texture )
				{
					EditTorch->Texture->DestroyResources( false );
					EditTorch->Texture->LoadResources();
				}
			}
			break;
		}
	case eReset:
		{
			if ( EditTorch && EditTorch->PType[curEmitterSlot] )
				EditTorch->PType[curEmitterSlot]->Reset ();
		}
		break;

	}


	imgui2_CheckboxEx(x, y, 100, "Textured", &bTex); x += 105;
	ParticleObj->Torch->bRenderUntextured = !bTex;

	imgui2_CheckboxEx(x, y, 100, "MoveSystem", &torchMoving); x += 105;
	x += 100;

	extern bool g_bNewParticle;
	char sFileName [MAX_PATH];
	if ( g_bNewParticle )
		r3dscpy ( sFileName, "new particle(not saved)" );
	else
		r3dscpy ( sFileName, EditTorch->FileName );

	int toAdd = 0;

	if( strlen( sFileName ) > sizeof "Data\\Particles\\" - 1 )
		toAdd = sizeof "Data\\Particles\\" - 1;

	imgui2_StaticEx(x, y, 200, false, false, "File: %s", sFileName + toAdd ); x += 180;

	const int FPS_AVG_SIZE = 21;

	static float prevFPS[ FPS_AVG_SIZE ] = { 0 } ;
	float fps = 1.f / R3D_MAX( r3dGetFrameTime(), 0.00000000001f );

	float fps_avg = 0;

	for( size_t i = 0, e = FPS_AVG_SIZE - 1; i < e; i ++ )
	{
		prevFPS[ i ] = prevFPS[ i + 1 ];

		fps_avg += prevFPS[ i ];
	}

	prevFPS[ FPS_AVG_SIZE - 1 ] = fps;

	fps_avg += fps;

	fps_avg /= FPS_AVG_SIZE ;

	imgui2_StaticEx(x, y, 200, false, false, "FPS: %.2f, Time: %02.02f Particles %d / %d (%d bytes), f: %.2f, u: %.2f", fps_avg, r3dGetTime() - ParticleObj->Torch->StartTime, ParticleObj->Torch->NumAliveParticles, ParticleObj->Torch->CalculatedArraySize, ParticleObj->Torch->CalculatedArraySize * sizeof(r3dSingleParticle), gFillBufferTime * 1000.f, gFillUpdateTime * 1000.f );

	imgui2_EndArea();

	// restart on button was pressed
	if(Keyboard->WasPressed(kbsSpace))
	{
		ParticleObj->Restart();
	}

	return;
}

static bool ProcessTextEdit()
{
	if(ActiveTextEditId == 0)
		return false;

	int sel = 0;

	r3dDrawBox2D(200-5, 200-5, 400+10, 100+10, r3dColor24(255,255,255));
	r3dDrawBox2D(200, 200, 400, 100, r3dColor24(55,55,55));

	if(imgui2_StringValueEx(210, 210, 380, InputLabel, InputLine, true)) {
		sel = ActiveTextEditId;
	}

	if(imgui2_val != InputLine)
		ActiveTextEditId = 0;

	if(sel == 0)
		return true;

	if(*InputLine == '\0')
		return false;

	switch(sel)
	{
	default:
		r3d_assert(0 && "ActiveTextEditId");
		break;

	case eCreate: // create
	case eSaveAs: // save as
		strlwr(InputLine);
		if(strstr(InputLine, ".prt") == NULL) 
			strcat(InputLine, ".prt");

		char Str2[256];
		sprintf(Str2, PARTICLE_DATA_PATH_PATTERN, InputLine);

		r3dscpy(EditTorch->FileName, Str2);
		saveParticle(EditTorch, EditTorch->FileName, sel != eCreate );

		char sPartName[MAX_PATH];
		_splitpath(InputLine, NULL, NULL, sPartName, NULL );
		char sReloadName[MAX_PATH];
		r3dscpy ( sReloadName, ParticleObj->Name.c_str () );

		ParticleObj->Name = sPartName;
		ParticleObj->FileName = sPartName;

		ParticleEditorSetDefaultParticle ( sPartName );

		if ( _stricmp ( sReloadName, sPartName )!=0 )
			ParticleEditorReloadParticle ( sReloadName );
		
		break;

	case eChangeEmitterName: // edit emitter name
		r3dscpy(EditTorch->PType[curEmitterSlot]->Name, InputLine);
		break;
	}

	return false;
}

static void ProcessIMControls()
{
	assert(EditTorch);

	if(ProcessTextEdit())
		return;

	switch(CurMode)
	{
	case 0:	
		if(DrawPickMenu())
			break;

		ProcessMenu();

		for(int i=0;i<r3dParticleData::MAX_EMITTER_SLOTS;i++)
			DrawParticleSlot(i);

		if ( imgui_Button( 4, r3dRenderer->ScreenH-25, 140.0f, 20,  Va( "Undo: %s", g_pUndoHistory->CanUndo() ? g_pUndoHistory->GetUndoInfo() : "empty" ), 0, false) || ( Keyboard->IsPressed( kbsLeftControl ) && Keyboard->WasPressed( kbsZ ) ))
		{
			g_pUndoHistory->Undo();
		}

		if ( imgui_Button( 13 + 140.0f, r3dRenderer->ScreenH-25, 140.0f, 20, Va( "Redo: %s", g_pUndoHistory->CanRedo() ? g_pUndoHistory->GetRedoInfo() : "empty" ), 0, false) || ( Keyboard->IsPressed( kbsLeftControl ) && Keyboard->WasPressed( kbsU ) ) )
		{
			g_pUndoHistory->Redo();
		}

		DrawFileInfo();
		break;

	case 5000: // saved!
	case 5001: // was not able to save
		if(Keyboard->WasPressed(kbsSpace) || Keyboard->WasPressed(kbsEnter) || Keyboard->WasPressed(kbsEsc)) {
			CurMode = 0;
		}
		const char* msg = (CurMode == 5000) ? "Saved!" : "Could not save because file is not checked out";
		r3dColor    clr = (CurMode == 5000) ? r3dColor(255, 255, 255) : r3dColor(255, 0, 0);

		r3dDrawBox2D(200-5, 200-5, 600+10, 100+10, r3dColor24(255,255,255));
		r3dDrawBox2D(200, 200, 600, 100, r3dColor24(55,55,55));
		MenuFont_Editor->PrintF(210,210, clr, msg);
		break;
	}

	return;
}

void LoadParticle ( const char * sPartName )
{
	char sFullName[256];
	sprintf(sFullName, "Data\\Particles\\%s.prt", sPartName);

	char sReloadName[MAX_PATH];
	r3dscpy ( sReloadName, ParticleObj->Name.c_str () );
	SAFE_DELETE(ParticleObj->Torch);
	ParticleObj->Torch = r3dParticleSystemLoad(sFullName);
	ParticleObj->FileName = sPartName;
	ParticleObj->Name = sPartName;

	ParticleEditorSetDefaultParticle ( sPartName );

	if ( _stricmp ( sReloadName, sFullName )!=0 )
		ParticleEditorReloadParticle ( sReloadName );

	EditTorch = const_cast<r3dParticleData*>(ParticleObj->Torch->PD);
	InitParticleValueTrackers(EditTorch);

	if( EditTorch->PType[ 0 ] )
	{
		InitParticleEmitterValueTrackers( *EditTorch->PType[ 0 ] );
	}

	g_pUndoHistory->ClearUndoHistory();

	ParticleObj->Torch->Restart(r3dGetTime());

	//BeamTargetOffset = ParticleObj->Torch->BeamTargetPosition - ParticleObj->Torch->Position;

	if ( DummyTargetObj && ParticleObj )
		DummyTargetObj->SetPosition(ParticleObj->GetPosition () + BeamTargetOffset);
}

static bool DrawPickMenu()
{
	float x  = 0;
	float y  = 20;
	float w  = 450;
	float h  = R3D_MIN( 750, int( r3dRenderer->ScreenH - 133 ) );
	float dy = 25;

	// esc clear picking menu
	if(activeListID_ != PICK_None && Keyboard->IsPressed(kbsEsc)) {
		activeListID_ = PICK_None;
		return false;
	}
	if(activeListID_ == PICK_None) {
		return false;
	}

	// weird variable, if it's true - spells tab will not be drawn
	bool closeTab = true;

	if( activeListID_ == PICK_GlobalMeshName )
	{
		static char selectedDepot[ 512 ] = { 0 };
		static char selectedMesh[ 512 ] = { 0 };

		static float offset0 = 0.0f;
		static float offset1 = 0.0f;

		static int dirSel = 0;

		imgui_DirList( x+20, y, w, h, "Data/ObjectsDepot/", ".sco", selectedDepot, &offset0, &dirSel );

		char path[ 768 ];

		if( selectedDepot[ 0 ] )
		{
			sprintf( path, "%s/*.sco", selectedDepot );
			imgui_FileList( x + w + 22, y, w, h - 36.f, path, selectedMesh, &offset1 );
		}

		if( selectedMesh[ 0 ] )
		{
			if( imgui_Button( x + w + 22, y + h - 33.f, w, 33.f, "Select" ) )
			{
				r3dParticleEmitter& PE = *EditTorch->PType[curEmitterSlot];

				PE.MeshFName = selectedDepot + sizeof "Data/ObjectsDepot/" - 1;
				PE.MeshFName += "/";
				PE.MeshFName += selectedMesh;

				ParticleReloadMesh( PE, ( "Data/ObjectsDepot/" + PE.MeshFName ).c_str(), false );

				activeListID_ = PICK_None;

				return false;
			}
		}

		return true;

		closeTab = false;
	}
	else
	{
		const stringlist_t* listData    = NULL;
		float*              listOffset  = NULL;

		const char* pickInfo     = "";
		const char* outName      = NULL;

		switch(activeListID_) 
		{
		default: assert(0);
		case PICK_None:
			break;

			// picking particle
		case PICK_Particle:
			listData   = &vParticleNames;
			listOffset = &particleNamesOffset;
			pickInfo   = "Select Particle";
			break;


			// picking textures      
		case PICK_MeshTexture:
		case PICK_MeshDistortTexture:
		case PICK_NormalTex:
		case PICK_DistortTex:
		case PICK_PartTexture:
		case PICK_BackTexture:
			listData   = &vTextureNames;
			listOffset = &textureNamesOffset;
			pickInfo   = "Select Texture";
			break;

			// picking meshes
		case PICK_MeshName: 
			listData   = &vMeshNames;
			listOffset = &meshNamesOffset;
			pickInfo   = "Select Mesh";
			break;
		}

		MenuFont_Editor->PrintF(x, y, r3dColor::white, pickInfo);
		y += dy * 2.1f; 

		int idx;
		if(imgui2_DrawList(x+20, y, w, h, *listData, -1, listOffset, &idx) == false) 
			return closeTab;

		// something was selected
		outName      = listData->at(idx).c_str();

		char Str2[256];
		char sPartName[256];
		sprintf(Str2, PARTICLE_DATA_PATH_PATTERN, outName);

		_splitpath(Str2, NULL, NULL, sPartName, NULL );

		switch(activeListID_) 
		{
		default: assert(0);

		case PICK_Particle:
			{
				LoadParticle ( sPartName );
				break;
			}

		case PICK_MeshName:
			{
				r3dParticleEmitter& PE = *EditTorch->PType[curEmitterSlot];
				PE.MeshFName = outName;

				ParticleReloadMesh(PE, Str2, true);
				break;
			}

		case PICK_DistortTex:
			{
				EditTorch->DistortTextureFileName = outName;
				ParticleReloadTex(&EditTorch->DistortTexture, Str2);
				break;
			}

		case PICK_NormalTex:
			{
				EditTorch->NormalTextureFileName = outName;
				ParticleReloadTex(&EditTorch->NormalTexture, Str2);
				break;
			}

		case PICK_PartTexture:
			{
				EditTorch->TextureFileName = outName;
				NotifyReloadPartTexture(Str2);
				break;
			}

		case PICK_BackTexture:
			{
				r3dscpy(BackFileName, Str2);
				ParticleReloadTex(&BackTexture, Str2);
				break;
			}
		}
	}

	if(closeTab) 
		activeListID_ = PICK_None;

	return false;
}




void ParticleEditor_Start_NEW()
{
	extern bool _r3d_ParticleEditorActive;
	_r3d_ParticleEditorActive = true;

	ScanForFiles();

	CurMode = 0;
	curEmitterSlot = 0;

	r3d_assert(ParticleObj);
	ParticleObj->bKill      = false;
	ParticleObj->bKeepAlive = true;
	ParticleObj->Torch->Restart(r3dGetTime());
	EditTorch    = const_cast<r3dParticleData*>(ParticleObj->Torch->PD);
	InitParticleValueTrackers(EditTorch);

	if( EditTorch->PType[ 0 ] )
	{
		InitParticleEmitterValueTrackers(*EditTorch->PType[0]);
	}

	return;
}


void ParticleEditor_Process_NEW()
{
	r3d_assert(ParticleObj->Torch);
	r3d_assert(EditTorch);
	r3d_assert(ParticleObj->Torch->PD == EditTorch);

	// draw these anyways
	void DEBUG_DrawDebugBoxes();
	DEBUG_DrawDebugBoxes();
	void DEBUG_DrawSceneBoxes();
	DEBUG_DrawSceneBoxes();

	void DrawPreGUIHelpers ();
	DrawPreGUIHelpers();

	imgui_Update();
	extern void imgui2_Update();
	imgui2_Update();

	if(Keyboard->WasPressed(kbsG)) bDrawGrid = 1 - bDrawGrid;

	if( torchMoving )
	{
		r3dPoint3D v1 = editTorchPosition;
		torchAnimTime += r3dGetFrameTime() * 1.0f;
		v1.x += sinf(torchAnimTime) * torchMoveRadius;
		if(torchMoveUp) v1.y += sinf(torchAnimTime) * torchMoveRadius / 2;
		v1.z += cosf(torchAnimTime) * torchMoveRadius;
		ParticleObj->SetPosition ( v1 );
	}
	else
	{
		ParticleObj->SetPosition( editTorchPosition );
	}

	ParticleObj->Torch->BeamTargetPosition = ParticleObj->GetPosition() + BeamTargetOffset;

	//ParticleObj->Torch->Update(r3dGetTime());
	/* this isn't needed, because obj_ParticleSystem will restart it
	if(ParticleObj->Torch->bEmit == 0 && ParticleObj->Torch->NumAliveParticles == 0) 
	ParticleObj->Torch->Restart(r3dGetTime());
	*/
	ParticleObj->Torch->bRenderUntextured = !bTex;

	if ( ParticleObj && DummyTargetObj )
	{
		r3dVector vDirOrg = EditTorch->OrgDirection;
		if ( vDirOrg.LengthSq() < FLT_EPSILON )
			vDirOrg = r3dVector ( 1,0,0 );
		vDirOrg.Normalize();
		r3dVector vUpDir;
		r3dVector vRightDir;
		memcpy ( vUpDir.d3dx (), ParticleObj->GetRotationMatrix().m[0], sizeof ( r3dVector ) );
		memcpy ( vRightDir.d3dx (), ParticleObj->GetRotationMatrix().m[1], sizeof ( r3dVector ) );
		r3dVector vLineFrom = ParticleObj->GetPosition();
		float fLineSize = 5.0f;
		r3dCone tCone1;	tCone1.fFOV = D3DX_PI*0.5f;	tCone1.vCenter = vLineFrom + 2.0f * fLineSize * vDirOrg; tCone1.vDir = -vDirOrg;
		r3dCone tCone2;	tCone2.fFOV = D3DX_PI*0.5f;	tCone2.vCenter = vLineFrom + fLineSize * vUpDir; tCone2.vDir = -vUpDir;
		r3dCone tCone3;	tCone3.fFOV = D3DX_PI*0.5f;	tCone3.vCenter = vLineFrom + fLineSize * vRightDir; tCone3.vDir = -vRightDir;

		r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_PUSH);

		r3dDrawUniformLine3D ( vLineFrom, vLineFrom + 2.0f * fLineSize * vDirOrg, gCam, r3dColor::white );
		/*if ( !g_Manipulator3d.PickedObject() )
		{
			r3dDrawUniformLine3D ( vLineFrom, vLineFrom + fLineSize * vUpDir, gCam, r3dColor::red );
			r3dDrawUniformLine3D ( vLineFrom, vLineFrom + fLineSize * vRightDir, gCam, r3dColor::red );
		}*/
		r3dRenderer->Flush();
		r3dDrawConeSolid ( tCone1, gCam, r3dColor::white, fLineSize / 20.0f);
		/*if ( !g_Manipulator3d.PickedObject() )
		{
			r3dDrawConeSolid ( tCone2, gCam, r3dColor::red, fLineSize / 20.0f);
			r3dDrawConeSolid ( tCone3, gCam, r3dColor::red, fLineSize / 20.0f);
		}*/

		if ( EditTorch->PType[curEmitterSlot] != NULL )
		{
			if ( EditTorch->PType[curEmitterSlot]->ParticleType == R3D_PARTICLE_BEAM )
				r3dDrawUniformLine3D ( ParticleObj->GetPosition(), DummyTargetObj->GetPosition(), gCam, /*0.2f,*/ r3dColor::red );		
		}

		if ( EditTorch )
		{
			switch ( EditTorch->EmitterType )
			{
			case R3D_EMITTYPE_LINE:
				r3dDrawUniformLine3D ( ParticleObj->GetPosition(), ParticleObj->GetPosition() + EditTorch->EmitVector, gCam, r3dColor ( 255, 255, 0 ) );
				break;
			case R3D_EMITTYPE_RING:
			case R3D_EMITTYPE_SPHERE:
				r3dDrawUniformCircle3D ( ParticleObj->GetPosition(), EditTorch->EmitRadius, gCam, r3dColor ( 255, 255, 0 ) );
				break;
			};

			if ( EditTorch->PType[curEmitterSlot] )
			{
				r3dParticleEmitter const & pe = *EditTorch->PType[curEmitterSlot];
				if(!pe.bRandomDirection)
				{
					if ( pe.DirectionRand.LengthSq() > 0.00001f )
					{
						r3dVector vPos = ParticleObj->GetPosition ();
						r3dVector vDir = EditTorch->OrgDirection;
						r3dVector vDirRand = pe.DirectionRand;
						r3dVector vDirNorm = vDir;
						vDirNorm.Normalize();

						D3DXMATRIX mRot, mRotInv;

						D3DXMatrixIdentity ( &mRot );

						r3dVector vR = ( fabsf ( vDirNorm.Dot (r3dVector(1,0,0)) ) > 0.9f ? r3dVector(0,0,1) : r3dVector(1,0,0));
						r3dVector vF = vR.Cross ( vDirNorm );
						vF.Normalize ();
						vR = vDirNorm.Cross ( vF );
						vR.Normalize();

						memcpy ( mRot.m[0], vR.d3dx (), sizeof (r3dVector));
						memcpy ( mRot.m[1], vDirNorm.d3dx (), sizeof (r3dVector));
						memcpy ( mRot.m[2], vF.d3dx (), sizeof (r3dVector));

						D3DXMatrixInverse(&mRotInv, NULL, &mRot );
						r3dVector vDirRandTransf;
						D3DXVec3TransformNormal ( vDirRandTransf.d3dx(), vDirRand.d3dx (), &mRotInv );

						vDirRandTransf.x = fabsf ( vDirRandTransf.x );
						vDirRandTransf.y = fabsf ( vDirRandTransf.y );
						vDirRandTransf.z = fabsf ( vDirRandTransf.z );

						float fDirLen = vDir.Length();

						r3dVector vOffset1 = r3dVector ( 1.0f * vDirRandTransf.x, 0.0f, 1.0f * vDirRandTransf.z );
						r3dVector vOffset2 = r3dVector ( 1.0f * vDirRandTransf.x, 0.0f, -1.0f * vDirRandTransf.z );
						r3dVector vOffset3 = r3dVector ( -1.0f * vDirRandTransf.x, 0.0f, -1.0f * vDirRandTransf.z );
						r3dVector vOffset4 = r3dVector ( -1.0f * vDirRandTransf.x, 0.0f, 1.0f * vDirRandTransf.z );
						D3DXVec3TransformNormal ( vOffset1.d3dx(), vOffset1.d3dx (), &mRot );
						D3DXVec3TransformNormal ( vOffset2.d3dx(), vOffset2.d3dx (), &mRot );
						D3DXVec3TransformNormal ( vOffset3.d3dx(), vOffset3.d3dx (), &mRot );
						D3DXVec3TransformNormal ( vOffset4.d3dx(), vOffset4.d3dx (), &mRot );

						const float fSize = 10.0f;
						const float fDirScl = r3dTL::Max(0.001f, fDirLen - vDirRandTransf.y);

						//vDir = vDirNorm;
						r3dVector vDir1 = fDirScl*vDir+vOffset1; vDir1.Normalize();
						r3dVector vDir2 = fDirScl*vDir+vOffset2; vDir2.Normalize();
						r3dVector vDir3 = fDirScl*vDir+vOffset3; vDir3.Normalize();
						r3dVector vDir4 = fDirScl*vDir+vOffset4; vDir4.Normalize();


						r3dDrawUniformLine3D(vPos, vPos+fSize*vDir1, gCam, r3dColor::red );
						r3dDrawUniformLine3D(vPos+fSize*vDir1, vPos+fSize*vDir2, gCam, r3dColor::red );
						
						r3dDrawUniformLine3D(vPos, vPos+fSize*vDir2, gCam, r3dColor::red );
						r3dDrawUniformLine3D(vPos+fSize*vDir2, vPos+fSize*vDir3, gCam, r3dColor::red );
						
						r3dDrawUniformLine3D(vPos, vPos+fSize*vDir3, gCam, r3dColor::red );
						r3dDrawUniformLine3D(vPos+fSize*vDir3, vPos+fSize*vDir4, gCam, r3dColor::red );
						
						r3dDrawUniformLine3D(vPos, vPos+fSize*vDir4, gCam, r3dColor::red );
						r3dDrawUniformLine3D(vPos+fSize*vDir4, vPos+fSize*vDir1, gCam, r3dColor::red );
					}
				}
				else
				{
					r3dVector vPos = ParticleObj->GetPosition ();
					r3dVector vDir = EditTorch->OrgDirection;
					r3dVector vSize = pe.RandomVector;
					const float fSize = 10.0f;
					r3dDrawUniformSphere(vPos, fSize, gCam, r3dColor::red, 6, 12, vSize);
				}
			}

		}

		if(torchMoving) 
		{
			const int iSteps = 90;
			r3dPoint3D vOldPos = editTorchPosition;
			for ( int i = 0; i < iSteps; i++ )
			{
				r3dPoint3D v1 = editTorchPosition;
				float torchAnimTime = D3DX_PI * 2.0f * float(i)/float(iSteps-1);
				v1.x += sinf(torchAnimTime) * torchMoveRadius;
				if(torchMoveUp) v1.y += sinf(torchAnimTime) * torchMoveRadius / 2;
				v1.z += cosf(torchAnimTime) * torchMoveRadius;
				
				if ( i > 0 )
					r3dDrawUniformLine3D ( vOldPos, v1, gCam, r3dColor ( 255, 0, 255 ) );
			
				vOldPos = v1;
			}
		}
		

#if ( 0 )
		if(EditTorch->PType[curEmitterSlot] != NULL && !EditTorch->PType[curEmitterSlot]->bRandomDirection )
		{
			r3dParticleEmitter& PE = *EditTorch->PType[curEmitterSlot];
			
			r3dCone cone; 
			cone.vCenter = ParticleObj->GetPosition();
			cone.vDir = vDirOrg;
			cone.fFOV = D3DX_PI / 3.0f;
			r3dDrawConeSolid( cone, gCam, r3dColor(255, 255, 255, 200), 15.0f );
		}
#endif
		r3dRenderer->SetRenderingMode( R3D_BLEND_POP);
	}

	Draw_3D_Window();

	r3dSetFiltering( R3D_POINT );

	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);


	ProcessIMControls();
}

void UpdateParticleAtlas( const char* basePath, const char* atlasName )
{
	STARTUPINFO si = {0};
	si.cb = sizeof si;

	char fullPath[ 512 ];
	strcpy( fullPath, basePath );
	strcat( fullPath, atlasName );

	if( !r3dDirectoryExists( fullPath ) )
	{
		MessageBoxA( r3dRenderer->HLibWin, "Couldn't find source textures!", "Error", MB_ICONERROR );
		return;
	}

	int res = ComposeAtlas( basePath, atlasName );

	if( res )
		MessageBoxA( r3dRenderer->HLibWin, "Created!", "Message", MB_OK );
	else
		MessageBoxA( r3dRenderer->HLibWin, "Failed to create!", "Error", MB_ICONERROR );

#if 0
	si.hStdOutput = CreateFile( "nul",	// file name 
								GENERIC_WRITE,          // open for reading 
								0,                     // do not share 
								NULL,                  // default security 
								CREATE_NEW,         // existing file only 
								FILE_ATTRIBUTE_NORMAL, // normal file 
								NULL );                 // no template 

	si.hStdError = (HANDLE) STD_ERROR_HANDLE;
	si.hStdInput = (HANDLE) STD_INPUT_HANDLE;
	si.dwFlags = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION pi;

	char cmdLine[ 512 ];
	sprintf( cmdLine, "AtlasComposer.Release.exe %s", path );

	if( !CreateProcess( NULL,							// No module name (use command line)
						cmdLine,						// Command line
						NULL,							// Process handle not inheritable
						NULL,							// Thread handle not inheritable
						FALSE,							// Set handle inheritance to FALSE
						0,								// No creation flags
						NULL,							// Use parent's environment block
						NULL,							// Use parent's starting directory 
						&si,							// Pointer to STARTUPINFO structure
						&pi )							// Pointer to PROCESS_INFORMATION structure
						) 
	{
		MessageBoxA( r3dRenderer->HLibWin, "Failed to start atlas tool", "Error", MB_ICONERROR );
		return;
	}

	WaitForSingleObject( pi.hProcess, INFINITE );
	MessageBoxA( r3dRenderer->HLibWin, "Started the atlas tool", "Success", MB_OK );
#endif
}

#include <windows.h>
#include <string>

#pragma warning(pop)


#endif // FINAL_BUILD