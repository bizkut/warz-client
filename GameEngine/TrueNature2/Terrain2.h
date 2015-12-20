#pragma once

#ifdef WO_SERVER
#define R3D_TERRAIN_V2_GRAPHICS 0
#else
#define R3D_TERRAIN_V2_GRAPHICS 1
#endif

#include "r3dTypeTuples.h"
#include "../SF/RenderBuffer.h"
#include "../SF/script.h"

#include "../TrueNature/ITerrain.h"

#include "r3dBitMaskArray.h"

#define R3D_TERRAIN2_FLOAT_POS 0

struct r3dTerrainLayer
{
	r3dString Name;

	float ScaleU;
	float ScaleV;

	float ShaderScaleU;
	float ShaderScaleV;

	float SpecularPow;

	r3dTexture* DiffuseTex;
	r3dTexture* NormalTex;

	r3dTexture* MaskTex;
	float4 ChannelMask;
	int ChannelIdx;

	r3dString MaterialTypeName;

	r3dTerrainLayer();
};

struct r3dTerrainStats
{
	int VolumeCount;
	int TileCount;
	int TileTextureMemory;
	int MaskTextureMemory;
	int LayerTextureMemory;

	r3dTerrainStats();
};

#pragma pack(push)
#pragma pack(1)

struct R3D_TERRAIN_VERTEX_2
{
#if R3D_TERRAIN2_FLOAT_POS
	float2 pos;
#else
	short2 pos;
#endif
};

#pragma pack(pop)

extern	r3dITerrain	*Terrain;

class r3dTerrain2 : public r3dITerrain
{
public:
	enum
	{
		NUM_QUALITY_LAYERS	= 12,
		LAYERS_PER_MASK		= 3,
		MAX_TEXTURE_LAYERS	= 255,
		MAX_VERTEX_TILE_DIM	= 8
	};

	enum
	{
		QS_LOW,
		QS_MED,
		QS_HIGH,
		QS_COUNT
	};

	enum
	{
		ATLAS_TEXTURE_DIM = 2048
	};

	struct DecalRecord
	{
		int		TypeIdx;
		float	X;
		float	Z;
		float	Scale;
		float	ZRot;

		DecalRecord();
	};

	struct CreationParams
	{
		int CellCountX;
		int CellCountZ;

		int SplatSizeX;
		int SplatSizeZ;

		float CellSize;
		float Height;

		r3dString LevelDir;

		CreationParams();
	};

	struct QualitySettings
	{
		enum
		{
			MIN_VERTEX_TILE_DIM = 1,
			MIN_VERTEX_TILE_DIM_V101 = 2,
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
		int												RoadVertexDimStart;

		int												BakeRoads;

		// NOTE: update compare when you add new members

		int												Compare( const QualitySettings& sts );

		void											Sync();

		QualitySettings();
	};

	typedef r3dTL::TFixedArray< QualitySettings, QS_COUNT > QualitySettingsArr;

	struct Settings
	{
		float	Specular;
		float	DetailNormalMix;
		float		FarFadeStart;
		float		FarFadeEnd;

		Settings();
	};

	static const D3DFORMAT ATLAS_FMT;

	typedef r3dTL::TArray< unsigned char > Bytes;

	struct AtlasVolume
	{
		r3dScreenBuffer*	Diffuse;
		r3dScreenBuffer*	Normal;

		Bytes				Occupied;
		int					FreeTiles;
	};

	struct TileInfo
	{
		float HeightMin;
		float HeightRange;
	};

	enum
	{
		ROAD_INFO_LENGTH = 4
	};

	// ROAD_INFO_LENGTH-1 obj_Road pointers, if [ROAD_INFO_LENGTH] is non-null, then it's a pointer to another ROAD_INFO_LENGTH road block
	// road pointer array is null-terminated in case of less than ROAD_INFO_LENGTH-1 pointers
	struct RoadInfo
	{
		void* ptrs [ ROAD_INFO_LENGTH ];

		RoadInfo();
	};

	struct AllocatedTile
	{
		short X;
		short Z;

		short AtlasTileID;
		short AtlasVolumeID;

		char Tagged;
		char L;

		char ConFlags;

		void	Init();
	};

	typedef r3dTL::TFixedArray< float, NUM_QUALITY_LAYERS > LayerFloats;
	typedef r3dTL::TArray< r3dTexture* > MaskArray;
	typedef r3dTL::TArray< r3dTerrainLayer > TerrainLayerArr;
	typedef r3dTL::TArray< PxI16 > Shorts;
	typedef r3dTL::TArray< PxU32 > HFSamples;
	typedef r3dTL::TArray< UINT16 > UShorts;
	typedef r3dTL::TArray< float > Floats;
	typedef r3dTL::TArray< r3dPoint3D > Vectors;
	typedef r3dTL::TArray< AtlasVolume > Atlas;
	typedef r3dTL::TFixedArray< int, 16 > ConnectionArray;
	typedef r3dTL::TArray< ConnectionArray > ConnectionArrays;

	typedef r3dTL::TArray< r3dBitMaskArray2D > LayerBaseBitMasks;
	typedef r3dTL::TArray< r3dBitMaskArray2D > LayerBitMaskMipChain;
	typedef r3dTL::TArray< LayerBitMaskMipChain > LayerBitMaskMipChains;

	typedef r3dTL::T2DArray< char > TileQLMap2D;
	typedef r3dTL::T2DArray< TileInfo > TileInfoArr2D;
	typedef r3dTL::TArray< TileInfoArr2D > TileInfoMipChain;

	typedef r3dTL::T2DArray< RoadInfo > RoadInfoArr2D;
	typedef r3dTL::TArray< RoadInfoArr2D > RoadInfoMipChain;

	typedef r3dTL::TArray< AllocatedTile > AllocatedTileArr;
	typedef r3dTL::TArray< const AllocatedTile* > AllocatedTilePtrArr;
	typedef r3dTL::TArray< AllocatedTileArr > AllocatedTileLodArray;

	typedef r3dTL::TArray< int > IntArr;

	typedef r3dTL::TArray< float > LayerChannel;
	typedef r3dTL::TArray< LayerChannel > LayerChannels;

	typedef r3dTL::T2DArray< UINT8 > Bytes2D;
	typedef r3dTL::T2DArray< float > Floats2D;

	typedef r3dTL::TArray< DecalRecord > DecalRecords;


public:
	r3dTerrain2();
	~r3dTerrain2();

public:
	static void LoadShaders();

	void	Init();
	void	InitDynamic();
	void	CloseDynamic();
	void	Close();

	void	UpdateAtlas( const r3dCamera& cam );
	void	Render( const r3dCamera& cam );
	void	RenderShadows();
	void	RenderDepth();

	void	DrawDebug();

	void	RecalcLayerVars();

	// functions to load from old terrain format for conversion
	int		Load1( const char* DirName );
	bool	LoadFromScript1( const char * fileName, const char* sourceLevelPath );
	bool	LoadLayerFromScript1( Script_c *script, r3dTerrainLayer* layer, int* count );	
	void	LoadMaterialsFromScript1( Script_c *script, const char* sourceLevelPath );
	void	ReadTerrainHeader( r3dFile* file, uint32_t& dwSignature, uint32_t& dwVersion );
	bool	LoadBin1( r3dFile* file );
	void	ConvertSplatsFrom1();

	void	SaveEmpty( const CreationParams& createParams );

	int		Save( const char* targetDir );
	int		Load();

	const Settings& GetSettings() const;
	void			SetSettings( const Settings& sts );

	const QualitySettings&	GetBaseQualitySettings( int ql ) const;
	const QualitySettings&	GetCurrentQualitySettings() const;
	void					SetQualitySettings( const QualitySettings& settings, bool affectBaseQS, int affectBaseQSIdx );

	void	GetStats( r3dTerrainStats* oStats ) const;

	void	ResetAtlasTiles();
	void	RefreshAtlasTiles();
	
	void	RefreshAtlasTiles( const RECT& rect );

	void	RefreshAtlasTile( int tileX, int tileZ );	

	void	SetPhysUserData( void* data );

	void	ClearRoadInfo( );
	void	UpdateRoadInfo( );
	void	OnRoadDelete( class obj_Road* road );

	// make roads visible again - for editor when terrain2 is switched off in favour of terrain1
	void	ReleaseRoads( );

	void	SaveHeightField( Floats* oHeights );
	void	SaveHeightField( Shorts* oHeights );
	bool	UpdateHeightRanges( const Floats& floatHeights );
	void	ConvertHeightField( Shorts* oHeights, const Floats& floatHeights, const RECT* rect );
	void	UpdateGraphicsHeightField( const Shorts& heights, const RECT* rect );
	void	UpdateTileField( const Shorts& heightField );
	void	RecalcTileField( const Shorts& heightField, const RECT* rect );
	void	UpdatePhysicsHeightField( const Shorts& heightField );
	void	UpdateNormals( const Floats& heights, Vectors* tempVectors0, Vectors* tempVectors1, const RECT* rect );

	const r3dTerrainLayer&	GetLayer( int idx ) const;
	void					SetLayer( int idx, const r3dTerrainLayer& layer );

	void					UpdateTilesWithLayer( int idx );

	void					MarkLayer( int baseX, int baseZ, int lidx );

	// layer painting helpers
	r3dTexture*				GetLayerMask( int sidx ) const;

	void					UpdateLayerMaskFromReplacementMask( int sidx );
	void					SetReplacementMask( int sidx, r3dTexture* mask );

	void					OptimizeLayerMasks();

	r3dTexture*				GetColorTexture() const;
	void					SetReplacementColorTexture( r3dTexture* tex );
	void					UpdateColorTexture();

	r3dTexture*				GetHeightTexture() const;
	r3dTexture*				GetNormalTexture() const;

	void					InsertLayer( int idx, Floats* channel, const r3dTerrainLayer* layerDesc );
	void					DestroyLayer( int idx );

	void					GetLayerChannel( Floats * oChannel, int idx );

	void					UpdateDominantLayerData();

	int						GetNumDecals() const;
	const DecalRecord&		GetDecalRecord( int idx ) const;
	void					SetDecalRecord( int idx, const DecalRecord& record );
	void					AddDecalRecord();

	void					UpdateQualitySettings();
	void					SwitchVFetchMode();

	void					Resize( int newVertexCountX, int newVertexCountZ );
	void					UpdateLayersForMask( int idx );
	void					EnsureMaskFormat( int idx );

	const r3dString&		GetNormalDetailTextureName();
	r3dTexture*				GetNormalDetailTexture();
	void					SetNormalDetailTexture( const r3dString& Name );

	void					UpdateNormals();

	// r3dITerrain implementation
public:
	virtual float				GetHeight( int x, int z )			OVERRIDE;
	virtual float				GetHeight( const r3dPoint3D& pos )	OVERRIDE;
	virtual r3dPoint3D			GetNormal( const r3dPoint3D& pos )	OVERRIDE;
	virtual void				GetHeightRange( float* oMinHeight, float* oMaxHeight, r3dPoint2D start, r3dPoint2D end ) OVERRIDE;
	virtual const MaterialType*	GetMaterialType( const r3dPoint3D& pnt ) OVERRIDE;
	virtual bool				IsLoaded() OVERRIDE;
	virtual bool				PrepareOrthographicTerrainRender( int renderTargetWidth, int renderTargetHeight ) OVERRIDE;
	virtual bool				DrawOrthographicTerrain( const r3dCamera& Cam, bool UseZ ) OVERRIDE;
	virtual void				ReloadTextures() OVERRIDE;
	virtual void				GetNormalTangentBitangent(const r3dPoint3D &pnt, r3dPoint3D *outNorm, r3dPoint3D *outTangent, r3dPoint3D *outBitangent) OVERRIDE;
	virtual r3dTexture*			GetDominantTexture( const r3dPoint3D &pos ) OVERRIDE;

private:
	void					Construct();
	void					Destroy();

	void					CreateDynaVertexBuffer();

	void					ExtractMaskToChannels( int midx );
	void					ExtractMasksToChannels();
	void					StoreChannelsInMasks();

	int						DoLoad( const char* dir );
	int						DoSave( const char* targetDir, const Shorts& hfShrinkedSamples, int rescaleVertexCountX, int rescaleVertexCountZ );

	int						SaveBin( const char* dirName, const Shorts& hfShrinkedSamples, int rescaleVertexCountX, int rescaleVertexCountZ );
	void					ExpendBaseBitMasksFrom101();
	int						LoadBin( const char* dirName );

	void					ExtractFloatHeights( const Shorts& shorts, Floats* oFloats );
	void					InitFromHeights( const Shorts& shorts, bool recalcNormals );

	void					SaveLayerToScript( FILE* hFile, const r3dTerrainLayer& layer, char* tab );
	int						LoadLayerFromScript( Script_c *script, r3dTerrainLayer *layer );
	void					SetupHFScale();

	void					UpdateHFShape();
	PxHeightFieldGeometry	GetHFShape() const;
	void					PreparePhysXHeightFieldDesc_NoAlloc( PxHeightFieldDesc* hfDesc );
	void					PreparePhysXHeightFieldDesc( PxHeightFieldDesc* hfDesc );
	void					FinishPhysXHeightFieldDesc	( PxHeightFieldDesc* hfDesc );

	void					UpdatePhysHeightField ( const Shorts& source );
	void					UpdatePhysHeightField ( const Floats& heightFieldData );

	void					UpdateSceneBox();


	void	RecalcLayerVar( r3dTerrainLayer* oLayer );
	void	RecalcTileCounts();

	void	UpdateDesc();

	void	RecalcVars();
	void	InitTileField( const Shorts& heightField );
	void	RecalcTileInfo( const Shorts& heightField, const RECT* rect );
	void	InitLayerBaseBitMask( int idx );
	void	InitLayerBaseBitMasks();
	void	InitLayerBitMaskChain( int idx );
	void	InitLayerBitMaskChains();
	void	InitLayers();
	void	DistributeLayerMasks();
	void	UpdateTileHeights();

	void	RecalcNormalMap( const Floats& heights, Vectors* tempVectors0, Vectors* tempVectors1, const RECT* rect );

	void	UpdateBaseBitMask( r3dBitMaskArray2D* array, int baseTileX, int baseTileZ, const Floats& mask );
	void	UpdateTileBitMaskChain( LayerBitMaskMipChain* oChain, const r3dBitMaskArray2D& array, int tileX, int tileZ );

	float	GetPaddingCoef() const;

	void	UpdateVisibleTiles();

	void	AddToAllocatedTiles( int x, int z, int L, int conFlags );
	void	UpdateTiles( const r3dCamera& cam );
	void	UpdateTileMips();
	void	UpdateTileRoads();

	void	AddAtlasVolume();
	void	FreeAtlas();

	void	AllocateAtlasTile( AllocatedTile* tile );
	void	FreeAtlasTile( AllocatedTile* tile );

	int2	GetTileAtlasXZ( const AllocatedTile* tile ) const;

	void	StartUsingTileGeom( bool forAtlas );
	void	StopUsingTileGeom();

	void	SetupTileAtlasRTs( const AllocatedTile* tile );

	void	StartTileUpdating();
	void	SetAtlasTilePosTransform( const AllocatedTile* tile, float4* oPos );
	void	SetTileUpdateVSConsts( const AllocatedTile* tile, int passN, const IntArr& layerIndexes );
	void	SetTileUpdatePSConsts( const AllocatedTile* tile, int startLayer, const IntArr& layerIndexes );
	void	FillTempTileLayerArray( int x, int z, int L  );
	void	UpdateTileInAtlas( const AllocatedTile* tile );
	void	StopTileUpdating();

	void	FlushAtlasRTs();

	void	StartTileRoadUpdating();
	void	SetupTileRoadUpdateCamera( const AllocatedTile* tile );
	void	UpdateAtlasTileRoads( const AllocatedTile* tile );
	void	StopTileRoadUpdating();

	void	StartMipChainsTileRendering();
	void	RenderTileMipChain(	const AllocatedTile* tile );
	void	StopMipChainsTileRendering();

	void	SetupTileRenderingVertexStates();
	void	StartTileRendering();
	void	SetAtlasTileTexcTransform( const AllocatedTile* tile, float4* oTransform );
	void	SetTileRenderVSConsts( const AllocatedTile* tile );
	void	SetDynaBufferFor( const AllocatedTile* tile, int density, int staticBufferOffset );
	void	RenderTile( const AllocatedTile* tile );
	void	EndTileRendering();

	void	StartShadowRender();
	void	RenderShadowTile( const AllocatedTile* tile );
	void	StopShadowRender();

	void	StartDepthRender();
	void	RenderDepthTile( const AllocatedTile* tile );
	void	StopDepthRender();

	void	GetBaseTileCounts( int* oBaseTileCountX, int* oBaseTileCountZ );
	void	GetBaseTileCounts_101( int* oBaseTileCountX, int* oBaseTileCountZ );
	void	FreeAtlasTiles();
	void	RemoveAllocatedTiles();

	void	DoUpdateDominantLayerData( int baseWidth, int baseHeight );

	const QualitySettings&	SelectQualitySettings();
	void					PopulateInferiorQualitySettings();

	void					UpdateEditor();

	void					LoadDetailNormalTex();
	void					ConformNormalTex();

private:
	AllocatedTileLodArray	m_AllocTileLodArray;

	LayerBaseBitMasks		m_LayerBaseBitMasks;
	LayerBitMaskMipChains	m_LayerBitMasksMipChains;
	TileInfoMipChain		m_TileInfoMipChain;
	RoadInfoMipChain		m_RoadInfoMipChain;

	Bytes2D					m_DominantLayerData;
	UShorts					m_MatTypeIdxes;

	LayerChannels			m_LayerChannels;

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

	PxRigidStatic*		m_PhysicsTerrain;
	PxHeightFieldDesc	m_PhysicsHeightFieldDesc;
	PxHeightField*		m_PhysicsHeightField;

	unsigned long		m_HeightFieldDataCRC32;

	Atlas				m_Atlas;
	int					m_TotalAllocatedTiles;
	int					m_MaxAllocatedTiles;
	r3dScreenBuffer*	m_TempDiffuseRT;
	r3dScreenBuffer*	m_TempNormalRT;

	// works as replacement for m_HeightTex in case 
	// there's no vertex texture fetch
	UShorts				m_HeightArr;

	r3dTexture*			m_HeightTex;
	r3dTexture*			m_NormalTex;
	r3dTexture*			m_DetailNormalTex;
	r3dTexture*			m_ColorTex;

	r3dString			m_DetailNormalTexSrc;

	MaskArray			m_Masks;

	TerrainLayerArr		m_Layers;
	r3dTerrainLayer		m_BaseLayer;

	int					m_TileCountX;
	int					m_TileCountZ;

	int					m_VertexCountX;
	int					m_VertexCountZ;

	int					m_NormalDensity;

	int					m_SplatResolutionU;
	int					m_SplatResolutionV;

	float				m_TerrainPosToSplatU;
	float				m_TerrainPosToSplatV;
	
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

	int					m_AllowVFetch;

	DecalRecords		m_DecalRecords;
};

extern r3dTerrain2*	Terrain2;