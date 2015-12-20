//=========================================================================
//	Module: HUDFilters.h
//	Created by Roman E. Marchenko <roman.marchenko@gmail.com>
//	Copyright (C) 2011.
//=========================================================================

#pragma once
#include "PFX_ExtractBloom.h"
#include "PFX_FilmGrain.h"

#include "r3dAtmosphere.h"

#include "CommonPostFX.h"

//////////////////////////////////////////////////////////////////////////

enum HUDFilters
{
	HUDFilter_Default = 0,
	HUDFilter_NightVision,
	HUDFilter_CameraDrone,
	HUDFilter_UAV,
	HUDFilter_C130,
	HUDFilter_Laser,
	HUDFilter_Total
};

//////////////////////////////////////////////////////////////////////////

struct HUDFilterSettings
{
	PFX_ExtractBloom::Settings bloomSettings;
	PFX_FilmGrain::Settings filmGrainSettings;
	int bloomBlurPasses;
	BlurTaps bloomBlurTaps;
	float directionalStreaksOnOffCoef;
	int filmGrain;

	r3dAtmosphere::SkyPhaseTextureNames		colorCorrectionTextureNames;
	r3dAtmosphere::SkyPhaseTextures			colorCorrectionTextures;

	int enableColorCorrection;

	int overrideHDRControls ;
	float hdrExposureBias ;

	int			overrideAmbientAndIntensity;
	r3dColor	overrideAmbient;
	float		overrideIntensity;

	HUDFilterSettings()
	: bloomBlurPasses(0)
	, bloomBlurTaps(BT_17)
	, directionalStreaksOnOffCoef(0)
	, filmGrain(0)
	, enableColorCorrection(0)
	, overrideHDRControls(0)
	, hdrExposureBias(0.f)
	, overrideAmbientAndIntensity( 0 )
	, overrideAmbient( r3dColor::black )
	, overrideIntensity( 1.0f )
	{
		SetAllColorCorrectionTexturesTo( "default.dds" );
	}

	void SetAllColorCorrectionTexturesTo( const r3dString& name );
	int IsTextureUsedAsColorCorrection( const r3dString& name );
	void DeleteColorCorrectionTextures();
	void LoadColorCorrectionTextures();
	void FixColorCorrectionTextureNames();
	void GetCurrentColorCorrection( r3dTexture** oLerpFrom, r3dTexture** oLerpTo, float* oLerpFactor );
	int HasActiveColorCorrection();
};

//////////////////////////////////////////////////////////////////////////

extern HUDFilterSettings gHUDFilterSettings[HUDFilter_Total];

//////////////////////////////////////////////////////////////////////////

void InitHudFilters();