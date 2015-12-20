#pragma once

#include "Shader.h"
#include "SceneParameter.h"

class CloudGrid;

// Shader for rendering cloud density
class RenderDensityShader : public Shader {
public :
	// vertex shader constants
	enum {
		VS_CONST_W2C = 0, // transform world to projection space
		VS_CONST_XZPARAM, // xz scale and offset of the position
		VS_CONST_HEIGHT,    // parameter to compute height
		VS_CONST_EYE,       // view position
		VS_CONST_UVPARAM,   // uv scale and offset
		VS_CONST_NUM,
	};
	// pixel shader constants
	enum {
		PS_CONST_COVER = 0, // cloud cover
		PS_CONST_NUM,
	};

public :
	void Create();
	void Begin(CloudGrid* pCloud, const SSceneParamter& sceneParam);
};


// Shader for rendering clouds to shadow map
class RenderShadowShader : public Shader 
{
public :
	void Create();
	void Update(const SBoundingBox* pGround, const SBoundingBox* pCloud, const SSceneParamter& sceneParam);
	void Begin(CloudGrid* pCloud, const SSceneParamter& sceneParam, LPDIRECT3DTEXTURE9 cloudTex);

	inline const D3DXMATRIX* GetW2ShadowMapMatrix() const;
protected :
	D3DXMATRIX            m_mW2SProj;        // Transform world to Projection matrix to render to the shadowmap
	D3DXMATRIX            m_mW2S;            // Transform world to shadowmap texture coordinate
};

// Return transform world space to shadowmap texture coordinate
const D3DXMATRIX* RenderShadowShader::GetW2ShadowMapMatrix() const
{
	return &m_mW2S;
}

//    Shader for blur density.
struct CloudBlurMesh 
{
	struct S_VERTEX 
	{
		float afPos[4];
		float afTex[2];
	};

	CloudBlurMesh():decl(0){}
	void create();
	void draw();
	void destroy();

	r3dD3DVertexBufferTunnel     vb;
	LPDIRECT3DVERTEXDECLARATION9 decl;
};

class CloudBlurShader : public Shader 
{
public:
	~CloudBlurShader();

	void Create();
	void Delete();

	void Blur(r3dTexture* pTex, const SSceneParamter& sceneParam);

protected:
	void CreateShaders();
	void SetShaderConstant(r3dTexture* pTex, const SSceneParamter& sceneParam);
	CloudBlurMesh mesh;
};



// render final cloud as a screen quad.
struct CloudPlaneMesh
{
	struct S_VERTEX 
	{
		D3DXVECTOR4 vPos;
		float vTex[2];
	};

	enum 
	{
		DIV_X = 4,
		DIV_Y = 4,
		NUM_VERTICES = (DIV_X+1) * (DIV_Y+1),
		NUM_INDICES  = 2*DIV_Y * (DIV_X+1) + (DIV_Y-1)*2,
		NUM_TRIANGLES = NUM_INDICES-2,
	};

	CloudPlaneMesh(): decl(0){}
	void create();
	void draw();
	void destroy();

	r3dD3DVertexBufferTunnel     vb;  
	r3dD3DIndexBufferTunnel      ib;  
	LPDIRECT3DVERTEXDECLARATION9 decl;
};

class CloudPlaneShader : public Shader 
{
public :
	~CloudPlaneShader();

	void Create();
	void Delete();
	void Draw(const SSceneParamter& sceneParam, r3dTexture* pDensityMap, r3dTexture* pBlurredMap);

protected :
	void CreateShaders();
	void SetShaderConstant(const SSceneParamter& sceneParam);
	CloudPlaneMesh mesh;
};
