//=========================================================================
//	Module: r3dMeshTexDensityVisualizer.cpp
//	Copyright (C) 2012.
//=========================================================================

#include	"r3dPCH.h"
#include	"r3d.h"

#ifndef FINAL_BUILD

//////////////////////////////////////////////////////////////////////////

#include "r3dMeshTexDensityVisualizer.h"

//////////////////////////////////////////////////////////////////////////

/**	Minimum, optimal and maximum color and density. */
D3DXVECTOR4 gMinTexDensityParams(0, 0, 1.0f, 40.0f);
D3DXVECTOR4 gOptTexDensityParams(0, 1.0f, 0, 70.0f);
D3DXVECTOR4 gMaxTexDensityParams(1.0f, 0, 0, 120.0f);

//////////////////////////////////////////////////////////////////////////

class ModifiedVDecl
{
	IDirect3DVertexDeclaration9 *pDecl;

public:
	D3DVERTEXELEMENT9 VDecl[MAXD3DDECLLENGTH];
	uint32_t refCount;
	ModifiedVDecl()
	: pDecl(0)
	{

	}

	IDirect3DVertexDeclaration9 * GetDecl()
	{
		R3D_ENSURE_MAIN_THREAD();
		if(pDecl == 0)
		{
			r3dDeviceTunnel::CreateVertexDeclaration(VDecl, &pDecl);
		}
		return pDecl;
	}

	bool operator == (D3DVERTEXELEMENT9 *toFind)
	{
		if (!toFind || !VDecl)
			return false;

		if (D3DXGetDeclLength(toFind) != D3DXGetDeclLength(VDecl))
			return false;

		if (D3DXGetDeclVertexSize(toFind, 0) != D3DXGetDeclVertexSize(VDecl, 0))
			return false;

		for (UINT i = 0; i < D3DXGetDeclLength(toFind); ++i)
		{
			if
			(
				toFind[i].Method != VDecl[i].Method ||
				toFind[i].Offset != VDecl[i].Offset ||
				toFind[i].Stream != VDecl[i].Stream ||
				toFind[i].Type != VDecl[i].Type ||
				toFind[i].Usage != VDecl[i].Usage ||
				toFind[i].UsageIndex != VDecl[i].UsageIndex
			)
			return false;
		}
		return true;
	}
};

r3dTL::TArray<ModifiedVDecl> gModifiedVDecls;

const uint32_t MODIFIED_VDECL_INVALID_INDEX = 0xffffffff;
const uint32_t TEX_DENSITY_DATA_STREAM_INDEX = 1;

//////////////////////////////////////////////////////////////////////////

template <typename T>
float CalcArea(const T &v0, const T &v1, const T &v2)
{
	float a = (v0 - v1).Length();
	float b = (v1 - v2).Length();
	float c = (v2 - v0).Length();

	float p = (a + b + c) * 0.5f;

	float area = sqrtf(p * (p - a) * (p - b) * (p - c));
	return area;
};

//////////////////////////////////////////////////////////////////////////

MeshTextureDensityVisualizer::MeshTextureDensityVisualizer(r3dMesh *m)
: mesh(m)
, modifiedVDeclIdx(MODIFIED_VDECL_INVALID_INDEX)
{

}

//////////////////////////////////////////////////////////////////////////

MeshTextureDensityVisualizer::~MeshTextureDensityVisualizer()
{
}

//////////////////////////////////////////////////////////////////////////

void MeshTextureDensityVisualizer::CalculateTextureDensity()
{
	if (!mesh || !mesh->VertexPositions)
		return;

	void *vb = 0;
	void *ib = 0;
	mesh->buffers.Lock(vb, ib);

	if (!vb)
		return;

	char *vbData = reinterpret_cast<char*>(vb);
	uint32_t vertexStride = D3DXGetDeclVertexSize(R3D_MESH_VERTEX::VBDecl, 0);

	if (mesh->VertexFlags & r3dMesh::vfPrecise )
		vertexStride = D3DXGetDeclVertexSize(R3D_MESH_PRECISE_VERTEX::VBDecl, 0);
	else if ( mesh->VertexFlags & r3dMesh::vfBending )
		vertexStride = D3DXGetDeclVertexSize(R3D_BENDING_MESH_VERTEX::VBDecl, 0);
	else if (mesh->IsSkeletal())
		vertexStride = D3DXGetDeclVertexSize(r3dSkinnedMeshVertex::VBDecl, 0);

	uint32_t bufSize = sizeof(float) * mesh->NumVertices;
	r3dDeviceTunnel::CreateVertexBuffer(bufSize, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &texelDensityData);
	float* dstPtr = 0;
	texelDensityData.Lock(0, 0, (void**)&dstPtr, 0) ;

	::ZeroMemory(dstPtr, bufSize);

	for (int k = 0; k < mesh->NumMatChunks; ++k)
	{
		const r3dTriBatch& batch = mesh->MatChunks[k];
		if (!batch.Mat || !batch.Mat->Texture)
			continue;

		for (uint32_t i = batch.StartIndex; i != batch.EndIndex; i += 3)
		{
			uint32_t i0 = mesh->Indices[i + 0];
			uint32_t i1 = mesh->Indices[i + 1];
			uint32_t i2 = mesh->Indices[i + 2];

			const r3dVector &v0 = mesh->VertexPositions[i0];
			const r3dVector &v1 = mesh->VertexPositions[i1];
			const r3dVector &v2 = mesh->VertexPositions[i2];

			r3dPoint2D uv0, uv1, uv2;

			if( mesh->VertexFlags & r3dMesh::vfPrecise )
			{
				R3D_MESH_PRECISE_VERTEX *vtx0 = reinterpret_cast<R3D_MESH_PRECISE_VERTEX *>(vbData + vertexStride * i0);
				R3D_MESH_PRECISE_VERTEX *vtx1 = reinterpret_cast<R3D_MESH_PRECISE_VERTEX *>(vbData + vertexStride * i1);
				R3D_MESH_PRECISE_VERTEX *vtx2 = reinterpret_cast<R3D_MESH_PRECISE_VERTEX *>(vbData + vertexStride * i2);
				uv0 = r3dPoint2D(vtx0->texcoord.x, vtx0->texcoord.y);
				uv1 = r3dPoint2D(vtx1->texcoord.x, vtx1->texcoord.y);
				uv2 = r3dPoint2D(vtx2->texcoord.x, vtx2->texcoord.y);
			}
			else
			if( mesh->VertexFlags & r3dMesh::vfBending )
			{
				R3D_BENDING_MESH_VERTEX *vtx0 = reinterpret_cast<R3D_BENDING_MESH_VERTEX *>(vbData + vertexStride * i0);
				R3D_BENDING_MESH_VERTEX *vtx1 = reinterpret_cast<R3D_BENDING_MESH_VERTEX *>(vbData + vertexStride * i1);
				R3D_BENDING_MESH_VERTEX *vtx2 = reinterpret_cast<R3D_BENDING_MESH_VERTEX *>(vbData + vertexStride * i2);
				uv0 = r3dPoint2D(vtx0->texcoord.x, vtx0->texcoord.y);
				uv1 = r3dPoint2D(vtx1->texcoord.x, vtx1->texcoord.y);
				uv2 = r3dPoint2D(vtx2->texcoord.x, vtx2->texcoord.y);
			}
			else
			{
				R3D_MESH_VERTEX *vtx0 = reinterpret_cast<R3D_MESH_VERTEX *>(vbData + vertexStride * i0);
				R3D_MESH_VERTEX *vtx1 = reinterpret_cast<R3D_MESH_VERTEX *>(vbData + vertexStride * i1);
				R3D_MESH_VERTEX *vtx2 = reinterpret_cast<R3D_MESH_VERTEX *>(vbData + vertexStride * i2);
				uv0 = UnpackTexcoord(vtx0->texcoord.x, vtx0->texcoord.y, mesh->texcUnpackScale);
				uv1 = UnpackTexcoord(vtx1->texcoord.x, vtx1->texcoord.y, mesh->texcUnpackScale);
				uv2 = UnpackTexcoord(vtx2->texcoord.x, vtx2->texcoord.y, mesh->texcUnpackScale);
			}
			float posArea = CalcArea(v0, v1, v2);
			float uvArea = CalcArea(uv0, uv1, uv2);

			int pixelArea = batch.Mat->Texture->GetWidth() * batch.Mat->Texture->GetHeight();

			float texelDensity = uvArea / posArea * pixelArea;
			texelDensity = sqrt(texelDensity);

			dstPtr[i0] = std::max(dstPtr[i0], texelDensity);
			dstPtr[i1] = std::max(dstPtr[i1], texelDensity);
			dstPtr[i2] = std::max(dstPtr[i2], texelDensity);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void MeshTextureDensityVisualizer::CreateNewVertexDeclarationForMesh()
{
	if (!mesh)
		return;

	const D3DVERTEXELEMENT9 *elems = 0;
	if (mesh->IsSkeletal())
	{
		elems = r3dSkinnedMeshVertex::VBDecl;
	}
	else if (mesh->VertexFlags & r3dMesh::vfPrecise )
	{
		elems = R3D_MESH_PRECISE_VERTEX::VBDecl;
	}
	else if (mesh->VertexFlags & r3dMesh::vfBending )
	{
		elems = R3D_BENDING_MESH_VERTEX::VBDecl;
	}
	else
	{
		elems = R3D_MESH_VERTEX::VBDecl;
	}

	static const D3DVERTEXELEMENT9 newElem = { TEX_DENSITY_DATA_STREAM_INDEX, 0, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3 };
	ModifiedVDecl newMVD;
	UINT origDeclLen = D3DXGetDeclLength(elems) + 1;
	D3DVERTEXELEMENT9 *newElems = newMVD.VDecl;
	for (uint32_t i = 0; i < origDeclLen; ++i)
	{
		newElems[i] = elems[i];
	}
	std::swap(newElems[origDeclLen], newElems[origDeclLen - 1]);
	newElems[origDeclLen - 1] = newElem;

	for (uint32_t i = 0; i < gModifiedVDecls.Count(); ++i)
	{
		ModifiedVDecl &mvd = gModifiedVDecls[i];
		if (mvd == newElems)
		{
			modifiedVDeclIdx = i;
			return;
		}
	}

	// Not found, create new one
	gModifiedVDecls.PushBack(newMVD);
	modifiedVDeclIdx = gModifiedVDecls.Count() - 1;
}

//////////////////////////////////////////////////////////////////////////

void MeshTextureDensityVisualizer::SetTexelDensityStreamAndVDecl()
{
	if (!mesh || !d_visualize_tex_density->GetBool())
		return;

	if (!texelDensityData.Get())
	{
		//	Create and fill texture density data
		CalculateTextureDensity();
	}

	if (modifiedVDeclIdx == MODIFIED_VDECL_INVALID_INDEX)
	{
		CreateNewVertexDeclarationForMesh();
	}
	r3d_assert(modifiedVDeclIdx < gModifiedVDecls.Count());

	ModifiedVDecl &mvd = gModifiedVDecls[modifiedVDeclIdx];

	r3d_assert(mvd.GetDecl());

	d3dc._SetDecl(mvd.GetDecl());
	d3dc._SetStreamSource(TEX_DENSITY_DATA_STREAM_INDEX, texelDensityData.Get(), 0, sizeof(float));
}

//////////////////////////////////////////////////////////////////////////

void MeshTextureDensityVisualizer::SetShaderConstants()
{
	if (!d_visualize_tex_density->GetBool())
		return;

	r3dRenderer->pd3ddev->SetPixelShaderConstantF(MC_MATERIAL_PARAMS, &gMinTexDensityParams.x, 1);
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(MC_MAT_DIFFUSE, &gOptTexDensityParams.x, 1);
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(MC_MAT_SPECULAR, &gMaxTexDensityParams.x, 1);
}

//////////////////////////////////////////////////////////////////////////

void MeshTextureDensityVisualizer::SetShaders()
{
	if (!d_visualize_tex_density->GetBool())
		return;

	r3dRenderer->SetPixelShader(r3dRenderer->GetPixelShaderIdx("PS_FILLGBUFFER_TEXEL_DENSITY"));
	r3dRenderer->SetVertexShader(r3dRenderer->GetVertexShaderIdx("VS_FILLGBUFFER_TEXEL_DENSITY"));
}

//////////////////////////////////////////////////////////////////////////

#endif // FINAL_BUILD