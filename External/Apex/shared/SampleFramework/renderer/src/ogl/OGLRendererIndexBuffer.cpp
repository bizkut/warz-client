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

#if defined(RENDERER_ENABLE_OPENGL) 

#include "OGLRendererIndexBuffer.h"
#include <RendererIndexBufferDesc.h>

#if defined(PX_WINDOWS)
#include <PxTaskIncludes.h>
#endif

using namespace SampleRenderer;

OGLRendererIndexBuffer::OGLRendererIndexBuffer(const RendererIndexBufferDesc &desc) :
	RendererIndexBuffer(desc)
{
	m_indexSize  = getFormatByteSize(getFormat());

	RENDERER_ASSERT(GLEW_ARB_vertex_buffer_object, "Vertex Buffer Objects not supported on this machine!");
	if(GLEW_ARB_vertex_buffer_object)
	{
		RENDERER_ASSERT(desc.maxIndices > 0, "Cannot create zero size Index Buffer.");
		if(desc.maxIndices > 0)
		{
			GLenum usage = GL_STATIC_DRAW_ARB;
			if(getHint() == HINT_DYNAMIC)
			{
				usage = GL_DYNAMIC_DRAW_ARB;
			}

			glGenBuffersARB(1, &m_ibo);
			RENDERER_ASSERT(m_ibo, "Failed to create Index Buffer.");
			if(m_ibo)
			{
				m_maxIndices = desc.maxIndices;
				const PxU32 bufferSize = m_indexSize * m_maxIndices;

				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_ibo);
				glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, bufferSize, 0, usage);
				glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

#if defined(PX_WINDOWS)
				if(m_interopContext && m_mustBeRegisteredInCUDA)
				{
					m_registeredInCUDA = m_interopContext->registerResourceInCudaGL(m_InteropHandle, (PxU32) m_ibo);
				}
#endif
			}
		}
	}
}

OGLRendererIndexBuffer::~OGLRendererIndexBuffer(void)
{
	if(m_ibo)
	{
#if defined(PX_WINDOWS)
		if(m_registeredInCUDA)
		{
			m_interopContext->unregisterResourceInCuda(m_InteropHandle);
		}
#endif
		glDeleteBuffersARB(1, &m_ibo);
	}
}

void *OGLRendererIndexBuffer::lock(void)
{
	void *buffer = 0;
	if(m_ibo)
	{
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_ibo);
		buffer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_READ_WRITE);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
	}
	return buffer;
}

void OGLRendererIndexBuffer::unlock(void)
{
	if(m_ibo)
	{
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_ibo);
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER_ARB);
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
	}
}

void OGLRendererIndexBuffer::bind(void) const
{
	if(m_ibo)
	{
#if !defined(RENDERER_PS3)
		glEnableClientState(GL_INDEX_ARRAY);
#endif
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, m_ibo);
	}
}

void OGLRendererIndexBuffer::unbind(void) const
{
	if(m_ibo)
	{
		glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
#if !defined(RENDERER_PS3)
		glDisableClientState(GL_INDEX_ARRAY);
#endif
	}
}

#endif // #if defined(RENDERER_ENABLE_OPENGL)
