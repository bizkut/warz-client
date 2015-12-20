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
#ifndef RENDERER_MESH_H
#define RENDERER_MESH_H

#include <RendererConfig.h>
#include <RendererIndexBuffer.h>

namespace SampleRenderer
{
	class Renderer;
	class RendererMeshDesc;
	class RendererVertexBuffer;
	class RendererIndexBuffer;
	class RendererInstanceBuffer;
	class RendererMaterial;

	class RendererMesh
	{
		friend class Renderer;
		friend class GLES2Renderer;
	public:
		enum Primitive
		{
			PRIMITIVE_POINTS = 0,
			PRIMITIVE_LINES,
			PRIMITIVE_LINE_STRIP,
			PRIMITIVE_TRIANGLES,
			PRIMITIVE_TRIANGLE_STRIP,
			PRIMITIVE_POINT_SPRITES,

			NUM_PRIMITIVES
		}_Primitive;

	protected:
		RendererMesh(const RendererMeshDesc &desc);
		virtual ~RendererMesh(void);

	public:
		void release(void);

		Primitive getPrimitives(void) const;

		PxU32 getNumVertices(void) const;
		PxU32 getNumIndices(void) const;
		PxU32 getNumInstances(void) const;

		void setVertexBufferRange(PxU32 firstVertex, PxU32 numVertices);
		void setIndexBufferRange(PxU32 firstIndex, PxU32 numIndices);
		void setInstanceBufferRange(PxU32 firstInstance, PxU32 numInstances);

		PxU32                             getNumVertexBuffers(void) const;
		const RendererVertexBuffer *const*getVertexBuffers(void) const;

		const RendererIndexBuffer        *getIndexBuffer(void) const;

		const RendererInstanceBuffer     *getInstanceBuffer(void) const;

		virtual bool willRender() const { return true; }

	protected:
		void         bind(void) const;
		void         render(RendererMaterial *material) const;
		void         unbind(void) const;

		virtual void renderIndices(PxU32 numVertices, PxU32 firstIndex, PxU32 numIndices, RendererIndexBuffer::Format indexFormat, RendererMaterial *material) const = 0;
		virtual void renderVertices(PxU32 numVertices, RendererMaterial *material) const = 0;

		virtual void renderIndicesInstanced(PxU32 numVertices, PxU32 firstIndex, PxU32 numIndices, RendererIndexBuffer::Format indexFormat,RendererMaterial *material) const = 0;
		virtual void renderVerticesInstanced(PxU32 numVertices,RendererMaterial *material) const = 0;

		virtual Renderer& renderer() = 0;

	protected:
		Primitive               m_primitives;

		RendererVertexBuffer  **m_vertexBuffers;
		PxU32                   m_numVertexBuffers;
		PxU32                   m_firstVertex;
		PxU32                   m_numVertices;

		RendererIndexBuffer    *m_indexBuffer;
		PxU32                   m_firstIndex;
		PxU32                   m_numIndices;

		RendererInstanceBuffer *m_instanceBuffer;
		PxU32                   m_firstInstance;
		PxU32                   m_numInstances;
	};

} // namespace SampleRenderer

#endif
