#ifndef	__R3D_RENDER_H
#define	__R3D_RENDER_H

#include "r3d.h"

#include "r3dColor.h"
#include "pShader.h"
#include "VShader.h"

class r3dMaterial;

struct R3D_SCREEN_VERTEX;
struct R3D_DEBUG_VERTEX;

// bitwise flags for SetRenderingMode()
#define R3D_BLEND_NONE          0
#define R3D_BLEND_PUSH          (1<<30)
#define R3D_BLEND_POP           (1<<31)

#define R3D_BLEND_NZ            (1<<1)      // No Z Check: zero flag
#define R3D_BLEND_ZC            (1<<2)      // Z Check Enabled
#define R3D_BLEND_ZW            (1<<3)      // Z Write Enabled

#define R3D_BLEND_NOALPHA       (1<<10)     // Alpha Disabled
#define R3D_BLEND_ALPHA         (1<<11)     // SrcAlpha->InvAlpha.
#define R3D_BLEND_ADD           (1<<12)     // Add
#define R3D_BLEND_SUB           (1<<13)     // Sub
#define R3D_BLEND_MODULATE      (1<<14)     // Lightmap
#define R3D_BLEND_ADDMODULATE   (1<<15)     // Lightmap
#define R3D_BLEND_COLOR         (1<<16)
#define R3D_BLEND_MAX           (1<<17)     // BlendOp = MAX, Src and Dest ONE ONE
#define R3D_BLEND_MIN           (1<<18)     // BlendOp = MIN, Src and Dest ONE ONE

// few predefined states

#define R3D_BLEND_COPY_NZ	 (R3D_BLEND_ALPHA | R3D_BLEND_NZ)
#define R3D_BLEND_COPY_ZCW	 (R3D_BLEND_ALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW)
#define R3D_BLEND_ADD_ZC	 (R3D_BLEND_ADD   | R3D_BLEND_ZC)
#define R3D_BLEND_SUB_ZC	 (R3D_BLEND_SUB   | R3D_BLEND_ZC)
#define R3D_BLEND_LIGHTMAP	 (R3D_BLEND_MODULATE | R3D_BLEND_ZC)
#define R3D_BLEND_LIGHTMAP2X	 (R3D_BLEND_ADDMODULATE | R3D_BLEND_ZC)

//
// Shading
//
#define CLRMODE_CONSTANT	0	// using ConstantRGB as color
#define CLRMODE_VERTICES	1	// using Vertices color
#define CLRMODE_SHADING		2	// using Vertices color as intensity of Material Color

enum ShadowPassTypeEnum
{
	SPT_ORTHO				= 0,
	SPT_ORTHO_WARPED		= 1,
	SPT_PERSP				= 2,
	SPT_PERSP_PARABOLOID	= 3
};

enum StereoEyeEnum
{
	R3D_STEREO_EYE_LEFT,
	R3D_STEREO_EYE_RIGHT,
	R3D_STEREO_EYE_MONO
};

enum VisibilityInfoEnum
{
	VI_OUTSIDE = 0,
	VI_INSIDE = 1,
	VI_INTERSECTING = 2
};

const D3DFORMAT NULL_RT_FORMAT = static_cast<D3DFORMAT>(MAKEFOURCC('N', 'U', 'L', 'L'));
const D3DFORMAT INTZ_FORMAT = static_cast<D3DFORMAT>(MAKEFOURCC('I', 'N', 'T', 'Z'));
const D3DFORMAT RAWZ_FORMAT = static_cast<D3DFORMAT>(MAKEFOURCC('R', 'A', 'W', 'Z'));
const D3DFORMAT RESZ_FORMAT = static_cast<D3DFORMAT>(MAKEFOURCC('R', 'E', 'S', 'Z'));
const D3DFORMAT DF24_FORMAT = static_cast<D3DFORMAT>(MAKEFOURCC('D', 'F', '2', '4'));

typedef r3dTL::TArray<D3DDISPLAYMODE> ModesArray;

extern DWORD gMainThreadID ;

#define R3D_ENSURE_MAIN_THREAD() r3d_assert( gMainThreadID == GetCurrentThreadId() )
#define R3D_IS_MAIN_THREAD() ( gMainThreadID == GetCurrentThreadId() )

enum
{
	SHADETEXT_BLUE					= 0,
	SHADETEXT_DEFAULT_NORMAL		= 1,
	SHADETEXT_WHITE					= 2,
	SHADETEXT_BLACK					= 3,
	SHADETEXT_MISSING				= 4,
	SHADETEXT_NOISE					= 5,
	SHADETEXT_SUNFLAREMASK			= 6,
	SHADETEXT_DEFAULT_METALNESS		= 7,
	SHADETEXT_GREY					= 8,
	SHADETEXT_DENSITY_CHECKER		= 9,
	SHADETEXT_DEFAULT_DISTORTION	= 10,
	SHADETEXT_WINTER_DIFFUSE		= 11,
	SHADETEXT_WINTER_SPECULAR		= 12,
	SHADETEXT_WINTER_NORMAL			= 13,

};

extern 	r3dTexture*	__r3dShadeTexture[16];

static const float r_wall_hack_anti_cheat_z = 0.99950f;
enum r3dZRange
{
	R3D_ZRANGE_FIRST_PERSON,
	R3D_ZRANGE_WORLD,
	R3D_ZRANGE_WHOLE,
	R3D_ZRANGE_ANTICHEAT
};

enum r3dFrustumType
{
	R3D_FRUSTUM_NORMAL,
	R3D_FRUSTUM_PARABOLOID,
};

enum
{
	R3D_NUM_OCCLUSION_QUERIES = 1536
};

struct r3dFogState
{
	int			Enabled;
	r3dColor	Color;
	float		Start;
	float		End;
};


class r3dGPUStats
{
private:
	void AddVMem( int val ) ;

public:
	void Reset();

	void ResetVMemStats();

	void AddTexMem( int val );
	void AddPlayerTexMem( int val );
	void AddUITexMem( int val );
	void AddBufferMem( int val );
	void AddPlayerBufferMem( int val );
	void AddRenderTargetMem( int val );
	void AddTerrainBufferDXMem( int val );
	void AddGrassBufferDXMem( int val );
	void AddOtherBufferDXMem( int val );
	void AddUIBufferMem( int val );
	void AddNumMaterialChanges( int val );
	void AddNumModesChanges( int val );
	void AddNumTextureChanges( int val );
	void AddNumTrianglesRendered( int val );
	void AddAverageStripLength( int val );
	void AddNumDraws( int val );
	void AddNumTerrainDraws( int val );
	void AddNumTerrainTris( int val );
	void AddNumOcclusionQueries( int val );
	void AddNumParticlesRendered( int val );
	void AddNumLocks( int val );
	void AddNumLocksDiscard( int val );
	void AddNumLocksNooverwrite( int val );
	void AddBytesLocked( int val );
	void AddNumVisGridOcclusions( int val );

	void AddNumDoubleShadowDips( int val );
	void AddNumShadowDips( int val );

	int GetTexMem() const;
	int GetPlayerTexMem() const;
	int GetUITexMem() const;
	int GetBufferMem() const;
	int GetPlayerBufferMem() const;
	int GetUIBufferMem() const;
	int GetRenderTargetMem() const;
	int GetTerrainBufferDXMem() const;
	int GetGrassBufferDXMem() const;
	int GetOtherBufferDXMem() const;
	int GetVMemTotal() const;
	int GetNumMaterialChanges() const;
	int GetNumModesChanges() const;
	int GetNumTextureChanges() const;
	int GetNumTrianglesRendered() const;
	int GetAverageStripLength() const;
	int GetNumDraws() const;
	int GetNumTerrainDraws() const;
	int GetNumTerrainTris() const;
	int GetNumOcclusionQueries() const;
	int GetNumParticlesRendered() const;
	int GetNumLocks() const;
	int GetNumLocksDiscard() const;
	int GetNumLocksNooverwrite() const;
	int GetBytesLocked() const;
	int GetNumVisGridOcclusions() const;

	int GetNumDoubleShadowDips() const;
	int GetNumShadowDips() const;

private:
	volatile LONG	mTexMem;
	volatile LONG	mPlayerTexMem;
	volatile LONG	mUITexMem;
	volatile LONG	mBufferMem;
	volatile LONG	mUIBufferMem;
	volatile LONG	mPlayerBufferMem;
	volatile LONG	mRenderTargetMem;
	volatile LONG	mTerrainBufferDXMem;
	volatile LONG	mGrassBufferDXMem;
	volatile LONG	mOtherBufferDXMem;
	volatile LONG	mVMemTotal;

	int		mNumMaterialChanges;
	int		mNumModesChanges;
	int		mNumTextureChanges;
	int		mNumTrianglesRendered;
	int		mAverageStripLength;
	int		mNumDraws;
	int		mNumTerrainDraws;
	int		mNumTerrainTris;
	int		mNumOcclusionQueries;
	int		mNumParticlesRendered;
	int		mNumLocks;
	int		mNumLocksDiscard;
	int		mNumLocksNooverwrite;
	int		mBytesLocked;
	int		mNumVisGridOcclusions;
	int		mNumDoubleShadowDips;
	int		mNumShadowDips;
};

R3D_FORCEINLINE void r3dGPUStats::AddNumMaterialChanges( int val )
{
	mNumMaterialChanges += val ;
}

R3D_FORCEINLINE void r3dGPUStats::AddNumModesChanges( int val )
{
	mNumModesChanges += val ;
}

R3D_FORCEINLINE void r3dGPUStats::AddNumTextureChanges( int val )
{
	mNumTextureChanges += val ;
}

R3D_FORCEINLINE void r3dGPUStats::AddNumTrianglesRendered( int val )
{
	mNumTrianglesRendered += val ;
}

R3D_FORCEINLINE void r3dGPUStats::AddAverageStripLength( int val )
{
	mAverageStripLength += val ;
	mAverageStripLength /= 2;
}

R3D_FORCEINLINE void r3dGPUStats::AddNumDraws( int val )
{
	mNumDraws += val ;
}

R3D_FORCEINLINE void r3dGPUStats::AddNumTerrainDraws( int val )
{
	mNumTerrainDraws += val ;
}

R3D_FORCEINLINE void r3dGPUStats::AddNumTerrainTris( int val )
{
	mNumTerrainTris += val ;
}

R3D_FORCEINLINE void r3dGPUStats::AddNumOcclusionQueries( int val )
{
	mNumOcclusionQueries += val ;
}

R3D_FORCEINLINE void r3dGPUStats::AddNumParticlesRendered( int val )
{
	mNumParticlesRendered += val ;
}

R3D_FORCEINLINE void r3dGPUStats::AddNumLocks( int val )
{
	mNumLocks += val ;
}

R3D_FORCEINLINE void r3dGPUStats::AddNumLocksDiscard( int val )
{
	mNumLocksDiscard += val ;
}

R3D_FORCEINLINE void r3dGPUStats::AddNumLocksNooverwrite( int val )
{
	mNumLocksNooverwrite += val ;
}

R3D_FORCEINLINE void r3dGPUStats::AddBytesLocked( int val )
{
	mBytesLocked += val ;
}

R3D_FORCEINLINE void r3dGPUStats::AddNumVisGridOcclusions( int val )
{
	mNumVisGridOcclusions += val ;
}

R3D_FORCEINLINE void r3dGPUStats::AddNumDoubleShadowDips( int val )
{
	mNumDoubleShadowDips += val;
}

R3D_FORCEINLINE void r3dGPUStats::AddNumShadowDips( int val )
{
	mNumShadowDips += val;
}

R3D_FORCEINLINE int r3dGPUStats::GetTexMem() const 
{
	return mTexMem ;
}

R3D_FORCEINLINE int r3dGPUStats::GetPlayerTexMem() const
{
	return mPlayerTexMem ;
}

R3D_FORCEINLINE int r3dGPUStats::GetUITexMem() const
{
	return mUITexMem ;
}

R3D_FORCEINLINE int r3dGPUStats::GetBufferMem() const 
{
	return mBufferMem ;
}

R3D_FORCEINLINE int r3dGPUStats::GetUIBufferMem() const
{
	return mUIBufferMem;
}

R3D_FORCEINLINE int r3dGPUStats::GetPlayerBufferMem() const
{
	return mPlayerBufferMem ;
}

R3D_FORCEINLINE int r3dGPUStats::GetRenderTargetMem() const 
{
	return mRenderTargetMem ;
}

R3D_FORCEINLINE int r3dGPUStats::GetTerrainBufferDXMem() const 
{
	return mTerrainBufferDXMem ;
}

R3D_FORCEINLINE int r3dGPUStats::GetGrassBufferDXMem() const 
{
	return mGrassBufferDXMem ;
}

R3D_FORCEINLINE int r3dGPUStats::GetOtherBufferDXMem() const 
{
	return mOtherBufferDXMem ;
}

R3D_FORCEINLINE int r3dGPUStats::GetVMemTotal() const 
{
	return mVMemTotal ;
}

R3D_FORCEINLINE int r3dGPUStats::GetNumMaterialChanges() const 
{
	return mNumMaterialChanges ;
}

R3D_FORCEINLINE int r3dGPUStats::GetNumModesChanges() const 
{
	return mNumModesChanges ;
}

R3D_FORCEINLINE int r3dGPUStats::GetNumTextureChanges() const 
{
	return mNumTextureChanges ;
}

R3D_FORCEINLINE int r3dGPUStats::GetNumTrianglesRendered() const 
{
	return mNumTrianglesRendered ;
}

R3D_FORCEINLINE int r3dGPUStats::GetAverageStripLength() const 
{
	return mAverageStripLength ;
}

R3D_FORCEINLINE int r3dGPUStats::GetNumDraws() const 
{
	return mNumDraws ;
}

R3D_FORCEINLINE int r3dGPUStats::GetNumTerrainDraws() const 
{
	return mNumTerrainDraws ;
}

R3D_FORCEINLINE int r3dGPUStats::GetNumTerrainTris() const 
{
	return mNumTerrainTris ;
}

R3D_FORCEINLINE int r3dGPUStats::GetNumOcclusionQueries() const 
{
	return mNumOcclusionQueries ;
}

R3D_FORCEINLINE int r3dGPUStats::GetNumParticlesRendered() const 
{
	return mNumParticlesRendered ;
}

R3D_FORCEINLINE int r3dGPUStats::GetNumLocks() const 
{
	return mNumLocks ;
}

R3D_FORCEINLINE int r3dGPUStats::GetNumLocksDiscard() const 
{
	return mNumLocksDiscard ;
}

R3D_FORCEINLINE int r3dGPUStats::GetNumLocksNooverwrite() const 
{
	return mNumLocksNooverwrite ;
}

R3D_FORCEINLINE int r3dGPUStats::GetBytesLocked() const 
{
	return mBytesLocked ;
}

R3D_FORCEINLINE int r3dGPUStats::GetNumVisGridOcclusions() const
{
	return mNumVisGridOcclusions ;
}

R3D_FORCEINLINE int r3dGPUStats::GetNumDoubleShadowDips() const
{
	return mNumDoubleShadowDips;
}

R3D_FORCEINLINE int r3dGPUStats::GetNumShadowDips() const
{
	return mNumShadowDips;
}

struct r3dDisplayResolution
{
	int Width ;
	int Height ;
};

typedef r3dTL::TArray< r3dDisplayResolution > DisplayResolutions ;

#ifdef FINAL_BUILD
#define R3D_DEBUG_MULTITHREADING 0
#else
#define R3D_DEBUG_MULTITHREADING 1
#endif

class r3dIResource;
typedef r3dTL::TArray <D3DXMACRO> ShaderMacros;

class r3dRenderLayer ;
extern 	r3dRenderLayer*	r3dRenderer ;

// Main 3D Renderer Class
class r3dRenderLayer
{
	friend class r3dIResource;
	friend void ProcessItem( const struct DeviceQueueItem& item );
public:
	typedef r3dTL::TArray< r3dTexture* > TextureArr;
	typedef r3dTL::TArray< r3dThumbnailTexture* > ThumbnailTexArr;

	enum
	{
		MAX_RENDER_TARGETS = 4
	};

	static const D3DFORMAT BACK_BUFFER_FORMAT	= D3DFMT_X8R8G8B8 ;
	static const D3DFORMAT DISPLAY_FORMAT_32	= D3DFMT_X8R8G8B8 ;
	static const D3DFORMAT DISPLAY_FORMAT_16	= D3DFMT_R5G6B5 ;

	int     bInited;
	int     Version;
	HWND    HLibWin;

	D3DDEVTYPE	DeviceType;	// D3DDEVTYPE_HAL or SW

	IDirectInput8	*pdi;
	IDirect3D9     	*pd3d;

	D3DPRESENT_PARAMETERS	d3dpp;

#if R3D_DEBUG_MULTITHREADING
	class DeviceWrapper
	{
	public:
		DeviceWrapper();

		IDirect3DDevice9* operator->() const;
		operator IDirect3DDevice9*() const;

		void Set( IDirect3DDevice9* dev);

	private:
		IDirect3DDevice9	*mDev;
	} pd3ddev;

#else
	IDirect3DDevice9		*pd3ddev;
#endif

	void SetD3DDev( IDirect3DDevice9* dev );

	//	IDirect3DSurface9 	*RTBuffer; 


	D3DCAPS9	d3dCaps;

	// camera

	r3dPoint3D	CameraPosition;

	r3dPoint3D	DistanceCullRefPos;

	D3DXMATRIXA16	ViewMatrix;
	D3DXMATRIXA16	InvViewMatrix;
	D3DXMATRIXA16	ProjMatrix;
	D3DXMATRIXA16	ViewProjMatrix;
	D3DXMATRIXA16	InvProjMatrix;

	D3DXMATRIXA16	ViewMatrix_Localized;
	D3DXMATRIXA16	InvViewMatrix_Localized;
	D3DXMATRIXA16	ViewProjMatrix_Localized;

	r3dPoint3D	LocalizationPos;

	D3DXVECTOR3 FrustumCorners[8];    // corners of the view frustum
	D3DXPLANE 	FrustumPlanes[6];    // planes of the view frustum

	float		NearClip;
	float		FarClip;

	r3dTL::TArray< r3dCamera > m_CameraStack;

	void		PushCamera			( const r3dCamera &cam );
	void		PopCamera			();

	// textures data
	r3dTexture*		FirstTexture;
	ThumbnailTexArr	ThumbnailTextures;
	int				LastThumbUpdated;

	int			bFullScreen;

	int			CurrentBPP;
	float		ScreenW;
	float		ScreenH;
	float		ScreenW2;
	float		ScreenH2;

	float 		ViewX, ViewY, ViewW, ViewH, ViewMinZ, ViewMaxZ ;

	r3dFogState	Fog;

	r3dColor	AmbientColor;

	r3dGPUStats	Stats;

	char		VertexShaderProfileName[16];
	char		PixelShaderProfileName[16];

	static const int NUM_SHADERS_SIZE = 1024 + 512;
	int		NumPixelShaders;
	r3dPixelShader  PixelShaders[NUM_SHADERS_SIZE];

	int		NumVertexShaders;
	r3dVertexShader  VertexShaders[NUM_SHADERS_SIZE];

	int		CurrentPixelShaderID;
	int		CurrentVertexShaderID;

	int		SupportsVertexTextureFetch;
	int		SupportsR32FBlending;
	int		SupportsOQ;
	int		SupportsEventQueries;
	int		SupportsStampQueries;
	int		SupportsNULLRenderTarget;
	int		SupportsHWShadowMapping;
	int		SupportsRESZResolve;
	int		SupportsINTZTextures;

	int		NVApiActive ;
	int		NVApiStereoMode ;
	void*	NVApiStereoHandle ;

	int		IsStereoActive() ;
	void	SetPresentEye(StereoEyeEnum eye);
	StereoEyeEnum GetPresentEye() const { return PresentEye; }
	
	StereoEyeEnum	ActiveStereoEye ;

	int		AllowNullViewport;
	int		ForceBlackPixelShader;
	int		ForceBlackPixelShaderId;

	enum ZDirEnum
	{
		ZDIR_NORMAL,
		ZDIR_INVERSED
	}		ZDir;

	volatile int		DeviceAvailable ;

	int					DoubleDepthShadowPath;

	ShadowPassTypeEnum	ShadowPassType;
	D3DCULL				DefaultCullMode;
	D3DCULL				CurrentCullMode;
	uint8_t				ShadowSliceIndexBit;

	void	SetDeviceLost();

	void	EnumerateRenderTargets( r3dTL::TArray< class r3dScreenBuffer* >* oSBArray ) ;	
	void	RegisterScreenBuffer( r3dScreenBuffer* sb ) ;
	void	UnregisterScreenBuffer( r3dScreenBuffer* sb ) ;

	r3dgfxVector(LPDIRECT3DVERTEXDECLARATION9) VertexDecls;
private:
	r3dTL::TArray<r3dIResource*>	resources_;
	r3dTL::TArray<r3dScreenBuffer*>	ScreenBuffers_;

	volatile bool					deviceLost_;
	bool							allowShaderLoading_;

	r3dZRange						ZRange ;

	void		RegisterResource(r3dIResource* res);
	void		DeregisterResource(r3dIResource* res);

	StereoEyeEnum	PresentEye;

public:

	r3dRenderLayer();

	~r3dRenderLayer();

	// reset rendering layer as parameters in r3d->d3dpp describes
	bool	Reset();

	int     Init(HWND hWindow, const char* displayName);
	int     Close();
	void    FlushTextures();

#define R3DSetMode_Windowed	(1<<2)
	bool    AdjustWindowSize( int xRes, int yRes, int BPP, int isWindowed, D3DDISPLAYMODE& result );
	int		InitStereo() ;
	bool	DetectSLIorCrossfire() const;
	void	SetEye( StereoEyeEnum eye ) ;
	int     SetMode(int xRes, int yRes, int BPP, int flags, int renderPath);

	void	ApplyGammaRamp() ;

	// Start/End frame rendering placeholders

	void		SetZRange( r3dZRange range ) ;
	r3dZRange	GetZRange() const ;

	// NOTE : defined as SetViewport to prevent direct device viewport usage
	void		DoSetViewport ( float x, float y, float w, float h );

private:
	void		SetViewportInternal ( const D3DVIEWPORT9* viewport );
	void		SetViewportZRange( D3DVIEWPORT9* oViewport ) ;
	bool		SearchForModeWithOptimalRefreshRate(int w, int h, int bpp, D3DFORMAT neededFormat, D3DDISPLAYMODE &result);
	bool		FindCompatibleNVidiaStereoDisplayMode(const char *displayName, D3DDISPLAYMODE &inOutMode, const ModesArray &allModes);

public:
	void		DoGetViewport( D3DVIEWPORT9* oViewport );

	// NOTE : defined as SetStreamSourceFreq to prevent direct device viewport stream source config
	void		DoSetStreamSourceFreq( UINT streamNumber,UINT setting ) ;
	void		ResetViewport();

	void		ResetStats();
	void		StartRender(int bClear = 1, bool clearToWhite=false);
	void		EndRender( bool present = false );

	void FlushGPU();

	void		StartFrame();
	void		EndFrame();
	void		SetBackBufferViewport();
	void		GetBackBufferViewport( float* x, float* y, float* width, float* height );
	void		StartRenderSimple(int bClear = 1);
	void		EndRenderSimple( bool present = false );


	void		BuildViewMtx( const r3dCamera &Cam, D3DXMATRIX* oMtx, D3DXMATRIX* oLocalizedMtx );
	void		BuildPerspectiveMtx( const r3dCamera &cam, D3DXMATRIX* oMtx );
	void		SetCamera( const r3dCamera &cam, bool asDistanceCullReference );
	void		SetCameraEx(const D3DXMATRIX& view, const D3DXMATRIX& proj, float nearD, float farD, bool asDistanceCullReference);

	void		BuildMatrixPerspectiveFovLH( D3DXMATRIX *pOut, FLOAT fovy, FLOAT Aspect, FLOAT zn, FLOAT zf );
	void		BuildMatrixPerspectiveOffCenterLH( D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn, FLOAT zf );
	void		BuildMatrixOrthoLH( D3DXMATRIX *pOut, FLOAT w, FLOAT h, FLOAT zn, FLOAT zf );
	void		BuildMatrixOrthoOffCenterLH( D3DXMATRIX *pOut, FLOAT l, FLOAT r, FLOAT b, FLOAT t, FLOAT zn, FLOAT zf );

	void		ResetShaders(int bResetSystemShaders = 0);
	int			AddPixelShaderFromFile(const char* shaderName, const char* fileName, int bSystem = 0);
	int			AddPixelShaderFromFile(const char* shaderName, const char* fileName, int bSystem, const ShaderMacros & defines);
	void		ReloadShaderByFileName(const char* fileName);
	void		ReloadPixelShader( const char* shaderName, const char* fileName);
	void		ReloadPixelShader( const char* shaderName, const char* fileName, const ShaderMacros& defines);

	int			AddVertexShaderFromFile(const char* shaderName, const char* fileName, int bSystem = 0);
	int			AddVertexShaderFromFile(const char* shaderName, const char* fileName, int bSystem, const ShaderMacros & defines);
	void		ReloadVertexShader( const char* shaderName, const char* fileName );

	int			GetShaderIdx(const char* name);

	int			GetPixelShaderIdx( const char* name );
	int			GetVertexShaderIdx( const char* name );

	void		SetPixelShader(const char* name);
	void		SetPixelShader(int id = -1);
	void		SetVertexShader(const char* name);
	void		SetVertexShader(int id = -1);
	void		SetValidVertexShader(int id );

	void		SetDefaultCullMode( D3DCULL cullMode );
	void		SetCullMode( D3DCULL cullMode );
	void		RestoreCullMode();

	int			GetCurrentPixelShaderIdx() const;
	int			GetCurrentVertexShaderIdx() const;

	//
	// textures & colors
	//

	int			IsTextureFormatAvailable(D3DFORMAT fmt);
	bool		IsRenderTargetFormatAvailable(D3DFORMAT fmt);

	void		SetMipMapBias(float bias, int stage=-1);

	r3dColorFormat*			GetColorFormatData(D3DFORMAT fmt);
	r3dTexture*				AllocateTexture();
	r3dTexture*				LoadTexture( const char* texFile, D3DFORMAT texFormat = D3DFMT_UNKNOWN, bool bCheckFormat = false, int downScale = 1, int maxDim = 0, D3DPOOL pool = D3DPOOL_MANAGED, int gameResourcePool = TexMem, bool deferLoading = false );
	r3dThumbnailTexture*	LoadThumbnailTexture( const char* texFile, int downScale, int maxDim, D3DPOOL pool, int gameResourcePool );
	int						GetThumbnailTextureCount() const;
	r3dThumbnailTexture*	GetThumbnailTexture( int idx );
	void					DeleteThumbnailTexture( r3dThumbnailTexture* tex );
	void					PurgeThumbnailTextures();
	void					DeletePersistentTextures();

	void					ReloadTextureData( const char * fileName );

	void		DeleteTextureDelayed(r3dTexture *tex);
	void		DeleteTexture(r3dTexture* tex, int bForceDelete=0);
	void		ProcessDeletedTextures();

	// Set mode function
	void    	SetRenderingMode(int mode);
	int			GetRenderingMode() const;

	void		ResetMaterial();
	void		SetMaterial(r3dMaterial *mat);
	void		SetTex_( IDirect3DBaseTexture9* d3dTex, int StageID );
	void		SetTex(r3dTexture *Tex, int StageID=0);
	void		SetThumbTex(r3dThumbnailTexture* Tex, int StageID=0);
	void		SetRT( int slot, IDirect3DSurface9* surf);
	void		SetDSS( IDirect3DSurface9* dss );
	void		GetRT( int slot, IDirect3DSurface9** oRT ) const ;
	void		GetDSS( IDirect3DSurface9** dss ) const ;

	void		SetFog(int fogEnabled);

	void		StretchRect( class r3dScreenBuffer* source, class r3dScreenBuffer* target, int filter = 0 );

	//
	// Rendering Function
	//
	void		Flush();

	void Render2DPolygon(int numV, R3D_SCREEN_VERTEX *v);
	void Begin2DPolygon(int numVertsInPoly, int numPoly);
	void Fill2DPolygon(int numV, R3D_SCREEN_VERTEX *v);
	void End2DPolygon();

	void		Render3DPolygon(int numV, R3D_DEBUG_VERTEX *v);
	void		BeginFill3DPolygon(int NumV);
	void		Fill3DPolygon(int NumV, R3D_DEBUG_VERTEX *V);
	void		EndFill3DPolygon();
	int			GetPolygon3DBufferCapacity();

	int			Get3DLineMaxVertexCount();

	void		Render3DLine(int numV, R3D_DEBUG_VERTEX *v);
	void		BeginFill3DLine(int numV);
	void		Fill3DLine(int numV, R3D_DEBUG_VERTEX *v);
	void		EndFill3DLine();

	void		Render3DTriangles(int numV, R3D_DEBUG_VERTEX *v);

	float		GetClearZValue() const;
	float		GetNearPlaneZValue() const;

	void		SetZDir( ZDirEnum zdir );
	void		RestoreZFunc();

	void		SetForceBlackPixelShader( int bForceBlackShader );

	void		UnloadUnmanagedTextures();
	void		LoadUnmanagedTextures();

private:
	void		SetBlackPixelShader();

	VisibilityInfoEnum	IsBoxInsideFrustum_NORMAL(const r3dBoundBox& bbox, const D3DXPLANE (&planes)[6]);
	VisibilityInfoEnum	IsBoxInsideFrustum_PARABOLOID(const r3dBoundBox& bbox);

	VisibilityInfoEnum	IsSphereInsideFrustum_NORMAL(const r3dPoint3D& c, float r, const D3DXPLANE (&planes)[6]);
	VisibilityInfoEnum	IsSphereInsideFrustum_PARABOLOID(const r3dPoint3D& c, float r, const D3DXPLANE (&planes)[6]);

public:
	VisibilityInfoEnum	IsBoxInsideFrustum(const r3dBoundBox& bbox);
	VisibilityInfoEnum	IsBoxInsideCustomFrustum(const r3dBoundBox& bbox, const D3DXPLANE (&planes)[6]);
	VisibilityInfoEnum	IsSphereInsideFrustum(const r3dPoint3D& c, float r);
	VisibilityInfoEnum	IsSphereInsideCustomFrustum(const r3dPoint3D& c, float r, const D3DXPLANE (&planes)[6]);
	bool	DoesBoxIntersectNearPlane(const r3dBoundBox& bbox);

	bool		IsDeviceLost() const {
		return deviceLost_;
	}

	int		TryToRestoreDevice();
	void	UpdateSettings();
	void	UpdateDimmensions();
	void	ChangeForceAspect( float val );
	void	SetShaderLoadingEnabled( bool enabled );

	void	ResetTextureCache();

	DisplayResolutions	GetDisplayResolutions() const;

	void SetVertexDecl( IDirect3DVertexDeclaration9* decl ) const ;
	
	void DrawIndexed( D3DPRIMITIVETYPE type, INT baseVertexIndex, UINT minVertexIndex, UINT numVertices, UINT startIndex, UINT primCount );
	void DrawIndexedUP( D3DPRIMITIVETYPE primitiveType, UINT minVertexIndex, UINT numVertices, UINT primitiveCount, CONST void* pIndexData, D3DFORMAT indexDataFormat, CONST void* pVertexStreamZeroData, UINT vertexStreamZeroStride );
	void Draw( D3DPRIMITIVETYPE primitiveType,UINT startVertex,UINT primitiveCount );
	void DrawUP ( D3DPRIMITIVETYPE primitiveType, UINT primitiveCount, CONST void* pVertexStreamZeroData, UINT vertexStreamZeroStride );

	//
	// various renderer callback's.
	//

	// vertex color callback: called after each vertex color is calculated.
#define R3D_DEFINE_GETVERTCOLOR_CALLBACK(x)	\
	void x(R3D_SCREEN_VERTEX &V, r3dColor &clr)
	typedef void (*fn_GetVertColor)(R3D_SCREEN_VERTEX &V, r3dColor &clr);
	fn_GetVertColor	pfnGetVertColor;

	UINT InstanceCount ;

	int					ActiveCommandCount ;
	int					LastIssueActiveCommandsCount ;

	void MarkEssentialD3DCommand();
	void IssuePendingQuery();
	void SetPendingQuery( IDirect3DQuery9* query );
	void ResetQueryCounters();
	void UpdateShaderProfiles();

	void CheckOutOfMemory( HRESULT hr ) ;

	IDirect3DSurface9* GetTempSurfaceForScreenShots();
	IDirect3DTexture9* GetTempTextureForFrontBufferCopy();

	r3dFrustumType	GetFrustumType() const ;
	void			SetFrustumType( r3dFrustumType type );

	R3D_FORCEINLINE IDirect3DSurface9* GetMainDSS() const
	{
		return MainDSS;
	}

private:

	void AquireMainRT();

	IDirect3DQuery9*	PendingQuery;
	IDirect3DSurface9*	MainRT;
	IDirect3DSurface9*	MainDSS;
	D3DFORMAT			MainDSSFormat;
	IDirect3DSurface9*	RTs[ MAX_RENDER_TARGETS ];
	IDirect3DSurface9*	DSS;
	IDirect3DSurface9*  sysMemSurfaceForScreenShots;
	IDirect3DTexture9*  sysMemTextureForFrontBufferCopy;
	TextureArr			TexturesToDelete;

	r3dFrustumType		FrustumType;
};

R3D_FORCEINLINE
void r3dRenderLayer::DrawIndexed( D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount )
{
#if !DISABLE_PROFILER
	IssuePendingQuery();
	ActiveCommandCount ++ ;
	Stats.AddNumDraws( 1 ) ;

	Stats.AddNumTrianglesRendered ( primCount * InstanceCount );
	Stats.AddAverageStripLength ( primCount * InstanceCount );
#endif

	D3D_V( pd3ddev->DrawIndexedPrimitive( Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount ) );

}

R3D_FORCEINLINE
void r3dRenderLayer::DrawIndexedUP( D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride )
{
#if !DISABLE_PROFILER
	IssuePendingQuery();
	ActiveCommandCount ++ ;

	Stats.AddNumDraws( 1 );

	Stats.AddNumTrianglesRendered ( PrimitiveCount );
	Stats.AddAverageStripLength ( PrimitiveCount );
#endif

	// UP effectivly zeroes 0 stream ( others not? - find out )
	void ZeroZeroStreamCache();
	ZeroZeroStreamCache();

	// IndexedUP effectively zeroes indices
	void ZeroIndexCache();
	ZeroIndexCache();

	D3D_V( pd3ddev->DrawIndexedPrimitiveUP( PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride ) );
}

R3D_FORCEINLINE
void r3dRenderLayer::Draw( D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount )
{
#if !DISABLE_PROFILER
	IssuePendingQuery();
	ActiveCommandCount ++ ;

	Stats.AddNumDraws( 1 );

	Stats.AddNumTrianglesRendered ( PrimitiveCount );
	Stats.AddAverageStripLength	( PrimitiveCount );
#endif

	D3D_V( pd3ddev->DrawPrimitive( PrimitiveType, StartVertex, PrimitiveCount ) );
}

R3D_FORCEINLINE
void r3dRenderLayer::DrawUP ( D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride )
{
#if !DISABLE_PROFILER
	IssuePendingQuery();
	ActiveCommandCount ++ ;

	Stats.AddNumDraws( 1 );

	Stats.AddNumTrianglesRendered( PrimitiveCount );
	Stats.AddAverageStripLength	( PrimitiveCount );
#endif

	// UP effectivly zeroes 0 stream ( others not? - find out )
	void ZeroZeroStreamCache();
	ZeroZeroStreamCache();

	D3D_V( pd3ddev->DrawPrimitiveUP( PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride ) );
}

R3D_FORCEINLINE
void r3dRenderLayer::SetCullMode( D3DCULL CullMode )
{
	if( CullMode != CurrentCullMode )
	{
		D3D_V( pd3ddev->SetRenderState( D3DRS_CULLMODE, CullMode ) );
		CurrentCullMode = CullMode ;
	}
}

R3D_FORCEINLINE
void r3dRenderLayer::RestoreCullMode()
{
	SetCullMode( DefaultCullMode );
}

R3D_FORCEINLINE
void
r3dRenderLayer::SetRT( int slot, IDirect3DSurface9* surf )
{
	r3d_assert( slot >= 0 && slot < MAX_RENDER_TARGETS );

	// otherwise you get d3d error
	r3d_assert( slot || surf );

	if( surf != RTs[ slot ] )
	{
		RTs[ slot ] = surf ;	
		D3D_V( pd3ddev->SetRenderTarget( slot, surf ) );
	}
}

R3D_FORCEINLINE
void
r3dRenderLayer::SetDSS( IDirect3DSurface9* dss )
{
	if( dss != DSS )
	{
		D3D_V( pd3ddev->SetDepthStencilSurface( dss ) );
		DSS = dss ;
	}
}

R3D_FORCEINLINE
void
r3dRenderLayer::GetRT( int slot, IDirect3DSurface9** oRT ) const
{
	R3D_ENSURE_MAIN_THREAD();

	r3d_assert( slot >= 0 && slot < MAX_RENDER_TARGETS );

	IDirect3DSurface9* rt = RTs[ slot ] ;
	if( rt ) rt->AddRef();

	*oRT =  rt ;
}

R3D_FORCEINLINE
void
r3dRenderLayer::GetDSS( IDirect3DSurface9** dss ) const
{
	R3D_ENSURE_MAIN_THREAD();

	if( DSS ) DSS->AddRef() ;
	*dss = DSS ;
}

R3D_FORCEINLINE
void r3dRenderLayer::DoSetStreamSourceFreq( UINT streamNumber,UINT setting )
{
	D3D_V( pd3ddev->SetStreamSourceFreq( streamNumber, setting ) ) ;
}

R3D_FORCEINLINE
r3dZRange
r3dRenderLayer::GetZRange() const
{
	return ZRange ;
}

R3D_FORCEINLINE float r3dRenderLayer::GetClearZValue() const
{
	return ZDir == ZDIR_INVERSED ? 0.0f : 1.0f;
}

R3D_FORCEINLINE float r3dRenderLayer::GetNearPlaneZValue() const
{
	return ZDir == ZDIR_INVERSED ? 1.0f : 0.0f;
}

R3D_FORCEINLINE void r3dRenderLayer::SetZDir( ZDirEnum zdir )
{
	if( ZDir != zdir )
	{
		ZDir = zdir;

		RestoreZFunc();
	}
}

//------------------------------------------------------------------------

R3D_FORCEINLINE void r3dRenderLayer::RestoreZFunc()
{
	if( ZDir == ZDIR_INVERSED )
	{
		pd3ddev->SetRenderState( D3DRS_ZFUNC, D3DCMP_GREATEREQUAL );
	}
	else
	{
		pd3ddev->SetRenderState( D3DRS_ZFUNC, D3DCMP_LESSEQUAL );
	}
}

//------------------------------------------------------------------------

#define DrawIndexedPrimitive DIRECT_CALLS_OF_DRAW_XX_FUNCTIONS_NOT_ALLOWED_USE_REDRENDERLAYER_WRAPPER
#define DrawIndexedPrimitiveUP DIRECT_CALLS_OF_DRAW_XX_FUNCTIONS_NOT_ALLOWED_USE_REDRENDERLAYER_WRAPPER
#define DrawPrimitive DIRECT_CALLS_OF_XX_FUNCTIONS_NOT_ALLOWED_USE_REDRENDERLAYER_WRAPPER
#define DrawPrimitiveUP DIRECT_CALLS_OF_XX_FUNCTIONS_NOT_ALLOWED_USE_REDRENDERLAYER_WRAPPER
#define D3DRS_CULLMODE DIRECT_CULL_MODE_SETTING_NOT_ALLOWED_USE_RENDERLAYER_WRAPPER
#define SetTexture DIRECT_CALLS_OF_SET_TEXTURE_FUNCTION_NOT_ALLOWED_USE_REDRENDERLAYER_WRAPPER
#define SetRenderTarget DIRECT_CALLS_OF_SET_RENDER_TARGET_FUNCTION_NOT_ALLOWED_USE_REDRENDERLAYER_SETRT
#define SetDepthStencilSurface DIRECT_CALLS_OF_SET_RENDER_TARGET_FUNCTION_NOT_ALLOWED_USE_REDRENDERLAYER_SETDSS
#define GetRenderTarget DIRECT_CALLS_OF_GET_RENDER_TARGET_FUNCTION_NOT_ALLOWED_USE_REDRENDERLAYER_GETRT
#define GetDepthStencilSurface DIRECT_CALLS_OF_GET_DEPTH_STENCIL_SURFACE_FUNCTION_NOT_ALLOWED_USE_REDRENDERLAYER_GETDSS
#define SetVertexDeclaration DIRECT_CALLS_OF_SetVertexDeclaration_FUNCTION_NOT_ALLOWED_USE_D3DC_
#define SetViewport DoSetViewport // no direct viewports too
#define GetViewport DoGetViewport
#define SetStreamSourceFreq DoSetStreamSourceFreq // and no direct stream source freq too

struct r3dInstancedDraw
{
	R3D_FORCEINLINE
	r3dInstancedDraw( UINT InstanceCount )
	{
		r3dRenderer->InstanceCount = InstanceCount ;
	}

	R3D_FORCEINLINE
	~r3dInstancedDraw()
	{
		r3dRenderer->InstanceCount = 1 ;
	}
};

class r3dD3DResourceTunnel
{
public:
	r3dD3DResourceTunnel( )
	: mRes( 0 )
	{

	}

	R3D_FORCEINLINE
	int Valid() const
	{
		return (int)mRes ;
	}

	void ReleaseAndReset();

	IDirect3DResource9* Get() const
	{
		R3D_ENSURE_MAIN_THREAD();

		return mRes ;
	}

protected:
	IDirect3DResource9* mRes ;
};


template <typename T>
class r3dD3DResourceTunnelT : public r3dD3DResourceTunnel
{
public:
	typedef r3dD3DResourceTunnelT Parent ;


	R3D_FORCEINLINE
	void Set( T* res )
	{
		R3D_ENSURE_MAIN_THREAD();
		mRes = res ;
	}

	T* Get() const
	{
		R3D_ENSURE_MAIN_THREAD();

		return (T*)mRes ;
	}

	// WARNING: Do not use this to access the pointer! This is used for material sorting.
	UINT64 GetStamp() const
	{
		return (UINT64)mRes;
	}

	R3D_FORCEINLINE
	T* operator ->() const
	{
		R3D_ENSURE_MAIN_THREAD();

		r3d_assert( mRes );

		return (T*)mRes ;
	}

};

class r3dD3DVertexBufferTunnel : public r3dD3DResourceTunnelT< IDirect3DVertexBuffer9 >
{
public:
	void Lock( UINT OffsetToLock, UINT SizeToLock, VOID **ppData, DWORD Flags );
	void Unlock( );

};

class r3dD3DIndexBufferTunnel : public r3dD3DResourceTunnelT< IDirect3DIndexBuffer9 >
{
public:
	void Lock( UINT OffsetToLock, UINT SizeToLock, VOID **ppData, DWORD Flags );
	void Unlock( );
};

class r3dD3DTextureTunnel : public r3dD3DResourceTunnelT< IDirect3DBaseTexture9 >
{
public:
	IDirect3DBaseTexture9*		AsBaseTex() const ;
	IDirect3DTexture9*			AsTex2D() const ;
	IDirect3DCubeTexture9*		AsTexCube() const ;
	IDirect3DVolumeTexture9*	AsTexVolume() const ;

	int							GetLevelCount() const ;
	void						GetLevelDesc2D( int level, D3DSURFACE_DESC* oDesc ) const ;

	D3DFORMAT					GetFormat() const;

	void LockRect( UINT Level, D3DLOCKED_RECT *pLockedRect, const RECT *pRect, DWORD Flags );
	void UnlockRect( UINT Level );
};

class r3dD3DSurfaceTunnel : r3dD3DResourceTunnelT< IDirect3DSurface9 >
{
public:
	R3D_FORCEINLINE
	r3dD3DSurfaceTunnel()
	: mFormat( D3DFMT_UNKNOWN )
	{

	}

	R3D_FORCEINLINE
	D3DFORMAT GetFormat() const
	{
		return mFormat ;
	}

	void Set( IDirect3DSurface9* resource );
	int ReleaseAndReset();

	using Parent::Get;
	using Parent::Valid;
	using Parent::operator ->;

	friend class r3dDeviceTunnel;

private:
	D3DFORMAT mFormat ;
};

class r3dDeviceTunnel
{
public:
	// downscales if no VMEM
	static void CreateTextureAutoDownScale( UINT* Width,UINT* Height,UINT* Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool, r3dD3DTextureTunnel* TextureTunnel  );
	static void CreateTexture( UINT Width,UINT Height,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool, r3dD3DTextureTunnel* TextureTunnel  );
	static void CreateVolumeTexture( UINT Width,UINT Height,UINT Depth,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool, r3dD3DTextureTunnel* TextureTunnel );
	static void CreateCubeTexture( UINT EdgeLength,UINT Levels,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool, r3dD3DTextureTunnel* TextureTunnel );
	static void CreateVertexBuffer( UINT Length,DWORD Usage,DWORD FVF,D3DPOOL Pool, r3dD3DVertexBufferTunnel* VertexBufferTunnel );
	static void CreateIndexBuffer( UINT Length,DWORD Usage,D3DFORMAT Format,D3DPOOL Pool, r3dD3DIndexBufferTunnel* IndexBufferTunnel );
	static void CreateRenderTarget( UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,r3dD3DSurfaceTunnel* SurfaceTunnel );
	static void CreateDepthStencilSurface( UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard, r3dD3DSurfaceTunnel* SurfaceTunnel );
	static void CreateQuery( D3DQUERYTYPE Type,IDirect3DQuery9** ppQuery) ;
	static void CreateVertexDeclaration( const D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl );

	static void D3DXCreateTextureFromFileInMemoryEx( LPCVOID pSrcData, UINT SrcDataSize, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, r3dD3DTextureTunnel* TextureTunnel, const char* DEBUG_NAME, bool async = false );
	static void D3DXCreateVolumeTextureFromFileInMemoryEx( LPCVOID pSrcData, UINT SrcDataSize, UINT Width, UINT Height, UINT Depth, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, r3dD3DTextureTunnel* TextureTunnel, bool async = false );
	static void D3DXCreateCubeTextureFromFileInMemoryEx( LPCVOID pSrcData, UINT SrcDataSize, UINT Size, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pSrcInfo, PALETTEENTRY* pPalette, r3dD3DTextureTunnel* TextureTunnel, bool async = false );
	static void SetD3DResourcePrivateData( r3dD3DResourceTunnel* tunnel, const char* data ) ;
	static void SetD3DResourcePrivateData( r3dD3DSurfaceTunnel* tunnel, const char* data ) ;

} ;


// this is needed because of multithreaded loading.
// during such loading, a split between RegisterResource and D3DCreateResource may occur.
// inside this split device reset may occur.
// this may lead to D3D resource being created twice & D3D memory leaks.
// constructor/destructor of this class blocks device reset.
// it is required to be passed to r3dIResource constructor, thus hinting
// the user to put the block to prevent device reset between registering( construction ) of r3dIResource 
// and its D3D content creation

class r3dIntegrityGuardian
{
public:
	r3dIntegrityGuardian();
	~r3dIntegrityGuardian();
};

//
// some class for storing resources that should be recreated when d3d device is lost
//  please note that every derived class must be not created statically (at least for now)
//


class r3dIResource
{
public:
	explicit r3dIResource( const r3dIntegrityGuardian& ig ) ;
	virtual	~r3dIResource() ;

	virtual	void		D3DCreateResource()  = 0 ;
	virtual	void		D3DReleaseResource() = 0 ;
};

struct r3dDeviceInfo
{
	enum FormatFlag
	{
		DYNAMIC			= 1 << 0,
		FILTER			= 1 << 1,
		SRGB_READ		= 1 << 2,
		SRGB_WRITE		= 1 << 3,
		VERTEX_TEXTURE	= 1 << 4,
		DEPTHSTENCIL	= 1 << 5,
		RENDERTARGET	= 1 << 6,
		AUTOGENMIPMAP	= 1 << 7,
	};

	enum ExtraFlag
	{
		OCCLUSION_QUERIES	= 1 << 0,
		STAMP_QUERIES		= 1 << 1
	};

	typedef r3dTL::TFixedArray< WORD, 200 > FormatCapsArr ;

	DWORD					Ver ;	

	D3DCAPS9				Caps ;
	D3DADAPTER_IDENTIFIER9	AdapterIdentifier ;
	DWORD					ExtraFlags ;
	INT64					LocalMemSize ;

	FormatCapsArr			CubeTextureFormatCaps ;
	FormatCapsArr			VolTextureFormatCaps ;
	FormatCapsArr			TextureFormatCaps ;
	FormatCapsArr			SurfaceFormatCaps ;

	r3dDeviceInfo();
};

void FillDeviceInfo( r3dDeviceInfo* oInfo ) ;
void FPrintfDeviceInfo( const char* file, const r3dDeviceInfo& info ) ;

bool IsAspectLessThanForceAspect( float aspect );
bool IsAspectGreaterThanForceAspect( float aspect );
void ClearBackBufferFringes();
void r3dGetDesktopDimmensions( int * oWidth, int* oHeight );
void r3dProcessWindowMessages() ;

void r3dEnableWatchDog( float TimeOut );
void r3dSetWatchdogItemName( const char* ItemName );
void r3dResetWatchDog();
void r3dDisableWatchDog();

int  r3dGetTextureSizeInVideoMemory(int w, int h, int d, int mips, D3DFORMAT fmt);

void r3dPrintVMem() ;

void r3dEnsureDeviceAvailable();

extern void ( *r3dDeviceResetCallback )();
extern void ( *r3dDevicePreResetCallback )();

VisibilityInfoEnum IsBoxOnPositiveHalfSpace(const D3DXPLANE &p, const r3dBoundBox &bb);

extern int gDisableDynamicObjectShadows ;
extern CRITICAL_SECTION g_ResourceCritSection ;

struct SetRestoreZDir
{
	SetRestoreZDir( r3dRenderLayer::ZDirEnum zdir )
	{
		prevZDir = r3dRenderer->ZDir;
		r3dRenderer->SetZDir( zdir );
	}

	~SetRestoreZDir()
	{
		r3dRenderer->SetZDir( prevZDir );
	}

	r3dRenderLayer::ZDirEnum prevZDir;
};

IDirect3DQuery9* r3dGetOcclusionQuery( int i );
int r3dAllocateOcclusionQuery();
void r3dFreeOcclusionQuery( int i );

#endif // __R3D_RENDER_H
