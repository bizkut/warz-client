#pragma once

#include "r3dTypeTuples.h"

#include "PostFX.h"

class PFX_Fill : public PostFX
{
public:
	struct Settings
	{
		Settings();

		float4	Value;
		int		ColorWriteMask;
	};

	typedef r3dTL::TArray< Settings > SettingsStack ;

	// construction/ destruction
public:
	explicit PFX_Fill(); 
	~PFX_Fill();

public:
	void PushSettings( const Settings& settings );


	// polymorphism
private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(	r3dScreenBuffer* dest, 
								r3dScreenBuffer* src )	OVERRIDE;
	virtual void FinishImpl()							OVERRIDE;

	// data
private:
	SettingsStack mSettingsStack;
};