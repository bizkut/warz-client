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
#ifndef RENDERER_VERTEXBUFFER_H
#define RENDERER_VERTEXBUFFER_H

#include <RendererConfig.h>
#include "RendererInteropableBuffer.h"

namespace SampleRenderer
{

	class RendererVertexBufferDesc;

	class RendererVertexBuffer: public RendererInteropableBuffer
	{
		friend class RendererMesh;
		friend class GLES2Renderer;
	public:
		enum Semantic
		{
			SEMANTIC_POSITION = 0,
			SEMANTIC_COLOR,
			SEMANTIC_NORMAL,
			SEMANTIC_TANGENT,

			SEMANTIC_DISPLACEMENT_TEXCOORD,
			SEMANTIC_DISPLACEMENT_FLAGS,

			SEMANTIC_BONEINDEX,
			SEMANTIC_BONEWEIGHT,

			SEMANTIC_TEXCOORD0,
			SEMANTIC_TEXCOORD1,
			SEMANTIC_TEXCOORD2,
			SEMANTIC_TEXCOORD3,
			SEMANTIC_TEXCOORDMAX = SEMANTIC_TEXCOORD3,

			NUM_SEMANTICS,
			NUM_TEXCOORD_SEMANTICS = SEMANTIC_TEXCOORDMAX - SEMANTIC_TEXCOORD0,
		}_Semantic;

		enum Format
		{
			FORMAT_FLOAT1 = 0,
			FORMAT_FLOAT2,
			FORMAT_FLOAT3,
			FORMAT_FLOAT4,

			FORMAT_UBYTE4,
			FORMAT_USHORT4,

			FORMAT_COLOR_BGRA, // RendererColor
			FORMAT_COLOR_RGBA, // OGL format
			FORMAT_COLOR_NATIVE, // do not convert

			NUM_FORMATS,
		}_Format;

		enum Hint
		{
			HINT_STATIC = 0,
			HINT_DYNAMIC,
		}_Hint;

	public:
		static PxU32 getFormatByteSize(Format format);

	protected:
		RendererVertexBuffer(const RendererVertexBufferDesc &desc);
		virtual ~RendererVertexBuffer(void);

	public:
		void release(void) { delete this; }

		PxU32  getMaxVertices(void) const;

		Hint   getHint(void) const;
		Format getFormatForSemantic(Semantic semantic) const;

		void *lockSemantic(Semantic semantic, PxU32 &stride);
		void  unlockSemantic(Semantic semantic);

		//Checks buffer written state for vertex buffers in D3D
		virtual bool checkBufferWritten() { return true; }

	private:
		virtual void  swizzleColor(void *colors, PxU32 stride, PxU32 numColors, RendererVertexBuffer::Format inFormat) = 0;

		virtual void *lock(void) = 0;
		virtual void  unlock(void) = 0;

		virtual void  bind(PxU32 streamID, PxU32 firstVertex) = 0;
		virtual void  unbind(PxU32 streamID) = 0;

		RendererVertexBuffer &operator=(const RendererVertexBuffer &) { return *this; }

	protected:
		void prepareForRender(void);

	protected:
		class SemanticDesc
		{
		public:
			Format format;
			PxU32  offset;
			bool   locked;
		public:
			SemanticDesc(void)
			{
				format = NUM_FORMATS;
				offset = 0;
				locked = false;
			}
		};

	protected:
		const Hint   m_hint;
		PxU32        m_maxVertices;
		PxU32        m_stride;
		bool         m_deferredUnlock;
		SemanticDesc m_semanticDescs[NUM_SEMANTICS];

	private:
		void        *m_lockedBuffer;
		PxU32        m_numSemanticLocks;
	};

} // namespace SampleRenderer

#endif
