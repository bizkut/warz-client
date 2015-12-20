
inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

static void ClearFloatSurface( const D3DXVECTOR4& value ) ;


extern float __WorldRenderBias;

extern int __RenderSSAO;
void ActivatePrimaryDepth( int Target )
{
	if( r_double_depth_ssao->GetInt() )
	{
		gBuffer_PrimaryDepth->Activate( Target );
	}
	else
	{
		gBuffer_Depth->Activate( Target );
	}
}

static inline r3dScreenBuffer* GetSecondaryDepth()
{
	return ( r_double_depth_ssao->GetInt() ) ? gBuffer_SecondaryDepth : gBuffer_Depth;
}

void ActivateSecondaryDepth( int Target )
{
	GetSecondaryDepth()->Activate( Target );
}

void DeactivatePrimaryDepth()
{
	if( r_double_depth_ssao->GetInt() )
	{
		gBuffer_PrimaryDepth->Deactivate();
	}
	else
	{
		gBuffer_Depth->Deactivate();
	}
}

void DeactivateSecondaryDepth()
{
	GetSecondaryDepth()->Deactivate();
}

void SetMRTClearShaders( bool depth_only )
{
	r3dRenderer->SetVertexShader("VS_DS_CLEAR");

	if( depth_only )
	{
		r3dRenderer->SetPixelShader( "PS_DS_CLEAR_DEPTHONLY" ) ;
	}
	else
	{
		if( r_lighting_quality->GetInt() == 1 )
		{
			r3dRenderer->SetPixelShader( "PS_DS_CLEAR_NO_AUX" ) ;
		}
		else
		{
			r3dRenderer->SetPixelShader( "PS_DS_CLEAR" );
		}
	}
}

void SyncSkydomeStaticSkyParams()
{
	StaticSkySettings sts;

	const r3dAtmosphere* atmo = r3dGameLevel::Environment;

	sts.bEnabled		= atmo->bStaticSkyEnable;
	sts.bPlanarMapping	= atmo->bStaticSkyPlanarMapping;

	float midDawn = ( atmo->DawnStart + atmo->DawnEnd ) * 0.5f;
	float midDusk = ( atmo->DuskStart + atmo->DuskEnd ) * 0.5f;

	r3dAtmosphere::SkyPhase phase0, phase1;
	float lerpT;

	GetAdjecantSkyPhasesAndLerpT( &phase0, &phase1, &lerpT );

	sts.tex0			= atmo->StaticSkyTextures[ phase0 ];
	sts.tex1			= atmo->StaticSkyTextures[ phase1 ];

	sts.glowTex0		= atmo->StaticSkyGlowTextures[ phase0 ];
	sts.glowTex1		= atmo->StaticSkyGlowTextures[ phase1 ];

	sts.texLerpT		= lerpT;

	sts.texScaleX		= atmo->StaticTexGenScaleX;
	sts.texScaleY		= atmo->StaticTexGenScaleY;
	sts.texOffsetX		= atmo->StaticTexGetOffsetX;
	sts.texOffsetY		= atmo->StaticTexGetOffsetY;

	sts.mesh			= r3dGameLevel::Environment->bCustomStaticMeshEnable ? r3dGameLevel::Environment->StaticSkyMesh : 0;

	float dayT			= EnvGetDayT();

	sts.SunCtrlX		= r3dGameLevel::Environment->SunSpotAmplify.GetFloatValue( dayT );
	sts.SunCtrlY		= r3dGameLevel::Environment->SunSpotPow.GetFloatValue( dayT );
	sts.SunSpotColor	= r3dGameLevel::Environment->SunSpotColor.GetColorValue( dayT );

	sts.SunSpotIntensity	= r3dGameLevel::Environment->SunSpotIntensity.GetFloatValue( dayT );

	sts.dayT				= EnvGetDayT();
	sts.bIsNight			= IsNight();

	sts.SunIntensity		= r3dGameLevel::Environment->SunIntensityCoef.GetFloatValue( dayT );

	sts.DomeRotationY		= atmo->SkyDomeRotationY;

	SkyDome->SetStaticSkyParams( sts );
}

void SyncSkyParams()
{
	hoffman.m_HGg						= 0.9994f;//r3dGameLevel::Environment->GetCurrentHGg();
	hoffman.m_inscatteringMultiplier	= r3dGameLevel::Environment->GetCurrentInscatteringMultiplier();
	hoffman.m_betaRayMultiplier			= 20.0f;//r3dGameLevel::Environment->GetCurrentBetaRayMultiplier();
	hoffman.m_betaMieMultiplier			= 0.005f;//r3dGameLevel::Environment->GetCurrentBetaMieMultiplier();
	hoffman.m_sunIntensity				= r3dGameLevel::Environment->GetCurrentSunIntensityCoef();
	hoffman.m_turbitity					= 0;//r3dGameLevel::Environment->GetCurrentTurbitity();
	hoffman.m_fCloudsScale				= r3dGameLevel::Environment->SkyDomeCloudsScale;
	hoffman.m_fCloudDensity				= r3dGameLevel::Environment->SkyDomeCloudsDensity;

	hoffman.m_fSkySunsetBumpness		= r3dGameLevel::Environment->SkySunsetBumpness;
	hoffman.m_fSkySunsetOffset			= r3dGameLevel::Environment->SkySunsetOffset;
	hoffman.m_fSkySunsetPower			= r3dGameLevel::Environment->SkySunsetPower;
	hoffman.m_fSkySunsetStartCoef		= r3dGameLevel::Environment->SkySunsetStartCoef;

	hoffman.m_fSkyCloudsFadeStart		= r3dGameLevel::Environment->SkyCloudsFadeStart;
	hoffman.m_fSkyCloudsFadeEnd			= r3dGameLevel::Environment->SkyCloudsFadeEnd;

	hoffman.m_fSkyIntensity				= GetEnvSkyIntensity();

	extern float fLambda[3];

	r3dColor LCol = r3dGameLevel::Environment->GetCurrentLambdaColor();

	fLambda[0] = float(LCol.R) / 256.0f;
	fLambda[1] = float(LCol.G) / 256.0f;
	fLambda[2] = float(LCol.B) / 256.0f;

	SyncSkydomeStaticSkyParams();

}

void UpdateSkyDome( const r3dCamera& cam )
{
	R3DPROFILE_FUNCTION("UpdateSkyDome");
	SyncSkyParams();
	SkyDome->Update( cam );
}

void DrawSkyDome( bool drawNormals, bool hemisphere )
{
	if( SkyDome )
	{
		R3DPROFILE_FUNCTION("SkyDome");
		D3DPERF_BeginEvent( 0x0, L"SkyDome" );

		SyncSkyParams() ;
	
		SkyDome->Draw( gCam, drawNormals, hemisphere );

		D3DPERF_EndEvent();
	}
}

void RenderDepthScene()
{

	if( Terrain2 && r_terrain2->GetInt() ) // EDITADO
	{
		Terrain2->RenderDepth() ;
	}

	r3dRenderer->SetVertexShader( "VS_FWD_COLOR" ) ;

	GameWorld().Prepare( gCam );
	GameWorld().Draw( rsDrawDepth );
}


void ClearMRTUsingShaders()
{
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	SetMRTClearShaders( false );

	D3DXVECTOR4 pconst0 = D3DXVECTOR4 ( gCam.NearClip, gCam.FarClip, 0.0f, 0.0f );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF ( 0, (float*) pconst0, 1 );

	pconst0 = D3DXVECTOR4 ( Sun->SunLight.Direction.x, Sun->SunLight.Direction.y, Sun->SunLight.Direction.z, 0.0f );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF ( MC_SUNDIR, (float*) pconst0, 1  );

	r3dColor Cl = r3dGameLevel::Environment->Fog_Color.GetColorValue(r3dGameLevel::Environment->__CurTime/24.0f);

	r3dDrawBoxFS( r3dRenderer->ScreenW, r3dRenderer->ScreenH, Cl);
}

void RenderDeferredScene1()
{
	R3DPROFILE_FUNCTION( "RenderDeferredScene1" );

	r3dRenderer->ResetTextureCache();
	d3dc.Reset();

	r3dVector V;

	int __NumLights = 0;
	int __NumObjects = 0;
	int __NumPasses = 0;

	SetVolumeFogParams();

	void UpdateSSAOClearValue();
	UpdateSSAOClearValue();

	r3dRenderer->SetMipMapBias(__WorldRenderBias);

	if( r_double_depth_ssao->GetInt() )
	{
		gBuffer_SecondaryDepth->Activate();
		SetMRTClearShaders( true ) ;
		r3dDrawBoxFS( r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor::black );
		gBuffer_SecondaryDepth->Deactivate();
	}

	r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ ) ;

	r3dRenderer->SetDefaultCullMode( D3DCULL_CCW );

	d3dc._SetDecl( R3D_MESH_VERTEX::getDecl() );

	// sometimes scaleform or whatever ruins our cache so we make sure depth stencil is refreshed here
	r3dRenderer->SetDSS( 0 ) ;

	extern int gSLI_Crossfire_NumGPUs;

	int v = r_need_reset_exposure->GetInt();
	if( v )
	{
		SceneExposure0->Activate() ;

		ClearFloatSurface( D3DXVECTOR4( 1, 1, 1, 1 ) ) ;

		SceneExposure0->Deactivate() ;

		SceneExposure1->Activate() ;

		ClearFloatSurface( D3DXVECTOR4( 1, 1, 1, 1 ) ) ;

		SceneExposure1->Deactivate() ;

		r_need_reset_exposure->SetInt( std::max(0, v - 1) ) ;
	}

	if( r_device_clear->GetInt() )
	{
		D3DPERF_BeginEvent( 0x0, L"ClearScreen" );

		ActivatePrimaryDepth( 0 );
		SetMRTClearShaders( true ) ;
		r3dDrawBoxFS( r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor::black );
		DeactivatePrimaryDepth();

		gBuffer_Normal->Activate(0);
		// NOTE : sync clear value with DS_ClearBuffer_ps.hls
		D3D_V( r3dRenderer->pd3ddev->Clear( 0, NULL, D3DCLEAR_TARGET, 0x7fff7f, r3dRenderer->GetClearZValue(), 0 ) );
		gBuffer_Normal->Deactivate();

		// in case of half scale ssao R part of the AUX needs to be black
		// because grass will render into it with white ( brightness + contrast applied )
		// then MAX operation will be done with half ssao buffer from the rest of the scene
		if( r_lighting_quality->GetInt() > 1 && r_half_scale_ssao->GetInt() )
		{
			gBuffer_Aux->Activate( 0 );
	
			DWORD ssaoClearVal = R3D_MIN( R3D_MAX( int( 0xff * r_ssao_clear_val->GetFloat() ), 0 ), 0xff );

			D3D_V(	r3dRenderer->pd3ddev->Clear(	0, NULL, D3DCLEAR_TARGET, 
													D3DCOLOR_ARGB( 0, ssaoClearVal, 0, 0xff ),
													r3dRenderer->GetClearZValue(), 0 ) );
			gBuffer_Aux->Deactivate();
		}

		D3DPERF_EndEvent();
	}

	gBuffer_Color->Activate();

#ifndef FINAL_BUILD
	if( r_full_zreject->GetInt() )
	{
		D3D_V( r3dRenderer->pd3ddev->Clear( 0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, 1.0f - r3dRenderer->GetClearZValue(), 0 ) );
	}
	else
#endif
	{
		D3D_V( r3dRenderer->pd3ddev->Clear( 0, NULL, D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0, r3dRenderer->GetClearZValue(), 0 ) );
	}

	gDEBUG_DrawPositions.Clear();
	GameWorld().Prepare( gCam );

	//------------------------------------------------------------------------
	int MRTShaderCleared = 0 ;

	if( r_first_person->GetInt() )
	{
		R3DPROFILE_START( "First Person" ) ;
		D3DPERF_BeginEvent( 0x0, L"First Person" );

		if( !r_device_clear->GetInt() )
		{
			ClearMRTUsingShaders() ;
			MRTShaderCleared = 1 ;
		}

		r3dRenderer->SetZRange( R3D_ZRANGE_FIRST_PERSON ) ;

		r3dCamera firstPersonCam = gCam ;
		firstPersonCam.NearClip = r_first_person_render_z_start->GetFloat();
		firstPersonCam.FarClip = r_first_person_render_z_end->GetFloat();
		firstPersonCam.FOV = r_first_person_fov->GetFloat();

		r3dRenderer->SetCamera( firstPersonCam, true ) ;

		gBuffer_Normal->Activate(1);

		ActivatePrimaryDepth( 2 );

		gBuffer_Aux->Activate(3);

		r3dRenderer->StartRender(0);

		r3dRenderer->SetMaterial(NULL);
		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );

		SetupMaskWriteStencilStates( 1 );

		GameWorld().Draw( rsFillGBufferFirstPerson );	
	
		r3dRenderer->EndRender(0);

		gBuffer_Aux->Deactivate() ;
		DeactivatePrimaryDepth() ;

		gBuffer_Normal->Deactivate() ;

		r3dRenderer->SetCamera( gCam, true ) ;

		r3dRenderer->SetVertexShader() ;
		r3dRenderer->SetPixelShader() ;

		D3DPERF_EndEvent() ;
		R3DPROFILE_END( "First Person" ) ;
	}

	r3dRenderer->SetZRange( R3D_ZRANGE_WORLD ) ;

	D3DPERF_BeginEvent( 0x0, L"Depth Prepass" );
	R3DPROFILE_D3DSTART( D3DPROFILE_DEPTH_PREPASS ) ;

	if( r_z_prepass_method->GetInt() != Z_PREPASS_METHOD_NONE )
	{
		r3dRenderer->SetVertexShader( VS_DEPTH_PREPASS_ID ) ;
		extern int PS_FWD_COLOR_ID ;
		r3dRenderer->SetPixelShader( PS_FWD_COLOR_ID ) ;

		r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW | R3D_BLEND_PUSH ) ;

		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, 0 ) ) ;
		D3D_V( r3dRenderer->pd3ddev->SetRenderState(D3DRS_STENCILENABLE, FALSE ) ) ;

		GameWorld().Draw( rsDepthPrepass ) ;

		r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE,	D3DCOLORWRITEENABLE_RED | 
																		D3DCOLORWRITEENABLE_GREEN |
																		D3DCOLORWRITEENABLE_BLUE |
																		D3DCOLORWRITEENABLE_ALPHA	) ;

		r3dRenderer->SetRenderingMode( R3D_BLEND_POP ) ;

		r3dRenderer->SetVertexShader() ;
		r3dRenderer->SetPixelShader() ;
	}
	
	R3DPROFILE_D3DEND( D3DPROFILE_DEPTH_PREPASS ) ;
	D3DPERF_EndEvent();

	gBuffer_Normal->Activate(1);

	ActivatePrimaryDepth( 2 );

	gBuffer_Aux->Activate(3);
	r3dRenderer->StartRender(0);

	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_NZ);

	if( !r_device_clear->GetInt() )
	{
		if( !MRTShaderCleared )
		{
			ClearMRTUsingShaders() ;
		}
	}

	r3dRenderer->SetPixelShader();

	R3DPROFILE_D3DSTART( D3DPROFILE_FILLGBUFFER ) ;

	r3dRenderer->SetCamera ( gCam, true );

	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );
	// r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHAREF, 1);

	DeactivatePrimaryDepth();

	SetupMaskWriteStencilStates( 1 );

	R3DPROFILE_START("DR: Terrain");	
	ActivateSecondaryDepth( 2 );

	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );

	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

	if( Terrain3 && r_terrain3->GetInt() )
	{
		R3DPROFILE_FUNCTION("Draw Terrain3");
		Terrain3->Render( gCam ) ;
	}

	if( Terrain2 && r_terrain2->GetInt() )
	{
		R3DPROFILE_FUNCTION("Draw Terrain2");
		Terrain2->Render( gCam ) ;
	}

	if( r_double_depth_ssao->GetInt() )
	{
		DeactivateSecondaryDepth();
		ActivatePrimaryDepth( 2 );
	}

	r3dRenderer->SetCamera ( gCam, true );

	R3DPROFILE_END("DR: Terrain");	

	r3dRenderer->SetMaterial(NULL);
	r3dRenderer->SetVertexShader(VS_FILLGBUFFER_ID);
	FillbufferShaderKey key;

	SetLQ( &key ) ;
	
	SetFillGBufferPixelShader( key ) ;

	//	r3dRenderer->SetTex( g_EnvmapProbes.GetClosestTexture( gCam ), 5 );

	//	r3dRenderer->pd3ddev->SetSamplerState( 5, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
	//	r3dRenderer->pd3ddev->SetSamplerState( 5, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );


	D3DXVECTOR4 CamVec = D3DXVECTOR4(gCam.x, gCam.y, gCam.z, 1);
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(MC_CAMVEC, (float*)&CamVec, 1);

	float defSSAO[ 4 ] = { r_ssao_clear_val->GetFloat(), 0.f, 0.f, 0.f };
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(MC_DEF_SSAO, defSSAO, 1);

	r3dRenderer->SetMipMapBias(__WorldRenderBias);

	for( int i = 0; i < 6; i ++ )
	{
		r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
		r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );

		r3dSetFiltering( R3D_ANISOTROPIC, i );
	}

	extern int _r3d_Mesh_LoadSecondUV;
	if(_r3d_Mesh_LoadSecondUV)
	{
		// decal map at s11 must be wrapped
		r3dRenderer->pd3ddev->SetSamplerState( 11, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
		r3dRenderer->pd3ddev->SetSamplerState( 11, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );
	}

	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );

#ifndef FINAL_BUILD
	if( r_draw_meshes->GetInt() )
#endif
	{
		extern bool g_bEditMode ;
		extern RenderArray	g_render_arrays[ rsCount ] ;
		if( g_bEditMode && r_limit_renderables->GetInt() )
		{
			g_render_arrays[ rsFillGBuffer ].Resize( R3D_MIN( r_rend_draw_limit->GetInt(), (int)g_render_arrays[ rsFillGBuffer ].Count() ) ) ;
		}

#ifndef FINAL_BUILD
		if( !g_bEditMode )
#endif
		{
			UpdateD3DAntiCheat();
		}

#ifndef FINAL_BUILD
		if( r_wireframe->GetInt() )
		{
			r3dSetWireframe( 1 );
		}
#endif
		GameWorld().Draw( rsFillGBuffer );

#ifndef FINAL_BUILD
		if( r_wireframe->GetInt() )
		{
			r3dSetWireframe( 0 );
		}
#endif

		r3dRenderer->SetRenderingMode( R3D_BLEND_ZC | R3D_BLEND_NOALPHA | R3D_BLEND_PUSH );
		GameWorld().Draw( rsFillGBufferEffects );
		r3dRenderer->SetRenderingMode( R3D_BLEND_POP );

		GameWorld().Draw( rsFillGBufferAfterEffects );
	}

	if( r_grass_ssao->GetInt() )
	{
		bool useAux = r_lighting_quality->GetInt() > 1;

		if( !useAux )
		{
			gBuffer_Aux->Deactivate();
		}

		DrawGrass( GrassMap::COMBINED_PATH, false, useAux );

		if( !useAux )
		{
			gBuffer_Aux->Activate( 3 );
		}

		r_split_grass_render->SetInt( 0 );
	}

	R3DPROFILE_D3DEND( D3DPROFILE_FILLGBUFFER ) ;

	GameWorld().IssueOcclusionQueries();
	gFlashbangVisualController.IssueVisibilityQuery();

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();
	r3dRenderer->SetFog(0);

	DeactivatePrimaryDepth();

	gBuffer_Normal->Deactivate();

	gBuffer_Color->Deactivate();
	gBuffer_Aux->Deactivate();

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, FALSE ) );

	r3dRenderer->EndRender();

}

void RenderDeferredScene()
{

	R3DPROFILE_START("Render: Scene");
	RenderDeferredScene1();
	R3DPROFILE_END("Render: Scene");

	R3DPROFILE_START("Render: Buffers");

	//
	// create offscreen version of rendertarget for postprocessing
	//
	/*

	IDirect3DSurface9 	*BBuf; 
	IDirect3DSurface9	*surf1;

	r3dRenderer->pd3ddev->GetRenderTarget(0, &BBuf );
	ScreenBuffer->OurBBuf->GetSurfaceLevel(0, &surf1);
	r3dRenderer->pd3ddev->StretchRect( BBuf , NULL, surf1, NULL, D3DTEXF_NONE);
	surf1->Release();
	BBuf->Release();
	*/

	//
	// create blurred scaled down version of rendertarget for FOV/scope effects
	//

	D3DXVECTOR4 BlurMul;
	BlurMul = D3DXVECTOR4(0.25f, 0.25f, 0.25f, 1.0f);
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(  0, (float *)&BlurMul,  1 );

	// r3dBlur2Buffer(ScreenBuffer, BlurBuffer, TempBuffer,4);


	R3DPROFILE_END("Render: Buffers");
}


