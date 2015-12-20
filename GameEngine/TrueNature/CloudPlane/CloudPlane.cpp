#include "r3dPCH.h"
#include "r3d.h"
#include "CloudPlane.h"

CloudPlane::CloudPlane()
: initialized( 0 )
{

}

bool CloudPlane::IsActive() const
{
	return sceneParam.isActive ? true : false ;
}

void CloudPlane::Init()
{
	if( !initialized )
	{
		cloud.Create();
		SetDefaults();

		initialized = 1 ;
	}
}

bool CloudPlane::NeedSave() const
{
	return !storedSceneParam.isEqual(sceneParam);
}

void CloudPlane::Save(const char* folder)
{
	char fn[256];
	sprintf(fn, "%s%s", folder, "\\CloudPlane.bin");
	FILE* f = fopen(fn, "wb");
	fwrite(&sceneParam, sizeof(sceneParam), 1, f);
	fclose(f);

	extern bool g_bEditMode ;
	if( g_bEditMode )
	{
		sprintf(fn, "%s%s", folder, "\\ssclouds.bin");

		if( r3dFileExists(fn) )
		{
			remove( fn ) ;
		}
	}
}

template< int N >
static void FixOldTexturePath( char (&str)[ N ] )
{
	char temp[ N ] ;

	char oldName[] = "SSCLOUDS" ;

	if( char* substr = stristr( str, oldName ) )
	{
		memcpy( temp, str, substr - str ) ;
		temp[ substr - str ] = 0 ;
		strcat( temp, "CloudPlane" ) ;
		strcat( temp, str + (substr - str) + sizeof( oldName ) - 1 ) ;

		strcpy( str, temp ) ;
	}
}

void CloudPlane::Load(const char* folder)
{

	char fn[256];
	sprintf(fn, "%s%s", folder, "\\CloudPlane.bin");

	if(!r3dFileExists(fn))
	{
		sprintf(fn, "%s%s", folder, "\\ssclouds.bin");
	}

	if(r3dFileExists(fn))
	{
		r3dFile* f = r3d_open(fn, "rb");
		fread(&sceneParam, sizeof(sceneParam), 1, f);
		fclose(f);
	}

	FixOldTexturePath( sceneParam.textureFileName ) ;
	FixOldTexturePath( sceneParam.textureCoverFileName ) ;

	storedSceneParam = sceneParam;
}

void CloudPlane::SetDefaults()
{
	// Setup the camera's view parameters
	D3DXVECTOR3 vecEye;
	D3DXVECTOR3 vecAt( 14000.0f, 0.0f, 12730.8f );
	vecEye = vecAt + D3DXVECTOR3( -10.0f, 5.0f, -10.0f );

	sceneParam.eyePt = vecEye;

	// light
	sceneParam.m_vLightDir = D3DXVECTOR3( 0.0f, -1.0f, 0.0f );
	sceneParam.m_vLightColor = D3DXVECTOR3( 1.2f, 1.2f, 1.2f );
	sceneParam.m_vAmbientLight = D3DXVECTOR3( 0.3f, 0.35f, 0.4f );
	// scattering
	sceneParam.m_vRayleigh = D3DXVECTOR3( 0.3f, 0.45f, 1.0f );
	sceneParam.m_vMie = D3DXVECTOR3( 0.3f, 0.3f, 0.3f );
	sceneParam.m_fG = 0.7f;
	sceneParam.m_fLightScale = 8.0f;
	sceneParam.m_fAmbientScale = 0.1f;
	// sky scattering 
	sceneParam.m_fEarthRadius = 21600000.0f;
	sceneParam.m_fAtomosHeight = 30000.0f;
	sceneParam.m_fCloudHeight = 600.0f;		

	sceneParam.m_fCoverage = cloud.GetCurrentCloudCover();
	sceneParam.m_fUVScale = 5.0f;
	sceneParam.m_fWindVel = 1.0f;
	sceneParam.m_vFade_Density = D3DXVECTOR4(150.0f, 1500.0f, 1.0f, 0.0f);
	sceneParam.useEngineColors = 1;
	sceneParam.isActive = 1;
	r3dscpy(sceneParam.textureFileName, "Data\\CloudPlane\\cloud.dds");
	r3dscpy(sceneParam.textureCoverFileName, "Data\\Shaders\\Texture\\White.dds");

	storedSceneParam = sceneParam;
}

void CloudPlane::Clear()
{
	cloud.Delete();
	initialized = 0 ;
}

void CloudPlane::Render(const D3DXVECTOR3& cameraPos, const D3DXVECTOR3& lightDir, float dt, const D3DXMATRIX& viewProj, float farPlane, r3dTexture* cloudTex, r3dTexture* cloudCoverTex)
{

	D3DPERF_BeginEvent( 0, L"CloudPlane::Render" );

	sceneParam.eyePt = cameraPos; 
	sceneParam.viewProj = viewProj;
	sceneParam.farPlane = farPlane;
	sceneParam.m_vLightDir = lightDir;
	//sceneParam.SetTime(dayTime);

	cloud.SetCloudCover(sceneParam.m_fCoverage);

	SBoundingBox bbox;
	D3DXVECTOR3 c(0,0,0);
	bbox.Centroid(&c);
	cloud.Update( dt, bbox, sceneParam );


	LPDIRECT3DDEVICE9 pDev = r3dRenderer->pd3ddev;

 	DWORD us[8], vs[8], mag[8], min[8], mip[8];
 	// Save all sampler
 	for (unsigned int i = 0; i < 8; ++i) 
 	{
 		pDev->GetSamplerState( i, D3DSAMP_MAGFILTER, &mag[i] );
 		pDev->GetSamplerState( i, D3DSAMP_MINFILTER, &min[i] );
 		pDev->GetSamplerState( i, D3DSAMP_MIPFILTER, &mip[i] );
 		pDev->GetSamplerState(i, D3DSAMP_ADDRESSU, &us[i]);
 		pDev->GetSamplerState(i, D3DSAMP_ADDRESSV, &vs[i]);
 	}

	LPDIRECT3DSURFACE9 dss;
	r3dRenderer->GetDSS(&dss);
	r3dRenderer->SetDSS( 0 );

	LPDIRECT3DSURFACE9 targets[4] = {0};
 	for(int i = 0; i < 4; ++i)
 	{
		if(i==2)	continue; // no RT2
 		r3dRenderer->GetRT( i, &targets[i] );

		if( i )	r3dRenderer->SetRT(i, 0);
 	}
	
	// Clear all sampler
	for (unsigned int i = 0; i < 3; ++i) 
	{
		r3dRenderer->SetTex(0, i);
		pDev->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		pDev->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
		pDev->SetSamplerState( i, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	}

	r3dRenderer->SetTex(cloudCoverTex, 2);
		
	// Render shadowmap, density and blur
	cloud.PrepareCloudTextures(cloudTex, sceneParam);

	// Pass 3 : Draw scene

	// Draw clouds 
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	if(dss)
	{
		r3dRenderer->SetDSS(dss);
		SAFE_RELEASE(dss);
	}	

	pDev->SetRenderState( D3DRS_ZENABLE, TRUE );

	cloud.DrawFinalQuad(sceneParam);

	pDev->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE);
	pDev->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );

 	// Save all sampler
 	for (unsigned int i = 0; i < 3; ++i) 
 	{
 		pDev->SetSamplerState( i, D3DSAMP_MAGFILTER, mag[i] );
 		pDev->SetSamplerState( i, D3DSAMP_MINFILTER, min[i] );
 		pDev->SetSamplerState( i, D3DSAMP_MIPFILTER, mip[i] );
 		pDev->SetSamplerState(i, D3DSAMP_ADDRESSU, us[i]);
 		pDev->SetSamplerState(i, D3DSAMP_ADDRESSV, vs[i]);
 	}

	for(int i = 0; i < 4; ++i)
	{
		if(targets[i])	r3dRenderer->SetRT(i, targets[i]);
		if(targets[i])	targets[i]->Release();
	}

	r3dRenderer->ResetTextureCache();

	D3DPERF_EndEvent();
}
