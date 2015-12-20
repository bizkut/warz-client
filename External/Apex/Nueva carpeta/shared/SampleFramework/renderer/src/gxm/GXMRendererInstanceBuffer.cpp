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

#if defined(RENDERER_ENABLE_LIBGXM) 

#include "GXMRendererInstanceBuffer.h"
#include <RendererInstanceBufferDesc.h>
#include "GXMRenderer.h"

using namespace SampleRenderer;

GXMRendererInstanceBuffer::GXMRendererInstanceBuffer(const RendererInstanceBufferDesc &desc, GXMRenderer& renderer) :
	RendererInstanceBuffer(desc), m_Renderer(renderer)
{
	m_memoryId = 0;
	m_bufferSize   = (PxU32)(desc.maxInstances * m_stride);
	m_buffer       = GXMRenderer::graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
		m_bufferSize,
		8,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&m_memoryId);

	RENDERER_ASSERT(m_buffer, "Failed to create Vertex Buffer Object.");
		
	m_maxInstances = desc.maxInstances;
}

GXMRendererInstanceBuffer::~GXMRendererInstanceBuffer(void)
{
	if(m_memoryId)
	{
		m_buffer = NULL;
		GXMRenderer::graphicsFree(m_memoryId);
	}
}

physx::PxMat44 GXMRendererInstanceBuffer::getModelMatrix(PxU32 index) const
{
	physx::PxMat44 model;
	model = physx::PxMat44::createIdentity();
	if(index < m_maxInstances)
	{
		const void *instance = ((PxU8*)m_buffer)+(m_stride*index);
		PxVec3 column0 = getInstanceColumn(instance, m_semanticDescs[SEMANTIC_NORMALX]);
		PxVec3 column1 = getInstanceColumn(instance, m_semanticDescs[SEMANTIC_NORMALY]);
		PxVec3 column2 = getInstanceColumn(instance, m_semanticDescs[SEMANTIC_NORMALZ]);
		PxVec3 column3 = getInstanceColumn(instance, m_semanticDescs[SEMANTIC_POSITION]);

		model = PxMat44(column0, column1, column2, column3);
	}
	return model;
}

PxVec3 GXMRendererInstanceBuffer::getInstanceColumn(const void *instance, const GXMRendererInstanceBuffer::SemanticDesc &sd) const
{
	PxVec3 col = *(PxVec3*)(((PxU8*)instance)+sd.offset);
	return col;
}

void *GXMRendererInstanceBuffer::lock(void)
{
	return m_buffer;
}

void GXMRendererInstanceBuffer::unlock(void)
{

}

void GXMRendererInstanceBuffer::bind(PxU32 streamID, PxU32 firstInstance) const
{
	sceGxmSetVertexStream( m_Renderer.getContext(), 1, m_buffer);
}

void GXMRendererInstanceBuffer::unbind(PxU32 streamID) const
{

}

#endif // #if defined(RENDERER_ENABLE_OPENGL) 
