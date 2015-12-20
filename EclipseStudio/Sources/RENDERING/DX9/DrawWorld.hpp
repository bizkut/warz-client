
#include "rendering/Deffered/CommonPostFX.h"

#include "../SF/Console/Config.h"
#include "../../ObjectsCode/Nature/obj_LocalColorCorrection.h"
#include "../Deffered/HUDFilters.h"

float SpecPower = 16.0f;
float FOVRange = 150.0f;


int HV_RenderShadows = 1;

int FogFilterMode = R3D_BLEND_ALPHA;
r3dColor FogFilterColor = r3dColor(255, 255, 255, 120);

float	__BlurCoef = 0.255f;
int	__BlurPower = 8;

float ZBias = 0.0001f;

inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

float HueAdjust = 1.0f; 
extern r3dScreenBuffer *D1;

//////////////////////////////////////////////////////////////////////////

int g_RenderRadialBlur = 0;
int g_RenderScopeEffect = 0;
int __RenderSSAOOnScreen = 0;
int __SSAOBlurEnable = 1;

//////////////////////////////////////////////////////////////////////////

static const int		MAX_SSAO_RINGS				= 4;
static const int		MAX_SSAO_SEGMENTS			= 8;
static const int		MAX_SSAO_SAMPLES			= MAX_SSAO_RINGS*MAX_SSAO_SEGMENTS;

static D3DXVECTOR3		g_vAORays[ MAX_SSAO_SAMPLES ];

namespace
{
	// sync with NUM_DIRS & NUM_STEPS of hsao_ps.hls
	const unsigned int	AO_NUM_DIRS = 5;
	const unsigned int  AO_NUM_STEPS = 3;
	const unsigned int	AO_DIRS_CONST_COUNT = AO_NUM_DIRS / 2 + (AO_NUM_STEPS & 1 );
	D3DXVECTOR4			g_vAODirs[ AO_DIRS_CONST_COUNT ];

	r3dTexture *					SSAO_RotTex					= NULL;
	r3dTexture *					SSAO_RotTex2D				= NULL;
	r3dTexture *					RadialBlur_ModulationTex	= NULL;

}

r3dTexture * CC_LUT1D_RGB_Tex		= NULL;
r3dTexture * CC_LUT1D_HSV_Tex		= NULL;

static void FillAORays ()
{
	const int dwNumSphereRings = MAX_SSAO_RINGS, dwNumSphereSegments = MAX_SSAO_SEGMENTS-1;
	float fDeltaRingAngle = ( R3D_PI / dwNumSphereRings );
	float fDeltaSegAngle  = ( 2.0f * R3D_PI / dwNumSphereSegments );
	int iRay = 0;
	for( unsigned int ring = 0; ring < dwNumSphereRings; ring++ )
	{
		float r = sinf( (ring) * fDeltaRingAngle );
		float y = cosf( (ring) * fDeltaRingAngle );
		for( unsigned int seg = 0; seg < (dwNumSphereSegments+1); seg++ )
			D3DXVec3Normalize ( &g_vAORays[ iRay++ ], &D3DXVECTOR3 ( r*cosf( seg * fDeltaSegAngle ), y, r*sinf( seg * fDeltaSegAngle ) ) );
	}

	{
		float alpha = 2.0f * R3D_PI / AO_NUM_DIRS;
		unsigned int j = 0;
		for( unsigned int i = 0; i < AO_NUM_DIRS / 2; i ++ )
		{
			float angle = alpha * j;
			g_vAODirs[ i ].x = cosf( angle );
			g_vAODirs[ i ].y = sinf( angle );

			j ++;

			angle = alpha * j;
			g_vAODirs[ i ].z = cosf( angle );
			g_vAODirs[ i ].w = sinf( angle );

			j ++;
		}

		if( AO_NUM_DIRS & 1 )
		{
			float angle = alpha * j;

			g_vAODirs[ AO_NUM_DIRS / 2 ].x = cosf( angle );
			g_vAODirs[ AO_NUM_DIRS / 2 ].y = sinf( angle );
		}
	}

	r3d_assert ( iRay == MAX_SSAO_SAMPLES );
}

//////////////////////////////////////////////////////////////////////////

void InitSSAO ()
{
	FillAORays ();

	// NORMAL defaults
	{
		SSAOSettings& sts = g_SSAOSettings[ SSM_REF ];

		sts.Radius					= 1.0f;
		sts.DepthRange				= 2.0f;
		sts.Brightness				= 0.9f;
		sts.Contrast				= 1.2f;

		sts.BlurDepthSensitivity	= 20.0f;
		sts.BlurStrength			= 1.0f;
		sts.RadiusExpandStart		= 100.f;
		sts.RadiusExpandCoef		= 0.03f;

		sts.TemporalTolerance		= 4.0f;
		sts.TemporalHistoryDepth	= 8.0f;

		sts.DetailPathEnable			= 0;
		sts.DetailRadius				= 0.0f;
		sts.DetailDepthRange			= 0.0f;
		sts.DetailStrength				= 0.0f;
		sts.DetailRadiusExpandStart		= 0.f;
		sts.DetailRadiusExpandCoef		= 0.f;
		sts.DetailFadeOut				= 0.f;

		sts.BlurTapCount			= 4;
		sts.BlurPassCount			= 1;

		sts.BlurGaussian      = 0;
	}

	// ALT LW defaults
	{
		SSAOSettings& sts = g_SSAOSettings[ SSM_DEFAULT ];

		sts.Radius			= 1.0f;
		sts.DepthRange		= 0.5f;
		sts.Brightness		= 0.9f;
		sts.Contrast		= 1.2f;

		sts.BlurDepthSensitivity	= 20.0f;
		sts.BlurStrength			= 1.0f;
		sts.RadiusExpandStart		= 0.f;
		sts.RadiusExpandCoef		= 0.f;

		sts.TemporalTolerance		= 4.0f;
		sts.TemporalHistoryDepth	= 8.0f;

		sts.DetailPathEnable			= 0;
		sts.DetailRadius				= 0.33f;
		sts.DetailDepthRange			= 3.3f;
		sts.DetailStrength				= 0.7f;
		sts.DetailRadiusExpandStart		= 0.f;
		sts.DetailRadiusExpandCoef		= 0.f;
		sts.DetailFadeOut				= 8.f;

		sts.BlurTapCount			= 4;
		sts.BlurPassCount			= 1;

 		sts.BlurGaussian      = 0;
	}

	// ALT defaults
	{
		SSAOSettings& sts = g_SSAOSettings[ SSM_HQ ];

		sts.Radius			= 1.0f;
		sts.DepthRange		= 0.5f;
		sts.Brightness		= 0.9f;
		sts.Contrast		= 1.2f;

		sts.BlurDepthSensitivity	= 20.0f;
		sts.BlurStrength			= 1.0f;
		sts.RadiusExpandStart		= 0.f;
		sts.RadiusExpandCoef		= 0.f;

		sts.TemporalTolerance		= 4.0f;
		sts.TemporalHistoryDepth	= 8.0f;

		sts.DetailPathEnable			= 0;
		sts.DetailRadius				= 0.33f;
		sts.DetailDepthRange			= 3.3f;
		sts.DetailStrength				= 0.7f;
		sts.DetailRadiusExpandStart	= 0.f;
		sts.DetailRadiusExpandCoef		= 0.f;
		sts.DetailFadeOut				= 8.f;

		sts.BlurTapCount			= 4;
		sts.BlurPassCount			= 1;

		sts.BlurGaussian      = 0;

	}

	// NORMAL constraints
	{
		SSAOConstraints& constr = g_SSAOConstraints[ SSM_REF ];

		constr.MinRadius					= 0.1f,		constr.MaxRadius					= 1.5f;
		constr.MinDepthRange				= 0.1f, 	constr.MaxDepthRange				= 10.f;
		constr.MinBrightness				= 0.0f, 	constr.MaxBrightness				= 2.0f;
		constr.MinContrast					= 0.0f, 	constr.MaxContrast					= 2.0f;
		constr.MinBlurDepthSensitivity		= 0.0f, 	constr.MaxBlurDepthSensitivity		= 50.0f;
		constr.MinBlurStrength				= 0.0f, 	constr.MaxBlurStrength				= 1.0f;
		constr.MinRadiusExpandStart			= 0.0f, 	constr.MaxRadiusExpandStart			= 200.f;
		constr.MinRadiusExpandCoef			= 0.0f,		constr.MaxRadiusExpandCoef			= 0.07f;

		constr.MinDetailStrength			= 0.0f,		constr.MaxDetailStrength			= 0.0f;
		constr.MinDetailRadius				= 0.0f,		constr.MaxDetailRadius				= 0.0f;
		constr.MinDetailDepthRange			= 0.0f,		constr.MaxDetailDepthRange			= 0.0f;
		constr.MinDetailRadiusExpandStart	= 0.0f,		constr.MaxDetailRadiusExpandStart	= 0.0f;
		constr.MinDetailRadiusExpandCoef	= 0.0f,		constr.MaxDetailRadiusExpandCoef	= 0.0f;
		constr.MinDetailFadeOut				= 0.0f,		constr.MaxDetailFadeOut				= 0.0f;
	}
	
	// ALT LW constraints
	{
		SSAOConstraints& constr = g_SSAOConstraints[ SSM_DEFAULT ];

		constr.MinRadius					= 0.1f,		constr.MaxRadius					= 1.5f;
		constr.MinDepthRange				= 0.05f,	constr.MaxDepthRange				= 2.2f;
		constr.MinBrightness				= 0.0f, 	constr.MaxBrightness				= 2.0f;
		constr.MinContrast					= 0.0f, 	constr.MaxContrast					= 2.0f;
		constr.MinBlurDepthSensitivity		= 0.0f, 	constr.MaxBlurDepthSensitivity		= 50.0f;
		constr.MinBlurStrength				= 0.0f, 	constr.MaxBlurStrength				= 1.0f;
		constr.MinRadiusExpandStart			= 0.0f, 	constr.MaxRadiusExpandStart			= 200.f;
		constr.MinRadiusExpandCoef			= 0.0f,		constr.MaxRadiusExpandCoef			= 0.07f;

		constr.MinDetailStrength			= 0.0f,		constr.MaxDetailStrength			= 2.0f;
		constr.MinDetailRadius				= 0.01f,	constr.MaxDetailRadius				= 1.0f;
		constr.MinDetailDepthRange			= 0.2f,		constr.MaxDetailDepthRange			= 5.f;
		constr.MinDetailRadiusExpandStart	= 0.0f,		constr.MaxDetailRadiusExpandStart	= 200.0f;
		constr.MinDetailRadiusExpandCoef	= 0.0f,		constr.MaxDetailRadiusExpandCoef	= 0.28f;
		constr.MinDetailFadeOut				= 0.0f,		constr.MaxDetailFadeOut				= 32.0f;
	}

	// ALT constraints
	{
		SSAOConstraints& constr = g_SSAOConstraints[ SSM_HQ ];

		constr.MinRadius					= 0.1f,		constr.MaxRadius					= 1.5f;
		constr.MinDepthRange				= 0.05f,	constr.MaxDepthRange				= 2.2f;
		constr.MinBrightness				= 0.0f, 	constr.MaxBrightness				= 2.0f;
		constr.MinContrast					= 0.0f, 	constr.MaxContrast					= 2.0f;
		constr.MinBlurDepthSensitivity		= 0.0f, 	constr.MaxBlurDepthSensitivity		= 33.0f;
		constr.MinBlurStrength				= 0.0f, 	constr.MaxBlurStrength				= 1.0f;
		constr.MinRadiusExpandStart			= 0.0f, 	constr.MaxRadiusExpandStart			= 200.f;
		constr.MinRadiusExpandCoef			= 0.0f,		constr.MaxRadiusExpandCoef			= 0.07f;

		constr.MinDetailStrength			= 0.0f,		constr.MaxDetailStrength			= 2.0f;
		constr.MinDetailRadius				= 0.05f,	constr.MaxDetailRadius				= 1.0f;
		constr.MinDetailDepthRange			= 0.05f,	constr.MaxDetailDepthRange			= 5.f;
		constr.MinDetailRadiusExpandStart	= 0.0f,		constr.MaxDetailRadiusExpandStart	= 200.0f;
		constr.MinDetailRadiusExpandCoef	= 0.0f,		constr.MaxDetailRadiusExpandCoef	= 0.28f;
		constr.MinDetailFadeOut				= 0.0f,		constr.MaxDetailFadeOut				= 32.0f;
	}

}

void InitColorCorrection()
{
	R3D_ENSURE_MAIN_THREAD();

	g_ColorCorrectionSettings.scheme = ColorCorrectionSettings::CCS_USE_RGB_CURVES;
	g_ColorCorrectionSettings.uiTexDim = 0;

	g_ColorCorrectionSettings.ResetCurvesRGB();
	g_ColorCorrectionSettings.ResetCurvesHSV();

	// RGB
	{
		CC_LUT1D_RGB_Tex	= r3dRenderer->AllocateTexture();
		CC_LUT1D_RGB_Tex->Create(256, 1, D3DFMT_A8R8G8B8, 1);

		unsigned char* Locked = (unsigned char*)CC_LUT1D_RGB_Tex->Lock(1);
		for(int i =0; i<256; ++i)
		{
			*Locked++ = i; // blue
			*Locked++ = i; // green
			*Locked++ = i; // red
			*Locked++ = 0; // alpha
		}
		CC_LUT1D_RGB_Tex->Unlock();
	}

	// HSV
	{
		CC_LUT1D_HSV_Tex	= r3dRenderer->AllocateTexture();
		CC_LUT1D_HSV_Tex->Create(256, 1, D3DFMT_A8R8G8B8, 1);

		unsigned char* Locked = (unsigned char*)CC_LUT1D_HSV_Tex->Lock(1);
		for(int i =0; i<256; ++i)
		{
			*Locked++ = i; // blue
			*Locked++ = i; // green
			*Locked++ = 127; // red
			*Locked++ = 0; // alpha
		}
		CC_LUT1D_HSV_Tex->Unlock();
	}

}

#define EC_CCLUT3D_TEX_PATH "Data\\Shaders\\Texture\\CCLUT3D\\"

void GetCCLUT3DTextureFullPath( char (&buffer)[512], const char* name )
{
	sprintf( buffer, EC_CCLUT3D_TEX_PATH "%s", name );
}

int IsCCLUT3DTextureUsedAsGlobal( const char* name )
{
	return gHUDFilterSettings[ HUDFilter_Default ].IsTextureUsedAsColorCorrection( name );
}

void RestoreCCLUT3DTexture ()
{
	for( int i = 0; i < HUDFilter_Total; i++ )
	{
		gHUDFilterSettings[i].DeleteColorCorrectionTextures();
	}

	for( int i = 0; i < HUDFilter_Total; i++ )
	{
		gHUDFilterSettings[i].LoadColorCorrectionTextures();
	}
}

const r3dString& GetCCLUT3DTextureName( HUDFilters filter, r3dAtmosphere::SkyPhase phase )
{
	const HUDFilterSettings &hfs = gHUDFilterSettings[filter];
	return hfs.colorCorrectionTextureNames[ phase ];
}

void ReloadCCLUT3DTexture( const char* newName, r3dAtmosphere::SkyPhase phase, HUDFilters filter )
{
	HUDFilterSettings &hfs = gHUDFilterSettings[filter];

	if( hfs.colorCorrectionTextures[ phase ] )
	{
		r3dRenderer->DeleteTexture( hfs.colorCorrectionTextures[ phase ] );
		hfs.colorCorrectionTextures[ phase ] = NULL;
	}

	char fullPath[ 512 ];

	GetCCLUT3DTextureFullPath( fullPath, newName );

	r3dscpy( hfs.colorCorrectionTextureNames[ phase ], newName );

	hfs.colorCorrectionTextures[ phase ] = r3dRenderer->LoadTexture( fullPath );
}

void ReloadAllCCLUT3DTextures( const char* newName, HUDFilters filter )
{	
	for( int i = 0, e = r3dAtmosphere::SKY_PHASE_COUNT; i < e ; i ++ )
	{
		ReloadCCLUT3DTexture( newName, r3dAtmosphere::SkyPhase( i ), filter );
	}
}

void PostFX_UpdateResources()
{
	if( SSAO_RotTex == NULL )
	{
		SSAO_RotTex		= r3dRenderer->LoadTexture( "Data\\Shaders\\Texture\\Rot4.dds" );
		SSAO_RotTex2D	= r3dRenderer->LoadTexture( "Data\\Shaders\\Texture\\Rot2D.dds" );
	}

	if( RadialBlur_ModulationTex == NULL )
	{
		RadialBlur_ModulationTex = r3dRenderer->LoadTexture( "Data\\Shaders\\Texture\\Modulation.dds" );
	}

	if( gHUDFilterSettings[HUDFilter_Default].colorCorrectionTextures[ 0 ] == NULL )
	{
		gHUDFilterSettings[HUDFilter_Default].FixColorCorrectionTextureNames();
		gHUDFilterSettings[HUDFilter_Default].LoadColorCorrectionTextures();
	}
}

void RenderSSAORefEffect()
{
 if ( !r_ssao->GetBool() ) 
	 return;

 PostFX_UpdateResources();

 r3dSetRestoreFSQuadVDecl setRestoreVDECL; (void)setRestoreVDECL;

 r3dRenderer->SetVertexShader("VS_SSAO");
 r3dRenderer->SetPixelShader("PS_SSAO_REF");

 //r3dRenderer->SetRenderingMode(R3D_BLEND_MODULATE | R3D_BLEND_NZ);
 r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_NZ);

 r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
 r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP);
 r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
 r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP);
 r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
 r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP);

 float noiseScaleK = r_half_scale_ssao->GetInt() ? 0.5f : 1.0f;

 D3DXVECTOR4 vconst = D3DXVECTOR4( 0.5f / r3dRenderer->ScreenW, 0.5f / r3dRenderer->ScreenH, r3dRenderer->ScreenW * 0.25f * noiseScaleK, r3dRenderer->ScreenH * 0.25f * noiseScaleK );
 r3dRenderer->pd3ddev->SetVertexShaderConstantF(  0, (float *)&vconst,  1 );

 // mat proj

 float fNear = r3dRenderer->NearClip;
 float fFar = r3dRenderer->FarClip;

 const SSAOSettings& sts = g_SSAOSettings[ SSM_REF ];

 D3DXVECTOR4 pconst0 = D3DXVECTOR4(sts.Radius/fFar, sts.Radius/fFar*r3dRenderer->ScreenH/r3dRenderer->ScreenW, sts.DepthRange/fFar, 1.0f / fFar );
 D3DXVECTOR4 pconst1 = D3DXVECTOR4(0.f, 0.f, sts.RadiusExpandStart / fFar, fFar * sts.RadiusExpandCoef );
 D3DXVECTOR4 pconst2 = D3DXVECTOR4( sts.Contrast, sts.Brightness * sts.Contrast - 1.5f * sts.Contrast + 0.5f, 0.f, 0.f ); 

 r3dRenderer->pd3ddev->SetPixelShaderConstantF(  0, (float *)&pconst0,  1 );
 r3dRenderer->pd3ddev->SetPixelShaderConstantF(  1, (float *)&pconst1,  1 );
 r3dRenderer->pd3ddev->SetPixelShaderConstantF(  2, (float *)&pconst2,  1 );
 
 r3dSetFiltering( R3D_POINT, 0 );
 r3dSetFiltering( R3D_POINT, 1 );
 r3dSetFiltering( R3D_POINT, 2 );

 r3dRenderer->SetMipMapBias(-6, 2);
 
 r3dRenderer->SetTex( SSAO_RotTex, 2 );
 r3dRenderer->SetTex( DepthBuffer->Tex );

 r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, 	D3DCOLORWRITEENABLE_RED );

 r3dDrawFullScreenQuad(!!r_half_scale_ssao->GetInt());

 r3dRenderer->SetMipMapBias(0, 2);

 r3dRenderer->SetVertexShader();
 r3dRenderer->SetPixelShader();
 r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, 	0xffffffff );
}

D3DXMATRIX g_PrevSSAO_View ;
int g_PrevSSAO_Valid ;

int g_SSAO_Temporal_Reveal ;

int g_SSAO_ResetCount ;

void FinalizeSSAORender()
{
	g_PrevSSAO_Valid = 1 ;
	g_PrevSSAO_View = r3dRenderer->ViewMatrix ;
}

float GetDefaultSSAOValue()
{
	const SSAOSettings& sts = g_SSAOSettings[ r_ssao_method->GetInt() ];

	float val = sts.Contrast + sts.Brightness * sts.Contrast - 1.5f * sts.Contrast + 0.5f;

	return val;
}

void UpdateSSAOClearValue()
{
	if( r_half_scale_ssao->GetInt() )
	{
		r_ssao_clear_val->SetFloat( 0.f );
	}
	else
	{
		float defVal = GetDefaultSSAOValue();

		r_ssao_clear_val->SetFloat( defVal );
	}
}

void RenderSSAOEffect( bool lightWeight )
{
	if ( !r_ssao->GetBool() ) 
		return;

	PostFX_UpdateResources();

	r3dSetRestoreFSQuadVDecl setRestoreVDECL; (void)setRestoreVDECL;

#if R3D_ALLOW_TEMPORAL_SSAO
	int doSSAOTemporalOptimize = R3D_SSAO_TEMPORAL_OPTIMIZE == r_ssao_temporal_filter->GetInt() ;

	if( ++g_SSAO_ResetCount >= r_ssao_temporal_reset_freq->GetInt() )
	{
		g_SSAO_ResetCount = 0 ;
		doSSAOTemporalOptimize = 0 ;
	}
#else
	int doSSAOTemporalOptimize = 0 ;
#endif

	const SSAOSettings& sts = g_SSAOSettings[ lightWeight ? SSM_DEFAULT : SSM_HQ ];

	int detailEnable = sts.DetailPathEnable ;

	r3dRenderer->SetVertexShader( "VS_SSAO" ) ;

	SSAOPShaderId psid ;

	psid.num_rays				= !lightWeight ;
	psid.optimized				= !!r_optimized_ssao->GetInt() ;
	psid.detail_radius			= detailEnable ;

#if R3D_ALLOW_TEMPORAL_SSAO
	psid.temporal_optimisation	= doSSAOTemporalOptimize ;
	psid.temporal_show_passed	= g_SSAO_Temporal_Reveal ;
	psid.output_stability_mask	= r_ssao_temporal_filter->GetInt() == R3D_SSAO_TEMPORAL_FILTER ;
#endif
	
	r3dRenderer->SetPixelShader( gSSAOPSIds[ psid.Id ] ) ;

	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_NZ);

	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );

	float noiseScaleK = r_half_scale_ssao->GetInt() ? 0.5f : 1.0f;

	extern r3dScreenBuffer* gBuffer_Depth;

	D3DXVECTOR4 vconst = D3DXVECTOR4( 0.5f / gBuffer_Depth->Width, 0.5f / gBuffer_Depth->Height, gBuffer_Depth->Width * 0.25f * noiseScaleK, gBuffer_Depth->Height * 0.25f * noiseScaleK );
	r3dRenderer->pd3ddev->SetVertexShaderConstantF(  0, (float *)&vconst,  1 );

	// mat proj

	float fNear = r3dRenderer->NearClip;
	float fFar = r3dRenderer->FarClip;

	const int RAYS_START = 7 + 3 + 3 + 3 ;

#define SSAO_ALT_DETAIL_NUM_RAYS	24

	COMPILE_ASSERT( SSAO_ALT_DETAIL_NUM_RAYS > SSAO_ALT_NUM_RAYS );

	const int RADIUS_SPLIT = SSAO_ALT_NUM_RAYS;

	// NOTE: sync with ssao_alt_ps.hls
	const int NUM_RAYS = 
			lightWeight ?
			SSAO_LW_NUM_RAYS 
				:
			( detailEnable ? SSAO_ALT_DETAIL_NUM_RAYS : SSAO_ALT_NUM_RAYS );

	if( !g_PrevSSAO_Valid )
	{
		D3DXMatrixIdentity( &g_PrevSSAO_View ) ;
	}

	float aspect = 1.0f ;

	D3DXVECTOR4 pconsts[ RAYS_START + SSAO_ALT_DETAIL_NUM_RAYS ];

	// float4      g_vZScale0_ColorControl     : register ( c0 );
	pconsts[ 0 ] = D3DXVECTOR4( sts.DepthRange / sts.Radius, 128.0f / sts.Radius, sts.Contrast, sts.Brightness * sts.Contrast - 1.5f * sts.Contrast + 0.5f );
	// float4      g_vProjScaleTrans           : register ( c1 );
	pconsts[ 1 ] = D3DXVECTOR4( 0.5f*r3dRenderer->ProjMatrix._11, -0.5f*r3dRenderer->ProjMatrix._22, 0.5f, 0.5f );
	// float4      g_vInvProjScaleTrans        : register ( c2 );
	pconsts[ 2 ] = D3DXVECTOR4( 2.0f / r3dRenderer->ProjMatrix._11, -2.0f / r3dRenderer->ProjMatrix._22, -1.0f / r3dRenderer->ProjMatrix._11, 1.0f / r3dRenderer->ProjMatrix._22 );
	// float4      g_vInvRes_DepthFadeRange    : register ( c3 );
	pconsts[ 3 ] = D3DXVECTOR4( 1.0f / gBuffer_Depth->Width, 1.0f / gBuffer_Depth->Height, 0.985f * fFar, 0.99f * fFar );
	// float4      g_vExpandRanges             : register ( c4 );
	pconsts[ 4 ] = D3DXVECTOR4( sts.RadiusExpandStart, sts.DetailRadiusExpandStart, sts.RadiusExpandCoef, sts.DetailRadiusExpandCoef );
	// float4      g_vDetail_Fade_ZScale1      : register ( c5 );
	pconsts[ 5 ] = D3DXVECTOR4( sts.DetailStrength, sts.DetailFadeOut * sts.DetailRadius, sts.DetailDepthRange / sts.DetailRadius, 128.0f / sts.DetailRadius );
	// float4      g_vTempoCtrl                : register ( c6 );
	pconsts[ 6 ] = D3DXVECTOR4( 0.00125f * sts.TemporalTolerance, 1.f / 512.f, aspect / 512.f, 0.f );

	if( doSSAOTemporalOptimize )
	{
		pconsts[ 6 ].w = r_half_scale_ssao->GetInt() ? 0.5f : 1.0f ;
	}

	// float4x3    g_mViewMtx                  : register ( c7 );
	D3DXMatrixTranspose( (D3DXMATRIX*)&pconsts[7], &r3dRenderer->ViewMatrix );
	// ^^^^
	// NOTE : last row is overwritten below

	D3DXMATRIX toPrevViewMtx = r3dRenderer->InvViewMatrix * g_PrevSSAO_View ;

	// float4x3    g_mToPrevViewMtx    : register ( c10 );
	D3DXMatrixTranspose( (D3DXMATRIX*)&pconsts[10], &toPrevViewMtx );
	// ^^^^
	// NOTE : last row is overwritten below

	// float4x3    g_mFromPrevViewMtx  : register ( c13 );
	D3DXMATRIX fromPrevViewMtx ;
	D3DXMatrixInverse( &fromPrevViewMtx, NULL, &toPrevViewMtx ) ;
	D3DXMatrixTranspose( (D3DXMATRIX*)&pconsts[13], &fromPrevViewMtx );
	// ^^^^
	// NOTE : last row is overwritten below

	int r = 0;

	if( !lightWeight )
	{
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4(-0.18486f, 0.32808f, 0.00708f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4( 0.22890f, 0.93380f,-0.09171f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4(-0.36097f, 0.18230f,-0.38227f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4( 0.06232f, 0.32664f, 0.21049f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4( 0.68342f, 0.25225f,-0.06311f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4(-0.05478f, 0.09994f, 0.34463f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4( 0.01732f, 0.36483f,-0.49192f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4(-0.72131f, 0.22451f,-0.09716f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4( 0.32283f, 0.33296f, 0.11536f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4(-0.27977f, 0.18833f, 0.16797f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4(-0.50663f, 0.08494f, 0.63250f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4( 0.35578f, 0.11564f, 0.50909f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4( 0.54817f, 0.23470f,-0.61668f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4(-0.15970f, 0.08904f,-0.66253f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4( 0.73055f, 0.08323f, 0.30949f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4(-0.68126f, 0.50000f, 0.10878f, 0 );
	}

	if( detailEnable || lightWeight )
	{
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4(-0.01209f, 0.95443f, 0.11314f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4( 0.79393f, 0.23041f, 0.11043f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4(-0.77869f, 0.26835f,-0.35691f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4(-0.24429f, 0.15942f, 0.46862f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4( 0.32674f, 0.51379f,-0.52337f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4( 0.34767f, 0.07975f, 0.67386f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4(-0.13244f, 0.06251f,-0.58013f, 0 );
		pconsts[ RAYS_START + r++ ] = D3DXVECTOR4(-0.30083f, 0.50000f, 0.09436f, 0 );
	}

	r3d_assert( r == NUM_RAYS );

	for( int i = 0, e = RADIUS_SPLIT; i < e; i ++ )
	{
		pconsts[ RAYS_START + i ] *= sts.Radius;
	}

	for( int i = RADIUS_SPLIT, e = NUM_RAYS; i < e; i ++ )
	{
		pconsts[ RAYS_START + i ] *= sts.DetailRadius;
	}

	if( !r_optimized_ssao->GetInt() )
	{
		for( int i = 0, e = NUM_RAYS; i < e; i ++ )
		{
			D3DXVec4Transform( pconsts + RAYS_START + i, pconsts + RAYS_START + i, &r3dRenderer->ViewMatrix );
		}
	}

	r3dRenderer->pd3ddev->SetPixelShaderConstantF(  0, (float*)pconsts, RAYS_START + NUM_RAYS );

	r3dSetFiltering( R3D_POINT, 0 );
	r3dSetFiltering( R3D_POINT, 1 );
	r3dSetFiltering( R3D_POINT, 2 );

	r3dRenderer->SetMipMapBias( -6, 2 );

	extern r3dScreenBuffer* gBuffer_Normal;

	r3dRenderer->SetTex( DepthBuffer->Tex		, 0	);
	r3dRenderer->SetTex( gBuffer_Normal->Tex	, 1	);
	r3dRenderer->SetTex( SSAO_RotTex2D			, 2	);

	if( doSSAOTemporalOptimize )
	{
		r3dRenderer->pd3ddev->SetSamplerState( 3, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
		r3dRenderer->pd3ddev->SetSamplerState( 3, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );
		r3dRenderer->pd3ddev->SetSamplerState( 4, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
		r3dRenderer->pd3ddev->SetSamplerState( 4, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );

		r3dSetFiltering( R3D_BILINEAR, 3 );
		r3dSetFiltering( R3D_POINT, 4 );

		r3dRenderer->SetTex( PrevDepthBuffer->Tex	, 3 );
		r3dRenderer->SetTex( PrevSSAOBuffer->Tex	, 4 );
	}

#if R3D_ALLOW_TEMPORAL_SSAO
	if( r_ssao_temporal_filter->GetInt() == R3D_SSAO_TEMPORAL_FILTER )
		r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN );
	else
#endif
		r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED );

	r3dDrawFullScreenQuad( false );

	r3dRenderer->SetMipMapBias(0, 2);

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, 	0xffffffff );
}


void RenderSSAOEffect ()
{
	if ( !r_ssao->GetBool() )
		return;

	D3DPERF_BeginEvent(D3DCOLOR_XRGB(255,255,255), L"RenderSSAOEffect" );

	R3DPROFILE_D3DSTART( D3DPROFILE_SSAO ) ;

	if( !__SSAOBlurEnable || r_half_scale_ssao->GetInt())
	{
		// with half scale ssao we don't use stencil so the background
		// is SSAOed. With full scale ssao we use stencil to avoid expensive
		// stuff where sky is, so we have to clear to not let it through.
		// ( this step is done later if SSAO blur is on )
		D3D_V( r3dRenderer->pd3ddev->Clear( 0, 0, D3DCLEAR_TARGET, 0xffffffff, r3dRenderer->GetClearZValue(), 0 ) ) ;
	}

	switch( r_ssao_method->GetInt() )
	{
	case SSM_REF:
		RenderSSAORefEffect();
		break;
	case SSM_DEFAULT:
		RenderSSAOEffect( true );
		break;
	case SSM_HQ:
		RenderSSAOEffect( false );
		break;
	}

	R3DPROFILE_D3DEND( D3DPROFILE_SSAO ) ;

	D3DPERF_EndEvent ();
}

static void SetupScreenTexDSP( int contNum )
{
	D3DXVECTOR4 vconst = D3DXVECTOR4( 0.5f / r3dRenderer->ScreenW, 0.5f / r3dRenderer->ScreenH, 0.f, 0.f );
	r3dRenderer->pd3ddev->SetVertexShaderConstantF(  contNum, (float *)&vconst, 1  );
}

void RenderLUT1DColorCorrection( r3dScreenBuffer* SourceTex, bool hsv )
{
	PostFX_UpdateResources();

	r3dSetRestoreFSQuadVDecl setRestoreVDECL; (void)setRestoreVDECL;

	r3dRenderer->SetVertexShader( "VS_POSTFX" );
	r3dRenderer->SetPixelShader( hsv ? "PS_CC_LUT1D_HSV" : "PS_CC_LUT1D_RGB" );

	r3dSetFiltering( R3D_POINT,		0 );
	r3dSetFiltering( R3D_BILINEAR,	1 );

	r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	r3dRenderer->SetTex( SourceTex->Tex );
	extern r3dTexture* CC_LUT1D_HSV_Tex, *CC_LUT1D_RGB_Tex;
	r3dRenderer->SetTex( hsv ? CC_LUT1D_HSV_Tex : CC_LUT1D_RGB_Tex, 1 );

	SetupScreenTexDSP( 0 );

	r3dDrawFullScreenQuad(false);

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();
}

void CopyScreen( r3dScreenBuffer* SourceTex )
{
	PostFX_UpdateResources();

	r3dSetRestoreFSQuadVDecl setRestoreVDECL; (void)setRestoreVDECL;

	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_NZ);

	r3dRenderer->SetVertexShader( "VS_POSTFX" );
	r3dRenderer->SetPixelShader( "PS_FSCOPY" );

	r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	r3dSetFiltering( R3D_POINT, 0 );

	r3dRenderer->SetTex( SourceTex->Tex );

	SetupScreenTexDSP( 0 );

	r3dDrawFullScreenQuad(false);

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();
}

void DiluteSSAOMask( r3dScreenBuffer *sourceTex )
{
	D3DPERF_BeginEvent( 0, L"DiluteSSAOMask" ) ;

	r3dSetRestoreFSQuadVDecl setRestoreVDECL; (void)setRestoreVDECL;

	float resK;
	int HalfScale;

	resK = r_half_scale_ssao->GetInt() ? 0.5f : 1.0f ;
	HalfScale = !!r_half_scale_ssao->GetInt() ;

	float vConsts[ 4 ] = {	1.0f / r3dRenderer->ScreenW, 0.f, 0.f, 1.0f / r3dRenderer->ScreenH } ;
	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, vConsts, 1 ) ) ;

	r3dRenderer->SetVertexShader( "VS_SSAO" ) ;
	r3dRenderer->SetPixelShader( "PS_SSAO_MASK_DILUTE" ) ;

	r3dRenderer->SetTex( sourceTex->Tex, 0 ) ;
	r3dSetFiltering( R3D_BILINEAR, 0 ) ;

	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP ) );

	D3DXVECTOR4 vconst = D3DXVECTOR4( 0.5f / r3dRenderer->ScreenW, 0.5f / r3dRenderer->ScreenH, resK, resK ) ;
	r3dRenderer->pd3ddev->SetVertexShaderConstantF(  0, (float *)&vconst, 1 ) ;

	r3dDrawFullScreenQuad(!!HalfScale);

	D3DPERF_EndEvent() ;
}

void CompositeSSAO( r3dScreenBuffer* currSSAO )
{
	D3DPERF_BeginEvent( 0, L"CompositeSSAO" ) ;

	const SSAOSettings& sts = g_SSAOSettings[ R3D_MIN( R3D_MAX( r_ssao_method->GetInt(), 0 ), SSM_COUNT - 1 ) ] ;

	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);

	r3dSetRestoreFSQuadVDecl setRestoreVDECL; (void)setRestoreVDECL;

	float resK ;
	int HalfScale ;

	resK = r_half_scale_ssao->GetInt() ? 0.5f : 1.0f ;
	HalfScale = !!r_half_scale_ssao->GetInt() ;

	r3dRenderer->SetVertexShader( "VS_SSAO" ) ;
	r3dRenderer->SetPixelShader( "PS_SSAO_COMPOSITE" ) ;

	const float HISTORY_DEPTH = sts.TemporalHistoryDepth ;

	r3dTL::TFixedArray< D3DXVECTOR4, 10 > psConsts ;

	// float4   gSettings            : register ( c0 ) ;
	psConsts[ 0 ] = D3DXVECTOR4( HISTORY_DEPTH / ( HISTORY_DEPTH + 1.f ), 1.0f / ( HISTORY_DEPTH + 1.f ), 0.f, 0.125f * sts.TemporalTolerance ) ;
	// float4   g_vProjScaleTrans    : register ( c1 ) ;
	psConsts[ 1 ] = D3DXVECTOR4( 0.5f*r3dRenderer->ProjMatrix._11, -0.5f*r3dRenderer->ProjMatrix._22, 0.5f, 0.5f );
	// float4   g_vInvProjScaleTrans : register ( c2 ) ;
	psConsts[ 2 ] = D3DXVECTOR4( 2.0f / r3dRenderer->ProjMatrix._11, -2.0f / r3dRenderer->ProjMatrix._22, -1.0f / r3dRenderer->ProjMatrix._11, 1.0f / r3dRenderer->ProjMatrix._22 );
	// float4x3 g_vToPrevViewMtx     : register ( c3 ) ;
	D3DXMATRIX toPrevViewMtx = r3dRenderer->InvViewMatrix * g_PrevSSAO_View ;
	D3DXMatrixTranspose( (D3DXMATRIX*)&psConsts[3], &toPrevViewMtx );
	// float4x3 g_vFromPrevViewMtx   : register ( c6 ) ;
	D3DXMATRIX fromPrevViewMtx ;
	D3DXMatrixInverse( &fromPrevViewMtx, NULL, &toPrevViewMtx ) ;
	D3DXMatrixTranspose( (D3DXMATRIX*)&psConsts[6], &fromPrevViewMtx );

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, &psConsts[ 0 ].x, psConsts.COUNT ) ) ;

	extern r3dScreenBuffer* gBuffer_Depth ;

	r3dRenderer->SetTex( PrevSSAOBuffer->Tex, 0 ) ;
	r3dRenderer->SetTex( currSSAO->Tex, 1 ) ;
	r3dRenderer->SetTex( PrevDepthBuffer->Tex, 2 ) ;
	r3dRenderer->SetTex( gBuffer_Depth->Tex, 3 ) ;
	
	for( int i = 0 ; i < 4 ; i ++ )
	{
		r3dSetFiltering( R3D_BILINEAR, i ) ;

		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP ) );
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP ) );
	}

	D3DXVECTOR4 vconst = D3DXVECTOR4( 0.5f / r3dRenderer->ScreenW, 0.5f / r3dRenderer->ScreenH, resK, resK ) ;
	r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float *)&vconst, 1 ) ;

	r3dDrawFullScreenQuad( !!HalfScale ) ;

	D3DPERF_EndEvent() ;
}

float GaussianDistribution( float x, float y, float rho )
{
	float g = 1.0f / sqrtf(2.0f * R3D_PI * rho * rho);
	g *= expf( -(x*x + y*y)/(2*rho*rho) );

	return g;
}

// ------------------------------------------------------------------------------------------------
void GetSampleOffsetsGauss(float dx, float dy, D3DXVECTOR4 * tcOffset, D3DXVECTOR4* weights, unsigned int sampleCount, D3DXVECTOR4& offset)
{
	float weight = GaussianDistribution(0.0f, 0.0f, 4.0f);

	weights[0] = D3DXVECTOR4(weight, weight, weight, weight);
	tcOffset[0]= D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);


	float totalWeights = weights[0].x;

	for (unsigned int i = 0; i < sampleCount / 2; i++)
	{
		// Store weights for the positive and negative taps.
		weight = GaussianDistribution(i + 1.0f, 0.0f, 4.0f);

		weights[i * 2 + 1] = D3DXVECTOR4(weight, weight, weight, weight);
		weights[i * 2 + 2] = D3DXVECTOR4(weight, weight, weight, weight);

		totalWeights += weight * 2.0f;

		float sampleOffset = i * 2.0f + 1.5f;

		D3DXVECTOR4 delta(dx* sampleOffset, dy* sampleOffset, 0.0f, 0.0f);

		tcOffset[i * 2 + 1] = delta + offset;
		tcOffset[i * 2 + 2] = -delta + offset;
	}

	for (unsigned int i = 0; i < sampleCount; i++)
	{
		weights[i] *= 1.0f/totalWeights;
	}
}


void BlurSSAO(r3dScreenBuffer *SourceTex, r3dScreenBuffer *TempTex)
{
	PostFX_UpdateResources();

	r3dRenderer->StartRenderSimple(0);
	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);

	extern r3dScreenBuffer*	gBuffer_Normal;
	r3dRenderer->SetTex( DepthBuffer->Tex, 1 );
	r3dRenderer->SetTex( gBuffer_Normal->Tex, 2 );

	r3dSetFiltering( R3D_BILINEAR, 2 );

	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);

	r3dSetRestoreFSQuadVDecl setRestoreVDECL; (void)setRestoreVDECL;

	int Normals = !!r_ssao_blur_w_normals->GetInt() ;


	D3DXVECTOR4 vconst = D3DXVECTOR4( 0.5f / r3dRenderer->ScreenW, 0.5f / r3dRenderer->ScreenH, 1.0f, 1.0f );
	r3dRenderer->pd3ddev->SetVertexShaderConstantF(  0, (float *)&vconst,  1 );

	r3dRenderer->SetVertexShader("VS_SSAO"); 

	const SSAOSettings& sts = g_SSAOSettings[ r_ssao_method->GetInt() ];
	const int gaussianBlur = sts.BlurGaussian;

	int tapCount = sts.BlurTapCount;

	// Need to adjust tapCount to odd value, this method doesn't support even count of samples
	if (gaussianBlur && (tapCount % 2 == 0))
		tapCount += 1;

	char SSAOBlurPSName[ 32 ];
	GetSSAOBlurPSName( SSAOBlurPSName, tapCount, 0, Normals, gaussianBlur  );

	D3DXVECTOR4 pconsts[1];

	const int maxTapCount  = SSAOConstraints::MAX_BLUR_TAP_COUNT;

	D3DXVECTOR4 horizontalWeights_offsets[maxTapCount*2];
	D3DXVECTOR4 verticalWeights_offsets[maxTapCount*2];

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED ) );

	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP ) );

	TempTex->Activate();

	// with half scale ssao we don't use stencil so the background
	// is SSAOed. With full scale ssao we use stencil to avoid expensive
	// stuff where sky is, so we have to clear to not let it through.

	DWORD clearVal = R3D_MIN( R3D_MAX( int( r_ssao_clear_val->GetFloat() * 0xff ), 0 ), 0xff );

	clearVal |= clearVal << 8 | clearVal << 16 | clearVal << 24;

	D3D_V( r3dRenderer->pd3ddev->Clear( 0, 0, D3DCLEAR_TARGET, clearVal, r3dRenderer->GetClearZValue(), 0 ) ) ;

	TempTex->Deactivate();

	if (gaussianBlur)
	{
		D3DXVECTOR4 offset(0.5f / SourceTex->Width, 0.5f / SourceTex->Height, 0.0f, 0.0f);
		GetSampleOffsetsGauss(1.0f / SourceTex->Width, 0.0f, horizontalWeights_offsets, horizontalWeights_offsets + tapCount, tapCount, offset); 
		GetSampleOffsetsGauss(0.0f, 1.0f / TempTex->Height, verticalWeights_offsets, verticalWeights_offsets + tapCount, tapCount, offset); 
	}

	for( int i = 0, e = sts.BlurPassCount; i < e; i ++ )
	{
		r3dRenderer->SetPixelShader( SSAOBlurPSName );

		const SSAOSettings& sts = g_SSAOSettings[ r_ssao_method->GetInt() ];

		pconsts[0] = D3DXVECTOR4( -sts.BlurDepthSensitivity, sts.BlurStrength, 1.0f / TempTex->Width, 0.0f );

		r3dRenderer->pd3ddev->SetPixelShaderConstantF(  0, (float *)pconsts,  1 );

		if (gaussianBlur)
		{
			r3dRenderer->pd3ddev->SetPixelShaderConstantF(  2, (float *)horizontalWeights_offsets,  tapCount*2 );
		}

		r3dSetFiltering( gaussianBlur ? R3D_BILINEAR : R3D_POINT, 0 );
		r3dSetFiltering( R3D_POINT, 1 );
		r3dSetFiltering( R3D_POINT, 2 );
		TempTex->Activate();

		r3dRenderer->SetTex( SourceTex->Tex );

		r3dDrawFullScreenQuad( false  );

		TempTex->Deactivate();

		SourceTex->Activate();

		r3dRenderer->SetPixelShader( SSAOBlurPSName );

		pconsts[0] = D3DXVECTOR4( -sts.BlurDepthSensitivity, sts.BlurStrength, 0.0f, 1.0f / r3dRenderer->ScreenH );

		r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float *)pconsts, 1 );

		if (gaussianBlur)
		{
			r3dRenderer->pd3ddev->SetPixelShaderConstantF(  2, (float *)verticalWeights_offsets,  tapCount*2 );
		}

		r3dSetFiltering( gaussianBlur ? R3D_BILINEAR : R3D_POINT, 0 );
		r3dSetFiltering( R3D_POINT, 1 );
		r3dSetFiltering( R3D_POINT, 2 );

		r3dRenderer->SetTex( TempTex->Tex );

		r3dDrawFullScreenQuad( false );

		SourceTex->Deactivate();
	}

	r3dRenderer->SetPixelShader();
	r3dRenderer->SetVertexShader();
	r3dRenderer->EndRenderSimple();
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORWRITEENABLE,	D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | 
																	D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );
}


void FillColorCorrectionTexture( const r3dBezierGradient& xcurve, const r3dBezierGradient& ycurve, const r3dBezierGradient& zcurve, bool hsv, r3dTexture* outTex )
{
	R3D_ENSURE_MAIN_THREAD();

	const int TEX_SIZE = 256; 
	r3d_assert ( outTex );
	r3d_assert ( outTex->GetD3DFormat() == D3DFMT_A8R8G8B8 );
	r3d_assert ( outTex->GetWidth() == TEX_SIZE );
	r3d_assert ( outTex->GetHeight() == 1 );

	byte * pMem;

	pMem = (byte *)outTex->Lock( true );
	r3d_assert ( pMem );

	for ( int i = 0; i < TEX_SIZE; i++ )
	{
		float time = float (i) / float (TEX_SIZE-1);

		float x = xcurve.GetFloatValue ( time );
		float y = ycurve.GetFloatValue ( time );
		float z = zcurve.GetFloatValue ( time );

		if( hsv )
		{
			x = x + 0.5f;
		}

		pMem[0] = (byte)R3D_MIN( R3D_MAX( z * 255.f, 0.f ), 255.f );
		pMem[1] = (byte)R3D_MIN( R3D_MAX( y * 255.f, 0.f ), 255.f );
		pMem[2] = (byte)R3D_MIN( R3D_MAX( x * 255.f, 0.f ), 255.f );
		pMem[3] = 0;

		pMem += 4;
	}

	outTex->Unlock ();
}
