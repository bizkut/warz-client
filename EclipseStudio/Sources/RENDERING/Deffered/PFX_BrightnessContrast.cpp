#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "PFX_BrightnessContrast.h"

//------------------------------------------------------------------------

PFX_BrightnessContrast::Settings::Settings()
: brightness( 0.0f )
, constrast( 1.0f )

{
}

//------------------------------------------------------------------------

PFX_BrightnessContrast::PFX_BrightnessContrast()
: Parent( this )
{

}

//------------------------------------------------------------------------

PFX_BrightnessContrast::~PFX_BrightnessContrast()
{

}

//------------------------------------------------------------------------

void
PFX_BrightnessContrast::PushSettings( const Settings& sts )
{
	mSettings.PushBack( sts );
	mSettingsPushed = 1;
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_BrightnessContrast::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_BRIGHTNESS_CONTRAST" );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_BrightnessContrast::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_BrightnessContrast::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* /*src*/ ) /*OVERRIDE*/
{
	r3d_assert( mSettings.Count() );

	const Settings& sts = mSettings[ 0 ];

	float vConst[ 4 ] = { sts.constrast, -0.5f * sts.constrast + sts.brightness + 0.5f, 0.f, 0.f };

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, vConst, 1 ) );

	mSettings.Erase( 0 );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_BrightnessContrast::FinishImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------

/*virtual*/
void
PFX_BrightnessContrast::PushDefaultSettingsImpl()	/*OVERRIDE*/
{
	mSettings.PushBack( Settings() );
}