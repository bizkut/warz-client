//=========================================================================
//	Module: NightVisionController.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dpch.h"
#include "r3d.h"
#include "NightVisionController.h"
#include "../RENDERING/Deffered/CommonPostFX.h"

//////////////////////////////////////////////////////////////////////////

#if 0

NightVisionController gNightVisionController;
extern int _BloomBlurMagnitude;
extern int _NightVisionBloomBlurMagnitude ;
extern float DirectionalStreaks_LuminanceThreshold;
extern float DirectionalStreaks_Strength;
extern PFX_ExtractBloom::Settings gNightVisionBloomSettings ;
extern PFX_FilmGrain::Settings gNightVisionFilmGrainSettings ;


//////////////////////////////////////////////////////////////////////////

NightVisionController::NightVisionController()
: isOn(false)
, origFilmGrainState(0)
, oldLevelFilmGrainState(0)
, isLoaded(false)
, origBlurBloomMagnitude(0)
, origDirStreaksLum(0)
, newDirStreaksLum(0)
, origDirStreaksStrength(0)
, newDirStreaksStrength(0)
{

}

//////////////////////////////////////////////////////////////////////////

NightVisionController::~NightVisionController()
{

}

//////////////////////////////////////////////////////////////////////////

void NightVisionController::On()
{
	if (isOn) return;
	isOn = true;

	//	Memorize current affected post FX settings.
	origBloomSettings = gPFX_ExtractBloom.GetDefaultSettings();
	origFilmGrainSettings = gPFX_FilmGrain.GetSettings();
	origBlurBloomMagnitude = _BloomBlurMagnitude;
	origDirStreaksLum = DirectionalStreaks_LuminanceThreshold;
	origDirStreaksStrength = DirectionalStreaks_Strength;

	if (!isLoaded)
	{
		newDirStreaksLum = origDirStreaksLum;
		newDirStreaksStrength = origDirStreaksStrength;
	}
	isLoaded = true;

	//	Enable film grain
	origFilmGrainState = r_film_grain->GetInt();
	r_film_grain->SetInt(1);
	oldLevelFilmGrainState = LevelFilmGrain;
	LevelFilmGrain = 1;

	//	Modify default settings
	gPFX_ExtractBloom.SetDefaultSettings( gNightVisionBloomSettings );
	gPFX_FilmGrain.SetSettings( gNightVisionFilmGrainSettings );

	_BloomBlurMagnitude = _NightVisionBloomBlurMagnitude;
	DirectionalStreaks_Strength = newDirStreaksStrength;
	DirectionalStreaks_LuminanceThreshold = newDirStreaksLum;

	r_night_vision_mode->SetInt(1);
}

//////////////////////////////////////////////////////////////////////////

void NightVisionController::Off()
{
	if (!isOn) return;
	isOn = false;

	CommitSettings();

	gPFX_ExtractBloom.SetDefaultSettings(origBloomSettings);
	gPFX_FilmGrain.SetSettings(origFilmGrainSettings);

	r_film_grain->SetInt(origFilmGrainState);
	r_night_vision_mode->SetInt(0);
	LevelFilmGrain = oldLevelFilmGrainState;
	_BloomBlurMagnitude = origBlurBloomMagnitude;
	DirectionalStreaks_LuminanceThreshold = origDirStreaksLum;
	DirectionalStreaks_Strength = origDirStreaksStrength;
}

//////////////////////////////////////////////////////////////////////////

void NightVisionController::CommitSettings()
{
	newBloomSettings = gPFX_ExtractBloom.GetDefaultSettings();
	newFilmGrainSettings = gPFX_FilmGrain.GetSettings();
	newBlurBloomMagnitude = _BloomBlurMagnitude;
	newDirStreaksLum = DirectionalStreaks_LuminanceThreshold;
	newDirStreaksStrength = DirectionalStreaks_Strength;
}

#endif