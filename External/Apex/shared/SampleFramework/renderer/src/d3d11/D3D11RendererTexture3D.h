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
#ifndef D3D11_RENDERER_TEXTURE_3D_H
#define D3D11_RENDERER_TEXTURE_3D_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_DIRECT3D11)

#include <RendererTexture.h>

#include "D3D11Renderer.h"
#include "D3D11RendererTextureCommon.h"

namespace SampleRenderer
{

class D3D11RendererTexture3D : public RendererTexture3D, public D3D11RendererResource
{
	friend class D3D11RendererTarget;
	friend class D3D11RendererSpotLight;
public:
	D3D11RendererTexture3D(ID3D11Device& d3dDevice, ID3D11DeviceContext& d3dDeviceContext, const RendererTexture3DDesc& desc);
	virtual ~D3D11RendererTexture3D(void);

public:
	virtual void* lockLevel(PxU32 level, PxU32& pitch);
	virtual void  unlockLevel(PxU32 level);

	void bind(PxU32 samplerIndex, PxU32 flags = BIND_PIXEL);

	virtual void select(PxU32 stageIndex)
	{
		bind(stageIndex);
	}

private:
	virtual void onDeviceLost(void);
	virtual void onDeviceReset(void);

	void loadTextureDesc(const RendererTexture3DDesc&);
	void loadSamplerDesc(const RendererTexture3DDesc&);
	void loadResourceDesc(const RendererTexture3DDesc&);

private:
	ID3D11Device&               m_d3dDevice;
	ID3D11DeviceContext&        m_d3dDeviceContext;
	ID3D11Texture3D*            m_d3dTexture;
	D3D11_TEXTURE3D_DESC        m_d3dTextureDesc;

	ID3D11SamplerState*         m_d3dSamplerState;
	D3D11_SAMPLER_DESC          m_d3dSamplerDesc;

	ID3D11ShaderResourceView*       m_d3dSRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC m_d3dSRVDesc;

	PxU8**                      m_data;
	D3D11_SUBRESOURCE_DATA*     m_resourceData;
};

} // namespace SampleRenderer

#endif // #if defined(RENDERER_ENABLE_DIRECT3D11)
#endif
