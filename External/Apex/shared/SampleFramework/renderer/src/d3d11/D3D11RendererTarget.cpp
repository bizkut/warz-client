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
#include "D3D11RendererTarget.h"

#if defined(RENDERER_ENABLE_DIRECT3D11) && defined(RENDERER_ENABLE_DIRECT3D11_TARGET)

#include <RendererTargetDesc.h>
#include "D3D11RendererTexture2D.h"
#include "D3D11RendererMemoryMacros.h"

using namespace SampleRenderer;

D3D11RendererTarget::D3D11RendererTarget(ID3D11Device& d3dDevice, ID3D11DeviceContext& d3dDeviceContext, const RendererTargetDesc& desc) :
	m_d3dDevice(d3dDevice),
	m_d3dDeviceContext(d3dDeviceContext),
	m_depthStencilSurface(NULL),
	m_d3dDSV(NULL),
	m_d3dLastDSV(NULL),
	m_d3dRS(NULL),
	m_d3dLastRS(NULL)
{
	for (PxU32 i = 0; i < desc.numTextures; i++)
	{
		D3D11RendererTexture2D& texture = *static_cast<D3D11RendererTexture2D*>(desc.textures[i]);
		m_textures.push_back(&texture);
		RENDERER_ASSERT(texture.m_d3dRTV, "Invalid render target specification");
		if (texture.m_d3dRTV)
		{
			m_d3dRTVs.push_back(texture.m_d3dRTV);
		}
	}
	m_depthStencilSurface = static_cast<D3D11RendererTexture2D*>(desc.depthStencilSurface);
	RENDERER_ASSERT(m_depthStencilSurface && m_depthStencilSurface->m_d3dTexture, "Invalid Target Depth Stencil Surface!");
	m_d3dDSV = m_depthStencilSurface->m_d3dDSV;
	onDeviceReset();
}

D3D11RendererTarget::~D3D11RendererTarget(void)
{
	dxSafeRelease(m_d3dRS);
}

void D3D11RendererTarget::bind(void)
{
	RENDERER_ASSERT(m_d3dRS && m_d3dLastDSV == NULL && m_d3dLastRTVs.size() == 0, "Render target in a bad state");
	if (m_d3dRS && !m_d3dLastDSV && m_d3dLastRTVs.size() == 0)
	{
		m_d3dLastRTVs.resize(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, NULL);
		m_d3dLastDSV = NULL;

		m_d3dDeviceContext.OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT,
		                                      &m_d3dLastRTVs[0],
		                                      &m_d3dLastDSV);
		m_d3dDeviceContext.RSGetState(&m_d3dLastRS);

		static const PxF32 black[4] = {0.f, 0.f, 0.f, 0.f};
		for (PxU32 i = 0; i < m_d3dRTVs.size(); ++i)
		{
			m_d3dDeviceContext.ClearRenderTargetView(m_d3dRTVs[i], black);
		}
		m_d3dDeviceContext.ClearDepthStencilView(m_d3dDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1., 0);

		m_d3dDeviceContext.OMSetRenderTargets((UINT)m_d3dRTVs.size(), &m_d3dRTVs[0], m_d3dDSV);
		m_d3dDeviceContext.RSSetState(m_d3dRS);
	}
}

void D3D11RendererTarget::unbind(void)
{
	RENDERER_ASSERT(m_d3dLastDSV && m_d3dLastRTVs.size() > 0, "Render Target in a bad state.");
	if (m_d3dLastDSV && m_d3dLastRTVs.size() > 0)
	{
		m_d3dDeviceContext.OMSetRenderTargets((UINT)m_d3dLastRTVs.size(), &m_d3dLastRTVs[0], m_d3dLastDSV);
		for (PxU32 i = 0; i < m_d3dLastRTVs.size(); ++i)
		{
			dxSafeRelease(m_d3dLastRTVs[i]);
		}
		m_d3dLastRTVs.clear();
		dxSafeRelease(m_d3dLastDSV);
	}
	if (m_d3dLastRS)
	{
		m_d3dDeviceContext.RSSetState(m_d3dLastRS);
		dxSafeRelease(m_d3dLastRS);
	}
}

void D3D11RendererTarget::onDeviceLost(void)
{
	RENDERER_ASSERT(m_d3dLastRS == NULL,  "Render Target in bad state!");
	RENDERER_ASSERT(m_d3dRS,              "Render Target in bad state!");
	dxSafeRelease(m_d3dRS);
}

void D3D11RendererTarget::onDeviceReset(void)
{
	RENDERER_ASSERT(m_d3dRS == NULL, "Render Target in a bad state!");
	if (!m_d3dRS)
	{
		D3D11_RASTERIZER_DESC rasterizerDesc =
		{
			D3D11_FILL_SOLID, // D3D11_FILL_MODE FillMode;
			D3D11_CULL_NONE,  // D3D11_CULL_MODE CullMode;
			FALSE,            // BOOL FrontCounterClockwise;
			0,                // INT DepthBias;
			0,                // FLOAT DepthBiasClamp;
			1.0,              // FLOAT SlopeScaledDepthBias;
			TRUE,             // BOOL DepthClipEnable;
			FALSE,            // BOOL ScissorEnable;
			TRUE,             // BOOL MultisampleEnable;
			FALSE,            // BOOL AntialiasedLineEnable;
		};

		//float depthBias = 0.0001f;
		//float biasSlope = 1.58f;
#if RENDERER_ENABLE_DRESSCODE
		//depthBias = dcParam("depthBias", depthBias, 0.0f, 0.01f);
		//biasSlope = dcParam("biasSlope", biasSlope, 0.0f, 5.0f);
#endif

		m_d3dDevice.CreateRasterizerState(&rasterizerDesc, &m_d3dRS);
	}
}

#endif //#if defined(RENDERER_ENABLE_DIRECT3D11) && defined(RENDERER_ENABLE_DIRECT3D11_TARGET)
