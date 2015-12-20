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
#ifndef D3D9_RENDERER_INSTANCEBUFFER_H
#define D3D9_RENDERER_INSTANCEBUFFER_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_DIRECT3D9)

#include <RendererInstanceBuffer.h>
#include "D3D9Renderer.h"

namespace SampleRenderer
{

	class D3D9RendererInstanceBuffer : public RendererInstanceBuffer, public D3D9RendererResource
	{
	public:
		D3D9RendererInstanceBuffer(IDirect3DDevice9 &d3dDevice, const RendererInstanceBufferDesc &desc);
		virtual ~D3D9RendererInstanceBuffer(void);

		void addVertexElements(PxU32 streamIndex, std::vector<D3DVERTEXELEMENT9> &vertexElements) const;

	protected:
		virtual void *lock(void);
		virtual void  unlock(void);

		virtual void  bind(PxU32 streamID, PxU32 firstInstance) const;
		virtual void  unbind(PxU32 streamID) const;

	private:
		virtual void onDeviceLost(void);
		virtual void onDeviceReset(void);

	private:
#if RENDERER_INSTANCING
		IDirect3DDevice9       &m_d3dDevice;
		IDirect3DVertexBuffer9 *m_d3dVertexBuffer;
#else
		void					*mInstanceBuffer;
#endif
		DWORD                   m_usage;
		D3DPOOL                 m_pool;
		UINT                    m_bufferSize;
	};

} // namespace SampleRenderer

#endif // #if defined(RENDERER_ENABLE_DIRECT3D9)
#endif
