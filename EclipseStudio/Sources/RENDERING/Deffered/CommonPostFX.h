#pragma once

#include "TimeGradient.h"
#include "BezierGradient.h"

#include "PFX_1DLUTColorCorrection.h"
#include "PFX_Copy.h"
#include "PFX_DirectionalBlur.h"
#include "PFX_FilmGrain.h"
#include "PFX_RadialBlur.h"
#include "PFX_ExtractBloom.h"
#include "PFX_ExtractGlow.h"
#include "PFX_GodRays.h"
#include "PFX_StarShapeBlur.h"
#include "PFX_SunGlare.h"
#include "PFX_ObjectMotionBlur.h"
#include "PFX_CameraMotionBlur.h"
#include "PFX_3DLUTColorCorrection.h"
#include "PFX_BlackWhiteColorCorrection.h"
#include "PFX_MLAA.h"
#include "PFX_BlurEdges.h"
#include "PFX_DOFExtractNear.h"
#include "PFX_Combine.h"
#include "PFX_SeedSunThroughStencil.h"
#include "PFX_BrightnessContrast.h"
#include "PFX_GammaCorrect.h"
#include "PFX_StencilToMask.h"
#include "PFX_Fill.h"
#include "PFX_MinExpand.h"
#include "PFX_ScopeEffect.h"
#include "PFX_ExplosionBlur.h"
#include "PFX_StereoReproject.h"
#include "PFX_AnaglyphComposite.h"
#include "PFX_Transform.h"
#include "PFX_DirectionalStreaks.h"
#include "PFX_Interpolate.h"
#include "PFX_BrightPass.h"
#include "PFX_MixIn.h"
#include "PFX_NightVision.h"
#include "PFX_FXAA.h"
#include "PFX_FilmTone.h"
#include "PFX_CopyOutput.h"
#include "PFX_ConvertToLDR.h"
#include "PFX_DownSample.h"
#include "PFX_CalcLuma.h"
#include "PFX_ExposureBlend.h"
#include "PFX_ComposeMultibloom.h"
#include "PFX_LensDirt.h"
#include "PFX_LensDirt_ExtractLuma.h"

extern PFX_1DLUTColorCorrection			gPFX_1DLUTColorCorrectionRGB;
extern PFX_1DLUTColorCorrection			gPFX_1DLUTColorCorrectionHSV;
extern PFX_3DLUTColorCorrection			gPFX_3DLUTColorCorrection;
extern PFX_BlackWhiteColorCorrection	gPFX_BlackWhiteColorCorrection;
extern PFX_Copy							gPFX_Copy;
extern PFX_Copy							gPFX_CopyR;
extern PFX_Copy							gPFX_Add;
extern PFX_StereoReproject				gPFX_StereoReproject;
extern PFX_AnaglyphComposite			gPFX_AnaglyphComposite;

enum BlurTaps
{
	BT_5,
	BT_9,
	BT_13,
	BT_17,
	BT_21,
	BT_25,
	BT_29,
	BT_33,
	BT_COUNT
};

enum BlurDOFTaps
{
	BT_DOF_3,
	BT_DOF_5,
	BT_DOF_7,
	BT_DOF_9,
	BT_DOF_11,
	BT_DOF_13,
	BT_DOF_15,
	BT_DOF_17,
	BT_DOF_COUNT
};

enum EdgeBlurTaps
{
	EBT_3,
	EBT_5,
	EBT_7,
	EBT_9,
	EBT_COUNT
};

typedef r3dTL::TFixedArray< PFX_DirectionalBlurOptimized*, BT_COUNT > BlurOptimizedFilterArray ;

extern BlurOptimizedFilterArray gPFX_BlurH;
extern BlurOptimizedFilterArray gPFX_BlurV;

extern BlurOptimizedFilterArray gPFX_DoubleBlurH;
extern BlurOptimizedFilterArray gPFX_DoubleBlurV;

typedef r3dTL::TFixedArray< PFX_ComposeMultibloom*, 5 > ComposeMultibloomArray ;
extern ComposeMultibloomArray gPFX_ComposeMultibloom;

extern PFX_FilmGrain			gPFX_FilmGrain;
extern PFX_RadialBlur			gPFX_RadialBlur;
extern PFX_ExtractBloom			gPFX_ExtractBloom;
extern PFX_ExtractGlow			gPFX_ExtractGlow;
extern PFX_GodRays				gPFX_GodRays;
extern PFX_StarShapeBlur		gPFX_StarShapeBlur;
extern PFX_ObjectMotionBlur		gPFX_ObjectMotionBlur;
extern PFX_CameraMotionBlur		gPFX_CameraMotionBlur;
extern PFX_MLAA_DiscontMap		gPFX_MLAA_DiscontMap;
extern PFX_MLAA_LineDetectH		gPFX_MLAA_LineDetectH;
extern PFX_MLAA_LineDetectV		gPFX_MLAA_LineDetectV;
extern PFX_MLAA_AlphaCalc		gPFX_MLAA_AlphaCalc;
extern PFX_MLAA_Blend			gPFX_MLAA_Blend;

extern PFX_DOFExtractNear		gPFX_DOFExtractNear;

extern PFX_Combine				gPFX_Combine;
extern PFX_Transform			gPFX_Transform ;

#if 0
extern PFX_BlurEdges			gPFX_BlurEdgesH [ EBT_COUNT ];
extern PFX_BlurEdges			gPFX_BlurEdgesV [ EBT_COUNT ];
#endif

extern PFX_SunGlare				gPFX_SunGlare;

extern PFX_DirectionalStreaks		gPFX_DirectionalStreaks;
extern PFX_SeedSunThroughStencil	gPFX_SeedSunThroughStencil;
extern PFX_BrightnessContrast		gPFX_BrightnessContrast;
extern PFX_GammaCorrect				gPFX_GammaCorrect;
extern PFX_StencilToMask			gPFX_StencilToMask;
extern PFX_Fill						gPFX_Fill; 
extern PFX_Interpolate				gPFX_Interpolate;

extern PFX_MinExpand				gPFX_MinExpand;

extern PFX_FilmTone					gPFX_FilmTone ;
extern PFX_CopyOutput				gPFX_CopyOutput ;
extern PFX_ConvertToLDR				gPFX_ConvertToLDR ;

extern PFX_DirectionalBloomWithGlowBlur gPFX_DirectionalBloomWithGlowBlurV;
extern PFX_DirectionalBloomWithGlowBlur gPFX_DirectionalBloomWithGlowBlurH;

extern PFX_MixIn gPFX_MixIn ;
extern PFX_FXAA gPFX_FXAA;
extern PFX_FXAA_LumPass gPFX_FXAA_LumPass;

extern PFX_ScopeEffect gPFX_ScopeEffect;

extern PFX_DownSample gPFX_DownSample ;

extern PFX_CalcLuma gPFX_CalcLuma ;

extern PFX_ExposureBlend gPFX_ExposureBlend ;

extern PFX_LensDirt gPFX_LensDirt;

extern PFX_LensDirt_ExtractLuma gPFX_LensDirt_ExtractLuma;

extern int LevelDOF;
extern int LevelBloom;
extern int LevelSunRays;

extern int FilmGrainOn ;
extern PFX_FilmGrain::Settings gFilmGrainSettings ;

void AddCalcAvarageLumaStack();
void AddDepthOfFieldStack();
void AddBloomStack();
void AddObjectMotionBlurStack();
void AddMLAAStack();
void AddScopeEffectStack();
void AddDirectionalStreaksStack(PostFXChief::RTType bloomImageID);
void AddNightVisionStack();
void AddFXAAStack();
void AddLensDirtStack();

void AddSeparateEyesStereoReprojectionStack(PostFXChief::RTType srcImage, PostFXChief::RTType dstLeft, PostFXChief::RTType dstRight);

enum SSAOMethod
{
	SSM_REF,
	SSM_DEFAULT,
	SSM_HQ,
	SSM_COUNT
};

struct SSAOSettings
{
	float 	Radius;
	float 	DepthRange;
	float 	Brightness;
	float 	Contrast;
	float 	BlurDepthSensitivity;
	float 	BlurStrength;
	float 	RadiusExpandStart;
	float 	RadiusExpandCoef;

	float	TemporalTolerance;
	float	TemporalHistoryDepth;

	int		DetailPathEnable;

	float	DetailStrength;
	float	DetailRadius;
	float	DetailDepthRange;

	float	DetailRadiusExpandStart;
	float	DetailRadiusExpandCoef;
	float	DetailFadeOut;

	int		BlurTapCount;
	int		BlurPassCount;

 	int		BlurGaussian;
} extern g_SSAOSettings[ SSM_COUNT ];

struct SSAOConstraints
{
	float MinRadius,					MaxRadius;
	float MinDepthRange,				MaxDepthRange;
	float MinBrightness,				MaxBrightness;
	float MinContrast,					MaxContrast;
	float MinBlurDepthSensitivity,		MaxBlurDepthSensitivity;
	float MinBlurStrength,				MaxBlurStrength;
	float MinRadiusExpandStart,			MaxRadiusExpandStart;
	float MinRadiusExpandCoef,			MaxRadiusExpandCoef;
	float MinDetailStrength,			MaxDetailStrength;
	float MinDetailRadius,				MaxDetailRadius;
	float MinDetailDepthRange,			MaxDetailDepthRange;
	float MinDetailRadiusExpandStart,	MaxDetailRadiusExpandStart;
	float MinDetailRadiusExpandCoef,	MaxDetailRadiusExpandCoef;
	float MinDetailFadeOut,				MaxDetailFadeOut;

	// NOTE : keep in sync with SSAO_Blur.hls and with GetSSAOBlurPSName ( ..BLUR_TAP_VALUES )
	static const int	MAX_BLUR_TAP_COUNT	= 7;
	static const int	MIN_BLUR_TAP_COUNT	= 2;

	static const int	MIN_BLUR_PASS_COUNT = 1;
	static const int	MAX_BLUR_PASS_COUNT = 5;

	void Restrict( SSAOSettings& sts );

} extern g_SSAOConstraints[ SSM_COUNT ];

const char* SSAOMethodToString( SSAOMethod method );
SSAOMethod StringToSSAOMethod( const char* szName );

void GetSSAOBlurPSName( char(&buf)[32], int tapCount, int scaled, int normals, int gaussianBlur);
void FillSSAOBlurMacros( ShaderMacros& oMacro, int tapCount, int scaled, int normals, int gaussianBlur );

void RenderSSAOEffect();
void RenderLUT1DColorCorrection( r3dScreenBuffer* SourceTex, bool hsv );
void RenderRadialBlur( r3dScreenBuffer* SourceTex );
void CopyScreen( r3dScreenBuffer* SourceTex );

struct ColorCorrectionSettings
{
	r3dBezierGradient 	RGBCurves[3];
	r3dBezierGradient 	HSVCurves[3];

	enum Scheme
	{
		CCS_USE_RGB_CURVES,
		CCS_USE_HSV_CURVES,
		CCS_CUSTOM_3DLUT,
		CCS_USE_RGB_HSV_CURVES,
		CCS_COUNT

	}					scheme;

	int					uiScheme;
	int					uiTexDim;

	void	ResetCurvesRGB()
	{
		for( int i = 0, e = 3; i < e; i ++ )
		{
			g_ColorCorrectionSettings.RGBCurves[ i ].Reset();
			g_ColorCorrectionSettings.RGBCurves[ i ].Values[ 0 ].val[0] = 0.0f;
			g_ColorCorrectionSettings.RGBCurves[ i ].Values[ 1 ].val[0] = 1.0f;
		}
	}

	void	ResetCurvesHSV()
	{
		for( int i = 0, e = 3; i < e; i ++ )
		{
			g_ColorCorrectionSettings.HSVCurves[ i ].Reset();
			g_ColorCorrectionSettings.HSVCurves[ i ].Values[ 0 ].val[0] = 0.0f;
			g_ColorCorrectionSettings.HSVCurves[ i ].Values[ 1 ].val[0] = 1.0f;
		}

		g_ColorCorrectionSettings.HSVCurves[ 0 ].Values[ 0 ].val[0] = 0.0f;
		g_ColorCorrectionSettings.HSVCurves[ 0 ].Values[ 1 ].val[0] = 0.0f;
	}

	ColorCorrectionSettings();

} extern g_ColorCorrectionSettings;

extern int DepthOfField_NearTaps;
extern int DepthOfField_FarTaps;

extern float DepthOfField_NearStart;
extern float DepthOfField_NearEnd;

extern float DepthOfField_NearAmplify;

extern float DepthOfField_FarStart;
extern float DepthOfField_FarEnd;

void FillColorCorrectionTexture( const r3dBezierGradient& xcurve, const r3dBezierGradient& ycurve, const r3dBezierGradient& zcurve, bool hsv, r3dTexture* outTex );

void InitPostFX();
void ClosePostFX();

static const UINT FS_VERTEX_SIZE = 8;

#define SSAO_ALT_NUM_RAYS	16
#define SSAO_LW_NUM_RAYS	8
