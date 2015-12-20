//=========================================================================
//	Module: PFX_FXAA.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once
#include "PostFX.h"

//////////////////////////////////////////////////////////////////////////

class PFX_FXAA: public PostFX
{
public:
	PFX_FXAA();
	~PFX_FXAA();

	float subpixQuality;
	float edgeThreshold;
	float edgeThresholdMin;

private:
	virtual void InitImpl();
	virtual	void CloseImpl();
	virtual void PrepareImpl(r3dScreenBuffer* dest, r3dScreenBuffer* src);
	virtual void FinishImpl();

	DWORD magFilter;
	DWORD mipFilter;
	DWORD minFilter;
};

//////////////////////////////////////////////////////////////////////////

class PFX_FXAA_LumPass: public PostFX
{
public:
	PFX_FXAA_LumPass();

private:
	virtual void InitImpl();
	virtual	void CloseImpl();
	virtual void PrepareImpl(r3dScreenBuffer* dest, r3dScreenBuffer* src);
	virtual void FinishImpl();
};
