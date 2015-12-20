#include "r3dPCH.h"
#include "r3d.h"

#include "CommonPostFX.h"

#include "PostFXChief.h"
#include "PFX_ExplosionBlur.h"

PFX_NightVision::PFX_NightVision()
: Parent( this )
, colorRemapTex(0)
{
}

//------------------------------------------------------------------------

PFX_NightVision::~PFX_NightVision()
{

}

//------------------------------------------------------------------------

void
PFX_NightVision::SetDefaultSettings( const Settings& settings )
{
	mDefaultSettings = settings;
}

//------------------------------------------------------------------------

const
PFX_NightVision::Settings&
PFX_NightVision::GetDefaultSettings() const
{
	return mDefaultSettings;
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_NightVision::InitImpl()	/*OVERRIDE*/
{	
	mData.PixelShaderID		= r3dRenderer->GetPixelShaderIdx( "PS_NIGHT_VISION" );
	colorRemapTex = r3dRenderer->LoadTexture("Data\\Shaders\\Texture\\NightVisionColorRemap.dds");
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_NightVision::CloseImpl() /*OVERRIDE*/
{
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_NightVision::PrepareImpl( r3dScreenBuffer* /*dest*/, r3dScreenBuffer* /*src*/ ) /*OVERRIDE*/
{
	const Settings *settings = &mDefaultSettings;
	if (mSettingsArr.Count() > 0)
		settings = &mSettingsArr[0];

	r3dRenderer->SetTex(colorRemapTex, 1);

	if (mSettingsArr.Count() > 0)
		mSettingsArr.Erase(0);
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_NightVision::FinishImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------

void
PFX_NightVision::PushSettings( const Settings& settings )
{
	mSettingsArr.PushBack(settings);
}