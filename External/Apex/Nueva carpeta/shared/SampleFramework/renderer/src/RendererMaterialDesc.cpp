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
#include <RendererMaterialDesc.h>

using namespace SampleRenderer;

RendererMaterialDesc::RendererMaterialDesc(void)
{
	type               = RendererMaterial::TYPE_UNLIT;
	alphaTestFunc      = RendererMaterial::ALPHA_TEST_ALWAYS;
	alphaTestRef       = 0;
	blending           = false;
	instanced		   = false;
	srcBlendFunc       = RendererMaterial::BLEND_ZERO;
	dstBlendFunc       = RendererMaterial::BLEND_ZERO;
	geometryShaderPath = 0;
	hullShaderPath     = 0;
	domainShaderPath   = 0;
	vertexShaderPath   = 0;
	fragmentShaderPath = 0;
}

bool RendererMaterialDesc::isValid(void) const
{
	bool ok = true;
	if(type >= RendererMaterial::NUM_TYPES) ok = false;
	if(alphaTestRef < 0 || alphaTestRef > 1) ok = false;
	// Note: Lighting and blending may not be properly supported, but that 
	//    shouldn't crash the system, so this check is disabled for now
	// if(blending && type != RendererMaterial::TYPE_UNLIT) ok = false;
	if(geometryShaderPath || domainShaderPath || fragmentShaderPath)
	{
		// Note: These should be completely optional! Disabling for now.
		//RENDERER_ASSERT(0, "Not implemented!");
		//ok = false;
	}
	if(!vertexShaderPath)   ok = false;
	if(!fragmentShaderPath) ok = false;
	return ok;
}
