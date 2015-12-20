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

#include "GXMRendererMesh.h"

#include "GXMRendererInstanceBuffer.h"
#include "GXMRendererMaterial.h"

using namespace SampleRenderer;


GXMRendererMesh::GXMRendererMesh(GXMRenderer &renderer, const RendererMeshDesc &desc) :
	RendererMesh(desc), m_renderer(renderer), mRendererIndexBuffer(0), mCurrentRendererIndexBufferSize(0), mMaxVerts(0),
		mRendererIndexBufferUid(0)
{
	InitVertexRenderer();
}

GXMRendererMesh::~GXMRendererMesh(void)
{

	GXMRenderer::graphicsFree(mRendererIndexBufferUid);

}

void GXMRendererMesh::InitVertexRenderer()
{
	mMaxVerts = 4096*3;

	mRendererIndexBuffer = GXMRenderer::graphicsAlloc(SCE_KERNEL_MEMBLOCK_TYPE_USER_RWDATA_UNCACHE,
		sizeof(PxU16) * mMaxVerts,
		8,
		SCE_GXM_MEMORY_ATTRIB_READ,
		&mRendererIndexBufferUid);

	PX_ASSERT(mRendererIndexBuffer);

	mCurrentRendererIndexBufferSize = 0;
}

void GXMRendererMesh::renderIndices(PxU32 numVertices, PxU32 firstIndex, PxU32 numIndices, RendererIndexBuffer::Format indexFormat) const
{
	if(m_renderer.getShaderEnvironment().mValid)
	{
		Primitive prim = getPrimitives();

		switch(prim)
		{
		case PRIMITIVE_LINES:
			{				
				sceGxmSetCullMode( m_renderer.getContext(), SCE_GXM_CULL_NONE);

				sceGxmSetFrontPolygonMode( m_renderer.getContext(), SCE_GXM_POLYGON_MODE_TRIANGLE_LINE );
				sceGxmSetBackPolygonMode( m_renderer.getContext(), SCE_GXM_POLYGON_MODE_TRIANGLE_LINE );
				/*sceGxmSetFrontDepthFunc( m_renderer.GetContext(), SCE_GXM_DEPTH_FUNC_ALWAYS );
				sceGxmSetBackDepthFunc( m_renderer.GetContext(), SCE_GXM_DEPTH_FUNC_ALWAYS );*/

				SceGxmErrorCode err = sceGxmDraw(m_renderer.getContext(), SCE_GXM_PRIMITIVE_TRIANGLES , SCE_GXM_INDEX_FORMAT_U16, m_indexBuffer->lock(),m_numIndices); 

				RENDERER_ASSERT(err == SCE_OK, "Failed to render!");				

				sceGxmSetFrontPolygonMode( m_renderer.getContext(), SCE_GXM_POLYGON_MODE_TRIANGLE_FILL );
				sceGxmSetBackPolygonMode( m_renderer.getContext(), SCE_GXM_POLYGON_MODE_TRIANGLE_FILL );
				//sceGxmSetFrontDepthFunc( m_renderer.GetContext(), SCE_GXM_DEPTH_FUNC_ALWAYS );
				//sceGxmSetBackDepthFunc( m_renderer.GetContext(), SCE_GXM_DEPTH_FUNC_ALWAYS );
			}
			break;
		case PRIMITIVE_TRIANGLES:
			{							
				SceGxmErrorCode err = sceGxmDraw(m_renderer.getContext(), SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, m_indexBuffer->lock(),m_numIndices);

				if(err != SCE_OK)
				{
					RENDERER_ASSERT(err == SCE_OK, "Failed to render!");	
				}			
			}
			break;
		case PRIMITIVE_POINT_SPRITES:
			{				
				sceGxmSetFrontDepthWriteEnable(m_renderer.getContext(), SCE_GXM_DEPTH_WRITE_DISABLED);
				sceGxmSetFrontDepthFunc( m_renderer.getContext(), SCE_GXM_DEPTH_FUNC_LESS_EQUAL);

				sceGxmSetFrontPolygonMode(m_renderer.getContext(),SCE_GXM_POLYGON_MODE_POINT_01UV);

				SceGxmErrorCode err = sceGxmDraw(m_renderer.getContext(), SCE_GXM_PRIMITIVE_POINTS, SCE_GXM_INDEX_FORMAT_U16 , m_indexBuffer->lock(), m_numIndices);

				if(err != SCE_OK)
				{
					RENDERER_ASSERT(err == SCE_OK, "Failed to render!");
				}

				sceGxmSetFrontPolygonMode(m_renderer.getContext(),SCE_GXM_POLYGON_MODE_TRIANGLE_FILL);

				sceGxmSetFrontDepthWriteEnable(m_renderer.getContext(), SCE_GXM_DEPTH_WRITE_ENABLED);
				sceGxmSetFrontDepthFunc( m_renderer.getContext(), SCE_GXM_DEPTH_FUNC_LESS_EQUAL);
			}
			break;
		}
		m_renderer.getShaderEnvironment().mValid = false;
	}

}

bool GXMRendererMesh::willRender() const
{
	if(m_indexBuffer)
		return true;

	if(m_primitives == PRIMITIVE_TRIANGLES || m_primitives == PRIMITIVE_LINES)
	{
		if(m_numVertices > 0)
			return true;
	}

	return false;
}

void GXMRendererMesh::renderVertices(PxU32 numVertices) const
{
	Primitive prim = getPrimitives();

	switch(prim)
	{
	case PRIMITIVE_TRIANGLES:
		{
			void* currentRendererIndexBuffer = NULL;

			if((mCurrentRendererIndexBufferSize + sizeof(PxU16)*numVertices) > mMaxVerts*sizeof(PxU16))
			{
				mCurrentRendererIndexBufferSize = 0;
			}	

			currentRendererIndexBuffer = static_cast<PxU8*>(mRendererIndexBuffer) + mCurrentRendererIndexBufferSize;
			mCurrentRendererIndexBufferSize += sizeof(PxU16)*numVertices;

			PxU16* ib = (PxU16*) currentRendererIndexBuffer;

			for (PxU16 i = 0; i < numVertices; i++)
			{
				ib[i] = i;
			}

			SceGxmErrorCode err = sceGxmDraw(m_renderer.getContext(), SCE_GXM_PRIMITIVE_TRIANGLES, SCE_GXM_INDEX_FORMAT_U16, currentRendererIndexBuffer,numVertices);

			if(err != SCE_OK)
			{
				RENDERER_ASSERT(err == SCE_OK, "Failed to render!");	
			}	
		}
		break;
	case PRIMITIVE_LINES:
		{
			void* currentRendererIndexBuffer = NULL;

			if((mCurrentRendererIndexBufferSize + sizeof(PxU16)*numVertices) > mMaxVerts*sizeof(PxU16))
			{
				mCurrentRendererIndexBufferSize = 0;
			}	

			currentRendererIndexBuffer = static_cast<PxU8*>(mRendererIndexBuffer) + mCurrentRendererIndexBufferSize;
			mCurrentRendererIndexBufferSize += sizeof(PxU16)*numVertices;

			PxU16* ib = (PxU16*) currentRendererIndexBuffer;

			for (PxU16 i = 0; i < numVertices; i++)
			{
				ib[i] = i;
			}

			sceGxmSetFrontPolygonMode( m_renderer.getContext(), SCE_GXM_POLYGON_MODE_LINE );
			sceGxmSetBackPolygonMode( m_renderer.getContext(), SCE_GXM_POLYGON_MODE_LINE );

			sceGxmSetRegionClip(m_renderer.getContext(), SCE_GXM_REGION_CLIP_NONE, 0, 0, 0, 0);

			SceGxmErrorCode err = sceGxmDraw(m_renderer.getContext(), SCE_GXM_PRIMITIVE_LINES, SCE_GXM_INDEX_FORMAT_U16, currentRendererIndexBuffer,numVertices);

			if(err != SCE_OK)
			{
				RENDERER_ASSERT(err == SCE_OK, "Failed to render!");	
			}	

			sceGxmSetFrontPolygonMode( m_renderer.getContext(), SCE_GXM_POLYGON_MODE_TRIANGLE_FILL );
			sceGxmSetBackPolygonMode( m_renderer.getContext(), SCE_GXM_POLYGON_MODE_TRIANGLE_FILL );

		}
		break;
	}



}

void GXMRendererMesh::renderIndicesInstanced(PxU32 numVertices, PxU32 firstIndex, PxU32 numIndices, RendererIndexBuffer::Format indexFormat,RendererMaterial *material) const
{
	if(m_numInstances > 0)
	{
		SceGxmErrorCode err = sceGxmDrawInstanced(m_renderer.getContext(), SCE_GXM_PRIMITIVE_TRIANGLES,	SCE_GXM_INDEX_FORMAT_U32, 
			m_indexBuffer->lock(), numIndices * m_numInstances, numIndices); 
		if(err != SCE_OK)
		{
			RENDERER_ASSERT(err == SCE_OK, "Failed to render!");
		}
	}
}

void GXMRendererMesh::renderVerticesInstanced(PxU32 numVertices,RendererMaterial *material) const
{
	//currently unsupported on PSP2
	RENDERER_ASSERT(0, "Cannot render vertices without indices?");
}

#endif // #if defined(RENDERER_ENABLE_LIBGXM)
		
