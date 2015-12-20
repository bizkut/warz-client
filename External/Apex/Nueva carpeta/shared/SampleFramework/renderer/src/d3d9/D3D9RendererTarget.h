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
#ifndef D3D9_RENDERER_TARGET_H
#define D3D9_RENDERER_TARGET_H

#include <RendererConfig.h>

// TODO: 360 can't render directly into texture memory, so we need to create a pool of
//       surfaces to share internally and then "resolve" them into the textures inside of
//       the 'unbind' call.
#if defined(RENDERER_WINDOWS)
	#define RENDERER_ENABLE_DIRECT3D9_TARGET
#endif

#if defined(RENDERER_ENABLE_DIRECT3D9) && defined(RENDERER_ENABLE_DIRECT3D9_TARGET)

#include <RendererTarget.h>
#include "D3D9Renderer.h"

namespace SampleRenderer
{

	class D3D9RendererTexture2D;

	class D3D9RendererTarget : public RendererTarget, public D3D9RendererResource
	{
	public:
		D3D9RendererTarget(IDirect3DDevice9 &d3dDevice, const RendererTargetDesc &desc);
		virtual ~D3D9RendererTarget(void);

	private:
		D3D9RendererTarget& operator=( const D3D9RendererTarget& ) {}
		virtual void bind(void);
		virtual void unbind(void);

	private:
		virtual void onDeviceLost(void);
		virtual void onDeviceReset(void);

	private:
		IDirect3DDevice9               &m_d3dDevice;
		IDirect3DSurface9              *m_d3dLastSurface;
		IDirect3DSurface9              *m_d3dLastDepthStencilSurface;
		IDirect3DSurface9              *m_d3dDepthStencilSurface;
		std::vector<D3D9RendererTexture2D*> m_textures;
		D3D9RendererTexture2D          *m_depthStencilSurface;
	};

} // namespace SampleRenderer

#endif // #if defined(RENDERER_ENABLE_DIRECT3D9) && defined(RENDERER_ENABLE_DIRECT3D9_TARGET)
#endif
