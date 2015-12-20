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
//
// RendererGridShape : convenience class for generating a grid mesh.
//
#include <RendererGridShape.h>

#include <Renderer.h>

#include <RendererVertexBuffer.h>
#include <RendererVertexBufferDesc.h>

#ifdef RENDERER_PSP2
#include <RendererIndexBuffer.h>
#include <RendererIndexBufferDesc.h>
#endif

#include <RendererMesh.h>
#include <RendererMeshDesc.h>

#include <RendererMemoryMacros.h>

#include <foundation/PxStrideIterator.h>
#include <PsMathUtils.h>

using namespace SampleRenderer;

#ifdef RENDERER_PSP2
RendererGridShape::RendererGridShape(Renderer &renderer, PxU32 size, float cellSize, bool showAxis /*= false*/, UpAxis up/*= UP_Y*/) :
RendererShape(renderer), m_UpAxis(up)
{
	m_vertexBuffer = 0;

	const PxU32 numVerts    = (size + 1)*(size + 1);

	RendererVertexBufferDesc vbdesc;
	vbdesc.hint                                                     = RendererVertexBuffer::HINT_STATIC;
	vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_POSITION] = RendererVertexBuffer::FORMAT_FLOAT3;
	vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_NORMAL]   = RendererVertexBuffer::FORMAT_FLOAT3;
	vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_TEXCOORD0] = RendererVertexBuffer::FORMAT_FLOAT2;
	vbdesc.maxVertices                                              = numVerts;
	m_vertexBuffer = m_renderer.createVertexBuffer(vbdesc);

	m_indexBuffer = 0;

	RendererIndexBufferDesc ibDesc;
	ibDesc.hint = RendererIndexBuffer::HINT_STATIC;
	ibDesc.format = RendererIndexBuffer::FORMAT_UINT16;
	ibDesc.maxIndices = (size + 1)*(size + 1)*2*3;

	m_indexBuffer = m_renderer.createIndexBuffer(ibDesc);

	if(m_vertexBuffer)
	{
		RendererMeshDesc meshdesc;
		meshdesc.primitives       = RendererMesh::PRIMITIVE_LINES;
		meshdesc.vertexBuffers    = &m_vertexBuffer;
		meshdesc.numVertexBuffers = 1;
		meshdesc.indexBuffer      = m_indexBuffer;
		meshdesc.firstIndex		  = 0;
		meshdesc.numIndices		  = ibDesc.maxIndices;
		meshdesc.firstVertex      = 0;
		meshdesc.numVertices      = numVerts;
		m_mesh = m_renderer.createMesh(meshdesc);
	}
	if(m_vertexBuffer && m_mesh)
	{
		PxU32 color1 = m_renderer.convertColor(RendererColor ( 94, 108, 127));
		PxU32 color2 = m_renderer.convertColor(RendererColor (120, 138, 163));
		PxU32 colorRed   = m_renderer.convertColor(RendererColor (255,   0,   0));
		PxU32 colorGreen = m_renderer.convertColor(RendererColor (  0, 255,   0));
		PxU32 colorBlue  = m_renderer.convertColor(RendererColor (  0,   0, 255));

		PxStrideIterator<PxVec3> positions;
		{
			PxU32 positionStride = 0;
			void* pos = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, positionStride);
			positions = PxMakeIterator((PxVec3*)pos, positionStride);
		}

		PxU16* indices = NULL;
		if(m_indexBuffer)
		{
			indices = (PxU16*) m_indexBuffer->lock();
		}

		if(positions.ptr())
		{
			PxVec3 upAxis;
			
			switch(up)
			{
			case UP_X: upAxis = PxVec3(1.0f, 0.0f, 0.0f); break;
			case UP_Y: upAxis = PxVec3(0.0f, 1.0f, 0.0f); break;
			case UP_Z: upAxis = PxVec3(0.0f, 0.0f, 1.0f); break;
			}
			PxMat33 rotation = physx::shdfnd::rotFrom2Vectors(PxVec3(0.0f, 1.0f, 0.0f), upAxis);

			float radius   = size*cellSize;
			float halfRadius = radius * 0.5f;

			PxVec3 tempV;

			for (PxU32 i = 0; i <= size; i++)
			{
				for (PxU32 j = 0; j <= size; j++)
				{
					tempV = PxVec3( -halfRadius + j*cellSize, 0 , -halfRadius + i*cellSize);
					
					positions[(i * (size + 1)) + j] = rotation * tempV;					
				}
			}

			PxU32 numSquares = 0;
			for (PxU32 i = 0; i < size; i++)
			{
				for (PxU32 j = 0; j < size; j++)
				{					
					indices[numSquares * 6]	 =	(i + 1)*(size + 1) + j + 1;
					indices[numSquares * 6 + 1] =  (i * (size + 1)) + j; 
					indices[numSquares * 6 + 2] =  (i * (size + 1)) + j + 1;
					indices[numSquares * 6 + 3] =  (i + 1)*(size + 1) + j + 1;
					indices[numSquares * 6 + 4] =   (i +1)*(size + 1) + j;
					indices[numSquares * 6 + 5] =  (i*(size + 1)) + j;

					numSquares++;
				}
			}

		}


		m_indexBuffer->unlock();
		
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
	}
}
#else
RendererGridShape::RendererGridShape(Renderer &renderer, PxU32 size, float cellSize, bool showAxis /*= false*/, UpAxis up/*= UP_Y*/) :
RendererShape(renderer), m_UpAxis(up)
{
	m_vertexBuffer = 0;

	const PxU32 numVerts    = size*8 + 8;

	RendererVertexBufferDesc vbdesc;
	vbdesc.hint                                                     = RendererVertexBuffer::HINT_STATIC;
	vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_POSITION] = RendererVertexBuffer::FORMAT_FLOAT3;
	vbdesc.semanticFormats[RendererVertexBuffer::SEMANTIC_COLOR]    = RendererVertexBuffer::FORMAT_COLOR_NATIVE;
	vbdesc.maxVertices                                              = numVerts;
	m_vertexBuffer = m_renderer.createVertexBuffer(vbdesc);

	if(m_vertexBuffer)
	{
		RendererMeshDesc meshdesc;
		meshdesc.primitives       = RendererMesh::PRIMITIVE_LINES;
		meshdesc.vertexBuffers    = &m_vertexBuffer;
		meshdesc.numVertexBuffers = 1;
		meshdesc.firstVertex      = 0;
		meshdesc.numVertices      = numVerts;
		m_mesh = m_renderer.createMesh(meshdesc);
	}
	if(m_vertexBuffer && m_mesh)
	{
		PxU32 color1 = m_renderer.convertColor(RendererColor ( 94, 108, 127));
		PxU32 color2 = m_renderer.convertColor(RendererColor (120, 138, 163));
		PxU32 colorRed   = m_renderer.convertColor(RendererColor (255,   0,   0));
		PxU32 colorGreen = m_renderer.convertColor(RendererColor (  0, 255,   0));
		PxU32 colorBlue  = m_renderer.convertColor(RendererColor (  0,   0, 255));

		PxStrideIterator<PxVec3> positions;
		{
			PxU32 positionStride = 0;
			void* pos = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_POSITION, positionStride);
			positions = PxMakeIterator((PxVec3*)pos, positionStride);
		}
		PxStrideIterator<PxU32> colors;
		{
			PxU32 colorStride = 0;
			void* color = m_vertexBuffer->lockSemantic(RendererVertexBuffer::SEMANTIC_COLOR, colorStride);
			colors = PxMakeIterator((PxU32*)color, colorStride);
		}

		if(positions.ptr() && colors.ptr())
		{
			PxVec3 upAxis(0.0f);

			PxU32 colorX1 = color1;
			PxU32 colorX2 = color1;
			PxU32 colorZ1 = color1;
			PxU32 colorZ2 = color1;
			
			switch(up)
			{
			case UP_X: upAxis = PxVec3(1.0f, 0.0f, 0.0f); break;
			case UP_Y: upAxis = PxVec3(0.0f, 1.0f, 0.0f); break;
			case UP_Z: upAxis = PxVec3(0.0f, 0.0f, 1.0f); break;
			}
			PxMat33 rotation = physx::shdfnd::rotFrom2Vectors(PxVec3(0.0f, 1.0f, 0.0f), upAxis);

			if (showAxis)
			{
				switch(up)
				{
				case UP_X: colorX2 = colorGreen; colorZ1 = colorBlue; break;
				case UP_Y: colorX1 = colorRed; colorZ1 = colorBlue; break;
				case UP_Z: colorX1 = colorRed; colorZ2 = colorGreen; break;
				}
			}

			float radius   = size*cellSize;

			positions[0] = rotation * PxVec3(   0.0f, 0.0f,   0.0f); colors[0] = colorX1;
			positions[1] = rotation * PxVec3( radius, 0.0f,   0.0f); colors[1] = colorX1;
			positions[2] = rotation * PxVec3(   0.0f, 0.0f,   0.0f); colors[2] = colorX2;
			positions[3] = rotation * PxVec3(-radius, 0.0f,   0.0f); colors[3] = colorX2;
			positions[4] = rotation * PxVec3(   0.0f, 0.0f,   0.0f); colors[4] = colorZ1;
			positions[5] = rotation * PxVec3(   0.0f, 0.0f, radius); colors[5] = colorZ1;
			positions[6] = rotation * PxVec3(   0.0f, 0.0f,   0.0f); colors[6] = colorZ2;
			positions[7] = rotation * PxVec3(   0.0f, 0.0f,-radius); colors[7] = colorZ2;


			for (PxU32 i = 1; i <= size; i++)
			{
				positions[i*8+0] = rotation * PxVec3(-radius, 0.0f, cellSize * i);
				positions[i*8+1] = rotation * PxVec3( radius, 0.0f, cellSize * i);
				positions[i*8+2] = rotation * PxVec3(-radius, 0.0f,-cellSize * i);
				positions[i*8+3] = rotation * PxVec3( radius, 0.0f,-cellSize * i);
				positions[i*8+4] = rotation * PxVec3( cellSize * i, 0.0f,-radius);
				positions[i*8+5] = rotation * PxVec3( cellSize * i, 0.0f, radius);
				positions[i*8+6] = rotation * PxVec3(-cellSize * i, 0.0f,-radius);
				positions[i*8+7] = rotation * PxVec3(-cellSize * i, 0.0f, radius);

				for (PxU32 j = 0; j < 8; j++)
					colors[i*8+j] = color2;
			}
		}

		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_COLOR);
		m_vertexBuffer->unlockSemantic(RendererVertexBuffer::SEMANTIC_POSITION);
	}
}
#endif

RendererGridShape::~RendererGridShape(void)
{
#ifdef RENDERER_PSP2
	SAFE_RELEASE(m_indexBuffer);
#endif
	SAFE_RELEASE(m_vertexBuffer);
	SAFE_RELEASE(m_mesh);
}
