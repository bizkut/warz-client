#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "PFX_Combine.h"

//------------------------------------------------------------------------

PFX_Combine::Settings::Settings()
: mask ( NULL )
{
	choseChannel[ 0 ] = 0.f;
	choseChannel[ 1 ] = 0.f;
	choseChannel[ 2 ] = 0.f;
	choseChannel[ 3 ] = 1.f;
}

//------------------------------------------------------------------------

PFX_Combine::PFX_Combine( int BlendMode, int colorWriteMask )
: Parent( this )
, mBlendMode( BlendMode | R3D_BLEND_NZ )
, mColorWriteMask( colorWriteMask )
{

}

//------------------------------------------------------------------------

PFX_Combine::~PFX_Combine()
{

}

//------------------------------------------------------------------------

void
PFX_Combine::PushSettings( const Settings& sts )
{
	mSettings.PushBack( sts );
	mSettingsPushed = 1;
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_Combine::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_COMBINE" );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_Combine::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_Combine::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* /*src*/ ) /*OVERRIDE*/
{
	r3d_assert( mSettings.Count() );

	const Settings& sts = mSettings[ 0 ];

	r3dRenderer->SetTex( sts.mask->Tex, PostFXChief::FREE_TEX_STAGE_START );

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, sts.choseChannel, 1 ) );

	mSettings.Erase( 0 );

	r3dRenderer->SetRenderingMode( mBlendMode | R3D_BLEND_PUSH );

	if( mColorWriteMask != PostFXChief::DEFAULT_COLOR_WRITE_MASK )
	{		
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, mColorWriteMask ) );
	}
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_Combine::FinishImpl() /*OVERRIDE*/
{
	r3dRenderer->SetRenderingMode( R3D_BLEND_POP );

	if( mColorWriteMask != PostFXChief::DEFAULT_COLOR_WRITE_MASK )
	{
		g_pPostFXChief->SetDefaultColorWriteMask();
	}
}

//------------------------------------------------------------------------

/*virtual*/
void
PFX_Combine::PushDefaultSettingsImpl()	/*OVERRIDE*/
{
	mSettings.PushBack( Settings() );
}