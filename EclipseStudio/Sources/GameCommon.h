#ifndef __GAMECOMMON_H
#define __GAMECOMMON_H

#include "GameObjects\GameObj.h"
#include "GameObjects\ObjManag.h"
#include "gameobjects\PhysXWorld.h"
#include "gameobjects\ApexWorld.h"

#include "TrueNature\skydome.h"
#include "TrueNature\sun.h"

#include "ObjectsCode\World\Building.h"

#include "UI\r3dMenu.h"
#include "UI\UIimEdit.h"
#include "UI\UIimEdit2.h"

#include "UI\HUD_Base.h"

#include "LangMngr.h"

#include "r3dAtmosphere.h"

#include "ObjectsCode\World\Lake.h"
#include "ObjectsCode\Nature\obj_Terrain.h"

extern	r3dSun		*Sun;
extern r3dSkyDome	*SkyDome;


extern r3dLightSystem	WorldLightSystem;
extern float 		RenderDistance;


extern CD3DFont 	*Font_Label;
extern CD3DFont 	*Font_Editor;

extern r3dCamera 	gCam;

extern r3dRenderPipeline*	CurRenderPipeline;


#include "fmod/soundsys.h"
enum SoundGroup_e {
  // all sound groups starts with SOUND_GROUP_START enum (1000, as in SoundSys.h)
  SOUND_GROUP_A = SOUND_GROUP_START,
  SOUND_GROUP_B,
  SOUND_GROUP_C,
  SOUND_GROUP_D,
  SOUND_GROUP_E,
};


enum GGameState_s {
		GAMESTATE_PREGAME,
		GAMESTATE_SPAWN,
		GAMESTATE_GAMELOOP,
		GAMESTATE_MOVIE,
		GAMESTATE_PAUSE,
		GAMESTATE_ENDGAME,
		GAMESTATE_AFTERMATH,
		GAMESTATE_AFTERMATHEND,
		GAMESTATE_EXIT,
	};

enum GGameExitCode_s {
		EXITCODE_NONE,
		EXITCODE_WIN,
		EXITCODE_LOSE,
		EXITCODE_ABANDON,
	};


extern GGameState_s	CurrentGameMode;
extern GGameExitCode_s	GameExitCode;




void LM_Set(float F, char *Str);
void LM_Add(float F, char *Str);
void LM_Init();
void LM_Unload();
void LM_DrawStart();

extern int bRenderReflection;
extern int bRenderRefraction;

#include "Rendering\DX9\RenderDX9.h"
#include "Rendering\Deffered\RenderDeffered.h"

#define GLOBAL_ANIM_FOLDER "data\\Animations5"

#endif __GAMECOMMON_H
