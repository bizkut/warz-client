#pragma once

#include "../UndoHistory/UndoHistory.h"

#if !defined(FINAL_BUILD) && !defined(WO_SERVER)

class Terrain2Editor
{
public:
	friend class CHeightChanged2;
	friend class CLayerMaskPaint2;
	friend class CLayerColorPaint2;
	friend class CLayerMaskEraseAll2;

	typedef r3dTL::TArray< float > Floats;
	typedef r3dTL::TArray< PxI16 > Shorts;
	typedef r3dTL::TArray< r3dTexture* > PaintMasks;
	typedef r3dTL::TArray< uint32_t > UInts;
	typedef r3dTL::TArray< r3dPoint3D > Vectors;

	struct NoiseParams
	{
		r3dPerlinNoise	noise;

		float			maxHeight;
		float			minHeight;
		float			heightApplySpeed;
		int				heightRelative;

		bool		painting_;

		struct cell_s
		{
			float		org;		// original terrain height
			float		trg;		// calculated noisy terrain
			float		mc;		// current morph coef
		};

		cell_s*		noiseData;
		int		Width;
		int		Height;

		NoiseParams();
		~NoiseParams();

		void	DrawNoiseParams( float& SliderX, float& SliderY );
		void	ResetCache();
		float	GetNoise( int x, int y );
		void	Apply( const r3dPoint3D& pos, float radius, float hardiness );

	} noiseParams;

public:
	Terrain2Editor();
	~Terrain2Editor();

public:
	void LoadHeightsFromTerrain();
	void SaveHeightsToTerrain();

	void FinalizeHeightEditing();

	void LoadColorsFromTerrain();

	int AreHeightsLoaded() const;

	void ApplyHeightBrush(const r3dPoint3D &pnt, const float strength, const float radius, const float hardness);
	void ApplyHeightLevel(const r3dPoint3D &pnt, const float H, const float strength, const float radius, const float hardness);
	void ApplyHeightSmooth(const r3dPoint3D &pnt, const float radius);
	void ApplyHeightErosion(const r3dPoint3D &pnt, const float strength, const float radius, const float hardness);
	void ApplyHeightNoise(const r3dPoint3D &pnt, const float radius, const float hardness);
	void ApplyHeightRamp(const r3dPoint3D& rampStart, const r3dPoint3D& rampEnd, const float rampWidthOuter, const float rampWidthInner);

	void StartLayerBrush( int layerIdx );
	void ApplyLayerBrush(const r3dTerrainPaintBoundControl& boundCtrl, const r3dPoint3D &pnt, int opType, int layerIdx, float val, const float radius, const float hardness );
	void EndLayerBrush();

	void StartEraseAllBrush();
	void ApplyEraseAllBrush(const r3dTerrainPaintBoundControl& boundCtrl, const r3dPoint3D &pnt, const float val, const float radius, const float hardness );
	void EndEraseAllBrush();

	void StartColorBrush();
	void ApplyColorBrush( const r3dTerrainPaintBoundControl& boundCtrl, const r3dPoint3D &pnt, const r3dColor &dwColor, const float strength, const float radius, const float hardness );
	void EndColorBrush();

	int ImportHeight( const char* path, float scale, float offset );
	int ExportHeight( const char* path );

	void UpdateHeightRect( const RECT& rc );
	void UpdateTerrainHeightRect( const RECT& rc );

	r3dTexture*		GetPaintMask( int idx );

	void			BeginUndoRecord				( const char * title, UndoAction_e eAction );
	void			EndUndoRecord				();
	bool			IsUndoRecord				() const { return m_UndoItem != NULL; }
	IUndoItem*		GetUndoRecord				() const { return m_UndoItem; }

	int IsHeightDirty() const;
	int GetMaskCount() const;

private:
	int			m_PaintLayerIdx;
	PaintMasks	m_PaintLayerMasks;

	r3dTexture*	m_ColorTex;

	Floats		m_FloatHeights;
	Floats		m_TempFloatHeights;

	Vectors		m_TempVectors0;
	Vectors		m_TempVectors1;

	Shorts		m_ShortHeights;

	IUndoItem*	m_UndoItem;

	int			m_HeightDirty;

} extern * g_pTerrain2Editor;

//------------------------------------------------------------------------

class CHeightChanged2 : public IUndoItem
{
public:

	struct UndoHeight_t
	{
		int			nIndex;
		float		fPrevHeight;
		float		fCurrHeight;
	};

private:

	static const UndoAction_e	ms_eActionID = UA_TERRAIN2_HEIGHT;


	r3dTL::TArray< UndoHeight_t >	m_pData;

	RECT				m_rc;

public:

	void				Release ();
	UndoAction_e		GetActionID ();

	void				Undo ();
	void				Redo ();

	void				AddData ( const UndoHeight_t & data );
	void				AddRectUpdate ( const RECT &rc );

	CHeightChanged2 ();

	static IUndoItem*	CreateUndoItem ();
	static void			Register();
};

//------------------------------------------------------------------------

class CLayerMaskPaint2 : public IUndoItem
{
public:

	struct PaintData_t
	{
		PaintData_t();

		uint16_t	*	pData; // old data + new data
		RECT			rc;
		int				LayerIdx;
	};

private:

	static const UndoAction_e		ms_eActionID = UA_TERRAIN2_MASK_PAINT;
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

	CLayerMaskPaint2();

	static IUndoItem * CreateUndoItem	();
	static void Register();

};

//------------------------------------------------------------------------

class CLayerMaskEraseAll2 : public IUndoItem
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

	CLayerMaskEraseAll2();

	static IUndoItem * CreateUndoItem	();
	static void Register();

};

//------------------------------------------------------------------------

class CLayerColorPaint2 : public IUndoItem
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

	CLayerColorPaint2	();

	static IUndoItem * CreateUndoItem	();

	static void Register();
};

//------------------------------------------------------------------------

class CTerrain2DestroyLayer : public IUndoItem
{
public:
	typedef r3dTL::TArray< float > Floats;

public:
	CTerrain2DestroyLayer();

public:
	void				Release			();
	UndoAction_e		GetActionID		();

	void				Undo			();
	void				Redo			();

	void				SetData			( int layerIdx );

	static IUndoItem *	CreateUndoItem	();

	static void			Register();

private:
	static const UndoAction_e	ms_eActionID = UA_TERRAIN2_DESTROY_LAYER;
	r3dTerrainLayer				m_TerrainLayer;
	Floats						m_TerrainLayerData;
	int							m_TerrainLayerIdx;

};

//------------------------------------------------------------------------

class CTerrain2InsertLayer : public IUndoItem
{
public:
	typedef r3dTL::TArray< float > Floats;

public:
	CTerrain2InsertLayer();

public:
	void				Release			();
	UndoAction_e		GetActionID		();

	void				Undo			();
	void				Redo			();

	void				SetData			( int layerIdx );

	static IUndoItem *	CreateUndoItem	();

	static void			Register();

private:
	static const UndoAction_e	ms_eActionID = UA_TERRAIN2_INSERT_LAYER;
	int							m_TerrainLayerIdx;

};

#endif