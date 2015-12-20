#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "UI/UIimEdit2.h"

#include "m_Main.h"

#include "GameLevel.h"

#include "TrueNature2/Terrain3.h"
#include "../../../Eternity/UndoHistory/UndoHistory.h"

Menu_Main::Menu_Main()
{
}

Menu_Main::~Menu_Main()
{
}



void Menu_Main::Draw()
{

  return;
}

enum CreateTerrainType
{
	CREATE_TERRAIN_NONE,
	CREATE_TERRAIN_1,
	CREATE_TERRAIN_2,
	CREATE_TERRAIN_3
};


				  static int __CreateTerrainType = 0;
				  static int __CreateMesh = 0;
				  static int __TerrainSize = 0;
				  static int __TerrainSize_Z = 0;
				  static float __TerrainSizeCell = 1.0f;
				  static int __TerrainSMapSize = 0;
				  static float __TerrainStartHeight = 100.0f;
				  static float __TerrainHeightRange = 512.0f;
				  static float __TerrainStepY = 1.0f;
				  static char HMapName[64];
				  static char HMeshName[64];

char LevelEditName[256];

void SaveLevelData( char* Str );

static void* ____DummyObjectConstructor()
{
	return NULL;
}

bool gNewLevelCreated = false ;

bool CreateNewLevel()
{
#ifndef FINAL_BUILD
	char Path[256];
	char Str[256];

	gNewLevelCreated = true ;

	{
		char tmpStr[512];
		r3dscpy(tmpStr, LevelEditName);
		sprintf(LevelEditName, "WorkInProgress\\%s", tmpStr);
	}

	sprintf(Path, "Levels\\%s", LevelEditName );
	if ( mkdir(Path) == -1 )
	{
		if ( errno == EEXIST )
		{
			r3dOutToLog( "Map with name \"%s\" already exist.\n", LevelEditName );
			return false;
		}
	}

	if( __CreateTerrainType != CREATE_TERRAIN_NONE )
	{
		r3d_assert(g_pPhysicsWorld == 0);
		g_pPhysicsWorld = game_new PhysXWorld();
		g_pPhysicsWorld->Init();

		r3dGameLevel::SetHomeDir( LevelEditName);

		if( __CreateTerrainType == CREATE_TERRAIN_3 )
		{
			r3dTerrain3::CreationParams params ;

			params.StartHeight = __TerrainStartHeight;
			params.HeightRange = __TerrainHeightRange;

			params.CellCountX = int( pow( 2.0f, 8.0f + __TerrainSize ) ) ;
			params.CellCountZ = int( pow( 2.0f, 8.0f + __TerrainSize_Z ) ) ; ;
			params.MaskSizeX = int( pow( 2.0f, 8.0f + __TerrainSMapSize ) ) ;
			params.MaskSizeZ = params.MaskSizeX * params.CellCountZ / params.CellCountX ;

			params.CellSize = __TerrainSizeCell ;
			params.LevelDir = r3dString( "Levels\\" ) + LevelEditName + "\\" ;;

			r3dTerrain3 createTerra;

			r3dTerrain3Layer lay = createTerra.GetLayer(0);
			lay.DiffuseTex = r3dRenderer->LoadTexture("Data/TerrainData/Materials/RefTextures/1_white.dds");
			createTerra.SetLayer(0, lay);

			createTerra.SaveEmpty( params ) ;
		}

		if( __CreateTerrainType == CREATE_TERRAIN_2 )
		{
			r3dTerrain2::CreationParams params ;

			params.CellCountX = int( pow( 2.0f, 8.0f + __TerrainSize ) ) ;
			params.CellCountZ = params.CellCountX ;
			params.SplatSizeX = int( pow( 2.0f, 8.0f + __TerrainSMapSize ) ) ;
			params.SplatSizeZ = params.SplatSizeX ;

			params.CellSize = __TerrainSizeCell ;
			params.LevelDir = r3dString( "Levels\\" ) + LevelEditName + "\\" ;;
			
			r3dTerrain2 createTerra ;

			r3dTerrainLayer lay = createTerra.GetLayer(0);
			lay.DiffuseTex = r3dRenderer->LoadTexture("Data/TerrainData/Materials/RefTextures/1_white.dds");
			createTerra.SetLayer(0, lay);

			createTerra.SaveEmpty( params ) ;
		}

		GameWorld().Init(10,10);
		obj_Terrain* pObjTerrain = game_new obj_Terrain();
		obj_Terrain& objTerrain = *pObjTerrain;
		objTerrain.DrawOrder      = OBJ_DRAWORDER_FIRST;
		objTerrain.ObjFlags      |= OBJFLAG_SkipCastRay;
		objTerrain.FileName = "terra1";
		AClass classData( NULL, "obj_Terrain", "Type", ____DummyObjectConstructor );
		classData.Name = "obj_Terrain";
		objTerrain.Class = &classData;
		GameWorld().AddObject( pObjTerrain );

		SaveLevelData( Str );
		GameWorld().Destroy();

		g_pPhysicsWorld->Destroy();
		SAFE_DELETE(g_pPhysicsWorld);

		delete g_pUndoHistory;
		g_pUndoHistory = NULL;
	}

#endif
	return true;
}


void ClearFullScreen_Menu();

extern bool g_bExit;
int Menu_Main::DoModal()
{
 LevelEditName[0] = 0;
 char tempName[256] = {0};

  while(1)
  {
	  if(g_bExit)
		  return 0;
    r3dStartFrame();

	ClearFullScreen_Menu();

	mUpdate();

	imgui_Update();
	imgui2_Update();

	int ret = 1;

	mDrawStart();

	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);
	r3dSetFiltering( R3D_POINT );

	const static char* list[3] = { "LIVE MAPS", "EDITOR MAPS", "CREATE MAP"};


	static int LevelIndex = 0;

	int lastLevelIndex = LevelIndex;

    imgui_Toolbar(r3dRenderer->ScreenW/2-250, r3dRenderer->ScreenH/2-150-40, 500, 40, &LevelIndex, 0, list, 3);
    r3dDrawBox2D(r3dRenderer->ScreenW/2-250, r3dRenderer->ScreenH/2-150+2, 500, 350, imgui_bkgDlg);

	switch (LevelIndex)
	{
			case	0: // LIVE MAPS
				{
					static float offset = 0.f ;
					imgui_FileList(r3dRenderer->ScreenW/2-240, r3dRenderer->ScreenH/2-140+2, 360, 330, "Levels\\*.", LevelEditName, &offset );

					if (LevelEditName[0] && stricmp(LevelEditName, "workinprogress")!=0)
						if (imgui_Button(r3dRenderer->ScreenW/2+250-150, r3dRenderer->ScreenH/2+200+5,150,30, "Load Level", 0))
							released_id = bEditor;
				}
				break;

			case	1: // EDITOR MAPS
				{
					static float offset = 0.f ;
					imgui_FileList(r3dRenderer->ScreenW/2-240, r3dRenderer->ScreenH/2-140+2, 360, 330, "Levels\\WorkInProgress\\*.", tempName, &offset );

					if (tempName[0])
					{
						sprintf(LevelEditName, "WorkInProgress\\%s", tempName);
						if (imgui_Button(r3dRenderer->ScreenW/2+250-150, r3dRenderer->ScreenH/2+200+5,150,30, "Load Level", 0))
							released_id = bEditor;
					}
				}
				break;

			case	2:
				{
					if( lastLevelIndex != LevelIndex )
					{
						r3dscpy( LevelEditName, "NewLevel" );
					}

					float SliderX = r3dRenderer->ScreenW/2-240;
					float SliderY = r3dRenderer->ScreenH/2-140+2;

					const float MAP_NAME_HEIGHT = 24;

					imgui2_StartArea ( "MAP_NAME_AREA", SliderX, SliderY, 360.f, MAP_NAME_HEIGHT );
					imgui2_StringValue ( "MAP NAME", LevelEditName );
					imgui2_EndArea ();

					SliderY += MAP_NAME_HEIGHT;

					static float terraListOffset = 0.f;

					stringlist_t terraList;

					terraList.push_back( "No Terrain" );
					terraList.push_back( "Terrain 1" );
					terraList.push_back( "Terrain 2" );
					terraList.push_back( "Terrain 3" );

					const float HEIGHT = 85.0f;

					imgui_DrawList( SliderX, SliderY, 360.0f, HEIGHT, terraList, &terraListOffset, &__CreateTerrainType );

					SliderY += HEIGHT;

					if( __CreateTerrainType )
					{
						const static char* list[7] = { "256", "512", "1024", "2048", "4096", "8192", "16384" };

						if( __CreateTerrainType == CREATE_TERRAIN_3 )
						{
							SliderY += imgui_Value_Slider( SliderX, SliderY, "Terrain height range", &__TerrainHeightRange,	0, 10000, "%-02.2f", 1 );
						}

						int isMega = __CreateTerrainType == CREATE_TERRAIN_3;

						SliderY += imgui_Static(SliderX, SliderY, "Terrain Size");
						SliderY += imgui_Value_Slider(SliderX, SliderY, "Cell size in Meters",			&__TerrainSizeCell,	1,100,	"%-02.2f",1);
						imgui_Toolbar(SliderX, SliderY, 360, 35, &__TerrainSize, 0, list, isMega ? 7 : 5 );
						SliderY += 36;

						if( isMega )
						{
							imgui_Toolbar(SliderX, SliderY, 360, 35, &__TerrainSize_Z, 0, list, 7 );
							SliderY += 36;
						}

						if( isMega )
						{
							__TerrainSMapSize = __TerrainSize;
						}
						else
						{
							SliderY += imgui_Static(SliderX, SliderY, "Splat map Size");

							__TerrainSMapSize = R3D_MIN( __TerrainSMapSize, __TerrainSize  ) ;

							imgui_Toolbar(SliderX, SliderY, 360, 35, &__TerrainSMapSize, 0, list, __TerrainSize + 1 );
							SliderY += 36;
						}

						SliderY += imgui_Value_Slider(SliderX, SliderY, "Terrain height in Meters",	&__TerrainStartHeight,	0,1200,	"%-02.2f",1);
					}

				}
				if (imgui_Button(r3dRenderer->ScreenW/2+250-150, r3dRenderer->ScreenH/2+200+5,150,30, "Create Level", 0))
				{
					if ( CreateNewLevel() )
					{
						released_id = bEditor;
					}
				}

				break;
	}


	mDrawEnd();

	switch(released_id)
	{
		case bEditor:
				   return bEditor;
	};

    r3dEndFrame();
  }

  return 0;
}
