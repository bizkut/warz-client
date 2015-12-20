//=========================================================================
//	Module: PFX_BrightPass.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once
#include "PostFX.h"

//////////////////////////////////////////////////////////////////////////

class PFX_BrightPass : public PostFX
{
	// settings
public:
	struct Settings
	{
		float minimumBrightness;
		Settings(): minimumBrightness(0.0f) {}
	};

	typedef r3dTL::TArray<Settings> SettingsVec;

	// construction/ destruction
public:
	PFX_BrightPass();
	~PFX_BrightPass();	
	void PushSettings( const Settings& sts );

private:
	virtual void InitImpl()								OVERRIDE;
	virtual	void CloseImpl()							OVERRIDE;
	virtual void PrepareImpl(r3dScreenBuffer* dest, r3dScreenBuffer* src)	OVERRIDE;
	virtual void FinishImpl()							OVERRIDE;

	// data
private:

	SettingsVec			mSettings;
};