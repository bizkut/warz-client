#include "r3dPCH.h"
#include "r3d.h"

#include "CommonPostFX.h"

#include "PostFXChief.h"
#include "PFX_CameraMotionBlur.h"

void CalculateMotionMatrix( D3DXMATRIX& output, D3DXMATRIX& lastViewProj );

//------------------------------------------------------------------------

PFX_CameraMotionBlur::PFX_CameraMotionBlur()
: Parent( this )
{
	mSettings.MotionScale		= 2.5f;
	mSettings.MotionThreshold	= 0.005f;

	D3DXMatrixIdentity( &mMotionMatrix );
	D3DXMatrixIdentity( &mLastViewProj );
}

//------------------------------------------------------------------------

PFX_CameraMotionBlur::~PFX_CameraMotionBlur()
{

}

//------------------------------------------------------------------------

const
PFX_CameraMotionBlur::Settings&
PFX_CameraMotionBlur::GetSettings() const
{
	return mSettings;
}

//------------------------------------------------------------------------

void
PFX_CameraMotionBlur::SetSettings( const Settings& settings )
{
	mSettings = settings;
}

//------------------------------------------------------------------------

void
PFX_CameraMotionBlur::Update()
{
	CalculateMotionMatrix( mMotionMatrix, mLastViewProj );
}

//------------------------------------------------------------------------

bool
PFX_CameraMotionBlur::IsCameraInMotion() const
{
	float THRE = mSettings.MotionThreshold;
	return R3D_ABS(mMotionMatrix._41) > THRE || R3D_ABS(mMotionMatrix._42) > THRE || R3D_ABS(mMotionMatrix._43) > THRE;
}


//------------------------------------------------------------------------
/*virtual*/

void
PFX_CameraMotionBlur::InitImpl()	/*OVERRIDE*/
{	
	mData.PixelShaderID		= r3dRenderer->GetPixelShaderIdx( "PS_DS_CAMERA_MOTION" );
	mData.VertexShaderID	= r3dRenderer->GetVertexShaderIdx( "VS_DS_CAMERA_MOTION" );

	mData.VSType			= PostFXData::VST_CUSTOM;
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_CameraMotionBlur::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_CameraMotionBlur::PrepareImpl( r3dScreenBuffer* /*dest*/, r3dScreenBuffer* src ) /*OVERRIDE*/
{
	D3DPERF_BeginEvent( 0x1, L"camera motion blur" );

	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float*)&mMotionMatrix._11, 1 );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 1, (float*)&mMotionMatrix._21, 1 );
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 2, (float*)&mMotionMatrix._41, 1 );
	D3DXVECTOR4 vCamScale = D3DXVECTOR4 ( mSettings.MotionScale,1.0f / r3dRenderer->FarClip,0,0);
	r3dRenderer->pd3ddev->SetPixelShaderConstantF( 3, (float*)&vCamScale, 1 );

	r3dRenderer->SetTex( src->Tex, PostFXChief::FREE_TEX_STAGE_START );
}
//------------------------------------------------------------------------
/*virtual*/

void
PFX_CameraMotionBlur::FinishImpl() /*OVERRIDE*/
{
	D3DPERF_EndEvent();
}

