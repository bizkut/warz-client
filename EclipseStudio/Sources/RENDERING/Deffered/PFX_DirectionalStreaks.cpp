//=========================================================================
//	Module: PFX_DirectionalStreaks.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"
#include "GameCommon.h"
#include "PFX_DirectionalStreaks.h"

//////////////////////////////////////////////////////////////////////////

PFX_DirectionalStreaks::PFX_DirectionalStreaks()
: Parent(this)
, psID(-1)
, direction(1.0f, 0.0f, 0.0f)
, prevAddressU(D3DTADDRESS_CLAMP)
, prevAddressV(D3DTADDRESS_CLAMP)
{

}

//////////////////////////////////////////////////////////////////////////

PFX_DirectionalStreaks::~PFX_DirectionalStreaks()
{

}

//////////////////////////////////////////////////////////////////////////

void PFX_DirectionalStreaks::InitImpl()
{
	psID = r3dRenderer->GetPixelShaderIdx("PS_DIRECTIONAL_STREAKS");
	mData.PixelShaderID = psID;
}

//////////////////////////////////////////////////////////////////////////


void PFX_DirectionalStreaks::CloseImpl()
{

}

//////////////////////////////////////////////////////////////////////////

void PFX_DirectionalStreaks::PushSettings(const Settings& settings)
{
	settingsVec.PushBack(settings);
}

//////////////////////////////////////////////////////////////////////////

void PFX_DirectionalStreaks::PrepareImpl(r3dScreenBuffer* /*dest*/, r3dScreenBuffer* /*src*/)
{
	if (settingsVec.Count() > 0)
	{
		const Settings &s = settingsVec[0];
		D3DXVECTOR4 psParams[5];
		psParams[0].x = -s.maxOffset * direction.x;
		psParams[0].y = -s.maxOffset * direction.y;
		psParams[0].z = -s.maxOffset / 4 * 3 * direction.x;
		psParams[0].w = -s.maxOffset / 4 * 3 * direction.y;
		psParams[1].x = -s.maxOffset / 2 * direction.x;
		psParams[1].y = -s.maxOffset / 2 * direction.y;
		psParams[1].z = -s.maxOffset / 4 * direction.x;
		psParams[1].w = -s.maxOffset / 4 * direction.y;
		psParams[2] = -psParams[0];
		psParams[3] = -psParams[1];
		psParams[4].x = s.strength;
		psParams[4].y = 0;
		psParams[4].z = 0;
		psParams[4].w = 0;

		r3dRenderer->pd3ddev->SetPixelShaderConstantF(0, &psParams[0].x, _countof(psParams));
		settingsVec.Erase(0);

		D3D_V( r3dRenderer->pd3ddev->GetSamplerState(0, D3DSAMP_ADDRESSU, &prevAddressU));
		D3D_V( r3dRenderer->pd3ddev->GetSamplerState(0, D3DSAMP_ADDRESSV, &prevAddressV));

		D3D_V( r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER));
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER));
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0));

		r3dSetFiltering(R3D_BILINEAR, 0);
	}
}

//////////////////////////////////////////////////////////////////////////

void PFX_DirectionalStreaks::FinishImpl()
{
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, prevAddressU));
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, prevAddressV));

	r3dSetFiltering(g_pPostFXChief->GetZeroTexStageFilter(), 0);
}

