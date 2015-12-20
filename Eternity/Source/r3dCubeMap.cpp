
#include "r3dPCH.h"
#include "r3d.h"

#include "r3dDeviceQueue.h"

r3dCubeMap::r3dCubeMap(int size, const r3dIntegrityGuardian& ig /*= r3dIntegrityGuardian()*/ )
: r3dIResource( ig )
{
  size_ = size;
  
  m_pRenderToEnvMap = NULL;
  m_pCubeMap        = NULL;

  CreateQueuedResource( this );

  return;
}

r3dCubeMap::~r3dCubeMap()
{
	ReleaseQueuedResource( this );
}

void SetD3DResourcePrivateData(LPDIRECT3DRESOURCE9 res, const char* FName);


void r3dCubeMap::D3DCreateResource()
{
  D3DFORMAT fmt = r3dRenderer->d3dpp.BackBufferFormat;

  // Create RenderToEnvMap object
  D3DXCreateRenderToEnvMap(r3dRenderer->pd3ddev, size_, 0, fmt, TRUE, D3DFMT_D16, &m_pRenderToEnvMap);

  // create cubemap
  if(FAILED( D3DXCreateCubeTexture(r3dRenderer->pd3ddev, size_, 1, D3DUSAGE_RENDERTARGET, fmt, D3DPOOL_DEFAULT, &m_pCubeMap))) {
    if(FAILED( D3DXCreateCubeTexture(r3dRenderer->pd3ddev, size_, 1, 0, fmt, D3DPOOL_DEFAULT, &m_pCubeMap ))) {
      m_pCubeMap = NULL;
      return;
    }
  }

  SetD3DResourcePrivateData(m_pCubeMap, "r3dCubeMap");

  r3dRenderer->Stats.AddRenderTargetMem ( +r3dGetPixelSize( size_* size_ * 6, fmt ) );

  return;
}

void r3dCubeMap::D3DReleaseResource()
{
  R3D_ENSURE_MAIN_THREAD();

  SAFE_RELEASE(m_pCubeMap);
  SAFE_RELEASE(m_pRenderToEnvMap);

  D3DFORMAT fmt = r3dRenderer->d3dpp.BackBufferFormat;
  r3dRenderer->Stats.AddRenderTargetMem ( -(int)r3dGetPixelSize( size_* size_ * 6, fmt ) );
}


void r3dCubeMap::GetCubeMapViewMatrix(D3DXMATRIX *m, D3DCUBEMAP_FACES face)
{
	D3DXVECTOR3 vEyePt   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vLookDir;
	D3DXVECTOR3 vUpDir;

  switch(face)
  {
    case D3DCUBEMAP_FACE_POSITIVE_X:
      vLookDir = D3DXVECTOR3( 1.0f, 0.0f, 0.0f);
      vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f);
      break;
    case D3DCUBEMAP_FACE_NEGATIVE_X:
      vLookDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
      vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f);
      break;
    case D3DCUBEMAP_FACE_POSITIVE_Y:
      vLookDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f);
      vUpDir   = D3DXVECTOR3( 0.0f, 0.0f,-1.0f);
      break;
    case D3DCUBEMAP_FACE_NEGATIVE_Y:
      vLookDir = D3DXVECTOR3( 0.0f,-1.0f, 0.0f);
      vUpDir   = D3DXVECTOR3( 0.0f, 0.0f, 1.0f);
      break;
    case D3DCUBEMAP_FACE_POSITIVE_Z:
      vLookDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f);
      vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f);
      break;
    case D3DCUBEMAP_FACE_NEGATIVE_Z:
      vLookDir = D3DXVECTOR3( 0.0f, 0.0f,-1.0f);
      vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f);
      break;
  }

  // Set the view transform for this cubemap surface
  D3DXMatrixLookAtLH(m, &vEyePt, &vLookDir, &vUpDir );
  return;
}


void r3dCubeMap::GetCubeMapViewDir(D3DXVECTOR3 *vLookDir, D3DCUBEMAP_FACES face)
{
  switch(face)
  {
    case D3DCUBEMAP_FACE_POSITIVE_X:
      *vLookDir = D3DXVECTOR3( 1.0f, 0.0f, 0.0f);
      break;
    case D3DCUBEMAP_FACE_NEGATIVE_X:
      *vLookDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
      break;
    case D3DCUBEMAP_FACE_POSITIVE_Y:
      *vLookDir = D3DXVECTOR3( 0.01f, 0.99f, 0.0f);
      break;
    case D3DCUBEMAP_FACE_NEGATIVE_Y:
      *vLookDir = D3DXVECTOR3( 0.01f,-0.99f, 0.0f);
      break;
    case D3DCUBEMAP_FACE_POSITIVE_Z:
      *vLookDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f);
      break;
    case D3DCUBEMAP_FACE_NEGATIVE_Z:
      *vLookDir = D3DXVECTOR3( 0.0f, 0.0f,-1.0f);
      break;
  }

  return;
}



int r3dCubeMap::Render(const r3dPoint3D& pos, r3dCamera &Cam1, fn_DrawWorld draw)
{
  if(!m_pCubeMap || !m_pRenderToEnvMap)
    return 0;

  struct EnableDisableDistanceCull
  {
    EnableDisableDistanceCull()
    {
      oldValue = r_allow_distance_cull->GetInt();
      r_allow_distance_cull->SetInt( 0 );
    }

    ~EnableDisableDistanceCull()
    {
      r_allow_distance_cull->SetInt( oldValue );
    }

    int oldValue;

  } enableDisableDistanceCull; (void)enableDisableDistanceCull;

  D3DXMATRIX mWorld1, 
			mView1 = r3dRenderer->ViewMatrix , 
			mProj1 = r3dRenderer->ProjMatrix ;

  D3DXMatrixIdentity( &mWorld1 ) ;

  HRESULT hr;
  hr = m_pRenderToEnvMap->BeginCube(m_pCubeMap);
  if(FAILED(hr))
    return 0;

  for(int face = 0; face < 6; face++) {
    m_pRenderToEnvMap->Face((D3DCUBEMAP_FACES)face, D3DX_FILTER_LINEAR);
    r3dRenderer->pd3ddev->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, r3dRenderer->Fog.Color.GetPacked(), r3dRenderer->GetClearZValue(), 0 );

    D3DXMATRIX mWorld, mView, mProj;

    D3DXVECTOR3 vDir;
    GetCubeMapViewDir(&vDir, (D3DCUBEMAP_FACES)face);

    float oldfov = Cam1.FOV;
    Cam1.FOV = 90;
    Cam1.SetPosition(pos);
    Cam1.PointTo(r3dPoint3D(pos.x + vDir.x, pos.y + vDir.y, pos.z + vDir.z));
    r3dRenderer->SetCamera(Cam1, false);

    draw();
    Cam1.FOV = oldfov;
  }

  m_pRenderToEnvMap->End(D3DX_FILTER_LINEAR);

#ifndef FINAL_BUILD
  r3dRenderer->pd3ddev->SetTransform(D3DTS_WORLD,      &mWorld1);
  r3dRenderer->pd3ddev->SetTransform(D3DTS_PROJECTION, &mProj1);
  r3dRenderer->pd3ddev->SetTransform(D3DTS_VIEW,       &mView1);
#endif

  return 1;
}

#if 0
int r3dCubeMap::Load(const char* FName)
{
  if(isDynamic) SAFE_RELEASE(m_pRenderToEnvMap);
  SAFE_RELEASE(m_pCubeMap);

  isDynamic = 0;

/*
  FAILED( D3DXCreateTextureFromFile(r3dRenderer->pd3ddev, FName, m_pCubeMap))
  {
      m_pCubeMap = NULL;
      return 0;
  }
*/
  return 0;
}
#endif

int r3dCubeMap::Save(const char* fname)
{
  D3DXSaveTextureToFile(fname, D3DXIFF_DDS, m_pCubeMap, NULL);
  return 1;
}

//------------------------------------------------------------------------

void RenderCubeFaceMipChain( r3dScreenBuffer* cube, int face, r3dScreenBuffer* temp0, r3dScreenBuffer* temp1 ) 
{
	if( cube->ActualNumMipLevels < 2 )
	{
		r3dOutToLog( "RenderMipChain: called for r3dScreenBuffer with only one mip!\n" ) ;
		return ;
	}

	r3d_assert( temp0->Width >= cube->Width ) ;
	r3d_assert( temp0->Height >= cube->Height ) ;

	r3d_assert( temp1->Width >= cube->Width / 2 ) ;
	r3d_assert( temp1->Height >= cube->Height / 2 ) ;

	struct PerfEventsAndStates
	{
		PerfEventsAndStates()
		{	
			D3DPERF_BeginEvent( 0, L"RenderCubeFaceMipChain") ;

			D3D_V( r3dRenderer->pd3ddev->GetRenderState( D3DRS_SCISSORTESTENABLE, & prevScissorEnable ) ) ;

			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) ) ;

			r3dRenderer->SetCullMode( D3DCULL_NONE ) ;
			
		}

		~PerfEventsAndStates()	
		{	
			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, prevScissorEnable ) ) ;

			r3dRenderer->RestoreCullMode() ;

			D3DPERF_EndEvent() ;
		}

		DWORD prevScissorEnable ;

	} perfEventsAndStates ; (void)perfEventsAndStates ;

	r3dSetRestoreFSQuadVDecl setRestoreVDecl ; (void) setRestoreVDecl ;

	r3dRenderer->SetVertexShader( "VS_POSTFX_DEFAULT" );
	r3dRenderer->SetPixelShader( "PS_FSCOPY" );

	for( int i = 0, e = 6 ; i < e ; i ++ )
	{
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP ) ) ;
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( i, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP ) ) ;
	}

	// copy 0 mip
	{
		cube->Activate( 0, face, 0 ) ;

		r3dRenderer->SetTex( temp0->Tex ) ;
		r3dSetFiltering( R3D_POINT, 0 ) ;

		const r3dScreenBuffer::Dims& dims = cube->MipDims[ 0 ] ;

		// float4 		g_vTexcTransform	: register ( c36 );
		float vsConst[ 4 ] = { dims.Width / temp0->Width , dims.Height / temp0->Height, 0.5f / temp0->Width, 0.5f / temp0->Height } ;
		D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 36, vsConst, 1 ) ) ;

		r3dDrawFullScreenQuad( false );

		cube->Deactivate() ;
	}

	for( int i = 1, e = cube->ActualNumMipLevels ; i < e ; i ++ )
	{
		const r3dScreenBuffer::Dims& prevDims = cube->MipDims[ i - 1 ] ;
		const r3dScreenBuffer::Dims& dims = cube->MipDims[ i ] ;

		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_MAXMIPLEVEL, 0 ) ) ;
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_MIPMAPLODBIAS, 0 ) ) ;

		temp1->Activate() ;

		r3dRenderer->SetTex( temp0->Tex ) ;

		r3dSetFiltering( R3D_BILINEAR, 0 ) ;
		D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT ) ) ;

		// float4 		g_vTexcTransform	: register ( c36 );
		float vsConst[ 4 ] = { prevDims.Width / temp0->Width , prevDims.Height / temp0->Height, 1.0f / temp0->Width, 1.0f / temp0->Height } ;
		D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 36, vsConst, 1 ) ) ;

		r3dRenderer->SetViewport( 0, 0, dims.Width, dims.Height ) ;

		r3dDrawFullScreenQuad( false );

		temp1->Deactivate() ;

		// copy mip
		{
			cube->Activate( 0, face, i ) ;

			r3dRenderer->SetTex( temp1->Tex ) ;
			r3dSetFiltering( R3D_POINT, 0 ) ;

			// float4 		g_vTexcTransform	: register ( c36 );
			float vsConst[ 4 ] = { dims.Width / temp1->Width , dims.Height / temp1->Height, 0.5f / temp1->Width, 0.5f / temp1->Height } ;
			D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 36, vsConst, 1 ) ) ;

			r3dDrawFullScreenQuad( false );

			cube->Deactivate() ;
		}

		R3D_SWAP( temp0, temp1 ) ;
	}

	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_MIPMAPLODBIAS, 0 ) ) ;
	// to override our mip filter hack
	r3dSetFiltering( R3D_POINT, 0 ) ;

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();

}

//------------------------------------------------------------------------

void RenderCubeMapMipChainWithEdgeSmoothing( r3dScreenBuffer* cube, r3dScreenBuffer* temp0, r3dScreenBuffer* (&faces)[ 6 ] )
{
	if( cube->ActualNumMipLevels < 2 )
	{
		r3dOutToLog( "RenderMipChain: called for r3dScreenBuffer with only one mip!\n" ) ;
		return ;
	}

	r3d_assert( temp0->Width >= cube->Width ) ;
	r3d_assert( temp0->Height >= cube->Height ) ;

	struct PerfEventsAndStates
	{
		PerfEventsAndStates()
		{	
			D3DPERF_BeginEvent( 0, L"RenderCubeMapMipChainWithEdgeSmoothing") ;

			D3D_V( r3dRenderer->pd3ddev->GetRenderState( D3DRS_SCISSORTESTENABLE, & prevScissorEnable ) ) ;

			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) ) ;

			r3dRenderer->SetCullMode( D3DCULL_NONE ) ;

		}

		~PerfEventsAndStates()	
		{	
			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, prevScissorEnable ) ) ;

			r3dRenderer->RestoreCullMode() ;

			D3DPERF_EndEvent() ;
		}

		DWORD prevScissorEnable;

	} perfEventsAndStates; (void)perfEventsAndStates;

	r3dSetRestoreFSQuadVDecl setRestoreVDecl; (void) setRestoreVDecl;

	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_MAXMIPLEVEL, 0 ) );
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState( 0, D3DSAMP_MIPMAPLODBIAS, 0 ) );

	//------------------------------------------------------------------------

	for( int i = 0, e = cube->ActualNumMipLevels ; i < e ; i ++ )
	{
		struct BeginEndMip
		{
			BeginEndMip( int mip )
			{
				wchar_t msg[ 64 ];
				wsprintfW( msg, L"Mip %d", mip );
				D3DPERF_BeginEvent( 0, msg );
			}

			~BeginEndMip()
			{
				D3DPERF_EndEvent();
			}
		} beginEndMip( i ); ( void )beginEndMip;

		const r3dScreenBuffer::Dims& dims = cube->MipDims[ i ] ;

		enum Edge
		{
			EDGE_LEFT,
			EDGE_RIGHT,
			EDGE_TOP,
			EDGE_BOTTOM
		};

		enum Point
		{
			POINT_TOP_LEFT,
			POINT_TOP_RIGHT,
			POINT_BOTTOM_LEFT,
			POINT_BOTTOM_RIGHT
		};

		enum Face
		{
			F_XP,
			F_XN,
			F_YP,
			F_YN,
			F_ZP,
			F_ZN
		};

		struct Blender
		{
			void FillPointTexXfm( Point p, float *constant )
			{
				float HHALF_PIXEL = 0.5f / RTWIDTH;
				float VHALF_PIXEL = 0.5f / RTHEIGHT;

				switch( p )
				{
				case POINT_TOP_LEFT:
					constant[ 0 ] = HORIZ_MULT;
					constant[ 1 ] = VERT_MULT;
					constant[ 2 ] = HHALF_PIXEL + LEFT_OFFSET;
					constant[ 3 ] = VHALF_PIXEL + TOP_OFFSET;
					break;
				case POINT_TOP_RIGHT:
					constant[ 0 ] = HORIZ_MULT;
					constant[ 1 ] = VERT_MULT;
					constant[ 2 ] = HHALF_PIXEL + RIGHT_OFFSET;
					constant[ 3 ] = VHALF_PIXEL + TOP_OFFSET;
					break;
				case POINT_BOTTOM_LEFT:
					constant[ 0 ] = HORIZ_MULT;
					constant[ 1 ] = VERT_MULT;
					constant[ 2 ] = HHALF_PIXEL + LEFT_OFFSET;
					constant[ 3 ] = VHALF_PIXEL + BOTTOM_OFFSET;
					break;
				case POINT_BOTTOM_RIGHT:
					constant[ 0 ] = HORIZ_MULT;
					constant[ 1 ] = VERT_MULT;
					constant[ 2 ] = HHALF_PIXEL + RIGHT_OFFSET;
					constant[ 3 ] = VHALF_PIXEL + BOTTOM_OFFSET;
					break;
				}
			}

			void FillPointPosXfm( Point p, float* constant )
			{
				switch( p )
				{
				case POINT_TOP_LEFT:
					constant[ 0 ] = P_HORIZ_MULT;
					constant[ 1 ] = P_VERT_MULT;
					constant[ 2 ] = P_LEFT_OFFSET;
					constant[ 3 ] = P_TOP_OFFSET;
					break;
				case POINT_TOP_RIGHT:
					constant[ 0 ] = P_HORIZ_MULT;
					constant[ 1 ] = P_VERT_MULT;
					constant[ 2 ] = P_RIGHT_OFFSET;
					constant[ 3 ] = P_TOP_OFFSET;
					break;
				case POINT_BOTTOM_LEFT:
					constant[ 0 ] = P_HORIZ_MULT;
					constant[ 1 ] = P_VERT_MULT;
					constant[ 2 ] = P_LEFT_OFFSET;
					constant[ 3 ] = P_BOTTOM_OFFSET;
					break;
				case POINT_BOTTOM_RIGHT:
					constant[ 0 ] = P_HORIZ_MULT;
					constant[ 1 ] = P_VERT_MULT;
					constant[ 2 ] = P_RIGHT_OFFSET;
					constant[ 3 ] = P_BOTTOM_OFFSET;
					break;
				}
			}

			void FillEdgeTexXfm( Edge e, float inv, float *constant )
			{
				float addCoef = R3D_MAX( -inv, 0.f );

				float HHALF_PIXEL = 0.5f / RTWIDTH;
				float VHALF_PIXEL = 0.5f / RTHEIGHT;

				switch( e )
				{
				case EDGE_LEFT:
					constant[ 0 ] = HORIZ_MULT;
					constant[ 1 ] = V_SCALE_COEF * inv;
					constant[ 2 ] = HHALF_PIXEL + LEFT_OFFSET;
					constant[ 3 ] = VHALF_PIXEL + ( V_SCALE_COEF - 1.0f / RTHEIGHT ) * addCoef;
					break;
				case EDGE_RIGHT:
					constant[ 0 ] = HORIZ_MULT;
					constant[ 1 ] = V_SCALE_COEF * inv;
					constant[ 2 ] = HHALF_PIXEL + RIGHT_OFFSET;
					constant[ 3 ] = VHALF_PIXEL + ( V_SCALE_COEF - 1.0f / RTHEIGHT ) * addCoef;
					break;
				case EDGE_TOP:
					constant[ 0 ] = H_SCALE_COEF * inv;
					constant[ 1 ] = VERT_MULT;
					constant[ 2 ] = HHALF_PIXEL + ( H_SCALE_COEF - 1.0f / RTWIDTH ) * addCoef;
					constant[ 3 ] = VHALF_PIXEL + TOP_OFFSET;
					break;
				case EDGE_BOTTOM:
					constant[ 0 ] = H_SCALE_COEF * inv;
					constant[ 1 ] = VERT_MULT;
					constant[ 2 ] = HHALF_PIXEL + ( H_SCALE_COEF - 1.0f / RTWIDTH ) * addCoef;
					constant[ 3 ] = VHALF_PIXEL + BOTTOM_OFFSET;
					break;
				}
			}

			void FillEdgePosXfm( Edge e, float* constant )
			{
				switch( e )
				{
				case EDGE_LEFT:
					constant[ 0 ] = P_HORIZ_MULT;
					constant[ 1 ] = 1.0f;
					constant[ 2 ] = P_LEFT_OFFSET;
					constant[ 3 ] = 0.0f;
					break;
				case EDGE_RIGHT:
					constant[ 0 ] = P_HORIZ_MULT;
					constant[ 1 ] = 1.0f;
					constant[ 2 ] = P_RIGHT_OFFSET;
					constant[ 3 ] = 0.0f;
					break;
				case EDGE_TOP:
					constant[ 0 ] = 1.0f;
					constant[ 1 ] = P_VERT_MULT;
					constant[ 2 ] = 0.0f;
					constant[ 3 ] = P_TOP_OFFSET;
					break;
				case EDGE_BOTTOM:
					constant[ 0 ] = 1.0f;
					constant[ 1 ] = P_VERT_MULT;
					constant[ 2 ] = 0.0f;
					constant[ 3 ] = P_BOTTOM_OFFSET;
					break;
				}
			}

			void FillUnusedTexXfm( float* constant )
			{
				constant[ 0 ] = 1.0f;
				constant[ 1 ] = 1.0f;
				constant[ 2 ] = 9.0f;
				constant[ 3 ] = 9.0f;
			}

			void BlendEdge( r3dScreenBuffer** faces, Face f1, Face f2, Edge e1, Edge e2, float e2_inv, int swap )
			{
				float vsConsts[ 5 ][ 4 ];

				// float4 TexXfm[ 3 ] : register( c0 );
				FillEdgeTexXfm( e1, 1.0f, vsConsts[ 0 ] );
				FillEdgeTexXfm( e2, e2_inv, vsConsts[ 1 ] );
				FillUnusedTexXfm( vsConsts[ 2 ] );
				// float4 PosXfm : register( c3 );
				FillEdgePosXfm( e1, vsConsts[ 3 ] );

				memset( vsConsts[ 4 ], 0, sizeof vsConsts[ 4 ] );

				if( swap )
				{
					vsConsts[ 4 ][ 1 ] = 1.0f;
				}

				D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, vsConsts[ 0 ], R3D_ARRAYSIZE(vsConsts ) ) );

				r3dRenderer->SetTex( faces[ f1 ]->Tex, 0 );
				r3dRenderer->SetTex( faces[ f2 ]->Tex, 1 );

				r3dDrawFullScreenQuad( false );	
			}

			void BlendPoint( r3dScreenBuffer** faces, Face f1, Face f2, Face f3, Point p1, Point p2, Point p3 )
			{
				float vsConsts[ 5 ][ 4 ];

				// float4 TexXfm[ 3 ] : register( c0 );
				FillPointTexXfm( p1, vsConsts[ 0 ] );
				FillPointTexXfm( p2, vsConsts[ 1 ] );
				FillPointTexXfm( p3, vsConsts[ 2 ] );
				// float4 PosXfm : register( c3 );
				FillPointPosXfm( p1, vsConsts[ 3 ] );

				memset( vsConsts[ 4 ], 0, sizeof vsConsts[ 4 ] );

				D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, vsConsts[ 0 ], R3D_ARRAYSIZE(vsConsts ) ) );

				r3dRenderer->SetTex( faces[ f1 ]->Tex, 0 );
				r3dRenderer->SetTex( faces[ f2 ]->Tex, 1 );
				r3dRenderer->SetTex( faces[ f3 ]->Tex, 2 );

				r3dDrawFullScreenQuad( false );	
			}

			void Blend1PixelMip( r3dScreenBuffer** faces )
			{
				float vsConsts[ 5 ][ 4 ];

				// float4 TexXfm[ 3 ] : register( c0 );
				vsConsts[ 0 ][ 0 ] = 0.f;
				vsConsts[ 0 ][ 1 ] = 0.f;
				vsConsts[ 0 ][ 2 ] = 0.5f / RTWIDTH;
				vsConsts[ 0 ][ 3 ] = 0.5f / RTHEIGHT;

				FillUnusedTexXfm( vsConsts[ 1 ] );
				FillUnusedTexXfm( vsConsts[ 2 ] );
				// float4 PosXfm : register( c3 );
				vsConsts[ 3 ][ 0 ] = 1.0f;
				vsConsts[ 3 ][ 1 ] = 1.0f;
				vsConsts[ 3 ][ 2 ] = 0.0f;
				vsConsts[ 3 ][ 3 ] = 0.0f;

				memset( vsConsts[ 4 ], 0, sizeof vsConsts[ 4 ] );

				D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, vsConsts[ 0 ], R3D_ARRAYSIZE(vsConsts ) ) );

				for( int i = 0; i < 6; i ++ )
				{
					r3dSetFiltering( R3D_POINT, i );
					r3dRenderer->SetTex( faces[ i ]->Tex, i );
				}

				r3dDrawFullScreenQuad( false );	
			}

			void CopyFace( r3dScreenBuffer* src, float srcAreaWidth, float srcAreaHeight )
			{				
				r3dRenderer->SetVertexShader( "VS_POSTFX_DEFAULT" );
				r3dRenderer->SetPixelShader( "PS_FSCOPY" );

				r3dRenderer->SetViewport( 0, 0, srcAreaWidth, srcAreaHeight );

				r3dRenderer->SetTex( src->Tex );
				r3dSetFiltering( R3D_POINT, 0 );

				// float4 		g_vTexcTransform	: register ( c36 );
				float vsConst[ 4 ] = { srcAreaWidth / src->Width, srcAreaHeight / src->Height, 0.5f / src->Width, 0.5f / src->Height };
				D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 36, vsConst, 1 ) );

				r3dDrawFullScreenQuad( false );	
			}

			void StartBlending( int isOnePixelMip )
			{
				r3dRenderer->SetViewport( 0, 0, VIEW_WIDTH, VIEW_HEIGHT ) ;

				r3dRenderer->SetVertexShader( "VS_REMOVECUBEMAPEDGE" );

				if( isOnePixelMip )
					r3dRenderer->SetPixelShader( "PS_REMOVECUBEMAPEDGE_ONE_PIXEL_MIP" );
				else
					r3dRenderer->SetPixelShader( "PS_REMOVECUBEMAPEDGE" );

				r3dRenderer->SetTex( NULL, 2 );				

				r3dSetFiltering( R3D_POINT, 0 );
				r3dSetFiltering( R3D_POINT, 1 );
				r3dSetFiltering( R3D_POINT, 2 );
			}

			void CopyToCubeFace( r3dScreenBuffer* cube, r3dScreenBuffer* src, float ViewW, float ViewH, Face face, int mip )
			{
				struct D3DBrackets
				{
					D3DBrackets()
					{
						D3DPERF_BeginEvent( 0, L"CopyToCubeFace" );
					}

					~D3DBrackets()
					{
						D3DPERF_EndEvent();
					}
				} d3dBrackets; (void)d3dBrackets;


				cube->Activate( 0, face, mip );

				r3dRenderer->SetVertexShader( "VS_POSTFX_DEFAULT" );
				r3dRenderer->SetPixelShader( "PS_FSCOPY" );

				r3dRenderer->SetTex( src->Tex ) ;
				r3dSetFiltering( R3D_POINT, 0 ) ;

				// float4 		g_vTexcTransform	: register ( c36 );
				float vsConst[ 4 ] = { ViewW / src->Width, ViewH / src->Height, 0.5f / src->Width, 0.5f / src->Height } ;
				D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 36, vsConst, 1 ) ) ;

				r3dDrawFullScreenQuad( false );	

				cube->Deactivate();
			}

			const float BOTTOM_OFFSET;
			const float TOP_OFFSET;
			const float LEFT_OFFSET;
			const float RIGHT_OFFSET;

			const float P_LEFT_OFFSET;
			const float P_RIGHT_OFFSET;

			const float P_TOP_OFFSET;
			const float P_BOTTOM_OFFSET;

			const float HORIZ_MULT;
			const float VERT_MULT;

			const float P_HORIZ_MULT;
			const float P_VERT_MULT;

			const float VIEW_WIDTH;
			const float VIEW_HEIGHT;

			const float RTWIDTH;
			const float RTHEIGHT;

			const float H_SCALE_COEF;
			const float V_SCALE_COEF;

			Blender( float ViewWidth, float ViewHeight, float RTWidth, float RTHeight )
			: BOTTOM_OFFSET( ( ViewHeight - 1.0f ) / RTHeight )
			, TOP_OFFSET( 0.f )
			, LEFT_OFFSET( 0.0f )
			, RIGHT_OFFSET( ( ViewWidth - 1.0f ) / RTWidth )
			, P_LEFT_OFFSET( - ( ViewWidth - 1.0f ) / ViewWidth )
			, P_RIGHT_OFFSET( + ( ViewWidth - 1.0f ) / ViewWidth )
			, P_TOP_OFFSET ( + ( ViewHeight - 1.0f ) / ViewHeight )
			, P_BOTTOM_OFFSET( - ( ViewHeight - 1.0f ) / ViewHeight )
			, HORIZ_MULT( 1.0f / RTWidth )
			, VERT_MULT( -1.0f / RTHeight )
			, P_HORIZ_MULT( 1.0f / ViewWidth )
			, P_VERT_MULT( 1.0f / ViewHeight )
			, RTWIDTH( RTWidth )
			, RTHEIGHT( RTHeight )
			, VIEW_WIDTH( ViewWidth )
			, VIEW_HEIGHT( ViewHeight )
			, H_SCALE_COEF( ViewWidth / RTWidth )
			, V_SCALE_COEF( ViewHeight / RTHeight )
			{

			}
		} blender( dims.Width, dims.Height, faces[ 0 ]->Width, faces[ 0 ]->Height );

		int onePixelMip = dims.Width == 1 && dims.Height == 1;

		//------------------------------------------------------------------------
		// X +
		{
			temp0->Activate( 0 );

			const Face TARGET = F_XP;

			// copy face
			blender.CopyFace( faces[ TARGET ], dims.Width, dims.Height );
			blender.StartBlending( onePixelMip );

			if( onePixelMip )
			{
				blender.Blend1PixelMip( faces );
			}
			else
			{
				// blend X+ right & Z- left
				blender.BlendEdge( faces, TARGET, F_ZN, EDGE_RIGHT, EDGE_LEFT, 1.0f, 0 );
				// blend X+ left & Z+ right
				blender.BlendEdge( faces, TARGET, F_ZP, EDGE_LEFT, EDGE_RIGHT, 1.0f, 0 );
				// blend X+ top & Y+ right
				blender.BlendEdge( faces, TARGET, F_YP, EDGE_TOP, EDGE_RIGHT, -1.0f, 1 );
				// blend X+ bottom & Y- right
				blender.BlendEdge( faces, TARGET, F_YN, EDGE_BOTTOM, EDGE_RIGHT, 1.0f, 1 );

				// blend X+ left bottom
				// && Y- right top
				// && Z+ right bottom
				blender.BlendPoint( faces, TARGET, F_YN, F_ZP, POINT_BOTTOM_LEFT, POINT_TOP_RIGHT, POINT_BOTTOM_RIGHT );

				// blend X+ right bottom
				// && Y- right bottom
				// && Z- left bottom
				blender.BlendPoint( faces, TARGET, F_YN, F_ZN, POINT_BOTTOM_RIGHT, POINT_BOTTOM_RIGHT, POINT_BOTTOM_LEFT );

				// blend X+ left top
				// && Y+ right bottom
				// && Z+ right top
				blender.BlendPoint( faces, TARGET, F_YP, F_ZP, POINT_TOP_LEFT, POINT_BOTTOM_RIGHT, POINT_TOP_RIGHT );

				// blend X+ right top
				// && Y+ right top
				// && Z- left top
				blender.BlendPoint( faces, TARGET, F_YP, F_ZN, POINT_TOP_RIGHT, POINT_TOP_RIGHT, POINT_TOP_LEFT );
			}

			temp0->Deactivate();

			blender.CopyToCubeFace( cube, temp0, dims.Width, dims.Height, TARGET, i );
		}

		//------------------------------------------------------------------------
		// X -
		{
			temp0->Activate( 0 );

			const Face TARGET = F_XN;

			// copy face
			blender.CopyFace( faces[ TARGET ], dims.Width, dims.Height );
			blender.StartBlending( onePixelMip );

			if( onePixelMip )
			{
				blender.Blend1PixelMip( faces );
			}
			else
			{
				// blend X- right & Z+ left
				blender.BlendEdge( faces, TARGET, F_ZP, EDGE_RIGHT, EDGE_LEFT, 1.0f, 0 );
				// blend X- left & Z- right
				blender.BlendEdge( faces, TARGET, F_ZN, EDGE_LEFT, EDGE_RIGHT, 1.0f, 0 );
				// blend X- top & Y+ left
				blender.BlendEdge( faces, TARGET, F_YP, EDGE_TOP, EDGE_LEFT, 1.0f, 1 );
				// blend X- bottom & Y- left
				blender.BlendEdge( faces, TARGET, F_YN, EDGE_BOTTOM, EDGE_LEFT, -1.0f, 1 );

				// blend X- left bottom
				// && Y- bottom left
				// && Z- right bottom
				blender.BlendPoint( faces, TARGET, F_YN, F_ZN, POINT_BOTTOM_LEFT, POINT_BOTTOM_LEFT, POINT_BOTTOM_RIGHT );

				// blend X- right bottom
				// && Y- left top
				// && Z+ left bottom
				blender.BlendPoint( faces, TARGET, F_YN, F_ZP, POINT_BOTTOM_RIGHT, POINT_TOP_LEFT, POINT_BOTTOM_LEFT );

				// blend X- left top
				// && Y+ left top
				// && Z- right top
				blender.BlendPoint( faces, TARGET, F_YP, F_ZN, POINT_TOP_LEFT, POINT_TOP_LEFT, POINT_TOP_RIGHT );

				// blend X- right top
				// && Y+ left bottom
				// && Z+ left top
				blender.BlendPoint( faces, TARGET, F_YP, F_ZP, POINT_TOP_RIGHT, POINT_BOTTOM_LEFT, POINT_TOP_LEFT );
			}

			temp0->Deactivate();

			blender.CopyToCubeFace( cube, temp0, dims.Width, dims.Height, TARGET, i );
		}

		//------------------------------------------------------------------------
		// Y +
		{
			temp0->Activate( 0 );

			const Face TARGET = F_YP;

			// copy face

			blender.CopyFace( faces[ TARGET ], dims.Width, dims.Height );
			blender.StartBlending( onePixelMip );

			if( onePixelMip )
			{
				blender.Blend1PixelMip( faces );
			}
			else
			{
				// blend Y+ right & X+ top
				blender.BlendEdge( faces, TARGET, F_XP, EDGE_RIGHT, EDGE_TOP, -1.0f, 1 );
				// blend Y+ left & X- top
				blender.BlendEdge( faces, TARGET, F_XN, EDGE_LEFT, EDGE_TOP, 1.0f, 1 );
				// blend Y+ top & Z- top
				blender.BlendEdge( faces, TARGET, F_ZN, EDGE_TOP, EDGE_TOP, -1.0f, 0 );
				// blend Y+ bottom & Z+ top
				blender.BlendEdge( faces, TARGET, F_ZP, EDGE_BOTTOM, EDGE_TOP, 1.0f, 0 );

				// blend Y+ bottom left
				// && X- top right
				// && Z+ top left
				blender.BlendPoint( faces, TARGET, F_XN, F_ZP, POINT_BOTTOM_LEFT, POINT_TOP_RIGHT, POINT_TOP_LEFT );

				// blend Y+ bottom right
				// && X+ top left
				// && Z+ top right
				blender.BlendPoint( faces, TARGET, F_XP, F_ZP, POINT_BOTTOM_RIGHT, POINT_TOP_LEFT, POINT_TOP_RIGHT );

				// blend Y+ top left
				// && X- top left
				// && Z- top right
				blender.BlendPoint( faces, TARGET, F_XN, F_ZN, POINT_TOP_LEFT, POINT_TOP_LEFT, POINT_TOP_RIGHT );

				// blend Y+ top right
				// && X+ top right
				// && Z- top left
				blender.BlendPoint( faces, TARGET, F_XP, F_ZN, POINT_TOP_RIGHT, POINT_TOP_RIGHT, POINT_TOP_LEFT );
			}

			temp0->Deactivate();

			blender.CopyToCubeFace( cube, temp0, dims.Width, dims.Height, TARGET, i );
		}

		//------------------------------------------------------------------------
		// Y -
		{
			temp0->Activate( 0 );

			const Face TARGET = F_YN;

			// copy face

			blender.CopyFace( faces[ TARGET ], dims.Width, dims.Height );
			blender.StartBlending( onePixelMip );

			if( onePixelMip )
			{
				blender.Blend1PixelMip( faces );
			}
			else
			{
				// blend Y- right & X+ bottom
				blender.BlendEdge( faces, TARGET, F_XP, EDGE_RIGHT, EDGE_BOTTOM, 1.0f, 1 );
				// blend Y- left & X- bottom
				blender.BlendEdge( faces, TARGET, F_XN, EDGE_LEFT, EDGE_BOTTOM, -1.0f, 1 );
				// blend Y- top & Z+ bottom
				blender.BlendEdge( faces, TARGET, F_ZP, EDGE_TOP, EDGE_BOTTOM, 1.0f, 0 );
				// blend Y- bottom & Z- bottom
				blender.BlendEdge( faces, TARGET, F_ZN, EDGE_BOTTOM, EDGE_BOTTOM, -1.0f, 0 );

				// blend Y- bottom left
				// && X- bottom left
				// && Z- bottom right
				blender.BlendPoint( faces, TARGET, F_XN, F_ZN, POINT_BOTTOM_LEFT, POINT_BOTTOM_LEFT, POINT_BOTTOM_RIGHT );

				// blend Y- bottom right
				// && X+ bottom right
				// && Z- bottom left
				blender.BlendPoint( faces, TARGET, F_XP, F_ZN, POINT_BOTTOM_RIGHT, POINT_BOTTOM_RIGHT, POINT_BOTTOM_LEFT );

				// blend Y- top left
				// && X- bottom right
				// && Z+ bottom left
				blender.BlendPoint( faces, TARGET, F_XN, F_ZP, POINT_TOP_LEFT, POINT_BOTTOM_RIGHT, POINT_BOTTOM_LEFT );

				// blend Y- top right
				// && X+ bottom left
				// && Z+ bottom right
				blender.BlendPoint( faces, TARGET, F_XP, F_ZP, POINT_TOP_RIGHT, POINT_BOTTOM_LEFT, POINT_BOTTOM_RIGHT );
			}

			temp0->Deactivate();

			blender.CopyToCubeFace( cube, temp0, dims.Width, dims.Height, TARGET, i );
		}

		//------------------------------------------------------------------------
		// Z +
		{
			temp0->Activate( 0 );

			const Face TARGET = F_ZP;

			// copy face

			blender.CopyFace( faces[ TARGET ], dims.Width, dims.Height );
			blender.StartBlending( onePixelMip );

			if( onePixelMip )
			{
				blender.Blend1PixelMip( faces );
			}
			else
			{
				// blend Z+ right & X+ left
				blender.BlendEdge( faces, TARGET, F_XP, EDGE_RIGHT, EDGE_LEFT, 1.0f, 0 );
				// blend Z+ left & X- right
				blender.BlendEdge( faces, TARGET, F_XN, EDGE_LEFT, EDGE_RIGHT, 1.0f, 0 );
				// blend Z+ top & Y+ bottom
				blender.BlendEdge( faces, TARGET, F_YP, EDGE_TOP, EDGE_BOTTOM, 1.0f, 0 );
				// blend Z+ bottom & Y- top
				blender.BlendEdge( faces, TARGET, F_YN, EDGE_BOTTOM, EDGE_TOP, 1.0f, 0 );

				// blend Z+ bottom left
				// && X- bottom right
				// && Y- top left
				blender.BlendPoint( faces, TARGET, F_XN, F_YN, POINT_BOTTOM_LEFT, POINT_BOTTOM_RIGHT, POINT_TOP_LEFT );

				// blend Z+ bottom right
				// && X+ bottom left
				// && Y- top right
				blender.BlendPoint( faces, TARGET, F_XP, F_YN, POINT_BOTTOM_RIGHT, POINT_BOTTOM_LEFT, POINT_TOP_RIGHT );

				// blend Z+ top left
				// && X- top right
				// && Y+ bottom left
				blender.BlendPoint( faces, TARGET, F_XN, F_YP, POINT_TOP_LEFT, POINT_TOP_RIGHT, POINT_BOTTOM_LEFT );

				// blend Z+ top right
				// && X+ top left
				// && Y+ bottom right
				blender.BlendPoint( faces, TARGET, F_XP, F_YP, POINT_TOP_RIGHT, POINT_TOP_LEFT, POINT_BOTTOM_RIGHT );
			}

			temp0->Deactivate();

			blender.CopyToCubeFace( cube, temp0, dims.Width, dims.Height, TARGET, i );
		}

		//------------------------------------------------------------------------
		// Z -
		{
			temp0->Activate( 0 );

			const Face TARGET = F_ZN;

			// copy face

			blender.CopyFace( faces[ TARGET ], dims.Width, dims.Height );
			blender.StartBlending( onePixelMip );

			if( onePixelMip )
			{
				blender.Blend1PixelMip( faces );
			}
			else
			{
				// blend Z- right & X- left
				blender.BlendEdge( faces, TARGET, F_XN, EDGE_RIGHT, EDGE_LEFT, 1.0f, 0 );
				// blend Z- left & X+ right
				blender.BlendEdge( faces, TARGET, F_XP, EDGE_LEFT, EDGE_RIGHT, 1.0f, 0 );
				// blend Z- top & Y+ top
				blender.BlendEdge( faces, TARGET, F_YP, EDGE_TOP, EDGE_TOP, -1.0f, 0 );
				// blend Z- bottom & Y- bottom
				blender.BlendEdge( faces, TARGET, F_YN, EDGE_BOTTOM, EDGE_BOTTOM, -1.0f, 0 );

				// blend Z- bottom left
				// && X+ bottom right
				// && Y- bottom right
				blender.BlendPoint( faces, TARGET, F_XP, F_YN, POINT_BOTTOM_LEFT, POINT_BOTTOM_RIGHT, POINT_BOTTOM_RIGHT );

				// blend Z- bottom right
				// && X- bottom left
				// && Y- bottom left
				blender.BlendPoint( faces, TARGET, F_XN, F_YN, POINT_BOTTOM_RIGHT, POINT_BOTTOM_LEFT, POINT_BOTTOM_LEFT );

				// blend Z- top left
				// && X+ top right
				// && Y+ top right
				blender.BlendPoint( faces, TARGET, F_XP, F_YP, POINT_TOP_LEFT, POINT_TOP_RIGHT, POINT_TOP_RIGHT );

				// blend Z- top right
				// && X- top left
				// && Y+ top left
				blender.BlendPoint( faces, TARGET, F_XN, F_YP, POINT_TOP_RIGHT, POINT_TOP_LEFT, POINT_TOP_LEFT );
			}

			temp0->Deactivate();

			blender.CopyToCubeFace( cube, temp0, dims.Width, dims.Height, TARGET, i );
		}

		if( i != e - 1 )
		{
			struct D3DBrackets
			{
				D3DBrackets()
				{
					D3DPERF_BeginEvent( 0, L"Downscale Mip" );
				}

				~D3DBrackets()
				{
					D3DPERF_EndEvent();
				}
			} d3dBrackets; (void)d3dBrackets;

			r3dSetFiltering( R3D_BILINEAR, 0 );

			for( int f = 0, e = 6; f < e; f ++ )
			{
				int nextWidth = R3D_MAX( int( dims.Width / 2 ), 1 );
				int nextHeight = R3D_MAX( int( dims.Height / 2 ), 1 );

				//------------------------------------------------------------------------
				{
					temp0->Activate(0);

					r3dRenderer->SetViewport(	0, 0, 
												(float)nextWidth,
												(float)nextHeight );


					r3dRenderer->SetVertexShader( "VS_POSTFX_DEFAULT" );
					r3dRenderer->SetPixelShader( "PS_FSCOPY" );

					r3dScreenBuffer* src = faces[ f ];

					r3dRenderer->SetTex( src->Tex );
					r3dSetFiltering( R3D_BILINEAR, 0 );

					// float4 		g_vTexcTransform	: register ( c36 );
					float vsConst[ 4 ] = { dims.Width / src->Width, dims.Height / src->Height, 0.5f / src->Width, 0.5f / src->Height };
					D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 36, vsConst, 1 ) );

					r3dDrawFullScreenQuad( false );
					temp0->Deactivate();
				}

				//------------------------------------------------------------------------
				{
					faces[ f ]->Activate();

					r3dRenderer->SetViewport(	0, 0, 
												(float)nextWidth,
												(float)nextHeight );

					// float4 		g_vTexcTransform	: register ( c36 );
					float vsConst[ 4 ] = { nextWidth / temp0->Width, nextHeight / temp0->Height, 0.5f / temp0->Width, 0.5f / temp0->Height };
					D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 36, vsConst, 1 ) );

					r3dSetFiltering( R3D_POINT, 0 );
					r3dRenderer->SetTex( temp0->Tex );

					r3dDrawFullScreenQuad( false );

					faces[ f ]->Deactivate();
				}
			}
		}
	}
}