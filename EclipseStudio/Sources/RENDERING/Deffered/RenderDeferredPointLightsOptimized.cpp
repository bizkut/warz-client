//=========================================================================
//	Module: RenderDeferredPointLightsOptimized.cpp
//	Copyright (C) Arktos 2011.
//=========================================================================

#include "r3dpch.h"
#include "r3d.h"
#include "RenderDeferredPointLightsOptimized.h"
#include "GameCommon.h"
#include "GameLevel.h"
#include "r3dBudgeter.h"
#include "../../ObjectsCode/WORLD/EnvmapProbes.h"

//#define LIGHTS_RENDERER_V2_ENABLED 1

//////////////////////////////////////////////////////////////////////////

extern int PS_POINTLIGHT_V2_ID;
extern int VS_POINTLIGHT_V2_ID;

extern r3dScreenBuffer *gBuffer_Color;
extern r3dScreenBuffer *gBuffer_Normal;
extern r3dScreenBuffer *gBuffer_Depth;
extern r3dScreenBuffer *gBuffer_Aux;
extern EnvmapProbes g_EnvmapProbes;

//////////////////////////////////////////////////////////////////////////

namespace
{
const int LIGHT_TILE_TEXTURE_SIZE	= 64;
const int LIGHT_TILE_TEXTURE_DEPTH	= 32;
const int LIGHT_PARAMS_TEXTURE_SIZE = 64;

/** Tile info structure. */
struct TileInfo
{
	uint16_t lightIDs[LIGHT_TILE_TEXTURE_SIZE][LIGHT_TILE_TEXTURE_SIZE];
};

//////////////////////////////////////////////////////////////////////////

struct TileGridVertex
{
	r3dPoint2D pos;
	TileGridVertex(): pos(0, 0) {}
};

const D3DVERTEXELEMENT9 vertexElemetsDef[] = 
{
	{0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{1, 0, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END()
};


//////////////////////////////////////////////////////////////////////////

/** Optimized point lights renderer. */
class PointLightsRenderer
{
	float zMinCache[LIGHT_TILE_TEXTURE_SIZE][LIGHT_TILE_TEXTURE_SIZE];
	TileInfo tileCache[LIGHT_TILE_TEXTURE_DEPTH];

	r3dTexture *tileCacheVolumeTex;
	r3dVertexBuffer *tileGridVB;
	r3dVertexBuffer *tileInstanceData;
	r3dIndexBuffer *tileGridIB;
	IDirect3DVertexDeclaration9 *tileGridVD;
	
	r3dTexture *lightParamsTex;
	float *lightParamsData;

	void FillTileCacheVolumeTexture();
	void ResetTileCache();
	void CreateGeometry();
	void UpdateInstanceData();
	void UpdateLightParametersTexture();

public:
	PointLightsRenderer();
	void Init();
	void Destroy();
	void RasterizeLightToTileCache(uint32_t lightID, const D3DXMATRIX &viewMat, const D3DXMATRIX &projMat);
	void RenderLights();
};

//------------------------------------------------------------------------
// class PointLightsRenderer
//------------------------------------------------------------------------

PointLightsRenderer::PointLightsRenderer()
: tileCacheVolumeTex(0)
, tileGridVB(0)
, tileGridIB(0)
, tileGridVD(0)
, lightParamsData(0)
, lightParamsTex(0)
{

}

//////////////////////////////////////////////////////////////////////////

void PointLightsRenderer::ResetTileCache()
{
	R3DPROFILE_FUNCTION("ResetTileCache");
	DWORD len = sizeof(tileCache);
	::ZeroMemory(&tileCache[0], len);
	float f = 1.0f;
	for (int i = 0; i < LIGHT_TILE_TEXTURE_SIZE; ++i)
	{
		for (int j = 0; j < LIGHT_TILE_TEXTURE_SIZE; ++j)
		{
			zMinCache[i][j] = f;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void PointLightsRenderer::Init()
{

	tileCacheVolumeTex = r3dRenderer->AllocateTexture();
	lightParamsTex = r3dRenderer->AllocateTexture();

	tileCacheVolumeTex->CreateVolume(LIGHT_TILE_TEXTURE_SIZE, LIGHT_TILE_TEXTURE_SIZE, LIGHT_TILE_TEXTURE_DEPTH, D3DFMT_L16, 1, D3DPOOL_DEFAULT);
	lightParamsTex->Create(LIGHT_PARAMS_TEXTURE_SIZE, LIGHT_PARAMS_TEXTURE_SIZE, D3DFMT_A32B32G32R32F, 1, D3DPOOL_DEFAULT);

	//	WARNING! Don't forget change size of this array if you change lightParamsTex texture format
	lightParamsData = gfx_new float[lightParamsTex->GetWidth() * lightParamsTex->GetHeight() * 4];

	const int vbSize = LIGHT_TILE_TEXTURE_SIZE * LIGHT_TILE_TEXTURE_SIZE * 4;
	tileGridVB = gfx_new r3dVertexBuffer(vbSize, sizeof(TileGridVertex), 0, false, false, BufferMem);

	TileGridVertex *vertexStream = reinterpret_cast<TileGridVertex*>(tileGridVB->Lock(0, 0, 0));
	r3d_assert(vertexStream);
	if (!vertexStream) return;

	int x = 0;
	int y = 0;
	float w = static_cast<float>(LIGHT_TILE_TEXTURE_SIZE);
	float tileCellIdx = 0;
	for (int i = 0; i < vbSize; i += 4, vertexStream += 4, ++tileCellIdx)
	{
		vertexStream[0].pos = r3dPoint2D(x / w * 2 - 1, -(y / w * 2 - 1));
		vertexStream[1].pos = r3dPoint2D((x + 1) / w * 2 - 1, -(y / w * 2 - 1));
		vertexStream[2].pos = r3dPoint2D(x / w * 2 - 1, -((y + 1) / w * 2 - 1));
		vertexStream[3].pos = r3dPoint2D((x + 1) / w * 2 - 1, -((y + 1) / w * 2 - 1));

		y += x / (LIGHT_TILE_TEXTURE_SIZE - 1);
		x = (x + 1) % LIGHT_TILE_TEXTURE_SIZE;
	}
	tileGridVB->Unlock();

	const int ibSize = LIGHT_TILE_TEXTURE_SIZE * LIGHT_TILE_TEXTURE_SIZE * 6;
	tileGridIB = gfx_new r3dIndexBuffer(ibSize, false, 2, BufferMem);

	uint16_t *indexStream = reinterpret_cast<uint16_t*>(tileGridIB->Lock());
	r3d_assert(indexStream);
	if (!indexStream) return;

	uint16_t offset = 0;
	for (int i = 0; i < ibSize; i += 6, indexStream += 6, offset += 4)
	{
		indexStream[0] = 0 + offset;
		indexStream[1] = 1 + offset;
		indexStream[2] = 2 + offset;
		indexStream[3] = 1 + offset;
		indexStream[4] = 3 + offset;
		indexStream[5] = 2 + offset;
	}

	tileGridIB->Unlock();

	//	Instances buffer with tile z values
	const int instVBSize = LIGHT_TILE_TEXTURE_SIZE * LIGHT_TILE_TEXTURE_SIZE * 4;
	tileInstanceData = gfx_new r3dVertexBuffer(instVBSize, sizeof(float), 0, true, false, BufferMem);

	(r3dDeviceTunnel::CreateVertexDeclaration(vertexElemetsDef, &tileGridVD) );
}

//////////////////////////////////////////////////////////////////////////

void PointLightsRenderer::Destroy()
{
	SAFE_DELETE(tileGridIB)
	SAFE_DELETE(tileGridVB);
	SAFE_DELETE(tileInstanceData);
	r3dRenderer->DeleteTexture(tileCacheVolumeTex);
	SAFE_DELETE_ARRAY(lightParamsData);
	r3dRenderer->DeleteTexture(lightParamsTex);
}

//////////////////////////////////////////////////////////////////////////

void PointLightsRenderer::RenderLights()
{
	R3DPROFILE_FUNCTION("DR: Pointlights Opt");
	D3DPERF_BeginEvent(0x0, L"PointLights Opt");
	WorldLightSystem.FillVisibleArray();
	WorldLightSystem.SortVisibleLightsByDistance();

	IDirect3DDevice9 *d = r3dRenderer->pd3ddev;
	if (WorldLightSystem.nVisibleLights == 0) return;

	ResetTileCache();

	R3DPROFILE_START("RasterizeLightsToTileCache");
	for (int i = 0, i_end = WorldLightSystem.nVisibleLights; i != i_end; ++i)
	{
		r3dLight *l = WorldLightSystem.VisibleLights[i];

		if (!l || l->Type != R3D_OMNI_LIGHT) continue;

		l->ProcessLightFunction(r3dGetFrameTime());

		if(!l->IsOn()) continue;

		l->RecalcBoundBox();
		if (!r3dRenderer->IsBoxInsideFrustum(l->BBox)) continue;

		float VIS_RANGE = RenderDistance + l->GetOuterRadius();
		r3dVector V = gCam - *l;
		if (V.Length() > VIS_RANGE) continue;

		RasterizeLightToTileCache(l->id, r3dRenderer->ViewMatrix, r3dRenderer->ProjMatrix);
	}
	R3DPROFILE_END("RasterizeLightsToTileCache");

	UpdateInstanceData();
	UpdateLightParametersTexture();
	FillTileCacheVolumeTexture();

	R3DPROFILE_START("RenderLights");
	tileGridVB->Set(0);
	tileInstanceData->Set(1);
	tileGridIB->Set();
	d3dc._SetDecl(tileGridVD);
	

	DWORD prevSampMinF0 = 0;
	DWORD prevSampMagF0 = 0;
	DWORD prevSampMinF1 = 0;
	DWORD prevSampMagF1 = 0;
	d->GetSamplerState(0, D3DSAMP_MINFILTER, &prevSampMinF0);
	d->GetSamplerState(0, D3DSAMP_MAGFILTER, &prevSampMagF0);
	d->GetSamplerState(1, D3DSAMP_MINFILTER, &prevSampMinF1);
	d->GetSamplerState(1, D3DSAMP_MAGFILTER, &prevSampMagF1);

	d->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	d->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	d->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	d->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	r3dRenderer->SetTex(tileCacheVolumeTex, 0);
	r3dRenderer->SetTex(lightParamsTex, 1);

	r3dRenderer->SetTex(gBuffer_Color->Tex, 2);
	r3dRenderer->SetTex(gBuffer_Normal->Tex, 3);
	r3dRenderer->SetTex(gBuffer_Depth->Tex, 4);
	r3dRenderer->SetTex(gBuffer_Aux->Tex, 5);

	d->SetRenderState(D3DRS_ZENABLE, TRUE);
	d->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	r3dRenderer->SetVertexShader(VS_POINTLIGHT_V2_ID);
	r3dRenderer->SetPixelShader(PS_POINTLIGHT_V2_ID);

	D3DXMATRIXA16 m[2] =
	{
		D3DXMATRIXA16(r3dRenderer->ViewMatrix * r3dRenderer->ProjMatrix),
		D3DXMATRIXA16(r3dRenderer->ViewMatrix * r3dRenderer->ProjMatrix)
	};
	D3DXMatrixInverse(&m[0], 0, &m[0]);

	D3DXVECTOR4 consts[3] =
	{
		D3DXVECTOR4(0.5f / r3dRenderer->ScreenW, 0.5f / r3dRenderer->ScreenH, 0, 0),
		D3DXVECTOR4(0, 0, 0, 0),
		D3DXVECTOR4(1, 1, 0, 0)
	};
	d->SetVertexShaderConstantF(0, &consts[0].x, 1);
	d->SetVertexShaderConstantF(1, &m[0]._11, 8);

	consts[0].x = 1.0f / LIGHT_TILE_TEXTURE_DEPTH;
	consts[0].y = 1.0f / LIGHT_PARAMS_TEXTURE_SIZE;
	consts[0].z = static_cast<float>(LIGHT_PARAMS_TEXTURE_SIZE);
	consts[0].w = 1.0f / (r3dRenderer->FarClip * 0.9375f);

	consts[1] = D3DXVECTOR4(gCam.x, gCam.y, gCam.z, 0);

	d->SetPixelShaderConstantF(0, &consts[0].x, _countof(consts));

#if 0
	SetSSSParams(4, false);
#endif

	r3dRenderer->SetRenderingMode(R3D_BLEND_ADD | R3D_BLEND_NZ | R3D_BLEND_PUSH);

	r3dRenderer->DrawIndexed(D3DPT_TRIANGLELIST, 0, 0, tileGridVB->GetItemCount(), 0, tileGridIB->GetItemCount() / 3);

	r3dRenderer->SetRenderingMode(R3D_BLEND_POP);

	d->SetSamplerState(0, D3DSAMP_MINFILTER, prevSampMinF0);
	d->SetSamplerState(0, D3DSAMP_MAGFILTER, prevSampMagF0);
	d->SetSamplerState(1, D3DSAMP_MINFILTER, prevSampMinF1);
	d->SetSamplerState(1, D3DSAMP_MAGFILTER, prevSampMagF1);
	R3DPROFILE_END("RenderLights");

	D3DPERF_EndEvent();
}

//////////////////////////////////////////////////////////////////////////

void PointLightsRenderer::RasterizeLightToTileCache
(
	uint32_t lightID,
	const D3DXMATRIX &viewMat,
	const D3DXMATRIX &projMat
)
{
	D3DXMATRIX viewProjMat = viewMat * projMat;
	r3d_assert(lightID < WorldLightSystem.Lights.Count());
	r3dLight *l = WorldLightSystem.Lights[lightID];
	if (!l) return;

	float r = l->GetOuterRadius();
	D3DXVECTOR3 lightPos(l->x, l->y, l->z);
	D3DXVec3TransformCoord(&lightPos, &lightPos, &viewMat);

	D3DXVECTOR3 bb[9] = 
	{
		D3DXVECTOR3(lightPos.x - r, lightPos.y - r, R3D_MAX(lightPos.z + r / 2, gCam.NearClip)),
		D3DXVECTOR3(lightPos.x - r, lightPos.y + r, R3D_MAX(lightPos.z + r / 2, gCam.NearClip)),
		D3DXVECTOR3(lightPos.x + r, lightPos.y - r, R3D_MAX(lightPos.z + r / 2, gCam.NearClip)),
		D3DXVECTOR3(lightPos.x + r, lightPos.y + r, R3D_MAX(lightPos.z + r / 2, gCam.NearClip)),
		D3DXVECTOR3(lightPos.x - r, lightPos.y - r, R3D_MAX(lightPos.z - r / 2, gCam.NearClip)),
		D3DXVECTOR3(lightPos.x - r, lightPos.y + r, R3D_MAX(lightPos.z - r / 2, gCam.NearClip)),
		D3DXVECTOR3(lightPos.x + r, lightPos.y - r, R3D_MAX(lightPos.z - r / 2, gCam.NearClip)),
		D3DXVECTOR3(lightPos.x + r, lightPos.y + r, R3D_MAX(lightPos.z - r / 2, gCam.NearClip)),
		D3DXVECTOR3(lightPos.x, lightPos.y, R3D_MAX(lightPos.z - r, gCam.NearClip)),
	};
	D3DXVec3TransformCoordArray(&bb[0], sizeof(D3DXVECTOR3), &bb[0], sizeof(D3DXVECTOR3), &projMat, _countof(bb));

	float minX = 10000.0f;
	float maxX = -10000.0f;
	float minY = 10000.0f;
	float maxY = -10000.0f;
	float minZ = bb[_countof(bb) - 1].z;

	for (int i = 0; i < _countof(bb) - 1; ++i)
	{
		minX = R3D_MIN(bb[i].x, minX);
		maxX = R3D_MAX(bb[i].x, maxX);
		minY = R3D_MIN(bb[i].y, minY);
		maxY = R3D_MAX(bb[i].y, maxY);
	}

	int tilesXStart = static_cast<int>(floor((minX * 0.5f + 0.5f) * LIGHT_TILE_TEXTURE_SIZE));
	int tilesXEnd = static_cast<int>(ceilf((maxX * 0.5f + 0.5f) * LIGHT_TILE_TEXTURE_SIZE));
	int tilesYStart = static_cast<int>(floor((minY * 0.5f + 0.5f) * LIGHT_TILE_TEXTURE_SIZE));
	int tilesYEnd = static_cast<int>(ceilf((maxY * 0.5f + 0.5f) * LIGHT_TILE_TEXTURE_SIZE));

	tilesXStart = r3dTL::Clamp(tilesXStart, 0, LIGHT_TILE_TEXTURE_SIZE - 1);
	tilesXEnd = r3dTL::Clamp(tilesXEnd, 0, LIGHT_TILE_TEXTURE_SIZE - 1);
	tilesYStart = r3dTL::Clamp(tilesYStart, 0, LIGHT_TILE_TEXTURE_SIZE - 1);
	tilesYEnd = r3dTL::Clamp(tilesYEnd, 0, LIGHT_TILE_TEXTURE_SIZE - 1);

	//	Now loop through influenced tiles, check for light within tile presence and calculate z in that tile
	for (int i = tilesYStart; i <= tilesYEnd; ++i)
	{
		for (int j = tilesXStart; j <= tilesXEnd; ++j)
		{
			uint16_t &numLights = tileCache[0].lightIDs[i][j];
			float &zMin = zMinCache[i][j];
			if (numLights < LIGHT_TILE_TEXTURE_DEPTH - 1)
			{
				numLights += 1;
				tileCache[numLights].lightIDs[i][j] = static_cast<uint16_t>(lightID);
				zMin = R3D_MIN(zMin, minZ);
			}
			else
			{
				_asm nop;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void PointLightsRenderer::FillTileCacheVolumeTexture()
{
	R3DPROFILE_FUNCTION("FillTileCacheVolumeTexture");
	if (!tileCacheVolumeTex || !tileCacheVolumeTex->GetD3DTexture()) return;

	IDirect3DVolumeTexture9 *v = static_cast<IDirect3DVolumeTexture9*>(tileCacheVolumeTex->GetD3DTexture());

	D3DVOLUME_DESC vd;
	v->GetLevelDesc(0, &vd);

	D3DLOCKED_BOX b;

	HRESULT hr = v->LockBox(0, &b, 0, D3DLOCK_DISCARD);
	r3d_assert(SUCCEEDED(hr));
	if (SUCCEEDED(hr))
	{
		size_t dstBufSize = vd.Width * vd.Height * vd.Depth * sizeof(uint16_t);
		size_t srcBufSize = sizeof(tileCache);
		memcpy_s(b.pBits, dstBufSize, &tileCache, srcBufSize);
		hr = v->UnlockBox(0);
		r3d_assert(SUCCEEDED(hr));	
	}
}

//////////////////////////////////////////////////////////////////////////

void PointLightsRenderer::UpdateInstanceData()
{
	R3DPROFILE_FUNCTION("UpdateInstanceData");
	if (!tileInstanceData) return;

	float *d = reinterpret_cast<float *>(tileInstanceData->Lock(0, 0, 0));
	if (!d) return;

	for (int i = 0; i < LIGHT_TILE_TEXTURE_SIZE; ++i)
	{
		int iInv = LIGHT_TILE_TEXTURE_SIZE - 1 - i;
		for (int j = 0; j < LIGHT_TILE_TEXTURE_SIZE; ++j)
		{
			int idx = (i * LIGHT_TILE_TEXTURE_SIZE + j) * 4;
			d[idx + 0] = zMinCache[iInv][j];
			d[idx + 1] = zMinCache[iInv][j];
			d[idx + 2] = zMinCache[iInv][j];
			d[idx + 3] = zMinCache[iInv][j];
		}
	}

	tileInstanceData->Unlock();
}

//////////////////////////////////////////////////////////////////////////

struct LightParams
{
	r3dVector pos;
	float filler;
	float color[4];
	float attenuationParams[2];
	float filler2[2];
};

void PointLightsRenderer::UpdateLightParametersTexture()
{
	R3DPROFILE_FUNCTION("UpdateLightParametersTexture");
	IDirect3DTexture9 *t = static_cast<IDirect3DTexture9*>(lightParamsTex->GetD3DTexture());
	if (!t) return;
	
	D3DLOCKED_RECT r;
	HRESULT hr = t->LockRect(0, &r, 0, D3DLOCK_DISCARD);
	r3d_assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return;

	r3d_assert(r.Pitch == sizeof(float) * 4 * lightParamsTex->GetWidth());
	if (r.Pitch != sizeof(float) * 4 * LIGHT_PARAMS_TEXTURE_SIZE)
		return;

	LightParams *lightData = reinterpret_cast<LightParams*>(r.pBits);

	r3dVector colors[6] =
	{
		r3dVector(1.0f, 1.0f, 1.0f),
		r3dVector(1.0f, 0.0f, 0.0f),
		r3dVector(0.0f, 1.0f, 0.0f),
		r3dVector(0.0f, 0.0f, 1.0f),
		r3dVector(1.0f, 0.0f, 1.0f),
		r3dVector(0.0f, 1.0f, 1.0f),
	};

	for (int i = 0, i_end = WorldLightSystem.nVisibleLights; i != i_end; ++i)
	{
		//	Calculate position of specified lights
		r3dLight &l = *WorldLightSystem.VisibleLights[i];
		r3d_assert(static_cast<int>(l.id) < r.Pitch * lightParamsTex->GetHeight());
		if (static_cast<int>(l.id) >= lightParamsTex->GetWidth() * r.Pitch) continue;
		
		LightParams &ld = lightData[l.id];
		ld.pos = l;
		ld.filler = 1.0f;
		ld.color[0] = l.R / 255.0f * l.Intensity;
		ld.color[1] = l.G / 255.0f * l.Intensity;
		ld.color[2] = l.B / 255.0f * l.Intensity;
		ld.color[3] = l.Intensity;

		float invFadeDistance = 1.0f / (l.GetOuterRadius() - l.GetInnerRadius());
		ld.attenuationParams[0] = invFadeDistance;
		ld.attenuationParams[1] = invFadeDistance * l.GetInnerRadius();
	}

	t->UnlockRect(0);
}

//////////////////////////////////////////////////////////////////////////

PointLightsRenderer gPointLightsRenderer;

} // unnamed namespace

//////////////////////////////////////////////////////////////////////////

void RenderDeferredPointLightsOptimized()
{
#ifdef LIGHTS_RENDERER_V2_ENABLED
	gPointLightsRenderer.RenderLights();
#endif
}

//////////////////////////////////////////////////////////////////////////

void InitPointLightsRendererV2()
{
#ifdef LIGHTS_RENDERER_V2_ENABLED
	gPointLightsRenderer.Init();
#endif
}

//////////////////////////////////////////////////////////////////////////

void DestroyPointLightsRendererV2()
{
#ifdef LIGHTS_RENDERER_V2_ENABLED
	gPointLightsRenderer.Destroy();
#endif
}