#include "r3dPCH.h"
#include "r3d.h"

#include "GameObjects/ObjManag.h"

#include "GameObjects/GameObj.h"
#include "ObjectsCode/world/obj_Road.h"
#include "ObjectsCode/world/MaterialTypes.h"
#include "ObjectsCode/world/DecalChief.h"

#include "GameObjects/PhysXWorld.h"
#include "GameObjects/PhysObj.h"

#include "TrueNature/Sun.h"

#include "GameLevel.h"

#include "r3dBudgeter.h"

#include "r3dDeviceQueue.h"

#include "Terrain2.h"

#include "..\gameobjects\VehicleManager.h"

#define FNAME_TERRAIN2_INI			"%s/Terrain2/terrain2.ini"
#define FNAME_TERRAIN2_BIN			"%s/Terrain2/terrain2.bin"

#define TERRAIN2_SIGNATURE			'2RET'
#define TERRAIN2_VERSION_101		101
#define TERRAIN2_VERSION			103

#define GENERATE_MIPS 0

const char* DigitToCString( int digit );

enum
{
	NORTH_CONNECTION	= 1,
	EAST_CONNECTION		= 2,
	SOUTH_CONNECTION	= 4,
	WEST_CONNECTION		= 8,
};

enum
{
	TERRA_CONNECTION_TYPE_COUNT = 16
};

#pragma pack(push)
#pragma pack(1)

struct r3dDynaTerraVertex
{
	short height;
	short unused;
};

#pragma pack(pop)

static int CountConnectionIndices( int vertexTileDim, int sideLodConnections );
static int ConstructConnectionIndices( UINT16* target, int vertexTileDim, int sideLodConnections );
static void PrintSplatLocation( char (& buf ) [ 1024 ], int idx );
static uint32_t GetHeightFieldDataSize( const PxHeightFieldDesc& desc );
static void PrintFullSplatPath( const char* levelDir, int i, char (& buf ) [ 1024 ] );
static void PrintFullColorPath( const char* levelDir, char (& buf ) [ 1024 ] );
static void PrintFullNormalPath( const char* levelDir, char (& buf ) [ 1024 ] );
static void FillDecalRect( RECT* oRect, const r3dTerrain2::DecalRecord& drec );

static void TerraTextureReloadListener( r3dTexture* tex );

static IDirect3DVertexDeclaration9* g_TerraVDecl;
static IDirect3DVertexDeclaration9* g_TerraDynaVDecl;

static float gSaveCountDown;
static float gSaveCountDownLastTime;

static const D3DFORMAT TERRA2_LAYERMASK_FORMAT = D3DFMT_DXT1;
static const D3DFORMAT TERRA2_COLORMODULATION_FORMAT = D3DFMT_DXT1;

extern bool g_bEditMode;

static int GetTerraTexDownScale()
{
	int DownScale = 1;

	switch( r_texture_quality->GetInt() )
	{
	case 1:
		return 2;
		break;
	default:
		return 1;
		break;
	}

	return DownScale;
}


static int MipCount( int dim )
{
	int c = 0;
	for(; dim; )
	{
		dim /= 2;
		c ++;
	}

	return c;
}

//------------------------------------------------------------------------

static R3D_FORCEINLINE bool tile_comp_func_ATLAS_ID( const r3dTerrain2::AllocatedTile* tile0, const r3dTerrain2::AllocatedTile* tile1 )
{
	return tile0->AtlasVolumeID < tile1->AtlasVolumeID;
}

//------------------------------------------------------------------------

struct GenerateAtlasPixelShaderId
{
	enum
	{
		MAX_LAYERS = 3
	};

	union
	{
		struct
		{
			unsigned numLayers : 2;
			unsigned firstBatch : 1;
			unsigned modulationBatch : 1;
		};

		int Id;
	};

	GenerateAtlasPixelShaderId();

	void FillMacros( ShaderMacros& defines );
	void ToString( char* oStr );
};

r3dTL::TFixedArray< int, 16 > g_AtlasPixelShaderIdMap;

//------------------------------------------------------------------------

GenerateAtlasPixelShaderId::GenerateAtlasPixelShaderId()
: Id( 0 )
{

}

//------------------------------------------------------------------------

void
GenerateAtlasPixelShaderId::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 3 );

	defines[ 0 ].Name		= "NUM_LAYERS";
	defines[ 0 ].Definition	= DigitToCString( numLayers );

	defines[ 1 ].Name		= "FIRST_BATCH";
	defines[ 1 ].Definition	= DigitToCString( firstBatch );

	defines[ 2 ].Name		= "MODULATION_BATCH";
	defines[ 2 ].Definition	= DigitToCString( modulationBatch );
}

//------------------------------------------------------------------------

void
GenerateAtlasPixelShaderId::ToString( char* oStr )
{
	strcpy( oStr, "PS_TERRAIN_GEN_ATLAS" );
	strcat( oStr, "_" );
	strcat( oStr, DigitToCString( numLayers ));

	if( firstBatch )
	{
		strcat( oStr, "_FIRST" );
	}

	if( modulationBatch )
	{
		strcat( oStr, "_MODULATION" );
	}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------

struct GenerateAtlasVertexShaderId
{

	union
	{
		struct
		{
			unsigned unused : 1;
		};

		int Id;
	};

	GenerateAtlasVertexShaderId();

	void FillMacros( ShaderMacros& defines );
	void ToString( char* oStr );
};

r3dTL::TFixedArray< int, 1 > g_AtlasVertexShaderIdMap;

//------------------------------------------------------------------------

GenerateAtlasVertexShaderId::GenerateAtlasVertexShaderId()
: Id( 0 )
{

}

//------------------------------------------------------------------------

void
GenerateAtlasVertexShaderId::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 1 );

	defines[ 0 ].Name		= "UNUSED_DEFINE";
	defines[ 0 ].Definition	= DigitToCString( unused );
}

//------------------------------------------------------------------------

void
GenerateAtlasVertexShaderId::ToString( char* oStr )
{
	strcpy( oStr, "VS_TERRAIN_GEN_ATLAS" );

	if( unused )
	{
		strcat( oStr, "_UNUSED" );
	}
}

//------------------------------------------------------------------------

struct Terrain2PixelShaderId
{

	union
	{
		struct
		{
			unsigned aux : 1;
			unsigned forward_lighting : 1;
		};

		int Id;
	};

	Terrain2PixelShaderId();

	void FillMacros( ShaderMacros& defines );
	void ToString( char* oStr );
};

r3dTL::TFixedArray< int, 4 > g_Terrain2PixelShaderIdMap;

struct Terrain2VertexShaderId
{
	union
	{
		struct
		{
			unsigned shadowPath : 1;
			unsigned depthPath : 1;
			unsigned vfetchless : 1;
			unsigned recticular_warp : 1;
		};

		int Id;
	};

	Terrain2VertexShaderId();

	void FillMacros( ShaderMacros& defines );
	void ToString( char* oStr );
};

r3dTL::TFixedArray< int, 16 > g_Terrain2VertexShaderIdMap;

//------------------------------------------------------------------------

Terrain2VertexShaderId::Terrain2VertexShaderId()
: Id( 0 )
{

}

//------------------------------------------------------------------------

void
Terrain2VertexShaderId::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 4 );

	defines[ 0 ].Name		= "SHADOW_PATH";
	defines[ 0 ].Definition	= DigitToCString( shadowPath );

	defines[ 1 ].Name		= "DEPTH_PATH";
	defines[ 1 ].Definition	= DigitToCString( depthPath );

	defines[ 2 ].Name		= "VERTEX_FETCHLESS";
	defines[ 2 ].Definition	= DigitToCString( vfetchless );

	defines[ 3 ].Name		= "RECTICULAR_WARP";
	defines[ 3 ].Definition	= DigitToCString( recticular_warp );

}

//------------------------------------------------------------------------

void
Terrain2VertexShaderId::ToString( char* oStr )
{
	strcpy( oStr, "VS_TERRAIN2" );

	if( shadowPath )
	{
		strcat( oStr, "_SHADOWS" );
	}

	if( depthPath )
	{
		strcat( oStr, "_DEPTH" );
	}

	if( vfetchless )
	{
		strcat( oStr, "_VFETCHLESS" );
	}

	if( recticular_warp )
	{
		strcat( oStr, "_RECTICULARWARP" );
	}
}


int g_RoadAtlasPixelShaderId;

//------------------------------------------------------------------------

Terrain2PixelShaderId::Terrain2PixelShaderId()
: Id( 0 )
{

}

//------------------------------------------------------------------------

void
Terrain2PixelShaderId::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 2 );

	defines[ 0 ].Name		= "AUX_ENABLED";
	defines[ 0 ].Definition	= DigitToCString( aux );

	defines[ 1 ].Name		= "FORWARD_LIGHTING";
	defines[ 1 ].Definition	= DigitToCString( forward_lighting );
}

//------------------------------------------------------------------------

void
Terrain2PixelShaderId::ToString( char* oStr )
{
	strcpy( oStr, "PS_TERRAIN2" );

	if( aux )
	{
		strcat( oStr, "_AUX" );
	}

	if( forward_lighting )
	{
		strcat( oStr, "_FORW" );
	}
}

//------------------------------------------------------------------------

r3dTerrainLayer::r3dTerrainLayer()
: Name( "Nameless Layer" )
, ScaleU( 0.f )
, ScaleV( 0.f )

, SpecularPow( 8.f )

, DiffuseTex( NULL )
, NormalTex( NULL )

, MaskTex( NULL )
, ChannelMask( 1.0f, 0.f, 0.f, 0.f )
, ChannelIdx( 0 )

, MaterialTypeName( "" )
, ShaderScaleU( 0.f )
, ShaderScaleV( 0.f )

{

}

//------------------------------------------------------------------------

r3dTerrainStats::r3dTerrainStats()
{
	memset( this, 0, sizeof( *this ) );
}

//------------------------------------------------------------------------

r3dTerrain2::DecalRecord::DecalRecord()
: TypeIdx ( -1 )
, X ( 0 )
, Z ( 0 )
, Scale ( 1 )
, ZRot ( 0 )
{

}

//------------------------------------------------------------------------

r3dTerrain2::CreationParams::CreationParams()
: CellCountX( 0 )
, CellCountZ( 0 )

, SplatSizeX( 0 )
, SplatSizeZ( 0 )

, CellSize( 1.f )
, Height( 100.f )
{

}

//------------------------------------------------------------------------

r3dTerrain2::QualitySettings::QualitySettings()
: AtlasTileDim( 256 )
, VertexTileDim( 2 )
, RoadVertexDimStart( 4 )
, BakeRoads( 1 )
, VertexDensity( 32 )
{
	TileCounts[ 0 ] = 4;
	TileCounts[ 1 ] = 4;

	for( int i = 2, e = TileCounts.COUNT; i < e; i ++ )
	{
		TileCounts[ i ] = 2;
	}

	for( int i = 0, e = TileVertexDensitySteps.COUNT; i < e; i ++ )
	{
		TileVertexDensitySteps[ i ] = 32;
	}

	for( int i = 0, e = TileDistances.COUNT; i < e; i ++ )
	{
		TileDistances[ i ] = 0.f;
	}

	Sync();
}

//------------------------------------------------------------------------

int
r3dTerrain2::QualitySettings::Compare( const QualitySettings& sts )
{
	// NOTE : we skip TileDistances because they are derived from other settings

	int res = 0;

	res |= memcmp( &TileCounts[ 0 ], &sts.TileCounts[ 0 ], sizeof TileCounts[ 0 ] * TileCounts.COUNT );
	res |= memcmp( &TileVertexDensities[ 0 ], &sts.TileVertexDensities[ 0 ], sizeof TileVertexDensities[ 0 ] * TileVertexDensities.COUNT );
	res |= memcmp( &TileVertexDensitySteps[ 0 ], &sts.TileVertexDensitySteps[ 0 ], sizeof TileVertexDensitySteps[ 0 ] * TileVertexDensitySteps.COUNT );

	// skip QualitySettings::TileDistances;

	res |= VertexDensity != sts.VertexDensity;

	res |= AtlasTileDim != sts.AtlasTileDim;
	res |= VertexTileDim != sts.VertexTileDim;
	res |= RoadVertexDimStart != sts.RoadVertexDimStart;

	res |= BakeRoads != sts.BakeRoads;

	return res;
}

//------------------------------------------------------------------------

void
r3dTerrain2::QualitySettings::Sync()
{
	r3d_assert( VertexTileDim <= 8 );
	r3d_assert( VertexDensity <= 128 );

	VertexTileDim = R3D_MIN( VertexTileDim, 8 );
	VertexDensity = R3D_MIN( VertexDensity, 128 );

	for( int i = 0, e = TileCounts.COUNT; i < e; i ++ )
	{
		if( VertexTileDim * ( 1 << i ) <= VertexDensity )
		{
			TileVertexDensities[ i ] = i;
		}
		else
		{
			if( i )
				TileVertexDensities[ i ] = TileVertexDensities[ i - 1 ];
			else
				TileVertexDensities[ i ] = 0;
		}
	}

	if( RoadVertexDimStart < VertexTileDim )
	{
		RoadVertexDimStart = VertexTileDim;
	}

}

//------------------------------------------------------------------------

r3dTerrain2::Settings::Settings()
: Specular( 0.f )
, DetailNormalMix( 0.5f )
, FarFadeStart( FLT_MAX )
, FarFadeEnd( FLT_MAX )
{

}

//------------------------------------------------------------------------

r3dTerrain2::RoadInfo::RoadInfo()
{
	memset( ptrs, 0, sizeof ptrs );
}

//------------------------------------------------------------------------

void
r3dTerrain2::AllocatedTile::Init()
{
	X = 0;
	Z = 0;

	AtlasTileID = -1;
	AtlasVolumeID = -1;

	Tagged = 0;
	L = 0;

	ConFlags = 0;
}

//------------------------------------------------------------------------

int g_AtlasMipPSId = -1;
int g_AtlasMipVSId = -1;

//------------------------------------------------------------------------

int g_Terrain2ShadowPSId = -1;

//------------------------------------------------------------------------
/*static*/

const D3DFORMAT r3dTerrain2::ATLAS_FMT = D3DFMT_R5G6B5;

//------------------------------------------------------------------------

r3dTerrain2::r3dTerrain2()
{
	// NOTE : move all initialization code to Cosntruct()
	Construct();
}

//------------------------------------------------------------------------

r3dTerrain2::~r3dTerrain2()
{
	Destroy();
}

//------------------------------------------------------------------------
/*static*/

void
r3dTerrain2::LoadShaders()
{
#if R3D_TERRAIN_V2_GRAPHICS

	//------------------------------------------------------------------------
	// Atlas vertex shaders
	{
		GenerateAtlasVertexShaderId vsid;

		ShaderMacros macros;

		vsid.unused = 0;

		char name[ 512 ];

		vsid.ToString( name );
		vsid.FillMacros( macros );

		g_AtlasVertexShaderIdMap[ vsid.Id ] = r3dRenderer->AddVertexShaderFromFile( name, "Nature\\TerrainAtlas_vs.hls", 0, macros );

	}

	//------------------------------------------------------------------------
	// atlas pixel shaders
	{
		GenerateAtlasPixelShaderId psid;

		ShaderMacros macros;

		for( int i = 0, e = g_AtlasPixelShaderIdMap.COUNT; i < e; i ++ )
		{
			g_AtlasPixelShaderIdMap[ i ] = -1;
		}

		for( int i = 0, e = GenerateAtlasPixelShaderId::MAX_LAYERS; i <= e; i ++ )
		{
			psid.numLayers = i;

			for( int fi = 0, e = 2; fi < e; fi ++ )
			{
				psid.firstBatch = fi;

				char name[ 512 ];

				psid.modulationBatch = 0;

				psid.ToString( name );
				psid.FillMacros( macros );

				g_AtlasPixelShaderIdMap[ psid.Id ] = r3dRenderer->AddPixelShaderFromFile( name, "Nature\\TerrainAtlas_ps.hls", 0, macros );
			}
		}

		// modulation batch
		{
			psid.Id = 0;
			psid.modulationBatch = 1;

			char name[ 512 ];

			psid.ToString( name );
			psid.FillMacros( macros );

			g_AtlasPixelShaderIdMap[ psid.Id ] = r3dRenderer->AddPixelShaderFromFile( name, "Nature\\TerrainAtlas_ps.hls", 0, macros );
		}
	}

	//------------------------------------------------------------------------
	// Main Terrain Vertex Shader
	{
		for( int s = 0; s < 2; s ++ )
		{
			Terrain2VertexShaderId vsid;

			vsid.shadowPath = s;

			for( int d = 0; d < 2; d ++ )
			{
				vsid.depthPath = d;

				for( int vfl = 0, e = 2; vfl < e; vfl ++ )
				{
					vsid.vfetchless = vfl;

					for( int recticular_warp = 0; recticular_warp < 2; recticular_warp ++ )
					{
						vsid.recticular_warp = recticular_warp;

						if( !r3dRenderer->SupportsVertexTextureFetch && !vfl )
						{
							g_Terrain2VertexShaderIdMap[ vsid.Id ] = -1;
							continue;
						}

						ShaderMacros macros;
						char name[ 512 ];

						vsid.ToString( name );
						vsid.FillMacros( macros );

						if( ( vsid.depthPath && vsid.shadowPath )
								||
							( vsid.recticular_warp && !vsid.shadowPath )
								)
						{
							g_Terrain2VertexShaderIdMap[ vsid.Id ] = -1;
						}
						else
						{
							g_Terrain2VertexShaderIdMap[ vsid.Id ] = r3dRenderer->AddVertexShaderFromFile( name, "Nature\\Terrain2_vs.hls", 0, macros );
						}
					}
				}
			}
		}
	}

	//------------------------------------------------------------------------
	// Main Terrain Pixel Shader
	{
		Terrain2PixelShaderId psid;

		ShaderMacros macros;

		for( int l = 0, e = 2; l < e; l ++ )
		{
			for( int a = 0, e = 2; a < e; a ++ )
			{
				psid.aux = a;
				psid.forward_lighting = l;

				char name[ 512 ];

				psid.ToString( name );
				psid.FillMacros( macros );

				g_Terrain2PixelShaderIdMap[ psid.Id ] = r3dRenderer->AddPixelShaderFromFile( name, "Nature\\Terrain2_ps.hls", 0, macros );
			}
		}
	}

	//------------------------------------------------------------------------
	// Shadow PS
	g_Terrain2ShadowPSId = r3dRenderer->AddPixelShaderFromFile( "PS_TERRA_SM", "Nature\\TerrainV2_SM_ps.hls", 1 );

	//------------------------------------------------------------------------
	// Tile mip generation shaders

	g_AtlasMipVSId = r3dRenderer->AddVertexShaderFromFile( "VS_TERRAIN_ATLAS_MIP", "Nature\\TerrainAtlasMip_vs.hls" );
	g_AtlasMipPSId = r3dRenderer->AddPixelShaderFromFile( "PS_TERRAIN_ATLAS_MIP", "Nature\\TerrainAtlasMip_ps.hls" );

	//------------------------------------------------------------------------
	// road to atlas
	g_RoadAtlasPixelShaderId = r3dRenderer->AddPixelShaderFromFile( "PS_TERRA_ATLAS_ROAD_PS", "DS_FillTerraAtlas_PS.hls" );
#endif

}

//------------------------------------------------------------------------


#if R3D_TERRAIN2_FLOAT_POS

static R3D_FORCEINLINE float Conform( float val )
{
	return val;
}

static R3D_FORCEINLINE float2 Conform( float2 val )
{
	return val;
}

#else

static R3D_FORCEINLINE short Conform( float val )
{
	int res;
	if( val < 0 )
		res = int( val * 32768.f );
	else
		res = int( val * 32767.f );

	res = R3D_MIN( R3D_MAX( res, -32768 ), 32767 );

	return (short) res;
}

static R3D_FORCEINLINE short2 Conform( float2 val )
{
	return short2( Conform( val.x ), Conform( val.y ) );
}

#endif

void
r3dTerrain2::Init()
{
	RecalcVars();
	RecalcLayerVars();

#if R3D_TERRAIN_V2_GRAPHICS
	if( !g_TerraVDecl )
	{
		enum
		{
#if R3D_TERRAIN2_FLOAT_POS
			DECL_TYPE = D3DDECLTYPE_FLOAT2,
#else
			DECL_TYPE = D3DDECLTYPE_SHORT2N,
#endif
		};

		D3DVERTEXELEMENT9 dclVert[] =
		{
			{ 0, 0, DECL_TYPE, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			D3DDECL_END()
		};

		r3dDeviceTunnel::CreateVertexDeclaration( dclVert, &g_TerraVDecl );

		D3DVERTEXELEMENT9 dclDynaVert[] =
		{
			{ 0, 0, DECL_TYPE,				D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
			{ 1, 0, D3DDECLTYPE_SHORT2N,	D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
			D3DDECL_END()
		};

		r3dDeviceTunnel::CreateVertexDeclaration( dclDynaVert, &g_TerraDynaVDecl );
	}

	if( !m_AllowVFetch )
	{
		CreateDynaVertexBuffer();
	}

	InitDynamic();
#endif

}

//------------------------------------------------------------------------

void
r3dTerrain2::InitDynamic()
{
#if R3D_TERRAIN_V2_GRAPHICS
	const float TILE_DIM = (float)m_QualitySettings.AtlasTileDim;

	AddAtlasVolume();

	m_TempDiffuseRT		= r3dScreenBuffer::CreateClass( "TERRAIN_ATLAS_TEMP_DIFFUSE", TILE_DIM, TILE_DIM, ATLAS_FMT, r3dScreenBuffer::Z_NO_Z, 0, 1 );
	m_TempNormalRT		= r3dScreenBuffer::CreateClass( "TERRAIN_ATLAS_TEMP_NORMAL", TILE_DIM, TILE_DIM, ATLAS_FMT, r3dScreenBuffer::Z_NO_Z, 0, 1 );

	//------------------------------------------------------------------------

	int MAX_VERTEX_DIM = m_QualitySettings.VertexTileDim;

	for( int i = 0, e = m_QualitySettings.TileVertexDensities.COUNT; i < e; i ++ )
	{
		MAX_VERTEX_DIM = R3D_MAX( MAX_VERTEX_DIM, m_QualitySettings.VertexTileDim * ( 1 << m_QualitySettings.TileVertexDensities[ i ] ) );
	}

	int totalVertexCount = 0;

	for( int vc = m_QualitySettings.VertexTileDim; vc <= MAX_VERTEX_DIM; vc *= 2 )
	{
		m_ConnectionVertexOffsets.PushBack( totalVertexCount );
		totalVertexCount += ( vc + 1 ) * ( vc + 1 ) + vc * 4;
	}

	m_TileVertexBuffer	= new r3dVertexBuffer( totalVertexCount, sizeof( R3D_TERRAIN_VERTEX_2 ), 0, false, false, TerrainBufferMem );

	int connectedCount = 0;

	for( int vc = m_QualitySettings.VertexTileDim, d = 0; vc <= MAX_VERTEX_DIM; d ++, vc *= 2 )
	{
		m_ConnectionIndexOffsets.Resize( m_ConnectionIndexOffsets.Count() + 1 );
		m_ConnectionIndexCounts.Resize( m_ConnectionIndexCounts.Count() + 1 );

		for( int i = 0; i < TERRA_CONNECTION_TYPE_COUNT; i ++ )
		{
			m_ConnectionIndexOffsets[ d ][ i ] = connectedCount;
			int count = CountConnectionIndices( vc, i );
			m_ConnectionIndexCounts[ d ][ i ] = count / 3;

			connectedCount += count;
		}
	}

	m_TileIndexBuffer	= new r3dIndexBuffer( connectedCount + 6 + m_QualitySettings.VertexTileDim * 4 + 1, false, 2, TerrainBufferMem );

	//------------------------------------------------------------------------
	// fill vertex buffer
	{
		R3D_TERRAIN_VERTEX_2* vertexData = static_cast<R3D_TERRAIN_VERTEX_2*>( m_TileVertexBuffer->Lock() );
		R3D_TERRAIN_VERTEX_2* vertexStart = vertexData;

		for( int vc = m_QualitySettings.VertexTileDim, d = 0; vc <= MAX_VERTEX_DIM; d ++, vc *= 2 )
		{
			const int VERTEX_TILE_DIM = vc;

			float x = -1.f, z = -1.f;
			float step = 2.f / VERTEX_TILE_DIM;

			for( int i = 0, e = VERTEX_TILE_DIM + 1; i < e; i ++, z += step )
			{
				x = -1.f;
				for( int j = 0, e = VERTEX_TILE_DIM + 1; j < e; j ++, x += step )
				{
					vertexData->pos.x = Conform( x );
					vertexData->pos.y = Conform( z );

					vertexData ++;
				}
			}

			// connection vertices
			x = -1.f + 1.0f / VERTEX_TILE_DIM;
			for( int i = 0, e = VERTEX_TILE_DIM; i < e; i ++, x += step )
			{
				vertexData->pos.x = Conform( x );
				vertexData->pos.y = Conform( -1.0f );

				vertexData ++;
			}

			x = -1.f + 1.0f / VERTEX_TILE_DIM;
			for( int i = 0, e = VERTEX_TILE_DIM; i < e; i ++, x += step )
			{
				vertexData->pos.x = Conform( x );
				vertexData->pos.y = Conform( +1.0f );

				vertexData ++;
			}

			z = -1.f + 1.0f / VERTEX_TILE_DIM;
			for( int i = 0, e = VERTEX_TILE_DIM; i < e; i ++, z += step )
			{
				vertexData->pos.x = Conform( -1.0f );
				vertexData->pos.y = Conform( z );

				vertexData ++;
			}

			z = -1.f + 1.0f / VERTEX_TILE_DIM;
			for( int i = 0, e = VERTEX_TILE_DIM; i < e; i ++, z += step )
			{
				vertexData->pos.x = Conform( +1.0f );
				vertexData->pos.y = Conform( z );

				vertexData ++;
			}
		}

		r3d_assert( vertexData - vertexStart == m_TileVertexBuffer->GetItemCount() );

		m_TileVertexBuffer->Unlock();
	}

	//------------------------------------------------------------------------
	// fill index buffer
	{
		UINT16* indexData = (UINT16*) m_TileIndexBuffer->Lock();

		UINT16* pLodIndStart = indexData;

		for( int vc = m_QualitySettings.VertexTileDim, d = 0; vc <= MAX_VERTEX_DIM; d ++, vc *= 2 )
		{
			for( int i = 0, e = m_ConnectionIndexOffsets[ 0 ].COUNT; i < e; i ++ )
			{
				indexData += ConstructConnectionIndices( indexData, vc, i );
			}
		}

		int v0 = 0;
		int v1 = m_QualitySettings.VertexTileDim;
		int v2 = ( m_QualitySettings.VertexTileDim + 1 ) * m_QualitySettings.VertexTileDim;
		int v3 = ( m_QualitySettings.VertexTileDim + 1 ) * ( m_QualitySettings.VertexTileDim + 1 ) - 1;

		m_4VertTileIndexOffset = indexData - pLodIndStart;

		*indexData++ = v0;
		*indexData++ = v1;
		*indexData++ = v2;
		*indexData++ = v1;
		*indexData++ = v2;
		*indexData++ = v3;

		m_DebugVisIndexOffset = indexData - pLodIndStart;

		*indexData++ = 0;

		for( int i = 0, e = m_QualitySettings.VertexTileDim; i < e; i ++ )
		{
			*indexData ++ = i + 1;
		}

		for( int i = 1, e = m_QualitySettings.VertexTileDim + 1; i < e; i ++ )
		{
			*indexData ++ = i * ( m_QualitySettings.VertexTileDim + 1 ) + m_QualitySettings.VertexTileDim;
		}

		for( int i = 1, e = m_QualitySettings.VertexTileDim + 1; i < e; i ++ )
		{
			*indexData ++ = ( m_QualitySettings.VertexTileDim + 1 ) * m_QualitySettings.VertexTileDim + m_QualitySettings.VertexTileDim - i;
		}

		for( int i = 1, e = m_QualitySettings.VertexTileDim + 1; i < e; i ++ )
		{
			*indexData ++ = ( m_QualitySettings.VertexTileDim - i ) * ( m_QualitySettings.VertexTileDim + 1 );
		}

		r3d_assert( indexData - pLodIndStart == m_TileIndexBuffer->GetItemCount() );

		m_TileIndexBuffer->Unlock();
	}

	//------------------------------------------------------------------------
	// unit quad vertex buffer

	m_UnitQuadVertexBuffer = new r3dVertexBuffer( 4, sizeof (R3D_TERRAIN_VERTEX_2), 0, false, false, TerrainBufferMem );

	{
		R3D_TERRAIN_VERTEX_2* vertexData = static_cast<R3D_TERRAIN_VERTEX_2*>( m_UnitQuadVertexBuffer->Lock() );

		vertexData ++ ->pos = Conform( float2( -1.0f, -1.0f ) );
		vertexData ++ ->pos = Conform( float2( +1.0f, -1.0f ) );
		vertexData ++ ->pos = Conform( float2( -1.0f, +1.0f ) );
		vertexData ++ ->pos = Conform( float2( +1.0f, +1.0f ) );

		m_UnitQuadVertexBuffer->Unlock();
	}
#endif

	m_MaxAllocatedTiles = 0;
}

//------------------------------------------------------------------------

void
r3dTerrain2::CloseDynamic()
{
#if R3D_TERRAIN_V2_GRAPHICS

	FreeAtlas();

	for( int i = 0, e = (int) m_AllocTileLodArray.Count(); i < e; i ++ )
	{
		m_AllocTileLodArray[ i ].Clear();
	}

	SAFE_DELETE( m_TempDiffuseRT );
	SAFE_DELETE( m_TempNormalRT	);

	SAFE_DELETE( m_UnitQuadVertexBuffer );

	SAFE_DELETE( m_TileVertexBuffer );
	SAFE_DELETE( m_TileIndexBuffer );

	m_ConnectionVertexOffsets.Clear();
	m_ConnectionIndexCounts.Clear();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::Close()
{
#if R3D_TERRAIN_V2_GRAPHICS

	for( int i = 0, e = m_Layers.Count(); i < e; i ++ )
	{
		r3dTerrainLayer& layer = m_Layers[ i ];

		if( layer.DiffuseTex )
		{
			r3dRenderer->DeleteTexture( layer.DiffuseTex );
			layer.DiffuseTex = NULL;
		}

		if( layer.NormalTex )
		{
			r3dRenderer->DeleteTexture( layer.NormalTex );
			layer.NormalTex = NULL;
		}
	}

	if( m_BaseLayer.DiffuseTex )
	{
		r3dRenderer->DeleteTexture( m_BaseLayer.DiffuseTex );
		m_BaseLayer.DiffuseTex = NULL;
	}

	if( m_BaseLayer.NormalTex )
	{
		r3dRenderer->DeleteTexture( m_BaseLayer.NormalTex );
		m_BaseLayer.NormalTex = NULL;
	}

	m_Layers.Clear();

	for( int i = 0, e = (int)m_Masks.Count(); i < e; i ++ )
	{
		r3dRenderer->DeleteTexture( m_Masks[ i ] );
		m_Masks[ i ] = NULL;
	}

	m_Masks.Clear();

	if( m_ColorTex )
	{
		r3dRenderer->DeleteTexture( m_ColorTex );
		m_ColorTex = NULL;
	}

	if( m_HeightTex )
	{
		r3dRenderer->DeleteTexture( m_HeightTex );
		m_HeightTex = NULL;
	}

	if( m_NormalTex )
	{
		r3dRenderer->DeleteTexture( m_NormalTex );
		m_NormalTex = NULL;
	}

	if( m_DetailNormalTex )
	{
		r3dRenderer->DeleteTexture( m_DetailNormalTex );
		m_DetailNormalTex = NULL;
	}

	SAFE_DELETE( m_DynamicVertexBuffer );
#endif

	CloseDynamic();
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateAtlas( const r3dCamera& cam )
{
#if R3D_TERRAIN_V2_GRAPHICS

	if( !m_IsQualityUpdated )
	{
		UpdateQualitySettings();
		m_IsQualityUpdated = 1;
	}

#ifndef FINAL_BUILD
	if( d_terrain2_no_updates->GetInt() )
		return;
#endif

	UpdateTiles( cam );
	UpdateTileMips();

	UpdateEditor();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::Render( const r3dCamera& cam )
{
#if R3D_TERRAIN_V2_GRAPHICS
	extern int _render_Terrain;
	if(!_render_Terrain) return;

	R3DPROFILE_FUNCTION( "r3dTerrain2::Render" );

	D3DPERF_BeginEvent( 0, L"r3dTerrain2::Render" );

	UpdateVisibleTiles();

#ifndef FINAL_BUILD
	if( r_terrain2_show_tiles->GetInt() && m_VisibleTiles.Count() )
	{
		m_DebugVisibleTiles.Resize( m_VisibleTiles.Count() );
		memcpy( &m_DebugVisibleTiles[ 0 ], &m_VisibleTiles[ 0 ], m_VisibleTiles.Count() * sizeof m_VisibleTiles[ 0 ] );
	}
	else
	{
		m_DebugVisibleTiles.Clear();
	}
#endif

	StartTileRendering();

	for( int i = 0, e  = m_VisibleTiles.Count(); i < e; i ++ )
	{
		const AllocatedTile* tile = m_VisibleTiles[ i ];
		RenderTile( tile );
	}

	EndTileRendering();

	D3DPERF_EndEvent();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::RenderShadows()
{
#if R3D_TERRAIN_V2_GRAPHICS
	R3DPROFILE_FUNCTION( "r3dTerrain2::RenderShadows" );

	D3DPERF_BeginEvent( 0, L"r3dTerrain2::RenderShadows" );

	UpdateVisibleTiles();

	StartShadowRender();

	for( int i = 0, e  = m_VisibleTiles.Count(); i < e; i ++ )
	{
		const AllocatedTile* tile = m_VisibleTiles[ i ];
		RenderShadowTile( tile );
	}

	StopShadowRender();

	D3DPERF_EndEvent();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::RenderDepth()
{
#if R3D_TERRAIN_V2_GRAPHICS
	R3DPROFILE_FUNCTION( "r3dTerrain2::RenderDepth" );

	D3DPERF_BeginEvent( 0, L"r3dTerrain2::RenderDepth" );

	UpdateVisibleTiles();

	StartDepthRender();

	for( int i = 0, e  = m_VisibleTiles.Count(); i < e; i ++ )
	{
		const AllocatedTile* tile = m_VisibleTiles[ i ];
		RenderDepthTile( tile );
	}

	StopDepthRender();

	D3DPERF_EndEvent();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::DrawDebug()
{
#if R3D_TERRAIN_V2_GRAPHICS
#ifndef FINAL_BUILD
	if( r_terrain2_show_atlas->GetInt() )
	{
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );

		int vol = r_terrain2_show_atlas_volume->GetInt();

		vol = R3D_MIN( R3D_MAX( vol, 0 ), (int)m_Atlas.Count() - 1 );

		r3dTexture* tex = m_Atlas[ vol ].Diffuse->Tex;

		float scale = 1.0f / r_terrain2_show_atlas_scale->GetFloat();

		float w = scale * tex->GetWidth();
		float h = scale * tex->GetHeight();

		float off_x = scale * r_terrain2_show_atlas_off_x->GetFloat();
		float off_y = scale * r_terrain2_show_atlas_off_y->GetFloat();

		r3dColor colr = r3dColor::white;
		colr.A = R3D_MAX( R3D_MIN( int( r_terrain2_show_atlas_oppa->GetFloat() * 255 ), 255 ), 0 );

		r3dDrawBox2D( r3dRenderer->ScreenW2 - w / 2 + off_x, r3dRenderer->ScreenH2 - h / 2 + off_y, w, h, colr, tex );

		r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
	}

	if( r_terrain2_show_splat->GetInt() )
	{
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );

		int idx = r_terrain2_show_splat_idx->GetInt();

		idx = R3D_MIN( R3D_MAX( idx, 0 ), (int)m_Masks.Count() - 1 );

		r3dTexture* tex = m_Masks[ idx ];

		float w = 512;
		float h = 512;

		r3dColor colr = r3dColor::white;

		r3dDrawBox2D( r3dRenderer->ScreenW2 - w / 2, r3dRenderer->ScreenH2 - h / 2, w, h, colr, tex );

		r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
	}

	if( r_terrain2_show_tiles->GetInt() )
	{
		Terrain2VertexShaderId vsid;

		vsid.shadowPath = 0;
		vsid.depthPath = 0;
		vsid.vfetchless = !m_AllowVFetch;

		r3dRenderer->SetValidVertexShader( g_Terrain2VertexShaderIdMap[ vsid.Id ] );

		extern int g_FwdColorPS;
		r3dRenderer->SetPixelShader( g_FwdColorPS );

		float psColor[ 4 ] = { 0, 1, 0, 1 };
		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, psColor, 1 ) );

		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_PUSH );

		SetupTileRenderingVertexStates();

		for( int i = 0, e = (int)m_DebugVisibleTiles.Count(); i < e; i ++ )
		{
			const AllocatedTile* tile = m_DebugVisibleTiles[ i ];

			// hack offset...
			float oldOffset = m_HeightOffset;

			m_HeightOffset += 0.05f;

			SetTileRenderVSConsts( tile );

			m_HeightOffset = oldOffset;

			r3dRenderer->DrawIndexed(	D3DPT_LINESTRIP, 0, 0,
										( m_QualitySettings.VertexTileDim + 1 ) * ( m_QualitySettings.VertexTileDim + 1 ),
										m_DebugVisIndexOffset,
										m_QualitySettings.VertexTileDim * 4 );
		}

		StopUsingTileGeom();

		r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
	}
#endif
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::RecalcLayerVars()
{
#if R3D_TERRAIN_V2_GRAPHICS
	for( int i = 0, e = m_Layers.Count(); i < e; i ++ )
	{
		RecalcLayerVar( &m_Layers[ i ] );
	}

	RecalcLayerVar( &m_BaseLayer );

	m_MatTypeIdxes.Clear();
	m_MatTypeIdxes.Resize( m_Layers.Count() + 1 );

	m_MatTypeIdxes[ 0 ] = g_pMaterialTypes->GetIdx( m_BaseLayer.MaterialTypeName );

	for( int i = 0, e = m_Layers.Count(); i < e; i ++ )
	{
		const r3dTerrainLayer& layer = m_Layers[ i ];
		if(g_pMaterialTypes)
			m_MatTypeIdxes[ i + 1 ] = g_pMaterialTypes->GetIdx( layer.MaterialTypeName );
	}
#endif
}

//------------------------------------------------------------------------

int	r3dTerrain2::Load1( const char* DirName )
{
#if R3D_TERRAIN_V2_GRAPHICS
	char 	TempStr[512];

	m_Path = DirName;

	sprintf( TempStr, "%s/terrain.ini", DirName );

	r3d_assert ( r3dFileExists( TempStr ) );

	r3dOutToLog("TERRAIN2: Loading from ancient script '%s'\n", TempStr );
	if( !LoadFromScript1( TempStr, DirName ) )
		return 0;

	r3dFile	*f = r3d_open( Va( "%s/terrain.heightmap", m_Path.c_str() ), "rb");

	if( !f )
		return 0;

	RecalcVars();

	if( !LoadBin1( f ) )
		return 0;

	ConvertSplatsFrom1();

	Init();

	UpdateRoadInfo();

	UpdateDesc();

	m_BaseQualitySettings[ QS_HIGH ] = m_QualitySettings;
	PopulateInferiorQualitySettings();

	m_IsQualityUpdated = 0;
	m_IsLoaded = 1;
#endif

	return 1;
}

//------------------------------------------------------------------------

bool
r3dTerrain2::LoadFromScript1( const char * fileName, const char* sourceLevelPath )
{
#if R3D_TERRAIN_V2_GRAPHICS
	Script_c script;

	if ( ! script.OpenFile( fileName ) )
		return false;

	char buffer[ MAX_PATH ];

	int layerCount = 0;

	while ( ! script.EndOfFile() )
	{
		script.GetToken( buffer );
		if ( ! *buffer )
			break;

		if ( ! strcmp( buffer, "size:" ) )
		{
			m_VertexCountX = int( script.GetFloat() );
			m_VertexCountZ = m_VertexCountX;

			RecalcTileCounts();
		}
		else if ( ! strcmp( buffer, "cell_size:" ) )
		{
			m_TileUnitWorldDim = script.GetFloat();
		}
		else if ( ! strcmp( buffer, "height:" ) )
		{
			m_HeightScale = script.GetFloat();
		}
		else if ( ! strcmp( buffer, "normal_scl:" ) )
		{
			// unused
			script.GetFloat();
		}
		else if ( ! strcmp( buffer, "hm_blend:" ) )
		{
			// unused
			script.GetFloat();
		}
		else if ( ! strcmp( buffer, "split_dist:" ) )
		{
			// unused
			script.GetFloat();
		}
		else if ( ! strcmp( buffer, "lod:" ) )
		{
			// unused
			script.GetFloat();
			script.GetFloat();
			script.GetFloat();
		}
		else if ( ! strcmp( buffer, "tiling_mode:" ) )
		{
			script.GetToken( buffer );
			// unused
		}
		else if ( ! strcmp( buffer, "base_layer" ) )
		{
			LoadLayerFromScript1( &script, &m_BaseLayer, &layerCount );
		}
		else if ( ! strcmp( buffer, "material" ) )
		{
			LoadMaterialsFromScript1( &script, sourceLevelPath );
		}
		else if (	! strcmp( buffer, "SimpleTerrainRenderFadeStart:" ) ||
					! strcmp( buffer, "shader_lod_fade_start:" ) )
		{
			// unused
			script.GetFloat();
		}
		else if (	! strcmp( buffer, "SimpleTerrainRenderFadeEnd:" ) ||
					! strcmp( buffer, "shader_lod_fade_end:" ) )
		{
			// unused
			script.GetFloat();
		}
		else if( ! strcmp( buffer, "camera_near_plane:" ))
		{
			r_near_plane->SetFloat( script.GetFloat() );
		}
		else if( ! strcmp( buffer, "camera_far_plane:" ) )
		{
			r_far_plane->SetFloat( script.GetFloat() );
		}
	}
	r3dOutToLog("TERRAIN2: Finished reading ancient script file\n");
	script.CloseFile();

	if( !m_SplatResolutionU )
	{
		m_SplatResolutionU = m_VertexCountX;
		m_SplatResolutionV = m_SplatResolutionU;
	}

#endif

	return true;
}

//------------------------------------------------------------------------

bool
r3dTerrain2::LoadLayerFromScript1( Script_c *script, r3dTerrainLayer* layer, int* count )
{
#if !R3D_TERRAIN_V2_GRAPHICS
	return false;
#endif

	char buffer[ MAX_PATH ];
	char szName[ MAX_PATH ];

	bool hasTextures = false;
    // fixed buffer steepsounds
	int DownScale = GetTerraTexDownScale();

	script->SkipToken( "{" );
	while ( ! script->EndOfFile() )
	{
		script->GetToken( buffer );
		if ( *buffer == '}' )
			break;

		if ( ! strcmp( buffer, "map_diffuse:" ) )
		{
			hasTextures = true;

			script->GetString( szName, sizeof( szName ) );
			layer->DiffuseTex = r3dRenderer->LoadTexture( szName, D3DFMT_UNKNOWN, false, DownScale );
		}
		else if( !strcmp(buffer, "split_enable:" ))
		{
			// unused
			script->GetInt();
		}
		else if ( ! strcmp( buffer, "scale:" ) )
		{
			layer->ScaleU = script->GetFloat();
			layer->ScaleV = layer->ScaleU;
		}
		else if ( ! strcmp( buffer, "split:" ) )
		{
			// unused
			script->GetFloat();
		}
		else if ( ! strcmp( buffer, "map_normal:" ) )
		{
			hasTextures = true;

			script->GetString( szName, sizeof( szName ) );
			layer->NormalTex = r3dRenderer->LoadTexture( szName, D3DFMT_UNKNOWN, false, DownScale );
		}
		else if ( ! strcmp( buffer, "gloss:" ) )
		{
			layer->SpecularPow = script->GetFloat();
		}
		else if( ! strcmp( buffer, "mat_type:"  ) )
		{
			char buff[ 512 ];

			buff[ sizeof buff - 1 ] = 0;

			script->GetLine( buff, sizeof buff - 1 );

			if(strstr(strupr(buff), "CONCRETE"))
				strcpy(buff,"Concrete");
			else if(strstr(strupr(buff), "DIRT"))
				strcpy(buff,"Dirt");
			else if(strstr(strupr(buff), "SAND"))
				strcpy(buff,"Sand");
			else if(strstr(strupr(buff), "SNOW"))
				strcpy(buff,"Snow");
			else if(strstr(strupr(buff), "GRASS"))
				strcpy(buff,"Grass");
			else if(strstr(strupr(buff), "FOREST"))
				strcpy(buff,"Forest");
			else if(strstr(strupr(buff), "WOOD"))
				strcpy(buff,"Wood");
			else if(strstr(strupr(buff), "METAL_RESOURCES"))
				strcpy(buff,"Metal_Resources");
			else if(strstr(strupr(buff), "WOOD_RESOURCES"))
				strcpy(buff,"Wood_Resources");
			else if(strstr(strupr(buff), "WATER"))
				strcpy(buff,"Water");		
			else if(strstr(strupr(buff), "METAL"))
				strcpy(buff,"Metal");				
				

			layer->MaterialTypeName = buff;

			/*char buff[ 512 ];

			buff[ sizeof buff - 1 ] = 0;

			script->GetLine( buff, sizeof buff - 1 );

			char name[ 128 ];

			name[ 0 ] = 0;

			sscanf( buff, "%31s", name );

			layer->MaterialTypeName = name;*/
		}
	}

	char name[ 256 ];

	sprintf( name, "Layer %d", *count );

	layer->Name = name;

	++*count;

	return hasTextures;
}

//------------------------------------------------------------------------

void
r3dTerrain2::LoadMaterialsFromScript1( Script_c *script, const char* sourceLevelPath )
{
#if !R3D_TERRAIN_V2_GRAPHICS
	return;
#endif

	char buffer[ MAX_PATH ];
	char szName[ MAX_PATH ] = {0};

	script->SkipToken( "{" );

	bool layersHasTextures = false;

	#define TERRAIN_LAYERS_PER_MAT 4

	int channelIdx = 0;

	int startMatLayer = m_Layers.Count();

	int layerCount = 0;

	while ( ! script->EndOfFile() )
	{
		script->GetToken( buffer );
		if ( *buffer == '}' )
			break;

		if ( ! strcmp( buffer, "map_splat_mask:" ) )
		{
			char ralativePath[ MAX_PATH ] = {0};
			script->GetString( ralativePath, sizeof( ralativePath ) );
			char levelDir[MAX_PATH];
			sprintf(levelDir, "%s", sourceLevelPath);
			_strlwr(levelDir);
			_strlwr(ralativePath);

			int rl = strlen(ralativePath);
			int dl = strlen(levelDir);

			std::replace(levelDir, levelDir + dl, '\\', '/');
			std::replace(ralativePath, ralativePath + rl, '\\', '/');

			char* fr = strstr(ralativePath, levelDir);
			if(!fr)
			{
				sprintf(szName, "%s/%s", levelDir, ralativePath);
			}
			else
			{
				sprintf(szName, "%s", ralativePath);
			}


		}
		else if ( ! strcmp( buffer, "layer" ) )
		{
			m_Layers.Resize( m_Layers.Count() + 1 );
			m_Layers.GetLast().ChannelIdx = channelIdx ++;
			layersHasTextures |= LoadLayerFromScript1( script, &m_Layers.GetLast(), &layerCount );
		}
	}

	int keepLayer = 0;

	if( szName[0] && layersHasTextures )
	{
		r3d_assert( g_bEditMode );

		{
			m_Masks.Resize( m_Masks.Count() + 1 );

			int width = m_TileCountX * m_QualitySettings.VertexTileDim;
			int height = m_TileCountZ * m_QualitySettings.VertexTileDim;

			IDirect3DTexture9* tex;
			D3D_V( D3DXCreateTextureFromFileEx( r3dRenderer->pd3ddev, szName, width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM,  D3DX_DEFAULT,  D3DX_DEFAULT, 0, NULL, NULL, &tex ) );

			r3dTexture* r3dtex = r3dRenderer->AllocateTexture();
			r3dD3DTextureTunnel texTunnel;

			texTunnel.Set( tex );

			r3dtex->Setup( width, height, 1,  D3DFMT_A8R8G8B8, 1, &texTunnel, false );

			m_Masks.GetLast() = r3dtex;

			if( !(m_Masks.GetLast()->PendingAsyncOps() ) )
			{
				r3dRenderer->DeleteTexture( m_Masks.GetLast() );
				m_Masks.Resize( m_Masks.Count() - 1 );
			}
		}
	}
	else
	{
		m_Layers.Resize( startMatLayer );
	}
}

//------------------------------------------------------------------------

static void check_fread( void* p, size_t size, r3dFile* fin )
{
	size_t num_read = fread( p, size, 1, fin );
	r3d_assert( num_read == 1 );
}

void r3dTerrain2::ReadTerrainHeader( r3dFile* file, uint32_t& dwSignature, uint32_t& dwVersion )
{
	fread( &dwSignature, sizeof( dwSignature ), 1, file );
	if ( dwSignature == TERRAIN2_SIGNATURE )
	{
		fread( &dwVersion, sizeof( dwVersion ), 1, file );
	}
	else
	{
		fseek( file, 0, SEEK_SET );
		r3dArtBug( "!r3dTerrain::InitData : Old version terrain data file" );
	}
}

bool
r3dTerrain2::LoadBin1( r3dFile* file )
{
#if R3D_TERRAIN_V2_GRAPHICS
	uint32_t signature, version;
	ReadTerrainHeader( file, signature, version );

	r3d_assert( version == 5 );

	int totalCountX = m_TileCountX * m_QualitySettings.VertexTileDim;
	// bcox we supported only square in 1
	int totalCountZ = totalCountX;
	int dim = totalCountX;
	int elemCount = dim * dim;

	float minHeight;
	float maxHeight;

	check_fread( &minHeight, sizeof minHeight, file );
	check_fread( &maxHeight, sizeof minHeight, file );

	m_HeightOffset = minHeight;
	m_HeightScale = maxHeight - minHeight;

	//------------------------------------------------------------------------
	// height texture
	{
		Shorts hfData( elemCount );

		check_fread( &hfData[0], hfData.Count() * sizeof( hfData[0] ), file );

		InitFromHeights( hfData, true );
	}

	//------------------------------------------------------------------------
	// color texture
	{
		uint32_t* colorData;

		colorData	= new uint32_t[ elemCount ];
		uint32_t memSize = elemCount * sizeof(uint32_t);

		fseek( file, -(int)memSize, SEEK_END );

		fread( colorData, memSize, 1, file );

		m_ColorTex = r3dRenderer->AllocateTexture();

		IDirect3DTexture9* tex_argb;
		D3D_V( r3dRenderer->pd3ddev->CreateTexture( dim, dim, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &tex_argb, NULL ) );

		D3DLOCKED_RECT lrect;

		D3D_V( tex_argb->LockRect( 0, &lrect, NULL, 0 ) );

		memcpy( lrect.pBits, colorData, memSize );

		for( int i = 0, e = memSize / sizeof( uint32_t ); i < e; i ++ )
		{
			((uint32_t*)lrect.pBits)[ i ] |= 0xff000000;
		}

		tex_argb->UnlockRect( 0 );

		ID3DXBuffer* fileInMem;
		D3D_V( D3DXSaveTextureToFileInMemory( &fileInMem, D3DXIFF_DDS, tex_argb, NULL ) );

		SAFE_RELEASE( tex_argb );

		IDirect3DTexture9* tex_dxt1;

		D3D_V( D3DXCreateTextureFromFileInMemoryEx(	r3dRenderer->pd3ddev, fileInMem->GetBufferPointer(), fileInMem->GetBufferSize(), dim, dim, 1, 0,
													TERRA2_COLORMODULATION_FORMAT, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &tex_dxt1 ) );

		r3dD3DTextureTunnel tunnel;

		tunnel.Set( tex_dxt1 );

		m_ColorTex->Setup( dim, dim, 1, TERRA2_COLORMODULATION_FORMAT, 1, &tunnel, false );

		SAFE_RELEASE( fileInMem );

		SAFE_DELETE_ARRAY( colorData );
	}

	//------------------------------------------------------------------------
#endif

	return 1;
}

//------------------------------------------------------------------------

void
r3dTerrain2::ConvertSplatsFrom1()
{
#if R3D_TERRAIN_V2_GRAPHICS

	int baseWidth = m_Masks[ 0 ]->GetWidth();
	int baseHeight = m_Masks[ 0 ]->GetHeight();

	int elemCount = baseWidth * baseHeight;

	for( int i = 0, e = m_Masks.Count(); i < e; i ++ )
	{
		r3dTexture* mask = m_Masks[ i ];
		r3d_assert( mask->GetWidth() == baseWidth
						&&
					mask->GetHeight() == baseHeight );

		int lastChannelCount = m_LayerChannels.Count();
		m_LayerChannels.Resize( lastChannelCount + 4 );

		// sizzle numbers to represent ARGB
		LayerChannel& ch3 = m_LayerChannels[ lastChannelCount ++ ];
		LayerChannel& ch2 = m_LayerChannels[ lastChannelCount ++ ];
		LayerChannel& ch1 = m_LayerChannels[ lastChannelCount ++ ];
		LayerChannel& ch0 = m_LayerChannels[ lastChannelCount ++ ];

		ch0.Resize( elemCount );
		ch1.Resize( elemCount );
		ch2.Resize( elemCount );
		ch3.Resize( elemCount );

#pragma pack( push, 1 )
		struct Channeled
		{
			unsigned char x;
			unsigned char y;
			unsigned char z;
			unsigned char w;
		} * locked = (Channeled*) mask->Lock( 0 );
#pragma pack( pop )

		for( int i = 0, e = elemCount; i < e; i ++, locked ++ )
		{
			int x = i % baseWidth;
			int z = i / baseWidth;

			int idx = x + ( baseWidth - z - 1 ) * baseWidth;

			ch0[ idx ] = locked->x;
			ch1[ idx ] = locked->y;
			ch2[ idx ] = locked->z;
			ch3[ idx ] = locked->w;
		}

		mask->Unlock();
	}

	for( int i = 0, e = m_Masks.Count(); i < e; i ++ )
	{
		r3dRenderer->DeleteTexture( m_Masks[ i ] );
	}

	m_Masks.Clear();

	struct TempTex
	{
		TempTex( int baseWidth, int baseHeight )
		{
			D3D_V( r3dRenderer->pd3ddev->CreateTexture( baseWidth, baseHeight, 1, 0, D3DFMT_R5G6B5, D3DPOOL_SYSTEMMEM, &resource, NULL ) );
		}

		~TempTex()
		{
			SAFE_RELEASE( resource );
		}

		IDirect3DTexture9* resource;
	} tempTex( baseWidth, baseHeight );

	DoUpdateDominantLayerData( baseWidth, baseHeight );

	for( int L = 0, e = m_Layers.Count(); L < e; L += 3 )
	{
		int channelCount = R3D_MIN( int( m_Layers.Count() - L ) , 3 );

		D3DLOCKED_RECT lockedRect;
		D3D_V( tempTex.resource->LockRect( 0, &lockedRect, NULL, 0 ) );

		struct Channeled16
		{
			UINT16 b : 5;
			UINT16 g : 6;
			UINT16 r : 5;
		} * locked = ( Channeled16* )lockedRect.pBits;

		COMPILE_ASSERT( sizeof *locked == 2 );

		struct To16bit
		{
			R3D_FORCEINLINE UINT8 operator() ( float val, int base )
			{
				int res = int( val * base / 255 );

				res = R3D_MAX( R3D_MIN( (int)res, base ), 0 );

				return UINT8 ( res );
			}

		} to16bit;

		for( int i = 0, e = elemCount; i < e; i ++, locked ++ )
		{
			int ch = 0;

			locked->r = to16bit( m_LayerChannels[ L + 0 ][ i ], 31 );

			if( ++ch < channelCount )
				locked->g = to16bit( m_LayerChannels[ L + 1 ][ i ], 63 );
			else
				locked->g = 0;

			if( ++ch < channelCount )
				locked->b = to16bit( m_LayerChannels[ L + 2 ][ i ], 31 );
			else
				locked->b = 0;
		}

		tempTex.resource->UnlockRect( 0 );

		ID3DXBuffer * buffer;
		D3D_V( D3DXSaveTextureToFileInMemory( &buffer, D3DXIFF_DDS, tempTex.resource, NULL ) );

		IDirect3DTexture9* finalTex;
		D3D_V( D3DXCreateTextureFromFileInMemoryEx( r3dRenderer->pd3ddev, buffer->GetBufferPointer(), buffer->GetBufferSize(), baseWidth, baseHeight, 1, 0, TERRA2_LAYERMASK_FORMAT, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &finalTex ) );

		r3dTexture* mask = r3dRenderer->AllocateTexture();

		r3dD3DTextureTunnel tunnel;
		tunnel.Set( finalTex );
		mask->Setup( baseWidth, baseHeight, 1, TERRA2_LAYERMASK_FORMAT, 1, &tunnel, false );

		SAFE_RELEASE( buffer );

		m_Masks.PushBack( mask );
	}

	InitLayerBaseBitMasks();
	InitLayerBitMaskChains();
	InitLayers();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::SaveEmpty( const CreationParams& createParams )
{
#if R3D_TERRAIN_V2_GRAPHICS
	PopulateInferiorQualitySettings();

	const QualitySettings& saveQS = m_BaseQualitySettings[ QS_HIGH ];

	m_VertexCountX = createParams.CellCountX;
	m_VertexCountZ = createParams.CellCountZ;

	m_SplatResolutionU = createParams.SplatSizeX;
	m_SplatResolutionV = createParams.SplatSizeZ;

	m_TileUnitWorldDim = createParams.CellSize;
	m_HeightOffset = createParams.Height;
	m_HeightScale = 1.0f;

	int tcx = m_VertexCountX / saveQS.VertexTileDim;
	int tcz = m_VertexCountZ / saveQS.VertexTileDim;

	m_NumActiveQualityLayers = 0;

	for(; tcx > 0 && tcz > 0; tcx /= 2, tcz /= 2 )
	{
		m_NumActiveQualityLayers ++;
	}

	SetupHFScale();

	UINT16 val = UINT16( m_HeightOffset * m_HFScale );

	Shorts heightValues ( m_VertexCountX * m_VertexCountZ, val );

	IDirect3DTexture9* white_r5g6b5;

	D3D_V( r3dRenderer->pd3ddev->CreateTexture( m_SplatResolutionU, m_SplatResolutionV, 1, 0, D3DFMT_R5G6B5, D3DPOOL_SYSTEMMEM, &white_r5g6b5, NULL ) );

	D3DLOCKED_RECT lrect;
	D3D_V( white_r5g6b5->LockRect( 0, &lrect, NULL, 0 ) );

	memset( lrect.pBits, 0xff, lrect.Pitch * m_SplatResolutionV );

	D3D_V( white_r5g6b5->UnlockRect( 0 ) );

	ID3DXBuffer* fileInMem;
	D3D_V( D3DXSaveTextureToFileInMemory( &fileInMem, D3DXIFF_DDS, white_r5g6b5, NULL ) );

	SAFE_RELEASE( white_r5g6b5 );

	IDirect3DTexture9* tex_dxt1;

	D3D_V( D3DXCreateTextureFromFileInMemoryEx(	r3dRenderer->pd3ddev, fileInMem->GetBufferPointer(), fileInMem->GetBufferSize(), m_SplatResolutionU, m_SplatResolutionV, 1, 0,
												TERRA2_COLORMODULATION_FORMAT, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &tex_dxt1 ) );

	r3dD3DTextureTunnel tunnel;

	tunnel.Set( tex_dxt1 );

	m_ColorTex = r3dRenderer->AllocateTexture();
	m_ColorTex->Setup( m_SplatResolutionU, m_SplatResolutionV, 1, TERRA2_COLORMODULATION_FORMAT, 1, &tunnel, false );

	SAFE_RELEASE( fileInMem );

	int baseW = m_SplatResolutionU / QualitySettings::MIN_VERTEX_TILE_DIM;
	int baseH = m_SplatResolutionV / QualitySettings::MIN_VERTEX_TILE_DIM;

	m_DominantLayerData.Resize( baseW, baseH, 0 );

	m_NormalTex = r3dRenderer->AllocateTexture();
	m_NormalTex->Create( m_VertexCountX * m_NormalDensity, m_VertexCountZ * m_NormalDensity, D3DFMT_R5G6B5, 1 );

#pragma pack( push )
#pragma pack( 1 )
	{
		struct RGB16
		{
			UINT16 b : 5;
			UINT16 g : 6;
			UINT16 r : 5;

		} * ptr = (RGB16*) m_NormalTex->Lock( 1, NULL );

		int lock_pitch = m_NormalTex->GetLockPitch();

		for( int y = 0, e = m_NormalTex->GetHeight(); y < e; y ++ )
		{
			for( int x = 0, e = m_NormalTex->GetHeight(); x < e; x ++ )
			{
				int idx = x + y * lock_pitch / sizeof( *ptr );

				ptr[ idx ].r = 16;
				ptr[ idx ].g = 63;
				ptr[ idx ].b = 16;
			}
		}

		m_NormalTex->Unlock();
	}
#pragma pack( pop )

	DoSave( createParams.LevelDir.c_str(), heightValues, m_VertexCountX, m_VertexCountZ );
#endif
}


//------------------------------------------------------------------------

int
r3dTerrain2::Save( const char* targetDir )
{
#if R3D_TERRAIN_V2_GRAPHICS
	if( g_bEditMode )
	{
		RemoveTextureReloadListener( TerraTextureReloadListener );
	}

	r3d_assert( m_PhysicsHeightField );

	r3d_assert(_CrtCheckMemory());

	r3dTL::TArray< PxI16 > hfShrinkedSamples( m_VertexCountX * m_VertexCountZ );

	SaveHeightField( &hfShrinkedSamples );

	gSaveCountDown = 5.0f;

	return DoSave( targetDir, hfShrinkedSamples, m_VertexCountX, m_VertexCountZ );
#else
	return 1;
#endif
}

//------------------------------------------------------------------------


int	 r3dTerrain2::Load()
{
	DoLoad( r3dGameLevel::GetHomeDir() );
	return 1;
}

//------------------------------------------------------------------------

const r3dTerrain2::Settings&
r3dTerrain2::GetSettings() const
{
	return m_Settings;
}

//------------------------------------------------------------------------

void
r3dTerrain2::SetSettings( const Settings& sts )
{
	m_Settings = sts;
}

//------------------------------------------------------------------------

const r3dTerrain2::QualitySettings&
r3dTerrain2::GetBaseQualitySettings( int ql ) const
{
	return m_BaseQualitySettings[ ql ];
}

//------------------------------------------------------------------------

const r3dTerrain2::QualitySettings&
r3dTerrain2::GetCurrentQualitySettings() const
{
	return m_QualitySettings;
}

//------------------------------------------------------------------------

void
r3dTerrain2::SetQualitySettings( const QualitySettings& settings, bool affectBaseQS, int affectBaseQSIdx )
{
#if R3D_TERRAIN_V2_GRAPHICS
	QualitySettings fixedSettings = settings;

	fixedSettings.Sync();

	if( m_QualitySettings.BakeRoads && !settings.BakeRoads )
	{
		ReleaseRoads();
	}

	if( m_QualitySettings.Compare( fixedSettings ) )
	{
		CloseDynamic();
		m_QualitySettings = fixedSettings;
		RecalcVars();
		RecalcLayerVars();
		InitDynamic();

		Shorts heightField( m_VertexCountX * m_VertexCountZ );
		SaveHeightField( &heightField );
		InitTileField( heightField );

		InitLayerBitMaskChains();
		UpdateRoadInfo();

		if( affectBaseQS )
		{
			m_BaseQualitySettings[ affectBaseQSIdx ] = m_QualitySettings;
		}
	}

#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::GetStats( r3dTerrainStats* oStats ) const
{
	*oStats = r3dTerrainStats();

#if R3D_TERRAIN_V2_GRAPHICS
	oStats->VolumeCount = m_Atlas.Count();

	for( int i = 0, e = (int) m_Atlas.Count(); i < e; i ++ )
	{
		const AtlasVolume& vol = m_Atlas[ i ];
		oStats->TileCount += vol.Occupied.Count() - vol.FreeTiles;
		oStats->TileTextureMemory += vol.Diffuse->Tex->GetTextureSizeInVideoMemory();
		oStats->TileTextureMemory += vol.Normal->Tex->GetTextureSizeInVideoMemory();
	}

	for( int i = 0, e = (int)m_Masks.Count(); i < e; i ++ )
	{
		oStats->MaskTextureMemory += m_Masks[ i ]->GetTextureSizeInVideoMemory();
	}

	if( m_ColorTex )
	{
		oStats->MaskTextureMemory += m_ColorTex->GetTextureSizeInVideoMemory();
	}

	typedef std::set< r3dTexture* > TextureSet;
	TextureSet uniqueLayerTexes;

	for( int i = 0, e = (int)m_Layers.Count(); i < e; i ++ )
	{
		const r3dTerrainLayer& layer = m_Layers[ i ];
		uniqueLayerTexes.insert( layer.DiffuseTex );
		uniqueLayerTexes.insert( layer.NormalTex );
	}

	for( TextureSet::iterator i = uniqueLayerTexes.begin(), e = uniqueLayerTexes.end(); i != e; ++ i )
	{
		if( r3dTexture* tex = *i )
		{
			oStats->LayerTextureMemory += tex->GetTextureSizeInVideoMemory();
		}
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::ResetAtlasTiles()
{
#if R3D_TERRAIN_V2_GRAPHICS
	RemoveAllocatedTiles();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::RefreshAtlasTiles()
{
#if R3D_TERRAIN_V2_GRAPHICS
	m_TilesToUpdate.Clear();

	// free all tiles to force reallocate & update
	FreeAtlasTiles();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::RefreshAtlasTiles( const RECT& rect )
{
#if R3D_TERRAIN_V2_GRAPHICS
	m_TilesToUpdate.Clear();

	int tx0 = rect.left / m_QualitySettings.VertexTileDim;
	int tx1 = rect.right / m_QualitySettings.VertexTileDim + 1;
	int tz0 = rect.top / m_QualitySettings.VertexTileDim;
	int tz1 = rect.bottom / m_QualitySettings.VertexTileDim + 1;

	tx0 = R3D_MIN( R3D_MAX( tx0, 0 ), m_TileCountX );
	tx1 = R3D_MIN( R3D_MAX( tx1, 0 ), m_TileCountX );
	tz0 = R3D_MIN( R3D_MAX( tz0, 0 ), m_TileCountZ );
	tz1 = R3D_MIN( R3D_MAX( tz1, 0 ), m_TileCountZ );

	for( int tz = tz0; tz < tz1; tz ++ )
	{
		for( int tx = tx0; tx < tx1; tx ++ )
		{
			RefreshAtlasTile( tx, tz );
		}
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::RefreshAtlasTile( int tileX, int tileZ )
{
	for( int L = 0, e = m_NumActiveQualityLayers, tx = tileX, tz = tileZ; L < e; L ++, tx /=2, tz /= 2 )
	{
		for( int i = 0, e = (int)m_TilesToUpdate.Count(); i < e;  )
		{
			if( m_TilesToUpdate[ i ]->X == tx
				&&
				m_TilesToUpdate[ i ]->Z == tz
				&&
				m_TilesToUpdate[ i ]->L == L
				)
			{
				m_TilesToUpdate.Erase( i );
				e --;
			}
			else
			{
				i ++;
			}
		}
	}

#if R3D_TERRAIN_V2_GRAPHICS
	for( int L = 0, e = m_NumActiveQualityLayers; L < e; L ++ )
	{
		AllocatedTileArr& arr = m_AllocTileLodArray[ L ];

		for( int i = 0, e = arr.Count(); i < e; i ++ )
		{
			AllocatedTile& tile = arr[ i ];

			if( tile.X == tileX && tile.Z == tileZ )
			{
				if( tile.AtlasTileID >= 0)
				{
					FreeAtlasTile( &tile );
				}
			}
		}

		tileX /= 2;
		tileZ /= 2;
	}
#endif
}


//------------------------------------------------------------------------

void
r3dTerrain2::SetPhysUserData( void* data )
{
	if( m_PhysicsTerrain )
	{
		m_PhysicsTerrain->userData = data;
	}
}

//------------------------------------------------------------------------

void
r3dTerrain2::ClearRoadInfo()
{
#if R3D_TERRAIN_V2_GRAPHICS
	for( int i = 0, e = (int)m_RoadInfoMipChain.Count(); i < e; i ++ )
	{
		RoadInfoArr2D& arr = m_RoadInfoMipChain[ i ];

		for( int z = 0, e = (int)arr.Height(); z < e; z ++ )
		{
			for( int x = 0, e = (int)arr.Width(); x < e; x ++ )
			{
				RoadInfo* info = static_cast< RoadInfo* >( arr[ z ][ x ].ptrs[ ROAD_INFO_LENGTH - 1 ] );

				for(; info; )
				{
					void* toDelete = info;
					info = static_cast< RoadInfo* >( info->ptrs[ ROAD_INFO_LENGTH - 1 ] );
					delete toDelete;
				}
			}
		}
	}

	m_RoadInfoMipChain.Clear();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateRoadInfo()
{
#if R3D_TERRAIN_V2_GRAPHICS
	// get road objects
	r3dTL::TArray< obj_Road* > roads;

	roads.Reserve( 2048 );

	m_RoadInfoMipChain.Clear();

	if( GetCurrentQualitySettings().BakeRoads )
	{
		for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
		{
			if( obj->ObjTypeFlags & OBJTYPE_Road )
			{
				obj->ObjFlags |= OBJFLAG_SkipDraw;

				obj->Update();

				roads.PushBack( static_cast<obj_Road*>( obj ) );
			}
		}
	}

	if( roads.Count() )
	{
		struct RoadCompare
		{
			bool operator() ( obj_Road* r0, obj_Road* r1 )
			{
				return r0->drawPriority < r1->drawPriority;
			}
		} compare;

		std::sort( &roads[ 0 ], &roads[ 0 ] + roads.Count(), compare );


		int startMip = 0;

		int d = m_QualitySettings.RoadVertexDimStart;

		for(; d > m_QualitySettings.VertexTileDim; d /= 2 )
		{
			startMip ++;
		}

		m_RoadInfoMipChain.Resize( m_NumActiveQualityLayers - startMip );

		int roadTileCountX = m_VertexCountX / m_QualitySettings.RoadVertexDimStart;
		int roadTileCountZ = m_VertexCountZ / m_QualitySettings.RoadVertexDimStart;

		for( int i = 0, e = (int)m_RoadInfoMipChain.Count(); i < e;
				i ++,
				roadTileCountX /= 2,
				roadTileCountZ /= 2
				)
		{
			RoadInfoArr2D& mip = m_RoadInfoMipChain[ i ];

			r3d_assert( roadTileCountX && roadTileCountZ );

			mip.Resize( roadTileCountX, roadTileCountZ );

			float fz = 0.f, fx = 0.f;

			float dim = m_TileWorldDims[ startMip + i ];

			for( int z = 0, e = roadTileCountZ; z < e; z ++, fz += dim )
			{
				fx = 0.f;
				for( int x = 0, e = roadTileCountX; x < e; x ++, fx += dim )
				{
					for( int i = 0, e = roads.Count(); i < e; i ++ )
					{
						GameObject* road = roads[ i ];

						const r3dBoundBox& bbox = road->GetBBoxWorld();

						if( bbox.Org.x > fx + dim
								||
							bbox.Org.z > fz + dim
								||
							bbox.Org.x + bbox.Size.x < fx
								||
							bbox.Org.z + bbox.Size.z < fz
							)
							continue;

						RoadInfo* info = &mip[ z ][ x ];
						RoadInfo* prevInfo = info;

						int success = 0;

						for(; info && !success; )
						{
							for( int i = 0, e = ROAD_INFO_LENGTH - 1; i < e; i ++ )
							{
								if( !info->ptrs[ i ] )
								{
									info->ptrs[ i ] = road;
									success = 1;
									break;
								}
							}

							prevInfo = info;
							info = static_cast<RoadInfo*>( info->ptrs[ ROAD_INFO_LENGTH - 1 ] );
						}

						if( !success )
						{
							RoadInfo * target  = static_cast<RoadInfo*>( prevInfo->ptrs[ ROAD_INFO_LENGTH - 1 ] = new RoadInfo ) ;

							target->ptrs[ 0 ] = road;
						}
					}
				}
			}
		}
	}
#endif
}

//------------------------------------------------------------------------

void r3dTerrain2::OnRoadDelete( class obj_Road* road )
{
	for( int i = 0, e = m_RoadInfoMipChain.Count(); i <  e; i ++ )
	{
		RoadInfoArr2D& arr = m_RoadInfoMipChain[ i ];

		for( int z = 0, e = arr.Height(); z < e; z ++ )
		{
			for( int x = 0, e = arr.Width(); x < e; x ++ )
			{
				RoadInfo* info = &arr[ z ][ x ];

				int success = 0;

				for(; info; )
				{
					for( int i = 0, e = ROAD_INFO_LENGTH - 1; i < e; i ++ )
					{
						if( info->ptrs[ i ] == road )
						{
							for( ; i < e - 1; i ++ )
								info->ptrs[ i ] = info->ptrs[ i + 1 ];

							info->ptrs[ e - 1 ] = NULL;
						}
					}

					info = static_cast< RoadInfo* > ( info->ptrs[ ROAD_INFO_LENGTH - 1 ] );
				}

			}
		}
	}
}

//------------------------------------------------------------------------

void
r3dTerrain2::ReleaseRoads( )
{
#if R3D_TERRAIN_V2_GRAPHICS
	for( GameObject* obj = GameWorld().GetFirstObject(); obj; obj = GameWorld().GetNextObject(obj) )
	{
		if( obj->ObjTypeFlags & OBJTYPE_Road )
		{
			obj->ObjFlags &= ~OBJFLAG_SkipDraw;
		}
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::SaveHeightField( Shorts* oHeights )
{
	r3d_assert( oHeights->Count() == m_VertexCountX * m_VertexCountZ );

	uint32_t hmSize = GetHeightFieldDataSize( m_PhysicsHeightFieldDesc );

	uint32_t paranoidHMSize = hmSize + 16;

	Bytes sampleData( paranoidHMSize );

	r3d_assert(_CrtCheckMemory());

	uint32_t numWritten = m_PhysicsHeightField->saveCells( &sampleData[0], paranoidHMSize );

	r3d_assert( numWritten == hmSize );

	int sampleCount = hmSize / m_PhysicsHeightFieldDesc.samples.stride;
	char* byteSamplePtr = (char*)&sampleData[ 0 ];

	for( int i = 0, e = sampleCount; i < e; i ++, byteSamplePtr += m_PhysicsHeightFieldDesc.samples.stride )
	{
		PxHeightFieldSample* s = (PxHeightFieldSample*)byteSamplePtr;
		( *oHeights )[ i ] = s->height;
	}

}

//------------------------------------------------------------------------

void
r3dTerrain2::SaveHeightField( Floats* oHeights )
{
	Shorts shorts;

	shorts.Resize( m_VertexCountX * m_VertexCountZ );

	SaveHeightField( &shorts );

	r3d_assert( shorts.Count() == oHeights->Count() );

	for( int i = 0, e = (int)shorts.Count(); i < e; i ++ )
	{
		int x = i % m_VertexCountX;
		int z = i / m_VertexCountX;

		(*oHeights)[ x * m_VertexCountZ + z ] = shorts[ i ] * m_InvHFScale;
	}
}

//------------------------------------------------------------------------

bool
r3dTerrain2::UpdateHeightRanges( const Floats& floatHeights )
{
#if R3D_TERRAIN_V2_GRAPHICS
	R3DPROFILE_FUNCTION( "r3dTerrain2::UpdateHeightRanges" );

	float minHeight = floatHeights[ 0 ];
	float maxHeight = floatHeights[ 0 ];

	for( int i = 1, e = (int)floatHeights.Count(); i < e; i ++ )
	{
		minHeight = R3D_MIN( minHeight, floatHeights[ i ] );
		maxHeight = R3D_MAX( maxHeight, floatHeights[ i ] );
	}

	float heightScale = maxHeight - minHeight;

	bool rangeChange =	m_HeightOffset != minHeight
							||
						m_HeightScale != heightScale
							;

	m_HeightOffset = minHeight;
	m_HeightScale = heightScale;

	SetupHFScale();

	return rangeChange;
#else
	return false;
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::ConvertHeightField( Shorts* oHeights, const Floats& floatHeights, const RECT* rect )
{
#if R3D_TERRAIN_V2_GRAPHICS
	R3DPROFILE_FUNCTION( "r3dTerrain2::ConvertHeightField" );

	RECT r;

	if( rect )
	{
		r = *rect;
	}
	else
	{
		r.left		= 0;
		r.right		= m_VertexCountX;
		r.top		= 0;
		r.bottom	= m_VertexCountZ;
	}

	r3d_assert( oHeights->Count() == floatHeights.Count() );

	for( int z = r.top; z < r.bottom; z ++ )
	{
		for( int x = r.left; x < r.right; x ++ )
		{
			(*oHeights)[ x * m_VertexCountZ + z ] = (INT16)R3D_MIN( R3D_MAX( (int)( floatHeights[ x + m_VertexCountX * z ] * m_HFScale ) , -32767 ), 32767 );
		}
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateGraphicsHeightField( const Shorts& shorts, const RECT* rect )
{
#if R3D_TERRAIN_V2_GRAPHICS
	R3DPROFILE_FUNCTION( "r3dTerrain2::UpdateGraphicsHeightField" );

	int signedMin = 0x10000;
	int signedMax = -0x10000;

	for( int i = 0, e = (int)shorts.Count(); i < e; i ++ )
	{
		int val = shorts[ i ];

		signedMin = R3D_MIN( val, signedMin );
		signedMax = R3D_MAX( val, signedMax );
	}

	int range = signedMax - signedMin;

	int z0 = 0;
	int z1 = m_VertexCountZ;
	int x0 = 0;
	int x1 = m_VertexCountX;

	if( rect )
	{
		x0 = rect->left;
		x1 = rect->right + 1;
		z0 = rect->top;
		z1 = rect->bottom + 1;

		x0 = R3D_MIN( R3D_MAX( x0, 0 ), m_VertexCountX );
		x1 = R3D_MIN( R3D_MAX( x1, 0 ), m_VertexCountX );
		z0 = R3D_MIN( R3D_MAX( z0, 0 ), m_VertexCountZ );
		z1 = R3D_MIN( R3D_MAX( z1, 0 ), m_VertexCountZ );

		r3d_assert( x1 >= x0 );
		r3d_assert( z1 >= z0 );
	}

	int i = 0, e = (int)shorts.Count();

	if( !range )
		range = 1;

	UINT16* ptr = NULL;

	UINT64 bitshift = 16;
	int packRange = 65535;

	if( m_HeightTex )
	{
		ptr = (UINT16*)m_HeightTex->Lock( 1, rect );
	}
	else
	{
		m_HeightArr.Resize( m_VertexCountX * m_VertexCountZ );
		ptr = (UINT16*)&m_HeightArr[ 0 ];

		bitshift = 15;
		packRange = 32767;
	}

	for( int z = z0; z < z1; z ++ )
	{
		for( int x = x0; x < x1; x ++, i ++ )
		{
			r3d_assert( i < e );

			int val = shorts[ z + x * m_VertexCountZ ];

			val = ( UINT64( val - signedMin ) << bitshift ) / range;
			val = R3D_MAX( R3D_MIN( val, packRange ), 0 );

			ptr[ ( z - z0 ) * m_VertexCountX + x - x0 ] = (UINT16) val;
		}
	}

	if( m_HeightTex )
	{
		m_HeightTex->Unlock();
	}

#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateTileField( const Shorts& heightField )
{
	R3DPROFILE_FUNCTION( "r3dTerrain2::UpdateTileField" );

	InitTileField( heightField );
}


//------------------------------------------------------------------------

void
r3dTerrain2::RecalcTileField( const Shorts& heightField, const RECT* rect )
{
	R3DPROFILE_FUNCTION( "r3dTerrain2::RecalcTileField" );
	RecalcTileInfo( heightField, rect );
}

//------------------------------------------------------------------------
/*virtual*/

float
r3dTerrain2::GetHeight( int x, int z )			/*OVERRIDE*/
{
	return m_PhysicsHeightField->getHeight( (PxReal)x, (PxReal)z ) * m_InvHFScale;
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdatePhysicsHeightField( const Shorts& heightField )
{
	UpdatePhysHeightField( heightField );
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateNormals( const Floats& heights, Vectors* tempVectors0, Vectors* tempVectors1, const RECT* rect )
{
#if R3D_TERRAIN_V2_GRAPHICS
	RecalcNormalMap( heights, tempVectors0, tempVectors1, rect );
	RefreshAtlasTiles();
#endif
}

//------------------------------------------------------------------------

const r3dTerrainLayer&
r3dTerrain2::GetLayer( int idx ) const
{
	return idx ? m_Layers[ idx - 1 ] : m_BaseLayer;
}

//------------------------------------------------------------------------

void
r3dTerrain2::SetLayer( int idx, const r3dTerrainLayer& layer )
{
#if R3D_TERRAIN_V2_GRAPHICS
	( idx ? m_Layers[ idx - 1 ] : m_BaseLayer ) = layer;

	RecalcLayerVars();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateTilesWithLayer( int idx )
{
#if R3D_TERRAIN_V2_GRAPHICS
	m_TilesToUpdate.Clear();

	if( !idx )
	{
		RefreshAtlasTiles();
	}
	else
	{
		for( int i = 0, e = m_AllocTileLodArray.Count(); i < e; i ++ )
		{
			AllocatedTileArr& arr = m_AllocTileLodArray[ i ];

			for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
			{
				AllocatedTile& tile = arr[ i ];

				if( tile.AtlasVolumeID >= 0 )
				{
					LayerBitMaskMipChain& chains = m_LayerBitMasksMipChains[ idx - 1 ];

					if( chains.Count() )
					{
						if( chains[ tile.L ].Get( tile.X, tile.Z ) )
						{
							FreeAtlasTile( &tile );
						}
					}
				}
			}
		}
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::MarkLayer( int baseX, int baseZ, int lidx )
{
#if R3D_TERRAIN_V2_GRAPHICS
	if( lidx > 0 )
	{
		m_TilesToUpdate.Clear();

		m_LayerBaseBitMasks[ lidx - 1 ].Set( baseX, baseZ, 1 );

		int scale = m_QualitySettings.VertexTileDim / r3dTerrain2::QualitySettings::MIN_VERTEX_TILE_DIM;

		UpdateTileBitMaskChain( &m_LayerBitMasksMipChains[ lidx - 1 ], m_LayerBaseBitMasks[ lidx - 1 ], baseX / scale, baseZ / scale );

		int tx = baseX / scale;
		int tz = baseZ / scale;

		RefreshAtlasTile( tx, tz );
	}
#endif
}

//------------------------------------------------------------------------

r3dTexture*
r3dTerrain2::GetLayerMask( int sidx ) const
{
	return m_Masks[ sidx ];
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateLayerMaskFromReplacementMask( int sidx )
{
#if R3D_TERRAIN_V2_GRAPHICS
	r3dOutToLog( "r3dTerrain2::UpdateLayerMaskFromReplacementMask: creating DXT1 texture from editor R5G6B5 texture\n" );

	r3dTexture* newTex = r3dRenderer->AllocateTexture();
	r3dTexture* source = m_Masks[ sidx ];

	r3d_assert( source->GetD3DFormat() ==  D3DFMT_R5G6B5 );

	ID3DXBuffer* fileInMem( NULL );

	D3D_V( D3DXSaveTextureToFileInMemory( &fileInMem, D3DXIFF_DDS, source->AsTex2D(), NULL ) );

	IDirect3DTexture9* dxt1;

	D3D_V( D3DXCreateTextureFromFileInMemoryEx(
				r3dRenderer->pd3ddev, fileInMem->GetBufferPointer(), fileInMem->GetBufferSize(),
				source->GetWidth(), source->GetHeight(), 1, 0, TERRA2_LAYERMASK_FORMAT, D3DPOOL_MANAGED,
				D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &dxt1 ) );

	r3dD3DTextureTunnel tunnel;

	tunnel.Set( dxt1 );

	newTex->Setup( source->GetWidth(), source->GetHeight(), 1, TERRA2_LAYERMASK_FORMAT, 1, &tunnel, false);

	SAFE_RELEASE( fileInMem );

	// there should be replacement here now
	m_Masks[ sidx ] = newTex;

	RecalcLayerVars();
	DistributeLayerMasks();

	RefreshAtlasTiles();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::SetReplacementMask( int sidx, r3dTexture* mask )
{
#if R3D_TERRAIN_V2_GRAPHICS
	r3d_assert( m_Masks[ sidx ] && m_Masks[ sidx ]->GetD3DFormat() == TERRA2_LAYERMASK_FORMAT );

	r3dRenderer->DeleteTexture( m_Masks[ sidx ] );

	m_Masks[ sidx ] = mask;

	DistributeLayerMasks();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::OptimizeLayerMasks()
{
#if R3D_TERRAIN_V2_GRAPHICS
	r3dOutToLog( "r3dTerrain2::OptimizeLayerMasks: optimizing %d masks..\n", m_Masks.Count() );

	m_LayerChannels.Resize( m_Layers.Count() );

	for( int i = 0, e = m_Masks.Count(), c = 0; i < e; i ++, c += 3 )
	{
		r3dTexture* mask = m_Masks[ i ];

		ID3DXBuffer* fileInMem( NULL );

		D3D_V( D3DXSaveTextureToFileInMemory( &fileInMem, D3DXIFF_DDS, mask->AsTex2D(), NULL ) );

		IDirect3DTexture9* r5g6b5( NULL );

		D3D_V( D3DXCreateTextureFromFileInMemoryEx( r3dRenderer->pd3ddev, fileInMem->GetBufferPointer(), fileInMem->GetBufferSize(),
													mask->GetWidth(), mask->GetHeight(), 1, 0, D3DFMT_R5G6B5, D3DPOOL_SYSTEMMEM,
													D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &r5g6b5 ) );

		D3DLOCKED_RECT lrect;
		D3D_V( r5g6b5->LockRect( 0, &lrect, NULL, D3DLOCK_READONLY ) );

		struct Channeled16
		{
			UINT16 b : 5;
			UINT16 g : 6;
			UINT16 r : 5;
		} * locked = ( Channeled16* )lrect.pBits;

		int have1 = c + 1 < (int)m_LayerChannels.Count();
		int have2 = c + 2 < (int)m_LayerChannels.Count();

		int totalSampleCount = mask->GetWidth() * mask->GetHeight();

		m_LayerChannels[ c ].Resize( totalSampleCount );

		if( have1 )	m_LayerChannels[ c + 1 ].Resize( totalSampleCount );
		if( have2 )	m_LayerChannels[ c + 2 ].Resize( totalSampleCount );

		for( int i = 0, e = totalSampleCount; i < e; i ++, locked ++ )
		{
			m_LayerChannels[ c + 0 ][ i ] = 255.f * locked->r / 31.f;

			if( have1 )		m_LayerChannels[ c + 1 ][ i ] = 255.f * locked->g / 63.f;
			if( have2 )		m_LayerChannels[ c + 2 ][ i ] = 255.f * locked->b / 31.f;
		}

		D3D_V( r5g6b5->UnlockRect( 0 ) );

		SAFE_RELEASE( r5g6b5 );
		SAFE_RELEASE( fileInMem );
	}

	InitLayerBaseBitMasks();
	InitLayerBitMaskChains();
#endif
}

//------------------------------------------------------------------------

r3dTexture*
r3dTerrain2::GetColorTexture() const
{
	return m_ColorTex;
}

//------------------------------------------------------------------------

void
r3dTerrain2::SetReplacementColorTexture( r3dTexture* tex )
{
#if R3D_TERRAIN_V2_GRAPHICS
	r3d_assert( m_ColorTex->GetD3DFormat() == TERRA2_COLORMODULATION_FORMAT );

	r3dRenderer->DeleteTexture( m_ColorTex );

	m_ColorTex = tex;
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateColorTexture()
{
#if R3D_TERRAIN_V2_GRAPHICS
	r3d_assert( m_ColorTex->GetD3DFormat() == D3DFMT_A8R8G8B8 );
	r3d_assert( g_bEditMode );

	ID3DXBuffer* fileInMem( NULL );

	bool onDisk = false;

	HRESULT hres = D3DXSaveTextureToFileInMemory( &fileInMem, D3DXIFF_DDS, m_ColorTex->GetD3DTexture(), NULL );

	if( hres == E_OUTOFMEMORY )
	{
		D3D_V( D3DXSaveTextureToFile( "temp_tex.dds", D3DXIFF_DDS, m_ColorTex->GetD3DTexture(), NULL ) );
		r3dOutToLog( "r3dTerrain2::UpdateColorTexture: Failed to save temp texture to file in memory - saving to disk.\n" );
		onDisk = true;
	}
	else
		D3D_V( hres );


	IDirect3DTexture9* tex_dxt1;

	if( onDisk )
	{
		hres = D3DXCreateTextureFromFileEx(	r3dRenderer->pd3ddev, "temp_tex.dds", m_ColorTex->GetWidth(), m_ColorTex->GetHeight(), 1, 0,
											TERRA2_COLORMODULATION_FORMAT, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &tex_dxt1 );
		remove( "temp_tex.dds" );
	}
	else
	{
		hres = D3DXCreateTextureFromFileInMemoryEx(	r3dRenderer->pd3ddev, fileInMem->GetBufferPointer(), fileInMem->GetBufferSize(), m_ColorTex->GetWidth(), m_ColorTex->GetHeight(), 1, 0,
													TERRA2_COLORMODULATION_FORMAT, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &tex_dxt1 );
	}

	if ( hres == D3D_OK )
	{
		int prevWidth = m_ColorTex->GetWidth();
		int prevHeight = m_ColorTex->GetHeight();

		// NOTE : current m_ColorTex is substituted by editor and owned by editor - hence it is not deleted.
		m_ColorTex = r3dRenderer->AllocateTexture();

		r3dD3DTextureTunnel texTunnel;

		texTunnel.Set( tex_dxt1 );

		m_ColorTex->Setup( prevWidth, prevHeight, 1, TERRA2_COLORMODULATION_FORMAT, 1, &texTunnel, false );

		SAFE_RELEASE( fileInMem );
	}
	else
	{
		if( hres == E_OUTOFMEMORY )
			r3dError( "Can't create color texture: out of SYSTEM memory" );
		else
		if( hres == D3DERR_OUTOFVIDEOMEMORY )
			r3dError( "Can't create color texture: out of VIDEO memory" );
		else
			D3D_V( hres );
	}
#endif
}

//------------------------------------------------------------------------

r3dTexture*
r3dTerrain2::GetHeightTexture() const
{
	return m_HeightTex;
}

//------------------------------------------------------------------------

r3dTexture*
r3dTerrain2::GetNormalTexture() const
{
	return m_NormalTex;
}

//------------------------------------------------------------------------

void
r3dTerrain2::InsertLayer( int idx, Floats* channel, const r3dTerrainLayer* layerDesc )
{
#if R3D_TERRAIN_V2_GRAPHICS
	r3d_assert( idx >= 0 && idx <= (int)m_Layers.Count() );

	ExtractMasksToChannels();

	int splatElemCount = m_SplatResolutionU * m_SplatResolutionV;

	if( channel )
	{
		m_LayerChannels.Insert( idx, *channel );
	}
	else
	{
		m_LayerChannels.Insert( idx, LayerChannel() );
		m_LayerChannels[ idx ].Resize( splatElemCount, 0 );
	}

	r3dTerrainLayer tlayer;

	m_Layers.Insert( idx, tlayer );

	m_LayerBaseBitMasks.Insert( idx, r3dBitMaskArray2D() );
	InitLayerBaseBitMask( idx );

	m_LayerBitMasksMipChains.Insert( idx, LayerBitMaskMipChain() );

	InitLayerBitMaskChain( idx );

	StoreChannelsInMasks();

	if( layerDesc )
	{
		m_Layers[ idx ] = *layerDesc;
	}

	InitLayers();

	RecalcLayerVars();

	UpdateDesc();

	RefreshAtlasTiles();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::DestroyLayer( int idx )
{
#if R3D_TERRAIN_V2_GRAPHICS
	r3d_assert( idx >= 0 && idx < (int)m_Layers.Count() );

	ExtractMasksToChannels();

	m_LayerChannels.Erase( idx );
	m_Layers.Erase( idx );
	m_LayerBaseBitMasks.Erase( idx );
	m_LayerBitMasksMipChains.Erase( idx );

	StoreChannelsInMasks();

	InitLayers();

	RecalcLayerVars();

	UpdateDesc();

	RefreshAtlasTiles();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::GetLayerChannel( Floats * oChannel, int idx )
{
#if R3D_TERRAIN_V2_GRAPHICS
	ExtractMasksToChannels();

	*oChannel = m_LayerChannels[ idx ];
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateDominantLayerData()
{
#if R3D_TERRAIN_V2_GRAPHICS
	ExtractMasksToChannels();
	DoUpdateDominantLayerData( m_SplatResolutionU, m_SplatResolutionV );
#endif
}

//------------------------------------------------------------------------

int
r3dTerrain2::GetNumDecals() const
{
	return (int)m_DecalRecords.Count();
}

//------------------------------------------------------------------------

const r3dTerrain2::DecalRecord&
r3dTerrain2::GetDecalRecord( int idx ) const
{
	return m_DecalRecords[ idx ];
}

//------------------------------------------------------------------------

void
r3dTerrain2::SetDecalRecord( int idx, const DecalRecord& record )
{
	int needUpdate = memcmp( &record, &m_DecalRecords[ idx ], sizeof record );
	m_DecalRecords[ idx ] = record;

	r3d_assert( record.TypeIdx >= 0 );

	if( needUpdate )
	{
		RECT rect;
		FillDecalRect( &rect, record );
		RefreshAtlasTiles( rect );
	}
}

//------------------------------------------------------------------------

void
r3dTerrain2::AddDecalRecord()
{
	m_DecalRecords.PushBack( DecalRecord() );
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateQualitySettings()
{
	const QualitySettings& qs = SelectQualitySettings();
	SetQualitySettings( qs, false, 0 );
}

//------------------------------------------------------------------------

void
r3dTerrain2::Resize( int newVertexCountX, int newVertexCountZ )
{
	// check for powers of 2
	r3d_assert( !( ~newVertexCountX + 1 + newVertexCountX ) );
	r3d_assert( !( ~newVertexCountZ + 1 + newVertexCountZ ) );
	r3d_assert( newVertexCountX >= 256 && newVertexCountX <= 4096 );
	r3d_assert( newVertexCountZ >= 256 && newVertexCountZ <= 4096 );

	r3d_assert( m_PhysicsHeightField );

	r3d_assert(_CrtCheckMemory());

	r3dTL::TArray< PxI16 > hfShrinkedSamples( m_VertexCountX * m_VertexCountZ );
	SaveHeightField( &hfShrinkedSamples );

	TCHAR tempFolder [MAX_PATH * 2];

	int res = GetTempPath (MAX_PATH, tempFolder);

	r3d_assert( res );

	strcat( tempFolder, "tempterra" );
	mkdir( tempFolder );

	DoSave( tempFolder, hfShrinkedSamples, newVertexCountX, newVertexCountZ );

	Destroy();
	Construct();

	DoLoad( tempFolder );

	Terrain2 = this;
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateLayersForMask( int idx )
{
	if( !m_LayerChannels.Count() )
	{
		// first timer - have to extract all
		ExtractMasksToChannels();
	}
	else
	{
		ExtractMaskToChannels( idx );
	}

	for( int i = 0, e = LAYERS_PER_MASK; i < e; i ++ )
	{
		int lidx = idx * LAYERS_PER_MASK + i;

		if( lidx >= (int)m_Layers.Count() )
			break;

		InitLayerBaseBitMask( lidx );
		InitLayerBitMaskChain( lidx );
	}
}

//------------------------------------------------------------------------

struct EnsureParams
{
	int width;
	int height;
	r3dD3DTextureTunnel* result;
	const char* path;
};

static void EnsureInMainThread( void* param )
{
	EnsureParams *eparams = (EnsureParams*)param;

	IDirect3DTexture9* convertedTex( NULL );

	D3D_V( D3DXCreateTextureFromFileEx( r3dRenderer->pd3ddev, eparams->path, eparams->width, eparams->height, 1, 0, TERRA2_LAYERMASK_FORMAT, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &convertedTex ) );

	eparams->result->Set( convertedTex );
}

void
r3dTerrain2::EnsureMaskFormat( int idx )
{
	r3dTexture* mask = m_Masks[ idx ];

	if( mask->GetD3DFormat() != TERRA2_LAYERMASK_FORMAT )
	{
		r3dOutToLog( "Terrain2: WARNING: layer mask %d has improper format - converting.\n", idx );

		char fname[ MAX_PATH * 2 ];
		strcpy( fname, mask->getFileLoc().FileName );

		int width = mask->GetWidth();
		int height = mask->GetHeight();

		r3dRenderer->DeleteTexture( mask );

		EnsureParams eparams;
		r3dD3DTextureTunnel tunnel;

		eparams.width	= width;
		eparams.height	= height;
		eparams.result	= &tunnel;
		eparams.path	= fname;

		ProcessCustomDeviceQueueItem( EnsureInMainThread, &eparams );

		mask = r3dRenderer->AllocateTexture();

		mask->Setup( width, height, 1, TERRA2_LAYERMASK_FORMAT, 1, &tunnel, false );
		mask->OverwriteFileLocation( fname );

		m_Masks[ idx ] = mask;
	}
}

//------------------------------------------------------------------------

const r3dString&
r3dTerrain2::GetNormalDetailTextureName()
{
	return m_DetailNormalTexSrc;
}

//------------------------------------------------------------------------

r3dTexture*
r3dTerrain2::GetNormalDetailTexture()
{
	return m_DetailNormalTex;
}

//------------------------------------------------------------------------

void
r3dTerrain2::SetNormalDetailTexture( const r3dString& Name )
{
	if( m_DetailNormalTexSrc != Name )
	{
		m_DetailNormalTexSrc = Name;

		if( m_DetailNormalTexSrc.Length() )
		{
			LoadDetailNormalTex();
		}
		else
		{
			if( m_DetailNormalTex )
			{
				r3dRenderer->DeleteTexture( m_DetailNormalTex );
				m_DetailNormalTex = NULL;
			}
		}

		UpdateNormals();
	}
}

//------------------------------------------------------------------------

void r3dTerrain2::UpdateNormals()
{

	Floats heights;

	heights.Resize( m_VertexCountX * m_VertexCountZ );

	Vectors temp0, temp1;

	SaveHeightField( &heights );

	UpdateNormals( heights, &temp0, &temp1, NULL );
}

//------------------------------------------------------------------------

void
r3dTerrain2::SwitchVFetchMode()
{
#if R3D_TERRAIN_V2_GRAPHICS
	m_AllowVFetch = !m_AllowVFetch;

	if( m_AllowVFetch )
	{
		if( m_HeightTex )
			r3dRenderer->DeleteTexture( m_HeightTex );

		m_HeightTex = r3dRenderer->AllocateTexture();

		m_HeightTex->Create( m_VertexCountX, m_VertexCountZ, D3DFMT_L16, 1 );

		UINT16* locked = (UINT16*)m_HeightTex->Lock( 1 );

		for( int i = 0, e = m_VertexCountX * m_VertexCountZ; i < e; i ++, locked ++ )
		{
			*locked = UINT16( (int)m_HeightArr[ i ] * 2 );
		}

		m_HeightTex->Unlock();
	}
	else
	{
		CreateDynaVertexBuffer();

		UINT16* locked = (UINT16*)m_HeightTex->Lock( 0 );

		m_HeightArr.Resize( m_VertexCountX * m_VertexCountZ );

		for( int i = 0, e = m_VertexCountX * m_VertexCountZ; i < e; i ++, locked ++ )
		{
			m_HeightArr[ i ] = *locked / 2;
		}

		m_HeightTex->Unlock();

		r3dRenderer->DeleteTexture( m_HeightTex );
		m_HeightTex = NULL;
	}
#endif
}

//------------------------------------------------------------------------
/*virtual*/

float
r3dTerrain2::GetHeight( const r3dPoint3D& pos )	/*OVERRIDE*/
{
	r3d_assert( m_PhysicsHeightField );

	float rx = pos.x / m_CellSize;
	float rz = pos.z / m_CellSize;

	PxReal x = R3D_MAX( R3D_MIN( rx, m_VertexCountX  - 1.5f ), 0.f );
	PxReal z = R3D_MAX( R3D_MIN( rz, m_VertexCountZ - 1.5f ), 0.f );
	return m_PhysicsHeightField->getHeight(x, z) * m_InvHFScale;
}

//------------------------------------------------------------------------
/*virtual*/

r3dPoint3D
r3dTerrain2::GetNormal( const r3dPoint3D& pos )	/*OVERRIDE*/
{
	r3dPoint3D v1,v2,v3;
	r3dVector a, b, Normal;

	v1 = pos + r3dPoint3D(0,0,m_CellSize);
	v2 = pos + r3dPoint3D(m_CellSize,0,-m_CellSize);
	v3 = pos + r3dPoint3D(-m_CellSize,0,-m_CellSize);

	v1.Y = GetHeight(v1);
	v2.Y = GetHeight(v2);
	v3.Y = GetHeight(v3);

	a      = v2 - v1;
	b      = v3 - v1;
	Normal = a.Cross(b);
	Normal.Normalize();
	if(R3D_ABS(Normal.X) < 0.001) Normal.X = 0.;
	if(R3D_ABS(Normal.Y) < 0.001) Normal.Y = 0.;
	if(R3D_ABS(Normal.Z) < 0.001) Normal.Z = 0.;

	return Normal;
}

//------------------------------------------------------------------------
/*virtual*/

void
r3dTerrain2::GetHeightRange( float* oMinHeight, float* oMaxHeight, r3dPoint2D start, r3dPoint2D end ) /*OVERRIDE*/
{
	const r3dTerrainDesc& desc = GetDesc();

	int	x0	= (int)( start.x / desc.CellSize );
	int	z0	= (int)( start.y / desc.CellSize );
	int	x1	= (int)( end.x / desc.CellSize );
	int	z1	= (int)( end.y / desc.CellSize );

	x0 = R3D_MAX( R3D_MIN( x0, (int)desc.CellCountX - 1 ), 0 );
	x1 = R3D_MAX( R3D_MIN( x1, (int)desc.CellCountX - 1 ), 0 );

	z0 = R3D_MAX( R3D_MIN( z0, (int)desc.CellCountZ - 1 ), 0 );
	z1 = R3D_MAX( R3D_MIN( z1, (int)desc.CellCountZ - 1 ), 0 );

	int xmi = R3D_MIN( x0, x1 );
	int xma = R3D_MAX( x0, x1 );

	int zmi = R3D_MIN( z0, z1 );
	int zma = R3D_MAX( z0, z1 );

	int iwidth = (int) desc.CellCountX;

	float minHeight = FLT_MAX;
	float maxHeight = -FLT_MAX;

	for( int j = z0; j <= z1; j ++ )
	{
		for( int i = x0; i <= x1; i ++ )
		{
			float h = GetHeight( i, j );
			minHeight = R3D_MIN( h, minHeight );
			maxHeight = R3D_MAX( h, maxHeight );
		}
	}

	*oMinHeight = minHeight;
	*oMaxHeight = maxHeight;
}

//------------------------------------------------------------------------
/*virtual*/

const MaterialType*
r3dTerrain2::GetMaterialType( const r3dPoint3D& pnt ) /*OVERRIDE*/
{
	if( !m_DominantLayerData.Count() )
	{
		return g_pMaterialTypes->GetDefaultMaterial();
	}

	int domLayerDataWidth = m_DominantLayerData.Width();
	int domLayerDataHeight = m_DominantLayerData.Height();

	int	X     = (int)( pnt.X / m_CellSize * domLayerDataWidth / m_SplatResolutionU );
	int	Z     = (int)( pnt.Z / m_CellSize * domLayerDataHeight / m_SplatResolutionV );

	X = R3D_MIN( R3D_MAX( X, 0 ), (int)domLayerDataWidth - 1 );
	Z = R3D_MIN( R3D_MAX( Z, 0 ), (int)domLayerDataHeight - 1 );

	int lidx = m_DominantLayerData[ Z ][ X ];

	if( lidx >= 0 && lidx < (int)m_MatTypeIdxes.Count() )
	{
		return g_pMaterialTypes->GetByIdx( m_MatTypeIdxes[ lidx ] );
	}
	else
	{
		return g_pMaterialTypes->GetDefaultMaterial();
	}

}

//------------------------------------------------------------------------
/*virtual*/

bool
r3dTerrain2::IsLoaded() /*OVERRIDE*/
{
	return !!m_IsLoaded;
}

//------------------------------------------------------------------------
/*virtual*/

bool
r3dTerrain2::PrepareOrthographicTerrainRender( int renderTargetWidth, int renderTargetHeight ) /*OVERRIDE*/
{
#if R3D_TERRAIN_V2_GRAPHICS

	// deallocate everything ( it'll get auto allocated before normal render again )
	RemoveAllocatedTiles();

	//------------------------------------------------------------------------

	int tileMip = 0;

	for( int v = m_QualitySettings.VertexTileDim; v < 64; v *= 2 )
	{
		tileMip ++;
	}

	tileMip = R3D_MIN( tileMip, m_NumActiveQualityLayers - 1 );

	int tileVertexCount = m_QualitySettings.VertexTileDim << tileMip;

	int tileCountX = m_VertexCountX / tileVertexCount;
	int tileCountZ = m_VertexCountZ / tileVertexCount;

	for( int z = 0, e = tileCountZ; z < e; z ++ )
	{
		for( int x = 0, e = tileCountX; x < e; x ++ )
		{
			AddToAllocatedTiles( x, z, tileMip, 0 );
		}
	}

	StartTileUpdating();

	{
		AllocatedTileArr& targetMipArr = m_AllocTileLodArray[ tileMip ];

		for( int i = 0, e = (int)targetMipArr.Count(); i < e; i ++ )
		{
			AllocatedTile& atile = targetMipArr[ i ];

			AllocateAtlasTile( &atile );
			UpdateTileInAtlas( &atile );
		}
	}

	StopTileUpdating();

	StartTileRoadUpdating();

	if( m_RoadInfoMipChain.Count() )
	{
		AllocatedTileArr& targetMipArr = m_AllocTileLodArray[ tileMip ];

		for( int i = 0, e = (int)targetMipArr.Count(); i < e; i ++ )
		{
			AllocatedTile& atile = targetMipArr[ i ];
			UpdateAtlasTileRoads( &atile );
		}
	}

	StopTileRoadUpdating();

#endif

	return true;
}

//------------------------------------------------------------------------
/*virtual*/

bool
r3dTerrain2::DrawOrthographicTerrain( const r3dCamera& Cam, bool UseZ ) /*OVERRIDE*/
{
#if R3D_TERRAIN_V2_GRAPHICS
	R3D_ENSURE_MAIN_THREAD();

	if( UseZ )
		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZW | R3D_BLEND_ZC );
	else
		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );

	D3D_V( r3dRenderer->pd3ddev->Clear( 0, NULL, D3DCLEAR_TARGET, 0xff000000, r3dRenderer->GetClearZValue(), 0 ) );

	// need white alpha or else our d3dxsave/d3dxload bezzle produces enterily black dxt1...
	D3D_V( r3dRenderer->pd3ddev->SetRenderState(	D3DRS_COLORWRITEENABLE,
													D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
													D3DCOLORWRITEENABLE_BLUE ) );



	StartTileRendering();

	for( int i = 0, e = (int) m_AllocTileLodArray.Count(); i < e; i ++ )
	{
		AllocatedTileArr& arr = m_AllocTileLodArray[ i ];

		for( int i = 0, e = arr.Count(); i < e; i ++ )
		{
			RenderTile(	&arr[ i ] );
		}
	}

	EndTileRendering();

	D3D_V( r3dRenderer->pd3ddev->SetRenderState(	D3DRS_COLORWRITEENABLE,
													D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |
													D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA ) );

	RemoveAllocatedTiles();

	FreeAtlas();
#endif

	return true;
}

//------------------------------------------------------------------------
/*virtual*/

static void ReloadTexture( r3dTexture* tex, int DownScale )
{
	if( tex )
	{
		char fname[ 1024 ];
		r3dscpy( fname, tex->getFileLoc().FileName );

		tex->Unload();
		tex->Load( fname, D3DFMT_UNKNOWN, DownScale );
	}
}

void
r3dTerrain2::ReloadTextures() /*OVERRIDE*/
{
	int DownScale = GetTerraTexDownScale();

	ReloadTexture( m_BaseLayer.DiffuseTex, DownScale );
	ReloadTexture( m_BaseLayer.NormalTex, DownScale );

	for( int i = 0, e = m_Layers.Count(); i < e; i ++ )
	{
		ReloadTexture( m_Layers[ i ].DiffuseTex, DownScale );
		ReloadTexture( m_Layers[ i ].NormalTex, DownScale );
	}

	UpdateQualitySettings();

	RefreshAtlasTiles();
}

//------------------------------------------------------------------------
/*virtual*/

void r3dTerrain2::GetNormalTangentBitangent(const r3dPoint3D &pos, r3dPoint3D *outNorm, r3dPoint3D *outTangent, r3dPoint3D *outBitangent)
{
	r3dPoint3D v1,v2,v3;
	r3dVector a, b, Normal;

	v1 = pos + r3dPoint3D(0,0,m_CellSize);
	v2 = pos + r3dPoint3D(m_CellSize,0,-m_CellSize);
	v3 = pos + r3dPoint3D(-m_CellSize,0,-m_CellSize);

	v1.Y = GetHeight(v1);
	v2.Y = GetHeight(v2);
	v3.Y = GetHeight(v3);

	a      = v2 - v1;
	b      = v3 - v1;
	Normal = a.Cross(b);
	Normal.Normalize();
	if(R3D_ABS(Normal.X) < 0.001) Normal.X = 0.;
	if(R3D_ABS(Normal.Y) < 0.001) Normal.Y = 0.;
	if(R3D_ABS(Normal.Z) < 0.001) Normal.Z = 0.;

	if (outNorm)
	{
		*outNorm = Normal;
	}

	if (outTangent)
	{
		*outTangent = a.NormalizeTo();
	}

	if (outBitangent)
	{
		*outBitangent = Normal.Cross(a).NormalizeTo();
	}
}

//------------------------------------------------------------------------
/*virtual*/

r3dTexture*
r3dTerrain2::GetDominantTexture( const r3dPoint3D &pos ) /*OVERRIDE*/
{
#if R3D_TERRAIN_V2_GRAPHICS
	const r3dTerrainDesc& desc = GetDesc();

	int x = int( pos.x / desc.CellSize ) * m_DominantLayerData.Width() / desc.CellCountX;
	int z = int( pos.z / desc.CellSize ) * m_DominantLayerData.Height() / desc.CellCountZ;

	x = R3D_MAX( R3D_MIN( x, (int)m_DominantLayerData.Width() - 1 ), 0 );
	z = R3D_MAX( R3D_MIN( z, (int)m_DominantLayerData.Height() - 1 ), 0 );

	int domiLayer = m_DominantLayerData[ z ][ x ];

	if( !domiLayer )
		return m_BaseLayer.DiffuseTex;
	else
	{
		if( domiLayer > 0 && domiLayer <= (int)m_Layers.Count() )
		{
			return m_Layers[ domiLayer - 1 ].DiffuseTex;
		}
	}
#endif

	return NULL;
}

//------------------------------------------------------------------------

void
r3dTerrain2::Construct()
{
	m_TileCountX				= 0;
	m_TileCountZ				= 0;
	m_AtlasTileCountPerSide		= 0;
	m_TempDiffuseRT				= NULL;
	m_TempNormalRT				= NULL;
	m_TileVertexBuffer			= NULL;
	m_TileIndexBuffer			= NULL;
	m_HeightTex					= NULL;
	m_NormalTex					= NULL;
	m_ColorTex					= NULL;
	m_LastDiffuseRT				= NULL;
	m_LastNormalRT				= NULL;
	m_DetailNormalTex			= NULL;
	m_UnitQuadVertexBuffer		= NULL;
	m_TerrainPosToSplatU		= 0.f;
	m_TerrainPosToSplatV		= 0.f;
	m_HeightScale				= 30.f;
	m_HeightOffset				= 0.f;
	m_TotalTerrainXLength		= 0.f;
	m_TotalTerrainZLength		= 0.f;
	m_NumActiveQualityLayers	= 0;
	m_TotalAllocatedTiles		= 0;
	m_MaxAllocatedTiles			= 0;
	m_4VertTileIndexOffset		= 0;
	m_DebugVisIndexOffset		= 0;
	m_TileUnitWorldDim			= 0.f;
	m_VertexCountX				= 0;
	m_VertexCountZ				= 0;
	m_SplatResolutionU			= 0;
	m_SplatResolutionV			= 0;
	m_PhysicsTerrain			= 0;
	m_PhysicsHeightField		= 0;
	m_HFScale					= 1.0f;
	m_InvHFScale				= 1.0f;
	m_HeightFieldDataCRC32		= 0;
	m_RoadTileUpdateZFar		= 0.f;
	m_IsLoaded					= 0;
	m_IsQualityUpdated			= 0;
	m_DynamicVertexBuffer		= 0;
	m_DynamicVertexBufferPtr	= 0;
	m_AllowVFetch				= 0;

	m_NormalDensity				= 1;

	m_BaseLayer.Name = "Base Layer";

#if R3D_TERRAIN_V2_GRAPHICS
	m_AllowVFetch = r3dRenderer->SupportsVertexTextureFetch;
#endif

}

//------------------------------------------------------------------------

void
r3dTerrain2::Destroy()
{
	if( m_PhysicsTerrain )
	{
		m_PhysicsTerrain->release();
		m_PhysicsHeightField->release();

		m_PhysicsTerrain		= NULL;
		m_PhysicsHeightField	= NULL;
	}

	if( Terrain == Terrain2 )
	{
		Terrain = NULL;
	}

	Terrain2 = NULL;
	Close();
}

//------------------------------------------------------------------------

void
r3dTerrain2::CreateDynaVertexBuffer()
{
	if( !m_DynamicVertexBuffer )
	{
		m_DynamicVertexBuffer = new r3dVertexBuffer( 128 * 1024, sizeof( r3dDynaTerraVertex), 0, true );
	}
}

//------------------------------------------------------------------------

void
r3dTerrain2::ExtractMaskToChannels( int midx )
{
	// new layer added?
	if( m_LayerChannels.Count() < m_Layers.Count() )
	{
		m_LayerChannels.Resize( m_Layers.Count() );
	}

	int c = midx * LAYERS_PER_MASK;

	ID3DXBuffer* fileInMem;

	r3dTexture* source = m_Masks[ midx ];

	D3D_V( D3DXSaveTextureToFileInMemory( &fileInMem, D3DXIFF_DDS, source->AsTex2D(), NULL ) );

	IDirect3DTexture9* r5g6b6;

	D3D_V( D3DXCreateTextureFromFileInMemoryEx(	r3dRenderer->pd3ddev, fileInMem->GetBufferPointer(), fileInMem->GetBufferSize(), source->GetWidth(), source->GetHeight(), 1, 0,
												D3DFMT_R5G6B5, D3DPOOL_SYSTEMMEM, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &r5g6b6 ) );


	D3DLOCKED_RECT lrect;
	D3D_V( r5g6b6->LockRect( 0, &lrect, NULL, D3DLOCK_READONLY ) );

	int have1 = c + 1 < (int)m_Layers.Count();
	int have2 = c + 2 < (int)m_Layers.Count();

	struct Channeled16
	{
		UINT16 b : 5;
		UINT16 g : 6;
		UINT16 r : 5;
	} * locked = ( Channeled16* ) lrect.pBits;

	int totalCount = source->GetWidth() * source->GetHeight();

	m_LayerChannels[ c + 0 ].Resize( totalCount );

	if( have1 ) m_LayerChannels[ c + 1 ].Resize( totalCount );
	if( have2 ) m_LayerChannels[ c + 2 ].Resize( totalCount );

	for( int i = 0, e = totalCount; i < e; i ++, locked ++ )
	{
		// add a little extra, bcoz continuous dxt compression seems to fade intensity away.

		m_LayerChannels[ c + 0 ][ i ] = 255.5f * locked->r / 31.f;

		if( have1 )	m_LayerChannels[ c + 1 ][ i ] = 255.5f * locked->g / 63.f;
		if( have2 )	m_LayerChannels[ c + 2 ][ i ] = 255.5f * locked->b / 31.f;
	}

	D3D_V( r5g6b6->UnlockRect( 0 ) );

	SAFE_RELEASE( r5g6b6 );
	SAFE_RELEASE( fileInMem );
}

//------------------------------------------------------------------------

void
r3dTerrain2::ExtractMasksToChannels()
{
#if R3D_TERRAIN_V2_GRAPHICS
	m_LayerChannels.Resize( m_Layers.Count() );

	for( int i = 0, e = m_Masks.Count() ; i < e; i ++ )
	{
		ExtractMaskToChannels( i );
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::StoreChannelsInMasks()
{
#if R3D_TERRAIN_V2_GRAPHICS
	for( int i = 0, e = (int)m_Masks.Count(); i < e; i ++ )
	{
		r3dRenderer->DeleteTexture( m_Masks[ i ] );
	}

	m_Masks.Resize( m_LayerChannels.Count() / LAYERS_PER_MASK + ( m_LayerChannels.Count() % LAYERS_PER_MASK ? 1 : 0 ) );

	int maskTexWidth = m_SplatResolutionU;
	int maskTexHeight = m_SplatResolutionV;

	IDirect3DTexture9* r5g6b5;
	D3D_V( r3dRenderer->pd3ddev->CreateTexture( maskTexWidth, maskTexHeight, 1, 0, D3DFMT_R5G6B5, D3DPOOL_SYSTEMMEM, &r5g6b5, NULL ) );

	for( int i = 0, e = m_Masks.Count(), c = 0; i < e; i ++, c += LAYERS_PER_MASK )
	{
		D3DLOCKED_RECT lrect;
		D3D_V( r5g6b5->LockRect( 0, &lrect, NULL, D3DLOCK_READONLY ) );

		struct Channeled16
		{
			UINT16 b : 5;
			UINT16 g : 6;
			UINT16 r : 5;
		} * locked = ( Channeled16* ) lrect.pBits;

		int have1 = c + 1 < (int)m_LayerChannels.Count();
		int have2 = c + 2 < (int)m_LayerChannels.Count();

		{
			for( int i = 0, e = maskTexWidth * maskTexHeight; i < e; i ++, locked ++ )
			{
				locked->r = R3D_MIN( R3D_MAX( int( m_LayerChannels[ c + 0 ][ i ] * 31.f / 255.f ), 0 ), 31 );
				if( have1 )
					locked->g = R3D_MIN( R3D_MAX( int( m_LayerChannels[ c + 1 ][ i ] * 63.f / 255.f ), 0 ), 63 );
				else
					locked->g = 0;

				if( have2 )
					locked->b = R3D_MIN( R3D_MAX( int( m_LayerChannels[ c + 2 ][ i ] * 31.f / 255.f ), 0 ), 31 );
				else
					locked->b = 0;

				if( locked->r < 2 ) locked->r = 0;
				if( locked->g < 2 ) locked->g = 0;
				if( locked->b < 2 ) locked->b = 0;
			}
		}

		D3D_V( r5g6b5->UnlockRect( 0 ) );

		ID3DXBuffer* fileInMem( NULL );

		D3D_V( D3DXSaveTextureToFileInMemory( &fileInMem, D3DXIFF_DDS, r5g6b5, NULL ) );

		IDirect3DTexture9* dxt1;

		D3D_V( D3DXCreateTextureFromFileInMemoryEx(		r3dRenderer->pd3ddev, fileInMem->GetBufferPointer(), fileInMem->GetBufferSize(),
														maskTexWidth, maskTexHeight, 1, 0, TERRA2_LAYERMASK_FORMAT, D3DPOOL_MANAGED,
														D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &dxt1 ) );

		r3dTexture* targ = r3dRenderer->AllocateTexture();

		r3dD3DTextureTunnel tunnel;
		tunnel.Set( dxt1 );

		targ->Setup( maskTexWidth, maskTexHeight, 1, TERRA2_LAYERMASK_FORMAT, 1, &tunnel, false );

		m_Masks[ i ] = targ;

		SAFE_RELEASE( fileInMem );
	}

	SAFE_RELEASE( r5g6b5 );
#endif
}

//------------------------------------------------------------------------

struct CreateBlankTexParams
{
	r3dD3DTextureTunnel* Tunnel;
	int Width;
	int Height;
	D3DFORMAT TargetFmt;
};

static void CreateBlankTextureInMainThread( void* param )
{
	CreateBlankTexParams * params = static_cast< CreateBlankTexParams* >( param );

	IDirect3DTexture9* tex;
	D3D_V( r3dRenderer->pd3ddev->CreateTexture( params->Width, params->Height, 1, 0, D3DFMT_R5G6B5, D3DPOOL_SYSTEMMEM, &tex, NULL ) );

	D3DLOCKED_RECT lrect;
	D3D_V( tex->LockRect( 0, &lrect, NULL, 0 ) );

	memset( lrect.pBits, 0, lrect.Pitch * params->Height );

	D3D_V( tex->UnlockRect( 0 ) );

	ID3DXBuffer* savedTexture;

	D3D_V( D3DXSaveTextureToFileInMemory( &savedTexture, D3DXIFF_DDS, tex, NULL ) );

	SAFE_RELEASE( tex );

	D3D_V( D3DXCreateTextureFromFileInMemoryEx( r3dRenderer->pd3ddev, savedTexture->GetBufferPointer(), savedTexture->GetBufferSize(), params->Width, params->Height, 1, 0, TERRA2_LAYERMASK_FORMAT, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL,  NULL, &tex ) );

	params->Tunnel->Set( tex );
}

int
r3dTerrain2::DoLoad( const char* dir )
{
	const char* dirName = dir;

	char tempStr[ 1024 ];

	sprintf( tempStr, FNAME_TERRAIN2_INI, dirName );

	Script_c script;

	if ( ! script.OpenFile( tempStr ) )
		return 0;

	char buffer[ MAX_PATH ];

	while ( ! script.EndOfFile() )
	{
		script.GetToken( buffer );
		if ( ! *buffer )
			break;

		if ( ! strcmp( buffer, "vert_count_x:" ) )
		{
			m_VertexCountX = script.GetInt();
		}
		else
		if( ! strcmp( buffer, "vert_count_z:" ) )
		{
			m_VertexCountZ = script.GetInt();
		}
		else
		if( ! strcmp( buffer, "splat_res_u:" ) )
		{
			m_SplatResolutionU = script.GetInt();
		}
		else
		if( ! strcmp( buffer, "splat_res_v:" ) )
		{
			m_SplatResolutionV = script.GetInt();
		}
		else
		if( ! strcmp( buffer, "tile_unit_size:" ) )
		{
			m_TileUnitWorldDim = script.GetFloat();
		}
		else
		if( ! strcmp( buffer, "height_offset:" ) )
		{
			m_HeightOffset = script.GetFloat();
		}
		else
		if( ! strcmp( buffer, "height_scale:" ) )
		{
			m_HeightScale = script.GetFloat();
		}
		else
		if( ! strcmp( buffer, "atlas_tile_dim:" ) )
		{
			int atlasDim = script.GetInt();

			m_BaseQualitySettings[ QS_HIGH ].AtlasTileDim = atlasDim;
			m_BaseQualitySettings[ QS_MED ].AtlasTileDim = atlasDim;
			m_BaseQualitySettings[ QS_LOW ].AtlasTileDim = atlasDim;
		}
		else
		if( ! strcmp( buffer, "atlas_tile_dim2:" ) )
		{
			m_BaseQualitySettings[ QS_HIGH ].AtlasTileDim = script.GetInt();
		}
		else
		if( ! strcmp( buffer, "atlas_tile_dim1:" ) )
		{
			m_BaseQualitySettings[ QS_MED ].AtlasTileDim = script.GetInt();
		}
		else
		if( ! strcmp( buffer, "atlas_tile_dim0:" ) )
		{
			m_BaseQualitySettings[ QS_LOW ].AtlasTileDim = script.GetInt();
		}
		else
		if( ! strcmp( buffer, "vertex_tile_dim:" ) )
		{
			int vertexTileDim = script.GetInt();

			m_BaseQualitySettings[ QS_HIGH ].VertexTileDim = vertexTileDim;
			m_BaseQualitySettings[ QS_MED ].VertexTileDim = vertexTileDim;
			m_BaseQualitySettings[ QS_LOW ].VertexTileDim = vertexTileDim;
		}
		else
		if( ! strcmp( buffer, "vertex_tile_dim2:" ) )
		{
			m_BaseQualitySettings[ QS_HIGH ].VertexTileDim = script.GetInt();
		}
		else
		if( ! strcmp( buffer, "vertex_tile_dim1:" ) )
		{
			m_BaseQualitySettings[ QS_MED ].VertexTileDim = script.GetInt();
		}
		else
		if( ! strcmp( buffer, "vertex_tile_dim0:" ) )
		{
			m_BaseQualitySettings[ QS_LOW ].VertexTileDim = script.GetInt();
		}
		else
		if(  ! strcmp( buffer, "base_layer" ) )
		{
			LoadLayerFromScript( &script, &m_BaseLayer );
		}
		else
		if(  ! strcmp( buffer, "layer" ) )
		{
			m_Layers.Resize( m_Layers.Count() + 1 );
			LoadLayerFromScript( &script, &m_Layers[ m_Layers.Count() - 1 ] );
		}
		else
		if( ! strcmp( buffer, "vertex_density:" ) )
		{
			int vertDensity = script.GetInt();

			m_BaseQualitySettings[ QS_HIGH ].VertexDensity = vertDensity;
			m_BaseQualitySettings[ QS_MED ].VertexDensity = vertDensity;
			m_BaseQualitySettings[ QS_LOW ].VertexDensity = vertDensity;
		}
		else
		if( ! strcmp( buffer, "vertex_density2:" ) )
		{
			m_BaseQualitySettings[ QS_HIGH ].VertexDensity = script.GetInt();
		}
		else
		if( ! strcmp( buffer, "vertex_density1:" ) )
		{
			m_BaseQualitySettings[ QS_MED ].VertexDensity = script.GetInt();
		}
		else
		if( ! strcmp( buffer, "vertex_density0:" ) )
		{
			m_BaseQualitySettings[ QS_LOW ].VertexDensity = script.GetInt();
		}
		else
		if( !strcmp( buffer, "normal_density:" ) )
		{
			m_NormalDensity = script.GetInt();
		}
		else
		if( !strcmp( buffer, "normal_det_mix:" ) )
		{
			m_Settings.DetailNormalMix = script.GetFloat();
		}
		else
		if( !strcmp( buffer, "normal_detail_tex:") )
		{
			char buf[ 1024 ];
			script.GetString( buf, 1023 );

			m_DetailNormalTexSrc = buf;
		}
		else
		{
			char tstr[ 256 ];

			int found = 0;

			for( int tq = QS_LOW, tqe = QS_COUNT; tq < tqe && !found; tq ++ )
			{
				for( int i = 0, e = NUM_QUALITY_LAYERS; i < e; i ++ )
				{
					sprintf( tstr, "lod%d_%d:", i, tq );

					if( !strcmp( buffer, tstr ) )
					{
						m_BaseQualitySettings[ tq ].TileCounts[ i ] = script.GetInt();
						found = 1;
						break;
					}
				}
			}

			if( !found )
			{
				for( int i = 0, e = NUM_QUALITY_LAYERS; i < e; i ++ )
				{
					sprintf( tstr, "lod%d:", i );

					if( !strcmp( buffer, tstr ) )
					{
						int lod = script.GetInt();
						m_BaseQualitySettings[ QS_HIGH ].TileCounts[ i ]  = lod;
						m_BaseQualitySettings[ QS_MED ].TileCounts[ i ]  = lod;
						m_BaseQualitySettings[ QS_LOW ].TileCounts[ i ]  = lod;
						found = 1;
						break;
					}
				}
			}

			if( !found )
			{
				for( int tq = QS_LOW, tqe = QS_COUNT; tq < tqe && !found; tq ++ )
				{
					for( int i = 0, e = m_BaseQualitySettings[ tq ].TileVertexDensitySteps.COUNT; i < e; i ++ )
					{
						sprintf( tstr, "vertex_density_step%d_%d:", i, tq );

						if( !strcmp( buffer, tstr ) )
						{
							m_BaseQualitySettings[ tq ].TileVertexDensitySteps[ i ] = script.GetInt();
							found = 1;
							break;
						}
					}
				}
			}

			if( !found )
			{
				for( int i = 0, e = m_BaseQualitySettings[ QS_LOW ].TileVertexDensitySteps.COUNT; i < e; i ++ )
				{
					sprintf( tstr, "vertex_density_step%d:", i );

					if( !strcmp( buffer, tstr ) )
					{
						int tileVertDensity = script.GetInt();

						m_BaseQualitySettings[ QS_HIGH ].TileVertexDensitySteps[ i ] = tileVertDensity;
						m_BaseQualitySettings[ QS_MED ].TileVertexDensitySteps[ i ] = tileVertDensity;
						m_BaseQualitySettings[ QS_LOW ].TileVertexDensitySteps[ i ] = tileVertDensity;

						found = 1;
						break;
					}
				}
			}
		}
	}

	if( !m_SplatResolutionU )
	{
		m_SplatResolutionU = m_VertexCountX;
		m_SplatResolutionV = m_VertexCountZ;
	}
	else
	{
		m_SplatResolutionU = R3D_MIN( m_SplatResolutionU, m_VertexCountX );
		m_SplatResolutionV = R3D_MIN( m_SplatResolutionV, m_VertexCountZ );
	}


	m_QualitySettings = SelectQualitySettings();

	RecalcVars();
	RecalcLayerVars();

	r3dOutToLog("TERRAIN2: Finished reading script file\n");
	script.CloseFile();

	if( !LoadBin( dirName ) )
		return 0;

	Init();

#if R3D_TERRAIN_V2_GRAPHICS
	int desiredMaskCount = m_Layers.Count() / 3 + ( m_Layers.Count() % 3 ? 1 : 0 );

	char fullPath[ 1024 ];

	for( int i = 0, e = desiredMaskCount; i < e; i ++ )
	{
		PrintFullSplatPath( dirName, i, fullPath );

		r3dTexture* splat = r3dRenderer->LoadTexture( fullPath );

		if( splat->IsMissing() )
		{
			r3dRenderer->DeleteTexture( splat );
			splat = NULL;

			splat = r3dRenderer->AllocateTexture();

			CreateBlankTexParams params;

			r3dD3DTextureTunnel tunnel;

			params.Height		= m_SplatResolutionV;
			params.Width		= m_SplatResolutionU;
			params.Tunnel		= &tunnel;
			params.TargetFmt	= TERRA2_LAYERMASK_FORMAT;

			ProcessCustomDeviceQueueItem( CreateBlankTextureInMainThread, &params );

			splat->Setup( m_SplatResolutionU, m_SplatResolutionV, 1, TERRA2_LAYERMASK_FORMAT, 1, &tunnel, false );
			splat->OverwriteFileLocation( fullPath );
		}

		m_Masks.PushBack( splat );

		EnsureMaskFormat( i );
	}

	InitLayers();

	PrintFullColorPath( dirName, fullPath );

	m_ColorTex = r3dRenderer->LoadTexture( fullPath );

	PrintFullNormalPath( dirName, fullPath );

	if( !m_NormalTex )
	{
		m_NormalTex = r3dRenderer->LoadTexture( fullPath );
		ConformNormalTex();
	}

	if( g_bEditMode && !m_DetailNormalTex )
	{
		LoadDetailNormalTex();
	}

#endif

	UpdateDesc();

	m_IsQualityUpdated = 0;
	m_IsLoaded = 1;

	return 1;
}

//------------------------------------------------------------------------

static uint32_t GetHeightFieldDataSize( const PxHeightFieldDesc& desc )
{
	return desc.nbColumns * desc.nbRows * desc.samples.stride;
}

static void PrintSplatLocation( char (& buf ) [ 1024 ], int idx )
{
	_snprintf( buf, 1024, "Terrain2\\Mat-Splat%d.dds", idx );
}

static void PrintFullSplatPath( const char* levelDir, int i, char (& buf ) [ 1024 ] )
{
	char tempStr[ 1024 ];
	PrintSplatLocation( tempStr, i );

	r3dscpy( buf, levelDir );
	strcat( buf, "\\" );
	strcat( buf, tempStr );
}

static void PrintFullNormalPath( const char* levelDir, char (& buf ) [ 1024 ] )
{
	char *tempStr= "Terrain2\\Normal.dds";

	r3dscpy( buf, levelDir );
	strcat( buf, "\\" );
	strcat( buf, tempStr );
}

static void PrintFullColorPath( const char* levelDir, char (& buf ) [ 1024 ] )
{
	char *tempStr= "Terrain2\\Color.dds";

	r3dscpy( buf, levelDir );
	strcat( buf, "\\" );
	strcat( buf, tempStr );
}

static void SaveScaledTexture( const char* fullPath, r3dTexture* srcTex, int newWidth, int newHeight  )
{
	ID3DXBuffer* targetBuf( NULL );
	D3D_V( D3DXSaveTextureToFileInMemory( &targetBuf, D3DXIFF_DDS, srcTex->AsTex2D(), NULL ) );

	IDirect3DTexture9* sysmemRescaledTex( NULL );
	D3D_V( D3DXCreateTextureFromFileInMemoryEx( r3dRenderer->pd3ddev, targetBuf->GetBufferPointer(), targetBuf->GetBufferSize(), newWidth, newHeight, 1, 0, srcTex->GetD3DFormat(), D3DPOOL_SYSTEMMEM, D3DX_FILTER_TRIANGLE, D3DX_FILTER_NONE, 0, NULL, NULL, &sysmemRescaledTex ) );

	D3DXSaveTextureToFile( fullPath, D3DXIFF_DDS, sysmemRescaledTex, NULL );

	SAFE_RELEASE( sysmemRescaledTex );

	SAFE_RELEASE( targetBuf );
}

int
r3dTerrain2::DoSave( const char* targetDir, const Shorts& hfShrinkedSamples, int rescaleVertexCountX, int rescaleVertexCountZ )
{
#if R3D_TERRAIN_V2_GRAPHICS

	struct SaveRestoreCurrentQS
	{
		SaveRestoreCurrentQS( r3dTerrain2 * a_father )
		{
			father = a_father;

			savedQS = father->m_QualitySettings;
			father->RecalcVars();
		}

		~SaveRestoreCurrentQS()
		{
			father->m_QualitySettings = savedQS;
			father->RecalcVars();
		}

		QualitySettings savedQS;
		r3dTerrain2 * father;

	} saveRestoreCurrentQS ( this ); saveRestoreCurrentQS;

	const QualitySettings& saveQS = m_BaseQualitySettings[ QS_HIGH ];

	OptimizeLayerMasks();
	// relies on OptimizeLayerMasks to extract all texture channel data
	DoUpdateDominantLayerData( m_SplatResolutionU, m_SplatResolutionV );

	const char * fileName = targetDir;

	{
		char terraDir[ 1024 ];
		strcpy( terraDir, fileName );
		strcat( terraDir, "\\Terrain2" );
		mkdir( terraDir );
	}

	SaveBin( fileName, hfShrinkedSamples, rescaleVertexCountX, rescaleVertexCountZ );

	FILE* hFile = fopen_for_write( Va( FNAME_TERRAIN2_INI, fileName ), "wt");
	assert( hFile );
	if ( !hFile )
		return 0;

	int rescaleSplatResolutionU = R3D_MIN( m_SplatResolutionU, rescaleVertexCountX );
	int rescaleSplatResolutionV = R3D_MIN( m_SplatResolutionV, rescaleVertexCountZ );

	float scaleFactor = float( m_VertexCountX ) / rescaleVertexCountX ;

	fprintf( hFile, "vert_count_x:\t\t%d\n", rescaleVertexCountX );
	fprintf( hFile, "vert_count_z:\t\t%d\n", rescaleVertexCountZ );
	fprintf( hFile, "splat_res_u:\t\t%d\n", rescaleSplatResolutionU );
	fprintf( hFile, "splat_res_v:\t\t%d\n", rescaleSplatResolutionV );
	fprintf( hFile, "tile_unit_size:\t%f\n", m_TileUnitWorldDim * scaleFactor );
	fprintf( hFile, "height_offset:\t%f\n", m_HeightOffset );
	fprintf( hFile, "height_scale:\t%f\n", m_HeightScale );
	fprintf( hFile, "normal_density:\t%d\n", m_NormalDensity );
	fprintf( hFile, "normal_det_mix:\t%f\n", m_Settings.DetailNormalMix );

	fprintf( hFile, "normal_detail_tex:\t\"%s\"\n", m_DetailNormalTexSrc.Length() ? m_DetailNormalTexSrc.c_str() : "" );

	for( int tq = QS_LOW, tqe = QS_COUNT; tq < tqe; tq ++ )
	{
		for( int i = 0, e = m_NumActiveQualityLayers; i < e; i ++ )
		{
			fprintf( hFile, "lod%d_%d:\t\t%d\n", i, tq, m_BaseQualitySettings[ tq ].TileCounts[ i ] );
		}

		for( int i = 0, e = saveQS.TileVertexDensitySteps.COUNT; i < e; i ++ )
		{
			fprintf( hFile, "vertex_density_step%d_%d:\t\t%d\n", i, tq, m_BaseQualitySettings[ tq ].TileVertexDensitySteps[ i ] );
		}

		fprintf( hFile, "vertex_density%d:\t\t%d\n", tq, m_BaseQualitySettings[ tq ].VertexDensity );

		fprintf( hFile, "atlas_tile_dim%d:\t\t%d\n", tq, m_BaseQualitySettings[ tq ].AtlasTileDim );
		fprintf( hFile, "vertex_tile_dim%d:\t%d\n", tq, m_BaseQualitySettings[ tq ].VertexTileDim );
	}

	fprintf( hFile, "\nbase_layer\n" );
	SaveLayerToScript( hFile, m_BaseLayer, "" );

	for( int i = 0, e = (int)m_Layers.Count(); i < e; i ++ )
	{
		fprintf( hFile, "layer\n" );
		SaveLayerToScript( hFile, m_Layers[ i ], "" );
	}

	fclose( hFile );

	char fullPath[ 1024 ];

	for( int i = 0, e = m_Masks.Count(); i < e; i ++ )
	{
		r3dTexture* splatTex = m_Masks[ i ];

		PrintFullSplatPath( fileName, i, fullPath );

		splatTex->OverwriteFileLocation( fullPath );

		if( m_SplatResolutionU == rescaleSplatResolutionU
				&&
			m_SplatResolutionV == rescaleSplatResolutionV )
		{
			splatTex->Save( splatTex->getFileLoc().FileName );
		}
		else
		{
			SaveScaledTexture( fullPath, splatTex, rescaleSplatResolutionU, rescaleSplatResolutionV );
		}
	}

	PrintFullColorPath( fileName, fullPath );

	if( m_SplatResolutionU == rescaleSplatResolutionU
			&&
		m_SplatResolutionV == rescaleSplatResolutionV )
	{
		m_ColorTex->Save( fullPath );
	}
	else
	{
		SaveScaledTexture( fullPath, m_ColorTex, rescaleSplatResolutionU, rescaleSplatResolutionV );
	}

	{
		PrintFullNormalPath( fileName, fullPath );

		m_NormalTex->Save( fullPath );
		m_NormalTex->OverwriteFileLocation( fullPath );
	}

#endif

	return 1;
}

static void RescaleThroughTexture( r3dTerrain2::Shorts* dest, const r3dTerrain2::Shorts& src, int srcVertexCountX, int srcVertexCountZ, int destVertexCountX, int destVertexCountZ )
{
	r3d_assert( src.Count() == srcVertexCountX * srcVertexCountZ );

	dest->Resize( destVertexCountX * destVertexCountZ );

	IDirect3DTexture9* r32ftex;
	D3D_V( r3dRenderer->pd3ddev->CreateTexture( srcVertexCountX, srcVertexCountZ, 1, 0, D3DFMT_R32F, D3DPOOL_SYSTEMMEM, &r32ftex, NULL ) );

	D3DLOCKED_RECT lrect;
	D3D_V( r32ftex->LockRect( 0, &lrect, NULL, 0 ) );

	for( int z = 0, e = srcVertexCountZ; z < e; z ++ )
	{
		for( int x = 0, e = srcVertexCountX; x < e; x ++ )
		{
			((float*)((char*)lrect.pBits + z * lrect.Pitch))[ x ] = src[ x + z * srcVertexCountX ];
		}
	}

	D3D_V( r32ftex->UnlockRect( 0 ) );

	ID3DXBuffer* targetBuffer;
	D3D_V( D3DXSaveTextureToFileInMemory( &targetBuffer, D3DXIFF_DDS, r32ftex, NULL ) );

	SAFE_RELEASE( r32ftex );

	D3DXCreateTextureFromFileInMemoryEx(	r3dRenderer->pd3ddev, targetBuffer->GetBufferPointer(), targetBuffer->GetBufferSize(),
											destVertexCountX, destVertexCountZ,
											1, 0, D3DFMT_R32F, D3DPOOL_SYSTEMMEM,
											D3DX_FILTER_TRIANGLE, D3DX_FILTER_NONE, 0, NULL, NULL, &r32ftex );

	SAFE_RELEASE( targetBuffer );

	D3D_V( r32ftex->LockRect( 0, &lrect, NULL, 0 ) );

	for( int z = 0, e = destVertexCountZ; z < e; z ++ )
	{
		for( int x = 0, e = destVertexCountX; x < e; x ++ )
		{
			(*dest)[ x + z * destVertexCountX ] = (short)( R3D_MIN( R3D_MAX( ((float*)((char*)lrect.pBits + z * lrect.Pitch))[ x ], -32768.f ), 32767.f ) );
		}
	}

	D3D_V( r32ftex->UnlockRect( 0 ) );

	SAFE_RELEASE( r32ftex );
}

//------------------------------------------------------------------------

static void ScaleBitMask( r3dBitMaskArray2D* destMask, const r3dBitMaskArray2D& srcMask, int srcVertexCountX, int srcVertexCountZ, int destVertexCountX, int destVertexCountZ )
{
	destMask->Resize( destVertexCountX, destVertexCountZ );
	destMask->ZeroAll();

	if( srcVertexCountX >= destVertexCountX )
	{
		r3d_assert( srcVertexCountZ >= destVertexCountZ );

		int scaleZ = srcVertexCountZ / destVertexCountZ;
		int scaleX = srcVertexCountX / destVertexCountX;

		for( int z = 0, e = destVertexCountZ; z < e; z ++ )
		{
			for( int x = 0, e = destVertexCountX; x < e; x ++ )
			{
				bool set = false;
				for( int sz = 0, e = scaleZ; sz < e; sz ++ )
				{
					for( int sx = 0, e = scaleX; sx < e; sx ++ )
					{
						if( srcMask.Get( x * scaleX + sx, z * scaleZ + sz ) )
						{
							set = true;
							break;
						}
					}
				}

				destMask->Set( x, z, set );
			}
		}
	}
	else
	{
		r3d_assert( destVertexCountZ >= srcVertexCountZ );

		int scaleZ = destVertexCountZ / srcVertexCountZ;
		int scaleX = destVertexCountX / srcVertexCountX;

		for( int z = 0, e = srcVertexCountZ; z < e; z ++ )
		{
			for( int x = 0, e = srcVertexCountX; x < e; x ++ )
			{
				if( !srcMask.Get( x, z ) )
					continue;

				for( int sz = 0, e = scaleZ; sz < e; sz ++ )
				{
					for( int sx = 0, e = scaleX; sx < e; sx ++ )
					{
						destMask->Set( x * scaleX + sx, z * scaleZ + sz, true );
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------------

int
r3dTerrain2::SaveBin( const char* dirName, const Shorts& hfShrinkedSamples, int rescaleVertexCountX, int rescaleVertexCountZ )
{
#if R3D_TERRAIN_V2_GRAPHICS
	FILE * hFile;

	r3d_assert(_CrtCheckMemory());

	hFile = fopen_for_write( Va( FNAME_TERRAIN2_BIN, dirName ), "wb");
	r3d_assert( hFile );
	if ( ! hFile )
		return 0;

	uint32_t dwSignature = TERRAIN2_SIGNATURE;
	uint32_t dwVersion = TERRAIN2_VERSION;

	fwrite( &dwSignature, sizeof( dwSignature ), 1, hFile );
	fwrite( &dwVersion, sizeof( dwVersion ), 1, hFile );

	const Shorts* saveSource = &hfShrinkedSamples;

	Shorts hfRescaledSamples;

	if( rescaleVertexCountX != m_VertexCountX
			||
		rescaleVertexCountZ != m_VertexCountZ )
	{
		RescaleThroughTexture( &hfRescaledSamples, hfShrinkedSamples, m_VertexCountX, m_VertexCountZ, rescaleVertexCountX, rescaleVertexCountZ );

		saveSource = &hfRescaledSamples;
	}

	uint32_t finalSampleCount = saveSource->Count();

	fwrite( &finalSampleCount, sizeof finalSampleCount, 1, hFile );

	fwrite( &(*saveSource)[0], finalSampleCount * sizeof (*saveSource)[0], 1, hFile );
	m_HeightFieldDataCRC32 = r3dCRC32((const BYTE *)&(*saveSource)[0], saveSource->Count() * sizeof( (*saveSource)[0] ));

	int tileLayersCount = m_LayerBaseBitMasks.Count();
	fwrite( &tileLayersCount, sizeof(tileLayersCount), 1, hFile );

	for( int i = 0, e = (int)m_LayerBaseBitMasks.Count(); i < e; i ++ )
	{
		const r3dBitMaskArray2D* srcBits = &m_LayerBaseBitMasks[ i ];

		r3dBitMaskArray2D scaleDownBits;

		if( rescaleVertexCountX != m_VertexCountX
				||
			rescaleVertexCountZ != m_VertexCountZ )
		{
			ScaleBitMask( &scaleDownBits, *srcBits, m_VertexCountX, m_VertexCountZ, rescaleVertexCountX, rescaleVertexCountZ );
			srcBits = &scaleDownBits;
		}

		int one = srcBits->GetDataSize();
		fwrite( &one, sizeof one, 1, hFile );
		fwrite( srcBits->GetDataPtr(), srcBits->GetDataSize(), 1, hFile );
	}

	int width_height[ 2 ];

	width_height[ 0 ] = m_DominantLayerData.Width();
	width_height[ 1 ] = m_DominantLayerData.Height();

	if( rescaleVertexCountX != m_VertexCountX
			||
		rescaleVertexCountZ != m_VertexCountZ )
	{
		// dominant layer data is gameplay only
		// scaled terrain save may be temporary save only
		// dominant layer will be recalculated properly during unscaled save
		// hence we are fine with saving here erroneous data (unscaled data with scaled dimensions)
		width_height[ 0 ] = R3D_MIN( m_SplatResolutionU, rescaleVertexCountX );
		width_height[ 1 ] = R3D_MIN( m_SplatResolutionV, rescaleVertexCountZ );
	}

	fwrite( width_height, sizeof width_height, 1, hFile );
	fwrite( m_DominantLayerData.GetDataPtr(), width_height[ 0 ] * width_height[ 1 ] * sizeof m_DominantLayerData[ 0 ][ 0 ], 1, hFile );

	fclose(hFile);
#endif

	return 1;
}

//------------------------------------------------------------------------

void
r3dTerrain2::ExpendBaseBitMasksFrom101()
{
	int oldBaseTileCountX;
	int oldBaseTileCountZ;

	GetBaseTileCounts_101( &oldBaseTileCountX, &oldBaseTileCountZ );

	int baseTileCountX;
	int baseTileCountZ;

	GetBaseTileCounts( &baseTileCountX, &baseTileCountZ );

	int scaleX = baseTileCountX / oldBaseTileCountX;
	int scaleZ = baseTileCountZ / oldBaseTileCountZ;

	for( int i = 0, e = (int)m_LayerBaseBitMasks.Count(); i < e; i ++ )
	{
		r3dBitMaskArray2D oldBits = m_LayerBaseBitMasks[ i ];
		r3dBitMaskArray2D& bits = m_LayerBaseBitMasks[ i ];

		bits.Resize( baseTileCountX, baseTileCountZ );

		for( int z = 0, ze = baseTileCountZ; z < ze; z ++ )
		{
			for( int x = 0, xe = baseTileCountX; x < xe; x ++ )
			{
				bits.Set( x, z, oldBits.Get( x / scaleX, z / scaleZ ) );
			}
		}
	}
}

//------------------------------------------------------------------------

static void FixBitMaskData( r3dBitMaskArray2D* oArray, const r3dTerrain2::Bytes& data )
{
	int nominalSize = oArray->GetWidth() * oArray->GetHeight();

	int sizeRatio = data.Count() * 8 / nominalSize;

	int sideRatio = (int) sqrtf( (float)sizeRatio );

	r3d_assert( sideRatio * sideRatio == sizeRatio );

	r3dBitMaskArray2D fixupArray;

	fixupArray.Resize( sideRatio * oArray->GetWidth(), sideRatio * oArray->GetHeight() );

	fixupArray.SetData( &data[ 0 ], data.Count() );

	for( int z = 0, e = oArray->GetHeight(); z < e; z ++ )
	{
		for( int x = 0, e = oArray->GetHeight(); x < e; x ++ )
		{
			oArray->Set( x, z, fixupArray.Get( x, z ) );
		}
	}
}

int
r3dTerrain2::LoadBin( const char* dirName )
{
	r3dFile* hFile = r3d_open( Va( FNAME_TERRAIN2_BIN, dirName ), "rb");
	r3d_assert( hFile );
	if ( ! hFile )
		return 0;

	struct AtExit
	{
		r3dFile*		closeMe;

		~AtExit()
		{
			fclose( closeMe );
		}
	} atExit = { hFile };

	uint32_t dwSignature = -1;
	uint32_t dwVersion = -1;

	fread( &dwSignature, sizeof( dwSignature ), 1, hFile );
	fread( &dwVersion, sizeof( dwVersion ), 1, hFile );

	if( dwSignature != TERRAIN2_SIGNATURE
			||
		( dwVersion != TERRAIN2_VERSION
			&&
			dwVersion != TERRAIN2_VERSION_101
		)
	)
	{
		r3dOutToLog( "r3dTerrain2::LoadBin: unkown .heightmap file version\n!" );
		return 0;
	}

	PreparePhysXHeightFieldDesc_NoAlloc( &m_PhysicsHeightFieldDesc );

	uint32_t hmSize = GetHeightFieldDataSize( m_PhysicsHeightFieldDesc );
	uint32_t desiredSampleCount = hmSize / m_PhysicsHeightFieldDesc.samples.stride;

	uint32_t sampleCount;

	fread( &sampleCount, sizeof sampleCount, 1, hFile );

	if( sampleCount != desiredSampleCount )
	{
		r3dOutToLog( "r3dTerrain2::LoadBin: Heightmap/Terrain Settings mismatch! Can't load!\n" );
	}

	Shorts samples( sampleCount );

	fread( &samples[ 0 ], sampleCount * sizeof samples[ 0 ], 1, hFile );

	char normalMapFile[ 1024 ];
	PrintFullNormalPath( dirName, normalMapFile );

	bool needCalcNormals = false;

	if( !r3dFileExists( normalMapFile ) )
	{
		LoadDetailNormalTex();

		needCalcNormals = true;
	}

	InitFromHeights( samples, needCalcNormals );

	if( !g_bEditMode )
	{
		if( m_DetailNormalTex )
		{
			r3dRenderer->DeleteTexture( m_DetailNormalTex );
			m_DetailNormalTex = NULL;
		}
	}

	m_HeightFieldDataCRC32 = r3dCRC32((const BYTE *)&samples[0], samples.Count() * sizeof( samples[0] ) );

	int tileLayersCount;
	fread( &tileLayersCount, sizeof(tileLayersCount), 1, hFile );

	r3d_assert( tileLayersCount >= (int)m_Layers.Count() );

	int fileLayerCount = tileLayersCount;

	if( tileLayersCount > (int)m_Layers.Count() )
	{
		r3dArtBug( "Terrain2 was uploaded in inconsistent state - layer count in terrain2.bin is greater than in terrain2.ini. Someone forgot to upload terrain2.ini?" );
		tileLayersCount = m_Layers.Count();
	}

	m_LayerBaseBitMasks.Resize( tileLayersCount );

	Bytes data;

	int baseTileCountX;
	int baseTileCountZ;

	if( dwVersion == TERRAIN2_VERSION_101 )
	{
		GetBaseTileCounts_101( &baseTileCountX, &baseTileCountZ );
	}
	else
	{
		GetBaseTileCounts( &baseTileCountX, &baseTileCountZ );
	}

	for( int i = 0, e = (int)m_LayerBaseBitMasks.Count(); i < e; i ++ )
	{
		r3dBitMaskArray2D& bits = m_LayerBaseBitMasks[ i ];
		bits.Resize( baseTileCountX, baseTileCountZ );

		int one = 0;
		fread( &one, sizeof one, 1, hFile );

		data.Resize( one );

		fread( &data[ 0 ], data.Count() * sizeof data[ 0 ], 1, hFile );

		// fix erroneously saved data
		if( (int)data.Count() > bits.GetDataSize() )
		{
			FixBitMaskData( &bits, data );
		}
		else
		{
			bits.SetData( &data[ 0 ], data.Count() );
		}
	}

	for( int i = (int)m_LayerBaseBitMasks.Count(), e = fileLayerCount; i < e; i ++ )
	{
		int one = 0;
		fread( &one, sizeof one, 1, hFile );
		fseek( hFile, one * sizeof( data[ 0 ] ), SEEK_CUR );
	}

	if( dwVersion == TERRAIN2_VERSION_101 )
	{
		ExpendBaseBitMasksFrom101();
	}

	int width_height[ 2 ];

	fread( width_height, sizeof width_height, 1, hFile );

	r3dTL::TArray< UINT8 > temp;

	temp.Resize( width_height[ 0 ] * width_height[ 1 ] );

	fread( &temp[ 0 ], temp.Count() * sizeof temp[ 0 ], 1, hFile );

	m_DominantLayerData.Swap( temp, width_height[ 0 ], width_height[ 1 ] );

	InitLayerBitMaskChains();

	return 1;
}

//------------------------------------------------------------------------

void r3dTerrain2::ExtractFloatHeights( const Shorts& shorts, Floats* oFloats )
{
	SetupHFScale();

	oFloats->Resize( shorts.Count() );

	for( int i = 0, e = shorts.Count(); i < e; i ++ )
	{
		int x = i % m_VertexCountX;
		int z = i / m_VertexCountX;

		(*oFloats)[ x * m_VertexCountZ + z ] = shorts[ i ] * m_InvHFScale;
	}

}

//------------------------------------------------------------------------

void
r3dTerrain2::InitFromHeights( const Shorts& shorts, bool recalcNormals )
{
#if R3D_TERRAIN_V2_GRAPHICS
	if( m_HeightTex )
		r3dRenderer->DeleteTexture( m_HeightTex );

	if( m_AllowVFetch )
	{
		m_HeightTex = r3dRenderer->AllocateTexture();
		m_HeightTex->Create( m_VertexCountX, m_VertexCountZ, D3DFMT_L16, 1 );

		UShorts().Swap( m_HeightArr );
	}
	else
	{
		m_HeightArr.Resize( m_VertexCountX * m_VertexCountZ );
	}

	UpdateGraphicsHeightField( shorts, NULL );

	InitTileField( shorts );
#endif

	//------------------------------------------------------------------------
	// convert shorts to floats compute normal map

	Floats heights;

	ExtractFloatHeights( shorts, &heights );

#if R3D_TERRAIN_V2_GRAPHICS
	if( recalcNormals )
	{
		if( m_NormalTex )
			r3dRenderer->DeleteTexture( m_NormalTex );

		m_NormalTex = r3dRenderer->AllocateTexture();

		m_NormalTex->Create( m_VertexCountX * m_NormalDensity, m_VertexCountZ * m_NormalDensity, D3DFMT_R5G6B5, 1 );

		Vectors temp0;
		Vectors temp1;

		RecalcNormalMap( heights, &temp0, &temp1, NULL );
	}
#endif

	UpdatePhysHeightField( shorts );
}

//------------------------------------------------------------------------

void
r3dTerrain2::SaveLayerToScript( FILE* hFile, const r3dTerrainLayer& layer, char* tab )
{
#if R3D_TERRAIN_V2_GRAPHICS
	fprintf( hFile, "%s{\n", tab );

	fprintf( hFile, "%s\tname:\t\t\"%s\"\n",	tab, layer.Name.c_str()	);

	if( layer.DiffuseTex )
	{
		fprintf( hFile, "%s\tmap_diffuse:\t\"%s\"\n", tab, layer.DiffuseTex->getFileLoc().FileName );
	}

	if( layer.NormalTex )
	{
		fprintf( hFile, "%s\tmap_normal:\t\t\"%s\"\n", tab, layer.NormalTex->getFileLoc().FileName );
	}

	fprintf( hFile, "%s\tscale_u:\t\t%f\n", 	tab, layer.ScaleU		);
	fprintf( hFile, "%s\tscale_v:\t\t%f\n", 	tab, layer.ScaleU		);

	fprintf( hFile, "%s\tspecular:\t\t%f\n", 	tab, layer.SpecularPow		);

	fprintf( hFile, "%s\tmat_type:\t%s\n",		tab, layer.MaterialTypeName.c_str()	);

	fprintf( hFile, "%s}\n", tab );
#endif
}

//------------------------------------------------------------------------

int
r3dTerrain2::LoadLayerFromScript( Script_c *script, r3dTerrainLayer *layer )
{
#if !R3D_TERRAIN_V2_GRAPHICS
	return 0;
#endif
    // fixed buffer steepsounds
	char buffer[ MAX_PATH ];
	char szName[ MAX_PATH ];

	int hasTextures = 0;

	int DownScale = GetTerraTexDownScale();

	layer->MaterialTypeName = "";

	script->SkipToken( "{" );
	while ( ! script->EndOfFile() )
	{
		script->GetToken( buffer );
		if ( *buffer == '}' )
			break;

		if( ! strcmp( buffer, "name:" ) )
		{
			script->GetString( szName, sizeof( szName ) );
			layer->Name = szName;
		}
		else if ( ! strcmp( buffer, "map_diffuse:" ) )
		{
			hasTextures = 1;

			script->GetString( szName, sizeof( szName ) );

			layer->DiffuseTex = r3dRenderer->LoadTexture( szName, D3DFMT_UNKNOWN, false, DownScale );
		}
		else if ( ! strcmp( buffer, "map_normal:" ) )
		{
			hasTextures = 1;

			script->GetString( szName, sizeof( szName ) );
			layer->NormalTex = r3dRenderer->LoadTexture( szName, D3DFMT_UNKNOWN, false, DownScale );
		}
		else if ( ! strcmp( buffer, "scale_u:" ) )
		{
			layer->ScaleU = script->GetFloat();
		}
		else if ( ! strcmp( buffer, "scale_v:" ) )
		{
			layer->ScaleV = script->GetFloat();
		}
		else if ( ! strcmp( buffer, "specular:" ) )
		{
			layer->SpecularPow = script->GetFloat();
		}
		else if( ! strcmp( buffer, "mat_type:"  ) )
		{
			char buff[ 512 ];

			buff[ sizeof buff - 1 ] = 0;

			script->GetLine( buff, sizeof buff - 1 );
			if(strstr(strupr(buff), "CONCRETE"))
				strcpy(buff,"Concrete");
			else if(strstr(strupr(buff), "DIRT"))
				strcpy(buff,"Dirt");
			else if(strstr(strupr(buff), "SAND"))
				strcpy(buff,"Sand");
			else if(strstr(strupr(buff), "SNOW"))
				strcpy(buff,"Snow");
			else if(strstr(strupr(buff), "GRASS"))
				strcpy(buff,"Grass");
			else if(strstr(strupr(buff), "FOREST"))
				strcpy(buff,"Forest");
			else if(strstr(strupr(buff), "WOOD"))
				strcpy(buff,"Wood");
			else if(strstr(strupr(buff), "METAL_RESOURCES"))
				strcpy(buff,"Metal_Resources");
			else if(strstr(strupr(buff), "WOOD_RESOURCES"))
				strcpy(buff,"Wood_Resources");
			else if(strstr(strupr(buff), "WATER"))
				strcpy(buff,"Water");		
			else if(strstr(strupr(buff), "METAL"))
				strcpy(buff,"Metal");	

			layer->MaterialTypeName = buff;

		}
	}

	return hasTextures;
}

//------------------------------------------------------------------------

void
r3dTerrain2::SetupHFScale()
{
	float heightScale = R3D_MAX( fabs( m_HeightOffset ), fabs( m_HeightOffset + m_HeightScale ) );

	if(heightScale == 0.0f)
		heightScale = 1.0f;

	m_HFScale = 32767.0f / heightScale;
	m_InvHFScale = 1.f / m_HFScale;
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateHFShape()
{
	r3d_assert(_CrtCheckMemory());
	PxHeightFieldGeometry hfGeom = GetHFShape();

	r3d_assert(_CrtCheckMemory());
	PxU32	w = (PxU32)this->m_VertexCountX,
			h = (PxU32)this->m_VertexCountZ;

	// do not create a new shape, just update current shape with new parameters
	hfGeom.heightScale		= m_InvHFScale;
	hfGeom.rowScale			= m_TotalTerrainZLength / PxReal(h);
	hfGeom.columnScale		= m_TotalTerrainXLength / PxReal(w);

	r3d_assert( m_PhysicsTerrain );
	r3d_assert( m_PhysicsTerrain->getNbShapes () == 1 );

	r3d_assert(_CrtCheckMemory());
	PxShape* shapes[1] = {0};

	m_PhysicsTerrain->getShapes(&shapes[0], 1);

	r3d_assert(_CrtCheckMemory());

	r3d_assert( shapes[0]->getGeometryType() == PxGeometryType::eHEIGHTFIELD );
	//shapes[0]->setGeometry(hfGeom); // memory corruption. not sure why.

	r3d_assert(_CrtCheckMemory());
}

//------------------------------------------------------------------------

PxHeightFieldGeometry
r3dTerrain2::GetHFShape() const
{
	r3d_assert( m_PhysicsTerrain );
	r3d_assert( m_PhysicsTerrain->getNbShapes () == 1 );

	PxShape* shapes[1] = {0};
	m_PhysicsTerrain->getShapes(&shapes[0], 1);
	r3d_assert( shapes[0]->getGeometryType() == PxGeometryType::eHEIGHTFIELD );
	PxHeightFieldGeometry geom;

	bool res = shapes[0]->getHeightFieldGeometry(geom);
	r3d_assert(res);

	return geom;
}

//------------------------------------------------------------------------

void
r3dTerrain2::PreparePhysXHeightFieldDesc_NoAlloc( PxHeightFieldDesc* hfDesc )
{
	int w = m_VertexCountX,
		h = m_VertexCountZ;

	hfDesc->format				= PxHeightFieldFormat::eS16_TM;
	hfDesc->nbColumns			= w;
	hfDesc->nbRows				= h;
	hfDesc->convexEdgeThreshold	= 0;
	hfDesc->thickness			= -1000.0f;

	// allocate storage for samples
	hfDesc->samples.stride		= sizeof(PxU32);

}

//------------------------------------------------------------------------

void
r3dTerrain2::PreparePhysXHeightFieldDesc( PxHeightFieldDesc* hfDesc )
{
	PreparePhysXHeightFieldDesc_NoAlloc( hfDesc );

	hfDesc->samples.data		= malloc( hfDesc->samples.stride * hfDesc->nbColumns* hfDesc->nbRows );

	if( hfDesc->samples.data == NULL )
		r3dError("Out of memory!");
}

//------------------------------------------------------------------------

void
r3dTerrain2::FinishPhysXHeightFieldDesc	( PxHeightFieldDesc* hfDesc )
{
	free((void*)hfDesc->samples.data);
	hfDesc->samples.data = NULL;
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdatePhysHeightField( const Shorts& source )
{
	PreparePhysXHeightFieldDesc( &m_PhysicsHeightFieldDesc );

	SetupHFScale();

	if( m_PhysicsTerrain )
	{
		m_PhysicsTerrain->release();
		m_PhysicsTerrain = NULL;
	}

	if( m_PhysicsHeightField )
	{
		m_PhysicsHeightField->release();
		m_PhysicsHeightField = NULL;
	}

	PxU32	w = (PxU32)m_VertexCountX,
			h = (PxU32)m_VertexCountZ;

	char* currentByte = (char*)m_PhysicsHeightFieldDesc.samples.data;

	for ( int i = 0, e = h * w;  i < e; i ++ )
	{
		PxI16 height = (PxI32) source[ i ];

		PxHeightFieldSample* currentSample = (PxHeightFieldSample*)currentByte;
		currentSample->height = height;
		currentSample->materialIndex0 = 1;
		currentSample->materialIndex1 = 1;
		currentSample->clearTessFlag();
		currentByte += m_PhysicsHeightFieldDesc.samples.stride;
	}

	m_PhysicsHeightField = g_pPhysicsWorld->PhysXSDK->createHeightField( m_PhysicsHeightFieldDesc );

	FinishPhysXHeightFieldDesc( &m_PhysicsHeightFieldDesc );

	PxHeightFieldGeometry shapeGeom(m_PhysicsHeightField, PxMeshGeometryFlags(), m_InvHFScale, m_TotalTerrainZLength / PxReal(h), m_TotalTerrainXLength / PxReal(w));
	PxTransform pose(PxVec3(0,0,0), PxQuat(0,0,0,1));
	m_PhysicsTerrain = g_pPhysicsWorld->PhysXSDK->createRigidStatic(pose);

	PxShape* aHeightFieldShape = m_PhysicsTerrain->createShape(shapeGeom, *g_pPhysicsWorld->defaultMaterial);

	PxFilterData filterData(PHYSCOLL_STATIC_GEOMETRY, 0, 0, 0);
	aHeightFieldShape->setSimulationFilterData(filterData);
	PxFilterData qfilterData(1<<PHYSCOLL_STATIC_GEOMETRY, 0, 0, 0);
#ifndef WO_SERVER
#if VEHICLES_ENABLED
	VehicleSetupDrivableShapeQueryFilterData(qfilterData);
#endif
#endif
	aHeightFieldShape->setQueryFilterData(qfilterData);

	g_pPhysicsWorld->AddActor(*m_PhysicsTerrain);

	UpdateHFShape();
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdatePhysHeightField ( const Floats& heightFieldData )
{
	r3d_assert(_CrtCheckMemory());
	r3dOutToLog("TERRAIN2: UpdatePhysHeightField\n");

	if(m_PhysicsTerrain)
	{
		m_PhysicsTerrain->release();
		m_PhysicsTerrain = 0;
		m_PhysicsHeightField->release();
		m_PhysicsHeightField = 0;
	}

	const Floats& source = heightFieldData;
	r3d_assert( source.Count() == m_VertexCountX * m_VertexCountZ );

	r3d_assert(_CrtCheckMemory());
	PreparePhysXHeightFieldDesc( &m_PhysicsHeightFieldDesc );
	r3d_assert(_CrtCheckMemory());

	char* currentByte = (char*)m_PhysicsHeightFieldDesc.samples.data;

	float maxHeight = -FLT_MAX;
	float minHeight = FLT_MAX;

	for( uint32_t i = 0, e = source.Count(); i < e; i ++ )
	{
		maxHeight = R3D_MAX( source[ i ], maxHeight );
		minHeight = R3D_MIN( source[ i ], minHeight );
	}

	r3d_assert(_CrtCheckMemory());
	SetupHFScale();

	r3d_assert(_CrtCheckMemory());
	float norm = m_HFScale;

	PxU32	w = (PxU32)m_VertexCountX,
			h = (PxU32)m_VertexCountZ;

	for (PxU32 column = 0; column < w; column++)
	{
		for (PxU32 row = 0; row < h; row++)
		{
			PxI16 height = (PxI32)( R3D_MIN( source[ row * w + column ] * norm + 0.5f, 32767.f ) );

			PxHeightFieldSample* currentSample = (PxHeightFieldSample*)currentByte;
			currentSample->height = height;
			currentSample->materialIndex0 = 1;
			currentSample->materialIndex1 = 1;
			currentSample->clearTessFlag();
			currentByte += m_PhysicsHeightFieldDesc.samples.stride;
		}
	}

	m_PhysicsHeightField = g_pPhysicsWorld->PhysXSDK->createHeightField(m_PhysicsHeightFieldDesc);

	FinishPhysXHeightFieldDesc( &m_PhysicsHeightFieldDesc );

	PxHeightFieldGeometry shapeGeom(m_PhysicsHeightField, PxMeshGeometryFlags(), m_InvHFScale, m_TotalTerrainZLength / PxReal(h), m_TotalTerrainXLength / PxReal(w));
	PxTransform pose(PxVec3(0,0,0), PxQuat(0,0,0,1));
	m_PhysicsTerrain = g_pPhysicsWorld->PhysXSDK->createRigidStatic(pose);

	PxShape* aHeightFieldShape = m_PhysicsTerrain->createShape(shapeGeom, *g_pPhysicsWorld->defaultMaterial);

	PxFilterData filterData(PHYSCOLL_STATIC_GEOMETRY, 0, 0, 0);
	aHeightFieldShape->setSimulationFilterData(filterData);
	PxFilterData qfilterData(1<<PHYSCOLL_STATIC_GEOMETRY, 0, 0, 0);
#ifndef WO_SERVER
#if VEHICLES_ENABLED
	VehicleSetupDrivableShapeQueryFilterData(qfilterData);
#endif
#endif
	aHeightFieldShape->setQueryFilterData(qfilterData);

	g_pPhysicsWorld->AddActor(*m_PhysicsTerrain);

	UpdateHFShape();

	r3d_assert(_CrtCheckMemory());
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateSceneBox()
{
	// make scene boxes bigger for a huge level
	extern float gSceneBox_LevelBase;
	extern unsigned int gSceneBox_MinObjCount;

	float worldSize = R3D_MAX( m_TotalTerrainXLength, m_TotalTerrainZLength );

	if( worldSize > 10000.0f)
	{
		gSceneBox_LevelBase = 30.0f;
		gSceneBox_MinObjCount = 5;
	}
	if( worldSize > 20000.0f)
	{
		gSceneBox_LevelBase = 60.0f;
		gSceneBox_MinObjCount = 10;
	}
}

//------------------------------------------------------------------------

void r3dTerrain2::RecalcLayerVar( r3dTerrainLayer* oLayer )
{
	oLayer->ShaderScaleU = oLayer->ScaleU * m_TerrainPosToSplatU;
	// note SplatU again - to maintain uniformity on non square terrains
	oLayer->ShaderScaleV = oLayer->ScaleV * m_TerrainPosToSplatU;
}

//------------------------------------------------------------------------

void
r3dTerrain2::RecalcTileCounts()
{
	m_TileCountX = m_VertexCountX / m_QualitySettings.VertexTileDim;
	m_TileCountZ = m_VertexCountZ / m_QualitySettings.VertexTileDim;
}

//------------------------------------------------------------------------

void r3dTerrain2::UpdateDesc()
{
	r3dTerrainDesc desc;

	desc.LayerCount = m_Layers.Count() + 1;

	desc.XSize = m_TotalTerrainXLength;
	desc.ZSize = m_TotalTerrainZLength;

	desc.CellCountX = m_VertexCountX;
	desc.CellCountZ = m_VertexCountZ;

	desc.SplatResolutionU = m_SplatResolutionU;
	desc.SplatResolutionV = m_SplatResolutionV;

	desc.MinHeight = m_HeightOffset;
	desc.MaxHeight = m_HeightOffset + m_HeightScale;

	desc.CellSize = m_CellSize;
	desc.CellCountPerTile = m_QualitySettings.VertexTileDim;

	SetDesc( desc );
}

//------------------------------------------------------------------------

void r3dTerrain2::RecalcVars()
{
	m_QualitySettings.Sync();

	RecalcTileCounts();

	m_AtlasTileCountPerSide		= ATLAS_TEXTURE_DIM / m_QualitySettings.AtlasTileDim;
	m_AtlasTileCountPerSideInv	= 1.0f / m_AtlasTileCountPerSide;

	m_TileWorldDims[ 0 ] = m_TileUnitWorldDim * m_QualitySettings.VertexTileDim;

	m_HalfTileWorldDims[ 0 ] = m_TileWorldDims[ 0 ] * 0.5f;

	for( int i = 1, e = m_TileWorldDims.COUNT; i < e; i ++ )
	{
		m_TileWorldDims[ i ] = m_TileWorldDims[ i - 1 ] * 2.0f;
		m_HalfTileWorldDims[ i ] = m_TileWorldDims[ i - 1 ];
	}

	for( int i = 0; i < NUM_QUALITY_LAYERS; i ++ )
	{
		m_QualitySettings.TileDistances[ i ] = m_QualitySettings.TileCounts[ i ] * m_TileWorldDims[ i ];

		if( i )
		{
			m_QualitySettings.TileDistances[ i ] += m_QualitySettings.TileDistances[ i - 1 ];
		}
	}

	m_TotalTerrainXLength = m_TileCountX * m_TileWorldDims [ 0 ];
	m_TotalTerrainZLength = m_TileCountZ * m_TileWorldDims [ 0 ];

	m_TerrainPosToSplatU = 0.5f / m_TotalTerrainXLength;
	m_TerrainPosToSplatV = 0.5f / m_TotalTerrainZLength;

	m_CellSize = m_TileWorldDims [ 0 ] / m_QualitySettings.VertexTileDim;

	UpdateDesc();
}

//------------------------------------------------------------------------

void r3dTerrain2::InitTileField( const Shorts& heightField )
{
#if R3D_TERRAIN_V2_GRAPHICS
	RecalcVars();
	SetupHFScale();

	int tileCountX = m_TileCountX;
	int tileCountZ = m_TileCountZ;

	m_VisibleTiles.Reserve( m_TileCountX * m_TileCountZ );

	m_NumActiveQualityLayers = NUM_QUALITY_LAYERS;

	for( int L = 0, e = NUM_QUALITY_LAYERS; L < e; L ++ )
	{
		tileCountX /= 2;
		tileCountZ /= 2;

		if( !tileCountX || !tileCountZ )
		{
			m_NumActiveQualityLayers = L;
			break;
		}
	}

	m_TileInfoMipChain.Resize( m_NumActiveQualityLayers );

	m_TileInfoMipChain[ 0 ].Resize( m_TileCountX, m_TileCountZ );

	tileCountX = m_TileCountX;
	tileCountZ = m_TileCountZ;

	for( int i = 1, e = m_TileInfoMipChain.Count(); i < e; i ++ )
	{

		TileInfoArr2D& mip		= m_TileInfoMipChain[ i ];

		tileCountX /= 2;
		tileCountZ /= 2;

		mip.Resize( tileCountX, tileCountZ );
	}

	m_AllocTileLodArray.Resize( m_NumActiveQualityLayers );

	RecalcTileInfo( heightField, NULL );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::RecalcTileInfo( const Shorts& heightField, const RECT* rect )
{
#if R3D_TERRAIN_V2_GRAPHICS
	RECT r;

	if( rect )
	{
		r = * rect;
	}
	else
	{
		r.left = 0;
		r.right = m_VertexCountX;
		r.top = 0;
		r.bottom = m_VertexCountZ;
	}

	r.left		/= m_QualitySettings.VertexTileDim;
	r.right		/= m_QualitySettings.VertexTileDim;
	r.top		/= m_QualitySettings.VertexTileDim;
	r.bottom	/= m_QualitySettings.VertexTileDim;

	r.left		= R3D_MAX( R3D_MIN( (int)r.left, m_TileCountX ), 0 );
	r.right		= R3D_MAX( R3D_MIN( (int)r.right, m_TileCountX ), 0 );
	r.top		= R3D_MAX( R3D_MIN( (int)r.top, m_TileCountZ ), 0 );
	r.bottom	= R3D_MAX( R3D_MIN( (int)r.bottom, m_TileCountZ ), 0 );

	int tileCountX = m_TileCountX;
	int tileCountZ = m_TileCountZ;

	int vertDim = m_QualitySettings.VertexTileDim;
	int fullXVertDim = m_TileCountX * m_QualitySettings.VertexTileDim;
	int fullZVertDim = m_TileCountZ * m_QualitySettings.VertexTileDim;

	TileInfoArr2D& arr = m_TileInfoMipChain[ 0 ];

	for( int zz = r.top; zz < r.bottom; zz ++ )
	{
		for( int xx = r.left; xx < r.right; xx ++ )
		{
			int ax = xx;
			int az = zz;

			vertDim = m_QualitySettings.VertexTileDim;

			int tcx = tileCountX;
			int tcz = tileCountZ;

			int heightMin = +0x10000;
			int heightMax = -0x10000;

			for( int	sz = az * vertDim,
				sze = R3D_MIN( ( az + 1 ) * vertDim + 1, fullZVertDim );
				sz < sze; sz ++ )
			{
				for( int	sx = ax * vertDim,
					sxe = R3D_MIN( ( ax + 1 ) * vertDim + 1, fullXVertDim );
					sx < sxe; sx ++ )
				{
					int height = heightField[ sz + sx * fullZVertDim ];

					heightMin = R3D_MIN( height, heightMin );
					heightMax = R3D_MAX( height, heightMax );
				}
			}

			TileInfo& tinfo = arr[ az ][ ax ];

			tinfo.HeightMin		= heightMin * m_InvHFScale;
			tinfo.HeightRange	= ( heightMax - heightMin ) * m_InvHFScale;
		}
	}

	for( int i = 1, e = m_TileInfoMipChain.Count(); i < e; i ++ )
	{

		TileInfoArr2D& prevMip	= m_TileInfoMipChain[ i - 1 ];
		TileInfoArr2D& mip		= m_TileInfoMipChain[ i - 0 ];

		int prevTileCountX = tileCountX;

		tileCountX /= 2;
		tileCountZ /= 2;

		for( int z = 0, e = tileCountZ; z < e; z ++ )
		{
			for( int x = 0, e = tileCountX; x < e; x ++ )
			{
				int ax = x * 2;
				int az = z * 2;

				TileInfo& tarInfo = mip[ z ][ x ];

				const TileInfo& srcInfo00 = prevMip[ az + 0 ][ ax + 0 ];
				const TileInfo& srcInfo10 = prevMip[ az + 0 ][ ax + 1 ];
				const TileInfo& srcInfo01 = prevMip[ az + 1 ][ ax + 0 ];
				const TileInfo& srcInfo11 = prevMip[ az + 1 ][ ax + 1 ];

				tarInfo.HeightMin = R3D_MIN(
					R3D_MIN( srcInfo00.HeightMin, srcInfo01.HeightMin ),
					R3D_MIN( srcInfo10.HeightMin, srcInfo11.HeightMin )
					);

				float heightMax =
					R3D_MAX(
					R3D_MAX( srcInfo00.HeightMin + srcInfo00.HeightRange, srcInfo10.HeightMin + srcInfo10.HeightRange ),
					R3D_MAX( srcInfo01.HeightMin + srcInfo01.HeightRange, srcInfo11.HeightMin + srcInfo11.HeightRange )
					);

				tarInfo.HeightRange = heightMax - tarInfo.HeightMin;
			}
		}
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::InitLayerBaseBitMask( int idx )
{
#if R3D_TERRAIN_V2_GRAPHICS
	int baseCountX;
	int baseCountZ;

	GetBaseTileCounts( &baseCountX, &baseCountZ );

	m_LayerBaseBitMasks[ idx ].Resize( baseCountX, baseCountZ );
	m_LayerBaseBitMasks[ idx ].ZeroAll();

	for( int z = 0, t = 0, e = baseCountZ; z < e; z ++ )
	{
		for( int x = 0, e = baseCountX; x < e; x ++, t ++ )
		{
			UpdateBaseBitMask( &m_LayerBaseBitMasks[ idx ], x, z, m_LayerChannels[ idx ] );
		}
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::InitLayerBaseBitMasks()
{
#if R3D_TERRAIN_V2_GRAPHICS
	m_LayerBaseBitMasks.Resize( m_Layers.Count() );

	for( int i = 0, e = m_Layers.Count(); i < e; i ++ )
	{
		InitLayerBaseBitMask( i );
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::InitLayerBitMaskChain( int idx )
{
#if R3D_TERRAIN_V2_GRAPHICS
	m_LayerBitMasksMipChains[ idx ].Clear();

	for( int z = 0, t = 0,  e = m_TileCountZ; z < e; z ++ )
	{
		for( int x = 0, e = m_TileCountX; x < e; x ++, t ++ )
		{
			UpdateTileBitMaskChain( &m_LayerBitMasksMipChains[ idx ], m_LayerBaseBitMasks[ idx ], x, z );
		}
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::InitLayerBitMaskChains()
{
#if R3D_TERRAIN_V2_GRAPHICS
	m_LayerBitMasksMipChains.Resize( m_Layers.Count() );

	for( int i = 0, e = m_Layers.Count(); i < e; i ++ )
	{
		InitLayerBitMaskChain( i );
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::InitLayers()
{
	for( int i = 0, m = 0, ch = 0, e = m_Layers.Count(); i < e; i ++, ch ++ )
	{
		if( ch == 3 )
		{
			m ++;
			ch = 0;
		}

		r3dTerrainLayer* layer = &m_Layers[ i ];

		layer->ChannelIdx = ch;

		switch( ch )
		{
		case 0:
			layer->ChannelMask = float4( 1, 0, 0, 0 );
			break;
		case 1:
			layer->ChannelMask = float4( 0, 1, 0, 0 );
			break;
		case 2:
			layer->ChannelMask = float4( 0, 0, 1, 0 );
			break;
		}

		layer->MaskTex = m_Masks[ m ];
	}

	m_BaseLayer.ChannelIdx = 0;
	m_BaseLayer.ChannelMask = float4( 1, 0, 0, 0 );

	DistributeLayerMasks();
}

//------------------------------------------------------------------------

void
r3dTerrain2::DistributeLayerMasks()
{
#if R3D_TERRAIN_V2_GRAPHICS
	int maskIdx = 0;

	for( int i = 0, e = m_Layers.Count(), j = 0; i < e; i ++, j ++ )
	{
		if( j == 3 )
		{
			maskIdx ++;
			j = 0;
		}

		m_Layers[ i ].MaskTex = m_Masks[ maskIdx ];
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::RecalcNormalMap( const Floats& heights, Vectors* tempVectors0, Vectors* tempVectors1, const RECT* rect )
{
#if R3D_TERRAIN_V2_GRAPHICS

	RECT r;

	if( rect )
	{
		r = * rect;
	}
	else
	{
		r.left		= 0;
		r.right		= m_VertexCountX;
		r.top		= 0;
		r.bottom	= m_VertexCountZ;
	}

	int xTotal = m_VertexCountX;
	int zTotal = m_VertexCountZ;

	struct
	{
		float operator() ( int x, int z )
		{
			return (*heights)[ x + z * xTot ];
		}

		int xTot;

		const Floats * heights;

	} getHeight = { xTotal, &heights };

	Vectors& NormalMapData = *tempVectors0;
	Vectors& NormalBuf = *tempVectors1;

	NormalMapData.Resize( xTotal * zTotal );
	NormalBuf.Resize( NormalMapData.Count() );

	int bx0 = r.left - 2;
	int bx1 = r.right + 2;
	int bz0 = r.top - 2;
	int bz1 = r.bottom + 2;

	bx0 = R3D_MIN( R3D_MAX( bx0, 0 ), m_VertexCountX );
	bx1 = R3D_MIN( R3D_MAX( bx1, 0 ), m_VertexCountX );
	bz0 = R3D_MIN( R3D_MAX( bz0, 0 ), m_VertexCountZ );
	bz1 = R3D_MIN( R3D_MAX( bz1, 0 ), m_VertexCountZ );

	for ( int z = bz0, ze = bz1; z < ze; z++ )
	{
		for ( int x = bx0, xe = bx1; x < xe; x++ )
		{
			int xx = R3D_MIN( x, xTotal - 1 );
			int zz = R3D_MIN( z, zTotal - 1 );
			float sx = getHeight( xx < xTotal-1 ? xx+1 : xx, zz) - getHeight(xx>0 ? xx-1 : xx, zz);
			if (xx == 0 || xx == xTotal-1)
				sx *= 2;

			float sy = getHeight( xx, zz < zTotal - 1 ? zz+1 : zz) - getHeight(xx, zz>0 ?  zz-1 : zz);
			if( zz == 0 || zz == zTotal - 1 )
				sy *= 2;

			r3dPoint3D vN(-sx, m_CellSize, -sy);
			vN.Normalize();

			NormalMapData[ zz * xTotal + xx ] = vN;
		}
	}

	// do separable normal 'blur'
	for( int i = 0; i < 4; i ++ )
	{
		r3dPoint3D* source;
		r3dPoint3D* target;

		int dirx;
		int dirz;

		if( i & 1 )
		{
			source = &NormalBuf[ 0 ];
			target = &NormalMapData[ 0 ];

			dirx = 1;
			dirz = 0;
		}
		else
		{
			source = &NormalMapData[ 0 ];
			target = &NormalBuf[ 0 ];

			dirx = 0;
			dirz = 1;
		}

		for ( int z = bz0, ze = bz1; z < ze; z++ )
		{
			for ( int x = bx0, xe = bx1; x < xe; x++ )
			{

				int xc = x, zc = z;

				int x0	= R3D_MAX( xc - dirx, 0 ),
					x1	= R3D_MIN( xc + dirx, xTotal - 1 ),
					z0	= R3D_MAX( zc - dirz, 0 ),
					z1	= R3D_MIN( zc + dirz, zTotal - 1 );


				r3dPoint3D	vN =	source[ zc * xTotal + xc ];
				vN +=	source[ z0 * xTotal + x0 ];
				vN +=	source[ z1 * xTotal + x1 ];

				vN.Normalize();

				target[ zc * xTotal + xc ] = vN;
			}
		}
	}

	RECT nr = r;

	nr.left *= m_NormalDensity;
	nr.right *= m_NormalDensity;
	nr.top *= m_NormalDensity;
	nr.bottom *= m_NormalDensity;

	struct R5G6B5
	{
		UINT16 b : 5;
		UINT16 g : 6;
		UINT16 r : 5;
	} * lockee = static_cast<R5G6B5*> ( m_NormalTex->Lock( 1, &nr ) ),
	  * detail_lockee = NULL;

	if( m_DetailNormalTex )
		detail_lockee = static_cast<R5G6B5*> ( m_DetailNormalTex->Lock( 0, &nr ) );

	int pitch = m_NormalTex->GetLockPitch() / sizeof *lockee;
	int detail_pitch = pitch;

	if( m_DetailNormalTex )
		detail_pitch = m_DetailNormalTex->GetLockPitch() / sizeof *lockee;

	r3d_assert( pitch == detail_pitch );

	for( int z = r.top, ze = r.bottom; z < ze; z ++ )
	{
		for( int x = r.left, xe = r.right; x < xe; x ++ )
		{
			const r3dPoint3D& n = NormalMapData[ x + z * xTotal ];

			for( int nz = z * m_NormalDensity, e = nz + m_NormalDensity; nz < e; nz ++ )
			{
				for( int nx = x * m_NormalDensity, e = nx + m_NormalDensity; nx < e; nx ++ )
				{
					int idx = ( nx - nr.left ) + ( nz - nr.top ) * pitch;

					R5G6B5& sample = lockee[ idx ];

					r3dPoint3D fn = n;

					if( detail_lockee )
					{
						R5G6B5& detail = detail_lockee[ idx ];

						r3dPoint3D detailNormal(	detail.r / 31.f * 2.f - 1.f,
													detail.b / 31.f * 2.f - 1.f,
													detail.g / 63.f * 2.f - 1.f );

						fn = R3D_LERP( fn, detailNormal, m_Settings.DetailNormalMix );
						fn.Normalize();
					}

					sample.r = R3D_MIN( R3D_MAX( int( ( fn.x * 0.5f + 0.5f ) * 31 ), 0 ), 31 );
					sample.g = R3D_MIN( R3D_MAX( int( ( fn.y * 0.5f + 0.5f ) * 63 ), 0 ), 63 );
					sample.b = R3D_MIN( R3D_MAX( int( ( fn.z * 0.5f + 0.5f ) * 31 ), 0 ), 31 );
				}
			}
		}
	}

	if( m_DetailNormalTex )
		m_DetailNormalTex->Unlock();

	m_NormalTex->Unlock();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateBaseBitMask( r3dBitMaskArray2D* array, int baseTileX, int baseTileZ, const Floats& mask )
{
#if R3D_TERRAIN_V2_GRAPHICS
	for( int sz = baseTileZ * QualitySettings::MIN_VERTEX_TILE_DIM - 1 , sze = sz + QualitySettings::MIN_VERTEX_TILE_DIM + 2; sz < sze; sz ++ )
	{
		for( int sx = baseTileX * QualitySettings::MIN_VERTEX_TILE_DIM - 1 , sxe = sx + QualitySettings::MIN_VERTEX_TILE_DIM + 2; sx < sxe; sx ++ )
		{
			if( sx < 0 || sx >= m_VertexCountX )
				continue;

			if( sz < 0 || sz >= m_VertexCountZ )
				continue;

			int mx = sx * m_SplatResolutionU / m_VertexCountX;
			int mz = sz * m_SplatResolutionV / m_VertexCountZ;

			if( mask[ mx + mz * m_SplatResolutionU ] > 1.f )
			{
				array->Set( baseTileX, baseTileZ, true );
				return;
			}
		}
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateTileBitMaskChain( LayerBitMaskMipChain* oChain, const r3dBitMaskArray2D& array, int tileX, int tileZ )
{
#if R3D_TERRAIN_V2_GRAPHICS
	int scale = m_QualitySettings.VertexTileDim / r3dTerrain2::QualitySettings::MIN_VERTEX_TILE_DIM;

	for( int sz = tileZ * scale , sze = sz + scale; sz < sze; sz ++ )
	{
		for( int sx = tileX * scale , sxe = sx + scale; sx < sxe; sx ++ )
		{
			if( array.Get( sx, sz ) )
			{
				if( !oChain->Count() )
				{
					oChain->Resize( m_NumActiveQualityLayers );

					int tileCountX = m_TileCountX;
					int tileCountZ = m_TileCountZ;

					for( int i = 0, e = m_NumActiveQualityLayers; i < e; i ++ )
					{
						(*oChain)[ i ].Resize( tileCountX, tileCountZ );

						tileCountX = R3D_MAX( tileCountX / 2, 1 );
						tileCountZ = R3D_MAX( tileCountZ / 2, 1 );
					}
				}

				int tx = tileX;
				int tz = tileZ;

				for( int i = 0, e = m_NumActiveQualityLayers; i < e; i ++ )
				{
					(*oChain)[ i ].Set( tx, tz, true );

					tx /= 2;
					tz /= 2;
				}

				return;
			}
		}
	}
#endif
}

//------------------------------------------------------------------------

R3D_FORCEINLINE static void PushDebugBBox( const r3dBoundBox& bbox )
{
#if R3D_TERRAIN_V2_GRAPHICS
	void PushDebugBox(	r3dPoint3D p0, r3dPoint3D p1, r3dPoint3D p2, r3dPoint3D p3,
						r3dPoint3D p4, r3dPoint3D p5, r3dPoint3D p6, r3dPoint3D p7,
						r3dColor color );

	PushDebugBox(	bbox.Org,
					bbox.Org + r3dPoint3D( bbox.Size.x, 0.f, 0.f ),
					bbox.Org + r3dPoint3D( 0.f, bbox.Size.y, 0.f ),
					bbox.Org + r3dPoint3D( bbox.Size.x, bbox.Size.y, 0.f ),
					bbox.Org + r3dPoint3D( 0.f, 0.f, bbox.Size.z ),
					bbox.Org + r3dPoint3D( bbox.Size.x, 0.f, bbox.Size.z ),
					bbox.Org + r3dPoint3D( 0.f, bbox.Size.y, bbox.Size.z ),
					bbox.Org + r3dPoint3D( bbox.Size.x, bbox.Size.y, bbox.Size.z ),
					r3dColor( 0, 255, 0 )
				);
#endif
}

R3D_FORCEINLINE
float
r3dTerrain2::GetPaddingCoef() const
{
	return ( m_QualitySettings.AtlasTileDim + 2.f * r_terrain2_padding->GetInt() ) / m_QualitySettings.AtlasTileDim;
}

void
r3dTerrain2::UpdateVisibleTiles()
{
#if R3D_TERRAIN_V2_GRAPHICS
	m_VisibleTiles.Clear();

	for( int L = 0, e = m_NumActiveQualityLayers; L < e; L ++ )
	{
		AllocatedTileArr& layer = m_AllocTileLodArray[ L ];

		float size = m_TileWorldDims[ L ];

		for( int i = 0, e = layer.Count(); i < e; i ++ )
		{
			const AllocatedTile& tile = layer[ i ];

			const TileInfo& info = m_TileInfoMipChain[ L ][ tile.Z ][ tile.X ];

			r3dBoundBox bbox;
			bbox.Org = r3dPoint3D( tile.X * size, info.HeightMin, tile.Z * size );
			bbox.Size = r3dPoint3D( size, info.HeightRange, size );

			if( r3dRenderer->IsBoxInsideFrustum( bbox ) )
			{
				m_VisibleTiles.PushBack( &tile );

#ifndef FINAL_BUILD
#if 0
				if( r_debug_helper->GetInt() )
				{
					PushDebugBBox( bbox );
				}
#endif
#endif
			}
		}
	}

#if 0
	if( m_VisibleTiles.Count() > 2 )
	{
		std::sort( &m_VisibleTiles[ 0 ], &m_VisibleTiles[ 0 ] + m_VisibleTiles.Count() - 1, tile_comp_func );
	}
#endif
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::AddToAllocatedTiles( int x, int z, int L, int conFlags )
{
#if R3D_TERRAIN_V2_GRAPHICS
	R3DPROFILE_FUNCTION( "r3dTerrain2::AddToAllocatedTiles" );

	AllocatedTileArr& arr = m_AllocTileLodArray[ L ];

	int W = ( m_TileCountX >> L );

	int incomingId = W * z + x;

	int l = 0;
	int r = arr.Count() - 1;

	int insert_idx = -1;

	if( r >= l )
	{
		for(;; )
		{
			int idx = ( l + r ) / 2;

			int id = arr[ idx ].Z * W + arr[ idx ].X;

			if( id > incomingId )
			{
				if( r == l )
				{
					insert_idx = r;
					break;
				}

				r = idx;
			}
			else
			{
				if( id < incomingId )
				{
					if( r == l )
					{
						insert_idx = l + 1;
						break;
					}

					l = idx + 1;
				}
				else
				{
					arr[ idx ].Tagged = 1;
					arr[ idx ].ConFlags = conFlags;
					break;
				}
			}
		}
	}
	else
	{
		insert_idx = 0;
	}

	if( insert_idx >= 0 )
	{
		AllocatedTile tile;

		tile.X = x;
		tile.Z = z;
		tile.AtlasVolumeID = -1;
		tile.AtlasTileID = -1;

		tile.Tagged = 1;
		tile.L = L;

		tile.ConFlags = conFlags;

		arr.Insert( insert_idx, tile );
	}

#ifdef _DEBUG
#if 0
	for( int i = 1, e = arr.Count(); i < e; i ++  )
	{
		int id0 = arr[ i - 1 ].X + arr[ i - 1 ].Z * W;
		int id1 = arr[ i + 0 ].X + arr[ i + 0 ].Z * W;

		r3d_assert( id1 > id0 );
	}
#endif
#endif
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateTiles( const r3dCamera& cam )
{
#if R3D_TERRAIN_V2_GRAPHICS
	R3DPROFILE_FUNCTION( "r3dTerrain2::UpdateAtlas" );

	//------------------------------------------------------------------------
	// untag

	for( int L = 0, e = m_NumActiveQualityLayers; L < e; L ++ )
	{
		AllocatedTileArr& arr = m_AllocTileLodArray[ L ];

		for( int i = 0, e = arr.Count(); i < e; i ++ )
		{
			AllocatedTile& tile = arr[ i ];
			tile.Tagged = 0;
		}
	}

	//------------------------------------------------------------------------
	// update activity according to camera

	R3DPROFILE_START( "Tile Activity" );

	int tcx = m_TileCountX;
	int tcz = m_TileCountZ;

	r3dTL::TFixedArray< int, NUM_QUALITY_LAYERS > countXs;
	r3dTL::TFixedArray< int, NUM_QUALITY_LAYERS > countZs;

	for( int i = 0, e = m_NumActiveQualityLayers; i < e; i ++ )
	{
		countXs[ i ] = tcx;
		countZs[ i ] = tcz;

		tcx = R3D_MAX( tcx / 2, 1 );
		tcz = R3D_MAX( tcz / 2, 1 );
	}

	int z_bottom_prev	= 0;
	int z_top_prev		= countZs[ m_NumActiveQualityLayers - 1 ];
	int x_left_prev		= 0;
	int	x_right_prev	= countXs[ m_NumActiveQualityLayers - 1 ];

	for( int L = m_NumActiveQualityLayers - 1; L >= 1; L -- )
	{
		int sx = m_TileCountX / countXs[ L ];
		int sz = m_TileCountZ / countZs[ L ];

		int connMask = 0xffffffff;

		if( m_QualitySettings.TileVertexDensities[ L - 1 ] < m_QualitySettings.TileVertexDensities[ L ] )
		{
			connMask = 0;
		}

		//------------------------------------------------------------------------

		float halfDim = m_HalfTileWorldDims[ L ];

		float distDown = cam.Z - m_QualitySettings.TileDistances[ L - 1 ] + halfDim;

		int endZ = (int) ( distDown / m_TileWorldDims[ L ] );
		endZ = R3D_MIN( R3D_MAX( endZ, 0 ), countZs[ L ] );

		float distUp = cam.Z + m_QualitySettings.TileDistances[ L - 1 ] + halfDim;

		int startZ = ( int ) ( distUp / m_TileWorldDims[ L ] );
		startZ = R3D_MIN( R3D_MAX( startZ, 0 ), countZs[ L ] );

		float distLeft = cam.X - m_QualitySettings.TileDistances[ L - 1 ] + halfDim;

		int endX = ( int )( distLeft / m_TileWorldDims[ L ] );
		endX = R3D_MIN( R3D_MAX( endX, 0 ), countXs[ L ] );

		float distRight = cam.X + m_QualitySettings.TileDistances[ L - 1 ] + halfDim;

		int startX = ( int )( distRight / m_TileWorldDims[ L ] );
		startX = R3D_MIN( R3D_MAX( startX, 0 ), countXs[ L ] );

		//------------------------------------------------------------------------
		// bottom

		for( int z = z_bottom_prev, e = endZ; z < e; z ++ )
		{
			for( int x = x_left_prev, e = x_right_prev; x < e; x ++ )
			{
				int connFlags = 0;

				if( z == endZ - 1 && x >= endX && x < startX  )
				{
					connFlags |= SOUTH_CONNECTION;
				}

				AddToAllocatedTiles( x, z, L, connFlags & connMask );
			}
		}

		//------------------------------------------------------------------------
		// up

		for( int z = startZ, e = z_top_prev; z < e; z ++ )
		{
			for( int x = x_left_prev, e = x_right_prev; x < e; x ++ )
			{
				int connFlags = 0;

				if( z == startZ && x >= endX && x < startX )
				{
					connFlags |= NORTH_CONNECTION;
				}

				AddToAllocatedTiles( x, z, L, connFlags & connMask );
			}
		}

		//------------------------------------------------------------------------
		// left

		for( int z = z_bottom_prev, e = z_top_prev; z < e; z ++ )
		{
			for( int x = x_left_prev, e = endX; x < e; x ++ )
			{
				int connFlags = 0;

				if( x == endX - 1 && z >= endZ && z < startZ )
				{
					connFlags |= EAST_CONNECTION;
				}

				AddToAllocatedTiles( x, z, L, connFlags & connMask );
			}
		}

		//------------------------------------------------------------------------
		// right

		for( int z = z_bottom_prev, e = z_top_prev; z < e; z ++ )
		{
			for( int x = startX, e = x_right_prev; x < e; x ++ )
			{
				int connFlags = 0;

				if( x == startX && z >= endZ && z < startZ )
				{
					connFlags |= WEST_CONNECTION;
				}

				AddToAllocatedTiles( x, z, L, connFlags & connMask );
			}
		}

		z_bottom_prev = endZ * 2;
		z_top_prev = startZ * 2;

		x_left_prev	= endX * 2;
		x_right_prev = startX * 2;
	}

	// fill in highest lod in remaining tiles
	for( int z = z_bottom_prev, e = z_top_prev; z < e; z ++ )
	{
		for( int x = x_left_prev, e = x_right_prev; x < e; x ++ )
		{
			AddToAllocatedTiles( x, z, 0, 0 );
		}
	}

	R3DPROFILE_END( "Tile Activity" );

	//------------------------------------------------------------------------
	// sync activity with atlas tile allocation

	R3DPROFILE_START( "Render tiles" );

	m_TilesToUpdate.Clear();

	for( int L = 0, e = m_NumActiveQualityLayers; L < e; L ++ )
	{
		AllocatedTileArr& arr = m_AllocTileLodArray[ L ];

		for( int i = (int)arr.Count() - 1; i >= 0; i -- )
		{
			AllocatedTile& tile = arr[ i ];

			if( !tile.Tagged )
			{
				// tile.AtlasVolumeID >= 0 because it may have "refreshed" to -1 in editor etc.
				if( tile.AtlasVolumeID >= 0 )
				{
					FreeAtlasTile( &tile );
				}
				arr.Erase( i );
			}
		}

		for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
		{
			AllocatedTile& tile = arr[ i ];

			if( tile.AtlasTileID <= 0 )
			{
				AllocateAtlasTile( &tile );
				m_TilesToUpdate.PushBack( &tile );
			}
#if 0
			else
			{
				r3d_assert( tile.AtlasVolumeID >= 0 );
				m_TilesToUpdate.PushBack( &tile );
			}
#endif
		}
	}

	if( m_TilesToUpdate.Count() > 2 )
	{
		std::sort( &m_TilesToUpdate[ 0 ], &m_TilesToUpdate[ 0 ] + m_TilesToUpdate.Count() - 1, tile_comp_func_ATLAS_ID );
	}

	D3DPERF_BeginEvent( 0, L"r3dTerrain2::UpdateTiles" );

	StartTileUpdating();

	for( int i = 0, e = m_TilesToUpdate.Count(); i < e; i ++ )
	{
		const AllocatedTile* tile = m_TilesToUpdate[ i ];
		UpdateTileInAtlas( tile );
	}

	StopTileUpdating();

	D3DPERF_EndEvent();

	UpdateTileRoads();

	R3DPROFILE_END( "Render tiles" );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateTileMips()
{
#if R3D_TERRAIN_V2_GRAPHICS
#if GENERATE_MIPS
	StartMipChainsTileRendering();

	for( int i = 0, e = m_TilesWantMips.Count(); i < e; i ++ )
	{
		RenderTileMipChain( m_TilesWantMips[ i ] );
	}

	m_TilesWantMips.Clear();

	StopMipChainsTileRendering();
#endif
#endif
}

void r3dTerrain2::UpdateTileRoads()
{
#if R3D_TERRAIN_V2_GRAPHICS
	D3DPERF_BeginEvent( 0, L"r3dTerrain2::UpdateAtlasTileRoads" );
	// start and stop no matter what, this will prevent a pair of bugs
	StartTileRoadUpdating();

	if( m_RoadInfoMipChain.Count() && m_QualitySettings.BakeRoads )
	{
		for( int i = 0, e = m_TilesToUpdate.Count(); i < e; i ++ )
		{
			const AllocatedTile* tile = m_TilesToUpdate[ i ];
			UpdateAtlasTileRoads( tile );
		}
	}

	StopTileRoadUpdating();
	D3DPERF_EndEvent();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::AddAtlasVolume()
{
#if R3D_TERRAIN_V2_GRAPHICS
	AtlasVolume volume;

	int mipCount = MipCount( m_QualitySettings.AtlasTileDim ) - 2;
	// we're making 4x4 mip monocolor and ignore the rest, because otherwise we have to waste too
	// much extra space to allow smooth tile to tile transition

#if !GENERATE_MIPS
	mipCount = 1;
#endif

	volume.Diffuse = r3dScreenBuffer::CreateClass( "TERRAIN_ATLAS_DIFFUSE", ATLAS_TEXTURE_DIM, ATLAS_TEXTURE_DIM, ATLAS_FMT, r3dScreenBuffer::Z_NO_Z, 0, mipCount );
	volume.Normal = r3dScreenBuffer::CreateClass( "TERRAIN_ATLAS_NORMAL", ATLAS_TEXTURE_DIM, ATLAS_TEXTURE_DIM, ATLAS_FMT, r3dScreenBuffer::Z_NO_Z, 0, mipCount );

	volume.Occupied.Resize( m_AtlasTileCountPerSide * m_AtlasTileCountPerSide, 0 );

	volume.FreeTiles = m_AtlasTileCountPerSide * m_AtlasTileCountPerSide;

	m_Atlas.PushBack( volume );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::FreeAtlas()
{
	for( int i = 0, e = m_Atlas.Count(); i < e; i ++ )
	{
		SAFE_DELETE( m_Atlas[ i ].Diffuse );
		SAFE_DELETE( m_Atlas[ i ].Normal );
	}

	m_Atlas.Clear();
}

//------------------------------------------------------------------------

void
r3dTerrain2::AllocateAtlasTile( AllocatedTile* tile )
{
#if R3D_TERRAIN_V2_GRAPHICS
	for( int i = 0;; i ++ )
	{
		if( i >= (int)m_Atlas.Count() )
		{
			AddAtlasVolume();
		}

		AtlasVolume& vol = m_Atlas[ i ];

		if( !vol.FreeTiles )
			continue;

		for( int t = 0, e = vol.Occupied.Count(); t < e; t ++ )
		{
			if( !vol.Occupied[ t ] )
			{
				vol.FreeTiles --;
				vol.Occupied[ t ] = 1;
				tile->AtlasTileID = t;
				tile->AtlasVolumeID = i;
				m_TotalAllocatedTiles ++;
				m_MaxAllocatedTiles = R3D_MAX( m_MaxAllocatedTiles, m_TotalAllocatedTiles );
				return;
			}
		}
	}

	r3dError( "r3dTerrain2::AllocateAtlasTile: out of space!" );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::FreeAtlasTile( AllocatedTile* tile )
{
#if R3D_TERRAIN_V2_GRAPHICS
	r3d_assert( tile->AtlasTileID >= 0 && tile->AtlasVolumeID >= 0 );

#ifdef _DEBUG
	for( int i = 0, e = m_TilesToUpdate.Count(); i < e; i ++ )
	{
		r3d_assert( tile != m_TilesToUpdate[ i ] );
	}
#endif

	// 'damage control' in final build
	if( tile->AtlasVolumeID >= 0 )
	{
		m_Atlas[ tile->AtlasVolumeID ].Occupied[ tile->AtlasTileID ] = 0;
		m_Atlas[ tile->AtlasVolumeID ].FreeTiles ++;

		m_TotalAllocatedTiles --;
	}

	tile->AtlasVolumeID = -1;
	tile->AtlasTileID = -1;
#endif
}

//------------------------------------------------------------------------

int2
r3dTerrain2::GetTileAtlasXZ( const AllocatedTile* tile ) const
{
	int idInAtlas = tile->AtlasTileID;

	int x = idInAtlas % m_AtlasTileCountPerSide;
	int z = idInAtlas / m_AtlasTileCountPerSide;

	return int2( x, z );
}

//------------------------------------------------------------------------

void
r3dTerrain2::StartUsingTileGeom( bool forAtlas )
{
#if R3D_TERRAIN_V2_GRAPHICS
	m_TileVertexBuffer->Set( 0 );
	m_TileIndexBuffer->Set();

	if( !forAtlas && !m_AllowVFetch )
		r3dRenderer->SetVertexDecl( g_TerraDynaVDecl );
	else
		r3dRenderer->SetVertexDecl( g_TerraVDecl );

#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::StopUsingTileGeom()
{
#if R3D_TERRAIN_V2_GRAPHICS
	r3dRenderer->SetVertexDecl( R3D_MESH_VERTEX::getDecl() );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::SetupTileAtlasRTs( const AllocatedTile* tile )
{
#if R3D_TERRAIN_V2_GRAPHICS
	AtlasVolume& vol = m_Atlas[ tile->AtlasVolumeID ];

	if( vol.Diffuse != m_LastDiffuseRT )
	{
		if( m_LastDiffuseRT )
		{
			m_LastDiffuseRT->Deactivate( 0 );
			m_LastNormalRT->Deactivate( 0 );
		}

		vol.Diffuse->Activate( 0 );
		vol.Normal->Activate( 1 );

		m_LastDiffuseRT = vol.Diffuse;
		m_LastNormalRT = vol.Normal;
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::StartTileUpdating()
{
#if R3D_TERRAIN_V2_GRAPHICS
	StartUsingTileGeom( true );

	r3dRenderer->SetMipMapBias( 0.f );

	r3dRenderer->SetCullMode( D3DCULL_NONE );

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE ) );
	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) );

	//------------------------------------------------------------------------
	// sampler sColor                 : register ( s11 );
	{
		r3dRenderer->SetTex( m_ColorTex, 11 );
	}

	r3dRenderer->SetTex( m_NormalTex, 12 );

	//------------------------------------------------------------------------
	// setup WRAP for these:
	// sampler sBaseDiffuse           : register ( s0 );
	// sampler sBaseNormal            : register ( s1 );
	// sampler sDiffuse[ MAX_LAYERS ] : register ( s2 );
	// sampler sNormal[ MAX_LAYERS ]  : register ( s5 );

	for( int i = 0, e = 8; i < e; i ++ )
	{
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP ) );
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP ) );

		r3dSetFiltering( R3D_BILINEAR, i );
	}

	//------------------------------------------------------------------------
	// setup CLAMP for these
	// sampler sSplat[ MAX_LAYERS ]   : register ( s8 );
	// sampler sColor                 : register ( s11 );
	// sampler sVertexNormal          : register ( s12 );

	for( int i = 8, e = 13; i < e; i ++ )
	{
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ) );
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ) );

		r3dSetFiltering( R3D_BILINEAR, i );
	}

	//------------------------------------------------------------------------
	// setup vertex textures
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ) );

	r3dRenderer->SetTex( m_NormalTex, D3DVERTEXTEXTURESAMPLER0 );
	r3dSetFiltering( R3D_POINT, D3DVERTEXTEXTURESAMPLER0 );

	m_LastDiffuseRT = NULL;
	m_LastNormalRT = NULL;
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::SetAtlasTilePosTransform( const AllocatedTile* tile, float4* oConst )
{
#if R3D_TERRAIN_V2_GRAPHICS
	int2 atlasXZ = GetTileAtlasXZ( tile );

	oConst->x = m_AtlasTileCountPerSideInv;
	oConst->y = m_AtlasTileCountPerSideInv;
	oConst->z = ( atlasXZ.x + 0.5f ) * m_AtlasTileCountPerSideInv * 2.0f - 1.0f;
	oConst->w = ( atlasXZ.y + 0.5f ) * m_AtlasTileCountPerSideInv * 2.0f - 1.0f;
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::SetTileUpdateVSConsts( const AllocatedTile* tile, int passN, const IntArr& layerIndexes )
{
#if R3D_TERRAIN_V2_GRAPHICS
	r3dTL::TFixedArray< float4, 6 > vsConsts;

	float extraSpaceCoef = GetPaddingCoef();

	int L = tile->L;

	struct
	{
		void operator() ( float4* tarConst, const r3dTerrainLayer* layer, const AllocatedTile* tile, float tileDim, float extraSpaceCoef )
		{
			float multU = layer->ShaderScaleU * tileDim;
			float multV = layer->ShaderScaleV * tileDim;

			tarConst->x = multU * extraSpaceCoef;
			tarConst->y = multV * extraSpaceCoef;
			tarConst->z = multU + tile->X * tileDim * layer->ShaderScaleU * 2.0f;
			tarConst->w = multV + tile->Z * tileDim * layer->ShaderScaleV * 2.0f;
		}
	} setLayerConsts;

	struct
	{
		void operator() ( float4* tarConst )
		{
			tarConst->x = tarConst->y = tarConst->z = tarConst->w = 0.5f;
		}
	} setUnusedConst;

	//------------------------------------------------------------------------
	// float4 gToPos               : register( c0 );
	{
		SetAtlasTilePosTransform( tile, vsConsts + 0 );
	}

	//------------------------------------------------------------------------
	// float4 gToSplatTexc         : register( c1 );
	{
		float halfTileDim = m_HalfTileWorldDims[ L ];
		float tileDim = m_TileWorldDims[ L ];

		vsConsts[ 1 ].x = halfTileDim * m_TerrainPosToSplatU * 2.0f * extraSpaceCoef;
		vsConsts[ 1 ].y = halfTileDim * m_TerrainPosToSplatV * 2.0f * extraSpaceCoef;

		vsConsts[ 1 ].z = ( tile->X * tileDim + halfTileDim ) * m_TerrainPosToSplatU * 2.0f;
		vsConsts[ 1 ].w = ( tile->Z * tileDim + halfTileDim ) * m_TerrainPosToSplatV * 2.0f;

		if( m_Masks.Count() )
		{
			r3dTexture* firstMask = m_Masks[ 0 ];

			vsConsts[ 1 ].z += 0.5f / firstMask->GetWidth();
			vsConsts[ 1 ].w += 0.5f / firstMask->GetHeight();
		}
	}

	//------------------------------------------------------------------------
	// float4 gToBaseTileTexc      : register( c2 );
	{
		if( !passN )
		{
			setLayerConsts( vsConsts + 2, &m_BaseLayer, tile, m_TileWorldDims[ L ], extraSpaceCoef );
		}
		else
		{
			setUnusedConst( vsConsts + 2 );
		}
	}

	//------------------------------------------------------------------------

	int startLayer = passN * LAYERS_PER_MASK;
	int layerCount = R3D_MIN( int( layerIndexes.Count() - startLayer ), (int)LAYERS_PER_MASK );

	//------------------------------------------------------------------------
	// float4 gToTile0Texc         : register( c3 );
	// float4 gToTile1Texc         : register( c4 );
	// float4 gToTile2Texc         : register( c5 );

	for( int i = startLayer, c = 0, e = startLayer + layerCount; i < e; i ++, c ++ )
	{
		const r3dTerrainLayer* layer = &m_Layers[ layerIndexes[ i ] ];
		setLayerConsts( vsConsts + 3 + c, layer, tile, m_TileWorldDims[ L ], extraSpaceCoef );
	}

	for( int c = layerCount, e = 3; c < e; c ++ )
	{
		setUnusedConst( vsConsts + 3 + c );
	}

	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, &vsConsts[0].x, vsConsts.COUNT ) );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::SetTileUpdatePSConsts( const AllocatedTile* tile, int startLayer, const IntArr& layerIndexes )
{
#if R3D_TERRAIN_V2_GRAPHICS
	int layerCount = layerIndexes.Count() - startLayer;

	layerCount = R3D_MIN( layerCount , (int)LAYERS_PER_MASK );

	if( layerCount )
	{
		r3dTL::TFixedArray< float4, LAYERS_PER_MASK > psConsts;

		for( int i = 0, e = layerCount; i < e; i ++ )
		{
			psConsts[ i ] = m_Layers[ layerIndexes[ startLayer + i ] ].ChannelMask;
		}

		// float3 gSplatControls[3]    : register( c0 );
		r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, &psConsts[ 0 ].x, layerCount );
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::FillTempTileLayerArray( int x, int z, int L  )
{
	m_TempLayerIndexes.Clear();

	for( int i = 0, e = m_LayerBitMasksMipChains.Count(); i < e; i ++ )
	{
		LayerBitMaskMipChain& chain = m_LayerBitMasksMipChains[ i ];

		if( chain.Count() )
		{
			r3dBitMaskArray2D& arr = chain[ L ];
			if( arr.Get( x, z ) )
			{
				m_TempLayerIndexes.PushBack( i );
			}
		}
	}
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateTileInAtlas( const AllocatedTile* tile )
{
#if R3D_TERRAIN_V2_GRAPHICS
	int qlMultiplier = 1 << tile->L;

	FillTempTileLayerArray( tile->X, tile->Z, tile->L );

	int totalLayerCount = (int)m_TempLayerIndexes.Count();

	int passCount = totalLayerCount / LAYERS_PER_MASK;

	if( totalLayerCount % LAYERS_PER_MASK )
		passCount ++;

	if( !passCount )
		passCount = 1;

	int startLayer = 0;

	SetupTileAtlasRTs( tile );

	for( int i = 0, e = passCount; i < e; i ++, startLayer += LAYERS_PER_MASK )
	{
		int final = i == passCount - 1;

		int layerCount = R3D_MIN( (int)( totalLayerCount - LAYERS_PER_MASK * i), (int)LAYERS_PER_MASK );

		SetTileUpdateVSConsts( tile, i, m_TempLayerIndexes );

		// vertex shader
		{
			GenerateAtlasVertexShaderId vsid;
			vsid.unused = 0;

			r3dRenderer->SetValidVertexShader( g_AtlasVertexShaderIdMap[ vsid.Id ] );
		}

		SetTileUpdatePSConsts( tile, startLayer, m_TempLayerIndexes );

		// pixel shader
		{
			GenerateAtlasPixelShaderId psid;
			psid.numLayers = layerCount;
			psid.firstBatch = i == 0;

			int id = g_AtlasPixelShaderIdMap[ psid.Id ];
			r3d_assert( id >= 0 );

			r3dRenderer->SetPixelShader( id );
		}

		//------------------------------------------------------------------------
		// sampler sBaseDiffuse           : register ( s0 );
		// sampler sBaseNormal            : register ( s1 );
		{
			if( !i )
			{
				r3dRenderer->SetTex( m_BaseLayer.DiffuseTex, 0 );
				r3dRenderer->SetTex( m_BaseLayer.NormalTex, 1 );

				D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ) );
			}
			else
			{
				D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) );

				D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE ) );
				D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCALPHA ) );
			}
		}

		//------------------------------------------------------------------------
		// sampler sDiffuse[3]            : register ( s2 );
		// sampler sNormal[3]             : register ( s5 );
		{
			for( int i = startLayer, s = 0, e = startLayer + layerCount; i < e; i ++, s ++ )
			{
				const r3dTerrainLayer* layer = &m_Layers[ m_TempLayerIndexes [ i ] ];

				r3dRenderer->SetTex( layer->DiffuseTex, 2 + s );
				r3dRenderer->SetTex( layer->NormalTex, 5 + s );
			}
		}

		//------------------------------------------------------------------------
		// sampler sSplat[3]              : register ( s8 );
		{
			for( int i = startLayer, s = 0, e = startLayer + layerCount; i < e; i ++, s ++ )
			{
				const r3dTerrainLayer* layer = &m_Layers[ m_TempLayerIndexes [ i ] ];

				r3dRenderer->SetTex( layer->MaskTex, 8 + s );
			}
		}

		int density = m_QualitySettings.TileVertexDensities[ tile->L ];

		int primitiveCount = m_ConnectionIndexCounts[ density ][ 0 ];
		int indexOffset = m_ConnectionIndexOffsets[ density ][ 0 ];
		int vertexOffset = m_ConnectionVertexOffsets[ density ];

		r3dRenderer->Stats.AddNumTerrainDraws( 1 );
		r3dRenderer->Stats.AddNumTerrainTris( primitiveCount );
		r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, vertexOffset, 0, m_TileVertexBuffer->GetItemCount(), indexOffset, primitiveCount );
	}

	// modulate
	{
		GenerateAtlasPixelShaderId psid;

		psid.Id = 0;
		psid.modulationBatch = 1;

		int id = g_AtlasPixelShaderIdMap[ psid.Id ];

		r3d_assert( id >= 0 );

		r3dRenderer->SetPixelShader( id );

		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) );

		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO ) );
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR ) );

		r3dRenderer->Stats.AddNumTerrainDraws( 1 );
		r3dRenderer->Stats.AddNumTerrainTris( 2 );
		r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, 0, 0, ( m_QualitySettings.VertexTileDim + 1 ) * ( m_QualitySettings.VertexTileDim + 1 ), m_4VertTileIndexOffset, 2 );
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::StopTileUpdating()
{
#if R3D_TERRAIN_V2_GRAPHICS
	StopUsingTileGeom();
	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE ) );
	r3dRenderer->RestoreCullMode();

	FlushAtlasRTs();

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::FlushAtlasRTs()
{
#if R3D_TERRAIN_V2_GRAPHICS
	if(	m_LastDiffuseRT )
	{
		r3d_assert( m_LastNormalRT );

		m_LastDiffuseRT->Deactivate();
		m_LastNormalRT->Deactivate();
	}

	m_LastDiffuseRT = 0;
	m_LastNormalRT = 0;
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::StartTileRoadUpdating()
{
#if R3D_TERRAIN_V2_GRAPHICS
	m_RoadTileUpdateZFar = m_HeightOffset + m_HeightScale * 2.33f;

	r3dRenderer->SetValidVertexShader( VS_FILLGBUFFER_ID );
	r3dRenderer->SetPixelShader( g_RoadAtlasPixelShaderId );

	r3dRenderer->SetCullMode( D3DCULL_CCW );

	r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::SetupTileRoadUpdateCamera( const AllocatedTile* tile )
{
#if R3D_TERRAIN_V2_GRAPHICS
	float dim = m_TileWorldDims[ tile->L ];

	float tx = ( tile->X + 0.5f ) * dim;
	float tz = ( tile->Z + 0.5f ) * dim;

	D3DXMATRIX view;
	D3DXVECTOR3 eye = D3DXVECTOR3( tx, m_HeightOffset + m_HeightScale * 1.22f, tz );
	D3DXVECTOR3 at = D3DXVECTOR3( tx, m_HeightOffset, tz );
	D3DXVECTOR3 up = D3DXVECTOR3( 0, 0, 1 );

	D3DXMatrixLookAtLH( &view, &eye, &at, &up );

	D3DXMATRIX proj;

	float projDim = dim * GetPaddingCoef();

	float n = 0.01f;
	float f = m_RoadTileUpdateZFar;

	r3dRenderer->BuildMatrixOrthoLH( &proj, projDim, projDim, n, f );

	r3dRenderer->SetCameraEx( view, proj, 0.01f, m_RoadTileUpdateZFar, false );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::UpdateAtlasTileRoads( const AllocatedTile* tile )
{
#if R3D_TERRAIN_V2_GRAPHICS
	int tileVertSize = ( 1 << tile->L ) * m_QualitySettings.VertexTileDim;

	int sz = R3D_MAX( m_QualitySettings.RoadVertexDimStart / tileVertSize, 1 );

	int startRoadMip = m_NumActiveQualityLayers - m_RoadInfoMipChain.Count();

	int mipI = R3D_MAX( tile->L - startRoadMip, 0 );

	SetupTileRoadUpdateCamera( tile );

	SetupTileAtlasRTs( tile );

	if( RoadInfo* info = &m_RoadInfoMipChain[ mipI ][ tile->Z / sz ][ tile->X / sz ] )
	{
		int2 atlasXZ = GetTileAtlasXZ( tile );

		r3dRenderer->SetViewport( 	float( atlasXZ.x * m_QualitySettings.AtlasTileDim ),
									float( ATLAS_TEXTURE_DIM - ( atlasXZ.y + 1 ) * m_QualitySettings.AtlasTileDim ),
									float( m_QualitySettings.AtlasTileDim ),
									float( m_QualitySettings.AtlasTileDim )	);

		for(; info; )
		{
			for( int i = 0, e = ROAD_INFO_LENGTH - 1; i < e; i ++ )
			{
				obj_Road* obj = static_cast<obj_Road*> ( info->ptrs[ i ] );

				if( !obj )
					break;

#if R3D_TERRAIN_V2_GRAPHICS
				obj->DrawRoadToTerraAtlas();
#endif
			}

			info = static_cast< RoadInfo* >( info->ptrs[ ROAD_INFO_LENGTH - 1 ] );
		}
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::StopTileRoadUpdating()
{
#if R3D_TERRAIN_V2_GRAPHICS
	extern r3dCamera gCam;
	r3dRenderer->SetCamera( gCam, false );

	FlushAtlasRTs();

	r3dRenderer->RestoreCullMode();

	r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE ) );

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::StartMipChainsTileRendering()
{
#if R3D_TERRAIN_V2_GRAPHICS
	r3dRenderer->SetValidVertexShader( g_AtlasMipVSId );
	r3dRenderer->SetPixelShader( g_AtlasMipPSId );

	for( int i = 0, e = 2; i < e; i ++ )
	{
		r3dSetFiltering( R3D_BILINEAR, i );

		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ) );
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ) );
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_MIPFILTER, D3DTEXF_POINT ) );

		float bias = float( -11.0f );
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_MIPMAPLODBIAS, *(DWORD*)&bias ) );

	}

	m_UnitQuadVertexBuffer->Set( 0 );
	r3dRenderer->SetVertexDecl( g_TerraVDecl );
	r3dRenderer->SetCullMode( D3DCULL_NONE );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::RenderTileMipChain( const AllocatedTile* tile )
{
#if R3D_TERRAIN_V2_GRAPHICS
	D3DPERF_BeginEvent( 0, L"r3dTerrain2::RenderTileMipChain" );

	int2 atlasXZ = GetTileAtlasXZ( tile );

	const AtlasVolume& av = m_Atlas[ tile->AtlasVolumeID ];

	r3dScreenBuffer* atlasDiffuse = av.Diffuse;
	r3dScreenBuffer* atlasNormal = av.Normal;

	float mipMult = 1.0f;

	for( int i = 1, e = atlasDiffuse->ActualNumMipLevels; i < e; i ++, mipMult *= 2.0f )
	{
		const r3dScreenBuffer::Dims& dims = atlasDiffuse->MipDims[ i ];

		m_TempDiffuseRT->Activate( 0 );
		m_TempNormalRT->Activate( 1 );

		r3dRenderer->SetTex( atlasDiffuse->Tex, 0 );
		r3dRenderer->SetTex( atlasNormal->Tex, 1 );

		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_MAXMIPLEVEL, i - 1 ) );
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_MAXMIPLEVEL, i - 1 ) );

		r3dTL::TFixedArray< float4, 2 > vsConst;

		//------------------------------------------------------------------------
		// float4 gToPos               : register( c0 );

		float4 *vconst = vsConst + 0;

		vconst->x = dims.Width / atlasDiffuse->Width;
		vconst->y = dims.Height / atlasDiffuse->Height;
		vconst->z = -1.0f + vconst->x;
		vconst->w = +1.0f - vconst->y;

		//------------------------------------------------------------------------
		// float4 gToTexc              : register( c1 );

		vconst = vsConst + 1;

		SetAtlasTileTexcTransform( tile, vconst );

		vconst->z += mipMult / atlasDiffuse->Width;
		vconst->w += mipMult / atlasDiffuse->Height;

		D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, &vsConst[ 0 ].x, vsConst.COUNT ) );

		r3dRenderer->Stats.AddNumTerrainDraws( 1 );
		r3dRenderer->Stats.AddNumTerrainTris( 2 );
		r3dRenderer->Draw( D3DPT_TRIANGLESTRIP, 0, 2 );

		m_TempDiffuseRT->Deactivate( 0 );
		m_TempNormalRT->Deactivate( 0 );

		// copy mip
		{
			atlasDiffuse->Activate( 0, 0, i );
			atlasNormal->Activate( 1, 0, i );

			r3dRenderer->SetTex( m_TempDiffuseRT->Tex, 0 );
			r3dRenderer->SetTex( m_TempNormalRT->Tex, 1 );

			//------------------------------------------------------------------------
			// float4 gToPos               : register( c0 );

			vconst = vsConst + 0;
			SetAtlasTilePosTransform( tile, vconst );

			//------------------------------------------------------------------------
			// float4 gToTexc              : register( c1 );

			vconst = vsConst + 1;

			vconst->x = 0.25f / mipMult;
			vconst->y = -0.25f / mipMult;
			vconst->z = vconst->x;
			vconst->w = dims.Height / atlasDiffuse->Height + vconst->y;

			D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, &vsConst[ 0 ].x, vsConst.COUNT ) );

			r3dRenderer->Stats.AddNumTerrainDraws( 1 );
			r3dRenderer->Stats.AddNumTerrainTris( 2 );
			r3dRenderer->Draw( D3DPT_TRIANGLESTRIP, 0, 2 );

			atlasDiffuse->Deactivate( 0 );
			atlasNormal->Deactivate( 0 );
		}
	}

	D3DPERF_EndEvent();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::StopMipChainsTileRendering()
{
#if R3D_TERRAIN_V2_GRAPHICS
	for( int i = 0, e = 2; i < e; i ++ )
	{
		r3dSetFiltering( R3D_BILINEAR, i );

		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP ) );
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP ) );
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_MAXMIPLEVEL, 0 ) );
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_MIPMAPLODBIAS, 0 ) );
	}

	r3dRenderer->SetVertexDecl( R3D_MESH_VERTEX::getDecl() );
	r3dRenderer->RestoreCullMode();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::SetupTileRenderingVertexStates()
{
#if R3D_TERRAIN_V2_GRAPHICS
	StartUsingTileGeom( false );

	if( m_AllowVFetch )
	{
		r3dRenderer->SetTex( m_HeightTex, D3DVERTEXTEXTURESAMPLER0 );

		r3dSetFiltering( R3D_POINT, D3DVERTEXTEXTURESAMPLER0 );
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ) );
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ) );
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::StartTileRendering()
{
#if R3D_TERRAIN_V2_GRAPHICS
	SetupTileRenderingVertexStates();

	r3dRenderer->SetCullMode( D3DCULL_CCW );

	r3dSetAnisotropy( r_terrain2_anisotropy->GetInt(), 0 );
	r3dSetAnisotropy( r_terrain2_anisotropy->GetInt(), 1 );

	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ) );

	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ) );

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );

	// common pixel shader constants
	{
		r3dTL::TFixedArray< float4, 2 > psConsts;

		// float4 g_SpecularPow_DefSSAO    :  register( c0 );
		psConsts[ 0 ] = float4( m_Settings.Specular, r_ssao_clear_val->GetFloat(), 0.f, 0.f );

		// float4 g_Light                  :  register( c1 );
		extern r3dSun* Sun;
		psConsts[ 1 ] = float4( Sun->SunDir.x, Sun->SunDir.y, Sun->SunDir.z, 0.0f );

		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, &psConsts[ 0 ].x, psConsts.COUNT ) );
	}

	// Vertex Shader
	{
		Terrain2VertexShaderId vsid;
		vsid.shadowPath = 0;
		vsid.depthPath = 0;
		vsid.vfetchless = !m_AllowVFetch;

		r3dRenderer->SetValidVertexShader( g_Terrain2VertexShaderIdMap[ vsid.Id ] );
	}

	// Pixel Shader
	{
		Terrain2PixelShaderId psid;

		psid.aux = r_lighting_quality->GetInt() == 1 ? 0 : 1;
		psid.forward_lighting = r_in_minimap_render->GetInt() ?  1 : 0;

		r3dRenderer->SetPixelShader( g_Terrain2PixelShaderIdMap[ psid.Id ] );
	}

#ifndef FINAL_BUILD
	if( r_terra_wire->GetInt() )
	{
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_FILLMODE , D3DFILL_WIREFRAME ) );
	}
#endif
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::SetAtlasTileTexcTransform( const AllocatedTile* tile, float4* oTransform )
{
#if R3D_TERRAIN_V2_GRAPHICS
	int2 atlasXZ = GetTileAtlasXZ( tile );

	float antiBorder = m_QualitySettings.AtlasTileDim / float ( m_QualitySettings.AtlasTileDim + 2 * r_terrain2_padding->GetInt() );

	float antiBorderAddU = float( r_terrain2_padding->GetInt() ) / ATLAS_TEXTURE_DIM * antiBorder;
	float antiBorderAddV = float( r_terrain2_padding->GetInt() ) / ATLAS_TEXTURE_DIM * antiBorder;

	float uvScaleMul0 = m_AtlasTileCountPerSideInv * 0.5f ;
	float uvScaleMul1 = uvScaleMul0 * antiBorder;

	oTransform->x = uvScaleMul1;
	oTransform->y = -uvScaleMul1;

	oTransform->z = uvScaleMul1 + atlasXZ.x * m_AtlasTileCountPerSideInv + 0.5f / ATLAS_TEXTURE_DIM + antiBorderAddU;
	oTransform->w = 1.0f - uvScaleMul1 - atlasXZ.y * m_AtlasTileCountPerSideInv - 0.5f / ATLAS_TEXTURE_DIM - antiBorderAddV;
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::SetTileRenderVSConsts( const AllocatedTile* tile )
{
#if R3D_TERRAIN_V2_GRAPHICS
	r3dTL::TFixedArray< float4, 6 > vsConsts;

	float qlMult = float( 1 << tile->L );

	float halfTileDim = m_HalfTileWorldDims [ tile->L ];
	float tileDim = m_TileWorldDims[ tile->L ];

	//------------------------------------------------------------------------
	// float4x4 gWVP               : register( c0 );
	{
		float x = tile->X * tileDim - r3dRenderer->LocalizationPos.x;
		float z = tile->Z * tileDim - r3dRenderer->LocalizationPos.z;
		float y = - r3dRenderer->LocalizationPos.y;

		float dim = halfTileDim ;

		// fill in World
		D3DXMATRIX mtx (
				dim,		0.f,				0.f,		0.f,
				0.f,		m_HeightScale,		0.f,		0.f,
				0.f,		0.f,				dim,		0.f,
				dim + x,	m_HeightOffset + y,	dim + z,	1.f
			);

		D3DXMatrixMultiplyTranspose( (D3DXMATRIX*)&vsConsts[ 0 ], &mtx, &r3dRenderer->ViewProjMatrix_Localized );
	}

	//------------------------------------------------------------------------
	// float4 gToHeightTexc        : register( c4 );
	if( m_HeightTex )
	{
		float4* cnst = vsConsts + 4;

		cnst->x = halfTileDim * m_TerrainPosToSplatU * 2.0f;
		cnst->y = halfTileDim * m_TerrainPosToSplatV * 2.0f;

		cnst->z = ( tile->X * tileDim + halfTileDim ) * m_TerrainPosToSplatU * 2.0f;
		cnst->w = ( tile->Z * tileDim + halfTileDim ) * m_TerrainPosToSplatV * 2.0f;

		cnst->z += 0.5f / m_HeightTex->GetWidth();
		cnst->w += 0.5f / m_HeightTex->GetHeight();
	}
	else
	{
		vsConsts[ 4 ] = float4( 0, 0, 0, 0 );
	}

	//------------------------------------------------------------------------
	// float4 gToPSTexc            : register( c5 );
	{
		float4* cnst = vsConsts + 5;
		SetAtlasTileTexcTransform( tile, cnst );
	}

	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, &vsConsts[ 0 ].x, vsConsts.COUNT ) );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::SetDynaBufferFor( const AllocatedTile* tile, int density, int staticBufferOffset )
{
	m_TileVertexBuffer->Set( 0, staticBufferOffset );

	int sideCount = ( 1 << density ) * m_QualitySettings.VertexTileDim + 1;

	int count = sideCount * sideCount;

	int connSide = sideCount - 1;

	if( tile->ConFlags )
		count += connSide * 4;

	r3d_assert( count < m_DynamicVertexBuffer->GetItemCount() );

	if( m_DynamicVertexBufferPtr + count > m_DynamicVertexBuffer->GetItemCount() )
	{
		m_DynamicVertexBufferPtr = 0;
	}

	int tileSize = ( 1 << tile->L ) * m_QualitySettings.VertexTileDim;
	int step = tileSize / ( m_QualitySettings.VertexTileDim * ( 1 << density ) );

	r3dDynaTerraVertex* vtx = (r3dDynaTerraVertex*) m_DynamicVertexBuffer->Lock( m_DynamicVertexBufferPtr, count );

	int checker = 0;

	for( int z = tile->Z * tileSize, ze = ( tile->Z + 1 ) * tileSize; z <= ze; z += step )
	{
		for( int x = tile->X * tileSize, xe = ( tile->X + 1 ) * tileSize; x <= xe; x += step, vtx ++, checker ++ )
		{
			int ax = x;
			int az = z;

			if( ax >= m_VertexCountX )
				ax = m_VertexCountX - 1;

			if( az >= m_VertexCountZ )
			{
				az = m_VertexCountZ - 1;
			}

			vtx->height = m_HeightArr[ az * m_VertexCountX + ax ];
		}
	}

	if( tile->ConFlags )
	{
		int zoffset = tile->Z * tileSize * m_VertexCountX;
		int halfStep = step / 2;

		r3d_assert( halfStep );

		for( int x = tile->X * tileSize, xe = ( tile->X + 1 ) * tileSize; x < xe; x += step, vtx ++, checker ++ )
		{
			vtx->height = m_HeightArr[ zoffset + x + halfStep ];
		}

		int z_plus = ( tile->Z + 1 ) * tileSize;

		if( z_plus >= m_VertexCountZ )
			z_plus = m_VertexCountZ - 1;

		zoffset = z_plus * m_VertexCountX;

		for( int x = tile->X * tileSize, xe = ( tile->X + 1 ) * tileSize; x < xe; x += step, vtx ++, checker ++ )
		{
			vtx->height = m_HeightArr[ zoffset + x + halfStep ];
		}

		int xoffset = tile->X * tileSize;
		for( int z = tile->Z * tileSize, ze = ( tile->Z + 1 ) * tileSize; z < ze; z += step, vtx ++, checker ++ )
		{
			vtx->height = m_HeightArr[ ( z + halfStep ) * m_VertexCountX + xoffset ];
		}

		xoffset = ( tile->X + 1 ) * tileSize;

		if( xoffset >= m_VertexCountX )
			xoffset = m_VertexCountX - 1;

		for( int z = tile->Z * tileSize, ze = ( tile->Z + 1 ) * tileSize; z < ze; z += step, vtx ++, checker ++ )
		{
			vtx->height = m_HeightArr[ ( z + halfStep ) * m_VertexCountX + xoffset ];
		}
	}

	r3d_assert( count == checker );

	m_DynamicVertexBuffer->Unlock();

	m_DynamicVertexBuffer->Set( 1, m_DynamicVertexBufferPtr );

	m_DynamicVertexBufferPtr += count;
}

//------------------------------------------------------------------------

void
r3dTerrain2::RenderTile( const AllocatedTile* tile )
{
#if R3D_TERRAIN_V2_GRAPHICS
	SetTileRenderVSConsts( tile );

	const AtlasVolume& vol = m_Atlas[ tile->AtlasVolumeID ];

	r3dRenderer->SetTex( vol.Diffuse->Tex, 0 );
	r3dRenderer->SetTex( vol.Normal->Tex, 1 );

	int connFlags = tile->ConFlags;

	int density = m_QualitySettings.TileVertexDensities[ tile->L ];

	int primitiveCount = m_ConnectionIndexCounts[ density ][ connFlags ];
	int indexOffset = m_ConnectionIndexOffsets[ density ][ connFlags ];
	int vertexOffset = m_ConnectionVertexOffsets[ density ];

	if( !m_AllowVFetch )
	{
		SetDynaBufferFor( tile, density, vertexOffset );
		vertexOffset = 0;
	}

	r3dRenderer->Stats.AddNumTerrainDraws( 1 );
	r3dRenderer->Stats.AddNumTerrainTris( primitiveCount );
	r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, vertexOffset, 0, m_TileVertexBuffer->GetItemCount(), indexOffset, primitiveCount );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::EndTileRendering()
{
#if R3D_TERRAIN_V2_GRAPHICS
	r3dRenderer->RestoreCullMode();
	StopUsingTileGeom()	;

#ifndef FINAL_BUILD
	if( r_terra_wire->GetInt() )
	{
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_FILLMODE , D3DFILL_SOLID ) );
	}
#endif

	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP ) );

	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP ) );

	// restore default max anisotropy
	r3dSetDefaultMaxAnisotropy();
	r3dSetFiltering( R3D_ANISOTROPIC, 0 );
	r3dSetFiltering( R3D_ANISOTROPIC, 1 );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::StartShadowRender()
{
#if R3D_TERRAIN_V2_GRAPHICS
	SetupTileRenderingVertexStates();

	// Vertex Shader
	{
		Terrain2VertexShaderId vsid;
		vsid.shadowPath = 1;
		vsid.depthPath = 0;
		vsid.vfetchless = !m_AllowVFetch;
		vsid.recticular_warp = r3dRenderer->ShadowPassType == SPT_ORTHO_WARPED;

		r3dRenderer->SetValidVertexShader( g_Terrain2VertexShaderIdMap[ vsid.Id ] );
	}

	// Pixel Shader
	{
		r3dRenderer->SetPixelShader( g_Terrain2ShadowPSId );
	}

	r3dRenderer->SetCullMode( D3DCULL_CW );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::RenderShadowTile( const AllocatedTile* tile )
{
#if R3D_TERRAIN_V2_GRAPHICS
	SetTileRenderVSConsts( tile );

	int connFlags = tile->ConFlags;

	int density = m_QualitySettings.TileVertexDensities[ tile->L ];

	int primitiveCount = m_ConnectionIndexCounts[ density ][ connFlags ];
	int indexOffset = m_ConnectionIndexOffsets[ density ][ connFlags ];
	int vertexOffset = m_ConnectionVertexOffsets[ density ];

	if( !m_AllowVFetch )
	{
		SetDynaBufferFor( tile, density, vertexOffset );
		vertexOffset = 0;
	}

	r3dRenderer->Stats.AddNumTerrainDraws( 1 );
	r3dRenderer->Stats.AddNumTerrainTris( primitiveCount );
	r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, vertexOffset, 0, m_TileVertexBuffer->GetItemCount(), indexOffset, primitiveCount );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::StopShadowRender()
{
#if R3D_TERRAIN_V2_GRAPHICS
	r3dRenderer->RestoreCullMode();
	StopUsingTileGeom()	;
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::StartDepthRender()
{
#if R3D_TERRAIN_V2_GRAPHICS
	SetupTileRenderingVertexStates();

	// Vertex Shader // EDITADO
	{
		Terrain2VertexShaderId vsid;
		vsid.shadowPath = 0;
		vsid.depthPath = 1;
		vsid.vfetchless = !m_AllowVFetch;

		r3dRenderer->SetValidVertexShader( g_Terrain2VertexShaderIdMap[ vsid.Id ] );
	}

	r3dRenderer->SetCullMode( D3DCULL_CCW );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::RenderDepthTile( const AllocatedTile* tile )
{
#if R3D_TERRAIN_V2_GRAPHICS
	SetTileRenderVSConsts( tile );

	int connFlags = tile->ConFlags;

	int density = m_QualitySettings.TileVertexDensities[ tile->L ];

	int primitiveCount = m_ConnectionIndexCounts[ density ][ connFlags ];
	int indexOffset = m_ConnectionIndexOffsets[ density ][ connFlags ];
	int vertexOffset = m_ConnectionVertexOffsets[ density ];

	r3dRenderer->Stats.AddNumTerrainDraws( 1 );
	r3dRenderer->Stats.AddNumTerrainTris( primitiveCount );
	r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, vertexOffset, 0, m_TileVertexBuffer->GetItemCount(), indexOffset, primitiveCount );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::StopDepthRender()
{
#if R3D_TERRAIN_V2_GRAPHICS
	r3dRenderer->RestoreCullMode();
	StopUsingTileGeom()	;
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::GetBaseTileCounts( int* oBaseTileCountX, int* oBaseTileCountZ )
{
	*oBaseTileCountX = m_VertexCountX / QualitySettings::MIN_VERTEX_TILE_DIM;
	*oBaseTileCountZ = m_VertexCountZ / QualitySettings::MIN_VERTEX_TILE_DIM;
}

//------------------------------------------------------------------------

void
r3dTerrain2::GetBaseTileCounts_101( int* oBaseTileCountX, int* oBaseTileCountZ )
{
	*oBaseTileCountX = m_VertexCountX / QualitySettings::MIN_VERTEX_TILE_DIM_V101;
	*oBaseTileCountZ = m_VertexCountZ / QualitySettings::MIN_VERTEX_TILE_DIM_V101;
}

//------------------------------------------------------------------------

void
r3dTerrain2::FreeAtlasTiles()
{
#if R3D_TERRAIN_V2_GRAPHICS
	for( int i = 0, e = (int)m_AllocTileLodArray.Count(); i < e; i ++ )
	{
		AllocatedTileArr& arr = m_AllocTileLodArray[ i ];

		for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
		{
			AllocatedTile& tile = arr[ i ];

			if( tile.AtlasVolumeID >= 0 )
			{
				FreeAtlasTile( &tile );
			}
		}
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::RemoveAllocatedTiles()
{
#if R3D_TERRAIN_V2_GRAPHICS
	m_TilesToUpdate.Clear();
	FreeAtlasTiles();

	for( int i = 0, e = (int)m_AllocTileLodArray.Count(); i < e; i ++ )
	{
		m_AllocTileLodArray[ i ].Clear();
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain2::DoUpdateDominantLayerData( int baseWidth, int baseHeight )
{
#if R3D_TERRAIN_V2_GRAPHICS

	r3d_assert( !m_LayerChannels.Count() || m_LayerChannels[ 0 ].Count() == baseWidth * baseHeight );

	m_DominantLayerData.Clear();
	m_DominantLayerData.Resize( baseWidth, baseHeight, 0 );

	for( int L = 0, e = m_LayerChannels.Count(); L < e; L ++ )
	{
		const LayerChannel& channel = m_LayerChannels[ L ];

		for( int z = 0, e = baseHeight; z < e; z ++ )
		{
			for( int x = 0, e = baseWidth; x < e; x ++ )
			{
				float val = channel[ x + z * baseWidth ];
				if( val > 127 )
				{
					// 0 is base layer
					m_DominantLayerData[ z ][ x ] = L + 1;
				}
			}
		}
	}
#endif
}

//------------------------------------------------------------------------

static r3dTerrain2::QualitySettings g_EmptyQS;

const r3dTerrain2::QualitySettings&
r3dTerrain2::SelectQualitySettings()
{
	switch( r_terrain_quality->GetInt() )
	{
	case 1:
		return m_BaseQualitySettings[ QS_LOW ];
	case 2:
		return m_BaseQualitySettings[ QS_MED ];
	case 3:
		return m_BaseQualitySettings[ QS_HIGH ];
	}

	r3dError( "r3dTerrain2::SelectQualitySettings: quality settings out of range!" );
	return g_EmptyQS;
}

//------------------------------------------------------------------------

void
r3dTerrain2::PopulateInferiorQualitySettings()
{
	m_BaseQualitySettings[ QS_MED ] = m_BaseQualitySettings[ QS_HIGH ];
	m_BaseQualitySettings[ QS_LOW ] = m_BaseQualitySettings[ QS_HIGH ];

	// Low QS
	{
		QualitySettings& newQS = m_BaseQualitySettings[ QS_LOW ];

		newQS.VertexTileDim *= 2;
		newQS.AtlasTileDim /= 4;

		for( int i = 0,  e = NUM_QUALITY_LAYERS; i < e; i ++ )
		{
			newQS.TileDistances[ i ] /= 2;
		}

		newQS.VertexDensity /= 4;
	}

	// Med QS
	{
		QualitySettings& newQS = m_BaseQualitySettings[ QS_MED ];

		newQS.VertexDensity /= 2;
		newQS.AtlasTileDim /= 2;
	}
}

//------------------------------------------------------------------------

void r3dTerrain2::UpdateEditor()
{
	if( g_bEditMode )
	{
		float newTime = r3dGetTime();
		float dt = newTime - gSaveCountDownLastTime;

		gSaveCountDownLastTime = newTime;

		if( !HasTextureReloadListener( TerraTextureReloadListener ) )
		{
			gSaveCountDown -= dt;
			if( gSaveCountDown <= 0 )
				AddTextureReloadListener( TerraTextureReloadListener );
		}
	}
}

//------------------------------------------------------------------------

struct LoadTextureParams
{
	r3dTexture** tex;
	int desiredWidth;
	int desiredHeight;
	D3DFORMAT desiredFmt;
};

void LoadTexInMainThread( void* vparams )
{
	LoadTextureParams* params = static_cast<LoadTextureParams*> ( vparams );

	ID3DXBuffer * buffer( NULL );

	D3D_V( D3DXSaveTextureToFileInMemory( &buffer, D3DXIFF_DDS, (*params->tex)->AsTex2D(), NULL ) );

	r3dRenderer->DeleteTexture( *params->tex );
	*params->tex = NULL;


	r3dD3DTextureTunnel tunnel;

	// NOTE : for game, we use system mem - because we only need it to lock & merge with terrain normals.
	// for editor, we use managed - to be able to lock & read + to be able to display it in UI...
	r3dDeviceTunnel::D3DXCreateTextureFromFileInMemoryEx(
							buffer->GetBufferPointer(),
								buffer->GetBufferSize(),
									params->desiredWidth,
										params->desiredHeight,
											1, 0, params->desiredFmt, g_bEditMode ? D3DPOOL_MANAGED : D3DPOOL_SYSTEMMEM, D3DX_FILTER_LINEAR, D3DX_FILTER_NONE,
												0, 0, 0, &tunnel, "Terrain2NormalDetail" );

	*params->tex = r3dRenderer->AllocateTexture();

	(**params->tex).Setup( params->desiredWidth, params->desiredHeight, 1, params->desiredFmt, 1, &tunnel, false );

	SAFE_RELEASE( buffer );

}

void r3dTerrain2::LoadDetailNormalTex()
{
	if( m_DetailNormalTex )
	{
		r3dRenderer->DeleteTexture( m_DetailNormalTex );
		m_DetailNormalTex = NULL;
	}

	if( r3dFileExists( m_DetailNormalTexSrc.c_str() ) )
	{
		m_DetailNormalTex = r3dRenderer->LoadTexture( m_DetailNormalTexSrc.c_str(), D3DFMT_R5G6B5 );

		if( m_DetailNormalTex->GetWidth() != m_VertexCountX * m_NormalDensity
				||
			m_DetailNormalTex->GetHeight() != m_VertexCountZ * m_NormalDensity
				)
		{
			LoadTextureParams params;

			params.desiredWidth = m_VertexCountX * m_NormalDensity;
			params.desiredHeight = m_VertexCountZ * m_NormalDensity;
			params.desiredFmt = D3DFMT_R5G6B5;
			params.tex = &m_DetailNormalTex;

			ProcessCustomDeviceQueueItem( LoadTexInMainThread, &params );
		}
	}

}

//------------------------------------------------------------------------

void
r3dTerrain2::ConformNormalTex()
{
	if( m_NormalTex->GetWidth() != m_VertexCountX * m_NormalDensity
			||
		m_NormalTex->GetHeight() != m_VertexCountZ * m_NormalDensity
		)
	{
		LoadTextureParams params;

		params.desiredWidth = m_VertexCountX * m_NormalDensity;
		params.desiredHeight = m_VertexCountZ * m_NormalDensity;
		params.desiredFmt = D3DFMT_R5G6B5;
		params.tex = &m_NormalTex;

		ProcessCustomDeviceQueueItem( LoadTexInMainThread, &params );
	}
}

//------------------------------------------------------------------------

r3dTerrain2* Terrain2;

//------------------------------------------------------------------------

#if R3D_TERRAIN_V2_GRAPHICS

static int CountConnectionIndices( int vertexTileDim, int sideLodConnections )
{
	int total;

	if( vertexTileDim == 1 )
	{
		switch( sideLodConnections )
		{
		case 0:
			total = 2;
			break;
		case NORTH_CONNECTION:
		case EAST_CONNECTION:
		case SOUTH_CONNECTION:
		case WEST_CONNECTION:
			total = 3;
			break;
		case NORTH_CONNECTION | EAST_CONNECTION:
			total = 4;
			break;
		case NORTH_CONNECTION | SOUTH_CONNECTION:
			total = 4;
			break;
		case EAST_CONNECTION | SOUTH_CONNECTION:
			total = 4;
			break;
		case NORTH_CONNECTION | EAST_CONNECTION | SOUTH_CONNECTION:
			total = 5;
			break;
		case NORTH_CONNECTION | WEST_CONNECTION:
			total = 4;
			break;

		case EAST_CONNECTION | WEST_CONNECTION:
			total = 4;
			break;

		case NORTH_CONNECTION | EAST_CONNECTION | WEST_CONNECTION:
			total = 5;
			break;

		case SOUTH_CONNECTION | WEST_CONNECTION:
			total = 4;
			break;

		case NORTH_CONNECTION | SOUTH_CONNECTION | WEST_CONNECTION:
			total = 5;
			break;

		case EAST_CONNECTION | SOUTH_CONNECTION | WEST_CONNECTION:
			total = 5;
			break;

		case NORTH_CONNECTION | EAST_CONNECTION | SOUTH_CONNECTION | WEST_CONNECTION:
			total = 6;
			break;

		default:
			total = 2;
			break;
		}
	}
	else
	{
		total = ( vertexTileDim - 2 ) * ( vertexTileDim - 2 ) * 2;

		int ratio = 2;

		int connectCount = ratio * 2 + ( ratio + 1 ) * ( vertexTileDim - 2 );
		int looseCount = ( vertexTileDim - 2 ) * 2 + 2;

		if( sideLodConnections & NORTH_CONNECTION )	total += connectCount;
		else total += looseCount;

		if( sideLodConnections & EAST_CONNECTION )	total += connectCount;
		else total += looseCount;

		if( sideLodConnections & SOUTH_CONNECTION )	total += connectCount;
		else total += looseCount;

		if( sideLodConnections & WEST_CONNECTION )	total += connectCount;
		else total += looseCount;
	}

	return total * 3;
}

static int ConstructConnectionIndices( uint16_t* target, int vertexTileDim, int sideLodConnections )
{
	uint16_t* start = target;

	const int CELL_VERT_SIZE = vertexTileDim + 1;

	const int TO_NEXT_LINE	= CELL_VERT_SIZE;

	int northVertStart	= ( vertexTileDim + 1 ) * ( vertexTileDim + 1 );
	int southVertStart	= northVertStart	+ vertexTileDim;
	int westVertStart	= southVertStart	+ vertexTileDim;
	int eastVertStart	= westVertStart		+ vertexTileDim;

	if( vertexTileDim == 1 )
	{
		int vidx = 0;

		switch( sideLodConnections )
		{
		case 0:
			*target ++ = vidx;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + 1;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + TO_NEXT_LINE + 1;
			*target ++ = vidx + 1;

			break;

		case NORTH_CONNECTION:
			*target ++ = vidx;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = northVertStart;

			*target ++ = northVertStart;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + 1;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + TO_NEXT_LINE + 1;
			*target ++ = vidx + 1;
			break;

		case EAST_CONNECTION:
			*target ++ = vidx;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + 1;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = eastVertStart;
			*target ++ = vidx + 1;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + TO_NEXT_LINE + 1;
			*target ++ = eastVertStart;
			break;

		case NORTH_CONNECTION | EAST_CONNECTION:
			*target ++ = vidx;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = northVertStart;

			*target ++ = northVertStart;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + 1;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = eastVertStart;
			*target ++ = vidx + 1;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + TO_NEXT_LINE + 1;
			*target ++ = eastVertStart;

			break;

		case SOUTH_CONNECTION:
			*target ++ = vidx;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + 1;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = southVertStart;
			*target ++ = vidx + 1;

			*target ++ = southVertStart;
			*target ++ = vidx + TO_NEXT_LINE + 1;
			*target ++ = vidx + 1;

			break;

		case NORTH_CONNECTION | SOUTH_CONNECTION:
			*target ++ = vidx;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = northVertStart;

			*target ++ = northVertStart;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + 1;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = southVertStart;
			*target ++ = vidx + 1;

			*target ++ = southVertStart;
			*target ++ = vidx + TO_NEXT_LINE + 1;
			*target ++ = vidx + 1;
			break;

		case EAST_CONNECTION | SOUTH_CONNECTION:
			*target ++ = vidx;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + 1;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = southVertStart;
			*target ++ = vidx + 1;

			*target ++ = vidx + 1;
			*target ++ = southVertStart;
			*target ++ = eastVertStart;

			*target ++ = southVertStart;
			*target ++ = vidx + TO_NEXT_LINE + 1;
			*target ++ = eastVertStart;

			break;

		case NORTH_CONNECTION | EAST_CONNECTION | SOUTH_CONNECTION:
			*target ++ = vidx;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = northVertStart;

			*target ++ = northVertStart;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + 1;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = southVertStart;
			*target ++ = vidx + 1;

			*target ++ = vidx + 1;
			*target ++ = southVertStart;
			*target ++ = eastVertStart;

			*target ++ = southVertStart;
			*target ++ = vidx + TO_NEXT_LINE + 1;
			*target ++ = eastVertStart;

			break;

		case WEST_CONNECTION:
			*target ++ = vidx;
			*target ++ = westVertStart;
			*target ++ = vidx + 1;

			*target ++ = vidx + 1;
			*target ++ = westVertStart;
			*target ++ = vidx + TO_NEXT_LINE;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + TO_NEXT_LINE + 1;
			*target ++ = vidx + 1;

			break;

		case NORTH_CONNECTION | WEST_CONNECTION :
			*target ++ = vidx;
			*target ++ = westVertStart;
			*target ++ = northVertStart;

			*target ++ = northVertStart;
			*target ++ = westVertStart;
			*target ++ = vidx + TO_NEXT_LINE;

			*target ++ = northVertStart;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + 1;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + TO_NEXT_LINE + 1;
			*target ++ = vidx + 1;
			break;

		case EAST_CONNECTION | WEST_CONNECTION:
			*target ++ = vidx;
			*target ++ = westVertStart;
			*target ++ = vidx + 1;

			*target ++ = vidx + 1;
			*target ++ = westVertStart;
			*target ++ = vidx + TO_NEXT_LINE;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = eastVertStart;
			*target ++ = vidx + 1;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + TO_NEXT_LINE + 1;
			*target ++ = eastVertStart;

			break;

		case NORTH_CONNECTION | EAST_CONNECTION | WEST_CONNECTION:
			*target ++ = vidx;
			*target ++ = westVertStart;
			*target ++ = northVertStart;

			*target ++ = northVertStart;
			*target ++ = westVertStart;
			*target ++ = vidx + TO_NEXT_LINE;

			*target ++ = northVertStart;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + 1;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = eastVertStart;
			*target ++ = vidx + 1;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + TO_NEXT_LINE + 1;
			*target ++ = eastVertStart;

			break;

		case SOUTH_CONNECTION | WEST_CONNECTION:
			*target ++ = vidx;
			*target ++ = westVertStart;
			*target ++ = vidx + 1;

			*target ++ = vidx + 1;
			*target ++ = westVertStart;
			*target ++ = vidx + TO_NEXT_LINE;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = southVertStart;
			*target ++ = vidx + 1;

			*target ++ = southVertStart;
			*target ++ = vidx + TO_NEXT_LINE + 1;
			*target ++ = vidx + 1;

			break;

		case NORTH_CONNECTION | SOUTH_CONNECTION | WEST_CONNECTION:
			*target ++ = vidx;
			*target ++ = westVertStart;
			*target ++ = northVertStart;

			*target ++ = northVertStart;
			*target ++ = westVertStart;
			*target ++ = vidx + TO_NEXT_LINE;

			*target ++ = northVertStart;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + 1;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = southVertStart;
			*target ++ = vidx + 1;

			*target ++ = southVertStart;
			*target ++ = vidx + TO_NEXT_LINE + 1;
			*target ++ = vidx + 1;

			break;

		case EAST_CONNECTION | SOUTH_CONNECTION | WEST_CONNECTION:

			*target ++ = vidx;
			*target ++ = westVertStart;
			*target ++ = vidx + 1;

			*target ++ = vidx + 1;
			*target ++ = westVertStart;
			*target ++ = vidx + TO_NEXT_LINE;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = southVertStart;
			*target ++ = vidx + 1;

			*target ++ = vidx + 1;
			*target ++ = southVertStart;
			*target ++ = eastVertStart;

			*target ++ = southVertStart;
			*target ++ = vidx + TO_NEXT_LINE + 1;
			*target ++ = eastVertStart;

			break;

		case NORTH_CONNECTION | EAST_CONNECTION | SOUTH_CONNECTION | WEST_CONNECTION:
			*target ++ = vidx;
			*target ++ = westVertStart;
			*target ++ = northVertStart;

			*target ++ = northVertStart;
			*target ++ = westVertStart;
			*target ++ = vidx + TO_NEXT_LINE;

			*target ++ = northVertStart;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + 1;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = southVertStart;
			*target ++ = vidx + 1;

			*target ++ = vidx + 1;
			*target ++ = southVertStart;
			*target ++ = eastVertStart;

			*target ++ = southVertStart;
			*target ++ = vidx + TO_NEXT_LINE + 1;
			*target ++ = eastVertStart;

			break;

		default:
			*target ++ = vidx;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + 1;

			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + TO_NEXT_LINE + 1;
			*target ++ = vidx + 1;

			break;
		}
	}
	else
	{
		bool NorthAndWest		= sideLodConnections & NORTH_CONNECTION && sideLodConnections & WEST_CONNECTION;
		bool NotNorthAndNotWest	= !(sideLodConnections & NORTH_CONNECTION) && !(sideLodConnections & WEST_CONNECTION);

		bool SouthAndEast		= sideLodConnections & SOUTH_CONNECTION && sideLodConnections & EAST_CONNECTION;
		bool NotSouthAndNotEast	= !(sideLodConnections & SOUTH_CONNECTION) && !(sideLodConnections & EAST_CONNECTION);

		// inside
		{
			int vidx = TO_NEXT_LINE + 1;

			for( uint32_t i = 1, e = vertexTileDim - 1; i < e; i += 1 )
			{
				for( uint32_t j = 1, e = vertexTileDim - 1; j < e; j += 1, vidx += 1 )
				{
					*target ++ = vidx;
					*target ++ = vidx + TO_NEXT_LINE;
					*target ++ = vidx + 1;

					*target ++ = vidx + TO_NEXT_LINE;
					*target ++ = vidx + TO_NEXT_LINE + 1;
					*target ++ = vidx + 1;
				}

				vidx += 3;
			}
		}

		// construct north connection
		if( sideLodConnections & NORTH_CONNECTION )
		{
			int vidx = 0;

			// exterior 0

			if( !NorthAndWest )
			{
				*target ++ = vidx;
				*target ++ = vidx + TO_NEXT_LINE;
				*target ++ = northVertStart;

				*target ++ = northVertStart;
				*target ++ = vidx + TO_NEXT_LINE;
				*target ++ = vidx + 1;
			}

			vidx = 1;

			// interior
			for( int i = 1, e = vertexTileDim - 1; i < e; i ++, vidx ++ )
			{
				*target ++ = vidx;
				*target ++ = vidx + TO_NEXT_LINE;
				*target ++ = northVertStart + i;

				*target ++ = northVertStart + i;
				*target ++ = vidx + TO_NEXT_LINE;
				*target ++ = vidx + 1;

				*target ++ = vidx + 1;
				*target ++ = vidx + TO_NEXT_LINE;
				*target ++ = vidx + TO_NEXT_LINE + 1;
			}

			// exterior 1

			*target ++ = vidx;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = northVertStart + vertexTileDim - 1;

			*target ++ = northVertStart + vertexTileDim - 1;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + 1;
		}
		else
		{
			int vidx = 0;

			// exterior 0
			if( !NotNorthAndNotWest )
			{
				*target ++ = vidx + TO_NEXT_LINE;
				*target ++ = vidx + TO_NEXT_LINE + 1;
				*target ++ = vidx + 1;
			}

			vidx += 1;

			// interior
			for( int i = 1; i < vertexTileDim - 1; i += 1, vidx += 1 )
			{
				*target ++ = vidx;
				*target ++ = vidx + TO_NEXT_LINE;
				*target ++ = vidx + 1;

				*target ++ = vidx + TO_NEXT_LINE;
				*target ++ = vidx + TO_NEXT_LINE + 1;
				*target ++ = vidx + 1;
			}

			// exterior 1
			*target ++ = vidx;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + 1;
		}

		// construct east connection
		if( sideLodConnections & EAST_CONNECTION )
		{
			int vidx = CELL_VERT_SIZE - 1;

			// exterior 0
			{
				*target ++ = vidx;
				*target ++ = vidx + TO_NEXT_LINE - 1;
				*target ++ = eastVertStart;

				*target ++ = eastVertStart;
				*target ++ = vidx + TO_NEXT_LINE - 1;
				*target ++ = vidx + TO_NEXT_LINE;
			}

			vidx += TO_NEXT_LINE;

			// interior
			for( int i = 1, e = vertexTileDim - 1; i < e; i += 1, vidx += TO_NEXT_LINE )
			{
				*target ++ = vidx - 1;
				*target ++ = vidx + TO_NEXT_LINE - 1;
				*target ++ = vidx;

				*target ++ = vidx;
				*target ++ = vidx + TO_NEXT_LINE - 1;
				*target ++ = eastVertStart + i;

				*target ++ = eastVertStart + i;
				*target ++ = vidx + TO_NEXT_LINE - 1;
				*target ++ = vidx + TO_NEXT_LINE;
			}

			// exterior 1

			if( !SouthAndEast )
			{
				*target ++ = vidx;
				*target ++ = vidx - 1 + TO_NEXT_LINE;
				*target ++ = eastVertStart + vertexTileDim - 1;

				*target ++ = eastVertStart + vertexTileDim - 1;
				*target ++ = vidx - 1 + TO_NEXT_LINE;
				*target ++ = vidx + TO_NEXT_LINE;
			}
		}
		else
		{
			int vidx = CELL_VERT_SIZE - 1;

			*target ++ = vidx;
			*target ++ = vidx + TO_NEXT_LINE - 1;
			*target ++ = vidx + TO_NEXT_LINE;

			vidx += TO_NEXT_LINE;

			// interior
			for( int i = 1; i < vertexTileDim - 1; i ++, vidx += TO_NEXT_LINE )
			{
				*target ++ = vidx - 1;
				*target ++ = vidx + TO_NEXT_LINE - 1;
				*target ++ = vidx;

				*target ++ = vidx + TO_NEXT_LINE - 1;
				*target ++ = vidx + TO_NEXT_LINE;
				*target ++ = vidx;
			}

			*target ++ = vidx - 1;
			*target ++ = vidx + TO_NEXT_LINE - 1;
			*target ++ = vidx;
		}


		// construct south connection
		if( sideLodConnections & SOUTH_CONNECTION )
		{
			int vidx = vertexTileDim * CELL_VERT_SIZE;

			// exterior 0
			{
				*target ++ = vidx ;
				*target ++ = southVertStart;
				*target ++ = vidx - TO_NEXT_LINE + 1;

				*target ++ = southVertStart;
				*target ++ = vidx + 1;
				*target ++ = vidx - TO_NEXT_LINE + 1;
			}

			vidx += 1;

			// interior
			for( int i = 1, e = vertexTileDim - 1; i < e; i ++, vidx ++ )
			{
				*target ++ = vidx;
				*target ++ = vidx - TO_NEXT_LINE + 1;
				*target ++ = vidx - TO_NEXT_LINE;

				*target ++ = vidx;
				*target ++ = southVertStart + i;
				*target ++ = vidx - TO_NEXT_LINE + 1;

				*target ++ = southVertStart + i;
				*target ++ = vidx + 1;
				*target ++ = vidx - TO_NEXT_LINE + 1;
			}

			// exterior 1

			if( !SouthAndEast )
			{
				*target ++ = southVertStart + CELL_VERT_SIZE - 2;
				*target ++ = vidx - TO_NEXT_LINE + 1;
				*target ++ = vidx;

				*target ++ = vidx + 1;
				*target ++ = vidx - TO_NEXT_LINE + 1;
				*target ++ = southVertStart + CELL_VERT_SIZE - 2;
			}
		}
		else
		{
			int vidx = vertexTileDim * CELL_VERT_SIZE;

			// exterior 0
			*target ++ = vidx;
			*target ++ = vidx + 1;
			*target ++ = vidx - TO_NEXT_LINE + 1;

			vidx += 1;

			// interior
			for( int i = 1; i < vertexTileDim - 1; i ++, vidx ++ )
			{
				*target ++ = vidx;
				*target ++ = vidx + 1;
				*target ++ = vidx - TO_NEXT_LINE + 1;

				*target ++ = vidx;
				*target ++ = vidx - TO_NEXT_LINE + 1;
				*target ++ = vidx - TO_NEXT_LINE;
			}

			// exterior 1

			if( !NotSouthAndNotEast )
			{
				*target ++ = vidx - TO_NEXT_LINE;
				*target ++ = vidx;
				*target ++ = vidx - TO_NEXT_LINE + 1;
			}
		}

		// construct west connection
		if( sideLodConnections & WEST_CONNECTION )
		{
			int vidx = 0;

			// exterior 0

			if( !NorthAndWest )
			{
				*target ++ = vidx;
				*target ++ = westVertStart;
				*target ++ = vidx + 1;

				*target ++ = westVertStart;
				*target ++ = vidx + CELL_VERT_SIZE;
				*target ++ = vidx + 1;
			}

			vidx += TO_NEXT_LINE;

			// interior
			for( int i = 1, e = vertexTileDim - 1; i < e; i ++, vidx += TO_NEXT_LINE )
			{
				*target ++ = vidx;
				*target ++ = westVertStart + i;
				*target ++ = vidx + 1;

				*target ++ = westVertStart + i;
				*target ++ = vidx + CELL_VERT_SIZE ;
				*target ++ = vidx + 1;

				*target ++ = vidx + 1;
				*target ++ = vidx + CELL_VERT_SIZE;
				*target ++ = vidx + TO_NEXT_LINE + 1;
			}

			// exterior 1
			{
				*target ++ = vidx;
				*target ++ = westVertStart + CELL_VERT_SIZE - 2;
				*target ++ = vidx + 1;

				*target ++ = westVertStart + CELL_VERT_SIZE - 2;;
				*target ++ = vidx + CELL_VERT_SIZE;
				*target ++ = vidx + 1;
			}

		}
		else
		{
			int vidx = 0;

			*target ++ = vidx + 1;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + TO_NEXT_LINE + 1;

			vidx += TO_NEXT_LINE;

			// interior
			for( int i = 1; i < vertexTileDim - 1; i ++, vidx += TO_NEXT_LINE )
			{
				*target ++ = vidx + 1;
				*target ++ = vidx;
				*target ++ = vidx + TO_NEXT_LINE;

				*target ++ = vidx + TO_NEXT_LINE + 1;
				*target ++ = vidx + 1;
				*target ++ = vidx + TO_NEXT_LINE;
			}

			*target ++ = vidx + 1;
			*target ++ = vidx;
			*target ++ = vidx + TO_NEXT_LINE;
		}

		if( NorthAndWest )
		{
			int vidx = 0;

			*target ++ = northVertStart;
			*target ++ = vidx;
			*target ++ = westVertStart;

			*target ++ = vidx + 1;
			*target ++ = northVertStart;
			*target ++ = vidx + CELL_VERT_SIZE;

			*target ++ = northVertStart;
			*target ++ = westVertStart;
			*target ++ = vidx + CELL_VERT_SIZE;

			*target ++ = vidx + 1;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + TO_NEXT_LINE + 1;
		}

		if( SouthAndEast )
		{
			int vidx = ( vertexTileDim - 1 + 1 ) * CELL_VERT_SIZE - 1 - 1;

			*target ++ = vidx + 1;
			*target ++ = vidx;
			*target ++ = vidx + TO_NEXT_LINE;

			*target ++ = vidx + 1;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = southVertStart + CELL_VERT_SIZE - 2;

			*target ++ = eastVertStart + CELL_VERT_SIZE - 2;
			*target ++ = southVertStart + CELL_VERT_SIZE - 2;
			*target ++ = vidx + TO_NEXT_LINE + 1;

			*target ++ = eastVertStart + CELL_VERT_SIZE - 2;
			*target ++ = vidx + 1;
			*target ++ = southVertStart + CELL_VERT_SIZE - 2;;
		}

		if( NotNorthAndNotWest )
		{
			int vidx = 0;

			*target ++ = vidx;
			*target ++ = vidx + TO_NEXT_LINE;
			*target ++ = vidx + 1;
		}

		if( NotSouthAndNotEast )
		{
			int vidx = ( vertexTileDim - 1 + 1 ) * CELL_VERT_SIZE - 1 - 1;

			*target ++ = vidx + 1 + TO_NEXT_LINE;
			*target ++ = vidx + 1;
			*target ++ = vidx + TO_NEXT_LINE;
		}
	}

	return target - start;
}

#endif

//------------------------------------------------------------------------

static void FillDecalRect( RECT* oRect, const r3dTerrain2::DecalRecord& drec )
{
	r3d_assert( drec.TypeIdx >= 0 );

	const r3dTerrainDesc& tdesc = Terrain2->GetDesc();

	const r3dTerrain2::QualitySettings& qsts = Terrain2->GetCurrentQualitySettings();

	const DecalType& type = g_pDecalChief->GetTypeByIdx( drec.TypeIdx );

	float scaleX = type.ScaleX * drec.Scale;
	float scaleZ = ( type.UniformScale ? type.ScaleX : type.ScaleY ) * drec.Scale;

	float diagonal = sqrtf( scaleX * scaleX + scaleZ * scaleZ );

	float startX = drec.X - diagonal * 0.5f;
	float startZ = drec.Z - diagonal * 0.5f;
	float endX = drec.X + diagonal * 0.5f;
	float endZ = drec.Z + diagonal * 0.5f;

	oRect->left		= LONG( startX / tdesc.CellSize / qsts.VertexTileDim );
	oRect->right	= LONG( endX / tdesc.CellSize / qsts.VertexTileDim );
	oRect->top		= LONG( startZ / tdesc.CellSize / qsts.VertexTileDim );
	oRect->bottom	= LONG( endZ / tdesc.CellSize / qsts.VertexTileDim );

	oRect->left = R3D_MIN( R3D_MAX( (int)oRect->left, 0 ), tdesc.CellCountX - 1 );
	oRect->right = R3D_MIN( R3D_MAX( (int)oRect->right, 0 ), tdesc.CellCountX - 1 );
	oRect->top = R3D_MIN( R3D_MAX( (int)oRect->top, 0 ), tdesc.CellCountZ - 1 );
	oRect->bottom = R3D_MIN( R3D_MAX( (int)oRect->bottom, 0 ), tdesc.CellCountZ - 1 );
}


void SwitchTerraFetchT2()
{
	if( Terrain2 )
	{
		Terrain2->SwitchVFetchMode();
	}
}

static void TerraTextureReloadListener( r3dTexture* tex )
{
	if( Terrain2 )
	{
		int lc = Terrain2->GetDesc().LayerCount;
		int maskCount = lc / r3dTerrain2::LAYERS_PER_MASK;

		for( int i = 0, e = maskCount; i < e; i ++ )
		{
			r3dTexture* layer = Terrain2->GetLayerMask( i );

			if( layer == tex )
			{
				Terrain2->EnsureMaskFormat( i );
				Terrain2->UpdateLayersForMask( i );
				Terrain2->RefreshAtlasTiles();
			}
		}
	}
}