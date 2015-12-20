#include "r3dPCH.h"
#include "r3d.h"
#include "Cloud.h"	 

Cloud::Cloud() : m_pDensityMap(NULL), m_pBlurredMap(NULL)//, m_pShadowMap(NULL)
{
}

Cloud::~Cloud()
{
	Delete();
}

void Cloud::Delete()
{
	R3D_ENSURE_MAIN_THREAD();

	D3DVIEWPORT9 viewport;
	r3dRenderer->GetViewport(&viewport);

	int halfW = viewport.Width/2;
	int halfH = viewport.Height/2;

	SAFE_DELETE(m_pDensityMap);
	SAFE_DELETE(m_pBlurredMap);

	m_finalCloud.Delete();
	m_grid.Delete();
	m_blur.Delete();
}

void Cloud::Create()
{
	// create render targets of density map and blurred density map.

	float halfW = float( r3dRenderer->d3dpp.BackBufferWidth/2 );
	float halfH = float( r3dRenderer->d3dpp.BackBufferHeight/2 );

	m_pDensityMap = r3dScreenBuffer::CreateClass("Clouds Density Map", halfW, halfH, D3DFMT_A8R8G8B8);
	m_pBlurredMap = r3dScreenBuffer::CreateClass("Clouds Blurred Map", halfW, halfH, D3DFMT_A8R8G8B8);
	m_pDensityMap->SetDebugD3DComment("Clouds Density Map");
	m_pDensityMap->SetDebugD3DComment("Clouds Blurred Map");

	m_grid.Create();
	m_densityShader.Create();
	m_shadowShader.Create();
	m_blur.Create();
	m_finalCloud.Create();
}



// Animate clouds and compute shadowmap volume.
void Cloud::Update(float dt, const SBoundingBox& bbGround, const SSceneParamter& sceneParam)
{
	// animate uv 
	m_grid.Update( dt, sceneParam.farPlane, sceneParam.eyePt, sceneParam.m_fWindVel );

	// compute transform matrix for shadowmap
	m_shadowShader.Update( &bbGround, &m_grid.GetBoundingBox(), sceneParam );
}


// Render clouds into redertargets before scene rendering 
//  Cloud shadowmap, densitymap are rendered and then the density map is blurred.
void Cloud::PrepareCloudTextures(r3dTexture* cloudTex, const SSceneParamter& sceneParam)
{
	LPDIRECT3DDEVICE9 pDev = r3dRenderer->pd3ddev;

	// preserve current render target 
 	LPDIRECT3DSURFACE9 pCurrentSurface;
 	r3dRenderer->GetRT( 0, &pCurrentSurface );

	// Setup render states.
	// All passes in this function do not require a depth buffer and alpha blending 
	//  because there is no multiple clouds in this demo.
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDev->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
	pDev->SetRenderState( D3DRS_ZENABLE, FALSE );

// 	// Pass 1 : Render clouds to a shadow map 


	// Pass 2 : Render cloud density 
	
	m_pDensityMap->Activate() ;

	pDev->Clear( 0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0 );
	m_densityShader.Begin(&m_grid, sceneParam);
	m_grid.Draw(cloudTex);

	m_pDensityMap->Deactivate();

	// Pass 3 : Blur the density map

	if(r_environment_quality->GetInt()!=1)
	{
 		m_pBlurredMap->Activate();

		m_blur.Blur(m_pDensityMap->Tex, sceneParam );

		m_pBlurredMap->Deactivate();
	}

	// restore render target and render states.
 	r3dRenderer->SetRT( 0, pCurrentSurface );
 
 	pCurrentSurface->Release();
}

// Render final clouds with daylight scattering 
void Cloud::DrawFinalQuad(const SSceneParamter& sceneParam)
{
	m_finalCloud.Draw(sceneParam, m_pDensityMap->Tex, m_pBlurredMap->Tex );
}

void Cloud::SetRenderTarget(r3dScreenBuffer* screenBuf) 
{
	screenBuf->Activate();
// 	LPDIRECT3DDEVICE9 pDev = r3dRenderer->pd3ddev;
// 
// 	LPDIRECT3DSURFACE9 pSurface;
// 	pTex->GetSurfaceLevel( 0, &pSurface );
// 	pDev->SetRenderTarget( 0, pSurface );
// 	pSurface->Release();
}

// fCloudCover = [0.0f 1.0f]
void Cloud::SetCloudCover(float fCloudCover)
{
	m_grid.SetCloudCover( fCloudCover );
}

float Cloud::GetCurrentCloudCover() const
{
	return m_grid.GetCurrentCloudCover();
}

const D3DXMATRIX* Cloud::GetWorld2ShadowMatrix() const
{
	return m_shadowShader.GetW2ShadowMapMatrix();
}
