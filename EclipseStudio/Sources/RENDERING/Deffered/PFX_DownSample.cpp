#include "r3dPCH.h"
#include "r3d.h"

#include "CommonPostFX.h"

#include "PostFXChief.h"
#include "PFX_DownSample.h"

PFX_DownSample::Settings::Settings()
: ColorWriteMask(	D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | 
					D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA )
, TexMultiplyU( 1.0f )
, TexMultiplyV( 1.0f )

, TexAddU( 0.0f )
, TexAddV( 0.0f )
{

}

//------------------------------------------------------------------------

PFX_DownSample::PFX_DownSample()
: Parent( this )
{
}

//------------------------------------------------------------------------

PFX_DownSample::~PFX_DownSample()
{

}

//------------------------------------------------------------------------

void
PFX_DownSample::PushSettings( const Settings& sts )
{
	mSettingsVec.PushBack( sts ) ;

	mSettingsPushed = 1 ;
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_DownSample::InitImpl()	/*OVERRIDE*/
{	
	mData.PixelShaderID		= r3dRenderer->GetPixelShaderIdx( "PS_DOWNSAMPLE" );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_DownSample::CloseImpl() /*OVERRIDE*/
{
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_DownSample::PrepareImpl( r3dScreenBuffer* /*dest*/, r3dScreenBuffer* src ) /*OVERRIDE*/
{
	Settings sts = mSettingsVec[ 0 ] ;

	mSettingsVec.Erase( 0 ) ;

	mData.TexTransform[ 0 ] = sts.TexMultiplyU ;
	mData.TexTransform[ 1 ] = sts.TexMultiplyV ;
	mData.TexTransform[ 2 ] = sts.TexAddU ;
	mData.TexTransform[ 3 ] = sts.TexAddV ;

	float vConst[ 4 ] = { 1.0f / src->Width, 1.0f / src->Height, 0.f, 0.f } ;

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, vConst, 1 ) ) ;

	D3D_V( r3dRenderer->pd3ddev->SetRenderState(D3DRS_COLORWRITEENABLE, sts.ColorWriteMask ) ) ;
	
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_DownSample::FinishImpl() /*OVERRIDE*/
{
	g_pPostFXChief->SetDefaultColorWriteMask() ;
}

//------------------------------------------------------------------------

/*virtual*/
void
PFX_DownSample::PushDefaultSettingsImpl() /*OVERRIDE*/
{
	mSettingsVec.PushBack( Settings() ) ;
}