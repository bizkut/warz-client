#pragma once

#include "PostFX.h"

class PFX_BrightnessContrast : public PostFX
{
	// settings
public:
	struct Settings
	{
		Settings();

		float	brightness;
		float	constrast;
	};

	typedef r3dTL::TArray< Settings > SettingsVec;


	// construction/ destruction
public:
	explicit PFX_BrightnessContrast();
	~PFX_BrightnessContrast();	

	// manipulation/ access
public:
	void PushSettings( const Settings& sts );

	// polymorphism
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest, 
								r3dScreenBuffer* src )	OVERRIDE;
	virtual void FinishImpl()							OVERRIDE;

	virtual void PushDefaultSettingsImpl()				OVERRIDE;

	// data
private:

	SettingsVec			mSettings;
};