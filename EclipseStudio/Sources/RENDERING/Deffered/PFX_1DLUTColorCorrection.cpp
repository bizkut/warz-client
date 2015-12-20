#include "r3dPCH.h"
#include "r3d.h"

#include "PostFXChief.h"

#include "PFX_1DLUTColorCorrection.h"

PFX_1DLUTColorCorrection::PFX_1DLUTColorCorrection( bool useHSV )
: Parent( this )
, mLUTTexture( NULL )
, mUseHSV( useHSV )
{

}

//------------------------------------------------------------------------

PFX_1DLUTColorCorrection::~PFX_1DLUTColorCorrection()
{

}

//------------------------------------------------------------------------
/*virtual*/

void PFX_1DLUTColorCorrection::InitImpl()	/*OVERRIDE*/
{
	extern r3dTexture* CC_LUT1D_RGB_Tex;
	extern r3dTexture* CC_LUT1D_HSV_Tex;

	if( mUseHSV )
	{
		mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_CC_LUT1D_HSV"		);
		mLUTTexture			= CC_LUT1D_HSV_Tex;
	}
	else
	{
		mData.PixelShaderID = r3dRenderer->GetPixelShaderIdx( "PS_CC_LUT1D_RGB"		);
		mLUTTexture			= CC_LUT1D_RGB_Tex;
	}

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_1DLUTColorCorrection::CloseImpl()		/*OVERRIDE*/
{

}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_1DLUTColorCorrection::PrepareImpl( r3dScreenBuffer* /*dest*/, r3dScreenBuffer* /*src*/ )	/*OVERRIDE*/
{
	r3dRenderer->SetTex( mLUTTexture, PostFXChief::FREE_TEX_STAGE_START );
}

//------------------------------------------------------------------------
/*virtual*/

void
PFX_1DLUTColorCorrection::FinishImpl()	/*OVERRIDE*/
{

}
