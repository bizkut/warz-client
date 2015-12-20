#pragma once

#include "PostFX.h"

class PFX_ExtractBloom : public PostFX
{
public:
	struct Settings
	{
		Settings();

//Bloom
		float		Power;
		float		Threshold;
		r3dColor	MultiplyColor;

// Glow part
		r3dColor	GlowTint;
		float		GlowAmplify;
		float		GlowThreshold ;
	};

	// construction/ destruction
public:
	PFX_ExtractBloom();
	~PFX_ExtractBloom();

	// manipulation/ access
public:
	void			PushSettings(const Settings &settings);
	const Settings& GetDefaultSettings() const;
	void			SetDefaultSettings( const Settings& settings );

	// polymorphism
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest,
								r3dScreenBuffer* src )	OVERRIDE;

	virtual void FinishImpl()							OVERRIDE;

	// data
private:
	Settings mDefaultSettings;
	r3dTL::TArray<Settings> mSettingsArr;

};