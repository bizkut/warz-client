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

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_DIRECT3D9)

#include "D3D9RendererTexture2D.h"
#include <RendererTexture2DDesc.h>

#include <SamplePlatform.h>

using namespace SampleRenderer;

static void supportsAnisotropic(IDirect3DDevice9& device, bool& min, bool& mag, bool& mip)
{
	D3DCAPS9 caps;
	device.GetDeviceCaps(&caps);
	min = (caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC) != 0;
	mag = (caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC) != 0;
	mip = false;
}

static D3DFORMAT getD3D9TextureFormat(RendererTexture2D::Format format)
{
	D3DFORMAT d3dFormat = static_cast<D3DFORMAT>(SampleFramework::SamplePlatform::platform()->getD3D9TextureFormat(format));
	RENDERER_ASSERT(d3dFormat != D3DFMT_UNKNOWN, "Unable to convert to D3D9 Texture Format.");
	return d3dFormat;
}

static D3DTEXTUREFILTERTYPE getD3D9TextureFilter(RendererTexture2D::Filter filter, bool anisoSupport)
{
	D3DTEXTUREFILTERTYPE d3dFilter = D3DTEXF_FORCE_DWORD;
	switch(filter)
	{
	case RendererTexture2D::FILTER_NEAREST:     d3dFilter = D3DTEXF_POINT;                              break;
	case RendererTexture2D::FILTER_LINEAR:      d3dFilter = D3DTEXF_LINEAR;                             break;
	case RendererTexture2D::FILTER_ANISOTROPIC: d3dFilter = anisoSupport ? D3DTEXF_ANISOTROPIC : D3DTEXF_LINEAR; break;
	}
	RENDERER_ASSERT(d3dFilter != D3DTEXF_FORCE_DWORD, "Unable to convert to D3D9 Filter mode.");
	return d3dFilter;
}

static D3DTEXTUREADDRESS getD3D9TextureAddressing(RendererTexture2D::Addressing addressing)
{
	D3DTEXTUREADDRESS d3dAddressing = D3DTADDRESS_FORCE_DWORD;
	switch(addressing)
	{
	case RendererTexture2D::ADDRESSING_WRAP:   d3dAddressing = D3DTADDRESS_WRAP;   break;
	case RendererTexture2D::ADDRESSING_CLAMP:  d3dAddressing = D3DTADDRESS_CLAMP;  break;
	case RendererTexture2D::ADDRESSING_MIRROR: d3dAddressing = D3DTADDRESS_MIRROR; break;
	}
	RENDERER_ASSERT(d3dAddressing != D3DTADDRESS_FORCE_DWORD, "Unable to convert to D3D9 Addressing mode.");
	return d3dAddressing;
}

D3D9RendererTexture2D::D3D9RendererTexture2D(IDirect3DDevice9 &d3dDevice, const RendererTexture2DDesc &desc) :
RendererTexture2D(desc),
	m_d3dDevice(d3dDevice)
{
	RENDERER_ASSERT(desc.depth == 1, "Invalid depth for 2D Texture!");

	m_d3dTexture     = 0;
	m_usage          = 0;
	m_pool           = D3DPOOL_MANAGED;
	m_format         = getD3D9TextureFormat(desc.format);

	bool minAniso, magAniso, mipAniso;
	supportsAnisotropic(d3dDevice, minAniso, magAniso, mipAniso);
	m_d3dMinFilter   = getD3D9TextureFilter(desc.filter, minAniso);
	m_d3dMagFilter   = getD3D9TextureFilter(desc.filter, magAniso);
	m_d3dMipFilter   = getD3D9TextureFilter(desc.filter, mipAniso);
	m_d3dAddressingU = getD3D9TextureAddressing(desc.addressingU);
	m_d3dAddressingV = getD3D9TextureAddressing(desc.addressingV);
	if(desc.renderTarget)
	{
		m_usage = D3DUSAGE_RENDERTARGET; 
		m_pool  = D3DPOOL_DEFAULT;
	}
	if(isDepthStencilFormat(desc.format))
	{
		m_usage = D3DUSAGE_DEPTHSTENCIL; 
		m_pool  = D3DPOOL_DEFAULT;
	}
	onDeviceReset();
}

D3D9RendererTexture2D::~D3D9RendererTexture2D(void)
{
	if(m_d3dTexture)
	{
		D3DCAPS9 pCaps;
		m_d3dDevice.GetDeviceCaps(&pCaps);
		DWORD i = pCaps.MaxTextureBlendStages;
		while(i--) m_d3dDevice.SetTexture(i, NULL);
		SampleFramework::SamplePlatform::platform()->D3D9BlockUntilNotBusy(m_d3dTexture);
		m_d3dTexture->Release();
	}
}

void *D3D9RendererTexture2D::lockLevel(PxU32 level, PxU32 &pitch)
{
	void *buffer = 0;
	if(m_d3dTexture)
	{
		D3DLOCKED_RECT lockedRect;
		HRESULT result = m_d3dTexture->LockRect((DWORD)level, &lockedRect, 0, D3DLOCK_NOSYSLOCK);
		RENDERER_ASSERT(result == D3D_OK, "Unable to lock Texture 2D.");
		if(result == D3D_OK)
		{
			buffer = lockedRect.pBits;
			pitch  = (PxU32)lockedRect.Pitch;
		}
	}
	return buffer;
}

void D3D9RendererTexture2D::unlockLevel(PxU32 level)
{
	if(m_d3dTexture)
	{
		m_d3dTexture->UnlockRect(level);
	}
}

void D3D9RendererTexture2D::bind(PxU32 samplerIndex)
{
	m_d3dDevice.SetTexture(     (DWORD)samplerIndex, m_d3dTexture);
	m_d3dDevice.SetSamplerState((DWORD)samplerIndex, D3DSAMP_MINFILTER, m_d3dMinFilter);
	m_d3dDevice.SetSamplerState((DWORD)samplerIndex, D3DSAMP_MAGFILTER, m_d3dMagFilter);
	m_d3dDevice.SetSamplerState((DWORD)samplerIndex, D3DSAMP_MIPFILTER, m_d3dMipFilter);
	m_d3dDevice.SetSamplerState((DWORD)samplerIndex, D3DSAMP_ADDRESSU,  m_d3dAddressingU);
	m_d3dDevice.SetSamplerState((DWORD)samplerIndex, D3DSAMP_ADDRESSV,  m_d3dAddressingV);
}

void D3D9RendererTexture2D::onDeviceLost(void)
{
	if(m_pool != D3DPOOL_MANAGED)
	{
		if(m_d3dTexture)
		{
			m_d3dTexture->Release();
			m_d3dTexture = 0;
		}
	}
}

void D3D9RendererTexture2D::onDeviceReset(void)
{
	if(!m_d3dTexture)
	{
		HRESULT result   = m_d3dDevice.CreateTexture((UINT)getWidth(), (UINT)getHeight(), (UINT)getNumLevels(), m_usage, m_format, m_pool, &m_d3dTexture, 0);
		RENDERER_ASSERT(result == D3D_OK, "Unable to create D3D9 Texture.");
		if(result == D3D_OK)
		{

		}
	}
}

#endif // #if defined(RENDERER_ENABLE_DIRECT3D9)
