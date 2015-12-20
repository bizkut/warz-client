#include "r3dPCH.h"
#include "r3d.h"

#include "CommonPostFX.h"

#include "PostFXChief.h"
#include "PFX_ObjectMotionBlur.h"

//------------------------------------------------------------------------

PFX_ObjectMotionBlur::PFX_ObjectMotionBlur()
: Parent( this )
{
	mSettings.BlurStrength = 1.0f;
}

//------------------------------------------------------------------------

PFX_ObjectMotionBlur::~PFX_ObjectMotionBlur()
{

}

//------------------------------------------------------------------------

const
PFX_ObjectMotionBlur::Settings&
PFX_ObjectMotionBlur::GetSettings() const
{
	return mSettings;
}

//------------------------------------------------------------------------

void
PFX_ObjectMotionBlur::SetSettings( const Settings& settings )
{
	mSettings = settings;
}

//------------------------------------------------------------------------
/*virtual*/

void PFX_ObjectMotionBlur::InitImpl()	/*OVERRIDE*/
{	
	mData.PixelShaderID		= r3dRenderer->GetPixelShaderIdx( "PS_OBJECT_MOTION_BLUR" );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ObjectMotionBlur::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_ObjectMotionBlur::PrepareImpl( r3dScreenBuffer* /*dest*/, r3dScreenBuffer* src ) /*OVERRIDE*/
{
	D3DPERF_BeginEvent( 0x1, L"Object Motion Blur" );

	D3DXVECTOR4 vSettings = D3DXVECTOR4( mSettings.BlurStrength, 0, 0, 0 );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF ( 0, (float*)&vSettings, 1 ); 

	r3dRenderer->SetTex( src->Tex, PostFXChief::FREE_TEX_STAGE_START );
}
//------------------------------------------------------------------------
/*virtual*/

void
PFX_ObjectMotionBlur::FinishImpl() /*OVERRIDE*/
{
	D3DPERF_EndEvent();
}

