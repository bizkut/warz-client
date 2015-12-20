#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "PFX_StereoReproject.h"

//------------------------------------------------------------------------

PFX_StereoReproject::PFX_StereoReproject()
: Parent( this )
{

}

//------------------------------------------------------------------------

PFX_StereoReproject::~PFX_StereoReproject()
{

}

//------------------------------------------------------------------------

void
PFX_StereoReproject::PushSettings( const Settings& sts )
{
	mSettingsVec.PushBack( sts );
}

//------------------------------------------------------------------------

void PFX_StereoReproject::InitImpl()
{
	mLeftPSID	= r3dRenderer->GetShaderIdx( "PS_STEREOREPROJECT_LEFT" );
	mRightPSID	= r3dRenderer->GetShaderIdx( "PS_STEREOREPROJECT_RIGHT" );

	mData.PixelShaderID = mLeftPSID;
}

//------------------------------------------------------------------------

void PFX_StereoReproject::CloseImpl()
{
}

//------------------------------------------------------------------------


void PFX_StereoReproject::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* src )
{
	Settings sts = mSettingsVec[ 0 ];

	mSettingsVec.Erase( 0 );

	const D3DXMATRIX& gameP = r3dRenderer->ProjMatrix ;

	// inverse proj (assumes perspective proj)
	D3DXMATRIX invP;

	D3DXMATRIX altP = gameP ;

	altP._33 = 1.f ;
	altP._43 = 0.f ;
	altP._34 = 0.f ;
	altP._44 = 1.f ;

	D3DXMatrixInverse( &invP, NULL, &altP );

	D3DXMATRIX invView ;

	r3dOrthoInverse( invView, r3dRenderer->ViewMatrix );

	D3DXMATRIX invViewP = invP * invView ;

	D3DXMATRIX viewP = sts.ViewMtx * sts.ProjMtx ;

	// float4x4 mInvViewProjM : register( c0 );
	// float4x4 mViewProj : register( c4 );
	// float4 vMaxOffset_PixelStep_Sign : register( c8 );
	// float4 vReproParms : register( c9 );

	D3DXVECTOR4 vConsts[ 10 ] ;

	D3DXMatrixTranspose( (D3DXMATRIX*)(void*)vConsts, &invViewP );
	D3DXMatrixTranspose( (D3DXMATRIX*)(void*)vConsts + 1, &viewP );

	D3DXVECTOR4 vMaxOffset_PixelStep_Sign ;

	vMaxOffset_PixelStep_Sign.x = -sts.ProjMtx._31 ;
	vMaxOffset_PixelStep_Sign.y = 2.f / dest->Width ;
	vMaxOffset_PixelStep_Sign.z = 1.f / dest->Width ;
	vMaxOffset_PixelStep_Sign.w = vMaxOffset_PixelStep_Sign.x > 0.f ? +1.f : -1.f ;

	vMaxOffset_PixelStep_Sign.y *= vMaxOffset_PixelStep_Sign.w ;
	vMaxOffset_PixelStep_Sign.z *= vMaxOffset_PixelStep_Sign.w ;

	vConsts[ 8 ] = vMaxOffset_PixelStep_Sign ;

	vConsts[ 9 ] = D3DXVECTOR4( sts.ViewDelta * r3dRenderer->ProjMatrix._11, -sts.ProjMtx._31, 0.f, 0.f );

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float*)vConsts, R3D_ARRAYSIZE( vConsts ) ) );

	mData.PixelShaderID = sts.IsRight ? mRightPSID : mLeftPSID ;
}

//------------------------------------------------------------------------

void PFX_StereoReproject::FinishImpl()
{
}

//------------------------------------------------------------------------

void CalcStereoViewProj( float strength, D3DXMATRIX* oLeftView, D3DXMATRIX* oLeftProj, D3DXMATRIX* oRightView, D3DXMATRIX* oRightProj, float* oLeftCamOffset, float* oRightCamOffset )
{
	extern r3dCamera gCam ;
	const r3dCamera& Cam = gCam;

	float	fFOV		=	Cam.FOV ,
			fAspect		=	Cam.Aspect ,
			fNearPlane	=	Cam.NearClip ,
			fFarPlane	=	Cam.FarClip ;

	float height = fNearPlane * tanf( R3D_DEG2RAD( fFOV * 0.5f ) ) ;
	float width = height * fAspect;

	D3DXMATRIX persp ;

	float sep_on = strength ;

	float sep = r_3d_stereo_separation->GetFloat() * width * sep_on ;
	float conv = r_3d_stereo_convergance->GetFloat() * sep_on ;

	float psep = sep * conv ;

	D3DXMATRIX offsetLeft, offsetRight, view;

	D3DXMatrixTranslation( &offsetLeft, +sep, 0.f, 0.f );
	D3DXMatrixTranslation( &offsetRight, -sep, 0.f, 0.f );

	r3dRenderer->BuildMatrixPerspectiveOffCenterLH( &persp, -width + psep, width + psep, -height, height, fNearPlane, fFarPlane );

	*oLeftView = r3dRenderer->ViewMatrix * offsetLeft ;
	*oLeftProj = persp ;

	*oLeftCamOffset = offsetLeft._41 ;

	r3dRenderer->BuildMatrixPerspectiveOffCenterLH( &persp, -width - psep, width - psep, -height, height, fNearPlane, fFarPlane );

	*oRightView = r3dRenderer->ViewMatrix * offsetRight ;
	*oRightProj = persp ;

	*oRightCamOffset = offsetRight._41 ;
}
