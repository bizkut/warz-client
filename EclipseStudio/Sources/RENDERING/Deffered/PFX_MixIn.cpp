#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "PFX_MixIn.h"

//------------------------------------------------------------------------
PFX_MixIn::Settings::Settings()
: Sel ( SEL_RGBB )
, BlendMode ( R3D_BLEND_ADD | R3D_BLEND_NZ )
, ColorWriteMask ( D3DCOLORWRITEENABLE_ALPHA )
{
	
}

//------------------------------------------------------------------------
PFX_MixIn::PFX_MixIn()
: Parent( this )
{
	memset( PixelShaderIDs, -1, sizeof PixelShaderIDs ) ;
}

//------------------------------------------------------------------------
PFX_MixIn::~PFX_MixIn()
{

}

//------------------------------------------------------------------------

void
PFX_MixIn::SetSettings( const Settings& sts )
{
	mSettings = sts ;
}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_MixIn::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_MIXIN_RGBB" );

#define APPEND_SHADER(mask) PixelShaderIDs[SEL_##mask] = r3dRenderer->GetPixelShaderIdx( "PS_MIXIN_" #mask ) ;
	APPEND_SHADER( RGBB ) ;
#undef APPEND_SHADER
}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_MixIn::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_MixIn::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* src )	/*OVERRIDE*/
{
	r3dRenderer->SetRenderingMode( mSettings.BlendMode | R3D_BLEND_PUSH );

	const Settings& sts = mSettings ;

	if( sts.ColorWriteMask != PostFXChief::DEFAULT_COLOR_WRITE_MASK )
	{		
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, sts.ColorWriteMask ) );
	}

	mData.PixelShaderID = PixelShaderIDs[ sts.Sel ] ;
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_MixIn::FinishImpl() /*OVERRIDE*/
{
	r3dRenderer->SetRenderingMode( R3D_BLEND_POP );

	if( mSettings.ColorWriteMask != PostFXChief::DEFAULT_COLOR_WRITE_MASK )
	{
		g_pPostFXChief->SetDefaultColorWriteMask();
	}
}

//------------------------------------------------------------------------
