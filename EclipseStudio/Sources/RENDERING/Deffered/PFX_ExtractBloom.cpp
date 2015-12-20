#include "r3dPCH.h"
#include "r3d.h"

#include "PFX_ExtractBloom.h"
#include "PostFXChief.h"

//------------------------------------------------------------------------

PFX_ExtractBloom::Settings::Settings()
{
	Power			= 2.0f;
	Threshold		= 0.8f;
	MultiplyColor	= r3dColor(255, 255, 255);
	GlowTint		= r3dColor(255, 255, 255);
	GlowAmplify	= 1.f ;
	GlowThreshold = 0.5f ;
}

//------------------------------------------------------------------------

PFX_ExtractBloom::PFX_ExtractBloom()
: Parent( this )
{
	mDefaultSettings.Power			= 2.0f;
	mDefaultSettings.Threshold		= 0.8f;
	mDefaultSettings.MultiplyColor	= r3dColor(255, 255, 255);
	mDefaultSettings.GlowTint		= r3dColor(255, 255, 255);
	mDefaultSettings.GlowAmplify	= 2.f ;
	mDefaultSettings.GlowThreshold	= 0.5f ;
}

//------------------------------------------------------------------------

PFX_ExtractBloom::~PFX_ExtractBloom()
{

}

//------------------------------------------------------------------------

const
PFX_ExtractBloom::Settings&
PFX_ExtractBloom::GetDefaultSettings() const
{
	return mDefaultSettings;
}

//------------------------------------------------------------------------

void
PFX_ExtractBloom::SetDefaultSettings( const Settings& settings )
{
	mDefaultSettings = settings;
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ExtractBloom::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_CREATEBLOOM" );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ExtractBloom::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ExtractBloom::PrepareImpl( r3dScreenBuffer* /*dest*/, r3dScreenBuffer* /*src*/ ) /*OVERRIDE*/
{
	const Settings *settings = &mDefaultSettings;
	if (mSettingsArr.Count() > 0)
		settings = &mSettingsArr[0];

	float threshold = settings->Threshold ;

	float glowThreshold = settings->GlowThreshold ;

	float glowMul = settings->GlowAmplify / ( 1.0f - settings->GlowThreshold );

	D3DXVECTOR4 bloom_params[ 4 ] = { 
		D3DXVECTOR4(    settings->Power, threshold, 0, 0 ),
		D3DXVECTOR4(	settings->MultiplyColor.R / 255.f * settings->Power, 
						settings->MultiplyColor.G / 255.f * settings->Power, 
						settings->MultiplyColor.B / 255.f * settings->Power, 1.f ),
	
		D3DXVECTOR4(	settings->GlowTint.R / 255.f * settings->GlowAmplify, 
						settings->GlowTint.G / 255.f * settings->GlowAmplify, 
						settings->GlowTint.B / 255.f * settings->GlowAmplify, 1.f ),
		D3DXVECTOR4(	glowMul, -settings->GlowThreshold * glowMul, 0, 0.f )

	};

	r3dRenderer->pd3ddev->SetPixelShaderConstantF(0, (float*)&bloom_params, sizeof bloom_params / sizeof bloom_params[ 0 ] );

	r3dRenderer->SetTex(g_pPostFXChief->GetBuffer(PostFXChief::RTT_DIFFUSE_32BIT)->Tex, 4);

	if (mSettingsArr.Count() > 0)
		mSettingsArr.Erase(0);
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ExtractBloom::FinishImpl()	/*OVERRIDE*/
{
	
}

//------------------------------------------------------------------------

void
PFX_ExtractBloom::PushSettings(const Settings &settings)
{
	mSettingsArr.PushBack(settings);
}