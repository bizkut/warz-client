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
#ifndef GXM_RENDERER_UTILS_H
#define GXM_RENDERER_UTILS_H

#include <RendererConfig.h>
#include <RendererColor.h>

#if defined(RENDERER_ENABLE_LIBGXM)

static void convertToSce(sce::Vectormath::Scalar::Aos::Vector4 &scecolor, const SampleRenderer::RendererColor &color)
{
	const float inv255 = 1.0f / 255.0f;
	scecolor = sce::Vectormath::Scalar::Aos::Vector4(color.r*inv255, color.g*inv255, color.b*inv255, color.a*inv255);
}

static void convertToSce(float *scevec, const physx::PxVec3 &vec)
{
	scevec[0] = vec.x;
	scevec[1] = vec.y;
	scevec[2] = vec.z;
	scevec[3] = 0.0f;
}

static void convertToSce(sce::Vectormath::Scalar::Aos::Matrix4 &dxmat, const physx::PxMat44 &mat)
{
	physx::PxMat44 mat44 = mat.getTranspose();
	memcpy((physx::PxF32*)&dxmat, mat44.front(), 4 * 4 * sizeof (float));	
}

static void convertToSce(sce::Vectormath::Scalar::Aos::Matrix4 &dxmat, const SampleRenderer::RendererProjection &mat)
{
	float temp[16];
	mat.getColumnMajor44(temp);
	for(physx::PxU32 r=0; r<4; r++)
		for(physx::PxU32 c=0; c<4; c++)
		{
			dxmat[r][c] = temp[c*4+r];
		}
}

#endif

#endif
