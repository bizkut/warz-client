//=========================================================================
//	Module: PFX_LensDirt_ExtractLuma.h
//	Copyright (C) 2011.
//=========================================================================
#pragma once

#include "PostFX.h"


class PFX_LensDirt_ExtractLuma: public PostFX
{
public:
	struct Settings
	{
		float Threshold;

		Settings();
	};

public:
	PFX_LensDirt_ExtractLuma();
	~PFX_LensDirt_ExtractLuma();
public:
	void SetSettings( const Settings& sts );
	const Settings& GetSettings() const;

private:
	virtual void InitImpl();
	virtual	void CloseImpl();
	virtual void PrepareImpl(r3dScreenBuffer* dest, r3dScreenBuffer* src);
	virtual void FinishImpl();

private:
	Settings m_Settings;

};
