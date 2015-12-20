//=========================================================================
//	Module: PFX_FXAA.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"
#include "PFX_FXAA.h"

//////////////////////////////////////////////////////////////////////////

PFX_FXAA::PFX_FXAA()
: Parent(this)
, subpixQuality(0.75f)
, edgeThreshold(0.166f)
, edgeThresholdMin(0.0833f)
{

}

//////////////////////////////////////////////////////////////////////////

PFX_FXAA::~PFX_FXAA()
{

}

//////////////////////////////////////////////////////////////////////////

void PFX_FXAA::InitImpl()
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx("PS_FXAA");
}

//////////////////////////////////////////////////////////////////////////

void PFX_FXAA::CloseImpl()
{

}

//////////////////////////////////////////////////////////////////////////

void PFX_FXAA::PrepareImpl(r3dScreenBuffer* dest, r3dScreenBuffer* src)
{
	float consts[8] =
	{
		1.0f / src->Width, 1.0f / src->Height, 0, 0,
		subpixQuality, edgeThreshold, edgeThresholdMin
	};

	mData.TexTransform[2] = 0.5f / src->Width;
	mData.TexTransform[3] = 0.5f / src->Height;

	r3dRenderer->pd3ddev->GetSamplerState(0, D3DSAMP_MIPFILTER, &mipFilter);
	r3dRenderer->pd3ddev->GetSamplerState(0, D3DSAMP_MINFILTER, &minFilter);
	r3dRenderer->pd3ddev->GetSamplerState(0, D3DSAMP_MAGFILTER, &magFilter);

	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(0, consts, _countof(consts) / 4);
}

//////////////////////////////////////////////////////////////////////////

void PFX_FXAA::FinishImpl()
{
	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_MIPFILTER, mipFilter);
	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_MINFILTER, minFilter);
	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_MAGFILTER, magFilter);
}

//-------------------------------------------------------------------------
//	PFX_FXAA_LumPass
//-------------------------------------------------------------------------

PFX_FXAA_LumPass::PFX_FXAA_LumPass()
: Parent(this)
{

}

//////////////////////////////////////////////////////////////////////////

void PFX_FXAA_LumPass::InitImpl()
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx("PS_FXAA_LumPass");
}

//////////////////////////////////////////////////////////////////////////

void PFX_FXAA_LumPass::CloseImpl()
{

}

//////////////////////////////////////////////////////////////////////////

void PFX_FXAA_LumPass::FinishImpl()
{

}

//////////////////////////////////////////////////////////////////////////

void PFX_FXAA_LumPass::PrepareImpl(r3dScreenBuffer* dest, r3dScreenBuffer* src)
{
	mData.TexTransform[2] = 0.5f / src->Width;
	mData.TexTransform[3] = 0.5f / src->Height;
}
