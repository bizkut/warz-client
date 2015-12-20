#pragma once

#include "r3d.h"
#include "r3dRender.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class r3dRenderBuffer : public r3dIResource
{
protected:

	uint32_t				m_dwDataLength;	
	uint32_t				m_dwItemSize;	
	uint32_t				m_dwFVF;			
	int						m_nLockOffset;	
	int						m_nLockLength;	
	uint32_t				m_dwLockFlags;	
	int						m_Category;

	bool					m_bIsDynamic;
	uint32_t				m_dwUsage;

							r3dRenderBuffer		( int nItemCount, int nItemSize, uint32_t dwFVF, int category, const r3dIntegrityGuardian& ig );
	virtual					~r3dRenderBuffer		();

	void					IncrementStats			();
	void					DecrementStats			();

	template < typename T > 
	R3D_FORCEINLINE void* Lock( T& lockee, int nSkipItems, int nNumItems, uint32_t dwFlags = 0 ); 

public:

	R3D_FORCEINLINE uint32_t	GetDataLength		() const		{ return m_dwDataLength; }
	R3D_FORCEINLINE uint32_t	GetItemSize			() const		{ return m_dwItemSize; }	
	R3D_FORCEINLINE uint32_t	GetFVF				() const		{ return m_dwFVF; }
	/// get total item count
	R3D_FORCEINLINE int			GetItemCount		() const		{ return m_dwDataLength / m_dwItemSize; }

};


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	CVertexBufferDX
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class r3dVertexBuffer : public r3dRenderBuffer
{
	r3dD3DVertexBufferTunnel m_pBuffer ;

	void				ReleaseResource( bool DeviceReset );

public:
	IDirect3DVertexBuffer9* GetResource() const ;

	virtual	void		D3DCreateResource();
	virtual	void		D3DReleaseResource();

	void *				Lock				( int nSkipItems = 0, int nNumItems = 0, uint32_t dwFlags = 0 );

	void				Unlock				();

	void				Set					( int nStream, int nOffset = 0 );

						r3dVertexBuffer		( int nItemCount, int nItemSize, uint32_t dwFVF = 0, bool bDynamic = false, bool bReadable = false, int category = -1, const r3dIntegrityGuardian& ig = r3dIntegrityGuardian() );
	virtual				~r3dVertexBuffer	();
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class r3dIndexBuffer : public r3dRenderBuffer
{
	r3dD3DIndexBufferTunnel	m_pBuffer;

	void				ReleaseResource( bool DeviceReset );

public:

	IDirect3DIndexBuffer9* GetResource() const ;

	virtual	void		D3DCreateResource();
	virtual	void		D3DReleaseResource();

	void *				Lock				( int nSkipItems = 0, int nNumItems = 0, uint32_t dwFlags = 0 );
	void				Unlock				();


	void				Set					();

						r3dIndexBuffer		( int nItemCount, bool bDynamic = false, int nItemSize = 2, int category = -1, const r3dIntegrityGuardian& ig = r3dIntegrityGuardian() );
	virtual				~r3dIndexBuffer		();
};

template < typename T >
struct TUnlock
{
	TUnlock( T* a_lockee )
	{
		lockee = a_lockee ;
	}

	~TUnlock()
	{
		lockee->Unlock() ;
	}

	T* lockee ;
};
