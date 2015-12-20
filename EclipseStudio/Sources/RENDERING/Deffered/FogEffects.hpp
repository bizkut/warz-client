//=========================================================================
//	Module: FogEffects.hpp
//	Copyright (C) 2013.
//=========================================================================

extern float g_waterLevel;

//////////////////////////////////////////////////////////////////////////

float GetFogDepthZ()
{
	return r3dRenderer->FarClip * 0.9375f;
}

//////////////////////////////////////////////////////////////////////////

//To be sure that fog is turned off
void SetVolumeFogParams()
{
	D3DXVECTOR4 shaderConsts[5];
	ZeroMemory(&shaderConsts, _countof(shaderConsts) * sizeof(shaderConsts[0]));

	if (!r3dGameLevel::Environment->bVolumeFog)
	{
		D3DXVECTOR4 AerialDensity_Distance(0.0f, 100.0f, r3dGameLevel::Environment->Aerial_MipBias, 0);
		shaderConsts[0] = AerialDensity_Distance;
		r3dRenderer->SetTex(0, 7);
	}
	else
	{
		float time = r3dGameLevel::Environment->__CurTime/24.0f;

		float aerialDensity = r3dGameLevel::Environment->Aerial_Density.GetFloatValue(time) / 1000.0f;
		float aerialDistance = r3dGameLevel::Environment->Aerial_Distance.GetFloatValue(time);
		D3DXVECTOR4 AerialDensity_Distance(aerialDensity, aerialDistance,  r3dGameLevel::Environment->Aerial_MipBias, 0);
		shaderConsts[0] = AerialDensity_Distance;

		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 7, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP ) );
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 7, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP ) );
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 7, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP ) );

		r3dSetFiltering( R3D_BILINEAR, 7 ) ;

		r3dRenderer->SetTex(SkyDome->cubemap->Tex, 7);

		float t_m = r3dGameLevel::Environment->Fog_Distance.GetFloatValue(time);
		float Density = -0.05f*r3dGameLevel::Environment->Fog_Density.GetFloatValue(time);

		Density = Density* 500.0f / powf(400.0f, t_m);

		r3dColor Cl = r3dGameLevel::Environment->Fog_Color.GetColorValue(time);
		D3DXVECTOR4 TintVector(float(Cl.R)/255.0f, float(Cl.G)/255.0f, float(Cl.B)/255.0f, Density);

		TintVector.x = powf( TintVector.x, 2.2f ) ;
		TintVector.y = powf( TintVector.y, 2.2f ) ; 
		TintVector.z = powf( TintVector.z, 2.2f ) ;
		shaderConsts[1] = TintVector;

		// New Fog Parameters
		float 	Fog_HStart = r3dGameLevel::Environment->Fog_HStart.GetFloatValue(time);
		float 	Fog_HRange = r3dGameLevel::Environment->Fog_HRange.GetFloatValue(time);

		float	Fog_SkydomeMult1 = r3dGameLevel::Environment->Fog_SkydomeMult1.GetFloatValue(time);
		float	Fog_SkydomeMult2 = r3dGameLevel::Environment->Fog_SkydomeMult2.GetFloatValue(time);

		D3DXVECTOR4 Params1(Fog_HStart, Fog_HRange, Fog_SkydomeMult1, Fog_SkydomeMult2);
		shaderConsts[2] = Params1;

		float	Fog_CameraDist= r3dGameLevel::Environment->Fog_CameraDist.GetFloatValue(time);
		float	Fog_CameraRange= r3dGameLevel::Environment->Fog_CameraRange.GetFloatValue(time);
		float	Fog_CameraExp= r3dGameLevel::Environment->Fog_CameraExp.GetFloatValue(time);

		Density = r3dGameLevel::Environment->Fog_MaxDensity.GetFloatValue(time);

		D3DXVECTOR4 Params2(Fog_CameraDist, Fog_CameraRange, Fog_CameraExp, Density);
		shaderConsts[3] = Params2;

		float DepthZ = GetFogDepthZ();
		shaderConsts[4] = D3DXVECTOR4( gCam.x, gCam.y, gCam.z, 1.f / DepthZ );
	} // fog turned on

	//	FOG_CONST0, FOG_CONST1, FOG_CONST2, FOG_CONST3, FOG_CONST4
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(FOGC_CONST0, &shaderConsts[0].x, _countof(shaderConsts));
	r3dRenderer->pd3ddev->SetVertexShaderConstantF(250, &shaderConsts[0].x, _countof(shaderConsts));
}

void DrawVolumeFogEffect( r3dScreenBuffer* depthBuffer, r3dScreenBuffer* normalBuffer )
{

	//skip fog when camera is underwater
	extern float g_waterLevel;
	if(gCam.y < g_waterLevel - 0.1f)	return;

	r3dRenderer->SetTex(normalBuffer->Tex,3);
	r3dRenderer->SetTex(depthBuffer->Tex,4);

	/*
	r3dRenderer->pd3ddev->SetTexture(5, gNoiseTexture2->AsTexVolume());
	r3dRenderer->pd3ddev->SetSamplerState( 5, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( 5, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( 5, D3DSAMP_ADDRESSW,   D3DTADDRESS_WRAP );
	*/

	r3dRenderer->pd3ddev->SetSamplerState( 4, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
	r3dRenderer->pd3ddev->SetSamplerState( 4, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );

	SetVolumeFogParams();

	//	float4 HalfInvRes : register (c11);
	D3DXVECTOR4 params = D3DXVECTOR4( .5f / r3dRenderer->ScreenW, 0.5f / r3dRenderer->ScreenH, g_waterLevel, 0.f );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(11, &params.x, 1);

	r3dRenderer->SetPixelShader( "PS_VOLUMEFOGNOISE" ) ;

	r3dRenderer->SetVertexShader("VS_VOLUMEFOG");

	D3DXMATRIX 	mWorld;
	D3DXMATRIX ShaderMat;
	D3DXMatrixIdentity(&mWorld);

	ShaderMat =  mWorld * 	r3dRenderer->ViewProjMatrix ;

	D3DXMatrixTranspose( &ShaderMat, &ShaderMat );

	r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float *)&ShaderMat,  4 );

	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);
	r3dDrawBox2DZ(0,0, r3dRenderer->ScreenW, r3dRenderer->ScreenH, GetFogDepthZ(), r3dColor(255,150,0));

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();

	r3dRenderer->SetTex(0, 7);

	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

	r3dRenderer->pd3ddev->SetSamplerState( 4, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( 4, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );

	r3dRenderer->SetTex(NULL);
	r3dRenderer->SetMaterial(NULL);

	//r3dRenderer->pd3ddev->SetTexture(5, 0);

}

