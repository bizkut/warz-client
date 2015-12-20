#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "PFX_GammaCorrect.h"

PFX_GammaCorrect::PFX_GammaCorrect()
: Parent( this )
, mPow ( 1.0f )
{

}

PFX_GammaCorrect::~PFX_GammaCorrect()
{

}


void PFX_GammaCorrect::SetPower ( float Pow )
{
	mPow = Pow;
}
void PFX_GammaCorrect::InitImpl()
{
	mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_GAMMA_CORRECT" );
}

void PFX_GammaCorrect::CloseImpl()
{
}

void PFX_GammaCorrect::PrepareImpl( r3dScreenBuffer* dest, r3dScreenBuffer* src )
{
	r3d_assert( mPow > 0.f );

	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF ( 0, (float*)&D3DXVECTOR4 ( 1.f / mPow, 0.0f, 0.0f, 0.0f ), 1 ) );
}

void PFX_GammaCorrect::FinishImpl()
{

}