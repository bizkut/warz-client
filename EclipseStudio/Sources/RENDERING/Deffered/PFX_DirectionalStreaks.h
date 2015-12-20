//=========================================================================
//	Module: PFX_DirectionalStreaks.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once

#include "PostFX.h"

//////////////////////////////////////////////////////////////////////////

class PFX_DirectionalStreaks: public PostFX
{
	int psID;

public:
	struct Settings
	{
		float maxOffset;
		float strength;
		Settings(): maxOffset(0), strength(1.0f) {};
	};

	r3dVector direction;
	DWORD prevAddressU;
	DWORD prevAddressV;

	typedef r3dTL::TArray<Settings> SettingsVec;

	PFX_DirectionalStreaks();
	~PFX_DirectionalStreaks();
	void PushSettings(const Settings& settings);

private:
	virtual void InitImpl();
	virtual	void CloseImpl();
	virtual void PrepareImpl(r3dScreenBuffer* dest, r3dScreenBuffer* src);
	virtual void FinishImpl();

	SettingsVec settingsVec;
};
