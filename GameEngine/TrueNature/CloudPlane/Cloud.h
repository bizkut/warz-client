#pragma once

#include "CloudGrid.h"
#include "Shaders.h"


// Interface object to light and render clouds.
class Cloud
{
public :
	Cloud();
	~Cloud();

	void Create();
	void Delete();

	void Update(float dt, const SBoundingBox& bbGround, const SSceneParamter& sceneParam);

	void PrepareCloudTextures(r3dTexture* cloudTex, const SSceneParamter& sceneParam);
	void DrawFinalQuad(const SSceneParamter& sceneParam);

	void SetCloudCover(float fCloudCover);
	float GetCurrentCloudCover() const;
	const D3DXMATRIX* GetWorld2ShadowMatrix() const;
	//LPDIRECT3DTEXTURE9 GetShadowMap();

protected :
	void SetRenderTarget(r3dScreenBuffer* screenBuf);

protected :
	// render targets
	r3dScreenBuffer*    m_pBlurredMap;
	r3dScreenBuffer*	m_pDensityMap;
//	LPDIRECT3DTEXTURE9     m_pShadowMap;         // shadow map

	CloudGrid             m_grid;               // cloud grid
	RenderDensityShader   m_densityShader;      // shader to render density 
	RenderShadowShader    m_shadowShader;       // shader to render shadow
	CloudBlurShader			  m_blur;               // blur shader
	CloudPlaneShader            m_finalCloud;         // object to render a screen cloud in the final pass 
};
