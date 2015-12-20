#include "r3dPCH.h"
#include "r3d.h"

#include "TrueNature\Sun.h"

#include "PFX_GodRays.h"

//------------------------------------------------------------------------

PFX_GodRays::PFX_GodRays()
: Parent( this )
, prevAddressU(0)
, prevAddressV(0)
{
	mSettings.Density	= 1.0f;
	mSettings.Decay 	= 0.5f;
	mSettings.Exposure	= 2.0f;
}

//------------------------------------------------------------------------

PFX_GodRays::~PFX_GodRays()
{

}

//------------------------------------------------------------------------

const
PFX_GodRays::Settings&
PFX_GodRays::GetSettings() const
{
	return mSettings;
}

//------------------------------------------------------------------------

void
PFX_GodRays::SetSettings( const Settings& settings )
{
	mSettings = settings;
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_GodRays::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "POST_GODRAYS" );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_GodRays::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_GodRays::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* /*src*/ ) /*OVERRIDE*/
{
	extern r3dSun* Sun;

	extern r3dCamera gCam;

	r3dPoint3D SunPos = gCam;
	SunPos -= Sun->SunDir * 300000.0f;

	r3dPoint3D scr;
	r3dProjectToScreen(SunPos, &scr);

	// NOTE : Sync with Post_GodRays.hls
	const int NUM_SAMPLES = 40;

	float dotVal = -gCam.vPointTo.Dot( Sun->SunDir );
	float fadeOut = R3D_MAX( R3D_MIN( ( dotVal - 0.5f ) * 5.f, 1.f ), 0.f );

	D3DXVECTOR4 vConsts[2] = 
	{
		D3DXVECTOR4( mSettings.Density, 1.f, mSettings.Decay, mSettings.Exposure ),
		D3DXVECTOR4( scr.x/dest->Width, scr.y/dest->Height, 1.f, 1.f )
	};	

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float*)vConsts, 2 ) );

	D3D_V( r3dRenderer->pd3ddev->GetSamplerState(0, D3DSAMP_ADDRESSU, &prevAddressU));
	D3D_V( r3dRenderer->pd3ddev->GetSamplerState(0, D3DSAMP_ADDRESSV, &prevAddressV));

	D3D_V( r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER));
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER));
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0));

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_GodRays::FinishImpl()	/*OVERRIDE*/
{
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSU, prevAddressU));
	D3D_V( r3dRenderer->pd3ddev->SetSamplerState(0, D3DSAMP_ADDRESSV, prevAddressV));
}

