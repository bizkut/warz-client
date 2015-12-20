#include "r3dPCH.h"
#include "r3d.h"

#include "CommonPostFX.h"

#include "PostFXChief.h"
#include "PFX_StarShapeBlur.h"

#include "TrueNature\Sun.h"

PFX_StarShapeBlur::PFX_StarShapeBlur()
: Parent( this )
{
}

//------------------------------------------------------------------------

PFX_StarShapeBlur::~PFX_StarShapeBlur()
{

}

//------------------------------------------------------------------------

void
PFX_StarShapeBlur::SetDefaultSettings( const Settings& settings )
{
	mDefaultSettings = settings;
}

//------------------------------------------------------------------------

const
PFX_StarShapeBlur::Settings&
PFX_StarShapeBlur::GetDefaultSettings() const
{
	return mDefaultSettings;
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_StarShapeBlur::InitImpl()	/*OVERRIDE*/
{	
	mData.PixelShaderID		= r3dRenderer->GetPixelShaderIdx( "POST_STARSHAPEBLUR" );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_StarShapeBlur::CloseImpl() /*OVERRIDE*/
{
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_StarShapeBlur::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* /*src*/ ) /*OVERRIDE*/
{
	const Settings *settings = &mDefaultSettings;
	if (mSettingsArr.Count() > 0)
		settings = &mSettingsArr[0];

	extern r3dSun* Sun;
	extern r3dCamera gCam;

	r3dPoint3D SunPos = gCam;
	SunPos -= Sun->SunDir * 300000.0f;

	r3dPoint3D scr;
	r3dProjectToScreen(SunPos, &scr);

	D3DXVECTOR4 pconst[1] =
	{
		D3DXVECTOR4(scr.x / dest->Width, scr.y / dest->Height, settings->Density, 0)
	};
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float *)&pconst[0], 1 );

	if (mSettingsArr.Count() > 0)
		mSettingsArr.Erase(0);
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_StarShapeBlur::FinishImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------

void
PFX_StarShapeBlur::PushSettings( const Settings& settings )
{
	mSettingsArr.PushBack(settings);
}