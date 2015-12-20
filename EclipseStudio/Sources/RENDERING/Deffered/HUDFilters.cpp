//=========================================================================
//	Module: HUDFilters.cpp
//	Created by Roman E. Marchenko <roman.marchenko@gmail.com>
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"
#include "HUDFilters.h"

//////////////////////////////////////////////////////////////////////////

HUDFilterSettings gHUDFilterSettings[HUDFilter_Total];

//------------------------------------------------------------------------

void HUDFilterSettings::SetAllColorCorrectionTexturesTo( const r3dString& name )
{
	for( int i = 0, e = r3dAtmosphere::SKY_PHASE_COUNT; i < e ; i ++ )
	{
		colorCorrectionTextureNames[ i ] = name;
	}
}

//------------------------------------------------------------------------

int HUDFilterSettings::IsTextureUsedAsColorCorrection( const r3dString& name )
{
	for( int i = 0, e = r3dAtmosphere::SKY_PHASE_COUNT; i < e ; i ++ )
	{
		if( !stricmp( colorCorrectionTextureNames[ i ].c_str(), name.c_str() ) )
			return 1;
	}

	return 0;
}

//------------------------------------------------------------------------

void HUDFilterSettings::DeleteColorCorrectionTextures()
{
	for( int i = 0, e = r3dAtmosphere::SKY_PHASE_COUNT; i < e ; i ++ )
	{
		r3dRenderer->DeleteTexture( colorCorrectionTextures[ i ] );
		colorCorrectionTextures[ i ] = NULL;
	}
}

//------------------------------------------------------------------------

void HUDFilterSettings::LoadColorCorrectionTextures()
{
	for( int i = 0, e = r3dAtmosphere::SKY_PHASE_COUNT; i < e ; i ++ )
	{
		char fullPath[512];
		void GetCCLUT3DTextureFullPath( char (&buffer)[512], const char* name );
		GetCCLUT3DTextureFullPath( fullPath, colorCorrectionTextureNames[ i ].c_str() );
		colorCorrectionTextures[ i ] = r3dRenderer->LoadTexture( fullPath );
	}
}

//------------------------------------------------------------------------

void HUDFilterSettings::FixColorCorrectionTextureNames()
{
	for( int i = 0, e = colorCorrectionTextureNames.COUNT; i < e ; i ++ )
	{
		if( !colorCorrectionTextureNames[ i ].Length() )
		{
			colorCorrectionTextureNames[ i ] = "default.dds";
		}
	}
}

//------------------------------------------------------------------------

void HUDFilterSettings::GetCurrentColorCorrection( r3dTexture** oLerpFrom, r3dTexture** oLerpTo, float* oLerpFactor )
{
	void GetAdjecantSkyPhasesAndLerpT(	r3dAtmosphere::SkyPhase *oPhase0, r3dAtmosphere::SkyPhase *oPhase1,	float* oLerpT );

	r3dAtmosphere::SkyPhase phase0, phase1;

	GetAdjecantSkyPhasesAndLerpT( &phase0, &phase1, oLerpFactor );

	*oLerpFrom = colorCorrectionTextures[ phase0 ];
	*oLerpTo = colorCorrectionTextures[ phase1 ];
}

//------------------------------------------------------------------------

int HUDFilterSettings::HasActiveColorCorrection()
{
	r3dTexture* tex0, * tex1;
	float lerpT;

	GetCurrentColorCorrection( &tex0, &tex1, &lerpT );

	return tex0 || tex1;
}

//////////////////////////////////////////////////////////////////////////

void InitHudFilters()
{
	gHUDFilterSettings[ HUDFilter_CameraDrone ].bloomBlurPasses = 2;
	gHUDFilterSettings[ HUDFilter_CameraDrone ].filmGrain = 1;
	gHUDFilterSettings[ HUDFilter_CameraDrone ].filmGrainSettings.GrainScale = 10.0f;
	gHUDFilterSettings[ HUDFilter_CameraDrone ].filmGrainSettings.Strength = 0.5f;
	gHUDFilterSettings[ HUDFilter_CameraDrone ].filmGrainSettings.FPS = 24.0f; ///////
	gHUDFilterSettings[ HUDFilter_CameraDrone ].SetAllColorCorrectionTexturesTo( "cameradrone.dds" );	
	gHUDFilterSettings[ HUDFilter_CameraDrone ].directionalStreaksOnOffCoef = 1.0f;
	gHUDFilterSettings[ HUDFilter_CameraDrone ].enableColorCorrection = true;
	gHUDFilterSettings[ HUDFilter_CameraDrone ].overrideHDRControls = 1;
	gHUDFilterSettings[ HUDFilter_CameraDrone ].hdrExposureBias = -0.84f;
	gHUDFilterSettings[ HUDFilter_CameraDrone ].overrideAmbientAndIntensity = 1;
	gHUDFilterSettings[ HUDFilter_CameraDrone ].overrideAmbient = r3dColor::white;
	gHUDFilterSettings[ HUDFilter_CameraDrone ].overrideIntensity = 0.21;

	gHUDFilterSettings[ HUDFilter_Default ].directionalStreaksOnOffCoef = 1.0f ;

	gHUDFilterSettings[HUDFilter_NightVision].enableColorCorrection = true;
	gHUDFilterSettings[HUDFilter_NightVision].filmGrain = 1 ;
}
