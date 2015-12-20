#include "r3dPCH.h"
#include "r3d.h"

#include "PFX_DOFExtractNear.h"

PFX_DOFExtractNear::Settings::Settings()
: NearEnd( 1.f )
, NearStart( 0.f )
{

}

//------------------------------------------------------------------------

PFX_DOFExtractNear::PFX_DOFExtractNear()
: Parent( this )
{
	
}

//------------------------------------------------------------------------

PFX_DOFExtractNear::~PFX_DOFExtractNear()
{

}

//------------------------------------------------------------------------

const
PFX_DOFExtractNear::Settings&
PFX_DOFExtractNear::GetSettings() const
{
	return mSettings;
}

//------------------------------------------------------------------------

void
PFX_DOFExtractNear::SetSettings( const Settings& settings )
{
	mSettings = settings;
}


//------------------------------------------------------------------------
/*virtual*/

void
PFX_DOFExtractNear::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_DOF_EXTRACT_NEAR" );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_DOFExtractNear::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_DOFExtractNear::PrepareImpl( r3dScreenBuffer* /*dest*/, r3dScreenBuffer* /*src*/ ) /*OVERRIDE*/
{
	D3DXVECTOR4 Const( 
			-1.f / ( mSettings.NearEnd - mSettings.NearStart ),
			1.f + mSettings.NearStart / ( mSettings.NearEnd - mSettings.NearStart ),
			0.f, 0.f 
			);
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float*)Const, 1 );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_DOFExtractNear::FinishImpl()	/*OVERRIDE*/
{

}

