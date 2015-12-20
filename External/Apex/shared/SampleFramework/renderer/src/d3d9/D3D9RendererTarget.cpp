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

// suppress LNK4221 on Xbox
namespace {char dummySymbol; }

#include <RendererConfig.h>
#include "D3D9RendererTarget.h"

#if defined(RENDERER_ENABLE_DIRECT3D9) && defined(RENDERER_ENABLE_DIRECT3D9_TARGET)

#include <RendererTargetDesc.h>
#include "D3D9RendererTexture2D.h"

using namespace SampleRenderer;

D3D9RendererTarget::D3D9RendererTarget(IDirect3DDevice9 &d3dDevice, const RendererTargetDesc &desc) :
	m_d3dDevice(d3dDevice)
{
	m_d3dLastSurface = 0;
	m_d3dLastDepthStencilSurface = 0;
	m_d3dDepthStencilSurface = 0;
	for(PxU32 i=0; i<desc.numTextures; i++)
	{
		D3D9RendererTexture2D &texture = *static_cast<D3D9RendererTexture2D*>(desc.textures[i]);
		m_textures.push_back(&texture);
	}
	m_depthStencilSurface = static_cast<D3D9RendererTexture2D*>(desc.depthStencilSurface);
	RENDERER_ASSERT(m_depthStencilSurface && m_depthStencilSurface->m_d3dTexture, "Invalid Target Depth Stencil Surface!");
	onDeviceReset();
}

D3D9RendererTarget::~D3D9RendererTarget(void)
{
	if(m_d3dDepthStencilSurface) m_d3dDepthStencilSurface->Release();
}

void D3D9RendererTarget::bind(void)
{
	RENDERER_ASSERT(m_d3dLastSurface==0 && m_d3dLastDepthStencilSurface==0, "Render Target in bad state!");
	if(m_d3dDepthStencilSurface && !m_d3dLastSurface && !m_d3dLastDepthStencilSurface)
	{
		m_d3dDevice.GetRenderTarget(0, &m_d3dLastSurface);
		m_d3dDevice.GetDepthStencilSurface(&m_d3dLastDepthStencilSurface);
		const PxU32 numTextures = (PxU32)m_textures.size();
		for(PxU32 i=0; i<numTextures; i++)
		{
			IDirect3DSurface9 *d3dSurcace = 0;
			D3D9RendererTexture2D &texture = *m_textures[i];
			/* HRESULT result = */ texture.m_d3dTexture->GetSurfaceLevel(0, &d3dSurcace);
			RENDERER_ASSERT(d3dSurcace, "Cannot get Texture Surface!");
			if(d3dSurcace)
			{
				m_d3dDevice.SetRenderTarget(i, d3dSurcace);
				d3dSurcace->Release();
			}
		}
		m_d3dDevice.SetDepthStencilSurface(m_d3dDepthStencilSurface);
		const DWORD flags = D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER;
		m_d3dDevice.Clear(0, 0, flags, 0x00000000, 1.0f, 0);
	}
	float depthBias = 0.0001f;
	float biasSlope = 1.58f;
#if RENDERER_ENABLE_DRESSCODE
	depthBias = dcParam("depthBias", depthBias, 0.0f, 0.01f);
	biasSlope = dcParam("biasSlope", biasSlope, 0.0f, 5.0f);
#endif
	m_d3dDevice.SetRenderState(D3DRS_DEPTHBIAS,           *(DWORD*)&depthBias);
	m_d3dDevice.SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&biasSlope);
}

void D3D9RendererTarget::unbind(void)
{
	RENDERER_ASSERT(m_d3dLastSurface && m_d3dLastDepthStencilSurface, "Render Target in bad state!");
	if(m_d3dDepthStencilSurface && m_d3dLastSurface && m_d3dLastDepthStencilSurface)
	{
		m_d3dDevice.SetDepthStencilSurface(m_d3dLastDepthStencilSurface);
		m_d3dDevice.SetRenderTarget(0, m_d3dLastSurface);
		const PxU32 numTextures = (PxU32)m_textures.size();
		for(PxU32 i=1; i<numTextures; i++)
		{
			m_d3dDevice.SetRenderTarget(i, 0);
		}
		m_d3dLastSurface->Release();
		m_d3dLastSurface = 0;
		m_d3dLastDepthStencilSurface->Release();
		m_d3dLastDepthStencilSurface = 0;
	}
	float depthBias = 0;
	float biasSlope = 0;
	m_d3dDevice.SetRenderState(D3DRS_DEPTHBIAS,           *(DWORD*)&depthBias);
	m_d3dDevice.SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, *(DWORD*)&biasSlope);
}

void D3D9RendererTarget::onDeviceLost(void)
{
	RENDERER_ASSERT(m_d3dLastDepthStencilSurface==0, "Render Target in bad state!");
	RENDERER_ASSERT(m_d3dDepthStencilSurface,        "Render Target in bad state!");
	if(m_d3dDepthStencilSurface)
	{
		m_d3dDepthStencilSurface->Release();
		m_d3dDepthStencilSurface = 0;
	}
}

void D3D9RendererTarget::onDeviceReset(void)
{
	RENDERER_ASSERT(m_d3dDepthStencilSurface==0, "Render Target in bad state!");
	if(!m_d3dDepthStencilSurface && m_depthStencilSurface && m_depthStencilSurface->m_d3dTexture)
	{
		bool ok = m_depthStencilSurface->m_d3dTexture->GetSurfaceLevel(0, &m_d3dDepthStencilSurface) == D3D_OK;
		if(!ok)
		{
			RENDERER_ASSERT(ok, "Failed to create Render Target Depth Stencil Surface.");
		}
	}
}

#endif //#if defined(RENDERER_ENABLE_DIRECT3D9) && defined(RENDERER_ENABLE_DIRECT3D9_TARGET)
