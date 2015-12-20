/*
 * Copyright 2008-2012 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */
#include <RendererLight.h>
#include <RendererLightDesc.h>

#include <Renderer.h>

using namespace SampleRenderer;

RendererLight::RendererLight(const RendererLightDesc &desc) :
m_type(desc.type),
m_shadowProjection(45, 1, 0.1f, 100.0f)
{
	m_renderer = 0;
	setColor(desc.color);
	setIntensity(desc.intensity);
	setShadowMap(desc.shadowMap);
	setShadowTransform(desc.shadowTransform);
	setShadowProjection(desc.shadowProjection);
}

RendererLight::~RendererLight(void)
{
	RENDERER_ASSERT(!isLocked(), "Light is locked by a Renderer during release.");
}

void SampleRenderer::RendererLight::release(void)
{
	if (m_renderer) m_renderer->removeLightFromRenderQueue(*this);
	delete this;
}

RendererLight::Type RendererLight::getType(void) const
{
	return m_type;
}

RendererMaterial::Pass RendererLight::getPass(void) const
{
	RendererMaterial::Pass pass = RendererMaterial::NUM_PASSES;
	switch(m_type)
	{
	case TYPE_POINT:       pass = RendererMaterial::PASS_POINT_LIGHT;       break;
	case TYPE_DIRECTIONAL: pass = RendererMaterial::PASS_DIRECTIONAL_LIGHT; break;
	case TYPE_SPOT:        pass = m_shadowMap != NULL ? RendererMaterial::PASS_SPOT_LIGHT : RendererMaterial::PASS_SPOT_LIGHT_NO_SHADOW;        break;
	default: break;
	}
	RENDERER_ASSERT(pass < RendererMaterial::NUM_PASSES, "Unable to compute the Pass for the Light.");
	return pass;
}

const RendererColor &RendererLight::getColor(void) const
{
	return m_color;
}

void RendererLight::setColor(const RendererColor &color)
{
	m_color = color;
}

float RendererLight::getIntensity(void) const
{
	return m_intensity;
}

void RendererLight::setIntensity(float intensity)
{
	RENDERER_ASSERT(intensity >= 0, "Light intensity is negative.");
	if(intensity >= 0)
	{
		m_intensity = intensity;
	}
}

bool RendererLight::isLocked(void) const
{
	return m_renderer ? true : false;
}

RendererTexture2D *RendererLight::getShadowMap(void) const
{
	return m_shadowMap;
}

void RendererLight::setShadowMap(RendererTexture2D *shadowMap)
{
	m_shadowMap = shadowMap;
}

const RendererProjection &RendererLight::getShadowProjection(void) const
{
	return m_shadowProjection;
}

void RendererLight::setShadowProjection(const RendererProjection &shadowProjection)
{
	m_shadowProjection = shadowProjection;
}

