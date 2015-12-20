#pragma once

#include "../UndoHistory/UndoHistory.h"

#if !defined(FINAL_BUILD) && !defined(WO_SERVER)

#include "TrueNature2/Terrain3.h"

class Terrain3Editor
{
public:
	friend class CHeightChanged3;
	friend class CLayerMaskPaint3;
	friend class CLayerColorPaint3;
	friend class CLayerMaskEraseAll3;

	typedef r3dTerrain3::Bytes Bytes;

	typedef r3dTL::TArray< float > Floats;
	typedef r3dTL::TArray< PxI16 > Shorts;
	typedef r3dTL::TArray< UINT16 > UShorts;
	typedef r3dTL::TArray< r3dTexture* > PaintMasks;
	typedef r3dTL::TArray< uint32_t > UInts;
	typedef r3dTL::TArray< r3dPoint3D > Vectors;
	typedef r3dTL::TArray< UShorts* > UShortsPtrArr;

	enum
	{
		MAX_MASK_CACHE_ENTRIES = 1024
	};

	struct UnpackedMaskTile
	{
		UShorts Data;
		int X;
		int Z;
		int L;
		int MaskId;

		int Freshness;
		int Synced;

		UnpackedMaskTile();
	};

	typedef r3dTerrain3::TileCoord TileCoord;

	typedef r3dTL::TArray< TileCoord > TileCoords;

	struct UnpackedHeightTile
	{
		r3dTL::TArray< UINT16 > Data;
		r3dTL::TArray< float > FloatData;

		int X;
		int Z;
		int L;

		int Freshness;
		int Synced;

		UnpackedHeightTile();
	};

	typedef std::map< UINT64, UnpackedMaskTile > UnpackedMaskTiles;
	typedef std::map< UINT64, UnpackedHeightTile > UnpackedHeightTiles;

	struct NoiseParams
	{
		r3dPerlinNoise	noise;

		float			maxHeight;
		float			minHeight;
		float			heightApplySpeed;
		int				heightRelative;

		bool		painting_;

		NoiseParams();
		~NoiseParams();

		void	DrawNoiseParams( float& SliderX, float& SliderY );
		float	GetNoise( int x, int y );
		void	Apply( const r3dPoint3D& pos, float radius, float hardiness );

	} noiseParams;

public:
	Terrain3Editor();
	~Terrain3Editor();

public:
	void Update();

	void SaveTerrain( const char * targetDir );

	template < typename T >
	void OptimizeMaskCache( T& cache );

	UnpackedMaskTile* GetUnpackedMaskTile( int X, int Z, int L, int MaskID );
	UnpackedHeightTile* GetUnpackedHeightTile( int X, int Z, int L );

	void FinalizeHeightEditing();

	void LoadColorsFromTerrain();

	int AreHeightsLoaded() const;

	void StartHeightEditing();

	float GetPaddingExtraSpace();

	template< typename OutFunc, typename GatherFunc >
	void ApplyHeightBrushT( const r3dPoint3D &pnt, const float strength, const float radius, const float hardness, OutFunc outFunc, GatherFunc* gatherFunc, int gatherSize );

	void ApplyHeightBrush(const r3dPoint3D &pnt, const float strength, const float radius, const float hardness);
	void ApplyHeightLevel(const r3dPoint3D &pnt, const float H, const float strength, const float radius, const float hardness);
	void ApplyHeightSmooth(const r3dPoint3D &pnt, const float radius, const float hardness );
	void ApplyHeightErosion(const r3dPoint3D &pnt, const float strength, const float radius, const float hardness);
	void ApplyHeightNoise(const r3dPoint3D &pnt, const float radius, const float hardness);
	void ApplyHeightRamp(const r3dPoint3D& rampStart, const r3dPoint3D& rampEnd, const float rampWidthOuter, const float rampWidthInner);

	void EndHeightEditing();

	void StartLayerBrush_G( int layerIdx );
	void ApplyRect( int xStart, int zStart, int xSize, int zSize, int L, const uint16_t* data );
	void ApplyLayerBrush_G(const r3dTerrainPaintBoundControl& boundCtrl, const r3dPoint3D &pnt, int opType, int layerIdx, float val, float radius, const float hardness );
	void EndLayerBrush_G();

	void OptimizeLayerMasks();

	void StartEraseAllBrush();
	void ApplyEraseAllBrush(const r3dTerrainPaintBoundControl& boundCtrl, const r3dPoint3D &pnt, const float val, const float radius, const float hardness );
	void EndEraseAllBrush();

	void StartColorBrush();
	void ApplyColorBrush( const r3dTerrainPaintBoundControl& boundCtrl, const r3dPoint3D &pnt, const r3dColor &dwColor, const float strength, const float radius, const float hardness );
	void EndColorBrush();

	int ImportHeight( const char* path, float scaleY, float offsetY, int cellOffsetX, int cellOffsetZ );
	int ExportHeight( const char* path, int cellOffsetX, int cellOffsetZ, int sizeX, int sizeZ );

	void RecalcTileCoordBorders( const TileCoords& tcoords );
	void RecalcAllMasksMipMaps();
	void RebuildLayerInfo();

	void RecalcAllHeightMipMaps();

	void UpdateLayerMaskMipFromTemps( int tx, int tz, int maskId, TileCoords* oTileCoords );
	void UpdateLayerMaskMipFromLevelBelow( int tx, int tz, int L, int maskId );

	int ImportLayer( const char* path, int layerIndex, int cellOffsetX, int cellOffsetZ );
	int ImportLayer_RawTiles( const char* path, int layerIndex );
	int ExportLayer( const char* path, int layerIndex, int cellOffsetX, int cellOffsetZ, int texSizeX, int texSizeZ );
	int ExportLayer_RawTiles( const char* path, int layerIndex );

	int SwapLayers( int layerIndex0, int layerIndex1 );

	void RecalcTileHeightMipChain( int tileX, int tileZ, UShortsPtrArr& datas );

	void UpdateHeightRect( const RECT& rc );
	void RecalcNormalEdges();

	void			BeginUndoRecord				( const char * title, UndoAction_e eAction );
	void			EndUndoRecord				();
	int				IsUndoRecord				() const { return m_InUndoRecord; }
	IUndoItem*		GetUndoRecord				() const { return m_UndoItem; }

	int IsHeightDirty() const;
	int GetMaskCount() const;

	INT64 MakeCachedTileKey( int X, int Z, int L, int MaskID );

	void CreateTempTextures();

	void DeleteLayer( int lidx );

	void FinishTasksWithReporting();


private:
	int			m_PaintLayerIdx;

	Floats		m_FloatHeights;
	Floats		m_TempFloatHeights;

	enum
	{
		ADJ_00,
		ADJ_10,
		ADJ_20,
		ADJ_01,
		ADJ_21,
		ADJ_02,
		ADJ_12,
		ADJ_22,
		ADJ_COUNT
	};

	Floats		m_TempFloatHeightsAdj[ ADJ_COUNT ];

	Vectors		m_TempVectors0;
	Vectors		m_TempVectors1;

	Shorts		m_ShortHeights;
	UShorts		m_TempUShorts;
	UShorts		m_TempUShorts2;
	UShorts		m_TempUShorts3;

	Bytes		m_TempBytes;

	IUndoItem*	m_UndoItem;

	int			m_HeightDirty;

	int			m_InUndoRecord;

	UnpackedMaskTiles	m_UnpackedMaskTilesCache;
	UnpackedHeightTiles	m_UnpackedHeightTilesCache;

	IDirect3DTexture9*	m_TempUnpackedMaskTexture;
	IDirect3DTexture9*	m_TempPackedMaskTexture;

	Bytes				m_TempNormalData;

} extern * g_pTerrain3Editor;

//------------------------------------------------------------------------

class CHeightChanged3 : public IUndoItem
{
public:
#pragma pack(push,1)
	struct UndoHeight_t
	{
		uint16_t	PrevHeight;
		uint16_t	CurrHeight;
		uint16_t	X;
		uint16_t	Z;
	};
#pragma pack(pop)

private:
	static const UndoAction_e	ms_eActionID = UA_TERRAIN3_HEIGHT;

	typedef r3dTL::TArray< UndoHeight_t > UndoArray;
	UndoArray			DataArr;

	RECT				m_rc;

public:

	void				Release ();
	UndoAction_e		GetActionID ();

	void				UndoRedo( bool redo );

	void				Undo ();
	void				Redo ();

	void				AddData ( const UndoHeight_t & data );
	void				AddRectUpdate ( const RECT &rc );

	CHeightChanged3 ();

	static IUndoItem*	CreateUndoItem ();
	static void			Register();
};

//------------------------------------------------------------------------

class CLayerMaskPaint3 : public IUndoItem
{
public:

	struct PaintData_t
	{
		PaintData_t();
		~PaintData_t();

		uint16_t	*	pData; // old data + new data
		RECT			rc;
		int				L;
	};

	int	LayerIdx;

private:

	static const UndoAction_e		ms_eActionID = UA_TERRAIN3_MASK_PAINT;
	r3dTL::TArray< PaintData_t >	m_pData;

	RECT				m_rc;

public:

	void				Release			();
	UndoAction_e		GetActionID		();

	void				UndoRedo		( bool redo );

	void				Undo			();
	void				Redo			();

	void				AddData			( const PaintData_t & data );
	void				AddRectUpdate	( const RECT &rc );

	CLayerMaskPaint3();

	static IUndoItem * CreateUndoItem	();
	static void Register();

};

//------------------------------------------------------------------------

class CLayerMaskEraseAll3 : public IUndoItem
{
public:

	struct PaintData_t
	{
		PaintData_t();

		r3dTL::TArray< r3dTL::TArray< uint16_t > >	masks; // old data + new data
		RECT										rc;
	};

private:

	static const UndoAction_e		ms_eActionID = UA_TERRAIN2_MASK_ERASEALL;
	r3dTL::TArray< PaintData_t >	m_pData;

	int					m_MaskCount;

	RECT				m_rc;

public:

	void				Release			();
	UndoAction_e		GetActionID		();

	void				UndoRedo		( bool redo );

	void				Undo			();
	void				Redo			();

	void				AddData			( const PaintData_t & data );
	void				AddRectUpdate	( const RECT &rc );

	CLayerMaskEraseAll3();

	static IUndoItem * CreateUndoItem	();
	static void Register();

};

//------------------------------------------------------------------------

class CLayerColorPaint3 : public IUndoItem
{
public:

	struct UndoColor_t
	{
		int			nIndex;
		uint32_t	dwPrevColor;
		uint32_t	dwCurrColor;
	};


private:

	static const UndoAction_e	ms_eActionID = UA_TERRAIN2_COLOR_PAINT;
	r3dTL::TArray< UndoColor_t >	m_pData;

	RECT				m_rc;

	int					m_CellCount;

public:

	void				Release			();
	UndoAction_e		GetActionID		();

	void				UndoRedo		( bool redo );

	void				Undo			();
	void				Redo			();

	void				AddData			( const UndoColor_t & data );
	void				AddRectUpdate	( const RECT &rc );

	CLayerColorPaint3	();

	static IUndoItem * CreateUndoItem	();

	static void Register();
};

#endif