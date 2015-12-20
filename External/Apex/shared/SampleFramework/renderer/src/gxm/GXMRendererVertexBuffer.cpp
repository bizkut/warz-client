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

#include "RendererConfig.h"
#include <SamplePlatform.h>

#if defined(RENDERER_ENABLE_LIBGXM)

#include "GXMRendererVertexBuffer.h"

#include <RendererVertexBufferDesc.h>
#include "GXMRenderer.h"


using namespace SampleRenderer;

GXMRendererVertexBuffer::GXMRendererVertexBuffer(const RendererVertexBufferDesc &desc, GXMRenderer& renderer) 
	:RendererVertexBuffer(desc),m_Renderer(renderer), m_Buffer(0) , m_MemoryId(0)
{			
	RENDERER_ASSERT(m_stride && desc.maxVertices, "Unable to create Vertex Buffer of zero size.");
	if(m_stride && desc.maxVertices)
	{		
		m_Buffer = GXMRenderer::graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
			m_stride * desc.maxVertices,
			8,
			SCE_GXM_MEMORY_ATTRIB_READ,
			&m_MemoryId);
		
		RENDERER_ASSERT(m_Buffer, "Failed to create Vertex Buffer Object.");

		m_maxVertices = desc.maxVertices;
	}
}

GXMRendererVertexBuffer::~GXMRendererVertexBuffer(void)
{
	if(m_MemoryId)
	{
		m_Buffer = NULL;
		GXMRenderer::graphicsFree(m_MemoryId);
	}
}

PxI32 GXMRendererVertexBuffer::GetSemanticOffset(const Semantic& semantic) const
{
	if(m_semanticDescs[semantic].format != RendererVertexBuffer::NUM_FORMATS)
	{
		return m_semanticDescs[semantic].offset;
	}
	else
	{
		return -1;
	}
}

void GXMRendererVertexBuffer::swizzleColor(void *colors, PxU32 stride, PxU32 numColors, RendererVertexBuffer::Format inFormat)
{
}

void *GXMRendererVertexBuffer::lock(void)
{
	void *buffer = m_Buffer;
	return buffer;
}

void GXMRendererVertexBuffer::unlock(void)
{
}


void GXMRendererVertexBuffer::bind(PxU32 streamID, PxU32 firstVertex)
{
	sceGxmSetVertexStream(m_Renderer.getContext(), 0, m_Buffer);
}

void GXMRendererVertexBuffer::unbind(PxU32 streamID)
{
}

#endif // #if defined(RENDERER_ENABLE_LIBGXM)
