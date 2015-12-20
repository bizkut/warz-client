#include "r3dPCH.h"
#include "r3d.h"
#include "r3dBudgeter.h"

#include "../sf/RenderBuffer.h"

#include "r3dDeviceQueue.h"

#include "GrassLib.h"
#include "GrassGen.h"

#pragma warning(disable:4244)

extern bool g_bEditMode;

GrassGen* g_pGrassGen = 0;
char g_LastGrassGenError[ 512 ];

namespace
{
	bool	FillGrassChunk( GrassChunk& oChunk, const GrassPatchGenSettings& patchSettings, int Ordinal );
}

void GrassGenSettingsToChunkSettings( GrassChunk& oChunk, const GrassChunkGenSettings& chunkSettings )
{
	oChunk.AlphaRef			= chunkSettings.AlphaRef;
	oChunk.TintStrength		= chunkSettings.TintStrength;
	oChunk.TintFade			= chunkSettings.TintFade;
	oChunk.TintFadeStrength	= chunkSettings.TintFadeStrength;
	oChunk.IsStiff			= chunkSettings.IsStiff;
}

struct GenData
{
	GrassChunk* chunk ;
	const GrassPatchGenSettings* settings ;
	int i ;

	int res ;
};

static void InMainThread( void* v )
{
	GenData* data = (GenData*) v ;

	data->res = FillGrassChunk( *data->chunk, *data->settings, data->i ) ;
}

bool GenerateGrass( GrassLibEntry& oEntry, const GrassPatchGenSettings& settings )
{
	oEntry.Name = settings.Name;	

	for( uint32_t i = 0, e = settings.ChunkSettings.Count(); i < e; i ++ )
	{
		GrassChunk chunk;

		const GrassChunkGenSettings& chunkSts = settings.ChunkSettings[ i ];

		GenData dt ;

		dt.chunk = &chunk ;
		dt.settings = & settings ;
		dt.i = i ;

		ProcessCustomDeviceQueueItem( InMainThread, &dt ) ;

		if( !dt.res )
			return false;		

		GrassGenSettingsToChunkSettings( chunk, chunkSts );

		oEntry.Chunks.PushBack( chunk );
	}

	return true;
}

//------------------------------------------------------------------------

namespace
{
	float saturate( float val )
	{
		return R3D_MAX( R3D_MIN( val, 1.0f ), 0.f );
	}

	float rnd( float start, float end )
	{
		return ( end - start ) * rand() / RAND_MAX +  start;
	}

	r3dPoint3D rnd( r3dPoint3D start, r3dPoint3D end )
	{
		return r3dPoint3D(
				rnd( start.x, end.x ),
				rnd( start.y, end.y ),
				rnd( start.z, end.z )
			);
	}

	void ReportGenError( const char* fmt, ... )
	{
		_vsnprintf( g_LastGrassGenError, sizeof g_LastGrassGenError, fmt, (va_list)(&fmt + 1) );
		r3dOutToLog( g_LastGrassGenError );
	}

	bool FillGrassChunk( GrassChunk& oChunk, const GrassPatchGenSettings& patchSettings, int Ordinal )
	{
		R3D_ENSURE_MAIN_THREAD();

		const GrassChunkGenSettings& settings = patchSettings.ChunkSettings[ Ordinal ];

		int finalDensity = R3D_MAX( int( patchSettings.Density * settings.Density ), 1 );

		if( finalDensity > GrassGen::MAX_MESHES_PER_BATCH )
		{
			ReportGenError( "GenerateGrass: grass chunk %d density is out of bounds!\n", Ordinal );
			return false;
		}

		r3dString grassTexPath = GetGrassPath( settings.TextureName.c_str() );

		oChunk.Texture = r3dRenderer->LoadTexture( grassTexPath.c_str() );
		if( !oChunk.Texture->IsDrawable() )
		{
			ReportGenError( "GenerateGrass: chunk %d texture %s is missing!\n", Ordinal, grassTexPath.c_str() );
			return false;
		}

		r3dMesh* mesh = gfx_new r3dMesh( 0 );

		struct DelMesh
		{
			r3dMesh* delme;
			~DelMesh()
			{
				delete delme;
			}
		} delMesh = { mesh }; (void)delMesh;

		r3dString grassMeshPath = GetGrassPath( settings.MeshName.c_str() );

		if( !mesh->Load( grassMeshPath.c_str(), true ) )
		{
			ReportGenError( "GenerateGrass: couldn't load chunk %d mesh( %s )!\n", Ordinal, grassMeshPath.c_str() );
			return false;
		}

		int numInstances = finalDensity;

		int numVertices = numInstances * mesh->NumVertices;

		oChunk.VerticesPerItem = mesh->NumVertices ;
		oChunk.IndicesPerItem = mesh->NumIndices ;

		if( numVertices > 65536 )
		{
			ReportGenError( "GenerateGrass: chunk %d need too many vertices ( %d, max: 65536 )!\n", Ordinal, numVertices );
			return false;
		}

		oChunk.IndexBuffer = gfx_new r3dIndexBuffer( numInstances * mesh->NumIndices, false, 2, GrassBufferMem );

		if( settings.NumVariations > GrassChunk::MAX_VARIATIONS )
		{
			ReportGenError( "GenerateGrass: chunk %d has too many variations ( %d, max: %d )!\n", Ordinal, settings.NumVariations, GrassChunk::MAX_VARIATIONS );
		}

		oChunk.NumVariations = settings.NumVariations;
		
		for( uint32_t i = 0, e = settings.NumVariations; i < e; i ++ )
		{
			bool readable = false;
#ifndef FINAL_BUILD
			if(g_bEditMode)
				readable = true;
#endif
			oChunk.VertexBuffs[ i ] = gfx_new r3dVertexBuffer( numVertices, sizeof( GrassVertex ), 0, false, readable );
		}

		//------------------------------------------------------------------------
		// Find out what the extents are

		r3dPoint3D	mi ( FLT_MAX, FLT_MAX, FLT_MAX ), 
					ma ( -FLT_MAX, -FLT_MAX, -FLT_MAX );

		for( int i = 0, e = mesh->NumVertices; i < e; i ++ )
		{
			mi.x = R3D_MIN( mesh->VertexPositions[ i ].x, mi.x );
			mi.y = R3D_MIN( mesh->VertexPositions[ i ].y, mi.y );
			mi.z = R3D_MIN( mesh->VertexPositions[ i ].z, mi.z );

			ma.x = R3D_MAX( mesh->VertexPositions[ i ].x, ma.x );
			ma.y = R3D_MAX( mesh->VertexPositions[ i ].y, ma.y );
			ma.z = R3D_MAX( mesh->VertexPositions[ i ].z, ma.z );
		}

		r3dPoint3D span		= ( ma - mi );
		r3dPoint3D centre	= ( ma + mi ) * 0.5f;

		centre.y = 0;

		float cellScale = g_pGrassLib->GetSettings().CellScale;
		float maxMeshScale = g_pGrassLib->GetSettings().MaxMeshScale;

		float scale = settings.MeshScale;

		float ssx = span.x * scale;
		float ssz = span.z * scale;

		float xzspan = sqrtf( ssx * ssx + ssz * ssz );

		if( xzspan			> maxMeshScale || 
			span.y * scale	> maxMeshScale )
		{
			ReportGenError(	"GenerateGrass: chunk %d mesh is too big ( [%f,%f,%f], max: [%f,%f,%f] )!", Ordinal, 
							span.x * scale, span.y * scale, span.z * scale, 
							maxMeshScale, maxMeshScale, maxMeshScale );

			return false;
		}

		r3dPoint3D halfChunkScale = GetGrassChunkScale() * 0.5f;

		//------------------------------------------------------------------------
		// Fill indices

		oChunk.SysmemIndices.Resize( oChunk.IndexBuffer->GetItemCount() ) ;

		unsigned short *indices = &oChunk.SysmemIndices[ 0 ] ;
		unsigned short *indices_start = indices ;

		for( int inst = 0, e = numInstances; inst < e; inst ++ )
		{
			int instDsp = mesh->NumVertices * inst;

			for ( int i = 0, e = mesh->NumIndices/3; i < e; i ++ )
			{
				for ( int k = 0; k < 3; k ++ )
				{
					uint32_t vertIdx = mesh->Indices[i*3+k];

					*indices++ = (unsigned short)( instDsp + vertIdx );
				}
			}
		}

		r3d_assert( indices - indices_start == oChunk.SysmemIndices.Count() ) ;

		unsigned short *indices_vmem = (unsigned short *)oChunk.IndexBuffer->Lock() ;

		memcpy( indices_vmem, indices_start, oChunk.IndexBuffer->GetItemCount() * sizeof( UINT16 ) ) ;

		oChunk.IndexBuffer->Unlock();

		//------------------------------------------------------------------------
		// Fill vertices

		for( int i = 0, e = settings.NumVariations; i < e; i ++ )
		{
			GrassChunk::Vertices& systemVerts = oChunk.SysmemVertices[ i ] ;
			systemVerts.Resize( numVertices ) ;

			GrassVertex* vertices = &systemVerts[ 0 ] ;
			GrassVertex* vertices_start = vertices ;

			for( int inst = 0, e = numInstances; inst < e; inst ++ )
			{
				int instDsp = mesh->NumVertices * inst;

				r3dPoint3D randRange1 = r3dPoint3D( cellScale, span.y * 0.0625f, cellScale ) * 0.5f;
				r3dPoint3D randRange0 = -randRange1;

				// above the ground a little to prevent holes in some plane configurations
				randRange0.y = span.y * 0.015625f;

				r3dPoint3D instPos = rnd( randRange0, randRange1 );

				float scaling	= rnd( settings.MinMeshScaling, settings.MaxMeshScaling ) * scale;

				float rotation	= rnd( 0.f, 2* settings.RotationVariation * R3D_PI );

				float sinr = sinf( rotation );
				float cosr = cosf( rotation );

				for ( int i = 0, e = mesh->NumIndices/3; i < e; i ++ )
				{
					for ( int k = 0; k < 3; k ++ )
					{
						uint32_t vertIdx = mesh->Indices[i*3+k];

						R3D_WORLD_VERTEX vert;
						mesh->FillSingleVertexBase<R3D_WORLD_VERTEX>( &vert, vertIdx );

						r3dPoint3D pos = ( vert.Pos - centre ) * scaling;

						r3dPoint3D rpos;

						rpos.x = pos.x * cosr - pos.z * sinr;
						rpos.y = pos.y;
						rpos.z = pos.x * sinr + pos.z * cosr;

						r3dPoint3D rnorm;

						rnorm.x = vert.Normal.x * cosr - vert.Normal.z * sinr;
						rnorm.y = vert.Normal.y;
						rnorm.z = vert.Normal.x * sinr + vert.Normal.z * cosr;

						short3 pos3 = r3dNormShort3( rpos + instPos, halfChunkScale );

						r3d_assert( instDsp + vertIdx < systemVerts.Count() ) ;
						
						vertices[ instDsp + vertIdx ].pos.x	= pos3.x;
						vertices[ instDsp + vertIdx ].pos.y	= pos3.y;
						vertices[ instDsp + vertIdx ].pos.z	= pos3.z;
						vertices[ instDsp + vertIdx ].pos.w = 32767;

						vertices[ instDsp + vertIdx ].normal.x = R3D_MIN( R3D_MAX( int( 255 * ( rnorm.x * 0.5f + 0.5f ) ), 0 ), 255 );
						vertices[ instDsp + vertIdx ].normal.y = R3D_MIN( R3D_MAX( int( 255 * ( rnorm.y * 0.5f + 0.5f ) ), 0 ), 255 );
						vertices[ instDsp + vertIdx ].normal.z = R3D_MIN( R3D_MAX( int( 255 * ( rnorm.z * 0.5f + 0.5f ) ), 0 ), 255 );
						vertices[ instDsp + vertIdx ].normal.w = 0;

						vertices[ instDsp + vertIdx ].texc.x	= r3dNormShort( saturate( vert.tu ), 1.0f );
						vertices[ instDsp + vertIdx ].texc.y	= r3dNormShort( saturate( vert.tv ), 1.0f );
					}
				}
			}

			GrassVertex* vertices_vmem = (GrassVertex*)oChunk.VertexBuffs[ i ]->Lock( 0, numVertices );
			memcpy( vertices_vmem, vertices_start, numVertices * sizeof(GrassVertex) ) ;
			oChunk.VertexBuffs[ i ]->Unlock();
		}

		return true;
	}

}

//------------------------------------------------------------------------

GrassChunkGenSettings::GrassChunkGenSettings()
: NumVariations( 1 )
, Density( 1024 )
, MeshScale( 1.f )
, MinMeshScaling( 1.f )
, MaxMeshScaling( 1.f )
, RotationVariation( 0.f )
, AlphaRef( 0.25f )
, TintStrength( 1.f )
, TintFade( 0.f )
, TintFadeStrength( 1.f )
, IsStiff( 0 )
{

}

//------------------------------------------------------------------------

GrassPatchGenSettings::GrassPatchGenSettings()
: Density( 1.f )
, FadeDistance( 1.f )
{

}

//------------------------------------------------------------------------

GrassGen::GrassGen()
: mDirty( 0 )
{

}

//------------------------------------------------------------------------

GrassGen::~GrassGen()
{

}

//------------------------------------------------------------------------

bool
GrassGen::IsNewerThan( INT64 stamp )
{
	WIN32_FIND_DATA ffblk;
	HANDLE h;

	r3dString searchPath = GetGrassPath( "*.xml" );	

	h = FindFirstFile( searchPath.c_str() , &ffblk );
	if( h != INVALID_HANDLE_VALUE) 
	{
		do 
		{
			if ( !( ffblk.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ) 
			{
				if( r3d_fstamp( GetGrassPath( ffblk.cFileName ).c_str() ) > stamp )
				{
					FindClose(h);
					return true ;
				}
			}
		} while(FindNextFile(h, &ffblk) != 0);
		FindClose(h);
	}

	return false;

}

//------------------------------------------------------------------------

bool
GrassGen::Load()
{
	stringlist_t loadList ;

#define R3D_GLIST_FILE "grasslist.dat"

#ifndef FINAL_BUILD
	WIN32_FIND_DATA ffblk;
	HANDLE h;

	r3dString searchPath = GetGrassPath( "*.xml" );	

	h = FindFirstFile( searchPath.c_str() , &ffblk );
	if( h != INVALID_HANDLE_VALUE) 
	{
		do 
		{
			if ( !( ffblk.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ) 
			{
				if( stricmp( ffblk.cFileName, "grassmeshes.xml" ) )
				{
					loadList.push_back( ffblk.cFileName ) ;
				}
			}
		} while(FindNextFile(h, &ffblk) != 0);
		FindClose(h);
	}

	FILE *fout = fopen( GetGrassPath( R3D_GLIST_FILE ).c_str(), "wt" ) ;
	r3d_assert( fout ) ;

	fprintf( fout, "%d\n", (int)loadList.size() ) ;

	for( int i = 0, e = (int)loadList.size() ; i < e ; i ++ )
	{
		fprintf( fout, "%s\n", loadList[ i ].c_str() ) ;
	}
	
	fclose( fout ) ;

	loadList.clear() ;
#endif

	r3dFile* glistFile = r3d_open( GetGrassPath( R3D_GLIST_FILE ).c_str(), "rt" ) ;

	if( glistFile )
	{
		int glist_n = 0 ;
		int loaded = 0 ;

		char buf[ 1024 ];
		fgets( buf, sizeof buf - 1, glistFile );

		if( sscanf( buf, "%d\n", &glist_n ) == 1 )
		{
			for( int i = 0, e = glist_n ; i < e ; i ++ )
			{
				buf[ 0 ] = 0;
				fgets( buf, sizeof buf - 1, glistFile );

				char loc[ 512 ];

				if( sscanf( buf, "%511s", loc ) == 1 )
				{
					loadList.push_back( loc ) ;
					loaded ++ ;
				}
			}
		}

		fclose( glistFile ) ;

		r3dOutToLog( "Loaded %d of %d grass settings locations from the glist file\n", loaded, glist_n ) ;
	}
	else
	{
		r3dOutToLog( "WARNING: couldn't open glist file '%s'\n", GetGrassPath( R3D_GLIST_FILE ).c_str() ) ;
	}

	int actuallyLoaded = 0 ;

	for( int i = 0, e = loadList.size() ; i < e ; i ++ )
	{
		if( AddSettingsFromFile( loadList[ i ].c_str() ) )
		{
			actuallyLoaded ++ ;
		}
	}

	r3dOutToLog( "Loaded %d grass settings files\n", actuallyLoaded ) ;

	return true;
}

//------------------------------------------------------------------------

void
GrassGen::Save()
{
	for( uint32_t i = 0, e = mPatchSettings.Count(); i < e; i ++ )
	{
		SaveSettingsToFile( mPatchSettings[ i ] );
	}

	r3dOutToLog( "Generating grass.. \n" );

	float timeStart = r3dGetTime();

	GenerateAll();
	g_pGrassLib->Save();

	mDirty = 0 ;

	r3dOutToLog( "Generated grass for %f\n", r3dGetTime() - timeStart );
}

void
GrassGen::Save( const r3dString& entryName )
{
	for( uint32_t i = 0, e = mPatchSettings.Count(); i < e; i ++ )
	{
		if( mPatchSettings[ i ].Name == entryName )
		{
			SaveSettingsToFile( mPatchSettings[ i ] );
			if( !Generate( i ) )
			{
				r3dArtBug( "GrassGen::Save: failed to generate grass mesh from %s", mPatchSettings[ i ].Name.c_str() );
			}

		}
	}
}

//------------------------------------------------------------------------

bool
GrassGen::GenerateAll()
{
	for( uint32_t i = 0, e = mPatchSettings.Count(); i < e; i ++ )
	{
		if( !Generate( i ) )
		{
			r3dArtBug( "GrassGen::GenerateAll: failed to generate grass mesh from %s", mPatchSettings[ i ].Name.c_str() );
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------

bool
GrassGen::Generate( uint32_t idx )
{
	GrassLibEntry gle;

	strcpy( g_LastGrassGenError, "Unknown Error" );

	if( !GenerateGrass( gle, mPatchSettings[ idx ] ) )
	{
		const r3dString& fileToDelete = GetGrassLibFile();
		remove( fileToDelete.c_str() );
		return false;
	}

	g_pGrassLib->UpdateEntry( gle );

	mDirty = 1 ;

	return true;
}

//------------------------------------------------------------------------

void
GrassGen::FillTypes( string_vec& oVec )
{
	oVec.clear();

	for( uint32_t i = 0, e = mPatchSettings.Count(); i < e; i ++ )
	{
		oVec.push_back( r3dSTLString( mPatchSettings[ i ].Name.c_str() ) );
	}
}

//------------------------------------------------------------------------

uint32_t
GrassGen::GetTypeCount() const
{
	return mPatchSettings.Count();
}

//------------------------------------------------------------------------

const GrassPatchGenSettings&
GrassGen::GetPatchSettings( uint32_t idx ) const
{
	return mPatchSettings[ idx ];
}

//------------------------------------------------------------------------

const GrassPatchGenSettings*
GrassGen::GetPatchSettings( const r3dString& name ) const
{
	for( int i = 0, e = mPatchSettings.Count() ; i < e ; i ++ )
	{
		if( mPatchSettings[ i ].Name == name )
			return &mPatchSettings[ i ] ;
	}

	return NULL ;
}

//------------------------------------------------------------------------

void
GrassGen::SetPatchSettings( uint32_t idx, const GrassPatchGenSettings& sts )
{
	mPatchSettings[ idx ] = sts;
}

//------------------------------------------------------------------------

bool
GrassGen::AddSettingsFromFile( const r3dString& fname )
{
	pugi::xml_attribute attrib;

#define PUGI_GET_IF_SET(var,node,name,type) attrib = node.attribute(name); if( !attrib.empty() ) { var = attrib.as_##type(); }

	r3dFile* f = r3d_open( GetGrassPath( fname.c_str() ).c_str(), "rb" );
	if ( ! f )
	{
		return false;
	}

	r3dTL::TArray< char > fileBuffer( f->size + 1 );

	fread( &fileBuffer[ 0 ], f->size, 1, f );
	fileBuffer[ f->size ] = 0;

	pugi::xml_document xmlLevelFile;
	pugi::xml_parse_result parseResult = xmlLevelFile.load_buffer_inplace( &fileBuffer[0], f->size );
	fclose( f );

	if( !parseResult )
	{
		r3dError( "GenerateGrass: Failed to parse %s, error: %s", fname.c_str(), parseResult.description() );
		return false;
	}

	pugi::xml_node xmlGrass = xmlLevelFile.child( "grass" );
	pugi::xml_node xmlChunk = xmlGrass.child( "chunk" );

//	int ordinal = 0;

	GrassPatchGenSettings patchSettings;

	patchSettings.Name = fname;

	PUGI_GET_IF_SET( patchSettings.Density,			xmlGrass, "density",		float	);
	PUGI_GET_IF_SET( patchSettings.FadeDistance,	xmlGrass, "fade_distance",	float	);	

	while( !xmlChunk.empty() )
	{
		GrassChunkGenSettings settings;

		settings.MeshName			= xmlChunk.attribute( "mesh" ).value();
		settings.TextureName		= xmlChunk.attribute( "texture" ).value();

		PUGI_GET_IF_SET( settings.NumVariations,		xmlChunk,	"num_vars",				int		);
		PUGI_GET_IF_SET( settings.Density,				xmlChunk,	"density",				int		);
		PUGI_GET_IF_SET( settings.MeshScale,			xmlChunk,	"mesh_scale",			float	);
		PUGI_GET_IF_SET( settings.MinMeshScaling,		xmlChunk,	"min_mesh_scaling",		float	);
		PUGI_GET_IF_SET( settings.MaxMeshScaling,		xmlChunk,	"max_mesh_scaling",		float	);
		PUGI_GET_IF_SET( settings.RotationVariation,	xmlChunk,	"rot_var",				float	);
		PUGI_GET_IF_SET( settings.AlphaRef,				xmlChunk,	"alpha_ref",			int		);
		PUGI_GET_IF_SET( settings.TintStrength,			xmlChunk,	"tint_strength",		float	);
		PUGI_GET_IF_SET( settings.TintFade,				xmlChunk,	"tint_fade",			float	);
		PUGI_GET_IF_SET( settings.TintFadeStrength,		xmlChunk,	"tint_fade_strength",	float	);
		PUGI_GET_IF_SET( settings.IsStiff,				xmlChunk,	"is_stiff",				int		);
		

		settings.AlphaRef /= 255.f;

		patchSettings.ChunkSettings.PushBack( settings );

		xmlChunk = xmlChunk.next_sibling();
	}

	mPatchSettings.PushBack( patchSettings );

	return true;

#undef PUGI_GET_IF_SET
}

//------------------------------------------------------------------------

int
GrassGen::IsDirty() const
{
	return mDirty ;
}

//------------------------------------------------------------------------

bool
GrassGen::SaveSettingsToFile( const GrassPatchGenSettings& settings ) const
{
	pugi::xml_document xmlGrassFile;
	xmlGrassFile.append_child(pugi::node_comment).set_value("Grass File");
	pugi::xml_node xmlGrass = xmlGrassFile.append_child();
	xmlGrass.set_name("grass");

	xmlGrass.append_attribute( "density" )			= settings.Density;
	xmlGrass.append_attribute( "fade_distance" )	= settings.FadeDistance;

	for( uint32_t i = 0, e = settings.ChunkSettings.Count(); i < e; i ++ )
	{
		const GrassChunkGenSettings& csts =  settings.ChunkSettings[ i ];

		pugi::xml_node xmlChunk = xmlGrass.append_child();

		xmlChunk.set_name( "chunk" );

		xmlChunk.append_attribute( "num_vars" )				= csts.NumVariations;
		xmlChunk.append_attribute( "mesh" )					= csts.MeshName.c_str();
		xmlChunk.append_attribute( "texture" )				= csts.TextureName.c_str();
		xmlChunk.append_attribute( "density" )				= csts.Density;
		xmlChunk.append_attribute( "mesh_scale")			= csts.MeshScale;
		xmlChunk.append_attribute( "min_mesh_scaling")		= csts.MinMeshScaling;
		xmlChunk.append_attribute( "max_mesh_scaling")		= csts.MaxMeshScaling;
		xmlChunk.append_attribute( "rot_var" )				= csts.RotationVariation;
		xmlChunk.append_attribute( "alpha_ref" )			= int( csts.AlphaRef * 255.f );
		xmlChunk.append_attribute( "tint_strength" )		= csts.TintStrength;
		xmlChunk.append_attribute( "tint_fade" )			= csts.TintFade;
		xmlChunk.append_attribute( "tint_fade_strength" )	= csts.TintFadeStrength;
		xmlChunk.append_attribute( "is_stiff" )				= csts.IsStiff;

	}

	xmlGrassFile.save_file( GetGrassPath( settings.Name.c_str() ) .c_str() );

	return true;
}
