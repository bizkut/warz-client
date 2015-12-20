#ifndef	__R3D_D3DCACHE_H
#define	__R3D_D3DCACHE_H


class IDirect3D9Cache
{
  public:
	IDirect3DVertexShader9*	pVS;
	IDirect3DPixelShader9*	pPS;
	LPDIRECT3DVERTEXDECLARATION9	pDecl;

	IDirect3DIndexBuffer9*	pIB;

	enum
	{
		NUM_STREAM_SOURCES = 16
	};

	IDirect3DVertexBuffer9*		VBuffs[ NUM_STREAM_SOURCES ] ;
	UINT						VBOffsets[ NUM_STREAM_SOURCES ] ;
	UINT						VBStrides[ NUM_STREAM_SOURCES ] ;
  
  public:
	IDirect3D9Cache();
	~IDirect3D9Cache();

	void		Reset();
	
	HRESULT		_SetVertexShader(IDirect3DVertexShader9 *v);
	HRESULT		_SetPixelShader(IDirect3DPixelShader9 *v);
	void		_SetDecl(LPDIRECT3DVERTEXDECLARATION9 decl);
	void		_SetStreamSource( UINT idx, IDirect3DVertexBuffer9* buff, UINT offset, UINT stride );
	void		_SetIndices( IDirect3DIndexBuffer9* ibuff );

	void		_ZeroZeroStreamCache();
	void		_ZeroIndicesCahce();
};

R3D_FORCEINLINE HRESULT IDirect3D9Cache::_SetVertexShader(IDirect3DVertexShader9 *v) 
{
	if ( pVS != v )
	{
		pVS = v;
		return r3dRenderer->pd3ddev->SetVertexShader( v ) ;
	}

	return S_OK ;
}

R3D_FORCEINLINE HRESULT IDirect3D9Cache::_SetPixelShader(IDirect3DPixelShader9* v)
{
	if ( pPS != v )
	{
		pPS = v;
		return r3dRenderer->pd3ddev->SetPixelShader( v ) ;
	}

	return S_OK ;
}

R3D_FORCEINLINE void IDirect3D9Cache::_SetDecl(LPDIRECT3DVERTEXDECLARATION9 decl)
{
	if( pDecl != decl )
	{
		pDecl = decl;
#undef SetVertexDeclaration

		r3d_assert( decl ) ;
		D3D_V( r3dRenderer->pd3ddev->SetVertexDeclaration(decl) );
#define SetVertexDeclaration DIRECT_CALLS_OF_SetVertexDeclaration_FUNCTION_NOT_ALLOWED_USE_D3DC_
	}
}

R3D_FORCEINLINE void IDirect3D9Cache::_SetStreamSource( UINT idx, IDirect3DVertexBuffer9* buff, UINT offset, UINT stride )
{
	r3d_assert( idx < R3D_ARRAYSIZE( VBuffs ) );

	if( VBOffsets[ idx ] != offset ||
		VBuffs[ idx ] != buff ||
		VBStrides[ idx ] != stride )
	{
		VBuffs[ idx ] = buff ;
		VBOffsets[ idx ] = offset ;
		VBStrides[ idx ] = stride ;

		D3D_V( r3dRenderer->pd3ddev->SetStreamSource( idx, buff, offset, stride ) );
	}
}

R3D_FORCEINLINE void IDirect3D9Cache::_SetIndices( IDirect3DIndexBuffer9* ibuff )
{
	if( pIB != ibuff )
	{
		pIB = ibuff;

		D3D_V( r3dRenderer->pd3ddev->SetIndices( ibuff ) ) ;
	}
}


extern	IDirect3D9Cache	d3dc;

#endif // __R3D_D3DCACHE_H

