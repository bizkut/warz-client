#include "r3dPCH.h"
#include "r3d.h"

#include "skydome.h"
#include "sun.h"
#include "cubemap.h"

#include "XPSObject.h"

#include "skydome.h"
#include "sun.h"

#include "rendering/Deffered/RenderDeffered.h"

#include "r3dDeviceQueue.h"

extern r3dSun		*Sun;


// A structure for our custom vertex type


LPDIRECT3DVERTEXDECLARATION9 R3D_SKY_VERTEX::pDecl = 0;
D3DVERTEXELEMENT9 R3D_SKY_VERTEX::VBDecl[] = 
{
	{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END()
};

LPDIRECT3DVERTEXDECLARATION9 R3D_SKY_VERTEX::getDecl()
{
	if(pDecl == 0)
	{
		r3dDeviceTunnel::CreateVertexDeclaration( VBDecl, &pDecl ) ;
		r3d_assert(pDecl);
	}
	return pDecl;
}

r3dXPSObject<R3D_SKY_VERTEX>*	SkyDomeXPS;
r3dXPSObject<R3D_SKY_VERTEX>*	SkyDomeDown;

CubemapTransform::Matrices cubeMtx;

bool g_bSkyDomeNeedFullUpdate = false;
bool updateAfterRestore = false;

StaticSkySettings::StaticSkySettings()
: bEnabled( 0 )
, bPlanarMapping( 0 )
, tex0( 0 )
, tex1( 0 )
, glowTex0( 0 )
, glowTex1( 0 )
, texLerpT( 0.f )
, mesh( 0 )
, texScaleX( 0.0001f )
, texScaleY( 0.0001f )
, texOffsetX( 0.5f )
, texOffsetY( 0.5f )

, SunCtrlX( 4.0f )
, SunCtrlY( 2.0f )
, SunSpotColor( 0, 0, 0 )

, SunSpotIntensity( 2.0f )
, dayT( 0.f )

, bIsNight( 0 )
, SunIntensity( 1.f )

, DomeRotationY( 0.f )

{

}

r3dSkyDome::r3dSkyDome( const r3dIntegrityGuardian& ig )
: r3dIResource( ig )
, cloudTex(0), cloudCoverTex(0)
, m_fFogHeight		( 200 )
, m_fFogOffset		( 0 )
, m_volumeFogColor	( 255, 255, 255 )
, m_fCloadAnim		( 0 )
, m_fWindFactor		( 1 )
, StatidSkyVS_ID	( -1 )
, StatidSkyTexgVS_ID( -1 )
{
	memset( faceRTs, 0, sizeof faceRTs );

	bLoaded		= 0;
	cubemap		= 0;
}

r3dSkyDome ::~r3dSkyDome()
{
	Unload();
}

void r3dSkyDome::SetParams(float fFogHeight, float fFogOffset, r3dColor volumeFogColor, float fWindFactor)
{
	m_fFogHeight = fFogHeight;
	m_fFogOffset = fFogOffset;
	m_volumeFogColor = volumeFogColor;
	m_fWindFactor = fWindFactor;
}

void r3dSkyDome::SetStaticSkyParams( const StaticSkySettings& sts )
{
	SSTSettings = sts ;
}

BOOL r3dSkyDome::Load(const char* DirName)
{
	bLoaded  = 1;

	SkyDomeXPS = gfx_new r3dXPSObject<R3D_SKY_VERTEX>;
	SkyDomeDown = gfx_new r3dXPSObject<R3D_SKY_VERTEX>;

	SkyDomeXPS->InitDome(25160.0, 3000.0, 64, 64);
	SkyDomeDown->InitDome(25160.0, -3000.0, 64, 64);

	cubemap = r3dScreenBuffer::CreateClass( "Skydome cubemap", 128, 128, D3DFMT_R5G6B5, r3dScreenBuffer::Z_NO_Z, 1, 0 );
	cubemap->SetDebugD3DComment("r3dSkyDome");

	for( int i = 0, e = R3D_ARRAYSIZE( faceRTs ) ; i < e ; i ++ )
	{
		faceRTs[ i ] = r3dScreenBuffer::CreateClass( "Temp Aerial Cubemap Face", 128, 128, D3DFMT_R5G6B5 );
	}

	tempRt0 = r3dScreenBuffer::CreateClass( "Temp Cubemap RT0", 128, 128, D3DFMT_R5G6B5 );
	tempRt0->SetDebugD3DComment("r3dSkyDome helper RT0");

	updateAfterRestore = true;	//redraw cubemap

	CubemapTransform::getMatrices(D3DXVECTOR3(0,0,0), cubeMtx);

	hoffman.create();

	StatidSkyVS_ID = r3dRenderer->GetVertexShaderIdx( "VS_SKYSTATIC" );
	StatidSkyTexgVS_ID = r3dRenderer->GetVertexShaderIdx( "VS_SKYSTATIC_TEXG" );

	r3d_assert( StatidSkyVS_ID >= 0 &&
				StatidSkyTexgVS_ID >= 0		);

	return TRUE;
}

BOOL r3dSkyDome::Unload()
{
	if(!bLoaded)
		return 0;

	for( int i = 0, e = R3D_ARRAYSIZE( faceRTs ) ; i < e ; i ++ )
	{
		SAFE_DELETE(faceRTs[ i ]);
	}

	SAFE_DELETE(cubemap);
	SAFE_DELETE(tempRt0);

	r3dRenderer->DeleteTexture(cloudTex);
	r3dRenderer->DeleteTexture(cloudCoverTex);

	cloudTex = NULL ;
	cloudCoverTex = NULL ;

	SAFE_DELETE(SkyDomeXPS);
	SAFE_DELETE(SkyDomeDown);

	bLoaded = 0;
	return TRUE;
}

static R3D_FORCEINLINE void SetTransformMatrix( const D3DXMATRIX& world, const D3DXMATRIX& mtx )
{
	D3DXMATRIX ShaderMat[2];

	// float4x4  MVP                       : register(c0);
	D3DXMatrixTranspose( &ShaderMat[0], &mtx );
	// float4x3  WorldM                    : register(c4);
	D3DXMatrixTranspose( &ShaderMat[1], &world );

	r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float *)&ShaderMat,  8 );
}

void
r3dSkyDome::DrawDome( const r3dCamera& Cam, const D3DXMATRIX& viewProj, float mieScale, bool normals, bool hemisphere, bool hdr )
{
	r3dVector V = -Sun->SunLight.Direction;
	V.Normalize();

	D3DXMATRIX ShaderMat;
	bool customMesh = false;

	r3dPoint2D tCloudsDim (256.0f, 256.0f);

	hoffman.renderSkyLight( Cam, V, r3dPoint3D(m_fFogHeight, m_fFogOffset, m_fCloadAnim), m_volumeFogColor, tCloudsDim, normals, mieScale, Sun->GetAngleAtNormalizedTime( Sun->TimeToValD( Sun->Time ) ), Sun->AngleRange, !SSTSettings.bEnabled );

	if( SSTSettings.bEnabled )
	{
		StaticSkyPShaderID psid;

		psid.twoTextures	= SSTSettings.tex0 != SSTSettings.tex1;
		psid.outputNormals	= normals ? 1 : 0;
		psid.scatterSun		= 1;
		psid.hdr			= hdr;

		int id = gStaticSkyPSIds[ psid.Id ];

		r3d_assert( id >= 0 );
		
		r3dRenderer->SetPixelShader( id ) ;

		r3dSetFiltering( R3D_BILINEAR, 0 );
		r3dSetFiltering( R3D_BILINEAR, 1 );

		r3dSetFiltering( R3D_BILINEAR, 2 );
		r3dSetFiltering( R3D_BILINEAR, 3 );

		r3dRenderer->SetTex( SSTSettings.tex0, 0 );
		r3dRenderer->SetTex( SSTSettings.tex1, 1 );

		r3dRenderer->SetTex( SSTSettings.glowTex0, 2 );
		r3dRenderer->SetTex( SSTSettings.glowTex1, 3 );

		for( int i = 0 ; i < 4 ; i ++ )
		{
			D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP ) );
			D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP ) );
		}

#if 0
		r3dPoint3D lightDir = GetEnvLightDir();

		r3dColor color = SSTSettings.SunSpotColor;
		float convCoef = SSTSettings.SunSpotIntensity / 255.0f;

		float dayT = SSTSettings.dayT;
		float intoNight = R3D_CLAMP( dayT * ( 1 - dayT ) * 64.f, 0.f, 1.f );

		intoNight *= intoNight;
		intoNight *= intoNight;

		convCoef *= intoNight;

		float psConsts[3][4] =
		{
			// float4 Amplify_SkyLerp_SunCtrl  : register( c0 );
			{ amplify, SSTSettings.texLerpT, SSTSettings.SunCtrlX, SSTSettings.SunCtrlY },
			// float3 SkyDir                   : register( c1 );
			{ -lightDir.x, -lightDir.y, -lightDir.z, 0.f },
			// float3 SunColor                 : register( c2 );
			{ color.R * convCoef, color.G * convCoef, color.B * convCoef, 0.f }
		};

		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, psConsts[0], sizeof psConsts / sizeof( float[4] ) ) );
#else
		float intoNight = 0.f;

		if( SSTSettings.bIsNight )
		{
			intoNight = 0.f;
		}
		else
		{
			intoNight = 1.f;
		}

#if 0
		float dayT = SSTSettings.dayT;
		intoNight *= R3D_CLAMP( dayT * ( 1 - dayT ) * 8.f, 0.f, 1.f );
#endif

		float psConsts[ 4 ] = { SSTSettings.texLerpT, intoNight * SSTSettings.SunIntensity, 0.0f, 0.0f };
		// float4    SkyLerp_SpotAmplify  : register(c13);
		D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 13, psConsts, sizeof psConsts / sizeof( float[4] ) ) );
#endif

		if( SSTSettings.mesh )
		{
			r3dRenderer->SetVertexShader( StatidSkyVS_ID ) ;
			customMesh = true ;
		}
		else
		{
			if( SSTSettings.bPlanarMapping )
			{
				float vConst[ 4 ] = {  SSTSettings.texScaleX, SSTSettings.texScaleY, SSTSettings.texOffsetX, SSTSettings.texOffsetY };
				D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 8, vConst, 1 ) );

				r3dRenderer->SetVertexShader( StatidSkyTexgVS_ID ) ;
			}
			else
			{
				r3dRenderer->SetVertexShader( StatidSkyVS_ID ) ;
			}
		}
	}
	else
	{
		r3dRenderer->SetCullMode( D3DCULL_NONE );
	}

	D3DXMATRIX rot;
	D3DXMatrixRotationY( &rot, SSTSettings.DomeRotationY / 180.0f * float( M_PI ) );

	if( customMesh )
	{
		D3DXMATRIX world;

		// this mesh is  short4 packed so we extend it to 30000.f using these numbers
		D3DXMatrixScaling( &world, SKY_DOME_RADIUS, SKY_DOME_RADIUS, SKY_DOME_RADIUS );

		world *= rot;

		r3dRenderer->SetCullMode( D3DCULL_CCW ) ;
		SetTransformMatrix( world, world * viewProj ) ;
	
		SSTSettings.mesh->DrawMeshSimple( 0 ) ;

		if( !hemisphere )
		{
			D3DXMatrixScaling( &world, SKY_DOME_RADIUS, -SKY_DOME_RADIUS, SKY_DOME_RADIUS ) ;

			world *= rot;

			r3dRenderer->SetCullMode( D3DCULL_CW ) ;

			SetTransformMatrix( world, world * viewProj ) ;
			SSTSettings.mesh->DrawMeshSimple( 0 ) ;

			r3dRenderer->RestoreCullMode();
		}
	}
	else
	{
		D3DXMATRIX world = rot;

		SetTransformMatrix( world, world * viewProj );

		r3dRenderer->SetCullMode( D3DCULL_CCW ) ;
		SkyDomeXPS->Draw();

		if( !hemisphere )
		{
			r3dRenderer->SetCullMode( D3DCULL_CW ) ;
			SkyDomeDown->Draw();
		}
	}

	for( int i = 0 ; i < 2 ; i ++ )
	{
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP ) );
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP ) );
	}

	r3dRenderer->RestoreCullMode() ;
}

void r3dSkyDome::Update(const r3dCamera &Cam)
{
	struct PerfEvent
	{
		PerfEvent()
		{
			D3DPERF_BeginEvent( 0, L"r3dSkyDome::Update" ) ;
		}

		~PerfEvent()
		{
			D3DPERF_EndEvent() ;
		}

	} perfEvent; (void)perfEvent ;

	r3dVector V = -Sun->SunLight.Direction;
	V.Normalize();

	static r3dVector OldV = -V;

	if( OldV != V || g_bSkyDomeNeedFullUpdate || updateAfterRestore )
	{
		DrawCubemap( Cam );
		OldV = V;
		updateAfterRestore = false;
	}	

	r3dRenderer->SetCamera( Cam, false );
	r3dRenderer->RestoreCullMode();

}

void r3dSkyDome::Draw( const r3dCamera &Cam, bool normals, bool hemisphere )
{
	R3DPROFILE_FUNCTION("r3dSkyDome::Draw");

	struct PerfEvent
	{
		PerfEvent()
		{
			D3DPERF_BeginEvent( 0, L"r3dSkyDome::Draw" ) ;
		}

		~PerfEvent()
		{
			D3DPERF_EndEvent() ;
		}
		
	} perfEvent; (void)perfEvent ;

	if(!bLoaded)
		return;

	r3dCamera 	Cam1;
	r3dPoint3D  Pos;

	memcpy ( &Cam1, Cam, sizeof ( r3dCamera));

	Pos.Assign(0, 0, 0); //-300
	Cam1.SetPosition(Pos);
	Cam1.SetPlanes     ( 1.0f, SKY_FAR_PLANE );

	r3dRenderer->SetCamera(Cam1, false);
	r3dRenderer->SetMaterial(NULL);

	// update
	m_fCloadAnim += 0.002f * m_fWindFactor * r3dGetFrameTime();
	if(m_fCloadAnim > 2.0f) 
		m_fCloadAnim = m_fCloadAnim - 2.0f;

	r3dRenderer->SetRenderingMode ( R3D_BLEND_NOALPHA | R3D_BLEND_ZC );

	D3DXMATRIX viewProj = r3dRenderer->ViewProjMatrix ;

	DWORD oldCLWE = 0;


	if (normals)
	{
		r3dRenderer->pd3ddev->GetRenderState(D3DRS_COLORWRITEENABLE, &oldCLWE);
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_ALPHA ) );
	}
	
	DrawDome( Cam, viewProj, 1.f, normals, hemisphere, true );

	if (normals)
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, oldCLWE ) );

	//r3dRenderer->pd3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	r3dRenderer->SetCamera(Cam,false);
	r3dRenderer->RestoreCullMode();
}

void r3dSkyDome::DrawCubemap( const r3dCamera& Cam )
{
	D3DPERF_BeginEvent( 0, L"r3dSkyDome::DrawCubemap" ) ;

	D3DXMATRIX ShaderMat;

	LPDIRECT3DDEVICE9 d = r3dRenderer->pd3ddev;

	r3dRenderer->SetTex( NULL, 7 );
	//avoid "Direct3D9: (WARN) :Can not render to a render target that is also used as a texture"

	LPDIRECT3DSURFACE9 dss;
	r3dRenderer->GetDSS(&dss);	
	r3dRenderer->SetDSS(0);

	LPDIRECT3DSURFACE9 targets[4];
	for(int i = 0; i < 4; ++i)
	{
		r3dRenderer->GetRT( i, &targets[i] );

		if( i )
		{
			r3dRenderer->SetRT(i, 0);
		}
	}

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ ) ;

	for(int i = D3DCUBEMAP_FACE_POSITIVE_X; i <= D3DCUBEMAP_FACE_NEGATIVE_Z; ++i)
	{			
		D3DXMATRIX viewProj = cubeMtx.view[i] * cubeMtx.projection;

		D3DXMATRIX ShaderMat ;

		D3DXMatrixTranspose( &ShaderMat, &viewProj );

		d->SetVertexShaderConstantF( 0, (float *)&ShaderMat,  4 );
		
		faceRTs[ i ]->Activate() ;
		d->Clear(0, NULL, D3DCLEAR_TARGET, 0xFFFFFFFF, 0.0f, 0);

		DrawDome( Cam, viewProj, 0.f, false, false, false ) ;

		faceRTs[ i ]->Deactivate() ;
	}

	RenderCubeMapMipChainWithEdgeSmoothing( cubemap, tempRt0, faceRTs );

	for(int i = 0; i < 4; ++i)
	{
		if(targets[i])	r3dRenderer->SetRT(i, targets[i]);
		if(targets[i])	targets[i]->Release();
	}	

	if(dss)
	{
		r3dRenderer->SetDSS(dss);
	}

	SAFE_RELEASE(dss);

	D3DPERF_EndEvent();
};

void SetD3DResourcePrivateData(LPDIRECT3DRESOURCE9 res, const char* FName);

void r3dSkyDome::D3DCreateResource()
{
	updateAfterRestore = true;	//redraw cubemap
}

void r3dSkyDome::D3DReleaseResource()
{
}


