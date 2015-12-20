//=========================================================================
//	Module: ExplosionVisualController.cpp
//	Copyright (C) Arktos 2011.
//=========================================================================

#include "r3dpch.h"
#include "r3d.h"
#include "ExplosionVisualController.h"
#include "../../rendering/Deffered/PostFX.h"
#include "../../rendering/Deffered/PostFXChief.h"
#include "../../rendering/Deffered/PFX_ExplosionBlur.h"
#include "../../rendering/Deffered/PFX_ExtractBloom.h"

extern PFX_ExplosionBlur gPFX_ExplosionBlur;
extern PFX_ExtractBloom gPFX_ExtractBloom;
ExplosionVisualController gExplosionVisualController;
extern r3dLightSystem WorldLightSystem;

//-------------------------------------------------------------------------
//	ExplosionParameters
//-------------------------------------------------------------------------

ExplosionVisualController::ExplosionParameters::ExplosionParameters()
: maxStrength(0)
, startTime(0)
, duration(0)
, radius(0)
{
	light.TurnOff();
	light.SetType(R3D_OMNI_LIGHT);
	light.SetColor(255, 201, 14);
	light.bCastShadows = 0;
	WorldLightSystem.Add(&light);
}

//////////////////////////////////////////////////////////////////////////

ExplosionVisualController::ExplosionParameters::ExplosionParameters(const ExplosionVisualController::ExplosionParameters &o)
: maxStrength(o.maxStrength)
, pos(o.pos)
, startTime(o.startTime)
, duration(o.duration)
, light(o.light)
, radius(o.radius)
, brightThreshold(o.brightThreshold)
{
	WorldLightSystem.Add(&light);
}

//////////////////////////////////////////////////////////////////////////

ExplosionVisualController::ExplosionParameters::~ExplosionParameters()
{
	WorldLightSystem.Remove(&light);
}

//-------------------------------------------------------------------------
//	ExplosionVisualController
//-------------------------------------------------------------------------

ExplosionVisualController::ExplosionVisualController()
: maxVisibleDist(30.0f)
, defaultMaxStrength(0.2f)
, defaultDuration(0.2f)
, defaultBrightThreshold(0.1f)
{

}

//////////////////////////////////////////////////////////////////////////

void ExplosionVisualController::AddExplosion(const r3dVector &pos, float radius, float duration, float maxStrength, float brightThreshold)
{
#ifdef FINAL_BUILD
	return; //ptumik: temp disabled again. not working properly in low quality settings
#endif

	ExplosionParameters p;
	p.startTime = r3dGetTime();
	p.maxStrength = maxStrength == 0 ? defaultMaxStrength : maxStrength;
	p.pos = pos;
	p.duration = duration == 0 ? defaultDuration : duration;
	p.radius = radius;
	p.brightThreshold = brightThreshold == 0 ? defaultBrightThreshold : brightThreshold;

	r3dVector ppp(pos);
	ppp.y += 1;
	p.light.Assign(ppp);

	explosions.PushBack(p);
}

//////////////////////////////////////////////////////////////////////////

void ExplosionVisualController::ApplyPostFXExplosionEffects()
{
	float currT = r3dGetTime();

	uint32_t i = 0;
	while (i < explosions.Count())
	{
		ExplosionParameters &p = explosions[i];
		if (currT - p.startTime > p.duration)
		{
			explosions.Erase(i);
			continue;
		}
		++i;
		
		D3DXVECTOR4 v2;
		D3DXVec3Transform(&v2, p.pos.d3dx(), &r3dRenderer->ViewMatrix);
		//	Check if this explosion within the range
		D3DXVECTOR3 v3(v2.x, v2.y, v2.z);
		float explosionDist = D3DXVec3Length(&v3);
		if (explosionDist > maxVisibleDist) continue;

		//	Determine screen space position of explosion.

		D3DXVECTOR4 tmp(p.pos.x, p.pos.y, p.pos.z, 1.0f), v;
		D3DXVec4Transform(&v, &tmp, &r3dRenderer->ViewProjMatrix );
		float sign = 1.0f;
		if (v.w < 0) sign = -1.0f;
		v /= v.w;
		v *= sign;

		v *= 0.5f;
		v += D3DXVECTOR4(0.5f, 0.5f, 0, 0);
		v.y = 1.0f - v.y;

		//	Calculate strength of explosion using time from start, duration and position distance
		float explosionPhase = (currT - p.startTime) / (p.duration);
		float currentStrength = (std::max)(0.0f, (std::min)(1.0f - powf(explosionPhase, 2.0f), 1.0f));

		//	Configure light parameters
		p.light.TurnOn();
		p.light.SetRadius(0.0f, p.radius * currentStrength);

		//	Attenuate strength by distance
		float s = explosionDist / maxVisibleDist;
		currentStrength = powf(currentStrength * (1 - s), 0.5f);
		currentStrength *= p.maxStrength;

		//	Add effect to PostFX chain
		PFX_ExplosionBlur::Settings settings;
		settings.BrightnessThreshold = p.brightThreshold;
		settings.BlurStrength = currentStrength;
		settings.centerX = r3dTL::Clamp(v.x, 0, 1);
		settings.centerY = r3dTL::Clamp(v.y, 0, 1);
		settings.toBlurImage = PostFXChief::RTT_ONEFOURTH0_64BIT;
		gPFX_ExplosionBlur.PushSettings(settings);

		r3dScreenBuffer *blurredBB = g_pPostFXChief->GetBuffer(PostFXChief::RTT_ONEFOURTH0_64BIT);
		g_pPostFXChief->AddGrabScreen(blurredBB, PostFXChief::RTT_PINGPONG_LAST);
		g_pPostFXChief->AddFX(gPFX_ExplosionBlur);
		g_pPostFXChief->AddSwapBuffers();
	}
}

//------------------------------------------------------------------------

void ExplosionVisualController::RemoveAll()
{
	explosions.Clear();
}