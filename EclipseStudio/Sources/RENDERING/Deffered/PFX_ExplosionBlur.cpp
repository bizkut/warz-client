#include "r3dPCH.h"
#include "r3d.h"

#include "CommonPostFX.h"

#include "PostFXChief.h"
#include "PFX_ExplosionBlur.h"

PFX_ExplosionBlur::PFX_ExplosionBlur()
: Parent( this )
{
}

//------------------------------------------------------------------------

PFX_ExplosionBlur::~PFX_ExplosionBlur()
{

}

//------------------------------------------------------------------------

void
PFX_ExplosionBlur::SetDefaultSettings( const Settings& settings )
{
	mDefaultSettings = settings;
}

//------------------------------------------------------------------------

const
PFX_ExplosionBlur::Settings&
PFX_ExplosionBlur::GetDefaultSettings() const
{
	return mDefaultSettings;
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ExplosionBlur::InitImpl()	/*OVERRIDE*/
{	
	mData.PixelShaderID		= r3dRenderer->GetPixelShaderIdx( "PS_EXPLOSION_BLUR" );
	mData.VertexShaderID	= r3dRenderer->GetVertexShaderIdx( "VS_EXPLOSION_BLUR" );

	mData.VSType			= PostFXData::VST_CUSTOM;
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ExplosionBlur::CloseImpl() /*OVERRIDE*/
{
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ExplosionBlur::PrepareImpl( r3dScreenBuffer* /*dest*/, r3dScreenBuffer* /*src*/ ) /*OVERRIDE*/
{
	const Settings *settings = &mDefaultSettings;
	if (mSettingsArr.Count() > 0)
		settings = &mSettingsArr[0];

	r3dScreenBuffer *o = g_pPostFXChief->GetBuffer(settings->toBlurImage);
	r3dRenderer->SetTex(o->Tex, PostFXChief::FREE_TEX_STAGE_START);

	D3DXVECTOR4 pconst[1] =
	{
		D3DXVECTOR4(settings->BrightnessThreshold, settings->BlurStrength, 0, 0)
	};
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float *)&pconst[0], _countof(pconst) );
	pconst[0] = D3DXVECTOR4( settings->centerX, settings->centerY, 0, 0 );
	r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, (float *)&pconst[0], 1 );

	if (mSettingsArr.Count() > 0)
		mSettingsArr.Erase(0);
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ExplosionBlur::FinishImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------

void
PFX_ExplosionBlur::PushSettings( const Settings& settings )
{
	mSettingsArr.PushBack(settings);
}