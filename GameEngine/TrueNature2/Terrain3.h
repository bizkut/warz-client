 #pragma once

#ifdef WO_SERVER
#define R3D_TERRAIN_V3_GRAPHICS 0
#else
#define R3D_TERRAIN_V3_GRAPHICS 1
#endif

#include "r3dTypeTuples.h"
#include "../SF/RenderBuffer.h"
#include "../SF/script.h"
#include "../TrueNature/ITerrain.h"
#include "r3dBitMaskArray.h"
#include "XPSObject.h"
#include "../../eternity/SF/script.h"
#include "../UndoHistory/UndoHistory.h"
#include "../DebugHelpers.h"



class r3dTerrain;
class r3dVertexBuffer;
class r3dIndexBuffer;

#define CQuadTerrain r3dTerrain

extern int __terra_UseHackedWorld;
extern D3DXMATRIX *__terra_HackedWorldMatrix;



//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class CHeightChanged : public IUndoItem
{
public:

	struct UndoHeight_t
	{
		int			nIndex;
		float		fPrevHeight;
		float		fCurrHeight;
	};

private:

	static const UndoAction_e	ms_eActionID = UA_TERRAIN_HEIGHT;


	r3dTL::TArray< UndoHeight_t >	m_pData;

	RECT				m_rc;

public:

	void				Release			()							{ PURE_DELETE( this ); };
	UndoAction_e		GetActionID		()							{ return ms_eActionID; };

	void				Undo			();
	void				Redo			();

	void				AddData			( const UndoHeight_t & data )	{ m_pData.PushBack( data ); };
	void				AddRectUpdate	( const RECT &rc )				
	{
		if ( m_rc.bottom > rc.bottom ) 
			m_rc.bottom = rc.bottom;
		if ( m_rc.left > rc.left ) 
			m_rc.left = rc.left;
		if ( m_rc.top < rc.top ) 
			m_rc.top = rc.top;
		if ( m_rc.right < rc.right ) 
			m_rc.right = rc.right;
	}

	CHeightChanged	();

	static IUndoItem * CreateUndoItem	()
	{
		return game_new CHeightChanged;
	};

	static void Register()
	{
		UndoAction_t action;
		action.nActionID = ms_eActionID;
		action.pCreateUndoItem = CreateUndoItem;
#ifndef FINAL_BUILD
		g_pUndoHistory->RegisterUndoAction( action );
#endif
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class CLayerMaskPaint : public IUndoItem
{

public:

	struct PaintData_t
	{
		PaintData_t();

		uint32_t	*	pData; // old data + new data
		RECT			rc;
		int				MatIdx;
	};

private:

	static const UndoAction_e		ms_eActionID = UA_TERRAIN_MASK_PAINT;
	r3dTL::TArray< PaintData_t >	m_pData;

	RECT				m_rc;

public:

	void				Release			()							
	{ 
		for ( int i = 0; i < (int)m_pData.Count(); i++ )
		{
			SAFE_DELETE_ARRAY( m_pData[ i ].pData );
		}
		PURE_DELETE( this ); 
	};
	UndoAction_e		GetActionID		()							{ return ms_eActionID; };

	void				Undo			();
	void				Redo			();

	void				AddData			( const PaintData_t & data )	{ m_pData.PushBack( data ); };
	void				AddRectUpdate	( const RECT &rc )				
	{
		if ( m_rc.bottom < rc.bottom ) 
			m_rc.bottom = rc.bottom;
		if ( m_rc.left > rc.left ) 
			m_rc.left = rc.left;
		if ( m_rc.top > rc.top ) 
			m_rc.top = rc.top;
		if ( m_rc.right < rc.right ) 
			m_rc.right = rc.right;
	}

	CLayerMaskPaint	();

	static IUndoItem * CreateUndoItem	()
	{
		return game_new CLayerMaskPaint;
	};

	static void Register()
	{
		UndoAction_t action;
		action.nActionID = ms_eActionID;
		action.pCreateUndoItem = CreateUndoItem;
#ifndef FINAL_BUILD
		g_pUndoHistory->RegisterUndoAction( action );
#endif
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class CLayerColorPaint : public IUndoItem
{
public:

	struct UndoColor_t
	{
		int			nIndex;
		uint32_t	dwPrevColor;
		uint32_t	dwCurrColor;
	};


private:

	static const UndoAction_e	ms_eActionID = UA_TERRAIN_COLOR_PAINT;
	r3dTL::TArray< UndoColor_t >	m_pData;

	RECT				m_rc;

public:

	void				Release			()							{ PURE_DELETE( this ); };
	UndoAction_e		GetActionID		()							{ return ms_eActionID; };

	void				Undo			();
	void				Redo			();

	void				AddData			( const UndoColor_t & data )	{ m_pData.PushBack( data ); };
	void				AddRectUpdate	( const RECT &rc )				
	{
		if ( m_rc.bottom > rc.bottom ) 
			m_rc.bottom = rc.bottom;
		if ( m_rc.left > rc.left ) 
			m_rc.left = rc.left;
		if ( m_rc.top < rc.top ) 
			m_rc.top = rc.top;
		if ( m_rc.right < rc.right ) 
			m_rc.right = rc.right;
	}

	CLayerColorPaint	();

	static IUndoItem * CreateUndoItem	()
	{
		return game_new CLayerColorPaint;
	};

	static void Register()
	{
		UndoAction_t action;
		action.nActionID = ms_eActionID;
		action.pCreateUndoItem = CreateUndoItem;
#ifndef FINAL_BUILD
		g_pUndoHistory->RegisterUndoAction( action );
#endif
	}
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#define TERRAIN_MAT_COUNT 8
#define TERRAIN_LAYERS_PER_MAT 4


#pragma pack(push)
#pragma pack(1)
class TerrainTile_c
{
public: 
	float m_HeightMin;
	float m_HeightMax;

	TerrainTile_c()
		: m_HeightMin( 0 )
		, m_HeightMax( 0 )
	{
	}
};
#pragma pack(pop)

typedef r3dTL::TArray< TerrainTile_c > TerrainTiles;

enum
{
	TERRA_CONNECTION_TYPE_COUNT = 16
};


struct TerrainLodDesc
{
	int nStep;
	int nIndexOffset[ TERRA_CONNECTION_TYPE_COUNT ];
	int nTriCount[ TERRA_CONNECTION_TYPE_COUNT ];

	int nConnectorFaceCount;

	float fDist;
};

#define LODDESC_COUNT			4

struct UpdateVertexDataSettings
{
	UpdateVertexDataSettings();

	uint32_t TileCountX;
	uint32_t TileCountZ;

	uint32_t UpdateTileXStart;
	uint32_t UpdateTileXEnd;

	uint32_t UpdateTileZStart;
	uint32_t UpdateTileZEnd;

	uint32_t CellGridDim;

	TerrainTiles*	Tiles;
	TerrainLodDesc (*dLods)[ LODDESC_COUNT ];

	int (*RemapOffset)( int , int , int );

	void*		LockedBuffer;
	uint32_t	LockOffset;
};

struct MaterialType;
struct HeightNormalVert;

struct TileCoord
{
	int X;
	int Z;

	bool operator == ( const TileCoord& c )
	{
		return c.X == X && c.Z == Z ;
	}
};

typedef r3dTL::TArray< TileCoord > TileCoords ;

extern	r3dITerrain	*Terrain;

#if 0
#ifndef FINAL_BUILD
#define FINAL_BUILD
#define ARTIFICIAL_FINAL_BUILD
#endif
#endif

#ifdef FINAL_BUILD
#define R3D_TERRAIN3_ALLOW_FULL_PHYSCHUNKS 0
#else // also enabled for WO_SERVER in that case
#define R3D_TERRAIN3_ALLOW_FULL_PHYSCHUNKS 1
#endif

struct r3dTerrain3Layer
{
	r3dString Name;

	float ScaleU;
	float ScaleV;

	float ShaderScaleU;
	float ShaderScaleV;

	float SpecularPow;

	float NormUVMult;

	r3dTexture* DiffuseTex;
	r3dTexture* NormalTex;

	float4 ChannelMask;
	int ChannelIdx;

	r3dString MaterialTypeName;

	r3dTerrain3Layer();
};

struct r3dTerrain3Stats
{
	int VolumeCount;

	int MaskVolumeCount;
	int NormalAndHeightVolumeCount;

	int TileCount;
	int TileTextureMemory;
	int MaskTextureMemory;
	int LayerTextureMemory;

	int MegaTileCount;

	r3dTerrain3Stats();
};

#pragma pack(push)
#pragma pack(1)

struct R3D_TERRAIN_VERTEX_3
{
	short4 pos;

	R3D_TERRAIN_VERTEX_3( short x, short y, short z, short w )
	{
		pos.x = x;
		pos.y = y;
		pos.z = z;
		pos.w = w;
	}

	R3D_TERRAIN_VERTEX_3() { }
};

#pragma pack(pop)

struct r3dTerrainOptiStats
{
	int NumDeletedLayerInfos;
	int NumDeletedMasks;
};

class r3dTerrain3 : public r3dITerrain
{
public:
	enum
	{
		NUM_MASK_QUALITY_LAYERS		= 16,
		NUM_QUALITY_LAYERS			= 16,
		LAYERS_PER_MASK				= 3,
		MAX_TEXTURE_LAYERS			= 255,
		MAX_VERTEX_TILE_DIM			= 8,
		MEGA_TEX_BORDER				= 4,
		TILE_BORDER					= 4,
		DEFAULT_ATLAS_TILE_DIM		= 256,
		HEIGHT_TILE_BORDER			= 0,
		NORMAL_TILE_BORDER			= 4,
		PHYS_CHUNK_COUNT_PER_SIDE	= 5,
		FILE_CHUNK_COUNT_PER_SIDE	= 8,
		GRID_ID_MIPSFILE			= 255
	};

	enum
	{
		QS_LOW,
		QS_MED,
		QS_HIGH,
		QS_COUNT
	};

	enum Side
	{
		SIDE_XNEG,
		SIDE_XPOS,
		SIDE_ZNEG,
		SIDE_ZPOS
	};

	enum RefreshMode
	{
		RM_NONE,
		RM_RELOAD,
		RM_UPDATE
	};

	enum
	{
		ATLAS_TEXTURE_DIM = 2048,
		MASK_ATLAS_TEXTURE_DIM = 2048,
		HEIGHT_NORMAL_ATLAS_TEXTURE_DIM = 2048
	};

	enum
	{
		LOADTILE_MASKS			= 0x01,
		LOADTILE_NORMAL			= 0x02,
		LOADTILE_HEIGHT			= 0x04,
		LOADTILE_UPDATEATLAS	= 0x08,
		LOADTILE_UPDATEROADS	= 0x10,
		LOADTILE_DISABLEFILTER	= 0x20,
		LOADTILE_ALL			= LOADTILE_MASKS | LOADTILE_NORMAL | LOADTILE_HEIGHT | LOADTILE_UPDATEROADS | LOADTILE_UPDATEATLAS
	};

	typedef r3dTL::TArray< unsigned char > Bytes;
	typedef r3dTL::TArray< int > Ints;

	typedef r3dTL::T2DArray< UINT8 > Bytes2D;
	typedef r3dTL::TArray< PxI16 > Shorts;
	typedef r3dTL::TArray< PxU32 > HFSamples;
	typedef r3dTL::TArray< UINT16 > UShorts;
	typedef r3dTL::TArray< float > Floats;
	typedef r3dTL::TArray< r3dPoint3D > Vectors;
	typedef std::set< INT64 > MaskIdSet;

#ifndef FINAL_BUILD
	typedef r3dTL::T2DArray< FILE* > FileGrid;
#else
	typedef r3dTL::T2DArray< r3dUnpackedFile* > FileGrid;
#endif

	struct TileCoord
	{
		int X;
		int Z;
		int L;
		int M;
	};
	
	typedef r3dTL::TArray< TileCoord > TileCoords;

	struct DecalRecord
	{
		int		TypeIdx;
		float	X;
		float	Z;
		float	Scale;
		float	ZRot;

		DecalRecord();
	};

	struct PhysicsChunk
	{
		PxRigidStatic*		PhysicsTerrain;
		PxHeightFieldDesc	PhysicsHeightFieldDesc;
		PxHeightField*		PhysicsHeightField;

		int					StartX;
		int					StartZ;

		int					ChunkX;
		int					ChunkZ;

		int					Dirty;

		PhysicsChunk();
	};

	typedef r3dTL::TArray< PhysicsChunk > PhysicsChunkArr;

	struct CreationParams
	{
		int CellCountX;
		int CellCountZ;

		int MaskSizeX;
		int MaskSizeZ;

		float CellSize;
		float StartHeight;

		float HeightRange;

		r3dString LevelDir;

		int MegaTerrain;

		CreationParams();
	};

	struct QualitySettings
	{
		enum
		{
			MIN_VERTEX_TILE_DIM = 1,
			NUM_DENSITY_STEPS = 5
		};

		r3dTL::TFixedArray< int, NUM_QUALITY_LAYERS >	TileCounts;
		r3dTL::TFixedArray< int, NUM_QUALITY_LAYERS >	TileVertexDensities;
		r3dTL::TFixedArray< int, NUM_DENSITY_STEPS >	TileVertexDensitySteps;
		r3dTL::TFixedArray< float, NUM_QUALITY_LAYERS > TileDistances;

		int												VertexDensity;

		int												AtlasTileDim;
		int												VertexTileDim;

		// starting from which vertex dim we store road information for terrain tiles
		int												RoadTileDiv;

		int												BakeRoads;

		int												VertexTilesInMegaTexTileCount;
		int												MaskAtlasTileDim;

		int												HeightNormalAtlasTileDim;

		int												PhysicsTileCellCount;
		int												DominantLayerDataDiv;

		// NOTE: update compare when you add new members

		int												CompareNoRoads( const QualitySettings& sts );
		int												Compare( const QualitySettings& sts );

		void											Sync( const r3dTerrainDesc* desc );

		QualitySettings();
	};

	struct Info
	{
		int NumActiveMegaTexLayers;

		int MegaTileCountX;
		int MegaTileCountZ;

		D3DFORMAT MaskFormat;

		int AtlasTileCountPerSide;
		int MaskAtlasTileCountPerSide;

		Info();
	};

	typedef r3dTL::TFixedArray< QualitySettings, QS_COUNT > QualitySettingsArr;

	struct Settings
	{
		float		Specular;

		r3dString	FarNormalTextureName;
		r3dTexture*	FarNormalTexture;
		r3dString	FarDiffuseTextureName;
		r3dTexture*	FarDiffuseTexture;

		float		FarFadeStart;
		float		FarFadeEnd;

		int			FarLayerEnabled;

		float		ShadowOffset;

		float		FringeHeight;

		Settings();
	};

	static const D3DFORMAT ATLAS_FMT;

	struct AtlasVolume
	{
		r3dScreenBuffer*	Diffuse;
		r3dScreenBuffer*	Normal;

		Bytes				Occupied;
		int					FreeTiles;
	};

	struct MaskAtlasVolume
	{
		r3dTexture*			Mask;

		Bytes				Occupied;
		int					FreeTiles;
	};

	struct HeightNormalAtlasVolume
	{
		r3dTexture*			Height;
		r3dTexture*			Normal;

		Bytes				Occupied;
		int					FreeTiles;
	};

	struct CompressedTileInfo
	{
		UINT16 MinHeight;
		UINT16 HeightRange;
	};

	struct MegaTexTileInfo
	{
		struct LinkedArraySettings
		{
			enum
			{
				VACANT_VALUE = -1,
				CHUNK_LENGTH = 8
			};
		};

		typedef r3dTL::TLinkedArray< char, LinkedArraySettings > LayerListArr;

		LayerListArr LayerList;
	};

	struct MegaTexTile
	{
		typedef r3dTL::TArray< CompressedTileInfo > CompressedTileInfoArr;
		typedef r3dTL::TArray< CompressedTileInfoArr > CompressedTileInfoMips;
		typedef r3dTL::TArray< r3dBitMaskArray2D > BitMaskArray;
		typedef r3dTL::TArray< MegaTexTile* > MegaTilePtrArr;

		struct LayerBitMaskEntry
		{
			BitMaskArray Bits;
			int LayerIndex;

			LayerBitMaskEntry();
		};

		typedef r3dTL::TArray< LayerBitMaskEntry > LayerBitMaskArr;

		typedef r3dTL::TLinkedArray< int > MaskEntryList;
		typedef r3dTL::TLinkedArray< int > MaskIndexList;

		struct LinkedRoadArraySettings
		{
			enum
			{
				VACANT_VALUE = 0,
				CHUNK_LENGTH = 3
			};
		};

		typedef r3dTL::TLinkedArray< class obj_Road*, LinkedRoadArraySettings > RoadLinkedList;
		typedef r3dTL::T2DArray< RoadLinkedList > RoadArr2D;
		typedef r3dTL::TArray< RoadArr2D > RoadArr;

		CompressedTileInfoMips	TileInfoMips;
		Bytes					DominantLayerData;

		UShorts					BorderData;
		UShorts					HeightData;

		MaskEntryList	MaskList;
		LayerBitMaskArr	LayerBitMasks;
		RoadArr			RoadMipChain;

		short HeightNormalAtlasVolumeId;
		short HeightNormalAtlasTileId;

		short X;
		short Z;

		char L;

		char ActiveBitMasks;

		char IsLoaded;
		char Tagged;

		char IsAllocated;

		char Dirty;

#ifndef FINAL_BUILD
		LONG IsLoading;
#endif

		MegaTexTile();

		R3D_FORCEINLINE void ClearMaskList();
		R3D_FORCEINLINE void PushMaskEntry( int entry );

		void	Init();

		int		IsInAtlas() const;

		INT64	GetID( int maskIdx ) const;

		static INT64 GetID( short x, short z, char l, int maskId );
		static void DecodeID( INT64 Id, short* oX, short* oZ, char* oL, int* oMaskId );
		static void DecodeMaskEntry( UINT32 entryVal, int* oAtlasVolumeID, int* oAtlasTileID, int* oMaskIdx );
		static void EncodeMaskEntry( UINT32* oEntryVal, int atlasVolumeID, int atlasTileID, int maskIdx );
	};

	struct AllocatedTile
	{
		enum TypeFlags
		{
			FAR_TILE	= 1,
			NEAR_TILE	= 2,
		};

		MegaTexTile* MegaTile;

		UShorts AdjecantHeights;

		UINT16 AdjecantMin;
		UINT16 AdjecantMax;

		short AdjecantPosZOffset;

		short X;
		short Z;

		short AtlasTileID;
		short AtlasVolumeID;

		char Tagged;
		char L;

		char ConFlags;
		char MegaConFlags;
		char Dirty;
		char IsCorner;

		char FarNearFlags;

		void	Init();
	};

	struct DebugTileInfo
	{
		int X;
		int Z;
		int L;

		int AtlasTileX;
		int AtlasTileZ;
		int AtlasVolumeID;

		int MegaX;
		int MegaZ;
		int MegaL;
		int MegaMaskX;
		int MegaMaskZ;
		int MegaMaskAtlasVolumeId;

		r3dTexture* TileAtlasVolumeDiffuse;
		r3dTexture* TileAtlasVolumeNormal;
		r3dTexture* MegaMaskAtlasVolume;

		DebugTileInfo();
	};

	enum
	{
		MEGATEXTILE_ENTRY_CHUNK = 512
	};

	typedef r3dTL::TFixedArray< MegaTexTile, MEGATEXTILE_ENTRY_CHUNK > MegaTexTileEntryChunk;
	typedef r3dTL::TArray< MegaTexTileEntryChunk* > MegaTexTileEntryArr;

	typedef r3dTL::TFixedArray< float, NUM_QUALITY_LAYERS > LayerFloats;

	typedef r3dTL::TArray< r3dTerrain3Layer > TerrainLayerArr;
	typedef r3dTL::TArray< AtlasVolume > Atlas;
	typedef r3dTL::TArray< MaskAtlasVolume > MaskAtlas;
	typedef r3dTL::TArray< HeightNormalAtlasVolume > HeightNormalAtlas;
	typedef r3dTL::TFixedArray< int, 16 > ConnectionArray;
	typedef r3dTL::TArray< ConnectionArray > ConnectionArrays;

	typedef r3dTL::T2DArray< char > TileQLMap2D;

	typedef r3dTL::TArray< AllocatedTile > AllocatedTileArr;
	typedef r3dTL::TArray< AllocatedTile* > AllocatedTilePtrArr;
	typedef r3dTL::TArray< AllocatedTileArr > AllocatedTileLodArray;

	typedef r3dTL::TArray< MegaTexTile* > MegaTexTilePtrArr;
	typedef r3dTL::TArray< MegaTexTilePtrArr > MegaTexTileLodArray;

	typedef r3dTL::TArray< int > IntArr;

	typedef r3dTL::TArray< float > LayerChannel;
	typedef r3dTL::TArray< LayerChannel > LayerChannels;

	typedef r3dTL::T2DArray< float > Floats2D;

	typedef r3dTL::T2DArray< MegaTexTileInfo > MegaTexTileInfoArr;
	typedef r3dTL::TArray< MegaTexTileInfoArr > MegaTexTileInfoMipChain;

	typedef r3dTL::TArray< DecalRecord > DecalRecords;
	typedef r3dTL::TArray< char > CharArr;

	struct MegaTexFileOffset
	{
#ifndef FINAL_BUILD
		UINT64 Value;
		UINT64 EditorValue : 63;
		UINT64 IsInEditorFile : 1;
#else
		UINT64 Value;
#endif
		MegaTexFileOffset();
	};

	struct MegaTexFileGridOffsetBase
	{
		UINT64 Value : 56;
		UINT64 GridId : 8;
	};

	struct MegaTexFileGridOffset : MegaTexFileGridOffsetBase
	{
		UINT64 EditorValue : 63;
		UINT64 IsInEditorFile : 1;

		MegaTexFileGridOffset();
	};

	typedef std::map< INT64, MegaTexFileOffset > FileTextureMap;
	typedef std::map< INT64, MegaTexFileGridOffset > FileTextureGridMap;

	typedef std::map< INT64, INT32 > EditorReplacementMaskTiles;

	struct UnpackedHeightTile
	{
		UShorts height;
		int tileX;
		int tileZ;

		int Freshness;

		UnpackedHeightTile();
	};

	typedef r3dTL::TArray< UnpackedHeightTile > UnpackedHeightTileArr;

public:
	r3dTerrain3();
	~r3dTerrain3();

public:
	static void LoadShaders();
	static void ReloadShaders();

	void	Init();
	void	InitDynamic();
	void	CloseDynamic();
	void	Close();

	void	Update( const r3dCamera& cam );
	void	Render( const r3dCamera& cam );

private:

	void	DoUpdate( const r3dCamera& cam );
	void	DoRender( const r3dCamera& cam );

public:
	void	RenderShadows();
	void	RenderDepth();

	void	DrawDebug();

	void	RecalcLayerVars();

	// functions to load from old terrain format for conversion
	void	SaveEmpty( const CreationParams& createParams );

	int		Save( const char* targetDir );
	int		Load();

	const Settings& GetSettings() const;
	void			SetSettings( const Settings& sts );

	const QualitySettings&	GetBaseQualitySettings( int ql ) const;
	const QualitySettings&	GetCurrentQualitySettings() const;
	void					SetQualitySettings( const QualitySettings& settings, bool affectBaseQS, int affectBaseQSIdx );

	void	GetStats( r3dTerrain3Stats* oStats ) const;

	void	ResetAtlasTiles();
	void	RefreshAtlasTiles();
	void	RefreshAtlasTilesAndFreeAtlas();

	void	RefreshAllTiles();
	
	void	RefreshAtlasTiles( const RECT& rect );

	void	RefreshAtlasTile( int tileX, int tileZ );
	void	RefreshAllMaskAtlasTiles();
	void	RefreshMaskAtlasTile( int tileX, int tileZ, int L, RefreshMode mode );
	void	RefreshHeightNormalAtlasTile( int tileX, int tileZ, int L, int flags );

	void	SetPhysUserData( void* data );

	void	UpdateRoadInfos();
	void	UpdateRoadInfo( MegaTexTile* tile );
	void	OnRoadDelete( class obj_Road* road );

	// make roads visible again - for editor when terrain2 is switched off in favour of terrain1
	void	ReleaseRoads();
	void	TurnOffBakedRoads();

	bool	UpdateHeightRanges( const Floats& floatHeights );
	void	ConvertHeightField( Shorts* oHeights, const Floats& floatHeights, const RECT* rect );
	void	UpdateGraphicsHeightField( const Shorts& heights, const RECT* rect );
	void	RecalcTileField( int tileX, int tileZ, int L, const UShorts& heightField );

	const r3dTerrain3Layer&	GetLayer( int idx ) const;
	void					SetLayer( int idx, const r3dTerrain3Layer& layer );

	void					UpdateTilesWithLayer( int idx );

	void					InsertLayerInfo( int tileX, int tileZ, int tileL, int lidx );
	void					InsertLayerInfo( int tileX, int tileZ, int lidx );
	void					MarkLayer( int baseX, int baseZ, int lidx );

	void					CheckMegaTileConsistency( const MegaTexTile* mtt );

	void					GetDebugTileInfoAt( r3dPoint3D& pt, DebugTileInfo* oInfo, int requestedMaskId );

	r3dTerrainOptiStats		OptimizeLayerMasks( int isAutoSave, r3dTL::TArray< UINT16 > * oTempData, IDirect3DTexture9* packedTempMask, IDirect3DTexture9* unpackedTempMask );

	void					InsertLayer(	int idx, Floats* channel, UShorts* tempShorts, 
											IDirect3DTexture9* tempPackedMask,
											IDirect3DTexture9* tempUnpackedMask,
											const r3dTerrain3Layer* layerDesc );

	void					ReportProgress( const char* msg );

	void					DeleteLayer( int lidx, IDirect3DTexture9* packedTempMask, IDirect3DTexture9* unpackedTempMask );

	void					UpdateDominantLayerData();

	int						GetNumDecals() const;
	const DecalRecord&		GetDecalRecord( int idx ) const;
	void					SetDecalRecord( int idx, const DecalRecord& record );
	void					AddDecalRecord();

	void					UpdateQualitySettings();
	void					SwitchVFetchMode();

	void					UnpackHeight_OLD( r3dTL::TArray< UINT16 > * oData, int X, int Z, int L );
	void					UnpackHeight( r3dTL::TArray< UINT16 > * oData, int X, int Z, int L );

	UShorts&				UnpackHeightCached( int X, int Z );
	void					DefreshCachedHeights();

	template< typename T >
	void					UnpackHeightFrom( T file, INT64 offset, r3dTL::TArray< UINT16 > * oData, int X, int Z, int L );

#ifndef FINAL_BUILD
	int						HasMask_OLD( int X, int Z, int L, int MaskId );
#endif

	int						HasMask( int X, int Z, int L, int MaskId );

#ifndef FINAL_BUILD
	void					UnpackMask_OLD( r3dTL::TArray< UINT16 > * oData, IDirect3DTexture9* TempPackMask, IDirect3DTexture9* TempMask, int X, int Z, int L, int MaskId );
#endif

	void					UnpackMask( r3dTL::TArray< UINT16 > * oData, IDirect3DTexture9* TempPackMask, IDirect3DTexture9* TempMask, int X, int Z, int L, int MaskId );

	template< typename T >
	void					UnpackMaskFrom( T file, INT64 offset, r3dTL::TArray< UINT16 > * oData, IDirect3DTexture9* TempPackedMask, IDirect3DTexture9* TempMask, int X, int Z, int L, int MaskId );

	template< typename T >
	void					UnpackMaskFromInMainThread( T file, INT64 offset, r3dTL::TArray< UINT16 > * oData, IDirect3DTexture9* TempPackedMask, IDirect3DTexture9* TempMask, int X, int Z, int L, int MaskId );

	int						GetMaskTileSizeInFile();
	int						GetHeightTileSizeInFile();
	int						GetNormalTileSizeInFile();
	int						GetHeightNormalRecordSizeInFile();

	void					UpdateMask( const r3dTL::TArray< UINT16 > * data, IDirect3DTexture9* tempPackedTex, IDirect3DTexture9* tempUnpackedTex, int X, int Z, int L, int MaskId );
	void					UpdateMaskInMainThread( const r3dTL::TArray< UINT16 > * data, IDirect3DTexture9* tempPackedTex, IDirect3DTexture9* tempUnpackedTex, int X, int Z, int L, int MaskId );
	MegaTexFileGridOffset	GetEditorHeightNormalOffset( int X, int Z, int L, int * oIsNew );
	void					WriteHeightToMipFile( FILE* file, const r3dTL::TArray< UINT16 > * data, int X, int Z, int L );
	void					WriteNormalsToMipFile( FILE* file, const r3dTL::TArray< UINT16 > * data, int X, int Z, int L, IDirect3DTexture9* unpackedTempNormals, IDirect3DTexture9* packedTempNormals, r3dTL::TArray< UINT16 > * tempData );
	void					WriteNormalsToMipFileInMainThread( FILE* file, const r3dTL::TArray< UINT16 > * data, int X, int Z, int L, IDirect3DTexture9* unpackedTempNormals, IDirect3DTexture9* packedTempNormals, r3dTL::TArray< UINT16 > * tempData );
	void					UpdateHeight( const r3dTL::TArray< UINT16 > * data, Bytes* tempNormalData, int X, int Z, int L );
	void					UpdateTileNormalEdges( int X, int Z, int L, r3dTL::TArray< UINT16 > * tempData0, r3dTL::TArray< UINT16 > * tempData1, r3dTL::TArray< UINT16 > * tempData2 );
	void					UpdateNormalEdges( int X, int Z, int L, UINT16* target, int targCount, r3dTL::TArray< UINT16 > * tempData );
	void					UpdateNormals( const r3dTL::TArray< UINT16 > * data, int X, int Z, int L, IDirect3DTexture9* unpackedTempNormals, IDirect3DTexture9* packedTempNormals, r3dTL::TArray< UINT16 > * tempData );
	void					UpdateHeightNormalsPacked( const r3dTL::TArray< UINT16 > * heightData, const r3dTL::TArray< UINT16 > * normalData, int X, int Z, int L );
	void					ReadNormalsPacked( r3dTL::TArray< UINT16 > * oData, int X, int Z, int L );
	void					RecalcNormalMap( const UShorts* ( &heights )[ 9 ], Floats* tempHeights, Vectors* tempVectors0, Vectors* tempVectors1, UShorts* oTempNormals, int L );
	void					ReloadMasks( int X, int Z, int L );

	Info					GetInfo() const;
	int						GetMaskCount() const;

	void					GetHeightTileCountsForLod( int L, int* oTileX, int* oTileZ ) const;

#ifndef FINAL_BUILD
	void					AddReplacementTile( int X, int Z, int L, int maskIdx, IDirect3DTexture9* tex );
	void					ClearEditorReplacementTileIndexes();
#endif

	void					MarkDirtyPhysChunk( int tileX, int tileZ );

#ifndef FINAL_BUILD
	void					ToggleCameraFixed();
	int						IsCameraFixed() const;
#endif

	void					RecreateLowResNormals();

	void					DEBUG_PrintMegaTileMap();
	void					DEBUG_PrintStats();

	void					DEBUG_CheckTileInfoConsistency();

	float					GetPaddingCoef() const;
	float					GetNormalPaddingCoef() const;
	float					GetMaskPaddingCoef() const;

	void					CreateDefaultPoolResources();
	void					DestroyDefaultPoolResources();

	int						CheckMegaMap();
	int						CheckMegaOffsets();

	void					MarkMegaTilesRecalculated();
	int						AreMegaTilesRecalculated();

	int						GetPhysChunkSize() const;

	void					DEBUG_ResetTerraPhysics();

	void					ClearMegaLayerInfos();
	void					AddLayerToMegaInfo( int tx, int tz, int L, int layerIdx );

	void					DEBUG_RenderTileBoundaries();

	int						IsMegaTileUsed( int tx, int tz, int L );
	void					ReloadMegaTileInMainThread( int tx, int tz, int L, int flags );

	void					SwapLayers( int layerIdx0, int layerIdx1 );
	void					ClearLayerInfo( int layerIdx );

	// r3dITerrain implementation
public:
	virtual float				GetHeight( int x, int z )			OVERRIDE;
	virtual float				GetHeight( const r3dPoint3D& pos )	OVERRIDE;
	virtual r3dPoint3D			GetNormal( const r3dPoint3D& pos )	OVERRIDE;
	virtual void				GetNormalTangentBitangent(const r3dPoint3D &pnt, r3dPoint3D *outNorm, r3dPoint3D *outTangent, r3dPoint3D *outBitangent) OVERRIDE;
	virtual void				GetHeightRange( float* oMinHeight, float* oMaxHeight, r3dPoint2D start, r3dPoint2D end ) OVERRIDE;
	virtual const MaterialType*	GetMaterialType( const r3dPoint3D& pnt ) OVERRIDE;
	virtual bool				IsLoaded() OVERRIDE;
	virtual bool				PrepareOrthographicTerrainRender( int renderTargetWidth, int renderTargetHeight ) OVERRIDE;
	virtual bool				DrawOrthographicTerrain( const r3dCamera& Cam, bool UseZ ) OVERRIDE;
	virtual void				ReloadTextures() OVERRIDE;
	virtual r3dTexture*			GetDominantTexture( const r3dPoint3D &pos ) OVERRIDE;
	virtual r3dTexture*			GetNormalTexture() const OVERRIDE;
	virtual int					IsPosWithinPreciseHeights( const r3dPoint3D& pos ) const OVERRIDE;

	int							GetNumLoadingMegaTiles() const;

private:
	void					Construct();
	void					Destroy();

	void					CreateDynaVertexBuffer();

	void					MakeSaveDir( const char* dir );
	int						DoLoad( const char* dir );
	int						SaveINI( const char* targetDir, int rescaleVertexCountX, int rescaleVertexCountZ );
	int						DoSave( const char* targetDir, int rescaleVertexCountX, int rescaleVertexCountZ, bool saveMega );

	int						LoadBin( const char* dirName );

	void					ExtractFloatHeights( const Shorts& shorts, Floats* oFloats );

	void					InitFromHeights_MEGA( const Shorts& shorts );

	void					SaveLayerToScript( FILE* hFile, const r3dTerrain3Layer& layer, char* tab );
	int						LoadLayerFromScript( Script_c *script, r3dTerrain3Layer *layer );
	void					SetupHFScale();

	void					InitTileInfoMipChain( const QualitySettings& qs );

	void							UpdateHFShape( PhysicsChunk* chunk );

	PxHeightFieldGeometry			GetHFShape( PhysicsChunk* chunk ) const;

	virtual PxHeightFieldGeometry	GetHFShape() const OVERRIDE;

	void					PreparePhysXHeightFieldDesc_NoAlloc( PxHeightFieldDesc* hfDesc );
	void					PreparePhysXHeightFieldDesc( PxHeightFieldDesc* hfDesc );
	void					FinishPhysXHeightFieldDesc	( PxHeightFieldDesc* hfDesc );

	void					UpdatePhysChunks( const r3dCamera& cam );
	void					AllocatePhysChunkSamples( PhysicsChunk* oChunk, int physicsTileCellCount );
	void					CreatePhysHeightField( PhysicsChunk& chunk, int chunkX, int chunkZ );
	void					DoCreatePhysHeightField( PhysicsChunk& chunk, int chunkX, int chunkZ, int physicsTileCellCount );
	void					DestroyPhysHeightField( PhysicsChunk& chunk );

	void					UpdateSceneBox();

	void	RecalcLayerVar( r3dTerrain3Layer* oLayer );
	void	RecalcTileCounts();

	void	UpdateDesc();

	void	RecalcVars();
	void	InitTileField();
	void 	UpdateMegaTexQualityLayerCount();
	void	InitMegaTexTileField();
	void	RecalcTileInfo( int tileX, int tileZ, int L, const UShorts& heightField );
	void	UpdateMegaTexTileCompressedTileInfoMipChain( MegaTexTile* megaTile, const UINT16* heightField );
	void	InitLayers();
	void	UpdateTileHeights();

	void	UpdateVisibleTiles();

	void	AddToAllocatedTiles( int x, int z, int L, int conFlags );

	MegaTexTile* GetFreeMegaTexTile();

	void	AddToAllocatedMegaTexTiles( int x, int z, int L );

	void	UpdateMegaTexTiles( const r3dCamera& cam );

	void	UpdateTiles_G( const r3dCamera& cam );
	void	UpdateTileMips();
	void	UpdateTileRoads();

	void	AddAtlasVolume();
	void	FreeAtlas();

	void	AllocateAtlasTile( AllocatedTile* tile );
	void	FreeAtlasTile( AllocatedTile* tile );

	void	AddMaskAtlasVolume();
	void	CreateMaskTexture(r3dTexture *mask);
	void	FreeMaskAtlas();

	void	AllocateMaskAtlasTile( MegaTexTile* tile );
	void	AppendToMaskAtlasTile( MegaTexTile* tile );
	void	FreeMaskAtlasTile( MegaTexTile* tile );

	void	AddHeightNormalAtlasVolume();
	void	CreateHeightNormalTexture(r3dTexture *tex, D3DFORMAT fmt);
	void	FreeHeightNormalAtlas();

	void	AllocateHeightNormalAtlasTile( MegaTexTile* tile );
	void	FreeHeightNormalAtlasTile( MegaTexTile* tile );

	void	AllocateMegaTexTile( MegaTexTile* tile );
	void	FreeMegaTexTile( MegaTexTile* tile );
	void	FreeAllMegaTexTiles();

#ifndef FINAL_BUILD
	void	AddEditorMaskTexAtlasVolume();
	void	FreeEditorMaskTexAtlas();

	void	AllocateEditorMaskTexTile( INT32* oID, int maskId );
	void	DeallocateAllEditorMaskTexTiles();
#endif

	int2	GetTileAtlasXZ( const AllocatedTile* tile ) const;
	int2	GetMaskTileAtlasXZ( int tileId ) const;
	int2	GetHeightNormalAtlasXZ( int tileId ) const;

	void	SetTileGeomVDecl( bool forAtlas );
	void	SetTileGeomVDeclNoVFetch();

	void	StartUsingTileGeom( bool forAtlas );
	void	StopUsingTileGeom();

	void	SetupTileAtlasRTs( const AllocatedTile* tile );

	void	StartTileUpdating();
	void	SetAtlasTilePosTransform( const AllocatedTile* tile, float4* oPos );
	void	SetTileUpdateVSConsts_G( const AllocatedTile* tile, const MegaTexTile* megaTexTile, int passN, const IntArr& layerIndexes );
	void	SetTileUpdatePSConsts( const AllocatedTile* tile, int startLayer, const IntArr& layerIndexes );
	void	FillTempTileLayerArray( const AllocatedTile* tile );
	void	UpdateTileInAtlas_G( AllocatedTile* tile );

	int		IsMegaTileOnBorder( MegaTexTile* megaTile, AllocatedTile* allocTile ) const;

	template< r3dTerrain3::Side side, int IS_FULL >
	void	FillAdjecantTileVertices( AllocatedTile* atile, int sideCount, int innerCount );

	void	UpdateAdjecantHeights( AllocatedTile* tile );
	void	StopTileUpdating();

	MegaTexTile* FindCoveringMegaTexTile( const AllocatedTile* tile );
	MegaTexTile* GetMegaTexTile( int X, int Z, int L );

	void	FlushAtlasRTs();

	void	StartTileRoadUpdating();
	void	SetupTileRoadUpdateCamera( const AllocatedTile* tile );
	void	UpdateAtlasTileRoads( const AllocatedTile* tile );
	void	StopTileRoadUpdating();

	void	StartMipChainsTileRendering();
	void	RenderTileMipChain(	const AllocatedTile* tile );
	void	StopMipChainsTileRendering();

	void	SetMegaVertexShader( int shadow, int depth, int cam_vec );
	void	SetMegaVertexShaderNoVFetch( int shadow, int depth, int cam_vec );

	void			SetupTileRenderingVertexStates();
	void			StartTileRendering();
	void			SetAtlasTileTexcTransform( const AllocatedTile* tile, float4* oTransform );
	void			SetMaskAtlasTileTexcTransform( int tileId, const AllocatedTile* tile, const MegaTexTile* megaTile, float extraSpaceCoef, float4* oTransform );
	void			SetNormalAtlasTileTexcTransform( const MegaTexTile* megaTile, const AllocatedTile * tile, float4* oTransform );
	void			SetHeightAtlasTileTexcTransform( const MegaTexTile* megaTile, const AllocatedTile * tile, float4* oTransform, int megaConnFlags );
	void			SetTileRenderVSConsts_G( const AllocatedTile* tile, const r3dCamera& cam );
	void			SetDynaBufferFor( const AllocatedTile* tile, int density, int staticBufferOffset );

	int				GetTileFringeOffset( const AllocatedTile* tile );

	template < int CON_FLAG, int IS_FULL >
	void			SetMegaConnectionDynaBufferFor( const AllocatedTile* tile, int density );

	template < int CON_FLAG >
	void			RenderTileConnection( const AllocatedTile* tile );

	void			Set1x1MegaConnectionDynaBuffer( const AllocatedTile* tile );
	void			RenderConnected1x1Tile( const AllocatedTile* tile );

	void			IssueTileDIPs( const AllocatedTile* tile, int shadow, int cam_vec );

	void			RenderTile_G( const AllocatedTile* tile, const r3dCamera& cam, bool forceNear, bool skipPixelShader = false );
	void			EndTileRendering();

	void	StartShadowRender();
	void	RenderShadowTile( const AllocatedTile* tile, const r3dCamera& cam );
	void	StopShadowRender();

	void	StartDepthRender();
	void	RenderDepthTile( const AllocatedTile* tile, const r3dCamera& cam );
	void	StopDepthRender();

	void	GetBaseTileCounts( int* oBaseTileCountX, int* oBaseTileCountZ );
	void	FreeAtlasTiles();
	void	FreeMegaAtlasTiles();
	void	RemoveAllocatedTiles();

	const QualitySettings&	SelectQualitySettings();
	void					PopulateInferiorQualitySettings();

	void					UpdateEditor();

	void					GetMaskTileXY( const r3dPoint3D& pos, int* oOutX, int* oOutZ );

	void					OptimizeMegaTexTiles();
	
	MegaTexFileGridOffset	GetMegaTexMaskOffsetInFile( MegaTexTile* tile, int maskIdx );
	MegaTexFileOffset		GetMegaTexMaskOffsetInFile_OLD( MegaTexTile* tile, int maskIdx );

	MegaTexFileGridOffset	GetMegaTexHeightNormalOffsetInFile( MegaTexTile* tile );
	MegaTexFileOffset		GetMegaTexHeightNormalOffsetInFile_OLD( MegaTexTile* tile );

	void					GetMegaTexHeightNormalTargetOffsetInFile( MegaTexTile* tile, MegaTexFileGridOffset* oOffset );
	INT64					GetMegaTexHeightNormalTargetOffsetInFile( MegaTexTile* tile );

	static void				MegaTexTileLoadJob( struct r3dTaskParams* parameters );
	void					AddMegaTexTileLoadJobs( MegaTexTile* tile, int loadFlags );
	void					AddMegaTexTileUpdateJobs( MegaTexTile* tile, int loadFlags );
	void					AddMegaTexTileJobs( MegaTexTile* tile, int loadFlags );

	void					UnpackDXT1( void* src, UShorts* dest, int width, int height );

	template < typename T >
	void					DoLoadMegaTexTileMaskFromFile( T file, INT64 offset, D3DSURFACE_DESC* maskSurfDesc, r3dD3DTextureTunnel targetMask, int expectedMaskId, MegaTexTile* targetTile, Bytes* dominantLayerData, Bytes* dominantLayerValues, int bCheckEmpty );

	template < typename T >
	void					DoLoadMegaTexTileHeightAndNormalFromFile( T file, INT64 offset, MegaTexTile* tile, UINT16 *targetNormal, UINT16* targetHeight, int loadFlags );

	void					UpdateHeightNormalAtlasTile( MegaTexTile* tile, r3dD3DTextureTunnel targetNormal, r3dD3DTextureTunnel targetHeight );

	void					DoLoadMegaTexTile( MegaTexTile* tile, int loadFlags );

	void					CloseEditorFile();
	void					CloseMegaTexGridFiles();
	void					GetEditorFilePath( char ( &oPath ) [ 1024 ] );
	void					OpenEditorFile();
	void					OpenMegaTexGridFiles( const char* levelPath );

	void					ReadDistribStamp( const char* levelPath, Bytes* oBytes );
	void					UpdateMegaTexMipsFile( const char* levelPath );
	void					DeleteMegaTexAtlasMipsFile();
	void					OpenMegaTexAtlasMipsFile( const char* accessMode );

	int						LoadMegaTexFile( const char* levelPath );
	int						LoadMegaTexFile_OLD( const char* levelPath );
	void					CreateMegaTexFile( const char* levelPath, UINT16 initialHeight );

	void					SaveMegaTexMapFile( const char* levelPath );
	int						LoadMegaTexMapFile( const char* levelPath );
	int						LoadMegaTexMapFile_OLD( const char* levelPath );

	void					SaveMegaTexTileInfos( const char* levelPath );
	int						LoadMegaTexTileInfos( const char* levelPath );
	int						LoadMegaTexTileInfos_OLD( const char* levelPath );

	void					WriteMipStamps( const char* levelPath );

	void					MergeEditedMega( const char* levelPath );
	void					CheckMasksPresenceInInfo( INT64 finalSize, int startMaskIdx, int gridX, int gridZ );
	void					GatherMasksAbsentInInfo( r3dTL::TArray< INT64 > * oArr );
	int						DeleteLayerMasks( MaskIdSet& masksForDeletion, int startMaskIdx );

	template< typename T >
	void					WriteHeightNormalTileInFile( T file, const void * heightData, const void* normalData );

	static void				CreateEditorBlankMaskInMainThreadHelper( void* param );
	void					CreateEditorBlankMask();
	void					CreateEditorBlankMaskInMainThread();

	static void				InitDynamicInMainThread( void* param );
	void					DoInitDynamicInMainThread();

	MegaTexTileInfo*		GetMegaTexTileInfo( MegaTexTile* tile );
	MegaTexTileInfo*		GetMegaTexTileInfo( int x, int z, int L );

	void					GetPhysHeightmapBoundaries( int* oXStart, int *oZStart, int* oXEnd, int* oZEnd );
	void					GetPhysHeightmapBoundaries( float* oXStart, float *oZStart, float* oXEnd, float* oZEnd );

	void					SplitMegaTexFile( const char* levelPath, const char* filePath );

	void					ErrorOnMissingMask( const char* srcFunc, INT64 id );

	void					GetGridCoordinates( const MegaTexFileGridOffsetBase& offset, int *oX, int *oZ );
	void					EncodeGridCoordinates( MegaTexFileGridOffsetBase* oOffset, int x, int z );
	void					GetChunkFileName( char* buff, int x, int z );
	void					GetTileInfosChunkFileName( char* buff, int x, int z );
	void					GetMegaMapChunkFileName( char* buff, int x, int z );

	void					GetMipFileDir( char* buff );
	void					GetMipFileName( char* buff );
	void					GetMipFileStampName( char* buff );
	void					GetDistribStampName( const char* levelPath, char* buff );

	void					CreateMegaTexInfo( const char* dirPath );
	int						ConvertToFileGrid( const char* dirPath );

public:
	void					UpdateLayerMaskMipFromLevelBelow( int tx, int tz, int L, int maskId, UShorts& tempUShorts0, UShorts& tempUShorts1, IDirect3DTexture9* packedTexture, IDirect3DTexture9* unpackedTexture );
	void					RecalcTileCoordBorders( const TileCoords& tcoords, UShorts& tempUShorts0, UShorts& tempUShorts1, IDirect3DTexture9* packedTexture, IDirect3DTexture9* unpackedTexture, void (*reportCallBack)( int, int ) );

private:
	void					CreateMipFile( const char* levelPath );
	Bytes					GenerateStamp();

private:
	AllocatedTileLodArray	m_AllocTileLodArray;

	UShorts					m_MatTypeIdxes;

	int					m_NumActiveQualityLayers;
	AllocatedTilePtrArr	m_TilesToUpdate;
	AllocatedTilePtrArr	m_VisibleTiles;
	AllocatedTilePtrArr	m_DebugVisibleTiles;

	ConnectionArrays	m_ConnectionIndexOffsets;
	ConnectionArrays	m_ConnectionIndexCounts;
	IntArr				m_ConnectionVertexOffsets;
	int					m_4VertTileIndexOffset;
	int					m_DebugVisIndexOffset;

	r3dVertexBuffer*	m_UnitQuadVertexBuffer;
	r3dVertexBuffer*	m_DynamicVertexBuffer;
	int					m_DynamicVertexBufferPtr;

	r3dVertexBuffer*	m_TileVertexBuffer;
	r3dIndexBuffer*		m_TileIndexBuffer;

	unsigned long		m_HeightFieldDataCRC32;

	Atlas				m_Atlas;
	int					m_TotalAllocatedTiles;
	int					m_MaxAllocatedTiles;
	r3dScreenBuffer*	m_TempDiffuseRT;
	r3dScreenBuffer*	m_TempNormalRT;

	// works as replacement for m_HeightTex in case 
	// there's no vertex texture fetch
	UShorts					m_HeightArr;
	UnpackedHeightTileArr	m_UnpackedHeightCache;

	TerrainLayerArr		m_Layers;
	r3dTerrain3Layer	m_BaseLayer;

	int					m_TileCountX;
	int					m_TileCountZ;

	int					m_TilesInFileChunk;
	int					m_FileChunkCountX;
	int					m_FileChunkCountZ;

	int					m_VertexCountX;
	int					m_VertexCountZ;

	int					m_NormalDensity;

	int					m_MaskResolutionU;
	int					m_MaskResolutionV;

	float				m_TerrainPosToMaskU;
	float				m_TerrainPosToMaskV;
	
	float				m_TotalTerrainXLength;
	float				m_TotalTerrainZLength;
	float				m_CellSize;

	float				m_TileUnitWorldDim;

	LayerFloats			m_TileWorldDims;
	LayerFloats			m_HalfTileWorldDims;
	float				m_HeightScale;
	float				m_HeightOffset;

	float				m_HFScale;
	float				m_InvHFScale;

	int					m_AtlasTileCountPerSide;
	float				m_AtlasTileCountPerSideInv;

	float				m_RoadTileUpdateZFar;

	QualitySettings		m_QualitySettings;
		
	QualitySettingsArr	m_BaseQualitySettings;

	Settings			m_Settings;

	r3dScreenBuffer*	m_LastDiffuseRT;
	r3dScreenBuffer*	m_LastNormalRT;

	r3dString			m_Path;

	IntArr				m_TempLayerIndexes;

	int					m_IsLoaded;
	int					m_IsQualityUpdated;

	int					m_RoadInfosUpdated;

	int					m_AllowVFetch;

	DecalRecords		m_DecalRecords;

	//------------------------------------------------------------------------
	// new
	MaskAtlas			m_MaskAtlas;

#ifndef FINAL_BUILD
	MaskAtlas			m_EDITOR_ReplacementMaskAtlas;
#endif

	HeightNormalAtlas	m_HeightNormalAtlas;

	int					m_NumActiveMegaTexQualityLayers;
	int					m_MegaTexTileLodOffset;

	int					m_MaskTotalAllocatedTiles;
	int					m_MaskMaxAllocatedTiles;

	int					m_MaskAtlasTileCountPerSide;
	float				m_MaskAtlasTileCountPerSideInv;

	int					m_HeightNormalTotalAllocatedTiles;
	int					m_HeightNormalMaxAllocatedTiles;

	int					m_HeightNormalAtlasTileCountPerSide;
	float				m_HeightNormalAtlasTileCountPerSideInv;

	int					m_MegaTexAtlasFile_NormalDim;
	int					m_MegaTexAtlasFile_HeightDim;
	int					m_MegaTexAtlasFile_MaskDim;
	int					m_MegaTexAtlasFile_BaseTileCountX;
	int					m_MegaTexAtlasFile_BaseTileCountZ;
	int					m_MegaTexAtlasFile_NormalFormat;
	int					m_MegaTexAtlasFile_HeightFormat;
	int					m_MegaTexAtlasFile_MaskFormat;
	int					m_MegaTexAtlasFile_ColorModulationFormat;

	int					m_MegaTexAtlasFile_NumActiveLayers;

	r3dChunkedFile*		m_MegaTexAtlasFile;
	FILE*				m_MegaTexAtlasMipsFile;

	FileGrid			m_FileGrid;


	HANDLE				m_MegaTexAtlasFile_EDITOR;
	HANDLE				m_MegaTexAtlasFile_EDITOR_Mapping;
	int					m_MegaTexAtlasFile_EDITOR_Size;

	r3dD3DTextureTunnel	m_EDITOR_BlankMask;

#ifndef FINAL_BUILD
	FileTextureGridMap				m_EDITOR_HeightNormalTileMap;
	EditorReplacementMaskTiles		m_EDITOR_ReplacementTileIndexes;

	MegaTexTilePtrArr				m_EDITOR_TilesToReload;
	r3dTL::TArray< INT64 >			m_EDITOR_MaskOffsetsToPurge;

	int								m_EDITOR_IsCameraFixed;
	r3dCamera						m_EDITOR_FixedCamera;
#endif

	int					m_MegaTexAtlasFile_MaskOffset;
	int					m_MegaTexAtlasFile_HeightNormalOffset;
	FileTextureMap		m_MegaTexAtlasFile_Map;
	FileTextureGridMap	m_MegaTexGridFile_Map;

	r3dD3DTextureTunnel	m_MegaTexTile_TempHeight;
	r3dD3DTextureTunnel	m_MegaTexTile_TempNormal;
	r3dD3DTextureTunnel	m_MegaTexTile_TempMask;

	MegaTexTilePtrArr	m_TempMegaTexTileArr;
	MegaTexTilePtrArr	m_TempMegaTexTile_NonAdjArr;
	CharArr				m_TempCharArr;

	MegaTexTileLodArray	m_AllocMegaTexTileLodArray;
	MegaTexTilePtrArr	m_DebugVisibleMegaTexTiles;

	MegaTexTileEntryArr	m_MegaTexTileEntries;

	int					m_PhysChunksCentreX;
	int					m_PhysChunksCentreZ;

	PhysicsChunkArr		m_PhysicsChunks;
	void*				m_PhysUserData;
	void*				m_SharedPhysDescData;

	MegaTexTileInfoMipChain m_MegaTexTileInfoMipChain;
	UShorts					m_UnpackedMask;

	int m_RenderOrthoTextureTileMip;

	r3dTexture*			m_LowResNormalTexture;

	Ints				m_TileUpdateLog;

#ifndef FINAL_BUILD
	CRITICAL_SECTION	m_MegaTexFileCritSec;
	CRITICAL_SECTION	m_MegaTexTileInfoCritSec;

	int					m_MegaTexMipsRecalculated;
#endif

#if R3D_TERRAIN3_ALLOW_FULL_PHYSCHUNKS
	int					m_FullPhysChunkModeOn;
#endif

	int					m_StampID;
	int					m_WasEdited;

	//------------------------------------------------------------------------
};

extern r3dTerrain3*	Terrain3;

//------------------------------------------------------------------------
void InitErosionPattern(const char *filePath);
void FreeErosionPattern();

struct ErosionPattern
{
	DWORD w, h;
	byte *data;

	ErosionPattern() ;

	explicit  ErosionPattern(const char *ddsFileName) ;

	~ErosionPattern() ;

	float Sample(float u, float v) ;

	float SampleBilinear(float u, float v) ;

};

extern ErosionPattern *gErosionPattern ;
//------------------------------------------------------------------------

R3D_FORCEINLINE
float
r3dTerrain3::GetPaddingCoef() const
{
#if 0
	return m_QualitySettings.AtlasTileDim / ( m_QualitySettings.AtlasTileDim - 2.f * TILE_BORDER );
#else
	return DEFAULT_ATLAS_TILE_DIM / ( DEFAULT_ATLAS_TILE_DIM - 2.f * TILE_BORDER );
#endif
}

//------------------------------------------------------------------------

R3D_FORCEINLINE
float
r3dTerrain3::GetNormalPaddingCoef() const
{
	return ( m_QualitySettings.HeightNormalAtlasTileDim ) / ( m_QualitySettings.HeightNormalAtlasTileDim - 2.f * NORMAL_TILE_BORDER );
}

//------------------------------------------------------------------------

R3D_FORCEINLINE
float
r3dTerrain3::GetMaskPaddingCoef() const
{
	return m_QualitySettings.MaskAtlasTileDim / ( m_QualitySettings.MaskAtlasTileDim - 2.0f * MEGA_TEX_BORDER );
}

#ifdef ARTIFICIAL_FINAL_BUILD
#undef FINAL_BUILD
#endif