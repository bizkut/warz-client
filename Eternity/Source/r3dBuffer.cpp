#include "r3dPCH.h"
#include "r3d.h"

#include "r3dBuffer.h"

#include "r3dDeviceQueue.h"

static	HRESULT		hr;

r3dD3DQuery::r3dD3DQuery(D3DQUERYTYPE type, const r3dIntegrityGuardian& ig /*= r3dIntegrityGuardian()*/ )
: r3dIResource( ig )
{
	type_ = type;
	CreateQueuedResource( this ) ;
}

r3dD3DQuery::~r3dD3DQuery()
{
	ReleaseQueuedResource( this ) ;
}

void r3dD3DQuery::D3DCreateResource()
{
	D3D_V( r3dRenderer->pd3ddev->CreateQuery(type_, &query_) ) ;
}

void r3dD3DQuery::D3DReleaseResource()
{
	R3D_ENSURE_MAIN_THREAD();
	SAFE_RELEASE(query_);
}

//------------------------------------------------------------------------
/*static*/
r3dD3DSurfaceTunnel r3dScreenBuffer::ShadowZBuf		;
r3dD3DTextureTunnel r3dScreenBuffer::ShadowZBufTex	;
r3dD3DSurfaceTunnel r3dScreenBuffer::NullRT			;

/*static*/ int r3dScreenBuffer::ActivateGuard ;

struct ShadowZBuffer : r3dIResource
{
	ShadowZBuffer( const r3dIntegrityGuardian& ig )
	: r3dIResource( ig )
	{
		X = 64 ;
		Y = 64 ;
		UsedMemory = 0 ;
	}

	static void CreateZSurfaces(	r3dD3DTextureTunnel* oTexture,
									r3dD3DSurfaceTunnel* oDepthStencil,
									r3dD3DSurfaceTunnel* oNullRT,
									int X, int Y,
									int* oUsedMemory
								)
	{
		bool needTex = false ;
		D3DFORMAT fmt = D3DFMT_D24X8 ;

		switch( r_hardware_shadow_method->GetInt() )
		{
		case HW_SHADOW_METHOD_R32F:
			needTex = false ;			
			break;
		case HW_SHADOW_METHOD_INTZ:
			fmt = INTZ_FORMAT ;
		case HW_SHADOW_METHOD_HW_PCF:
			needTex = true ;
			break;
		}

		*oUsedMemory = 0 ;

		if( needTex )
		{
			r3dDeviceTunnel::CreateTexture( X, Y, 1, D3DUSAGE_DEPTHSTENCIL, fmt, D3DPOOL_DEFAULT, oTexture ) ;

			IDirect3DSurface9* surf ;
			oTexture->AsTex2D()->GetSurfaceLevel( 0, &surf ) ;

			oDepthStencil->Set( surf ) ;

			D3DFORMAT nullFmt = r3dRenderer->SupportsNULLRenderTarget ? NULL_RT_FORMAT : D3DFMT_R5G6B5 ;
			r3dDeviceTunnel::CreateRenderTarget( X, Y, nullFmt, D3DMULTISAMPLE_NONE, 0, FALSE, oNullRT ) ;

			if( !r3dRenderer->SupportsNULLRenderTarget )
			{
				*oUsedMemory += int( GetD3DTexFormatSize( nullFmt ) * (int)(X * Y) ) ;
			}
		}
		else
		{
			oTexture->Set( 0 ) ;
			r3dDeviceTunnel::CreateDepthStencilSurface( X, Y, fmt, D3DMULTISAMPLE_NONE, 1, TRUE, oDepthStencil ) ;
		}

		*oUsedMemory += 4* (int)(X * Y) ;
	}

	virtual	void D3DCreateResource()  OVERRIDE
	{
		R3D_ENSURE_MAIN_THREAD();

		r3d_assert( !r3dScreenBuffer::ShadowZBuf.Get() );

		CreateZSurfaces( &r3dScreenBuffer::ShadowZBufTex, &r3dScreenBuffer::ShadowZBuf, &r3dScreenBuffer::NullRT, X, Y, &UsedMemory ) ;
		r3dRenderer->Stats.AddRenderTargetMem ( UsedMemory ) ;
	}

	virtual	void D3DReleaseResource() OVERRIDE
	{
		R3D_ENSURE_MAIN_THREAD();

		r3d_assert( r3dScreenBuffer::ShadowZBuf.Get() );

		D3DSURFACE_DESC desc ;

		D3D_V( r3dScreenBuffer::ShadowZBuf->GetDesc( &desc ) )

		r3dScreenBuffer::ShadowZBuf.ReleaseAndReset() ;
		r3dRenderer->Stats.AddRenderTargetMem ( - UsedMemory ) ;

		r3dScreenBuffer::ShadowZBufTex.ReleaseAndReset() ;

		D3DFORMAT formatForNull = r3dScreenBuffer::NullRT.GetFormat() ;

		r3dScreenBuffer::NullRT.ReleaseAndReset() ;
	}

	int X, Y ;
	int UsedMemory ;
} *gShadowZBuffer;

/*static*/
void
r3dScreenBuffer::CreateShadowZBuffer( int X, int Y )
{
	r3d_assert( !gShadowZBuffer );

	r3dIntegrityGuardian ig ;

	gShadowZBuffer = gfx_new ShadowZBuffer( ig ) ;

	gShadowZBuffer->X = X ;
	gShadowZBuffer->Y = Y ;

	gShadowZBuffer->D3DCreateResource() ;
}

//------------------------------------------------------------------------
/*static*/

void
r3dScreenBuffer::ReleaseShadowZBuffer()
{
	if (!gShadowZBuffer)
		return;

	gShadowZBuffer->D3DReleaseResource();

	delete gShadowZBuffer ;

	gShadowZBuffer = NULL ;
}

r3dScreenBuffer::r3dScreenBuffer(const CreationParameters &p)
: r3dIResource( p.ig )
{
	r3dRenderer->RegisterScreenBuffer( this ) ;

	memset( MipDims, 0, sizeof MipDims ) ;

	debug_name				= p.name;

	Width					= p.width ;
	Height					= p.height ;

	BufferFormat    		= p.Format;
	zType					= p.ZT;
	BufferID        		= 0;
	bCubemap        		= p.bCubemap;
	CreationNumMipLevels	= p.numMipLevels;
	ActualNumMipLevels		= CreationNumMipLevels ;

	UsedMemory				= 0 ;
	Tex = 0;
	ActiveMip = -1;

	AllowAutoScaleDown = p.allowAutoScaleDown;

	D3DCreateResource();
}

//////////////////////////////////////////////////////////////////////////

r3dScreenBuffer::r3dScreenBuffer(const char * name)
: r3dIResource( r3dIntegrityGuardian( ) )
, debug_name(name)
, BufferFormat(D3DFMT_UNKNOWN)
, Width( 0 )
, Height( 0 )
, zType(Z_OWN)
, BufferID(0)
, bCubemap(false)
, CreationNumMipLevels(0)
, ActualNumMipLevels(0)
, Tex(0)
, AllowAutoScaleDown( 0 )
, UsedMemory( 0 )
{
	r3dRenderer->RegisterScreenBuffer( this ) ;
	memset( MipDims, 0, sizeof MipDims ) ;
	ActiveMip = -1;
}

r3dScreenBuffer::~r3dScreenBuffer()
{
	R3D_ENSURE_MAIN_THREAD();
	D3DReleaseResource();

	r3dRenderer->UnregisterScreenBuffer( this ) ;
}

void r3dScreenBuffer::SetDebugD3DComment(const char* text)
{
	if (!Tex) return;
	Tex->SetDebugD3DComment(text);
}

const char*
r3dScreenBuffer::GetDebugName() const
{
	return debug_name ? debug_name : "Unknown" ;
}

void SetD3DResourcePrivateData(LPDIRECT3DRESOURCE9 res, const char* FName);

//------------------------------------------------------------------------

static void SetupSurfaces( void* param )
{
	r3dScreenBuffer* screenBuffer = (r3dScreenBuffer*) param ;

	IDirect3DSurface9* surf ;

	r3dRenderer->GetRT(0, &surf );

	screenBuffer->BBuf.Set( surf ) ;

	r3dRenderer->GetDSS( &surf );

	screenBuffer->ZBuf.Set( surf ) ;
}

bool IsDepthTextureFormat( D3DFORMAT fmt )
{
	return
		fmt == D3DFMT_D24X8 
			||
			fmt == INTZ_FORMAT ;
}

static void SetupSurfacePointers( void* params )
{
	r3dScreenBuffer* sbuf = (r3dScreenBuffer*) params ;

	IDirect3DSurface9* surf ;

	if( sbuf->bCubemap )
	{
		if (sbuf->AsTexCUBE())
		{
			for( int i = 0, e = r3dScreenBuffer::FACE_COUNT; i < e; i ++ )
			{
				for( int m = 0, e = sbuf->AsTexCUBE()->GetLevelCount() ; m < e ; m ++ )
				{
					D3D_V(sbuf->AsTexCUBE()->GetCubeMapSurface( (D3DCUBEMAP_FACES)i, m, &surf ));
					sbuf->Surfs[ i ][ m ].Set( surf );
				}				
			}
		}
	}
	else
	{
		if (sbuf->AsTex2D())
		{
			for( int m = 0, e = sbuf->AsTex2D()->GetLevelCount() ; m < e ; m ++ )
			{
				D3D_V(sbuf->AsTex2D()->GetSurfaceLevel(m, &surf ));
				sbuf->Surfs[ 0 ][ m ].Set( surf ) ;
			}
		}
	}
}

void r3dScreenBuffer::D3DCreateResource()
{
	if (Width == 0 || Height == 0) return;

	r3d_assert( !OurBBuf.Valid() );

	// Store the current back buffer and z-buffer.

	ProcessCustomDeviceQueueItem( SetupSurfaces, this ) ;

	UINT intW = static_cast<UINT>(Width);
	UINT intH = static_cast<UINT>(Height);

	if( BufferFormat != D3DFMT_UNKNOWN )
	{
		if( IsDepthTextureFormat( BufferFormat ) )
		{
			Tex = r3dRenderer->AllocateTexture();

			if( zType == r3dScreenBuffer::Z_SHADOW )
			{
				ShadowZBufTex->AddRef() ;
				Tex->Setup( intW, intH, 1, BufferFormat, 1, &ShadowZBufTex, true ) ;
				Surfs[ 0 ][ 0 ] = r3dScreenBuffer::NullRT ;
				Surfs[ 0 ][ 0 ]->AddRef() ;
			}
			else
			{
				ShadowZBuffer::CreateZSurfaces( &OurBBuf, &OurZBuf, &Surfs[ 0 ][ 0 ], intW, intH, &UsedMemory );
				Tex->Setup( intW, intH, 1, BufferFormat, 1, &OurBBuf, true );
			}
		}
		else
		{
			if( bCubemap )
			{
				r3d_assert( intW == intH );
				r3dDeviceTunnel::CreateCubeTexture( intW, CreationNumMipLevels, D3DUSAGE_RENDERTARGET, BufferFormat, D3DPOOL_DEFAULT, &OurBBuf ) ;
			}
			else
			{
				if( AllowAutoScaleDown )
				{
					UINT W = intW ;
					UINT H = intH ;

					UINT L = (UINT)CreationNumMipLevels ;

					r3dDeviceTunnel::CreateTextureAutoDownScale( &W, &H, &L, D3DUSAGE_RENDERTARGET, BufferFormat, D3DPOOL_DEFAULT, &OurBBuf ) ;

					Width = (float)W ;
					Height = (float)H ;

					intW = W ;
					intH = H ;
				}
				else
					r3dDeviceTunnel::CreateTexture(intW, intH, CreationNumMipLevels, D3DUSAGE_RENDERTARGET, BufferFormat, D3DPOOL_DEFAULT, &OurBBuf ) ;
			}

			Tex = r3dRenderer->AllocateTexture();

			if( bCubemap )
				Tex->SetupCubemap( intW, BufferFormat, ActualNumMipLevels, &OurBBuf, true );
			else
				Tex->Setup( intW, intH, 1, BufferFormat, ActualNumMipLevels, &OurBBuf, true );

			Tex->SetDebugD3DComment("r3dScreenBuffer: Color");

			UsedMemory += r3dGetPixelSize(intW * intH, BufferFormat) * ( bCubemap ? 6 : 1 ) ;

			if( zType == Z_OWN ) 
			{
				r3d_assert( !OurZBuf.Valid() );

				r3dDeviceTunnel::CreateDepthStencilSurface(intW, intH, ZBuf.GetFormat(), D3DMULTISAMPLE_NONE, 0, FALSE, &OurZBuf ) ;

				r3dDeviceTunnel::SetD3DResourcePrivateData( &OurZBuf, "r3dScreenBuffer: DepthStencil");
				UsedMemory += r3dGetPixelSize(intW*intH, ZBuf.GetFormat() ) ;
			}
			ProcessCustomDeviceQueueItem( SetupSurfacePointers, this ) ;
		}

		memset( MipDims, 0, sizeof MipDims ) ;

		if( IsDepthTextureFormat( BufferFormat ) )
		{
			ActualNumMipLevels = 1 ;
			
			Dims &dms = MipDims[ 0 ] ;

			dms.Width	= Width ;
			dms.Height	= Height ;
		}
		else
		{
			ActualNumMipLevels = OurBBuf.GetLevelCount() ;

			for( int i = 0, e = ActualNumMipLevels ; i < e ; i ++ )
			{
				D3DSURFACE_DESC desc ;
				OurBBuf.GetLevelDesc2D( i, &desc ) ;

				Dims &dms = MipDims[ i ] ;

				dms.Width = (float)desc.Width ;
				dms.Height = (float)desc.Height ;

				if( i )
				{
					UsedMemory += r3dGetPixelSize( (int)dms.Width* (int)dms.Height, BufferFormat) * ( bCubemap ? 6 : 1 ) ;
				}
			}
		}

		r3dRenderer->Stats.AddRenderTargetMem ( UsedMemory );
	}
}

void r3dScreenBuffer::D3DReleaseResource()
{
	R3D_ENSURE_MAIN_THREAD();

	if( Surfs[ 0 ][ 0 ].Get() )
	{
		r3dRenderer->DeleteTexture(Tex, true);
		Tex		= NULL ;
		OurBBuf.Set( 0 );	// OurBBuf is already released as Tex->Tex

		r3dRenderer->Stats.AddRenderTargetMem ( -UsedMemory ) ;
		UsedMemory = 0 ;

		if ( OurZBuf.Get() && zType == Z_OWN )
		{
			OurZBuf.ReleaseAndReset();
		}

		BBuf.ReleaseAndReset();
		ZBuf.ReleaseAndReset();

		int refCount = 0 ;

		for( int i = 0, e = FACE_COUNT; i < e; i ++ )
		{
			for( int m = 0, e = ActualNumMipLevels ; m < e ; m ++  )
			{
				refCount = Surfs[ i ][ m ].ReleaseAndReset() ;
			}			
		}

		// have to watch last ref count with cubemaps somehow
		if( refCount > 0 )
		{
			r3dOutToLog( "r3dScreenBuffer::D3DReleaseResource: non zero reference count after release! (%s, %d refs)\n", this->debug_name ?this->debug_name : "Unknown", refCount ) ;
		}

	}

	return;
}



void r3dScreenBuffer::SetMRT(int RTID, int bSet)
{
	if (!Tex) return;

	if (!bSet)
	{
		r3dRenderer->SetRT(RTID, NULL);
		return;
	}

	BufferID	= RTID;

	r3dRenderer->SetRT( RTID, Surfs[ FACE_PX ][ 0 ].Get() );

	return;
}

bool r3dScreenBuffer::IsNull() const
{
	return BufferFormat == D3DFMT_UNKNOWN || !Surfs[ 0 ][ 0 ].Get() ;
}

IDirect3DSurface9* r3dScreenBuffer::GetTex2DSurface()
{
	return Surfs[ FACE_PX ][ 0 ].Get();
}

IDirect3DSurface9* r3dScreenBuffer::GetTexCUBESurface( int face, int mip )
{
	return Surfs[ face ][ mip ].Get();
}

int r3dScreenBuffer::GetShadowZBufSize()
{
	return gShadowZBuffer->X ;
}

int r3dScreenBuffer::GetNumMipLevels() const
{
	if(!Tex) return 0;
	if( bCubemap )
		return Tex->AsTexCUBE()->GetLevelCount();
	else
		return Tex->AsTex2D()->GetLevelCount();
}

void r3dScreenBuffer::Activate(int RTID, int Face /*= FACE_PX*/, int Mip /*= 0*/)
{
	r3d_assert( Mip < ActualNumMipLevels || !ActualNumMipLevels ) ;

	BufferID	= RTID;
	if (ActiveMip != -1)
	{
		r3d_assert(!Mip);
		Mip = ActiveMip;
	}
	r3dD3DSurfaceTunnel s(Surfs[ Face ][ Mip ]);
	if (s.Get())
	{
		r3d_assert( bCubemap || Face == FACE_PX );

		r3dRenderer->SetRT( RTID, s.Get() );
	}

	if ( zType == Z_SYSTEM  )
	{
		r3dRenderer->SetDSS(ZBuf.Get());
	}
	else
	{
		if (!BufferID) 
		{
			if ( zType == Z_OWN )
			{
				r3dRenderer->SetDSS(OurZBuf.Get());
			}
			else
			if( zType == Z_SHADOW )
			{
				r3dRenderer->SetDSS(ShadowZBuf.Get());
			}
			else
			{
				r3dRenderer->SetDSS(NULL);
			}
		}
	}

	if ( !BufferID )
	{
		r3d_assert( !ActivateGuard ) ;

		ActivateGuard ++ ;

		const Dims& dms = MipDims[ Mip ] ;

		OldW                       = r3dRenderer->ScreenW ;
		OldH                       = r3dRenderer->ScreenH ;
		r3dRenderer->ScreenW       = dms.Width ;
		r3dRenderer->ScreenH       = dms.Height ;
		r3dRenderer->ScreenW2      = r3dRenderer->ScreenW / 2 ;
		r3dRenderer->ScreenH2      = r3dRenderer->ScreenH / 2 ;

		if( s.Get() )
		{
			r3dRenderer->SetViewport(0,0,r3dRenderer->ScreenW, r3dRenderer->ScreenH);
		}
	}

}


void r3dScreenBuffer::Deactivate(int bReset)
{
	if (BufferID)
	{
		r3dRenderer->SetRT(BufferID, NULL);

		//  r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORWRITEENABLE1, 0x00000000 );

		return;
	}

	r3d_assert( ActivateGuard == 1 ) ;
	ActivateGuard -- ;

	if (bReset && BBuf.Get())
	{
		r3dRenderer->SetRT(0, BBuf.Get());
		r3dRenderer->SetDSS(ZBuf.Get());
	}

	r3dRenderer->ScreenW  = OldW;
	r3dRenderer->ScreenH  = OldH;
	r3dRenderer->ScreenW2 = r3dRenderer->ScreenW / 2;
	r3dRenderer->ScreenH2 = r3dRenderer->ScreenH / 2;

	r3dRenderer->SetViewport(0,0,r3dRenderer->ScreenW, r3dRenderer->ScreenH);

	//  if (bUseSystemZ)
	//    r3dRenderer->pd3ddev->StretchRect(r3dRenderer->RTBuffer, NULL, surf1, NULL, D3DTEXF_NONE);

}



r3dD3DBuffer::r3dD3DBuffer(type_e type, int size, int stride, LPDIRECT3DVERTEXDECLARATION9 decl, const r3dIntegrityGuardian& ig /*= r3dIntegrityGuardian()*/)
: r3dIResource( ig )
{
	r3d_assert( r3dRenderer && r3dRenderer->pd3ddev );

	m_LockSize  = 0;
	m_LockStart = 0;

	m_Type   = type;
	m_Size   = size;
	m_Stride = stride;
	m_Pos    = 0;
	m_NeedDiscard = 0;

	m_Decl    = decl;

	R3D_ENSURE_MAIN_THREAD();

	D3DCreateResource();
}

r3dD3DBuffer::~r3dD3DBuffer()
{
	R3D_ENSURE_MAIN_THREAD();

	D3DReleaseResource();
}

void r3dD3DBuffer::D3DCreateResource()
{
	R3D_ENSURE_MAIN_THREAD();

	r3d_assert(m_Size);
	r3d_assert(m_Stride);

	switch(m_Type) 
	{
	default: r3d_assert(0);

	case BUFFER_Index: 
		hr = r3dRenderer->pd3ddev->CreateIndexBuffer(
			m_Size * m_Stride,
			D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 
			m_Stride == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32,
			D3DPOOL_DEFAULT,
			&pIB, 
			NULL);

		r3dRenderer->CheckOutOfMemory( hr ) ;

		if(!SUCCEEDED(hr)) 
			r3dError("failed (%x) to create index buffer (%d bytes)\n", hr, m_Size * m_Stride);

		SetD3DResourcePrivateData(pIB, "r3dD3DBuffer: IB");
		r3dRenderer->Stats.AddBufferMem ( m_Size * m_Stride );
		break;

	case BUFFER_Vertex:
		hr = r3dRenderer->pd3ddev->CreateVertexBuffer(
			m_Size * m_Stride,
			D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
			0,
			D3DPOOL_DEFAULT,
			&pVB, 
			NULL);
		
		r3dRenderer->CheckOutOfMemory( hr ) ;

		if(!SUCCEEDED(hr)) 
			r3dError("failed (%x) to create vertex buffer (%d bytes)\n", hr, m_Size * m_Stride);

		SetD3DResourcePrivateData(pVB, "r3dD3DBuffer: VB");
		r3dRenderer->Stats.AddBufferMem ( + m_Size * m_Stride );
		break;
	} 

	return;
}

void r3dD3DBuffer::D3DReleaseResource()
{
	R3D_ENSURE_MAIN_THREAD();

	switch(m_Type) 
	{
	default: r3d_assert(0);
	case BUFFER_Index:	SAFE_RELEASE(pIB); break;
	case BUFFER_Vertex: SAFE_RELEASE(pVB); break;
	}

	r3dRenderer->Stats.AddBufferMem ( -(int) m_Size * m_Stride );

	return;
}

void* r3dD3DBuffer::LockData(int size, int *lstart)
{
	r3d_assert(pIB);
	r3d_assert(!m_LockSize);
	if((unsigned int)size > m_Size) r3dError("r3dD3DBuffer::LockData overflow %d vs %d", size, m_Size);

	// No overwrite will be used if the data can fit into the space remaining in the buffer.
	DWORD dwLockFlags = D3DLOCK_NOOVERWRITE;

	// Check to see if the entire buffer has been used up yet.
	if(m_Pos + size >= m_Size || m_NeedDiscard) {
		// No space remains. Start over from the beginning 
		dwLockFlags   = D3DLOCK_DISCARD ;
		m_Pos         = 0;
		m_NeedDiscard = 0;
		r3dRenderer->Stats.AddNumLocksDiscard ( 1 ) ;
	}
	else
		r3dRenderer->Stats.AddNumLocksNooverwrite ( 1 ) ;

	// Lock the buffer.
	void* pOut;

	switch(m_Type) {
	case BUFFER_Index:
		hr = pIB->Lock(m_Pos * m_Stride, size * m_Stride, &pOut, dwLockFlags );
		break;
	case BUFFER_Vertex:
		hr = pVB->Lock(m_Pos * m_Stride, size * m_Stride, &pOut, dwLockFlags );
		break;
	}

	r3d_assert( hr == S_OK );

	// locked start/size
	m_LockStart = m_Pos;
	m_LockSize  = size;
	// advance to the next position
	m_Pos      += size;

	if(lstart)
		*lstart = m_LockStart;

	r3dRenderer->Stats.AddBytesLocked ( + size * m_Stride ) ;
	r3dRenderer->Stats.AddNumLocks ( 1 ) ;

	return pOut;
}

void r3dD3DBuffer::Unlock()
{
	r3d_assert(m_LockSize);

	switch(m_Type) {
	case BUFFER_Index:
		hr = pIB->Unlock();
		break;
	case BUFFER_Vertex:
		hr = pVB->Unlock();
		break;
	}

	m_LockSize = 0;
}

void r3dD3DBuffer::Activate()
{
	r3d_assert(pIB);

	switch(m_Type) 
	{
	default: r3d_assert(0);
	case BUFFER_Index:
		d3dc._SetIndices(pIB);
		break;
	case BUFFER_Vertex:
		d3dc._SetDecl(m_Decl);
		d3dc._SetStreamSource(0, GetVB(), 0, m_Stride);
		break;
	}

	return;
}


void r3dVertexArray::D3DCreateResource()
{
	r3dDeviceTunnel::CreateVertexBuffer(size_*stride_, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &vrtBuff_ ) ;
	r3dDeviceTunnel::SetD3DResourcePrivateData( &vrtBuff_, "r3dVertexArray: VB");
}

void r3dVertexArray::D3DReleaseResource()
{
	R3D_ENSURE_MAIN_THREAD();

	if( !vrtBuff_.Valid() )	return;

	//unlock buffer if still locked

	if(lockedPtr_!=0)	vrtBuff_->Unlock();
	vrtBuff_.ReleaseAndReset();
}

r3dVertexArray::r3dVertexArray( const r3dIntegrityGuardian& ig )
: r3dIResource( ig )
{
	size_ = 0;
	pos_  = 0;
	type = TRIANGLE_LIST;
}

r3dVertexArray::~r3dVertexArray()
{
	R3D_ENSURE_MAIN_THREAD();
	D3DReleaseResource();
}

/*
handle errors
1) unlock when is not locked
2) lock of already locked
3) 0 bytes for lock
4) unlock but not filled completely
5) overflow
6) flush when is still locked
7) lock more then entire buffer size
8) reset when locked
r3d_assert(num >= iVertexVol);
*/
r3dVertexArray*	r3dCreateVertexArray(int size, int stride, LPDIRECT3DVERTEXDECLARATION9 decl, r3dVertexArray::Type type )
{
	r3dIntegrityGuardian ig ;

	r3dVertexArray* t = gfx_new r3dVertexArray( ig );

	t->size_   = size;
	t->pos_    = 0;
	t->actual_    = 0;
	t->unflushed_ = 0;
	t->stride_ = stride;
	t->decl_    = decl;
	t->needed_ = 0;
	t->lockedPtr_ = 0;
	t->type = type;

	R3D_ENSURE_MAIN_THREAD();

	t->D3DCreateResource();

	return t;
}


//------------------------------------------------------------------------
void r3dVertexArray::AddList( const void* vtx, int num)
{
	StartListVertices(num);
	SetListVtx(vtx, num);
	EndListVertices();
}
void r3dVertexArray::StartListVertices(int num)
{
	needed_ = num;
	Lock();
}
void r3dVertexArray::SetListVtx(const void* vtx, int num)
{
	if(actual_ + num > needed_) r3dError("r3dVertexArray::SetListVtx overflow\n");

	memcpy( lockedPtr_ + actual_*stride_, vtx , num * stride_ );
	actual_ += num;
}
void r3dVertexArray::EndListVertices()
{
	Unlock();
}






void r3dVertexArray::Add(const void* vtx, int num)
{
	StartVertices(num, 1);
	SetVtx(vtx, num);
	EndVertices();
}
void r3dVertexArray::StartVertices(int numVertsInPoly, int numPoly)
{
	switch( type )
	{
	case TRIANGLE_LIST:
		needed_ = (numVertsInPoly - 2) * 3 * numPoly ;
		break ;
	case LINE_LIST:
		needed_ = numVertsInPoly * numPoly ;
		break ;
	case LINE_STRIP:
		needed_ = numPoly + 1 ;
		break ;
	}

	Lock();
}
void r3dVertexArray::SetVtx(const void* vtx, int num)
{
	if(actual_ + num > needed_) r3dError("r3dVertexArray::SetVtx overflow\n");

	switch( type )
	{
	case LINE_LIST:
	case LINE_STRIP:
		for(int i = 0; i < num; i++) 
			AddVtx ( vtx, 1 );
		break ;
	case TRIANGLE_LIST:
		for(int i = 1; i < num-1; i++)
		{
			AddVtx(vtx, 0);
			AddVtx(vtx, i);
			AddVtx(vtx, i+1);
		}
		break ;
	}
}
void r3dVertexArray::EndVertices()
{
	Unlock();
	needed_ = 0;
}





void r3dVertexArray::Lock()
{
	if(needed_ == 0) return;

	if(needed_ > size_) r3dError("r3dVertexArray::Lock overflow %d vs %d\n", needed_, size_);
	if(lockedPtr_ != 0) r3dError("r3dVertexArray::Lock error: already locked\n");

	unsigned int flg = D3DLOCK_NOOVERWRITE;
	if(pos_ + unflushed_ + needed_ >= size_)
	{
		r3dRenderer->Stats.AddNumLocksDiscard( 1 );

		Flush();
		flg = D3DLOCK_DISCARD;
		pos_ = 0;
	}
	else
	{
		r3dRenderer->Stats.AddNumLocksNooverwrite( 1 );
	}

	UINT toLock = needed_ * stride_;

	r3dRenderer->Stats.AddBytesLocked( +toLock );
	r3dRenderer->Stats.AddNumLocks( 1 );

	vrtBuff_->Lock((pos_ + unflushed_)* stride_, toLock, (void**)&lockedPtr_, flg);
}
void r3dVertexArray::Unlock()
{
	if(needed_ == 0) return;

	if(lockedPtr_ == 0) r3dError("r3dVertexArray::Unlock error: is not locked\n");
	if(actual_ < needed_) r3dError("r3dVertexArray::Unlock error: the buffer is not filled completely\n");

	vrtBuff_->Unlock();
	unflushed_ += actual_;
	actual_ = 0;
	lockedPtr_ = 0;
}



void r3dVertexArray::Flush()
{
	if(lockedPtr_ != 0) r3dError("r3dVertexArray::Flush error: is still locked\n");

	if(unflushed_ == 0)	return;

	d3dc._SetDecl(decl_);
	d3dc._SetStreamSource(0, vrtBuff_.Get(), 0, stride_);

	switch( type )
	{
	case LINE_LIST:
		r3dRenderer->Draw(D3DPT_LINELIST, pos_, unflushed_ / 2);
		break ;
	case LINE_STRIP:
		r3dRenderer->Draw(D3DPT_LINESTRIP, pos_, unflushed_ - 1 );
		break ;
	case TRIANGLE_LIST:
		r3dRenderer->Draw(D3DPT_TRIANGLELIST, pos_, unflushed_ / 3);
		break ;
	}

	pos_ += unflushed_;
	unflushed_ = 0;
}

int r3dGetShadowZBufferSize()
{
	return gShadowZBuffer->UsedMemory ;
}