#include "r3dPCH.h"
#include "r3d.h"

#include "../SF/script.h"
#include "r3dBackgroundTaskDispatcher.h"

#include "r3dDeviceQueue.h"

typedef r3dTL::TArray< TextureReloadListener > TextureReloadListeners ;

static TextureReloadListeners gTextureReloadListeners ;

static 	int 	r3dLastTextureID     = 100;

int	r3dTexture_ScaleCoef = 1;
int	r3dTexture_MinSize   = 1;		// Minimum possible texture dimension after scaling
int	r3dTexture_ScaleBoxInterpolate = 0;	// Use D3D_FILTER_BOX interpolation
int	r3dTexture_UseEmpty  = 0; // for server

//C
//
// r3dTexture
//
//

static void DoCheckPow2( int width, int height, const char* file )
{
	if( !r3dIsPow2( width ) || !r3dIsPow2( height ) )
	{
		//r3dArtBug( "Texture '%s' is not power of 2 ( %d, %d )\n", file, width, height );
	}
}

//------------------------------------------------------------------------

r3dTexture::r3dTexture()
{
	Missing = 0;

	m_PendingAsyncOps = 0;
	m_TexArray = NULL;

	m_pDelayTextureArray = 0;
	m_iNumTextures = 0;
	m_LastAccess = 0;
	m_AccessCounter = 0;

	Width 		= 0;
	Height    	= 0;
	Depth			= 0;
	TexFormat 	= D3DFMT_UNKNOWN;
	Flags         = 0;

	Instances     = 0;

	pNext         = NULL;
	pPrev         = NULL;

	NumMipMaps	= 0;

	m_DownScale = 1;
	m_MaxDim = 0;

	bPersistent	= 0;
	bCubemap		= false;

	m_Pool = D3DPOOL_MANAGED;

	Pitch = 0;

	ID	        = r3dLastTextureID++;
}

//------------------------------------------------------------------------


struct TextureUnloadTaskParams : r3dTaskParams
{
	r3dTexture* UnLoadee;
};

r3dTaskParramsArray< TextureUnloadTaskParams > g_TextureUnloadTaskParams;

struct TextureLoadTaskParams : r3dTaskParams
{
	r3dTexture* Loadee;
};

r3dTaskParramsArray< TextureLoadTaskParams > g_TextureLoadTaskParams;

int g_CollapsedLoadUnloadTextureCount;

static int CompareForLoadUnloadTex( const r3dBackgroundTaskDispatcher::TaskDescriptor* this_, const r3dBackgroundTaskDispatcher::TaskDescriptor* td )
{
	if( this_->Fn == r3dTexture::DestroyTextureResources && td->Fn == r3dTexture::LoadTexture )
	{
		TextureUnloadTaskParams* unparams = static_cast< TextureUnloadTaskParams* >( this_->Params );
		TextureLoadTaskParams* loparams = static_cast< TextureLoadTaskParams* > ( td->Params );

		if( unparams->UnLoadee == loparams->Loadee )
		{
			g_CollapsedLoadUnloadTextureCount ++;
			return 1;
		}
	}

	if( this_->Fn == r3dTexture::LoadTexture && td->Fn == r3dTexture::DestroyTextureResources )
	{
		TextureUnloadTaskParams* unparams = static_cast< TextureUnloadTaskParams* >( td->Params );
		TextureLoadTaskParams* loparams = static_cast< TextureLoadTaskParams* > ( this_->Params );

		if( unparams->UnLoadee == loparams->Loadee )
		{
			g_CollapsedLoadUnloadTextureCount ++;
			return 1;
		}
	}

	return 0;
}

void r3dTexture::Unload( bool allowAsync /*= true*/ )
{
	if( Instances == 1 && allowAsync && g_async_loading->GetInt() && R3D_IS_MAIN_THREAD() )
	{
		for( ; !g_TextureUnloadTaskParams.HasFree(); )
		{
			ProcessDeviceQueue( r3dGetTime(), 0.1f );
		}
	}

	r3dCSHolderWithDeviceQueue csholder( g_ResourceCritSection ); (void)csholder;

	r3d_assert( Instances > 0 );

	InterlockedDecrement( &Instances );

	if( Instances == 0 )
	{
		DestroyResources( allowAsync );
	}
}

//------------------------------------------------------------------------


/*static*/ void r3dTexture::DestroyTextureResources( struct r3dTaskParams* taskParams )
{
	TextureUnloadTaskParams* params = static_cast< TextureUnloadTaskParams* >( taskParams );
	if( params->Cancel )
	{
		InterlockedDecrement( &params->UnLoadee->m_PendingAsyncOps );
		r3d_assert( params->UnLoadee->m_PendingAsyncOps >= 0 );
	}
	else
	{
		params->UnLoadee->DoDestroyResources();
	}
}

void r3dTexture::DestroyResources( bool allowAsync )
{
	InterlockedIncrement( &m_PendingAsyncOps );

	if( allowAsync && g_async_loading->GetInt() && R3D_IS_MAIN_THREAD() && g_pBackgroundTaskDispatcher && !( Flags & fRenderTarget ) )
	{
		r3dBackgroundTaskDispatcher::TaskDescriptor td;

		TextureUnloadTaskParams* params = g_TextureUnloadTaskParams.Alloc();

		params->UnLoadee			= this;

		td.Params = params;
		td.Fn = DestroyTextureResources;
		td.CompareFn = CompareForLoadUnloadTex;
		td.TaskClass = r3dBackgroundTaskDispatcher::TASK_CLASS_TEXTURE;
		td.CompletionFlag = 0;

		g_pBackgroundTaskDispatcher->AddTask( td );
	}
	else
	{
		DoDestroyResources();
	}
}

//------------------------------------------------------------------------

void r3dTexture::DoDestroyResources()
{
	if( !( Flags & fRenderTarget ) )
	{
		int size = -(int) GetTextureSizeInVideoMemory() ;

		UpdateTextureStats( size ) ;
	}

	DestroyInternal();

	InterlockedDecrement( &m_PendingAsyncOps );

	r3d_assert( m_PendingAsyncOps >= 0 );
}

//------------------------------------------------------------------------

void r3dTexture::LoadResources()
{
	InterlockedIncrement( &m_PendingAsyncOps );

	if( g_async_loading->GetInt() && R3D_IS_MAIN_THREAD() && g_pBackgroundTaskDispatcher)
	{
		r3dBackgroundTaskDispatcher::TaskDescriptor td;

		TextureLoadTaskParams* params = g_TextureLoadTaskParams.Alloc();

		params->Loadee			= this;

		td.Params = params;
		td.Fn = LoadTexture;
		td.CompareFn = CompareForLoadUnloadTex;
		td.TaskClass = r3dBackgroundTaskDispatcher::TASK_CLASS_TEXTURE;
		td.CompletionFlag = 0;

		g_pBackgroundTaskDispatcher->AddTask( td );
	}
	else
	{
		DoLoad();
	}
}

//------------------------------------------------------------------------

void r3dTexture::SetNeedReload( int bDoNeed )
{
	if( bDoNeed )
		Flags |= fNeedReload;
	else
		Flags &= ~fNeedReload;
}

//------------------------------------------------------------------------

int r3dTexture::NeedsReload() const
{
	return Flags & fNeedReload;
}

//------------------------------------------------------------------------

r3dTexture::~r3dTexture()
{
	if( m_TexArray )
	{
		r3dOutToLog( "r3dTexture::~r3dTexture: rogue texture '%s' - escaped usual deleting procedures\n", getFileLoc().FileName );
		Unload( false );
	}
}

void *r3dTexture::Lock(int LockForWrite, const RECT *LocRect)
{
	D3DLOCKED_RECT lr;

	if( !m_TexArray[0].Valid() )
		return NULL;

	DWORD flags( 0 );

	if( !LockForWrite )
	{
		flags |= D3DLOCK_READONLY;
	}

	GetD3DTunnel().LockRect(0, &lr, LocRect, flags) ;

	Flags |= r3dTexture::fLocked;
	if(LockForWrite)
		Flags |= r3dTexture::fLockedForWrite;
	Pitch  = lr.Pitch;
	return lr.pBits;

}

void r3dTexture::Unlock()
{
	if(Flags & r3dTexture::fLocked) 
	{
		GetD3DTunnel().UnlockRect(0);
		Flags &= ~r3dTexture::fLocked;
	}

}

void CalcDownScaleToMatchMaxDim( int& ioRatio, int Dim, int MaxDim )
{
	if( Dim / ioRatio > MaxDim )
	{
		ioRatio = R3D_MAX( Dim / MaxDim, 1 );

		if( Dim / ioRatio > MaxDim )
		{
			ioRatio ++;
		}

		r3d_assert( Dim / ioRatio <= MaxDim );
	}
}

namespace
{
	struct DownTexParams
	{
		r3dD3DTextureTunnel* texTunnel ;
		int mipsDown ;
		D3DPOOL pool;
	};

	void DownCube( r3dD3DTextureTunnel* texTunnel, int mipsDown, D3DPOOL pool )
	{
		R3D_ENSURE_MAIN_THREAD();

		IDirect3DCubeTexture9* cubeTex = texTunnel->AsTexCube();

		D3DSURFACE_DESC desc;

		D3D_V( cubeTex->GetLevelDesc( mipsDown, &desc ) );

		int origLevelCount = cubeTex->GetLevelCount();

		IDirect3DCubeTexture9* res;

		D3D_V( r3dRenderer->pd3ddev->CreateCubeTexture( desc.Width, origLevelCount - mipsDown, 0, desc.Format, pool, &res, NULL ) );

		for( int i = 0, e = res->GetLevelCount(); i < e; i ++ )
		{
			for( int j = 0; j < 6; j ++ )
			{
				if( pool == D3DPOOL_DEFAULT )
				{
					IDirect3DSurface9* src, * dest;

					D3D_V( cubeTex->GetCubeMapSurface( D3DCUBEMAP_FACES( j ), i + mipsDown, &src ) );
					D3D_V( res->GetCubeMapSurface( D3DCUBEMAP_FACES( j ), i, &dest ) );

					D3D_V( r3dRenderer->pd3ddev->UpdateSurface( src, NULL, dest, NULL ) );

					SAFE_RELEASE( src );
					SAFE_RELEASE( dest );
				}
				else
				{
					D3DLOCKED_RECT from, to;

					D3D_V( cubeTex->LockRect( D3DCUBEMAP_FACES( j ), i + mipsDown, &from, NULL, D3DLOCK_READONLY ) );
					D3D_V( res->LockRect( D3DCUBEMAP_FACES( j ), i, &to, NULL, 0 ) );

					D3DSURFACE_DESC desc;

					D3D_V( res->GetLevelDesc( i, &desc ) );

					r3d_assert( from.Pitch == to.Pitch );
					memcpy( to.pBits, from.pBits, r3dGetPixelSize( to.Pitch * desc.Height, desc.Format ) );

					D3D_V( cubeTex->UnlockRect( D3DCUBEMAP_FACES( j ), i + mipsDown ) );
					D3D_V( res->UnlockRect( D3DCUBEMAP_FACES( j ), i ) );
				}
			}
		}

		texTunnel->ReleaseAndReset();

		texTunnel->Set( res );
	}

	void DoDownCube( void* params )
	{
		DownTexParams* pms = ( DownTexParams* ) params ;

		DownCube( pms->texTunnel, pms->mipsDown, pms->pool );
	}


	void DownTex2D( r3dD3DTextureTunnel* tex2DTunnel, int mipsDown, D3DPOOL pool )
	{
		R3D_ENSURE_MAIN_THREAD();

		D3DSURFACE_DESC desc;

		IDirect3DTexture9* tex2D = tex2DTunnel->AsTex2D() ;

		D3D_V( tex2D->GetLevelDesc( mipsDown, &desc ) );

		int origLevelCount = tex2D->GetLevelCount();

		IDirect3DTexture9* res;

		int mips = origLevelCount - mipsDown ;

		HRESULT hres = r3dRenderer->pd3ddev->CreateTexture( desc.Width, desc.Height, mips, 0, desc.Format, pool, &res, NULL );

		r3dRenderer->CheckOutOfMemory( hres ) ;

		if( hres != D3D_OK )
		{
			if( hres == E_OUTOFMEMORY )
			{
				r3dOutToLog( "DownTex2D: out of memory!\n" );
			}

			if( hres == D3DERR_OUTOFVIDEOMEMORY )
			{
				r3dOutToLog( "DownTex2D: out VIDEO memory!\n" );
			}

			r3dError( "DownTex2D: CreateTexture call failed: Width = %d, Height = %d, mips = %d, format = %d, pool = %d\n", desc.Width, desc.Height, mips, desc.Format, desc.Pool );
		}


		for( int i = 0, e = res->GetLevelCount(); i < e; i ++ )
		{
			if( pool == D3DPOOL_DEFAULT )
			{
				IDirect3DSurface9* sourceSurf, * targSurf ;
				
				D3D_V( tex2D->GetSurfaceLevel( i + mipsDown, & sourceSurf ) );
				D3D_V( res->GetSurfaceLevel( i, & targSurf ) );

				D3D_V( r3dRenderer->pd3ddev->UpdateSurface( sourceSurf, NULL, targSurf, NULL ) );

				sourceSurf->Release();
				targSurf->Release();
			}
			else
			{
				D3DLOCKED_RECT from, to;

				D3D_V( tex2D->LockRect( i + mipsDown, &from, NULL, D3DLOCK_READONLY ) );
				D3D_V( res->LockRect( i, &to, NULL, 0 ) );

				D3DSURFACE_DESC desc;

				D3D_V( res->GetLevelDesc( i, &desc ) );

				r3d_assert( from.Pitch == to.Pitch );
				memcpy( to.pBits, from.pBits, to.Pitch * r3dGetPitchHeight( desc.Height, desc.Format ) );

				D3D_V( tex2D->UnlockRect( i + mipsDown ) );
				D3D_V( res->UnlockRect( i ) );
			}
		}
		
		tex2DTunnel->ReleaseAndReset();

		tex2DTunnel->Set( res );
	}

	void DoDownTex2D( void* params )
	{
		DownTexParams* pms = ( DownTexParams* ) params ;

		DownTex2D( pms->texTunnel, pms->mipsDown, pms->pool );
	}

	void DownTex3D( r3dD3DTextureTunnel* TexTunnel, int mipsDown, D3DPOOL pool )
	{
		R3D_ENSURE_MAIN_THREAD();

		D3DVOLUME_DESC desc;

		IDirect3DVolumeTexture9* volTex = TexTunnel->AsTexVolume();

		D3D_V( volTex->GetLevelDesc( mipsDown, &desc ) );

		int origLevelCount = volTex->GetLevelCount();

		IDirect3DVolumeTexture9* res;

		int finalMipCount = origLevelCount - mipsDown;

		D3D_V( r3dRenderer->pd3ddev->CreateVolumeTexture(	desc.Width, desc.Height, desc.Depth, finalMipCount, 0, desc.Format, 
															// in case of volumetric pool default texture
															// we have to downscale in temporary system mem texture and then
															// call UpdateTexture
															pool == D3DPOOL_DEFAULT ? D3DPOOL_SYSTEMMEM : pool, &res, NULL 
															) );

		for( int i = 0, e = res->GetLevelCount(); i < e; i ++ )
		{
			D3DLOCKED_BOX from, to;

			D3D_V( volTex->LockBox( i + mipsDown, &from, NULL, D3DLOCK_READONLY ) );
			D3D_V( res->LockBox( i, &to, NULL, 0 ) );

			D3DVOLUME_DESC desc;

			D3D_V( res->GetLevelDesc( i, &desc ) );

			r3d_assert( from.RowPitch == to.RowPitch && 
						from.SlicePitch == to.SlicePitch );

			memcpy( to.pBits, from.pBits, r3dGetPixelSize( to.SlicePitch * desc.Depth, desc.Format ) );

			D3D_V( volTex->UnlockBox( i + mipsDown ) );
			D3D_V( res->UnlockBox(  i ) );
		}

		IDirect3DVolumeTexture9* final ;

		if( pool == D3DPOOL_DEFAULT )
		{
			D3D_V( r3dRenderer->pd3ddev->CreateVolumeTexture( desc.Width, desc.Height, desc.Depth, finalMipCount, 0, desc.Format, pool, &final, NULL ) );

			final->AddDirtyBox( NULL );

			D3D_V( r3dRenderer->pd3ddev->UpdateTexture( res, final ) );

			SAFE_RELEASE( res );
		}
		else
		{
			final = res;
		}

		TexTunnel->ReleaseAndReset() ;
		TexTunnel->Set( final ) ;
	}

	void DoDownTex3D( void* params )
	{
		DownTexParams* pms = ( DownTexParams* ) params ;

		DownTex3D( pms->texTunnel, pms->mipsDown, pms->pool );
	}


	DWORD ilog2( DWORD val )
	{
		if( val <= 1 )
			return 0;

		int res = 0;

		while( val /= 2 )
		{
			res ++;
		}

		return res;
				
	}
}

#include "D3Dcommon.h"

void SetD3DResourcePrivateData(LPDIRECT3DRESOURCE9 res, const char* FName)
{
#if R3D_SET_DEBUG_D3D_NAMES

	DWORD sz = strlen(FName);
	res->SetPrivateData(WKPDID_D3DDebugObjectName, FName, sz, 0);

	void* p = 0;

	res->QueryInterface(IID_IDirect3DTexture9, &p);
	if(p)
	{
		LPDIRECT3DTEXTURE9 t = (LPDIRECT3DTEXTURE9)p;
		int mipsCount = t->GetLevelCount();

		for (int i = 0; i < mipsCount; ++i)
		{
			LPDIRECT3DSURFACE9 surf; 
			t->GetSurfaceLevel(i, &surf);
			surf->SetPrivateData(WKPDID_D3DDebugObjectName, FName, sz, 0);
			surf->Release();
		}

		t->Release();
		return;
	}

	p = 0;
	res->QueryInterface(IID_IDirect3DCubeTexture9, &p);
	if(p)
	{
		LPDIRECT3DCUBETEXTURE9 t = (LPDIRECT3DCUBETEXTURE9)p;
		int mipsCount = t->GetLevelCount();

		for (int i = 0; i < mipsCount; ++i)
		{
			for (int j = D3DCUBEMAP_FACE_POSITIVE_X; j <= D3DCUBEMAP_FACE_NEGATIVE_Z; ++j)
			{
				LPDIRECT3DSURFACE9 surf; 
				t->GetCubeMapSurface((D3DCUBEMAP_FACES)j, i, &surf);
				surf->SetPrivateData(WKPDID_D3DDebugObjectName, FName, sz, 0);
				surf->Release();
			}
		}

		t->Release();

		return;
	}

#endif
}

struct PostLoadStuffParams
{
	r3dD3DTextureTunnel* tunnel;
	int * oNumMips;
	const char* FileName;
	void* StuffTofree;
	bool checkPow2;
};

void DoPostLoadStuff( void* params )
{
	PostLoadStuffParams* pms = (PostLoadStuffParams*) params ;

	*pms->oNumMips = (*pms->tunnel)->GetLevelCount();
	SetD3DResourcePrivateData( (*pms->tunnel).Get(), pms->FileName );

	free( pms->StuffTofree );

	if( pms->checkPow2 )
	{
		IDirect3DBaseTexture9* base = (*pms->tunnel).Get();

		switch( base->GetType() )
		{
		case D3DRTYPE_TEXTURE:
			{
				D3DSURFACE_DESC sdesc;
				D3D_V( static_cast<IDirect3DTexture9*> ( base )->GetLevelDesc( 0, &sdesc ) );
				DoCheckPow2( sdesc.Width, sdesc.Height, pms->FileName );
			}
			break;
		case D3DRTYPE_VOLUMETEXTURE:
			{
				D3DVOLUME_DESC vdesc;
				D3D_V( static_cast<IDirect3DVolumeTexture9*> ( base )->GetLevelDesc( 0, &vdesc ) );
				DoCheckPow2( vdesc.Width, vdesc.Height, pms->FileName );
			}
			break;
		case D3DRTYPE_CUBETEXTURE:
			{
				D3DSURFACE_DESC sdesc;
				D3D_V( static_cast<IDirect3DCubeTexture9*> ( base )->GetLevelDesc( 0, &sdesc ) );
				DoCheckPow2( sdesc.Width, sdesc.Height, pms->FileName );
			}
			break;
		}
	}
}

void r3dTexture::LoadTextureInternal(int index, void* FileInMemoryData, uint32_t FileInMemorySize, const char* DEBUG_NAME )
{
	int TgW = 0;
	int TgH = 0;
	int TgD = 0;

	R3D_DEIVCE_QUEUE_OBJ( D3DXIMAGE_INFO, pInfo) ;
	ZeroMemory(&pInfo, sizeof (pInfo));

	D3DXGetImageInfoFromFileInMemory( FileInMemoryData, FileInMemorySize, &pInfo );

	int Mip = 1;

	int DownScaledDueToMaxDim = 1;

	UINT ScaledWidth	= pInfo.Width;
	UINT ScaledHeight	= pInfo.Height;
	UINT ScaledDepth	= pInfo.Depth;

	int maxDim = m_MaxDim;

	if( r_max_texture_dim->GetInt() )
	{
		if( maxDim )
			maxDim = R3D_MIN( maxDim, r_max_texture_dim->GetInt() );
		else
			maxDim = r_max_texture_dim->GetInt();
	}

	if( maxDim )
	{
		CalcDownScaleToMatchMaxDim( DownScaledDueToMaxDim, ScaledWidth	, maxDim );
		CalcDownScaleToMatchMaxDim( DownScaledDueToMaxDim, ScaledHeight	, maxDim );
		CalcDownScaleToMatchMaxDim( DownScaledDueToMaxDim, ScaledDepth	, maxDim );

		ScaledWidth		= R3D_MAX( int( ScaledWidth		/ DownScaledDueToMaxDim ), 1 );
		ScaledHeight	= R3D_MAX( int( ScaledHeight	/ DownScaledDueToMaxDim ), 1 );
		ScaledDepth		= R3D_MAX( int( ScaledDepth		/ DownScaledDueToMaxDim ), 1 );
	}

	int NextDownScale = R3D_MAX( m_DownScale / DownScaledDueToMaxDim, 1 );

	ScaledWidth		= R3D_MAX( int( ScaledWidth		/ NextDownScale ), 1 );
	ScaledHeight	= R3D_MAX( int( ScaledHeight	/ NextDownScale ), 1 );
	ScaledDepth		= R3D_MAX( int( ScaledDepth		/ NextDownScale ), 1 );

	int isDXT = r3dIsDXT( pInfo.Format );

	if( isDXT )
	{
		ScaledWidth = R3D_MAX( (int)ScaledWidth, 4 );
		ScaledHeight = R3D_MAX( (int)ScaledHeight, 4 );
	}

	int allowMipDown = pInfo.Width / ScaledWidth == pInfo.Height / ScaledHeight;

	if( isDXT )
	{
		if( ScaledWidth % 4 )
		{
			allowMipDown = 0;
			ScaledWidth += 4 - ScaledWidth % 4;
		}

		if( ScaledHeight % 4 )
		{
			allowMipDown = 0;
			ScaledHeight += 4 - ScaledHeight % 4;
		}
	}

	if( pInfo.Depth > 1 )
	{
		if( pInfo.Width / ScaledWidth != pInfo.Depth / ScaledDepth )
		{
			allowMipDown = 0;
		}
	}

	UINT totalMipDown = ilog2( R3D_MAX( 
								R3D_MAX( pInfo.Width / ScaledWidth, 
										pInfo.Height / ScaledHeight ), 
											pInfo.Depth / ScaledDepth ) );

	// may be incompatible with DXT after downscale

	TgW = ScaledWidth;
	TgH = ScaledHeight;
	TgD = ScaledDepth;

	bCubemap = pInfo.ResourceType == D3DRTYPE_CUBETEXTURE;

	int ScaleFilter = totalMipDown ? D3DX_FILTER_POINT : D3DX_FILTER_NONE ;

	r3d_assert( !(TgD > 1 && bCubemap) );

	const bool ALLOW_ASYNC = !!g_async_d3dqueue->GetInt() ;

	if( bCubemap )
	{			
		if( totalMipDown && totalMipDown < pInfo.MipLevels && allowMipDown )
		{

			r3dDeviceTunnel::D3DXCreateCubeTextureFromFileInMemoryEx(	FileInMemoryData, FileInMemorySize, pInfo.Width, pInfo.MipLevels - totalMipDown, 0, TexFormat, D3DPOOL_SYSTEMMEM,
																		D3DX_FILTER_NONE, D3DX_FILTER_NONE, 0x00000000, &pInfo, NULL, &m_TexArray[ index ], ALLOW_ASYNC );

			R3D_DEIVCE_QUEUE_OBJ( DownTexParams, parms );

			parms.texTunnel = &m_TexArray[ index ];
			parms.mipsDown = totalMipDown;
			parms.pool = m_Pool;

			AddCustomDeviceQueueItem( DoDownCube, &parms );
		}
		else
		{
			r3dDeviceTunnel::D3DXCreateCubeTextureFromFileInMemoryEx(	FileInMemoryData, FileInMemorySize, ScaledWidth, pInfo.MipLevels, 0, TexFormat, m_Pool,
																		ScaleFilter, ScaleFilter, 0x00000000, &pInfo, NULL, &m_TexArray[ index ], ALLOW_ASYNC ) ;
		}
	}
	else
	{
		if( TgD <= 1 )
		{
			if( totalMipDown && totalMipDown < pInfo.MipLevels && allowMipDown )
			{
				// load original texture

				r3dDeviceTunnel::D3DXCreateTextureFromFileInMemoryEx(	FileInMemoryData, FileInMemorySize, pInfo.Width, pInfo.Height, pInfo.MipLevels, 0, TexFormat, D3DPOOL_SYSTEMMEM,
																		D3DX_FILTER_NONE, D3DX_FILTER_NONE ,0x00000000, &pInfo, NULL, &m_TexArray[ index ], DEBUG_NAME, ALLOW_ASYNC );
				
				// create downscaled version by copying mip levels into a new texture
				R3D_DEIVCE_QUEUE_OBJ( DownTexParams, params );

				params.texTunnel = &m_TexArray[ index ];
				params.mipsDown = totalMipDown;
				params.pool = m_Pool;

				AddCustomDeviceQueueItem( DoDownTex2D, &params );
			}
			else
			{
				r3dDeviceTunnel::D3DXCreateTextureFromFileInMemoryEx(	FileInMemoryData, FileInMemorySize, ScaledWidth, ScaledHeight, pInfo.MipLevels, 0, TexFormat, m_Pool,
																		ScaleFilter, ScaleFilter, 0x00000000, &pInfo, NULL, &m_TexArray[ index ], DEBUG_NAME, ALLOW_ASYNC );
			}
		}
		else
		{
			if( totalMipDown && totalMipDown < pInfo.MipLevels && allowMipDown )
			{
				r3dDeviceTunnel::D3DXCreateVolumeTextureFromFileInMemoryEx(	FileInMemoryData, FileInMemorySize, ScaledWidth, ScaledHeight, ScaledDepth, pInfo.MipLevels - totalMipDown, 0, TexFormat, D3DPOOL_SYSTEMMEM,
																			D3DX_FILTER_POINT, D3DX_FILTER_POINT, 0x00000000, &pInfo, NULL, &m_TexArray[ index ], ALLOW_ASYNC );

				R3D_DEIVCE_QUEUE_OBJ( DownTexParams, params ) ;

				params.texTunnel = &m_TexArray[ index ];
				params.mipsDown = totalMipDown;
				params.pool = m_Pool;

				AddCustomDeviceQueueItem( DoDownTex3D, &params );

			}
			else
			{
				r3dDeviceTunnel::D3DXCreateVolumeTextureFromFileInMemoryEx(	FileInMemoryData, FileInMemorySize, ScaledWidth, ScaledHeight, ScaledDepth, pInfo.MipLevels, 0, TexFormat, m_Pool,
																			ScaleFilter, ScaleFilter, 0x00000000, &pInfo, NULL, &m_TexArray[ index ], ALLOW_ASYNC );
			}
		}
	}


	if( !ALLOW_ASYNC )
	{
		free( FileInMemoryData );
		FileInMemoryData = 0 ;
	}

	if( !g_async_d3dqueue->GetInt() )
	{
		if(!m_TexArray[ index ].Valid()) // failed to load?
		{
			r3dArtBug("Failed to load texture '%s'\n", Location.FileName);
		}
	}

	Width       = TgW;
	Height      = TgH;
	Depth		= TgD;

	if( m_TexArray[ 0 ].Valid() )
	{
		TexFormat = m_TexArray[ 0 ].GetFormat();
	}
	else
	{
		TexFormat   = pInfo.Format;
	}

	R3D_DEIVCE_QUEUE_OBJ( PostLoadStuffParams, parms ) ;

	parms.FileName = Location.FileName;
	parms.oNumMips = &NumMipMaps;
	parms.tunnel = &m_TexArray[index];
	parms.StuffTofree = FileInMemoryData;
	parms.checkPow2 = Flags & fCheckPow2 ? true : false;

	if( g_async_d3dqueue->GetInt() )
		AddCustomDeviceQueueItem( DoPostLoadStuff, &parms );
	else
		ProcessCustomDeviceQueueItem( DoPostLoadStuff, &parms );

}

void r3dTexture::LoadTextureInternal( int index, const char* FName )
{
	r3d_assert(index >=0 && index < m_iNumTextures);
	if(r3dTexture_UseEmpty) 
	{
		Width       = 16;
		Height      = 16;
		Depth		= 1;
		TexFormat   = D3DFMT_A8R8G8B8;

		sprintf(Location.FileName, "%s", FName);
		strlwr(&Location.FileName[0]);

		m_TexArray[index].Set( 0 );
		return;
	}

	r3dFile *ff = r3d_open(FName,"rb");

	if (ff)
	{		
		Missing = 0 ;

		int ff_size = ff->size;
		if(ff_size == 0) // bad texture???
			r3dError("Bad texture '%s' - has zero size!!!\n", FName);

		void* SrcData = malloc ( ff_size+1 );
		if(SrcData == NULL)
			r3dError("Out of memory!");
		fread(SrcData, 1, ff_size, ff);
		fclose(ff);

		LoadTextureInternal( index, SrcData, ff_size, Location.FileName );
	}
	else
	{
		LoadTextureInternal(index, "Data\\Shaders\\Texture\\MissingTexture.dds" );
		Missing = 1;
		return;
	}
}

void r3dTexture::UpdateTextureStats( int size )
{
	if( Flags & fPlayerTexture )
		r3dRenderer->Stats.AddPlayerTexMem ( size );
	else
		r3dRenderer->Stats.AddTexMem ( size );
}

void SetD3DResourcePrivateData(LPDIRECT3DRESOURCE9 res, const char* FName);

void r3dTexture::SetDebugD3DComment(const char* text)
{
	for (int i = 0; i < m_iNumTextures; ++i)
	{
		r3dDeviceTunnel::SetD3DResourcePrivateData(	&m_TexArray[i], text );
	}
}

void
r3dTexture::MarkPlayerTexture()
{
	Flags |= fPlayerTexture;
}

/*static*/
void
r3dTexture::LoadTexture( struct r3dTaskParams* taskParams )
{
	TextureLoadTaskParams* params = static_cast<TextureLoadTaskParams*>( taskParams );

	if( params->Cancel )
	{
		InterlockedDecrement( &params->Loadee->m_PendingAsyncOps );
		r3d_assert( params->Loadee->m_PendingAsyncOps >= 0 );
	}
	else
	{
		params->Loadee->DoLoad();
	}
}

int r3dTexture::Load( const char* fname, D3DFORMAT targetTexFormat, int downScale /*= 1*/, int maxDim /*= 0*/, D3DPOOL pool /*= D3DPOOL_MANAGED*/ )
{
	r3d_assert( maxDim != 1 );

	if( !Instances && R3D_IS_MAIN_THREAD() && g_async_loading->GetInt() )
	{
		for( ; !g_TextureLoadTaskParams.HasFree(); )
		{
			ProcessDeviceQueue( r3dGetTime(), 0.1f );
		}
	}

	r3dCSHolderWithDeviceQueue csholder( g_ResourceCritSection ); (void)csholder;

	if( Instances == 0 )
	{
		SetLoadData( fname, downScale, maxDim, pool, targetTexFormat );
		LoadResources();
	}

	InterlockedIncrement( &Instances );

	return 1 ;	
}

void r3dTexture::SetLoadData( const char* fname, int downScale, int maxDim, D3DPOOL pool, D3DFORMAT fmt )
{
	sprintf(Location.FileName, "%s", fname);
	strlwr(&Location.FileName[0]);

	m_DownScale = downScale;
	m_MaxDim = maxDim;

	m_Pool = pool;

	TexFormat = fmt;

	Flags |= fFileTexture;
}

void r3dTexture::CheckPow2()
{
	if( !Instances )
	{
		Flags |= fCheckPow2;
	}
	else
	{
		DoCheckPow2( GetWidth(), GetHeight(), getFileLoc().FileName );
	}
}

//------------------------------------------------------------------------

void r3dTexture::Load()
{
	Load( getFileLoc().FileName, GetD3DFormat(), GetDownScale(), GetMaxDim(), GetPool() );
}

//------------------------------------------------------------------------

void r3dTexture::DestroyInternal()
{
	if( m_TexArray )
	{
		for(int i=0; i<m_iNumTextures; ++i)
		{
			if( m_TexArray[i].Valid() )
			{
				m_TexArray[i].ReleaseAndReset();
			}
		}

		delete [] m_TexArray;
		m_TexArray = NULL ;
	}
	SAFE_DELETE_ARRAY(m_pDelayTextureArray);
}

int r3dTexture::DoLoad()
{
	D3DXIMAGE_INFO pInfo;

	int Ret = 0;

	ZeroMemory(&pInfo, sizeof (pInfo));

	const char* FName = Location.FileName ;

	// check if we are loading animated texture *.ddv
	if(strstr(FName, ".ddv"))
	{
		Script_c script;
		if ( ! script.OpenFile( FName, true ) )
		{
			r3dArtBug("Failed to open '%s'\n", FName);
			return 0;
		}

		int numElements = 0;
		// we don't how many textures in ddv, so let's reserve for 50 maximum
		char tempFilenames[50][512];
		int  tempDelays[50];

		char tempStr[1024];
		while(!script.EndOfFile())
		{
			memset(tempStr, 0, sizeof(tempStr));
			script.GetLine(tempStr, 1024);
			if(strlen(tempStr) > 5)
			{
				sscanf(tempStr, "%s %d", tempFilenames[numElements], &tempDelays[numElements]);
				numElements++;
				if(numElements>=50)
				{
					r3dArtBug("Too many textures in DDV ('%s'), limit is 50!\n", FName);
					break;
				}
			}
		}
		script.CloseFile();

		const char* tSlash = strrchr(FName, '/');
		r3dscpy_s(tempStr, int(tSlash-FName)+1, FName);

		m_iNumTextures = numElements;
		m_TexArray = gfx_new r3dD3DTextureTunnel [ numElements ] ;
		m_pDelayTextureArray = gfx_new float[numElements];
		for(int i=0; i<numElements; ++i)
		{
			char fullpath[512];
			sprintf(fullpath, "%s%s", tempStr, tempFilenames[i]);
			LoadTextureInternal( i, fullpath );
			m_pDelayTextureArray[i] = (float)tempDelays[i]/1000.0f; // ms->seconds
		}		
	}
	else
	{
		if( m_TexArray )
		{
			r3dOutToLog( "r3dTexture::DoLoad: collision for '%s'\n", FName );
		}

		r3d_assert( !m_TexArray );

		m_iNumTextures = 1;
		m_TexArray = gfx_new r3dD3DTextureTunnel[ 1 ] ;

		LoadTextureInternal( 0, FName );
	}

	int size = GetTextureSizeInVideoMemory() ;

	UpdateTextureStats( size ) ;

	InterlockedDecrement( &m_PendingAsyncOps );

	r3d_assert( m_PendingAsyncOps >= 0 );

	return 1;
}

IDirect3DBaseTexture9* r3dTexture::GetD3DTexture()
{
	if( !m_PendingAsyncOps && Instances )
	{
		if(m_iNumTextures == 1)
			return m_TexArray[0].Get();
		else if(m_iNumTextures > 1)
		{
			r3d_assert(m_pDelayTextureArray);
			r3d_assert(m_AccessCounter >= 0 && m_AccessCounter < m_iNumTextures);
			if((r3dGetTime() - m_LastAccess) > m_pDelayTextureArray[m_AccessCounter])
			{
				m_LastAccess = r3dGetTime();
				m_AccessCounter = (m_AccessCounter+1)%m_iNumTextures;
			}
			r3d_assert(m_AccessCounter >= 0 && m_AccessCounter < m_iNumTextures);
			return m_TexArray[m_AccessCounter].Get();
		}

	}

	return NULL;
}

void r3dTexture::SetNewD3DTexture(IDirect3DBaseTexture9* newTex) 
{
	if(m_TexArray[0].Valid()) 
		m_TexArray[0].ReleaseAndReset(); 

	m_TexArray[0].Set( newTex );
}

void r3dTexture::Setup( int XSize, int YSize, int ZSize, D3DFORMAT TexFmt, int aNumMipMaps, r3dD3DTextureTunnel* texture, bool isRenderTarget )
{
	bCubemap = false;

	TexFormat = TexFmt;

	NumMipMaps = aNumMipMaps;
	Width      = XSize;
	Height     = YSize;
	Depth      = ZSize;

	r3d_assert(Instances == 0);
	InterlockedExchange( &Instances, 1 );

	r3d_assert(m_iNumTextures==0);
	m_TexArray = gfx_new r3dD3DTextureTunnel [ 1 ] ;
	m_iNumTextures = 1;
	m_TexArray[ 0 ] = *texture ;

	if( isRenderTarget )
	{
		Flags |= fRenderTarget ;
	}
	else
	{
		int size = GetTextureSizeInVideoMemory() ;

		UpdateTextureStats( size ) ;
	}
}

void r3dTexture::SetupCubemap( int EdgeLength, D3DFORMAT TexFmt, int aNumMipMaps, r3dD3DTextureTunnel* texture, bool isRenderTarget )
{
	TexFormat = TexFmt;

	bCubemap    = true;
	NumMipMaps  = aNumMipMaps;
	Width       = EdgeLength;
	Height      = EdgeLength;
	Depth       = 1;

	r3d_assert(Instances == 0);
	InterlockedExchange( &Instances, 1 );

	r3d_assert(m_iNumTextures==0);
	m_TexArray = gfx_new r3dD3DTextureTunnel [ 1 ] ;
	m_iNumTextures = 1;
	m_TexArray[ 0 ] = *texture ;

	if( isRenderTarget )
	{
		Flags |= fRenderTarget ;
	}
	else
	{
		int size = GetTextureSizeInVideoMemory() ;

		UpdateTextureStats( size ) ;
	}
}

float GetD3DTexFormatSize(D3DFORMAT Fmt)
{
	switch(Fmt)
	{
	default:
		r3dOutToLog("GetD3DTexFormatSize, unknown format: %d\n", Fmt);
		return 2;
		// depth texture
	case D3DFMT_D24X8:
	case D3DFMT_D24S8:
		return 4.0f ;
	case D3DFMT_DXT1: 
		return 0.5f; // dxt1 compression ratio 1:8
	case D3DFMT_DXT2:
	case D3DFMT_DXT3:
	case D3DFMT_DXT4:
	case D3DFMT_DXT5: 
		return 1.0f; // dxt2,3,4,5 compression ratio 1:4
	case D3DFMT_R8G8B8: 
		return 3.0f;
	case D3DFMT_A8B8G8R8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_Q8W8V8U8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_G16R16:
	case D3DFMT_R32F:
		return 4.0f;
	case D3DFMT_G32R32F:
		return 8.0f ;
	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_X4R4G4B4:
	case D3DFMT_A4R4G4B4:
		return 2.0f;
	case D3DFMT_L16:
		return 2.0f;
	case D3DFMT_R3G3B2:
	case D3DFMT_A8:
	case D3DFMT_L8:
	case D3DFMT_A8R3G3B2: 
		return 1.0f;
	case D3DFMT_A32B32G32R32F:
		return 16.0f;
	case D3DFMT_A16B16G16R16F:
		return 8.0f;
	}
}

int r3dTexture::GetTextureSizeInVideoMemory()
{
	int mips = NumMipMaps ;

	if( !mips )
	{
		if( IDirect3DBaseTexture9* tex = m_TexArray[ 0 ].AsBaseTex() )
		{
			mips = tex->GetLevelCount() ;
		}
	}

	int texSize = r3dGetTextureSizeInVideoMemory(Width, Height, Depth, mips, TexFormat);
	return texSize * m_iNumTextures;
}

void r3dTexture::RegisterCreated()
{
	Flags     |= fCreated;
	sprintf(Location.FileName, "$Created");
	
	InterlockedExchange( &Instances, 1 );
}

int r3dTexture::Create(int XSize, int YSize, D3DFORMAT TexFmt, int _NumMipMaps, D3DPOOL Pool /*= D3DPOOL_MANAGED*/ )
{
	r3dD3DTextureTunnel texTun;

	m_Pool = Pool;

	r3dDeviceTunnel::CreateTexture(	XSize, YSize, _NumMipMaps,
									(Pool == D3DPOOL_DEFAULT) ? D3DUSAGE_DYNAMIC : 0,
									TexFmt, Pool,
									&texTun );

	Setup( XSize, YSize, 1, TexFmt, _NumMipMaps, &texTun, false );

	RegisterCreated();

#if 0
	r3dOutToLog("r3dTexture: Texture [%dx%d] created\n", Width, Height);
#endif

	return 1;
}

//------------------------------------------------------------------------

void r3dTexture::Destroy()
{
	r3d_assert( Flags & fCreated ) ;

	if( !( Flags & fRenderTarget ) )
	{
		int size = -GetTextureSizeInVideoMemory() ;
		UpdateTextureStats( size ) ;
	}

	DestroyInternal() ;

	Flags &= ~fCreated ;

	r3d_assert(Instances == 1);
	InterlockedExchange( &Instances, 0 );

	r3d_assert(m_iNumTextures>0);
	m_iNumTextures = 0 ;
}

//-------------------------------------------------------------------------

int r3dTexture::CreateVolume(int Width, int Height, int Depth, D3DFORMAT TargetTexFormat, int NumMipMaps, D3DPOOL Pool /*= D3DPOOL_MANAGED*/ )
{
	r3dD3DTextureTunnel tex3D;

	m_Pool = Pool;
	
	r3dDeviceTunnel::CreateVolumeTexture
	(
		Width,
		Height,
		Depth,
		NumMipMaps,
		(Pool == D3DPOOL_DEFAULT) ? D3DUSAGE_DYNAMIC : 0,
		TargetTexFormat,
		Pool,
		&tex3D
	);

	Setup( Width, Height, Depth, TargetTexFormat, NumMipMaps, &tex3D, false );

	RegisterCreated();

#if 0
	r3dOutToLog("r3dTexture: Texture [%dx%d] created\n", Width, Height);
#endif

	return 1;
}

//------------------------------------------------------------------------

int
r3dTexture::CreateCubemap( int EdgeLength, D3DFORMAT TargetTexFormat, int aNumMipMaps, D3DPOOL Pool /*= D3DPOOL_MANAGED*/ )
{
	r3dD3DTextureTunnel texTunnel ;

	m_Pool = Pool;

	r3dDeviceTunnel::CreateCubeTexture(
		EdgeLength,
		aNumMipMaps,
		(Pool == D3DPOOL_DEFAULT) ? D3DUSAGE_DYNAMIC : 0,
		TargetTexFormat,
		Pool,
		&texTunnel );

	SetupCubemap( EdgeLength, TargetTexFormat, aNumMipMaps, &texTunnel, false );
	
	NumMipMaps	= aNumMipMaps;

	RegisterCreated();

	return 1;
}

//------------------------------------------------------------------------
int r3dTexture::Save(const char* Name, bool bFullMipChain/* = false*/)
{
	D3DXIMAGE_FILEFORMAT	Format = D3DXIFF_TGA;

	char drive[ 16 ], path[ 512 ], name[ 512 ], ext[ 128 ];

	_splitpath( Name, drive, path, name, ext );

	if ( !stricmp(ext,".BMP") ) Format = D3DXIFF_BMP;
	if ( !stricmp(ext,".DDS") ) Format = D3DXIFF_DDS; 

	HRESULT hres = 0 ;

	if ( bFullMipChain )
		hres = D3DXSaveTextureToFile ( Name, Format, AsTex2D(), NULL ) ;
	else
	{
		IDirect3DSurface9 	*L0 = NULL;
		AsTex2D()->GetSurfaceLevel(0, &L0);
		hres = D3DXSaveSurfaceToFile(Name, Format, L0, NULL, NULL) ;
		L0->Release();
	}

	if( hres != S_OK )
	{
		r3dOutToLog( "r3dTexture::Save: Couldn't save to : %s\n", Name ) ;
		return 0 ;
	}

	return 1 ;
}

// r3d:: helpers
void _InsertTexture(r3dTexture **FirstTexture, r3dTexture *Tex)
{
	r3dCSHolderWithDeviceQueue csholder( g_ResourceCritSection ) ; (void)csholder ;
	
	Tex->pPrev        = NULL;
	Tex->pNext        = *FirstTexture;
	if(Tex->pNext)
		Tex->pNext->pPrev = Tex;
	*FirstTexture     = Tex;
}

r3dTexture* _CreateTexture()
{
	r3dCSHolderWithDeviceQueue csholder( g_ResourceCritSection ) ; (void)csholder ;
	// ID in constructor is simple ++, if called from more than one thread at the same time will fuck up ID for textures
	return gfx_new r3dTexture;
}

void _DeleteTexture(r3dTexture* tex)
{
	r3dCSHolderWithDeviceQueue csholder( g_ResourceCritSection ) ; (void)csholder ;

	r3d_assert(tex);
	delete tex;
}


r3dTexture *r3dRenderLayer::AllocateTexture()
{
	r3dCSHolderWithDeviceQueue csholder( g_ResourceCritSection ) ; (void)csholder ;

	r3dTexture	*Tex = _CreateTexture();

	_InsertTexture(&FirstTexture, Tex);

	return Tex;
}

r3dTexture* r3dRenderLayer::LoadTexture( const char* TexFile, D3DFORMAT TexFormat, bool bCheckFormat, int DownScale /*= 1*/, int MaxDim /*= 0*/, D3DPOOL Pool /*= D3DPOOL_MANAGED*/, int gameResourcePool /*= 0*/, bool deferLoading /*= false*/ )
{
	if(!bInited)
		return NULL;

	r3dCSHolderWithDeviceQueue csholder( g_ResourceCritSection ) ; (void)csholder ;

	assert(TexFile);

	char szFileName[ MAX_PATH ];
	FixFileName( TexFile, szFileName );

	r3dTexture*	Tex;

	// goes thru all textures and see if we already have with that name
	for(Tex = FirstTexture; Tex; Tex = Tex->pNext)
	{
		int isLoose = 0;

		int needLoad = (!bCheckFormat || (Tex->GetD3DFormat() == TexFormat) )
								&& 
						(
							( strcmp(Tex->getFileLoc().FileName, szFileName ) == NULL)
								||
							( isLoose = r_loose_texture_filter->GetBool() && !r3dCompareFileNameInPath( Tex->getFileLoc().FileName, szFileName ) )
						);

		if( needLoad ) 
		{
#ifndef FINAL_BUILD
			extern bool g_bEditMode;
			if( isLoose && g_bEditMode )
			{
				if( !r3dFilesEqual( Tex->getFileLoc().FileName, szFileName ) )
				{
					static std::set< std::string > reported;

					std::string compound = std::string( Tex->getFileLoc().FileName ) + szFileName;

					if( reported.find( compound ) == reported.end() )
					{
						reported.insert( compound );
						r3dArtBug( "Textures '%s' and '%s' are loosely filtered but are different!\n", Tex->getFileLoc().FileName, szFileName );
					}
				}
			}
#endif

			if( !deferLoading )
			{
				Tex->Load();
			}
			return Tex;
		}
	}

	Tex = _CreateTexture();

	if( gameResourcePool == PlayerTexMem )
	{
		Tex->MarkPlayerTexture();
	}

	if( deferLoading )
	{
		Tex->SetLoadData( szFileName, DownScale, MaxDim, Pool, TexFormat );
	}
	else
	{
		if(!Tex->Load(szFileName, TexFormat, DownScale, MaxDim, Pool )) 
		{
			_DeleteTexture(Tex);
			return NULL;
		}
	}

	//r3dOutToLog("TEXTURE: LoadTexture %s complete\n", szFileName);

	// insert to LList.
	_InsertTexture(&FirstTexture, Tex);

	return Tex;
}

//------------------------------------------------------------------------

r3dThumbnailTexture* r3dRenderLayer::LoadThumbnailTexture( const char* texFile, int downScale, int maxDim, D3DPOOL pool, int gameResourcePool )
{
	for( int i = 0, e = (int)ThumbnailTextures.Count(); i < e; i ++ )
	{
		r3dThumbnailTexture* tex = ThumbnailTextures[ i ];

		if( !stricmp( tex->GetFilePath(), texFile ) )
		{
			tex->LoadFullSizeTexture();
			return tex;
		}
	}

	r3dThumbnailTexture* tex = gfx_new r3dThumbnailTexture;

	tex->Init( texFile, downScale, maxDim, pool, gameResourcePool );

	ThumbnailTextures.PushBack( tex );

	return tex;
}

//------------------------------------------------------------------------

int r3dRenderLayer::GetThumbnailTextureCount() const
{
	return ThumbnailTextures.Count();
}

//------------------------------------------------------------------------

r3dThumbnailTexture* r3dRenderLayer::GetThumbnailTexture( int idx )
{
	return ThumbnailTextures[ idx ];
}


//------------------------------------------------------------------------

void r3dRenderLayer::DeleteThumbnailTexture( r3dThumbnailTexture* tex )
{
	for( int i = 0, e = (int)ThumbnailTextures.Count(); i < e; i ++ )
	{
		if( tex == ThumbnailTextures[ i ] )
		{
			tex->Unload();

			r3d_assert( tex->GetRefs() >= 0 );
			break;
		}
	}
}

//------------------------------------------------------------------------

void r3dRenderLayer::PurgeThumbnailTextures()
{
	typedef r3dgfxSet(r3dTexture*) TextureSet;

	TextureSet fullSizeTexes;

	for( int i = 0, e = (int)ThumbnailTextures.Count(); i < e; i ++ )
	{
		r3dThumbnailTexture* tnt = ThumbnailTextures[ i ];

		fullSizeTexes.insert( tnt->GetFullSizeTexture() );
		delete tnt;
	}

	ThumbnailTextures.Clear();

	for( TextureSet::iterator i = fullSizeTexes.begin(), e = fullSizeTexes.end(); i != e ; ++ i )
	{
		DeleteTexture( *i );
	}
}

//------------------------------------------------------------------------

void r3dRenderLayer::DeletePersistentTextures()
{
	r3dTexture* nextTex;
	for( r3dTexture* tex = FirstTexture; tex ; tex = nextTex )
	{
		nextTex = tex->pNext;

		if( tex->bPersistent )
		{
			if( tex->GetRefs() )
			{
				r3dOutToLog( "Possible texture leak for '%s'(%d)\n", tex->getFileLoc().FileName, tex->GetRefs() );
			}
			else
			{
				DeleteTexture( tex, 1 );
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------
void r3dRenderLayer::ReloadTextureData( const char * texFile )
{
	if(!bInited)
		return;

	assert( texFile );
	char szFileName[ MAX_PATH ];
	FixFileName( texFile, szFileName );


	for( r3dTexture * Tex = FirstTexture; Tex; Tex = Tex->pNext)
	{
		if( Tex->PendingAsyncOps() || !r3dIsSamePath(Tex->getFileLoc().FileName, szFileName) )
			continue;

		Tex->DestroyResources( false );
		Tex->LoadResources();

		for( int i = 0, e = gTextureReloadListeners.Count() ; i < e ; i ++ )
		{
			gTextureReloadListeners[ i ]( Tex ) ;
		}
	}
}




void r3dRenderLayer::DeleteTextureDelayed(r3dTexture *Tex)
{
	if( !Tex )
		return;

	TexturesToDelete.PushBack( Tex );
}

//------------------------------------------------------------------------

void r3dRenderLayer::DeleteTexture(r3dTexture* Tex, int bForceDelete )
{
	if( !Tex )
		return;

	if( R3D_IS_MAIN_THREAD() )
	{
		int message = 0;
		for( ; Tex->PendingAsyncOps() ; )
		{
			if( !message )
			{
				r3dOutToLog( "r3dRenderLayer::DeleteTexture: Load/Unload collision for '%s'\n", Tex->getFileLoc().FileName );
				message = 1;
			}

			ProcessDeviceQueue( r3dGetTime(), 0.033f );
		}
	}

	r3dCSHolderWithDeviceQueue csholder( g_ResourceCritSection ) ; (void)csholder ;

	r3d_assert( Tex->bPersistent || Tex->GetRefs() || bForceDelete || !(Tex->GetFlags() & r3dTexture::fFileTexture) );

	if( Tex->GetRefs() )
	{
		Tex->Unload( false );
	}
	else
	{
		r3d_assert( Tex->bPersistent || !(Tex->GetFlags() & r3dTexture::fFileTexture) );
	}

	if( ( Tex->bPersistent || Tex->GetRefs() ) && !bForceDelete )
		return;

	if(Tex->pNext)
		Tex->pNext->pPrev = Tex->pPrev;
	if(Tex->pPrev)
		Tex->pPrev->pNext = Tex->pNext;
	if(Tex == FirstTexture)
		FirstTexture      = Tex->pNext;

	_DeleteTexture(Tex);
}

//------------------------------------------------------------------------

void r3dRenderLayer::ProcessDeletedTextures()
{
	for( int i = 0, e = TexturesToDelete.Count(); i < e; i ++ )
	{
		DeleteTexture( TexturesToDelete[ i ] );
	}

	TexturesToDelete.Clear();
}

//------------------------------------------------------------------------
/*static*/

void
r3dTexture::Init()
{
	g_TextureLoadTaskParams.Init( 1024 );
	g_TextureUnloadTaskParams.Init( 1024 );
}

//------------------------------------------------------------------------
/*static*/

void
r3dTexture::Close()
{
}

//------------------------------------------------------------------------

void AddTextureReloadListener( TextureReloadListener listener )
{
	gTextureReloadListeners.PushBack( listener ) ;
}

void RemoveTextureReloadListener( TextureReloadListener listener )
{
	for( int i = 0, e = gTextureReloadListeners.Count() ; i < e ;  )
	{
		if( gTextureReloadListeners[ i ] == listener )
		{
			gTextureReloadListeners.Erase( i ) ;
			e -- ;
		}
		else
		{
			i ++ ;
		}
	}
}

bool HasTextureReloadListener( TextureReloadListener listener )
{
	for( int i = 0, e = gTextureReloadListeners.Count() ; i < e ;  )
	{
		if( gTextureReloadListeners[ i ] == listener )
		{
			return true ;
		}
	}

	return false ;
}

//------------------------------------------------------------------------

r3dThumbnailTexture::r3dThumbnailTexture()
: FullSizeTex( NULL )
{

}

//------------------------------------------------------------------------

r3dThumbnailTexture::~r3dThumbnailTexture()
{
	r3dRenderer->DeleteTexture( FullSizeTex );

	ReleasePoolDefaultThumnail();
	ReleaseSysMemThumbnail();
}

//------------------------------------------------------------------------

void r3dThumbnailTexture::Init( const char* path, int fullSizeTextureDownScale, int fullSizeTextureMaxDim, D3DPOOL pool, int gameResourcePool )
{
	r3d_assert( !SysMemThumbnail.Valid() && !PoolDefaultThumbnail.Valid() );

	r3dFile *ff = r3d_open( path, "rb" );

	if( !ff )
	{
		ff = r3d_open( "Data\\Shaders\\Texture\\MissingTexture.dds", "rb" );
	}

	r3d_assert( ff );

	if( ff )
	{		
		int ff_size = ff->size;
		if( ff_size == 0 ) // bad texture???
			r3dError( "Bad texture '%s' - has zero size!!!\n", path );

		void* SrcData = malloc( ff_size + 1 );

		if( SrcData == NULL )
			r3dError( "r3dThumbnailTexture::LoadThumbnail: out of memory!" );

		fread( SrcData, 1, ff_size, ff );
		fclose( ff );

		D3DXIMAGE_INFO info;
		ZeroMemory( &info, sizeof info );

		D3DXGetImageInfoFromFileInMemory( SrcData, ff_size, &info );

		int thumbWidth;
		int thumbHeight;

		if( info.Width > info.Height )
		{
			thumbWidth	= THUMBNAIL_DIM;
			thumbHeight	= info.Height * THUMBNAIL_DIM / info.Width;
		}
		else
		{
			thumbWidth	= info.Width * THUMBNAIL_DIM / info.Height;
			thumbHeight	= THUMBNAIL_DIM;
		}

		// DXT constraints
		thumbWidth = R3D_MAX( thumbWidth, 4 );
		thumbHeight = R3D_MAX( thumbHeight, 4 );

		// TODO : load completely, then chose mip!
		r3dDeviceTunnel::D3DXCreateTextureFromFileInMemoryEx( SrcData, ff_size, thumbWidth, thumbHeight, 0, 0, info.Format, D3DPOOL_SYSTEMMEM, D3DX_FILTER_POINT, D3DX_FILTER_POINT, 0, NULL, NULL, &SysMemThumbnail, "THUMBNAIL" );

		TotalThumbnailSizeInSysMem += GetThumbnailByteSize();
		free( SrcData );
	}

	FullSizeTex = r3dRenderer->LoadTexture( path, D3DFMT_FROM_FILE, false, fullSizeTextureDownScale, fullSizeTextureMaxDim, pool, gameResourcePool, true );
	FullSizeTex->bPersistent = 1;
}

//------------------------------------------------------------------------

void r3dThumbnailTexture::UpdateTexture( r3dTexture* newFullSizeTexture )
{
	if( FullSizeTex != newFullSizeTexture )
	{
		r3dRenderer->DeleteTexture( FullSizeTex );

		ReleasePoolDefaultThumnail();
		ReleaseSysMemThumbnail();

		Init( newFullSizeTexture->getFileLoc().FileName, newFullSizeTexture->GetDownScale(), newFullSizeTexture->GetMaxDim(), newFullSizeTexture->GetPool(), newFullSizeTexture->GetFlags() & r3dTexture::fPlayerTexture ? PlayerTexMem : TexMem );
	}
}

//------------------------------------------------------------------------

void r3dThumbnailTexture::LoadFullSizeTexture()
{
	r3d_assert( FullSizeTex );
	{
		FullSizeTex->Load();
		return;
	}
}

//------------------------------------------------------------------------

static void MakeThumbNailInMainThread( void * param )
{
	((r3dThumbnailTexture*)param)->DoMakeThumbnail();
}

void r3dThumbnailTexture::MakeThumbnail()
{
	if( R3D_IS_MAIN_THREAD() )
	{
		DoMakeThumbnail();
	}
	else
	{
		ProcessCustomDeviceQueueItem( MakeThumbNailInMainThread, this );
	}
}

//------------------------------------------------------------------------

void r3dThumbnailTexture::RecreatePoolDefaultThumbnail()
{
	if( !FullSizeTex->GetRefs() )
	{
		MakeThumbnail();
	}
}

//------------------------------------------------------------------------

void r3dThumbnailTexture::ReleasePoolDefaultThumnail()
{
	if( PoolDefaultThumbnail.Valid() )
	{
		AddThumbToVMEM( -GetThumbnailByteSize() );
		PoolDefaultThumbnail.ReleaseAndReset();
	}
}

//------------------------------------------------------------------------

void r3dThumbnailTexture::Unload()
{
	FullSizeTex->Unload();
}

//------------------------------------------------------------------------

void r3dThumbnailTexture::Update()
{
	if( FullSizeTex->IsDrawable() )
	{
		ReleasePoolDefaultThumnail();
	}
}

//------------------------------------------------------------------------

int r3dThumbnailTexture::HasPoolDefaultThumbnail() const
{
	return PoolDefaultThumbnail.Valid();
}

//------------------------------------------------------------------------

int r3dThumbnailTexture::IsFullSize() const
{
	return FullSizeTex->IsDrawable();
}

//------------------------------------------------------------------------

int r3dThumbnailTexture::GetNumMipmaps()
{
	if( FullSizeTex->IsDrawable() )
		return FullSizeTex->GetNumMipmaps();
	else
		return THUMBNAIL_MIPS;
}

//------------------------------------------------------------------------

D3DFORMAT r3dThumbnailTexture::GetD3DFormat() const
{
	D3DSURFACE_DESC sdesc;
	SysMemThumbnail.GetLevelDesc2D( 0, &sdesc );

	return sdesc.Format;
}

//------------------------------------------------------------------------

int r3dThumbnailTexture::GetThumbnailByteSize()
{
	int mips = SysMemThumbnail.GetLevelCount();

	D3DSURFACE_DESC sdesc;

	SysMemThumbnail.GetLevelDesc2D( 0, &sdesc );

	return r3dGetTextureSizeInVideoMemory( sdesc.Width, sdesc.Height, 1, mips, sdesc.Format );
}

//------------------------------------------------------------------------

const char* r3dThumbnailTexture::GetFilePath() const
{
	return FullSizeTex->getFileLoc().FileName;
}

//------------------------------------------------------------------------

void r3dThumbnailTexture::AddThumbToVMEM( int size )
{
	r3dRenderer->Stats.AddTexMem( size );
	TotalThumbnailSizeInVMem += size;
}

//------------------------------------------------------------------------

int r3dThumbnailTexture::TotalThumbnailSizeInVMem = 0;

//------------------------------------------------------------------------

int r3dThumbnailTexture::TotalThumbnailSizeInSysMem = 0;

//------------------------------------------------------------------------

void r3dThumbnailTexture::DoMakeThumbnail()
{
	r3d_assert( !PoolDefaultThumbnail.Valid() );

	D3DSURFACE_DESC highestMipDesc;
	SysMemThumbnail.GetLevelDesc2D( 0, &highestMipDesc );

	IDirect3DTexture9* srcTex = SysMemThumbnail.AsTex2D();

	r3dDeviceTunnel::CreateTexture( highestMipDesc.Width, highestMipDesc.Height, SysMemThumbnail.GetLevelCount(), 0, highestMipDesc.Format, D3DPOOL_DEFAULT, &PoolDefaultThumbnail );

	IDirect3DTexture9* destTex = PoolDefaultThumbnail.AsTex2D();

	for( int i = 0, e = SysMemThumbnail.GetLevelCount(); i < e; i ++ )
	{
		IDirect3DSurface9* src( NULL );
		D3D_V( srcTex->GetSurfaceLevel( i, &src ) );

		IDirect3DSurface9* dest( NULL );
		D3D_V( destTex->GetSurfaceLevel( i, &dest ) );

		D3D_V( r3dRenderer->pd3ddev->UpdateSurface( src, NULL, dest, NULL ) );

		SAFE_RELEASE( src );
		SAFE_RELEASE( dest );
	}

	AddThumbToVMEM( GetThumbnailByteSize() );
}

//------------------------------------------------------------------------

void r3dThumbnailTexture::ReleaseSysMemThumbnail()
{
	if( SysMemThumbnail.Valid() )
	{
		TotalThumbnailSizeInSysMem -= GetThumbnailByteSize();
	}

	SysMemThumbnail.ReleaseAndReset();
}

//------------------------------------------------------------------------

void CheckLoadTextureStack( r3dTL::TArray<r3dBackgroundTaskDispatcher::TaskDescriptor> * tasks )
{
#if 0
	r3dgfxVector( r3dTexture* ) processed;

	processed.reserve( 1024 );
	
	r3dgfxVector( int ) sequence;

	sequence.reserve( 128 );

	for( int i = 0, e = (int)tasks->Count(); i < e; i ++ )
	{
		r3dBackgroundTaskDispatcher::TaskDescriptor& td = (*tasks)[ i ];

		if( td.Fn == r3dTexture::LoadTexture || td.Fn == r3dTexture::DestroyTextureResources )
		{
			r3dTexture* loadee;

			if( td.Fn == r3dTexture::LoadTexture )
			{
				TextureLoadTaskParams* params = (TextureLoadTaskParams*)td.Params;
				loadee = params->Loadee;
			}
			else
			{
				TextureUnloadTaskParams* params = (TextureUnloadTaskParams*)td.Params;
				loadee = params->UnLoadee;
			}

			if( find( processed.begin(), processed.end(), loadee ) != processed.end() )
				continue;

			processed.push_back( loadee );

			sequence.clear();

			for( int j = 0, e = (int)tasks->Count(); j < e; j ++ )
			{
				r3dBackgroundTaskDispatcher::TaskDescriptor& td = (*tasks)[ j ];

				if( td.Fn == r3dTexture::LoadTexture && ((TextureLoadTaskParams*)td.Params)->Loadee == loadee )
				{
					sequence.push_back( 1 );
				}

				if( td.Fn == r3dTexture::DestroyTextureResources && ((TextureUnloadTaskParams*)td.Params)->UnLoadee == loadee )
				{
					sequence.push_back( 0 );
				}
			}

			char seqBuff[ 1024 ];

			for( int i = 0, e = (int)sequence.size(); i < e; i ++ )
			{
				seqBuff[ i ] = sequence[ i ] ? '+' : '-';
			}

			seqBuff[ sequence.size() ] = 0;

			if( sequence.size() > 1 )
			{
				r3dOutToLog( "Texutre stack(%d): '%s': %s\n", tasks->Count(), loadee->getFileLoc().FileName, seqBuff );
			}
		}
	}
#endif
}

#ifndef FINAL_BUILD

static float g_LastReportStats;

void DEBUG_ReportTexStats()
{
	float newTime = r3dGetTime();

	if( newTime - g_LastReportStats > 1.0f && g_CollapsedLoadUnloadTextureCount )
	{
		r3dOutToLog( "Loads/unloads collapsed: %d\n", g_CollapsedLoadUnloadTextureCount );
		g_CollapsedLoadUnloadTextureCount = 0;

		g_LastReportStats = newTime;
	}
}

#endif