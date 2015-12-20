#pragma once

#include "PostFX.h"

class PFX_StarShapeBlur : public PostFX
{
	// types
public:
	struct Settings
	{
		float Density;

		Settings()
		: Density(0.5f)
		{}
	};

	// construction/ destruction
public:
	PFX_StarShapeBlur();
	~PFX_StarShapeBlur();

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