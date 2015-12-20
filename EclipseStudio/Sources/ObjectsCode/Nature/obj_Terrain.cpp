#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "GameLevel.h"
#include "obj_Terrain.h"

//CQuadTerrain	*Terrain;


IMPLEMENT_CLASS(obj_Terrain, "obj_Terrain", "Object");
AUTOREGISTER_CLASS(obj_Terrain);

obj_Terrain		*objTerrain = NULL;

//
//
//
//
//-----------------------------------------------------------------------
BOOL obj_Terrain::Load(const char* fname)
//-----------------------------------------------------------------------
{
	DrawOrder	= OBJ_DRAWORDER_FIRST;

	ObjFlags	|= OBJFLAG_SkipCastRay;
	setSkipOcclusionCheck(true);
	ObjFlags	|= OBJFLAG_DisableShadows;

	ObjTypeFlags |= OBJTYPE_Terrain;

	char Str[256];

	sprintf(Str, "%s\\TERRAIN", r3dGameLevel::GetHomeDir());

	Name     = "r3dTerrain"; //Str;
	FileName = "terra1";

	r3dOutToLog ("TERRAIN:  LOADING %s\n", Str);

	char filePath[ 512 ] ;
	sprintf( filePath, "%s\\terrain.heightmap", r3dGameLevel::GetHomeDir() ) ;

	bool have_terrain1 = r3dFileExists( filePath ) ;

	bool loaded_terrain2 = false;
	bool loaded_terrain3 = false;

	sprintf( filePath, "%s\\TERRAIN2\\terrain2.bin", r3dGameLevel::GetHomeDir() ) ;

	if( r3dFileExists( filePath ) ) // if terrain2 exists, always load terrain2
	{
		Terrain2 = gfx_new r3dTerrain2 ;
		if( Terrain2->Load() )
		{
			Terrain2->SetPhysUserData( this ) ;

			Terrain = Terrain2 ;

			r_terrain2->SetInt( 1 ) ;

			FileName = "terra2";

			loaded_terrain2 = true ;
		}
		else
		{
			delete Terrain2 ;
		}
	}

if ( !Terrain2 )
{
	sprintf( filePath, "%s\\TERRAIN3\\terrain3.ini", r3dGameLevel::GetHomeDir() ) ;

	if( r3dFileExists( filePath ) )
	{
		Terrain3 = gfx_new r3dTerrain3;

		if( Terrain3->Load() )
		{
			Terrain3->SetPhysUserData( this );

			Terrain = Terrain3;

			r_terrain3->SetInt( 1 );

			FileName = "terra3";

			loaded_terrain3 = true ;
		}
		else
		{
			delete Terrain3;
			Terrain3 = NULL;
		}
	}
	
	if( !loaded_terrain3 )
	{
		r3d_assert(0);
	}
}

	r3dOutToLog ("TERRAIN:  LOADED\n");

	objTerrain = this;
	return TRUE;
}


BOOL obj_Terrain::OnCreate()
{
	parent::OnCreate();

	if(m_SceneBox)
		m_SceneBox->Remove(this);
	m_SceneBox = 0;

	return TRUE;
}



BOOL obj_Terrain::OnDestroy()
{
  SAFE_DELETE(Terrain);

  objTerrain = NULL;

  return parent::OnDestroy();
}


BOOL obj_Terrain::Update()
{
  return TRUE;
}


void obj_Terrain::ReadSerializedData(pugi::xml_node& node)
{
}

void obj_Terrain::WriteSerializedData(pugi::xml_node& node)
{
}


void DrawTerrain()
{
#ifndef WO_SERVER
	extern float __WorldRenderBias;

	if( Terrain3 && r_terrain3->GetInt() )
	{
		R3DPROFILE_FUNCTION("Draw Terrain3");
		Terrain3->Render( gCam ) ;
	}

	if( Terrain2 && r_terrain2->GetInt() )
	{
		R3DPROFILE_FUNCTION("Draw Terrain2");
		Terrain2->Render( gCam ) ;
	}
#endif
}


void UpdateTerrain3()
{
#ifndef WO_SERVER
	if( Terrain3 )
	{
		Terrain3->Update( gCam ) ;
	}
	if( Terrain2 )
	{
		Terrain2->UpdateAtlas( gCam ) ;
	}
#endif
}

void UpdateTerrain2Atlas()
{
#ifndef WO_SERVER
	if( Terrain2 )
	{
		Terrain2->UpdateAtlas( gCam ) ;
	}
#endif
}