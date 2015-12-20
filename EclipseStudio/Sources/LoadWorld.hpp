#include "ObjectsCode\World\Lamp.h"

r3dColor BloomColor = r3dColor(155, 125, 105);
r3dColor BloomColor1 = r3dColor(255,255,255);
r3dColor BloomColor2 = r3dColor(0,0,0);
r3dColor TintColor = r3dColor(0,0,0,0);
float 	RenderDistance = 123000;






void LoadWorldLights(const char* Name);



static void LoadWorld(const char* Name, float startLoadingProgress, int maxPlayers, bool isMenuLevel )
{
 char 	TempStr1[128];

 SetLoadingProgress( startLoadingProgress );

 //**********************************************************
 //*
 //* Load sound and material properties mapper for current level
 //*
 //**********************************************************
 sprintf(TempStr1,"%s\\soundmap.ini", Name);
 //pwSoundScheme.Load(TempStr1);

 sprintf(TempStr1,"%s\\Sounds", Name);
// snd_LoadSoundEffects(TempStr1, "Sound.Dat");

 //**********************************************************
 //*
 //* Initialize environment ( fog, effects, sun )
 //*
 //**********************************************************
 Sun = NULL;

 sprintf(TempStr1,"%s\\sun.ini", Name);
 Sun = gfx_new r3dSun;
 Sun->Init();

 if (Sun->bLoaded)
 {
	 SunVector = GetEnvLightDir();

  Sun->SunLight.bCastShadows = 1;

//  Sun->SunLight.LightFunction = R3D_LIGHTFUNC_THUNDER;
  Sun->SunLight.LightFunctionParams[1] = 5;
  Sun->SunLight.LightFunctionParams[2] = 3;
  Sun->SunLight.LightFunctionParams[6] = 0.5f;

  WorldLightSystem.Add(&Sun->SunLight);
 }

 //**********************************************************
 //*
 //* Initialize dynamic lights
 //*
 //**********************************************************
 // LoadWorldLights(Name);

 SetLoadingPhase( "Loading" );
 SetLoadingProgress( startLoadingProgress + PROGRESS_LOAD_LEVEL_START * 0.33F );

 SkyDome = gfx_new r3dSkyDome( r3dIntegrityGuardian() );
 SkyDome->Load(Name);

 SetLoadingProgress( startLoadingProgress + PROGRESS_LOAD_LEVEL_START * 0.66F );

 LightPreset::LoadFromScript( FNAME_LIGHT_PRESETS );

 SetLoadingProgress( startLoadingProgress + PROGRESS_LOAD_LEVEL_START );


 //**********************************************************
 //*
 //* Load level geometry data - gameworld, terrain, sky, etc
 //*
 //**********************************************************

 extern  int LoadLevel( float startLoadingProgress );

 LoadLevel( startLoadingProgress + PROGRESS_LOAD_LEVEL_START );

 // volume fog params for skydome
 r3dColor	fogColor  = r3dGameLevel::Environment->Fog_Color.GetColorValue(r3dGameLevel::Environment->__CurTime/24.0f);
 float		fogHeight = r3dGameLevel::Environment->Fog_HeightFadeStart.GetFloatValue(r3dGameLevel::Environment->__CurTime/24.0f);	
 float		fWindFactor = r3dGameLevel::Environment->SkyDomeWindFactor;

 SkyDome->SetParams(fogHeight, 0.0f, fogColor,fWindFactor);

	GameObject* obj = NULL;

	r3dPoint3D pos = r3dPoint3D(0,0,0);

  obj = srv_CreateGameObject("obj_Tree", "Tree1", pos);

  SetLoadingProgress( PROGRESS_LOAD_LEVEL_END );
 
  r3dOutToLog( "Building skeleton cache.." );
  InitializePhysSkeletonCache( maxPlayers, PROGRESS_LOAD_LEVEL_END, PLAYER_CACHE_INIT_END ) ;
  r3dOutToLog( "\n" );
  
  r3dOutToLog("Building obstacles cache..");
  InitializePhysObstacleCache(512, 512);
  r3dOutToLog( "\n" );
  

  r3dOutToLog( "done.\n" );

 return;  
}

