#include "r3dPCH.h"
#include "r3d.h"

#include "CommonPostFX.h"

#include "PostFXChief.h"
#include "PFX_RadialBlur.h"

PFX_RadialBlur::PFX_RadialBlur()
: Parent( this )
, mModulationTex( NULL )
{
}

//------------------------------------------------------------------------

PFX_RadialBlur::~PFX_RadialBlur()
{

}

//------------------------------------------------------------------------

void
PFX_RadialBlur::SetDefaultSettings( const Settings& settings )
{
	mDefaultSettings = settings;
}

//------------------------------------------------------------------------

const
PFX_RadialBlur::Settings&
PFX_RadialBlur::GetDefaultSettings() const
{
	return mDefaultSettings;
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_RadialBlur::InitImpl()	/*OVERRIDE*/
{	
	mData.PixelShaderID		= r3dRenderer->GetPixelShaderIdx( "PS_RADIALBLUR" );
	mData.VertexShaderID	= r3dRenderer->GetVertexShaderIdx( "VS_RADIALBLUR" );

	mData.VSType			= PostFXData::VST_CUSTOM;

	mModulationTex			= r3dRenderer->LoadTexture( "Data\\Shaders\\Texture\\Modulation.dds" );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_RadialBlur::CloseImpl() /*OVERRIDE*/
{
	r3dRenderer->DeleteTexture( mModulationTex );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_RadialBlur::PrepareImpl( r3dScreenBuffer* /*dest*/, r3dScreenBuffer* /*src*/ ) /*OVERRIDE*/
{
	r3dRenderer->SetTex( mModulationTex, PostFXChief::FREE_TEX_STAGE_START );

	const Settings *settings = &mDefaultSettings;
	if (mSettingsArr.Count() > 0)
		settings = &mSettingsArr[0];

	D3DXVECTOR4 pconst[1] =
	{
		D3DXVECTOR4(settings->BlurStart, settings->BlurStrength, 0, 0)
	};
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float *)&pconst[0], 1 );

	if (mSettingsArr.Count() > 0)
		mSettingsArr.Erase(0);
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_RadialBlur::FinishImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------

/*static*/ const float	PFX_RadialBlur::Settings::MIN_BLURSTART		= 0.f,
						PFX_RadialBlur::Settings::MAX_BLURSTART		= 1.f;
/*static*/ const float	PFX_RadialBlur::Settings::MIN_BLURSTRENGTH	= 0.0f,
						PFX_RadialBlur::Settings::MAX_BLURSTRENGTH	= 0.05f;

void
PFX_RadialBlur::Settings::Restrict()
{
	BlurStart		= R3D_MIN( R3D_MAX( BlurStart,		MIN_BLURSTART ),	MAX_BLURSTART		);
	BlurStrength	= R3D_MIN( R3D_MAX( BlurStrength,	MIN_BLURSTRENGTH ),	MAX_BLURSTRENGTH	);
}

//------------------------------------------------------------------------

void
PFX_RadialBlur::PushSettings( const Settings& settings )
{
	mSettingsArr.PushBack(settings);
}