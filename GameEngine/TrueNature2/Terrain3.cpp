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

#include "r3dBackgroundTaskDispatcher.h"

#include "Terrain3.h"

#ifdef ARTIFICIAL_FINAL_BUILD
#define FINAL_BUILD
#endif

#define FNAME_TERRAIN3_INI			"%s/Terrain3/terrain3.ini"	
#define FNAME_TERRAIN3_BIN			"%s/Terrain3/terrain3.bin"

#define R3D_MEGATEX_STAMP_FILE "\\Terrain3\\stamp.stp"

#define TERRAIN3_SIGNATURE			'3RET'
#define TERRAIN3_VERSION			101

#define GENERATE_MIPS 0

r3dITerrain	*Terrain = NULL;
int g_bTerrainUseLightHack = 0;

const char* DigitToCString( int digit );
const char* TwoDigitsToCString( int digit );

void AdvanceLoadingProgress( float );

int _render_Terrain = 1;
int	_terra_smoothBoxOff = 1;
float	_terra_smoothSpeed   = 1.0f;

enum
{
	NORTH_CONNECTION	= 1,
	EAST_CONNECTION		= 2,
	SOUTH_CONNECTION	= 4,
	WEST_CONNECTION		= 8,
};

enum
{
	PHYS_TILE_BORDER				= 1,
	GAME_PHYS_CHUNK_SIZE			= 256,
	FULL_PHYS_CHUNK_SIZE			= 1024,
	NUM_CACHED_UNPACKED_HEIGHTS		= ( r3dTerrain3::PHYS_CHUNK_COUNT_PER_SIDE + 2 ) * ( r3dTerrain3::PHYS_CHUNK_COUNT_PER_SIDE + 2 )
};


enum TileGeomType
{
	TILEGEOMTYPE_INSIDE,
	TILEGEOMTYPE_WEST,
	TILEGEOMTYPE_EAST,
	TILEGEOMTYPE_NORTH,

	TILEGEOMTYPE_SOUTH,
	TILEGEOMTYPE_SOUTH_PARTOFMEGA,

	TILEGEOMTYPE_WEST_CONNECTED,
	TILEGEOMTYPE_EAST_CONNECTED,
	TILEGEOMTYPE_NORTH_CONNECTED,

	TILEGEOMTYPE_SOUTH_CONNECTED,
	TILEGEOMTYPE_SOUTH_CONNECTED_PARTOFMEGA,

	TILEGEOMTYPE_EAST_MEGACONNECTED,
	TILEGEOMTYPE_SOUTH_MEGACONNECTED,

	TILEGEOMTYPE_EAST_CONNECTED_MEGACONNECTED,
	TILEGEOMTYPE_SOUTH_CONNECTED_MEGACONNECTED,

	TILEGEOMTYPE_SOUTH_EAST_MEGACONNECTED,
	TILEGEOMTYPE_SOUTH_EAST_CONNECTED_MEGACONNECTED,
	TILEGEOMTYPE_SOUTH_CONNECTED_EAST_MEGACONNECTED,
	TILEGEOMTYPE_SOUTH_CONNECTED_EAST_CONNECTED_MEGACONNECTED,

	TILEGOEMTYPE_COUNT
};

enum
{
	R3D_MEGATEX_CHUNK_SIZE = 16 * 1024 * 1024
};

enum
{
	DOMINANT_DATA_MAX_LOD = 1
};

enum
{
	FRINGE_ZVAL = -32768,
};

typedef r3dTL::TFixedArray< int, TILEGOEMTYPE_COUNT > GoemTypeInts;
typedef r3dTL::TFixedArray< GoemTypeInts, r3dTerrain3::NUM_QUALITY_LAYERS > TileDimGeomTypeInts;

TileDimGeomTypeInts g_StitchTileIBOffsets;
TileDimGeomTypeInts g_StitchTileIBCounts;

TileDimGeomTypeInts g_StitchTileVBOffsets;
TileDimGeomTypeInts g_StitchTileVBCounts;

extern r3dCamera gCam;

#pragma pack(push)
#pragma pack(1)

struct r3dDynaTerraVertex3
{
	short height;
	short lerpFactor;
};

#pragma pack(pop)

static int ConstructTileVertices( R3D_TERRAIN_VERTEX_3* target, int vertexTileDim, int type );
static int ConstructTileIndices( UINT16* target, int vertexTileDim, int type );

static int ConstructConnectionIndices( UINT16* target, int vertexTileDim, int sideLodConnections );
static void PrintSplatLocation( char (& buf ) [ 1024 ], int idx );
static uint32_t GetHeightFieldDataSize( const PxHeightFieldDesc& desc );
static void FillDecalRect( RECT* oRect, const r3dTerrain3::DecalRecord& drec );

static int IsAutoSave( const char* levelPath );

static IDirect3DVertexDeclaration9* g_TerraVDecl;
static IDirect3DVertexDeclaration9* g_TerraDynaVDecl;

static float gSaveCountDown;
static float gSaveCountDownLastTime;

static const D3DFORMAT TERRA3_LAYERMASK_FORMAT = D3DFMT_DXT1;
static const D3DFORMAT TERRA3_MEGANORMAL_FORMAT = D3DFMT_DXT1;
static const D3DFORMAT TERRA3_MEGAHEIGHT_FORMAT = D3DFMT_L16;
static const D3DFORMAT TERRA3_COLORMODULATION_FORMAT = D3DFMT_DXT1;

extern bool g_bEditMode;

r3dString g_TempEditFilePath;

static volatile int g_TerrainLoadingFrozen;
static int g_AdvanceTerrain3Loading;

static int g_FoundEmptyMask = 0;
static int g_FoundEmptyMaskShown = 0;

static r3dTerrain3::Bytes g_DominantLayerData;
static r3dTerrain3::Bytes g_DominantLayerValues;

struct MegaTexLoadJobParams : r3dTaskParams
{
	r3dTerrain3* terrain;
	r3dTerrain3::MegaTexTile* tile;

	int loadFlags;
};

r3dTaskParramsArray< MegaTexLoadJobParams > g_MexaTexLoadTaskParams;

char MEGA_HN_SIG[ 2 ] = { 'H', 'N' };

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

static R3D_FORCEINLINE bool tile_comp_func_ATLAS_ID( const r3dTerrain3::AllocatedTile* tile0, const r3dTerrain3::AllocatedTile* tile1 )
{
	return tile0->AtlasVolumeID < tile1->AtlasVolumeID;
}

//------------------------------------------------------------------------

struct GenerateAtlas3PixelShaderId
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

	GenerateAtlas3PixelShaderId();

	void FillMacros( ShaderMacros& defines );
	void ToString( char* oStr );
};

static r3dTL::TFixedArray< int, 16 > g_AtlasPixelShaderIdMap_G;

//------------------------------------------------------------------------

GenerateAtlas3PixelShaderId::GenerateAtlas3PixelShaderId()
: Id( 0 )
{

}

//------------------------------------------------------------------------

void
GenerateAtlas3PixelShaderId::FillMacros( ShaderMacros& defines )
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
GenerateAtlas3PixelShaderId::ToString( char* oStr )
{
	strcpy( oStr, "PS_TERRAIN_GEN_ATLAS_G" );
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

struct GenerateAtlas3VertexShaderId
{

	union
	{
		struct
		{
			unsigned unused : 1;
		};

		int Id;
	};

	GenerateAtlas3VertexShaderId();

	void FillMacros( ShaderMacros& defines );
	void ToString( char* oStr );
};

r3dTL::TFixedArray< int, 1 > g_AtlasVertexShaderIdMap_G;

//------------------------------------------------------------------------

GenerateAtlas3VertexShaderId::GenerateAtlas3VertexShaderId()
: Id( 0 )
{

}

//------------------------------------------------------------------------

void
GenerateAtlas3VertexShaderId::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 1 );

	defines[ 0 ].Name		= "UNUSED_DEFINE";
	defines[ 0 ].Definition	= DigitToCString( unused );	
}

//------------------------------------------------------------------------

void
GenerateAtlas3VertexShaderId::ToString( char* oStr )
{
	strcpy( oStr, "VS_TERRAIN_GEN_ATLAS_G" );

	if( unused )
	{
		strcat( oStr, "_UNUSED" );
	}
}

//------------------------------------------------------------------------

struct Terrain3PixelShaderId
{

	union
	{
		struct
		{
			unsigned aux : 1;
			unsigned forward_lighting : 1;
			unsigned near_textures : 1;
			unsigned far_textures : 1;
		};

		int Id;
	};

	Terrain3PixelShaderId();

	void FillMacros( ShaderMacros& defines );
	void ToString( char* oStr );
};

r3dTL::TFixedArray< int, 16 > g_Terrain3PixelShaderIdMap;

struct Terrain3VertexShaderId
{
	union
	{
		struct
		{
			unsigned shadowPath : 1;
			unsigned depthPath : 1;
			unsigned vfetchless : 1;
			unsigned recticular_warp : 1;
			unsigned cam_vec : 1;
		};

		int Id;
	};

	Terrain3VertexShaderId();

	void FillMacros( ShaderMacros& defines );
	void ToString( char* oStr );
};

r3dTL::TFixedArray< int, 32 > g_Terrain3VertexShaderIdMap;

//------------------------------------------------------------------------

Terrain3VertexShaderId::Terrain3VertexShaderId()
: Id( 0 )
{

}

//------------------------------------------------------------------------

void
Terrain3VertexShaderId::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 5 );

	defines[ 0 ].Name		= "SHADOW_PATH";
	defines[ 0 ].Definition	= DigitToCString( shadowPath );

	defines[ 1 ].Name		= "DEPTH_PATH";
	defines[ 1 ].Definition	= DigitToCString( depthPath );	

	defines[ 2 ].Name		= "VERTEX_FETCHLESS";
	defines[ 2 ].Definition	= DigitToCString( vfetchless );

	defines[ 3 ].Name		= "RECTICULAR_WARP";
	defines[ 3 ].Definition	= DigitToCString( recticular_warp );

	defines[ 4 ].Name		= "CAM_VEC";
	defines[ 4 ].Definition	= DigitToCString( cam_vec );

}

//------------------------------------------------------------------------

void
Terrain3VertexShaderId::ToString( char* oStr )
{
	strcpy( oStr, "VS_TERRAIN3" );

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

	if( cam_vec )
	{
		strcat( oStr, "_CAMVEC" );
	}
}

//------------------------------------------------------------------------


static int g_RoadAtlas3PixelShaderId;

//------------------------------------------------------------------------

Terrain3PixelShaderId::Terrain3PixelShaderId()
: Id( 0 )
{

}

//------------------------------------------------------------------------

void
Terrain3PixelShaderId::FillMacros( ShaderMacros& defines )
{
	defines.Resize( 4 );

	defines[ 0 ].Name		= "AUX_ENABLED";
	defines[ 0 ].Definition	= DigitToCString( aux );

	defines[ 1 ].Name		= "FORWARD_LIGHTING";
	defines[ 1 ].Definition	= DigitToCString( forward_lighting );

	defines[ 2 ].Name		= "FAR_TEXTURES";
	defines[ 2 ].Definition	= DigitToCString( far_textures );

	defines[ 3 ].Name		= "NEAR_TEXTURES";
	defines[ 3 ].Definition	= DigitToCString( near_textures );
}

//------------------------------------------------------------------------

void
Terrain3PixelShaderId::ToString( char* oStr )
{
	strcpy( oStr, "PS_TERRAIN3" );

	if( aux )
	{
		strcat( oStr, "_AUX" );
	}

	if( forward_lighting )
	{
		strcat( oStr, "_FORW" );
	}

	if( near_textures )
	{
		strcat( oStr, "_NEAR" );
	}

	if( far_textures )
	{
		strcat( oStr, "_FAR" );
	}
}

//------------------------------------------------------------------------

r3dTerrain3Layer::r3dTerrain3Layer()
: Name( "Nameless Layer" )
, ScaleU( 0.f )
, ScaleV( 0.f )

, SpecularPow( 0.125f )

, DiffuseTex( NULL )
, NormalTex( NULL )

, ChannelMask( 1.0f, 0.f, 0.f, 0.f )
, ChannelIdx( 0 )

, MaterialTypeName( "" )
, ShaderScaleU( 0.f )
, ShaderScaleV( 0.f )

, NormUVMult( 1.0f )
{
	
}

//------------------------------------------------------------------------

r3dTerrain3Stats::r3dTerrain3Stats()
{
	memset( this, 0, sizeof( *this ) );
}

//------------------------------------------------------------------------

r3dTerrain3::DecalRecord::DecalRecord()
: TypeIdx ( -1 )
, X ( 0 )
, Z ( 0 )
, Scale ( 1 )
, ZRot ( 0 )
{

}

//------------------------------------------------------------------------

r3dTerrain3::PhysicsChunk::PhysicsChunk()
: PhysicsTerrain( NULL )
, PhysicsHeightField( NULL )
, StartX( 0 )
, StartZ( 0 )
, ChunkX( 0 )
, ChunkZ( 0 )
, Dirty( 0 )
{

}

//------------------------------------------------------------------------

r3dTerrain3::CreationParams::CreationParams()
: CellCountX( 0 )
, CellCountZ( 0 )

, MaskSizeX( 0 )
, MaskSizeZ( 0 )

, CellSize( 1.f )
, StartHeight( 100.f )
, HeightRange( 512.f )

, MegaTerrain( 0 )
{

}

//------------------------------------------------------------------------

r3dTerrain3::QualitySettings::QualitySettings()
: AtlasTileDim( 256 )
, VertexTileDim( 2 )
, RoadTileDiv( 2 )
, BakeRoads( 1 )
, VertexDensity( 32 )
, VertexTilesInMegaTexTileCount( 8 )
, MaskAtlasTileDim( DEFAULT_ATLAS_TILE_DIM )
, HeightNormalAtlasTileDim( 256 )
, PhysicsTileCellCount( GAME_PHYS_CHUNK_SIZE )
, DominantLayerDataDiv( 2 )
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

	Sync( NULL );
}

//------------------------------------------------------------------------

int
r3dTerrain3::QualitySettings::CompareNoRoads( const QualitySettings& sts )
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
	res |= RoadTileDiv != sts.RoadTileDiv;

	res |= VertexTilesInMegaTexTileCount != sts.VertexTilesInMegaTexTileCount;
	res |= MaskAtlasTileDim != sts.MaskAtlasTileDim;

	res |= HeightNormalAtlasTileDim != sts.HeightNormalAtlasTileDim;

	return res;
}

//------------------------------------------------------------------------

int
r3dTerrain3::QualitySettings::Compare( const QualitySettings& sts )
{
	// NOTE : we skip TileDistances because they are derived from other settings
	int res = CompareNoRoads( sts );

	res |= BakeRoads != sts.BakeRoads;
	
	return res;
}

//------------------------------------------------------------------------

void
r3dTerrain3::QualitySettings::Sync( const r3dTerrainDesc* desc )
{
	if( desc )
	{
		PhysicsTileCellCount = R3D_MIN( R3D_MIN( PhysicsTileCellCount, desc->CellCountX ), desc->CellCountZ );

		VertexTilesInMegaTexTileCount = ( desc->CellCountX / VertexTileDim ) / ( desc->MaskResolutionU / HeightNormalAtlasTileDim );
	}

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
}

//------------------------------------------------------------------------

r3dTerrain3::Info::Info()
: NumActiveMegaTexLayers( 0 )
, MaskFormat( TERRA3_LAYERMASK_FORMAT )
, MegaTileCountX( 0 )
, MegaTileCountZ( 0 )
, AtlasTileCountPerSide( 0 )
, MaskAtlasTileCountPerSide( 0 )
{
	
}

//------------------------------------------------------------------------

r3dTerrain3::Settings::Settings()
: Specular( 0.f )
, FarFadeStart( FLT_MAX )
, FarFadeEnd( FLT_MAX )
, FarNormalTexture( NULL )
, FarDiffuseTexture( NULL )
, FarLayerEnabled( 0 )
, ShadowOffset( 0.f )
, FringeHeight( 0.125f )
{

}

//------------------------------------------------------------------------

void
r3dTerrain3::AllocatedTile::Init()
{
	AdjecantHeights.Clear();

	AdjecantMin = 0xffff;
	AdjecantMax = 0;

	AdjecantPosZOffset = 0;

	MegaTile = NULL;

	X = 0;
	Z = 0;

	AtlasTileID = -1;
	AtlasVolumeID = -1;

	Tagged = 0;
	L = 0;

	ConFlags = 0;
	MegaConFlags = 0;
	Dirty = 0;
	IsCorner = 0;

	FarNearFlags = 0;
}

//------------------------------------------------------------------------

r3dTerrain3::MegaTexTile::LayerBitMaskEntry::LayerBitMaskEntry()
: LayerIndex( -1 )
{

}

//------------------------------------------------------------------------

r3dTerrain3::MegaTexTile::MegaTexTile()
{
	IsAllocated = 0;

#ifndef FINAL_BUILD
	IsLoading = 0;
#endif
}

//------------------------------------------------------------------------

R3D_FORCEINLINE void r3dTerrain3::MegaTexTile::ClearMaskList()
{
	MaskList.ClearValues();
}

//------------------------------------------------------------------------

R3D_FORCEINLINE void r3dTerrain3::MegaTexTile::PushMaskEntry( int entry )
{
	MaskList.PushBack( entry );
}

//------------------------------------------------------------------------

void
r3dTerrain3::MegaTexTile::Init()
{
	X = 0;
	Z = 0;

	HeightNormalAtlasVolumeId = -1;
	HeightNormalAtlasTileId = -1;

	L = 0;
	IsLoaded = 0;
	Tagged = 0;

	Dirty = 0;

	ActiveBitMasks = 0;

	for( int i = 0, e = LayerBitMasks.Count(); i < e; i ++ )
	{
		LayerBitMasks[ i ].LayerIndex = -1;
	}

	MaskList.ClearValues();
	DominantLayerData.Clear();

#ifndef FINAL_BUILD
	IsLoading = 0;
#endif
}

//------------------------------------------------------------------------

R3D_FORCEINLINE int r3dTerrain3::MegaTexTile::IsInAtlas() const
{
	return HeightNormalAtlasTileId >= 0;
}

//------------------------------------------------------------------------

INT64 r3dTerrain3::MegaTexTile::GetID( int maskIdx ) const
{
	return GetID( X, Z, L, maskIdx );
}

//------------------------------------------------------------------------
/*static*/ INT64 r3dTerrain3::MegaTexTile::GetID( short x, short z, char l, int maskId )
{
	r3d_assert( x < 256 && z < 256 && x >= 0 && z >= 0 );

	INT64 id = 0;
	INT64 bits = 0;

	id |= INT64( x ) << bits; bits += 16;
	id |= INT64( z ) << bits; bits += 16;
	id |= INT64( l ) << bits; bits += 8;
	id |= INT64( maskId ) << bits; bits += 8;

	return id;
}

//------------------------------------------------------------------------
/*static*/ void r3dTerrain3::MegaTexTile::DecodeID( INT64 Id, short* oX, short* oZ, char* oL, int* oMaskId )
{
	struct
	{
		INT64 operator() ( int bits )
		{
			int m = 1;
			for( int i = 1; i < bits; i ++ )
			{
				m <<= 1;
				m |= 1;
			}

			return m;
		}
	} mask;

	INT64 bits = 0;
	int bitDist;
	bitDist = 16; *oX = short( Id >> bits & mask( bitDist ) ); bits += bitDist;
	bitDist = 16; *oZ = short( Id >> bits & mask( bitDist ) ); bits += bitDist;
	bitDist = 8; *oL = char( Id >> bits & mask( bitDist ) ); bits += bitDist;
	bitDist = 8; *oMaskId = short( Id >> bits & mask( bitDist ) ); bits += bitDist;
}

//------------------------------------------------------------------------
/*static*/ void r3dTerrain3::MegaTexTile::DecodeMaskEntry( UINT32 entryVal, int * oAtlasVolumeID, int* oAtlasTileID, int* oMaskIdx )
{
	*oMaskIdx = entryVal >> 24 & 0xff;
	*oAtlasVolumeID  = entryVal >> 16 & 0xff;
	*oAtlasTileID = entryVal & 0xffff;
}

//------------------------------------------------------------------------
/*static*/ void r3dTerrain3::MegaTexTile::EncodeMaskEntry( UINT32* oEntryVal, int atlasVolumeID, int atlasTileID, int maskIdx )
{
	*oEntryVal = ( maskIdx << 24 ) | ( atlasVolumeID << 16 ) | atlasTileID;
}

//------------------------------------------------------------------------

static int g_AtlasMipPSId = -1;
static int g_AtlasMipVSId = -1;

//------------------------------------------------------------------------

static int g_Terrain3ShadowPSId = -1;

//------------------------------------------------------------------------
/*static*/

const D3DFORMAT r3dTerrain3::ATLAS_FMT = D3DFMT_R5G6B5;

//------------------------------------------------------------------------

r3dTerrain3::DebugTileInfo::DebugTileInfo()
{
	X = -1;
	Z = -1;
	L = -1;

	AtlasTileX = -1;
	AtlasTileZ = -1;
	AtlasVolumeID = -1;

	MegaX = -1;
	MegaZ = -1;
	MegaL = -1;
	MegaMaskX = -1;
	MegaMaskZ = -1;
	MegaMaskAtlasVolumeId = -1;

	MegaMaskAtlasVolume = NULL;
	TileAtlasVolumeDiffuse = NULL;
	TileAtlasVolumeNormal = NULL;
}

//------------------------------------------------------------------------

r3dTerrain3::MegaTexFileOffset::MegaTexFileOffset()
{
#ifndef FINAL_BUILD
	Value = UINT64( -1LL );
	EditorValue = UINT64( -1LL );
	IsInEditorFile  = 0;
#else
	Value = UINT64( -1LL );
#endif
}

//------------------------------------------------------------------------

r3dTerrain3::MegaTexFileGridOffset::MegaTexFileGridOffset()
{
#ifndef FINAL_BUILD
	Value = UINT64( -1LL );
	GridId = UINT64( -1LL );
	EditorValue = UINT64( -1LL );
	IsInEditorFile  = 0;
#else
	Value = UINT64( -1LL );
	GridId = UINT64( -1LL );
#endif
}

//------------------------------------------------------------------------

r3dTerrain3::UnpackedHeightTile::UnpackedHeightTile()
: tileX( -1 )
, tileZ( -1 )
, Freshness( INT_MAX )
{

}

//------------------------------------------------------------------------

r3dTerrain3::r3dTerrain3()
{
	// must be odd
	COMPILE_ASSERT( PHYS_CHUNK_COUNT_PER_SIDE & 1 );

	// NOTE : move all initialization code to Cosntruct()
	Construct();
}

//------------------------------------------------------------------------

r3dTerrain3::~r3dTerrain3()
{
	Destroy();
}

//------------------------------------------------------------------------
/*static*/

void
r3dTerrain3::LoadShaders()
{
#if R3D_TERRAIN_V3_GRAPHICS

	//------------------------------------------------------------------------
	// Atlas vertex shaders
	{
		GenerateAtlas3VertexShaderId vsid;

		ShaderMacros macros;

		vsid.unused = 0;

		char name[ 512 ];

		vsid.ToString( name );
		vsid.FillMacros( macros );

		g_AtlasVertexShaderIdMap_G[ vsid.Id ] = r3dRenderer->AddVertexShaderFromFile( name, "Nature\\TerrainAtlas_G_vs.hls", 0, macros );
	}

	//------------------------------------------------------------------------
	// atlas pixel shaders
	{
		GenerateAtlas3PixelShaderId psid;

		ShaderMacros macros;

		for( int i = 0, e = g_AtlasPixelShaderIdMap_G.COUNT; i < e; i ++ )
		{
			g_AtlasPixelShaderIdMap_G[ i ] = -1;
		}

		for( int i = 0, e = GenerateAtlas3PixelShaderId::MAX_LAYERS; i <= e; i ++ )
		{
			psid.numLayers = i;

			for( int fi = 0, e = 2; fi < e; fi ++ )
			{
				psid.firstBatch = fi;

				char name[ 512 ];

				psid.modulationBatch = 0;

				psid.ToString( name );
				psid.FillMacros( macros );

				if (r_show_winter->GetBool())
				{
					int count = macros.Count();

					macros.Resize(count + 1);
					macros[ count ].Name		= "WINTER";
					macros[ count ].Definition	= "1";
				}

				g_AtlasPixelShaderIdMap_G[ psid.Id ] = r3dRenderer->AddPixelShaderFromFile( name, "Nature\\TerrainAtlas_G_ps.hls", 0, macros );
			}
		}

		// modulation batch
		{
			psid.Id = 0;
			psid.modulationBatch = 1;

			char name[ 512 ];

			psid.ToString( name );
			psid.FillMacros( macros );

			if (r_show_winter->GetBool())
			{
				int count = macros.Count();

				macros.Resize(count + 1);
				macros[ count ].Name		= "WINTER";
				macros[ count ].Definition	= "1";
			}


			g_AtlasPixelShaderIdMap_G[ psid.Id ] = r3dRenderer->AddPixelShaderFromFile( name, "Nature\\TerrainAtlas_G_ps.hls", 0, macros );
		}
	}

	//------------------------------------------------------------------------
	// Main Terrain Vertex Shader
	{
		for( int s = 0; s < 2; s ++ )
		{
			Terrain3VertexShaderId vsid;

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

						for( int cam_vec = 0; cam_vec < 2; cam_vec ++ )
						{
							vsid.cam_vec = cam_vec;

							if( !r3dRenderer->SupportsVertexTextureFetch && !vfl )
							{
								g_Terrain3VertexShaderIdMap[ vsid.Id ] = -1;
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
								g_Terrain3VertexShaderIdMap[ vsid.Id ] = -1;
							}
							else
							{
								g_Terrain3VertexShaderIdMap[ vsid.Id ] = r3dRenderer->AddVertexShaderFromFile( name, "Nature\\Terrain2_G_vs.hls", 0, macros );
							}
						}
					}
				}
			}
		}
	}

	//------------------------------------------------------------------------
	// Main Terrain Pixel Shader
	{
		Terrain3PixelShaderId psid;

		ShaderMacros macros;

		for( int i = 0, e = g_Terrain3PixelShaderIdMap.COUNT; i < e; i ++ )
		{
			g_Terrain3PixelShaderIdMap[ i ] = -1;
		}

		for( int l = 0, e = 2; l < e; l ++ )
		{
			for( int a = 0, e = 2; a < e; a ++ )
			{
				for( int near_texture = 0; near_texture < 2; near_texture ++ )
				{
					for( int far_texture = 0; far_texture < 2; far_texture ++ )
					{
						if( !near_texture && !far_texture )
							continue;

						psid.aux = a;
						psid.forward_lighting = l;
						psid.near_textures = near_texture;
						psid.far_textures = far_texture;


						char name[ 512 ];

						psid.ToString( name );
						psid.FillMacros( macros );

						if (r_show_winter->GetBool())
						{
							int count = macros.Count();

							macros.Resize(count + 1);
							macros[ count ].Name		= "WINTER";
							macros[ count ].Definition	= "1";
						}

						g_Terrain3PixelShaderIdMap[ psid.Id ] = r3dRenderer->AddPixelShaderFromFile( name, "Nature\\Terrain2_G_ps.hls", 0, macros );

					}
				}
			}
		}
	}

	//------------------------------------------------------------------------
	// Shadow PS
	g_Terrain3ShadowPSId = r3dRenderer->AddPixelShaderFromFile( "PS_TERRA3_SM", "Nature\\Terrain3_SM_ps.hls", 1 );

	//------------------------------------------------------------------------
	// Tile mip generation shaders

	g_AtlasMipVSId = r3dRenderer->AddVertexShaderFromFile( "VS_TERRAIN_ATLAS_MIP", "Nature\\TerrainAtlasMip_vs.hls" );
	g_AtlasMipPSId = r3dRenderer->AddPixelShaderFromFile( "PS_TERRAIN_ATLAS_MIP", "Nature\\TerrainAtlasMip_ps.hls" );

	//------------------------------------------------------------------------
	// road to atlas
	g_RoadAtlas3PixelShaderId = r3dRenderer->AddPixelShaderFromFile( "PS_TERRA3_ATLAS_ROAD_PS", "DS_FillTerraAtlas3_PS.hls" );
#endif

}

void
r3dTerrain3::ReloadShaders()
{
#if R3D_TERRAIN_V3_GRAPHICS


	//------------------------------------------------------------------------
	// atlas pixel shaders
	{
		GenerateAtlas3PixelShaderId psid;

		ShaderMacros macros;

		//for( int i = 0, e = g_AtlasPixelShaderIdMap_G.COUNT; i < e; i ++ )
		//{
		//	g_AtlasPixelShaderIdMap_G[ i ] = -1;
		//}

		for( int i = 0, e = GenerateAtlas3PixelShaderId::MAX_LAYERS; i <= e; i ++ )
		{
			psid.numLayers = i;

			for( int fi = 0, e = 2; fi < e; fi ++ )
			{
				psid.firstBatch = fi;

				char name[ 512 ];

				psid.modulationBatch = 0;

				psid.ToString( name );
				psid.FillMacros( macros );

				if (r_show_winter->GetBool())
				{
					int count = macros.Count();

					macros.Resize(count + 1);
					macros[ count ].Name		= "WINTER";
					macros[ count ].Definition	= "1";
				}

				r3dRenderer->ReloadPixelShader( name, "Nature\\TerrainAtlas_G_ps.hls", macros );
			}
		}

		// modulation batch
		{
			psid.Id = 0;
			psid.modulationBatch = 1;

			char name[ 512 ];

			psid.ToString( name );
			psid.FillMacros( macros );

			if (r_show_winter->GetBool())
			{
				int count = macros.Count();

				macros.Resize(count + 1);
				macros[ count ].Name		= "WINTER";
				macros[ count ].Definition	= "1";
			}


			r3dRenderer->ReloadPixelShader( name, "Nature\\TerrainAtlas_G_ps.hls", macros );
		}
	}


	//------------------------------------------------------------------------
	// Main Terrain Pixel Shader
	{
		Terrain3PixelShaderId psid;

		ShaderMacros macros;

		//for( int i = 0, e = g_Terrain3PixelShaderIdMap.COUNT; i < e; i ++ )
		//{
		//	g_Terrain3PixelShaderIdMap[ i ] = -1;
		//}

		for( int l = 0, e = 2; l < e; l ++ )
		{
			for( int a = 0, e = 2; a < e; a ++ )
			{
				for( int near_texture = 0; near_texture < 2; near_texture ++ )
				{
					for( int far_texture = 0; far_texture < 2; far_texture ++ )
					{
						if( !near_texture && !far_texture )
							continue;

						psid.aux = a;
						psid.forward_lighting = l;
						psid.near_textures = near_texture;
						psid.far_textures = far_texture;


						char name[ 512 ];

						psid.ToString( name );
						psid.FillMacros( macros );

						if (r_show_winter->GetBool())
						{
							int count = macros.Count();

							macros.Resize(count + 1);
							macros[ count ].Name		= "WINTER";
							macros[ count ].Definition	= "1";
						}

						r3dRenderer->ReloadPixelShader( name, "Nature\\Terrain2_G_ps.hls", macros );

					}
				}
			}
		}
	}

	////------------------------------------------------------------------------
	//// Shadow PS
	//g_Terrain3ShadowPSId = r3dRenderer->AddPixelShaderFromFile( "PS_TERRA3_SM", "Nature\\Terrain3_SM_ps.hls", 1 );

	////------------------------------------------------------------------------
	//// Tile mip generation shaders

	//g_AtlasMipVSId = r3dRenderer->AddVertexShaderFromFile( "VS_TERRAIN_ATLAS_MIP", "Nature\\TerrainAtlasMip_vs.hls" );
	//g_AtlasMipPSId = r3dRenderer->AddPixelShaderFromFile( "PS_TERRAIN_ATLAS_MIP", "Nature\\TerrainAtlasMip_ps.hls" );

	//------------------------------------------------------------------------
	// road to atlas
	{

		ShaderMacros macros;

		if (r_show_winter->GetBool())
		{
			int count = macros.Count();

			macros.Resize(count + 1);
			macros[ count ].Name		= "WINTER";
			macros[ count ].Definition	= "1";
		}
	
		g_RoadAtlas3PixelShaderId = r3dRenderer->AddPixelShaderFromFile( "PS_TERRA3_ATLAS_ROAD_PS", "DS_FillTerraAtlas3_PS.hls", 0, macros  );
	}
#endif

}

//------------------------------------------------------------------------

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

static R3D_FORCEINLINE short4 Conform4( float2 val )
{
	return short4( Conform( val.x ), Conform( val.y ), 0, 0 );
}

static R3D_FORCEINLINE short4 Conform( float2 val, short sub )
{
	return short4( Conform( val.x ), Conform( val.y ), sub, 0 );
}

//------------------------------------------------------------------------

void
r3dTerrain3::Init()
{
#if R3D_TERRAIN_V3_GRAPHICS
	if( !r3dRenderer->SupportsVertexTextureFetch )
	{
		const char* errMsg = "You need a video card which supports vertex texture fetch in order to render Terrain 3 ( ATI Radeon 2xxx or higher, nVidia GeForce 6xxx or higher )";
		MessageBoxA( r3dRenderer->HLibWin, errMsg, "Error", MB_ICONERROR );
		r3dError( errMsg );
	}
#endif

	RecalcVars();
	RecalcLayerVars();

#if R3D_TERRAIN_V3_GRAPHICS
	if( !g_TerraVDecl )
	{
		enum
		{
			DECL_TYPE = D3DDECLTYPE_SHORT4N,
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

	CreateDynaVertexBuffer();
#endif

	InitDynamic();
}

//------------------------------------------------------------------------

void
r3dTerrain3::InitDynamic()
{
#if R3D_TERRAIN_V3_GRAPHICS
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
		totalVertexCount += ( vc + 1 ) * ( vc + 1 ) + vc * 4 + ( vc * 2 + 1 ) * 4;
	}

	for( int vc = m_QualitySettings.VertexTileDim, d = 0; vc <= MAX_VERTEX_DIM; vc *= 2, d ++ )
	{
		for( int i = 0, e = TILEGOEMTYPE_COUNT; i < e; i ++ )
		{
			g_StitchTileVBOffsets[ d ][ i ] = totalVertexCount;
			g_StitchTileVBCounts[ d ][ i ] = ConstructTileVertices( NULL, vc, i );
			totalVertexCount += g_StitchTileVBCounts[ d ][ i ];
		}
	}

	m_TileVertexBuffer	= gfx_new r3dVertexBuffer( totalVertexCount, sizeof( R3D_TERRAIN_VERTEX_3 ), 0, false, false, TerrainBufferMem );

	int connectedCount = 0;

	for( int vc = m_QualitySettings.VertexTileDim, d = 0; vc <= MAX_VERTEX_DIM; d ++, vc *= 2 )
	{
		m_ConnectionIndexOffsets.Resize( m_ConnectionIndexOffsets.Count() + 1 );
		m_ConnectionIndexCounts.Resize( m_ConnectionIndexCounts.Count() + 1 );

		for( int i = 0; i < TERRA_CONNECTION_TYPE_COUNT; i ++ )
		{
			m_ConnectionIndexOffsets[ d ][ i ] = connectedCount;
			int count = ConstructConnectionIndices( NULL, vc, i );
			m_ConnectionIndexCounts[ d ][ i ] = count / 3;

			connectedCount += count;
		}
	}

	connectedCount += 6 + MAX_VERTEX_DIM * 4 + 1;

	for( int vc = m_QualitySettings.VertexTileDim, d = 0; vc <= MAX_VERTEX_DIM; vc *= 2, d ++ )
	{
		for( int i = 0, e = TILEGOEMTYPE_COUNT; i < e; i ++ )
		{
			g_StitchTileIBOffsets[ d ][ i ] = connectedCount;
			g_StitchTileIBCounts[ d ][ i ] = ConstructTileIndices( NULL, vc, i );
			connectedCount += g_StitchTileIBCounts[ d ][ i ];

			g_StitchTileIBCounts[ d ][ i ] /= 3;
		}
	}


	m_TileIndexBuffer	= gfx_new r3dIndexBuffer( connectedCount, false, 2, TerrainBufferMem );

	//------------------------------------------------------------------------
	// fill vertex buffer
	{
		R3D_TERRAIN_VERTEX_3* vertexData = static_cast<R3D_TERRAIN_VERTEX_3*>( m_TileVertexBuffer->Lock() );
		R3D_TERRAIN_VERTEX_3* vertexStart = vertexData;

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
					vertexData->pos.z = 0;
					vertexData->pos.w = 0;

					vertexData ++;
				}
			}			

			// connection vertices
			x = -1.f + 1.0f / VERTEX_TILE_DIM;
			for( int i = 0, e = VERTEX_TILE_DIM; i < e; i ++, x += step )
			{
				vertexData->pos.x = Conform( x );
				vertexData->pos.y = Conform( -1.0f );
				vertexData->pos.z = 0;
				vertexData->pos.w = 0;

				vertexData ++;
			}

			x = -1.f + 1.0f / VERTEX_TILE_DIM;
			for( int i = 0, e = VERTEX_TILE_DIM; i < e; i ++, x += step )
			{
				vertexData->pos.x = Conform( x );
				vertexData->pos.y = Conform( +1.0f );
				vertexData->pos.z = 0;
				vertexData->pos.w = 0;

				vertexData ++;
			}

			z = -1.f + 1.0f / VERTEX_TILE_DIM;
			for( int i = 0, e = VERTEX_TILE_DIM; i < e; i ++, z += step )
			{
				vertexData->pos.x = Conform( -1.0f );
				vertexData->pos.y = Conform( z );
				vertexData->pos.z = 0;
				vertexData->pos.w = 0;

				vertexData ++;
			}

			z = -1.f + 1.0f / VERTEX_TILE_DIM;
			for( int i = 0, e = VERTEX_TILE_DIM; i < e; i ++, z += step )
			{
				vertexData->pos.x = Conform( +1.0f );
				vertexData->pos.y = Conform( z );
				vertexData->pos.z = 0;
				vertexData->pos.w = 0;

				vertexData ++;
			}

			//------------------------------------------------------------------------
			// down guards

			enum
			{
				ZVAL = FRINGE_ZVAL
			};

			float halfStep = step * 0.5f;

			x = -1.f;
			z = -1.f;
			for( int j = 0, e = VERTEX_TILE_DIM * 2 + 1; j < e; j ++, x += halfStep )
			{
				vertexData->pos.x = Conform( x );
				vertexData->pos.y = Conform( z );
				vertexData->pos.z = ZVAL;
				vertexData->pos.w = 0;

				vertexData ++;
			}

			x = -1.f;
			z = +1.f;
			for( int j = 0, e = VERTEX_TILE_DIM * 2 + 1; j < e; j ++, x += halfStep )
			{
				vertexData->pos.x = Conform( x );
				vertexData->pos.y = Conform( z );
				vertexData->pos.z = ZVAL;
				vertexData->pos.w = 0;

				vertexData ++;
			}

			x = -1.f;
			z = -1.f;
			for( int j = 0, e = VERTEX_TILE_DIM * 2 + 1; j < e; j ++, z += halfStep )
			{
				vertexData->pos.x = Conform( x );
				vertexData->pos.y = Conform( z );
				vertexData->pos.z = ZVAL;
				vertexData->pos.w = 0;

				vertexData ++;
			}

			x = +1.f;
			z = -1.f;
			for( int j = 0, e = VERTEX_TILE_DIM * 2 + 1; j < e; j ++, z += halfStep )
			{
				vertexData->pos.x = Conform( x );
				vertexData->pos.y = Conform( z );
				vertexData->pos.z = ZVAL;
				vertexData->pos.w = 0;

				vertexData ++;
			}

			//------------------------------------------------------------------------

		}

		for( int vc = m_QualitySettings.VertexTileDim, d = 0; vc <= MAX_VERTEX_DIM; vc *= 2, d ++ )
		{
			for( int i = 0, e = TILEGOEMTYPE_COUNT; i < e; i ++ )
			{
				vertexData += ConstructTileVertices( vertexData, vc, i );
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

		int LAST_D_OFFSET = m_ConnectionVertexOffsets[ m_ConnectionVertexOffsets.Count() - 1 ];

		*indexData++ = LAST_D_OFFSET;

		for( int i = 0, e = MAX_VERTEX_DIM; i < e; i ++ )
		{
			*indexData ++ = i + 1 + LAST_D_OFFSET;
		}

		for( int i = 1, e = MAX_VERTEX_DIM + 1; i < e; i ++ )
		{
			*indexData ++ = i * ( MAX_VERTEX_DIM + 1 ) + MAX_VERTEX_DIM + LAST_D_OFFSET;
		}

		for( int i = 1, e = MAX_VERTEX_DIM + 1; i < e; i ++ )
		{
			*indexData ++ = ( MAX_VERTEX_DIM + 1 ) * MAX_VERTEX_DIM + MAX_VERTEX_DIM - i + LAST_D_OFFSET;
		}

		for( int i = 1, e = MAX_VERTEX_DIM + 1; i < e; i ++ )
		{
			*indexData ++ = ( MAX_VERTEX_DIM - i ) * ( MAX_VERTEX_DIM + 1 ) + LAST_D_OFFSET;
		}

		//------------------------------------------------------------------------

		for( int vc = m_QualitySettings.VertexTileDim, d = 0; vc <= MAX_VERTEX_DIM; vc *= 2, d ++ )
		{
			for( int i = 0, e = TILEGOEMTYPE_COUNT; i < e; i ++ )
			{
				indexData += ConstructTileIndices( indexData, vc, i );
			}
		}

		r3d_assert( indexData - pLodIndStart == m_TileIndexBuffer->GetItemCount() );

		m_TileIndexBuffer->Unlock();
	}

	//------------------------------------------------------------------------
	// unit quad vertex buffer

	m_UnitQuadVertexBuffer = gfx_new r3dVertexBuffer( 4, sizeof (R3D_TERRAIN_VERTEX_3), 0, false, false, TerrainBufferMem );

	{
		R3D_TERRAIN_VERTEX_3* vertexData = static_cast<R3D_TERRAIN_VERTEX_3*>( m_UnitQuadVertexBuffer->Lock() );

		vertexData ++ ->pos = Conform4( float2( -1.0f, -1.0f ) );
		vertexData ++ ->pos = Conform4( float2( +1.0f, -1.0f ) );
		vertexData ++ ->pos = Conform4( float2( -1.0f, +1.0f ) );
		vertexData ++ ->pos = Conform4( float2( +1.0f, +1.0f ) );

		m_UnitQuadVertexBuffer->Unlock();
	}

	m_RoadInfosUpdated = 0;

	ProcessCustomDeviceQueueItem( InitDynamicInMainThread, this );

	if( m_Settings.FarNormalTextureName.Length() > 0 )
	{
		r3d_assert( !m_Settings.FarNormalTexture );
		m_Settings.FarNormalTexture = r3dRenderer->LoadTexture( m_Settings.FarNormalTextureName.c_str() );
	}

	if( m_Settings.FarDiffuseTextureName.Length() > 0 )
	{
		r3d_assert( !m_Settings.FarDiffuseTexture );
		m_Settings.FarDiffuseTexture = r3dRenderer->LoadTexture( m_Settings.FarDiffuseTextureName.c_str() );
	}

	m_MaxAllocatedTiles = 0;

#endif //R3D_TERRAIN_V3_GRAPHICS

#if R3D_TERRAIN3_ALLOW_FULL_PHYSCHUNKS
	if( m_FullPhysChunkModeOn )
	{
		for( int i = 0, e = m_BaseQualitySettings.COUNT; i < e; i ++ )
		{
			m_BaseQualitySettings[ i ].PhysicsTileCellCount = GetPhysChunkSize();
		}

		m_QualitySettings.PhysicsTileCellCount = GetPhysChunkSize();
	}
#endif

	m_UnpackedHeightCache.Resize( NUM_CACHED_UNPACKED_HEIGHTS );

	m_TileUpdateLog.Resize( 20 );
}

//------------------------------------------------------------------------

void
r3dTerrain3::CloseDynamic()
{
#if R3D_TERRAIN_V3_GRAPHICS

	FreeAtlas();
	FreeMaskAtlas();
	FreeHeightNormalAtlas();

	for( int i = 0, e = (int) m_AllocTileLodArray.Count(); i < e; i ++ )
	{
		m_AllocTileLodArray[ i ].Clear();
	}

	for( int i = 0, e = (int)m_AllocMegaTexTileLodArray.Count(); i < e; i ++ )
	{
		m_AllocMegaTexTileLodArray[ i ].Clear();
	}

	SAFE_DELETE( m_TempDiffuseRT );
	SAFE_DELETE( m_TempNormalRT	);

	SAFE_DELETE( m_UnitQuadVertexBuffer );

	SAFE_DELETE( m_TileVertexBuffer );
	SAFE_DELETE( m_TileIndexBuffer );

	m_ConnectionVertexOffsets.Clear();
	m_ConnectionIndexCounts.Clear();

	m_MegaTexTile_TempHeight.ReleaseAndReset();
	m_MegaTexTile_TempNormal.ReleaseAndReset();
	m_MegaTexTile_TempMask.ReleaseAndReset();

	for( int i = 0, e = m_MegaTexTileEntries.Count(); i < e; i ++ )
	{
		delete m_MegaTexTileEntries[ i ];
	}

	m_MegaTexTileEntries.Clear();

	if( m_Settings.FarNormalTexture )
	{
		r3dRenderer->DeleteTexture( m_Settings.FarNormalTexture );
		m_Settings.FarNormalTexture = NULL;
	}

	if( m_Settings.FarDiffuseTexture )
	{
		r3dRenderer->DeleteTexture( m_Settings.FarDiffuseTexture );
		m_Settings.FarDiffuseTexture = NULL;
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::Close()
{
#if R3D_TERRAIN_V3_GRAPHICS
	r3dFinishBackGroundTasks();

	if( m_LowResNormalTexture )
	{
		r3dRenderer->DeleteTexture( m_LowResNormalTexture );
		m_LowResNormalTexture = NULL;
	}

	if( m_Settings.FarNormalTexture )
	{
		r3dRenderer->DeleteTexture( m_Settings.FarNormalTexture );
		m_Settings.FarNormalTexture = NULL;
	}

	if( m_Settings.FarDiffuseTexture )
	{
		r3dRenderer->DeleteTexture( m_Settings.FarDiffuseTexture );
		m_Settings.FarDiffuseTexture = NULL;
	}

	for( int i = 0, e = m_Layers.Count(); i < e; i ++ )
	{
		r3dTerrain3Layer& layer = m_Layers[ i ];

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

	SAFE_DELETE( m_DynamicVertexBuffer );

#ifndef FINAL_BUILD
	CloseEditorFile();
#endif

#endif

	if( m_MegaTexAtlasFile )
	{
		fclose( m_MegaTexAtlasFile );
		m_MegaTexAtlasFile = NULL;
	}

	CloseMegaTexGridFiles();

	CloseDynamic();

	if( m_MegaTexAtlasMipsFile )
	{
		fclose( m_MegaTexAtlasMipsFile );
		m_MegaTexAtlasMipsFile = NULL;
	}

	free( m_SharedPhysDescData );
}

//------------------------------------------------------------------------

void r3dTerrain3::Update( const r3dCamera& cam )
{
#ifndef FINAL_BUILD
	if( g_bEditMode )
	{
		if( g_FoundEmptyMask && !g_FoundEmptyMaskShown )
		{
			r3dArtBug( "Found empty mask tile in Terrain. Please do Terrain 3->Masks->Optimize Layer Masks!" );
			g_FoundEmptyMaskShown = 1;
		}
	}

	if( m_EDITOR_IsCameraFixed )
	{
		DoUpdate( m_EDITOR_FixedCamera );
	}
	else
#endif
	{
		DoUpdate( cam );
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::Render( const r3dCamera& cam )
{
#ifndef FINAL_BUILD
	if( m_EDITOR_IsCameraFixed )
	{
		DoRender( m_EDITOR_FixedCamera );
	}
	else
#endif
	{
		DoRender( cam );
	}
}

//------------------------------------------------------------------------

void
r3dTerrain3::DoUpdate( const r3dCamera& cam ) 
{
	R3DPROFILE_FUNCTION( "r3dTerrain3::DoUpdate" );

	UpdatePhysChunks( cam );

#if R3D_TERRAIN_V3_GRAPHICS
	if( !m_IsQualityUpdated )
	{
		UpdateQualitySettings();
		m_IsQualityUpdated = 1;
	}

	if( !m_RoadInfosUpdated )
	{
		UpdateRoadInfos();
	}

#ifndef FINAL_BUILD
	if( d_terrain2_no_updates->GetInt() )
		return;
#endif

#if 1
	UpdateMegaTexTiles( cam );
	UpdateTiles_G( cam );
#else
	if( r_0debug_helper->GetInt() )
	{
		PrepareOrthographicTerrainRender( 2048, 2048 );
	}
	else
	{
		UpdateMegaTexTiles( cam );
		UpdateTiles_G( cam );
	}
#endif

	UpdateTileMips();

	UpdateEditor();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::DoRender( const r3dCamera& cam )
{
#if R3D_TERRAIN_V3_GRAPHICS

#if 0
	if( r_0debug_helper->GetInt() )
	{
		StartTileRendering();

		{
			m_VisibleTiles.Clear();

			AllocatedTileArr& arr = m_AllocTileLodArray[ m_RenderOrthoTextureTileMip ];

			for( int i = 0, e = arr.Count(); i < e; i ++ )
			{
				RenderTile_G( &arr[ i ], gCam, true );
				m_VisibleTiles.PushBack( &arr[ i ] );
			}
		}

		EndTileRendering();

		DEBUG_RenderTileBoundaries();

		return;
	}
#endif

	extern int _render_Terrain;
	if(!_render_Terrain) return;

	R3DPROFILE_FUNCTION( "r3dTerrain3::Render" );

	D3DPERF_BeginEvent( 0, L"r3dTerrain3::Render" );

	UpdateVisibleTiles();

#ifndef FINAL_BUILD
	DEBUG_RenderTileBoundaries();
#endif

	StartTileRendering();

	for( int i = 0, e  = m_VisibleTiles.Count(); i < e; i ++ )
	{
		const AllocatedTile* tile = m_VisibleTiles[ i ];
#if 1
		RenderTile_G( tile, cam, false );
#else
		RenderTile_G( tile, cam, true );
#endif
	}

	EndTileRendering();

	D3DPERF_EndEvent();
#endif
}

//------------------------------------------------------------------------

void	DEBUG_PrintMegaTileMap();

//------------------------------------------------------------------------

void
r3dTerrain3::RenderShadows()
{
#if R3D_TERRAIN_V3_GRAPHICS
	R3DPROFILE_FUNCTION( "r3dTerrain3::RenderShadows" );

	D3DPERF_BeginEvent( 0, L"r3dTerrain3::RenderShadows" );

	UpdateVisibleTiles();

	StartShadowRender();

	for( int i = 0, e  = m_VisibleTiles.Count(); i < e; i ++ )
	{
		const AllocatedTile* tile = m_VisibleTiles[ i ];
		RenderShadowTile( tile, gCam );
	}

	StopShadowRender();

	D3DPERF_EndEvent();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::RenderDepth()
{
#if R3D_TERRAIN_V3_GRAPHICS
	R3DPROFILE_FUNCTION( "r3dTerrain3::RenderDepth" );

	D3DPERF_BeginEvent( 0, L"r3dTerrain3::RenderDepth" );

	UpdateVisibleTiles();

	StartDepthRender();

	for( int i = 0, e  = m_VisibleTiles.Count(); i < e; i ++ )
	{
		const AllocatedTile* tile = m_VisibleTiles[ i ];
		RenderDepthTile( tile, gCam );
	}

	StopDepthRender();

	D3DPERF_EndEvent();
#endif
}

//------------------------------------------------------------------------

static int g_DEBUG_FrameId;

void
r3dTerrain3::DrawDebug()
{
#if R3D_TERRAIN_V3_GRAPHICS
#ifndef FINAL_BUILD

	r3dRenderer->SetPixelShader();
	r3dRenderer->SetVertexShader();

	g_DEBUG_FrameId ++;

	if( r_terrain2_show_atlas->GetInt() )
	{
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );

		int vol = r_terrain2_show_atlas_volume->GetInt();

		vol = R3D_MIN( R3D_MAX( vol, 0 ), (int)m_Atlas.Count() - 1 );

		r3dTexture* tex = NULL;
		
		if( r_terrain2_show_atlas->GetInt() == 2 )
			tex = m_Atlas[ vol ].Normal->Tex;
		else
			tex = m_Atlas[ vol ].Diffuse->Tex;

		float scale = 1.0f / r_terrain2_show_atlas_scale->GetFloat();

		float w = scale * tex->GetWidth();
		float h = scale * tex->GetHeight();

		float off_x = scale * r_terrain2_show_atlas_off_x->GetFloat();
		float off_y = scale * r_terrain2_show_atlas_off_y->GetFloat();

		r3dColor colr = r3dColor::white;
		colr.A = R3D_MAX( R3D_MIN( int( r_terrain2_show_atlas_oppa->GetFloat() * 255 ), 255 ), 0 );

		r3dDrawBox2D( r3dRenderer->ScreenW2 - w / 2 + off_x, r3dRenderer->ScreenH2 - h / 2 + off_y, w, h, colr, tex );
		r3dRenderer->Flush();

		if( r_terrain2_show_atlas_grid->GetInt() )
		{
			r3dRenderer->SetTex( NULL );

			for( int x = 0, e = m_AtlasTileCountPerSide; x < e; x ++ )
			{
				float x0 = r3dRenderer->ScreenW2 - w / 2 + off_x + w * x / e;
				float y0 = r3dRenderer->ScreenH2 - h / 2 + off_y;

				float x1 = x0;
				float y1 = y0 + h;

				r3dDrawLine2D( x0, y0, x1, y1, 1.0f, r3dColor( 127, 127, 127, 127 ) );
			}

			for( int y = 0, e = m_AtlasTileCountPerSide; y < e; y ++ )
			{
				float x0 = r3dRenderer->ScreenW2 - w / 2 + off_x;
				float y0 = r3dRenderer->ScreenH2 - h / 2 + off_y  + h * y / e;

				float x1 = x0 + w;
				float y1 = y0;

				r3dDrawLine2D( x0, y0, x1, y1, 1.0f, r3dColor( 127, 127, 127, 127 ) );
			}
		}

		r3dRenderer->Flush();
		r3dRenderer->SetRenderingMode( R3D_BLEND_POP );

	}

	if( r_terrain2_show_mega_tex->GetInt() )
	{
		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );

		int idx = int( r_terrain2_show_mega_tex_idx->GetInt() );


		r3dTexture* visTex = NULL;
		
		switch( r_terrain2_show_mega_tex->GetInt() )
		{
		case SHOWMEGA_MASKS:
			idx = R3D_MIN( R3D_MAX( idx, 0 ), (int)m_MaskAtlas.Count() - 1 );
			visTex = idx >= 0 ? m_MaskAtlas[ idx ].Mask : 0;
			break;
		case SHOWMEGA_HEIGHT:
			idx = R3D_MIN( R3D_MAX( idx, 0 ), (int)m_HeightNormalAtlas.Count() - 1 );
			visTex = idx >= 0 ? m_HeightNormalAtlas[ idx ].Height : 0;
			break;
		case SHOWMEGA_NORMAL:
			idx = R3D_MIN( R3D_MAX( idx, 0 ), (int)m_HeightNormalAtlas.Count() - 1 );
			visTex = idx >= 0 ? m_HeightNormalAtlas[ idx ].Normal : 0;
			break;
		}

		if( visTex )
		{
			float w = 512 * r_terrain2_show_mt_scale->GetFloat();
			float h = w;

			r3dColor colr = r3dColor::white;

			float offx = r_terrain2_show_mt_offx->GetFloat() * w;
			float offy = r_terrain2_show_mt_offy->GetFloat() * h;

			r3dDrawBox2D( r3dRenderer->ScreenW2 - w / 2 + offx, r3dRenderer->ScreenH2 - h / 2 + offy, w, h, colr, visTex );

			if( r_terrain2_show_mt_grid->GetInt() )
			{
				D3DPERF_BeginEvent( 0, L"Show grid" );

				r3dRenderer->SetTex( NULL );

				int cps = 8;

				switch( r_terrain2_show_mega_tex->GetInt() )
				{
				case SHOWMEGA_MASKS:
					cps = m_MaskAtlasTileCountPerSide;
					break;
				case SHOWMEGA_HEIGHT:
				case SHOWMEGA_NORMAL:
					cps = m_HeightNormalAtlasTileCountPerSide;
					break;
				}

				for( int x = 0, e = cps; x < e; x ++ )
				{
					float x0 = r3dRenderer->ScreenW2 - w / 2 + offx + w * x / e;
					float y0 = r3dRenderer->ScreenH2 - h / 2 + offy;

					float x1 = x0;
					float y1 = y0 + h;

					r3dDrawLine2D( x0, y0, x1, y1, 1.0f, r3dColor( 127, 127, 127, 127 ) );
				}

				for( int y = 0, e = cps; y < e; y ++ )
				{
					float x0 = r3dRenderer->ScreenW2 - w / 2 + offx;
					float y0 = r3dRenderer->ScreenH2 - h / 2 + offy  + h * y / e;

					float x1 = x0 + w;
					float y1 = y0;

					r3dDrawLine2D( x0, y0, x1, y1, 1.0f, r3dColor( 127, 127, 127, 127 ) );
				}

				D3DPERF_EndEvent();
			}

			r3dRenderer->Flush();
		}

		r3dRenderer->SetRenderingMode( R3D_BLEND_POP );		
	}

	if( r_terrain2_show_tiles->GetInt() )
	{
		int MAX_VERTEX_DIM = m_QualitySettings.VertexTileDim;

		for( int i = 0, e = m_QualitySettings.TileVertexDensities.COUNT; i < e; i ++ )
		{
			MAX_VERTEX_DIM = R3D_MAX( MAX_VERTEX_DIM, m_QualitySettings.VertexTileDim * ( 1 << m_QualitySettings.TileVertexDensities[ i ] ) );
		}

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

			if( MegaTexTile* megaTile = tile->MegaTile )
			{
				if( megaTile->IsInAtlas() )
					r3dRenderer->SetTex( m_HeightNormalAtlas[ megaTile->HeightNormalAtlasVolumeId ].Height, D3DVERTEXTEXTURESAMPLER0 );
			}

			SetMegaVertexShader( 0, 0, 0 );
			SetTileRenderVSConsts_G( tile, gCam );

			m_HeightOffset = oldOffset;

			r3dRenderer->DrawIndexed(	D3DPT_LINESTRIP, 0, 0, 
										( MAX_VERTEX_DIM + 1 ) * ( MAX_VERTEX_DIM + 1 ),
										m_DebugVisIndexOffset, 
										MAX_VERTEX_DIM * 4 );
		}

		{
			float psColor[ 4 ] = { 0, 0, 1, 1 };
			D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, psColor, 1 ) );
		}

		for( int i = 0, e = m_DebugVisibleMegaTexTiles.Count(); i < e; i ++ )
		{
			MegaTexTile* tile = m_DebugVisibleMegaTexTiles[ i ];

			if( !tile->IsInAtlas() || !tile->IsLoaded )
				continue;
			
			r3dRenderer->SetTex( m_HeightNormalAtlas[ tile->HeightNormalAtlasVolumeId ].Height, D3DVERTEXTEXTURESAMPLER0 );

			// hack offset...
			float oldOffset = m_HeightOffset;

			m_HeightOffset += 0.05f;

			AllocatedTile atile;

			atile.Init();

			atile.X = tile->X;
			atile.Z = tile->Z;

			atile.L = tile->L + m_MegaTexTileLodOffset;
			atile.MegaTile = tile;

			atile.MegaConFlags = EAST_CONNECTION | SOUTH_CONNECTION;

			SetTileRenderVSConsts_G( &atile, gCam );

			//------------------------------------------------------------------------
			// float4x4 gWVP               : register( c0 );
			{
				float halfTileDim = m_HalfTileWorldDims [ atile.L ];
				float tileDim = m_TileWorldDims[ atile.L ];

				float x = atile.X * tileDim - r3dRenderer->LocalizationPos.x;
				float z = atile.Z * tileDim - r3dRenderer->LocalizationPos.z;
				float y = - r3dRenderer->LocalizationPos.y;

				float delta = 0.025f * halfTileDim;

				float dim = halfTileDim - delta;

				// fill in World
				D3DXMATRIX mtx (
						dim,				0.f,				0.f,				0.f,
						0.f,				m_HeightScale,		0.f,				0.f,
						0.f,				0.f,				dim,				0.f,
						dim + x + delta,	m_HeightOffset + y,	dim + z + delta,	1.f
					);

				r3dTL::TFixedArray< float4, 4 > vsConsts;

				D3DXMatrixMultiplyTranspose( (D3DXMATRIX*)&vsConsts[ 0 ], &mtx, &r3dRenderer->ViewProjMatrix_Localized );

				D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, &vsConsts[ 0 ].x, vsConsts.COUNT ) );
			}

			//------------------------------------------------------------------------

			m_HeightOffset = oldOffset;

			r3dRenderer->DrawIndexed(	D3DPT_LINESTRIP, 0, 0, 
										( MAX_VERTEX_DIM + 1 ) * ( MAX_VERTEX_DIM + 1 ),
										m_DebugVisIndexOffset, 
										MAX_VERTEX_DIM * 4 );
		}

		StopUsingTileGeom();

		r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
	}

	if( r_terrain2_show_mega_struture->GetInt() )
	{
		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );
		SetupTileRenderingVertexStates();

		r3dRenderer->SetTex( NULL, D3DVERTEXTEXTURESAMPLER0 );

		//------------------------------------------------------------------------

		r3dCamera camera;

		const r3dTerrainDesc& desc = GetDesc();

		float ss = sqrtf( desc.XSize * desc.XSize + desc.ZSize * desc.ZSize ) * 1.33f;

		camera.NearClip = 1.0f;
		camera.FarClip = 16.0f * ss;

		camera.SetOrtho( ss, ss );

		r3dPoint3D terraCentre( desc.XSize * 0.5f, 0.f, desc.XSize * 0.5f );

		camera.SetPosition( terraCentre - r3dPoint3D( 2.0f * ss, -2.0f *ss, 2.0f * ss ) );

		camera.PointTo( terraCentre );

		r3dRenderer->SetCamera( camera, false );

		Terrain3VertexShaderId vsid;

		vsid.shadowPath = 0;
		vsid.depthPath = 0;
		vsid.vfetchless = !m_AllowVFetch;
		vsid.cam_vec = 0;

		r3dRenderer->SetValidVertexShader( g_Terrain3VertexShaderIdMap[ vsid.Id ] );

		extern int g_FwdColorPS;
		r3dRenderer->SetPixelShader( g_FwdColorPS );


		for( int i = m_AllocMegaTexTileLodArray.Count() - 1 ; i >= 0 ; i -- )
		{
			MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ i ];

			float psColor[ 4 ] = { 0, 0, 0, 1 };

			if( i & 1 )
				psColor[ 0 ] = 1;

			if( i & 2 )
				psColor[ 1 ] = 1;

			if( i & 4 )
				psColor[ 2 ] = 1;

			if( !( i & 7 ) )
			{
				psColor[ 0 ] = 0.75f;
				psColor[ 1 ] = 0.75f;
				psColor[ 2 ] = 0.75f;
			}

			float psInvColr[ 4 ] = { 1, 1, 1, 1 };

			for( int c = 0, e = 3; c < e ; c ++ )
			{
				psInvColr[ c ] = 1.0f - psColor[ c ];
			}

			int lev = i;

			for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
			{
				MegaTexTile* tile = arr[ i ];

				AllocatedTile atile;

				atile.Init();

				atile.X = tile->X;
				atile.Z = tile->Z;

				if( !tile->IsLoaded && g_DEBUG_FrameId & 1 )
				{
					D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, psInvColr, 1 ) );
				}
				else
				{
					D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, psColor, 1 ) );
				}

				r3d_assert( tile->L == lev );

				atile.L = tile->L + r3dLog2( m_QualitySettings.VertexTilesInMegaTexTileCount );

				SetTileRenderVSConsts_G( &atile, gCam );

				//------------------------------------------------------------------------
				// float4x4 gWVP               : register( c0 );
				{
					float halfTileDim = m_HalfTileWorldDims [ atile.L ];
					float tileDim = m_TileWorldDims[ atile.L ];

					float x = atile.X * tileDim;
					float z = atile.Z * tileDim;
					float y = 0.f;

					float dim = halfTileDim;

					int invL = m_NumActiveMegaTexQualityLayers - tile->L;

					// fill in World
					D3DXMATRIX mtx (
							dim,		0.f,					0.f,		0.f,
							0.f,		0.f,					0.f,		0.f,
							0.f,		0.f,					dim,		0.f,
							dim + x,	invL * ss * 0.0125f,	dim + z,	1.f
						);

					r3dTL::TFixedArray< float4, 4 > vsConsts;

					D3DXMatrixMultiplyTranspose( (D3DXMATRIX*)&vsConsts[ 0 ], &mtx, &r3dRenderer->ViewProjMatrix );

					D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, &vsConsts[ 0 ].x, vsConsts.COUNT ) );
				}

				int MAX_VERTEX_DIM = m_QualitySettings.VertexTileDim;

				for( int i = 0, e = m_QualitySettings.TileVertexDensities.COUNT; i < e; i ++ )
				{
					MAX_VERTEX_DIM = R3D_MAX( MAX_VERTEX_DIM, m_QualitySettings.VertexTileDim * ( 1 << m_QualitySettings.TileVertexDensities[ i ] ) );
				}

				r3dRenderer->DrawIndexed(	D3DPT_LINESTRIP, 0, 0, 
											( MAX_VERTEX_DIM + 1 ) * ( MAX_VERTEX_DIM + 1 ),
											m_DebugVisIndexOffset, 
											MAX_VERTEX_DIM * 4 );

			}
		}

		//------------------------------------------------------------------------

		r3dRenderer->SetCamera( gCam, false );

		r3dRenderer->SetRenderingMode( R3D_BLEND_POP );

		StopUsingTileGeom();
	}

	if( r_terrain3_showeditable->GetInt() )
	{
		UpdateVisibleTiles();

		const QualitySettings& qs = GetCurrentQualitySettings();

		int physStartX, physEndX, physStartZ, physEndZ;

		GetPhysHeightmapBoundaries( &physStartX, &physStartZ, &physEndX, &physEndZ );

		StartTileRendering();

		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_ZC | R3D_BLEND_PUSH );

		extern int g_FwdColorPS;

		r3dRenderer->SetPixelShader( g_FwdColorPS );

		float psConst[ 4 ] = { 1, 0, 0, 0.133f };

		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, psConst, 1 ) );

		for( int i = 0, e  = m_VisibleTiles.Count(); i < e; i ++ )
		{
			const AllocatedTile* tile = m_VisibleTiles[ i ];

			int tileSize = qs.VertexTileDim << tile->L;

			int tileX = tile->X * qs.VertexTileDim << tile->L;
			int tileZ = tile->Z * qs.VertexTileDim << tile->L;

			if( tileX < physStartX || tileX + tileSize > physEndX
					||
				tileZ < physStartZ || tileZ + tileSize > physEndZ )
			{
				RenderTile_G( tile, gCam, true, true );
			}
		}

		EndTileRendering();

		r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
	}

	if( r_show_terraupdates->GetInt() )
	{
		int totalAllocated = 0;

		for( int L = 0, e = (int)m_AllocTileLodArray.Count(); L < e; L ++ )
		{
			AllocatedTileArr& arr = m_AllocTileLodArray[ L ];

			for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
			{
				AllocatedTile& tile = arr[ i ];

				if( tile.AtlasTileID >= 0 )
				{
					totalAllocated ++;
				}
			}
		}

		r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );

		char updates[ 512 ];

		char* ptr = updates;

		ptr += sprintf( ptr, "%d:", totalAllocated );

		for( int i = 0, e = (int)m_TileUpdateLog.Count(); i < e; i ++ )
		{
			int n = sprintf( ptr, "%d,", m_TileUpdateLog[ m_TileUpdateLog.Count() - 1 - i ]  );

			if( n > 0 )
				ptr += n;
		}

		_r3dSystemFont->PrintF( 300.f, 300.f, r3dColor::white, updates );

		r3dRenderer->SetRenderingMode( R3D_BLEND_POP );
	}
#endif
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::RecalcLayerVars()
{
#if R3D_TERRAIN_V3_GRAPHICS
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
		const r3dTerrain3Layer& layer = m_Layers[ i ];
		if(g_pMaterialTypes)
			m_MatTypeIdxes[ i + 1 ] = g_pMaterialTypes->GetIdx( layer.MaterialTypeName );
	}
#endif
}

//------------------------------------------------------------------------

static void check_fread( void* p, size_t size, r3dFile* fin )
{
	size_t num_read = fread( p, size, 1, fin );
	r3d_assert( num_read == 1 );
}

void ReadTerrainHeader( r3dFile* file, uint32_t& dwSignature, uint32_t& dwVersion );


//------------------------------------------------------------------------

void
r3dTerrain3::SaveEmpty( const CreationParams& createParams )
{
#if R3D_TERRAIN_V3_GRAPHICS
	PopulateInferiorQualitySettings();

	const QualitySettings& saveQS = m_BaseQualitySettings[ QS_HIGH ];

	m_VertexCountX = createParams.CellCountX;
	m_VertexCountZ = createParams.CellCountZ;

	m_MaskResolutionU = createParams.MaskSizeX;
	m_MaskResolutionV = createParams.MaskSizeZ;

	m_TileUnitWorldDim = createParams.CellSize;
	m_HeightOffset = createParams.StartHeight;
	m_HeightScale = createParams.HeightRange;

	int tcx = m_VertexCountX / saveQS.VertexTileDim; 
	int tcz = m_VertexCountZ / saveQS.VertexTileDim;

	m_NumActiveQualityLayers = 0;

	for(; tcx > 0 && tcz > 0; )
	{
		tcx /= 2, tcz /= 2;

		m_NumActiveQualityLayers ++;
	}

	UpdateMegaTexQualityLayerCount();

	SetupHFScale();

	InitTileInfoMipChain( saveQS );
		
	DoSave( createParams.LevelDir.c_str(), m_VertexCountX, m_VertexCountZ, false );

	CreateMegaTexFile( createParams.LevelDir.c_str(), 0 );
	SaveMegaTexMapFile( createParams.LevelDir.c_str() );
#endif
}


//------------------------------------------------------------------------

int	
r3dTerrain3::Save( const char* targetDir )
{
#if R3D_TERRAIN_V3_GRAPHICS

	gSaveCountDown = 5.0f;

	return DoSave( targetDir, m_VertexCountX, m_VertexCountZ, true );
#else
	return 1;
#endif
}

//------------------------------------------------------------------------


int	 r3dTerrain3::Load()
{
	DoLoad( r3dGameLevel::GetHomeDir() );
	return 1;
}

//------------------------------------------------------------------------

const r3dTerrain3::Settings&
r3dTerrain3::GetSettings() const
{
	return m_Settings;
}

//------------------------------------------------------------------------

void
r3dTerrain3::SetSettings( const Settings& sts )
{
	Settings msettings = sts;

	msettings.FarDiffuseTexture = m_Settings.FarDiffuseTexture;
	msettings.FarNormalTexture = m_Settings.FarNormalTexture;

	if( m_Settings.FarDiffuseTextureName != sts.FarDiffuseTextureName )
	{
		if( msettings.FarDiffuseTexture )
		{
			r3dRenderer->DeleteTexture( msettings.FarDiffuseTexture );
			msettings.FarDiffuseTexture = NULL;
		}

		if( sts.FarDiffuseTextureName.Length() > 0 )
		{
			msettings.FarDiffuseTexture = r3dRenderer->LoadTexture( sts.FarDiffuseTextureName.c_str() );
		}
	}

	if( m_Settings.FarNormalTextureName != sts.FarNormalTextureName )
	{
		if( msettings.FarNormalTexture )
		{
			r3dRenderer->DeleteTexture( msettings.FarNormalTexture );
			msettings.FarNormalTexture = NULL;
		}

		if( sts.FarNormalTextureName.Length() > 0 )
		{
			msettings.FarNormalTexture = r3dRenderer->LoadTexture( sts.FarNormalTextureName.c_str() );
		}
	}

	m_Settings = msettings;
}

//------------------------------------------------------------------------

const r3dTerrain3::QualitySettings&
r3dTerrain3::GetBaseQualitySettings( int ql ) const
{
	return m_BaseQualitySettings[ ql ];
}

//------------------------------------------------------------------------

const r3dTerrain3::QualitySettings&
r3dTerrain3::GetCurrentQualitySettings() const
{
	return m_QualitySettings;
}

//------------------------------------------------------------------------

void
r3dTerrain3::SetQualitySettings( const QualitySettings& settings, bool affectBaseQS, int affectBaseQSIdx )
{
#if R3D_TERRAIN_V3_GRAPHICS
	// some terrain texture tiles may still be loading
	r3dFinishBackGroundTasks();
#endif

	QualitySettings fixedSettings = settings;

	fixedSettings.Sync( &GetDesc() );

#if R3D_TERRAIN3_ALLOW_FULL_PHYSCHUNKS
	if( m_FullPhysChunkModeOn )
	{
		fixedSettings.PhysicsTileCellCount = GetPhysChunkSize();
	}
	else
#endif
	{
		fixedSettings.PhysicsTileCellCount = GAME_PHYS_CHUNK_SIZE;
	}

#if R3D_TERRAIN_V3_GRAPHICS
	if( m_QualitySettings.BakeRoads && !settings.BakeRoads )
	{
		ReleaseRoads();
	}

	if( m_QualitySettings.CompareNoRoads( fixedSettings ) )
	{
		CloseDynamic();
		m_QualitySettings = fixedSettings;
		RecalcVars();
		RecalcLayerVars();
		InitDynamic();
 
		InitTileField();
		InitMegaTexTileField();

		UpdateRoadInfos();

		if( affectBaseQS )
		{
			m_BaseQualitySettings[ affectBaseQSIdx ] = m_QualitySettings;
		}
	}
	else
	{
		if( m_QualitySettings.BakeRoads != fixedSettings.BakeRoads )
		{
			m_QualitySettings = fixedSettings;

			if( affectBaseQS )
			{
				m_BaseQualitySettings[ affectBaseQSIdx ] = m_QualitySettings;
			}

			UpdateRoadInfos();
		}
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::GetStats( r3dTerrain3Stats* oStats ) const
{
	*oStats = r3dTerrain3Stats();

#if R3D_TERRAIN_V3_GRAPHICS
	oStats->VolumeCount = m_Atlas.Count();
	oStats->MaskVolumeCount = m_MaskAtlas.Count();
	oStats->NormalAndHeightVolumeCount = m_HeightNormalAtlas.Count();

	for( int i = 0, e = (int) m_Atlas.Count(); i < e; i ++ )
	{
		const AtlasVolume& vol = m_Atlas[ i ];
		oStats->TileCount += vol.Occupied.Count() - vol.FreeTiles;
		oStats->TileTextureMemory += vol.Diffuse->Tex->GetTextureSizeInVideoMemory();
		oStats->TileTextureMemory += vol.Normal->Tex->GetTextureSizeInVideoMemory();
	}

	for( int i = 0, e = (int)m_AllocMegaTexTileLodArray.Count(); i < e; i ++ )
	{
		const MegaTexTilePtrArr& mttPtrArr = m_AllocMegaTexTileLodArray[ i ];
		oStats->MegaTileCount += mttPtrArr.Count();
	}

	for( int i = 0, e = (int)m_MaskAtlas.Count(); i < e; i ++ )
	{
		oStats->MaskTextureMemory += m_MaskAtlas[ i ].Mask->GetTextureSizeInVideoMemory();
	}

	typedef r3dgfxSet( r3dTexture* ) TextureSet;
	TextureSet uniqueLayerTexes;

	for( int i = 0, e = (int)m_Layers.Count(); i < e; i ++ )
	{
		const r3dTerrain3Layer& layer = m_Layers[ i ];
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
r3dTerrain3::ResetAtlasTiles()
{
#if R3D_TERRAIN_V3_GRAPHICS
	RemoveAllocatedTiles();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::RefreshAtlasTiles()
{
#if R3D_TERRAIN_V3_GRAPHICS
	m_TilesToUpdate.Clear();

	// free all tiles to force reallocate & update
	for( int i = 0, e = (int)m_AllocTileLodArray.Count(); i < e; i ++ )
	{
		AllocatedTileArr& arr = m_AllocTileLodArray[ i ];

		for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
		{
			AllocatedTile& tile = arr[ i ];

			if( tile.AtlasVolumeID >= 0 )
			{
				tile.Dirty = 1;
			}
		}
	}
#endif
}

void r3dTerrain3::RefreshAtlasTilesAndFreeAtlas()
{
	for( int i = 0, e = (int)m_AllocTileLodArray.Count(); i < e; i ++ )
	{
		AllocatedTileArr& arr = m_AllocTileLodArray[ i ];

		for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
		{
			AllocatedTile& tile = arr[ i ];

			tile.AtlasVolumeID = -1;
			tile.AtlasTileID = -1;
		}
	}

	FreeAtlas();
}

//------------------------------------------------------------------------

void r3dTerrain3::RefreshAllTiles()
{
	m_TilesToUpdate.Clear();
	m_VisibleTiles.Clear();
	m_DebugVisibleTiles.Clear();
	m_DebugVisibleMegaTexTiles.Clear();

	FreeAtlasTiles();
	FreeMegaAtlasTiles();
}

//------------------------------------------------------------------------

void
r3dTerrain3::RefreshAtlasTiles( const RECT& rect )
{
#if R3D_TERRAIN_V3_GRAPHICS
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
r3dTerrain3::RefreshAtlasTile( int tileX, int tileZ )
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

#if R3D_TERRAIN_V3_GRAPHICS
	for( int L = 0, e = m_NumActiveQualityLayers, tx = tileX, tz = tileZ; L < e; L ++ )
	{
		AllocatedTileArr& arr = m_AllocTileLodArray[ L ];

		for( int i = 0, e = arr.Count(); i < e; i ++ )
		{
			AllocatedTile& tile = arr[ i ];

			if( tile.X == tx && tile.Z == tz )
			{
				if( tile.AtlasTileID >= 0)
				{
					FreeAtlasTile( &tile );
				}
			}
		}

		tx /= 2;
		tz /= 2;
	}
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::RefreshAllMaskAtlasTiles()
{
	Info info = GetInfo();

	for( int tz = 0, e = info.MegaTileCountZ; tz < e; tz ++ )
	{
		for( int tx = 0, e = info.MegaTileCountX; tx < e; tx ++ )
		{
			for( int L = 0, e = info.NumActiveMegaTexLayers; L < e; L ++ )
			{
				Terrain3->RefreshMaskAtlasTile( tx, tz, L, RM_RELOAD );
			}
		}
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::RefreshMaskAtlasTile( int tileX, int tileZ, int L, RefreshMode mode )
{
#if R3D_TERRAIN_V3_GRAPHICS
	R3DPROFILE_FUNCTION( "r3dTerrain3::RefreshMaskAtlasTile" );

	MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ L ];

	for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
	{
		MegaTexTile& tile = *arr[ i ];

		if( tile.X == tileX && tile.Z == tileZ )
		{
			AppendToMaskAtlasTile( &tile );

			MegaTexTile::MaskEntryList::Iterator iter = tile.MaskList.GetIterator();

			if( iter.IsValid() )
			{
				r3dOutToLog( "Marked tile(%d,%d,%d) has: ", tile.X, tile.Z, (int)tile.L );

				for( ; iter.IsValid(); iter.Advance() )
				{
					r3dOutToLog( "%d", (int)iter.GetValue() );
				}

				r3dOutToLog( "\n" );
			}

			switch( mode )
			{
			case RM_RELOAD:
				tile.IsLoaded = 0;
				AddMegaTexTileLoadJobs( &tile, LOADTILE_MASKS | LOADTILE_UPDATEATLAS );
				break;
			case RM_UPDATE:
				AddMegaTexTileUpdateJobs( &tile, LOADTILE_MASKS | LOADTILE_UPDATEATLAS );
				break;
			}
		}
	}
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::RefreshHeightNormalAtlasTile( int tileX, int tileZ, int L, int flags )
{
#if R3D_TERRAIN_V3_GRAPHICS
	MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ L ];

	for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
	{
		MegaTexTile& tile = *arr[ i ];

		if( tile.X == tileX && tile.Z == tileZ )
		{
			AddMegaTexTileUpdateJobs( &tile, flags | LOADTILE_UPDATEATLAS );
		}
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::SetPhysUserData( void* data )
{
	m_PhysUserData = data;
}

//------------------------------------------------------------------------

void r3dTerrain3::UpdateRoadInfos()
{
	R3DPROFILE_FUNCTION( "r3dTerrain3::UpdateRoadInfos" );

	TurnOffBakedRoads();

	for( int i = 0, e = (int)m_AllocMegaTexTileLodArray.Count(); i < e; i ++ )
	{
		MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ i ];

		for( int i = 0, e = arr.Count(); i < e; i ++ )
		{
			UpdateRoadInfo( arr[ i ] );
		}
	}

	m_RoadInfosUpdated = 1;
}

//------------------------------------------------------------------------

void r3dTerrain3::UpdateRoadInfo( MegaTexTile* tile )
{
#if R3D_TERRAIN_V3_GRAPHICS
	const QualitySettings& qs = GetCurrentQualitySettings();

	if( obj_Road::AllRoads.Count() )
	{
		int megaMipOffset = m_MegaTexTileLodOffset;

		int roadCellsInTile = qs.VertexTilesInMegaTexTileCount / qs.RoadTileDiv;

		int numMips = r3dLog2( roadCellsInTile ) + 1;

		int roadMipOffset = tile->L + megaMipOffset - numMips + 1;

		tile->RoadMipChain.Resize( numMips );

		for( int m = 0, e = numMips, rc = roadCellsInTile; m < e; 
				m ++,
				rc /= 2
				)
		{
			r3d_assert( rc );

			float megaTileDim = m_TileWorldDims[ tile->L + megaMipOffset ];

			float fz = tile->Z * megaTileDim, fx = 0.f;
			float dim = m_TileWorldDims[ roadMipOffset + m ];

			float fxStart = tile->X * megaTileDim;

			for( int z = 0, e = rc; z < e; z ++, fz += dim )
			{
				fx = fxStart;
				for( int x = 0, e = rc; x < e; x ++, fx += dim )
				{
					int cleared = 0;

					for( int i = 0, e = obj_Road::AllRoads.Count(); i < e; i ++ )
					{
						obj_Road* road = obj_Road::AllRoads[ i ];

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

						MegaTexTile::RoadArr2D& mip = tile->RoadMipChain[ m ];

						if( !mip.Count() )
						{
							mip.Resize( rc, rc );
						}

						MegaTexTile::RoadLinkedList& rll = mip[ z ][ x ];

						if( !cleared )
						{
							rll.ClearValues();
							cleared = 1;
						}

						rll.PushBack( road );
					}
				}
			}
		}
	}
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::OnRoadDelete( class obj_Road* road )
{
	for( int i = 0, e = (int)m_AllocMegaTexTileLodArray.Count(); i < e; i ++ )
	{
		MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ i ];

		for( int i = 0, e = arr.Count(); i < e; i ++ )
		{
			MegaTexTile* mt = arr[ i ];

			for( int i = 0, e = mt->RoadMipChain.Count(); i < e; i ++ )
			{
				MegaTexTile::RoadArr2D& arr2d = mt->RoadMipChain[ i ];
				for( int z = 0, e = arr2d.Height(); z < e; z ++ )
				{
					for( int x = 0, e = arr2d.Width(); x < e; x ++ )
					{
						MegaTexTile::RoadLinkedList& rll = arr2d[ z ][ x ];

						MegaTexTile::RoadLinkedList::Iterator iter = rll.GetIterator();

						for( ; iter.IsValid(); iter.Advance() )
						{
							obj_Road* r = iter.GetValue();

							if( r == road )
							{
								rll.Erase( iter );
								break;
							}
						}
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------------

void
r3dTerrain3::ReleaseRoads()
{
#if R3D_TERRAIN_V3_GRAPHICS
	for( int i = 0, e = (int)obj_Road::AllRoads.Count(); i < e; i ++ )
	{
		obj_Road::AllRoads[ i ]->ObjFlags &= ~( OBJFLAG_SkipDraw | OBJFLAG_SkipCastRay );
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::TurnOffBakedRoads()
{
#if R3D_TERRAIN_V3_GRAPHICS
	if( GetCurrentQualitySettings().BakeRoads )
	{
		for( int i = 0, e = (int)obj_Road::AllRoads.Count(); i < e; i ++ )
		{
			obj_Road* obj = obj_Road::AllRoads[ i ];

			if( obj->ObjTypeFlags & OBJTYPE_Road )
			{
				obj->ObjFlags |= OBJFLAG_SkipDraw | OBJFLAG_SkipCastRay;
				obj->Update();
			}
		}
	}
#endif
}


//------------------------------------------------------------------------

bool
r3dTerrain3::UpdateHeightRanges( const Floats& floatHeights )
{
#if R3D_TERRAIN_V3_GRAPHICS
	R3DPROFILE_FUNCTION( "r3dTerrain3::UpdateHeightRanges" );

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
r3dTerrain3::ConvertHeightField( Shorts* oHeights, const Floats& floatHeights, const RECT* rect )
{
#if R3D_TERRAIN_V3_GRAPHICS
	R3DPROFILE_FUNCTION( "r3dTerrain3::ConvertHeightField" );

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
r3dTerrain3::UpdateGraphicsHeightField( const Shorts& shorts, const RECT* rect )
{
#if R3D_TERRAIN_V3_GRAPHICS
	
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::RecalcTileField( int tileX, int tileZ, int L, const UShorts& heightField )
{
	R3DPROFILE_FUNCTION( "r3dTerrain3::RecalcTileField" );
	RecalcTileInfo( tileX, tileZ, L, heightField );
}
//------------------------------------------------------------------------

void r3dTerrain3::UnpackHeight_OLD( r3dTL::TArray< UINT16 > * oData, int X, int Z, int L )
{
#ifndef FINAL_BUILD
	r3dCSHolderWithDeviceQueue cs( m_MegaTexFileCritSec ); ( void )cs;
#endif

	MegaTexTile tile;
	tile.X = X;
	tile.Z = Z;
	tile.L = L;

	MegaTexFileOffset offset = GetMegaTexHeightNormalOffsetInFile_OLD( &tile );

#ifndef FINAL_BUILD
	if( offset.IsInEditorFile )
	{
		int mapSize = GetHeightNormalRecordSizeInFile();
		r3dMappedViewOfFile mappedViewOfFile = r3dCreateMappedViewOfFile( m_MegaTexAtlasFile_EDITOR_Mapping, FILE_MAP_READ, offset.EditorValue, mapSize );
		UnpackHeightFrom( &mappedViewOfFile, 0, oData, X, Z, L );
		UnmapViewOfFile( mappedViewOfFile.Mapped );
	}
	else
#endif
	{
		UnpackHeightFrom( m_MegaTexAtlasFile, offset.Value, oData, X, Z, L );
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::UnpackHeight( r3dTL::TArray< UINT16 > * oData, int X, int Z, int L )
{
	R3DPROFILE_FUNCTION( "UnpackHeight" );

#ifndef FINAL_BUILD
	r3dCSHolderWithDeviceQueue cs( m_MegaTexFileCritSec ); ( void )cs;
#endif

	MegaTexTile tile;
	tile.X = X;
	tile.Z = Z;
	tile.L = L;

	MegaTexFileGridOffset offset = GetMegaTexHeightNormalOffsetInFile( &tile );

#ifndef FINAL_BUILD
	if( offset.IsInEditorFile )
	{
		int mapSize = GetHeightNormalRecordSizeInFile();
		r3dMappedViewOfFile mappedViewOfFile = r3dCreateMappedViewOfFile( m_MegaTexAtlasFile_EDITOR_Mapping, FILE_MAP_READ, offset.EditorValue, mapSize );
		UnpackHeightFrom( &mappedViewOfFile, 0, oData, X, Z, L );
		UnmapViewOfFile( mappedViewOfFile.Mapped );
	}
	else
#endif
	{
		if( offset.GridId == GRID_ID_MIPSFILE )
		{
			UnpackHeightFrom( m_MegaTexAtlasMipsFile, offset.Value, oData, X, Z, L );
		}
		else
		{
			R3D_FILE_SYSTEM_LOCK_SCOPE();
			int gridX( 0 ), gridZ( 0 );
			GetGridCoordinates( offset, &gridX, &gridZ );
			UnpackHeightFrom( m_FileGrid[ gridZ ][ gridX ], offset.Value, oData, X, Z, L );
		}
	}
}

//------------------------------------------------------------------------

r3dTerrain3::UShorts& r3dTerrain3::UnpackHeightCached( int X, int Z )
{
	R3DPROFILE_FUNCTION( "UnpackHeightCached" );

	for( int i = 0, e = (int)m_UnpackedHeightCache.Count(); i < e; i ++ )
	{
		UnpackedHeightTile& uht = m_UnpackedHeightCache[ i ];

		if( uht.tileX == X && uht.tileZ == Z )
		{
			DefreshCachedHeights();

			uht.Freshness = 0;
			return uht.height;
		}
	}

	int maxFreshnessI = 0;

	int maxFreshness = m_UnpackedHeightCache[ 0 ].Freshness;

	for( int i = 1, e = (int)m_UnpackedHeightCache.Count(); i < e; i ++ )
	{
		UnpackedHeightTile& uht = m_UnpackedHeightCache[ i ];

		if( uht.Freshness > maxFreshness )
		{
			maxFreshnessI = i;
			maxFreshness = uht.Freshness;
		}
	}

	UnpackedHeightTile& uht = m_UnpackedHeightCache[ maxFreshnessI ];

#ifndef FINAL_BUILD
	r3dOutToLog( "r3dTerrain3::UnpackHeightCached: unpacking (%d,%d) for [%d]\n", X, Z, maxFreshnessI );
#endif

	UnpackHeight( &uht.height, X, Z, 0 );

	DefreshCachedHeights();

	uht.Freshness = 0;
	uht.tileX = X;
	uht.tileZ = Z;

	return uht.height;
}

void r3dTerrain3::DefreshCachedHeights()
{
	for( int i = 0, e = (int)m_UnpackedHeightCache.Count(); i < e; i ++ )
	{
		UnpackedHeightTile& uht = m_UnpackedHeightCache[ i ];

		if( uht.Freshness < INT_MAX )
			uht.Freshness ++;
	}
}

//------------------------------------------------------------------------

template< typename T >
void r3dTerrain3::UnpackHeightFrom( T file, INT64 offset, r3dTL::TArray< UINT16 > * oData, int X, int Z, int L )
{
	MegaTexTile fakeTile;

	fakeTile.X = X;
	fakeTile.Z = Z;

	fakeTile.L = L;

	oData->Resize( GetHeightTileSizeInFile() / sizeof( (*oData)[ 0 ] ) );

	DoLoadMegaTexTileHeightAndNormalFromFile( file, offset, &fakeTile, NULL, &(*oData)[ 0 ], LOADTILE_HEIGHT );
}

//------------------------------------------------------------------------

#ifndef FINAL_BUILD

int r3dTerrain3::HasMask_OLD( int X, int Z, int L, int MaskId )
{
	r3dCSHolderWithDeviceQueue cs( m_MegaTexFileCritSec ); ( void )cs;

	INT64 id = r3dTerrain3::MegaTexTile::GetID( X, Z, L, MaskId );

	FileTextureMap::const_iterator i = m_MegaTexAtlasFile_Map.find( id );

	return  i != m_MegaTexAtlasFile_Map.end();
}

//------------------------------------------------------------------------

#endif

int r3dTerrain3::HasMask( int X, int Z, int L, int MaskId )
{
#ifndef FINAL_BUILD
	r3dCSHolderWithDeviceQueue cs( m_MegaTexFileCritSec ); ( void )cs;
#endif

	INT64 id = r3dTerrain3::MegaTexTile::GetID( X, Z, L, MaskId );

	FileTextureGridMap::const_iterator i = m_MegaTexGridFile_Map.find( id );

	return  i != m_MegaTexGridFile_Map.end();
}

#ifndef FINAL_BUILD

//------------------------------------------------------------------------

void r3dTerrain3::UnpackMask_OLD( r3dTL::TArray< UINT16 > * oData, IDirect3DTexture9* TempPackMask, IDirect3DTexture9* TempMask, int X, int Z, int L, int MaskId )
{
	r3dCSHolderWithDeviceQueue cs( m_MegaTexFileCritSec ); ( void )cs;

	INT64 id = r3dTerrain3::MegaTexTile::GetID( X, Z, L, MaskId );

	FileTextureMap::const_iterator i = m_MegaTexAtlasFile_Map.find( id );

	if( i != m_MegaTexAtlasFile_Map.end() )
	{
		if( i->second.IsInEditorFile )
		{
			int mapSize = GetMaskTileSizeInFile() + sizeof( short );
			r3dMappedViewOfFile mappedViewOfFile = r3dCreateMappedViewOfFile( m_MegaTexAtlasFile_EDITOR_Mapping, FILE_MAP_READ, i->second.EditorValue, mapSize );
			UnpackMaskFrom( &mappedViewOfFile, 0, oData, TempPackMask, TempMask, X, Z, L, MaskId );
			UnmapViewOfFile( mappedViewOfFile.Mapped );
		}
		else
		{
			UnpackMaskFrom( m_MegaTexAtlasFile, i->second.Value, oData, TempPackMask, TempMask, X, Z, L, MaskId );
		}
	}
	else
	{
		Info info = GetInfo();

		int tileCountX = info.MegaTileCountX >> L;
		int tileCountZ = info.MegaTileCountZ >> L;

		r3d_assert( X < tileCountX && X >= 0 );
		r3d_assert( Z < tileCountZ && Z >= 0 );

		MegaTexFileOffset mtfoffset;

		mtfoffset.EditorValue		= m_MegaTexAtlasFile_EDITOR_Size;
		mtfoffset.IsInEditorFile	= 1;

		D3DSURFACE_DESC sdesc;
		m_EDITOR_BlankMask.GetLevelDesc2D( 0, &sdesc );

		D3DLOCKED_RECT lrect;
		m_EDITOR_BlankMask.LockRect( 0, &lrect, NULL, D3DLOCK_READONLY );

		short inFileMaskId = (short)MaskId;

		DWORD texSize = r3dGetTextureBlockLineSize( sdesc.Width, sdesc.Format ) * r3dGetTextureBlockHeight( sdesc.Height, sdesc.Format );

		int sizeToMap = sizeof( short ) + texSize;

		r3dMappedViewOfFile mvof = r3dCreateMappedViewOfFile( m_MegaTexAtlasFile_EDITOR_Mapping, FILE_MAP_WRITE, mtfoffset.EditorValue, sizeToMap );
		void* start = mvof.Start;
		void* end = (char*)mvof.Start + sizeToMap;

		void* ptr = start;

		*((short*&)ptr)++ = inFileMaskId;

		memcpy( ptr, lrect.pBits, texSize );
		(char*&)ptr += texSize;

		r3d_assert( ptr == end );

		m_MegaTexAtlasFile_EDITOR_Size += sizeToMap;

		int res;
		res = UnmapViewOfFile( mvof.Mapped ); r3d_assert( res );

		m_EDITOR_BlankMask.UnlockRect( 0 );

		m_MegaTexAtlasFile_Map.insert( FileTextureMap::value_type( id , mtfoffset ) );
		UnpackMask( oData, TempPackMask, TempMask, X, Z, L, MaskId );
	}
}

#endif

//------------------------------------------------------------------------

void
r3dTerrain3::UnpackMask( r3dTL::TArray< UINT16 > * oData, IDirect3DTexture9* TempPackMask, IDirect3DTexture9* TempMask, int X, int Z, int L, int MaskId )
{
#ifndef FINAL_BUILD
	r3dCSHolderWithDeviceQueue cs( m_MegaTexFileCritSec ); ( void )cs;
#endif

	INT64 id = r3dTerrain3::MegaTexTile::GetID( X, Z, L, MaskId );

	FileTextureGridMap::const_iterator i = m_MegaTexGridFile_Map.find( id );

	if( i != m_MegaTexGridFile_Map.end() )
	{
		if( i->second.IsInEditorFile )
		{
			int mapSize = GetMaskTileSizeInFile() + sizeof( short );
			r3dMappedViewOfFile mappedViewOfFile = r3dCreateMappedViewOfFile( m_MegaTexAtlasFile_EDITOR_Mapping, FILE_MAP_READ, i->second.EditorValue, mapSize );
			UnpackMaskFrom( &mappedViewOfFile, 0, oData, TempPackMask, TempMask, X, Z, L, MaskId );
			UnmapViewOfFile( mappedViewOfFile.Mapped );
		}
		else
		{
			MegaTexFileGridOffset offset = i->second;

			if( offset.GridId == GRID_ID_MIPSFILE )
			{
				UnpackMaskFrom( m_MegaTexAtlasMipsFile, offset.Value, oData, TempPackMask, TempMask, X, Z, L, MaskId );
			}
			else
			{
				R3D_FILE_SYSTEM_LOCK_SCOPE();
				int x, z;
				GetGridCoordinates( offset, &x, &z );
				UnpackMaskFrom( m_FileGrid[ z ][ x ], offset.Value, oData, TempPackMask, TempMask, X, Z, L, MaskId );
			}
		}
	}
	else
	{
		Info info = GetInfo();

		int tileCountX = info.MegaTileCountX >> L;
		int tileCountZ = info.MegaTileCountZ >> L;

		r3d_assert( X < tileCountX && X >= 0 );
		r3d_assert( Z < tileCountZ && Z >= 0 );

		MegaTexFileGridOffset mtfoffset;

		mtfoffset.EditorValue		= m_MegaTexAtlasFile_EDITOR_Size;
		mtfoffset.IsInEditorFile	= 1;

		D3DSURFACE_DESC sdesc;
		m_EDITOR_BlankMask.GetLevelDesc2D( 0, &sdesc );

		D3DLOCKED_RECT lrect;
		m_EDITOR_BlankMask.LockRect( 0, &lrect, NULL, D3DLOCK_READONLY );

		short inFileMaskId = (short)MaskId;

		DWORD texSize = r3dGetTextureBlockLineSize( sdesc.Width, sdesc.Format ) * r3dGetTextureBlockHeight( sdesc.Height, sdesc.Format );

		int sizeToMap = sizeof( short ) + texSize;

		r3dMappedViewOfFile mvof = r3dCreateMappedViewOfFile( m_MegaTexAtlasFile_EDITOR_Mapping, FILE_MAP_WRITE, mtfoffset.EditorValue, sizeToMap );
		void* start = mvof.Start;
		void* end = (char*)mvof.Start + sizeToMap;

		void* ptr = start;

		*((short*&)ptr)++ = inFileMaskId;

		memcpy( ptr, lrect.pBits, texSize );
		(char*&)ptr += texSize;

		r3d_assert( ptr == end );

		m_MegaTexAtlasFile_EDITOR_Size += sizeToMap;

		int res;
		res = UnmapViewOfFile( mvof.Mapped ); r3d_assert( res );

		m_EDITOR_BlankMask.UnlockRect( 0 );

		m_MegaTexGridFile_Map.insert( FileTextureGridMap::value_type( id , mtfoffset ) );

		UnpackMask( oData, TempPackMask, TempMask, X, Z, L, MaskId );
	}
}

//------------------------------------------------------------------------

template < typename T >
struct UnpackMaskParams
{
	T file;
	INT64 offset;
	r3dTL::TArray< UINT16 > * oData;
	IDirect3DTexture9* TempPackedMask;
	IDirect3DTexture9* TempMask;
	int X;
	int Z;
	int L;
	int MaskId;

	r3dTerrain3* master;
};

template < typename T >
static void UnpackMaskHelper( void* params )
{
	UnpackMaskParams< T > * tparms = static_cast< UnpackMaskParams< T >* > ( params );
	tparms->master->UnpackMaskFromInMainThread(	tparms->file, tparms->offset, tparms->oData,
												tparms->TempPackedMask, tparms->TempMask,
												tparms->X, tparms->Z, tparms->L, tparms->MaskId );

}

template< typename T >
void
r3dTerrain3::UnpackMaskFrom( T file, INT64 offset, r3dTL::TArray< UINT16 > * oData, IDirect3DTexture9* TempPackedMask, IDirect3DTexture9* TempMask, int X, int Z, int L, int MaskId )
{
	if( R3D_IS_MAIN_THREAD() )
	{
		UnpackMaskFromInMainThread( file, offset, oData, TempPackedMask, TempMask, X, Z, L, MaskId );
	}
	else
	{
		UnpackMaskParams< T > params;

		params.file				= file;
		params.offset			= offset;
		params.oData			= oData;
		params.TempPackedMask	= TempPackedMask;
		params.TempMask			= TempMask;
		params.X				= X;
		params.Z				= Z;
		params.L				= L;
		params.MaskId			= MaskId;

		params.master			= this;

		ProcessCustomDeviceQueueItem(  UnpackMaskHelper< T >, &params );
	}
}


//------------------------------------------------------------------------

template< typename T >
void
r3dTerrain3::UnpackMaskFromInMainThread( T file, INT64 offset, r3dTL::TArray< UINT16 > * oData, IDirect3DTexture9* TempPackMask, IDirect3DTexture9* TempMask, int X, int Z, int L, int MaskId )
{
	D3DSURFACE_DESC sdesc;
	TempMask->GetLevelDesc( 0, &sdesc );

	r3dD3DTextureTunnel tunnel;

	tunnel.Set( TempPackMask );

	DoLoadMegaTexTileMaskFromFile( file, offset, &sdesc, tunnel, MaskId, NULL, NULL, NULL, 0 );

	IDirect3DSurface9* destSurf( NULL );
	D3D_V( TempMask->GetSurfaceLevel( 0, &destSurf ) );

	IDirect3DSurface9* srcSurf( NULL );
	D3D_V( TempPackMask->GetSurfaceLevel( 0, &srcSurf ) );

	D3D_V( D3DXLoadSurfaceFromSurface( destSurf, NULL, NULL, srcSurf, NULL, NULL, D3DX_FILTER_NONE, 0 ) );

	D3DLOCKED_RECT lrect;

	D3D_V( destSurf->LockRect( &lrect, NULL, D3DLOCK_READONLY ) );

	oData->Resize( m_MegaTexAtlasFile_MaskDim * m_MegaTexAtlasFile_MaskDim );

	D3DSURFACE_DESC tempMaskDesc;
	D3D_V( TempMask->GetLevelDesc( 0, &tempMaskDesc ) );

	r3d_assert( lrect.Pitch * r3dGetTextureBlockHeight( m_MegaTexAtlasFile_MaskDim, D3DFORMAT( tempMaskDesc.Format ) ) == oData->Count() * sizeof( UINT16 ) );
	memcpy( &(*oData)[ 0 ], lrect.pBits, oData->Count() * sizeof( UINT16 ) );

	D3D_V( destSurf->UnlockRect() );

	SAFE_RELEASE( srcSurf );
	SAFE_RELEASE( destSurf );
}

//------------------------------------------------------------------------

int r3dTerrain3::GetMaskTileSizeInFile()
{
	return r3dGetTextureBlockLineSize( m_MegaTexAtlasFile_MaskDim, D3DFORMAT( m_MegaTexAtlasFile_MaskFormat ) ) * r3dGetTextureBlockHeight( m_MegaTexAtlasFile_MaskDim, D3DFORMAT( m_MegaTexAtlasFile_MaskFormat ) );
}

//------------------------------------------------------------------------

int r3dTerrain3::GetHeightTileSizeInFile()
{
	return r3dGetTextureBlockLineSize( m_MegaTexAtlasFile_HeightDim, D3DFORMAT( m_MegaTexAtlasFile_HeightFormat ) ) * r3dGetTextureBlockHeight( m_MegaTexAtlasFile_HeightDim, D3DFORMAT( m_MegaTexAtlasFile_HeightFormat ) );
}

//------------------------------------------------------------------------

int r3dTerrain3::GetNormalTileSizeInFile()
{
	return r3dGetTextureBlockLineSize( m_MegaTexAtlasFile_NormalDim, D3DFORMAT( m_MegaTexAtlasFile_NormalFormat ) ) * r3dGetTextureBlockHeight( m_MegaTexAtlasFile_NormalDim, D3DFORMAT( m_MegaTexAtlasFile_NormalFormat ) );
}

//------------------------------------------------------------------------

int r3dTerrain3::GetHeightNormalRecordSizeInFile()
{
	return 2 + GetHeightTileSizeInFile() + GetNormalTileSizeInFile();
}

//------------------------------------------------------------------------

struct Terrain3UpdateMaskParams
{
	const r3dTL::TArray< UINT16 > * data;
	IDirect3DTexture9* tempPackedTex;
	IDirect3DTexture9* tempUnpackedTex; 
	int X;
	int Z; 
	int L; 
	int MaskId;

	r3dTerrain3* caller;
};

static void UpdateMaskHelper( void* param )
{
	Terrain3UpdateMaskParams* tump = static_cast<Terrain3UpdateMaskParams*>( param );

	tump->caller->UpdateMaskInMainThread(	tump->data, tump->tempPackedTex, tump->tempUnpackedTex,
											tump->X, tump->Z, tump->L, tump->MaskId );
}

void r3dTerrain3::UpdateMask( const r3dTL::TArray< UINT16 > * data, IDirect3DTexture9* tempPackedTex, IDirect3DTexture9* tempUnpackedTex, int X, int Z, int L, int MaskId )
{
	if( R3D_IS_MAIN_THREAD() )
	{
		UpdateMaskInMainThread( data, tempPackedTex, tempUnpackedTex, X, Z, L, MaskId );
	}
	else
	{
		Terrain3UpdateMaskParams params;

		params.data = data;
		params.tempPackedTex = tempPackedTex;
		params.tempUnpackedTex = tempUnpackedTex; 
		params.X = X;
		params.Z = Z; 
		params.L = L; 
		params.MaskId = MaskId;

		params.caller = this;

		ProcessCustomDeviceQueueItem( UpdateMaskHelper, &params );
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::UpdateMaskInMainThread( const r3dTL::TArray< UINT16 > * data, IDirect3DTexture9* tempPackedTex, IDirect3DTexture9* tempUnpackedTex, int X, int Z, int L, int MaskId )
{
	m_WasEdited = 1;

#ifndef FINAL_BUILD
	r3dCSHolderWithDeviceQueue cs( m_MegaTexFileCritSec ); ( void )cs;
#endif

	D3DLOCKED_RECT lrect;
	D3DSURFACE_DESC tdesc;
	D3D_V( tempUnpackedTex->GetLevelDesc( 0, &tdesc ) );
	D3D_V( tempUnpackedTex->LockRect( 0, &lrect, NULL, 0 ) );

	r3d_assert( lrect.Pitch * tdesc.Height == data->Count() * sizeof( (*data)[ 0 ]) );

	memcpy( lrect.pBits, &(*data)[ 0 ], lrect.Pitch * tdesc.Height );

	D3D_V( tempUnpackedTex->UnlockRect( 0 ) );

	IDirect3DSurface9 * srcSurf( NULL ), * destSurf( NULL );

	D3D_V( tempUnpackedTex->GetSurfaceLevel( 0, &srcSurf ) );

	D3D_V( tempPackedTex->GetSurfaceLevel( 0, &destSurf ) );

	D3D_V( D3DXLoadSurfaceFromSurface( destSurf, NULL, NULL, srcSurf, NULL, NULL, D3DX_FILTER_NONE, 0 ) );

	SAFE_RELEASE( srcSurf );
	SAFE_RELEASE( destSurf );

	INT64 id = MegaTexTile::GetID( X, Z, L, MaskId );

	D3D_V( tempPackedTex->GetLevelDesc( 0, &tdesc ) );

	D3D_V( tempPackedTex->LockRect( 0, &lrect, NULL, D3DLOCK_READONLY ) );

	FileTextureGridMap::iterator found = m_MegaTexGridFile_Map.find( id );

	if( found == m_MegaTexGridFile_Map.end() )
	{
		ErrorOnMissingMask( "r3dTerrain3::UpdateMask", id );
	}

	if( !found->second.IsInEditorFile )
	{
		found->second.EditorValue = m_MegaTexAtlasFile_EDITOR_Size;
		found->second.IsInEditorFile = 1;
	}

	int texSize = r3dGetTextureBlockLineSize( tdesc.Width, tdesc.Format ) * r3dGetTextureBlockHeight( tdesc.Height, tdesc.Format );

	int sizeToMap = texSize + sizeof( short );
	r3dMappedViewOfFile mvof = r3dCreateMappedViewOfFile( m_MegaTexAtlasFile_EDITOR_Mapping, FILE_MAP_WRITE, found->second.EditorValue, sizeToMap );
	void* ptr = mvof.Start;
	void* end = (char*)ptr + sizeToMap;

	*((short*&)ptr) ++ = (short)MaskId;
	memcpy( ptr, lrect.pBits, texSize );
	(char*&)ptr += texSize;

	r3d_assert( ptr == end );

	m_MegaTexAtlasFile_EDITOR_Size += sizeToMap;

	int res = 0;
	res = UnmapViewOfFile( mvof.Mapped ); r3d_assert( res );

	D3D_V( tempPackedTex->UnlockRect( 0 ) );
}

#ifndef FINAL_BUILD

//------------------------------------------------------------------------

r3dTerrain3::MegaTexFileGridOffset r3dTerrain3::GetEditorHeightNormalOffset( int X, int Z, int L, int * oIsNew )
{
	INT64 id = MegaTexTile::GetID( X, Z, L, 0 );

	FileTextureGridMap::iterator found = m_EDITOR_HeightNormalTileMap.find( id );

	*oIsNew = 0;

	if( found == m_EDITOR_HeightNormalTileMap.end() )
	{
		MegaTexFileGridOffset offset;

		MegaTexTile fakeTile;
		fakeTile.X = X;
		fakeTile.Z = Z;
		fakeTile.L = L;

		offset = GetMegaTexHeightNormalOffsetInFile( &fakeTile );

		offset.IsInEditorFile = 1;
		offset.EditorValue = m_MegaTexAtlasFile_EDITOR_Size;

		*oIsNew = 1;

		m_MegaTexAtlasFile_EDITOR_Size += GetHeightNormalRecordSizeInFile();

		found = m_EDITOR_HeightNormalTileMap.insert( FileTextureGridMap::value_type( id, offset ) ).first;
	}

	if( !found->second.IsInEditorFile )
	{
		found->second.EditorValue = m_MegaTexAtlasFile_EDITOR_Size;
		found->second.IsInEditorFile = 1;
	}

	return found->second;
}

//------------------------------------------------------------------------

#endif

void r3dTerrain3::WriteHeightToMipFile( FILE* file, const r3dTL::TArray< UINT16 > * data, int X, int Z, int L )
{
#ifndef FINAL_BUILD
	r3dCSHolderWithDeviceQueue cs( m_MegaTexFileCritSec ); ( void )cs;
#endif

	MegaTexTile mtt;

	mtt.X = X;
	mtt.Z = Z;
	mtt.L = L;

	MegaTexFileGridOffset offset = GetMegaTexHeightNormalOffsetInFile( &mtt );

	fseek( file, offset.Value, SEEK_SET );

	fwrite( MEGA_HN_SIG, sizeof MEGA_HN_SIG, 1, file );
	fwrite( &(*data)[ 0 ], data->Count() * sizeof(UINT16), 1, file );

}

//------------------------------------------------------------------------

struct Terrain3WriteNormalsToMipFileParams
{
	FILE* file;
	const r3dTL::TArray< UINT16 > * data;
	int X;
	int Z;
	int L;
	IDirect3DTexture9* unpackedTempNormals;
	IDirect3DTexture9* packedTempNormals;
	r3dTL::TArray< UINT16 > * tempData;

	r3dTerrain3* caller;
};

static void WriteNormalsToMipFileHelper( void* params )
{
	Terrain3WriteNormalsToMipFileParams* twntmfp = static_cast< Terrain3WriteNormalsToMipFileParams* >( params );

	twntmfp->caller->WriteNormalsToMipFileInMainThread( twntmfp->file, twntmfp->data, twntmfp->X,
														twntmfp->Z, twntmfp->L, twntmfp->unpackedTempNormals,
														twntmfp->packedTempNormals, twntmfp->tempData );
}

void r3dTerrain3::WriteNormalsToMipFile( FILE* file, const r3dTL::TArray< UINT16 > * data, int X, int Z, int L, IDirect3DTexture9* unpackedTempNormals, IDirect3DTexture9* packedTempNormals, r3dTL::TArray< UINT16 > * tempData )
{
	if( R3D_IS_MAIN_THREAD() )
	{
		WriteNormalsToMipFileInMainThread( file, data, X, Z, L, unpackedTempNormals, packedTempNormals, tempData );
	}
	else
	{
		Terrain3WriteNormalsToMipFileParams params;

		params.file					= file;
		params.data					= data;
		params.X					= X;
		params.Z					= Z;
		params.L					= L;
		params.unpackedTempNormals	= unpackedTempNormals;
		params.packedTempNormals	= packedTempNormals;
		params.tempData				= tempData;

		params.caller				= this;

		ProcessCustomDeviceQueueItem( WriteNormalsToMipFileHelper, &params );
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::WriteNormalsToMipFileInMainThread( FILE* file, const r3dTL::TArray< UINT16 > * data, int X, int Z, int L, IDirect3DTexture9* unpackedTempNormals, IDirect3DTexture9* packedTempNormals, r3dTL::TArray< UINT16 > * tempData )
{
#ifndef FINAL_BUILD
	r3dCSHolderWithDeviceQueue cs( m_MegaTexFileCritSec ); ( void )cs;
#endif

	int texSize = data->Count() * sizeof( (*data)[ 0 ] );

	D3DLOCKED_RECT lrect;

	D3DSURFACE_DESC sdesc;

	D3D_V( unpackedTempNormals->LockRect( 0, &lrect,  NULL, 0 ) );
	D3D_V( unpackedTempNormals->GetLevelDesc( 0, &sdesc ) );

	r3d_assert( lrect.Pitch * sdesc.Height == data->Count() * sizeof (*data)[ 0 ] );

	memcpy( lrect.pBits, &(*data)[ 0 ], lrect.Pitch * sdesc.Height );

	D3D_V( unpackedTempNormals->UnlockRect( 0 ) );

	IDirect3DSurface9* src, *dest;
	D3D_V( unpackedTempNormals->GetSurfaceLevel( 0, &src ) );

	D3D_V( packedTempNormals->GetSurfaceLevel( 0, &dest ) );

	D3D_V( D3DXLoadSurfaceFromSurface( dest, NULL, NULL, src, NULL, NULL, D3DX_FILTER_NONE, 0 ) );

	SAFE_RELEASE( src );
	SAFE_RELEASE( dest );

	D3D_V( packedTempNormals->LockRect( 0, &lrect, NULL, D3DLOCK_READONLY ) );

	D3D_V( packedTempNormals->GetLevelDesc( 0, &sdesc ) );

	int packTexSize = r3dGetPixelSize( sdesc.Width * sdesc.Height, sdesc.Format );

	r3d_assert( packTexSize == GetNormalTileSizeInFile() );

	MegaTexTile mtt;

	mtt.X = X;
	mtt.Z = Z;
	mtt.L = L;

	MegaTexFileGridOffset offset = GetMegaTexHeightNormalOffsetInFile( &mtt );

	fseek( file, long( offset.Value + sizeof MEGA_HN_SIG + data->Count() * sizeof( UINT16 ) ), SEEK_SET );

	fwrite( lrect.pBits, GetNormalTileSizeInFile(), 1, file );

	D3D_V( packedTempNormals->UnlockRect( 0 ) );
}

#ifndef FINAL_BUILD

//------------------------------------------------------------------------

void r3dTerrain3::UpdateHeight( const r3dTL::TArray< UINT16 > * data, Bytes* tempNormalData, int X, int Z, int L )
{
	r3dCSHolderWithDeviceQueue cs( m_MegaTexFileCritSec ); ( void )cs;

	m_WasEdited = 1;

	int isNew = 0;

	MegaTexFileGridOffset offset = GetEditorHeightNormalOffset( X, Z, L, &isNew );

	int texSize = data->Count() * sizeof( (*data)[ 0 ] );

	int sizeToMap = GetHeightNormalRecordSizeInFile();
	r3dMappedViewOfFile mvof = r3dCreateMappedViewOfFile( m_MegaTexAtlasFile_EDITOR_Mapping, FILE_MAP_WRITE, offset.EditorValue, sizeToMap );

	if( isNew )
	{
		tempNormalData->Resize( GetNormalTileSizeInFile() / sizeof( *tempNormalData )[ 0 ] );

		FILE* fin = NULL;

		if( offset.GridId == GRID_ID_MIPSFILE )
		{
			fin = m_MegaTexAtlasMipsFile;
		}
		else
		{
			R3D_FILE_SYSTEM_LOCK_SCOPE();
			int gridX, gridZ;
			GetGridCoordinates( offset, &gridX, &gridZ );
			fin = m_FileGrid[ gridZ ][ gridX ];
		}

		fseek( fin, long(offset.Value + 2 + GetHeightTileSizeInFile()), SEEK_SET );
		fread( &(*tempNormalData)[ 0 ], GetNormalTileSizeInFile(), 1, fin );

		WriteHeightNormalTileInFile( &mvof, &(*data)[ 0 ], &(*tempNormalData)[ 0 ] );
	}
	else
	{
		WriteHeightNormalTileInFile( &mvof, &(*data)[ 0 ], NULL );
	}

	int res = 0;
	res = UnmapViewOfFile( mvof.Mapped ); r3d_assert( res );
}

//------------------------------------------------------------------------

void r3dTerrain3::UpdateTileNormalEdges( int X, int Z, int L, r3dTL::TArray< UINT16 > * tempData0, r3dTL::TArray< UINT16 > * tempData1, r3dTL::TArray< UINT16 > * tempData2 )
{
	m_WasEdited = 1;

	MegaTexTile fakeTile;

	fakeTile.X = X;
	fakeTile.Z = Z;

	fakeTile.L = L;

	MegaTexFileGridOffset offset = GetMegaTexHeightNormalOffsetInFile( &fakeTile );

	tempData0->Resize( GetNormalTileSizeInFile() / sizeof( (*tempData0)[ 0 ] ) );
	tempData2->Clear();

#ifndef FINAL_BUILD
	if( offset.IsInEditorFile )
	{
		int mapSize = GetHeightNormalRecordSizeInFile();
		r3dMappedViewOfFile mappedViewOfFile = r3dCreateMappedViewOfFile( m_MegaTexAtlasFile_EDITOR_Mapping, FILE_MAP_READ, offset.EditorValue, mapSize );
		DoLoadMegaTexTileHeightAndNormalFromFile( &mappedViewOfFile, 0, &fakeTile, &(*tempData0)[ 0 ], NULL, LOADTILE_NORMAL );
		UnmapViewOfFile( mappedViewOfFile.Mapped );
	}
	else
#endif
	{
		tempData2->Resize( GetHeightTileSizeInFile() / sizeof( (*tempData2)[ 0 ] ) );

		if( offset.GridId == GRID_ID_MIPSFILE )
		{
			DoLoadMegaTexTileHeightAndNormalFromFile( m_MegaTexAtlasMipsFile, offset.Value, &fakeTile, &(*tempData0)[ 0 ], &(*tempData2)[ 0 ], LOADTILE_NORMAL | LOADTILE_HEIGHT );
		}
		else
		{
			R3D_FILE_SYSTEM_LOCK_SCOPE();

			int gridZ, gridX;
			GetGridCoordinates( offset, &gridZ, &gridX );
			DoLoadMegaTexTileHeightAndNormalFromFile( m_FileGrid[ gridZ ][ gridX ], offset.Value, &fakeTile, &(*tempData0)[ 0 ], &(*tempData2)[ 0 ], LOADTILE_NORMAL | LOADTILE_HEIGHT );
		}
	}

	UpdateNormalEdges( X, Z, L, &(*tempData0)[ 0 ], tempData0->Count(), tempData1 );

	UpdateHeightNormalsPacked( tempData2->Count() ? tempData2 : NULL, tempData0, X, Z, L );
}

//------------------------------------------------------------------------

void r3dTerrain3::UpdateNormalEdges( int X, int Z, int L, UINT16* targ16, int targCount, r3dTL::TArray< UINT16 > * tempData )
{
	m_WasEdited = 1;

	const QualitySettings& qs = GetCurrentQualitySettings();
	const Info& info = GetInfo();

	int tileCountX = info.MegaTileCountX;
	int tileCountZ = info.MegaTileCountZ;

	tileCountX >>= L;
	tileCountZ >>= L;

	tempData->Resize( GetNormalTileSizeInFile() / sizeof (*tempData) [ 0 ] );

	struct MemCpy
	{
		void operator () ( int offset, const UINT16* src, int size )
		{
			r3d_assert( offset + size <= totalSize );

			memcpy( targ + offset, src, size * sizeof( UINT16 ) );

#if 0
			memset( targ + offset, 0, size * sizeof( UINT16 ) );

			if( count % 3 == 1 )
			{
				targ[offset] = 0xffff;
			}

			if( count % 3 == 2 )
			{
				targ[offset] = 0x001f;
			}

			count ++;
#endif
		}

		int count;

		UINT16* targ;
		int totalSize;
	} targCpy;

	targCpy.count = 0;

	targCpy.targ = targ16;
	targCpy.totalSize = targCount;

	enum
	{
		BLOCK_SIZE = 4
	};

	COMPILE_ASSERT( TERRA3_MEGANORMAL_FORMAT == D3DFMT_DXT1 );
	COMPILE_ASSERT( NORMAL_TILE_BORDER == BLOCK_SIZE );

	int blockSize = r3dGetTextureBlockLineSize( BLOCK_SIZE, TERRA3_MEGANORMAL_FORMAT ) / sizeof (*tempData)[ 0 ];

	int blockCount = qs.HeightNormalAtlasTileDim / BLOCK_SIZE;

	if( X > 0 )
	{
		ReadNormalsPacked( tempData, X - 1, Z, L );

		for( int z = 1; z < blockCount - 1; z ++ )
		{
			targCpy( z * blockCount * blockSize, &(*tempData)[ z * blockCount * blockSize + ( blockCount - 2 ) * blockSize], blockSize );
		}

		if( Z > 0 )
		{
			ReadNormalsPacked( tempData, X - 1, Z - 1, L );

			targCpy( 0, &(*tempData)[ ( blockCount - 2 ) * blockCount * blockSize + ( blockCount - 2 ) * blockSize], blockSize );

			ReadNormalsPacked( tempData, X, Z - 1, L );

			for( int x = 1; x < blockCount - 1; x ++ )
			{
				targCpy( x * blockSize, &(*tempData)[ ( blockCount - 2 ) * blockCount * blockSize + x * blockSize ],	blockSize );
			}
		}

		if( Z < tileCountZ - 1 )
		{
			ReadNormalsPacked( tempData, X - 1, Z + 1, L );

			targCpy( ( blockCount - 1 ) * blockCount * blockSize, &(*tempData)[ 1 * blockCount * blockSize + ( blockCount - 2 ) * blockSize], blockSize );

			ReadNormalsPacked( tempData, X, Z + 1, L );

			for( int x = 1; x < blockCount - 1; x ++ )
			{
				targCpy( x * blockSize + ( blockCount - 1 ) * blockCount * blockSize, &(*tempData)[ 1 * blockCount * blockSize + x * blockSize ], blockSize );
			}
		}

		if( X < tileCountX - 1 )
		{
			ReadNormalsPacked( tempData, X + 1, Z, L );

			for( int z = 1; z < blockCount - 1; z ++ )
			{
				targCpy( z * blockCount * blockSize + ( blockCount - 1 ) * blockSize, &(*tempData)[ z * blockCount * blockSize + 1 * blockSize ], blockSize );
			}

			if( Z > 0 )
			{
				ReadNormalsPacked( tempData, X + 1, Z - 1, L );

				targCpy( 0 * blockCount * blockSize + ( blockCount - 1 ) * blockSize, &(*tempData)[ ( blockCount - 2 ) * blockCount * blockSize + 1 * blockSize ], blockSize );
			}

			if( Z < tileCountZ - 1 )
			{
				ReadNormalsPacked( tempData, X + 1, Z + 1, L );

				targCpy( ( blockCount - 1 ) * blockCount * blockSize + ( blockCount - 1 ) * blockSize, &(*tempData)[ 1 * blockCount * blockSize + 1 * blockSize ], blockSize );				
			}
		}
	}

}

//------------------------------------------------------------------------

void r3dTerrain3::UpdateNormals( const r3dTL::TArray< UINT16 > * data, int X, int Z, int L, IDirect3DTexture9* unpackedTempNormals, IDirect3DTexture9* packedTempNormals, r3dTL::TArray< UINT16 > * tempData )
{
	m_WasEdited = 1;

	r3dCSHolderWithDeviceQueue cs( m_MegaTexFileCritSec ); ( void )cs;

	int isNew = 0;

	MegaTexFileGridOffset offset = GetEditorHeightNormalOffset( X, Z, L, &isNew );

	r3d_assert( !isNew );

	int texSize = data->Count() * sizeof( (*data)[ 0 ] );

	int sizeToMap = GetHeightNormalRecordSizeInFile();
	r3dMappedViewOfFile mvof = r3dCreateMappedViewOfFile( m_MegaTexAtlasFile_EDITOR_Mapping, FILE_MAP_WRITE, (int)offset.EditorValue, sizeToMap );

	D3DLOCKED_RECT lrect;

	D3DSURFACE_DESC sdesc;

	D3D_V( unpackedTempNormals->LockRect( 0, &lrect,  NULL, 0 ) );
	D3D_V( unpackedTempNormals->GetLevelDesc( 0, &sdesc ) );

	r3d_assert( lrect.Pitch * sdesc.Height == data->Count() * sizeof (*data)[ 0 ] );

	memcpy( lrect.pBits, &(*data)[ 0 ], lrect.Pitch * sdesc.Height );

	D3D_V( unpackedTempNormals->UnlockRect( 0 ) );

	IDirect3DSurface9* src, *dest;
	D3D_V( unpackedTempNormals->GetSurfaceLevel( 0, &src ) );

	D3D_V( packedTempNormals->GetSurfaceLevel( 0, &dest ) );

	D3D_V( D3DXLoadSurfaceFromSurface( dest, NULL, NULL, src, NULL, NULL, D3DX_FILTER_NONE, 0 ) );

	SAFE_RELEASE( src );
	SAFE_RELEASE( dest );

	D3D_V( packedTempNormals->LockRect( 0, &lrect, NULL, D3DLOCK_READONLY ) );

	D3D_V( packedTempNormals->GetLevelDesc( 0, &sdesc ) );

	int packTexSize = r3dGetPixelSize( sdesc.Width * sdesc.Height, sdesc.Format );

	r3d_assert( packTexSize == GetNormalTileSizeInFile() );

	const r3dTerrain3::QualitySettings& qs = GetCurrentQualitySettings();

	UpdateNormalEdges( X, Z, L, (UINT16*)lrect.pBits, packTexSize / sizeof(UINT16), tempData );

	WriteHeightNormalTileInFile( &mvof, NULL, lrect.pBits );

	D3D_V( packedTempNormals->UnlockRect( 0 ) );

	int res = 0;
	res = UnmapViewOfFile( mvof.Mapped ); r3d_assert( res );
}

//------------------------------------------------------------------------

void
r3dTerrain3::UpdateHeightNormalsPacked( const r3dTL::TArray< UINT16 > * heightData, const r3dTL::TArray< UINT16 > * normalData,int X, int Z, int L )
{
	m_WasEdited = 1;

	r3dCSHolderWithDeviceQueue cs( m_MegaTexFileCritSec ); ( void )cs;

	int isNew = 0;

	MegaTexFileGridOffset offset = GetEditorHeightNormalOffset( X, Z, L, &isNew );

	int sizeToMap = GetHeightNormalRecordSizeInFile();
	r3dMappedViewOfFile mvof = r3dCreateMappedViewOfFile( m_MegaTexAtlasFile_EDITOR_Mapping, FILE_MAP_WRITE, (int)offset.EditorValue, sizeToMap );

	r3d_assert( !normalData || normalData->Count() * sizeof ( *normalData )[ 0 ] == GetNormalTileSizeInFile() );
	r3d_assert( !heightData || heightData->Count() * sizeof ( *heightData )[ 0 ] == GetHeightTileSizeInFile() );

	WriteHeightNormalTileInFile( &mvof, heightData ? &( *heightData )[ 0 ] : NULL, normalData ? &( *normalData )[ 0 ] : NULL );

	int res = 0;
	res = UnmapViewOfFile( mvof.Mapped ); r3d_assert( res );
}

//------------------------------------------------------------------------

void
r3dTerrain3::ReadNormalsPacked( r3dTL::TArray< UINT16 > * oData, int X, int Z, int L )
{
	MegaTexTile fakeTile;

	fakeTile.X = X;
	fakeTile.Z = Z;

	fakeTile.L = L;

	oData->Resize( GetNormalTileSizeInFile() / sizeof( (*oData)[ 0 ] ) );

	MegaTexFileGridOffset offset = GetMegaTexHeightNormalOffsetInFile( &fakeTile );

#ifndef FINAL_BUILD
	if( offset.IsInEditorFile )
	{
		int mapSize = GetHeightNormalRecordSizeInFile();
		r3dMappedViewOfFile mappedViewOfFile = r3dCreateMappedViewOfFile( m_MegaTexAtlasFile_EDITOR_Mapping, FILE_MAP_READ, offset.EditorValue, mapSize );
		DoLoadMegaTexTileHeightAndNormalFromFile( &mappedViewOfFile, 0, &fakeTile, &(*oData)[ 0 ], NULL, LOADTILE_NORMAL );
		UnmapViewOfFile( mappedViewOfFile.Mapped );
	}
	else
#endif
	{
		if( offset.GridId == GRID_ID_MIPSFILE )
		{
			DoLoadMegaTexTileHeightAndNormalFromFile( m_MegaTexAtlasMipsFile, offset.Value, &fakeTile, &(*oData)[ 0 ], NULL, LOADTILE_NORMAL );
		}
		else
		{
			R3D_FILE_SYSTEM_LOCK_SCOPE();

			int gridX, gridZ;
			GetGridCoordinates( offset, &gridX, &gridZ );
			DoLoadMegaTexTileHeightAndNormalFromFile( m_FileGrid[ gridZ ][ gridX ], offset.Value, &fakeTile, &(*oData)[ 0 ], NULL, LOADTILE_NORMAL );
		}
	}
}

#endif

//------------------------------------------------------------------------

void
r3dTerrain3::RecalcNormalMap( const UShorts* ( &heights )[ 9 ], Floats* tempHeights, Vectors* tempVectors0, Vectors* tempVectors1, UShorts* oTempNormals, int L )
{
	m_WasEdited = 1;

#if R3D_TERRAIN_V3_GRAPHICS
	const QualitySettings& curQS = GetCurrentQualitySettings();
	const r3dTerrainDesc& tdesc = GetDesc();

	int xTotal = curQS.HeightNormalAtlasTileDim;
	int zTotal = curQS.HeightNormalAtlasTileDim;

	r3d_assert( xTotal * zTotal == heights[ 4 ]->Count() );

	oTempNormals->Resize( heights[ 4 ]->Count() );
	tempHeights->Resize( heights[ 4 ]->Count() );

	const float mulConv = ( tdesc.MaxHeight - tdesc.MinHeight ) / 65535.0f;

	int halfWidth = xTotal / 2;
	int halfHeight = zTotal / 2;

	float padding = GetNormalPaddingCoef();

	for( int z = 0, ze = zTotal; z < ze; z ++ )
	{
		int lz = int( ( z - halfHeight ) * padding + halfHeight );

		for( int x = 0, xe = xTotal; x < xe; x ++ )
		{
			int lx = int( ( x - halfWidth ) * padding + halfWidth );

			if( lx < 0 )
			{
				if( lz < 0 )
				{
					if( heights[ 0 ] )
						(*tempHeights)[ z * zTotal + x ] = (*heights[ 0 ])[ ( zTotal + lz ) * xTotal + xTotal + lx ];
					else
						(*tempHeights)[ z * zTotal + x ] = (*heights[ 4 ])[ 0 ];
				}
				else
				{
					if( lz < zTotal )
					{
						if( heights[ 3 ] )
							(*tempHeights)[ z * zTotal + x ] = (*heights[ 3 ])[ lz * xTotal + xTotal + lx ];
						else
							(*tempHeights)[ z * zTotal + x ] = (*heights[ 4 ])[ lz * xTotal + 0 ];
					}
					else
					{
						if( heights[ 6 ] )
							(*tempHeights)[ z * zTotal + x ] = (*heights[ 6 ])[ ( lz - zTotal ) * xTotal + xTotal + lx ];
						else
							(*tempHeights)[ z * zTotal + x ] = (*heights[ 4 ])[ ( zTotal - 1 ) * xTotal + 0 ];
					}
				}
			}
			else
			{
				if( lx < xTotal )
				{
					if( lz < 0 )
					{
						if( heights[ 1 ] )
							(*tempHeights)[ z * zTotal + x ] = (*heights[ 1 ])[ ( zTotal + lz ) * xTotal + lx ];
						else
							(*tempHeights)[ z * zTotal + x ] = (*heights[ 4 ])[ 0 * xTotal + lx ];
					}
					else
					{
						if( lz < zTotal )
						{
							(*tempHeights)[ z * zTotal + x ] = (*heights[ 4 ])[ lz * xTotal + lx ];
						}
						else
						{
							if( heights[ 7 ] )
								(*tempHeights)[ z * zTotal + x ] = (*heights[ 7 ])[ ( lz - zTotal ) * xTotal + lx ];
							else
								(*tempHeights)[ z * zTotal + x ] = (*heights[ 4 ])[ ( zTotal - 1 ) * xTotal + lx ];
						}
					}
				}
				else
				{
					if( lz < 0 )
					{
						if( heights[ 2 ] )
							(*tempHeights)[ z * zTotal + x ] = (*heights[ 2 ])[ ( zTotal + lz ) * xTotal + lx - xTotal ];
						else
							(*tempHeights)[ z * zTotal + x ] = (*heights[ 4 ])[ 0 * xTotal + xTotal -  1 ];
					}
					else
					{
						if( lz < zTotal )
						{
							if( heights[ 5 ] )
								(*tempHeights)[ z * zTotal + x ] = (*heights[ 5 ])[ lz * xTotal + lx - xTotal ];
							else
								(*tempHeights)[ z * zTotal + x ] = (*heights[ 4 ])[ lz * xTotal + xTotal - 1 ];
						}
						else
						{
							if( heights[ 8 ] )
								(*tempHeights)[ z * zTotal + x ] = (*heights[ 8 ])[ ( lz - zTotal ) * xTotal + lx - xTotal ];
							else
								(*tempHeights)[ z * zTotal + x ] = (*heights[ 4 ])[ ( zTotal - 1 ) * xTotal + xTotal - 1 ];
						}
					}
				}
			}
		}
	}

	for( int i = 0, e = tempHeights->Count(); i < e; i ++ )
	{
		(*tempHeights)[ i ] = (*tempHeights)[ i ] * mulConv;
	}

	struct
	{
		float operator() ( int x, int z )
		{
			return (*heights)[ x + z * xTot ];
		}

		int xTot;

		const Floats * heights;

	} getHeight = { xTotal, tempHeights };

	Vectors& NormalMapData = *tempVectors0;
	Vectors& NormalBuf = *tempVectors1;

	NormalMapData.Resize( xTotal * zTotal );
	NormalBuf.Resize( NormalMapData.Count() );

	r3d_assert( heights[ 4 ]->Count() == NormalMapData.Count() );
	r3d_assert( heights[ 4 ]->Count() == NormalBuf.Count() );

	int bx0 = 0;
	int bx1 = xTotal;
	int bz0 = 0;
	int bz1 = zTotal;

	float cellSize = m_CellSize * ( 1 << L );

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

			r3dPoint3D vN(-sx, cellSize, -sy);
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

	struct R5G6B5
	{
		UINT16 b : 5;
		UINT16 g : 6;
		UINT16 r : 5;
	} * lockee = static_cast<R5G6B5*> ( (void*)&(*oTempNormals)[ 0 ] );

	for( int z = 0, ze = zTotal; z < ze; z ++ )
	{
		for( int x = 0, xe = xTotal; x < xe; x ++ )
		{
			const r3dPoint3D& n = NormalMapData[ x + z * xTotal ];			

			for( int nz = z * m_NormalDensity, e = nz + m_NormalDensity; nz < e; nz ++ )
			{
				for( int nx = x * m_NormalDensity, e = nx + m_NormalDensity; nx < e; nx ++ )
				{
					int idx = nx + nz * xTotal;

					R5G6B5& sample = lockee[ idx ];

					r3dPoint3D fn = n;

					sample.r = R3D_MIN( R3D_MAX( int( ( fn.x * 0.5f + 0.5f ) * 31 ), 0 ), 31 );
					sample.g = R3D_MIN( R3D_MAX( int( ( fn.y * 0.5f + 0.5f ) * 63 ), 0 ), 63 );
					sample.b = R3D_MIN( R3D_MAX( int( ( fn.z * 0.5f + 0.5f ) * 31 ), 0 ), 31 );
				}
			}
		}
	}	
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::ReloadMasks( int X, int Z, int L )
{
	MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ L ];

	for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
	{
		MegaTexTile* mtt = arr[ i ];

		if( mtt->X == X && mtt->Z == Z && mtt->L == L )
		{
			mtt->IsLoaded = 0;
			AddMegaTexTileLoadJobs( mtt, LOADTILE_ALL );
		}
	}
}

//------------------------------------------------------------------------

r3dTerrain3::Info r3dTerrain3::GetInfo() const
{
	Info info;

	info.NumActiveMegaTexLayers = m_NumActiveMegaTexQualityLayers;

	info.MaskFormat = TERRA3_LAYERMASK_FORMAT;

	info.MegaTileCountX = m_MaskResolutionU / m_QualitySettings.MaskAtlasTileDim;
	info.MegaTileCountZ = m_MaskResolutionV / m_QualitySettings.MaskAtlasTileDim;

	info.MaskAtlasTileCountPerSide = m_MaskAtlasTileCountPerSide;
	info.AtlasTileCountPerSide = m_AtlasTileCountPerSide;

	return info;
}

//------------------------------------------------------------------------

int r3dTerrain3::GetMaskCount() const
{
	return m_Layers.Count() / LAYERS_PER_MASK + ( m_Layers.Count() % LAYERS_PER_MASK ? 1 : 0 );
}

//------------------------------------------------------------------------

void r3dTerrain3::GetHeightTileCountsForLod( int L, int* oTileX, int* oTileZ ) const
{
	const QualitySettings& curQS = GetCurrentQualitySettings();

	int heightTileCountX = m_TileCountX / curQS.VertexTilesInMegaTexTileCount;
	int heightTileCountZ = m_TileCountZ / curQS.VertexTilesInMegaTexTileCount;

	*oTileX = heightTileCountX >> L;
	*oTileZ = heightTileCountZ >> L;
}

//------------------------------------------------------------------------

#ifndef FINAL_BUILD

void r3dTerrain3::AddReplacementTile( int X, int Z, int L, int maskIdx, IDirect3DTexture9* tex )
{
	R3DPROFILE_FUNCTION( "r3dTerrain3::AddReplacementTile" );

	const QualitySettings& curQS = GetCurrentQualitySettings();

	INT64 mapId = MegaTexTile::GetID( X, Z, L, maskIdx );

	EditorReplacementMaskTiles::iterator iter = m_EDITOR_ReplacementTileIndexes.find( mapId );

	if( iter == m_EDITOR_ReplacementTileIndexes.end() )
	{
		INT32 id( 0 );
		AllocateEditorMaskTexTile( &id, maskIdx );

		iter = m_EDITOR_ReplacementTileIndexes.insert( EditorReplacementMaskTiles::value_type( mapId, id ) ).first;
	}

	int volId( -1 ), tileId( -1 ), maskId( -1 );
	MegaTexTile::DecodeMaskEntry( iter->second, &volId, &tileId, &maskId );

	r3d_assert( maskIdx == maskId );

	IDirect3DSurface9* srcSurf( NULL ), *destSurf( NULL );

	D3D_V( tex->GetSurfaceLevel( 0, &srcSurf ) );
	D3D_V( m_EDITOR_ReplacementMaskAtlas[ volId ].Mask->AsTex2D()->GetSurfaceLevel( 0, &destSurf ) );

	int2 xz = GetMaskTileAtlasXZ( tileId );

	POINT targPt;

	targPt.x = xz.x * curQS.MaskAtlasTileDim;
	targPt.y = xz.y * curQS.MaskAtlasTileDim;

	D3D_V( r3dRenderer->pd3ddev->UpdateSurface( srcSurf, NULL, destSurf, &targPt ) );

	SAFE_RELEASE( srcSurf );
	SAFE_RELEASE( destSurf );
}

//------------------------------------------------------------------------

void r3dTerrain3::ClearEditorReplacementTileIndexes()
{
	m_EDITOR_ReplacementTileIndexes.clear();
}
#endif

void r3dTerrain3::MarkDirtyPhysChunk( int tileX, int tileZ )
{
	const QualitySettings& qs = GetCurrentQualitySettings();

	int cellsPerTile = qs.VertexTilesInMegaTexTileCount * qs.VertexTileDim;

	int ctx = tileX * cellsPerTile;
	int ctz = tileZ * cellsPerTile;

	for( int i = 0, e = m_PhysicsChunks.Count(); i < e; i ++ )
	{
		PhysicsChunk* chunk = &m_PhysicsChunks[ i ];
		if( chunk->StartX <= ctx && chunk->StartZ <= ctz
				&&
			chunk->StartX + qs.PhysicsTileCellCount >= ctx
				&&
			chunk->StartZ + qs.PhysicsTileCellCount >= ctz
			)
		{
			chunk->Dirty = 1;
		}
	}
}

//------------------------------------------------------------------------

#ifndef FINAL_BUILD

void r3dTerrain3::ToggleCameraFixed()
{
	m_EDITOR_IsCameraFixed = !m_EDITOR_IsCameraFixed;

	if( m_EDITOR_IsCameraFixed )
	{
		extern r3dCamera gCam;
		m_EDITOR_FixedCamera = gCam;
	}

	void ConPrint( const char* fmt, ... );

	if( m_EDITOR_IsCameraFixed )
	{
		ConPrint( "Camera is now fixed." );
	}
	else
	{
		ConPrint( "Camera is now free." );
	}

}

//------------------------------------------------------------------------

int r3dTerrain3::IsCameraFixed() const
{
	return m_EDITOR_IsCameraFixed;
}

#endif

struct SubTexInMainThreadParams
{
	r3dD3DTextureTunnel* tunnel;

	int subresSizeU;
	int subresSizeV;
};

static void CreateSubTexInMainThread( void * params )
{
	SubTexInMainThreadParams* sparms = static_cast< SubTexInMainThreadParams* >( params );

	IDirect3DTexture9* subResTex( NULL );

	D3D_V( r3dRenderer->pd3ddev->CreateTexture( sparms->subresSizeU, sparms->subresSizeV, 1, 0, TERRA3_MEGANORMAL_FORMAT, D3DPOOL_SYSTEMMEM, &subResTex, NULL ) );

	sparms->tunnel->Set( subResTex );
}

struct ScaleSubTexInMainThreadParams
{
	r3dD3DTextureTunnel* tunnel;
	r3dD3DTextureTunnel* result;
	int finalSizeX;
	int finalSizeZ;
};

static void ScaleSubTexInMainThread( void * param )
{
	ScaleSubTexInMainThreadParams* sparams = static_cast<ScaleSubTexInMainThreadParams*>( param );

	ID3DXBuffer* buff( NULL );

	D3D_V( D3DXSaveTextureToFileInMemory( &buff, D3DXIFF_DDS, sparams->tunnel->AsTex2D(), NULL ) );

	sparams->tunnel->ReleaseAndReset();

	IDirect3DTexture9* fullTex( NULL );

	D3D_V( D3DXCreateTextureFromFileInMemoryEx( r3dRenderer->pd3ddev, buff->GetBufferPointer(), buff->GetBufferSize(), sparams->finalSizeX, sparams->finalSizeZ, 1, 0, TERRA3_MEGANORMAL_FORMAT, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE, D3DX_FILTER_NONE, 0, NULL, NULL, &fullTex ) );

	sparams->result->Set( fullTex );

	SAFE_RELEASE( buff );
}

void r3dTerrain3::RecreateLowResNormals()
{
#if R3D_TERRAIN_V3_GRAPHICS
	if( m_LowResNormalTexture )
	{
		r3dRenderer->DeleteTexture( m_LowResNormalTexture );
		m_LowResNormalTexture = NULL;
	}


	int LOWRES_NORMAL_DIM_U = 512;
	int LOWRES_NORMAL_DIM_V = 512;

	// otherwise have to rewrite
	COMPILE_ASSERT( NORMAL_TILE_BORDER == 4 );

	const QualitySettings& qs = GetCurrentQualitySettings();
	const r3dTerrainDesc& tdesc = GetDesc();

	// need other implemention for non square terrains

	LOWRES_NORMAL_DIM_U = R3D_MIN( tdesc.MaskResolutionU, LOWRES_NORMAL_DIM_U );
	LOWRES_NORMAL_DIM_V = LOWRES_NORMAL_DIM_U * tdesc.MaskResolutionV / tdesc.MaskResolutionU;

	for( ;	LOWRES_NORMAL_DIM_V < qs.HeightNormalAtlasTileDim || 
			LOWRES_NORMAL_DIM_U < qs.HeightNormalAtlasTileDim ; )
	{
		LOWRES_NORMAL_DIM_V *= 2;
		LOWRES_NORMAL_DIM_U *= 2;
	}

	int maskCountU = LOWRES_NORMAL_DIM_U / qs.HeightNormalAtlasTileDim;
	int maskCountV = LOWRES_NORMAL_DIM_V / qs.HeightNormalAtlasTileDim;

	int subresSizeU = maskCountU * ( qs.HeightNormalAtlasTileDim - 2 * NORMAL_TILE_BORDER );
	int subresSizeV = maskCountV * ( qs.HeightNormalAtlasTileDim - 2 * NORMAL_TILE_BORDER );

	r3dD3DTextureTunnel subResTex;
	SubTexInMainThreadParams params;

	params.subresSizeU = subresSizeU;
	params.subresSizeV = subresSizeV;

	params.tunnel = &subResTex;

	ProcessCustomDeviceQueueItem( CreateSubTexInMainThread, &params );

	int mip = m_NumActiveMegaTexQualityLayers - 1;

	int MIN_DIM = R3D_MIN( LOWRES_NORMAL_DIM_U, LOWRES_NORMAL_DIM_V );

	for( ; qs.HeightNormalAtlasTileDim << ( m_NumActiveMegaTexQualityLayers - 1 - mip ) < MIN_DIM; mip -- )
	{

	}

	r3d_assert( mip >= 0 );

	D3DLOCKED_RECT lrect;
	subResTex.LockRect( 0, &lrect, NULL, 0 );

	char* locked = ( char* )lrect.pBits;

	int blockLineSize = r3dGetTextureBlockLineSize( subresSizeU, TERRA3_MEGANORMAL_FORMAT );
	int blockHeight = r3dGetTextureBlockHeight( subresSizeV, TERRA3_MEGANORMAL_FORMAT );

	int tileBlockLineSize = r3dGetTextureBlockLineSize( qs.HeightNormalAtlasTileDim, TERRA3_MEGANORMAL_FORMAT );
	int tileBlockHeight = r3dGetTextureBlockHeight( qs.HeightNormalAtlasTileDim, TERRA3_MEGANORMAL_FORMAT );

	int subResBlockLineSize = r3dGetTextureBlockLineSize( qs.HeightNormalAtlasTileDim - 2 * NORMAL_TILE_BORDER, TERRA3_MEGANORMAL_FORMAT );
	int subResBlockHeight = r3dGetTextureBlockHeight( qs.HeightNormalAtlasTileDim - 2 * NORMAL_TILE_BORDER, TERRA3_MEGANORMAL_FORMAT );

	for( int z = 0, e = maskCountV; z < e; z ++ )
	{
		for( int x = 0, e = maskCountU; x < e; x ++ )
		{
			MegaTexTile tile;

			tile.Init();

			tile.X = x;
			tile.Z = z;

			tile.Tagged = 1;
			tile.L = mip;

			tile.IsLoaded = 0;

			MegaTexFileGridOffset offset = GetMegaTexHeightNormalOffsetInFile( &tile );

			D3DLOCKED_RECT tempNormRect;

			m_MegaTexTile_TempNormal.LockRect( 0, &tempNormRect, NULL, 0 );

#ifndef FINAL_BUILD
			if( offset.IsInEditorFile )
			{
				int mapSize = GetHeightNormalRecordSizeInFile();
				r3dMappedViewOfFile mappedViewOfFile = r3dCreateMappedViewOfFile( m_MegaTexAtlasFile_EDITOR_Mapping, FILE_MAP_READ, offset.EditorValue, mapSize );
				DoLoadMegaTexTileHeightAndNormalFromFile( &mappedViewOfFile, 0, &tile, (UINT16*)tempNormRect.pBits, NULL, LOADTILE_NORMAL );
				UnmapViewOfFile( mappedViewOfFile.Mapped );
			}
			else
#endif
			{
				if( offset.GridId == GRID_ID_MIPSFILE )
				{
					DoLoadMegaTexTileHeightAndNormalFromFile( m_MegaTexAtlasMipsFile, offset.Value, &tile, (UINT16*)tempNormRect.pBits, NULL, LOADTILE_NORMAL );
				}
				else
				{
					R3D_FILE_SYSTEM_LOCK_SCOPE();

					int gridX( 0 ), gridZ( 0 );
					GetGridCoordinates( offset, &gridX, &gridZ );
					DoLoadMegaTexTileHeightAndNormalFromFile( m_FileGrid[ gridZ ][ gridX ], offset.Value, &tile, (UINT16*)tempNormRect.pBits, NULL, LOADTILE_NORMAL );
				}
			}

			m_MegaTexTile_TempNormal.UnlockRect( 0 );

			D3DLOCKED_RECT lrect;
			m_MegaTexTile_TempNormal.LockRect( 0, &lrect, NULL, D3DLOCK_READONLY );

			for( int i = 1, e = tileBlockHeight - 1; i < e; i ++ )
			{
				char * dest = locked	+ z * subResBlockLineSize * subResBlockHeight * maskCountU
					+ ( i - 1 ) * blockLineSize + x * subResBlockLineSize;

				r3d_assert( dest + subResBlockLineSize <= locked + blockLineSize * blockHeight );
				memcpy( dest,  (char*)lrect.pBits + tileBlockLineSize*i, subResBlockLineSize );
			}

			m_MegaTexTile_TempNormal.UnlockRect( 0 );
		}
	}

	r3dTexture* tex = r3dRenderer->AllocateTexture();

	subResTex.UnlockRect( 0 );

	r3dD3DTextureTunnel result;
	ScaleSubTexInMainThreadParams scaleParams;

	scaleParams.finalSizeX = LOWRES_NORMAL_DIM_U;
	scaleParams.finalSizeZ = LOWRES_NORMAL_DIM_V;

	scaleParams.tunnel	= &subResTex;
	scaleParams.result	= &result;

	ProcessCustomDeviceQueueItem( ScaleSubTexInMainThread, &scaleParams );

	tex->Setup( LOWRES_NORMAL_DIM_U, LOWRES_NORMAL_DIM_V, 1, TERRA3_MEGANORMAL_FORMAT, 1, &result, false );

	m_LowResNormalTexture = tex;
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::DEBUG_PrintMegaTileMap()
{
	void ConPrint( const char* fmt, ... );

	FILE* fout = fopen( "maskdump.txt", "wt" );

	for( FileTextureGridMap::iterator	i = m_MegaTexGridFile_Map.begin(),
										e = m_MegaTexGridFile_Map.end();
										i != e;
										++ i )
	{
		short x, z;
		char L;
		int maskId;

		MegaTexTile::DecodeID( i->first, &x, &z, &L, &maskId );

		ConPrint( "(%d,%d,%d,%d),", x, z, L, maskId );
		fprintf( fout, "(%d,%d,%d,%d)\n", x, z, L, maskId );
	}

	fclose( fout );
}

//------------------------------------------------------------------------

void r3dTerrain3::DEBUG_PrintStats()
{
	void ConPrint( const char* fmt, ... );

	int total = 0;

	for( int i = 0, e = m_AllocTileLodArray.Count(); i < e; i ++ )
	{
		AllocatedTileArr& arr = m_AllocTileLodArray[ i ];

		total += arr.Count();

		ConPrint( "Lod %d: %d\n", i, arr.Count() );
	}

	ConPrint( "Total: %d\n", total );
}

//------------------------------------------------------------------------

void r3dTerrain3::DEBUG_CheckTileInfoConsistency()
{
	for( FileTextureGridMap::iterator	i = m_MegaTexGridFile_Map.begin(),
										e = m_MegaTexGridFile_Map.end();
										i != e;
										++ i
										)
	{
		short tx, tz;
		char L;
		int maskId;

		MegaTexTile::DecodeID( i->first, &tx, &tz, &L, &maskId );

		MegaTexTileInfo& info = m_MegaTexTileInfoMipChain[ L ][ tz ][ tx ];
		
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::CreateDefaultPoolResources()
{
	for (size_t i = 0; i < m_HeightNormalAtlas.Count(); ++i)
	{
		HeightNormalAtlasVolume &v = m_HeightNormalAtlas[i];
		CreateHeightNormalTexture(v.Height, TERRA3_MEGAHEIGHT_FORMAT);
		CreateHeightNormalTexture(v.Normal, TERRA3_MEGANORMAL_FORMAT);
	}

	for (size_t i = 0; i < m_MaskAtlas.Count(); ++i)
	{
		MaskAtlasVolume &v = m_MaskAtlas[i];
		CreateMaskTexture(v.Mask);
	}

	for( int L = 0, e = (int)m_AllocMegaTexTileLodArray.Count(); L < e; L ++ )
	{
		MegaTexTilePtrArr& ptrArr = m_AllocMegaTexTileLodArray[ L ];

		for( int i = 0, e = (int)ptrArr.Count(); i < e; i ++ )
		{
			MegaTexTile* mega = ptrArr[ i ];

			if( mega->IsInAtlas() && mega->IsLoaded )
			{
				AddMegaTexTileUpdateJobs( mega,	LOADTILE_MASKS | LOADTILE_NORMAL | 
												LOADTILE_HEIGHT | LOADTILE_UPDATEATLAS );
			}
		}
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::DestroyDefaultPoolResources()
{
	for (size_t i = 0; i < m_HeightNormalAtlas.Count(); ++i)
	{
		HeightNormalAtlasVolume &v = m_HeightNormalAtlas[i];
		v.Normal->Destroy();
		v.Height->Destroy();
	}

	for (size_t i = 0; i < m_MaskAtlas.Count(); ++i)
	{
		MaskAtlasVolume &v = m_MaskAtlas[i];
		v.Mask->Destroy();
	}
}

//------------------------------------------------------------------------

int
r3dTerrain3::CheckMegaMap()
{
#ifndef FINAL_BUILD
	INT64 tileSizeInfFile = GetMaskTileSizeInFile() + sizeof( short );

	int count = 0;

	for( FileTextureGridMap::iterator	i = m_MegaTexGridFile_Map.begin(),
									e = m_MegaTexGridFile_Map.end();
									i != e;
									++ i )
	{
		INT64 val = i->second.Value;
		INT64 gridId = i->second.GridId;

		for( FileTextureGridMap::iterator	i = m_MegaTexGridFile_Map.begin(),
										e = m_MegaTexGridFile_Map.end();
										i != e;
										++ i )
		{
			if( i->second.GridId == gridId )
			{
				INT64 cmpVal = i->second.Value;
				if( val < cmpVal && val + tileSizeInfFile > cmpVal )
					count ++;
			}
		}
	}

	return count;
#else
	return 0;
#endif
}

//------------------------------------------------------------------------

int r3dTerrain3::CheckMegaOffsets()
{
#if !defined( FINAL_BUILD ) && R3D_TERRAIN_V3_GRAPHICS
	R3D_FILE_SYSTEM_LOCK_SCOPE();

	int n = 0;

	int outOfBoundsShown = 0;

	INT64 tileSizeInFile = GetMaskTileSizeInFile() + sizeof( short );

	for( FileTextureGridMap::iterator	i = m_MegaTexGridFile_Map.begin(),
										e = m_MegaTexGridFile_Map.end();
										i != e;
										++ i )
	{
		INT64 val = i->second.Value;

		short tx, tz;
		char L;
		int maskId;
		MegaTexTile::DecodeID( i->first, &tx, &tz, &L, &maskId );

		FILE* fin( NULL );

		int gridX = -1, gridZ = -1;

		if( i->second.GridId == GRID_ID_MIPSFILE )
		{
			fin = m_MegaTexAtlasMipsFile;
		}
		else
		{
			GetGridCoordinates( i->second, &gridX, &gridZ );
			fin = m_FileGrid[ gridZ ][ gridX ];
		}

		fseek( fin, (long)0, SEEK_END );

		int fileSize = ftell( fin );

		fseek( fin, (long)val, SEEK_SET );

		if( val + tileSizeInFile > fileSize && !outOfBoundsShown )
		{
			outOfBoundsShown = 1;

			char message[ 512 ];
			sprintf( message, "Mask tile is located beyond atlas file size!" );

			MessageBoxA( r3dRenderer->HLibWin, message, "Error", MB_ICONERROR );
		}

		short index;
		fread( &index, sizeof index, 1, fin );

		if( index != maskId )
		{
			n ++;
			r3dOutToLog( "Tile mask (%d,%d,%d,%d) has wrong offset! ( %d != %d )\n", tx, tz, L, maskId, maskId, index );
		}
	}

	return n;
#else
	return 0;
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::MarkMegaTilesRecalculated()
{
#ifndef FINAL_BUILD
	m_MegaTexMipsRecalculated = 1;
#endif
}

//------------------------------------------------------------------------

int r3dTerrain3::AreMegaTilesRecalculated()
{
#ifndef FINAL_BUILD
	return m_MegaTexMipsRecalculated;
#else
	return 1;
#endif
}

//------------------------------------------------------------------------

int r3dTerrain3::GetPhysChunkSize() const
{
#if R3D_TERRAIN3_ALLOW_FULL_PHYSCHUNKS
	if( m_FullPhysChunkModeOn )
	{
		const r3dTerrainDesc& desc = GetDesc();
		return R3D_MIN( (int)FULL_PHYS_CHUNK_SIZE, R3D_MIN( desc.MaskResolutionU, desc.MaskResolutionV ) );
	}
	else
	{
		return GAME_PHYS_CHUNK_SIZE;
	}
#else
	return GAME_PHYS_CHUNK_SIZE;
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::DEBUG_ResetTerraPhysics()
{
	for( int i = 0, e = (int)m_PhysicsChunks.Count(); i < e; i ++ )
	{
		PhysicsChunk& physChunk = m_PhysicsChunks[ i ];

		DoCreatePhysHeightField( physChunk, physChunk.ChunkX, physChunk.ChunkZ, m_QualitySettings.PhysicsTileCellCount );
	}
}

//------------------------------------------------------------------------

void
r3dTerrain3::ClearMegaLayerInfos()
{
	for( int i = 0, e = (int)m_MegaTexTileInfoMipChain.Count(); i < e; i ++ )
	{
		MegaTexTileInfoArr& arr = m_MegaTexTileInfoMipChain[ i ];

		for( int z = 0, e = (int) arr.Height(); z < e; z ++ )
		{
			for( int x = 0, e = (int) arr.Width(); x < e; x ++ )
			{
				MegaTexTileInfo& info = arr[ z ][ x ];
				info.LayerList.ClearValues();
			}
		}
	}
}

//------------------------------------------------------------------------

void
r3dTerrain3::AddLayerToMegaInfo( int tx, int tz, int L, int layerIdx )
{
	MegaTexTileInfo& info = m_MegaTexTileInfoMipChain[ L ][ tz ][ tx ];
	info.LayerList.PushBack( layerIdx );
}

//------------------------------------------------------------------------

void
r3dTerrain3::DEBUG_RenderTileBoundaries()
{
#ifndef FINAL_BUILD
	if( r_terrain2_show_tiles->GetInt() && m_VisibleTiles.Count() )
	{
		m_DebugVisibleTiles.Resize( m_VisibleTiles.Count() );
		memcpy( &m_DebugVisibleTiles[ 0 ], &m_VisibleTiles[ 0 ], m_VisibleTiles.Count() * sizeof m_VisibleTiles[ 0 ] );

		m_DebugVisibleMegaTexTiles.Clear();

		const QualitySettings& curQS = GetCurrentQualitySettings();
		const r3dTerrainDesc& tdesc = GetDesc();

		float heightRange = float( tdesc.MaxHeight - tdesc.MinHeight ) / 65535.0f;

		int numMegaTileInsideMips = r3dLog2( curQS.HeightNormalAtlasTileDim ) - 1;

		for( int i = 0, e = (int)m_AllocMegaTexTileLodArray.Count(); i < e; i ++ )
		{
			int L = i;

			MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ i ];
			for( int j = 0, e = (int)arr.Count(); j < e; j ++ )
			{
				MegaTexTile* tile = arr[ j ];

				int actL = r3dLog2( m_QualitySettings.VertexTilesInMegaTexTileCount ) + L;

				float size = m_TileWorldDims[ actL ];

				float tileHeightMin = 0.f;
				float tileHeightRange = 0.f;

				if( tile->IsLoaded )
				{
					tileHeightMin = tile->TileInfoMips[ numMegaTileInsideMips - 1 ][ 0 ].MinHeight;
					tileHeightRange = tile->TileInfoMips[ numMegaTileInsideMips - 1 ][ 0 ].HeightRange * heightRange;
				}
				else
				{
					tileHeightMin = tdesc.MinHeight;
					tileHeightRange = tdesc.MaxHeight - tdesc.MinHeight;
				}

				tileHeightMin = tileHeightMin * heightRange + tdesc.MinHeight;

				r3dBoundBox bbox;
				bbox.Org = r3dPoint3D( tile->X * size, tileHeightMin, tile->Z * size );
				bbox.Size = r3dPoint3D( size, tileHeightRange, size );

				if( r3dRenderer->IsBoxInsideFrustum( bbox ) )
				{
					m_DebugVisibleMegaTexTiles.PushBack( tile );
				}
			}
		}
	}
	else
	{
		m_DebugVisibleTiles.Clear();
		m_DebugVisibleMegaTexTiles.Clear();
	}
#endif
}

//------------------------------------------------------------------------

int r3dTerrain3::IsMegaTileUsed( int tx, int tz, int L )
{
	for( int L = 0, e = (int)m_AllocTileLodArray.Count(); L < e; L ++ )
	{
		AllocatedTileArr& arr = m_AllocTileLodArray[ L ];

		for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
		{
			AllocatedTile& tile = arr[ i ];

			if( MegaTexTile* mtt = tile.MegaTile )
			{
				if( mtt->X == tx && mtt->Z == tz && mtt->L == L )
					return 1;
			}
		}
	}

	return 0;
}

//------------------------------------------------------------------------

void r3dTerrain3::ReloadMegaTileInMainThread( int tx, int tz, int L, int flags )
{
	MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ L ];

	for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
	{
		MegaTexTile& tile = *arr[ i ];

		if( tile.X == tx && tile.Z == tz && tile.L == L )
		{
#ifndef FINAL_BUILD
			if( !tile.IsLoading )
#endif
			DoLoadMegaTexTile( &tile, flags );
		}
	}
}

//------------------------------------------------------------------------

void
r3dTerrain3::SwapLayers( int layerIdx0, int layerIdx1 )
{
	R3D_SWAP( m_Layers[ layerIdx0 ], m_Layers[ layerIdx1 ] );

	InitLayers();
	RecalcLayerVars();

#if 0
	Info tinfo = GetInfo();

	int tileCountX = tinfo.MegaTileCountX;
	int tileCountZ = tinfo.MegaTileCountZ;

	for( int L = 0, e = (int)m_MegaTexTileInfoMipChain.Count(); L < e; L ++, tileCountX /= 2, tileCountZ /= 2 )
	{
		for( int tz = 0; tz < tileCountZ; tz ++ )
		{
			for( int tx = 0; tx < tileCountX; tx ++ )
			{
				if( MegaTexTileInfo* info = GetMegaTexTileInfo( tx, tz, L ) )
				{
					MegaTexTileInfo::LayerListArr::Iterator iter0 = info->LayerList.Find( layerIdx0 );
					int had0 = 0, had1 = 0;

					if( iter0.IsValid() )
					{
						info->LayerList.Erase( iter0 );
						had0 = 1;
					}

					MegaTexTileInfo::LayerListArr::Iterator iter1 = info->LayerList.Find( layerIdx1 );

					if( iter1.IsValid() )
					{
						info->LayerList.Erase( iter1 );
						had1 = 1;
					}

					if( had0 )
					{
						MegaTexTileInfo::LayerListArr::Iterator iter = info->LayerList.GetIterator();

						int idx = 0;
						for( ; iter.IsValid(); iter.Advance(), idx ++ )
						{
							if( iter.GetValue() > layerIdx1 )
								break;
						}
						if( idx )
							info->LayerList.Insert( info->LayerList.GetIterator( idx - 1 ), layerIdx1 );
						else
							info->LayerList.PushBack( layerIdx1 );
					}

					if( had1 )
					{
						MegaTexTileInfo::LayerListArr::Iterator iter = info->LayerList.GetIterator();

						int idx = 0;
						for( ; iter.IsValid(); iter.Advance(), idx ++ )
						{
							if( iter.GetValue() > layerIdx0 )
								break;
						}
						if( idx )
							info->LayerList.Insert( info->LayerList.GetIterator( idx - 1 ), layerIdx0 );
						else
							info->LayerList.PushBack( layerIdx0 );
					}
				}
			}
		}
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::ClearLayerInfo( int layerIdx )
{
	Info tinfo = GetInfo();

	int tileCountX = tinfo.MegaTileCountX;
	int tileCountZ = tinfo.MegaTileCountZ;

	for( int L = 0, e = (int)m_MegaTexTileInfoMipChain.Count(); L < e; L ++, tileCountX /= 2, tileCountZ /= 2 )
	{
		for( int tz = 0; tz < tileCountZ; tz ++ )
		{
			for( int tx = 0; tx < tileCountX; tx ++ )
			{
				if( MegaTexTileInfo* info = GetMegaTexTileInfo( tx, tz, L ) )
				{
					MegaTexTileInfo::LayerListArr::Iterator iter = info->LayerList.Find( layerIdx );

					if( iter.IsValid() )
					{
						info->LayerList.Erase( iter );
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------------

const r3dTerrain3Layer&
r3dTerrain3::GetLayer( int idx ) const
{
	return idx ? m_Layers[ idx - 1 ] : m_BaseLayer;
}

//------------------------------------------------------------------------

void
r3dTerrain3::SetLayer( int idx, const r3dTerrain3Layer& layer )
{
#if R3D_TERRAIN_V3_GRAPHICS
	( idx ? m_Layers[ idx - 1 ] : m_BaseLayer ) = layer;

	RecalcLayerVars();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::UpdateTilesWithLayer( int idx )
{
#if R3D_TERRAIN_V3_GRAPHICS
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
					if( MegaTexTile* megaTile = tile.MegaTile )
					{
						for( int i = 0, e = (int)megaTile->ActiveBitMasks; i < e; i ++ )
						{
							if( megaTile->LayerBitMasks[ i ].LayerIndex == idx - 1 )
							{
								tile.Dirty = 1;
								break;
							}
						}						
					}
				}
			}
		}
	}
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::InsertLayerInfo( int tileX, int tileZ, int tileL, int lidx )
{
	if( MegaTexTileInfo* info = GetMegaTexTileInfo( tileX, tileZ, tileL ) )
	{
		MegaTexTileInfo::LayerListArr::Iterator iter = info->LayerList.Find( lidx - 1 );

		if( !iter.IsValid() )
		{
			iter = info->LayerList.GetIterator();

			int inserted = 0;

			for( ; iter.IsValid(); iter.Advance() )
			{
				if( iter.GetValue() > lidx - 1 )
				{
					info->LayerList.Insert( iter, lidx - 1 );
					inserted = 1;
					break;
				}
			}
			if( !inserted )
				info->LayerList.PushBack( lidx - 1 );
		}
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::InsertLayerInfo( int tileX, int tileZ, int lidx )
{
	// update MegaTexTileInfo
	{
		int mtx = tileX;
		int mtz = tileZ;

		for( int L = 0, e = (int)m_MegaTexTileInfoMipChain.Count(); L < e; L ++, mtx /= 2, mtz /= 2 )
		{
			InsertLayerInfo( mtx, mtz, L, lidx );
		}
	}
}

//------------------------------------------------------------------------

void
r3dTerrain3::MarkLayer( int baseX, int baseZ, int lidx )
{
#if R3D_TERRAIN_V3_GRAPHICS
#ifndef FINAL_BUILD
	R3DPROFILE_FUNCTION( "r3dTerrain3::MarkLayer" );

	if( lidx > 0 )
	{
		r3dCSHolderWithDeviceQueue cs( m_MegaTexTileInfoCritSec ); ( void )cs;

		const QualitySettings& qs = GetCurrentQualitySettings();

		m_TilesToUpdate.Clear();

		int bitLodCount = r3dLog2( qs.MaskAtlasTileDim ) + 1;

		// update MegaTexTileInfo
		InsertLayerInfo( baseX / qs.MaskAtlasTileDim, baseZ / qs.MaskAtlasTileDim, lidx );

		int isSet = 0;

		for( int L = 0, e = m_AllocMegaTexTileLodArray.Count(), bx = baseX, bz = baseZ; L < e; L ++, bx /= 2, bz /= 2 )
		{
			MegaTexTilePtrArr& ptrArr = m_AllocMegaTexTileLodArray[ L ];

			int tx = bx / qs.MaskAtlasTileDim;
			int tz = bz / qs.MaskAtlasTileDim;

			for( int i = 0, e = (int)ptrArr.Count(); i < e; i ++ )
			{
				MegaTexTile* tile = ptrArr[ i ];

				if( tile->X == tx && tile->Z == tz )
				{
					CheckMegaTileConsistency( tile );

					if( !tile->IsLoaded )
					{
						int found = 0;
						for( int i = 0, e = m_EDITOR_TilesToReload.Count(); i < e; i ++ )
						{
							MegaTexTile* toReload = m_EDITOR_TilesToReload[ i ];
							if( toReload == tile )
							{
								found = 1;
								break;
							}
						}

						if( !found )
						{
							m_EDITOR_TilesToReload.PushBack( tile );
						}
						continue;
					}

					isSet = 0;

					int bitX = bx - tx * qs.MaskAtlasTileDim;
					int bitZ = bz - tz * qs.MaskAtlasTileDim;

					for( int i = 0, e = (int)tile->ActiveBitMasks; i < e; i ++ )
					{
						MegaTexTile::LayerBitMaskEntry& entry = tile->LayerBitMasks[ i ];

						if( entry.LayerIndex == lidx - 1 )
						{
							isSet = 1;

							for( int i = 0, e = bitLodCount, bx = bitX, bz = bitZ; i < e; i ++, bx /= 2, bz /= 2 )
							{
								entry.Bits[ i ].Set( bx, bz, 1 );
							}
							break;
						}
					}

					if( !isSet )
					{
						MegaTexTile::LayerBitMaskEntry newEntry;

						int numBitMips = r3dLog2( qs.MaskAtlasTileDim ) + 1;

						newEntry.Bits.Resize( numBitMips );

						newEntry.LayerIndex = lidx - 1;

						{
							for( int i = 0, e = bitLodCount, bx = bitX, bz = bitZ, bs = qs.MaskAtlasTileDim; i < e; i ++, bx /= 2, bz /= 2, bs /= 2 )
							{
								newEntry.Bits[ i ].Resize( bs, bs );
								newEntry.Bits[ i ].Set( bx, bz, 1 );
							}
						}

						if( tile->ActiveBitMasks )
						{
							int inserted = 0;

							for( int i = 0, e = (int)tile->ActiveBitMasks; i < e; i ++ )
							{
								MegaTexTile::LayerBitMaskEntry& entry = tile->LayerBitMasks[ i ];

								r3d_assert( entry.LayerIndex >= 0 );

								if( entry.LayerIndex > lidx - 1 )
								{
									tile->LayerBitMasks.Insert( i, newEntry );
									inserted = 1;
									break;
								}
							}

							if( !inserted )
							{
								if( tile->ActiveBitMasks < (int)tile->LayerBitMasks.Count() )
								{
									tile->LayerBitMasks[ tile->ActiveBitMasks ] = newEntry;
								}
								else
								{
									tile->LayerBitMasks.PushBack( newEntry );
								}								
								inserted = 1;
							}
						}
						else
						{
							if( tile->LayerBitMasks.Count() )
							{
								tile->LayerBitMasks[ 0 ] = newEntry;
							}
							else
							{
								tile->LayerBitMasks.PushBack( newEntry );
							}
						}

						tile->ActiveBitMasks ++;
						CheckMegaTileConsistency( tile );

						MegaTexTileInfo* info = GetMegaTexTileInfo( tile );

						r3d_assert( info->LayerList.Count() == tile->ActiveBitMasks );
					}

					CheckMegaTileConsistency( tile );
				}
			}
		}

		int scale = m_QualitySettings.VertexTileDim / r3dTerrain3::QualitySettings::MIN_VERTEX_TILE_DIM;

		int tx = baseX / scale;
		int tz = baseZ / scale;

		RefreshAtlasTile( tx, tz );
	}
#endif
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::CheckMegaTileConsistency( const MegaTexTile* mtt )
{
#if 0
	for( int i = 0, e = (int)mtt->ActiveBitMasks; i < e; i ++ )
	{
		const MegaTexTile::LayerBitMaskEntry& entry = mtt->LayerBitMasks[ i ];

		r3d_assert( entry.LayerIndex >= 0 );
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::GetDebugTileInfoAt( r3dPoint3D& pt, DebugTileInfo* oInfo, int requestedMaskId )
{
	for( int L = 0, e = (int)m_AllocTileLodArray.Count(); L < e; L ++ )
	{
		AllocatedTileArr& atr = m_AllocTileLodArray[ L ];

		float dim = m_TileWorldDims[ L ];

		int tx = int( pt.x / dim );
		int tz = int( pt.z / dim );

		for( int i = 0, e = (int)atr.Count(); i < e; i ++ )
		{
			AllocatedTile& at = atr[ i ];

			if( at.X == tx && at.Z == tz )
			{				
				oInfo->X = tx;
				oInfo->Z = tz;
				oInfo->L = L;

				int2 tileAtlasXZ = GetTileAtlasXZ( &at );

				oInfo->AtlasTileX = tileAtlasXZ.x;
				oInfo->AtlasTileZ = tileAtlasXZ.y;
				oInfo->AtlasVolumeID = at.AtlasVolumeID;

				if( at.AtlasVolumeID >= 0 )
				{
					oInfo->TileAtlasVolumeDiffuse = m_Atlas[ at.AtlasVolumeID ].Diffuse->Tex;
					oInfo->TileAtlasVolumeNormal = m_Atlas[ at.AtlasVolumeID ].Normal->Tex;
				}

				if( at.MegaTile )
				{					
					oInfo->MegaX = at.MegaTile->X;
					oInfo->MegaZ = at.MegaTile->Z;
					oInfo->MegaL = at.MegaTile->L;

					for( MegaTexTile::MaskEntryList::Iterator iter = at.MegaTile->MaskList.GetIterator(); iter.IsValid(); iter.Advance() )
					{
						int tileId, maskId;

						at.MegaTile->DecodeMaskEntry( iter.GetValue(), &oInfo->MegaMaskAtlasVolumeId, &tileId, &maskId );

						if( maskId == requestedMaskId )
						{
							int2 xz = GetMaskTileAtlasXZ( tileId );

							oInfo->MegaMaskX = xz.x;
							oInfo->MegaMaskZ = xz.y;

							oInfo->MegaMaskAtlasVolume = m_MaskAtlas[ oInfo->MegaMaskAtlasVolumeId ].Mask;
						}
					}
				}
				return;
			}
		}
	}
}

//------------------------------------------------------------------------

r3dTerrainOptiStats
r3dTerrain3::OptimizeLayerMasks( int isAutoSave, r3dTL::TArray< UINT16 > * oTempData, IDirect3DTexture9* packedTempMask, IDirect3DTexture9* unpackedTempMask )
{
	r3dTerrainOptiStats stats = { 0 };

#if R3D_TERRAIN_V3_GRAPHICS
#ifndef FINAL_BUILD
	if( isAutoSave )
	{
		return stats;
	}

	r3dOutToLog( "r3dTerrain3::OptimizeLayerMasks: optimizing %d masks..\n", (int)m_MegaTexGridFile_Map.size() );

	r3dRenderer->EndFrame();
	r3dRenderer->EndRender( true );

	r3dFinishBackGroundTasks();

	MaskIdSet masksForDeletion;

	float lastInfoFrame = r3dGetTime();
	const float INFO_DELTA = 0.125F;

	int countForReport = 0;

	Info info = GetInfo();

	for( FileTextureGridMap::const_iterator		miter = m_MegaTexGridFile_Map.begin(),
												e = m_MegaTexGridFile_Map.end();
												miter != e;
												++ miter, countForReport ++ )
	{

		if( r3dGetTime() - lastInfoFrame > INFO_DELTA )
		{
			lastInfoFrame = r3dGetTime();
			char buf[ 512 ];
			sprintf( buf, "Checking %d of %d", countForReport, (int)m_MegaTexGridFile_Map.size() );
			ReportProgress( buf );
		}

		short tx, tz;
		char L;

		int maskId;

		INT64 fullId = miter->first;

		MegaTexTile::DecodeID( fullId, &tx, &tz, &L, &maskId );

		if( L )
			continue;

		UnpackMask( oTempData, packedTempMask, unpackedTempMask, tx, tz, L, maskId );

		int chanPresent[ LAYERS_PER_MASK ] = { 0 };

		for( int i = 0, e = (int)oTempData->Count(); i < e; i ++ )
		{
			union
			{
				struct
				{
					UINT16 b : 5;
					UINT16 g : 6;
					UINT16 r : 5;
				};

				UINT16 sample;
			};

			sample = (*oTempData)[ i ];

			if( r )
				chanPresent[ 0 ] = 1;

			if( g )
				chanPresent[ 1 ] = 1;

			if( b )
				chanPresent[ 2 ] = 1;
		}

		int presentOnce = 0;

		for( int i = 0, e = LAYERS_PER_MASK; i < e; i ++ )
		{
			if( chanPresent[ i ] )
				presentOnce = 1;
		}

		if( !presentOnce )
		{
			masksForDeletion.insert( fullId );
		}

		int outOfBounds = 0;

		int tileCountX = info.MegaTileCountX >> L;
		int tileCountZ = info.MegaTileCountZ >> L;

		if( tx < 0 || tx >= tileCountX
				||
			tz < 0 || tz >= tileCountZ )
		{
			outOfBounds = 1;
		}

		if( outOfBounds )
		{
			r3dOutToLog( "Rogue tile( %d,%d,%d,%d )\n", tx, tz, L, maskId );
			masksForDeletion.insert( fullId );
		}
		else
		{
			MegaTexTileInfo* tileInfo = &m_MegaTexTileInfoMipChain[ L ][ tz ][ tx ];

			for( int i = 0, e = (int)tileInfo->LayerList.Count(); i < e; i ++ )
			{
				int layerVal = tileInfo->LayerList.GetIterator( i ).GetValue();

				int needErase = 0;

				int layerStart = maskId * LAYERS_PER_MASK;

				if( layerVal >= layerStart && layerVal < layerStart + LAYERS_PER_MASK )
				{
					if( !chanPresent[ layerVal - layerStart ] )
						needErase = 1;
				}

				if( needErase )
				{
					stats.NumDeletedLayerInfos ++;
					tileInfo->LayerList.Erase( tileInfo->LayerList.GetIterator( i ) );
					e --;
				}
				else
				{
					i ++;
				}
			}
		}
	}

	r3dTL::TArray< INT64 > missingMasks;

	GatherMasksAbsentInInfo( &missingMasks );

	r3dTL::TArray< int > layersToCheck;
	r3dTL::TArray< int > foundLayers;

	for( int i = 0, e = missingMasks.Count(); i < e; i ++ )
	{
		INT64 checkedMissingMask = missingMasks[ i ];

		if( masksForDeletion.find( missingMasks[ i ] ) == masksForDeletion.end() )
		{
			short x, z;
			char l;
			int m;
			MegaTexTile::DecodeID( missingMasks[ i ], &x, &z, &l, &m );

			r3dOutToLog("Trying to fix mask %lld (%d,%d,%d,%d) which is not present in layer info!\n", missingMasks[ i ], x, z, l, m );

			UnpackMask( oTempData, packedTempMask, unpackedTempMask, x, z, l, m );

			int r_present = 0, g_present = 0, b_present = 0;

			layersToCheck.Clear();

			for( int i = 0, e = (int)oTempData->Count(); i < e; i ++ )
			{
				union
				{
					struct
					{
						UINT16 b : 5;
						UINT16 g : 6;
						UINT16 r : 5;
					};

					UINT16 sample;
				};

				sample = (*oTempData)[ i ];

				if( r )
					r_present = 1;

				if( g )
					b_present = 1;

				if( b )
					g_present = 1;
			}

			int something_present = 0;

			if( r_present )
			{
				layersToCheck.PushBack( m * LAYERS_PER_MASK + 0 );
			}

			if( g_present )
			{
				layersToCheck.PushBack( m * LAYERS_PER_MASK + 1 );
			}

			if( b_present )
			{
				layersToCheck.PushBack( m * LAYERS_PER_MASK + 2 );
			}

			if( l )
			{
				int someChannelFound = 0;

				foundLayers.Clear();

				{
					for( int i = 0, e = (int)layersToCheck.Count(); i < e; i ++ )
					{
						int checkedLayer = layersToCheck[ i ];

						int found = 0;
						for( int ll = 0; ll < l ; ll ++ )
						{
							for( int tz = z << ( l - ll ), ez = ( z + 1 ) << ( l - ll ); tz < ez && !found ; tz ++ )
							{
								for( int tx = x << ( l - ll ), ex = ( x + 1 ) << ( l - ll ); tx < ex && !found ; tx ++ )
								{
									if( MegaTexTileInfo* info = GetMegaTexTileInfo( tx, tz, ll ) )
									{
										if( info->LayerList.Find( checkedLayer ).IsValid() )
										{
											found = 1;
										}
									}
								}
							}
						}

						if( found )
						{
							someChannelFound = 1;
							foundLayers.PushBack( checkedLayer );
						}
					}
				}


				if( !someChannelFound )
				{
					r3dOutToLog("Mask %lld (%d,%d,%d,%d) is not present in layer info, and has no higher mips with said layer info. Hence we delete it in spite of not being empty.\n", checkedMissingMask, x, z, l, m );
					masksForDeletion.insert( checkedMissingMask );
				}
				else
				{
					r3dOutToLog("Inserting layers for %lld (%d,%d,%d,%d) into layer info.\n", checkedMissingMask, x, z, l, m );

					for( int i = 0, e = foundLayers.Count(); i < e; i ++ )
					{
						for( int ll = l, e = GetInfo().NumActiveMegaTexLayers; ll < e; ll ++ )
						{
							InsertLayerInfo( x >> ( ll - l ), z >> ( ll - l ), ll, foundLayers[ i ] + 1 );
						}
					}
				}
			}
			else
			{
				r3dOutToLog("Inserting layers for %lld (%d,%d,%d,%d) into layer info.\n", checkedMissingMask, x, z, l, m );

				for( int i = 0, e = layersToCheck.Count(); i < e; i ++ )
				{
					for( int ll = l, e = GetInfo().NumActiveMegaTexLayers; ll < e; ll ++ )
					{
						InsertLayerInfo( x >> ( ll - l ), z >> ( ll - l ), ll, layersToCheck[ i ] + 1 );
					}
				}
			}
		}
	}

	stats.NumDeletedMasks = DeleteLayerMasks( masksForDeletion, 0 );

	SaveMegaTexTileInfos( r3dGameLevel::GetHomeDir() );
	SaveMegaTexMapFile( r3dGameLevel::GetHomeDir() );

	r3dRenderer->StartFrame();
	d3dc.Reset();
	r3dRenderer->StartRender( 0 );
#endif
#endif

	return stats;
}

//------------------------------------------------------------------------

void
r3dTerrain3::InsertLayer(	int idx, Floats* channel, 
							UShorts* tempShorts,
							IDirect3DTexture9* tempPackedMask,
							IDirect3DTexture9* tempUnpackedMask,
							const r3dTerrain3Layer* layerDesc )
{
#if R3D_TERRAIN_V3_GRAPHICS
	r3d_assert( idx >= 0 && idx <= (int)m_Layers.Count() );

	int splatElemCount = m_MaskResolutionU * m_MaskResolutionV;

	r3dTerrain3Layer tlayer;

	m_Layers.Insert( idx, tlayer );

	if( layerDesc )
	{
		m_Layers[ idx ] = *layerDesc;
	}

	InitLayers();

	RecalcLayerVars();

	UpdateDesc();

	RefreshAtlasTiles();

	if( channel )
	{
		int maskId = idx / r3dTerrain3::LAYERS_PER_MASK;
		int channelId = idx % r3dTerrain3::LAYERS_PER_MASK;

		const r3dTerrainDesc& desc = GetDesc();

		Info info = GetInfo();

		const QualitySettings& qs = GetCurrentQualitySettings();

		const int MASKTILEDIM = qs.MaskAtlasTileDim;

		int shrinkedCellSize = ( MASKTILEDIM - 2 * r3dTerrain3::MEGA_TEX_BORDER );

		int scaledWidth = info.MegaTileCountX * shrinkedCellSize;
		int scaledHeight = info.MegaTileCountZ * shrinkedCellSize;

		for( int tz = 0; tz < info.MegaTileCountZ; tz ++ )	
		{
			for( int tx = 0; tx < info.MegaTileCountX; tx ++ )
			{				
				Terrain3->UnpackMask( &*tempShorts, tempPackedMask, tempUnpackedMask, tx, tz, 0, maskId );

				int rawPsx = ( tx + 0 ) * shrinkedCellSize;
				int rawPsz = ( tz + 0 ) * shrinkedCellSize;

				int halfDim = qs.MaskAtlasTileDim / 2;

				for( int destZ = 0, e = qs.MaskAtlasTileDim; destZ < e; destZ ++ )
				{
					int srcZ = destZ + rawPsz - r3dTerrain3::MEGA_TEX_BORDER * 1;

					int finZ = srcZ;

					if( finZ < 0 || finZ >= (int)scaledHeight )
						continue;

					for( int destX = 0, e = qs.MaskAtlasTileDim; destX < e; destX ++ )
					{
						int srcX = destX + rawPsx - r3dTerrain3::MEGA_TEX_BORDER * 1;

						int finX = srcX;

						if( finX < 0 || finX >= (int)scaledWidth )
							continue;

						int destIdx = destX + destZ * qs.MaskAtlasTileDim;

						UINT16& destSample = (*tempShorts)[ destIdx ];

						UINT8 finalSample = R3D_MAX( R3D_MIN( int( (*channel)[ finZ * scaledWidth + finX ] ), 255 ), 0 );

						int channeledSample = 0;

						switch( channelId )
						{
						case 0:
							channeledSample = finalSample * 31 / 255;
							destSample &= ~( 0x1f << 11 );
							destSample |= channeledSample << 11;
							break;
						case 1:
							channeledSample = finalSample * 63 / 255;
							destSample &= ~( 0x3f << 5 );
							destSample |= channeledSample << 5;
							break;
						case 2:
							channeledSample = finalSample * 31 / 255;
							destSample &= ~0x1f;
							destSample |= channeledSample << 0;
							break;
						}
					}
				}

				Terrain3->UpdateMask( &( *tempShorts ), tempPackedMask, tempUnpackedMask, tx, tz, 0, maskId );
				Terrain3->InsertLayerInfo( tx, tz, idx + 1 );
			}
		}
	}

#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::ReportProgress( const char* msg )
{
	r3dRenderer->StartFrame();

	d3dc.Reset();

	r3dRenderer->StartRender( 0 );

	r3dRenderer->pd3ddev->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), r3dRenderer->GetClearZValue(), 0 );

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );

	extern	CD3DFont*	MenuFont_Editor;

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();

	MenuFont_Editor->DrawScaledText( 128.f, 64.f, 2, 2, 1, r3dColor::white, msg, 0 );

	r3dProcessWindowMessages();

	r3dRenderer->EndFrame();
	r3dRenderer->EndRender( true );
}

//------------------------------------------------------------------------

struct RelocatedTerrainMask
{
	INT64 id;
	INT64 oldOffset;
	INT64 newOffset;
	int gridX;
	int gridZ;
};

void r3dTerrain3::DeleteLayer( int lidx, IDirect3DTexture9* packedTempMask, IDirect3DTexture9* unpackedTempMask )
{
#ifndef FINAL_BUILD
	r3dRenderer->EndFrame();
	r3dRenderer->EndRender( true );

	r3dFinishBackGroundTasks();

	const QualitySettings& qs = GetCurrentQualitySettings();

	int tileCountX = m_TileCountX / qs.VertexTilesInMegaTexTileCount;
	int tileCountZ = m_TileCountZ / qs.VertexTilesInMegaTexTileCount;

	int startMaskIdx = ( lidx - 1 ) / LAYERS_PER_MASK;
	int maskCount = GetMaskCount();

	int neededMasks = maskCount - startMaskIdx;

	UShorts unpackedMask;
	r3dTL::TArray< r3dTL::TArray< UINT8 > > unpackedChannels;
	r3dTL::TArray< int > presentChannels;
	r3dTL::TArray< int > prevPresentMasks;

	MaskIdSet masksForDeletion;

	unpackedMask.Resize( qs.MaskAtlasTileDim * qs.MaskAtlasTileDim );

	float lastInfoFrame = r3dGetTime();
	const float INFO_DELTA = 0.125F;

	for( int L = 0; L < m_NumActiveMegaTexQualityLayers; L ++, tileCountX >>= 1, tileCountZ >>= 1 )
	{
		for( int tz = 0; tz < tileCountZ; tz ++ )
		{
			for( int tx = 0; tx < tileCountX; tx ++ )
			{
				unpackedChannels.Resize( neededMasks * LAYERS_PER_MASK );
				for( int i = 0, e = unpackedChannels.Count(); i < e; i ++ )
				{
					unpackedChannels[ i ].Clear();
					unpackedChannels[ i ].Resize( qs.MaskAtlasTileDim * qs.MaskAtlasTileDim, 0 );
				}
				presentChannels.Clear();
				presentChannels.Resize( neededMasks * LAYERS_PER_MASK, 0 );

				prevPresentMasks.Clear();
				prevPresentMasks.Resize( neededMasks, 0 );

				for( int m = startMaskIdx; m < maskCount; m ++ )
				{
					if( r3dGetTime() - lastInfoFrame > INFO_DELTA )
					{
						lastInfoFrame = r3dGetTime();
						char buf[ 512 ];
						sprintf( buf, "Updating (%d,%d,%d)", tx, tz, L );
						ReportProgress( buf );
					}

					INT64 id = MegaTexTile::GetID( tx, tz, L, m );
					FileTextureGridMap::iterator found = m_MegaTexGridFile_Map.find( id );

					int midx = m - startMaskIdx;

					if( found != m_MegaTexGridFile_Map.end() )
					{
						prevPresentMasks[ midx ] = 1;

						UnpackMask( &unpackedMask, packedTempMask, unpackedTempMask, tx, tz, L, m );

						for( int i = 0, e = (int)unpackedMask.Count(); i < e; i ++ )
						{
							union
							{
								struct
								{
									UINT16 b : 5;
									UINT16 g : 6;
									UINT16 r : 5;
								};

								UINT16 sample;
							};

							sample = unpackedMask[ i ];

							int r_conv = r * 255 / 31;
							int g_conv = g * 255 / 63;
							int b_conv = b * 255 / 31;

							int cidx = midx * LAYERS_PER_MASK;

							if( r_conv ) presentChannels[ cidx + 0 ] = 1;

							if( cidx + 1 < (int)unpackedChannels.Count() )
								if( g_conv ) presentChannels[ cidx + 1 ] = 1;

							if( cidx + 2 < (int)unpackedChannels.Count() )
								if( b_conv ) presentChannels[ cidx + 2 ] = 1;

							unpackedChannels[ cidx + 0 ][ i ] = r_conv;

							if( cidx + 1 < (int)unpackedChannels.Count() )
								unpackedChannels[ cidx + 1 ][ i ] = g_conv;
							else
								unpackedChannels[ cidx + 1 ][ i ] = 0;

							if( cidx + 2 < (int)unpackedChannels.Count() )
								unpackedChannels[ cidx + 2 ][ i ] = b_conv;
							else
								unpackedChannels[ cidx + 2 ][ i ] = 0;
						}
					}
				}

				{
					MegaTexTileInfo& info = m_MegaTexTileInfoMipChain[ L ][ tz ][ tx ];

					for( int i = 0, e = (int)presentChannels.Count(); i < e; i ++ )
					{
						int found = 0;
						for( MegaTexTileInfo::LayerListArr::Iterator iter = info.LayerList.GetIterator(); iter.IsValid(); iter.Advance() )
						{
							int val = iter.GetValue();

							if( val == i + startMaskIdx * LAYERS_PER_MASK )
							{
								found = 1;
								break;
							}
						}

						if( !found && presentChannels[ i ] )
						{
							r3dOutToLog( "Channel (%d,%d,%d,%d) present but not in tile info!\n", tx, tz, L, i + startMaskIdx * LAYERS_PER_MASK );
							presentChannels[ i ] = 0;
						}
					}
				}

				unpackedChannels.Erase( lidx - 1 - startMaskIdx * LAYERS_PER_MASK );
				presentChannels.Erase( lidx - 1 - startMaskIdx * LAYERS_PER_MASK );

				for( int midx = 0, e = prevPresentMasks.Count() ; midx < e; midx ++ )
				{
					int maskPresent = 0;

					for( int	i = ( midx + 0 ) * LAYERS_PER_MASK, 
								e = ( midx + 1 ) * LAYERS_PER_MASK;
									i < e;
									i ++ )
					{
						if( i < (int)presentChannels.Count() && presentChannels[ i ] )
						{
							maskPresent = 1;
							break;
						}
					}

					if( maskPresent )
					{
						INT64 id = MegaTexTile::GetID( tx, tz, L, midx + startMaskIdx );
						FileTextureGridMap::iterator found = m_MegaTexGridFile_Map.find( id );

						if( found == m_MegaTexGridFile_Map.end() )
						{
							UnpackMask( &unpackedMask, packedTempMask, unpackedTempMask, tx, tz, L, midx + startMaskIdx );
						}

						int cidx = midx * LAYERS_PER_MASK;
						for( int i = 0, e = unpackedMask.Count(); i < e; i ++ )
						{
							union
							{
								struct
								{
									UINT16 b : 5;
									UINT16 g : 6;
									UINT16 r : 5;
								};

								UINT16 sample;
							};

							r = unpackedChannels[ cidx + 0 ][ i ] * 31 / 255;

							if( cidx + 1 < (int)unpackedChannels.Count() )
								g = unpackedChannels[ cidx + 1 ][ i ] * 63 / 255;
							else
								g = 0;

							if( cidx + 2 < (int)unpackedChannels.Count() )
								b = unpackedChannels[ cidx + 2 ][ i ] * 31 / 255;
							else
								b = 0;

							unpackedMask[ i ] = sample;
						}

						UpdateMask( &unpackedMask, packedTempMask, unpackedTempMask, tx, tz, L, midx + startMaskIdx );
					}
					else
					{
						INT64 id = MegaTexTile::GetID( tx, tz, L, midx + startMaskIdx );
						masksForDeletion.insert( id );
					}
				}
			}
		}

		r3dOutToLog( "r3dTerrain3::DeleteLayer.Update: %d\n", L );
	}

	//------------------------------------------------------------------------
	// also delete "rogue masks"
	{
		Info info = GetInfo();

		for( FileTextureGridMap::iterator	i = m_MegaTexGridFile_Map.begin(),
											e = m_MegaTexGridFile_Map.end();
											i != e;
											++ i )
		{
			short x, z;
			char L;
			int maskID;

			MegaTexTile::DecodeID( i->first, &x, &z, &L, &maskID );

			int tileCountX = info.MegaTileCountX >> L;
			int tileCountZ = info.MegaTileCountZ >> L;

			if( x < 0 || x >= tileCountX
					||
				z < 0 || z >= tileCountZ )
			{
				r3dOutToLog( "Rogue tile( %d,%d,%d,%d )\n", x, z, L, maskID );
				masksForDeletion.insert( i->first );
			}
		}
	}

	//------------------------------------------------------------------------

	for( int L = 0, e = m_MegaTexTileInfoMipChain.Count(); L < e; L ++ )
	{
		MegaTexTileInfoArr& infoArr = m_MegaTexTileInfoMipChain[ L ];

		for( int tz = 0, e = infoArr.Height(); tz < e; tz ++ )
		{
			for( int tx = 0, e = infoArr.Width(); tx < e; tx ++ )
			{
				if( r3dGetTime() - lastInfoFrame > INFO_DELTA )
				{
					lastInfoFrame = r3dGetTime();
					char buf[ 512 ];
					sprintf( buf, "Deleting (%d,%d,%d)", tx, tz, L );
					ReportProgress( buf );
				}

				MegaTexTileInfo& info = infoArr[ tz ][ tx ];

				int eraseIdx = -1;
				int i = 0;

				for( MegaTexTileInfo::LayerListArr::Iterator iter = info.LayerList.GetIterator(); iter.IsValid(); iter.Advance(), i ++ )
				{
					int val = iter.GetValue();

					if( val == lidx - 1 )
					{
						eraseIdx = i;
					}
					else
					{
						if( val > lidx - 1 )
						{
							iter.SetValue( val - 1 );
						}
					}
				}

				if( eraseIdx >= 0 )
				{
					info.LayerList.Erase( info.LayerList.GetIterator( eraseIdx ) );
				}
			}
		}
	}

	//------------------------------------------------------------------------

	DeleteLayerMasks( masksForDeletion, startMaskIdx );

	//------------------------------------------------------------------------

	SaveMegaTexTileInfos( r3dGameLevel::GetHomeDir() );
	SaveMegaTexMapFile( r3dGameLevel::GetHomeDir() );

	//------------------------------------------------------------------------
	{
		int idx = lidx - 1;

		r3d_assert( idx >= 0 && idx < (int)m_Layers.Count() );

		m_Layers.Erase( idx );

		InitLayers();

		RecalcLayerVars();

		UpdateDesc();

		FreeMegaAtlasTiles();

		UpdateMegaTexTiles( gCam );

		SaveINI( r3dGameLevel::GetHomeDir(), m_VertexCountX, m_VertexCountZ );

		r3dFinishBackGroundTasks();

		RefreshAtlasTiles();
	}

	r3dRenderer->StartFrame();
	d3dc.Reset();
	r3dRenderer->StartRender( 0 );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::UpdateDominantLayerData()
{
#if R3D_TERRAIN_V3_GRAPHICS
#if 0
	ExtractMasksToChannels();
	DoUpdateDominantLayerData( m_MaskResolutionU, m_MaskResolutionV );
#else
	r3dError( "r3dTerrain3::UpdateDominantLayerData: implement!" );
#endif
#endif
}

//------------------------------------------------------------------------

int
r3dTerrain3::GetNumDecals() const
{
	return (int)m_DecalRecords.Count();
}

//------------------------------------------------------------------------

const r3dTerrain3::DecalRecord&
r3dTerrain3::GetDecalRecord( int idx ) const
{
	return m_DecalRecords[ idx ];
}

//------------------------------------------------------------------------

void
r3dTerrain3::SetDecalRecord( int idx, const DecalRecord& record )
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
r3dTerrain3::AddDecalRecord()
{
	m_DecalRecords.PushBack( DecalRecord() );
}

//------------------------------------------------------------------------

void
r3dTerrain3::UpdateQualitySettings()
{
	const QualitySettings& qs = SelectQualitySettings();
	SetQualitySettings( qs, false, 0 );
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

	D3D_V( D3DXCreateTextureFromFileEx( r3dRenderer->pd3ddev, eparams->path, eparams->width, eparams->height, 1, 0, TERRA3_LAYERMASK_FORMAT, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &convertedTex ) );

	eparams->result->Set( convertedTex );
}

//------------------------------------------------------------------------

void
r3dTerrain3::SwitchVFetchMode()
{
#if R3D_TERRAIN_V3_GRAPHICS
	m_AllowVFetch = !m_AllowVFetch;

	r3dError( "r3dTerrain3::SwitchVFetchMode: implement me!" );

	if( m_AllowVFetch )
	{
	
	}
	else
	{
		CreateDynaVertexBuffer();
	}
#endif
}

//------------------------------------------------------------------------

//------------------------------------------------------------------------
/*virtual*/

float
r3dTerrain3::GetHeight( int x, int z ) /*OVERRIDE*/
{
	const QualitySettings& qs = GetCurrentQualitySettings();
	const r3dTerrainDesc& tdesc = GetDesc();

	int chunkSize = qs.PhysicsTileCellCount;

	int chunksStartX = 0;
	int chunksStartZ = 0;

	int chunkEndX = 0;
	int chunkEndZ = 0;
	
#if R3D_TERRAIN3_ALLOW_FULL_PHYSCHUNKS
	if( m_FullPhysChunkModeOn )
	{
		chunksStartX = 0;
		chunksStartZ = 0;

		chunkEndX = tdesc.CellCountX;
		chunkEndZ = tdesc.CellCountZ;
	}
	else
#endif
	{
		chunksStartX = ( m_PhysChunksCentreX - PHYS_CHUNK_COUNT_PER_SIDE / 2 ) * chunkSize;
		chunksStartZ = ( m_PhysChunksCentreZ - PHYS_CHUNK_COUNT_PER_SIDE / 2 ) * chunkSize;

		chunkEndX = chunksStartX + PHYS_CHUNK_COUNT_PER_SIDE * chunkSize;
		chunkEndZ = chunksStartZ + PHYS_CHUNK_COUNT_PER_SIDE * chunkSize;
	}

	if( x >= chunksStartX && x <= chunkEndX
		&&
		z >= chunksStartZ && z <= chunkEndZ )
	{
		int chunkX = x / chunkSize;
		int chunkZ = z / chunkSize;

		for( int i = 0, e = (int)m_PhysicsChunks.Count(); i < e; i ++ )
		{
			PhysicsChunk& chunk = m_PhysicsChunks[ i ];

			if( chunk.ChunkX == chunkX && chunk.ChunkZ == chunkZ )
			{
				int chunksStartX1 = chunkX * chunkSize;
				int chunksStartZ1 = chunkZ * chunkSize;

				int relChunkX1 = x - chunksStartX1;
				int relChunkZ1 = z - chunksStartZ1;

				if( chunk.PhysicsHeightField )
					return ( chunk.PhysicsHeightField->getHeight( (PxReal)relChunkX1, (PxReal)relChunkZ1 ) + 32768 ) * m_InvHFScale + tdesc.MinHeight;
				else
					return tdesc.MinHeight;
			}
		}
	}

	return tdesc.MinHeight;
}

//------------------------------------------------------------------------
/*virtual*/

float
r3dTerrain3::GetHeight( const r3dPoint3D& pos )	/*OVERRIDE*/
{
	const QualitySettings& qs = GetCurrentQualitySettings();
	const r3dTerrainDesc& tdesc = GetDesc();

	float chunkSize = qs.PhysicsTileCellCount * tdesc.CellSize;

	float chunksStartX = 0.0f;
	float chunksStartZ = 0.0f;

	float chunkEndX = 0.0f;
	float chunkEndZ = 0.0f;

#if R3D_TERRAIN3_ALLOW_FULL_PHYSCHUNKS
	if( m_FullPhysChunkModeOn )
	{
		chunksStartX = 0.0f;
		chunksStartZ = 0.0f;

		chunkEndX = tdesc.XSize;
		chunkEndZ = tdesc.ZSize;
	}
	else
#endif
	{
		chunksStartX = ( m_PhysChunksCentreX - PHYS_CHUNK_COUNT_PER_SIDE / 2 ) * chunkSize;
		chunksStartZ = ( m_PhysChunksCentreZ - PHYS_CHUNK_COUNT_PER_SIDE / 2 ) * chunkSize;

		chunkEndX = chunksStartX + PHYS_CHUNK_COUNT_PER_SIDE * chunkSize;
		chunkEndZ = chunksStartZ + PHYS_CHUNK_COUNT_PER_SIDE * chunkSize;
	}

	if( pos.x >= chunksStartX && pos.x <= chunkEndX
			&&
		pos.z >= chunksStartZ && pos.z <= chunkEndZ )
	{
		int chunkX = int( pos.x / chunkSize );
		int chunkZ = int( pos.z / chunkSize );

		for( int i = 0, e = (int)m_PhysicsChunks.Count(); i < e; i ++ )
		{
			PhysicsChunk& chunk = m_PhysicsChunks[ i ];

			if( chunk.ChunkX == chunkX && chunk.ChunkZ == chunkZ )
			{
				float chunksStartX1 = chunkX * chunkSize;
				float chunksStartZ1 = chunkZ * chunkSize;

				float relChunkX1 = ( pos.x - chunksStartX1 ) / tdesc.CellSize;
				float relChunkZ1 = ( pos.z - chunksStartZ1 ) / tdesc.CellSize;

				if( chunk.PhysicsHeightField )
					return ( chunk.PhysicsHeightField->getHeight( relChunkX1, relChunkZ1 ) + 32768 ) * m_InvHFScale + tdesc.MinHeight;
				else
					return tdesc.MinHeight;
				}
			}
		}

	return tdesc.MinHeight;
}

//------------------------------------------------------------------------
/*virtual*/

r3dPoint3D
r3dTerrain3::GetNormal( const r3dPoint3D& pos )	/*OVERRIDE*/
{
	r3dVector Normal;
	GetNormalTangentBitangent(pos, &Normal, 0, 0);
	return Normal; 
}

//------------------------------------------------------------------------
/*virtual*/

void r3dTerrain3::GetNormalTangentBitangent(const r3dPoint3D &pos, r3dPoint3D *outNorm, r3dPoint3D *outTangent, r3dPoint3D *outBitangent)
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

void
r3dTerrain3::GetHeightRange( float* oMinHeight, float* oMaxHeight, r3dPoint2D start, r3dPoint2D end ) /*OVERRIDE*/
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
r3dTerrain3::GetMaterialType( const r3dPoint3D& pnt ) /*OVERRIDE*/
{
	int dominantDim = m_QualitySettings.MaskAtlasTileDim / m_QualitySettings.DominantLayerDataDiv;

	int idx = -1;

	for( int i = 0, e = R3D_MIN( (int)DOMINANT_DATA_MAX_LOD + 1, (int)m_AllocMegaTexTileLodArray.Count() ); i < e; i ++ )
	{
		MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ i ];

		float worldDim = m_TileWorldDims[ i + m_MegaTexTileLodOffset ];

		for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
		{
			MegaTexTile* mega = arr[ i ];

			if( !mega->IsLoaded )
				continue;

			if( !mega->DominantLayerData.Count() )
				continue;

			float xStart = mega->X * worldDim;
			float zStart = mega->Z * worldDim;

			if( pnt.X >= xStart && pnt.X < xStart + worldDim
					&&
				pnt.Z >= zStart && pnt.Z < zStart + worldDim )
			{
				int iX = int( ( pnt.X - xStart ) / worldDim * dominantDim );
				int iZ = int( ( pnt.Z - zStart ) / worldDim * dominantDim );

				iX = R3D_MIN( R3D_MAX( iX, 0 ), dominantDim - 1 );
				iZ = R3D_MIN( R3D_MAX( iZ, 0 ), dominantDim - 1 );

				idx = mega->DominantLayerData[ iX + iZ * dominantDim ];
				break;
			}
		}

		if( idx >= 0 )
			break;
	}

	if( idx < 0 )
		return g_pMaterialTypes->GetDefaultMaterial();


	if( idx >= 0 && idx < (int)m_MatTypeIdxes.Count() )
	{
		return g_pMaterialTypes->GetByIdx( m_MatTypeIdxes[ idx ] );
	}
	else
	{
		return g_pMaterialTypes->GetDefaultMaterial();
	}
}

//------------------------------------------------------------------------
/*virtual*/

bool
r3dTerrain3::IsLoaded() /*OVERRIDE*/
{
	return !!m_IsLoaded;
}

//------------------------------------------------------------------------
/*virtual*/

bool
r3dTerrain3::PrepareOrthographicTerrainRender( int renderTargetWidth, int renderTargetHeight ) /*OVERRIDE*/
{
#if R3D_TERRAIN_V3_GRAPHICS

	r3dFinishBackGroundTasks();

	r3d_assert( renderTargetWidth == renderTargetHeight );

	const QualitySettings& qs = GetCurrentQualitySettings();

	int ratio = m_MaskResolutionU / renderTargetWidth;

	int maskMip = 0;

	int maskCountU = 0;
	int maskCountV = 0;

	if( ratio > 1 )
	{
		for( int r = ratio; r >= 1; r /= 2 )
		{
			maskMip ++;
		}
	}
	else
	{
		maskMip = 0;
	}

	maskCountU = m_MaskResolutionU / qs.MaskAtlasTileDim >> maskMip;
	maskCountV = m_MaskResolutionV / qs.MaskAtlasTileDim >> maskMip;

	for( int z = 0, e = maskCountV; z < e; z ++ )
	{
		for( int x = 0, e = maskCountU; x < e; x ++ )
		{
			AddToAllocatedMegaTexTiles( x, z, maskMip );
		}
	}

	{
		MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ maskMip ];

		for( int i = 0, e = arr.Count(); i < e; i ++ )
		{
			MegaTexTile* tile = arr[ i ];

			if( !tile->IsInAtlas() )
			{
				AllocateMegaTexTile( tile );
			}

			if( !tile->IsLoaded )
			{
				DoLoadMegaTexTile( tile, LOADTILE_ALL );
			}
		}
	}

	// deallocate everything ( it'll get auto allocated before normal render again )
	RemoveAllocatedTiles();

	m_VisibleTiles.Clear();

	int atileMip = maskMip + m_MegaTexTileLodOffset;

	m_RenderOrthoTextureTileMip = atileMip;

	for( int z = 0, e = maskCountV; z < e; z ++ )
	{
		for( int x = 0, e = maskCountU; x < e; x ++ )
		{
			AddToAllocatedTiles( x, z, atileMip, 0 );
		}
	}

	StartTileUpdating();

	{
		AllocatedTileArr& targetMipArr = m_AllocTileLodArray[ atileMip ];

		for( int i = 0, e = (int)targetMipArr.Count(); i < e; i ++ )
		{
			AllocatedTile& atile = targetMipArr[ i ];

			AllocateAtlasTile( &atile );

			atile.MegaTile = FindCoveringMegaTexTile( &atile );

			UpdateTileInAtlas_G( &atile );
		}

		for( int i = 0, e = (int)targetMipArr.Count(); i < e; i ++ )
		{
			AllocatedTile& atile = targetMipArr[ i ];

			atile.MegaConFlags |= EAST_CONNECTION;
			atile.MegaConFlags |= SOUTH_CONNECTION;

			if( atile.MegaConFlags )
			{
				UpdateAdjecantHeights( &atile );
			}
		}
	}
	
	StopTileUpdating();

#if 0
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

#endif

	return true;
}

//------------------------------------------------------------------------
/*virtual*/

bool
r3dTerrain3::DrawOrthographicTerrain( const r3dCamera& Cam, bool UseZ ) /*OVERRIDE*/
{
#if R3D_TERRAIN_V3_GRAPHICS
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

	{
		AllocatedTileArr& arr = m_AllocTileLodArray[ m_RenderOrthoTextureTileMip ];

		for( int i = 0, e = arr.Count(); i < e; i ++ )
		{
			RenderTile_G( &arr[ i ], gCam, true );
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

static void ReloadTexture( r3dTexture* tex, int MaxTexDim )
{
	if( tex )
	{
		char fname[ 1024 ];
		r3dscpy( fname, tex->getFileLoc().FileName );

		tex->Unload();
		tex->Load( fname, D3DFMT_UNKNOWN, 1, MaxTexDim );
	}
}

void
r3dTerrain3::ReloadTextures() /*OVERRIDE*/
{
	int MaxTexDim = 0;
	if(r_terrain_quality->GetInt() < 2)
		MaxTexDim = 4096;

	ReloadTexture( m_BaseLayer.DiffuseTex, MaxTexDim );
	ReloadTexture( m_BaseLayer.NormalTex, MaxTexDim );

	for( int i = 0, e = m_Layers.Count(); i < e; i ++ )
	{
		ReloadTexture( m_Layers[ i ].DiffuseTex, MaxTexDim );
		ReloadTexture( m_Layers[ i ].NormalTex, MaxTexDim );
	}

	UpdateQualitySettings();

	RefreshAtlasTiles();
}

//------------------------------------------------------------------------
/*virtual*/

r3dTexture*
r3dTerrain3::GetDominantTexture( const r3dPoint3D &pos ) /*OVERRIDE*/
{
#if R3D_TERRAIN_V3_GRAPHICS
#if 0
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
#else
	r3dError( "r3dTerrain3::GetDominantTexture: implement me!" );
#endif
#endif

	return NULL;
}

//------------------------------------------------------------------------
/*virtual*/ r3dTexture* r3dTerrain3::GetNormalTexture() const /*OVERRIDE*/
{
	return m_LowResNormalTexture;
}

//------------------------------------------------------------------------
/*virtual*/ int r3dTerrain3::IsPosWithinPreciseHeights( const r3dPoint3D& pos ) const /*OVERRIDE*/
{
#if R3D_TERRAIN3_ALLOW_FULL_PHYSCHUNKS
	if( m_FullPhysChunkModeOn )
	{
		return 1;
	}
	else
#endif
	{
		const QualitySettings& qs = GetCurrentQualitySettings();
		const r3dTerrainDesc& tdesc = GetDesc();

		float chunkSize = qs.PhysicsTileCellCount * tdesc.CellSize;

		float chunksStartX = ( m_PhysChunksCentreX - PHYS_CHUNK_COUNT_PER_SIDE / 2 ) * chunkSize;
		float chunksStartZ = ( m_PhysChunksCentreZ - PHYS_CHUNK_COUNT_PER_SIDE / 2 ) * chunkSize;

		float chunkEndX = chunksStartX + PHYS_CHUNK_COUNT_PER_SIDE * chunkSize;
		float chunkEndZ = chunksStartZ + PHYS_CHUNK_COUNT_PER_SIDE * chunkSize;

		if( pos.x >= chunksStartX && pos.x <= chunkEndX
			&&
			pos.z >= chunksStartZ && pos.z <= chunkEndZ )
		{
			int chunkX = int( pos.x / chunkSize );
			int chunkZ = int( pos.z / chunkSize );

			for( int i = 0, e = (int)m_PhysicsChunks.Count(); i < e; i ++ )
			{
				const PhysicsChunk& chunk = m_PhysicsChunks[ i ];

				if( chunk.ChunkX == chunkX && chunk.ChunkZ == chunkZ )
				{
					if( chunk.PhysicsHeightField )
						return 1;
					else
						return 0;
				}
			}
		}
	}

	return 0;
}

//------------------------------------------------------------------------

int r3dTerrain3::GetNumLoadingMegaTiles() const
{
	int loadingCount = 0;

	for( int i = 0, e = (int)m_AllocMegaTexTileLodArray.Count(); i < e; i ++ )
	{
		const MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ i ];

		for( int i = 0, e = arr.Count(); i < e; i ++ )
		{
			MegaTexTile* tile = arr[ i ];

			if( !tile->IsLoaded )
				loadingCount ++;
		}
	}

	return loadingCount;
}

//------------------------------------------------------------------------

void
r3dTerrain3::Construct()
{
	m_TileCountX							= 0;
	m_TileCountZ							= 0;

	m_TilesInFileChunk						= 0;
	m_FileChunkCountX						= 0;
	m_FileChunkCountZ						= 0;

	m_AtlasTileCountPerSide					= 0;
	m_MaskAtlasTileCountPerSide				= 0;
	m_HeightNormalAtlasTileCountPerSide		= 0;

	m_AtlasTileCountPerSideInv				= 0;
	m_MaskAtlasTileCountPerSideInv			= 0;
	m_HeightNormalAtlasTileCountPerSideInv	= 0;

	m_HeightNormalTotalAllocatedTiles	= 0;
	m_HeightNormalMaxAllocatedTiles		= 0;

	m_TempDiffuseRT						= NULL;
	m_TempNormalRT						= NULL;
	m_TileVertexBuffer					= NULL;
	m_TileIndexBuffer					= NULL;
	m_LastDiffuseRT						= NULL;
	m_LastNormalRT						= NULL;
	m_UnitQuadVertexBuffer				= NULL;
	m_TerrainPosToMaskU				= 0.f;
	m_TerrainPosToMaskV				= 0.f;
	m_HeightScale					= 30.f;
	m_HeightOffset					= 0.f;
	m_TotalTerrainXLength			= 0.f;
	m_TotalTerrainZLength			= 0.f;
	m_NumActiveQualityLayers		= 0;
	m_TotalAllocatedTiles			= 0;
	m_MaxAllocatedTiles				= 0;
	m_4VertTileIndexOffset			= 0;
	m_DebugVisIndexOffset			= 0;
	m_TileUnitWorldDim				= 0.f;
	m_VertexCountX					= 0;
	m_VertexCountZ					= 0;
	m_MaskResolutionU				= 0;
	m_MaskResolutionV				= 0;
	m_PhysUserData					= NULL;

	m_HFScale						= 1.0f;
	m_InvHFScale					= 1.0f;
	m_HeightFieldDataCRC32			= 0;
	m_RoadTileUpdateZFar			= 0.f;
	m_IsLoaded						= 0;
	m_IsQualityUpdated				= 0;
	m_DynamicVertexBuffer			= 0;
	m_DynamicVertexBufferPtr		= 0;
	m_AllowVFetch					= 0;

	m_NormalDensity					= 1;
	m_MegaTexAtlasFile = NULL;
	m_MegaTexAtlasMipsFile = NULL;

	m_MegaTexAtlasFile_EDITOR_Size	= 0;

	m_MegaTexAtlasFile_EDITOR = INVALID_HANDLE_VALUE;
	m_MegaTexAtlasFile_EDITOR_Mapping = INVALID_HANDLE_VALUE;

#ifndef FINAL_BUILD
	m_EDITOR_IsCameraFixed = 0;

	m_MegaTexMipsRecalculated		= 0;
#endif

	m_BaseLayer.Name = "Base Layer";

	m_MegaTexAtlasFile_MaskOffset = 0;
	m_MegaTexAtlasFile_HeightNormalOffset = 0;

#if R3D_TERRAIN_V3_GRAPHICS
	m_AllowVFetch = r3dRenderer->SupportsVertexTextureFetch;
#endif

	g_MexaTexLoadTaskParams.Init( 2048 );

	m_MegaTexAtlasFile_NormalDim		= 256;
	m_MegaTexAtlasFile_HeightDim		= 256;
	m_MegaTexAtlasFile_MaskDim			= 256;
	m_MegaTexAtlasFile_BaseTileCountX	= 0;
	m_MegaTexAtlasFile_BaseTileCountZ	= 0;

	m_MegaTexAtlasFile_NormalFormat				= TERRA3_MEGANORMAL_FORMAT;
	m_MegaTexAtlasFile_HeightFormat				= TERRA3_MEGAHEIGHT_FORMAT;
	m_MegaTexAtlasFile_MaskFormat				= TERRA3_LAYERMASK_FORMAT;
	m_MegaTexAtlasFile_ColorModulationFormat	= TERRA3_COLORMODULATION_FORMAT;

	m_MegaTexAtlasFile_NumActiveLayers = 0;

	m_CellSize = 0;
	m_NumActiveMegaTexQualityLayers = 0;
	m_MaskTotalAllocatedTiles = 0;
	m_MaskMaxAllocatedTiles = 0;

	m_PhysChunksCentreX = 0;
	m_PhysChunksCentreZ = 0;

	m_MegaTexTileLodOffset = 0;

	m_RenderOrthoTextureTileMip = 0;

	m_LowResNormalTexture = NULL;
	m_Settings.FarNormalTexture = NULL;
	m_Settings.FarDiffuseTexture = NULL;

	m_RoadInfosUpdated = 0;
	m_StampID = 0;

	m_WasEdited = 0;

#ifndef FINAL_BUILD
	InitializeCriticalSection( &m_MegaTexFileCritSec );	
	InitializeCriticalSection( &m_MegaTexTileInfoCritSec );
#endif

#if R3D_TERRAIN3_ALLOW_FULL_PHYSCHUNKS
#ifndef WO_SERVER
	if( g_bEditMode && !r_terrain3_game_physics->GetInt() )
#endif //WO_SERVER
	{
		m_FullPhysChunkModeOn = 1;
	}
#ifndef WO_SERVER
	else
	{
		m_FullPhysChunkModeOn = 0;
	}
#endif //WO_SERVER
#endif //R3D_TERRAIN3_ALLOW_FULL_PHYSCHUNKS

	m_SharedPhysDescData = NULL;
}

//------------------------------------------------------------------------

void
r3dTerrain3::Destroy()
{
	for( int i = 0, e = (int)m_PhysicsChunks.Count(); i < e; i ++ )
	{
		PhysicsChunk& chunk = m_PhysicsChunks[ i ];

		if( chunk.PhysicsTerrain )
		{
			DestroyPhysHeightField( chunk );
			chunk.PhysicsHeightFieldDesc.samples.data = NULL;
		}
	}

	if( Terrain == Terrain3 )
	{
		Terrain = NULL;
	}

	Terrain3 = NULL;
	Close();

#ifndef FINAL_BUILD
	DeleteCriticalSection( &m_MegaTexFileCritSec );
	DeleteCriticalSection( &m_MegaTexTileInfoCritSec );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::CreateDynaVertexBuffer()
{
	if( !m_DynamicVertexBuffer )
	{
		m_DynamicVertexBuffer = gfx_new r3dVertexBuffer( 128 * 1024, sizeof( r3dDynaTerraVertex3), 0, true );	
	}
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

	D3D_V( D3DXCreateTextureFromFileInMemoryEx( r3dRenderer->pd3ddev, savedTexture->GetBufferPointer(), savedTexture->GetBufferSize(), params->Width, params->Height, 1, 0, TERRA3_LAYERMASK_FORMAT, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL,  NULL, &tex ) );

	params->Tunnel->Set( tex );
}

int
r3dTerrain3::DoLoad( const char* dir )
{
	const char* dirName = dir;

	char tempStr[ 1024 ];

	sprintf( tempStr, FNAME_TERRAIN3_INI, dirName );

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
			m_MaskResolutionU = script.GetInt();
		}
		else
		if( ! strcmp( buffer, "splat_res_v:" ) )
		{
			m_MaskResolutionV = script.GetInt();
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
		if( !strcmp( buffer, "far_fade_start:" ) )
		{
			m_Settings.FarFadeStart = script.GetFloat();
		}
		else
		if( !strcmp( buffer, "far_fade_end:" ) )
		{
			m_Settings.FarFadeEnd = script.GetFloat();
		}
		else
		if( !strcmp( buffer, "far_diffuse_texture_name:" ) )
		{
			char buffer[ 512 ];
			script.GetString( buffer, sizeof buffer - 1 );
			m_Settings.FarDiffuseTextureName = buffer;
		}
		else
		if( !strcmp( buffer, "far_normal_texture_name:" ) )
		{
			char buffer[ 512 ];
			script.GetString( buffer, sizeof buffer - 1 );
			m_Settings.FarNormalTextureName = buffer;
		}
		else
		if( !strcmp( buffer, "far_tile_enabled:" ) )
		{
			m_Settings.FarLayerEnabled = script.GetInt();
		}
		else
		if( !strcmp( buffer, "shadow_offset:" ) )
		{
			m_Settings.ShadowOffset = script.GetFloat();
		}
		else
		if( !strcmp( buffer, "fringe_height:" ) )
		{
			m_Settings.FringeHeight = script.GetFloat();
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

	if( !m_MaskResolutionU )
	{
		m_MaskResolutionU = m_VertexCountX;
		m_MaskResolutionV = m_VertexCountZ;
	}
	else
	{
		m_MaskResolutionU = R3D_MIN( m_MaskResolutionU, m_VertexCountX );
		m_MaskResolutionV = R3D_MIN( m_MaskResolutionV, m_VertexCountZ );
	}

	m_QualitySettings = SelectQualitySettings();

	RecalcVars();
	RecalcLayerVars();

	r3dOutToLog("TERRAIN3: Finished reading script file\n");
	script.CloseFile();

	if( !LoadBin( dirName ) )
		return 0;

	Init();

#if R3D_TERRAIN_V3_GRAPHICS

	InitLayers();

#endif

	UpdateDesc();

	RecreateLowResNormals();

	m_IsQualityUpdated = 0;
	m_IsLoaded = 1;

	if( g_bEditMode )
	{
		if( int n = CheckMegaMap() )
		{
			r3dArtBug( "(%s) Terrain BIN file is broken in %d places! Please contact Vlad about it.\n", dir, n );
		}
		else
		{
			r3dOutToLog( "CheckMegaMap: all good!\n" );
		}

		if( int n = CheckMegaOffsets() )
		{
			r3dArtBug( "(%s) Terrain masks are mismatched in %d places! Please contact Vlad about it.\n", dir, n );
		}
		else
		{
			r3dOutToLog( "CheckMegaOffsets: all good!\n" );
		}
	}

	return 1;
}

//------------------------------------------------------------------------

static uint32_t GetHeightFieldDataSize( const PxHeightFieldDesc& desc )
{
	return desc.nbColumns * desc.nbRows * desc.samples.stride;
}

static void PrintSplatLocation( char (& buf ) [ 1024 ], int idx )
{
	_snprintf( buf, 1024, "Terrain3\\Mat-Splat%d.dds", idx );
}

static void PrintFullSplatPath( const char* levelDir, int i, char (& buf ) [ 1024 ] )
{
	char tempStr[ 1024 ];
	PrintSplatLocation( tempStr, i );

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

void r3dTerrain3::MakeSaveDir( const char* dir )
{
	char terraDir[ 1024 ];
	strcpy( terraDir, dir );
	strcat( terraDir, "\\Terrain3" );
	mkdir( terraDir );
}

//------------------------------------------------------------------------

int
r3dTerrain3::SaveINI( const char* targetDir, int rescaleVertexCountX, int rescaleVertexCountZ )
{
	const QualitySettings& saveQS = m_BaseQualitySettings[ QS_HIGH ];

	const char * fileName = targetDir;
	FILE* hFile = fopen_for_write( Va( FNAME_TERRAIN3_INI, fileName ), "wt");
	assert( hFile );
	if ( !hFile )
		return 0;

	int rescaleSplatResolutionU = R3D_MIN( m_MaskResolutionU, rescaleVertexCountX );
	int rescaleSplatResolutionV = R3D_MIN( m_MaskResolutionV, rescaleVertexCountZ );

	float scaleFactor = float( m_VertexCountX ) / rescaleVertexCountX ;

	fprintf( hFile, "vert_count_x:\t\t%d\n", rescaleVertexCountX );
	fprintf( hFile, "vert_count_z:\t\t%d\n", rescaleVertexCountZ );
	fprintf( hFile, "splat_res_u:\t\t%d\n", rescaleSplatResolutionU );
	fprintf( hFile, "splat_res_v:\t\t%d\n", rescaleSplatResolutionV );
	fprintf( hFile, "tile_unit_size:\t%f\n", m_TileUnitWorldDim * scaleFactor );
	fprintf( hFile, "height_offset:\t%f\n", m_HeightOffset );
	fprintf( hFile, "height_scale:\t%f\n", m_HeightScale );
	fprintf( hFile, "normal_density:\t%d\n", m_NormalDensity );
	fprintf( hFile, "far_fade_start:\t%f\n", m_Settings.FarFadeStart );
	fprintf( hFile, "far_fade_end:\t%f\n", m_Settings.FarFadeEnd );
	fprintf( hFile, "fringe_height:\t%f\n", m_Settings.FringeHeight );

	if( m_Settings.FarDiffuseTextureName.Length() > 0 )
		fprintf( hFile, "far_diffuse_texture_name:\t\"%s\"\n", m_Settings.FarDiffuseTextureName.c_str() );

	if( m_Settings.FarNormalTextureName.Length() > 0 )
		fprintf( hFile, "far_normal_texture_name:\t\"%s\"\n", m_Settings.FarNormalTextureName.c_str() );

	fprintf( hFile, "far_tile_enabled:\t%d\n", m_Settings.FarLayerEnabled );
	fprintf( hFile, "shadow_offset:\t%f\n", m_Settings.ShadowOffset );

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

	return 1;
}

//------------------------------------------------------------------------


int
r3dTerrain3::DoSave( const char* targetDir, int rescaleVertexCountX, int rescaleVertexCountZ, bool saveMega )
{
#if R3D_TERRAIN_V3_GRAPHICS

	struct SaveRestoreCurrentQS
	{
		SaveRestoreCurrentQS( r3dTerrain3 * a_father )
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
		r3dTerrain3 * father;

	} saveRestoreCurrentQS ( this ); saveRestoreCurrentQS;

	const QualitySettings& saveQS = m_BaseQualitySettings[ QS_HIGH ];

	int isAutoSave = IsAutoSave( targetDir );

	MakeSaveDir( targetDir );

	SaveINI( targetDir, rescaleVertexCountX, rescaleVertexCountZ );

	if( saveMega )
	{
		MergeEditedMega( targetDir );
		SaveMegaTexMapFile( targetDir );
	}
	
	SaveMegaTexTileInfos( targetDir );

	if( m_WasEdited )
	{
		WriteMipStamps( targetDir );
		m_WasEdited = 0;
	}

#endif

	return 1;
}

static void RescaleThroughTexture( r3dTerrain3::Shorts* dest, const r3dTerrain3::Shorts& src, int srcVertexCountX, int srcVertexCountZ, int destVertexCountX, int destVertexCountZ )
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

static void FixBitMaskData( r3dBitMaskArray2D* oArray, const r3dTerrain3::Bytes& data )
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
r3dTerrain3::LoadBin( const char* dirName )
{
	InitTileField();

	InitMegaTexTileField();

	if( g_bEditMode )
	{
		char buff[ 512 ] = { 0 };
		GetChunkFileName( buff, 0, 0 );

		if( !r3dFileExists( buff ) )
		{
			LoadMegaTexMapFile_OLD( dirName );
			LoadMegaTexFile_OLD( dirName );

			LoadMegaTexTileInfos_OLD( dirName );

			if( !ConvertToFileGrid( dirName ) )
			{
				r3dError( "r3dTerrain3::LoadBin: couldn't convert from old format!" );
			}

			SaveMegaTexMapFile( dirName );
			SaveMegaTexTileInfos( dirName );

			m_MegaTexGridFile_Map.clear();
		}
	}

	if( !LoadMegaTexMapFile( dirName ) )
	{
		r3dError( "r3dTerrain3::LoadBin: couldn't load megatex map files!" );
	}

	AdvanceLoadingProgress( r3dITerrain::LoadingProgress * 0.25f / 3 );

	LoadMegaTexFile( dirName );

	AdvanceLoadingProgress( r3dITerrain::LoadingProgress * 0.25f / 3 );

	LoadMegaTexTileInfos( dirName );

	AdvanceLoadingProgress( r3dITerrain::LoadingProgress * 0.25f / 3 );

	r3dITerrain::LoadingProgress *= 0.75f;

#if R3D_TERRAIN_V3_GRAPHICS
	UpdateMegaTexMipsFile( dirName );
	OpenMegaTexAtlasMipsFile( "rb" );
#endif

	r3dITerrain::LoadingProgress = 0.f;

	return 1;
}

//------------------------------------------------------------------------

void r3dTerrain3::ExtractFloatHeights( const Shorts& shorts, Floats* oFloats )
{
	SetupHFScale();

	oFloats->Resize( shorts.Count() );

	for( int i = 0, e = shorts.Count(); i < e; i ++ )
	{
		int x = i % m_VertexCountX;
		int z = i / m_VertexCountX;

		(*oFloats)[ x * m_VertexCountZ + z ] = ( shorts[ i ] + 32768 ) * m_InvHFScale + GetDesc().MinHeight;
	}

}

//------------------------------------------------------------------------

void
r3dTerrain3::SaveLayerToScript( FILE* hFile, const r3dTerrain3Layer& layer, char* tab )
{
#if R3D_TERRAIN_V3_GRAPHICS
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
	fprintf( hFile, "%s\tscale_v:\t\t%f\n", 	tab, layer.ScaleV		);

	fprintf( hFile, "%s\tnormal_uv_mult:\t\t%f\n", tab, layer.NormUVMult	);

	fprintf( hFile, "%s\tspecular:\t\t%f\n", 	tab, layer.SpecularPow		);

	fprintf( hFile, "%s\tmat_type:\t%s\n",		tab, layer.MaterialTypeName.c_str()	);

	fprintf( hFile, "%s}\n", tab );
#endif
}

//------------------------------------------------------------------------

int
r3dTerrain3::LoadLayerFromScript( Script_c *script, r3dTerrain3Layer *layer )
{
#if !R3D_TERRAIN_V3_GRAPHICS
	return 0;
#endif

	char buffer[ MAX_PATH ];
	char szName[ MAX_PATH ];

	int hasTextures = 0;

	int MaxTexDim = 0;
	if(r_terrain_quality->GetInt() < 2)
		MaxTexDim = 4096;

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

			layer->DiffuseTex = r3dRenderer->LoadTexture( szName, D3DFMT_UNKNOWN, false, 1, MaxTexDim );				
		}
		else if ( ! strcmp( buffer, "map_normal:" ) )
		{
			hasTextures = 1;

			script->GetString( szName, sizeof( szName ) );
			layer->NormalTex = r3dRenderer->LoadTexture( szName, D3DFMT_UNKNOWN, false, 1, MaxTexDim );
		}
		else if ( ! strcmp( buffer, "scale_u:" ) )
		{
			layer->ScaleU = script->GetFloat();
		}
		else if ( ! strcmp( buffer, "scale_v:" ) )
		{
			layer->ScaleV = script->GetFloat();
		}
		else if( ! strcmp( buffer, "normal_uv_mult:" ) )
		{
			layer->NormUVMult = script->GetFloat();
		}
		else if ( ! strcmp( buffer, "specular:" ) )
		{
			layer->SpecularPow = script->GetFloat();
		}
		else if( ! strcmp( buffer, "mat_type:"  ) )
		{
			char buff[ 512 ] = {0};

			script->GetLine( buff, sizeof buff - 1 );
			// check for \n\r at the end of string
			for(int kk=0; kk<512; ++kk)
			{
				if(buff[kk]==0)
					break;
				if(buff[kk]=='\n')
					buff[kk]=0;
				if(buff[kk]=='\r')
					buff[kk]=0;
			}

			layer->MaterialTypeName = buff;
		}
	}

	return hasTextures;
}

//------------------------------------------------------------------------

void
r3dTerrain3::SetupHFScale()
{
#if 0
	float heightScale = R3D_MAX( fabs( m_HeightOffset ), fabs( m_HeightOffset + m_HeightScale ) );

	if(heightScale == 0.0f)
		heightScale = 1.0f;
#endif

	m_HFScale = 65535.0f / m_HeightScale;
	m_InvHFScale = 1.f / m_HFScale;
}

//------------------------------------------------------------------------

void r3dTerrain3::InitTileInfoMipChain( const QualitySettings& qs )
{
	m_MegaTexTileInfoMipChain.Resize( m_NumActiveMegaTexQualityLayers );

	int cellsPerTile = qs.MaskAtlasTileDim;

	int tileCountX = m_MaskResolutionU / cellsPerTile;
	int tileCountZ = m_MaskResolutionV / cellsPerTile;

	for( int i = 0, e = (int)m_MegaTexTileInfoMipChain.Count(); i < e; i ++, tileCountX /= 2, tileCountZ /= 2 )
	{
		tileCountX = R3D_MAX( tileCountX, 1 );
		tileCountZ = R3D_MAX( tileCountZ, 1 );

		m_MegaTexTileInfoMipChain[ i ].Resize( tileCountX, tileCountZ );

		MegaTexTileInfoArr& arr = m_MegaTexTileInfoMipChain[ i ];

		for( int z = 0, e = arr.Height(); z < e; z ++ )
		{
			for( int x = 0, e = arr.Width(); x < e; x ++ )
			{
				arr[ z ][ x ].LayerList.ClearValues();
			}
		}
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::UpdateHFShape( r3dTerrain3::PhysicsChunk* chunk )
{
	const r3dTerrainDesc& tdesc = GetDesc();
	const QualitySettings& curQS = GetCurrentQualitySettings();

	r3d_assert(_CrtCheckMemory());
	PxHeightFieldGeometry hfGeom = GetHFShape( chunk );

	// do not create a new shape, just update current shape with new parameters
	hfGeom.heightScale		= m_InvHFScale;
	hfGeom.rowScale			= tdesc.CellSize;
	hfGeom.columnScale		= tdesc.CellSize;

	r3d_assert( chunk->PhysicsTerrain );
	r3d_assert( chunk->PhysicsTerrain->getNbShapes () == 1 );

	r3d_assert(_CrtCheckMemory());
	PxShape* shapes[1] = {0};

	chunk->PhysicsTerrain->getShapes(&shapes[0], 1);

	r3d_assert(_CrtCheckMemory());

	r3d_assert( shapes[0]->getGeometryType() == PxGeometryType::eHEIGHTFIELD );

	r3d_assert(_CrtCheckMemory());
}

//------------------------------------------------------------------------

PxHeightFieldGeometry
r3dTerrain3::GetHFShape( PhysicsChunk* chunk ) const
{
	r3d_assert( chunk->PhysicsTerrain );
	r3d_assert( chunk->PhysicsTerrain->getNbShapes () == 1 );

	PxShape* shapes[1] = { 0 };
	chunk->PhysicsTerrain->getShapes( &shapes[0], 1 );
	r3d_assert( shapes[0]->getGeometryType() == PxGeometryType::eHEIGHTFIELD );
	PxHeightFieldGeometry geom;

	bool res = shapes[0]->getHeightFieldGeometry(geom);
	r3d_assert(res);

	return geom;
}

//------------------------------------------------------------------------

/*virtual*/
PxHeightFieldGeometry
r3dTerrain3::GetHFShape() const /*OVERRIDE*/
{
	return PxHeightFieldGeometry();
}

//------------------------------------------------------------------------

void
r3dTerrain3::PreparePhysXHeightFieldDesc_NoAlloc( PxHeightFieldDesc* hfDesc )
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
r3dTerrain3::PreparePhysXHeightFieldDesc( PxHeightFieldDesc* hfDesc )
{
	PreparePhysXHeightFieldDesc_NoAlloc( hfDesc );

	hfDesc->samples.data		= malloc( hfDesc->samples.stride * hfDesc->nbColumns* hfDesc->nbRows );

	if( hfDesc->samples.data == NULL )
		r3dError("Out of memory!");
}

//------------------------------------------------------------------------

void
r3dTerrain3::FinishPhysXHeightFieldDesc	( PxHeightFieldDesc* hfDesc )
{
	free((void*)hfDesc->samples.data);
	hfDesc->samples.data = NULL;
}

//------------------------------------------------------------------------

void r3dTerrain3::UpdatePhysChunks( const r3dCamera& cam )
{

#ifdef WO_SERVER
	r3d_assert(m_FullPhysChunkModeOn==1); // server should always have full phys chunks
#endif
	R3DPROFILE_FUNCTION( "r3dTerrain3::UpdatePhysChunks" );

#if R3D_TERRAIN3_ALLOW_FULL_PHYSCHUNKS
	if( m_FullPhysChunkModeOn )
	{
		R3DPROFILE_START( "m_FullPhysChunkModeOn" );

		const QualitySettings& curQS = GetCurrentQualitySettings();

		const r3dTerrainDesc& tdesc = Terrain3->GetDesc();
		const QualitySettings& qs = Terrain3->GetCurrentQualitySettings();

		int countX = R3D_MAX( tdesc.CellCountX / qs.PhysicsTileCellCount, 1 );
		int countZ = R3D_MAX( tdesc.CellCountZ / qs.PhysicsTileCellCount, 1 );

		if( !m_PhysicsChunks.Count() || !m_PhysicsChunks[ 0 ].PhysicsHeightField )
		{
#ifdef WO_SERVER
			r3dOutToLog("Terrain3: Creating physics data\n");
#endif

			r3dCSHolder cs( g_pPhysicsWorld->GetConcurrencyGuard() ); (void)cs;

			m_PhysicsChunks.Resize( countX * countZ );

			for( int z = 0, e = countZ; z < e; z ++ )
			{
				for( int x = 0, e = countX; x < e; x ++ )
				{
					DoCreatePhysHeightField( m_PhysicsChunks[ x + z * countX ], x, z, qs.PhysicsTileCellCount );
				}
			}

#ifdef WO_SERVER
			r3dOutToLog("Terrain3: Creating physics data - DONE!\n");
#endif
		}
		else
		{
			int critSect = 0;

			for( int z = 0, e = countZ; z < e; z ++ )
			{
				for( int x = 0, e = countX; x < e; x ++ )
				{
					PhysicsChunk& physChunk = m_PhysicsChunks[ x + z * countX ];

					if( physChunk.Dirty )
					{
						if( !critSect )
						{
							EnterCriticalSection( &g_pPhysicsWorld->GetConcurrencyGuard() );
							critSect = true;
						}

						if( physChunk.PhysicsTerrain )
						{
							physChunk.PhysicsTerrain->release();
							physChunk.PhysicsTerrain = NULL;
						}

						if( physChunk.PhysicsHeightField )
						{
							physChunk.PhysicsHeightField->release();
							physChunk.PhysicsHeightField = NULL;
						}

						CreatePhysHeightField( physChunk, x, z );
					}
				}
			}

			if( critSect )
			{
				LeaveCriticalSection( &g_pPhysicsWorld->GetConcurrencyGuard() );
			}
		}

		R3DPROFILE_END( "m_FullPhysChunkModeOn" );
	}
	else
#endif
	{
		m_PhysicsChunks.Resize( PHYS_CHUNK_COUNT_PER_SIDE * PHYS_CHUNK_COUNT_PER_SIDE );

		const QualitySettings& curQS = GetCurrentQualitySettings();

		const r3dTerrainDesc& desc = GetDesc();

		int camCellX = int( cam.x / desc.CellSize );
		int camCellZ = int( cam.z / desc.CellSize );

		camCellX = R3D_MIN( R3D_MAX( camCellX, 0 ), desc.CellCountX - 1 );
		camCellZ = R3D_MIN( R3D_MAX( camCellZ, 0 ), desc.CellCountZ - 1 );

		int camChunkX = camCellX / curQS.PhysicsTileCellCount;
		int camChunkZ = camCellZ / curQS.PhysicsTileCellCount;

		m_PhysChunksCentreX = camChunkX;
		m_PhysChunksCentreZ = camChunkZ;

		int critSect = false;

		int megaTileCountX = m_TileCountX / curQS.VertexTilesInMegaTexTileCount;
		int megaTileCountZ = m_TileCountZ / curQS.VertexTilesInMegaTexTileCount;

		R3DPROFILE_START( "Free Unusued Chunks" );

		for( int i = 0, e = m_PhysicsChunks.Count(); i < e; i ++ )
		{
			PhysicsChunk& physChunk = m_PhysicsChunks[ i ];

			if( physChunk.ChunkX < camChunkX - PHYS_CHUNK_COUNT_PER_SIDE / 2 
					||
				physChunk.ChunkZ < camChunkZ - PHYS_CHUNK_COUNT_PER_SIDE / 2 
					||
				physChunk.ChunkX > camChunkX + PHYS_CHUNK_COUNT_PER_SIDE / 2 
					||
				physChunk.ChunkZ > camChunkZ + PHYS_CHUNK_COUNT_PER_SIDE / 2 
					||
				physChunk.Dirty
					)
			{
				if( !critSect )
				{
					EnterCriticalSection( &g_pPhysicsWorld->GetConcurrencyGuard() );
					critSect = true;
				}

				if( physChunk.PhysicsTerrain )
				{
					physChunk.PhysicsTerrain->release();
					physChunk.PhysicsTerrain = NULL;
				}

				if( physChunk.PhysicsHeightField )
				{
					physChunk.PhysicsHeightField->release();
					physChunk.PhysicsHeightField = NULL;
				}
			}
		}

		R3DPROFILE_END( "Free Unusued Chunks" );

		for( int az = camChunkZ - PHYS_CHUNK_COUNT_PER_SIDE / 2, e = az + PHYS_CHUNK_COUNT_PER_SIDE; az < e; az ++ )
		{
			for( int ax = camChunkX - PHYS_CHUNK_COUNT_PER_SIDE / 2, e = ax + PHYS_CHUNK_COUNT_PER_SIDE; ax < e; ax ++ )
			{
				if( ax < 0 || ax >= megaTileCountX
							||
					az < 0 || az >= megaTileCountZ )
				{
					continue;
				}

				int found = 0;

				for( int i = 0, e = m_PhysicsChunks.Count(); i < e; i ++ )
				{
					PhysicsChunk& physChunk = m_PhysicsChunks[ i ];

					if( physChunk.ChunkX == ax && physChunk.ChunkZ == az && physChunk.PhysicsTerrain )
					{
						found = 1;
						break;
					}
				}

				if( !found )
				{
					for( int i = 0, e = m_PhysicsChunks.Count(); i < e; i ++ )
					{
						PhysicsChunk& physChunk = m_PhysicsChunks[ i ];

						if( !physChunk.PhysicsTerrain )
						{
							if( !critSect )
							{
								R3DPROFILE_START( "Phys CritSect" );

								EnterCriticalSection( &g_pPhysicsWorld->GetConcurrencyGuard() );
								critSect = true;

								R3DPROFILE_END( "Phys CritSect" );
							}

							CreatePhysHeightField( physChunk, ax, az );
							break;
						}
					}
				}
			}
		}

		if( critSect )
		{
			LeaveCriticalSection( &g_pPhysicsWorld->GetConcurrencyGuard() );
		}
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::AllocatePhysChunkSamples( PhysicsChunk* oChunk, int physicsTileCellCount )
{
	if( !oChunk->PhysicsHeightFieldDesc.samples.data )
	{
		const r3dTerrainDesc& tdesc = GetDesc();
		//const QualitySettings& curQS = GetCurrentQualitySettings();

		int sampleCount =  ( physicsTileCellCount + PHYS_TILE_BORDER ) * ( physicsTileCellCount + PHYS_TILE_BORDER );

		if( !m_SharedPhysDescData )
		{
			m_SharedPhysDescData = malloc( sampleCount * sizeof(PxHeightFieldSample) );
		}

		oChunk->PhysicsHeightFieldDesc.samples.data		= (PxHeightFieldSample*) m_SharedPhysDescData;
		oChunk->PhysicsHeightFieldDesc.samples.stride	= sizeof( PxHeightFieldSample );

		PxHeightFieldSample defSample;
		defSample.height = PxI16( tdesc.MinHeight * m_HFScale );
		defSample.materialIndex0 = 1;
		defSample.materialIndex1 = 1;
		defSample.clearTessFlag();

		for( int i = 0, e = sampleCount; i < e; i ++ )
		{
			(( PxHeightFieldSample* )oChunk->PhysicsHeightFieldDesc.samples.data)[ i ] = defSample;
		}
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::CreatePhysHeightField( PhysicsChunk& chunk, int chunkX, int chunkZ )
{
	const QualitySettings& curQS = GetCurrentQualitySettings();

	DoCreatePhysHeightField( chunk, chunkX, chunkZ, curQS.PhysicsTileCellCount );
}

//------------------------------------------------------------------------

void r3dTerrain3::DoCreatePhysHeightField( PhysicsChunk& chunk, int chunkX, int chunkZ, int physicsTileCellCount )
{
	R3DPROFILE_FUNCTION( "CreatePhysHeightField" );

	const r3dTerrainDesc& tdesc = GetDesc();
	const QualitySettings& curQS = GetCurrentQualitySettings();

	PxU32	minDim = (PxU32)R3D_MIN( (int)m_VertexCountX, (int)physicsTileCellCount );
	PxU32	w = minDim, h = minDim;

	float heightRange = ( tdesc.MaxHeight - tdesc.MinHeight ) / 65535.0f;

	DestroyPhysHeightField( chunk );

	chunk.ChunkX = chunkX;
	chunk.ChunkZ = chunkZ;

	int startCellX = chunkX * physicsTileCellCount;
	int startCellZ = chunkZ * physicsTileCellCount;

	chunk.StartX = startCellX;
	chunk.StartZ = startCellZ;

	int endCellX = startCellX + physicsTileCellCount;
	int endCellZ = startCellZ + physicsTileCellCount;

	int cellsInTile = m_QualitySettings.VertexTileDim * m_QualitySettings.VertexTilesInMegaTexTileCount;

	int tileSX = startCellX / cellsInTile;
	int tileSZ = startCellZ / cellsInTile;

	int tileEX = endCellX / cellsInTile;
	int tileEZ = endCellZ / cellsInTile;

	int numTilesInPhysTile = physicsTileCellCount / cellsInTile;

	r3d_assert( numTilesInPhysTile > 0 );

	AllocatePhysChunkSamples( &chunk, physicsTileCellCount );

	int totalPhysCellCount = ( physicsTileCellCount + PHYS_TILE_BORDER ) * ( physicsTileCellCount + PHYS_TILE_BORDER );

	for( int tz = tileSZ, tcz = 0; tz < tileEZ; tz ++, tcz += cellsInTile )
	{
		for( int tx = tileSX, tcx = 0; tx < tileEX; tx ++, tcx += cellsInTile )
		{
			if( tx < 0 || tz < 0 || tx >= tdesc.TileCountX || tz >= tdesc.TileCountZ )
				continue;

			UShorts& height = UnpackHeightCached( tx, tz );

			for( int zz = 0; zz < cellsInTile; zz ++ )
			{
				for( int xx = 0; xx < cellsInTile; xx ++ )
				{
					int targIdx = ( xx + tcx ) * ( physicsTileCellCount + PHYS_TILE_BORDER ) + zz + tcz;
					r3d_assert( targIdx < totalPhysCellCount );

					PxHeightFieldSample* currentSample = &( (PxHeightFieldSample*)chunk.PhysicsHeightFieldDesc.samples.data )[ targIdx ];

					UINT16 incomingHeight = height[ zz * cellsInTile + xx ];

					currentSample->height = incomingHeight - 32768;
				}
			}
		}
	}

	if( PHYS_TILE_BORDER )
	{
		int edgeOfX = chunk.StartX + physicsTileCellCount;
		int edgeOfZ = chunk.StartZ + physicsTileCellCount;

		int edgeTileX = edgeOfX / cellsInTile;
		int edgeTileZ = edgeOfZ / cellsInTile;

		int sampleCellCount = physicsTileCellCount + PHYS_TILE_BORDER;

		Info info = GetInfo();

		if( edgeTileX >= 0 && edgeTileX < info.MegaTileCountX )
		{
			for( int tz = tileSZ, tcz = 0; tz < edgeTileZ; tz ++, tcz += cellsInTile )
			{
				UShorts& height = UnpackHeightCached( edgeTileX, tz );

				for( int zz = 0; zz < cellsInTile; zz ++ )
				{
					int targIdx = ( sampleCellCount - 1 ) * sampleCellCount + zz + tcz;
					r3d_assert( targIdx < totalPhysCellCount );

					PxHeightFieldSample* currentSample = &( (PxHeightFieldSample*)chunk.PhysicsHeightFieldDesc.samples.data )[ targIdx ];

					UINT16 incomingHeight = height[ zz * cellsInTile + 0 ];

					currentSample->height = incomingHeight - 32768;
				}
			}
		}
		else
		{
			PxHeightFieldSample* currentSample = &( (PxHeightFieldSample*)chunk.PhysicsHeightFieldDesc.samples.data )[ sampleCellCount * ( sampleCellCount - 1 ) ];

			for( int i = 0, e = sampleCellCount; i < e; i ++ )
			{
				*( currentSample + i ) = *( currentSample + i - sampleCellCount);
			}			
		}

		if( edgeTileZ >= 0 && edgeTileZ < info.MegaTileCountZ )
		{
			for( int tx = tileSX, tcx = 0; tx < edgeTileX; tx ++, tcx += cellsInTile )
			{
				UShorts& height = UnpackHeightCached( tx, edgeTileZ );

				for( int xx = 0; xx < cellsInTile; xx ++ )
				{
					int targIdx = ( xx + tcx ) * sampleCellCount + sampleCellCount - 1;
					r3d_assert( targIdx < totalPhysCellCount );

					PxHeightFieldSample* currentSample = &( (PxHeightFieldSample*)chunk.PhysicsHeightFieldDesc.samples.data )[ targIdx ];

					UINT16 incomingHeight = height[ xx ];

					currentSample->height = incomingHeight - 32768;
				}
			}
		}
		else
		{
			PxHeightFieldSample* currentSample = &( (PxHeightFieldSample*)chunk.PhysicsHeightFieldDesc.samples.data )[ sampleCellCount - 1 ];

			for( int i = 0, e = sampleCellCount; i < e; i ++, currentSample += sampleCellCount )
			{
				currentSample->height = ( currentSample - 1 )->height;
			}
		}

		if( edgeTileX >= 0 && edgeTileX < info.MegaTileCountX
				&&
			edgeTileZ >= 0 && edgeTileZ < info.MegaTileCountZ )
		{
			UShorts& height = UnpackHeightCached( edgeTileX, edgeTileZ );
			int targIdx = ( sampleCellCount - 1 ) * sampleCellCount + sampleCellCount - 1;
			r3d_assert( targIdx < totalPhysCellCount );

			PxHeightFieldSample* currentSample = &( (PxHeightFieldSample*)chunk.PhysicsHeightFieldDesc.samples.data )[ targIdx ];

			UINT16 incomingHeight = height[ 0 ];

			currentSample->height = incomingHeight - 32768;
		}
		else
		{
			( (PxHeightFieldSample*)chunk.PhysicsHeightFieldDesc.samples.data )
					[ ( sampleCellCount - 1 ) * sampleCellCount + sampleCellCount - 1 ]
							=
						( (PxHeightFieldSample*)chunk.PhysicsHeightFieldDesc.samples.data )
								[ ( sampleCellCount - 2 ) * sampleCellCount + sampleCellCount - 2 ];
		}
	}

	chunk.PhysicsHeightFieldDesc.format					= PxHeightFieldFormat::eS16_TM;
	chunk.PhysicsHeightFieldDesc.nbColumns				= physicsTileCellCount + PHYS_TILE_BORDER;
	chunk.PhysicsHeightFieldDesc.nbRows					= physicsTileCellCount + PHYS_TILE_BORDER;
	chunk.PhysicsHeightFieldDesc.convexEdgeThreshold	= 0;
	chunk.PhysicsHeightFieldDesc.thickness				= -1000.0f;

	// allocate storage for samples
	chunk.PhysicsHeightFieldDesc.samples.stride	= sizeof(PxU32);

	R3DPROFILE_START( "Finalize" );

	chunk.PhysicsHeightField = g_pPhysicsWorld->PhysXSDK->createHeightField( chunk.PhysicsHeightFieldDesc );

	int kx = R3D_MAX( tdesc.CellCountX / physicsTileCellCount, 1 );
	int kz = R3D_MAX( tdesc.CellCountZ / physicsTileCellCount, 1 );

	float vertexXSize = m_TotalTerrainXLength / PxReal(w) / kx;
	float vertexZSize = m_TotalTerrainZLength / PxReal(h) / kz;

	PxHeightFieldGeometry shapeGeom( chunk.PhysicsHeightField, PxMeshGeometryFlags(), m_InvHFScale, vertexZSize, vertexXSize );
	PxTransform pose( 
		PxVec3(
			chunkX * tdesc.CellSize * physicsTileCellCount,
			0.5f * ( tdesc.MaxHeight - tdesc.MinHeight ) + tdesc.MinHeight,
			chunkZ * tdesc.CellSize * physicsTileCellCount), 
				PxQuat(0,0,0,1) );

	chunk.PhysicsTerrain = g_pPhysicsWorld->PhysXSDK->createRigidStatic(pose);

	PxShape* aHeightFieldShape = chunk.PhysicsTerrain->createShape(shapeGeom, *g_pPhysicsWorld->defaultMaterial);

	PxFilterData filterData(PHYSCOLL_STATIC_GEOMETRY, 0, 0, 0);
	aHeightFieldShape->setSimulationFilterData(filterData);
	PxFilterData qfilterData(1<<PHYSCOLL_STATIC_GEOMETRY, 0, 0, 0);

#ifndef WO_SERVER
#ifdef VEHICLES_ENABLED
	VehicleSetupDrivableShapeQueryFilterData(qfilterData);
#endif
#endif

	aHeightFieldShape->setQueryFilterData(qfilterData);

	g_pPhysicsWorld->AddActor(*chunk.PhysicsTerrain);

	chunk.PhysicsTerrain->userData = m_PhysUserData;

	UpdateHFShape( &chunk );

	chunk.Dirty = 0;
	chunk.PhysicsHeightFieldDesc.samples.data = NULL;

	R3DPROFILE_END( "Finalize" );
}

//------------------------------------------------------------------------

void r3dTerrain3::DestroyPhysHeightField( PhysicsChunk& chunk )
{
	R3DPROFILE_FUNCTION( "DestroyPhysHeightField" );

	if( chunk.PhysicsTerrain )
	{
		g_pPhysicsWorld->RemoveActor( *chunk.PhysicsTerrain );
		chunk.PhysicsTerrain->release();
		chunk.PhysicsTerrain = NULL;
	}

	if( chunk.PhysicsHeightField )
	{
		chunk.PhysicsHeightField->release();
		chunk.PhysicsHeightField = NULL;
	}
}

//------------------------------------------------------------------------

void
r3dTerrain3::UpdateSceneBox()
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

void r3dTerrain3::RecalcLayerVar( r3dTerrain3Layer* oLayer )
{
	oLayer->ShaderScaleU = oLayer->ScaleU * m_TerrainPosToMaskU;
	// note SplatU again - to maintain uniformity on non square terrains
	oLayer->ShaderScaleV = oLayer->ScaleV * m_TerrainPosToMaskU;
}

//------------------------------------------------------------------------

void
r3dTerrain3::RecalcTileCounts()
{
	m_TileCountX = m_VertexCountX / m_QualitySettings.VertexTileDim;
	m_TileCountZ = m_VertexCountZ / m_QualitySettings.VertexTileDim;
}

//------------------------------------------------------------------------

void r3dTerrain3::UpdateDesc()
{
	r3dTerrainDesc desc;

	desc.LayerCount = m_Layers.Count() + 1;

	desc.XSize = m_TotalTerrainXLength;
	desc.ZSize = m_TotalTerrainZLength;

	desc.CellCountX = m_VertexCountX;
	desc.CellCountZ = m_VertexCountZ;

	desc.MaskResolutionU = m_MaskResolutionU;
	desc.MaskResolutionV = m_MaskResolutionV;

	desc.MinHeight = m_HeightOffset;
	desc.MaxHeight = m_HeightOffset + m_HeightScale;

	desc.CellSize = m_CellSize;
	desc.CellCountPerTile = m_QualitySettings.VertexTileDim;

	SetDesc( desc );
}

//------------------------------------------------------------------------

void r3dTerrain3::RecalcVars()
{
	m_QualitySettings.Sync( NULL );

	RecalcTileCounts();

	m_AtlasTileCountPerSide			= ATLAS_TEXTURE_DIM / m_QualitySettings.AtlasTileDim;
	m_AtlasTileCountPerSideInv		= 1.0f / m_AtlasTileCountPerSide;

	m_MaskAtlasTileCountPerSide	= MASK_ATLAS_TEXTURE_DIM / m_QualitySettings.MaskAtlasTileDim;
	m_MaskAtlasTileCountPerSideInv = 1.0f / m_MaskAtlasTileCountPerSide;

	m_HeightNormalAtlasTileCountPerSide = HEIGHT_NORMAL_ATLAS_TEXTURE_DIM / m_QualitySettings.HeightNormalAtlasTileDim;
	m_HeightNormalAtlasTileCountPerSideInv = 1.0f / m_HeightNormalAtlasTileCountPerSide;

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

	m_TerrainPosToMaskU = 0.5f / m_TotalTerrainXLength;
	m_TerrainPosToMaskV = 0.5f / m_TotalTerrainZLength;

	m_CellSize = m_TileWorldDims [ 0 ] / m_QualitySettings.VertexTileDim;

	m_MegaTexTileLodOffset = r3dLog2( m_QualitySettings.VertexTilesInMegaTexTileCount );

	UpdateDesc();

	m_QualitySettings.Sync( &GetDesc() );

	//------------------------------------------------------------------------

	int fileChunkCountX = 0;
	int fileChunkCountZ = 0;

	Info info = GetInfo();

	if( info.MegaTileCountX >= info.MegaTileCountZ )
	{
		m_TilesInFileChunk = R3D_MAX( info.MegaTileCountX / FILE_CHUNK_COUNT_PER_SIDE, 1 );

		fileChunkCountX = info.MegaTileCountX / m_TilesInFileChunk;
		fileChunkCountZ = fileChunkCountX * m_TileCountZ / m_TileCountX;
	}
	else
	{
		m_TilesInFileChunk = R3D_MAX( info.MegaTileCountZ / FILE_CHUNK_COUNT_PER_SIDE, 1 );

		fileChunkCountZ = info.MegaTileCountZ / m_TilesInFileChunk;
		fileChunkCountX = fileChunkCountZ * m_TileCountX / m_TileCountZ;
	}

	m_FileChunkCountX = fileChunkCountX;
	m_FileChunkCountZ = fileChunkCountZ;
}

//------------------------------------------------------------------------

void r3dTerrain3::InitTileField()
{
	RecalcVars();
	SetupHFScale();

#if R3D_TERRAIN_V3_GRAPHICS
	m_VisibleTiles.Reserve( m_TileCountX * m_TileCountZ );

	m_NumActiveQualityLayers = NUM_QUALITY_LAYERS;

	int tileCountX = m_TileCountX;
	int tileCountZ = m_TileCountZ;

	for( int L = 0, e = NUM_QUALITY_LAYERS; L < e; L ++ )
	{
		tileCountX /= 2;
		tileCountZ /= 2;

		if( !tileCountX || !tileCountZ )
		{
			m_NumActiveQualityLayers = L + 1;
			break;
		}
	}

	m_NumActiveQualityLayers = R3D_MIN( (int)NUM_QUALITY_LAYERS, (int)m_NumActiveQualityLayers );
	
	m_AllocTileLodArray.Resize( m_NumActiveQualityLayers );
#endif
}

//------------------------------------------------------------------------

void  r3dTerrain3::UpdateMegaTexQualityLayerCount()
{
	m_QualitySettings.VertexTilesInMegaTexTileCount = ( m_VertexCountX / m_QualitySettings.VertexTileDim ) / ( m_MaskResolutionU / m_QualitySettings.HeightNormalAtlasTileDim );

	int tileCountX = m_MaskResolutionU / m_QualitySettings.VertexTileDim / m_QualitySettings.VertexTilesInMegaTexTileCount;
	int tileCountZ = m_MaskResolutionV / m_QualitySettings.VertexTileDim / m_QualitySettings.VertexTilesInMegaTexTileCount;

	m_NumActiveMegaTexQualityLayers = NUM_MASK_QUALITY_LAYERS;

	for( int L = 0, e = NUM_MASK_QUALITY_LAYERS; L < e; L ++ )
	{
		tileCountX /= 2;
		tileCountZ /= 2;

		if( !tileCountX || !tileCountZ )
		{
			m_NumActiveMegaTexQualityLayers = L + 1;
			break;
		}
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::InitMegaTexTileField()
{
	UpdateMegaTexQualityLayerCount();

#if R3D_TERRAIN_V3_GRAPHICS
	m_AllocMegaTexTileLodArray.Resize( m_NumActiveMegaTexQualityLayers );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::RecalcTileInfo( int tileX, int tileZ, int L, const UShorts& heightField )
{
#if R3D_TERRAIN_V3_GRAPHICS
	MegaTexTile* megaTile = GetMegaTexTile( tileX, tileZ, L );

	if( !megaTile )
		return;

	const QualitySettings& curQS = GetCurrentQualitySettings();

	r3d_assert( heightField.Count() == curQS.HeightNormalAtlasTileDim * curQS.HeightNormalAtlasTileDim );

	UpdateMegaTexTileCompressedTileInfoMipChain( megaTile, &heightField[ 0 ] );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::UpdateMegaTexTileCompressedTileInfoMipChain( MegaTexTile* megaTile, const UINT16* heightField )
{
	const QualitySettings& curQS = GetCurrentQualitySettings();

	int dimIterator = curQS.HeightNormalAtlasTileDim;

	int numMips = r3dLog2( dimIterator );

	megaTile->TileInfoMips.Resize( numMips );

	for( int i = 0, e = numMips; i < e; i ++ )
	{
		int prevDimIterator = dimIterator;
		
		dimIterator /= 2;

		MegaTexTile::CompressedTileInfoArr& arr = megaTile->TileInfoMips[ i ];

		MegaTexTile::CompressedTileInfoArr* prevArr( NULL );

		if( i )
		{
			prevArr = &megaTile->TileInfoMips[ i - 1 ];
		}

		arr.Resize( dimIterator * dimIterator );

		for( int z = 0, e = dimIterator; z < e; z ++ )
		{
			for( int x = 0, e = dimIterator; x < e; x ++ )
			{
				CompressedTileInfo& info = arr[ x + z * dimIterator ];

				info.MinHeight = 0xffff;
				info.HeightRange = 0;

				if( prevArr )
				{
					CompressedTileInfo& t00 = (*prevArr)[ x * 2 + 0 + ( z * 2 + 0 ) * prevDimIterator ];
					CompressedTileInfo& t10 = (*prevArr)[ x * 2 + 1 + ( z * 2 + 0 ) * prevDimIterator ];
					CompressedTileInfo& t01 = (*prevArr)[ x * 2 + 0 + ( z * 2 + 1 ) * prevDimIterator ];
					CompressedTileInfo& t11 = (*prevArr)[ x * 2 + 1 + ( z * 2 + 1 ) * prevDimIterator ];

					info.MinHeight		=	R3D_MIN(	R3D_MIN( t00.MinHeight, t10.MinHeight ),
														R3D_MIN( t01.MinHeight, t11.MinHeight ) );

					UINT16 maxHeight	=	R3D_MAX(	R3D_MAX(	t00.MinHeight + t00.HeightRange, 
																	t10.MinHeight + t10.HeightRange ),
														R3D_MAX(	t01.MinHeight + t01.HeightRange, 
																	t11.MinHeight + t11.HeightRange ) );

					info.HeightRange = maxHeight - info.MinHeight;

				}
				else
				{
					UINT16 h0 = heightField[ x * 2 + 0 + ( z * 2 + 0 ) * prevDimIterator ];
					UINT16 h1 = heightField[ x * 2 + 1 + ( z * 2 + 0 ) * prevDimIterator ];
					UINT16 h2 = heightField[ x * 2 + 0 + ( z * 2 + 1 ) * prevDimIterator ];
					UINT16 h3 = heightField[ x * 2 + 1 + ( z * 2 + 1 ) * prevDimIterator ];

					UINT16 minH = R3D_MIN( R3D_MIN( h0, h1 ), R3D_MIN( h2, h3 ) );
					UINT16 maxH = R3D_MAX( R3D_MAX( h0, h1 ), R3D_MAX( h2, h3 ) );

					if( x < e - 1 && z < e - 1 )
					{
						UINT16 h4 = heightField[ x * 2 + 2 + ( z * 2 + 0 ) * prevDimIterator ];
						UINT16 h5 = heightField[ x * 2 + 2 + ( z * 2 + 1 ) * prevDimIterator ];
						UINT16 h6 = heightField[ x * 2 + 2 + ( z * 2 + 2 ) * prevDimIterator ];
						UINT16 h7 = heightField[ x * 2 + 0 + ( z * 2 + 2 ) * prevDimIterator ];
						UINT16 h8 = heightField[ x * 2 + 1 + ( z * 2 + 2 ) * prevDimIterator ];

						minH = R3D_MIN( R3D_MIN( R3D_MIN( R3D_MIN( h4, h5 ), R3D_MIN( h6, h7 ) ), h8 ), minH );
						maxH = R3D_MAX( R3D_MAX( R3D_MAX( R3D_MAX( h4, h5 ), R3D_MAX( h6, h7 ) ), h8 ), maxH );
					}
					else
					{
						if( x < e - 1 )
						{
							UINT16 h4 = heightField[ x * 2 + 2 + ( z * 2 + 0 ) * prevDimIterator ];
							UINT16 h5 = heightField[ x * 2 + 2 + ( z * 2 + 1 ) * prevDimIterator ];

							minH = R3D_MIN( R3D_MIN( h4, h5 ), minH );
							maxH = R3D_MAX( R3D_MAX( h4, h5 ), maxH );
						}
						else
						{
							if( z < e - 1 )
							{
								UINT16 h4 = heightField[ x * 2 + 0 + ( z * 2 + 2 ) * prevDimIterator ];
								UINT16 h5 = heightField[ x * 2 + 1 + ( z * 2 + 2 ) * prevDimIterator ];

								minH = R3D_MIN( R3D_MIN( h4, h5 ), minH );
								maxH = R3D_MAX( R3D_MAX( h4, h5 ), maxH );
							}
						}
					}

					info.MinHeight = minH;
					info.HeightRange =  maxH - minH;
				}
			}
		}
	}

	r3d_assert( dimIterator == 1 );
}

//------------------------------------------------------------------------

void
r3dTerrain3::InitLayers()
{
	for( int i = 0, m = 0, ch = 0, e = m_Layers.Count(); i < e; i ++, ch ++ )
	{
		if( ch == 3 )
		{
			m ++;
			ch = 0;
		}

		r3dTerrain3Layer* layer = &m_Layers[ i ];

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
	}

	m_BaseLayer.ChannelIdx = 0;
	m_BaseLayer.ChannelMask = float4( 1, 0, 0, 0 );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE static void PushDebugBBox( const r3dBoundBox& bbox )
{
#if R3D_TERRAIN_V3_GRAPHICS
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

//------------------------------------------------------------------------

void
r3dTerrain3::UpdateVisibleTiles()
{
#if R3D_TERRAIN_V3_GRAPHICS
	m_VisibleTiles.Clear();

	const QualitySettings& curQS = GetCurrentQualitySettings();
	const r3dTerrainDesc& tdesc = GetDesc();

	int megaTileLodOffset = m_MegaTexTileLodOffset;
	int numTileInfoLodsInMega = r3dLog2( curQS.HeightNormalAtlasTileDim ) - 1;

	float heightRangeConv = ( tdesc.MaxHeight - tdesc.MinHeight ) / 65535.0f;

	for( int L = 0, e = m_NumActiveQualityLayers; L < e; L ++ )
	{
		AllocatedTileArr& layer = m_AllocTileLodArray[ L ];

		float size = m_TileWorldDims[ L ];

		for( int i = 0, e = layer.Count(); i < e; i ++ )
		{
			AllocatedTile& tile = layer[ i ];

			MegaTexTile* covering = tile.MegaTile;

			if( !covering )
			{
				r3dOutToLog( "r3dTerrain3::UpdateVisibleTiles: couldn't find covering tile for (%d,%d,%d)\n", tile.X, tile.Z, tile.L );
				return;
			}

			int lodDistance = covering->L + megaTileLodOffset - tile.L;

			int mult = 1 << lodDistance;

			int megaRelTileX = covering->X * mult;
			int megaRelTileZ = covering->Z * mult;

			int relTileX = tile.X - megaRelTileX;
			int relTileZ = tile.Z - megaRelTileZ;

			int idealLod = numTileInfoLodsInMega - lodDistance;

			const CompressedTileInfo& info = 
				idealLod >= 0 
					?
					covering->TileInfoMips[ idealLod ][ relTileX + relTileZ * mult]
						:
					covering->TileInfoMips[ 0 ][ ( relTileX >> -idealLod ) + ( relTileZ >> -idealLod ) * curQS.HeightNormalAtlasTileDim / 2 ];
					;

			int minHeightI = info.MinHeight;
			int heightRangeI = info.HeightRange;

			if( tile.MegaConFlags )
			{
				int maxHeightI = info.MinHeight + info.HeightRange;

				minHeightI = R3D_MIN( (int)tile.AdjecantMin, minHeightI );
				maxHeightI = R3D_MAX( (int)tile.AdjecantMax, maxHeightI );

				heightRangeI = maxHeightI - minHeightI;
			}

			float heightMin = minHeightI * heightRangeConv + tdesc.MinHeight;
			float heightRange = heightRangeI * heightRangeConv;

			r3dBoundBox bbox;
			bbox.Org = r3dPoint3D( tile.X * size, heightMin, tile.Z * size );
			bbox.Size = r3dPoint3D( size, heightRange, size );

			if( r3dRenderer->IsBoxInsideFrustum( bbox ) )
			{		
				m_VisibleTiles.PushBack( &tile );

#ifndef FINAL_BUILD
#if 0
				if( r_debug_helper->GetInt() == 221 )
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
r3dTerrain3::AddToAllocatedTiles( int x, int z, int L, int conFlags )
{
#if R3D_TERRAIN_V3_GRAPHICS
	R3DPROFILE_FUNCTION( "r3dTerrain3::AddToAllocatedTiles" );

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

		tile.Init();

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

r3dTerrain3::MegaTexTile* r3dTerrain3::GetFreeMegaTexTile()
{
	for( int i = 0, e = m_MegaTexTileEntries.Count(); i < e; i ++ )
	{
		MegaTexTileEntryChunk& entryArr = *m_MegaTexTileEntries[ i ];

		for( int i = 0, e = entryArr.COUNT; i < e; i ++ )
		{
			MegaTexTile& entry = entryArr[ i ];

			if( !entry.IsAllocated )
			{
				entry.IsAllocated = 1;
				return &entry;
			}
		}
	}

	MegaTexTileEntryChunk* newChunk = new MegaTexTileEntryChunk;

	m_MegaTexTileEntries.PushBack( newChunk );

	MegaTexTile& entry = (*newChunk)[ 0 ];

	entry.IsAllocated = 1;

	return &entry;
}

//------------------------------------------------------------------------

void r3dTerrain3::AddToAllocatedMegaTexTiles( int x, int z, int L )
{
#if R3D_TERRAIN_V3_GRAPHICS
	R3DPROFILE_FUNCTION( "r3dTerrain3::AddToAllocatedMegaTexTiles" );

	MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ L ];

	int tileCountX = m_TileCountX / m_QualitySettings.VertexTilesInMegaTexTileCount;

	int W = ( tileCountX >> L );

	int incomingId = W * z + x;

	int l = 0;
	int r = arr.Count() - 1;

	int insert_idx = -1;

	if( r >= l )
	{
		for( ; ; )
		{
			int idx = ( l + r ) / 2;

			int id = arr[ idx ]->Z * W + arr[ idx ]->X;

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
					arr[ idx ]->Tagged = 1;
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
		MegaTexTile* tile = GetFreeMegaTexTile();

		tile->Init();

		tile->X = x;
		tile->Z = z;

		tile->Tagged = 1;
		tile->L = L;

		tile->IsLoaded = 0;

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
r3dTerrain3::UpdateMegaTexTiles( const r3dCamera& cam )
{
#if R3D_TERRAIN_V3_GRAPHICS
	R3DPROFILE_FUNCTION( "r3dTerrain3::UpdateMegaTexTiles" );
	//------------------------------------------------------------------------
	// untag

	for( int L = 0, e = m_NumActiveMegaTexQualityLayers; L < e; L ++ )
	{
		MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ L ];

		for( int i = 0, e = arr.Count(); i < e; i ++ )
		{
			MegaTexTile* tile = arr[ i ];
			tile->Tagged = 0;
		}
	}

	//------------------------------------------------------------------------
	// update activity according to camera		

	R3DPROFILE_START( "Tile Activity" );

	int maskTilesX = m_TileCountX / m_QualitySettings.VertexTilesInMegaTexTileCount;
	int maskTilesZ = m_TileCountZ / m_QualitySettings.VertexTilesInMegaTexTileCount;

	int tcx = maskTilesX;
	int tcz = maskTilesZ;

	r3dTL::TFixedArray< int, NUM_QUALITY_LAYERS > countXs;
	r3dTL::TFixedArray< int, NUM_QUALITY_LAYERS > countZs;

	for( int i = 0, e = m_NumActiveMegaTexQualityLayers; i < e; i ++ )
	{
		countXs[ i ] = tcx;
		countZs[ i ] = tcz;

		tcx = R3D_MAX( tcx / 2, 1 );
		tcz = R3D_MAX( tcz / 2, 1 );
	}

	int z_bottom_prev	= 0;
	int z_top_prev		= countZs[ m_NumActiveMegaTexQualityLayers - 1 ];
	int x_left_prev		= 0;
	int	x_right_prev	= countXs[ m_NumActiveMegaTexQualityLayers - 1 ];

#ifndef FINAL_BUILD
	if( r_force_min_terramegalod->GetInt() )
	{
		z_top_prev		= countZs[ r_min_terramegalod->GetInt() ];
		x_right_prev	= countXs[ r_min_terramegalod->GetInt() ];
	}
#endif


	// always keep lowest mip for cases when requested tile is not yet loaded
	for( int z = 0, e = countZs[ m_NumActiveMegaTexQualityLayers - 1 ]; z < e; z ++ )
	{
		for( int x = 0, e = countXs[ m_NumActiveMegaTexQualityLayers - 1 ]; x < e; x ++ )
		{
			AddToAllocatedMegaTexTiles( x, z, m_NumActiveMegaTexQualityLayers - 1 );
		}
	}

	r3dTL::TFixedArray< float, NUM_QUALITY_LAYERS > tileDistances;

	for( int i = 0; i < NUM_QUALITY_LAYERS; i ++ )
	{
		tileDistances[ i ] = 2 * m_TileWorldDims[ i ] * m_QualitySettings.VertexTilesInMegaTexTileCount;

		if( i )
		{
			tileDistances[ i ] += tileDistances[ i - 1 ];
		}
	}

#ifndef FINAL_BUILD
	if( !r_force_min_terramegalod->GetInt() )
#endif
	{
		for( int L = m_NumActiveMegaTexQualityLayers - 1, e = 1 
#ifndef FINAL_BUILD
			+ r_min_terramegalod->GetInt()
#endif
			; L >= e; L -- )
		{
			int sx = maskTilesX / countXs[ L ];
			int sz = maskTilesZ / countZs[ L ];

			int connMask = 0xffffffff;

			float halfDim = m_HalfTileWorldDims[ L ] * m_QualitySettings.VertexTilesInMegaTexTileCount;

			float tileDistance = tileDistances[ L - 1 ];
			float tileWorldDim = m_TileWorldDims[ L ] * m_QualitySettings.VertexTilesInMegaTexTileCount;

			float distDown = cam.Z - tileDistance + halfDim;

			int endZ = (int) ( distDown / tileWorldDim );
			endZ = R3D_MIN( R3D_MAX( endZ, 0 ), countZs[ L ] );

			float distUp = cam.Z + tileDistance + halfDim;

			int startZ = ( int ) ( distUp / tileWorldDim );
			startZ = R3D_MIN( R3D_MAX( startZ, 0 ), countZs[ L ] );

			float distLeft = cam.X - tileDistance + halfDim;

			int endX = ( int )( distLeft / tileWorldDim );
			endX = R3D_MIN( R3D_MAX( endX, 0 ), countXs[ L ] );

			float distRight = cam.X + tileDistance + halfDim;

			int startX = ( int )( distRight / tileWorldDim );
			startX = R3D_MIN( R3D_MAX( startX, 0 ), countXs[ L ] );

			//------------------------------------------------------------------------
			// bottom

			for( int z = z_bottom_prev, e = endZ; z < e; z ++ )
			{
				for( int x = x_left_prev, e = x_right_prev; x < e; x ++ )
				{
					AddToAllocatedMegaTexTiles( x, z, L );
				}
			}

			//------------------------------------------------------------------------
			// up

			for( int z = startZ, e = z_top_prev; z < e; z ++ )
			{
				for( int x = x_left_prev, e = x_right_prev; x < e; x ++ )
				{
					AddToAllocatedMegaTexTiles( x, z, L );
				}
			}

			//------------------------------------------------------------------------
			// left

			for( int z = z_bottom_prev, e = z_top_prev; z < e; z ++ )
			{
				for( int x = x_left_prev, e = endX; x < e; x ++ )
				{
					AddToAllocatedMegaTexTiles( x, z, L );
				}
			}

			//------------------------------------------------------------------------
			// right

			for( int z = z_bottom_prev, e = z_top_prev; z < e; z ++ )
			{
				for( int x = startX, e = x_right_prev; x < e; x ++ )
				{
					AddToAllocatedMegaTexTiles( x, z, L );
				}
			}

			z_bottom_prev = endZ * 2;
			z_top_prev = startZ * 2;

			x_left_prev	= endX * 2;
			x_right_prev = startX * 2;
		}
	}

	// fill in highest lod in remaining tiles

	int MIN_LOD = 0;

#ifndef FINAL_BUILD
	MIN_LOD = r_min_terramegalod->GetInt();
#endif

	for( int z = z_bottom_prev, e = z_top_prev; z < e; z ++ )
	{
		for( int x = x_left_prev, e = x_right_prev; x < e; x ++ )
		{
			AddToAllocatedMegaTexTiles( x, z, MIN_LOD );
		}
	}

	R3DPROFILE_END( "Tile Activity" );

	//------------------------------------------------------------------------
	// sync activity with atlas tile allocation

	R3DPROFILE_START( "Load mask tiles" );

	for( int L = 0, e = m_NumActiveMegaTexQualityLayers; L < e; L ++ )
	{
		MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ L ];

		for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
		{
			MegaTexTile* tile = arr[ i ];

			if( !tile->IsInAtlas() )
			{
				r3d_assert( !tile->IsLoaded );

				AllocateMegaTexTile( tile );
				AddMegaTexTileLoadJobs( tile, LOADTILE_ALL );
			}
		}
	}

	OptimizeMegaTexTiles();

	{
		const QualitySettings& curQS = GetCurrentQualitySettings();
		int lodOffset = m_MegaTexTileLodOffset;

		for( int i = 0, e = m_AllocMegaTexTileLodArray.Count(); i < e; i ++ )
		{
			MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ i ];

			for( int i = 0, e = arr.Count(); i < e; i ++ )
			{
				MegaTexTile* megaTile = arr[ i ];

				if( megaTile->Dirty )
				{
					int startX	= ( megaTile->X + 0 ) << ( megaTile->L + lodOffset );
					int endX	= ( megaTile->X + 1 ) << ( megaTile->L + lodOffset );
					int startZ	= ( megaTile->Z + 0 ) << ( megaTile->L + lodOffset );
					int endZ	= ( megaTile->Z + 1 ) << ( megaTile->L + lodOffset );

					int L = megaTile->L + lodOffset;

					for( int i = 0, e = L; i < e; i ++ )
					{
						AllocatedTileArr& arr = m_AllocTileLodArray[ i ];

						for( int i = 0, e = arr.Count(); i < e; i ++ )
						{
							AllocatedTile& tile = arr[ i ];
#if 0
							if( tile.MegaTile && tile.MegaTile != megaTile )
								continue;
#endif
							int tsx = ( tile.X + 0 ) << tile.L;
							int tsz = ( tile.Z + 0 ) << tile.L;
							int tex = ( tile.X + 1 ) << tile.L;
							int tez = ( tile.Z + 1 ) << tile.L;

							// they shouldn't intersect so we can test like this
							if( tsx >= startX && tsz >= startZ && tex <= endX && tez <= endZ )
							{
								tile.Dirty = 1;
							}
						}
					}

					megaTile->Dirty = 0;
				}
			}
		}
	}

	R3DPROFILE_END( "Load mask tiles" );
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::UpdateTiles_G( const r3dCamera& cam )
{
#if R3D_TERRAIN_V3_GRAPHICS
	R3DPROFILE_FUNCTION( "r3dTerrain3::UpdateTiles_G" );

	//------------------------------------------------------------------------
	// untag

	for( int L = 0, e = m_NumActiveQualityLayers; L < e; L ++ )
	{
		AllocatedTileArr& arr = m_AllocTileLodArray[ L ];

		for( int i = 0, e = arr.Count(); i < e; i ++ )
		{
			AllocatedTile& tile = arr[ i ];
			tile.Tagged = 0;
			tile.MegaTile = NULL;
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

#ifndef FINAL_BUILD
	if( r_force_min_terramegalod->GetInt() )
	{
		int LOD = r_min_terramegalod->GetInt() + m_MegaTexTileLodOffset;
		z_top_prev		= countZs[ LOD ];
		x_right_prev	= countXs[ LOD ];
	}
#endif

	for( int L = 
#ifndef FINAL_BUILD
		r_force_min_terramegalod->GetInt()
		? 
		r_min_terramegalod->GetInt() + m_MegaTexTileLodOffset
			:
#endif
		m_NumActiveQualityLayers - 1
		; L >= 1; L -- )
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

			if( m_Settings.FarLayerEnabled )
			{
				float tileDimDiag = m_TileWorldDims[ tile.L ] * 1.42f * 2.0f;

				float tileDim = m_TileWorldDims[ tile.L ];

				float dx = ( tile.X + 0.5f ) * tileDim - cam.X;
				float dz = ( tile.Z + 0.5f ) * tileDim - cam.Z;

				float tileDistanceToCam = sqrt( dx * dx + dz * dz );

				tile.FarNearFlags = 0;

				if( tileDistanceToCam < m_Settings.FarFadeEnd + tileDimDiag )
					tile.FarNearFlags |= AllocatedTile::NEAR_TILE;

				if( tileDistanceToCam > m_Settings.FarFadeStart - tileDimDiag )
					tile.FarNearFlags |= AllocatedTile::FAR_TILE;
			}
			else
			{
				tile.FarNearFlags = AllocatedTile::NEAR_TILE;
			}
		}

		for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
		{
			AllocatedTile& tile = arr[ i ];

			if( tile.AtlasTileID < 0 )
			{
				if( tile.FarNearFlags & AllocatedTile::NEAR_TILE )
				{
					AllocateAtlasTile( &tile );
				}
				else
				{
					tile.Dirty = 0;
				}
			}

			if( tile.Dirty && tile.AtlasTileID >= 0 )
			{
#if !ENABLE_MULTI_GPU_OPTIMIZATIONS
				m_TilesToUpdate.PushBack( &tile );
#endif
			}
#if 0
			else
			{
				m_TilesToUpdate.PushBack( &tile );
			}
#endif

#if ENABLE_MULTI_GPU_OPTIMIZATIONS
			m_TilesToUpdate.PushBack( &tile );
#endif
		}
	}

	//------------------------------------------------------------------------

	for( int L = 0, e = m_NumActiveQualityLayers; L < e; L ++ )
	{
		AllocatedTileArr& arr = m_AllocTileLodArray[ L ];

		for( int i = 0, e = arr.Count(); i < e; i ++ )
		{
			AllocatedTile& tile = arr[ i ];
			if( tile.Tagged )
			{
				tile.MegaTile = FindCoveringMegaTexTile( &tile );
			}
		}
	}

	//------------------------------------------------------------------------

	if( m_TilesToUpdate.Count() > 2 )
	{
		std::sort( &m_TilesToUpdate[ 0 ], &m_TilesToUpdate[ 0 ] + m_TilesToUpdate.Count() - 1, tile_comp_func_ATLAS_ID );
	}

	D3DPERF_BeginEvent( 0, L"r3dTerrain3::UpdateTiles" );

#ifndef FINAL_BUILD
	if( m_TilesToUpdate.Count() )
	{
		for( int i = 0, e = (int)m_TileUpdateLog.Count() - 1; i < e; i ++ )
		{
			m_TileUpdateLog[ i ] = m_TileUpdateLog[ i + 1 ];
		}
	}
#endif

	StartTileUpdating();

	for( int i = 0, e = m_TilesToUpdate.Count(); i < e; i ++ )
	{
		AllocatedTile* tile = m_TilesToUpdate[ i ];
		UpdateTileInAtlas_G( tile );		
	}

	StopTileUpdating();

#ifndef FINAL_BUILD
	if( m_TilesToUpdate.Count() )
	{
		m_TileUpdateLog.GetLast() = m_TilesToUpdate.Count();
	}
#endif

	for( int L = 0, e = m_AllocTileLodArray.Count(); L < e; L ++ )
	{
		AllocatedTileArr& arr = m_AllocTileLodArray[ L ];

		for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
		{
			AllocatedTile& tile = arr[ i ];

			if( !tile.Tagged )
				continue;

			if( MegaTexTile* megaTile = tile.MegaTile )
			{
				int lodDistance = megaTile->L + m_MegaTexTileLodOffset - tile.L;
				int relSize = 1 << lodDistance;

				int relX = tile.X - ( megaTile->X << lodDistance );
				int relZ = tile.Z - ( megaTile->Z << lodDistance );

				int conns = 0;

				if( relX == relSize - 1 )
					conns |= EAST_CONNECTION;

				if( relZ == relSize - 1 )
					conns |= SOUTH_CONNECTION;

				if( conns != tile.MegaConFlags || conns )
				{
					UpdateAdjecantHeights( &tile );
				}
			}
		}
	}

	D3DPERF_EndEvent();

	UpdateTileRoads();

	R3DPROFILE_END( "Render tiles" );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::UpdateTileMips()
{
#if R3D_TERRAIN_V3_GRAPHICS
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

void r3dTerrain3::UpdateTileRoads()
{
#if R3D_TERRAIN_V3_GRAPHICS
	D3DPERF_BeginEvent( 0, L"r3dTerrain3::UpdateAtlasTileRoads" );
	// start and stop no matter what, this will prevent a pair of bugs
	StartTileRoadUpdating();

	if( m_QualitySettings.BakeRoads )
	{
		for( int i = 0, e = m_TilesToUpdate.Count(); i < e; i ++ )
		{
			const AllocatedTile* tile = m_TilesToUpdate[ i ];
			if( tile->AtlasVolumeID >= 0 )
			{
				UpdateAtlasTileRoads( tile );
			}
		}
	}

	StopTileRoadUpdating();
	D3DPERF_EndEvent();
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::AddAtlasVolume()
{
#if R3D_TERRAIN_V3_GRAPHICS
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
r3dTerrain3::FreeAtlas()
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
r3dTerrain3::AllocateAtlasTile( AllocatedTile* tile )
{
#if R3D_TERRAIN_V3_GRAPHICS
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
				tile->Dirty = 1;
				m_TotalAllocatedTiles ++;
				m_MaxAllocatedTiles = R3D_MAX( m_MaxAllocatedTiles, m_TotalAllocatedTiles );
				return;
			}
		}
	}

	r3dError( "r3dTerrain3::AllocateAtlasTile: out of space!" );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::FreeAtlasTile( AllocatedTile* tile )
{
#if R3D_TERRAIN_V3_GRAPHICS
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

void r3dTerrain3::CreateMaskTexture(r3dTexture *mask)
{
	if (mask)
	{
		mask->Create( MASK_ATLAS_TEXTURE_DIM, MASK_ATLAS_TEXTURE_DIM, TERRA3_LAYERMASK_FORMAT, 1, D3DPOOL_DEFAULT );
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::AddMaskAtlasVolume()
{
#if R3D_TERRAIN_V3_GRAPHICS
	MaskAtlasVolume volume;

	int mipCount = MipCount( m_QualitySettings.AtlasTileDim ) - 2;
	// we're making 4x4 mip monocolor and ignore the rest, because otherwise we have to waste too
	// much extra space to allow smooth tile to tile transition

	mipCount = 1;

	volume.Mask = r3dRenderer->AllocateTexture();
	CreateMaskTexture(volume.Mask);

	volume.Occupied.Resize( m_MaskAtlasTileCountPerSide * m_MaskAtlasTileCountPerSide, 0 );

	volume.FreeTiles = m_MaskAtlasTileCountPerSide * m_MaskAtlasTileCountPerSide;

	m_MaskAtlas.PushBack( volume );
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::FreeMaskAtlas()
{
	for( int i = 0, e = m_MaskAtlas.Count(); i < e; i ++ )
	{
		r3dRenderer->DeleteTexture( m_MaskAtlas[ i ].Mask );
	}

	m_MaskAtlas.Clear();
}

//------------------------------------------------------------------------

void r3dTerrain3::AllocateMaskAtlasTile( MegaTexTile* tile )
{
#if R3D_TERRAIN_V3_GRAPHICS

	int mipOffset = r3dLog2( m_QualitySettings.VertexTilesInMegaTexTileCount );

	MegaTexTileInfo* info = GetMegaTexTileInfo( tile );

	r3d_assert( info );

	for( int i = 0, e = GetMaskCount(); i < e; i ++ )
	{
		int maskIsNeeded = 0;

		int maskIdx = i;

		MegaTexTileInfo::LayerListArr::Iterator iter = info->LayerList.GetIterator();

		for( ; iter.IsValid(); iter.Advance() )
		{
			if( iter.GetValue() / LAYERS_PER_MASK == maskIdx )
			{
				maskIsNeeded = 1;
			}
		}

		if( maskIsNeeded )
		{
			int allocated = 0;

			for( int i = 0 ; !allocated; i ++ )
			{
				if( i >= (int)m_MaskAtlas.Count() )
				{
					AddMaskAtlasVolume();
				}

				MaskAtlasVolume& vol = m_MaskAtlas[ i ];

				if( !vol.FreeTiles )
					continue;

				for( int t = 0, e = vol.Occupied.Count(); t < e && !allocated; t ++ )
				{
					if( !vol.Occupied[ t ] )
					{
						allocated = 1;

						vol.FreeTiles --;
						vol.Occupied[ t ] = 1;

						UINT32 entry = 0;

						MegaTexTile::EncodeMaskEntry( &entry, i, t, maskIdx );

						tile->PushMaskEntry( entry );

						m_MaskTotalAllocatedTiles ++;
						m_MaskMaxAllocatedTiles = R3D_MAX( m_MaskMaxAllocatedTiles, m_MaskTotalAllocatedTiles );
					}
				}
			}
		}
	}
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::AppendToMaskAtlasTile( MegaTexTile* tile )
{
#if R3D_TERRAIN_V3_GRAPHICS

	int mipOffset = r3dLog2( m_QualitySettings.VertexTilesInMegaTexTileCount );

	int neededCount = 0;

	MegaTexTileInfo* info = GetMegaTexTileInfo( tile );

	r3d_assert( info );

	for( int i = 0, e = GetMaskCount(); i < e; i ++ )
	{
		int maskIsNeeded = 0;

		int maskIdx = i;

		MegaTexTileInfo::LayerListArr::Iterator iter = info->LayerList.GetIterator();

		for( ; iter.IsValid(); iter.Advance() )
		{
			if( iter.GetValue() / LAYERS_PER_MASK == maskIdx )
			{
				maskIsNeeded = 1;
			}
		}

		if( maskIsNeeded )
		{
			MegaTexTile::MaskEntryList::Iterator iter = tile->MaskList.GetIterator();

			int found = 0;
			for( ; iter.IsValid(); iter.Advance() )
			{
				int val = iter.GetValue();

				int atlas, tile, mask;
				MegaTexTile::DecodeMaskEntry( val, &atlas, &tile, &mask );

				if( mask == maskIdx )
				{
					found = 1;
					break;
				}
			}

			if( !found )
			{
				int allocated = 0;

				for( int i = 0 ; !allocated; i ++ )
				{
					if( i >= (int)m_MaskAtlas.Count() )
					{
						AddMaskAtlasVolume();
					}

					MaskAtlasVolume& vol = m_MaskAtlas[ i ];

					if( !vol.FreeTiles )
						continue;

					for( int t = 0, e = vol.Occupied.Count(); t < e && !allocated; t ++ )
					{
						if( !vol.Occupied[ t ] )
						{
							allocated = 1;

							vol.FreeTiles --;
							vol.Occupied[ t ] = 1;

							UINT32 entry = 0;

							MegaTexTile::EncodeMaskEntry( &entry, i, t, maskIdx );

							MegaTexTile::MaskEntryList::Iterator iter = tile->MaskList.GetIterator();

							int inserted = 0;

							for( ; iter.IsValid(); iter.Advance() )
							{
								UINT32 val = iter.GetValue();
								int decVol, decTile, decMask;

								MegaTexTile::DecodeMaskEntry( val, &decVol, &decTile, &decMask );

								if( decMask > maskIdx )
								{
									tile->MaskList.Insert( iter, entry );
									inserted = 1;
									break;
								}
							}

							if( !inserted )
							{
								tile->PushMaskEntry( entry );
							}

							m_MaskTotalAllocatedTiles ++;
							m_MaskMaxAllocatedTiles = R3D_MAX( m_MaskMaxAllocatedTiles, m_MaskTotalAllocatedTiles );
						}
					}
				}
			}

			neededCount ++;
		}
	}
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::FreeMaskAtlasTile( MegaTexTile* tile )
{
#if R3D_TERRAIN_V3_GRAPHICS
	r3d_assert( tile->IsAllocated );
	r3d_assert( tile->IsInAtlas() );

#ifdef _DEBUG
#if 0
	for( int i = 0, e = m_TilesToUpdate.Count(); i < e; i ++ )
	{
		r3d_assert( tile != m_TilesToUpdate[ i ] );
	}
#endif
#endif

	// 'damage control' in final build
	if( tile->IsInAtlas() )
	{
		MegaTexTile::MaskEntryList::Iterator iter = tile->MaskList.GetIterator();

		for( ; iter.IsValid(); iter.Advance() )
		{
			int volId, tileId, maskId;

			MegaTexTile::DecodeMaskEntry( iter.GetValue(), &volId, &tileId, &maskId );

			m_MaskAtlas[ volId ].Occupied[ tileId ] = 0;
			m_MaskAtlas[ volId ].FreeTiles ++;

			m_MaskTotalAllocatedTiles --;
		}
	}

	tile->ClearMaskList();

#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::CreateHeightNormalTexture(r3dTexture *tex, D3DFORMAT fmt)
{
	if (tex)
	{	
		tex->Create( HEIGHT_NORMAL_ATLAS_TEXTURE_DIM, HEIGHT_NORMAL_ATLAS_TEXTURE_DIM, fmt, 1, D3DPOOL_DEFAULT );
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::AddHeightNormalAtlasVolume()
{
	HeightNormalAtlasVolume volume;

	int mipCount = MipCount( m_QualitySettings.AtlasTileDim ) - 2;
	// we're making 4x4 mip monocolor and ignore the rest, because otherwise we have to waste too
	// much extra space to allow smooth tile to tile transition

	mipCount = 1;

	volume.Height = r3dRenderer->AllocateTexture();
	CreateHeightNormalTexture(volume.Height, TERRA3_MEGAHEIGHT_FORMAT);

	volume.Normal = r3dRenderer->AllocateTexture();
	CreateHeightNormalTexture(volume.Normal, TERRA3_MEGANORMAL_FORMAT);

	volume.Occupied.Resize( m_HeightNormalAtlasTileCountPerSide * m_HeightNormalAtlasTileCountPerSide, 0 );

	volume.FreeTiles = m_HeightNormalAtlasTileCountPerSide * m_HeightNormalAtlasTileCountPerSide;

	m_HeightNormalAtlas.PushBack( volume );
}

void r3dTerrain3::FreeHeightNormalAtlas()
{
	for( int i = 0, e = m_HeightNormalAtlas.Count(); i < e; i ++ )
	{
		r3dRenderer->DeleteTexture( m_HeightNormalAtlas[ i ].Normal );
		r3dRenderer->DeleteTexture( m_HeightNormalAtlas[ i ].Height );
	}

	m_HeightNormalAtlas.Clear();
}

void r3dTerrain3::AllocateHeightNormalAtlasTile( MegaTexTile* tile )
{
	int mipOffset = r3dLog2( m_QualitySettings.VertexTilesInMegaTexTileCount );

	for( int i = 0 ; ; i ++ )
	{
		if( i >= (int)m_HeightNormalAtlas.Count() )
		{
			AddHeightNormalAtlasVolume();
		}

		HeightNormalAtlasVolume& vol = m_HeightNormalAtlas[ i ];

		if( !vol.FreeTiles )
			continue;

		for( int t = 0, e = vol.Occupied.Count(); t < e; t ++ )
		{
			if( !vol.Occupied[ t ] )
			{
				vol.FreeTiles --;
				vol.Occupied[ t ] = 1;

				int entry = 0;

				tile->HeightNormalAtlasVolumeId = i;
				tile->HeightNormalAtlasTileId = t;

				m_HeightNormalTotalAllocatedTiles ++;
				m_HeightNormalMaxAllocatedTiles = R3D_MAX( m_HeightNormalMaxAllocatedTiles, m_HeightNormalTotalAllocatedTiles );

				return;
			}
		}
	}
}

void r3dTerrain3::FreeHeightNormalAtlasTile( MegaTexTile* tile )
{
#if R3D_TERRAIN_V3_GRAPHICS
	r3d_assert( tile->IsAllocated );

	// 'damage control' in final build
	if( tile->IsInAtlas() )
	{
		m_HeightNormalAtlas[ tile->HeightNormalAtlasVolumeId ].Occupied[ tile->HeightNormalAtlasTileId ] = 0;
		m_HeightNormalAtlas[ tile->HeightNormalAtlasVolumeId ].FreeTiles ++;

		m_HeightNormalTotalAllocatedTiles --;

		tile->HeightNormalAtlasTileId = -1;
		tile->HeightNormalAtlasVolumeId = -1;
	}
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::AllocateMegaTexTile( MegaTexTile* tile )
{
	AllocateMaskAtlasTile( tile );
	AllocateHeightNormalAtlasTile( tile );
}

//------------------------------------------------------------------------

#ifndef FINAL_BUILD

void r3dTerrain3::AddEditorMaskTexAtlasVolume()
{
#if R3D_TERRAIN_V3_GRAPHICS
	MaskAtlasVolume volume;

	int mipCount = MipCount( m_QualitySettings.AtlasTileDim ) - 2;
	// we're making 4x4 mip monocolor and ignore the rest, because otherwise we have to waste too
	// much extra space to allow smooth tile to tile transition

	mipCount = 1;

	volume.Mask = r3dRenderer->AllocateTexture();
	volume.Mask->Create( MASK_ATLAS_TEXTURE_DIM, MASK_ATLAS_TEXTURE_DIM, D3DFMT_R5G6B5, 1, D3DPOOL_DEFAULT );

	volume.Occupied.Resize( m_MaskAtlasTileCountPerSide * m_MaskAtlasTileCountPerSide, 0 );

	volume.FreeTiles = m_MaskAtlasTileCountPerSide * m_MaskAtlasTileCountPerSide;

	m_EDITOR_ReplacementMaskAtlas.PushBack( volume );
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::FreeEditorMaskTexAtlas()
{
#if R3D_TERRAIN_V3_GRAPHICS
	for( int i = 0, e = m_EDITOR_ReplacementMaskAtlas.Count(); i < e; i ++ )
	{
		r3dRenderer->DeleteTexture( m_EDITOR_ReplacementMaskAtlas[ i ].Mask );
	}

	m_EDITOR_ReplacementMaskAtlas.Clear();
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::AllocateEditorMaskTexTile( INT32* oID, int maskId )
{
#if R3D_TERRAIN_V3_GRAPHICS
	int allocated = 0;

	for( int i = 0 ; !allocated; i ++ )
	{
		if( i >= (int)m_EDITOR_ReplacementMaskAtlas.Count() )
		{
			AddEditorMaskTexAtlasVolume();
		}

		MaskAtlasVolume& vol = m_EDITOR_ReplacementMaskAtlas[ i ];

		if( !vol.FreeTiles )
			continue;

		for( int t = 0, e = vol.Occupied.Count(); t < e && !allocated; t ++ )
		{
			if( !vol.Occupied[ t ] )
			{
				allocated = 1;

				vol.FreeTiles --;
				vol.Occupied[ t ] = 1;

				UINT32 entry = 0;

				MegaTexTile::EncodeMaskEntry( &entry, i, t, maskId );

				*oID = entry;
			}
		}
	}
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::DeallocateAllEditorMaskTexTiles()
{
	for( int i = 0, e = m_EDITOR_ReplacementMaskAtlas.Count(); i != e; i ++ )
	{
		MaskAtlasVolume& vol = m_EDITOR_ReplacementMaskAtlas[ i ];

		memset( &vol.Occupied[ 0 ], 0, vol.Occupied.Count() );

		vol.FreeTiles = vol.Occupied.Count();
	}
}

#endif

//------------------------------------------------------------------------

void r3dTerrain3::FreeMegaTexTile( MegaTexTile* tile )
{
	r3d_assert( tile->IsAllocated && tile->IsInAtlas() );

	FreeMaskAtlasTile( tile );
	FreeHeightNormalAtlasTile( tile );

	tile->IsAllocated = 0;
	tile->IsLoaded = 0;
}

//------------------------------------------------------------------------

void r3dTerrain3::FreeAllMegaTexTiles()
{
	for( int i = 0, e = (int)m_AllocMegaTexTileLodArray.Count(); i < e; i ++ )
	{
		MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ i ];

		for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
		{
			MegaTexTile* mega = arr[ i ];
			FreeMaskAtlasTile( mega );
		}

		arr.Clear();
	}

	for( int L = 0, e = m_NumActiveQualityLayers; L < e; L ++ )
	{
		AllocatedTileArr& arr = m_AllocTileLodArray[ L ];

		for( int i = 0, e = arr.Count(); i < e; i ++ )
		{
			arr[ i ].MegaTile = NULL;
		}
	}
}

//------------------------------------------------------------------------

int2
r3dTerrain3::GetTileAtlasXZ( const AllocatedTile* tile ) const
{
	int idInAtlas = tile->AtlasTileID;

	int x = idInAtlas % m_AtlasTileCountPerSide;
	int z = idInAtlas / m_AtlasTileCountPerSide;

	return int2( x, z );
}

//------------------------------------------------------------------------

int2 r3dTerrain3::GetMaskTileAtlasXZ( int tileId ) const
{
	int x = tileId % m_MaskAtlasTileCountPerSide;
	int z = tileId / m_MaskAtlasTileCountPerSide;

	return int2( x, z );
}

//------------------------------------------------------------------------

int2 r3dTerrain3::GetHeightNormalAtlasXZ( int tileId ) const
{
	int x = tileId % m_HeightNormalAtlasTileCountPerSide;
	int z = tileId / m_HeightNormalAtlasTileCountPerSide;

	return int2( x, z );
}

//------------------------------------------------------------------------

void r3dTerrain3::SetTileGeomVDecl( bool forAtlas )
{
	if( !forAtlas && !m_AllowVFetch )
		r3dRenderer->SetVertexDecl( g_TerraDynaVDecl );
	else
		r3dRenderer->SetVertexDecl( g_TerraVDecl );
}

//------------------------------------------------------------------------

void r3dTerrain3::SetTileGeomVDeclNoVFetch()
{
	r3dRenderer->SetVertexDecl( g_TerraDynaVDecl );
}

//------------------------------------------------------------------------

void
r3dTerrain3::StartUsingTileGeom( bool forAtlas )
{
#if R3D_TERRAIN_V3_GRAPHICS
	m_TileVertexBuffer->Set( 0 );
	m_TileIndexBuffer->Set();

	SetTileGeomVDecl( forAtlas );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::StopUsingTileGeom()
{
#if R3D_TERRAIN_V3_GRAPHICS
	r3dRenderer->SetVertexDecl( R3D_MESH_VERTEX::getDecl() );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::SetupTileAtlasRTs( const AllocatedTile* tile )
{
#if R3D_TERRAIN_V3_GRAPHICS
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
r3dTerrain3::StartTileUpdating()
{
#if R3D_TERRAIN_V3_GRAPHICS
	StartUsingTileGeom( true );

	r3dRenderer->SetMipMapBias( 0.f );

	r3dRenderer->SetCullMode( D3DCULL_NONE );

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ );

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE ) );
	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) );

	//------------------------------------------------------------------------
	// sampler sColor                 : register ( s11 );
	{
		r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_WHITE], 11 );
	}

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
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP ) );

	r3dSetFiltering( R3D_POINT, D3DVERTEXTEXTURESAMPLER0 );

	m_LastDiffuseRT = NULL;
	m_LastNormalRT = NULL;
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::SetAtlasTilePosTransform( const AllocatedTile* tile, float4* oConst )
{
#if R3D_TERRAIN_V3_GRAPHICS
	int2 atlasXZ = GetTileAtlasXZ( tile );

	oConst->x = m_AtlasTileCountPerSideInv;
	oConst->y = m_AtlasTileCountPerSideInv;
	oConst->z = ( atlasXZ.x + 0.5f ) * m_AtlasTileCountPerSideInv * 2.0f - 1.0f;
	oConst->w = ( atlasXZ.y + 0.5f ) * m_AtlasTileCountPerSideInv * 2.0f - 1.0f;
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::SetTileUpdateVSConsts_G( const AllocatedTile* tile, const MegaTexTile* megaTexTile, int passN, const IntArr& layerIndexes )
{
#if R3D_TERRAIN_V3_GRAPHICS
	r3dTL::TFixedArray< float4, 9 > vsConsts;

	float extraSpaceCoef = GetPaddingCoef();

	int L = tile->L;

	struct
	{
		void operator() ( float4* tarConst, const r3dTerrain3Layer* layer, const AllocatedTile* tile, float tileDim, float extraSpaceCoef )
		{
			float multU = layer->ShaderScaleU * tileDim;
			float multV = layer->ShaderScaleV * tileDim;

			tarConst->x = multU * extraSpaceCoef;
			tarConst->y = -multV * extraSpaceCoef;
			tarConst->z = multU + tile->X * tileDim * layer->ShaderScaleU * 2.0f;
			tarConst->w = -multV - tile->Z * tileDim * layer->ShaderScaleV * 2.0f;
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
	// float4 gToBaseTileTexc      : register( c1 );
	{
		if( !passN )
		{
			setLayerConsts( vsConsts + 1, &m_BaseLayer, tile, m_TileWorldDims[ L ], extraSpaceCoef );
		}
		else
		{
			setUnusedConst( vsConsts + 1 );
		}
	}

	//------------------------------------------------------------------------

	int startLayer = passN * LAYERS_PER_MASK;
	int layerCount = R3D_MIN( int( layerIndexes.Count() - startLayer ), (int)LAYERS_PER_MASK );

	//------------------------------------------------------------------------
	// float4 gToTile0Texc         : register( c2 );
	// float4 gToTile1Texc         : register( c3 );
	// float4 gToTile2Texc         : register( c4 );

	for( int i = startLayer, c = 0, e = startLayer + layerCount; i < e; i ++, c ++ )
	{
		const r3dTerrain3Layer* layer = &m_Layers[ layerIndexes[ i ] ];
		setLayerConsts( vsConsts + 2 + c, layer, tile, m_TileWorldDims[ L ], extraSpaceCoef );
	}

	for( int c = layerCount, e = 3; c < e; c ++ )
	{
		setUnusedConst( vsConsts + 2 + c );
	}

	//------------------------------------------------------------------------
	// float4 gToSplatTexc0        : register( c5 );
	// float4 gToSplatTexc1        : register( c6 );
	// float4 gToSplatTexc2        : register( c7 );
	{
		int prevMaskIndex = -1;
		int curTileId = -1;

		int first = 1;

		for( int i = 0, c = 0, e = startLayer + layerCount; i < e; i ++ )
		{
			int layerIndex = layerIndexes[ i ];
			int maskIndex = layerIndex / LAYERS_PER_MASK;

			if( maskIndex != prevMaskIndex )
			{
				first = 0;

				int volId, tileId, maskId;
#ifndef FINAL_BUILD
				int needReplace = 0;

				if( !m_EDITOR_ReplacementTileIndexes.empty() )
				{
					INT64 id = megaTexTile->GetID( maskIndex );

					EditorReplacementMaskTiles::iterator iter = m_EDITOR_ReplacementTileIndexes.find( id );

					if( iter != m_EDITOR_ReplacementTileIndexes.end() )
					{
						MegaTexTile::DecodeMaskEntry( iter->second, &volId, &tileId, &maskId );

						r3d_assert( maskId == maskIndex );

						needReplace = 1;
					}
				}

				if( !needReplace )
#endif
				{
					MegaTexTile::MaskEntryList::Iterator iter = megaTexTile->MaskList.GetIterator();

					int found = 0;

					for( ; iter.IsValid(); iter.Advance() )
					{
						int val = iter.GetValue();

						MegaTexTile::DecodeMaskEntry( val, &volId, &tileId, &maskId );
						
						if( maskId == maskIndex )
						{
							found = 1;
							break;
						}
					}

					r3d_assert( found );
				}

				curTileId = tileId;
				prevMaskIndex = maskIndex;
			}

			if( i >= startLayer )
			{
				float halfTileDim = m_HalfTileWorldDims[ L ];
				float tileDim = m_TileWorldDims[ L ];

				SetMaskAtlasTileTexcTransform( curTileId, tile, megaTexTile, extraSpaceCoef, &vsConsts[ c + 5 ] );
				c ++;
			}
		}
	}

	//------------------------------------------------------------------------
	// float4 gToNormaTexc         : register( c8 );
	SetNormalAtlasTileTexcTransform( megaTexTile, tile, &vsConsts[ 8 ] );

	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, &vsConsts[0].x, vsConsts.COUNT ) );
#endif

}

//------------------------------------------------------------------------

void
r3dTerrain3::SetTileUpdatePSConsts( const AllocatedTile* tile, int startLayer, const IntArr& layerIndexes )
{
#if R3D_TERRAIN_V3_GRAPHICS
	int layerCount = layerIndexes.Count() - startLayer;

	layerCount = R3D_MIN( layerCount , (int)LAYERS_PER_MASK );

	if( layerCount )
	{
		r3dTL::TFixedArray< float4, LAYERS_PER_MASK + 1 > psConsts;

		float* multPtr = &psConsts[ LAYERS_PER_MASK ].x;

		// float4 gSplatControls_specPows[ MAX_LAYERS ] : register( c1 );
		// float4 gNormalTileMultis : register( c4 );
		for( int i = 0, e = layerCount; i < e; i ++ )
		{
			int layerIndex = layerIndexes[ startLayer + i ];

			psConsts[ i ] = m_Layers[ layerIndex ].ChannelMask;
			psConsts[ i ].w = m_Layers[ layerIndex ].SpecularPow;

			multPtr[ i ] = m_Layers[ layerIndex ].NormUVMult;
		}

		r3dRenderer->pd3ddev->SetPixelShaderConstantF( 1, &psConsts[ 0 ].x, LAYERS_PER_MASK + 1 );
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::FillTempTileLayerArray( const AllocatedTile* tile )
{
	MegaTexTile* megaTile = tile->MegaTile;

	r3d_assert( megaTile );

	int lodOffset = m_MegaTexTileLodOffset;

	int megaLod = megaTile->L + lodOffset;

	int lodDistance = megaLod - tile->L;

	int maxLodDistance = r3dLog2( m_QualitySettings.MaskAtlasTileDim ) + 1;
	
	m_TempLayerIndexes.Clear();

	int relX = tile->X - ( megaTile->X << lodDistance );
	int relZ = tile->Z - ( megaTile->Z << lodDistance );

	if( lodDistance > maxLodDistance - 1 )
	{
		int maxDiff = lodDistance - maxLodDistance + 1;

		relX >>= maxDiff;
		relZ >>= maxDiff;

		lodDistance = maxLodDistance - 1;
	}

	int bitLod = maxLodDistance - lodDistance - 1;

	for( int i = 0, e = megaTile->ActiveBitMasks; i < e; i ++ )
	{
		MegaTexTile::LayerBitMaskEntry& entry = megaTile->LayerBitMasks[ i ];

		if( entry.Bits[ bitLod ].Get( relX, relZ ) )
		{
#ifndef FINAL_BUILD
			// tile may be refreshing in background and have -1 here
			if( entry.LayerIndex >= 0 )
#endif
			m_TempLayerIndexes.PushBack( entry.LayerIndex );
		}
	}
}
//------------------------------------------------------------------------

void r3dTerrain3::UpdateTileInAtlas_G( AllocatedTile* tile )
{
#if R3D_TERRAIN_V3_GRAPHICS

	if( !tile->MegaTile )
	{
		r3dOutToLog( "r3dTerrain3::UpdateTileInAtlas_G: couldn't find mega tex tile for tile (%d,%d,%d)\n", tile->X, tile->Z, tile->L );
		return;
	}

	MegaTexTile* megaTexTile = tile->MegaTile;

	int qlMultiplier = 1 << tile->L;

	FillTempTileLayerArray( tile );

	int totalLayerCount = (int)m_TempLayerIndexes.Count();

	int passCount = totalLayerCount / LAYERS_PER_MASK;

	if( totalLayerCount % LAYERS_PER_MASK )
		passCount ++;

	if( !passCount )
		passCount = 1;

	int startLayer = 0;

	SetupTileAtlasRTs( tile );

	int prevMaskIndex = 0;
	
	if( m_TempLayerIndexes.Count() )
	{
		prevMaskIndex = m_TempLayerIndexes [ 0 ] / LAYERS_PER_MASK;
	}

	// float4 gBaseSpecPow_NormalMult : register( c0 );
	float4 BaseSpecPow = float4( m_BaseLayer.SpecularPow, m_BaseLayer.NormUVMult, 0.f, 0.f );
	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, &BaseSpecPow.x, 1 ) );

	for( int i = 0, e = passCount; i < e; i ++, startLayer += LAYERS_PER_MASK )
	{
		int final = i == passCount - 1;

		int layerCount = R3D_MIN( (int)( totalLayerCount - LAYERS_PER_MASK * i ), (int)LAYERS_PER_MASK );

		SetTileUpdateVSConsts_G( tile, megaTexTile, i, m_TempLayerIndexes );

		// vertex shader
		{
			GenerateAtlas3VertexShaderId vsid;
			vsid.unused = 0;

			r3dRenderer->SetValidVertexShader( g_AtlasVertexShaderIdMap_G[ vsid.Id ] );
		}

		SetTileUpdatePSConsts( tile, startLayer, m_TempLayerIndexes );

		// pixel shader
		{
			GenerateAtlas3PixelShaderId psid;
			psid.numLayers = layerCount;
			psid.firstBatch = i == 0;

			int id = g_AtlasPixelShaderIdMap_G[ psid.Id ];

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
				const r3dTerrain3Layer* layer = &m_Layers[ m_TempLayerIndexes [ i ] ];

				r3dRenderer->SetTex( layer->DiffuseTex, 2 + s );
				r3dRenderer->SetTex( layer->NormalTex, 5 + s );
			}
		}

		//------------------------------------------------------------------------
		// sampler sSplat[ MAX_LAYERS ]   : register ( s8 );
		{
			for( int i = startLayer, s = 0, e = startLayer + layerCount; i < e; i ++, s ++ )
			{
				int layerIndex = m_TempLayerIndexes [ i ];
				int maskIndex = layerIndex / LAYERS_PER_MASK;

				if( maskIndex != prevMaskIndex )
				{
					prevMaskIndex = maskIndex;
				}

				int atlasId, tileId, maskId;

#ifndef FINAL_BUILD
				int needReplace = 0;

				if( !m_EDITOR_ReplacementTileIndexes.empty() )
				{					
					INT64 id = megaTexTile->GetID( maskIndex );

					EditorReplacementMaskTiles::iterator iter = m_EDITOR_ReplacementTileIndexes.find( id );

					if( iter != m_EDITOR_ReplacementTileIndexes.end() )
					{
						MegaTexTile::DecodeMaskEntry( iter->second, &atlasId, &tileId, &maskId );

						r3d_assert( maskId == maskIndex );

						needReplace = 1;
					}
				}

				if( !needReplace )
#endif
				{
					MegaTexTile::MaskEntryList::Iterator maskIter = megaTexTile->MaskList.GetIterator();

					int found = 0;

					int elem = 0;

					for( ; maskIter.IsValid();  maskIter.Advance() )
					{
						MegaTexTile::DecodeMaskEntry( maskIter.GetValue(), &atlasId, &tileId, &maskId );

						if( maskId == maskIndex )
						{
							found = 1;
							break;
						}
					}

					r3d_assert( found );

					r3dRenderer->SetTex( m_MaskAtlas[ atlasId ].Mask, 8 + s );
				}

#ifndef FINAL_BUILD
				else
				{
					r3dRenderer->SetTex( m_EDITOR_ReplacementMaskAtlas[ atlasId ].Mask, 8 + s );
				}
#endif
			}
		}

		//------------------------------------------------------------------------
		// sampler sVertexNormal          : register ( s12 );
		{
			r3dTexture* vertexNormal = m_HeightNormalAtlas[ megaTexTile->HeightNormalAtlasVolumeId ].Normal;

			r3dRenderer->SetTex( vertexNormal, 12 );
			r3dRenderer->SetTex( vertexNormal, D3DVERTEXTEXTURESAMPLER0 );
		}

		if (r_show_winter->GetBool())
		{
			r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_WINTER_DIFFUSE], 13);
			r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_WINTER_NORMAL], 14);
			r3dRenderer->SetTex( __r3dShadeTexture[SHADETEXT_WINTER_SPECULAR], 15);
		}

		int density = m_QualitySettings.TileVertexDensities[ tile->L ];

		int primitiveCount = m_ConnectionIndexCounts[ density ][ 0 ];
		int indexOffset = m_ConnectionIndexOffsets[ density ][ 0 ];
		int vertexOffset = m_ConnectionVertexOffsets[ density ];

#ifndef FINAL_BUILD
		r3dRenderer->Stats.AddNumTerrainDraws( 1 );
		r3dRenderer->Stats.AddNumTerrainTris( primitiveCount );
#endif

		r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, vertexOffset, 0, m_TileVertexBuffer->GetItemCount(), indexOffset, primitiveCount );
	}

	// modulate
#if 0
	{
		GenerateAtlas3PixelShaderId psid;

		psid.Id = 0;
		psid.modulationBatch = 1;

		int id = g_AtlasPixelShaderIdMap_G[ psid.Id ];

		r3d_assert( id >= 0 );

		r3dRenderer->SetPixelShader( id );

		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ) );

		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO ) );
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR ) );

#ifndef FINAL_BUILD
		r3dRenderer->Stats.AddNumTerrainDraws( 1 );
		r3dRenderer->Stats.AddNumTerrainTris( 2 );
#endif
		r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, 0, 0, ( m_QualitySettings.VertexTileDim + 1 ) * ( m_QualitySettings.VertexTileDim + 1 ), m_4VertTileIndexOffset, 2 );
	}
#endif

	tile->Dirty = 0;
#endif
}

//------------------------------------------------------------------------

R3D_FORCEINLINE int r3dTerrain3::IsMegaTileOnBorder( MegaTexTile* megaTile, AllocatedTile* allocTile ) const
{
	int lodDistance = megaTile->L + m_MegaTexTileLodOffset - allocTile->L;

	if( lodDistance >= 0 )
	{
		int relX = megaTile->X << lodDistance;
		int relZ = megaTile->Z << lodDistance;

		if( allocTile->X + 1 == relX 
			||
			allocTile->Z + 1 == relZ )
			return 1;
	}
	else
	{
		int tileX = allocTile->X << -lodDistance;
		int tileZ = allocTile->Z << -lodDistance;

		int step = 1 << -lodDistance;

		if( tileX + step == megaTile->X 
				||
			tileZ + step == megaTile->Z )
			return 1;
	}

	return 0;
}

//------------------------------------------------------------------------

R3D_FORCEINLINE DWORD ilog2( DWORD val )
{
	if( val <= 1 )
		return 0;

	int res = 0;

	while( val /= 2 )
	{
		res ++;
	}

	return res;
}

template< r3dTerrain3::Side side, int IS_FULL >
void r3dTerrain3::FillAdjecantTileVertices( AllocatedTile* atile, int sideCount, int innerCount )
{
	const QualitySettings& qs = GetCurrentQualitySettings();

	m_TempMegaTexTileArr.Clear();
	m_TempMegaTexTile_NonAdjArr.Clear();

	MegaTexTile* megaTile = atile->MegaTile;

	if( !megaTile )
		return;

	int megaMultiply = 1 << ( megaTile->L + m_MegaTexTileLodOffset - atile->L );

	int megaRelX = atile->X - megaTile->X * megaMultiply;
	int megaRelZ = atile->Z - megaTile->Z * megaMultiply;

	int XPosZPosCorner = megaRelX == megaMultiply - 1 && megaRelZ == megaMultiply - 1 && IS_FULL;

	if( XPosZPosCorner )
		atile->IsCorner = 1;

	if( !atile->L && qs.VertexTileDim == 1 && !IS_FULL )
	{
		if( side == SIDE_ZPOS )
		{
			atile->MegaConFlags |= SOUTH_CONNECTION;
		}
		return;
	}

	int heightNormLog2 = ilog2( qs.HeightNormalAtlasTileDim );

	MegaTexTile* megaXPosZPos = NULL;
	MegaTexTile* megaXPosZPosForInner = NULL;

	int megaXPosZPos_IsAdj = 0;
	int megaXPosZPosForInner_IsAdj = 0;

	int mtx = megaTile->X << megaTile->L;
	int mtz = megaTile->Z << megaTile->L;

	int tileAbsX = atile->X << atile->L;
	int tileAbsZ = atile->Z << atile->L;
	int tileScale = 1 << atile->L;

	int mts = 1 << megaTile->L;

	for( int L = R3D_MAX( atile->L - 1, 0 ), e = atile->L; L <= e; L ++ )
	{
		AllocatedTileArr& arr = m_AllocTileLodArray[ L ];

		int lodMult = 1 << ( e - L );

		int relX = atile->X * lodMult;
		int relZ = atile->Z * lodMult;

		for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
		{
			AllocatedTile& at = arr[ i ];

			if( !at.Tagged || !at.MegaTile || at.MegaTile == atile->MegaTile )
				continue;

			if( !megaXPosZPos )
			{
				if( at.X == relX + lodMult && at.Z == relZ + lodMult )
				{
					if( at.MegaTile )
					{
						megaXPosZPos = at.MegaTile;

						if( IsMegaTileOnBorder( at.MegaTile, atile ) )
						{
							megaXPosZPos_IsAdj = 1;
						}
						else
						{
							megaXPosZPos_IsAdj = 0;
						}
					}
				}
			}

			if( XPosZPosCorner )
			{
				if( !megaXPosZPosForInner )
				{
					if( side == SIDE_XPOS )
					{
						if( at.X - 1 % lodMult == atile->X * lodMult && at.Z == relZ + lodMult )
						{
							if( at.MegaTile )
							{
								megaXPosZPosForInner = at.MegaTile;

								if( IsMegaTileOnBorder( at.MegaTile, atile ) )
								{
									megaXPosZPosForInner_IsAdj = 1;
								}
								else
								{
									megaXPosZPosForInner_IsAdj = 0;
								}
							}
						}
					}
					else
					{
						if( at.Z - 1 % lodMult == atile->Z * lodMult && at.X == relX + lodMult )
						{
							if( at.MegaTile )
							{
								megaXPosZPosForInner = at.MegaTile;

								if( IsMegaTileOnBorder( at.MegaTile, atile ) )
								{
									megaXPosZPosForInner_IsAdj = 1;
								}
								else
								{
									megaXPosZPosForInner_IsAdj = 0;
								}
							}
						}
					}
				}
			}

			int conditionIsTrue = 0;

			switch( side )
			{
			case SIDE_XPOS:
				conditionIsTrue = at.X == relX + lodMult && at.Z / lodMult == atile->Z;
				break;
			case SIDE_ZPOS:
				conditionIsTrue = at.Z == relZ + lodMult && at.X / lodMult == atile->X;
				break;
			}

			if( conditionIsTrue )
			{
				if( MegaTexTile* candidateMegaTile = at.MegaTile )
				{
					int megaTileIsAdjecant = 0;

					int cmtx = candidateMegaTile->X << ( candidateMegaTile->L + m_MegaTexTileLodOffset );
					int cmtz = candidateMegaTile->Z << ( candidateMegaTile->L + m_MegaTexTileLodOffset );

					switch( side )
					{
					case SIDE_XPOS:
						if( tileAbsX + tileScale == cmtx )
							megaTileIsAdjecant = 1;
						break;
					case SIDE_ZPOS:
						if( tileAbsZ + tileScale == cmtz )
							megaTileIsAdjecant = 1;
						break;
					}

					if( megaTileIsAdjecant )
					{
						int found = 0;

						for( int i = 0, e = m_TempMegaTexTileArr.Count(); i < e; i ++ )
						{
							if( m_TempMegaTexTileArr[ i ] == candidateMegaTile )
							{
								found = 1;
								break;
							}
						}

						if( !found )
						{
							m_TempMegaTexTileArr.PushBack( candidateMegaTile );
						}
					}
					else
					{
						int found = 0;

						for( int i = 0, e = m_TempMegaTexTile_NonAdjArr.Count(); i < e; i ++ )
						{
							if( m_TempMegaTexTile_NonAdjArr[ i ] == candidateMegaTile )
							{
								found = 1;
								break;
							}
						}

						if( !found )
						{
							m_TempMegaTexTile_NonAdjArr.PushBack( candidateMegaTile );
						}
					}
				}
			}
		}
	}

	if( ( !m_TempMegaTexTileArr.Count() || ( !megaXPosZPos || ( XPosZPosCorner && !megaXPosZPosForInner ) ) ) && atile->L < m_NumActiveQualityLayers - 1 )
	{
		int needNormalMega = !m_TempMegaTexTileArr.Count();

		AllocatedTileArr& arr = m_AllocTileLodArray[ atile->L + 1 ];

		int relX = atile->X;
		int relZ = atile->Z;

		for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
		{
			AllocatedTile& at = arr[ i ];

			if( !at.Tagged || !at.MegaTile || at.MegaTile == atile->MegaTile )
				continue;

			if( !megaXPosZPos )
			{
				if( (	at.X * 2 == relX + 1 && ( at.Z * 2 == relZ || at.Z * 2 == relZ + 1 ) )
							||
					(	at.Z * 2 == relZ + 1 && ( at.X * 2 == relX || at.X * 2 == relX + 1 ) )
					)
				{
					if( at.MegaTile )
					{
						megaXPosZPos = at.MegaTile;

						if( IsMegaTileOnBorder( at.MegaTile, atile ) )
						{
							megaXPosZPos_IsAdj = 1;
						}
						else
						{
							megaXPosZPos_IsAdj = 0;
						}
					}
				}
			}

			if( XPosZPosCorner )
			{
				if( !megaXPosZPosForInner )
				{
					if( side == SIDE_XPOS )
					{
						if( at.Z * 2 == relZ + 1 && at.X == atile->X / 2 )
						{
							if( at.MegaTile )
							{
								megaXPosZPosForInner = at.MegaTile;

								if( IsMegaTileOnBorder( at.MegaTile, atile ) )
								{
									megaXPosZPosForInner_IsAdj = 1;
								}
								else
								{
									megaXPosZPosForInner_IsAdj = 0;
								}
							}
						}
					}
					else
					{
						if( at.X * 2 == relX + 1 && at.Z == atile->Z / 2 )
						{
							if( at.MegaTile )
							{
								megaXPosZPosForInner = at.MegaTile;

								if( IsMegaTileOnBorder( at.MegaTile, atile ) )
								{
									megaXPosZPosForInner_IsAdj = 1;
								}
								else
								{
									megaXPosZPosForInner_IsAdj = 0;
								}
							}
						}
					}
				}
			}

			if( needNormalMega )
			{
				int conditionIsTrue = 0;

				switch( side )
				{
				case SIDE_XPOS:
					conditionIsTrue = at.X * 2 == relX + 1 && at.Z == atile->Z / 2;
					break;
				case SIDE_ZPOS:
					conditionIsTrue = at.Z * 2 == relZ + 1 && at.X == atile->X / 2;
					break;
				}

				if( conditionIsTrue )
				{
					if( MegaTexTile* candidateMegaTile = at.MegaTile )
					{
						int megaTileIsAdjecant = 0;

						int cmtx = candidateMegaTile->X << candidateMegaTile->L;
						int cmtz = candidateMegaTile->Z << candidateMegaTile->L;

						switch( side )
						{
						case SIDE_XPOS:
							if( mtx + mts == cmtx )
								megaTileIsAdjecant = 1;
							break;
						case SIDE_ZPOS:
							if( mtz + mts == cmtz )
								megaTileIsAdjecant = 1;
							break;
						}

						if( megaTileIsAdjecant )
						{
							int found = 0;

							for( int i = 0, e = m_TempMegaTexTileArr.Count(); i < e; i ++ )
							{
								if( m_TempMegaTexTileArr[ i ] == candidateMegaTile )
								{
									found = 1;
									break;
								}
							}

							if( !found )
							{
								m_TempMegaTexTileArr.PushBack( candidateMegaTile );
							}
						}
						else
						{
							int found = 0;

							for( int i = 0, e = m_TempMegaTexTile_NonAdjArr.Count(); i < e; i ++ )
							{
								if( m_TempMegaTexTile_NonAdjArr[ i ] == candidateMegaTile )
								{
									found = 1;
									break;
								}
							}

							if( !found )
							{
								m_TempMegaTexTile_NonAdjArr.PushBack( candidateMegaTile );
							}
						}
					}
				}
			}
		}
	}

	int startAdjOffset = atile->AdjecantHeights.Count();
	int megaSideCount = qs.HeightNormalAtlasTileDim;

	switch( side )
	{
	case SIDE_XPOS:
		atile->MegaConFlags |= EAST_CONNECTION;
		break;
	case SIDE_ZPOS:
		atile->MegaConFlags |= SOUTH_CONNECTION;
		break;
	}

	r3d_assert( atile );
	int totalCount = sideCount + innerCount;

	if( IS_FULL )
		totalCount += 2;

	if( sideCount > innerCount )
		totalCount ++ ;

	atile->AdjecantHeights.Resize( startAdjOffset + totalCount );

	int outerCornerInserted = 0;

	if( m_TempMegaTexTileArr.Count() || m_TempMegaTexTile_NonAdjArr.Count() )
	{
		struct
		{
			bool operator ()( MegaTexTile* t0, MegaTexTile* t1 )
			{
				return t0->L > t1->L;
			}
		} sortOp;

		m_TempCharArr.Resize( IS_FULL ? sideCount + 1 : sideCount );
		memset( &m_TempCharArr[ 0 ], 0, m_TempCharArr.Count() );

		if( m_TempMegaTexTileArr.Count() )
		{
			std::sort( &m_TempMegaTexTileArr[ 0 ], &m_TempMegaTexTileArr[ 0 ] +  m_TempMegaTexTileArr.Count(), sortOp );

			for( int i = 0, e = m_TempMegaTexTileArr.Count(); i < e; i ++ )
			{
				MegaTexTile* adjMegaTile = m_TempMegaTexTileArr[ i ];

				int megaSideOffset = side * megaSideCount;

				int lodDistance = adjMegaTile->L + m_MegaTexTileLodOffset - atile->L;

				if( lodDistance >= 0 )
				{
					int pixelsPerTile = qs.HeightNormalAtlasTileDim >> lodDistance;

					int pixelsPerTileMul = R3D_MAX( pixelsPerTile, 1 );
					int pixelsPerTileDiv = 1 << R3D_MAX( lodDistance - heightNormLog2, 0 );

					int relCoord = 0;
					int adjMegaOffset = 0;

					switch( side )
					{
					case SIDE_XPOS:
						relCoord = atile->Z - ( adjMegaTile->Z << lodDistance );
						adjMegaOffset = 0;
						break;
					case SIDE_ZPOS:
						relCoord = atile->X - ( adjMegaTile->X << lodDistance );
						adjMegaOffset = 2 * megaSideCount;
						break;
					}

					int subPixelOffset = 0;

					if( !pixelsPerTile )
					{
						subPixelOffset = relCoord % pixelsPerTileDiv == pixelsPerTileDiv - 1;
					}

					int borderOffset = relCoord * qs.HeightNormalAtlasTileDim >> lodDistance;

					for( int i = 0, e = sideCount; i < e; i ++ )
					{
						atile->AdjecantHeights[ startAdjOffset + i ] = adjMegaTile->BorderData[ i * pixelsPerTileMul / pixelsPerTileDiv / sideCount + borderOffset + adjMegaOffset ];
						m_TempCharArr[ i ] = 1;
					}

					if( IS_FULL )
					{
						if( pixelsPerTile + borderOffset + subPixelOffset < megaSideCount )
						{
							megaXPosZPos = NULL;
							outerCornerInserted = 1;
							atile->AdjecantHeights[ startAdjOffset + sideCount ] = adjMegaTile->BorderData[ pixelsPerTile + subPixelOffset + borderOffset + adjMegaOffset ];
							startAdjOffset += 1;
							m_TempCharArr[ sideCount ] = 1;
						}
					}
				}
				else
				{
					int relCoord = 0;
					int adjMegaOffset = 0;

					int multiplier = 1 << -lodDistance;

					switch( side )
					{
					case SIDE_XPOS:
						relCoord = adjMegaTile->Z - atile->Z * multiplier;
						adjMegaOffset = 0;
						break;
					case SIDE_ZPOS:
						relCoord = adjMegaTile->X - atile->X * multiplier;
						adjMegaOffset = 2 * megaSideCount;
						break;
					}

					int borderOffset = relCoord * sideCount >> -lodDistance;
					int pixelsPerTile = qs.HeightNormalAtlasTileDim;

					for( int i = 0, e = sideCount >> -lodDistance; i < e; i ++ )
					{
						atile->AdjecantHeights[ startAdjOffset + borderOffset + i ] = adjMegaTile->BorderData[ i * pixelsPerTile / e + adjMegaOffset ];
						m_TempCharArr[ borderOffset + i ] = 1;
					}
				}
			}
		}

		if( m_TempMegaTexTile_NonAdjArr.Count() )
		{
			std::sort( &m_TempMegaTexTile_NonAdjArr[ 0 ], &m_TempMegaTexTile_NonAdjArr[ 0 ] + m_TempMegaTexTile_NonAdjArr.Count(), sortOp );
			for( int i = 0, e = R3D_MIN( (int)m_TempMegaTexTile_NonAdjArr.Count(), 1 ); i < e; i ++ )
			{
				MegaTexTile* mtile = m_TempMegaTexTile_NonAdjArr[ i ];

				int lodDistance = mtile->L + m_MegaTexTileLodOffset - atile->L;

				int pixelsPerTileMul = R3D_MAX( qs.HeightNormalAtlasTileDim >> lodDistance, 1 );
				int pixelsPerTileDiv = 1 << R3D_MAX( lodDistance - heightNormLog2, 0 );

				int relX = atile->X - ( mtile->X << lodDistance );
				int relZ = atile->Z - ( mtile->Z << lodDistance );

				int offsetZ = relZ * pixelsPerTileMul / pixelsPerTileDiv;
				int offsetX = relX * pixelsPerTileMul / pixelsPerTileDiv;

				int pixelsPerTile = pixelsPerTileMul / pixelsPerTileDiv;

				int subPixelOffsetX = 0;
				int subPixelOffsetZ = 0;

				if( !pixelsPerTile )
				{
					subPixelOffsetX = relX % pixelsPerTileDiv == pixelsPerTileDiv - 1;
					subPixelOffsetZ = relZ % pixelsPerTileDiv == pixelsPerTileDiv - 1;
				}

				switch( side )
				{
				case SIDE_XPOS:
					for( int i = 0, e = sideCount; i < e; i ++ )
					{
						if( !m_TempCharArr[ i ] )
						{
							atile->AdjecantHeights[ startAdjOffset + i ] = mtile->HeightData[ ( offsetZ + i * pixelsPerTileMul / pixelsPerTileDiv / e ) * qs.HeightNormalAtlasTileDim + offsetX + pixelsPerTile + subPixelOffsetX ];
							m_TempCharArr[ i ] = 1;
						}
					}
					break;

				case SIDE_ZPOS:
					for( int i = 0, e = sideCount; i < e; i ++ )
					{
						if( !m_TempCharArr[ i ] )
						{
							atile->AdjecantHeights[ startAdjOffset + i ] = mtile->HeightData[ ( offsetX + i * pixelsPerTileMul / pixelsPerTileDiv / e ) + ( offsetZ + pixelsPerTile + subPixelOffsetZ ) * qs.HeightNormalAtlasTileDim ];
							m_TempCharArr[ i ] = 1;
						}
					}
					break;
				}
			}
		}

		if( IS_FULL )
		{
			if( megaXPosZPos )
			{
				if( megaXPosZPos_IsAdj )
				{
					int lodDistance = megaXPosZPos->L + m_MegaTexTileLodOffset - atile->L;

					int relCoord = 0;
					int adjMegaOffset = 0;

					switch( side )
					{
					case SIDE_XPOS:
						relCoord = atile->X - ( megaXPosZPos->X << lodDistance);
						adjMegaOffset = 2 * megaSideCount;
						break;
					case SIDE_ZPOS:
						relCoord = atile->Z - ( megaXPosZPos->Z << lodDistance);
						adjMegaOffset = 0;
						break;
					}

					if( lodDistance <= 0 )
					{
						atile->AdjecantHeights[ startAdjOffset + sideCount ] = megaXPosZPos->BorderData[ adjMegaOffset ];
					}
					else
					{
						if( relCoord < 0 )
						{
							int secondDistance = 0;
							int secondOffset = 0;

							if( side == SIDE_XPOS )
							{
								secondDistance = atile->Z - ( megaXPosZPos->Z << lodDistance );
								secondOffset = 0;
							}
							else
							{
								secondDistance = atile->X - ( megaXPosZPos->X << lodDistance );
								secondOffset = 2 * megaSideCount;
							}

							if( secondDistance >= 0 )
							{
								int subSideCount = megaSideCount >> lodDistance;
								int pixelsPerTile = qs.HeightNormalAtlasTileDim >> lodDistance;

								atile->AdjecantHeights[ startAdjOffset + sideCount ] = megaXPosZPos->BorderData[ secondDistance * subSideCount + secondOffset + pixelsPerTile ];
							}
							else
							{
								atile->AdjecantHeights[ startAdjOffset + sideCount ] = megaXPosZPos->BorderData[ adjMegaOffset ];
							}
						}
						else
						{
							int subSideCount = megaSideCount >> lodDistance;
							int pixelsPerTile = qs.HeightNormalAtlasTileDim >> lodDistance;
							atile->AdjecantHeights[ startAdjOffset + sideCount ] = megaXPosZPos->BorderData[ relCoord * subSideCount + adjMegaOffset + pixelsPerTile ];
						}
					}
				}
				else
				{
					int lodDistance = megaXPosZPos->L + m_MegaTexTileLodOffset - atile->L;

					int pixelsPerTile = qs.HeightNormalAtlasTileDim >> lodDistance;

					int pixelsPerTileMul = R3D_MAX( pixelsPerTile, 1 );
					int pixelsPerTileDiv = 1 << R3D_MAX( lodDistance - heightNormLog2, 0 );

					int relX = atile->X - ( megaXPosZPos->X << lodDistance );
					int relZ = atile->Z - ( megaXPosZPos->Z << lodDistance );

					int offsetZ = relZ * pixelsPerTileMul / pixelsPerTileDiv;
					int offsetX = relX * pixelsPerTileMul / pixelsPerTileDiv;

					int subPixelOffsetX = 0;
					int subPixelOffsetZ = 0;

					if( !pixelsPerTile )
					{
						subPixelOffsetX = relX % pixelsPerTileDiv == pixelsPerTileDiv - 1;
						subPixelOffsetZ = relZ % pixelsPerTileDiv == pixelsPerTileDiv - 1;
					}

					switch( side )
					{
					case SIDE_XPOS:
						atile->AdjecantHeights[ startAdjOffset + sideCount ] = megaXPosZPos->HeightData[ ( offsetZ + pixelsPerTile + subPixelOffsetZ ) * qs.HeightNormalAtlasTileDim + offsetX + pixelsPerTile + subPixelOffsetX ];
						break;

					case SIDE_ZPOS:
						atile->AdjecantHeights[ startAdjOffset + sideCount ] = megaXPosZPos->HeightData[ ( offsetX + pixelsPerTile + subPixelOffsetX ) + ( offsetZ + pixelsPerTile + subPixelOffsetZ ) * qs.HeightNormalAtlasTileDim ];
						break;
					}
				}

				m_TempCharArr[ sideCount ] = 1;

				startAdjOffset += 1;
			}
			else
			{
				if( !outerCornerInserted )
				{
					atile->AdjecantHeights[ startAdjOffset + sideCount ] = atile->AdjecantHeights[ startAdjOffset + sideCount - 1 ];

					m_TempCharArr[ sideCount ] = 1;

					startAdjOffset += 1;
					outerCornerInserted = 1;
				}
			}
		}

		startAdjOffset += sideCount;
	}
	else
	{
		int lodDistance = megaTile->L + m_MegaTexTileLodOffset - atile->L;

		int pixelsPerTile = qs.HeightNormalAtlasTileDim >> lodDistance;

		int relCoord = 0;
		int megaSideOffset = 0;

		switch( side )
		{
		case SIDE_XPOS:
			relCoord = atile->Z - ( megaTile->Z << lodDistance );
			megaSideOffset = megaSideCount;
			break;
		case SIDE_ZPOS:
			relCoord = atile->X - ( megaTile->X << lodDistance );
			megaSideOffset = 3 * megaSideCount;
			break;
		}

		int borderOffset = relCoord * qs.HeightNormalAtlasTileDim >> lodDistance;

		for( int i = 0, e = sideCount; i < e; i ++ )
		{
			atile->AdjecantHeights[ startAdjOffset + i ] = megaTile->BorderData[ i * pixelsPerTile / innerCount + borderOffset + megaSideOffset ];
		}

		if( IS_FULL )
		{
			atile->AdjecantHeights[ startAdjOffset + sideCount ] = atile->AdjecantHeights[ startAdjOffset + sideCount - 1 ];

			startAdjOffset += 1;
		}

		startAdjOffset += sideCount;
	}

	int lodDistance = megaTile->L + m_MegaTexTileLodOffset - atile->L;

	int pixelsPerTile = qs.HeightNormalAtlasTileDim >> lodDistance;

	int pixelsPerTileMul = R3D_MAX( pixelsPerTile, 1 );
	int pixelsPerTileDiv = 1 << R3D_MAX( lodDistance - heightNormLog2, 0 );

	int relCoord = 0;
	int megaSideOffset = 0;

	switch( side )
	{
	case SIDE_XPOS:
		relCoord = atile->Z - ( megaTile->Z << lodDistance );
		megaSideOffset = megaSideCount;
		break;
	case SIDE_ZPOS:
		relCoord = atile->X - ( megaTile->X << lodDistance );
		megaSideOffset = 3 * megaSideCount;
		break;
	}

	int subPixelOffset = 0;

	if( !pixelsPerTile )
	{
		subPixelOffset = relCoord % pixelsPerTileDiv == pixelsPerTileDiv - 1;
	}

	int borderOffset = relCoord * qs.HeightNormalAtlasTileDim >> lodDistance;

	for( int i = 0, e = innerCount; i < e; i ++ )
	{
		atile->AdjecantHeights[ startAdjOffset + i ] = megaTile->BorderData[ i * pixelsPerTile / innerCount + borderOffset + megaSideOffset ];
	}

	if( IS_FULL )
	{
		if( pixelsPerTile + borderOffset + subPixelOffset < megaSideCount )
			atile->AdjecantHeights[ startAdjOffset + innerCount ] = megaTile->BorderData[ pixelsPerTile + subPixelOffset + borderOffset + megaSideOffset ];
		else
		{
			if( megaXPosZPosForInner )
			{
				if( megaXPosZPosForInner_IsAdj )
				{
					int lodDistance = megaXPosZPosForInner->L + m_MegaTexTileLodOffset - atile->L;

					int relCoord = 0;
					int megaSideOffset = 0;

					switch( side )
					{
					case SIDE_XPOS:
						relCoord = atile->X - ( megaXPosZPosForInner->X << lodDistance );
						megaSideOffset = 2 * megaSideCount;
						break;
					case SIDE_ZPOS:
						relCoord = atile->Z - ( megaXPosZPosForInner->Z << lodDistance );
						megaSideOffset = 0 * megaSideCount;
						break;
					}

					if( lodDistance < 0 )
						atile->AdjecantHeights[ startAdjOffset + innerCount ] = megaXPosZPosForInner->BorderData[ megaSideOffset + megaSideCount - ( 1 << -lodDistance ) * megaSideCount / sideCount ];
					else
					{
						int pixelsPerTile = qs.HeightNormalAtlasTileDim >> lodDistance;
						atile->AdjecantHeights[ startAdjOffset + innerCount ] = megaXPosZPosForInner->BorderData[ megaSideOffset + ( relCoord * megaSideCount >> lodDistance ) + pixelsPerTile * ( innerCount - 1 ) / innerCount ];
					}
				}
				else
				{

					int lodDistance = megaXPosZPosForInner->L + m_MegaTexTileLodOffset - atile->L;

					int pixelsPerTile = qs.HeightNormalAtlasTileDim >> lodDistance;

					int pixelsPerTileMul = R3D_MAX( pixelsPerTile, 1 );
					int pixelsPerTileDiv = 1 << R3D_MAX( lodDistance - heightNormLog2, 0 );

					int relX = atile->X - ( megaXPosZPosForInner->X << lodDistance );
					int relZ = atile->Z - ( megaXPosZPosForInner->Z << lodDistance );

					int subPixelOffsetX = 0;
					int subPixelOffsetZ = 0;

					if( !pixelsPerTile )
					{
						subPixelOffsetX = relX % pixelsPerTileDiv == pixelsPerTileDiv - 1;
						subPixelOffsetZ = relZ % pixelsPerTileDiv == pixelsPerTileDiv - 1;
					}

					int offsetZ = relZ * pixelsPerTileMul / pixelsPerTileDiv;
					int offsetX = relX * pixelsPerTileMul / pixelsPerTileDiv;

					switch( side )
					{
					case SIDE_XPOS:
						if( lodDistance < 0 )
							atile->AdjecantHeights[ startAdjOffset + innerCount ] = megaXPosZPosForInner->HeightData[ ( offsetZ + 0 ) * qs.HeightNormalAtlasTileDim + offsetX + megaSideCount - ( 1 << -lodDistance ) * megaSideCount / sideCount ];
						else
							atile->AdjecantHeights[ startAdjOffset + innerCount ] = megaXPosZPosForInner->HeightData[ ( offsetZ + pixelsPerTile + subPixelOffsetZ ) * qs.HeightNormalAtlasTileDim + offsetX + pixelsPerTileMul / pixelsPerTileDiv * ( innerCount - 1 ) / innerCount ];
						break;

					case SIDE_ZPOS:
						if( lodDistance < 0 )
							atile->AdjecantHeights[ startAdjOffset + innerCount ] = megaXPosZPosForInner->HeightData[ ( offsetX + 0 ) + ( offsetZ + megaSideCount - ( 1 << -lodDistance ) * megaSideCount / sideCount ) * qs.HeightNormalAtlasTileDim ];
						else
							atile->AdjecantHeights[ startAdjOffset + innerCount ] = megaXPosZPosForInner->HeightData[ ( offsetX + pixelsPerTile + subPixelOffsetX ) + ( offsetZ + pixelsPerTileMul / pixelsPerTileDiv * ( innerCount - 1 ) / innerCount ) * qs.HeightNormalAtlasTileDim ];
						break;
					}
				}
			}
			else
			{
				atile->AdjecantHeights[ startAdjOffset + innerCount ] = atile->AdjecantHeights[ startAdjOffset + innerCount - 1 ];
			}
		}
	}
}

//------------------------------------------------------------------------

void
r3dTerrain3::UpdateAdjecantHeights( AllocatedTile* tile )
{
	if( !tile->MegaTile )
		return;

	int tileSideVertexCount = m_QualitySettings.VertexTileDim << m_QualitySettings.TileVertexDensities[ tile->L ];

	int vertCountAtXNeg = tileSideVertexCount;
	int vertCountAtXPos = tileSideVertexCount;
	int vertCountAtZNeg = tileSideVertexCount;
	int vertCountAtZPos = tileSideVertexCount;

	if( tile->ConFlags & WEST_CONNECTION ) vertCountAtXNeg *= 2;
	if( tile->ConFlags & EAST_CONNECTION ) vertCountAtXPos *= 2;
	if( tile->ConFlags & SOUTH_CONNECTION ) vertCountAtZPos *= 2;
	if( tile->ConFlags & NORTH_CONNECTION ) vertCountAtZNeg *= 2;

	int lodDistance = tile->MegaTile->L + m_MegaTexTileLodOffset - tile->L;

	int relCoordX = tile->X - ( tile->MegaTile->X << lodDistance );
	int relCoordZ = tile->Z - ( tile->MegaTile->Z << lodDistance );

	tile->MegaConFlags = 0;

	tile->AdjecantHeights.Clear();
	tile->AdjecantPosZOffset = 0;

	int borderCoord = ( 1 << lodDistance ) - 1;

	tile->IsCorner = 0;

	int needX = 0;

	if( relCoordX == borderCoord )
	{
		needX = 1;
	}
	else
	{
		for( int L = R3D_MAX( tile->L - 1, 0 ), e = tile->L; L <= e && !needX; L ++ )
		{
			AllocatedTileArr& arr = m_AllocTileLodArray[ L ];

			int lodMult = 1 << ( e - L );

			int relX = tile->X * lodMult;
			int relZ = tile->Z * lodMult;

			for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
			{
				AllocatedTile& at = arr[ i ];

				if( !at.Tagged || !at.MegaTile || at.MegaTile == tile->MegaTile )
					continue;

				if( at.X == relX + lodMult && at.Z / lodMult == tile->Z )
				{
					needX = 1;
					break;
				}
			}
		}

		if( !needX )
		{
			AllocatedTileArr& arr = m_AllocTileLodArray[ tile->L + 1 ];

			int relX = tile->X;
			int relZ = tile->Z;

			for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
			{
				AllocatedTile& at = arr[ i ];

				if( !at.Tagged || !at.MegaTile || at.MegaTile == tile->MegaTile )
					continue;

				if( at.X * 2 == relX + 1 && at.Z == tile->Z / 2 )
				{
					needX = 1;
					break;
				}
			}
		}
	}

	if( needX )
	{
		FillAdjecantTileVertices< SIDE_XPOS, 1 >( tile, vertCountAtXPos, tileSideVertexCount );
		tile->AdjecantPosZOffset = (short)tile->AdjecantHeights.Count();
	}

	int needZ = 0;

	if( relCoordZ == borderCoord )
	{
		needZ = 1;
	}
	else
	{
		for( int L = R3D_MAX( tile->L - 1, 0 ), e = tile->L; L <= e && !needZ; L ++ )
		{
			AllocatedTileArr& arr = m_AllocTileLodArray[ L ];

			int lodMult = 1 << ( e - L );

			int relX = tile->X * lodMult;
			int relZ = tile->Z * lodMult;

			for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
			{
				AllocatedTile& at = arr[ i ];

				if( !at.Tagged || !at.MegaTile || at.MegaTile == tile->MegaTile )
					continue;

				if( at.Z == relZ + lodMult && at.X / lodMult == tile->X )
				{
					needZ = 1;
					break;
				}
			}
		}

		if( !needZ )
		{
			AllocatedTileArr& arr = m_AllocTileLodArray[ tile->L + 1 ];

			int relX = tile->X;
			int relZ = tile->Z;

			for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
			{
				AllocatedTile& at = arr[ i ];

				if( !at.Tagged || !at.MegaTile || at.MegaTile == tile->MegaTile )
					continue;

				if( at.Z * 2 == relZ + 1 && at.X == tile->X / 2 )
				{
					needZ = 1;
					break;
				}
			}
		}
	}

	if( needZ )
	{
		if( tile->AdjecantPosZOffset )
			FillAdjecantTileVertices< SIDE_ZPOS, 0 >( tile, vertCountAtZPos, tileSideVertexCount );
		else
			FillAdjecantTileVertices< SIDE_ZPOS, 1 >( tile, vertCountAtZPos, tileSideVertexCount );
	}

	tile->AdjecantMin = 0xffff;
	tile->AdjecantMax = 0;

	for( int i = 0, e = (int)tile->AdjecantHeights.Count(); i < e; i ++ )
	{
		tile->AdjecantMin = R3D_MIN( tile->AdjecantHeights[ i ], tile->AdjecantMin );
		tile->AdjecantMax = R3D_MAX( tile->AdjecantHeights[ i ], tile->AdjecantMax );
	}
}

//------------------------------------------------------------------------

void
r3dTerrain3::StopTileUpdating()
{
#if R3D_TERRAIN_V3_GRAPHICS
	StopUsingTileGeom();
	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE ) );
	r3dRenderer->RestoreCullMode();

	FlushAtlasRTs();

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );
#endif
}

//------------------------------------------------------------------------

r3dTerrain3::MegaTexTile* r3dTerrain3::FindCoveringMegaTexTile( const AllocatedTile* tile )
{
	int tileVertexCount = m_QualitySettings.VertexTileDim << tile->L;

	int tileVertexStartX = tileVertexCount * tile->X;
	int tileVertexStartZ = tileVertexCount * tile->Z;

	int tileVertexEndX = tileVertexStartX + tileVertexCount;
	int tileVertexEndZ = tileVertexStartZ + tileVertexCount;

	int minL = 0;

	int baseTexVertexCount = m_QualitySettings.VertexTileDim * m_QualitySettings.VertexTilesInMegaTexTileCount;

	for( ; baseTexVertexCount << minL < tileVertexCount ; minL ++ )
	{

	}

	for( int l = minL, e = (int)m_AllocMegaTexTileLodArray.Count(); l < e; l ++ )
	{
		MegaTexTilePtrArr& texTileLodArr = m_AllocMegaTexTileLodArray[ l ];

		int texTileVertexCount = baseTexVertexCount << l;

		for( int i = 0, e = (int)texTileLodArr.Count(); i < e; i ++ )
		{
			MegaTexTile& tile = *texTileLodArr[ i ];

			if( !tile.IsLoaded )
				continue;

			int vertStartX = tile.X * texTileVertexCount;
			int vertStartZ = tile.Z * texTileVertexCount;

			int vertEndX = vertStartX + texTileVertexCount;
			int vertEndZ = vertStartZ + texTileVertexCount;

			if( vertStartX <= tileVertexStartX && 
				vertEndX >= tileVertexEndX &&
				vertStartZ <= tileVertexStartZ && 
				vertEndZ >= tileVertexEndZ )
			{
				return &tile;
			}
		}
	}

	return NULL;
}

//------------------------------------------------------------------------

r3dTerrain3::MegaTexTile* r3dTerrain3::GetMegaTexTile( int X, int Z, int L )
{
	MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ L ];

	for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
	{
		MegaTexTile* tile = arr[ i ];

		if( tile->X == X && tile->Z == Z )
			return tile;
	}

	return NULL;
}

//------------------------------------------------------------------------

void
r3dTerrain3::FlushAtlasRTs()
{
#if R3D_TERRAIN_V3_GRAPHICS
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
r3dTerrain3::StartTileRoadUpdating()
{
#if R3D_TERRAIN_V3_GRAPHICS
	m_RoadTileUpdateZFar = m_HeightOffset + m_HeightScale * 2.33f;

	r3dRenderer->SetValidVertexShader( VS_FILLGBUFFER_ID );
	r3dRenderer->SetPixelShader( g_RoadAtlas3PixelShaderId );

	r3dRenderer->SetCullMode( D3DCULL_CCW );

	r3dRenderer->SetRenderingMode( R3D_BLEND_ALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::SetupTileRoadUpdateCamera( const AllocatedTile* tile )
{
#if R3D_TERRAIN_V3_GRAPHICS
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
r3dTerrain3::UpdateAtlasTileRoads( const AllocatedTile* tile )
{
#if R3D_TERRAIN_V3_GRAPHICS
	MegaTexTile* megaTile = tile->MegaTile;

	if( !megaTile )
		return;

	if( !megaTile->RoadMipChain.Count() )
		return;

	const QualitySettings& qs = GetCurrentQualitySettings();

	int megaLodOffset = m_MegaTexTileLodOffset;
	int totalMegaLod = megaTile->L + megaLodOffset;

	int lodDistance = totalMegaLod - tile->L;

	int numRoadLods = r3dLog2( qs.VertexTilesInMegaTexTileCount / qs.RoadTileDiv ) + 1;

	int roadLod = numRoadLods - lodDistance - 1;

	int coordDiv = 1 << R3D_MAX( -roadLod, 0 );

	roadLod = R3D_MAX( roadLod, 0 );

	SetupTileRoadUpdateCamera( tile );

	SetupTileAtlasRTs( tile );

	int relX = tile->X - ( megaTile->X << lodDistance );
	int relZ = tile->Z - ( megaTile->Z << lodDistance );

	MegaTexTile::RoadArr2D& rarr2d = megaTile->RoadMipChain[ roadLod ];

	if( rarr2d.Count() )
	{
		MegaTexTile::RoadLinkedList& rll = rarr2d[ relZ / coordDiv ][ relX / coordDiv ];

		MegaTexTile::RoadLinkedList::Iterator iter = rll.GetIterator();

		if( iter.IsValid() )
		{
			int2 atlasXZ = GetTileAtlasXZ( tile );

			r3dRenderer->SetViewport( 	float( atlasXZ.x * m_QualitySettings.AtlasTileDim ), 
										float( ATLAS_TEXTURE_DIM - ( atlasXZ.y + 1 ) * m_QualitySettings.AtlasTileDim ),
										float( m_QualitySettings.AtlasTileDim ),
										float( m_QualitySettings.AtlasTileDim )	);

			for( ; iter.IsValid(); iter.Advance() )
			{
				obj_Road* obj = iter.GetValue();
				obj->DrawRoadToTerraAtlas();
			}
		}
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::StopTileRoadUpdating()
{
#if R3D_TERRAIN_V3_GRAPHICS
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
r3dTerrain3::StartMipChainsTileRendering()
{
#if R3D_TERRAIN_V3_GRAPHICS
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
r3dTerrain3::RenderTileMipChain( const AllocatedTile* tile )
{
#if R3D_TERRAIN_V3_GRAPHICS
	D3DPERF_BeginEvent( 0, L"r3dTerrain3::RenderTileMipChain" );

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

#ifndef FINAL_BUILD
		r3dRenderer->Stats.AddNumTerrainDraws( 1 );
		r3dRenderer->Stats.AddNumTerrainTris( 2 );
#endif

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

#ifndef FINAL_BUILD
			r3dRenderer->Stats.AddNumTerrainDraws( 1 );
			r3dRenderer->Stats.AddNumTerrainTris( 2 );
#endif

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
r3dTerrain3::StopMipChainsTileRendering()
{
#if R3D_TERRAIN_V3_GRAPHICS
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

void r3dTerrain3::SetMegaVertexShader( int shadow, int depth, int cam_vec )
{
	Terrain3VertexShaderId vsid;
	vsid.shadowPath = shadow;
	vsid.depthPath = depth;
	vsid.vfetchless = !m_AllowVFetch;
	vsid.cam_vec = cam_vec;

	r3dRenderer->SetValidVertexShader( g_Terrain3VertexShaderIdMap[ vsid.Id ] );
}

//------------------------------------------------------------------------

void r3dTerrain3::SetMegaVertexShaderNoVFetch( int shadow, int depth, int cam_vec )
{
	Terrain3VertexShaderId vsid;
	vsid.shadowPath = shadow;
	vsid.depthPath = depth;
	vsid.vfetchless = 1;
	vsid.cam_vec = cam_vec;

	r3dRenderer->SetValidVertexShader( g_Terrain3VertexShaderIdMap[ vsid.Id ] );
}

//------------------------------------------------------------------------

void
r3dTerrain3::SetupTileRenderingVertexStates()
{
#if R3D_TERRAIN_V3_GRAPHICS
	StartUsingTileGeom( false );

	if( m_AllowVFetch )
	{
		r3dSetFiltering( R3D_POINT, D3DVERTEXTEXTURESAMPLER0 );
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP ) );
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( D3DVERTEXTEXTURESAMPLER0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP ) );
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::StartTileRendering()
{
#if R3D_TERRAIN_V3_GRAPHICS
	SetupTileRenderingVertexStates();

	r3dRenderer->SetCullMode( D3DCULL_CCW );

#ifndef FINAL_BUILD
#if 0
	if( r_debug_helper->GetInt() )
	{
		r3dSetFiltering( R3D_POINT, 0 );
		r3dSetFiltering( R3D_POINT, 1 );
	}
	else
#endif
#endif
	{
		r3dSetAnisotropy( r_terrain2_anisotropy->GetInt(), 0 );
		r3dSetAnisotropy( r_terrain2_anisotropy->GetInt(), 1 );
	}

	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ) );

	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ) );

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );

	// common pixel shader constants
	{
		r3dTL::TFixedArray< float4, 2 > psConsts;

		// float4 g_SpecularPow_DefSSAO_RangeCoefs    : register( c0 );
		psConsts[ 0 ] = float4( m_Settings.Specular, r_ssao_clear_val->GetFloat(),
								1.0f / ( m_Settings.FarFadeEnd - m_Settings.FarFadeStart ),
								-m_Settings.FarFadeStart / ( m_Settings.FarFadeEnd - m_Settings.FarFadeStart ) );

		// float4 g_Light                             : register( c1 );
		extern r3dSun* Sun;
		psConsts[ 1 ] = float4( Sun->SunDir.x, Sun->SunDir.y, Sun->SunDir.z, 0.0f );

		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, &psConsts[ 0 ].x, psConsts.COUNT ) );
	}

#ifndef FINAL_BUILD
	if( r_terra_wire->GetInt() )
	{
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_FILLMODE , D3DFILL_WIREFRAME ) );
	}
#endif

	SetMegaVertexShader( 0, 0, 0 );

	r3dRenderer->SetTex( m_Settings.FarDiffuseTexture, 2 );
	r3dRenderer->SetTex( m_Settings.FarNormalTexture, 3 );

#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::SetAtlasTileTexcTransform( const AllocatedTile* tile, float4* oTransform )
{
#if R3D_TERRAIN_V3_GRAPHICS
	int2 atlasXZ = GetTileAtlasXZ( tile );

	int FIN_TILE_BORDER = TILE_BORDER * m_QualitySettings.AtlasTileDim / DEFAULT_ATLAS_TILE_DIM;

	float antiBorder = float( DEFAULT_ATLAS_TILE_DIM - 2 * TILE_BORDER ) / float ( DEFAULT_ATLAS_TILE_DIM );

	float antiBorderAddU = float( FIN_TILE_BORDER ) / ATLAS_TEXTURE_DIM;
	float antiBorderAddV = float( FIN_TILE_BORDER ) / ATLAS_TEXTURE_DIM;

	float uvScaleMul0 = m_AtlasTileCountPerSideInv * 0.5f ;
	float uvScaleMul1 = uvScaleMul0 * antiBorder;

	oTransform->x = uvScaleMul1;
	oTransform->y = -uvScaleMul1;

	oTransform->z = uvScaleMul1 + atlasXZ.x * m_AtlasTileCountPerSideInv + 0.0f / ATLAS_TEXTURE_DIM + antiBorderAddU;
	oTransform->w = 1.0f - uvScaleMul1 - atlasXZ.y * m_AtlasTileCountPerSideInv + 0.0f / ATLAS_TEXTURE_DIM - antiBorderAddV;
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::SetMaskAtlasTileTexcTransform( int tileId, const AllocatedTile* tile, const MegaTexTile* megaTile, float extraSpaceCoef, float4* oTransform )
{
#if R3D_TERRAIN_V3_GRAPHICS
	int2 atlasXZ = GetMaskTileAtlasXZ( tileId );
	int lodOffset = m_MegaTexTileLodOffset;

	int effectiveMegaLod = megaTile->L + lodOffset;

	int lodDistance = effectiveMegaLod - tile->L;

	int mtx = megaTile->X;
	int mtz = megaTile->Z;

	int relMegaTileSize = 1 << lodDistance;

	mtx *= relMegaTileSize;
	mtz *= relMegaTileSize;

	int xoff = tile->X - mtx;
	int zoff = tile->Z - mtz;

	int atlasPixelBorder = MEGA_TEX_BORDER;

	float antiBorder = float( m_QualitySettings.MaskAtlasTileDim - 2 * atlasPixelBorder ) / float ( m_QualitySettings.MaskAtlasTileDim );

	float antiBorderAddU = float( atlasPixelBorder ) / MASK_ATLAS_TEXTURE_DIM;
	float antiBorderAddV = float( atlasPixelBorder ) / MASK_ATLAS_TEXTURE_DIM;

	float uvScaleMul0 = m_MaskAtlasTileCountPerSideInv * 0.5f ;
	float uvScaleMul1 = uvScaleMul0 * antiBorder;

	uvScaleMul1 /= relMegaTileSize;

	oTransform->x = uvScaleMul1 * extraSpaceCoef;
	oTransform->y = uvScaleMul1 * extraSpaceCoef;

	float uvOffsetMul = m_MaskAtlasTileCountPerSideInv * antiBorder / relMegaTileSize;

	oTransform->z = ( 0.5f + xoff ) * uvOffsetMul + atlasXZ.x * m_MaskAtlasTileCountPerSideInv + 0.5f / MASK_ATLAS_TEXTURE_DIM + antiBorderAddU;
	oTransform->w = ( 0.5f + zoff ) * uvOffsetMul + atlasXZ.y * m_MaskAtlasTileCountPerSideInv - 0.5f / MASK_ATLAS_TEXTURE_DIM + antiBorderAddV;
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::SetNormalAtlasTileTexcTransform( const MegaTexTile* megaTile, const AllocatedTile * tile, float4* oTransform )
{
	float extraSpaceCoef = GetPaddingCoef();

	int2 atlasXZ = GetHeightNormalAtlasXZ( megaTile->HeightNormalAtlasTileId );
	int lodOffset = m_MegaTexTileLodOffset;

	int effectiveMegaLod = megaTile->L + lodOffset;

	int lodDistance = effectiveMegaLod - tile->L;

	int mtx = megaTile->X;
	int mtz = megaTile->Z;

	int relMegaTileSize = 1 << lodDistance;

	mtx *= relMegaTileSize;
	mtz *= relMegaTileSize;

	int xoff = tile->X - mtx;
	int zoff = tile->Z - mtz;

	int atlasPixelBorder = NORMAL_TILE_BORDER;

	float antiBorder = ( m_QualitySettings.HeightNormalAtlasTileDim - 2 * atlasPixelBorder ) / float ( m_QualitySettings.HeightNormalAtlasTileDim );

	float antiBorderAddU = float( atlasPixelBorder ) / HEIGHT_NORMAL_ATLAS_TEXTURE_DIM * antiBorder;
	float antiBorderAddV = float( atlasPixelBorder ) / HEIGHT_NORMAL_ATLAS_TEXTURE_DIM * antiBorder;

	float uvScaleMul0 = m_HeightNormalAtlasTileCountPerSideInv * 0.5f ;
	float uvScaleMul1 = uvScaleMul0 * antiBorder;

	uvScaleMul1 /= relMegaTileSize;

	oTransform->x = uvScaleMul1 * extraSpaceCoef;
	oTransform->y = uvScaleMul1 * extraSpaceCoef;

	float uvOffsetMul = m_HeightNormalAtlasTileCountPerSideInv * antiBorder / relMegaTileSize;

	oTransform->z = ( 0.5f + xoff ) * uvOffsetMul + atlasXZ.x * m_HeightNormalAtlasTileCountPerSideInv + 0.5f / HEIGHT_NORMAL_ATLAS_TEXTURE_DIM + antiBorderAddU;
	oTransform->w = ( 0.5f + zoff ) * uvOffsetMul + atlasXZ.y * m_HeightNormalAtlasTileCountPerSideInv - 0.5f / HEIGHT_NORMAL_ATLAS_TEXTURE_DIM + antiBorderAddV;
}

//------------------------------------------------------------------------

void r3dTerrain3::SetHeightAtlasTileTexcTransform( const MegaTexTile* megaTile, const AllocatedTile * tile, float4* oTransform, int megaConnFlags )
{
#if R3D_TERRAIN_V3_GRAPHICS
	if( megaTile )
	{
		const QualitySettings& qs = GetCurrentQualitySettings();

		float4* cnst = oTransform;

		int2 atlasXZ = GetHeightNormalAtlasXZ( megaTile->HeightNormalAtlasTileId );
		int lodOffset = m_MegaTexTileLodOffset;

		int effectiveMegaLod = megaTile->L + lodOffset;

		int lodDistance = effectiveMegaLod - tile->L;

		int mtx = megaTile->X;
		int mtz = megaTile->Z;

		int relMegaTileSize = 1 << lodDistance;

		mtx *= relMegaTileSize;
		mtz *= relMegaTileSize;

		int xoff = tile->X - mtx;
		int zoff = tile->Z - mtz;

		float uvScaleMul = m_HeightNormalAtlasTileCountPerSideInv * 0.5f / relMegaTileSize;

#ifndef FINAL_BUILD
		uvScaleMul *= 1.0f + float( r_0debug_helper->GetInt() ) / HEIGHT_NORMAL_ATLAS_TEXTURE_DIM;
#endif

		cnst->x = uvScaleMul;
		cnst->y = uvScaleMul;

		float uvOffsetMul = m_HeightNormalAtlasTileCountPerSideInv / relMegaTileSize;

		float SUBPIXELOFFSET = 0.5f / HEIGHT_NORMAL_ATLAS_TEXTURE_DIM;

		int vertexCount = qs.VertexTileDim << qs.TileVertexDensities[ tile->L ];
		int verticesInMegaTile = vertexCount << lodDistance;

		if( tile->ConFlags )
			verticesInMegaTile *= 2;

		if( verticesInMegaTile > qs.HeightNormalAtlasTileDim )
		{
			SUBPIXELOFFSET = 0.5f / HEIGHT_NORMAL_ATLAS_TEXTURE_DIM * qs.HeightNormalAtlasTileDim / verticesInMegaTile;
		}

#if 0
		SUBPIXELOFFSET += float( r_1debug_helper->GetInt() ) / HEIGHT_NORMAL_ATLAS_TEXTURE_DIM;
#endif

		cnst->z = ( 0.5f + xoff ) * uvOffsetMul + atlasXZ.x * m_HeightNormalAtlasTileCountPerSideInv + SUBPIXELOFFSET;
		cnst->w = ( 0.5f + zoff ) * uvOffsetMul + atlasXZ.y * m_HeightNormalAtlasTileCountPerSideInv + SUBPIXELOFFSET;

		cnst ++;

		if( megaConnFlags & EAST_CONNECTION )
		{
			cnst->x = ( ( atlasXZ.x + 1 ) * qs.HeightNormalAtlasTileDim - 1 ) * m_HeightNormalAtlasTileCountPerSideInv / qs.HeightNormalAtlasTileDim + SUBPIXELOFFSET;
		}
		else
		{
			cnst->x = 2.0f;
		}

		if( megaConnFlags & SOUTH_CONNECTION )
		{
			cnst->y = ( ( atlasXZ.y + 1 ) * qs.HeightNormalAtlasTileDim - 1 ) * m_HeightNormalAtlasTileCountPerSideInv / qs.HeightNormalAtlasTileDim + SUBPIXELOFFSET;
		}
		else
		{
			cnst->y = 2.0f;
		}

		cnst->z = 0.0f;
		cnst->w = 0.0f;
	}
	else
	{
		float4* cnst = oTransform;

		cnst->x = 0.0f;
		cnst->y = 0.0f;
		cnst->z = 0.0f;
		cnst->w = 0.0f;

		cnst ++;

		cnst->x = 2.0f;
		cnst->y = 2.0f;
		cnst->z = 2.0f;
		cnst->w = 2.0f;
	}
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::SetTileRenderVSConsts_G( const AllocatedTile* tile, const r3dCamera& cam )
{
#if R3D_TERRAIN_V3_GRAPHICS
	r3dTL::TFixedArray< float4, 13 > vsConsts;

	float qlMult = float( 1 << tile->L );

	float halfTileDim = m_HalfTileWorldDims [ tile->L ];
	float tileDim = m_TileWorldDims[ tile->L ];

	//------------------------------------------------------------------------
	// float4x4 gWVP               : register( c0 );
	// float4x4 gWV                : register( c4 );
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

		mtx._41 -= cam.X - r3dRenderer->LocalizationPos.X;
		mtx._42 -= cam.Y - r3dRenderer->LocalizationPos.Y;
		mtx._43 -= cam.Z - r3dRenderer->LocalizationPos.Z;

		D3DXMatrixTranspose( (D3DXMATRIX*)&vsConsts[ 4 ], &mtx );
	}

	//------------------------------------------------------------------------
	// float4 gToHeightTexc        : register( c8 );
	// float2 gHeightTexcClamp     : register( c9 );
	SetHeightAtlasTileTexcTransform( tile->MegaTile, tile, vsConsts + 8, tile->MegaConFlags );

	//------------------------------------------------------------------------
	// float4 gToPSTexc            : register( c10 );
	{
		float4* cnst = vsConsts + 10;
		SetAtlasTileTexcTransform( tile, cnst );
	}

	//------------------------------------------------------------------------
	// float4 gToTexcFAR           : register( c11 );
	{
		float4* cnst = vsConsts + 11;

		float halfTileDim = m_HalfTileWorldDims [ tile->L ];
		float tileDim = m_TileWorldDims[ tile->L ];

		cnst->x = halfTileDim / m_TotalTerrainXLength;
		cnst->y = -halfTileDim / m_TotalTerrainZLength;

		cnst->z = ( tile->X * tileDim + halfTileDim ) / m_TotalTerrainXLength;
		cnst->w = -( tile->Z * tileDim + halfTileDim ) / m_TotalTerrainZLength;
	}

	//------------------------------------------------------------------------
	// float gHeightScale          : register( c12 );
	{
		float4* cnst = vsConsts + 12;

		const r3dTerrainDesc& tdesc = Terrain3->GetDesc();
		cnst->x = m_Settings.FringeHeight * ( 1 << tile->L ) / ( tdesc.MaxHeight - tdesc.MinHeight );
	}

	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, &vsConsts[ 0 ].x, vsConsts.COUNT ) );
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::SetDynaBufferFor( const AllocatedTile* tile, int density, int staticBufferOffset )
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

	r3dDynaTerraVertex3* vtx = (r3dDynaTerraVertex3*) m_DynamicVertexBuffer->Lock( m_DynamicVertexBufferPtr, count );

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

int r3dTerrain3::GetTileFringeOffset( const AllocatedTile* tile )
{
	const r3dTerrainDesc& tdesc = GetDesc();
	return R3D_MAX( int( m_Settings.FringeHeight * 65535 / ( tdesc.MaxHeight - tdesc.MinHeight ) * ( 1 << tile->L ) * 0.5f ), 1 );
}


//------------------------------------------------------------------------

template < int CON_FLAG, int IS_FULL >
void r3dTerrain3::SetMegaConnectionDynaBufferFor( const AllocatedTile* tile, int density )
{
	int connFlags = tile->ConFlags;

	int connectedToLowerLod = 0;

	int adjOffset = 0;

	switch( CON_FLAG )
	{
	case SOUTH_CONNECTION:
		adjOffset = tile->AdjecantPosZOffset;
		connectedToLowerLod = tile->ConFlags & SOUTH_CONNECTION;
		break;
	case EAST_CONNECTION:
		connectedToLowerLod = tile->ConFlags & EAST_CONNECTION;
		adjOffset = 0;
		break;
	default:
		r3d_assert( false );
	}

	int sideCount = ( 1 << density ) * m_QualitySettings.VertexTileDim + 1;

	int totalCount		= 0;
	int fringelessCount	= 0;
	int outerCount		= 0;

	if( connectedToLowerLod )
	{
		outerCount = ( sideCount - 1 ) * 2 + 1;	
		fringelessCount = sideCount + outerCount;

		if( !IS_FULL )
		{
			fringelessCount -= 3;
			outerCount -= 2;
		}
	}
	else
	{
		outerCount = sideCount;		
		fringelessCount = sideCount + outerCount;

		if( !IS_FULL )
		{
			fringelessCount -= 2;
			outerCount -= 1;
		}
	}

	totalCount = fringelessCount + outerCount;

	if( IS_FULL )
	{
		totalCount += 2;
	}
	else
	{
		totalCount += 1;
	}


	r3d_assert( totalCount < m_DynamicVertexBuffer->GetItemCount() );

	if( m_DynamicVertexBufferPtr + totalCount > m_DynamicVertexBuffer->GetItemCount() )
	{
		m_DynamicVertexBufferPtr = 0;
	}

	r3dDynaTerraVertex3* vtx = (r3dDynaTerraVertex3*) m_DynamicVertexBuffer->Lock( m_DynamicVertexBufferPtr, totalCount );
	r3dDynaTerraVertex3* start = vtx;

	vtx += outerCount;

	for( int i = 0, e = fringelessCount; i < e; i ++, vtx ++, adjOffset ++ )
	{
		vtx->height = short( tile->AdjecantHeights[ adjOffset ] - 32768 );

		if( i >= outerCount && ( i != e - 1 || !IS_FULL || !tile->IsCorner ) )
			vtx->lerpFactor = 32767;
		else
			vtx->lerpFactor = 0;
	}

	r3d_assert( vtx - start == totalCount - ( IS_FULL ? 2 : 1 ) );

	vtx = start;

	const r3dTerrainDesc& tdesc = GetDesc();

	const int OFFSET = GetTileFringeOffset( tile );

	for( int i = 0, e = outerCount; i < e; i ++, vtx ++ )
	{
		vtx->height = R3D_MAX( (int)( vtx + outerCount )->height - OFFSET, -32768 );
		vtx->lerpFactor = 0;
	}

	vtx = start + totalCount - ( IS_FULL ? 2 : 1 );

	vtx->height = R3D_MAX( ( start + outerCount * 2 )->height - OFFSET, -32768 );
	vtx->lerpFactor = 32767;

	if( IS_FULL )
	{
		vtx ++;

		int second_idx = outerCount * 2 + sideCount - 1;

		r3d_assert( second_idx < totalCount );

		vtx->height = R3D_MAX( (int)( start + second_idx )->height - OFFSET, -32768 );
		vtx->lerpFactor = tile->IsCorner ? 0 : 32767;
	}

	m_DynamicVertexBuffer->Unlock();

	m_DynamicVertexBuffer->Set( 1, m_DynamicVertexBufferPtr );

	m_DynamicVertexBufferPtr += totalCount;
}

//------------------------------------------------------------------------

template < int CON_FLAG >
void r3dTerrain3::RenderTileConnection( const AllocatedTile* tile )
{
	int density = m_QualitySettings.TileVertexDensities[ tile->L ];

	int megaConFlags = tile->MegaConFlags;
	int connFlags = tile->ConFlags;

	if( megaConFlags & CON_FLAG )
	{
		int vertexOffset = 0;
		int indexOffset = 0;
		int primCount = 0;

		int isMega = 0;

		switch( CON_FLAG )
		{
		case SOUTH_CONNECTION:
			if( megaConFlags & EAST_CONNECTION )
			{
				if( connFlags & CON_FLAG )
				{
					vertexOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_SOUTH_CONNECTED_PARTOFMEGA ];
					indexOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_SOUTH_CONNECTED_PARTOFMEGA ];
					primCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_SOUTH_CONNECTED_PARTOFMEGA ];
				}
				else
				{
					vertexOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_SOUTH_PARTOFMEGA ];
					indexOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_SOUTH_PARTOFMEGA ];
					primCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_SOUTH_PARTOFMEGA ];
				}
			}
			else
			{
				isMega = 1;

				if( connFlags & CON_FLAG )
				{
					vertexOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_SOUTH_CONNECTED_MEGACONNECTED ];
					indexOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_SOUTH_CONNECTED_MEGACONNECTED ];
					primCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_SOUTH_CONNECTED_MEGACONNECTED ];
				}
				else
				{
					vertexOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_SOUTH_MEGACONNECTED ];
					indexOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_SOUTH_MEGACONNECTED ];
					primCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_SOUTH_MEGACONNECTED ];
				}
			}
			break;
		case EAST_CONNECTION:
			isMega = 1;

			if( connFlags & CON_FLAG )
			{
				vertexOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_EAST_CONNECTED_MEGACONNECTED ];
				indexOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_EAST_CONNECTED_MEGACONNECTED ];
				primCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_EAST_CONNECTED_MEGACONNECTED ];
			}
			else
			{
				vertexOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_EAST_MEGACONNECTED ];
				indexOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_EAST_MEGACONNECTED ];
				primCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_EAST_MEGACONNECTED ];
			}
			break;
		default:
			r3d_assert( false );
		}

		m_TileVertexBuffer->Set( 0, vertexOffset );

		if( isMega )
			SetMegaConnectionDynaBufferFor< CON_FLAG, 1 > ( tile, density );
		else
			SetMegaConnectionDynaBufferFor< CON_FLAG, 0  > ( tile, density );

		r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, 0, 0, m_TileVertexBuffer->GetItemCount(), indexOffset, primCount );

#ifndef FINAL_BUILD
		r3dRenderer->Stats.AddNumTerrainTris( primCount );
#endif
	}
	else
	{
		int vertexOffset = 0;
		int indexOffset = 0;
		int primCount = 0;

		if( connFlags & CON_FLAG )
		{
			switch( CON_FLAG )
			{
			case NORTH_CONNECTION:
				vertexOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_NORTH_CONNECTED ];
				indexOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_NORTH_CONNECTED ];
				primCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_NORTH_CONNECTED ];
				break;
			case SOUTH_CONNECTION:
				vertexOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_SOUTH_CONNECTED ];
				indexOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_SOUTH_CONNECTED ];
				primCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_SOUTH_CONNECTED ];
				break;
			case WEST_CONNECTION:
				vertexOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_WEST_CONNECTED ];
				indexOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_WEST_CONNECTED ];
				primCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_WEST_CONNECTED ];
				break;
			case EAST_CONNECTION:
				vertexOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_EAST_CONNECTED ];
				indexOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_EAST_CONNECTED ];
				primCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_EAST_CONNECTED ];
				break;
			default:
				r3d_assert( false );
			}
		}
		else
		{
			switch( CON_FLAG )
			{
			case NORTH_CONNECTION:
				vertexOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_NORTH ];
				indexOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_NORTH ];
				primCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_NORTH ];
				break;
			case SOUTH_CONNECTION:
				vertexOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_SOUTH ];
				indexOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_SOUTH ];
				primCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_SOUTH ];
				break;
			case WEST_CONNECTION:
				vertexOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_WEST ];
				indexOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_WEST ];
				primCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_WEST ];
				break;
			case EAST_CONNECTION:
				vertexOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_EAST ];
				indexOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_EAST ];
				primCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_EAST ];
				break;
			default:
				r3d_assert( false );
			}
		}

#ifndef FINAL_BUILD
		r3dRenderer->Stats.AddNumTerrainTris( primCount );
#endif

		r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, vertexOffset, 0, m_TileVertexBuffer->GetItemCount(), indexOffset, primCount );
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::Set1x1MegaConnectionDynaBuffer( const AllocatedTile* tile )
{
	int lockCount = 0;

	if( tile->MegaConFlags & SOUTH_CONNECTION
		&&
		tile->MegaConFlags & EAST_CONNECTION
		)
	{
		lockCount = 7;
	}
	else
	{
		lockCount = 6;
	}

	if( m_DynamicVertexBufferPtr + lockCount > m_DynamicVertexBuffer->GetItemCount() )
	{
		m_DynamicVertexBufferPtr = 0;
	}

	const r3dTerrainDesc& tdesc = GetDesc();

	const int OFFSET = GetTileFringeOffset( tile ) + 32768;

	r3dDynaTerraVertex3* vtx = (r3dDynaTerraVertex3*) m_DynamicVertexBuffer->Lock( m_DynamicVertexBufferPtr, lockCount );
	r3dDynaTerraVertex3* start = vtx;

	if( tile->MegaConFlags & SOUTH_CONNECTION
			&&
		tile->MegaConFlags & EAST_CONNECTION
			)
	{
		// v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
		vtx->height = 0;
		vtx->lerpFactor = 32767;
		vtx ++;

		// v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
		vtx->height = tile->AdjecantHeights[ 0 ] - 32768;
		vtx->lerpFactor = 0;
		vtx ++;

		// v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
		vtx->height = tile->AdjecantHeights[ 3 ] - 32768;
		vtx->lerpFactor = 0;
		vtx ++;

		// v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
		vtx->height = tile->AdjecantHeights[ 1 ] - 32768;
		vtx->lerpFactor = 0;
		vtx ++;

		//------------------------------------------------------------------------
		// v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
		vtx->height = R3D_MAX( tile->AdjecantHeights[ 3 ] - OFFSET, -32768 );
		vtx->lerpFactor = 0;
		vtx ++;
		// v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
		vtx->height = R3D_MAX(tile->AdjecantHeights[ 1 ] - OFFSET, -32768 );
		vtx->lerpFactor = 0;
		vtx ++;

		// v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, FRINGE_ZVAL, 0 );
		vtx->height = R3D_MAX( tile->AdjecantHeights[ 0 ] - OFFSET, -32768 );
		vtx->lerpFactor = 0;
		vtx ++;
	}
	else
	{
		if( tile->MegaConFlags & SOUTH_CONNECTION )
		{
			// v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
			vtx->height = 0;
			vtx->lerpFactor = 32767;
			vtx ++;

			// v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
			vtx->height = 0;
			vtx->lerpFactor = 32767;
			vtx ++;

			// v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
			vtx->height = tile->AdjecantHeights[ 0 + tile->AdjecantPosZOffset ] - 32768;
			vtx->lerpFactor = 0;
			vtx ++;

			// v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
			vtx->height = tile->AdjecantHeights[ 1 + tile->AdjecantPosZOffset ] - 32768;
			vtx->lerpFactor = 0;
			vtx ++;

			//------------------------------------------------------------------------
			// v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
			vtx->height = R3D_MAX( tile->AdjecantHeights[ 0 + tile->AdjecantPosZOffset ] - OFFSET, -32768 );
			vtx->lerpFactor = 0;
			vtx ++;

			// v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
			vtx->height = R3D_MAX( tile->AdjecantHeights[ 1 + tile->AdjecantPosZOffset ] - OFFSET, -32768 );
			vtx->lerpFactor = 0;
			vtx ++;
		}
		else
		{
			r3d_assert( tile->MegaConFlags & EAST_CONNECTION );

			// v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
			vtx->height = 0;
			vtx->lerpFactor = 32767;
			vtx ++;

			// v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
			vtx->height = tile->AdjecantHeights[ 0 ] - 32768;
			vtx->lerpFactor = 0;
			vtx ++;

			// v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
			vtx->height = 0;
			vtx->lerpFactor = 32767;
			vtx ++;

			// v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
			vtx->height = tile->AdjecantHeights[ 1 ] - 32768;
			vtx->lerpFactor = 0;
			vtx ++;

			//------------------------------------------------------------------------
			// v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, FRINGE_ZVAL, 0 );
			vtx->height = R3D_MAX( tile->AdjecantHeights[ 0 ] - OFFSET, -32768 );
			vtx->lerpFactor = 0;
			vtx ++;

			// v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
			vtx->height = R3D_MAX( tile->AdjecantHeights[ 1 ] - OFFSET, -32768 );
			vtx->lerpFactor = 0;
			vtx ++;

		}
	}

	r3d_assert( vtx - start == lockCount );

	m_DynamicVertexBuffer->Unlock();

	m_DynamicVertexBuffer->Set( 1, m_DynamicVertexBufferPtr );

	m_DynamicVertexBufferPtr += lockCount;
}

//------------------------------------------------------------------------

void r3dTerrain3::RenderConnected1x1Tile( const AllocatedTile* tile )
{
	int vertexOffset = 0;
	int indexOffset = 0;
	int primCount = 0;

	int density = m_QualitySettings.TileVertexDensities[ tile->L ];

	if( tile->MegaConFlags & SOUTH_CONNECTION 
			&&
		tile->MegaConFlags & EAST_CONNECTION
		)
	{
		vertexOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_SOUTH_EAST_MEGACONNECTED ];
		indexOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_SOUTH_EAST_MEGACONNECTED ];
		primCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_SOUTH_EAST_MEGACONNECTED ];
	}
	else
	{
		if( tile->MegaConFlags & SOUTH_CONNECTION  )
		{
			vertexOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_SOUTH_MEGACONNECTED ];
			indexOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_SOUTH_MEGACONNECTED ];
			primCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_SOUTH_MEGACONNECTED ];
		}
		else
		{
			if( tile->MegaConFlags & EAST_CONNECTION )
			{
				vertexOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_EAST_MEGACONNECTED ];
				indexOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_EAST_MEGACONNECTED ];
				primCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_EAST_MEGACONNECTED ];
			}
		}
	}

	m_TileVertexBuffer->Set( 0, vertexOffset );

	Set1x1MegaConnectionDynaBuffer( tile );

#ifndef FINAL_BUILD
	r3dRenderer->Stats.AddNumTerrainTris( primCount );
#endif

	r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, 0, 0, m_TileVertexBuffer->GetItemCount(), indexOffset, primCount );
}

//------------------------------------------------------------------------

void r3dTerrain3::IssueTileDIPs( const AllocatedTile* tile, int shadow, int cam_vec )
{
	int density = m_QualitySettings.TileVertexDensities[ tile->L ];

	if( int megaConFlags = tile->MegaConFlags )
	{
		int tooSmall = m_QualitySettings.VertexTileDim << tile->L < 2;

#ifndef FINAL_BUILD
		int dipCount = tooSmall ? 1 : 5;

		r3dRenderer->Stats.AddNumTerrainDraws( dipCount );
#endif

		{
			if( m_QualitySettings.VertexTileDim << tile->L > 2 )
			{
#ifndef FINAL_BUILD
				r3dRenderer->Stats.AddNumTerrainTris( g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_INSIDE ] );
#endif

				int vbOffset = g_StitchTileVBOffsets[ density ][ TILEGEOMTYPE_INSIDE ];
				int vbCount = g_StitchTileVBCounts[ density ][ TILEGEOMTYPE_INSIDE ];

				int ibOffset = g_StitchTileIBOffsets[ density ][ TILEGEOMTYPE_INSIDE ];
				int ibCount = g_StitchTileIBCounts[ density ][ TILEGEOMTYPE_INSIDE ];

				r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, vbOffset, 0, vbCount, ibOffset, ibCount );
			}
		}

		SetMegaVertexShaderNoVFetch( shadow, 0, cam_vec );
		SetTileGeomVDeclNoVFetch();

		if( tooSmall )
		{
			RenderConnected1x1Tile( tile );
		}
		else
		{
			if( megaConFlags & SOUTH_CONNECTION ) RenderTileConnection< SOUTH_CONNECTION > ( tile );
			if( megaConFlags & EAST_CONNECTION ) RenderTileConnection< EAST_CONNECTION > ( tile );
		}


		SetTileGeomVDecl( false );

		SetMegaVertexShader( shadow, 0, cam_vec );

		m_TileVertexBuffer->Set( 0 );

		{
			if( !tooSmall )
			{
				if( !( megaConFlags & SOUTH_CONNECTION ) ) RenderTileConnection< SOUTH_CONNECTION > ( tile );
				if( !( megaConFlags & EAST_CONNECTION ) ) RenderTileConnection< EAST_CONNECTION > ( tile );

				RenderTileConnection< NORTH_CONNECTION > ( tile );
				RenderTileConnection< WEST_CONNECTION > ( tile );
			}
		}
	}
	else
	{
		int vertexOffset = m_ConnectionVertexOffsets[ density ];

		int connFlags = tile->ConFlags;

		int primitiveCount = m_ConnectionIndexCounts[ density ][ connFlags ];
		int indexOffset = m_ConnectionIndexOffsets[ density ][ connFlags ];

		if( !m_AllowVFetch )
		{
			SetDynaBufferFor( tile, density, vertexOffset );
			vertexOffset = 0;
		}

		{
#ifndef FINAL_BUILD
			r3dRenderer->Stats.AddNumTerrainDraws( 1 );
			r3dRenderer->Stats.AddNumTerrainTris( primitiveCount );
#endif

			r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, vertexOffset, 0, m_TileVertexBuffer->GetItemCount(), indexOffset, primitiveCount );
		}
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::RenderTile_G( const AllocatedTile* tile, const r3dCamera& cam, bool forceNear, bool skipPixelShader /*= false*/ )
{
#if R3D_TERRAIN_V3_GRAPHICS
	MegaTexTile* megaTile = tile->MegaTile;
	if( !megaTile )
		return;

#ifndef FINAL_BUILD
	wchar_t sign[ 64 ];

	swprintf( sign, sizeof sign, L"%d,%d,%d", tile->L, tile->Z, tile->X );

	D3DPERF_BeginEvent( 0, sign );
#endif

	int needCamVec = 0;

	// Pixel Shader
	if( !skipPixelShader )
	{
		Terrain3PixelShaderId psid;

		psid.aux = r_lighting_quality->GetInt() == 1 ? 0 : 1;
		psid.forward_lighting = r_in_minimap_render->GetInt() ?  1 : 0;

		if( !forceNear && m_Settings.FarDiffuseTexture && m_Settings.FarLayerEnabled )
		{
			float tileDimDiag = m_TileWorldDims[ tile->L ] * 1.42f * 2.0f;

			psid.near_textures = !!( tile->FarNearFlags & AllocatedTile::NEAR_TILE );
			psid.far_textures = !!( tile->FarNearFlags & AllocatedTile::FAR_TILE );
		}
		else
		{
			psid.near_textures = 1;
			psid.far_textures = 0;
		}

		int psID = g_Terrain3PixelShaderIdMap[ psid.Id ];

		r3dRenderer->SetPixelShader( psID );

		needCamVec = psid.near_textures && psid.far_textures;
	}

	SetMegaVertexShader( 0, 0, needCamVec );

	// check if at the edges of the megatile
	SetTileRenderVSConsts_G( tile, cam );

	if( tile->AtlasVolumeID >= 0 )
	{
		const AtlasVolume& vol = m_Atlas[ tile->AtlasVolumeID ];

		r3dRenderer->SetTex( vol.Diffuse->Tex, 0 );
		r3dRenderer->SetTex( vol.Normal->Tex, 1 );
	}
	else
	{
		r3d_assert( !( tile->FarNearFlags & AllocatedTile::NEAR_TILE ) );
	}
	
	r3dRenderer->SetTex( m_HeightNormalAtlas[ megaTile->HeightNormalAtlasVolumeId ].Height, D3DVERTEXTEXTURESAMPLER0 );

	IssueTileDIPs( tile, 0, needCamVec );

#ifndef FINAL_BUILD
	D3DPERF_EndEvent();
#endif

#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::EndTileRendering()
{
#if R3D_TERRAIN_V3_GRAPHICS
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
r3dTerrain3::StartShadowRender()
{
#if R3D_TERRAIN_V3_GRAPHICS
	SetupTileRenderingVertexStates();

	// Pixel Shader
	{
		r3dRenderer->SetPixelShader( g_Terrain3ShadowPSId );

		// float4 gShadowOffset        : register( c13 );
		float psConst[ 4 ] = { m_Settings.ShadowOffset, 0.f, 0.f, 0.f };
		D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 13, psConst, 1 ) );
	}	

	r3dRenderer->SetCullMode( D3DCULL_CCW );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::RenderShadowTile( const AllocatedTile* tile, const r3dCamera& cam )
{
#if R3D_TERRAIN_V3_GRAPHICS
	SetMegaVertexShader( 1, 0, 0 );
	SetTileRenderVSConsts_G( tile, cam );

	r3dRenderer->SetTex( m_HeightNormalAtlas[ tile->MegaTile->HeightNormalAtlasVolumeId ].Height, D3DVERTEXTEXTURESAMPLER0 );

	IssueTileDIPs( tile, 1, 0 );

#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::StopShadowRender()
{
#if R3D_TERRAIN_V3_GRAPHICS
	r3dRenderer->RestoreCullMode();
	StopUsingTileGeom()	;
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::StartDepthRender()
{
#if R3D_TERRAIN_V3_GRAPHICS
	SetupTileRenderingVertexStates();
	r3dRenderer->SetCullMode( D3DCULL_CCW );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::RenderDepthTile( const AllocatedTile* tile, const r3dCamera& cam )
{
#if R3D_TERRAIN_V3_GRAPHICS
	SetMegaVertexShader( 0, 1, 0 );
	SetTileRenderVSConsts_G( tile, cam );

	int connFlags = tile->ConFlags;

	int density = m_QualitySettings.TileVertexDensities[ tile->L ];

	int primitiveCount = m_ConnectionIndexCounts[ density ][ connFlags ];
	int indexOffset = m_ConnectionIndexOffsets[ density ][ connFlags ];
	int vertexOffset = m_ConnectionVertexOffsets[ density ];

#ifndef FINAL_BUILD
	r3dRenderer->Stats.AddNumTerrainDraws( 1 );
	r3dRenderer->Stats.AddNumTerrainTris( primitiveCount );
#endif

	r3dRenderer->DrawIndexed( D3DPT_TRIANGLELIST, vertexOffset, 0, m_TileVertexBuffer->GetItemCount(), indexOffset, primitiveCount );
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::StopDepthRender()
{
#if R3D_TERRAIN_V3_GRAPHICS
	r3dRenderer->RestoreCullMode();
	StopUsingTileGeom()	;
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::GetBaseTileCounts( int* oBaseTileCountX, int* oBaseTileCountZ )
{
	*oBaseTileCountX = m_VertexCountX / QualitySettings::MIN_VERTEX_TILE_DIM;
	*oBaseTileCountZ = m_VertexCountZ / QualitySettings::MIN_VERTEX_TILE_DIM;
}

//------------------------------------------------------------------------

void
r3dTerrain3::FreeAtlasTiles()
{
#if R3D_TERRAIN_V3_GRAPHICS
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

void r3dTerrain3::FreeMegaAtlasTiles()
{
#if R3D_TERRAIN_V3_GRAPHICS
	for( int i = 0, e = (int)m_AllocMegaTexTileLodArray.Count(); i < e; i ++ )
	{
		MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ i ];

		for( int i = 0, e = (int)arr.Count(); i < e; i ++ )
		{
			MegaTexTile* tile = arr[ i ];

			if( tile->IsInAtlas() )
			{
				FreeMegaTexTile( tile );
			}
		}

		arr.Clear();
	}
#endif
}

//------------------------------------------------------------------------

void
r3dTerrain3::RemoveAllocatedTiles()
{
#if R3D_TERRAIN_V3_GRAPHICS
	m_TilesToUpdate.Clear();
	FreeAtlasTiles();

	for( int i = 0, e = (int)m_AllocTileLodArray.Count(); i < e; i ++ )
	{
		m_AllocTileLodArray[ i ].Clear();
	}
#endif
}

//------------------------------------------------------------------------

static r3dTerrain3::QualitySettings g_EmptyQS;

const r3dTerrain3::QualitySettings&
r3dTerrain3::SelectQualitySettings()
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

	r3dError( "r3dTerrain3::SelectQualitySettings: quality settings out of range!" );
	return g_EmptyQS;
}

//------------------------------------------------------------------------

void
r3dTerrain3::PopulateInferiorQualitySettings()
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

void r3dTerrain3::UpdateEditor()
{
#ifndef FINAL_BUILD
	if( g_bEditMode )
	{
		float newTime = r3dGetTime();
		float dt = newTime - gSaveCountDownLastTime;

		gSaveCountDownLastTime = newTime;

		for( int i = 0, e = m_EDITOR_TilesToReload.Count(); i < e; )
		{
			MegaTexTile* tile = m_EDITOR_TilesToReload[ i ];

			if( tile->IsAllocated && tile->IsLoaded )
			{
				AddMegaTexTileUpdateJobs( tile, LOADTILE_MASKS | LOADTILE_UPDATEATLAS );
				m_EDITOR_TilesToReload.Erase( i );
				e --;
			}
			else
			{
				i ++;
			}
		}
	}
#endif
}

//------------------------------------------------------------------------

struct LoadTextureParams
{
	r3dTexture** tex;
	int desiredWidth;
	int desiredHeight;
	D3DFORMAT desiredFmt;
};

static void LoadTexInMainThread( void* vparams )
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

//------------------------------------------------------------------------

void
r3dTerrain3::GetMaskTileXY( const r3dPoint3D& pos, int* oOutX, int* oOutZ )
{
	*oOutX = int( pos.x / m_CellSize ) / m_QualitySettings.VertexTilesInMegaTexTileCount;
	*oOutZ = int( pos.z / m_CellSize ) / m_QualitySettings.VertexTilesInMegaTexTileCount;
}

//------------------------------------------------------------------------

void
r3dTerrain3::OptimizeMegaTexTiles()
{
	for( int i = 0, e = (int)m_AllocMegaTexTileLodArray.Count() - 1; i < e; i ++ )
	{
		MegaTexTilePtrArr& arr0 = m_AllocMegaTexTileLodArray[ i ];

		for( int j = arr0.Count() - 1; j >= 0 ; j -- )
		{
			MegaTexTile& tile = *arr0[ j ];

			// !tile.IsLoading is for editor only - where it is possible that something is loaded and also loading ( updating ).
			if( tile.IsLoaded && 
#ifndef FINAL_BUILD
				!tile.IsLoading && 
#endif
				!tile.Tagged )
			{
				int covered = 0;

				int div = 2;

				int endLayer = m_NumActiveMegaTexQualityLayers - 1;

#ifndef FINAL_BUILD
				// enable debugging the topmost quality layer
				if( r_min_terramegalod->GetInt() == m_NumActiveMegaTexQualityLayers - 1 )
					endLayer ++;
#endif

				if( m_TileCountX != m_TileCountZ )
				{
					endLayer = m_NumActiveMegaTexQualityLayers;
				}

#if 0
				// only allow checking against the last LOD if we are a LOD before the last LOD.
				// otherwise we will always jump to the last covering LOD no matter what ( more detailed covering lods
				// will be deleted prematurely ).
				// This is because the last LOD is always present (loaded)
				if( tile.L == m_NumActiveMegaTexQualityLayers - 2 )
					endLayer ++;
#endif

				for( int ii = i + 1; ii < endLayer && !covered; ii ++, div *= 2 )
				{
					MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ ii ];

					for( int i = (int)arr.Count() - 1; i >= 0; i -- )
					{
						MegaTexTile& covering = *arr[ i ];

						if( tile.X / div == covering.X 
							&&
							tile.Z / div == covering.Z
							&&
							covering.IsLoaded
							&&
							covering.Tagged
							)
						{
							FreeMegaTexTile( &tile );

							arr0.Erase( j );
							covered = 1;
							break;
						}
					}
				}

				if( !covered && i - 1 >= 0 )
				{
					MegaTexTilePtrArr& arr = m_AllocMegaTexTileLodArray[ i - 1 ];

					int coveringTiles[ 4 ][ 2 ];

					for( int i = 0; i < 4; i ++ )
					{
						coveringTiles[ i ][ 0 ] = -1;
						coveringTiles[ i ][ 1 ] = 0;
					}

					for( int i = (int)arr.Count() - 1; i >= 0; i -- )
					{
						MegaTexTile& covering = *arr[ i ];

						if( covering.IsLoaded
							&&
							covering.Tagged
							&&
							tile.X == covering.X / 2
							&&
							tile.Z == covering.Z / 2
							)
						{
							int code = covering.X + ( covering.Z << 16 );

							int found = 0;

							for( int i = 0; i < 4; i ++ )
							{
								if( coveringTiles[ i ][ 0 ] == code )
								{
									coveringTiles[ i ][ 1 ] ++;
									found = 1;
									break;
								}
							}

							if( !found )
							{
								for( int i = 0; i < 4; i ++ )
								{
									if( !coveringTiles[ i ][ 1 ] )
									{
										coveringTiles[ i ][ 0 ] = code;
										coveringTiles[ i ][ 1 ] = 1;
										break;
									}
								}
							}
						}
					}

					int c = 0;

					for( int i = 0, e = 4; i < e; i ++ )
					{
						if( coveringTiles[ i ][ 1 ] )
							c ++;
					}

					if( c == 4 )
					{
						FreeMegaTexTile( &tile );

						arr0.Erase( j );
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------------

r3dTerrain3::MegaTexFileGridOffset r3dTerrain3::GetMegaTexMaskOffsetInFile( MegaTexTile* tile, int maskIdx )
{
	INT64 id = tile->GetID( maskIdx );

	FileTextureGridMap::iterator found = m_MegaTexGridFile_Map.find( id );

	if( found == m_MegaTexGridFile_Map.end() )
	{
		ErrorOnMissingMask( "r3dTerrain3::GetMegaTexMaskOffsetInFile", id );
	}

	return found->second;
}

//------------------------------------------------------------------------

r3dTerrain3::MegaTexFileOffset r3dTerrain3::GetMegaTexMaskOffsetInFile_OLD( MegaTexTile* tile, int maskIdx )
{
	INT64 id = tile->GetID( maskIdx );

	FileTextureMap::iterator found = m_MegaTexAtlasFile_Map.find( id );

	if( found == m_MegaTexAtlasFile_Map.end() )
	{
		ErrorOnMissingMask( "r3dTerrain3::GetMegaTexMaskOffsetInFile", id );
	}

	return found->second;
}

//------------------------------------------------------------------------

r3dTerrain3::MegaTexFileGridOffset	r3dTerrain3::GetMegaTexHeightNormalOffsetInFile( MegaTexTile* tile )
{
#ifndef FINAL_BUILD
	INT64 id = tile->GetID( 0 );
	FileTextureGridMap::const_iterator found = m_EDITOR_HeightNormalTileMap.find( id );

	if( found != m_EDITOR_HeightNormalTileMap.end() )
	{
		return found->second;
	}
#endif

	MegaTexFileGridOffset offset;

	GetMegaTexHeightNormalTargetOffsetInFile( tile, &offset );

	return offset;
}

//------------------------------------------------------------------------

r3dTerrain3::MegaTexFileOffset r3dTerrain3::GetMegaTexHeightNormalOffsetInFile_OLD( MegaTexTile* tile )
{
	MegaTexFileOffset offset;
	offset.Value = GetMegaTexHeightNormalTargetOffsetInFile( tile );

	return offset;
}

//------------------------------------------------------------------------

void r3dTerrain3::GetMegaTexHeightNormalTargetOffsetInFile( MegaTexTile* tile, MegaTexFileGridOffset* oOffset )
{
	int hmSize = r3dGetPixelSize( m_MegaTexAtlasFile_HeightDim * m_MegaTexAtlasFile_HeightDim, D3DFORMAT( m_MegaTexAtlasFile_HeightFormat ) );
	int nmSize = r3dGetPixelSize( m_MegaTexAtlasFile_NormalDim * m_MegaTexAtlasFile_NormalDim, D3DFORMAT( m_MegaTexAtlasFile_NormalFormat ) );

	int tileCountX = m_TileCountX / m_QualitySettings.VertexTilesInMegaTexTileCount;
	int tileCountZ = m_TileCountZ / m_QualitySettings.VertexTilesInMegaTexTileCount;

	INT64 offset = m_MegaTexAtlasFile_HeightNormalOffset;

	int chunkSize = sizeof MEGA_HN_SIG + hmSize + nmSize;

	if( tile->L )
	{
		oOffset->GridId = GRID_ID_MIPSFILE;

		for( int i = 1, e = tile->L; i < e ; i ++ )
		{
			r3d_assert( tileCountX && tileCountZ );

			tileCountX /= 2;
			tileCountZ /= 2;

			offset += tileCountX * tileCountZ * chunkSize;
		}

		tileCountX /= 2;
		tileCountZ /= 2;

		offset += ( tileCountX * tile->Z + tile->X ) * chunkSize;
	}
	else
	{
		int fileGridX = tile->X / m_TilesInFileChunk;
		int fileGridZ = tile->Z / m_TilesInFileChunk;

		EncodeGridCoordinates( oOffset, fileGridX, fileGridZ );

		int subChunkX = tile->X % m_TilesInFileChunk;
		int subChunkZ = tile->Z % m_TilesInFileChunk;

		offset += ( m_TilesInFileChunk * subChunkZ + subChunkX ) * chunkSize;
	}

	oOffset->Value = offset;
}

//------------------------------------------------------------------------

INT64 r3dTerrain3::GetMegaTexHeightNormalTargetOffsetInFile( MegaTexTile* tile )
{
	MegaTexFileOffset result;

	int hmSize = r3dGetPixelSize( m_MegaTexAtlasFile_HeightDim * m_MegaTexAtlasFile_HeightDim, D3DFORMAT( m_MegaTexAtlasFile_HeightFormat ) );
	int nmSize = r3dGetPixelSize( m_MegaTexAtlasFile_NormalDim * m_MegaTexAtlasFile_NormalDim, D3DFORMAT( m_MegaTexAtlasFile_NormalFormat ) );

	int tileCountX = m_TileCountX / m_QualitySettings.VertexTilesInMegaTexTileCount;
	int tileCountZ = m_TileCountZ / m_QualitySettings.VertexTilesInMegaTexTileCount;

	INT64 offset = m_MegaTexAtlasFile_HeightNormalOffset;

	int chunkSize = sizeof MEGA_HN_SIG + hmSize + nmSize;

	for( int i = 0, e = tile->L; i < e ; i ++ )
	{
		r3d_assert( tileCountX && tileCountZ );

		offset += tileCountX * tileCountZ * chunkSize;

		tileCountX /= 2;
		tileCountZ /= 2;
	}

	offset += ( tileCountX * tile->Z + tile->X ) * chunkSize;

	return offset;
}

//------------------------------------------------------------------------
/*static*/ void r3dTerrain3::MegaTexTileLoadJob( r3dTaskParams* parameters )
{
	MegaTexLoadJobParams* mtlParms = static_cast<MegaTexLoadJobParams*> ( parameters );
	mtlParms->terrain->DoLoadMegaTexTile( mtlParms->tile, mtlParms->loadFlags );
}

//------------------------------------------------------------------------

void r3dTerrain3::AddMegaTexTileLoadJobs( MegaTexTile* tile, int loadFlags )
{
	r3d_assert( !tile->IsLoaded );

	AddMegaTexTileJobs( tile, loadFlags );
}

//------------------------------------------------------------------------

void r3dTerrain3::AddMegaTexTileUpdateJobs( MegaTexTile* tile, int loadFlags )
{
	r3dBackgroundTaskDispatcher::Tasks tasks;
	g_pBackgroundTaskDispatcher->GetPendingTasks( &tasks );

	for( int i = 0, e = (int)tasks.Count(); i < e; i ++ )
	{
		r3dBackgroundTaskDispatcher::TaskDescriptor desc = tasks[ i ];

		if( desc.Fn == MegaTexTileLoadJob )
		{
			MegaTexLoadJobParams* params = static_cast< MegaTexLoadJobParams* >( desc.Params );
			if( params )
			{
				if( params->tile == tile )
					return;
			}
		}
	}

	AddMegaTexTileJobs( tile, loadFlags | LOADTILE_DISABLEFILTER );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE void DecompressBlockDXT1(unsigned long x, unsigned long y, unsigned long width, const unsigned char *blockStorage, r3dTerrain3::UShorts *image)
{
	unsigned short color0 = *reinterpret_cast<const unsigned short *>(blockStorage);
	unsigned short color1 = *reinterpret_cast<const unsigned short *>(blockStorage + 2);

	unsigned char r0 = (unsigned char)( color0 >> 11 );
	unsigned char g0 = (unsigned char)( color0 >> 5 & 0x3F );
	unsigned char b0 = (unsigned char)( color0 & 0x1F );

	unsigned char r1 = (unsigned char)( color1 >> 11 );
	unsigned char g1 = (unsigned char)( color1 >> 5 & 0x3F );
	unsigned char b1 = (unsigned char)( color1 & 0x1F );

	unsigned long code = *reinterpret_cast<const unsigned long *>(blockStorage + 4);

	for (int j=0; j < 4; j++)
	{
		for (int i=0; i < 4; i++)
		{
			unsigned short finalColor = 0;
			unsigned char positionCode = (code >>  2*(4*j+i)) & 0x03;

			if ( color0 > color1 )
			{
				switch (positionCode)
				{
				case 0:
					finalColor = color0;
					break;
				case 1:
					finalColor = color1;
					break;
				case 2:
					finalColor = ( (2*r0+r1)/3 << 11 ) | ( (2*g0+g1)/3 << 5 ) | (2*b0+b1)/3;
					break;
				case 3:
					finalColor = ( (r0+2*r1)/3 << 11 ) | ( (g0+2*g1)/3 << 5 ) | (b0+2*b1)/3;
					break;
				}
			}
			else
			{
				switch( positionCode )
				{
				case 0:
					finalColor = color0;
					break;
				case 1:
					finalColor = color1;
					break;
				case 2:
					finalColor = ( (r0+r1)/2 << 11 ) | ( (g0+g1)/2 << 5 ) | (b0+b1)/2;
					break;
				case 3:
					finalColor = 0;
					break;
				}
			}

			if (x + i < width)
			{
				int idx = (y + j)*width + (x + i);
				(*image)[ idx ] = finalColor;
			}
		}
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::UnpackDXT1( void* src, UShorts* dest, int width, int height )
{
	unsigned long blockCountX = (width + 3) / 4;
	unsigned long blockCountY = (height + 3) / 4;
	unsigned long blockWidth = (width < 4) ? width : 4;
	unsigned long blockHeight = (height < 4) ? height : 4;

	unsigned char* blockStorage = (unsigned char*)src;

	for ( unsigned long j = 0; j < blockCountY; j++ )
	{
		for( unsigned long i = 0; i < blockCountX; i++ ) DecompressBlockDXT1( i*4, j*4, width, blockStorage + i * 8, dest );
		blockStorage += blockCountX * 8;
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::AddMegaTexTileJobs( MegaTexTile* tile, int loadFlags )
{
	r3dBackgroundTaskDispatcher::TaskDescriptor td;

	MegaTexLoadJobParams* params = g_MexaTexLoadTaskParams.Alloc();

#ifndef FINAL_BUILD
	InterlockedExchange( &tile->IsLoading, 1 );
#endif

	params->terrain = this;
	params->tile = tile;
	params->loadFlags = loadFlags;

	td.Params = params;
	td.Fn = MegaTexTileLoadJob;
	td.TaskClass = r3dBackgroundTaskDispatcher::TASK_CLASS_MEGATEXTURE;
	td.CompletionFlag = 0;

	g_pBackgroundTaskDispatcher->AddTask( td );
}

//------------------------------------------------------------------------

struct UpdateSurfaceParams
{
	r3dD3DTextureTunnel	source;
	r3dD3DTextureTunnel	dest;
	int					destX;
	int					destY;
};

void UpdateSurface_MainThread( void* params )
{
	UpdateSurfaceParams* usParams = static_cast<UpdateSurfaceParams*>( params );

	POINT pt;

	pt.x = usParams->destX;
	pt.y = usParams->destY;

	IDirect3DTexture9* destTex = usParams->dest.AsTex2D();
	IDirect3DTexture9* srcTex = usParams->source.AsTex2D();

	IDirect3DSurface9 *srcSurf, *destSurf;

	D3D_V( srcTex->GetSurfaceLevel( 0, &srcSurf ) );
	D3D_V( destTex->GetSurfaceLevel( 0, &destSurf ) );

	D3D_V( r3dRenderer->pd3ddev->UpdateSurface( srcSurf, NULL, destSurf, &pt ) );

	SAFE_RELEASE( srcSurf );
	SAFE_RELEASE( destSurf );
}

template < typename T >
void r3dTerrain3::DoLoadMegaTexTileMaskFromFile( T file, INT64 offset, D3DSURFACE_DESC* maskSurfDesc, r3dD3DTextureTunnel targetMask, int expectedMaskID, MegaTexTile* targetTile, Bytes* dominantLayerData, Bytes* dominantLayerValues, int bCheckEmpty )
{
	const r3dTerrain3::QualitySettings& qs = GetCurrentQualitySettings();

	fseek( file, (long)offset, SEEK_SET );

	short index = -1;
	fread( &index, sizeof index, 1, file );

	r3d_assert( index == expectedMaskID );

	D3DLOCKED_RECT lrect;

	targetMask.LockRect( 0, &lrect, NULL, 0 );

	r3d_assert( lrect.Pitch == r3dGetTextureBlockLineSize( maskSurfDesc->Width, TERRA3_LAYERMASK_FORMAT ) );

	fread( lrect.pBits, lrect.Pitch * r3dGetTextureBlockHeight( maskSurfDesc->Height, TERRA3_LAYERMASK_FORMAT ), 1, file );

	if( targetTile )
	{
		m_UnpackedMask.Resize( maskSurfDesc->Width * maskSurfDesc->Height );
		UnpackDXT1( lrect.pBits, &m_UnpackedMask, maskSurfDesc->Width, maskSurfDesc->Height );

		if( dominantLayerData )
		{
			int baseLayer = index * LAYERS_PER_MASK;

			union
			{
				struct
				{
					UINT16 r : 5;
					UINT16 g : 6;
					UINT16 b : 5;
				};

				UINT16 val;
			};

			for( int i = 0, e = (int)m_UnpackedMask.Count(); i < e; i ++ )
			{
				val = m_UnpackedMask[ i ];

				UINT8& dominantValue = (*dominantLayerValues)[ i ];

				if( b > 17 )
				{
					dominantValue = b;
					(*dominantLayerData)[ i ] = baseLayer + 1;
				}

				int g2 = g / 2;

				if( g2 > 17 )
				{
					dominantValue = g2;
					(*dominantLayerData)[ i ] = baseLayer + 2;
				}

				if( r > 17 )
				{
					dominantValue = r;
					(*dominantLayerData)[ i ] = baseLayer + 3;
				}
			}
		}

#ifndef FINAL_BUILD
		if( g_bEditMode && !g_FoundEmptyMask && bCheckEmpty )
		{
			int found = 0;

			for( int i = 0, e = (int)m_UnpackedMask.Count(); i < e; i ++ )
			{
				if( m_UnpackedMask[ i ] )
				{
					found = 1;
					break;
				}
			}

			if( !found )
			{
				g_FoundEmptyMask = 1;
			}
		}
#endif

		MegaTexTileInfo* info = GetMegaTexTileInfo( targetTile );

		int layerCount = info->LayerList.Count();

		targetTile->LayerBitMasks.Resize( R3D_MAX( layerCount, (int)targetTile->LayerBitMasks.Count() ) );

		targetTile->ActiveBitMasks = layerCount;

		MegaTexTileInfo::LayerListArr::Iterator iter = info->LayerList.GetIterator();

		int numBitMips = r3dLog2( qs.MaskAtlasTileDim ) + 1;

		float padding = 1.f / GetMaskPaddingCoef();

		for( ; iter.IsValid(); iter.Advance() )
		{
			int val = iter.GetValue();

			if( val / LAYERS_PER_MASK == expectedMaskID )
			{
				int layerBitMaskArrIdx = -1;

				for( int i = 0, e = targetTile->ActiveBitMasks; i < e; i ++ )
				{
					int bitLayerIndex = targetTile->LayerBitMasks[ i ].LayerIndex;

					if( bitLayerIndex < 0 || bitLayerIndex == val )
					{
						layerBitMaskArrIdx = i;
						break;
					}
				}

				r3d_assert( layerBitMaskArrIdx >= 0 );

				int idx = val % LAYERS_PER_MASK;

				MegaTexTile::LayerBitMaskEntry& entry = targetTile->LayerBitMasks[ layerBitMaskArrIdx ];
				entry.LayerIndex = val;
				entry.Bits.Resize( numBitMips );

				r3dBitMaskArray2D& baseMask = entry.Bits[ 0 ];

				baseMask.Resize( qs.MaskAtlasTileDim, qs.MaskAtlasTileDim );
				baseMask.ZeroAll();

				int halfBaseDim = qs.MaskAtlasTileDim / 2;

				enum
				{
					OFFSET_PAD = MEGA_TEX_BORDER / 2
				};

				switch( idx )
				{
				case 0:
					for( int z = 0, e = baseMask.GetHeight(); z < e; z ++ )
					{
						for( int x = 0, e = baseMask.GetWidth(); x < e; x ++ )
						{
							int xx = int( ( x - halfBaseDim ) * padding + halfBaseDim );
							int zz = int( ( z - halfBaseDim ) * padding + halfBaseDim );

							for( int zzz = zz - OFFSET_PAD; zzz <= zz + OFFSET_PAD; zzz ++ )
							{
								for( int xxx = xx - OFFSET_PAD; xxx <= xx + OFFSET_PAD; xxx ++ )
								{
									int idx = xxx + zzz * qs.MaskAtlasTileDim;

									if( ( m_UnpackedMask[ idx ] >> 11 ) & 0x1f )
									{
										baseMask.Set( x, z, 1 );
									}
								}
							}
						}
					}
					break;
				case 1:
					for( int z = 0, e = baseMask.GetHeight(); z < e; z ++ )
					{
						for( int x = 0, e = baseMask.GetWidth(); x < e; x ++ )
						{
							int xx = int( ( x - halfBaseDim ) * padding + halfBaseDim );
							int zz = int( ( z - halfBaseDim ) * padding + halfBaseDim );

							for( int zzz = zz - OFFSET_PAD; zzz <= zz + OFFSET_PAD; zzz ++ )
							{
								for( int xxx = xx - OFFSET_PAD; xxx <= xx + OFFSET_PAD; xxx ++ )
								{
									int idx = xxx + zzz * qs.MaskAtlasTileDim;

									if( ( m_UnpackedMask[ idx ] >> 5 ) & 0x3f )
									{
										baseMask.Set( x, z, 1 );
									}
								}
							}
						}
					}
					break;
				case 2:
					for( int z = 0, e = baseMask.GetHeight(); z < e; z ++ )
					{
						for( int x = 0, e = baseMask.GetWidth(); x < e; x ++ )
						{
							int xx = int( ( x - halfBaseDim ) * padding + halfBaseDim );
							int zz = int( ( z - halfBaseDim ) * padding + halfBaseDim );

							for( int zzz = zz - OFFSET_PAD; zzz <= zz + OFFSET_PAD; zzz ++ )
							{
								for( int xxx = xx - OFFSET_PAD; xxx <= xx + OFFSET_PAD; xxx ++ )
								{
									int idx = xxx + zzz * qs.MaskAtlasTileDim;

									if( m_UnpackedMask[ idx ] & 0x1f )
									{
										baseMask.Set( x, z, 1 );
									}
								}
							}
						}
					}
					break;
				default:
					r3d_assert( false );
				}

				for( int i = 1; i < numBitMips; i ++ )
				{
					r3dBitMaskArray2D& prev = entry.Bits[ i - 1 ];
					r3dBitMaskArray2D& curr = entry.Bits[ i ];

					curr.Resize( prev.GetWidth() / 2, prev.GetHeight() / 2 );

					for( int z = 0, e = (int)curr.GetHeight(); z < e; z ++ )
					{
						for( int x = 0, e = (int)curr.GetWidth(); x < e; x ++ )
						{
							int x2 = x * 2;
							int z2 = z * 2;

							bool isSet = prev.Get( x2 + 0, z2 + 0 );

							isSet |= prev.Get( x2 + 1, z2 + 0 );
							isSet |= prev.Get( x2 + 0, z2 + 1 );
							isSet |= prev.Get( x2 + 1, z2 + 1 );

							if( isSet )
								curr.Set( x, z, true );
						}
					}
				}
			}
		}
	}

	targetMask.UnlockRect( 0 );
}

template < typename T >
void r3dTerrain3::DoLoadMegaTexTileHeightAndNormalFromFile( T file, INT64 offset, MegaTexTile* tile, UINT16 *targetNormal, UINT16* targetHeight, int loadFlags )
{
	const QualitySettings& qs = GetCurrentQualitySettings();

	fseek( file, (long)offset, SEEK_SET );

	char NH_Sig[ sizeof MEGA_HN_SIG ];

	fread( NH_Sig, sizeof NH_Sig, 1, file );

	r3d_assert( !memcmp( NH_Sig, MEGA_HN_SIG, sizeof MEGA_HN_SIG ) );

	// Height
	if( loadFlags & LOADTILE_HEIGHT )
	{
		int heighSizeInFile = GetHeightTileSizeInFile();

		fread( targetHeight, heighSizeInFile, 1, file );

		r3d_assert( heighSizeInFile / sizeof tile->HeightData[ 0 ] == qs.HeightNormalAtlasTileDim * qs.HeightNormalAtlasTileDim );

		tile->HeightData.Resize( heighSizeInFile / sizeof tile->HeightData[ 0 ] );

		memcpy( &tile->HeightData[ 0 ], targetHeight, tile->HeightData.Count() * sizeof tile->HeightData[ 0 ] );

		tile->BorderData.Resize( qs.HeightNormalAtlasTileDim * 4 + ( qs.HeightNormalAtlasTileDim - 2 ) * 4 );

		int ti = 0;

		//------------------------------------------------------------------------
		// OUTER
		// X -
		for( int i = 0, e = qs.HeightNormalAtlasTileDim; i < e; i ++, ti ++ )
		{
			tile->BorderData[ ti ] = targetHeight[ i * qs.HeightNormalAtlasTileDim ];
		}
		// X +
		for( int i = 0, e = qs.HeightNormalAtlasTileDim; i < e; i ++, ti ++ )
		{
			tile->BorderData[ ti ] = targetHeight[ i * qs.HeightNormalAtlasTileDim + qs.HeightNormalAtlasTileDim - 1 ];
		}
		// Z -
		for( int i = 0, e = qs.HeightNormalAtlasTileDim; i < e; i ++, ti ++ )
		{
			tile->BorderData[ ti ] = targetHeight[ i ];
		}

		// Z +
		for( int i = 0, e = qs.HeightNormalAtlasTileDim; i < e; i ++, ti ++ )
		{
			tile->BorderData[ ti ] = targetHeight[ i + qs.HeightNormalAtlasTileDim * ( qs.HeightNormalAtlasTileDim - 1 ) ];
		}
		//------------------------------------------------------------------------
		// INNER
		// X -
		for( int i = 1, e = qs.HeightNormalAtlasTileDim - 1; i < e; i ++, ti ++ )
		{
			tile->BorderData[ ti ] = targetHeight[ i * qs.HeightNormalAtlasTileDim + 1];
		}

		// X +
		for( int i = 1, e = qs.HeightNormalAtlasTileDim - 1; i < e; i ++, ti ++ )
		{
			tile->BorderData[ ti ] = targetHeight[ i * qs.HeightNormalAtlasTileDim + qs.HeightNormalAtlasTileDim - 2 ];
		}

		// Z -
		for( int i = 1, e = qs.HeightNormalAtlasTileDim - 1; i < e; i ++, ti ++ )
		{
			tile->BorderData[ ti ] = targetHeight[ i + qs.HeightNormalAtlasTileDim ];
		}

		// Z +
		for( int i = 1, e = qs.HeightNormalAtlasTileDim - 1; i < e; i ++, ti ++ )
		{
			tile->BorderData[ ti ] = targetHeight[ i + qs.HeightNormalAtlasTileDim * ( qs.HeightNormalAtlasTileDim - 2 ) ];
		}

		UpdateMegaTexTileCompressedTileInfoMipChain( tile, (UINT16*)targetHeight );
	}
	else
	{
		fseek( file, GetHeightTileSizeInFile(), SEEK_CUR );
	}

	// Normal
	if( loadFlags & LOADTILE_NORMAL )
	{
		fread( targetNormal, GetNormalTileSizeInFile(), 1, file );
	}
	else
	{
		fseek( file, GetNormalTileSizeInFile(), SEEK_CUR );
	}
}

void r3dTerrain3::UpdateHeightNormalAtlasTile( MegaTexTile* tile, r3dD3DTextureTunnel targetNormal, r3dD3DTextureTunnel targetHeight )
{
	UpdateSurfaceParams usparams[ 2 ];

	usparams[ 0 ].source = targetNormal;
	usparams[ 0 ].dest = m_HeightNormalAtlas[ tile->HeightNormalAtlasVolumeId ].Normal->GetD3DTunnel();

	int2 res = GetHeightNormalAtlasXZ( tile->HeightNormalAtlasTileId );

	usparams[ 0 ].destX = res.x * m_QualitySettings.HeightNormalAtlasTileDim;
	usparams[ 0 ].destY = res.y * m_QualitySettings.HeightNormalAtlasTileDim;

	usparams[ 1 ] = usparams[ 0 ];

	if( targetNormal.Valid() )
	{
		if( targetHeight.Valid() )
			AddCustomDeviceQueueItem( UpdateSurface_MainThread, &usparams[ 0 ] );
		else
			ProcessCustomDeviceQueueItem( UpdateSurface_MainThread, &usparams[ 0 ] );
	}

	usparams[ 1 ].source = targetHeight;
	usparams[ 1 ].dest = m_HeightNormalAtlas[ tile->HeightNormalAtlasVolumeId ].Height->GetD3DTunnel();

	if( targetHeight.Valid() )
	{
		ProcessCustomDeviceQueueItem( UpdateSurface_MainThread, &usparams[ 1 ] );
	}
}

void r3dTerrain3::DoLoadMegaTexTile( MegaTexTile* tile, int loadFlags )
{
#ifndef FINAL_BUILD
	for( ; g_TerrainLoadingFrozen ; )
	{
		Sleep( 1 );

		if( g_AdvanceTerrain3Loading )
		{
			g_AdvanceTerrain3Loading = 0;
			break;
		}
	}
#endif

	int needCheckEmpty = 0;

#ifndef FINAL_BUILD
	if( !tile->L )
		needCheckEmpty = 1;
#endif

	if( tile->IsLoaded && !( loadFlags & LOADTILE_DISABLEFILTER ) )
		return;
#ifndef FINAL_BUILD
	r3dCSHolderWithDeviceQueue cs( m_MegaTexFileCritSec ); ( void )cs;
#endif

	if( loadFlags & LOADTILE_MASKS )
	{
#ifndef FINAL_BUILD
		r3dCSHolderWithDeviceQueue cs( m_MegaTexTileInfoCritSec ); ( void )cs;
#endif

		for( int i = 0, e = (int)tile->LayerBitMasks.Count(); i < e; i ++ )
		{
			tile->LayerBitMasks[ i ].LayerIndex = -1;
		}

		int size = m_QualitySettings.VertexTileDim * m_QualitySettings.VertexTilesInMegaTexTileCount << tile->L;
		int mipOffset = r3dLog2( m_QualitySettings.VertexTilesInMegaTexTileCount );

		int mip = tile->L + mipOffset;

		D3DSURFACE_DESC maskDesc;
		m_MegaTexTile_TempMask.GetLevelDesc2D( 0, &maskDesc );

		MegaTexTile::MaskEntryList::Iterator iter = tile->MaskList.GetIterator();

		int maskIdx = 0;

		int maskCount = GetMaskCount();

		g_DominantLayerData.Clear();
		g_DominantLayerData.Resize( m_QualitySettings.MaskAtlasTileDim * m_QualitySettings.MaskAtlasTileDim, 0 );
		g_DominantLayerValues.Clear();
		g_DominantLayerValues.Resize( m_QualitySettings.MaskAtlasTileDim * m_QualitySettings.MaskAtlasTileDim, 0 );

		int needDominantData = tile->L <= DOMINANT_DATA_MAX_LOD;

		for( int i = 0 ; i < maskCount; i ++, maskIdx ++ )
		{
			int needMask = 0;

			{
				MegaTexTile::MaskEntryList::Iterator findIter = tile->MaskList.GetIterator();

				for( ; findIter.IsValid(); findIter.Advance() )
				{
					int atlasId, tileId, maskId;
					MegaTexTile::DecodeMaskEntry( findIter.GetValue(), &atlasId, &tileId, &maskId );
					if( maskId == maskIdx )
					{
						needMask = 1;
						break;
					}
				}
			}

			int maskIndex = i;

			if( needMask )
			{
				MegaTexFileGridOffset offset = GetMegaTexMaskOffsetInFile( tile, maskIdx );

#ifndef FINAL_BUILD
				if( offset.IsInEditorFile )
				{
					int mapSize = GetMaskTileSizeInFile() + sizeof( short );
					r3dMappedViewOfFile mappedViewOfFile = r3dCreateMappedViewOfFile( m_MegaTexAtlasFile_EDITOR_Mapping, FILE_MAP_READ, offset.EditorValue, mapSize );
					DoLoadMegaTexTileMaskFromFile( &mappedViewOfFile, 0, &maskDesc, m_MegaTexTile_TempMask, maskIdx, tile, 
													needDominantData ? &g_DominantLayerData : NULL, 
													needDominantData ? &g_DominantLayerValues : NULL,
													needCheckEmpty );
					UnmapViewOfFile( mappedViewOfFile.Mapped );
				}
				else
				{
					Bytes* dominantLayerData = needDominantData ? &g_DominantLayerData : NULL;
					Bytes* dominantLayerValues = needDominantData ? &g_DominantLayerValues : NULL;

					if( offset.GridId == GRID_ID_MIPSFILE )
					{
						DoLoadMegaTexTileMaskFromFile( m_MegaTexAtlasMipsFile, offset.Value, &maskDesc, m_MegaTexTile_TempMask, maskIdx, tile,
														dominantLayerData, dominantLayerValues, needCheckEmpty );
					}
					else
					{
						R3D_FILE_SYSTEM_LOCK_SCOPE()
						int gridX, gridZ;
						GetGridCoordinates( offset, &gridX, &gridZ );
						DoLoadMegaTexTileMaskFromFile( m_FileGrid[ gridZ ][ gridX ], offset.Value, &maskDesc, m_MegaTexTile_TempMask, maskIdx, tile,
														dominantLayerData, dominantLayerValues, needCheckEmpty );

					}
				}
#else
				{
					R3D_FILE_SYSTEM_LOCK_SCOPE();

					if( offset.GridId == GRID_ID_MIPSFILE )
					{
						DoLoadMegaTexTileMaskFromFile(	m_MegaTexAtlasMipsFile, offset.Value, &maskDesc, m_MegaTexTile_TempMask, maskIdx, tile,
														needDominantData ? &g_DominantLayerData : NULL,
														needDominantData ? &g_DominantLayerValues : NULL, needCheckEmpty );
					}
					else
					{
						int gridX, gridZ;
						GetGridCoordinates( offset, &gridX, &gridZ );
						DoLoadMegaTexTileMaskFromFile(	m_FileGrid[ gridZ ][ gridX ], offset.Value, &maskDesc, m_MegaTexTile_TempMask, maskIdx, tile,
														needDominantData ? &g_DominantLayerData : NULL,
														needDominantData ? &g_DominantLayerValues : NULL, needCheckEmpty );
					}
				}
#endif

				int maskVol, maskTile, maskId;

				if( loadFlags & LOADTILE_UPDATEATLAS )
				{
					MegaTexTile::DecodeMaskEntry( iter.GetValue(), &maskVol, &maskTile, &maskId );

					r3d_assert( maskId == maskIndex );

					UpdateSurfaceParams usparams;

					usparams.source = m_MegaTexTile_TempMask;
					usparams.dest = m_MaskAtlas[ maskVol ].Mask->GetD3DTunnel();

					int2 res = GetMaskTileAtlasXZ( maskTile );

					usparams.destX = res.x * m_QualitySettings.MaskAtlasTileDim;
					usparams.destY = res.y * m_QualitySettings.MaskAtlasTileDim;

					ProcessCustomDeviceQueueItem( UpdateSurface_MainThread, &usparams );
				}

				iter.Advance();
			}
		}

		if( needDominantData )
		{
			int div = m_QualitySettings.DominantLayerDataDiv;
			int side = m_QualitySettings.MaskAtlasTileDim;

			tile->DominantLayerData.Clear();
			tile->DominantLayerData.Resize( side * side / div / div, 0 );

			int dominantSize = side / div;

			for( int z = 0, e = dominantSize; z < e; z ++ )
			{
				for( int x = 0; x < e; x ++ )
				{
					int maxLayerVal = 0;
					int maxLayerIndex = 0;

					for( int sz = 0, e = div; sz < e; sz ++ )
					{
						for( int sx = 0; sx < e; sx ++ )
						{
							int idx = x * div + sx + ( z * div + sz ) * side;

							if( g_DominantLayerValues[ idx ] > maxLayerVal )
							{
								maxLayerVal = g_DominantLayerValues[ idx ];
								maxLayerIndex = g_DominantLayerData[ idx ];
							}
						}
					}

					tile->DominantLayerData[ x + z * dominantSize ] = maxLayerIndex;
				}
			}

		}

		for( int i = 0, e = (int)tile->ActiveBitMasks; i < e; i ++ )
		{
			MegaTexTile::LayerBitMaskEntry& entry = tile->LayerBitMasks[ i ];
			r3d_assert( entry.LayerIndex != -1 );
		}
	}

	MegaTexFileGridOffset offset = GetMegaTexHeightNormalOffsetInFile( tile );

	D3DLOCKED_RECT tempHeightLockedRect, tempNormalLockedRect;

	tempHeightLockedRect.pBits = NULL;
	tempNormalLockedRect.pBits = NULL;

	if( loadFlags & LOADTILE_HEIGHT )
	{
		m_MegaTexTile_TempHeight.LockRect( 0, &tempHeightLockedRect, NULL, 0 );
	}
	if( loadFlags & LOADTILE_NORMAL )
	{
		m_MegaTexTile_TempNormal.LockRect( 0, &tempNormalLockedRect, NULL, 0 );
	}

#ifndef FINAL_BUILD
	if( offset.IsInEditorFile )
	{
		int mapSize = GetHeightNormalRecordSizeInFile();
		r3dMappedViewOfFile mappedViewOfFile = r3dCreateMappedViewOfFile( m_MegaTexAtlasFile_EDITOR_Mapping, FILE_MAP_READ, offset.EditorValue, mapSize );
		DoLoadMegaTexTileHeightAndNormalFromFile( &mappedViewOfFile, 0, tile, (UINT16*)tempNormalLockedRect.pBits, (UINT16*)tempHeightLockedRect.pBits, loadFlags );
		UnmapViewOfFile( mappedViewOfFile.Mapped );
	}
	else
#endif
	{
		if( offset.GridId == GRID_ID_MIPSFILE )
		{
			DoLoadMegaTexTileHeightAndNormalFromFile( m_MegaTexAtlasMipsFile, offset.Value, tile, (UINT16*)tempNormalLockedRect.pBits, (UINT16*)tempHeightLockedRect.pBits, loadFlags );
		}
		else
		{
			R3D_FILE_SYSTEM_LOCK_SCOPE();

			int gridX, gridZ;
			GetGridCoordinates( offset, &gridX, &gridZ );
			DoLoadMegaTexTileHeightAndNormalFromFile( m_FileGrid[ gridZ ][ gridX ], offset.Value, tile, (UINT16*)tempNormalLockedRect.pBits, (UINT16*)tempHeightLockedRect.pBits, loadFlags );
		}
	}

	if( loadFlags & LOADTILE_NORMAL )
	{
		m_MegaTexTile_TempNormal.UnlockRect( 0 );
	}

	if( loadFlags & LOADTILE_HEIGHT )
	{
		m_MegaTexTile_TempHeight.UnlockRect( 0 );
	}

	// update what we read - in the very main thread
	if( loadFlags & LOADTILE_UPDATEATLAS )
	{
		r3dD3DTextureTunnel normal;
		r3dD3DTextureTunnel height;

		int needUpdate = 0;

		if( loadFlags & LOADTILE_HEIGHT )
		{
			height = m_MegaTexTile_TempHeight;
			needUpdate = 1;
		}

		if( loadFlags & LOADTILE_NORMAL )
		{
			normal = m_MegaTexTile_TempNormal;
			needUpdate = 1;
		}

		if( needUpdate )
		{
			UpdateHeightNormalAtlasTile( tile, normal, height );
		}
	}

	if( !( loadFlags & LOADTILE_DISABLEFILTER ) )
	{
		r3d_assert( !tile->IsLoaded );
	}

	if( loadFlags & LOADTILE_UPDATEROADS )
	{
		UpdateRoadInfo( tile );
	}

	tile->IsLoaded = 1;

	// do not 'dirty' on height only change
	if( loadFlags & ( LOADTILE_NORMAL | LOADTILE_MASKS ) )
	{
		tile->Dirty = 1;
	}

#ifndef FINAL_BUILD
	InterlockedExchange( &tile->IsLoading, 0 );
#endif

}

//------------------------------------------------------------------------

const char MEGA_TEX_SIG_101[  ] = "MEGATEX101";
const char MEGA_TEX_SIG[  ] = "MEGATEX102";
const char MEGA_TEX_INFO_SIG[  ] = "MEGATEXINFO100";

#define R3D_MEGATEX_PATH_BASE "\\Terrain3\\megatex"
#define R3D_MEGATEX_PATH_INFO "\\Terrain3\\megatexinfo.bin"
#define R3D_MEGATEX_PATH_EXT "bin"
#define R3D_MEGATEX_EDIT_PATH "\\megatex_edit.bin"

int r3dTerrain3::LoadMegaTexFile_OLD( const char* levelPath )
{
	char fullPath[ 1024 ];

	strcpy( fullPath, levelPath );
	strcat( fullPath, R3D_MEGATEX_PATH_BASE );

	if( m_MegaTexAtlasFile )
	{
		fclose( m_MegaTexAtlasFile );
		m_MegaTexAtlasFile = NULL;
	}

#ifndef FINAL_BUILD
	if( g_bEditMode )
	{
		char checkOldPath[ 1024 ];
		strcpy( checkOldPath, levelPath );
		strcat( checkOldPath, R3D_MEGATEX_PATH_BASE );
		strcat( checkOldPath, "." );
		strcat( checkOldPath, R3D_MEGATEX_PATH_EXT );

		if( r3dFileExists( checkOldPath ) )
		{
			SplitMegaTexFile( levelPath, checkOldPath );
			remove( checkOldPath );
		}
	}
#endif

	m_MegaTexAtlasFile = r3d_open_chunked( fullPath, R3D_MEGATEX_PATH_EXT, R3D_MEGATEX_CHUNK_SIZE );

	struct OnFail
	{
		~OnFail()
		{
			if( file )
			{
				fclose( *file );
				*file = 0;
			}
		}
		r3dChunkedFile ** file;
	} onFail = { &m_MegaTexAtlasFile };

	char sig[ sizeof MEGA_TEX_SIG ];

	if( !fread( sig, sizeof sig, 1, m_MegaTexAtlasFile ) )
	{
		return 0;
	}

	if( memcmp( sig, MEGA_TEX_SIG, sizeof sig ) )
	{
		r3dError( "Unsupported Terrain3 Version!" );
		return 0;
	}

	if( !fread( &m_MegaTexAtlasFile_NormalDim, sizeof m_MegaTexAtlasFile_NormalDim, 1, m_MegaTexAtlasFile ) ) return 0;
	if( !fread( &m_MegaTexAtlasFile_HeightDim, sizeof m_MegaTexAtlasFile_HeightDim, 1, m_MegaTexAtlasFile ) ) return 0;
	if( !fread( &m_MegaTexAtlasFile_MaskDim, sizeof m_MegaTexAtlasFile_MaskDim, 1, m_MegaTexAtlasFile ) ) return 0;
	if( !fread( &m_MegaTexAtlasFile_BaseTileCountX, sizeof m_MegaTexAtlasFile_BaseTileCountX, 1, m_MegaTexAtlasFile ) ) return 0;
	if( !fread( &m_MegaTexAtlasFile_BaseTileCountZ, sizeof m_MegaTexAtlasFile_BaseTileCountZ, 1, m_MegaTexAtlasFile ) ) return 0;

	if( !fread( &m_MegaTexAtlasFile_NormalFormat, sizeof m_MegaTexAtlasFile_NormalFormat, 1, m_MegaTexAtlasFile ) ) return 0;
	if( !fread( &m_MegaTexAtlasFile_HeightFormat, sizeof m_MegaTexAtlasFile_HeightFormat, 1, m_MegaTexAtlasFile ) ) return 0;
	if( !fread( &m_MegaTexAtlasFile_MaskFormat, sizeof m_MegaTexAtlasFile_MaskFormat, 1, m_MegaTexAtlasFile ) ) return 0;
	if( !fread( &m_MegaTexAtlasFile_ColorModulationFormat, sizeof m_MegaTexAtlasFile_ColorModulationFormat, 1, m_MegaTexAtlasFile ) ) return 0;	

	if( !fread( &m_MegaTexAtlasFile_NumActiveLayers, sizeof m_MegaTexAtlasFile_NumActiveLayers, 1, m_MegaTexAtlasFile ) ) return 0;

	m_MegaTexAtlasFile_HeightNormalOffset = ftell( m_MegaTexAtlasFile );

	r3d_assert( m_MegaTexAtlasFile_NumActiveLayers == m_NumActiveMegaTexQualityLayers );
	
	// leave the file open - we need it for background loading
	onFail.file = 0;

	return 1;
}

//------------------------------------------------------------------------

void r3dTerrain3::CloseEditorFile()
{
	if( m_MegaTexAtlasFile_EDITOR != INVALID_HANDLE_VALUE )
	{
		CloseHandle( m_MegaTexAtlasFile_EDITOR );
		m_MegaTexAtlasFile_EDITOR = INVALID_HANDLE_VALUE;
	}

	if( m_MegaTexAtlasFile_EDITOR_Mapping != INVALID_HANDLE_VALUE )
	{
		CloseHandle( m_MegaTexAtlasFile_EDITOR_Mapping );
		m_MegaTexAtlasFile_EDITOR_Mapping = INVALID_HANDLE_VALUE;
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::CloseMegaTexGridFiles()
{
	if( m_FileGrid.Count() )
	{
		for( int z = 0, e = m_FileGrid.Height(); z < e; z ++ )
		{
			for( int x = 0, e = m_FileGrid.Width(); x < e; x ++ )
			{
				fclose( m_FileGrid[ z ][ x ] );
			}
		}
	}

	m_FileGrid.Clear();
}

//------------------------------------------------------------------------

void r3dTerrain3::GetEditorFilePath( char ( &oPath ) [ 1024 ] )
{
	GetTempPath( sizeof oPath - 1, oPath );

	strcat( oPath, R3D_MEGATEX_EDIT_PATH );
}

//------------------------------------------------------------------------

void r3dTerrain3::OpenEditorFile()
{
	CloseEditorFile();

	char fullPath[ 1024 ];

	GetEditorFilePath( fullPath );

	g_TempEditFilePath = fullPath;

	m_MegaTexAtlasFile_EDITOR = CreateFile( fullPath, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	r3d_assert( m_MegaTexAtlasFile_EDITOR && m_MegaTexAtlasFile_EDITOR != INVALID_HANDLE_VALUE );

#define R3D_TERRAEDIT_FILEMAPPING "Local\\TerraEdit_FileMapping"

	m_MegaTexAtlasFile_EDITOR_Mapping = CreateFileMapping( m_MegaTexAtlasFile_EDITOR, NULL, PAGE_READWRITE, 0, 1024 * 1024 * 1024, R3D_TERRAEDIT_FILEMAPPING );
	r3d_assert( m_MegaTexAtlasFile_EDITOR_Mapping && m_MegaTexAtlasFile_EDITOR_Mapping != INVALID_HANDLE_VALUE );
}

//------------------------------------------------------------------------

void r3dTerrain3::OpenMegaTexGridFiles( const char* levelPath )
{
	CloseMegaTexGridFiles();

	m_FileGrid.Resize( m_FileChunkCountX, m_FileChunkCountZ );

	for( int z = 0, e = m_FileGrid.Height(); z < e; z ++ )
	{
		for( int x = 0, e = m_FileGrid.Width(); x < e; x ++ )
		{
			char fileBuff[ 512 ];
			GetChunkFileName( fileBuff, x, z );

#ifndef FINAL_BUILD
			m_FileGrid[ z ][ x ] = fopen( fileBuff, "rb" );
#else
			m_FileGrid[ z ][ x ] = r3d_open_unpacked( fileBuff );
#endif
		}
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::ReadDistribStamp( const char* levelPath, Bytes* oBytes )
{
	char buffer[ 512 ];

	GetDistribStampName( levelPath, buffer );

	r3dFile * stamp = r3d_open( buffer, "rb" );

	if( stamp )
	{
		fseek( stamp, 0, SEEK_END );
		size_t size = ftell( stamp );
		fseek( stamp, 0, SEEK_SET );

		oBytes->Resize( size );
		fread( &(*oBytes)[ 0 ], oBytes->Count(), 1, stamp );

		fclose( stamp );
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::UpdateMegaTexMipsFile( const char* levelPath )
{
	char buffer[ 512 ];
	GetMipFileName( buffer );

	int needCreate = 0;

	if( !r3dFileExists( buffer ) )
	{
		needCreate = 1;
	}
	else
	{
		GetMipFileStampName( buffer );

		if( !r3dFileExists( buffer ) )
		{
			needCreate = 1;
		}
		else
		{
			r3dFile* stamp = r3d_open( buffer, "rb" );
			fseek( stamp, 0, SEEK_END );
			int size = ftell( stamp );
			fseek( stamp, 0, SEEK_SET );

			Bytes bytes0;
			bytes0.Resize( size );

			if( bytes0.Count() )
			{
				fread( &bytes0[ 0 ], bytes0.Count(), 1, stamp );
			}

			fclose( stamp );

			Bytes bytes1;
			ReadDistribStamp( levelPath, &bytes1 );

			if( bytes1.Count() )
			{
				int stampId = 0;
				if( _snscanf( (char*)&bytes1[ 0 ], 8, "%d", &stampId ) == 1 )
				{
					m_StampID = stampId;
				}
			}

			if(	!bytes1.Count() || !bytes0.Count() ||
				bytes0.Count() != bytes1.Count() || 
				memcmp( &bytes0[ 0 ], &bytes1[ 0 ], bytes0.Count() ) )
				needCreate = 1;
		}
	}

	if( needCreate )
	{
		CreateMipFile( levelPath );
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::DeleteMegaTexAtlasMipsFile()
{
	char buffer[ 512 ];
	GetMipFileName( buffer );

	remove( buffer );
}

//------------------------------------------------------------------------

void r3dTerrain3::OpenMegaTexAtlasMipsFile( const char* accessMode )
{
	char buffer[ 512 ];
	GetMipFileName( buffer );

	m_MegaTexAtlasMipsFile = fopen( buffer, accessMode );
}

//------------------------------------------------------------------------

int r3dTerrain3::LoadMegaTexFile( const char* levelPath )
{

	char fullPath[ 1024 ];

	strcpy( fullPath, levelPath );
	strcat( fullPath, R3D_MEGATEX_PATH_INFO );

	r3dFile* megaTexInfoFile = r3d_open( fullPath, "rb" );

	struct OnExit
	{
		~OnExit()
		{
			fclose( file );
		}
		r3dFile *file;
	} onExit = { megaTexInfoFile }; (void)onExit;

	char sig[ sizeof MEGA_TEX_INFO_SIG ];

	if( !fread( sig, sizeof sig, 1, megaTexInfoFile ) )
	{
		return 0;
	}

	if( memcmp( sig, MEGA_TEX_INFO_SIG, sizeof sig ) )
	{
		r3dError( "Unsupported Terrain3 Version!" );
		return 0;
	}

	if( !fread( &m_MegaTexAtlasFile_NormalDim, sizeof m_MegaTexAtlasFile_NormalDim, 1, megaTexInfoFile ) ) return 0;
	if( !fread( &m_MegaTexAtlasFile_HeightDim, sizeof m_MegaTexAtlasFile_HeightDim, 1, megaTexInfoFile ) ) return 0;
	if( !fread( &m_MegaTexAtlasFile_MaskDim, sizeof m_MegaTexAtlasFile_MaskDim, 1, megaTexInfoFile ) ) return 0;
	if( !fread( &m_MegaTexAtlasFile_BaseTileCountX, sizeof m_MegaTexAtlasFile_BaseTileCountX, 1, megaTexInfoFile ) ) return 0;
	if( !fread( &m_MegaTexAtlasFile_BaseTileCountZ, sizeof m_MegaTexAtlasFile_BaseTileCountZ, 1, megaTexInfoFile ) ) return 0;

	if( !fread( &m_MegaTexAtlasFile_NormalFormat, sizeof m_MegaTexAtlasFile_NormalFormat, 1, megaTexInfoFile ) ) return 0;
	if( !fread( &m_MegaTexAtlasFile_HeightFormat, sizeof m_MegaTexAtlasFile_HeightFormat, 1, megaTexInfoFile ) ) return 0;
	if( !fread( &m_MegaTexAtlasFile_MaskFormat, sizeof m_MegaTexAtlasFile_MaskFormat, 1, megaTexInfoFile ) ) return 0;
	if( !fread( &m_MegaTexAtlasFile_ColorModulationFormat, sizeof m_MegaTexAtlasFile_ColorModulationFormat, 1, megaTexInfoFile ) ) return 0;	

	if( !fread( &m_MegaTexAtlasFile_NumActiveLayers, sizeof m_MegaTexAtlasFile_NumActiveLayers, 1, megaTexInfoFile ) ) return 0;

	m_MegaTexAtlasFile_HeightNormalOffset = ftell( megaTexInfoFile );

	r3d_assert( m_MegaTexAtlasFile_NumActiveLayers == m_NumActiveMegaTexQualityLayers );

#if !defined(FINAL_BUILD) && R3D_TERRAIN_V3_GRAPHICS
	if( g_bEditMode )
	{
		OpenEditorFile();
	}
#endif

	//------------------------------------------------------------------------

	OpenMegaTexGridFiles( levelPath );

	m_MegaTexAtlasFile_HeightNormalOffset = 0;

	return 1;
}

//------------------------------------------------------------------------

void r3dTerrain3::CreateMegaTexFile( const char* levelPath, UINT16 initialHeight )
{
	MakeSaveDir( levelPath );

	char fullPath[ 1024 ];

	strcpy( fullPath, levelPath );
	strcat( fullPath, R3D_MEGATEX_PATH_BASE );

	CreateMegaTexInfo( levelPath );

	IDirect3DTexture9* flatNormals565( NULL );
	D3D_V( r3dRenderer->pd3ddev->CreateTexture( m_MegaTexAtlasFile_NormalDim, m_MegaTexAtlasFile_NormalDim, 1, 0, D3DFMT_R5G6B5, D3DPOOL_SYSTEMMEM, &flatNormals565, NULL ) );
	IDirect3DTexture9* flatNormalsFinal( NULL );
	D3D_V( r3dRenderer->pd3ddev->CreateTexture( m_MegaTexAtlasFile_NormalDim, m_MegaTexAtlasFile_NormalDim, 1, 0, D3DFORMAT( m_MegaTexAtlasFile_NormalFormat ), D3DPOOL_SYSTEMMEM, &flatNormalsFinal, NULL ) );

	{
		D3DSURFACE_DESC sdesc;
		D3DLOCKED_RECT lrect;

		D3D_V( flatNormals565->GetLevelDesc( 0, &sdesc ) );

		D3D_V( flatNormals565->LockRect( 0, &lrect, NULL, 0 ) );

		for( int y = 0, e = sdesc.Height; y < e; y ++  )
		{
			for( int x = 0, e = sdesc.Width; x < e; x ++ )
			{
				((WORD*)lrect.pBits)[ y * lrect.Pitch / sizeof ( WORD ) + x ] = 16 << 11 | 63 << 5 | 16;
			}
		}

		D3D_V( flatNormals565->UnlockRect( 0 ) );

		IDirect3DSurface9* src( NULL ), *dest( NULL );

		D3D_V( flatNormals565->GetSurfaceLevel( 0, &src ) );

		D3D_V( flatNormalsFinal->GetSurfaceLevel( 0, &dest ) );

		D3D_V( D3DXLoadSurfaceFromSurface( dest, NULL, NULL, src, NULL, NULL, D3DX_FILTER_NONE, 0 ) );

		SAFE_RELEASE( src );
		SAFE_RELEASE( dest );
	}

	SAFE_RELEASE( flatNormals565 );

	D3DSURFACE_DESC normDesc;
	flatNormalsFinal->GetLevelDesc( 0, &normDesc );

	D3DLOCKED_RECT normalLRect;
	D3D_V( flatNormalsFinal->LockRect( 0, &normalLRect, NULL, 0 ) );

	r3d_assert( normalLRect.Pitch == r3dGetTextureBlockLineSize( normDesc.Width, D3DFORMAT( m_MegaTexAtlasFile_NormalFormat ) ) );

	int heightSampleCount = m_MegaTexAtlasFile_HeightDim * m_MegaTexAtlasFile_HeightDim;
	int heightSize = heightSampleCount * sizeof( UINT16 );
	UINT16* heightBits = static_cast< UINT16* >( malloc( heightSize ) );

	for( int i = 0, e = heightSampleCount; i < e; i ++ )
	{
		heightBits[ i ] = initialHeight;
	}

	const r3dTerrainDesc& tdesc = GetDesc();

	float initialHeightF = initialHeight * ( tdesc.MaxHeight - tdesc.MinHeight ) + tdesc.MinHeight;

	int tileCountX = m_TileCountX / m_QualitySettings.VertexTilesInMegaTexTileCount;
	int tileCountZ = m_TileCountZ / m_QualitySettings.VertexTilesInMegaTexTileCount;
	{
		r3dTL::T2DArray< FILE* > fileGrid;

		fileGrid.Resize( m_FileChunkCountX, m_FileChunkCountZ );

		for( int z = 0, e = fileGrid.Height(); z < e; z ++ )
		{
			for( int x = 0, e = fileGrid.Width(); x < e; x ++ )
			{
				char buffer[ 512 ];
				GetChunkFileName( buffer, x, z );
				fileGrid[ z ][ x ] = fopen_for_write( buffer, "wb" );
			}
		}

		for( int tz = 0, e = tileCountZ; tz < e; tz ++ )
		{
			for( int tx = 0, e = tileCountX; tx < e; tx ++ )
			{
				int fileGridX = tx / m_TilesInFileChunk;
				int fileGridZ = tz / m_TilesInFileChunk;

				WriteHeightNormalTileInFile( fileGrid[ fileGridZ ][ fileGridX ], heightBits, normalLRect.pBits );
			}
		}

		for( int z = 0, e = fileGrid.Height(); z < e; z ++ )
		{
			for( int x = 0, e = fileGrid.Width(); x < e; x ++ )
			{
				fclose( fileGrid[ z ][ x ] );
			}
		}
	}

	free( heightBits );

	SAFE_RELEASE( flatNormalsFinal );

}

//------------------------------------------------------------------------

#define R3D_MEGATEXMAP_PATH_OLD "\\Terrain3\\megatex.map"
#define R3D_MEGATEXMAP_PATH "\\Terrain3\\megatex2.map"
const char MEGATEX_MAP_SIG100[] = "MEGAMAP100";

const char MEGATEX_MAP_CHUNKED_SIG[] = "MEGAMAPCHNK200";

void r3dTerrain3::SaveMegaTexMapFile( const char* levelPath )
{
	r3dTL::T2DArray< FILE* > fileGrid;
	r3dTL::T2DArray< int > elemCountGrid;

	fileGrid.Resize( m_FileChunkCountX, m_FileChunkCountZ );
	elemCountGrid.Resize( m_FileChunkCountX, m_FileChunkCountZ, 0 );

	int offsetForMapCount = 0;

	for( int z = 0, e = fileGrid.Height(); z < e; z ++ )
	{
		for( int x = 0, e = fileGrid.Width(); x < e; x ++ )
		{
			char buffer[ 512 ];
			GetMegaMapChunkFileName( buffer, x, z );

			FILE* fout = fopen_for_write( buffer, "wb" );

			fwrite( MEGATEX_MAP_CHUNKED_SIG, sizeof MEGATEX_MAP_CHUNKED_SIG, 1, fout );

			offsetForMapCount = ftell( fout );

			INT32 mapCount = m_MegaTexGridFile_Map.size();
			fwrite( &mapCount, sizeof mapCount, 1, fout );

			fileGrid[ z ][ x ] = fout;
		}
	}

	for( FileTextureGridMap::const_iterator i = m_MegaTexGridFile_Map.begin(), 
											e = m_MegaTexGridFile_Map.end(); i != e ; ++ i )
	{
		INT64 id = i->first;

		short tileX, tileZ;
		char tileL;

		int maskId;

		MegaTexTile::DecodeID( id, &tileX, &tileZ, &tileL, &maskId );

		int fileGridX = tileX / m_TilesInFileChunk;
		int fileGridZ = tileZ / m_TilesInFileChunk;

		FILE* fout = fileGrid[ fileGridZ ][ fileGridX ];

		elemCountGrid[ fileGridZ ][ fileGridX ] ++;

		fwrite( &id, sizeof id, 1, fout );

		const MegaTexFileGridOffsetBase& base = i->second;

		fwrite( &base, sizeof base, 1, fout );
	}

	for( int z = 0, e = fileGrid.Height(); z < e; z ++ )
	{
		for( int x = 0, e = fileGrid.Width(); x < e; x ++ )
		{
			FILE* fout = fileGrid[ z ][ x ];
			fseek( fout, offsetForMapCount, SEEK_SET );
			INT32 count = elemCountGrid[ z ][ x ];
			fwrite( &count, sizeof count, 1, fout );
			fclose( fout );
		}
	}
}

//------------------------------------------------------------------------

int r3dTerrain3::LoadMegaTexMapFile( const char* levelPath )
{
	r3dTL::T2DArray< r3dFile* > fileGrid;
	fileGrid.Resize( m_FileChunkCountX, m_FileChunkCountZ, NULL );

	int maskCount = GetMaskCount();

	struct OnExit
	{
		~OnExit()
		{
			r3dTL::T2DArray< r3dFile* >& fileGrid = *pFileGrid;

			for( int z = 0, e = (int)fileGrid.Height(); z < e; z ++ )
			{
				for( int x = 0, e = (int)fileGrid.Width(); x < e; x ++ )
				{
					fclose( fileGrid[ z ][ x ] );
				}
			}
		}

		r3dTL::T2DArray< r3dFile* > * pFileGrid;

	} onExit = { &fileGrid };

	for( int z = 0, e = (int)fileGrid.Height(); z < e; z ++ )
	{
		for( int x = 0, e = (int)fileGrid.Width(); x < e; x ++ )
		{
			char buffer[ 512 ];
			GetMegaMapChunkFileName( buffer, x, z );

			r3dFile* fin = r3d_open( buffer, "rb" );

			if( !fin )
			{
				r3dOutToLog( "r3dTerrain3::LoadMegaTexMapFile: missing megatex map chunk: %s\n", buffer );
				return 0;
			}

			char sig[ sizeof MEGATEX_MAP_CHUNKED_SIG ];

			if( !fread( sig, sizeof sig, 1, fin ) )
				return 0;

			if( memcmp( sig, MEGATEX_MAP_CHUNKED_SIG, sizeof sig ) )
			{
				r3dOutToLog( "r3dTerrain3::LoadMegaTexMapFile: bad file format in: %s\n", buffer );
				return 0;
			}

			fileGrid[ z ][ x ] = fin;
		}
	}

	m_MegaTexGridFile_Map.clear();

	int rogueMasksFound = 0;

	for( int z = 0, e = (int)fileGrid.Height(); z < e; z ++ )
	{
		for( int x = 0, e = (int)fileGrid.Width(); x < e; x ++ )
		{
			r3dFile* fin = fileGrid[ z ][ x ];

			INT32 mapCount( 0 );

			if( !fread( &mapCount, sizeof mapCount, 1, fin ) )
				return 0;

			for( INT32 i = 0, e = mapCount; i < e; i ++ )
			{
				INT64 id;

				if( !fread( &id, sizeof id, 1, fin ) )
					return 0;

				MegaTexFileGridOffsetBase baseOffset;

				if( !fread( &baseOffset, sizeof baseOffset, 1, fin ) )
					return 0;

				MegaTexFileGridOffset mtfo;

				(MegaTexFileGridOffsetBase&)mtfo = baseOffset;

				short x, z;
				char L;
				int m;
				MegaTexTile::DecodeID( id, &x, &z, &L, &m );

				if( m < maskCount )
				{
					m_MegaTexGridFile_Map.insert( FileTextureGridMap::value_type( id, mtfo ) );
				}
#ifndef FINAL_BUILD
				else
				{
					rogueMasksFound++;
				}
#endif
			}
		}
	}

#ifndef FINAL_BUILD
	if( rogueMasksFound )
	{
		r3dArtBug( "%d rogue masks found in terrain! Please resave level!\n", rogueMasksFound );
	}
#endif

	return 1;
}

//------------------------------------------------------------------------

int r3dTerrain3::LoadMegaTexMapFile_OLD( const char* levelPath )
{
	char fullPath[ 1024 ];

	strcpy( fullPath, levelPath );
	strcat( fullPath, R3D_MEGATEXMAP_PATH_OLD );

	r3dFile* fin = r3d_open( fullPath );

	if( !fin )
		return 0;

	struct OnExit
	{
		~OnExit()
		{
			fclose( file );
		}
		r3dFile * file;
	} onExit = { fin };

	char SIG[ sizeof MEGATEX_MAP_SIG100 ];
	if( !fread( SIG, sizeof SIG, 1, fin ) )
		return 0;

	if( memcmp( SIG, MEGATEX_MAP_SIG100, sizeof SIG ) )
	{
		r3dOutToLog( "r3dTerrain3::LoadMegaTexMapFile_OLD: unknown version signature %s!", SIG );
		return 0;
	}

	INT64 mapCount( 0 );

	if( !fread( &mapCount, sizeof mapCount, 1, fin ) )
		return 0;

	m_MegaTexAtlasFile_Map.clear();

	for( INT64 i = 0, e = mapCount; i < e; i ++ )
	{
		INT64 id;
		INT64 offset;

		if( !fread( &id, sizeof id, 1, fin ) )
			return 0;

		if( !fread( &offset, sizeof offset, 1, fin ) )
			return 0;

		MegaTexFileOffset mtfo;

		mtfo.Value = offset;

		m_MegaTexAtlasFile_Map.insert( FileTextureMap::value_type( id, mtfo ) );
	}

	return 1;
}

//------------------------------------------------------------------------

#define R3D_MEGATEXTILEINFOS_PATH "\\Terrain3\\megatileinfos.dat"

const char MEGATEXMEGATEXTILEINFOS_SIG100[] = "MEGATILEINFOS100";
const char MEGATEXMEGATEXTILEINFOS_SIG101[] = "MEGATILEINFOS101";
const char MEGATEXMEGATEXTILEINFOS_SIG[] = "MEGATILEINFOS102";
const char MEGATEXMEGATEXTILEINFOSCHUNKED_SIG[] = "MEGATILEINFOS_CHNK100";

void r3dTerrain3::SaveMegaTexTileInfos( const char* levelPath )
{
	r3dTL::T2DArray< FILE* > fileGrid;

	fileGrid.Resize( m_FileChunkCountX, m_FileChunkCountZ );

	for( int z = 0, e = fileGrid.Height(); z < e; z ++ )
	{
		for( int x = 0, e = fileGrid.Width(); x < e; x ++ )
		{
			char buffer[ 512 ];
			GetTileInfosChunkFileName( buffer, x, z );

			FILE* fout = fopen_for_write( buffer, "wb" );

			fwrite( MEGATEXMEGATEXTILEINFOSCHUNKED_SIG, sizeof MEGATEXMEGATEXTILEINFOSCHUNKED_SIG, 1, fout );

			fileGrid[ z ][ x ] = fout;
		}
	}

	{
		MegaTexTileInfoArr& arr = m_MegaTexTileInfoMipChain[ 0 ];

		UINT32 width = arr.Width();
		UINT32 height = arr.Height();

		for( int z = 0, e = arr.Height(); z < e; z ++ )
		{
			for( int x = 0, e = arr.Width(); x < e; x ++ )
			{
				MegaTexTileInfo& info = arr[ z ][ x ];

				int fileGridX = x / m_TilesInFileChunk;
				int fileGridZ = z / m_TilesInFileChunk;

				FILE* fout = fileGrid[ fileGridZ ][ fileGridX ];

				UINT8 layerCount = info.LayerList.Count();

				fwrite( &layerCount, sizeof layerCount, 1, fout );

				MegaTexTileInfo::LayerListArr::Iterator iter = info.LayerList.GetIterator();

				char vals[ 256 ];

				for( int i = 0; iter.IsValid(); iter.Advance(), i ++ )
				{
					vals[ i ] = iter.GetValue();
				}

				if( layerCount )
				{
					fwrite( vals, layerCount, sizeof(char), fout );
				}
			}
		}
	}

	for( int z = 0, e = fileGrid.Height(); z < e; z ++ )
	{
		for( int x = 0, e = fileGrid.Width(); x < e; x ++ )
		{
			fclose( fileGrid[ z ][ x ] );
		}
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::WriteMipStamps( const char* levelPath )
{
	// save stamp
#ifndef FINAL_BUILD
	Bytes stamp = GenerateStamp();

	FILE* stampFile = fopen( ( levelPath + r3dString( R3D_MEGATEX_STAMP_FILE ) ).c_str(), "wt" );
	fwrite( &stamp[ 0 ], stamp.Count(), 1, stampFile );
	fclose( stampFile );
#else
	Bytes stamp;
	ReadDistribStamp( levelPath, &stamp );
#endif

	char appDataStampFilePath[ 512 ];
	GetMipFileStampName( appDataStampFilePath );

	FILE* appDataFile = fopen( appDataStampFilePath, "wt" );
	fwrite( &stamp[ 0 ], stamp.Count(), 1, appDataFile );
	fclose( appDataFile );
}

//------------------------------------------------------------------------

int r3dTerrain3::LoadMegaTexTileInfos( const char* levelPath )
{
	InitTileInfoMipChain( m_QualitySettings );

	r3dTL::T2DArray< r3dFile* > fileGrid;

	fileGrid.Resize( m_FileChunkCountX, m_FileChunkCountZ );

	for( int z = 0, e = fileGrid.Height(); z < e; z ++ )
	{
		for( int x = 0, e = fileGrid.Width(); x < e; x ++ )
		{
			char buffer[ 512 ];
			GetTileInfosChunkFileName( buffer, x, z );
			fileGrid[ z ][ x ] = r3d_open( buffer, "rb" );
		}
	}

	for( int z = 0, e = fileGrid.Height(); z < e; z ++ )
	{
		for( int x = 0, e = fileGrid.Width(); x < e; x ++ )
		{
			if( !fileGrid[ z ][ x ] )
				return 0;
		}
	}

	struct CloseOnExit
	{
		~CloseOnExit()
		{
			r3dTL::T2DArray< r3dFile* >& fileGrid = *filesToClose;

			for( int z = 0, e = fileGrid.Height(); z < e; z ++ )
			{
				for( int x = 0, e = fileGrid.Width(); x < e; x ++ )
				{
					if( fileGrid[ z ][ x ] )
					{
						fclose( fileGrid[ z ][ x ] );
					}
				}
			}
		}

		r3dTL::T2DArray< r3dFile* >* filesToClose;
	} closeOnExit; (void)closeOnExit;

	closeOnExit.filesToClose = &fileGrid;

	//------------------------------------------------------------------------
	for( int z = 0, e = fileGrid.Height(); z < e; z ++ )
	{
		for( int x = 0, e = fileGrid.Width(); x < e; x ++ )
		{
			char sigChars[ sizeof MEGATEXMEGATEXTILEINFOSCHUNKED_SIG ] = { 0 };

			if( !fread( sigChars, sizeof sigChars, 1, fileGrid[ z ][ x ] ) )
				return 0;

			if( memcmp( sigChars, MEGATEXMEGATEXTILEINFOSCHUNKED_SIG, sizeof sigChars ) )
			{
				r3dOutToLog( "r3dTerrain3::LoadMegaTexTileInfos: unknown chunked info signature %s\n", MEGATEXMEGATEXTILEINFOSCHUNKED_SIG );
				return 0;
			}
		}
	}

	//------------------------------------------------------------------------
	MegaTexTileInfoArr& arr = m_MegaTexTileInfoMipChain[ 0 ];

	for( int z = 0, e = arr.Height(); z < e; z ++ )
	{
		for( int x = 0, e = arr.Width(); x < e; x ++ )
		{
			int fileGridX = x / m_TilesInFileChunk;
			int fileGridZ = z / m_TilesInFileChunk;

			r3dFile* fin = fileGrid[ fileGridZ ][ fileGridX ];

			MegaTexTileInfo& info = arr[ z ][ x ];

			UINT8 layerCount = 0;

			if( !fread( &layerCount, sizeof layerCount, 1, fin ) )
				return 0;

			char vals[ 256 ] = { 0 };

			if( layerCount )
			{
				r3d_assert( layerCount < sizeof vals );

				if( !fread( vals, layerCount * sizeof(char), 1, fin ) )
					return 0;
			}

			info.LayerList.ClearValues();

			for( int i = 0, e = layerCount; i < e; i ++ )
			{
				info.LayerList.PushBack( vals[ i ] );
			}
		}
	}

	for( int L = 1, e = (int)m_MegaTexTileInfoMipChain.Count(); L < e; L ++ )
	{
		MegaTexTileInfoArr& prevArr = m_MegaTexTileInfoMipChain[ L - 1 ];
		MegaTexTileInfoArr& arr = m_MegaTexTileInfoMipChain[ L ];

		for( int z = 0, e = arr.Height(); z < e; z ++ )
		{
			for( int x = 0, e = arr.Width(); x < e; x ++ )
			{
				MegaTexTileInfo& currInfo = arr[ z ][ x ];

				MegaTexTileInfo* prevMipInfos[ 4 ];

				prevMipInfos[ 0 ] = &prevArr[ z * 2 + 0 ][ x * 2 + 0 ];
				prevMipInfos[ 1 ] = &prevArr[ z * 2 + 0 ][ x * 2 + 1 ];
				prevMipInfos[ 2 ] = &prevArr[ z * 2 + 1 ][ x * 2 + 0 ];
				prevMipInfos[ 3 ] = &prevArr[ z * 2 + 1 ][ x * 2 + 1 ];

				for( int i = 0, e = 4; i < e; i ++ )
				{
					MegaTexTileInfo* prevInfo = prevMipInfos[ i ];

					for( MegaTexTileInfo::LayerListArr::Iterator iter = prevInfo->LayerList.GetIterator(); iter.IsValid(); iter.Advance() )
					{
						int insertVal = iter.GetValue();

						int maskId = insertVal / LAYERS_PER_MASK;

						INT64 id = MegaTexTile::GetID( x, z, L, maskId );

						if( m_MegaTexGridFile_Map.find( id ) == m_MegaTexGridFile_Map.end() )
							continue;

						int isThere = 0;

						for( MegaTexTileInfo::LayerListArr::Iterator iter = currInfo.LayerList.GetIterator(); iter.IsValid(); iter.Advance() )
						{
							if( iter.GetValue() == insertVal )
							{
								isThere = 1;
								break;
							}

							if( iter.GetValue() > insertVal )
							{
								currInfo.LayerList.Insert( iter, insertVal );
								isThere = 1;
								break;
							}
						}

						if( !isThere )
							currInfo.LayerList.PushBack( insertVal );

						int count = currInfo.LayerList.Count();
						r3d_assert( count <= (int)m_Layers.Count() );
					}
				}
			}
		}
	}

#ifndef FINAL_BUILD
	m_MegaTexMipsRecalculated = 1;
#endif

	return 1;
}

//------------------------------------------------------------------------

int r3dTerrain3::LoadMegaTexTileInfos_OLD( const char* levelPath )
{
	char fullPath[ 1024 ];

	strcpy( fullPath, levelPath );
	strcat( fullPath, R3D_MEGATEXTILEINFOS_PATH );

	r3dFile* fin = r3d_open( fullPath );

	if( !fin )
		return 0;

	struct OnExit
	{
		~OnExit()
		{
			fclose( file );
		}
		r3dFile * file;
	} onExit = { fin };

	char SIG[ sizeof MEGATEXMEGATEXTILEINFOS_SIG ];
	if( !fread( SIG, sizeof SIG, 1, fin ) )
		return 0;

#ifndef FINAL_BUILD
	m_MegaTexMipsRecalculated = 0;
#endif

	if( memcmp( SIG, MEGATEXMEGATEXTILEINFOS_SIG, sizeof SIG ) )
	{
		if( memcmp( SIG, MEGATEXMEGATEXTILEINFOS_SIG100, sizeof SIG ) 
				&&
			memcmp( SIG, MEGATEXMEGATEXTILEINFOS_SIG101, sizeof SIG ) )
		{
			r3dOutToLog( "r3dTerrain3::LoadMegaTexTileInfos: unknown version signature %s!", SIG );
			return 0;
		}
	}
#ifndef FINAL_BUILD
	else
	{
		m_MegaTexMipsRecalculated = 1;
	}
#endif


	UINT32 count;
	if( !fread( &count, sizeof count, 1, fin ) )
		return 0;

	m_MegaTexTileInfoMipChain.Resize( count );

	for( int i = 0, e = (int)m_MegaTexTileInfoMipChain.Count(); i < e; i ++ )
	{
		UINT32 width;
		UINT32 height;

		if( !fread( &width, sizeof width, 1, fin ) )
			return 0;

		if( !fread( &height, sizeof height, 1, fin ) )
			return 0;

		MegaTexTileInfoArr& arr = m_MegaTexTileInfoMipChain[ i ];

		arr.Resize( width, height );

		for( int z = 0, e = (int)height; z < e; z ++ )
		{
			for( int x = 0, e = (int)width; x < e; x ++ )
			{
				MegaTexTileInfo& info = arr[ z ][ x ];

				UINT8 layerCount = 0;

				if( !fread( &layerCount, sizeof layerCount, 1, fin ) )
					return 0;

				char vals[ 256 ] = { 0 };

				if( layerCount )
				{
					if( !fread( vals, layerCount * sizeof(char), 1, fin ) )
						return 0;
				}

				for( int i = 0, e = layerCount; i < e; i ++ )
				{
					info.LayerList.PushBack( vals[ i ] );
				}
			}
		}
	}

	return 1;
}

//------------------------------------------------------------------------

void r3dTerrain3::MergeEditedMega( const char* levelPath )
{
#ifndef FINAL_BUILD
	if( IsAutoSave( levelPath ) )
	{
		return;
	}
#endif
	r3dFinishBackGroundTasks();

	R3D_FILE_SYSTEM_LOCK_SCOPE();

	r3dWindowsFileHandle wfh;

	int res = FlushFileBuffers( m_MegaTexAtlasFile_EDITOR ); r3d_assert( res );

	wfh.Handle = CreateFile( g_TempEditFilePath.c_str(), GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	r3dTL::TArray< UINT8 > data( GetMaskTileSizeInFile() );

	char fullPath[ 1024 ];

	strcpy( fullPath, levelPath );
	strcat( fullPath, R3D_MEGATEX_PATH_BASE );

	CloseMegaTexGridFiles();

	if( m_MegaTexAtlasMipsFile )
	{
		fclose( m_MegaTexAtlasMipsFile );
		m_MegaTexAtlasMipsFile = NULL;
	}

	OpenMegaTexAtlasMipsFile( "r+b" );

#ifndef FINAL_BUILD
	r3dTL::T2DArray< FILE* > fileGrid;

	fileGrid.Resize( m_FileChunkCountX, m_FileChunkCountZ );

	for( int z = 0, e = fileGrid.Height(); z < e; z ++ )
	{
		for( int x = 0, e = fileGrid.Width(); x < e; x ++ )
		{
			char buffer[ 512 ];
			GetChunkFileName( buffer, x, z );
			fileGrid[ z ][ x ] = fopen_for_write( buffer, "r+b" );
		}
	}
#endif

	for( FileTextureGridMap::iterator	i = m_MegaTexGridFile_Map.begin(),
										e = m_MegaTexGridFile_Map.end(); i != e; ++i )
	{
		MegaTexFileGridOffset& offset = i->second;

		if( offset.IsInEditorFile )
		{
			FILE* fout = NULL;

			MegaTexFileGridOffset maxOffset;
			maxOffset.Value = UINT64( -1LL );

			short x, z; char L; int m;
			MegaTexTile::DecodeID( i->first, &x, &z, &L, &m );

			if( offset.Value == maxOffset.Value )
			{
				if( L )
				{
					offset.GridId = GRID_ID_MIPSFILE;
				}
				else
				{
					int fileGridX = x / m_TilesInFileChunk;
					int fileGridZ = z / m_TilesInFileChunk;

					EncodeGridCoordinates( &offset, fileGridX, fileGridZ );
				}
			}

			if( offset.GridId == GRID_ID_MIPSFILE )
			{
				fout = m_MegaTexAtlasMipsFile;
			}
			else
			{
#ifndef FINAL_BUILD
				int gridX, gridZ;
				GetGridCoordinates( offset, &gridX, &gridZ );

				fout = fileGrid[ gridZ ][ gridX ];
#else
				r3dOutToLog( "Error: zero mip tile modified in FINAL!\n" );
				fout = NULL;
#endif
			}

#ifdef FINAL_BUILD
			if( !fout )
				continue;
#endif

			int isNew = 0;

			if( offset.Value == maxOffset.Value )
			{
				fseek( fout, 0, SEEK_END );
				UINT64 newOffset = ftell( fout );

				offset.Value = newOffset;

				isNew = 1;
			}

			fseek( fout, (long) offset.Value, SEEK_SET );
			fseek( wfh, offset.EditorValue, SEEK_SET );

			short maskIdx;
			int n;
			n = fread( &maskIdx, sizeof maskIdx, 1, wfh ); r3d_assert( n );
			n = fread( &data[ 0 ], data.Count(), 1, wfh ); r3d_assert( n );

			r3d_assert( maskIdx == m );

#if 0
			r3dOutToLog( "~~~Saved %d,%d,%d,%d  to  %X(%c)\n", x, z, L, m, int( offset.Value ), isNew ? 'n': 'o' );
#endif

			fwrite( &maskIdx, sizeof maskIdx, 1, fout );
			fwrite( &data[ 0 ], data.Count(), 1, fout );
		}
	}

#ifndef FINAL_BUILD
	Bytes heightNormalBytes;

	heightNormalBytes.Resize( GetHeightNormalRecordSizeInFile() );

	for( FileTextureGridMap::iterator	i = m_EDITOR_HeightNormalTileMap.begin(),
										e = m_EDITOR_HeightNormalTileMap.end();
										i != e;
										++ i )
	{
		MegaTexFileGridOffset& offset = i->second;

		if( offset.IsInEditorFile )
		{
			MegaTexTile fakeTile;

			short tx, tz;
			char tl;
			int midx;
			MegaTexTile::DecodeID( i->first, &tx, &tz, &tl, &midx );

			fakeTile.X = tx;
			fakeTile.Z = tz;
			fakeTile.L = tl;

			MegaTexFileGridOffset checkOffset;
			GetMegaTexHeightNormalTargetOffsetInFile( &fakeTile, &checkOffset );

			r3d_assert( offset.Value == checkOffset.Value && offset.GridId == checkOffset.GridId );

			FILE* fout = NULL;

			if( offset.GridId == GRID_ID_MIPSFILE )
			{
				fout = m_MegaTexAtlasMipsFile;
			}
			else
			{
				int gridX, gridZ;
				GetGridCoordinates( offset, &gridX, &gridZ );

				fout = fileGrid[ gridZ ][ gridX ];
			}

			fseek( fout, (long) offset.Value, SEEK_SET );
			fseek( wfh, offset.EditorValue, SEEK_SET );

			fread( &heightNormalBytes[ 0 ], heightNormalBytes.Count(), 1, wfh );

			r3d_assert( !memcmp( &heightNormalBytes[ 0 ], MEGA_HN_SIG, sizeof MEGA_HN_SIG ) );

			fwrite( &heightNormalBytes[ 0 ], heightNormalBytes.Count(), 1, fout );
		}
	}

	m_EDITOR_HeightNormalTileMap.clear();
	m_MegaTexAtlasFile_EDITOR_Size = 0;
#endif

	for( FileTextureGridMap::iterator	i = m_MegaTexGridFile_Map.begin(),
										e = m_MegaTexGridFile_Map.end(); i != e; ++i )
	{
		i->second.EditorValue = -1;
		i->second.IsInEditorFile = 0;
	}

#ifndef FINAL_BUILD
	for( int z = 0, e = fileGrid.Height(); z < e; z ++ )
	{
		for( int x = 0, e = fileGrid.Width(); x < e; x ++ )
		{
			fclose( fileGrid[ z ][ x ] );
		}
	}
#endif

	fclose( m_MegaTexAtlasMipsFile );
	m_MegaTexAtlasMipsFile = NULL;

	CloseHandle( wfh.Handle );

	OpenMegaTexGridFiles( levelPath );
	OpenMegaTexAtlasMipsFile( "rb" );
}

//------------------------------------------------------------------------

void r3dTerrain3::CheckMasksPresenceInInfo( INT64 finalSize, int startMaskIdx, int gridX, int gridZ )
{
	int maskTileSizeInFile = GetMaskTileSizeInFile();

	Info info = GetInfo();

	for( FileTextureGridMap::iterator	i = m_MegaTexGridFile_Map.begin(),
										e = m_MegaTexGridFile_Map.end(); 
										i != e; i ++ )
	{
#ifndef FINAL_BUILD
		r3d_assert( !i->second.IsInEditorFile );
#endif

		int gX, gZ;
		GetGridCoordinates( i->second, &gX, &gZ );

		if( gridX != gX || gridZ != gZ )
			continue;

		r3d_assert( i->second.Value + maskTileSizeInFile + sizeof(short) <= (UINT64)finalSize );

		short x, z;
		char level;
		int m;
		MegaTexTile::DecodeID( i->first, &x, &z, &level, &m );

		if( m < startMaskIdx )
			continue;

		int tileCountX = info.MegaTileCountX >> level;
		int tileCountZ = info.MegaTileCountZ >> level;

		if( x < 0 || x >= tileCountX
			||
			z < 0 || z >= tileCountZ )
		{
			r3dOutToLog( "Rogue tile: %d, %d, %d, %d\n", x, z, level, m );
			continue;
		}

		MegaTexTileInfo* mttinfo = &m_MegaTexTileInfoMipChain[ level ][ z ][ x ];

		int found = 0;
		for( MegaTexTileInfo::LayerListArr::Iterator iter = mttinfo->LayerList.GetIterator(); iter.IsValid(); iter.Advance() )
		{
			if( iter.GetValue() / LAYERS_PER_MASK == m )
				found = 1;
		}

		r3d_assert( found );
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::GatherMasksAbsentInInfo( r3dTL::TArray< INT64 > * oArr )
{
	oArr->Clear();

	Info info = GetInfo();

	for(	 FileTextureGridMap::iterator	i = m_MegaTexGridFile_Map.begin(),
			e = m_MegaTexGridFile_Map.end(); 
			i != e; i ++ )
	{

		short x, z;
		char level;
		int m;
		MegaTexTile::DecodeID( i->first, &x, &z, &level, &m );

		int tileCountX = info.MegaTileCountX >> level;
		int tileCountZ = info.MegaTileCountZ >> level;


		MegaTexTileInfo* mttinfo = &m_MegaTexTileInfoMipChain[ level ][ z ][ x ];

		int found = 0;
		for( MegaTexTileInfo::LayerListArr::Iterator iter = mttinfo->LayerList.GetIterator(); iter.IsValid(); iter.Advance() )
		{
			if( iter.GetValue() / LAYERS_PER_MASK == m )
				found = 1;
		}

		if( !found )
		{
			oArr->PushBack( i->first );
		}
	}
}

//------------------------------------------------------------------------

int r3dTerrain3::DeleteLayerMasks( MaskIdSet& masksForDeletion, int startMaskIdx )
{
	int numDeleted = 0;

#ifndef FINAL_BUILD
	r3dFinishBackGroundTasks();
	MergeEditedMega( r3dGameLevel::GetHomeDir() );

	float lastInfoFrame = r3dGetTime();
	const float INFO_DELTA = 0.125F;

	for( int gridZ = 0, e = m_FileGrid.Height(); gridZ < e; gridZ ++ )
	{
		for( int gridX = 0, e = m_FileGrid.Width(); gridX < e; gridX ++ )
		{
			typedef std::map< INT64, FileTextureGridMap::value_type* > OffsetToEntryMap;
			OffsetToEntryMap offsetEntryMap;

			for( FileTextureGridMap::iterator	i = m_MegaTexGridFile_Map.begin(),
												e = m_MegaTexGridFile_Map.end();
												i != e;
												++ i )
			{
				int gX( 0 ), gZ( 0 );

				GetGridCoordinates( i->second, &gX, &gZ );

				if( gridX == gX && gridZ == gZ )
				{
					offsetEntryMap.insert( OffsetToEntryMap::value_type( i->second.Value, &*i ) );
				}
			}

			int accumOffset = 0;

			r3dTL::TArray< RelocatedTerrainMask > relocatedMasks;

			int maskTileSizeInFile = GetMaskTileSizeInFile() + sizeof( short );

			for( OffsetToEntryMap::iterator		i = offsetEntryMap.begin(),
												e = offsetEntryMap.end();
												i != e;
												++ i )
			{
				FileTextureGridMap::value_type* valPtr = i->second;

				if( masksForDeletion.find( valPtr->first ) != masksForDeletion.end() )
				{
					accumOffset += maskTileSizeInFile;

					numDeleted ++;

					FileTextureGridMap::iterator found = m_MegaTexGridFile_Map.find( valPtr->first );

					if( found == m_MegaTexGridFile_Map.end() )
					{
						ErrorOnMissingMask( "r3dTerrain3::DeleteLayerMasks", valPtr->first );
					}

					m_MegaTexGridFile_Map.erase( found );
				}
				else
				{
					if( accumOffset )
					{
						RelocatedTerrainMask rm;

						rm.oldOffset = valPtr->second.Value;
						valPtr->second.Value -= accumOffset;
						rm.newOffset = valPtr->second.Value;
						rm.id = valPtr->first;

						int curSize = 0;
						fseek( m_FileGrid[ gridZ ][ gridX ], 0, SEEK_END );
						curSize = ftell( m_FileGrid[ gridZ ][ gridX ] );
						r3d_assert( curSize >= rm.oldOffset + maskTileSizeInFile );

						GetGridCoordinates( valPtr->second, &rm.gridX, &rm.gridZ );

						relocatedMasks.PushBack( rm );
					}
				}
			}

			Bytes buffer;

			buffer.Resize( maskTileSizeInFile );


			for( int i = 0, e = (int)relocatedMasks.Count(); i < e; i ++ )
			{
				const RelocatedTerrainMask& rm = relocatedMasks[ i ];

				if( r3dGetTime() - lastInfoFrame > INFO_DELTA )
				{
					lastInfoFrame = r3dGetTime();
					char buf[ 512 ];
					sprintf( buf, "Saving %d of %d", i, relocatedMasks.Count() );
					ReportProgress( buf );
				}

				r3d_assert( rm.gridX == gridX && rm.gridZ == gridZ );

				fseek( m_FileGrid[ gridZ ][ gridX ], (long)rm.oldOffset, SEEK_SET );
				int n = fread( &buffer[ 0 ], buffer.Count(), 1, m_FileGrid[ gridZ ][ gridX ] );
				r3d_assert( n );

				fclose( m_FileGrid[ gridZ ][ gridX ] );

				char fileName[ 512 ];
				GetChunkFileName( fileName, gridX, gridZ );

				FILE* file = fopen( fileName, "r+b" );

				fseek( file, (long)rm.newOffset, SEEK_SET );
				fwrite( &buffer[ 0 ], buffer.Count(), 1, file );

				if( i == relocatedMasks.Count() - 1 )
				{
					size_t eof_Offset = ftell( file );
					ftruncate( file, eof_Offset );
				}

				fclose( file );

				m_FileGrid[ gridZ ][ gridX ] = fopen( fileName, "rb" );
			}

			fseek( m_FileGrid[ gridZ ][ gridX ], 0, SEEK_END );
			int finalSize = ftell( m_FileGrid[ gridZ ][ gridX ] );

			CheckMasksPresenceInInfo( finalSize, startMaskIdx, gridX, gridZ );

			for( int L = 0, e = m_MegaTexTileInfoMipChain.Count(); L < e; L ++ )
			{
				MegaTexTileInfoArr& infoArr = m_MegaTexTileInfoMipChain[ L ];

				for( int tz = 0, e = infoArr.Height(); tz < e; tz ++ )
				{
					for( int tx = 0, e = infoArr.Width(); tx < e; tx ++ )
					{
						if( r3dGetTime() - lastInfoFrame > INFO_DELTA )
						{
							lastInfoFrame = r3dGetTime();
							char buf[ 512 ];
							sprintf( buf, "Checking tile info (%d,%d,%d)", tx, tz, L );
							ReportProgress( buf );
						}

						MegaTexTileInfo& info = infoArr[ tz ][ tx ];

						for( int i = 0, e = (int)info.LayerList.Count(); i < e; )
						{
							int val = info.LayerList.GetIterator( i ).GetValue();
							int m = val / LAYERS_PER_MASK;

							INT64 id = MegaTexTile::GetID( tx, tz, L, m );

							if( m_MegaTexGridFile_Map.find( id ) == m_MegaTexGridFile_Map.end() )
							{
								r3dOutToLog( "Optimizing (%d,%d,%d,%d)\n", tx, tz, L, m );
								info.LayerList.Erase( info.LayerList.GetIterator( i ) );
								e --;
							}
							else
								i ++;
						}
					}
				}
			}
		}
	}

	FreeAllMegaTexTiles();
	
#endif

	return numDeleted;
}

//------------------------------------------------------------------------

template< typename T >
void r3dTerrain3::WriteHeightNormalTileInFile( T file, const void * heightData, const void* normalData )
{
	fwrite( MEGA_HN_SIG, sizeof MEGA_HN_SIG, 1, file );

	int htsif = GetHeightTileSizeInFile();

	if( heightData )
		fwrite( (void*)heightData, htsif, 1, file );
	else
		fseek( file, htsif, SEEK_CUR );

	int ntsif = GetNormalTileSizeInFile();

	if( normalData )
		fwrite( (void*)normalData, ntsif, 1, file );
	else
		fseek( file, ntsif, SEEK_CUR );
}

//------------------------------------------------------------------------

/*static*/ void r3dTerrain3::CreateEditorBlankMaskInMainThreadHelper( void* param )
{
	static_cast< r3dTerrain3* > ( param )->CreateEditorBlankMaskInMainThread();
}

void r3dTerrain3::CreateEditorBlankMask()
{
	if( R3D_IS_MAIN_THREAD() )
	{
		CreateEditorBlankMaskInMainThread();
	}
	else
	{
		ProcessCustomDeviceQueueItem( CreateEditorBlankMaskInMainThreadHelper, this );
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::CreateEditorBlankMaskInMainThread()
{
	if( !m_EDITOR_BlankMask.Valid() )
	{
		IDirect3DTexture9* blank( NULL );
		D3D_V( r3dRenderer->pd3ddev->CreateTexture( m_MegaTexAtlasFile_MaskDim, m_MegaTexAtlasFile_MaskDim, 1, 0, D3DFMT_R5G6B5, D3DPOOL_SYSTEMMEM, &blank, NULL ) );

		r3dDeviceTunnel::CreateTexture( m_MegaTexAtlasFile_MaskDim, m_MegaTexAtlasFile_MaskDim, 1, 0, D3DFORMAT( m_MegaTexAtlasFile_MaskFormat ), D3DPOOL_SYSTEMMEM, &m_EDITOR_BlankMask );

		D3DSURFACE_DESC sdesc;
		D3DLOCKED_RECT lrect;

		D3D_V( blank->GetLevelDesc( 0, &sdesc ) );

		D3D_V( blank->LockRect( 0, &lrect, NULL, 0 ) );

		for( int y = 0, e = sdesc.Height; y < e; y ++  )
		{
			for( int x = 0, e = sdesc.Width; x < e; x ++ )
			{
				((WORD*)lrect.pBits)[ y * lrect.Pitch / sizeof ( WORD ) + x ] = 0;
			}
		}

		D3D_V( blank->UnlockRect( 0 ) );

		IDirect3DSurface9* src( NULL ), *dest( NULL );

		D3D_V( blank->GetSurfaceLevel( 0, &src ) );

		D3D_V( m_EDITOR_BlankMask.AsTex2D()->GetSurfaceLevel( 0, &dest ) );

		D3D_V( D3DXLoadSurfaceFromSurface( dest, NULL, NULL, src, NULL, NULL, D3DX_FILTER_NONE, 0 ) );

		SAFE_RELEASE( src );
		SAFE_RELEASE( dest );

		SAFE_RELEASE( blank );
	}
}

//------------------------------------------------------------------------
/*static*/ void r3dTerrain3::InitDynamicInMainThread( void* param )
{
	static_cast< r3dTerrain3* > ( param )->DoInitDynamicInMainThread();
}

//------------------------------------------------------------------------

void r3dTerrain3::DoInitDynamicInMainThread()
{
	IDirect3DTexture9 *mask( NULL ), *normal( NULL ), *height( NULL );

	D3D_V( r3dRenderer->pd3ddev->CreateTexture( m_QualitySettings.MaskAtlasTileDim, m_QualitySettings.MaskAtlasTileDim, 1, 0, TERRA3_LAYERMASK_FORMAT, D3DPOOL_SYSTEMMEM, &mask, NULL ) );
	D3D_V( r3dRenderer->pd3ddev->CreateTexture( m_QualitySettings.HeightNormalAtlasTileDim, m_QualitySettings.HeightNormalAtlasTileDim, 1, 0, TERRA3_MEGANORMAL_FORMAT, D3DPOOL_SYSTEMMEM, &normal, NULL ) );
	D3D_V( r3dRenderer->pd3ddev->CreateTexture( m_QualitySettings.HeightNormalAtlasTileDim, m_QualitySettings.HeightNormalAtlasTileDim, 1, 0, TERRA3_MEGAHEIGHT_FORMAT, D3DPOOL_SYSTEMMEM, &height, NULL ) );

	m_MegaTexTile_TempMask.Set( mask );
	m_MegaTexTile_TempNormal.Set( normal );
	m_MegaTexTile_TempHeight.Set( height );

	CreateEditorBlankMask();
}

//------------------------------------------------------------------------

r3dTerrain3::MegaTexTileInfo* r3dTerrain3::GetMegaTexTileInfo( MegaTexTile* tile )
{
	MegaTexTileInfoArr& arr = m_MegaTexTileInfoMipChain[ tile->L ];

	return &arr[ tile->Z ][ tile->X ];
}

//------------------------------------------------------------------------

r3dTerrain3::MegaTexTileInfo* r3dTerrain3::GetMegaTexTileInfo( int x, int z, int L )
{
	return &m_MegaTexTileInfoMipChain[ L ][ z ][ x ];
}

//------------------------------------------------------------------------

void r3dTerrain3::GetPhysHeightmapBoundaries( int* oXStart, int *oZStart, int* oXEnd, int* oZEnd )
{
	const QualitySettings& qs = GetCurrentQualitySettings();
	const r3dTerrainDesc& desc = Terrain3->GetDesc();

#if R3D_TERRAIN3_ALLOW_FULL_PHYSCHUNKS
	if( m_FullPhysChunkModeOn )
	{
		*oXStart = 0;
		*oZStart = 0;

		*oXEnd = desc.CellCountX;
		*oZEnd = desc.CellCountZ;
	}
	else
#endif
	{
		for( int i = 0, e = m_PhysicsChunks.Count(); i < e; i ++ )
		{
			PhysicsChunk& physChunk = m_PhysicsChunks[ i ];

			*oXStart = R3D_MIN( physChunk.StartX, *oXStart );
			*oZStart = R3D_MIN( physChunk.StartZ, *oZStart );

			*oXEnd = R3D_MAX( physChunk.StartX + qs.PhysicsTileCellCount, *oXEnd );
			*oZEnd = R3D_MAX( physChunk.StartZ + qs.PhysicsTileCellCount, *oZEnd );
		}
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::GetPhysHeightmapBoundaries( float* oXStart, float *oZStart, float* oXEnd, float* oZEnd )
{
#if R3D_TERRAIN3_ALLOW_FULL_PHYSCHUNKS
	if( m_FullPhysChunkModeOn )
	{
		const r3dTerrainDesc& desc = Terrain3->GetDesc();

		*oXStart = 0;
		*oZStart = 0;

		*oXEnd = desc.XSize;
		*oZEnd = desc.ZSize;
	}
	else
#endif
	{
		const QualitySettings& qs = GetCurrentQualitySettings();

		*oXStart = -FLT_MAX;
		*oZStart = -FLT_MAX;

		*oXEnd = FLT_MAX;
		*oZEnd = FLT_MAX;

		for( int i = 0, e = m_PhysicsChunks.Count(); i < e; i ++ )
		{
			PhysicsChunk& physChunk = m_PhysicsChunks[ i ];

			*oXStart = R3D_MIN( physChunk.StartX * m_CellSize, *oXStart );
			*oZStart = R3D_MIN( physChunk.StartZ * m_CellSize, *oZStart );

			*oXEnd = R3D_MAX( ( physChunk.StartX + qs.PhysicsTileCellCount ) * m_CellSize, *oXEnd );
			*oZEnd = R3D_MAX( ( physChunk.StartZ + qs.PhysicsTileCellCount ) * m_CellSize, *oZEnd );
		}
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::SplitMegaTexFile( const char* levelPath, const char* filePath )
{
#ifndef FINAL_BUILD
	r3dFile* source = r3d_open( filePath, "rb" );

	fseek( source, 0, SEEK_END );

	int size = ftell( source );

	fseek( source, 0, SEEK_SET );

	Bytes buffer;
	buffer.Resize( R3D_MEGATEX_CHUNK_SIZE );

	for( int ptr = 0, i = 0; ptr < size; ptr += R3D_MEGATEX_CHUNK_SIZE, i ++ )
	{
		char fullPath[ 1024 ];
		sprintf( fullPath, "%s%s_%d.%s", levelPath, R3D_MEGATEX_PATH_BASE, i, R3D_MEGATEX_PATH_EXT );

		FILE* fout = fopen( fullPath, "wb" );

		int sizeToWrite = R3D_MIN( int( R3D_MEGATEX_CHUNK_SIZE ), int( size - ptr ) ); 

		int n = fread( &buffer[ 0 ], sizeToWrite, 1, source );
		r3d_assert( n );

		fwrite( &buffer[ 0 ], sizeToWrite, 1, fout );

		if( !i )
		{
			fseek( fout, 0, SEEK_SET );
			fwrite( MEGA_TEX_SIG, sizeof MEGA_TEX_SIG, 1, fout );
		}

		fclose( fout );
	}

	r3d_assert( source );

	fclose( source );
#endif
}

//------------------------------------------------------------------------

void r3dTerrain3::ErrorOnMissingMask( const char* srcFunc, INT64 id )
{
	short x, z;
	char L;
	int maskId;

	MegaTexTile::DecodeID( id, &x, &z, &L, &maskId );

	r3dError( "%s: missing mask (%d,%d,%d,%d)", srcFunc, x, z, L, maskId );
}

//------------------------------------------------------------------------

void r3dTerrain3::GetGridCoordinates( const MegaTexFileGridOffsetBase& offset, int *oX, int *oZ )
{
	*oX = offset.GridId & 0xF;
	*oZ = int( offset.GridId >> 4 );
}

//------------------------------------------------------------------------

void r3dTerrain3::EncodeGridCoordinates( MegaTexFileGridOffsetBase* oOffset, int x, int z )
{
	oOffset->GridId = x | ( z << 4 );
}

//------------------------------------------------------------------------

void r3dTerrain3::GetChunkFileName( char* buff, int x, int z )
{
	sprintf( buff, "%s/Terrain3/chunk_%d_%d.bin", r3dGameLevel::GetHomeDir(), x, z );
}

//------------------------------------------------------------------------

void r3dTerrain3::GetTileInfosChunkFileName( char* buff, int x, int z )
{
	sprintf( buff, "%s/Terrain3/tileinfos_%d_%d.bin", r3dGameLevel::GetHomeDir(), x, z );
}

//------------------------------------------------------------------------

void r3dTerrain3::GetMegaMapChunkFileName( char* buff, int x, int z )
{
	sprintf( buff, "%s/Terrain3/megatexmap_%d_%d.bin", r3dGameLevel::GetHomeDir(), x, z );
}

//------------------------------------------------------------------------

void r3dTerrain3::GetMipFileDir( char* buff )
{
	SHGetFolderPath( r3dRenderer->HLibWin, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, buff );

	strcat( buff, "/Arktos Entertainment/" );
	if( !r3dDirectoryExists( buff ) )
	{
		mkdir( buff );
	}

	strcat( buff, "Infestation/" );
	if( !r3dDirectoryExists( buff ) )
	{
		mkdir( buff );
	}

	r3dString homeDir = r3dGameLevel::GetHomeDir();

	int c = 0;
	int lc = 0;
	for( int e = homeDir.Length(); c < e; c ++ )
	{
		if( homeDir[ c ] == '/' || 
			homeDir[ c ] == '\\' )
		{
			strncat( buff, homeDir.c_str() + lc, c - lc );

			if( !r3dDirectoryExists( buff ) )
			{
				mkdir( buff );
			}

			lc = c;
		}
	}

	if( c > lc )
	{
		strncat( buff, homeDir.c_str() + lc, c - lc );
		strcat( buff, "/" );

		mkdir( buff );
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::GetMipFileName( char* buff )
{
	GetMipFileDir( buff );
	strcat( buff, "terrain.mips" );
}

//------------------------------------------------------------------------

void r3dTerrain3::GetMipFileStampName( char* buff )
{
	GetMipFileDir( buff );
	strcat( buff, "stamp.txt" );
}

//------------------------------------------------------------------------

void r3dTerrain3::GetDistribStampName( const char* levelPath, char* buff )
{
	sprintf( buff, "%s%s", levelPath, R3D_MEGATEX_STAMP_FILE );
}

//------------------------------------------------------------------------

void r3dTerrain3::CreateMegaTexInfo( const char* dirPath )
{
	int tileCountX = m_TileCountX / m_QualitySettings.VertexTilesInMegaTexTileCount;
	int tileCountZ = m_TileCountZ / m_QualitySettings.VertexTilesInMegaTexTileCount;

	m_MegaTexAtlasFile_NormalDim		= m_QualitySettings.HeightNormalAtlasTileDim;
	m_MegaTexAtlasFile_HeightDim		= m_QualitySettings.HeightNormalAtlasTileDim;
	m_MegaTexAtlasFile_MaskDim			= m_QualitySettings.MaskAtlasTileDim;
	m_MegaTexAtlasFile_BaseTileCountX	= tileCountX;
	m_MegaTexAtlasFile_BaseTileCountZ	= tileCountZ;

	//------------------------------------------------------------------------

	char fullPath[ 1024 ];

	strcpy( fullPath, dirPath );
	strcat( fullPath, R3D_MEGATEX_PATH_INFO );

	FILE* megaTexInfoFile = fopen_for_write( fullPath, "wb" );

	struct OnExit
	{
		~OnExit()
		{
			fclose( file );
		}
		FILE *file;
	} onExit = { megaTexInfoFile };

	//------------------------------------------------------------------------

	fwrite( MEGA_TEX_INFO_SIG, sizeof MEGA_TEX_INFO_SIG, 1, megaTexInfoFile );

	fwrite( &m_MegaTexAtlasFile_NormalDim, sizeof m_MegaTexAtlasFile_NormalDim, 1, megaTexInfoFile );
	fwrite( &m_MegaTexAtlasFile_HeightDim, sizeof m_MegaTexAtlasFile_HeightDim, 1, megaTexInfoFile );
	fwrite( &m_MegaTexAtlasFile_MaskDim, sizeof m_MegaTexAtlasFile_MaskDim, 1, megaTexInfoFile );
	fwrite( &m_MegaTexAtlasFile_BaseTileCountX, sizeof m_MegaTexAtlasFile_BaseTileCountX, 1, megaTexInfoFile );
	fwrite( &m_MegaTexAtlasFile_BaseTileCountZ, sizeof m_MegaTexAtlasFile_BaseTileCountZ, 1, megaTexInfoFile );

	fwrite( &m_MegaTexAtlasFile_NormalFormat, sizeof m_MegaTexAtlasFile_NormalFormat, 1, megaTexInfoFile );
	fwrite( &m_MegaTexAtlasFile_HeightFormat, sizeof m_MegaTexAtlasFile_HeightFormat, 1, megaTexInfoFile );
	fwrite( &m_MegaTexAtlasFile_MaskFormat, sizeof m_MegaTexAtlasFile_MaskFormat, 1, megaTexInfoFile );
	fwrite( &m_MegaTexAtlasFile_ColorModulationFormat, sizeof m_MegaTexAtlasFile_ColorModulationFormat, 1, megaTexInfoFile );

	m_MegaTexAtlasFile_NumActiveLayers = m_NumActiveMegaTexQualityLayers;
	fwrite( &m_MegaTexAtlasFile_NumActiveLayers, sizeof m_MegaTexAtlasFile_NumActiveLayers, 1, megaTexInfoFile );
}

//------------------------------------------------------------------------

int r3dTerrain3::ConvertToFileGrid( const char* dirPath )
{
	r3dTL::T2DArray< FILE* > fileGrid;

	fileGrid.Resize( m_FileChunkCountX, m_FileChunkCountZ );

	for( int z = 0, e = fileGrid.Height(); z < e; z ++ )
	{
		for( int x = 0, e = fileGrid.Width(); x < e; x ++ )
		{
			char buffer[ 512 ];
			GetChunkFileName( buffer, x, z );
			fileGrid[ z ][ x ] = fopen_for_write( buffer, "wb" );
		}
	}

	r3dTL::TArray< UINT8 > buffer;

	Info info = GetInfo();

	for( int tz = 0, e = info.MegaTileCountZ; tz < e; tz ++ )
	{
		for( int tx = 0, e = info.MegaTileCountX; tx < e; tx ++ )
		{
			int fileGridX = tx / m_TilesInFileChunk;
			int fileGridZ = tz / m_TilesInFileChunk;

			MegaTexTile tile;
			tile.X = tx;
			tile.Z = tz;
			tile.L = 0;

			MegaTexFileOffset offset = GetMegaTexHeightNormalOffsetInFile_OLD( &tile );

			int mapSize = GetHeightNormalRecordSizeInFile();

			buffer.Resize( mapSize );

			fseek( m_MegaTexAtlasFile, (long)offset.Value, SEEK_SET );

			fread( &buffer[ 0 ], mapSize, 1, m_MegaTexAtlasFile );
			fwrite( &buffer[ 0 ], mapSize, 1, fileGrid[ fileGridZ ][ fileGridX ] );
		}
	}

	for( FileTextureMap::iterator	i = m_MegaTexAtlasFile_Map.begin(),
									e = m_MegaTexAtlasFile_Map.end();
									i != e;
									++ i )
	{
		short tx, tz;
		char l;
		int m;

		MegaTexTile::DecodeID( i->first, &tx, &tz, &l, &m );

		if( !l )
		{
			int fileGridX = tx / m_TilesInFileChunk;
			int fileGridZ = tz / m_TilesInFileChunk;

			FILE* targetFile = fileGrid[ fileGridZ ][ fileGridX ];

			MegaTexFileGridOffset offset;

			offset.Value = ftell( targetFile );

			r3d_assert( fileGridX <= 8 && fileGridZ <= 8 );

			EncodeGridCoordinates( &offset, fileGridX, fileGridZ );

			m_MegaTexGridFile_Map.insert( FileTextureGridMap::value_type( i->first, offset ) );

			int maskSize = GetMaskTileSizeInFile() + sizeof( short );

			buffer.Resize( maskSize );

			fseek( m_MegaTexAtlasFile, (long)i->second.Value, SEEK_SET );

			fread( &buffer[ 0 ], maskSize, 1, m_MegaTexAtlasFile );
			fwrite( &buffer[ 0 ], maskSize, 1, targetFile );
		}
	}

	for( int z = 0, e = fileGrid.Height(); z < e; z ++ )
	{
		for( int x = 0, e = fileGrid.Width(); x < e; x ++ )
		{
			fclose( fileGrid[ z ][ x ] );
		}
	}

	//------------------------------------------------------------------------

	for( FileTextureMap::iterator	i = m_MegaTexAtlasFile_Map.begin(),
									e = m_MegaTexAtlasFile_Map.end();
									i != e;
									++ i )
	{
		r3dTL::TArray< UINT8 > buff;

		short tx, tz;
		char l;
		int m;

		MegaTexTile::DecodeID( i->first, &tx, &tz, &l, &m );

		if( l )
		{
			// insert uninitialized offset so that it is generated later on
			m_MegaTexGridFile_Map.insert( FileTextureGridMap::value_type( i->first, MegaTexFileGridOffset() ) );
		}
	}

	//------------------------------------------------------------------------

	CreateMegaTexInfo( dirPath );

	OpenMegaTexGridFiles( dirPath );

	m_MegaTexAtlasFile_HeightNormalOffset = 0;

	CreateMipFile( dirPath );

	CloseMegaTexGridFiles();

	// delete old files
	{
		char buffer[ 512 ];

		strcpy( buffer, dirPath );
		strcat( buffer, "/Terrain3/" );
		strcat( buffer, "megatileinfos.dat" );
		remove( buffer );

		strcpy( buffer, dirPath );
		strcat( buffer, "/Terrain3/" );
		strcat( buffer, "megatex.map" );
		remove( buffer );

		int chunkCount = m_MegaTexAtlasFile->GetChunkCount();

		fclose( m_MegaTexAtlasFile );
		m_MegaTexAtlasFile = NULL;

		for( int i = 0, e = chunkCount; i < e; i ++ )
		{
			strcpy( buffer, dirPath );
			strcat( buffer, "/Terrain3/" );
			sprintf( buffer + strlen( buffer ), "megatex_%d.bin", i );
			remove( buffer );
		}
	}

	//------------------------------------------------------------------------

	return 1;
}

//------------------------------------------------------------------------

void r3dTerrain3::RecalcTileCoordBorders( const TileCoords& tcoords, UShorts& tempUShorts0, UShorts& tempUShorts1, IDirect3DTexture9* packedTexture, IDirect3DTexture9* unpackedTexture, void (*reportCallBack)( int, int ) )
{
	const r3dTerrain3::QualitySettings& qs = Terrain3->GetCurrentQualitySettings();

	enum
	{
		HALF_MEGA_TEX_BORDER = r3dTerrain3::MEGA_TEX_BORDER / 2,
		FULL_MEGA_TEX_BORDER = r3dTerrain3::MEGA_TEX_BORDER
	};

	r3dTerrain3::Info info = GetInfo();

	float lastInfoFrame = r3dGetTime();
	const float INFO_DELTA = 0.125f;

	for( int i = 0, e = (int)tcoords.Count(); i < e; i ++ )
	{
		const TileCoord& tc = tcoords[ i ];

		if( !HasMask( tc.X, tc.Z, tc.L, tc.M ) )
			continue;

		if( r3dGetTime() - lastInfoFrame > INFO_DELTA )
		{
			lastInfoFrame = r3dGetTime();

			if( reportCallBack )
				reportCallBack( i, e );
		}

		int tileCountX = info.MegaTileCountX >> tc.L;
		int tileCountZ = info.MegaTileCountZ >> tc.L;

		UnpackMask( &tempUShorts0, packedTexture, unpackedTexture, tc.X, tc.Z, tc.L, tc.M );

		if( tc.X > 0 && tc.Z > 0 )
		{
			if( HasMask( tc.X - 1, tc.Z - 1, tc.L, tc.M ) )
			{
				UnpackMask( &tempUShorts1, packedTexture, unpackedTexture, tc.X - 1, tc.Z - 1, tc.L, tc.M );

				for( int z = 0, e = FULL_MEGA_TEX_BORDER; z < e; z ++ )
				{
					for( int x = 0, e = FULL_MEGA_TEX_BORDER; x < e; x ++ )
					{
						tempUShorts0[ x + z * qs.MaskAtlasTileDim ] = tempUShorts1[ ( qs.MaskAtlasTileDim - 2 * FULL_MEGA_TEX_BORDER + z ) * qs.MaskAtlasTileDim + ( qs.MaskAtlasTileDim - 2 * FULL_MEGA_TEX_BORDER + x ) ];
					}
				}
			}
		}

		if( tc.X < tileCountX - 1 && tc.Z < tileCountZ - 1 )
		{
			if( HasMask( tc.X + 1, tc.Z + 1, tc.L, tc.M ) )
			{
				UnpackMask( &tempUShorts1, packedTexture, unpackedTexture, tc.X + 1, tc.Z + 1, tc.L, tc.M );

				for( int z = 0, e = FULL_MEGA_TEX_BORDER; z < e; z ++ )
				{
					for( int x = 0, e = FULL_MEGA_TEX_BORDER; x < e; x ++ )
					{
						tempUShorts0[ ( x + qs.MaskAtlasTileDim - FULL_MEGA_TEX_BORDER ) + ( z + qs.MaskAtlasTileDim - FULL_MEGA_TEX_BORDER ) * qs.MaskAtlasTileDim ] = tempUShorts1[ ( FULL_MEGA_TEX_BORDER + z ) * qs.MaskAtlasTileDim + ( FULL_MEGA_TEX_BORDER + x ) ];
					}
				}
			}
		}

		if( tc.X > 0 && tc.Z < tileCountZ - 1 )
		{
			if( HasMask( tc.X - 1, tc.Z + 1, tc.L, tc.M ) )
			{
				UnpackMask( &tempUShorts1, packedTexture, unpackedTexture, tc.X - 1, tc.Z + 1, tc.L, tc.M );

				for( int z = 0, e = FULL_MEGA_TEX_BORDER; z < e; z ++ )
				{
					for( int x = 0, e = FULL_MEGA_TEX_BORDER; x < e; x ++ )
					{
						tempUShorts0[ x + ( z + qs.MaskAtlasTileDim - FULL_MEGA_TEX_BORDER ) * qs.MaskAtlasTileDim ] = tempUShorts1[ ( FULL_MEGA_TEX_BORDER + z ) * qs.MaskAtlasTileDim + ( qs.MaskAtlasTileDim - 2 * FULL_MEGA_TEX_BORDER + x ) ];
					}
				}
			}
		}

		if( tc.X < tileCountX - 1 && tc.Z > 0 )
		{
			if( HasMask( tc.X + 1, tc.Z - 1, tc.L, tc.M ) )
			{
				UnpackMask( &tempUShorts1, packedTexture, unpackedTexture, tc.X + 1, tc.Z - 1, tc.L, tc.M );

				for( int z = 0, e = FULL_MEGA_TEX_BORDER; z < e; z ++ )
				{
					for( int x = 0, e = FULL_MEGA_TEX_BORDER; x < e; x ++ )
					{
						tempUShorts0[ ( x + qs.MaskAtlasTileDim - FULL_MEGA_TEX_BORDER ) + z * qs.MaskAtlasTileDim ] = tempUShorts1[ ( qs.MaskAtlasTileDim - 2 * FULL_MEGA_TEX_BORDER + z ) * qs.MaskAtlasTileDim + ( FULL_MEGA_TEX_BORDER + x ) ];
					}
				}
			}
		}

		//------------------------------------------------------------------------

		if( tc.X > 0 )
		{
			if( HasMask( tc.X - 1, tc.Z, tc.L, tc.M ) )
			{
				UnpackMask( &tempUShorts1, packedTexture, unpackedTexture, tc.X - 1, tc.Z, tc.L, tc.M );

				for( int z = 0, e = qs.MaskAtlasTileDim - 2 * FULL_MEGA_TEX_BORDER; z < e; z ++ )
				{
					for( int x = 0, e = FULL_MEGA_TEX_BORDER; x < e; x ++ )
					{
						tempUShorts0[ x + ( z + FULL_MEGA_TEX_BORDER ) * qs.MaskAtlasTileDim ] = tempUShorts1[ ( z + FULL_MEGA_TEX_BORDER ) * qs.MaskAtlasTileDim + ( qs.MaskAtlasTileDim - 2 * FULL_MEGA_TEX_BORDER + x ) ];
					}
				}
			}
		}

		if( tc.X < tileCountX - 1 )
		{
			if( HasMask( tc.X + 1, tc.Z, tc.L, tc.M ) )
			{
				UnpackMask( &tempUShorts1, packedTexture, unpackedTexture, tc.X + 1, tc.Z, tc.L, tc.M );

				for( int z = 0, e = qs.MaskAtlasTileDim - 2 * FULL_MEGA_TEX_BORDER; z < e; z ++ )
				{
					for( int x = 0, e = FULL_MEGA_TEX_BORDER; x < e; x ++ )
					{
						tempUShorts0[ x +  qs.MaskAtlasTileDim - FULL_MEGA_TEX_BORDER + ( z + FULL_MEGA_TEX_BORDER ) * qs.MaskAtlasTileDim ] = tempUShorts1[ ( z + FULL_MEGA_TEX_BORDER ) * qs.MaskAtlasTileDim + ( FULL_MEGA_TEX_BORDER + x ) ];
					}
				}
			}
		}

		//------------------------------------------------------------------------

		if( tc.Z > 0 )
		{
			if( HasMask( tc.X, tc.Z - 1, tc.L, tc.M ) )
			{
				UnpackMask( &tempUShorts1, packedTexture, unpackedTexture, tc.X, tc.Z - 1, tc.L, tc.M );

				for( int x = 0, e = qs.MaskAtlasTileDim - 2 * FULL_MEGA_TEX_BORDER; x < e; x ++ )
				{
					for( int z = 0, e = FULL_MEGA_TEX_BORDER; z < e; z ++ )
					{
						tempUShorts0[ z * qs.MaskAtlasTileDim + ( x + FULL_MEGA_TEX_BORDER ) ] = tempUShorts1[ ( x + FULL_MEGA_TEX_BORDER ) + ( qs.MaskAtlasTileDim - 2 * FULL_MEGA_TEX_BORDER + z ) * qs.MaskAtlasTileDim ];
					}
				}
			}
		}

		if( tc.Z < tileCountZ - 1 )
		{
			if( HasMask( tc.X, tc.Z + 1, tc.L, tc.M ) )
			{
				UnpackMask( &tempUShorts1, packedTexture, unpackedTexture, tc.X, tc.Z + 1, tc.L, tc.M );

				for( int x = 0, e = qs.MaskAtlasTileDim - 2 * FULL_MEGA_TEX_BORDER; x < e; x ++ )
				{
					for( int z = 0, e = FULL_MEGA_TEX_BORDER; z < e; z ++ )
					{
						tempUShorts0[ ( z + qs.MaskAtlasTileDim - FULL_MEGA_TEX_BORDER ) * qs.MaskAtlasTileDim + ( x + FULL_MEGA_TEX_BORDER ) ] = tempUShorts1[ ( x + FULL_MEGA_TEX_BORDER ) + ( FULL_MEGA_TEX_BORDER + z ) * qs.MaskAtlasTileDim  ];
					}
				}
			}
		}

		//------------------------------------------------------------------------

		UpdateMask( &tempUShorts0, packedTexture, unpackedTexture, tc.X, tc.Z, tc.L, tc.M );
	}
}

//------------------------------------------------------------------------

void r3dTerrain3::UpdateLayerMaskMipFromLevelBelow( int tx, int tz, int L, int maskId, 
													UShorts& tempUShorts0, UShorts& tempUShorts1, 
													IDirect3DTexture9* packedTexture, 
													IDirect3DTexture9* unpackedTexture )
{
	const r3dTerrain3::QualitySettings& qs = GetCurrentQualitySettings();

	int orgDim = qs.MaskAtlasTileDim;
	int dim = orgDim / 2;

	enum
	{
		HALF_MEGA_TEX_BORDER = r3dTerrain3::MEGA_TEX_BORDER / 2
	};

	UnpackMask( &tempUShorts1, packedTexture, unpackedTexture, tx, tz, L, maskId );

	for( int ttz = tz * 2, e = ( tz + 1 ) * 2; ttz < e; ttz ++ )
	{
		for( int ttx = tx * 2, e = ( tx + 1 ) * 2; ttx < e; ttx ++ )
		{
			int xStart = 0;
			int zStart = 0;

			if( !HasMask( ttx, ttz, L - 1, maskId ) )
				continue;

			UnpackMask( &tempUShorts0, packedTexture, unpackedTexture, ttx, ttz, L - 1, maskId );

			int relToPrevX = ( ttx - ( ttx >> 1 << 1 ) );
			int relToPrevZ = ( ttz - ( ttz >> 1 << 1 ) );

			int xTargetStartOrg = ( ttx - ( ttx >> 1 << 1 ) ) * dim;
			int zTargetStartOrg = ( ttz - ( ttz >> 1 << 1 ) ) * dim;

			int xTargetStart = xTargetStartOrg;
			int zTargetStart = zTargetStartOrg;

			if( relToPrevX )
				xTargetStart -= HALF_MEGA_TEX_BORDER;
			else
				xTargetStart += HALF_MEGA_TEX_BORDER;

			if( relToPrevZ )
				zTargetStart -= HALF_MEGA_TEX_BORDER;
			else
				zTargetStart += HALF_MEGA_TEX_BORDER;

			int xTargetEnd = xTargetStart + dim;
			int zTargetEnd = zTargetStart + dim;

			if( xStart >= r3dTerrain3::MEGA_TEX_BORDER )
			{
				xStart -= r3dTerrain3::MEGA_TEX_BORDER;
				xTargetStart -= HALF_MEGA_TEX_BORDER;
			}

			if( zStart >= r3dTerrain3::MEGA_TEX_BORDER )
			{
				zStart -= r3dTerrain3::MEGA_TEX_BORDER;
				zTargetStart -= HALF_MEGA_TEX_BORDER;
			}

			if( xStart + dim * 2 < orgDim - r3dTerrain3::MEGA_TEX_BORDER )
			{
				xTargetEnd += HALF_MEGA_TEX_BORDER;
			}

			if( zStart + dim * 2 < orgDim - r3dTerrain3::MEGA_TEX_BORDER )
			{
				zTargetEnd += HALF_MEGA_TEX_BORDER;
			}

			for( int z = zTargetStart, lz = 0; z < zTargetEnd; z ++, lz ++ )
			{
				for( int x = xTargetStart, lx = 0; x < xTargetEnd; x ++, lx ++ )
				{
					int srcIdxes[ 4 ];

					srcIdxes[ 0 ] = ( zStart + lz * 2 + 0 ) * qs.MaskAtlasTileDim + xStart + lx * 2 + 0;
					srcIdxes[ 1 ] = ( zStart + lz * 2 + 1 ) * qs.MaskAtlasTileDim + xStart + lx * 2 + 0;
					srcIdxes[ 2 ] = ( zStart + lz * 2 + 0 ) * qs.MaskAtlasTileDim + xStart + lx * 2 + 1;
					srcIdxes[ 3 ] = ( zStart + lz * 2 + 1 ) * qs.MaskAtlasTileDim + xStart + lx * 2 + 1;

					int rsum = 0;
					int gsum = 0;
					int bsum = 0;

					union
					{
						struct
						{
							UINT16 r : 5;
							UINT16 g : 6;
							UINT16 b : 5;
						};

						UINT16 sample;
					};

					for( int i = 0; i < 4; i ++ )
					{
						sample = tempUShorts0[ srcIdxes[ i ] ];

						rsum += r;
						gsum += g;
						bsum += b;
					}

					rsum /= 4;
					gsum /= 4;
					bsum /= 4;

					r = rsum;
					g = gsum;
					b = bsum;

					tempUShorts1[ z * qs.MaskAtlasTileDim + x ] = sample;
				}
			}
		}
	}

	UpdateMask( &tempUShorts1, packedTexture, unpackedTexture, tx, tz, L, maskId );

}

//------------------------------------------------------------------------

struct TwoTextures
{
	IDirect3DTexture9 * packedTexture;
	IDirect3DTexture9 * unpackedTexture;
};

static void CreateTexturesInMainThread( void * params )
{
	const r3dTerrain3::QualitySettings& qs = Terrain3->GetCurrentQualitySettings();
	IDirect3DTexture9 * tempUnpackedNormTexture( NULL );
	D3D_V( r3dRenderer->pd3ddev->CreateTexture( qs.MaskAtlasTileDim, qs.MaskAtlasTileDim, 1, 0, D3DFMT_R5G6B5, D3DPOOL_SYSTEMMEM, &tempUnpackedNormTexture, NULL ) );

	IDirect3DTexture9 * tempPackedNormTexture;
	D3DFORMAT fmt = Terrain3->GetInfo().MaskFormat;
	D3D_V( r3dRenderer->pd3ddev->CreateTexture( qs.MaskAtlasTileDim, qs.MaskAtlasTileDim, 1, 0, fmt, D3DPOOL_SYSTEMMEM, &tempPackedNormTexture, NULL ) );

	TwoTextures* twoTexes = static_cast< TwoTextures* > ( params );

	twoTexes->packedTexture = tempPackedNormTexture;
	twoTexes->unpackedTexture = tempUnpackedNormTexture;
}

static void ReleaseTexturesInMainThread( void * params )
{
	TwoTextures* twoTexes = static_cast< TwoTextures* > ( params );

	SAFE_RELEASE( twoTexes->packedTexture );
	SAFE_RELEASE( twoTexes->unpackedTexture );
}

void r3dTerrain3::CreateMipFile( const char* levelPath )
{
	int hadEditorBlankMask = m_EDITOR_BlankMask.Valid();

	char editorPath[ 1024 ];
	GetEditorFilePath( editorPath );

	INT64 space = r3dGetDiskSpace( editorPath );

	r3dOutToLog( "r3dTerrain3::CreateMipFile: we have %.2f GB free space.\n", float( space / 1024.0 / 1024.0 / 1024.0 ) );

	if( space < 2ull * 1024ULL * 1024ULL * 1024ULL )
	{
		if( r3dRenderer->bFullScreen )
		{
			ShowWindow( r3dRenderer->HLibWin, SW_MINIMIZE );
		}

		MessageBoxA( r3dRenderer->HLibWin, "You need at least 2 GB of free space on your system drive ( C: ). Otherwise the game may crash!", "WARNING", MB_ICONEXCLAMATION );
	}

	CreateEditorBlankMask();

	r3dOutToLog( "r3dTerrain3::CreateMipFile.." );

	float timeStart = r3dGetTime();

	if( m_MegaTexAtlasFile_EDITOR == INVALID_HANDLE_VALUE )
		OpenEditorFile();

	DeleteMegaTexAtlasMipsFile();
	OpenMegaTexAtlasMipsFile( "w+b" );

	TwoTextures texes;
	
	ProcessCustomDeviceQueueItem( CreateTexturesInMainThread, &texes );

	Info info = GetInfo();

	float range_div_3 = r3dITerrain::LoadingProgress / 3.f;

	for( int i = 1, e = info.NumActiveMegaTexLayers; i < e; i ++ )
	{
		AdvanceLoadingProgress( range_div_3 / ( info.NumActiveMegaTexLayers - 1 ) );

		int tileCountX = info.MegaTileCountX >> i;
		int tileCountZ = info.MegaTileCountZ >> i;

		r3dTL::TArray< UINT16 > shorts00;
		r3dTL::TArray< UINT16 > shorts10;
		r3dTL::TArray< UINT16 > shorts01;
		r3dTL::TArray< UINT16 > shorts11;

		r3dTL::TArray< UINT16 > resultingShorts;

		Bytes tempNormalData;

		UShorts heightsForNorms[ 9 ];
		Floats tempFloats;
		Vectors tempVectors0, tempVectors1;
		UShorts tempForNorms;

		for( int z = 0, e = tileCountZ; z < e; z ++ )
		{
			for( int x = 0, e = tileCountX; x < e; x ++ )
			{
				UnpackHeight( &shorts00, x * 2 + 0, z * 2 + 0, i - 1 );
				UnpackHeight( &shorts10, x * 2 + 1, z * 2 + 0, i - 1 );
				UnpackHeight( &shorts01, x * 2 + 0, z * 2 + 1, i - 1 );
				UnpackHeight( &shorts11, x * 2 + 1, z * 2 + 1, i - 1 );

				resultingShorts.Resize( shorts00.Count() );

				for( int ii = 0; ii < 4; ii ++ )
				{
					int offsetX = 0;
					int offsetZ = 0;

					r3dTL::TArray< UINT16 >* usedShorts( NULL );

					int halfTileDim = m_QualitySettings.MaskAtlasTileDim / 2;

					switch( ii )
					{
					case 0:
						offsetX = 0;
						offsetZ = 0;
						usedShorts = &shorts00;
						break;
					case 1:
						offsetX = halfTileDim;
						offsetZ = 0;
						usedShorts = &shorts10;
						break;
					case 2:
						offsetX = 0;
						offsetZ = halfTileDim;
						usedShorts = &shorts01;
						break;
					case 3:
						offsetX = halfTileDim;
						offsetZ = halfTileDim;
						usedShorts = &shorts11;
						break;
					}

					for( int zz = 0, ee = m_QualitySettings.MaskAtlasTileDim / 2; zz < ee; zz ++ )
					{
						for( int xx = 0, ee = m_QualitySettings.MaskAtlasTileDim / 2; xx < ee; xx ++ )
						{
							int x2 = xx * 2;
							int z2 = zz * 2;
							int height	 = ( *usedShorts )[ x2 + 0 + ( z2 + 0 ) * m_QualitySettings.MaskAtlasTileDim ];
							height		+= ( *usedShorts )[ x2 + 1 + ( z2 + 0 ) * m_QualitySettings.MaskAtlasTileDim ];
							height		+= ( *usedShorts )[ x2 + 0 + ( z2 + 1 ) * m_QualitySettings.MaskAtlasTileDim ];
							height		+= ( *usedShorts )[ x2 + 1 + ( z2 + 1 ) * m_QualitySettings.MaskAtlasTileDim ];

							resultingShorts[ ( xx + offsetX ) + ( zz + offsetZ ) * m_QualitySettings.MaskAtlasTileDim ] = height / 4;
						}
					}
				}

				WriteHeightToMipFile( m_MegaTexAtlasMipsFile, &resultingShorts, x, z, i );
			}
		}

		for( int z = 0, e = tileCountZ; z < e; z ++ )
		{
			for( int x = 0, e = tileCountX; x < e; x ++ )
			{
				const UShorts* heightsPtr[ 9 ] = { NULL };

				if( x > 0 )
				{
					if( z > 0 )
					{
						heightsPtr[ 0 ] = &heightsForNorms[ 0 ];
						UnpackHeight( &heightsForNorms[ 0 ], x - 1, z - 1, i );
					}

					heightsPtr[ 3 ] = &heightsForNorms[ 3 ];
					UnpackHeight( &heightsForNorms[ 3 ], x - 1, z, i );

					if( z < tileCountZ - 1 )
					{
						heightsPtr[ 6 ] = &heightsForNorms[ 6 ];
						UnpackHeight( &heightsForNorms[ 6 ], x - 1, z + 1, i );
					}
				}

				if( x < tileCountX - 1 )
				{
					if( z > 0 )
					{
						heightsPtr[ 2 ] = &heightsForNorms[ 2 ];
						UnpackHeight( &heightsForNorms[ 2 ], x + 1, z - 1, i );
					}

					heightsPtr[ 5 ] = &heightsForNorms[ 5 ];
					UnpackHeight( &heightsForNorms[ 5 ], x + 1, z, i );

					if( z < tileCountZ - 1 )
					{
						heightsPtr[ 8 ] = &heightsForNorms[ 8 ];
						UnpackHeight( &heightsForNorms[ 8 ], x + 1, z + 1, i );
					}
				}

				if( z > 0 )
				{
					heightsPtr[ 1 ] = &heightsForNorms[ 1 ];
					UnpackHeight( &heightsForNorms[ 1 ], x, z - 1, i );
				}

				heightsPtr[ 4 ] = &heightsForNorms[ 4 ];
				UnpackHeight( &heightsForNorms[ 4 ], x, z, i );

				if( z < tileCountZ - 1 )
				{
					heightsPtr[ 7 ] = &heightsForNorms[ 7 ];
					UnpackHeight( &heightsForNorms[ 7 ], x, z + 1, i );
				}

				RecalcNormalMap( heightsPtr, &tempFloats, &tempVectors0, &tempVectors1, &tempForNorms, i );
				WriteNormalsToMipFile( m_MegaTexAtlasMipsFile, &tempForNorms, x, z, i, texes.unpackedTexture, texes.packedTexture, &heightsForNorms[ 0 ] );
			}
		}
	}

	//------------------------------------------------------------------------

	fclose( m_MegaTexAtlasMipsFile );
	m_MegaTexAtlasMipsFile = NULL;

	//------------------------------------------------------------------------

	UShorts tempUShorts0;
	UShorts tempUShorts1;

	int mdim = GetCurrentQualitySettings().MaskAtlasTileDim;

	tempUShorts0.Clear();
	tempUShorts0.Resize( mdim * mdim, 0 );

	int maskCount = GetMaskCount();

	for( int L = 1, e = info.NumActiveMegaTexLayers; L < e; L ++ )
	{
		AdvanceLoadingProgress( range_div_3 / ( info.NumActiveMegaTexLayers - 1 ) );

		for( int tz = 0, e = info.MegaTileCountZ >> L; tz < e; tz ++ )
		{
			for( int tx = 0, e = info.MegaTileCountX >> L; tx < e; tx ++ )
			{
				for( int m = 0, e = maskCount; m < e; m ++ )
				{
					if( Terrain3->HasMask( tx, tz, L, m ) )
					{
						Terrain3->UpdateMask( &tempUShorts0, texes.packedTexture, texes.unpackedTexture, tx, tz, L, m );
					}
				}
			}
		}
	}

	//------------------------------------------------------------------------

	for( int L = 1, e = info.NumActiveMegaTexLayers; L < e; L ++ )
	{
		AdvanceLoadingProgress( range_div_3 / ( info.NumActiveMegaTexLayers - 1 ) );

		for( int tz = 0, e = info.MegaTileCountZ >> L; tz < e; tz ++ )
		{
			for( int tx = 0, e = info.MegaTileCountX >> L; tx < e; tx ++ )
			{
				for( int m = 0, e = maskCount; m < e; m ++ )
				{
					if( Terrain3->HasMask( tx * 2 + 0, tz * 2 + 0, L - 1, m ) ||
						Terrain3->HasMask( tx * 2 + 1, tz * 2 + 0, L - 1, m ) ||
						Terrain3->HasMask( tx * 2 + 0, tz * 2 + 1, L - 1, m ) ||
						Terrain3->HasMask( tx * 2 + 1, tz * 2 + 1, L - 1, m )
						)
					{
						UpdateLayerMaskMipFromLevelBelow( tx, tz, L, m, tempUShorts0, tempUShorts1, texes.packedTexture, texes.unpackedTexture );
					}
				}
			}
		}

		TileCoords tileCoords;

		for( int tz = 0, e = info.MegaTileCountZ >> L; tz < e; tz ++ )
		{
			for( int tx = 0, e = info.MegaTileCountX >> L; tx < e; tx ++ )
			{	
				for( int m = 0; m < maskCount; m ++ )
				{
					if( Terrain3->HasMask( tx, tz, L, m ) )
					{
						TileCoord tc;

						tc.X = tx;
						tc.Z = tz;
						tc.L = L;
						tc.M = m;

						tileCoords.PushBack( tc );
					}
				}
			}
		}

		RecalcTileCoordBorders( tileCoords, tempUShorts0, tempUShorts1, texes.packedTexture, texes.unpackedTexture, NULL );
	}

	MergeEditedMega( levelPath );

	ProcessCustomDeviceQueueItem( ReleaseTexturesInMainThread, &texes );

	WriteMipStamps( levelPath );

	if( !hadEditorBlankMask )
	{
		m_EDITOR_BlankMask.ReleaseAndReset();
	}

	r3dOutToLog( "done in %.2f seconds\n", r3dGetTime() - timeStart );
}

//------------------------------------------------------------------------

r3dTerrain3::Bytes r3dTerrain3::GenerateStamp()
{
	m_StampID ++;

	char keyVal[ 512 ];
	DWORD BufferSize = sizeof keyVal;

	Bytes res;

	res.Resize( 18 );
	int n = sprintf( (char*)&res[ 0 ], "%08d:%08d", m_StampID, rand() );
	r3d_assert( n == 17 );
	res.Resize( 17 );

	HKEY key;

	if( RegOpenKeyEx( HKEY_LOCAL_MACHINE, _T( "SOFTWARE\\Microsoft\\Cryptography\\" ), 0, KEY_WOW64_64KEY | KEY_QUERY_VALUE, &key ) == ERROR_SUCCESS )
	{
		DWORD resCode = RegQueryValueEx( key, _T( "MachineGuid" ), NULL, NULL, (LPBYTE)&keyVal, &BufferSize );
		if( resCode == ERROR_SUCCESS )
		{
			int oldSize = (int)res.Count();
			res.Resize( oldSize + BufferSize );
			memcpy( &res[ oldSize ], keyVal, BufferSize );
		}

		RegCloseKey( key );
	}

	if( res.Count() && !res[ res.Count() - 1 ] )
	{
		res.Resize( res.Count() - 1 );
	}

	return res;
}

//------------------------------------------------------------------------

r3dTerrain3* Terrain3;

//------------------------------------------------------------------------

#if R3D_TERRAIN_V3_GRAPHICS

static int ConstructConnectionIndices( uint16_t* target, int vertexTileDim, int sideLodConnections )
{
	uint16_t* start = target;

	const int CELL_VERT_SIZE = vertexTileDim + 1;

	const int TO_NEXT_LINE	= CELL_VERT_SIZE;

	int northVertStart	= ( vertexTileDim + 1 ) * ( vertexTileDim + 1 );
	int southVertStart	= northVertStart	+ vertexTileDim;
	int westVertStart	= southVertStart	+ vertexTileDim;
	int eastVertStart	= westVertStart		+ vertexTileDim;

	int fringeStart_NORTH = ( vertexTileDim + 1 ) * ( vertexTileDim + 1 ) + vertexTileDim * 4;
	int fringeStart_SOUTH = fringeStart_NORTH + vertexTileDim * 2 + 1;
	int fringeStart_WEST = fringeStart_SOUTH + vertexTileDim * 2 + 1;
	int fringeStart_EAST = fringeStart_WEST + vertexTileDim * 2 + 1;

#define CONSTRUCT_BORDER_NORTH_SINGLE							\
	if( start ) *target = 0;						target ++;	\
	if( start ) *target = 1;						target ++;	\
	if( start ) *target = 8;						target ++;	\
																\
	if( start ) *target = 8;						target ++;	\
	if( start ) *target = 1;						target ++;	\
	if( start ) *target = 10;						target ++;

#define CONSTRUCT_BORDER_SOUTH_SINGLE							\
	if( start ) *target = 2;						target ++;	\
	if( start ) *target = 11;						target ++;	\
	if( start ) *target = 3;						target ++;	\
																\
	if( start ) *target = 3;						target ++;	\
	if( start ) *target = 11;						target ++;	\
	if( start ) *target = 13;						target ++;

#define CONSTRUCT_BORDER_EAST_SINGLE							\
	if( start ) *target = 1;						target ++;	\
	if( start ) *target = 3;						target ++;	\
	if( start ) *target = 10;						target ++;	\
																\
	if( start ) *target = 10;						target ++;	\
	if( start ) *target = 3;						target ++;	\
	if( start ) *target = 13;						target ++;

#define CONSTRUCT_BORDER_WEST_SINGLE							\
	if( start ) *target = 0;						target ++;	\
	if( start ) *target = 8;						target ++;	\
	if( start ) *target = 2;						target ++;	\
																\
	if( start ) *target = 8;						target ++;	\
	if( start ) *target = 11;						target ++;	\
	if( start ) *target = 2;						target ++;

#define CONSTRUCT_BORDER_NORTH_DOUBLE							\
	if( start ) *target = 0;						target ++;	\
	if( start ) *target = 4;						target ++;	\
	if( start ) *target = 8;						target ++;	\
																\
	if( start ) *target = 4;						target ++;	\
	if( start ) *target = 9;						target ++;	\
	if( start ) *target = 8;						target ++;	\
																\
	if( start ) *target = 4;						target ++;	\
	if( start ) *target = 1;						target ++;	\
	if( start ) *target = 9;						target ++;	\
																\
	if( start ) *target = 1;						target ++;	\
	if( start ) *target = 10;						target ++;	\
	if( start ) *target = 9;						target ++;

#define CONSTRUCT_BORDER_EAST_DOUBLE							\
	if( start ) *target = 1;						target ++;	\
	if( start ) *target = 7;						target ++;	\
	if( start ) *target = 10;						target ++;	\
																\
	if( start ) *target = 10;						target ++;	\
	if( start ) *target = 7;						target ++;	\
	if( start ) *target = 18;						target ++;	\
																\
	if( start ) *target = 7;						target ++;	\
	if( start ) *target = 3;						target ++;	\
	if( start ) *target = 18;						target ++;	\
																\
	if( start ) *target = 19;						target ++;	\
	if( start ) *target = 18;						target ++;	\
	if( start ) *target = 3;						target ++;

#define CONSTRUCT_BORDER_SOUTH_DOUBLE							\
	if( start ) *target = 2;						target ++;	\
	if( start ) *target = 11;						target ++;	\
	if( start ) *target = 5;						target ++;	\
																\
	if( start ) *target = 5;						target ++;	\
	if( start ) *target = 11;						target ++;	\
	if( start ) *target = 12;						target ++;	\
																\
	if( start ) *target = 5;						target ++;	\
	if( start ) *target = 12;						target ++;	\
	if( start ) *target = 3;						target ++;	\
																\
	if( start ) *target = 3;						target ++;	\
	if( start ) *target = 12;						target ++;	\
	if( start ) *target = 13;						target ++;

#define CONSTRUCT_BORDER_WEST_DOUBLE							\
	if( start ) *target = 0;						target ++;	\
	if( start ) *target = 8;						target ++;	\
	if( start ) *target = 6;						target ++;	\
																\
	if( start ) *target = 6;						target ++;	\
	if( start ) *target = 8;						target ++;	\
	if( start ) *target = 15;						target ++;	\
																\
	if( start ) *target = 6;						target ++;	\
	if( start ) *target = 15;						target ++;	\
	if( start ) *target = 2;						target ++;	\
																\
	if( start ) *target = 2;						target ++;	\
	if( start ) *target = 15;						target ++;	\
	if( start ) *target = 11;						target ++;

	if( vertexTileDim == 1 )
	{
		int vidx = 0;

		switch( sideLodConnections )
		{
		case 0:
			if( start ) *target = vidx;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx + 1;					target ++;

			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	target ++;
			if( start ) *target = vidx + 1;					target ++;

			CONSTRUCT_BORDER_NORTH_SINGLE
			CONSTRUCT_BORDER_SOUTH_SINGLE
			CONSTRUCT_BORDER_EAST_SINGLE
			CONSTRUCT_BORDER_WEST_SINGLE
			break;

		case NORTH_CONNECTION:
			if( start ) *target = vidx;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = northVertStart;			target ++;

			if( start ) *target = northVertStart;			target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx + 1;					target ++;

			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	target ++;
			if( start ) *target = vidx + 1;					target ++;

			CONSTRUCT_BORDER_SOUTH_SINGLE
			CONSTRUCT_BORDER_EAST_SINGLE
			CONSTRUCT_BORDER_WEST_SINGLE
			CONSTRUCT_BORDER_NORTH_DOUBLE
			break;

		case EAST_CONNECTION:
			if( start ) *target = vidx;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx + 1;					target ++;

			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = eastVertStart;			target ++;
			if( start ) *target = vidx + 1;					target ++;

			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	target ++;
			if( start ) *target = eastVertStart;			target ++;

			CONSTRUCT_BORDER_SOUTH_SINGLE
			CONSTRUCT_BORDER_WEST_SINGLE
			CONSTRUCT_BORDER_NORTH_SINGLE
			CONSTRUCT_BORDER_EAST_DOUBLE
			break;

		case NORTH_CONNECTION | EAST_CONNECTION:
			if( start ) *target = vidx;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = northVertStart;			target ++;
			
			if( start ) *target = northVertStart;			target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx + 1;					target ++;
			
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = eastVertStart;			target ++;
			if( start ) *target = vidx + 1;					target ++;

			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	target ++;
			if( start ) *target = eastVertStart;			target ++;

			CONSTRUCT_BORDER_SOUTH_SINGLE
			CONSTRUCT_BORDER_WEST_SINGLE
			CONSTRUCT_BORDER_NORTH_DOUBLE
			CONSTRUCT_BORDER_EAST_DOUBLE
			break;

		case SOUTH_CONNECTION:
			if( start ) *target = vidx;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx + 1;					target ++;
			
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = southVertStart;			target ++;
			if( start ) *target = vidx + 1;					target ++;
			
			if( start ) *target = southVertStart;			target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	target ++;
			if( start ) *target = vidx + 1;					target ++;

			CONSTRUCT_BORDER_EAST_SINGLE
			CONSTRUCT_BORDER_WEST_SINGLE
			CONSTRUCT_BORDER_NORTH_SINGLE
			CONSTRUCT_BORDER_SOUTH_DOUBLE
			break;

		case NORTH_CONNECTION | SOUTH_CONNECTION:
			if( start ) *target = vidx;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = northVertStart;			target ++;
			
			if( start ) *target = northVertStart;			target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx + 1;					target ++;
			
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = southVertStart;			target ++;
			if( start ) *target = vidx + 1;					target ++;
			
			if( start ) *target = southVertStart;			target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	target ++;
			if( start ) *target = vidx + 1;					target ++;

			CONSTRUCT_BORDER_EAST_SINGLE
			CONSTRUCT_BORDER_WEST_SINGLE
			CONSTRUCT_BORDER_NORTH_DOUBLE
			CONSTRUCT_BORDER_SOUTH_DOUBLE
			break;

		case EAST_CONNECTION | SOUTH_CONNECTION:
			if( start ) *target = vidx;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx + 1;					target ++;
			
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = southVertStart;			target ++;
			if( start ) *target = vidx + 1;					target ++;
			
			if( start ) *target = vidx + 1;					target ++;
			if( start ) *target = southVertStart;			target ++;
			if( start ) *target = eastVertStart;			target ++;
			
			if( start ) *target = southVertStart;			target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	target ++;
			if( start ) *target = eastVertStart;			target ++;

			CONSTRUCT_BORDER_EAST_DOUBLE
			CONSTRUCT_BORDER_WEST_SINGLE
			CONSTRUCT_BORDER_NORTH_SINGLE
			CONSTRUCT_BORDER_SOUTH_DOUBLE
			break;

		case NORTH_CONNECTION | EAST_CONNECTION | SOUTH_CONNECTION:
			if( start ) *target = vidx;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = northVertStart;			target ++;

			if( start ) *target = northVertStart;			target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx + 1;					target ++;

			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = southVertStart;			target ++;
			if( start ) *target = vidx + 1;					target ++;

			if( start ) *target = vidx + 1;					target ++;
			if( start ) *target = southVertStart;			target ++;
			if( start ) *target = eastVertStart;			target ++;

			if( start ) *target = southVertStart;			target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	target ++;
			if( start ) *target = eastVertStart;			target ++;

			CONSTRUCT_BORDER_EAST_DOUBLE
			CONSTRUCT_BORDER_WEST_SINGLE
			CONSTRUCT_BORDER_NORTH_DOUBLE
			CONSTRUCT_BORDER_SOUTH_DOUBLE
			break;

		case WEST_CONNECTION:
			if( start ) *target = vidx;						target ++;
			if( start ) *target = westVertStart;			target ++;
			if( start ) *target = vidx + 1;					target ++;

			if( start ) *target = vidx + 1;					target ++;
			if( start ) *target = westVertStart;			target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;

			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	target ++;
			if( start ) *target = vidx + 1;					target ++;

			CONSTRUCT_BORDER_EAST_SINGLE
			CONSTRUCT_BORDER_NORTH_SINGLE
			CONSTRUCT_BORDER_SOUTH_SINGLE
			CONSTRUCT_BORDER_WEST_DOUBLE
			break;

		case NORTH_CONNECTION | WEST_CONNECTION :
			if( start ) *target = vidx;						*target ++;
			if( start ) *target = westVertStart;			*target ++;
			if( start ) *target = northVertStart;			*target ++;

			if( start ) *target = northVertStart;			*target ++;
			if( start ) *target = westVertStart;			*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;

			if( start ) *target = northVertStart;			*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;
			if( start ) *target = vidx + 1;					*target ++;

			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	*target ++;
			if( start ) *target = vidx + 1;					*target ++;

			CONSTRUCT_BORDER_EAST_SINGLE
			CONSTRUCT_BORDER_NORTH_DOUBLE
			CONSTRUCT_BORDER_SOUTH_SINGLE
			CONSTRUCT_BORDER_WEST_DOUBLE
			break;

		case EAST_CONNECTION | WEST_CONNECTION:
			if( start ) *target = vidx;						*target ++;
			if( start ) *target = westVertStart;			*target ++;
			if( start ) *target = vidx + 1;					*target ++;

			if( start ) *target = vidx + 1;					*target ++;
			if( start ) *target = westVertStart;			*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;

			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;
			if( start ) *target = eastVertStart;	  		*target ++;
			if( start ) *target = vidx + 1;			  		*target ++;

			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	*target ++;
			if( start ) *target = eastVertStart;			*target ++;

			CONSTRUCT_BORDER_EAST_DOUBLE
			CONSTRUCT_BORDER_NORTH_SINGLE
			CONSTRUCT_BORDER_SOUTH_SINGLE
			CONSTRUCT_BORDER_WEST_DOUBLE
			break;

		case NORTH_CONNECTION | EAST_CONNECTION | WEST_CONNECTION:
			if( start ) *target = vidx;						*target ++;
			if( start ) *target = westVertStart;			*target ++;
			if( start ) *target = northVertStart;			*target ++;

			if( start ) *target = northVertStart;			*target ++;
			if( start ) *target = westVertStart;			*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;

			if( start ) *target = northVertStart;			*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;
			if( start ) *target = vidx + 1;					*target ++;

			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;
			if( start ) *target = eastVertStart;	  		*target ++;
			if( start ) *target = vidx + 1;			  		*target ++;

			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	*target ++;
			if( start ) *target = eastVertStart;			*target ++;

			CONSTRUCT_BORDER_EAST_DOUBLE
			CONSTRUCT_BORDER_NORTH_DOUBLE
			CONSTRUCT_BORDER_SOUTH_SINGLE
			CONSTRUCT_BORDER_WEST_DOUBLE
			break;

		case SOUTH_CONNECTION | WEST_CONNECTION:
			if( start ) *target = vidx;						*target ++;
			if( start ) *target = westVertStart;			*target ++;
			if( start ) *target = vidx + 1;					*target ++;

			if( start ) *target = vidx + 1;					*target ++;
			if( start ) *target = westVertStart;			*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;

			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;
			if( start ) *target = southVertStart;	  		*target ++;
			if( start ) *target = vidx + 1;			  		*target ++;

			if( start ) *target = southVertStart;			*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	*target ++;
			if( start ) *target = vidx + 1;					*target ++;

			CONSTRUCT_BORDER_EAST_SINGLE
			CONSTRUCT_BORDER_NORTH_SINGLE
			CONSTRUCT_BORDER_SOUTH_DOUBLE
			CONSTRUCT_BORDER_WEST_DOUBLE
			break;

		case NORTH_CONNECTION | SOUTH_CONNECTION | WEST_CONNECTION:
			if( start ) *target = vidx;						*target ++;
			if( start ) *target = westVertStart;			*target ++;
			if( start ) *target = northVertStart;			*target ++;

			if( start ) *target = northVertStart;			*target ++;
			if( start ) *target = westVertStart;			*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;

			if( start ) *target = northVertStart;			*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;
			if( start ) *target = vidx + 1;					*target ++;

			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;
			if( start ) *target = southVertStart;			*target ++;
			if( start ) *target = vidx + 1;					*target ++;

			if( start ) *target = southVertStart;			*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	*target ++;
			if( start ) *target = vidx + 1;					*target ++;

			CONSTRUCT_BORDER_EAST_SINGLE
			CONSTRUCT_BORDER_NORTH_DOUBLE
			CONSTRUCT_BORDER_SOUTH_DOUBLE
			CONSTRUCT_BORDER_WEST_DOUBLE
			break;

		case EAST_CONNECTION | SOUTH_CONNECTION | WEST_CONNECTION:

			if( start ) *target = vidx;						*target ++;
			if( start ) *target = westVertStart;			*target ++;
			if( start ) *target = vidx + 1;					*target ++;

			if( start ) *target = vidx + 1;					*target ++;
			if( start ) *target = westVertStart;			*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;

			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;
			if( start ) *target = southVertStart;	  		*target ++;
			if( start ) *target = vidx + 1;			  		*target ++;

			if( start ) *target = vidx + 1;					*target ++;
			if( start ) *target = southVertStart;			*target ++;
			if( start ) *target = eastVertStart;			*target ++;

			if( start ) *target = southVertStart;			*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	*target ++;
			if( start ) *target = eastVertStart;			*target ++;

			CONSTRUCT_BORDER_EAST_DOUBLE
			CONSTRUCT_BORDER_NORTH_SINGLE
			CONSTRUCT_BORDER_SOUTH_DOUBLE
			CONSTRUCT_BORDER_WEST_DOUBLE
			break;

		case NORTH_CONNECTION | EAST_CONNECTION | SOUTH_CONNECTION | WEST_CONNECTION:
			if( start ) *target = vidx;						*target ++;
			if( start ) *target = westVertStart;			*target ++;
			if( start ) *target = northVertStart;			*target ++;

			if( start ) *target = northVertStart;			*target ++;
			if( start ) *target = westVertStart;			*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;

			if( start ) *target = northVertStart;			*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;
			if( start ) *target = vidx + 1;					*target ++;

			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;
			if( start ) *target = southVertStart;	  		*target ++;
			if( start ) *target = vidx + 1;			  		*target ++;

			if( start ) *target = vidx + 1;					*target ++;
			if( start ) *target = southVertStart;			*target ++;
			if( start ) *target = eastVertStart;			*target ++;

			if( start ) *target = southVertStart;			*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	*target ++;
			if( start ) *target = eastVertStart;			*target ++;

			CONSTRUCT_BORDER_EAST_DOUBLE
			CONSTRUCT_BORDER_NORTH_DOUBLE
			CONSTRUCT_BORDER_SOUTH_DOUBLE
			CONSTRUCT_BORDER_WEST_DOUBLE
			break;

		default:
			if( start ) *target = vidx;						*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;
			if( start ) *target = vidx + 1;					*target ++;

			if( start ) *target = vidx + TO_NEXT_LINE;		*target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	*target ++;
			if( start ) *target = vidx + 1;					*target ++;

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
					if( start ) *target = vidx;						target ++;
					if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
					if( start ) *target = vidx + 1;					target ++;

					if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
					if( start ) *target = vidx + TO_NEXT_LINE + 1;	target ++;
					if( start ) *target = vidx + 1;					target ++;
				}

				vidx += 3;
			}
		}

		// construct north connection
		if( sideLodConnections & NORTH_CONNECTION )
		{
			int vidx = 0;
			int f = fringeStart_NORTH;

			// exterior 0

			if( !NorthAndWest )
			{
				if( start ) *target = vidx;					target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;	target ++;
				if( start ) *target = northVertStart;		target ++;

				if( start ) *target = northVertStart;		target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;	target ++;
				if( start ) *target = vidx + 1;				target ++;
			}

			//------------------------------------------------------------------------	
			if( start ) *target = vidx;							target ++;
			if( start ) *target = northVertStart + 0;			target ++;
			if( start ) *target = f;							target ++;

			if( start ) *target = f;							target ++;
			if( start ) *target = northVertStart + 0;			target ++;
			if( start ) *target = f + 1;						target ++;

			if( start ) *target = northVertStart + 0;			target ++;
			if( start ) *target = vidx + 1;						target ++;
			if( start ) *target = f + 1;						target ++;

			if( start ) *target = f + 1;						target ++;
			if( start ) *target = vidx + 1;						target ++;
			if( start ) *target = f + 2;						target ++;
			//------------------------------------------------------------------------

			vidx = 1;
			f += 2;

			// interior
			for( int i = 1, e = vertexTileDim - 1; i < e; i ++, vidx ++, f += 2 )
			{
				if( start ) *target = vidx;						target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
				if( start ) *target = northVertStart + i;		target ++;

				if( start ) *target = northVertStart + i;		target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
				if( start ) *target = vidx + 1;					target ++;

				if( start ) *target = vidx + 1;					target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
				if( start ) *target = vidx + TO_NEXT_LINE + 1;	target ++;

				//------------------------------------------------------------------------	
				if( start ) *target = vidx;							target ++;
				if( start ) *target = northVertStart + i;			target ++;
				if( start ) *target = f;							target ++;

				if( start ) *target = f;							target ++;
				if( start ) *target = northVertStart + i;			target ++;
				if( start ) *target = f + 1;						target ++;

				if( start ) *target = northVertStart + i;			target ++;
				if( start ) *target = vidx + 1;						target ++;
				if( start ) *target = f + 1;						target ++;

				if( start ) *target = f + 1;						target ++;
				if( start ) *target = vidx + 1;						target ++;
				if( start ) *target = f + 2;						target ++;
				//------------------------------------------------------------------------
			}

			// exterior 1

			if( start ) *target = vidx;									target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;					target ++;
			if( start ) *target = northVertStart + vertexTileDim - 1;	target ++;

			if( start ) *target = northVertStart + vertexTileDim - 1;	target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;					target ++;
			if( start ) *target = vidx + 1;								target ++;

			//------------------------------------------------------------------------	
			if( start ) *target = vidx;									target ++;
			if( start ) *target = northVertStart + vertexTileDim - 1;	target ++;
			if( start ) *target = f;									target ++;

			if( start ) *target = f;									target ++;
			if( start ) *target = northVertStart + vertexTileDim - 1;	target ++;
			if( start ) *target = f + 1;								target ++;

			if( start ) *target = northVertStart + vertexTileDim - 1;	target ++;
			if( start ) *target = vidx + 1;								target ++;
			if( start ) *target = f + 1;								target ++;

			if( start ) *target = f + 1;								target ++;
			if( start ) *target = vidx + 1;								target ++;
			if( start ) *target = f + 2;								target ++;
			//------------------------------------------------------------------------
		}
		else
		{
			int vidx = 0;
			int f = fringeStart_NORTH;

			// exterior 0
			if( !NotNorthAndNotWest )
			{
				if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
				if( start ) *target = vidx + TO_NEXT_LINE + 1;	target ++;
				if( start ) *target = vidx + 1;					target ++;
			}

			//------------------------------------------------------------------------
			if( start ) *target = vidx;							target ++;
			if( start ) *target = vidx + 1;						target ++;
			if( start ) *target = f;							target ++;

			if( start ) *target = f;							target ++;
			if( start ) *target = vidx + 1;						target ++;
			if( start ) *target = f + 2;						target ++;
			//------------------------------------------------------------------------

			vidx += 1;
			f += 2;

			// interior
			for( int i = 1; i < vertexTileDim - 1; i += 1, vidx += 1, f += 2 )
			{
				if( start ) *target = vidx;						target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
				if( start ) *target = vidx + 1;					target ++;

				if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
				if( start ) *target = vidx + TO_NEXT_LINE + 1;	target ++;
				if( start ) *target = vidx + 1;					target ++;

				//------------------------------------------------------------------------	
				if( start ) *target = vidx;							target ++;
				if( start ) *target = vidx + 1;						target ++;
				if( start ) *target = f;							target ++;

				if( start ) *target = f;							target ++;
				if( start ) *target = vidx + 1;						target ++;
				if( start ) *target = f + 2;						target ++;
				//------------------------------------------------------------------------
			}

			// exterior 1
			if( start ) *target = vidx;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx + 1;					target ++;

			//------------------------------------------------------------------------
			if( start ) *target = vidx;							target ++;
			if( start ) *target = vidx + 1;						target ++;
			if( start ) *target = f;							target ++;

			if( start ) *target = f;							target ++;
			if( start ) *target = vidx + 1;						target ++;
			if( start ) *target = f + 2;						target ++;
			//------------------------------------------------------------------------
		}

		// construct east connection
		if( sideLodConnections & EAST_CONNECTION )
		{
			int vidx = CELL_VERT_SIZE - 1;
			int f = fringeStart_EAST;

			// exterior 0
			{
				if( start ) *target = vidx;						target ++;
				if( start ) *target = vidx + TO_NEXT_LINE - 1;	target ++;
				if( start ) *target = eastVertStart;			target ++;

				if( start ) *target = eastVertStart;			target ++;
				if( start ) *target = vidx + TO_NEXT_LINE - 1;	target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			}

			if( start ) *target = vidx;							target ++;
			if( start ) *target = eastVertStart + 0;			target ++;
			if( start ) *target = f;							target ++;

			if( start ) *target = f;							target ++;
			if( start ) *target = eastVertStart + 0;			target ++;
			if( start ) *target = f + 1;						target ++;

			if( start ) *target = eastVertStart + 0;			target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;			target ++;
			if( start ) *target = f + 1;						target ++;

			if( start ) *target = f + 1;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;			target ++;
			if( start ) *target = f + 2;						target ++;

			vidx += TO_NEXT_LINE;
			f += 2;

			// interior
			for( int i = 1, e = vertexTileDim - 1; i < e; i += 1, vidx += TO_NEXT_LINE, f += 2 )
			{
				if( start ) *target = vidx - 1;					target ++;
				if( start ) *target = vidx + TO_NEXT_LINE - 1;	target ++;
				if( start ) *target = vidx;						target ++;

				if( start ) *target = vidx;						target ++;
				if( start ) *target = vidx + TO_NEXT_LINE - 1;	target ++;
				if( start ) *target = eastVertStart + i;		target ++;

				if( start ) *target = eastVertStart + i;		target ++;
				if( start ) *target = vidx + TO_NEXT_LINE - 1;	target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;		target ++;

				//------------------------------------------------------------------------

				if( start ) *target = vidx;							target ++;
				if( start ) *target = eastVertStart + i;			target ++;
				if( start ) *target = f;							target ++;

				if( start ) *target = f;							target ++;
				if( start ) *target = eastVertStart + i;			target ++;
				if( start ) *target = f + 1;						target ++;

				if( start ) *target = eastVertStart + i;			target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;			target ++;
				if( start ) *target = f + 1;						target ++;

				if( start ) *target = f + 1;						target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;			target ++;
				if( start ) *target = f + 2;						target ++;
			}

			// exterior 1

			if( !SouthAndEast )
			{
				if( start ) *target = vidx;									target ++;
				if( start ) *target = vidx - 1 + TO_NEXT_LINE;				target ++;
				if( start ) *target = eastVertStart + vertexTileDim - 1;	target ++;

				if( start ) *target = eastVertStart + vertexTileDim - 1;	target ++;
				if( start ) *target = vidx - 1 + TO_NEXT_LINE;				target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;					target ++;
			}

			//------------------------------------------------------------------------
			if( start ) *target = vidx;									target ++;
			if( start ) *target = eastVertStart + vertexTileDim - 1;	target ++;
			if( start ) *target = f;									target ++;

			if( start ) *target = f;									target ++;
			if( start ) *target = eastVertStart + vertexTileDim - 1;	target ++;
			if( start ) *target = f + 1;								target ++;

			if( start ) *target = eastVertStart + vertexTileDim - 1;	target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;					target ++;
			if( start ) *target = f + 1;								target ++;

			if( start ) *target = f + 1;								target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;					target ++;
			if( start ) *target = f + 2;								target ++;
		}
		else
		{
			int vidx = CELL_VERT_SIZE - 1;
			int f = fringeStart_EAST;

			if( start ) *target = vidx;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE - 1;	target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;

			//------------------------------------------------------------------------
			if( start ) *target = f;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = f + 2;					target ++;

			if( start ) *target = f;						target ++;
			if( start ) *target = vidx;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			//------------------------------------------------------------------------

			vidx += TO_NEXT_LINE;
			f += 2;

			// interior
			for( int i = 1; i < vertexTileDim - 1; i ++, vidx += TO_NEXT_LINE, f += 2 )
			{
				if( start ) *target = vidx - 1;					target ++;
				if( start ) *target = vidx + TO_NEXT_LINE - 1;	target ++;
				if( start ) *target = vidx;						target ++;

				if( start ) *target = vidx + TO_NEXT_LINE - 1;	target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
				if( start ) *target = vidx;						target ++;

				//------------------------------------------------------------------------
				if( start ) *target = f;						target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
				if( start ) *target = f + 2;					target ++;

				if( start ) *target = f;						target ++;
				if( start ) *target = vidx;						target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
				//------------------------------------------------------------------------
			}

			if( start ) *target = vidx - 1;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE - 1;		target ++;
			if( start ) *target = vidx;							target ++;

			//------------------------------------------------------------------------
			if( start ) *target = f;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = f + 2;					target ++;

			if( start ) *target = f;						target ++;
			if( start ) *target = vidx;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			//------------------------------------------------------------------------
		}


		// construct south connection
		if( sideLodConnections & SOUTH_CONNECTION )
		{
			int vidx = vertexTileDim * CELL_VERT_SIZE;
			int f = fringeStart_SOUTH;

			// exterior 0
			{
				if( start ) *target = vidx ;					target ++;
				if( start ) *target = southVertStart;			target ++;
				if( start ) *target = vidx - TO_NEXT_LINE + 1;	target ++;

				if( start ) *target = southVertStart;			target ++;
				if( start ) *target = vidx + 1;					target ++;
				if( start ) *target = vidx - TO_NEXT_LINE + 1;	target ++;
			}

			//------------------------------------------------------------------------
			if( start ) *target = vidx;					target ++;
			if( start ) *target = f;					target ++;
			if( start ) *target = southVertStart + 0;	target ++;

			if( start ) *target = f;					target ++;
			if( start ) *target = f + 1;				target ++;
			if( start ) *target = southVertStart + 0;	target ++;

			if( start ) *target = southVertStart + 0;	target ++;
			if( start ) *target = f + 1;				target ++;
			if( start ) *target = vidx + 1;				target ++;

			if( start ) *target = f + 1;				target ++;
			if( start ) *target = f + 2;				target ++;
			if( start ) *target = vidx + 1;				target ++;

			vidx += 1;
			f += 2;

			// interior
			for( int i = 1, e = vertexTileDim - 1; i < e; i ++, vidx ++, f += 2 )
			{
				if( start ) *target = vidx;						target ++;
				if( start ) *target = vidx - TO_NEXT_LINE + 1;	target ++;
				if( start ) *target = vidx - TO_NEXT_LINE;		target ++;

				if( start ) *target = vidx;						target ++;
				if( start ) *target = southVertStart + i;		target ++;
				if( start ) *target = vidx - TO_NEXT_LINE + 1;	target ++;

				if( start ) *target = southVertStart + i;		target ++;
				if( start ) *target = vidx + 1;					target ++;
				if( start ) *target = vidx - TO_NEXT_LINE + 1;	target ++;

				//------------------------------------------------------------------------	
				if( start ) *target = vidx;						target ++;
				if( start ) *target = f;						target ++;
				if( start ) *target = southVertStart + i;		target ++;

				if( start ) *target = f;						target ++;
				if( start ) *target = f + 1;					target ++;
				if( start ) *target = southVertStart + i;		target ++;

				if( start ) *target = southVertStart + i;		target ++;
				if( start ) *target = f + 1;					target ++;
				if( start ) *target = vidx + 1;					target ++;

				if( start ) *target = f + 1;					target ++;
				if( start ) *target = f + 2;					target ++;
				if( start ) *target = vidx + 1;					target ++;
				//------------------------------------------------------------------------
			}

			// exterior 1

			if( !SouthAndEast ) 
			{
				if( start ) *target = southVertStart + CELL_VERT_SIZE - 2;	target ++;
				if( start ) *target = vidx - TO_NEXT_LINE + 1;				target ++;
				if( start ) *target = vidx;									target ++;

				if( start ) *target = vidx + 1;								target ++;
				if( start ) *target = vidx - TO_NEXT_LINE + 1;				target ++;
				if( start ) *target = southVertStart + CELL_VERT_SIZE - 2;	target ++;
			}

			//------------------------------------------------------------------------
			if( start ) *target = vidx;									target ++;
			if( start ) *target = f;									target ++;
			if( start ) *target = southVertStart + vertexTileDim - 1;	target ++;

			if( start ) *target = f;									target ++;
			if( start ) *target = f + 1;								target ++;
			if( start ) *target = southVertStart + vertexTileDim - 1;	target ++;

			if( start ) *target = southVertStart + vertexTileDim - 1;	target ++;
			if( start ) *target = f + 1;								target ++;
			if( start ) *target = vidx + 1;								target ++;

			if( start ) *target = f + 1;								target ++;
			if( start ) *target = f + 2;								target ++;
			if( start ) *target = vidx + 1;								target ++;
		}
		else
		{
			int vidx = vertexTileDim * CELL_VERT_SIZE;
			int f = fringeStart_SOUTH;

			// exterior 0
			if( start ) *target = vidx;						target ++;
			if( start ) *target = vidx + 1;					target ++;
			if( start ) *target = vidx - TO_NEXT_LINE + 1;	target ++;

			//------------------------------------------------------------------------	
			if( start ) *target = vidx;						target ++;
			if( start ) *target = f;						target ++;
			if( start ) *target = vidx + 1;					target ++;

			if( start ) *target = f;						target ++;
			if( start ) *target = f + 2;					target ++;
			if( start ) *target = vidx + 1;					target ++;
			//------------------------------------------------------------------------

			vidx += 1;
			f += 2;

			// interior
			for( int i = 1; i < vertexTileDim - 1; i ++, vidx ++, f += 2 )
			{
				if( start ) *target = vidx;						target ++;
				if( start ) *target = vidx + 1;					target ++;
				if( start ) *target = vidx - TO_NEXT_LINE + 1;	target ++;

				if( start ) *target = vidx;						target ++;
				if( start ) *target = vidx - TO_NEXT_LINE + 1;	target ++;
				if( start ) *target = vidx - TO_NEXT_LINE;		target ++;

				//------------------------------------------------------------------------	
				if( start ) *target = vidx;						target ++;
				if( start ) *target = f;						target ++;
				if( start ) *target = vidx + 1;					target ++;

				if( start ) *target = f;						target ++;
				if( start ) *target = f + 2;					target ++;
				if( start ) *target = vidx + 1;					target ++;
				//------------------------------------------------------------------------
			}

			// exterior 1

			if( !NotSouthAndNotEast )
			{
				if( start ) *target = vidx - TO_NEXT_LINE;		target ++;
				if( start ) *target = vidx;						target ++;
				if( start ) *target = vidx - TO_NEXT_LINE + 1;	target ++;
			}

			//------------------------------------------------------------------------	
			if( start ) *target = vidx;						target ++;
			if( start ) *target = f;						target ++;
			if( start ) *target = vidx + 1;					target ++;

			if( start ) *target = f;						target ++;
			if( start ) *target = f + 2;					target ++;
			if( start ) *target = vidx + 1;					target ++;
			//------------------------------------------------------------------------
		}

		// construct west connection
		if( sideLodConnections & WEST_CONNECTION )
		{
			int vidx = 0;
			int f = fringeStart_WEST;

			// exterior 0

			if( !NorthAndWest )
			{
				if( start ) *target = vidx;						target ++;
				if( start ) *target = westVertStart;			target ++;
				if( start ) *target = vidx + 1;					target ++;

				if( start ) *target = westVertStart;			target ++;
				if( start ) *target = vidx + CELL_VERT_SIZE;	target ++;
				if( start ) *target = vidx + 1;					target ++;
			}

			if( start ) *target = vidx;							target ++;
			if( start ) *target = f;							target ++;
			if( start ) *target = westVertStart + 0;			target ++;

			if( start ) *target = f;							target ++;
			if( start ) *target = f + 1;						target ++;
			if( start ) *target = westVertStart + 0;			target ++;

			if( start ) *target = westVertStart + 0;			target ++;
			if( start ) *target = f + 1;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;			target ++;

			if( start ) *target = f + 1;						target ++;
			if( start ) *target = f + 2;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;			target ++;

			vidx += TO_NEXT_LINE;
			f += 2;

			// interior
			for( int i = 1, e = vertexTileDim - 1; i < e; i ++, vidx += TO_NEXT_LINE, f += 2 )
			{
				if( start ) *target = vidx;						target ++;
				if( start ) *target = westVertStart + i;		target ++;
				if( start ) *target = vidx + 1;					target ++;

				if( start ) *target = westVertStart + i;		target ++;
				if( start ) *target = vidx + CELL_VERT_SIZE;	target ++;
				if( start ) *target = vidx + 1;					target ++;

				if( start ) *target = vidx + 1;					target ++;
				if( start ) *target = vidx + CELL_VERT_SIZE;	target ++;
				if( start ) *target = vidx + TO_NEXT_LINE + 1;	target ++;

				//------------------------------------------------------------------------
				if( start ) *target = vidx;							target ++;
				if( start ) *target = f;							target ++;
				if( start ) *target = westVertStart + i;			target ++;

				if( start ) *target = f;							target ++;
				if( start ) *target = f + 1;						target ++;
				if( start ) *target = westVertStart + i;			target ++;

				if( start ) *target = westVertStart + i;			target ++;
				if( start ) *target = f + 1;						target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;			target ++;

				if( start ) *target = f + 1;						target ++;
				if( start ) *target = f + 2;						target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;			target ++;
				//------------------------------------------------------------------------
			}

			// exterior 1
			{
				if( start ) *target = vidx;									target ++;
				if( start ) *target = westVertStart + CELL_VERT_SIZE - 2;	target ++;
				if( start ) *target = vidx + 1;								target ++;

				if( start ) *target = westVertStart + CELL_VERT_SIZE - 2;	target ++;
				if( start ) *target = vidx + CELL_VERT_SIZE;				target ++;
				if( start ) *target = vidx + 1;								target ++;

				//------------------------------------------------------------------------
				if( start ) *target = vidx;									target ++;
				if( start ) *target = f;									target ++;
				if( start ) *target = westVertStart + vertexTileDim - 1;	target ++;

				if( start ) *target = f;									target ++;
				if( start ) *target = f + 1;								target ++;
				if( start ) *target = westVertStart + vertexTileDim - 1;	target ++;

				if( start ) *target = westVertStart + vertexTileDim - 1;	target ++;
				if( start ) *target = f + 1;								target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;					target ++;

				if( start ) *target = f + 1;								target ++;
				if( start ) *target = f + 2;								target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;					target ++;
			}

		}
		else
		{
			int vidx = 0;
			int f = fringeStart_WEST;

			if( start ) *target = vidx + 1;					target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	target ++;

			//------------------------------------------------------------------------
			if( start ) *target = f;						target ++;
			if( start ) *target = f + 2;					target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;

			if( start ) *target = f;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx;						target ++;
			//------------------------------------------------------------------------

			vidx += TO_NEXT_LINE;
			f += 2;

			// interior
			for( int i = 1; i < vertexTileDim - 1; i ++, vidx += TO_NEXT_LINE, f += 2 )
			{
				if( start ) *target = vidx + 1;					target ++;
				if( start ) *target = vidx;						target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;		target ++;

				if( start ) *target = vidx + TO_NEXT_LINE + 1;	target ++;
				if( start ) *target = vidx + 1;					target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;		target ++;

				//------------------------------------------------------------------------
				if( start ) *target = f;						target ++;
				if( start ) *target = f + 2;					target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;		target ++;

				if( start ) *target = f;						target ++;
				if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
				if( start ) *target = vidx;						target ++;
				//------------------------------------------------------------------------
			}

			if( start ) *target = vidx + 1;				target ++;
			if( start ) *target = vidx;					target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;	target ++;

			//------------------------------------------------------------------------
			if( start ) *target = f;						target ++;
			if( start ) *target = f + 2;					target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;

			if( start ) *target = f;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx;						target ++;
			//------------------------------------------------------------------------
		}

		if( NorthAndWest )
		{
			int vidx = 0;

			if( start ) *target = northVertStart;			target ++;
			if( start ) *target = vidx;						target ++;
			if( start ) *target = westVertStart;			target ++;

			if( start ) *target = vidx + 1;					target ++;
			if( start ) *target = northVertStart;			target ++;
			if( start ) *target = vidx + CELL_VERT_SIZE;	target ++;

			if( start ) *target = northVertStart;			target ++;
			if( start ) *target = westVertStart;			target ++;
			if( start ) *target = vidx + CELL_VERT_SIZE;	target ++;

			if( start ) *target = vidx + 1;					target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;	target ++;
		}

		if( SouthAndEast )
		{
			int vidx = ( vertexTileDim - 1 + 1 ) * CELL_VERT_SIZE - 1 - 1;

			if( start ) *target = vidx + 1;								target ++;
			if( start ) *target = vidx;									target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;					target ++;

			if( start ) *target = vidx + 1;								target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;					target ++;
			if( start ) *target = southVertStart + CELL_VERT_SIZE - 2;	target ++;

			if( start ) *target = eastVertStart + CELL_VERT_SIZE - 2;	target ++;
			if( start ) *target = southVertStart + CELL_VERT_SIZE - 2;	target ++;
			if( start ) *target = vidx + TO_NEXT_LINE + 1;				target ++;

			if( start ) *target = eastVertStart + CELL_VERT_SIZE - 2;	target ++;
			if( start ) *target = vidx + 1;								target ++;
			if( start ) *target = southVertStart + CELL_VERT_SIZE - 2;	target ++;
		}

		if( NotNorthAndNotWest )
		{
			int vidx = 0;

			if( start ) *target = vidx;						target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
			if( start ) *target = vidx + 1;					target ++;
		}

		if( NotSouthAndNotEast )
		{
			int vidx = ( vertexTileDim - 1 + 1 ) * CELL_VERT_SIZE - 1 - 1;

			if( start ) *target = vidx + 1 + TO_NEXT_LINE;	target ++;
			if( start ) *target = vidx + 1;					target ++;
			if( start ) *target = vidx + TO_NEXT_LINE;		target ++;
		}
	}

	return target - start;
}

//------------------------------------------------------------------------

static int ConstructTileVertices( R3D_TERRAIN_VERTEX_3* target, int vertexTileDim, int type )
{
	R3D_TERRAIN_VERTEX_3* start = target;

	const int WIDTH = 65535;
	const int HEIGHT = 65535;
	const int OFFSET = 32768;

	if( vertexTileDim == 1 )
	{
		switch( type )
		{
		case TILEGEOMTYPE_EAST_MEGACONNECTED:
			{
				R3D_TERRAIN_VERTEX_3 v;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				//------------------------------------------------------------------------

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;
			}
			break;
		case TILEGEOMTYPE_SOUTH_MEGACONNECTED:
			{
				R3D_TERRAIN_VERTEX_3 v;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				//------------------------------------------------------------------------

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;
			}
			break;
		case TILEGEOMTYPE_EAST_CONNECTED_MEGACONNECTED:
			{
				R3D_TERRAIN_VERTEX_3 v;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT / 2 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				//------------------------------------------------------------------------

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT / 2 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;
			}
			break;
		case TILEGEOMTYPE_SOUTH_CONNECTED_MEGACONNECTED:
			{
				R3D_TERRAIN_VERTEX_3 v;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH / 2 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				//------------------------------------------------------------------------

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH / 2 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;
			}
			break;
		case TILEGEOMTYPE_SOUTH_EAST_MEGACONNECTED:
			{
				R3D_TERRAIN_VERTEX_3 v;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				//------------------------------------------------------------------------

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;
			}
			break;
		case TILEGEOMTYPE_SOUTH_EAST_CONNECTED_MEGACONNECTED:
			{
				R3D_TERRAIN_VERTEX_3 v;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT / 2 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				//------------------------------------------------------------------------

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT / 2 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;
			}
			break;
		case TILEGEOMTYPE_SOUTH_CONNECTED_EAST_MEGACONNECTED:
			{
				R3D_TERRAIN_VERTEX_3 v;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH / 2 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				//------------------------------------------------------------------------

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH / 2 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;
			}
			break;
		case TILEGEOMTYPE_SOUTH_CONNECTED_EAST_CONNECTED_MEGACONNECTED:
			{
				R3D_TERRAIN_VERTEX_3 v;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT / 2 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH / 2 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;

				//------------------------------------------------------------------------

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT / 2 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH / 2 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;
			}
			break;
		}
	}
	else
	{
		switch( type )
		{
		case TILEGEOMTYPE_INSIDE:
			for( int z = 1; z < vertexTileDim; z ++ )
			{
				for( int x = 1; x < vertexTileDim; x ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3 ( WIDTH * x / vertexTileDim - OFFSET, HEIGHT * z / vertexTileDim  - OFFSET, 0, 0 );

					if( start ) { *target = v; } target ++;
				}
			}
			break;
		case TILEGEOMTYPE_WEST:
			for( int z = 0; z < vertexTileDim; z ++ )
			{
				R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3 ( WIDTH * 0 / vertexTileDim - OFFSET, HEIGHT * z / vertexTileDim - OFFSET, FRINGE_ZVAL, 0 );

				if( start ) { *target = v; } target ++;
			}

			for( int z = 0; z < vertexTileDim; z ++ )
			{
				if( start ) 
				{
					*target = *(target - vertexTileDim); 
					target->pos.z = 0;
				}
				target ++;
			}

			for( int z = 1; z < vertexTileDim; z ++ )
			{
				R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 / vertexTileDim - OFFSET, HEIGHT * z / vertexTileDim - OFFSET, 0, 0 );

				if( start ) { *target = v; } target ++;
			}
			break;
		case TILEGEOMTYPE_EAST:
			for( int z = 0; z < vertexTileDim; z ++ )
			{
				R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * ( vertexTileDim - 0 ) / vertexTileDim - OFFSET, HEIGHT * z / vertexTileDim - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;
			}

			for( int z = 0; z < vertexTileDim; z ++ )
			{
				if( start )
				{
					*target = *( target - vertexTileDim );
					target->pos.z = 0;
				}
				target ++;
			}

			for( int z = 0; z < vertexTileDim; z ++ )
			{
				R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, HEIGHT * z / vertexTileDim - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;
			}

			{
				R3D_TERRAIN_VERTEX_3 lastV = R3D_TERRAIN_VERTEX_3( WIDTH * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, HEIGHT * 0 - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = lastV; } target ++;
			}
			break;
		case TILEGEOMTYPE_NORTH:
			for( int x = 0; x < vertexTileDim; x ++ )
			{
				R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * x / vertexTileDim - OFFSET, HEIGHT * 0 / vertexTileDim - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;
			}

			for( int x = 0; x < vertexTileDim; x ++ )
			{
				if( start )
				{ 
					*target = *( target - vertexTileDim );
					target->pos.z = 0;
				}
				target ++;
			}

			for( int x = 1; x < vertexTileDim; x ++ )
			{
				R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * x / vertexTileDim - OFFSET, HEIGHT * 1 / vertexTileDim - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;
			}
			break;
		case TILEGEOMTYPE_SOUTH:
			for( int x = 0; x < vertexTileDim; x ++ )
			{
				R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * x / vertexTileDim - OFFSET, HEIGHT * ( vertexTileDim - 0 ) / vertexTileDim - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = v; } target ++;
			}

			for( int x = 0; x < vertexTileDim; x ++ )
			{
				if( start ) 
				{ 
					*target = *(target - vertexTileDim); 
					target->pos.z = 0;
				}
				target ++;
			}

			for( int x = 0; x < vertexTileDim; x ++ )
			{
				R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * x / vertexTileDim - OFFSET, HEIGHT * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;
			}

			{
				R3D_TERRAIN_VERTEX_3 lastV = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = lastV; } target ++;
			}
			break;

		case TILEGEOMTYPE_SOUTH_PARTOFMEGA:
			for( int x = 0; x < vertexTileDim; x ++ )
			{
				R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * x / vertexTileDim - OFFSET, HEIGHT * ( vertexTileDim - 0 ) / vertexTileDim - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;
			}

			for( int x = 0; x < vertexTileDim; x ++ )
			{
				if( start ) { *target = *(target - vertexTileDim); } target ++;
			}

			for( int x = 0; x < vertexTileDim; x ++ )
			{
				R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * x / vertexTileDim - OFFSET, HEIGHT * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, 0, 0 );
				if( start ) { *target = v; } target ++;
			}

			{
				R3D_TERRAIN_VERTEX_3 lastV = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, FRINGE_ZVAL, 0 );
				if( start ) { *target = lastV; } target ++;
			}
			break;
		case TILEGEOMTYPE_WEST_CONNECTED:
			{
				int outerCount = vertexTileDim * 2 - 1;

				for( int z = 0; z < outerCount; z ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 / vertexTileDim - OFFSET, HEIGHT * z / vertexTileDim / 2 - OFFSET, FRINGE_ZVAL, 0 );
					if( start ) { *target = v; } target ++;
				}

				for( int z = 0; z < outerCount; z ++ )
				{
					if( start )
					{
						*target = *( target - outerCount );
						target->pos.z = 0;
					}
					target ++;
				}

				for( int z = 1; z < vertexTileDim; z ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 / vertexTileDim - OFFSET, HEIGHT * z / vertexTileDim - OFFSET, 0, 0 );

					if( start ) { *target = v; } target ++;
				}
			}
			break;
		case TILEGEOMTYPE_EAST_CONNECTED:
			{
				int outerCount = vertexTileDim * 2 - 1;

				for( int z = 0; z < outerCount; z ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * ( vertexTileDim - 0 ) / vertexTileDim - OFFSET, HEIGHT * z / vertexTileDim / 2 - OFFSET, FRINGE_ZVAL, 0 );

					if( start ) { *target = v; } target ++;
				}

				for( int z = 0; z < outerCount; z ++ )
				{
					if( start )
					{
						*target = *( target - outerCount ); 
						target->pos.z = 0;
					}

					target ++;
				}

				for( int z = 0; z < vertexTileDim; z ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, HEIGHT * z / vertexTileDim - OFFSET, 0, 0 );

					if( start ) { *target = v; } target ++;
				}

				{
					R3D_TERRAIN_VERTEX_3 lastV = R3D_TERRAIN_VERTEX_3( WIDTH * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, HEIGHT * 0 - OFFSET, FRINGE_ZVAL, 0 );
					if( start ) { *target = lastV; } target ++;
				}
			}
			break;
		case TILEGEOMTYPE_NORTH_CONNECTED:
			{
				int outerCount = vertexTileDim * 2 - 1;

				for( int x = 0; x < outerCount; x ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * x / vertexTileDim / 2 - OFFSET, HEIGHT * 0 / vertexTileDim - OFFSET, FRINGE_ZVAL, 0 );

					if( start ) { *target = v; } target ++;
				}

				for( int x = 0; x < outerCount; x ++ )
				{
					if( start )
					{
						*target = *( target - outerCount );
						target->pos.z = 0;
					} 
					target ++;
				}

				for( int x = 1; x < vertexTileDim; x ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * x / vertexTileDim - OFFSET, HEIGHT * 1 / vertexTileDim - OFFSET, 0, 0 );

					if( start ) { *target = v; } target ++;
				}
			}
			break;
		case TILEGEOMTYPE_SOUTH_CONNECTED:
			{
				int outerCount = vertexTileDim * 2 - 1;

				for( int x = 0; x < outerCount; x ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * x / vertexTileDim / 2 - OFFSET, HEIGHT * ( vertexTileDim - 0 ) / vertexTileDim - OFFSET, FRINGE_ZVAL, 0 );
					if( start ) { *target = v; } target ++;
				}

				for( int x = 0; x < outerCount; x ++ )
				{
					if( start )
					{
						*target = *(target - outerCount);
						target->pos.z = 0;
					}
					target ++;
				}

				for( int x = 0; x < vertexTileDim; x ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * x / vertexTileDim - OFFSET, HEIGHT * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, 0, 0 );

					if( start ) { *target = v; } target ++;
				}

				{
					R3D_TERRAIN_VERTEX_3 lastV = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, FRINGE_ZVAL, 0 );
					if( start ) { *target = lastV; } target ++;
				}
			}
			break;

		case TILEGEOMTYPE_SOUTH_CONNECTED_PARTOFMEGA:
			{
				int outerCount = vertexTileDim * 2 - 1;

				for( int x = 0; x < outerCount; x ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * x / vertexTileDim / 2 - OFFSET, HEIGHT * ( vertexTileDim - 0 ) / vertexTileDim - OFFSET, 0, 0 );

					if( start ) { *target = v; } target ++;
				}

				for( int x = 0; x < outerCount; x ++ )
				{
					if( start ) { *target = *( target - outerCount); } target ++;
				}

				for( int x = 0; x < vertexTileDim; x ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * x / vertexTileDim - OFFSET, HEIGHT * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, 0, 0 );

					if( start ) { *target = v; } target ++;
				}

				{
					R3D_TERRAIN_VERTEX_3 lastV = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, FRINGE_ZVAL, 0 );
					if( start ) { *target = lastV; } target ++;
				}
			}
			break;

		case TILEGEOMTYPE_EAST_MEGACONNECTED:
			{
				int vertexCount = vertexTileDim + 1;

				for( int z = 0; z < vertexCount; z ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * ( vertexTileDim - 0 ) / vertexTileDim - OFFSET, HEIGHT * z / vertexTileDim - OFFSET, 0, 0 );

					if( start ) { *target = v; } target ++;
				}

				for( int z = 0; z < vertexCount; z ++ )
				{
					if( start ) { *target = *(target - vertexCount ); } target ++;
				}

				for( int z = 0; z < vertexCount; z ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, HEIGHT * z / vertexTileDim  - OFFSET, 0, 0 );

					if( start ) { *target = v; } target ++;
				}

				if( start )
				{
					R3D_TERRAIN_VERTEX_3 lastV = R3D_TERRAIN_VERTEX_3( WIDTH * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, 0 - OFFSET, FRINGE_ZVAL, 0 );
					*target = lastV;
				}
				target ++;

				if( start )
				{
					R3D_TERRAIN_VERTEX_3 lastV = R3D_TERRAIN_VERTEX_3( WIDTH * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, HEIGHT - OFFSET, FRINGE_ZVAL, 0 );
					*target = lastV;
				}
				target ++;
			}
			break;

		case TILEGEOMTYPE_SOUTH_MEGACONNECTED:
			{
				int vertexCount = vertexTileDim + 1;

				for( int x = 0; x < vertexCount; x ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * x / vertexTileDim - OFFSET, HEIGHT * ( vertexTileDim - 0 ) / vertexTileDim - OFFSET, 0, 0 );
					if( start ) { *target = v; } target ++;
				}

				for( int x = 0; x < vertexCount; x ++ )
				{
					if( start ) { *target = *( target - vertexCount ); } target ++;
				}

				for( int x = 0; x < vertexCount; x ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * x / vertexTileDim - OFFSET, HEIGHT * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, 0, 0 );
					if( start ) { *target = v; } target ++;
				}

				if( start )
				{
					R3D_TERRAIN_VERTEX_3 lastV = R3D_TERRAIN_VERTEX_3( 0 - OFFSET, HEIGHT * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, FRINGE_ZVAL, 0 );
					*target = lastV;
				}
				target ++;

				if( start )
				{
					R3D_TERRAIN_VERTEX_3 lastV = R3D_TERRAIN_VERTEX_3( WIDTH - OFFSET, HEIGHT * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, FRINGE_ZVAL, 0 );
					*target = lastV;
				}
				target ++;
			}
			break;

		case TILEGEOMTYPE_EAST_CONNECTED_MEGACONNECTED:
			{
				int outerCount = vertexTileDim * 2 + 1;

				for( int z = 0; z < outerCount; z ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * ( vertexTileDim - 0 ) / vertexTileDim - OFFSET, HEIGHT * z / vertexTileDim / 2 - OFFSET, 0, 0 );

					if( start ) { *target = v; } target ++;
				}

				for( int z = 0; z < outerCount; z ++ )
				{
					if( start ) { *target = *( target - outerCount ); } target ++;
				}

				for( int z = 0; z < vertexTileDim + 1; z ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, HEIGHT * z / vertexTileDim - OFFSET, 0, 0 );

					if( start ) { *target = v; } target ++;
				}

				if( start )
				{
					R3D_TERRAIN_VERTEX_3 lastV = R3D_TERRAIN_VERTEX_3( WIDTH * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, 0 - OFFSET, FRINGE_ZVAL, 0 );
					*target = lastV;
				}
				target ++;

				if( start )
				{
					R3D_TERRAIN_VERTEX_3 lastV = R3D_TERRAIN_VERTEX_3( WIDTH * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, HEIGHT - OFFSET, FRINGE_ZVAL, 0 );
					*target = lastV;
				}
				target ++;
			}
			break;

		case TILEGEOMTYPE_SOUTH_CONNECTED_MEGACONNECTED:
			{
				int outerCount = vertexTileDim * 2 + 1;

				for( int x = 0; x < outerCount; x ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * x / vertexTileDim / 2 - OFFSET, HEIGHT * ( vertexTileDim - 0 ) / vertexTileDim - OFFSET, 0, 0 );

					if( start ) { *target = v; } target ++;
				}

				for( int x = 0; x < outerCount; x ++ )
				{
					if( start ) { *target = *(target - outerCount); } target ++;
				}

				for( int x = 0; x < vertexTileDim + 1; x ++ )
				{
					R3D_TERRAIN_VERTEX_3 v = R3D_TERRAIN_VERTEX_3( WIDTH * x / vertexTileDim - OFFSET, HEIGHT * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, 0, 0 );

					if( start ) { *target = v; } target ++;
				}

				if( start )
				{
					R3D_TERRAIN_VERTEX_3 lastV = R3D_TERRAIN_VERTEX_3( 0 - OFFSET, HEIGHT * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, FRINGE_ZVAL, 0 );
					*target = lastV;
				}
				target ++;

				if( start )
				{
					R3D_TERRAIN_VERTEX_3 lastV = R3D_TERRAIN_VERTEX_3( WIDTH - OFFSET, HEIGHT * ( vertexTileDim - 1 ) / vertexTileDim - OFFSET, FRINGE_ZVAL, 0 );
					*target = lastV;
				}
				target ++;
			}
			break;
		}
	}

	return target - start;
}

static int ConstructTileIndices( UINT16* target, int vertexTileDim, int type )
{
	UINT16* start = target;

	const UINT16 VERTEX_TILE_DIM = UINT16( vertexTileDim );

	const UINT16 CELL_VERT_SIZE = VERTEX_TILE_DIM + 1;

	const UINT16 TO_NEXT_LINE	= CELL_VERT_SIZE - 2;

	if( vertexTileDim == 1 )
	{
		switch( type )
		{
		case TILEGEOMTYPE_EAST_MEGACONNECTED:
			{
				if( start ) { *target = 0; }	target ++;
				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 1; }	target ++;

				if( start ) { *target = 1; }	target ++;
				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 3; }	target ++;

				//------------------------------------------------------------------------

				if( start ) { *target = 1; }	target ++;
				if( start ) { *target = 3; }	target ++;
				if( start ) { *target = 4; }	target ++;

				if( start ) { *target = 4; }	target ++;
				if( start ) { *target = 3; }	target ++;
				if( start ) { *target = 5; }	target ++;

#if 0
				R3D_TERRAIN_VERTEX_3 v;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				//------------------------------------------------------------------------

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;
#endif
			}
			break;
		case TILEGEOMTYPE_SOUTH_MEGACONNECTED:
			{
				if( start ) { *target = 0; }	target ++;
				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 1; }	target ++;

				if( start ) { *target = 1; }	target ++;
				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 3; }	target ++;

				//------------------------------------------------------------------------

				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 4; }	target ++;
				if( start ) { *target = 3; }	target ++;

				if( start ) { *target = 4; }	target ++;
				if( start ) { *target = 5; }	target ++;
				if( start ) { *target = 3; }	target ++;
#if 0
				R3D_TERRAIN_VERTEX_3 v;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				//------------------------------------------------------------------------

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;
#endif
			}
			break;
		case TILEGEOMTYPE_EAST_CONNECTED_MEGACONNECTED:
			{
				if( start ) { *target = 0; }	target ++;
				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 1; }	target ++;

				if( start ) { *target = 4; }	target ++;
				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 0; }	target ++;

				if( start ) { *target = 4; }	target ++;
				if( start ) { *target = 0; }	target ++;
				if( start ) { *target = 3; }	target ++;

				//------------------------------------------------------------------------

				if( start ) { *target = 1; }	target ++;
				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 6; }	target ++;

				if( start ) { *target = 6; }	target ++;
				if( start ) { *target = 5; }	target ++;
				if( start ) { *target = 1; }	target ++;

				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 4; }	target ++;
				if( start ) { *target = 7; }	target ++;

				if( start ) { *target = 7; }	target ++;
				if( start ) { *target = 6; }	target ++;
				if( start ) { *target = 2; }	target ++;

#if 0
				R3D_TERRAIN_VERTEX_3 v;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT / 2 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				//------------------------------------------------------------------------

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT / 2 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;
#endif
			}
			break;
		case TILEGEOMTYPE_SOUTH_CONNECTED_MEGACONNECTED:
			{
				if( start ) { *target = 0; }	target ++;
				if( start ) { *target = 4; }	target ++;
				if( start ) { *target = 1; }	target ++;

				if( start ) { *target = 0; }	target ++;
				if( start ) { *target = 3; }	target ++;
				if( start ) { *target = 4; }	target ++;

				if( start ) { *target = 0; }	target ++;
				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 3; }	target ++;

				//------------------------------------------------------------------------

				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 5; }	target ++;
				if( start ) { *target = 3; }	target ++;

				if( start ) { *target = 3; }	target ++;
				if( start ) { *target = 5; }	target ++;
				if( start ) { *target = 6; }	target ++;

				if( start ) { *target = 3; }	target ++;
				if( start ) { *target = 6; }	target ++;
				if( start ) { *target = 4; }	target ++;

				if( start ) { *target = 6; }	target ++;
				if( start ) { *target = 7; }	target ++;
				if( start ) { *target = 4; }	target ++;

#if 0
				R3D_TERRAIN_VERTEX_3 v;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH / 2 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				//------------------------------------------------------------------------

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH / 2 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;
#endif
			}
			break;
		case TILEGEOMTYPE_SOUTH_EAST_MEGACONNECTED:
			{
				if( start ) { *target = 0; }	target ++;
				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 1; }	target ++;

				if( start ) { *target = 1; }	target ++;
				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 3; }	target ++;

				//------------------------------------------------------------------------

				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 4; }	target ++;
				if( start ) { *target = 3; }	target ++;

				if( start ) { *target = 4; }	target ++;
				if( start ) { *target = 5; }	target ++;
				if( start ) { *target = 3; }	target ++;

				if( start ) { *target = 5; }	target ++;
				if( start ) { *target = 1; }	target ++;
				if( start ) { *target = 3; }	target ++;

				if( start ) { *target = 5; }	target ++;
				if( start ) { *target = 6; }	target ++;
				if( start ) { *target = 1; }	target ++;

#if 0
				R3D_TERRAIN_VERTEX_3 v;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				//------------------------------------------------------------------------

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;
#endif
			}
			break;
		case TILEGEOMTYPE_SOUTH_EAST_CONNECTED_MEGACONNECTED:
			{
				if( start ) { *target = 0; }	target ++;
				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 1; }	target ++;

				if( start ) { *target = 4; }	target ++;
				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 0; }	target ++;

				if( start ) { *target = 4; }	target ++;
				if( start ) { *target = 0; }	target ++;
				if( start ) { *target = 3; }	target ++;

				//------------------------------------------------------------------------

				if( start ) { *target = 1; }	target ++;
				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 6; }	target ++;

				if( start ) { *target = 1; }	target ++;
				if( start ) { *target = 6; }	target ++;
				if( start ) { *target = 5; }	target ++;

				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 4; }	target ++;
				if( start ) { *target = 7; }	target ++;

				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 7; }	target ++;
				if( start ) { *target = 6; }	target ++;

				if( start ) { *target = 3; }	target ++;
				if( start ) { *target = 8; }	target ++;
				if( start ) { *target = 4; }	target ++;

				if( start ) { *target = 8; }	target ++;
				if( start ) { *target = 7; }	target ++;
				if( start ) { *target = 4; }	target ++;

#if 0
				R3D_TERRAIN_VERTEX_3 v;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT / 2 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				//------------------------------------------------------------------------

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT / 2 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;
#endif
			}
			break;
		case TILEGEOMTYPE_SOUTH_CONNECTED_EAST_MEGACONNECTED:
			{
				if( start ) { *target = 0; }	target ++;
				if( start ) { *target = 4; }	target ++;
				if( start ) { *target = 1; }	target ++;

				if( start ) { *target = 0; }	target ++;
				if( start ) { *target = 3; }	target ++;
				if( start ) { *target = 4; }	target ++;

				if( start ) { *target = 0; }	target ++;
				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 3; }	target ++;

				//------------------------------------------------------------------------

				if( start ) { *target = 1; }	target ++;
				if( start ) { *target = 4; }	target ++;
				if( start ) { *target = 8; }	target ++;

				if( start ) { *target = 1; }	target ++;
				if( start ) { *target = 8; }	target ++;
				if( start ) { *target = 5; }	target ++;

				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 6; }	target ++;
				if( start ) { *target = 3; }	target ++;

				if( start ) { *target = 6; }	target ++;
				if( start ) { *target = 7; }	target ++;
				if( start ) { *target = 3; }	target ++;

				if( start ) { *target = 3; }	target ++;
				if( start ) { *target = 7; }	target ++;
				if( start ) { *target = 4; }	target ++;

				if( start ) { *target = 7; }	target ++;
				if( start ) { *target = 8; }	target ++;
				if( start ) { *target = 4; }	target ++;
#if 0
				R3D_TERRAIN_VERTEX_3 v;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH / 2 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				//------------------------------------------------------------------------

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH / 2 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;
#endif
			}
			break;
		case TILEGEOMTYPE_SOUTH_CONNECTED_EAST_CONNECTED_MEGACONNECTED:
			{
				if( start ) { *target = 0; }	target ++;
				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 1; }	target ++;

				if( start ) { *target = 0; }	target ++;
				if( start ) { *target = 5; }	target ++;
				if( start ) { *target = 2; }	target ++;

				if( start ) { *target = 0; }	target ++;
				if( start ) { *target = 4; }	target ++;
				if( start ) { *target = 5; }	target ++;

				if( start ) { *target = 0; }	target ++;
				if( start ) { *target = 3; }	target ++;
				if( start ) { *target = 4; }	target ++;

				//------------------------------------------------------------------------

				if( start ) { *target = 1; }	target ++;
				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 7; }	target ++;

				if( start ) { *target = 1; }	target ++;
				if( start ) { *target = 7; }	target ++;
				if( start ) { *target = 6; }	target ++;

				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 5; }	target ++;
				if( start ) { *target = 10; }	target ++;

				if( start ) { *target = 2; }	target ++;
				if( start ) { *target = 10; }	target ++;
				if( start ) { *target = 7; }	target ++;

				if( start ) { *target = 3; }	target ++;
				if( start ) { *target = 8; }	target ++;
				if( start ) { *target = 4; }	target ++;

				if( start ) { *target = 4; }	target ++;
				if( start ) { *target = 8; }	target ++;
				if( start ) { *target = 9; }	target ++;

				if( start ) { *target = 4; }	target ++;
				if( start ) { *target = 9; }	target ++;
				if( start ) { *target = 5; }	target ++;

				if( start ) { *target = 5; }	target ++;
				if( start ) { *target = 9; }	target ++;
				if( start ) { *target = 10; }	target ++;


#if 0
				R3D_TERRAIN_VERTEX_3 v;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 0 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT / 2 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH / 2 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, 0, 1.0f );
				if( start ) { *target = v; } target ++;

				//------------------------------------------------------------------------

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 0 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT / 2 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 0 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH / 2 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;

				v = R3D_TERRAIN_VERTEX_3( WIDTH * 1 - OFFSET, HEIGHT * 1 - OFFSET, FRINGE_ZVAL, 1.0f );
				if( start ) { *target = v; } target ++;
#endif
			}
			break;
		}
	}
	else
	{
		switch( type )
		{
			// inside
		case TILEGEOMTYPE_INSIDE:
			if( vertexTileDim > 2 )
			{
				UINT16 vidx = 0;

				for( int i = 1, e = vertexTileDim - 1; i < e; i += 1 )
				{
					for( int j = 1, e = vertexTileDim - 1; j < e; j += 1, vidx += 1 )
					{
						if( start ) { *target = vidx; }						target ++;
						if( start ) { *target = vidx + TO_NEXT_LINE; }		target ++;
						if( start ) { *target = vidx + 1; }					target ++;

						if( start ) { *target = vidx + TO_NEXT_LINE; }		target ++;
						if( start ) { *target = vidx + TO_NEXT_LINE + 1; }	target ++;
						if( start ) { *target = vidx + 1; }					target ++;
					}

					vidx += 1;

				}
			}
			break;
		case TILEGEOMTYPE_WEST:
			if( vertexTileDim > 1 )
			{
				UINT16 vidx = 0;

				{
					// fring down
					for( int i = 0, e = vertexTileDim - 1; i < e; i ++, vidx ++ )
					{
						if( start ) { *target = vidx;						}	target ++;
						if( start ) { *target = vidx + 1;					}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM;		}	target ++;

						if( start ) { *target = vidx + 1;					}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM;		}	target ++;
					}
				}

				{
					vidx = VERTEX_TILE_DIM;

					// top fringe
					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = vidx + 1;					}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM;		}	target ++;

					vidx ++;

					// inside
					for( int i = 1, e = vertexTileDim - 1; i < e; i ++, vidx ++ )
					{
						if( start ) { *target = vidx;						}	target ++;
						if( start ) { *target = vidx + 1;					}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM - 1;	}	target ++;

						if( start ) { *target = vidx + 1;					}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM;		}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM - 1;	}	target ++;
					}
				}
			}
			break;

		case TILEGEOMTYPE_EAST:
			if( vertexTileDim > 1 )
			{
				UINT16 vidx = 0;
				{
					// inside
					for( int i = 0, e = vertexTileDim - 1; i < e; i ++, vidx ++ )
					{
						if( start ) { *target = vidx + VERTEX_TILE_DIM;		}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;
						if( start ) { *target = vidx;						}	target ++;

						if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;
						if( start ) { *target = vidx + 1;					}	target ++;
						if( start ) { *target = vidx;						}	target ++;
					}
				}

				{
					vidx = VERTEX_TILE_DIM;

					// top fringe
					if( start ) { *target = vidx + VERTEX_TILE_DIM;			}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;		}	target ++;
					if( start ) { *target = vidx;							}	target ++;

					if( start ) { *target = vidx;							}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;		}	target ++;
					if( start ) { *target = vidx + 1;						}	target ++;

					vidx ++;

					// inside
					for( int i = 1, e = vertexTileDim - 1; i < e; i ++, vidx ++ )
					{
						if( start ) { *target = vidx + VERTEX_TILE_DIM;		}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;
						if( start ) { *target = vidx;						}	target ++;

						if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;
						if( start ) { *target = vidx + 1;					}	target ++;
						if( start ) { *target = vidx;						}	target ++;
					}
				}

				{
					int END_VERTEX = VERTEX_TILE_DIM * 3;

					vidx = VERTEX_TILE_DIM;

					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = END_VERTEX;					}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM;		}	target ++;

					if( start ) { *target = 0;							}	target ++;
					if( start ) { *target = END_VERTEX;					}	target ++;
					if( start ) { *target = vidx;						}	target ++;
				}
			}
			break;

		case TILEGEOMTYPE_NORTH:
			if( vertexTileDim > 1 )
			{
				UINT16 vidx = 0;

				{
					// inside
					for( int i = 0, e = vertexTileDim - 1; i < e; i ++, vidx ++ )
					{
						if( start ) { *target = vidx;							}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM;			}	target ++;
						if( start ) { *target = vidx + 1;						}	target ++;

						if( start ) { *target = vidx + 1;						}	target ++;				
						if( start ) { *target = vidx + VERTEX_TILE_DIM;			}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;		}	target ++;
					}
				}

				{
					vidx = VERTEX_TILE_DIM;
					// top fringe
					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM;		}	target ++;
					if( start ) { *target = vidx + 1;					}	target ++;

					vidx ++;

					// inside
					for( int i = 1, e = vertexTileDim - 1; i < e; i ++, vidx ++ )
					{
						if( start ) { *target = vidx;						}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM - 1;	}	target ++;
						if( start ) { *target = vidx + 1;					}	target ++;

						if( start ) { *target = vidx + 1;					}	target ++;				
						if( start ) { *target = vidx + VERTEX_TILE_DIM - 1;	}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM;		}	target ++;
					}
				}
			}
			break;

		case TILEGEOMTYPE_SOUTH:
			if( vertexTileDim > 1 )
			{
				UINT16 vidx = 0;

				{
					// inside
					for( int i = 0, e = vertexTileDim - 1; i < e; i ++, vidx ++ )
					{
						if( start ) { *target = vidx + 1;					}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;
						if( start ) { *target = vidx;						}	target ++;

						if( start ) { *target = vidx;						}	target ++;								
						if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM;		}	target ++;
					}
				}

				{
					vidx = VERTEX_TILE_DIM;

					// top fringe
					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = vidx + 1;					}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;

					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM;		}	target ++;

					vidx ++;

					// inside
					for( int i = 1, e = vertexTileDim - 1; i < e; i ++, vidx ++ )
					{
						if( start ) { *target = vidx + 1;					}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;
						if( start ) { *target = vidx;						}	target ++;

						if( start ) { *target = vidx;						}	target ++;								
						if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM;		}	target ++;
					}
				}

				{
					int END_VERTEX = VERTEX_TILE_DIM * 3;

					vidx = VERTEX_TILE_DIM;

					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM;		}	target ++;
					if( start ) { *target = END_VERTEX;					}	target ++;

					if( start ) { *target = 0;							}	target ++;
					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = END_VERTEX;					}	target ++;
				}
			}
			break;

		case TILEGEOMTYPE_SOUTH_PARTOFMEGA:
			if( vertexTileDim > 1 )
			{
				UINT16 vidx = 0;

				for( int i = 0; i < 2; vidx = VERTEX_TILE_DIM, i ++ )
				{
					// top fringe
					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = vidx + 1;					}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;

					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM;		}	target ++;

					vidx ++;

					// inside
					for( int i = 1, e = vertexTileDim - 1; i < e; i ++, vidx ++ )
					{
						if( start ) { *target = vidx + 1;					}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;
						if( start ) { *target = vidx;						}	target ++;

						if( start ) { *target = vidx;						}	target ++;								
						if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM;		}	target ++;
					}
				}

				{
					int END_VERTEX = VERTEX_TILE_DIM * 3;

					vidx = VERTEX_TILE_DIM;

					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM;		}	target ++;
					if( start ) { *target = END_VERTEX;					}	target ++;

					if( start ) { *target = 0;							}	target ++;
					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = END_VERTEX;					}	target ++;
				}
			}
			break;

		case TILEGEOMTYPE_WEST_CONNECTED:
			if( vertexTileDim > 1 )
			{
				UINT16 vidx = 0;

				{
					int TO_NEXT_LINE = VERTEX_TILE_DIM * 2 - 1;

					// fring down
					for( int i = 0, e = vertexTileDim * 2 - 2; i < e; i ++, vidx ++ )
					{
						if( start ) { *target = vidx;						}	target ++;
						if( start ) { *target = vidx + 1;					}	target ++;
						if( start ) { *target = vidx + TO_NEXT_LINE;		}	target ++;

						if( start ) { *target = vidx + 1;					}	target ++;
						if( start ) { *target = vidx + TO_NEXT_LINE + 1;	}	target ++;
						if( start ) { *target = vidx + TO_NEXT_LINE;		}	target ++;
					}
				}


				{
					vidx = 0;

					int offs = VERTEX_TILE_DIM * 2 - 1;

					// top fringe
					if( start ) { *target = offs + vidx;							}	target ++;
					if( start ) { *target = offs + vidx + 1;						}	target ++;
					if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2 - 1;	}	target ++;

					if( start ) { *target = offs + vidx + 1;						}	target ++;
					if( start ) { *target = offs + vidx + 2;						}	target ++;
					if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2 - 1;	}	target ++;

					vidx ++;

					// inside
					for( int i = 2, e = vertexTileDim ; i < e; i ++, vidx ++ )
					{

						if( start ) { *target = offs + vidx * 2;						}	target ++;
						if( start ) { *target = offs + vidx * 2 + 1;					}	target ++;
						if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2 - 2;	}	target ++;

						if( start ) { *target = offs + vidx * 2 + 1;					}	target ++;
						if( start ) { *target = offs + vidx * 2 + 2;					}	target ++;
						if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2 - 2;	}	target ++;

						if( start ) { *target = offs + vidx * 2 + 2;					}	target ++;
						if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2 - 1;	}	target ++;
						if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2 - 2;	}	target ++;
					}
				}
			}
			break;

		case TILEGEOMTYPE_EAST_CONNECTED:
			if( vertexTileDim > 1 )
			{
				UINT16 vidx = 0;

				{
					const int TO_NEXT_LINE = VERTEX_TILE_DIM * 2 - 1;

					// inside
					for( int i = 0, e = vertexTileDim * 2 - 2; i < e; i ++, vidx ++ )
					{
						if( start ) { *target = vidx + 1;					}	target ++;
						if( start ) { *target = vidx;						}	target ++;
						if( start ) { *target = vidx + TO_NEXT_LINE + 1;	}	target ++;

						if( start ) { *target = vidx;						}	target ++;								
						if( start ) { *target = vidx + TO_NEXT_LINE;		}	target ++;
						if( start ) { *target = vidx + TO_NEXT_LINE + 1;	}	target ++;
					}
				}

				{
					vidx = 0;

					int offs = VERTEX_TILE_DIM * 2 - 1;

					// top fringe
					if( start ) { *target = offs + vidx;							}	target ++;
					if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2 - 1;	}	target ++;
					if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2;		}	target ++;

					if( start ) { *target = offs + vidx;							}	target ++;
					if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2;		}	target ++;
					if( start ) { *target = offs + vidx + 1;						}	target ++;

					if( start ) { *target = offs + vidx + 1;						}	target ++;			
					if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2;		}	target ++;
					if( start ) { *target = offs + vidx + 2;						}	target ++;

					vidx ++;

					// inside
					for( int i = 2, e = vertexTileDim ; i < e; i ++, vidx ++ )
					{

						if( start ) { *target = offs + vidx * 2;						}	target ++;				
						if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2;		}	target ++;
						if( start ) { *target = offs + vidx * 2 + 1;					}	target ++;

						if( start ) { *target = offs + vidx * 2 + 1;					}	target ++;				
						if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2;		}	target ++;
						if( start ) { *target = offs + vidx * 2 + 2;					}	target ++;

						if( start ) { *target = offs + vidx * 2 ;						}	target ++;				
						if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2 - 1;	}	target ++;
						if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2;		}	target ++;
					}
				}

				{
					int END_VERTEX = ( VERTEX_TILE_DIM * 2 - 1 ) * 2 + VERTEX_TILE_DIM;

					int TO_NEXT_LINE = VERTEX_TILE_DIM * 2 - 1;

					vidx = TO_NEXT_LINE;

					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = END_VERTEX;					}	target ++;
					if( start ) { *target = vidx + TO_NEXT_LINE;		}	target ++;

					if( start ) { *target = 0;							}	target ++;
					if( start ) { *target = END_VERTEX;					}	target ++;
					if( start ) { *target = vidx;						}	target ++;
				}
			}
			break;

		case TILEGEOMTYPE_NORTH_CONNECTED:
			if( vertexTileDim > 1 )
			{
				UINT16 vidx = 0;

				{
					const int TO_NEXT_LINE = VERTEX_TILE_DIM * 2 - 1;

					// inside
					for( int i = 0, e = vertexTileDim * 2 - 2; i < e; i ++, vidx ++ )
					{
						if( start ) { *target = vidx;						}	target ++;
						if( start ) { *target = vidx + TO_NEXT_LINE;		}	target ++;
						if( start ) { *target = vidx + 1;					}	target ++;

						if( start ) { *target = vidx + 1;					}	target ++;				
						if( start ) { *target = vidx + TO_NEXT_LINE;		}	target ++;
						if( start ) { *target = vidx + TO_NEXT_LINE + 1;	}	target ++;
					}
				}

				{
					vidx = 0;
					int offs = VERTEX_TILE_DIM * 2 - 1;

					// top fringe
					if( start ) { *target = offs + vidx;							}	target ++;
					if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2 - 1;	}	target ++;
					if( start ) { *target = offs + vidx + 1;						}	target ++;

					if( start ) { *target = offs + vidx + 1;						}	target ++;			
					if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2 - 1;	}	target ++;
					if( start ) { *target = offs + vidx + 2;						}	target ++;

					vidx ++;

					// inside
					for( int i = 2, e = vertexTileDim ; i < e; i ++, vidx ++ )
					{
						if( start ) { *target = offs + vidx * 2;							}	target ++;				
						if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2 - 1 - 1;	}	target ++;
						if( start ) { *target = offs + vidx * 2 + 1;						}	target ++;

						if( start ) { *target = offs + vidx * 2 + 1;						}	target ++;				
						if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2 - 1 - 1;	}	target ++;
						if( start ) { *target = offs + vidx * 2 + 2;						}	target ++;

						if( start ) { *target = offs + vidx * 2 + 2;						}	target ++;				
						if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2 - 2;		}	target ++;
						if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2 - 1;		}	target ++;
					}
				}
			}
			break;

		case TILEGEOMTYPE_SOUTH_CONNECTED:
			if( vertexTileDim > 1 )
			{
				UINT16 vidx = 0;

				{
					const int TO_NEXT_LINE = VERTEX_TILE_DIM * 2 - 1;

					// inside
					for( int i = 0, e = vertexTileDim * 2 - 2; i < e; i ++, vidx ++ )
					{
						if( start ) { *target = vidx + 1;					}	target ++;
						if( start ) { *target = vidx + TO_NEXT_LINE + 1;	}	target ++;
						if( start ) { *target = vidx;						}	target ++;

						if( start ) { *target = vidx;						}	target ++;								
						if( start ) { *target = vidx + TO_NEXT_LINE + 1;	}	target ++;
						if( start ) { *target = vidx + TO_NEXT_LINE;		}	target ++;
					}
				}

				// top fringe
				{
					int offs = VERTEX_TILE_DIM * 2 - 1;
					vidx = 0;

					if( start ) { *target = offs + vidx;							}	target ++;
					if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2;		}	target ++;
					if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2 - 1;	}	target ++;

					if( start ) { *target = offs + vidx;							}	target ++;
					if( start ) { *target = offs + vidx + 1;						}	target ++;
					if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2;		}	target ++;

					if( start ) { *target = offs + vidx + 1;						}	target ++;			
					if( start ) { *target = offs + vidx + 2;						}	target ++;
					if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2;		}	target ++;

					vidx ++;

					// inside
					for( int i = 2, e = vertexTileDim ; i < e; i ++, vidx ++ )
					{
						if( start ) { *target = offs + vidx * 2;						}	target ++;				
						if( start ) { *target = offs + vidx * 2 + 1;					}	target ++;
						if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2;		}	target ++;

						if( start ) { *target = offs + vidx * 2 + 1;					}	target ++;				
						if( start ) { *target = offs + vidx * 2 + 2;					}	target ++;
						if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2;		}	target ++;

						if( start ) { *target = offs + vidx * 2 ;						}	target ++;				
						if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2;		}	target ++;
						if( start ) { *target = offs + vidx + VERTEX_TILE_DIM * 2 - 1;	}	target ++;
					}
				}

				{
					int END_VERTEX = ( VERTEX_TILE_DIM * 2 - 1 ) * 2 + VERTEX_TILE_DIM;

					int TO_NEXT_LINE = VERTEX_TILE_DIM * 2 - 1;

					vidx = TO_NEXT_LINE;

					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = vidx + TO_NEXT_LINE;		}	target ++;
					if( start ) { *target = END_VERTEX;					}	target ++;

					if( start ) { *target = 0;							}	target ++;
					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = END_VERTEX;					}	target ++;
				}
			}
			break;

		case TILEGEOMTYPE_SOUTH_CONNECTED_PARTOFMEGA:
			if( vertexTileDim > 1 )
			{
				UINT16 vidx = 0;

				int VERTEX_TILE_DIM2 = VERTEX_TILE_DIM * 2;

				// inside
				for( int i = 0, e = vertexTileDim * 2 - 2; i < e; i ++, vidx ++ )
				{
					if( start ) { *target = vidx + 1;						}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM2;		}	target ++;
					if( start ) { *target = vidx;							}	target ++;

					if( start ) { *target = vidx;							}	target ++;								
					if( start ) { *target = vidx + VERTEX_TILE_DIM2;		}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM2 - 1;	}	target ++;
				}

				//------------------------------------------------------------------------

				vidx = 0;
				int off = VERTEX_TILE_DIM2 - 1;

				// top fringe
				if( start ) { *target = vidx + off;								}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM * 2 + off;		}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM * 2 - 1 + off;	}	target ++;

				if( start ) { *target = vidx + off;							}	target ++;
				if( start ) { *target = vidx + 1 + off;						}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM * 2 + off;	}	target ++;

				if( start ) { *target = vidx + 1 + off;						}	target ++;			
				if( start ) { *target = vidx + 2 + off;						}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM * 2 + off;	}	target ++;

				vidx ++;

				// inside
				for( int i = 2, e = vertexTileDim ; i < e; i ++, vidx ++ )
				{
					if( start ) { *target = vidx * 2 + off;						}	target ++;				
					if( start ) { *target = vidx * 2 + 1 + off;					}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM * 2 + off;	}	target ++;

					if( start ) { *target = vidx * 2 + 1 + off;					}	target ++;				
					if( start ) { *target = vidx * 2 + 2 + off;					}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM * 2 + off;	}	target ++;

					if( start ) { *target = vidx * 2 + off;							}	target ++;				
					if( start ) { *target = vidx + VERTEX_TILE_DIM * 2 + off;		}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM * 2 - 1 + off;	}	target ++;
				}

				//------------------------------------------------------------------------
				{
					int END_VERTEX = ( VERTEX_TILE_DIM * 2 - 1 ) * 2 + VERTEX_TILE_DIM;

					int TO_NEXT_LINE = VERTEX_TILE_DIM * 2 - 1;

					vidx = TO_NEXT_LINE;

					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = vidx + TO_NEXT_LINE;		}	target ++;
					if( start ) { *target = END_VERTEX;					}	target ++;

					if( start ) { *target = 0;							}	target ++;
					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = END_VERTEX;					}	target ++;
				}
			}
			break;

		case TILEGEOMTYPE_EAST_MEGACONNECTED:
			{
				UINT16 vidx = 0;

				int VERTEX_TILE_DIM_1 = VERTEX_TILE_DIM + 1;

				for( int i = 0; i < 2; i ++, vidx = VERTEX_TILE_DIM_1 )
				{
					// top fringe
					if( start ) { *target = vidx + VERTEX_TILE_DIM_1 + 1;	}	target ++;
					if( start ) { *target = vidx + 1;						}	target ++;
					if( start ) { *target = vidx;							}	target ++;


					if( start ) { *target = vidx + VERTEX_TILE_DIM_1;		}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM_1 + 1;	}	target ++;
					if( start ) { *target = vidx;							}	target ++;

					vidx ++;

					if( vertexTileDim > 1 )
					{
						// inside
						for( int i = 1, e = vertexTileDim - 1; i < e; i ++, vidx ++ )
						{
							if( start ) { *target = vidx + VERTEX_TILE_DIM_1;		}	target ++;
							if( start ) { *target = vidx + VERTEX_TILE_DIM_1 + 1;	}	target ++;
							if( start ) { *target = vidx;							}	target ++;

							if( start ) { *target = vidx + VERTEX_TILE_DIM_1 + 1;	}	target ++;
							if( start ) { *target = vidx + 1;						}	target ++;
							if( start ) { *target = vidx;							}	target ++;
						}

						// bottom fringe
						if( start ) { *target = vidx + VERTEX_TILE_DIM_1;		}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM_1 + 1;	}	target ++;
						if( start ) { *target = vidx;							}	target ++;

						if( start ) { *target = vidx;							}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM_1 + 1;	}	target ++;
						if( start ) { *target = vidx + 1;						}	target ++;
					}
				}

				const int END_VERTEX = 3 * ( VERTEX_TILE_DIM + 1 );

				//------------------------------------------------------------------------
				vidx = VERTEX_TILE_DIM + 1;

				if( start ) { *target = vidx;							}	target ++;			
				if( start ) { *target = END_VERTEX;						}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;		}	target ++;

				if( start ) { *target = END_VERTEX;						}	target ++;			
				if( start ) { *target = vidx;							}	target ++;
				if( start ) { *target = 0;								}	target ++;

				int TO_END_OF_LINE = VERTEX_TILE_DIM;

				if( start ) { *target = vidx + TO_END_OF_LINE;							}	target ++;			
				if( start ) { *target = vidx + VERTEX_TILE_DIM + 1 + TO_END_OF_LINE;	}	target ++;
				if( start ) { *target = END_VERTEX + 1;									}	target ++;

				if( start ) { *target = END_VERTEX + 1;			}	target ++;
				if( start ) { *target = TO_END_OF_LINE;			}	target ++;
				if( start ) { *target = vidx + TO_END_OF_LINE;	}	target ++;
			}
			break;

		case TILEGEOMTYPE_SOUTH_MEGACONNECTED:
			{
				UINT16 vidx = 0;

				for( int i = 0; i < 2; i ++, vidx = VERTEX_TILE_DIM + 1 )
				{
					// top fringe
					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = vidx + 1;					}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM + 2;	}	target ++;

					if( start ) { *target = vidx;						}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM + 2;	}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;

					vidx ++;

					if( vertexTileDim > 1 )
					{
						// inside
						for( int i = 1, e = vertexTileDim - 1; i < e; i ++, vidx ++ )
						{
							if( start ) { *target = vidx + 1;					}	target ++;
							if( start ) { *target = vidx + VERTEX_TILE_DIM + 2;	}	target ++;
							if( start ) { *target = vidx;						}	target ++;

							if( start ) { *target = vidx;						}	target ++;								
							if( start ) { *target = vidx + VERTEX_TILE_DIM + 2;	}	target ++;
							if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;
						}

						// bottom fringe
						if( start ) { *target = vidx;						}	target ++;			
						if( start ) { *target = vidx + VERTEX_TILE_DIM + 2;	}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;	}	target ++;

						if( start ) { *target = vidx;						}	target ++;			
						if( start ) { *target = vidx + 1;					}	target ++;
						if( start ) { *target = vidx + VERTEX_TILE_DIM + 2;	}	target ++;
					}
				}

				const int END_VERTEX = 3 * ( VERTEX_TILE_DIM + 1 );

				//------------------------------------------------------------------------
				vidx = VERTEX_TILE_DIM + 1;

				if( start ) { *target = vidx;							}	target ++;			
				if( start ) { *target = vidx + VERTEX_TILE_DIM + 1;		}	target ++;
				if( start ) { *target = END_VERTEX;						}	target ++;

				if( start ) { *target = END_VERTEX;						}	target ++;			
				if( start ) { *target = 0;								}	target ++;
				if( start ) { *target = vidx;							}	target ++;

				int TO_END_OF_LINE = VERTEX_TILE_DIM;

				if( start ) { *target = vidx + TO_END_OF_LINE;							}	target ++;			
				if( start ) { *target = END_VERTEX + 1;									}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM + 1 + TO_END_OF_LINE;	}	target ++;

				if( start ) { *target = END_VERTEX + 1;			}	target ++;
				if( start ) { *target = vidx + TO_END_OF_LINE;	}	target ++;
				if( start ) { *target = TO_END_OF_LINE;			}	target ++;
			}
			break;

		case TILEGEOMTYPE_EAST_CONNECTED_MEGACONNECTED:
			if( vertexTileDim > 1 )
			{
				UINT16 vidx = 0;

				int VERTEX_TILE_DIM2 = VERTEX_TILE_DIM * 2;

				//------------------------------------------------------------------------
				// top fringe

				if( start ) { *target = vidx + 1;						}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 1;	}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 2;	}	target ++;

				if( start ) { *target = vidx;							}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 1;	}	target ++;
				if( start ) { *target = vidx + 1;						}	target ++;

				vidx ++;

				// inside
				for( int i = 1, e = VERTEX_TILE_DIM2 - 1; i < e; i ++, vidx ++ )
				{
					if( start ) { *target = vidx;							}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 1;	}	target ++;
					if( start ) { *target = vidx + 1;						}	target ++;

					if( start ) { *target = vidx + 1;						}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 1;	}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 2;	}	target ++;
				}

				// bottom fringe
				if( start ) { *target = vidx;							}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 1;	}	target ++;
				if( start ) { *target = vidx + 1;						}	target ++;

				if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 1;	}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 2;	}	target ++;
				if( start ) { *target = vidx + 1;						}	target ++;

				//------------------------------------------------------------------------

				vidx = 0;
				int off = VERTEX_TILE_DIM2 + 1;

				// inside
				for( int i = 1, e = vertexTileDim + 1; i < e; i ++, vidx ++ )
				{

					if( start ) { *target = vidx * 2 + off;							}	target ++;				
					if( start ) { *target = vidx + VERTEX_TILE_DIM * 2 + 2 + off;	}	target ++;
					if( start ) { *target = vidx * 2 + 1 + off;						}	target ++;

					if( start ) { *target = vidx * 2 + 1 + off;						}	target ++;				
					if( start ) { *target = vidx + VERTEX_TILE_DIM * 2 + 2 + off;	}	target ++;
					if( start ) { *target = vidx * 2 + 2 + off;						}	target ++;

					if( start ) { *target = vidx * 2 + off;							}	target ++;				
					if( start ) { *target = vidx + VERTEX_TILE_DIM * 2 + 1 + off;	}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM * 2 + 2 + off;	}	target ++;
				}

				//------------------------------------------------------------------------

				int END_VERTEX = 2 * ( VERTEX_TILE_DIM2 + 1 ) + VERTEX_TILE_DIM + 1;

				vidx = VERTEX_TILE_DIM2 + 1;

				if( start ) { *target = vidx;							}	target ++;			
				if( start ) { *target = END_VERTEX;						}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 1;	}	target ++;

				if( start ) { *target = END_VERTEX;						}	target ++;			
				if( start ) { *target = vidx;							}	target ++;
				if( start ) { *target = 0;								}	target ++;

				int TO_END_OF_LINE = VERTEX_TILE_DIM;

				if( start ) { *target = vidx + TO_END_OF_LINE * 2 ;						}	target ++;			
				if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 1 + TO_END_OF_LINE;	}	target ++;
				if( start ) { *target = END_VERTEX + 1;									}	target ++;

				if( start ) { *target = END_VERTEX + 1;				}	target ++;
				if( start ) { *target = TO_END_OF_LINE * 2;			}	target ++;
				if( start ) { *target = vidx + TO_END_OF_LINE * 2;	}	target ++;
			}
			break;

		case TILEGEOMTYPE_SOUTH_CONNECTED_MEGACONNECTED:
			if( vertexTileDim > 1 )
			{
				UINT16 vidx = 0;

				//------------------------------------------------------------------------
				// top fringe

				int VERTEX_TILE_DIM2 = VERTEX_TILE_DIM * 2;

				if( start ) { *target = vidx;							}	target ++;
				if( start ) { *target = vidx + 1;						}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 2;	}	target ++;

				if( start ) { *target = vidx;							}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 2;	}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 1;	}	target ++;

				vidx ++;

				// inside
				for( int i = 1, e = VERTEX_TILE_DIM2 - 1; i < e; i ++, vidx ++ )
				{
					if( start ) { *target = vidx + 1;						}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 2;	}	target ++;
					if( start ) { *target = vidx;							}	target ++;

					if( start ) { *target = vidx;							}	target ++;								
					if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 2;	}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 1;	}	target ++;
				}

				// bottom fringe
				if( start ) { *target = vidx;							}	target ++;			
				if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 2;	}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 1;	}	target ++;

				if( start ) { *target = vidx;							}	target ++;			
				if( start ) { *target = vidx + 1;						}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 2;	}	target ++;

				vidx = 0;
				int off = VERTEX_TILE_DIM * 2 + 1;

				//------------------------------------------------------------------------
				// inside
				for( int i = 1, e = vertexTileDim + 1; i < e; i ++, vidx ++ )
				{
					if( start ) { *target = vidx * 2 + off;							}	target ++;				
					if( start ) { *target = vidx * 2 + 1 + off;						}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM * 2 + 2 + off;	}	target ++;

					if( start ) { *target = vidx * 2 + 1 + off;						}	target ++;				
					if( start ) { *target = vidx * 2 + 2 + off;						}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM * 2 + 2 + off;	}	target ++;

					if( start ) { *target = vidx * 2 + off;							}	target ++;				
					if( start ) { *target = vidx + VERTEX_TILE_DIM * 2 + 2 + off;	}	target ++;
					if( start ) { *target = vidx + VERTEX_TILE_DIM * 2 + 1 + off;	}	target ++;
				}

				//------------------------------------------------------------------------

				int END_VERTEX = 2 * ( VERTEX_TILE_DIM2 + 1 ) + VERTEX_TILE_DIM + 1;

				vidx = VERTEX_TILE_DIM2 + 1;

				if( start ) { *target = vidx;							}	target ++;			
				if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 1;	}	target ++;
				if( start ) { *target = END_VERTEX;						}	target ++;

				if( start ) { *target = END_VERTEX;						}	target ++;			
				if( start ) { *target = 0;								}	target ++;
				if( start ) { *target = vidx;							}	target ++;

				if( start ) { *target = vidx + VERTEX_TILE_DIM2;						}	target ++;			
				if( start ) { *target = END_VERTEX + 1;									}	target ++;
				if( start ) { *target = vidx + VERTEX_TILE_DIM2 + 1 + VERTEX_TILE_DIM;	}	target ++;

				if( start ) { *target = END_VERTEX + 1;									}	target ++;			
				if( start ) { *target = vidx + VERTEX_TILE_DIM2;						}	target ++;
				if( start ) { *target = VERTEX_TILE_DIM2;								}	target ++;
			}
			break;
		}
	}

	return target - start;
}


#endif

//------------------------------------------------------------------------

static void FillDecalRect( RECT* oRect, const r3dTerrain3::DecalRecord& drec )
{
	r3d_assert( drec.TypeIdx >= 0 );

	const r3dTerrainDesc& tdesc = Terrain3->GetDesc();

	const r3dTerrain3::QualitySettings& qsts = Terrain3->GetCurrentQualitySettings();

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


void SwitchTerraFetch()
{
	if( Terrain3 )
	{
		Terrain3->SwitchVFetchMode();
	}

	if( Terrain3 )
	{
		Terrain3->SwitchVFetchMode();
	}
}

void RefreshTerraTiles()
{
	//void RefreshTerra2Tiles();
	//RefreshTerra2Tiles();

	if( Terrain3 )
	{
#if 1
		Terrain3->RefreshAtlasTiles();
#else
		Terrain3->RefreshAllTiles();
#endif
	}
}

//------------------------------------------------------------------------

void ToggleTerrain3FixedCam()
{
#ifndef FINAL_BUILD
	if( Terrain3 )
		Terrain3->ToggleCameraFixed();
#endif
}

//------------------------------------------------------------------------

void FreezeTerrain3()
{
#ifndef FINAL_BUILD
	g_TerrainLoadingFrozen = !g_TerrainLoadingFrozen;
#endif
}

int IsTerrain3Frozen()
{
	return g_TerrainLoadingFrozen;
}

void AdvanceTerrain3Loading()
{
	g_AdvanceTerrain3Loading = 1;
}

int IsAutoSave( const char* levelPath )
{
	return stricmp( r3dGameLevel::GetHomeDir(), levelPath );
}

void DumpTerrain3MegaTiles()
{
	if( Terrain3 )
	{
		Terrain3->DEBUG_PrintMegaTileMap();
	}
}

void PrintTerraStats()
{
	if( Terrain3 )
	{
		Terrain3->DEBUG_PrintStats();
	}
}

void RefreshMaskTerraTiles()
{
	if( Terrain3 )
	{
		Terrain3->RefreshAllMaskAtlasTiles();
	}
}

void ResetTerraPhysics()
{
	if( Terrain3 )
	{
		Terrain3->DEBUG_ResetTerraPhysics();
	}
}


ErosionPattern::ErosionPattern()
: w(0)
, h(0)
, data(0)
{

}

ErosionPattern::ErosionPattern(const char *ddsFileName)
{
	r3dFile *f = r3d_open(ddsFileName);
	if (!f)
		return;

	int size = f->size - 128;
	if (size <= 0)
		return;

	fseek(f, 12, SEEK_SET);
	fread(&h, sizeof(h), 1, f);
	fread(&w, sizeof(w), 1, f);

	r3d_assert(w * h <= (unsigned int)size);
	if (w * h > (unsigned int)size)
		return;
	size = w * h;

	data = game_new byte[size];
	fseek(f, 128, SEEK_SET);
	fread(data, size, 1, f);
	fclose(f);
}

//------------------------------------------------------------------------

ErosionPattern::~ErosionPattern()
{
	delete [] data;
}

//------------------------------------------------------------------------

float ErosionPattern::Sample(float u, float v)
{
	u = r3dTL::Clamp(u, 0.0f, 1.0f);
	v = r3dTL::Clamp(v, 0.0f, 1.0f);

	int x = static_cast<int>(u * w);
	int y = static_cast<int>(v * h);

	int idx = y * w + x;
	return data[idx] / 255.0f;
}

//--------------------------------------------------------------------------------------------------------
template <class T> 
inline T LerpFast ( T from, T to, float weight )
{
	return from + weight * ( to - from );
}
//------------------------------------------------------------------------

float ErosionPattern::SampleBilinear(float u, float v)
{
	float uPixelSize = 1.0f / w;
	float vPixelSize = 1.0f / h;

	float s0 = Sample(u, v);
	float s1 = Sample(u + uPixelSize, v);
	float s2 = Sample(u, v + vPixelSize);
	float s3 = Sample(u + uPixelSize, v + vPixelSize);

	float dummy = 0;
	float subpixU = modf(u * w, &dummy);
	float subpixV = modf(v * h, &dummy);

	float h0 = LerpFast(s0, s1, subpixU);
	float h1 = LerpFast(s2, s3, subpixU);
	float h = LerpFast(h0, h1, subpixV);

	return h;
}

ErosionPattern *gErosionPattern = 0;
void InitErosionPattern(const char *filePath)
{
	delete gErosionPattern;
	gErosionPattern = game_new ErosionPattern(filePath);
}

void FreeErosionPattern()
{
	delete gErosionPattern;
	gErosionPattern = 0;
}