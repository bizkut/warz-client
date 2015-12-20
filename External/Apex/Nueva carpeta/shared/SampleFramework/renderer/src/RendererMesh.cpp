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
#include <RendererMesh.h>
#include <RendererMeshDesc.h>

#include <Renderer.h>
#include <RendererVertexBuffer.h>
#include <RendererIndexBuffer.h>
#include <RendererInstanceBuffer.h>


using namespace SampleRenderer;

RendererMesh::RendererMesh(const RendererMeshDesc &desc)
{
	m_primitives = desc.primitives;

	m_numVertexBuffers = desc.numVertexBuffers;
	m_vertexBuffers = new RendererVertexBuffer*[m_numVertexBuffers];
	for(PxU32 i=0; i<m_numVertexBuffers; i++)
	{
		m_vertexBuffers[i] = desc.vertexBuffers[i];
	}
	m_firstVertex    = desc.firstVertex;
	m_numVertices    = desc.numVertices;

	m_indexBuffer    = desc.indexBuffer;
	m_firstIndex     = desc.firstIndex;
	m_numIndices     = desc.numIndices;

	m_instanceBuffer = desc.instanceBuffer;
	m_firstInstance  = desc.firstInstance;
	m_numInstances   = desc.numInstances;
}

RendererMesh::~RendererMesh(void)
{
	delete [] m_vertexBuffers;
}

void SampleRenderer::RendererMesh::release(void)
{
	renderer().removeMeshFromRenderQueue(*this);
	delete this;
}

RendererMesh::Primitive RendererMesh::getPrimitives(void) const
{
	return m_primitives;
}

PxU32 RendererMesh::getNumVertices(void) const
{
	return m_numVertices;
}

PxU32 RendererMesh::getNumIndices(void) const
{
	return m_numIndices;
}

PxU32 RendererMesh::getNumInstances(void) const
{
	return m_numInstances;
}

void RendererMesh::setVertexBufferRange(PxU32 firstVertex, PxU32 numVertices)
{
	// TODO: Check for valid range...
	m_firstVertex = firstVertex;
	m_numVertices = numVertices;
}

void RendererMesh::setIndexBufferRange(PxU32 firstIndex, PxU32 numIndices)
{
	// TODO: Check for valid range...
	m_firstIndex = firstIndex;
	m_numIndices = numIndices;
}

void RendererMesh::setInstanceBufferRange(PxU32 firstInstance, PxU32 numInstances)
{
	// TODO: Check for valid range...
	m_firstInstance = firstInstance;
	m_numInstances  = numInstances;
}

PxU32 RendererMesh::getNumVertexBuffers(void) const
{
	return m_numVertexBuffers;
}

const RendererVertexBuffer *const*RendererMesh::getVertexBuffers(void) const
{
	return m_vertexBuffers;
}

const RendererIndexBuffer *RendererMesh::getIndexBuffer(void) const
{
	return m_indexBuffer;
}

const RendererInstanceBuffer *RendererMesh::getInstanceBuffer(void) const
{
	return m_instanceBuffer;
}

void RendererMesh::bind(void) const
{
	for(PxU32 i=0; i<m_numVertexBuffers; i++)
	{
		//RENDERER_ASSERT(m_vertexBuffers[i]->checkBufferWritten(), "Vertex buffer is empty!");
		if (m_vertexBuffers[i]->checkBufferWritten())
		{
			m_vertexBuffers[i]->bind(i, m_firstVertex);
		}
	}
	if(m_instanceBuffer)
	{
		m_instanceBuffer->bind(m_numVertexBuffers, m_firstInstance);
	}
	if(m_indexBuffer)
	{
		m_indexBuffer->bind();
	}
}

void RendererMesh::render(RendererMaterial *material) const
{
	if(m_instanceBuffer)
	{
		if(m_indexBuffer)
		{
			renderIndicesInstanced(m_numVertices, m_firstIndex, m_numIndices, m_indexBuffer->getFormat(), material);
		}
		else if(m_numVertices)
		{
			renderVerticesInstanced(m_numVertices, material);
		}
	}
	else
	{
		if(m_indexBuffer)
		{
			renderIndices(m_numVertices, m_firstIndex, m_numIndices, m_indexBuffer->getFormat(), material);
		}
		else if(m_numVertices)
		{
			renderVertices(m_numVertices, material);
		}
	}
}

void RendererMesh::unbind(void) const
{
	if(m_indexBuffer)
	{
		m_indexBuffer->unbind();
	}
	if(m_instanceBuffer)
	{
		m_instanceBuffer->unbind(m_numVertexBuffers);
	}
	for(PxU32 i=0; i<m_numVertexBuffers; i++)
	{
		m_vertexBuffers[i]->unbind(i);
	}
}

