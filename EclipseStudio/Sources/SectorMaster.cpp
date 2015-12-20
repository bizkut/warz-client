#include "r3dPCH.h"

#include "r3d.h"

#include "GameCommon.h"

#include "SectorMaster.h"

#define SECTOR_SETTINGS_FILE "/SectorSettings.xml"

Sector::Sector()
: Loaded( 0 )
{

}

//------------------------------------------------------------------------

SectorMaster::Settings::Settings()
: KeepAliveRadius( 512.0f )
, SectorSize( 64.0f )
, MapStartX( 0.0f )
, MapStartZ( 0.0f )
, MapSizeX( 0.0f )
, MapSizeZ( 0.0f )
{  

}

//------------------------------------------------------------------------

SectorMaster::SectorMaster()
{

}

//------------------------------------------------------------------------

SectorMaster::~SectorMaster()
{

}

//------------------------------------------------------------------------

void SectorMaster::Load( const char* path )
{
	r3dString fullPath = r3dString( path ) + SECTOR_SETTINGS_FILE;

	r3dFile* f = r3d_open( fullPath.c_str(), "rb" );
	if( !f )
	{
		return;
	}

	char* fileBuffer = game_new char[f->size + 1];
	fread(fileBuffer, f->size, 1, f);
	fileBuffer[f->size] = 0;

	pugi::xml_document xmlFile;
	pugi::xml_parse_result parseResult = xmlFile.load_buffer_inplace(fileBuffer, f->size);
	if(!parseResult)
		r3dError( "Failed to parse XML %s, error: %s", path, parseResult.description() );

	pugi::xml_node root = xmlFile.child( "root" );
	if( !root.empty() )
	{
		pugi::xml_node settings = root.child( "settings" );
		m_Settings.KeepAliveRadius = settings.attribute( "keep_alive_radius" ).as_float();
		m_Settings.SectorSize = settings.attribute( "sector_size" ).as_float();
	}

	delete [] fileBuffer;

	fclose( f );
}

//------------------------------------------------------------------------

void SectorMaster::Save( const char* path )
{
	pugi::xml_document xmlFile;
	xmlFile.append_child( pugi::node_comment ).set_value("SectorMaster Settings File");
	pugi::xml_node xmlRoot = xmlFile.append_child();
	xmlRoot.set_name( "root" );

	pugi::xml_node xmlSettings = xmlRoot.append_child();
	xmlSettings.set_name( "settings" );

	xmlSettings.append_attribute( "keep_alive_radius" )		= m_Settings.KeepAliveRadius;
	xmlSettings.append_attribute( "sector_size" )			= m_Settings.SectorSize;

	r3dString fullPath = r3dString( path ) + SECTOR_SETTINGS_FILE;

	xmlFile.save_file( fullPath.c_str() );
}

//------------------------------------------------------------------------

void SectorMaster::Reset()
{
	m_Sectors.Clear();
}

//------------------------------------------------------------------------

void SectorMaster::Init( float mapStartX, float mapStartZ, float mapSizeX, float mapSizeZ )
{
	m_Settings.MapStartX = mapStartX;
	m_Settings.MapStartZ = mapStartZ;
	m_Settings.MapSizeX = mapSizeX;
	m_Settings.MapSizeZ = mapSizeZ;

	m_Sectors.Clear();

	m_Sectors.Resize(	int( m_Settings.MapSizeX / m_Settings.SectorSize ) + 1, 
						int( m_Settings.MapSizeZ / m_Settings.SectorSize ) + 1 );

	r3dResetCachedMeshSectorReferences();
}

//------------------------------------------------------------------------

void SectorMaster::AddObject( GameObject* obj )
{
	// only static ( non-moving objects ) are supported.
	r3d_assert( obj->IsStatic() );
	r3d_assert( obj->ObjTypeFlags & OBJTYPE_Mesh || obj->ObjTypeFlags & OBJTYPE_Prefab );

	MeshGameObject* mgo = static_cast<MeshGameObject*>( obj );

	r3dPoint3D pos = obj->GetPosition();

	int sectorX = int( ( pos.x - m_Settings.MapStartX ) / m_Settings.SectorSize );
	int sectorZ = int( ( pos.z - m_Settings.MapStartZ ) / m_Settings.SectorSize );
	if( sectorX >= 0 && sectorX < (int)m_Sectors.Width()
			&&
		sectorZ >= 0 && sectorZ < (int)m_Sectors.Height()
		)
	{
		Sector& target = m_Sectors[ sectorZ ][ sectorX ];

		ExtractMeshes( &target.Meshes, obj );
	}
}

//------------------------------------------------------------------------

void SectorMaster::InitSectors( bool allLoaded )
{
#ifndef FINAL_BUILD

	int totalMeshes = 0;

	for( int si = 0, e = m_Sectors.Count(); si < e; si ++ )
	{
		Sector& s = m_Sectors.AtIndex( si );

		for( int i = 0, e = (int)s.Meshes.Count(); i < e; i ++ )
		{
			r3d_assert(  s.Meshes[ i ]->SectorRefCount == 0 );
		}

		totalMeshes += s.Meshes.Count();
	}

	r3dOutToLog( "SectorMaster::InitSectors: average mesh count per sector: %.2f\n", float( totalMeshes ) / m_Sectors.Count() );
#endif

	for( int si = 0, e = m_Sectors.Count(); si < e; si ++ )
	{
		Sector& s = m_Sectors.AtIndex( si );

		if( allLoaded )
		{
			for( int i = 0, e = (int)s.Meshes.Count(); i < e; i ++ )
			{
				r3dMesh* mesh = s.Meshes[ i ];

				mesh->SectorRefCount ++;
			}

			s.Loaded = 1;
		}
		else
		{
			s.Loaded = 0;
		}

	}
}

//------------------------------------------------------------------------

void SectorMaster::Update( const r3dCamera& cam )
{
	int loadedCount = 0;
	int unloadedCount = 0;

	float effectiveKeepAliveRadius = GetEffectiveKeepAliveRadius();

	for( int z = 0, e = m_Sectors.Height(); z < e; z ++ )
	{
		for( int x = 0, e = m_Sectors.Width(); x < e; x ++ )
		{
			Sector& s = m_Sectors[ z ][ x ];

			r3dPoint3D pos = GetSectorPosition( x, z );

			pos.y = cam.y;

			float distance = ( cam - pos ).Length();

			if( distance > effectiveKeepAliveRadius )
			{
				if( s.Loaded )
				{
					unloadedCount += UnloadSector( s );
				}
			}
			else
			{
				if( !s.Loaded )
				{
					loadedCount += LoadSector( s );
				}
			}
		}
	}

#ifndef FINAL_BUILD
	if( loadedCount || unloadedCount )
	{
		r3dOutToLog( "SectorMaster::Update: unloaded %d, loaded %d mesh materials\n", unloadedCount, loadedCount );
	}
#endif
}

//------------------------------------------------------------------------

int SectorMaster::UnloadSector( Sector& s )
{
	int count = 0;

	for( int i = 0, e = s.Meshes.Count(); i < e; i ++ )
	{
		r3dMesh* mesh = s.Meshes[ i ];

		r3d_assert( mesh->SectorRefCount > 0 );

		mesh->SectorRefCount--;

		if( !mesh->SectorRefCount )
		{
			mesh->ReleaseMaterials();
			count ++;
		}
	}

	s.Loaded = 0;
	return count;
}

//------------------------------------------------------------------------

int SectorMaster::LoadSector( Sector& s )
{
	int count = 0;

	for( int i = 0, e = s.Meshes.Count(); i < e; i ++ )
	{
		r3dMesh* mesh = s.Meshes[ i ];

		r3d_assert( mesh->SectorRefCount >= 0 );

		if( !mesh->SectorRefCount )
		{
			mesh->TouchMaterials();
			count ++;
		}

		mesh->SectorRefCount++;
	}

	s.Loaded = 1;

	return count;
}

//------------------------------------------------------------------------

void SectorMaster::LoadAllSectors()
{
	for( int z = 0, e = m_Sectors.Height(); z < e; z ++ )
	{
		for( int x = 0, e = m_Sectors.Width(); x < e; x ++ )
		{
			Sector& s = m_Sectors[ z ][ x ];

			if( !s.Loaded )
				LoadSector( s );
		}
	}
}

//------------------------------------------------------------------------

const SectorMaster::Settings&
SectorMaster::GetSettings() const
{
	return m_Settings;
}

//------------------------------------------------------------------------

void
SectorMaster::SetSettings( const Settings& sts )
{
	// changing this is not yet supported
	r3d_assert( sts.SectorSize == m_Settings.SectorSize );

	m_Settings = sts;
}

//------------------------------------------------------------------------

void SectorMaster::ExtractMeshes( Sector::MeshArr* targArr, GameObject* obj )
{
	// only static ( non-moving objects ) are supported.
	if( !obj->IsStatic() || !( obj->ObjTypeFlags & OBJTYPE_Mesh ) )
		return;

	MeshGameObject* mgo = static_cast<MeshGameObject*>( obj );

	r3dPoint3D pos = obj->GetPosition();

	for( int i = 0, e = MeshGameObject::NUM_LODS; i < e; i ++ )
	{
		if( r3dMesh* mesh = mgo->MeshLOD[ i ] )
		{
			targArr->PushBack( mesh );
		}

		if( r3dMesh* mesh = mgo->DestructionMeshLOD[ i ] )
		{
			targArr->PushBack( mesh );
		}
	}
}

//------------------------------------------------------------------------

float SectorMaster::GetEffectiveKeepAliveRadius() const
{
	return m_Settings.KeepAliveRadius * r_sector_keep_alive_coef->GetFloat();
}

//------------------------------------------------------------------------

SectorMaster* g_pSectorMaster;