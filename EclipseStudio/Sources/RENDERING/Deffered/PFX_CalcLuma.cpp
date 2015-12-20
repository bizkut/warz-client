#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "PFX_CalcLuma.h"


//------------------------------------------------------------------------
PFX_CalcLuma::PFX_CalcLuma( )
: Parent( this )
, mDebugChecker( NULL )
{

}

//------------------------------------------------------------------------
PFX_CalcLuma::~PFX_CalcLuma()
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_CalcLuma::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_CALC_LUMA" );

#if 0
#ifdef _DEBUG
	mDebugChecker = r3dRenderer->LoadTexture( "data/shaders/texture/checker1600x900.dds" ) ;
#endif
#endif

}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_CalcLuma::CloseImpl() /*OVERRIDE*/
{
	if( mDebugChecker )
		r3dRenderer->DeleteTexture( mDebugChecker ) ;
}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_CalcLuma::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* src )	/*OVERRIDE*/
{

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED ) ) ;

#if 0
	r3dRenderer->SetTex( mDebugChecker, 4 ) ;
#endif
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_CalcLuma::FinishImpl() /*OVERRIDE*/
{
	g_pPostFXChief->SetDefaultColorWriteMask() ;
}

//------------------------------------------------------------------------
