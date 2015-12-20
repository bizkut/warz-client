#include "r3dPCH.h"

#include "r3d.h"

#include "ObjectsCode/world/MaterialTypes.h"

#include "r3dDeviceQueue.h"

#include "ITerrain.h"

//------------------------------------------------------------------------

r3dTerrainDesc::r3dTerrainDesc()
: LayerCount( 0 )

, XSize( 0.f )
, ZSize( 0.f )

, MinHeight( 0.f )
, MaxHeight( 0.f )

, CellSize( 0 )

, CellCountX( 0 )
, CellCountZ( 0 )

, MaskResolutionU( 0 )
, MaskResolutionV( 0 )

, TileCountX( 0 )
, TileCountZ( 0 )

, CellCountPerTile( 0 )

, WorldSize( 0.f )
, OrthoDiffuseTex( NULL )

{

}

//------------------------------------------------------------------------

void
r3dTerrainDesc::Sync()
{
	XSize	= CellCountX * CellSize ;
	ZSize	= CellCountZ * CellSize ;

	TileCountX = CellCountX / CellCountPerTile ;
	TileCountZ = CellCountZ / CellCountPerTile ;

	WorldSize = R3D_MAX( XSize, ZSize ) ;
}

//------------------------------------------------------------------------

/*static*/ float r3dITerrain::LoadingProgress = 0.f;

//------------------------------------------------------------------------

r3dITerrain::r3dITerrain()
: m_OrthoDiffuseTexDirty( 1 )
{

}

//------------------------------------------------------------------------

r3dITerrain::~r3dITerrain()
{
	if( m_Desc.OrthoDiffuseTex )
	{
		r3dRenderer->DeleteTexture( m_Desc.OrthoDiffuseTex ) ;
	}
}

//------------------------------------------------------------------------
/*virtual*/ int r3dITerrain::IsPosWithinPreciseHeights( const r3dPoint3D& pos ) const
{
	return 1;
}

//------------------------------------------------------------------------

void
r3dITerrain::SetDesc( const r3dTerrainDesc& desc )
{
	r3dTexture* ortho = m_Desc.OrthoDiffuseTex ;

	m_Desc = desc ;
	m_Desc.Sync() ;

	if( !m_Desc.OrthoDiffuseTex )
	{
		m_Desc.OrthoDiffuseTex = ortho ;
	}
}

//------------------------------------------------------------------------

void SetD3DResourcePrivateData(LPDIRECT3DRESOURCE9 res, const char* FName);

struct DrawOrthoTerraParams
{
	int DownScale ;
	r3dITerrain* terra ;
};

static void DoDrawOrthoTerra( void* params )
{
	DrawOrthoTerraParams * pms = ( DrawOrthoTerraParams * ) params ;

	pms->terra->DrawOrthographicDiffuseTexture_MainThread( pms->DownScale ) ;
}

bool
r3dITerrain::DrawOrthographicDiffuseTexture( int DownScale )
{
	DrawOrthoTerraParams parms;

	parms.DownScale = DownScale ;
	parms.terra = this ;

	ProcessCustomDeviceQueueItem( DoDrawOrthoTerra, &parms ) ;

	return true ;
}


//------------------------------------------------------------------------

void
r3dITerrain::DrawOrthographicDiffuseTexture_MainThread( int DownScale )
{
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


	int Width = (int)( m_Desc.XSize / m_Desc.CellSize ) ;
	int Height = (int)( m_Desc.ZSize / m_Desc.CellSize ) ;

	int texWidth	= Width / DownScale;
	int texHeight	= Height / DownScale;

	D3DCAPS9 caps;
	r3dRenderer->pd3ddev->GetDeviceCaps(&caps);

	int SuggestedDim = 4096 ;

	// assume things are bad with vmem
	if( r_texture_quality->GetInt() == 1 )
	{
		SuggestedDim = 2048 ;
	}

	int texFullWidth = std::min<DWORD>( caps.MaxTextureWidth, SuggestedDim );
	int texFullHeight = std::min<DWORD>( caps.MaxTextureHeight, SuggestedDim );

	const D3DFORMAT texFMT = D3DFMT_A8R8G8B8;

	IDirect3DTexture9* tex( NULL );

	r3dScreenBuffer* rtFull = r3dScreenBuffer::CreateClass("DrawOrthographicDiffuseTextureFullSize", (float)texFullWidth, (float)texFullHeight, texFMT, r3dScreenBuffer::Z_NO_Z, 0, 1, 1 );
	rtFull->SetDebugD3DComment("DrawOrthographicDiffuseTextureFullSize");

	texFullWidth = (int)rtFull->Width ;
	texFullHeight = (int)rtFull->Height ;

	if( texFullWidth < texWidth )
	{
		texWidth = texFullWidth ;
		texHeight = texFullHeight ;
	}

	// this is system mem thus unlikely to fail.
	D3D_V( r3dRenderer->pd3ddev->CreateTexture( texWidth, texHeight, 1, 0, texFMT, D3DPOOL_SYSTEMMEM, &tex, NULL ) );

	r3dScreenBuffer* rt = NULL ;
	r3dScreenBuffer* rtIntermediate = NULL ;

	// otherwise we don't need it
	if( texWidth < texFullWidth )
	{
		rtIntermediate = r3dScreenBuffer::CreateClass("DrawOrthographicDiffuseTextureIntermediate", (float)texWidth, (float)texFullHeight, texFMT );
		rtIntermediate->SetDebugD3DComment("DrawOrthographicDiffuseTextureIntermediate");

		rt = r3dScreenBuffer::CreateClass("DrawOrthographicDiffuseTexture", (float)texWidth, (float)texHeight, texFMT);
		rt->SetDebugD3DComment("DrawOrthographicDiffuseTexture");
	}

	D3DPERF_BeginEvent( 0, L"Update OrthoDiffuse" );

	struct SaveRestoreStates
	{
		explicit SaveRestoreStates( r3dScreenBuffer* rt )
		{
			//------------------------------------------------------------------------
			// get previous states
			for( int i = 0, e = sizeof PrevRTs / sizeof PrevRTs[ 0 ]; i < e; i ++ )
			{
				r3dRenderer->GetRT( i, &PrevRTs[ i ] );
			}

			r3dRenderer->GetDSS( &PrevDSS ) ;

			r3dRenderer->GetViewport( &PrevVP );

			PrevView			= r3dRenderer->ViewMatrix;
			PrevProj			= r3dRenderer->ProjMatrix;
			PrevNear			= r3dRenderer->NearClip;
			PrevFar				= r3dRenderer->FarClip;			

			r3dRenderer->SetRenderingMode( R3D_BLEND_NOALPHA | R3D_BLEND_NZ | R3D_BLEND_PUSH );

			D3D_V( r3dRenderer->pd3ddev->GetRenderState( D3DRS_SCISSORTESTENABLE, &PrevScissor ) );

			D3D_V( r3dRenderer->pd3ddev->GetRenderState( D3DRS_STENCILENABLE, &PrevStencil ) );
		}

		~SaveRestoreStates()
		{
			for( int i = 0, e = sizeof PrevRTs / sizeof PrevRTs[ 0 ]; i < e; i ++ )
			{
				if( !i && !PrevRTs[ i ] )
				{
					extern r3dScreenBuffer*	gBuffer_Color;
					r3dRenderer->SetRT( i, gBuffer_Color->GetTex2DSurface() );
				}
				else
				{
					r3dRenderer->SetRT( i, PrevRTs[ i ] );
				}

				if ( PrevRTs[ i ] )
					PrevRTs[ i ]->Release();
			}

			r3dRenderer->SetDSS( PrevDSS ) ;
			SAFE_RELEASE( PrevDSS );

			r3dRenderer->SetViewport( (float)PrevVP.X, (float)PrevVP.Y, (float)PrevVP.Width, (float)PrevVP.Height );

			r3dRenderer->SetCameraEx( PrevView, PrevProj, PrevNear, PrevFar, false );

			r3dRenderer->SetRenderingMode( R3D_BLEND_POP  );

			r3dRenderer->RestoreCullMode();

			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, PrevScissor ) );
			D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, PrevStencil ) );
		}

		IDirect3DSurface9*	PrevRTs[ 4 ];
		IDirect3DSurface9*	PrevDSS ;
		D3DVIEWPORT9		PrevVP;

		D3DXMATRIX	PrevView;
		D3DXMATRIX	PrevProj;
		float		PrevNear;
		float		PrevFar;
		DWORD		PrevScissor;
		DWORD		PrevStencil;

	} saveRestoreStates( rt ); (void)saveRestoreStates;


	for( int i = 1, e = 4; i < e; i ++ )
	{
		r3dRenderer->SetRT( i, NULL );
	}

	r3dRenderer->SetDSS( NULL ) ;

	if( !PrepareOrthographicTerrainRender( texFullWidth, texFullHeight ) )
		return ;

	r3dRenderer->SetViewport( 0.f, 0.f, (float)texWidth, (float)texHeight );

	r3dRenderer->SetCullMode( D3DCULL_NONE );

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE ) );

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, FALSE ) );

	float nearClip = 0.f;
	float farClip = m_Desc.MaxHeight * 2.f ;

	D3DXMATRIX proj;
	r3dRenderer->BuildMatrixOrthoOffCenterLH( &proj, 0, m_Desc.XSize, 0, m_Desc.ZSize, nearClip, farClip );

	float camY = m_Desc.MaxHeight * 1.5f;

	D3DXMATRIX view;
	D3DXVECTOR3 vEye( 0, camY, 0 );
	D3DXVECTOR3 vAt( 0, -m_Desc.MaxHeight, 0 );
	D3DXVECTOR3 vUp( 0, 0, 1 );

	D3DXMatrixLookAtLH( &view, &vEye, &vAt, &vUp );

	r3dRenderer->SetCameraEx( view, proj, nearClip, farClip, false );

	r3dCamera cam ( r3dPoint3D( vEye.x, vEye.y, vEye.z ) );

	cam.NearClip	= nearClip;
	cam.FarClip		= farClip;

	cam.vPointTo	= r3dPoint3D( 0, -1, 0 );
	cam.vUP			= r3dPoint3D( vUp.x, vUp.y, vUp.z );

	cam.FOV			= 90;

	cam.ProjectionType	= r3dCamera::PROJTYPE_ORTHO;
	cam.Width			= m_Desc.XSize ;
	cam.Height			= m_Desc.ZSize ;
	cam.Aspect			= 1.f;

	for( ; ; )
	{
		rtFull->Activate();
		
		for( int i = 1, e = sizeof saveRestoreStates.PrevRTs / sizeof saveRestoreStates.PrevRTs[ 0 ]; i < e; i ++ )
		{
			r3dRenderer->SetRT( i, NULL );
		}

		DrawOrthographicTerrain( cam, false );

		rtFull->Deactivate();


		if( rtIntermediate )
		{
			rtIntermediate->Activate();

			r3dRenderer->SetVertexShader("VS_GRASS_TINT_BLUR");
			r3dRenderer->SetPixelShader("PS_GRASS_TINT_BLUR");
			float numTaps = static_cast<float>(texFullHeight) / texHeight;
			D3DXVECTOR4 psParams[2] = 
			{
				D3DXVECTOR4(1.0f / texFullWidth, 0.0f, numTaps, 1 / numTaps),
				D3DXVECTOR4(2.0f, 0, 0, 0)
			};
			r3dRenderer->pd3ddev->SetPixelShaderConstantF(0, &psParams[0].x, _countof(psParams));
			r3dDrawBoxFS((float)texWidth, (float)texFullHeight, r3dColor::white, rtFull->Tex);

			rtIntermediate->Deactivate();

			rt->Activate();

			numTaps = static_cast<float>(texFullHeight) / texHeight;
			psParams[0] = D3DXVECTOR4(0.0f, 1.0f / texFullHeight, numTaps, 1 / numTaps );
			psParams[1] = D3DXVECTOR4(1.0f, 0, 0, 0);
			r3dRenderer->pd3ddev->SetPixelShaderConstantF(0, &psParams[0].x, _countof(psParams));
			r3dDrawBoxFS((float)texWidth, (float)texHeight, r3dColor::white, rtIntermediate->Tex);

			rt->Deactivate();
		}

		IDirect3DSurface9* texSurf;

		D3D_V( tex->GetSurfaceLevel( 0, &texSurf ) );

		bool success = true;

		IDirect3DSurface9* rtSurf = 0;
		
		if( rt )
			rtSurf = rt->GetTex2DSurface();
		else
			rtSurf = rtFull->GetTex2DSurface();


 		if( r3dRenderer->pd3ddev->GetRenderTargetData( rtSurf, texSurf ) == D3DERR_DEVICELOST )
 		{
			bool hadRTs = rt || rtIntermediate ;

			delete rt;
			delete rtFull;
			delete rtIntermediate;
			r3dRenderer->SetDeviceLost();
 
 			for( int i = 0, e = sizeof saveRestoreStates.PrevRTs / sizeof saveRestoreStates.PrevRTs[ 0 ]; i < e; i ++ )
 			{
 				SAFE_RELEASE( saveRestoreStates.PrevRTs[ i ] )
 			}

			SAFE_RELEASE( saveRestoreStates.PrevDSS );
 
 			while( !r3dRenderer->TryToRestoreDevice() )
 			{
 				Sleep( 250 );
				r3dProcessWindowMessages();
 			}

			if( hadRTs )
			{
				rt = r3dScreenBuffer::CreateClass("DrawOrthographicDiffuseTexture", (float)texWidth, (float)texHeight, texFMT);
				rt->SetDebugD3DComment("DrawOrthographicDiffuseTexture");
				rtIntermediate = r3dScreenBuffer::CreateClass("DrawOrthographicDiffuseTextureIntermediate", (float)texWidth, (float)texFullHeight, texFMT);
				rtIntermediate->SetDebugD3DComment("DrawOrthographicDiffuseTextureIntermediate");
			}

			rtFull = r3dScreenBuffer::CreateClass("DrawOrthographicDiffuseTextureFullSize", (float)texFullWidth, (float)texFullHeight, texFMT);
			rtFull->SetDebugD3DComment("DrawOrthographicDiffuseTextureFullSize");

 			success = false;
 		}

		texSurf->Release();

		if( success )
			break ;

		r3dProcessWindowMessages();
	}

	D3DPERF_EndEvent();

	ID3DXBuffer* buffer;

	D3D_V( D3DXSaveTextureToFileInMemory( &buffer, D3DXIFF_DDS, tex, NULL ) );

	if( m_Desc.OrthoDiffuseTex )
	{
		r3dRenderer->DeleteTexture( m_Desc.OrthoDiffuseTex );
	}

	m_Desc.OrthoDiffuseTex = r3dRenderer->AllocateTexture();

	D3DFORMAT targetFormat = D3DFMT_DXT1;

	r3dD3DTextureTunnel result ;

	r3dDeviceTunnel::D3DXCreateTextureFromFileInMemoryEx( buffer->GetBufferPointer(), buffer->GetBufferSize(), texWidth, texHeight, 1, 0, targetFormat, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0, NULL, NULL, &result, "Terrain Ortho" ) ;

	m_Desc.OrthoDiffuseTex->Setup( texWidth, texHeight, 1, targetFormat, 1, &result, false );

	buffer->Release();

	tex->Release();

	delete rt;
	delete rtIntermediate;
	delete rtFull;
}

//------------------------------------------------------------------------

int
r3dITerrain::GetDecalID( const r3dPoint3D& pnt, uint32_t sourceHash )
{
	const DecalMapEntry* defEntry = NULL;

	if( const MaterialType* type = GetMaterialType( pnt ) )
	{
		for( uint32_t i = 0, e = type->entries.Count(); i < e; i ++ )
		{
			const DecalMapEntry& en = type->entries[ i ];

			if( en.SourceNameHash == sourceHash )
			{
				return DeriveDecalID( en );
			}
			else
				if( en.SourceNameHash == DEFAULT_DECAL_SOURCE_HASH )
				{
					defEntry = & en;
				}
		}
	}

	if( defEntry )
	{
		return DeriveDecalID( *defEntry );
	}

	return g_pMaterialTypes->GetDefaultDecal( sourceHash );
}

//------------------------------------------------------------------------

void
r3dITerrain::SetOrthoDiffuseTextureDirty()
{
	m_OrthoDiffuseTexDirty = 1 ;
}

//------------------------------------------------------------------------

void
r3dITerrain::UpdateOrthoDiffuseTexture()
{
	if( m_OrthoDiffuseTexDirty )
	{
		DrawOrthographicDiffuseTexture( 2 );
		m_OrthoDiffuseTexDirty = 0 ;
	}
}

//------------------------------------------------------------------------

r3dTerrainPaintBoundControl::r3dTerrainPaintBoundControl()
: PaintUseBounds( 0 )
, InvertBounds( 0 )
, PaintMinHeight( 0.f )
, PaintMaxHeight( 1.f )
, PaintMinSlope( 0.f )
, PaintMaxSlope( 90.f )
{

}

//------------------------------------------------------------------------

static bool IsValidPaintBoundsInt( const r3dTerrainPaintBoundControl& ctrl, const r3dPoint3D &v, float fSize, float *oFactor )
{
	*oFactor = 1.f;
	if ( ! ctrl.PaintUseBounds )
		return true;

	float fDelta;

	float fMin = ctrl.PaintMinHeight;
	if ( v.y < fMin )
		return false;

	float fMax = ctrl.PaintMaxHeight;
	if ( v.y > fMax )
		return false;

	if ( ctrl.PaintMinHeight > 0.f )
	{
		fDelta = v.y - fMin;
		if ( fDelta < fSize )
			*oFactor = fDelta / fSize;
	}

	if ( ctrl.PaintMaxHeight < 1.f )
	{
		fDelta = fMax - v.y;
		if ( fDelta < fSize )
			*oFactor = *oFactor * fDelta / fSize;
	}

	r3dVector vN = Terrain->GetNormal( v );
	float f = vN.y;

	float fMinCos = cosf( D3DXToRadian( ctrl.PaintMaxSlope ) );
	if ( f < fMinCos )
		return false;

	float fMaxCos = cosf( D3DXToRadian( ctrl.PaintMinSlope ) );
	if ( f > fMaxCos )
		return false;

	float fVal = 0.4f;

	if ( ctrl.PaintMaxSlope < 90.f )
	{
		fDelta = f - fMinCos;
		if ( fDelta < fVal )
			*oFactor = *oFactor * fDelta / fVal;
	}

	if ( ctrl.PaintMinSlope > 0.f )
	{
		fDelta = fMaxCos - f;
		if ( fDelta < fSize )
			*oFactor = *oFactor * fDelta / fVal;
	}

	return true;
}

//--------------------------------------------------------------------------------------------------------

bool terra_IsValidPaintBounds( const r3dTerrainPaintBoundControl& ctrl, const r3dPoint3D &v, float fSize, float* oFactor )
{
	bool bRes = IsValidPaintBoundsInt( ctrl, v, fSize, oFactor );
	return ctrl.InvertBounds ? ! bRes : bRes;
}


//------------------------------------------------------------------------

float terra_GetH(const r3dPoint3D &P)
{
	if( Terrain && Terrain->IsLoaded() )
	{
		return Terrain->GetHeight(P);
	}

	return 0;
}

//------------------------------------------------------------------------

int terra_IsWithinPreciseHeightmap( const r3dPoint3D &vPos )
{
	if( Terrain )
		return Terrain->IsPosWithinPreciseHeights( vPos );

	return 1;
}

//------------------------------------------------------------------------

int terra_IsWithinPreciseHeightmap( const r3dBoundBox &bb )
{
	return	terra_IsWithinPreciseHeightmap( bb.Org + r3dPoint3D( 0.f, 0.f, 0.f ) )
				&&
			terra_IsWithinPreciseHeightmap( bb.Org + r3dPoint3D( bb.Size.x, 0.f, 0.f ) )
				&&
			terra_IsWithinPreciseHeightmap( bb.Org + r3dPoint3D( 0.f, 0.f, bb.Size.z ) )
				&&
			terra_IsWithinPreciseHeightmap( bb.Org + r3dPoint3D( bb.Size.x, 0.f, bb.Size.z ) );
}

//------------------------------------------------------------------------

BOOL terra_FindIntersection(const r3dPoint3D &vFrom, const r3dPoint3D &vTo, r3dPoint3D &colpos, int iterations)
{
	if(!Terrain || !Terrain->IsLoaded() ) return FALSE;

	R3DPROFILE_FUNCTION( "terra_FindIntersection" ) ;

	// if start is below terrain
	if(vFrom.y <= Terrain->GetHeight(vFrom)) {
		colpos   = vFrom;
		colpos.y = Terrain->GetHeight(colpos);
		return TRUE;
	}

	// check end point if we do not have to iterate
	if(iterations == 0) {
		if(vTo.y <= Terrain->GetHeight(vTo)) {
			colpos   = vTo;
			colpos.y = Terrain->GetHeight(colpos);
			return TRUE;
		} 
		return FALSE;
	}

	// traverse path
	r3dPoint3D delta = (vTo - vFrom) / (float)iterations;
	r3dPoint3D pos   = vFrom;
	for(int i = 0; i < iterations; i++)
	{
		pos += delta;
		if(pos.Y <= Terrain->GetHeight(pos)) 
		{
			colpos   = pos;
			colpos.y = Terrain->GetHeight(colpos);

			// run more precise check
			pos -= delta;
			delta /= 1000.0f;
			for(int i=0; i<1000; ++i)
			{
				pos += delta;
				if(pos.Y <= Terrain->GetHeight(pos)) 
				{
					colpos   = pos;
					colpos.y = Terrain->GetHeight(colpos);
					return TRUE;
				}
			}
			return TRUE;
		}
	}

	return FALSE;
}

//------------------------------------------------------------------------

void terra_GetMinMaxTerraHeight( float miX, float miZ, float maX, float maZ, float* oMiH, float* oMaH ) 
{
	r3d_assert( Terrain ) ;

	Terrain->GetHeightRange( oMiH, oMaH, r3dPoint2D( miX, miZ ), r3dPoint2D( maX, maZ ) ) ;
}

//------------------------------------------------------------------------

int terra_IsTerrain()
{
	return (int)Terrain;
}
