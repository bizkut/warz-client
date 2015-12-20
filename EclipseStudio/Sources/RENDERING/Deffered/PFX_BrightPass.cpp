//=========================================================================
//	Module: PFX_BrightPass.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"
#include "PostFXChief.h"
#include "PFX_BrightPass.h"

//------------------------------------------------------------------------

PFX_BrightPass::PFX_BrightPass()
: Parent(this)
{

}

//------------------------------------------------------------------------

PFX_BrightPass::~PFX_BrightPass()
{

}

//------------------------------------------------------------------------

void
PFX_BrightPass::PushSettings(const Settings& sts)
{
	mSettings.PushBack(sts);
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_BrightPass::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_BRIGHT_PASS" );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_BrightPass::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_BrightPass::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* /*src*/ ) /*OVERRIDE*/
{
	r3d_assert(mSettings.Count());
	if (mSettings.Count() == 0) return;

	const Settings& sts = mSettings[ 0 ];

	D3DXVECTOR4 psParams(sts.minimumBrightness, 0.0f, 0.0f, 0.0f);
	D3D_V(r3dRenderer->pd3ddev->SetPixelShaderConstantF(0, &psParams.x, 1));

	mSettings.Erase(0);
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_BrightPass::FinishImpl() /*OVERRIDE*/
{
}

//------------------------------------------------------------------------
