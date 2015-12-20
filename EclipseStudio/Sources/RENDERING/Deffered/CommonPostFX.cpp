#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"
#include "PFX_Fill.h"
#include "CommonPostFX.h"
#include "HUDFilters.h"
#include "GameCommon.h"

PFX_1DLUTColorCorrection	gPFX_1DLUTColorCorrectionRGB( false );
PFX_1DLUTColorCorrection	gPFX_1DLUTColorCorrectionHSV( true );
PFX_3DLUTColorCorrection	gPFX_3DLUTColorCorrection;
PFX_BlackWhiteColorCorrection gPFX_BlackWhiteColorCorrection;
PFX_Copy					gPFX_Copy( R3D_BLEND_NOALPHA, PostFXChief::DEFAULT_COLOR_WRITE_MASK );
PFX_Copy					gPFX_CopyR( R3D_BLEND_NOALPHA, D3DCOLORWRITEENABLE_RED );
PFX_Copy					gPFX_CopyAlpha( R3D_BLEND_NOALPHA, D3DCOLORWRITEENABLE_ALPHA );
PFX_Fill					gPFX_Fill;
PFX_StencilToMask			gPFX_StencilToMask;
PFX_Copy					gPFX_Add( R3D_BLEND_ADD, PostFXChief::DEFAULT_COLOR_WRITE_MASK );
PFX_Copy					gPFX_Blend( R3D_BLEND_ALPHA, PostFXChief::DEFAULT_COLOR_WRITE_MASK );
PFX_Copy					gPFX_AddRGB( R3D_BLEND_ADD, D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED );
PFX_Interpolate				gPFX_Interpolate;

PFX_DOFExtractNear			gPFX_DOFExtractNear;
PFX_StereoReproject			gPFX_StereoReproject;
PFX_AnaglyphComposite		gPFX_AnaglyphComposite;

BlurOptimizedFilterArray gPFX_BlurH;
BlurOptimizedFilterArray gPFX_BlurV;

BlurOptimizedFilterArray gPFX_DoubleBlurH;
BlurOptimizedFilterArray gPFX_DoubleBlurV;

PFX_DirectionalDepthBlur	gPFX_DepthBlurH[ BT_DOF_COUNT ] = 
{
	PFX_DirectionalDepthBlur( 1.0f, 0.0f, 1 ),
	PFX_DirectionalDepthBlur( 1.0f, 0.0f, 2 ),
	PFX_DirectionalDepthBlur( 1.0f, 0.0f, 3 ),
	PFX_DirectionalDepthBlur( 1.0f, 0.0f, 4 ),
	PFX_DirectionalDepthBlur( 1.0f, 0.0f, 5 ),
	PFX_DirectionalDepthBlur( 1.0f, 0.0f, 6 ),
	PFX_DirectionalDepthBlur( 1.0f, 0.0f, 7 ),
	PFX_DirectionalDepthBlur( 1.0f, 0.0f, 8 )
};										  

PFX_DirectionalDepthBlur	gPFX_DepthBlurV[ BT_DOF_COUNT ] =
{
	PFX_DirectionalDepthBlur( 0.0f, 1.0f, 1 ),
	PFX_DirectionalDepthBlur( 0.0f, 1.0f, 2 ),
	PFX_DirectionalDepthBlur( 0.0f, 1.0f, 3 ),
	PFX_DirectionalDepthBlur( 0.0f, 1.0f, 4 ),
	PFX_DirectionalDepthBlur( 0.0f, 1.0f, 5 ),
	PFX_DirectionalDepthBlur( 0.0f, 1.0f, 6 ),
	PFX_DirectionalDepthBlur( 0.0f, 1.0f, 7 ),
	PFX_DirectionalDepthBlur( 0.0f, 1.0f, 8 )
};										  

PFX_DirectionalBloomWithGlowBlur gPFX_DirectionalBloomWithGlowBlurV(1.0f, 0.0f);
PFX_DirectionalBloomWithGlowBlur gPFX_DirectionalBloomWithGlowBlurH(0.0f, 1.0f);

PFX_MixIn gPFX_MixIn ;

#if 0
PFX_BlurEdges				gPFX_BlurEdgesH [ EBT_COUNT ] =
{
	PFX_BlurEdges( 1.f, 0.f, 2 ),
	PFX_BlurEdges( 1.f, 0.f, 3 ),
	PFX_BlurEdges( 1.f, 0.f, 4 ),
	PFX_BlurEdges( 1.f, 0.f, 5 )
};

PFX_BlurEdges				gPFX_BlurEdgesV [ EBT_COUNT ] =
{
	PFX_BlurEdges( 0.f, 1.f, 2 ),
	PFX_BlurEdges( 0.f, 1.f, 3 ),
	PFX_BlurEdges( 0.f, 1.f, 4 ),
	PFX_BlurEdges( 0.f, 1.f, 5 )
};
#endif

PFX_FilmGrain				gPFX_FilmGrain;
PFX_RadialBlur				gPFX_RadialBlur;
PFX_ExtractBloom			gPFX_ExtractBloom;
PFX_ExtractGlow				gPFX_ExtractGlow;
PFX_GodRays					gPFX_GodRays;
PFX_StarShapeBlur			gPFX_StarShapeBlur;
PFX_ObjectMotionBlur		gPFX_ObjectMotionBlur;
PFX_CameraMotionBlur		gPFX_CameraMotionBlur;
PFX_SunGlare				gPFX_SunGlare;
PFX_MLAA_DiscontMap			gPFX_MLAA_DiscontMap;
PFX_MLAA_LineDetectH		gPFX_MLAA_LineDetectH;
PFX_MLAA_LineDetectV		gPFX_MLAA_LineDetectV;
PFX_MLAA_AlphaCalc			gPFX_MLAA_AlphaCalc;
PFX_MLAA_Blend				gPFX_MLAA_Blend;
PFX_Combine					gPFX_Combine( R3D_BLEND_ALPHA, D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_RED );
PFX_Transform				gPFX_Transform ;
PFX_ScopeEffect				gPFX_ScopeEffect;
PFX_ExplosionBlur			gPFX_ExplosionBlur;
PFX_DirectionalStreaks		gPFX_DirectionalStreaks;
PFX_BrightPass				gPFX_BrightPass;
PFX_NightVision				gPFX_NightVision;

PFX_SeedSunThroughStencil	gPFX_SeedSunThroughStencil;
PFX_BrightnessContrast		gPFX_BrightnessContrast;
PFX_GammaCorrect			gPFX_GammaCorrect;
PFX_MinExpand				gPFX_MinExpand;

PFX_FXAA					gPFX_FXAA;
PFX_FXAA_LumPass			gPFX_FXAA_LumPass;

PFX_FilmTone				gPFX_FilmTone ;
PFX_CopyOutput				gPFX_CopyOutput ;
PFX_ConvertToLDR			gPFX_ConvertToLDR ;

PFX_DownSample				gPFX_DownSample;
PFX_CalcLuma				gPFX_CalcLuma ;
PFX_ExposureBlend			gPFX_ExposureBlend ;

PFX_LensDirt				gPFX_LensDirt;

PFX_LensDirt_ExtractLuma	gPFX_LensDirt_ExtractLuma;

ComposeMultibloomArray		gPFX_ComposeMultibloom;


int DepthOfField_NearTaps	= 6;
int DepthOfField_FarTaps	= 8;

float DepthOfField_NearStart	= 0.f;
float DepthOfField_NearEnd		= 1.f;

float DepthOfField_NearAmplify	= 1.0f;

float DepthOfField_FarStart		= 200.f;
float DepthOfField_FarEnd		= 800.f;
float DepthOfField_FarRadius	= 4.f;

float DirectionalStreaks_Strength = 1.0f;
float DirectionalStreaks_Length = 0.5f;

float DirectionalStreaksOnOffCoef = 1.f ;

int LevelDOF				= 0;
int LevelBloom		= 0;
int LevelSunRays	= 0;

int FilmGrainOn		= 0 ;
PFX_FilmGrain::Settings gFilmGrainSettings ;

extern int gSLI_Crossfire_NumGPUs;

//------------------------------------------------------------------------

void InitCommonPostFX()
{
	
	int i = 0 ;
	gPFX_BlurH[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 1.0f, 0.0f, 2 ) ;
	gPFX_BlurH[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 1.0f, 0.0f, 4 );
	gPFX_BlurH[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 1.0f, 0.0f, 6 );
	gPFX_BlurH[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 1.0f, 0.0f, 8 );
	gPFX_BlurH[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 1.0f, 0.0f, 10 );
	gPFX_BlurH[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 1.0f, 0.0f, 12 );
	gPFX_BlurH[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 1.0f, 0.0f, 14 );
	gPFX_BlurH[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 1.0f, 0.0f, 16 );

	i = 0 ;

	gPFX_BlurV[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 0.0f, 1.0f, 2 ) ;
	gPFX_BlurV[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 0.0f, 1.0f, 4 ) ;
	gPFX_BlurV[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 0.0f, 1.0f, 6 ) ;
	gPFX_BlurV[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 0.0f, 1.0f, 8 ) ;
	gPFX_BlurV[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 0.0f, 1.0f, 10 ) ;
	gPFX_BlurV[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 0.0f, 1.0f, 12 ) ;
	gPFX_BlurV[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 0.0f, 1.0f, 14 ) ;
	gPFX_BlurV[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 0.0f, 1.0f, 16 ) ;

	i = 0;
	gPFX_DoubleBlurH[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 2.0f, 0.0f, 2 );
	gPFX_DoubleBlurH[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 2.0f, 0.0f, 4 );
	gPFX_DoubleBlurH[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 2.0f, 0.0f, 6 );
	gPFX_DoubleBlurH[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 2.0f, 0.0f, 8 );
	gPFX_DoubleBlurH[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 2.0f, 0.0f, 10 );
	gPFX_DoubleBlurH[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 2.0f, 0.0f, 12 );
	gPFX_DoubleBlurH[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 2.0f, 0.0f, 14 );
	gPFX_DoubleBlurH[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 2.0f, 0.0f, 16 );

	i = 0;
	gPFX_DoubleBlurV[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 0.0f, 2.0f, 2 );
	gPFX_DoubleBlurV[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 0.0f, 2.0f, 4 );
	gPFX_DoubleBlurV[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 0.0f, 2.0f, 6 );
	gPFX_DoubleBlurV[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 0.0f, 2.0f, 8 );
	gPFX_DoubleBlurV[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 0.0f, 2.0f, 10 );
	gPFX_DoubleBlurV[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 0.0f, 2.0f, 12 );
	gPFX_DoubleBlurV[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 0.0f, 2.0f, 14 );
	gPFX_DoubleBlurV[ i ++ ] = gfx_new PFX_DirectionalBlurOptimized( 0.0f, 2.0f, 16 );

	gPFX_1DLUTColorCorrectionRGB.Init();
	gPFX_1DLUTColorCorrectionHSV.Init();

	gPFX_GodRays.Init();
	gPFX_StarShapeBlur.Init();

	gPFX_MLAA_DiscontMap.Init();
	gPFX_MLAA_LineDetectH.Init();
	gPFX_MLAA_LineDetectV.Init();
	gPFX_MLAA_AlphaCalc.Init();
	gPFX_MLAA_Blend.Init();

	gPFX_ExplosionBlur.Init();
	gPFX_DirectionalStreaks.Init();

	gPFX_DirectionalBloomWithGlowBlurH.Init();
	gPFX_DirectionalBloomWithGlowBlurV.Init();

	for( int i = 0, e = BT_DOF_COUNT; i < e; i ++ )
	{
		gPFX_DepthBlurH[ i ].Init();
		gPFX_DepthBlurV[ i ].Init();
	}

	gPFX_StereoReproject.Init();
	gPFX_FXAA.Init();
	gPFX_FXAA_LumPass.Init();

	gPFX_3DLUTColorCorrection.Init();
	gPFX_BlackWhiteColorCorrection.Init();
	
	gPFX_Copy.Init();
	gPFX_CopyR.Init();
	gPFX_Add.Init();
	gPFX_AddRGB.Init();
	gPFX_Fill.Init();
	gPFX_StencilToMask.Init();
	gPFX_CopyAlpha.Init();
	gPFX_Blend.Init();

	gPFX_FilmGrain.Init();
	gPFX_RadialBlur.Init();
	gPFX_ExtractBloom.Init();
	gPFX_ExtractGlow.Init();

	gPFX_ObjectMotionBlur.Init();
	gPFX_CameraMotionBlur.Init();	

	gPFX_DOFExtractNear.Init();
	gPFX_Combine.Init();
	gPFX_Transform.Init();

	gPFX_SunGlare.Init();
	gPFX_SeedSunThroughStencil.Init();

	gPFX_BrightnessContrast.Init();
	gPFX_GammaCorrect.Init();
	
	gPFX_ScopeEffect.Init();
	
	gPFX_Interpolate.Init();
	
	gPFX_BrightPass.Init();
	gPFX_NightVision.Init();

	for( int i = 0, e = BT_COUNT; i < e; i ++ )
	{
		gPFX_BlurH[ i ]->Init();
		gPFX_BlurV[ i ]->Init();

		gPFX_DoubleBlurH[ i ]->Init();
		gPFX_DoubleBlurV[ i ]->Init();
	}


#if 0
	for( int i = 0, e = EBT_COUNT; i < e; i ++ )
	{
		gPFX_BlurEdgesH[ i ].Init();
	}

	for( int i = 0, e = EBT_COUNT; i < e; i ++ )
	{
		gPFX_BlurEdgesV[ i ].Init();
	}
#endif

	gPFX_MinExpand.Init();
	gPFX_AnaglyphComposite.Init();
	gPFX_MixIn.Init();

	gPFX_FilmTone.Init() ;
	gPFX_CopyOutput.Init() ;
	gPFX_ConvertToLDR.Init() ;

	gPFX_DownSample.Init() ;
	gPFX_CalcLuma.Init() ;
	gPFX_ExposureBlend.Init() ;

	for (int i = 0; i < gPFX_ComposeMultibloom.COUNT; ++i)
	{
		gPFX_ComposeMultibloom[i] = gfx_new PFX_ComposeMultibloom(i);
		gPFX_ComposeMultibloom[i]->Init();
	}

	gPFX_LensDirt.Init();
	gPFX_LensDirt_ExtractLuma.Init();
}

void CloseCommonPostFX()
{
	gPFX_1DLUTColorCorrectionRGB.Close();
	gPFX_1DLUTColorCorrectionHSV.Close();

	gPFX_GodRays.Close();
	gPFX_StarShapeBlur.Close();

	gPFX_MLAA_DiscontMap.Close();
	gPFX_MLAA_LineDetectH.Close();
	gPFX_MLAA_LineDetectV.Close();
	gPFX_MLAA_AlphaCalc.Close();
	gPFX_MLAA_Blend.Close();

	gPFX_ExplosionBlur.Close();
	gPFX_DirectionalStreaks.Close();

	gPFX_DirectionalBloomWithGlowBlurV.Close();
	gPFX_DirectionalBloomWithGlowBlurH.Close();

	for( int i = 0, e = BT_DOF_COUNT; i < e; i ++ )
	{
		gPFX_DepthBlurH[ i ].Close();
		gPFX_DepthBlurV[ i ].Close();
	}

	gPFX_StereoReproject.Close();
	gPFX_FXAA.Close();
	gPFX_FXAA_LumPass.Close();

	gPFX_3DLUTColorCorrection.Close();
	gPFX_BlackWhiteColorCorrection.Close();

	gPFX_Copy.Close();
	gPFX_CopyR.Close();
	gPFX_Add.Close();
	gPFX_Fill.Close();
	gPFX_StencilToMask.Close();
	gPFX_CopyAlpha.Close();
	gPFX_Blend.Close();

	gPFX_FilmGrain.Close();
	gPFX_RadialBlur.Close();
	gPFX_ExtractBloom.Close();
	gPFX_ExtractGlow.Close();

	gPFX_ObjectMotionBlur.Close();
	gPFX_CameraMotionBlur.Close();

	gPFX_DOFExtractNear.Close();
	gPFX_Combine.Close();
	gPFX_Transform.Close();

	gPFX_SunGlare.Close();
	gPFX_SeedSunThroughStencil.Close();

	gPFX_BrightnessContrast.Close();
	gPFX_GammaCorrect.Close();

	gPFX_ScopeEffect.Close();
	gPFX_Interpolate.Close();
	gPFX_BrightPass.Close();
	gPFX_NightVision.Close();

	for( int i = 0, e = BT_COUNT; i < e; i ++ )
	{
		gPFX_BlurH[ i ]->Close();
		SAFE_DELETE( gPFX_BlurH[ i ] );

		gPFX_BlurV[ i ]->Close();
		SAFE_DELETE( gPFX_BlurV[ i ] );

		gPFX_DoubleBlurH[ i ]->Close();
		SAFE_DELETE( gPFX_DoubleBlurH[ i ] );

		gPFX_DoubleBlurV[ i ]->Close();
		SAFE_DELETE( gPFX_DoubleBlurV[ i ] );
	}
	
#if 0	
	for( int i = 0, e = EBT_COUNT; i < e; i ++ )
	{
		gPFX_BlurEdgesH[ i ].Close();
	}

	for( int i = 0, e = EBT_COUNT; i < e; i ++ )
	{
		gPFX_BlurEdgesV[ i ].Close();
	}
#endif

	gPFX_MinExpand.Close();
	gPFX_AnaglyphComposite.Close();

	gPFX_MixIn.Close();

	gPFX_FilmTone.Close() ;
	gPFX_CopyOutput.Close() ;
	gPFX_ConvertToLDR.Close() ;

	gPFX_DownSample.Close() ;
	gPFX_CalcLuma.Close() ;
	gPFX_ExposureBlend.Close() ;

	gPFX_LensDirt.Close();

	for (int i = 0; i < gPFX_ComposeMultibloom.COUNT; ++i)
	{
		gPFX_ComposeMultibloom[i]->Close();
		SAFE_DELETE(gPFX_ComposeMultibloom[i]);
	}

	gPFX_LensDirt_ExtractLuma.Close();
}

void CalcLumaPostFx()
{
	RECT viewport = { 0, 0, 512, 512 } ;

	r3dScreenBuffer* oneOfMany = g_pPostFXChief->GetBuffer( PostFXChief::RTT_DIFFUSE_32BIT ) ;
	int targWidth = (int)oneOfMany->Width ;
	int targHeight = (int)oneOfMany->Height ;

	for( ; viewport.right > targWidth || viewport.bottom > targHeight ; )
	{
		viewport.right /= 2 ;
		viewport.bottom /= 2 ;
	}

#if 0
	PFX_Fill::Settings fsts ;

	fsts.Value = float4( 1, 1, 1, 1 ) ;

	gPFX_Fill.PushSettings( fsts ) ;

	g_pPostFXChief->AddFX( gPFX_Fill, PostFXChief::RTT_TEMP0_64BIT, PostFXChief::RTT_PINGPONG_LAST ) ;
#endif
	g_pPostFXChief->AddFX( gPFX_CalcLuma, PostFXChief::RTT_TEMP0_64BIT, PostFXChief::RTT_PINGPONG_LAST, &viewport ) ;

	int prevWidth = viewport.right ;
	int prevHeight = viewport.bottom ;

	int right = viewport.right / 8 ;
	int bottom = viewport.bottom / 8 ;

	right = R3D_MAX( right, 1 ) ;
	bottom = R3D_MAX( bottom, 1 ) ;

	PostFXChief::RTType dest = PostFXChief::RTT_ONEFOURTH0_64BIT ;
	PostFXChief::RTType src = PostFXChief::RTT_TEMP0_64BIT ;

	// NOTE : sync this 8 with H/V sample count in 
	for( ; ; right /= 8, bottom /= 8 )
	{
		viewport.right = right ;
		viewport.bottom = bottom ;

		viewport.right = R3D_MAX( (int) viewport.right, 1 ) ;
		viewport.bottom = R3D_MAX( (int) viewport.bottom, 1 ) ;

		PFX_DownSample::Settings sts ;

		sts.ColorWriteMask = D3DCOLORWRITEENABLE_RED ;

		r3dScreenBuffer* srcBuf = g_pPostFXChief->GetBuffer( src ) ;

		sts.TexMultiplyU = float( prevWidth ) / srcBuf->Width ;
		sts.TexMultiplyV = float( prevHeight ) / srcBuf->Height ;

		prevWidth = viewport.right ;
		prevHeight = viewport.bottom ;

		sts.TexAddU = 0.0f ;
		sts.TexAddV = 0.0f ;

		gPFX_DownSample.PushSettings( sts ) ;

		if( viewport.right == 1 && viewport.bottom == 1 )
			dest = PostFXChief::RTT_AVG_SCENE_LUMA ;

		g_pPostFXChief->AddFX( gPFX_DownSample, dest, src, &viewport ) ;

		if( src == PostFXChief::RTT_TEMP0_64BIT )
			src = PostFXChief::RTT_ONEFOURTH1_64BIT ;

		R3D_SWAP( dest, src ) ;

		if( viewport.right == 1 && viewport.bottom == 1 )
			break ;
	}
}

static float g_lastRayCast = 0.f;
static int g_lastHitRay = 1;

void AddLensDirtStack()
{
	g_pPostFXChief->AddFX( gPFX_LensDirt_ExtractLuma, PostFXChief::RTT_ONE16_0_64BIT, PostFXChief::RTT_PINGPONG_LAST );

	BlurTaps taps = (BlurTaps)R3D_MIN( R3D_MAX( int( r_lens_dirt_blur_width->GetInt() - 1 ), 0 ), BT_COUNT - 1 );

	for( int i = 0, e = r_lens_dirt_blur_count->GetInt(); i < e; i ++ )
	{
		g_pPostFXChief->AddFX( *gPFX_BlurH[ taps ], PostFXChief::RTT_ONE16_1_64BIT, PostFXChief::RTT_ONE16_0_64BIT );
		g_pPostFXChief->AddFX( *gPFX_BlurV[ taps ], PostFXChief::RTT_ONE16_0_64BIT, PostFXChief::RTT_ONE16_1_64BIT );
	}

	extern r3dCamera gCam;

	WorldLightSystem.FillVisibleArrayWithLights();
	WorldLightSystem.SortVisibleLightsByDistanceTo( gCam );

	gPFX_LensDirt.SetVPMatrix( r3dRenderer->ViewProjMatrix );

	gPFX_LensDirt.PurgeLights();

	r3dPoint3D lightDir = Sun->SunDir.NormalizeTo();

	r3dPoint3D dirLightPos = gCam - lightDir * 512.0f;

	gPFX_LensDirt.AddLight( dirLightPos, Sun->SunColor, Sun->SunLight.Intensity, 0.33f, 1 );
	gPFX_LensDirt.SetSunOn( Sun->IsVisible );

#if 0
	for (int i = 0 ; i < WorldLightSystem.nVisibleLights; ++i )
	{
		r3dLight* l = WorldLightSystem.VisibleLights[ i ];

		if( l->GetType() == R3D_DIRECT_LIGHT )
			continue;

		gPFX_LensDirt.AddLight( *l, l->GetColor(), l->Intensity, l->GetOuterRadius(), 0 );

		if( gPFX_LensDirt.IsFull() )
			break;
	}
#endif

	g_pPostFXChief->AddFX( gPFX_LensDirt );
	g_pPostFXChief->AddSwapBuffers();
}
void AddCalcAvarageLumaStackMultiGPU()
{
	bool IsSceneExposure0IsSource();
	bool is0Source = IsSceneExposure0IsSource();

	PostFXChief::RTType rt0 = PostFXChief::RTT_SCENE_EXPOSURE0 ;
	PostFXChief::RTType rt1 = PostFXChief::RTT_SCENE_EXPOSURE1 ;

	PostFXChief::RTType srcRT = is0Source ? rt0 : rt1;
	PostFXChief::RTType dstRT = is0Source ? rt1 : rt0;

	r_last_exposure_source->SetInt(is0Source ? 1 : 0);

	g_pPostFXChief->AddFX( gPFX_ExposureBlend, dstRT, srcRT );
}

void AddCalcAvarageLumaStackSingleGPU()
{
	PostFXChief::RTType exDest = PostFXChief::RTT_SCENE_EXPOSURE0 ;
	PostFXChief::RTType exSrc = PostFXChief::RTT_SCENE_EXPOSURE1 ;

	if( r_last_exposure_source->GetInt() )
	{
		R3D_SWAP( exDest, exSrc ) ;
	}

	g_pPostFXChief->AddFX( gPFX_ExposureBlend, exDest, exSrc ) ;

	r_last_exposure_source->SetInt( !r_last_exposure_source->GetInt() ) ;
}

void AddCalcAvarageLumaStack()
{
	CalcLumaPostFx();

#if ENABLE_MULTI_GPU_OPTIMIZATIONS
 	if (gSLI_Crossfire_NumGPUs > 1)
 	{
 		AddCalcAvarageLumaStackMultiGPU();
 	}
 	else
#endif
	{
		AddCalcAvarageLumaStackSingleGPU();
	}
}

void AddDepthOfFieldStack()
{
	extern int _FAR_DOF;
	extern int _NEAR_DOF;

	if( _FAR_DOF )
	{
		DepthOfField_FarTaps = R3D_MAX( R3D_MIN( DepthOfField_FarTaps, (int)BT_DOF_COUNT ), 1 );

		PFX_DirectionalDepthBlur& blurH = gPFX_DepthBlurH[ DepthOfField_FarTaps - 1 ];
		PFX_DirectionalDepthBlur& blurV = gPFX_DepthBlurV[ DepthOfField_FarTaps - 1 ];

		PFX_DirectionalDepthBlur::Settings sts;

		sts.Start	= DepthOfField_FarStart;
		sts.End		= DepthOfField_FarEnd;
		sts.Reverse	= false;
		sts.UseStencil = false;

		blurH.PushSettings( sts );
		blurV.PushSettings( sts );

		g_pPostFXChief->AddFX( blurH,		PostFXChief::RTT_TEMP0_64BIT	, PostFXChief::RTT_PINGPONG_LAST	);
		g_pPostFXChief->AddFX( blurV,		PostFXChief::RTT_PINGPONG_NEXT	, PostFXChief::RTT_TEMP0_64BIT		);

		g_pPostFXChief->AddSwapBuffers();
	}

	if( _NEAR_DOF )
	{
		DepthOfField_NearTaps = R3D_MAX( R3D_MIN( DepthOfField_NearTaps, (int)BT_DOF_COUNT ), 1 );

		PostFXChief::RTType MASK_RTT = PostFXChief::RTT_ONEFOURTH0_64BIT ;

		// extract near
		{
			PFX_DOFExtractNear::Settings sts;

			sts.NearEnd		= DepthOfField_NearEnd;
			sts.NearStart	= DepthOfField_NearStart;

			gPFX_DOFExtractNear.SetSettings( sts );

			g_pPostFXChief->AddFX( gPFX_DOFExtractNear, MASK_RTT						, PostFXChief::RTT_PINGPONG_LAST	);

			int taps = R3D_MAX( DepthOfField_NearTaps - 1, 1 );

			PFX_DirectionalBlur::Settings bsts;

			bsts.AlphaMul = DepthOfField_NearAmplify;

			gPFX_BlurH[ taps ]->PushSettings( bsts );
			gPFX_BlurV[ taps ]->PushSettings( bsts );

			g_pPostFXChief->AddFX( *gPFX_BlurH[ taps ] , PostFXChief::RTT_ONEFOURTH1_64BIT	, MASK_RTT							);
			g_pPostFXChief->AddFX( *gPFX_BlurV[ taps ] , MASK_RTT							, PostFXChief::RTT_ONEFOURTH1_64BIT	);
		}

		{
			PFX_DirectionalDepthBlur& blurH = gPFX_DepthBlurH[ DepthOfField_NearTaps - 1 ];
			PFX_DirectionalDepthBlur& blurV = gPFX_DepthBlurV[ DepthOfField_NearTaps - 1 ];

			PFX_DirectionalDepthBlur::Settings sts;

			sts.Start	= DepthOfField_NearStart;
			sts.End		= DepthOfField_NearEnd;

			sts.ReverseMask = g_pPostFXChief->GetBuffer( MASK_RTT );
			sts.Reverse		= true;
			
			blurH.PushSettings( sts );
			blurV.PushSettings( sts );

			g_pPostFXChief->AddFX( blurH, PostFXChief::RTT_TEMP0_64BIT		,	PostFXChief::RTT_PINGPONG_LAST	);
			g_pPostFXChief->AddFX( blurV, PostFXChief::RTT_PINGPONG_NEXT	,	PostFXChief::RTT_TEMP0_64BIT	);

			g_pPostFXChief->AddSwapBuffers();
		}
	}
}

void AddBloomStack()
{
	g_pPostFXChief->AddFX( gPFX_ExtractBloom, PostFXChief::RTT_ONEFOURTH0_64BIT, PostFXChief::RTT_PINGPONG_LAST );

	extern int _UsedBloomBlurPasses ;
	extern int _UsedBloomBlurTaps ;

	PFX_Copy::Settings copySts;
	PFX_ComposeMultibloom::Settings sts;
	
	PostFXChief::RTType steps[] =
	{
		PostFXChief::RTT_COUNT, PostFXChief::RTT_ONEFOURTH1_64BIT, PostFXChief::RTT_ONEFOURTH0_64BIT,
		PostFXChief::RTT_ONEFOURTH0_64BIT, PostFXChief::RTT_ONE8_1_64BIT, PostFXChief::RTT_ONE8_0_64BIT,
		PostFXChief::RTT_ONE8_0_64BIT, PostFXChief::RTT_ONE16_1_64BIT, PostFXChief::RTT_ONE16_0_64BIT,
		PostFXChief::RTT_ONE16_0_64BIT, PostFXChief::RTT_ONE32_1_64BIT, PostFXChief::RTT_ONE32_0_64BIT,
		PostFXChief::RTT_ONE32_0_64BIT, PostFXChief::RTT_ONE64_1_64BIT, PostFXChief::RTT_ONE64_0_64BIT
	};

	int cnt = R3D_MIN<int>(_UsedBloomBlurPasses, gPFX_ComposeMultibloom.COUNT);
	cnt = R3D_MAX( cnt, 1 );

	int k = 0;
	for (int i = 0; i < cnt; ++i)
	{
		PostFXChief::RTType r0 = steps[i * 3 + 0];
		PostFXChief::RTType r1 = steps[i * 3 + 1];
		PostFXChief::RTType r2 = steps[i * 3 + 2];

		if (r0 != PostFXChief::RTT_COUNT)
		{
			copySts.TexOffsetX = 1 / g_pPostFXChief->GetBuffer(r0)->Width;
			copySts.TexOffsetY = 1 / g_pPostFXChief->GetBuffer(r0)->Height;
			gPFX_Copy.PushSettings(copySts);
			g_pPostFXChief->AddFX( gPFX_Copy, r2, r0 );

			sts.bloomParts[k++] = r2;
		}
		g_pPostFXChief->AddFX( *gPFX_BlurH[ _UsedBloomBlurTaps ], r1, r2 );
		g_pPostFXChief->AddFX( *gPFX_BlurV[ _UsedBloomBlurTaps ], r2, r1 );

	}

	PFX_ComposeMultibloom *cmb = gPFX_ComposeMultibloom[cnt - 1];
	cmb->PushSettings(sts);

	g_pPostFXChief->AddFX( *cmb, PostFXChief::RTT_PINGPONG_LAST,	PostFXChief::RTT_ONEFOURTH0_64BIT );
}

void AddObjectMotionBlurStack()
{
	g_pPostFXChief->AddFX( *gPFX_BlurH[ BT_9 ],	PostFXChief::RTT_PINGPONG_NEXT_AS_TEMP,	PostFXChief::RTT_AUX_32BIT					);
	g_pPostFXChief->AddFX( *gPFX_BlurV[ BT_9 ],	PostFXChief::RTT_AUX_32BIT,				PostFXChief::RTT_PINGPONG_NEXT_AS_TEMP	);

	g_pPostFXChief->AddFX( gPFX_ObjectMotionBlur );

	g_pPostFXChief->AddSwapBuffers();
}

void AddMLAAStack()
{
	g_pPostFXChief->AddFX( gPFX_MLAA_DiscontMap );

	PostFXChief::RTType first;
	PostFXChief::RTType second;

	if ( r_mlaa_pow2_len->GetInt() % 2 == 0 )
	{
		first = PostFXChief::RTT_MLAA_LINES_H;
		second = PostFXChief::RTT_TEMP0_64BIT;
	} 
	else
	{
		first = PostFXChief::RTT_TEMP0_64BIT;
		second = PostFXChief::RTT_MLAA_LINES_H;
	}
	 
	gPFX_Fill.PushSettings( PFX_Fill::Settings() );
	g_pPostFXChief->AddFX( gPFX_Fill, first );

	gPFX_MLAA_LineDetectH.ResetLevel();
	
	for ( int i = 0; i < r_mlaa_pow2_len->GetInt(); i++ )
	{
		if ( i % 2 == 0 )
			g_pPostFXChief->AddFX( gPFX_MLAA_LineDetectH, second, first );
		else
			g_pPostFXChief->AddFX( gPFX_MLAA_LineDetectH, first, second );
	}

	if ( r_mlaa_pow2_len->GetInt() % 2 == 0 )
		first = PostFXChief::RTT_MLAA_LINES_V;
	else
		second = PostFXChief::RTT_MLAA_LINES_V;

	gPFX_Fill.PushSettings( PFX_Fill::Settings() );
	g_pPostFXChief->AddFX( gPFX_Fill, first );

	gPFX_MLAA_LineDetectV.ResetLevel();

	for ( int i = 0; i < r_mlaa_pow2_len->GetInt(); i++ )
	{
		if ( i % 2 == 0 )
			g_pPostFXChief->AddFX( gPFX_MLAA_LineDetectV, second, first );
		else
			g_pPostFXChief->AddFX( gPFX_MLAA_LineDetectV, first, second );
	}

	g_pPostFXChief->AddFX( gPFX_MLAA_AlphaCalc, PostFXChief::RTT_TEMP0_64BIT, PostFXChief::RTT_PINGPONG_NEXT );

	g_pPostFXChief->AddFX( gPFX_MLAA_Blend );

	g_pPostFXChief->AddSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////

void AddScopeEffectStack()
{

	g_pPostFXChief->AddFX( gPFX_Copy, PostFXChief::RTT_ONEFOURTH0_64BIT );

	g_pPostFXChief->AddFX(*gPFX_BlurV[BT_17], PostFXChief::RTT_ONEFOURTH1_64BIT, PostFXChief::RTT_ONEFOURTH0_64BIT);
	g_pPostFXChief->AddFX(*gPFX_BlurH[BT_17], PostFXChief::RTT_ONEFOURTH0_64BIT, PostFXChief::RTT_ONEFOURTH1_64BIT);
	gPFX_ScopeEffect.SetBlurredSceneImage(g_pPostFXChief->GetBuffer(PostFXChief::RTT_ONEFOURTH0_64BIT));
	g_pPostFXChief->AddFX(gPFX_ScopeEffect);
	g_pPostFXChief->AddSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////

void AddDirectionalStreaksStack(PostFXChief::RTType bloomImageID)
{
	PostFXChief::RTType src = bloomImageID;
	PostFXChief::RTType dst = bloomImageID == PostFXChief::RTT_ONEFOURTH0_64BIT ? PostFXChief::RTT_ONEFOURTH1_64BIT : PostFXChief::RTT_ONEFOURTH0_64BIT;
	PFX_DirectionalStreaks::Settings s;

	s.maxOffset = DirectionalStreaks_Length;
	s.strength = DirectionalStreaks_Strength * DirectionalStreaksOnOffCoef ;
	gPFX_DirectionalStreaks.PushSettings(s);
	g_pPostFXChief->AddFX(gPFX_DirectionalStreaks, dst, src);
	std::swap(src, dst);

	s.maxOffset = s.maxOffset * 3 / 4;
	s.strength = 1.0f;
	gPFX_DirectionalStreaks.PushSettings(s);
	g_pPostFXChief->AddFX(gPFX_DirectionalStreaks, dst, src);
	std::swap(src, dst);

	s.maxOffset /= 2;
	gPFX_DirectionalStreaks.PushSettings(s);
	g_pPostFXChief->AddFX(gPFX_DirectionalStreaks, dst, src);
	std::swap(src, dst);

	s.maxOffset = s.maxOffset * 3 / 4;
	gPFX_DirectionalStreaks.PushSettings(s);
	g_pPostFXChief->AddFX(gPFX_DirectionalStreaks, dst, src);
	std::swap(src, dst);

	g_pPostFXChief->AddFX(gPFX_AddRGB, PostFXChief::RTT_PINGPONG_LAST, src);
}

//////////////////////////////////////////////////////////////////////////

void AddNightVisionStack()
{
	g_pPostFXChief->AddFX(gPFX_NightVision);
	g_pPostFXChief->AddSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////

void AddFXAAStack()
{
	g_pPostFXChief->AddFX(gPFX_FXAA_LumPass, PostFXChief::RTT_TEMP0_64BIT, PostFXChief::RTT_PINGPONG_LAST);
	g_pPostFXChief->AddFX(gPFX_FXAA, PostFXChief::RTT_PINGPONG_NEXT, PostFXChief::RTT_TEMP0_64BIT);
	g_pPostFXChief->AddSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////

void AddSeparateEyesStereoReprojectionStack(PostFXChief::RTType srcImage, PostFXChief::RTType dstLeft, PostFXChief::RTType dstRight)
{
	D3DXMATRIX leftView, leftProj, rightView, rightProj ;
	float offsetLeft, offsetRight ;
	CalcStereoViewProj( 1.f, &leftView, &leftProj, &rightView, &rightProj, &offsetLeft, &offsetRight );

	//------------------------------------------------------------------------
	// LEFT eye
	{
		PFX_StereoReproject::Settings sts ;

		sts.ViewMtx = leftView ;
		sts.ProjMtx = leftProj ;
		sts.ViewDelta = offsetLeft ;
		sts.IsRight = false ;

		gPFX_StereoReproject.PushSettings( sts );

		g_pPostFXChief->AddFX( gPFX_StereoReproject, dstLeft, srcImage );
	}

	//------------------------------------------------------------------------
	// RIGHT eye
	{
		PFX_StereoReproject::Settings sts ;

		sts.ViewMtx = rightView ;
		sts.ProjMtx = rightProj ;
		sts.ViewDelta = offsetRight ;
		sts.IsRight = true ;

		gPFX_StereoReproject.PushSettings( sts );

		g_pPostFXChief->AddFX( gPFX_StereoReproject, dstRight, srcImage );
	}
}

//------------------------------------------------------------------------

void InitSSAO();
void InitColorCorrection();

void InitPostFX()
{
	r3d_assert(g_pPostFXChief == 0);
	g_pPostFXChief = gfx_new PostFXChief();
	g_pPostFXChief->Init();

	InitSSAO();
	InitColorCorrection();

	InitCommonPostFX();
	InitHudFilters();
}

void ClosePostFX()
{
	CloseCommonPostFX();
	g_pPostFXChief->Close();
	SAFE_DELETE(g_pPostFXChief);
}

//------------------------------------------------------------------------

ColorCorrectionSettings::ColorCorrectionSettings()
{
	for( size_t i = 0, e = sizeof RGBCurves / sizeof RGBCurves[ 0 ] ; i < e; i ++ )
	{
		RGBCurves[ i ].ClampMin = 0.f ;
		RGBCurves[ i ].ClampMax = 1.f ;
	}

	for( size_t i = 0, e = sizeof RGBCurves / sizeof RGBCurves[ 0 ] ; i < e; i ++ )
	{
		HSVCurves[ i ].ClampMin = 0.f ;
		HSVCurves[ i ].ClampMax = 1.f ;
	}

	HSVCurves[ 0 ].ClampMin = -0.5f ;
	HSVCurves[ 0 ].ClampMax = +0.5f ;
}

//------------------------------------------------------------------------

