#include "r3dPCH.h"

#include "r3dBudgeter.h"
#include "RenderBuffer.h"

#include "r3dDeviceQueue.h"


//--------------------------------------------------------------------------------------------------------
r3dRenderBuffer::r3dRenderBuffer( int nItemCount, int nItemSize, uint32_t dwFVF, int category, const r3dIntegrityGuardian& ig )
: r3dIResource( ig )
{
	m_dwDataLength = nItemCount * nItemSize;
	m_dwItemSize = nItemSize;
	m_dwFVF = dwFVF;

	m_nLockOffset = 0;
	m_nLockLength = 0;
	m_dwLockFlags = 0;

	m_Category = category;
}


//--------------------------------------------------------------------------------------------------------
r3dRenderBuffer::~r3dRenderBuffer()
{

}

//------------------------------------------------------------------------

void r3dRenderBuffer::DecrementStats()
{
	r3dGPUStats& s = r3dRenderer->Stats;

	s.AddBufferMem ( -(int)m_dwDataLength );

	switch(m_Category)
	{
	case TerrainBufferMem: s.AddTerrainBufferDXMem( -(int)m_dwDataLength ); break;
	case GrassBufferMem: s.AddGrassBufferDXMem ( -(int)m_dwDataLength ); break;
	default: s.AddOtherBufferDXMem ( -(int)m_dwDataLength ); break;
	}	
}

void r3dRenderBuffer::IncrementStats()
{
	r3dGPUStats& s = r3dRenderer->Stats;
	
	s.AddBufferMem ( m_dwDataLength );
	
	switch(m_Category)
	{
	case TerrainBufferMem: s.AddTerrainBufferDXMem ( m_dwDataLength ); break;
	case GrassBufferMem: s.AddGrassBufferDXMem ( m_dwDataLength ); break;
	default: s.AddOtherBufferDXMem ( m_dwDataLength ); break;
	}
	
}

//------------------------------------------------------------------------

template < typename T > 
R3D_FORCEINLINE
void*
r3dRenderBuffer::Lock( T& lockee, int nSkipItems, int nNumItems, uint32_t dwFlags )
{
	void* data = NULL;

	uint32_t nSizeToLock ;

	if( nNumItems )
	{
		nSizeToLock = nNumItems * m_dwItemSize ;
	}
	else
	{
		nSizeToLock = m_dwDataLength - nSkipItems * m_dwItemSize ;
	}

	uint32_t nLockStart = nSkipItems * m_dwItemSize ;

	r3d_assert( nLockStart + nSizeToLock <= m_dwDataLength );

	uint32_t dwAddFlags = D3DLOCK_NOSYSLOCK;

	if( m_bIsDynamic )
	{
		if ( nSkipItems )
		{
			dwAddFlags |= D3DLOCK_NOOVERWRITE;   // append
			r3dRenderer->Stats.AddNumLocksNooverwrite ( 1 ) ;
		}
		else
		{
			dwAddFlags |= D3DLOCK_DISCARD;       // flush
			r3dRenderer->Stats.AddNumLocksDiscard ( 1 ) ;
		}
	}

	m_dwLockFlags = dwAddFlags | dwFlags;

	lockee.Lock( nLockStart, nSizeToLock, &data, m_dwLockFlags );

	r3dRenderer->Stats.AddBytesLocked ( nSizeToLock ) ;
	r3dRenderer->Stats.AddNumLocks ( 1 ) ;

	return data;		
}

//--------------------------------------------------------------------------------------------------------
r3dVertexBuffer::r3dVertexBuffer( int nItemCount, int nItemSize, uint32_t dwFVF, bool bDynamic, bool bReadable /*= false*/, int category /* = -1*/, const r3dIntegrityGuardian& ig /*= r3dIntegrityGuardian()*/ )
	: r3dRenderBuffer ( nItemCount, nItemSize, dwFVF, category, ig )
{
	m_bIsDynamic = bDynamic;
	m_dwUsage = bReadable ? 0 : D3DUSAGE_WRITEONLY;

	if ( bDynamic )
	{
		m_dwUsage |= D3DUSAGE_DYNAMIC;
	}

	if( bDynamic )
	{
		r3dOutToLog( "Creating dynamic buffer of size %.2f MB\n", nItemCount * nItemSize / 1024.f / 1024.f );
	}

	CreateQueuedResource( this ) ;
}

//--------------------------------------------------------------------------------------------------------
r3dVertexBuffer::~r3dVertexBuffer()
{
	ReleaseResource( false );
}

void SetD3DResourcePrivateData(LPDIRECT3DRESOURCE9 res, const char* FName);

//--------------------------------------------------------------------------------------------------------
void r3dVertexBuffer::ReleaseResource( bool DeviceReset )
{
	if ( ! m_pBuffer.Valid() )
		return;

	// no need to release on device reset if managed ( non dynamic in our case )
	if( !DeviceReset || m_bIsDynamic )
	{
		m_pBuffer.ReleaseAndReset();

		r3dRenderBuffer::DecrementStats();
	}
}

//------------------------------------------------------------------------

IDirect3DVertexBuffer9*
r3dVertexBuffer::GetResource() const
{
	return m_pBuffer.Get() ;
}

//------------------------------------------------------------------------
/*virtual*/

void
r3dVertexBuffer::D3DCreateResource()
{
	if ( m_pBuffer.Valid() )
		return;

	r3dDeviceTunnel::CreateVertexBuffer( m_dwDataLength, m_dwUsage, m_dwFVF, m_bIsDynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &m_pBuffer );

	SetD3DResourcePrivateData(m_pBuffer.Get(), "r3dVertexBuffer");

	r3dRenderBuffer::IncrementStats();

}

//------------------------------------------------------------------------
/*virtual*/

void
r3dVertexBuffer::D3DReleaseResource()
{
	R3D_ENSURE_MAIN_THREAD();

	ReleaseResource( true ) ;
}

//--------------------------------------------------------------------------------------------------------
void r3dVertexBuffer::Set( int nStream, int nOffset )
{
	r3d_assert( m_pBuffer.Valid() );
	d3dc._SetStreamSource( nStream, m_pBuffer.Get(), nOffset * m_dwItemSize, m_dwItemSize );
}

//--------------------------------------------------------------------------------------------------------
void * r3dVertexBuffer::Lock( int nSkipItems /*= 0*/, int nNumItems /*= 0*/, uint32_t dwFlags /*= 0*/ )
{
	return r3dRenderBuffer::Lock( m_pBuffer, nSkipItems, nNumItems, dwFlags ) ;
}

//--------------------------------------------------------------------------------------------------------
void r3dVertexBuffer::Unlock()
{
	m_pBuffer.Unlock();
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------------------
r3dIndexBuffer::r3dIndexBuffer( int nItemCount, bool bDynamic, int nItemSize, int category /* = -1*/, const r3dIntegrityGuardian& ig /*= r3dIntegrityGuardian()*/ ) 
: r3dRenderBuffer( nItemCount, nItemSize, nItemSize == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, category, ig )
{
	assert( nItemSize == 2 || nItemSize==4 );

	m_bIsDynamic = bDynamic;

	if ( bDynamic )
	{
		m_dwUsage = D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC;
	}
	else
	{
		m_dwUsage = D3DUSAGE_WRITEONLY;
	}

	if( bDynamic )
	{
		r3dOutToLog( "Creating dynamic buffer of size %.2f MB\n", nItemCount * nItemSize / 1024.f / 1024.f );
	}

	CreateQueuedResource( this ) ;
}

//--------------------------------------------------------------------------------------------------------
r3dIndexBuffer::~r3dIndexBuffer()
{
	ReleaseResource( false );
}


//--------------------------------------------------------------------------------------------------------
void r3dIndexBuffer::ReleaseResource( bool DeviceReset )
{
	if ( ! m_pBuffer.Valid() )
		return;

	// no need to do anything on reset if buffer is managed
	if( !DeviceReset || m_bIsDynamic )
	{
		m_pBuffer.ReleaseAndReset();
		r3dRenderBuffer::DecrementStats();
	}
}

//------------------------------------------------------------------------

IDirect3DIndexBuffer9*
r3dIndexBuffer::GetResource() const
{
	return m_pBuffer.Get() ;
}

//------------------------------------------------------------------------
/*virtual*/

void
r3dIndexBuffer::D3DCreateResource()
{
	if ( m_pBuffer.Valid() )
		return;

	r3dDeviceTunnel::CreateIndexBuffer( m_dwDataLength, m_dwUsage | D3DUSAGE_WRITEONLY, (D3DFORMAT)m_dwFVF, m_bIsDynamic ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED, &m_pBuffer ) ;

	SetD3DResourcePrivateData(m_pBuffer.Get(), "r3dIndexBuffer");

	r3dRenderBuffer::IncrementStats();

}

//------------------------------------------------------------------------
/*virtual*/

void
r3dIndexBuffer::D3DReleaseResource()
{
	R3D_ENSURE_MAIN_THREAD();

	ReleaseResource( true ) ;
}

//------------------------------------------------------------------------

void*
r3dIndexBuffer::Lock( int nSkipItems /*= 0*/, int nNumItems /*= 0*/, uint32_t dwFlags /*= 0*/ )
{
	return r3dRenderBuffer::Lock( m_pBuffer, nSkipItems, nNumItems, dwFlags ) ;
}

//--------------------------------------------------------------------------------------------------------
void r3dIndexBuffer::Unlock()
{
	m_pBuffer.Unlock();
}

//--------------------------------------------------------------------------------------------------------
void r3dIndexBuffer::Set()
{
	d3dc._SetIndices( m_pBuffer.Get() ) ;
}
