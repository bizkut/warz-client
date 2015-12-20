#include "r3dPCH.h"
#include "r3d.h"
#include "Shaders.h"
#include "Cloud.h"
#include "CloudGrid.h"
#include "GameLevel.h"

void SetD3DResourcePrivateData(LPDIRECT3DRESOURCE9 res, const char* FName);

void RenderDensityShader::Create()
{
	Shader::Create("CloudPlane/CloudGrid_vs.hls", "CloudPlane/CloudGrid_ps.hls");
}


void RenderDensityShader::Begin(CloudGrid* pCloud, const SSceneParamter& sceneParam)
{
	LPDIRECT3DDEVICE9 pDev = r3dRenderer->pd3ddev;
	SetShaders();

	// world to projection transform 
	D3DXMATRIX mtx = sceneParam.viewProj;
	D3DXMatrixTranspose(&mtx, &mtx);
	pDev->SetVertexShaderConstantF(0, (float*)&mtx, 4);
	// view position
	pDev->SetVertexShaderConstantF(6, (float*)&sceneParam.eyePt, 1);

	CloudGrid::SVSParam param;
	pCloud->GetVSParam( param, sceneParam );
	// uv scale and offset parameter
	pDev->SetVertexShaderConstantF(7, (float*)&param.vUVParam, 1);
	// xz position scale and offset parameter
	pDev->SetVertexShaderConstantF(4, (float*)&param.vXZParam, 1);
	// height parameters
	pDev->SetVertexShaderConstantF(5, (float*)&param.vHeight, 1);

	// cloud cover
	float fCloudCover = pCloud->GetCurrentCloudCover();
	D3DXVECTOR4 v(sceneParam.m_vFade_Density.x + sceneParam.eyePt.y, sceneParam.m_vFade_Density.y + sceneParam.eyePt.y, sceneParam.m_vFade_Density.z, fCloudCover);
	pDev->SetPixelShaderConstantF(8, (float*)&v, 1);
}


//   Shader for rendering cloud grid to shadowmap
void RenderShadowShader::Create()
{
	Shader::Create("CloudPlane/CloudShadow_vs.hls", "CloudPlane/CloudShadow_ps.hls");
}



//   Compute volume of the shadow
void RenderShadowShader::Update(const SBoundingBox* pGround, const SBoundingBox* pCloud, const SSceneParamter& sceneParam)
{
	// look at the scene from light
	D3DXVECTOR3 vLight( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUp( 0.0f, 1.0f, 0.0f );
	D3DXVECTOR3 vAt;
	D3DXVec3Add( &vAt, &vLight, &sceneParam.m_vLightDir );
	D3DXMATRIX mW2Light;
	D3DXMatrixLookAtLH( &mW2Light, &vLight, &vAt, &vUp );

	// transform ground and cloud bounding box to the light coordinate
	SBoundingBox bbGroundInView;
	SBoundingBox bbCloudInView;
	pGround->Transform( bbGroundInView, &mW2Light );
	pCloud->Transform( bbCloudInView, &mW2Light );

	// minimize bounding box 
	// The view frustom should be take into account as well.
	SBoundingBox bound;
	D3DXVec3Minimize( &bound.vMax, &bbGroundInView.vMax, &bbCloudInView.vMax );
	D3DXVec3Maximize( &bound.vMin, &bbGroundInView.vMin, &bbCloudInView.vMin );
	bound.vMin.z = bbCloudInView.vMin.z;

	// if there is a valid volume 
	if (bound.vMin.x < bound.vMax.x && bound.vMin.y < bound.vMax.y && bound.vMin.z < bound.vMax.z) {
		D3DXVECTOR3 vCenter;
		D3DXVECTOR3 vDiag;
		bound.Centroid( &vCenter );
		D3DXVec3Subtract( &vDiag, &bound.vMax, &bound.vMin );

		// Move the view position to the center of the bounding box.
		// z is located behined the volume.
		D3DXVECTOR3 vEye( vCenter );
		vEye.z = vCenter.z - 0.5f * vDiag.z;
		D3DXVECTOR3 vMove;
		D3DXVec3Subtract( &vMove, &vLight, &vEye );
		D3DXMATRIX mTrans;
		D3DXMatrixTranslation( &mTrans, vMove.x, vMove.y, vMove.z );

		// Orthogonal projection matrix
		D3DXMATRIX mProj;
		r3dRenderer->BuildMatrixOrthoLH( &mProj, vDiag.x, vDiag.y, 0.0f, vDiag.z );

		// Compute world to shadow map projection matrix
		D3DXMatrixMultiply( &m_mW2SProj, &mW2Light, &mTrans );
		D3DXMatrixMultiply( &m_mW2SProj, &m_mW2SProj, &mProj );

		// Compute world to shadowmap texture coordinate matrix
		D3DXMATRIX mProj2Tex(
			0.5f,  0.0f, 0.0f, 0.0f,
			0.0f, -0.5f, 0.0f, 0.0f,
			0.0f,  0.0f, 1.0f, 0.0f,
			0.5f,  0.5f, 0.0f, 1.0f );
		D3DXMatrixMultiply( &m_mW2S, &m_mW2SProj, &mProj2Tex );
	}
}



// Setup shaders and shader constants.

void RenderShadowShader::Begin(CloudGrid* pCloud, const SSceneParamter& sceneParam, LPDIRECT3DTEXTURE9 cloudTex)
{
	LPDIRECT3DDEVICE9 pDev = r3dRenderer->pd3ddev;

	SetShaders();

	// world to projection transform 
	D3DXMATRIX mtx = m_mW2SProj;
	D3DXMatrixTranspose(&mtx, &mtx);
	pDev->SetVertexShaderConstantF(0, (float*)&mtx, 4 );
	// view position
	pDev->SetVertexShaderConstantF(6, (float*)&sceneParam.eyePt, 1 );

	CloudGrid::SVSParam param;
	pCloud->GetVSParam( param, sceneParam );		
	// uv scale and offset parameter
	pDev->SetVertexShaderConstantF(7, (float*)&param.vUVParam, 1 );
	// xz position scale and offset parameter
	pDev->SetVertexShaderConstantF(4, (float*)&param.vXZParam, 1 );
	// height parameters
	pDev->SetVertexShaderConstantF(5, (float*)&param.vHeight, 1 );

	// cloud cover
	float fCloudCover = pCloud->GetCurrentCloudCover();
	pDev->SetPixelShaderConstantF(0, (float*)&fCloudCover, 1 );
}



//
//
//
void CloudBlurMesh::create()
{
	r3dDeviceTunnel::CreateVertexBuffer( sizeof(S_VERTEX)*4, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &vb );
	r3dDeviceTunnel::SetD3DResourcePrivateData( &vb, "CloudPlane: VB");

	r3dRenderer->Stats.AddBufferMem ( sizeof(S_VERTEX)*4 );

	S_VERTEX* pV = NULL;
	vb.Lock( 0, 0, (void**)&pV, 0 );

	static const S_VERTEX s_vertices[] = {
		{ {  1.0f,  1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
		{ {  1.0f, -1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
		{ { -1.0f,  1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
		{ { -1.0f, -1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
	};
	memcpy( pV, s_vertices, sizeof(s_vertices) );
	vb.Unlock();


	static const D3DVERTEXELEMENT9 s_elements[] = 
	{
		{ 0,  0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 16,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	r3dDeviceTunnel::CreateVertexDeclaration( s_elements, &decl );
}

void CloudBlurMesh::destroy()
{
	if( vb.Valid() )
	{
		R3D_ENSURE_MAIN_THREAD();

		vb.ReleaseAndReset();
		r3dRenderer->Stats.AddBufferMem ( -(int)sizeof(S_VERTEX)*4 );
	}
}

void CloudBlurMesh::draw()
{
	LPDIRECT3DDEVICE9 pDev = r3dRenderer->pd3ddev;

	d3dc._SetDecl( decl );
	d3dc._SetStreamSource( 0, vb.Get(), 0, sizeof(S_VERTEX) );

	r3dRenderer->Draw(D3DPT_TRIANGLESTRIP, 0, 2 );
}

//
//
CloudBlurShader::~CloudBlurShader()
{
	Delete();
}


void CloudBlurShader::Delete()
{
	mesh.destroy();
}

void CloudBlurShader::Create()
{
	Delete();
	mesh.create();
	CreateShaders();
}

void CloudBlurShader::SetShaderConstant(r3dTexture* pTex, const SSceneParamter& sceneParam)
{
	LPDIRECT3DDEVICE9 pDev = r3dRenderer->pd3ddev;

	// offset parameter to sample center of texels.
	D3DXVECTOR2 vv( 0.5f / (float)pTex->GetWidth(), 0.5f / (float)pTex->GetHeight() );
	pDev->SetVertexShaderConstantF(4, (float*)&vv, 1);

	// view position
	pDev->SetPixelShaderConstantF(9, (float*)&sceneParam.eyePt, 1);

	// transform screen position to world space
	D3DXMATRIX mC2W;
	D3DXMatrixInverse( &mC2W, NULL, &sceneParam.viewProj );
	D3DXMatrixTranspose(&mC2W, &mC2W);
	pDev->SetVertexShaderConstantF(0, (float*)&mC2W, 4);

	// Directional Light in projection space.
	D3DXVECTOR4 vLit( sceneParam.m_vLightDir.x, sceneParam.m_vLightDir.y, sceneParam.m_vLightDir.z, 0.0f );
	if ( vLit.y > 0.0f )
	{
		// assuming light direction is horizontal when sunset or sunrise.
		// otherwise, shadow of clouds converges at a point on the screen opposite to the light.
		vLit.y = 0.0f;
		D3DXVec4Normalize( &vLit, &vLit );
	}
	D3DXVECTOR4 vProjPos;
	D3DXVec4Transform( &vProjPos, &vLit, &sceneParam.viewProj );

	// blur vector = vBlurDir.xy * uv + vBlurDir.zw 
	D3DXVECTOR4 vBlurDir;
	static const float EPSIRON = 0.000001f;
	if ( ( fabsf(vProjPos.w) < EPSIRON )|| ( fabsf(vProjPos.z) < EPSIRON ) ) 
	{
		// if dot( litdir, ray ) == 0.0f : directional.
		// light is stil directional in projection space.
		vProjPos.w = vProjPos.z = 0.0f;
		D3DXVec4Normalize( &vProjPos, &vProjPos );
		vProjPos.y = -vProjPos.y;
		// directional blur
		vBlurDir = D3DXVECTOR4( 0.0f, 0.0f, -vProjPos.x, -vProjPos.y );			
	}
	else 
	{
		// otherwise : point blur.
		// light direction is a position in projection space.
		if ( 0.0f < vProjPos.w ) 
		{
			// transform screen position to texture coordinate
			D3DXVec4Scale( &vProjPos, &vProjPos, 1.0f/vProjPos.w );
			vProjPos.x =  0.5f * vProjPos.x + 0.5f; //  
			vProjPos.y = -0.5f * vProjPos.y + 0.5f; // upside down 
			vBlurDir = D3DXVECTOR4( 1.0f, 1.0f, -vProjPos.x, -vProjPos.y );
		}
		else 
		{
			// transform screen position to texture coordinate
			D3DXVec4Scale( &vProjPos, &vProjPos, 1.0f/vProjPos.w );
			vProjPos.x =  0.5f * vProjPos.x + 0.5f; //  
			vProjPos.y = -0.5f * vProjPos.y + 0.5f; // upside down 
			// invert vector if light comes from behind the camera.
			vBlurDir = D3DXVECTOR4( -1.0f, -1.0f, vProjPos.x, vProjPos.y );
		}
	}
	pDev->SetVertexShaderConstantF(5, (float*)&vBlurDir, 1);

	// parameter to scale down blur vector acoording to the distance from the view position.
	SScatteringShaderParameters param;
	sceneParam.GetShaderParam( param );

	D3DXVECTOR3 v( param.vESun.w, param.vSum.w, sceneParam.m_fAtomosHeight );
	pDev->SetPixelShaderConstantF(6, (float*)&v, 1);


	// maximum length of blur vector in texture space.
	float fMaxMove = 0.1f/(float)16;
	D3DXVECTOR2 vInvMax( 1.0f/fMaxMove, 1.0f/fMaxMove );
	pDev->SetPixelShaderConstantF(8, (float*)&vInvMax, 1);


	// fall off parameter of weights.
	D3DXVECTOR4 vFallOff( -5000.0f, -1.5f, -1.5f, -1000.0f );
	pDev->SetPixelShaderConstantF(7, (float*)&vFallOff, 1);
}


//  Blur the indicated texture pTex 
void CloudBlurShader::Blur(r3dTexture* pTex, const SSceneParamter& sceneParam)
{
	LPDIRECT3DDEVICE9 pDev = r3dRenderer->pd3ddev;

	SetShaders();
	SetShaderConstant(pTex, sceneParam );

	r3dRenderer->SetTex(pTex, 0);
	pDev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	pDev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	mesh.draw();
}


void CloudBlurShader::CreateShaders()
{
	Shader::Create("CloudPlane/CloudBlur_vs.hls", "CloudPlane/CloudBlur_ps.hls");
}



// CloudPlane 
void CloudPlaneMesh::create()
{
	// Create vertex declaration
	static const D3DVERTEXELEMENT9 s_elements[] = 
	{
		{ 0,  0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 16,  D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	r3dDeviceTunnel::CreateVertexDeclaration( s_elements, &decl );

	// create vertex buffer
	r3dDeviceTunnel::CreateVertexBuffer( sizeof(S_VERTEX)*NUM_VERTICES, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &vb );
	r3dDeviceTunnel::SetD3DResourcePrivateData( &vb, "CloudPlane: VB");
	r3dRenderer->Stats.AddBufferMem ( sizeof(S_VERTEX)*NUM_VERTICES );

	S_VERTEX* pV = NULL;
	vb.Lock( 0, 0, (void**)&pV, 0 );

	float fDepth = 0.99999f;
	for (unsigned int i = 0; i <= DIV_Y; ++i) 
	{
		for (unsigned int j = 0; j <= DIV_X; ++j) 
		{
			float fX = 1.0f - j/(float)(DIV_X);
			float fY = i/(float)(DIV_Y);
			pV->vPos = D3DXVECTOR4( fX*2.0f-1.0f, -(fY*2.0f-1.0f), fDepth, 1.0f );
			pV->vTex[0] = fX;
			pV->vTex[1] = fY;
			++pV;
		}
	}
	vb.Unlock();

	// create index buffer
	r3dDeviceTunnel::CreateIndexBuffer( sizeof(USHORT)*NUM_INDICES, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &ib );
	r3dDeviceTunnel::SetD3DResourcePrivateData(&ib, "CloudPlane: IB");

	r3dRenderer->Stats.AddBufferMem ( sizeof(USHORT)*NUM_INDICES );

	USHORT* pI = NULL;
	ib.Lock( 0, 0, (void**)&pI, 0 );

	for (USHORT i = 0; i < DIV_Y; ++i) 
	{
		for (USHORT j = 0; j <= DIV_X; ++j) 
		{
			(*pI) = i*(DIV_X+1) + j;
			++pI;
			(*pI) = (i+1)*(DIV_X+1) + j;
			++pI;
		}

		if (i+1 < DIV_Y) 
		{
			(*pI) = (i+1)*(DIV_X+1) + DIV_X;
			++pI;
			(*pI) = (i+1)*(DIV_X+1);
			++pI;
		}
	}
	ib.Unlock();
}

void CloudPlaneMesh::destroy()
{
	if( vb.Valid() )
	{
		R3D_ENSURE_MAIN_THREAD();

		vb.ReleaseAndReset();
		ib.ReleaseAndReset();

		r3dRenderer->Stats.AddBufferMem( -(int)sizeof(USHORT)*NUM_INDICES );
		r3dRenderer->Stats.AddBufferMem( -(int)sizeof(S_VERTEX)*NUM_VERTICES );
	}
}

void CloudPlaneMesh::draw()
{
	LPDIRECT3DDEVICE9 pDev = r3dRenderer->pd3ddev;

	d3dc._SetIndices( ib.Get() );
	d3dc._SetStreamSource( 0, vb.Get(), 0, sizeof(S_VERTEX) );
	d3dc._SetDecl( decl );

	r3dRenderer->DrawIndexed(D3DPT_TRIANGLESTRIP, 0, 0, NUM_VERTICES, 0, NUM_TRIANGLES);
}


CloudPlaneShader::~CloudPlaneShader()
{
	Delete();
}

void CloudPlaneShader::Delete()
{
	mesh.destroy();
}

void CloudPlaneShader::Create()
{
	Delete();

	// create index and vertex buffer.
	mesh.create();

	// Create shaders
	CreateShaders();
}

void CloudPlaneShader::CreateShaders()
{
	//Shader::Create("CloudPlane/CloudPlane_vs.hls", "CloudPlane/CloudPlane_ps.hls");
}



void CloudPlaneShader::Draw(const SSceneParamter& sceneParam, r3dTexture* pDensityMap, r3dTexture* pBlurredMap)
{
	LPDIRECT3DDEVICE9 pDev = r3dRenderer->pd3ddev;

	//SetShaders();
	if(r_environment_quality->GetInt()==1)
	{
	 	r3dRenderer->SetVertexShader("VS_CLOUD_PLANE");
 		r3dRenderer->SetPixelShader("PS_CLOUD_PLANELQ");
	}
	else
	{
	 	r3dRenderer->SetVertexShader("VS_CLOUD_PLANE");
 		r3dRenderer->SetPixelShader("PS_CLOUD_PLANE");
	}

	SetShaderConstant(sceneParam);

	// set textures
	r3dRenderer->SetTex(pDensityMap, 0);
	pDev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	pDev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	r3dRenderer->SetTex(pBlurredMap, 1);
	pDev->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	pDev->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

	mesh.draw();
}



void CloudPlaneShader::SetShaderConstant(const SSceneParamter& sceneParam)
{
	LPDIRECT3DDEVICE9 pDev = r3dRenderer->pd3ddev;

	D3DXMATRIX mC2W;
	D3DXMatrixInverse( &mC2W, NULL, &sceneParam.viewProj );
	D3DXMatrixTranspose(&mC2W, &mC2W);

	pDev->SetVertexShaderConstantF(0, (float*)&mC2W, 4);
	pDev->SetVertexShaderConstantF(4, (float*)&sceneParam.eyePt, 1);
	pDev->SetVertexShaderConstantF(5, (float*)&sceneParam.m_vLightDir, 1);

	pDev->SetPixelShaderConstantF(4, (float*)&sceneParam.eyePt, 1);
	pDev->SetPixelShaderConstantF(5, (float*)&sceneParam.m_vLightDir, 1);
	pDev->SetPixelShaderConstantF(12, (float*)&sceneParam.m_vLightColor, 1);
	pDev->SetPixelShaderConstantF(13, (float*)&sceneParam.m_vAmbientLight, 1);

	SScatteringShaderParameters param;
	sceneParam.GetShaderParam( param );
	pDev->SetPixelShaderConstantF(6, (float*)&param, 5);

	// parameter to compute distance of cloud.
	D3DXVECTOR2 v;
	sceneParam.GetCloudDistance( v );
	pDev->SetPixelShaderConstantF(11, (float*)&v, 1);
}
