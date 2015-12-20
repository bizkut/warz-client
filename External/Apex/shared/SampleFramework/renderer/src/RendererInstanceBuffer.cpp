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
#include <RendererInstanceBuffer.h>
#include <RendererInstanceBufferDesc.h>

using namespace SampleRenderer;

PxU32 RendererInstanceBuffer::getFormatByteSize(Format format)
{
	PxU32 size = 0;
	switch(format)
	{
	case FORMAT_FLOAT1: size = sizeof(float)*1; break;
	case FORMAT_FLOAT2: size = sizeof(float)*2; break;
	case FORMAT_FLOAT3: size = sizeof(float)*3; break;
	case FORMAT_FLOAT4: size = sizeof(float)*4; break;
	default: break;
	}
	RENDERER_ASSERT(size, "Unable to determine size of Format.");
	return size;
}

RendererInstanceBuffer::RendererInstanceBuffer(const RendererInstanceBufferDesc &desc) :
RendererInteropableBuffer(desc.registerInCUDA, desc.interopContext),
	m_hint(desc.hint)
{
	m_maxInstances     = 0;
	m_stride           = 0;
	m_lockedBuffer     = 0;
	m_numSemanticLocks = 0;
	for(PxU32 i=0; i<NUM_SEMANTICS; i++)
	{
		Format format = desc.semanticFormats[i];
		if(format < NUM_FORMATS)
		{
			SemanticDesc &sm  = m_semanticDescs[i];
			sm.format         = format;
			sm.offset         = m_stride;
			m_stride         += getFormatByteSize(format);
		}
	}
}

RendererInstanceBuffer::~RendererInstanceBuffer(void)
{
	RENDERER_ASSERT(m_lockedBuffer==0 && m_numSemanticLocks==0, "InstanceBuffer had outstanding locks during destruction!");
}

RendererInstanceBuffer::Hint RendererInstanceBuffer::getHint(void) const
{
	return m_hint;
}

RendererInstanceBuffer::Format RendererInstanceBuffer::getFormatForSemantic(Semantic semantic) const
{
	return m_semanticDescs[semantic].format;
}

PxU32 SampleRenderer::RendererInstanceBuffer::getMaxInstances(void) const
{
	return m_maxInstances;
}

PxU32  RendererInstanceBuffer::getOffsetForSemantic(Semantic semantic) const
{
		return m_semanticDescs[semantic].offset;
}

void *RendererInstanceBuffer::lockSemantic(Semantic semantic, PxU32 &stride)
{
	void *semanticBuffer = 0;
	RENDERER_ASSERT(semantic < NUM_SEMANTICS, "Invalid InstanceBuffer Semantic!");
	if(semantic < NUM_SEMANTICS)
	{
		SemanticDesc &sm = m_semanticDescs[semantic];
		RENDERER_ASSERT(!sm.locked,              "InstanceBuffer Semantic already locked.");
		RENDERER_ASSERT(sm.format < NUM_FORMATS, "InstanceBuffer Semantic format not valid.");
		if(!sm.locked && sm.format < NUM_FORMATS)
		{
			if(!m_lockedBuffer && !m_numSemanticLocks)
			{
				m_lockedBuffer = lock();
			}
			RENDERER_ASSERT(m_lockedBuffer, "Unable to lock InstanceBuffer!");
			if(m_lockedBuffer)
			{
				m_numSemanticLocks++;
				sm.locked        = true;
				semanticBuffer   = ((PxU8*)m_lockedBuffer) + sm.offset;
				stride           = m_stride;
			}
		}
	}
	return semanticBuffer;
}

void RendererInstanceBuffer::unlockSemantic(Semantic semantic)
{
	RENDERER_ASSERT(semantic < NUM_SEMANTICS, "Invalid InstanceBuffer Semantic!");
	if(semantic < NUM_SEMANTICS)
	{
		SemanticDesc &sm = m_semanticDescs[semantic];
		RENDERER_ASSERT(m_lockedBuffer && m_numSemanticLocks && sm.locked, "Trying to unlock a semantic that was not locked.");
		if(m_lockedBuffer && m_numSemanticLocks && sm.locked)
		{
			sm.locked = false;
			m_numSemanticLocks--;
		}
		if(m_lockedBuffer && m_numSemanticLocks == 0)
		{
			unlock();
			m_lockedBuffer = 0;
		}
	}
}

