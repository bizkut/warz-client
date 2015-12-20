#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "PFX_Fill.h"

//------------------------------------------------------------------------

PFX_Fill::Settings::Settings()
: Value( 0.f, 0.f, 0.f, 0.f )
, ColorWriteMask( D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA )
{

}

//------------------------------------------------------------------------
PFX_Fill::PFX_Fill()
: Parent( this )
{

}

//------------------------------------------------------------------------
PFX_Fill::~PFX_Fill()
{

}

//------------------------------------------------------------------------

void
PFX_Fill::PushSettings( const PFX_Fill::Settings& settings )
{
	mSettingsStack.PushBack( settings );
}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_Fill::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_FILL" );
}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_Fill::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_Fill::PrepareImpl( r3dScreenBuffer* /*dest*/, r3dScreenBuffer* /*src*/ )	/*OVERRIDE*/
{
	const Settings& sts = mSettingsStack[ 0 ];

	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, &sts.Value.x, 1 );

	if( sts.ColorWriteMask != PostFXChief::DEFAULT_COLOR_WRITE_MASK )
	{		
		r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, sts.ColorWriteMask );
	}
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_Fill::FinishImpl() /*OVERRIDE*/
{
	Settings sts = mSettingsStack[ 0 ];

	mSettingsStack.Erase( 0 );

	if( sts.ColorWriteMask != PostFXChief::DEFAULT_COLOR_WRITE_MASK )
	{
		g_pPostFXChief->SetDefaultColorWriteMask();
	}
}

