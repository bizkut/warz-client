/*
 * Copyright 2009-2011 NVIDIA Corporation.  All rights reserved.
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
#ifndef D3D11_RENDERER_TEXTURE_COMMON_H
#define D3D11_RENDERER_TEXTURE_COMMON_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_DIRECT3D11)

#include <RendererTexture.h>
#include <RendererTextureDesc.h>
#include <SamplePlatform.h>

#include "D3D11RendererTraits.h"


namespace SampleRenderer
{

enum TEXTURE_BIND_FLAGS
{
	BIND_NONE     =  0,
	BIND_VERTEX   =  1 << D3DTypes::SHADER_VERTEX,
	BIND_GEOMETRY =  1 << D3DTypes::SHADER_GEOMETRY,
	BIND_PIXEL    =  1 << D3DTypes::SHADER_PIXEL,
	BIND_HULL     =  1 << D3DTypes::SHADER_HULL,
	BIND_DOMAIN   =  1 << D3DTypes::SHADER_DOMAIN,
	BIND_ALL      = (1 << D3DTypes::NUM_SHADER_TYPES) - 1
};

PX_INLINE PxU32 getBindFlags(D3DType d3dType) { return 1 << d3dType; }

PX_INLINE DXGI_FORMAT getD3D11TextureFormat(RendererTexture::Format format)
{
	DXGI_FORMAT dxgiFormat = static_cast<DXGI_FORMAT>(SampleFramework::SamplePlatform::platform()->getD3D11TextureFormat(format));
	RENDERER_ASSERT(dxgiFormat != DXGI_FORMAT_UNKNOWN, "Unable to convert to D3D11 Texture Format.");
	return dxgiFormat;
}

PX_INLINE D3D11_FILTER getD3D11TextureFilter(RendererTexture::Filter filter)
{
	D3D11_FILTER d3dFilter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	switch (filter)
	{
	case RendererTexture2D::FILTER_NEAREST:
		d3dFilter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		break;
	case RendererTexture2D::FILTER_LINEAR:
		d3dFilter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;
	case RendererTexture2D::FILTER_ANISOTROPIC:
		d3dFilter = D3D11_FILTER_ANISOTROPIC;
		break;
	default:
		RENDERER_ASSERT(0, "Unable to convert to D3D11 Filter mode.");
	}
	return d3dFilter;
}

PX_INLINE D3D11_TEXTURE_ADDRESS_MODE getD3D11TextureAddressing(RendererTexture2D::Addressing addressing)
{
	D3D11_TEXTURE_ADDRESS_MODE d3dAddressing = (D3D11_TEXTURE_ADDRESS_MODE)0;
	switch (addressing)
	{
	case RendererTexture2D::ADDRESSING_WRAP:
		d3dAddressing = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case RendererTexture2D::ADDRESSING_CLAMP:
		d3dAddressing = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	case RendererTexture2D::ADDRESSING_MIRROR:
		d3dAddressing = D3D11_TEXTURE_ADDRESS_MIRROR;
		break;
	}
	RENDERER_ASSERT(d3dAddressing != 0, "Unable to convert to D3D11 Addressing mode.");
	return d3dAddressing;
}

}

#endif // #if defined(RENDERER_ENABLE_DIRECT3D11)

#endif
