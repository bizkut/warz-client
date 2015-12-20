#pragma once

#include "PostFX.h"
#include "PostFXChief.h"

class PFX_ExplosionBlur : public PostFX
{
	// types
public:
	struct Settings
	{
		float BrightnessThreshold;
		float BlurStrength;
		float centerX, centerY;
		PostFXChief::RTType toBlurImage;

		Settings()
		: BrightnessThreshold(0.1f)
		, BlurStrength(0.17f)
		, centerX(0.5f)
		, centerY(0.5f)
		{}
	};

	// construction/ destruction
public:
	PFX_ExplosionBlur();
	~PFX_ExplosionBlur();

public:
	void			PushSettings( const Settings& settings );
	void			SetDefaultSettings( const Settings& settings );
	const Settings&	GetDefaultSettings() const;

	// polymorphism
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest,
								r3dScreenBuffer* src )	OVERRIDE;

	virtual void FinishImpl()							OVERRIDE;

	// data
private:
	r3dTL::TArray<Settings> mSettingsArr;
	Settings mDefaultSettings;

};