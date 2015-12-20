#include "r3dPCH.h"
#include "r3d.h"
#include "r3dLight.h"
#include "d3dfont.h"

#include "Editors/LevelEditor_Collections.h"
#include "../../ObjectsCode/world/WaterPlane.H"

#include "GameCommon.h"

r3dVector	SunVector;
float		SunVectorXZProj;

#define BUFSIZE (r3dRenderer->ScreenW/2)
#define REFBUFSIZE 256

r3dScreenBuffer*	ScreenBuffer;
r3dScreenBuffer*	BlurredScreenBuffer;
r3dScreenBuffer*	TempShadowBuffer; // NOTE : Same as Scaleform_RenderToTextureRT
r3dScreenBuffer*	DistortionBuffer;
r3dScreenBuffer*	TempScreenBuffer;

r3dScreenBuffer*	BlurBuffer;
r3dScreenBuffer*	TempBuffer;

r3dScreenBuffer*	One8Buffer0;
r3dScreenBuffer*	One8Buffer1;
r3dScreenBuffer*	One16Buffer0;
r3dScreenBuffer*	One16Buffer1;
r3dScreenBuffer*	One32Buffer0;
r3dScreenBuffer*	One32Buffer1;
r3dScreenBuffer*	One64Buffer0;
r3dScreenBuffer*	One64Buffer1;

r3dScreenBuffer*	SunShadowMap;

r3dScreenBuffer*	SharedShadowMap;
r3dScreenBuffer*	LFUpdateShadowMap; // low frequency update shadow map - biggest slice for which there is little care
r3dScreenBuffer*	SharedShadowCubeMap;
r3dScreenBuffer*	TransparentShadowMap;

r3dScreenBuffer*	DepthBuffer;
r3dScreenBuffer*	gFlashbang_Multiframe;

r3dScreenBuffer*	InternalScreenshotBuffer ;



void r3dDrawPB(float x, float y, float w, float h)
{
 R3D_SCREEN_VERTEX 	V[6];
 float  	Z = r3dRenderer->NearClip;

 for (int i = 0; i < _countof(V); i++)
 {
  V[i].color  = 0xffffffff; //clr.GetPacked();
  V[i].z  = Z;
  V[i].rwh = Z;//1.0f / Z;
 }

 float DS =  -0.5f;

 V[0].x   = x+DS;  	V[0].y   = y + h+DS;
 V[1].x   = x+DS;  	V[1].y   = y+DS;
 V[2].x   = x + w+DS;  	V[2].y   = y + h+DS;
 V[3].x   = x + w+DS;  	V[3].y   = y+DS;

 V[0].tu = 0; 		V[0].tv = 1;
 V[1].tu = 0; 		V[1].tv = 0;
 V[2].tu = 1; 		V[2].tv = 1;
 V[3].tu = 1; 		V[3].tv = 0;

 V[4] = V[3];
 V[3] = V[1];
 V[5] = V[2];

r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
r3dRenderer->pd3ddev->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
r3dRenderer->pd3ddev->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
r3dRenderer->pd3ddev->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
r3dRenderer->pd3ddev->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
r3dRenderer->pd3ddev->SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
r3dRenderer->pd3ddev->SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	
 D3D_V( d3dc._SetVertexShader(0) );
 r3dDrawTriangleList(V, _countof(V));

 r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
 r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
 r3dRenderer->pd3ddev->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
 r3dRenderer->pd3ddev->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
 r3dRenderer->pd3ddev->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
 r3dRenderer->pd3ddev->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
 r3dRenderer->pd3ddev->SetSamplerState(3, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
 r3dRenderer->pd3ddev->SetSamplerState(3, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

}


void DrawDepthEffectMask ()
{
	D3DPERF_BeginEvent ( 0x0, L"DrawDepthEffectMask" );

	r3dRenderer->SetTex(DepthBuffer->Tex,4);

	r3dRenderer->pd3ddev->SetSamplerState( 4, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 4, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );

	D3DXVECTOR4 CamVector(gCam.x,gCam.y,gCam.z,1.0f);
	
	float Shade = R3D_CLAMP(-SunVector.Dot(r3dVector(0,1,0)),0.0f, 1.0f);

	Shade += (float(r3dRenderer->AmbientColor.B)/255.0f);

	float ShadeAddit = 0.0f;//(float(r3dRenderer->AmbientColor.B)/255.0f);

	D3DXVECTOR4 TintVector(55.0f/255.0f*Shade, 153.0f/255.0f*Shade, 221.0f/255.0f*Shade, ShadeAddit);
	
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(  9, (float *)&TintVector,  1 );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(  10, (float *)&CamVector,  1 );
	
	r3dRenderer->SetPixelShader("PS_ZDRAW");
	r3dRenderer->SetVertexShader("VS_ZDRAW");

	D3DXMATRIX 	mWorld;
	D3DXMATRIX ShaderMat;
	D3DXMatrixIdentity(&mWorld);
	ShaderMat =  mWorld * 	r3dRenderer->ViewProjMatrix;
	D3DXMatrixTranspose( &ShaderMat, &ShaderMat );

	r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float *)&ShaderMat,  4 );

	GameWorld().Draw( rsDrawDepthEffect );

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();

	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

	r3dRenderer->pd3ddev->SetSamplerState( 4, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( 4, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );

	D3DPERF_EndEvent ();
}

float g_waterLevel = -1e6f;
void DrawDepthEffect()
{
	const float deltaHeight = 0.1f;

	//get waterplane that is under/over the camera
	obj_WaterPlane * pPlane = 0;
	for(uint32_t i = 0; i < obj_WaterPlane::m_dWaterPlanes.Count (); i++ )
	{
		obj_WaterPlane * pP = obj_WaterPlane::m_dWaterPlanes[i];

		const obj_WaterPlane::Settings& sts = pP->GetSettings();

		r3dBoundBox const & bBox = pP->GetBBoxWorld();
		if(gCam.X >= bBox.Org.X && gCam.X <= bBox.Org.X + bBox.Size.X &&
			 gCam.Z >= bBox.Org.Z && gCam.Z <= bBox.Org.Z + bBox.Size.Z &&
			 gCam.Y <= sts.WaterPlaneHeight + deltaHeight)
		{
			//check for grid's cell where the camera is presents

			int i, j;

			const obj_WaterPlane::Info& info = pP->GetInfo();

			pP->GetCellIndexes( gCam, &i, &j );

			if( i < 0 || i >= info.CellCountX )	continue;
			if( j < 0 || j >= info.CellCountZ )	continue;

			if(pP->GetWaterGridValue(i,j)==0)	continue;

			if( pPlane==0 || ( pPlane->GetSettings().WaterPlaneHeight < pP->GetSettings().WaterPlaneHeight + deltaHeight ) )
				pPlane = pP;
		}
	}

	//select highest waterplane or ocean
	extern float LakePlaneY;
	float height = LakePlaneY + deltaHeight;
	WaterBase* wb = 0;
	if(objOceanPlane && gCam.Y < LakePlaneY + deltaHeight)
		if(pPlane && pPlane->GetSettings().WaterPlaneHeight > LakePlaneY)
		{
			wb = pPlane;
			height = pPlane->GetSettings().WaterPlaneHeight + deltaHeight;
		}
		else	wb = objOceanPlane;
	else
	{
		wb = pPlane;
		if(pPlane)	height = pPlane->GetSettings().WaterPlaneHeight + deltaHeight;
	}

	if(wb!=0)	g_waterLevel = height - deltaHeight;
	else	g_waterLevel = -1e6f;

	if(!wb)	return;




	D3DPERF_BeginEvent ( 0x0, L"DrawDepthEffect" );

	r3dRenderer->SetTex(DepthBuffer->Tex,4);

	float DepthZ = r3dRenderer->FarClip * 0.9375f;
	D3DXVECTOR4 CamVector(gCam.x,gCam.y,gCam.z,1.0f/DepthZ);

	r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILENABLE, false);
	r3dRenderer->pd3ddev->SetSamplerState( 4, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 4, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );

	float fdeepColor_LakeHeight[4];
	float fShallowColor_Depth[4];
	float fAttenColor_Dist[4];
	float fBumpness_RefrIndex_TileSize[4];
	D3DXVECTOR4 texLerp;
	float ic = 1.0f / 255.0f;

	fdeepColor_LakeHeight[0] = wb->uDeepColor.R * ic;	fdeepColor_LakeHeight[1] = wb->uDeepColor.G * ic;	fdeepColor_LakeHeight[2] = wb->uDeepColor.B * ic;	fdeepColor_LakeHeight[3] = height-deltaHeight;
	fShallowColor_Depth[0] = wb->uShallowColor.R * ic;	fShallowColor_Depth[1] = wb->uShallowColor.G * ic;	fShallowColor_Depth[2] = wb->uShallowColor.B * ic;	fShallowColor_Depth[3] = wb->shallowDepth;
	fAttenColor_Dist[0] = wb->uAttenColor.R * ic;	fAttenColor_Dist[1] = wb->uAttenColor.G * ic;	fAttenColor_Dist[2] = wb->uAttenColor.B * ic;	fAttenColor_Dist[3] = wb->uAttenDist;
	fBumpness_RefrIndex_TileSize[0] = 50 - wb->bumpness;	fBumpness_RefrIndex_TileSize[1] = wb->uRefractionIndex;	fBumpness_RefrIndex_TileSize[2] = wb->tileSize;	fBumpness_RefrIndex_TileSize[3] = 1;
	texLerp.x = wb->SetNormalTextures(2,3,10.f);

	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 14, fdeepColor_LakeHeight,  1 );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 15, fShallowColor_Depth,  1 );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 16, fAttenColor_Dist,  1 );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 17, fBumpness_RefrIndex_TileSize,  1 );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 18, &texLerp.x,  1 );


	D3DXVECTOR4 NearFarPlane(r3dRenderer->NearClip, r3dRenderer->FarClip, 0, 0);
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(  10, (float *)&CamVector,  1 );
	D3DXVECTOR4 halfInvRes( 1.f / r3dRenderer->ViewW, 1.f / r3dRenderer->ViewH, 0.f, 0.f );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(  11, (float *)&halfInvRes,  1 );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(  12, (float *)&NearFarPlane,  1 );



	r3dRenderer->SetVertexShader("VS_ZDRAW_FULL");
	r3dRenderer->SetPixelShader("PS_ZDRAW_FULL");

	D3DXMATRIX 	mWorld;
	D3DXMATRIX ShaderMat;
	D3DXMatrixIdentity(&mWorld);
	ShaderMat =  mWorld * 	r3dRenderer->ViewProjMatrix;
	//D3DXMatrixInverse(&ShaderMat, NULL, &ShaderMat );
	D3DXMatrixTranspose( &ShaderMat, &ShaderMat );

	r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float *)&ShaderMat,  4 );

	r3dRenderer->SetRenderingMode(R3D_BLEND_MODULATE | R3D_BLEND_NZ);
	//dest = dest*alpha + src;
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);

	r3dDrawBox2DZ(0,0, r3dRenderer->ScreenW, r3dRenderer->ScreenH, DepthZ, r3dColor(255,150,0));
	
	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();


	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

	r3dRenderer->pd3ddev->SetSamplerState( 4, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( 4, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );

	r3dRenderer->SetTex(NULL);
	r3dRenderer->SetMaterial(NULL);

	D3DPERF_EndEvent ();
}



#include "DrawWorld.hpp"


float			g_DoubleDepthSSAO_Blur = 32.0f;
float			g_DoubleDepthSSAO_BlurSens = 0.125f;
SSAOSettings	g_SSAOSettings[ SSM_COUNT ];

void SSAOConstraints::Restrict( SSAOSettings& sts )
{
	sts.Radius					= R3D_MIN( R3D_MAX( sts.Radius, MinRadius ), MaxRadius );
	sts.DepthRange				= R3D_MIN( R3D_MAX( sts.DepthRange, MinDepthRange ), MaxDepthRange );
	sts.RadiusExpandStart		= R3D_MIN( R3D_MAX( sts.RadiusExpandStart, MinRadiusExpandStart ), MaxRadiusExpandStart );
	sts.RadiusExpandCoef		= R3D_MIN( R3D_MAX( sts.RadiusExpandCoef, MinRadiusExpandCoef ), MaxRadiusExpandCoef );
	sts.Brightness				= R3D_MIN( R3D_MAX( sts.Brightness, MinBrightness ), MaxBrightness );
	sts.Contrast				= R3D_MIN( R3D_MAX( sts.Contrast, MinContrast ), MaxContrast );
	sts.BlurDepthSensitivity	= R3D_MIN( R3D_MAX( sts.BlurDepthSensitivity, MinBlurDepthSensitivity), MaxBlurDepthSensitivity );
	sts.BlurStrength			= R3D_MIN( R3D_MAX( sts.BlurStrength, MinBlurStrength), MaxBlurStrength );

	sts.DetailStrength			= R3D_MIN( R3D_MAX( sts.DetailStrength, MinDetailStrength ), MaxDetailStrength );
	sts.DetailRadius			= R3D_MIN( R3D_MAX( sts.DetailRadius, MinDetailRadius ), MaxDetailRadius );
	sts.DetailDepthRange		= R3D_MIN( R3D_MAX( sts.DetailDepthRange, MinDetailDepthRange ), MaxDetailDepthRange );

	sts.DetailRadiusExpandStart	= R3D_MIN( R3D_MAX( sts.DetailRadiusExpandStart, MinDetailRadiusExpandStart ), MaxDetailRadiusExpandStart );
	sts.DetailRadiusExpandCoef		= R3D_MIN( R3D_MAX( sts.DetailRadiusExpandCoef, MinDetailRadiusExpandCoef ), MaxDetailRadiusExpandCoef );
	sts.DetailFadeOut				= R3D_MIN( R3D_MAX( sts.DetailFadeOut, MinDetailFadeOut ), MaxDetailFadeOut );

	sts.BlurTapCount			= R3D_MIN( R3D_MAX( sts.BlurTapCount, MIN_BLUR_TAP_COUNT ), MAX_BLUR_TAP_COUNT );
	sts.BlurPassCount			= R3D_MIN( R3D_MAX( sts.BlurPassCount, MIN_BLUR_PASS_COUNT ), MAX_BLUR_PASS_COUNT );
}

SSAOConstraints g_SSAOConstraints[ SSM_COUNT ];

const char* SSAOMethodToString( SSAOMethod method )
{
	switch( method )
	{
	case SSM_REF:
		return "REF";
	case SSM_DEFAULT:
		return "DEFAULT";
	case SSM_HQ:
		return "HQ";
	default:
		return "";
	}
}

SSAOMethod StringToSSAOMethod( const char* szName )
{
	if ( ! strcmp( szName, "NORMAL" ) || 
		 ! strcmp( szName, "REF" ) )
		return SSM_REF;
	else if (	! strcmp( szName, "ALT_LW" ) ||
				! strcmp( szName, "DEFAULT" ) ||
				! strcmp( szName, "HSAO" ) )
		return SSM_DEFAULT;
	else if (	! strcmp( szName, "ALT" ) || 
				! strcmp( szName, "HQ" ) )
		return SSM_HQ;

	assert( false );
	return SSM_DEFAULT;
}

namespace
{
	char BLUR_PS_NAME_BASE[] = "PS_SSAO_BLUR0000";

	const char *BLUR_TAP_VALUES[ SSAOConstraints::MAX_BLUR_TAP_COUNT - SSAOConstraints::MIN_BLUR_TAP_COUNT + 1 ] = 
	{
		"2",
		"3",
		"4",
		"5",
		"6",
		"7"
	};
}

void GetSSAOBlurPSName( char(&buf)[32], int tapCount, int scaled, int normals, int gaussianBlur )
{
	r3d_assert( tapCount >= SSAOConstraints::MIN_BLUR_TAP_COUNT && 
				tapCount <= SSAOConstraints::MAX_BLUR_TAP_COUNT );

	memcpy( buf, BLUR_PS_NAME_BASE, sizeof BLUR_PS_NAME_BASE );
	buf[ sizeof BLUR_PS_NAME_BASE - 2 ] += tapCount;
	buf[ sizeof BLUR_PS_NAME_BASE - 3 ] += scaled;
	buf[ sizeof BLUR_PS_NAME_BASE - 4 ] += normals;
	buf[ sizeof BLUR_PS_NAME_BASE - 5 ] += gaussianBlur;
}

void FillSSAOBlurMacros( ShaderMacros& oMacros, int tapCount, int scaled, int normals, int gaussianBlur )
{
	oMacros.Resize( 4 ) ;

	oMacros[0].Name			= "NUM_SAMPLES";
	oMacros[0].Definition	= BLUR_TAP_VALUES[ tapCount - SSAOConstraints::MIN_BLUR_TAP_COUNT ];

	oMacros[1].Name			= "SCALED";
	oMacros[1].Definition	= scaled ? "1" : "0";

	oMacros[2].Name			= "USE_NORMAL";
	oMacros[2].Definition	= normals ? "1" : "0";

	oMacros[3].Name			= "GAUSSIAN_BLUR";
	oMacros[3].Definition	= gaussianBlur ? "1" : "0";

}

ColorCorrectionSettings g_ColorCorrectionSettings;

