//=========================================================================
//	Module: NightVisionController.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once
#include "../RENDERING/Deffered/PFX_ExtractBloom.h"
#include "../RENDERING/Deffered/PFX_FilmGrain.h"

//////////////////////////////////////////////////////////////////////////

class NightVisionController
{
	bool isOn;
	PFX_ExtractBloom::Settings origBloomSettings;
	PFX_FilmGrain::Settings origFilmGrainSettings;

	int origFilmGrainState;
	int oldLevelFilmGrainState;
	int origBlurBloomMagnitude;

	float origDirStreaksStrength;
	float newDirStreaksStrength;
	float origDirStreaksLum;
	float newDirStreaksLum;
	//	Is configuration of controller was loaded.
	bool isLoaded;

	void CommitSettings();

public:
	NightVisionController();
	~NightVisionController();

	void On();
	void Off();
};

extern NightVisionController gNightVisionController;