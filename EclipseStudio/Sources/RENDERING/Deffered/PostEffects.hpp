

void Post_DrawTerrainBounds( const r3dTerrainPaintBoundControl& boundCtrl )
{
// r3dRenderer->SetTex(gBuffer_Color->Tex);
 r3dRenderer->SetTex(gBuffer_Normal->Tex,1);
 r3dRenderer->SetTex(gBuffer_Depth->Tex,2);
// r3dRenderer->SetTex(gBuffer_Aux->Tex,3);

 float DepthZ = r3dRenderer->FarClip * 0.9375f;

 const D3DXMATRIX& invV = r3dRenderer->InvViewMatrix ;

 D3DXVECTOR4 psConts[] = 
 {
   // float4   vCamPos    : register(c7);  // x,y,z
   D3DXVECTOR4( gCam.x,gCam.y,gCam.z, DepthZ ),
   // float4   vBounds    : register(c8);  // minh, maxh, minslope, maxslope
   D3DXVECTOR4( boundCtrl.PaintMinHeight, boundCtrl.PaintMaxHeight, cosf( D3DXToRadian( boundCtrl.PaintMaxSlope ) ), cosf( D3DXToRadian( boundCtrl.PaintMinSlope ) )  ),
   // float4   vExtra     : register(c9);
   D3DXVECTOR4( boundCtrl.InvertBounds ? 0.f : 0.5f, boundCtrl.InvertBounds ? 0.5f : 0.0f, 0.f, 0.f ),
   // float4   vInvResolution : register(c10) ;
   D3DXVECTOR4( 1.0f / gBuffer_Depth->Width, 1.0f / gBuffer_Depth->Height, 0.f, 0.f ),
   // float4   g_vInvProjScaleTrans : register ( c11 ) ;
   D3DXVECTOR4( 2.0f / r3dRenderer->ProjMatrix._11, -2.0f / r3dRenderer->ProjMatrix._22, -1.0f / r3dRenderer->ProjMatrix._11, 1.0f / r3dRenderer->ProjMatrix._22 ),
   // float4x3 g_vInvView : register( c12 ) ;
   D3DXVECTOR4( invV._11, invV._21, invV._31, invV._41 ),
   D3DXVECTOR4( invV._12, invV._22, invV._32, invV._42 ),
   D3DXVECTOR4( invV._13, invV._23, invV._33, invV._43 )
 } ;

 D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF(  7, (float *)psConts,  R3D_ARRAYSIZE( psConts ) ) );

 r3dRenderer->SetPixelShader( "POST_TERRAINBOUNDS" );
 r3dRenderer->SetVertexShader( "VS_POST_QUAD" );
 
 D3DXMATRIX 	mWorld;
 D3DXMATRIX ShaderMat;
 D3DXMatrixIdentity(&mWorld);

 ShaderMat =  mWorld * 	r3dRenderer->ViewProjMatrix;

 D3DXMatrixTranspose( &ShaderMat, &ShaderMat );

 r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float *)&ShaderMat,  4 );

 r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);
 r3dDrawBox2DZ(0,0, r3dRenderer->ScreenW, r3dRenderer->ScreenH, DepthZ, r3dColor(255,150,0));

 r3dRenderer->SetVertexShader();
 r3dRenderer->SetPixelShader();

 r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

 r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
 r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );
 r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
 r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );

 r3dRenderer->SetTex(NULL);
 r3dRenderer->SetMaterial(NULL);
}




void Post_DrawTerrainMaterialTest()
{
// r3dRenderer->SetTex(gBuffer_Color->Tex);
// r3dRenderer->SetTex(gBuffer_Normal->Tex,1);
 r3dRenderer->SetTex(gBuffer_Depth->Tex);
// r3dRenderer->SetTex(gBuffer_Aux->Tex,3);

 float DepthZ = r3dRenderer->FarClip * 0.9375f;

 D3DXVECTOR4 CamVector(gCam.x,gCam.y,gCam.z, DepthZ);
 D3DXVECTOR4 PVector(20,100, 0,0);

 r3dRenderer->pd3ddev->SetPixelShaderConstantF(  7, (float *)&CamVector,  1 );
 r3dRenderer->pd3ddev->SetPixelShaderConstantF(  8, (float *)&PVector,  1 );

 r3dRenderer->SetPixelShader( "POST_TERRAINMAT" );
 r3dRenderer->SetVertexShader( "VS_POST_QUAD" );
 
 D3DXMATRIX 	mWorld;
 D3DXMATRIX ShaderMat;
 D3DXMatrixIdentity(&mWorld);

 ShaderMat =  mWorld * 	r3dRenderer->ViewProjMatrix;

 D3DXMatrixTranspose( &ShaderMat, &ShaderMat );

 r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float *)&ShaderMat,  4 );

 r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);
 r3dDrawBox2DZ(0,0, r3dRenderer->ScreenW, r3dRenderer->ScreenH, DepthZ, r3dColor(255,150,0));

 r3dRenderer->SetVertexShader();
 r3dRenderer->SetPixelShader();

 r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);

 r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
 r3dRenderer->pd3ddev->SetSamplerState( 1, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );
 r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_ADDRESSU,   D3DTADDRESS_WRAP );
 r3dRenderer->pd3ddev->SetSamplerState( 2, D3DSAMP_ADDRESSV,   D3DTADDRESS_WRAP );

 r3dRenderer->SetTex(NULL);
 r3dRenderer->SetMaterial(NULL);
}


