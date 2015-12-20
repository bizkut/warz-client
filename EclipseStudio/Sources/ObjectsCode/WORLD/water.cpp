#include "r3dpch.h"
#include "r3d.h"
#include "GameLevel.h"

#ifndef WO_SERVER
#include "UI/UIimEdit.h"
#endif // WO_SERVER

#include "GameCommon.h"
#ifndef WO_SERVER
#include "GameObjects\gameobj.h"
#include "GameObjects\objmanag.h"
#endif // WO_SERVER

#include "water.h"

#ifndef WO_SERVER
void waterReloadTexture( r3dTexture*& tex ) ;

float RIPPLES_SIMULATION_FPS = 20.0f;	//for texsize=1024 and area size=30
float RIPPLES_TEXTURE_SIZE = 0.0f;	//
float RIPPLES_AREA_SIZE = 30.0f;	//in metres
float RIPPLES_VISCOSITY = 0.99f;

r3dScreenBuffer *gWaterRefractionBuffer = 0;
#endif // WO_SERVER

/*static*/ r3dTL::TArray< WaterBase* > WaterBase::AllWaters ;
#ifndef WO_SERVER
/*static*/ void WaterBase::UpdateAllRipples()
{
	for( int i = 0, e = (int)AllWaters.Count() ; i < e ; i ++ )
	{
		AllWaters[ i ]->UpdateRipples() ;
	}
}

/*static*/ IDirect3DVertexDeclaration9* WaterBase::pScreenDecl = NULL;
#endif // WO_SERVER

WaterBase::WaterBase()
#ifndef WO_SERVER
	: shallowColor(0, 200, 0)
	, deepColor(3, 31, 50)
	, attenColor(127, 100, 80)
	, shallowDepth(5.0f)
	, attenDist(10.0f)
	, bumpness(40.0f)
	, refractionIndex(3.0f)
	, tileSize(0.05f)
	, specularCosinePower(400.0f)
	, specularTiling(5.0f)
	, causticDepth(10.0f)
	, causticStrength(0.5f)
	, causticTiling(0.05f)
	, curQualitySettings(0)

	, waterColorTile(0.01f)
	, waterColorBlend(0.5f)
	, refractionPerturbation(0.1f)
	, foamTile(0.01f)
	, foamPerturbation(0.1f)
	, foamSpeed(0.1f)
	, foamIntensity(0.01f)

	, surfTile(0.1f)
	, surfPeriod(5.0f)
	, surfWidth(1.0f)
	, surfRange(0.0005f)
	, surfPerturbation(0.02f)
	, surfIntensity(0.0f)
	, surf2Coast(5.0f)
	
	, FoamTexture(0)
	, SpecularTexture(0)
	, ColorTexture(0)
	, MaskTexture(0)
	, showFoam(0)
	, showSurf(0)

	, uRefractionIndex(3.0f)
	, uRefractionPerturbation(0.1f)
	, uAttenDist(10.0f)
	, uShallowColor(0, 200, 0)
	, uDeepColor(3, 31, 50)
	, uAttenColor(127, 100, 80)
	, HeightTexture(0)
	, RipplesTex(0)

	, PlaneOffsetX( 0 )
	, PlaneOffsetZ( 0 )

	, specBumpiness( 1.0f )
	, specIntensity( 1.0f )

	, reflectionIntensity( 1.0f )
	, fresnelPow( 4.f )

	, farTileScale( 1.0f )
	, farTileFadeStart( 32.f )
	, farTileFadeEnd( 64.f )
	, farTileAmmount( 1.0f )
	, farTileBumpiness( 1.0f )
	, fresnelBumpiness( 1.0f )

	, lastWaterLevel( 0.f )
	, isRippled( false )
#endif // WO_SERVER
{
	AllWaters.PushBack( this ) ;
#ifndef WO_SERVER
	rainRipplesRT[0] = rainRipplesRT[1] = 0;
	ripplesRT[0] = ripplesRT[1] = 0;
	curRipplesRT=-1;
	nSplashes = 0;
	lastSplashTime = r3dGetTime();
#endif // WO_SERVER
}

WaterBase::~WaterBase()
{
	bool erased = false ;

	for( int i = 0, e = (int)AllWaters.Count() ; i < e ; i ++ )
	{
		if( AllWaters[ i ] == this )
		{
			erased = true ;
			AllWaters.Erase( i ) ;
			break ;
		}
	}

	r3d_assert( erased ) ;
}


#include "waterplane.h"
#include "lake.h"
#ifndef WO_SERVER
bool TraceWater(const r3dPoint3D& start, const r3dPoint3D& finish, r3dPoint3D& waterSplashPos)
{
	//get waterplane that is under/over the camera
	for(uint32_t i = 0; i < obj_WaterPlane::m_dWaterPlanes.Count (); i++ )
	{
		obj_WaterPlane * pP = obj_WaterPlane::m_dWaterPlanes[i];

		const obj_WaterPlane::Settings& sts = pP->GetSettings();

		float h = sts.WaterPlaneHeight;
		if( (start.y - h) * (h - finish.y) < 0)	continue;

		waterSplashPos = start + (start.y-h) * (finish-start) / (start.y-finish.y);
		r3dBoundBox const & bBox = pP->GetBBoxWorld();
		if(waterSplashPos.X >= bBox.Org.X && waterSplashPos.X <= bBox.Org.X + bBox.Size.X &&
			 waterSplashPos.Z >= bBox.Org.Z && waterSplashPos.Z <= bBox.Org.Z + bBox.Size.Z)
		{
			//check for grid's cell where the camera is presents
			const obj_WaterPlane::Info& info = pP->GetInfo();

			int i, j;

			pP->GetCellIndexes( waterSplashPos, &i, &j );

			if( i < 0 || i >= info.CellCountX )
				continue;

			if( j < 0 || j >= info.CellCountZ )
				continue;

			if(pP->GetWaterGridValue(i,j)!=0)	return true;
		}
	}

	extern float LakePlaneY;
	if(objOceanPlane && (start.y - LakePlaneY) * (LakePlaneY - finish.y) > 0)
	{
		waterSplashPos = start + (start.y-LakePlaneY) * (finish-start) / (start.y-finish.y);
		return true;
	}

	return false;
}

void WaterSplash(const r3dPoint3D& waterSplashPos, float height, float size, float amount, int texIdx)
{
	for(uint32_t i = 0; i < obj_WaterPlane::m_dWaterPlanes.Count (); i++ )
	{
		obj_WaterPlane * pP = obj_WaterPlane::m_dWaterPlanes[i];
		pP->WaterSplash(waterSplashPos, height, size, amount, texIdx);
	}
}
#endif // WO_SERVER

float getWaterDepthAtPos(const r3dPoint3D& pos)
{
	float depth = 100; 

	for(uint32_t i = 0; i < obj_WaterPlane::m_dWaterPlanes.Count (); i++ )
	{
		obj_WaterPlane * pP = obj_WaterPlane::m_dWaterPlanes[i];

		const obj_WaterPlane::Settings& sts = pP->GetSettings();

		float h = sts.WaterPlaneHeight;
		if( pos.y - h > 0)	continue; // above water

		r3dBoundBox const & bBox = pP->GetBBoxWorld();
		if(pos.X >= bBox.Org.X && pos.X <= bBox.Org.X + bBox.Size.X &&
			pos.Z >= bBox.Org.Z && pos.Z <= bBox.Org.Z + bBox.Size.Z)
		{
			//check for grid's cell where the camera is presents
			const obj_WaterPlane::Info& info = pP->GetInfo();

			int i, j;

			pP->GetCellIndexes( pos, &i, &j );

			if( i < 0 || i >= info.CellCountX )
				continue;

			if( j < 0 || j >= info.CellCountZ )
				continue;

			if(pP->GetWaterGridValue(i,j)!=0)	
				depth = pos.y - h;
		}
	}

	extern float LakePlaneY;
	if(objOceanPlane && (pos.y - LakePlaneY) < 0)
	{
		if((pos.y - LakePlaneY)<depth)
			depth = pos.y - LakePlaneY;
	}

	return -depth;
}

#ifndef WO_SERVER
bool WaterBase::Ripples(D3DXVECTOR4& camd)
{
	R3DPROFILE_FUNCTION("WaterBase::Ripples");
	//xz - ripples grid movement, yw - fractional UV offset
	static D3DXVECTOR4 prevCam(0.0f, 0.0f, 0.0f, 0.0f);
	camd = D3DXVECTOR4(gCam.x, 0.0f, gCam.z, 0.0f) - prevCam;

	if(RIPPLES_TEXTURE_SIZE==0.0f)	return false;

	static float prevTime = -1.0f;
	if(prevTime<0.0f)	prevTime = r3dGetTime();	//first init
	float dt = r3dGetTime() - prevTime;
	//FPS of simulation depend of size of texture and area size
	const float timestep = 1.0f/(RIPPLES_SIMULATION_FPS * RIPPLES_TEXTURE_SIZE / 512.0f / (RIPPLES_AREA_SIZE/30.0f));
	int nIterations = int(dt/timestep);
	if(curRipplesRT==-1)	nIterations = 1;	//first draw

	if(nSplashes>0)	lastSplashTime = r3dGetTime();
	else
		if(lastSplashTime+30.0f < r3dGetTime())	return false;	//skip if no impacts

	if(nIterations==0)
	{
		camd.y = camd.x;
		camd.w = camd.z;
		return true;
	}
	camd.y = fmod(camd.x, 1.0f/RIPPLES_TEXTURE_SIZE*RIPPLES_AREA_SIZE);
	camd.w = fmod(camd.z, 1.0f/RIPPLES_TEXTURE_SIZE*RIPPLES_AREA_SIZE);
	camd.x -= camd.y;
	camd.z -= camd.w;
	prevCam += camd;
	prevTime += timestep*float(nIterations);

	if(nIterations>5)	nIterations = 5;

	// preserve current render target 
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
	r3dRenderer->SetDSS(0);

	LPDIRECT3DSURFACE9 targets[4] = {0};
 	for(int i = 0; i < 4; ++i)
 	{
		if(i==2)	continue; // no RT2
 		r3dRenderer->GetRT( i, &targets[i] );

		if( i )	r3dRenderer->SetRT(i, 0);
 	}

	bool needClear = false ;
	
	if( curRipplesRT == -1 )
	{
		needClear = true ;
		curRipplesRT = 0 ;
	}

	r3dRenderer->SetCullMode(D3DCULL_NONE);
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	d3dc._SetDecl(pScreenDecl);
	r3dRenderer->SetVertexShader("RIPPLES_VS");
	D3DXVECTOR3 sq[] = {
		D3DXVECTOR3(-0.5f, -0.5f, 0.0f),
		D3DXVECTOR3( 0.5f, -0.5f, 0.0f),
		D3DXVECTOR3( 0.5f,  0.5f, 0.0f),
		D3DXVECTOR3(-0.5f,  0.5f, 0.0f)
	};

	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

	/*r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	r3dRenderer->SetTex(HeightTexture, 1);*/


	D3DXVECTOR4 params(	1.0f/RIPPLES_TEXTURE_SIZE,	1.0f/RIPPLES_AREA_SIZE,	RIPPLES_VISCOSITY,	0.0f);
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(  6, &params.x,  1 );

	for(int i=0; i<nIterations; i++)
	{
		int prevRT = curRipplesRT;
		curRipplesRT++;
		if(curRipplesRT==2)	curRipplesRT = 0;

		if( needClear )
		{
			ripplesRT[prevRT]->Activate();
			r3dRenderer->pd3ddev->Clear( 0, NULL, D3DCLEAR_TARGET, 0x80808080, r3dRenderer->GetClearZValue(), 0 );
			ripplesRT[prevRT]->Deactivate();
		}

		if(1)	//water riples global
		{
			ripplesRT[curRipplesRT]->Activate();

			r3dRenderer->SetTex(ripplesRT[prevRT]->Tex, 0);
			r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
			r3dRenderer->SetTex(RipplesTex, 1);
			r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
			r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

			if(r_environment_quality->GetInt()==3)
				if(nSplashes>0)	r3dRenderer->SetPixelShader("RIPPLESR_PS");
				else		r3dRenderer->SetPixelShader("RIPPLES_PS");
			else
				if(nSplashes>0)	r3dRenderer->SetPixelShader("RIPPLESR_N_PS");
				else	r3dRenderer->SetPixelShader("RIPPLES_N_PS");

			r3dRenderer->pd3ddev->SetPixelShaderConstantF(  5, &camd.x,  1 );
			camd.x = camd.z = 0.0f;

			D3DXVECTOR4 splash(0.0f, 0.0f, 0.0f, 0.0f);
			for(int s=nSplashes; s<MaxSplashes; s++)
				r3dRenderer->pd3ddev->SetPixelShaderConstantF(  s, &splash.x,  1 );
			for(; nSplashes>0; nSplashes--)
			{
				splash = splashes[nSplashes-1].params;
				splash.z = floorf(100000.0f*splash.z/RIPPLES_TEXTURE_SIZE/512.0f * RIPPLES_AREA_SIZE) + float(splashes[nSplashes-1].texIdx+1)*0.2f;
				splash.w *= RIPPLES_TEXTURE_SIZE/512.0f;
				r3dRenderer->pd3ddev->SetPixelShaderConstantF(  nSplashes-1, &splash.x,  1 );
			}


			r3dRenderer->DrawUP(D3DPT_TRIANGLEFAN, 2, sq, sizeof D3DXVECTOR3);
			ripplesRT[curRipplesRT]->Deactivate();
		}


#if 0
		if(0)	//rain
		{
			rainRipplesRT[curRipplesRT]->Activate();
			if(needs2clear)	r3dRenderer->pd3ddev->Clear( 0, NULL, D3DCLEAR_TARGET, 0, r3dRenderer->GetClearZValue(), 0 );

			bool drops = GetAsyncKeyState('1')<0;
			for(int n=0; n<4; n++)
			{
				D3DXVECTOR4 drop(0.0f,0.0f, 0.0f, 0.0f);
				if(drops)
				{
					drop.x = float(rand())/RAND_MAX;
					drop.y = float(rand())/RAND_MAX;
					drop.z = 0.05f*float(rand())/RAND_MAX;
				}
				r3dRenderer->pd3ddev->SetPixelShaderConstantF(  n, &drop.x,  1 );
			}

			r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
			r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
			r3dRenderer->SetTex(rainRipplesRT[prevRT]->Tex, 0);
			r3dRenderer->SetPixelShader("RAINRIPPLES_PS");

			r3dRenderer->DrawUP(D3DPT_TRIANGLEFAN, 2, sq, sizeof D3DXVECTOR3);
			rainRipplesRT[curRipplesRT]->Deactivate();
		}
#endif

	}

	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_MAXANISOTROPY, 16 );

	if(dss)
	{
		r3dRenderer->SetDSS(dss);
		SAFE_RELEASE(dss);
	}	

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
	return true;
}


void WaterBase::RenderBegin(const r3dCamera& Cam, float waterLevel)
{
	R3DPROFILE_FUNCTION("WaterBase::RenderBegin");

	lastWaterLevel = waterLevel ;

	float4 envLightColor = GetEnvLightColor();
	float4 envAmbientColor = GetEnvLightAmbient();

	float ic_r = 1.0f / 255.0f * R3D_MIN( envLightColor.x + envAmbientColor.x, 1.0f );
	float ic_g = 1.0f / 255.0f * R3D_MIN( envLightColor.y + envAmbientColor.y, 1.0f );
	float ic_b = 1.0f / 255.0f * R3D_MIN( envLightColor.z + envAmbientColor.z, 1.0f );

	float ic = 1.0f / 255.f;

	r3dRenderer->SetMaterial(NULL);

	r3dRenderer->SetCullMode( D3DCULL_NONE );

	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_ZC); 

	float _refractionIndex,_refractionPerturbation, _attenDist;
	r3dColor _shallowColor, _deepColor, _attenColor;

	// WATER[U - underwater][F - follow terrain]_[H-high, N-normal, L-low][P-pixel, V-vertex]
	char sVS[16];
	strcpy(sVS, "WATER");

	if(r3dRenderer->CameraPosition.y <= waterLevel)
	{
		_refractionIndex = uRefractionIndex;
		_refractionPerturbation = uRefractionPerturbation;
		_attenDist = uAttenDist;
		_shallowColor = uShallowColor;
		_deepColor = uDeepColor;
		_attenColor = uAttenColor;

		strcat(sVS, "U");
	}
	else
	{
		_refractionIndex = refractionIndex;
		_refractionPerturbation = refractionPerturbation;
		_attenDist = attenDist;
		_shallowColor = shallowColor;
		_deepColor = deepColor;
		_attenColor = attenColor;
	}
	if(isRippled)	strcat(sVS, "R");

	char* q[] = { "_L", "_N", "_H" };
	strcat(sVS, q[r_environment_quality->GetInt()-1]);

	char sPS[16];
	strcpy(sPS, sVS);
	strcat(sPS, "P");
	strcat(sVS, "V");
	r3dRenderer->SetVertexShader(sVS);
	r3dRenderer->SetPixelShader(sPS);

	D3DXMATRIX RefractViewProjTexXf = r3dRenderer->ViewMatrix;
	RefractViewProjTexXf._41 = 0.0f;
	RefractViewProjTexXf._42 = 0.0f;
	RefractViewProjTexXf._43 = 0.0f;
	RefractViewProjTexXf *= r3dRenderer->ProjMatrix;
	{
		D3DXMATRIX texCoordTM; D3DXMatrixIdentity(&texCoordTM);
		texCoordTM._11 = +0.5f; texCoordTM._41 = 0.5f;
		texCoordTM._22 = -0.5f; texCoordTM._42 = 0.5f;
		RefractViewProjTexXf *= texCoordTM;
	}
	D3DXMatrixTranspose(&RefractViewProjTexXf, &RefractViewProjTexXf);

	float fBumpness_RefrIndex_TileSize[] = { 50 - bumpness, (_refractionIndex-2.0f)/20.0f, tileSize, 10.0f*tileSize };

	r3dPoint3D envLightDir = GetEnvLightDir();
	D3DXVECTOR4 vSun (-envLightDir.x, -envLightDir.y, -envLightDir.z, r3dGetTime());

	//---------------------------------------
	//VERTEX shader CONSTS
	D3DXMATRIX 	mWorld;
	D3DXMatrixIdentity(&mWorld);
	D3DXMatrixTranslation(&mWorld, PlaneOffsetX, waterLevel, PlaneOffsetZ);
	D3DXMATRIX ShaderMat;
	ShaderMat =  mWorld * 	r3dRenderer->ViewProjMatrix ;
	D3DXMatrixTranspose( &ShaderMat, &ShaderMat );
	r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float *)&ShaderMat,  4 );
	D3DXVECTOR4 vWaveParam( specularTiling, 0, waterLevel, 0);
	r3dRenderer->pd3ddev->SetVertexShaderConstantF(  5, (float *)&vWaveParam,	1 );


	D3DXVECTOR4 vsCamReg (r3dRenderer->CameraPosition.x, r3dRenderer->CameraPosition.y, r3dRenderer->CameraPosition.z, 0.f );
	r3dRenderer->pd3ddev->SetVertexShaderConstantF(  8, (float *)&vsCamReg,	1 );
	r3dRenderer->pd3ddev->SetVertexShaderConstantF(  22, fBumpness_RefrIndex_TileSize,  1 );

	if( r_environment_quality->GetInt() == 1 )
	{
		r3dRenderer->pd3ddev->SetVertexShaderConstantF(  23, (float *)&RefractViewProjTexXf,	4 );
	}
	else
	{
		r3dRenderer->pd3ddev->SetVertexShaderConstantF(  23, (float *)&vSun,	1 );
	}

	//---------------------------------------
	//PIXEL SHADER CONSTS

	r3dTL::TFixedArray< D3DXVECTOR4, 27 > psConsts ;

	D3DXMATRIX im = r3dRenderer->InvViewMatrix;

	//------------------------------------------------------------------------
	// float4x3    invViewMtx                              : register(  c0 );
	D3DXMatrixTranspose( (D3DXMATRIX*)( psConsts + 0 ), &im );

	//------------------------------------------------------------------------
	// float4x4    RefractViewProjTexXf                    : register(  c3 );
	*(D3DXMATRIX*)( psConsts + 3 ) = RefractViewProjTexXf ;

	//------------------------------------------------------------------------
	// float4      vCamera_Lerp                            : register(  c7 );
	psConsts [ 7 ] = D3DXVECTOR4 (r3dRenderer->CameraPosition.x, r3dRenderer->CameraPosition.y, r3dRenderer->CameraPosition.z, SetNormalTextures(0,1,10.f) ) ;

	//------------------------------------------------------------------------
	// float4      prevCam_mipA                            : register(  c8 );
	static D3DXVECTOR4 prevCam;
	D3DXVECTOR4 curCam(r3dRenderer->CameraPosition.x, r3dRenderer->CameraPosition.y, r3dRenderer->CameraPosition.z, 0.0f);
	D3DXVECTOR4 dc = curCam - prevCam;
	prevCam.w = std::min(2.0f, std::max( 1.0f + sqrtf(dc.x*dc.x + dc.z*dc.z)*2.0f, 1.0f + fabsf(dc.y)*30.0f ));
	
	psConsts [ 8 ] = prevCam ;

	prevCam = curCam;

	//------------------------------------------------------------------------
	// float4      projVal                                 : register(  c9 );

	// need to calculate projection coefficients manually, because they may be for inversed z range

	float zn = r3dRenderer->NearClip;
	float zf = r3dRenderer->FarClip;

	float p33 = zf/(zf-zn);
	float p34 = -zn*zf/(zf-zn);

	psConsts [ 9 ] = D3DXVECTOR4 (p33, p34, 2.0f / r3dRenderer->ProjMatrix.m[0][0], -2.0f / r3dRenderer->ProjMatrix.m[1][1]) ;

	//------------------------------------------------------------------------
	// float4      SunVector_Time                          : register( c10 );
	psConsts [ 10 ] = vSun ;

	//------------------------------------------------------------------------
	// float4      deepColor_SpecularCosinePower           : register( c11 );

	D3DXVECTOR4 fDeepColor_SpecularCosinePower = D3DXVECTOR4 (_deepColor.R * ic_r, _deepColor.G * ic_g, _deepColor.B * ic_b, specularCosinePower);
	fDeepColor_SpecularCosinePower.x = powf(fDeepColor_SpecularCosinePower.x, 2.2f);
	fDeepColor_SpecularCosinePower.y = powf(fDeepColor_SpecularCosinePower.y, 2.2f);
	fDeepColor_SpecularCosinePower.z = powf(fDeepColor_SpecularCosinePower.z, 2.2f);
	psConsts [ 11 ] = fDeepColor_SpecularCosinePower ;

	//------------------------------------------------------------------------
	// float4      attenColor_Dist                         : register( c12 );

	D3DXVECTOR4 fAttenColor_Dist = D3DXVECTOR4 (_attenColor.R * ic, _attenColor.G * ic, _attenColor.B * ic, _attenDist );
	fAttenColor_Dist.x = powf(fAttenColor_Dist.x, 2.2f);
	fAttenColor_Dist.y = powf(fAttenColor_Dist.y, 2.2f);
	fAttenColor_Dist.z = powf(fAttenColor_Dist.z, 2.2f);
	if( int(_attenColor.R) + _attenColor.G + _attenColor.B == 0)	fAttenColor_Dist.x = fAttenColor_Dist.y = fAttenColor_Dist.z = 0.5f;

	psConsts [ 12 ] = fAttenColor_Dist ;

	//------------------------------------------------------------------------
	// float4      bumpness_RefrIndex_TileSize_x10         : register( c13 );

	psConsts [ 13 ] = fBumpness_RefrIndex_TileSize ;

	//------------------------------------------------------------------------
	// float4      CausticDepth_Strength_projVal           : register( c14 );

	psConsts [ 14 ] = D3DXVECTOR4 (1.0f / causticDepth, 4.0f * causticStrength, -1.0f / r3dRenderer->ProjMatrix.m[0][0], 1.0f / r3dRenderer->ProjMatrix.m[1][1]) ;

	//------------------------------------------------------------------------
	// float4      beamAttColor                            : register( c15 );

	float ltMx = std::max(fAttenColor_Dist.x, std::max(fAttenColor_Dist.y, fAttenColor_Dist.z));
	ltMx = 0.5f / ltMx;
	fAttenColor_Dist.x = fAttenColor_Dist.x * ltMx;
	fAttenColor_Dist.y = fAttenColor_Dist.y * ltMx;
	fAttenColor_Dist.z = fAttenColor_Dist.z * ltMx;

	D3DXVECTOR4 beamAttColor;
	beamAttColor.x = logf( powf(fabs(fAttenColor_Dist.x), 4.0f / fAttenColor_Dist.w) );
	beamAttColor.y = logf( powf(fabs(fAttenColor_Dist.y), 4.0f / fAttenColor_Dist.w) );
	beamAttColor.z = logf( powf(fabs(fAttenColor_Dist.z), 4.0f / fAttenColor_Dist.w) );
	beamAttColor.w = causticTiling;

	psConsts [ 15 ] = beamAttColor ;

	//------------------------------------------------------------------------
	// float3      diffAttColor                            : register( c16 );

	D3DXVECTOR4 diffAttColor;

	D3DXVECTOR4 fShallowColor_Depth = D3DXVECTOR4 ( _shallowColor.R * ic, _shallowColor.G * ic, _shallowColor.B * ic, 1.0f );

	fShallowColor_Depth.x = powf(fShallowColor_Depth.x, 2.2f);
	fShallowColor_Depth.y = powf(fShallowColor_Depth.y, 2.2f);
	fShallowColor_Depth.z = powf(fShallowColor_Depth.z, 2.2f);

	diffAttColor.x = log( std::min(1.0f, std::max(0.0f, (1.0f - std::min(1.0f, std::max(0.0f, fShallowColor_Depth.x / 4.4f / fDeepColor_SpecularCosinePower.x)) / exp(-fShallowColor_Depth.w * beamAttColor.x)))));
	diffAttColor.y = log( std::min(1.0f, std::max(0.0f, (1.0f - std::min(1.0f, std::max(0.0f, fShallowColor_Depth.y / 4.4f / fDeepColor_SpecularCosinePower.y)) / exp(-fShallowColor_Depth.w * beamAttColor.y)))));
	diffAttColor.z = log( std::min(1.0f, std::max(0.0f, (1.0f - std::min(1.0f, std::max(0.0f, fShallowColor_Depth.z / 4.4f / fDeepColor_SpecularCosinePower.z)) / exp(-fShallowColor_Depth.w * beamAttColor.z)))));

	psConsts [ 16 ] = diffAttColor ;

	//------------------------------------------------------------------------
	// float4      vars                                    : register( c17 );

	psConsts [ 17 ] = D3DXVECTOR4 ( (fBumpness_RefrIndex_TileSize[1]-1.0f)/2.0f, 1.0f - fBumpness_RefrIndex_TileSize[1]/9.0f, 10.0f*fBumpness_RefrIndex_TileSize[2], fBumpness_RefrIndex_TileSize[1]/9.0f);

	//------------------------------------------------------------------------
	// float4      ColTileBlend_Pert_Coast                 : register( c18 );

	psConsts[ 18 ] = D3DXVECTOR4( waterColorTile, waterColorBlend, _refractionPerturbation, 4.0f/shallowDepth );

	//------------------------------------------------------------------------
	// float4      SunColor_refrIdx2                       : register( c19 );

	D3DXVECTOR4 vv = D3DXVECTOR4(envLightColor.x, envLightColor.y, envLightColor.z, 0.0f)*0.5f;
	vv.w = 1.0f - fBumpness_RefrIndex_TileSize[1];
	psConsts[ 19 ] = vv ;

	//------------------------------------------------------------------------
	// float4      addFactor                               : register( c20 );

	D3DXVECTOR4 addFactor( 0, 0, 0, 0 );

	if(r3dRenderer->CameraPosition.y <= waterLevel)
	{
		float dc = waterLevel - r3dRenderer->CameraPosition.y + 0.0001f;
		
		addFactor.x = powf(2.0f, dc * diffAttColor.x);
		addFactor.y = powf(2.0f, dc * diffAttColor.y);
		addFactor.z = powf(2.0f, dc * diffAttColor.z);
		addFactor.w = powf(2.0f, -dc/uAttenDist*3.0f);
	}

	psConsts[ 20 ] = addFactor ;

	//------------------------------------------------------------------------
	// float4      rippleOffset                            : register( c21 );

	psConsts[ 21 ] = rippleCamd ;

	//------------------------------------------------------------------------
	// float4      simParams                               : register( c22 );

	psConsts[ 22 ] = D3DXVECTOR4( 1.0f/RIPPLES_TEXTURE_SIZE,	1.0f/RIPPLES_AREA_SIZE,	RIPPLES_VISCOSITY,	0.0f );

	//------------------------------------------------------------------------
	// float4      specIntensity_Bumpiness                 : register( c23 );

	float SunR = envLightColor.x * specIntensity * 0.5f;
	float SunG = envLightColor.y * specIntensity * 0.5f;
	float SunB = envLightColor.z * specIntensity * 0.5f;

	psConsts[ 23 ] = D3DXVECTOR4 ( SunR, SunG, SunB, 1.0f / specBumpiness ) ;

	//------------------------------------------------------------------------
	// float4      reflIntensity_fresnelPow                : register( c24 );

	psConsts[ 24 ] = D3DXVECTOR4 ( reflectionIntensity, reflectionIntensity, reflectionIntensity, fresnelPow ) ;

	//------------------------------------------------------------------------
	// float4      farTileControl                          : register( c25 );

	float farTileCoef = 1.0f / ( farTileFadeEnd - farTileFadeStart ) ;
	psConsts[ 25 ] = D3DXVECTOR4 ( 1.0f / farTileScale, farTileCoef, - farTileFadeStart * farTileCoef, farTileAmmount ) ;

	//------------------------------------------------------------------------
	// float4      farTileBumpiness_Lerp_FresBumpiness     : register( c26 );

	psConsts[ 26 ] = D3DXVECTOR4 ( 1.0f / farTileBumpiness, SetNormalTextures(10,11,10.f / farTileScale ), 1.0f / fresnelBumpiness, 0.f ) ;


	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, &psConsts[0].x, psConsts.COUNT ) ) ;
	
	//-----------------------------------------------------
	// TEXTURES
	r3dRenderer->SetTex(SkyDome->cubemap->Tex, 7);

	r3dRenderer->SetTex(DepthBuffer->Tex, 2);
	r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
	
	r3dRenderer->SetTex(gWaterRefractionBuffer->Tex, 3);
	r3dRenderer->pd3ddev->SetSamplerState( 3, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 3, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	r3dRenderer->pd3ddev->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	r3dRenderer->pd3ddev->SetSamplerState( 3, D3DSAMP_MIPFILTER, D3DTEXF_NONE );

	r3dRenderer->SetTex(TempShadowBuffer->Tex,5);

	r3dRenderer->SetTex(ColorTexture, 6);
	r3dRenderer->pd3ddev->SetSamplerState( 6, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( 6, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( 6, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	r3dRenderer->pd3ddev->SetSamplerState( 6, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	r3dRenderer->pd3ddev->SetSamplerState( 6, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	r3dRenderer->pd3ddev->SetSamplerState( 6, D3DSAMP_MAXANISOTROPY, 16 );

	r3dRenderer->SetTex(FoamTexture, 12);
	r3dRenderer->pd3ddev->SetSamplerState( 12, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( 12, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( 12, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	r3dRenderer->pd3ddev->SetSamplerState( 12, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	r3dRenderer->pd3ddev->SetSamplerState( 12, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	r3dRenderer->pd3ddev->SetSamplerState( 12, D3DSAMP_MAXANISOTROPY, 16 );

	if(RIPPLES_TEXTURE_SIZE > 0.0f && curRipplesRT!=-1)
	{
		//r3dRenderer->SetTex(rainRipplesRT[curRipplesRT]->Tex, 8);
		r3dRenderer->pd3ddev->SetSamplerState( 8, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
		r3dRenderer->pd3ddev->SetSamplerState( 8, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
		r3dRenderer->pd3ddev->SetSamplerState( 8, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		r3dRenderer->pd3ddev->SetSamplerState( 8, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
		r3dRenderer->pd3ddev->SetSamplerState( 8, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
		r3dRenderer->pd3ddev->SetSamplerState( 8, D3DSAMP_MAXANISOTROPY, 16 );

		r3dRenderer->SetTex(ripplesRT[curRipplesRT]->Tex, 9);
		r3dRenderer->pd3ddev->SetSamplerState( 9, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
		r3dRenderer->pd3ddev->SetSamplerState( 9, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
		r3dRenderer->pd3ddev->SetSamplerState( 9, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		r3dRenderer->pd3ddev->SetSamplerState( 9, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
		r3dRenderer->pd3ddev->SetSamplerState( 9, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
		r3dRenderer->pd3ddev->SetSamplerState( 9, D3DSAMP_MAXANISOTROPY, 16 );
	}
}

void WaterBase::RenderEnd()
{
	R3DPROFILE_FUNCTION("WaterBase::RenderEnd");
	r3dRenderer->SetTex(0, 7); // remove skydome cubemap texture
	r3dRenderer->RestoreCullMode();

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();

	d3dc._SetDecl( R3D_MESH_VERTEX::getDecl() );

	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetTex(NULL);
	r3dRenderer->SetRenderingMode(R3D_BLEND_COPY_ZCW);
}

void WaterBase::ReloadTextures()
{
	for( int i = 0, e = R3D_ARRAYSIZE( WaterColor ) ; i < e; i ++ )
	{
		waterReloadTexture( WaterColor[ i ] ) ;
	}

	waterReloadTexture( ColorTexture );
	waterReloadTexture( FoamTexture );
}

void WaterBase::UpdateRipples()
{
	isRippled = Ripples( rippleCamd ) ;
}
#endif // WO_SERVER

void WaterBase::ReadSerializedData(pugi::xml_node& ch_node)
{
#ifndef WO_SERVER
	if(!ch_node.attribute("shallow_color").empty())	shallowColor.SetPacked( ch_node.attribute("shallow_color").as_uint() );
	if(!ch_node.attribute("deep_color").empty())	deepColor.SetPacked( ch_node.attribute("deep_color").as_uint() );
	if(!ch_node.attribute("atten_color").empty())	attenColor.SetPacked( ch_node.attribute("atten_color").as_uint() );
	if(!ch_node.attribute("specular").empty())	specularCosinePower = ch_node.attribute("specular").as_float();
	if(!ch_node.attribute("specularTiling").empty())	specularTiling = ch_node.attribute("specularTiling").as_float();

	if(!ch_node.attribute("atten_dist").empty())	attenDist = ch_node.attribute("atten_dist").as_float();
	if(!ch_node.attribute("shallow_depth").empty())	shallowDepth = ch_node.attribute("shallow_depth").as_float();
	if(!ch_node.attribute("tile_size").empty())	tileSize = ch_node.attribute("tile_size").as_float();
	if(!ch_node.attribute("caustic_tile").empty())	causticTiling = ch_node.attribute("caustic_tile").as_float();
	if(!ch_node.attribute("refraction_index").empty())	refractionIndex = ch_node.attribute("refraction_index").as_float();
	if(!ch_node.attribute("bumpness").empty())	bumpness = ch_node.attribute("bumpness").as_float();
	if(!ch_node.attribute("caustic_depth").empty())	causticDepth = ch_node.attribute("caustic_depth").as_float();
	if(!ch_node.attribute("caustic_strength").empty())	causticStrength = ch_node.attribute("caustic_strength").as_float();

	if(!ch_node.attribute("uRefractionIndex").empty())	uRefractionIndex = ch_node.attribute("uRefractionIndex").as_float();
	if(!ch_node.attribute("uRefractionPerturbation").empty())	uRefractionPerturbation = ch_node.attribute("uRefractionPerturbation").as_float();
	if(!ch_node.attribute("uAttenDist").empty())	uAttenDist = ch_node.attribute("uAttenDist").as_float();
	if(!ch_node.attribute("uShallowColor").empty())	uShallowColor.SetPacked( ch_node.attribute("uShallowColor").as_uint() );
	if(!ch_node.attribute("uDeepColor").empty())	uDeepColor.SetPacked( ch_node.attribute("uDeepColor").as_uint() );
	if(!ch_node.attribute("uAttenColor").empty())	uAttenColor.SetPacked( ch_node.attribute("uAttenColor").as_uint() );


	if(!ch_node.attribute("waterColorTile").empty())	waterColorTile = ch_node.attribute("waterColorTile").as_float();
	if(!ch_node.attribute("waterColorBlend").empty())	waterColorBlend = ch_node.attribute("waterColorBlend").as_float();
	if(!ch_node.attribute("refractionPerturbation").empty())	refractionPerturbation = ch_node.attribute("refractionPerturbation").as_float();
	if(!ch_node.attribute("foamTile").empty())	foamTile = ch_node.attribute("foamTile").as_float();
	if(!ch_node.attribute("foamPerturbation").empty())	foamPerturbation = ch_node.attribute("foamPerturbation").as_float();
	if(!ch_node.attribute("foamSpeed").empty())	foamSpeed = ch_node.attribute("foamSpeed").as_float();
	if(!ch_node.attribute("foamIntensity").empty())	foamIntensity = ch_node.attribute("foamIntensity").as_float();
	if(!ch_node.attribute("surfTile").empty())	surfTile = ch_node.attribute("surfTile").as_float();
	if(!ch_node.attribute("surfPeriod").empty())	surfPeriod = ch_node.attribute("surfPeriod").as_float();
	if(!ch_node.attribute("surfWidth").empty())	surfWidth = ch_node.attribute("surfWidth").as_float();
	if(!ch_node.attribute("surfRange").empty())	surfRange = ch_node.attribute("surfRange").as_float();
	if(!ch_node.attribute("surfPerturbation").empty())	surfPerturbation = ch_node.attribute("surfPerturbation").as_float();
	if(!ch_node.attribute("surfIntensity").empty())	surfIntensity = ch_node.attribute("surfIntensity").as_float();
	if(!ch_node.attribute("surf2Coast").empty())	surf2Coast = ch_node.attribute("surf2Coast").as_float();

	if(!ch_node.attribute("specBumpiness").empty()) specBumpiness = ch_node.attribute("specBumpiness").as_float();
	if(!ch_node.attribute("specIntensity").empty()) specIntensity = ch_node.attribute("specIntensity").as_float();

	if(!ch_node.attribute("reflectionIntensity").empty()) reflectionIntensity = ch_node.attribute("reflectionIntensity").as_float();
	if(!ch_node.attribute("fresnelPow").empty()) fresnelPow = ch_node.attribute("fresnelPow").as_float();
	if(!ch_node.attribute("fresnelBumpiness").empty()) fresnelBumpiness = ch_node.attribute("fresnelBumpiness").as_float();

	if(!ch_node.attribute("farTileScale").empty()) farTileScale = ch_node.attribute("farTileScale").as_float();	
	if(!ch_node.attribute("farTileFadeStart").empty()) farTileFadeStart = ch_node.attribute("farTileFadeStart").as_float();
	if(!ch_node.attribute("farTileFadeEnd").empty()) farTileFadeEnd = ch_node.attribute("farTileFadeEnd").as_float();
	if(!ch_node.attribute("farTileAmmount").empty()) farTileAmmount = ch_node.attribute("farTileAmmount").as_float();
	if(!ch_node.attribute("farTileBumpiness").empty()) farTileBumpiness = ch_node.attribute("farTileBumpiness").as_float();
#endif // WO_SERVER
}

#ifndef WO_SERVER
void WaterBase::WriteSerializedData(pugi::xml_node& ch_node)
{
	ch_node.append_attribute("shallow_color") = (unsigned int)shallowColor.GetPacked();
	ch_node.append_attribute("deep_color") = (unsigned int)deepColor.GetPacked();
	ch_node.append_attribute("atten_color") = (unsigned int)attenColor.GetPacked();
	ch_node.append_attribute("specular") = specularCosinePower;
	ch_node.append_attribute("specularTiling") = specularTiling;

	ch_node.append_attribute("atten_dist") = attenDist;
	ch_node.append_attribute("shallow_depth") = shallowDepth;
	ch_node.append_attribute("refraction_index") = refractionIndex;
	ch_node.append_attribute("tile_size") = tileSize;
	ch_node.append_attribute("caustic_tile") =	causticTiling;
	ch_node.append_attribute("bumpness") = bumpness;
	ch_node.append_attribute("caustic_depth") = causticDepth;
	ch_node.append_attribute("caustic_strength") = causticStrength;


	ch_node.append_attribute("uRefractionIndex") = uRefractionIndex;
	ch_node.append_attribute("uRefractionPerturbation") = uRefractionPerturbation;
	ch_node.append_attribute("uAttenDist") = uAttenDist;
	ch_node.append_attribute("uShallowColor") = (unsigned int)uShallowColor.GetPacked();
	ch_node.append_attribute("uDeepColor") = (unsigned int)uDeepColor.GetPacked();
	ch_node.append_attribute("uAttenColor") = (unsigned int)uAttenColor.GetPacked();


	ch_node.append_attribute("waterColorTile") = waterColorTile;
	ch_node.append_attribute("waterColorBlend") = waterColorBlend;
	ch_node.append_attribute("refractionPerturbation") = refractionPerturbation;
	ch_node.append_attribute("foamTile") = foamTile;
	ch_node.append_attribute("foamPerturbation") = foamPerturbation;
	ch_node.append_attribute("foamSpeed") = foamSpeed;
	ch_node.append_attribute("foamIntensity") = foamIntensity;
	ch_node.append_attribute("surfTile") = surfTile;
	ch_node.append_attribute("surfPeriod") = surfPeriod;
	ch_node.append_attribute("surfWidth") = surfWidth;
	ch_node.append_attribute("surfRange") = surfRange;
	ch_node.append_attribute("surfPerturbation") = surfPerturbation;
	ch_node.append_attribute("surfIntensity") = surfIntensity;
	ch_node.append_attribute("surf2Coast") = surf2Coast;

	ch_node.append_attribute( "specBumpiness" ) = specBumpiness ;
	ch_node.append_attribute( "specIntensity" ) = specIntensity ;

	ch_node.append_attribute( "reflectionIntensity" ) = reflectionIntensity ;
	ch_node.append_attribute( "fresnelPow" ) = fresnelPow ;
	ch_node.append_attribute( "fresnelBumpiness" ) = fresnelBumpiness ;

	ch_node.append_attribute( "farTileScale" ) = farTileScale ;
	ch_node.append_attribute( "farTileFadeStart" ) = farTileFadeStart ;
	ch_node.append_attribute( "farTileFadeEnd" ) = farTileFadeEnd ;
	ch_node.append_attribute( "farTileAmmount" ) = farTileAmmount ;
	ch_node.append_attribute( "farTileBumpiness" ) = farTileBumpiness ;
}

float WaterBase::SetNormalTextures(unsigned int t0, unsigned int t1, float fps) const
{
	float frame = r3dGetTime() * fps;
	int texIdx = int(frame) % 25;

	if (r_show_winter->GetBool())
		texIdx = 0;

	r3dRenderer->SetTex(WaterColor[texIdx], t0);
	r3dRenderer->pd3ddev->SetSamplerState( t0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( t0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( t0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	r3dRenderer->pd3ddev->SetSamplerState( t0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	r3dRenderer->pd3ddev->SetSamplerState( t0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	r3dRenderer->pd3ddev->SetSamplerState( t0, D3DSAMP_MAXANISOTROPY, 16 );
	texIdx++;
	if(texIdx==25)	texIdx = 0;

	if (r_show_winter->GetBool())
		texIdx = 0;

	r3dRenderer->SetTex(WaterColor[texIdx], t1);
	r3dRenderer->pd3ddev->SetSamplerState( t1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( t1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( t1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	r3dRenderer->pd3ddev->SetSamplerState( t1, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	r3dRenderer->pd3ddev->SetSamplerState( t1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
	r3dRenderer->pd3ddev->SetSamplerState( t1, D3DSAMP_MAXANISOTROPY, 16 );

	return frame - int(frame);
}

void WaterBase::OnDestroyBase()
{
	if(RipplesTex)	r3dRenderer->DeleteTexture(RipplesTex);	RipplesTex = 0;

	DeleteRippleBuffers();
}

void WaterBase::DeleteRippleBuffers()
{
	SAFE_DELETE( ripplesRT[0] );
	SAFE_DELETE( ripplesRT[1] );

	// force clear next time we update ripples
	curRipplesRT = -1 ;
}

void WaterBase::OnCreateBase()
{
	if(!RipplesTex)	RipplesTex = r3dRenderer->LoadTexture("Data\\Water\\ripples.dds");

	// NOTE : Vertex declaration is auto deleted upon exit in r3dRenderLayer
	if( !pScreenDecl )
	{
		D3DVERTEXELEMENT9 screenDecl[] = 
		{
			{0,  0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
			D3DDECL_END()
		};
		r3dDeviceTunnel::CreateVertexDeclaration( screenDecl, &pScreenDecl );
	}
}

void WaterBase::CreateWaterBuffers()
{
	R3DPROFILE_FUNCTION("WaterBase::CreateRefractionBuffer");

	DeleteRippleBuffers();

	float w, h ;

	GetDesiredRTDimmensions( &w, &h ) ;

	RIPPLES_TEXTURE_SIZE = 1024.0f;
	RIPPLES_AREA_SIZE = 40.0f;

	switch(r_environment_quality->GetInt())
	{
	case 1:
		RIPPLES_TEXTURE_SIZE = 0.0f;
		break;
	case 2:
		RIPPLES_TEXTURE_SIZE /= 2.0f;
		RIPPLES_AREA_SIZE = 30.0f;
		break;
	}
	curQualitySettings = r_environment_quality->GetInt();

	if(RIPPLES_TEXTURE_SIZE>0.0f && ripplesRT[0]==0)
	{
		ripplesRT[0] = r3dScreenBuffer::CreateClass("ripples 1", RIPPLES_TEXTURE_SIZE, RIPPLES_TEXTURE_SIZE, D3DFMT_G16R16);
		ripplesRT[1] = r3dScreenBuffer::CreateClass("ripples 2", RIPPLES_TEXTURE_SIZE, RIPPLES_TEXTURE_SIZE, D3DFMT_G16R16);

		//rainRipplesRT[0] = r3dScreenBuffer::CreateClass(512, 512, D3DFMT_G16R16);
		//rainRipplesRT[1] = r3dScreenBuffer::CreateClass(512, 512, D3DFMT_G16R16);
	}

	UpdateRefractionBuffer( false );
}

void WaterBase::UpdateRefractionBuffer( bool allowZero )
{
	if( gWaterRefractionBuffer || !allowZero )
	{
		if( r_environment_quality->GetInt() == 1 )
			gWaterRefractionBuffer = TempBuffer;
		else
			gWaterRefractionBuffer = BlurredScreenBuffer;
	}
}

void WaterBase::FlushRefractionBuffer()
{
	gWaterRefractionBuffer = NULL;
}

#ifndef FINAL_BUILD
float WaterBase::DrawPropertyEditorWater(float scrx, float scry, float scrw, float scrh, float DEFAULT_CONTROLS_WIDTH, float DEFAULT_CONTROLS_HEIGHT)
{
	static int showBounds = 0 ;
	if( showBounds )
	{
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) ) ;

		D3DXVECTOR4 pointNear( gCam.x + gCam.vPointTo.x * farTileFadeStart, lastWaterLevel, gCam.z + gCam.vPointTo.z * farTileFadeStart, 1 ) ;

		D3DXVec4Transform( &pointNear, &pointNear, &r3dRenderer->ViewProjMatrix ) ;

		pointNear *= 1.f / pointNear.w ;

		float nearY = ( -pointNear.y * 0.5f + 0.5f ) * r3dRenderer->ScreenH ;
		r3dDrawLine2D( 0, nearY, r3dRenderer->ScreenW, nearY, 2.0f, r3dColor::green ) ;

		D3DXVECTOR4 pointFar( gCam.x + gCam.vPointTo.x * farTileFadeEnd, lastWaterLevel, gCam.z + gCam.vPointTo.z * farTileFadeEnd, 1 ) ;

		D3DXVec4Transform( &pointFar, &pointFar, &r3dRenderer->ViewProjMatrix ) ;

		pointFar *= 1.f / pointFar.w ;

		float farY = ( -pointFar.y * 0.5f + 0.5f ) * r3dRenderer->ScreenH ;
		r3dDrawLine2D( 0, farY, r3dRenderer->ScreenW, farY, 2.0f, r3dColor::green ) ;

		r3dRenderer->Flush() ;

		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE ) ) ;
	}

	float starty = scry;

	static float maxAttDist = attenDist * 3.0f;
	starty += imgui_Static(scrx, starty, "UNDERWATER", int(DEFAULT_CONTROLS_WIDTH - DEFAULT_CONTROLS_HEIGHT) - 200);
	starty += imgui_DrawColorPicker(scrx, starty, "Shallow water color", &shallowColor, 360, false, true );
	starty += imgui_DrawColorPicker(scrx, starty, "Light color", &attenColor, 360, false, true);

	starty += 22 ;

	if( r_environment_quality->GetInt() < 3 )
	{
		starty += imgui_Static( scrx, starty, "Set water quality to high\n for far tile settings!", 360, true, 33, true ) ;
	}
	else
	{
		starty += imgui_Value_Slider( scrx, starty, "Far tile scale", &farTileScale, 1.0f, 64.0f,	"%-02.2f",1 );
		starty += imgui_Value_Slider( scrx, starty, "Far fade start", &farTileFadeStart, 1.0f, 1024.0f,	"%-02.2f",1 );
		starty += imgui_Value_Slider( scrx, starty, "Far fade end", &farTileFadeEnd, farTileFadeStart + 0.25f, 2048.0f,	"%-02.2f",1 );
		starty += imgui_Value_Slider( scrx, starty, "Far tile ammount", &farTileAmmount, 0.125f, 16.0f,	"%-02.2f",1 );
		starty += imgui_Value_Slider( scrx, starty, "Far tile bumpiness", &farTileBumpiness, 0.125f, 16.0f,	"%-02.2f",1 );

		starty += imgui_Checkbox( scrx, starty, "Show bounds", &showBounds, 1 ) ;
	}

	starty += 22 ;

	starty += imgui_Value_Slider( scrx, starty, "Reflect strength", &reflectionIntensity, 0.01f, 3.0f,	"%-02.2f",1 );
	starty += imgui_Value_Slider( scrx, starty, "Fresnel pow", &fresnelPow, 0.01f, 32.0f,	"%-02.2f",1 );
	starty += imgui_Value_Slider( scrx, starty, "Fresnel bumpiness", &fresnelBumpiness, 0.125f, 16.0f,	"%-02.2f",1 );
	starty += imgui_Value_Slider( scrx, starty, "Refr. index", &refractionIndex,	1.0f, 10.f,	"%-02.2f",1 );
	starty += imgui_Value_Slider( scrx, starty, "Refr. perturbation", &refractionPerturbation, 0.0f, 1.0f,	"%-01.2f",1 );
	starty += imgui_Value_Slider( scrx, starty, "Caustic Depth", &causticDepth,	0.01f, maxAttDist,	"%-02.3f",1 );
	starty += imgui_Value_Slider( scrx, starty, "Caustic Strength", &causticStrength, 0.0f, 1.0f,	"%-02.3f",1 );
	starty += imgui_Value_Slider( scrx, starty, "Caustic tiling", &causticTiling,	0.001f, 0.2f,	"%-02.3f",1 );
	starty += DEFAULT_CONTROLS_HEIGHT;

	starty += imgui_Value_Slider( scrx, starty, "Max Att Dist", &maxAttDist,	1.0f, 1000.f,	"%-02.2f",1 );
	starty += imgui_Value_Slider( scrx, starty, "Attenuation Dist", &attenDist,	0.0f, maxAttDist,	"%-02.2f",1 );
	starty += DEFAULT_CONTROLS_HEIGHT;

	starty += imgui_Static(scrx, starty, "WATER SURFACE", int(DEFAULT_CONTROLS_WIDTH - DEFAULT_CONTROLS_HEIGHT) - 200);
	starty += imgui_DrawColorPicker(scrx, starty, "Water color", &deepColor, 360, false, true );
	starty += imgui_Value_Slider( scrx, starty, "Color tiling", &waterColorTile, 0.001f, 0.1f,	"%-0.3f",1 );
	starty += imgui_Value_Slider( scrx, starty, "Color blend", &waterColorBlend, 0.0f, 1.0f,	"%-01.2f",1 );
	starty += imgui_Value_Slider( scrx, starty, "Bumpness", &bumpness,	2.0f, 40.0f,	"%-02.2f",1 );
	starty += imgui_Value_Slider( scrx, starty, "Bump Tiling", &tileSize,	0.001f, 0.2f,	"%-02.3f",1 );
	starty += imgui_Value_Slider( scrx, starty, "Sun CosPow", &specularCosinePower,	1.0f, 500.0f,	"%-02.3f",1 );
	starty += imgui_Value_Slider( scrx, starty, "Sun Bumpiness", &specBumpiness,	0.1f, 30.0f,	"%-02.3f" );
	starty += imgui_Value_Slider( scrx, starty, "Sun Intensity", &specIntensity,	0.01f, 10.0f,	"%-02.3f" );

	if( r_environment_quality->GetInt() != 3 )
	{
		starty += imgui_Value_Slider( scrx, starty, "Specular Tiling", &specularTiling,	1.0f, 10.0f,	"%-02.3f",1 );
	}

	starty += imgui_Value_Slider( scrx, starty, "Coastline width", &shallowDepth,	0.1f, 10.f,	"%-02.2f",1 );
	starty += DEFAULT_CONTROLS_HEIGHT;

	if ( imgui_Button(scrx, starty, DEFAULT_CONTROLS_HEIGHT, DEFAULT_CONTROLS_HEIGHT, showSurf ? "-":"+" ) )	showSurf = 1 - showSurf;
	starty += imgui_Static(scrx+DEFAULT_CONTROLS_HEIGHT+2, starty, "Underwater mode", int(DEFAULT_CONTROLS_WIDTH - DEFAULT_CONTROLS_HEIGHT) - 2);
	if ( showSurf )
	{
		starty += imgui_Value_Slider( scrx, starty, "Refr. index", &uRefractionIndex, 1.0f, 10.0f,	"%-02.2f",1 );
		starty += imgui_Value_Slider( scrx, starty, "Refr. perturbation", &uRefractionPerturbation, 0.0f, 1.0f,	"%-01.2f",1 );
		starty += imgui_Value_Slider( scrx, starty, "Attenuation Dist", &uAttenDist, 0.01f, maxAttDist,	"%-02.2f",1 );
		starty += imgui_DrawColorPicker(scrx, starty, "Shallow water color", &uShallowColor, 360, false, true );
		starty += imgui_DrawColorPicker(scrx, starty, "Water color", &uDeepColor, 360, false, true );
		starty += imgui_DrawColorPicker(scrx, starty, "Light color", &uAttenColor, 360, false, true );
		if( imgui_Button( scrx, starty, 130, 20, "Apply from water", false, true))
		{
			uRefractionIndex = refractionIndex;
			uRefractionPerturbation = refractionPerturbation;
			uAttenDist = attenDist;
			uShallowColor = shallowColor;
			uDeepColor = deepColor;
			uAttenColor = attenColor;
		}
	}


	/*if ( imgui_Button(scrx, starty, DEFAULT_CONTROLS_HEIGHT, DEFAULT_CONTROLS_HEIGHT, showFoam ? "-":"+" ) )	showFoam = 1 - showFoam;
	starty += imgui_Static(scrx+DEFAULT_CONTROLS_HEIGHT+2, starty, "Foam properties", int(DEFAULT_CONTROLS_WIDTH - DEFAULT_CONTROLS_HEIGHT) - 2);
	if ( showFoam )
	{
		starty += imgui_Value_Slider( scrx, starty, "Tiling", &foamTile, 0.01f, 0.2f,	"%-01.2f",1 );
		starty += imgui_Value_Slider( scrx, starty, "Perturbation", &foamPerturbation, 0.0f, 0.5f,	"%-01.2f",1 );
		starty += imgui_Value_Slider( scrx, starty, "Movement speed", &foamSpeed, 0.0f, 1.0f,	"%-01.2f",1 );
		starty += imgui_Value_Slider( scrx, starty, "Intensity", &foamIntensity, 0.0f, 1.0f,	"%-01.2f",1 );
		starty += DEFAULT_CONTROLS_HEIGHT;
	}

	if ( imgui_Button(scrx, starty, DEFAULT_CONTROLS_HEIGHT, DEFAULT_CONTROLS_HEIGHT, showSurf ? "-":"+" ) )	showSurf = 1 - showSurf;
	starty += imgui_Static(scrx+DEFAULT_CONTROLS_HEIGHT+2, starty, "Shallow foam properties", int(DEFAULT_CONTROLS_WIDTH - DEFAULT_CONTROLS_HEIGHT) - 2);
	if ( showSurf )
	{
		starty += imgui_Value_Slider( scrx, starty, "Tiling", &surfTile, 0.01f, 0.2f,	"%-01.2f",1 );
		starty += imgui_Value_Slider( scrx, starty, "Period (sec)", &surfPeriod, 0.0f, 10.0f,	"%-02.2f",1 );
		starty += imgui_Value_Slider( scrx, starty, "Width from coast", &surfWidth, 0.0f, 5.0f,	"%-01.2f",1 );
		starty += imgui_Value_Slider( scrx, starty, "Range of movement", &surfRange, 0.0001f, 0.001f,	"%-01.5f",1 );
		starty += imgui_Value_Slider( scrx, starty, "Perturbation", &surfPerturbation, 0.0f, 1.0f,	"%-01.2f",1 );
		starty += imgui_Value_Slider( scrx, starty, "Intensity", &surfIntensity, 0.0f, 2.0f,	"%-02.2f",1 );
		starty += imgui_Value_Slider( scrx, starty, "Blend to coast", &surf2Coast, 0.0f, 1.0f,	"%-01.2f",1 );
	}*/

	return starty-scry;
}
#endif
#endif // WO_SERVER
