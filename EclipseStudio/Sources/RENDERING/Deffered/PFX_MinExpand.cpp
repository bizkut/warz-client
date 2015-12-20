#include "r3dPCH.h"
#include "r3d.h"

#include "PFX_MinExpand.h"
#include "PostFXChief.h"

//------------------------------------------------------------------------
PFX_MinExpand::PFX_MinExpand()
: Parent( this )
{

}

//------------------------------------------------------------------------
PFX_MinExpand::~PFX_MinExpand()
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_MinExpand::InitImpl() /*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_MIN_EXPAND" );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_MinExpand::CloseImpl() /*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_MinExpand::PrepareImpl( r3dScreenBuffer* /*dest*/, r3dScreenBuffer* src )	/*OVERRIDE*/
{
	D3DXVECTOR4 vConst[ 2 ] = 
	{
		D3DXVECTOR4(	0.5f / src->Width ,		0.f ,
						0.0f ,					0.5f / src->Height )
	};

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float*)&vConst, 1 ) );

	r3dSetFiltering( R3D_POINT, 0 );

	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED ) );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_MinExpand::FinishImpl() /*OVERRIDE*/
{
	r3dSetFiltering( g_pPostFXChief->GetZeroTexStageFilter() );
	D3D_V( r3dRenderer->pd3ddev->SetRenderState( D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA ) );
}

