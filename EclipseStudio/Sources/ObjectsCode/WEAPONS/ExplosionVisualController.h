//=========================================================================
//	Module: ExplosionVisualController.h
//	Copyright (C) Arktos 2011.
//=========================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

class ExplosionVisualController
{
	/**	Explosion parameters structure. */
	struct ExplosionParameters
	{
		/**	Maximum explosion strength. */
		float maxStrength;

		/**	World space explosion position. */
		r3dVector pos;

		/**	Explosion start time. */
		float startTime;

		/**	Explosion duration. */
		float duration;

		/**	Explosion light. */
		r3dLight light;

		/**	Radius of explosion. */
		float radius;

		/**	Bright threshold. */
		float brightThreshold;

		ExplosionParameters();
		ExplosionParameters(const ExplosionParameters &o);
		~ExplosionParameters();
	};

	/**	Max effect visible distance. */
	float maxVisibleDist;

	/**	Array of explosions. */
	r3dTL::TArray<ExplosionParameters> explosions;

	/**	Default strength. */
	float defaultMaxStrength;

	/**	Default duration. */
	float defaultDuration;

	/**	Default bright threshold. */
	float defaultBrightThreshold;

public:
	ExplosionVisualController();

	/**	Add new explosion that happened in world coordinates pos with world space radius. */
	void AddExplosion(const r3dVector &pos, float radius, float duration = 0, float maxStrength = 0, float brightThreshold = 0);

	/**	Set maximum distance from camera, where post process explosion effect still visible. */
	void SetMaxVisibleDistance(float dist) { maxVisibleDist = dist; }
	float GetMaxVisibleDistance() const { return maxVisibleDist; }

	void SetDefaultDuration(float dur) { defaultDuration = dur; }
	void SetDefaultMaxStrength(float str) { defaultMaxStrength = str; }
	float GetDefaultMaxStrength() const { return defaultMaxStrength; }
	float GetDefaultDuration() const { return defaultDuration; }
	float GetDefaultBrightThreshold() const { return defaultBrightThreshold; }
	void SetDefaultBrightThreshold(float thr) { defaultBrightThreshold = thr; }

	/**	Add explosion effects into post process chain. */
	void ApplyPostFXExplosionEffects();

	void RemoveAll();
};

//////////////////////////////////////////////////////////////////////////

extern ExplosionVisualController gExplosionVisualController;
