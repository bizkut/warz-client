#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "RenderDeffered.h"

#include "PFX_Copy.h"

//------------------------------------------------------------------------
PFX_Copy::Settings::Settings()
: TexScaleX ( 1.f )
, TexScaleY ( 1.f )
, TexOffsetX( 0.f )
, TexOffsetY( 0.f )
, SkyOnly( false )
, ForceFiltering( false )
{

}

//------------------------------------------------------------------------
PFX_Copy::PFX_Copy( int BlendMode, int colorWriteMask )
: Parent( this )
, mBlendMode( BlendMode | R3D_BLEND_NZ )
, mColorWriteMask( colorWriteMask )
, mNeedRestoreFiltering( 0 )
{

}

//------------------------------------------------------------------------
PFX_Copy::~PFX_Copy()
{

}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_Copy::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_COPY" );
}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_Copy::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_Copy::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* src )	/*OVERRIDE*/
{
	r3dRenderer->SetRenderingMode( mBlendMode | R3D_BLEND_PUSH );

	const Settings& sts = mSettingsArr[ 0 ];

	mData.TexTransform[ 0 ] = sts.TexScaleX ;
	mData.TexTransform[ 1 ] = sts.TexScaleY ;
	mData.TexTransform[ 2 ] = sts.TexOffsetX ;
	mData.TexTransform[ 3 ] = sts.TexOffsetY ;

	if( fabs( dest->Width - src->Width * sts.TexScaleX ) > 0.125f
			|| 
		fabs( dest->Height - src->Height * sts.TexScaleY ) > 0.125f
			||
		sts.ForceFiltering
		)
	{
		r3dSetFiltering( R3D_BILINEAR, 0 );
		mNeedRestoreFiltering = 1 ;
	}
	else
	{
		mNeedRestoreFiltering = 0 ;
	}

	if( mColorWriteMask != PostFXChief::DEFAULT_COLOR_WRITE_MASK )
	{		
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, mColorWriteMask ) );
	}

	if( sts.SkyOnly )
	{
		SetupLightMaskStencilStates( SCM_UNLITAREA );
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, TRUE ) ) ;
	}
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_Copy::FinishImpl() /*OVERRIDE*/
{
	r3dRenderer->SetRenderingMode( R3D_BLEND_POP );

	if( mColorWriteMask != PostFXChief::DEFAULT_COLOR_WRITE_MASK )
	{
		g_pPostFXChief->SetDefaultColorWriteMask();
	}

	if( mNeedRestoreFiltering )
	{
		if( g_pPostFXChief->GetZeroTexStageFilter() )
			r3dSetFiltering( R3D_BILINEAR, 0 );
		else
			r3dSetFiltering( R3D_POINT, 0 );

		mNeedRestoreFiltering = 0 ;
	}

	const Settings& sts = mSettingsArr[ 0 ];

	if( sts.SkyOnly )
	{
		D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, FALSE ) ) ;
	}

	mSettingsArr.Erase( 0 );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_Copy::PushDefaultSettingsImpl() /*OVERRIDE*/
{
	mSettingsArr.PushBack( Settings() ) ;
}

//------------------------------------------------------------------------

void
PFX_Copy::PushSettings( const Settings& settings ) 
{
	mSettingsArr.PushBack( settings );

	mSettingsPushed = 1;
}
