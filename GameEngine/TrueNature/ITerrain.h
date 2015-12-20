#pragma once

struct MaterialType ;

struct r3dTerrainDesc
{
	int			LayerCount ;

	float		XSize ;
	float		ZSize ;

	float		MinHeight ;
	float		MaxHeight ;

	float		CellSize ;

	int			CellCountX ;
	int			CellCountZ ;

	int			MaskResolutionU ;
	int			MaskResolutionV ;

	int			SplatResolutionU ;
	int			SplatResolutionV ;

	int			TileCountX ;
	int			TileCountZ ;

	int			CellCountPerTile ;

	float		WorldSize ;

	// downscaled diffuse texture for grass modulation
	r3dTexture*	OrthoDiffuseTex ;

	r3dTerrainDesc() ;

	void Sync() ;
};

class r3dITerrain
{
public:
	static float LoadingProgress;

public:
	r3dITerrain();
	virtual ~r3dITerrain();

public:
	const r3dTerrainDesc&		GetDesc() const ;
	int							GetDecalID( const r3dPoint3D& pnt, uint32_t sourceHash ) ;
	void						SetOrthoDiffuseTextureDirty();
	void						UpdateOrthoDiffuseTexture();

	virtual float				GetHeight( int x, int z )			= 0 ;
	virtual float				GetHeight( const r3dPoint3D& pos )	= 0 ;

	virtual r3dPoint3D			GetNormal( const r3dPoint3D& pos )	= 0 ;
	virtual void				GetNormalTangentBitangent(const r3dPoint3D &pos, r3dPoint3D *outNorm, r3dPoint3D *outTangent, r3dPoint3D *outBitangent) = 0;

	virtual void				GetHeightRange( float* oMinHeight, float* oMaxHeight, r3dPoint2D start, r3dPoint2D end ) = 0 ;

	virtual const MaterialType*	GetMaterialType( const r3dPoint3D& pnt ) = 0 ;

	virtual bool				IsLoaded() = 0 ;

	virtual bool				PrepareOrthographicTerrainRender( int renderTargetWidth, int renderTargetHeight ) = 0 ;

	virtual bool				DrawOrthographicTerrain( const r3dCamera& Cam, bool UseZ ) = 0 ;

	virtual void				ReloadTextures() = 0 ;

	virtual r3dTexture*			GetDominantTexture( const r3dPoint3D &pos ) = 0 ;

	virtual PxHeightFieldGeometry GetHFShape() const = 0;

	virtual r3dTexture*			GetNormalTexture() const = 0;

	virtual int					IsPosWithinPreciseHeights( const r3dPoint3D& pos ) const;

protected:
	void SetDesc( const r3dTerrainDesc& desc ) ;
	
	bool DrawOrthographicDiffuseTexture( int DownScale ) ;

	void DrawOrthographicDiffuseTexture_MainThread( int DownScale ) ;

	friend void DoDrawOrthoTerra( void* params ) ;

private:
	r3dTerrainDesc	m_Desc ;
	int				m_OrthoDiffuseTexDirty ;
};

//------------------------------------------------------------------------

R3D_FORCEINLINE
const r3dTerrainDesc&
r3dITerrain::GetDesc() const
{
	return m_Desc ;
}

//------------------------------------------------------------------------

struct r3dTerrainPaintBoundControl
{
	int		PaintUseBounds;
	int		InvertBounds;

	float	PaintMinHeight;
	float	PaintMaxHeight;
	float	PaintMinSlope;
	float	PaintMaxSlope;

	r3dTerrainPaintBoundControl() ;
};

//------------------------------------------------------------------------

extern	bool		terra_IsValidPaintBounds( const r3dTerrainPaintBoundControl& ctrl, const r3dPoint3D &v, float fSize, float* oFactor );
extern	float		terra_GetH(const r3dPoint3D &P);
extern	BOOL		terra_FindIntersection(const r3dPoint3D &vFrom, const r3dPoint3D &vTo, r3dPoint3D &colpos, int iterations);
extern	void		terra_GetMinMaxTerraHeight( float miX, float miZ, float maX, float maZ, float* oMiH, float* oMaH ) ;
extern	int			terra_IsWithinPreciseHeightmap( const r3dPoint3D &vPos );
extern	int			terra_IsWithinPreciseHeightmap( const r3dBoundBox &bb );

//------------------------------------------------------------------------

extern	r3dITerrain	*Terrain;