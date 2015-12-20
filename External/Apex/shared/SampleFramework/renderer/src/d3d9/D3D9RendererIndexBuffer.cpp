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

#include "D3D9RendererIndexBuffer.h"
#include <RendererIndexBufferDesc.h>

#if defined(PX_WINDOWS)
#include <PxTaskIncludes.h>
#endif

using namespace SampleRenderer;

static D3DFORMAT getD3D9Format(RendererIndexBuffer::Format format)
{
	D3DFORMAT d3dFormat = D3DFMT_UNKNOWN;
	switch(format)
	{
	case RendererIndexBuffer::FORMAT_UINT16: d3dFormat = D3DFMT_INDEX16; break;
	case RendererIndexBuffer::FORMAT_UINT32: d3dFormat = D3DFMT_INDEX32; break;
	}
	RENDERER_ASSERT(d3dFormat != D3DFMT_UNKNOWN, "Unable to convert to D3DFORMAT.");
	return d3dFormat;
}

D3D9RendererIndexBuffer::D3D9RendererIndexBuffer(IDirect3DDevice9 &d3dDevice, const RendererIndexBufferDesc &desc) :
	RendererIndexBuffer(desc),
	m_d3dDevice(d3dDevice)
{
	m_d3dIndexBuffer = 0;

	m_usage      = 0;
	m_pool       = D3DPOOL_MANAGED;
	PxU32	indexSize  = getFormatByteSize(desc.format);
	m_format     = getD3D9Format(desc.format);
	m_bufferSize = indexSize * desc.maxIndices;

#if RENDERER_ENABLE_DYNAMIC_VB_POOLS
	if(desc.hint == RendererIndexBuffer::HINT_DYNAMIC)
	{
		m_usage = desc.registerInCUDA ? 0 : D3DUSAGE_DYNAMIC;
		m_pool  = D3DPOOL_DEFAULT;
	}
#endif

	onDeviceReset();

	if(m_d3dIndexBuffer)
	{
		m_maxIndices = desc.maxIndices;
	}
}

D3D9RendererIndexBuffer::~D3D9RendererIndexBuffer(void)
{
	if(m_d3dIndexBuffer)
	{
#if defined(PX_WINDOWS)
		if(m_interopContext && m_registeredInCUDA)
		{
			m_registeredInCUDA = !m_interopContext->unregisterResourceInCuda(m_InteropHandle);
		}
#endif
		m_d3dIndexBuffer->Release();
	}
}

void D3D9RendererIndexBuffer::onDeviceLost(void)
{
	m_registeredInCUDA = false;

	if(m_pool != D3DPOOL_MANAGED && m_d3dIndexBuffer)
	{
#if defined(PX_WINDOWS)
		if(m_interopContext && m_registeredInCUDA)
		{
			m_registeredInCUDA = !m_interopContext->unregisterResourceInCuda(m_InteropHandle);
		}
#endif
		m_d3dIndexBuffer->Release();
		m_d3dIndexBuffer = 0;
	}
}

void D3D9RendererIndexBuffer::onDeviceReset(void)
{
	if(!m_d3dIndexBuffer)
	{
		m_d3dDevice.CreateIndexBuffer(m_bufferSize, m_usage, m_format, m_pool, &m_d3dIndexBuffer, 0);
		RENDERER_ASSERT(m_d3dIndexBuffer, "Failed to create Direct3D9 Index Buffer.");
#if defined(PX_WINDOWS)
		if(m_interopContext && m_d3dIndexBuffer && m_mustBeRegisteredInCUDA)
		{
			m_registeredInCUDA = m_interopContext->registerResourceInCudaD3D(m_InteropHandle, m_d3dIndexBuffer);
		}
#endif
	}
}

void *D3D9RendererIndexBuffer::lock(void)
{
	void *buffer = 0;
	if(m_d3dIndexBuffer)
	{
		const Format format     = getFormat();
		const PxU32  maxIndices = getMaxIndices();
		const PxU32  bufferSize = maxIndices * getFormatByteSize(format);
		if(bufferSize > 0)
		{
			m_d3dIndexBuffer->Lock(0, (UINT)bufferSize, &buffer, 0);
		}
	}
	return buffer;
}

void D3D9RendererIndexBuffer::unlock(void)
{
	if(m_d3dIndexBuffer)
	{
		m_d3dIndexBuffer->Unlock();
	}
}

void D3D9RendererIndexBuffer::bind(void) const
{
	m_d3dDevice.SetIndices(m_d3dIndexBuffer);
}

void D3D9RendererIndexBuffer::unbind(void) const
{
	m_d3dDevice.SetIndices(0);
}

#endif // #if defined(RENDERER_ENABLE_DIRECT3D9)
