//=========================================================================
//	Module: PFX_Interpolate.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"
#include "PostFXChief.h"
#include "PFX_Interpolate.h"

//------------------------------------------------------------------------

PFX_Interpolate::PFX_Interpolate()
: Parent(this)
{

}

//------------------------------------------------------------------------

PFX_Interpolate::~PFX_Interpolate()
{

}

//------------------------------------------------------------------------

void
PFX_Interpolate::PushSettings(const Settings& sts)
{
	mSettings.PushBack(sts);
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_Interpolate::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_INTERPOLATE" );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_Interpolate::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_Interpolate::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* /*src*/ ) /*OVERRIDE*/
{
	r3d_assert(mSettings.Count());

	const Settings& sts = mSettings[ 0 ];

	r3dRenderer->SetTex(sts.image2, PostFXChief::FREE_TEX_STAGE_START);

	D3DXVECTOR4 psParams(sts.coef.x, sts.coef.y, sts.coef.z, 0.0f);
	D3D_V(r3dRenderer->pd3ddev->SetPixelShaderConstantF(0, &psParams.x, 1));

	mSettings.Erase(0);
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_Interpolate::FinishImpl() /*OVERRIDE*/
{
}

//------------------------------------------------------------------------
