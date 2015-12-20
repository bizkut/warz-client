//=========================================================================
//	Module: FlashbangVisualController.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

/**	Flash bang visual parameters. */
struct FlashBangEffectParams 
{
	/**	Solid color display time. In fraction [0..1] of total effect duration. */
	float solidColorTimeFraction;

	/**	Transition form solid color to still picture. In fraction [0..1] of total effect duration. */
	float color2StillPictureTimeFraction;

	/**	Grenade world position. */
	r3dVector pos;

	/**	Rest of the time, till effect ends it will merge from still picture to normal game frame. */

	/**	Total effect time. In seconds. */
	float duration;

	FlashBangEffectParams()
	: solidColorTimeFraction(0.4f)
	, color2StillPictureTimeFraction(0.7f)
	, duration(10.0f)
	, pos(0, 0, 0)
	{}
};

//////////////////////////////////////////////////////////////////////////

class FlashbangVisualController
{
	/**	Effect start time. */
	float startTime;

	/**	First frame of effect animation flag. Needed for memorizing current picture. */
	bool firstFrame;

	/**	Effect parameters. */
	FlashBangEffectParams effectParams;

	/**	Visibility query. */
	r3dD3DQuery *visQuery;

	/**	Check effect visibility. See isVisible. */
	void CheckEffectVisibility();

	/**	Result of last successful query. -1 if no successful results till this time. */
	int isVisible;

	/**	Occlusion query status. */
	bool issuePendingOcclusionQuery;

public:
	FlashbangVisualController();
	~FlashbangVisualController();

	/**	Issue visibility query. */
	void IssueVisibilityQuery();

	/**	Need to create query object in right place. */
	void Init();

	/**	Need to create query object in right place. */
	void Destroy();

	/**
	* Start visual effect.
	* @param dur Duration of effect in seconds.
	* @param str Initial strength of effect in range [0..1]
	*/
	void StartEffect(const FlashBangEffectParams &params, float delay = 0.1f);

	/**	Add visual effects into post process chain. */
	void ApplyPostFXEffects();
};

//////////////////////////////////////////////////////////////////////////

extern FlashbangVisualController gFlashbangVisualController;