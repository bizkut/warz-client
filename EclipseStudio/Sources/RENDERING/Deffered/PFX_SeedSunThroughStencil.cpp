#include "r3dPCH.h"

#include "r3d.h"

#include "PFX_SeedSunThroughStencil.h"

#include "PostFXChief.h"

#include "TrueNature/Sun.h"

#include "../DX9/RenderDX9.h"

#include "RenderDeffered.h"

//------------------------------------------------------------------------

PFX_SeedSunThroughStencil::Settings::Settings()
: Weight( 0.25f )
, Radius( 8.f )
{

}

//------------------------------------------------------------------------
PFX_SeedSunThroughStencil::PFX_SeedSunThroughStencil( )
: Parent( this )
{

}

//------------------------------------------------------------------------
PFX_SeedSunThroughStencil::~PFX_SeedSunThroughStencil()
{

}

//------------------------------------------------------------------------

const PFX_SeedSunThroughStencil::Settings&
PFX_SeedSunThroughStencil::GetSettings() const
{
	return mSettings;
}

//------------------------------------------------------------------------

void
PFX_SeedSunThroughStencil::SetSettings( const Settings& sts )
{
	mSettings = sts;
}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_SeedSunThroughStencil::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_SUNSPOT" );
	mData.VertexShaderID = r3dRenderer->GetVertexShaderIdx( "VS_SUNSPOT" );
	mData.VSType = PostFXData::VST_CUSTOM;
}

//------------------------------------------------------------------------
/*virtual*/
void
PFX_SeedSunThroughStencil::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_SeedSunThroughStencil::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* /*src*/ )	/*OVERRIDE*/
{
	// cause we use main buffer stencil
	r3d_assert(		dest->Width		<= ScreenBuffer->Width && 
					dest->Height	<= ScreenBuffer->Height );

	// contrary to what some may think, this sets main zbuffer
	r3dRenderer->SetDSS( dest->ZBuf.Get() ) ;

	SetupLightMaskStencilStates( SCM_UNLITAREA );

	extern r3dSun* Sun;
	extern r3dCamera gCam;

	D3DXVECTOR3 sunDir(Sun->SunDir.x, Sun->SunDir.y, Sun->SunDir.z);
	D3DXMATRIXA16 viewMat(r3dRenderer->ViewMatrix);
	viewMat._41 = viewMat._42 = viewMat._43 = 0;
	D3DXVec3TransformCoord(&sunDir, &sunDir, &viewMat);

	float conts[ 8 ] = 
	{
		mSettings.Weight, (0.5f - mSettings.Radius) * 128, 0, 0,
		sunDir.x, sunDir.y, sunDir.z, 0
	};

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, conts, 2 ) );
	
	conts[0] = dest->Height / dest->Width;

	D3D_V( r3dRenderer->pd3ddev->SetVertexShaderConstantF( 0, conts, 1 ) );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_SeedSunThroughStencil::FinishImpl() /*OVERRIDE*/
{
	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_STENCILENABLE, FALSE ) );
}

