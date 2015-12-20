#include "r3dPCH.h"

#include "r3d.h"

#include "PFX_StencilToMask.h"

#include "PostFXChief.h"

#include "TrueNature/Sun.h"

#include "../DX9/RenderDX9.h"

#include "RenderDeffered.h"

//------------------------------------------------------------------------

PFX_StencilToMask::Settings::Settings()
: Ref( 1 )
, Value( 1.f, 1.f, 1.f, 1.f )
, ColorWriteMask( D3DCOLORWRITEENABLE_ALPHA )
{

}

//------------------------------------------------------------------------
PFX_StencilToMask::PFX_StencilToMask( )
: Parent( this )
{

}

//------------------------------------------------------------------------
PFX_StencilToMask::~PFX_StencilToMask()
{

}

//------------------------------------------------------------------------

void
PFX_StencilToMask::PushSettings( const Settings& sts )
{
	mSettings.PushBack( sts );
}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_StencilToMask::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_FILL" );
}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_StencilToMask::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_StencilToMask::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* /*src*/ )	/*OVERRIDE*/
{
	// cause we use main buffer stencil
	r3d_assert(		dest->Width		<= ScreenBuffer->Width && 
					dest->Height	<= ScreenBuffer->Height );

	// contrary to what some may think, this sets main zbuffer
	r3dRenderer->SetDSS( dest->ZBuf.Get() ) ;

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, true )			);
	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP )	);
	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILREF, 1 )					);
	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILMASK, 0xFFFFFFFF )		);

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL )	);

	const Settings& sts = mSettings[ 0 ] ;

	if( sts.ColorWriteMask != PostFXChief::DEFAULT_COLOR_WRITE_MASK )
	{		
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, sts.ColorWriteMask ) );
	}

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, &sts.Value.x, 1 ) );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_StencilToMask::FinishImpl() /*OVERRIDE*/
{
	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, FALSE ) );

	const Settings& sts = mSettings[ 0 ];

	if( sts.ColorWriteMask != PostFXChief::DEFAULT_COLOR_WRITE_MASK )
	{
		g_pPostFXChief->SetDefaultColorWriteMask();
	}

	mSettings.Erase( 0 );
}

