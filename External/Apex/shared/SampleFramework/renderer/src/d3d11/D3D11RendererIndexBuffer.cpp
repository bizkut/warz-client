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

#include "D3D11RendererIndexBuffer.h"
#include <RendererIndexBufferDesc.h>

#if defined(PX_WINDOWS)
#include <PxTaskIncludes.h>
#endif

using namespace SampleRenderer;

static DXGI_FORMAT getD3D11Format(RendererIndexBuffer::Format format)
{
	DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;
	switch (format)
	{
	case RendererIndexBuffer::FORMAT_UINT16:
		dxgiFormat = DXGI_FORMAT_R16_UINT;
		break;
	case RendererIndexBuffer::FORMAT_UINT32:
		dxgiFormat = DXGI_FORMAT_R32_UINT;
		break;
	}
	RENDERER_ASSERT(dxgiFormat != DXGI_FORMAT_UNKNOWN, "Unable to convert to DXGI_FORMAT.");
	return dxgiFormat;
}

D3D11RendererIndexBuffer::D3D11RendererIndexBuffer(ID3D11Device& d3dDevice, ID3D11DeviceContext& d3dDeviceContext, const RendererIndexBufferDesc& desc, bool bUseMapForLocking) :
	RendererIndexBuffer(desc),
	m_d3dDevice(d3dDevice),
	m_d3dDeviceContext(d3dDeviceContext),
	m_d3dIndexBuffer(NULL),
	m_bUseMapForLocking(bUseMapForLocking),
	m_buffer(NULL)
{
	memset(&m_d3dBufferDesc, 0, sizeof(D3D11_BUFFER_DESC));
	m_d3dBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_d3dBufferDesc.ByteWidth = (UINT)(getFormatByteSize(desc.format) * desc.maxIndices);
	m_d3dBufferFormat = getD3D11Format(desc.format);

	if (m_bUseMapForLocking)
	{
		m_d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		m_d3dBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;
	}
	else
	{
		m_d3dBufferDesc.CPUAccessFlags = 0;
		m_d3dBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
		m_buffer = new PxU8[m_d3dBufferDesc.ByteWidth];
		memset(m_buffer, 0, sizeof(PxU8)*m_d3dBufferDesc.ByteWidth);
	}

	onDeviceReset();

	if (m_d3dIndexBuffer)
	{
		m_maxIndices = desc.maxIndices;
	}

}

D3D11RendererIndexBuffer::~D3D11RendererIndexBuffer(void)
{
	if (m_d3dIndexBuffer)
	{
		if (m_interopContext && m_registeredInCUDA)
		{
			m_registeredInCUDA = !m_interopContext->unregisterResourceInCuda(m_InteropHandle);
		}
		m_d3dIndexBuffer->Release();
		m_d3dIndexBuffer = NULL;
	}

	delete [] m_buffer;
}


void D3D11RendererIndexBuffer::onDeviceLost(void)
{
	m_registeredInCUDA = false;

	if (m_d3dIndexBuffer)
	{
		if (m_interopContext && m_registeredInCUDA)
		{
			m_registeredInCUDA = !m_interopContext->unregisterResourceInCuda(m_InteropHandle);
		}
		m_d3dIndexBuffer->Release();
		m_d3dIndexBuffer = 0;
	}
}

void D3D11RendererIndexBuffer::onDeviceReset(void)
{
	if (!m_d3dIndexBuffer)
	{
		m_d3dDevice.CreateBuffer(&m_d3dBufferDesc, NULL, &m_d3dIndexBuffer);
		RENDERER_ASSERT(m_d3dIndexBuffer, "Failed to create DIRECT3D11 Index Buffer.");
		if (m_interopContext && m_d3dIndexBuffer && m_mustBeRegisteredInCUDA)
		{
			m_registeredInCUDA = m_interopContext->registerResourceInCudaD3D(m_InteropHandle, m_d3dIndexBuffer);
		}
	}
}

void* D3D11RendererIndexBuffer::lock(void)
{
	// For now NO_OVERWRITE is the only mapping that functions properly
	return internalLock(getHint() == HINT_STATIC ? /* D3D11_MAP_WRITE_DISCARD */ D3D11_MAP_WRITE_NO_OVERWRITE : D3D11_MAP_WRITE_NO_OVERWRITE);
}

void* D3D11RendererIndexBuffer::internalLock(D3D11_MAP MapType)
{
	void* buffer = 0;
	if (m_d3dIndexBuffer)
	{
		if (m_bUseMapForLocking)
		{
			D3D11_MAPPED_SUBRESOURCE mappedRead;
			m_d3dDeviceContext.Map(m_d3dIndexBuffer, 0, MapType, NULL, &mappedRead);
			RENDERER_ASSERT(mappedRead.pData, "Failed to lock DIRECT3D11 Index Buffer.");
			buffer = mappedRead.pData;	
		}
		else
		{
			buffer = m_buffer;
		}
	}
	return buffer;
}

void D3D11RendererIndexBuffer::unlock(void)
{
	if (m_d3dIndexBuffer)
	{
		if (m_bUseMapForLocking)
		{
			m_d3dDeviceContext.Unmap(m_d3dIndexBuffer, 0);
		}
		else
		{
			m_d3dDeviceContext.UpdateSubresource(m_d3dIndexBuffer, 0, NULL, m_buffer, m_d3dBufferDesc.ByteWidth, 0);
		}
	}
}

void D3D11RendererIndexBuffer::bind(void) const
{
	m_d3dDeviceContext.IASetIndexBuffer(m_d3dIndexBuffer, m_d3dBufferFormat, 0);
}

void D3D11RendererIndexBuffer::unbind(void) const
{
	m_d3dDeviceContext.IASetIndexBuffer(NULL, DXGI_FORMAT(), 0);
}

#endif // #if defined(RENDERER_ENABLE_DIRECT3D11)
