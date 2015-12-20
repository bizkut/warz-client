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

#include <RendererClothShape.h>

#include <Renderer.h>

#include <RendererVertexBuffer.h>
#include <RendererVertexBufferDesc.h>

#include <RendererIndexBuffer.h>
#include <RendererIndexBufferDesc.h>

#include <RendererMesh.h>
#include <RendererMeshDesc.h>

#include <RendererMemoryMacros.h>

using namespace SampleRenderer;

RendererClothShape::RendererClothShape(	Renderer& renderer, 
	const PxVec3* verts, PxU32 numVerts, 
	const PxVec3* normals,
	const PxReal* uvs,
	const PxU16* faces, PxU32 numFaces, bool flipWinding) :
RendererShape(renderer)
{
	RendererVertexBufferDesc vbdesc;
	vbdesc.hint = RendererVertexBuffer::HINT_DYNAMIC;
	vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_POSITION]  = RendererVertexBuffer::FORMAT_FLOAT3;
	vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_NORMAL]    = RendererVertexBuffer::FORMAT_FLOAT3;
	vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_TEXCOORD0] = RendererVertexBuffer::FORMAT_FLOAT2;
	vbdesc.maxVertices = numVerts;
	m_vertexBuffer = m_renderer.createVertexBuffer(vbdesc);
	RENDERER_ASSERT(m_vertexBuffer, "Failed to create Vertex Buffer.");
	if(m_vertexBuffer)
	{
		PxU32 positionStride = 0;
		void* vertPositions = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, positionStride);

		PxU32 normalStride = 0;
		void* vertNormals = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL, normalStride);

		PxU32 uvStride = 0;
		void* vertUVs = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0, uvStride);

		if(vertPositions && vertNormals && vertUVs)
		{
			for(PxU32 i=0; i<numVerts; i++)
			{
				memcpy(vertPositions, verts+i, sizeof(PxVec3));
				if(normals)
					memcpy(vertNormals, normals+i, sizeof(PxVec3));
				else
					memset(vertNormals, 0, sizeof(PxVec3));

				if(uvs)
					memcpy(vertUVs, uvs+i*2, sizeof(PxReal)*2);
				else
					memset(vertUVs, 0, sizeof(PxReal)*2);

				vertPositions = (void*)(((PxU8*)vertPositions) + positionStride);
				vertNormals   = (void*)(((PxU8*)vertNormals)   + normalStride);
				vertUVs       = (void*)(((PxU8*)vertUVs)       + uvStride);
			}
		}
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_NORMAL);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_TEXCOORD0);
	}

	const PxU32 numIndices = numFaces*3;

	RendererIndexBufferDesc ibdesc;
	ibdesc.hint       = RendererIndexBuffer::HINT_STATIC;
	ibdesc.format     = RendererIndexBuffer::FORMAT_UINT16;
	ibdesc.maxIndices = numIndices;
	m_indexBuffer = m_renderer.createIndexBuffer(ibdesc);
	RENDERER_ASSERT(m_indexBuffer, "Failed to create Index Buffer.");
	if(m_indexBuffer)
	{
		PxU16* indices = (PxU16*)m_indexBuffer->lock();
		if(indices)
		{
			if(flipWinding)
			{
				for(PxU32 i=0;i<numFaces;i++)
				{
					indices[i*3+0] = faces[i*3+0];
					indices[i*3+1] = faces[i*3+2];
					indices[i*3+2] = faces[i*3+1];
				}
			}
			else
			{
				memcpy(indices, faces, sizeof(*faces)*numFaces*3);
			}
		}
		m_indexBuffer->unlock();
	}

	if(m_vertexBuffer && m_indexBuffer)
	{
		RendererMeshDesc meshdesc;
		meshdesc.primitives       = RendererMesh::PRIMITIVE_TRIANGLES;
		meshdesc.vertexBuffers    = &m_vertexBuffer;
		meshdesc.numVertexBuffers = 1;
		meshdesc.firstVertex      = 0;
		meshdesc.numVertices      = numVerts;
		meshdesc.indexBuffer      = m_indexBuffer;
		meshdesc.firstIndex       = 0;
		meshdesc.numIndices       = numIndices;
		m_mesh = m_renderer.createMesh(meshdesc);
		RENDERER_ASSERT(m_mesh, "Failed to create Mesh.");
	}
}

void RendererClothShape::update(const PxVec3* verts, PxU32 numVerts, const PxVec3* normals)
{
    PxU32 positionStride = 0, normalStride = 0;

    PxU8 *locked_positions = static_cast<PxU8*>(m_vertexBuffer->lockSemantic(
                              RendererVertexBuffer::SEMANTIC_POSITION, positionStride));
    PxU8 *locked_normals = static_cast<PxU8*>(m_vertexBuffer->lockSemantic(
                              RendererVertexBuffer::SEMANTIC_NORMAL, normalStride));
    
    for(PxU32 i=0; i<numVerts; i++)
    {
        memcpy(locked_positions, verts + i, sizeof(PxVec3));
        memcpy(locked_normals,  normals + i, sizeof(PxVec3));

        locked_positions += positionStride;
        locked_normals += normalStride;
    }

    m_vertexBuffer->unlockSemantic(SampleRenderer::RendererVertexBuffer::SEMANTIC_NORMAL);
    m_vertexBuffer->unlockSemantic(SampleRenderer::RendererVertexBuffer::SEMANTIC_POSITION);

    m_mesh->setVertexBufferRange(0, numVerts);
}

RendererClothShape::~RendererClothShape(void)
{
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_indexBuffer);
	SAFE_RELEASE(m_mesh);
}
