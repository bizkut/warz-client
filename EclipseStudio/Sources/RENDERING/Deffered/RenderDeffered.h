#pragma once
#include "HUDFilters.h"

#include "gameobjects/ObjManag.h"

extern int SG_SlicesNum; // limited to 10

// shadow slices
struct ShadowSlice
{
	int   index;
	float depthBias; // bias for PCF
	float depthBias_HW ;
	float shadowMapSize;
	D3DXMATRIX lightView;
	D3DXMATRIX lightProj;
	r3dPoint3D camPos;
	float sphereRadius;
	float worldSizeX ;
	float worldSizeY ;
	float pixelDiameter ;

	ShadowSlice(int i, float size, float bias) : 
		index(i), depthBias(bias), depthBias_HW(bias*3), sphereRadius(1), shadowMapSize( 2048 ),
		worldSizeX( 1.f ), worldSizeY( 1.f ), pixelDiameter( 0.f )
		{

		}
};

struct TCloudsShadowParams
{
	float fTiling;
	float fScrollSpeedU;
	float fScrollSpeedV;
};

extern int CloudsShadows_Enable;
extern char CloudsShadows_TexName[ R3D_MAX_FILE_NAME ];
extern float CloudsShadows_BlendFactor;
extern TCloudsShadowParams CloudsShadows_Mask;
extern TCloudsShadowParams CloudsShadows_Noise1;
extern TCloudsShadowParams CloudsShadows_Noise2;

extern r3dScreenBuffer*	SunShadowMap;

extern float ShadowDrawingDistance;
extern float ShadowSunOffset;
extern float *ShadowSplitDistancesOpaque ;
extern float *ShadowSplitDistancesTransparent ;

extern float ShadowSplitDistancesOpaqueHigh[NumShadowSlices+1];
extern float ShadowSplitDistancesOpaqueLow[NumShadowSlices+1];
extern float ShadowSplitDistancesOpaqueMed[NumShadowSlices+1];
extern float ShadowSplitDistancesOpaque_DynamicCubemap[NumShadowSlices+1];

extern float ShadowSplitDistancesTransparentHigh[NumTransparentShadowSlices+1];

void SetupSMTransform(ShadowSlice& slice);
void ClearRenderTargetsForMultiGPU();

class r3dDefferedRenderer : public r3dRenderPipeline, public IObjectListener
{
public:

	r3dDefferedRenderer();
virtual	~r3dDefferedRenderer();

virtual	void		Init() OVERRIDE;
virtual	void		Close() OVERRIDE;

virtual	void		PreRender() OVERRIDE;

void				RenderShadows( r3dScreenBuffer* depthBuffer );

virtual	void		Render() OVERRIDE;

virtual void		AppendPostFXes() OVERRIDE;

virtual	void		PostProcess() OVERRIDE;

virtual void		RenderStage_Prepare() OVERRIDE;
virtual void		RenderStage_Terrain() OVERRIDE;
void				RenderStageHelper_StartDeferred();
virtual void		RenderStage_Deferred( int startObject, int objectCount ) OVERRIDE;
void				RenderStageHelper_EndDeferred();
virtual void		RenderStage_Finilize( const D3DVIEWPORT9* viewport ) OVERRIDE;

virtual int			GetStagedRenderObjectCount() OVERRIDE;

virtual void		CreateResolutionBoundResources() OVERRIDE;
virtual void		DestroyResolutionBoundResources() OVERRIDE;

virtual void		CreateShadowResources() OVERRIDE;
virtual void		DestroyShadowResources() OVERRIDE;

virtual void		DestroyAuxResource();
virtual void		CreateAuxResource();

virtual void		Finalize() OVERRIDE;
virtual void		SetPreColorCorrectionScreenshotTarget( r3dScreenBuffer* target ) OVERRIDE;

virtual void		OnDynamicObjectDeleted( GameObject* obj ) OVERRIDE;

private:
	r3dScreenBuffer*	mPreCCScreenshotTarget;
	RenderArray			mTempRenderArrays[ rsCount ];
	GameObject*			mTempObjectArray[ OBJECTMANAGER_MAXOBJECTS ];
	int					mTempObjectArrayCount;
};

struct ShadowMapOptimizationData
{
	float fMinX ;
	float fMaxX ;
	float fMinY ;
	float fMaxY ;

} extern gShadowMapOptimizationDataOpaque [ NumShadowSlices ] ;

extern int gCurrentShadowSlice ;

R3D_FORCEINLINE
void AppendShadowOptimizations( ShadowMapOptimizationData* oSMO, float miX, float maX, float miY, float maY )
{
	oSMO->fMinX = R3D_MIN( oSMO->fMinX, miX );
	oSMO->fMaxX = R3D_MAX( oSMO->fMaxX, maX );
	oSMO->fMinY = R3D_MIN( oSMO->fMinY, miY );
	oSMO->fMaxY = R3D_MAX( oSMO->fMaxY, maY );
}


void GetCCLUT3DTextureFullPath( char (&buffer)[512], const char* name );
int IsCCLUT3DTextureUsedAsGlobal( const char* name );
void RestoreCCLUT3DTexture ();
void ReloadCCLUT3DTexture( const char* newName, r3dAtmosphere::SkyPhase phase, HUDFilters filter );

enum StencilCheckMode
{
	SCM_LITAREA,
	SCM_UNLITAREA,
	SCM_SSAO,
	SCM_GRASS
};

void SetupLightMaskStencilStates( StencilCheckMode scm );
void SetupMaskWriteStencilStates( int Ref );
void RenderShadowScheme( float sx, float sy, float width, float height );
void RenderTransparentShadowScheme( float sx, float sy, float width, float height );

void ResetShadowCache();
void UpdateHWSchadowScheme();

void HardwareShadowMapsChangeCallback(int oldI, float oldF);
void ShadowBlurChangeCallback(int oldI, float oldF);

void InitDedicatedD3DProfilerStamps();

extern int D3DPROFILE_DEPTH_PREPASS ;
extern int D3DPROFILE_FILLGBUFFER ;
extern int D3DPROFILE_SSAO ;
extern int D3DPROFILE_SCALEFORM ;
extern int D3DPROFILE_FULLFRAME ;

//------------------------------------------------------------------------

struct SunHDRLightPShaderId
{
	union
	{
		struct
		{
			UINT32 sss				: 1;
			UINT32 transp_shadows	: 1;
			UINT32 white_albedo		: 1;
			UINT32 ambient_only		: 1;
			UINT32 quality			: 2;
			UINT32 ssr				: 1;
		};

		UINT32 Id;
	};

	SunHDRLightPShaderId();

	void ToString( char* str );
	void FillMacros( ShaderMacros& defines );
};

typedef r3dTL::TFixedArray< int, 256 > SunHDRLightPSIds ;

extern SunHDRLightPSIds gSunHDRLightPSIds ;

//------------------------------------------------------------------------

typedef r3dTL::TFixedArray< int, 1024 > PointLightPSIds ;

struct PointLightPShaderId
{
	union
	{
		struct
		{
			UINT32 sss : 1;
			UINT32 ssr : 1;
			UINT32 diffuse_only : 1;
			UINT32 ssao : 1;
			UINT32 proj_texture : 1;
			UINT32 shadow_mode : 3;
			UINT32 quality : 2;
		};

		UINT32 Id;
	};

	PointLightPShaderId();

	void ToString( char* str );
	void FillMacros( ShaderMacros& defines );
};

extern PointLightPSIds gPointLightPSIds;

//------------------------------------------------------------------------

typedef r3dTL::TFixedArray< int, 64 > SpotLightPSIds ;

struct SpotLightPShaderId
{
	union
	{
		struct
		{
			UINT32 aux_enabled	: 1;
			UINT32 shadow_mode	: 2;
			UINT32 proj_texture	: 1;
			UINT32 quality		: 2;
		};

		UINT32 Id;
	};

	SpotLightPShaderId();

	void ToString( char* str );
	void FillMacros( ShaderMacros& defines );
};

extern SpotLightPSIds gSpotLightPSIds ;


struct SSAOPShaderId
{
	union
	{
		struct
		{
			UINT32 num_rays					: 1 ;
			UINT32 optimized				: 1 ;
			UINT32 detail_radius			: 1 ;
#if R3D_ALLOW_TEMPORAL_SSAO
			UINT32 temporal_optimisation	: 1 ;
			UINT32 temporal_show_passed		: 1 ;
			UINT32 output_stability_mask	: 1 ;
#endif
		};

		UINT32 Id ;
	};

	SSAOPShaderId() ;

	void ToString( char* str ) ;
	void FillMacros( ShaderMacros& defines ) ;
	const char* GetShaderFile() ;
};

typedef r3dTL::TFixedArray< int, 64 > SSAOPSIds ;

extern SSAOPSIds gSSAOPSIds ;

struct StaticSkyPShaderID
{
	union
	{
		struct
		{
			UINT32 twoTextures			: 1;
			UINT32 outputNormals		: 1;
			UINT32 scatterSun			: 1;
			UINT32 hdr					: 1;
		};

		UINT32 Id;
	};

	StaticSkyPShaderID() ;

	void ToString( char* str ) ;
	void FillMacros( ShaderMacros& defines ) ;
};

typedef r3dTL::TFixedArray< int, 16 > StaticSkyPSIds;

extern StaticSkyPSIds gStaticSkyPSIds;

//------------------------------------------------------------------------

struct GrassVShaderID
{
	union
	{
		struct
		{
			UINT32 masked		: 1;
			UINT32 has_normals	: 1;
			UINT32 is_stiff		: 1;
		};

		UINT32 Id;
	};

	GrassVShaderID();

	void ToString( char* str );
	void FillMacros( ShaderMacros& defines );
};

typedef r3dTL::TFixedArray< int, 8 > GrassVSIds;

extern GrassVSIds gGrassVSIds;

//------------------------------------------------------------------------

struct GrassPShaderID
{
	union
	{
		struct
		{
			UINT32 do_clip		: 1;
			UINT32 aux			: 1;
			UINT32 output_depth	: 1;
		};

		UINT32 Id;
	};

	GrassPShaderID();

	void ToString( char* str );
	void FillMacros( ShaderMacros& defines );
};

typedef r3dTL::TFixedArray< int, 16 > GrassPSIds;

extern GrassPSIds gGrassPSIds;

//------------------------------------------------------------------------

enum ShadowAccumLightType
{
	SHADOWACCUM_LIGHT_DIRECT,
	SHADOWACCUM_LIGHT_POINT,
	SHADOWACCUM_LIGHT_SPOT,
	SHADOWACCUM_LIGHT_TYPE_COUNT
};

struct AccumShadowsPShaderID
{
	union
	{
		struct
		{
			UINT32 light_type		: 2;
			UINT32 fxaa_blur		: 1;
			UINT32 hw_shadowmaps	: 1;
			UINT32 recticular_warp	: 1;
			UINT32 use_normals	: 1;
		};

		UINT32 Id;
	};

	AccumShadowsPShaderID();

	void ToString( char* str );
	void FillMacros( ShaderMacros& defines );
};

typedef r3dTL::TFixedArray< int, 64 > AccumShadowsPShaderIDs;

extern AccumShadowsPShaderIDs gAccumShadowsPShaderIDs;

//------------------------------------------------------------------------

struct LensDirtShaderId
{
	enum
	{
		MAX_LIGHTS = 4
	};

	union
	{
		struct
		{
			UINT32 light_count		: 3;
		};

		UINT32 Id;
	};

	LensDirtShaderId();

	void ToString( char* str );
	void FillMacros( ShaderMacros& defines );
};

typedef r3dTL::TFixedArray< int, 8 > LensDirtShaderIDs;
extern LensDirtShaderIDs gLensDirtShaderIDs;

//------------------------------------------------------------------------

void GetDesiredRTDimmensions( float* oWidth, float* oHeight ) ;

void SetupFilmToneConstants( int reg ) ;

