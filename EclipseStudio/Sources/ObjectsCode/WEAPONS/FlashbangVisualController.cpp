//=========================================================================
//	Module: FlashbangVisualController.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dpch.h"
#include "r3d.h"
#include "FlashbangVisualController.h"
#include "../../rendering/Deffered/PostFX.h"
#include "../../rendering/Deffered/PostFXChief.h"
#include "../../rendering/Deffered/CommonPostFX.h"

//////////////////////////////////////////////////////////////////////////

#define TEST_SPHERE_RADIUS 0.25f
FlashbangVisualController gFlashbangVisualController;
extern r3dCamera gCam;

//////////////////////////////////////////////////////////////////////////

FlashbangVisualController::FlashbangVisualController()
: startTime(0)
, firstFrame(false)
, visQuery(0)
, isVisible(-1)
, issuePendingOcclusionQuery(false)
{

}

//////////////////////////////////////////////////////////////////////////

FlashbangVisualController::~FlashbangVisualController()
{
	Destroy();
}

//////////////////////////////////////////////////////////////////////////

void FlashbangVisualController::Init()
{
	if (!visQuery && r3dRenderer->SupportsOQ)
		visQuery = r3dD3DQuery::CreateClass(D3DQUERYTYPE_OCCLUSION);
}

//////////////////////////////////////////////////////////////////////////

void FlashbangVisualController::Destroy()
{
	delete visQuery;
	visQuery = 0;
}

//////////////////////////////////////////////////////////////////////////

void FlashbangVisualController::CheckEffectVisibility()
{
	if (!visQuery || isVisible >= 0 || issuePendingOcclusionQuery)
	{
		return;
	}

	DWORD pixelsVisible = 0;
	HRESULT hr = visQuery->GetData(&pixelsVisible, sizeof(pixelsVisible), 0);

	if (hr == S_OK)
	{
		isVisible = pixelsVisible > 0 ? 1 : 0;
	}
}

//////////////////////////////////////////////////////////////////////////

void FlashbangVisualController::IssueVisibilityQuery()
{
	if (!visQuery || !issuePendingOcclusionQuery)
		return;

	issuePendingOcclusionQuery = false;

	//	If camera inside test sphere, skip test entirely
	if ((gCam - effectParams.pos).Length() < TEST_SPHERE_RADIUS)
	{
		isVisible = 1;
		return;
	}

	r3dRenderer->SetMaterial(0);
	r3dSetFwdColorShaders(r3dColor::white);
	r3dRenderer->SetTex(0);

	D3DXMATRIX worldMat, tmp;
	D3DXMatrixTranslation(&worldMat, effectParams.pos.x, effectParams.pos.y, effectParams.pos.z);
	D3DXMatrixScaling(&tmp, TEST_SPHERE_RADIUS, TEST_SPHERE_RADIUS, TEST_SPHERE_RADIUS);
	D3DXMatrixMultiply(&worldMat, &tmp, &worldMat);

	r3dMeshSetShaderConsts( worldMat, NULL );

	r3dDrawGeoSpheresStart();

	DWORD oldCLWE = 0;
	r3dRenderer->pd3ddev->GetRenderState(D3DRS_COLORWRITEENABLE, &oldCLWE);
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, 0);

	HRESULT hr = visQuery->Issue(D3DISSUE_BEGIN);
	r3d_assert(SUCCEEDED(hr));

	r3dDrawGeoSphere();

	hr = visQuery->Issue(D3DISSUE_END);
	r3d_assert(SUCCEEDED(hr));

	r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, oldCLWE);

	r3dDrawGeoSpheresEnd();
	isVisible = -1;
}
//////////////////////////////////////////////////////////////////////////

void FlashbangVisualController::StartEffect(const FlashBangEffectParams &params, float delay)
{
	startTime = r3dGetTime() + delay;

	effectParams.solidColorTimeFraction = params.solidColorTimeFraction * params.duration;
	effectParams.color2StillPictureTimeFraction = params.color2StillPictureTimeFraction * params.duration;
	effectParams.duration = params.duration;
	effectParams.pos = params.pos;

	//	Request pending occlusion query
	issuePendingOcclusionQuery = true;

	firstFrame = true;
}

//////////////////////////////////////////////////////////////////////////

void FlashbangVisualController::ApplyPostFXEffects()
{
	float currT = r3dGetTime();
	CheckEffectVisibility();

	bool notVisible = isVisible == 0;

	if (notVisible || currT > effectParams.duration + startTime || currT < startTime)
		return;

	if (firstFrame)
	{
		g_pPostFXChief->AddFX(gPFX_Copy, PostFXChief::RTT_FLASHBANG_MULTIFRAME_16BIT);
		firstFrame = false;
	}

	PFX_Interpolate::Settings s;
	s.image2 = g_pPostFXChief->GetBuffer(PostFXChief::RTT_FLASHBANG_MULTIFRAME_16BIT)->Tex;

	if (currT < effectParams.solidColorTimeFraction + startTime)
	{
		s.coef.x = 1.0f;
		s.coef.y = 0;
		s.coef.z = 0;
	}
	else if (currT < effectParams.color2StillPictureTimeFraction + startTime)
	{
		float f = effectParams.solidColorTimeFraction;
		s.coef.z = 0;
		s.coef.y = R3D_CLAMP((currT - startTime - f) / (effectParams.color2StillPictureTimeFraction - f), 0.0f, 1.0f); 
		s.coef.x = 1.0f - s.coef.y;
	}
	else
	{
		float f = effectParams.color2StillPictureTimeFraction;
		s.coef.x = 0;
		s.coef.z = R3D_CLAMP((currT - startTime - f) / (effectParams.duration - f), 0.0f, 1.0f);
		s.coef.y = 1.0f - s.coef.z;
	}


	gPFX_Interpolate.PushSettings(s);
	g_pPostFXChief->AddFX(gPFX_Interpolate);
	g_pPostFXChief->AddSwapBuffers();
}

//------------------------------------------------------------------------

void TestFlashBang()
{
	FlashBangEffectParams fbep;
	fbep.duration = 3 ;
	fbep.pos = gCam ;
	gFlashbangVisualController.StartEffect(fbep);
}