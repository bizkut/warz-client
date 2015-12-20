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

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_DIRECT3D11)

#include "D3D11RendererTexture2D.h"
#include "D3D11RendererMemoryMacros.h"

#include <RendererTexture2DDesc.h>

using namespace SampleRenderer;

D3D11RendererTexture2D::D3D11RendererTexture2D(ID3D11Device& d3dDevice, ID3D11DeviceContext& d3dDeviceContext, const RendererTexture2DDesc& desc) :
	RendererTexture2D(desc),
	m_d3dDevice(d3dDevice),
	m_d3dDeviceContext(d3dDeviceContext),
	m_d3dTexture(NULL),
	m_d3dSamplerState(NULL),
	m_d3dSRV(NULL),
	m_d3dRTV(NULL),
	m_d3dDSV(NULL)
{
	loadTextureDesc(desc);
	onDeviceReset();
}

D3D11RendererTexture2D::~D3D11RendererTexture2D(void)
{
	dxSafeRelease(m_d3dTexture);
	dxSafeRelease(m_d3dSamplerState);
	dxSafeRelease(m_d3dSRV);
	dxSafeRelease(m_d3dRTV);
	dxSafeRelease(m_d3dDSV);

	if (m_data)
	{
		for (PxU32 i = 0; i < getNumLevels(); i++)
		{
			delete [] m_data[i];
		}
		delete [] m_data;
	}
	if (m_resourceData)
	{
		delete [] m_resourceData;
	}
}

void* D3D11RendererTexture2D::lockLevel(PxU32 level, PxU32& pitch)
{
	void* buffer = 0;
	RENDERER_ASSERT(level < getNumLevels(), "Level out of range!");
	if (level < getNumLevels())
	{
		buffer = m_data[level];
		pitch  = getFormatNumBlocks(getWidth() >> level, getFormat()) * getBlockSize();
	}
	return buffer;
}

void D3D11RendererTexture2D::unlockLevel(PxU32 level)
{
	RENDERER_ASSERT(level < getNumLevels(), "Level out of range!");

	if (m_d3dTexture && level < getNumLevels())
	{
		PxU32 w = getLevelDimension(getWidth(),  level);
		PxU32 h = getLevelDimension(getHeight(), level);
		m_d3dDeviceContext.UpdateSubresource(m_d3dTexture,
		                                     level,
		                                     NULL,
		                                     m_data[level],
		                                     getFormatNumBlocks(w, getFormat()) * getBlockSize(),
		                                     computeImageByteSize(w, h, 1, getFormat()));
	}
}

void D3D11RendererTexture2D::bind(PxU32 samplerIndex, PxU32 flags)
{
	if (flags)
	{
		if (m_d3dSRV)
		{
			if (flags & BIND_VERTEX)
				m_d3dDeviceContext.VSSetShaderResources(samplerIndex, 1, &m_d3dSRV);
			if (flags & BIND_GEOMETRY)
				m_d3dDeviceContext.GSSetShaderResources(samplerIndex, 1, &m_d3dSRV);
			if (flags & BIND_PIXEL)
				m_d3dDeviceContext.PSSetShaderResources(samplerIndex, 1, &m_d3dSRV);
			if (flags & BIND_HULL)
				m_d3dDeviceContext.HSSetShaderResources(samplerIndex, 1, &m_d3dSRV);
			if (flags & BIND_DOMAIN)
				m_d3dDeviceContext.DSSetShaderResources(samplerIndex, 1, &m_d3dSRV);
		}
		if (m_d3dSamplerState)
		{
			if (flags & BIND_VERTEX)
				m_d3dDeviceContext.VSSetSamplers(samplerIndex, 1, &m_d3dSamplerState);
			if (flags & BIND_GEOMETRY)
				m_d3dDeviceContext.GSSetSamplers(samplerIndex, 1, &m_d3dSamplerState);
			if (flags & BIND_PIXEL)
				m_d3dDeviceContext.PSSetSamplers(samplerIndex, 1, &m_d3dSamplerState);
			if (flags & BIND_HULL)
				m_d3dDeviceContext.HSSetSamplers(samplerIndex, 1, &m_d3dSamplerState);
			if (flags & BIND_DOMAIN)
				m_d3dDeviceContext.DSSetSamplers(samplerIndex, 1, &m_d3dSamplerState);
		}
	}
	else
	{
		ID3D11ShaderResourceView* nullResources[] = { NULL };
		m_d3dDeviceContext.VSSetShaderResources(samplerIndex, 1, nullResources);
		m_d3dDeviceContext.GSSetShaderResources(samplerIndex, 1, nullResources);
		m_d3dDeviceContext.PSSetShaderResources(samplerIndex, 1, nullResources);
		m_d3dDeviceContext.HSSetShaderResources(samplerIndex, 1, nullResources);
		m_d3dDeviceContext.DSSetShaderResources(samplerIndex, 1, nullResources);
		ID3D11SamplerState* nullSamplers[] = { NULL };
		m_d3dDeviceContext.VSSetSamplers(samplerIndex, 1, nullSamplers);
		m_d3dDeviceContext.GSSetSamplers(samplerIndex, 1, nullSamplers);
		m_d3dDeviceContext.PSSetSamplers(samplerIndex, 1, nullSamplers);
		m_d3dDeviceContext.HSSetSamplers(samplerIndex, 1, nullSamplers);
		m_d3dDeviceContext.DSSetSamplers(samplerIndex, 1, nullSamplers);
	}
}

void D3D11RendererTexture2D::onDeviceLost(void)
{
	dxSafeRelease(m_d3dTexture);
	dxSafeRelease(m_d3dSamplerState);
	dxSafeRelease(m_d3dSRV);
	dxSafeRelease(m_d3dRTV);
	dxSafeRelease(m_d3dDSV);
}

void D3D11RendererTexture2D::onDeviceReset(void)
{
	HRESULT result = S_OK;
	if (!m_d3dTexture)
	{
		D3D11_SUBRESOURCE_DATA* pData = isDepthStencilFormat(getFormat()) ? NULL : m_resourceData;
		result = m_d3dDevice.CreateTexture2D(&m_d3dTextureDesc, pData, &m_d3dTexture);
		RENDERER_ASSERT(SUCCEEDED(result), "Unable to create D3D11 Texture.");
	}
	if (SUCCEEDED(result) && !m_d3dSamplerState)
	{
		result = m_d3dDevice.CreateSamplerState(&m_d3dSamplerDesc, &m_d3dSamplerState);
		RENDERER_ASSERT(SUCCEEDED(result), "Unable to create D3D11 Sampler.");
	}
	if (SUCCEEDED(result) && m_d3dTextureDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE && !m_d3dSRV)
	{
		result = m_d3dDevice.CreateShaderResourceView(m_d3dTexture, &m_d3dSRVDesc, &m_d3dSRV);
		RENDERER_ASSERT(SUCCEEDED(result), "Unable to create D3D11 Shader Resource View.");
	}
	if (SUCCEEDED(result) && m_d3dTextureDesc.BindFlags & D3D11_BIND_RENDER_TARGET && !m_d3dRTV)
	{
		result = m_d3dDevice.CreateRenderTargetView(m_d3dTexture, &m_d3dRTVDesc, &m_d3dRTV);
		RENDERER_ASSERT(SUCCEEDED(result), "Unable to create D3D11 Render Target View.");
	}
	if (SUCCEEDED(result) && m_d3dTextureDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL && !m_d3dDSV)
	{
		result = m_d3dDevice.CreateDepthStencilView(m_d3dTexture, &m_d3dDSVDesc, &m_d3dDSV);
		RENDERER_ASSERT(SUCCEEDED(result), "Unable to create D3D11 Depth Stencil View.");
	}
}


void D3D11RendererTexture2D::loadTextureDesc(const RendererTexture2DDesc& desc)
{
	RENDERER_ASSERT(desc.depth == 1, "Invalid depth for 2D Texture!");

	//memset(&m_d3dTextureDesc, 0, sizeof(m_d3dTextureDesc));
	m_d3dTextureDesc = D3D11_TEXTURE2D_DESC();
	m_d3dTextureDesc.Width = getWidth();
	m_d3dTextureDesc.Height = getHeight();
	m_d3dTextureDesc.MipLevels = getNumLevels();
	m_d3dTextureDesc.ArraySize = 1;
	m_d3dTextureDesc.Format = getD3D11TextureFormat(desc.format);
	m_d3dTextureDesc.SampleDesc.Count = 1;
	m_d3dTextureDesc.SampleDesc.Quality = 0;
	m_d3dTextureDesc.CPUAccessFlags = 0;
	m_d3dTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	m_d3dTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	loadResourceDesc(desc);

	if (isDepthStencilFormat(desc.format))
	{
		m_d3dTextureDesc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
		m_d3dTextureDesc.CPUAccessFlags = 0;
		m_d3dTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		loadDepthStencilDesc(desc);
	}
	else if (desc.renderTarget)
	{
		m_d3dTextureDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		m_d3dTextureDesc.CPUAccessFlags = 0;
		m_d3dTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		loadTargetDesc(desc);
	} 

	loadSamplerDesc(desc);

	//if (m_d3dTextureDesc.CPUAccessFlags)
	{
		m_data = new PxU8*[getNumLevels()];
		m_resourceData = new D3D11_SUBRESOURCE_DATA[getNumLevels()];
		memset(m_data, 0, sizeof(PxU8)*getNumLevels());
		memset(m_resourceData, 0, sizeof(D3D11_SUBRESOURCE_DATA)*getNumLevels());

		for (PxU32 i = 0; i < desc.numLevels; i++)
		{
			PxU32 w = getLevelDimension(getWidth(),  i);
			PxU32 h = getLevelDimension(getHeight(), i);
			PxU32 levelSize = computeImageByteSize(w, h, 1, desc.format);
			m_data[i]       = new PxU8[levelSize];
			memset(m_data[i], 0, levelSize);
			m_resourceData[i].pSysMem = m_data[i];
			m_resourceData[i].SysMemPitch = levelSize / h;
			m_resourceData[i].SysMemSlicePitch = 0;
		}
	}
}

void D3D11RendererTexture2D::loadSamplerDesc(const RendererTexture2DDesc& desc)
{
	m_d3dSamplerDesc.Filter   = getD3D11TextureFilter(desc.filter);
	m_d3dSamplerDesc.AddressU = getD3D11TextureAddressing(desc.addressingU);
	m_d3dSamplerDesc.AddressV = getD3D11TextureAddressing(desc.addressingV);
	m_d3dSamplerDesc.AddressW = getD3D11TextureAddressing(desc.addressingW);
	m_d3dSamplerDesc.MipLODBias = 0.f;
	m_d3dSamplerDesc.MaxAnisotropy = 1;
	m_d3dSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	m_d3dSamplerDesc.BorderColor[0] = m_d3dSamplerDesc.BorderColor[1] = m_d3dSamplerDesc.BorderColor[2] = m_d3dSamplerDesc.BorderColor[3] = 0.;
	m_d3dSamplerDesc.MinLOD = 0;
	if (desc.numLevels <= 1)
	{
		m_d3dSamplerDesc.MaxLOD = 0.;
	}
	else
	{
		m_d3dSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	}
}

void D3D11RendererTexture2D::loadResourceDesc(const RendererTexture2DDesc& desc)
{
	m_d3dSRVDesc.Format = (m_d3dTextureDesc.Format == DXGI_FORMAT_R16_TYPELESS) ? DXGI_FORMAT_R16_UNORM : m_d3dTextureDesc.Format;
	m_d3dSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	m_d3dSRVDesc.Texture2D.MipLevels = m_d3dTextureDesc.MipLevels;
	m_d3dSRVDesc.Texture2D.MostDetailedMip = 0;
	//m_d3dSRVDesc.Texture2D.MostDetailedMip = m_d3dTextureDesc.MipLevels-1;
}

void D3D11RendererTexture2D::loadTargetDesc(const RendererTexture2DDesc& desc)
{
	m_d3dRTVDesc = D3D11_RENDER_TARGET_VIEW_DESC();
	m_d3dRTVDesc.Format = (m_d3dTextureDesc.Format == DXGI_FORMAT_R16_TYPELESS) ? DXGI_FORMAT_R16_UNORM : m_d3dTextureDesc.Format;
	m_d3dRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	m_d3dRTVDesc.Texture2D.MipSlice = 0;
}

void D3D11RendererTexture2D::loadDepthStencilDesc(const RendererTexture2DDesc& desc)
{
	m_d3dDSVDesc = D3D11_DEPTH_STENCIL_VIEW_DESC();
	m_d3dDSVDesc.Format = (m_d3dTextureDesc.Format == DXGI_FORMAT_R16_TYPELESS) ? DXGI_FORMAT_D16_UNORM : m_d3dTextureDesc.Format;
	m_d3dDSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	m_d3dDSVDesc.Texture2D.MipSlice = 0;
}

#endif // #if defined(RENDERER_ENABLE_DIRECT3D11)
