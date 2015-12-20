
float __ShadowBoxSize = 200.0f;
float __ShadowDistance = 800.0f;

float RealD = __ShadowBoxSize;
r3dCamera ShadowCam;


void SetupSMTransform(r3dCamera &Cam, float W, float H)
{
 D3DXMATRIX V, InvV;
 D3DXMATRIX matTexScale;
 D3DXMATRIX m_matTex2;
 D3DXMATRIX m_matLightProj, m_matLightView;

 D3DXVECTOR3 vUp;
 vUp.x = 0;
 vUp.y = 1;
 vUp.z = 0;


 D3DXVECTOR3 From;
 D3DXVECTOR3 To;

 Cam.vPointTo.Normalize();
 r3dVector vTo = Cam - Cam.vPointTo * __ShadowDistance;

 From =  D3DXVECTOR3(Cam.X,Cam.Y,Cam.Z);
 To = D3DXVECTOR3(vTo.x,vTo.y, vTo.Z);

 // Set the light projection matrix.
 D3DXMatrixOrthoLH(&m_matLightProj, W, H, 1, 10000);
 
 // Set the light view matrix.
 D3DXMatrixLookAtLH( &m_matLightView, &From, &To, &vUp);

 // This will scale and offset -1 to 1 range of x, y
 // coords output by projection matrix to 0-1 texture
 // coord range.
    ZeroMemory( &matTexScale, sizeof( D3DMATRIX ) );
    matTexScale._11 = -0.5f;
    matTexScale._22 = -0.5f;
    matTexScale._33 = 0.0f; 
    matTexScale._41 = 0.5f; 
    matTexScale._42 = 0.5f;
    matTexScale._43 = 1.0f; 
    matTexScale._44 = 1.0f;


    D3DXMATRIX mat, mat2;
    D3DXMatrixMultiply( &mat, &m_matLightProj, &matTexScale );
    D3DXMatrixMultiply( &mat2, &m_matLightView, &mat ); 

// ShaderMat =  mWorld * 	r3dRenderer->mCamera * r3dRenderer->mProj;
  D3DXMatrixTranspose( &mat2, &mat2 );

  r3dRenderer->pd3ddev->SetPixelShaderConstantF(  20, (float *)&From,  1 );
  r3dRenderer->pd3ddev->SetVertexShaderConstantF(  8, (float *)&mat2,  4 );
}




float __CurTime = 15.0;

int bUseDynamicLight = 1;




void RenderDX9Scene(int FullDraw = 1)
{
 float VIS_RANGE;
 r3dVector V;

 int __NumLights = 0;
 int __NumObjects = 0;
 int __NumPasses = 0;
  

 if (Terrain) Terrain->UpdateNormalMap();

 // ST - hack code for day/night cycle :)

		if (__CurTime > 24) __CurTime -= 24.0f;
		
		static int _DrawNight = 1;

	    if (!_DrawNight) __CurTime = 16.5f;

		Sun->SetTime(__CurTime);
		
        r3dRenderer->AmbientColor = Sun->SkyColor.GetColorValue(__CurTime/24.0f);

		 
 Distort_Reset();

 NumObjectsForDraw = 0;
 GameWorld.UpdateOccluders();

 r3dRenderer->SetCamera ( gCam);

 extern float hud_CameraMinDist;
 extern float hud_CameraMaxDist;


 RealD = __ShadowBoxSize;// + CD*__ShadowBoxSize; 


 // Set light active system and gather list of rendered objects
 r3dRenderer->SetFog(1);

 	 r3dLight* SunL = &Sun->SunLight;

	SunVector = SunL->Direction;
	SunVector.Normalize();

		D3DXVECTOR4 LightVec;

 //
 //
 //  Render Ambient and Sunlight pass with shadowmap and create list of meshes that need to be rendered 
 //  at this frame
 //
 //

 ShadowCam.SetPlanes(r3dRenderer->NearClip, r3dRenderer->FarClip);
 ShadowCam.FOV = 90;

 r3dPoint3D SP = Hud->FPS_Position; //gCam; 
 
 ShadowCam.SetPosition( SP - SunL->Direction * __ShadowDistance);
 ShadowCam.PointTo( SP );

 r3dRenderer->SetOrthoCamera ( ShadowCam, RealD, RealD);
 
 NumObjectsForDraw = 0;
 GameWorld.RecalcDynamicLights(WorldLightSystem);
 GameWorld.Draw(ShadowCam, OBJ_DRAW_USERDEFINED+100);



 if (1)
 {
	ShadowBuffer->Activate();
	r3dRenderer->StartRender(1);

	r3dRenderer->SetOrthoCamera ( ShadowCam, RealD, RealD);
	r3dRenderer->SetFog(0);
				
	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA);
	//r3dDrawBox2D(0,0,r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor(255,255,255));


	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );

	LightVec = D3DXVECTOR4(ShadowCam.X,ShadowCam.Y,ShadowCam.Z, 0);

	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 20, (float *)&LightVec,	1 );

	r3dRenderer->SetVertexShader("VS_SMDEPTHPASS");
	r3dRenderer->SetPixelShader("PS_SMDEPTHPASS");

	GameWorld.Draw(gCam, OBJ_DRAW_USERDEFINED+101);

	LightVec = D3DXVECTOR4(ShadowCam.X,ShadowCam.Y,ShadowCam.Z, 0);

	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 20, (float *)&LightVec,	1 );

	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );

	r3dRenderer->SetVertexShader("VS_SMDEPTHPASS");
	r3dRenderer->SetPixelShader("PS_SMDEPTHPASS");

//	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHAFUNC, 	D3DCMP_GREATEREQUAL);
//	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
//	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHAREF, 100);

    
    if (Terrain)
		Terrain->DrawBlack(gCam); //ShadowCam);

//r3dRenderer->SetTexture( __r3dShadeTexture[2]);

	r3dRenderer->SetVertexShader("VS_SMDEPTHPASS");
	r3dRenderer->SetPixelShader("PS_SMDEPTHPASS");

	for (int i=0;i<NumObjectsForDraw;i++)
	{
		r3dVector V;

		V = ShadowCam - *ForDraw[i]->CentralPoint;

		if (!r3dRenderer->IsBoxInsideFrustum(ForDraw[i]->BBox)) continue;

		ForDraw[i]->DrawSimple(1);
	}

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();

	r3dRenderer->EndRender();
	ShadowBuffer->Deactivate();
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, FALSE );

 }


// if (GameConfig.bShadowFiltering) 
//	   r3dBlurBuffer(&ShadowBuffer, &TempSMBuffer, GameConfig.bShadowFiltering);
   //r3dBlurBuffer(ShadowBuffer, TempSMBuffer, 4);



 D3DPERF_SetMarker(0xff0000ff, L"Render Ambient pass ( lightmaps / GI here )");
 r3d__SkipPresent = 1;
 r3dColor OldAmbient = r3dRenderer->AmbientColor;

// if (GameRenderFlags & Render_NVG)
// r3dRenderer->AmbientColor = __NVGAmbientColor;

 DepthBuffer->Activate(1);
 r3dRenderer->StartRender(1);

 r3dRenderer->SetCamera ( gCam);
 NumObjectsForDraw = 0;
 GameWorld.RecalcDynamicLights(WorldLightSystem);
 GameWorld.Draw(gCam, OBJ_DRAW_USERDEFINED+100);


 r3dRenderer->SetFog(1);



 if (1)
 if (SkyDome)
 {
  r3dRenderer->SetMaterial(NULL);
  r3dRenderer->SetFog(0);
  r3dRenderer->SetMipMapBias(-6.0f);

  SkyDome->Draw(gCam);
  Sun->DrawSun(gCam);

  r3dRenderer->SetFog(1);
 }



 r3dRenderer->SetFog(1);

 r3dRenderer->SetMaterial(NULL);
 r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );

 r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
 r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHAREF, 1);

//
 //
 if (1)
 {
	SunL->bCastShadows = 1;

	SunL->ProcessLightFunction(r3dGetFrameTime());

//	if(SunL->IsOn()) continue;

	r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW);
	SunL->SetShaderConstants(gCam);



//
// ST ------------- SET SHADOW MAPPING
//
 
 if ( Terrain ) 
 {
   SetupSMTransform(ShadowCam, RealD, RealD);	

   r3dRenderer->SetTexture(ShadowBuffer->Tex,7);
   r3dRenderer->pd3ddev->SetSamplerState( 7, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
   r3dRenderer->pd3ddev->SetSamplerState( 7, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );

   Terrain->Draw(gCam);
 }

 r3dRenderer->SetMaterial(NULL);
 r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_ZC | R3D_BLEND_ZW );

 r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
 r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHAREF, 1);
	
	SunL->SetShaderConstants(gCam);

  		SetupSMTransform(ShadowCam, RealD, RealD);	

		r3dRenderer->SetTexture(ShadowBuffer->Tex,5);
		r3dRenderer->pd3ddev->SetSamplerState( 5, D3DSAMP_ADDRESSU,   D3DTADDRESS_CLAMP );
		r3dRenderer->pd3ddev->SetSamplerState( 5, D3DSAMP_ADDRESSV,   D3DTADDRESS_CLAMP );


	GameWorld.Draw(gCam, OBJ_DRAW_USERDEFINED+102);

	for (int i=0;i<NumObjectsForDraw;i++)
	{
		r3dVector V;

		VIS_RANGE = RenderDistance + ForDraw[i]->BBox.Size.Length()/2.0f;

		V = gCam - *ForDraw[i]->CentralPoint;

		if (!r3dRenderer->IsBoxInsideFrustum(ForDraw[i]->BBox)) continue;

		if (V.Length() > VIS_RANGE && (ForDraw[i]->BBox.Size.Z<VIS_RANGE/2)) continue;

		ForDraw[i]->DrawLightPass();
	}
 }
 else
 {
	r3dVector SunV = SunVector;
	//if (!bSkyLight ) SunV= r3dVector (-1,-1,-1);
	WorldLightSystem.SetEnvironmentConstants(SunV, gCam);

	for (int i=0;i<NumObjectsForDraw;i++)
	{
		r3dVector V;

		VIS_RANGE = RenderDistance + ForDraw[i]->BBox.Size.Length()/2.0f;

		V = gCam - *ForDraw[i]->CentralPoint;

		if (!r3dRenderer->IsBoxInsideFrustum(ForDraw[i]->BBox)) continue;

		if (V.Length() > VIS_RANGE) continue;

		ForDraw[i]->DrawAmbientPass();
    }
 }

 GameWorld.RecalcDynamicLights(WorldLightSystem);
 GameWorld.Draw(gCam, OBJ_DRAW_USERDEFINED+200);
 GameWorld.Draw(gCam, OBJ_DRAW_USERDEFINED+300);  // SFX


 r3dRenderer->SetVertexShader();
 r3dRenderer->SetPixelShader();
 r3dRenderer->SetFog(0);

 DepthBuffer->Deactivate();
 r3dRenderer->EndRender();
 r3d__SkipPresent = 0;


 D3DPERF_SetMarker(0xff0000ff, L"Light Pass");

 //
 //
 // Now we have filled Z buffer as well as ambient pass in framebuffer
 // Let's create shadow mask for each light and then add light pass into frame buffer
 // 
 //
/*         
          switch (ShadowMode)
          {
	   	case 0:
		          if (TT1) r3dShadow->BuildFailVolume(ForDraw[i], l);

			  if (TT1 && TT2)
                          {
//                           r3dRenderer->pd3ddev->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.0f));
//			   r3dRenderer->pd3ddev->SetRenderState( D3DRS_DEPTHBIAS, F2DW(ZBias) );

			   r3dShadow->RenderZFail();

//		           r3dRenderer->pd3ddev->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.0f));
//			   r3dRenderer->pd3ddev->SetRenderState( D3DRS_DEPTHBIAS, F2DW(0.0f) );
                          }
		break;


	   	case 1:
                          if (TT1) r3dShadow->BuildPassVolume(ForDraw[i], l);

			  if (TT1 && TT2)
                          {
                           r3dRenderer->pd3ddev->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.0f));
			   r3dRenderer->pd3ddev->SetRenderState( D3DRS_DEPTHBIAS, F2DW(ZBias) );

			   r3dShadow->RenderZPass();

		           r3dRenderer->pd3ddev->SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, F2DW(0.0f));
			   r3dRenderer->pd3ddev->SetRenderState( D3DRS_DEPTHBIAS, F2DW(0.0f) );
			  }
		break;

	  } // Fail/Pass selection

*/


 r3dLight 	*l;


 if (bUseDynamicLight)
 for(l = WorldLightSystem.Lights; l; l = l->pNext)
 {
	if (l->Type == R3D_DIRECT_LIGHT ) continue; //l->OccluderID = -1;

	l->ProcessLightFunction(r3dGetFrameTime());

	if(!l->IsOn()) continue;

	l->RecalcBoundBox();
	if (!r3dRenderer->IsBoxInsideFrustum(l->BBox)) continue;

	if (l->OccluderID == -100 )
	{
		r3dBoundBox BBox = l->BBox;
		BBox.Org = *l;
		BBox.Size = r3dVector(1,1,1);
		l->OccluderID = GameWorld.FindOccluder(BBox);
	}

	if (l->bGlobalLight) l->OccluderID = -1;

	r3dVector VV = gCam - *l;
	if (VV.Length() > l->GetOuterRadius())
	{
		if (!GameWorld.CheckPVS(l->OccluderID, gCam)) continue;
	}

	VIS_RANGE = RenderDistance + l->GetOuterRadius();
	V = gCam - (r3dPoint3D)*l;
	if (V.Length() > VIS_RANGE) continue;

	D3DPERF_SetMarker(0xff0000ff, L"Creating shadow buffer");

	if (!HV_RenderShadows)  bRenderSoftShadows = 0;

	if (HV_RenderShadows)
	{
		ScreenBuffer->Activate();
		r3dRenderer->StartRender(0);
		r3dRenderer->SetCamera ( gCam);
		r3dRenderer->SetFog(0);
		r3dSetFiltering(0);
		r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

		D3DPERF_SetMarker(0xff0000ff, L"Clearing buffer screen quad");
		r3dDrawBox2D(0,0,r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor(0,0,0));

		if (l->bCastShadows)
		{
			// Depth testing enabled
			// Depth writes disabled
			// Color buf writes disabled
			D3DPERF_SetMarker(0xff0000ff, L"if light cast shadows render shadows - shadow volumes used here");
  
			r3dRenderer->SetRenderingMode(R3D_BLEND_NOALPHA | R3D_BLEND_ZC);
			r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00000000 );

			// Enable stenciling
			// Enable write to Stencil
			// Set stencil fun - always
			r3dRenderer->pd3ddev->Clear( 0L, NULL, D3DCLEAR_STENCIL, 0x0, 1.0f, 0L );

			r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE,   	TRUE );
			r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILFUNC,     	D3DCMP_ALWAYS );
			r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILREF,      	0x1 );
			r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILMASK,     	0xffffffff );
			r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILWRITEMASK,	0xffffffff );
			r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILFAIL,  	D3DSTENCILOP_KEEP );
			r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILPASS,  	D3DSTENCILOP_KEEP );

			extern FLOAT g_fExt; // = 300.0f; // Absolute extrusion distance

			r3dRenderer->SetMaterial(NULL);  
			r3dSetFiltering(0);
			r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA); // | R3D_BLEND_ZC);

			r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, 	0xffffffff );
		//    r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_BLUE);

			r3dRenderer->pd3ddev->SetRenderState(D3DRS_CULLMODE,       	D3DCULL_CCW);
		//        r3dRenderer->pd3ddev->SetRenderState(D3DRS_ZENABLE,        	D3DZB_TRUE);
		//        r3dRenderer->pd3ddev->SetRenderState(D3DRS_ZWRITEENABLE,   	FALSE);

			r3dRenderer->pd3ddev->SetRenderState ( D3DRS_STENCILFUNC,  	D3DCMP_EQUAL );
			r3dRenderer->pd3ddev->SetRenderState ( D3DRS_STENCILPASS,  	D3DSTENCILOP_KEEP );
			r3dRenderer->pd3ddev->SetRenderState ( D3DRS_STENCILFAIL,  	D3DSTENCILOP_KEEP );
			r3dRenderer->pd3ddev->SetRenderState ( D3DRS_STENCILZFAIL, 	D3DSTENCILOP_KEEP );
			r3dRenderer->pd3ddev->SetRenderState ( D3DRS_STENCILMASK,  	0xFFFFFFFF );
			r3dRenderer->pd3ddev->SetRenderState ( D3DRS_STENCILREF,	0x0 );

		} // IF light cast shadows


		D3DPERF_SetMarker(0xff0000ff, L"Fill light mask");
  
		r3dDrawBox2D(0,0,r3dRenderer->ScreenW, r3dRenderer->ScreenH, r3dColor(255,255,255));

		r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE,   	FALSE );

		r3dSetFiltering(1);

		r3dRenderer->EndRender();
		ScreenBuffer->Deactivate();

		D3DPERF_SetMarker(0xff0000ff, L"If shadow bluring enabled do it here - ping pong blur");
  
		if (bRenderSoftShadows && 0)
		{
			D3DXVECTOR4 BlurMul(0.25f, 0.25, 0.25f, 1.0f);
			r3dRenderer->pd3ddev->SetPixelShaderConstantF(  0, (float *)&BlurMul,  1 );
			r3dBlurBuffer(ScreenBuffer, TempBuffer, 8);
		}

		D3DPERF_SetMarker(0xff0000ff, L"BLUR ENDED");
	  
	} // if hardware shadows active

	//
	// Now Draw light pass for each object using shadow mask
	//
	D3DPERF_SetMarker(0xff0000ff, L"Now we go with actual light pass");
  
	r3d__SkipPresent = 1;
	r3dRenderer->StartRender(0);
	r3dSetFiltering(1);

	r3dRenderer->SetCamera ( gCam);

	r3dRenderer->SetFog(0);

	r3dRenderer->SetRenderingMode(R3D_BLEND_ADD | R3D_BLEND_ZC);

//  r3dRenderer->pd3ddev->SetRenderState(D3DRS_ALPHAREF, 1);
//  r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, 	TRUE );
//  r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHAFUNC, 	D3DCMP_GREATEREQUAL);

	l->SetShaderConstants(gCam);

	D3DXMATRIX V, P;
	D3DXMATRIX matTexScale1;

	r3dRenderer->pd3ddev->GetTransform( D3DTS_VIEW, &V );
	r3dRenderer->pd3ddev->GetTransform( D3DTS_PROJECTION, &P );

  // Create texture scaling matrix:
  // | 0.5      0        0        0 |
  // | 0        -0.5     0        0 |
  // | 0        0        0        0 |
  // | 0.5      0.5      1        1 |
  // This will scale and offset -1 to 1 range of x, y
    // coords output by projection matrix to 0-1 texture
    // coord range.
    D3DXMatrixIdentity(&matTexScale1);

    matTexScale1._11 = 0.5f;
    matTexScale1._22 = -0.5f; 
    matTexScale1._33 = 0.0f; 
    matTexScale1._41 = 0.5f; 
    matTexScale1._42 = 0.5f;
    matTexScale1._43 = 1.0f; 
    matTexScale1._44 = 1.0f;

    D3DXMATRIX mat, mat2;
    D3DXMatrixMultiply( &mat, &P, &matTexScale1 );
    D3DXMatrixMultiply( &mat2, &V, &mat ); 
    D3DXMatrixTranspose( &mat2, &mat2 );

    //r3dRenderer->pd3ddev->SetVertexShaderConstantF(25 , (float *)&mat2,  4 );

	r3dRenderer->SetTexture(ScreenBuffer->Tex, 5);

	for (int i=0;i<NumObjectsForDraw;i++)
	{
		r3dVector V;

		VIS_RANGE = RenderDistance + ForDraw[i]->BBox.Size.Length()/2.0f;

		V = gCam - *ForDraw[i]->CentralPoint;

		if (!r3dRenderer->IsBoxInsideFrustum(ForDraw[i]->BBox))	continue;

		if (!ForDraw[i]->BBox.Intersect(l->BBox)) continue;
		if (ForDraw[i]->LightmapTex && l->Type == R3D_DIRECT_LIGHT) continue;

		if (ForDraw[i]->LightmapTex && !l->bAffectLightmap) continue;

		if (V.Length() > VIS_RANGE && (ForDraw[i]->BBox.Size.Z<VIS_RANGE/2)) continue;

		ForDraw[i]->DrawLightPass();
	}

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();

	if (HV_RenderShadows)
		if (l->bCastShadows)
			r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE,  FALSE );

	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	r3dRenderer->pd3ddev->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	r3dRenderer->pd3ddev->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	r3dRenderer->pd3ddev->SetSamplerState(2, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	r3dRenderer->pd3ddev->SetSamplerState(2, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	r3dRenderer->EndRender();
	r3d__SkipPresent = 0;
	D3DPERF_SetMarker(0xff0000ff, L"LIGHT PASS FOR THIS LIGHT COMPLETED");
  
 } // FOR EACH LIGHT



 // Render final pass for misc objects
 D3DPERF_SetMarker(0xff0000ff, L"Render rest of objects");
 
 r3d__SkipPresent = 1;
 r3dRenderer->StartRender(0);
 r3dRenderer->SetCamera ( gCam);

// r3dRenderer->pd3ddev->SetRenderState(D3DRS_ALPHAREF, 1);
// r3dRenderer->pd3ddev->SetRenderState( D3DRS_ALPHATESTENABLE, 	TRUE );

 r3dRenderer->SetFog(1);


 GameWorld.RecalcDynamicLights(WorldLightSystem);
 GameWorld.Draw(gCam, OBJ_DRAW_USERDEFINED+200);
 GameWorld.Draw(gCam, OBJ_DRAW_USERDEFINED+300);  // SFX

 void DrawDepthEffect();

 if (objOceanPlane) DrawDepthEffect();

 r3dRenderer->EndRender();
 r3d__SkipPresent = 0;

 if (1) // FullDraw)
 {
	 
	 IDirect3DSurface9 	*BBuf; 
	  IDirect3DSurface9	*surf1;

	  if (bRenderRefraction) // && (gCam.Y > LakePlaneY))
	  {
	   r3dTexture* Tex = r3dEffectsBuffer.GetBuffer(r3dEffectsBuffer.GetBufferID("LakeRefract"));

	   r3dRenderer->pd3ddev->GetRenderTarget(0, &BBuf );
	   Tex->Tex->GetSurfaceLevel(0, &surf1);
	   r3dRenderer->pd3ddev->StretchRect( BBuf , NULL, surf1, NULL, D3DTEXF_LINEAR);
           surf1->Release();
           BBuf->Release();
	  }

	  r3d__SkipPresent = 1;

	  r3dRenderer->StartRender(0);

	  GameWorld.Draw(gCam, OBJ_DRAW_USERDEFINED+500);  // For water
	  GameWorld.Draw(gCam, OBJ_DRAW_USERDEFINED+600);  // For grass 
	  r3dRenderer->SetMaterial(NULL);

	  GameWorld.RenderOccluders();


	  r3dRenderer->EndRender();
	  r3d__SkipPresent = 0;
 }  

 r3dRenderer->AmbientColor = OldAmbient;
 D3DPERF_SetMarker(0xff0000ff, L"OK, primary Draw is finished. Rest is post processing shit");

}





