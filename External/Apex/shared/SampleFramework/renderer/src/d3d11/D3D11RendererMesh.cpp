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

#include <RendererConfig.h>

#if defined(RENDERER_ENABLE_DIRECT3D11)

#include "D3D11RendererMesh.h"
#include "D3D11RendererVertexBuffer.h"
#include "D3D11RendererInstanceBuffer.h"
#include "D3D11RendererMemoryMacros.h"
#include "D3D11RendererMaterial.h"
#include "D3D11RendererUtils.h"
#include "D3D11RendererResourceManager.h"
#include "D3D11RendererVariableManager.h"

#include <RendererMeshDesc.h>

#include <SamplePlatform.h>

#pragma warning(disable:4702 4189)

using namespace SampleRenderer;

D3D11RendererMesh::D3D11RendererMesh(D3D11Renderer& renderer, const RendererMeshDesc& desc) :
	RendererMesh(desc),
	m_renderer(renderer),
	m_inputHash(0),
	m_instancedInputHash(0),
	m_bPopStates(false),
	m_spriteShaderPath(std::string(m_renderer.getAssetDir())+"shaders/vertex/pointsprite.cg")
{
	ID3D11Device* d3dDevice = m_renderer.getD3DDevice();
	RENDERER_ASSERT(d3dDevice, "Renderer's D3D Device not found!");
	if (d3dDevice)
	{
		PxU32                             numVertexBuffers = getNumVertexBuffers();
		const RendererVertexBuffer* const* vertexBuffers   = getVertexBuffers();

		for (PxU32 i = 0; i < numVertexBuffers; i++)
		{
			const RendererVertexBuffer* vb = vertexBuffers[i];
			if (vb)
			{
				const D3D11RendererVertexBuffer& d3dVb = *static_cast<const D3D11RendererVertexBuffer*>(vb);
				d3dVb.addVertexElements(i, m_inputDescriptions);
			}
		}
		m_inputHash = D3DX11::getInputHash(m_inputDescriptions);

		m_instancedInputDescriptions = m_inputDescriptions;
		if (m_instanceBuffer)
		{
			static_cast<const D3D11RendererInstanceBuffer*>(m_instanceBuffer)->addVertexElements(numVertexBuffers, m_instancedInputDescriptions);
			m_instancedInputHash = D3DX11::getInputHash(m_instancedInputDescriptions);
		}
		else
		{
			m_instancedInputHash = m_inputHash;
		}
	}
}

D3D11RendererMesh::~D3D11RendererMesh(void)
{

}

static D3D_PRIMITIVE_TOPOLOGY getD3DPrimitive(const RendererMesh::Primitive& primitive, bool bTessellationEnabled)
{
	D3D_PRIMITIVE_TOPOLOGY d3dPrimitive = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	switch (primitive)
	{
	case RendererMesh::PRIMITIVE_POINTS:
		d3dPrimitive = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		break;
	case RendererMesh::PRIMITIVE_LINES:
		d3dPrimitive = D3D_PRIMITIVE_TOPOLOGY_LINELIST;
		break;
	case RendererMesh::PRIMITIVE_LINE_STRIP:
		d3dPrimitive = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;
		break;
	case RendererMesh::PRIMITIVE_TRIANGLES:
		d3dPrimitive = bTessellationEnabled ? D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST : D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	case RendererMesh::PRIMITIVE_TRIANGLE_STRIP:
		d3dPrimitive = bTessellationEnabled ? D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST : D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		break;
	case RendererMesh::PRIMITIVE_POINT_SPRITES:
		d3dPrimitive = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
		break;
	}
	RENDERER_ASSERT(d3dPrimitive != D3D_PRIMITIVE_TOPOLOGY_UNDEFINED, "Unable to find DIRECT3D11 Primitive.");
	return d3dPrimitive;
}

class D3D11RendererMesh::ScopedMeshRender {
public:

	ScopedMeshRender(const D3D11RendererMesh& mesh_, RendererMaterial* material, bool instanced) : mesh(mesh_) 
	{ 
		mesh.setTopology(mesh.getPrimitives(), static_cast<D3D11RendererMaterial*>(material)->tessellationEnabled());
		mesh.setLayout(material, instanced);
		if (mesh.getPrimitives() == RendererMesh::PRIMITIVE_POINT_SPRITES) 
			mesh.setSprites(true);
	}

	~ScopedMeshRender() 
	{
		if (mesh.getPrimitives() == RendererMesh::PRIMITIVE_POINT_SPRITES) 
			mesh.setSprites(false);
	}

	ScopedMeshRender& operator=(const ScopedMeshRender&) { return *this; }

	const D3D11RendererMesh& mesh;
};

void D3D11RendererMesh::renderIndices(PxU32 numVertices, PxU32 firstIndex, PxU32 numIndices, RendererIndexBuffer::Format indexFormat, RendererMaterial* material) const
{
	ID3D11DeviceContext* d3dDeviceContext = m_renderer.getD3DDeviceContext();
	RENDERER_ASSERT(material, "Invalid RendererMaterial");
	if (d3dDeviceContext && material)
	{
		ScopedMeshRender renderScope(*this, material, false);
		d3dDeviceContext->DrawIndexed(numIndices, firstIndex, 0);
	}
}

void D3D11RendererMesh::renderVertices(PxU32 numVertices, RendererMaterial* material) const
{
	ID3D11DeviceContext* d3dDeviceContext = m_renderer.getD3DDeviceContext();
	RENDERER_ASSERT(material, "Invalid RendererMaterial");
	if (d3dDeviceContext && material)
	{
		ScopedMeshRender renderScope(*this, material, false);
		d3dDeviceContext->Draw(numVertices, 0);
	}
}

void D3D11RendererMesh::renderIndicesInstanced(PxU32 numVertices, PxU32 firstIndex, PxU32 numIndices, RendererIndexBuffer::Format indexFormat, RendererMaterial* material) const
{
	ID3D11DeviceContext* d3dDeviceContext = m_renderer.getD3DDeviceContext();
	RENDERER_ASSERT(material, "Invalid RendererMaterial");
	if (d3dDeviceContext && material)
	{
		ScopedMeshRender renderScope(*this, material, true);
		d3dDeviceContext->DrawIndexedInstanced(numIndices, m_numInstances, firstIndex, 0, m_firstInstance);
	}
}

void D3D11RendererMesh::renderVerticesInstanced(PxU32 numVertices, RendererMaterial* material) const
{
	ID3D11DeviceContext* d3dDeviceContext = m_renderer.getD3DDeviceContext();
	RENDERER_ASSERT(material, "Invalid RendererMaterial");
	if (d3dDeviceContext && material)
	{
		ScopedMeshRender renderScope(*this, material, true);
		d3dDeviceContext->DrawInstanced(numVertices, m_numInstances, 0, m_firstInstance);
	}
}

void D3D11RendererMesh::bind(void) const
{
	RendererMesh::bind();
}

void D3D11RendererMesh::render(RendererMaterial* material) const
{
	RendererMesh::render(material);
}

void D3D11RendererMesh::setTopology(const Primitive& primitive, bool bTessellationEnabled) const
{
	ID3D11DeviceContext* d3dContext = m_renderer.getD3DDeviceContext();
	RENDERER_ASSERT(d3dContext, "Invalid D3D11 context");
	d3dContext->IASetPrimitiveTopology(getD3DPrimitive(primitive, bTessellationEnabled));
}

ID3D11InputLayout* D3D11RendererMesh::getInputLayoutForMaterial(const D3D11RendererMaterial* pMaterial, bool bInstanced) const
{
	ID3D11InputLayout* pLayout = NULL;

	RENDERER_ASSERT(pMaterial, "Invalid D3D11 Material");
	const LayoutVector& inputDescriptions = bInstanced ? m_instancedInputDescriptions : m_inputDescriptions;
	const PxU64&        inputHash         = bInstanced ? m_instancedInputHash : m_inputHash;
	ID3DBlob* pVSBlob = pMaterial->getVSBlob(inputDescriptions, inputHash, bInstanced);
	RENDERER_ASSERT(pVSBlob, "Invalid D3D11 Shader Blob");

	D3DTraits<ID3D11InputLayout>::key_type ilKey(inputHash, pVSBlob);
	pLayout = m_renderer.getResourceManager()->hasResource<ID3D11InputLayout>(ilKey);
	if (!pLayout)
	{
		ID3D11Device* d3dDevice = m_renderer.getD3DDevice();
		RENDERER_ASSERT(d3dDevice, "Invalid D3D11 device");
		HRESULT result = d3dDevice->CreateInputLayout(&inputDescriptions[0],
		                 (UINT)inputDescriptions.size(),
		                 pVSBlob->GetBufferPointer(),
		                 pVSBlob->GetBufferSize(),
		                 &pLayout);
		RENDERER_ASSERT(SUCCEEDED(result) && pLayout, "Failed to create DIRECT3D11 Input Layout.");
		if (SUCCEEDED(result) && pLayout)
		{
			m_renderer.getResourceManager()->registerResource<ID3D11InputLayout>(ilKey, pLayout);
		}
	}

	RENDERER_ASSERT(pLayout, "Failed to find DIRECT3D11 Input Layout.");
	return pLayout;
}

void D3D11RendererMesh::setLayout(const RendererMaterial* pMaterial, bool bInstanced) const
{
	ID3D11DeviceContext* d3dContext = m_renderer.getD3DDeviceContext();
	RENDERER_ASSERT(d3dContext, "Invalid D3D11 context");
	ID3D11InputLayout* pLayout = getInputLayoutForMaterial(static_cast<const D3D11RendererMaterial*>(pMaterial), bInstanced);
	d3dContext->IASetInputLayout(pLayout);
}

void D3D11RendererMesh::setSprites(bool bEnabled) const
{
	if (bEnabled)
	{
		static const D3D_SHADER_MACRO geometryDefines[] =
		{
			"RENDERER_GEOMETRY", "1",
			"RENDERER_D3D11",    "1",
			"PX_WINDOWS",        "1",
			"USE_ALL",           "1",
			"SEMANTIC_TANGENT",  "TANGENT",
			NULL,                NULL
		};

		ID3D11GeometryShader* pShader = NULL;
		ID3DBlob*               pBlob = NULL;
		bool         bLoadedFromCache = false;

		D3D11ShaderLoader gsLoader(m_renderer);
		if (SUCCEEDED(gsLoader.load("pointsprite", m_spriteShaderPath.c_str(), geometryDefines, &pShader, &pBlob, true, &bLoadedFromCache)))
		{
			// If the shader was just compiled we need to load the proper variables for it
			if (!bLoadedFromCache) 
			{
				m_renderer.getVariableManager()->loadSharedVariables(this, pBlob, D3DTypes::SHADER_GEOMETRY);
			}
			m_renderer.getVariableManager()->bind(this, D3DTypes::SHADER_GEOMETRY);
			m_renderer.getD3DDeviceContext()->GSSetShader(pShader, NULL, 0);
		}
	}
	else
	{
		m_renderer.getD3DDeviceContext()->GSSetShader(NULL, NULL, 0);
	}
}

void D3D11RendererMesh::setNumVerticesAndIndices(PxU32 nbVerts, PxU32 nbIndices)
{
	m_numVertices = nbVerts;
	m_numIndices =  nbIndices;
}


#endif // #if defined(RENDERER_ENABLE_DIRECT3D11)
