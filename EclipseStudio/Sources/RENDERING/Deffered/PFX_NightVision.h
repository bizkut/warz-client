#pragma once

#include "PostFX.h"
#include "PostFXChief.h"

class PFX_NightVision : public PostFX
{
	r3dTexture *colorRemapTex;
public:
	struct Settings
	{
	};

	// construction/ destruction
public:
	PFX_NightVision();
	~PFX_NightVision();

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