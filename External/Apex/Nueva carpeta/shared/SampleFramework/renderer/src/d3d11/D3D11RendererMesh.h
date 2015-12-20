/*
 * Copyright 2009-2011 NVIDIA Corporation.  All rights reserved.
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
#ifndef D3D11_RENDERER_MESH_H
#define D3D11_RENDERER_MESH_H

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_DIRECT3D11)

#include <RendererMesh.h>
#include "D3D11Renderer.h"

namespace SampleRenderer
{
class D3D11RendererMaterial;

class D3D11RendererMesh : public RendererMesh
{
	friend class D3D11Renderer;
	
public:
	D3D11RendererMesh(D3D11Renderer& renderer, const RendererMeshDesc& desc);
	virtual ~D3D11RendererMesh(void);

protected:
	virtual void renderIndices(PxU32 numVertices, PxU32 firstIndex, PxU32 numIndices, RendererIndexBuffer::Format indexFormat, RendererMaterial* material) const;
	virtual void renderVertices(PxU32 numVertices, RendererMaterial* material) const;

	virtual void renderIndicesInstanced(PxU32 numVertices, PxU32 firstIndex, PxU32 numIndices, RendererIndexBuffer::Format indexFormat, RendererMaterial* material) const;
	virtual void renderVerticesInstanced(PxU32 numVertices, RendererMaterial* material) const;

	D3D11RendererMesh& operator=(const D3D11RendererMesh&) { return *this; }

	Renderer& renderer() { return m_renderer; }

private:
	void         bind(void) const;
	void         render(RendererMaterial* material) const;

	void setTopology(const Primitive& primitive, bool bTessellationEnabled) const;
	void setLayout(const RendererMaterial*, bool bInstanced) const;
	void setSprites(bool bEnabled) const;
	void setNumVerticesAndIndices(PxU32 numIndices, PxU32 numVertices);

	ID3D11InputLayout* getInputLayoutForMaterial(const D3D11RendererMaterial*, bool bInstanced) const;

private:
	class ScopedMeshRender;
	friend class ScopedMeshRender;

	typedef std::vector<D3D11_INPUT_ELEMENT_DESC> LayoutVector;

	D3D11Renderer&           m_renderer;

	LayoutVector             m_inputDescriptions;
	LayoutVector             m_instancedInputDescriptions;
	PxU64                    m_inputHash;
	PxU64                    m_instancedInputHash;

	// This is merely for cleanup, and has no effect on externally visible state
	mutable bool             m_bPopStates;

	std::string              m_spriteShaderPath;
};

} // namespace SampleRenderer

#endif // #if defined(RENDERER_ENABLE_DIRECT3D11)
#endif
