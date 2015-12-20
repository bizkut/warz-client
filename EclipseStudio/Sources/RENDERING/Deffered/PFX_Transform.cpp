#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "PFX_Transform.h"

PFX_Transform::PFX_Transform()
: Parent( this )
{
	mTCoefs[ 0 ] = D3DXVECTOR4( 1, 1, 1, 1 );
	mTCoefs[ 1 ] = D3DXVECTOR4( 0, 0, 0, 0 );
}

//------------------------------------------------------------------------

PFX_Transform::~PFX_Transform()
{

}

//------------------------------------------------------------------------

void
PFX_Transform::SetSettings( D3DXVECTOR4 a, D3DXVECTOR4 b )
{
	mTCoefs[ 0 ] = a ;
	mTCoefs[ 1 ] = b ;
}

//------------------------------------------------------------------------
/*virtual*/

void PFX_Transform::InitImpl()	/*OVERRIDE*/
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_TRANSFORM" );

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_Transform::CloseImpl()		/*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_Transform::PrepareImpl( r3dScreenBuffer* /*dest*/, r3dScreenBuffer* /*src*/ )	/*OVERRIDE*/
{
	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, (float*)mTCoefs, 2 ) );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_Transform::FinishImpl()	/*OVERRIDE*/
{

}
