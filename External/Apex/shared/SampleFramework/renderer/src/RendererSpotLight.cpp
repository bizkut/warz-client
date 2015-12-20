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
#include <RendererSpotLight.h>
#include <RendererSpotLightDesc.h>

using namespace SampleRenderer;

RendererSpotLight::RendererSpotLight(const RendererSpotLightDesc &desc) :
	RendererLight(desc)
{
	setPosition(desc.position);
	setDirection(desc.direction);
	setRadius(desc.innerRadius, desc.outerRadius);
	setCone(desc.innerCone, desc.outerCone);
}

RendererSpotLight::~RendererSpotLight(void)
{

}

const PxVec3 &RendererSpotLight::getPosition(void) const
{
	return m_position;
}

void RendererSpotLight::setPosition(const PxVec3 &pos)
{
	m_position = pos;
}

const PxVec3 &RendererSpotLight::getDirection(void) const
{
	return m_direction;
}

void RendererSpotLight::setDirection(const PxVec3 &dir)
{
	RENDERER_ASSERT(dir.magnitudeSquared() >= 0.1f, "Trying to give Direction Light invalid Direction value.");
	if(dir.magnitudeSquared() >= 0.1f)
	{
		m_direction = dir;
		m_direction.normalize();
	}
}

PxF32 RendererSpotLight::getInnerRadius(void) const
{
	return m_innerRadius;
}

PxF32 RendererSpotLight::getOuterRadius(void) const
{
	return m_outerRadius;
}

void RendererSpotLight::setRadius(PxF32 innerRadius, PxF32 outerRadius)
{
	RENDERER_ASSERT(innerRadius>=0 && innerRadius<=outerRadius, "Invalid Spot Light radius values.");
	if(innerRadius>=0 && innerRadius<=outerRadius)
	{
		m_innerRadius = innerRadius;
		m_outerRadius = outerRadius;
	}
}

PxF32 RendererSpotLight::getInnerCone(void) const
{
	return m_innerCone;
}

PxF32 RendererSpotLight::getOuterCone(void) const
{
	return m_outerCone;
}

void RendererSpotLight::setCone(PxF32 innerCone, PxF32 outerCone)
{
	RENDERER_ASSERT(innerCone<=1 && innerCone>=outerCone, "Invalid Spot Light cone values.");
	if(innerCone<=1 && innerCone>=outerCone)
	{
		m_innerCone = innerCone;
		m_outerCone = outerCone;
	}
}
