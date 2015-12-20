#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"
#include "PFX_FilmGrain.h"

//------------------------------------------------------------------------

PFX_FilmGrain::PFX_FilmGrain()
: Parent( this )
, mNoiseTexture( NULL )
, mLastDSP0_X( 0 )
, mLastDSP0_Y( 0 )
, mLastDSP1_X( 0 )
, mLastDSP1_Y( 0 )
, mLastNoiseDSP( 0 )
, mLastTick( clock() )
{
	mSettings.GrainScale	= 1000.0f;
	mSettings.Strength		= 0.33f;
	mSettings.FPS			= 24.0f;
}

//------------------------------------------------------------------------

PFX_FilmGrain::~PFX_FilmGrain()
{

}

//------------------------------------------------------------------------

const
PFX_FilmGrain::Settings&
PFX_FilmGrain::GetSettings() const
{
	return mSettings;
}

//------------------------------------------------------------------------

void
PFX_FilmGrain::SetSettings( const Settings& settings )
{
	mSettings = settings;

	if( mSettings.FPS <= 0.f )
	{
		mSettings.FPS = 24.f;
	}
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_FilmGrain::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_FILM_GRAIN" );
	mNoiseTexture		= r3dRenderer->LoadTexture( "Data/Shaders/Texture/uninoise_64x64.dds" );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_FilmGrain::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_FilmGrain::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* /*src*/ ) /*OVERRIDE*/
{
	r3dSetFiltering( R3D_POINT, PostFXChief::FREE_TEX_STAGE_START );

	r3dRenderer->pd3ddev->SetSamplerState( PostFXChief::FREE_TEX_STAGE_START, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP );
	r3dRenderer->pd3ddev->SetSamplerState( PostFXChief::FREE_TEX_STAGE_START, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP );

	r3dRenderer->SetTex( mNoiseTexture, PostFXChief::FREE_TEX_STAGE_START + 0 );

	float delta = float( clock() - mLastTick ) / CLOCKS_PER_SEC;

	if( delta > 1./mSettings.FPS )
	{
		mLastDSP0_X		= float( random( mNoiseTexture->GetWidth() ) ) / mNoiseTexture->GetWidth();
		mLastDSP0_Y		= float( random( mNoiseTexture->GetHeight() ) ) / mNoiseTexture->GetHeight();
		mLastDSP1_X		= float( random( mNoiseTexture->GetWidth() ) ) / mNoiseTexture->GetWidth();
		mLastDSP1_Y		= float( random( mNoiseTexture->GetHeight() ) ) / mNoiseTexture->GetHeight();

		mLastNoiseDSP	= random( 256 ) / 255.f;

		mLastTick = clock();
	}	

	D3DXVECTOR4 vConsts[3];

	const float DISGUISE_SCALE = 1.f / mNoiseTexture->GetWidth();

	float aspect = dest->Height / dest->Width;
	float strength = mSettings.Strength * 0.25f;

	// float4 TexTransform 		: register (c0);
	vConsts[0] = D3DXVECTOR4( mSettings.GrainScale * DISGUISE_SCALE, mSettings.GrainScale * DISGUISE_SCALE * aspect, mLastDSP0_X, mLastDSP0_Y );
	// float4 TexTransform 		: register (c1);
	vConsts[1] = D3DXVECTOR4( mSettings.GrainScale, mSettings.GrainScale * aspect, mLastDSP1_X, mLastDSP1_Y );
	// float4 GrainTransform 	: register (c2);
	vConsts[2] = D3DXVECTOR4( strength, -strength * 0.5f, mLastNoiseDSP, 0 );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF(  0, (float *)vConsts,  3 );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_FilmGrain::FinishImpl()	/*OVERRIDE*/
{
	g_pPostFXChief->SetDefaultTexAddressMode( PostFXChief::FREE_TEX_STAGE_START );
	g_pPostFXChief->SetDefaultTexFiltering< PostFXChief::FREE_TEX_STAGE_START >();
}

