#include "r3dPCH.h"
#include "r3d.h"
#include "r3dD3DCache.h"

	IDirect3D9Cache		d3dc;

IDirect3D9Cache::IDirect3D9Cache()
{
  Reset();
}

IDirect3D9Cache::~IDirect3D9Cache()
{
}

#define UPDATE_VAR1(fset, fget, vtype, xx, yy)   return r3dRenderer->pd3ddev-> fset (yy);
#define UPDATE_VAR1_R(fset, fget, vtype, xx, yy) return r3dRenderer->pd3ddev-> fset (yy);


extern	void	fog_UpdateVS(int bVShaderActive);

void IDirect3D9Cache::Reset()
{
	_SetVertexShader(0);
	_SetPixelShader(0);
	pDecl 	= 0;

	pIB		= NULL ;

	for( int i = 0; i < NUM_STREAM_SOURCES ; i ++ )
	{
		VBuffs[ i ]	= 0 ;
		VBOffsets[ i ] = -1 ;
		VBStrides[ i ] = 0 ;
	}
}

void
IDirect3D9Cache::_ZeroZeroStreamCache()
{
	VBuffs[ 0 ] = 0 ;
	VBOffsets[ 0 ] = 0 ;
	VBStrides[ 0 ] = 0 ;
}

void
IDirect3D9Cache::_ZeroIndicesCahce()
{
	pIB = 0 ;
}

