//=========================================================================
//	Module: PFX_Interpolate.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once
#include "PostFX.h"

//////////////////////////////////////////////////////////////////////////

class PFX_Interpolate : public PostFX
{
	// settings
public:
	struct Settings
	{
		r3dVector coef;
		r3dTexture *image2;
		Settings(): image2(0) {}
	};

	typedef r3dTL::TArray<Settings> SettingsVec;


	// construction/ destruction
public:
	PFX_Interpolate();
	~PFX_Interpolate();	
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