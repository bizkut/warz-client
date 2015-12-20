#include "r3dPCH.h"
#include "r3d.h"

#include "PFX_ExtractGlow.h"

//------------------------------------------------------------------------

PFX_ExtractGlow::PFX_ExtractGlow()
: Parent( this )
{
	mSettings.TintColor		= r3dColor( 255, 255, 255 );
	mSettings.Multiplier	= 0.99f;
}

//------------------------------------------------------------------------

PFX_ExtractGlow::~PFX_ExtractGlow()
{

}

//------------------------------------------------------------------------

const
PFX_ExtractGlow::Settings&
PFX_ExtractGlow::GetSettings() const
{
	return mSettings;
}

//------------------------------------------------------------------------

void
PFX_ExtractGlow::SetSettings( const Settings& settings )
{
	mSettings = settings;
}


//------------------------------------------------------------------------
/*virtual*/

void
PFX_ExtractGlow::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_CREATEGLOW" );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ExtractGlow::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ExtractGlow::PrepareImpl( r3dScreenBuffer* /*dest*/, r3dScreenBuffer* /*src*/ ) /*OVERRIDE*/
{
	D3DXVECTOR4 Const( mSettings.TintColor.R, mSettings.TintColor.G, mSettings.TintColor.B, mSettings.TintColor.A );
	Const *= mSettings.Multiplier / 255.0f,

	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float*)Const, 1 );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ExtractGlow::FinishImpl()	/*OVERRIDE*/
{

}

